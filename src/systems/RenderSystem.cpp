#include "systems/RenderSystem.hpp"
#include "components/Components.hpp"

void RenderSystem::Draw(entt::registry& registry, sf::RenderTarget& target) {
    // 1. ORDINAMENTO (z-sorting)
    // Ordiniamo in memoria gli SpriteComponent in base al loro z_index.
    // TODO: più avanti bisognerà rendere dinamico l'ordinamento, quando le entity si nuoveranno
    //      all'interno della mappa di gioco.
    registry.sort<SpriteComponent>([](const auto& lhs, const auto& rhs) {
        return lhs.z_index < rhs.z_index;
    });

    // 2. LA QUERY: Otteniamo una "Vista" di tutte le entità che possiedono
    // sia un TransformerComponent sia uno SpriteComponent.
    auto view = registry.view<TransformComponent, SpriteComponent>();

    // 3. IL LOOP: Iteriamo su tutte le entità trovate (dal livello più basso al più alto)
    for (auto entity : view) {
        // Recuperiamo i dati reali della i-esima specifica entità
        auto& transform = view.get<TransformComponent>(entity);
        auto& sprite_comp = view.get<SpriteComponent>(entity);

        // 3. APPLICAZIONE DEI DATI: Aggiorniamo lo sprite di SFML usando i nostri dati POD
        // Applicazione in 2.5D, se elevation è 0, Y resta normale, se è > 0, Y diminuisce (spostamento verso l'alto)
        sprite_comp.sprite.setPosition(
            transform.position.x, 
            transform.position.y - transform.elevation
        );
        
        sprite_comp.sprite.setScale(transform.scale);
        sprite_comp.sprite.setRotation(transform.rotation);

        // 4. RENDERING: Disegniamo lo sprite a schermo
        target.draw(sprite_comp.sprite);
    }
}