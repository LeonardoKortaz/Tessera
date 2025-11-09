#include "audio_manager.h"

AudioManager::AudioManager()
    : currentMusic(nullptr)
    , masterVolume(100.0f)
    , lastMasterVolume(100.0f)
    , isMuted(false)
{
}

bool AudioManager::loadAllAudio() {
    bool allLoaded = true;


    if (splashMusic.openFromFile("Assets/Sound/SFX/Kilonia Studios Splash Screen.mp3")) {
        splashMusic.setLooping(false);
        splashMusic.setVolume((splashMusicVolume * masterVolume) / 100.0f);
        std::cout << "Splash music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load splash music (Assets/Sound/SFX/Kilonia Studios Splash Screen.mp3)" << std::endl;
        allLoaded = false;
    }


    if (menuMusic.openFromFile("Assets/Sound/Music/Jigzter_Menu.ogg")) {
        menuMusic.setLooping(true);
        menuMusic.setVolume((menuMusicVolume * masterVolume) / 100.0f);
        std::cout << "Menu music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load menu music (Assets/Sound/Music/Jigzter_Menu.ogg)" << std::endl;
        allLoaded = false;
    }


    if (gameplayMusic.openFromFile("Assets/Sound/Music/Jigzter_Main.ogg")) {
        gameplayMusic.setLooping(true);
        gameplayMusic.setVolume((gameplayMusicVolume * masterVolume) / 100.0f);
        std::cout << "Gameplay music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load gameplay music (Assets/Sound/Music/Jigzter_Main.ogg)" << std::endl;
        allLoaded = false;
    }


    if (gameWinMusic.openFromFile("Assets/Sound/SFX/Game_Win.ogg")) {
        gameWinMusic.setLooping(false);
        gameWinMusic.setVolume((gameWinMusicVolume * masterVolume) / 100.0f);
        std::cout << "Game Win music loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load game win music (Assets/Sound/Music/Game_Win.ogg)" << std::endl;
        allLoaded = false;
    }


    if (spaceSoundBuffer.loadFromFile("Assets/Sound/SFX/ground.ogg")) {
        spaceSound = std::make_unique<sf::Sound>(spaceSoundBuffer);
        spaceSound->setVolume((spaceVolume * masterVolume) / 100.0f);
        std::cout << "Space sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load space sound (Assets/Sound/SFX/ground.ogg)" << std::endl;
        allLoaded = false;
    }


    if (laserSoundBuffer.loadFromFile("Assets/Sound/SFX/laser.ogg")) {
        laserSound = std::make_unique<sf::Sound>(laserSoundBuffer);
        laserSound->setVolume((laserVolume * masterVolume) / 100.0f);
        std::cout << "Laser sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load laser sound (Assets/Sound/SFX/laser.ogg)" << std::endl;
        allLoaded = false;
    }


    if (bombSoundBuffer.loadFromFile("Assets/Sound/SFX/bomb.ogg")) {
        bombSound = std::make_unique<sf::Sound>(bombSoundBuffer);
        bombSound->setVolume((bombVolume * masterVolume) / 100.0f);
        std::cout << "Bomb sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load bomb sound (Assets/Sound/SFX/bomb.ogg)" << std::endl;
        allLoaded = false;
    }


    if (achievementSoundBuffer.loadFromFile("Assets/Sound/SFX/Achievement.ogg")) {
        achievementSound = std::make_unique<sf::Sound>(achievementSoundBuffer);
        achievementSound->setVolume((achievementVolume * masterVolume) / 100.0f);
        std::cout << "Achievement sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load achievement sound (Assets/Sound/SFX/Achievement.ogg)" << std::endl;
        allLoaded = false;
    }


    if (gameOverSoundBuffer.loadFromFile("Assets/Sound/SFX/Game_Over.ogg")) {
        gameOverSound = std::make_unique<sf::Sound>(gameOverSoundBuffer);
        gameOverSound->setVolume((gameOverVolume * masterVolume) / 100.0f);
        std::cout << "Game Over sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load game over sound (Assets/Sound/SFX/Game_Over.ogg)" << std::endl;
        allLoaded = false;
    }


    if (menuClickSoundBuffer.loadFromFile("Assets/Sound/SFX/Menu_Click.ogg")) {
        menuClickSound = std::make_unique<sf::Sound>(menuClickSoundBuffer);
        menuClickSound->setVolume((menuClickVolume * masterVolume) / 100.0f);
        std::cout << "Menu Click sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load menu click sound (Assets/Sound/SFX/Menu_Click.ogg)" << std::endl;
        allLoaded = false;
    }


    if (menuBackSoundBuffer.loadFromFile("Assets/Sound/SFX/Menu_Back.ogg")) {
        menuBackSound = std::make_unique<sf::Sound>(menuBackSoundBuffer);
        menuBackSound->setVolume((menuBackVolume * masterVolume) / 100.0f);
        std::cout << "Menu Back sound loaded successfully!" << std::endl;
    } else {
        std::cout << "Unable to load menu back sound (Assets/Sound/SFX/Menu_Back.ogg)" << std::endl;
        allLoaded = false;
    }


    wowSoundBuffers.resize(3);
    wowSounds.resize(3);
    for (int i = 0; i < 3; i++) {
        std::string filename = "Assets/Sound/SFX/wow_0" + std::to_string(i + 1) + ".ogg";
        if (wowSoundBuffers[i].loadFromFile(filename)) {
            wowSounds[i] = std::make_unique<sf::Sound>(wowSoundBuffers[i]);
            wowSounds[i]->setVolume((wowVolume * masterVolume) / 100.0f);
            std::cout << "Wow sound loaded successfully: " << filename << std::endl;
        } else {
            std::cout << "Unable to load wow sound: " << filename << std::endl;
            allLoaded = false;
        }
    }

    return allLoaded;
}

