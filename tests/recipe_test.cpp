// A recipe reads as its steps, each map one loupe.* key and an optional name,
// with the line it starts on; a recipe that is not a list of such maps is
// refused at its line, so a misspelt step never passes unrun.
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

#include "checks.hpp"
#include "storyboard/recipe.hpp"

namespace {

constexpr const char TEST[] = "recipe_test";
constexpr loupe::test::Checks check{TEST};

using loupe::storyboard::Item;
using loupe::storyboard::parse_recipe;

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
    };
    bool ok = reads_steps();
    for (const Refusal& refusal : refusals) {
        ok &= refuses(refusal);
    }
    return ok ? 0 : 1;
}
