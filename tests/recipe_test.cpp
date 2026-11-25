// A recipe reads as its steps: each map with the line it starts on, the
// loupe.* ones Loupe's, the rest autana's; a recipe that is not a list of maps,
// or a step with two loupe.* keys, is refused at its line.
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
                                                                "- loupe.hold: {key: S, ticks: 60}\n"
                                                                "- command: sand pour\n"
                                                                "  expect: poured\n",
                                                                "r.yaml", &why);
    bool ok = check(items && items->size() == 3, "three steps read");
    if (!items || items->size() != 3) {
        return false;
    }
    const Item& open = (*items)[0];
    ok &= check(open.line == 2 && open.ours() == "loupe.open" && open.label() == "The scene",
                "a named loupe step: its line, its key, its name");
    ok &= check(open.raw["loupe.open"] == "scene.toml --dt 100", "a step's value as written");
    const Item& hold = (*items)[1];
    ok &= check(hold.line == 4 && hold.label() == "loupe.hold" && hold.raw["loupe.hold"]["ticks"] == "60",
                "an unnamed step is labelled by its key, its map's scalars as strings");
    const Item& autana = (*items)[2];
    ok &= check(!autana.ours() && autana.line == 5, "a step without a loupe.* key is autana's");
    return ok;
}

bool
refuses(const std::string& text, const std::string& refusal, const std::string& what) {
    std::string why;
    const bool refused = !parse_recipe(text, "r.yaml", &why) && why.starts_with(refusal);
    if (!refused) {
        std::fprintf(stderr, "%s: got \"%s\"\n", TEST, why.c_str());
    }
    return check(refused, what);
}

} // namespace

int
main() {
    bool ok = reads_steps();
    ok &= refuses("loupe.wait: 4\n", "r.yaml: a recipe is a list of steps", "a map, not a list, refused");
    ok &= refuses("- loupe.wait: 4\n- just words\n", "r.yaml:2: a step is a map",
                  "a step that is no map refused at its line");
    ok &= refuses("- loupe.click: a\n  loupe.wait: 4\n", "r.yaml:1: one loupe.* key per step",
                  "two loupe.* keys in one step refused");
    ok &= refuses("- loupe.click: [a\n", "r.yaml:", "YAML that does not parse refused at a line");
    return ok ? 0 : 1;
}
