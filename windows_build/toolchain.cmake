# CMake Toolchain File for Windows (LLVM-MinGW on Linux)

set(CMAKE_SYSTEM_NAME Windows)

# Get the directory where this toolchain file is located
set(WINDOWS_BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(LLVM_MINGW_ROOT "${WINDOWS_BUILD_DIR}/toolchain/llvm-mingw")

# Specify the cross compiler
set(CMAKE_C_COMPILER "${LLVM_MINGW_ROOT}/bin/x86_64-w64-mingw32-clang")
set(CMAKE_CXX_COMPILER "${LLVM_MINGW_ROOT}/bin/x86_64-w64-mingw32-clang++")
set(CMAKE_RC_COMPILER "${LLVM_MINGW_ROOT}/bin/x86_64-w64-mingw32-windres")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# No need for SFML_ROOT here as we'll use FetchContent
