// ImVec2's arithmetic, for the crop.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "storyboard/runner.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <memory>
#include <sstream>
#include <string_view>
#include <system_error>
#include <utility>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_te_context.h"
#include "imgui_te_coroutine.h"
#include "imgui_te_engine.h"
#include "paths.hpp"

namespace loupe::storyboard {

namespace {

constexpr const char OPEN[] = "loupe.open";
constexpr const char CLICK[] = "loupe.click";
constexpr const char CHECK[] = "loupe.check";
constexpr const char UNCHECK[] = "loupe.uncheck";
constexpr const char INPUT[] = "loupe.input";
constexpr const char KEY[] = "loupe.key";
constexpr const char HOLD[] = "loupe.hold";
constexpr const char WAIT[] = "loupe.wait";
constexpr const char SCREENSHOT[] = "loupe.screenshot";
// Where a picture is taken with the mouse: off the window, so no item shows hovered.
constexpr ImVec2 OFF_WINDOW(-FLT_MAX, -FLT_MAX);

// The member `name` of a step's map value, as a string; empty when absent.
std::string
member(const nlohmann::json& value, const char* name) {
    return value.is_object() && value.contains(name) && value[name].is_string() ? value[name].get<std::string>() : "";
}

// `value` as a whole number from `least`; none for anything else.
std::optional<int>
count(const nlohmann::json& value, int least) {
    long long parsed = 0;
    if (!value.is_string() || !parse_count(value.get<std::string>(), least, INT32_MAX, &parsed)) {
        return std::nullopt;
    }
    return static_cast<int>(parsed);
}

bool
same_letters(std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
    });
}

// A chord such as "ctrl+z", "Escape" or "ctrl+shift+z", in ImGui's key names
// and any case; none for a word that names no key or modifier.
std::optional<ImGuiKeyChord>
chord(const std::string& text) {
    ImGuiKeyChord mods = 0;
    std::string_view rest = text;
    for (size_t plus = rest.find('+'); plus != std::string_view::npos && plus + 1 < rest.size();
         plus = rest.find('+')) {
        const std::string_view mod = rest.substr(0, plus);
        if (same_letters(mod, "ctrl")) {
            mods |= ImGuiMod_Ctrl;
        } else if (same_letters(mod, "shift")) {
            mods |= ImGuiMod_Shift;
        } else if (same_letters(mod, "alt")) {
            mods |= ImGuiMod_Alt;
        } else if (same_letters(mod, "super")) {
            mods |= ImGuiMod_Super;
        } else {
            return std::nullopt;
        }
        rest = rest.substr(plus + 1);
    }
    for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; key++) {
        if (same_letters(ImGui::GetKeyName(static_cast<ImGuiKey>(key)), rest)) {
            return mods | key;
        }
    }
    return std::nullopt;
}

// The command line's words in a loupe.open: a string split on spaces, or a list.
std::vector<std::string>
words(const nlohmann::json& value) {
    std::vector<std::string> out;
    if (value.is_string()) {
        std::istringstream split(value.get<std::string>());
        for (std::string word; split >> word;) {
            out.push_back(word);
        }
    } else if (value.is_array()) {
        for (const nlohmann::json& word : value) {
            out.push_back(word.is_string() ? word.get<std::string>() : word.dump());
        }
    }
    return out;
}

// The part of `picture` the ImGui window `crop` covered on the last frame,
// both corners clamped to the picture, or the whole picture when `crop` is
// empty; none, with `why` set, when no window has that name.
std::optional<ImRect>
picture_rect(const window::Picture& picture, const std::string& crop, std::string* why) {
    const ImVec2 size(static_cast<float>(picture.width), static_cast<float>(picture.height));
    if (crop.empty()) {
        return ImRect(ImVec2(0, 0), size);
    }
    const ImGuiWindow* panel = ImGui::FindWindowByName(crop.c_str());
    if (panel == nullptr) {
        *why = std::format("no panel {}", crop);
        return std::nullopt;
    }
    const ImVec2 scale = ImGui::GetIO().DisplayFramebufferScale;
    const auto clamp = [&size](ImVec2 at) {
        return ImVec2(std::clamp(std::floor(at.x), 0.0F, size.x), std::clamp(std::floor(at.y), 0.0F, size.y));
    };
    return ImRect(clamp(panel->Pos * scale), clamp((panel->Pos + panel->Size) * scale));
}

} // namespace

