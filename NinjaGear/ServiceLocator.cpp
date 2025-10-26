#include "ServiceLocator.h"

AudioService* ServiceLocator::service_ = nullptr;
NullAudioService ServiceLocator::nullService_;