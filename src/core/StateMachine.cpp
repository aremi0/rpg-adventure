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
    Logger::Debug("Richiesta rimozione stato corrente ({})", states_.back()->GetStateName());
}

void StateMachine::ProcessStateChanges() {  
    // 1. Gestione rimozione
    if (is_removing_ && !states_.empty()) {
        Logger::Info("Stato in rimozione ({})", states_.back()->GetStateName());
        states_.pop_back();
        if (!states_.empty()) {
            states_.back()->Resume();
            Logger::Info("Stato precedente ripristinato (resume) ({})", states_.back()->GetStateName());
        }
        is_removing_ = false;
    }

    // 2. Gestione aggiunta
    if (is_adding_) {
        if (!states_.empty()) {
            if (is_replacing_) {
                Logger::Info("Stato precedente in rimozione (sostituzione) ({})", states_.back()->GetStateName());
                states_.pop_back();
            } else {
                states_.back()->Pause();
                Logger::Info("Stato precedente messo in pausa ({})", states_.back()->GetStateName());
            }
        }

        states_.push_back(std::move(new_state_));
        states_.back()->Init();
        Logger::Info("Nuovo stato inizializzato e pushato sullo stack ({})", states_.back()->GetStateName());
        is_adding_ = false;
    }
}

std::unique_ptr<State>& StateMachine::GetActiveState() {
    // TODO: in un contesto reale, qui andrebbe un controllo se lo stack è vuoto
    return states_.back();
}

// Renderizza gli stati dal basso verso l'alto
void StateMachine::Draw() {
    for (auto& state : states_) {
        state->Draw();
    }
}
