// Original file: X/Client/UserInterface/Console/UIConsoleHallOfFallenHeroes.cpp
// Local declarations stand in for headers not adopted yet; each names its defining file.
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "../../Battlenet/Subsystems/ModalNotificationManager.h"
#include "../../CGame.h"
#include "../../Console/ControllerUtils.h"
#include "../../Console/GamerProfile.h"
#include "ConsoleInputUtilities.h"
#include "ConsoleModalNotificationManager.h"
#include "ConsoleScreenManager.h"
#include "ControllerActions.h"
#include "UIConsoleHallOfFallenHeroes.h"
#include "../Controls/UIRActorControl.h"
#include "../Controls/UITextControl.h"
#include "../Controls/UIWindowControl.h"
#include "../Interface/UIHero.h"
#include "../UI.h"
#include "../UIID.h"
#include "../../RActors/RActors.h"
#include "../../EffectGroup.h"
#include "../../Sound/Sound.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Common/Definitions/AttachmentDefinition.h"
#include "../../../Common/Definitions/StringListDefinition.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/OnlineService/OnlineService.h"
#include "../../../Common/Protobuf/Console.pb.h"
#include "../../../Common/TagMaps/AnimTagsSwig.h"
#include "../../../Common/VisualInventory.h"
#include "../../../Sigma/LanguageRules.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Transform.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/UnicodeText.h"
// X/Client/Console/GamerProfile.h; declared here because adding it there would pull VisualInventory.h into every includer.
namespace Console { namespace GamerProfile {
struct HeroInfo { HeroInfo(); void Import(const D3::Console::HeroInfo & tHeroInfo); void Export(D3::Console::HeroInfo & tHeroInfo) const; OnlineService::HeroId uHeroId; Blizzard::Time::UnixTime uCreationTime; char szFilename[32]; char szName[49]; PlayerClass eClass; uint32 uLevel; uint32 uAltLevel; uint32 uPlayerFlags; Act eLastAct; Blizzard::Time::UnixTime uDeathTime; SNO snoKiller; MonsterRarity eKillerRarity; GBID arKillerRareNameGBIDs[2]; CRefString usKillerName; uint64 uMonstersKilled; uint64 uElitesKilled; uint64 uGoldCollected; uint32 uTimePlayed; uint32 uSeasonCreated; VisualEquipment tEquipment; };
static_assert(sizeof(HeroInfo) == 0x150, "HeroInfo");
const HeroInfo * GetHeroList(int32 nUserIdx, uint32 & uNumHeroes); } }  // GamerProfileNX64.cpp:1202
extern UIID UWND_CONSOLE_HOFH, URACTOR_CONSOLE_HOFH_HERO, UTEXT_CONSOLE_HOFH_NAME, UTEXT_CONSOLE_HOFH_SLAIN_BY_LABEL, UTEXT_CONSOLE_HOFH_SLAIN_BY, UTEXT_CONSOLE_HOFH_GOLD, UTEXT_CONSOLE_HOFH_MONSTERS, UTEXT_CONSOLE_HOFH_TIME, UTEXT_CONSOLE_HOFH_LEVEL, USTACK_CONSOLE_HOFH_HEROES;  // UIIDs.h
namespace UIInventory { namespace Console { void UpdatePlayerActorUI(const UIID & idRActorControl, VisualInventory & tVisualInventory, PlayerClass eClass, BOOL bIsFemale, uint32 dwAnimTag); } }  // UIConsoleInventoryViewUtilities.cpp:1834
UIRActor & UI_GetRActorFromID(const UIID & id); void UI_SetPrimaryUIController(int32 nPrimaryController); SNO UI_GetDefaultHeroLight();  // UI.cpp
LightID LightAdd(const uint32 dwFlags, const SNO sno, const PRTransform & transform, const int32 nPriorityBoost, const RenderContext eContext);  // Lights.cpp:1104
AttachmentID RActorAttachExistingRActor(const RActorID idRActorMain, const RActorID idRActorAttachment, HardpointHash szHardpointName);  // RActorAttachments.cpp:2348
struct EffectGroup; EffectGroup * EffectGroupTryToGet(EffectGroupID idEffectGroup); void EffectGroupDelete(const EffectGroupID idEffectGroup); EffectGroupID EffectGroupCreateTracked(SNO snoEffectGroup, RActorID idRActorSource, RActorID idRActorTarget, int32 nUnused0, int32 nUnused1);  // X/Client/EffectGroup.cpp
CRefString CPlayerGetLevelString(const int32 nLevel, const int32 nAltLevel);  // X/Client/CPlayer.cpp

