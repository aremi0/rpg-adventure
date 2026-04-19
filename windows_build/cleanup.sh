#!/bin/bash
set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m'

echo -e "${BLUE}== Windows Build Revert/Cleanup ==${NC}"

# 1. Remove downloaded tools and libs
echo -e "${YELLOW}[1/2] Removing local windows_build files (toolchain, libs, build)...${NC}"
rm -rf windows_build/toolchain
rm -rf windows_build/libs
rm -rf windows_build/build
rm -rf windows_build/dist
rm -f game-windows-x64.zip

# 2. Instructions for apt packages
echo -e "${YELLOW}[2/2] Local files cleaned up.${NC}"
echo -e "\n${GREEN}Note:${NC} Per una pulizia completa dei pacchetti di sistema:"

echo -e "\n1. Rimuovi le dipendenze inutilizzate di MinGW (fortemente consigliato):"
echo -e "  ${BLUE}sudo apt-get remove --purge mingw-w64${NC}"
echo -e "  ${BLUE}sudo apt autoremove --purge${NC}"

echo -e "\n2. Se desideri rimuovere anche i tool di compressione/download installati (opzionale):"
echo -e "  ${BLUE}sudo apt-get remove --purge zip unzip wget tar xz-utils${NC}"

echo -e "\n${YELLOW}P.S.${NC} Il compilatore Clang (LLVM-MinGW) è già stato rimosso cancellando la cartella locale."

echo -e "\n${BLUE}== Cleanup Complete! ==${NC}"
