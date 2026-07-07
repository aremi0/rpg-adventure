#include "states/GameState.hpp"
#include "states/MainMenuState.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/MapRenderSystem.hpp"
#include "systems/DebugRenderSystem.hpp"
#include "components/Components.hpp"
#include "utils/Logger.hpp"
#include "core/Constants.hpp"
#include "systems/PlayerInputSystem.hpp"
#include "systems/CollisionSystem.hpp"
#include "systems/RampTransitionSystem.hpp"
#include "map/LdtkEntityUtils.hpp"
#include "map/MapCollisionUtils.hpp"

GameState::GameState(GameDataRef data) 
    : State("GameState"),
    data_(data) {}

GameState::~GameState() {
    Logger::Debug("Distruzione GameState: Pulizia del registry EnTT...");
    data_->registry.clear(); // Distrugge TUTTE le entità e libera la RAM
}

void GameState::Init() {
    Logger::Debug("Inizializzazione ({})", this->GetStateName());

    map_ready_ = false;
    map_renderer_.Clear();

    // 1. CARICAMENTO ASSET
    data_->assets.LoadAsset<sf::Texture>(
        std::string(Config::Game::Textures::kHeroTexName), 
        std::string(Config::Game::Textures::kHeroTexPath)
    );
    data_->assets.LoadAsset<sf::Font>(
        std::string(Config::Game::kFontName), 
        std::string(Config::Game::kFontPath)
    );

    // 2. CARICAMENTO MAPPA
    if (auto map_result = data_->map.Load(
            std::string(Config::Map::kWorldMapPath));
        map_result) {
        if (auto build_result = map_renderer_.Build(data_->map.GetLevel(),
                                                    data_->map.GetTileset());
            build_result) {
            map_ready_ = true;
            Logger::Info("Mappa pronta per il rendering");
        } else {
            Logger::Error("MapRenderSystem build fallito: {}",
                          build_result.error());
        }
    } else {
        Logger::Error("Caricamento mappa fallito: {}", map_result.error());
    }

    // 3. CREAZIONE ENTITÀ
    hero_ = data_->registry.create(); // hero_ ora vale 0 (o l'ID disponibile)
    data_->registry.emplace<PlayerComponent>(hero_); // Aggiungiamo il Tag Player

    // 4. ASSEGNAZIONE COMPONENTI
    auto& transform = data_->registry.emplace<TransformComponent>(hero_);
    transform.scale = sf::Vector2f(1.0f, 1.0f);

    sf::Vector2f spawn_position{
        Config::Game::kLogicalWidth / 2.0f,
        Config::Game::kLogicalHeight / 2.0f};
    Facing spawn_facing = Facing::kSouth;

    if (map_ready_) {
        const auto& level = data_->map.GetLevel();
        const int tile_size =
            level.tile_size > 0 ? level.tile_size : Config::Map::kTileSize;

        auto resolve_spawn_position =
            [this](const sf::Vector2f preferred,
                   const char* context) -> sf::Vector2f {
              if (const auto resolved =
                      FindNearbySpawnPosition(data_->map, preferred.x,
                                            preferred.y)) {
                if (*resolved != preferred) {
                  Logger::Info("{}: spawn spostato a ({:.0f},{:.0f})",
                               context, resolved->x, resolved->y);
                }
                return *resolved;
              }
              Logger::Warn(
                  "{}: nessun tile walkable con vicino adiacente entro {} "
                  "celle, uso ({:.0f},{:.0f})",
                  context, Config::Map::kMaxSpawnSearchRadius, preferred.x,
                  preferred.y);
              return preferred;
            };

        if (const auto* player_start = data_->map.FindEntity("PlayerStart")) {
            spawn_position = GetEntityCenter(*player_start, tile_size);
            if (const auto facing_str = GetFieldString(*player_start, "Facing")) {
                spawn_facing = ParseFacing(*facing_str);
            }

            const auto collision =
                data_->map.CollisionAt(player_start->grid_x, player_start->grid_y);
            Logger::Info(
                "PlayerStart: grid ({},{}), pos ({:.0f},{:.0f}), facing {}, "
                "collision {}",
                player_start->grid_x, player_start->grid_y, spawn_position.x,
                spawn_position.y, FacingToString(spawn_facing),
                CollisionTypeToString(collision));

            if (!IsPassableCollision(collision) ||
                !IsValidSpawnTile(data_->map, player_start->grid_x,
                                  player_start->grid_y)) {
                Logger::Warn("PlayerStart su tile non valido per spawn");
                spawn_position =
                    resolve_spawn_position(spawn_position, "PlayerStart");
            }
        } else {
            Logger::Warn("PlayerStart non trovato, fallback centro schermo");
            spawn_position =
                resolve_spawn_position(spawn_position, "Spawn fallback");
        }
    }

    transform.position = spawn_position;
    data_->registry.emplace<FacingComponent>(
        hero_, FacingComponent{.facing = spawn_facing});

    // Aggiungiamo il componente Velocità
    data_->registry.emplace<VelocityComponent>(hero_);

    // Aggiungiamo lo Sprite
    auto& sprite_comp = data_->registry.emplace<SpriteComponent>(hero_);
    sprite_comp.sprite.setTexture(data_->assets.GetAsset<sf::Texture>(std::string(Config::Game::Textures::kHeroTexName)));
    
    // Centriamo l'origine dello sprite a metà dell'immagine
    sf::FloatRect bounds = sprite_comp.sprite.getLocalBounds();
    sprite_comp.sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    sprite_comp.z_index = 10;

    auto& collider = data_->registry.emplace<BoxColliderComponent>(hero_);
    collider.size = sf::Vector2f(bounds.width * 0.5f, bounds.height * 0.25f);
    collider.offset = sf::Vector2f(0.f, bounds.height * 0.25f);

    auto& elevation = data_->registry.emplace<ElevationComponent>(hero_);
    if (map_ready_) {
        const auto& level = data_->map.GetLevel();
        const sf::FloatRect foot = GetFootAabb(transform, collider);
        const float foot_x = foot.left + foot.width * 0.5f;
        const float foot_y = foot.top + foot.height * 0.5f;
        int tx = 0;
        int ty = 0;
        WorldToTile(foot_x, foot_y, level, tx, ty);
        elevation.floor_level = data_->map.ElevationAt(tx, ty);
        elevation.height = static_cast<float>(elevation.floor_level) *
                           Config::Map::kHeightPerLevel;
    }
    
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

        // Tasto per togglare il debug rendering
        if (Config::Debug::kEnableVisualDebug && event.type == sf::Event::KeyPressed && event.key.code == Config::Debug::kVisualDebugKey) {
            is_debug_mode_ = !is_debug_mode_;
            Logger::Info("Debug mode toggled: {}", is_debug_mode_ ? "ON" : "OFF");
        }

        // Tasto per ciclare il livello di log
        if (Config::Debug::kEnableLogSwitching && event.type == sf::Event::KeyPressed && event.key.code == Config::Debug::kLogSwitchKey) {
            Logger::CycleLevel();
        }
    }
}

void GameState::Update(float dt) {
    PlayerInputSystem::Update(data_->registry);
    CollisionSystem::Update(data_->registry, data_->map, dt);
    RampTransitionSystem::Update(data_->registry, data_->map, dt);
}

void GameState::UpdateVisuals(float dt) {
    // Per ora vuoto
}

void GameState::Draw() {
    if (map_ready_) {
        map_renderer_.Draw(data_->window);
    }

    RenderSystem::Draw(data_->registry, data_->window);

    // 2. Rendering del debug (se attivo e abilitato)
    if (Config::Debug::kEnableVisualDebug && is_debug_mode_) {
        // Recuperiamo il font dall'asset manager
        const sf::Font& font = data_->assets.GetAsset<sf::Font>(std::string(Config::Game::kFontName));
        DebugRenderSystem::Draw(data_->registry, data_->window, font,
                                map_ready_ ? &data_->map : nullptr);
    }
}