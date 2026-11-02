#include "path/path.hpp"

#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <climits>
#include <cstdio>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "engine/frames.hpp"
#include "pack.hpp"
#include "paths.hpp"
#include "process.hpp"
#include "scratch.hpp"

namespace fs = std::filesystem;

namespace loupe {

namespace {

constexpr int FAILED = 1;
constexpr int REFUSED = 2;

// Why a step stopped, and the exit code it stops with.
struct Stop {
    int code = 0;
    std::string why;
};

bool
stop(Stop* stop, int code, std::string why) {
    *stop = {.code = code, .why = std::move(why)};
    return false;
}

std::string
read_text(const fs::path& file) {
    const std::ifstream in(file, std::ios::binary);
    std::ostringstream text;
    text << in.rdbuf();
    return text.str();
}

// `frame` as a PNG at `out`, its bottom-up rows turned top-down as a surface
// holds them; false when it could not be written.
bool
write_png(const Frame& frame, const fs::path& out) {
    std::vector<uint8_t> rows(frame.stride * static_cast<size_t>(frame.height));
    for (int y = 0; y < frame.height; y++) {
        const auto from = frame.bgr.begin() + static_cast<std::ptrdiff_t>(frame.stride * (frame.height - 1 - y));
        std::copy_n(from, frame.stride, rows.begin() + static_cast<std::ptrdiff_t>(frame.stride * y));
    }
    const std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surface(
        SDL_CreateSurfaceFrom(frame.width, frame.height, SDL_PIXELFORMAT_BGR24, rows.data(),
                              static_cast<int>(frame.stride)),
        &SDL_DestroySurface);
    return surface != nullptr && SDL_SavePNG(surface.get(), utf8(out).c_str());
}

// `side`'s run of `options` stepped to frame `index`, written to `out` as a
// PNG, whatever the frame watch says of the warm-up the close draws after it.
bool
write_frame(const std::string& side, Options options, int index, const fs::path& out, Stop* failed) {
    options.frames = index + 1;
    std::string why;
    std::unique_ptr<Frames> frames = Frames::open(options, &why);
    if (frames == nullptr) {
        return stop(failed, FAILED, std::format("{}: {}", side, why));
    }
    if (!frames->seek(index)) {
        return stop(failed, FAILED, std::format("{}: frame {} was not drawn", side, index));
    }
    if (!write_png(frames->frame(), out)) {
        return stop(failed, FAILED, std::format("{}: cannot write {}: {}", side, utf8(out), SDL_GetError()));
    }
    if (!frames->close()) {
        std::fprintf(stderr, "loupe: %s's run failed after frame %d; autana said why above\n", side.c_str(), index);
    }
    std::printf("%s\n", utf8(out).c_str());
    return true;
}

// The renderer flags of `options`, as a label shows them.
std::string
flags_text(const Options& options) {
    std::string text;
    const auto add = [&text](const char* flag, const std::string& value) {
        if (!value.empty()) {
            text += std::format(" {} {}", flag, value);
        }
    };
    for (const std::string& object : options.objects) {
        add("--object", object);
    }
    add("--camera", options.camera);
    add("--view", options.view);
    for (const std::string& set : options.sets) {
        add("--set", set);
    }
    return text;
}

// `options` with only the renderer's flags and the scene: the path, the outputs
// and the compare are each side's run's to set.
Options
renderer_only(Options options) {
    options.frames.reset();
    options.dt_ms.reset();
    options.out_bmp.clear();
    options.out_video.clear();
    options.frame.reset();
    options.compare.clear();
    options.b_args.reset();
    return options;
}

// One side of a compare: what its panel is labelled, and its run.
struct Side {
    std::string label;
    Options options;
};

// Where side `index` of --frame's pair goes: OUT for A, OUT's stem with -b for B.
fs::path
side_path(const fs::path& out, size_t index) {
    return index == 0 ? out : fs::path(out).replace_filename(utf8(out.stem()) + "-b" + utf8(out.extension()));
}

// A's timing, from a one-frame run of it: its own frame time, its path's
// length, and how many of its frames one video holds; --frames and --dt from
// `given`.
bool
time_path(const Side& a, const Options& given, const fs::path& work, PathTiming* timing, Stop* failed) {
    Options probe = a.options;
    probe.frames = 1;
    probe.dt_ms = given.dt_ms;
    probe.out_bmp = utf8(work / "probe.bmp");
    std::string why;
    std::unique_ptr<Frames> frames = Frames::open(probe, &why);
    if (frames == nullptr) {
        return stop(failed, FAILED, why);
    }
    const std::optional<uint32_t> path_ms = frames->path_ms();
    const PathFacts path = {
        .path_ms = path_ms.value_or(0), .own_dt_ms = frames->dt_ms(), .fit = frames->video_frames_fit()};
    (void)frames->close();
    if (!path_ms) {
        return stop(failed, REFUSED, "the scene has no camera path to time");
    }
    *timing = path_timing(path, given);
    if (!given.dt_ms && timing->dt_ms != path.own_dt_ms) {
        std::fprintf(stderr, "loupe: the %u ms path fits one video at --dt %u\n", path.path_ms, timing->dt_ms);
    }
    return true;
}

// False with `failed` set when --frame asks for a frame past the path's end.
bool
frame_on_path(std::optional<int> frame, const PathTiming& timing, Stop* failed) {
    if (!frame || *frame < timing.frames) {
        return true;
    }
    return stop(failed, REFUSED,
                std::format("the path has frames 0 to {} at --dt {}, not {}", timing.frames - 1, timing.dt_ms, *frame));
}

// Side A: the scene given, its pack built once for every run of it, under its own flags.
bool
a_side(const Options& options, Side* a, Stop* failed) {
    std::string why;
    const std::optional<ScenePack> pack = resolve_scene(options.scene, &why);
    if (!pack) {
        return stop(failed, FAILED, why);
    }
    a->label = utf8(options.scene.filename()) + flags_text(options);
    a->options = renderer_only(options);
    a->options.scene = pack_file(*pack);
    return true;
}

// A run's own files: a scratch folder and, for a compare at a revision, B's
// git worktree in it, removed again before the folder goes.
class Work {
  public:
    Work() = default;

