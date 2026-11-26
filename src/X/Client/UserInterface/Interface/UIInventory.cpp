// Original file: X/Client/UserInterface/Interface/UIInventory.cpp
// Function bodies written from the DWARF inline trees and line tables; declarations from the DWARF.

#include "UIInventory.h"

#include "../UI.h"
#include "../UIID.h"
#include "../../CGame.h"
#include "../../CPlayerInput.h"
#include "../Controls/UIWindowControl.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/ItemsSwig.h"
#include "../../../Common/UserInterface/UIInventory.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/DataStructures/XGrowableList.h"
#include <stddef.h>
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../Input/Input.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../Sprite.h"
#include "../../Gfx/GfxCommon.h"
#include "../UIUtils.h"
#include "../../../Common/ActorInventory.h"
#include "../Controls/UITextControl.h"
#include "../../../Common/UserInterface/FontDefinition.h"
#include "../../ImageCollection.h"
#include "../../../Sigma/RefString.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Items.h"
#include "../../../Common/Player.h"
#include "../../../Common/Attributes/FastAttribute.h"
#include "../../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Controls/UITimerControl.h"
#include "../Controls/UIToolTipControl.h"
struct InvGlobals;
#include "UIToolTipManager.h"
// X/Client/UserInterface/UI.cpp
InvGlobals * UI_GetInventoryData();
extern const UIID UIDLG_INVENTORY_CONSOLE;
CursorMgr & PlayerGetCursorMgr();
// X/Common/Items.cpp
void ItemSetIsBeingManipulated(ActorCommonData & tACD, BOOL bManipulated, uint32 dwTimeout);
// X/Common/ActorInventory.cpp
void ACDInventoryGetSlotPageSize(const InventorySlotType eSlot, IVector2D & tSize);

// Declared in X/Common/Tutorials.h (not adopted yet).
namespace Tutorial {
BOOL PlayerHasSeen(const Player & tPlayer, const SNO snoTutorial);
}
// Declared in X/Client/UserInterface/Interface/UITutorials.h (not adopted yet); only the
// member this file calls.
class UITutorial;
#include "UITutorials.h"

#include "../../Definitions/PreferencesDefinition.h"

extern UITutorialManager gtTutorialMgr;
// X/Common/Items.cpp
uint32 ItemGetRemainingCooldownTime(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);
SNO ItemGetUsePower(const GBID gbidItem);

ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACDItem);  // X/Common/Items.cpp

// X/Common/ActorInventory.cpp
BOOL ACDInventorySlotIsMultiItemSlot(const InventorySlotType eSlot);
void ACDInventoryLocationSet(InventoryLocation & tInvLoc, ACDID idACDContainer, InventorySlotType eSlot, int32 x, int32 y);
BOOL ActorInventoryIsVariableSizedSlot(const InventorySlotType eSlot);
BOOL ActorInventoryIsVariableSizedSlotValid(const InventoryLocation & tInvLoc);
// X/Common/Items.cpp
BOOL ItemIsDrawnAsLargeSlotItem(const GBID gbidItem);

BOOL ItemIsLegendary(GBID gbidItem);  // X/Common/Items.cpp:5080
GBID ItemGetItemTypeGBID(GBID gbidItem);  // X/Common/Items.cpp:1717
ACDID LocalPlayerACDGet();  // CPlayer.cpp:1686
BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:3403
InventorySlotType ACDInventoryGetOppositeTwoHandedSlot(InventorySlotType eSlot);  // X/Common/ActorInventory.cpp:532
CRefString ItemGetCooldownTooltip(const ActorCommonData & tACDItem, const ActorCommonData & tACDPlayer);  // X/Common/Items.cpp
#include "../../Font/Font.h"
// X/Common/ActorInventory.cpp
BOOL ACDInventoryCanHold(const ActorCommonData & tACD, const ActorCommonData & tACDItem, const InventorySlotType eSlot, IVector2D & tWhereCanHold, const int32 * pnSlotYMinMax);
BOOL ACDInventoryFindNonFullStackInSlot(const ActorCommonData & tACDItem, const ActorCommonData & tACDContainer, const InventorySlotType eCheckSlot, InventoryLocation & tNewLocation, const int32 * pnSlotYMinMax);
// X/Client/CActorInventory.cpp
void CACDInventoryRequestQuickMove(ActorCommonData & tACDItem, const InventoryLocation & tInvLoc, const int32 * pnSlotYMinMax, const Player * ptPlayer);
// X/Common/Player.h: the error record (the header does not carry it yet).
#include "../../Font/FormattedText.h"

BOOL ItemHasCubeEnchant(const ActorCommonData & tACDItem); // Items.cpp:5325
Attrib ItemGetAttributeForCubeEnchant(const ActorCommonData & tACDItem); // Items.cpp:5334
BOOL PlayerCanManipulateInventory(const ActorCommonData & tACDPlayer); // Player.cpp:4878
#include "../../../Sigma/ErrorManager/ErrorManager.h"
namespace UIInventory { void DrawItemOverlay(const Rect2D &, UIItemOverlay, float, float &, const Rect2D *); } // UIInventory.cpp:2537

namespace UIGame { void DisplayError(const GameErrorData & tData, BOOL fDisplayBnetError, const Player * ptPlayer); }

namespace UIInventory {
void PlayItemInventorySound(const ActorCommonData & tACDItem);
void TryToQuickMoveToLocation(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const InventorySlotType eSlot);
CRefString GetItemStatsTooltip(const PlayerClass ePlayerClass, const ActorCommonData * ptACDPlayer, const ActorCommonData & tACDItem);
BOOL GetComparisonValues(ActorCommonData * ptACDOwner, ACDID idACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, float & flComparisonDeltaPctEHP, float & flComparisonDeltaPctDmg, float & flComparisonDeltaPctHealing);
BOOL GetComparisonValuesPct(ActorCommonData * ptACDOwner, ACDID idACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, float & flComparisonDeltaPctEHP, float & flComparisonDeltaPctDmg, float & flComparisonDeltaPctHealing);
void GetComparisonValueStrings(ActorCommonData * ptACDOwner, const ActorCommonData & tACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, CRefString & sValEHP, CRefString & sValDmg, CRefString & sValHealing);
CRefString FormatItemComparisonField(const float flValue);
void AddCraftingNotification(ACDNetworkName annItem);
void ApplySwapIndicator(const ActorCommonData & tACDItem);
BOOL GetSlotRect(const InventoryControlType eInventoryControlType, const InventorySlotType eSlot, const IVector2D & tPoint, const UIInventoryControl eInventoryControl, const BOOL fUnshifted, Rect2D & wRect);
BOOL GetItemSlotEmptyImage(const ActorCommonData * ptACD, const ActorCommonData * ptACDItem, const InventorySlotType eSlot, ImageTextureFrame & tImageTexFrame);
void DrawEmptyPaperDollSlot(const ActorCommonData * ptACD, const ActorCommonData * ptACDItem, const InventorySlotType eSlot, float flZ, const UIInventoryControl eInventoryControl);
CRefString ComparisonPctToIcon(const float flValue);
void GetSelectedSlot(const UIID & uId, InventorySlotType & eSlot, IVector2D & tSlot, UIInventoryControl & eContext);
BOOL GetMouseOverSlot(UIID & uId, InventorySlotType & eSlot, IVector2D & tSlot, UIInventoryControl & eContext);
void TrackItemDrop(const ActorCommonData & tACDItem, InventorySlotType & eSlot, IVector2D & tSlot);
int32 GetStashOffset();
ACDID FindBestItemToUse(GBID gbidItem);
// X/Client/UserInterface/Interface/UIInventoryCommon.cpp
BOOL IsMagicColor(const ItemQualityLevel eIQL);
BOOL IsRareColor(const ItemQualityLevel eIQL);
BOOL IsLegendaryColor(const ItemQualityLevel eIQL);
BOOL GetItemBkgrndImage(const ItemQualityLevel eIQL, ImageTextureFrame & tImageTexFrame, const BOOL fEquipped, const BOOL fLargeItem, const int32 nAncientRank, const BOOL fSeasonOnly, const BOOL fSanctified, const BOOL fPrimalized);
}
uint32 ItemGetCooldownTimeToApply(const ActorCommonData & tACDItem); // Items.cpp:2488
uint32 ItemGetTradeTimeLeft(const ActorCommonData & tACDItem); // Items.cpp:10344
const UIID & UI_GetCapture(); // UI.cpp:526
namespace UIInventory { BOOL SlotIsEmpty(const ActorCommonData & tACD, const InventorySlotType eSlot, const IVector2D & pt); } // UIInventoryCommon.cpp:363
enum ShopQuickMoveResult { SQMR_UNHANDLED = 0, SQMR_REPAIR = 1, SQMR_SELL = 2 };
namespace UIInventory { void UpdateToolTips(ActorCommonData & tACDPlayer); }
void UIFunction_GetMouse(Vector2D & vecMousePos); // UIFunctions.cpp:3238




enum CrafterQuickMoveResult { CQMR_UNHANDLED = 0, CQMR_DO_NOTHING = 1, CQMR_IDENTIFY = 2, CQMR_ENCHANT = 3, CQMR_REFORGE = 4, CQMR_TRANSMOG = 5, CQMR_REMOVE_GEM = 6, CQMR_REPAIR = 7, CQMR_TRANSMUTE = 8, CQMR_DYE_ITEM = 9 };
namespace UIInventory { void DrawItemRarityIcon(GBID, float, float &, SNO, const Rect2D &, const Vector2D &, const Rect2D *, BOOL); void DrawLegendaryGemRank(int32, float &, const Rect2D &, const Rect2D *); } // UIInventoryCommon.cpp:533,592
struct BackpackTimers { int32 nNumTimers; UIID * pidTimers; };
struct UIMap { UIID arSlotId[4]; InventorySlotType eSlot; IVector2D pt; InventoryControlType eInventoryControlType; };

BOOL ActorInventoryIsLargeSlot(InventorySlotType eSlot); // X/Common/ActorInventory.cpp:12434
IVector2D ACDInventoryGetItemSize(const ActorCommonData & tACDItem, InventorySlotType eSlot);
namespace UIInventory { void DrawSlotHighlights(InventorySlotType eSlot, const RGBAColor & rgbaColor, float & flZ, UIInventoryControl eInventoryControl); }

BOOL ACDInventoryIsSlotHireling(InventorySlotType eSlot); // X/Common/ActorInventory.cpp:10875
namespace UIHireling { ACDID GetCurrentHireling(); } // UIHireling.cpp:1214
static ACDID sGetSlotOwner(ACDID idACDPlayer, InventorySlotType eSlot);
namespace UIInventory { ACDID GetHighlightedItem(); }
namespace UIInventory { void SlotDrawVariableSlotBackground(const InventorySlotType eSlot, float flZ, const UIInventoryControl eInventoryControl, const int32 nSlots, const int32 nSlotYOffset); }
namespace UIInventory { void InitItemTooltip(ItemTooltipData & tData, const ACDID idACD, const GBID gbid, const ItemTooltipParameterBlock & tParams, const BOOL bCalculateComparisonData, ActorCommonData * ptACDPlayer, const BOOL bDisableSocketText); }
namespace UIInventory { void DrawItemSockets(ActorCommonData & tACD, XPooledDataIDList & listSocketedACDs, const PlayerClass eClassToDisplay, const BOOL fIsMale, const Rect2D & rectItemImage, const BOOL fEquipped, const float flAlpha, float & flZ); }
BOOL InputShiftKeyIsDown(InputContext eContext); // X/Client/Input/Input.h
BOOL InputAltKeyIsDown(InputContext eContext); // X/Client/Input/Input.h
void ACDInventoryWhereCanThisGo(const ActorCommonData & tACDPlayer, const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots); // ActorInventory.cpp:11604
void ACDInventoryWhereCanThisGoHireling(const GBID gbidItem, int32 & nNumSlots, InventorySlotType * eValidSlots); // ActorInventory.cpp:11712
BOOL ItemIsRandomItemPlaceholder(const GBID gbidItem); // Items.cpp:2342
BOOL ACDIsPlayer(ACDID idACD); // ActorCommonData.h
namespace UIInventory { void ShowAdditionalTooltips(const UIID & idParent, ActorCommonData & tACDOwner, const GBID gbidItem, const InventorySlotType eSlotToIgnore, const BOOL bResetScrollPos); }
#include "../../../Common/NPC.h"
#include "../../../Common/Crafting/Salvage.h"
CrafterType NPCGetCrafterType(const ActorCommonData & tACD); // X/Common/NPC.cpp
enum { INDICATOR_DURATION = 12, NUM_SWAP_INDICATORS = 13 }; // DWARF cpp:122


class TempHighlight {

public:
    BOOL ShouldFade();

    uint8 GetFadeValue();

    ACDNetworkName GetItemANN() { return m_annItem; }




    ACDID GetItem();

    void Set(ACDNetworkName annItem, uint32 nFadeDelay, uint32 nFadeTime);



private:
    ACDNetworkName m_annItem;
    uint32 m_nFadeStart;
    uint32 m_nFadeEnd;
};




class FlashOverlayInfo {  // size 0xc
public:
    void Create(ACDNetworkName annItem, int32 nDuration);
    void Destroy();
    ACDID GetItem();
    ACDNetworkName GetItemANN() { return m_annItem; }
    BOOL GetAlpha(uint8 & bAlpha);
private:
    uint32 m_nStartTime;  // 0x00
    uint32 m_nDuration;  // 0x04
    ACDNetworkName m_annItem;  // 0x08
};

typedef XGrowableList<UIMap, UIMap &> UIMapList;
typedef XGrowableList<TempHighlight, TempHighlight &> UITempHighlights;
typedef XGrowableList<FlashOverlayInfo, FlashOverlayInfo &> UIFlashOverlays;

class ItemTooltipState {  // size 0x10
public:
    void Initialize();
    BOOL Changed(const InventorySlotType eSlot, const ActorCommonData * ptACDItem) const;
    void Set(const InventorySlotType eSlot, const ActorCommonData * ptACDItem);
private:
    InventorySlotType m_eSlot;  // 0x00
    ACDID m_idACDItem;  // 0x04
    BOOL m_fIdentified;  // 0x08
    int32 m_nFilledSockets;  // 0x0c
};

class NewItemIndicatorMgr {  // size 0x60
public:
    NewItemIndicatorMgr();
    void MarkAsNew(const ActorCommonData & tACD);
    void StartIndicators();
    BOOL FoundMagicItem() const;
private:
    XGrowableList<ACDID, ACDID> m_list;  // 0x00
    BOOL m_fFoundMagicItem;  // 0x58
};

class SwapIndicator {  // size 0x10
public:
    SwapIndicator();
    void Start(const uint32 dwDuration);
    Vector2D GetJiggleOffset() const;
    void Update();
private:
    void sUpdateDesiredOffset(const uint32 dwElapsedTime);
    uint32 m_dwStartTime;  // 0x00
    uint32 m_dwDuration;  // 0x04
    Vector2D m_vecOffset;  // 0x08
};

struct TutorialInfo {  // size 0x20
    UIID idTutorial;  // 0x00
    ACDID idACDItem;  // 0x08
    UIID idAnchor;  // 0x10
    SNO snoTutorialOnOpen;  // 0x18
    TutorialInfo();
};

struct InvGlobals {
    ItemTooltipState tPreviousTooltipState;  // 0x00
    BackpackTimers tBackpackTimers;  // 0x10
    BackpackTimers tSharedStashTimers;  // 0x20
    VisualInventory tVisualInventory;  // 0x30
    UIMapList tUIMapList;  // 0x150
    UITempHighlights tUIHighlightList;  // 0x1a8
    UIFlashOverlays tUIFlashOverlays;  // 0x200
    SwapIndicator * ptSwapIndicators;  // 0x258
    NewItemIndicatorMgr tNewItemMgr;  // 0x260
    int32 nStashPageOffset;  // 0x2c0
    TutorialInfo tTutorialInfo;  // 0x2c8
    BOOL fFoundNewCurrency;  // 0x2e8
    ACDID idACDDragged;  // 0x2ec
    CRefString unk_2f0;  // 0x2f0, stash filter text
};
static_assert(offsetof(InvGlobals, unk_2f0) == 0x2f0, "InvGlobals");
static_assert(offsetof(InvGlobals, nStashPageOffset) == 0x2c0, "InvGlobals");






inline void SwapIndicator::Start(const uint32 dwDuration)
{

    m_dwStartTime = CGameGetRealTicks();
    m_dwDuration = dwDuration;
    sUpdateDesiredOffset(0);
}

inline Vector2D SwapIndicator::GetJiggleOffset() const { return m_vecOffset; }

BOOL ItemIsQuest(const ActorCommonData & tACDItem); // X/Common/Items.cpp
ItemSellError ItemCanBeSold(const ActorCommonData & tACDItem, const ActorCommonData * ptACDVendor); // X/Common/Items.cpp:3963
ItemTypeDefinition * ItemsItemTypeDefLockR(const GBID gbidItem, const GameBalanceDefinition *& pdefGBItemTypes); // X/Common/Items.cpp:3367
extern UIID UIDLG_STASH_PAGE; // X/Client/UserInterface/UIGlobals.cpp
namespace UIInventory { CRefString GetInstructionTooltip(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem, const InventorySlotType eSlot, const BOOL fHotbar, const BOOL fShowQuickTip); }
#include "../../../Common/Powers/PowersCommon.h"
void PowerApplyGenericAttributeBuffAttributes(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, AttribGroupID idFastAttrib); // PowersCommon.cpp:8452
BOOL ACDCanDualWield(const ActorCommonData & tACD); // ActorInventory.cpp:11419
BOOL ACDInventoryDoItemsConflict(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItemMain, const ActorCommonData & tACDItemOffHand); // ActorInventory.cpp:1007
BOOL ItemMeetsOwnerClassRestrictions(GBID gbidItem, PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight); // Items.cpp:1883
AttribGroupID ItemGenerateUpscaledAttribForPlayer(ActorCommonData & tACDOwner, ActorCommonData & tACDItem); // Items.cpp:6706
void ClearUpscaledAttributes(ActorCommonData & tACDItem); // Items.cpp:6929
void FastAttribLightCopy(AttribGroupID idAttribGroupSource, AttribGroupID idAttribGroupDest); // FastAttribute.cpp:4615
void FastAttribSetValueToDefault(AttribGroupID idAttribGroup, FastAttribKey tKey); // FastAttribute.cpp:4401
static InventorySlotType sComputeComparisonSlot(const ActorCommonData & tACDOwner, GBID gbidItem, InventorySlotType eSlotToIgnore, BOOL bUseAltSlot, BOOL & bShowAltTooltipInstruction, BOOL bCheckVisiblePage);
BOOL ItemIsEquippableByPlayerOrHireling(GBID gbidItem); // Items.cpp:9460
#include <stdlib.h>
#include "../../Prefetch.h"
#include "../../../Common/Hireling.h"
void TextureGetFrameRectPixels(IRect2D & rectTexturePixels, SNO snoTexture, uint32 dwFrameIndex); // TextureDefinition.cpp:2697
BOOL ACDInventoryValidateSlotForACD(const ActorCommonData & tACD, InventorySlotType eSlot); // ActorInventory.cpp:2156
BOOL ACDInventoryIsSlotMerchant(InventorySlotType eSlot); // ActorInventory.cpp:10888
BOOL ACDInventoryIsSlotValidForHireling(const ActorCommonData & tACD, HirelingClass eClass, InventorySlotType eSlot); // ActorInventory.cpp:552
namespace UIInventory { void PlayItemPickupSound(); }
static const RGBAColor sgrgbaNone(0, 0, 0, 0); // Original UIInventory.cpp:114-119 in DWARF order; values from surviving HighlightSlot/SlotDrawItemInSlot immediate stores.
static const RGBAColor sgrgbaRed(255, 0, 0, 128);
static const RGBAColor sgrgbaBroken(255, 0, 0, 255); // Inference: 0xff0000ff stored at sGetHighlightColor:2967 in SlotDrawItemInSlot
static const RGBAColor sgrgbaYellow(240, 210, 5, 128);
static const RGBAColor sgrgbaWhite(255, 255, 255, 128); static const RGBAColor sgrgbaDisabled(255, 64, 64, 192); // Disabled: 0xc04040ff stored at SlotDrawItemInSlot:3296 (no RGBAColor ctor inline there)
BOOL ItemIsBeingManipulated(const ActorCommonData & tACDItem); // CItems.cpp:1310
namespace UIInventory { void HighlightSlot(InventorySlotType eSlot, const IVector2D & tSlot, const ActorCommonData & tACD, const ActorCommonData & tACDItem, float & flZ); }
namespace UIInventory { void SlotDrawItems(ActorCommonData & tACD, PlayerClass eClassToDisplay, BOOL fIsMale, InventorySlotType eSlot, float & flZ, UIInventoryControl eInventoryControl); }
namespace UIInventory { void SlotDrawItemInSlot(ActorCommonData * ptACDOwner, ActorCommonData & tACDItem, XPooledDataIDList & listSocketedACDs, BOOL fHighlighted, BOOL fEnabled, BOOL fGhosted, PlayerClass eClassToDisplay, BOOL fIsMale, const InventoryLocation & tInvLoc, float & flZ, UIInventoryControl eInventoryControl); }
BOOL ACDInventoryItemShouldBeGhosted(const ActorCommonData & tACDUser, const ActorCommonData & tACDItem); // ActorInventory.cpp:11378
Attrib ActorInventoryGetVariableSizedSlotAttrib(InventorySlotType eSlot); // ActorInventory.cpp:12416
BOOL TradeItemIsBeingOffered(const Player & tPlayer, ACDID idACDItem); // Trade.cpp:245
#include "../../../Sigma/MemoryPools.h"
Player * LocalPlayerGet(); // CPlayer.cpp:1612
UITimer * UI_GetTimerFromID(const UIID &); // X/Client/UserInterface/UI.cpp
BOOL ItemIsDamaged(const ActorCommonData &); // X/Common/Items.cpp
BOOL ItemIsBroken(const ActorCommonData &); // X/Common/Items.cpp
BOOL ItemUsesQuiver(const ActorCommonData &); // X/Common/Items.cpp
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../Battlenet/Hub.h"
#include "../../../Common/Shell/CmdLine.h"
#include "../../Sound/Sound.h"
#include "UIShop.h"
Hub & HubGet(); // X/Client/Battlenet/Hub.cpp:93
void CACDInventoryRequestToSocket(const ActorCommonData &, const ActorCommonData &, Player *); // CActorInventory.cpp:560
void CACDInventoryRequestUse(ActorCommonData &, ACDID, BOOL, Player *); // CActorInventory.cpp:278
void CACDInventoryRequestMove(ActorCommonData &, const InventoryLocation &, BOOL, Player *); // CActorInventory.cpp:484
void CACDInventoryRequestSplitStack(const ActorCommonData *, const InventoryLocation &, uint64); // CActorInventory.cpp:596
void CACDInventoryRequestStackTransfer(ACDID, ACDID, uint64); // CActorInventory.cpp:625
BOOL ItemUseIsTargettedInInventory(ItemUseType); // Items.cpp:2504
GameError ItemCanBeUsed(ACDID, const ActorCommonData &, const Target *); // Items.cpp:2594
namespace UIGame { BOOL MenuIsActive(); } // UIGame.cpp:7067
GameError ACDInventoryCanMove(const ActorCommonData &, const InventoryLocation &, ACDID *); // ActorInventory.cpp:10573
namespace UIInventory { void OnClick(const UIID &); }
#include "../../Console/GamerProfile.h"
BOOL ItemCrafting_CanUnlockTransmog(GBID, const Player *, BOOL); // X/Common/Crafting/ItemCrafting.cpp:2199
BOOL ItemCrafting_IsTransmogUnlocked(GBID, const Player *, GBID *); // X/Common/Crafting/ItemCrafting.cpp:2134
BOOL ItemIsUnique(const ActorCommonData &); // Items.cpp:5206
BOOL ItemIsUniqueEquipped(const ActorCommonData &); // Items.cpp:5219
BOOL ItemIsEquippedButDisabled(const ActorCommonData &); // Items.cpp:8690
BOOL ItemIsEquippedButDisabledDuplicateLegendary(const ActorCommonData &); // Items.cpp:8705
namespace UIInventory { CRefString GetRequirementsTooltip(const ActorCommonData *, const ActorCommonData &, GBID); }
#include "../../../Sigma/Random.h"
#include "../../../Common/Attributes/AttributeEnums.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Common/UserInterface/UIToolTips.h"
#include <float.h>
int32 SortBonuses(const void * arg0, const void * arg1); // X/Common/UserInterface/UIInventory.cpp:131
namespace UIInventory { CRefString GetSetItemTooltipBonuses(const GBID gbidSet); }
#include "../../../Common/Protobuf/Items.pb.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/TypeDescriptor/SymbolicValue.h"
extern LPCSTR guszItemLabelTransmogrification; // X/Common/Items.cpp:134
extern const SymbolicEntry gDyeValueTooltips[24]; // X/Common/SNO/SymbolicValue.cpp:1214
void ACDGetItemEffectTypeAndLevel(const ActorCommonData &, ItemEffectType &, int32 &, BOOL, BOOL); // X/Common/VisualInventory.cpp:877
namespace UIInventory { CRefString GetCostTooltip(const ActorCommonData *, const ActorCommonData *, const ActorCommonData &, BOOL); }
namespace UIInventory { CRefString GetItemLevelTooltip(const ActorCommonData &); }
BOOL ACDInventorySlotIsEquippable(InventorySlotType); // ActorInventory.cpp:10928
UIItemOverlay ItemGetOverlay(const ActorCommonData &, const ActorCommonData &); // CItems.cpp:117
void SpriteSubmitTimerControlSprite(const UIID &, float); // UITimerControl.cpp:229
// Only this call with dwElapsedTime == 0 survives (inlined into ApplySwapIndicator, where it folds
// to a single store of -JIGGLE_AMOUNT); the angle and amplitude terms are reconstructed guesses.
inline void SwapIndicator::sUpdateDesiredOffset(const uint32 dwElapsedTime)
{
    const float JIGGLE_AMOUNT = 5.0f;
    Angle ang = (float)dwElapsedTime * (2.0f * PI / (float)m_dwDuration);
    float flAmp = JIGGLE_AMOUNT * (1.0f - (float)dwElapsedTime / (float)m_dwDuration);


    m_vecOffset.y = -flAmp * Math::Cos(ang);
}




