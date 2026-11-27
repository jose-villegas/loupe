// Other processes: the tools loupe waits on.
#pragma once

#include <cstdint>
#include <filesystem>
#include <stop_token>
#include <string>
#include <vector>

namespace loupe {

// `args` as the null-ended argv posix_spawn, SDL and autana take: pointers
// into `args`, which must outlive it.
std::vector<char*> argv_of(const std::vector<std::string>& args);
// Runs `args` with no shell between, its output into `log`; true when it
// exits 0. A request on `stop` ends it, and it counts as failed.
bool run_logged(const std::vector<std::string>& args, const std::filesystem::path& log,
                const std::stop_token& stop = {});
// The running loupe's own executable; empty when the system does not say.
std::filesystem::path exe_path();
// The system's own `name` (curl, tar): System32's on Windows, the first on
// PATH elsewhere; empty when there is none.
std::filesystem::path system_program(const std::string& name);
// Starts `program` with `args` and returns without waiting; on Windows through
// the shell, so a program that asks for elevation gets its UAC prompt. False
// when it did not start.
bool start_unwaited(const std::filesystem::path& program, const std::vector<std::string>& args);

// A child's exit code, held by its pid from while it runs. On Windows a child
// started without a console window (SDL's background process) reports 0
// whatever it exits with; this reads the code it gave. Elsewhere the reported
// code stands.
class ExitCode {
  public:
    explicit ExitCode(int64_t pid);
    ~ExitCode();
    ExitCode(const ExitCode&) = delete;
    ExitCode& operator=(const ExitCode&) = delete;
    ExitCode(ExitCode&&) = delete;
    ExitCode& operator=(ExitCode&&) = delete;
    // Once the child has ended: the code it gave, or `reported` where there is none to read.
    [[nodiscard]] int of(int reported) const;

  private:
    void* handle_ = nullptr;
};

} // namespace loupe
