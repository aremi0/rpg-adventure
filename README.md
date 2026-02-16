# RPG Adventure

This is a personal 2D RPG game project created for fun and to strengthen my understanding of C++ and the new C++23 standard features.

## Requirements

To build this project, you will need:

*   **C++ Compiler**: Must support C++23.
*   **CMake**: Version 3.22 or higher.
*   **SFML**: Simple and Fast Multimedia Library (Graphics, Window, System, Audio modules).
*   **EnTT**: Open-source, header-only Entity Component System (ECS) library.

## Documentation

You can find detailed documentation in the `doc/` directory:

*   [C++ Review](doc/cpp_review.md): A quick review of C++23 features.
*   [TODO](doc/todo_list.md): List of TODOs and future features.
*   [Naming Conventions](doc/naming_conventions.md): Naming Conventions used in this project.

---

*   [Project Overview](doc/core/0_structure_and_roadmap.md): Explains the project overview.
*   [Project Configuration & Setup](doc/core/1_project_setup.md): Explains the `.vscode` configuration and `CMakeLists.txt` file structure.
*   [Core Engine & State Machine](doc/core/2_0_core_engine_and_state_machine.md): Explains the core engine and state machine.
*   [Core Engine & State Machine](doc/core/2_1_logging_and_menu_state.md): Explains the logging system and the menu state.
*   [Resource Management](doc/core/3_resource_managment.md): Explains the resource management system.

---

### 🗺️ Roadmap Aggiornata

Ho inserito la nuova **Fase 3.5** come richiesto.

* ✅ **Fase 1: Setup dell'Ambiente e Boilerplate**
* ✅ **Fase 2: Core Engine & State Machine**
* ✅ **Fase 3: Resource Management** (Texture & Animations)
* 🔄 **Fase 3.5: UI, Audio e Menu Completo**
    * ⬜ **3.5.1: Classe UI Button** (Creazione pulsanti interattivi e centrati).
    * ⬜ **3.5.2: Gestione Audio** (Aggiornamento AssetManager per SFX e Music).
    * ⬜ **3.5.3: Settings State** (Nuovo stato per risoluzione e volumi).
    * ⬜ **3.5.4: Assemblaggio Main Menu** (Unire tutto con sfondo, musica e pulsanti).

* ⬜ **Fase 4: Integrazione ECS (EnTT)**
* ⬜ **Fase 5: Input & Movimento**
* ⬜ **Fase 6: Tilemap & Collisioni**
* ⬜ **Fase 7: RPG Logic & UI**

---