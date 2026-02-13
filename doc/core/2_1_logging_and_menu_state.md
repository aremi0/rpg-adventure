## Punto Intermedio: Sistema di Logging

Sfruttiamo **C++23** e la libreria `<print>`, che è molto più efficiente dei vecchi `std::cout` e supporta nativamente la formattazione stile Python/Rust.

### 1. L'Header `include/utils/Logger.hpp`

Useremo una classe con metodi statici per poter loggare ovunque senza dover passare istanze in giro.

```cpp
#pragma once
#include <string_view>
#include <print>
#include <chrono>
#include <format>

enum class LogLevel {
    Trace, Debug, Info, Warning, Error
};

class Logger {
public:
    // Disabilitiamo l'istanziazione
    Logger() = delete;

    template<typename... Args>
    static void log(LogLevel level, std::format_string<Args...> fmt, Args&&... args) {
        std::string_view levelStr;
        std::string_view color;

        switch (level) {
            case LogLevel::Trace:   levelStr = "[TRACE]";   color = "\033[90m"; break; // Grigio
            case LogLevel::Debug:   levelStr = "[DEBUG]";   color = "\033[36m"; break; // Ciano
            case LogLevel::Info:    levelStr = "[INFO] ";   color = "\033[32m"; break; // Verde
            case LogLevel::Warning: levelStr = "[WARN] ";   color = "\033[33m"; break; // Giallo
            case LogLevel::Error:   levelStr = "[ERROR]";   color = "\033[31m"; break; // Rosso
        }

        auto now = std::chrono::system_clock::now();
        
        // Stampa: [HH:MM:SS] [LEVEL] Messaggio
        std::print("{}{:%T} {} ", color, now, levelStr);
        std::print(fmt, std::forward<Args>(args)...);
        std::print("\033[0m\n"); // Reset colore
    }

    // Scorciatoie comode
    template<typename... Args> static void trace(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Trace, fmt, std::forward<Args>(args)...); }
    template<typename... Args> static void debug(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Debug, fmt, std::forward<Args>(args)...); }
    template<typename... Args> static void info(std::format_string<Args...> fmt, Args&&... args)  { log(LogLevel::Info, fmt, std::forward<Args>(args)...); }
    template<typename... Args> static void warn(std::format_string<Args...> fmt, Args&&... args)  { log(LogLevel::Warning, fmt, std::forward<Args>(args)...); }
    template<typename... Args> static void error(std::format_string<Args...> fmt, Args&&... args) { log(LogLevel::Error, fmt, std::forward<Args>(args)...); }
};

```

> **Perché questo approccio?** Usando i variadic templates e `std::format_string`, il compilatore può controllare la correttezza dei parametri di log a tempo di compilazione. Inoltre, i codici ANSI (es. `\033[31m`) colorano l'output del terminale, rendendo gli errori immediatamente visibili.

---

## Creazione dello Stato di Test: `MainMenuState`

Ora integriamo il Logger e la State Machine creando il nostro primo stato reale.

### 2. `include/states/MainMenuState.hpp`

```cpp
#pragma once
#include "core/State.hpp"
#include "core/Game.hpp"

class MainMenuState : public State {
public:
    MainMenuState(GameDataRef data);

    void init() override;
    void handleInput() override;
    void update(float dt) override;
    void draw() override;

private:
    GameDataRef _data;
    sf::RectangleShape _testButton; // Solo per vedere qualcosa a schermo
};

```

### 3. `src/states/MainMenuState.cpp`

```cpp
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

MainMenuState::MainMenuState(GameDataRef data) : _data(data) {}

void MainMenuState::init() {
    Logger::info("Inizializzazione MainMenuState...");
    
    _testButton.setSize({200.0f, 50.0f});
    _testButton.setFillColor(sf::Color::Red);
    _testButton.setPosition({300.0f, 275.0f}); // Al centro circa
}

void MainMenuState::handleInput() {
    sf::Event event;
    while (_data->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _data->window.close();
        }
        
        // Esempio: premi un tasto per loggare qualcosa
        if (event.type == sf::Event::KeyPressed) {
            Logger::debug("Tasto premuto: {}", (int)event.key.code);
        }
    }
}

void MainMenuState::update(float dt) {
    // Logica del menu (es. animazioni)
}

void MainMenuState::draw() {
    _data->window.draw(_testButton);
}

```

---

### Ultimo passaggio: Avviare lo stato in `Game.cpp`

Ora dobbiamo effettivamente dire al gioco di partire con il menu. Nel costruttore di `Game.cpp`, aggiungi l'inclusione e il comando:

```cpp
#include "states/MainMenuState.hpp"
#include "utils/Logger.hpp"

// ... nel costruttore Game::Game ...
_data->machine.addState(std::make_unique<MainMenuState>(_data));
Logger::info("Stato iniziale caricato: Main Menu");

```

**Cosa dovresti vedere ora:**

1. Una finestra con un rettangolo rosso al centro.
2. Nel terminale, dei log colorati che ti dicono esattamente cosa sta succedendo (avvio engine, caricamento stato).
3. Premendo i tasti, vedrai i codici nel terminale via `Logger::debug`.

**Una volta che hai compilato e verificato che il logger e il rettangolo rosso funzionano, siamo pronti per la Fase 3: Resource Management. Dammi l'OK!**