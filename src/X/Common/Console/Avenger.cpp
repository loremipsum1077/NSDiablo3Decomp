#include "Avenger.h"
#include "../ActorCommonData.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../GameCommonData.h"
#include "../GlobalSNO.h"
#include "../GameSwig.h"
#include "../Actors/ActorEffectCommon.h"
#include "../Player.h"
#include "../Protobuf/Account.pb.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "../../Server/Actors/ActorEffect.h"
#include "../../Server/Actors/Actors.h"
#include "../../Server/SGame.h"
#include "../../Client/CPlayer.h"
#include "../../Client/Console/GamerProfile.h"
#include "../../Client/Console/Online.h"
#include "../Definitions/GlobalDefinition.h"
#include "../Game.h"
#include "../../Sigma/Random.h"
#include "../../Client/UserInterface/UI.h"
#include "../../Client/UserInterface/UIID.h"
#include "../../Client/UserInterface/Controls/UIWindowControl.h"
#include "../Nav/NavigationMesh.h"
#include "../SceneCommon.h"
#include "../Attributes/FastAttribute.h"
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer); void MessageSendToServer(int32 nMessageID, const google::protobuf::MessageLite & tMessage, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
namespace Console { namespace Online { BOOL PresenceGetRandomFriend(int32 nUserIndex, Friend & tOutFriend, unsigned long idBlacklistPlayer); } }  // X/Client/Console/PresenceManager.cpp:4160

// X/Common/Console/Avenger.h (kept here: message structs are defined per file)











PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);  // Player.cpp:1634
void sPrefetchShadowCloneItems(PlayerAvengerData * ptPlayerAvengerData);
BOOL sSpawnAvenger(PlayerAvengerData * ptPlayerAvengerData, Player & tPlayer);
static const CHAR CHAR_MESSAGE_NO_MORE_VICTIMS = 'd';


void SavePlayerAvengerDataToAvengerAccountInfo(PlayerAvengerData & tPlayerAvengerData, D3::Account::Avenger & tAvengerAccountInfo)
{
    tAvengerAccountInfo.set_avenger_name(tPlayerAvengerData.sAvengerName.str(), tPlayerAvengerData.sAvengerName.Length());
    tAvengerAccountInfo.set_sent_from(0);
    tAvengerAccountInfo.set_affix_bucket(tPlayerAvengerData.nAvengerAffixBucket);
    Avenger::Console::VictimsCopyArrayToProtobuf(tPlayerAvengerData.arVictims, tAvengerAccountInfo.mutable_victims());
}




void Avenger::Console::SaveAvengerInfoToAccount(Player * ptPlayer, D3::Account::AvengerData & tAccountAvengerData)
{
    if (ptPlayer->tSoloAvenger.bHasAvenger)
    {
        PlayerAvengerData & tPlayerAvengerData = ptPlayer->tSoloAvenger;
        D3::Account::Avenger & tAvenger = *tAccountAvengerData.mutable_avenger_solo();
        SavePlayerAvengerDataToAvengerAccountInfo(tPlayerAvengerData, tAvenger);
    }
    else
    {
        tAccountAvengerData.clear_avenger_solo();
    }

    if (ptPlayer->tFriendAvenger.bHasAvenger)
    {
        PlayerAvengerData & tPlayerAvengerData = ptPlayer->tFriendAvenger;
        D3::Account::Avenger & tAvenger = *tAccountAvengerData.mutable_avenger_friends();
        SavePlayerAvengerDataToAvengerAccountInfo(tPlayerAvengerData, tAvenger);
        tAvenger.set_sent_from(tPlayerAvengerData.nSentFrom);
    }
    else
    {
        tAccountAvengerData.clear_avenger_friends();
    }
}





void Avenger::Console::SaveAvengerToPlayer(const D3::Account::Avenger & tAvenger, PlayerAvengerData * ptPlayerAvengerData, AvengerSource eSource)
{
    ptPlayerAvengerData->bHasAvenger = TRUE;
    ptPlayerAvengerData->bHasSpawned = FALSE;
    ptPlayerAvengerData->eAvengerSource = eSource;
    ptPlayerAvengerData->sAvengerName = tAvenger.avenger_name().c_str();
    ptPlayerAvengerData->nSentFrom = tAvenger.sent_from();
    ptPlayerAvengerData->nAvengerAffixBucket = tAvenger.affix_bucket();
    VictimsCopyProtobufToArray(tAvenger.victims(), ptPlayerAvengerData->arVictims);
}





