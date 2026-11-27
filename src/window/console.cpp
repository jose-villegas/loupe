#include "window/console.hpp"

#include <algorithm>
#include <ctime>
#include <format>
#include <string>
#include <vector>

#include "window/fonts.hpp"

namespace loupe::window {

namespace {

// Warnings and errors, in the colours editors give them.
constexpr ImVec4 WARNING_COLOUR(1.0F, 0.8F, 0.35F, 1.0F);
constexpr ImVec4 ERROR_COLOUR(1.0F, 0.42F, 0.4F, 1.0F);

const char*
level_word(log::Level level) {
    return level == log::Level::Error ? "error" : level == log::Level::Warning ? "warning" : "info";
}

std::tm
local_time(std::time_t at) {
    std::tm out{};
#if defined(_WIN32)
    localtime_s(&out, &at);
#else
    localtime_r(&at, &out);
#endif
    return out;
}

// `console.rows`: the seqs of the lines its filter passes, each line's source
// and text as one. A line is filtered once, when it comes; the rows of lines
// the ring let go leave from the front; a new filter filters every line again.
void
filter_rows(Console& console) {
    const std::string filter = console.filter.InputBuf;
    if (console.rows_filter != filter) {
        console.rows.clear();
        console.rows_seen = 0;
        console.rows_filter = filter;
    }
    if (console.lines.size() == 0) {
        console.rows.clear();
        return;
    }
    const uint64_t first = console.lines[0].seq;
    console.rows.erase(console.rows.begin(), std::ranges::lower_bound(console.rows, first));
    const bool all = !console.filter.IsActive();
    for (size_t i = console.rows_seen < first ? 0 : console.rows_seen - first + 1; i < console.lines.size(); i++) {
        const log::Line& line = console.lines[i];
        if (all || console.filter.PassFilter(std::format("[{}] {}", line.source, line.text).c_str())) {
            console.rows.push_back(line.seq);
        }
    }
    console.rows_seen = console.lines[console.lines.size() - 1].seq;
}

// The kept line numbered `seq`: the lines' seqs run on without a gap.
const log::Line&
line_of(const Console& console, uint64_t seq) {
    return console.lines[static_cast<size_t>(seq - console.lines[0].seq)];
}

// One line in its level's colour, its right-click menu copying it.
void
draw_line(const log::Line& line, bool times) {
    const std::string text = shown_line(line, times);
    const bool coloured = line.level != log::Level::Info;
    if (coloured) {
        ImGui::PushStyleColor(ImGuiCol_Text, line.level == log::Level::Error ? ERROR_COLOUR : WARNING_COLOUR);
    }
    ImGui::PushID(static_cast<int>(line.seq));
    // Text, not a label: a line's own "##" would hide what follows it.
    ImGui::TextUnformatted(text.c_str());
    if (ImGui::BeginPopupContextItem("line")) {
        if (ImGui::MenuItem("Copy line")) {
            ImGui::SetClipboardText(text.c_str());
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
    if (coloured) {
        ImGui::PopStyleColor();
    }
}

// The filtered lines in a scrolling child, following new ones while scrolled to the bottom.
void
draw_lines(const Console& console) {
    if (!ImGui::BeginChild("lines", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::EndChild();
        return;
    }
    const bool at_bottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY();
    {
        // Ended before EndChild, which checks the font stack is as Begin found it.
        const FontScope mono(FontRole::Mono);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(console.rows.size()));
        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
                draw_line(line_of(console, console.rows[static_cast<size_t>(row)]), console.times);
            }
        }
        ImGui::PopStyleVar();
    }
    if (at_bottom) {
        ImGui::SetScrollHereY(1.0F);
    }
    ImGui::EndChild();
}

} // namespace

void
pull(Console& console, const log::Sink& from) {
    console.seen = from.read_since(console.seen, [&console](const log::Line& line) { console.lines.push(line); });
}

std::string
shown_line(const log::Line& line, bool times) {
    std::string out;
    if (times) {
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(line.time.time_since_epoch()).count();
        constexpr long long MS_PER_S = 1000;
        const std::tm at = local_time(static_cast<std::time_t>(ms / MS_PER_S));
        out = std::format("{:02}:{:02}:{:02}.{:03} ", at.tm_hour, at.tm_min, at.tm_sec, ms % MS_PER_S);
    }
    return out + std::format("{:<7} [{}] {}", level_word(line.level), line.source, line.text);
}

void
draw_console(Console& console) {
    if (ImGui::Button("Clear")) {
        console.lines.clear();
    }
    ImGui::SameLine();
    const bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    ImGui::Checkbox("Time", &console.times);
    ImGui::SameLine();
    constexpr const char FILTER[] = "Filter";
    ImGui::SetNextItemWidth(-(ImGui::CalcTextSize(FILTER).x + ImGui::GetStyle().ItemInnerSpacing.x));
    console.filter.Draw(FILTER);
    filter_rows(console);
    if (copy) {
        std::string text;
        for (const uint64_t row : console.rows) {
            text += shown_line(line_of(console, row), console.times) + '\n';
        }
        ImGui::SetClipboardText(text.c_str());
    }
    ImGui::Separator();
    draw_lines(console);
}

} // namespace loupe::window
