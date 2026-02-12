#include <SFML/Graphics.hpp>
#include <print>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RPG Adventure");

    std::print("Engine avviato correttamente con C++23!\n");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Red);
        window.display();
    }

    return 0;
}
