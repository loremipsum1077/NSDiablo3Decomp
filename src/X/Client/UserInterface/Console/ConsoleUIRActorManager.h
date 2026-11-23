// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/ConsoleUIRActorManager.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console {
namespace UIRActorManager {

enum {
    RACTOR_CONTROL_NONE = 0,
    RACTOR_CONTROL_RING = 1,
    RACTOR_CONTROL_CENTERED = 2,
    NUM_RACTOR_CONTROLS = 3,
};

typedef int32 RActorControlType;

enum {
    RACTOR_CHARACTER_NONE = 0,
    RACTOR_CHARACTER_NPC = 1,
    RACTOR_CHARACTER_PLAYER = 2,
};

typedef int32 RActorCharacterType;

void Show();
void Hide();
void SetActive(BOOL bActive);
void ForceRActorRecreate();
void CloseRActor();
BOOL ShouldCreateRActor(RActorControlType eRActorControl, RActorCharacterType eRActorCharacterType, BOOL bForceRecreate, BOOL bClean);
void InitializeForGame();
void DisposeForGame();

}  // namespace UIRActorManager
}  // namespace Console
