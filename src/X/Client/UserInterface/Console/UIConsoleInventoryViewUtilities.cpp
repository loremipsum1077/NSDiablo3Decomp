// Original file: X/Client/UserInterface/Console/UIConsoleInventoryViewUtilities.cpp
#include "../Controls/UIBlinkerControl.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "UIConsoleInventoryViewUtilities.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryView.h"
#include "../UI.h"
#include "../UIID.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UITextControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Game.h"
#include "../../../Common/Items.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/Player.h"
#include "../UIStoryboard.h"
#include "ConsoleScreenManager.h"
#include "../Controls/UIRActorControl.h"
#include "ConsoleUIRActorManager.h"
#include "../../CGame.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../SubObjectGfx.h"
#include "../Controls/UIControlGroup.h"
#include "../UITypes.h"
#include "../Controls/UIItemButtonControl.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/ActorInventory.h"
#include "../../../Common/HotbarButton.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../RActors/RActors.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Common/Crafting/ItemCrafting.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/Definitions/GlobalDefinition.h"
#include "../../../Common/Shell/CmdLine.h"
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);  // X/Client/UserInterface/UI.h
BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem);  // X/Common/Items.h
UIRActor & UI_GetRActorFromID(const UIID & id);  // X/Client/UserInterface/UI.h
#include "../Interface/UITutorials.h"
SNO UI_GetDefaultHeroLight();  // X/Client/UserInterface/UI.h
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);  // X/Client/Render/Light.h
RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tEquipment, VisualInventoryType eVIT, PlayerClass ePlayerClass, BOOL fFemale, const PRTransform & tTransform, RenderContext eContext, uint32 dwAnimTag, float flScale, BOOL bUnknown);  // X/Client/UserInterface/UI.h
void UI_UpdateRActorForInventory(RActor * ptRActor, VisualInventory & tVisualInventory, SNO snoIrradianceTex, VisualInventoryType eVIT, PlayerClass ePlayerClass, BOOL fFemale, uint32 dwAnimTag, BOOL bUnknown);  // X/Client/UserInterface/UI.h
namespace UIInventory { void UpdateGoldAmount(const UIID & idText, const ACDID idPlayerACD); }  // X/Client/UserInterface/Interface/UIInventoryCommon.cpp
namespace UIUtils {
namespace Console {
void UpdateHeroNameAndLevel(const UIID & idParent, const Player & tPlayer);  // X/Client/UserInterface/Console/UIConsoleUtils.cpp
void UpdateHeroStats(const UIID & idParent, const Player & tPlayer);
void UpdatePlayerIndicator(const Player & tPlayer);
SNO GetObserverForPlayerClass(PlayerClass ePlayerClass);
}  // namespace Console
}  // namespace UIUtils
extern const UIID UIRACTOR_CONSOLE_RING;  // X/Client/UserInterface/UIIDs.h
void ACDInventoryGetItemDrawInfoWithGBID(const ACDID idACDPlayer, const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType);  // X/Client/ACDClientInventory.cpp
UIItemOverlay ItemGetOverlay(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer);  // X/Client/UserInterface/UIItems.h
CRefString UIText_ValueToGameCurrency(const uint64 uAmount, CurrencyType eCurrencyType);  // X/Client/UserInterface/UIText.h
int32 ItemGetMaxDurability(const ActorCommonData & tACDItem);  // X/Common/Items.h
int32 ItemGetCurrentDurability(const ActorCommonData & tACDItem);  // X/Common/Items.h
BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // X/Common/Items.h
BOOL ItemIsDamaged(const ActorCommonData & tACDItem);  // X/Common/Items.h
EInvItemClass GetInventoryItemClass(const GBID gbidItem);  // X/Common/ActorInventory.h
UIItemButton & UI_GetItemButtonFromID(const UIID & id);  // X/Client/UserInterface/UI.h
typedef int32 ItemDrawType;  // X/Common/ActorInventory.h
void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType, const PlayerClass eClass, const BOOL fIsMale);  // X/Client/ACDClientInventory.cpp
namespace UITransmogrify { void UpdateTransmogLegendaryBackground(const UIID & idBackground, GBID gbidItem); }  // X/Client/UserInterface/Interface/UITransmogrifyCommon.h
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);  // X/Common/Player.h
SNO GetFrontendCharacterSNO(PlayerClass eClass, BOOL bIsFemale);  // X/Client/UserInterface/Console/UIConsoleCharacterSelect.h
void CHirelingUpdateAppearance(const ActorCommonData & tACDHireling, RActor * ptRActor, BOOL bForce);  // X/Client/CHireling.h
int32 ItemCrafting_GetRepairCost(ACDID idACDItem, ACDID idACDPlayer);  // X/Common/Crafting/ItemCrafting.h
extern UIID UIDLG_INVENTORY_CONSOLE, UIDLG_SKILLS_CONSOLE, UIWND_PARAGON_CONSOLE, UIDLG_LORE_CONSOLE; extern const UIID UIWND_INVENTORY_CONSOLE_ROOT;  // X/Client/UserInterface/UIIDs.h
#include "../../../Common/Hireling.h"
namespace UIInventory { uint32 TryIdentifyWithCast(ACDID idACDItem, const ActorCommonData & tACDPlayer, const Player * ptPlayer); }  // X/Client/UserInterface/UIInventory.h
// DWARF Player.cpp and console UI owner declarations; full signatures retained from CU00f32f51.
namespace Console {
namespace Screens {
// X/Client/UserInterface/Console/ConsoleScreenManager.h (GameplayScreens)
enum {
    INVENTORY_SCREEN = 0,
    SKILLS_SCREEN = 1,
    PARAGON_SCREEN = 2,
    FOLLOWER_SCREEN = 3,
    PARTY_SCREEN = 4,
    JOURNAL_SCREEN = 5,
};
}  // namespace Screens
}  // namespace Console
BOOL PlayerCanOpenInventory(const ActorCommonData & tACDPlayer); BOOL PlayerCanOpenQuests(); BOOL PlayerCanChangeParagon(const ActorCommonData & tACDPlayer);
namespace {
struct ItemSocketCallbackData {
    UIID idContainer;
    ACDID idACDPlayer;
};
BOOL sUpdateItemSocketUI(const InventoryLocation & tInvLoc, void * pCallbackData);
BOOL sResolveSocketTag(const ActorCommonData * ptACDItem, AttribGroupID, AttribGroupID, LPCSTR szTag, float & flResult, float *, CRefString &, SNO, int32, void * pData, void *, FontSpecifier &);
}  // namespace
// DWARF UI.h / UIIDs.h / UITutorials.cpp:41 and console owners: external functions and globals.
// GBIDToString, ArmorTotal/DPSTotal descriptions, ItemGetCosmeticFromItem: X/Common/Definitions/GameBalanceDefinition.h, X/Common/Items.h; GetComparisonValueStrings: X/Client/UserInterface/Interface/UIInventory.h; gGemTypeNames: X/Common/ItemsSwig.h; GB_ITEMTYPES: GameBalanceDefinition.h; CRAFTER_JEWELER: X/Common/Crafting/Crafting.h
CRefString GBIDToString(const GBHandle & hGB, const BOOL fUsedForXMLFields); enum { GB_ITEMTYPES = 1 }; namespace UIPortraits { ImageTextureFrame GetPortraitFrameImage(GBID gbidPortrait); } extern const SymbolicEntry gGemTypeNames[]; enum { CRAFTER_JEWELER = 1 };
GBID ItemGetCosmeticFromItem(const ActorCommonData & tACDItem); BOOL ItemIsSetItem(const GBID gbid); BOOL ItemIsDye(const ActorCommonData & tACDItem); void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const ItemDrawType eDrawType); BOOL ItemIsLegendary(GBID gbidItem); AncientRank ItemGetAncientRank(const ActorCommonData & tACDItem); CRefString ItemGetArmorTotalDescription(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetDPSTotalDescription(const ActorCommonData & tACD, const PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); namespace UIInventory { void GetComparisonValueStrings(ActorCommonData * ptACDOwner, const ActorCommonData & tACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, CRefString & sValEHP, CRefString & sValDmg, CRefString & sValHealing); }
UIBlinker & UI_GetBlinkerFromID(const UIID & id); extern UITutorialManager gtTutorialMgr; extern UIID UIDLG_FOLLOWER_CONSOLE, UIDLG_PARTY_CONSOLE; namespace UISkills { namespace Console { BOOL HasNewSkillBlinker(const Player * ptPlayer); } } namespace UIFollower { namespace Console { BOOL HasNewSkillBlinker(); } } namespace UIParagonBonuses { namespace Console { BOOL HasLeveledSinceLastVisit(const Player * ptPlayer); } }
namespace UIInventory {
namespace Console {

CRefString sGetGemEffectsString(const ActorCommonData & tACDGem, const ACDID idACDItem); void GetSocketedEffects(const ActorCommonData & tACDSocketedItem, SocketEffectInfo * arEffects); ACDID sGetEquippedItemBySlotType(const Player * ptPlayer, const UIFilterType eFilterType, const BOOL fFallbackTo2HMainHand);
ACDID sGetEquippedWeaponByCraftingType(const Player * ptPlayer, const UIFilterType eFilterType); ACDID sGetEquippedArmorByCraftingType(const Player * ptPlayer, const UIFilterType eFilterType); void UpdateNavBarTabState(const Player * ptPlayer, const UIID & idRoot, LPCSTR szNavigationContainerID, BOOL bUpdatePartyFollowerTab);
void SetUpItemButton(const UIID & idItemUI, const ACDID idItem, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bShowOverlays); void GetRepairCostInfo(const ACDID idItem, const ACDID idPlayer, int32 & nRepairCost, BOOL & bCanAfford); void UpdateRequiredLevel(const UIID & idParent, const ActorCommonData & tACDItem, const ACDID idACDPlayer);
void ShowItemPrimaryStats(const UIID & idParent, const ActorCommonData * ptACDItem, const ACDID idACDPlayer); CRefString FormatExpirationTimeFromNow(Blizzard::Time::Timestamp nExpiration); void UpdateRepairCost(const UIID & idParent, const ACDID idPlayer, const ActorCommonData & tACDItem); void UpdateComparisonInfo(const UIID & idParent, const ActorCommonData * ptACDItem, const ACDID idACDOwner, const BOOL bUseAltSlot); inline void ShowRepairCost(const UIID & idParent, const UIFilterType eFilterType, const BOOL bIsDetailedView); void ShowComparisonInfo(const UIID & idParent, const ActorCommonData & tACDItem, const UIFilterType eFilterType, const Blizzard::Time::Timestamp nExpiration, const BOOL bIsDetailedView); void ShowItemStatusIndicators(const UIID & idParent, const ACDID idPlayer, const ActorCommonData & tACDItem, const UIFilterType eFilterType, const Blizzard::Time::Timestamp nExpiration, const BOOL bIsDetailedView); CRefString GetItemDescription(const ACDID idACDItem, const ACDID idACDOwner, const PlayerClass ePlayerClass, const BOOL bShowStatRanges);
inline void ShowOnly(const int32 nIndexToShow, const UIID & idParent, const LPCSTR * arszIDNames, const int32 nIDListLength);




void ShowDetailsMode(const UIID & idParent, const DetailsViewMode eViewMode)
{
    static const LPCSTR arszModeIDNames[] =
    {
        "ItemDetails",
        "GemCombining",
        "ItemSocket",
        "GemRemoval",
        "ItemCategory",
        "Train",
        "Salvage",
        "ItemEnchant",
        "ItemTransmog",
        "ItemDye",
    };


    UIID idPanelParent = UIIDCreateFromParent(idParent, ".LayoutRoot");
    ShowOnly(eViewMode, idPanelParent, arszModeIDNames, 10);
}







void SetUpItemButton(const UIID & idItemUI, const ACDID idItem, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bShowOverlays)
{





    UIItemButton & tItemButton = UI_GetItemButtonFromID(idItemUI);
    tItemButton.SetACDID(idItem);
    tItemButton.SetActive(idItem != ACDID_INVALID);

    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItem != NULL)
    {

        if (ptACDItem->hGB.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
        {
            tItemButton.SetQualityLevel(IQL_NORMAL);
        }

        ImageTextureFrame tImageTexFrame;

        ACDInventoryGetItemDrawInfoWithGBID(idACDPlayer, ptACDItem->hGB.gbid, tImageTexFrame, 0);
        tItemButton.SetAllTextureFrames(tImageTexFrame);

        if (idACDOwner != ACDID_INVALID && bShowOverlays)
        {
            ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
            if (ptACDOwner != NULL)
            {
                tItemButton.SetOverlay(ItemGetOverlay(*ptACDItem, *ptACDOwner));
            }
        }
    }
}




void UpdateViewItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const UIFilterType eFilterType, const BOOL bIsSelected, const BOOL bIsDetailedView, const BOOL bIsNew, const BOOL bIdentifyAllowed, const Blizzard::Time::Timestamp nExpiration)
{










    BOOL bItemIsIdentified = !ItemIsUnidentified(tACDItem);

    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");

    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");
    SetUpItemButton(idIcon, tACDItem.id, idACDPlayer, idACDOwner, TRUE);


    CRefString sName;
    if (bItemIsIdentified)
    {
        sName = ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE);

        if (ItemIsUpgradeableJewel(tACDItem))
        {
            int32 nRank = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_JEWEL_RANK));
            sName = LocalizedSprintf(StringListFetch("ConsoleInventory:UpgradeableGemNameFormat").str(),
                                     sName.str(),
                                     UnicodeText::FormatNumber(nRank, TRUE, FNS_NORMAL, FALSE).str(), NULL);
        }
    }
    else
    {

        sName = StringListFetch("ConsoleInventory:UnidentifiedItemName");
        if (ItemIsSetItem(tACDItem.hGB.gbid))
        {
            Set_Text_Color(sName, 3);
        }
        else
        {
            Set_Text_Color(sName, 15);
        }

        if (bIsSelected && bIdentifyAllowed)
        {
            sName += CRefString("\n") + StringListFetch("ConsoleInventory:UnidentifiedInstructions");
        }
    }

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");
    UI_GetTextFromID(idName).SetText(sName.str(), FALSE);

    ShowItemStatusIndicators(idRoot, idACDPlayer, tACDItem, eFilterType, nExpiration, bIsDetailedView);

    ShowRepairCost(idRoot, eFilterType, bIsDetailedView);
    UpdateRepairCost(idRoot, idACDPlayer, tACDItem);

    ShowComparisonInfo(idRoot, tACDItem, eFilterType, nExpiration, bIsDetailedView);

    BOOL bUseAltSlot = FilterUsesAltSlotForComparisons(eFilterType);
    UpdateComparisonInfo(idRoot, &tACDItem, idACDOwner, bUseAltSlot);

    ShowItemPrimaryStats(idRoot, &tACDItem, idACDOwner);


    UIID idProgress;
    idProgress.SetFromParent(idRoot, "IdentifyProgress");
    UIProgressBar & tProgressBar = UI_GetProgressBarFromID(idProgress);
    tProgressBar.SetActive(!bItemIsIdentified);


    BOOL bIsElite = bIsNew && (ItemIsLegendary(tACDItem.hGB.gbid) || ItemIsSetItem(tACDItem.hGB.gbid));
    UIID idNewIndicator = UIIDCreateFromParent(idRoot, ".ItemNew");
    UIID idNewEliteIndicator = UIIDCreateFromParent(idRoot, ".ItemNewElite");
    UI_GetWindowFromID(idNewIndicator).SetActive(bIsNew && !bIsElite);
    UI_GetWindowFromID(idNewEliteIndicator).SetActive(bIsElite);


    BOOL bBroken = FALSE;
    BOOL bDamaged = FALSE;

    if (eFilterType == FILTER_DAMAGED)
    {
        float flDurabilityPercent = ItemGetDurabilityPercent(tACDItem);

        ScopedGlobalDefinitionLockR pdefGlobals;
        bBroken = flDurabilityPercent == 0.0f;
        bDamaged = !bBroken && flDurabilityPercent <= pdefGlobals->flItemsDamagedThresh;
    }


    UIID idDamaged, idBroken;
    idDamaged.SetFromParent(idRoot, "DurabilityDamaged");
    idBroken.SetFromParent(idRoot, "DurabilityBroken");

    UI_GetWindowFromID(idDamaged).SetActive(bDamaged);
    UI_GetWindowFromID(idBroken).SetActive(bBroken);


    GBID gbidTransmog = tACDItem.AttributesGetInt(FastAttribKey(ATTRIBUTE_TRANSMOG_GBID));
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemTransmog")).SetActive(gbidTransmog != GBID_NONE);


    BOOL bIsEnchanted = ItemHasEnchantedAffix(tACDItem);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemEnchanted")).SetActive(bIsEnchanted);


    UIItemButton & tDyeButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemDyed"));
    if (ItemIsDye(tACDItem))
    {
        tDyeButton.SetActive(FALSE);
    }
    else
    {
        DyeType eDye = tACDItem.GetDyeType();
        if (eDye == DYE_TYPE_NONE)
        {
            tDyeButton.SetActive(FALSE);
        }
        else
        {
            GBID gbidDye = ItemGetGBIDForDyeType(eDye);
            if (gbidDye != GBID_NONE)
            {
                ImageTextureFrame tImageTexFrame;
                ACDInventoryGetItemDrawInfoWithGBID(gbidDye, tImageTexFrame, 0);

                tDyeButton.SetGBID(gbidDye);
                tDyeButton.SetQualityLevel(-1);
                tDyeButton.SetAllTextureFrames(tImageTexFrame);

                tDyeButton.SetActive(TRUE);
            }
        }
    }

    AncientRank eAncientRank = ItemGetAncientRank(tACDItem);
    BOOL bIsIdentified = !ItemIsUnidentified(tACDItem);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".Ancient")).SetActive(eAncientRank == 1 && bIsIdentified);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".PrimalAncient")).SetActive(eAncientRank == 2 && bIsIdentified);
}




