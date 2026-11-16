// A frame record stream opens like a scene: a host run's --record, opened as
// a .framerec, draws every frame with each keyframe's CHECK the same, and a
// seek back draws the bytes the forward step drew. Fed live in pieces, its
// count grows as frames arrive and draws them alike; bytes that are no stream
// are refused.
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <span>
#include <string>
#include <vector>

#include "frames_fixture.hpp"
#include "paths.hpp"

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

// Where the scene's packs are and where the test writes.
struct Folders {
    fs::path packs, out;
};

bool
expect(bool ok, const char* what) {
    std::fprintf(stderr, "%s: %s %s\n", TEST, what, ok ? "ok" : "FAILED");
    return ok;
}

std::vector<uint8_t>
read_bytes(const fs::path& file) {
    std::ifstream in(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
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

bool
saved_stream(const Folders& at, loupe::test::Straight* drawn) {
    loupe::Options options;
    options.scene = at.out / STREAM;
    options.out_bmp = loupe::utf8(at.out / "saved.bmp");
    std::string why;
    const std::unique_ptr<loupe::Frames> frames = loupe::Frames::open_stream(options, at.packs, "saved", &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
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
    loupe::Options options;
    options.out_bmp = loupe::utf8(at.out / "live.bmp");
    std::string why;
    const std::unique_ptr<loupe::Frames> frames = loupe::Frames::open_stream(options, at.packs, "live", &why);
    if (frames == nullptr) {
        std::fprintf(stderr, "%s: %s\n", TEST, why.c_str());
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
    loupe::Options options;
    options.out_bmp = loupe::utf8(at.out / "refused.bmp");
    std::string why;
    const std::unique_ptr<loupe::Frames> frames = loupe::Frames::open_stream(options, at.packs, "refused", &why);
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
    ok &= live_stream(read_bytes(at.out / STREAM), at, drawn);
    ok &= refused(at);
    return ok ? 0 : 1;
}
