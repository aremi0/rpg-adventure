#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${BLUE}== Windows Cross-Compilation Build (LLVM-Clang) ==${NC}"

# Get absolute path of this script's directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$( dirname "$SCRIPT_DIR" )"
TOOLCHAIN_BIN="$SCRIPT_DIR/toolchain/llvm-mingw/bin"

# Check if toolchain exists
if [ ! -d "$TOOLCHAIN_BIN" ]; then
    echo -e "${RED}Error: Toolchain non trovato in $TOOLCHAIN_BIN. Esegui prima ./windows_build/setup.sh${NC}"
    exit 1
fi

# Add toolchain to PATH just for this session
export PATH="$TOOLCHAIN_BIN:$PATH"

# 1. Prepare build directory
echo -e "${GREEN}[1/3] Configuring CMake...${NC}"
mkdir -p "$SCRIPT_DIR/build"
cd "$SCRIPT_DIR/build"

# Usiamo il nostro toolchain file
cmake -DCMAKE_TOOLCHAIN_FILE="$SCRIPT_DIR/toolchain.cmake" \
      -DBUILD_SHARED_LIBS=OFF \
      -DSFML_STATIC_LIBRARIES=ON \
      "$PROJECT_ROOT"

# 2. Compile
echo -e "${GREEN}[2/3] Compiling (this might take a few minutes the first time)...${NC}"
make -j$(nproc)

# 3. Create distribution folder
echo -e "${GREEN}[3/3] Packaging...${NC}"
DIST_DIR="$SCRIPT_DIR/dist"
GAME_DIR="$DIST_DIR/game"
rm -rf "$DIST_DIR"
mkdir -p "$GAME_DIR"

# Copy executable
cp "$SCRIPT_DIR/build/game.exe" "$GAME_DIR/"

# Copy assets
if [ -d "$PROJECT_ROOT/assets" ]; then
    cp -r "$PROJECT_ROOT/assets" "$GAME_DIR/"
fi

# Copy required DLLs from SFML extlibs
echo -e "${YELLOW}Copying dependencies...${NC}"
cp "$SCRIPT_DIR/build/_deps/sfml-src/extlibs/bin/x64/openal32.dll" "$GAME_DIR/"

# 4. Zip the output
echo -e "${GREEN}Creating ZIP archive...${NC}"
cd "$DIST_DIR"
zip -r "$PROJECT_ROOT/game-windows-x64.zip" game/

echo -e "${BLUE}== Build Complete! ==${NC}"
echo -e "Archive: ${GREEN}game-windows-x64.zip${NC}"
