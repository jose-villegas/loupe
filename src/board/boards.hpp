// The boards autana's board pool knows, and the command line that runs autana.
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace loupe {

// Who has a board: nobody; a person, through `autana lock hand`, whose own
// commands run inside the reservation with its token; or a command, which
// keeps it until it ends.
enum class Holder : uint8_t { Nobody, Person, Command };

// One board the board pool knows: its serial, its port when plugged in here,
// who has it, and a person's reservation's note.
struct Board {
    std::string id, port;
    Holder holder = Holder::Command;
    std::string note;
};

// What one listing found: the boards, or none with `why` set.
struct BoardList {
    std::vector<Board> boards;
    std::string why;
};

// The command line that runs autana with `args`: through cmd.exe on Windows,
// where autana is a .cmd; autana from PATH elsewhere.
std::vector<std::string> autana_command(const std::vector<std::string>& args);
// The boards in `autana devices --json`'s output, which may hold its stderr
// notes too: the JSON is the line that opens with {"boards".
BoardList parse_boards(const std::string& output);
// The pool's boards, from `autana devices --json`, its output into `log`.
BoardList list_boards(const std::filesystem::path& log);
// Whether `board` is plugged in here: autana gave its port. The pool also
// lists boards only seen here before, or named by a lapsed lock, with none.
bool plugged_here(const Board& board);
// Whether `board` can be recorded from here: plugged in here (autana record
// reads its USB port) and not held by a command.
bool recordable(const Board& board);
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
