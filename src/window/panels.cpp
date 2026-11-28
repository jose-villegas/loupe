#include "window/panels.hpp"

#include <algorithm>
#include <array>
#include <cfloat>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

#include "imgui.h"
#include "imgui_internal.h"
#include "paths.hpp"
#include "window/flow.hpp"
#include "window/fonts.hpp"
#include "window/spinner.hpp"

namespace loupe::window {

namespace {

constexpr const char ABOUT[] = "About";
// The console lines a live board's controls send (console_freeze).
constexpr const char FREEZE[] = "FREEZE";
constexpr const char RESUME[] = "RESUME";
constexpr const char STEP[] = "STEP";
// The scrubber's least width beside the step buttons; narrower, it takes a line of its own.
constexpr float SCRUBBER_MIN_FONTS = 8.0F;
// The view picker's label.
constexpr const char VIEW[] = "view";

// A stream's frame and drop counts.
std::string
counts_line(const Frames::StreamState& stream) {
    return std::format("{} frames, {} dropped", stream.frames, stream.dropped);
}

// Shows or hides the Console, as in Unity.
constexpr ImGuiKeyChord CONSOLE_KEYS = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_C;

// The field rows' frames: this share of the style's padding, so they sit
// tighter than buttons and a short panel still fits each value.
constexpr float FIELD_PADDING = 0.5F;
// Room for one value as a field row's format writes it: DragScalar's own buffer.
constexpr size_t FIELD_TEXT_SIZE = 64;

// A DragFloat with its value left-aligned: ImGui centres a drag's text, so
// this draws it unseen and writes the value again from the frame's left. A
// value being typed is ImGui's own text field, left-aligned already.
bool
drag_left(float* value, float speed, const char* format) {
    const bool typing = ImGui::TempInputIsActive(ImGui::GetID(""));
    if (!typing) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 0));
    }
    const bool changed = ImGui::DragFloat("", value, speed, 0.0F, 0.0F, format);
    if (!typing) {
        // Typing may start inside the drag, its text field hidden too: this frame, this text stands in for it.
        ImGui::PopStyleColor();
        std::array<char, FIELD_TEXT_SIZE> text = {};
        ImGui::DataTypeFormatString(text.data(), static_cast<int>(text.size()), ImGuiDataType_Float, value, format);
        const ImVec2 pad = ImGui::GetStyle().FramePadding;
        const ImVec2 min = ImGui::GetItemRectMin();
        const ImVec2 max = ImGui::GetItemRectMax();
        ImGui::RenderTextClipped(ImVec2(min.x + pad.x, min.y), ImVec2(max.x - pad.x, max.y), text.data(), nullptr,
                                 nullptr, ImVec2(0.0F, 0.5F));
    }
    return changed;
}

// One selectable row per scene under `heading`, labelled by `label`; a click picks it.
template <typename Label>
void
scene_list(const char* heading, const std::vector<std::filesystem::path>& scenes, Label label, Start& start) {
    ImGui::SeparatorText(heading);
    if (scenes.empty()) {
        ImGui::TextDisabled("none");
    }
    ImGui::PushID(heading);
    for (size_t i = 0; i < scenes.size(); i++) {
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::Selectable(label(scenes[i]).c_str())) {
            start.picked = scenes[i];
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}

// After a board's port, who has it: a person records inside their own
// reservation; a command's hold keeps Record off, and says whose it is.
std::string
holder_text(const Board& board) {
    switch (board.holder) {
        case Holder::Nobody: return "";
        case Holder::Person: return board.note.empty() ? "  (reserved)" : std::format("  (reserved: {})", board.note);
        case Holder::Command: break;
    }
    return board.busy.empty() ? "  (held)" : std::format("  ({})", board.busy);
}

// Whole seconds since `since`, for a waiting step's line.
int64_t
seconds_since(std::chrono::steady_clock::time_point since) {
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - since).count();
}

