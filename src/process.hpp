// Other processes: the tools loupe waits on.
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace loupe {

// Runs `args` with no shell between, its output into `log`; true when it exits 0.
bool run_logged(const std::vector<std::string>& args, const std::filesystem::path& log);
// The running loupe's own executable; empty when the system does not say.
std::filesystem::path exe_path();
// The system's own `name` (curl, tar): System32's on Windows, the first on
// PATH elsewhere; empty when there is none.
std::filesystem::path system_program(const std::string& name);
// Starts `program` with `args` and returns without waiting; on Windows through
// the shell, so a program that asks for elevation gets its UAC prompt. False
// when it did not start.
bool start_unwaited(const std::filesystem::path& program, const std::vector<std::string>& args);

} // namespace loupe
