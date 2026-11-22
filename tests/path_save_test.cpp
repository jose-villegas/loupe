// Saving a camera path through autana's scene_edit.py set-keys, on a copy of
// Sponza's tour.keys.toml with a comment added: a moved eye, an added key and
// a deleted key change only their own lines, the comment stays, the file reads
// back as the document, and a path that would not build is refused with the
// file left as it was. Run as `path_save_test COPY/tour.keys.toml`.
#include <algorithm>
#include <cstdio>
#include <format>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "checks.hpp"
#include "editor/camera_path.hpp"
#include "editor/scene_tool.hpp"
#include "scratch.hpp"

namespace {

constexpr const char TEST[] = "path_save_test";
constexpr const char COMMENT[] = "# Up to the gallery.";
constexpr const char NEXT_COMMENT[] = "# Past the arches.";
// The keys the comments sit above, the key moved, and the key deleted: tour's keys at 25 s, 10 s, 5 s and 15 s.
// The next comment's key lands where the added one was after the first save, so an undo then a save must
// not hand its table to the added key's place.
constexpr size_t COMMENTED = 5, NEXT_COMMENTED = 2, MOVED = 1, DELETED = 3;
constexpr float MOVE = 10.0F;
constexpr float ADDED_AT_S = 7.5F;
// The lines of a table from its t on: t, eye, look_at and the blank after.
constexpr std::ptrdiff_t TABLE_TAIL = 4;

constexpr loupe::test::Checks check{TEST};

using loupe::test::read_file;

using loupe::test::write_file;

std::vector<std::string>
lines(const std::string& text) {
    std::vector<std::string> out;
    std::istringstream in(text);
    for (std::string line; std::getline(in, line);) {
        out.push_back(line);
    }
    return out;
}

// `text` with `comment` on the line above its `index`th [[keys]] header.
std::string
with_comment(const std::string& text, size_t index, const char* comment) {
    size_t at = 0;
    for (size_t found = 0; found <= index; found++) {
        at = text.find("[[keys]]", found == 0 ? 0 : at + 1);
    }
    return text.substr(0, at) + comment + "\n" + text.substr(at);
}

// Whether `comment` is in `lines` once, above the table whose t is `t`.
bool
above(const std::vector<std::string>& lines, const char* comment, float t) {
    const auto at = std::ranges::find(lines, std::string(comment));
    return std::ranges::count(lines, std::string(comment)) == 1 && lines.end() - at > 2
           && *(at + 2) == std::format("t = {}", t);
}

// Whether `part` appears in `whole` in order, other lines between.
bool
in_order(const std::vector<std::string>& part, const std::vector<std::string>& whole) {
    auto at = whole.begin();
    for (const std::string& line : part) {
        at = std::find(at, whole.end(), line);
        if (at == whole.end()) {
            std::fprintf(stderr, "%s: not kept in order: %s\n", TEST, line.c_str());
            return false;
        }
        ++at;
    }
    return true;
}

bool
same_keys(const std::vector<loupe::editor::CameraKey>& a, const std::vector<loupe::editor::CameraKey>& b) {
    return std::ranges::equal(
        a, b, [](const auto& x, const auto& y) { return x.t == y.t && x.eye == y.eye && x.look_at == y.look_at; });
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s COPY/tour.keys.toml\n", TEST);
        return 2;
    }
    const char* keys_toml = argv[1];
    write_file(keys_toml,
               with_comment(with_comment(read_file(keys_toml), COMMENTED, COMMENT), NEXT_COMMENTED, NEXT_COMMENT));
    const std::string before = read_file(keys_toml);
    const loupe::Scratch logs("path_save_test-");
    std::string why;
    std::optional<loupe::editor::CameraPath> path = loupe::editor::read_path(keys_toml, logs.path(), &why);
    if (!check(path.has_value() && path->keys.size() > COMMENTED, "the keys read: " + why) || !path) {
        return 1;
    }
    const std::vector<loupe::editor::CameraKey> read = path->keys;
    const std::string moved_old =
        std::format("eye = [{}, {}, {}]", read[MOVED].eye[0], read[MOVED].eye[1], read[MOVED].eye[2]);
    loupe::editor::CameraKey moved = read[MOVED];
    moved.eye[0] += MOVE;
    std::vector<loupe::editor::CameraKey> keys = loupe::editor::with_key(read, MOVED, moved);
    size_t added = 0;
    keys = loupe::editor::with_key_added(keys, ADDED_AT_S, &added).value_or(keys);
    const size_t deleted = DELETED + (added <= DELETED ? 1 : 0);
    bool ok = check(added == MOVED + 1 && !loupe::editor::delete_refusal(keys, deleted),
                    "a key is added after the moved one, and the deleted one may go");
    const std::vector<loupe::editor::CameraKey> undeleted = keys;
    keys.erase(keys.begin() + static_cast<std::ptrdiff_t>(deleted));
    path->keys = keys;

