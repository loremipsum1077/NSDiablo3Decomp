#include "ConsoleScreenManager.h"
#include "../../Console/TouchGestureTracker.h"
#include "../../../Common/Game.h"
#include "../../../Common/Player.h"

namespace UIConversation {
BOOL ACDIsInConversation(ACDID idACD, BOOL bConvDialogOnly, BOOL bUnusableOnly);
}

namespace UIQuests {
void MakeCurrentObjectivesReappear();
}

namespace Console {
namespace Gestures {
BOOL IsSwipeLeft(const SwipeGesture & tGesture);
BOOL IsSwipeRight(const SwipeGesture & tGesture);
}

namespace Touch {
struct TouchGlobals {
    TouchGestureTracker tTouchGestureTracker;
};

static TouchGlobals stTouchGlobals;




void Update()
{
    if (GameIsStartUp())
        return;

    if (Screens::IsScreenSetOpen())
        return;

    GestureArray arGestures;
    int32 nGestureCount = stTouchGlobals.tTouchGestureTracker.ProcessInput(arGestures);
    for (int32 i = 0; i < nGestureCount; ++i) {
        Player * ptPlayer = PlayerGetByControllerIndex(arGestures[i].nControllerIndex);
        if (ptPlayer) {
            if (UIConversation::ACDIsInConversation(ptPlayer->idPlayerACD, TRUE, FALSE))
                break;

            const SwipeGesture & tSwipe = arGestures[i];
            if (Gestures::IsSwipeRight(tSwipe) || Gestures::IsSwipeLeft(tSwipe))
                UIQuests::MakeCurrentObjectivesReappear();
        }
    }
}
}  // namespace Touch
}  // namespace Console