void Avenger::Console::SaveAccountInfoToAvengerData(Player * ptPlayer, const D3::Account::AvengerData & tAvengerAccountData)
{
    if (tAvengerAccountData.has_avenger_solo())
    {
        D3::Account::Avenger tAvenger(tAvengerAccountData.avenger_solo());
        SaveAvengerToPlayer(tAvenger, &ptPlayer->tSoloAvenger, AVENGER_SOURCE_SOLO);
    }

    if (tAvengerAccountData.has_avenger_friends())
    {
        D3::Account::Avenger tAvenger(tAvengerAccountData.avenger_friends());
        SaveAvengerToPlayer(tAvenger, &ptPlayer->tFriendAvenger, AVENGER_SOURCE_FRIEND);
        if (!GameActiveGameCommonDataIsServer())
            CPlayerSetAvengerGiftRecipient(ptPlayer->tFriendAvenger.sAvengerName, tAvenger.sent_from());
    }
}











void Avenger::Console::InitializePlayerAvengerData(Player * ptPlayer, D3::Account::SavedDefinition & tDefinition)
{
    D3::Account::AvengerData & tAvengerAccountData = *tDefinition.mutable_console_data()->mutable_avenger_data();
    SaveAccountInfoToAvengerData(ptPlayer, tAvengerAccountData);
    if (ptPlayer->nControllerIndex == ::Console::GamerProfile::GetPrimaryProfileUserIndex())
        TryToGetFriendAvenger(FALSE);

    MessageSendToServer(519, tAvengerAccountData, ptPlayer);

    AvengerUpdateSourceMessage tSourceMessage;
    if (ptPlayer->tHeroStateData.uPlayerFlags & 1)
    {
        tSourceMessage.eAvengerSource = AVENGER_SOURCE_HARDCORE;
    }
    else
    {
        ::Console::Online::Friend tFriend;
        if (::Console::Online::PresenceGetRandomFriend(ptPlayer->nControllerIndex, tFriend, 0))
            tSourceMessage.eAvengerSource = AVENGER_SOURCE_FRIEND;
        else
            tSourceMessage.eAvengerSource = AVENGER_SOURCE_SOLO;
    }
    ptPlayer->eAvengerSource = tSourceMessage.eAvengerSource;
    MessageSendToServer(521, &tSourceMessage, sizeof(tSourceMessage), ptPlayer);
}
int32 MonsterGetNumberOfRandomAffixes(int32 nLevel, int32 nFixedAffixes);  // X/Server/RandomMonster.cpp:664
void MonsterAddAffixes(ActorID idActor, const ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);  // X/Server/RandomMonster.cpp:1197
ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd);  // X/Server/Actors/Actors.cpp:5915
void AwardPlayerKillAffixes(
    ActorAddStruct & tActorAdd,
    int32 & nAffixIndex,
    int32 nNumPlayerKills)
{

    if (nNumPlayerKills >= 1)
    {
        GBID gbidFast = StringToGBID("Fast");
        tActorAdd.pMonsterAffixGBIDs[nAffixIndex++] = gbidFast;
    }

    if (nNumPlayerKills >= 5)  // literal 5: cmp #5, no inlined GetMaxPlayerKills in the DWARF
    {
        GBID gbidReflectsDamageAffix = StringToGBID("ReflectsDamage");
        tActorAdd.pMonsterAffixGBIDs[nAffixIndex++] = gbidReflectsDamageAffix;
    }
}



static const int32 AFFIX_BUCKETS = 4;
static const int32 AFFIXES_PER_BUCKET = 3;

static const char s_asAvengerAffixBuckets[AFFIX_BUCKETS][AFFIXES_PER_BUCKET][40] = {
    { "AvengerArcaneEnchanted", "Vortex", "AvengerOrbiter" },
    { "AvengerFrozenPulse", "Illusionist", "Frozen" },
    { "AvengerPoisonEnchanted", "Vortex", "Waller" },
    { "Frozen", "AvengerPoisonEnchanted", "AvengerMortar" },
};
#include "../../Server/PlayerList.h"
#include "../../Client/SNOPreload.h"
#include "../Networking/GameMessage.h"
#include "../GameBalance.h"
#include "../Definitions/MonsterDefinition.h"
#include "../Definitions/TreasureClassDefinition.h"

// X/Common/Networking/GameMessage.h (not adopted yet).
BOOL NavMeshSearchForLocation(const WorldPlace & placeStart, const PassabilitySettings & tPassability, const float flMinSearchRadius, const float flMaxSearchRadius, const uint32 dwFindLocationFlags, NavMeshHandle & tNavMeshHandleLanding, WorldPlace & placeLanding, BOOL bSearchOverImpassable, Team eTeam, float flRadiusOverride);  // X/Common/Nav/NavigationMesh.cpp:3421
void BrainAPIActorSetAwarenessRadii(ACDID idACD, float flIdleRadius, float flCombatRadius);  // X/Server/AI/BrainComponents.cpp:8217
#include "../ActorCommonDataSwig.h"







