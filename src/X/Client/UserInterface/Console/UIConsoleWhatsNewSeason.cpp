// Original file: X/Client/UserInterface/Console/UIConsoleWhatsNewSeason.cpp
//
// The console "what's new this season" screen: the splash, the season rewards and the conquests.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../Gfx/GfxVB.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../../Contrib/Contrib/blz/include/blz/list.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleUtils.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UIProgressBarControl.h"
#include "../Controls/UIStackPanelControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../Console/GamerProfile.h"
#include "../../Console/ControllerUtils.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../../Common/OnlineService/Achievements.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Protobuf/Hero.pb.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../Sound/Sound.h"
#include "../../../Common/Player.h"
#include "../../../Common/Definitions/HeroDefinition.h"
#include "../../Battlenet/UI/Util/UIAchievementsUtil.h"
#include "../../Battlenet/UI/Controls/ControlVisibilityManager.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_sort.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../Controls/UIControlControl.h"
#include "../UIInit.h"
#include "../../CGame.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Common/Items.h"
#include "../../../Common/Definitions/GameBalanceDefinition.h"
#include "../../../Common/UserInterface/UIInventory.h"
#include "../../../Common/UserInterface/UITextControl.h"
#include "../../../Common/CosmeticItems.h"
#include "../../../Common/Definitions/TreasureClassDefinition.h"
#include "../../RActors/RActors.h"

// Declared here because their headers are not adopted yet. Each comment names the defining file.
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringList.cpp
UIRActor & UI_GetRActorFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
UIStackPanel & UI_GetStackPanelFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
namespace UIUtils { namespace Console { BOOL CanScrollContentInScrollBox(const UIID & idScrollBox); } }  // UIConsoleUtils.cpp
namespace UIWhatsNew { namespace Console {  // X/Client/UserInterface/Console/UIConsoleWhatsNew.cpp
void OpenForController(int32 nControllerIndex, BOOL bAnniversary, BOOL bNecromancer);
} }

// X/Client/UserInterface/UIIDs.cpp
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON;
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH;
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET;
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS;
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS;
extern UIID UISTACKPANEL_CONSOLE_WHATS_NEW_SEASON_CONQUESTS_PANEL;
extern UIID UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT;
extern UIID UIPROGRESS_CONSOLE_WHATS_NEW_SEASON_CONQUEST_PROGRESS_BAR;
extern UIID UITEXT_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_TITLE;
extern UIID UITEXT_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC;
extern UIID UISCROLLBOX_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC;

namespace UIWhatsNewSeason {
namespace Console {

void UpdateActions();
void ShowSplash();
void ShowRewards();
void ShowConquests();
void sUpdateDisplayedConquest();
void sUpdateConquestDetails(const OnlineService::AchievementId & idAchievement);
void sGetArmorRewardsForClass(PlayerClass eClass, BOOL bIsFemale);
BOOL IsVisible();

enum DisplayMode {
    DISPLAY_SEASON_SPLASH = 0,
    DISPLAY_SEASON_REWARDS = 1,
    DISPLAY_SEASON_CONQUESTS = 2,
};

enum { ACTION_OK = 0, ACTION_VIEW_SPLASH = 1, ACTION_VIEW_CONQUESTS = 2, ACTION_VIEW_REWARDS = 3,
       ACTION_TOGGLE_HARDCORE = 4, ACTION_CHANGE_CLASS = 5, ACTION_SCROLL_DETAILS = 6, };

enum { MAX_CONQUESTS = 5, };
enum { PROGRESS_BAR = 0, CHECKLIST = 1, };

struct ItemInfo {
    GBID gbidItem;
    InventorySlotType eSlot;
};
typedef blz::list<ItemInfo> ItemInfoList;



struct WhatsNewSeasonData {
    DisplayMode m_eDisplayMode;
    BOOL m_bHardcore;
    PlayerClass m_eCurrentClass;
    BOOL m_bIsFemale;
    GBID m_gbidSet;
    ::Console::Achievements * m_ptAchievements;
    OnlineService::RewardIdList m_idItemsList;
    OnlineService::RewardIdList m_idCosmeticsList;
    OnlineService::AchievementIdList m_idConquestsList;
    VisualEquipment m_tVisualEquipment;
    int32 m_nSeasonNum;
    CRefString m_strArmorSetTitle;
    CRefString m_strSetItems;
    ItemInfoList m_listSetItems;
    int32 m_nControllerIndex;
    int32 m_nSelectedConquest;
    OnlineService::GameAccountHandle idPlayer;
    void Init();
};
void WhatsNewSeasonData::Init()
{
    m_nControllerIndex = -1;
    m_nSelectedConquest = 1;
    m_eDisplayMode = DISPLAY_SEASON_SPLASH;
    m_gbidSet = GBID_NONE;

    int32 nControllerIndex = UI_GetPrimaryUIController();
    if (nControllerIndex != -1 && ::Console::GamerProfile::IsHeroLoaded(nControllerIndex))
    {
        D3::Hero::SavedDefinition & tHeroDefinition = ::Console::GamerProfile::GetHero(nControllerIndex);
        m_eCurrentClass = PlayerClassFromGBID(tHeroDefinition.digest().gbid_class());
        m_bIsFemale = (tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_FEMALE)) ? TRUE : FALSE;
        m_bHardcore = (tHeroDefinition.digest().player_flags() & (1 << PLAYERFLAG_HARDCORE)) ? TRUE : FALSE;
    }
    else
    {
        m_eCurrentClass = PLAYER_CLASS_WITCHDOCTOR;
        m_bIsFemale = TRUE;
        m_bHardcore = FALSE;
    }

