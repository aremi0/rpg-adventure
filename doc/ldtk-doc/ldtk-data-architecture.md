# Architettura Dati — LDtk vs Engine vs JSON

> Dove vive ogni tipo di dato nel progetto C++23 / SFML / EnTT / nlohmann/json.

---

## Panoramica a tre livelli

```
┌──────────────────────────────────────────────────────────────────┐
│                         LDtk (.ldtk)                             │
│  Posizione, layout, stato INIZIALE, spawn, riferimenti template  │
└────────────────────────────┬─────────────────────────────────────┘
                             │ load at level start
                             ▼
┌──────────────────────────────────────────────────────────────────┐
│                    JSON Cataloghi (data/)                        │
│  Archetipi: nemici, NPC, oggetti, terreno, loot, dialoghi, shop  │
└────────────────────────────┬─────────────────────────────────────┘
                             │ istanzia in ECS
                             ▼
┌──────────────────────────────────────────────────────────────────┐
│                         ECS Runtime                              │
│  HP corrente, XP, status, inventario, AI state, animazioni       │
└──────────────────────────────────────────────────────────────────┘
```

### Regola decisionale

| Domanda | Risposta |
|---------|----------|
| È legato alla **posizione sulla mappa**? | LDtk |
| È un **archetipo** riusabile in più mappe? | JSON |
| **Cambia durante il gameplay**? | ECS |
| È **bilanciamento** da iterare spesso? | JSON (mai LDtk) |

---

## Cosa va in LDtk

### Tile layers
- Grafica statica (terreno, decorazioni, edifici non interattivi)
- Frame 0 dei tile animati

### IntGrid layers
- `Collisions` — walkable, blocked, water, stairs
- `Elevation` — livello per cella
- `TerrainFX` — ID tipo terreno (→ JSON)

### Entity fields
- Posizione (`px`, `py`)
- Tipo entity (`identifier`)
- `template_id` — puntatore a JSON archetipo
- Stato iniziale (porta chiusa, forziere non aperto, nemico livello 3)
- Override puntuali rispetto al template
- Riferimenti mappa (`target_level_iid`, `patrol_path_id`)

### Cosa NON va in LDtk
- Stats complete del player
- Tabelle loot dettagliate
- Definizioni armi/armature
- Formule di danno
- Dialoghi completi (solo `dialogue_id`)
- HP corrente a runtime

---

## Cosa va in JSON

### Struttura cartelle consigliata

```
data/
  enemies/           # Archetipi nemici
    goblin.json
    troll.json
  npcs/              # Archetipi NPC
    blacksmith.json
    elder.json
  items/
    weapons/
    armor/
    consumables/
    keys/
  destructibles/     # Muri, barili, cancelli
    brick_wall.json
    wooden_barrel.json
  resources/         # Alberi, miniere
    oak_tree.json
    iron_ore.json
  terrain/
    terrain_types.json
  loot_tables/
    goblin_common.json
    chest_rare.json
  shops/
    blacksmith_shop.json
  dialogues/
    elder_intro.json
  quests/
    find_the_key.json
  tile_anims/
    water.json
    waterfall.json
  factions/
    factions.json
```

---

## Cosa va in ECS (runtime)

| Componente | Contenuto |
|------------|-----------|
| `TransformComponent` | posizione, rotazione, foot_offset |
| `ElevationComponent` | livello corrente |
| `VelocityComponent` | velocità |
| `SpriteComponent` | texture, rect, sort_offset |
| `BoxColliderComponent` | hitbox |
| `StatsComponent` | STR, DEX, CON, INT, WIS, CHA, HP, MP |
| `CombatComponent` | attacco, difesa, resistenze |
| `StatusEffectComponent` | lista status attivi con durata/stack |
| `InventoryComponent` | oggetti equipaggiati e borsa |
| `AIComponent` | stato AI, target, patrol index |
| `InteractableComponent` | può essere interagito, raggio |
| `DestructibleComponent` | HP corrente, soglia danno, alive |
| `DoorComponent` | open, locked, key_id |
| `LootableComponent` | già lootato, loot_table_id |
| `QuestGiverComponent` | quest disponibili |
| `ShopComponent` | shop_id, inventario shop |
| `PlayerComponent` | tag player |
| `EnemyTag` | tag nemico |
| `NPCTag` | tag NPC |

---

## Player e Nemici — divisione responsabilità

### LDtk (spawn only)

```json
// Entity PlayerStart
{ "facing": "South", "spawn_id": "village_inn" }

// Entity EnemySpawn
{
  "template_id": "goblin",
  "level_override": 0,
  "facing": "West",
  "patrol_path_id": "goblin_patrol_01",
  "aggro_range_override": 0
}
```

`level_override = 0` significa "usa il livello del template".

### JSON archetipo nemico

