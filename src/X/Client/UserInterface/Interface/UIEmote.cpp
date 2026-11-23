// Original file: X/Client/UserInterface/Interface/UIEmote.cpp
#include "../../CGame.h"
#include "../../RActors/RActors.h"
#include "../Controls/UIHotKeyControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Powers/PowersCommon.h"

// Declared here because their headers are not adopted yet.
RActorID LocalPlayerRActorGet();  // X/Client/CGame.cpp
void CPlayerInputClickWithPower(const RActor * ptRActor, int32 nMouseButton, SNO snoPower, BOOL bForceStand);  // X/Client/CPlayerInput.cpp:3080
UIHotkey & UI_GetHotkeyFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp:5330
BOOL PowerIsEmoteCoolDownDone(const ActorCommonData & tACD);  // X/Common/Powers/PowersCommon.cpp:4506










static void sEmoteOnClick(const UIID & id, GlobalSNO eGlobalSNOPower)
{
    if (GameIsPaused())
        return;

    RActor * ptPlayerRActor = RActorTryToGet(LocalPlayerRActorGet());
    if (ptPlayerRActor == NULL)
        return;

    if (!UI_TestID(id))
        return;

    SNO snoPower = GlobalSNOGet(eGlobalSNOPower);
    if (snoPower == SNO_NONE)
        return;

    ActorCommonData * ptACD = ACDTryToGet(ptPlayerRActor->idACD);
    if (ptACD == NULL)
        return;

    if (!PowerIsEmoteCoolDownDone(*ptACD))
        return;

    CPlayerInputClickWithPower(ptPlayerRActor, UI_GetHotkeyFromID(id).GetUIAction(), snoPower, TRUE);
}


















void UIEmote_OnClick_Follow(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_FOLLOW);
}

void UIEmote_OnClick_Give(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_GIVE);
}

void UIEmote_OnClick_Thanks(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_THANKS);
}

void UIEmote_OnClick_Sorry(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_SORRY);
}

void UIEmote_OnClick_Bye(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_BYE);
}

void UIEmote_OnClick_Die(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_DIE);
}

void UIEmote_OnClick_Run(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_RUN);
}

void UIEmote_OnClick_Wait(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_WAIT);
}

void UIEmote_OnClick_Go(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_GO);
}

void UIEmote_OnClick_Help(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_HELP);
}

void UIEmote_OnClick_Yes(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_YES);
}

void UIEmote_OnClick_No(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_NO);
}

void UIEmote_OnClick_Stay(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_STAY);
}

void UIEmote_OnClick_Attack(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_ATTACK);
}

void UIEmote_OnClick_Retreat(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_RETREAT);
}

void UIEmote_OnClick_Hold(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_HOLD);
}

void UIEmote_OnClick_TakeObjective(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_TAKEOBJECTIVE);
}

void UIEmote_OnClick_Laugh(const UIID & id)
{
    sEmoteOnClick(id, GLOBALSNO_POWERS_EMOTE_LAUGH);
}