    m_nSeasonNum = OnlineService::GetSeasonNum();
    m_ptAchievements = sgtAppGlobals.ptAchievements;
    m_idItemsList.clear();
    m_idCosmeticsList.clear();
    m_ptAchievements->GetAllRewardsOfTypes(1 << OnlineService::REWARD_TYPE_ITEM, m_idItemsList, m_nSeasonNum);
    m_ptAchievements->GetAllRewardsOfTypes(1 << OnlineService::REWARD_TYPE_COSMETIC, m_idCosmeticsList, m_nSeasonNum);
    m_tVisualEquipment.Init();
}













static WhatsNewSeasonData sgtWhatsNewSeasonData;




void UpdateActions()
{
    ::Console::Screens::GetActionManager().ClearAllActions();
    switch (sgtWhatsNewSeasonData.m_eDisplayMode)
    {
    case DISPLAY_SEASON_SPLASH:
    {
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_VIEW_SPLASH, StringListFetch("Console:Close").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_VIEW_CONQUESTS, StringListFetch("Console:SeasonConquestDetails").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_VIEW_REWARDS, StringListFetch("Console:SeasonRewardDetails").str());
        break;
    }
    case DISPLAY_SEASON_REWARDS:
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_VIEW_SPLASH, StringListFetch("Console:Back").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_VIEW_CONQUESTS, StringListFetch("Console:SeasonConquestDetails").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_DPAD_HORIZONTAL, ACTION_CHANGE_CLASS, StringListFetch("Console:ChangeClass").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_DETAILS, StringListFetch("Console:SeasonScroll_Rewards"));
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_DETAILS, StringListFetch("Console:SeasonScroll_Rewards"));
        break;
    case DISPLAY_SEASON_CONQUESTS:
    {
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, ACTION_VIEW_SPLASH, StringListFetch("Console:Back").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_3, ACTION_TOGGLE_HARDCORE, sgtWhatsNewSeasonData.m_bHardcore ? StringListFetch("Console:ViewRegularConquests").str() : StringListFetch("Console:ViewHardcoreConquests").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_BUTTON_4, ACTION_VIEW_REWARDS, StringListFetch("Console:SeasonRewardDetails").str());
        ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_DPAD_HORIZONTAL, ACTION_CHANGE_CLASS, StringListFetch("Console:Page").str());
        UIID idScrollbox = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS, ".AchievementScrollBox");
        if (UIUtils::Console::CanScrollContentInScrollBox(idScrollbox))
        {
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_LEFT_TRIGGER, ACTION_SCROLL_DETAILS, StringListFetch("Console:SeasonScroll_Rewards").str());
            ::Console::Screens::GetActionManager().SetAction(::Console::CONTROLLER_INPUT_RIGHT_TRIGGER, ACTION_SCROLL_DETAILS, StringListFetch("Console:SeasonScroll_Rewards").str());
        }
        break;
    }
    }
    ::Console::Screens::UpdateButtonLegendText();
}

}  }
extern UIID UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_ARMOR_SET, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST1, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST2, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PENNANT, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PORTRAIT, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG1, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG2, UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_WINGS, UIRACTOR_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET, UIRACTOR_CONSOLE_WHATS_NEW_SEASON_SPLASH_ARMOR_SET, UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_NAME, UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_DETAILS, UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_EARNED, UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_VALUE, UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUEST_PROGRESS, UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUEST_CHECKLIST, UIWRAP_CONSOLE_WHATS_NEW_SEASON_CONQUEST_CHECKLIST_PANEL, UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PORTRAIT, UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_WINGS, UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PENNANT, UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PET;  // X/Client/UserInterface/UIIDs.cpp
// Declared here because their headers are not adopted yet (X/Common/Items.cpp, VisualInventory.cpp, RActors.cpp, Light.cpp, UI.cpp, UIConsoleUtils.cpp, UIInventory.cpp, Achievement).
namespace UIInventory { CRefString GetSetItemTooltipBonuses(GBID gbidSet); } LightID LightAdd(uint32 dwFlags, SNO snoLight, const PRTransform & tTransform, BOOL bUnused, RenderContext eContext); SNO UI_GetDefaultHeroLight(); SNO GetFrontendCharacterSNO(int32 ePlayerClass, int32 bIsFemale); RActorID UI_CreateHeroUIRActor(SNO snoActor, SNO snoIrradianceTex, const VisualEquipment & tVisualEquipment, int32 nUnused, int32 ePlayerClass, int32 bIsFemale, const PRTransform & transform, RenderContext eContext, uint32 dwFlags, float flScale, int32 nUnused2); void UI_DeleteControl(const UIID & idControl, const BOOL fAssertIfDeletingRoot); namespace Achievement { CRefString DisplayStringFromEvent(int32 eEventId, uint64 nComparand, CRefString & sStringLabelKey); } SNO VisualInventoryGetPennantSnoFromGbid(GBID gbidItem); SNO VisualInventoryGetPetSnoFromGbid(GBID gbidItem); void RActorSetIrradianceMap(RActor * ptRActor, SNO snoIrradianceTex); namespace UIUtils { namespace Console { void DisplayPortrait(PlayerClass eClass, BOOL bIsFemale, BOOL bHardcore, const UIID & idPortrait, GBID gbidFrame); } } BOOL ItemMeetsOwnerClassRestrictions(const GBID gbidItem, const PlayerClass ePlayerClass, BOOL bCheckLegendaryClassDropWeight); BOOL ItemIsAllowedInSlot(const InventorySlotType eSlot, const ItemTypeDefinition * pItemTypeDef); BOOL UI_TestID(const UIID & id); void ACDInventoryGetItemDrawInfoWithGBID(const GBID gbidItem, ImageTextureFrame & tImageTexFrame, const int32 eDrawType, const PlayerClass eClass, const BOOL fIsMale);
namespace UIWhatsNewSeason { namespace Console {

void sGetArmorSetForRActor(const OnlineService::RewardIdList & idItemsList, VisualEquipment & tVisualEquipment, ItemInfoList & gbidList, PlayerClass eClass)
{




    gbidList.clear();

    for (int32 n = 0; n < idItemsList.size(); n++)
    {
        ScopedItemDefinitionLockR pdefSetItem(idItemsList[n]);
        if (!pdefSetItem)
            continue;

        SNO snoTreasureClass = pdefSetItem->snoSalvageTreasureClassNormal;
        if (snoTreasureClass == SNO_NONE || snoTreasureClass == INVALID_SNO)
            continue;






        ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);
        for (int32 i = 0; i < pdefTreasureClass->nModifierCount; i++)
        {
            GBID gbidItem = ((LootDropModifier *)pdefTreasureClass->ptModifierList)[i].tSpecificItem.gbidItem;
            if (!ItemMeetsOwnerClassRestrictions(gbidItem, eClass, TRUE))
                continue;





            ScopedItemDefinitionLockR pdefItem(gbidItem);
            if (!pdefItem)
                continue;


            ScopedItemTypeDefinitonLockR pItemTypeDef(pdefItem->gbidItemType);
            if (!pItemTypeDef)
                continue;

            if (pdefItem->gbidSet != GBID_NONE)
                sgtWhatsNewSeasonData.m_gbidSet = pdefItem->gbidSet;

            InventorySlotType eInventorySlot;
            for (eInventorySlot = 0; eInventorySlot < 14; eInventorySlot++)
            {
                if (ItemIsAllowedInSlot(eInventorySlot, pItemTypeDef))
                    break;
            }
            if (eInventorySlot >= 14)
                continue;

            ItemInfo tInfo;
            tInfo.gbidItem = gbidItem;
            tInfo.eSlot = eInventorySlot;
            gbidList.push_back(tInfo);

            VisualInventorySlot eVisualSlot = VisualInventorySlotFromInventorySlot(eInventorySlot);
            if (eVisualSlot != -1)
                tVisualEquipment.tVisualItem[eVisualSlot].gbidItem = gbidItem;
        }
    }
}







void sPopulateHeroRactor(const UIID & idRactor,
                         PlayerClass eClass,
                         BOOL bIsFemale,
                         const VisualEquipment & tVisualEquipment)

{
    PRTransform transform(cgqIdentity, cgwpNull);
    SNO snoActor = GetFrontendCharacterSNO(eClass, bIsFemale);
    UIRActor & tRactor = UI_GetRActorFromID(idRactor);
    tRactor.Cleanup();
    tRactor.DeleteRActor();
    if (snoActor != SNO_NONE)
    {
        RActorID ractorID = UI_CreateHeroUIRActor(
            snoActor,
            GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE),
            tVisualEquipment,
            0,
            eClass,
            bIsFemale,
            transform,
            tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(),
            0x68700,
            1.0f,
            TRUE);
        tRactor.SetRActor(ractorID);

        LightID idLight = LightAdd(
            0,
            UI_GetDefaultHeroLight(),
            PRTransform(cgqIdentity, cgwpNull),
            FALSE,
            tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
        tRactor.SetLight(idLight);
    }
}


void ShowSplash()
{
    sgtWhatsNewSeasonData.m_eDisplayMode = DISPLAY_SEASON_SPLASH;
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS).SetActive(FALSE);

