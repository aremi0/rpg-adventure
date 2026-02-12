cmake_minimum_required(VERSION 3.22)
project(RPG-ADVENTURE VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Cerca SFML 2.6
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# Trova AUTOMATICAMENTE tutti i file .cpp nella cartella src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Crea l'eseguibile (DEVE ESSERE PRIMA DI target_include_directories)
add_executable(game ${SOURCES})

# Include la cartella degli header
target_include_directories(game PUBLIC include)

# Link alle librerie
target_link_libraries(game sfml-graphics sfml-window sfml-system sfml-audio)