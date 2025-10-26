#include "MiniaudioService.h"
#include <stdio.h>

MiniaudioService::MiniaudioService() : musicLoaded(false), initialized(false)
{
    ma_result result = ma_engine_init(NULL, &engine);
    if (result == MA_SUCCESS) {
        initialized = true;
        printf("Audio engine initialized!\n");
    }
    else {
        printf("Failed to initialize audio engine\n");
    }
}

MiniaudioService::~MiniaudioService()
{
    // Clean up all active sounds
    for (ma_sound* sound : activeSounds) {
        ma_sound_uninit(sound);
        delete sound;
    }
    activeSounds.clear();

    if (musicLoaded) {
        ma_sound_uninit(&currentMusic);
    }
    if (initialized) {
        ma_engine_uninit(&engine);
    }
}

void MiniaudioService::playSound(const char* filepath)
{
    if (!initialized) return;

    ma_sound* sound = new ma_sound;
    ma_result result = ma_sound_init_from_file(&engine, filepath,
        MA_SOUND_FLAG_DECODE, NULL, NULL, sound);

    if (result == MA_SUCCESS) {
        ma_sound_start(sound);
        activeSounds.push_back(sound);
    }
    else {
        printf("Failed to play sound: %s\n", filepath);
        delete sound;
    }
}

void MiniaudioService::playSoundOnce(const char* filepath, const char* soundId)
{
    if (!initialized) return;

    // Si ya está sonando, no hacer nada
    if (playingOnceSounds.find(soundId) != playingOnceSounds.end()) {
        return;
    }

    // Marcar como "sonando"
    playingOnceSounds.insert(soundId);

    // Reproducir el sonido normalmente
    playSound(filepath);
}

void MiniaudioService::update()
{
    cleanupFinishedSounds();
    playingOnceSounds.clear();
}

void MiniaudioService::cleanupFinishedSounds()
{
    for (auto it = activeSounds.begin(); it != activeSounds.end(); ) {
        ma_sound* sound = *it;

        // Check if sound has finished playing
        if (!ma_sound_is_playing(sound)) {
            ma_sound_uninit(sound);
            delete sound;
            it = activeSounds.erase(it);
        }
        else {
            ++it;
        }
    }
}

void MiniaudioService::playMusic(const char* filepath, bool loop)
{
    if (!initialized) return;

    if (musicLoaded) {
        ma_sound_uninit(&currentMusic);
        musicLoaded = false;
    }

    ma_result result = ma_sound_init_from_file(&engine, filepath, 0, NULL, NULL, &currentMusic);
    if (result == MA_SUCCESS) {
        ma_sound_set_looping(&currentMusic, loop ? MA_TRUE : MA_FALSE);
        ma_sound_set_volume(&currentMusic, 0.2f);
        ma_sound_start(&currentMusic);
        musicLoaded = true;
    }
}

void MiniaudioService::stopMusic()
{
    if (initialized && musicLoaded) {
        ma_sound_stop(&currentMusic);
    }
}

void MiniaudioService::stopAllSounds()
{
    // Stop and clean all sound effects
    for (ma_sound* sound : activeSounds) {
        ma_sound_stop(sound);
        ma_sound_uninit(sound);
        delete sound;
    }
    activeSounds.clear();

    // Stop music
    if (musicLoaded) {
        ma_sound_stop(&currentMusic);
    }
}