    OnlineService::RewardIdList idTransmogList;
    sgtWhatsNewSeasonData.m_ptAchievements->GetAllRewardsByType(OnlineService::REWARD_TYPE_ITEM_TRANSMOG, idTransmogList, sgtWhatsNewSeasonData.m_nSeasonNum);
    if (idTransmogList.size() < 1)
        return;

    OnlineService::RewardDefinition tDefinition1;
    if (sgtWhatsNewSeasonData.m_ptAchievements->GetRewardDefinition(idTransmogList[0], tDefinition1))
    {
        GBID gbidTransmog = tDefinition1.m_id;
        ImageTextureFrame tIcon;
        ACDInventoryGetItemDrawInfoWithGBID(gbidTransmog, tIcon, 0, sgtWhatsNewSeasonData.m_eCurrentClass, !sgtWhatsNewSeasonData.m_bIsFemale);
        if (UI_TestID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG1))
        {
            UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG1);
            tWindow.SetTextureFrame(tIcon);
        }
    }

    if (idTransmogList.size() < 2)
        return;

    OnlineService::RewardDefinition tDefinition2;
    if (sgtWhatsNewSeasonData.m_ptAchievements->GetRewardDefinition(idTransmogList[1], tDefinition2))
    {
        GBID gbidTransmog = tDefinition2.m_id;
        ImageTextureFrame tIcon;
        ACDInventoryGetItemDrawInfoWithGBID(gbidTransmog, tIcon, 0, sgtWhatsNewSeasonData.m_eCurrentClass, !sgtWhatsNewSeasonData.m_bIsFemale);
        if (UI_TestID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG2))
        {
            UIWindow & tWindow = UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_TRANSMOG2);
            tWindow.SetTextureFrame(tIcon);
        }
    }

    if (UI_TestID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_ARMOR_SET))
        UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_ARMOR_SET).SetActive(sgtWhatsNewSeasonData.m_idItemsList.size() != 0);

    sgtWhatsNewSeasonData.m_tVisualEquipment.Init();
    if (sgtWhatsNewSeasonData.m_idItemsList.size())
    {
        sGetArmorSetForRActor(sgtWhatsNewSeasonData.m_idItemsList, sgtWhatsNewSeasonData.m_tVisualEquipment, sgtWhatsNewSeasonData.m_listSetItems, sgtWhatsNewSeasonData.m_eCurrentClass);
        sPopulateHeroRactor(UIRACTOR_CONSOLE_WHATS_NEW_SEASON_SPLASH_ARMOR_SET, sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale, sgtWhatsNewSeasonData.m_tVisualEquipment);
    }

    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PORTRAIT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_WINGS).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PENNANT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET).SetActive(FALSE);

    for (uint32 i = 0; i < sgtWhatsNewSeasonData.m_idCosmeticsList.size(); i++)
    {
        GBID gbid = sgtWhatsNewSeasonData.m_idCosmeticsList[i];
        CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbid);
        switch (eCosmeticType)
        {
        case COSMETIC_ITEM_FRAME:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PORTRAIT).SetActive(TRUE);
            const UIID & idPortraitFrame = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PORTRAIT, ".RewardPortraitFrame.PortraitFrame");
            UIUtils::Console::DisplayPortrait(sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale, FALSE, idPortraitFrame, gbid);
            break;
        }
        case COSMETIC_ITEM_WINGS:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_WINGS).SetActive(TRUE);
            const UIID & idWingsRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_WINGS, ".RewardWings.Model3DSet");
            sgtWhatsNewSeasonData.m_tVisualEquipment.tCosmeticItems[COSMETIC_ITEM_WINGS].gbidItem = gbid;
            sPopulateHeroRactor(idWingsRActor, sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale, sgtWhatsNewSeasonData.m_tVisualEquipment);
            break;
        }
        case COSMETIC_ITEM_PENNANT:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PENNANT).SetActive(TRUE);
            const UIID & idPennantRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PENNANT, ".RewardPennant.Model3DPennant");
            UIRActor & tRactor = UI_GetRActorFromID(idPennantRActor);
            tRactor.Cleanup();
            tRactor.DeleteRActor();
            SNO snoPennant = VisualInventoryGetPennantSnoFromGbid(gbid);
            if (snoPennant != SNO_NONE)
            {
                PRTransform transform(cgqIdentity, cgwpNull);
                RActorID ractorID = RActorCreate(snoPennant, 1.0f, -1.0f, transform, 0, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                tRactor.SetRActor(ractorID);
                RActor * ptPetRActor = RActorTryToGet(ractorID);
                if (ptPetRActor)
                    RActorSetIrradianceMap(ptPetRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                tRactor.SetLight(idLight);
            }
            break;
        }
        case COSMETIC_ITEM_PET:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET).SetActive(TRUE);
            const UIID & idPetRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET, ".RewardPet.Model3DPet");
            UIRActor & tRactor = UI_GetRActorFromID(idPetRActor);
            tRactor.Cleanup();
            tRactor.DeleteRActor();
            SNO snoPet = VisualInventoryGetPetSnoFromGbid(gbid);
            if (snoPet != SNO_NONE)
            {
                PRTransform transform(cgqIdentity, cgwpNull);
                RActorID ractorID = RActorCreate(snoPet, 1.0f, -1.0f, transform, 0, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                tRactor.SetRActor(ractorID);
                RActor * ptPetRActor = RActorTryToGet(ractorID);
                if (ptPetRActor)
                    RActorSetIrradianceMap(ptPetRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                tRactor.SetLight(idLight);
            }
            break;
        }
        }
    }

    OnlineService::CategoryIdList tTopCategories;
    if (sgtWhatsNewSeasonData.m_ptAchievements->GetChildCategories(0, tTopCategories))
    {
        const OnlineService::CategoryDefinition * ptConquestCategoryDefinition = NULL;
        for (uint32 i = 0; i < tTopCategories.size(); i++)
        {
            const OnlineService::CategoryDefinition * ptCategoryDefinition = sgtWhatsNewSeasonData.m_ptAchievements->GetCategoryDefinition(tTopCategories[i]);
            if (strcmp(ptCategoryDefinition->m_pszNameKey, "Achievements:Conquests") == 0)
            {
                ptConquestCategoryDefinition = ptCategoryDefinition;
                break;
            }
        }
        if (ptConquestCategoryDefinition)
        {
            OnlineService::AchievementIdList arConquestAchievementIds;
            sgtWhatsNewSeasonData.m_ptAchievements->GetCategoryAchievements(ptConquestCategoryDefinition->m_id, arConquestAchievementIds);
            OnlineService::SortAchievementsByOrder(arConquestAchievementIds);
            int32 nConquestsShown = 0;
            for (uint32 i = 0; i < arConquestAchievementIds.size() && nConquestsShown < 2; i++)
            {
                const OnlineService::AchievementId & idAchievement = arConquestAchievementIds[i];
                const OnlineService::AchievementDefinition * ptDefinition = sgtWhatsNewSeasonData.m_ptAchievements->GetAchievementDefinition(idAchievement);
                if (ptDefinition && sgtWhatsNewSeasonData.m_ptAchievements->IsAchievementValidForSeason(ptDefinition, sgtWhatsNewSeasonData.m_nSeasonNum))
                {
                    if (nConquestsShown == 0)
                    {
                        UIWindow & wndConquest = UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST1);
                        wndConquest.SetActive(TRUE);
                        ImageTextureFrame tIconTexture;
                        tIconTexture = ImageTextureFrame(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
                        wndConquest.SetTextureFrame(tIconTexture);
                        nConquestsShown++;
                    }
                    else if (nConquestsShown == 1)
                    {
                        UIWindow & wndConquest = UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST2);
                        wndConquest.SetActive(TRUE);
                        ImageTextureFrame tIconTexture;
                        tIconTexture = ImageTextureFrame(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
                        wndConquest.SetTextureFrame(tIconTexture);
                        nConquestsShown++;
                    }
                }
            }
            if (nConquestsShown < 2)
                UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST2).SetActive(FALSE);
            if (nConquestsShown < 1)
                UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_CONQUEST1).SetActive(FALSE);
        }
    }
}


































































































