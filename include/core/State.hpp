#pragma once
#include <SFML/Graphics.hpp>
#include <string_view>

/**
 * @brief Classe base astratta (interfaccia) per gli stati del gioco
 * 
 * Questa classe definisce l'interfaccia comune per tutti gli stati del gioco.
 * Ogni stato deve implementare i metodi init(), handleInput(), update() e draw().
 */

class State {
public:
    virtual ~State() = default;

    virtual void Init() = 0;
    virtual void HandleInput() = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    // Utile per mettere in pausa uno stato senza distruggerlo
    virtual void Pause() {}
    virtual void Resume() {}

    std::string_view GetStateName() const { return state_name_; }

protected:
    explicit State(std::string_view state_name) : state_name_(state_name) {}

private:
    std::string state_name_;
};