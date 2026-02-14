#include "core/StateMachine.hpp"
#include "utils/Logger.hpp"

void StateMachine::AddState(std::unique_ptr<State> new_state, bool is_replacing) {
    is_adding_ = true;
    is_replacing_ = is_replacing;
    new_state_ = std::move(new_state);
    Logger::Debug("Richiesta aggiunta nuovo stato ({}) (Replacing: {})", new_state_->GetStateName(), is_replacing);
}

void StateMachine::RemoveState() {
    is_removing_ = true;
    Logger::Debug("Richiesta rimozione stato corrente ({})", states_.top()->GetStateName());
}

void StateMachine::ProcessStateChanges() {  
    // 1. Gestione rimozione
    if (is_removing_ && !states_.empty()) {
        states_.pop();
        Logger::Info("Stato rimosso ({})", states_.top()->GetStateName());
        if (!states_.empty()) {
            states_.top()->Resume();
            Logger::Info("Stato precedente ripristinato (resume) ({})", states_.top()->GetStateName());
        }
        is_removing_ = false;
    }

    // 2. Gestione aggiunta
    if (is_adding_) {
        if (!states_.empty()) {
            if (is_replacing_) {
                states_.pop();
                Logger::Info("Stato precedente rimosso (sostituzione) ({})", states_.top()->GetStateName());
            } else {
                states_.top()->Pause();
                Logger::Info("Stato precedente messo in pausa ({})", states_.top()->GetStateName());
            }
        }

        states_.push(std::move(new_state_));
        states_.top()->Init();
        Logger::Info("Nuovo stato inizializzato e pushato sullo stack ({})", states_.top()->GetStateName());
        is_adding_ = false;
    }
}

std::unique_ptr<State>& StateMachine::GetActiveState() {
    // TODO: in un contesto reale, qui andrebbe un controllo se lo stack è vuoto
    return states_.top();
}