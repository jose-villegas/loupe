// Saving through autana's scene_edit.py, on a copy of the capybara scene:
// moving and scaling the skinned capybara adds exactly its two lines to the
// file, the read sees them, and a baked renderer is refused with the file left
// as it was. Run as `scene_save_test COPY/capybara.scene.toml`.
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "editor/doc.hpp"
#include "editor/scene_tool.hpp"
#include "scratch.hpp"

namespace {

constexpr const char TEST[] = "scene_save_test";

bool
check(bool ok, const std::string& what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what.c_str(), ok ? "ok" : "FAILED");
    return ok;
}

std::string
read_file(const char* path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

std::vector<std::string>
lines(const std::string& text) {
    std::vector<std::string> out;
    std::istringstream in(text);
    for (std::string line; std::getline(in, line);) {
        out.push_back(line);
    }
    return out;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s COPY/capybara.scene.toml\n", TEST);
        return 2;
    }
    const char* scene = argv[1];
    const std::string before = read_file(scene);
    const loupe::Scratch logs("scene_save_test-");
    std::string why;
    std::optional<loupe::editor::SceneDoc> doc = loupe::editor::read_scene(scene, logs.path(), &why);
    if (!check(doc.has_value(), "the scene reads: " + why)) {
        return 1;
    }
    loupe::editor::Docs docs{.scene = std::move(*doc)};
    const loupe::editor::Object* capybara = docs.scene.find("capybara");
    const loupe::editor::Object* meadow = docs.scene.find("meadow");
    bool ok = check(capybara != nullptr && loupe::editor::placeable(*capybara) && meadow != nullptr && meadow->baked,
                    "the capybara places; the meadow is baked");
    if (!ok) {
        return 1;
    }
    loupe::editor::Placement moved = capybara->local;
    moved.position = {0.5F, 0.0F, -0.25F};
    moved.scale = {1.5F, 1.5F, 1.5F};
    loupe::editor::EditStack edits;
    edits.push(std::make_unique<loupe::editor::Place>("capybara",
                                                      loupe::editor::Move{.before = capybara->local, .after = moved}),
               docs);
    ok &= check(loupe::editor::save_scene(docs.scene, logs.path(), &why) && !docs.scene.unsaved(),
                "the save writes, and nothing is left unsaved: " + why);
    const std::vector<std::string> old_lines = lines(before), new_lines = lines(read_file(scene));
    std::vector<std::string> added, kept;
    for (const std::string& line : new_lines) {
        (std::ranges::find(old_lines, line) == old_lines.end() ? added : kept).push_back(line);
    }
    std::ranges::sort(added);
    ok &= check(kept == old_lines
                    && added == std::vector<std::string>{"position = [0.5, 0.0, -0.25]", "scale = [1.5, 1.5, 1.5]"},
                "the file gains the capybara's two lines and nothing else changes");
    std::optional<loupe::editor::SceneDoc> again = loupe::editor::read_scene(scene, logs.path(), &why);
    ok &= check(again && again->find("capybara")->local == moved, "the read sees the saved placement");
    const std::string saved = read_file(scene);
    docs.scene.find("meadow")->local.position = {1.0F, 0.0F, 0.0F};
    ok &= check(!loupe::editor::save_scene(docs.scene, logs.path(), &why) && why.find("baked") != std::string::npos,
                "a baked renderer is refused, saying why");
    ok &= check(read_file(scene) == saved && docs.scene.unsaved(),
                "a refused save leaves the file as it was, and the change still unsaved");
    return ok ? 0 : 1;
}