void UpdateViewItemSelectionUI(const UIID & idItem, const BOOL fShowSelection)
{


    UIID idProgress = UIIDCreateFromParent(idItem, ".LayoutRoot.IdentifyProgress");
    BOOL bIsUnidentified = UI_TestID(idProgress) ? UI_GetProgressBarFromID(idProgress).IsItselfActive() : FALSE;


    UIID idHighlight = UIIDCreateFromParent(idItem, ".LayoutRoot.Selected");
    UI_GetWindowFromID(idHighlight).SetActive(fShowSelection && !bIsUnidentified);

    UIID idIdentifyHighlight = UIIDCreateFromParent(idItem, ".LayoutRoot.Identify");
    if (UI_TestID(idIdentifyHighlight))
    {
        UI_GetWindowFromID(idIdentifyHighlight).SetActive(fShowSelection && bIsUnidentified);
    }

    if (fShowSelection)
    {
        UIID idStoryboard = UIIDCreateFromParent(idItem, ".ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(0, 0.0f);
    }
}










void UpdateViewBlacksmithRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew)
{
    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");

    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");
    if (UI_TestID(idIcon))
    {
        SetUpItemButton(idIcon, tACDItem.id, idACDPlayer, idACDOwner, TRUE);
    }

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");
    if (UI_TestID(idName))
    {
        UI_GetTextFromID(idName).SetText(sItemName.str(), FALSE);
    }

    UpdateRequiredLevel(idRoot, tACDItem, idACDPlayer);

    UIID idNewIndicator = GetUIIDNone();
    idNewIndicator.SetFromParent(idRoot, "ItemNew");
    if (UI_TestID(idNewIndicator))
    {
        UI_GetWindowFromID(idNewIndicator).SetActive(bIsNew);
    }
}














void UpdateViewJewelerRecipeUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const int32 nRequiredLevel, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew)
{







    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");

    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");
    SetUpItemButton(idIcon, tACDItem.id, idACDPlayer, idACDOwner, TRUE);

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");
    UI_GetTextFromID(idName).SetText(sItemName.str(), FALSE);


    UIID idGemTier;
    idGemTier.SetFromParent(idRoot, "GemTier");

    int32 nTier = ItemCrafting_GetGemTier(tACDItem);
    CRefString sText = LocalizedSprintf(StringListFetch("ConsoleCrafting:GemTier").str(),
                                        UnicodeText::FormatNumber(nTier, TRUE, FNS_NORMAL, FALSE).str(), NULL);

    UI_GetTextFromID(idGemTier).SetText(sText.str(), FALSE);


    UIID idBackgrounds;
    idBackgrounds.SetFromParent(idRoot, "GemRecipeBackgrounds");

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    CRefString sGemTypeName = SymbolToName(pdefItem->eGemType, gGemTypeNames);

    UIID idGemType = UIIDCreate(CRefString().Format("%s.GemRecipe%s", idBackgrounds.GetIDAsString().str(), sGemTypeName.str()));
    const UIControlItemArray & aridChildren = UI_GetWindowFromID(idBackgrounds).GetChildList();
    for (uint32 i = 0; i < aridChildren.size(); ++i)
    {

        UI_GetWindowFromID(aridChildren[i].id).SetActive(aridChildren[i].id == idGemType);
    }


    CRefString sLevel;

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL)
    {
        if (GetCraftLevel(CRAFTER_JEWELER, *ptPlayer) < nRequiredLevel)
        {

            sLevel = LocalizedSprintf(StringListFetch("ConsoleArtisan:JewelerReqLevel"),
                                      UnicodeText::FormatNumber(nRequiredLevel, TRUE, FNS_NORMAL, FALSE).str(), NULL);
        }
    }


    UIID idRequiredLevel = UIIDCreateFromParent(idRoot, ".ItemRequiredLevel");
    UI_GetTextFromID(idRequiredLevel).SetText(sLevel, FALSE);


    UIID idNewIndicator = GetUIIDNone();
    idNewIndicator.SetFromParent(idRoot, "ItemNew");
    UI_GetWindowFromID(idNewIndicator).SetActive(bIsNew);
}


void UpdateViewArtisanSelectionUI(const UIID & idItem, const BOOL fShowSelection)
{
    UIID idHighlight = UIIDCreateFromParent(idItem, ".LayoutRoot.Selected");
    UI_GetWindowFromID(idHighlight).SetActive(fShowSelection);

    if (fShowSelection)
    {

        UIID idStoryboard = UIIDCreateFromParent(idItem, ".ItemListHighlight");
        UIStoryboard & tStoryboard = UI_GetStoryboardFromID(idStoryboard);
        tStoryboard.Stop();
        tStoryboard.Play(0, 0.0f);
    }
}






void UpdateViewMysticTransmogUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL)
    {
        return;
    }

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    PlayerClass ePlayerClass = PlayerGetPlayerClass(ptPlayer);
    BOOL bIsMale = PlayerIsMale(*ptPlayer);

    ImageTextureFrame tImageTexFrame;
    ACDInventoryGetItemDrawInfoWithGBID(tACDItem.hGB.gbid, tImageTexFrame, 0, ePlayerClass, bIsMale);
    UI_GetWindowFromID(UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisual")).SetTextureFrame(tImageTexFrame);

    UIID idItemBackground = UIIDCreateFromParent(idItemUI, ".LayoutRoot.ItemVisualLoadBG");
    UITransmogrify::UpdateTransmogLegendaryBackground(idItemBackground, tACDItem.hGB.gbid);
}







inline void SetUpPortraitFrameButton(const UIID & idItemUI, const ActorCommonData & tACDItem)
{


    UIItemButton & tItemButton = UI_GetItemButtonFromID(idItemUI);
    tItemButton.SetACDID(tACDItem.id);
    tItemButton.SetActive(tACDItem.id != ACDID_INVALID);

    tItemButton.SetQualityLevel(1);



    ImageTextureFrame tImageTexFrame = UIPortraits::GetPortraitFrameImage(tACDItem.hGB.gbid);
    tItemButton.SetAllTextureFrames(tImageTexFrame);
}

// NON_MATCHING: 1524 vs 1520: the original jump-threads the paragon-frame path straight to the bEquipped test; ours re-tests bKnown (one extra cbz). Lines 612-627 re-laid on DWARF rows/columns (613:59 call, 615:16 ||, 617:20 &&), no byte change.


