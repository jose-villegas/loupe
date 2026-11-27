# Recipes

`loupe-storyboard` runs a recipe on the real window: Dear ImGui's test engine
gives each step to the panels as a person's clicks, keys and typing, so what
it shows is what that input does. It is built with `-DLOUPE_STORYBOARD=ON`, as
the `default` preset is, and never for the downloads (the README's License).
The pictures of the window in these docs are drawn this way ([Tests](Tests.md)).

```bash
build/default/loupe-storyboard --recipe tests/stories/doc_shots/sponza.yaml -o shots --scale 1.5
```

A recipe is a YAML list of steps. Each step is a map of one `loupe.` key,
its value the step's arguments, and an optional `name`; any other map is
refused at its line before the run starts. A step that fails stops the run at
its line, and `loupe-storyboard` exits 1.

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
| `loupe.clipboard: TEXT` | fails unless the clipboard contains TEXT, as the Console's Copy leaves it |
| `loupe.screenshot: OUT.png` or `{out: OUT.png, crop: PANEL}` | moves the mouse off the window, lets 4 ticks settle, and writes the window, or the panel PANEL names, under `-o` |

Every tick stands for 1/30 s, whatever the machine, so a held key moves as far
anywhere. `--scale` is the panels' scale, 1 when left out.