    // A path that would not build is refused, the file untouched and the document still unsaved.
    loupe::editor::CameraPath faulty = *path;
    faulty.keys[2].t = faulty.keys[1].t;
    ok &= check(!loupe::editor::save_path(faulty, logs.path(), &why) && read_file(keys_toml) == before
                    && faulty.unsaved(),
                "a path that would not build is refused, the file as it was");

    why.clear();
    ok &= check(loupe::editor::save_path(*path, logs.path(), &why) && !path->unsaved(), "the save writes " + why);
    const std::optional<loupe::editor::CameraPath> back = loupe::editor::read_path(keys_toml, logs.path(), &why);
    ok &= check(back && same_keys(back->keys, path->keys), "the file reads back as the document");

    const std::vector<std::string> old_lines = lines(before), new_lines = lines(read_file(keys_toml));
    const std::string deleted_t = std::format("t = {}", read[DELETED].t);
    // The deleted key's table: its header, its three lines and the blank after them.
    const auto deleted_at = std::ranges::find(old_lines, deleted_t) - old_lines.begin();
    std::vector<std::string> kept;
    for (std::ptrdiff_t i = 0; i < std::ssize(old_lines); i++) {
        if (old_lines[static_cast<size_t>(i)] != moved_old && (i < deleted_at - 1 || i >= deleted_at + TABLE_TAIL)) {
            kept.push_back(old_lines[static_cast<size_t>(i)]);
        }
    }
    ok &= check(in_order(kept, new_lines), "every line but the moved and deleted ones stays, in order");
    ok &= check(std::ranges::count(new_lines, std::format("t = {}", ADDED_AT_S)) == 1,
                "the added key is written at its millisecond, not a float step past it");
    ok &= check(above(new_lines, COMMENT, read[COMMENTED].t) && above(new_lines, NEXT_COMMENT, read[NEXT_COMMENTED].t),
                "each comment stays above its own key");
    ok &= check(std::ranges::count(new_lines, deleted_t) == 0 && std::ranges::count(new_lines, moved_old) == 0,
                "the deleted key and the moved eye's old line are gone");
    ok &= check(new_lines.size() == old_lines.size(), "a table added and a table deleted: as many lines");

    // The delete undone after the save, as the undo stack gives its keys back, and saved again.
    path->keys = undeleted;
    ok &= check(path->unsaved() && loupe::editor::save_path(*path, logs.path(), &why) && !path->unsaved(),
                "the delete undone after a save saves again: " + why);
    const std::optional<loupe::editor::CameraPath> undone = loupe::editor::read_path(keys_toml, logs.path(), &why);
    const std::vector<std::string> undone_lines = lines(read_file(keys_toml));
    ok &= check(undone && same_keys(undone->keys, undeleted), "the file reads back as the undone document");
    ok &= check(above(undone_lines, COMMENT, read[COMMENTED].t)
                    && above(undone_lines, NEXT_COMMENT, read[NEXT_COMMENTED].t),
                "each comment is still above its own key");
    path->keys = keys;
    ok &= check(path->unsaved(), "a redo after that save is unsaved again");
    return ok ? 0 : 1;
}
