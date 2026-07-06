# Agent Development Guide — RPG Adventure

> **Scopo:** documento unico da fornire a ogni agente AI che lavora su una fase della roadmap.  
> Contiene contesto progetto, decisioni di design vincolanti, scope per fase e criteri di completamento.

**Ultimo aggiornamento:** allineato al piano Fase 7 v2 (elevazione dual-layer, party WoES, WASD-only).

---

## Come usare questo documento (per agenti)

1. Leggi le sezioni **Stack**, **Convenzioni** e **Regole d'oro** — sono vincolanti per ogni fase.
2. Identifica la **fase/task** assegnata nella tabella sotto e vai alla sezione dedicata.
3. Leggi solo la documentazione correlata indicata in quella sezione (non tutto `doc/`).
4. **Non implementare** meccaniche di fasi future (es. party in Fase 7, pathfinding in Fase 7).
5. Rispetta lo scope: diff minimo, stile esistente, nessun refactor non richiesto.
6. Prima di chiudere: verifica i **criteri di accettazione** della tua fase.

### Assegnazione rapida fasi → agente

| Task agente | Sezione | Doc aggiuntivi |
|-------------|---------|----------------|
| Doc cleanup / LDtk editor | [A — Preparazione](#fase-a-preparazione-doc--ldtk-editor) | [`ldtk-doc/ldtk-index.md`](ldtk-doc/ldtk-index.md) |
| 7.1 Loader + MapManager | [B1–B2](#b1b2--ldtkloader--mapmanager-71) | [`ldtk-phase1-integration.md`](ldtk-doc/ldtk-phase1-integration.md) |
| 7.2 Tile rendering | [B3–B4](#b3b4--tileset--maprendersystem-72) | phase1 |
| 7.x GameState integration | [B5, B8](#b5--integrazione-gamestate) | [`4_0_ecs.md`](core/4_0_ecs.md) |
| 7.3–7.4 Elevation + collisioni | [B6–B7](#b6b7--elevazione--collisioni-7374) | phase1, questo doc § Elevazione |
| 7.3, 7.5 Depth sort + camera + debug | [B9–B11](#b9b11--depth-sort-camera-debug-7375) | [`5_4_2_visual_debug_rendering.md`](core/5_4_2_visual_debug_rendering.md) |
| LDtk phase2 (post-7) | [Fase C](#fase-c--ldtk-phase2-post-fase-7) | [`ldtk-phase2-complete.md`](ldtk-doc/ldtk-phase2-complete.md) |
| 8.1 Animazioni sprite | [Fase 8.1](#fase-81--entity-animation-system) | [`ldtk-data-architecture.md`](ldtk-doc/ldtk-data-architecture.md) |
| 8.3 Party + RPG | [Fase 8.3](#fase-83--rpg-core--party-woes) | [`ldtk-entity-schema.md`](ldtk-doc/ldtk-entity-schema.md) |
| 8.2 Pathfinding | [Fase 8.2](#fase-82--mouse-navigation-opzionale) | phase1 (grid Collisions) |

---

## Snapshot progetto

| Aspetto | Valore |
|---------|--------|
| Tipo | RPG 2D fantasy **2.5D**, ispirato a *Warriors of the Eternal Sun* |
| Linguaggio | **C++23** |
| Grafica / audio | **SFML 2.6.1** |
| ECS | **EnTT 3.13** |
| JSON | **nlohmann_json 3.12** (CMake FetchContent) |
| Map editor | **LDtk** (export `.ldtk` / `.ldtkl`) |
| Tileset | Fantasy RPG by Franuka v1.9.7 — **32×32 px (2x)** consigliato |
| Risoluzione logica UI | **1024×768** fissa (`Config::Game::kLogicalWidth/Height`) |
| Risoluzione finestra | Configurabile via `ConfigManager` / JSON |
| Input esplorazione (Fase 7) | **WASD** + frecce (punta-e-clicca = Fase 8.2) |
| Party (futuro) | Max **3** personaggi, reclutamento NPC — **Fase 8.3** |

---

## Build e dipendenze

```bash
# Linux/WSL2: librerie dev SFML (vedi README.md)
cmake -B build && cmake --build build
./normalize_audio.sh ./raw_audio ./assets/audio   # prima del primo run
./build/game
```

- CMake scarica automaticamente SFML, EnTT, nlohmann_json.
- Struttura speculare: `include/` ↔ `src/`.
- Nuovi `.cpp` in `src/` vengono inclusi automaticamente (`GLOB_RECURSE` in `CMakeLists.txt`).

---

## Convenzioni codice (obbligatorie)

Seguire **[Naming Conventions](naming_conventions.md)** — Google C++ Style:

| Entità | Stile | Esempio |
|--------|-------|---------|
| Tipi, funzioni | PascalCase | `MapManager`, `LoadFromFile()` |
| Variabili, parametri | snake_case | `tile_size` |
| Membri privati | snake_case_ | `current_level_` |
| Costanti | kPascalCase | `kTileSize` |
| File | snake_case | `map_manager.hpp` |

---

## Architettura runtime

```
main.cpp → Game (fixed timestep 60 FPS)
              └── GameData (std::shared_ptr)
                    ├── sf::RenderWindow
                    ├── StateMachine  → MainMenuState | SettingsState | GameState
                    ├── AssetManager  (std::expected, async batch load)
                    ├── entt::registry
                    └── ConfigManager (JSON settings, fallback chain)
```

**Game loop:** `HandleInput` → `Update(dt)` solo stato attivo; `UpdateVisuals(dt)` per tutti gli stati nello stack; `Draw()`.

**Fase 7 aggiungerà:** `MapManager` in `GameData`, sistemi mappa in `GameState`.

---

## Regole d'oro — dove vive ogni dato

```
LDtk (.ldtk)  → layout spaziale, spawn, stato INIZIALE, metadata posizionamento
JSON (data/)  → archetipi riusabili (nemici, NPC, item, animazioni, terreno)
ECS           → runtime (HP, XP, party, AI, anim frame corrente, status)
```

| Domanda | Risposta |
|---------|----------|
| È legato alla posizione sulla mappa? | LDtk |
| È un archetipo riusabile? | JSON |
| Cambia durante il gameplay? | ECS |
| È bilanciamento da iterare spesso? | JSON (mai LDtk) |

Dettaglio: [`ldtk-data-architecture.md`](ldtk-doc/ldtk-data-architecture.md).

---

## Decisioni di design vincolanti

### Movimento

- Posizione **(x, y) float** — sub-tile, fluida (`MovementSystem` + `dt` fisso 1/60s).
- **WASD** in Fase 7; **punta-e-clicca A\*** solo in Fase 8.2.
- Fase 7: **un solo** personaggio con `PlayerComponent`.

### Elevazione dual-layer (Fase 7)

| Campo | Tipo | Fonte | Uso |
|-------|------|-------|-----|
| `floor_level` | `int` | IntGrid `Elevation` + runtime | Collisioni tra piani, gameplay |
| `height` | `float` | Runtime (interpolato su rampe) | Offset Y visivo, depth sort |
| `position` | `float` | `TransformComponent` | Movimento WASD |

- Cella piatta: `height ≈ floor_level * kHeightPerLevel`.
- Cella `Stairs`: `Collisions = Stairs`; IntGrid `Elevation` = piano **destinazione**; `height` interpola con `ramp_t ∈ [0,1]`; snap `floor_level` a destinazione (es. `ramp_t >= 0.5`).
- **Migrare** `TransformComponent.elevation` (float attuale) verso `ElevationComponent.height` — non duplicare.

```cpp
struct ElevationComponent {
    int floor_level = 0;
    float height = 0.f;
};
```

Sistema: `RampTransitionSystem` (non snap istantaneo int-only).

### Party WoES (Fase 8.3 — NON in Fase 7)

- Party max **3**: eroe + 2 slot reclutabili.
- Reclutamento/dismiss via dialoghi/quest; compagni **non** spawnati da LDtk al load.
- Movimento iniziale: **unità** (leader + follower), input sul leader.
- Un solo `PlayerStart` per mappa in LDtk.

### Rendering 2.5D

```cpp
sort_key = static_cast<int>(elev.height * kSortScale)
         + static_cast<int>(transform.position.y + foot_offset.y)
         + sprite.sort_offset;
```

`DepthSortSystem` (7.3) sostituisce lo `z_index` statico attuale in `RenderSystem`.

### View e camera

- Menu/Settings: view logica 1024×768.
- `GameState`: view **mondo** con `CameraSystem` che segue il player (7.5).

---

## Stato codebase (file chiave)

| Area | Path |
|------|------|
| Entry + loop | `src/main.cpp`, `src/core/Game.cpp`, `include/core/Game.hpp` |
| Stati | `src/states/MainMenuState.cpp`, `SettingsState.cpp`, `GameState.cpp` |
| ECS componenti | `include/components/Components.hpp` |
| Sistemi | `src/systems/RenderSystem.cpp`, `MovementSystem.cpp`, `PlayerInputSystem.cpp`, `DebugRenderSystem.cpp` |
| Risorse | `include/resources/AssetManager.hpp` |
| Config | `include/core/ConfigManager.hpp`, `include/core/Constants.hpp` |
| Settings JSON | `assets/config/default_settings.json`, `saves/user_settings.json` |
| Mappa (da creare) | `include/map/`, `src/map/`, `assets/maps/world/` |

### Componenti ECS attuali (pre-Fase 7)

```cpp
TransformComponent  // position, elevation (float, da migrare), scale, rotation
SpriteComponent     // sf::Sprite embedded, z_index
VelocityComponent   // velocity, speed
PlayerComponent     // tag vuoto
```

### Fasi completate (1–6)

| Fase | Contenuto |
|------|-----------|
| 1 | CMake C++23, struttura cartelle |
| 2 | `Game`, `StateMachine`, logging |
| 3 | `AssetManager`, menu, audio, UI |
| 4 | EnTT registry, componenti base, `RenderSystem` |
| 5 | Input WASD, movimento, debug tools (F1/F2) |
| 6 | `ConfigManager`, settings persistence, async asset load — **semi-completa, da testare end-to-end** |

---

## Fase A — Preparazione (doc + LDtk editor)

**Obiettivo:** ambiente documentazione e mappa test pronti prima/durante il codice C++.

### Task

- [ ] Correggere link in `doc/ldtk-doc/` (path relativi corretti).
- [ ] ~~`doc/ldtk-layer-structure.md`~~ — **eliminato**; fonte unica: `doc/ldtk-doc/`.
- [ ] Creare progetto LDtk **phase1**:
  - Layer: `Terrain`, `Decor`, `Buildings`
  - IntGrid: `Collisions`, `Elevation`
  - Entity: `PlayerStart`
  - Tile 32×32 Franuka
- [ ] Mappa test ~40×30: erba, acqua, cliff, scale (0→1), edificio, spawn.
- [ ] Export in `assets/maps/world/`.

### Criteri di accettazione

- File `.ldtk` + `.ldtkl` + tileset PNG in repo.
- Layer e IntGrid rispettano [`ldtk-phase1-integration.md`](ldtk-doc/ldtk-phase1-integration.md).

### Non fare

- Layer phase2 completi (Water, Canopy, …).
- Entity nemici/NPC/porte.

---

## Fase 7 — Mappa, collisioni, telecamera (LDtk)

**Obiettivo finale:** un personaggio si muove fluidamente (WASD) su mappa LDtk, collide, sale rampe con elevazione visiva continua, camera lo segue.

**Doc LDtk:** [`ldtk-index.md`](ldtk-doc/ldtk-index.md) → [`ldtk-phase1-integration.md`](ldtk-doc/ldtk-phase1-integration.md).

### Ordine implementazione consigliato

```
B1–B2 → B3–B4 → B5 → B7 → B6 → B8 → B9 → B10 → B11
```

(B6 e B7 possono essere invertiti; collisioni prima dell'elevazione è spesso più testabile.)

---

### B1–B2 — LdtkLoader + MapManager (7.1)

**Scope:** parsing JSON `.ldtk`, strutture dati, API grid.

**Creare:**

- `include/map/LdtkLoader.hpp`, `src/map/LdtkLoader.cpp`
- `include/map/MapManager.hpp`, `src/map/MapManager.cpp`
- Aggiungere `MapManager` a `GameData` in `include/core/Game.hpp`

**Contratto API (minimo):**

```cpp
struct LdtkLevel { /* tile_layers, int_grid_layers, entities, dimensioni */ };

class MapManager {
 public:
  std::expected<void, std::string> Load(const std::filesystem::path& path);
  const LdtkLevel& GetLevel() const;
  CollisionType CollisionAt(int tx, int ty) const;
  int ElevationAt(int tx, int ty) const;
  // Forward-compatible: ignorare layer sconosciuti
};
```

**Pattern:** `std::expected` come `ConfigManager`; log con `Logger::`.

**Criteri di accettazione:**

- [ ] Carica mappa test senza crash.
- [ ] `CollisionAt` / `ElevationAt` corretti su celle note.
- [ ] Layer non riconosciuti ignorati.

**Non fare:** rendering, ECS, collisioni entity.

---

### B3–B4 — Tileset + MapRenderSystem (7.2)

**Scope:** caricare tileset da path LDtk; disegnare tile layer con `sf::VertexArray`.

**Creare:** `include/systems/MapRenderSystem.hpp`, `src/systems/MapRenderSystem.cpp`

**Note:**

- Un `VertexArray` per layer (o batch per layer).
- `std::mdspan` opzionale per accesso grid type-safe (roadmap 7.2).
- Aggiungere `Config::Map::kTileSize = 32` in `Constants.hpp`.

**Criteri di accettazione:**

- [ ] Mappa visibile in `GameState::Draw` sotto le entity.
- [ ] Ordine layer: Terrain → Decor → Buildings.

---

### B5 — Integrazione GameState

**Scope:** load mappa in `GameState::Init`, chiamare `MapRenderSystem`, WASD invariato.

**Modificare:** `src/states/GameState.cpp`, `include/states/GameState.hpp`

**Criteri di accettazione:**

- [ ] Avvio partita mostra mappa + player (ancora spawn hardcoded ok temporaneamente).
- [ ] ESC torna al menu; registry pulito in distruttore.

---

### B6–B7 — Elevazione + collisioni (7.3–7.4)

**Scope:**

- `ElevationComponent { floor_level, height }`
- `RampTransitionSystem` — interpolazione float su `Stairs`
- `CollisionSystem` — AABB piedi vs IntGrid, posizione float
- `BoxColliderComponent` (size, offset, is_trigger)
- Estendere `MovementSystem` o pre-check collisioni prima del move

**IntGrid Collisions (Fase 1):**

| Valore | Significato |
|--------|-------------|
| 0 | Walkable |
| 1 | Blocked |
| 2 | Water |
| 3 | Stairs |

**Criteri di accettazione:**

- [ ] Player non attraversa muri/acqua.
- [ ] Su rampa, `height` cambia fluidamente; `floor_level` aggiornato con snap.
- [ ] Movimento sub-tile (non snap a griglia).

**Non fare:** party, NPC, pathfinding mouse.

---

### B8 — Spawn da PlayerStart

**Scope:** leggere entity `PlayerStart` da `MapManager`, posizionare player, applicare `facing`.

**Criteri di accettazione:**

- [ ] Player spawna sulla posizione LDtk, non al centro schermo.

---

### B9–B11 — Depth sort, camera, debug (7.3, 7.5)

**Scope:**

- `DepthSortSystem` con `height` float
- `CameraSystem` — view mondo segue player
- Estendere `DebugRenderSystem`: griglia tile, celle collision/elevation, hitbox (F1)

**Criteri di accettazione:**

- [ ] Entity ordinate correttamente davanti/dietro per Y + height.
- [ ] Camera segue player; UI menu non regressa.
- [ ] Debug overlay toggle funziona.

---

### Definition of Done — Fase 7 completa

- [ ] Mappa LDtk phase1 caricata e renderizzata.
- [ ] Un player, WASD, collisioni, rampe fluide, camera, debug.
- [ ] Nessun party, nessun pathfinding mouse, nessun `AnimatorComponent`.
- [ ] Build pulita; convenzioni Google rispettate.

---

## Fase C — LDtk phase2 (post Fase 7)

**Obiettivo:** layer completi Franuka, entity interattive, terreno avanzato.

**Doc:** [`ldtk-phase2-complete.md`](ldtk-doc/ldtk-phase2-complete.md), [`ldtk-entity-schema.md`](ldtk-doc/ldtk-entity-schema.md)

### Scope

- Splittare layer (`Water`, `Ground`, `Cliffs`, `Decor_Canopy`, …).
- IntGrid `TerrainFX` + `data/terrain/terrain_types.json`.
- `LdtkEntityFactory` per Door, NPC, EnemySpawn, Chest, …
- `AnimatedTileComponent` + `data/tile_anims/`.
- Pattern tile vuoto + entity per oggetti interattivi.

### Prerequisiti

- Fase 7 completata.
- Loader ignora layer sconosciuti (già richiesto in 7.1).

### Non fare in questa fase

- Party system (8.3).
- Combattimento completo.

---

## Fase 8.1 — Entity Animation System

**Obiettivo:** `AnimatorComponent` per personaggi (idle, walk, …).

**Dati:**

| Cosa | Dove |
|------|------|
| Definizione anim (frame, fps, file PNG) | `data/sprites/{id}.json` |
| Frame corrente, timer | ECS `AnimatorComponent` |
| Facing iniziale | LDtk `PlayerStart.facing` |

**Nota tileset Franuka:** PNG separati per azione (run, attack, …) — referenziati da JSON, non in LDtk.

**Prerequisito:** Fase 7 (personaggio visibile su mappa).

**Non fare:** party, pathfinding.

---

## Fase 8.3 — RPG Core + Party WoES

**Obiettivo:** party 3, reclutamento/dismiss NPC, hook quest/dialoghi, save party.

### Componenti previsti

```cpp
struct PartyState {
    static constexpr int kMaxMembers = 3;
    std::array<entt::entity, kMaxMembers> members{};
    int active_member_index = 0;
};
struct PartyMemberComponent { int slot_index; bool is_leader; };
struct RecruitableComponent { /* npc_template_id, dialogue_id, quest_flag */ };
```

### LDtk

- Entity `NPC` con `recruitable: true` (phase2 + JSON `data/npcs/`).
- Compagni entrano in party solo a runtime.

### Save

- Delta su stato iniziale LDtk; chiavi `ldtk_uid` per entity modificate.

**Prerequisito consigliato:** 8.1 (animazioni base).

---

## Fase 8.2 — Mouse Navigation (opzionale)

**Obiettivo:** click → path A* su grid `Collisions`.

**Prerequisito:** Fase 7 (grid collisioni funzionante).

**Nota:** indipendente dal party; può essere parallelo a 8.3 ma **dopo** Fase 7.

**Non usare** pathfinding per follow party nella prima iterazione.

---

## Ordine globale roadmap (7 → 8)

```
Fase 7 (engine + LDtk phase1)
    → LDtk phase2 (layer/entity estesi)
    → 8.1 Animator
    → 8.3 Party / RPG
    → 8.2 Pathfinding (opz.)
```

---

## Documentazione correlata

| Documento | Contenuto |
|-----------|-----------|
| [README.md](../README.md) | Build, roadmap sintetica |
| [naming_conventions.md](naming_conventions.md) | Stile codice |
| [core/0_structure_and_roadmap.md](core/0_structure_and_roadmap.md) | Struttura cartelle |
| [core/4_0_ecs.md](core/4_0_ecs.md) | ECS EnTT |
| [core/6_settings_persistence.md](core/6_settings_persistence.md) | ConfigManager |
| [ldtk-doc/ldtk-index.md](ldtk-doc/ldtk-index.md) | Indice LDtk |
| [ldtk-doc/ldtk-phase1-integration.md](ldtk-doc/ldtk-phase1-integration.md) | LDtk minimo Fase 7 |
| [ldtk-doc/ldtk-phase2-complete.md](ldtk-doc/ldtk-phase2-complete.md) | LDtk completo |
| [ldtk-doc/ldtk-data-architecture.md](ldtk-doc/ldtk-data-architecture.md) | LDtk vs JSON vs ECS |
| [ldtk-doc/ldtk-entity-schema.md](ldtk-doc/ldtk-entity-schema.md) | Custom fields entity |

---

## Prompt suggerito per avviare un agente

Copia e adatta:

```
Stai lavorando sul progetto RPG Adventure (C++23, SFML, EnTT).
Leggi OBBLIGATORIAMENTE doc/agent_development_guide.md, sezione [NOME FASE].
Rispetta naming_conventions.md e le regole "Non fare" della sezione.
Il tuo scope è solo: [descrizione task].
Criteri di accettazione: [dalla sezione].
Non implementare funzionalità di fasi future.
```
