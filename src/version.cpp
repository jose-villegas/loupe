#include "version.hpp"

#include "commits.inc"

namespace loupe {

namespace {

const char*
channel_name(Channel channel) {
    switch (channel) {
        case Channel::Releases: return "release";
        case Channel::Main: return "main build";
        case Channel::None: break;
    }
    return "local build";
}

// An installed copy: the installer's uninstaller beside loupe.
bool
installed_in(const std::filesystem::path& exe_dir) {
#ifdef LOUPE_UNINSTALLER
    std::error_code error;
    return !exe_dir.empty() && std::filesystem::is_regular_file(exe_dir / LOUPE_UNINSTALLER, error);
#else
    (void)exe_dir;
    return false;
#endif
}

} // namespace

BuildId
build_id(const std::filesystem::path& exe_dir) {
    return {.version = LOUPE_VERSION,
            .commit = LOUPE_COMMIT,
            .autana_commit = LOUPE_AUTANA_COMMIT,
            .built_for = Channel::LOUPE_CHANNEL,
            .installed = installed_in(exe_dir)};
}

std::string
version_line(const BuildId& id) {
    return "loupe " + id.version + " (" + id.commit + ", " + channel_name(id.built_for) + ", "
           + (id.installed ? "installed" : "portable") + "), autana " + id.autana_commit;
}

} // namespace loupe
