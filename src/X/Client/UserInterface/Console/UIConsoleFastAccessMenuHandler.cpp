#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UIID.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../Console/ControllerUtils.h"
#include "../../CPlayer.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Player.h"
#include "ConsoleScreenManager.h"
#include "UIConsoleFastEquip.h"
#include "UIConsoleMultiplayerDeathMenu.h"

namespace UIFastAccessMenuHandler {
namespace Console {

BOOL CanOpenFastEquip(const int32 nControllerIndex)
{
    int32 nPlayerIndex = PlayerLocalIndexToPlayerIndex(nControllerIndex);

    if (nPlayerIndex == -1)
    {
        return FALSE;
    }

    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        return FALSE;
    }

    if (::Console::Screens::IsScreenSetOpen())
    {
        return FALSE;
    }

    Player * ptPlayer = PlayerGetByPlayerIndex(nPlayerIndex);
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    if (!ptACDPlayer->IsAlive())
    {
        return FALSE;
    }

    return !ACDIsParticipatingInSetDungeon(*ptACDPlayer);
}

void OnFastEquipHotkey(const UIID & id, const int32 nControllerIndex)
{
    if (CanOpenFastEquip(nControllerIndex))
    {
        UIFastEquip::Console::OpenFastEquip(nControllerIndex);
    }
}

void OnUp(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnUp(id, nControllerIndex);
    }
}

void OnDown(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnDown(id, nControllerIndex);
    }
}

void OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnButton1(id, nControllerIndex);
    }
}

void OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnButton2(id, nControllerIndex);
    }
    else if (UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        UIMultiplayerDeathMenu::Console::OnButton2(nControllerIndex);
    }
}

void OnButton3(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnButton3(id, nControllerIndex);
    }
    else if (UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        UIMultiplayerDeathMenu::Console::OnButton3(nControllerIndex);
    }
}

void OnButton4(const UIID & id, const int32 nControllerIndex)
{
    if (UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnButton4(id, nControllerIndex);
    }
    else if (UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        UIMultiplayerDeathMenu::Console::OnButton4(nControllerIndex);
    }
}

void OnBack(const UIID & id, const int32 nControllerIndex)
{
    if (UIMultiplayerDeathMenu::Console::IsActiveForController(nControllerIndex))
    {
        UIMultiplayerDeathMenu::Console::OnBackButton(nControllerIndex);
    }
}

void OnLStick(const UIID & id, const int32 nControllerIndex)
{
    BOOL bInDeadZone = ::Console::ControllerUtils::LStickIsInDeadZone(nControllerIndex, 0);

    if (!bInDeadZone && UIFastEquip::Console::IsActiveForController(nControllerIndex))
    {
        UIFastEquip::Console::OnLStick(nControllerIndex);
    }
}

}  // namespace Console
}  // namespace UIFastAccessMenuHandler
