# Open a scene

```bash
build/default/loupe third_party/autana/launcher/demo/sponza/sponza.scene.toml
```

Run without a scene, the window opens on a start panel: the sample packs beside
`loupe` (`scenes/`, or `../share/loupe/scenes/` when installed), the last 10
scenes opened, and Open... for any other. A scene picked there opens in this
window with the scene's own defaults. Scenes, in a
scene's window, closes the scene and goes back to the panel. The recent list is `recent.txt` in
`%APPDATA%\autana\loupe` on Windows and `~/.local/share/autana/loupe` on Linux.

The window shows the engine's frame at the largest whole zoom that fits. Play
steps one frame per display refresh; the arrows and the scrubber seek through
checkpoints, so a frame sought is the frame stepping straight there drew. The
view picker draws the frame on screen again under another debug view.

A view that draws the frame a second time from elsewhere, such as `cull`,
shows an observer checkbox. While it is on, the timeline pauses and the shown
frame is drawn again from a camera the person flies, the frame's own camera
left where its path put it: W, A, S and D move, Q and E go down and up,
dragging with the right button looks round, and the wheel scales the speed,
which starts at crossing the scene's box corner to corner in 20 seconds.
autana's `fly_motion` flies it. Each move costs one engine frame, from a
checkpoint held just before the shown frame. Unticked, or with another view
picked, the frame is the straight run's again.

## Console

Window > Console (Ctrl+Shift+C) shows what Loupe and what it runs said: its
own messages, the engine's output, the tools it starts (`build_pack.py`,
`bake.py`, `git`), the update check, and anything written to stderr. It docks
beside the frame debugger, or floats over the start panel.

| Control | Does |
|---|---|
| Filter | keeps the lines whose text or source holds it; `a,b` keeps either, `-a` drops `a` |
| Clear | empties the panel; the log file keeps every line |
| Copy | puts the lines shown on the clipboard; a line's right-click menu copies that one |
| Time | shows each line's time of day |

Warnings are yellow, errors red. At the bottom the panel follows new lines;
scrolled up, it stays put. It keeps the last 10,000 lines. The window also
writes every line to `loupe.log` in the settings folder
(`%APPDATA%\autana\loupe` or `~/.local/share/autana/loupe`), the last run's
kept as `loupe-prev.log`. On Windows `loupe` opens no console window; run from
a terminal, its errors, usage and `--version` still print there.
