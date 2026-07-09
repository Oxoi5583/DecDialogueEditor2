# DecDialogueEditor2

DecDialogueEditor2 is an early-stage C++ dialogue graph editor for building game dialogue logic with a visual, WYSIWYG-style workflow. The long-term goal is to let writers/designers compose dialogue nodes, branches, options, modules, and workspaces in a GUI, then export the dialogue into formats that can be consumed by a game project.

<img width="400" height="300" alt="DecDialogueEditor2 screenshot" src="https://github.com/user-attachments/assets/bb8a456d-b01f-4acd-be87-35d45bce50dc" />

This repository is also a learning-oriented editor/runtime experiment. It is not a finished product yet, but it already contains a useful set of patterns for anyone who wants to fork it, customize the editor, or study how a small C++ immediate-mode desktop tool can be structured.

## Project Focus

The project is built around three ideas:

- A custom editor runtime on top of SDL3, OpenGL, and Dear ImGui.
- A node graph model where dialogue objects manage their own interaction, rendering, connection, and persistence behavior.
- JSON-first project/workspace data, so editor state can be saved, exported, inspected, and transformed for game runtime use.

The current implementation is Windows-first and development-focused. The shipped scripts, bundled DLLs, and file-hidden behavior are mainly aimed at a Windows/MSVC workflow.

## Tech Stack

- C++20
- CMake + Ninja
- SDL3 for windowing, input, and OpenGL context setup
- OpenGL + GLAD for custom graph canvas rendering
- Dear ImGui for editor panels, popups, menus, and text input
- GLM for vector/matrix math
- nlohmann/json for config, themes, workspace data, and export data
- Boost headers/interprocess, currently pulled through CMake
- SimZip for `.ddlg` project packaging
- stb and vendored ImGui sources under `src_extern/`
- DecToolsBox submodule for singleton, ordered containers, debug messaging, random codes, rules/conditions, and small utilities

## Repository Map

```text
src/
  main.cpp                  App bootstrap and frame loop.
  engine/                   SDL/OpenGL/ImGui runtime layer.
  server/                   Global services: events, objects, project, file, mouse, timer, physics.
  editor/                   Layout system, shortcut menu, and UI component layer.
  editor/components/        Menu bar, tools bar, left panel, popups, explorer, messages, detail windows.
  system/graph/             Camera, viewport, grid, selection, background, connection state/rules.
  system/obj/graph/         Dialogue graph objects: base, entry, node, option, repeater, module nodes.
  system/obj/abstract/      Reusable object behavior: movable, hoverable, clickable, draggable, selectable.
  system/obj/fstream/       File/folder objects used by FileServer and ProjectServer.
  struct/shape/             Rect, line, circle, and shape helpers.
  theme/                    Theme JSON loader.
  config/                   Setting JSON loader.

bin/
  config/setting.json       Runtime setting file.
  theme/*.json              Theme definitions.
  fonts/                    UI and icon fonts.
  assets/                   Runtime texture assets.
  *.dll                     Windows runtime DLLs.

src_extern/
  imgui/                    Vendored Dear ImGui source and SDL3/OpenGL backends.
  stb/                      Vendored stb headers/sources.
```

## Runtime Flow

The editor has a small custom runtime instead of using a full game engine. The main loop in `src/main.cpp` wires the systems together in a fixed order:

```text
init:
  FileServer
  GraphGrid
  ThemeLoader / ConfigLoader
  EngineWindow / EngineRenderer / Input / Texture / Font
  GraphCamera / Viewport / Background / Selection
  GraphManager
  EditorLayout
  UiTextBank

frame:
  poll SDL input
  begin window + ImGui frame
  process project/workspace actions
  update camera, viewport, graph connection, selection, graph manager
  update editor layout and UI components
  ObjectServer lifecycle:
    ready -> pre_process -> process -> post_process -> draw
  draw graph/grid/selection/UI overlays
  flush typed events
  render OpenGL + ImGui
  update physics spatial index

shutdown:
  clean up project temp data
  shut down servers and engine resources
```

This makes the editor easy to inspect: if something happens every frame, it is usually either in the main loop, a singleton server, or an object registered into `ObjectServer`.

## Core Patterns

### Singleton Service Layer

Most systems are globally reachable services using `Singleton<T>::Ref()` from DecToolsBox. Examples:

- `EngineWindow`
- `EngineRenderer`
- `EngineInputHub`
- `ObjectServer`
- `EventServer`
- `ProjectServer`
- `GraphManager`
- `GraphCamera`
- `GraphConnection`
- `GraphSelection`

