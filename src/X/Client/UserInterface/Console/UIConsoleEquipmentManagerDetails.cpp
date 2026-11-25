#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleUIRActorManager.h"
#include "UIConsoleInventoryDetailsView.h"
#include "UIConsoleInventoryFilters.h"
#include "UIConsoleInventoryScreen.h"
#include "UIConsoleInventoryView.h"
#include "Wrappers/RingWrapper.h"
#include "../Controls/UIButtonControl.h"
#include "../Controls/UIConsoleScrollBoxControl.h"
#include "../Controls/UIHotbarButtonControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../UIID.h"
#include "../../Gfx/GfxCommon.h"
#include "../../../Common/ActorCommonData.h"
#include "../../../Common/GameSwig.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/HotbarButton.h"
#include "../../../Common/Player.h"
#include "../../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../../Sigma/DataStructures/XPooledList.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/MemoryPools.h"
#include "../../../../Contrib/Contrib/blz/include/blz/unordered_map.h"
#include "../../../Sigma/Math/Quaternion.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
typedef int32 LegendaryPowerSlotType;
typedef int32 SkillCategory;
// X/Common/Player.cpp
Player * PlayerGetByControllerIndex(int32 nControllerIndex);
PlayerClass PlayerGetPlayerClass(const Player * ptPlayer);
BOOL PlayerLoadoutGetDesiredSocketables(GBIDArray & arGBIDSocketables, const Player & tPlayer, int32 nLoadoutIndex);
void PlayerLoadoutFindSocketables(XPooledDataIDList & listACDIDSocketables, const ActorCommonData & tACDPlayer, const GBIDArray * arGBIDSocketables);
extern const InventorySlotType cgarPlayerLoadoutSocketableSlotsOrder[9];  // Player.cpp:173
// X/Common/Game.cpp
BOOL GameIsStartUp();
// X/Common/ActorCommonData.cpp
ACDID ACDFromANN(ACDNetworkName ann);
#include "../../Definitions/PreferencesDefinition.h"




namespace Prefs {
const GameplayPrefs & GetGameplayPrefs();
}  // namespace Prefs
// X/Common/Powers/PowersCommon.cpp
CRefString PowerGetRuneName(SNO snoPower, RuneType eRune);
// X/Common/GameBalance.cpp
CRefString GBIDToString(const GBID & gbid);
// X/Common/Definitions/StringList.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);
CRefString StringListFetch(LPCSTR szLabel);
// X/Common/Definitions/ActorDefinition.cpp
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);
#include "../../../Common/Networking/GameMessage.h"



// X/Common/Networking/MessageHeaders.h. Only the size is known.



// X/Client/Network/CGameMessage.cpp
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);
// X/Client/Lights.cpp
LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext);
// X/Client/CGame.cpp
uint32 CGameAsyncGetRenderDataIndex(int32 nIndex);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
UIWindow & UI_GetWindowFromID(const UIID & id);
UIText & UI_GetTextFromID(const UIID & id);
UIButton & UI_GetButtonFromID(const UIID & id);
UIHotbarButton & UI_GetHotbarButtonFromID(const UIID & id);
UIConsoleScrollBox & UI_GetConsoleScrollBoxFromID(const UIID & id);
UIRActor & UI_GetRActorFromID(const UIID & id);
SNO UI_GetDefaultHeroLight();
// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
BOOL IsButtonDown(int32 nControllerIndex, int32 eButton);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bUnused);
}  // namespace ControllerUtils
}  // namespace Console
#include "ControllerActions.h"
namespace Console {





typedef int32 ControllerInput;
typedef int32 ControllerActionId;







}  // namespace Console
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Console {
namespace Screens {
ControllerActionManager & GetActionManager();
void UpdateButtonLegendText();
void CloseScreenSet(int32 nUnused0, int32 nUnused1);
void ShowModalDialog(int32 nControllerIndex, const UIID & idDialog, BOOL bUnused, const UIID & idLightboxOverride);
void GoToPreviousScreenSet(int32 nControllerIndex, BOOL bUnused);
}  // namespace Screens
}  // namespace Console
// X/Client/UserInterface/Console/UIConsoleUtils.cpp
namespace UIUtils {
namespace Console {
SNO GetObserverForPlayerClass(PlayerClass ePlayerClass);
BOOL CanScrollContentInScrollBox(const UIID & idScrollBox);
void ScrollContentInScrollBox(const UIID & idScrollBox, const float flDelta);
}  // namespace Console
}  // namespace UIUtils
// X/Client/UserInterface/Interface/UIEquipmentManagerCommon.cpp
namespace UIEquipmentManager {
int32 GetSelectedLoadout();
bool LoadoutCanFindItem(ACDID idACDItem);
BOOL LoadoutCanEquipItemInSlot(const Player & tPlayer, int32 nLoadoutIndex, InventorySlotType eSlot);
void UpdateLoadoutPreview(const Player & tPlayer, const LoadoutSavedData & tLoadout, UIRActor & tRActor, bool bUnused);
void AssignLoadoutItemSocketables(ACDID * arLoadoutEquipmentSockets, int32 & nLoadoutEquipmentSocketsCount, blz::unordered_map<ACDID, int> & arConsumedSocketables, const LoadoutItemData & tLoadoutItem, const XPooledDataIDList & listACDIDSocketables);
}  // namespace UIEquipmentManager
// X/Client/UserInterface/Interface/UISkillsCommon.cpp
namespace UISkills {
SkillCategory GetSkillCategoryForSlot(const int32 nSlot, const ACDID idPlayer);
BOOL GetName(SNO snoPower, CRefString & sName);
}  // namespace UISkills
// X/Client/UserInterface/Console/UISkillsConsole.cpp
namespace UISkills {
namespace Console {
UIID GetUIIDForSkillSlot(const int32 nSlot, const UIID & idParent);
UIID GetUIIDForTraitSlot(const int32 nSlot, const UIID & idParent);
BOOL IconIsKanaiPowers(const UIID & idIcon);
BOOL IconIsSkill(const UIID & idIcon);
int32 SlotFromIcon(const UIID & idIcon);
void UpdateSkillSlotIcon(const UIID & idIcon, const int32 nSlot, const Player * ptPlayer, SNO snoPower, BOOL bEnabled, BOOL bSelected, BOOL bUnused0, BOOL bUnused1);
void UpdateTraitSlotIcon(const UIID & idIcon, const int32 nSlot, const Player * ptPlayer, SNO snoPower, BOOL bEnabled, BOOL bSelected, BOOL bUnused0, BOOL bUnused1);
CRefString GetSkillName(SNO snoPower);
void ShowSkillDescription(const UIID & idText, SNO snoPower, ActorCommonData & tACD, BOOL bIsTrait, BOOL bShowDetails);
CRefString GetRuneTextureFrame(RuneType eRune, BOOL bUnused);
void UpdatePlayerInfo(Player * ptPlayer);
void ShowRuneDescription(const UIID & idText, SNO snoPower, RuneType eRune, BOOL bShowDetails);
void ShowCategoryName(const UIID & idText, int32 nSlot, SkillCategory eCategory);
void ShowCategoryDescription(const UIID & idText, const Player * ptPlayer, int32 nSlot, SkillCategory eCategory);
SkillCategory GetSkillCategory(SNO snoPower, const ActorCommonData & tACD, BOOL bUnused);
}  // namespace Console
}  // namespace UISkills
// X/Client/UserInterface/Interface/UIEquipLegendaryPowers.cpp
namespace UIEquipLegendaryPowers {
CRefString GetPowerTooltipText(GBID gbidPower, bool bUnused);
void ShowPowerIcon(const UIID & idVisual, GBID gbidPower, BOOL bUnused);
}  // namespace UIEquipLegendaryPowers
// X/Client/UserInterface/UIGlobals.cpp
extern UIID UIDLG_EQUIPMENT_MANAGER_CONSOLE;
extern UIID UIRACTOR_CONSOLE_RING;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_EQUIPPED;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_CREST;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_PANE;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_MISSING_ITEM;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_RING;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_RING_CURSOR;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_RING_GLOW;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_VISUAL;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_VISUAL;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_RING_GLOW;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_ICON;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_CURSOR;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_GLOW;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNES;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_ICON;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILLS;
extern UIID UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_ICON;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_ARM_LEFT;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_ARM_RIGHT;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_BAG;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_BRACERS;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FEET;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FINGER_LEFT;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FINGER_RIGHT;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_HANDS;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_HEAD;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_LEGS;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_NECK;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_SHOULDER;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_TORSO;
extern UIID UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_WAIST;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO;
extern UIID UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_DESCRIPTION;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_HEADER;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_DETAILS;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_DETAILS;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_NAME;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO;
extern UIID UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_NAME;

