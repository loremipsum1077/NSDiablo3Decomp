#include "TouchGestureTracker.h"
#include "../Input/Input.h"

BOOL InputIsControllerConnected(int32 nControllerIndex);











int32 s_nTouchDelay = 50;

namespace Console {



    int32 TouchGestureTracker::ProcessInput(
        GestureArray & arGestures)
    {
        int32 nGestureCount = 0;


        for (uint32 i = 0; i < 4; ++i) {
            if (InputIsControllerConnected(i))
                nGestureCount += ProcessInput(i, arGestures);
        }
        return nGestureCount;
    }







    int32 TouchGestureTracker::ProcessInput(
        int32 nControllerIndex,
        GestureArray & arGestures)
    {
        if (nControllerIndex == -1)
            return 0;



        int32 nDetectedGestureCount = 0;
        TrackingState & tTrackingState = m_arTrackingStates[nControllerIndex];

        const InputState & tInputState = InputGetCurrentState(INPUTCONTEXT_DEFAULT);
        ControllerState tControllerState = tInputState.tControllerState[nControllerIndex];

        Blizzard::Time::Millisecond nCurrentTime = Blizzard::Time::Milliseconds();

        if (tControllerState.tCurrentFrame.nNumTouches >= 1 &&
            !tControllerState.tCurrentFrame.IsButtonDown(16)) {

            const TouchState & tTouch = tControllerState.tCurrentFrame.arTouches[0];

            if (tTrackingState.tStartTime == 0) {

                tTrackingState.tStartTime = nCurrentTime;
                tTrackingState.vecStartPoint.Set(tTouch.uX, tTouch.uY);
            }

            if (nCurrentTime - tTrackingState.tStartTime > s_nTouchDelay) {


                SwipeGesture tSwipe;
                tSwipe.vecDirection = tTrackingState.vecStartPoint - IVector2D(tTouch.uX, tTouch.uY);
                tSwipe.nControllerIndex = nControllerIndex;
                arGestures.push_back(tSwipe);
                // Inferred: count update has no row of its own (folded into the return value).
                ++nDetectedGestureCount;
            }
        }
        else {

            tTrackingState.tStartTime = 0;
        }


        return nDetectedGestureCount;
    }



    const float HORIZONTAL_SWIPE_THRESHOLD = 400.0f;


    namespace Gestures {

        BOOL IsSwipeLeft(
            const SwipeGesture & tGesture)
        {
            return tGesture.vecDirection.x <= -HORIZONTAL_SWIPE_THRESHOLD;
        }



        BOOL IsSwipeRight(
            const SwipeGesture & tGesture)
        {
            return tGesture.vecDirection.x >= HORIZONTAL_SWIPE_THRESHOLD;
        }















    } // namespace Gestures
} // namespace Console
