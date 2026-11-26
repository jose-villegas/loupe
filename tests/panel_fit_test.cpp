// The window's panels fit any width: drawn headless at the default width and
// at narrow ones, with a recording's longest status, no item reaches past the
// panel's right edge; rows wrap instead. Each panel is also rasterized on the
// CPU to OUT/<panel>-<width>.bmp, so a person can look at the layout.
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "frames_fixture.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "paths.hpp"
#include "window/editor.hpp"
#include "window/panels.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "panel_fit_test";
// The frame debugger's width in a 1280-wide window, and narrow ones.
constexpr std::array<float, 3> WIDTHS = {900.0F, 320.0F, 200.0F};
// Tall enough for the debugger's rows at the narrowest width; the check is on width only.
constexpr float HEIGHT = 260.0F;
// How far past the edge counts as past it: rounding, not an item.
constexpr float SLACK = 0.5F;
// A recording's status as it ends: a long line, as autana writes one.
constexpr const char LAST_LINE[] =
    "autana record: FRAMEREC_END frames=1234 dropped=5 bytes=12345678; the stream is saved under "
    "C:\\Users\\someone\\AppData\\Roaming\\autana\\loupe\\recordings\\907069FEA308-20261117-214512.framerec";
// The background a shot is drawn over, as the window's own.
constexpr std::array<uint8_t, 3> BACKGROUND = {15, 15, 15};
constexpr int BMP_FILE_HEADER = 14;
constexpr int BMP_INFO = 40;
constexpr int BMP_HEADER = BMP_FILE_HEADER + BMP_INFO;
constexpr int BMP_BITS = 24;
// The info header's fields after the bit count: compression, image size,
// resolution x and y, colours used and important, all zero.
constexpr int BMP_ZERO_FIELDS = 6;

int failures = 0;
// The board and log a waiting viewport names, and how long it has waited.
constexpr const char BOARD[] = "907069FEA308 (COM5)";
constexpr const char PACKS_AUTANA[] = "b71ea7171";
constexpr const char LOG[] =
    "C:\\Users\\someone\\AppData\\Roaming\\autana\\loupe\\recordings\\907069FEA308-20261117-214512.framerec.log";
constexpr int WAITED_MS = 2400;

bool
expect(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    failures += ok ? 0 : 1;
    return ok;
}

// Marks every texture ImGui asks for as made, as a renderer would.
void
take_textures() {
    if (ImGui::GetDrawData() == nullptr || ImGui::GetDrawData()->Textures == nullptr) {
        return;
    }
    for (ImTextureData* texture : *ImGui::GetDrawData()->Textures) {
        if (texture->Status != ImTextureStatus_OK) {
            texture->SetTexID(static_cast<ImTextureID>(texture->UniqueID));
            texture->SetStatus(texture->Status == ImTextureStatus_WantDestroy ? ImTextureStatus_Destroyed
                                                                              : ImTextureStatus_OK);
        }
    }
}

// A shot being drawn: linear RGB, `width` by `height`, rows top down.
struct Canvas {
    std::vector<float> rgb;
    int width = 0, height = 0;
};

using Triangle = std::array<const ImDrawVert*, 3>;

// `point`'s barycentric weights in `v`, whose doubled signed area is `area`;
// none when it lies outside.
std::optional<std::array<float, 3>>
weights(const Triangle& v, float area, ImVec2 point) {
    std::array<float, 3> w{};
    for (size_t k = 0; k < 3; k++) {
        const ImVec2 a = v[(k + 1) % 3]->pos;
        const ImVec2 b = v[(k + 2) % 3]->pos;
        w[k] = (((b.x - a.x) * (point.y - a.y)) - ((b.y - a.y) * (point.x - a.x))) / area;
    }
    if (w[0] < 0.0F || w[1] < 0.0F || w[2] < 0.0F) {
        return std::nullopt;
    }
    return w;
}

// The texel of `texture` at `uv` scaling `colour`, as ImGui's renderer does.
void
sample(ImTextureData* texture, ImVec2 uv, std::array<float, 4>& colour) {
    if (texture == nullptr || texture->Pixels == nullptr) {
        return;
    }
    const int tx = std::clamp(static_cast<int>(uv.x * static_cast<float>(texture->Width)), 0, texture->Width - 1);
    const int ty = std::clamp(static_cast<int>(uv.y * static_cast<float>(texture->Height)), 0, texture->Height - 1);
    const auto* texel = static_cast<const uint8_t*>(texture->GetPixelsAt(tx, ty));
    if (texture->Format == ImTextureFormat_Alpha8) {
        colour[3] *= static_cast<float>(texel[0]) / 255.0F;
        return;
    }
    for (size_t c = 0; c < colour.size(); c++) {
        colour[c] *= static_cast<float>(texel[c]) / 255.0F;
    }
}

