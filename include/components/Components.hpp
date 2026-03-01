#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

// -----------------------------------------------------------------------------
// TransformComponent
// Determina DOVE si trova un'entità nel mondo, la sua grandezza, la rotazione
//      e la sua altezza dal suolo (per il rendering 2.5D).
// -----------------------------------------------------------------------------
struct TransformComponent {
    sf::Vector2f position{0.0f, 0.0f};  // Posizione logica sul terreno (X, Y)
    float elevation{0.0f};              // Asse Z: altezza dal suolo (salto/alture)
    sf::Vector2f scale{1.0f, 1.0f};
    float rotation{0.0f};
};

// -----------------------------------------------------------------------------
// SpriteComponent
// Determina COME appare un'entità a schermo.
// -----------------------------------------------------------------------------
struct SpriteComponent {
    sf::Sprite sprite;
    int z_index{0};                     // Ordine di disegno (chi sta davanti a chi)
};

// -----------------------------------------------------------------------------
// VelocityComponent
// Determina in che direzione e a che velocità si sta muovendo un'entità.
// -----------------------------------------------------------------------------
struct VelocityComponent {
    sf::Vector2f velocity{0.0f, 0.0f}; // Vettore di movimento (es. {100.f, 0.f} va a destra)
    float speed{200.0f};               // Velocità base in pixel al secondo
};

// -----------------------------------------------------------------------------
// PlayerComponent (TAG)
// È una struct VUOTA. Serve solo come "etichetta" per dire al sistema di input:
// "Ehi, è QUESTA l'entità che devi muovere quando l'utente preme i tasti!"
// -----------------------------------------------------------------------------
struct PlayerComponent {};