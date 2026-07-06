# LDtk Map Design — Indice Documentazione

> **Tileset:** Fantasy RPG by Franuka v1.9.7 (16/32/48 px)  
> **Engine:** C++23 · SFML 2.6.1 · EnTT · nlohmann/json  
> **Genere:** RPG fantasy 2.5D, meccaniche stile D&D / *Warriors of the Eternal Sun*

**Guida sviluppo agenti:** [Agent Development Guide](../agent_development_guide.md) — contesto unificato per tutte le fasi.

---

## Documenti

| File | Contenuto |
|------|-----------|
| [ldtk-phase1-integration.md](ldtk-phase1-integration.md) | Fase 1 — setup minimo per Fase 7 della roadmap |
| [ldtk-phase2-complete.md](ldtk-phase2-complete.md) | Fase 2 — layering completo Franuka |
| [ldtk-data-architecture.md](ldtk-data-architecture.md) | Cosa va in LDtk vs Engine vs JSON |
| [ldtk-entity-schema.md](ldtk-entity-schema.md) | Custom fields standardizzati per ogni entity |

---

## Principio generale

```
LDtk        → layout spaziale, spawn, stato iniziale, metadata di posizionamento
Engine/ECS  → runtime, sorting, animazioni, combattimento, AI, party, stati
JSON        → cataloghi (nemici, oggetti, equip, terreno, loot tables, animazioni sprite)
```

**Regola d'oro:** LDtk descrive *dove* e *con quale configurazione iniziale*; il motore possiede *come* evolve nel tempo.

---

## Due fasi di sviluppo LDtk

### Fase 1 — Integrazione rapida (Roadmap Fase 7)

Obiettivo: caricare una mappa, muovere il player (WASD), collisioni base, camera, elevazione fluida su rampe.

```
Layer visivi:   Terrain → Decor → Buildings
IntGrid:        Collisions, Elevation
Entities:       PlayerStart
```

Vedi [ldtk-phase1-integration.md](ldtk-phase1-integration.md).

### Fase 2 — Mappa completa (post Fase 7)

Obiettivo: tutte le meccaniche RPG, oggetti interattivi, terreno avanzato, occlusione.

```
Layer visivi:   Water → Ground → Paths → Cliffs → Stairs → Bridges
                → Decor_Floor → Buildings → Decor_Canopy → Waterfall
IntGrid:        Collisions, Elevation, TerrainFX (opzionale)
Entities:       tutti i tipi definiti nello schema
```

Vedi [ldtk-phase2-complete.md](ldtk-phase2-complete.md).

### Espandibilità Fase 1 → Fase 2

| Fase 1 | Fase 2 — si aggiunge |
|--------|----------------------|
| `Terrain` | si splitta in `Water`, `Ground`, `Paths`, `Cliffs`, `Stairs`, `Bridges` |
| `Decor` | si splitta in `Decor_Floor`, `Decor_Canopy`, `Waterfall` |
| `Collisions` | resta; si aggiunge `TerrainFX` |
| `Elevation` | resta invariato |
| `PlayerStart` | si aggiungono altre entity |

Il loader C++ deve ignorare layer sconosciuti (forward-compatible).

---

## Dimensione tile

**Raccomandazione: 32×32 px (2x Franuka).** Vedi tabella dettagliata in [ldtk-phase1-integration.md](ldtk-phase1-integration.md).

---

## Elevazione dual-layer (decisione engine)

| Livello | Tipo | Dove |
|---------|------|------|
| Piano logico | `int` (`floor_level`) | IntGrid `Elevation` + ECS |
| Altezza visiva | `float` (`height`) | ECS — interpolata su rampe |
| Posizione | `float` (x, y) | `TransformComponent` |

Su celle `Stairs`: IntGrid `Elevation` = piano **destinazione**; `height` interpola a runtime. Dettaglio in [Agent Guide](../agent_development_guide.md) e [ldtk-phase1-integration.md](ldtk-phase1-integration.md).

---

## Oggetti interattivi: Entity con tile layer vuoto

Gli oggetti che cambiano stato (porte, alberi, chest) sono **Entity**; il tile layer sotto resta vuoto. Edifici puramente decorativi → tile layer `Buildings`.

---

## Ordine di rendering (2.5D)

```cpp
sort_key = static_cast<int>(elev.height * kSortScale)
         + static_cast<int>(foot_y)
         + sort_offset;
```

**Implementazione:** `DepthSortSystem` (Fase 7.3), non LDtk.

---

## Mapping roadmap engine

| Roadmap | Documento |
|---------|-----------|
| 7.1–7.5 | [phase1](ldtk-phase1-integration.md) + [Agent Guide](../agent_development_guide.md) |
| LDtk phase2 | [phase2](ldtk-phase2-complete.md) |
| 8.1 Animazioni sprite | [data-architecture](ldtk-data-architecture.md) § Sprite Animation |
| 8.3 Party / RPG | [data-architecture](ldtk-data-architecture.md) § Party + [entity-schema](ldtk-entity-schema.md) |
| 8.2 Pathfinding | Dopo Fase 7; grid `Collisions` |

---

## Errori comuni

1. Mettere stats RPG in LDtk → usare JSON + ECS.
2. Tile fissi per oggetti che cambiano stato → usare Entity.
3. Un solo IntGrid per tutto → separa `Collisions` e `Elevation`.
4. `ramp_up` / `ramp_down` separati → usa `Stairs` + `Elevation` grid.
5. Party/compagni spawnati da LDtk al load → **no**; reclutamento runtime (Fase 8.3).
6. Punta-e-clicca in Fase 7 → **no**; solo WASD fino a Fase 8.2.
