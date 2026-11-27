var e=e=>{switch(e){case`index`:return`---
title: "Context"
---
graph TB
  Developer@{ icon: "fa:user", shape: rounded, label: "Developer" }
  Ci@{ shape: rectangle, label: "GitHub Actions" }
  Loupe@{ shape: rectangle, label: "Loupe" }
  Autana@{ shape: rectangle, label: "autana" }
  Developer -. "\`opens scenes, edits, renders\`" .-> Loupe
  Loupe -. "\`draws with its code, builds its packs\`" .-> Autana
  Ci -. "\`builds, tests, releases\`" .-> Loupe
`;case`containers`:return`---
title: "Containers"
---
graph TB
  Developer@{ icon: "fa:user", shape: rounded, label: "Developer" }
  Ci@{ shape: rectangle, label: "GitHub Actions" }
  subgraph Loupe["\`Loupe\`"]
    Loupe.Loupe@{ shape: rectangle, label: "Loupe" }
  end
  Autana@{ shape: rectangle, label: "autana" }
  Developer -. "\`opens scenes, edits, renders\`" .-> Loupe.Loupe
  Ci -. "\`builds, tests, releases\`" .-> Loupe.Loupe
  Loupe.Loupe -. "\`draws with its code, builds its packs\`" .-> Autana
`;case`loupe`:return`---
title: "Loupe: components"
---
graph TB
  subgraph LoupeLoupe["\`Loupe\`"]
    LoupeLoupe.C_path@{ shape: rectangle, label: "path" }
    LoupeLoupe.C_storyboard@{ shape: rectangle, label: "storyboard" }
    LoupeLoupe.C_window@{ shape: rectangle, label: "window" }
    LoupeLoupe.C_editor@{ shape: rectangle, label: "editor" }
    LoupeLoupe.C_engine@{ shape: rectangle, label: "engine" }
    LoupeLoupe.C_log@{ shape: rectangle, label: "log" }
    LoupeLoupe.C_update@{ shape: rectangle, label: "update" }
    LoupeLoupe.C_src@{ shape: rectangle, label: "src" }
  end
  LoupeLoupe.C_editor -.-> LoupeLoupe.C_src
  LoupeLoupe.C_window -.-> LoupeLoupe.C_editor
  LoupeLoupe.C_engine -.-> LoupeLoupe.C_src
  LoupeLoupe.C_window -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_window -.-> LoupeLoupe.C_log
  LoupeLoupe.C_path -.-> LoupeLoupe.C_src
  LoupeLoupe.C_src -.-> LoupeLoupe.C_path
  LoupeLoupe.C_src -.-> LoupeLoupe.C_window
  LoupeLoupe.C_update -.-> LoupeLoupe.C_src
  LoupeLoupe.C_storyboard -.-> LoupeLoupe.C_window
  LoupeLoupe.C_window -.-> LoupeLoupe.C_update
`;case`loupe_all`:return`---
title: "Loupe: components, with tests and tools"
---
graph TB
  subgraph LoupeLoupe["\`Loupe\`"]
    LoupeLoupe.C_tests@{ shape: rectangle, label: "tests" }
    LoupeLoupe.C_path@{ shape: rectangle, label: "path" }
    LoupeLoupe.C_storyboard@{ shape: rectangle, label: "storyboard" }
    LoupeLoupe.C_window@{ shape: rectangle, label: "window" }
    LoupeLoupe.C_editor@{ shape: rectangle, label: "editor" }
    LoupeLoupe.C_update@{ shape: rectangle, label: "update" }
    LoupeLoupe.C_engine@{ shape: rectangle, label: "engine" }
    LoupeLoupe.C_src@{ shape: rectangle, label: "src" }
    LoupeLoupe.C_log@{ shape: rectangle, label: "log" }
  end
  LoupeLoupe.C_editor -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_editor -.-> LoupeLoupe.C_src
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_editor
  LoupeLoupe.C_window -.-> LoupeLoupe.C_editor
  LoupeLoupe.C_engine -.-> LoupeLoupe.C_log
  LoupeLoupe.C_engine -.-> LoupeLoupe.C_src
  LoupeLoupe.C_path -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_src -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_window -.-> LoupeLoupe.C_engine
  LoupeLoupe.C_path -.-> LoupeLoupe.C_log
  LoupeLoupe.C_src -.-> LoupeLoupe.C_log
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_log
  LoupeLoupe.C_update -.-> LoupeLoupe.C_log
  LoupeLoupe.C_window -.-> LoupeLoupe.C_log
  LoupeLoupe.C_path -.-> LoupeLoupe.C_src
  LoupeLoupe.C_src -.-> LoupeLoupe.C_path
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_path
  LoupeLoupe.C_src -.-> LoupeLoupe.C_window
  LoupeLoupe.C_storyboard -.-> LoupeLoupe.C_src
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_src
  LoupeLoupe.C_update -.-> LoupeLoupe.C_src
  LoupeLoupe.C_window -.-> LoupeLoupe.C_src
  LoupeLoupe.C_storyboard -.-> LoupeLoupe.C_window
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_storyboard
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_update
  LoupeLoupe.C_tests -.-> LoupeLoupe.C_window
  LoupeLoupe.C_window -.-> LoupeLoupe.C_update
`;case`c_editor`:return`---
title: "editor"
---
graph TB
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  subgraph LoupeLoupeC_editor["\`editor\`"]
    LoupeLoupeC_editor.M_scene_tool@{ shape: rectangle, label: "scene_tool" }
    LoupeLoupeC_editor.M_doc@{ shape: rectangle, label: "doc" }
    LoupeLoupeC_editor.M_camera_path@{ shape: rectangle, label: "camera_path" }
    LoupeLoupeC_editor.M_placement@{ shape: rectangle, label: "placement" }
  end
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_engine@{ shape: rectangle, label: "engine" }
  LoupeLoupeC_window -.-> LoupeLoupeC_editor.M_scene_tool
  LoupeLoupeC_editor.M_camera_path -.-> LoupeLoupeC_editor.M_placement
  LoupeLoupeC_editor.M_doc -.-> LoupeLoupeC_editor.M_camera_path
  LoupeLoupeC_editor.M_placement -.-> LoupeLoupeC_editor.M_doc
  LoupeLoupeC_editor.M_scene_tool -.-> LoupeLoupeC_editor.M_doc
  LoupeLoupeC_editor.M_placement -.-> LoupeLoupeC_engine
  LoupeLoupeC_editor.M_scene_tool -.-> LoupeLoupeC_src
`;case`c_engine`:return`---
title: "engine"
---
graph TB
  LoupeLoupeC_editor@{ shape: rectangle, label: "editor" }
  LoupeLoupeC_path@{ shape: rectangle, label: "path" }
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  subgraph LoupeLoupeC_engine["\`engine\`"]
    LoupeLoupeC_engine.M_frames@{ shape: rectangle, label: "frames" }
    LoupeLoupeC_engine.M_path_curve@{ shape: rectangle, label: "path_curve" }
    LoupeLoupeC_engine.M_engine_frame@{ shape: rectangle, label: "engine_frame" }
    LoupeLoupeC_engine.M_autana@{ shape: rectangle, label: "autana" }
    LoupeLoupeC_engine.M_pose@{ shape: rectangle, label: "pose" }
  end
  LoupeLoupeC_log@{ shape: rectangle, label: "log" }
  LoupeLoupeC_editor -.-> LoupeLoupeC_engine.M_frames
  LoupeLoupeC_editor -.-> LoupeLoupeC_engine.M_path_curve
  LoupeLoupeC_path -.-> LoupeLoupeC_engine.M_frames
  LoupeLoupeC_src -.-> LoupeLoupeC_engine.M_autana
  LoupeLoupeC_src -.-> LoupeLoupeC_engine.M_frames
  LoupeLoupeC_window -.-> LoupeLoupeC_engine.M_engine_frame
  LoupeLoupeC_window -.-> LoupeLoupeC_engine.M_frames
  LoupeLoupeC_engine.M_path_curve -.-> LoupeLoupeC_engine.M_autana
  LoupeLoupeC_engine.M_engine_frame -.-> LoupeLoupeC_engine.M_pose
  LoupeLoupeC_engine.M_frames -.-> LoupeLoupeC_engine.M_pose
  LoupeLoupeC_engine.M_path_curve -.-> LoupeLoupeC_engine.M_pose
  LoupeLoupeC_engine.M_frames -.-> LoupeLoupeC_log
  LoupeLoupeC_engine.M_frames -.-> LoupeLoupeC_src
`;case`c_log`:return`---
title: "log"
---
graph TB
  LoupeLoupeC_engine@{ shape: rectangle, label: "engine" }
  LoupeLoupeC_path@{ shape: rectangle, label: "path" }
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_update@{ shape: rectangle, label: "update" }
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  subgraph LoupeLoupeC_log["\`log\`"]
    LoupeLoupeC_log.M_log@{ shape: rectangle, label: "log" }
  end
  LoupeLoupeC_engine -.-> LoupeLoupeC_log.M_log
  LoupeLoupeC_path -.-> LoupeLoupeC_log.M_log
  LoupeLoupeC_src -.-> LoupeLoupeC_log.M_log
  LoupeLoupeC_update -.-> LoupeLoupeC_log.M_log
  LoupeLoupeC_window -.-> LoupeLoupeC_log.M_log
`;case`c_path`:return`---
title: "path"
---
graph TB
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  subgraph LoupeLoupeC_path["\`path\`"]
    LoupeLoupeC_path.M_path@{ shape: rectangle, label: "path" }
  end
  LoupeLoupeC_engine@{ shape: rectangle, label: "engine" }
  LoupeLoupeC_log@{ shape: rectangle, label: "log" }
  LoupeLoupeC_src -.-> LoupeLoupeC_path.M_path
  LoupeLoupeC_path.M_path -.-> LoupeLoupeC_engine
  LoupeLoupeC_path.M_path -.-> LoupeLoupeC_log
  LoupeLoupeC_path.M_path -.-> LoupeLoupeC_src
`;case`c_src`:return`---
title: "src"
---
graph TB
  LoupeLoupeC_editor@{ shape: rectangle, label: "editor" }
  LoupeLoupeC_engine@{ shape: rectangle, label: "engine" }
  LoupeLoupeC_storyboard@{ shape: rectangle, label: "storyboard" }
  LoupeLoupeC_update@{ shape: rectangle, label: "update" }
  subgraph LoupeLoupeC_src["\`src\`"]
    LoupeLoupeC_src.M_main@{ shape: rectangle, label: "main" }
    LoupeLoupeC_src.M_options@{ shape: rectangle, label: "options" }
    LoupeLoupeC_src.M_scratch@{ shape: rectangle, label: "scratch" }
    LoupeLoupeC_src.M_version@{ shape: rectangle, label: "version" }
    LoupeLoupeC_src.M_library@{ shape: rectangle, label: "library" }
    LoupeLoupeC_src.M_pack@{ shape: rectangle, label: "pack" }
    LoupeLoupeC_src.M_tools@{ shape: rectangle, label: "tools" }
    LoupeLoupeC_src.M_process@{ shape: rectangle, label: "process" }
    LoupeLoupeC_src.M_paths@{ shape: rectangle, label: "paths" }
  end
  LoupeLoupeC_path@{ shape: rectangle, label: "path" }
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  LoupeLoupeC_log@{ shape: rectangle, label: "log" }
  LoupeLoupeC_editor -.-> LoupeLoupeC_src.M_tools
  LoupeLoupeC_engine -.-> LoupeLoupeC_src.M_options
  LoupeLoupeC_path -.-> LoupeLoupeC_src.M_pack
  LoupeLoupeC_path -.-> LoupeLoupeC_src.M_scratch
  LoupeLoupeC_storyboard -.-> LoupeLoupeC_src.M_options
  LoupeLoupeC_update -.-> LoupeLoupeC_src.M_options
  LoupeLoupeC_update -.-> LoupeLoupeC_src.M_scratch
  LoupeLoupeC_update -.-> LoupeLoupeC_src.M_version
  LoupeLoupeC_window -.-> LoupeLoupeC_src.M_library
  LoupeLoupeC_window -.-> LoupeLoupeC_src.M_scratch
  LoupeLoupeC_window -.-> LoupeLoupeC_src.M_version
  LoupeLoupeC_src.M_library -.-> LoupeLoupeC_src.M_pack
  LoupeLoupeC_src.M_options -.-> LoupeLoupeC_src.M_pack
  LoupeLoupeC_src.M_pack -.-> LoupeLoupeC_src.M_tools
  LoupeLoupeC_src.M_process -.-> LoupeLoupeC_src.M_paths
  LoupeLoupeC_src.M_tools -.-> LoupeLoupeC_src.M_process
  LoupeLoupeC_src.M_main -.-> LoupeLoupeC_path
  LoupeLoupeC_src.M_main -.-> LoupeLoupeC_window
  LoupeLoupeC_src.M_pack -.-> LoupeLoupeC_engine
  LoupeLoupeC_src.M_process -.-> LoupeLoupeC_log
`;case`c_storyboard`:return`---
title: "storyboard"
---
graph TB
  subgraph LoupeLoupeC_storyboard["\`storyboard\`"]
    LoupeLoupeC_storyboard.M_main@{ shape: rectangle, label: "main" }
    LoupeLoupeC_storyboard.M_runner@{ shape: rectangle, label: "runner" }
    LoupeLoupeC_storyboard.M_recipe@{ shape: rectangle, label: "recipe" }
    LoupeLoupeC_storyboard.C_config@{ shape: rectangle, label: "storyboard/config" }
  end
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_storyboard.M_main -.-> LoupeLoupeC_storyboard.M_runner
  LoupeLoupeC_storyboard.M_runner -.-> LoupeLoupeC_storyboard.M_recipe
  LoupeLoupeC_storyboard.M_recipe -.-> LoupeLoupeC_src
  LoupeLoupeC_storyboard.M_runner -.-> LoupeLoupeC_window
`;case`c_tests`:return`---
title: "tests"
---
graph TB
`;case`c_update`:return`---
title: "update"
---
graph TB
  LoupeLoupeC_window@{ shape: rectangle, label: "window" }
  subgraph LoupeLoupeC_update["\`update\`"]
    LoupeLoupeC_update.M_update@{ shape: rectangle, label: "update" }
    LoupeLoupeC_update.M_sha256@{ shape: rectangle, label: "sha256" }
  end
  LoupeLoupeC_log@{ shape: rectangle, label: "log" }
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_window -.-> LoupeLoupeC_update.M_update
  LoupeLoupeC_update.M_update -.-> LoupeLoupeC_update.M_sha256
  LoupeLoupeC_update.M_update -.-> LoupeLoupeC_log
  LoupeLoupeC_update.M_update -.-> LoupeLoupeC_src
`;case`c_window`:return`---
title: "window"
---
graph TB
  LoupeLoupeC_src@{ shape: rectangle, label: "src" }
  LoupeLoupeC_storyboard@{ shape: rectangle, label: "storyboard" }
  subgraph LoupeLoupeC_window["\`window\`"]
    LoupeLoupeC_window.M_run_window@{ shape: rectangle, label: "run_window" }
    LoupeLoupeC_window.M_console@{ shape: rectangle, label: "console" }
    LoupeLoupeC_window.M_editor@{ shape: rectangle, label: "editor" }
    LoupeLoupeC_window.M_files@{ shape: rectangle, label: "files" }
    LoupeLoupeC_window.M_frame_texture@{ shape: rectangle, label: "frame_texture" }
    LoupeLoupeC_window.M_observer@{ shape: rectangle, label: "observer" }
    LoupeLoupeC_window.M_path_editor@{ shape: rectangle, label: "path_editor" }
    LoupeLoupeC_window.M_gpu@{ shape: rectangle, label: "gpu" }
    LoupeLoupeC_window.M_panels@{ shape: rectangle, label: "panels" }
    LoupeLoupeC_window.M_fonts@{ shape: rectangle, label: "fonts" }
  end
  LoupeLoupeC_update@{ shape: rectangle, label: "update" }
  LoupeLoupeC_engine@{ shape: rectangle, label: "engine" }
  LoupeLoupeC_editor@{ shape: rectangle, label: "editor" }
  LoupeLoupeC_log@{ shape: rectangle, label: "log" }
  LoupeLoupeC_src -.-> LoupeLoupeC_window.M_run_window
  LoupeLoupeC_storyboard -.-> LoupeLoupeC_window.M_run_window
  LoupeLoupeC_window.M_console -.-> LoupeLoupeC_window.M_fonts
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_window.M_console
  LoupeLoupeC_window.M_editor -.-> LoupeLoupeC_window.M_path_editor
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_window.M_editor
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_window.M_files
  LoupeLoupeC_window.M_gpu -.-> LoupeLoupeC_window.M_fonts
  LoupeLoupeC_window.M_frame_texture -.-> LoupeLoupeC_window.M_gpu
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_window.M_frame_texture
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_window.M_observer
  LoupeLoupeC_window.M_path_editor -.-> LoupeLoupeC_window.M_panels
  LoupeLoupeC_window.M_console -.-> LoupeLoupeC_log
  LoupeLoupeC_window.M_files -.-> LoupeLoupeC_src
  LoupeLoupeC_window.M_frame_texture -.-> LoupeLoupeC_engine
  LoupeLoupeC_window.M_gpu -.-> LoupeLoupeC_log
  LoupeLoupeC_window.M_observer -.-> LoupeLoupeC_engine
  LoupeLoupeC_window.M_panels -.-> LoupeLoupeC_src
  LoupeLoupeC_window.M_path_editor -.-> LoupeLoupeC_editor
  LoupeLoupeC_window.M_run_window -.-> LoupeLoupeC_update
`;case`c_storyboard_config`:return`---
title: "config"
---
graph TB
  subgraph LoupeLoupeC_storyboardC_config["\`storyboard/config\`"]
    LoupeLoupeC_storyboardC_config.M_imgui_config@{ shape: rectangle, label: "imgui_config" }
  end
`;default:throw Error(`Unknown viewId: `+e)}};export{e as mmdSource};