    ~Work() {
#if LOUPE_PACK_TOOL
        if (!repo_.empty()) {
            (void)run_logged({LOUPE_GIT, "-C", utf8(repo_), "worktree", "remove", "--force", utf8(tree())},
                             dir() / "worktree-remove.log");
        }
#endif
    }

    Work(const Work&) = delete;
    Work& operator=(const Work&) = delete;
    Work(Work&&) = delete;
    Work& operator=(Work&&) = delete;

    const fs::path&
    dir() const {
        return scratch_.path();
    }

    fs::path
    tree() const {
        return dir() / "b";
    }

#if LOUPE_PACK_TOOL
    // Git in `repo` with `args`; its output, or none with `failed` set to say why.
    std::optional<std::string>
    git(const fs::path& repo, std::vector<std::string> args, Stop* failed) const {
        args.insert(args.begin(), {LOUPE_GIT, "-C", utf8(repo)});
        const fs::path log = dir() / "git.log";
        if (!run_logged(args, log)) {
            (void)stop(failed, FAILED, std::format("git {} failed:\n{}", args[3], read_text(log)));
            return std::nullopt;
        }
        std::string out = read_text(log);
        out.erase(out.find_last_not_of("\r\n") + 1);
        return out;
    }

    // Checks `rev` out of `repo`, submodules too, into tree(); false with `failed` set.
    bool
    add_worktree(const fs::path& repo, const std::string& rev, Stop* failed) {
        if (!git(repo, {"rev-parse", "--verify", "--quiet", rev + "^{commit}"}, failed)) {
            return stop(failed, REFUSED, "not a revision or directory: " + rev);
        }
        if (!git(repo, {"worktree", "add", "--detach", utf8(tree()), rev}, failed)) {
            return false;
        }
        repo_ = repo;
        return git(tree(), {"submodule", "update", "--init", "--recursive"}, failed).has_value();
    }
#endif

