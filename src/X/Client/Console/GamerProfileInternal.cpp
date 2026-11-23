// X/Client/Console/GamerProfileInternal.cpp
// Platform-independent gamer profile state: the save/load task thread, the main-thread callback
// queue, the primary user index and the per-user "new challenge" notifications.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"
#include "../Gfx/GfxVB.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../../BlizzardCore/Include/Blizzard/Thread.h"
#include "GamerProfile.h"
#include "GamerProfileInternal.h"
#include "../../Common/OnlineService/Callback/Callback_Impl.h"
#include "../../Common/OnlineService/Achievements.h"
#include "../../Sigma/DataStructures/XArray.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/SigmaProf.h"
#include "../../../Contrib/Contrib/blz/include/blz/atomic.h"
#include "../../Common/ThreadGlobals.h"
namespace Blizzard { namespace Process { void Sleep(uint32 uMilliseconds); } }  // BlizzardCore Process.h






















namespace Console { namespace GamerProfile { namespace Internal {
BOOL sbTaskThreadDone;


Blizzard::Thread::Thread stTaskThread;

Blizzard::Lock::RMutex stTaskQueueLock;
Blizzard::Lock::RSemaphore stTaskSemaphore;






XArray<Console::GamerProfile::Internal::SaveLoadTask *, Console::GamerProfile::Internal::SaveLoadTask *, 20> stTaskQueue;




Blizzard::Lock::RMutex stCallbackQueueLock;
XArray<OnlineService::Callback<void ()>, OnlineService::Callback<void ()>, 20> stCallbackQueue;

int32 snPrimaryUserIdx;

int32 snSaveEventRefCount;
Console::GamerProfile::BeginSaveTaskCallback spfnBeginSaveCallback;
Console::GamerProfile::EndSaveTaskCallback spfnEndSaveCallback;

blz::atomic_int32_t suNumOutstandingCriticalTasks(0); // DWARF line75 invokes atomic(int), original stores zero.
} } }