void sGetArmorRewardsForClass(PlayerClass eClass, BOOL bIsFemale)
{

    sgtWhatsNewSeasonData.m_tVisualEquipment.Init();
    if (sgtWhatsNewSeasonData.m_idItemsList.size())
    {
        sGetArmorSetForRActor(sgtWhatsNewSeasonData.m_idItemsList, sgtWhatsNewSeasonData.m_tVisualEquipment, sgtWhatsNewSeasonData.m_listSetItems, eClass);
        sPopulateHeroRactor(UIRACTOR_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET, eClass, bIsFemale, sgtWhatsNewSeasonData.m_tVisualEquipment);
    }

    sgtWhatsNewSeasonData.m_strArmorSetTitle = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SETS), GBIDToString(sgtWhatsNewSeasonData.m_gbidSet, GB_SET_ITEM_BONUSES, FALSE).str());
    sgtWhatsNewSeasonData.m_strSetItems = CRefString();

    for (blz::list_iterator<ItemInfo> iter = sgtWhatsNewSeasonData.m_listSetItems.begin(); iter != sgtWhatsNewSeasonData.m_listSetItems.end(); ++iter)
    {
        if (iter->gbidItem != GBID_NONE && iter->gbidItem != INVALID_GBID)
        {
            CRefString usSetItem;
            usSetItem += "    ";
            usSetItem += ItemGetDisplayName(iter->gbidItem);
            InventorySlotType eSlotType = iter->eSlot;
            BOOL fIs2Handed = ItemHasLabel(iter->gbidItem, ITL_2_HAND);
            CRefString sSlotLabel(UIInventory::ItemGetLabelForSlot(eSlotType, fIs2Handed));
            if (sSlotLabel.Length())
            {
                CRefString szLocalizedSlot = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ITEM_SLOTS), sSlotLabel.str());
                usSetItem += CRefString().Format(" (%s)", szLocalizedSlot.str());
            }
            usSetItem += "\n";
            Set_Text_Color(usSetItem, 0);
            sgtWhatsNewSeasonData.m_strSetItems += usSetItem;
        }
    }

    sgtWhatsNewSeasonData.m_strSetItems += UIInventory::GetSetItemTooltipBonuses(sgtWhatsNewSeasonData.m_gbidSet);
}













