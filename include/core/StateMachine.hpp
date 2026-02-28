#pragma once
#include <memory>
#include <vector>
#include "State.hpp"

class StateMachine {
    public:
        void AddState(std::unique_ptr<State> new_state, bool is_replacing = true);
        void RemoveState();
        void ProcessStateChanges();

        std::unique_ptr<State>& GetActiveState();
        const std::vector<std::unique_ptr<State>>& GetStates() const;

        void Draw(); // Disegna tutti gli State nello stack

    private:
        std::vector<std::unique_ptr<State>> states_;
        std::unique_ptr<State> new_state_;

        bool is_removing_ = false;
        bool is_adding_ = false;
        bool is_replacing_ = false;
};