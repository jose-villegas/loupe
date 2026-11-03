// A JSON reader (RFC 8259) for GitHub's API answers: the whole text into a
// tree, strings unescaped to UTF-8, numbers kept as written.
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace loupe::update {

struct Json {
    enum class Kind : uint8_t { Null, Bool, Number, String, Array, Object };
    Kind kind = Kind::Null;
    bool boolean = false;
    std::string text;               // a String's value, or a Number as written
    std::vector<Json> items;        // an Array's items, or an Object's values
    std::vector<std::string> names; // an Object's names, one per item

    // An Object's value named `name`; null for any other kind or a missing name.
    const Json* get(std::string_view name) const;
    // A String's value; empty for any other kind.
    std::string string() const;
};

// `text` as one JSON value; none when it is not one.
std::optional<Json> parse_json(std::string_view text);

} // namespace loupe::update