Runner::Runner(std::vector<Item> items, const std::filesystem::path& recipe, std::filesystem::path out_dir)
    : items_(std::move(items)), recipe_(utf8(recipe)), recipe_dir_(recipe.parent_path()), out_dir_(std::move(out_dir)) {
}

// After ImGui's own context is gone (run_scripted's window), as the engine asks.
Runner::~Runner() {
    if (engine_ != nullptr) {
        ImGuiTestEngine_DestroyContext(engine_);
    }
}

// The engine in fast mode at a fixed TICK_S, quiet but for its warnings, and
// never cutting a step short: a scene's pack can take minutes to build, and
// lavapipe draws slowly.
void
Runner::start() {
    engine_ = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& io = ImGuiTestEngine_GetIO(engine_);
    io.CoroutineFuncs = Coroutine_ImplStdThread_GetInterface();
    io.ConfigRunSpeed = ImGuiTestRunSpeed_Fast;
    io.ConfigFixedDeltaTime = TICK_S;
    io.ConfigSavedSettings = false;
    io.ConfigMouseDrawCursor = false;
    io.ConfigCaptureEnabled = false;
    io.ConfigLogToTTY = true;
    io.ConfigWatchdogWarning = FLT_MAX;
    io.ConfigWatchdogKillTest = FLT_MAX;
    ImGuiTestEngine_Start(engine_, ImGui::GetCurrentContext());
    ImGuiTest* test = ImGuiTestEngine_RegisterTest(engine_, "storyboard", "recipe");
    test->TestFunc = &Runner::run_items;
    test->UserData = this;
    ImGuiTestEngine_QueueTest(engine_, test);
}

Script::Ask
Runner::before_tick() {
    asked_ = {.open = open_,
              .picture = shot_.has_value(),
              .done = !ok_
                      || (ImGuiTestEngine_IsTestQueueEmpty(engine_) && !ImGuiTestEngine_GetIO(engine_).IsRunningTests)};
    return asked_;
}

// A step's ask is done by the tick after the one it was made in, which this
// tick's ask says: a step runs inside a tick, after its ask was taken.
void
Runner::after_tick(Outcome outcome) {
    ImGuiTestEngine_PostSwap(engine_);
    if (asked_.open) {
        opened_ = outcome.opened;
        open_.reset();
    }
    if (asked_.picture) {
        shot_why_.clear();
        if (!outcome.picture) {
            shot_why_ = "the window could not be read back";
        } else if (write_picture(*outcome.picture, shot_->crop, shot_->out, &shot_why_)) {
            std::printf("%s\n", utf8(shot_->out).c_str());
        }
        shot_.reset();
    }
}

void
Runner::stop() {
    ImGuiTestEngine_Stop(engine_);
    ImGuiTestEngineResultSummary summary;
    ImGuiTestEngine_GetResultSummary(engine_, &summary);
    ok_ &= summary.CountTested > 0 && summary.CountSuccess == summary.CountTested;
}

void
Runner::run_items(ImGuiTestContext* ctx) {
    Runner& self = *static_cast<Runner*>(ctx->Test->UserData);
    for (const Item& item : self.items_) {
        if (!item.ours()) {
            std::printf("%s:%d: %s is autana's, skipped\n", self.recipe_.c_str(), item.line, item.label().c_str());
            continue;
        }
        std::string why;
        if (!self.step(ctx, item, &why) || ctx->IsError()) {
            std::fprintf(stderr, "loupe-storyboard: %s:%d: %s: %s\n", self.recipe_.c_str(), item.line,
                         item.label().c_str(), why.empty() ? "failed, as the test engine says above" : why.c_str());
            self.ok_ = false;
            return;
        }
    }
}