void UpdateViewCosmeticItemUI(
    const UIID & idItemUI,
    const ActorCommonData & tACDItem,
    const ACDID idACDPlayer,
    const ACDID idACDOwner,
    const BOOL bIsNew)
{
    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");

    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");

    if (UI_TestID(idIcon))
    {
        CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(tACDItem.hGB.gbid);
        if (eType == COSMETIC_ITEM_FRAME)
        {
            SetUpPortraitFrameButton(idIcon, tACDItem);
        }
        else
        {
            SetUpItemButton(idIcon, tACDItem.id, idACDPlayer, idACDOwner, TRUE);
        }
    }

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");

    if (UI_TestID(idName))
    {
        UI_GetTextFromID(idName).SetText(ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE), FALSE);
    }

    UpdateRequiredLevel(idRoot, tACDItem, idACDPlayer);

    ShowItemPrimaryStats(idRoot, &tACDItem, idACDOwner);

    ShowItemStatusIndicators(idRoot, idACDPlayer, tACDItem, FILTER_COSMETICS, 0, FALSE);

    ShowComparisonInfo(idRoot, tACDItem, FILTER_COSMETICS, 0, FALSE);


    UIID idProgress;
    idProgress.SetFromParent(idRoot, "IdentifyProgress");
    UI_GetProgressBarFromID(idProgress).SetActive(FALSE);

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    CRefString strEquippedText;
    BOOL bShouldShow = FALSE;
    BOOL bInInventory = tACDItem.IsInInventory();
    BOOL bEquipped = CosmeticItems_IsEquipped(*ptPlayer, tACDItem.hGB.gbid);
    BOOL bKnown = CosmeticItems_KnownByPlayer(*ptPlayer, tACDItem.hGB.gbid);

    CosmeticItemType eType = CosmeticItems_GetCosmeticItemTypeFromGBID(tACDItem.hGB.gbid);

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    BOOL bIsParagonPortraitFrame = ptACDPlayer != NULL && CosmeticItems_IsParagonFrame(*ptACDPlayer, tACDItem.hGB.gbid, eType);

    if (bKnown || bIsParagonPortraitFrame)
    {
        if (bKnown && bInInventory)
        {
            strEquippedText = StringListFetch("ConsoleInventory:AlreadyKnown");
            bShouldShow = TRUE;
        }
        else if (bEquipped)
        {
            strEquippedText = StringListFetch("ConsoleInventory:Equipped");
            bShouldShow = TRUE;
        }
    }













    else if (bInInventory)
    {
        strEquippedText = StringListFetch("ConsoleInventory:Learnable");
        bShouldShow = TRUE;
    }




    UIID idEquippedIndicator = UIIDCreateFromParent(idRoot, ".EquippedIndicator");
    UI_GetTextFromID(idEquippedIndicator).SetText(strEquippedText.str(), FALSE);
    UI_GetWindowFromID(idEquippedIndicator).SetActive(bShouldShow);


    UIID idNewIndicator = GetUIIDNone();
    idNewIndicator.SetFromParent(idRoot, "ItemNew");

    if (UI_TestID(idNewIndicator))
    {
        UI_GetWindowFromID(idNewIndicator).SetActive(bIsNew);
    }

    UIID idNewEliteIndicator = UIIDCreateFromParent(idRoot, ".ItemNewElite");
    UI_GetWindowFromID(idNewEliteIndicator).SetActive(FALSE);

    UIItemButton & tDyeButton = UI_GetItemButtonFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemDyed"));
    tDyeButton.SetActive(FALSE);

    UIID idDamaged;
    UIID idBroken;
    idDamaged.SetFromParent(idRoot, "DurabilityDamaged");
    idBroken.SetFromParent(idRoot, "DurabilityBroken");

    UI_GetWindowFromID(idDamaged).SetActive(FALSE);
    UI_GetWindowFromID(idBroken).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemTransmog")).SetActive(FALSE);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemEnchanted")).SetActive(FALSE);
    UI_GetItemButtonFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemDyed")).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".Ancient")).SetActive(FALSE);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".PrimalAncient")).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ItemRepairCost")).SetActive(FALSE);
}




void UpdateViewCraftingMaterialUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const ACDID idACDOwner, const BOOL bIsNew)
{





    UIID idRoot;
    idRoot.SetFromParent(idItemUI, "LayoutRoot");

    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");

    if (UI_TestID(idIcon))
    {
        SetUpItemButton(idIcon, tACDItem.id, idACDPlayer, idACDOwner, TRUE);
    }

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");

    if (UI_TestID(idName))
    {
        UI_GetTextFromID(idName).SetText(ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE), FALSE);
    }

    UpdateRequiredLevel(idRoot, tACDItem, idACDPlayer);

    ShowItemPrimaryStats(idRoot, &tACDItem, idACDOwner);

    ShowItemStatusIndicators(idRoot, idACDPlayer, tACDItem, FILTER_CRAFTING_MATERIALS, 0, FALSE);

    ShowComparisonInfo(idRoot, tACDItem, FILTER_CRAFTING_MATERIALS, 0, FALSE);


    UIID idProgress;
    idProgress.SetFromParent(idRoot, "IdentifyProgress");
    UI_GetProgressBarFromID(idProgress).SetActive(FALSE);

    Player * ptPlayer = PlayerGetByACD(idACDPlayer);

    CRefString strEquippedText;
    BOOL bShouldShow = FALSE;

    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);

    UIID idEquippedIndicator = UIIDCreateFromParent(idRoot, ".EquippedIndicator");
    UI_GetTextFromID(idEquippedIndicator).SetText(strEquippedText.str(), FALSE);
    UI_GetWindowFromID(idEquippedIndicator).SetActive(bShouldShow);


    UIID idNewIndicator = GetUIIDNone();
    idNewIndicator.SetFromParent(idRoot, "ItemNew");

    if (UI_TestID(idNewIndicator))
    {
        UI_GetWindowFromID(idNewIndicator).SetActive(bIsNew);
    }


    UIID idNewEliteIndicator = UIIDCreateFromParent(idRoot, ".ItemNewElite");
    UI_GetWindowFromID(idNewEliteIndicator).SetActive(FALSE);


    UIID idDamaged, idBroken;
    idDamaged.SetFromParent(idRoot, "DurabilityDamaged");
    idBroken.SetFromParent(idRoot, "DurabilityBroken");

    UI_GetWindowFromID(idDamaged).SetActive(FALSE);
    UI_GetWindowFromID(idBroken).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemTransmog")).SetActive(FALSE);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemEnchanted")).SetActive(FALSE);
    UI_GetItemButtonFromID(UIIDCreateFromParent(idRoot, ".ModifiedItemIcons.ItemDyed")).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".Ancient")).SetActive(FALSE);
    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".PrimalAncient")).SetActive(FALSE);


    UI_GetWindowFromID(UIIDCreateFromParent(idRoot, ".ItemRepairCost")).SetActive(FALSE);
}



static LPCSTR sGetIconNameForItem(const ActorCommonData & tACDItem)
{
    static const ItemLabel LABELS[] = { ITL_1_HAND, ITL_2_HAND, ITL_AMULET, ITL_BELT, ITL_BOOTS, ITL_BRACERS, ITL_CHESTARMOR, ITL_GLOVES, ITL_HELM, ITL_MOJO, ITL_ORB, ITL_LEGS, ITL_QUIVER, ITL_RING, ITL_SHIELD, ITL_SHOULDERS, ITL_OFFHAND_NECROMANCER };

    static LPCSTR TEXTURE_NAMES[] = {
        "Console_Artisan_BloodshardVendor_ItemIcon_1H",
        "Console_Artisan_BloodshardVendor_ItemIcon_2H",
        "Console_Artisan_BloodshardVendor_ItemIcon_Amulet",
        "Console_Artisan_BloodshardVendor_ItemIcon_Belt",
        "Console_Artisan_BloodshardVendor_ItemIcon_Boots",
        "Console_Artisan_BloodshardVendor_ItemIcon_Bracer",
        "Console_Artisan_BloodshardVendor_ItemIcon_Chest",
        "Console_Artisan_BloodshardVendor_ItemIcon_Hands",
        "Console_Artisan_BloodshardVendor_ItemIcon_Helmet",
        "Console_Artisan_BloodshardVendor_ItemIcon_Mojo",
        "Console_Artisan_BloodshardVendor_ItemIcon_Orb",
        "Console_Artisan_BloodshardVendor_ItemIcon_Pants",
        "Console_Artisan_BloodshardVendor_ItemIcon_Quiver",
        "Console_Artisan_BloodshardVendor_ItemIcon_Ring",
        "Console_Artisan_BloodshardVendor_ItemIcon_Shield",
        "Console_Artisan_BloodshardVendor_ItemIcon_Shoulders",
        "Console_Artisan_BloodshardVendor_ItemIcon_Phylactery",
    };

    GBID gbidItem = tACDItem.hGB.gbid;
    for (int32 i = 0; i < 17; ++i)
    {
        if (ItemHasLabel(gbidItem, LABELS[i]))
        {
            return TEXTURE_NAMES[i];
        }
    }
    return NULL;
}




void UpdateViewBloodShardVendorUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const ACDID idACDPlayer, const CRefString sItemName)
{




    UIID idRoot = UIIDCreateFromParent(idItemUI, ".LayoutRoot");
    ImageTextureFrame tIconTextureFrame;
    LPCSTR szTextureName = sGetIconNameForItem(tACDItem);

    if (szTextureName != NULL)
    {
        tIconTextureFrame = ImageTextureFrame(CRefString().Format("UI:%s", sGetIconNameForItem(tACDItem)));
    }

    UIID idIcon = UIIDCreateFromParent(idRoot, ".ItemVisual");

    UI_GetWindowFromID(idIcon).SetTextureFrame(tIconTextureFrame);
    UIID idName = UIIDCreateFromParent(idRoot, ".ItemName");

    UI_GetTextFromID(idName).SetText(sItemName, FALSE);


    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer != NULL)
    {


        UIWindow & tClassRestriction = UI_GetWindowFromID(UIIDCreateFromParent(idIcon, ".ClassRequirementX"));
        UIWindow & tLock = UI_GetWindowFromID(UIIDCreateFromParent(idIcon, ".LevelRequirementLock"));
        UIWindow & tMystery = UI_GetWindowFromID(UIIDCreateFromParent(idIcon, ".MysteryQuestionMark"));

        UIItemOverlay eOverlay = ItemGetOverlay(tACDItem, *ptACDPlayer);
        switch (eOverlay)
        {
        case ITEMOVERLAY_LOCKED:
            tClassRestriction.SetActive(FALSE);
            tLock.SetActive(TRUE);
            tMystery.SetActive(FALSE);
            break;

        case ITEMOVERLAY_UNUSABLE:
            tClassRestriction.SetActive(TRUE);
            tLock.SetActive(FALSE);
            tMystery.SetActive(FALSE);
            break;

        default:
            tClassRestriction.SetActive(FALSE);
            tLock.SetActive(FALSE);
            tMystery.SetActive(TRUE);
            break;
        }
    }
}