namespace UIEquipmentManagerDetails {
void InitializeForGame();
void DisposeForGame();
void OpenForController(int32 nControllerIndex);
void Close();
BOOL IsActive();
void OpenInventory(int32 nControllerIndex);
void OpenSkills(int32 nControllerIndex);
void OnLeftStick(const UIID & id, const int32 nControllerIndex);
void OnButton1(const UIID & id, const int32 nControllerIndex);
void OnButton2(const UIID & id, const int32 nControllerIndex);
void OnButtonL2(const UIID & id, const int32 nControllerIndex);
void OnButtonR2(const UIID & id, const int32 nControllerIndex);
void OnUpdate();
}  // namespace UIEquipmentManagerDetails

static const int32 KANAI_SLOT = 10;  // line 56

class LoadoutItem : public UIInventory::Console::UIInventoryViewItemsProvider {  // size 0x40
public:
    virtual ~LoadoutItem() {}  // line 67

    LoadoutItem()  // line 70
        : nLoadoutIndex(-1)
    {
        m_arLoadoutItems.push_back(ACDID_INVALID);
    }

    void SetLoadout(int32 nIndex)  // line 76
    {
        nLoadoutIndex = nIndex;
    }

    virtual const ACDIDArray & GetItemsWithFilter(const UIFilterType eFilterType) const  // line 82
    {
        m_arLoadoutItems[0] = ACDID_INVALID;

        Player * ptPlayer = GetPlayer();
        if (ptPlayer && nLoadoutIndex >= 0 && nLoadoutIndex < 5)
        {
            const LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nLoadoutIndex];
            if (!tLoadout.bEmpty)
            {
                if (eFilterType == FILTER_BAG)
                {
                    const LoadoutPotionData & tPotionData = tLoadout.tEquippedPotion;
                    ACDID idPotion = ACDFromANN(tPotionData.annPotion);
                    m_arLoadoutItems[0] = idPotion;
                }
                else
                {
                    InventorySlotType eSlot = UIInventory::Console::GetSlotForFilterType(eFilterType);
                    if (eSlot >= 1 && eSlot <= 13)
                    {
                        const LoadoutItemData & tItemData = tLoadout.tEquipment[eSlot - 1];
                        ACDID idLoadoutItem = ACDFromANN(tItemData.annItem);
                        m_arLoadoutItems[0] = idLoadoutItem;
                    }
                }
            }
        }

        return m_arLoadoutItems;
    }

    virtual CRefString GetWarningMessage() const  // line 116
    {
        UIFilterType eFilterType = GetFilter();
        InventorySlotType eSlot = UIInventory::Console::GetSlotForFilterType(eFilterType);
        if (eSlot != -1)
        {
            const ActorCommonData * ptACDPlayer = ACDTryToGet(GetPlayer()->idPlayerACD);
            if (ptACDPlayer)
            {
                const LoadoutSavedData & tLoadout = GetPlayer()->tLoadouts[nLoadoutIndex];
                if (!tLoadout.bEmpty)
                {
                    GBIDArray arGBIDSocketables;
                    PlayerLoadoutGetDesiredSocketables(arGBIDSocketables, *GetPlayer(), nLoadoutIndex);

                    XPooledDataIDList listACDIDSocketables(MemoryPoolsGetDATAIDListPool());
                    PlayerLoadoutFindSocketables(listACDIDSocketables, *ptACDPlayer, &arGBIDSocketables);

                    ACDID arLoadoutEquipmentSockets[13][3];
                    int32 nLoadoutEquipmentSocketsCount[13];
                    SigmaMemoryClear(nLoadoutEquipmentSocketsCount, sizeof(nLoadoutEquipmentSocketsCount));

                    blz::unordered_map<ACDID, int> arConsumedSocketables;
                    for (int32 nOrderedSlot = 0; cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] != -1; nOrderedSlot++)
                    {
                        int32 nEquipmentSlotIndex = cgarPlayerLoadoutSocketableSlotsOrder[nOrderedSlot] - 1;
                        const LoadoutItemData & tLoadoutItem = tLoadout.tEquipment[nEquipmentSlotIndex];
                        UIEquipmentManager::AssignLoadoutItemSocketables(arLoadoutEquipmentSockets[nEquipmentSlotIndex], nLoadoutEquipmentSocketsCount[nEquipmentSlotIndex], arConsumedSocketables, tLoadoutItem, listACDIDSocketables);
                    }

                    int32 nBodySlotIndex = eSlot - 1;
                    if (tLoadout.tEquipment[nBodySlotIndex].nSocketsCount != nLoadoutEquipmentSocketsCount[nBodySlotIndex])
                    {
                        return StringListFetch("ConsoleEquipmentManager:WarningMissingGemDetail");
                    }
                    return "";
                }
            }
        }

        return "";
    }

