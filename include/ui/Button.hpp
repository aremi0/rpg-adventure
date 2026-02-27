#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>

enum class ButtonState {
    kIdle,
    kHover,
    kPressed
};

class Button {
    public:
        Button(float x, float y, float width, float height,
        const sf::Font& font, const std::string& text, unsigned int character_size,
        sf::Color idle_color, sf::Color hover_color, sf::Color active_color);

        ~Button() = default;

        void Update(const sf::Vector2f& mouse_pos); // Aggiorna lo stato del bottone in base alla posizione del mouse
        void Render(sf::RenderTarget& target); // Disegna il bottone a schermo
        bool IsPressed() const; //Ritorna true nel momento esatto in cui il bottone viene cliccato e rilasciato

    private:
        ButtonState button_state_;
        sf::RectangleShape shape_;
        sf::Text text_;

        sf::Color idle_color_;
        sf::Color hover_color_;
        sf::Color active_color_;

        bool is_pressed_; // Flag per evitare click multipli tenendo premuto
};