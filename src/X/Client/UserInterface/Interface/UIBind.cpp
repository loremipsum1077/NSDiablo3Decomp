// Original file: X/Client/UserInterface/Interface/UIBind.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIIDs.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Player.h"
// Declared here because its header is not adopted yet (CActorInventory.cpp:484).
void CACDInventoryRequestMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLocDest, BOOL bIgnoreSoulbound, Player * ptPlayer);

namespace {








ACDID g_idACDItem = ACDID_INVALID;
InventoryLocation g_tInvLocDest;
}




void UIBind_Confirm(const UIID &)
{
    ActorCommonData * ptACD = ACDTryToGet(g_idACDItem);
    if (ptACD)
    {
        CACDInventoryRequestMove(*ptACD, g_tInvLocDest, TRUE, NULL);
    }





    g_idACDItem = ACDID_INVALID;
    UI_GetWindowFromID(UIDLG_BIND_CONFIRM).SetActive(FALSE);
}

void UIBind_Cancel(const UIID &)
{

    g_idACDItem = ACDID_INVALID;
    UI_GetWindowFromID(UIDLG_BIND_CONFIRM).SetActive(FALSE);
}

// Present in the DWARF (line 54, 0x48 bytes) but removed by the linker; body inferred.

void UIBind_Open(const ACDID idACDItem, const InventoryLocation & tInvLocDest)
{
    g_idACDItem = idACDItem;
    g_tInvLocDest = tInvLocDest;
    UI_GetWindowFromID(UIDLG_BIND_CONFIRM).SetActive(TRUE);
}
