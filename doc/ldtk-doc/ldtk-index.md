# LDtk Map Design — Indice Documentazione

> **Tileset:** Fantasy RPG by Franuka v1.9.7 (16/32/48 px)  
> **Engine:** C++23 · SFML 2.6.1 · EnTT · nlohmann/json  
> **Genere:** RPG fantasy 2.5D, meccaniche stile D&D / Warriors of the Eternal Sun

---

## Documenti

| File | Contenuto |
|------|-----------|
| [docs/ldtk-phase1-integration.md](docs/ldtk-phase1-integration.md) | Fase 1 — setup minimo per Fase 7 della roadmap |
| [docs/ldtk-phase2-complete.md](docs/ldtk-phase2-complete.md) | Fase 2 — layering completo Franuka |
| [docs/ldtk-data-architecture.md](docs/ldtk-data-architecture.md) | Cosa va in LDtk vs Engine vs JSON |
| [docs/ldtk-entity-schema.md](docs/ldtk-entity-schema.md) | Custom fields standardizzati per ogni entity |

---

## Principio generale

```
LDtk        → layout spaziale, spawn, stato iniziale, metadata di posizionamento
Engine/ECS  → runtime, sorting, animazioni, combattimento, AI, stati
JSON        → cataloghi (nemici, oggetti, equip, terreno, loot tables)
```

**Regola d'oro:** LDtk descrive *dove* e *con quale configurazione iniziale*; il motore possiede *come* evolve nel tempo.

---

## Due fasi di sviluppo

### Fase 1 — Integrazione rapida (Fase 7 roadmap)

Obiettivo: caricare una mappa, muovere il player, collisioni base, camera, elevazione semplice.

```
Layer visivi:   Terrain → Decor → Buildings
IntGrid:        Collisions, Elevation
Entities:       PlayerStart
```

Vedi [docs/ldtk-phase1-integration.md](docs/ldtk-phase1-integration.md).

### Fase 2 — Mappa completa

Obiettivo: tutte le meccaniche RPG, oggetti interattivi, terreno avanzato, occlusione.

```
Layer visivi:   Water → Ground → Paths → Cliffs → Stairs → Bridges
                → Decor_Floor → Buildings → Decor_Canopy → Waterfall
IntGrid:        Collisions, Elevation, TerrainFX (opzionale)
Entities:       tutti i tipi definiti nello schema
```

Vedi [docs/ldtk-phase2-complete.md](docs/ldtk-phase2-complete.md).

### Espandibilità Fase 1 → Fase 2

La Fase 1 usa **gli stessi nomi di layer** della Fase 2 dove possibile. I layer aggiuntivi della Fase 2 si inseriscono senza rinominare quelli esistenti:

| Fase 1 | Fase 2 — si aggiunge |
|--------|----------------------|
| `Terrain` | si splitta in `Water`, `Ground`, `Paths`, `Cliffs`, `Stairs`, `Bridges` |
| `Decor` | si splitta in `Decor_Floor`, `Decor_Canopy`, `Waterfall` |
| `Collisions` | resta; si aggiunge `TerrainFX` |
| `Elevation` | resta invariato |
| `PlayerStart` | si aggiungono altre entity |

Il loader C++ deve ignorare layer sconosciuti (forward-compatible).

---

## Dimensione tile per schermi moderni

| Risoluzione | Tile consigliato | Zoom camera | Area visibile (~) |
|-------------|------------------|-------------|-------------------|
| 1080p Full HD | **32×32 px (2x)** | 1.0× – 1.5× | 60×34 – 40×23 tile |
| 1440p | **32×32 px (2x)** | 1.5× | 60×34 tile |
| 2160p 2K/4K | **32×32 px (2x)** | 2.0× – 3.0× | 60×34 – 40×23 tile |

**Raccomandazione: usa il pack a 32×32 (2x).**

Motivazioni:
- 16 px su Full HD richiede zoom 2×–3× per essere leggibile → complica UI e pixel-perfect scaling
- 48 px mostra poca mappa → più scrolling, meno overview da RPG
- 32 px è lo sweet spot: dettaglio Franuka, area visibile adeguata, integer scaling pulito

