#include "storyboard/check.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <format>
#include <regex>
#include <string_view>
#include <system_error>

#include "paths.hpp"
#include "storyboard/recipe.hpp"

namespace loupe::storyboard {

namespace {

constexpr std::array<std::string_view, 7> OPS = {"eq", "ne", "lt", "le", "gt", "ge", "match"};

// One check's field (without loupe.) and comparison; empty when it has not
// exactly one of each, or the field is not loupe.'s.
struct Parts {
    std::string field, op;
    nlohmann::json value;
};

Parts
parts(const nlohmann::json& check) {
    Parts found;
    if (!check.is_object() || check.size() != 2) {
        return {};
    }
    for (const auto& [key, value] : check.items()) {
        if (std::ranges::find(OPS, key) != OPS.end()) {
            found.op = key;
            found.value = value;
        } else if (key.starts_with(LOUPE_KEY_PREFIX) && key.size() > sizeof LOUPE_KEY_PREFIX - 1) {
            found.field = key.substr(sizeof LOUPE_KEY_PREFIX - 1);
        }
    }
    return found.field.empty() || found.op.empty() ? Parts{} : found;
}

// `value` as text: a string itself, a list its items joined by a space, else JSON.
std::string
text_of(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    }
    if (value.is_array()) {
        std::string joined;
        for (const nlohmann::json& item : value) {
            joined += (joined.empty() ? "" : " ") + text_of(item);
        }
        return joined;
    }
    return value.dump();
}

std::optional<double>
number_of(const nlohmann::json& value) {
    if (value.is_number()) {
        return value.get<double>();
    }
    if (!value.is_string()) {
        return std::nullopt;
    }
    const std::string& text = value.get_ref<const std::string&>();
    double parsed = 0.0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);
    return error == std::errc() && end == text.data() + text.size() ? std::optional(parsed) : std::nullopt;
}

template <typename T>
bool
compare(const std::string& op, const T& seen, const T& wanted) {
    if (op == "eq") {
        return seen == wanted;
    }
    if (op == "ne") {
        return seen != wanted;
    }
    if (op == "lt") {
        return seen < wanted;
    }
    if (op == "le") {
        return seen <= wanted;
    }
    if (op == "gt") {
        return seen > wanted;
    }
    return seen >= wanted;
}

bool
holds(const nlohmann::json& observed, const Parts& check, std::string* why) {
    if (!observed.contains(check.field)) {
        *why = std::format("nothing observed is called loupe.{}", check.field);
        return false;
    }
    const nlohmann::json& seen = observed[check.field];
    const std::string seen_text = text_of(seen);
    const std::string wanted_text = text_of(check.value);
    bool ok = false;
    if (check.op == "match") {
        ok = std::regex_search(seen_text, std::regex(wanted_text));
    } else if (const auto a = number_of(seen), b = number_of(check.value); seen.is_number() && a && b) {
        ok = compare(check.op, *a, *b);
    } else if (check.op == "eq" || check.op == "ne") {
        ok = compare(check.op, seen_text, wanted_text);
    } else {
        *why = std::format("loupe.{} is \"{}\", not a number to compare {} {}", check.field, seen_text, check.op,
                           wanted_text);
        return false;
    }
    if (!ok) {
        *why = std::format("loupe.{} is \"{}\", not {} \"{}\"", check.field, seen_text, check.op, wanted_text);
    }
    return ok;
}

} // namespace

std::optional<std::string>
check_refused(const nlohmann::json& checks) {
    if (!checks.is_array() || checks.empty()) {
        return "a check is a list, such as [{loupe.frames: ~, gt: 30}]";
    }
    for (const nlohmann::json& check : checks) {
        const Parts found = parts(check);
        if (found.field.empty()) {
            return std::format("a check is one loupe.FIELD and one of eq ne lt le gt ge match: {}", check.dump());
        }
        if (found.op == "match") {
            try {
                (void)std::regex(text_of(found.value));
            } catch (const std::regex_error& error) {
                return std::format("match {}: {}", text_of(found.value), error.what());
            }
        }
    }
    return std::nullopt;
}

nlohmann::json
observed_fields(const Observed& observed) {
    return {{"screen", observed.screen},
            {"refusal", observed.refusal},
            {"boards", observed.boards},
            {"busy", observed.busy},
            {"phase", observed.phase},
            {"frames", observed.frames},
            {"frame_age_s", observed.frame_age_s},
            {"saving", observed.saving},
            {"saved_frames", observed.saved_frames},
            {"recording", utf8(observed.recording)},
            {"recording_bytes", observed.recording_bytes}};
}

bool
checks_hold(const Observed& observed, const nlohmann::json& checks, std::string* why) {
    if (const std::optional<std::string> refused = check_refused(checks)) {
        *why = *refused;
        return false;
    }
    const nlohmann::json fields = observed_fields(observed);
    return std::ranges::all_of(checks, [&](const nlohmann::json& check) { return holds(fields, parts(check), why); });
}

std::optional<nlohmann::json>
substituted(const nlohmann::json& value, const std::map<std::string, std::string>& names, std::string* why) {
    if (value.is_string()) {
        std::string text = value.get<std::string>();
        for (size_t at = text.find("${"); at != std::string::npos; at = text.find("${", at)) {
            const size_t end = text.find('}', at);
            const std::string name = end == std::string::npos ? "" : text.substr(at + 2, end - at - 2);
            const auto given = names.find(name);
            if (given == names.end()) {
                *why = std::format("no ${{{}}} given: pass --set {}=VALUE", name, name);
                return std::nullopt;
            }
            text.replace(at, end - at + 1, given->second);
            at += given->second.size();
        }
        return text;
    }
    if (!value.is_structured()) {
        return value;
    }
    nlohmann::json out = value;
    for (auto& [key, item] : out.items()) {
        std::optional<nlohmann::json> replaced = substituted(item, names, why);
        if (!replaced) {
            return std::nullopt;
        }
        item = std::move(*replaced);
    }
    return out;
}

} // namespace loupe::storyboard