void AwardPlayerLevelAffixes(
    ActorAddStruct & tActorAdd,
    int32 & nAffixIndex,
    int32 nCurrentLevel,
    PlayerAvengerData * ptPlayerAvengerData)
{
    int32 nAffixBucket = ptPlayerAvengerData->nAvengerAffixBucket;


    int32 nNumOfAffixes = MonsterGetNumberOfRandomAffixes(nCurrentLevel, 0);
    nNumOfAffixes = Math::Min(nNumOfAffixes, AFFIXES_PER_BUCKET);

	for (int32 nAffix = 0; nAffix < nNumOfAffixes; nAffix++)
	{
		GBID gbidAffix = StringToGBID(s_asAvengerAffixBuckets[nAffixBucket][nAffix]);
		tActorAdd.pMonsterAffixGBIDs[nAffixIndex++] = gbidAffix;
	}

}

BOOL sSpawnAvenger(
    PlayerAvengerData * ptPlayerAvengerData,
    Player & tPlayer)
{
    ActorCommonData* ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;




    WorldPlace tPlaceStart = ptACDPlayer->GetPlace();
    WorldPlace tPlaceResult;
    NavMeshHandle tGoodNavMesh;
    PassabilitySettings tPassability;
    PassabilitySettingsInit(tPassability);
    tPassability.ePassabilityType = PASSABILITY_MONSTER;


    BOOL bFoundGoodLoc = NavMeshSearchForLocation(tPlaceStart, tPassability, 15.0f, 35.0f, 0, tGoodNavMesh, tPlaceResult, FALSE, TEAM_MONSTER_0, 0.0f);
    if (!bFoundGoodLoc)
        return FALSE;


    if (NavMeshHandleGetLevelAreaID(tGoodNavMesh, FALSE) != ACDGetLevelAreaID(*ptACDPlayer))
        return FALSE;




    SNO snoAvengerMonster = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_AVENGER_MONSTER);
    ActorAddStruct tActorAdd;
    tActorAdd.qOrientation.SetRotation(cgwvZAxis, EIGHTH_CIRCLE);
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoAvengerMonster;
    tActorAdd.snoActor = snoAvengerMonster;
    tActorAdd.dwActorAddFlags = (1 << ACTORADD_DONT_PERTURB);
    tActorAdd.tPlace = tPlaceResult;
    tActorAdd.eTeam = TEAM_MONSTER_0;
    tActorAdd.bAvenger = TRUE;


    int32 nAffixIndex = 0;
    int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    AwardPlayerLevelAffixes(tActorAdd, nAffixIndex, nPlayerLevel, ptPlayerAvengerData);
    AwardPlayerKillAffixes(tActorAdd, nAffixIndex, (int32)ptPlayerAvengerData->arVictims.size());

    GBHandleList listGroupAffixes(GBGetHandlePool());
    EncounterAddStruct tEncounterAdd;
    EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

    ActorID idActor = GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
    if (idActor == ACTORID_INVALID)
        return FALSE;


    MonsterAddAffixes(idActor, tActorAdd, tEncounterAdd);
    ACDID idACD = ActorGet(idActor)->idACD;

    ActorCommonData * ptACDNewAvenger = ACDTryToGet(idACD);
    if (!ptACDNewAvenger)
        return FALSE;




    CRefString sAvengerName = ptPlayerAvengerData->sAvengerName;
    if (!ptACDNewAvenger->ptAvengerData)

        ptACDNewAvenger->ptAvengerData = new AvengerData;

    AvengerData & tMonsterAvengerData = *ptACDNewAvenger->ptAvengerData;
    tMonsterAvengerData.eAvengerSource = ptPlayerAvengerData->eAvengerSource;
    tMonsterAvengerData.nPlayerIndex = tPlayer.nPlayerIndex;
    tMonsterAvengerData.sName = sAvengerName;
    tMonsterAvengerData.nAffixBucket = ptPlayerAvengerData->nAvengerAffixBucket;
    Avenger::Console::VictimsCopyArrayToArray(ptPlayerAvengerData->arVictims, tMonsterAvengerData.arVictims);
    ptACDNewAvenger->eMonsterRarity = MR_UNIQUE;

    ptPlayerAvengerData->annTarget = ANNFromACD(idACD);
    ptPlayerAvengerData->bHasSpawned = TRUE;

    const float VERY_FAR = 9999.0f;
    ptACDNewAvenger->AttributesSetInt(ATTRIBUTE_NEVER_DEACTIVATES, 1);
    ptACDNewAvenger->SetInvulnerable(TRUE);
    ptACDNewAvenger->SetTargetable(FALSE);
    BrainAPIActorSetAwarenessRadii(idACD, VERY_FAR, VERY_FAR);

    ptPlayerAvengerData->uLastSpawnTime = Blizzard::Time::Milliseconds();

    SimpleMessage tMessage;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayers, tPlayer.idPlayerACD);
    MessageSendToClient(listDestPlayers, SMSG_AVENGER_SPAWN, &tMessage, sizeof(tMessage));

    return TRUE;
}