namespace UIInventory {
void NotifyOfCurrencyPickup(Player & tPlayer, CurrencyType eCurrencyType);
void UpdateTimerVisibility(UITimer & tTimer, const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem);
void SetFoundNewCurrency(BOOL fVal);
void NotifyOfNewCraftingMatCurrency();
void Show(const BOOL fShow);
void UpdateItemManipulationTimers();
BOOL GetItemTooltipParametersForSlot(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const ActorCommonData * ptACDContainer, const InventorySlotType eSlot, const BOOL fShowQuickTip, const BOOL bConsoleInventory, const BOOL bShowAsRepaired);
void GetItemTooltipParameters(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const BOOL bConsoleInventory, const BOOL bShowAsRepaired);
void Display(const float flDeltaTime);
BOOL IsOpen();
void OnRightClick(const UIID & uId);
void SetStashPageIndex(const int32 nPage);
void OnDragStart(const UIID & uId);
UIResult OnDrag(const UIID & uId);
}



inline BOOL TempHighlight::ShouldFade()
{

    uint32 dwNow = CGameGetClientTicks();
    return dwNow > m_nFadeStart;
}




inline uint8 TempHighlight::GetFadeValue()
{

    uint32 dwNow = CGameGetClientTicks();
    uint32 dwElapsed = dwNow - m_nFadeStart;
    uint32 dwDuration = m_nFadeEnd - m_nFadeStart;
    float flVal = (float)dwElapsed / (float)dwDuration;
    float fAlpha = Math::Pin(Math::Lerp(255.0f, 0.0f, flVal), 0.0f, 255.0f);
    return (uint8)fAlpha;
}




inline ACDID TempHighlight::GetItem()
{
    return ACDFromANN(m_annItem);
}




inline void TempHighlight::Set(ACDNetworkName annItem, uint32 nFadeDelay, uint32 nFadeTime)
{




    uint32 dwNow = CGameGetClientTicks();
    m_annItem = annItem;
    m_nFadeStart = dwNow + nFadeDelay;
    m_nFadeEnd = m_nFadeStart + nFadeTime;
}




inline void FlashOverlayInfo::Create(ACDNetworkName annItem, int32 nDuration)
{
    m_annItem = annItem;


    m_nStartTime = CGameGetClientTicks();
    m_nDuration = nDuration;
}











inline ACDID FlashOverlayInfo::GetItem()
{
    return ACDFromANN(m_annItem);
}




inline BOOL FlashOverlayInfo::GetAlpha(
    uint8 & bAlpha)
{
    if (m_nDuration != 0)
    {

        uint32 dwElapsed = CGameGetClientTicks() - m_nStartTime;
        float flUnitAlpha = (float)dwElapsed / (float)m_nDuration;
        flUnitAlpha = Math::Pin(flUnitAlpha, 0.0f, 1.0f);

        bAlpha = Math::FloatToInt((1.0f - flUnitAlpha) * 255.0f);
        return bAlpha != 0;
    }
    return FALSE;
}













inline void ItemTooltipState::Initialize()
{

    m_eSlot = SLOT_NONE; m_idACDItem = ACDID_INVALID; m_fIdentified = FALSE; m_nFilledSockets = 0;
}






inline BOOL ItemTooltipState::Changed(
    const InventorySlotType eSlot,
    const ActorCommonData * ptACDItem) const
{
    ItemTooltipState tTemp;
    tTemp.Set(eSlot, ptACDItem);

    if (m_eSlot != tTemp.m_eSlot || // inferred: original rows 522:6/31 and 523-525:3 are if-operand columns (tab-indented original)
        m_idACDItem != tTemp.m_idACDItem ||
        m_fIdentified != tTemp.m_fIdentified ||
        m_nFilledSockets != tTemp.m_nFilledSockets)
    {
        return TRUE;
    }
    return FALSE;
}



inline void ItemTooltipState::Set(
    const InventorySlotType eSlot,
    const ActorCommonData * ptACDItem)
{
    m_eSlot = eSlot;
    if (ptACDItem)
    {
        m_idACDItem = ptACDItem->id;
        m_fIdentified = ItemIsUnidentified(*ptACDItem);
        m_nFilledSockets = ItemGetNumFilledSockets(*ptACDItem);
    }
    else
    {
        m_idACDItem = ACDID_INVALID; m_fIdentified = FALSE; m_nFilledSockets = 0;
    }
}
















































































































































static BOOL sItemShouldBeGhosted(
    const ActorCommonData & tACD,
    const ActorCommonData * ptACDCursorItem)
{
    if (ptACDCursorItem && tACD.id == ptACDCursorItem->id)
    {
        return TRUE;
    }

    if (ItemIsBeingManipulated(tACD))
    {
        return TRUE;
    }

    ActorCommonData & tACDPlayer = ACDGet(LocalPlayerACDGet());
    if (!PlayerCanManipulateInventory(tACDPlayer))
    {
        return TRUE;
    }

    if (PlayerGetCursorMgr().GetCursorState() == CURSORSTATE_SALVAGE_ITEM)
    {
        if (SalvageCommon::ItemCanSalvage(tACD, tACDPlayer) != -1)
        {
            return TRUE;
        }
    }


    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_LOOPING_ANIMATION_SUPPRESS_ITEM_TOOLTIPS))
    {
        return TRUE;
    }
















    if (TradeItemIsBeingOffered(*LocalPlayerGet(), tACD.id))
    {
        return TRUE;
    }

    return ACDInventoryItemShouldBeGhosted(tACDPlayer, tACD) != FALSE;
}




























static void sAddFlashOverlay(ACDNetworkName annItem, int32 nDuration)
{

    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
    if (ptInventoryGlobals == NULL)
        return;





    POS pos = ptInventoryGlobals->tUIFlashOverlays.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        FlashOverlayInfo & tInfo = ptInventoryGlobals->tUIFlashOverlays.GetNext(pos);
        if (tInfo.GetItemANN() == annItem)
        {
            ptInventoryGlobals->tUIFlashOverlays.RemoveAt(posOld);
        }
    }


    FlashOverlayInfo tInfo;
    tInfo.Create(annItem, nDuration);
    ptInventoryGlobals->tUIFlashOverlays.AddTail_Tracked(tInfo);
}



static void sAddHighlightOverlay(ACDNetworkName annItem, uint32 nHighlightFadeDelay, uint32 nHighlightFadeTime)
{




    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
    if (ptInventoryGlobals == NULL)
        return;





    POS pos = ptInventoryGlobals->tUIHighlightList.GetHeadPosition();
    while (pos)
    {
        POS posOld = pos;
        TempHighlight & tHighlight = ptInventoryGlobals->tUIHighlightList.GetNext(pos);
        if (tHighlight.GetItemANN() == annItem)
        {
            ptInventoryGlobals->tUIHighlightList.RemoveAt(posOld);
        }
    }



    TempHighlight tHighlight;
    tHighlight.Set(annItem, nHighlightFadeDelay, nHighlightFadeTime);
    ptInventoryGlobals->tUIHighlightList.AddTail_Tracked(tHighlight);
}
static int32 sGetSwapIndicatorIndex(const InventorySlotType eSlot)
{
    // Inference: single-row helper; original line 842 col 42 is the second range comparison.
    return (eSlot >= SLOT_PLAYER_HEAD && eSlot < SLOT_PLAYER_HEAD + NUM_SWAP_INDICATORS) ? eSlot - SLOT_PLAYER_HEAD : -1;



}





static void sApplySwapIndicator(const int32 nIndicatorIndex, const ACDID idACDItem, const uint32 dwDuration)
{



    if (nIndicatorIndex != -1)
    {
        InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
        if (ptInventoryGlobals == NULL)
            return;




        ptInventoryGlobals->ptSwapIndicators[nIndicatorIndex].Start(dwDuration);

        ACDNetworkName annItem = ANNFromACD(idACDItem);
        sAddFlashOverlay(annItem, 15);
    }
}


void UIInventory::ApplySwapIndicator(const ActorCommonData & tACDItem)
{


    const InventoryLocation & tInvLoc = tACDItem.GetInventoryLocation();


    int32 nIndicatorIndex = sGetSwapIndicatorIndex(tInvLoc.eSlot);
    sApplySwapIndicator(nIndicatorIndex, tACDItem.id, INDICATOR_DURATION);
}







































































































































































































































































































inline void UIInventory::SetFoundNewCurrency(BOOL fVal)
{
    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
    if (ptInventoryGlobals != NULL)
        ptInventoryGlobals->fFoundNewCurrency = fVal;
}













void UIInventory::NotifyOfNewCraftingMatCurrency()
{



    SetFoundNewCurrency(TRUE);
}



















void UIInventory::NotifyOfCurrencyPickup(Player & tPlayer, CurrencyType eCurrencyType)
{
    if (eCurrencyType == CURRENCY_BLOODSHARDS)
    {
        SNO snoTutorial = GlobalSNOGet(GLOBALSNO_TUTORIAL_CURRENCY);
        if (!Tutorial::PlayerHasSeen(tPlayer, snoTutorial))
            gtTutorialMgr.ShowTutorial(GetUIIDNone(), snoTutorial, NULL, TRUE);
    }
}





































































































































































































































































void UIInventory::AddCraftingNotification(ACDNetworkName annItem)
{
    const uint32 nFlashDuration = 60;
    const uint32 nHighlightFadeTime = 60;
    const uint32 nHighlightFadeDelay = 7200;


    sAddHighlightOverlay(annItem, nHighlightFadeDelay, nHighlightFadeTime);
    sAddFlashOverlay(annItem, nFlashDuration);
}






































































































































































































static InventorySlotType sGetSlotFromId(
    const UIID & uId,
    IVector2D & tSquare,
    UIInventoryControl & eContext)
{
    if (!UI_GetControlFromID(uId).IsActive())
    {
        tSquare.Set(0, 0);
        return SLOT_NONE;
    }











    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;


    UIMapList & tList = ptUIGlobals->ptInventory->tUIMapList;

    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        const UIMap & tButtonMap = tList.GetNext(pos);

        for (int32 nContext = 0; nContext < INVENTORY_COUNT; nContext++)
        {
            if (uId == tButtonMap.arSlotId[nContext])
            {

                tSquare = tButtonMap.pt;
                eContext = nContext;
                return tButtonMap.eSlot;
            }
        }
    }

    tSquare.Set(0, 0);
    eContext = INVENTORY_COUNT;
    return SLOT_NONE;
}



static UIID sGetIdFromSlot(
    InventorySlotType eSlot,
    const IVector2D * ptPoint,
    InventoryControlType eInventoryControlType,
    BOOL * pfExactUId,
    UIInventoryControl eInventoryControl)
{







    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;


    UIMapList & tList = ptUIGlobals->ptInventory->tUIMapList;

    POS pos = tList.GetHeadPosition();
    while (pos)
    {
        const UIMap & tButtonMap = tList.GetNext(pos);

        if (tButtonMap.eSlot == eSlot &&
            tButtonMap.eInventoryControlType == eInventoryControlType)
        {



            if (tButtonMap.pt.x == -1 && tButtonMap.pt.y == -1)
            {
                return tButtonMap.arSlotId[eInventoryControl];
            }

            // Inferred: the ptPoint null test leaves no instruction (the one caller passes &tPoint). Evidence: the original did not promote this pointer parameter to two ints (GetSlotRect keeps two str, not one stp), which needs a non-load use of it.
            if (ptPoint && tButtonMap.pt.x == ptPoint->x &&
                tButtonMap.pt.y == ptPoint->y)
            {
                if (pfExactUId)
                {
                    *pfExactUId = TRUE;
                }

                return tButtonMap.arSlotId[eInventoryControl];
            }
        }
    }









    return GetUIIDNone();
}






static void sGetSlotGridSize(
    const UIID & uId,
    const IVector2D & tSlotSize,
    Vector2D & vecGridSize)
{
    Rect2D rectUIC;
    UI_GetWindowFromID(uId).GetWindowRect(rectUIC);
    vecGridSize.x = rectUIC.Width();
    if (tSlotSize.x > 1)
    {
        vecGridSize.x /= (float)tSlotSize.x;
    }
    vecGridSize.y = rectUIC.Height();
    if (tSlotSize.y > 1)
    {
        vecGridSize.y /= (float)tSlotSize.y;
    }
}



void UIInventory::GetSelectedSlot(
    const UIID & uId,
    InventorySlotType & eSlot,
    IVector2D & tSlot,
    UIInventoryControl & eContext)
{

    eSlot = sGetSlotFromId(uId, tSlot, eContext);
    if (eSlot == SLOT_NONE)
    {
        tSlot.Set(0, 0);
    }


    if (tSlot.x == -1 || tSlot.y == -1)
    {












        if (ACDInventorySlotIsMultiItemSlot(eSlot))
        {
            Vector2D vecMousePos;
            UIFunction_GetMouse(vecMousePos);


            Rect2D rectSlotUIC;
            UI_GetWindowFromID(uId).GetWindowRect(rectSlotUIC);


            vecMousePos.x -= rectSlotUIC.left;
            vecMousePos.y -= rectSlotUIC.top;


            IVector2D tSlotPageSize;
            ACDInventoryGetSlotPageSize(eSlot, tSlotPageSize);
            Vector2D vecGridSize;
            sGetSlotGridSize(uId, tSlotPageSize, vecGridSize);


            tSlot.x = Math::FloatToInt(vecMousePos.x / vecGridSize.x);



            ActorCommonData * ptACDCursorItem = NULL;
            CursorMgr & tCursorMgr = PlayerGetCursorMgr();
            CursorState eCursorState = tCursorMgr.GetCursorState();
            switch (eCursorState)
            {
            case CURSORSTATE_MOVE_ITEM: ptACDCursorItem = tCursorMgr.GetItemToMove();
                break;


            case CURSORSTATE_SPLIT_STACK: ptACDCursorItem = tCursorMgr.GetStackToSplit();
                break;


            case CURSORSTATE_BUY_ITEM: ptACDCursorItem = tCursorMgr.GetItemToBuy();
                break;
            }
            if (ptACDCursorItem && ItemIsDrawnAsLargeSlotItem(ptACDCursorItem->hGB.gbid))

                tSlot.y = Math::FloatRoundToInt(vecMousePos.y / vecGridSize.y);

            else

                tSlot.y = Math::FloatToInt(vecMousePos.y / vecGridSize.y);


            tSlot.x = Math::Pin(tSlot.x, 0, tSlotPageSize.x - 1);
            tSlot.y = Math::Pin(tSlot.y, 0, tSlotPageSize.y - 1);

            if (eSlot == SLOT_PLAYER_SHARED_STASH)
            {
                tSlot.y += GetStashOffset();
            }
        }
    }








}



inline BOOL UIInventory::GetMouseOverSlot(
    UIID & uId,
    InventorySlotType & eSlot,
    IVector2D & tSlot,
    UIInventoryControl & eContext)
{

    uId = UI_GetMousedOverControl();
    if (!UI_TestID(uId))
        return FALSE;



    UIControl & tControl = UI_GetControlFromID(uId);
    if (!tControl.IsActive())
        return FALSE;




    GetSelectedSlot(uId, eSlot, tSlot, eContext);
    if (eSlot == SLOT_NONE) // inferred: original loads eSlot at 1957:9 (an if operand column, not a return expression)
        return FALSE;
    return TRUE;
}







static BOOL sGetSlotItemRect(
    const ActorCommonData * ptACDItem,
    const InventoryControlType eInventoryControlType,
    const InventorySlotType eSlot,
    const IVector2D & tPoint,
    UIInventoryControl eInventoryControl,
    const BOOL fUnshifted,
    Rect2D & rectSlot)
{
    if (!UIInventory::GetSlotRect(eInventoryControlType, eSlot, tPoint, eInventoryControl, fUnshifted, rectSlot))
        return FALSE;



    if (ptACDItem &&
        ACDInventorySlotIsMultiItemSlot(eSlot))
    {
        IVector2D vecItemSize = ACDInventoryGetItemSize(*ptACDItem, eSlot);
        rectSlot.right = rectSlot.left + rectSlot.Width() * vecItemSize.x;
        rectSlot.bottom = rectSlot.top + rectSlot.Height() * vecItemSize.y;
    }
    return TRUE;
}





BOOL UIInventory::GetSlotRect(
    const InventoryControlType eInventoryControlType,
    const InventorySlotType eSlot,
    const IVector2D & tPoint,
    const UIInventoryControl eInventoryControl,
    const BOOL fUnshifted,
    Rect2D & wRect)
{

    BOOL fExactUId = FALSE;
    UIID uId = sGetIdFromSlot(eSlot, &tPoint, eInventoryControlType, &fExactUId, eInventoryControl);
    if (!UI_TestID(uId))
        return FALSE;



    if (fUnshifted)
    {
        UI_GetWindowFromID(uId).GetUnshiftedWindowRect(wRect);
    }
    else
    {
        UI_GetWindowFromID(uId).GetWindowRect(wRect);
    }


    if (fExactUId)
        return TRUE;




    if (!ACDInventorySlotIsMultiItemSlot(eSlot))
        return TRUE;



    IVector2D tSize;
    ACDInventoryGetSlotPageSize(eSlot, tSize);

    Vector2D vecGridSize;
    sGetSlotGridSize(uId, tSize, vecGridSize);

    int32 nSlotYOffset = 0;
    if (eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        nSlotYOffset = GetStashOffset();
    }


    wRect.left += vecGridSize.x * (float)tPoint.x;
    wRect.top += vecGridSize.y * ((float)tPoint.y - nSlotYOffset);
    wRect.right = vecGridSize.x + wRect.left;
    wRect.bottom = vecGridSize.y + wRect.top;

    return TRUE;
}



static void sGetTextureFrameSize(
    const ImageTextureFrame & tImageTexFrame,
    Vector2D & vecTexDim)
{
    IRect2D rect;
    TextureGetFrameRectPixels(rect, tImageTexFrame.snoTexture, tImageTexFrame.nFrame);

    vecTexDim.x = (float)rect.Width();
    vecTexDim.y = (float)rect.Height();
}




static BOOL sDrawItemIconAtFullSize(
    const InventorySlotType eSlot)
{
    return (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END) ||
        (eSlot >= SLOT_HIRELING_START && eSlot < SLOT_HIRELING_END);
}







// NON_MATCHING: 440 vs 448 bytes; aspect-fit branches (2123-2131) are speculated into fcsel instead of the original branches; tried both comparison/branch orders and the 2131 operand order from its column (46 = '*').
static BOOL sGetItemRect(
    const ActorCommonData & tACDItem,
    const ImageTextureFrame & tImageTexFrame,
    const InventorySlotType eSlot,
    const IVector2D & tPoint,
    UIInventoryControl eInventoryControl,
    Rect2D & rectItem)
{

    if (!Prefetch::AddToQueue(SNOName(TEXTURE_SNO, tImageTexFrame.snoTexture), 2))
    {
        return FALSE;
    }

    Vector2D vecTexDim;
    sGetTextureFrameSize(tImageTexFrame, vecTexDim);

    Rect2D rectSlot;


    InventoryControlType eInventoryControlType = ICT_DEFAULT;


    if (!sGetSlotItemRect(&tACDItem, eInventoryControlType, eSlot, tPoint, eInventoryControl, FALSE, rectSlot))
    {
        return FALSE;
    }

    Vector2D vecSlotDim(rectSlot.Width(), rectSlot.Height());

    Vector2D vecTexSizeToUse = vecTexDim;


    if (!sDrawItemIconAtFullSize(eSlot) &&
        (vecSlotDim.x < vecTexDim.x || vecSlotDim.y < vecTexDim.y))
    {

        float flSlotAspectRatio = vecSlotDim.x / vecSlotDim.y;
        float flTexAspectRatio = vecTexDim.x / vecTexDim.y;
        if (flTexAspectRatio > flSlotAspectRatio)
        {
            vecTexSizeToUse.x = vecSlotDim.x;
            vecTexSizeToUse.y = vecSlotDim.x / flTexAspectRatio;
        }
        else
        {
            vecTexSizeToUse.y = vecSlotDim.y;
            vecTexSizeToUse.x = vecSlotDim.y * flTexAspectRatio;
        }
    }


    rectItem.left = rectSlot.left + (vecSlotDim.x - vecTexSizeToUse.x) * 0.5f;
    rectItem.right = rectItem.left + vecTexSizeToUse.x;
    rectItem.top = rectSlot.top + (vecSlotDim.y - vecTexSizeToUse.y) * 0.5f;
    rectItem.bottom = rectItem.top + vecTexSizeToUse.y;

    return TRUE;





}




void UIInventory::Show(const BOOL fShow)
{
}








































































































































































































ACDID UIInventory::GetHighlightedItem()

{

    CursorState eCursorState = PlayerGetCursorMgr().GetCursorState();
    if (eCursorState == CURSORSTATE_MOVE_ITEM || eCursorState == CURSORSTATE_BUY_ITEM)
        return ACDID_INVALID;





    UIID uId;
    InventorySlotType eSlot;
    IVector2D tSlot;
    UIInventoryControl eContext;
    if (!GetMouseOverSlot(uId, eSlot, tSlot, eContext))
        return ACDID_INVALID;



    if (eContext == INVENTORY_EQUIPMENT_MANAGER)
        return ACDID_INVALID;



    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer == ACDID_INVALID)
        return ACDID_INVALID;



    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    ACDID idACDInventory = sGetSlotOwner(tACDPlayer.id, eSlot);
    if (!ACDTryToGet(idACDInventory))
        return ACDID_INVALID;



    InventoryLocation tInvLoc;
    ACDInventoryLocationSet(tInvLoc, idACDInventory, eSlot, tSlot.x, tSlot.y);

    ACDID idACDHighlightedItem = ACDInventoryGetItemInSlot(tInvLoc);
    if (idACDHighlightedItem == ACDID_INVALID && ACDInventoryIsSlotHireling(eSlot))
    {
        ACDID idACDHireling = PlayerGetHireling(tACDPlayer.id);
        ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
        if (!ptACDHireling)
            return ACDID_INVALID;



        tInvLoc.idACDContainer = idACDHireling;
        idACDHighlightedItem = ACDInventoryGetItemInSlot(tInvLoc);
    }
    return idACDHighlightedItem;
}




void UIInventory::DrawItemSockets(
    ActorCommonData & tACD,
    XPooledDataIDList & listSocketedACDs,
    const PlayerClass eClassToDisplay,
    const BOOL fIsMale,
    const Rect2D & rectItemImage,
    const BOOL fEquipped,
    const float flAlpha,
    float & flZ)
{

    int32 nNumSockets = tACD.AttributesGetInt(ATTRIBUTE_SOCKETS);

    ImageTextureFrame tImageTexFrameSocket;
    ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, "GemSocket32", tImageTexFrameSocket);

    Vector2D vecSocketDim;
    vecSocketDim.x = vecSocketDim.y = fEquipped ? 32.0f : 28.0f;








    Vector2D vecSocketCenter;
    vecSocketCenter.x = rectItemImage.left + rectItemImage.Width() * 0.5f;
    vecSocketCenter.y = rectItemImage.top + rectItemImage.Height() * 0.5f;

    float flSocketLeft = vecSocketCenter.x - vecSocketDim.x * 0.5f;
    float flSocketTop = vecSocketCenter.y - vecSocketDim.y * nNumSockets * 0.5f;

    RGBAColor rgbaColor(255, 255, 255, (uint8)(flAlpha * 255.0f));

    POS pos = listSocketedACDs.GetHeadPosition();
    for (int32 nSocket = 0; nSocket < nNumSockets; ++nSocket)
    {
        Rect2D rectSocket;
        rectSocket.left = flSocketLeft;
        rectSocket.top = flSocketTop + vecSocketDim.y * nSocket;
        rectSocket.right = rectSocket.left + vecSocketDim.x;
        rectSocket.bottom = rectSocket.top + vecSocketDim.y;

        Rect2D rectSocketScreen;
        ConvertUICToPixels(rectSocket, rectSocketScreen);

        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
        SpriteDrawDeferred(tImageTexFrameSocket, snoShader, rectSocketScreen, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
        flZ -= UIDC_CONTROL_Z_INCREMENT;


        if (pos)
        {
            ACDID idACDSocketedItem = ACDID(listSocketedACDs.GetNext(pos));
            ActorCommonData * ptACDSocketedItem = ACDTryToGet(idACDSocketedItem);
            if (ptACDSocketedItem)
            {
                ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
                ACDInventoryGetItemDrawInfo(*ptACDSocketedItem, tImageTexFrame, ITEMDRAW_SOCKETED, eClassToDisplay, fIsMale);
                SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, snoShader, rectSocketScreen, rgbaColor, crgbaBlack, flZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
                flZ -= UIDC_CONTROL_Z_INCREMENT;
            }
        }
    }
}