// The colour `v` gives where its weights are `w`, textured.
std::array<float, 4>
shade(const Triangle& v, const std::array<float, 3>& w, ImTextureData* texture) {
    std::array<float, 4> colour{};
    ImVec2 uv(0.0F, 0.0F);
    for (size_t k = 0; k < 3; k++) {
        const ImVec4 c = ImGui::ColorConvertU32ToFloat4(v[k]->col);
        colour[0] += w[k] * c.x;
        colour[1] += w[k] * c.y;
        colour[2] += w[k] * c.z;
        colour[3] += w[k] * c.w;
        uv.x += w[k] * v[k]->uv.x;
        uv.y += w[k] * v[k]->uv.y;
    }
    sample(texture, uv, colour);
    return colour;
}

// `v` blended over `canvas` inside `clip`.
void
fill(Canvas& canvas, const Triangle& v, const ImVec4& clip, ImTextureData* texture) {
    const float area = ((v[1]->pos.x - v[0]->pos.x) * (v[2]->pos.y - v[0]->pos.y))
                       - ((v[2]->pos.x - v[0]->pos.x) * (v[1]->pos.y - v[0]->pos.y));
    if (area == 0.0F) {
        return;
    }
    // The triangle's box, inside the clip and the canvas.
    ImRect box(v[0]->pos, v[0]->pos);
    box.Add(v[1]->pos);
    box.Add(v[2]->pos);
    box.ClipWithFull(ImRect(clip.x, clip.y, clip.z, clip.w));
    box.ClipWithFull(ImRect(0.0F, 0.0F, static_cast<float>(canvas.width), static_cast<float>(canvas.height)));
    const int x0 = static_cast<int>(box.Min.x), y0 = static_cast<int>(box.Min.y);
    const int x1 = static_cast<int>(std::ceil(box.Max.x)), y1 = static_cast<int>(std::ceil(box.Max.y));
    for (int y = y0; y < y1; y++) {
        for (int x = x0; x < x1; x++) {
            const std::optional<std::array<float, 3>> w =
                weights(v, area, ImVec2(static_cast<float>(x) + 0.5F, static_cast<float>(y) + 0.5F));
            if (!w) {
                continue;
            }
            const std::array<float, 4> colour = shade(v, *w, texture);
            float* out = &canvas.rgb[(static_cast<size_t>(y * canvas.width) + static_cast<size_t>(x)) * 3];
            for (size_t c = 0; c < 3; c++) {
                out[c] += ((colour[c] * 255.0F) - out[c]) * colour[3];
            }
        }
    }
}

// The draw data's triangles over BACKGROUND, `width` by `height`, as BGR rows top down.
std::vector<uint8_t>
rasterize(const ImDrawData& data, int width, int height) {
    Canvas canvas{.rgb = std::vector<float>(static_cast<size_t>(width * height) * 3), .width = width, .height = height};
    for (size_t i = 0; i < canvas.rgb.size(); i++) {
        canvas.rgb[i] = BACKGROUND[i % 3];
    }
    for (const ImDrawList* list : data.CmdLists) {
        for (const ImDrawCmd& cmd : list->CmdBuffer) {
            for (unsigned t = 0; t + 2 < cmd.ElemCount; t += 3) {
                Triangle v{};
                for (unsigned k = 0; k < 3; k++) {
                    v[k] = &list->VtxBuffer[static_cast<int>(
                        cmd.VtxOffset + list->IdxBuffer[static_cast<int>(cmd.IdxOffset + t + k)])];
                }
                fill(canvas, v, cmd.ClipRect, cmd.TexRef._TexData);
            }
        }
    }
    std::vector<uint8_t> bgr(canvas.rgb.size());
    for (size_t i = 0; i < canvas.rgb.size(); i += 3) {
        for (size_t c = 0; c < 3; c++) {
            bgr[i + c] = static_cast<uint8_t>(std::clamp(canvas.rgb[i + 2 - c], 0.0F, 255.0F));
        }
    }
    return bgr;
}

// `value` as little-endian bytes, as wide as its type.
template <typename Value>
void
put(std::vector<uint8_t>& out, Value value) {
    for (size_t i = 0; i < sizeof(Value); i++) {
        out.push_back(static_cast<uint8_t>(static_cast<uint32_t>(value) >> (8 * i)));
    }
}

