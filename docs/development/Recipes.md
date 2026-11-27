# Recipes

`loupe-storyboard` runs a recipe on the real window: Dear ImGui's test engine
gives each step to the panels as a person's clicks, keys and typing, so what
it shows is what that input does. It is built with `-DLOUPE_STORYBOARD=ON`, as
the `default` preset is, and never for the downloads (the README's License).
The pictures of the window in these docs are drawn this way ([Tests](Tests.md)).
The window stays hidden, so a run never shows on the desktop or takes the
focus, and it keeps its settings and recordings under `-o/config`, not yours.

```bash
build/default/loupe-storyboard --recipe tests/stories/doc_shots/sponza.yaml -o shots --scale 1.5
```

A recipe is a YAML list of steps. Each step is a map of one `loupe.` key,
its value the step's arguments, an optional `name` and an optional `check`;
any other map is refused at its line before the run starts. A step that fails
stops the run at its line, saying what the window showed, and
`loupe-storyboard` exits 1. A step that passes prints its line, its name and
how long it took. `${NAME}` in a step takes the value of `--set NAME=VALUE`.

```yaml
- name: The Sponza scene
  loupe.open: ../../../third_party/autana/launcher/demo/sponza/sponza.scene.toml --dt 100
- loupe.click: //Outliner/tour
- loupe.input: {ref: "//Inspector/eye/$$0", value: "500"}
- loupe.screenshot: {out: key-moved.png, crop: Viewport}
```

| Step | Does |
|---|---|
| `loupe.open: SCENE [flags]` | opens a scene as `loupe SCENE [flags]` would; SCENE relative to the recipe |
| `loupe.click: ITEM` | clicks an item: `//Window/label`, ImGui's id path, such as `//Outliner/tour` |
| `loupe.check: ITEM`, `loupe.uncheck: ITEM` | sets a checkbox, clicking it only when it is not so already |
| `loupe.input: {ref: ITEM, value: TEXT}` | Ctrl+clicks a field or slider and types TEXT, then Enter; a three-number field's first is `ITEM/$$0` |
| `loupe.key: CHORD` | presses a key or chord in ImGui's key names, such as `Escape` or `ctrl+z` |
| `loupe.hold: {key: KEY, ticks: N}` | holds a key for N ticks |
| `loupe.wait: N` | lets N ticks pass |
| `loupe.wait: {until: CHECKS, timeout_s: S}` | waits until the checks hold, failing after S s by the clock |
| `loupe.wait: {while: CHECKS, for_s: S}` | waits S s by the clock, failing on the first tick a check does not hold |
| `loupe.autana: ARGS` or `{args: ARGS, until: REGEX, timeout_s: S}` | runs `autana ARGS`, the autana on PATH that Record also runs, inside `--lock-token`'s lock when given; with `until`, again each second until what it printed matches, for S s (60 when left out). ARGS split at spaces, or a list of words. Its output goes to `autana-N.log` under `-o`, and a run that does not exit 0 fails the step |
| `loupe.screenshot: OUT.png` or `{out: OUT.png, crop: PANEL}` | moves the mouse off the window, lets 4 ticks settle, and writes the window, or the panel PANEL names, under `-o` |

Every tick stands for 1/30 s, whatever the machine, so a held key moves as far
anywhere. `--scale` is the panels' scale, 1 when left out.

## Checks

A `check` is a list of comparisons over what the window shows, checked once
the step has drawn; `loupe.wait` takes the same list. Each is one
`loupe.FIELD` (its value unused, `~`) and one of `eq ne lt le gt ge match`:

```yaml
- name: The live view's first frame
  loupe.wait: {until: [{loupe.phase: ~, eq: Shown}], timeout_s: 60}
  check: [{loupe.screen: ~, eq: live}, {loupe.refusal: ~, eq: ""}]
```

| Field | Is |
|---|---|
| `screen` | `start`, `scene` or `live` |
| `refusal` | what the start panel says did not open, empty when nothing |
| `boards` | the boards listed, their serials joined by a space |
| `phase` | where a live view's wait stands: `Starting`, `Autana`, `Linked`, `Reading`, `Shown`, `Ended` or `Refused` |
| `frames` | the live stream's frames so far |
| `frame_age_s` | seconds since the live stream's last new frame |
| `saving` | Record's state: `off`, `waiting` or `saving` |
| `saved_frames` | the frames Record has written |
| `recording` | the file Record writes, under `-o/config/recordings` |
| `recording_bytes` | its size |

A number compares as a number; text by `eq`, `ne` and `match`, a regular
expression found anywhere in it.

## A board's live view

`tests/stories/attach.yaml` drives a board end to end, so it runs only where
the board is free to it, with the board's development firmware built from the
autana Loupe pins. Its `loupe.autana` steps put the board on a scene, then it
lists the boards, presses View, sees frames keep coming, records a stretch and
stops:

```bash
build/default/loupe-storyboard --recipe tests/stories/attach.yaml --set BOARD=<serial> -o attach
```