static void sDrawItemStackSize(
    uint64 uStackSize,
    const Rect2D & rectItemImageUIC,
    float & flZ)
{
    CHAR uzTemp[32];
    Blizzard::String::Format(uzTemp, sizeof(uzTemp), "%llu", uStackSize);

    TextRenderProperties tProps(GlobalSNOGet(GLOBALSNO_FONT_SERIF), 16, 1.0f, 1.0f);
    Vector2D vecTextSizeUIC;
    FontStringGetRenderedSizeUIC(tProps, FormattedText(uzTemp, FONTCOLOR_GOLD, FALSE), vecTextSizeUIC);

    Vector2D vecJustifyUIC;
    vecJustifyUIC.Set(vecTextSizeUIC.x + 4.0f, vecTextSizeUIC.y + 4.0f);

    Vector2D vecUIC;
    vecUIC.Set(rectItemImageUIC.right - vecJustifyUIC.x, rectItemImageUIC.bottom - vecJustifyUIC.y);

    FontStringDrawUIC(tProps, FormattedText(uzTemp, FONTCOLOR_GOLD, FALSE), vecUIC, crgbaWhite, &crgbaBlack, flZ, TRUE, NULL, RENDERLAYER_INVALID, FALSE);
    flZ -= UIDC_CONTROL_Z_INCREMENT;
}



static void sDrawHighlightHalo(
    const Rect2D & rectSlotPixels,
    const RGBAColor & rgbaColor,
    const InventorySlotType eSlot,
    float & flZ)
{

    if (rgbaColor == RGBAColor(0, 0, 0, 0))
        return;



    CRefString sLabel = ActorInventoryIsLargeSlot(eSlot) ? "ItemSelectLarge" : "ItemSelectSmall";
    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    if (!ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sLabel, tImageTexFrame))
        return;



    SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectSlotPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
    flZ -= UIDC_CONTROL_Z_INCREMENT;
}




void UIInventory::DrawItemOverlay(
    const Rect2D & rectSlotUIC,
    const UIItemOverlay eOverlay,
    const float flAlpha,
    float & flZ,
    const Rect2D * const pwrClipping)
{
    CRefString sOverlay;
    switch (eOverlay)
    {
    case ITEMOVERLAY_UNIDENTIFIED:
        sOverlay = "ItemStatus_Unidentified";
        break;
    case ITEMOVERLAY_UNUSABLE:
        sOverlay = "ItemStatus_Unusable";
        break;
    case ITEMOVERLAY_LOCKED:
        sOverlay = "ItemStatus_Unusable_Locked";
        break;
    case ITEMOVERLAY_RANDOM_ITEM_PLACEHOLDER:
        sOverlay = "BloodshardVendor_QuestionMark";
        break;
    default:
        return;
    }


    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    if (!ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sOverlay, tImageTexFrame))
        return;



    RGBAColor rgbaColor(255, 255, 255, (uint8)(flAlpha * 255.0f));

    Vector2D vecTexDim;
    sGetTextureFrameSize(tImageTexFrame, vecTexDim);


    Rect2D rectOverlayUIC;
    rectOverlayUIC.left = rectSlotUIC.left + (rectSlotUIC.Width() - vecTexDim.x) * 0.5f;
    rectOverlayUIC.right = rectOverlayUIC.left + vecTexDim.x;
    rectOverlayUIC.top = rectSlotUIC.top + (rectSlotUIC.Height() - vecTexDim.y) * 0.5f;
    rectOverlayUIC.bottom = rectOverlayUIC.top + vecTexDim.y;

    Rect2D rectOverlayPixels;
    ConvertUICToPixels(rectOverlayUIC, rectOverlayPixels);

    Rect2D rectClippingPixels;
    if (pwrClipping)
    {
        ConvertUICToPixels(*pwrClipping, rectClippingPixels);
        SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectOverlayPixels, rgbaColor, crgbaBlack, flZ, &rectClippingPixels, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
    }
    else
    {
        SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectOverlayPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
    }
    flZ -= UIDC_CONTROL_Z_INCREMENT;
}



static void sDrawBackpackSlotBkgrnd(
    const InventorySlotType eSlot,
    const IVector2D & tPoint,
    const UIInventoryControl eInventoryControl,
    float flZ)
{



    Rect2D rectSlotUIC;
    if (!UIInventory::GetSlotRect(ICT_DEFAULT, eSlot, tPoint, eInventoryControl, FALSE, rectSlotUIC))
        return;




    Rect2D rectSlotPixels;
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);

    SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);


    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    CRefString sSlotImage;
    switch (eSlot)
    {
    case SLOT_PLAYER_BACKPACK:
        sSlotImage = "UI:InventoryGrid_Slot";
        break;
    case SLOT_PLAYER_SHARED_STASH:
        sSlotImage = "UI:Stash_Slot_Empty";
        break;
    }


    if (!ImageCollectionGetImageInfo(sSlotImage, tImageTexFrame))
        return;




    flZ += UIDC_CONTROL_Z_INCREMENT * 2;
    SpriteDrawDeferred(tImageTexFrame, snoShader, rectSlotPixels, crgbaWhite, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
}


// Inference: Switch's surviving page lookup only assigns UIID none (DWARF line 2652).
static void sSlotGetPage(
    InventorySlotType eSlot,
    UIID & idSlot0)
{

    idSlot0 = GetUIIDNone();
}






























static BOOL sSlotIsOnVisiblePage(
    InventorySlotType eSlot)
{
    UIID idSlot;
    sSlotGetPage(eSlot, idSlot);

    return idSlot != GetUIIDNone() && UI_GetControlFromID(idSlot).IsActive();
}








// NON_MATCHING: 192 vs 192 bytes; visible-page flag load is ldrh versus original ldrb after genuine halo callers; retained canonical UIControl flag access.
void UIInventory::DrawSlotHighlights(
    InventorySlotType eSlot,
    const RGBAColor & rgbaColor,
    float & flZ,
    UIInventoryControl eInventoryControl)
{
    if (!sSlotIsOnVisiblePage(SLOT_PLAYER_BACKPACK))
        return;



    Rect2D rectSlotUIC;
    if (!sGetSlotItemRect(NULL, ICT_DEFAULT, eSlot, cgptNull, eInventoryControl, FALSE, rectSlotUIC))
        return;




    Rect2D rectSlotPixels;
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);


    sDrawHighlightHalo(rectSlotPixels, rgbaColor, eSlot, flZ);
}



















BOOL UIInventory::GetItemSlotEmptyImage(const ActorCommonData * ptACD, const ActorCommonData * ptACDItem, const InventorySlotType eSlot, ImageTextureFrame & tImageTexFrame)
{
    if (ptACDItem == NULL && eSlot != SLOT_HIRELING_RIGHT_HAND && eSlot != SLOT_PLAYER_RIGHT_HAND)
        return FALSE;

    CRefString sEmptySlotLabel("");
    switch (eSlot)
    {
    case SLOT_PLAYER_HEAD:
    case SLOT_HIRELING_HEAD:
        sEmptySlotLabel = "Grid_EmptySocket_Head";
        break;
    case SLOT_PLAYER_TORSO:
    case SLOT_HIRELING_TORSO:
        sEmptySlotLabel = "Grid_EmptySocket_Chest";
        break;
    case SLOT_PLAYER_RIGHT_HAND:
    case SLOT_HIRELING_RIGHT_HAND:
        if (ptACDItem)
            sEmptySlotLabel = "Grid_EmptySocket_OffHand";
        else if (ptACD)
        {
            IVector2D tPoint(0, 0);
            InventorySlotType eOppositeSlot = ACDInventoryGetOppositeTwoHandedSlot(eSlot);
            ACDID idACDOppositeItem = ACDInventoryGetSquareItem(*ptACD, eOppositeSlot, tPoint);
            ActorCommonData * ptACDOppositeItem = ACDTryToGet(idACDOppositeItem);
            if (ptACDOppositeItem && ItemIsTwoHanded(*ptACDOppositeItem) &&
                ptACD->AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD) == 0)
                sEmptySlotLabel = "Grid_EmptySocket_OffHand";
        }
        break;
    case SLOT_PLAYER_LEFT_HAND:
    case SLOT_HIRELING_LEFT_HAND:
        sEmptySlotLabel = "Grid_EmptySocket_MainHand";
        break;
    case SLOT_PLAYER_HANDS:
    case SLOT_HIRELING_HANDS:
        sEmptySlotLabel = "Grid_EmptySocket_Glove";
        break;
    case SLOT_PLAYER_FEET:
    case SLOT_HIRELING_FEET:
        sEmptySlotLabel = "Grid_EmptySocket_Feet";
        break;
    case SLOT_PLAYER_SHOULDERS:
    case SLOT_HIRELING_SHOULDERS:
        sEmptySlotLabel = "Grid_EmptySocket_Shoulder";
        break;
    case SLOT_PLAYER_LEGS:
    case SLOT_HIRELING_LEGS:
        sEmptySlotLabel = "Grid_EmptySocket_Leg";
        break;
    case SLOT_PLAYER_LEFT_FINGER:
    case SLOT_HIRELING_LEFT_FINGER:
        sEmptySlotLabel = "Grid_EmptySocket_RingLeft";
        break;
    case SLOT_PLAYER_RIGHT_FINGER:
    case SLOT_HIRELING_RIGHT_FINGER:
        sEmptySlotLabel = "Grid_EmptySocket_RingRight";
        break;
    case SLOT_PLAYER_NECK:
    case SLOT_HIRELING_SPECIAL:
    case SLOT_HIRELING_NECK:
        sEmptySlotLabel = "Grid_EmptySocket_Neck";
        break;
    case SLOT_PLAYER_WAIST:
    case SLOT_HIRELING_WAIST:
        sEmptySlotLabel = "Grid_EmptySocket_Belt";
        break;
    case SLOT_PLAYER_BRACERS:
    case SLOT_HIRELING_BRACERS:
        sEmptySlotLabel = "Grid_EmptySocket_Bracer";
        break;
    default:
        return FALSE;
    }

    if (sEmptySlotLabel.Length())
        return ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, sEmptySlotLabel, tImageTexFrame);

    return FALSE;
}





























void UIInventory::DrawEmptyPaperDollSlot(const ActorCommonData * ptACD, const ActorCommonData * ptACDItem, const InventorySlotType eSlot, float flZ, const UIInventoryControl eInventoryControl)
{





    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    if (!GetItemSlotEmptyImage(ptACD, ptACDItem, eSlot, tImageTexFrame))
        return;





    Rect2D rectSlotUIC;
    if (!GetSlotRect(ICT_DEFAULT, eSlot, cgptNull, eInventoryControl, FALSE, rectSlotUIC))
        return;



    Rect2D rectSlotPixels;
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);


    flZ += UIDC_CONTROL_Z_INCREMENT * 2;
    SpriteDrawDeferred(tImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectSlotPixels, crgbaWhite, crgbaBlack,
        flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
}
static void sDrawWhereCanThisGoHighlight(
    const ActorCommonData & tACD,
    const ActorCommonData & tACDHighlightedItem,
    const InventorySlotType eSlot,
    float flZ,
    UIInventoryControl eInventoryControl)
{
    InventorySlotType eHighlightedSlot = tACDHighlightedItem.GetInventoryLocation().eSlot;
    if (eHighlightedSlot != SLOT_PLAYER_BACKPACK && eHighlightedSlot != SLOT_PLAYER_SHARED_STASH)
    {
        return;
    }


    if (!ACDInventoryItemAllowedInThisSlotType(tACDHighlightedItem.hGB.gbid, eSlot))
    {
        return;
    }


    if (eSlot == SLOT_PLAYER_RIGHT_HAND &&
        ItemIsTwoHanded(tACDHighlightedItem) &&
        tACD.AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD))
    {
        return;
    }

    RGBAColor rgbaColorHighlight = crgbaWhite;
    InventoryLocation tInvLoc;
    ACDInventoryLocationSet(tInvLoc, tACD.id, eSlot, 0, 0);
    if (ACDInventoryItemAllowedInSlot(tACDHighlightedItem, tInvLoc, FALSE, FALSE) != -1)
    {
        const RGBAColor sgrgbaUnusableHighlight(255, 0, 0, 128);  // not static: the DWARF DIE has no location and the CU has no such data symbol; the RGBAColor ctor is not constant (UIHireling initialises its file statics dynamically)
        rgbaColorHighlight = sgrgbaUnusableHighlight;
    }


    Rect2D rectSlotUIC;
    if (!UIInventory::GetSlotRect(ICT_DEFAULT, eSlot, cgptNull, eInventoryControl, FALSE, rectSlotUIC))
    {
        return;
    }
    Rect2D rectSlotPixels;
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);

    flZ += UIDC_CONTROL_Z_INCREMENT * 2;  // original fadd s8, s0, s0 of UIDC_CONTROL_Z_INCREMENT feeds this add (as at 2641/2871)
    sDrawHighlightHalo(rectSlotPixels, rgbaColorHighlight, eSlot, flZ);
}


extern "C" BOOL sub_7100505c00(const ActorCommonData & tACDItem);
RGBAColor sGetHighlightColorFromQualityLevel(const ActorCommonData & tACDItem)
{
    ItemQualityLevel eIQL = ItemGetDisplayedQualityLevel(tACDItem);

    if (UIInventory::IsMagicColor(eIQL))
    {
        return crgbaBlue;
    }
    else if (UIInventory::IsRareColor(eIQL))
    {
        return crgbaYellow;
    }
    else if (eIQL == IQL_DISPLAY_AS_SET_ITEM)
    {
        return crgbaGreen;
    }
    else if (UIInventory::IsLegendaryColor(eIQL))
    {
        if (sub_7100505c00(tACDItem))
            return unk_7100ed3648;
        return crgbaOrange;
    }
    else if (ItemHasLabel(tACDItem.hGB.gbid, ITL_COLLECTIBLE))
    {
        return FontGetColorForFontSpecifier(FONTCOLOR_COLLECTIBLE, FALSE);
    }

    return crgbaWhite;
}







static void sGetHighlightColor(
    RGBAColor & rgbaColor,
    const ActorCommonData & tACDItem,
    const BOOL fItemIsHighlighted)
{
    rgbaColor = sgrgbaNone;
    if (ItemIsBroken(tACDItem) || ItemIsEquippedButDisabled(tACDItem))
    {
        rgbaColor = sgrgbaBroken;
        return;
    }
    if (ItemIsDamaged(tACDItem))
    {
        rgbaColor = sgrgbaYellow;
        return;
    }


    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
    if (ptInventoryGlobals)
    {
        POS pos = ptInventoryGlobals->tUIHighlightList.GetHeadPosition();
        while (pos)
        {
            POS posOld = pos;
            TempHighlight & tHighlight = ptInventoryGlobals->tUIHighlightList.GetNext(pos);

            if (tHighlight.GetItem() == tACDItem.id)
            {
                BOOL fRemove = FALSE;





                rgbaColor = sGetHighlightColorFromQualityLevel(tACDItem);

                if (tHighlight.ShouldFade())
                {
                    rgbaColor.a = tHighlight.GetFadeValue();

                    fRemove = rgbaColor.a == 0;
                }



                if (fRemove || fItemIsHighlighted)
                {
                    ptInventoryGlobals->tUIHighlightList.RemoveAt(posOld);
                }
            }
        }
    }

    CursorMgr & tCursorMgr = PlayerGetCursorMgr();

    ActorCommonData * ptACDHandItem = tCursorMgr.GetItemToMove();
    if (ptACDHandItem &&
        ItemIsSocketable(*ptACDHandItem) &&
        ItemHasSockets(tACDItem))
    {

        int32 nSocketsFilled = ItemGetNumFilledSockets(tACDItem);
        int32 nSocketsTotal = tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS);
        if (nSocketsTotal > nSocketsFilled)
        {
            rgbaColor = sgrgbaWhite;
        }
    }

    if (fItemIsHighlighted &&


            !ItemIsBeingManipulated(tACDItem))
    {
        rgbaColor = sgrgbaYellow;
    }
}






BOOL UIInventory::GetItemBkgrndImage(const ItemQualityLevel eIQL, ImageTextureFrame & tImageTexFrame, const BOOL fEquipped, const BOOL fLargeItem, const int32 nAncientRank, const BOOL fSeasonOnly, const BOOL fSanctified, const BOOL fPrimalized)
{




    CRefString sLabel("UI:Console_ItemQuality");

    if (IsMagicColor(eIQL))

        sLabel += "Magic";

    else if (IsRareColor(eIQL))

        sLabel += "Rare";

    else if (eIQL == IQL_DISPLAY_AS_SET_ITEM)

        sLabel += "Set";

    else if (IsLegendaryColor(eIQL))

        sLabel += "Legendary";

    else

        sLabel += "Common";

    if (fPrimalized)
        sLabel += "_Primalized";
    else if (fSanctified)
        sLabel += "_Sanctified";
    else if (fSeasonOnly)
        sLabel += "_SeasonOnly";

    ImageCollectionGetImageInfo(sLabel, tImageTexFrame);






































    return TRUE;
}



extern "C" BOOL sub_7100505c90(const ActorCommonData & tACDItem);
extern "C" BOOL sub_710050ece0(const ActorCommonData & tACDItem);

static void sDrawItemBackground(
    const ActorCommonData & tACDItem,
    const Rect2D & rectSlotPixels,
    const RGBAColor & rgbaColor,
    const BOOL fEquipped,
    float & flZ)
{
    BOOL fLargeItem = ItemIsDrawnAsLargeSlotItem(tACDItem.hGB.gbid);
    int32 nAncientRank = ItemGetAncientRank(tACDItem);
    BOOL fSeasonOnly = sub_7100505c00(tACDItem);
    BOOL fSanctified = sub_7100505c90(tACDItem);
    BOOL fPrimalized = sub_710050ece0(tACDItem);

    ImageTextureFrame tImageTexFrameBkgrnd;
    BOOL fFoundImage = UIInventory::GetItemBkgrndImage(ItemGetDisplayedQualityLevel(tACDItem), tImageTexFrameBkgrnd, fEquipped, fLargeItem, nAncientRank, fSeasonOnly, fSanctified, fPrimalized);


    SpriteDrawDeferred(tImageTexFrameBkgrnd, cgtImageTextureFrameNull, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectSlotPixels, rgbaColor, crgbaBlack,
        flZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
    flZ -= UIDC_CONTROL_Z_INCREMENT;
}





static void sDrawItemFlashOverlay(
    const ActorCommonData & tACDItem,
    const Rect2D & rectItemUIC,
    float & flZ)
{
    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();
    if (!ptInventoryGlobals || ptInventoryGlobals->tUIFlashOverlays.Count() == 0)
        return;



    POS pos = ptInventoryGlobals->tUIFlashOverlays.GetHeadPosition();
    while (pos)
    {
        POS oldPos = pos;
        FlashOverlayInfo & tInfo = ptInventoryGlobals->tUIFlashOverlays.GetNext(pos);
        if (tInfo.GetItem() == tACDItem.id)
        {
            uint8 bAlpha;
            if (!tInfo.GetAlpha(bAlpha))
            {


                ptInventoryGlobals->tUIFlashOverlays.RemoveAt(oldPos);
                continue;
            }


            ImageTextureFrame tImageTexFrame;
            ImageCollectionGetImageInfo("UI:SlotFlash_Generic", tImageTexFrame);

            // Inference: 132x132 flash centred on the item; original adds constants -66/+66 at 3174-3177.


            Vector2D vecItemCenterPoint = rectItemUIC.Center();
            Rect2D rectUIC;
            rectUIC.Set(
                vecItemCenterPoint.x - 66.0f,
                vecItemCenterPoint.y - 66.0f,
                vecItemCenterPoint.x + 66.0f,
                vecItemCenterPoint.y + 66.0f);

            Rect2D rectPixels;
            ConvertUICToPixels(rectUIC, rectPixels);

            RGBAColor rgbaColor(255, 255, 255, bAlpha);
            SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectPixels, rgbaColor, crgbaBlack,
                flZ, 1.0f, NULL, RENDERLAYER_INVALID, FALSE);
            flZ -= UIDC_CONTROL_Z_INCREMENT;
        }
    }
}





void UIInventory::UpdateItemManipulationTimers()
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    ActorCommonData * ptACDCursorItem = NULL;
    if (eCursorState == CURSORSTATE_MOVE_ITEM)
        ptACDCursorItem = tCursorMgr.GetItemToMove();
    else if (eCursorState == CURSORSTATE_SPLIT_STACK)
        ptACDCursorItem = tCursorMgr.GetStackToSplit();

    if (ptACDCursorItem != NULL)
        ItemSetIsBeingManipulated(*ptACDCursorItem, TRUE, 120);
}