// `bgr`, rows top down, as a 24-bit BMP at `file`.
bool
write_bmp(const fs::path& file, const std::vector<uint8_t>& bgr, int width, int height) {
    const int stride = ((width * 3) + 3) & ~3;
    std::vector<uint8_t> out = {'B', 'M'};
    put(out, static_cast<uint32_t>(BMP_HEADER + (stride * height)));
    put(out, uint32_t{0});
    put(out, static_cast<uint32_t>(BMP_HEADER));
    put(out, static_cast<uint32_t>(BMP_INFO));
    put(out, static_cast<uint32_t>(width));
    put(out, static_cast<uint32_t>(height));
    put(out, uint16_t{1});
    put(out, static_cast<uint16_t>(BMP_BITS));
    for (int i = 0; i < BMP_ZERO_FIELDS; i++) {
        put(out, uint32_t{0});
    }
    for (int y = height - 1; y >= 0; y--) {
        const auto row = bgr.begin() + (static_cast<std::ptrdiff_t>(y) * width * 3);
        out.insert(out.end(), row, row + (static_cast<std::ptrdiff_t>(width) * 3));
        out.resize(out.size() + static_cast<size_t>(stride - (width * 3)));
    }
    std::ofstream file_out(file, std::ios::binary);
    file_out.write(reinterpret_cast<const char*>(out.data()), static_cast<std::streamsize>(out.size()));
    return static_cast<bool>(file_out);
}

