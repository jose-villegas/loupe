# Download

Each [release](https://github.com/jose-villegas/loupe/releases) has three
downloads, unsigned, so Windows SmartScreen warns on their first run. Each holds
a pack of every autana demo scene as a sample, listed on the start panel. They
need no Python and no compiler; the window needs a Vulkan driver.

| Download | Holds | To run it |
|---|---|---|
| `loupe-<version>-windows-portable.zip` | `loupe.exe`, `scenes/`, `LICENSE` | unzip anywhere and run `loupe.exe` |
| `loupe-<version>-windows-setup.exe` | the same files, installed into Program Files with a Start menu entry and a desktop shortcut; `.apak` files then open in Loupe | run it; uninstall from Windows' installed apps, which removes the shortcuts and the `.apak` association too |
| `loupe-<version>-linux-x86_64.tar.gz` | `bin/loupe`, and in `share/` the samples, `applications/loupe.desktop` and the icon | run `bin/loupe` where it lands, or copy `bin/` and `share/` into `~/.local` to list Loupe among the desktop's applications |

The Linux build is made on Ubuntu 24.04 with GCC's runtime linked in, so it
needs only glibc 2.39 or newer, X11 or Wayland, and a Vulkan driver.

The [main build](https://github.com/jose-villegas/loupe/releases/tag/main-build)
pre-release holds the same downloads built from the latest push to `main`,
named `main` in place of the version (`loupe-main-windows-portable.zip`), so
its links stay the same.

About, on the window's menu bar, or `loupe --version`, names the build:

```text
loupe 0.1.0 (1a2b3c4d5, main build, portable), autana 262e7d0ab
```

| Part | Says |
|---|---|
| `0.1.0` | the version |
| `1a2b3c4d5` | Loupe's commit |
| `main build` | the channel it was built for: `release` from a version tag, `main build` from `main`, `local build` from anywhere else (`-DLOUPE_CHANNEL=None`, the default) |
| `portable` | `installed` for the setup's copy, else `portable` |
| `262e7d0ab` | the autana commit it was built against |

## Updates

A downloaded Loupe keeps itself current. About's **Updates** picks what it
follows; each copy starts on its own channel, and a local build never updates.

| Updates | Takes |
|---|---|
| Off | nothing |
| Official releases | the latest release, when its version is higher than this copy's |
| Main builds | the main build, when it is of another commit than this copy |

Each time the window opens, Loupe asks GitHub once, in the background. A newer
build is downloaded and checked against GitHub's SHA-256 digest of the file,
then kept in Loupe's settings folder (`%APPDATA%\autana\loupe` or
`~/.local/share/autana/loupe`) until the window closes. Then it takes this
copy's place: a portable copy's files are replaced, the running `loupe` renamed
aside until the next start; an installed copy runs the new setup silently,
which asks Windows for permission once. The next start's About says what it was
updated to. It downloads and unpacks with the system's own `curl` and `tar`,
nothing ever pops up, and every step is logged to the Console and `loupe.log` in
the settings folder. Runs with `-o`, `--video` or `--ticks` never update.
