## Fase 4.0: Entity Component System (ECS)

In questa fase, il motore di gioco ha abbandonato la classica ereditarietà Object-Oriented (OOP) per abbracciare il **Data-Oriented Design** tramite il pattern **Entity Component System (ECS)**, utilizzando la libreria ad alte prestazioni `EnTT`.

### 4.1: Setup del Registry (`Game.hpp`)

Integrazione del database centrale dell'ECS all'interno della struttura condivisa `GameData`.

* **Database Globale:** L'oggetto `entt::registry` è stato istanziato come core data manager. Questo permette a tutti gli stati (Menu, Partita, Pausa) e a tutti i Sistemi di accedere in modo centralizzato alle entità e ai loro componenti, garantendo una singola fonte di verità (Single Source of Truth).

### 4.2: Componenti Base e Approccio 2.5D (`components.hpp`)

Definizione dei mattoncini fondamentali (Componenti) come strutture dati pure (POD - Plain Old Data), prive di logica.

* **TransformComponent:** Gestisce la posizione spaziale (`sf::Vector2f position`), la scala e la rotazione. È stata introdotta la variabile `elevation` per gestire l'asse Z logico, permettendo al motore di supportare meccaniche 2.5D (salti, alture) pur renderizzando in 2D.
* **SpriteComponent:** Incapsula le informazioni visive (`sf::Sprite`) e introduce uno `z_index` per stabilire l'ordine di profondità visiva.

### 4.3: Il Render System e Z-Sorting (`RenderSystem.cpp`)

Creazione del primo Sistema ECS, completamente svincolato dai dati.

* **Query Ottimizzate:** Il sistema utilizza `registry.view<TransformComponent, SpriteComponent>()` per iterare in modo ultra-veloce su array contigui in memoria (SoA - Structure of Arrays), processando solo le entità renderizzabili e ignorando entità puramente logiche (es. trigger invisibili).
* **Z-Sorting Dinamico:** Prima del rendering, il sistema riordina i componenti in memoria tramite `registry.sort<SpriteComponent>()` basandosi sullo `z_index`. Questo risolve i problemi di sovrapposizione visiva garantendo che il background venga disegnato prima del personaggio.
* **Proiezione 2.5D:** Il calcolo visivo sottrae l'`elevation` alla coordinata `Y` logica, applicando istantaneamente l'effetto di altezza/salto sullo schermo senza alterare le coordinate logiche di base usate per le collisioni.

### 4.4: Game State, Ciclo di Vita e Memory Management (`GameState.cpp`)

Implementazione dello stato di gioco reale e risoluzione di bug architetturali critici legati alla gestione della memoria tra cambi di stato.

* **Spawn dell'Entità:** Creazione del primo ID Entità (`hero_`) tramite `registry.create()` e assegnazione dei componenti tramite `registry.emplace<T>()`.
* **Idempotenza dell'AssetManager:** Refactoring del sistema di caricamento risorse. Se un asset (es. font o texture) viene richiesto ma è già in RAM, l'`AssetManager` ora restituisce un successo silenzioso anziché un errore. Questo previene crash (Segmentation Fault) quando gli stati vengono distrutti e ricreati (es. tornando dal Gioco al Menu).
* **Prevenzione Entità Immortali (Memory Leak):** Introdotto il distruttore `~GameState()` che chiama esplicitamente `registry.clear()`. Questo garantisce che, all'uscita dalla partita, l'intera memoria allocata per i cloni, nemici o eroi venga rilasciata, assicurando che ogni "Nuova Partita" inizi con un database pulito e senza duplicati fantasma a schermo.

---