namespace Console { namespace GamerProfile {



void InitializeForApp()

{

    BOOL bSuccess = Blizzard::Thread::Create(Internal::sAsyncSaveLoadThread, NULL, &Internal::stTaskThread, "ConsoleSaveLoad", 1, 0x10000);
    Blizzard::Thread::SetPriority(Internal::stTaskThread, Blizzard::Thread::PRIORITY_BELOW_NORMAL);


    ConfigureConsoleSaveLoadThread(Internal::stTaskThread);

    SigmaProfRegisterThread(Internal::stTaskThread, "ConsoleSaveLoad", 0x1000);
    Blizzard::Thread::Resume(Internal::stTaskThread);
    Internal::snPrimaryUserIdx = -1;

    PlatformInitializeForApp();
}





void DisposeForApp()
{
    Internal::sbTaskThreadDone = TRUE;
    Internal::stTaskSemaphore.Set();


    SigmaProfUnregisterThread(Internal::stTaskThread);
    Blizzard::Thread::WaitForever(Internal::stTaskThread);
    Blizzard::Thread::Close(Internal::stTaskThread);

    PlatformDisposeForApp();
}




void Process()

{
    while (Internal::stCallbackQueue.GetArraySize() != 0)
    {
        Blizzard::Lock::MutexEnter(Internal::stCallbackQueueLock);
        Internal::SaveLoadTaskCallback pfnCallback = Internal::stCallbackQueue[0];
        Internal::stCallbackQueue.RemoveAt(0);
        Blizzard::Lock::MutexLeave(Internal::stCallbackQueueLock);

        if (pfnCallback)
        {
            pfnCallback();
        }
    }
}



































































void RegisterSaveTaskCallbacks(Console::GamerProfile::BeginSaveTaskCallback pfnBeginSaveCallback, Console::GamerProfile::EndSaveTaskCallback pfnEndSaveCallback)
{

    Internal::spfnBeginSaveCallback = pfnBeginSaveCallback;


    Internal::spfnEndSaveCallback = pfnEndSaveCallback;
}

























































































































































































































































































void Internal::QueueCallback(Console::GamerProfile::Internal::SaveLoadTaskCallback pfnCallback)
{

    if (pfnCallback)
    {
        Blizzard::Lock::ScopedMutex tMutex(stCallbackQueueLock);
        stCallbackQueue.AppendElement_Tracked() = pfnCallback;
    }
}













int32 Internal::GetPrimaryProfileUserIndex()
{

    return snPrimaryUserIdx;
}




void Internal::SetPrimaryProfileUserIndex(int32 nUserIdx)
{

    snPrimaryUserIdx = nUserIdx;
}












































































































































void Internal::SaveLoadTask::Queue()

{









    Acquire();

    {
        Blizzard::Lock::ScopedMutex tMutex(stTaskQueueLock);
        stTaskQueue.AppendElement_Tracked() = this;
        if (IsCritical())
        {
            ++suNumOutstandingCriticalTasks;
        }
    }
    stTaskSemaphore.Set();
}




void Internal::RemoveTasks(Console::GamerProfile::Internal::TaskCompareFunc pfnCompare)
{

    Blizzard::Lock::ScopedMutex tMutex(stTaskQueueLock);

    for (int32 i = 0; i < stTaskQueue.GetArraySize(); )
    {
        SaveLoadTask * ptTask = stTaskQueue[i];
        if (pfnCompare(ptTask))
        {
            if (ptTask->IsCritical())
            {
                --suNumOutstandingCriticalTasks;
            }

            stTaskQueue.RemoveAt(i);
            ptTask->Release();
        }
        else
        {
            i++;
        }
    }
}




uint32 Internal::sAsyncSaveLoadThread(void * pParam)

{


    ThreadGlobals::InitializeForThread();
    SigmaProfInitializeForThread();

    ThreadGlobals::Get().m_nThreadIndex = THREADINDEX_SAVELOADTASK;

    while (!sbTaskThreadDone || stTaskQueue.GetArraySize() != 0)
    {
        stTaskSemaphore.WaitForever();

        while (stTaskQueue.GetArraySize() != 0)
        {
            SaveLoadTask * ptTask;

            {
                Blizzard::Lock::ScopedMutex tMutex(stTaskQueueLock);
                ptTask = stTaskQueue[0];
                stTaskQueue.RemoveAt(0);
            }

            ptTask->Process();

            if (ptTask->IsCritical())
            {
                Blizzard::Lock::ScopedMutex tMutex(stTaskQueueLock);
                --suNumOutstandingCriticalTasks;
            }

            ptTask->Release();

            Blizzard::Process::Sleep(10);
        }
    }

    SigmaProfDisposeForThread();
    ThreadGlobals::DisposeForThread();

    return 0;
}




void Internal::NotifyBeginSaveCallback()
{
    if (snSaveEventRefCount == 0 && spfnBeginSaveCallback)
    {
        QueueCallback(spfnBeginSaveCallback);
    }



    snSaveEventRefCount++;
}




void Internal::NotifyEndSaveCallback()
{
    snSaveEventRefCount--;
    if (snSaveEventRefCount == 0 && spfnEndSaveCallback)
    {
        QueueCallback(spfnEndSaveCallback);
    }
}











struct UserNewChallengeNotifications {  // size 0x100

    UserNewChallengeNotifications() {}



    void Clear()
    {
        m_mapNewAchievements.RemoveAll();
        m_mapNewRewards.RemoveAll();
    }






    XGrowableMap<unsigned long, unsigned long, int, int> m_mapNewAchievements;  // 0x00
    XGrowableMap<unsigned int, unsigned int, int, int> m_mapNewRewards;  // 0x80
};

UserNewChallengeNotifications s_atUserNewChallengeNotifications[4];






void Internal::ClearUserNewChallengeNotifications(int32 nUserIdx)
{

    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        return;
    }

    s_atUserNewChallengeNotifications[nUserIdx].Clear();
}
































void OnChallengeViewed(int32 nUserIdx, OnlineService::AchievementId idAchievement)
{


    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        return;
    }
    UserNewChallengeNotifications & tNotifications = s_atUserNewChallengeNotifications[nUserIdx];

    if (tNotifications.m_mapNewAchievements.LookupPtr(idAchievement))
    {
        tNotifications.m_mapNewAchievements.RemoveAt(idAchievement);
    }
}




