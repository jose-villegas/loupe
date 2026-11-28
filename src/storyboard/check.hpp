// A step's checks: comparisons over what the window shows, each a map of one
// loupe.FIELD (its value unused) and one comparison, such as
// {loupe.frames: ~, gt: "30"}; and a recipe's ${NAME}s, given on the command line.
#pragma once

#include <map>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include "window/observed.hpp"

namespace loupe::storyboard {

// The key a step's checks are under.
constexpr const char CHECK_KEY[] = "check";

// Why `checks`, a list of checks, cannot be checked; none when it can.
std::optional<std::string> check_refused(const nlohmann::json& checks);
// `observed` as a map of field to value, the fields a check names.
nlohmann::json observed_fields(const Observed& observed);
// Whether every check in `checks` holds over `observed`'s fields; when one
// does not, `why` says which and what was seen. A number compares as a
// number; text by eq, ne and match (a regular expression found anywhere in
// it); a list as its items joined by a space.
bool checks_hold(const Observed& observed, const nlohmann::json& checks, std::string* why);
// `value` with each ${NAME} in its strings replaced by `names`' NAME; none,
// with `why` set, for a name not given.
std::optional<nlohmann::json> substituted(const nlohmann::json& value, const std::map<std::string, std::string>& names,
                                          std::string* why);

} // namespace loupe::storyboard
