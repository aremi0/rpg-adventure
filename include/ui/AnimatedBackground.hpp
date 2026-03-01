#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

// Forward declaration per evitare dipendenze circolari
struct GameData;
using GameDataRef = std::shared_ptr<GameData>;

class AnimatedBackground {
    public:
        AnimatedBackground(GameDataRef data);

        void AddFrame(const std::string& texture_name);

        void Update(float dt);
        void Draw();
        
        void Resize(const sf::Vector2u& window_size);

    private:
        GameDataRef data_;
        sf::Sprite sprite_;

        // È sicuro perchè il re-hashing della std::unordered_map sposta i nodi ma non cambia
        // l'indirizzo di memoria fisico degli elementi (delle Texture in questo caso).
        // In questo modo non avremo mai dangling pointer.
        std::vector<const sf::Texture*> frames_;

        size_t current_frame_index_;
        float animation_timer_;
        float frame_duration_;
};