bool
Runner::step(ImGuiTestContext* ctx, const Item& item, std::string* why) {
    const std::string key = *item.ours();
    const nlohmann::json& value = item.raw[key];
    const std::string word = value.is_string() ? value.get<std::string>() : "";
    if (key == OPEN) {
        return open(ctx, value, why);
    }
    if (key == SCREENSHOT) {
        return screenshot(ctx, value, why);
    }
    if (key == WAIT) {
        const std::optional<int> ticks = count(value, 1);
        if (!ticks) {
            *why = "loupe.wait needs a number of ticks";
            return false;
        }
        ctx->Yield(*ticks);
        return true;
    }
    if (key == CLICK || key == CHECK || key == UNCHECK) {
        if (word.empty()) {
            *why = std::format("{} needs an item, such as //Outliner/tour", key);
            return false;
        }
        if (key == CLICK) {
            ctx->ItemClick(word.c_str());
        } else if (key == CHECK) {
            ctx->ItemCheck(word.c_str());
        } else {
            ctx->ItemUncheck(word.c_str());
        }
        return true;
    }
    if (key == INPUT) {
        const std::string ref = member(value, "ref");
        const std::string typed = member(value, "value");
        if (ref.empty() || typed.empty()) {
            *why = "loupe.input needs {ref: ITEM, value: TEXT}";
            return false;
        }
        ctx->ItemInputValue(ref.c_str(), typed.c_str());
        return true;
    }
    if (key == KEY || key == HOLD) {
        const std::string name = key == KEY ? word : member(value, "key");
        const std::optional<ImGuiKeyChord> keys = chord(name);
        if (!keys) {
            *why = std::format("no key \"{}\": a chord of ImGui's key names, such as ctrl+z or Escape", name);
            return false;
        }
        if (key == KEY) {
            ctx->KeyPress(*keys);
            return true;
        }
        const std::optional<int> ticks = count(value.is_object() ? value.value("ticks", nlohmann::json()) : value, 1);
        if (!ticks) {
            *why = "loupe.hold needs {key: KEY, ticks: N}";
            return false;
        }
        ctx->KeyDown(*keys);
        ctx->Yield(*ticks);
        ctx->KeyUp(*keys);
        return true;
    }
    *why = std::format("no step {}", key);
    return false;
}

bool
Runner::open(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    std::vector<std::string> args = words(value);
    std::vector<char*> argv{const_cast<char*>("loupe")};
    for (std::string& arg : args) {
        argv.push_back(arg.data());
    }
    Options options;
    if (!parse_options(static_cast<int>(argv.size()), argv.data(), &options, why) || options.scene.empty()) {
        *why = std::format("loupe.open takes a scene and the window's flags, as loupe does: {}",
                           why->empty() ? "no scene" : *why);
        return false;
    }
    if (options.scene.is_relative()) {
        options.scene = recipe_dir_ / options.scene;
    }
    open_ = std::move(options);
    opened_ = false;
    await(ctx);
    if (!opened_) {
        *why = "the scene did not open, as said above";
        return false;
    }
    ctx->Yield(SETTLE_TICKS);
    return true;
}

bool
Runner::screenshot(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    Shot shot{.out = from_utf8(value.is_string() ? value.get<std::string>() : member(value, "out")),
              .crop = member(value, "crop")};
    if (shot.out.extension() != ".png") {
        *why = "loupe.screenshot needs OUT.png, or {out: OUT.png, crop: PANEL}";
        return false;
    }
    shot.out = out_dir_ / shot.out;
    std::error_code error;
    std::filesystem::create_directories(shot.out.parent_path(), error);
    ctx->MouseTeleportToPos(OFF_WINDOW);
    ctx->Yield(SETTLE_TICKS);
    shot_ = std::move(shot);
    await(ctx);
    *why = shot_why_;
    return shot_why_.empty();
}

void
Runner::await(ImGuiTestContext* ctx) {
    while (open_ || shot_) {
        ctx->Yield();
    }
}

bool
write_picture(const window::Picture& picture, const std::string& crop, const std::filesystem::path& out,
              std::string* why) {
    const std::optional<ImRect> rect = picture_rect(picture, crop, why);
    if (!rect) {
        return false;
    }
    const auto x = static_cast<int>(rect->Min.x), y = static_cast<int>(rect->Min.y);
    const auto width = static_cast<int>(rect->GetWidth()), height = static_cast<int>(rect->GetHeight());
    if (width <= 0 || height <= 0) {
        *why = std::format("panel {} is not on the picture", crop);
        return false;
    }
    const int pitch = picture.width * window::Picture::CHANNELS;
    // The surface only points into the picture: a const view SDL does not write through.
    auto* first = const_cast<uint8_t*>(picture.rgba.data()) + (static_cast<ptrdiff_t>(y) * pitch)
                  + (static_cast<ptrdiff_t>(x) * window::Picture::CHANNELS);
    const std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface(
        SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, first, pitch), &SDL_DestroySurface);
    if (surface == nullptr || !SDL_SavePNG(surface.get(), utf8(out).c_str())) {
        *why = std::format("cannot write {}: {}", utf8(out), SDL_GetError());
        return false;
    }
    return true;
}

} // namespace loupe::storyboard