private:
    mutable ACDIDArray m_arLoadoutItems;  // 0x20
    int32 nLoadoutIndex;  // 0x38
};
static_assert(sizeof(LoadoutItem) == 0x40, "LoadoutItem");

class MissingItemView : public UIInventory::Console::UIInventoryView {  // size 0x60
public:
    MissingItemView(const UIID & idRoot)  // line 180
        : UIInventory::Console::UIInventoryView(idRoot)
    {
    }

    virtual ~MissingItemView() {}  // line 185

protected:
    virtual void OnShow(const BOOL bShow)  // line 188
    {
        UI_GetWindowFromID(GetRootUIID()).SetActive(bShow);
    }
};
static_assert(sizeof(MissingItemView) == 0x60, "MissingItemView");

class DetailsInvScreen : public UIInventory::Console::UIInventoryScreen {  // size 0x3d0
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_LIST = 1,
    };
public:
    DetailsInvScreen();

    virtual ~DetailsInvScreen() {}  // line 198

    BOOL IsActive() const  // line 210
    {
        return UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY).IsActive();
    }

    virtual void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void MarkRingUIAsDirty();
    virtual void Update();

private:
    virtual BOOL ShouldEnableRingIcon(const UIFilterType eFilterType);

    virtual void InitializeRingWrapper(::Console::RingWrapper & tRingWrapper)  // line 230
    {
        tRingWrapper.WrapUsingContainerControl(UIWND_EQUIPMENT_MANAGER_CONSOLE_RING, UIWND_EQUIPMENT_MANAGER_CONSOLE_RING_CURSOR, NULL, FALSE);
    }

    virtual UIFilterType GetDefaultRingSelection() const  // line 236
    {
        return FILTER_PLAYER_START;
    }

    virtual void OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType);

    virtual void InitRingFilters(UIIDToFilterMap & tFilterMap)  // line 246
    {
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_ARM_LEFT, FILTER_MAIN_HAND);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_ARM_RIGHT, FILTER_OFF_HAND);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_BAG, FILTER_BAG);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_BRACERS, FILTER_WRIST);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FEET, FILTER_FEET);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FINGER_LEFT, FILTER_RING_LEFT);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_FINGER_RIGHT, FILTER_RING_RIGHT);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_HANDS, FILTER_HANDS);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_HEAD, FILTER_HEAD);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_LEGS, FILTER_LEGS);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_NECK, FILTER_NECK);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_SHOULDER, FILTER_SHOULDERS);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_TORSO, FILTER_CHEST);
        tFilterMap.SetAt_Tracked(UIBTN_EQUIPMENT_MANAGER_CONSOLE_RING_WAIST, FILTER_BELT);
    }

    virtual UIInventory::Console::UIInventoryView * GetPrimaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)  // line 265
    {
        Player * ptPlayer = GetPlayer();
        if (ptPlayer == NULL)
        {
            return &m_tLoadoutView;
        }

        int32 nLoadoutIndex = UIEquipmentManager::GetSelectedLoadout();
        const LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[nLoadoutIndex];
        if (tLoadout.bEmpty)
        {
            return &m_tLoadoutView;
        }

        InventorySlotType eSlot = UIInventory::Console::GetSlotForFilterType(GetFilter());
        int32 nBodySlotIndex = eSlot - 1;
        if (nBodySlotIndex < 0 || nBodySlotIndex >= 13)
        {
            return &m_tLoadoutView;
        }

        ACDNetworkName annItem = tLoadout.tEquipment[nBodySlotIndex].annItem;
        ACDID idACDItem = ACDFromANN(annItem);
        if (!UIEquipmentManager::LoadoutCanFindItem(idACDItem) && annItem != ANN_INVALID)
        {
            return &m_tMissingItemView;
        }

        return &m_tLoadoutView;
    }

    virtual UIInventory::Console::UIInventoryView * GetSecondaryViewForMode(const UIInventory::Console::UIInventoryScreen::BrowsingMode eBrowsingMode)  // line 304
    {
        return &m_tEquippedView;
    }

    virtual void OnOpen();
    virtual void OnClose();
    virtual void OnSetActive(BOOL bActive);
    virtual void UpdateControllerActions(::Console::ControllerActionManager & tActionManager);
    void UpdateWarningIcons();

    LoadoutItem m_tLoadoutItemProvider;  // 0x200
    UIInventory::Console::EquippedItem m_tEquippedItemProvider;  // 0x240
    UIInventory::Console::UIInventoryDetailsView m_tLoadoutView;  // 0x278
    MissingItemView m_tMissingItemView;  // 0x2f0
    UIInventory::Console::UIInventoryDetailsView m_tEquippedView;  // 0x350
    BOOL m_fRingNeedsUpdating;  // 0x3c8
};
static_assert(sizeof(DetailsInvScreen) == 0x3d0, "DetailsInvScreen");

// 2.7.7 Kanai's powers panel (unnamed 0x30-byte class; same definition as in UISkillsConsole.cpp).
// sub_710019b630 (this, parent UIID) builds the four owned 0xd8-byte children (sub_710019aec0);
// the inline destructor frees them in reverse order.
struct Unk19aec0 { uint8 unk_00[0xd8]; };
struct Unk19b630 {  // size 0x30
    ~Unk19b630()
    {
        delete unk_00[3];
        delete unk_00[2];
        delete unk_00[1];
        delete unk_00[0];
    }
    Unk19aec0 * unk_00[4];  // 0x00
    uint64 unk_20;  // 0x20
    UIID unk_28;  // 0x28
};
static_assert(sizeof(Unk19b630) == 0x30, "Unk19b630");

class DetailsSkillsScreen {  // size 0xc8
    enum ButtonActions {
        ACTION_BACK = 0,
        ACTION_SCROLL_DETAIL_INFO = 1,
    };
public:
    DetailsSkillsScreen();

