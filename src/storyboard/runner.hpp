// Runs a recipe's loupe.* steps on the real window: Dear ImGui's test engine
// gives each step's clicks, keys and typing to ImGui as a person's input, and
// Loupe's own panels act on them. A step that opens a scene or reads a picture
// asks the window between ticks (run_scripted).
#pragma once

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <variant>

#include "options.hpp"
#include "storyboard/recipe.hpp"
#include "window/run_window.hpp"

struct ImGuiTestEngine;
struct ImGuiTestContext;

namespace loupe::storyboard {

// The time one tick stands for, whatever the machine: a held key moves the
// same distance anywhere, and a recording's frames come at its rate.
constexpr float TICK_S = 1.0F / 30.0F;
// Ticks a step waits before its picture is read, and after its scene opens:
// the panels dock on the first, and a seek, a fly or an upload lands on the next.
constexpr int SETTLE_TICKS = 4;

class Runner final : public Script {
  public:
    // `recipe`'s steps, each ${NAME} in them one of `names`, its pictures and
    // autana's output written under `out_dir`; loupe.autana runs inside the
    // lock `lock_token` names when it is not empty.
    Runner(Recipe recipe, std::filesystem::path out_dir, std::map<std::string, std::string> names = {},
           std::string lock_token = "");
    Runner(const Runner&) = delete;
    Runner& operator=(const Runner&) = delete;
    Runner(Runner&&) = delete;
    Runner& operator=(Runner&&) = delete;
    ~Runner() override;

    void start() override;
    Ask before_tick() override;
    void after_tick(const Ask& asked, Outcome outcome) override;
    void stop() override;

    // Whether every step ran.
    bool ok() const;

  private:
    // A picture a step asks the window for.
    struct Shot {
        std::filesystem::path out;
        std::string crop;
    };

    static void run_steps(ImGuiTestContext* ctx);
    // Runs `item`; false, with `why` set, when it failed.
    bool step(ImGuiTestContext* ctx, const Item& item, std::string* why);
    bool open(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why);
    bool screenshot(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why);
    // N ticks; or, by the wall clock, until checks hold or as long as they do.
    bool wait(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why);
    // Ticks until `checks` hold (`until`) or as long as they do, for `seconds`.
    bool watch(ImGuiTestContext* ctx, const nlohmann::json& checks, bool until, int seconds, std::string* why);
    // Runs Loupe's autana, again until its output matches when asked to.
    bool autana(ImGuiTestContext* ctx, const nlohmann::json& value, std::string* why);
    // Asks the window for `request` and yields ticks until it is done; why it
    // failed, or empty when it was done.
    std::string request(ImGuiTestContext* ctx, std::variant<Options, Shot> request);

    Recipe recipe_;
    std::filesystem::path out_dir_;
    ImGuiTestEngine* engine_ = nullptr;
    // Asked by a step, done by the window between ticks, and why it failed.
    std::optional<std::variant<Options, Shot>> pending_;
    std::string pending_why_;
    std::map<std::string, std::string> names_;
    std::string lock_token_;
    // What the last tick left on screen.
    Observed observed_;
    int autana_runs_ = 0;
    bool ok_ = true;
};

// `picture` as a PNG at `out`, cropped to the ImGui window `crop` names where
// the last frame laid it out, or whole when `crop` is empty; false with `why`
// set when that window is not on the picture or the file cannot be written.
bool write_picture(const window::Picture& picture, const std::string& crop, const std::filesystem::path& out,
                   std::string* why);

} // namespace loupe::storyboard
