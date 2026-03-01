#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "systems/RenderSystem.hpp"
#include "components/Components.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include "systems/PlayerInputSystem.hpp"
#include "systems/MovementSystem.hpp"
// #include "systems/WorldSystem.hpp"
// #include "systems/PlayerSystem.hpp"
// #include "systems/InventorySystem.hpp"
// #include "systems/CombatSystem.hpp"
// #include "systems/QuestSystem.hpp"
// #include "systems/SaveSystem.hpp"
// #include "systems/LoadSystem.hpp"
// #include "systems/DebugSystem.hpp"
// #include "systems/RenderSystem.hpp"
// #include "systems/WorldSystem.hpp"
// #include "systems/PlayerSystem.hpp"
// #include "systems/InventorySystem.hpp"
// #include "systems/CombatSystem.hpp"
// #include "systems/QuestSystem.hpp"
// #include "systems/SaveSystem.hpp"
// #include "systems/LoadSystem.hpp"
// #include "systems/DebugSystem.hpp"

GameState::GameState(GameDataRef data) 
    : State("GameState"),
    data_(data) {}

GameState::~GameState() {
    Logger::Debug("Distruzione GameState: Pulizia del registry EnTT...");
    data_->registry.clear(); // Distrugge TUTTE le entità e libera la RAM
}

void GameState::Init() {
    Logger::Debug("Inizializzazione ({})", this->GetStateName());

    // 1. CARICAMENTO ASSET
    data_->assets.LoadAsset<sf::Texture>(
        std::string(Config::Game::Textures::kHeroTexName), 
        std::string(Config::Game::Textures::kHeroTexPath)
    );

    // 2. CREAZIONE ENTITÀ
    hero_ = data_->registry.create(); // hero_ ora vale 0 (o l'ID disponibile)
    data_->registry.emplace<PlayerComponent>(hero_); // Aggiungiamo il Tag Player

    // 3. ASSEGNAZIONE COMPONENTI
    // Aggiungiamo il Transform: lo mettiamo al centro della risoluzione logica
    auto& transform = data_->registry.emplace<TransformComponent>(hero_);
    transform.position = sf::Vector2f(
        Config::Game::kWindowWidth / 2.0f, 
        Config::Game::kWindowHeight / 2.0f
    );
    transform.elevation = 0.0f;
    transform.scale = sf::Vector2f(1.0f, 1.0f); // Lo facciamo un po' più grande

    // Aggiungiamo il componente Velocità
    data_->registry.emplace<VelocityComponent>(hero_);

    // Aggiungiamo lo Sprite
    auto& sprite_comp = data_->registry.emplace<SpriteComponent>(hero_);
    sprite_comp.sprite.setTexture(data_->assets.GetAsset<sf::Texture>(std::string(Config::Game::Textures::kHeroTexName)));
    
    // Centriamo l'origine dello sprite a metà dell'immagine
    sf::FloatRect bounds = sprite_comp.sprite.getLocalBounds();
    sprite_comp.sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite_comp.z_index = 10;
    
    Logger::Info("Eroe spawnato con successo con ID: {}", static_cast<uint32_t>(hero_));
}

void GameState::HandleInput() {
    sf::Event event;
    while (data_->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            data_->window.close();
        }
        
        // Tasto ESC per uscire dal gioco e tornare al menu
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            Logger::Info("Uscita dalla partita, torno al Menu...");
            data_->machine.AddState(std::make_unique<MainMenuState>(data_), true);
        }
    }
}

void GameState::Update(float dt) {
    // 1. Legge i tasti e calcola l'intento (modifica le Velocity)
    PlayerInputSystem::Update(data_->registry);

    // 2. Applica la fisica universale (sposta i Transform)
    MovementSystem::Update(data_->registry, dt);
}

void GameState::UpdateVisuals(float dt) {
    // Per ora vuoto
}

void GameState::Draw() {
    // MAGIA DELL'ECS: Disegniamo TUTTO il mondo di gioco in una sola riga!
    RenderSystem::Draw(data_->registry, data_->window);
}