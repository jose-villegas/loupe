// A frame record stream opens like a scene: a host run's --record, opened as
// a .framerec, draws every frame with each keyframe's CHECK the same, and a
// seek back draws the bytes the forward step drew. Fed live in pieces, its
// count grows as frames arrive and draws them alike; bytes that are no stream
// are refused, and so is a stream naming a pack its folder lacks, by name. Where
// loupe has the pack tool, its build made the pin's firmware packs, and they draw the stream.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "frames_fixture.hpp"
#include "pack.hpp"
#include "paths.hpp"
#include "read_bytes.hpp"
#include "tools.hpp"
#include "version.hpp"

namespace fs = std::filesystem;

namespace {

constexpr const char TEST[] = "stream_test";
// Past two keyframes at the recorder's default interval, so seeks start from several.
constexpr int FRAMES = 70;
// Backwards and across keyframes after the forward pass.
const int SEEKS[] = {45, 5, 61, 0, 30, 29};
// A piece of the live feed: smaller than most frames, so frames arrive split.
constexpr size_t PIECE = 1000;
// The recording, in the test's folder.
constexpr const char STREAM[] = "host.framerec";
// The pack the recorded scene names, and one only a board's firmware carries.
constexpr const char SCENE_PACK[] = "sponza";
constexpr const char FIRMWARE_ONLY_PACK[] = "bistro";

// Where the scene's packs are and where the test writes.
struct Folders {
    fs::path packs, out;
};

bool
expect(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

// Records `FRAMES` frames of `scene` into STREAM; false when the run failed.
bool
record(const fs::path& scene, const Folders& at) {
    loupe::Options options;
    options.scene = scene;
    options.out_bmp = loupe::utf8(at.out / "record.bmp");
    options.frames = FRAMES;
    options.record = loupe::utf8(at.out / STREAM);
    std::string why;
    if (!loupe::render(options, &why)) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    return true;
}

// Draws every frame forward, keeping each one's bytes; true when every drawn
// frame's CHECK is the same or absent, and at least one is the same.
bool
forward(loupe::Frames& frames, loupe::test::Straight* drawn) {
    int same = 0, differ = 0;
    bool stepped = frames.seek(0);
    (*drawn)[0] = loupe::test::pixels(frames);
    while (stepped && frames.index() < frames.count()) {
        stepped = frames.step(frames.dt_ms());
        (*drawn)[frames.index() - 1] = loupe::test::pixels(frames);
    }
    for (int i = 0; i < frames.count(); i++) {
        const loupe::Frames::Check check = frames.check(i);
        same += check == loupe::Frames::Check::Same ? 1 : 0;
        differ += check == loupe::Frames::Check::Differs || check == loupe::Frames::Check::OtherSize ? 1 : 0;
    }
    std::fprintf(stderr, "%s: %d CHECKs same, %d differ\n", TEST, same, differ);
    return stepped && same > 0 && differ == 0;
}

// The stream `id`, from `scene` (live when empty), writing `<id>.bmp`; null,
// said on stderr, when it does not open.
std::unique_ptr<loupe::Frames>
open(const Folders& at, const std::string& id, const fs::path& scene = {}) {
    loupe::Options options;
    options.scene = scene;
    options.out_bmp = loupe::utf8(at.out / (id + ".bmp"));
    std::string why;
    std::unique_ptr<loupe::Frames> frames = loupe::Frames::open_stream(options, at.packs, id, &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
    }
    return frames;
}

bool
saved_stream(const Folders& at, loupe::test::Straight* drawn) {
    const std::unique_ptr<loupe::Frames> frames = open(at, "saved", at.out / STREAM);
    if (frames == nullptr) {
        return false;
    }
    bool ok = expect(frames->count() == FRAMES && !frames->live(), "the saved stream holds every frame");
    ok &= expect(frames->first_seekable() == 0, "a stream seeks from frame 0");
    ok &= expect(forward(*frames, drawn), "every keyframe's CHECK is the same");
    ok &= expect(loupe::test::same_each(TEST, *frames, SEEKS, *drawn, "saved"), "seeks draw the forward bytes");
    return frames->close() && ok;
}

bool
live_stream(const std::vector<uint8_t>& bytes, const Folders& at, const loupe::test::Straight& drawn) {
    const std::unique_ptr<loupe::Frames> frames = open(at, "live");
    if (frames == nullptr) {
        return false;
    }
    bool ok = expect(frames->live() && frames->count() == 0, "a live stream opens empty");
    bool grew = true, fed = true;
    int last = 0;
    for (size_t from = 0; from < bytes.size(); from += PIECE) {
        const size_t count = std::min(PIECE, bytes.size() - from);
        fed &= frames->feed(std::span(bytes).subspan(from, count));
        grew &= frames->count() >= last;
        last = frames->count();
    }
    ok &= expect(fed && grew && frames->count() == FRAMES, "its count grows to every frame as bytes arrive");
    ok &= expect(loupe::test::same_each(TEST, *frames, SEEKS, drawn, "live"), "live frames draw the saved bytes");
    ok &= expect(frames->close(), "it closes");
    return ok;
}

bool
refused(const Folders& at) {
    const std::unique_ptr<loupe::Frames> frames = open(at, "refused");
    if (frames == nullptr) {
        return false;
    }
    const std::vector<uint8_t> noise(PIECE, 0xA5);
    const bool fed = frames->feed(noise);
    const std::optional<loupe::Frames::StreamState> state = frames->stream_state();
    const bool ok =
        expect(!fed && state && state->refused && !state->why.empty(), "bytes that are no stream are refused");
    std::fprintf(stderr, "%s: refused because: %s\n", TEST, state ? state->why.c_str() : "");
    (void)frames->close();
    return ok;
}

// The live stream fed whole into a stream over `packs`: its state after.
std::optional<loupe::Frames::StreamState>
fed_over(const std::vector<uint8_t>& bytes, const Folders& at, const fs::path& packs, const std::string& id) {
    const std::unique_ptr<loupe::Frames> frames = open({.packs = packs, .out = at.out}, id);
    if (frames == nullptr) {
        return std::nullopt;
    }
    for (size_t from = 0; from < bytes.size(); from += PIECE) {
        (void)frames->feed(std::span(bytes).subspan(from, std::min(PIECE, bytes.size() - from)));
    }
    std::optional<loupe::Frames::StreamState> state = frames->stream_state();
    (void)frames->close();
    return state;
}

bool
missing_pack(const std::vector<uint8_t>& bytes, const Folders& at) {
    const fs::path empty = at.out / "no_packs";
    fs::create_directories(empty);
    const auto state = fed_over(bytes, at, empty, "missing");
    std::fprintf(stderr, "%s: missing pack refused because: %s\n", TEST, state ? state->why.c_str() : "");
    return expect(state && state->refused
                      && state->why.find(std::string("missing pack ") + SCENE_PACK) != std::string::npos,
                  "a stream naming a pack its folder lacks is refused, naming the pack");
}

bool
firmware_packs_draw(const std::vector<uint8_t>& bytes, const Folders& at) {
    if (!loupe::builds_packs()) {
        std::fprintf(stderr, "%s: no pack tool in this build: no firmware packs built with it\n", TEST);
        return true;
    }
    const std::string pin = loupe::build_id(at.out).autana_commit;
    const fs::path cached = loupe::pack_cache() / ("firmware-" + pin);
    std::error_code error;
    fs::remove_all(cached, error);
    std::string why;
    const std::optional<fs::path> packs = loupe::firmware_packs(pin, at.out / "not_installed", &why);
    if (!packs.has_value()) {
        (void)expect(false, "the build made the pin's firmware packs");
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
        return false;
    }
    (void)expect(true, "the build made the pin's firmware packs");
    const fs::path& folder = *packs;
    const auto pack = [&](const char* name) { return folder / (std::string(name) + std::string(loupe::PACK_SUFFIX)); };
    bool ok = expect(fs::is_regular_file(pack(SCENE_PACK)) && fs::is_regular_file(pack(FIRMWARE_ONLY_PACK)),
                     "they hold the recorded scene's pack and one only the firmware carries");
    ok &= expect(!fs::exists(cached, error), "asking for them builds nothing");
    const auto state = fed_over(bytes, at, folder, "firmware");
    ok &= expect(state && !state->refused && state->frames == FRAMES, "the stream draws over them");
    return ok;
}

} // namespace

int
main(int argc, char** argv) {
    if (argc != 3) {
        std::fprintf(stderr, "usage: %s SCENE.apak FOLDER\n", TEST);
        return 2;
    }
    const fs::path scene = argv[1];
    const Folders at{.packs = scene.parent_path(), .out = argv[2]};
    fs::create_directories(at.out);
    if (!record(scene, at)) {
        return 1;
    }
    loupe::test::Straight drawn;
    bool ok = saved_stream(at, &drawn);
    ok &= live_stream(loupe::test::read_bytes(at.out / STREAM), at, drawn);
    ok &= refused(at);
    const std::vector<uint8_t> bytes = loupe::test::read_bytes(at.out / STREAM);
    ok &= missing_pack(bytes, at);
    ok &= firmware_packs_draw(bytes, at);
    return ok ? 0 : 1;
}
