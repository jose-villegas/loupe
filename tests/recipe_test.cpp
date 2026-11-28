// A recipe reads as its steps, each map one loupe.* key and an optional name
// and check, with the line it starts on; a recipe that is not a list of such
// maps, or whose check cannot be checked, is refused at its line, so a
// misspelt step never passes unrun. A check compares what the window shows,
// and a recipe's ${NAME}s take the values given.
#include <cstdio>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "checks.hpp"
#include "storyboard/check.hpp"
#include "storyboard/recipe.hpp"

namespace {

constexpr const char TEST[] = "recipe_test";
constexpr loupe::test::Checks check{TEST};

using loupe::storyboard::checks_hold;
using loupe::storyboard::Item;
using loupe::storyboard::parse_recipe;
using loupe::storyboard::substituted;

bool
reads_steps() {
    std::string why;
    const std::optional<std::vector<Item>> items = parse_recipe("# a comment\n"
                                                                "- name: The scene\n"
                                                                "  loupe.open: scene.toml --dt 100\n"
                                                                "- loupe.hold: {key: S, ticks: 60}\n",
                                                                "r.yaml", &why);
    bool ok = check(items && items->size() == 2, "two steps read");
    if (!items || items->size() != 2) {
        return false;
    }
    const Item& open = (*items)[0];
    ok &= check(open.line == 2 && open.key == "loupe.open" && open.label() == "The scene",
                "a named step: its line, its key, its name");
    ok &= check(open.raw["loupe.open"] == "scene.toml --dt 100", "a step's value as written");
    const Item& hold = (*items)[1];
    ok &= check(hold.line == 4 && hold.label() == "loupe.hold" && hold.raw["loupe.hold"]["ticks"] == "60",
                "an unnamed step is labelled by its key, its map's scalars as strings");
    return ok;
}

// A step's check is read with it, as written.
bool
reads_a_check() {
    std::string why;
    const std::optional<std::vector<Item>> items =
        parse_recipe("- loupe.wait: 4\n  check: [{loupe.frames: ~, gt: 30}]\n", "r.yaml", &why);
    return check(items && items->size() == 1 && (*items)[0].key == "loupe.wait"
                     && (*items)[0].raw["check"][0]["gt"] == "30",
                 "a step's check is read beside its key");
}

// What a live view shows, as the window reports it.
loupe::Observed
seen() {
    loupe::Observed live;
    live.screen = "live";
    live.boards = {"90:70:69:FE:A3:08", "44:BD"};
    live.phase = "Shown";
    live.frames = 120;
    live.frame_age_s = 0.25;
    live.saving = "off";
    live.recording = "C:/r/907069FEA308-1.framerec";
    return live;
}

const loupe::Observed SEEN = seen();

// `checks`, as YAML gives them (every value a string), over SEEN.
bool
holds(const nlohmann::json& checks) {
    std::string why;
    return checks_hold(SEEN, checks, &why);
}

bool
compares() {
    bool ok = check(holds(R"([{"loupe.frames": null, "gt": "30"}, {"loupe.frame_age_s": null, "lt": "1"}])"_json),
                    "numbers compare as numbers");
    ok &= check(!holds(R"([{"loupe.frames": null, "gt": "200"}])"_json), "a number that is not so fails");
    ok &= check(holds(R"([{"loupe.phase": null, "eq": "Shown"}, {"loupe.refusal": null, "eq": ""}])"_json),
                "text compares by eq, the empty text too");
    ok &= check(holds(R"([{"loupe.boards": null, "match": "90:70:69:FE:A3:08"}])"_json),
                "a list matches as its items joined");
    ok &= check(holds(R"([{"loupe.recording": null, "match": "\\.framerec$"}])"_json), "match finds a pattern");
    ok &= check(!holds(R"([{"loupe.phase": null, "gt": "1"}])"_json), "text is no number to order");
    std::string why;
    ok &= check(!checks_hold(SEEN, R"([{"loupe.fps": null, "gt": "1"}])"_json, &why)
                    && (why.find("loupe.fps") != std::string::npos),
                "a field nothing observes fails, naming it");
    ok &=
        check(!checks_hold(
                  SEEN, R"([{"loupe.phase": null, "eq": "Shown"}, {"loupe.saving": null, "eq": "saving"}])"_json, &why)
                  && (why.find("\"off\"") != std::string::npos),
              "the check that fails says what was seen");
    return ok;
}

bool
substitutes() {
    const std::map<std::string, std::string> names = {{"BOARD", "90:70"}};
    std::string why;
    const std::optional<nlohmann::json> step =
        substituted(R"({"loupe.click": "//loupe/${BOARD}/View"})"_json, names, &why);
    bool ok = check(step && (*step)["loupe.click"] == "//loupe/90:70/View", "a ${NAME} takes its value");
    ok &= check(!substituted(R"(["${PORT}"])"_json, names, &why) && (why.find("PORT") != std::string::npos),
                "a ${NAME} not given fails, naming it");
    const std::string named = "- loupe.wait: 4\n  check: [{loupe.busy: ~, match: \"${BOARD} held by [^ ]\"}]\n";
    ok &= check(parse_recipe(named, "r.yaml", &why).has_value(),
                "a pattern with a ${NAME} waits for its value to compile");
    return ok;
}

// A recipe's text, the refusal it should start with, and what the check says.
struct Refusal {
    std::string text, starts, what;
};

bool
refuses(const Refusal& refusal) {
    std::string why;
    const bool refused = !parse_recipe(refusal.text, "r.yaml", &why) && why.starts_with(refusal.starts);
    if (!refused) {
        std::fprintf(stderr, "%s: got \"%s\"\n", TEST, why.c_str());
    }
    return check(refused, refusal.what);
}

} // namespace

int
main() {
    const std::string not_a_step = "r.yaml:2: a step is one loupe.* key";
    const std::vector<Refusal> refusals = {
        {"loupe.wait: 4\n", "r.yaml: a recipe is a list of steps", "a map, not a list, refused"},
        {"- loupe.wait: 4\n- just words\n", "r.yaml:2: a step is a map", "a step that is no map refused"},
        {"- loupe.wait: 4\n- loup.click: //Outliner/tour\n", not_a_step, "a misspelt key refused at its line"},
        {"- loupe.wait: 4\n- command: sand pour\n", not_a_step, "a key not Loupe's refused"},
        {"- loupe.wait: 4\n- name: only a name\n", not_a_step, "a step with no key refused"},
        {"- loupe.wait: 4\n- loupe.click: a\n  loupe.wait: 4\n", not_a_step, "two keys in one step refused"},
        {"- loupe.click: [a\n", "r.yaml:", "YAML that does not parse refused at a line"},
        {"- loupe.wait: 4\n  check: {loupe.frames: ~, gt: 3}\n", "r.yaml:1: a check is a list",
         "a check not a list refused"},
        {"- loupe.wait: 4\n  check: [{loupe.frames: ~, about: 3}]\n", "r.yaml:1: a check is one loupe.FIELD",
         "a check with no comparison refused"},
        {"- loupe.wait: 4\n  check: [{frames: ~, gt: 3}]\n", "r.yaml:1: a check is one loupe.FIELD",
         "a field not Loupe's refused"},
        {"- loupe.wait: 4\n  check: [{loupe.phase: ~, match: \"[\"}]\n", "r.yaml:1: match [",
         "a pattern that does not compile refused"},
    };
    bool ok = reads_steps();
    ok &= reads_a_check();
    ok &= compares();
    ok &= substitutes();
    for (const Refusal& refusal : refusals) {
        ok &= refuses(refusal);
    }
    return ok ? 0 : 1;
}
