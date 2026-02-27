#include "ui/AnimatedBackground.hpp"
#include "core/Constants.hpp"
#include "core/Game.hpp"
#include "utils/Logger.hpp"

AnimatedBackground::AnimatedBackground(GameDataRef data) 
    : data_(data), current_frame_index_(0), animation_timer_(0.0f), frame_duration_(Config::MainMenu::kFrameDuration) {}

void AnimatedBackground::AddFrame(const std::string& texture_name) {
    frames_.push_back(texture_name);

    // Se è il primo frame aggiunto, lo imposto subito come texture corrente
    if (frames_.size() == 1) {
        const auto& tex = data_->assets.GetAsset<sf::Texture>(texture_name);
        sprite_.setTexture(tex);
    }
}

void AnimatedBackground::Update(float dt) {
    if (frames_.empty()) return;

    animation_timer_ += dt;

    if (animation_timer_ >= frame_duration_) {
        animation_timer_ -= frame_duration_;
        current_frame_index_ = (current_frame_index_ + 1) % frames_.size();

        const auto& next_frame_name = frames_[current_frame_index_];
        const auto& next_tex = data_->assets.GetAsset<sf::Texture>(next_frame_name);
        sprite_.setTexture(next_tex);
    }
}

void AnimatedBackground::Draw() {
    if (frames_.empty()) return;
    data_->window.draw(sprite_);
}

void AnimatedBackground::Resize(const sf::Vector2u& window_size) {
    if (frames_.empty()) return;
    
    // Per sicurezza, prendiamo la texture dallo sprite (se c'è)
    const sf::Texture* tex = sprite_.getTexture();
    if (!tex) return;

    sf::Vector2u tex_size = tex->getSize();
    
    sprite_.setScale(
        static_cast<float>(window_size.x) / tex_size.x,
        static_cast<float>(window_size.y) / tex_size.y
    );
}
