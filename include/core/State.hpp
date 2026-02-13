#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Classe base astratta (interfaccia) per gli stati del gioco
 * 
 * Questa classe definisce l'interfaccia comune per tutti gli stati del gioco.
 * Ogni stato deve implementare i metodi init(), handleInput(), update() e draw().
 */

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