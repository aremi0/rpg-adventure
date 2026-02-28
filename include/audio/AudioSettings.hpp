#pragma once
#include "core/Constants.hpp"

// Gestisce i livelli di volume per canale audio.
// Il volume effettivo è: (master / 100) × (canale / 100) × 100
// Risultato: valore 0.0f – 100.0f pronto per sf::Sound::setVolume() / sf::Music::setVolume()
struct AudioSettings {
    int master = Config::Settings::kDefaultMasterVolume;
    int music  = Config::Settings::kDefaultMusicVolume;
    int sfx    = Config::Settings::kDefaultSfxVolume;
    int ui     = Config::Settings::kDefaultUiVolume;

    // Calcola il volume effettivo per un canale specifico
    [[nodiscard]] float GetEffectiveVolume(int channel_volume) const {
        return (static_cast<float>(master) / 100.0f) 
             * (static_cast<float>(channel_volume) / 100.0f) 
             * 100.0f;
    }

    [[nodiscard]] float GetMusicVolume() const { return GetEffectiveVolume(music); }
    [[nodiscard]] float GetSfxVolume()   const { return GetEffectiveVolume(sfx); }
    [[nodiscard]] float GetUiVolume()    const { return GetEffectiveVolume(ui); }
};