// NON_MATCHING: 3720 vs 3772 bytes; original frame is 16 bytes larger (split stp/sub prologue) with different spills, the swap-indicator range test stays a select+tbnz instead of b.hi, and the highlight-removal test computes a==0|fItemIsHighlighted where the original hoists !fItemIsHighlighted; tried ||-order and set-flag forms.
void UIInventory::SlotDrawItemInSlot(
    ActorCommonData * ptACDOwner,
    ActorCommonData & tACDItem,
    XPooledDataIDList & listSocketedACDs,
    const BOOL fHighlighted,
    BOOL fEnabled,
    BOOL fGhosted,
    const PlayerClass eClassToDisplay,
    const BOOL fIsMale,
    const InventoryLocation & tInvLoc,
    float & flZ,
    UIInventoryControl eInventoryControl)
{
    InvGlobals * ptInventoryGlobals = UI_GetInventoryData();

    BOOL fIsVariableSizedSlot = ActorInventoryIsVariableSizedSlot(tInvLoc.eSlot);
    BackpackTimers * ptBackpackTimers = NULL;
    if (eInventoryControl == INVENTORY_MAIN)
    {


        switch (tInvLoc.eSlot)
        {
        case SLOT_PLAYER_BACKPACK:
            ptBackpackTimers = &ptInventoryGlobals->tBackpackTimers;
            break;
        case SLOT_PLAYER_SHARED_STASH:

            ptBackpackTimers = &ptInventoryGlobals->tSharedStashTimers;
            break;
        }
    }

    ActorCommonData * ptACDCursorItem = NULL;
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    if (eCursorState == CURSORSTATE_MOVE_ITEM)
    {
        ptACDCursorItem = tCursorMgr.GetItemToMove();
    }
    else if (eCursorState == CURSORSTATE_SPLIT_STACK)
    {
        ptACDCursorItem = tCursorMgr.GetStackToSplit();
    }


    ImageTextureFrame tImageTexFrame = cgtImageTextureFrameNull;
    ACDInventoryGetItemDrawInfo(tACDItem, tImageTexFrame, FALSE, eClassToDisplay, fIsMale);


    Rect2D rectItemUIC;
    if (!sGetItemRect(tACDItem, tImageTexFrame, tInvLoc.eSlot, tInvLoc.pt, eInventoryControl, rectItemUIC))
    {
        return;
    }

    Rect2D rectItemPixels;
    ConvertUICToPixels(rectItemUIC, rectItemPixels);

    Rect2D rectSlotPixels;
    Rect2D rectSlotUIC;
    sGetSlotItemRect(&tACDItem, 0, tInvLoc.eSlot, tInvLoc.pt, eInventoryControl, FALSE, rectSlotUIC);
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);

    float flItemAlpha = 1.0f;

    {

        RGBAColor rgbaSpriteColor = crgbaWhite;

        if (eInventoryControl != INVENTORY_EQUIPMENT_MANAGER && eInventoryControl != INVENTORY_GENERIC_PLAYER_INSPECT)
        {
            if (ACDInventorySlotIsEquippable(tInvLoc.eSlot) && !ItemIsEquipped(tACDItem)) fEnabled = FALSE;
        }


        if (!fEnabled)
        {
            rgbaSpriteColor = sgrgbaDisabled;
        }
        else if (fGhosted)
        {
            rgbaSpriteColor = crgbaWhite;
            rgbaSpriteColor.a = 128;
        }






        if (tACDItem.hGB.gbid != GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH))
        {

            sDrawItemBackground(tACDItem, rectSlotPixels, rgbaSpriteColor, !fIsVariableSizedSlot, flZ);
        }

        {

            RGBAColor rgbaColor;
            sGetHighlightColor(rgbaColor, tACDItem, fHighlighted);
            sDrawHighlightHalo(rectSlotPixels, rgbaColor, tInvLoc.eSlot, flZ);
        }

        Rect2D rectItemJiggledPixels = rectItemPixels;



        if (eInventoryControl == INVENTORY_MAIN)
        {
            sDrawItemFlashOverlay(tACDItem, rectItemUIC, flZ);


            int32 nIndicatorIndex = sGetSwapIndicatorIndex(tInvLoc.eSlot);
            if (nIndicatorIndex >= 0)
            {
                Vector2D vecJiggle;
                vecJiggle = ptInventoryGlobals->ptSwapIndicators[nIndicatorIndex].GetJiggleOffset();
                rectItemPixels.Offset(vecJiggle, rectItemJiggledPixels);
            }


            if (tACDItem.id == ptInventoryGlobals->tTutorialInfo.idACDItem &&
                ptInventoryGlobals->tTutorialInfo.snoTutorialOnOpen != SNO_NONE)
            {
                gtTutorialMgr.ShowTutorial(rectSlotUIC, ptInventoryGlobals->tTutorialInfo.snoTutorialOnOpen, NULL, TRUE);
            }
        }


        SpriteDrawDeferred(tImageTexFrame, cgtImageTextureFrameNull, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectItemJiggledPixels, rgbaSpriteColor, crgbaBlack,
            flZ, flItemAlpha, NULL, RENDERLAYER_INVALID, FALSE);
        flZ -= UIDC_CONTROL_Z_INCREMENT;
    }


    if (ItemHasSockets(tACDItem))
    {
        float flSocketAlpha = fHighlighted ? 1.0f : 0.5f;
        DrawItemSockets(tACDItem, listSocketedACDs, eClassToDisplay, fIsMale, rectItemUIC, !fIsVariableSizedSlot, flSocketAlpha, flZ);
    }










    UIItemOverlay eOverlay = ptACDOwner ? ItemGetOverlay(tACDItem, *ptACDOwner) : ITEMOVERLAY_NONE;
    DrawItemOverlay(rectSlotUIC, eOverlay, 1.0f, flZ, NULL);






    ImageTextureFrame tTransmogImageTexFrame = cgtImageTextureFrameNull;
    GBID gbidTransmog = tACDItem.AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    if (gbidTransmog != GBID_NONE && ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, "ItemStatus_Transmogrify", tTransmogImageTexFrame))
    {

        Vector2D vecTransmogTexDim;
        sGetTextureFrameSize(tTransmogImageTexFrame, vecTransmogTexDim);

        float flLeft = rectSlotPixels.right - (vecTransmogTexDim.x + 3.0f);
        float flTop = rectSlotPixels.top + 3.0f;

        RGBAColor rgbaColor(255, 255, 255, 255);
        Rect2D rectTransmogOverlayPixels(flLeft, flTop, flLeft + vecTransmogTexDim.x, flTop + vecTransmogTexDim.y);
        SpriteDrawDeferred(tTransmogImageTexFrame, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectTransmogOverlayPixels, rgbaColor, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
        flZ -= UIDC_CONTROL_Z_INCREMENT;
    }


    if (Prefs::GetGameplayPrefs().GetItemRarityIcons())
    {
        Vector2D vecOffsetPixels(-3.0f, -3.0f);
        DrawItemRarityIcon(tACDItem.hGB.gbid, 1.0f, flZ, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectSlotPixels, vecOffsetPixels, NULL, sub_710050ece0(tACDItem));
    }





    if (ptACDOwner && eInventoryControl == INVENTORY_MAIN && fIsVariableSizedSlot && ptBackpackTimers)
    {

        IVector2D tSize;
        ACDInventoryGetSlotPageSize(tInvLoc.eSlot, tSize);

        int32 nSlotYOffset = 0;
        if (tInvLoc.eSlot == SLOT_PLAYER_SHARED_STASH)
        {
            nSlotYOffset = GetStashOffset();
        }

        int32 nSlotIndex = tInvLoc.pt.x + (tInvLoc.pt.y - nSlotYOffset) * tSize.x;

        UIID idTimer = ptBackpackTimers->pidTimers[nSlotIndex];
        UITimer * ptTimer = UI_GetTimerFromID(idTimer);



        UpdateTimerVisibility(*ptTimer, *ptACDOwner, tACDItem);
        if (!ptTimer->IsFinished())
        {
            SpriteSubmitTimerControlSprite(ptTimer->GetID(), flZ);
            flZ -= UIDC_CONTROL_Z_INCREMENT;
        }
    }



    uint64 uStackSize = tACDItem.GetStackCount();
    if (uStackSize > 1 && uStackSize != ((uint64)1 << 63))
    {
        if (eCursorState == CURSORSTATE_SPLIT_STACK && ptACDCursorItem && tACDItem.id == ptACDCursorItem->id)
        {
            uStackSize -= tCursorMgr.GetStackSplitAmount();
        }




        sDrawItemStackSize(uStackSize, rectItemUIC, flZ);
    }


    if (ItemIsUpgradeableJewel(tACDItem.hGB.gbid))
    {
        int32 nRank = tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
        DrawLegendaryGemRank(nRank, flZ, rectItemUIC, NULL);
    }




    if (ptACDOwner &&
        (tInvLoc.eSlot == SLOT_HIRELING_LEFT_HAND || tInvLoc.eSlot == SLOT_PLAYER_LEFT_HAND) &&
        ItemIsTwoHanded(tACDItem))
    {


        InventorySlotType eOppositeSlot = ACDInventoryGetOppositeTwoHandedSlot(tInvLoc.eSlot);
        ACDID idACDOppositeItem = ACDInventoryGetSquareItem(*ptACDOwner, eOppositeSlot, tInvLoc.pt);

        if (idACDOppositeItem == ACDID_INVALID)
        {
            ImageTextureFrame tImageTexFrameOffHand = tImageTexFrame;
            RGBAColor rgbaColorOffHand(255, 128, 128, 255);
            PlayerClass ePlayerClass = PlayerGetPlayerClass(ptACDOwner->id);


            if (!ACDIsHireling(*ptACDOwner) &&
                ItemUsesQuiver(tACDItem) &&
                ItemMeetsOwnerClassRestrictions(GlobalGBIDGet(GLOBALGBID_ITEM_QUIVER), ePlayerClass, FALSE))
            {
                rgbaColorOffHand = crgbaWhite;
                ImageCollectionGetImageInfo(IMAGE_COLLECTION_UI, "Grid_EmptySocket_Quiver", tImageTexFrameOffHand);
            }

            if (!ptACDOwner->AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD))
            {




                if (sGetItemRect(tACDItem, tImageTexFrameOffHand, eOppositeSlot, tInvLoc.pt, eInventoryControl, rectSlotUIC))
                {
                    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);

                    SpriteDrawDeferred(tImageTexFrameOffHand, GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT), rectSlotPixels, rgbaColorOffHand, crgbaBlack, flZ, NULL, SNO_NONE, NULL, FALSE, RENDERLAYER_INVALID, FALSE);
                    flZ -= UIDC_CONTROL_Z_INCREMENT;
                }
            }
        }
    }
}



void UIInventory::SlotDrawVariableSlotBackground(
    const InventorySlotType eSlot,
    float flZ,
    const UIInventoryControl eInventoryControl,
    const int32 nSlots,
    const int32 nSlotYOffset)
{
    IVector2D tSize;
    ACDInventoryGetSlotPageSize(eSlot, tSize);

    BOOL fIsVariableSizedSlot = ActorInventoryIsVariableSizedSlot(eSlot);
    if (fIsVariableSizedSlot)
    {
        for (int32 y = 0; y < tSize.y; ++y)
        {
            for (int32 x = 0; x < tSize.x; ++x)
            {
                IVector2D tPoint(x, y + nSlotYOffset);
                int32 nSlotIndex = tPoint.y * tSize.x + tPoint.x;
                if (nSlotIndex >= nSlots)
                    continue;



                sDrawBackpackSlotBkgrnd(eSlot, tPoint, eInventoryControl, flZ);
            }
        }
    }
}



// 3569/3571/3575: MOVE_ITEM tested first (rows 3571:38 GetItemToMove, 3575:38 GetStackToSplit); 3603: control/highlight tests first (original ands eq&ne before the slot range).
void UIInventory::SlotDrawItems(
    ActorCommonData & tACD,
    const PlayerClass eClassToDisplay,
    const BOOL fIsMale,
    const InventorySlotType eSlot,
    float & flZ,
    UIInventoryControl eInventoryControl)
{




    IVector2D tSize;
    ACDInventoryGetSlotPageSize(eSlot, tSize);
    int32 nSlotYOffset = 0;
    if (eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        nSlotYOffset = GetStashOffset();
    }

    BOOL fIsVariableSizedSlot = ActorInventoryIsVariableSizedSlot(eSlot);
    int32 nBackpackSlots = 0;
    if (fIsVariableSizedSlot)
    {
        FastAttribKey eKeySlots = ActorInventoryGetVariableSizedSlotAttrib(eSlot);
        nBackpackSlots = tACD.AttributesGetInt(eKeySlots);
        SlotDrawVariableSlotBackground(eSlot, flZ, eInventoryControl, nBackpackSlots, nSlotYOffset);
    }


    ActorCommonData * ptACDCursorItem = NULL;
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    if (eCursorState == CURSORSTATE_MOVE_ITEM)
    {
        ptACDCursorItem = tCursorMgr.GetItemToMove();
    }
    else if (eCursorState == CURSORSTATE_SPLIT_STACK)
    {
        ptACDCursorItem = tCursorMgr.GetStackToSplit();
    }

    ACDID idACDHighlightedItem = GetHighlightedItem();


    for (int32 y = 0; y < tSize.y; ++y)
    {
        for (int32 x = 0; x < tSize.x; ++x)
        {

            IVector2D tPoint(x, y + nSlotYOffset);


            if (fIsVariableSizedSlot)
            {
                int32 nSlotIndex = x + tSize.x * y;
                if (nSlotIndex >= nBackpackSlots)
                {
                    continue;
                }
            }

            ACDID idACDItem = ACDInventoryGetSquareItem(tACD, eSlot, tPoint);

            ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
            DrawEmptyPaperDollSlot(&tACD, ptACDItem, eSlot, flZ, eInventoryControl);

            if (eInventoryControl == INVENTORY_MAIN && idACDHighlightedItem != ACDID_INVALID &&
                eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)
            {

                ActorCommonData & tACDHighlightedItem = ACDGet(idACDHighlightedItem);


                sDrawWhereCanThisGoHighlight(tACD, tACDHighlightedItem, eSlot, flZ, eInventoryControl);
            }

            if (!ptACDItem)
            {
                continue;
            }

            if (!GBIDExists(ptACDItem->hGB))
            {
                continue;
            }


            if (!Prefetch::AddToQueue(SNOName(ACTOR_SNO, ptACDItem->snoActor), 2))
            {
                continue;
            }

            BOOL fItemIsHighlighted = idACDHighlightedItem == ptACDItem->id;

            BOOL fGhosted = sItemShouldBeGhosted(*ptACDItem, ptACDCursorItem);
            InventoryLocation tInvLoc;
            tInvLoc.eSlot = eSlot;
            tInvLoc.pt = tPoint;

            DataIDMemoryPool * pMemoryPool;

            if (ThreadGlobals::Get().m_pDataIDMemoryPool)
            {
                pMemoryPool = ThreadGlobals::Get().m_pDataIDMemoryPool;
            }

            else
            {
                pMemoryPool = MemoryPoolsGetDATAIDListPool();
            }
            XPooledDataIDList listSocketedACDs(pMemoryPool);

            if (ItemHasSockets(*ptACDItem))
            {

                int32 nNumSockets = ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS);
                for (int32 nSocket = 0; nSocket < nNumSockets; ++nSocket)
                {
                    ACDID idACDSocketedItem = ACDInventoryGetSquareItem(*ptACDItem, SLOT_ITEM_SOCKETS, IVector2D(0, nSocket));
                    listSocketedACDs.AddTail_Tracked(idACDSocketedItem);
                }
            }

            SlotDrawItemInSlot(&tACD, *ptACDItem, listSocketedACDs, fItemIsHighlighted, TRUE, fGhosted, eClassToDisplay, fIsMale, tInvLoc, flZ, eInventoryControl);
        }
    }
}



static BOOL sFindTopLeftPoint(
    const ActorCommonData & tACD,
    InventorySlotType eSlot,
    const ACDID idACDItem,
    IVector2D & tSlot)
{

    IVector2D tSize;
    ACDInventorySlotGetSize(eSlot, tSize);


    for (int32 y = 0; y < tSize.y; ++y)
    {
        for (int32 x = 0; x < tSize.x; ++x)
        {
            tSlot.Set(x, y);
            ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACD, eSlot, tSlot);
            if (idACDSquareItem == idACDItem)
                return TRUE;
        }
    }

    return FALSE;
}





static BOOL sTestSlotIsValidForACD(
    const ActorCommonData & tACD,
    InventorySlotType eSlot)
{

    if (eSlot < SLOT_HIRELING_START || eSlot >= SLOT_HIRELING_END)
    {
        return TRUE;
    }


    HirelingClass eHireling = HirelingGetClass(tACD);
    if (eHireling == 0)
    {

        ACDID idACDHireling = PlayerGetHireling(tACD.id);
        if (idACDHireling != ACDID_INVALID)
        {
            ActorCommonData & tACDHireling = ACDGet(idACDHireling);
            eHireling = HirelingGetClass(tACDHireling);
            if (eHireling == 0)
            {
                return TRUE;
            }
        }
    }


    return ACDInventoryIsSlotValidForHireling(tACD, eHireling, eSlot);
}




static void sAdjustSelectedSlotForItem(const ActorCommonData & tACDItem, InventorySlotType eSlot, IVector2D & tSlot)
{



    int32 nSlotYOffset = 0;
    if (eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        nSlotYOffset = UIInventory::GetStashOffset();
    }




    if (tSlot.y > nSlotYOffset &&
        ACDInventorySlotIsMultiItemSlot(eSlot) &&
        ItemIsDrawnAsLargeSlotItem(tACDItem.hGB.gbid))
    {

        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, tACDItem.id, eSlot, tSlot.x, tSlot.y);

        if (ActorInventoryIsVariableSizedSlot(tInvLocDest.eSlot))
        {
            if (!ActorInventoryIsVariableSizedSlotValid(tInvLocDest))
            {
                tSlot.y--;
            }
        }
        else
        {

            IVector2D tSize;

            ACDInventorySlotGetSize(tInvLocDest.eSlot, tSize);
            if (tSlot.y >= tSize.y) tSlot.y--;
        }
    }
}




void UIInventory::TrackItemDrop(const ActorCommonData & tACDItem, InventorySlotType & eSlot, IVector2D & tSlot)
{



    eSlot = SLOT_NONE;
    tSlot.x = 0; tSlot.y = 0;

    UIID idWindow;
    UIInventoryControl eContext;

    if (GetMouseOverSlot(idWindow, eSlot, tSlot, eContext))
    {



        sAdjustSelectedSlotForItem(tACDItem, eSlot, tSlot);
    }
}


void UIInventory::HighlightSlot(
    const InventorySlotType eSlot,
    const IVector2D & tSlot,
    const ActorCommonData & tACD,
    const ActorCommonData & tACDItem,
    float & flZ)
{
    if (eSlot == SLOT_NONE)
    {
        return;
    }

    if (!ACDInventoryValidateSlotForACD(tACD, eSlot))
    {
        return;
    }


    if (ACDInventoryIsSlotMerchant(eSlot))
    {
        return;
    }


    if (!ACDInventoryIsSlotPlayer(eSlot) &&
        !ACDInventoryIsSlotHireling(eSlot))
    {
        return;
    }


    RGBAColor rgbaColor = sgrgbaNone;
    ACDID idACDOverItem = ACDID_INVALID;
    if (ACDInventorySlotIsMultiItemSlot(eSlot))
    {
        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, tACD.id, eSlot, tSlot.x, tSlot.y);

        if (ActorInventoryIsVariableSizedSlot(tInvLocDest.eSlot) &&
            !ActorInventoryIsVariableSizedSlotValidForItem(tACDItem, tInvLocDest))
        {
            return;
        }


        if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLocDest, FALSE, FALSE) == -1)
        {
            InventoryOverlapList tList;
            ACDInventoryTestOverlap(tACDItem, tInvLocDest, tList);
            switch (tList.Count())
            {
            case 0:

                rgbaColor = sgrgbaWhite;
                break;



            case 1:
            {
                ACDID idSwapItem = tList.GetHead();
                IVector2D tPoint;
                if (!sFindTopLeftPoint(tACD, eSlot, idSwapItem, tPoint))
                {
                    return;
                }

                idACDOverItem = idSwapItem;
                rgbaColor = sgrgbaYellow;
                break;
            }



            default:
                rgbaColor = sgrgbaRed;
                break;
            }
        }
        else
        {
            rgbaColor = sgrgbaRed;
        }
    }
    else
    {

        InventoryLocation tInvLoc;
        tInvLoc.idACDContainer = tACD.id;
        tInvLoc.eSlot = eSlot;
        tInvLoc.pt = cgptNull;

        if (ACDInventoryItemAllowedInSlot(tACDItem, tInvLoc, FALSE, FALSE) == -1)
        {

            ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACD, eSlot, tSlot);
            if (idACDSquareItem == ACDID_INVALID)
            {
                return;
            }

            idACDOverItem = idACDSquareItem;
            rgbaColor = sgrgbaYellow;
        }
        else
        {
            rgbaColor = sgrgbaRed;
        }
    }



    IVector2D tSlotToUse = tSlot;

    if (ACDInventorySlotIsMultiItemSlot(eSlot) && idACDOverItem != ACDID_INVALID)
    {
        ActorCommonData * ptACDOverItem = ACDTryToGet(idACDOverItem);

        if (ptACDOverItem &&
            ItemIsSocketable(tACDItem) &&
            ItemHasSockets(*ptACDOverItem))
        {

            IVector2D tPoint;
            if (sFindTopLeftPoint(tACD, eSlot, idACDOverItem, tPoint))
            {
                tSlotToUse = tPoint;
                if (ItemCanSocketInto(tACDItem, *ptACDOverItem) == SOCKET_REASON_SUCCESS)
                {
                    rgbaColor = sgrgbaYellow;
                }
                else
                {
                    rgbaColor = sgrgbaRed;
                }
            }
        }
    }


    if (!sTestSlotIsValidForACD(tACD, eSlot))
    {
        return;
    }

    Rect2D rectSlotUIC;
    if (!sGetSlotItemRect(&tACDItem, ICT_DEFAULT, eSlot, tSlotToUse, INVENTORY_MAIN, FALSE, rectSlotUIC))
    {
        return;
    }


    Rect2D rectSlotPixels;
    ConvertUICToPixels(rectSlotUIC, rectSlotPixels);

    sDrawHighlightHalo(rectSlotPixels, rgbaColor, eSlot, flZ);
}




static ShopQuickMoveResult sGetShopQuickMoveResult(
    ActorCommonData & tACDVendor)
{
    CurrencyType eCurrency = tACDVendor.GetCurrencyType();
    if (eCurrency == CURRENCY_BLOODSHARDS || eCurrency == CURRENCY_PLATINUM)
        return SQMR_UNHANDLED;

    return SQMR_SELL;
}



















static CrafterQuickMoveResult sGetCrafterQuickMoveResult(
    CrafterType eCrafterType,
    const ActorCommonData & tACDItem)
{
    switch (eCrafterType)
    {
    case 2:
        return CQMR_DO_NOTHING;























    case 1:
        if (ItemGetNumFilledSockets(tACDItem) > 0)
            return CQMR_REMOVE_GEM;
        return CQMR_DO_NOTHING;


    default:
        return CQMR_UNHANDLED;
    }
}






































static CRefString sGetQuickTip(
    const ActorCommonData & tACDItem,
    const ActorCommonData & tACDPlayer,
    const InventorySlotType eSlot,
    const BOOL fHotbar)
{



    if (fHotbar)
    {
        if (ItemHasLabel(tACDItem.hGB.gbid, ITL_POTION))

            return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickDrinkHotbar");
    }


    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    if (tCursorMgr.GetCursorState() == CURSORSTATE_SALVAGE_ITEM)
    {

        if (SalvageCommon::ItemCanSalvage(tACDItem, tACDPlayer) == GAMEERROR_NONE)

            return StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickSalvage");



        return CRefString("");
    }





    CRefString usRetval;
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return usRetval;



    ActorCommonData * ptACDNPC = ACDTryToGet(ptPlayer->idACDActiveVendor);


    if (eSlot != SLOT_NONE &&
        ptACDNPC &&
        NPCCanTrade(*ptACDNPC) &&
        NPCGetCrafterType(*ptACDNPC) == -1 &&
        !ItemIsQuest(tACDItem))
    {
        if (eSlot == SLOT_MERCHANT_DISPLAY || eSlot == SLOT_PLAYER_BUYBACK)
            usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickBuy");
        else
        {




            ShopQuickMoveResult eShopResult = sGetShopQuickMoveResult(*ptACDNPC);
            if (eShopResult == SQMR_SELL && ItemCanBeSold(tACDItem, ptACDNPC) == ISE_NONE)
                usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickSell");
        }
        return usRetval;
    }


















    if (ItemIsUnidentified(tACDItem))
    {
        usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "UnidentifiedNoScrolls");
        return usRetval;
    }
    if (ptACDNPC) {
        CrafterType eActiveCrafterType = NPCGetCrafterType(*ptACDNPC);
        CrafterQuickMoveResult eCrafterResult = sGetCrafterQuickMoveResult(eActiveCrafterType, tACDItem);

        if (eCrafterResult != CQMR_UNHANDLED)
        {
























            if (eCrafterResult == CQMR_REMOVE_GEM)
                usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickMoveToRemoveGemSlot");
            return usRetval;
        }
    }













    if (eSlot == SLOT_PLAYER_SHARED_STASH)
    {
        usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickMoveToBackpack");
    }
    else if (eSlot == SLOT_PLAYER_BACKPACK)
    {
        if (UI_GetWindowFromID(UIDLG_STASH_PAGE).IsActive())
        {
            usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickMoveToStash");
        }
        else
        {
            int32 nNumSlots = 0;
            InventorySlotType eValidSlots[4];
            ACDInventoryWhereCanThisGo(tACDPlayer, tACDItem.hGB.gbid, nNumSlots, eValidSlots);

            if (nNumSlots > 0)

                usRetval = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickEquip");
        }
    }






    return usRetval;
}

CRefString ItemGetCostString(const ActorCommonData &, CostType, const ActorCommonData *, BOOL); // X/Client/CItems.cpp:724
CostType ACDInventoryGetCostType(ACDID, ACDID, ACDID); // X/Common/ActorInventory.cpp:11791

CRefString UIInventory::GetCostTooltip(
    const ActorCommonData * ptACDPlayer,
    const ActorCommonData * ptACDContainer,
    const ActorCommonData & tACDItem,
    BOOL bNumericOnly)
{
    if (ItemIsCurrency(tACDItem))
    {

        return "";
    }

    CostType eCostType = -1;
    if (ptACDPlayer)
    {

        eCostType = ACDInventoryGetCostType(tACDItem.id, ptACDContainer->id, ptACDPlayer->id);
    }
    if (eCostType == -1)
        eCostType = 0;


    return ItemGetCostString(tACDItem, eCostType, ptACDContainer, bNumericOnly);
}


CRefString ItemGetItemTypeDescription(const ActorCommonData &, PlayerClass); // X/Client/CItems.cpp:896
CRefString ItemGetTradableToText(const ActorCommonData &); // X/Client/CItems.cpp:1258
UIItemOverlay ItemGetOverlay(const ActorCommonData &, const ActorCommonData &); // X/Client/CItems.cpp:117
CRefString ItemGetFlavorText(const ActorCommonData &); // X/Common/Items.cpp:9962
CRefString ItemGetDurability(const ActorCommonData &, BOOL, BOOL); // X/Common/Items.cpp:8620
CRefString ItemGetClassRestrictionDescription(const ActorCommonData &, PlayerClass); // X/Common/Items.cpp:9978
CRefString ItemGetPrimaryDescription(const ActorCommonData &); // X/Common/Items.cpp:11319
CRefString ItemGetArmorTotalDescription(const ActorCommonData &, PlayerClass, ItemDescriptionColorOverride, void *); // X/Common/Items.cpp:11362
CRefString ItemGetBlockStats(const ActorCommonData &, PlayerClass, ItemDescriptionColorOverride, void *); // X/Common/Items.cpp:11721
CRefString ItemGetDPSTotalDescription(const ActorCommonData &, PlayerClass, ItemDescriptionColorOverride, void *); // X/Common/Items.cpp:11826
CRefString ItemGetDamageStats(const ActorCommonData &, PlayerClass, ItemDescriptionColorOverride, void *); // X/Common/Items.cpp:11838
BOOL ItemIsDye(const ActorCommonData &); // X/Common/Items.cpp:9191

























static CRefString sGetCraftingPlanRequirementsTooltip(
    const ActorCommonData * ptACDPlayer,
    const ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (!pdefItem)
    {
        return "";
    }

    GBID gbidRecipe = pdefItem->gbidCraftingRecipesToGrant[0];
    ScopedRecipeEntryLockR pdefRecipeEntry(gbidRecipe);
    if (!pdefRecipeEntry || (uint32)pdefRecipeEntry->eCrafterType >= 4)
    {
        return "";
    }


    if (ptACDPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
        if (ptPlayer && Console::GamerProfile::IsGuest(ptPlayer->nControllerIndex))
        {


            CRefString usMsg = StringListFetch("ConsoleInventory:GuestCannotTrain");
            return usMsg + "\n";
        }
    }



    if (ptACDPlayer)
    {
        Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
        if (ptPlayer)
        {

            CrafterData & tCrafterSavedData = ptPlayer->tCrafterSavedData.tCrafterData[pdefRecipeEntry->eCrafterType];
            for (int32 i = 0; i < tCrafterSavedData.nNumRecipes; ++i)
            {
                if (tCrafterSavedData.gbidRecipes[i] == gbidRecipe)
                {
                    CRefString usMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "PlanAlreadyKnown");
                    return usMsg + "\n";
                }
            }
        }
    }
    return "";
}





static CRefString sGetTransmogPlanRequirementsTooltip(
    const ActorCommonData * ptACDPlayer,
    const ActorCommonData & tACDItem)
{
    if (!ptACDPlayer)
    {
        return "";
    }

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
    {
        return "";
    }

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (!pdefItem)
    {
        return "";
    }

    for (int32 i = 0; i < 8; ++i)
    {
        if (pdefItem->gbidTransmogsToGrant[i] != GBID_NONE &&
            ItemCrafting_CanUnlockTransmog(pdefItem->gbidTransmogsToGrant[i], NULL, TRUE) &&
            !ItemCrafting_IsTransmogUnlocked(pdefItem->gbidTransmogsToGrant[i], ptPlayer, NULL))
        {
            return "";
        }
    }

    CRefString usMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "PlanAlreadyKnown");
    return usMsg + "\n";
}