void SetUpItemButtonNoPlayer(const UIID & idItemUI, const ACDID idItem, const PlayerClass ePlayerClass, const BOOL bIsMale)
{
    UIItemButton & tItemButton = UI_GetItemButtonFromID(idItemUI);
    tItemButton.SetACDID(idItem);
    tItemButton.SetActive(idItem != ACDID_INVALID);

    ActorCommonData * ptACDItem = ACDTryToGet(idItem);
    if (ptACDItem != NULL)
    {
        if (ptACDItem->hGB.gbid == GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
        {
            tItemButton.SetQualityLevel(IQL_NORMAL);
        }

        ImageTextureFrame tImageTexFrame;
        ACDInventoryGetItemDrawInfoWithGBID(ptACDItem->hGB.gbid, tImageTexFrame, 0, ePlayerClass, bIsMale);
        tItemButton.SetAllTextureFrames(tImageTexFrame);
    }
}









void UpdateViewNoPlayerItemUI(const UIID & idItemUI, const ActorCommonData & tACDItem, const CRefString sItemName, const PlayerClass ePlayerClass, const BOOL bIsMale)
{





    UIID idRoot = UIIDCreateFromParent(idItemUI, ".LayoutRoot");


    UIID idIcon;
    idIcon.SetFromParent(idRoot, "ItemVisual");

    if (UI_TestID(idIcon))
    {
        SetUpItemButtonNoPlayer(idIcon, tACDItem.id, ePlayerClass, bIsMale);
    }

    UIID idName;
    idName.SetFromParent(idRoot, "ItemName");

    if (UI_TestID(idName))
    {
        UI_GetTextFromID(idName).SetText(ItemGetDisplayName(tACDItem, TRUE, FALSE, FALSE, TRUE), FALSE);
    }



    UIID idRequiredLevel;
    idRequiredLevel.SetFromParent(idRoot, "ItemRequiredLevel");

    if (UI_TestID(idRequiredLevel))
    {
        UI_GetTextFromID(idRequiredLevel).SetText("", FALSE);
    }

    ShowItemPrimaryStats(idRoot, &tACDItem, ACDID_INVALID);





    UIID idComparison;
    idComparison.SetFromParent(idRoot, "Bonuses");
    UI_GetWindowFromID(idComparison).SetActive(FALSE);


    UIID idProgress;
    idProgress.SetFromParent(idRoot, "IdentifyProgress");
    UI_GetProgressBarFromID(idProgress).SetActive(FALSE);

    UIID idEquippedIndicator = UIIDCreateFromParent(idRoot, ".EquippedIndicator");
    UI_GetWindowFromID(idEquippedIndicator).SetActive(FALSE);

    UIID idNewIndicator = GetUIIDNone();
    idNewIndicator.SetFromParent(idRoot, "ItemNew");

    if (UI_TestID(idNewIndicator))
    {
        UI_GetWindowFromID(idNewIndicator).SetActive(FALSE);
    }
}







CRefString sGetGemEffectsString(const ActorCommonData & tACDGem, const ACDID idACDItem)
{
    CRefString sResult;

    SocketEffectInfo arEffects[3];
    GetSocketedEffects(tACDGem, arEffects);

    ActorCommonData * ptItemACD = ACDTryToGet(idACDItem);
    BOOL bEffectHasApplied = FALSE;
    for (int32 i = 0; i < 3; ++i)
    {
        if (ptItemACD == NULL)
        {
            if (sResult.Length() > 0)
            {
                sResult += "\n\n";
            }
        }
        else
        {
            bEffectHasApplied = i == 2 || ItemIsOfType(ptItemACD->GetGBHandle(), arEffects[i].hgbItemType.gbid);
            if (!bEffectHasApplied)
            {
                continue;
            }
        }

        sResult += arEffects[i].sItemTypeName;
        sResult += "\n";
        sResult += arEffects[i].sDescription;

        if (bEffectHasApplied)
        {
            break;
        }
    }

    return sResult;
}





CRefString GetJewelEffectsString(const ActorCommonData & tACDJewel, const ACDID idACDItem)
{
    return sGetGemEffectsString(tACDJewel, idACDItem);
}






static LPCSTR arszBackgroundIds[] = { "GemSingle", "GemDouble", "GemTriple" };





void UpdateItemSocketsUI(const UIID & idParent, const ACDID idACDItem, const ACDID idPlayer)
{



    UIWindow & tParentWnd = UI_GetWindowFromID(idParent);
    const UIControlItemArray & aridChildren = tParentWnd.GetChildList();

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    int32 nNumSockets = ptACDItem != NULL ? ptACDItem->AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS)) : 0;
    tParentWnd.SetActive(nNumSockets != 0);
    if (nNumSockets != 0)
    {





        LPCSTR szBackgroundId = arszBackgroundIds[nNumSockets - 1];

        UIID idBackground;
        idBackground.SetFromParent(idParent, szBackgroundId);


        for (uint32 i = 0; i < aridChildren.size(); ++i)
        {

            UIWindow & tChildWnd = UI_GetWindowFromID(aridChildren[i].id);
            if (!tChildWnd.IsType(UIT_STACKPANEL))
            {

                tChildWnd.SetActive(tChildWnd.GetID() == idBackground);
            }
        }





        ItemSocketCallbackData tSocketData;
        tSocketData.idContainer.SetFromParent(idParent, "GemPanel");
        tSocketData.idACDPlayer = idPlayer;
        ACDInventoryIterateSlot(*ptACDItem, SLOT_ITEM_SOCKETS, NULL, sUpdateItemSocketUI, &tSocketData, FALSE, FALSE);
    }
}

inline BOOL sShouldShowRepairCost(const UIFilterType eFilterType, const BOOL bIsDetailedView) { return !bIsDetailedView && eFilterType == FILTER_DAMAGED; }



void ShowComparisonInfo(const UIID & idParent, const ActorCommonData & tACDItem, const UIFilterType eFilterType, const Blizzard::Time::Timestamp nExpiration, const BOOL bIsDetailedView)
{
    UIID idComparison;
    idComparison.SetFromParent(idParent, "Bonuses");

    BOOL bShouldDisplay = FALSE;
    if (!(eFilterType >= FILTER_RECIPES_START && eFilterType <= FILTER_RECIPES_END) &&
        !(eFilterType == FILTER_DAMAGED && !bIsDetailedView) &&
        !ItemIsUnidentified(tACDItem) &&
        ItemIsEquippableByPlayerOrHireling(tACDItem) &&
        !ItemIsEquipped(tACDItem))
    {
        bShouldDisplay = !ItemIsJunk(tACDItem) && nExpiration == 0;
    }


    UI_GetWindowFromID(idComparison).SetActive(bShouldDisplay);
}









void UpdateComparisonInfo(const UIID & idParent, const ActorCommonData * ptACDItem, const ACDID idACDOwner, const BOOL bUseAltSlot)
{




    UIID idComparison;
    idComparison.SetFromParent(idParent, "Bonuses");

    UIID idStat = GetUIIDNone();

    idStat.SetFromParent(idComparison, "BonusArmor");
    UIText & tQuickStatEHP = UI_GetTextFromID(idStat);

    idStat.SetFromParent(idComparison, "BonusDamage");
    UIText & tQuickStatDam = UI_GetTextFromID(idStat);

    idStat.SetFromParent(idComparison, "BonusVitality");
    UIText & tQuickStatHealing = UI_GetTextFromID(idStat);


    CRefString sValEHP, sValDmg, sValHealing;
    if (ptACDItem != NULL)
    {
        GetComparisonValueStrings(ACDTryToGet(idACDOwner), *ptACDItem, bUseAltSlot, FALSE, sValEHP, sValDmg, sValHealing);
    }

    tQuickStatDam.SetText(sValDmg, FALSE);
    tQuickStatEHP.SetText(sValEHP, FALSE);
    tQuickStatHealing.SetText(sValHealing, FALSE);
}




inline void ShowRepairCost(const UIID & idParent, const UIFilterType eFilterType, const BOOL bIsDetailedView)
{


    UIID idItemRepairCost;
    idItemRepairCost.SetFromParent(idParent, "ItemRepairCost");

    UI_GetWindowFromID(idItemRepairCost).SetActive(sShouldShowRepairCost(eFilterType, bIsDetailedView));
}







void GetRepairCostInfo(const ACDID idItem, const ACDID idPlayer, int32 & nRepairCost, BOOL & bCanAfford)
{
    if (idPlayer != ACDID_INVALID)
    {

        nRepairCost = ItemCrafting_GetRepairCost(idItem, idPlayer);
        ActorCommonData * ptACDPlayer = ACDTryToGet(idPlayer);
        if (ptACDPlayer != NULL)
        {
            if (!ptACDPlayer->CanAffordCurrencyCost(nRepairCost, CURRENCY_GOLD))
            {
                nRepairCost = (int32)ptACDPlayer->GetCurrencyAmount(CURRENCY_GOLD);
                bCanAfford = TRUE;
            }
        }
    }
}







void UpdateRepairCost(const UIID & idParent, const ACDID idPlayer, const ActorCommonData & tACDItem)
{
    UIID idRepairCost;
    idRepairCost.SetFromParent(idParent, "ItemRepairCost");

    int32 nRepairCost;
    BOOL bCanAfford;
    GetRepairCostInfo(tACDItem.id, idPlayer, nRepairCost, bCanAfford);

    CRefString sPrice = UIText_ValueToGameCurrency(nRepairCost, CURRENCY_GOLD);
    Set_Text_Color(sPrice, bCanAfford ? FONTCOLOR_WHITE : FONTCOLOR_RED);

    CRefString sText = LocalizedSprintf(StringListFetch("ConsoleShop:RepairCostFormat").str(), sPrice.str(), NULL);


    UI_GetTextFromID(idRepairCost).SetText(sText.str(), FALSE);
}










void UpdateRequiredLevel(const UIID & idParent, const ActorCommonData & tACDItem, const ACDID idACDPlayer)
{
    UIID idRequiredLevel;
    idRequiredLevel.SetFromParent(idParent, "ItemRequiredLevel");

    if (UI_TestID(idRequiredLevel))
    {
        CRefString sText;
        int32 nLevel = ItemGetLevelRequirement(tACDItem.hGB.gbid, &tACDItem, FALSE);
        if (nLevel > 0)
        {
            CRefString sLabel = "ConsoleArtisan:RequiredLevel";
            const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
            if (ptPlayer != NULL && ptPlayer->nCurrentLevel < nLevel)
            {
                sLabel = "ConsoleArtisan:RequiredLevelTooHigh";
            }

            sText = LocalizedSprintf(StringListFetch(sLabel.str()).str(), UnicodeText::FormatNumber(nLevel, TRUE, FNS_NORMAL, FALSE).str(), NULL);
        }

        UI_GetTextFromID(idRequiredLevel).SetText(sText.str(), FALSE);
    }
}















void ShowItemPrimaryStats(const UIID & idParent, const ActorCommonData * ptACDItem, const ACDID idACDOwner)
{



    UIID idStatContainer;
    idStatContainer.SetFromParent(idParent, "LargeStatContainer.LargeStatStackPanel");

    BOOL bShouldShow = ptACDItem != NULL && !ItemIsUnidentified(*ptACDItem) && ItemIsEquippableByPlayerOrHireling(*ptACDItem) && !ItemIsJunk(*ptACDItem) && !ItemIsCosmeticItem(ptACDItem->hGB.gbid);
    UI_GetWindowFromID(idStatContainer).SetActive(bShouldShow);

    if (!bShouldShow)
    {
        return;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(idACDOwner);


    CRefString sAttribute;
    CRefString sAttributeType;
    if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_ARMOR) || ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD))
    {
        sAttribute = CRefString().Format("%s ", ItemGetArmorTotalDescription(*ptACDItem, ePlayerClass, NULL, NULL).str());
        sAttributeType = StringListFetch("ConsoleInventory:Armor");
    }
    else if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON))
    {
        sAttribute = CRefString().Format("%s ", ItemGetDPSTotalDescription(*ptACDItem, ePlayerClass, NULL, NULL).str());
        sAttributeType = StringListFetch("ConsoleInventory:DPS");
    }

    UIID idStatValue;
    UIID idStatType;
    idStatValue.SetFromParent(idStatContainer, "LargeStatValue");
    idStatType.SetFromParent(idStatContainer, "LargeStatType");

    UI_GetTextFromID(idStatValue).SetText(sAttribute.str(), FALSE);
    UI_GetTextFromID(idStatType).SetText(sAttributeType.str(), FALSE);
}

// NON_MATCHING: original computes bShouldShow && bItemIsEquipped into w21 before UI_GetWindowFromID (4 bytes longer)