    BOOL IsActive() const  // line 351
    {
        return UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS).IsActive();
    }

    void HandleLeftStickInput(const int32 nControllerIndex);
    void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void Update();
    void Open();
    void Close();
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    void UpdateSkillSlotIcon(Player * ptPlayer, const int32 nSlot);
    void UpdateTraitSlotIcon(Player * ptPlayer, const int32 nSlot);
    void UpdateKanaisPowersSlotIcon(Player * ptPlayer);
    void UpdateSkillRing();
    void UpdateUIForSelectedSkill(const int32 nIconSlot, Player * ptPlayer);
    void UpdateUIForSelectedTrait(const int32 nIconSlot, Player * ptPlayer);

    int32 GetSelectedSlot() const  // line 399
    {
        const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
        if (idSelectedIcon != GetUIIDNone())
        {
            return UISkills::Console::SlotFromIcon(idSelectedIcon);
        }
        return -1;
    }

    ::Console::RingWrapper m_tRingUI;  // 0x00
    ::Console::AnalogStickInputFilter m_tRingStickInputFilter;  // 0x68
    BOOL m_fCurrentlyOpening;  // 0x90
    // 2.7.7: Kanai's powers panel, built by sub_710019b630 for
    // UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS (EquipmentManagerDetails
    // InitializeForGame 0x71001a5f40; UISkills::Console::InitializeForGame builds another).
    Unk19b630 unk_98;  // 0x98
private:
    BOOL CanScrollDetails();
    void ResetDetailScrollPosition(const UIID & idScrollBox);
    void ResetDetailScrollPositions();
    void ScrollDetailInfo(const float nDelta);
};
static_assert(sizeof(DetailsSkillsScreen) == 0xc8, "DetailsSkillsScreen");

enum class DetailsScreen : int {
    INVENTORY = 0,
    SKILLS = 1,
};

class EquipManDetailsGlobals {  // size 0x4a8 (InitializeForGame: new(0x4a8))
public:
    EquipManDetailsGlobals();
    void HandleLeftStickInput(const int32 nControllerIndex);
    void HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton);
    void Update();
    void OpenForController(int32 nControllerIndex);
    void Close();
    void OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected);
    void SwitchToScreen(DetailsScreen eScreen);

    BOOL IsActive() const  // line 477
    {
        return tInventoryScreen.IsActive() || tSkillsScreen.IsActive();
    }

    Player * GetPlayer()  // line 482
    {
        return m_ptPlayer;
    }

private:
    DetailsScreen eCurrentScreen;  // 0x00
    int32 m_nControllerIndex;  // 0x04
    Player * m_ptPlayer;  // 0x08
    DetailsInvScreen tInventoryScreen;  // 0x10
    DetailsSkillsScreen tSkillsScreen;  // 0x3e0
};
static_assert(sizeof(EquipManDetailsGlobals) == 0x4a8, "EquipManDetailsGlobals");

EquipManDetailsGlobals * sgptEquipManDetailsGlobals;  // line 499

static BOOL sCanStillInspectTarget();
static void AttachLightToModel(UIRActor & tModel);
void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected);

DetailsInvScreen::DetailsInvScreen()
    : m_tLoadoutView(m_tLoadoutItemProvider, UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT, 0)
    , m_tMissingItemView(UIWND_EQUIPMENT_MANAGER_CONSOLE_MISSING_ITEM)
    , m_tEquippedView(m_tEquippedItemProvider, UIWND_EQUIPMENT_MANAGER_CONSOLE_EQUIPPED, 0)
{
    m_tLoadoutView.SetHeaderText(StringListFetch("ConsoleEquipmentManager:LoadoutViewHeader"));
    m_tMissingItemView.SetHeaderText(StringListFetch("ConsoleEquipmentManager:LoadoutViewHeader"));
    m_tEquippedView.SetHeaderText(StringListFetch("ConsoleInventory:Equipped"));
    m_fRingNeedsUpdating = FALSE;
}

void DetailsInvScreen::OnFilterTypeChanged(const UIFilterType eOldFilterType, const UIFilterType eNewFilterType)
{
    m_tLoadoutItemProvider.SetFilter(eNewFilterType);
    m_tEquippedItemProvider.SetFilter(eNewFilterType);

    ResetDetailsScroll();

    if (GetPrimaryView())
    {
        GetPrimaryView()->ForceUpdate(UIInventory::Console::RESET_SELECTION);
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

void DetailsInvScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    switch (eInputButton)
    {
    case 6:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            ScrollDetails(-4.0f);
        }
        break;

    case 7:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            ScrollDetails(4.0f);
        }
        break;
    }
}

static void sUpdatePlayerInfoUI(const Player & tPlayer, const UIID & idParentControl)
{
    UIID idRActorControl = UIRACTOR_CONSOLE_RING;
    const LoadoutSavedData & tLoadout = tPlayer.tLoadouts[UIEquipmentManager::GetSelectedLoadout()];

    ::Console::UIRActorManager::ShouldCreateRActor(::Console::UIRActorManager::RACTOR_CONTROL_RING, ::Console::UIRActorManager::RACTOR_CHARACTER_PLAYER, TRUE, TRUE);

    UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
    UIEquipmentManager::UpdateLoadoutPreview(tPlayer, tLoadout, tModel, false);
    tModel.SetObserver(UIUtils::Console::GetObserverForPlayerClass(PlayerGetPlayerClass(&tPlayer)));

    UIID idName = UIIDCreateFromParent(idParentControl, ".LoadoutName");
    UI_GetTextFromID(idName).SetText(tLoadout.usName, FALSE);
}

BOOL DetailsInvScreen::ShouldEnableRingIcon(const UIFilterType eFilterType)
{
    const ACDIDArray & arLoadoutItems = m_tLoadoutItemProvider.GetItemsWithFilter(eFilterType);
    const ACDIDArray & arEquippedItems = m_tEquippedItemProvider.GetItemsWithFilter(eFilterType);
    if ((arLoadoutItems.size() > 0 && arLoadoutItems[0] != ACDID_INVALID) ||
        (arEquippedItems.size() > 0 && arEquippedItems[0] != ACDID_INVALID))
    {
        return TRUE;
    }

    int32 nLoadoutIndex = UIEquipmentManager::GetSelectedLoadout();
    InventorySlotType eSlot = UIInventory::Console::GetSlotForFilterType(eFilterType);
    return !UIEquipmentManager::LoadoutCanEquipItemInSlot(*GetPlayer(), nLoadoutIndex, eSlot);
}

inline void DetailsInvScreen::MarkRingUIAsDirty()
{
    m_fRingNeedsUpdating = TRUE;
}

void DetailsInvScreen::Update()
{
    if (m_fRingNeedsUpdating)
    {
        m_fRingNeedsUpdating = FALSE;
        UpdateRingIcons();

        if (GetFilter() == FILTER_NONE)
        {
            SetFilterType(GetInitialRingSelection(FILTER_PLAYER_START, FILTER_PLAYER_END));
        }

        const UIID & idParentControl = UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_PANE;
        sUpdatePlayerInfoUI(*GetPlayer(), idParentControl);

        UIID idRActorControl = UIIDCreateFromParent(idParentControl, ".Hero3DModel");
        UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
        tModel.Cleanup();
        AttachLightToModel(tModel);
    }

    UIInventory::Console::UIInventoryScreen::Update();
}

