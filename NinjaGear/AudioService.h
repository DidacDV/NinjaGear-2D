#ifndef _AUDIO_SERVICE_INCLUDE
#define _AUDIO_SERVICE_INCLUDE

class AudioService
{
public:
    virtual ~AudioService() {}
    virtual void playSound(const char* filepath) = 0;
    virtual void playSoundOnce(const char* filepath, const char* soundId) = 0;
    virtual void playMusic(const char* filepath, bool loop = true) = 0;
    virtual void stopMusic() = 0;
    virtual void stopAllSounds() = 0;
    virtual void update() = 0; 
};

#endif