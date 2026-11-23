#include "ChromaEffectsPlatform.h"

HotbarButtonState::HotbarButtonState()
{
    Clear();
}

void HotbarButtonState::Clear()
{
    fUsable = false;
    fOnCooldown = false;
    eKey = -1;
    dwFlashStartTime = 0;
}