void Avenger::Console::StartAvengerSpawn(PlayerAvengerData * ptPlayerAvengerData, Player * ptPlayer)
{
    ptPlayerAvengerData->eSpawnState = SPAWN_STATE_PHASE_1;
    uint32 uNow = GameGetServerTime();
    ptPlayerAvengerData->uPhaseAdvanceTime = uNow + 120;

    PrefetchActorDataMessage tMessage;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    for (int32 i = 0; i < 8; i++)
        tMessage.gbidMonsterAffixes[i] = GBID_NONE;

    for (uint32 nIndex = 0; nIndex < ptPlayerAvengerData->arVictims.size(); nIndex++)
    {
        GlobalSNO eGlobalSNOClone = GetGlobalSNOActorForShadowCloneVictim(ptPlayerAvengerData->arVictims[nIndex]);
        if (eGlobalSNOClone != GLOBALSNO_INVALID)
        {
            tMessage.snoActor = GlobalSNOGet(eGlobalSNOClone);
            MessageSendToClient(listDestPlayers, 11, &tMessage, sizeof(tMessage));
            SNOPreload::PrefetchServerActorAvenger(tMessage.snoActor, NULL);
        }
    }

    int32 nAffixBucket = ptPlayerAvengerData->nAvengerAffixBucket;
    tMessage.snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_AVENGER_MONSTER);
    for (int32 i = 0; i < 8; i++)
    {
        GBID gbidAffix = GBID_NONE;
        if (i < AFFIXES_PER_BUCKET)
            gbidAffix = StringToGBID(s_asAvengerAffixBuckets[nAffixBucket][i]);
        tMessage.gbidMonsterAffixes[i] = gbidAffix;
    }
    MessageSendToClient(listDestPlayers, 11, &tMessage, sizeof(tMessage));
    SNOPreload::PrefetchServerActorAvenger(tMessage.snoActor, tMessage.gbidMonsterAffixes);
}

namespace Console { namespace Online {
void MailDelete(int32 nUserIndex, uint64 idMail, DeleteMailCallback pfnCallback);  // X/Client/Console/MailService.cpp:1311
}
}





void Avenger::Console::ResetAvengerSpawnTimer(ActorCommonData & tACD)
{
    tACD.AttributesSetFloat(ATTRIBUTE_AVENGER_BUILDUP_SECS, 0.0f);
}



BOOL Avenger::Console::HasPlayerWaitedEnoughForAvengerSpawn(const ActorCommonData & tACD)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    ScopedGlobalDefinitionLockR pdefGlobal;
    float flTimeWaitedSecs = tACD.AttributesGetFloat(ATTRIBUTE_AVENGER_BUILDUP_SECS);
    float flMinimumTimeSecs = pdefGlobal->nAvengerSpawnWaitTimeMinutes * 60.0f;
    if (tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_QUICK_SPAWN_BIT))
        flMinimumTimeSecs = 120.0f;
    return flTimeWaitedSecs > flMinimumTimeSecs;
}
#include "../../../Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.h"
#include "../Protobuf/ConsoleMail.pb.h"


// NON_MATCHING: 1548 vs 1528 bytes; same calls and control flow, register allocation differs (tAvengerMail and GetSender result spilled, 0x10 more stack)
void Avenger::Console::TryToGetFriendAvenger(
    BOOL bSendToServer)
{
    if (GameIsStartUp())
        return;




    int32 nPlayerIndex = ::Console::GamerProfile::GetPrimaryProfileUserIndex();
    Player* ptPlayer = PlayerGetByControllerIndex(nPlayerIndex);
    PlayerAvengerData* ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, AVENGER_SOURCE_FRIEND);
    if (ptPlayer && !ptPlayerAvengerData->bHasAvenger)
    {
        ::Console::Online::MailArray arMailArray;
        ::Console::Online::MailGetAllOfType(ptPlayer->nControllerIndex, ::Console::Online::MAIL_TYPE_EVENT, arMailArray, 0, 10000);
        int32 nHighestKillCount = 0;
        ::Console::Online::MailRef ptMailOfAvengerWithHighestKillCount;

        for (::Console::Online::MailRef& ptMailRef : arMailArray)
        {
            const D3::Console::MailItem& tMailItem = ptMailRef->GetContents();

            if (!ptMailRef->AreContentsValid())
            {
                ::Console::Online::MailDelete(ptPlayer->nControllerIndex, ptMailRef->GetID(), ::Console::Online::DeleteMailCallback());
            }
            else if (tMailItem.has_avenger_mail())
            {
                const D3::Console::AvengerMail& tAvengerMail = tMailItem.avenger_mail();
                if (tAvengerMail.victims_size() > nHighestKillCount)
                {
                    nHighestKillCount = tAvengerMail.victims_size();
                    ptMailOfAvengerWithHighestKillCount = ptMailRef;
                }
            }
        }

        if (ptMailOfAvengerWithHighestKillCount)
        {
            ::Console::Online::MailRef ptMailRef = ptMailOfAvengerWithHighestKillCount;
            const D3::Console::MailItem& tMailItem = ptMailRef->GetContents();
            D3::Account::SavedDefinition& tDefinition = ::Console::GamerProfile::GetAccount(ptPlayer->nControllerIndex);
            const D3::Console::AvengerMail& tAvengerMail = tMailItem.avenger_mail();
            D3::Account::AvengerData& tAvengerAccountData = *tDefinition.mutable_console_data()->mutable_avenger_data();
            D3::Account::Avenger& tAvenger = *tAvengerAccountData.mutable_avenger_friends();
            uint64 nDeadPlayersID = ptMailRef->GetSenderID().m_userID;





            tAvenger.set_avenger_name(ptMailRef->GetSender());


            tAvenger.set_sent_from(nDeadPlayersID);
            tAvenger.set_affix_bucket(tAvengerMail.affix_bucket());
            Avenger::Console::VictimsCopyProtobufToProtobuf(tAvengerMail.victims(), tAvenger.mutable_victims());

            ptPlayerAvengerData->bHasAvenger = TRUE;
            ptPlayerAvengerData->bHasSpawned = FALSE;
            ptPlayerAvengerData->eAvengerSource = AVENGER_SOURCE_FRIEND;
            ptPlayerAvengerData->sAvengerName = tAvenger.avenger_name().c_str();
            Avenger::Console::VictimsCopyProtobufToArray(tAvengerMail.victims(), ptPlayerAvengerData->arVictims);
            ptPlayerAvengerData->nSentFrom = nDeadPlayersID;
            ptPlayerAvengerData->nAvengerAffixBucket = tAvengerMail.affix_bucket();
            CPlayerSetAvengerGiftRecipient(ptPlayerAvengerData->sAvengerName, nDeadPlayersID);

            ::Console::Online::MailDelete(ptPlayer->nControllerIndex, ptMailRef->GetID(), ::Console::Online::DeleteMailCallback());

            if (bSendToServer)
            {
                MessageSendToServer(520, tAvengerAccountData, ptPlayer);
            }
        }
    }
}