void OnRewardViewed(int32 nUserIdx, OnlineService::RewardId idReward)
{


    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        return;
    }
    UserNewChallengeNotifications & tNotifications = s_atUserNewChallengeNotifications[nUserIdx];

    if (tNotifications.m_mapNewRewards.LookupPtr(idReward))
    {
        tNotifications.m_mapNewRewards.RemoveAt(idReward);
    }
}




BOOL IsChallengeNew(int32 nUserIdx, OnlineService::AchievementId idAchievement)
{


    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        return FALSE;
    }
    UserNewChallengeNotifications & tNotifications = s_atUserNewChallengeNotifications[nUserIdx];

    return tNotifications.m_mapNewAchievements.LookupPtr(idAchievement) != NULL;
}




BOOL IsRewardNew(int32 nUserIdx, OnlineService::RewardId idReward)
{


    if (nUserIdx < 0 || nUserIdx >= 4)
    {
        return FALSE;
    }
    UserNewChallengeNotifications & tNotifications = s_atUserNewChallengeNotifications[nUserIdx];

    return tNotifications.m_mapNewRewards.LookupPtr(idReward) != NULL;
}



} }

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Common/Player.h"
#include "../../Common/VisualInventory.h"
#include "../../Common/GameBalance.h"
#include "../../Common/Protobuf/Hero.pb.h"
#include "../../Common/Protobuf/Console.pb.h"
#include "../../Common/Definitions/HeroDefinition.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/Achievements/Console/ConsoleAchievements.h"
#include "../../Common/Game.h"
// Original external declarations from Blizzard Util and HeroDefinition.cpp.
namespace Blizzard {
namespace Util {
uint64 Hash64(const char * string);
}
}
void HeroDefinition_Init(D3::Hero::SavedDefinition & tSavedDefinition);
namespace Console {
namespace GamerProfile {
// Owner-local recovery of the original GamerProfile.h:363 HeroInfo layout.
struct HeroInfo {  // size 0x150
    HeroInfo();
    void Import(const D3::Hero::SavedDefinition & tDefinition);
    void Import(const D3::Console::HeroInfo & tHeroInfo);
    void Export(D3::Console::HeroInfo & tHeroInfo) const;
    OnlineService::HeroId uHeroId;  // 0x00
    Blizzard::Time::UnixTime uCreationTime;  // 0x08
    char szFilename[32];  // 0x0c
    char szName[49];  // 0x2c
    PlayerClass eClass;  // 0x60
    uint32 uLevel;  // 0x64
    uint32 uAltLevel;  // 0x68
    uint32 uPlayerFlags;  // 0x6c
    Act eLastAct;  // 0x70
    Blizzard::Time::UnixTime uDeathTime;  // 0x74
    SNO snoKiller;  // 0x78
    MonsterRarity eKillerRarity;  // 0x7c
    GBID arKillerRareNameGBIDs[2];  // 0x80
    CRefString usKillerName;  // 0x88
    uint64 uMonstersKilled;  // 0xa0
    uint64 uElitesKilled;  // 0xa8
    uint64 uGoldCollected;  // 0xb0
    uint32 uTimePlayed;  // 0xb8
    uint32 uSeasonCreated;  // 0xbc
    VisualEquipment tEquipment;  // 0xc0
};
static_assert(sizeof(HeroInfo) == 0x150, "HeroInfo");


// Inferred structured uniqueness loop reproduces original low32-bit comparisons and zero-ID exit.
// Random creation uses original literals: player flag2, GB_HEROS7, classes0..6, name Maximus Randomus.
const HeroInfo * GetHeroList(int32 nUserIdx, uint32 & uNumHeroes);
void SetHero(int32 nUserIdx, const D3::Hero::SavedDefinition & tHeroDefinition);

#line 250
OnlineService::HeroId GenerateHeroID(int32 nUserIdx, const char * szCharName)
{
#line 254
    OnlineService::HeroId uId = 0;  // DWARF: uId is the constant 0 before the Hash64 store
#line 281
    uId = Blizzard::Util::Hash64(szCharName);
#line 285
    uint32 uNumHeroes;
    const HeroInfo * patHeroes = GetHeroList(nUserIdx, uNumHeroes);
#line 288
    for (;;)
    {
        BOOL bUnique = TRUE;
#line 292
        for (uint32 i = 0; bUnique && i < uNumHeroes; ++i)  // && at column 36
        {
#line 295
            bUnique = (uint32)patHeroes[i].uHeroId != (uint32)uId;
        }
#line 298
        if (bUnique || (uint32)uId == 0)
            break;
        ++uId;
    }
    return uId;

}

#line 215
void CreateRandomHero(int32 nUserIdx, PlayerClass eRequiredClass)
{
#line 219
    D3::Hero::SavedDefinition tHeroDefinition;
    HeroDefinition_Init(tHeroDefinition);
#line 223
    uint32 dwPlayerFlags = GameRandRangeInt(0, 1) ? 0 : 2;
#line 226
    PlayerClass eClass = eRequiredClass;
    if (eClass == -1)
        eClass = (PlayerClass)GameRandRangeInt(0, 6);
#line 228
    CRefString sRandomName("Maximus Randomus");
#line 229
    OnlineService::HeroId idHero = GenerateHeroID(nUserIdx, sRandomName.str());
#line 231
    D3::Hero::Digest & tDigest = *tHeroDefinition.mutable_digest();
    tDigest.set_hero_name(sRandomName.str(), sRandomName.Length());
    tDigest.set_gbid_class(PlayerClassToGBID(eClass));
    tDigest.set_player_flags(dwPlayerFlags);
    tHeroDefinition.set_version(gptGroupDefHero->GetCurrentVersion(geSigmaPlatform));
#line 237
    GBHandle gbHandle(7, PlayerClassToGBID(eClass));
    gbHandle.Export(*tHeroDefinition.mutable_saved_attributes()->mutable_gb_handle());
    tDigest.mutable_hero_id()->set_id_high(0);
    tDigest.mutable_hero_id()->set_id_low(idHero);
    SetHero(nUserIdx, tHeroDefinition);
}
#line 314
HeroInfo::HeroInfo()
    : uCreationTime(0), eClass((PlayerClass)-1), uLevel(0), uAltLevel(0), uPlayerFlags(0),
      eLastAct((Act)-1), uDeathTime(0), snoKiller(SNO_NONE), eKillerRarity((MonsterRarity)-1),
      uMonstersKilled(0), uElitesKilled(0), uGoldCollected(0), uTimePlayed(0), uSeasonCreated(0)
{
    SigmaMemoryClear(szName, sizeof(szName));
#line 334
    for (int32 i = 0; i < 2; ++i)
        arKillerRareNameGBIDs[i] = GBID_NONE;
    tEquipment.Init();
    szFilename[0] = '\0';
}

#line 345
void HeroInfo::Import(const D3::Hero::SavedDefinition & tDefinition)
{
#line 348
    const D3::Hero::Digest & tDigest = tDefinition.digest();
    uHeroId = tDigest.hero_id().id_low();
    uCreationTime = tDigest.create_time();
    Blizzard::String::Copy(szName, tDigest.hero_name().c_str(), sizeof(szName));
    eClass = PlayerClassFromGBID(tDigest.gbid_class());
    uLevel = tDigest.level();
    uAltLevel = tDigest.deprecated_alt_level();
    uPlayerFlags = tDigest.player_flags();
    if (tDefinition.console_data().has_last_played_adventure_mode_act()) eLastAct = tDefinition.console_data().last_played_adventure_mode_act(); else eLastAct = tDigest.last_played_act();  // if/else, not a ternary: the original tests at 359:9 and stores from 361/365
    tEquipment.Init();
    tEquipment.Import(tDigest.visual_equipment());
    if (tDefinition.has_console_data() && tDefinition.console_data().has_hero_profile())
    {
#line 374
        const D3::Hero::ConsoleData & tConsoleData = tDefinition.console_data();
#line 375
        const D3::Profile::HeroProfile & tProfile = tConsoleData.hero_profile();
        uDeathTime = tProfile.has_death_time() ? tProfile.death_time() : 0;
        if (tProfile.has_killer_info())
        {
#line 381
            const D3::Profile::KillerInfo & tKillerInfo = tProfile.killer_info();
            snoKiller = tKillerInfo.sno_killer();
            eKillerRarity = (MonsterRarity)tKillerInfo.rarity();
            if (eKillerRarity == 2)
#line 387
                for (int32 i = 0; i < 2; ++i)
                    arKillerRareNameGBIDs[i] = tKillerInfo.rare_name_gbids(i);
            usKillerName = tConsoleData.killer_name().c_str();
        }
        uMonstersKilled = tProfile.has_monsters_killed() ? tProfile.monsters_killed() : 0;
        uElitesKilled = tProfile.has_elites_killed() ? tProfile.elites_killed() : 0;
        uGoldCollected = tProfile.has_gold_collected() ? tProfile.gold_collected() : 0;
    }
    uTimePlayed = tDigest.time_played();
    uSeasonCreated = tDigest.season_created();
}
#line 409
void HeroInfo::Import(const D3::Console::HeroInfo & tHeroInfo)
{
    uHeroId = tHeroInfo.hero_id().id_low();
    Blizzard::String::Copy(szFilename, tHeroInfo.filename().c_str(), sizeof(szFilename));
    uCreationTime = tHeroInfo.create_time();
    Blizzard::String::Copy(szName, tHeroInfo.hero_name().c_str(), sizeof(szName));
    eClass = PlayerClassFromGBID(tHeroInfo.gbid_class());
    uLevel = tHeroInfo.level();
    uAltLevel = tHeroInfo.alt_level();
    uPlayerFlags = tHeroInfo.player_flags();
    eLastAct = tHeroInfo.last_played_act();
    tEquipment.Init();
    tEquipment.Import(tHeroInfo.visual_equipment());
    uDeathTime = tHeroInfo.death_time();
#line 428
    const D3::Profile::KillerInfo & tKillerInfo = tHeroInfo.killer_info();
    snoKiller = tKillerInfo.sno_killer();
    eKillerRarity = (MonsterRarity)tKillerInfo.rarity();
    if (eKillerRarity == 2)
#line 433
        for (int32 i = 0; i < 2; ++i)
            arKillerRareNameGBIDs[i] = i < tKillerInfo.rare_name_gbids_size() ? tKillerInfo.rare_name_gbids(i) : GBID_NONE;
    usKillerName = tHeroInfo.killer_name().c_str();
    uMonstersKilled = tHeroInfo.monsters_killed();
    uElitesKilled = tHeroInfo.elites_killed();
    uGoldCollected = tHeroInfo.gold_collected();
    uTimePlayed = tHeroInfo.time_played();
    uSeasonCreated = tHeroInfo.season_created();
}
#line 450
void HeroInfo::Export(D3::Console::HeroInfo & tHeroInfo) const
{
    tHeroInfo.mutable_hero_id()->set_id_high(0);
    tHeroInfo.mutable_hero_id()->set_id_low(uHeroId);
    tHeroInfo.set_filename(szFilename);
    tHeroInfo.set_create_time(uCreationTime);
    tHeroInfo.set_hero_name(szName);
    tHeroInfo.set_gbid_class(PlayerClassToGBID(eClass));
    tHeroInfo.set_level(uLevel);
    tHeroInfo.set_alt_level(uAltLevel);
    tHeroInfo.set_player_flags(uPlayerFlags);
    tHeroInfo.set_last_played_act(eLastAct);
    tEquipment.Export(*tHeroInfo.mutable_visual_equipment());
    tHeroInfo.set_death_time(uDeathTime);
#line 471
    D3::Profile::KillerInfo & tKillerInfo = *tHeroInfo.mutable_killer_info();
    tKillerInfo.set_sno_killer(snoKiller);
    tKillerInfo.set_rarity(eKillerRarity);
    if (eKillerRarity == 2)
#line 476
        for (int32 i = 0; i < 2; ++i)
            tKillerInfo.add_rare_name_gbids(arKillerRareNameGBIDs[i]);
    tHeroInfo.set_killer_name(usKillerName);
    tHeroInfo.set_monsters_killed(uMonstersKilled);
    tHeroInfo.set_elites_killed(uElitesKilled);
    tHeroInfo.set_gold_collected(uGoldCollected);
    tHeroInfo.set_time_played(uTimePlayed);
    tHeroInfo.set_season_created(uSeasonCreated);
}

#line 850
void OnChallengeUnlocked(int32 nUserIdx, OnlineService::AchievementId idAchievement)
{
    if ((uint32)nUserIdx >= 4)
        return;
#line 859
    UserNewChallengeNotifications & tNotifications = s_atUserNewChallengeNotifications[nUserIdx];
    tNotifications.m_mapNewAchievements.SetAt_Tracked(idAchievement, TRUE);
#line 864
    Console::Achievements & tAchievements = *sgtAppGlobals.ptAchievements;
    OnlineService::RewardIdList arRewards;
#line 866
    tAchievements.GetAchievementRewards(idAchievement, arRewards);
#line 867
    for (uint32 i = 0; i < arRewards.size(); ++i)
        tNotifications.m_mapNewRewards.SetAt_Tracked(arRewards[i], TRUE);
}
}
}
#include "Online.h"
#include "../CPlayer.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Networking/GameMessage.h"
#include "../UserInterface/Console/ConsoleScreenManager.h"
#include "../UserInterface/Console/ConsoleModalNotificationManager.h"
// Authentic owner declarations absent from adopted headers.
Player * PlayerGetByControllerIndexAlways(int32 nControllerIndex);
void MessageSendToServer(GameMessageType nMessageType, void * pMessage, uint32 dwMessageSize, int32 nControllerIndex);
namespace ControllerDisconnectedHandler {
namespace Console {
void ControllerConnected(int32 nControllerIndex);
}
}
namespace Console {
namespace Online {
void PartyLeave();
}
namespace Screens {
BOOL OpenToScreen(const ScreenSets eScreenSet, const ScreenID idScreen, int32 nControllerOpening, BOOL fSkipTransition);
}
namespace GamerProfile {
void SignOut(int32 nUserIdx);
// Inferred branch nesting follows the original primary/secondary user paths.
// Screen-set values20/22/23 and leave message19 are the original enum values.
#line 533
void Internal::HandleUserSignedOut(int32 nUserIndex)
{
    if (snPrimaryUserIdx == nUserIndex)
    {
        if (Online::PartyIsInParty())
            Online::PartyLeave();
        if (!GameIsStartUp())
            GameStartMenu(GameGetCurrentAct());
        else if (Screens::IsScreenManagerValid())
        {
            if (Screens::IsScreenOpen(20, 0))
                Screens::CloseScreenSet(TRUE, TRUE);
            Screens::CloseAllModalDialogs();
            Screens::OpenToScreen(20, 0, -1, TRUE);
        }
        SignOut(nUserIndex);
        if (Screens::IsScreenManagerValid())
#line 579
            for (int32 i = -1; i < 4; ++i)
                Screens::GetModalNotificationManager().RemoveAllNotificationsForController(i);
    }
    else if (snPrimaryUserIdx == -1)
    {
        if (GameIsStartUp() && Screens::IsScreenManagerValid())
        {
            if (Screens::IsScreenOpen(20, 0))
                Screens::CloseScreenSet(TRUE, TRUE);
            Screens::CloseAllModalDialogs();
            Screens::OpenToScreen(20, 0, -1, TRUE);
        }
    }
    else
    {
        if (Online::PartyIsInParty(nUserIndex))
        {
            if (!GameIsStartUp())
            {
#line 594
                Player * ptPlayer = PlayerGetByControllerIndexAlways(nUserIndex);
                if (ptPlayer)
                    CPlayerLocalPlayerLeave(ptPlayer);
                else
                {
#line 601
                    SimpleMessage tMessage;
                    MessageSendToServer((GameMessageType)19, &tMessage, sizeof(tMessage), nUserIndex);
                }
            }
            Online::PartyRemovePlayer(nUserIndex);
        }
        if (Screens::IsScreenManagerValid() && Screens::GetCurrentControllerIndex() == nUserIndex)
        {
            if (GameIsStartUp() && !Screens::IsScreenSetOpen(20))
                Screens::OpenToScreen(20, 1, snPrimaryUserIdx, FALSE);
            else if (!GameIsStartUp())
            {
#line 623
                BOOL bSkipTransitions = Screens::IsScreenSetOpen(23) || Screens::IsScreenSetOpen(22);
                Screens::CloseAllScreenSets(bSkipTransitions);
            }
            Screens::CloseAllModalDialogs();
        }
        SignOut(nUserIndex);
        if (Screens::IsScreenManagerValid())
            Screens::GetModalNotificationManager().RemoveAllNotificationsForController(nUserIndex);
    }
    ControllerDisconnectedHandler::Console::ControllerConnected(nUserIndex);
}
}
}
