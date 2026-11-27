var e=e=>{switch(e){case`index`:return`@startuml
title "Context"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam person<<Developer>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<Ci>>{
  BackgroundColor #737373
  FontColor #fafafa
  BorderColor #525252
}
skinparam rectangle<<Loupe>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<Autana>>{
  BackgroundColor #737373
  FontColor #fafafa
  BorderColor #525252
}
person "==Developer\\n<size:10>[Person]</size>\\n\\nOpens a scene, steps through its frames, moves its objects and saves them back." <<Developer>> as Developer
rectangle "==GitHub Actions\\n<size:10>[Software system]</size>\\n\\nBuilds and tests Loupe on every pull request and publishes its releases." <<Ci>> as Ci
rectangle "==Loupe\\n\\nLoupe is a frame debugger and scene editor for the autana engine." <<Loupe>> as Loupe
rectangle "==autana\\n<size:10>[Software system]</size>\\n\\nThe engine whose drawing code Loupe runs, scenes and packs included." <<Autana>> as Autana

Developer .[#8D8D8D,thickness=2].> Loupe : <color:#8D8D8D>opens scenes, edits, renders
Loupe .[#8D8D8D,thickness=2].> Autana : <color:#8D8D8D>draws with its code, builds its packs
Ci .[#8D8D8D,thickness=2].> Loupe : <color:#8D8D8D>builds, tests, releases
@enduml
`;case`containers`:return`@startuml
title "Containers"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam person<<Developer>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<Ci>>{
  BackgroundColor #737373
  FontColor #fafafa
  BorderColor #525252
}
skinparam rectangle<<LoupeLoupe>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<Autana>>{
  BackgroundColor #737373
  FontColor #fafafa
  BorderColor #525252
}
person "==Developer\\n<size:10>[Person]</size>\\n\\nOpens a scene, steps through its frames, moves its objects and saves them back." <<Developer>> as Developer
rectangle "==GitHub Actions\\n<size:10>[Software system]</size>\\n\\nBuilds and tests Loupe on every pull request and publishes its releases." <<Ci>> as Ci
rectangle "Loupe" <<Loupe>> as Loupe {
  skinparam RectangleBorderColor<<Loupe>> #3b82f6
  skinparam RectangleFontColor<<Loupe>> #3b82f6
  skinparam RectangleBorderStyle<<Loupe>> dashed

  rectangle "==Loupe\\n<size:10>[C++20 · SDL3 · Vulkan · Dear ImGui]</size>\\n\\nOpens a scene in the window, or the window's start panel without one; with -o or --video, writes the scene's frames and exits; with --frame or..." <<LoupeLoupe>> as LoupeLoupe
}
rectangle "==autana\\n<size:10>[Software system]</size>\\n\\nThe engine whose drawing code Loupe runs, scenes and packs included." <<Autana>> as Autana

Developer .[#8D8D8D,thickness=2].> LoupeLoupe : <color:#8D8D8D>opens scenes, edits, renders
Ci .[#8D8D8D,thickness=2].> LoupeLoupe : <color:#8D8D8D>builds, tests, releases
LoupeLoupe .[#8D8D8D,thickness=2].> Autana : <color:#8D8D8D>draws with its code, builds its packs
@enduml
`;case`loupe`:return`@startuml
title "Loupe: components"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_path>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_storyboard>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_editor>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_update>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "Loupe" <<LoupeLoupe>> as LoupeLoupe {
  skinparam RectangleBorderColor<<LoupeLoupe>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupe>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupe>> dashed

  rectangle "==path\\n<size:10>[2 files · 404 lines]</size>\\n\\nA scene's camera path from the command line: one frame of it, or the whole path drawn by two sides, A and B, laid out as an A | B | difference video." <<LoupeLoupeC_path>> as LoupeLoupeC_path
  rectangle "==storyboard\\n<size:10>[6 files · 736 lines]</size>" <<LoupeLoupeC_storyboard>> as LoupeLoupeC_storyboard
  rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
  rectangle "==editor\\n<size:10>[8 files · 1052 lines]</size>" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor
  rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine
  rectangle "==update\\n<size:10>[4 files · 853 lines]</size>\\n\\nLoupe keeps itself current from its GitHub releases." <<LoupeLoupeC_update>> as LoupeLoupeC_update
  rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
}

LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_editor
LoupeLoupeC_engine .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_path
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_update .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_storyboard .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_update
@enduml
`;case`loupe_all`:return`@startuml
title "Loupe: components, with tests and tools"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_tests>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_path>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_storyboard>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_editor>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_update>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "Loupe" <<LoupeLoupe>> as LoupeLoupe {
  skinparam RectangleBorderColor<<LoupeLoupe>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupe>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupe>> dashed

  rectangle "==tests\\n<size:10>[0 files · 0 lines]</size>" <<LoupeLoupeC_tests>> as LoupeLoupeC_tests
  rectangle "==path\\n<size:10>[2 files · 404 lines]</size>\\n\\nA scene's camera path from the command line: one frame of it, or the whole path drawn by two sides, A and B, laid out as an A | B | difference video." <<LoupeLoupeC_path>> as LoupeLoupeC_path
  rectangle "==storyboard\\n<size:10>[6 files · 736 lines]</size>" <<LoupeLoupeC_storyboard>> as LoupeLoupeC_storyboard
  rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
  rectangle "==editor\\n<size:10>[8 files · 1052 lines]</size>" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor
  rectangle "==update\\n<size:10>[4 files · 853 lines]</size>\\n\\nLoupe keeps itself current from its GitHub releases." <<LoupeLoupeC_update>> as LoupeLoupeC_update
  rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine
  rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
}

LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_editor
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_editor
LoupeLoupeC_engine .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_path
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_path
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_storyboard .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_update .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_storyboard .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_storyboard
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_update
LoupeLoupeC_tests .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_update
@enduml
`;case`c_editor`:return`@startuml
title "editor"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_editorM_scene_tool>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_editorM_doc>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_editorM_camera_path>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_editorM_placement>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
rectangle "editor" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor {
  skinparam RectangleBorderColor<<LoupeLoupeC_editor>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_editor>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_editor>> dashed

  rectangle "==scene_tool\\n<size:10>[scene_tool.cpp, scene_tool.hpp]</size>\\n\\nThe editor's calls into autana's tools (tools.hpp's autana_tool): scene_edit.py reads a scene's objects and saves changed placements (comments and..." <<LoupeLoupeC_editorM_scene_tool>> as LoupeLoupeC_editorM_scene_tool
  rectangle "==doc\\n<size:10>[doc.cpp, doc.hpp]</size>\\n\\nWhat the editor holds and saves: the scene file's objects, each placed as the file authors it, and its cameras' paths." <<LoupeLoupeC_editorM_doc>> as LoupeLoupeC_editorM_doc
  rectangle "==camera_path\\n<size:10>[camera_path.cpp, camera_path.hpp]</size>\\n\\nA camera path as its .keys.toml writes it (autana's tools/anim/camera_keys.py reads it): keys with a time, the eye and the point it looks at." <<LoupeLoupeC_editorM_camera_path>> as LoupeLoupeC_editorM_camera_path
  rectangle "==placement\\n<size:10>[placement.cpp, placement.hpp]</size>\\n\\nA Placement both ways: to the engine's transform and the matrix the handles move, and back." <<LoupeLoupeC_editorM_placement>> as LoupeLoupeC_editorM_placement
}
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine

LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_editorM_scene_tool
LoupeLoupeC_editorM_camera_path .[#64748b,thickness=2].> LoupeLoupeC_editorM_placement
LoupeLoupeC_editorM_doc .[#64748b,thickness=2].> LoupeLoupeC_editorM_camera_path
LoupeLoupeC_editorM_placement .[#64748b,thickness=2].> LoupeLoupeC_editorM_doc
LoupeLoupeC_editorM_scene_tool .[#64748b,thickness=2].> LoupeLoupeC_editorM_doc
LoupeLoupeC_editorM_placement .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_editorM_scene_tool .[#64748b,thickness=2].> LoupeLoupeC_src
@enduml
`;case`c_engine`:return`@startuml
title "engine"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_editor>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_path>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_engineM_frames>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engineM_path_curve>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engineM_engine_frame>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engineM_autana>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engineM_pose>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
rectangle "==editor\\n<size:10>[8 files · 1052 lines]</size>" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor
rectangle "==path\\n<size:10>[2 files · 404 lines]</size>\\n\\nA scene's camera path from the command line: one frame of it, or the whole path drawn by two sides, A and B, laid out as an A | B | difference video." <<LoupeLoupeC_path>> as LoupeLoupeC_path
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
rectangle "engine" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine {
  skinparam RectangleBorderColor<<LoupeLoupeC_engine>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_engine>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_engine>> dashed

  rectangle "==frames\\n<size:10>[frames.cpp, frames.hpp]</size>\\n\\nGfx's frame from a provider, drawn by autana's host frame loop; step and exact seek." <<LoupeLoupeC_engineM_frames>> as LoupeLoupeC_engineM_frames
  rectangle "==path_curve\\n<size:10>[path_curve.cpp, path_curve.hpp]</size>\\n\\nA camera path as the clip autana bakes from it: the eye on a cubic curve (glTF CUBICSPLINE, an in-tangent, value and out-tangent per key) and the..." <<LoupeLoupeC_engineM_path_curve>> as LoupeLoupeC_engineM_path_curve
  rectangle "==engine_frame\\n<size:10>[engine_frame.cpp, engine_frame.hpp]</size>\\n\\nThe z mirror between autana's frame and glTF's." <<LoupeLoupeC_engineM_engine_frame>> as LoupeLoupeC_engineM_engine_frame
  rectangle "==autana\\n<size:10>[autana.hpp]</size>\\n\\nThe one place Loupe includes autana's headers (tests/include_gate.cmake)." <<LoupeLoupeC_engineM_autana>> as LoupeLoupeC_engineM_autana
  rectangle "==pose\\n<size:10>[pose.hpp]</size>\\n\\nAutana's packages (launcher/packages) that the poses Frames and the window hand each other need: math for the poses and the scene's box,..." <<LoupeLoupeC_engineM_pose>> as LoupeLoupeC_engineM_pose
}

LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_engineM_frames
LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_engineM_path_curve
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_engineM_frames
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_engineM_autana
LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_engineM_frames
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_engineM_engine_frame
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_engineM_frames
LoupeLoupeC_engineM_path_curve .[#64748b,thickness=2].> LoupeLoupeC_engineM_autana
LoupeLoupeC_engineM_engine_frame .[#64748b,thickness=2].> LoupeLoupeC_engineM_pose
LoupeLoupeC_engineM_frames .[#64748b,thickness=2].> LoupeLoupeC_engineM_pose
LoupeLoupeC_engineM_path_curve .[#64748b,thickness=2].> LoupeLoupeC_engineM_pose
LoupeLoupeC_engineM_frames .[#64748b,thickness=2].> LoupeLoupeC_src
@enduml
`;case`c_path`:return`@startuml
title "path"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_pathM_path>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
rectangle "path" <<LoupeLoupeC_path>> as LoupeLoupeC_path {
  skinparam RectangleBorderColor<<LoupeLoupeC_path>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_path>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_path>> dashed

  rectangle "==path\\n<size:10>[path.cpp, path.hpp]</size>\\n\\nA scene's camera path from the command line: one frame of it, or the whole path drawn by two sides, A and B, laid out as an A | B | difference video." <<LoupeLoupeC_pathM_path>> as LoupeLoupeC_pathM_path
}
rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine

LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_pathM_path
LoupeLoupeC_pathM_path .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_pathM_path .[#64748b,thickness=2].> LoupeLoupeC_src
@enduml
`;case`c_src`:return`@startuml
title "src"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_editor>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_storyboard>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_update>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_srcM_main>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_path>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_srcM_options>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_scratch>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_version>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_library>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_pack>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_tools>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_process>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_srcM_paths>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
rectangle "==editor\\n<size:10>[8 files · 1052 lines]</size>" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor
rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine
rectangle "==storyboard\\n<size:10>[6 files · 736 lines]</size>" <<LoupeLoupeC_storyboard>> as LoupeLoupeC_storyboard
rectangle "==update\\n<size:10>[4 files · 853 lines]</size>\\n\\nLoupe keeps itself current from its GitHub releases." <<LoupeLoupeC_update>> as LoupeLoupeC_update
rectangle "src" <<LoupeLoupeC_src>> as LoupeLoupeC_src {
  skinparam RectangleBorderColor<<LoupeLoupeC_src>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_src>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_src>> dashed

  rectangle "==main\\n<size:10>[main.cpp]</size>\\n\\nOpens a scene in the window, or the window's start panel without one; with -o or --video, writes the scene's frames and exits; with --frame or..." <<LoupeLoupeC_srcM_main>> as LoupeLoupeC_srcM_main
  rectangle "==options\\n<size:10>[options.cpp, options.hpp]</size>\\n\\nWhat one Loupe run is asked for: the scene viewer's own flags, same names." <<LoupeLoupeC_srcM_options>> as LoupeLoupeC_srcM_options
  rectangle "==scratch\\n<size:10>[scratch.cpp, scratch.hpp]</size>\\n\\nA folder of a run's own files, under the system's temporary folder." <<LoupeLoupeC_srcM_scratch>> as LoupeLoupeC_srcM_scratch
  rectangle "==version\\n<size:10>[version.cpp, version.hpp]</size>\\n\\nWhich Loupe this is: its version and commit, the channel it was built for, whether it was installed, and the autana it was built against." <<LoupeLoupeC_srcM_version>> as LoupeLoupeC_srcM_version
  rectangle "==library\\n<size:10>[library.cpp, library.hpp]</size>\\n\\nThe scenes the window's start panel offers: the sample packs shipped beside loupe, and the scenes opened last." <<LoupeLoupeC_srcM_library>> as LoupeLoupeC_srcM_library
  rectangle "==pack\\n<size:10>[pack.cpp, pack.hpp]</size>\\n\\nA scene's asset pack: a .apak as it is, or one autana's build_pack.py builds from a .scene.toml into the build's cache, in builds with the pack tool." <<LoupeLoupeC_srcM_pack>> as LoupeLoupeC_srcM_pack
  rectangle "==tools\\n<size:10>[tools.cpp, tools.hpp]</size>\\n\\nThe programs a build with the pack tool runs: autana's tools (build_pack.py, render_compare.py, scene_edit.py, bake.py) and git." <<LoupeLoupeC_srcM_tools>> as LoupeLoupeC_srcM_tools
  rectangle "==process\\n<size:10>[process.cpp, process.hpp]</size>\\n\\nOther processes: the tools loupe waits on." <<LoupeLoupeC_srcM_process>> as LoupeLoupeC_srcM_process
  rectangle "==paths\\n<size:10>[paths.cpp, paths.hpp]</size>\\n\\nA path as UTF-8 text and back: the spelling of recent.txt, ImGui and SDL; and a file read whole, read by line or written by line." <<LoupeLoupeC_srcM_paths>> as LoupeLoupeC_srcM_paths
}
rectangle "==path\\n<size:10>[2 files · 404 lines]</size>\\n\\nA scene's camera path from the command line: one frame of it, or the whole path drawn by two sides, A and B, laid out as an A | B | difference video." <<LoupeLoupeC_path>> as LoupeLoupeC_path
rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window

LoupeLoupeC_editor .[#64748b,thickness=2].> LoupeLoupeC_srcM_tools
LoupeLoupeC_engine .[#64748b,thickness=2].> LoupeLoupeC_srcM_options
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_srcM_pack
LoupeLoupeC_path .[#64748b,thickness=2].> LoupeLoupeC_srcM_scratch
LoupeLoupeC_storyboard .[#64748b,thickness=2].> LoupeLoupeC_srcM_options
LoupeLoupeC_update .[#64748b,thickness=2].> LoupeLoupeC_srcM_options
LoupeLoupeC_update .[#64748b,thickness=2].> LoupeLoupeC_srcM_scratch
LoupeLoupeC_update .[#64748b,thickness=2].> LoupeLoupeC_srcM_version
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_srcM_library
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_srcM_scratch
LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_srcM_version
LoupeLoupeC_srcM_library .[#64748b,thickness=2].> LoupeLoupeC_srcM_pack
LoupeLoupeC_srcM_options .[#64748b,thickness=2].> LoupeLoupeC_srcM_pack
LoupeLoupeC_srcM_pack .[#64748b,thickness=2].> LoupeLoupeC_srcM_tools
LoupeLoupeC_srcM_process .[#64748b,thickness=2].> LoupeLoupeC_srcM_paths
LoupeLoupeC_srcM_tools .[#64748b,thickness=2].> LoupeLoupeC_srcM_process
LoupeLoupeC_srcM_main .[#64748b,thickness=2].> LoupeLoupeC_path
LoupeLoupeC_srcM_main .[#64748b,thickness=2].> LoupeLoupeC_window
LoupeLoupeC_srcM_pack .[#64748b,thickness=2].> LoupeLoupeC_engine
@enduml
`;case`c_storyboard`:return`@startuml
title "storyboard"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_storyboardM_main>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_storyboardM_runner>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_storyboardM_recipe>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_storyboardC_config>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "storyboard" <<LoupeLoupeC_storyboard>> as LoupeLoupeC_storyboard {
  skinparam RectangleBorderColor<<LoupeLoupeC_storyboard>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_storyboard>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_storyboard>> dashed

  rectangle "==main\\n<size:10>[main.cpp]</size>\\n\\nRuns a recipe's loupe.* steps on Loupe's real window, Dear ImGui's test engine giving each step to the panels as a person's clicks, keys and..." <<LoupeLoupeC_storyboardM_main>> as LoupeLoupeC_storyboardM_main
  rectangle "==runner\\n<size:10>[runner.cpp, runner.hpp]</size>\\n\\nImVec2's arithmetic, for the crop." <<LoupeLoupeC_storyboardM_runner>> as LoupeLoupeC_storyboardM_runner
  rectangle "==recipe\\n<size:10>[recipe.cpp, recipe.hpp]</size>\\n\\nA recipe: the YAML list of steps loupe-storyboard runs, each a map of one "loupe." key, its value the step's arguments, and an optional name." <<LoupeLoupeC_storyboardM_recipe>> as LoupeLoupeC_storyboardM_recipe
  rectangle "==storyboard/config\\n<size:10>[1 file · 6 lines]</size>\\n\\nDear ImGui's configuration in a storyboard build: the test engine's hooks, its coroutines on std::thread." <<LoupeLoupeC_storyboardC_config>> as LoupeLoupeC_storyboardC_config
}
rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src

LoupeLoupeC_storyboardM_main .[#64748b,thickness=2].> LoupeLoupeC_storyboardM_runner
LoupeLoupeC_storyboardM_runner .[#64748b,thickness=2].> LoupeLoupeC_storyboardM_recipe
LoupeLoupeC_storyboardM_recipe .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_storyboardM_runner .[#64748b,thickness=2].> LoupeLoupeC_window
@enduml
`;case`c_tests`:return`@startuml
title "tests"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

@enduml
`;case`c_update`:return`@startuml
title "update"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_window>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_updateM_update>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_updateM_sha256>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
rectangle "==window\\n<size:10>[18 files · 3203 lines]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_window>> as LoupeLoupeC_window
rectangle "update" <<LoupeLoupeC_update>> as LoupeLoupeC_update {
  skinparam RectangleBorderColor<<LoupeLoupeC_update>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_update>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_update>> dashed

  rectangle "==update\\n<size:10>[update.cpp, update.hpp]</size>\\n\\nLoupe keeps itself current from its GitHub releases." <<LoupeLoupeC_updateM_update>> as LoupeLoupeC_updateM_update
  rectangle "==sha256\\n<size:10>[sha256.cpp, sha256.hpp]</size>\\n\\nFor checking a download against GitHub's digest." <<LoupeLoupeC_updateM_sha256>> as LoupeLoupeC_updateM_sha256
}
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src

LoupeLoupeC_window .[#64748b,thickness=2].> LoupeLoupeC_updateM_update
LoupeLoupeC_updateM_update .[#64748b,thickness=2].> LoupeLoupeC_updateM_sha256
LoupeLoupeC_updateM_update .[#64748b,thickness=2].> LoupeLoupeC_src
@enduml
`;case`c_window`:return`@startuml
title "window"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_src>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_storyboard>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_windowM_run_window>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_windowM_editor>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_windowM_files>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_windowM_frame_texture>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_windowM_observer>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_update>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_windowM_path_editor>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_windowM_gpu>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_engine>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_windowM_panels>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
skinparam rectangle<<LoupeLoupeC_editor>>{
  BackgroundColor #3b82f6
  FontColor #eff6ff
  BorderColor #2563eb
}
skinparam rectangle<<LoupeLoupeC_windowM_fonts>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
rectangle "==src\\n<size:10>[17 files · 1258 lines]</size>" <<LoupeLoupeC_src>> as LoupeLoupeC_src
rectangle "==storyboard\\n<size:10>[6 files · 736 lines]</size>" <<LoupeLoupeC_storyboard>> as LoupeLoupeC_storyboard
rectangle "window" <<LoupeLoupeC_window>> as LoupeLoupeC_window {
  skinparam RectangleBorderColor<<LoupeLoupeC_window>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_window>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_window>> dashed

  rectangle "==run_window\\n<size:10>[run_window.cpp, run_window.hpp]</size>\\n\\nThe window: a scene's frame, a timeline and a debug-view picker, or the start panel while no scene is open." <<LoupeLoupeC_windowM_run_window>> as LoupeLoupeC_windowM_run_window
  rectangle "==editor\\n<size:10>[editor.cpp, editor.hpp]</size>\\n\\nThe scene editor's panels over an open .scene.toml: the outliner, the inspector and the move/rotate/scale handles over the viewport's frame." <<LoupeLoupeC_windowM_editor>> as LoupeLoupeC_windowM_editor
  rectangle "==files\\n<size:10>[files.cpp, files.hpp]</size>\\n\\nThe window's files through SDL: the system's open-file dialog for a scene, and the folders the start panel reads." <<LoupeLoupeC_windowM_files>> as LoupeLoupeC_windowM_files
  rectangle "==frame_texture\\n<size:10>[frame_texture.cpp, frame_texture.hpp]</size>\\n\\nThe engine's frame as one texture ImGui draws: a user texture the Vulkan backend creates, uploads and frees like its own font atlas." <<LoupeLoupeC_windowM_frame_texture>> as LoupeLoupeC_windowM_frame_texture
  rectangle "==observer\\n<size:10>[observer.cpp, observer.hpp]</size>\\n\\nThe observer: the person's own camera on a frozen frame, which the view's second draw is seen from (Frames::observe)." <<LoupeLoupeC_windowM_observer>> as LoupeLoupeC_windowM_observer
  rectangle "==path_editor\\n<size:10>[path_editor.cpp, path_editor.hpp]</size>\\n\\nThe editor's camera paths: the selected camera's path drawn over the frame with a handle on each key's eye and look_at, the key rows in the..." <<LoupeLoupeC_windowM_path_editor>> as LoupeLoupeC_windowM_path_editor
  rectangle "==gpu\\n<size:10>[gpu.cpp, gpu.hpp]</size>\\n\\nAfter imgui's examples/example_sdl3_vulkan: one instance, device and queue, the swapchain kept by imgui's ImGui_ImplVulkanH_* helpers." <<LoupeLoupeC_windowM_gpu>> as LoupeLoupeC_windowM_gpu
  rectangle "==panels\\n<size:10>[panels.cpp, panels.hpp]</size>\\n\\nThe window's panels." <<LoupeLoupeC_windowM_panels>> as LoupeLoupeC_windowM_panels
  rectangle "==fonts\\n<size:10>[fonts.cpp, fonts.hpp]</size>\\n\\nThe panels' typefaces by role: Text for words, Mono where values must line up (numbers, hashes, columns)." <<LoupeLoupeC_windowM_fonts>> as LoupeLoupeC_windowM_fonts
}
rectangle "==update\\n<size:10>[4 files · 853 lines]</size>\\n\\nLoupe keeps itself current from its GitHub releases." <<LoupeLoupeC_update>> as LoupeLoupeC_update
rectangle "==engine\\n<size:10>[8 files · 994 lines]</size>" <<LoupeLoupeC_engine>> as LoupeLoupeC_engine
rectangle "==editor\\n<size:10>[8 files · 1052 lines]</size>" <<LoupeLoupeC_editor>> as LoupeLoupeC_editor

LoupeLoupeC_src .[#64748b,thickness=2].> LoupeLoupeC_windowM_run_window
LoupeLoupeC_storyboard .[#64748b,thickness=2].> LoupeLoupeC_windowM_run_window
LoupeLoupeC_windowM_editor .[#64748b,thickness=2].> LoupeLoupeC_windowM_path_editor
LoupeLoupeC_windowM_run_window .[#64748b,thickness=2].> LoupeLoupeC_windowM_editor
LoupeLoupeC_windowM_run_window .[#64748b,thickness=2].> LoupeLoupeC_windowM_files
LoupeLoupeC_windowM_gpu .[#64748b,thickness=2].> LoupeLoupeC_windowM_fonts
LoupeLoupeC_windowM_frame_texture .[#64748b,thickness=2].> LoupeLoupeC_windowM_gpu
LoupeLoupeC_windowM_run_window .[#64748b,thickness=2].> LoupeLoupeC_windowM_frame_texture
LoupeLoupeC_windowM_run_window .[#64748b,thickness=2].> LoupeLoupeC_windowM_observer
LoupeLoupeC_windowM_path_editor .[#64748b,thickness=2].> LoupeLoupeC_windowM_panels
LoupeLoupeC_windowM_files .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_windowM_frame_texture .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_windowM_observer .[#64748b,thickness=2].> LoupeLoupeC_engine
LoupeLoupeC_windowM_panels .[#64748b,thickness=2].> LoupeLoupeC_src
LoupeLoupeC_windowM_path_editor .[#64748b,thickness=2].> LoupeLoupeC_editor
LoupeLoupeC_windowM_run_window .[#64748b,thickness=2].> LoupeLoupeC_update
@enduml
`;case`c_storyboard_config`:return`@startuml
title "config"
top to bottom direction

hide stereotype
skinparam ranksep 60
skinparam nodesep 30
skinparam {
  arrowFontSize 10
  defaultTextAlignment center
  wrapWidth 200
  maxMessageSize 100
  shadowing false
}

skinparam rectangle<<LoupeLoupeC_storyboardC_configM_imgui_config>>{
  BackgroundColor #64748b
  FontColor #f8fafc
  BorderColor #475569
}
rectangle "storyboard/config" <<LoupeLoupeC_storyboardC_config>> as LoupeLoupeC_storyboardC_config {
  skinparam RectangleBorderColor<<LoupeLoupeC_storyboardC_config>> #3b82f6
  skinparam RectangleFontColor<<LoupeLoupeC_storyboardC_config>> #3b82f6
  skinparam RectangleBorderStyle<<LoupeLoupeC_storyboardC_config>> dashed

  rectangle "==imgui_config\\n<size:10>[imgui_config.h]</size>\\n\\nDear ImGui's configuration in a storyboard build: the test engine's hooks, its coroutines on std::thread." <<LoupeLoupeC_storyboardC_configM_imgui_config>> as LoupeLoupeC_storyboardC_configM_imgui_config
}
@enduml
`;default:throw Error(`Unknown viewId: `+e)}};export{e as pumlSource};