void Avenger::Console::OnEventNewMail(int32 nUserIndex)
{
    if (::Console::GamerProfile::GetPrimaryProfileUserIndex() == nUserIndex)
        TryToGetFriendAvenger(TRUE);
}





void Avenger::Console::TryToRemoveAvenger(ACDID idACDPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer)
    {
        PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(ptPlayer, ptPlayer->eAvengerSource);
        if (ptPlayerAvengerData->bHasAvenger && ptPlayerAvengerData->bHasSpawned)
        {
            ACDID idAvenger = ACDFromANN(ptPlayerAvengerData->annTarget);
            ActorCommonData * ptACDAvenger = ACDTryToGet(idAvenger);
            if (ptACDAvenger)
            {
                RemoveShadowClones(ptACDAvenger->ptAvengerData->arVictimIDs);
                ptPlayerAvengerData->bHasSpawned = FALSE;
                ActorDelete((ActorID)ACDGetOwner(idAvenger));
            }
        }
    }
}



int32 Avenger::Console::AvengerGetNumberOfAffixBuckets()
{
    return AFFIX_BUCKETS;
}
#include "../Shell/CmdLine.h"
#include "../GameTemplate.h"
#include "../Definitions/LevelAreaDefinition.h"
BOOL SPlayerInScriptedSequence(const Player & tPlayer);  // X/Server/SPlayer.cpp:10849





































































































































































BOOL Avenger::Console::CanAvengerSpawnForPlayer(Player & tPlayer)
{
    if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        return FALSE;

    if (tPlayer.eAvengerSource == AVENGER_SOURCE_HARDCORE)
        return FALSE;

    PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource);
    ActorCommonData * ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    if ((tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_EASY_SPAWN_BIT)) && !ptPlayerAvengerData->bHasSpawned)
        return TRUE;

    if (!HasPlayerWaitedEnoughForAvengerSpawn(*ptACDPlayer))
        return FALSE;

    SNO snoLevelArea = ACDGetLevelArea(*ptACDPlayer);
    if (ptPlayerAvengerData->snoBossLevelArea != SNO_NONE && ptPlayerAvengerData->snoBossLevelArea == snoLevelArea)
        return FALSE;

    ScopedGlobalDefinitionLockR pdefGlobal;
    int32 nPlayerLevel = ptACDPlayer->AttributesGetInt(ATTRIBUTE_LEVEL);
    if (nPlayerLevel < pdefGlobal->nAvengerMinimumPlayerLevel)
        return FALSE;

    if (!(tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_QUICK_SPAWN_BIT)))
    {
        Blizzard::Time::Millisecond uNow = Blizzard::Time::Milliseconds();
        if (Blizzard::Time::GetTimeElapsed(ptPlayerAvengerData->uLastSpawnTime, uNow) < (uint32)(pdefGlobal->nAvengerSpawnCooldown * 1000))
            return FALSE;
    }

    if (ptPlayerAvengerData->bHasAvenger && !ptPlayerAvengerData->bHasSpawned && ptACDPlayer->IsAlive() && !ptACDPlayer->IsDueling() && !GameTypeIsPvP() &&
        !ptACDPlayer->AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER) && !ACDIsInTown(*ptACDPlayer) && tPlayer.bIsPrimaryPlayerServerOnly &&
        LevelAreaAllowsAvengers(snoLevelArea) && !SPlayerInScriptedSequence(tPlayer) && !ACDIsInTieredLootRun(*ptACDPlayer) &&
        !ptACDPlayer->AttributesGetInt(ATTRIBUTE_PARTICIPATING_IN_SET_DUNGEON))
        return TRUE;

    return FALSE;
}






























