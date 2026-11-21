#include "board/boards.hpp"

#include <algorithm>
#include <sstream>
#include <string_view>

#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "process.hpp"

namespace loupe {

namespace {

// The pool's words for a board no one holds and for a person's reservation.
constexpr std::string_view FREE_STATE = "unlocked";
constexpr std::string_view PERSON_STATE = "human";
// How the JSON line of `autana devices --json` opens.
constexpr std::string_view JSON_OPENS = R"({"boards")";
// A lock token: a UUID's hex digits, lower case (autana's device_lock.py).
constexpr size_t TOKEN_DIGITS = 32;
// The note `lock hand` renews a reservation with when it had none.
constexpr std::string_view RECORDING_NOTE = "recording";

Holder
holder_of(std::string_view state) {
    if (state == FREE_STATE) {
        return Holder::Nobody;
    }
    return state == PERSON_STATE ? Holder::Person : Holder::Command;
}

bool
is_token(std::string_view line) {
    return line.size() == TOKEN_DIGITS
           && std::ranges::all_of(line, [](char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); });
}

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
            const nlohmann::json& holder = item.at("holder");
            const Holder who = holder_of(item.at("state").get<std::string>());
            list.boards.push_back(
                {.id = item.at("board").get<std::string>(),
                 .port = port.is_null() ? std::string() : port.get<std::string>(),
                 .holder = who,
                 .note = who == Holder::Person && holder.is_object() ? holder.value("purpose", "") : ""});
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

bool
recordable(const Board& board) {
    return board.holder != Holder::Command && !board.port.empty();
}

std::string
token_in(const std::string& output) {
    std::istringstream lines(output);
    for (std::string line; std::getline(lines, line);) {
        if (line.ends_with('\r')) {
            line.pop_back();
        }
        if (is_token(line)) {
            return line;
        }
    }
    return "";
}

std::string
reservation_token(const Board& board, const std::filesystem::path& log, std::string* why) {
    const std::string note = board.note.empty() ? std::string(RECORDING_NOTE) : board.note;
    const bool ran = run_logged(autana_command({"--wait", "0", "-b", board.id, "lock", "hand", note}), log);
    const std::string output = read_text(log);
    const std::string token = ran ? token_in(output) : "";
    if (token.empty()) {
        *why = "autana lock hand gave no token to record inside the reservation (an autana that does not print it "
               "gives none): "
               + output;
    }
    return token;
}

} // namespace loupe
