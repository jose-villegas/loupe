// The boards autana's board pool knows, and the command line that runs autana.
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace loupe {

// One board the board pool knows: its serial, its port when plugged in here,
// and whether no one holds it.
struct Board {
    std::string id, port;
    bool free = false;
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

} // namespace loupe
