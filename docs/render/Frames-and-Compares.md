# Frames of a camera path, and A | B compares

`--frame N -o out.png` writes frame N of the scene's camera path as a PNG, the
pixels a run of `--frames N+1` at the same `--dt` writes with `-o`. `--compare REV|DIR` draws the
path twice: side A is the scene given, side B the same `.scene.toml`'s pack
built by B's own copy of `third_party/autana/launcher/tools/r3d/build_pack.py`,
in a git worktree at the revision REV of
the scene's repository (removed after) or in the checkout DIR. Both sides are
drawn by this Loupe, one after the other. `--b-args "FLAGS"` gives B its own
`--object`, `--camera`, `--view` and `--set` flags in place of A's, so
`--b-args ""` draws B with none.

```bash
loupe sponza.scene.toml --frame 120 -o f.png
loupe sponza.scene.toml --compare main --video ab.mp4
loupe sponza.scene.toml --compare . --b-args "--view meshlets" --frame 120 -o f.png
```

| Command | Writes |
|---|---|
| `--frame N -o out.png` | frame N; with `--compare`, A's to `out.png` and B's to `out-b.png` |
| `--compare REV\|DIR --video out.mp4` | every frame of the path as A \| B \| difference x8, each frame labelled with its index and time, by autana's `render_compare.py --video`; its numbers per frame go to `out.csv` |

Both time the path alike, so frame N of a video is what `--frame N` writes:
`--dt` when given, else the scene's own frame time, or the shortest whole
millisecond at which the path fits one video when it does not at that;
`--frames` when given, else the whole path. A frame past the path's end, a
revision that does not exist and a checkout without the scene exit 2 saying
why. `--compare` needs the pack tool, git, ffmpeg and the Python modules in
`third_party/autana/launcher/tools/render/requirements.txt`; a build with
`-DLOUPE_PACK_TOOL=OFF` refuses it, and says so.
