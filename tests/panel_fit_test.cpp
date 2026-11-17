// The window's panels fit any width: drawn headless at the default width and
// at narrow ones, with a recording's longest status, no item reaches past the
// panel's right edge; rows wrap instead. Each panel is also rasterized on the
// CPU to OUT/<panel>-<width>.bmp, so a person can look at the layout.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <span>
#include <string>
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
constexpr float HEIGHT = 260.0F;
// How far past the edge counts as past it: rounding, not an item.
constexpr float SLACK = 0.5F;
// A recording's status as it ends: a long line, as autana writes one.
constexpr const char LAST_LINE[] =
    "autana record: FRAMEREC_END frames=1234 dropped=5 bytes=12345678; the stream is saved under "
    "C:\\Users\\someone\\AppData\\Roaming\\autana\\loupe\\recordings\\907069FEA308-20261117-214512.framerec";
// The background a shot is drawn over, as the window's own.
constexpr std::array<uint8_t, 3> BACKGROUND = {15, 15, 15};
constexpr int BMP_HEADER = 54;
constexpr int BMP_BITS = 24;
constexpr int BMP_INFO = 40;

int failures = 0;

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

// The draw data's triangles over BACKGROUND, `width` by `height`, as BGR rows top down.
std::vector<uint8_t>
rasterize(const ImDrawData& data, int width, int height) {
    std::vector<float> rgb(static_cast<size_t>(width * height) * 3);
    for (size_t i = 0; i < rgb.size(); i++) {
        rgb[i] = BACKGROUND[i % 3];
    }
    for (const ImDrawList* list : data.CmdLists) {
        for (const ImDrawCmd& cmd : list->CmdBuffer) {
            ImTextureData* texture = cmd.TexRef._TexData;
            const int x0 = std::max(0, static_cast<int>(cmd.ClipRect.x));
            const int y0 = std::max(0, static_cast<int>(cmd.ClipRect.y));
            const int x1 = std::min(width, static_cast<int>(std::ceil(cmd.ClipRect.z)));
            const int y1 = std::min(height, static_cast<int>(std::ceil(cmd.ClipRect.w)));
            for (unsigned t = 0; t + 2 < cmd.ElemCount; t += 3) {
                const ImDrawVert* v[3];
                for (unsigned k = 0; k < 3; k++) {
                    v[k] = &list->VtxBuffer[static_cast<int>(
                        cmd.VtxOffset + list->IdxBuffer[static_cast<int>(cmd.IdxOffset + t + k)])];
                }
                const float area = ((v[1]->pos.x - v[0]->pos.x) * (v[2]->pos.y - v[0]->pos.y))
                                   - ((v[2]->pos.x - v[0]->pos.x) * (v[1]->pos.y - v[0]->pos.y));
                if (area == 0.0F) {
                    continue;
                }
                const int bx0 = std::max(x0, static_cast<int>(std::min({v[0]->pos.x, v[1]->pos.x, v[2]->pos.x})));
                const int by0 = std::max(y0, static_cast<int>(std::min({v[0]->pos.y, v[1]->pos.y, v[2]->pos.y})));
                const int bx1 =
                    std::min(x1, static_cast<int>(std::ceil(std::max({v[0]->pos.x, v[1]->pos.x, v[2]->pos.x}))));
                const int by1 =
                    std::min(y1, static_cast<int>(std::ceil(std::max({v[0]->pos.y, v[1]->pos.y, v[2]->pos.y}))));
                for (int y = by0; y < by1; y++) {
                    for (int x = bx0; x < bx1; x++) {
                        const float px = static_cast<float>(x) + 0.5F, py = static_cast<float>(y) + 0.5F;
                        std::array<float, 3> w{};
                        for (int k = 0; k < 3; k++) {
                            const ImDrawVert& a = *v[(k + 1) % 3];
                            const ImDrawVert& b = *v[(k + 2) % 3];
                            w[k] = (((b.pos.x - a.pos.x) * (py - a.pos.y)) - ((b.pos.y - a.pos.y) * (px - a.pos.x)))
                                   / area;
                        }
                        if (w[0] < 0.0F || w[1] < 0.0F || w[2] < 0.0F) {
                            continue;
                        }
                        std::array<float, 4> colour{};
                        float u = 0.0F, uv_y = 0.0F;
                        for (int k = 0; k < 3; k++) {
                            const ImVec4 c = ImGui::ColorConvertU32ToFloat4(v[k]->col);
                            colour[0] += w[k] * c.x;
                            colour[1] += w[k] * c.y;
                            colour[2] += w[k] * c.z;
                            colour[3] += w[k] * c.w;
                            u += w[k] * v[k]->uv.x;
                            uv_y += w[k] * v[k]->uv.y;
                        }
                        if (texture != nullptr && texture->Pixels != nullptr) {
                            const int tx = std::clamp(static_cast<int>(u * static_cast<float>(texture->Width)), 0,
                                                      texture->Width - 1);
                            const int ty = std::clamp(static_cast<int>(uv_y * static_cast<float>(texture->Height)), 0,
                                                      texture->Height - 1);
                            const auto* texel = static_cast<const uint8_t*>(texture->GetPixelsAt(tx, ty));
                            if (texture->Format == ImTextureFormat_Alpha8) {
                                colour[3] *= static_cast<float>(texel[0]) / 255.0F;
                            } else {
                                for (int c = 0; c < 4; c++) {
                                    colour[static_cast<size_t>(c)] *= static_cast<float>(texel[c]) / 255.0F;
                                }
                            }
                        }
                        float* out = &rgb[(static_cast<size_t>(y * width) + static_cast<size_t>(x)) * 3];
                        for (int c = 0; c < 3; c++) {
                            out[c] += (colour[static_cast<size_t>(c)] * 255.0F - out[c]) * colour[3];
                        }
                    }
                }
            }
        }
    }
    std::vector<uint8_t> bgr(rgb.size());
    for (size_t i = 0; i < rgb.size(); i += 3) {
        for (size_t c = 0; c < 3; c++) {
            bgr[i + c] = static_cast<uint8_t>(std::clamp(rgb[i + 2 - c], 0.0F, 255.0F));
        }
    }
    return bgr;
}