void DetailsInvScreen::OnOpen()
{
    Player * ptPlayer = sgptEquipManDetailsGlobals->GetPlayer();

    SetPlayer(ptPlayer);
    m_tLoadoutItemProvider.SetPlayer(ptPlayer);
    m_tLoadoutItemProvider.SetLoadout(UIEquipmentManager::GetSelectedLoadout());
    m_tEquippedItemProvider.SetPlayer(ptPlayer);

    UpdateRingIcons();
    UpdateMainhandAndOffhandRingIcons(ptPlayer);
    UpdateWarningIcons();
    MarkRingUIAsDirty();

    SetPrimaryView(GetPrimaryViewForMode(GetBrowsingMode()));
    SetSecondaryView(GetSecondaryViewForMode(GetBrowsingMode()));
    SetFocusedView(NULL);

    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_INVENTORY_OPEN), cgtPSODefaults);
}

void DetailsInvScreen::OnSetActive(BOOL bActive)
{
    if (bActive)
    {
        const UIID & idParentControl = UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_PANE;
        sUpdatePlayerInfoUI(*GetPlayer(), idParentControl);

        PlayerClass eClass = PlayerGetPlayerClass(GetPlayer());
        LPCSTR szLabel;
        switch (eClass)
        {
        case PLAYER_CLASS_DEMONHUNTER:
            szLabel = "UI:Console_CrestDemonHunter";
            break;
        case PLAYER_CLASS_BARBARIAN:
            szLabel = "UI:Console_CrestBarbarian";
            break;
        case PLAYER_CLASS_WIZARD:
            szLabel = "UI:Console_CrestWizard";
            break;
        case PLAYER_CLASS_WITCHDOCTOR:
            szLabel = "UI:Console_CrestWitchDoctor";
            break;
        case PLAYER_CLASS_MONK:
            szLabel = "UI:Console_CrestMonk";
            break;
        case PLAYER_CLASS_CRUSADER:
            szLabel = "UI:Console_CrestCrusader";
            break;
        default:
            szLabel = "";
            break;
        }
        UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_CREST).SetTextureFrame(szLabel);

        ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
        UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY).SetActive(TRUE);
    }
    else
    {
        UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY).SetActive(FALSE);
    }
}

void DetailsInvScreen::UpdateControllerActions(::Console::ControllerActionManager & tActionManager)
{
    tActionManager.ClearAllActions();
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());

    if (CanScrollDetails())
    {
        CRefString sScrollPrompt = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_CONSOLE_CHARACTER_DETAILS), "ScrollDetails");
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_LIST, sScrollPrompt);
    }
}

void DetailsInvScreen::UpdateWarningIcons()
{
    const UIIDToFilterMap & tFilterMap = m_tFilterMap;
    int32 nLoadoutIndex = UIEquipmentManager::GetSelectedLoadout();
    UIFilterType eFilterType;
    UIID idKey;
    POS pos = tFilterMap.GetStartAssoc();
    while (pos)
    {
        tFilterMap.GetNextAssoc(pos, idKey, eFilterType);

        UIID idWarningIndicator = UIIDCreateFromParent(idKey, ".LayoutRoot.LoadoutError");
        InventorySlotType eSlot = UIInventory::Console::GetSlotForFilterType(eFilterType);
        BOOL bHasWarning = eSlot != -1 && !UIEquipmentManager::LoadoutCanEquipItemInSlot(*GetPlayer(), nLoadoutIndex, eSlot);
        UI_GetWindowFromID(idWarningIndicator).SetActive(bHasWarning);
    }
}

void DetailsInvScreen::OnClose()
{
    ::Console::UIRActorManager::CloseRActor();
    SetFilterType(FILTER_NONE);
    SetPrimaryView(NULL);
    SetSecondaryView(NULL);
}

extern "C" void sub_710019b630(void * pThis, const UIID & idParent);

inline DetailsSkillsScreen::DetailsSkillsScreen()
    : m_fCurrentlyOpening(FALSE)
{
    sub_710019b630(&unk_98, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS);
    m_tRingStickInputFilter.AddRule(::Console::AnalogStickInputFilter::Rule::CatchAllRule);
}

BOOL sShouldIncludeInRing(const UIID & idControl)
{
    return idControl != UIWND_EQUIPMENT_MANAGER_CONSOLE_RING_GLOW &&
           idControl != UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_GLOW &&
           idControl != UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_RING_GLOW;
}

void DetailsSkillsScreen::Open()
{
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS).SetActive(TRUE);
    m_fCurrentlyOpening = TRUE;

    Player & tPlayer = *sgptEquipManDetailsGlobals->GetPlayer();
    UpdateSkillRing();

    m_tRingUI.WrapUsingContainerControl(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_CURSOR, sShouldIncludeInRing, FALSE);
    UIID idSkill = UISkills::Console::GetUIIDForSkillSlot(0, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING);
    m_tRingUI.SelectControl(idSkill, IconSelectionChanged);

    const UIID & idParentControl = UIWND_EQUIPMENT_MANAGER_CONSOLE_INVENTORY_PANE;
    sUpdatePlayerInfoUI(tPlayer, idParentControl);

    UIID idRActorControl = UIIDCreateFromParent(idParentControl, ".Hero3DModel");
    UIRActor & tModel = UI_GetRActorFromID(idRActorControl);
    tModel.Cleanup();
    AttachLightToModel(tModel);

    m_fCurrentlyOpening = FALSE;
    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_INVENTORY_OPEN), cgtPSODefaults);
}

inline void DetailsSkillsScreen::HandleLeftStickInput(const int32 nControllerIndex)
{
    Vector2D vecStick = ::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE);
    if (m_tRingStickInputFilter.HandleInput(vecStick, FALSE) == ::Console::AnalogStickInputFilter::Rule::CATCH_ALL_INPUT)
    {
        m_tRingUI.UpdateInput(nControllerIndex, IconSelectionChanged, NULL);
    }
}

inline void DetailsSkillsScreen::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    switch (eInputButton)
    {
    case 6:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 7))
        {
            ScrollDetailInfo(-4.0f);
        }
        break;

    case 7:
        if (!::Console::ControllerUtils::IsButtonDown(nControllerIndex, 6))
        {
            ScrollDetailInfo(4.0f);
        }
        break;
    }
}

