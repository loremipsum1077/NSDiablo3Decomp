// `X/Client/UserInterface/Console/UIConsoleRepair.cpp` -- the console repair
// pane.  Four free functions in namespace `UIRepair::Console` (no class, no
// vtable): two "ask the server to repair" wrappers, the cost query and the
// label refresh that prices both repair buttons.
//
// Line comments are the 2.6.2 source lines the DWARF and the line-attributed
// listing record (build/dwarf _ZN8UIRepair7Console..., and the `lines` evidence
// for 0x71003ce2e0 / 0x71003ce300 / 0x71003ce340 / 0x71003ce430).
#include "../Controls/UITextControl.h"
#include "../UIID.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Player.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Sigma/RefString.h"

// Declared here because their headers are not adopted in src/; each comment names
// the file that defines the function or object (the sibling console files name
// their callees the same way).
// X/Client/UserInterface/UI.cpp:5348
UIText & UI_GetTextFromID(const UIID & id);
// X/Client/Crafting/CItemCrafting.cpp:45 and :81
void CItemCrafting_TryRepairAll(Player * ptPlayer, const BOOL fEquippedOnly);
void CItemCrafting_TryRepair(ACDID idACDItem, Player * ptPlayer);
// X/Common/Crafting/ItemCrafting.cpp:1582
int32 ItemCrafting_GetRepairAllCost(const Player * ptPlayer, const BOOL fEquippedOnly);
// X/Common/Items.cpp:10408 (`CurrencyType` is the actor common data typedef)
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);
// X/Client/UserInterface/UIGlobals.cpp:3113 (GOT 0x71010e81a8 -> 0x71017d6728)
extern UIID UIDLG_INVENTORY_CONSOLE;

namespace UIRepair {
namespace Console {

// :38  (defined at line 35)
void RepairItem(Player * ptPlayer, const ACDID idItem)
{
	if (idItem == ACDID_INVALID)
	{
		return;
	}

	CItemCrafting_TryRepair(idItem, ptPlayer);
}

// :54  (defined at line 51)
void RepairAllItems(Player * ptPlayer, const BOOL bEquippedItemsOnly)
{
	int32 nRepairCost;

	nRepairCost = ItemCrafting_GetRepairAllCost(ptPlayer, bEquippedItemsOnly);

	if (nRepairCost == 0)
	{
		return;
	}

	CItemCrafting_TryRepairAll(ptPlayer, bEquippedItemsOnly);
}

// :76  (defined at line 71)
//
// The tail is the player ACD lookup inlined from
// `ActorCommonData * ACDTryToGet(const ACDID)` (X/Common/ActorCommonData.h:1445,
// inlined at F0:80) through
// `TChainedDataArray<ActorCommonData, ACDID>::ElementTryToGet` /
// `...::ElementGetFromBaseIndex` (ChainedDataArrayTemplated.h:432/555) and
// `XGrowableArray`/`XBaseArray` (F4:444..F5:70).  Those header bodies are
// pulled in through ActorCommonData.h, so the `and w8, w10, w8` at F4:562
// comes from the header, not from this file.
void GetRepairAllCostInfo(const Player * ptPlayer, const BOOL bEquippedOnly, int32 & nRepairCost,
                          BOOL & bCanAfford)
{
	if (ptPlayer != NULL)
	{
		nRepairCost = ItemCrafting_GetRepairAllCost(ptPlayer, bEquippedOnly);

		// read from the player's ACD id and looked up inline (F0:80, ActorCommonData.h:1445);
		// the DWARF scopes ptACDPlayer in a nested lexical block
		ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

		if (ptACDPlayer != NULL && !ptACDPlayer->CanAffordCurrencyCost(nRepairCost, 0))
		{
			nRepairCost = (int32)ptACDPlayer->GetCurrencyAmount(0);
			bCanAfford = TRUE;
		}
	}
}

// :97
//
// The two `CRefString`s are built in place at their first use (no copy
// assignment at line 105/108; the assignment operator only appears at 120/123),
// and `idRepairCost` is a copy-construction from `GetUIIDNone()` --
// `UIID::UIID(const UIID&)` inlined at line 113 (UIID.h:31).
//
// `UI_GetTextFromID(...).SetText(...)` dispatches the virtual
// `UIText::SetText(LPCSTR, BOOL)` at vtable offset 0x110; `UIText` is complete
// from ../Controls/UITextControl.h and `UI_GetTextFromID` is declared at the top
// of this file (owner X/Client/UserInterface/UI.cpp:5348).
void UpdateRepairUI(Player * ptPlayer)
{
	int32 nCost = 0;
	BOOL bCanAfford = TRUE;

	GetRepairAllCostInfo(ptPlayer, FALSE, nCost, bCanAfford);

	CRefString sCost = UIText_ValueToGameCurrency(nCost, 0);
	Set_Text_Color(sCost, !bCanAfford);

	CRefString sRepairCost = LocalizedSprintf(
	    StringListFetch("ConsoleShop:RepairAllCostFormat").str(), sCost.str(), NULL);

	UIID idRepairCost = GetUIIDNone();
	idRepairCost.SetFromParent(UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Repair.RepairAll");
	UI_GetTextFromID(idRepairCost).SetText(sRepairCost.str(), FALSE);

	GetRepairAllCostInfo(ptPlayer, TRUE, nCost, bCanAfford);

	sCost = UIText_ValueToGameCurrency(nCost, 0);
	Set_Text_Color(sCost, !bCanAfford);

	sRepairCost = LocalizedSprintf(
	    StringListFetch("ConsoleShop:RepairEquippedCostFormat").str(), sCost.str(), NULL);

	idRepairCost.SetFromParent(UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Repair.RepairEquipped");
	UI_GetTextFromID(idRepairCost).SetText(sRepairCost.str(), FALSE);
}

}  // namespace Console
}  // namespace UIRepair
