var e=e=>{switch(e){case`index`:return`direction: down

Developer: {
  label: "Developer"
  shape: c4-person
}
Ci: {
  label: "GitHub Actions"
}
Loupe: {
  label: "Loupe"
}
Autana: {
  label: "autana"
}

Developer -> Loupe: "opens scenes, edits, renders"
Loupe -> Autana: "draws with its code, builds its packs"
Ci -> Loupe: "builds, tests, releases"
`;case`containers`:return`direction: down

Developer: {
  label: "Developer"
  shape: c4-person
}
Ci: {
  label: "GitHub Actions"
}
Loupe: {
  label: "Loupe"

  Loupe: {
    label: "Loupe"
  }
}
Autana: {
  label: "autana"
}

Developer -> Loupe.Loupe: "opens scenes, edits, renders"
Ci -> Loupe.Loupe: "builds, tests, releases"
Loupe.Loupe -> Autana: "draws with its code, builds its packs"
`;case`loupe`:return`direction: down

LoupeLoupe: {
  label: "Loupe"

  C_path: {
    label: "path"
  }
  C_storyboard: {
    label: "storyboard"
  }
  C_window: {
    label: "window"
  }
  C_editor: {
    label: "editor"
  }
  C_engine: {
    label: "engine"
  }
  C_update: {
    label: "update"
  }
  C_src: {
    label: "src"
  }
}

LoupeLoupe.C_editor -> LoupeLoupe.C_src
LoupeLoupe.C_window -> LoupeLoupe.C_editor
LoupeLoupe.C_engine -> LoupeLoupe.C_src
LoupeLoupe.C_window -> LoupeLoupe.C_engine
LoupeLoupe.C_path -> LoupeLoupe.C_src
LoupeLoupe.C_src -> LoupeLoupe.C_path
LoupeLoupe.C_src -> LoupeLoupe.C_window
LoupeLoupe.C_update -> LoupeLoupe.C_src
LoupeLoupe.C_storyboard -> LoupeLoupe.C_window
LoupeLoupe.C_window -> LoupeLoupe.C_update
`;case`loupe_all`:return`direction: down

LoupeLoupe: {
  label: "Loupe"

  C_tests: {
    label: "tests"
  }
  C_path: {
    label: "path"
  }
  C_storyboard: {
    label: "storyboard"
  }
  C_window: {
    label: "window"
  }
  C_editor: {
    label: "editor"
  }
  C_update: {
    label: "update"
  }
  C_engine: {
    label: "engine"
  }
  C_src: {
    label: "src"
  }
}

LoupeLoupe.C_editor -> LoupeLoupe.C_engine
LoupeLoupe.C_editor -> LoupeLoupe.C_src
LoupeLoupe.C_tests -> LoupeLoupe.C_editor
LoupeLoupe.C_window -> LoupeLoupe.C_editor
LoupeLoupe.C_engine -> LoupeLoupe.C_src
LoupeLoupe.C_path -> LoupeLoupe.C_engine
LoupeLoupe.C_src -> LoupeLoupe.C_engine
LoupeLoupe.C_tests -> LoupeLoupe.C_engine
LoupeLoupe.C_window -> LoupeLoupe.C_engine
LoupeLoupe.C_path -> LoupeLoupe.C_src
LoupeLoupe.C_src -> LoupeLoupe.C_path
LoupeLoupe.C_tests -> LoupeLoupe.C_path
LoupeLoupe.C_src -> LoupeLoupe.C_window
LoupeLoupe.C_storyboard -> LoupeLoupe.C_src
LoupeLoupe.C_tests -> LoupeLoupe.C_src
LoupeLoupe.C_update -> LoupeLoupe.C_src
LoupeLoupe.C_window -> LoupeLoupe.C_src
LoupeLoupe.C_storyboard -> LoupeLoupe.C_window
LoupeLoupe.C_tests -> LoupeLoupe.C_storyboard
LoupeLoupe.C_tests -> LoupeLoupe.C_update
LoupeLoupe.C_tests -> LoupeLoupe.C_window
LoupeLoupe.C_window -> LoupeLoupe.C_update
`;case`c_editor`:return`direction: down

LoupeLoupeC_window: {
  label: "window"
}
LoupeLoupeC_editor: {
  label: "editor"

  M_scene_tool: {
    label: "scene_tool"
  }
  M_doc: {
    label: "doc"
  }
  M_camera_path: {
    label: "camera_path"
  }
  M_placement: {
    label: "placement"
  }
}
LoupeLoupeC_src: {
  label: "src"
}
LoupeLoupeC_engine: {
  label: "engine"
}

LoupeLoupeC_window -> LoupeLoupeC_editor.M_scene_tool
LoupeLoupeC_editor.M_camera_path -> LoupeLoupeC_editor.M_placement
LoupeLoupeC_editor.M_doc -> LoupeLoupeC_editor.M_camera_path
LoupeLoupeC_editor.M_placement -> LoupeLoupeC_editor.M_doc
LoupeLoupeC_editor.M_scene_tool -> LoupeLoupeC_editor.M_doc
LoupeLoupeC_editor.M_placement -> LoupeLoupeC_engine
LoupeLoupeC_editor.M_scene_tool -> LoupeLoupeC_src
`;case`c_engine`:return`direction: down

LoupeLoupeC_editor: {
  label: "editor"
}
LoupeLoupeC_path: {
  label: "path"
}
LoupeLoupeC_src: {
  label: "src"
}
LoupeLoupeC_window: {
  label: "window"
}
LoupeLoupeC_engine: {
  label: "engine"

  M_frames: {
    label: "frames"
  }
  M_path_curve: {
    label: "path_curve"
  }
  M_engine_frame: {
    label: "engine_frame"
  }
  M_autana: {
    label: "autana"
  }
  M_pose: {
    label: "pose"
  }
}

LoupeLoupeC_editor -> LoupeLoupeC_engine.M_frames
LoupeLoupeC_editor -> LoupeLoupeC_engine.M_path_curve
LoupeLoupeC_path -> LoupeLoupeC_engine.M_frames
LoupeLoupeC_src -> LoupeLoupeC_engine.M_autana
LoupeLoupeC_src -> LoupeLoupeC_engine.M_frames
LoupeLoupeC_window -> LoupeLoupeC_engine.M_engine_frame
LoupeLoupeC_window -> LoupeLoupeC_engine.M_frames
LoupeLoupeC_engine.M_path_curve -> LoupeLoupeC_engine.M_autana
LoupeLoupeC_engine.M_engine_frame -> LoupeLoupeC_engine.M_pose
LoupeLoupeC_engine.M_frames -> LoupeLoupeC_engine.M_pose
LoupeLoupeC_engine.M_path_curve -> LoupeLoupeC_engine.M_pose
LoupeLoupeC_engine.M_frames -> LoupeLoupeC_src
`;case`c_path`:return`direction: down

LoupeLoupeC_src: {
  label: "src"
}
LoupeLoupeC_path: {
  label: "path"

  M_path: {
    label: "path"
  }
}
LoupeLoupeC_engine: {
  label: "engine"
}

LoupeLoupeC_src -> LoupeLoupeC_path.M_path
LoupeLoupeC_path.M_path -> LoupeLoupeC_engine
LoupeLoupeC_path.M_path -> LoupeLoupeC_src
`;case`c_src`:return`direction: down

LoupeLoupeC_editor: {
  label: "editor"
}
LoupeLoupeC_engine: {
  label: "engine"
}
LoupeLoupeC_storyboard: {
  label: "storyboard"
}
LoupeLoupeC_update: {
  label: "update"
}
LoupeLoupeC_src: {
  label: "src"

  M_main: {
    label: "main"
  }
  M_options: {
    label: "options"
  }
  M_scratch: {
    label: "scratch"
  }
  M_version: {
    label: "version"
  }
  M_library: {
    label: "library"
  }
  M_pack: {
    label: "pack"
  }
  M_tools: {
    label: "tools"
  }
  M_process: {
    label: "process"
  }
  M_paths: {
    label: "paths"
  }
}
LoupeLoupeC_path: {
  label: "path"
}
LoupeLoupeC_window: {
  label: "window"
}

LoupeLoupeC_editor -> LoupeLoupeC_src.M_tools
LoupeLoupeC_engine -> LoupeLoupeC_src.M_options
LoupeLoupeC_path -> LoupeLoupeC_src.M_pack
LoupeLoupeC_path -> LoupeLoupeC_src.M_scratch
LoupeLoupeC_storyboard -> LoupeLoupeC_src.M_options
LoupeLoupeC_update -> LoupeLoupeC_src.M_options
LoupeLoupeC_update -> LoupeLoupeC_src.M_scratch
LoupeLoupeC_update -> LoupeLoupeC_src.M_version
LoupeLoupeC_window -> LoupeLoupeC_src.M_library
LoupeLoupeC_window -> LoupeLoupeC_src.M_scratch
LoupeLoupeC_window -> LoupeLoupeC_src.M_version
LoupeLoupeC_src.M_library -> LoupeLoupeC_src.M_pack
LoupeLoupeC_src.M_options -> LoupeLoupeC_src.M_pack
LoupeLoupeC_src.M_pack -> LoupeLoupeC_src.M_tools
LoupeLoupeC_src.M_process -> LoupeLoupeC_src.M_paths
LoupeLoupeC_src.M_tools -> LoupeLoupeC_src.M_process
LoupeLoupeC_src.M_main -> LoupeLoupeC_path
LoupeLoupeC_src.M_main -> LoupeLoupeC_window
LoupeLoupeC_src.M_pack -> LoupeLoupeC_engine
`;case`c_storyboard`:return`direction: down

LoupeLoupeC_storyboard: {
  label: "storyboard"

  M_main: {
    label: "main"
  }
  M_runner: {
    label: "runner"
  }
  M_recipe: {
    label: "recipe"
  }
  C_config: {
    label: "storyboard/config"
  }
}
LoupeLoupeC_window: {
  label: "window"
}
LoupeLoupeC_src: {
  label: "src"
}

LoupeLoupeC_storyboard.M_main -> LoupeLoupeC_storyboard.M_runner
LoupeLoupeC_storyboard.M_runner -> LoupeLoupeC_storyboard.M_recipe
LoupeLoupeC_storyboard.M_recipe -> LoupeLoupeC_src
LoupeLoupeC_storyboard.M_runner -> LoupeLoupeC_window
`;case`c_tests`:return`direction: down

`;case`c_update`:return`direction: down

LoupeLoupeC_window: {
  label: "window"
}
LoupeLoupeC_update: {
  label: "update"

  M_update: {
    label: "update"
  }
  M_sha256: {
    label: "sha256"
  }
}
LoupeLoupeC_src: {
  label: "src"
}

LoupeLoupeC_window -> LoupeLoupeC_update.M_update
LoupeLoupeC_update.M_update -> LoupeLoupeC_update.M_sha256
LoupeLoupeC_update.M_update -> LoupeLoupeC_src
`;case`c_window`:return`direction: down

LoupeLoupeC_src: {
  label: "src"
}
LoupeLoupeC_storyboard: {
  label: "storyboard"
}
LoupeLoupeC_window: {
  label: "window"

  M_run_window: {
    label: "run_window"
  }
  M_editor: {
    label: "editor"
  }
  M_files: {
    label: "files"
  }
  M_frame_texture: {
    label: "frame_texture"
  }
  M_observer: {
    label: "observer"
  }
  M_path_editor: {
    label: "path_editor"
  }
  M_gpu: {
    label: "gpu"
  }
  M_panels: {
    label: "panels"
  }
  M_fonts: {
    label: "fonts"
  }
}
LoupeLoupeC_update: {
  label: "update"
}
LoupeLoupeC_engine: {
  label: "engine"
}
LoupeLoupeC_editor: {
  label: "editor"
}

LoupeLoupeC_src -> LoupeLoupeC_window.M_run_window
LoupeLoupeC_storyboard -> LoupeLoupeC_window.M_run_window
LoupeLoupeC_window.M_editor -> LoupeLoupeC_window.M_path_editor
LoupeLoupeC_window.M_run_window -> LoupeLoupeC_window.M_editor
LoupeLoupeC_window.M_run_window -> LoupeLoupeC_window.M_files
LoupeLoupeC_window.M_gpu -> LoupeLoupeC_window.M_fonts
LoupeLoupeC_window.M_frame_texture -> LoupeLoupeC_window.M_gpu
LoupeLoupeC_window.M_run_window -> LoupeLoupeC_window.M_frame_texture
LoupeLoupeC_window.M_run_window -> LoupeLoupeC_window.M_observer
LoupeLoupeC_window.M_path_editor -> LoupeLoupeC_window.M_panels
LoupeLoupeC_window.M_files -> LoupeLoupeC_src
LoupeLoupeC_window.M_frame_texture -> LoupeLoupeC_engine
LoupeLoupeC_window.M_observer -> LoupeLoupeC_engine
LoupeLoupeC_window.M_panels -> LoupeLoupeC_src
LoupeLoupeC_window.M_path_editor -> LoupeLoupeC_editor
LoupeLoupeC_window.M_run_window -> LoupeLoupeC_update
`;case`c_storyboard_config`:return`direction: down

LoupeLoupeC_storyboardC_config: {
  label: "storyboard/config"

  M_imgui_config: {
    label: "imgui_config"
  }
}
`;default:throw Error(`Unknown viewId: `+e)}};export{e as d2Source};