void ShowItemStatusIndicators(
    const UIID & idParent,
    const ACDID idPlayer,
    const ActorCommonData & tACDItem,
    const UIFilterType eFilterType,
    const Blizzard::Time::Timestamp nExpiration,
    const BOOL bIsDetailedView)
{

    BOOL bItemIsEquipped = FALSE;
    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_POTION))
    {

        Player * ptPlayer = PlayerGetByACD(idPlayer);
        const HotbarButtonData & tPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
        GBID gbidEquippedPotion = tPotionData.m_gbidItem;
        ACDID idEquippedPotion = ACDFromANN(tPotionData.m_annItem);
        if (idEquippedPotion != ACDID_INVALID)
        {
            bItemIsEquipped = idEquippedPotion == tACDItem.id && !ItemIsUnidentified(tACDItem);
        }
        else
        {
            bItemIsEquipped = gbidEquippedPotion == tACDItem.hGB.gbid && !ItemIsUnidentified(tACDItem);
        }
    }
    else
    {
        ItemUseType eItemUseType = ItemGetUseType(tACDItem);
        if (eItemUseType == ITEM_USE_COLLECTORS_EDITION_BUFF)
        {

            GBID gbidCosmetic = ItemGetCosmeticFromItem(tACDItem);
            if (gbidCosmetic != GBID_NONE)
            {
                VisualInventory * ptVisualInventory = VisualInventoryGet(idPlayer);
                if (ptVisualInventory != NULL)
                {

                    for (int32 i = 0; i < 4; ++i)
                    {
                        if (ptVisualInventory->tEquipmentCurrent.tCosmeticItems[i].gbidItem == gbidCosmetic)
                        {
                            bItemIsEquipped = TRUE;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            bItemIsEquipped = ItemIsEquippableByPlayerOrHireling(tACDItem) && ItemIsEquipped(tACDItem) && !ItemIsUnidentified(tACDItem);
        }
    }


    if (nExpiration > 0)
    {
        UIID idExpiration = UIIDCreateFromParent(idParent, ".EquippedIndicator");
        CRefString sExpiration = FormatExpirationTimeFromNow(nExpiration);
        UI_GetTextFromID(idExpiration).SetText(sExpiration.str(), FALSE);
        UI_GetWindowFromID(idExpiration).SetActive(TRUE);

        UIID idJunkIndicator = UIIDCreateFromParent(idParent, ".JunkIndicator");
        UI_GetWindowFromID(idJunkIndicator).SetActive(FALSE);
    }
    else
    {

        UIID idEquippedIndicator = UIIDCreateFromParent(idParent, ".EquippedIndicator");
        UI_GetTextFromID(idEquippedIndicator).SetText(StringListFetch("ConsoleInventory:Equipped").str(), FALSE);
        BOOL bShouldShow = eFilterType != FILTER_DAMAGED || bIsDetailedView;
        UI_GetWindowFromID(idEquippedIndicator).SetActive(bShouldShow && bItemIsEquipped);

        UIID idJunkIndicator = UIIDCreateFromParent(idParent, ".JunkIndicator");
        UI_GetWindowFromID(idJunkIndicator).SetActive(bShouldShow && ItemIsJunk(tACDItem));
    }
}




void ShowItemDetails(const UIID & idDetailsParent, const BOOL bShow)
{
    UIID idItemDetails = GetUIIDNone();
    idItemDetails.SetFromParent(idDetailsParent, "ScrollBox");
    UI_GetWindowFromID(idItemDetails).SetActive(bShow);
}






inline void ShowOnly(const int32 nIndexToShow, const UIID & idParent, const LPCSTR * arszIDNames, const int32 nIDListLength)
{
    for (int32 i = 0; i < nIDListLength; ++i)
    {
        UIID idControl;
        idControl.SetFromParent(idParent, arszIDNames[i]);
        UI_GetWindowFromID(idControl).SetActive(i == nIndexToShow);
    }
}














struct FindEquippedPotionCallbackData {
    ACDID idOwnerACD;
    ACDID idPotionACD;
    GBID gbidEquippedPotion;
    FindEquippedPotionCallbackData(GBID gbidEquippedPotion)
        : idPotionACD(ACDID_INVALID), gbidEquippedPotion(gbidEquippedPotion) {}
};

BOOL sFindPotionCallback(const InventoryLocation & tInvLoc, void * pCallBackData)
{
    FindEquippedPotionCallbackData * ptData = (FindEquippedPotionCallbackData *)pCallBackData;

    const ActorCommonData & tACDOwner = ACDGet(ptData->idOwnerACD);
    ACDID idACDItem = ACDInventoryGetSquareItem(tACDOwner, tInvLoc.eSlot, tInvLoc.pt);
    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData * ptItemACD = ACDTryToGet(idACDItem);
        if (ptItemACD != NULL && ItemHasLabel(ptItemACD->hGB.gbid, ITL_POTION) && ptItemACD->hGB.gbid == ptData->gbidEquippedPotion)
        {
            ptData->idPotionACD = idACDItem;
            return FALSE;
        }
    }

    return TRUE;
}





ACDID sGetEquippedItem(const Player * ptPlayer, const UIFilterType eFilterType)
{
    if (ptPlayer == NULL)
    {
        return ACDID_INVALID;
    }

    if (eFilterType == FILTER_MISCELLANEOUS_ITEMS)
    {

        ActorCommonData * ptPlayerACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptPlayerACD != NULL)
        {
            const HotbarButtonData & tEquippedPotionData = PlayerGetPotionBtnAssignment(ptPlayer);
            FindEquippedPotionCallbackData tPotionData(tEquippedPotionData.m_gbidItem);

            tPotionData.idOwnerACD = ptPlayer->idPlayerACD;
            ACDInventoryIterateSlot(*ptPlayerACD, SLOT_PLAYER_BACKPACK, NULL, sFindPotionCallback, &tPotionData, FALSE, FALSE);
            return tPotionData.idPotionACD;
        }
    }


    InventorySlotType eSlot = GetSlotForFilterType(eFilterType);
    if (eSlot == SLOT_NONE || eSlot == SLOT_PLAYER_BUYBACK || eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        return ACDID_INVALID;
    }

    ACDID idOwnerACD = ptPlayer->idPlayerACD;
    if (eSlot >= SLOT_HIRELING_START && eSlot <= SLOT_HIRELING_END)
    {

        idOwnerACD = PlayerGetHireling(idOwnerACD);
    }



    InventoryLocation tInvLoc;
    tInvLoc.eSlot = eSlot;
    tInvLoc.idACDContainer = idOwnerACD;
    tInvLoc.pt = cgptNull;

    return ACDInventoryGetItemInSlot(tInvLoc);
}



ACDID sGetEquippedWeaponByCraftingType(const Player * ptPlayer, const UIFilterType eFilterType)
{


    ACDID idEquippedMainHand = sGetEquippedItem(ptPlayer, FILTER_MAIN_HAND);
    ACDID idEquippedOffHand = sGetEquippedItem(ptPlayer, FILTER_OFF_HAND);
    if (idEquippedMainHand == ACDID_INVALID || eFilterType == FILTER_BLACKSMITH_RECIPES_OFF_HAND)
    {
        return idEquippedOffHand;
    }
    return idEquippedMainHand;
}








ACDID sGetEquippedArmorByCraftingType(const Player * ptPlayer, const UIFilterType eFilterType)
{


    static XGrowableMap<int, int, int, int> tFilterMap;


    if (tFilterMap.Count() == 0)
    {
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_RING_LEFT, FILTER_RING_LEFT);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_RING_RIGHT, FILTER_RING_RIGHT);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_NECK, FILTER_NECK);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_HANDS, FILTER_HANDS);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_SHOULDERS, FILTER_SHOULDERS);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_CHEST, FILTER_CHEST);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_HEAD, FILTER_HEAD);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_BELT, FILTER_BELT);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_LEGS, FILTER_LEGS);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_FEET, FILTER_FEET);
        tFilterMap.SetAt_Tracked(FILTER_BLACKSMITH_RECIPES_WRIST, FILTER_WRIST);
    }

    UIFilterType eMappedFilterType;
    if (tFilterMap.Lookup(eFilterType, eMappedFilterType))
    {
        return sGetEquippedItem(ptPlayer, eMappedFilterType);
    }



    return ACDID_INVALID;
}


inline ACDID sGetEquippedItemByCraftingType(const Player * ptPlayer, const UIFilterType eFilterType)
{
    if (eFilterType >= FILTER_BLACKSMITH_RECIPES_WEAPONS_START && eFilterType <= FILTER_BLACKSMITH_RECIPES_WEAPONS_END)
    {
        return sGetEquippedWeaponByCraftingType(ptPlayer, eFilterType);
    }
    else if (eFilterType >= FILTER_BLACKSMITH_RECIPES_ARMOR_START && eFilterType <= FILTER_BLACKSMITH_RECIPES_ARMOR_END)
    {
        return sGetEquippedArmorByCraftingType(ptPlayer, eFilterType);
    }
    return ACDID_INVALID;
}









ACDID sGetEquippedItemBySlotType(const Player * ptPlayer, const UIFilterType eFilterType, const BOOL fFallbackTo2HMainHand)
{

    ACDID idEquippedItem = sGetEquippedItem(ptPlayer, eFilterType);


    if (fFallbackTo2HMainHand && idEquippedItem == ACDID_INVALID)
    {
        if (eFilterType == FILTER_FOLLOWER_OFF_HAND || eFilterType == FILTER_OFF_HAND)
        {
            UIFilterType eFallbackFilterType = eFilterType == FILTER_OFF_HAND ? FILTER_MAIN_HAND : FILTER_FOLLOWER_MAIN_HAND;
            ACDID idMainEquippedItem = sGetEquippedItem(ptPlayer, eFallbackFilterType);
            ActorCommonData * ptACDMain = ACDTryToGet(idMainEquippedItem);
            if (ptACDMain != NULL)
            {
                return GetInventoryItemClass(ptACDMain->hGB.gbid) == IIC_TWOHANDED ? idMainEquippedItem : ACDID_INVALID;
            }
        }
    }

    return idEquippedItem;
}









