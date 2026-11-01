#include "engine/frames.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>

#include "engine/autana.h"
#include "pack.h"

namespace loupe {

namespace {

constexpr const char SCENE_SUFFIX[] = ".scene.toml";

bool set_asset_dir(const std::string& folder) {
#if defined(_WIN32)
    return _putenv_s(ASSET_STORE_DIR_ENV, folder.c_str()) == 0;
#else
    return setenv(ASSET_STORE_DIR_ENV, folder.c_str(), 1) == 0;
#endif
}

// The scene provider's command line for `options`, as scene_viewer.sh passes it.
std::vector<std::string> provider_args(const Options& options) {
    const std::string name = std::filesystem::path(options.scene_toml).filename().string();
    std::vector<std::string> args = {"loupe", "--scene", name.substr(0, name.size() - (sizeof SCENE_SUFFIX - 1))};
    for (const std::string& object : options.objects) {
        args.insert(args.end(), {"--object", object});
    }
    if (!options.camera.empty()) {
        args.insert(args.end(), {"--camera", options.camera});
    }
    args.insert(args.end(), {"--view", options.view, "--frames", std::to_string(options.frames), "--dt",
                             std::to_string(options.dt_ms)});
    for (const std::string& set : options.sets) {
        args.insert(args.end(), {"--set", set});
    }
    if (!options.out_bmp.empty()) {
        args.insert(args.end(), {"-o", options.out_bmp});
    }
    if (!options.out_video.empty()) {
        args.insert(args.end(), {"--video", options.out_video});
    }
    return args;
}

}  // namespace

std::unique_ptr<Frames> Frames::open(const Options& options, std::string* why) {
    std::string pack;
    if (!scene_pack(options.scene_toml, &pack, why)) {
        return nullptr;
    }
    if (!set_asset_dir(pack)) {
        *why = "cannot set " ASSET_STORE_DIR_ENV;
        return nullptr;
    }
    std::unique_ptr<Frames> frames(new Frames());
    frames->args_ = provider_args(options);
    frames->dt_ms_ = options.dt_ms;
    for (std::string& arg : frames->args_) {
        frames->argv_.push_back(arg.data());
    }
    frames->argv_.push_back(nullptr);
    int exit_code = 0;
    if (!render_host_open(&scene_provider, static_cast<int>(frames->args_.size()), frames->argv_.data(),
                          &exit_code)) {
        *why = "the scene did not open; autana said why above";
        return nullptr;
    }
    frames->open_ = true;
    return frames;
}

Frames::~Frames() {
    (void)close();
}

void Frames::step(uint32_t dt_ms) {
    if (index_ < static_cast<int>(dts_.size()) && dts_[index_] != dt_ms) {
        drop_checkpoints_after(index_);
    }
    // From frame 1: autana restores only state whose allocations the first frame made.
    if (index_ % CHECKPOINT_EVERY == 1 && checkpoints_.count(index_) == 0) {
        if (render_host_checkpoint_t* saved = render_host_save()) {
            checkpoints_[index_] = saved;
        }
    }
    const input_t input = {};
    render_host_step(dt_ms, &input);
    if (index_ == static_cast<int>(dts_.size())) {
        dts_.push_back(dt_ms);
    }
    index_++;
}

bool Frames::seek(int index) {
    if (index < 0) {
        return false;
    }
    if (index < index_) {
        auto from = checkpoints_.upper_bound(index);
        if (from == checkpoints_.begin() || !render_host_restore((--from)->second)) {
            return false;
        }
        index_ = from->first;
    }
    while (index_ <= index) {
        step(index_ < static_cast<int>(dts_.size()) ? dts_[index_] : dt_ms_);
    }
    return true;
}

Frame Frames::frame() const {
    size_t bytes = 0;
    const uint8_t* pixels = render_host_pixels(&bytes);
    return {pixels, bytes};
}

std::optional<uint32_t> Frames::path_ms() const {
    if (scene_provider.path_ms == nullptr) {
        return std::nullopt;
    }
    return scene_provider.path_ms();
}

bool Frames::close() {
    if (!open_) {
        return true;
    }
    drop_checkpoints_after(-1);
    open_ = false;
    return render_host_close();
}

void Frames::drop_checkpoints_after(int index) {
    for (auto it = checkpoints_.upper_bound(index); it != checkpoints_.end(); it = checkpoints_.erase(it)) {
        render_host_checkpoint_free(it->second);
    }
    dts_.resize(static_cast<size_t>(std::max(index, 0)));
}

}  // namespace loupe
