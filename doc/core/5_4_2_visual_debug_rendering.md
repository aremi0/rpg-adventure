## Integrazione: Visual Debug Rendering e Architettura ECS

Per velocizzare lo sviluppo e monitorare visivamente lo stato delle entità gestite dal nostro sistema ECS (Entity Component System), abbiamo introdotto un pannello di debug visivo e dinamico. Questa funzionalità permette di visualizzare bounding box (hitbox), ID e valori critici (posizione, elevazione, velocità) in tempo reale, per ciascuna entità a schermo.

### 1. Costanti di Configurazione e Tasti Rapidi

Per mantenere il codice configurabile ed evitare hardcoding, le impostazioni del debug visivo sono state centralizzate nel namespace `Debug` insieme al classico logger testuale. Ciò consente a chi sviluppa di accendere o spegnere permanentemente la feature senza toccare l'engine di rendering.

**`include/core/Constants.hpp`**
```cpp
    namespace Debug {
        // Permette di attivare l'overlay visivo di debug in gioco
        inline constexpr bool kEnableVisualDebug = true;
        inline constexpr sf::Keyboard::Key kVisualDebugKey = sf::Keyboard::F1;
        // ...
    }
```

### 2. Creazione del DebugRenderSystem

Abbiamo costruito un nuovo sistema puramente dedicato alla visualizzazione dei dati tecnici delle entità. Questo metodo interroga l'ECS cercando iterativamente tutte le entità dotate di `TransformComponent`.

**`src/systems/DebugRenderSystem.cpp`** (Funzionamento)
- **Bounding Box Dinamica**: Il sistema riadatta le dimensioni del rettangolo nel caso in cui rilevi anche uno `SpriteComponent` sull'entità, altrimenti applica una dimensione spaziale prefissata.
- **Color-Coding**: Offre una distinzione visiva istantanea: le entità marcate con il tag `PlayerComponent` possiedono contorni **Verdi**, tutte le altre **Rossi**.
- **Informazioni in Realtà Aumentata**: Sfruttando `std::format` (da C++20), proiettiamo su un testo flottante le informazioni fondamentali come le coordinate logiche e l'asse Z 2.5D (`elevation`). In aggiunta, se l'entità possiede un `VelocityComponent`, il motore includerà dinamicamente queste informazioni sul testo.

### 3. Aggancio al Loop del GameState

Analogamente al logger dinamico, la cattura della pressione del tasto globale viene passata nel polling event loop.