static void sDisplayArmorRewardsForClass(PlayerClass eClass, BOOL bIsFemale)
{


    sGetArmorRewardsForClass(eClass, bIsFemale);

    UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_TITLE).SetText(sgtWhatsNewSeasonData.m_strArmorSetTitle, FALSE);
    UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC).SetText(sgtWhatsNewSeasonData.m_strSetItems, FALSE);
    UIUtils::Console::ResetScrollBox(UISCROLLBOX_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC);
}

void ShowRewards()
{
    sgtWhatsNewSeasonData.m_eDisplayMode = DISPLAY_SEASON_REWARDS;
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH).SetActive(FALSE);

    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS).SetActive(TRUE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS).SetActive(FALSE);

    int32 nCurrentSeason = OnlineService::GetSeasonNum();

    sDisplayArmorRewardsForClass(sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale);




    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PORTRAIT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_WINGS).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PENNANT).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PET).SetActive(FALSE);

    for (uint32 i = 0; i < sgtWhatsNewSeasonData.m_idCosmeticsList.size(); i++)
    {


        GBID gbid = sgtWhatsNewSeasonData.m_idCosmeticsList[i];
        CosmeticItemType eCosmeticType = CosmeticItems_GetCosmeticItemTypeFromGBID(gbid);
        switch (eCosmeticType)
        {
        case COSMETIC_ITEM_FRAME:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PORTRAIT).SetActive(TRUE);
            const UIID & idPortraitFrame = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PORTRAIT, ".PortraitFrame");
            UIUtils::Console::DisplayPortrait(sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale, FALSE, idPortraitFrame, gbid);
            break;
        }
        case COSMETIC_ITEM_WINGS:
        {

            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_WINGS).SetActive(TRUE);
            const UIID & idWingsRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_WINGS, ".3DModel");

            sgtWhatsNewSeasonData.m_tVisualEquipment.tCosmeticItems[COSMETIC_ITEM_WINGS].gbidItem = gbid;
            sPopulateHeroRactor(idWingsRActor, sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale, sgtWhatsNewSeasonData.m_tVisualEquipment);
            break;
        }
        case COSMETIC_ITEM_PENNANT:
        {

            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PENNANT).SetActive(TRUE);
            const UIID & idPennantRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PENNANT, ".3DModel");

            UIRActor & tRactor = UI_GetRActorFromID(idPennantRActor);
            tRactor.Cleanup();
            tRactor.DeleteRActor();
            SNO snoPennant = VisualInventoryGetPennantSnoFromGbid(gbid);
            if (snoPennant != SNO_NONE)
            {
                PRTransform transform(cgqIdentity, cgwpNull);
                RActorID ractorID = RActorCreate(snoPennant, 1.0f, -1.0f, transform, 0, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                tRactor.SetRActor(ractorID);
                RActor * ptPennantRActor = RActorTryToGet(ractorID);
                if (ptPennantRActor)
                    RActorSetIrradianceMap(ptPennantRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                tRactor.SetLight(idLight);
            }
            break;
        }
        case COSMETIC_ITEM_PET:
        {
            UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PET).SetActive(TRUE);
            const UIID & idPetRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS_PET, ".3DModel");

            UIRActor & tRactor = UI_GetRActorFromID(idPetRActor);
            tRactor.Cleanup();
            tRactor.DeleteRActor();
            SNO snoPet = VisualInventoryGetPetSnoFromGbid(gbid);
            if (snoPet != SNO_NONE)
            {
                PRTransform transform(cgqIdentity, cgwpNull);
                RActorID ractorID = RActorCreate(snoPet, 1.0f, -1.0f, transform, 0, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext(), -1);
                tRactor.SetRActor(ractorID);
                RActor * ptPetRActor = RActorTryToGet(ractorID);
                if (ptPetRActor)
                    RActorSetIrradianceMap(ptPetRActor, GlobalSNOGet(GLOBALSNO_RACTOR_NEUTRAL_IRRADIANCE_TEXTURE));
                LightID idLight = LightAdd(0, UI_GetDefaultHeroLight(), PRTransform(cgqIdentity, cgwpNull), FALSE, tRactor.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
                tRactor.SetLight(idLight);
            }
            break;
        }
        }
    }


























    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
}

uint32 GetCriteriaFlags(
    const OnlineService::AchievementDefinition & tDefinition)
{

    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;

    uint32 dwFlags =

        (tDefinition.m_flags & (1 << OnlineService::ACHIEVEMENT_FLAG_HAS_PROGRESS_BAR)) ? (1 << PROGRESS_BAR) : 0;




    OnlineService::CriteriaIdList arCriteriaIds;
    int32 nCount = 0;
    if (tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteriaIds))
    {
        for (uint32 i = 0; i < arCriteriaIds.size(); i++)
        {
            const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteriaIds[i]);
            if (!ptCritDef)
            {
                continue;
            }


            if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN))
            {
                continue;
            }

            if (++nCount > 1)
            {
                dwFlags |= (1 << CHECKLIST);
                break;
            }
        }
    }

    return dwFlags;
}



