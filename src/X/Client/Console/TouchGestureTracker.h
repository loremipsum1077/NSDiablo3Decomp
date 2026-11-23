// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Console/TouchGestureTracker.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../Sigma/Math/Vector.h"

namespace Console {
struct SwipeGesture {  // size 0xc
    int32 nControllerIndex;  // 0x00
    IVector2D vecDirection;  // 0x04
    // Bounded reconstruction: observed construction overwrites all members before use.
    SwipeGesture() = default;
};
static_assert(sizeof(SwipeGesture) == 0xc, "SwipeGesture");

typedef blz::vector<Console::SwipeGesture, blz::allocator<Console::SwipeGesture> > GestureArray;

class TouchGestureTracker {  // size 0x30
public:
    TouchGestureTracker() {}  // line 31
    int32 ProcessInput(Console::GestureArray & arGestures);
    int32 ProcessInput(int32 nControllerIndex, Console::GestureArray & arGestures);
    struct TrackingState {  // size 0xc
        Blizzard::Time::Millisecond tStartTime;  // 0x00
        IVector2D vecStartPoint;  // 0x04
        TrackingState() : tStartTime(0) {}  // line 54; only the start time is written
    };
private:
    Console::TouchGestureTracker::TrackingState m_arTrackingStates[4];  // 0x00
};
static_assert(sizeof(TouchGestureTracker) == 0x30, "TouchGestureTracker");

}  // namespace Console

