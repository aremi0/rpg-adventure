## Fase 2: Core Engine & State Machine

In un RPG, passerai continuamente da uno stato all'altro (es. dal Menu al Mondo, dal Mondo al Combattimento). Non vogliamo che il `main` sappia nulla di tutto ciò.

Useremo lo **State Pattern**. L'idea è: l'Engine possiede uno "stack" di stati. Solo lo stato in cima allo stack viene aggiornato e renderizzato.

### 2.1 L'interfaccia `State`

Crea il file `include/core/State.hpp`. Questa è la classe base astratta.

```cpp
#pragma once
#include <SFML/Graphics.hpp>

class State {
public:
    virtual ~State() = default;

    virtual void init() = 0;
    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // Utile per mettere in pausa uno stato senza distruggerlo
    virtual void pause() {}
    virtual void resume() {}
};

```

### 2.2 Lo `StateMachine`

Crea `include/core/StateMachine.hpp`. Questa classe gestisce l'aggiunta e la rimozione degli stati.

```cpp
#pragma once
#include <memory>
#include <stack>
#include "State.hpp"

class StateMachine {
public:
    void addState(std::unique_ptr<State> newState, bool isReplacing = true);
    void removeState();
    void processStateChanges();

    std::unique_ptr<State>& getActiveState();

private:
    std::stack<std::unique_ptr<State>> _states;
    std::unique_ptr<State> _newState;

    bool _isRemoving = false;
    bool _isAdding = false;
    bool _isReplacing = false;
};

```

*Nota: La logica di "sostituzione" o "aggiunta" (es. aprire l'inventario sopra il gioco) viene gestita qui.*

### 2.3 La Classe `Game` (L'Engine)

Questa classe conterrà il `sf::RenderWindow` e lo `StateMachine`. In `include/core/Game.hpp`:

```cpp
#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include <string>

struct GameData {
    sf::RenderWindow window;
    StateMachine machine;
    // Qui aggiungeremo in futuro AssetManager e InputManager
};

using GameDataRef = std::shared_ptr<GameData>;

class Game {
public:
    Game(int width, int height, std::string title);

private:
    const float dt = 1.0f / 60.0f; // Clock fisso a 60 FPS per la logica
    sf::Clock _clock;
    GameDataRef _data = std::make_shared<GameData>();

    void run();
};

```

### Perché questa struttura?

1. **GameDataRef:** Usiamo uno `shared_ptr` a una struct di dati globali. Questo permette di passare finestra, stati e risorse a ogni nuovo stato in modo semplice e sicuro.
2. **Encapsulation:** Il `main` dovrà solo fare `Game game(800, 600, "My RPG");`.

---
---

### 2.4. Implementazione di `StateMachine.cpp`

Questo file gestisce la logica di transizione. Invece di modificare lo stack immediatamente, usiamo dei "flag" (`_isAdding`, `_isRemoving`) per processare i cambiamenti all'inizio di ogni frame.

```cpp
#include "core/StateMachine.hpp"

void StateMachine::addState(std::unique_ptr<State> newState, bool isReplacing) {
    _isAdding = true;
    _isReplacing = isReplacing;
    _newState = std::move(newState);
}

void StateMachine::removeState() {
    _isRemoving = true;
}

void StateMachine::processStateChanges() {
    // 1. Gestione rimozione
    if (_isRemoving && !_states.empty()) {
        _states.pop();
        if (!_states.empty()) {
            _states.top()->resume();
        }
        _isRemoving = false;
    }

    // 2. Gestione aggiunta
    if (_isAdding) {
        if (!_states.empty()) {
            if (_isReplacing) {
                _states.pop();
            } else {
                _states.top()->pause();
            }
        }

        _states.push(std::move(_newState));
        _states.top()->init();
        _isAdding = false;
    }
}

std::unique_ptr<State>& StateMachine::getActiveState() {
    return _states.top();
}

```

---

### 2.5 Implementazione di `Game.cpp`

Qui implementiamo il **Fixed Timestep**. È una tecnica vitale nei giochi: la logica (update) gira a una velocità costante (es. 60 volte al secondo), mentre il rendering gira il più velocemente possibile. Questo previene bug fisici se il framerate oscilla.

```cpp
#include "core/Game.hpp"
#include <print>

Game::Game(int width, int height, std::string title) {
    _data->window.create(sf::VideoMode({(unsigned int)width, (unsigned int)height}), title);
    
    // Qui in futuro caricheremo lo stato iniziale (es. MenuState)
    // _data->machine.addState(std::make_unique<MainMenuState>(_data));

    this->run();
}

void Game::run() {
    float newTime, frameTime, interpolation;
    float currentTime = _clock.getElapsedTime().asSeconds();
    float accumulator = 0.0f;

    while (_data->window.isOpen()) {
        // Gestione cambi di stato
        _data->machine.processStateChanges();

        newTime = _clock.getElapsedTime().asSeconds();
        frameTime = newTime - currentTime;

        if (frameTime > 0.25f) frameTime = 0.25f; // Limite per evitare "salti" enormi dopo un lag

        currentTime = newTime;
        accumulator += frameTime;

        // Loop di Update (Fixed Timestep)
        while (accumulator >= dt) {
            _data->machine.getActiveState()->handleInput();
            _data->machine.getActiveState()->update(dt);

            accumulator -= dt;
        }

        // Render
        _data->window.clear();
        _data->machine.getActiveState()->draw();
        _data->window.display();
    }
}

```

---

### 2.6 Aggiornamento del `main.cpp`

Ora il tuo punto di ingresso diventa pulitissimo:

```cpp
#include "core/Game.hpp"

int main() {
    // Avvia il gioco. In un'app reale, potresti leggere 
    // risoluzione e titolo da un file di configurazione.
    Game(800, 600, "C++23 RPG Engine");
    
    return 0;
}

```

---