static CRefString sGetCosmeticItemKnownTooltip(
    const ActorCommonData * ptACDPlayer,
    const ActorCommonData & tACDItem)
{
    if (!ptACDPlayer)
    {
        return "";
    }

    Player * ptPlayer = PlayerGetByACD(ptACDPlayer->id);
    if (!ptPlayer)
    {
        return "";
    }

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    if (!pdefItem)
    {
        return "";
    }

    CosmeticItemData & tCosmeticItemData = ptPlayer->tCosmeticItemSavedData.tCosmeticItemData;
    for (int32 i = 0; i < tCosmeticItemData.nNumCosmeticItems; ++i)
    {
        if (tCosmeticItemData.gbidCosmeticItems[i] == tACDItem.hGB.gbid)
        {
            CRefString usMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "PlanAlreadyKnown");
            return usMsg + "\n";
        }
    }

    for (int32 j = 0; j < tCosmeticItemData.nNumLimitedDurationPets; ++j)
    {
        if (tCosmeticItemData.tLimitedDurationPets[j].gbidPet == tACDItem.hGB.gbid)
        {
            CRefString usMsg = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "PlanAlreadyKnown");
            return usMsg + "\n";
        }
    }
    return "";
}






CRefString UIInventory::GetRequirementsTooltip(
    const ActorCommonData * ptACDPlayer,
    const ActorCommonData & tACDItem,
    GBID gbidRecipe)
{
    CRefString usToolTip;

    if (ItemIsUnique(tACDItem))
    {
        CRefString usUniqueTip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "Unique");
        Set_Text_Color(usUniqueTip, FONTCOLOR_BLUE);
        usToolTip += usUniqueTip + "\n";
    }
    if (ItemIsUniqueEquipped(tACDItem))
    {
        CRefString usUniqueTip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "UniqueEquipped");
        Set_Text_Color(usUniqueTip, FONTCOLOR_BLUE);
        usToolTip += usUniqueTip + "\n";
    }





























































































    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_CRAFTING_PLAN))
    {
        ItemUseType eType = ItemGetUseType(tACDItem);
        if (eType == ITEM_USE_LEARN_RECIPE)
        {
            usToolTip += sGetCraftingPlanRequirementsTooltip(ptACDPlayer, tACDItem);
        }
        else if (eType == ITEM_USE_UNLOCK_TRANSMOGS)
        {
            usToolTip += sGetTransmogPlanRequirementsTooltip(ptACDPlayer, tACDItem);
        }
    }

    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_COSMETIC_PET) ||
        ItemHasLabel(tACDItem.hGB.gbid, ITL_COSMETIC_PENNANT) ||
        ItemHasLabel(tACDItem.hGB.gbid, ITL_COSMETIC_PORTRAIT_FRAME) || ItemHasLabel(tACDItem.hGB.gbid, ITL_COSMETIC_WINGS))
    {

        usToolTip += sGetCosmeticItemKnownTooltip(ptACDPlayer, tACDItem);
    }

    if (ItemIsEquippedButDisabled(tACDItem))
    {
        if (ItemIsEquippedButDisabledDuplicateLegendary(tACDItem))
            usToolTip += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GAME_ERRORS), "ItemIsUniqueEquipped");
        else
            usToolTip += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "RequiresSkill");
    }


































    if (tACDItem.AttributesGetInt(ATTRIBUTE_IS_VENDOR_BOUGHT))
    {
        CRefString usCannotSalvageTip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "CannotSalvage");
        Set_Text_Color(usCannotSalvageTip, FONTCOLOR_ENCHANTED);
        usToolTip += usCannotSalvageTip + "\n";
    }
    return usToolTip;
}




CRefString UIInventory::GetItemLevelTooltip(
    const ActorCommonData & tACDItem)
{



    return "";
}





















CRefString UIInventory::GetItemStatsTooltip(const PlayerClass ePlayerClass, const ActorCommonData * ptACDPlayer, const ActorCommonData & tACDItem)
{



    CRefString usToolTip;

    if (ItemIsUnidentified(tACDItem))
    {
        CRefString usUnidentified = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "UnidentifiedDetails");
        usToolTip += usUnidentified + CRefString("\n");
    }
    else
    {

        if (ItemGetUseType(tACDItem) != ITEM_USE_HEALTHPOTION || ItemIsLegendary(tACDItem.hGB.gbid))
        {
            ACDID idACDPlayer = ptACDPlayer ? ptACDPlayer->id : ACDID_INVALID;
            CRefString usItemDescription = ItemGetAttributeDescriptions(tACDItem, ePlayerClass, InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT), TRUE, NULL, NULL, TRUE, idACDPlayer);
            if (usItemDescription.Length())

                usToolTip += usItemDescription;


            if (ItemGetUseType(tACDItem) == ITEM_USE_HEALTHPOTION)

                usToolTip += "\n";

        }
        if (ptACDPlayer != NULL)
        {
            CRefString usCooldown = ItemGetCooldownTooltip(tACDItem, *ptACDPlayer);
            usToolTip += usCooldown;
        }
    }
    return usToolTip;
}




CRefString UIInventory::GetInstructionTooltip(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem,
    const InventorySlotType eSlot,
    const BOOL fHotbar,
    const BOOL fShowQuickTip)
{
    CRefString usToolTip;


    if (fShowQuickTip)
    {
        CRefString usQuicktip = sGetQuickTip(tACDItem, tACDPlayer, eSlot, fHotbar);
        if (usQuicktip.Length())
        {
            usToolTip += usQuicktip;
            usToolTip += "\n";
            return usToolTip;
        }
    }

    BOOL fItemIsInMail = tACDItem.ann >= 0x2e46 && tACDItem.ann < 0x2f18;
    if (fItemIsInMail)
        return usToolTip;





    {
        const GameBalanceDefinition * pdefGB;
        ItemTypeDefinition * pItemTypeDef = ItemsItemTypeDefLockR(tACDItem.hGB.gbid, pdefGB);
        if (pItemTypeDef)
        {
            CRefString sRetVal;
            if (StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_INSTRUCTIONS), pItemTypeDef->tHeader.szName, sRetVal))
            {
                usToolTip += sRetVal;
                usToolTip += "\n";
            }
        }
        GBUnlockR(pdefGB);
    }

    if (tACDItem.GetStackCount() > 1)

        usToolTip += StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "QuickSplitStack");

    return usToolTip;
}







CRefString UIInventory::GetSetItemTooltipBonuses(
    const GBID gbidSet)
{
    CRefString usToolTip;

    if (gbidSet != GBID_NONE && gbidSet != 0)
    {
        SNO snoStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SETS);
        SNO snoItemPassivePowerStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_PASSIVE_POWER_DESCRIPTIONS);


        GBHandleList listGBHandles(GBGetHandlePool());
        GBEnumerate(GB_ITEMS, listGBHandles);

        POS pos;
        CRefString usSetBonuses;
        GBHandleList listGBSetBonuses(GBGetHandlePool());
        GBEnumerate(GB_SET_ITEM_BONUSES, listGBSetBonuses);

        GBHandleList listGBSetBonusesSorted(GBGetHandlePool());

        pos = listGBSetBonuses.GetTailPosition();
        while (pos)
        {
            GBHandle & hGB = listGBSetBonuses.GetPrev(pos);

            const GameBalanceDefinition * pdefGBItems;
            SetItemBonusTableEntry * pdefSetItemBonus = (SetItemBonusTableEntry *)GBLockR(hGB, pdefGBItems, TRUE);
            if (pdefSetItemBonus->gbidSet == gbidSet &&
                pdefSetItemBonus->nRequired > 0)
            {
                listGBSetBonusesSorted.AddTail_Tracked(hGB);
            }
            GBUnlockR(pdefGBItems);
        }

        listGBSetBonusesSorted.Sort(SortBonuses);


        RNG tRNG;
        RandSetSeed_Internal(tRNG, 0);


        CRefString usSetPrefix = StringListFetch(snoStringList, "SetPrefix");

        pos = listGBSetBonusesSorted.GetTailPosition();
        while (pos)
        {
            GBHandle & hGB = listGBSetBonusesSorted.GetPrev(pos);

            const GameBalanceDefinition * pdefGBItems;
            SetItemBonusTableEntry * pdefSetItemBonus = (SetItemBonusTableEntry *)GBLockR(hGB, pdefGBItems, TRUE);
            CRefString usBonus;
            usBonus += LocalizedSprintf(usSetPrefix.str(), UnicodeText::FormatNumber(pdefSetItemBonus->nRequired, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
            usBonus += "\n";

            AttribGroupID idAttribGroupMin = FastAttribGroupCreate(FAT_ITEM, -1);

            AttribGroupID idAttribGroupMax = FastAttribGroupCreate(FAT_ITEM, -1);

            AttribGroupID idAttribGroupItemPassivesMin = FastAttribGroupCreate(FAT_ITEM, -1);



            for (int32 i = 0; i < MAX_SET_ITEM_BONUS_ATTRIBUTES; i++)
            {
                const AttributeSpecifier & tAttribute = pdefSetItemBonus->ptSetItemBonusAttributes[i];

                if (tAttribute.eAttribute == ATTRIBUTE_NONE)
                {
                    continue;
                }

                int32 nAttributeParam = tAttribute.nParam;
                FastAttribKey tKey = MakeAttribKey(tAttribute.eAttribute, nAttributeParam);
                AttributeDataType eAttribType = KeyGetDataType(tKey);
                if (eAttribType == ATTRIBUTEDT_UNKNOWN)
                {
                    continue;
                }

                float flMinAffixValue = AffixGetFormulaMinValue(
                    tAttribute, NULL, GBID_NONE, &tRNG, NULL);




                float flMaxAffixValue = AffixGetFormulaMaxValue(
                    tAttribute, NULL, GBID_NONE, &tRNG, NULL);




                if (flMinAffixValue < FLT_EPSILON && flMaxAffixValue < FLT_EPSILON)
                {
                    continue;
                }


                if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {

                    FastAttribSetValueFloat(idAttribGroupItemPassivesMin, tKey, flMinAffixValue);
                }
                else
                {
                    FastAttribSetValueFloat(idAttribGroupMin, tKey, flMinAffixValue);
                    FastAttribSetValueFloat(idAttribGroupMax, tKey, flMaxAffixValue);
                }
            }

            usBonus += ItemGetAttributeTip("    {icon:bullet} ", idAttribGroupMin, idAttribGroupMax, TTVC_NORMAL_COLORING);

            usBonus += ItemGetPowerBonusAttributeDescriptions("    {icon:bullet} ", NULL, idAttribGroupMin, -1, PLAYER_CLASS_NONE, TTVC_NO_COLORING, NULL, NULL);

            FastAttribGroupDelete(idAttribGroupMin);
            FastAttribGroupDelete(idAttribGroupMax);


            for (int32 i = 0; i < MAX_SET_ITEM_BONUS_ATTRIBUTES; i++)
            {
                const AttributeSpecifier & tAttribute = pdefSetItemBonus->ptSetItemBonusAttributes[i];

                if (tAttribute.eAttribute == ATTRIBUTE_ITEM_POWER_PASSIVE)
                {



                    CRefString sStringListLabel = AttribToString(ATTRIBUTE_ITEM_POWER_PASSIVE);

                    usBonus += "    {icon:bullet} ";

                    StringLabelHandle hLabel = StringListGetLabelHandle(SNOToString(SNOName(POWER_SNO, tAttribute.nParam), FALSE).str());

                    CRefString usAttributeFormatSpecifier;
                    if (!StringListFetchByLabel(snoItemPassivePowerStringList, hLabel, usAttributeFormatSpecifier))
                    {
                        CRefString usErrorTip;
                        usErrorTip.Format("Missing item passive power label (%s):(%d),(%s)\n", SNOToString(STRINGLIST_SNO, snoItemPassivePowerStringList, FALSE).str(), hLabel, sStringListLabel.str());
                        usBonus += usErrorTip;
                    }
                    else
                    {
                        usBonus += UIToolTips_ProcessTip(NULL, idAttribGroupItemPassivesMin, -1, tAttribute.nParam, 1, usAttributeFormatSpecifier, TTVC_NO_COLORING, ItemResolveValueTagForItemPassivePowerDescription, (void *)(intptr_t)tAttribute.eAttribute, NULL);

                        usBonus += "\n";
                    } }
            }
            FastAttribGroupDelete(idAttribGroupItemPassivesMin);

            Set_Text_Color(usBonus, FONTCOLOR_GREEN);

            usToolTip += usBonus;
            GBUnlockR(pdefGBItems);
        }

    }

    return usToolTip;
}



static CRefString sGetSlotTooltip(
    const ActorCommonData & tACDItem)
{
    if (ItemIsUpgradeableJewel(tACDItem))
    {
        int32 nJewelRank = tACDItem.AttributesGetInt(ATTRIBUTE_JEWEL_RANK);
        ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
        CRefString usNumRank = UnicodeText::FormatNumber(nJewelRank, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        CRefString usNumMaxRank = UnicodeText::FormatNumber(pdefItem->nJewelMaxRank, TRUE, FNS_NORMAL, XLOCALE_INVALID);
        LPCTSTR szRankLabel = "UpgradeableJewelRank";

        if (pdefItem && pdefItem->nJewelMaxRank >= 0 && nJewelRank < pdefItem->nJewelMaxRank)
        {
            szRankLabel = "UpgradeableJewelRankHasMax";
        }
        else if (pdefItem && pdefItem->nJewelMaxRank >= 0 && nJewelRank >= pdefItem->nJewelMaxRank)
        {
            szRankLabel = "UpgradeableJewelRankMax";
        }

        CRefString usJewelRank = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), szRankLabel), usNumRank.str(), usNumMaxRank.str(), NULL);

        return usJewelRank;
    }

    return GetSlotTooltip(tACDItem.hGB.gbid);
}


static void sGetAttribsFromSocket(
    const ActorCommonData & tACDSocketItem,
    const ActorCommonData & tACDParent,
    AttribGroupID idAttribGroup)
{
    if (ItemIsJewel(tACDSocketItem))
    {
        return;
    }


    if (ItemIsGem(tACDSocketItem))
    {
        GBHandle hGBHighestTypeEffect;
        UIInventory::GetHighestTypeEffectFromSocket(
            tACDSocketItem.hGB.gbid,
            tACDParent.hGB,
            hGBHighestTypeEffect);

        if (hGBHighestTypeEffect.gbid != -1)
        {
            AttribGroupID idAttribGroupTemp = FastAttribGroupCreate(FAT_ITEM, -1);
            UIInventory::GetAttributesFromSocketEffect(hGBHighestTypeEffect, tACDParent.hGB.gbid, idAttribGroupTemp, NULL);
            FastAttribMergePermanent(idAttribGroup, idAttribGroupTemp);
            FastAttribGroupDelete(idAttribGroupTemp);
        }
    }
}




extern "C" BOOL sub_710050e910(GBID gbidItem);
extern "C" BOOL sub_71004f71e0(const ActorCommonData & tACDItem);
extern "C" RGBAColor sub_710050e7e0();

BOOL UIInventory::GetItemTooltipParametersForSlot(
    ItemTooltipParameterBlock & tParams,
    const ActorCommonData & tACDItem,
    const ItemTooltipPlayerData & tTooltipPlayerData,
    const ActorCommonData * ptACDContainer,
    const InventorySlotType eSlot,
    const BOOL fShowQuickTip,
    const BOOL bConsoleInventory,
    const BOOL bShowAsRepaired)
{


    D3::Items::Generator tBaseItem;
    ItemGeneratorToDefaults(tBaseItem);
    ItemGeneratorPopulateFromACDID(tACDItem, tBaseItem, TRUE);

    ItemAffixFastAttribMap arBaseItemAffixSpecificAttributes;
    ACDID idACDBaseItem = CItemGenerate(static_cast<ACDNetworkName>(0x2e0e), tBaseItem, &arBaseItemAffixSpecificAttributes);
    if (idACDBaseItem == ACDID_INVALID)
    {
        return FALSE;
    }

    ActorCommonData & tACDBaseItem = ACDGet(idACDBaseItem);

    FastAttribSetValueInt(tACDBaseItem.idFastAttrib, ATTRIBUTE_SOCKETS, 0);
    FastAttribSetValueInt(tACDBaseItem.idFastAttrib, ATTRIBUTE_ITEM_EQUIPPED, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_EQUIPPED));
    FastAttribSetValueInt(tACDBaseItem.idFastAttrib, ATTRIBUTE_ITEM_BOUND_TO_ACD, tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_BOUND_TO_ACD));
    FastAttribSetValueInt(tACDBaseItem.idFastAttrib, ATTRIBUTE_SEASON, tACDItem.AttributesGetInt(ATTRIBUTE_SEASON));

    if (ItemIsUnidentified(tACDItem))
    {
        tParams.sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "UnidentifiedTitle");
    }
    else
    {
        tParams.sName = ItemGetDisplayName(tACDBaseItem, TRUE, FALSE, FALSE, TRUE);
    }
    tParams.sType = ItemGetItemTypeDescription(tACDBaseItem, tTooltipPlayerData.ePlayerClass);
    tParams.sFlavor = ItemGetFlavorText(tACDBaseItem);
    tParams.sTradableTo = ItemGetTradableToText(tACDItem);
    tParams.sCost = GetCostTooltip(tTooltipPlayerData.ptACDPlayer, ptACDContainer, tACDItem, bConsoleInventory);
    if (!ItemIsUnidentified(tACDItem))
    {
        tParams.sDurability = ItemGetDurability(tACDItem, bConsoleInventory, bShowAsRepaired);
    }
    tParams.sClassReqs = ItemGetClassRestrictionDescription(tACDBaseItem, tTooltipPlayerData.ePlayerClass);
    tParams.sItemLevel = GetItemLevelTooltip(tACDItem);
    tParams.sRequirements = GetRequirementsTooltip(tTooltipPlayerData.ptACDPlayer, tACDItem, GBID_NONE);



































































































    tParams.sStats += GetItemStatsTooltip(tTooltipPlayerData.ePlayerClass, tTooltipPlayerData.ptACDPlayer, tACDBaseItem);

    if (tTooltipPlayerData.ptACDPlayer)
    {
        tParams.sInstruction = GetInstructionTooltip(*tTooltipPlayerData.ptACDPlayer, tACDItem, eSlot, FALSE, fShowQuickTip);
    }
    tParams.sSpecialStats += ItemGetPrimaryDescription(tACDItem);
    const ActorCommonData * ptSetOwner = tTooltipPlayerData.ptACDPlayer;
    if (ptACDContainer && ACDIsHireling(*ptACDContainer))
        ptSetOwner = ptACDContainer;
    tParams.sSetItem = GetSetItemTooltip(ptSetOwner, tTooltipPlayerData.ptAlternateInventory, tTooltipPlayerData.ePlayerClass, tACDBaseItem, NULL);
    ACDDelete(idACDBaseItem, FALSE);


    POS pos = arBaseItemAffixSpecificAttributes.GetStartAssoc();
    while (pos)
    {
        GBID gbidAffix;
        AttribGroupID idAffixFastAttrib;
        arBaseItemAffixSpecificAttributes.GetNextAssoc(pos, gbidAffix, idAffixFastAttrib);
        FastAttribGroupDelete(idAffixFastAttrib);
    }

    tParams.eIQL = ItemGetDisplayedQualityLevel(tACDItem);
    tParams.tImageTexFrame = cgtImageTextureFrameNull;

    ACDInventoryGetItemDrawInfo(tACDItem, tParams.tImageTexFrame, FALSE, tTooltipPlayerData.ePlayerClass, tTooltipPlayerData.fIsMale);

    tParams.dwTooltipFlags = 0;
    tParams.eOverlay = ITEMOVERLAY_NONE;

    if (tTooltipPlayerData.ptACDPlayer)
    {
        tParams.eOverlay = ItemGetOverlay(tACDItem, *tTooltipPlayerData.ptACDPlayer);
    }
    else if (ItemIsUnidentified(tACDItem))
    {
        tParams.eOverlay = ITEMOVERLAY_UNIDENTIFIED;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(tTooltipPlayerData.ptACDPlayer);

    tParams.eTooltipType = 3; tParams.eItemEffectType = 0;

    if (!ItemIsRandomItemPlaceholder(tACDItem.hGB.gbid))
    {
        if (ItemHasLabel(tACDItem.hGB.gbid, ITL_ARMOR) ||
            ItemHasLabel(tACDItem.hGB.gbid, ITL_SHIELD))
        {
            tParams.eTooltipType = 1;
            tParams.sRating = ItemGetArmorTotalDescription(tACDItem, ePlayerClass, NULL, NULL);
            tParams.sSpecialStats = ItemGetBlockStats(tACDItem, ePlayerClass, NULL, NULL);
        }
        else if (ItemHasLabel(tACDItem.hGB.gbid, ITL_WEAPON))
        {
            tParams.eTooltipType = 2;
            tParams.sRating = ItemGetDPSTotalDescription(tACDItem, ePlayerClass, NULL, NULL);
            tParams.sSpecialStats = ItemGetDamageStats(tACDItem, ePlayerClass, NULL, NULL);

            int32 nItemEffectLevel;
            ACDGetItemEffectTypeAndLevel(tACDItem, tParams.eItemEffectType, nItemEffectLevel, FALSE, TRUE);
        }
    }

    if (!ItemIsUnidentified(tACDItem))
    {
        AncientRank eAncientRank = ItemGetAncientRank(tACDItem);
        switch (eAncientRank)
        {
        case ANCIENT_RANK_ANCIENT:
            tParams.dwTooltipFlags |= 0x20;
            break;
        case ANCIENT_RANK_PRIMAL_ANCIENT:
            tParams.dwTooltipFlags |= 0x40;
            break;
        }
        if (sub_7100505c00(tACDItem))
            tParams.dwTooltipFlags |= 0x80;
        if (sub_7100505c90(tACDItem))
            tParams.dwTooltipFlags |= 0x100;
        if (sub_710050ece0(tACDItem))
            tParams.dwTooltipFlags |= 0x200;
    }



    tParams.sSlot = sGetSlotTooltip(tACDItem);

    GBID gbidTransmog = tACDItem.AttributesGetInt(ATTRIBUTE_TRANSMOG_GBID);
    if (gbidTransmog != GBID_NONE)
    {
        ScopedItemDefinitionLockR pdefTransmogItem(gbidTransmog);
        if (pdefTransmogItem)
        {

            CRefString usInitialItemName; CRefString sName1; CRefString sName2Ignored;
            SNO snoItemStringList = GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS);
            if (StringListFetch(snoItemStringList, pdefTransmogItem->tHeader.szName, usInitialItemName))
            {
                uint32 dwGrammaticalForm;
                LanguageRules::ExtractGrammaticalForm(usInitialItemName, sName1, sName2Ignored, dwGrammaticalForm);

                tParams.sTransmog = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), guszItemLabelTransmogrification);
                Set_Text_Color(tParams.sTransmog, FONTCOLOR_BLUE);

                ItemQualityLevel eIQL = pdefTransmogItem->eDisplayedQualityLevel;
                if (eIQL == -1)
                {
                    if (ItemIsSetItem(gbidTransmog))
                    {
                        eIQL = 11;
                    }
                    else if (ItemIsLegendary(gbidTransmog))
                    {
                        eIQL = 9;
                    }
                }

                RGBAColor rgbaTransmogQuality = ItemGetDisplayNameColor(eIQL);
                if (sub_710050e910(gbidTransmog))
                    rgbaTransmogQuality = sub_710050e7e0();
                if (LanguageRules::ItemNamesShouldBeCapitalized())
                {
                    sName1 = UnicodeText::Capitalize(sName1);
                }
                Set_Text_Color(sName1, rgbaTransmogQuality);
                tParams.sTransmog += sName1;
            }
        }
    }

    DyeType eDyeType = tACDItem.GetDyeType();
    if (eDyeType != 0 && !ItemIsDye(tACDItem))
    {
        tParams.sDye = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), SymbolToName(eDyeType, gDyeValueTooltips));
    }

    if (tACDItem.AttributesGetInt(ATTRIBUTE_SOCKETS) && !ItemIsUnidentified(tACDItem))
    {
        SocketCallbackData tSocketData;
        tSocketData.ptParams = &tParams;
        tSocketData.ptTooltipPlayerData = &tTooltipPlayerData;

        ACDInventoryIterateSlot(tACDItem, SLOT_ITEM_SOCKETS, NULL, PopulateSocketData, &tSocketData, FALSE, FALSE);
    }

    return TRUE;
}




void UIInventory::GetItemTooltipParameters(ItemTooltipParameterBlock & tParams, const ActorCommonData & tACDItem, const ItemTooltipPlayerData & tTooltipPlayerData, const BOOL bConsoleInventory, const BOOL bShowAsRepaired)
{
    GetItemTooltipParametersForSlot(tParams, tACDItem, tTooltipPlayerData, tTooltipPlayerData.ptACDPlayer, SLOT_NONE, FALSE, bConsoleInventory, bShowAsRepaired);
}































































































































































































































