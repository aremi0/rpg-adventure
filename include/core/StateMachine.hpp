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