```cpp
// Esempio: integer scaling per pixel-perfect
const int TILE_SIZE = 32;
const int BASE_ZOOM = 2; // su 1080p → 32*2 = 64px effettivi per tile
view.setSize(windowWidth / BASE_ZOOM, windowHeight / BASE_ZOOM);
```

---

## Oggetti interattivi: Entity con tile layer vuoto

**Sì — gli oggetti interattivi vanno come Entity.** Il tile layer sottostante resta **vuoto** (o ha solo il pavimento base).

```
┌─────────────────────────────────────┐
│  Tile layer Buildings:  [vuoto]     │  ← nessun tile fisso sulla cella
│  Entity Door:           [sprite]    │  ← l'engine disegna e aggiorna lo stato
│  IntGrid Collisions:    [blocked]   │  ← collisione gestita dalla grid o entity
└─────────────────────────────────────┘
```

**Perché:**
- Porta aperta/chiusa → cambia sprite e collisione a runtime
- Albero abbattuto → rimuovi entity, cambi IntGrid a walkable, spawni loot
- Muro distrutto → aggiorni HP, sprite, collisione senza toccare LDtk

**Eccezione:** edifici **puramente decorativi** e mai distruttibili → possono restare tile layer.

---

## Player / Nemici: custom fields in LDtk?

| Dato | Dove |
|------|------|
| Posizione spawn, facing | LDtk entity |
| `template_id` (riferimento archetipo) | LDtk entity |
| Override puntuali (es. `level = 5`) | LDtk entity (opzionale) |
| STR, DEX, CON, INT, WIS, CHA | JSON archetipo + componente ECS |
| HP corrente, XP, livello runtime | ECS (mai LDtk) |
| Equipaggiamento | JSON + ECS `InventoryComponent` |
| AI state, target, cooldown | ECS |

LDtk fornisce solo lo **spawn recipe**:

```json
// In LDtk — entity EnemySpawn
{ "template_id": "goblin", "level_override": 3, "patrol_path": "path_01" }
```

```json
// In data/enemies/goblin.json — caricato dall'engine
{ "base_hp": 12, "str": 8, "dex": 14, "loot_table": "goblin_common", ... }
```

---

## Terreno, status, equipaggiamento

| Sistema | Fonte dati |
|---------|------------|
| Elevazione per cella | LDtk IntGrid `Elevation` |
| Walkable/blocked/acqua | LDtk IntGrid `Collisions` |
| Slow %, moltiplicatori elementali | IntGrid `TerrainFX` → JSON `terrain_types.json` |
| Status runtime (sanguinamento, bagnato…) | ECS `StatusEffectComponent` |
| Armi, armature, requisiti, raffinamento | JSON `data/items/` |
| Danno minimo per strutture | LDtk entity field + JSON archetipo |

Dettaglio completo in [docs/ldtk-data-architecture.md](docs/ldtk-data-architecture.md).

---

## Ordine di rendering (2.5D)

```cpp
sort_key = elevation * 10'000 + static_cast<int>(foot_y) + sort_offset;
```

1. Ordina tutti i renderable per `sort_key`
2. Disegna in ordine crescente
3. `Decor_Canopy` e parti `canopy` degli alberi usano lo stesso sort ma con `sort_offset` maggiore

**Implementazione:** `DepthSortSystem` (Fase 7.3), non LDtk.

---

## Mapping roadmap engine

| Roadmap | Documento di riferimento |
|---------|--------------------------|
| 7.1 MapManager & LDtk Loader | phase1 + data-architecture |
| 7.2 VertexArray rendering | phase1 (layer semplici) |
| 7.3 2.5D & Elevation | phase1 IntGrid + sort_key |
| 7.4 Box Collider | phase1 Collisions grid |
| 7.5 Camera & Debug | phase1 |
| 8.1 Animazioni | phase2 (Waterfall, tile animati) |
| 8.3 RPG Core | entity-schema + data-architecture |

---

## Errori comuni

1. Mettere stats RPG in LDtk → difficile bilanciare, no hot-reload
2. Tile fissi per oggetti che cambiano stato → porta chiusa non può aprirsi
3. Un solo IntGrid per tutto → separa `Collisions` e `Elevation`
4. `ramp_up` / `ramp_down` separati → usa `stairs` + `Elevation` grid
5. Dimenticare di lasciare vuoto il tile layer sotto le entity interattive
