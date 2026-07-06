# Schema Entity LDtk — Custom Fields Standardizzati

> Definizioni complete per ogni entity LDtk del progetto RPG fantasy.  
> Convenzioni: tipi LDtk nativi, default indicati, `template_id` punta a JSON in `data/`.

---

## Convenzioni globali

### Campi comuni (tutte le entity)

Questi campi si possono definire come **Field Definitions** a livello di progetto LDtk e riusarli:

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | `""` | ID archetipo JSON (es. `"goblin"`, `"oak_tree"`) |
| `facing` | Enum | `South` | `North`, `East`, `South`, `West` |
| `sort_offset` | Int | `0` | Offset Y per depth sort |
| `foot_y` | Int | `0` | Offset verticale punto piedi (px) |
| `notes` | String | `""` | Note designer (ignorate dall'engine) |

### Enum globali LDtk

```
Facing:     North, East, South, West
Disposition: NeutralPassive, NeutralFlee, NeutralFight, Hostile, Friendly
DamageType: Slashing, Piercing, Bludgeoning, Fire, Ice, Lightning, Poison, Holy, Dark
InitialState: Intact, Open, Closed, Locked, Destroyed, Looted, Depleted
TransitionType: WalkThrough, Interact, Fade, Instant
```

### Pattern `template_id` + override

```
Se override == 0 o "" → usa valore del template JSON
Se override != 0      → sovrascrive il template
```

---

## `PlayerStart`

Spawn del giocatore. Nessuna stat RPG.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `facing` | Enum | `South` | Direzione iniziale |
| `spawn_id` | String | `"default"` | ID per respawn/checkpoint |
| `player_profile` | String | `"default"` | Profilo JSON (`data/player/default.json`) |

```json
// Esempio LDtk field instances
{
  "facing": "South",
  "spawn_id": "village_square",
  "player_profile": "default"
}
```

---

## `EnemySpawn`

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | **obbligatorio** | Archetipo in `data/enemies/` |
| `level_override` | Int | `0` | 0 = usa livello template |
| `facing` | Enum | `South` | |
| `patrol_path_id` | String | `""` | ID entity `PatrolPath` o layer separato |
| `aggro_range_override` | Int | `0` | px; 0 = usa template |
| `respawn` | Bool | `false` | Respawna dopo morte |
| `respawn_sec` | Float | `60` | Secondi per respawn |
| `spawn_trigger` | String | `""` | ID `TriggerZone` che attiva lo spawn |

**Stats, loot, AI:** dal JSON `data/enemies/{template_id}.json`.

---

## `NPC`

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | **obbligatorio** | Archetipo in `data/npcs/` |
| `facing` | Enum | `South` | |
| `interactable` | Bool | `true` | Mostra prompt interazione |
| `dialogue_id` | String | `""` | Override dialogo (`data/dialogues/`) |
| `has_quest` | Bool | `false` | |
| `quest_ids` | String | `""` | CSV: `"quest_01,quest_02"` |
| `has_shop` | Bool | `false` | |
| `shop_id` | String | `""` | `data/shops/{shop_id}.json` |
| `killable` | Bool | `false` | Può essere ucciso |
| `faction` | String | `"civilian"` | `data/factions/factions.json` |
| `disposition` | Enum | `NeutralPassive` | Comportamento se attaccato |

### JSON archetipo NPC (esempio)

```json
{
  "id": "blacksmith",
  "display_name": "Fabbro Borin",
  "sprite": "blacksmith",
  "dialogue_id": "blacksmith_greet",
  "has_shop": true,
  "shop_id": "blacksmith_shop",
  "killable": false,
  "faction": "village"
}
```

---

## `Animal`

Fauna: pecore, maiali, galline, cavalli.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | **obbligatorio** | `data/animals/` |
| `facing` | Enum | `South` | |
| `disposition` | Enum | `NeutralFlee` | Vedi tabella sotto |
| `aggro_range` | Int | `0` | px; per `Hostile` |
| `flee_range` | Int | `80` | px; per `NeutralFlee` |
| `killable` | Bool | `true` | |
| `loot_table` | String | `""` | Override loot |
| `xp_reward` | Int | `0` | 0 = usa template |
| `wander_radius` | Int | `64` | Raggio vagabondaggio (px) |

### Disposition

| Valore | Comportamento |
|--------|---------------|
| `NeutralPassive` | Ignora attacchi (o immune) |
| `NeutralFlee` | Scappa se colpito |
| `NeutralFight` | Contrattacca se colpito |
| `Hostile` | Attacca se player entro `aggro_range` |
| `Friendly` | Mai ostile, non attacca |

---

## `Door`

Porte apribili/chiudibili/distruttibili. **Tile layer vuoto sotto.**

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | `"wooden_door"` | `data/doors/` |
| `facing` | Enum | `South` | Orientamento porta |
| `initial_state` | Enum | `Closed` | `Closed`, `Open`, `Locked`, `Destroyed` |
| `locked` | Bool | `false` | Richiede chiave |
| `key_id` | String | `""` | ID oggetto chiave (`data/items/keys/`) |
| `destroyable` | Bool | `true` | Può essere distrutta |
| `hp_override` | Int | `0` | 0 = usa template |
| `min_damage_threshold` | Int | `0` | 0 = usa template |
| `target_level_iid` | String | `""` | Livello interior (se porta verso mappa) |
| `target_x` | Int | `0` | Spawn X nel livello target |
| `target_y` | Int | `0` | Spawn Y nel livello target |
| `transition_type` | Enum | `Interact` | Come si attiva |
| `blocks_when_closed` | Bool | `true` | Collisione quando chiusa |

### Stati runtime (ECS `DoorComponent`)

```
Closed  → sprite chiusa, collision blocked
Open    → sprite aperta, collision walkable
Locked  → interazione mostra "Serve chiave X"
Destroyed → entity rimossa, walkable
```

---

## `Trapdoor`

Botola a pavimento verso dungeon/interior.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | `"wooden_trapdoor"` | |
| `initial_state` | Enum | `Closed` | |
| `locked` | Bool | `false` | |
| `key_id` | String | `""` | |
| `destroyable` | Bool | `false` | Di solito non distruttibile |
| `target_level_iid` | String | **obbligatorio** | |
| `target_x` | Int | **obbligatorio** | |
| `target_y` | Int | **obbligatorio** | |
| `transition_type` | Enum | `Interact` | |
| `requires_interact` | Bool | `true` | `false` = walk-through |

---

## `Portal`

Cambio mappa senza sprite porta (archi, rune, confine zona).

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `target_level_iid` | String | **obbligatorio** | |
| `target_x` | Int | **obbligatorio** | |
| `target_y` | Int | **obbligatorio** | |
| `transition_type` | Enum | `WalkThrough` | |
| `width` | Int | `32` | Larghezza hitbox (px) |
| `height` | Int | `32` | Altezza hitbox (px) |
| `one_way` | Bool | `false` | Solo entrata |
| `required_flag` | String | `""` | Flag quest/story per attivare |

---

## `Destructible`

Muri, barili, cancelli, ponti distruttibili.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | **obbligatorio** | `data/destructibles/` |
| `facing` | Enum | `South` | |
| `initial_state` | Enum | `Intact` | |
| `hp_override` | Int | `0` | |
| `min_damage_threshold` | Int | `0` | Danno minimo per ricevere danno |
| `defense_override` | Int | `0` | |
| `loot_table` | String | `""` | Override loot al destroy |
| `blocks_path` | Bool | `true` | Aggiorna collision a walkable se distrutto |
| `faction` | String | `""` | Se attaccabile solo da certe fazioni |

### JSON archetipo esempio

```json
{
  "id": "brick_wall",
  "sprite": "brick_wall",
  "hp": 30,
  "defense": 5,
  "min_damage_threshold": 10,
  "damage_types_allowed": ["bludgeoning", "slashing", "fire"],
  "loot_table": null
}
```

---

## `ResourceNode`

Alberi, miniere, cespugli raccoglibili.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | **obbligatorio** | `data/resources/` |
| `initial_state` | Enum | `Intact` | `Intact`, `Depleted` |
| `hp_override` | Int | `0` | Per alberi abbattibili |
| `min_damage_threshold` | Int | `0` | Es. ascia necessaria |
| `required_tool_tag` | String | `""` | Es. `"axe"`, `"pickaxe"` |
| `respawn` | Bool | `false` | |
| `respawn_sec` | Float | `300` | |
| `loot_table` | String | `""` | |
| `blocks_path` | Bool | `true` | Albero blocca finché abbattuto |
| `has_canopy` | Bool | `false` | Se true, chioma in `Decor_Canopy` tile |

### JSON archetipo albero

```json
{
  "id": "oak_tree",
  "sprite": "oak_tree",
  "hp": 20,
  "min_damage_threshold": 5,
  "required_tool_tag": "axe",
  "loot_table": "oak_tree_chop",
  "blocks_path": true,
  "has_canopy": true,
  "canopy_tile": "oak_canopy"
}
```

---

## `Chest`

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | `"wooden_chest"` | `data/chests/` |
| `initial_state` | Enum | `Closed` | `Closed`, `Open`, `Locked`, `Looted` |
| `locked` | Bool | `false` | |
| `key_id` | String | `""` | |
| `loot_table` | String | **obbligatorio** | |
| `destroyable` | Bool | `false` | |
| `hp_override` | Int | `0` | Se distruttibile |
| `one_time` | Bool | `true` | Loot una sola volta |
| `trap_id` | String | `""` | Trappola opzionale |

---

## `TriggerZone`

Area invisibile per eventi.

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `trigger_id` | String | **obbligatorio** | ID univoco evento |
| `width` | Int | `64` | px |
| `height` | Int | `64` | px |
| `one_shot` | Bool | `true` | Si attiva una volta |
| `required_flag` | String | `""` | Flag story necessario |
| `set_flag` | String | `""` | Flag da impostare all'attivazione |
| `event_type` | Enum | `Script` | `Script`, `Ambush`, `Cutscene`, `Quest` |
| `event_payload` | String | `""` | ID script/dialogo/quest |

---

## `PatrolPath` (opzionale)

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `path_id` | String | **obbligatorio** | Riferito da `EnemySpawn` |
| `waypoints` | String | `""` | Coordinate serializzate o usa array di entity figlie |
| `loop` | Bool | `true` | |
| `speed` | Float | `40` | px/sec |

> Alternativa: usa entity `PatrolPoint` multiple con campo `path_id` e `order`.

---

## `LightSource` (opzionale, Fase 2+)

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `template_id` | String | `"torch"` | |
| `radius` | Int | `96` | px |
| `flicker` | Bool | `true` | |
| `color` | String | `"#ffaa44"` | |

---

## Tile custom fields (su Tileset LDtk)

Per tile cliff, ponti, scale — metadata sul tile, non entity.

| Campo | Tipo | Applicato a | Descrizione |
|-------|------|-------------|-------------|
| `foot_y` | Int | alberi, oggetti | Punto sorting |
| `sort_offset` | Int | canopy | Offset Y sort |
| `occlusion_line_y` | Int | cliff | Riga oltre cui occlude |
| `anim_id` | String | acqua, cascata | Chiave in `tile_anims.json` |
| `collision_override` | Enum | ponti | Override collisione cella |
| `terrain_fx` | Int | terreno speciale | Valore TerrainFX grid |

---

## Livello — campi custom

| Campo | Tipo | Default | Descrizione |
|-------|------|---------|-------------|
| `tile_size` | Int | `32` | Validazione |
| `music_id` | String | `""` | BGM al caricamento |
| `ambient_id` | String | `""` | Suoni ambiente |
| `default_elevation` | Int | `0` | Elevazione di default |
| `is_interior` | Bool | `false` | Mappa interno (no ciclo giorno?) |
| `parent_level_iid` | String | `""` | Per interior collegati |

---

## Progetto — Field Definitions riusabili

In LDtk → Project Settings → Definitions, crea:

```
Enums:
  - Facing (4 valori)
  - Disposition (5 valori)
  - InitialState (6 valori)
  - TransitionType (4 valori)
  - DamageType (9 valori)
  - EventType (4 valori)

Fields:
  - template_id: String
  - facing: Facing
  - sort_offset: Int
  - foot_y: Int
  - initial_state: InitialState
  - locked: Bool
  - key_id: String
  - loot_table: String
  - target_level_iid: String
  - target_x: Int
  - target_y: Int
  - transition_type: TransitionType
  - hp_override: Int
  - min_damage_threshold: Int
  - disposition: Disposition
  - aggro_range: Int
  - killable: Bool
  - interactable: Bool
  - has_shop: Bool
  - shop_id: String
  - has_quest: Bool
  - quest_ids: String
```

Poi assegna i field rilevanti a ogni Entity Definition.

---

## Entity Definitions LDtk — lista

Crea in LDtk queste definizioni:

| Definition | Colore editor | Campi principali |
|------------|---------------|------------------|
| `PlayerStart` | Verde | facing, spawn_id, player_profile |
| `EnemySpawn` | Rosso | template_id, level_override, patrol_path_id |
| `NPC` | Blu | template_id, dialogue_id, has_shop, has_quest |
| `Animal` | Giallo | template_id, disposition, loot_table |
| `Door` | Marrone | template_id, locked, key_id, target_level_iid |
| `Trapdoor` | Marrone scuro | target_level_iid, locked, key_id |
| `Portal` | Viola | target_level_iid, transition_type |
| `Destructible` | Arancione | template_id, min_damage_threshold |
| `ResourceNode` | Verde scuro | template_id, required_tool_tag |
| `Chest` | Oro | loot_table, locked, key_id |
| `TriggerZone` | Trasparente | trigger_id, event_type, event_payload |
| `PatrolPath` | Grigio | path_id, loop |

---

## Factory C++ — dispatch

```cpp
entt::entity LdtkEntityFactory::create(const LdtkEntity& e) {
    static const std::unordered_map<std::string, Creator> creators = {
        {"PlayerStart",   &LdtkEntityFactory::create_player_start},
        {"EnemySpawn",    &LdtkEntityFactory::create_enemy},
        {"NPC",           &LdtkEntityFactory::create_npc},
        {"Animal",        &LdtkEntityFactory::create_animal},
        {"Door",          &LdtkEntityFactory::create_door},
        {"Trapdoor",      &LdtkEntityFactory::create_trapdoor},
        {"Portal",        &LdtkEntityFactory::create_portal},
        {"Destructible",  &LdtkEntityFactory::create_destructible},
        {"ResourceNode",  &LdtkEntityFactory::create_resource},
        {"Chest",         &LdtkEntityFactory::create_chest},
        {"TriggerZone",   &LdtkEntityFactory::create_trigger},
    };

    auto it = creators.find(e.identifier);
    if (it == creators.end()) {
        LOG_WARN("Unknown entity: {}", e.identifier);
        return entt::null;
    }
    return (this->*it->second)(e);
}
```

---

## Riferimenti

- [ldtk-data-architecture.md](ldtk-data-architecture.md) — cosa va in JSON vs ECS
- [ldtk-phase1-integration.md](ldtk-phase1-integration.md) — solo PlayerStart in Fase 1
- [ldtk-phase2-complete.md](ldtk-phase2-complete.md) — tutte le entity in Fase 2
