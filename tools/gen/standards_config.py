"""Print Loupe's .clang-format or .clang-tidy, derived from the pinned autana.

    python tools/gen/standards_config.py clang-format > .clang-format
    python tools/gen/standards_config.py clang-tidy > .clang-tidy

.clang-format is autana's own. .clang-tidy is the check set of autana's
docs/c-style/Cpp.md, with the cognitive-complexity limit read from autana's
complexity gate. Both carry autana's generated-file banner, so
check_generated_files.py fails a copy that drifts from the pin.
"""
import ast
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
AUTANA = ROOT / "third_party/autana"
COMPLEXITY_GATE = AUTANA / "launcher/tools/quality/complexity_gate.py"
MARKER = "GENERATED FILE - do not edit."

CHECKS = (
    "bugprone-*",
    "performance-*",
    "modernize-use-designated-initializers",
    "modernize-use-ranges",
    "modernize-use-std-numbers",
    "modernize-use-starts-ends-with",
    "modernize-use-integer-sign-comparison",
    "modernize-use-constraints",
    "cppcoreguidelines-owning-memory",
    "cppcoreguidelines-pro-type-member-init",
    "cppcoreguidelines-special-member-functions",
    "cppcoreguidelines-avoid-capturing-lambda-coroutines",
    "misc-const-correctness",
    "readability-use-std-min-max",
    "readability-function-cognitive-complexity",
)


def module_constant(path, name):
    """The literal a module assigns to `name` at top level."""
    for node in ast.parse(path.read_text(encoding="utf-8")).body:
        if isinstance(node, ast.Assign) and any(getattr(t, "id", None) == name for t in node.targets):
            return ast.literal_eval(node.value)
    raise SystemExit(f"{path} assigns no {name}")


def banner(tool):
    return f"# {MARKER}\n# python tools/gen/standards_config.py {tool} > .{tool}\n\n"


def clang_format():
    return banner("clang-format") + (AUTANA / ".clang-format").read_text(encoding="utf-8")


def clang_tidy():
    threshold = module_constant(COMPLEXITY_GATE, "FAIL_THRESHOLD")
    checks = ",\n".join(f"  {check}" for check in ("-*", *CHECKS))
    return (banner("clang-tidy")
            + f"Checks: >-\n{checks}\n"
            + "WarningsAsErrors: '*'\n"
            # Loupe's own headers; autana's come in as system headers and are not reported.
            + "HeaderFilterRegex: '/src/'\n"
            + "CheckOptions:\n"
            + f"  readability-function-cognitive-complexity.Threshold: {threshold}\n")


if __name__ == "__main__":
    tools = {"clang-format": clang_format, "clang-tidy": clang_tidy}
    if len(sys.argv) != 2 or sys.argv[1] not in tools:
        sys.exit(f"usage: {sys.argv[0]} {'|'.join(tools)}")
    # Bytes, so a Windows console writes LF as git stores it.
    sys.stdout.buffer.write(tools[sys.argv[1]]().encode("utf-8"))