#line 5608  // 2.7.7: this function's three __LINE__ constants are 121 higher (5632/5666/5699)
static AttribGroupID sCreateInverseAttribGroup(
    AttribGroupID idAttribOrig)
{
    AttribGroupID idAttribNew = FastAttribLightCreate(FAT_ACTOR);

    FastAttribIterator tIt(idAttribOrig, TRUE);
    while (FastAttribIterate(tIt))
    {
        FastAttribKey & tKey = tIt.tEntryBuffer.tKey;
        FastAttribValue & tValue = tIt.tEntryBuffer.tValue;
        AttributeOperator eAggOp = KeyGetAggregationOperator(tKey);
        AttributeDataType eDataType = KeyGetDataType(tKey);
        switch (eAggOp)
        {
        case ATTRIBUTEOP_PLUS:
            switch (eDataType)
            {
            case ATTRIBUTEDT_FLOAT:
                FastAttribSetValueFloat(idAttribNew, tKey, -AttribValueToFloat(tValue));
                break;
            case ATTRIBUTEDT_INT:
                FastAttribSetValueInt(idAttribNew, tKey, -AttribValueToInt(tValue));
                break;
            default:
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            break;
        case ATTRIBUTEOP_MULTIPLY:
            switch (eDataType)
            {
            case ATTRIBUTEDT_FLOAT:
            {
                float flVal = AttribValueToFloat(tValue);
                if (Math::Abs(flVal) > 0.000001f)
                {
                    flVal = 1.0f / flVal;
                }
                else
                {
                    flVal = 0.0f;
                }

                FastAttribSetValueFloat(idAttribNew, tKey, flVal);
                break;
            }
            case ATTRIBUTEDT_INT:
            {
                int32 nVal = AttribValueToInt(tValue);
                if (abs(nVal) > 0)
                {

                    nVal = Math::FloatRoundToInt(1.0f / nVal);
                }

                FastAttribSetValueInt(idAttribNew, tKey, nVal);
                break;
            }
            default:
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            break;
        case ATTRIBUTEOP_MULTIPLY_INVERSE:
            switch (eDataType)
            {
            case ATTRIBUTEDT_INT:
                break;






            case ATTRIBUTEDT_FLOAT:
            {
                float flOrigVal = AttribValueToFloat(tValue);
                float flDenom = 1.0f - flOrigVal;
                float flNewVal = 0.0f;
                if (Math::Abs(flDenom) > 0.000001f)
                {
                    flNewVal = 1.0f - (1.0f / flDenom);
                }

                FastAttribSetValueFloat(idAttribNew, tKey, flNewVal);
                break;
            }





            default:
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            break;








        }
    }
    return idAttribNew;
}




static BOOL sIsDualWieldingAfterCompare(
    const ActorCommonData & tACDOwner,
    InventorySlotType eSlot,
    const ActorCommonData & tACDItem,
    const InventorySlotType ceSlotLeftHand,
    const InventorySlotType ceSlotRightHand)
{
    if (!ACDCanDualWield(tACDOwner))
    {
        return FALSE;
    }

    if (ItemHasLabel(tACDItem.hGB.gbid, ITL_2_HAND))
    {
        return FALSE;
    }

    ACDID idACDItemLeft = ACDInventoryGetSquareItem(tACDOwner, ceSlotLeftHand, cgptNull);
    ACDID idACDItemRight = ACDInventoryGetSquareItem(tACDOwner, ceSlotRightHand, cgptNull);
    const ActorCommonData * ptACDItemLeft = ACDTryToGet(idACDItemLeft);
    const ActorCommonData * ptACDItemRight = ACDTryToGet(idACDItemRight);

    if (eSlot == ceSlotLeftHand)
    {
        ptACDItemLeft = &tACDItem;
    }
    else if (eSlot == ceSlotRightHand) // inferred: original row 5623:14 is the "else if (" operand column
    {
        ptACDItemRight = &tACDItem;
    }

    if (ptACDItemLeft && ItemHasLabel(ptACDItemLeft->hGB.gbid, ITL_WEAPON) && // inferred: original rows 5628:9/23/26/58 and 5630:13/31/64 fit this positive form

            ptACDItemRight && ItemHasLabel(ptACDItemRight->hGB.gbid, ITL_WEAPON))
    {
        return TRUE;
    }
    return FALSE;
}





static BOOL sIsBareHandedAfterCompare(
    const ActorCommonData & tACDOwner,
    InventorySlotType eSlot,
    const ActorCommonData & tACDItem,
    const InventorySlotType ceSlotLeftHand,
    const InventorySlotType ceSlotRightHand)
{
    ACDID idACDItemLeft = ACDInventoryGetSquareItem(tACDOwner, ceSlotLeftHand, cgptNull);
    ACDID idACDItemRight = ACDInventoryGetSquareItem(tACDOwner, ceSlotRightHand, cgptNull);
    const ActorCommonData * ptACDItemLeft = ACDTryToGet(idACDItemLeft);
    const ActorCommonData * ptACDItemRight = ACDTryToGet(idACDItemRight);

    if (eSlot == ceSlotLeftHand)
    {
        ptACDItemLeft = &tACDItem;
    }
    else if (eSlot == ceSlotRightHand) // inferred: original row 5657:14
    {
        ptACDItemRight = &tACDItem;
    }

    if (ptACDItemLeft && ItemHasLabel(ptACDItemLeft->hGB.gbid, ITL_WEAPON))
    {
        return FALSE;
    }

    if (ptACDItemRight && ItemHasLabel(ptACDItemRight->hGB.gbid, ITL_WEAPON))
    {
        return FALSE;
    }

    return TRUE;
}



static BOOL sConsideredBareHanded(
    ActorCommonData * ptACDItem)
{
    if (ptACDItem && ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON)) // inferred: original rows 5680:9/19/22/50
    {
        return FALSE;
    }

    return TRUE;
}



static BOOL sConsideredBareHanded(
    ActorCommonData * ptACDItemLeftHand,
    ActorCommonData * ptACDItemRightHand)
{
    return sConsideredBareHanded(ptACDItemLeftHand) &&
        sConsideredBareHanded(ptACDItemRightHand);
}








void UIInventory::InitItemTooltip(
    ItemTooltipData & tData,
    const ACDID idACD,
    const GBID gbid,
    const ItemTooltipParameterBlock & tParams,
    const BOOL bCalculateComparisonData,
    ActorCommonData * ptACDPlayer,
    const BOOL bDisableSocketText)
{
    tData.idACD = idACD;
    tData.gbid = gbid;
    tData.eTooltipType = tParams.eTooltipType;
    tData.eItemEffectType = tParams.eItemEffectType;
    tData.eIQL = tParams.eIQL;
    tData.dwTooltipFlags = tParams.dwTooltipFlags;
    tData.eOverlay = tParams.eOverlay;
    tData.tImageTexFrame = tParams.tImageTexFrame;
    tData.uszName = tParams.sName;
    tData.uszType = tParams.sType;
    tData.uszClassReqs = tParams.sClassReqs;
    tData.uszItemLevel = tParams.sItemLevel;
    tData.uszRequirements = tParams.sRequirements;
    tData.uszStats = tParams.sStats;
    tData.uszRating = tParams.sRating;
    tData.uszSpecialStats = tParams.sSpecialStats;
    tData.uszSetItem = tParams.sSetItem;
    tData.uszFlavor = tParams.sFlavor;
    tData.uszTradableTo = tParams.sTradableTo;
    tData.uszCost = tParams.sCost;
    tData.uszDurability = tParams.sDurability;
    tData.uszInstruction = tParams.sInstruction;
    tData.uszSlot = tParams.sSlot;
    tData.uszSocketCost = tParams.sSocketCost;
    tData.uszRepairCost = tParams.sRepairCost;
    tData.uszTransmog = tParams.sTransmog;
    tData.uszDye = tParams.sDye;

    BOOL bExcludeSockets = InputShiftKeyIsDown(INPUTCONTEXT_DEFAULT);
    for (int32 i = 0; i < 3; ++i)
    {
        if (bDisableSocketText && bExcludeSockets)
            Set_Text_Color(tParams.sSocketStats[i], FONTCOLOR_GREY);
        tData.uszSocketStats[i] = tParams.sSocketStats[i];
        tData.arrSocketGBIDs[i] = tParams.sSocketGBIDs[i];
        tData.arrSocketTextures[i] = tParams.tSocketImageFrames[i];
    }

    tData.fShowComparisonInfo = FALSE;
    if (bCalculateComparisonData)
    {


        BOOL bUseAltSlot = InputAltKeyIsDown(INPUTCONTEXT_DEFAULT);
        tData.fShowComparisonInfo = GetComparisonValuesPct(ptACDPlayer, idACD, bUseAltSlot, bExcludeSockets, tData.flComparisonDeltaEHP, tData.flComparisonDeltaDmg, tData.flComparisonDeltaHealing) != FALSE;

    }
}










static float sComputePercent(
    float flDelta,
    float flOriginal)
{
    const float cflInfinity = 1000000.0f;
    if (flOriginal < 0.000001f)
    {
        if (Math::Abs(flDelta) < 0.000001f)
        {
            return 0.0f;
        }
        else if (flDelta < 0.0f) // inferred: original rows 5783:26, 5785:13 and the fdiv at 5794:24 fit if/else-if/else returns
        {
            return -cflInfinity;
        }
        else
        {
            return cflInfinity;
        }
    }
    else
    {
        return flDelta / flOriginal;
    }
}


static AttribGroupID sMergeOutItem(
    ActorCommonData & tACDOwner,
    ActorCommonData * ptACDItem,
    AttribGroupID idAttribTemp)
{


    AttribGroupID idAttribInSlot = ATTRIBGROUPID_INVALID;
    if (ptACDItem)
    {

        int32 nUpscaleLevel = tACDOwner.AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);

        if (nUpscaleLevel > 0)
        {
            AttribGroupID idUpscaledAttribs = ItemGenerateUpscaledAttribForPlayer(tACDOwner, *ptACDItem);
            idAttribInSlot = sCreateInverseAttribGroup(idUpscaledAttribs);
            ClearUpscaledAttributes(*ptACDItem);
        }
        else
        {

            idAttribInSlot = sCreateInverseAttribGroup(ptACDItem->idFastAttrib);
        }

        FastAttribMerge(idAttribTemp, idAttribInSlot);
    }

    return idAttribInSlot;
}


struct MergeOutSocketCallbackData
{
    AttribGroupID idAttribSocket;
};

static BOOL sMergeOutSocketData(
    const InventoryLocation & tInvLoc,
    void * pCallBackData)
{
    MergeOutSocketCallbackData & tData = *static_cast<MergeOutSocketCallbackData *>(pCallBackData);
    ActorCommonData * ptACDContainer = ACDTryToGet(tInvLoc.idACDContainer);
    if (!ptACDContainer)
    {
        return FALSE;
    }

    int32 nMaxSockets = ptACDContainer->AttributesGetInt(ATTRIBUTE_SOCKETS);
    int32 nIndex = tInvLoc.pt.y;
    if (nIndex >= nMaxSockets)
    {
        return FALSE;
    }

    ACDID idACDItem = ACDInventoryGetSquareItem(*ptACDContainer, tInvLoc.eSlot, tInvLoc.pt);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem)
    {
        sGetAttribsFromSocket(*ptACDItem, *ptACDContainer, tData.idAttribSocket);
    }

    return TRUE;
}


static AttribGroupID sMergeOutSocketBonuses(
    ActorCommonData * ptACDItem,
    AttribGroupID idAttribTemp)
{
    AttribGroupID idAttribInverseSocket = ATTRIBGROUPID_INVALID;
    if (ptACDItem)
    {
        if (ptACDItem->AttributesGetInt(ATTRIBUTE_SOCKETS) > 0 && !ItemIsUnidentified(*ptACDItem))
        {
            MergeOutSocketCallbackData tData;
            tData.idAttribSocket = FastAttribGroupCreate(FAT_ITEM, -1);
            ACDInventoryIterateSlot(*ptACDItem, SLOT_ITEM_SOCKETS, NULL, sMergeOutSocketData, &tData, FALSE, FALSE);

            idAttribInverseSocket = sCreateInverseAttribGroup(tData.idAttribSocket);
            FastAttribGroupDelete(tData.idAttribSocket);
            FastAttribMerge(idAttribTemp, idAttribInverseSocket);
        }
    }
    return idAttribInverseSocket;
}



static AttribGroupID sMergeOutCubeEnchantBonuses(
    ActorCommonData * ptACDItem,
    AttribGroupID idAttribTemp)
{
    AttribGroupID idAttribInverseCubeEchant = ATTRIBGROUPID_INVALID;
    if (ptACDItem)
    {
        if (ItemHasCubeEnchant(*ptACDItem) && !ItemIsUnidentified(*ptACDItem))
        {
            AttribGroupID idAttribCubeEnchant = FastAttribGroupCreate(FAT_ITEM, -1);

            Attrib eAttrib = ItemGetAttributeForCubeEnchant(*ptACDItem);
            float flCubeEnchantValue = ptACDItem->AttributesGetFloat(eAttrib);

            FastAttribSetValueFloat(idAttribCubeEnchant, eAttrib, flCubeEnchantValue);

            idAttribInverseCubeEchant = sCreateInverseAttribGroup(idAttribCubeEnchant);
            FastAttribGroupDelete(idAttribCubeEnchant);
            FastAttribMerge(idAttribTemp, idAttribInverseCubeEchant);
        }
    }
    return idAttribInverseCubeEchant;
}


// NON_MATCHING: 4324 vs 4344; the original materialises (ptACDItemInSlot != NULL) and (eSlotSwapped == ceSlotLeftHand) at 6011/6012:34 as bools reused by the inlined helpers (5619/5653) and at 6149:38/42 (and, not a branch), 16 more frame bytes; 6149/6153 rows are 4 columns deeper than ours, spelling unknown
BOOL UIInventory::GetComparisonValues(
    ActorCommonData * ptACDOwner,
    ACDID idACDItem,
    const BOOL bUseAltSlot,
    const BOOL bExcludeSockets,
    float & flComparisonDeltaPctEHP,
    float & flComparisonDeltaPctDmg,
    float & flComparisonDeltaPctHealing)
{
    if (!ptACDOwner || idACDItem == ACDID_INVALID)
    {
        return FALSE;
    }

    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (!ptACDItem)
    {
        return FALSE;
    }

    BOOL fOwnerIsHireling = ACDIsHireling(*ptACDOwner);
    const InventorySlotType ceSlotRightHand = fOwnerIsHireling ? SLOT_HIRELING_RIGHT_HAND : SLOT_PLAYER_RIGHT_HAND;
    const InventorySlotType ceSlotLeftHand = fOwnerIsHireling ? SLOT_HIRELING_LEFT_HAND : SLOT_PLAYER_LEFT_HAND;

    BOOL bRet = FALSE;
    PlayerClass eClass = PlayerGetPlayerClass(ptACDOwner);
    if (ItemIsEquippableByPlayerOrHireling(ptACDItem->hGB.gbid) &&
        ItemMeetsOwnerClassRestrictions(ptACDItem->hGB.gbid, eClass, FALSE))
    {
        AttribGroupID idAttribTemp = FastAttribGroupCreate(FAT_ACTOR, -1);
        FastAttribIterator tIt(ptACDOwner->idFastAttrib, TRUE);
        while (FastAttribIterate(tIt))
        {
            FastAttribSetValue(idAttribTemp, tIt.tEntryBuffer.tKey, tIt.tEntryBuffer.tValue);
        }




        AttribGroupID idAttribPlayer = ptACDOwner->idFastAttrib;
        ptACDOwner->SetAttributeGroup(idAttribTemp);

        BOOL bWasDualWielding = ACDIsDualWielding(*ptACDOwner, FALSE);
        float flOrigHP = ptACDOwner->GetHitPointsMaxTotal();
        float flOrigDPS = ptACDOwner->GetDPSValue(bWasDualWielding);
        float flOrigDamageReduction = ptACDOwner->GetDamageReductionPercent();
        float flOrigHealing = ptACDOwner->GetHealingValue(bWasDualWielding, NULL);



        AttribGroupID idAttribInSlot = ATTRIBGROUPID_INVALID;
        AttribGroupID idAttribInSlotOffhand = ATTRIBGROUPID_INVALID;
        AttribGroupID idAttribInSocket = ATTRIBGROUPID_INVALID;
        AttribGroupID idAttributeCubeEnchant = ATTRIBGROUPID_INVALID;
        BOOL bShowAltTooltipInstruction; // the original stores nothing here before passing it by reference (no 5968 row)
        InventorySlotType eSlot = sComputeComparisonSlot(*ptACDOwner, ptACDItem->hGB.gbid, SLOT_NONE, bUseAltSlot, bShowAltTooltipInstruction, FALSE);
        InventorySlotType eSlotSwapped = eSlot;
        if (eSlot != SLOT_NONE)
        {

            int32 nUpscaleLevel = ptACDOwner->AttributesGetInt(ATTRIBUTE_UPSCALED_LEVEL);


            ACDID idACDItemInSlot = ACDInventoryGetSquareItem(*ptACDOwner, eSlot, cgptNull);
            if (idACDItemInSlot != ptACDItem->id)
            {
                ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);
                if (eSlot == ceSlotRightHand) // inferred: original rows all 5981:21 and the 5985-5996 rows are 4 columns deeper
                {
                    if (!ptACDItemInSlot)
                    {
                        ACDID idACDItemInLeftHandSlot = ACDInventoryGetSquareItem(*ptACDOwner, ceSlotLeftHand, cgptNull);
                        ActorCommonData * ptACDItemInLeftHandSlot = ACDTryToGet(idACDItemInLeftHandSlot);



                        if (ptACDItemInLeftHandSlot && ItemHasLabel(ptACDItemInLeftHandSlot->hGB.gbid, ITL_2_HAND))
                        {



                            BOOL bItemIsQuiver = ItemHasLabel(ptACDItem->hGB.gbid, ITL_QUIVER);
                            BOOL bItemIsShieldAndEquippable = ptACDOwner->AttributesGetInt(ATTRIBUTE_ALLOW_2H_AND_SHIELD) && ItemHasLabel(ptACDItem->hGB.gbid, ITL_SHIELD);
                            if (!bItemIsQuiver && !bItemIsShieldAndEquippable) // original rows 5997:34/48
                            {
                                ptACDItemInSlot = ptACDItemInLeftHandSlot;
                                eSlotSwapped = ceSlotLeftHand;
                            }
                        }
                    }
                }

                InventoryLocation tInvLocRightHand;
                ACDInventoryLocationSet(tInvLocRightHand, ptACDOwner->id, ceSlotRightHand, 0, 0);
                ACDID idACDItemRightHand = ACDInventoryGetItemInSlot(tInvLocRightHand);
                ActorCommonData * ptACDItemRightHand = ACDTryToGet(idACDItemRightHand);
                ActorCommonData * ptACDItemInSecondSlot = NULL;
                if (ptACDItemInSlot &&
                    eSlotSwapped == ceSlotLeftHand && ptACDItemRightHand &&
                    ACDInventoryDoItemsConflict(*ptACDOwner, *ptACDItem, *ptACDItemRightHand))
                {
                    ptACDItemInSecondSlot = ptACDItemRightHand;
                }


                if (bExcludeSockets)
                {


                    AttribGroupID idAttribInSocketSlot = sMergeOutSocketBonuses(ptACDItemInSlot, idAttribTemp);
                    AttribGroupID idAttribInSocketSlotOffhand = sMergeOutSocketBonuses(ptACDItemInSecondSlot, idAttribTemp);

                    AttribGroupID idAttribCubeEnchant = sMergeOutCubeEnchantBonuses(ptACDItemInSlot, idAttribTemp);
                    AttribGroupID idAttribCubeEnchantOffhand = sMergeOutCubeEnchantBonuses(ptACDItemInSecondSlot, idAttribTemp);

                    flOrigHP = ptACDOwner->GetHitPointsMaxTotal();
                    flOrigDPS = ptACDOwner->GetDPSValue(bWasDualWielding);
                    flOrigDamageReduction = ptACDOwner->GetDamageReductionPercent();
                    flOrigHealing = ptACDOwner->GetHealingValue(bWasDualWielding, NULL);

                    if (idAttribInSocketSlot != ATTRIBGROUPID_INVALID)
                    {
                        FastAttribUnmerge(idAttribTemp, idAttribInSocketSlot);
                        FastAttribGroupDelete(idAttribInSocketSlot);
                    }

                    if (idAttribInSocketSlotOffhand != ATTRIBGROUPID_INVALID)
                    {
                        FastAttribUnmerge(idAttribTemp, idAttribInSocketSlotOffhand);
                        FastAttribGroupDelete(idAttribInSocketSlotOffhand);
                    }

                    if (idAttribCubeEnchant != ATTRIBGROUPID_INVALID)
                    {
                        FastAttribUnmerge(idAttribTemp, idAttribCubeEnchant);
                        FastAttribGroupDelete(idAttribCubeEnchant);
                    }

                    if (idAttribCubeEnchantOffhand != ATTRIBGROUPID_INVALID)
                    {
                        FastAttribUnmerge(idAttribTemp, idAttribCubeEnchantOffhand);
                        FastAttribGroupDelete(idAttribCubeEnchantOffhand);
                    }
                }


                idAttribInSlot = sMergeOutItem(*ptACDOwner, ptACDItemInSlot, idAttribTemp);
                idAttribInSlotOffhand = sMergeOutItem(*ptACDOwner, ptACDItemInSecondSlot, idAttribTemp);

                InventoryLocation tInvLocLeftHand;
                ACDInventoryLocationSet(tInvLocLeftHand, ptACDOwner->id, ceSlotLeftHand, 0, 0);
                ACDID idACDItemLeftHand = ACDInventoryGetItemInSlot(tInvLocLeftHand);
                ActorCommonData * ptACDItemLeftHand = ACDTryToGet(idACDItemLeftHand);

                BOOL bWasBareHanded = sConsideredBareHanded(ptACDItemLeftHand, ptACDItemRightHand);
                if (bWasBareHanded) // original row 6069:21
                {

                    if (ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON))
                    {
                        FastAttribSetValueFloat(idAttribTemp, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN, 0), 0.0f);
                        FastAttribSetValueFloat(idAttribTemp, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA, 0), 0.0f);
                    }
                }

                AttribGroupID idAttribMergedItem = FastAttribLightCreate(FAT_ITEM);
                FastAttribLightCopy(ptACDItem->idFastAttrib, idAttribMergedItem);


                FastAttribIterator tItItem(idAttribMergedItem, TRUE);
                while (FastAttribIterate(tItItem))
                {
                    AttributeOperator eAggOp = KeyGetAggregationOperator(tItItem.tEntryBuffer.tKey);
                    if (eAggOp == ATTRIBUTEOP_SET)
                    {
                        FastAttribSetValueToDefault(idAttribMergedItem, tItItem.tEntryBuffer.tKey);
                    }
                }

                if (eSlot == ceSlotRightHand &&
                    ItemHasLabel(ptACDItem->hGB.gbid, ITL_WEAPON) &&
                    !ItemHasLabel(ptACDItem->hGB.gbid, ITL_2_HAND))
                {
                    FastAttribSetValue(idAttribMergedItem, ATTRIBUTE_HELD_IN_OFFHAND, FastAttribValue(TRUE));





                    if (ACDCanDualWield(*ptACDOwner) && !bWasDualWielding)
                    {
                        FastAttribSetValueInt(idAttribMergedItem, ATTRIBUTE_DUALWIELD_HAND, 1);
                    }
                }


                BOOL bIsDualWielding = sIsDualWieldingAfterCompare(*ptACDOwner, eSlotSwapped, *ptACDItem, ceSlotLeftHand, ceSlotRightHand);
                AttribGroupID idAttribDualWield = ATTRIBGROUPID_INVALID;
                if (eSlot == ceSlotRightHand || eSlot == ceSlotLeftHand)
                {


                    SNO snoDualWieldBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUAL_WIELD_BUFF);
                    if (snoDualWieldBuff != -1)
                    {
                        float flAttackSpeedIncrease = 0.0f;
                        float flDualWieldSpeedIncrease = PowerGetFormulaValue(ptACDOwner, AutoPowerDefinitionR(snoDualWieldBuff), TAG_POWER_ATTACK_SPEED_PERCENT_INCREASE_MIN, -1, ATTRIBGROUPID_INVALID, 0);
                        if (bWasDualWielding && !bIsDualWielding)
                        {
                            flAttackSpeedIncrease = -flDualWieldSpeedIncrease;
                        }
                        else if (!bWasDualWielding && bIsDualWielding)
                        {
                            flAttackSpeedIncrease = flDualWieldSpeedIncrease;
                        }

                        if (Math::Abs(flAttackSpeedIncrease) > 0.000001f)
                        {
                            idAttribDualWield = FastAttribLightCreate(FAT_ACTOR);
                            FastAttribSetValueFloat(idAttribDualWield, ATTRIBUTE_ATTACKS_PER_SECOND_PERCENT, flAttackSpeedIncrease);
                            FastAttribMerge(idAttribTemp, idAttribDualWield);
                        }
                    }
                }

                if (!bWasBareHanded && sIsBareHandedAfterCompare(*ptACDOwner, eSlotSwapped, *ptACDItem, ceSlotLeftHand, ceSlotRightHand))
                {
                    SNO snoBareHandedPassive = GlobalSNOGet(GLOBALSNO_POWERS_BAREHANDED_PASSIVE);
                    if (snoBareHandedPassive != -1)
                    {
                        PowerApplyGenericAttributeBuffAttributes(*ptACDOwner, AutoPowerDefinitionR(snoBareHandedPassive), idAttribTemp);
                    }
                }


                if (nUpscaleLevel > 0 && ptACDItemInSlot)
                {


                    FastAttribMerge(idAttribTemp, ptACDItemInSlot->idFastAttrib);

                    flOrigHP = ptACDOwner->GetHitPointsMaxTotal();
                    flOrigDPS = ptACDOwner->GetDPSValue(bIsDualWielding);
                    flOrigHealing = ptACDOwner->GetHealingValue(bIsDualWielding, NULL);
                    flOrigDamageReduction = ptACDOwner->GetDamageReductionPercent();

                    FastAttribUnmerge(idAttribTemp, ptACDItemInSlot->idFastAttrib);
                }


                FastAttribMerge(idAttribTemp, idAttribMergedItem);

                if (bExcludeSockets)
                {
                    idAttribInSocket = sMergeOutSocketBonuses(ptACDItem, idAttribTemp);
                    idAttributeCubeEnchant = sMergeOutCubeEnchantBonuses(ptACDItem, idAttribTemp);
                }

                float flNewHP = ptACDOwner->GetHitPointsMaxTotal();
                float flNewDPS = ptACDOwner->GetDPSValue(bIsDualWielding);
                float flNewHealing = ptACDOwner->GetHealingValue(bIsDualWielding, NULL);
                float flNewDamageReduction = ptACDOwner->GetDamageReductionPercent();

                if (idAttribInSocket != ATTRIBGROUPID_INVALID)
                {
                    FastAttribUnmerge(idAttribTemp, idAttribInSocket);
                    FastAttribGroupDelete(idAttribInSocket);
                }

                if (idAttributeCubeEnchant != ATTRIBGROUPID_INVALID)
                {
                    FastAttribUnmerge(idAttribTemp, idAttributeCubeEnchant);
                    FastAttribGroupDelete(idAttributeCubeEnchant);
                }

                if (idAttribInSlot != ATTRIBGROUPID_INVALID)
                {
                    FastAttribUnmerge(idAttribTemp, idAttribInSlot);
                    FastAttribGroupDelete(idAttribInSlot);
                }

                if (idAttribInSlotOffhand != ATTRIBGROUPID_INVALID)
                {
                    FastAttribUnmerge(idAttribTemp, idAttribInSlotOffhand);
                    FastAttribGroupDelete(idAttribInSlotOffhand);
                }

                if (idAttribDualWield != ATTRIBGROUPID_INVALID)
                {
                    FastAttribUnmerge(idAttribTemp, idAttribDualWield);
                    FastAttribGroupDelete(idAttribDualWield);
                }

                FastAttribUnmerge(idAttribTemp, idAttribMergedItem);
                FastAttribGroupDelete(idAttribMergedItem);



                float flOrigEHP = ActorCommonData::GetEHPValue(flOrigHP, flOrigDamageReduction);
                float flNewEHP = ActorCommonData::GetEHPValue(flNewHP, flNewDamageReduction);
                float flComparisonDeltaEHP = flNewEHP - flOrigEHP;
                float flComparisonDeltaDmg = flNewDPS - flOrigDPS;
                float flComparisonDeltaHealing = flNewHealing - flOrigHealing;

                flComparisonDeltaPctEHP = sComputePercent(flComparisonDeltaEHP, flOrigEHP);
                flComparisonDeltaPctDmg = sComputePercent(flComparisonDeltaDmg, flOrigDPS);
                flComparisonDeltaPctHealing = Math::Min(sComputePercent(flComparisonDeltaHealing, flOrigHealing), 9.999f);
                bRet = TRUE;
            }
        }
        FastAttribGroupDelete(idAttribTemp);


        ptACDOwner->SetAttributeGroup(idAttribPlayer);
    }

    return bRet;
}