void Avenger::Console::CheckSpawnStartForPlayer(Player & tPlayer)
{
    if (CanAvengerSpawnForPlayer(tPlayer))
    {
        PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource);
        if (ptPlayerAvengerData->eSpawnState == SPAWN_STATE_NONE)
        {
            AppGlobals & tAppGlobals = sgtAppGlobals;
            ScopedGlobalDefinitionLockR pdefGlobal;
            float flRoll = RandFloat_Internal(SGameGlobalsGet().rng);
            if (flRoll < pdefGlobal->flAvengerSpawnChance ||
                (tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_ALWAYS_SPAWN_BIT)) ||
                (tAppGlobals.dwDebugFlags2 & (1 << APP_GLOBAL2_AVENGER_EASY_SPAWN_BIT)))
            {
                Player * ptPlayer;  // the original passes this uninitialised pointer (no store to x1 before the call)
                StartAvengerSpawn(GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource), ptPlayer);
            }
        }
    }
}





void Avenger::Console::PlayExitEffect(ActorCommonData & tACDAvenger)
{
    if (!tACDAvenger.IsAvenger())
        return;

    RemoveShadowClones(tACDAvenger.ptAvengerData->arVictimIDs);

    NavPlace tNavPlace;
    tNavPlace.tPlace.wp = tACDAvenger.GetPlace().wp;
    tNavPlace.tPlace.idSWorld = tACDAvenger.GetPlace().idSWorld;
    SNO snoPortal = GlobalSNOGet(GLOBALSNO_ACTOR_CONSOLE_AVENGER_EXIT_PORTAL);
    ActorAddStruct tActorAdd;
    tActorAdd.dwActorAddFlags = 0xd0;
    tActorAdd.tPlace = tNavPlace.tPlace;
    tActorAdd.eTeam = tACDAvenger.GetTeam();
    tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
    tActorAdd.tSNOName.snoHandle = snoPortal;
    tActorAdd.snoActor = snoPortal;
    ScenesCommonNavMeshNodeFromPlace(tNavPlace.tPlace, &tNavPlace.tNavMeshHandle, FALSE, FALSE);
    ActorID idPortal = ActorAdd(tActorAdd);
    Actor * ptActorPortal = ActorTryToGet(idPortal);
    if (ptActorPortal)
    {
        ActorCommonData * ptACDPortal = ACDTryToGet(ptActorPortal->idACD);
        if (!ptACDPortal)
            return;

        ptACDPortal->SetInvulnerable(TRUE);
        ActorDeleteSoon((ActorID)ACDGetOwner(ptActorPortal->idACD), 360);
    }

    FastAttribSetValueInt(tACDAvenger.idFastAttrib, ATTRIBUTE_POWER_IMMOBILIZED, 1);
    FastAttribSetValueInt(tACDAvenger.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, 1);
    ActorDeleteSoon((ActorID)ACDGetOwner(tACDAvenger.id), 122);
}









int32 Avenger::Console::GetMaxPlayerKills()
{
    return 5;
}



void Avenger::Console::RemoveShadowClones(blz::vector<ActorID, blz::allocator<ActorID> > & tShadowClones)
{
    SNO snoShadowCloneSpawnEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_SHADOWCLONE_SPAWN);
    for (uint32 nIndex = 0; nIndex < tShadowClones.size(); nIndex++)
    {
        ActorID idActor = tShadowClones[nIndex];
        if (idActor != ACTORID_INVALID)
        {
            Actor * ptActor = ActorTryToGet(idActor);
            if (ptActor)
            {
                ACDPlayEffect(ptActor->idACD, EFFECT_PLAY_EFFECT_GROUP, snoShadowCloneSpawnEffect, ACDID_INVALID);
                ActorDeleteSoon(idActor, 30);
            }
        }
    }
}



