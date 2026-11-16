#include "board/boards.hpp"

#include <sstream>
#include <string_view>

#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "process.hpp"

namespace loupe {

namespace {

// The pool's word for a board no one holds.
constexpr std::string_view FREE_STATE = "unlocked";
// How the JSON line of `autana devices --json` opens.
constexpr std::string_view JSON_OPENS = R"({"boards")";

} // namespace

std::vector<std::string>
autana_command(const std::vector<std::string>& args) {
#if defined(_WIN32)
    std::vector<std::string> command = {utf8(system_program("cmd")), "/d", "/c", "autana"};
#else
    std::vector<std::string> command = {"autana"};
#endif
    command.insert(command.end(), args.begin(), args.end());
    return command;
}

BoardList
parse_boards(const std::string& output) {
    std::istringstream lines(output);
    std::string json;
    for (std::string line; std::getline(lines, line);) {
        if (line.starts_with(JSON_OPENS)) {
            json = line;
        }
    }
    const nlohmann::json parsed = nlohmann::json::parse(json, nullptr, false);
    if (parsed.is_discarded() || !parsed.contains("boards") || !parsed["boards"].is_array()) {
        return {.boards = {}, .why = "autana devices --json gave no boards: " + output};
    }
    BoardList list;
    try {
        for (const nlohmann::json& item : parsed["boards"]) {
            const nlohmann::json& port = item.at("port");
            list.boards.push_back({.id = item.at("board").get<std::string>(),
                                   .port = port.is_null() ? std::string() : port.get<std::string>(),
                                   .free = item.at("state").get<std::string>() == FREE_STATE});
        }
    } catch (const nlohmann::json::exception& error) {
        return {.boards = {},
                .why = std::string("autana devices --json gave a board without its fields: ") + error.what()};
    }
    return list;
}

BoardList
list_boards(const std::filesystem::path& log) {
    if (!run_logged(autana_command({"devices", "--json"}), log)) {
        return {.boards = {}, .why = "autana devices failed: " + read_text(log)};
    }
    return parse_boards(read_text(log));
}

} // namespace loupe
