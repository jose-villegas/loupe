// ImVec2's arithmetic, for the crop.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "storyboard/runner.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <future>
#include <memory>
#include <regex>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

#include "board/boards.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_te_context.h"
#include "imgui_te_coroutine.h"
#include "imgui_te_engine.h"
#include "paths.hpp"
#include "process.hpp"
#include "storyboard/check.hpp"

namespace loupe::storyboard {

namespace {

constexpr const char OPEN[] = "loupe.open";
constexpr const char SCREENSHOT[] = "loupe.screenshot";
constexpr const char WAIT[] = "loupe.wait";
constexpr const char AUTANA[] = "loupe.autana";
// How long loupe.autana tries again for its `until` when the step gives no timeout_s.
constexpr int AUTANA_UNTIL_S = 60;
// How long loupe.autana waits between tries.
constexpr auto AUTANA_AGAIN = std::chrono::seconds(1);
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

// The command line's words in a loupe.open: a string split at white space, or a list.
std::vector<std::string>
open_words(const nlohmann::json& value) {
    if (value.is_string()) {
        return words(value.get<std::string>());
    }
    std::vector<std::string> out;
    if (value.is_array()) {
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

namespace {

// A step that is input alone, needing nothing of the window between ticks.
struct InputStep {
    std::string_view key;
    bool (*run)(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why);
};

// The item a click, check or uncheck names; empty, with `why` set, for none
// or a disabled one: a person's click there does nothing either.
std::string
item_ref(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    if (!value.is_string() || value.get<std::string>().empty()) {
        *why = "the step needs an item, such as //Outliner/tour";
        return "";
    }
    std::string ref = value.get<std::string>();
    if ((ctx->ItemInfo(ref.c_str()).ItemFlags & ImGuiItemFlags_Disabled) != 0) {
        *why = std::format("{} is disabled", ref);
        return "";
    }
    return ref;
}

bool
click(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    const std::string ref = item_ref(ctx, value, why);
    if (!ref.empty()) {
        ctx->ItemClick(ref.c_str());
    }
    return !ref.empty();
}

// Ticks the checkbox `value` names, or clears it when `on` is false.
bool
tick_box(ImGuiTestContext* ctx, const nlohmann::json& value, bool on, std::string* why) {
    const std::string ref = item_ref(ctx, value, why);
    if (ref.empty()) {
        return false;
    }
    if (on) {
        ctx->ItemCheck(ref.c_str());
    } else {
        ctx->ItemUncheck(ref.c_str());
    }
    return true;
}

bool
check(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    return tick_box(ctx, value, true, why);
}

bool
uncheck(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    return tick_box(ctx, value, false, why);
}

bool
input(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    const std::string ref = member(value, "ref");
    const std::string typed = member(value, "value");
    if (ref.empty() || typed.empty()) {
        *why = "loupe.input needs {ref: ITEM, value: TEXT}";
        return false;
    }
    ctx->ItemInputValue(ref.c_str(), typed.c_str());
    return true;
}

// `name` as a chord; none, with `why` set, for a name that is none.
std::optional<ImGuiKeyChord>
named_chord(const std::string& name, std::string* why) {
    const std::optional<ImGuiKeyChord> keys = chord(name);
    if (!keys) {
        *why = std::format("no key \"{}\": a chord of ImGui's key names, such as ctrl+z or Escape", name);
    }
    return keys;
}

bool
key(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    const std::optional<ImGuiKeyChord> keys = named_chord(value.is_string() ? value.get<std::string>() : "", why);
    if (keys) {
        ctx->KeyPress(*keys);
    }
    return keys.has_value();
}

bool
hold(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    const std::optional<ImGuiKeyChord> keys = named_chord(member(value, "key"), why);
    const std::optional<int> ticks = count(value.is_object() ? value.value("ticks", nlohmann::json()) : value, 1);
    if (!keys || !ticks) {
        *why = "loupe.hold needs {key: KEY, ticks: N}: " + *why;
        return false;
    }
    ctx->KeyDown(*keys);
    ctx->Yield(*ticks);
    ctx->KeyUp(*keys);
    return true;
}

constexpr std::array INPUT_STEPS = {
    InputStep{.key = "loupe.click", .run = click},     InputStep{.key = "loupe.check", .run = check},
    InputStep{.key = "loupe.uncheck", .run = uncheck}, InputStep{.key = "loupe.input", .run = input},
    InputStep{.key = "loupe.key", .run = key},         InputStep{.key = "loupe.hold", .run = hold},
};

// Ticks for `span` by the wall clock.
void
ticking_for(ImGuiTestContext* ctx, std::chrono::steady_clock::duration span) {
    const auto end = std::chrono::steady_clock::now() + span;
    while (std::chrono::steady_clock::now() < end) {
        ctx->Yield();
    }
}

// Runs `command` with its output in `log` while the window ticks on; whether
// it exited 0.
bool
ticking_through(ImGuiTestContext* ctx, const std::vector<std::string>& command, const std::filesystem::path& log) {
    std::future<bool> run = std::async(std::launch::async, [&command, &log] { return run_logged(command, log); });
    while (run.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        ctx->Yield();
    }
    return run.get();
}

} // namespace

Runner::Runner(Recipe recipe, std::filesystem::path out_dir, std::map<std::string, std::string> names,
               std::string lock_token)
    : recipe_(std::move(recipe)), out_dir_(std::move(out_dir)), names_(std::move(names)),
      lock_token_(std::move(lock_token)) {}

// After ImGui's own context is gone (run_scripted's window), as the engine asks.
Runner::~Runner() {
    if (engine_ != nullptr) {
        ImGuiTestEngine_DestroyContext(engine_);
    }
}

bool
Runner::ok() const {
    return ok_;
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
    test->TestFunc = &Runner::run_steps;
    test->UserData = this;
    ImGuiTestEngine_QueueTest(engine_, test);
}

Script::Ask
Runner::before_tick() {
    Ask ask;
    ask.done = !ok_ || (ImGuiTestEngine_IsTestQueueEmpty(engine_) && !ImGuiTestEngine_GetIO(engine_).IsRunningTests);
    if (pending_) {
        if (const Options* open = std::get_if<Options>(&*pending_)) {
            ask.open = *open;
        } else {
            ask.picture = true;
        }
    }
    return ask;
}

// A step's request is done by the tick after the one it was made in, which
// `asked` carries: a step runs inside a tick, after that tick's ask was taken.
void
Runner::after_tick(const Ask& asked, Outcome outcome) {
    ImGuiTestEngine_PostSwap(engine_);
    observed_ = std::move(outcome.observed);
    if (asked.open) {
        pending_why_ = outcome.opened ? "" : "the scene did not open, as said above";
        pending_.reset();
        return;
    }
    const Shot* shot = pending_ ? std::get_if<Shot>(&*pending_) : nullptr;
    if (!asked.picture || shot == nullptr) {
        return;
    }
    pending_why_ = "the window could not be read back";
    if (outcome.picture && write_picture(*outcome.picture, shot->crop, shot->out, &pending_why_)) {
        pending_why_.clear();
        std::puts(utf8(shot->out).c_str());
    }
    pending_.reset();
}

// The test aborted first, so a step waiting on the window, which will not
// tick again, gives up instead of holding the engine's stop.
void
Runner::stop() {
    ImGuiTestEngine_AbortCurrentTest(engine_);
    ImGuiTestEngine_Stop(engine_);
    ImGuiTestEngineResultSummary summary;
    ImGuiTestEngine_GetResultSummary(engine_, &summary);
    ok_ &= summary.CountTested > 0 && summary.CountSuccess == summary.CountTested;
}

void
Runner::run_steps(ImGuiTestContext* ctx) {
    Runner& self = *static_cast<Runner*>(ctx->Test->UserData);
    const std::string recipe = utf8(self.recipe_.path);
    for (const Item& item : self.recipe_.steps) {
        std::string why;
        const auto began = std::chrono::steady_clock::now();
        if (!self.step(ctx, item, &why) || ctx->IsError()) {
            std::fprintf(stderr, "loupe-storyboard: %s:%d: %s: %s\n", recipe.c_str(), item.line, item.label().c_str(),
                         why.empty() ? "failed, as the test engine says above" : why.c_str());
            self.ok_ = false;
            return;
        }
        const std::chrono::duration<double> took = std::chrono::steady_clock::now() - began;
        std::printf("loupe-storyboard: %s:%d: %s: %.3f s\n", recipe.c_str(), item.line, item.label().c_str(),
                    took.count());
    }
}

bool
Runner::step(ImGuiTestContext* ctx, const Item& item, std::string* why) {
    const std::optional<nlohmann::json> raw = substituted(item.raw, names_, why);
    if (!raw) {
        return false;
    }
    const nlohmann::json& value = (*raw)[item.key];
    bool ran = false;
    if (item.key == OPEN) {
        ran = open(ctx, value, why);
    } else if (item.key == SCREENSHOT) {
        ran = screenshot(ctx, value, why);
    } else if (item.key == WAIT) {
        ran = wait(ctx, value, why);
    } else if (item.key == AUTANA) {
        ran = autana(ctx, value, why);
    } else if (const auto* found = std::ranges::find(INPUT_STEPS, item.key, &InputStep::key);
               found != INPUT_STEPS.end()) {
        ran = found->run(ctx, value, why);
    } else {
        *why = std::format("no step {}", item.key);
    }
    if (ran && raw->contains(CHECK_KEY)) {
        // The window has drawn what the step did.
        ctx->Yield();
        ran = checks_hold(observed_, (*raw)[CHECK_KEY], why);
        if (!ran) {
            *why += "; seen " + observed_fields(observed_).dump();
        }
    }
    return ran;
}

// N ticks; {until: CHECKS, timeout_s: S}, the checks holding within S s; or
// {while: CHECKS, for_s: S}, the checks holding on every tick for S s.
bool
Runner::wait(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    if (!value.is_object()) {
        const std::optional<int> ticks = count(value, 1);
        if (!ticks) {
            *why = "loupe.wait needs a number of ticks, {until: CHECKS, timeout_s: S} or {while: CHECKS, for_s: S}";
            return false;
        }
        ctx->Yield(*ticks);
        return true;
    }
    const bool until = value.contains("until");
    const char* checks_key = until ? "until" : "while";
    const char* seconds_key = until ? "timeout_s" : "for_s";
    const std::optional<int> seconds = count(value.value(seconds_key, nlohmann::json()), 1);
    if (!value.contains(checks_key) || !seconds) {
        *why = std::format("loupe.wait {{{}: CHECKS}} needs {}: S, whole seconds", checks_key, seconds_key);
        return false;
    }
    return watch(ctx, value[checks_key], until, *seconds, why);
}

bool
Runner::watch(ImGuiTestContext* ctx, const nlohmann::json& checks, bool until, int seconds, std::string* why) {
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    for (;;) {
        ctx->Yield();
        std::string failed;
        const bool hold = checks_hold(observed_, checks, &failed);
        if (hold == until) {
            if (!until) {
                *why = std::format("stopped holding before {} s: {}; seen {}", seconds, failed,
                                   observed_fields(observed_).dump());
            }
            return until;
        }
        if (std::chrono::steady_clock::now() >= end) {
            if (until) {
                *why = std::format("not within {} s: {}; seen {}", seconds, failed, observed_fields(observed_).dump());
            }
            return !until;
        }
        if (ctx->IsError()) {
            *why = "the run stopped first";
            return false;
        }
    }
}

// ARGS, split at white space, or a list of words; or {args: ARGS, until:
// REGEX, timeout_s: S}, run again each AUTANA_AGAIN until what it printed
// matches. Each run's output goes to autana-N.log under -o; a run that does
// not exit 0 fails the step.
bool
Runner::autana(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    const nlohmann::json args = value.is_object() ? value.value("args", nlohmann::json()) : value;
    std::vector<std::string> words = open_words(args);
    if (words.empty()) {
        *why = "loupe.autana needs autana's arguments, or {args: ARGS, until: REGEX, timeout_s: S}";
        return false;
    }
    if (!lock_token_.empty()) {
        words.insert(words.begin(), {"--lock-token", lock_token_});
    }
    const std::vector<std::string> command = autana_command(words);
    const std::string until = member(value, "until");
    const std::optional<int> timeout =
        value.is_object() && value.contains("timeout_s") ? count(value["timeout_s"], 1) : AUTANA_UNTIL_S;
    if (!timeout) {
        *why = "loupe.autana's timeout_s is whole seconds";
        return false;
    }
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(*timeout);
    std::error_code error;
    std::filesystem::create_directories(out_dir_, error);
    for (;;) {
        const std::filesystem::path log = out_dir_ / std::format("autana-{:02}.log", ++autana_runs_);
        if (!ticking_through(ctx, command, log)) {
            *why = std::format("autana {} did not exit 0; its output is in {}", args.dump(), utf8(log));
            return false;
        }
        if (until.empty() || std::regex_search(read_text(log), std::regex(until))) {
            return true;
        }
        if (std::chrono::steady_clock::now() >= end) {
            *why = std::format("autana {} never printed {}; its last output is in {}", args.dump(), until, utf8(log));
            return false;
        }
        ticking_for(ctx, AUTANA_AGAIN);
    }
}

bool
Runner::open(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why) {
    Options options;
    if (!parse_args(open_words(value), &options, why) || options.scene.empty()) {
        *why = std::format("loupe.open takes a scene and the window's flags, as loupe does: {}",
                           why->empty() ? "no scene" : *why);
        return false;
    }
    if (options.scene.is_relative()) {
        options.scene = recipe_.path.parent_path() / options.scene;
    }
    *why = request(ctx, std::move(options));
    if (!why->empty()) {
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
    *why = request(ctx, std::move(shot));
    return why->empty();
}

std::string
Runner::request(ImGuiTestContext* ctx, std::variant<Options, Shot> request) {
    pending_ = std::move(request);
    pending_why_.clear();
    while (pending_ && !ctx->IsError()) {
        ctx->Yield();
    }
    return pending_ ? "the run stopped first" : pending_why_;
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