void sPrefetchShadowCloneItems(PlayerAvengerData * ptPlayerAvengerData)
{
    PrefetchActorDataMessage tMessage;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    for (int32 i = 0; i < 8; i++)
        tMessage.gbidMonsterAffixes[i] = GBID_NONE;

    for (uint32 nIndex = 0; nIndex < ptPlayerAvengerData->arVictims.size(); nIndex++)
    {
        GlobalSNO eGlobalSNOClone = Avenger::Console::GetGlobalSNOActorForShadowCloneVictim(ptPlayerAvengerData->arVictims[nIndex]);
        if (eGlobalSNOClone != GLOBALSNO_INVALID)
        {
            SNO snoActor = GlobalSNOGet(eGlobalSNOClone);
            ScopedActorDefinitionLockR pdefActor(snoActor);
            ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
            ScopedTreasureClassDefinitionLockR pdefTreasureClass(pdefMonster->snoInventory);
            for (int32 i = 0; i < pdefTreasureClass->nModifierCount; i++)
            {
                LootDropModifier & tModifier = pdefTreasureClass->ptModifierList[i];
                if (tModifier.eDropType == LDT_ITEM && tModifier.tSpecificItem.gbidItem != GBID_NONE)
                {
                    ScopedItemDefinitionLockR pdefItem(tModifier.tSpecificItem.gbidItem);
                    tMessage.snoActor = pdefItem->snoActor;
                    MessageSendToClient(listDestPlayers, 11, &tMessage, sizeof(tMessage));
                }
            }
        }
    }
}











void Avenger::Console::UpdateForPlayer(Player & tPlayer)
{
    PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource);
    if (!ptPlayerAvengerData || !ptPlayerAvengerData->bHasAvenger || ptPlayerAvengerData->eSpawnState == SPAWN_STATE_NONE)
        return;

    uint32 uNow = GameGetServerTime();
    if (uNow <= ptPlayerAvengerData->uPhaseAdvanceTime)
        return;

    SNO snoCeremonyEffect = SNO_NONE;
    switch (ptPlayerAvengerData->eSpawnState)
    {
    case SPAWN_STATE_PHASE_1:
        snoCeremonyEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_AVENGER_CEREMONY_01);
        ptPlayerAvengerData->uPhaseAdvanceTime = uNow + 540;
        ptPlayerAvengerData->eSpawnState = SPAWN_STATE_PHASE_2;
        break;
    case SPAWN_STATE_PHASE_2:
        snoCeremonyEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_AVENGER_CEREMONY_02);
        ptPlayerAvengerData->uPhaseAdvanceTime = uNow + 540;
        ptPlayerAvengerData->eSpawnState = SPAWN_STATE_PHASE_3;
        sPrefetchShadowCloneItems(ptPlayerAvengerData);
        break;
    case SPAWN_STATE_PHASE_3:
        snoCeremonyEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_AVENGER_CEREMONY_03);
        ptPlayerAvengerData->uPhaseAdvanceTime = uNow + 120;
        ptPlayerAvengerData->eSpawnState = SPAWN_STATE_SPAWN;
        break;
    case SPAWN_STATE_SPAWN:
        if (CanAvengerSpawnForPlayer(tPlayer))
            sSpawnAvenger(ptPlayerAvengerData, tPlayer);
        ptPlayerAvengerData->eSpawnState = SPAWN_STATE_NONE;
        ptPlayerAvengerData->uPhaseAdvanceTime = 0;
        break;
    default:
        break;
    }

    if (snoCeremonyEffect != SNO_NONE)
        ACDPlayEffect(tPlayer.idPlayerACD, EFFECT_PLAY_EFFECT_GROUP, snoCeremonyEffect, ACDID_INVALID);
}












// NON_MATCHING: unrolled compare chain uses a 64-bit shifted size instead of a 32-bit load (264 vs 284 bytes)
void Avenger::Console::VictimsCopyArrayToMessage(
    const AvengerVictimArray & arVictims,
    AvengerInfoMessage & tMessage)
{



    int32 nIndexNoVictims = arVictims.size();
    for (int32 nIndex = 0; nIndex < GetMaxPlayerKills(); nIndex++)
    {
        if (nIndex >= nIndexNoVictims)
        {
            tMessage.szVictims[nIndex * 2] = CHAR_MESSAGE_NO_MORE_VICTIMS;
            tMessage.szVictims[nIndex * 2 + 1] = CHAR_MESSAGE_NO_MORE_VICTIMS;
        }
        else
        {
            tMessage.szVictims[nIndex * 2] = arVictims[nIndex].eClass + 1;
            tMessage.szVictims[nIndex * 2 + 1] = arVictims[nIndex].bIsFemale + 1;
        }
    }



    // qualified call: the store's row is 969:67, the column of `=` with this spelling
    tMessage.szVictims[Avenger::Console::GetMaxPlayerKills() * 2] = '\0';
}


void Avenger::Console::VictimsCopyMessageToArray(const AvengerInfoMessage & tMessage, AvengerVictimArray & arVictims)
{
    arVictims.clear();
    AvengerVictim tVictim;
    for (int32 nIndex = 0; nIndex < GetMaxPlayerKills() * 2; nIndex++)
    {
        if (nIndex % 2 == 0)
        {
            if (tMessage.szVictims[nIndex] == CHAR_MESSAGE_NO_MORE_VICTIMS)
                break;
            tVictim.eClass = tMessage.szVictims[nIndex] - 1;
        }
        else
        {
            tVictim.bIsFemale = tMessage.szVictims[nIndex] - 1;
            arVictims.push_back(tVictim);
        }
    }
}









