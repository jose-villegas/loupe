// The Console panel: the log's lines (log/log.hpp) in the mono face, newest at the bottom,
// each with its time, level and source, warnings and errors in their colours.
// A filter keeps the lines holding its words, Clear empties the panel (not
// the log file), Copy puts the lines shown on the clipboard, and a line's
// right-click menu copies that line. The panel follows new lines while it is
// scrolled to the bottom, and stays where it is while scrolled up.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "imgui.h"
#include "log/log.hpp"

namespace loupe::window {

// The panel's name, which docks it.
constexpr const char CONSOLE[] = "Console";

// The panel's lines, pulled from the sink after `seen`, its settings, and the
// seqs of the lines its filter passed, up to `rows_seen`, under `rows_filter`.
struct Console {
    log::Ring lines{log::KEPT_LINES};
    uint64_t seen = 0;
    ImGuiTextFilter filter;
    bool times = true;
    bool open = false;
    std::vector<uint64_t> rows;
    uint64_t rows_seen = 0;
    std::string rows_filter;
};

// Takes the lines the sink holds after `console.seen`; when the sink let go of
// some before they were taken, the Console starts again from the ones it has,
// so its lines' seqs run on without a gap.
void pull(Console& console, const log::Sink& from = log::sink());
// `line` as the panel shows it and copies it: its time of day, level, source and text.
std::string shown_line(const log::Line& line, bool times);
// The panel's contents, inside its window.
void draw_console(Console& console);

} // namespace loupe::window
