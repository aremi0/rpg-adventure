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
| Revisione post-B8 (qualità) | [C — Revisione](#c--revisione-integrazione-post-b8) | questo doc |
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

- Menu/Settings: view logica 1024×768 (UI, nessun zoom mondo).
- `GameState`: view **mondo** con `CameraSystem` che segue il player (7.5).
- **Zoom per mondo:** il fattore di zoom non è globale — dipende dal livello LDtk caricato (es. overworld `1.0×`, dungeon `1.5×`–`2.0×`). Vedi [B9–B11 § Camera](#b911--depth-sort-camera-debug-7375).

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

- [x] Correggere link in `doc/ldtk-doc/` (path relativi corretti).
- [x] ~~`doc/ldtk-layer-structure.md`~~ — **eliminato**; fonte unica: `doc/ldtk-doc/`.
- [x] Creare progetto LDtk **phase1**:
  - Layer: `Terrain`, `Decor`, `Buildings`
  - IntGrid: `Collision` (alias `Collisions`), `Elevation`
  - Entity: `PlayerStart`
  - Tile 32×32 Franuka
- [x] Mappa test ~26×22: erba, acqua, cliff, scale (0→1), edificio, spawn.
- [x] Export in `assets/maps/world/`.

### Criteri di accettazione

- File `.ldtk` + tileset PNG in repo (`assets/maps/world/rpg-adventure-phase1.ldtk`, livello embedded).
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
B1–B2 → B3–B4 → B5 → B7 → B6 → B8 → C → B9 → B10 → B11
```

(B6 e B7 possono essere invertiti; collisioni prima dell'elevazione è spesso più testabile.)

---

### B1–B2 — LdtkLoader + MapManager (7.1)

**Scope:** parsing JSON `.ldtk`, strutture dati, API grid.

**Stato:** implementato — da testare end-to-end.

**Pattern adottati:** `std::expected` (come `ConfigManager`), `Logger::` per errori/warn/debug, costante unica `Config::Map::kTileSize`, nomi layer condivisi in `map/MapLayerNames.hpp` (no duplicazione).

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

- [x] Carica mappa test senza crash.
- [x] `CollisionAt` / `ElevationAt` corretti su celle note.
- [x] Layer non riconosciuti ignorati.

**Non fare:** rendering, ECS, collisioni entity.

---

### B3–B4 — Tileset + MapRenderSystem (7.2)

**Scope:** caricare tileset da path LDtk; disegnare tile layer con `sf::VertexArray`.

**Stato:** implementato — integrazione visiva in `GameState::Draw` = B5.

**Creare:** `include/map/Tileset.hpp`, `src/map/Tileset.cpp`, `include/systems/MapRenderSystem.hpp`, `src/systems/MapRenderSystem.cpp`

**Note:**

- Un `VertexArray` per layer (o batch per layer).
- `std::mdspan` opzionale per accesso grid type-safe (roadmap 7.2).
- Costante tile: `Config::Map::kTileSize` in `Constants.hpp` (fonte unica, 32 px).

**Criteri di accettazione:**

- [x] `MapRenderSystem::Build` produce batch per Terrain → Decor → Buildings.
- [x] Mappa visibile in `GameState::Draw` sotto le entity.
- [x] Ordine layer: Terrain → Decor → Buildings.

---

### B5 — Integrazione GameState

**Scope:** load mappa in `GameState::Init`, chiamare `MapRenderSystem`, WASD invariato.

**Stato:** implementato — da testare end-to-end.

**Modificare:** `src/states/GameState.cpp`, `include/states/GameState.hpp`

**Criteri di accettazione:**

- [x] Avvio partita mostra mappa + player (ancora spawn hardcoded ok temporaneamente).
- [x] ESC torna al menu; registry pulito in distruttore.

---

### B6–B7 — Elevazione + collisioni (7.3–7.4)

**Scope:**

- `ElevationComponent { floor_level, height }`
- `RampTransitionSystem` — interpolazione float su `Stairs`
- `CollisionSystem` — AABB piedi vs IntGrid, posizione float
- `BoxColliderComponent` (size, offset, is_trigger)
- Estendere `MovementSystem` o pre-check collisioni prima del move

**Stato:** implementato — da testare end-to-end.

**Pattern adottati:** `MapManager::CollisionAt` / `ElevationAt`, `Config::Map::kTileSize` / `kHeightPerLevel`, movimento axis-separated in `CollisionSystem`, rendering via `ElevationComponent::height`.

**IntGrid Collisions (Fase 1):**

| Valore | Significato |
|--------|-------------|
| 0 | Walkable |
| 1 | Blocked |
| 2 | Water |
| 3 | Stairs |

**Criteri di accettazione:**

- [x] Player non attraversa muri/acqua.
- [x] Su rampa, `height` cambia fluidamente; `floor_level` aggiornato con snap.
- [x] Movimento sub-tile (non snap a griglia).

**Non fare:** party, NPC, pathfinding mouse.

---

### B8 — Spawn da PlayerStart

**Scope:** leggere entity `PlayerStart` da `MapManager`, posizionare player, applicare `facing`.

**Stato:** implementato — verificato in gioco (spawn walkable, facing, debug F1).

**Nota:** le verifiche B6–B7 richiedono spawn corretto (il centro schermo `(512,384)` cadeva su tile Blocked).

**Criteri di accettazione:**

- [x] Player spawna sulla posizione LDtk, non al centro schermo.
- [x] `FacingComponent` valorizzato da campo LDtk `Facing`.
- [x] Log di validazione collisione su tile `PlayerStart`.
- [x] Fallback documentato se `PlayerStart` assente (warn + centro schermo → `FindNearbySpawnPosition` in `MapCollisionUtils`).

### Verifica manuale B6–B7 (dopo B8)

Prerequisito: Nuova Partita, **F1** attivo.

| Test | Esito atteso |
|------|--------------|
| Spawn | Pos ~`(176, 336)`, tile `(5,10)`, `Coll: Walkable` |
| WASD | Movimento libero su erba; `Pos` sub-tile |
| Muri/acqua | Player si ferma |
| Scale | `Height` fluido, `Floor` snap a metà rampa |
| Hitbox F1 | Rettangolo verde ai piedi + tile cyan sotto i piedi |

### Verifica manuale B8

| Test | Esito atteso |
|------|--------------|
| Posizione | Non al centro schermo; log `PlayerStart: grid (5,10)...` |
| Facing | Debug: `Facing: South` |
| Ciclo vita | ESC → Nuova Partita senza entity fantasma |

---

### C — Revisione integrazione (post-B8)

**Obiettivo:** revisione mirata di quanto integrato in B1–B8 prima di procedere con B9–B11. Nessuna nuova feature gameplay; solo qualità, UX display e hardening codice.

**Stato:** completato.

**Prerequisito:** B1–B8 implementati.

> **Implementazione futura:** letterbox/pillarbox per aspect 16:9 (via `sf::View::setViewport()`) — alternativa documentata in C.1, non implementata in C (scope minimo). Vedi anche roadmap in [`README.md`](../README.md).

#### C.1 — Risoluzione finestra e aspect ratio (stretch tile)

**Problema attuale:** la view logica è fissa **1024×768** (4:3) in [`Game.cpp`](src/core/Game.cpp) e [`SettingsState.cpp`](src/states/SettingsState.cpp). SFML scala quella view alla finestra intera → aspect ratio diverso = **stretch non uniforme** sui pixel.

| Risoluzione | Aspect | Rapporto con 1024×768 | Effetto sui tile |
|-------------|--------|------------------------|------------------|
| 800×600 | 4:3 | Uguale | Scala uniforme (~0.78×), **nessuno stretch** — immagine più piccola |
| 1024×768 | 4:3 | 1:1 | Pixel-perfect rispetto alla view logica |
| 1280×720 | 16:9 | Diverso | **Stretch orizzontale/verticale** — tile deformati |

**Raccomandazione (preferita):** whitelist solo risoluzioni **4:3** compatibili con la view logica, senza stretch:

- Tenere: `800×600`, `1024×768`
- Sostituire `1280×720` con **`1280×960`** (4:3, stesso aspect di 1024×768)
- Opzionale futuro: `1600×1200`, `2048×1536` per monitor 2K (sempre 4:3)

**Alternativa più robusta (se si vogliono aspect 16:9):** mantenere view logica 1024×768 e applicare **letterbox/pillarbox** via `sf::View::setViewport()` — i tile restano proporzionati, bande nere ai lati. Non stretchare mai la view logica.

> **Nota (implementazione futura):** letterbox/pillarbox per aspect 16:9 è documentato come alternativa futura; non implementato in C (scope minimo). Whitelist 4:3 adottata come soluzione corrente.

**File coinvolti:** [`include/core/Constants.hpp`](include/core/Constants.hpp) (`kSupportedResolutions`), [`include/core/DisplayUtils.hpp`](include/core/DisplayUtils.hpp) (`ApplyDisplayView()`), [`src/states/SettingsState.cpp`](src/states/SettingsState.cpp).

**Criteri:**

- [x] Nessuna risoluzione in whitelist deforma i tile (aspect 4:3 o letterbox).
- [x] Cambio risoluzione in Settings non rompe menu né GameState.

#### C.2 — Risoluzioni piccole su monitor Full HD / 2K

**Problema:** su monitor 1080p/1440p, `800×600` (finestra piccola, scala ~0.78×) fa apparire tile 32×32 troppo piccoli.

| Opzione | Pro | Contro |
|---------|-----|--------|
| **A. Rimuovere 800×600** | Semplice; niente stretch | Perde opzione laptop / finestra piccola |
| **B. Zoom camera solo in GameState** (B10) | Gameplay più grande; menu resta 1024×768 | Richiede `CameraSystem`; non aiuta menu |
| **C. Integer scale 2× con letterbox** | Pixel art nitida su 1080p | Bande nere; implementazione viewport |
| **D. Zoom globale view logica** | Tutto più grande | Rompe layout UI menu calibrato su 1024×768 |

**Raccomandazione:** combinare **A + B**:

1. **Rimuovere `800×600`** dalla whitelist (o spostarla in "legacy" non default) — evita esperienza troppo piccola senza fix dedicato.
2. **Zoom camera in `GameState`** (task B10, non qui): fattore iniziale ~`1.5×`–`2.0×` sulla view mondo, così i tile 32px restano leggibili su Full HD senza toccare la UI del menu.

Non applicare zoom stretch sulla finestra: preferire sempre integer scale o camera world zoom.

**Criteri:**

- [x] Decisione documentata in `Constants.hpp` (risoluzioni ammesse).
- [x] Su 1920×1080, gameplay tile leggibili (via camera zoom B10 o risoluzione default 1024×768 fullscreen/maximised).

#### C.3 — Transizione elevazione fluida (no “salto”)

**Problema attuale:** in [`RampTransitionSystem.cpp`](src/systems/RampTransitionSystem.cpp), sulle celle **piatte** `height` viene assegnato **istantaneamente**:

```cpp
elevation.height = floor_level * Config::Map::kHeightPerLevel;
```

Quando il player passa da un piano all’altro (esce da rampa, cambia tile di elevazione, o `floor_level` fa snap a metà rampa), l’offset Y visivo salta in un frame.

**Fix raccomandato:**

1. Introdurre `target_height` (da tile/rampa come ora).
2. Interpolare ogni frame: `height = lerp(height, target_height, kHeightLerpSpeed * dt)` (passare `dt` a `RampTransitionSystem::Update`).
3. Su celle `Stairs`: mantenere `ramp_t` per `target_height`; `floor_level` snap a `ramp_t >= 0.5` resta per gameplay.
4. Opzionale: aumentare `Config::Map::kHeightPerLevel` (es. 24–32) se il salto percepito è troppo piccolo visivamente ma ancora discontinuo.

**Non fare:** tornare a `TransformComponent.elevation`; usare solo `ElevationComponent.height`.

**Criteri:**

- [x] Passaggio piano 0→1 (rampa e pianerottolo) senza scatto visibile dello sprite.
- [x] `Floor` continua a fare snap gameplay; `Height` converge con float.

#### C.4 — Code review: file modificati B1–B8

Revisione sistematica dei file sotto (duplicazioni, null safety, C++23, convenzioni).

##### Mappa e dati LDtk

| File | Note review |
|------|-------------|
| [`include/map/LdtkTypes.hpp`](include/map/LdtkTypes.hpp) | Struct allineate a LDtk; `source_path` popolato |
| [`include/map/LdtkLoader.hpp`](include/map/LdtkLoader.hpp) / [`.cpp`](src/map/LdtkLoader.cpp) | `std::expected`; layer sconosciuti ignorati |
| [`include/map/MapManager.hpp`](include/map/MapManager.hpp) / [`.cpp`](src/map/MapManager.cpp) | Cache layer; `FindEntity`; tileset world |
| [`include/map/MapLayerNames.hpp`](include/map/MapLayerNames.hpp) | Single source nomi layer |
| [`include/map/Tileset.hpp`](include/map/Tileset.hpp) / [`.cpp`](src/map/Tileset.cpp) | Path risolto da `source_path` |
| [`include/map/LdtkEntityUtils.hpp`](include/map/LdtkEntityUtils.hpp) / [`.cpp`](src/map/LdtkEntityUtils.cpp) | Parse `field_instances`; `constexpr` string helpers |
| [`include/map/MapCollisionUtils.hpp`](include/map/MapCollisionUtils.hpp) / [`.cpp`](src/map/MapCollisionUtils.cpp) | `GetFootAabb`, `GetTileSize`, `WorldToTile` (consolidato) |

##### Sistemi e stati

| File | Note review |
|------|-------------|
| [`include/systems/MapRenderSystem.hpp`](include/systems/MapRenderSystem.hpp) / [`.cpp`](src/systems/MapRenderSystem.cpp) | Ordine layer; `texture_` null se `Draw` senza `Build` |
| [`include/systems/CollisionSystem.hpp`](include/systems/CollisionSystem.hpp) / [`.cpp`](src/systems/CollisionSystem.cpp) | Usa `MapCollisionUtils` |
| [`include/systems/RampTransitionSystem.hpp`](include/systems/RampTransitionSystem.hpp) / [`.cpp`](src/systems/RampTransitionSystem.cpp) | Lerp `height` con `kHeightLerpSpeed`; usa `MapCollisionUtils` |
| [`include/systems/RenderSystem.hpp`](include/systems/RenderSystem.hpp) / [`.cpp`](src/systems/RenderSystem.cpp) | Usa `ElevationComponent::height` |
| [`include/systems/DebugRenderSystem.hpp`](include/systems/DebugRenderSystem.hpp) / [`.cpp`](src/systems/DebugRenderSystem.cpp) | Usa `MapCollisionUtils`; `map` opzionale |
| [`include/states/GameState.hpp`](include/states/GameState.hpp) / [`.cpp`](src/states/GameState.cpp) | Ordine Init; `map_ready_` guard |
| [`src/core/Game.cpp`](src/core/Game.cpp) | `ApplyDisplayView()`; mappa in GameState |
| [`include/core/DisplayUtils.hpp`](include/core/DisplayUtils.hpp) / [`.cpp`](src/core/DisplayUtils.cpp) | View logica 1024×768 condivisa |

##### Core, componenti, risorse

| File | Note review |
|------|-------------|
| [`include/core/Game.hpp`](include/core/Game.hpp) | `MapManager` in `GameData` |
| [`include/core/Constants.hpp`](include/core/Constants.hpp) | `Config::Map::*`; risoluzioni (C.1–C.2) |
| [`include/components/Components.hpp`](include/components/Components.hpp) | `Elevation`, `BoxCollider`, `Facing` |
| [`include/resources/AssetManager.hpp`](include/resources/AssetManager.hpp) | Fallback usa `Config::Map::kTileSize` |

##### Documentazione

| File | Note review |
|------|-------------|
| [`doc/agent_development_guide.md`](doc/agent_development_guide.md) | Stato fasi |
| [`README.md`](README.md) | Roadmap 7.x |

##### Checklist review tecnica

**Duplicazioni da consolidare:**

- [x] `GetFootAabb`, `GetTileSize`, `WorldToTile` — estratti in [`include/map/MapCollisionUtils.hpp`](include/map/MapCollisionUtils.hpp).
- [x] `ApplyDisplayView()` — in [`include/core/DisplayUtils.hpp`](include/core/DisplayUtils.hpp).

**Null / sicurezza:**

- [x] `MapRenderSystem::Draw` con `texture_ == nullptr` — già guarded; verificare `map_ready_` in `GameState`.
- [x] `MapManager::Load` fallito → GameState avvia senza mappa; player senza collisioni significative — OK con warn.
- [x] `FindEntity` / `GetFieldString` su JSON malformato — `nlohmann` può throw; valutare try/catch in loader (già presente a livello file).
- [x] `entt::registry` dopo `clear()` — distruttore `GameState` OK.

**C++23 — opportunità non sfruttate:**

- [ ] `std::expected` — già usato in loader/MapManager/Tileset; estendere a parse entity se utile.
- [ ] `std::span` — candidato per `MapLayerNames` in `IsKnownLayer` (già in loader).
- [x] `std::ranges` — `FindEntityByIdentifier` usa `std::ranges::find_if`.
- [x] `constexpr` — `CollisionTypeToString` / `FacingToString` sono `constexpr`.
- [ ] `std::mdspan` — opzionale per `tile_data` (roadmap 7.2, non urgente).

**Vulnerabilità / robustezza:**

- [x] Path LDtk da costante — nessun input utente; rischio basso.
- [x] Nessuna allocazione hot-path nei sistemi per frame — OK.
- [x] `MovementSystem` non più usato in GameState — documentato in header.

**Criteri di completamento sezione C:**

- [x] C.1–C.3 implementati o scartati con decisione documentata.
- [x] C.4 checklist review completata; duplicazioni critiche consolidate.
- [x] Build pulita; test manuali B6–B8 ancora validi.

**Non fare in C:** party, LDtk phase2, `DepthSortSystem`/`CameraSystem` completi (restano B9–B11), salvo zoom camera se accoppiato esplicitamente a C.2.

---

### B9–B11 — Depth sort, camera, debug (7.3, 7.5)

**Scope:**

- `DepthSortSystem` con `height` float
- `CameraSystem` — view mondo segue player, **zoom configurabile per mondo**
- Estendere `DebugRenderSystem`: griglia tile, celle collision/elevation, hitbox (F1)

#### B10 — CameraSystem (follow + zoom per mondo)

**Obiettivo:** la camera segue il player in `GameState` e applica un **fattore di zoom dipendente dal mondo/livello caricato**, non un valore fisso per tutta la partita.

**Perché:** overworld e dungeon hanno esigenze diverse — in un dungeon ristretto (pochi tile visibili, corridoi) uno zoom maggiore migliora leggibilità e immersione; sull’overworld uno zoom più ampio mostra più contesto. Collegato anche alla revisione [C.2](#c2--risoluzioni-piccole-su-monitor-full-hd--2k) (tile leggibili su Full HD).

**Regola d’oro (dove vive il dato):**

| Dato | Dove | Esempio |
|------|------|---------|
| Zoom default di un **tipo** di mondo riusabile | JSON `data/world/` (futuro) | `"dungeon_default": 1.75` |
| Zoom **di questo livello** | LDtk — custom field sul livello | `camera_zoom: 1.5` su `Level_Dungeon_01` |
| Posizione/offset camera runtime | ECS o `CameraSystem` | follow player, clamp ai bordi mappa |

**Fase 7 (minimo):**

1. Custom field LDtk sul livello: `camera_zoom` (Float, default `1.0`) — estendere `LdtkLoader` / `LdtkLevel` come già fatto per `tile_size`.
2. Costanti fallback in `Config::Map`:
   ```cpp
   inline constexpr float kDefaultCameraZoom = 1.f;
   inline constexpr float kDungeonCameraZoom = 1.75f;  // usato se assente in LDtk e level_id contiene "dungeon" (euristica opzionale Fase 7)
   ```
3. `CameraSystem` espone zoom corrente; alla `Load` del livello legge `camera_zoom` da `MapManager::GetLevel()`.
4. Applicazione SFML: ridurre le dimensioni della **view mondo** (o equivalente `zoom` su `sf::View`) — view più piccola = più ingrandimento. **Non** scalare la finestra (evita stretch tile, vedi C.1).
5. Solo `GameState` usa la view mondo; `MainMenuState` / `SettingsState` restano su view logica 1024×768.

**Valori indicativi (iterabili in LDtk):**

| Tipo mondo | `camera_zoom` | Note |
|------------|---------------|------|
| Overworld / villaggio | `1.0` | Campo visivo ampio |
| Interni / dungeon | `1.5` – `2.0` | Corridoi, stanze piccole |
| Boss arena | `1.25` | Compromesso |

**API prevista (indicativa):**

```cpp
class CameraSystem {
 public:
  static void InitFromLevel(const LdtkLevel& level);
  static void Update(entt::registry& registry, const MapManager& map);
  static void Apply(sf::RenderWindow& window);
  [[nodiscard]] static float GetZoom();
};
```

**Criteri di accettazione (camera):**

- [ ] Camera segue il centro del player (o offset configurabile).
- [ ] Zoom letto dal livello LDtk (`camera_zoom`); fallback `Config::Map::kDefaultCameraZoom`.
- [ ] Cambio mappa (futuro) può cambiare zoom senza modificare `CameraSystem`.
- [ ] Menu/Settings non usano la view mondo — nessuna regressione UI.
- [ ] (Stretch) Lo zoom camera non sostituisce il letterbox 4:3 di C.1 — sono complementari.

**Non fare in B10:**

- Zoom legato alla risoluzione finestra (quello è display scaling, C.1).
- Zoom diverso per entity singole.
- Smooth zoom animato al cambio livello (opzionale post-Fase 7).

#### B9 — DepthSortSystem

- Sostituisce `z_index` statico; sort key con `ElevationComponent.height` + `foot_y`.

#### B11 — Debug overlay mappa

- Griglia tile, celle collision/elevation, hitbox (F1) — estensione `DebugRenderSystem`.

**Criteri di accettazione (complessivi B9–B11):**

- [ ] Entity ordinate correttamente davanti/dietro per Y + height.
- [ ] Camera segue player con zoom per mondo; UI menu non regressa.
- [ ] Debug overlay toggle funziona.

---

### Definition of Done — Fase 7 completa

- [ ] Mappa LDtk phase1 caricata e renderizzata.
- [ ] Un player, WASD, collisioni, rampe fluide, camera, debug.
- [ ] Nessun party, nessun pathfinding mouse, nessun `AnimatorComponent`.
- [ ] Build pulita; convenzioni Google rispettate.

---

## Fase D — LDtk phase2 (post Fase 7)

**Obiettivo:** layer completi Franuka, entity interattive, terreno avanzato.

**Doc:** [`ldtk-phase2-complete.md`](ldtk-doc/ldtk-phase2-complete.md), [`ldtk-entity-schema.md`](ldtk-doc/ldtk-entity-schema.md)

### Scope

- Splittare layer (`Water`, `Ground`, `Cliffs`, `Decor_Canopy`, …).
- IntGrid `TerrainFX` + `data/terrain/terrain_types.json`.
- `LdtkEntityFactory` per Door, NPC, EnemySpawn, Chest, …
- `AnimatedTileComponent` + `data/tile_anims/`.
- Pattern tile vuoto + entity per oggetti interattivi.
- **Raffinamento collisioni** — decor senza BLOCKED, entity con hitbox stretta, (opz.) IntGrid sub-tile; vedi [`ldtk-phase2-complete.md` § Raffinamento collisioni](ldtk-doc/ldtk-phase2-complete.md#raffinamento-collisioni-task-dedicato).

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
