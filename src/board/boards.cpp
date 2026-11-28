#include "board/boards.hpp"

#include <algorithm>
#include <ctime>
#include <format>
#include <fstream>
#include <sstream>
#include <string_view>

#include <nlohmann/json.hpp>

#include "paths.hpp"
#include "process.hpp"
#include "tools.hpp"

namespace loupe {

namespace {

// The pool's words for a board no one holds and for a person's reservation.
constexpr std::string_view FREE_STATE = "unlocked";
constexpr std::string_view PERSON_STATE = "human";
// How the JSON line of `autana devices --json` opens.
constexpr std::string_view JSON_OPENS = R"({"boards")";
// Why a build without the pack tool lists no boards: it has no autana of its own.
constexpr std::string_view NO_AUTANA = "this loupe was built without the pack tool, so has no autana of its own";
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

// The clock time, here, of the epoch seconds `when`, as HH:MM.
std::string
clock_time(double when) {
    const auto seconds = static_cast<std::time_t>(when);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &seconds);
#else
    localtime_r(&seconds, &local);
#endif
    return std::format("{:02}:{:02}", local.tm_hour, local.tm_min);
}

// Who holds a board a command holds, and when the pool expects it free: the
// owner's program (its "user@host:program:pid" past the host) and its
// purpose, from `item`'s holder.
std::string
busy_text(const nlohmann::json& item) {
    const nlohmann::json& holder = item.at("holder");
    std::string who = holder.is_string() ? holder.get<std::string>() : "";
    if (holder.is_object()) {
        const std::string owner = holder.value("owner", "");
        const size_t host_end = owner.find(':');
        who = host_end == std::string::npos ? owner
                                            : owner.substr(host_end + 1, owner.find(':', host_end + 1) - host_end - 1);
        if (const std::string purpose = holder.value("purpose", ""); !purpose.empty()) {
            who += ": " + purpose;
        }
    }
    std::string text = who.empty() ? "held" : "held by " + who;
    if (const auto free = item.find("estimated_free"); free != item.end() && free->is_number()) {
        text += ", free ~" + clock_time(free->get<double>());
    }
    return text;
}

bool
is_token(std::string_view line) {
    return line.size() == TOKEN_DIGITS
           && std::ranges::all_of(line, [](char c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'); });
}

} // namespace

std::string
board_file_stem(std::string id) {
    std::erase(id, ':');
    return id;
}

std::vector<std::string>
autana_command(const std::vector<std::string>& args) {
    return autana_cli(args);
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
        return {.boards = {}, .why = "autana devices --json gave no boards: " + output, .json = {}};
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
                 .note = who == Holder::Person && holder.is_object() ? holder.value("purpose", "") : "",
                 .busy = who == Holder::Command ? busy_text(item) : ""});
        }
    } catch (const nlohmann::json::exception& error) {
        return {.boards = {},
                .why = std::string("autana devices --json gave a board without its fields: ") + error.what(),
                .json = {}};
    }
    list.json = json;
    return list;
}

BoardList
list_boards(const std::filesystem::path& log, const Run& run) {
    const std::vector<std::string> command = autana_command({"devices", "--json"});
    if (command.empty()) {
        return {.boards = {}, .why = std::string(NO_AUTANA), .json = {}};
    }
    if (!run(command, log)) {
        return {.boards = {}, .why = "autana devices failed: " + read_text(log), .json = {}};
    }
    return parse_boards(read_text(log));
}

BoardList
list_boards(const std::filesystem::path& log) {
    return list_boards(log, [](const std::vector<std::string>& args, const std::filesystem::path& file) {
        return run_logged(args, file);
    });
}

BoardList
kept_boards(const std::filesystem::path& file) {
    std::error_code error;
    if (!std::filesystem::exists(file, error)) {
        return {};
    }
    BoardList list = parse_boards(read_text(file));
    return list.why.empty() ? list : BoardList{};
}

bool
keep_boards(const BoardList& list, const std::filesystem::path& file) {
    if (list.json.empty()) {
        return true;
    }
    std::error_code error;
    std::filesystem::create_directories(file.parent_path(), error);
    std::ofstream out(file, std::ios::binary);
    out << list.json << '\n';
    return static_cast<bool>(out);
}

bool
plugged_here(const Board& board) {
    return !board.port.empty();
}

bool
recordable(const Board& board, bool in_its_hold) {
    return (in_its_hold || board.holder != Holder::Command) && plugged_here(board);
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
