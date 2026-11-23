// X/Client/UserInterface/Interface/UIDurabilityIndicator.cpp -- the HUD's armour-damage
// figure. `UIDurabilityIndicator` is a namespace, not a class: the one symbol has the ACD in
// x0 and no `this`. The file-static that computes the state is inlined whole into its only
// caller and carries no symbol.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
// X/Common/ActorInventory.cpp:2403
ACDID ACDInventoryGetSquareItem(const ActorCommonData & tACD, const InventorySlotType eSlot,
                                const IVector2D & pt);
// X/Common/Items.cpp:8921 and :8952
float ItemGetDurabilityPercent(const ActorCommonData & tACDItem);
extern const InventorySlotType cgtInventorySlotsThatTakeDamage[14];
// X/Client/UserInterface/UI.cpp:5339
UIWindow & UI_GetWindowFromID(const UIID & id);
// X/Common/GameTemplate.cpp:255
BOOL GameTypeIsPvP();
// X/Client/UserInterface/Interface/UITutorials.h:17 and :34
class UITutorial;
#include "UITutorials.h"





extern UITutorialManager gtTutorialMgr;

// UIDurabilityIndicator.cpp:24
enum DurabilityState {
    DURABILITY_OK = 0,
    DURABILITY_WARNING = 1,
    DURABILITY_DESTROYED = 2,
};

// UIDurabilityIndicator.cpp:33 -- the worst durability across the slots that take damage,
// bucketed against the authored warning threshold. PvP suppresses the indicator outright.
static DurabilityState sGetDurabilityStateForPlayer(const ActorCommonData & tACDPlayer)
{
    if (GameTypeIsPvP())
        return DURABILITY_OK;

    float flMinDurPercent = 1.0f;

    for (int32 nIndex = 0; cgtInventorySlotsThatTakeDamage[nIndex] != -1; nIndex++)
    {
        const InventorySlotType eSlot = cgtInventorySlotsThatTakeDamage[nIndex];

        const ACDID idACDItem = ACDInventoryGetSquareItem(tACDPlayer, eSlot, IVector2D(0, 0));
        ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);

        if (ptACDItem != NULL && ptACDItem->IsItem())
        {
            flMinDurPercent = Math::Min(flMinDurPercent, ItemGetDurabilityPercent(*ptACDItem));
        }
    }

    ScopedGlobalDefinitionLockR pdefGlobals;

    if (flMinDurPercent > pdefGlobals->flItemsDamagedThresh)
        return DURABILITY_OK;

    return (flMinDurPercent == 0.0f) ? DURABILITY_DESTROYED : DURABILITY_WARNING;
}

namespace UIDurabilityIndicator {

// UIDurabilityIndicator.cpp:76
void UpdateDurabilityIndicator(const ActorCommonData & tACDPlayer, const UIID & idIndicator)
{
    const DurabilityState eState = sGetDurabilityStateForPlayer(tACDPlayer);

    UIWindow & tIndicator = UI_GetWindowFromID(idIndicator);
    tIndicator.SetActive(eState != DURABILITY_OK);

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer != NULL)
    {
        switch (eState)
        {
        case DURABILITY_WARNING:
            tIndicator.SetTextureFrame("UI:DurabilityFigure_Damaged");
            gtTutorialMgr.ShowTutorial(idIndicator, GlobalSNOGet(GLOBALSNO_TUTORIAL_DURABILITY_WARNING),
                                       ptPlayer, TRUE);
            break;

        case DURABILITY_DESTROYED:
            tIndicator.SetTextureFrame("UI:DurabilityFigure_Broken");
            gtTutorialMgr.ShowTutorial(idIndicator, GlobalSNOGet(GLOBALSNO_TUTORIAL_DURABILITY_DESTROYED),
                                       ptPlayer, TRUE);
            break;

        default:
            break;
        }
    }
}

}  // namespace UIDurabilityIndicator
