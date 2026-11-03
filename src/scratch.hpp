// A folder of a run's own files, under the system's temporary folder.
#pragma once

#include <filesystem>
#include <string_view>

namespace loupe {

// A new, empty folder named `prefix` and random hex, removed with everything
// in it when this goes unless kept.
class Scratch {
  public:
    explicit Scratch(std::string_view prefix = "loupe-");
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

    // Leaves the folder in place when this goes.
    void
    keep() {
        kept_ = true;
    }

  private:
    std::filesystem::path path_;
    bool kept_ = false;
};

} // namespace loupe
