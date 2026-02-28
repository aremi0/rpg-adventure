### 3.5.1: Classe UI Button (`Button.hpp` / `Button.cpp`)

Creazione di un componente di interfaccia utente riutilizzabile e incapsulato.

* **Gestione Stati Visivi:** Il bottone gestisce autonomamente i suoi tre stati visivi (`Idle`, `Hover`, `Pressed`) in base alla posizione del mouse e al click.
* **Centratura Dinamica del Testo:** Implementata una logica all'interno del metodo `SetText()` che utilizza `getLocalBounds()` e `setOrigin()` per mantenere il testo sempre perfettamente centrato rispetto al rettangolo del bottone, indipendentemente dalla lunghezza della stringa.
* **Edge Detection sui Click:** Per risolvere il problema del "mouse mitragliatrice" (attivazioni multiple dovute all'alto framerate), è stata introdotta una variabile di stato (`last_mouse_state_`). Il click viene registrato come valido **solo** nel frame esatto in cui il bottone del mouse passa da "non premuto" a "premuto".

### 3.5.2: Gestione Audio (`AssetManager.hpp`)

Estensione dell'`AssetManager` per supportare il caricamento e la riproduzione dell'audio tramite SFML, differenziando la gestione della memoria in base al tipo di file.

* **SFX (SoundBuffer):** Gli effetti sonori brevi vengono caricati interamente in RAM. Essendo oggetti copiabili, sono stati integrati perfettamente nel metodo template esistente `LoadAsset<T>()`.
* **BGM (Music):** La musica di sottofondo (es. file MP3/OGG pesanti) non viene caricata in RAM ma letta in streaming (`openFromFile`). Poiché `sf::Music` non è copiabile, è stata creata una logica dedicata (`LoadMusic` e `GetMusic`) che gestisce queste risorse in modo sicuro tramite `std::unique_ptr`.
* **Error Handling:** Utilizzo di `std::expected` per una gestione moderna e type-safe degli errori di caricamento.

### 3.5.3: Settings State e Architettura Overlay

Implementazione del menu delle opzioni e refactoring profondo della `StateMachine` per supportare il rendering in sovraimpressione.

* **Stack to Vector Refactoring:** Il contenitore interno della `StateMachine` è stato convertito da `std::stack` a `std::vector`. Questo permette di iterare sugli stati dal basso verso l'alto durante il `Draw()`, rendendo possibile il rendering degli stati sottostanti (es. vedere il Main Menu sfocato sotto le Impostazioni).
* **Separazione Logica/Grafica:** Per mantenere vive le animazioni in background senza processare gli input degli stati in pausa, l'interfaccia `State` è stata divisa in `Update(dt)` (eseguito solo per lo stato in cima/attivo) e `UpdateVisuals(dt)` (eseguito per tutti gli stati nello stack).
* **Logical vs Physical Resolution:** Utilizzo di `sf::View` per definire una telecamera fissa a risoluzione logica (es. 800x600). Questo garantisce che gli elementi UI e l'overlay semitrasparente scalino correttamente in automatico quando l'utente cambia la risoluzione fisica della finestra.
* **Global Volume:** Integrazione con `sf::Listener` per la gestione centralizzata del volume dell'intero applicativo.

### 3.5.4: Assemblaggio Main Menu

Unione di tutti i sistemi creati per formare il menu principale interattivo.

* **Inizializzazione Centralizzata:** Il `MainMenuState` gestisce il primo caricamento delle risorse globali (Font, Musica di base) delegando il compito all'`AssetManager`.
* **Data-Driven Configuration:** Utilizzo estensivo di un file `Constants.hpp` strutturato in namespace, che impiega `inline constexpr` e `std::string_view` per definire percorsi, stringhe costanti e metriche a costo zero di allocazione memoria.
* **Format String Sicure:** Utilizzo di `std::format` (C++20/23) per generare dinamicamente e in sicurezza i path delle texture (es. per l'Animated Background) combinando i prefissi costanti con gli indici numerici.
* **Gestione Transizioni:** Corretto cablaggio dei bottoni per cambiare stato passando flag specifici (`is_replacing = true/false`) per definire se la transizione deve distruggere lo stato precedente o posizionarsi in overlay.