#include "core/StateMachine.hpp"
#include "utils/Logger.hpp"

void StateMachine::addState(std::unique_ptr<State> newState, bool isReplacing) {
    _isAdding = true;
    _isReplacing = isReplacing;
    _newState = std::move(newState);
    Logger::debug("Richiesta aggiunta nuovo stato (Replacing: {})", isReplacing);
}

void StateMachine::removeState() {
    _isRemoving = true;
    Logger::debug("Richiesta rimozione stato corrente");
}

void StateMachine::processStateChanges() {
    // 1. Gestione rimozione
    if (_isRemoving && !_states.empty()) {
        _states.pop();
        Logger::info("Stato rimosso");
        if (!_states.empty()) {
            _states.top()->resume();
            Logger::info("Stato precedente ripristinato (resume)");
        }
        _isRemoving = false;
    }

    // 2. Gestione aggiunta
    if (_isAdding) {
        if (!_states.empty()) {
            if (_isReplacing) {
                _states.pop();
                Logger::info("Stato precedente rimosso (sostituzione)");
            } else {
                _states.top()->pause();
                Logger::info("Stato precedente messo in pausa");
            }
        }

        _states.push(std::move(_newState));
        _states.top()->init();
        Logger::info("Nuovo stato inizializzato e pushato sullo stack");
        _isAdding = false;
    }
}

std::unique_ptr<State>& StateMachine::getActiveState() {
    // TODO: in un contesto reale, qui andrebbe un controllo se lo stack è vuoto
    return _states.top();
}