#include "window/panels.hpp"

#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <format>
#include <string>

#include "imgui.h"
#include "paths.hpp"
#include "window/flow.hpp"

namespace loupe::window {

namespace {

constexpr const char ABOUT[] = "About";
// The console lines a recording's controls send (console_freeze).
constexpr const char FREEZE[] = "FREEZE";
constexpr const char RESUME[] = "RESUME";
constexpr const char STEP[] = "STEP";
// The scrubber's least width beside the step buttons; narrower, it takes a line of its own.
constexpr float SCRUBBER_MIN_FONTS = 8.0F;
// The view picker's label.
constexpr const char VIEW[] = "view";

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

} // namespace

bool
draw_menu(About& about, EditMenu& edit) {
    bool open = false;
    if (ImGui::BeginMenuBar()) {
        if (edit.shown && ImGui::BeginMenu("Edit")) {
            edit.undo = ImGui::MenuItem("Undo", "Ctrl+Z", false, edit.can_undo);
            edit.redo = ImGui::MenuItem("Redo", "Ctrl+Y", false, edit.can_redo);
            ImGui::Separator();
            edit.save = ImGui::MenuItem("Save", "Ctrl+S", false, edit.can_save);
            ImGui::EndMenu();
        }
        open = ImGui::MenuItem(ABOUT);
        ImGui::EndMenuBar();
    }
    // Opened outside the menu bar, whose ID scope the modal is not in.
    if (open) {
        ImGui::OpenPopup(ABOUT);
    }
    bool changed = false;
    if (ImGui::BeginPopupModal(ABOUT, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted(about.version.c_str());
        if (!about.updated.empty()) {
            ImGui::TextUnformatted(about.updated.c_str());
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
        ImGui::SameLine();
        ImGui::TextDisabled("asking the board pool");
    }
    if (!attach.list.why.empty()) {
        ImGui::TextWrapped("%s", attach.list.why.c_str());
    } else if (attach.listed && attach.list.boards.empty()) {
        ImGui::TextDisabled("none");
    }
    for (const Board& board : attach.list.boards) {
        ImGui::PushID(board.id.c_str());
        // autana record reads the board's USB port, so only a free board plugged in here records.
        ImGui::BeginDisabled(!board.free || board.port.empty());
        if (ImGui::Button("Record")) {
            attach.record = board.id;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("%s  %s%s", board.id.c_str(), board.port.empty() ? "on another host" : board.port.c_str(),
                    board.free ? "" : "  (held)");
        ImGui::PopID();
    }
}

void
draw_record_controls(const Frames::StreamState& stream, const Recording::Status& status, RecordControls& controls) {
    // The board's own controls, apart from the timeline's Play and Pause.
    ImGui::PushID("board");
    ImGui::BeginDisabled(status.stopped || !status.running);
    controls.stop = ImGui::Button("Stop recording");
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
    const std::string counts = std::format("{} frames, {} dropped", stream.frames, stream.dropped);
    flow(text_width(counts.c_str()));
    ImGui::TextUnformatted(counts.c_str());
    if (stream.refused) {
        ImGui::TextWrapped("Refused: %s", stream.why.c_str());
    }
    if (!status.running) {
        ImGui::TextWrapped("Recording ended (exit %d): %s", status.exit.value_or(-1), status.last_line.c_str());
    } else if (status.stopped) {
        ImGui::TextDisabled("Stopping: the board drains what it holds");
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

bool
draw_debugger(const Debugger& shown, RecordControls& controls, Observer& observer, Timeline& timeline,
              std::optional<int>* view) {
    const bool back = ImGui::Button("Scenes");
    if (!shown.views.empty()) {
        int picked = shown.frames.view();
        flow(view_picker_width(shown.views));
        if (draw_view_picker(shown.views, &picked)) {
            *view = picked;
        }
    }
    draw_observer_toggle(shown.frames, observer);
    if (shown.stream && shown.recording != nullptr) {
        draw_record_controls(*shown.stream, *shown.recording, controls);
    } else if (shown.stream) {
        ImGui::Text("%d frames, %d dropped", shown.stream->frames, shown.stream->dropped);
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
    const float label = ImGui::GetStyle().ItemInnerSpacing.x + text_width(VIEW);
    ImGui::SetNextItemWidth(std::max(ImGui::GetFrameHeight(),
                                     std::min(view_picker_width(views), ImGui::GetContentRegionAvail().x) - label));
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

} // namespace loupe::window