ACDID GetEquippedItem(const Player * ptPlayer, const UIFilterType eFilterType, const BOOL fFallbackTo2HMainHand)
{
    if (eFilterType >= FILTER_RECIPES_START && eFilterType <= FILTER_RECIPES_END)
    {
        return sGetEquippedItemByCraftingType(ptPlayer, eFilterType);
    }
    else
    {
        return sGetEquippedItemBySlotType(ptPlayer, eFilterType, fFallbackTo2HMainHand);
    }
}
} // namespace Console
} // namespace UIInventory
#include "../../../Common/Protobuf/Leaderboard.pb.h"
// DWARF ActorInventory.cpp:3969 and VisualInventory.cpp:316; no src header declares them (UIConsoleMail.cpp:2136 does the same).
void ACDAddToInventory(ActorCommonData & tACDItem, const InventoryLocation & tLocation, const BOOL bMarkItemAsDirty, InventorySlotType eCameFrom); InventorySlotType VisualInventorySlotToInventorySlot(const VisualInventorySlot eVisualInventorySlot);
namespace UIInventory {
namespace Console {

ACDID GenerateLeaderboardEquippedItem(
    const D3::Leaderboard::HeroSnapshot & tHeroSnapshot,
    const UIFilterType eFilterType,
    blz::vector<ACDID, blz::allocator<ACDID> > & vecSocketACDs)
{

    InventorySlotType eSlotType = GetSlotForFilterType(eFilterType);

    BOOL bFoundARing = FALSE;

    for (int32 n = 0; n < tHeroSnapshot.equipped_items_size(); ++n)
    {
        const D3::Leaderboard::HeroEquippedItem & tItem = tHeroSnapshot.equipped_items(n);

        if (tItem.has_visual_inventory_slot())
        {
            VisualInventorySlot eVisualInventorySlot = tItem.visual_inventory_slot();
            if (eSlotType == VisualInventorySlotToInventorySlot(eVisualInventorySlot))
            {
                ACDID idACDItem = CItemGenerate(static_cast<ACDNetworkName>(10628 + eFilterType), tItem.generator(), NULL);

                if (idACDItem != ACDID_INVALID)
                {
                    ActorCommonData & tACDItem = ACDGet(idACDItem);

                    int32 nSocketsFilled = 0;
                    for (int32 i = 0; i < tItem.generator().contents().size(); ++i)
                    {
                        const D3::Items::EmbeddedGenerator & tEmbedded = tItem.generator().contents(i);

                        ACDID idACDSocketed = CItemGenerate(static_cast<ACDNetworkName>(9588 + vecSocketACDs.size()), tEmbedded.generator(), NULL);
                        vecSocketACDs.push_back(idACDSocketed);

                        ActorCommonData * ptACDGem = ACDTryToGet(idACDSocketed);
                        if (ptACDGem)
                        {
                            InventoryLocation tLocation;
                            tLocation.eSlot = SLOT_ITEM_SOCKETS;
                            tLocation.pt.Set(0, nSocketsFilled);
                            tLocation.idACDContainer = tACDItem.id;
                            ACDAddToInventory(*ptACDGem, tLocation, FALSE, SLOT_NONE);
                            ItemAddSocketedItemAttributes(*ptACDGem, tACDItem);
                            FastAttribMerge(tACDItem.idFastAttrib, ptACDGem->idFastAttrib);
                            ++nSocketsFilled;
                        }
                    }
                    tACDItem.AttributesSetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED), nSocketsFilled);
                }
                return idACDItem;
            }
        }
        else
        {



            if (eSlotType == SLOT_PLAYER_LEFT_FINGER || eSlotType == SLOT_PLAYER_RIGHT_FINGER)
            {
                GBID gbidItem = tItem.generator().gb_handle().gbid();

                ScopedItemDefinitionLockR pdefItem(gbidItem);
                if (pdefItem)
                {
                    ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);

                    if (ItemIsAllowedInSlot(SLOT_PLAYER_RIGHT_FINGER, pItemTypeDef) || ItemIsAllowedInSlot(SLOT_PLAYER_LEFT_FINGER, pItemTypeDef))
                    {

                        if (eSlotType == SLOT_PLAYER_LEFT_FINGER || (eSlotType == SLOT_PLAYER_RIGHT_FINGER && bFoundARing))
                        {
                            ACDID idACDItem = CItemGenerate(static_cast<ACDNetworkName>(10628 + eFilterType), tItem.generator(), NULL);

                            if (idACDItem != ACDID_INVALID)
                            {
                                ActorCommonData & tACDItem = ACDGet(idACDItem);

                                int32 nSocketsFilled = 0;
                                for (int32 i = 0; i < tItem.generator().contents().size(); ++i)
                                {
                                    const D3::Items::EmbeddedGenerator & tEmbedded = tItem.generator().contents(i);

                                    ACDID idACDSocketed = CItemGenerate(static_cast<ACDNetworkName>(9588 + vecSocketACDs.size()), tEmbedded.generator(), NULL);
                                    vecSocketACDs.push_back(idACDSocketed);

                                    ActorCommonData * ptACDGem = ACDTryToGet(idACDSocketed);
                                    if (ptACDGem)
                                    {
                                        InventoryLocation tLocation;
                                        tLocation.eSlot = SLOT_ITEM_SOCKETS;
                                        tLocation.pt.Set(0, nSocketsFilled);
                                        tLocation.idACDContainer = tACDItem.id;
                                        ACDAddToInventory(*ptACDGem, tLocation, FALSE, SLOT_NONE);
                                        ItemAddSocketedItemAttributes(*ptACDGem, tACDItem);
                                        FastAttribMerge(tACDItem.idFastAttrib, ptACDGem->idFastAttrib);
                                        ++nSocketsFilled;
                                    }
                                }
                                tACDItem.AttributesSetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED), nSocketsFilled);
                            }
                            return idACDItem;
                        }
                        else
                        {
                            bFoundARing = TRUE;
                        }
                    }
                }
            }



            else if (eSlotType == SLOT_PLAYER_WAIST || eSlotType == SLOT_PLAYER_BRACERS || eSlotType == SLOT_PLAYER_NECK)
            {

                GBID gbidItem = tItem.generator().gb_handle().gbid();

                ScopedItemDefinitionLockR pdefItem(gbidItem);
                if (pdefItem)
                {
                    ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);

                    if (ItemIsAllowedInSlot(eSlotType, pItemTypeDef))
                    {
                        ACDID idACDItem = CItemGenerate(static_cast<ACDNetworkName>(10628 + eFilterType), tItem.generator(), NULL);

                        if (idACDItem != ACDID_INVALID)
                        {
                            ActorCommonData & tACDItem = ACDGet(idACDItem);

                            int32 nSocketsFilled = 0;
                            for (int32 i = 0; i < tItem.generator().contents().size(); ++i)
                            {
                                const D3::Items::EmbeddedGenerator & tEmbedded = tItem.generator().contents(i);

                                ACDID idACDSocketed = CItemGenerate(static_cast<ACDNetworkName>(9588 + vecSocketACDs.size()), tEmbedded.generator(), NULL);
                                vecSocketACDs.push_back(idACDSocketed);

                                ActorCommonData * ptACDGem = ACDTryToGet(idACDSocketed);
                                if (ptACDGem)
                                {
                                    InventoryLocation tLocation;
                                    tLocation.eSlot = SLOT_ITEM_SOCKETS;
                                    tLocation.pt.Set(0, nSocketsFilled);
                                    tLocation.idACDContainer = tACDItem.id;
                                    ACDAddToInventory(*ptACDGem, tLocation, FALSE, SLOT_NONE);
                                    ItemAddSocketedItemAttributes(*ptACDGem, tACDItem);
                                    FastAttribMerge(tACDItem.idFastAttrib, ptACDGem->idFastAttrib);
                                    ++nSocketsFilled;
                                }
                            }
                            tACDItem.AttributesSetInt(FastAttribKey(ATTRIBUTE_SOCKETS_FILLED), nSocketsFilled);
                        }
                        return idACDItem;
                    }


                }
            }
        }
    }

    return ACDID_INVALID;

}



static void sUpdateActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, const SNO snoActor, const VisualInventoryType eVIT, const PlayerClass ePlayerClass, const BOOL fFemale, uint32 dwAnimTag)
{





    const SNO snoIrradianceTex = GlobalSNOGet(GLOBALSNO_RING_IRRADIANCE_TEXTURE);
    UIRActor & tHero = UI_GetRActorFromID(idRActorControl);

    RActor * ptRActor = RActorTryToGet(tHero.GetRActor());
    if (ptRActor != NULL && ptRActor->snoActor == snoActor)
    {


        VisualInventory tVisualInventoryCopy = tVisualInventory;
        UI_UpdateRActorForInventory(ptRActor, tVisualInventoryCopy, snoIrradianceTex, eVIT, ePlayerClass, fFemale, dwAnimTag, FALSE);
    }
    else
    {

        PRTransform transform(cgqIdentity, cgwpNull);
        tHero.DeleteRActor();

        RActorID idRActor = UI_CreateHeroUIRActor(snoActor, snoIrradianceTex, tVisualInventory.tEquipmentCurrent, eVIT, ePlayerClass, fFemale, transform,







            tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), dwAnimTag, 1.0f, FALSE);

        tHero.SetRActor(idRActor);
    }
}


void UpdatePlayerActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, const Player & tPlayer, uint32 dwAnimTag)
{
    PlayerClass eClass = PlayerGetPlayerClass(&tPlayer);
    BOOL bIsFemale = !PlayerIsMale(tPlayer);
    SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);
    sUpdateActorUI(idRActorControl, tVisualInventory, snoActor, VIT_PLAYER, eClass, bIsFemale, dwAnimTag);
}








void UpdatePlayerActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, PlayerClass eClass, BOOL bIsFemale, uint32 dwAnimTag)
{
    SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);
    sUpdateActorUI(idRActorControl, tVisualInventory, snoActor, VIT_PLAYER, eClass, bIsFemale, dwAnimTag);
}









void UpdateHirelingActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, ActorCommonData & tACDHireling, const SNO snoActor)
{
    sUpdateActorUI(idRActorControl, tVisualInventory, snoActor, VIT_HIRELING, PLAYER_CLASS_NONE, FALSE, 0x11000);

    UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
    RActor * ptRActorUI = RActorTryToGet(tModel.GetRActor());
    if (ptRActorUI != NULL)
    {
        CHirelingUpdateAppearance(tACDHireling, ptRActorUI, FALSE);
    }
}








void UpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl, VisualInventory & tVisualInventory)
{



    UIID idRActorControl = UIRACTOR_CONSOLE_RING;

    BOOL bShouldCreateRActor = ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, FALSE, TRUE);
    if (bShouldCreateRActor)
    {

        VisualInventory * ptVisualInventory = VisualInventoryGet(tPlayer.idPlayerACD);
        if (ptVisualInventory != NULL)
        {
            tVisualInventory.tEquipmentLast = tVisualInventory.tEquipmentCurrent;
            tVisualInventory.tEquipmentCurrent = ptVisualInventory->tEquipmentCurrent;

            UpdatePlayerActorUI(idRActorControl, tVisualInventory, tPlayer, 0x11000);
        }


        UIRActor & tHero = UI_GetRActorFromID(UIRACTOR_CONSOLE_RING);
        LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
        tHero.SetLight(idLight);
        tHero.SetObserver(UIUtils::Console::GetObserverForPlayerClass(PlayerGetPlayerClass(&tPlayer)));
    }



    UIID idPlayerStats = UIIDCreateFromParent(idParentControl, ".CharacterStats");
    UIUtils::Console::UpdateHeroNameAndLevel(idPlayerStats, tPlayer);
    UIUtils::Console::UpdateHeroStats(idPlayerStats, tPlayer);
    UIUtils::Console::UpdatePlayerIndicator(tPlayer);

    UIID idGold;
    idGold.SetFromParent(idParentControl, "GoldBalance");
    UpdateGoldAmount(idGold, tPlayer.idPlayerACD);
}









void ClearStatusLine(UIInventoryView * ptView)
{
    if (ptView != NULL)
    {

        const UIID & idStatusLine = ptView->GetStatusLineUIID();
        if (idStatusLine != GetUIIDNone())
        {
            UI_GetTextFromID(idStatusLine).SetText("", FALSE);
        }
    }
}




static const LPCSTR arszPanelIDNames[] =
{
    "CharacterStats",
    "StashUpgrade",
    "Repair",
    "Artisan.Crafting",
    "Artisan.Combine",
    "Artisan.JewelerSocketing",
    "Artisan.JewelerRemove",
    "Artisan.Salvage",
    "Artisan.SalvageResults",
    "VendorPanel",
    "Mail.MailPanel",
    "Artisan.MysticEnchanting",
    "SeasonCacheRingInfo.SeasonCacheInfo",
};



void SwitchToInfoPanel(const InfoPanelType eInfoPanel)
{

    ShowOnly(eInfoPanel, UIWND_INVENTORY_CONSOLE_ROOT, arszPanelIDNames, 13);
}






void UpdateNavBarTabState(
    const Player * ptPlayer,
    const UIID & idRoot,
    LPCSTR szNavigationContainerID,
    BOOL bUpdatePartyFollowerTab)
{


    ACDID idACDPlayer = ptPlayer->idPlayerACD;
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    // Inference: the original null-ACD paths share the final return.
    if (ptACDPlayer == NULL)
    {
        return;
    }
    UIID idBlinker;

    if (!PlayerCanOpenInventory(*ptACDPlayer))
    {

        UIID idTab;
        idTab.SetFromParent(idRoot, CRefString().Format("%s.Text1", szNavigationContainerID));
        UIButton & tTab = UI_GetButtonFromID(idTab);
        ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::INVENTORY_SCREEN, FALSE);
        tTab.SetEnabled(FALSE);
    }

    if (!PlayerCanOpenQuests())
    {
        UIID idTab;
        idTab.SetFromParent(idRoot, CRefString().Format("%s.Text5", szNavigationContainerID));
        UIButton & tTab = UI_GetButtonFromID(idTab);
        ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::JOURNAL_SCREEN, FALSE);
        tTab.SetEnabled(FALSE);
    }

    {
        // Inference: preserve the original short-circuit dialog check before querying new skills.
        BOOL bActive = idRoot != UIDLG_SKILLS_CONSOLE && UISkills::Console::HasNewSkillBlinker(ptPlayer);

        idBlinker.SetFromParent(idRoot, CRefString().Format("%s.New2", szNavigationContainerID));
        UIBlinker & tBlinker = UI_GetBlinkerFromID(idBlinker);
        tBlinker.SetActive(bActive);

        if (bActive && !gtTutorialMgr.IsAnyShowing(0))
        {

            gtTutorialMgr.ShowTutorial(idBlinker, GlobalSNOGet(GLOBALSNO_TUTORIAL_SKILL_MENU), ptPlayer, TRUE);
        }
    }


    ACDID idACDHireling = PlayerGetHireling(idACDPlayer);
    {
        ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
        BOOL bActive = idRoot != UIDLG_FOLLOWER_CONSOLE &&
                       idRoot != UIDLG_PARTY_CONSOLE &&
                       UIFollower::Console::HasNewSkillBlinker() && ptACDHireling != NULL &&
                       HirelingHasUnspentSkillPoints(*ptACDHireling);

        idBlinker.SetFromParent(idRoot, CRefString().Format("%s.New4", szNavigationContainerID));
        UIBlinker & tBlinker = UI_GetBlinkerFromID(idBlinker);
        tBlinker.SetActive(bActive);
    }

    BOOL bBlizzcon = CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT);



    if (!PlayerCanChangeParagon(*ptACDPlayer))
    {
        UIID idTab;
        idTab.SetFromParent(idRoot, CRefString().Format("%s.Text3", szNavigationContainerID));
        UIButton & tTab = UI_GetButtonFromID(idTab);
        ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::PARAGON_SCREEN, FALSE);
        tTab.SetEnabled(FALSE);
    }
    else
    {
        UIID idBlinkerScoped;
        idBlinkerScoped.SetFromParent(idRoot, CRefString().Format("%s.New3", szNavigationContainerID));
        UIBlinker & tBlinker = UI_GetBlinkerFromID(idBlinkerScoped);

        BOOL bActive = UIParagonBonuses::Console::HasLeveledSinceLastVisit(ptPlayer);
        tBlinker.SetActive(bActive);


        if (bActive && !gtTutorialMgr.IsAnyShowing(0))
        {
            gtTutorialMgr.ShowTutorial(idBlinkerScoped, GlobalSNOGet(GLOBALSNO_TUTORIAL_ALT_LEVEL_INVENTORY), ptPlayer, TRUE);
        }
    }


    // Inference: the original gates party/follower controls on these three conditions in order.
    if (bUpdatePartyFollowerTab &&
        idRoot != UIDLG_FOLLOWER_CONSOLE &&
        idRoot != UIDLG_PARTY_CONSOLE)
    {
        BOOL bHasHireling = idACDHireling != ACDID_INVALID;
        BOOL bHasParty = PlayersGetCount() > 1;

        ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::FOLLOWER_SCREEN, bHasHireling && !bBlizzcon);
        ::Console::Screens::EnableScreen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::PARTY_SCREEN, !bHasHireling && bHasParty && !bBlizzcon);

        UIID idTab;
        idTab.SetFromParent(idRoot, CRefString().Format("%s.Text4", szNavigationContainerID));
        UIButton & tTab = UI_GetButtonFromID(idTab);

        tTab.SetText(StringListFetch(bHasHireling ? "ConsoleNavMenu:Follower" : "ConsoleNavMenu:Party"), FALSE);
        tTab.SetEnabled((bHasHireling || bHasParty) && !bBlizzcon);
    }
}



