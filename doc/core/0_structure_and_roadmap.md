### Struttura delle cartelle sorgente (`/src` e `/include`)

Ti consiglio di mantenere una struttura speculare tra `include` e `src`. Se hai un file in `src/core/Engine.cpp`, il suo header dovrebbe essere in `include/core/Engine.hpp`.

```text
/MyRPG
├── include/                 # Header files (.hpp / .h)
│   ├── core/                # Motore di gioco, State Machine, Input
│   ├── states/              # Definizione dei vari stati (Menu, Play, Combat)
│   ├── entities/            # Player, Nemici, NPC
│   ├── components/          # Se usi ECS (Position, Velocity, Sprite)
│   ├── map/                 # Gestione Tilemap e Collisioni
│   ├── resources/           # Resource Holder/Manager
│   ├── ui/                  # Interfaccia utente (HUD, Menu)
│   └── utils/               # Funzioni matematiche, Logger, Costanti
├── src/                     # File sorgente (.cpp)
│   ├── core/                # Implementazione logica engine
│   ├── states/              # Logica degli stati
│   ├── entities/            # Comportamento entità
│   ├── ...                  # (stessa struttura di include)
│   └── main.cpp             # Entry point del gioco
├── assets/                  # Grafica, audio e config
└── CMakeLists.txt           # Configurazione build

```

---

### Dettaglio dei Moduli

Ecco cosa dovresti inserire in ogni cartella per mantenere il codice pulito:

#### 1. `core/` (Il cuore pulsante)

Qui risiede la classe `Game` o `Engine`. Gestisce il `sf::RenderWindow`, il loop principale e chiama lo `StateManager`.

* **Best Practice:** Gestisci qui il **DeltaTime**, passandolo poi a tutti i metodi `update(float dt)`.

#### 2. `states/` (La logica di scena)

Usa questa cartella per separare i contesti.

* `State.hpp`: Classe base astratta.
* `GameState.hpp/cpp`: La logica del mondo RPG.
* `MenuState.hpp/cpp`: La logica dell'interfaccia iniziale.

#### 3. `resources/` (I magazzinieri)

Fondamentale per non saturare la RAM. Qui implementi il `ResourceHolder`.

* **C++23 Tip:** Usa `std::expected<sf::Texture, std::string>` per gestire i fallimenti nel caricamento delle texture senza far crashare il gioco o usare eccezioni pesanti.

#### 4. `entities/` (Chi agisce)

Qui definisci cosa "esiste" nel mondo.

* Se usi l'ereditarietà: `Entity` -> `Actor` -> `Player`.
* Se usi **EnTT (ECS)**: Qui metterai i factory per creare entità (es. `EntityFactory::createOrc()`).

---

### 🗺️ Roadmap: RPG C++23 + SFML + ECS

1. **Fase 1: Setup dell'Ambiente e Boilerplate**
* Configurazione CMake (C++23, SFML 2.6, EnTT).
* Creazione della struttura delle cartelle.
* Il primo `main.cpp` con un game loop minimale.


2. **Fase 2: Core Engine & State Machine**
* Architettura della classe `Game`.
* Implementazione dello `State Manager` (per passare da Menu a Gioco).


3. **Fase 3: Resource Management**
* Creazione di un gestore centralizzato per Texture, Sound e Font (usando `std::expected` di C++23).
* Creazione del menu principale.


4. **Fase 4: Integrazione ECS (EnTT)**
* Creazione del primo `Registry`.
* Definizione dei primi Componenti (`TransformComponent`, `SpriteComponent`).
* Implementazione del `RenderSystem`.


5. **Fase 5: Input & Movimento**
* Gestione degli eventi SFML.
* `MovementSystem` e logica del Player.


6. **Fase 6: Tilemap & Collisioni**
* Rendering di una mappa a griglia.
* Collisioni AABB tra entità e mondo.


7. **Fase 7: RPG Logic & UI**
* Statistiche (HP, Mana), combattimento semplice e interfaccia utente.

---