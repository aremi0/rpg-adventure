# Fase 2 — Layering Completo (Fantasy RPG Franuka)

> Setup completo per mappa RPG con tutte le meccaniche previste.  
> Si costruisce **aggiungendo** layer e entity alla Fase 1.

---

## Layer visivi — Fase 2

Ordine di disegno (bottom → top):

| # | Layer | Tipo | Contenuto |
|---|-------|------|-----------|
| 1 | `Water` | Tiles | Lago, fiume — tile statici |
| 2 | `Waterfall` | Tiles | Cascate (frame 0; animazione in engine) |
| 3 | `Ground` | Tiles | Erba, terra, sabbia — autotile |
| 4 | `Paths` | Tiles | Sentieri sterrati e lastricati |
| 5 | `Cliffs` | Tiles | Pareti verticali tra livelli |
| 6 | `Shadows` | Tiles | Ombre sotto cliff, alberi, case |
| 7 | `Ground_Upper` | Tiles | Piano superiore (se presente) |
| 8 | `Stairs` | Tiles | Tile visivi scale salita/discesa |
| 9 | `Bridges` | Tiles | Ponti sopra l'acqua |
| 10 | `Decor_Floor` | Tiles | Fiori, funghi, sassi, siepi basse |
| 11 | `Farming` | Tiles | Orti, recinti (opzionale) |
| 12 | `Buildings` | Tiles | Solo edifici **statici e non interattivi** |
| 13 | `Decor_Canopy` | Tiles | Chiome alberi, tetto sporgente |

### Note per layer specifici

**`Water` + `Bridges`**
- L'acqua ha `Collisions = Water`
- Il ponte ha tile visivo su layer `Bridges` e `Collisions = Walkable` sulla stessa cella
- Alternativa: IntGrid separato `BridgeMask` (0/1)

**`Waterfall`**
- Animazione gestita dall'engine (`AnimatedTileComponent`)
- Collision di solito `Blocked` o `Water`

**`Stairs`**
- Solo grafica; la logica è su `Collisions = Stairs` + `Elevation` grid
- La cella stairs ha `elevation` = piano di destinazione

**`Decor_Canopy`**
- Parte alta degli alberi che occlude il player
- Per alberi grandi: tronco in entity `ResourceNode` (Y-sort), chioma in `Decor_Canopy` tile

**`Buildings` vs Entity**
- Case fisse mai distruttibili → tile `Buildings`
- Porte, muri distruttibili, botole → **Entity** (tile layer vuoto sotto)

---

## IntGrid logici — Fase 2

### `Collisions`

| Valore | Enum | Significato |
|--------|------|-------------|
| `0` | `Walkable` | Camminabile |
| `1` | `Blocked` | Ostacolo fisso |
| `2` | `Water` | Acqua |
| `3` | `Stairs` | Transizione elevazione |
| `4` | `Pit` | Caduta, lava, buco (danno o warp) |
| `5` | `Trigger` | Zona evento (anche gestita da entity) |

### `Elevation`

| Valore | Significato |
|--------|-------------|
| `0` | Piano base |
| `1` | Primo rialzo |
| `2` | Secondo rialzo |
| `n` | Estendibile |

### `TerrainFX` (nuovo in Fase 2)

Valori numerici che mappano a `data/terrain/terrain_types.json`:

| Valore | ID JSON | Effetto esempio |
|--------|---------|-----------------|
| `0` | `normal` | Nessun modificatore |
| `1` | `wet` | `movement_speed × 0.8`, fulmine × 1.5 |
| `2` | `mud` | `movement_speed × 0.6` |
| `3` | `ice` | `movement_speed × 0.5`, rischio scivolamento |
| `4` | `holy_ground` | rigenerazione lenta |
| `5` | `cursed` | `poison_chance` per turno |

```json
// data/terrain/terrain_types.json
{
  "wet": {
    "movement_speed_mult": 0.8,
    "elemental_mult": { "lightning": 1.5, "fire": 0.5 },
    "on_enter_status": ["wet"],
    "on_enter_chance": 1.0
  }
}
```

---

## Entity — Fase 2 (panoramica)

| Entity LDtk | Scopo |
|-------------|-------|
| `PlayerStart` | Spawn giocatore |
| `EnemySpawn` | Spawn nemico |
| `NPC` | Personaggio dialogo/quest/shop |
| `Animal` | Fauna neutrale/ostile |
| `Door` | Porta apribile/chiusa/distruttibile |
| `Trapdoor` | Botola verso interior/dungeon |
| `Portal` | Cambio mappa/livello |
| `Destructible` | Muro, barile, cancello distruttibile |
| `ResourceNode` | Albero, miniera, pianta raccoglibile |
| `Chest` | Forziere loot |
| `TriggerZone` | Area evento (cutscene, ambush) |
| `LightSource` | Torcia, fuoco (opzionale, per illuminazione) |

