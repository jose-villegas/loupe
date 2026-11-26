# Release

`project(loupe VERSION ...)` in `CMakeLists.txt` is the one version. Bump it, merge,
then push the tag `v<version>` on that commit. `.github/workflows/build.yml`
runs the `release` workflow preset on Windows and Linux, unpacks each download
into an empty folder and renders every sample in it with nothing on `PATH`
(`tools/ci/package_smoke.sh`), and on Windows installs the setup silently,
renders every sample from the installed `loupe.exe`, uninstalls it and checks
that no file, shortcut or registry key is left (`tools/ci/installer_smoke.sh`).
It then attaches the downloads to the tag's
GitHub Release, and refuses a tag that names another version. Every pull
request and nightly run builds and smoke-tests the downloads the same way,
without publishing them. A tag `v<version>-rc.1` publishes a pre-release whose
files and `--version` still name `<version>`.

Every push to `main` builds the same downloads with `-DLOUPE_CHANNEL=Main` and
puts them on the `main-build` pre-release (`tools/ci/publish_main.sh`): the tag
moves to the pushed commit, the files are replaced under the same names and the
notes list the pull requests since the latest release.
