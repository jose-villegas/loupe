// A folder of a run's own files, under the system's temporary folder.
#pragma once

#include <filesystem>

namespace loupe {

// A new, empty folder, removed with everything in it when this goes.
class Scratch {
  public:
    Scratch();
    ~Scratch();
    Scratch(const Scratch&) = delete;
    Scratch& operator=(const Scratch&) = delete;
    Scratch(Scratch&&) = delete;
    Scratch& operator=(Scratch&&) = delete;

    // Empty when the folder could not be made.
    const std::filesystem::path&
    path() const {
        return path_;
    }

  private:
    std::filesystem::path path_;
};

} // namespace loupe
