#include "systems/DebugRenderSystem.hpp"
#include "components/Components.hpp"
#include <string>
#include <format>

void DebugRenderSystem::Draw(entt::registry& registry, sf::RenderTarget& target, const sf::Font& font) {
    // Interroghiamo tutte le entità che hanno una posizione (TransformComponent)
    auto view = registry.view<TransformComponent>();
    
    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        
        // 1. CREAZIONE DEL RETTAGOLO (Bounding Box visiva)
        sf::RectangleShape rect;
        sf::Vector2f size(64.f, 64.f);
        
        sf::Color outlineColor = sf::Color::White;
        
        if (registry.any_of<PlayerComponent>(entity)) {
            outlineColor = sf::Color::Green;
        } else {
            outlineColor = sf::Color::Red;
        }
        
        if (registry.any_of<SpriteComponent>(entity)) {
            auto& spriteComp = registry.get<SpriteComponent>(entity);
            sf::FloatRect bounds = spriteComp.sprite.getLocalBounds();
            size = sf::Vector2f(bounds.width * transform.scale.x, bounds.height * transform.scale.y);
        }
        
        rect.setSize(size);
        rect.setOrigin(size.x / 2.0f, size.y / 2.0f);
        rect.setPosition(transform.position);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(outlineColor);
        rect.setOutlineThickness(2.0f);
        target.draw(rect);
        
        // 2. STAMPA DEI DATI (Testo Debug)
        sf::Text debug_info;
        debug_info.setFont(font);
        debug_info.setCharacterSize(14);
        debug_info.setFillColor(sf::Color::Yellow);
        
        // Formattiamo una stringa con ID e Coordinate
        std::string info_str = std::format("ID: {}\nPos: {:.1f}, {:.1f}\nElev: {:.1f}", 
                                           static_cast<uint32_t>(entity), 
                                           transform.position.x, 
                                           transform.position.y,
                                           transform.elevation);
        
        // Se l'entità ha una velocità, stampiamola
        if (auto* vel = registry.try_get<VelocityComponent>(entity)) {
            info_str += std::format("\nVel: {:.1f}, {:.1f}", vel->velocity.x, vel->velocity.y);
        }

        debug_info.setString(info_str);

        // Posizioniamo il testo sotto l'entità
        debug_info.setPosition(transform.position.x - size.x / 2.0f, transform.position.y + size.y / 2.0f);
        
        target.draw(debug_info);
    }
}
