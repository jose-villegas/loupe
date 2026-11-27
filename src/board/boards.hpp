// The boards autana's board pool knows, and the command line that runs autana.
#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace loupe {

// Who has a board: nobody; a person, through `autana lock hand`, whose own
// commands run inside the reservation with its token; or a command, which
// keeps it until it ends.
enum class Holder : uint8_t { Nobody, Person, Command };

// One board the board pool knows: its serial, its port when plugged in here,
// who has it, a person's reservation's note, and for a command's hold who
// holds it and when the pool expects it free, such as "held by board-job:
// autana lock run, free ~23:37".
struct Board {
    std::string id, port;
    Holder holder = Holder::Command;
    std::string note, busy;
};

// What one listing found: the boards, or none with `why` set; `json` is the
// line they were read from, kept to show them at once next time.
struct BoardList {
    std::vector<Board> boards;
    std::string why, json;
};

// Runs a command line, its output into a log; true when it exits 0. run_logged's shape.
using Run = std::function<bool(const std::vector<std::string>&, const std::filesystem::path&)>;

// `id` as a file name holds it: without the colons of a serial, which a
// Windows file name cannot hold.
std::string board_file_stem(std::string id);
// The command line that runs autana with `args`: the pinned autana's, as
// autana_cli() (tools.hpp) gives it; empty without the pack tool.
std::vector<std::string> autana_command(const std::vector<std::string>& args);
// The boards in `autana devices --json`'s output, which may hold its stderr
// notes too: the JSON is the line that opens with {"boards".
BoardList parse_boards(const std::string& output);
// The pool's boards, from one `autana devices --json` run by `run`, its output into `log`.
BoardList list_boards(const std::filesystem::path& log, const Run& run);
BoardList list_boards(const std::filesystem::path& log);
// The boards a listing kept in `file`, to show before a new one comes; none
// when there is no such file.
BoardList kept_boards(const std::filesystem::path& file);
// Keeps `list`'s boards in `file` when it found any; false when it could not write.
bool keep_boards(const BoardList& list, const std::filesystem::path& file);
// Whether `board` is plugged in here: autana gave its port. The pool also
// lists boards only seen here before, or named by a lapsed lock, with none.
bool plugged_here(const Board& board);
// Whether `board` can be recorded from here: plugged in here (autana record
// reads its USB port) and not held by a command, unless `in_its_hold`: the
// window runs inside that command's hold, with its token.
bool recordable(const Board& board, bool in_its_hold = false);
// The token in `autana lock hand`'s output `output`: the line that is a lock
// token alone. Empty when there is none.
std::string token_in(const std::string& output);
// The token of `board`'s person's reservation, from `autana --wait 0 -b BOARD
// lock hand NOTE`, which renews the reservation with its own note; the output
// into `log`. Without the wait, a board a command took since the listing
// refuses at once. Empty, with `why` set, when autana gave none: it also
// refuses another person's reservation.
std::string reservation_token(const Board& board, const std::filesystem::path& log, std::string* why);

} // namespace loupe