// One frame with `panel` drawn in a window `width` wide; true when nothing it
// drew reaches past the window's content. The frame is kept as OUT/<name>-<width>.bmp.
bool
fits(const char* name, float width, const fs::path& out, const std::function<void()>& panel,
     ImGuiWindowFlags flags = 0) {
    ImGui::GetIO().DisplaySize = {width, HEIGHT};
    // Twice: the first frame decides whether the window shows a scrollbar,
    // which narrows its content; the second is drawn and checked at that width.
    float reach = 0.0F, edge = 0.0F;
    for (int pass = 0; pass < 2; pass++) {
        take_textures();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos({0.0F, 0.0F});
        ImGui::SetNextWindowSize({width, HEIGHT});
        ImGui::Begin(name, nullptr,
                     flags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        panel();
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        reach = window->DC.CursorMaxPos.x;
        edge = window->WorkRect.Max.x;
        ImGui::End();
        ImGui::Render();
    }
    take_textures();
    const int w = static_cast<int>(width), h = static_cast<int>(HEIGHT);
    (void)write_bmp(out / (std::string(name) + "-" + std::to_string(w) + ".bmp"),
                    rasterize(*ImGui::GetDrawData(), w, h), w, h);
    return expect(reach <= edge + SLACK, std::string(name) + " at " + std::to_string(w) + " px: reaches "
                                             + std::to_string(reach) + ", edge " + std::to_string(edge));
}

// `panel` drawn `width` wide, measured with content_height(), then drawn in a
// window that tall: true when that window needs no scrolling, as the frame
// debugger's dock is fitted to its rows.
bool
tall_enough(const std::string& name, float width, const std::function<void()>& panel) {
    ImGui::GetIO().DisplaySize = {width, HEIGHT};
    float needed = HEIGHT;
    float scroll = 0.0F;
    // Twice each: a window's scroll range comes from the frame before.
    for (int pass = 0; pass < 4; pass++) {
        take_textures();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos({0.0F, 0.0F});
        ImGui::SetNextWindowSize({width, pass < 2 ? HEIGHT : needed});
        ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        panel();
        if (pass < 2) {
            needed = loupe::window::content_height();
        }
        scroll = ImGui::GetScrollMaxY();
        ImGui::End();
        ImGui::Render();
    }
    take_textures();
    return expect(scroll <= SLACK, name + " at " + std::to_string(static_cast<int>(width)) + " px: "
                                       + std::to_string(needed) + " px tall still scrolls " + std::to_string(scroll));
}

// A recording's state before its first frame, `latest` the last line autana
// wrote so far, `began` whether FRAMEREC_BEGIN was among them.
loupe::window::BoardLink::Status
waiting(const std::string& latest, uint64_t received, bool running = true, bool began = false) {
    loupe::window::BoardLink::Status status;
    status.running = running;
    status.exit = running ? std::nullopt : std::optional(1);
    status.last_line = running ? "" : "error: the board 907069FEA308 is held by someone@legion:4242";
    status.latest = latest;
    status.began = began;
    status.received = received;
    status.elapsed = std::chrono::milliseconds(WAITED_MS);
    status.log = LOG;
    return status;
}

// Each step of the wait, as the viewport says it.
std::vector<std::pair<std::string, loupe::window::WaitLine>>
wait_shots() {
    const loupe::Frames::StreamState none;
    loupe::Frames::StreamState refused;
    refused.refused = true;
    refused.why = "pack sponza: crc 1a2b3c4d here, 5e6f7a8b on the board";
    const auto line = [&](const loupe::window::BoardLink::Status& status, const loupe::Frames::StreamState& stream) {
        return loupe::window::waiting_for_frames(status, stream, {.board = BOARD, .packs_autana = PACKS_AUTANA});
    };
    return {{"wait_starting", line(waiting("", 0), none)},
            {"wait_autana", line(waiting("waiting for the board lock (held by ci@helios)", 0), none)},
            // A board line after FRAMEREC_BEGIN: still recording.
            {"wait_recording", line(waiting("I (55612) render_lab: scene sponza", 0, true, true), none)},
            {"wait_reading", line(waiting("FRAMEREC_BEGIN ring=98304 keyframe_every=30", 4096, true, true), none)},
            {"wait_ended", line(waiting("", 0, false), none)},
            {"wait_refused", line(waiting("FRAMEREC_BEGIN ring=98304 keyframe_every=30", 4096, true, true), refused)}};
}

// Each step of the wait comes from what autana wrote, in order, and a frame ends it.
void
waits() {
    using loupe::window::Waiting;
    const std::vector<std::pair<std::string, loupe::window::WaitLine>> shots = wait_shots();
    const std::array<Waiting, 6> phases = {Waiting::Starting, Waiting::Autana, Waiting::Linked,
                                           Waiting::Reading,  Waiting::Ended,  Waiting::Refused};
    for (size_t i = 0; i < phases.size(); i++) {
        (void)expect(shots[i].second.phase == phases[i] && !shots[i].second.text.empty(),
                     shots[i].first + " is its step");
    }
    (void)expect(shots[4].second.text.find(LOG) != std::string::npos, "an end before any frame names the log");
    (void)expect(shots[5].second.text.find("crc") != std::string::npos, "a refusal says why");
    (void)expect(shots[5].second.text.find(PACKS_AUTANA) != std::string::npos,
                 "a refusal names the autana our packs come from");
    loupe::window::WaitFor wait{.board = BOARD, .packs_autana = PACKS_AUTANA};
    loupe::Frames::StreamState shown;
    shown.frames = 1;
    (void)expect(loupe::window::waiting_for_frames(waiting("", 1), shown, wait).phase == Waiting::Shown,
                 "the first frame ends the wait");
    shown.refused = true;
    (void)expect(loupe::window::waiting_for_frames(waiting("", 1), shown, wait).phase == Waiting::Refused,
                 "a frame counted, then refused before it was drawn, says the refusal");
    wait.drew = true;
    (void)expect(loupe::window::waiting_for_frames(waiting("", 1), shown, wait).phase == Waiting::Shown,
                 "a refusal after a frame was drawn keeps the frame (the debugger says why)");
}

// The screen says what it is in one line: the board live, the file Record saves to, or that the link ended.
void
live_lines() {
    const fs::path file = fs::path("recordings") / "907069FEA308-20261117-214512.framerec";
    (void)expect(loupe::window::live_line(BOARD, {}, true) == std::string("Live: ") + BOARD,
                 "a live board says it is live");
    (void)expect(loupe::window::live_line(BOARD, file, true)
                     == std::string("Live: ") + BOARD + " · recording 907069FEA308-20261117-214512.framerec",
                 "while Record is on it names the file the frames go to");
    (void)expect(loupe::window::live_line(BOARD, file, false) == std::string(BOARD) + ": the link ended",
                 "an ended link says so");
}

} // namespace

