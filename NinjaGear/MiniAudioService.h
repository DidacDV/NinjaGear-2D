#ifndef _MINIAUDIO_SERVICE_INCLUDE
#define _MINIAUDIO_SERVICE_INCLUDE

#include "AudioService.h"
#include "miniaudio.h"
#include <vector>
#include <unordered_set>
#include <string>

class MiniaudioService : public AudioService
{
public:
    MiniaudioService();
    ~MiniaudioService();

    void playSound(const char* filepath) override;
    void playSoundOnce(const char* filepath, const char* soundId) override;
    void playMusic(const char* filepath, bool loop = true) override;
    void stopMusic() override;
    void stopAllSounds() override;
    void update();  

private:
    ma_engine engine;
    ma_sound currentMusic;
    std::vector<ma_sound*> activeSounds;
    std::unordered_set<std::string> playingOnceSounds;
    bool musicLoaded;
    bool initialized;

    void cleanupFinishedSounds();
};

#endif