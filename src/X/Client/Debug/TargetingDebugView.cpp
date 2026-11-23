// Original file: X/Client/Debug/TargetingDebugView.cpp

#include "../CGame.h"
#include "../CPlayerInput.h"
#include "../Font/Font.h"
#include "../Font/FormattedText.h"
#include "../Input/Input.h"
#include "../RActors/RActors.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/Player.h"
#include <string.h>

Player * LocalPlayerGet();  // X/Client/CPlayer.cpp:1612 (X/Client/CPlayer.h not adopted yet)
const CHAR * MarkerNameGetString(uint32 dwHash);  // X/Common/Marker/Marker.cpp:176
namespace Controller {
int32 GetSlotFromButton(InputButton eInputButton, const Player * ptPlayer);  // X/Client/Definitions/ControllerSchemeDefinition.cpp:434 (ControllerSchemeDefinition.h not adopted yet)
}
namespace Targeting { namespace Console {
void ResolveTargetForPlayer(const Player * ptPlayer, const SNO snoPowerToTry, ResolvedClick & tResolvedClick, BOOL bCheckOncePerClick, BOOL bCheckForHostiles, BOOL bCheckForOperatableGizmos, BOOL bAllowSecondaryTargets, BOOL bUseTargetLockPriorities);  // X/Client/Targeting/ConsoleTargeting.cpp:778
} }

namespace {
struct DebugGlobals {  // size 0x1
    bool bShowTargetName;  // 0x00

    DebugGlobals() : bShowTargetName(false) {}
};
static DebugGlobals tDebugGlobals;
}  // namespace

namespace Console {  // DWARF CU 0x00582b38 DIE 0x005af0c7 (line 33, low_pc=0, high_pc=0x14): dropped by linker GC; required so GlobalOpt does not fold tDebugGlobals.bShowTargetName to false.
void ToggleTargetName() {
    tDebugGlobals.bShowTargetName = !tDebugGlobals.bShowTargetName;
}

void UpdateTargetDebugging() {
    if (tDebugGlobals.bShowTargetName) {
        Rect2D rectViewportUIC;
        CGameGetSafeViewportUIC(rectViewportUIC);
        Vector2D vecUIC(rectViewportUIC.right * 0.5f, rectViewportUIC.top + 128.0f);
        TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SANS_SERIF), 13, 1.0f, 1.0f);

        CRefString strBlockingReads;
        const ActorCommonData * ptACDTarget = NULL;

        const Player * ptPlayer = LocalPlayerGet();

        int32 nSlot = Controller::GetSlotFromButton(0, ptPlayer);
        if (nSlot == -1)
        {
            return;
        }
        SNO snoPower = ptPlayer->tHeroStateData.tSavedData.tActiveSkills[nSlot].snoSkill;
        ResolvedClick tResolvedClick;
        Targeting::Console::ResolveTargetForPlayer(ptPlayer, snoPower, tResolvedClick, FALSE, TRUE, TRUE, TRUE, FALSE);
        const RActor * ptRActor = RActorTryToGet(tResolvedClick.idRActor);
        if (ptRActor)
            ptACDTarget = ACDTryToGet(ptRActor->idACD);

        if (ptACDTarget) {
            vecUIC.x -= (float)(strlen(MarkerNameGetString(ptACDTarget->tName)) / 2);
        } else {
            vecUIC.x -= 2.0f;
        }
        strBlockingReads.Format("Current Target: - %s", ptACDTarget ? MarkerNameGetString(ptACDTarget->tName) : "NONE");
        FontStringDrawUIC(tProps, FormattedText(strBlockingReads.str(), PARSE_DEFAULT, FALSE), vecUIC, crgbaWhite, &crgbaBlack, 0.0f, TRUE, NULL, RENDERLAYER_UI_OVERLAY, FALSE);
    }
}
}  // namespace Console