void PopulateProgressBar(OnlineService::CriteriaQuantity nEarned, OnlineService::CriteriaQuantity nTotal)
{
    CRefString sProgress = LocalizedSprintf(StringListFetch("Bnet_Achievements:AchievementProgress").str(),
                                            UnicodeText::FormatNumber((int32)nEarned, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                            UnicodeText::FormatNumber((int32)nTotal, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), NULL);
    UI_GetTextFromID(UITEXT_CONSOLE_ACHIEVEMENTS_DETAILVIEW_PROGRESS_TEXT).SetText(sProgress, FALSE);
    UI_GetProgressBarFromID(UIPROGRESS_CONSOLE_WHATS_NEW_SEASON_CONQUEST_PROGRESS_BAR).SetExtentsAndPosition(0.0f, (float)nTotal, (float)nEarned, -1.0f);
}






void PopulateCriteriaElement(
    const UIID & idElement,
    OnlineService::CriteriaId idCriteria)
{

    OnlineService::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(idCriteria);
    const OnlineService::AchievementDefinition * ptAchievementDef;
    if (ptCritDef != NULL &&
        (ptAchievementDef = tAchievements.GetAchievementDefinition(ptCritDef->m_parentAchievementId)) != NULL)
    {
        OnlineService::CriteriaRecord tCritRecord;
        BOOL fMetaAchievement = (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_IS_ACHIEVEMENT_ID)) ? TRUE : FALSE;

        BOOL fEarned = FALSE;
        CRefString sMessage;
        if (ptAchievementsInterface)
        {


            if (!ptAchievementsInterface->FindCriteriaRecord(sgtWhatsNewSeasonData.idPlayer, idCriteria, tCritRecord))
            {
                if (ptAchievementDef->m_nMinRequiredCriteria == 0)
                {
                    OnlineService::AchievementRecord tRecord;
                    fEarned = ptAchievementsInterface->FindAwardedAchievement(sgtWhatsNewSeasonData.idPlayer, ptCritDef->m_parentAchievementId, tRecord);
                }
            }
            else
            {




                fEarned = tCritRecord.m_quantity >= ptCritDef->m_necessaryQuantity;
            }
        }
        if (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_RETRIEVE_NAME_FROM_STRINGLIST))
        {
            int32 eEventId = ptCritDef->m_advanceEvent.m_id;  // DWARF type Achievement::EventID (typedef int32)
            CRefString sStringLabelKey = ptCritDef->m_pszStringlabelKey;
            sMessage = Achievement::DisplayStringFromEvent(eEventId, ptCritDef->m_advanceEvent.m_comparand, sStringLabelKey);
        }
        else
        {
            sMessage = StringListFetch(ptCritDef->m_pszNameKey);
        }

        UIID idTextEarned, idTextUnearned;
        idTextEarned = UIIDCreateFromParent(idElement, ".CriteriaEarned");
        idTextUnearned = UIIDCreateFromParent(idElement, ".CriteriaUnearned");
        ControlVisibilityManager tText;
        tText.AddControl(idTextEarned);
        tText.AddControl(idTextUnearned);
        tText.SetActive(fEarned ? 0 : 1, false);
        UIID idTargetTextfield = fEarned ? idTextEarned : idTextUnearned;
        UI_GetTextFromID(idTargetTextfield).SetText(sMessage, FALSE);

        UIID idEarned, idUnearned, idChecked, idUnchecked;
        idEarned = UIIDCreateFromParent(idElement, ".Earned");
        idUnearned = UIIDCreateFromParent(idElement, ".Unearned");
        idChecked = UIIDCreateFromParent(idElement, ".Checked");
        idUnchecked = UIIDCreateFromParent(idElement, ".UnChecked");

        int32 nTargetState = (fEarned ? 0 : 1) | (fMetaAchievement ? 0 : 2);
        ControlVisibilityManager tStates;
        tStates.AddControl(idEarned);
        tStates.AddControl(idUnearned);
        tStates.AddControl(idChecked);
        tStates.AddControl(idUnchecked);
        tStates.SetActive(nTargetState, false);
    }
}



void PopulateCriteriaDisplay(const OnlineService::AchievementDefinition & tDefinition)
{
    ::Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::CriteriaIdList arCriteria;
    tAchievements.GetAchievementCriteria(tDefinition.m_id, arCriteria);
    for (uint32 i = 0; i < arCriteria.size(); i++)
    {
        const OnlineService::CriteriaDefinition * ptCritDef = tAchievements.GetCriteriaDefinition(arCriteria[i]);
        if (ptCritDef == NULL || (ptCritDef->m_flags & (1 << OnlineService::CRITERIA_FLAG_HIDDEN)))
        {
            blzext::vector_erase_reorder(arCriteria, i--);
        }
    }
    blz::sort(arCriteria.begin(), arCriteria.end(), OnlineService::CriteriaSortPredicate);

    UIWindow & tWrapPanel = UI_GetWindowFromID(UIWRAP_CONSOLE_WHATS_NEW_SEASON_CONQUEST_CHECKLIST_PANEL);
    const UIControlItemArray & arChildren = tWrapPanel.GetChildList();
    uint32 nCriteriaControlIndex = 0;
    for (; nCriteriaControlIndex < arChildren.size() && nCriteriaControlIndex < arCriteria.size(); nCriteriaControlIndex++)
    {
        PopulateCriteriaElement(arChildren[nCriteriaControlIndex].id, arCriteria[nCriteriaControlIndex]);
    }

    UIControlSourceKey keySeriesItem = UIControlSourceKey::Parse("ConsoleAchievements:ConsoleAchievements_main.LayoutRoot.AchievementDetails.AchievementScrollBox.AchievementStack.ChecklistSpacer.ChecklistPanel.ChecklistItem", -1, NULL, NULL);
    for (; nCriteriaControlIndex < arCriteria.size(); nCriteriaControlIndex++)
    {
        UIControl * ptControl = UI_CreateControlFromTemplate(keySeriesItem, DynamicControlCreationParams(CRefString().Format("criteria%d", nCriteriaControlIndex), UIWRAP_CONSOLE_WHATS_NEW_SEASON_CONQUEST_CHECKLIST_PANEL), NULL);
        PopulateCriteriaElement(ptControl->GetID(), arCriteria[nCriteriaControlIndex]);
    }

    while (nCriteriaControlIndex < arChildren.size())
    {
        UI_DeleteControl(arChildren[nCriteriaControlIndex].id, TRUE);
    }
}










