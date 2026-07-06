# Fase 1 — Integrazione LDtk Minima

> Obiettivo: integrare la mappa nell'engine durante la **Fase 7** della roadmap.  
> Setup volutamente semplice, progettato per espandersi senza breaking changes.

**Guida agenti:** [Agent Development Guide](../agent_development_guide.md)

---

## Scope Fase 1

| Incluso | Escluso (Fase 2) |
|---------|------------------|
| Rendering tile layer | Ponti, cascate, canopy separati |
| IntGrid Collisions + Elevation | TerrainFX, status da terreno |
| PlayerStart entity | Nemici, NPC, oggetti interattivi |
| Camera follow | Pathfinding, animazioni tile, party |
| Box collision base | Danno, loot, porte, NPC reclutabili |
| Y-sort base (foot_y + height) | Occlusione avanzata per alberi |

---

## Layer LDtk — Fase 1

Ordine di disegno (bottom → top):

| # | Nome layer | Tipo | Note |
|---|------------|------|------|
| 1 | `Terrain` | Tiles | Acqua + erba + terra + cliff + scale in un unico layer |
| 2 | `Decor` | Tiles | Cespugli, fiori, rocce — puramente decorativi |
| 3 | `Buildings` | Tiles | Case statiche, recinti |
| 4 | `Collisions` | IntGrid | Logica camminabile |
| 5 | `Elevation` | IntGrid | Livello 0, 1, 2… per cella |
| 6 | `Entities` | Entities | Solo `PlayerStart` per ora |

### Perché un layer `Terrain` unico

In Fase 1 non serve separare acqua/ground/cliff. Disegni tutto in un layer e ti concentri sul loader. In Fase 2 **aggiungi** layer senza togliere `Terrain` (che puoi svuotare o rinominare gradualmente).

---

## IntGrid `Collisions` — Fase 1

| Valore | Enum LDtk | Significato |
|--------|-----------|-------------|
| `0` | `Walkable` | Camminabile |
| `1` | `Blocked` | Muro, cliff, edificio |
| `2` | `Water` | Acqua — non camminabile |
| `3` | `Stairs` | Zona transizione elevazione |

```cpp
enum class CollisionType : uint8_t {
    Walkable = 0,
    Blocked  = 1,
    Water    = 2,
    Stairs   = 3
};
```

### Logica scale / rampe (Fase 1)

Elevazione **dual-layer**: IntGrid intero per cella; runtime float per transizione fluida.

```cpp
struct ElevationComponent {
    int floor_level = 0;   // piano gameplay (intero)
    float height = 0.f;    // altezza continua per render e depth sort
};

void RampTransitionSystem::Update(entt::entity player, GridPos cell) {
    if (collision_grid[cell] != CollisionType::Stairs) {
        // Cella piatta: height = floor_level * kHeightPerLevel
        return;
    }
    const int to_level = elevation_grid[cell];
    const int from_level = /* dedotto da celle adiacenti walkable */;
    const float ramp_t = /* 0..1 in base a posizione sulla rampa */;
    auto& elev = registry.get<ElevationComponent>(player);
    elev.height = std::lerp(from_level * kHeightPerLevel,
                            to_level * kHeightPerLevel, ramp_t);
    if (ramp_t >= 0.5f) elev.floor_level = to_level;  // snap gameplay
}
```

Non servono `ramp_up` / `ramp_down`: usa `Stairs` + IntGrid `Elevation` (piano destinazione).

**Input:** solo WASD (`PlayerInputSystem`); punta-e-clicca = Fase 8.2.

---

## IntGrid `Elevation` — Fase 1

| Valore | Significato |
|--------|-------------|
| `0` | Piano base (acqua, isole basse) |
| `1` | Piano rialzato (villaggio) |
| `2` | Piano alto (torri, altipiani) |

Ogni cella walkable ha un valore di elevazione. Le celle `Stairs` hanno l'elevazione del piano **di destinazione**.

---

## Entity — Fase 1

### `PlayerStart`

| Campo | Tipo LDtk | Default | Note |
|-------|-----------|---------|------|
| `facing` | Enum | `South` | `North`, `East`, `South`, `West` |
| `spawn_id` | String | `"default"` | Per respawn/checkpoint futuri |

Nessun altro campo in Fase 1. Stats e equipaggiamento restano hardcoded o in JSON separato.

---

## Struttura file LDtk — Fase 1

```
world/
  world.ldtk              # Progetto LDtk
  World_Level_0.ldtkl     # Livello overworld test
  FantasyRPG_32px.png     # Tileset (2x)
```

### Campi custom sul livello

| Campo | Tipo | Default | Note |
|-------|------|---------|------|
| `tile_size` | Int | `32` | Per validazione loader |
| `bg_color` | String | `"#1a1a2e"` | Colore sfondo fuori mappa |

