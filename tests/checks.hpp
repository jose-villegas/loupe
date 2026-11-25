// What every test that checks one thing at a time does: say each check under
// the test's name, and read, split and write the files it works on.
#pragma once

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace loupe::test {

// A test's checks: check(ok, what) says "TEST: what ok", or FAILED, on stderr and gives back `ok`.
struct Checks {
    const char* test;

    bool
    operator()(bool ok, const std::string& what) const {
        std::fprintf(stderr, "%s: %s %s\n", test, what.c_str(), ok ? "ok" : "FAILED");
        return ok;
    }
};

// The bytes of the file at `path`; empty when it does not read.
inline std::string
read_file(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

// `text`'s lines, without their ends.
inline std::vector<std::string>
lines(const std::string& text) {
    std::vector<std::string> out;
    std::istringstream in(text);
    for (std::string line; std::getline(in, line);) {
        out.push_back(line);
    }
    return out;
}

// Writes `text` as the file at `path`, byte for byte.
inline void
write_file(const std::filesystem::path& path, const std::string& text) {
    std::ofstream(path, std::ios::binary) << text;
}

} // namespace loupe::test
