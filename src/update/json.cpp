#include "update/json.hpp"

#include <cstddef>
#include <utility>

namespace loupe::update {

namespace {

// Deeper than any answer GitHub gives; bounds the recursion on hostile input.
constexpr int MAX_DEPTH = 64;
// UTF-16's surrogates: a high one, then a low one, stand for one code point past the BMP.
constexpr uint32_t HIGH_SURROGATE = 0xd800;
constexpr uint32_t LOW_SURROGATE = 0xdc00;
constexpr uint32_t SURROGATES_END = 0xe000;
constexpr uint32_t PAST_BMP = 0x10000;
constexpr int SURROGATE_BITS = 10;
constexpr int HEX_DIGITS = 4;
constexpr std::string_view SPACE = " \t\n\r";
constexpr std::string_view NUMBER_CHARS = "+-0123456789.eE";
// The one-letter escapes after a backslash, and what each stands for.
constexpr std::string_view ESCAPES = "\"\\/bfnrt";
constexpr std::string_view ESCAPED = "\"\\/\b\f\n\r\t";

// `code` appended to `out` as UTF-8.
void
append_utf8(uint32_t code, std::string* out) {
    constexpr uint32_t ONE_BYTE = 0x80, TWO_BYTES = 0x800;
    constexpr uint32_t CONTINUATION = 0x80, LOW_SIX = 0x3f;
    if (code < ONE_BYTE) {
        *out += static_cast<char>(code);
        return;
    }
    // The lead byte's marker and the continuation bytes after it.
    const int continuations = code < TWO_BYTES ? 1 : code < PAST_BMP ? 2 : 3;
    constexpr uint32_t LEADS[] = {0, 0xc0, 0xe0, 0xf0};
    *out += static_cast<char>(LEADS[continuations] | (code >> (6 * continuations)));
    for (int i = continuations - 1; i >= 0; i--) {
        *out += static_cast<char>(CONTINUATION | ((code >> (6 * i)) & LOW_SIX));
    }
}

// A hex digit's value; none for any other character.
std::optional<uint32_t>
hex_digit(char c) {
    constexpr uint32_t TEN = 10;
    if (c >= '0' && c <= '9') {
        return static_cast<uint32_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return static_cast<uint32_t>(c - 'a') + TEN;
    }
    if (c >= 'A' && c <= 'F') {
        return static_cast<uint32_t>(c - 'A') + TEN;
    }
    return std::nullopt;
}

class Reader {
  public:
    explicit Reader(std::string_view text) : text_(text) {}

    std::optional<Json>
    document() {
        Json value;
        if (!read(&value, 0)) {
            return std::nullopt;
        }
        skip_space();
        return at_ == text_.size() ? std::optional<Json>(std::move(value)) : std::nullopt;
    }

  private:
    void
    skip_space() {
        while (at_ < text_.size() && SPACE.find(text_[at_]) != std::string_view::npos) {
            at_++;
        }
    }

    bool
    take(char c) {
        skip_space();
        if (at_ < text_.size() && text_[at_] == c) {
            at_++;
            return true;
        }
        return false;
    }

    bool
    word(std::string_view w) {
        if (text_.substr(at_, w.size()) != w) {
            return false;
        }
        at_ += w.size();
        return true;
    }

    bool
    read(Json* value, int depth) {
        skip_space();
        if (depth > MAX_DEPTH || at_ >= text_.size()) {
            return false;
        }
        switch (text_[at_]) {
            case '{': return object(value, depth);
            case '[': return array(value, depth);
            case '"': value->kind = Json::Kind::String; return string(&value->text);
            case 't':
                value->kind = Json::Kind::Bool;
                value->boolean = true;
                return word("true");
            case 'f': value->kind = Json::Kind::Bool; return word("false");
            case 'n': return word("null");
            default: value->kind = Json::Kind::Number; return number(&value->text);
        }
    }

    bool
    object(Json* value, int depth) {
        value->kind = Json::Kind::Object;
        at_++;
        if (take('}')) {
            return true;
        }
        do {
            std::string name;
            Json item;
            skip_space();
            if (!string(&name) || !take(':') || !read(&item, depth + 1)) {
                return false;
            }
            value->names.push_back(std::move(name));
            value->items.push_back(std::move(item));
        } while (take(','));
        return take('}');
    }

    bool
    array(Json* value, int depth) {
        value->kind = Json::Kind::Array;
        at_++;
        if (take(']')) {
            return true;
        }
        do {
            Json item;
            if (!read(&item, depth + 1)) {
                return false;
            }
            value->items.push_back(std::move(item));
        } while (take(','));
        return take(']');
    }

    bool
    number(std::string* out) {
        const size_t start = at_;
        while (at_ < text_.size() && NUMBER_CHARS.find(text_[at_]) != std::string_view::npos) {
            at_++;
        }
        *out = text_.substr(start, at_ - start);
        return at_ > start;
    }

    // The four hex digits after \u.
    bool
    hex4(uint32_t* code) {
        if (at_ + HEX_DIGITS > text_.size()) {
            return false;
        }
        *code = 0;
        for (int i = 0; i < HEX_DIGITS; i++) {
            const std::optional<uint32_t> digit = hex_digit(text_[at_++]);
            if (!digit) {
                return false;
            }
            *code = (*code << 4) | *digit;
        }
        return true;
    }

    // \uXXXX, or a surrogate pair's two of them, as UTF-8.
    bool
    escaped_code(std::string* out) {
        uint32_t code = 0;
        if (!hex4(&code) || (code >= LOW_SURROGATE && code < SURROGATES_END)) {
            return false;
        }
        if (code >= HIGH_SURROGATE && code < LOW_SURROGATE) {
            uint32_t low = 0;
            if (!word("\\u") || !hex4(&low) || low < LOW_SURROGATE || low >= SURROGATES_END) {
                return false;
            }
            code = PAST_BMP + ((code - HIGH_SURROGATE) << SURROGATE_BITS) + (low - LOW_SURROGATE);
        }
        append_utf8(code, out);
        return true;
    }

    bool
    string(std::string* out) {
        if (at_ >= text_.size() || text_[at_] != '"') {
            return false;
        }
        at_++;
        while (at_ < text_.size()) {
            const char c = text_[at_++];
            if (c == '"') {
                return true;
            }
            if (static_cast<unsigned char>(c) < ' ') {
                return false;
            }
            if (c != '\\') {
                *out += c;
                continue;
            }
            if (at_ >= text_.size()) {
                return false;
            }
            const char e = text_[at_++];
            const size_t simple = ESCAPES.find(e);
            if (simple != std::string_view::npos) {
                *out += ESCAPED[simple];
            } else if (e != 'u' || !escaped_code(out)) {
                return false;
            }
        }
        return false;
    }

    std::string_view text_;
    size_t at_ = 0;
};

} // namespace

const Json*
Json::get(std::string_view name) const {
    if (kind != Kind::Object) {
        return nullptr;
    }
    for (size_t i = 0; i < names.size(); i++) {
        if (names[i] == name) {
            return &items[i];
        }
    }
    return nullptr;
}

std::string
Json::string() const {
    return kind == Kind::String ? text : std::string();
}

std::optional<Json>
parse_json(std::string_view text) {
    return Reader(text).document();
}

} // namespace loupe::update