void
put(std::vector<uint8_t>& out, uint32_t value, int bytes) {
    for (int i = 0; i < bytes; i++) {
        out.push_back(static_cast<uint8_t>(value >> (8 * i)));
    }
}

// `bgr`, rows top down, as a 24-bit BMP at `file`.
bool
write_bmp(const fs::path& file, const std::vector<uint8_t>& bgr, int width, int height) {
    const int stride = ((width * 3) + 3) & ~3;
    std::vector<uint8_t> out = {'B', 'M'};
    put(out, static_cast<uint32_t>(BMP_HEADER + (stride * height)), 4);
    put(out, 0, 4);
    put(out, BMP_HEADER, 4);
    put(out, BMP_INFO, 4);
    put(out, static_cast<uint32_t>(width), 4);
    put(out, static_cast<uint32_t>(height), 4);
    put(out, 1, 2);
    put(out, BMP_BITS, 2);
    put(out, 0, 4 * 6);
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
fits(const char* name, float width, const fs::path& out, const std::function<void()>& panel) {
    ImGui::GetIO().DisplaySize = {width, HEIGHT};
    // Twice: a row's widths are measured from the last frame's items.
    float reach = 0.0F, edge = 0.0F;
    for (int pass = 0; pass < 2; pass++) {
        take_textures();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos({0.0F, 0.0F});
        ImGui::SetNextWindowSize({width, HEIGHT});
        ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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

    loupe::window::RecordControls controls;
    loupe::window::Observer observer;
    loupe::window::Timeline timeline;
    std::optional<int> view;
    const loupe::Frames::StreamState counts = {.frames = 1234, .dropped = 5, .refused = false, .why = ""};
    const loupe::window::Recording::Status live = {
        .running = true, .stopped = false, .exit = std::nullopt, .last_line = ""};
    const loupe::window::Recording::Status ended = {
        .running = false, .stopped = true, .exit = 0, .last_line = LAST_LINE};

    loupe::editor::SceneDoc doc;
    doc.objects.push_back({.path = "capybara_body",
                           .parent = "",
                           .kind = "renderer",
                           .local = {},
                           .saved = {},
                           .baked = false,
                           .locked = ""});
    loupe::window::Editor editor = loupe::window::editor_over(doc);
    editor.selected = doc.objects[0].path;

    for (const float width : WIDTHS) {
        (void)fits("debugger_recording", width, out, [&] {
            (void)loupe::window::draw_debugger(
                {.frames = *frames, .views = views, .stream = counts, .recording = &live}, controls, observer, timeline,
                &view);
        });
        (void)fits("debugger_ended", width, out, [&] {
            (void)loupe::window::draw_debugger(
                {.frames = *frames, .views = views, .stream = counts, .recording = &ended}, controls, observer,
                timeline, &view);
        });
        (void)fits("inspector", width, out, [&] { loupe::window::draw_inspector(editor); });
        (void)fits("outliner", width, out, [&] { loupe::window::draw_outliner(editor); });
        (void)fits("handle_bar", width, out, [&] { loupe::window::draw_handle_bar(editor); });
    }
    ImGui::DestroyContext();
    (void)frames->close();
    return failures == 0 ? 0 : 1;
}