```json
// data/enemies/goblin.json
{
  "id": "goblin",
  "display_name": "Goblin",
  "sprite": "goblin",
  "level": 2,
  "stats": {
    "str": 10, "dex": 12, "con": 8,
    "int": 6,  "wis": 8,  "cha": 6,
    "hp": 14,  "mp": 0
  },
  "combat": {
    "attack": 4,
    "defense": 2,
    "damage_dice": "1d6",
    "damage_bonus": 1,
    "resistances": { "poison": 0.5 },
    "weaknesses": { "fire": 1.5 }
  },
  "ai": {
    "disposition": "hostile",
    "aggro_range": 120,
    "flee_threshold": 0.2,
    "patrol_speed": 40
  },
  "loot_table": "goblin_common",
  "xp_reward": 25
}
```

### ECS al momento dello spawn

```cpp
entt::entity spawn_enemy(const LdtkEntity& spawn, DataRegistry& data) {
    const auto& archetype = data.enemy(spawn.field("template_id"));
    const int level = spawn.field_int("level_override", 0);
    const int effective_level = level > 0 ? level : archetype.level;

    auto e = registry.create();
    registry.emplace<TransformComponent>(e, spawn.px, spawn.py);
    registry.emplace<StatsComponent>(e, scale_stats(archetype.stats, effective_level));
    registry.emplace<CombatComponent>(e, archetype.combat);
    registry.emplace<AIComponent>(e, archetype.ai);
  // ...
    return e;
}
```

### Player stats

Il player **non** ha stats in LDtk. Caricamento:

1. **Nuova partita:** `data/player/default.json` o hardcoded in `PlayerFactory`
2. **Caricamento save:** file save JSON con stats correnti
3. **PlayerStart** in LDtk fornisce solo posizione e facing

```json
// data/player/default.json
{
  "stats": {
    "str": 14, "dex": 12, "con": 14,
    "int": 10, "wis": 10, "cha": 10,
    "hp": 20, "mp": 5
  },
  "level": 1,
  "xp": 0
}
```

---

## Terreno — elevazione, rallentamento, elementi

### LDtk IntGrid `Elevation`
Valore intero per cella. Logica pura di piano.

### LDtk IntGrid `TerrainFX`
Valore intero → chiave in `terrain_types.json`.

### JSON `terrain_types.json`

```json
{
  "normal": {
    "movement_speed_mult": 1.0,
    "elemental_mult": {},
    "on_enter_status": [],
    "on_stay_status": [],
    "on_stay_tick_sec": 1.0
  },
  "wet": {
    "movement_speed_mult": 0.8,
    "elemental_mult": {
      "lightning": 1.5,
      "fire": 0.5
    },
    "on_enter_status": ["wet"],
    "on_enter_chance": 1.0,
    "on_stay_status": [],
    "tags": ["conductive"]
  },
  "mud": {
    "movement_speed_mult": 0.6,
    "elemental_mult": {},
    "on_enter_status": [],
    "on_stay_status": ["slow"],
    "on_stay_tick_sec": 0.5
  }
}
```

### ECS `StatusEffectComponent`

```cpp
struct StatusEffect {
    std::string id;           // "wet", "poison", "bleed", "burn"
    float duration_sec;       // -1 = permanente finché non curato
    int stacks = 1;
    float tick_timer = 0.f;
    float tick_interval = 1.f;
    nlohmann::json params;    // danno per tick, ecc.
};

struct StatusEffectComponent {
    std::vector<StatusEffect> effects;
};
```

### Status — definizioni in JSON

```json
// data/status/wet.json
{
  "id": "wet",
  "display_name": "Bagnato",
  "duration_sec": 30,
  "stackable": false,
  "modifiers": {
    "lightning_vulnerability": 1.25
  },
  "interactions": {
    "lightning_damage": { "bonus_mult": 1.5, "spread_aoe": true }
  },
  "on_expire": null
}

// data/status/bleed.json
{
  "id": "bleed",
  "display_name": "Sanguinamento",
  "duration_sec": 10,
  "stackable": true,
  "max_stacks": 5,
  "tick_damage": "1d4",
  "tick_interval_sec": 2.0
}
```

### `TerrainEffectSystem` (pseudocodice)

```cpp
void TerrainEffectSystem::update(entt::entity e, GridPos cell) {
    const int fx = map.terrain_fx_at(cell);
    const auto& terrain = data.terrain_type(fx);

    auto& vel = registry.get<VelocityComponent>(e);
    vel.speed_mult = terrain.movement_speed_mult;

    if (!terrain.on_stay_status.empty()) {
        timer += dt;
        if (timer >= terrain.on_stay_tick_sec) {
            try_apply_status(e, terrain.on_stay_status, terrain.on_enter_chance);
            timer = 0;
        }
    }
}
```

---

## Equipaggiamento — 100% JSON

