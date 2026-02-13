#include "core/Game.hpp"

int main() {
    // Avvia il gioco. In un'app reale, potresti leggere 
    // risoluzione e titolo da un file di configurazione.
    Game game(800, 600, "RPG-ADVENTURE by Aremi"); // L'oggetto Game viene creato sullo Stack, il suo costruttore fa partire il metodo run()
    
    return 0;
}