void OnPlayerLeft()
{
    if (UI_GetPrimaryUIController() == -1 || !::Console::Screens::IsScreenSetOpen(::Console::Screens::GAMEPLAY_SCREEN_SET))
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(UI_GetPrimaryUIController());
    if (ptPlayer == NULL)
    {
        return;
    }

    if (::Console::Screens::IsScreenOpen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::PARTY_SCREEN))
    {
        ::Console::Screens::OpenPreviousScreen(UI_GetPrimaryUIController(), FALSE);
    }
    else if (::Console::Screens::IsScreenOpen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::INVENTORY_SCREEN))
    {
        UpdateNavBarTabState(ptPlayer, UIDLG_INVENTORY_CONSOLE, "LayoutRoot.Navigation.InventoryNav", TRUE);
    }
    else if (::Console::Screens::IsScreenOpen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::SKILLS_SCREEN))
    {
        UpdateNavBarTabState(ptPlayer, UIDLG_SKILLS_CONSOLE, "LayoutRoot.Navigation", TRUE);
    }
    else if (::Console::Screens::IsScreenOpen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::PARAGON_SCREEN))
    {
        UpdateNavBarTabState(ptPlayer, UIWND_PARAGON_CONSOLE, "LayoutRoot.Navigation", TRUE);
    }
    else if (::Console::Screens::IsScreenOpen(::Console::Screens::GAMEPLAY_SCREEN_SET, ::Console::Screens::JOURNAL_SCREEN))
    {
        UpdateNavBarTabState(ptPlayer, UIDLG_LORE_CONSOLE, "LayoutRoot.Navigation", TRUE);
    }
}





void GetSocketedEffects(const ActorCommonData & tACDSocketedItem, SocketEffectInfo * arEffects)
{


    SNO snoItemTypeNames = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_TYPES);
    SNO snoAttributeStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS);

    GBID gbidItemToSocket = tACDSocketedItem.hGB.gbid;

    POS pos;
    GBHandleList lstEffects(GBGetHandlePool());
    GBEnumerate(GB_SOCKETED_EFFECTS, lstEffects);

    const GameBalanceDefinition * pdefGB;
    pos = lstEffects.GetHeadPosition();

    while (pos)
    {
        const GBHandle & hGB = lstEffects.GetNext(pos);
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)GBLockR(hGB, pdefGB, TRUE);


        if (ptEffectDef->gbidItemBeingSocketed == gbidItemToSocket)
        {
            GBHandle hgbItemType(GB_ITEMTYPES, ptEffectDef->gbidItemTypeSocketedInto);

            CRefString usInitialItemTypeString;
            CRefString usAttributeFormatSpecifier;
            CRefString sItemType("");

            if (StringToGBID("All") == hgbItemType.gbid)
            {
                sItemType = "All";
            }
            else
            {
                sItemType = GBIDToString(hgbItemType, FALSE);
            }

            if (StringListFetch(snoItemTypeNames, sItemType, usInitialItemTypeString) &&
                StringListFetch(snoAttributeStringList, ptEffectDef->szStringlistLabel, usAttributeFormatSpecifier))
            {
                CRefString usEffect = UIToolTips_ProcessTip(&tACDSocketedItem,
                                                            tACDSocketedItem.AttributesGetAttributeGroup(),
                                                            -1,
                                                            SNO_NONE,
                                                            1,
                                                            usAttributeFormatSpecifier,
                                                            TTVC_NORMAL_COLORING,
                                                            sResolveSocketTag,
                                                            ptEffectDef,
                                                            NULL);

                if (usEffect.Length() > 0)
                {
                    int32 nStringIdx = 1;
                    if (StringToGBID("Weapon") == hgbItemType.gbid)
                    {
                        nStringIdx = 0;
                    }
                    if (StringToGBID("All") == hgbItemType.gbid)
                    {
                        nStringIdx = 2;
                    }

                    uint32 dwGrammaticalForm;
                    CRefString sItemTypeScoped, sName2Ignored;
                    LanguageRules::ExtractGrammaticalForm(usInitialItemTypeString, sItemTypeScoped, sName2Ignored, dwGrammaticalForm);
                    if (LanguageRules::ItemNamesShouldBeCapitalized())
                    {
                        sItemTypeScoped = UnicodeText::Capitalize(sItemTypeScoped);
                    }

                    Set_Text_Color(sItemTypeScoped, FONTCOLOR_GREY);
                    arEffects[nStringIdx].sItemTypeName = sItemTypeScoped;
                    arEffects[nStringIdx].sDescription = usEffect;
                    arEffects[nStringIdx].hgbItemType = hgbItemType;
                }
            }
        }

        GBUnlockR(pdefGB);
    }
}





CRefString GetFilterName(const UIFilterType eFilter)
{
    CRefString sResult;
    if (eFilter >= FILTER_PLAYER_START && eFilter < FILTER_COUNT)
    {
        sResult = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_INVENTORY), GetIconNameFromFilterType(eFilter));
    }
    return sResult;
}







void ShowFilterName(const UIID & idText, const UIFilterType eFilter, const BOOL bHighlight)
{
    CRefString sTitle = GetFilterName(eFilter);
    if (bHighlight)
    {
        Set_Text_Color(sTitle, FONTCOLOR_WHITE);
    }
    UI_GetTextFromID(idText).SetText(sTitle, FALSE);
}










CRefString sShowDurability(const ActorCommonData & tACDItem)
{
    CRefString sResult;

    int32 nMaxDurability = ItemGetMaxDurability(tACDItem);
    if (nMaxDurability != 0)
    {




        BOOL bItemIsBroken = ItemIsBroken(tACDItem);
        BOOL bItemIsDamaged = ItemIsDamaged(tACDItem);

        if (bItemIsDamaged || bItemIsBroken)
        {

            int32 nCurrentDurability = ItemGetCurrentDurability(tACDItem);
            sResult = LocalizedSprintf(StringListFetch("ConsoleInventory:Durability").str(), UnicodeText::FormatNumber(nCurrentDurability, TRUE, FNS_NORMAL, FALSE).str(), UnicodeText::FormatNumber(nMaxDurability, TRUE, FNS_NORMAL, FALSE).str(), NULL);

            if (bItemIsBroken)
            {
                Set_Text_Color(sResult, FONTCOLOR_RED);
            }
            else if (bItemIsDamaged)
            {
                Set_Text_Color(sResult, FONTCOLOR_YELLOW);
            }
        }
    }

    return sResult;
}


















} // namespace Console
} // namespace UIInventory
#include "../../../Common/UserInterface/UIInventory.h"
// DWARF Items.cpp:11319/11721/11838/9978/9962 and CItems.cpp:896; signatures use existing canonical types.
CRefString ItemGetPrimaryDescription(const ActorCommonData & tACD); CRefString ItemGetBlockStats(const ActorCommonData & tACD, PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData); CRefString ItemGetDamageStats(const ActorCommonData & tACD, PlayerClass ePlayerClass, ItemDescriptionColorOverride pfnColorOverride, void * ptColorOverrideData);
CRefString ItemGetClassRestrictionDescription(const ActorCommonData & tACDItem, PlayerClass ePlayerClass); CRefString ItemGetFlavorText(const ActorCommonData & tACDItem); CRefString ItemGetItemTypeDescription(const ActorCommonData & tACDItem, PlayerClass ePlayerClass); CRefString ItemGetCooldownTooltip(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer);
// DWARF Items.cpp:134 / SNO/SymbolicValue.cpp:1214; external data only.
extern LPCSTR guszItemLabelTransmogrification; extern const SymbolicEntry gDyeValueTooltips[24]; BOOL ItemMeetsOwnerClassRestrictions(GBID gbidItem, PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight);
namespace UIInventory { CRefString GetRequirementsTooltip(const ActorCommonData * ptACDPlayer, const ActorCommonData & tACDItem, GBID gbidRecipe); namespace Console {




























#line 2328
CRefString GetItemDescription(
    const ACDID idACDItem,
    const ACDID idACDOwner,
    const PlayerClass ePlayerClass,
    const BOOL bShowStatRanges)

{
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    // Inference: the item-null branch returns an empty string after both ACD lookups.
    if (ptACDItem == NULL)
    {
        return "";
    }

    if (ItemIsGift(ptACDItem->hGB.gbid))
    {

        CRefString sFriendName = ptACDItem->ptFriendGiftData->sNameRecipient;
        return LocalizedSprintf(
            StringListFetch("ConsoleInventory:GiftDetails").str(),
            sFriendName.str(), NULL);
    }

    // Inference: section indices start at zero, then advance through six DWARF-proven sections.
    const int32 MAX_SECTIONS = 6;
    CRefString sTextSections[MAX_SECTIONS];
    int32 nCurrentSection = 0;


    if (LanguageRules::PreferShorterMagicNames())
    {
        ItemQualityLevel eIQL = ItemGetQualityLevel(*ptACDItem);
        if (eIQL < IQL_RARE_AFFIX_START)
        {
            sTextSections[nCurrentSection] += ItemGetDisplayName(*ptACDItem, TRUE, FALSE, FALSE, FALSE);
            sTextSections[nCurrentSection] += "\n{icon:Console_InventoryBreak1}\n";
        }
    }
    nCurrentSection++;
#line 2372
    CRefString sAffixes = ItemGetAttributeDescriptions(*ptACDItem, ePlayerClass, bShowStatRanges, TRUE, NULL, NULL, TRUE, idACDOwner);
    if (sAffixes.Length())
    {
        sTextSections[nCurrentSection] += sAffixes;
    }


    if (ItemIsSetItem(ptACDItem->hGB.gbid))
    {
        sTextSections[nCurrentSection] += "{icon:Console_InventoryBreak2}\n";
        sTextSections[nCurrentSection] += GetSetItemTooltip(ptACDOwner, ePlayerClass, *ptACDItem, NULL);
    }
    nCurrentSection++;
#line 2389
    CRefString sPrimaryDescription = ItemGetPrimaryDescription(*ptACDItem);
    if (sPrimaryDescription.Length())
    {
        sTextSections[nCurrentSection] += sPrimaryDescription;
        sTextSections[nCurrentSection] += "\n";
    }


    if (ptACDOwner != NULL)
    {
        CRefString sCooldown = ItemGetCooldownTooltip(*ptACDItem, *ptACDOwner);
        if (sCooldown.Length())
        {
            sTextSections[nCurrentSection] += sCooldown;
        }
    }
#line 2408
    if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON))
    {
        sTextSections[nCurrentSection] += ItemGetDamageStats(*ptACDItem, ePlayerClass, NULL, NULL);
        sTextSections[nCurrentSection] += "\n";
    }
    else if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_ARMOR) ||
             ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD))
    {
        CRefString sBlockStats = ItemGetBlockStats(*ptACDItem, ePlayerClass, NULL, NULL);
        if (sBlockStats.Length() > 0 && sBlockStats != "\n")
        {
            sTextSections[nCurrentSection] += sBlockStats;
            sTextSections[nCurrentSection] += "\n";
        }
    }

    // Inference: preserve the original level/non-jewel/owner guard and required-level formatting.
    int32 nRequiredLevel = ItemGetLevelRequirement(ptACDItem->GetGBHandle().gbid, ptACDItem, FALSE);
    if (nRequiredLevel && !ItemIsUpgradeableJewel(ptACDItem->hGB.gbid) && ptACDOwner != NULL)
    {
        CRefString sRequiredLevel;
        sRequiredLevel = LocalizedSprintf(
            StringListFetch("ConsoleInventory:ItemRequiredLevel").str(),
            UnicodeText::FormatNumber(nRequiredLevel, TRUE, FNS_NORMAL, FALSE).str(), NULL);


        int32 nOwnerLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_LEVEL);
        if (nRequiredLevel > nOwnerLevel)
        {
            Set_Text_Color(sRequiredLevel, FONTCOLOR_RED);
            sTextSections[nCurrentSection] += "{icon:Console_BannerCustomization_Lock, 2.0} ";
        }
        sTextSections[nCurrentSection] += sRequiredLevel;
        sTextSections[nCurrentSection] += "\n";
    }


    sTextSections[nCurrentSection] += ItemGetItemTypeDescription(*ptACDItem, ePlayerClass);
    sTextSections[nCurrentSection] += "\n";


    if (!ItemMeetsOwnerClassRestrictions(ptACDItem->hGB.gbid, ePlayerClass, FALSE))
    {
        CRefString sRestriction = ItemGetClassRestrictionDescription(*ptACDItem, ePlayerClass);
        if (sRestriction.Length())
        {
            sTextSections[nCurrentSection] += sRestriction;
            sTextSections[nCurrentSection] += "\n";
        }
    }
    nCurrentSection++;
