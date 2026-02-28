#include "ui/Button.hpp"
#include <SFML/System/Vector2.hpp>

Button::Button(float x, float y, float width, float height,
               const sf::Font& font, const std::string& text, unsigned int character_size,
               sf::Color idle_color, sf::Color hover_color, sf::Color active_color)
    : button_state_(ButtonState::Idle),
      idle_color_(idle_color),
      hover_color_(hover_color),
      active_color_(active_color),
      is_pressed_(false) {
    
    // Configura il rettangolo di sfondo
    shape_.setPosition(sf::Vector2f(x, y));
    shape_.setSize(sf::Vector2f(width, height));
    shape_.setFillColor(idle_color_);

    // Configura il testo
    text_.setFont(font);
    text_.setCharacterSize(character_size);
    text_.setFillColor(sf::Color::White); // Testo bianco di default
    SetText(text); // Imposta il testo del bottone e lo allineo al suo centro
}

void Button::Update(const sf::Vector2f& mouse_pos) {
    // 1. Reset stato a Idle
    button_state_ = ButtonState::Idle;
    is_pressed_ = false;

    // 2. Controllo Hover (il mouse è sopra il bottone?)
    if (shape_.getGlobalBounds().contains(mouse_pos)) {
        button_state_ = ButtonState::Hover;

        // 3. Controllo Click (il tasto sinistro è premuto?)
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            button_state_ = ButtonState::Pressed;
            is_pressed_ = true;
        }
    }

    // 4. Aggiorna il colore in base allo stato
    switch (button_state_) {
        case ButtonState::Idle:
            shape_.setFillColor(idle_color_);
            break;
        case ButtonState::Hover:
            shape_.setFillColor(hover_color_);
            break;
        case ButtonState::Pressed:
            shape_.setFillColor(active_color_);
            break;
    }
}

void Button::Render(sf::RenderTarget& target) {
    target.draw(shape_);
    target.draw(text_);
}

bool Button::IsPressed() const {
    return is_pressed_;
}

void Button::SetText(const std::string& text) {
    text_.setString(text);
    
    sf::FloatRect text_bounds = text_.getLocalBounds();
    text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                    text_bounds.top  + text_bounds.height / 2.0f);
    
    text_.setPosition(
        shape_.getPosition().x + (shape_.getSize().x / 2.0f),
        shape_.getPosition().y + (shape_.getSize().y / 2.0f)
    );
}