void Avenger::Console::VictimsCopyArrayToArray(const AvengerVictimArray & arVictimsSource, AvengerVictimArray & arVictimsDestination)
{
    arVictimsDestination.clear();
    for (int32 nIndex = 0; nIndex < arVictimsSource.size(); nIndex++)
        arVictimsDestination.push_back(arVictimsSource[nIndex]);
}






void Avenger::Console::VictimsCopyProtobufToArray(const google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> & arProtobuf, AvengerVictimArray & arVictims)
{
    arVictims.clear();
    AvengerVictim tVictim;
    for (int32 i = 0; i < arProtobuf.size(); i++)
    {
        const D3::Account::AvengerVictim & tAccountVictim = arProtobuf.Get(i);
        tVictim.eClass = PlayerClassFromGBID(tAccountVictim.gbid_class());
        tVictim.bIsFemale = tAccountVictim.is_female();
        arVictims.push_back(tVictim);
    }
}




void Avenger::Console::VictimsCopyArrayToProtobuf(const AvengerVictimArray & arVictims, google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> * arProtobuf)
{
    arProtobuf->Clear();
    for (uint32 nIndex = 0; nIndex < arVictims.size(); nIndex++)
    {
        D3::Account::AvengerVictim & tAccountVictim = *arProtobuf->Add();
        const AvengerVictim & tPlayerVictim = arVictims[nIndex];
        tAccountVictim.set_gbid_class(PlayerClassToGBID(tPlayerVictim.eClass));
        tAccountVictim.set_is_female(tPlayerVictim.bIsFemale != FALSE);
    }
}




void Avenger::Console::VictimsCopyProtobufToProtobuf(const google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> & arProtobufSource, google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> * arProtobufDestination)
{
    arProtobufDestination->Clear();
    for (int32 nIndex = 0; nIndex < arProtobufSource.size(); nIndex++)
    {
        D3::Account::AvengerVictim & tAccountVictim = *arProtobufDestination->Add();
        tAccountVictim.CopyFrom(arProtobufSource.Get(nIndex));
    }
}




GlobalSNO Avenger::Console::GetGlobalSNOActorForShadowCloneVictim(AvengerVictim & tVictim)
{
    switch (tVictim.eClass)
    {
    case PLAYER_CLASS_DEMONHUNTER:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_DEMONHUNTER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_DEMONHUNTER_MALE;
    case PLAYER_CLASS_BARBARIAN:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_BARBARIAN_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_BARBARIAN_MALE;
    case PLAYER_CLASS_WIZARD:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_WIZARD_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_WIZARD_MALE;
    case PLAYER_CLASS_WITCHDOCTOR:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_WITCHDOCTOR_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_WITCHDOCTOR_MALE;
    case PLAYER_CLASS_MONK:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_MONK_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_MONK_MALE;
    case PLAYER_CLASS_CRUSADER:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_CRUSADER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_CRUSADER_MALE;
    case PLAYER_CLASS_NECROMANCER:
        return tVictim.bIsFemale ? GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_NECROMANCER_FEMALE : GLOBALSNO_ACTOR_CONSOLE_AVENGER_CLONE_NECROMANCER_MALE;
    }
    return GLOBALSNO_INVALID;
}


static const LPCSTR sgszAvengerClassWindowNames[] = {
    ".DemonHunter_Female",
    ".DemonHunter_Male",
    ".Barbarian_Female",
    ".Barbarian_Male",
    ".Wizard_Female",
    ".Wizard_Male",
    ".WitchDoctor_Female",
    ".WitchDoctor_Male",
    ".Monk_Female",
    ".Monk_Male",
    ".Crusader_Female",
    ".Crusader_Male",
    ".Necromancer_Female",
    ".Necromancer_Male",
};


void Avenger::Console::ShowOnly(const UIID & idParent, const int32 nIndexToShow)
{
    for (int32 i = 0; i < (int32)(sizeof(sgszAvengerClassWindowNames) / sizeof(sgszAvengerClassWindowNames[0])); i++)
    {
        UIID idEntry = UIIDCreateFromParent(idParent, sgszAvengerClassWindowNames[i]);
        UI_GetWindowFromID(idEntry).SetActive(i == nIndexToShow);
    }
}




void Avenger::Console::InitializeForGame()
{
    ::Console::Online::GetNewMailEvent().Subscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnEventNewMail>());
}



void Avenger::Console::DisposeForGame()
{
    ::Console::Online::GetNewMailEvent().Unsubscribe(OnlineService::internal::function_bind_0_1<void, int32, &OnEventNewMail>());
}