void sUpdateConquestDetails(
    const OnlineService::AchievementId & idAchievement)
{

    const OnlineService::AchievementDefinition * ptDefinition = sgtWhatsNewSeasonData.m_ptAchievements->GetAchievementDefinition(idAchievement);

    if (!ptDefinition)
        return;




    CRefString sName = StringListFetch(ptDefinition->m_pszNameKey);
    UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_NAME).SetText(sName.str(), FALSE);


    CRefString sDescription = StringListFetch(ptDefinition->m_pszDescriptionKey);
    UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_DETAILS).SetText(sDescription.str(), FALSE);


    OnlineService::AchievementRecord tRecord;
    CRefString sEarnedText;
    OnlineService::AchievementsInterface * ptAchievementsInterface = sgtAppGlobals.ptAchievementsInterface;
    if (ptAchievementsInterface && ptAchievementsInterface->FindAwardedAchievement(sgtWhatsNewSeasonData.idPlayer, idAchievement, tRecord))
    {
        sEarnedText = LocalizedSprintf(
            StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_TOOLTIPS), "AchievementTooltipEarned").str(),
            UnicodeText::FormatDate(tRecord.m_timestampEarned, FDS_SHORT, XLOCALE_INVALID).str(), NULL);
    }

    else
    {
        sEarnedText = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_TOOLTIPS), "AchievementTooltipUnearned");
    }

    UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_EARNED).SetText(sEarnedText.str(), FALSE);



    UIText & tPointsText = UI_GetTextFromID(UITEXT_CONSOLE_WHATS_NEW_SEASON_CONQUEST_VALUE);
    if (ptDefinition->m_pointsValue)
    {
        tPointsText.SetText(UnicodeText::FormatNumber((int32)ptDefinition->m_pointsValue, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(), FALSE);
        tPointsText.SetActive(TRUE);
    }
    else
    {
        tPointsText.SetActive(FALSE);
    }

    OnlineService::CriteriaQuantity nEarned = 0, nTotal = 0;
    uint32 dwFlags = GetCriteriaFlags(*ptDefinition);

    OnlineService::AchievementStatus eStatus = OnlineService::GetAchievementStatus(::Console::ClientAchievementsInterface::UserIndexToGameAccount(sgtWhatsNewSeasonData.m_nControllerIndex), idAchievement, &nEarned, &nTotal);


    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUEST_PROGRESS).SetActive(dwFlags & (1 << PROGRESS_BAR));
    if (dwFlags & (1 << PROGRESS_BAR))
    {
        PopulateProgressBar(nEarned, nTotal);
    }
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUEST_CHECKLIST).SetActive((dwFlags >> CHECKLIST) & 1);
    if ((dwFlags >> CHECKLIST) & 1)
    {
        PopulateCriteriaDisplay(*ptDefinition);
    }

}

void sUpdateDisplayedConquest()
{
    int32 nCurrentSlot = 1;


    for (uint32 i = 0; i < sgtWhatsNewSeasonData.m_idConquestsList.size() && nCurrentSlot <= MAX_CONQUESTS; i++)
    {
        const OnlineService::AchievementId & idAchievement = sgtWhatsNewSeasonData.m_idConquestsList[i];
        const OnlineService::AchievementDefinition * ptDefinition = sgtWhatsNewSeasonData.m_ptAchievements->GetAchievementDefinition(idAchievement);
        if (!ptDefinition)
            continue;


        if (!sgtWhatsNewSeasonData.m_ptAchievements->IsAchievementValidForSeason(ptDefinition, sgtWhatsNewSeasonData.m_nSeasonNum))
            continue;



        BOOL bHardcore = sgtWhatsNewSeasonData.m_ptAchievements->IsAchievementHardcoreByCriteria(idAchievement);
        if (sgtWhatsNewSeasonData.m_bHardcore != bHardcore)
            continue;



        UIID idCurrentIcon = UIIDCreateFromParent(UISTACKPANEL_CONSOLE_WHATS_NEW_SEASON_CONQUESTS_PANEL, CRefString().Format(".Conquest%d.AchievementIcon", nCurrentSlot));
        UIID idCurrentSelection = UIIDCreateFromParent(UISTACKPANEL_CONSOLE_WHATS_NEW_SEASON_CONQUESTS_PANEL, CRefString().Format(".Conquest%d.SelectedState", nCurrentSlot));

        UIWindow & tWnd = UI_GetWindowFromID(idCurrentIcon);
        ImageTextureFrame tIconTexture(CRefString().Format("UI:%s", ptDefinition->m_pszIconTexture));
        tWnd.SetTextureFrame(tIconTexture);

        UI_GetWindowFromID(idCurrentSelection).SetActive(sgtWhatsNewSeasonData.m_nSelectedConquest == nCurrentSlot);

        if (sgtWhatsNewSeasonData.m_nSelectedConquest == nCurrentSlot)
        {
            sUpdateConquestDetails(idAchievement);
        }

        nCurrentSlot++;
    }
}





void ShowConquests()
{
    sgtWhatsNewSeasonData.m_eDisplayMode = DISPLAY_SEASON_CONQUESTS;
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_REWARDS).SetActive(FALSE);
    UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS).SetActive(TRUE);
    sgtWhatsNewSeasonData.m_idConquestsList.clear();
    UI_GetStackPanelFromID(UISTACKPANEL_CONSOLE_WHATS_NEW_SEASON_CONQUESTS_PANEL);

    OnlineService::CategoryIdList tTopCategories;
    if (sgtWhatsNewSeasonData.m_ptAchievements->GetChildCategories(0, tTopCategories))
    {
        const OnlineService::CategoryDefinition * ptCategoryDefinition = NULL;
        for (uint32 i = 0; i < tTopCategories.size(); i++)
        {
            const OnlineService::CategoryDefinition * ptDefinition = sgtWhatsNewSeasonData.m_ptAchievements->GetCategoryDefinition(tTopCategories[i]);
            if (strcmp(ptDefinition->m_pszNameKey, "Achievements:Conquests") == 0)
            {
                ptCategoryDefinition = ptDefinition;
                break;
            }
        }
        if (ptCategoryDefinition)
        {
            sgtWhatsNewSeasonData.m_ptAchievements->GetCategoryAchievements(ptCategoryDefinition->m_id, sgtWhatsNewSeasonData.m_idConquestsList);
            OnlineService::SortAchievementsByOrder(sgtWhatsNewSeasonData.m_idConquestsList);
            sUpdateDisplayedConquest();
            SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_SELECT_GENERIC), cgtPSODefaults);
        }
    }
}















void OpenForController(int32 nControllerIndex)
{
    sgtWhatsNewSeasonData.Init();
    sgtWhatsNewSeasonData.m_nControllerIndex = nControllerIndex;
    sgtWhatsNewSeasonData.idPlayer = ::Console::ClientAchievementsInterface::UserIndexToGameAccount(nControllerIndex);
    ::Console::Screens::ShowModalDialog(nControllerIndex, UIWND_CONSOLE_WHATS_NEW_SEASON, TRUE, GetUIIDNone());
    ShowSplash();
    UpdateActions();
}









BOOL IsVisible()
{
    return UI_GetWindowFromID(UIWND_CONSOLE_WHATS_NEW_SEASON).IsActive();
}


static void Close()
{
    if (IsVisible())
    {
        const UIID & idPetRActor = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_SPLASH_PET, ".RewardPet.Model3DPet");
        UIRActor & tPet = UI_GetRActorFromID(idPetRActor);
        tPet.Cleanup();
        tPet.DeleteRActor();
        ::Console::Screens::CloseModalDialog(UIWND_CONSOLE_WHATS_NEW_SEASON);
    }
}