  private:
    Scratch scratch_;
    fs::path repo_; // the repository tree() was added to, once it was
};

#if LOUPE_PACK_TOOL
// B's pack: the scene's .scene.toml at the same path in the checkout at `rev_or_dir`,
// a folder or a revision of the scene's own repository, built by that
// checkout's build_pack.py. The pack's path, or none with `failed` set.
std::optional<ScenePack>
pack_at(const fs::path& scene, const std::string& rev_or_dir, Work& work, Stop* failed) {
    std::error_code error;
    const fs::path scene_file = fs::canonical(scene, error);
    if (error) {
        (void)stop(failed, REFUSED, "no scene file: " + utf8(scene));
        return std::nullopt;
    }
    Stop ignored;
    const std::optional<std::string> top =
        work.git(scene_file.parent_path(), {"rev-parse", "--show-toplevel"}, &ignored);
    if (!top) {
        (void)stop(failed, REFUSED, "--compare needs a scene file inside a git checkout: " + utf8(scene));
        return std::nullopt;
    }
    const fs::path repo = fs::canonical(from_utf8(*top), error);
    const fs::path relative = scene_file.lexically_relative(repo);
    fs::path root = fs::canonical(from_utf8(rev_or_dir), error);
    if (error || !fs::is_directory(root)) {
        if (!work.add_worktree(repo, rev_or_dir, failed)) {
            return std::nullopt;
        }
        root = work.tree();
    }
    const fs::path b_scene = root / relative;
    if (!fs::is_regular_file(b_scene, error)) {
        (void)stop(failed, REFUSED, std::format("{} has no {}", rev_or_dir, relative.generic_string()));
        return std::nullopt;
    }
    std::fprintf(stderr, "loupe: building B's pack in %s\n", utf8(root).c_str());
    const fs::path folder = work.dir() / "b_pack";
    const fs::path script = root / "launcher" / "tools" / "r3d" / "build_pack.py";
    const fs::path log = work.dir() / "b_pack.log";
    if (!run_logged({LOUPE_PYTHON, utf8(script), "-o", utf8(folder), utf8(b_scene)}, log)) {
        (void)stop(failed, FAILED, "B's build_pack.py failed:\n" + read_text(log));
        return std::nullopt;
    }
    return ScenePack{.folder = folder, .id = scene_id(b_scene)};
}

// Side B: the scene's pack at --compare's REV or DIR, under --b-args' flags or A's.
bool
b_side(const Options& options, Work& work, Side* b, Stop* failed) {
    const std::optional<ScenePack> pack = pack_at(options.scene, options.compare, work, failed);
    if (!pack) {
        return false;
    }
    std::string why;
    (void)b_options(options, &b->options, &why);
    b->options = renderer_only(b->options);
    b->options.scene = pack_file(*pack);
    b->label = options.compare + flags_text(b->options);
    return true;
}

// Both sides' whole path, one after the other, then autana's render_compare.py
// lays them out as A | B | difference x8 at `out`, every frame's numbers in
// OUT's stem .csv beside it. A failed frame watch fails a run after every
// frame is written, so a side whose video was written still stands.
bool
compare_video(const std::vector<Side>& sides, const PathTiming& timing, const fs::path& out, const fs::path& work,
              Stop* failed) {
    std::vector<std::string> videos;
    for (const auto& [name, side] : {std::pair{"a", &sides[0]}, std::pair{"b", &sides[1]}}) {
        Options run = side->options;
        run.frames = timing.frames;
        run.dt_ms = timing.dt_ms;
        run.out_bmp = utf8(work / std::format("{}.bmp", name));
        run.out_video = utf8(work / std::format("{}.avi", name));
        std::fprintf(stderr, "loupe: drawing %s, %d frames at --dt %u\n", side->label.c_str(), timing.frames,
                     timing.dt_ms);
        std::string why;
        if (!render(run, &why) && !fs::is_regular_file(run.out_video)) {
            return stop(failed, FAILED, std::format("{}: {}", side->label, why));
        }
        videos.push_back(run.out_video);
    }
    std::error_code ignored;
    fs::create_directories(fs::absolute(out).parent_path(), ignored);
    const fs::path script = fs::path(LOUPE_AUTANA_DIR) / "launcher" / "tools" / "render" / "render_compare.py";
    const fs::path csv = fs::path(out).replace_extension(".csv");
    const fs::path log = work / "render_compare.log";
    const bool ok = run_logged({LOUPE_PYTHON, utf8(script), "--out", utf8(out), "--csv", utf8(csv), "--video",
                                videos[0], videos[1], "--label-a", sides[0].label, "--label-b", sides[1].label},
                               log);
    std::fputs(read_text(log).c_str(), stderr);
    if (!ok) {
        return stop(failed, FAILED, "render_compare.py failed");
    }
    std::printf("%s\n", utf8(out).c_str());
    return true;
}
#endif

} // namespace

PathTiming
path_timing(const PathFacts& path, const Options& given) {
    uint32_t dt = given.dt_ms.value_or(path.own_dt_ms);
    if (!given.dt_ms && path.path_ms / dt + 1 > path.fit) {
        const int64_t gaps = std::max<int64_t>(path.fit - 1, 1);
        dt = static_cast<uint32_t>((path.path_ms + gaps - 1) / gaps);
    }
    const int64_t whole = std::min<int64_t>(path.path_ms / dt + 1, INT_MAX);
    return {.frames = given.frames.value_or(static_cast<int>(whole)), .dt_ms = dt};
}

bool
render(const Options& options, std::string* why) {
    std::unique_ptr<Frames> frames = Frames::open(options, why);
    if (frames == nullptr) {
        return false;
    }
    const int count = frames->count();
    for (int i = 0; i < count; i++) {
        frames->step(frames->dt_ms());
    }
    if (!frames->close()) {
        *why = "the run failed; autana said why above";
        return false;
    }
    return true;
}

int
run_path(const Options& options) {
    Stop failed;
    const auto said = [&failed] {
        std::fprintf(stderr, "loupe: %s\n", failed.why.c_str());
        return failed.code;
    };
    Work work;
    std::vector<Side> sides(1);
    PathTiming timing;
    if (!(!work.dir().empty() || stop(&failed, FAILED, "cannot make a scratch folder"))
        || !a_side(options, &sides[0], &failed)) {
        return said();
    }
#if LOUPE_PACK_TOOL
    if (!options.compare.empty() && !b_side(options, work, &sides.emplace_back(), &failed)) {
        return said();
    }
#endif
    if (!time_path(sides[0], options, work.dir(), &timing, &failed) || !frame_on_path(options.frame, timing, &failed)) {
        return said();
    }
#if LOUPE_PACK_TOOL
    if (!options.frame) {
        return compare_video(sides, timing, options.out_video, work.dir(), &failed) ? 0 : said();
    }
#endif
    for (size_t i = 0; i < sides.size(); i++) {
        Options run = sides[i].options;
        run.dt_ms = timing.dt_ms;
        run.out_bmp = utf8(work.dir() / std::format("frame-{}.bmp", i));
        if (!write_frame(sides[i].label, run, *options.frame, side_path(options.out_bmp, i), &failed)) {
            return said();
        }
    }
    return 0;
}

} // namespace loupe