Schema dettagliato: [ldtk-entity-schema.md](ldtk-entity-schema.md)

---

## Gestione oggetti interattivi

### Pattern "tile vuoto + entity"

```
LDtk editor:
  Ground layer:     [erba]
  Buildings layer:  [vuoto]        ← nessun tile
  Collisions:       [Blocked]
  Entity Door:      posizionata qui con sprite porta chiusa
```

### Runtime

```cpp
// All'interazione o al danno
if (door.is_open) {
    door.sprite = textures["door_open"];
    collision_grid[door.cell] = CollisionType::Walkable;
} else if (door.is_destroyed) {
    registry.destroy(door.entity);
    collision_grid[door.cell] = CollisionType::Walkable;
    spawn_loot(door.loot_table_id, door.position);
}
```

### Albero abbattibile

```
Spawn:    Entity ResourceNode (type=tree), collision Blocked sotto
Abbattuto: destroy entity, collision → Walkable, spawn entity/log loot
```

---

## Ponti — logica collisioni

```cpp
bool MapManager::is_walkable(int tx, int ty) const {
    const auto col = collision_at(tx, ty);
    if (col == CollisionType::Walkable) return true;
    if (col == CollisionType::Water && bridge_mask_at(tx, ty)) return true;
    return false;
}
```

---

## Occlusione 2.5D — Fase 2

### Albero diviso (tronco + chioma)

| Parte | Dove | Sort |
|-------|------|------|
| Tronco | Entity `ResourceNode` | Y-sort con player |
| Chioma | Tile `Decor_Canopy` | `sort_offset` alto, stessa Y base |

### Edificio con tetto sporgente

- Base muro: tile `Buildings` o entity `Destructible`
- Tetto sporgente: tile `Decor_Canopy`

### Cliff di livello superiore

```cpp
bool is_occluded_by_cliff(const Entity& player, const Cliff& cliff) {
    return player.elevation < cliff.elevation
        && player.foot_y < cliff.occlusion_line_y;
}
```

Metadata LDtk su tile cliff (tile custom fields):
- `occlusion_line_y` — riga Y sopra la quale il cliff copre entità più basse

---

## Animazioni tile — Fase 2

| Layer | File spritesheet | FPS suggerito |
|-------|------------------|---------------|
| `Water` | `water.png` | 8 |
| `Waterfall` | `waterfall.png` | 12 |
| `Decor_Floor` (fuochi, ecc.) | vari | 6–10 |

```cpp
struct AnimatedTileComponent {
    std::string anim_id;       // "water_shore", "waterfall"
    int current_frame = 0;
    float elapsed = 0.f;
};
```

Definizioni animazione in `data/tile_anims.json`, non in LDtk.

---

## Transizione tra mappe

### Entity `Portal` / `Trapdoor` / `Door`

Campi LDtk (vedi schema):
- `target_level_iid` — IID del livello LDtk destinazione (es. `"Level_Interior_Blacksmith"`)
- `target_x`, `target_y` — posizione spawn nel livello target
- `transition_type` — `walk_through`, `interact`, `fade`

```cpp
void PortalSystem::activate(const PortalComponent& portal, Entity player) {
    auto& game = Game::instance();
    game.load_level(portal.target_level_iid);
    player.get<TransformComponent>().position = {portal.target_x, portal.target_y};
}
```

---

## Checklist Fase 2

### Layer
- [ ] Separare `Terrain` Fase 1 in layer dedicati
- [ ] Aggiungere `TerrainFX` IntGrid
- [ ] Aggiungere `Decor_Canopy` per occlusione

### Entity
- [ ] Implementare factory `LdtkEntityFactory` con dispatch per `identifier`
- [ ] Template JSON per ogni `template_id`
- [ ] Sistema `InteractableSystem` generico

### Engine
- [ ] `TerrainEffectSystem` — applica modificatori da `TerrainFX`
- [ ] `StatusEffectSystem` — gestisce wet, poison, bleed
- [ ] `DamageSystem` — soglia minima danno per destructible
- [ ] `LootSystem` — tabelle loot da JSON

---

## Riferimenti

- [ldtk-index.md](ldtk-index.md) — indice LDtk
- [agent_development_guide.md](../agent_development_guide.md) — guida agenti per fase
- [ldtk-entity-schema.md](ldtk-entity-schema.md) — custom fields
- [ldtk-data-architecture.md](ldtk-data-architecture.md) — LDtk vs JSON vs ECS