This keeps the code simple for a single-window editor prototype. The tradeoff is that dependencies are implicit, so when changing behavior it is important to follow the frame order in `main.cpp`.

### Object Lifecycle Through `ObjectServer`

Runtime objects inherit from `ObjectBase` and are created through:

```cpp
ObjectServer::Ref()->queue_create<T>();
```

Objects can register lifecycle methods with the `BIND_CLASS(ClassName)` macro:

```text
ready()
pre_process()
process()
post_process()
draw()
```

`ObjectServer` owns objects with `unique_ptr`, assigns object IDs (`OID`), separates UI/graph layers, supports queued creation/deletion, and controls draw/process ordering.

This pattern is the backbone of graph nodes, UI popups, file-system objects, and interactive editor objects.

### Typed Event Bus

`EventServer` is a frame-based typed event bus. Events are plain structs in `src/server/events.h`.

There are two event categories:

- `UniqueEvent`: only one event of that type should exist in the current frame.
- `NonUniqueEvent`: multiple events of that type can be collected and processed.

Systems communicate by emitting and polling events:

```cpp
EventServer::Ref()->emit(event);
EventServer::Ref()->poll<EventSpawnNode>();
EventServer::Ref()->poll_first<EventMouseHoverObj>();
EventServer::Ref()->has<EventLockedAll>();
```

This avoids direct coupling between input, graph objects, popup windows, selection, connection creation, and editor panels.

### Behavior as Layered State Machines

Interactive graph objects are built by stacking reusable behavior classes:

```text
ObjectBase
  -> MovableObject
    -> HoverableObject
      -> ClickableObject
        -> DragableObject
          -> SelectableObject
            -> GraphBase
              -> GraphEntry / GraphNode / GraphOption / GraphRepeater / ...
```

Each layer owns a small part of the interaction model:

- `MovableObject`: shape, position, camera visibility, physics registration.
- `HoverableObject`: hover detection in screen/world space.
- `ClickableObject`: click/release/double-click state.
- `DragableObject`: drag/place state, grid alignment, camera-edge dragging.
- `SelectableObject`: selection, multi-selection, selection area behavior.
- `GraphBase`: node data, properties, children/parents, project persistence, graph drawing.

This is one of the more reusable parts of the project. If you want to add new editable graph objects, start by inheriting from `GraphBase` or one of the existing graph node types.

### Immediate-Mode UI + Retained Graph Objects

The UI layer uses Dear ImGui, but the graph editor is not purely ImGui widgets. The editor combines:

- ImGui for panels, menus, popups, text input, and file explorer UI.
- Custom OpenGL rendering for graph rectangles, lines, circles, selection areas, and grid.
- Retained runtime objects for nodes, connection lines, popups, and file-system items.

This hybrid approach keeps UI implementation fast while still allowing a custom canvas/editor feel.

### Batched Renderer

`EngineRenderer` collects draw requests each frame and renders them with instanced OpenGL buffers.

Current primitive APIs:

```cpp
draw_rect(Rect2 rect, vec4 color, TextureId id);
draw_circle(vec2 pos, float radius, vec4 color, TextureId id);
draw_line(vec2 from, vec2 to, vec4 color, float width);
```

Graph nodes and editor overlays submit draw commands instead of directly calling OpenGL. This makes most graph objects independent from shader/buffer details.

### Rule-Based Graph Connections

Connection behavior is split between:

- `src/system/graph/connection.*`
- `src/system/graph/connection_rules.*`

`GraphConnection::test_connection()` composes rule objects with DecToolsBox `Condition`, for example:

- target cannot be self
- target cannot be an entry type
- target cannot already be connected
- target cannot be a direct parent
- repeater-specific parent/child constraints
- option/module-specific constraints

If you want to customize the dialogue graph grammar, this is the main place to work.

### Workspace Persistence

Project data is stored as JSON workspaces managed by `ProjectServer`.

Graph nodes upload their own data into the current workspace:

- position
- type
- name
- children
- properties
- camera data

Saving a project writes workspace JSON files into a temporary project folder and packages them as `.ddlg` through SimZip. Exporting creates a flatter dialogue JSON payload with node data and name/code mapping.

Important files:

- `src/server/project_server.*`
- `src/system/obj/graph/base.*`
- `src/system/obj/fstream/*`
- `src/server/file_server.*`

## Customization Guide

### Add a New Node Type

Start in `src/system/obj/graph/`.

