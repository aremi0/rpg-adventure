## Integrazione: Logging Dinamico e Refactoring

Per permettere al giocatore o allo sviluppatore di cambiare il livello di log in tempo reale tramite la tastiera, abbiamo introdotto un sistema di switch dinamico. Inoltre, abbiamo ristrutturato i file per evitare inclusioni circolari tra le classi di utilità e le costanti.

### 1. Separazione dell'`enum class LogLevel`

Inizialmente, `LogLevel` era definito all'interno di `Logger.hpp`. Tuttavia, per rendere il livello di log di default configurabile dentro `Constants.hpp`, avevamo bisogno che quest'ultimo file conoscesse `LogLevel`, creando un conflitto di inclusione circolare.
La soluzione adottata è stata estrarre l'enumeratore in un header indipendente:

**`include/utils/LogLevel.hpp`**

### 2. Spostamento Costanti in `Constants.hpp`

Con l'enum separato, ora le costanti di gioco possono definire nativamente i parametri di debug:

**`include/core/Constants.hpp`**
```cpp
    // ...
    namespace Debug {
        // Permette di ciclare i log nel gioco usando un tasto
        inline constexpr bool kEnableLogSwitching = true;
        inline constexpr sf::Keyboard::Key kLogSwitchKey = sf::Keyboard::F1;
        inline constexpr LogLevel kDefaultLogLevel = LogLevel::Debug;
    }
}
```

### 3. Gestione Input nel `GameState`

Infine, aggiungiamo la reazione all'evento di sistema direttamente nel polling loop per scatenare il cambio del livello predefinito del Logger.

**`src/states/GameState.cpp`**
```cpp
        // Tasto per ciclare il livello di log
        if (Config::Debug::kEnableLogSwitching && event.type == sf::Event::KeyPressed && event.key.code == Config::Debug::kLogSwitchKey) {
            Logger::CycleLevel();
        }
```
