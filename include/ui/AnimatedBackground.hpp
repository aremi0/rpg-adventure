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
        void SetFrameDuration(float duration);

        void Update(float dt);
        void Draw();
        
        void Resize(const sf::Vector2u& window_size);

    private:
        GameDataRef data_;
        std::vector<std::string> frames_;
        sf::Sprite sprite_;

        size_t current_frame_index_;
        float animation_timer_;
        float frame_duration_;
};