void OnButton2(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex)
    {
        switch (sgtWhatsNewSeasonData.m_eDisplayMode)
        {
        case DISPLAY_SEASON_REWARDS:
        case DISPLAY_SEASON_CONQUESTS:
            ShowSplash();
            break;
        case DISPLAY_SEASON_SPLASH:
            Close();
            break;
        }
        SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
        UpdateActions();
    }
}






void OnButton3(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex)
    {
        switch (sgtWhatsNewSeasonData.m_eDisplayMode)
        {
        case DISPLAY_SEASON_SPLASH:
            ShowConquests();
            break;
        case DISPLAY_SEASON_CONQUESTS:
            sgtWhatsNewSeasonData.m_bHardcore = !sgtWhatsNewSeasonData.m_bHardcore;
            sUpdateDisplayedConquest();
            SoundManager::PlaySound(GlobalSNOGet(917), cgtPSODefaults);
            break;
        }
        UpdateActions();
    }
}








void OnButton4(const UIID &, const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() == nControllerIndex)
    {
        switch (sgtWhatsNewSeasonData.m_eDisplayMode)
        {
        case DISPLAY_SEASON_SPLASH:
            ShowRewards();
            break;
        case DISPLAY_SEASON_CONQUESTS:
            ShowRewards();
            break;
        case DISPLAY_SEASON_REWARDS:
            ShowConquests();
            break;
        }
        UpdateActions();
    }
}






void OnDPadLeft(
    const UIID & id,
    const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;



    switch (sgtWhatsNewSeasonData.m_eDisplayMode)
    {
    case DISPLAY_SEASON_REWARDS:
        sgtWhatsNewSeasonData.m_eCurrentClass = Math::PinWrap(sgtWhatsNewSeasonData.m_eCurrentClass - 1, 0, (int32)NUM_PLAYER_CLASSES);
        sDisplayArmorRewardsForClass(sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale);
        SoundManager::PlaySound(GlobalSNOGet(918), cgtPSODefaults);
        break;
    case DISPLAY_SEASON_CONQUESTS:
        sgtWhatsNewSeasonData.m_nSelectedConquest = Math::PinWrap(sgtWhatsNewSeasonData.m_nSelectedConquest - 1, 1, MAX_CONQUESTS + 1);
        sUpdateDisplayedConquest();
        {
            UIID idScrollbox = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS, ".AchievementScrollBox");
            UIUtils::Console::ResetScrollBox(idScrollbox);
        }
        UpdateActions();
        SoundManager::PlaySound(GlobalSNOGet(918), cgtPSODefaults);
        break;
    }



}

void OnDPadRight(
    const UIID & id,
    const int32 nControllerIndex)
{
    if (UI_GetPrimaryUIController() != nControllerIndex)
        return;



    switch (sgtWhatsNewSeasonData.m_eDisplayMode)
    {
    case DISPLAY_SEASON_REWARDS:
        sgtWhatsNewSeasonData.m_eCurrentClass = Math::PinWrap(sgtWhatsNewSeasonData.m_eCurrentClass + 1, 0, (int32)NUM_PLAYER_CLASSES);
        sDisplayArmorRewardsForClass(sgtWhatsNewSeasonData.m_eCurrentClass, sgtWhatsNewSeasonData.m_bIsFemale);
        SoundManager::PlaySound(GlobalSNOGet(918), cgtPSODefaults);
        break;
    case DISPLAY_SEASON_CONQUESTS:
        sgtWhatsNewSeasonData.m_nSelectedConquest = Math::PinWrap(sgtWhatsNewSeasonData.m_nSelectedConquest + 1, 1, MAX_CONQUESTS + 1);
        sUpdateDisplayedConquest();
        {
            UIID idScrollbox = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS, ".AchievementScrollBox");
            UIUtils::Console::ResetScrollBox(idScrollbox);
        }
        UpdateActions();
        SoundManager::PlaySound(GlobalSNOGet(918), cgtPSODefaults);
        break;
    }




}


void sSwitchToWhatsNew(const int32 nControllerIndex)
{
    Close();
    UIWhatsNew::Console::OpenForController(nControllerIndex, FALSE, FALSE);
}



void OnL1(const UIID &, const int32 nControllerIndex)
{
    sSwitchToWhatsNew(nControllerIndex);
}



void OnR1(const UIID &, const int32 nControllerIndex)
{
    sSwitchToWhatsNew(nControllerIndex);
}





void OnL2(const UIID &, const int32 nControllerIndex)
{
    switch (sgtWhatsNewSeasonData.m_eDisplayMode)
    {
    case DISPLAY_SEASON_REWARDS:
        if (!::Console::ControllerUtils::IsButtonDown(sgtWhatsNewSeasonData.m_nControllerIndex, 7))
        {
            UIUtils::Console::ScrollContentInScrollBox(UISCROLLBOX_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC, -4.0f);
        }
        break;
    case DISPLAY_SEASON_CONQUESTS:
        if (!::Console::ControllerUtils::IsButtonDown(sgtWhatsNewSeasonData.m_nControllerIndex, 7))
        {
            UIID idScrollbox = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS, ".AchievementScrollBox");

            UIUtils::Console::ScrollContentInScrollBox(idScrollbox, -4.0f);
        }
        break;
    }
}






void OnR2(const UIID &, const int32 nControllerIndex)
{
    switch (sgtWhatsNewSeasonData.m_eDisplayMode)
    {
    case DISPLAY_SEASON_REWARDS:
        if (!::Console::ControllerUtils::IsButtonDown(sgtWhatsNewSeasonData.m_nControllerIndex, 6))
        {
            UIUtils::Console::ScrollContentInScrollBox(UISCROLLBOX_CONSOLE_WHATS_NEW_SEASON_REWARDS_ARMOR_SET_DESC, 4.0f);
        }
        break;
    case DISPLAY_SEASON_CONQUESTS:
        if (!::Console::ControllerUtils::IsButtonDown(sgtWhatsNewSeasonData.m_nControllerIndex, 6))
        {
            UIID idScrollbox = UIIDCreateFromParent(UIWND_CONSOLE_WHATS_NEW_SEASON_CONQUESTS, ".AchievementScrollBox");

            UIUtils::Console::ScrollContentInScrollBox(idScrollbox, 4.0f);
        }
        break;
    }
}

}  // namespace Console
}  // namespace UIWhatsNewSeason
