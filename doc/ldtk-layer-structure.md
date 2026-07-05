# Struttura Layer LDtk per RPG 2.5D (Tiny Swords + C++/SFML/EnTT)

> Guida alla struttura dei layer per mappa LDtk, tileset animati e meccaniche di profondità/occlusione.

---

## Principio generale

**LDtk = dati della mappa** (cosa c'è e dove).  
**Engine C++/SFML/EnTT = rendering, animazioni, sorting, collisioni dinamiche.**

La regola d'oro: separa **layer visivi**, **layer logici** (IntGrid) e **entity** (spawn/interazione).

---

## Struttura consigliata dei layer (dall'alto verso il basso)

Ordine di disegno consigliato nel renderer (bottom → top):

### 1. `Water_Base` (Tiles)
- Acqua profonda, tile statici.
- Solo zone navigabili/non navigabili a livello visivo.

### 2. `Water_Edge` (Tiles, animati in engine)
- Schiuma/onde ai bordi dell'acqua.
- In LDtk metti i tile "base"; in engine animi i frame da `water_edge.png` (o simile).

### 3. `Ground_L0` (Tiles)
- Terreno del livello 0 (isole basse, prato).

### 4. `Cliffs_L0_to_L1` (Tiles)
- Facciate rocciose verticali tra livello 0 e 1.
- Sono **pareti**, non pavimento.

### 5. `Shadows` (Tiles o Entities)
- Ombre proiettate da cliff/case.
- Meglio come tile se sono fisse; come entity se devono muoversi (giorno/notte).

### 6. `Ground_L1` (Tiles)
- Piano superiore (villaggio principale).

### 7. `Cliffs_L1_to_L2` (Tiles)
- Transizione verso il castello/alto piano.

### 8. `Ground_L2` (Tiles)
- Piano più alto (castello, torri).

### 9. `Ramps_Stairs` (Tiles + IntGrid)
- Rampe/scalinata per salire/discesa.
- Tile visivi + valore IntGrid per "transition zone".

### 10. `Collisions` (IntGrid) — fondamentale

Valori tipici:

| Valore | Significato |
|--------|-------------|
| `0` | walkable |
| `1` | blocked |
| `2` | water (non camminabile) |
| `3` | ramp up (L0→L1) |
| `4` | ramp down |
| `5` | trigger/interaction zone |
| `6` | building interior footprint |

### 11. `Decor_Floor` (Tiles)
- Cespugli bassi, sassi piccoli, erba che il player **cammina sopra** (non occlude).

### 12. `Buildings` (Tiles o Entities)
- Case, torri, castello.
- Se sono fisse: tile layer va bene.
- Se hanno HP/stato/interazione: meglio **Entities**.

### 13. `Resources` (Entities)
- Alberi da tagliare, rocce da minare, chest, ecc.
- Con campi custom: `type`, `hp`, `respawn`, `loot_id`.

### 14. `Enemies` (Entities)
- Spawn point + AI data (`patrol_path`, `aggro_range`, `faction`).

### 15. `Player_Spawn` (Entities)
- Un'entity `PlayerStart` con `facing`, `level`, `checkpoint_id`.

### 16. `Decor_Canopy` (Tiles opzionale)
- Parti "alte" che possono coprire il player (chiome alberi, tetto sporgente).
- Utile se vuoi occlusione parziale senza entity complesse.

---

## Sistema altezze (2.5D): come modellarlo

Per ogni cella/tile usa **due dimensioni**:

1. **Floor level** (`z` o `elevation`): 0, 1, 2…
2. **Sort key** per profondità: di solito `world_y` (o `tile_y * tile_h + offset`)

### In LDtk

- Campo custom sul livello: `default_elevation = 0`
- IntGrid `Elevation` separato da `Collisions` (più pulito)
- Entity `Ramp` con campi:
  - `from_level = 0`
  - `to_level = 1`
  - `direction = N/E/S/W`

### In engine

Quando il player entra in ramp:

```cpp
player.elevation = target_level;
```

e aggiorni collisioni/pathfinding sul piano corrente.

---

## Player davanti/dietro montagne e oggetti

**Questa meccanica va nel game engine, non in LDtk.**

LDtk può solo fornire metadata utili (es. `sort_anchor_y`, `blocks_occlusion = true`), ma il sorting runtime è tuo.

### Approccio consigliato (Y-sort + elevation)

Per ogni oggetto renderizzabile:

```cpp
struct Renderable {
    sf::Vector2f position;   // piedi del personaggio/oggetto
    int elevation;
    int sort_offset;         // da LDtk custom field
};

int sort_key(const Renderable& r) {
    return r.elevation * LARGE + int(r.position.y) + r.sort_offset;
}
```

Poi ordini tutto per `sort_key` prima del draw.

### Regole pratiche

- **Player vs albero/casa**: confronta `position.y` dei "piedi".
- **Player vs cliff di livello superiore**: se `player.elevation < cliff.elevation` e il player è "sotto" la proiezione del muro, il muro lo copre.
- **Player su rampa**: transizione graduale di `elevation` + sorting continuo su Y.

### Cosa mettere in LDtk per aiutare

Su entity/tileset custom fields:

- `foot_y` (offset verticale punto di sorting)
- `occluder_height`
- `render_layer` (`floor`, `object`, `canopy`)
- `is_y_sortable = true`

Così il designer posiziona bene in editor, ma la logica resta nel codice.

---

## Tiles animati (ground, bush, acqua)

LDtk non anima: salva **tile ID statico** (frame 0), l'engine cicla i frame.

Pattern consigliato:

- Tileset con naming coerente (`bush1_0`, `bush1_1`…)
- Tabella animazioni in C++:

```cpp
struct TileAnim {
    std::vector<int> frames;
    float fps;
};
```

- Componente `AnimatedTile` su celle speciali (acqua, cespugli, fuochi).

---

## Entity vs Tile: guida rapida

| Elemento | Consiglio |
|---|---|
| Acqua/terreno/cliff | Tile layers |
| Collisioni/elevazione | IntGrid |
| Player/Nemici | Entities |
| Risorse interagibili | Entities |
| Cespugli puramente decorativi | Tiles |
| Cespugli con loot/nascosto | Entities |
| Case statiche | Tiles + collision IntGrid |
| Case distruttibili/apribili | Entities o ibrido |

---

## Architettura EnTT consigliata

**Componenti:**

- `Transform { x, y, elevation }`
- `SpriteRenderable { texture, frame, sort_offset }`
- `Collider { width, height, blocked }`
- `Occluder { min_y, max_y, elevation }`
- `Interactable`, `ResourceNode`, `EnemyTag`, `PlayerTag`

**Sistemi:**

1. `MovementSystem` (legge `Collisions` + `Elevation`)
2. `ElevationTransitionSystem` (rampe)
3. `AnimationSystem` (tile/frame)
4. `DepthSortSystem` (costruisce draw list ordinata)
5. `RenderSystem` (SFML draw)

---

## Schema architetturale

```
┌─────────────────────────────────────────────────────────┐
│                    LDtk (dati)                          │
│  ┌─────────────┐  ┌──────────────────┐  ┌────────────┐ │
│  │ Tiles visivi│  │ IntGrid collisioni│  │  Entities  │ │
│  └──────┬──────┘  └────────┬─────────┘  └─────┬──────┘ │
└─────────┼──────────────────┼────────────────────┼────────┘
          │                  │                    │
          ▼                  ▼                    ▼
┌─────────────────────────────────────────────────────────┐
│              Engine C++/SFML/EnTT                       │
│                                                         │
│  LdtkLoader → Mappa tile + grid → Animazioni tile      │
│                              ↘                          │
│                        Depth sort (Y + elevation)       │
│                              ↘                          │
│                           Render SFML                   │
└─────────────────────────────────────────────────────────┘
```

---

## Errori comuni da evitare

1. Mettere player/nemici come tile layer → difficile muoverli e sortarli.
2. Un solo layer "oggetti" per tutto → sorting ingestibile.
3. Collisioni solo visive → serve IntGrid dedicata.
4. Altezza solo visiva senza dato logico → il player "sale" senza effetto gameplay.
5. Tentare occlusion dinamica in LDtk → non è fatta per quello.

---

## Setup minimo per iniziare subito

Se vuoi partire lean:

1. `Water`
2. `Terrain` (tutti i ground+cliff inizialmente)
3. `Collisions` (IntGrid)
4. `Elevation` (IntGrid)
5. `Decor`
6. `Entities` (Player, Enemy, Resource, Building)

Poi, quando cresce la mappa, separi `Ground_L0/L1/L2`, `Shadows`, `Water_Edge`.

---

## Prossimi passi

- Template LDtk (nomi layer + enum IntGrid pronti)
- Scheletro C++ (`LdtkLoader` + `DepthSortSystem`) compatibile con SFML/EnTT
