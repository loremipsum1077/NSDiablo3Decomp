// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/LocalPlayerClamping.h

#pragma once

#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Vector.h"

namespace LocalPlayerClamping {
enum PlayerActivity {
    PA_NOT_CONNECTED = 0,
    PA_INACTIVE = 1,
    PA_DRAGGABLE = 2,
    PA_ACTIVE = 3,
    PA_INVALID_LOCATION = 4,
};

enum ActivityReason {
    AR_MOVEMENT = 0,
    AR_ABILITY_USED = 1,
};

struct InputPlayerState {  // size 0x1c
    WorldPlace m_tPlace;  // 0x00
    Vector2D m_stickDirection;  // 0x10
    LocalPlayerClamping::PlayerActivity m_eActivity;  // 0x18
};
static_assert(sizeof(InputPlayerState) == 0x1c, "InputPlayerState");

struct InputGameState {  // size 0x70
    LocalPlayerClamping::InputPlayerState m_players[4];  // 0x00
};
static_assert(sizeof(InputGameState) == 0x70, "InputGameState");

// X/Client/Console/LocalPlayerClamping.cpp:195
void Update();

// X/Client/Console/LocalPlayerClamping.cpp:230
Vector2D GetLStickPosition(int32 nControllerIndex);

}  // namespace LocalPlayerClamping