namespace UIHallOfFallenHeroes {
namespace Console {

namespace {
void OnFallenHeroInfoReceived(uint32 nNumHeroes);
void UpdateSelectedStateForMinis();
void UpdateSelectedHeroRActor();
void UpdateStatsDisplay();
void CleanupRActors(); UIID GetUIRactorForIndex(int32 nIndex); void CreateHeroMinis();
}



struct HallOfFallenHeroesData {  // size 0x6e8
    int32 nSelectedHero;  // 0x00
    int32 nNumFallenHeroes;  // 0x04
    OnlineService::HeroId idHeroToArchive;  // 0x08
    HeroArchivingComplete cbArchiveComplete;  // 0x10
    ::Console::AnalogStickInputFilter tStickFilter;  // 0x18
    ::Console::GamerProfile::HeroInfo arFallenHeroes[MAX_FALLEN_HEROES];  // 0x40
    RActorID arRActorMiniWaypoints[MAX_FALLEN_HEROES];  // 0x6d0
    EffectGroupID idSelectedEffect;  // 0x6e4


    HallOfFallenHeroesData()
        : nSelectedHero(-1), nNumFallenHeroes(0)

        , idHeroToArchive(OnlineService::INVALID_HERO_ID)
        , idSelectedEffect(-1)
    {
        tStickFilter = ::Console::GetDefaultStickInputFilter(::Console::FILTER_HORIZONTAL);
        tStickFilter.SetMinimumInputDelay(250);

        for (int32 i = 0; i < MAX_FALLEN_HEROES; i++)
        {
            arRActorMiniWaypoints[i] = RACTORID_INVALID;
        }
    }
};
HallOfFallenHeroesData sgtHallOfFallenHeroesData;


void CommonOpen(int32 nControllerIndex)

{
    ::Console::Screens::ShowModalDialog(nControllerIndex, UWND_CONSOLE_HOFH, TRUE, GetUIIDNone());
    UI_SetPrimaryUIController(nControllerIndex);
    sgtHallOfFallenHeroesData.nSelectedHero = -1;
    sgtHallOfFallenHeroesData.nNumFallenHeroes = 0;
    D3::Console::Profile & tProfile = ::Console::GamerProfile::GetProfile(nControllerIndex);
    sgtHallOfFallenHeroesData.nNumFallenHeroes = Math::Min<int32>(tProfile.fallen_heroes_size(), MAX_FALLEN_HEROES);
    for (int32 i = 0; i < sgtHallOfFallenHeroesData.nNumFallenHeroes; i++)
    {
        sgtHallOfFallenHeroesData.arFallenHeroes[i].Import(tProfile.fallen_heroes(i));
    }
    OnFallenHeroInfoReceived(sgtHallOfFallenHeroesData.nNumFallenHeroes);
    ::Console::ControllerActionManager & tActionManager = ::Console::Screens::GetActionManager();
    tActionManager.ClearAllActions();
    if (sgtHallOfFallenHeroesData.idHeroToArchive != OnlineService::INVALID_HERO_ID)
    {
        tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_1, 0, StringListFetch("Console:HardcoreReplace"));
    }
    tActionManager.SetAction(::Console::CONTROLLER_INPUT_BUTTON_2, 1, StringListFetch("ConsoleUI:Close"));
    ::Console::Screens::UpdateButtonLegendText();
}



void OpenForController(int32 nControllerIndex)

{
    sgtHallOfFallenHeroesData.idHeroToArchive = OnlineService::INVALID_HERO_ID;
    sgtHallOfFallenHeroesData.cbArchiveComplete.Clear();
    CommonOpen(nControllerIndex);
}



void OpenForController(int32 nControllerIndex, OnlineService::HeroId idHeroToArchive, HeroArchivingComplete cb)



{
    sgtHallOfFallenHeroesData.idHeroToArchive = idHeroToArchive;
    sgtHallOfFallenHeroesData.cbArchiveComplete = cb;
    CommonOpen(nControllerIndex);
    NotificationCreationParams tParams;
    tParams.nControllerIndex = nControllerIndex;
    tParams.uszMessage = StringListFetch("Console:HardcoreHallFull");
    ::Console::Screens::GetModalNotificationManager().SetOkNotification(tParams);
}



void Close()

{
    if (!IsVisible())
        return;
    SoundManager::PlaySound(GlobalSNOGet(GLOBALSNO_SOUND_PANEL_GENERIC_CLOSE), cgtPSODefaults);
    CleanupRActors();

    UI_SetPrimaryUIController(::Console::GamerProfile::GetPrimaryProfileUserIndex());
    ::Console::Screens::CloseModalDialog(UWND_CONSOLE_HOFH);

}



BOOL IsVisible()

{
    return GameIsStartUp() && UI_GetWindowFromID(UWND_CONSOLE_HOFH).IsActive();
}



void InitializeForWorld()

{
    if (IsVisible())
    {
        OnFallenHeroInfoReceived(sgtHallOfFallenHeroesData.nNumFallenHeroes);
    }
}



void DisposeForWorld()

{
    if (IsVisible())
    {
        if (sgtHallOfFallenHeroesData.idSelectedEffect != -1)
        {
            EffectGroupDelete(sgtHallOfFallenHeroesData.idSelectedEffect);
            sgtHallOfFallenHeroesData.idSelectedEffect = -1;
        }
    }
}

void OnLeftStick(const UIID &, const int32 nControllerIndex)


{
    ::Console::AnalogStickInputFilter::Rule::ID idRule = sgtHallOfFallenHeroesData.tStickFilter.HandleInput(::Console::ControllerUtils::GetLStickPosition(nControllerIndex, FALSE), FALSE);
    if (idRule == ::Console::RULE_STICK_LEFT)
    {
        OnDpadLeft(GetUIIDNone(), nControllerIndex);
    }
    else if (idRule == ::Console::RULE_STICK_RIGHT)
    {
        OnDpadRight(GetUIIDNone(), nControllerIndex);
    }
}



void OnDpadLeft(const UIID &, const int32)


{
    int32 nSelectedHero = Math::Pin(sgtHallOfFallenHeroesData.nSelectedHero - 1, 0, sgtHallOfFallenHeroesData.nNumFallenHeroes - 1);
    if (nSelectedHero != sgtHallOfFallenHeroesData.nSelectedHero)
    {
        sgtHallOfFallenHeroesData.nSelectedHero = nSelectedHero;

        UpdateSelectedStateForMinis();
        UpdateSelectedHeroRActor();
        UpdateStatsDisplay();
    }
}



void OnDpadRight(const UIID &, const int32)


{
    int32 nSelectedHero = Math::Pin(sgtHallOfFallenHeroesData.nSelectedHero + 1, 0, sgtHallOfFallenHeroesData.nNumFallenHeroes - 1);
    if (nSelectedHero != sgtHallOfFallenHeroesData.nSelectedHero)
    {
        sgtHallOfFallenHeroesData.nSelectedHero = nSelectedHero;

        UpdateSelectedStateForMinis();
        UpdateSelectedHeroRActor();
        UpdateStatsDisplay();
    }
}












void OnHeroArchivalFinished()


{
    Close();
    if (sgtHallOfFallenHeroesData.cbArchiveComplete)
    {
        sgtHallOfFallenHeroesData.cbArchiveComplete();
    }
}



void OnHeroArchivalAccepted(int32 nControllerIndex, const UIID &)


{
    D3::Console::Profile & tProfile = ::Console::GamerProfile::GetProfile(nControllerIndex);
    D3::Console::Profile tNewProfile;
    for (int32 i = 0; i < tProfile.fallen_heroes_size(); i++)
    {
        if (i != sgtHallOfFallenHeroesData.nSelectedHero)
        {
            tNewProfile.add_fallen_heroes()->CopyFrom(tProfile.fallen_heroes(i));
        }
        else
        {
            uint32 nNumHeroes = 0;
            const ::Console::GamerProfile::HeroInfo * ptHeroes = ::Console::GamerProfile::GetHeroList(nControllerIndex, nNumHeroes);
            for (uint32 j = 0; j < nNumHeroes; j++)
            {
                if (ptHeroes[j].uHeroId == sgtHallOfFallenHeroesData.idHeroToArchive)
                {
                    ptHeroes[j].Export(*tNewProfile.add_fallen_heroes());
                    break;
                }
            }
        }
    }
    tProfile.mutable_fallen_heroes()->Swap(tNewProfile.mutable_fallen_heroes());

    ::Console::GamerProfile::Save(nControllerIndex, ::Console::GamerProfile::SaveCallback());
    OnHeroArchivalFinished();

}



void OnAccept(const UIID &, const int32 nControllerIndex)


{
    if (sgtHallOfFallenHeroesData.idHeroToArchive != OnlineService::INVALID_HERO_ID
        && sgtHallOfFallenHeroesData.nSelectedHero != -1)
    {
        NotificationCreationParams tParams;
        tParams.nControllerIndex = nControllerIndex;
        tParams.uszMessage = LocalizedSprintf(
            StringListFetch("Console:HardcoreHallReplace").str(),
            sgtHallOfFallenHeroesData.arFallenHeroes[sgtHallOfFallenHeroesData.nSelectedHero].szName);
        tParams.tCallbacks.pfnOnClickOkButton = OnlineService::internal::function_binder_2<void, int32, const UIID &>::bind<&OnHeroArchivalAccepted>(nControllerIndex);
        ::Console::Screens::GetModalNotificationManager().SetOkCancelNotification(tParams);
    }
}



void OnCancel(const UIID &, const int32 nControllerIndex)


{
    if (GameIsStartUp())
    {
        Close();
    }
    else
    {
        ::Console::Screens::GoToPreviousScreenSet(nControllerIndex, FALSE);
    }
}



void OnLeftBumper(const UIID &, int32)
{








}



void OnRightBumper(const UIID &, int32)
{








}

namespace {



void OnFallenHeroInfoReceived(uint32 nNumHeroes)

{
    sgtHallOfFallenHeroesData.nNumFallenHeroes = nNumHeroes;

    if (sgtHallOfFallenHeroesData.nNumFallenHeroes > 0)
    {
        sgtHallOfFallenHeroesData.nSelectedHero = 0;
        CreateHeroMinis();
        UpdateSelectedHeroRActor();
        UpdateSelectedStateForMinis();
        UpdateStatsDisplay();
    }
}



UIID GetUIRactorForIndex(int32 nIndex)

{
    if (nIndex < sgtHallOfFallenHeroesData.nNumFallenHeroes)
    {
        return UIIDCreateFromParent(USTACK_CONSOLE_HOFH_HEROES, CRefString().Format(".Hero%d.Model", nIndex + 1));
    }

    return GetUIIDNone();
}



void CreateHeroMinis()

{
    for (int32 i = 0; i < sgtHallOfFallenHeroesData.nNumFallenHeroes; i++)
    {
        UIID idRactor = GetUIRactorForIndex(i);
        if (idRactor == GetUIIDNone())
        {
            continue;
        }

        UIRActor & tHero = UI_GetRActorFromID(idRactor);


        if (i == 0)
        {
            LightID idLight = LightAdd(
                0,
                UI_GetDefaultHeroLight(),
                PRTransform(cgqIdentity, cgwpNull),
                0,
                tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
            tHero.SetLight(idLight);
        }



        BOOL bFemale = (sgtHallOfFallenHeroesData.arFallenHeroes[i].uPlayerFlags & 2) != 0;

        VisualInventory tInventory;
        tInventory.Init();
        tInventory.tEquipmentCurrent = sgtHallOfFallenHeroesData.arFallenHeroes[i].tEquipment;

        UIInventory::Console::UpdatePlayerActorUI(idRactor, tInventory, sgtHallOfFallenHeroesData.arFallenHeroes[i].eClass, bFemale, TAG_ACTOR_ANIMATION_NEUTRAL);

        RActor * ptRActor = RActorGet(tHero.GetRActor());



        GlobalSNO gsnoWaypointActor = GLOBALSNO_ACTOR_CONSOLE_HERO_WAYPOINT_HC;
        RActorID idWaypoint = RActorCreate(
            GlobalSNOGet(gsnoWaypointActor),
            1.0f,
            -1.0f,
            PRTransform(cgqIdentity, cgwpNull),
            0,
            ptRActor->eContext,
            -1);
        sgtHallOfFallenHeroesData.arRActorMiniWaypoints[i] = idWaypoint;
        RActorAttachExistingRActor(
            ptRActor->id,
            idWaypoint,
            HardpointHash("Default"));
    }
}

void UpdateSelectedStateForMinis()

{
    int32 nSelectedHero = sgtHallOfFallenHeroesData.nSelectedHero;
    if (nSelectedHero < 0)
    {
        if (EffectGroupTryToGet(sgtHallOfFallenHeroesData.idSelectedEffect))
        {

            EffectGroupDelete(sgtHallOfFallenHeroesData.idSelectedEffect);
            sgtHallOfFallenHeroesData.idSelectedEffect = -1;
        }
        return;
    }

    UIRActor & tHero = UI_GetRActorFromID(GetUIRactorForIndex(nSelectedHero));
    if (EffectGroupTryToGet(sgtHallOfFallenHeroesData.idSelectedEffect))
    {

        if (tHero.GetRActor() == sgtHallOfFallenHeroesData.arRActorMiniWaypoints[nSelectedHero])
        {
            return;
        }




        EffectGroupDelete(sgtHallOfFallenHeroesData.idSelectedEffect);
        sgtHallOfFallenHeroesData.idSelectedEffect = -1;
    }
    sgtHallOfFallenHeroesData.idSelectedEffect =
        EffectGroupCreateTracked(
            GlobalSNOGet(GLOBALSNO_EFFECTGROUP_CONSOLE_HERO_WAYPOINT_HC),
            sgtHallOfFallenHeroesData.arRActorMiniWaypoints[nSelectedHero], RACTORID_INVALID, 0, 0);

}


void UpdateSelectedHeroRActor()

{
    int32 nSelectedHero = sgtHallOfFallenHeroesData.nSelectedHero;
    if (nSelectedHero < 0)
        return;


    BOOL bFemale = (sgtHallOfFallenHeroesData.arFallenHeroes[nSelectedHero].uPlayerFlags & 2) != 0;

    UIRActor & tHero = UI_GetRActorFromID(URACTOR_CONSOLE_HOFH_HERO);
    tHero.Cleanup();
    LightID idLight = LightAdd(
        0,
        UI_GetDefaultHeroLight(),
        PRTransform(cgqIdentity, cgwpNull),
        0,
        tHero.tRenderData[CGameAsyncGetRenderDataIndex(0)].m_tDisplayList.GetRenderContext());
    tHero.SetLight(idLight);
    VisualInventory tInventory;
    tInventory.Init();
    tInventory.tEquipmentCurrent = sgtHallOfFallenHeroesData.arFallenHeroes[nSelectedHero].tEquipment;

    UIInventory::Console::UpdatePlayerActorUI(URACTOR_CONSOLE_HOFH_HERO, tInventory, sgtHallOfFallenHeroesData.arFallenHeroes[nSelectedHero].eClass, bFemale, TAG_ACTOR_ANIMATION_NEUTRAL);

}



void UpdateStatsDisplay()

{
    int32 nSelectedHero = sgtHallOfFallenHeroesData.nSelectedHero;
    if (nSelectedHero < 0)
        return;
    ::Console::GamerProfile::HeroInfo & tHeroInfo = sgtHallOfFallenHeroesData.arFallenHeroes[nSelectedHero];
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_NAME).SetText(tHeroInfo.szName, FALSE);
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_GOLD).SetText(
        UnicodeText::FormatNumber((int64)tHeroInfo.uGoldCollected, TRUE, FNS_NORMAL, (XLocale)0), FALSE);
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_MONSTERS).SetText(
        UnicodeText::FormatNumber((int64)tHeroInfo.uMonstersKilled, TRUE, FNS_NORMAL, (XLocale)0), FALSE);
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_LEVEL).SetText(
        CPlayerGetLevelString(tHeroInfo.uLevel, tHeroInfo.uAltLevel), FALSE);


    CRefString sFormattedTime = UIText_GetTimeString(tHeroInfo.uTimePlayed);
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_TIME).SetText(sFormattedTime, FALSE);

    BOOL bFemale = (tHeroInfo.uPlayerFlags & 2) != 0;
    LanguageRules::SetClientGender(!bFemale);
    CRefString sSlainByLabel = LanguageRules::ApplyLanguageRules(
        StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_PROFILE), "SlainBy"));
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_SLAIN_BY_LABEL).SetText(sSlainByLabel, FALSE);


    CRefString strKiller;
    CRefString strPower;
    UIHero::GetKillerName(-1,
        tHeroInfo.snoKiller,
        -1,
        tHeroInfo.eKillerRarity,
        tHeroInfo.arKillerRareNameGBIDs, strKiller, strPower);








    if (strKiller.Length() == 0)
    {
        strKiller = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_HEROSELECT), "UnknownKiller");
    }
    UI_GetTextFromID(UTEXT_CONSOLE_HOFH_SLAIN_BY).SetText(strKiller.str(), FALSE);


}



void CleanupRActors()

{
    UI_GetRActorFromID(URACTOR_CONSOLE_HOFH_HERO).Cleanup();
    for (int32 i = 0; i < sgtHallOfFallenHeroesData.nNumFallenHeroes; i++)
    {
        UIID idRactor = GetUIRactorForIndex(i);
        if (idRactor != GetUIIDNone())
        {
            UI_GetRActorFromID(idRactor).Cleanup();
        }
    }


    sgtHallOfFallenHeroesData.idSelectedEffect = -1;
}

}  // namespace

}  // namespace Console
}  // namespace UIHallOfFallenHeroes
