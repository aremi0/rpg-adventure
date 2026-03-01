- [1.1 Delta Time (`dt`) Fissa vs Variabile](#11-delta-time-dt-fissa-vs-variabile)
- [1.2 Disaccoppiamento: Logica vs Animazione](#12-disaccoppiamento-logica-vs-animazione)
- [1.3 L'Accumulatore di Tempo (Il trucco del resto)](#13-laccumulatore-di-tempo-il-trucco-del-resto)

## 1. Il Loop di Gioco e la Gestione del Tempo

La corretta gestione del tempo è il cuore pulsante di qualsiasi motore di gioco. Separare la velocità con cui il computer esegue i calcoli dalla velocità con cui gli eventi accadono nel mondo di gioco è fondamentale per garantire un'esperienza stabile su hardware diversi.

### 1.1 Delta Time (`dt`) Fissa vs Variabile

```cpp
static constexpr float kDeltaTime = 1.0f / 60.0f; // ~0.016s

```

Il **Delta Time (`dt`)** rappresenta il tempo trascorso tra un aggiornamento logico e l'altro.
Moltiplicare le grandezze matematiche per `dt` (es. `posizione += velocità * dt`) garantisce che il gioco si muova alla stessa velocità su computer differenti, rendendolo *frame-rate independent*.

Nel nostro progetto utilizziamo un **Time Step Fisso** (1/60 di secondo).

* **Perché:** Garantisce che la logica di gioco, la fisica e le collisioni siano deterministiche e stabili. Anche se il frame rate visivo cala (es. il PC scende a 30 FPS visivi), la logica "recupererà" eseguendo più aggiornamenti da 0.016s consecutivi, senza mai "sfondare" i muri per colpa di un `dt` troppo grande.

### 1.2 Disaccoppiamento: Logica vs Animazione

Un errore comune nell'architettura è confondere il tempo logico generale (`dt`) con i tempi specifici di rendering (come la durata dei frame di un'animazione).

* **`dt` (Il "Passo"):** L'intervallo fisso con cui l'intero ecosistema di gioco si aggiorna. È il battito cardiaco globale.
* **`frame_duration` (La "Soglia"):** Quanto tempo uno specifico stato o frame di animazione deve perdurare prima di cambiare (es. 0.1s per ottenere 10 FPS).

Questa separazione (Decoupling) permette di avere un motore fluido a 60 FPS in cui coesistono decine di entità a velocità visive differenti. Ad esempio, un giocatore animato a 12 FPS e uno sfondo animato a 5 FPS consumeranno entrambi lo stesso `dt` globale, ma valuteranno indipendentemente quando è il momento di aggiornare la propria grafica.

### 1.3 L'Accumulatore di Tempo (Il trucco del resto)

Per far comunicare il `dt` con la soglia dell'animazione, si utilizza un **accumulatore di tempo** (o timer). Ad ogni iterazione del Game Loop, l'accumulatore somma a sé stesso un `dt` finché non raggiunge o supera la soglia necessaria (`frame_duration`).

```cpp
// 1. Accumulo il tempo trascorso (Il "bicchiere" si riempie)
animation_timer_ += dt; 

// 2. Controllo se ho superato la soglia
if (animation_timer_ >= frame_duration_) {
    
    // SCORRETTO: animation_timer_ = 0.0f; 
    // Azzardando a zero si perdono i millisecondi in eccesso, causando 
    // rallentamenti invisibili che si sommano (stuttering).
    
    // CORRETTO: Conserviamo il resto
    animation_timer_ -= frame_duration_; 
    
    // ... logica di avanzamento del frame ...
}

```

**Il Trucco del Resto:** Sottraendo la soglia (`frame_duration_`) invece di azzerare forzatamente il timer, conserviamo i millisecondi extra per il calcolo del frame successivo. Questo previene la perdita di sincronia e garantisce che l'animazione mantenga i suoi FPS teorici perfetti nel lungo termine.

---