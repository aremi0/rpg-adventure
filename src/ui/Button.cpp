#include "ui/Button.hpp"
#include <SFML/System/Vector2.hpp>

Button::Button(float x, float y, float width, float height,
               const sf::Font& font, const std::string& text, unsigned int character_size,
               sf::Color idle_color, sf::Color hover_color, sf::Color active_color,
               const sf::SoundBuffer* hover_sfx, const sf::SoundBuffer* click_sfx,
               float volume)
    : button_state_(ButtonState::Idle),
      idle_color_(idle_color),
      hover_color_(hover_color),
      active_color_(active_color),
      is_pressed_(false),
      last_mouse_pressed_(false) {
    
    if (hover_sfx) hover_sound_.setBuffer(*hover_sfx);
    if (click_sfx) click_sound_.setBuffer(*click_sfx);
    SetVolume(volume);

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
    bool current_mouse_down = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    is_pressed_ = false; // Resettiamo il flag "Azione" ogni frame

    ButtonState prev_state = button_state_;

    if (shape_.getGlobalBounds().contains(mouse_pos)) {
        // Se il mouse è sopra, decidiamo se colorarlo di Hover o Pressed
        button_state_ = current_mouse_down ? ButtonState::Pressed : ButtonState::Hover;

        // Se il tasto sinistro è premuto e non era premuto l'ultima volta, consideriamo un click
        if (current_mouse_down && !last_mouse_pressed_) {
            is_pressed_ = true;
            click_sound_.play();
        }
    } else {
        button_state_ = ButtonState::Idle;
    }

    if (prev_state == ButtonState::Idle && button_state_ == ButtonState::Hover) {
        hover_sound_.play(); // ← suono hover
    }

    // Aggiorniamo i colori
    switch (button_state_) {
        case ButtonState::Idle:    shape_.setFillColor(idle_color_); break;
        case ButtonState::Hover:   shape_.setFillColor(hover_color_); break;
        case ButtonState::Pressed: shape_.setFillColor(active_color_); break;
    }

    last_mouse_pressed_ = current_mouse_down;
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

void Button::SetVolume(float volume) {
    hover_sound_.setVolume(volume);
    click_sound_.setVolume(volume);
}

void Button::SetColors(sf::Color idle, sf::Color hover, sf::Color active) {
    idle_color_ = idle;
    hover_color_ = hover;
    active_color_ = active;
    shape_.setFillColor(idle); // Applica subito il colore idle
}