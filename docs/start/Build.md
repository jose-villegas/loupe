# Build

You need GCC (MinGW-w64 on Windows), CMake 3.25 or newer and the Python
autana's pack tool runs on (`third_party/autana/scripts/lib/python-min-version`);
without the tests, `-DLOUPE_PACK_TOOL=OFF -DBUILD_TESTING=OFF` builds a Loupe
that opens `.apak` files only and needs no Python.
On Linux, also the packages in `tools/ci/linux-packages.txt`, which
`tools/ci/install-linux-packages.sh` installs. The window draws through Vulkan,
loaded when it opens, so the build needs no Vulkan SDK.

```bash
git clone --recurse-submodules https://github.com/jose-villegas/loupe
cd loupe
pip install -r tools/ci/requirements.txt
cmake --workflow --preset default
```

`cmake --preset default -DAUTANA_DIR=<path>` configures against another autana checkout instead of the submodule.
