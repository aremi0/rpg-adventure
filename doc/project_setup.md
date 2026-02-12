# Project Configuration Guide

This document explains the configuration files found in `.vscode/` and the root `CMakeLists.txt`.

## 1. `CMakeLists.txt`
This file defines how the project is built using CMake.

- `cmake_minimum_required(VERSION 3.22)`: Sets the minimum version of CMake required to build the project.
- `project(RPG-ADVENTURE VERSION 1.0)`: Defines the project name and version.
- `set(CMAKE_CXX_STANDARD 23)`: Specifies that the C++23 standard should be used.
- `set(CMAKE_CXX_STANDARD_REQUIRED ON)`: Ensures that the build fails if C++23 is not supported by the compiler.
- `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`: Generates a `compile_commands.json` file, useful for tools like `clangd` to understand the project structure.
- `find_package(SFML 2.6 ... REQUIRED)`: Searches for the SFML library (version 2.6) and its components (graphics, window, system, audio). The build fails if not found.
- `file(GLOB_RECURSE SOURCES "src/*.cpp")`: Automatically finds all `.cpp` files in the `src` directory and stores them in the `SOURCES` variable.
- `add_executable(game ${SOURCES})`: Creates an executable named `game` using the source files found.
- `target_include_directories(game PUBLIC include)`: Tells the compiler to look for header files in the `include` directory.
- `target_link_libraries(game sfml-...)`: Links the executable against the specific SFML libraries found earlier.

## 2. `.vscode/settings.json`
This file contains workspace-specific settings for VS Code.

- `"cmake.cmakePath": "/usr/bin/cmake"`: Explicitly tells VS Code where to find the CMake executable.
- `"clangd.arguments": ["--compile-commands-dir=build"]`: Instructs the `clangd` language server to look for `compile_commands.json` in the `build` directory for accurate code intelligence.

## 3. `.vscode/tasks.json`
This file defines tasks that can be run from VS Code (e.g., Terminal -> Run Task).

- **Task: "CMake: Configure"**
  - Runs `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`.
  - Generates the build system in the `build` folder (`-B build`) from the source in the current folder (`-S .`) with Debug configuration.
- **Task: "CMake: Build"**
  - Runs `cmake --build build`.
  - Compiles the project using the files generated in the `build` directory.
  - `"group": { "kind": "build", "isDefault": true }`: Makes this the default build task (Ctrl+Shift+B).
  - `"dependsOn": "CMake: Configure"`: Ensures configuration runs before building.

## 4. `.vscode/launch.json`
This file configures the debugger.

- `"program": "${workspaceFolder}/build/game"`: Points to the executable to debug.
- `"cwd": "${workspaceFolder}"`: Sets the current working directory to the project root when running the program (important for loading assets).
- `"MIMode": "gdb"`: Uses `gdb` as the debugger.
- `"preLaunchTask": "CMake: Build"`: Automatically runs the build task before starting the debugger.
- `"miDebuggerPath": "/usr/bin/gdb"`: Specifies the path to the GDB debugger.