int
main(int argc, char** argv) {
    const std::span<char*> args(argv, static_cast<size_t>(argc));
    if (args.size() != 3) {
        std::fprintf(stderr, "usage: %s SCENE.apak OUT_DIR\n", TEST);
        return 2;
    }
    const fs::path out = args[2];
    fs::create_directories(out);
    loupe::Options options;
    options.scene = args[1];
    options.out_bmp = loupe::utf8(out / "frame.bmp");
    const std::unique_ptr<loupe::Frames> frames = loupe::test::open_or_report(TEST, options);
    if (frames == nullptr) {
        return 1;
    }
    (void)frames->step(frames->dt_ms());
    const std::vector<loupe::DebugView> views = loupe::debug_views();

    ImGui::CreateContext();
    ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    ImGui::GetIO().IniFilename = nullptr;

    loupe::window::BoardControls controls;
    loupe::window::Observer observer;
    loupe::window::Timeline timeline;
    std::optional<int> view;
    const loupe::Frames::StreamState counts = {.frames = 1234, .dropped = 5, .refused = false, .why = ""};
    loupe::window::BoardLink::Status live;
    live.running = true;
    loupe::window::BoardLink::Status stopping = live;
    stopping.stopped = true;
    loupe::window::BoardLink::Status ended;
    ended.stopped = true;
    ended.exit = 0;
    ended.last_line = LAST_LINE;
    loupe::Frames::StreamState refused = counts;
    refused.refused = true;
    refused.why = "pack sponza: crc 1a2b3c4d here, 5e6f7a8b on the board; flash the build this window reads";

    loupe::window::Attach attach;
    attach.listed = true;
    attach.list.boards = {{.id = "907069FEA308", .port = "COM5", .holder = loupe::Holder::Nobody, .note = ""},
                          {.id = "A0B1C2D3E4F5",
                           .port = "/dev/serial/by-id/usb-Espressif_USB_JTAG_serial_debug_unit-if00",
                           .holder = loupe::Holder::Command,
                           .note = ""},
                          {.id = "0123456789AB", .port = "", .holder = loupe::Holder::Nobody, .note = ""},
                          {.id = "907069FEA309",
                           .port = "COM9",
                           .holder = loupe::Holder::Person,
                           .note = "checking the panel after the reflash"}};
    const loupe::window::BoardRows rows = loupe::window::board_rows(attach.list);
    (void)expect(rows.here.size() == 3 && rows.elsewhere.size() == 1 && rows.elsewhere[0]->id == "0123456789AB",
                 "the boards plugged in here are listed, a board with no port goes under not plugged in here");
    std::promise<loupe::editor::Rebuild> asking;
    attach.listing = std::async(std::launch::deferred, [] { return loupe::BoardList{}; });
    waits();
    live_lines();

    loupe::editor::SceneDoc doc;
    doc.objects.push_back({.path = "capybara_body",
                           .parent = "",
                           .kind = "renderer",
                           .local = {},
                           .saved = {},
                           .baked = false,
                           .locked = "",
                           .path_source = std::nullopt});
    loupe::window::Editor editor = loupe::window::editor_over(doc);
    editor.selected = doc.objects[0].path;

    for (const float width : WIDTHS) {
        const auto debugger = [&](const char* name, const loupe::Frames::StreamState& stream,
                                  const std::optional<loupe::window::BoardLink::Status>& link,
                                  loupe::StreamSaver::State saving = loupe::StreamSaver::State::Off) {
            const auto panel = [&] {
                (void)loupe::window::draw_debugger(
                    {.frames = *frames, .views = views, .stream = stream, .link = link, .saving = saving, .what = ""},
                    controls, observer, timeline, &view);
            };
            (void)fits(name, width, out, panel);
            (void)tall_enough(std::string(name) + "_height", width, panel);
        };
        debugger("debugger_live", counts, live);
        debugger("debugger_record_waiting", counts, live, loupe::StreamSaver::State::Waiting);
        debugger("debugger_recording", counts, live, loupe::StreamSaver::State::Saving);
        debugger("debugger_stopping", counts, stopping);
        debugger("debugger_ended", counts, ended);
        debugger("debugger_refused", refused, live);
        debugger("debugger_saved", counts, std::nullopt);
        (void)fits("attach", width, out, [&] { loupe::window::draw_attach(attach); });
        editor.checked.reset();
        editor.checking.result = asking.get_future();
        (void)fits("inspector_checking", width, out, [&] { loupe::window::draw_inspector(editor); });
        asking.set_value({.ok = true,
                          .rebake_rows = {"sponza/lightmap  scenes/sponza/sponza.toml changed: sun.direction",
                                          "sponza/probes  scenes/sponza/sponza.toml changed: sun.direction"},
                          .why = ""});
        asking = {};
        (void)fits("inspector", width, out, [&] { loupe::window::draw_inspector(editor); });
        (void)fits(
            "outliner", width, out, [&] { loupe::window::draw_outliner(editor); },
            ImGuiWindowFlags_HorizontalScrollbar);
        (void)fits("handle_bar", width, out, [&] { loupe::window::draw_handle_bar(editor); });
        for (const auto& [name, wait] : wait_shots()) {
            (void)fits(name.c_str(), width, out, [&] { loupe::window::draw_waiting(wait); });
        }
    }
    ImGui::DestroyContext();
    (void)frames->close();
    return failures == 0 ? 0 : 1;
}
