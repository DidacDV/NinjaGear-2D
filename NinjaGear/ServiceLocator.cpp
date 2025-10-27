#include "ServiceLocator.h"

AudioService* ServiceLocator::service_ = nullptr;
NullAudioService ServiceLocator::nullService_;

UIManager* ServiceLocator::uiService_ = nullptr;
NullUIManager ServiceLocator::nullUiService_;