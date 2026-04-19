#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m'

echo -e "${BLUE}== Windows Cross-Compilation Setup (LLVM-Clang Version) ==${NC}"

# Get absolute path of this script's directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TOOLCHAIN_DIR="$SCRIPT_DIR/toolchain"

# 1. Install local tools
echo -e "${GREEN}[1/3] Installing host tools...${NC}"
sudo apt-get update
sudo apt-get install -y zip unzip wget tar xz-utils

# 2. Download LLVM-MinGW Toolchain
echo -e "${GREEN}[2/3] Setting up LLVM-MinGW (Clang) toolchain...${NC}"
mkdir -p "$TOOLCHAIN_DIR"
cd "$TOOLCHAIN_DIR"

# Note: We use a specific version known to support C++23 std::print
TOOLCHAIN_URL="https://github.com/mstorsjo/llvm-mingw/releases/download/20260407/llvm-mingw-20260407-ucrt-ubuntu-22.04-x86_64.tar.xz"
TOOLCHAIN_FILE="llvm-mingw.tar.xz"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo -e "Downloading toolchain..."
    wget -O "$TOOLCHAIN_FILE" "$TOOLCHAIN_URL"
fi

if [ ! -d "llvm-mingw" ]; then
    echo -e "Extracting toolchain..."
    tar -xf "$TOOLCHAIN_FILE"
    # Find the extracted folder name and rename it to a generic one
    EXTRACTED_DIR=$(ls -d llvm-mingw-*/ | head -n 1)
    mv "$EXTRACTED_DIR" "llvm-mingw"
fi

# 3. Clean up libs folder (no longer need manual binaries, will use FetchContent)
echo -e "${GREEN}[3/3] Cleaning up old libraries folder...${NC}"
rm -rf "$SCRIPT_DIR/libs"
mkdir -p "$SCRIPT_DIR/libs"

echo -e "${BLUE}== Setup Complete! ==${NC}"
echo -e "Compilatore Clang configurato in $TOOLCHAIN_DIR/llvm-mingw"
echo -e "Next step: Run ./windows_build/build.sh"
