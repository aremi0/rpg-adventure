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
        float visual_height = 0.f;
        if (const auto* elevation = registry.try_get<ElevationComponent>(entity)) {
            visual_height = elevation->height;
        }
        sprite_comp.sprite.setPosition(
            transform.position.x, 
            transform.position.y - visual_height
        );
        
        sprite_comp.sprite.setScale(transform.scale);
        sprite_comp.sprite.setRotation(transform.rotation);

        // 4. RENDERING: Disegniamo lo sprite a schermo
        target.draw(sprite_comp.sprite);
    }
}