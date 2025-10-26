#ifndef _SERVICE_LOCATOR_INCLUDE
#define _SERVICE_LOCATOR_INCLUDE

#include "AudioService.h"
#include "NullAudioService.h"

class ServiceLocator
{
public:
    static void initialize()
    {
        service_ = &nullService_;
    }

    static AudioService& getAudio()
    {
        return *service_;
    }

    static void provide(AudioService* service)
    {
        if (service == nullptr) {
            // Revert to null service
            service_ = &nullService_;
        }
        else {
            service_ = service;
        }
    }

private:
    static AudioService* service_;
    static NullAudioService nullService_;
};

#endif