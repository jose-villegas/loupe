# Open a scene

```bash
build/default/loupe third_party/autana/launcher/demo/sponza/sponza.scene.toml
```

Run without a scene, the window opens on a start panel: the sample packs beside
`loupe` (`scenes/`, or `../share/loupe/scenes/` when installed), the last 10
scenes or recordings opened, and Open... for any other. A scene picked there
opens in this window with the scene's own defaults. Scenes, in a scene's window,
closes the scene and goes back to the panel, ending a live board first
([View a board](../board/View-a-Board.md)). The recent list is `recent.txt` in
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
