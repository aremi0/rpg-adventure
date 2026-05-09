# RPG Adventure

This is a personal 2D RPG game project created for fun and to strengthen my understanding of C++ and the new C++23 standard features.

## Requirements

### 1. Build Tools (Manual Setup)
To build this project, you need the following tools installed on your system:

*   **C++ Compiler**: Must support C++23 (GCC 13+, Clang 16+, or the provided LLVM-MinGW for Windows).
*   **CMake**: Version 3.22 or higher.
*   **Git**: Required by CMake to download dependencies.
*   **Audio Pipeline Tools**: `zsh` and `ffmpeg` (only needed for `normalize_audio.sh`).

### 2. Linux System Dependencies
If you are building natively on Linux (including WSL2), you must install these development libraries for SFML to interact with the OS:
```bash
sudo apt update && sudo apt install -y \
    libudev-dev libfreetype-dev libx11-dev \
    libxrandr-dev libxcursor-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libasound2-dev \
    libpulse-dev libopenal-dev libvorbis-dev libflac-dev
```

### 3. C++ Libraries (Managed by CMake)
The following libraries are **automatically downloaded and built** by the project. You do **not** need to install them manually:

*   **SFML (2.6.1)**: Graphics, Window, System, Audio.
*   **EnTT**: Entity Component System.
*   **nlohmann_json**: JSON support.

### 2. Preliminary Steps (Audio Pipeline)
The project uses an asset pipeline for audio to maintain quality and standardization. You **must** run the normalization script before launching the game to generate the necessary assets:
```bash
./normalize_audio.sh ./raw_audio ./assets/audio
```
*Note: The `raw_audio/` directory must contain `sfx/` and `music/` subfolders with `.wav` files.*

### 3. Compilation
Use the standard CMake workflow to build the project:
```bash
cmake -B build
cmake --build build
```

### 4. Running the Game
After a successful build, you can run the executable:
```bash
# Linux / macOS
./build/game

# Windows
.\build\game.exe
```

### 5. Cross-Compilation for Windows (from Linux)
If you are on Linux (WSL2) and want to build a `.exe` for Windows with full **C++23 support** (including `std::print`), use the modernization scripts in `windows_build/`:

#### A. Initial Setup
This will download and configure the **LLVM-MinGW (Clang)** toolchain in a local folder:
```bash
./windows_build/setup.sh
```

#### B. Build and Package
Compiles the project (building SFML and EnTT from source for maximum compatibility) and creates a `game-windows-x64.zip`:
```bash
./windows_build/build.sh
```
*Note: The first build might take a few minutes as it compiles SFML from source.*

#### C. Cleanup (Optional)
To revert the cross-compilation setup and remove local toolchains/libs:
```bash
./windows_build/cleanup.sh
```


## Documentation

You can find detailed documentation in the `doc/` directory:

*   [C++ Review](doc/cpp_review.md): A quick review of C++23 features.
*   [TODO](doc/todo_list.md): List of TODOs and future features.
*   [Naming Conventions](doc/naming_conventions.md): Naming Conventions used in this project.

---

*   [0. Project Overview](doc/core/0_structure_and_roadmap.md): Explains the project overview.
*   [1. Project Configuration & Setup](doc/core/1_project_setup.md): Explains the `.vscode` configuration and `CMakeLists.txt` file structure.
*   [2.0 Core Engine & State Machine](doc/core/2_0_core_engine_and_state_machine.md): Explains the core engine and state machine.
*   [2.1 Logging System, MainMenuState initialization](doc/core/2_1_logging_and_menu_state.md): Explains the logging system and the menu state initialization.
*   [3.0 Resource Management](doc/core/3_0_resource_managment.md): Explains the resource management system.
*   [3.5 MainMenuState & SettingsState](doc/core/3_5_main_menu_settings_core.md): Explains the main menu and settings.
*   [3.6 Audio System & Settings UI](doc/core/3_6_audio_system_and_settings_ui.md): Explains the audio system and settings UI.
*   [3.7 Audio Asset Pipeline](doc/core/3_7_audio_asset_pipeline.md): Explains the audio asset pipeline.
*   [4.0 Entity Component System (ECS)](doc/core/4_0_ecs.md): Explains the ECS system.
*   [5.4.1: Dynamic Logger](doc/core/5_4_1_dynamic_logging.md): Explains the dynamic logger.
*   [5.4.2: Visual Debug Rendering](doc/core/5_4_2_visual_debug_rendering.md): Explains the visual debug rendering.
*   [6.0: Settings Persistence](doc/core/6_settings_persistence.md): Explains the settings persistence system and async loading.

---

### 🗺️ Roadmap Aggiornata

* ✅ **Fase 1: Setup dell'Ambiente e Boilerplate**
* ✅ **Fase 2: Core Engine & State Machine**
    * ✅ **2.1: Logging System, MainMenuState initialization**
* ✅ **Fase 3: Resource Management**
    * ✅ **3.5 - 3.6: UI, Audio e SettingsState**
* ✅ **Fase 4: Integrazione ECS (EnTT)**
    * ✅ **4.1: Registry Setup**
    * ✅ **4.2: Base Component**
    * ✅ **4.3: Render System**
    * ✅ **4.4: Game State & First Entity**
* ✅ **Fase 5: Input, Movimento e Tools**
    * ✅ **5.1: Nuovi Componenti** (`VelocityComponent` e `PlayerComponent`).
    * ✅ **5.2: Player Input System** (Legge la tastiera e imposta la direzione).
    * ✅ **5.3: Movement System** (Applica la velocità alla posizione in modo fluido).
    * ✅ **5.4: Advanced Debugging Tools**
        * ✅ **5.4.1: Dynamic Logger** (Switch in tempo reale dei log-level).
        * ✅ **5.4.2: Debug Render System** (Modalità visiva geometrica e testo overlay, verrà riespansa in futuro).
* 🔄 **Fase 6: Data-Driven Architecture & Persistence**
    * 🔄 **6.1: Integrazione Parser** (Setup CMake per nlohmann/json).
    * 🔄 **6.2: Settings Persistence** (Salvataggio/Caricamento base-fallback delle impostazioni audio/video su file).
* ⬜ **Fase 7: Mappa, Collisioni e Telecamera (LDtk Integration)**
    * ⬜ **7.1: Architettura MapManager & LDtk Loader** (Creazione di un loader dedicato che usa `std::expected` e `nlohmann/json` per caricare i dati spaziali).
    * ⬜ **7.2: Rendering Ottimizzato (Vertex Arrays)** (Implementazione del rendering dei layer di tile usando `sf::VertexArray` e `std::mdspan` per l'accesso ai dati).
    * ⬜ **7.3: Integrazione 2.5D & ECS** (Conversione delle entità LDtk in entità EnTT con `TransformComponent` ed `Elevation`).
    * ⬜ **7.4: Box Collider Component & System** (Collisioni mappa/entità basate sui dati estratti da LDtk).
    * ⬜ **7.5: Camera System & Advanced Debug** (La telecamera segue l'eroe; overlay per hitbox e griglia tilemap).
* ⬜ **Fase 8: Animazioni, Pathfinding & Intelligenza**
    * ⬜ **8.1: Entity Animation System** (`AnimatorComponent`, per ciclare agilmente i vari frames dei personaggi).
    * ⬜ **8.2: Mouse Navigation System (A* Pathfinding)** *(Opzionale)* (Movimento Punta e Clicca).
    * ⬜ **8.3: RPG Core Logic** (NPC, Nemici, HP).

---

<img src="doc/screen.png" alt="screen" width="400">