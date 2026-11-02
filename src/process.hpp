// Other processes: a tool loupe waits on, and another loupe it hands the window to.
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace loupe {

// Runs `args` with no shell between, its output into `log`; true when it exits 0.
bool run_logged(const std::vector<std::string>& args, const std::filesystem::path& log);
// Starts `args` with no shell between and does not wait for it; false when it did not start.
bool spawn(const std::vector<std::string>& args);
// The running loupe's own executable; empty when the system does not say.
std::filesystem::path exe_path();

} // namespace loupe