inline void DetailsSkillsScreen::Close()
{
    SimpleMessage tMsg = {};
    MessageSendToServer(CMSG_INSPECT_END, &tMsg, sizeof(tMsg), sgptEquipManDetailsGlobals->GetPlayer());

    ::Console::UIRActorManager::CloseRActor();
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS).SetActive(FALSE);
}

void DetailsSkillsScreen::Update()
{
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    if (tActionManager.AreActionsDirty())
    {
        tActionManager.SetActionsDirtyFlag(FALSE);
        tActionManager.ClearAllActions();
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_BACK, StringListFetch("ConsoleUI:Back").str());

        if (CanScrollDetails())
        {
            CRefString sScrollPrompt = StringListFetch("ConsoleSkills:ScrollSkillDescriptions");
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
            tActionManager.SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_DETAIL_INFO, sScrollPrompt);
        }

        ::Console::Screens::UpdateButtonLegendText();
    }
}

void DetailsSkillsScreen::UpdateSkillSlotIcon(Player * ptPlayer, const int32 nSlot)
{
    LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
    UIID idSkillIcon = UISkills::Console::GetUIIDForSkillSlot(nSlot, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING);
    BOOL fSlotSelected = GetSelectedSlot() == nSlot;
    SNO snoPower = tLoadout.tActiveSkills[nSlot].snoPower;
    UIButton & tBtnIcon = UI_GetButtonFromID(idSkillIcon);
    BOOL fEnabled = snoPower != -1;
    tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    tBtnIcon.SetEnabled(fEnabled);
    UISkills::Console::UpdateSkillSlotIcon(idSkillIcon, nSlot, ptPlayer, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
}

void DetailsSkillsScreen::UpdateTraitSlotIcon(Player * ptPlayer, const int32 nSlot)
{
    LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
    UIID idTraitSlot = UISkills::Console::GetUIIDForTraitSlot(nSlot, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING);
    BOOL fSlotSelected = nSlot + 6 == GetSelectedSlot();
    SNO snoPower = tLoadout.snoTraits[nSlot];
    UIButton & tBtnIcon = UI_GetButtonFromID(idTraitSlot);
    BOOL fEnabled = snoPower != -1;
    tBtnIcon.SetShader(fEnabled ? -1 : GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT_INACTIVE));
    tBtnIcon.SetEnabled(fEnabled);
    UISkills::Console::UpdateTraitSlotIcon(idTraitSlot, nSlot, ptPlayer, snoPower, fEnabled, fSlotSelected, FALSE, TRUE);
}

void DetailsSkillsScreen::UpdateKanaisPowersSlotIcon(Player * ptPlayer)
{
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON).SetUserData((void *)KANAI_SLOT);

    bool bAssigned = ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[0] != -1 ||
                     ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[1] != -1 ||
                     ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[2] != -1 ||
                     ptPlayer->tHeroStateData.tSavedData.gbidLegendaryPowers[3] != -1;

    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_UNASSIGNED).SetActive(!bAssigned);
    UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_RING_ICON_ASSIGNED).SetActive(bAssigned);
}

void DetailsSkillsScreen::UpdateSkillRing()
{
    Player * ptPlayer = sgptEquipManDetailsGlobals->GetPlayer();

    for (int32 i = 0; i < 6; i++)
    {
        UpdateSkillSlotIcon(ptPlayer, i);
    }

    for (int32 i = 0; i < 4; i++)
    {
        UpdateTraitSlotIcon(ptPlayer, i);
    }

    UpdateKanaisPowersSlotIcon(ptPlayer);
}

// Ternary arm order at 1034 and 1065 from the original columns (the call of the true arm sits
// before StringListFetch: 1034:52/96, 1065:57/93); `!= SNO_NONE` at 1034/1055 fits the column
// widths. The rune test at 1065 is `!= -1` here (RUNE_TYPE_INVALID, FastAttribConstantsSwig.h,
// fits the width but that header is not included by this unit).
void DetailsSkillsScreen::UpdateUIForSelectedSkill(const int32 nIconSlot, Player * ptPlayer)
{
    LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
    int32 nSlot = nIconSlot;
    SNO snoPower = tLoadout.tActiveSkills[nSlot].snoPower;

    CRefString sSkillName = snoPower != SNO_NONE ? UISkills::Console::GetSkillName(snoPower) : StringListFetch("ConsoleSkills:Skills");

    UIHotbarButton & tSkillIcon = UI_GetHotbarButtonFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tSkillIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_NAME).SetText(sSkillName.str(), FALSE);

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }
    else
    {
        UISkills::Console::ShowSkillDescription(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO, snoPower, tACDPlayer, FALSE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }

    RuneType eRune = snoPower != SNO_NONE ? tLoadout.tActiveSkills[nSlot].eRune : -1;
    UIHotbarButton & tRuneIcon = UI_GetHotbarButtonFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_ICON);
    CRefString sRuneTexture = UISkills::Console::GetRuneTextureFrame(eRune, TRUE);
    ImageTextureFrame tTexFrame;
    ImageCollectionGetImageInfo(sRuneTexture.str(), tTexFrame);
    tRuneIcon.SetAllTextureFrames(tTexFrame);

    CRefString sRuneName = eRune != -1 ? PowerGetRuneName(snoPower, eRune) : StringListFetch("ConsoleSkills:SkillRunes");
    UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_NAME).SetText(sRuneName.str(), FALSE);

    if (eRune == -1)
    {
        UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned"), FALSE);
    }
    else
    {
        UISkills::Console::UpdatePlayerInfo(ptPlayer);
        UISkills::Console::ShowRuneDescription(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO, snoPower, eRune, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }
}

BOOL DetailsSkillsScreen::CanScrollDetails()
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    if (UISkills::Console::IconIsKanaiPowers(idSelectedIcon))
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY);
    }
    else if (UISkills::Console::IconIsSkill(idSelectedIcon))
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO) ||
               UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO);
    }
    else
    {
        return UIUtils::Console::CanScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO);
    }
}

inline void DetailsSkillsScreen::ResetDetailScrollPosition(const UIID & idScrollBox)
{
    UI_GetConsoleScrollBoxFromID(idScrollBox).SetScrollPos(0.0f);
}

inline void DetailsSkillsScreen::ResetDetailScrollPositions()
{
    ResetDetailScrollPosition(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO);
    ResetDetailScrollPosition(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO);
    ResetDetailScrollPosition(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON);
    ResetDetailScrollPosition(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR);
    ResetDetailScrollPosition(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY);
}

void DetailsSkillsScreen::ScrollDetailInfo(const float nDelta)
{
    const UIID & idSelectedIcon = m_tRingUI.GetSelectedControlID();
    if (UISkills::Console::IconIsKanaiPowers(idSelectedIcon))
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY, nDelta);
    }
    else if (UISkills::Console::IconIsSkill(idSelectedIcon))
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILL_INFO, nDelta);
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNE_INFO, nDelta);
    }
    else
    {
        UIUtils::Console::ScrollContentInScrollBox(UISCROLL_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO, nDelta);
    }
}

