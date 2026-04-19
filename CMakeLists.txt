cmake_minimum_required(VERSION 3.22)
project(RPG-ADVENTURE VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Patch di compatibilità per SFML 2.x + Clang/libc++
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options("-include${CMAKE_CURRENT_SOURCE_DIR}/windows_build/sfml_compat.hpp")
endif()

# Gestione Dipendenze via FetchContent
include(FetchContent)

# 1. nlohmann_json
FetchContent_Declare(json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

# 2. EnTT (Header-only ECS)
FetchContent_Declare(entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
)

# 3. SFML (Building from source for maximum compatibility)
# Nota: Quando compiliamo per Windows, forziamo l'uso delle dipendenze interne
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_INSTALL_PDB OFF CACHE BOOL "" FORCE)
set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

# Per la cross-compilazione, dobbiamo assicurarci che SFML trovi le sue "extlibs"
# Scarichiamo subito il contenuto per poter puntare ai percorsi corretti
FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    
    if(WIN32 AND MINGW)
        # Diciamo a CMake dove trovare OpenAL e FreeType nelle extlibs di SFML
        set(SFML_EXTLIBS "${sfml_SOURCE_DIR}/extlibs")
        list(APPEND CMAKE_PREFIX_PATH 
            "${SFML_EXTLIBS}/libs-mingw/x64"
            "${SFML_EXTLIBS}/headers"
        )
        
        # Hint specifici per OpenAL e FreeType
        set(OPENAL_LIBRARY "${SFML_EXTLIBS}/libs-mingw/x64/libopenal32.a" CACHE FILEPATH "" FORCE)
        set(OPENAL_INCLUDE_DIR "${SFML_EXTLIBS}/headers/AL" CACHE PATH "" FORCE)
        set(FREETYPE_LIBRARY "${SFML_EXTLIBS}/libs-mingw/x64/libfreetype.a" CACHE FILEPATH "" FORCE)
        set(FREETYPE_INCLUDE_DIRS "${SFML_EXTLIBS}/headers/freetype2" CACHE PATH "" FORCE)
    endif()
    
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

# Rendi disponibili le altre dipendenze (json e entt sono semplici)
FetchContent_MakeAvailable(json entt)

# Trova AUTOMATICAMENTE tutti i file .cpp nella cartella src
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Aggiunge il fix del linker per Windows/Clang
if(WIN32)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/windows_build/linker_fix.c")
endif()

# Crea l'eseguibile
add_executable(game ${SOURCES})

# Include la cartella degli header interna
target_include_directories(game PUBLIC include)

# Link alle librerie
target_link_libraries(game 
    sfml-graphics 
    sfml-window 
    sfml-system 
    sfml-audio 
    nlohmann_json::nlohmann_json 
    EnTT::EnTT
)

# Ottimizzazioni per Windows (static linking delle runtime per evitare DLL mancanti)
if(WIN32)
    if(MINGW)
        target_link_options(game PRIVATE -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)
    endif()
endif()