---

## Loader C++ — contratto Fase 1

```cpp
struct LdtkLayer {
    std::string identifier;
    int grid_size;
    std::vector<uint32_t> tile_data;  // gid, 0 = empty
};

struct LdtkIntGridLayer {
    std::string identifier;
    std::vector<int> values;          // flat row-major
    int width, height;
};

struct LdtkEntity {
    std::string identifier;           // "PlayerStart"
    float px_x, px_y;
    nlohmann::json field_instances;   // custom fields raw
};

struct LdtkLevel {
    std::string identifier;
    int world_x, world_y;
    int px_width, px_height;
    std::vector<LdtkLayer> tile_layers;
    std::vector<LdtkIntGridLayer> int_grid_layers;
    std::vector<LdtkEntity> entities;
};

class MapManager {
public:
    std::expected<LdtkLevel, std::string> load(const std::filesystem::path& path);

    // Accesso rapido per sistemi
    const LdtkIntGridLayer& collisions() const;
    const LdtkIntGridLayer& elevation() const;
    CollisionType collision_at(int tx, int ty) const;
    int elevation_at(int tx, int ty) const;
};
```

Usa `std::expected` come da roadmap 7.1.

---

## Componenti ECS — Fase 1

Allineare con [`Components.hpp`](../../include/components/Components.hpp) esistente; estendere senza breaking change inutile.

```cpp
struct TransformComponent {
    sf::Vector2f position;
    sf::Vector2f foot_offset{0.f, 0.f};  // punto sorting (piedi)
    sf::Vector2f scale{1.f, 1.f};
    float rotation = 0.f;
    // NOTA: migrare TransformComponent.elevation (float legacy) → ElevationComponent.height
};

struct ElevationComponent {
    int floor_level = 0;
    float height = 0.f;
};

struct VelocityComponent {
    sf::Vector2f velocity;
    float speed = 200.f;
};

struct PlayerComponent {};  // tag — un solo player in Fase 7

struct BoxColliderComponent {
    sf::Vector2f size;
    sf::Vector2f offset;
    bool is_trigger = false;
};

struct SpriteComponent {
    sf::Sprite sprite;       // embedded, come in codebase attuale
    int sort_offset = 0;
    // z_index statico → sostituito da DepthSortSystem in 7.3
};
```

---

## Sistemi — Fase 1

| Sistema | Fase roadmap | Input |
|---------|--------------|-------|
| `MapRenderSystem` | 7.2 | tile layers → `sf::VertexArray` |
| `DepthSortSystem` | 7.3 | `height` float + `foot_y` + `sort_offset` |
| `MovementSystem` | 5.3 | velocity, posizione float |
| `RampTransitionSystem` | 7.3 | stairs + elevation grid, interpolazione `height` |
| `CollisionSystem` | 7.4 | box vs grid (sub-tile) |
| `CameraSystem` | 7.5 | player position |
| `PlayerInputSystem` | 5.2 | WASD → `VelocityComponent` |

### Depth sort minimale

```cpp
struct DrawItem {
    entt::entity entity;
    int sort_key;
};

std::vector<DrawItem> items;
for (auto [e, t, elev, spr] : view<TransformComponent, ElevationComponent, SpriteComponent>()) {
    int key = static_cast<int>(elev.height * kSortScale)
            + static_cast<int>(t.position.y + t.foot_offset.y)
            + spr.sort_offset;
    items.push_back({e, key});
}
std::ranges::sort(items, {}, &DrawItem::sort_key);
```

---

## Mappa test consigliata

Per validare Fase 1, crea una mappa piccola (~40×30 tile) con:

- [ ] Zona erba walkable
- [ ] Zona acqua blocked (collision = Water)
- [ ] Cliff con collision blocked
- [ ] Scale che portano da elevation 0 a 1
- [ ] Almeno un edificio statico (tile Buildings)
- [ ] PlayerStart posizionato su erba

---

## Checklist espansione → Fase 2

Quando passi a Fase 2, verifica:

- [ ] Il loader ignora layer sconosciuti senza crash
- [ ] `MapManager` espone API generiche (`get_int_grid("TerrainFX")`)
- [ ] Le entity hanno parsing generico dei `field_instances` (json)
- [ ] `DepthSortSystem` supporta già `sort_offset` per canopy
- [ ] `RampTransitionSystem` supporta interpolazione float e più di 2 livelli
- [ ] `BoxColliderComponent` supporta `is_trigger`

Se tutti ✅, la transizione è aggiungere layer e entity senza refactoring.

---

## Prossimo passo

Dopo aver validato il movimento su mappa: [ldtk-phase2-complete.md](ldtk-phase2-complete.md)