void DetailsSkillsScreen::UpdateUIForSelectedTrait(const int32 nIconSlot, Player * ptPlayer)
{
    LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
    int32 nSlot = nIconSlot - 6;
    SNO snoPower = tLoadout.snoTraits[nSlot];

    UIHotbarButton & tIcon = UI_GetHotbarButtonFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_ICON);
    HotbarButtonData tHotbarButtonData;
    tHotbarButtonData.SetSkill(snoPower, -1);
    tIcon.UpdateSkillButtonState(tHotbarButtonData, FALSE, TRUE, TRUE, FALSE, FALSE);

    CRefString sPowerName;
    if (!UISkills::GetName(snoPower, sPowerName))
    {
        sPowerName = StringListFetch("ConsoleSkills:PassiveSkillsTitle");
    }
    UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_NAME).SetText(sPowerName.str(), FALSE);

    if (snoPower == -1)
    {
        UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO).SetText(StringListFetch("ConsoleSkills:NoneAssigned").str(), FALSE);
    }
    else
    {
        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        UISkills::Console::ShowSkillDescription(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS_INFO, snoPower, tACDPlayer, TRUE, Prefs::GetGameplayPrefs().ShowAdvancedTooltips());
    }
}

static void sUpdateKanaisPowerUI(const Player * ptPlayer, LegendaryPowerSlotType eSlot, const UIID & idVisual, const UIID & idNameText, const UIID & idDescText)
{
    CRefString sPowerName;
    CRefString sPowerDesc;
    const LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
    GBID gbidPower = tLoadout.gbidLegendaryPowers[eSlot];
    if (gbidPower == -1)
    {
        sPowerName = StringListFetch("ConsoleSkills:NoKanaiPowerAssigned");
    }
    else
    {
        sPowerName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEMS), GBIDToString(gbidPower).str());
        sPowerDesc = UIEquipLegendaryPowers::GetPowerTooltipText(gbidPower, false);
    }

    UIEquipLegendaryPowers::ShowPowerIcon(idVisual, gbidPower, TRUE);
    UI_GetTextFromID(idNameText).SetText(sPowerName, FALSE);
    UI_GetTextFromID(idDescText).SetText(sPowerDesc, FALSE);
}

void DetailsSkillsScreen::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)
{
    if (idIcon == GetUIIDNone())
    {
        return;
    }

    if (!sCanStillInspectTarget())
    {
        ::Console::Screens::CloseScreenSet(1, 0);
        return;
    }

    Player * ptPlayer = sgptEquipManDetailsGlobals->GetPlayer();

    if (UISkills::Console::IconIsKanaiPowers(idIcon))
    {
        if (bSelected)
        {
            UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS).SetActive(TRUE);
            UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_HEADER).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_NAME).SetText(StringListFetch("ConsoleSkills:KanaiSlotName"), FALSE);
            UI_GetTextFromID(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_DESCRIPTION).SetText(StringListFetch("ConsoleSkills:Cat_Kanai_Description"), FALSE);

            sUpdateKanaisPowerUI(ptPlayer, 0, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_VISUAL, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_NAME, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_WEAPON_DETAILS);
            sUpdateKanaisPowerUI(ptPlayer, 1, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_VISUAL, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_NAME, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_ARMOR_DETAILS);
            sUpdateKanaisPowerUI(ptPlayer, 2, UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_VISUAL, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_NAME, UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS_JEWELRY_DETAILS);
        }
    }
    else
    {
        if (bSelected)
        {
            BOOL fIsSkill = UISkills::Console::IconIsSkill(idIcon);
            int32 nSlot = UISkills::Console::SlotFromIcon(idIcon);
            UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_KANAIS_POWERS).SetActive(FALSE);

            LoadoutSavedData & tLoadout = ptPlayer->tLoadouts[UIEquipmentManager::GetSelectedLoadout()];
            SkillCategory eCategory = -1;
            if (fIsSkill)
            {
                SNO snoPower = tLoadout.tActiveSkills[nSlot].snoPower;
                if (snoPower == -1)
                {
                    eCategory = UISkills::GetSkillCategoryForSlot(nSlot, ptPlayer->idPlayerACD);
                }
                else
                {
                    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
                    eCategory = UISkills::Console::GetSkillCategory(snoPower, tACDPlayer, FALSE);
                }
            }

            UISkills::Console::ShowCategoryName(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_HEADER, nSlot, eCategory);
            UISkills::Console::ShowCategoryName(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_NAME, nSlot, eCategory);
            UISkills::Console::ShowCategoryDescription(UITXT_EQUIPMENT_MANAGER_CONSOLE_SKILLS_CATEGORY_DESCRIPTION, ptPlayer, nSlot, eCategory);

            UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_SKILLS).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RUNES).SetActive(fIsSkill);
            UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_SKILLS).SetActive(!fIsSkill);

            UIWindow & tWndCursor = UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_CURSOR);
            if (fIsSkill)
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor");
                UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_GLOW).SetActive(TRUE);
                UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_RING_GLOW).SetActive(FALSE);
                UpdateUIForSelectedSkill(nSlot, ptPlayer);
            }
            else
            {
                tWndCursor.SetTextureFrame("UI:Console_Ring_Cursor2");
                UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_RING_GLOW).SetActive(FALSE);
                UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_SKILLS_PASSIVE_RING_GLOW).SetActive(TRUE);
                UpdateUIForSelectedTrait(nSlot, ptPlayer);
            }
        }

        UIID idSparkle;
        idSparkle.SetFromParent(idIcon, "LayoutRoot.FX");
        UI_GetWindowFromID(idSparkle).SetActive(bSelected);

        UIID idButton;
        idButton.SetFromParent(idIcon, "LayoutRoot.Button");
        UI_GetButtonFromID(idButton).SetButtonState(bSelected ? BS_MOUSEOVER : BS_NORMAL, FALSE, m_fCurrentlyOpening);

        ResetDetailScrollPositions();
    }

    ::Console::Screens::GetActionManager().SetActionsDirtyFlag(TRUE);
}

inline EquipManDetailsGlobals::EquipManDetailsGlobals()
    : eCurrentScreen(DetailsScreen::INVENTORY)
    , m_nControllerIndex(-1)
    , m_ptPlayer(NULL)
{
}