1. Create a class that inherits from `GraphBase`.
2. Override `get_type()`.
3. Add default properties in `ready()`.
4. Register the type in `GraphManager::init()`.
5. Add spawn handling in `GraphManager::m_poll_spawn_event()` and the related spawn helper.
6. Add a theme color key if the type should have its own visual color.
7. Update connection rules if the new node changes dialogue grammar.
8. Update export logic in `ProjectServer::m_action_export_project()` if the exported schema needs new fields.

### Change Connection Rules

Look at:

- `src/system/graph/connection_rules.h`
- `src/system/graph/connection_rules.cpp`
- `GraphConnection::test_connection()` in `src/system/graph/connection.cpp`

The recommended pattern is to add a small rule class, then compose it in `test_connection()` instead of mixing every condition into the connection state machine.

### Customize Theme and Visual Identity

Themes live in:

```text
bin/theme/*.json
```

The active theme is configured in:

```text
bin/config/setting.json
```

`ThemeLoader` validates each theme file by checking required keys and hex color format. The keys are used by graph nodes, ImGui styling, grid, selection, and editor accents.

### Customize Layout or Panels

Editor layout is built from `EditorSpace`, a split-tree layout helper with:

- vertical/horizontal splits
- fixed split values
- min/max limits
- draggable resizers
- magnet snapping

Main files:

- `src/editor/space.*`
- `src/editor/layout.*`
- `src/editor/components/*`

To add a new persistent panel, create an `EditorComponentBase` subclass, instantiate it in `EditorLayout::m_init_objs()`, and assign it to an `EditorSpace`.

### Customize Project Format or Export

Use `ProjectServer` as the data boundary:

- workspace creation/open/save/export
- JSON routing helpers
- `.ddlg` package creation
- exported dialogue JSON mapping

Graph object persistence currently happens from `GraphBase`, so schema changes often need updates in both `GraphBase` and `ProjectServer`.

### Customize Input Behavior

Raw SDL input is collected by `EngineInputHub`, then converted into higher-level mouse/editor events through `MouseServer`, object behavior classes, and `EventServer`.

Useful entry points:

- `src/engine/input_hub.*`
- `src/server/mouse_server.*`
- `src/server/events.h`
- `src/system/obj/abstract/*`

## Build Notes

This project currently assumes a Windows/MSVC-oriented workflow.

Recommended setup:

- Visual Studio Build Tools or Visual Studio with C++ toolchain
- CMake 3.13+
- Ninja
- Git with submodule support
- Network access for CMake `FetchContent`

Clone and prepare dependencies:

```powershell
git submodule update --init --recursive
```

Configure and build:

```powershell
mkdir build
cmake -S . -B build -G Ninja
cmake --build build --config Debug
```

Run:

```powershell
.\bin\DecDialogueEditor.exe
```

or:

```powershell
.\run.bat
```

`compile.bat` is a convenience script, but it currently assumes the `build/` folder already exists:

```bat
cd build
cmake .. -G Ninja
cmake --build . --config Debug
cd ..
```

The executable output is configured to `bin/`, and the app expects runtime folders such as `bin/config`, `bin/theme`, `bin/fonts`, and `bin/assets` to be available beside it.

## Current Development Notes

- The project is still in early development and many editor features are incomplete.
- There is no formal automated test suite yet.
- Some systems are experimental and intentionally direct because this is also a first larger C++ editor project.
- `include/DecToolsBox` is a required submodule. If it is empty after cloning, run the submodule command above.
- `.gitmodules` still lists `third_party/SDL3`, while the current `CMakeLists.txt` fetches SDL3 through `FetchContent`. Treat the CMake file as the current source of truth.
- The project file/export schema should be considered unstable until the editor reaches a more mature version.

## Good Places to Start Reading

For understanding the app boot order:

- `src/main.cpp`

For the editor runtime:

- `src/engine/window.*`
- `src/engine/renderer.*`
- `src/engine/input_hub.*`

For object lifecycle:

- `src/server/object_base.*`
- `src/server/object_server.*`

For event-driven behavior:

- `src/server/events.h`
- `src/server/event_server.*`

For graph behavior:

- `src/system/obj/graph/base.*`
- `src/system/obj/graph/manager.*`
- `src/system/graph/connection.*`
- `src/system/graph/connection_rules.*`
- `src/system/graph/selection.*`

For persistence/export:

- `src/server/project_server.*`
- `src/server/file_server.*`
- `src/system/obj/fstream/*`

For UI and layout:

- `src/editor/space.*`
- `src/editor/layout.*`
- `src/editor/components/*`
