# Tests

The preset's test step (`ctest --preset default`) builds the packs it opens with `third_party/autana/launcher/tools/r3d/build_pack.py`, then runs:

| Test | Checks |
|---|---|
| `parity_*` | `loupe` on the scene's `.apak` (and, with the pack tool, its `.scene.toml`) writes the same bytes as autana's scene viewer for the same arguments |
| `scene_toml_refused`, `compare_refused` | without the pack tool, a `.scene.toml` exits 1 and `--compare` exits 2, saying why |
| `wrong_version_refused` | a pack of another format version exits 1 with autana's reason |
| `reopen_*` | a scene opened again after another closed, in one process, draws what it drew opened first; a pack of the same name from another folder is read afresh, so the window switches scenes in one process |
| `path_flags` | `--frame`, `--compare` and `--b-args` parse only with what they need, and B takes `--b-args`' flags in place of A's |
| `path_timing` | a path is timed as [Frames and compares](../render/Frames-and-Compares.md) describes |
| `frame`, `frame_compare`, `frame_compare_head` | `--frame`'s PNG, alone and for both sides against a folder and against `HEAD`, holds the pixels a straight run to that frame writes; under other `--b-args`, B's differs from A's |
| `compare_same`, `compare_view` | a compare video's difference is nothing on every frame against the same pack, and something under another `--view`; skipped without ffmpeg, numpy or Pillow |
| `frame_past_end`, `compare_bad_rev`, `compare_no_scene` | exit 2 with the reason |
| `run_logged` | a tool loupe runs, from a folder named outside any one Windows code page, gets its arguments and paths intact and its output lands in the log and Loupe's (on Windows, through the UTF-8 code page manifest, `src/windows/utf8.manifest`) |
| `log` | the log keeps its latest lines in order and hands the Console only those after the last it took; only Loupe's own lines and stderr's echo to stderr; its file holds the lines from before it opened, the last run's set aside; a level is read from a program's words; a growing file is read by whole lines; what stderr gets while captured reaches the log |
| `console_recipe` | `tests/stories/console`, offscreen on lavapipe: the Console opens from its shortcut and shows, filtered and copied, the line a scene's opening logs; skipped without lavapipe |
| `library` | the start panel's samples, and a recent list kept most recent first, each once, at most 10 |
| `seek` | a seek back through a checkpoint draws the bytes stepping straight there drew |
| `thinning_*` | under a checkpoint budget the checkpoints stay within it, their interval doubles, and seeks still draw the straight bytes |
| `observe` | under `cull`, each observer move draws one frame and the same pose draws the same bytes; turned off, or left by a seek either way, a view pick or closing (the image `-o` writes), the frames are the straight run's |
| `observer_steer` | the observer starts at the speed that crosses the scene's box in 20 seconds; W, A, S, D, Q and E are its axes unless a text field has the keys, a right drag looks only when it began over the frame, and the wheel counts only over it |
| `view_switch` | a debug view picked mid-run, and kept through a seek back, draws what it drew when picked from the start |
| `window_smoke`, `window_start` | the window opens, on a scene and on the start panel, and draws 10 ticks offscreen on Mesa's lavapipe under the Khronos validation layer, with no validation error; skipped where either is missing |
| `doc_shots_current` | every picture of the window in `docs/shots` holds the pixels the recipes in `tests/stories/doc_shots` draw offscreen on lavapipe, each channel within one level (lavapipe compiles for the CPU it runs on), and none is left that no recipe draws; skipped without lavapipe, and with autana at any commit but the pinned one (the nightly's `main`), whose scenes can draw other pixels |
| `recipe` | a recipe reads as its steps, each with its line, its `loupe.*` key and its name; a recipe that is no list of maps, or a step that is not one `loupe.*` key and a name (a misspelt key, another key, none, two), is refused at its line |
| `version` | `--version` names the project version, Loupe's commit, the configured channel and the autana commit |
| `update` | SHA-256 on the standard's examples; GitHub's saved release answers give each system's download; which offers are newer; the setting; a check stages only a verified newer download and a local build never asks; a running copy, beside its samples and in `bin/`, is swapped for a staged one and says so at its next start |
| `build_id` | `--version`'s line for each channel, and a copy counts as installed only with the setup's uninstaller beside it |
| `editor_docs` | one undo stack, and whether anything is unsaved; only unbaked renderers place, each other object with the tool's reason; a placement's matrix turns as autana's `quatf_from_euler` and comes back from it, and a sheared, mirrored or zero-scale one is refused; the handles' matrices round trip and put a point where autana's pinhole does; a save sets only the fields that moved; the read's JSON parses, anything else is refused; the Outliner's tree by parent; a failed place is tried again the next frame |
| `place` | a placed object draws there on the shown frame at once and on frames sought or stepped to after; after the first, each place draws one engine frame; placed back, the shown frame and every frame are the straight run's; an unknown name is refused |
| `scene_save` | on a copy of the capybara scene, moving and scaling the capybara adds only its two lines and the read sees them; a baked renderer is refused, saying why, with the file unchanged and the change still unsaved |
| `camera_paths` | the curve matches `camera_keys.py`'s numbers, open and looped; key edits keep a loop closed, times in order and each key's id; adds and deletes follow their rules; a path that would not build, or holds more keys than a clip counts, is a fault; set-keys gets each kept key by its table in the file |
| `path_tour`, `path_lion` | on the Sponza pack, the camera stands where the curve puts it at every frame; flown on an edited path, the shown and later frames follow it while frames it does not reach stay the clip's; landed, every frame is the clip's again |
| `path_editor` | a key drag is one undo step and holds save; adds pick the new key; deletes refuse the first key and a two-key path; a path a bake reads waits for its warning; each camera flies and lands apart, once per change; the scrub's frame for a time |
| `path_save` | on a copy of the tour's keys with comments added, a moved eye, an added and a deleted key change only their own lines and the comments stay; a path that would not build is refused with the file unchanged; a delete undone after a save saves again with each comment above its key |
| `include_gate` | only `src/engine/autana.hpp` includes autana's engine headers; autana's packages, below the engine, are open to every file |

CI runs them on Linux and Windows at the pinned autana on every pull request,
and nightly against autana's `main`, keeping one issue open while that fails.

The pictures of the window in these docs, `docs/shots`, are drawn by Loupe
itself: each recipe in `tests/stories/doc_shots` opens its scene, clicks and
types its way to the state it shows, and keeps the window, or one panel of it
([Recipes](Recipes.md)). A change that alters one redraws them all, on Linux
with lavapipe as CI does (WSL on Windows):

```bash
cmake --build build/default --target doc_shots
```

When `doc_shots_current` fails, CI uploads the pictures it drew with the
renders, so they can be committed from there too. Another Mesa can draw other
pixels, so a runner image that updates it means one redraw.