inline void EquipManDetailsGlobals::HandleLeftStickInput(const int32 nControllerIndex)
{
    switch (eCurrentScreen)
    {
    case DetailsScreen::INVENTORY:
        tInventoryScreen.HandleLeftStickInput(nControllerIndex);
        break;

    case DetailsScreen::SKILLS:
        tSkillsScreen.HandleLeftStickInput(nControllerIndex);
        break;
    }
}

inline void EquipManDetailsGlobals::HandleButtonInput(const int32 nControllerIndex, const InputButton eInputButton)
{
    if (GetPlayer()->nControllerIndex != nControllerIndex)
    {
        return;
    }

    if (eInputButton == 1)
    {
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, TRUE);
        return;
    }

    switch (eCurrentScreen)
    {
    case DetailsScreen::INVENTORY:
        tInventoryScreen.HandleButtonInput(nControllerIndex, eInputButton);
        break;

    case DetailsScreen::SKILLS:
        tSkillsScreen.HandleButtonInput(nControllerIndex, eInputButton);
        break;
    }
}

inline void EquipManDetailsGlobals::Update()
{
    switch (eCurrentScreen)
    {
    case DetailsScreen::INVENTORY:
        tInventoryScreen.Update();
        break;

    case DetailsScreen::SKILLS:
        tSkillsScreen.Update();
        break;
    }
}

void EquipManDetailsGlobals::OpenForController(int32 nControllerIndex)
{
    if (nControllerIndex == -1 || IsActive())
    {
        return;
    }

    Player * ptPlayer = PlayerGetByControllerIndex(nControllerIndex);
    if (ptPlayer)
    {
        UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT).SetActive(FALSE);
        m_ptPlayer = ptPlayer;
        m_nControllerIndex = nControllerIndex;
        SwitchToScreen(DetailsScreen::INVENTORY);
        ::Console::Screens::ShowModalDialog(nControllerIndex, UIDLG_EQUIPMENT_MANAGER_CONSOLE, FALSE, GetUIIDNone());
    }
}

void EquipManDetailsGlobals::Close()
{
    if (IsActive())
    {
        if (tInventoryScreen.IsActive())
        {
            sgptEquipManDetailsGlobals->tInventoryScreen.Close();
        }

        if (tSkillsScreen.IsActive())
        {
            tSkillsScreen.Close();
        }

        UI_GetWindowFromID(UIWND_EQUIPMENT_MANAGER_CONSOLE_LOADOUT_SELECT).SetActive(TRUE);
    }
}

inline void EquipManDetailsGlobals::OnIconSelectionChanged(const UIID & idIcon, const BOOL bSelected)
{
    if (eCurrentScreen == DetailsScreen::SKILLS)
    {
        tSkillsScreen.OnIconSelectionChanged(idIcon, bSelected);
    }
}

void EquipManDetailsGlobals::SwitchToScreen(DetailsScreen eScreen)
{
    switch (eCurrentScreen)
    {
    case DetailsScreen::INVENTORY:
        if (tInventoryScreen.IsActive())
        {
            tInventoryScreen.Close();
        }
        break;

    case DetailsScreen::SKILLS:
        if (tSkillsScreen.IsActive())
        {
            tSkillsScreen.Close();
        }
        break;
    }

    eCurrentScreen = eScreen;

    switch (eScreen)
    {
    case DetailsScreen::INVENTORY:
        tInventoryScreen.OpenForController(m_nControllerIndex);
        break;

    case DetailsScreen::SKILLS:
        tSkillsScreen.Open();
        break;
    }
}

void UIEquipmentManagerDetails::InitializeForGame()
{
    if (!GameIsStartUp())
    {
        sgptEquipManDetailsGlobals = new EquipManDetailsGlobals();
    }
}

void UIEquipmentManagerDetails::DisposeForGame()
{
    if (!GameIsStartUp())
    {
        delete sgptEquipManDetailsGlobals;
        sgptEquipManDetailsGlobals = NULL;
    }
}

void UIEquipmentManagerDetails::OpenForController(int32 nControllerIndex)
{
    if (sgptEquipManDetailsGlobals)
    {
        sgptEquipManDetailsGlobals->OpenForController(nControllerIndex);
    }
}

void UIEquipmentManagerDetails::Close()
{
    if (sgptEquipManDetailsGlobals->IsActive())
    {
        sgptEquipManDetailsGlobals->Close();
    }
}

BOOL UIEquipmentManagerDetails::IsActive()
{
    return sgptEquipManDetailsGlobals && sgptEquipManDetailsGlobals->IsActive();
}

void UIEquipmentManagerDetails::OpenInventory(int32 nControllerIndex)
{
    if (sgptEquipManDetailsGlobals)
    {
        sgptEquipManDetailsGlobals->SwitchToScreen(DetailsScreen::INVENTORY);
    }
}

void UIEquipmentManagerDetails::OpenSkills(int32 nControllerIndex)
{
    if (sgptEquipManDetailsGlobals)
    {
        sgptEquipManDetailsGlobals->SwitchToScreen(DetailsScreen::SKILLS);
    }
}

void UIEquipmentManagerDetails::OnLeftStick(const UIID & id, const int32 nControllerIndex)
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->HandleLeftStickInput(nControllerIndex);
    }
}

void UIEquipmentManagerDetails::OnButton1(const UIID & id, const int32 nControllerIndex)
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->HandleButtonInput(nControllerIndex, 0);
    }
}

void UIEquipmentManagerDetails::OnButton2(const UIID & id, const int32 nControllerIndex)
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->HandleButtonInput(nControllerIndex, 1);
    }
}

void UIEquipmentManagerDetails::OnButtonL2(const UIID & id, const int32 nControllerIndex)
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->HandleButtonInput(nControllerIndex, 6);
    }
}

void UIEquipmentManagerDetails::OnButtonR2(const UIID & id, const int32 nControllerIndex)
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->HandleButtonInput(nControllerIndex, 7);
    }
}

void UIEquipmentManagerDetails::OnUpdate()
{
    if (IsActive())
    {
        sgptEquipManDetailsGlobals->Update();
    }
}

void IconSelectionChanged(const ::Console::RingWrapper &, const UIID & idIcon, const BOOL bSelected)
{
    sgptEquipManDetailsGlobals->OnIconSelectionChanged(idIcon, bSelected);
}

static BOOL sCanStillInspectTarget()
{
    Player * ptPlayer = sgptEquipManDetailsGlobals->GetPlayer();
    return ACDTryToGet(ptPlayer->idPlayerACD) != NULL;
}

static void AttachLightToModel(UIRActor & tModel)
{
    LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tModel.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tModel.SetLight(idLight);
}

// Headers whose statics the original's static initialiser runs; included last so they come in
// the original order.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Console/Online.h"