void AudioManager::updateAllVolumes() {

    menuMusic.setVolume((menuMusicVolume * masterVolume) / 100.0f);
    gameplayMusic.setVolume((gameplayMusicVolume * masterVolume) / 100.0f);
    splashMusic.setVolume((splashMusicVolume * masterVolume) / 100.0f);
    gameWinMusic.setVolume((gameWinMusicVolume * masterVolume) / 100.0f);


    if (spaceSound) {
        spaceSound->setVolume((spaceVolume * masterVolume) / 100.0f);
    }
    if (laserSound) {
        laserSound->setVolume((laserVolume * masterVolume) / 100.0f);
    }
    if (bombSound) {
        bombSound->setVolume((bombVolume * masterVolume) / 100.0f);
    }
    if (achievementSound) {
        achievementSound->setVolume((achievementVolume * masterVolume) / 100.0f);
    }
    if (gameOverSound) {
        gameOverSound->setVolume((gameOverVolume * masterVolume) / 100.0f);
    }
    if (menuClickSound) {
        menuClickSound->setVolume((menuClickVolume * masterVolume) / 100.0f);
    }
    if (menuBackSound) {
        menuBackSound->setVolume((menuBackVolume * masterVolume) / 100.0f);
    }
    for (auto& wowSound : wowSounds) {
        if (wowSound) {
            wowSound->setVolume((wowVolume * masterVolume) / 100.0f);
        }
    }
}

void AudioManager::playMenuMusic() {
    if (currentMusic != &menuMusic) {
        if (currentMusic) {
            currentMusic->stop();
        }
        menuMusic.play();
        currentMusic = &menuMusic;
    }
}

void AudioManager::playGameplayMusic() {
    if (currentMusic != &gameplayMusic) {
        if (currentMusic) {
            currentMusic->stop();
        }
        gameplayMusic.play();
        currentMusic = &gameplayMusic;
    }
}

void AudioManager::playSplashMusic() {
    if (currentMusic) {
        currentMusic->stop();
    }
    splashMusic.play();
    currentMusic = &splashMusic;
}

void AudioManager::playGameWinSound() {
    if (currentMusic) {
        currentMusic->stop();
    }
    gameWinMusic.play();
    currentMusic = &gameWinMusic;
    std::cout << "Playing Game Win music!" << std::endl;
}

void AudioManager::switchToGameplayMusic() {
    playGameplayMusic();
}

void AudioManager::switchToMenuMusic() {
    playMenuMusic();
}

void AudioManager::stopAllMusic() {
    menuMusic.stop();
    gameplayMusic.stop();
    gameWinMusic.stop();
    currentMusic = nullptr;
}

void AudioManager::playSpaceSound() {
    if (spaceSound) {
        spaceSound->play();
    }
}

void AudioManager::playLaserSound() {
    if (laserSound) {
        laserSound->play();
    }
}

void AudioManager::playBombSound() {
    if (bombSound) {
        bombSound->play();
    }
}

void AudioManager::playAchievementSound() {
    if (achievementSound) {
        achievementSound->play();
    }
}

void AudioManager::playGameOverSound() {
    if (gameOverSound) {
        gameOverSound->play();
    }
}

void AudioManager::playWowSound(int index) {
    if (index >= 0 && index < static_cast<int>(wowSounds.size()) && wowSounds[index]) {
        wowSounds[index]->play();
    }
}

void AudioManager::playMenuClickSound() {
    if (menuClickSound) {
        menuClickSound->play();
    }
}

void AudioManager::playMenuBackSound() {
    if (menuBackSound) {
        menuBackSound->play();
    }
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume = std::max(0.0f, std::min(100.0f, volume));
    updateAllVolumes();
}

float AudioManager::getMasterVolume() const {
    return masterVolume;
}

void AudioManager::increaseMasterVolume() {
    if (!isMuted && masterVolume < 100.0f) {
        masterVolume = std::min(100.0f, masterVolume + 10.0f);
        updateAllVolumes();
        std::cout << "Master volume increased to: " << masterVolume << "%" << std::endl;
    } else if (isMuted) {
        std::cout << "Cannot change volume while muted" << std::endl;
    } else {
        std::cout << "Master volume already at maximum (100%)" << std::endl;
    }
}

void AudioManager::decreaseMasterVolume() {
    if (!isMuted && masterVolume > 0.0f) {
        masterVolume = std::max(0.0f, masterVolume - 10.0f);
        updateAllVolumes();
        std::cout << "Master volume decreased to: " << masterVolume << "%" << std::endl;
    } else if (isMuted) {
        std::cout << "Cannot change volume while muted" << std::endl;
    } else {
        std::cout << "Master volume already at minimum (0%)" << std::endl;
    }
}

void AudioManager::toggleMute() {
    if (isMuted) {
        isMuted = false;
        masterVolume = lastMasterVolume;
        updateAllVolumes();
        std::cout << "Audio UNMUTED - Master volume restored to: " << masterVolume << "%" << std::endl;
    } else {
        isMuted = true;
        lastMasterVolume = masterVolume;
        masterVolume = 0.0f;
        updateAllVolumes();
        std::cout << "Audio MUTED" << std::endl;
    }
}

bool AudioManager::isMutedStatus() const {
    return isMuted;
}
