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
    /*
        virtual: chiama il distruttore della classe derivata (che implementa State)
        ~State(): distruttore virtuale
        = default: implementazione di default
    */
    virtual ~State() = default;         // "Ehi C++, pulisci sempre la classe derivata corretta!"

    virtual void init() = 0;            // Il "= 0" rende la classe una interfaccia: "Chi mi eredita DEVE implementare questo"
    virtual void handleInput() = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // Utile per mettere in pausa uno stato senza distruggerlo
    virtual void pause() {}         // il "{}" rende opzionale l'implementazione, se non lo scrivi non succede nulla
    virtual void resume() {}
};