// The menu bar's menus; whether About was clicked.
bool
draw_menu_bar(EditMenu& edit, bool* console) {
    if (!ImGui::BeginMenuBar()) {
        return false;
    }
    if (edit.shown && ImGui::BeginMenu("Edit")) {
        edit.undo = ImGui::MenuItem("Undo", "Ctrl+Z", false, edit.can_undo);
        edit.redo = ImGui::MenuItem("Redo", "Ctrl+Y", false, edit.can_redo);
        ImGui::Separator();
        edit.save = ImGui::MenuItem("Save", "Ctrl+S", false, edit.can_save);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window")) {
        ImGui::MenuItem("Console", "Ctrl+Shift+C", console);
        ImGui::EndMenu();
    }
    const bool about = ImGui::MenuItem(ABOUT);
    ImGui::EndMenuBar();
    return about;
}

} // namespace

BoardRows
board_rows(const BoardList& list) {
    BoardRows rows;
    for (const Board& board : list.boards) {
        (plugged_here(board) ? rows.here : rows.elsewhere).push_back(&board);
    }
    return rows;
}

bool
draw_menu(About& about, EditMenu& edit, bool* console) {
    if (!ImGui::GetIO().WantTextInput && ImGui::Shortcut(CONSOLE_KEYS, ImGuiInputFlags_RouteGlobal)) {
        *console = !*console;
    }
    // Opened outside the menu bar, whose ID scope the modal is not in.
    if (draw_menu_bar(edit, console)) {
        ImGui::OpenPopup(ABOUT);
    }
    bool changed = false;
    if (ImGui::BeginPopupModal(ABOUT, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        {
            // Both lines carry a commit's hash.
            const FontScope mono(FontRole::Mono);
            ImGui::TextUnformatted(about.version.c_str());
            if (!about.updated.empty()) {
                ImGui::TextUnformatted(about.updated.c_str());
            }
        }
        ImGui::SeparatorText("Updates");
        if (about.can_update) {
            int follow = static_cast<int>(about.follow);
            changed |= ImGui::RadioButton("Off", &follow, static_cast<int>(update::Follow::Off));
            ImGui::SameLine();
            changed |= ImGui::RadioButton("Official releases", &follow, static_cast<int>(update::Follow::Releases));
            ImGui::SameLine();
            changed |= ImGui::RadioButton("Main builds", &follow, static_cast<int>(update::Follow::Main));
            about.follow = static_cast<update::Follow>(follow);
            ImGui::TextDisabled("Checked at each start, applied when Loupe closes");
        } else {
            ImGui::TextDisabled("A local build does not update");
        }
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    return changed;
}

void
draw_start(const Library& library, const std::string& refusal, Start& start) {
    if (ImGui::Button("Open...")) {
        start.browse = true;
    }
    if (!refusal.empty()) {
        ImGui::TextWrapped("%s", refusal.c_str());
    }
    scene_list(
        "Samples", library.samples, [](const std::filesystem::path& scene) { return utf8(scene.stem()); }, start);
    scene_list("Recent", library.recent, [](const std::filesystem::path& scene) { return utf8(scene); }, start);
}

void
draw_attach(Attach& attach) {
    ImGui::SeparatorText("Boards");
    const bool listing = attach.listing.valid();
    ImGui::BeginDisabled(listing);
    attach.refresh = ImGui::Button(attach.listed ? "List again" : "List boards");
    ImGui::EndDisabled();
    if (listing) {
        const std::string asking = std::format("{} ({} s)", attach.listed ? "refreshing" : "asking the board pool",
                                               seconds_since(attach.listing_since));
        flow(spinner_text_width(asking.c_str()));
        spinner_text(asking.c_str(), true);
    }
    if (!attach.list.why.empty()) {
        ImGui::TextWrapped("%s", attach.list.why.c_str());
    } else if (attach.listed && attach.list.boards.empty()) {
        ImGui::TextDisabled("none");
    }
    const bool reserving = attach.reserving.valid();
    const BoardRows rows = board_rows(attach.list);
    for (const Board* board : rows.here) {
        ImGui::PushID(board->id.c_str());
        ImGui::BeginDisabled(!recordable(*board, !attach.lock_token.empty()) || reserving);
        if (ImGui::Button("View")) {
            attach.view = board->id;
        }
        ImGui::EndDisabled();
        if (!board->busy.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("%s", board->busy.c_str());
        }
        const std::string where = std::format("{}  {}{}", board->id, board->port, holder_text(*board));
        flow(text_width(where.c_str()));
        ImGui::TextWrapped("%s", where.c_str());
        ImGui::PopID();
    }
    if (!rows.elsewhere.empty() && ImGui::TreeNode("elsewhere", "not here (%zu)", rows.elsewhere.size())) {
        // A tree row cannot wrap: the heading stays short, and this line says what it holds.
        hint("not plugged in here: boards the pool saw here before, or names in a lapsed lock");
        for (const Board* board : rows.elsewhere) {
            ImGui::TextWrapped("%s%s", board->id.c_str(), holder_text(*board).c_str());
        }
        ImGui::TreePop();
    }
    if (reserving) {
        spinner_text(std::format("readying the live view ({} s)", seconds_since(attach.reserving_since)).c_str(), true);
    }
}

void
draw_board_controls(const Frames::StreamState& stream, const BoardLink::Status& status, StreamSaver::State saving,
                    BoardControls& controls) {
    ImGui::PushID("board");
    ImGui::BeginDisabled(status.stopped || !status.running);
    controls.record = ImGui::Button(saving == StreamSaver::State::Off ? "Record" : "Stop recording");
    const char* freeze = controls.paused ? "Unfreeze board" : "Freeze board";
    flow(button_width(freeze));
    if (ImGui::Button(freeze)) {
        controls.send = controls.paused ? RESUME : FREEZE;
        controls.paused = !controls.paused;
    }
    const char* step = "Step board";
    flow(button_width(step));
    ImGui::BeginDisabled(!controls.paused);
    if (ImGui::Button(step)) {
        controls.send = STEP;
    }
    ImGui::EndDisabled();
    ImGui::EndDisabled();
    ImGui::PopID();
    const std::string counts = counts_line(stream);
    flow(text_width(counts.c_str()));
    ImGui::TextWrapped("%s", counts.c_str());
    if (stream.refused) {
        ImGui::TextWrapped("Refused: %s", stream.why.c_str());
    }
    if (saving == StreamSaver::State::Waiting) {
        hint("Recording starts at the board's next keyframe");
    }
    if (!status.running) {
        ImGui::TextWrapped("The link ended (exit %d): %s", status.exit.value_or(-1), status.last_line.c_str());
    }
}

void
draw_timeline(const Frames& frames, Timeline& timeline) {
    const int shown = frames.index() - 1;
    const int first = frames.first_seekable();
    const int last = std::max(frames.count() - 1, first);
    if (ImGui::Button(timeline.playing ? "Pause" : "Play")) {
        timeline.playing = !timeline.playing;
    }
    flow(ImGui::GetFrameHeight());
    ImGui::BeginDisabled(shown <= first);
    if (ImGui::ArrowButton("back", ImGuiDir_Left)) {
        timeline.seek_to = shown - 1;
    }
    ImGui::EndDisabled();
    flow(ImGui::GetFrameHeight());
    ImGui::BeginDisabled(shown >= last);
    if (ImGui::ArrowButton("forward", ImGuiDir_Right)) {
        timeline.seek_to = shown + 1;
    }
    ImGui::EndDisabled();
    flow(ImGui::GetFontSize() * SCRUBBER_MIN_FONTS);
    int picked = shown;
    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::SliderInt("##frame", &picked, first, last, "frame %d")) {
        timeline.seek_to = picked;
    }
}

float
content_height() {
    // The cursor stands a row's spacing below the last row; the padding closes the window.
    const ImGuiStyle& style = ImGui::GetStyle();
    return ImGui::GetCursorPosY() - style.ItemSpacing.y + style.WindowPadding.y;
}

std::string
live_line(const std::string& board, const std::filesystem::path& saving, bool running) {
    if (!running) {
        return std::format("{}: the link ended", board);
    }
    return saving.empty() ? std::format("Live: {}", board)
                          : std::format("Live: {} · recording {}", board, utf8(saving.filename()));
}

bool
draw_debugger(const Debugger& shown, BoardControls& controls, Observer& observer, Timeline& timeline,
              std::optional<int>* view) {
    if (!shown.what.empty()) {
        ImGui::TextWrapped("%s", shown.what.c_str());
    }
    const bool back = ImGui::Button("Scenes");
    if (!shown.views.empty()) {
        int picked = shown.frames.view();
        flow(view_picker_width(shown.views));
        if (draw_view_picker(shown.views, &picked)) {
            *view = picked;
        }
    }
    if (shown.frames.can_observe()) {
        flow(check_width(OBSERVER));
        draw_observer_toggle(shown.frames, observer);
    }
    if (shown.stream && shown.link) {
        // Live: the newest frame shows, so there is no timeline to step.
        draw_board_controls(*shown.stream, *shown.link, shown.saving, controls);
        return back;
    }
    if (shown.stream) {
        ImGui::TextWrapped("%s", counts_line(*shown.stream).c_str());
    }
    ImGui::BeginDisabled(observer.on);
    draw_timeline(shown.frames, timeline);
    ImGui::EndDisabled();
    return back;
}

float
view_picker_width(const std::vector<DebugView>& views) {
    float widest = 0.0F;
    for (const DebugView& row : views) {
        widest = std::max(widest, text_width(row.name.c_str()));
    }
    const ImGuiStyle& style = ImGui::GetStyle();
    return widest + (style.FramePadding.x * 2) + ImGui::GetFrameHeight() + style.ItemInnerSpacing.x + text_width(VIEW);
}

bool
draw_view_picker(const std::vector<DebugView>& views, int* view) {
    const auto current = std::ranges::find(views, *view, &DebugView::id);
    bool changed = false;
    // As wide as its widest name, or what is left of the line beside its label.
    const float label_width = ImGui::GetStyle().ItemInnerSpacing.x + text_width(VIEW);
    ImGui::SetNextItemWidth(std::max(
        ImGui::GetFrameHeight(), std::min(view_picker_width(views), ImGui::GetContentRegionAvail().x) - label_width));
    if (ImGui::BeginCombo(VIEW, current == views.end() ? "" : current->name.c_str())) {
        for (const DebugView& row : views) {
            const bool selected = row.id == *view;
            if (ImGui::Selectable(row.name.c_str(), selected) && !selected) {
                *view = row.id;
                changed = true;
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

WaitLine
waiting_for_frames(const BoardLink::Status& status, const Frames::StreamState& stream, const WaitFor& wait) {
    const double seconds = std::chrono::duration<double>(status.elapsed).count();
    const std::string log = utf8(status.log);
    const std::string& board = wait.board;
    if (wait.drew) {
        return {.phase = Waiting::Shown, .text = ""};
    }
    if (stream.refused) {
        return {.phase = Waiting::Refused,
                .text = std::format("The stream was refused: {}\nLoupe draws a board's frames with the packs its "
                                    "pinned autana {} builds, so a pack the stream names that is missing or differs "
                                    "means the board runs another build. Flash the board from autana {} (autana flash "
                                    "dev in a checkout at that commit), or use a Loupe pinned to the board's autana. "
                                    "The stream names the board's build only by its image hash, not its autana "
                                    "commit.\nLog: {}",
                                    stream.why, wait.packs_autana, wait.packs_autana, log)};
    }
    if (stream.frames > 0) {
        return {.phase = Waiting::Shown, .text = ""};
    }
    if (!status.running) {
        return {.phase = Waiting::Ended,
                .text = std::format("autana record ended (exit {}) before any frame: {}\nLog: {}",
                                    status.exit.value_or(-1), status.last_line, log)};
    }
    if (status.received > 0) {
        return {.phase = Waiting::Reading,
                .text = std::format("Reading {}'s stream, {} bytes ({:.0f} s)", board, status.received, seconds)};
    }
    if (status.began) {
        return {.phase = Waiting::Linked,
                .text = std::format("{} is live, first frame next ({:.0f} s)", board, seconds)};
    }
    if (!status.latest.empty()) {
        return {.phase = Waiting::Autana, .text = std::format("autana: {} ({:.0f} s)", status.latest, seconds)};
    }
    return {.phase = Waiting::Starting, .text = std::format("Starting the live view on {} ({:.0f} s)", board, seconds)};
}

void
draw_waiting(const WaitLine& line) {
    // A step still under way turns; an end or a refusal stands still.
    const bool going = line.phase != Waiting::Ended && line.phase != Waiting::Refused;
    const ImVec2 room = ImGui::GetContentRegionAvail();
    const ImVec2 at = ImGui::GetCursorPos();
    const float label_x = going ? spinner_layout_here().label_x : 0.0F;
    const ImVec2 size = ImGui::CalcTextSize(line.text.c_str(), nullptr, false, room.x - label_x);
    ImGui::SetCursorPos(ImVec2(at.x, at.y + std::max(0.0F, (room.y - size.y) / 2)));
    if (going) {
        spinner_text(line.text.c_str(), false);
    } else {
        ImGui::TextWrapped("%s", line.text.c_str());
    }
}

FrameRect
draw_frame(const FrameTexture& texture) {
    const ImVec2 room = ImGui::GetContentRegionAvail();
    const int zoom =
        std::max(1, std::min(static_cast<int>(room.x) / texture.width(), static_cast<int>(room.y) / texture.height()));
    const ImVec2 size(static_cast<float>(texture.width() * zoom), static_cast<float>(texture.height() * zoom));
    const ImVec2 at = ImGui::GetCursorPos();
    ImGui::SetCursorPos(
        ImVec2(at.x + std::max(0.0F, (room.x - size.x) / 2), at.y + std::max(0.0F, (room.y - size.y) / 2)));
    // Whole zoom, nearest sampling: each engine pixel stays a sharp square.
    ImDrawList* list = ImGui::GetWindowDrawList();
    list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest, nullptr);
    ImGui::Image(texture.ref(), size);
    list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear, nullptr);
    return {.min = ImGui::GetItemRectMin(), .max = ImGui::GetItemRectMax()};
}

float
field_column(std::span<const char* const> labels) {
    const FontScope text(FontRole::Text);
    float widest = 0.0F;
    for (const char* label : labels) {
        widest = std::max(widest, ImGui::CalcTextSize(label).x);
    }
    return widest + ImGui::GetStyle().ItemSpacing.x;
}

bool
field_row(const char* label, float column, std::span<float> values, float speed, const char* format) {
    const ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(style.FramePadding.x * FIELD_PADDING, style.FramePadding.y * FIELD_PADDING));
    float width = 0.0F;
    {
        const FontScope text(FontRole::Text);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        width = ImGui::CalcTextSize(label).x;
    }
    ImGui::SameLine(0.0F, column - width);
    const FontScope mono(FontRole::Mono);
    bool changed = false;
    ImGui::BeginGroup();
    ImGui::PushID(label);
    ImGui::PushMultiItemsWidths(static_cast<int>(values.size()), ImGui::GetContentRegionAvail().x);
    for (size_t i = 0; i < values.size(); i++) {
        ImGui::PushID(static_cast<int>(i));
        if (i > 0) {
            ImGui::SameLine(0.0F, style.ItemInnerSpacing.x);
        }
        changed |= drag_left(&values[i], speed, format);
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();
    ImGui::EndGroup();
    ImGui::PopStyleVar();
    return changed;
}

} // namespace loupe::window
