#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <iostream>

class AudioManager {
private:

    sf::Music menuMusic;
    sf::Music gameplayMusic;
    sf::Music splashMusic;
    sf::Music gameWinMusic;
    sf::Music* currentMusic;


    sf::SoundBuffer spaceSoundBuffer;
    sf::SoundBuffer laserSoundBuffer;
    sf::SoundBuffer bombSoundBuffer;
    sf::SoundBuffer achievementSoundBuffer;
    sf::SoundBuffer gameOverSoundBuffer;
    sf::SoundBuffer menuClickSoundBuffer;
    sf::SoundBuffer menuBackSoundBuffer;
    std::vector<sf::SoundBuffer> wowSoundBuffers;


    std::unique_ptr<sf::Sound> spaceSound;
    std::unique_ptr<sf::Sound> laserSound;
    std::unique_ptr<sf::Sound> bombSound;
    std::unique_ptr<sf::Sound> achievementSound;
    std::unique_ptr<sf::Sound> gameOverSound;
    std::unique_ptr<sf::Sound> menuClickSound;
    std::unique_ptr<sf::Sound> menuBackSound;
    std::vector<std::unique_ptr<sf::Sound>> wowSounds;


    float masterVolume;
    float lastMasterVolume;
    bool isMuted;


    const float menuMusicVolume = 60.0f;
    const float gameplayMusicVolume = 60.0f;
    const float splashMusicVolume = 100.0f;
    const float gameWinMusicVolume = 80.0f;
    const float spaceVolume = 15.0f;
    const float laserVolume = 15.0f;
    const float bombVolume = 120.0f;
    const float achievementVolume = 120.0f;
    const float gameOverVolume = 150.0f;
    const float wowVolume = 70.0f;
    const float menuClickVolume = 100.0f;
    const float menuBackVolume = 100.0f;


    void updateAllVolumes();

public:
    AudioManager();
    ~AudioManager() = default;


    bool loadAllAudio();


    void playMenuMusic();
    void playGameplayMusic();
    void playSplashMusic();
    void playGameWinSound();
    void switchToGameplayMusic();
    void switchToMenuMusic();
    void stopAllMusic();


    void playSpaceSound();
    void playLaserSound();
    void playBombSound();
    void playAchievementSound();
    void playGameOverSound();
    void playWowSound(int index);
    void playMenuClickSound();
    void playMenuBackSound();


    void setMasterVolume(float volume);
    float getMasterVolume() const;
    void increaseMasterVolume();
    void decreaseMasterVolume();
    void toggleMute();
    bool isMutedStatus() const;


    sf::Sound* getSpaceSound() { return spaceSound.get(); }
    sf::Sound* getLaserSound() { return laserSound.get(); }
    sf::Sound* getBombSound() { return bombSound.get(); }
    sf::Sound* getAchievementSound() { return achievementSound.get(); }
    sf::Sound* getGameOverSound() { return gameOverSound.get(); }
    std::vector<std::unique_ptr<sf::Sound>>& getWowSounds() { return wowSounds; }
};

#endif
