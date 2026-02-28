#pragma once
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <string>
#include <SFML/Audio.hpp>

enum class ButtonState {
    Idle,
    Hover,
    Pressed
};

class Button {
    public:
        Button(float x, float y, float width, float height,
        const sf::Font& font, const std::string& text, unsigned int character_size,
        sf::Color idle_color, sf::Color hover_color, sf::Color active_color,
        const sf::SoundBuffer* hover_sfx = nullptr,
        const sf::SoundBuffer* click_sfx = nullptr,
        float volume = 100.f);

        ~Button() = default;

        void Update(const sf::Vector2f& mouse_pos); // Aggiorna lo stato del bottone in base alla posizione del mouse
        void Render(sf::RenderTarget& target); // Disegna il bottone a schermo
        bool IsPressed() const; // Ritorna true nel momento esatto in cui il bottone viene cliccato e rilasciato
        void SetText(const std::string& text); // Imposta il testo del bottone
        void SetVolume(float volume); // Imposta il volume dei suoni hover/click
        void SetColors(sf::Color idle, sf::Color hover, sf::Color active); // Cambia i colori del bottone
        void Reset(); // Resetta lo stato interno del bottone (utile quando viene disabilitato)

    private:
        ButtonState button_state_;
        sf::RectangleShape shape_;
        sf::Text text_;

        sf::Color idle_color_;
        sf::Color hover_color_;
        sf::Color active_color_;

        // Flags per evitare click multipli tenendo premuto
        bool is_pressed_;
        bool last_mouse_pressed_;

        sf::Sound hover_sound_;
        sf::Sound click_sound_;
};