inline BOOL UIInventory::GetComparisonValuesPct(ActorCommonData * ptACDOwner, ACDID idACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, float & flComparisonDeltaPctEHP, float & flComparisonDeltaPctDmg, float & flComparisonDeltaPctHealing)
{








    flComparisonDeltaPctDmg = 0.0f;

    flComparisonDeltaPctEHP = 0.0f;

    flComparisonDeltaPctHealing = 0.0f; return GetComparisonValues(ptACDOwner, idACDItem, bUseAltSlot, bExcludeSockets, flComparisonDeltaPctEHP, flComparisonDeltaPctDmg, flComparisonDeltaPctHealing);
}















void UIInventory::GetComparisonValueStrings(ActorCommonData * ptACDOwner, const ActorCommonData & tACDItem, const BOOL bUseAltSlot, const BOOL bExcludeSockets, CRefString & sValEHP, CRefString & sValDmg, CRefString & sValHealing)
{







    float flComparisonDeltaPctEHP = 0.0f;
    float flComparisonDeltaPctDmg = 0.0f;

    float flComparisonDeltaPctHealing = 0.0f;

    if (ItemIsUnidentified(tACDItem))
    {
        sValEHP = "?";
        sValDmg = "?";
        sValHealing = "?";
        return;
    }



    GetComparisonValuesPct(ptACDOwner, tACDItem.id, bUseAltSlot, bExcludeSockets, flComparisonDeltaPctEHP, flComparisonDeltaPctDmg, flComparisonDeltaPctHealing);







    sValEHP = ComparisonPctToIcon(flComparisonDeltaPctEHP);
    sValDmg = ComparisonPctToIcon(flComparisonDeltaPctDmg);
    sValHealing = ComparisonPctToIcon(flComparisonDeltaPctHealing);
}

CRefString UIInventory::ComparisonPctToIcon(const float flValue)
{
    const float THREE_ARROW_PCT = 0.1f;
    const float TWO_ARROW_PCT = 0.05f;
    const float ONE_ARROW_PCT = 0.001f;

    CRefString sIcon;
    if (flValue > THREE_ARROW_PCT)

        return CRefString("{icon:up3}"); else if (flValue < -THREE_ARROW_PCT)

        return CRefString("{icon:down3}"); else if (flValue > TWO_ARROW_PCT)

        return CRefString("{icon:up2}"); else if (flValue < -TWO_ARROW_PCT)

        return CRefString("{icon:down2}"); else if (flValue > ONE_ARROW_PCT)

        return CRefString("{icon:up1}"); else if (flValue < -ONE_ARROW_PCT)

        return CRefString("{icon:down1}");


    return CRefString("{icon:nostatchange}");
}














CRefString UIInventory::FormatItemComparisonField(
    const float flValue)
{
    float flRoundedValue = Math::FloatRoundToXPlaces(flValue, 3);
    CRefString sVal = UnicodeText::FormatNumber(flRoundedValue, TRUE, FNS_PERCENT, 1, 1, XLOCALE_INVALID);
    const RGBAColor * ptColor;
    if (flRoundedValue > 1e-06f)
    {
        sVal = CRefString("+") + sVal;
        ptColor = &crgbaGreen;
    }
    else if (flRoundedValue < -1e-06f) ptColor = &crgbaRed;
    else
    {




        sVal = CRefString("+") + sVal;
        ptColor = &crgbaGray;
    }
    Set_Text_Color(sVal, *ptColor);

    return sVal;
}



static void sDisplayToolTip(
    const UIID & uId,
    ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDContainer,
    const ActorCommonData & tACDItem,
    const InventorySlotType eSlot,
    const IVector2D & tSlot,
    const BOOL bPrimaryTip,
    const BOOL bShowAltTooltipInstruction,
    const BOOL bResetScrollPos,
    const BOOL bSuppressQuickTip,
    const Rect2D * prectSlotUIC)
{
    BOOL fShowQuickTip = bPrimaryTip && !bSuppressQuickTip;

    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (!ptPlayer)
        return;


    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_LOOPING_ANIMATION_SUPPRESS_ITEM_TOOLTIPS))
        return;




    ItemTooltipPlayerData tTooltipPlayerData(&tACDPlayer, NULL);
    ItemTooltipParameterBlock tParams;
    if (!UIInventory::GetItemTooltipParametersForSlot(tParams, tACDItem, tTooltipPlayerData, &tACDContainer, eSlot, fShowQuickTip, FALSE, FALSE))
        return;



    BOOL bEquipped = ItemIsEquipped(tACDItem);
    if (bEquipped)
    {
        if (ACDIsPlayer(tACDContainer.id))
            tParams.dwTooltipFlags |= 1 << TOOLTIP_CURRENTLY_EQUIPPED_PLAYER_BIT;
        else if (ACDIsHireling(tACDContainer))
            tParams.dwTooltipFlags |= 1 << TOOLTIP_CURRENTLY_EQUIPPED_HIRELING_BIT;
    }








    if (bShowAltTooltipInstruction && (tParams.dwTooltipFlags & ((1 << TOOLTIP_CURRENTLY_EQUIPPED_PLAYER_BIT) | (1 << TOOLTIP_CURRENTLY_EQUIPPED_HIRELING_BIT))))
    {
        CRefString sAltTooltip = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_UI_TOOLTIPS), "AlternateTooltip");
        tParams.dwTooltipFlags |= 1 << TOOLTIP_SHOW_ALT_INSTRUCTIONS_BIT;
        tParams.sInstruction += sAltTooltip;
        tParams.sInstruction += "\n";
    }


    BOOL bCalculateComparisonData = FALSE;
    if (bPrimaryTip && !bEquipped &&

        ItemIsEquippable(tACDItem) &&
        !ItemIsRandomItemPlaceholder(tACDItem.hGB.gbid))
        bCalculateComparisonData = TRUE;



    BOOL bDisableSocketText = bCalculateComparisonData || !fShowQuickTip;
    ItemTooltipData tData;
    UIInventory::InitItemTooltip(tData, tACDItem.id, tACDItem.hGB.gbid, tParams, bCalculateComparisonData, &tACDPlayer, bDisableSocketText);

    UIToolTip * ptTip = gtTooltipMgr.SetItemToolTip(uId, tData, bPrimaryTip, bResetScrollPos, NULL, FALSE);
    if (ptTip)
    {
        Rect2D rectSlotUIC;
        if (prectSlotUIC) rectSlotUIC = *prectSlotUIC;
        else
        {
            IVector2D tTopLeft = tSlot;
            sFindTopLeftPoint(tACDContainer, eSlot, tACDItem.id, tTopLeft);


            ACDID idACDLocalPlayer = LocalPlayerACDGet();

            InventoryControlType eICType = ICT_DEFAULT;
            if (tACDPlayer.id != idACDLocalPlayer &&
                tACDPlayer.id != PlayerGetHireling(idACDLocalPlayer))
                eICType = ICT_TRADING_PLAYER;



            UIInventoryControl eInventoryControl = INVENTORY_MAIN;










            BOOL fGotRect = UIInventory::GetSlotRect(eICType, eSlot, tTopLeft, eInventoryControl, FALSE, rectSlotUIC);
            if (!fGotRect)
                return;
        }

        gtTooltipMgr.AlignItemTooltip(rectSlotUIC, *ptTip);
    }
}








static InventorySlotType sComputeComparisonSlot(
    const ActorCommonData & tACDOwner,
    const GBID gbidItem,
    const InventorySlotType eSlotToIgnore,
    const BOOL bUseAltSlot,
    BOOL & bShowAltTooltipInstruction,
    BOOL bCheckVisiblePage)
{
    bShowAltTooltipInstruction = FALSE;

    BOOL fOwnerIsHireling = ACDIsHireling(tACDOwner);
    const InventorySlotType ceSlotRightHand = fOwnerIsHireling ? SLOT_HIRELING_RIGHT_HAND : SLOT_PLAYER_RIGHT_HAND;
    const InventorySlotType ceSlotLeftHand = fOwnerIsHireling ? SLOT_HIRELING_LEFT_HAND : SLOT_PLAYER_LEFT_HAND;

    InventorySlotType eSlot = SLOT_NONE;
    int32 nNumSlots = 0;
    InventorySlotType eValidSlots[4];
    if (fOwnerIsHireling)
    {
        ACDInventoryWhereCanThisGoHireling(gbidItem, nNumSlots, eValidSlots);
    }
    else
    {
        ACDInventoryWhereCanThisGo(tACDOwner, gbidItem, nNumSlots, eValidSlots);
    }


    if (fOwnerIsHireling && nNumSlots == 1 && eValidSlots[0] == SLOT_HIRELING_SPECIAL)
        return SLOT_HIRELING_SPECIAL;



    int32 nValidTooltipSlots[4];
    int32 nValidSlotCount = 0;


    for (int32 i = 0; i < nNumSlots; ++i)
    {
        InventorySlotType eSlotScoped = eValidSlots[i];

        if (eSlotScoped == eSlotToIgnore)
            continue;












        if (eSlotScoped == ceSlotRightHand &&
            ItemHasLabel(gbidItem, ITL_2_HAND))
            continue;



        ACDID idACDSquareItem = ACDInventoryGetSquareItem(tACDOwner, eSlotScoped, cgptNull);
        if (idACDSquareItem == ACDID_INVALID)
        {


            if (nNumSlots > 1 && eSlotScoped == ceSlotRightHand)
            {

                ACDID idACDMainHandItem = ACDInventoryGetSquareItem(tACDOwner, ceSlotLeftHand, cgptNull);
                if (idACDMainHandItem != ACDID_INVALID)
                {
                    ActorCommonData & tACDMainHand = ACDGet(idACDMainHandItem);
                    if (ItemIsTwoHanded(tACDMainHand))
                        continue;
                }
            }
        }












        nValidTooltipSlots[nValidSlotCount++] = i;
    }


    if (nValidSlotCount != 0)
    {
        int32 nWhichSlot = 0;
        if (nValidSlotCount > 1)
        {
            if (bUseAltSlot)
                nWhichSlot = 1;




            else
            {
                bShowAltTooltipInstruction = TRUE;
            }
        }

        eSlot = eValidSlots[nValidTooltipSlots[nWhichSlot]];
    }
    else if (nNumSlots > 0)
    {
        eSlot = eValidSlots[0];
    }

    return eSlot;
}




static void sDisplayAdditionalToolTips(
    const UIID & idParent,
    ActorCommonData & tACDOwner,
    const InventorySlotType eSlot,
    BOOL bShowAltTooltipInstruction,
    BOOL bResetScrollPos)
{
    if (eSlot == SLOT_NONE)
        return;




    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    if (eCursorState == CURSORSTATE_MOVE_ITEM || eCursorState == CURSORSTATE_SPLIT_STACK || eCursorState == CURSORSTATE_BUY_ITEM)
        return;





    ACDID idACDItem = ACDInventoryGetSquareItem(tACDOwner, eSlot, cgptNull);
    if (idACDItem != ACDID_INVALID)
    {
        ActorCommonData & tACDItem = ACDGet(idACDItem);
        sDisplayToolTip(idParent, tACDOwner, tACDOwner, tACDItem, eSlot, cgptNull, FALSE, bShowAltTooltipInstruction, bResetScrollPos, FALSE, NULL);
    }
}




void UIInventory::ShowAdditionalTooltips(
    const UIID & idParent,
    ActorCommonData & tACDOwner,
    const GBID gbidItem,
    const InventorySlotType eSlotToIgnore,
    const BOOL bResetScrollPos)
{
    BOOL bUseAltSlot = InputAltKeyIsDown(INPUTCONTEXT_DEFAULT);
    BOOL bShowAltTooltipInstruction;
    InventorySlotType eSlot = sComputeComparisonSlot(tACDOwner, gbidItem, eSlotToIgnore, bUseAltSlot, bShowAltTooltipInstruction, TRUE);
    sDisplayAdditionalToolTips(idParent, tACDOwner, eSlot, bShowAltTooltipInstruction, bResetScrollPos);
}




















































static void sShowToolTips(
    const UIID & uId,
    const ACDID idACDPlayer,
    InventoryLocation & tInvLocSlot,
    const ActorCommonData & tACDItemInSlot,
    const BOOL fDisableTooltipDelays,
    const BOOL fResetScrollPos)
{

    if (UI_GetCapture() != GetUIIDNone())
        return;






    if (!Prefetch::AddToQueue(SNOName(ACTOR_SNO, tACDItemInSlot.snoActor), 2))
        return;




    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    ActorCommonData & tACDContainer = ACDGet(tInvLocSlot.idACDContainer);
    sDisplayToolTip(uId, tACDPlayer, tACDContainer, tACDItemInSlot, tInvLocSlot.eSlot, tInvLocSlot.pt, TRUE, FALSE, fResetScrollPos, FALSE, NULL);


    if (tInvLocSlot.eSlot >= SLOT_PLAYER_START && tInvLocSlot.eSlot < SLOT_PLAYER_END &&
        tInvLocSlot.eSlot != SLOT_PLAYER_BACKPACK &&
        tInvLocSlot.eSlot != SLOT_PLAYER_BUYBACK &&
        tInvLocSlot.eSlot != SLOT_PLAYER_SHARED_STASH)
    {
        return;
    }





    if (!ACDIsHireling(tACDContainer)) // DWARF: idACDOwner 6751 and ptACDOwner 6760 are in a lexical block; rows 6749:7/6
    {
        ACDID idACDOwner = idACDPlayer;


        if (tInvLocSlot.eSlot == SLOT_PLAYER_SHARED_STASH || tInvLocSlot.eSlot == SLOT_MERCHANT_DISPLAY || tInvLocSlot.eSlot == SLOT_MERCHANT_SHOPLIST)
        {

            idACDOwner = LocalPlayerACDGet();
        }

        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ptACDOwner)
        {
            if (fDisableTooltipDelays)
            {
                gtTooltipMgr.SetAllowTooltipDelays(FALSE);
            }
            UIInventory::ShowAdditionalTooltips(uId, *ptACDOwner, tACDItemInSlot.hGB.gbid, tInvLocSlot.eSlot, fResetScrollPos);
        }
    }
}












































static ACDID sGetSlotOwner(
    ACDID idACDPlayer,
    InventorySlotType eSlot)
{

    if (eSlot >= SLOT_MERCHANT_START && eSlot < SLOT_MERCHANT_END)
    {
        const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
        if (!ptPlayer)
            return ACDID_INVALID;



        return ptPlayer->idACDActiveVendor;
    }


    if (eSlot >= SLOT_HIRELING_START && eSlot < SLOT_HIRELING_END)
    {

        return UIHireling::GetCurrentHireling();
    }
    return idACDPlayer;
}





static void sShowTooltipForEmptyBodySlot(
    UIID & id,
    const InventorySlotType eSlot)
{
    CRefString sSlotLabel = UIInventory::ItemGetLabelForSlot(eSlot, FALSE);
    CRefString sTooltipText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel);
    gtTooltipMgr.SetTextToolTip(id, sTooltipText, UIJH_CENTER, FALSE, TRUE);
}





static void sShowTooltipForEmptyHirelingSlot(
    UIID & id,
    const InventorySlotType eSlot,
    const HirelingClass eHirelingClass)
{
    if (eSlot == SLOT_HIRELING_RIGHT_HAND || eSlot == SLOT_HIRELING_LEFT_HAND)
    {


        CRefString sLabel;
        switch (eHirelingClass)
        {
        case 1:
            sLabel = eSlot == SLOT_HIRELING_LEFT_HAND ? "TemplarMainHand" : "TemplarOffHand";
            break;
        case 2:
            sLabel = eSlot == SLOT_HIRELING_LEFT_HAND ? "ScoundrelMainHand" : "ScoundrelOffHand";
            break;
        case 3:
            sLabel = eSlot == SLOT_HIRELING_LEFT_HAND ? "EnchantressMainHand" : "EnchantressOffHand";
            break;
        }
        CRefString sTooltipText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sLabel);
        gtTooltipMgr.SetTextToolTip(id, sTooltipText, UIJH_CENTER, FALSE, TRUE);
    }
    else if (eSlot == SLOT_HIRELING_SPECIAL)
    {
        CRefString sLabel;
        switch (eHirelingClass)
        {
        case 1:
            sLabel = "TemplarSpecial";
            break;
        case 2:
            sLabel = "ScoundrelSpecial";
            break;
        case 3:
            sLabel = "EnchantressSpecial";
            break;
        }
        CRefString sTooltipText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sLabel);
        gtTooltipMgr.SetTextToolTip(id, sTooltipText, UIJH_CENTER, FALSE, TRUE);
    }
    else
    {
        sShowTooltipForEmptyBodySlot(id, eSlot);
    }
}




static uint32 sGetCooldownTimeToShow(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{
    uint32 dwCooldownTime = ItemGetRemainingCooldownTime(tACDPlayer, tACDItem);
    uint32 dwTimeLeft;
    if (dwCooldownTime != (uint32)-1)
    {
        dwTimeLeft = dwCooldownTime - CGameGetPredictedServerTime();
    }
    else
    {
        dwTimeLeft = ItemGetCooldownTimeToApply(tACDItem);
    }
    return dwTimeLeft;
}




static uint32 sGetTradeSecondsToShow(
    const ActorCommonData & tACDItem)
{
    uint32 uTradeSecondsLeft = 0;
    ItemBindingLevel eBindLevel = ItemGetBindingLevel(tACDItem);
    if (eBindLevel == IBL_BOUND_TO_GAME)
    {
        uint32 uTimeLeft = ItemGetTradeTimeLeft(tACDItem);
        uTradeSecondsLeft = uTimeLeft / 60;
    }
    return uTradeSecondsLeft;
}




// NON_MATCHING: 2768 vs 2784 bytes (GetSelectedSlot now has its body). The first 180 instructions and everything after the unchanged-test match; between lines 7099 and 7120 the original spills ptACDItemInCompareSlot to the stack and reloads two flag registers (w24, w26) on each path into the test, ours keeps the pointer in x26 and saves w23/w24 as a pair. Helpers GetMouseOverSlot (1936-1960), ItemTooltipState::Changed (if form), sShowToolTips (lexical block) and the 7138-7144 store order were put on the original rows with no size change: spill choice only.
void UIInventory::UpdateToolTips(
    ActorCommonData & tACDPlayer)
{
    static BOOL sfAltWasDown = FALSE;
    static BOOL sfCtrlWasDown = FALSE;
    static BOOL sfShiftWasDown = FALSE;
    static BOOL sfWasInSalvageCursorMode = FALSE;
    static uint32 sdwCooldownTimeToShow = (uint32)-1;
    static int32 snLevel = 0;
    static uint32 suTradeSecondsLeft = 0;
    static uint32 suTradeSecondsLeftAlt = 0;


    InvGlobals * ptInv = UI_GetInventoryData();
    if (!ptInv)
        return;






















    UIID uId;
    IVector2D tSlot;
    InventorySlotType eSlot = SLOT_NONE;
    UIInventoryControl eContext = INVENTORY_MAIN;










    if (!GetMouseOverSlot(uId, eSlot, tSlot, eContext))
    {

        ptInv->tPreviousTooltipState.Initialize();
        return;
    }


    if (eContext == INVENTORY_EQUIPMENT_MANAGER)
        return;



    ActorCommonData * ptACDPlayer = &tACDPlayer;















    ACDID idACDInventory = sGetSlotOwner(ptACDPlayer->id, eSlot);
    ActorCommonData * ptACDInventory = ACDTryToGet(idACDInventory);
    if (!ptACDInventory)
        return;




    InventoryLocation tInvLocSlot;
    ACDInventoryLocationSet(tInvLocSlot, idACDInventory, eSlot, tSlot.x, tSlot.y);
    ACDID idACDItemInSlot;












    idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);


    ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);



    if (ptACDItemInSlot && ItemIsBeingManipulated(*ptACDItemInSlot))
        return;




    ActorCommonData * ptACDHandItem = NULL;
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    CursorState eCursorState = tCursorMgr.GetCursorState();
    switch (eCursorState)
    {
    case CURSORSTATE_MOVE_ITEM: ptACDHandItem = tCursorMgr.GetItemToMove(); break;



    case CURSORSTATE_SPLIT_STACK: ptACDHandItem = tCursorMgr.GetStackToSplit(); break;
    }
    if (ptACDHandItem && ptACDHandItem == ptACDItemInSlot)
        return;







    if (tACDPlayer.AttributesGetInt(ATTRIBUTE_LOOPING_ANIMATION_SUPPRESS_ITEM_TOOLTIPS))
        return;



    BOOL fInSalvageCursorMode = eCursorState == CURSORSTATE_SALVAGE_ITEM;
    BOOL fAltIsDown = InputAltKeyIsDown(INPUTCONTEXT_DEFAULT);
    BOOL fCtrlIsDown = InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT);
    BOOL fShiftIsDown = InputShiftKeyIsDown(INPUTCONTEXT_DEFAULT);
    uint32 dwCooldownTimeToShow = (uint32)-1;
    uint32 uTradeSecondsLeft = 0;
    uint32 uTradeSecondsLeftAlt = 0;
    int32 nLevel = tACDPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);
    if (ptACDItemInSlot)
    {

        dwCooldownTimeToShow = sGetCooldownTimeToShow(tACDPlayer, *ptACDItemInSlot);

        uTradeSecondsLeft = sGetTradeSecondsToShow(*ptACDItemInSlot);


        BOOL bShowAltTooltipInstruction;
        InventorySlotType eSlotScoped = sComputeComparisonSlot(tACDPlayer, ptACDItemInSlot->hGB.gbid, SLOT_NONE, fAltIsDown, bShowAltTooltipInstruction, TRUE);
        if (eSlotScoped != SLOT_NONE)
        {
            ACDID idACDItemInCompareSlot = ACDInventoryGetSquareItem(tACDPlayer, eSlotScoped, cgptNull);
            if (idACDItemInCompareSlot != ACDID_INVALID && idACDItemInCompareSlot != ptACDItemInSlot->id)
            {
                ActorCommonData * ptACDItemInCompareSlot = ACDTryToGet(idACDItemInCompareSlot);
                if (ptACDItemInCompareSlot)
                {
                    uTradeSecondsLeftAlt = sGetTradeSecondsToShow(*ptACDItemInCompareSlot);
                }
            }
        }
    }

    if (fAltIsDown != sfAltWasDown ||
        fCtrlIsDown != sfCtrlWasDown ||
        fShiftIsDown != sfShiftWasDown ||
        fInSalvageCursorMode != sfWasInSalvageCursorMode ||
        dwCooldownTimeToShow != sdwCooldownTimeToShow ||
        nLevel != snLevel ||
        uTradeSecondsLeft != suTradeSecondsLeft ||
        uTradeSecondsLeftAlt != suTradeSecondsLeftAlt ||
        !gtTooltipMgr.ControlIsShowingTooltip(uId) ||
        ptInv->tPreviousTooltipState.Changed(eSlot, ptACDItemInSlot))
    {

        BOOL fDisableTooltipDelays = fAltIsDown != sfAltWasDown || fCtrlIsDown != sfCtrlWasDown || fShiftIsDown != sfShiftWasDown ||
            uTradeSecondsLeft != suTradeSecondsLeft || uTradeSecondsLeftAlt != suTradeSecondsLeftAlt;

        BOOL fResetScrollPos = !gtTooltipMgr.ControlIsShowingTooltip(uId) || ptInv->tPreviousTooltipState.Changed(eSlot, ptACDItemInSlot);


        snLevel = nLevel; // original store rows: 7138:17 snLevel, 7139:28 suTradeSecondsLeft, 7141:22 sfAltWasDown, 7143:24 sfShiftWasDown
        suTradeSecondsLeft = uTradeSecondsLeft;
        suTradeSecondsLeftAlt = uTradeSecondsLeftAlt;
        sfAltWasDown = fAltIsDown;
        sfCtrlWasDown = fCtrlIsDown;
        sfShiftWasDown = fShiftIsDown;
        sfWasInSalvageCursorMode = fInSalvageCursorMode;

        gtTooltipMgr.HideTips();


        ptInv->tPreviousTooltipState.Set(eSlot, ptACDItemInSlot);

        if (ptACDItemInSlot)
        {




















            sShowToolTips(uId, tACDPlayer.id, tInvLocSlot, *ptACDItemInSlot, fDisableTooltipDelays, fResetScrollPos);
        }
        else
        {

            if (eSlot >= SLOT_PLAYER_BODYSLOT_START && eSlot < SLOT_PLAYER_BODYSLOT_END)
            {

                if (UIInventory::SlotIsEmpty(tACDPlayer, eSlot, tSlot))
                {
                    sShowTooltipForEmptyBodySlot(uId, eSlot);
                }
            }
            else if (eSlot >= SLOT_HIRELING_START && eSlot < SLOT_HIRELING_END)
            {

                ACDID idACDHireling = UIHireling::GetCurrentHireling();
                ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
                if (!ptACDHireling || UIInventory::SlotIsEmpty(*ptACDHireling, eSlot, tSlot))
                {
                    HirelingClass eHirelingClass = HirelingGetClass(*ptACDHireling);
                    sShowTooltipForEmptyHirelingSlot(uId, eSlot, eHirelingClass);
                }
            }
        }
    }
}