#line 2463
    CRefString sDurability = sShowDurability(*ptACDItem);
    if (sDurability.Length())
    {
        sTextSections[nCurrentSection] += sDurability;
        sTextSections[nCurrentSection] += "\n";
    }

    if (!ItemIsCosmeticItem(ptACDItem->hGB.gbid))
    {

        CRefString sRequirements = GetRequirementsTooltip(ptACDOwner, *ptACDItem, GBID_NONE);
        if (sRequirements.Length())
        {
            sTextSections[nCurrentSection] += sRequirements;
        }
    }
    nCurrentSection++;
#line 2483
    // Inference: zero denotes the no-dye value; only nondye items show this description.
    DyeType eDyeType = ptACDItem->GetDyeType();
    if (eDyeType != 0 && !ItemIsDye(*ptACDItem))
    {
        sTextSections[nCurrentSection] += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), SymbolToName(eDyeType, gDyeValueTooltips));
        sTextSections[nCurrentSection] += "\n";
    }


    GBID gbidTransmog = ptACDItem->AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    if (gbidTransmog != GBID_NONE)
    {
        ScopedItemDefinitionLockR pdefTransmogItem(gbidTransmog);
        if (pdefTransmogItem)
        {
            // DWARF f41c50/f41c60/f41c70 gives each of the next three locals logical line2499.
            CRefString usInitialItemName;
#line 2499
            CRefString sName1;
#line 2499
            CRefString sName2Ignored;
#line 2500
            SNO snoItemStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS);
            if (StringListFetch(snoItemStringList, pdefTransmogItem->tHeader.szName, usInitialItemName))
            {
                uint32 dwGrammaticalForm;
                LanguageRules::ExtractGrammaticalForm(usInitialItemName, sName1, sName2Ignored, dwGrammaticalForm);

                CRefString sTransmog = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelTransmogrification);
                Set_Text_Color(sTransmog, FONTCOLOR_BLUE);

                ItemQualityLevel eIQL = pdefTransmogItem->eDisplayedQualityLevel;
                if (eIQL == IQL_INVALID)
                {
                    if (ItemIsSetItem(gbidTransmog))
                    {
                        eIQL = IQL_DISPLAY_AS_SET_ITEM;
                    }
                    else if (ItemIsLegendary(gbidTransmog))
                    {
                        eIQL = IQL_LEGENDARY;
                    }
                }

                RGBAColor rgbaTransmogQuality = ItemGetDisplayNameColor(eIQL);
                if (LanguageRules::ItemNamesShouldBeCapitalized())
                {
                    sName1 = UnicodeText::Capitalize(sName1);
                }
                Set_Text_Color(sName1, rgbaTransmogQuality);
                sTransmog += sName1;

                sTextSections[nCurrentSection] += sTransmog;
                sTextSections[nCurrentSection] += "\n";
            }
        }
    }
    nCurrentSection++;
#line 2540
    CRefString sFlavor = ItemGetFlavorText(*ptACDItem);
    if (sFlavor.Length())
    {
        sTextSections[nCurrentSection] += sFlavor;
    }
#line 2549
    // Inference: the six unrolled append blocks correspond to this ascending section loop.
    CRefString sItemDescription;
    for (int32 i = 0; i < MAX_SECTIONS; ++i)
    {
        if (sTextSections[i].Length())
        {
            if (sItemDescription.Length())
            {

                sItemDescription += "\n";
            }
            sItemDescription += sTextSections[i];
        }
    }

    return sItemDescription;
}














#line 2567
CRefString GetItemDescription(const ACDID idACDOwner, const ACDID idACDItem, const BOOL bShowStatRanges)
{




    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDItem != NULL && ptACDOwner != NULL)
    {
        Player * ptPlayer = PlayerGetByACD(idACDOwner);


        PlayerClass ePlayerClass = ptPlayer != NULL ? PlayerGetPlayerClass(ptPlayer) : PLAYER_CLASS_NONE;


        return GetItemDescription(idACDItem, idACDOwner, ePlayerClass, bShowStatRanges);
    }

    return "";
}






UIFilterType GetFilterForSlot(InventorySlotType eSlotType)
{
    switch (eSlotType)
    {
    case SLOT_PLAYER_HEAD:
        return FILTER_HEAD;
    case SLOT_PLAYER_TORSO:
        return FILTER_CHEST;
    case SLOT_PLAYER_RIGHT_HAND:
        return FILTER_OFF_HAND;
    case SLOT_PLAYER_LEFT_HAND:
        return FILTER_MAIN_HAND;
    case SLOT_PLAYER_HANDS:
        return FILTER_HANDS;
    case SLOT_PLAYER_WAIST:
        return FILTER_BELT;
    case SLOT_PLAYER_FEET:
        return FILTER_FEET;
    case SLOT_PLAYER_SHOULDERS:
        return FILTER_SHOULDERS;
    case SLOT_PLAYER_LEGS:
        return FILTER_LEGS;
    case SLOT_PLAYER_BRACERS:
        return FILTER_WRIST;
    case SLOT_PLAYER_RIGHT_FINGER:
        return FILTER_RING_RIGHT;
    case SLOT_PLAYER_LEFT_FINGER:
        return FILTER_RING_LEFT;
    case SLOT_PLAYER_NECK:
        return FILTER_NECK;
    default:
        break;
    }
    return FILTER_NONE;
}

















float GetIdentifyingProgress(uint32 dwEndTime, uint32 dwDuration)
{
    uint32 dwNow = sgtAppGlobals.dwFrameCounter;
    uint32 dwRemaining = Math::Max(Math::Min<int32>(dwEndTime - dwNow, dwDuration), 0);
    return 1.0f - (float)dwRemaining / (float)dwDuration;
}






BOOL SendIdentifyRequest(const Player * ptPlayer, ACDID idACDItem)
{
    if (ptPlayer == NULL || idACDItem == ACDID_INVALID)
    {
        return FALSE;
    }



    if (GameIsPaused())
    {
        return FALSE;
    }

    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    uint32 dwFailureReason = TryIdentifyWithCast(idACDItem, *ptACDPlayer, ptPlayer);
    return dwFailureReason == 0;
}

}  // namespace Console
}  // namespace UIInventory

namespace {










int32 sGetKeyIdxFromStringTag(LPCSTR szTag)
{
    int32 nKeyIdx = -1;

    if (strcasecmp(szTag, "VALUE") == 0 || strcasecmp(szTag, "VALUE1") == 0)
    {
        nKeyIdx = 0;
    }
    else if (strcasecmp(szTag, "VALUE2") == 0)
    {
        nKeyIdx = 1;
    }
    else if (strcasecmp(szTag, "VALUE3") == 0)
    {
        nKeyIdx = 2;
    }
    else if (strcasecmp(szTag, "VALUE4") == 0)
    {
        nKeyIdx = 3;
    }

    return nKeyIdx;
}




BOOL sResolveSocketTag(const ActorCommonData * ptACDItem,
                              AttribGroupID,
                              AttribGroupID,
                              LPCSTR szTag,
                              float & flResult,
                              float *,
                              CRefString &,
                              SNO,
                              int32,
                              void * pData,
                              void *,
                              FontSpecifier &)

{
    int32 nKeyIdx = sGetKeyIdxFromStringTag(szTag);
    if (pData != NULL && nKeyIdx >= 0)
    {
        SocketedEffectsTableEntry * ptEffectDef = (SocketedEffectsTableEntry *)pData;


        {
            Attrib eAttrib = ptEffectDef->ptAttributes[nKeyIdx].eAttribute;
            FastAttribKey keyAttribute = MakeAttribKey(eAttrib, ptEffectDef->ptAttributes[nKeyIdx].nParam);
            flResult = AffixGetFormulaValue(
                ptEffectDef->ptAttributes[nKeyIdx].pbAttributeFormula,
                ptEffectDef->ptAttributes[nKeyIdx].serAttributeFormula.dwSize,
                ptACDItem,
                ptEffectDef->gbidItemBeingSocketed,
                NULL, 0, 0);
            return TRUE;
        }
    }

    return FALSE;
}





static LPCSTR arszSocketIds[] = { "Gem1", "Gem2", "Gem3" };





static LPCSTR arszSocketIconIds[] = { "GemIcon1", "GemIcon2", "GemIcon3" };





BOOL sUpdateItemSocketUI(const InventoryLocation & tInvLoc, void * pCallbackData)
{
    ItemSocketCallbackData & tData = *(ItemSocketCallbackData *)pCallbackData;

    ActorCommonData & tACDContainer = ACDGet(tInvLoc.idACDContainer);


    int32 nMaxSockets = tACDContainer.AttributesGetInt(FastAttribKey(ATTRIBUTE_SOCKETS));
    int32 nIndex = tInvLoc.pt.y;
    if (nIndex < 0 || nIndex >= 3)
    {
        return FALSE;
    }

    UIID idSocket;
    idSocket.SetFromParent(tData.idContainer, arszSocketIds[nIndex]);
    UIID idIcon;
    idIcon.SetFromParent(idSocket, arszSocketIconIds[nIndex]);

    UI_GetWindowFromID(idSocket).SetActive(nIndex < nMaxSockets);
    ACDID idACDItem = nIndex < nMaxSockets ? ACDInventoryGetSquareItem(tACDContainer, tInvLoc.eSlot, tInvLoc.pt) : ACDID_INVALID;
    UIInventory::Console::SetUpItemButton(idIcon, idACDItem, tData.idACDPlayer, tData.idACDPlayer, FALSE);

    return TRUE;
}

}  // namespace

namespace UIInventory {
namespace Console {

CRefString FormatExpirationTimeFromNow(Blizzard::Time::Timestamp nExpiration)
{
    CRefString sReturn;
    int64 nHoursToExpiration = nExpiration - Blizzard::Time::GetTimestamp();


    if (nHoursToExpiration < 86400000000000)
    {

        sReturn = StringListFetch("x1_ConsoleMail:ExpirationToday").str();
    }
    else if (nHoursToExpiration < 2678400000000000)
    {
        sReturn = LocalizedSprintf(StringListFetch("x1_ConsoleMail:ExpirationDays").str(), UnicodeText::FormatNumber(nHoursToExpiration / (86400000000000), TRUE, FNS_NORMAL, FALSE).str(), NULL);
    }
    else
    {
        sReturn = LocalizedSprintf(StringListFetch("x1_ConsoleMail:ExpirationMonths").str(), UnicodeText::FormatNumber(nHoursToExpiration / (2678400000000000), TRUE, FNS_NORMAL, FALSE).str(), NULL);
    }

    return sReturn;
}

}  // namespace Console
}  // namespace UIInventory
