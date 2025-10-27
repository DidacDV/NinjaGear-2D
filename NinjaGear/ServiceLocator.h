#ifndef _SERVICE_LOCATOR_INCLUDE
#define _SERVICE_LOCATOR_INCLUDE

#include "AudioService.h"
#include "NullAudioService.h"
#include "NullUIManager.h"

class ServiceLocator
{
public:
    static void initialize()
    {
        service_ = &nullService_;
        uiService_ = &nullUiService_;

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

    static UIManager& getUI()
    {
        return *uiService_;
    }

    static void provide(UIManager* service)
    {
        if (service == nullptr) {
            // Revert to null UI manager
            uiService_ = &nullUiService_;
        }
        else {
            uiService_ = service;
        }
    }

private:
    static AudioService* service_;
    static NullAudioService nullService_;
    static UIManager* uiService_;
    static NullUIManager nullUiService_;
};

#endif