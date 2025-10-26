#ifndef _NULL_AUDIO_SERVICE_INCLUDE
#define _NULL_AUDIO_SERVICE_INCLUDE

#include "AudioService.h"

class NullAudioService : public AudioService
{
public:
    void playSound(const char* filepath) override {}
    void playMusic(const char* filepath, bool loop = true) override {}
    void playSoundOnce(const char* filepath, const char* soundId) override {}
    void stopMusic() override {}
    void stopAllSounds() override {}
    void update() override {}
};

#endif