```json
// data/items/weapons/iron_sword.json
{
  "id": "iron_sword",
  "display_name": "Spada di Ferro",
  "type": "weapon",
  "slot": "main_hand",
  "sprite": "iron_sword",
  "requirements": {
    "min_str": 10,
    "min_level": 1
  },
  "refinement": {
    "grade": 0,
    "max_grade": 5,
    "bonus_per_grade": { "attack": 1 }
  },
  "stats": {
    "attack": 5,
    "damage_dice": "1d8",
    "damage_bonus": 0,
    "crit_chance": 0.05
  },
  "elemental": {
    "type": null,
    "damage_bonus": 0
  },
  "on_hit_status": [],
  "tags": ["metal", "melee", "slashing"]
}

// data/items/armor/leather_armor.json
{
  "id": "leather_armor",
  "type": "armor",
  "slot": "chest",
  "requirements": { "min_dex": 8 },
  "refinement": { "grade": 0, "max_grade": 3, "bonus_per_grade": { "defense": 1 } },
  "stats": { "defense": 3, "evasion": 0.02 },
  "resistances": { "slashing": 0.9 },
  "tags": ["leather", "light"]
}
```

### Raffinamento — runtime in ECS

```cpp
struct ItemInstance {
    std::string item_id;
    int refinement_grade = 0;
    std::optional<std::string> enchant_id;
    int durability = -1;  // -1 = indistruttibile
};
```

---

## Strutture distruttibili — soglia danno minima

### LDtk entity `Destructible`

```
template_id: "brick_wall"
hp_override: 0          (0 = usa template)
initial_state: "intact"
```

### JSON archetipo

```json
// data/destructibles/brick_wall.json
{
  "id": "brick_wall",
  "sprite": "brick_wall",
  "hp": 30,
  "defense": 5,
  "min_damage_threshold": 10,
  "damage_types_allowed": ["bludgeoning", "slashing", "fire"],
  "loot_table": null,
  "on_destroy": {
    "collision": "walkable",
    "spawn_entity": null,
    "fx": "wall_crumble"
  }
}
```

### Logica danno

```cpp
bool DamageSystem::apply(Entity target, const DamageInfo& dmg) {
    auto& dest = registry.get<DestructibleComponent>(target);

    if (dmg.amount < dest.min_damage_threshold) {
        spawn_fx("damage_immune");  // nemmeno un graffio
        return false;
    }
    if (!dest.allows_damage_type(dmg.type)) {
        spawn_fx("damage_immune");
        return false;
    }

    const int actual = std::max(1, dmg.amount - dest.defense);
    dest.current_hp -= actual;
    if (dest.current_hp <= 0) destroy_entity(target);
    return true;
}
```

---

## Loot tables

```json
// data/loot_tables/goblin_common.json
{
  "id": "goblin_common",
  "rolls": 1,
  "entries": [
    { "item_id": "gold", "weight": 60, "quantity": "2d6" },
    { "item_id": "rusty_dagger", "weight": 15, "quantity": 1 },
    { "item_id": "health_potion", "weight": 10, "quantity": 1 },
    { "item_id": null, "weight": 15, "quantity": 0 }
  ]
}
```

---

## Flusso caricamento livello

```
1. MapManager::load("world.ldtk")
2. Per ogni entity nel livello:
   a. Leggi identifier + field_instances
   b. LdtkEntityFactory::create(identifier, fields)
   c. Factory carica template JSON se template_id presente
   d. Applica override da LDtk
   e. Crea entità ECS con componenti
3. Costruisci VertexArray per tile layer
4. Indicizza IntGrid per accesso O(1)
```

```cpp
class LdtkEntityFactory {
    DataRegistry& data_;
    entt::registry& registry_;

public:
    entt::entity create(const LdtkEntity& ldtk) {
        if (ldtk.identifier == "EnemySpawn")  return create_enemy(ldtk);
        if (ldtk.identifier == "NPC")         return create_npc(ldtk);
        if (ldtk.identifier == "Door")        return create_door(ldtk);
        // ...
        return entt::null;
    }
};
```

---

## Salvataggio — cosa persistere

Il save **non** riscrive LDtk. Salva solo delta dallo stato iniziale:

```json
{
  "level_id": "World_Level_0",
  "player": {
    "position": [512, 384],
    "elevation": 1,
    "stats": { "hp": 18, "xp": 350, "level": 3 },
    "inventory": [...]
  },
  "world_state": {
    "entities": {
      "ldtk_uid_42": { "destroyed": true },
      "ldtk_uid_87": { "door_open": true },
      "ldtk_uid_103": { "looted": true }
    },
    "enemies_killed": ["ldtk_uid_55", "ldtk_uid_56"]
  }
}
```

Usa `ldtk_uid` (UID entity LDtk) come chiave stabile.

---

## Riepilogo rapido

| Dato | LDtk | JSON | ECS |
|------|:----:|:----:|:---:|
| Posizione spawn | ✅ | | |
| Template ID | ✅ | ✅ | |
| Stats base nemico | | ✅ | |
| HP corrente | | | ✅ |
| XP, livello player | | | ✅ |
| Inventario | | | ✅ |
| Elevazione cella | ✅ | | |
| Slow terreno | ✅ (ID) | ✅ (def) | ✅ (mult) |
| Status wet/poison | | ✅ (def) | ✅ (istanza) |
| Armi/armature | | ✅ | ✅ (istanza) |
| Porta chiusa inizialmente | ✅ | | ✅ (stato) |
| Loot table | | ✅ | |
| Dialoghi | | ✅ | |
| AI state | | | ✅ |
