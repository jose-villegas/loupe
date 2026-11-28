# Render a scene

With `-o` or `--video`, Loupe writes the frames and exits, without a window:

```bash
build/default/loupe third_party/autana/launcher/demo/sponza/sponza.scene.toml --view meshlets --frames 60 -o sponza.bmp
```

| Flag | Meaning |
|---|---|
| `--object NAME` | draw only this renderer and its children; repeatable |
| `--camera NAME` | the camera to look through; the scene's first by default |
| `--view NAME` | `shaded` or a debug view; an unknown name lists them |
| `--frames N`, `--dt MS` | how many frames, how far apart (30 at 16 ms) |
| `--set owner.name=value` | a tunable; repeatable |
| `-o out.bmp`, `--video out.avi` | the last frame, every frame |
| `--record out.framerec` | also write the stream of the frames drawn ([View a board](../board/View-a-Board.md)) |
| `--checkpoint-mb MB` | the window's checkpoint memory (1024); past it, every other checkpoint goes |
| `--ticks N` | closes the window after N ticks, for scripted runs |
| `--version` | prints the build's version, commit, channel and autana commit ([Download](../start/Download.md)), and exits |

The flags other than `--record`, `--checkpoint-mb`, `--ticks` and `--version` are autana's `third_party/autana/launcher/tools/render/scene_viewer.sh` flags, and the output is the same bytes.
A `.apak` opens as it is: its folder is the asset dir, its name the scene id.
A `.scene.toml` run first brings the scene's pack in `build/packs` up to date
with autana's `third_party/autana/launcher/tools/r3d/build_pack.py`; a build
with `-DLOUPE_PACK_TOOL=OFF` refuses one, and says so.