void UIInventory::Display(const float flDeltaTime)
{
}




































BOOL UIInventory::IsOpen()
{
    return UI_GetWindowFromID(UIDLG_INVENTORY_CONSOLE).IsActive();
}







































































































































struct ItemUsePromptData
{
    ACDID idACDItemToUse;
    ACDID idACDItemInSlot;
};

static ItemUsePromptData sgtItemUsePromptData;

void sInventoryConfirmItemUse(
    const UIID &)
{
    ActorCommonData * ptACDItemToUse = ACDTryToGet(sgtItemUsePromptData.idACDItemToUse);
    if (ptACDItemToUse)
        CACDInventoryRequestUse(*ptACDItemToUse, sgtItemUsePromptData.idACDItemInSlot, TRUE, NULL);
}



static void sDoItemUse(
    ActorCommonData * ptACDItemInSlot,
    const Player * ptPlayer)
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();

    if (ptACDItemInSlot)
    {

        ActorCommonData * ptACDItemToUse = tCursorMgr.GetItemToUse();
        if (!ptACDItemToUse) return;

        if (ItemUseIsTargettedInInventory(ItemGetUseType(*ptACDItemToUse)))
        {


            Target tTarget;
            tTarget.eType = TT_ACTOR;
            tTarget.idACDTarget = ptACDItemInSlot->id;

            GameError eError = ItemCanBeUsed(ptPlayer->idPlayerACD, *ptACDItemToUse, &tTarget);
            ItemUseType eItemUseType = ItemGetUseType(*ptACDItemToUse);
            if (eError == GAMEERROR_NONE)
            {
                if (eItemUseType == ITEM_USE_ADD_SOCKETS)
                {
                    NotificationCreationParams tParams;
                    tParams.uszTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "WarningTitle");
                    if (ItemGetBoundToACD(*ptACDItemInSlot, FALSE) == ACDID_INVALID)
                    {
                        tParams.uszMessage = StringListFetch("Inventory:AddSocketsConfirmation_AccountBound");
                    }
                    else
                    {
                        tParams.uszMessage = StringListFetch("Inventory:AddSocketsConfirmation");
                    }
                    tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_bind_0_1<void, const UIID &, &sInventoryConfirmItemUse>();
                    sgtItemUsePromptData.idACDItemToUse = ptACDItemToUse->id;
                    sgtItemUsePromptData.idACDItemInSlot = ptACDItemInSlot->id;
                    HubGet().ModalNotificationManagerGet().SetOkCancelNotification(tParams);
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_CONFIRMATION_OPEN), cgtPSODefaults);
                }
                else
                {

                    CACDInventoryRequestUse(*ptACDItemToUse, ptACDItemInSlot->id, TRUE, NULL);
                }
            }
            else if (eError != GAMEERROR_ITEMUSE_UNKNOWN)
            {
                if (eItemUseType == ITEM_USE_ADD_SOCKETS)
                {
                    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SOCKET_MAKER_FAIL), cgtPSODefaults);
                }
                UIGame::DisplayError(GameErrorData(eError), FALSE, NULL);
            }
        }
    }

    tCursorMgr.ResetCursorState(FALSE);
}




struct SocketItemPromptData
{
    ACDID idACDHostItemOwner;
    ACDID idACDHostItem;
    ACDID idACDGem;
};

static SocketItemPromptData sgtItemPromptData;

static void sCancelSocket(
    const UIID &)
{
    sgtItemPromptData.idACDHostItemOwner = ACDID_INVALID;
    sgtItemPromptData.idACDHostItem = ACDID_INVALID;
    sgtItemPromptData.idACDGem = ACDID_INVALID;
}

static void sDoSocket(
    const UIID &)
{
    ActorCommonData * ptACDHostItemOwner = ACDTryToGet(sgtItemPromptData.idACDHostItemOwner);
    ActorCommonData * ptACDHostItem = ACDTryToGet(sgtItemPromptData.idACDHostItem);
    ActorCommonData * ptACDGem = ACDTryToGet(sgtItemPromptData.idACDGem);
    if (!ptACDHostItemOwner || !ptACDHostItem || !ptACDGem ||
        ptACDHostItem->GetInventoryContainerOwner() != ptACDHostItemOwner->id ||
        !ItemHasSockets(*ptACDHostItem) ||
        ItemCanSocketInto(*ptACDGem, *ptACDHostItem) != SOCKET_REASON_SUCCESS)
    {
        sCancelSocket(GetUIIDNone());
        return;
    }



    ActorCommonData * ptACDGemOwner = ACDTryToGet(ptACDGem->GetInventoryContainerOwner());
    if (!ptACDGemOwner)
    {
        sCancelSocket(GetUIIDNone());
        return;
    }

    Player * ptHostItemOwnerPlayer = ptACDHostItemOwner->GetPetOwnerPlayer();
    Player * ptGemOwnerPlayer = ptACDGemOwner->GetPetOwnerPlayer();

    ACDID idACDHostItemOwner = ptHostItemOwnerPlayer ? ptHostItemOwnerPlayer->idPlayerACD : ptACDHostItemOwner->id;
    ACDID idACDGemOwner = ptGemOwnerPlayer ? ptGemOwnerPlayer->idPlayerACD : ptACDGemOwner->id;
    if (idACDHostItemOwner == idACDGemOwner)
    {
        UIInventory::PlayItemInventorySound(*ptACDGem);
        ItemSetIsBeingManipulated(*ptACDGem, FALSE, 120);
        CACDInventoryRequestToSocket(*ptACDGem, *ptACDHostItem, LocalPlayerGet());
    }
    sCancelSocket(GetUIIDNone());
}














static BOOL sAttemptToSocketItem(
    ActorCommonData & tACDHandItem,
    ActorCommonData & tACDItemInSlot,
    ActorCommonData & tACDSlotOwner)
{
    if ((ACDIsPlayer(tACDSlotOwner.id) || ACDIsHireling(tACDSlotOwner)) &&
        ItemHasSockets(tACDItemInSlot))
    {
        SocketReason eReason = ItemCanSocketInto(tACDHandItem, tACDItemInSlot);
        switch (eReason)
        {
        case SOCKET_REASON_SUCCESS:
            sgtItemPromptData.idACDHostItemOwner = tACDSlotOwner.id;
            sgtItemPromptData.idACDHostItem = tACDItemInSlot.id;
            sgtItemPromptData.idACDGem = tACDHandItem.id;
            sDoSocket(GetUIIDNone());
            PlayerGetCursorMgr().ResetCursorState(FALSE);
            return TRUE;




        case SOCKET_REASON_LEVEL_RESTRICTION_NOT_MET:
            UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_PLAYER_DOESNT_MEET_ITEM_LEVEL), FALSE, NULL);
            return TRUE;


        case SOCKET_REASON_TYPE_MISMATCH:
            UIGame::DisplayError(GameErrorData(sub_71004f71e0(tACDHandItem) ? GAMEERROR_UNK_138 : GAMEERROR_ITEM_SOCKET_TYPE_MISMATCH), FALSE, NULL);
            return TRUE;


        case SOCKET_REASON_UNIQUE_EQUIPPED:
            UIGame::DisplayError(GameErrorData(GAMEERROR_ITEM_SOCKET_UNIQUE_EQUIPPED), FALSE, NULL);
            return TRUE;
        default:
            if (ItemIsEquipped(tACDItemInSlot) && eReason == SOCKET_REASON_UNIDENTIFIED)
            {
                UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_CANT_EQUIP_UNIDENTIFIED_ITEM), FALSE, NULL);
                return TRUE;
            }
        }
    }
    return FALSE;
}



static void sDoItemMove(
    ActorCommonData * ptACDItemInSlot,
    ActorCommonData & tACDSlotOwner,
    InventoryLocation & tInvLocDest)
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    ActorCommonData * ptACDHandItem = tCursorMgr.GetItemToMove();


    sAdjustSelectedSlotForItem(*ptACDHandItem, tInvLocDest.eSlot, tInvLocDest.pt);


    if (ptACDHandItem == ptACDItemInSlot)
    {
        InventoryLocation & tInvLocExistingItem = ptACDItemInSlot->GetInventoryLocation();
        if (tInvLocExistingItem.pt == tInvLocDest.pt)
        {
            tCursorMgr.ResetCursorState(TRUE);
            return;
        }
    }



    if (ItemIsTwoHanded(*ptACDHandItem))
    {
        const InventoryLocation * ptInvLocHandItem = ptACDHandItem->TryToGetInventoryLocation();
        if (ptInvLocHandItem)
        {
            if ((tInvLocDest.eSlot == SLOT_PLAYER_RIGHT_HAND && ptInvLocHandItem->eSlot == SLOT_PLAYER_LEFT_HAND) ||
                (tInvLocDest.eSlot == SLOT_HIRELING_RIGHT_HAND && ptInvLocHandItem->eSlot == SLOT_HIRELING_LEFT_HAND))
            {
                tCursorMgr.ResetCursorState(TRUE);
                return;
            }
        }
    }

    if (tInvLocDest.eSlot == SLOT_PLAYER_BUYBACK)
    {

        UIShop::SellItem(*ptACDHandItem);
        tCursorMgr.ResetCursorState(FALSE);
        return;
    }

    if (ptACDItemInSlot && sAttemptToSocketItem(*ptACDHandItem, *ptACDItemInSlot, tACDSlotOwner))
    {
        return;
    }




    GameErrorData eErrorData(ACDInventoryCanMove(*ptACDHandItem, tInvLocDest, NULL));
    if (eErrorData.eError != GAMEERROR_NONE)
    {
        eErrorData.dwVal = tInvLocDest.idACDContainer;
        UIGame::DisplayError(eErrorData, FALSE, NULL);
        return;
    }


    UIInventory::PlayItemInventorySound(*ptACDHandItem);
    CACDInventoryRequestMove(*ptACDHandItem, tInvLocDest, FALSE, NULL);
}




static void sDoStackSplit(
    ActorCommonData * ptACDItemInSlot,
    ActorCommonData & tACDSlotOwner,
    InventoryLocation & tInvLocDest)
{
    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    ActorCommonData * ptACDHandItem = tCursorMgr.GetStackToSplit();


    ActorLocation tLocation = ptACDHandItem->GetLocation();
    if (tLocation.eActorLocation != AL_INVENTORY)
    {
        tCursorMgr.ResetCursorState(FALSE);
        return;
    }

    if (tLocation.tInvLocation.eSlot == SLOT_PLAYER_BUYBACK)
    {
        tCursorMgr.ResetCursorState(FALSE);
        return;
    }

    if (ptACDItemInSlot)
    {
        if (sAttemptToSocketItem(*ptACDHandItem, *ptACDItemInSlot, tACDSlotOwner))
        {
            return;
        }

        if (ptACDHandItem->hGB.gbid == ptACDItemInSlot->hGB.gbid)
        {

            UIInventory::PlayItemInventorySound(*ptACDHandItem);
            CACDInventoryRequestStackTransfer(ptACDHandItem->id, ptACDItemInSlot->id, tCursorMgr.GetStackSplitAmount());
            return;
        }
    }
    else
    {





        sAdjustSelectedSlotForItem(*ptACDHandItem, tInvLocDest.eSlot, tInvLocDest.pt);

        if (tInvLocDest.eSlot != SLOT_PLAYER_BACKPACK && tInvLocDest.eSlot != SLOT_PLAYER_SHARED_STASH)
        {
            tCursorMgr.ResetCursorState(FALSE);
            return;
        }


        UIInventory::PlayItemInventorySound(*ptACDHandItem);
        CACDInventoryRequestSplitStack(ptACDHandItem, tInvLocDest, tCursorMgr.GetStackSplitAmount());
        return;
    }
    tCursorMgr.ResetCursorState(FALSE);
}


static void sDoItemBuy(
    ActorCommonData * ptACDItemInSlot,
    InventoryLocation & tInvLocDest)
{



    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    ActorCommonData * ptACDHandItem = tCursorMgr.GetItemToBuy();
    UIShop::BuyItem(*ptACDHandItem);
    tCursorMgr.ResetCursorState(FALSE);
}






static void sItemPickup(
    const ActorCommonData & tACDItem)
{
    if (ItemIsBeingManipulated(tACDItem))
    {
        return;
    }


    ActorCommonData & tACDPlayer = ACDGet(LocalPlayerACDGet());
    if (!PlayerCanManipulateInventory(tACDPlayer))
    {
        UIGame::DisplayError(GameErrorData(GAMEERROR_ITEMS_CANNOT_BE_MOVED_AT_THIS_TIME), FALSE, NULL);
        return;
    }


    UIInventory::PlayItemPickupSound();

    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    tCursorMgr.SetItemToMove(tACDItem);
}



// Inference: linker-dropped original (DWARF low_pc 0, 0x130 bytes); body from its DWARF locals/inline tree and line table, prologue mirrors OnClick.
void UIInventory::OnDragStart(
    const UIID & uId)
{

    if (!UI_TestID(uId)) return;


    const Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
    {
        return;
    }


    InventorySlotType eSlot = SLOT_NONE;
    IVector2D tSlot;
    UIInventoryControl eContext;
    GetSelectedSlot(uId, eSlot, tSlot, eContext);


    ACDID idACDSlotOwner = sGetSlotOwner(LocalPlayerACDGet(), eSlot);
    if (idACDSlotOwner == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDSlotOwner = ACDGet(idACDSlotOwner);

    InvGlobals * ptInvGlobals = UI_GetInventoryData();
    if (ptInvGlobals)
    {
        InventoryLocation tInvLocSlot;
        ACDInventoryLocationSet(tInvLocSlot, tACDSlotOwner.id, eSlot, tSlot.x, tSlot.y);
        ptInvGlobals->idACDDragged = ACDInventoryGetItemInSlot(tInvLocSlot);
    }
}




UIResult UIInventory::OnDrag(
    const UIID & uId)
{

    if (!UI_TestID(uId)) return UIRESULT_UNHANDLED;






    // Inference: linker-dropped original (DWARF low_pc 0, 0xb0 bytes); body from DWARF locals/inline tree and line table 7807-7813; HANDLED/UNHANDLED returns reproduce its 0xb0 size and line-0 instruction before the 7818 epilogue.
    InvGlobals * ptInvGlobals = UI_GetInventoryData();
    if (ptInvGlobals)
    {
        ActorCommonData * ptACDItemInSlot = ACDTryToGet(ptInvGlobals->idACDDragged);
        if (ptACDItemInSlot)
        {
            sItemPickup(*ptACDItemInSlot);
        }
    }

    return UIRESULT_HANDLED;
}




void UIInventory::OnClick(
    const UIID & uId)
{

    if (!UI_TestID(uId)) return;


    if (UIGame::MenuIsActive()) return;





    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_PVP_BIT)) return;













    const Player * ptPlayer = LocalPlayerGet();
    if (!ptPlayer)
    {
        return;
    }


    InventorySlotType eSlot = SLOT_NONE;
    IVector2D tSlot;
    UIInventoryControl eContext;
    GetSelectedSlot(uId, eSlot, tSlot, eContext);


    ACDID idACDSlotOwner = sGetSlotOwner(LocalPlayerACDGet(), eSlot);
    if (idACDSlotOwner == ACDID_INVALID)
    {
        return;
    }

    ActorCommonData & tACDSlotOwner = ACDGet(idACDSlotOwner);

    InventoryLocation tInvLocSlot;
    ACDInventoryLocationSet(tInvLocSlot, tACDSlotOwner.id, eSlot, tSlot.x, tSlot.y);
    ACDID idACDItemInSlot = ACDInventoryGetItemInSlot(tInvLocSlot);
    ActorCommonData * ptACDItemInSlot = ACDTryToGet(idACDItemInSlot);















    CursorMgr & tCursorMgr = PlayerGetCursorMgr();
    switch (tCursorMgr.GetCursorState())
    {















    case CURSORSTATE_USE_ITEM:
        sDoItemUse(ptACDItemInSlot, ptPlayer);
        break;
    case CURSORSTATE_MOVE_ITEM:
    {

        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, idACDSlotOwner, eSlot, tSlot.x, tSlot.y);

        sDoItemMove(ptACDItemInSlot, tACDSlotOwner, tInvLocDest);
        break;
    }
    case CURSORSTATE_BUY_ITEM:
    {

        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, idACDSlotOwner, eSlot, tSlot.x, tSlot.y);

        sDoItemBuy(ptACDItemInSlot, tInvLocDest);
        break;
    }
    case CURSORSTATE_SPLIT_STACK:
    {

        InventoryLocation tInvLocDest;
        ACDInventoryLocationSet(tInvLocDest, idACDSlotOwner, eSlot, tSlot.x, tSlot.y);

        sDoStackSplit(ptACDItemInSlot, tACDSlotOwner, tInvLocDest);
        break;
    }




    default:
        // Inference: original line table places the pickup test at 7944 and the call at 7969 (intervening lines emit no code).
        if (ptACDItemInSlot)
        {























            sItemPickup(*ptACDItemInSlot);
        }
        break;
    }


}




























void UIInventory::TryToQuickMoveToLocation(ActorCommonData & tACDOwner, ActorCommonData & tACDItem, const InventorySlotType eSlot)
{



    int32 nSlotYMinMax[2];
    int32 * pnSlotYMinMax = NULL;
    if (eSlot == SLOT_PLAYER_SHARED_STASH)
    {

        IVector2D tSize;
        ACDInventoryGetSlotPageSize(SLOT_PLAYER_SHARED_STASH, tSize);

        nSlotYMinMax[0] = GetStashOffset();
        nSlotYMinMax[1] = nSlotYMinMax[0] + tSize.y;
        pnSlotYMinMax = nSlotYMinMax;
    }

    InventoryLocation tInvLoc;
    if (ACDInventoryFindNonFullStackInSlot(tACDItem, tACDOwner, eSlot, tInvLoc, pnSlotYMinMax))
    {
        CACDInventoryRequestQuickMove(tACDItem, tInvLoc, pnSlotYMinMax, NULL); PlayItemInventorySound(tACDItem);
        return;
    }



    IVector2D tPoint(0, 0);
    ACDInventoryLocationSet(tInvLoc, tACDOwner.id, eSlot, 0, 0);
    GameError eError = ACDInventoryItemAllowedInSlot(tACDItem, tInvLoc, FALSE, FALSE);
    if (eError != GAMEERROR_NONE) {

        UIGame::DisplayError(GameErrorData(eError), FALSE, NULL);
        return;
    }

    if (ACDInventoryCanHold(tACDOwner, tACDItem, eSlot, tPoint, pnSlotYMinMax))
    {
        ACDInventoryLocationSet(tInvLoc, tACDOwner.id, eSlot, tPoint.x, tPoint.y);

        Player * ptPlayer = PlayerGetByACD(tACDOwner.id);

        CACDInventoryRequestQuickMove(tACDItem, tInvLoc, pnSlotYMinMax, ptPlayer); PlayItemInventorySound(tACDItem);
        return;
    }


    if (eSlot == SLOT_PLAYER_BACKPACK)
    {
        gtTutorialMgr.ShowTutorial(GetUIIDNone(), GlobalSNOGet(GLOBALSNO_TUTORIAL_INVENTORY_FULL), NULL, TRUE);
    }

    UIGame::DisplayError(GameErrorData(GAMEERROR_IARESULT_NOT_ENOUGH_ROOM), FALSE, NULL);
}





void UIInventory::OnRightClick(const UIID & uId)
{
    if (!UI_TestID(uId))
        return;
}









































































































































































































































































































































ACDID UIInventory::FindBestItemToUse(
    GBID gbidItem)
{
    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    BOOL bFoundExactMatch = FALSE;
    ACDID idBestACD = ACDID_INVALID;
    uint64 uBestCost = (uint64)-1;
    const uint64 MAX_STACK_SIZE = 256;
    uint64 uBestStackSize = MAX_STACK_SIZE;

    GBID gbidItemType = ItemGetItemTypeGBID(gbidItem);

    InventorySlotType tSlots[] = { SLOT_PLAYER_BACKPACK };



    for (int32 nSlotIndex = 0; nSlotIndex < (int32)(sizeof(tSlots) / sizeof(tSlots[0])); nSlotIndex++)
    {
        InventorySlotType eSlotType = tSlots[nSlotIndex];

        IVector2D tSlotSize;
        ACDInventorySlotGetSize(eSlotType, tSlotSize);

        IVector2D tPoint;
        for (tPoint.x = 0; tPoint.x < tSlotSize.x; tPoint.x++)
        {
            for (tPoint.y = 0; tPoint.y < tSlotSize.y; tPoint.y++)
            {
                ACDID idACDSquare = ACDInventoryGetSquareItem(tACDPlayer, eSlotType, tPoint);
                if (idACDSquare == ACDID_INVALID)
                    continue;
                ActorCommonData & tACD = ACDGet(idACDSquare);
                uint64 uCost = ItemGetCost(tACD, 0, 0, NULL, FALSE, FALSE);
                uint64 uStackSize = Math::Max<uint64>(1, tACD.GetStackCount());

                // Inference: one-line condition; original line 8435 columns 36/58/72 are the &&, || and < of this test.
                if (tACD.hGB.gbid == gbidItem && (!bFoundExactMatch || uStackSize < uBestStackSize))
                {
                    uBestStackSize = uStackSize;
                    idBestACD = idACDSquare;
                    bFoundExactMatch = TRUE;
                }


                else if (!bFoundExactMatch)
                {



                    if (ItemGetItemTypeGBID(tACD.hGB.gbid) == gbidItemType && uCost != (uint64)-1)
                    {




                        BOOL fFoundBetter = FALSE;
                        // Inference: original rows 8457 and 8461 (cols 22/35/49 = ==, &&, <); the cost < test has no row and is merged after the equal-cost test.

                        if (uBestCost == (uint64)-1)
                            fFoundBetter = TRUE;


                        else if (uCost == uBestCost && uStackSize < uBestStackSize)
                            fFoundBetter = TRUE;
                        else if (uCost < uBestCost) fFoundBetter = TRUE;
                        if (fFoundBetter)
                        {
                            idBestACD = idACDSquare;
                            uBestStackSize = uStackSize;
                            uBestCost = uCost;
                        }
                    }
                }
            }
        }
    }






    return idBestACD;
}



void UIInventory::UpdateTimerVisibility(UITimer & tTimer, const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem)
{
    uint32 dwCooldownEnd = ItemGetRemainingCooldownTime(tACDPlayer, tACDItem);
    if (dwCooldownEnd != (uint32)-1)
    {
        uint32 dwCooldownStart = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_START, ItemGetUsePower(tACDItem.hGB.gbid)));
        tTimer.SetTimer(dwCooldownStart, dwCooldownEnd);
    }
    else
        tTimer.SetTimer((uint32)-1, (uint32)-1);
}









void UIInventory::SetStashPageIndex(const int32 nPage)
{
    InvGlobals * ptInv = UI_GetInventoryData();
    IVector2D tSize;
    ACDInventoryGetSlotPageSize(SLOT_PLAYER_SHARED_STASH, tSize);
    ptInv->nStashPageOffset = tSize.y * nPage;
}








inline int32 UIInventory::GetStashOffset()
{


    InvGlobals * ptInv = UI_GetInventoryData();


    return ptInv->nStashPageOffset;
}

extern "C" void sub_71003e5660(CRefString text)
{
    UI_GetInventoryData()->unk_2f0 = text;
}
