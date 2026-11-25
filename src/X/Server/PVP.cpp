// Original file: X/Server/PVP.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "PVP.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Player.h"
#include "../Common/GlobalSNO.h"
#include "../Common/GameTemplate.h"
#include "../Common/GameCommonData.h"
#include "../Common/Shell/CmdLine.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "Powers/PowerAPI.h"
#include "Powers/Powers.h"
#include "SPlayer.h"
#include "AI/ActorAI.h"
#include "AI/BrainComponents.h"
#include "SGame.h"
#include "SGameManager.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/GameConstants.h"
#include "../Common/Game.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/ActorCommonDataSwig.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../Sigma/DataStructures/ChainedDataArrayTemplated.h"

// Declared here from their definitions elsewhere (headers not adopted yet).
void ActorBoostPvP(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits);  // X/Server/ActorScript.cpp:78
enum { PVP_GAME_STATE_UNSTARTED = 0, PVP_GAME_STATE_PREGAME = 1, PVP_GAME_STATE_GAME = 2, PVP_GAME_STATE_GAME_OVER = 3 };  // X/Common/GameCommonData.h
typedef int32 PVPGameState;
PVPGameState GameGetPVPGameState();  // X/Common/GameCommonData.cpp:528
void SPlayerRecalculateUpscalingLevels();  // X/Server/SPlayer.cpp:13633
void NPCDismiss(ActorCommonData & tACDPlayer, BOOL bPlayerDismissed, BOOL bDisplayChatMessage);  // X/Server/SNPC.cpp:774
void BrainAPIDoResurrect(ActorID idActor, BOOL bForce);  // X/Server/AI/BrainComponents.cpp:7464
ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // X/Common/Player.cpp:2589
ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos);  // X/Common/Player.cpp:2600
#include "../Common/Networking/GameMessage.h"
uint32 TeamGetNumPlayers(Team eTeam);  // X/Common/Team.cpp:323
void PVPTeamDeathmatchPregameStart(float flCountDown);
ACDID SWorldGetPortalDestinationACD(const SWorldID idSWorld, const uint32 uEntranceGUID);  // X/Server/SWorld.cpp:1375
BOOL SPlayerFindGoodSpawnLocation(const WorldPlace & placeStart, WorldPlace & placeResult, float flSearchRadius);  // X/Server/SPlayer.cpp:2289
BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius);  // X/Server/AI/ActorAI.cpp:1802
void sPregameBuffApply(ACDID idACDPlayer);
  // X/Common/Networking/GameMessage.h

void sEnrageBuffUpdateDOT(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDVictim);
#include "Actors/ActorPowers.h"
#include "../Common/Powers/PowersCommon.h"
void ActorPowersRemoveBuffsForDueling(ActorCommonData & tACD, const SNO snoPower);  // X/Server/Actors/ActorPowers.cpp:723
void SPlayerKillPetsFromPowerSNO(const ACDID idACDPlayer, const SNO snoPower, const BOOL bEnteringRift);  // X/Server/SPlayer.cpp:3243
void ActorPowersInterruptPrimary(ActorID idActor, BOOL bIgnoreContactFrame);  // X/Server/Actors/ActorPowers.cpp:392
uint32 PVPGetPlayerEntranceGUID(const Player & tPlayer, SWorldID idSWorld);  // X/Server/PVP.cpp:917
BOOL SPlayerGetStartLocation(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID, WorldPlace & tPlace, Quaternion & q);  // X/Server/SPlayer.cpp:7938
void BrainStanceDoResurrect(ActorID idActor);  // X/Server/AI/BrainStance.cpp:2910

void PVPUpdate()
{

    uint32 dwNow = GameGetServerTime();
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    ScopedGlobalDefinitionLockR pdefGlobal;


    PVPGameState eGameState = GameGetPVPGameState();

    if (eGameState == PVP_GAME_STATE_UNSTARTED)
    {

        float flPregameDuration = pdefGlobal->ptServerData->flPVPPregameDuration;
        PVPTeamDeathmatchPregameStart(flPregameDuration);
        return;
    }
    if (eGameState == PVP_GAME_STATE_PREGAME && !tGCD.tGameSyncedData.dwRoundStart)
    {

        if (tGCD.tGameSyncedData.dwPregameEnd <= dwNow)
        {


            float flGameDuration = pdefGlobal->ptServerData->flPVPGameDuration;
            PvPArenaGameStartNewRound(Math::FloatToInt(flGameDuration * TICKS_PER_SECOND));
        }
        return;
    }




    if (!tGCD.tGameSyncedData.bPVPGameOver &&
        (tGCD.tGameSyncedData.dwRoundEnd <= dwNow ||
         TeamGetNumPlayers(TEAM_PLAYER_0) == 0 || TeamGetNumPlayers(TEAM_PLAYER_1) == 0))
    {
        PvPTeamDeathmatchGameEnd();
        return;
    }





    const uint32 MAX_POINTS = 300;

    if (tGCD.tGameSyncedData.nTeamScore[0] >= MAX_POINTS ||
        tGCD.tGameSyncedData.nTeamScore[1] >= MAX_POINTS)
    {
        PvPTeamDeathmatchGameEnd();
    }
}

#include "../Common/Movement/ActorMovement.h"
#include "LuaScript/LuaScriptUtil.h"
#include "../Common/OnlineService/OnlineService.h"
BOOL BrainAPIResurrectToCheckpoint(ACDID idACD);  // X/Server/AI/BrainComponents.cpp:3134
void ServerMessageSendAcceptedDeathAck(const ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp:1652
void SPlayerGhostedBuffRemove(ActorCommonData & tACDPlayer);  // X/Server/SPlayer.cpp:8364
lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript);  // X/Server/LuaScript/LuaScript.cpp:3353
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);  // X/Server/LuaScript/LuaScriptUtil.cpp:1084
#include "SGameConstants.h"
// X/Server/OnlineService/OnlineServiceServerPersist.h:273 (no src header yet)
struct PvpPlayerResult {  // size 0x68
    OnlineService::GameAccountHandle tAccountId;  // 0x00
    OnlineService::HeroId uHeroId;  // 0x10
    uint32 gbidClass;  // 0x18
    uint64 nTeamId;  // 0x20
    int eTeam;  // 0x28
    float teamRating;  // 0x2c
    float teamRatingVariance;  // 0x30
    float individualRating;  // 0x34
    float individualRatingVariance;  // 0x38
    bool win;  // 0x3c
    uint32 arena;  // 0x40
    uint32 score;  // 0x44
    int numTakedowns;  // 0x48
    int nDamage;  // 0x4c
    int nRank;  // 0x50
    int nTitle;  // 0x54
    int nBadge;  // 0x58
    int nGlory;  // 0x5c
    bool isFemale;  // 0x60
};
static_assert(sizeof(PvpPlayerResult) == 0x68, "PvpPlayerResult");
typedef blz::vector<PvpPlayerResult, blz::allocator<PvpPlayerResult> > PvpPlayerResultVec;  // OnlineServiceServerPersist.h:295
#include "SWorld.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "PlayerList.h"
#include "Crafting/SItemCrafting.h"
void ActorsBrainsToggle(const BOOL fEnabled);  // X/Server/Actors/Actors.cpp:3551
void SGameStatsClearForPlayerDeath(Player & tPlayer);  // X/Server/SGameStats.cpp:1756
void STrickleTriggerUpdate();  // X/Server/Network/STrickle.cpp:129
void ServerMessageSendKillCountMsg(const ACDID idACDPlayer);  // X/Server/Network/ServerMessageSend.cpp:1624
SNO SPlayerGetActiveSkillInSlot(const Player & tPlayer, const int32 nSlot);  // X/Server/SPlayer.cpp:5878
SNO PlayerGetPowerInTraitSlot(const Player & tPlayer, const int32 nSlot);  // X/Common/Player.cpp:3740
#include "SGameStats.h"
#include "LuaScript/ScriptUtil.h"
#include "../Sigma/MemoryPools.h"
#include "Achievements/AchievementModifierEvaluationLogic.h"
void PlayerListGetAllies(PlayerList & listPlayers, const ACDID idACDPlayerAlliesOf, const ACDID idACDPlayerIgnore);  // X/Server/PlayerList.cpp:615
BOOL PowerGetBuffAttributes(int32 nBuffIndex, Attrib & eAttribStart, Attrib & eAttribEnd, Attrib & eAttribCount);  // X/Common/Powers/PowersCommon.cpp:5977
typedef XPooledList<uint32, uint32> XPooledDWORDList;  // X/Common/Attributes/FastAttribute.h:14 (no src header declares it yet)
void FastAttribGetAllWithAttrib(AttribGroupID idFastAttrib, Attrib nAttrib, XPooledDWORDList & listParams);  // X/Common/Attributes/FastAttribute.cpp:3317
#include "../Common/Achievements/AchievementDefines.h"









































void sEnrageBuffUpdateDOT(PowerAPIBuffInstance * ptBuff, void * pData, PowerAPIDamage & tDamage, float flHitpointDamage, ACDID idACDAttacker, ACDID idACDVictim)
{









    ActorCommonData * ptACD = ACDTryToGet(idACDVictim);
    if (!ptACD)
        return;



    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_PVP_ROUNDEND_BUFF);
    if (tDamage.snoAltDamageSourcePower == snoBuff)
    {
        uint32 dwMinutesSinceApplication = (GameGetServerTime() - ptBuff->dwStartTick) / (TICKS_PER_SECOND * 60);

        ptBuff->flDamagePerSecond = ptACD->GetHitPointsMaxTotal() * 0.01f * (dwMinutesSinceApplication + 1);
    }
}

void sEnrageBuffApply(PowerAPIBuffInstance * ptBuff, ACDID idACDTarget, void *)
{


    ActorCommonData * ptACD = ACDTryToGet(idACDTarget);
    if (!ptACD)
        return;




    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_PVP_ROUNDEND_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.idACDSource = ACDID_INVALID;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.bPositiveEffect = FALSE;
    tBuffStartData.nBuffIndex = 1;
    tBuffStartData.eDamageType = 0;
    tBuffStartData.flDamagePerSecond = ptACD->GetHitPointsMaxTotal() * 0.01f;
    tBuffStartData.pOnGetHitEffect = sEnrageBuffUpdateDOT;
    tBuffStartData.snoDOTDamageSourcePower = snoBuff;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}

void sEnrageTimerBuffApply(ACDID idACDPlayer, uint32 dwRoundDuration)
{

    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (!ptACD)
        return;




    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_PVP_ROUNDEND_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.dwDurationMin = dwRoundDuration;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.idACDSource = ACDID_INVALID;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bPositiveEffect = FALSE;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.pExpirationEffect = sEnrageBuffApply;
    tBuffStartData.nBuffIndex = 0;
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}

void sGameEndBuffApply(ACDID idACDPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (!ptACD)
        return;





    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_PVP_ROUNDEND_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.dwDurationMin = 0;
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.idACDSource = ACDID_INVALID;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.nBuffIndex = 0;
    tBuffStartData.bImmobilize = TRUE;
    tBuffStartData.bFailsIfImmobilizeImmune = FALSE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}




void sPregameBuffApply(ACDID idACDPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(idACDPlayer);
    if (!ptACD)
        return;





    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_PVP_SKIRMISH_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.idACDSource = idACDPlayer;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.bPositiveEffect = TRUE;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_INVULNERABLE, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_CANT_START_DISPLAYED_POWERS, TRUE);
    PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
}

void PVPTeamDeathmatchPregameStart(float flCountDown)
{

    uint32 dwPregameLengthTicks = (uint32)(flCountDown * TICKS_PER_SECOND);
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.tGameSyncedData.bPregame = TRUE;
    tGCD.tGameSyncedData.dwPregameEnd = GameGetServerTime() + dwPregameLengthTicks;

    Player * ptPlayer = PlayerGetFirstInGame();
    while (ptPlayer)
    {
        sPregameBuffApply(ptPlayer->idPlayerACD);
        SGameSyncData(ptPlayer);
        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }
}



void PVPOnPlayerEnteredGame(Player * ptPlayer)
{

    if (GameTypeIsPvP())
    {

        ActorCommonData & tACD = ACDGet(ptPlayer->idPlayerACD);

        if (CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
        {
            ActorBoostPvP(tACD, 60, "ArenaPremade", "PvP Balanced", 3);
        }







        tACD.ResetLifeStats(TRUE, FALSE, TRUE, TRUE, FALSE);

        if (GameGetPVPGameState() == PVP_GAME_STATE_PREGAME)
        {
            sPregameBuffApply(ptPlayer->idPlayerACD);
        }
    }
}




















static void sPVPDetermineGameResult()
{

    if (!GameTypeIsPvP())
        return;



    if (!GameCommonDataExists())
        return;



    GameCommonData & tGCD = GameGetActiveGameCommonData();

    for (int32 i = 0; i < 2; i++)
    {
        tGCD.tGameSyncedData.ePVPGameResult[i] = PVP_GR_INVALID;
    }



    uint32 nPlayersOnTeam0 = TeamGetNumPlayers(TEAM_PLAYER_0);
    uint32 nPlayersOnTeam1 = TeamGetNumPlayers(TEAM_PLAYER_1);

    if (nPlayersOnTeam0 == 0 && nPlayersOnTeam1 == 0)
    {
        tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_TIE;
        tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_TIE;
    }

    else if (nPlayersOnTeam0 == 0)
    {
        tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_LOSS;
        tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_WIN;
    }
    else if (nPlayersOnTeam1 == 0)
    {
        tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_WIN;
        tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_LOSS;
    }
    else
    {



        if (tGCD.tGameSyncedData.nTeamScore[0] > tGCD.tGameSyncedData.nTeamScore[1])
        {
            tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_WIN;
            tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_LOSS;
        }
        else if (tGCD.tGameSyncedData.nTeamScore[0] < tGCD.tGameSyncedData.nTeamScore[1])
        {
            tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_LOSS;
            tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_WIN;
        }
        else
        {
            tGCD.tGameSyncedData.ePVPGameResult[0] = PVP_GR_TIE;
            tGCD.tGameSyncedData.ePVPGameResult[1] = PVP_GR_TIE;
        }
    }
}
void PvPTeamDeathmatchGameEnd()

{
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    if (tGCD.tGameSyncedData.bPVPGameOver)
        return;





    tGCD.tGameSyncedData.bPVPGameOver = TRUE;


    SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT);


    sPVPDetermineGameResult();
















    PvpPlayerResultVec tResults;
    tResults.resize(PlayerGetNumPlayers());






    Player * ptPlayer = PlayerGetFirstInGame();
    while (ptPlayer)
    {
        ActorCommonData &tACDPlayer = ACDGet(ptPlayer->idPlayerACD);




        if (!tACDPlayer.IsAlive())
        {
            BrainAPIResurrectToCheckpoint(tACDPlayer.id);
            ServerMessageSendAcceptedDeathAck(tACDPlayer.id);
        }




        SPlayerGhostedBuffRemove(tACDPlayer);



        ActorMovement *ptActorMovement = tACDPlayer.GetActorMovement();

        if (!(ptActorMovement->m_uMoveFlags & (1 << MOVEBIT_FROM_POWER)) && !ptActorMovement->IsInArcingMovement())
        {
            ptActorMovement->StopNow();
        }


        sGameEndBuffApply(ptPlayer->idPlayerACD);



        SGameSyncData(ptPlayer);
















































































































        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }

    SNO snoScript = GlobalSNOGet(GLOBALSNO_SCRIPT_PVPARENA);

    if (snoScript != SNO_NONE)
    {
        lua_State *L = LuaScriptGetLuaStateWithSNO(snoScript);


        if (LuaScriptGetFunction(L, "AssignRewards"))
        {
            LuaSNOScriptCall(snoScript, L, 0, 0, "AssignRewards");
        }




        if (LuaScriptGetFunction(L, "EvaluateEndOfGameAccolades"))
        {
            LuaSNOScriptCall(snoScript, L, 0, 0, "EvaluateEndOfGameAccolades");
        }
    }
}



































































void PvPArenaGameStartNewRound(
    uint32 dwRoundDuration)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    if (GameGetPVPGameState() == PVP_GAME_STATE_PREGAME)
    {
        tGCD.tGameSyncedData.bPregame = FALSE;
    }




    tGCD.tGameSyncedData.nTeamScore[0] = 0;
    tGCD.tGameSyncedData.nTeamScore[1] = 0;  // no row of its own: merged into line 759's 8-byte store

    uint32 dwNow = GameGetServerTime();
    tGCD.tGameSyncedData.dwRoundStart = dwNow;
    tGCD.tGameSyncedData.dwRoundEnd = dwNow + dwRoundDuration;

    Player * ptPlayer = PlayerGetFirstInGame();

    while (ptPlayer)
    {
        ActorCommonData &tACDPlayer = ACDGet(ptPlayer->idPlayerACD);





        tACDPlayer.SetObserver(FALSE);
        tACDPlayer.Hide(FALSE);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_RESURRECT_AS_OBSERVER, FALSE);
        PowerAPI_BuffFlagAllForDeletion(tACDPlayer);

        SPlayerKillAllPets(*ptPlayer, TRUE, TRUE, TRUE, FALSE);

        SGameStatsClearForPlayerDeath(*ptPlayer);

        ServerMessageSendAcceptedDeathAck(ptPlayer->idPlayerACD);
        BrainAPIResurrectToCheckpoint(ptPlayer->idPlayerACD);
        tACDPlayer.ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
        tACDPlayer.MarkHeroStateChanged();
        ACDInventoryEquipmentOrSkillChanged(tACDPlayer);


        int32 nPrevGamesPlayed = tACDPlayer.AttributesGetInt(ATTRIBUTE_PVP_TEAMDEATHMATCH_GAMES_PLAYED);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_TEAMDEATHMATCH_GAMES_PLAYED, nPrevGamesPlayed + 1);


        ptPlayer->nPlayerKills = 0;
        ptPlayer->nDeaths = 0;  // no row of its own: merged into line 796's 8-byte store (nDeaths at 0x64)
        ptPlayer->nAssists = 0;
        ServerMessageSendKillCountMsg(ptPlayer->idPlayerACD);

        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_KILLS, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_DEATHS, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_ASSISTS, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_PROGRESSION_POINTS_GAINED, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_KILL_STREAK, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_DEATH_STREAK, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_LONGEST_KILL_STREAK, 0);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_PVP_LONGEST_DEATH_STREAK, 0);

        PowerAPI_ResetCooldowns(tACDPlayer);


        SNO snoPowerLevelEqualizer = GlobalSNOGet(GLOBALSNO_POWERS_PVP_LEVEL_EQUALIZER_BUFF);
        Target tTarget;
        tTarget.InitializeFromPlace(tACDPlayer.GetPlace());
        PowerStart(tACDPlayer.id, snoPowerLevelEqualizer, tTarget);

        SGameSyncData(ptPlayer);


        if (GameGetGameType() == GT_PVP_ARENA)
        {
            sEnrageTimerBuffApply(ptPlayer->idPlayerACD, dwRoundDuration);
        }

        ptPlayer = PlayerGetNextInGame(ptPlayer);
    }
































    ActorsBrainsToggle(TRUE);
    SGameGlobalsGet().dwFlags &= ~(1 << SGAMEGLOBALS_PVP_GAME_OVER_BIT);


    SNO snoScript = GlobalSNOGet(GLOBALSNO_SCRIPT_PVPARENA);
    if (snoScript != SNO_NONE)
    {
        lua_State *L = LuaScriptGetLuaStateWithSNO(snoScript);


        if (LuaScriptGetFunction(L, "SetupRatings"))
        {
            LuaSNOScriptCall(snoScript, L, 0, 0, "SetupRatings");
        }
    }

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    SimpleMessage tMessage;
    SigmaMemoryClear(&tMessage, sizeof(tMessage));
    MessageSendToClient(listDestPlayers, SMSG_PVP_ARENA_RESTART, &tMessage, sizeof(tMessage));
    STrickleTriggerUpdate();
}




static void sOnPlayerDeathSetRespawnTime(
    ACDID idACDVictim,
    int32 nNumDeaths)
{


    uint32 dwNow = GameGetServerTime();
    if (GameIsParamFlagSet(GPF_RESPAWN_IN_WAVES))
    {
        // DWARF folds this local to 720 (= 12 seconds of ticks); the spelling is inferred
        const uint32 cdwRespawnTime = 12 * TICKS_PER_SECOND;
        uint32 dwNextRespawn = dwNow - (dwNow % cdwRespawnTime) + cdwRespawnTime;

        ActorCommonData &tACDVictim = ACDGet(idACDVictim);
        tACDVictim.AttributesSetInt(ATTRIBUTE_RESPAWN_GAMETIME, dwNextRespawn);
    }
    else
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        // fcvtzs at column 46 (the paren after a 5-letter cast): a signed (int32) cast

        const uint32 cdwRespawnTime = (int32)(pdefGlobal->ptServerData->flPVPPlayerResurrectionDelay * TICKS_PER_SECOND);

        ActorCommonData &tACDVictim = ACDGet(idACDVictim);
        tACDVictim.AttributesSetInt(ATTRIBUTE_RESPAWN_GAMETIME, dwNow + cdwRespawnTime);
    }
}



uint32 PVPGetPlayerEntranceGUID(
    const Player & tPlayer,
    SWorldID idSWorld)
{
    BOOL bInitialRoundSpawn = FALSE;
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    if (tGCD.tGameSyncedData.dwRoundStart == GameGetServerTime() ||
        tGCD.tGameSyncedData.bPregame)
    {
        bInitialRoundSpawn = TRUE;
    }

    uint32 uEntranceGUID = (uint32)-1;

    const int32 nMaxPVPEntrances = 32;
    uint32 arPossibleGUID[nMaxPVPEntrances];
    int32 nPossibleGUID = 0;

    SWorld *ptSWorld = SWorldGet(idSWorld);
    POS pos = ptSWorld->tEntranceMap.GetStartAssoc();
    while (pos)
    {
        uint32 uPortalGUID;
        EntranceLocation tEntranceLocation;
        ptSWorld->tEntranceMap.GetNextAssoc(pos, uPortalGUID, tEntranceLocation);
        ACDID idACDPortal = SWorldGetPortalDestinationACD(ptSWorld->id, uPortalGUID);
        ActorCommonData *ptACDPortal = ACDTryToGet(idACDPortal);
        if (ptACDPortal)
        {
            // NOTE: the original rows put the eTeam compare at 961:21 (one level deeper); the bytes match this layout
            if (GameIsParamFlagSet(GPF_USE_RANDOM_RESPAWN_LOCATION))
            {
                // the bInitialRoundSpawn tests leave no rows: the loop is unswitched on it (line 938)
                if (!bInitialRoundSpawn && ptACDPortal->GetTeam() == TEAM_NEUTRAL)
                {
                    arPossibleGUID[nPossibleGUID] = uPortalGUID;
                    nPossibleGUID++;
                }
                else if (bInitialRoundSpawn && ptACDPortal->GetTeam() == tPlayer.eTeam)
                {
                    arPossibleGUID[nPossibleGUID] = uPortalGUID;
                    nPossibleGUID++;
                }
            }
            else if (ptACDPortal->GetTeam() == tPlayer.eTeam)
            {
                arPossibleGUID[nPossibleGUID] = uPortalGUID;
                nPossibleGUID++;
            }

            if (nPossibleGUID >= nMaxPVPEntrances)
                break;
        }
    }




    if (nPossibleGUID == 0)
        return uEntranceGUID;



    if (bInitialRoundSpawn)
    {
        int32 nTeamMemberIndex = 0;
        PlayerList listPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

        POS posPlayer = listPlayers.GetHeadPosition();
        while (posPlayer)
        {
            Player *ptPlayerToCheck = listPlayers.GetNext(posPlayer);

            if (ptPlayerToCheck == &tPlayer)
                break;



            if (ptPlayerToCheck->eTeam == tPlayer.eTeam)
                nTeamMemberIndex++;
        }




        uEntranceGUID = arPossibleGUID[nTeamMemberIndex % nPossibleGUID];
    }
    else
    {
        if (GameIsParamFlagSet(GPF_USE_RANDOM_RESPAWN_LOCATION) || tPlayer.idPlayerACD == ACDID_INVALID)
        {
            // the original attributes SGameGlobalsGet to column 44 with RandRange_Internal: probably a macro src lacks
            uEntranceGUID = arPossibleGUID[RandRange_Internal(SGameGlobalsGet().rng, 0, nPossibleGUID - 1)];
        }
        else
        {

            ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);
            WorldDistance distanceSquaredClosest = 1e11f;

            for (int32 i = 0; i < nPossibleGUID; i++)
            {
                uint32 uPossibleGUID = arPossibleGUID[i];
                ACDID idACDPortal = SWorldGetPortalDestinationACD(ptSWorld->id, uPossibleGUID);
                ActorCommonData *ptACDPortal = ACDTryToGet(idACDPortal);
                if (ptACDPortal && ptACDPortal->GetSWorldID() == tACDPlayer.GetSWorldID())
                {
                    WorldDistance distanceSquared = WorldPlaceDistanceSquared(ptACDPortal->GetPlace(), tACDPlayer.GetPlace());
                    if (distanceSquared < distanceSquaredClosest)
                    {
                        distanceSquaredClosest = distanceSquared;
                        uEntranceGUID = uPossibleGUID;
                    }
                }
            }
        }
    }

    return uEntranceGUID;
}




void sOnPlayerDeathSpawnGhost(
    ACDID idACDPlayer)
{
    Player *ptPlayer = PlayerGetByACD(idACDPlayer);
    ActorCommonData &tACDExisting = ACDGet(ptPlayer->idPlayerACD);
    const ActorLocation &tLoc = tACDExisting.GetLocation();
    WorldPlace placeEntrance;
    Quaternion qEntrance;

    uint32 uEntranceGUID = PVPGetPlayerEntranceGUID(*ptPlayer, tLoc.tWorldLocation.idSWorld);

    BOOL bSuccess = SPlayerGetStartLocation(tLoc.tWorldLocation.idSWorld, tLoc.tWorldLocation.idSScene, tACDExisting.GetTeam(), uEntranceGUID, placeEntrance, qEntrance);

    if (!bSuccess)
        return;



    BrainStanceDoResurrect((ActorID)ACDGetOwner(ptPlayer->idPlayerACD));
    ActorCommonData &tACDPlayer = ACDGet(idACDPlayer);
    tACDPlayer.AttributesSetInt(ATTRIBUTE_RESURRECT_AS_OBSERVER, TRUE);
    tACDPlayer.Hide(FALSE);
}

























void PVPOnDeathPvPArena(
    ACDID idACDKiller,
    ACDID idACDVictim,
    SNO snoKillingPower)
{
    if (idACDVictim == ACDID_INVALID)
        return;




    if (!ACDIsPlayer(idACDVictim))
        return;



    ActorCommonData &tACDVictim = ACDGet(idACDVictim);

    Player *ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);


    Player *ptPlayerVictim = PlayerGetByACD(idACDVictim);
    if (ptPlayerVictim)
    {
        SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_DEATHS, 0, 0, 1.0f);

        SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_ALIVE_TIME, 0, 0, (float)(GameGetServerTime() - ptPlayerVictim->dwTickLastResurrection) / TICKS_PER_SECOND);

        ptPlayerVictim->nDeaths++;
        {





            PlayerList listDestPlayers( PlayerListGetElementPool() );
            PlayerListGetAllies(listDestPlayers, idACDVictim, idACDVictim);
            POS posPlayer = listDestPlayers.GetHeadPosition();
            while (posPlayer)
            {
                Player *ptPlayerAlly = listDestPlayers.GetNext(posPlayer);
                Achievement::EvaluationData tEvaluationDataAlly;
                tEvaluationDataAlly.SetPlayer(ptPlayerAlly);
                tEvaluationDataAlly.EvaluateEvent(ptPlayerAlly, Achievement::ACH_EVENT_PVP_TEAMMATE_DIED, 1, (uint32)sub_71007b5e70());



            }
        }

        GameCommonData & tGCD = GameGetActiveGameCommonData();

        if (tACDVictim.GetNaturalTeam() == TEAM_PLAYER_0)
        {
            PVPTeamAddScore(TEAM_PLAYER_1, 1, ptPlayerKiller ? ptPlayerKiller->idPlayerACD : ACDID_INVALID);
        }
        else
        {
            PVPTeamAddScore(TEAM_PLAYER_0, 1, ptPlayerKiller ? ptPlayerKiller->idPlayerACD : ACDID_INVALID);
        }


        SGameGlobals &tSGameGlobals = SGameGlobalsGet();

        if (tGCD.tGameSyncedData.nTeamScore[1] > tGCD.tGameSyncedData.nTeamScore[0])
        {
            int32 deficit = tGCD.tGameSyncedData.nTeamScore[1] - tGCD.tGameSyncedData.nTeamScore[0];
            if (deficit > tSGameGlobals.nLargestPVPScoreDeficitTeam0)
            {
                tSGameGlobals.nLargestPVPScoreDeficitTeam0 = deficit;
            }
        }
        else if (tGCD.tGameSyncedData.nTeamScore[0] > tGCD.tGameSyncedData.nTeamScore[1])
        {
            int32 deficit = tGCD.tGameSyncedData.nTeamScore[0] - tGCD.tGameSyncedData.nTeamScore[1];
            if (deficit > tSGameGlobals.nLargestPVPScoreDeficitTeam1)
            {
                tSGameGlobals.nLargestPVPScoreDeficitTeam1 = deficit;
            }
        }
        {

            Player *ptPlayer = PlayerGetFirstInGame();
            while (ptPlayer)
            {
                SGameSyncData(ptPlayer);
                ptPlayer = PlayerGetNextInGame(ptPlayer);
            }
        }

        ServerMessageSendKillCountMsg(idACDVictim);
        // the respawn-time helper (inlined at 1185) sits before the ghost spawn (1191), so the test is != GT_PVP_ARENA
        if (GameGetGameType() != GT_PVP_ARENA)
        {
            sOnPlayerDeathSetRespawnTime(idACDVictim, ptPlayerVictim->nDeaths);  // nNumDeaths is unused by the helper, so the argument is inferred
        }
        else
        {


            sOnPlayerDeathSpawnGhost(idACDVictim);
        }




        int32 nDeaths = 1 + tACDVictim.AttributesGetInt(ATTRIBUTE_PVP_DEATHS);
        int32 nCurrentDeathStreak = 1 + tACDVictim.AttributesGetInt(ATTRIBUTE_PVP_CURRENT_DEATH_STREAK);

        tACDVictim.AttributesSetInt(ATTRIBUTE_PVP_DEATHS, nDeaths);
        tACDVictim.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_DEATH_STREAK, nCurrentDeathStreak);

        if (nCurrentDeathStreak > tACDVictim.AttributesGetInt(ATTRIBUTE_PVP_LONGEST_DEATH_STREAK))
        {
            tACDVictim.AttributesSetInt(ATTRIBUTE_PVP_LONGEST_DEATH_STREAK, nCurrentDeathStreak);
        }


        tACDVictim.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_KILL_STREAK, 0);


        Player *ptPlayerFirstAssist = NULL;
        Player *ptPlayerSecondAssist = NULL;
        Player *ptPlayerThirdAssist = NULL;
        float flFirstAssistDamage = 0.0f;
        float flSecondAssistDamage = 0.0f;
        float flThirdAssistDamage = 0.0f;

        const float flAssistDamageThresholdPercent = 0.1f;
        float flPlayerMaxHealth = tACDVictim.GetHitPointsMaxTotal();

        for (int32 i = 0; i < MAX_PLAYERS; i++)
        {
            float flAssistDamage = ptPlayerVictim->flPVPPlayerDamageTaken[i];
            float flAssistDamageHealthPercent = flAssistDamage / flPlayerMaxHealth;



            Player *ptPlayerAssist = PlayerGetByPlayerIndex(i);
            if (ptPlayerAssist &&
                ACDTryToGet(ptPlayerAssist->idPlayerACD) &&
                ptPlayerAssist != ptPlayerKiller &&
                flAssistDamageHealthPercent > flAssistDamageThresholdPercent &&
                !TeamsAreFriendly(ptPlayerAssist->eTeam, ptPlayerVictim->eTeam))
            {
                if (flAssistDamage > flFirstAssistDamage)
                {
                    ptPlayerThirdAssist = ptPlayerSecondAssist;
                    flThirdAssistDamage = flSecondAssistDamage;
                    ptPlayerSecondAssist = ptPlayerFirstAssist;
                    flSecondAssistDamage = flFirstAssistDamage;
                    ptPlayerFirstAssist = ptPlayerAssist;
                    flFirstAssistDamage = flAssistDamage;
                }
                else if (flAssistDamage > flSecondAssistDamage)
                {
                    ptPlayerThirdAssist = ptPlayerSecondAssist;
                    flThirdAssistDamage = flSecondAssistDamage;
                    ptPlayerSecondAssist = ptPlayerAssist;
                    flSecondAssistDamage = flAssistDamage;
                }
                else if (flAssistDamage > flThirdAssistDamage)
                {
                    ptPlayerThirdAssist = ptPlayerAssist;
                    flThirdAssistDamage = flAssistDamage;
                }
            }
        }

        if (ptPlayerFirstAssist)
        {
            ActorCommonData &tACDPlayerFirstAssist = ACDGet(ptPlayerFirstAssist->idPlayerACD);
            int32 nAssists = 1 + tACDPlayerFirstAssist.AttributesGetInt(ATTRIBUTE_PVP_ASSISTS);
            tACDPlayerFirstAssist.AttributesSetInt(ATTRIBUTE_PVP_ASSISTS, nAssists);
            ptPlayerFirstAssist->nAssists++;
            ServerMessageSendKillCountMsg(ptPlayerFirstAssist->idPlayerACD);

            SGameStatsPlayerCounterIncrement(ptPlayerFirstAssist, STAT_COUNTER_PLAYER_ASSISTS, 0, 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerFirstAssist, STAT_COUNTER_PLAYER_TAKEDOWNS, 0, 0, 1.0f);

            PlayerList listDestPlayers( PlayerListGetElementPool() );
            PlayerListGetSingle( listDestPlayers, ptPlayerFirstAssist->idPlayerACD );
            PlayerIndexMessage tAssistMessage;
            tAssistMessage.nPlayerIndex = ptPlayerVictim->nPlayerIndex;
            MessageSendToClient(listDestPlayers, SMSG_PVP_ASSIST, &tAssistMessage, sizeof(tAssistMessage));
            {




                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.EvaluateEvent(ptPlayerFirstAssist, Achievement::ACH_EVENT_PVP_PLAYER_GOT_ASSIST, 1, (uint32)sub_71007b5e70());
            }
        }

        if (ptPlayerSecondAssist)
        {
            ActorCommonData &tACDPlayerSecondAssist = ACDGet(ptPlayerSecondAssist->idPlayerACD);
            int32 nAssists = 1 + tACDPlayerSecondAssist.AttributesGetInt(ATTRIBUTE_PVP_ASSISTS);
            tACDPlayerSecondAssist.AttributesSetInt(ATTRIBUTE_PVP_ASSISTS, nAssists);
            ptPlayerSecondAssist->nAssists++;
            ServerMessageSendKillCountMsg(ptPlayerSecondAssist->idPlayerACD);

            SGameStatsPlayerCounterIncrement(ptPlayerSecondAssist, STAT_COUNTER_PLAYER_ASSISTS, 0, 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerSecondAssist, STAT_COUNTER_PLAYER_TAKEDOWNS, 0, 0, 1.0f);

            PlayerList listDestPlayers( PlayerListGetElementPool() );
            PlayerListGetSingle( listDestPlayers, ptPlayerSecondAssist->idPlayerACD );
            PlayerIndexMessage tAssistMessage;
            tAssistMessage.nPlayerIndex = ptPlayerVictim->nPlayerIndex;
            MessageSendToClient(listDestPlayers, SMSG_PVP_ASSIST, &tAssistMessage, sizeof(tAssistMessage));
            {




                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.EvaluateEvent(ptPlayerSecondAssist, Achievement::ACH_EVENT_PVP_PLAYER_GOT_ASSIST, 1, (uint32)sub_71007b5e70());
            }
        }

        if (ptPlayerThirdAssist)
        {
            ActorCommonData &tACDPlayerThirdAssist = ACDGet(ptPlayerThirdAssist->idPlayerACD);
            int32 nAssists = 1 + tACDPlayerThirdAssist.AttributesGetInt(ATTRIBUTE_PVP_ASSISTS);
            tACDPlayerThirdAssist.AttributesSetInt(ATTRIBUTE_PVP_ASSISTS, nAssists);
            ptPlayerThirdAssist->nAssists++;
            ServerMessageSendKillCountMsg(ptPlayerThirdAssist->idPlayerACD);

            SGameStatsPlayerCounterIncrement(ptPlayerThirdAssist, STAT_COUNTER_PLAYER_ASSISTS, 0, 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerThirdAssist, STAT_COUNTER_PLAYER_TAKEDOWNS, 0, 0, 1.0f);

            PlayerList listDestPlayers( PlayerListGetElementPool() );
            PlayerListGetSingle( listDestPlayers, ptPlayerThirdAssist->idPlayerACD );
            PlayerIndexMessage tAssistMessage;
            tAssistMessage.nPlayerIndex = ptPlayerVictim->nPlayerIndex;
            MessageSendToClient(listDestPlayers, SMSG_PVP_ASSIST, &tAssistMessage, sizeof(tAssistMessage));
            {




                Achievement::EvaluationData tEvaluationData;
                tEvaluationData.EvaluateEvent(ptPlayerThirdAssist, Achievement::ACH_EVENT_PVP_PLAYER_GOT_ASSIST, 1, (uint32)sub_71007b5e70());
            }
        }


        for (int32 i = 0; i < MAX_PLAYERS; i++)
        {
            ptPlayerVictim->flPVPPlayerDamageTaken[i] = 0.0f;
        }
    }


    if (idACDKiller != ACDID_INVALID)
    {
        if (ptPlayerKiller)  // no row of its own (line 0, column 9); a nested if inferred because line 1350 is indented two levels
        {
            ptPlayerKiller->nPlayerKills++;
            ServerMessageSendKillCountMsg(ptPlayerKiller->idPlayerACD);

            if (ptPlayerVictim)
            {
                {



                    Achievement::EvaluationData tEvaluationData;
                    tEvaluationData.SetPlayer(ptPlayerKiller);

                    tEvaluationData.EvaluateEvent(ptPlayerKiller,
                        Achievement::ACH_EVENT_PVP_PLAYER_KILLED_PLAYER_OF_CLASS,

                        PlayerGetPlayerClass(ptPlayerVictim),
                        1, (uint32)sub_71007b5e70());
                }
            }


            ActorCommonData &tACDKiller = ACDGet(ptPlayerKiller->idPlayerACD);
            int32 nKills = 1 + tACDKiller.AttributesGetInt(ATTRIBUTE_PVP_KILLS);
            int32 nCurrentKillStreak = 1 + tACDKiller.AttributesGetInt(ATTRIBUTE_PVP_CURRENT_KILL_STREAK);

            tACDKiller.AttributesSetInt(ATTRIBUTE_PVP_KILLS, nKills);


            if (tACDKiller.IsAlive() && !tACDKiller.IsObserver())
            {
                tACDKiller.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_KILL_STREAK, nCurrentKillStreak);

                if (nCurrentKillStreak > tACDKiller.AttributesGetInt(ATTRIBUTE_PVP_LONGEST_KILL_STREAK))
                {
                    tACDKiller.AttributesSetInt(ATTRIBUTE_PVP_LONGEST_KILL_STREAK, nCurrentKillStreak);
                }
            }

            SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_PLAYERS_KILLED, 0, 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_PLAYERS_KILLED_OF_CLASS, PlayerGetPlayerClass(ptPlayerVictim), 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_TAKEDOWNS, 0, 0, 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_PLAYERS_KILLED_WITH_POWER_ALL_RUNES, snoKillingPower, 0, 1.0f);

            Attrib eAttribStart;
            Attrib eAttribEnd;
            Attrib eAttribCount;
            for (int32 i = 0; i < 8; i++)
            {
                PowerGetBuffAttributes(i, eAttribStart, eAttribEnd, eAttribCount);
                XPooledDWORDList listBuff(MemoryPoolsGetDWORDListPool());
                FastAttribGetAllWithAttrib(tACDKiller.idFastAttrib, eAttribCount, listBuff);
                POS pos = listBuff.GetHeadPosition();
                while (pos)
                {
                    SNO snoPower = (SNO)listBuff.GetNext(pos);
                    if (tACDKiller.AttributesGetInt(MakeAttribKey(eAttribCount, snoPower)) > 0)
                    {
                        SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_PLAYERS_KILLED_WITH_BUFF, snoPower, i, 1.0f);
                    }
                }
            }


            Target tTarget;
            tTarget.InitializeFromPlace(tACDVictim.GetPlace());
            PowerStart(idACDKiller, GlobalSNOGet(GLOBALSNO_POWERS_BANNER_DROP_PVP), tTarget);


            tACDKiller.AttributesSetInt(ATTRIBUTE_PVP_CURRENT_DEATH_STREAK, 0);

            PlayerList listDestPlayers( PlayerListGetElementPool() );
            PlayerListGetSingle( listDestPlayers, tACDKiller.id );
            PlayerIndexMessage tKillMessage;
            tKillMessage.nPlayerIndex = ptPlayerVictim->nPlayerIndex;
            MessageSendToClient(listDestPlayers, SMSG_PVP_KILL, &tKillMessage, sizeof(tKillMessage));
        }
    }


}



void PVPPlayerRemoveShrineBuffs(ActorCommonData & tACDPlayer)
{


    const int32 NUM_SHRINE_TYPES = 6;
    SNO arrShrinePowers[NUM_SHRINE_TYPES] = {
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Blessed"),
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Enlightened"),
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Fortune"),
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Frenzied"),
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Hoarder"),
        SNOFileLookupGUID(POWER_SNO, "Shrine_Desecrated_Reloaded"),
    };




    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    POS pos = tACDPlayer.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);
        PowerAPIBuffInstance * ptBuff = ptPowerGlobals->pArrayBuffs->ElementGet(idBuff);

        if (blz::find(arrShrinePowers, arrShrinePowers + NUM_SHRINE_TYPES, ptBuff->snoPower) != arrShrinePowers + NUM_SHRINE_TYPES)
        {
            Power * ptPower = PowerGet(ptBuff->idPower);
            if (ptPower && !ptPower->bFreeCast &&


                !ptBuff->bDeleted)
            {
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, FALSE, TRUE);
            }
        }
    }
}





static void sPVPPlayerRemoveBuffsAndPets(
    ActorCommonData & tACDPlayer,
    const SNO snoPower)
{
    if (snoPower != SNO_NONE)
    {
        ActorPowersRemoveBuffsForDueling(tACDPlayer, snoPower);
        SPlayerKillPetsFromPowerSNO(tACDPlayer.id, snoPower, TRUE);
    }

    ActorID idActor = (ActorID)ACDGetOwner(tACDPlayer.id);
    SNO snoCurrentPower = ActorPowersGetPrimaryPower(idActor);
    if (snoCurrentPower != SNO_NONE && snoCurrentPower == snoPower)
    {
        if (PowerIsChannelled(tACDPlayer, AutoPowerDefinitionR(snoPower)))
        {
            ActorPowersInterruptPrimary(idActor, TRUE);
        }
    }

    PVPPlayerRemoveShrineBuffs(tACDPlayer);
}




static void sPVPConfigureFollowerPetForDueling(ACDID idACDPet, BOOL bEnteringDuel)
{


    ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);

    if (ptACDPet && ptACDPet->IsAlive())
    {
        PetType ePetType = ptACDPet->AttributesGetInt(ATTRIBUTE_PET_TYPE);
        if (ePetType == 0 ||
            ePetType == 24 ||
            ePetType == 26 ||
            ePetType == 27)
        {
            if (bEnteringDuel)
            {
                PVPApplyFollowerDuelingBuff(*ptACDPet);
            }
            else
            {
                PVPRemoveFollowerDuelingBuff(*ptACDPet);
            }
        }
    }
}



void PVPResetPowersForDueling(
    Player & tPlayer,
    BOOL bEnteringDuel,
    BOOL bIgnoreCosmeticPets)
{
    ActorCommonData &tACDPlayer = ACDGet(tPlayer.idPlayerACD);


    tACDPlayer.SetInvulnerable(TRUE);

    if(bEnteringDuel || bIgnoreCosmeticPets)
    {

        tACDPlayer.AttributesSetInt(ATTRIBUTE_LAST_COSMETIC_PET, tACDPlayer.AttributesGetInt(ATTRIBUTE_CURRENT_COSMETIC_PET));
    }


    SPlayerKillAllPets(tPlayer, FALSE, FALSE, TRUE, !bEnteringDuel);

    if((!bEnteringDuel || bIgnoreCosmeticPets) && tACDPlayer.AttributesGetInt(ATTRIBUTE_LAST_COSMETIC_PET) != -1)
    {

        SPlayerActivatePet(tPlayer, tACDPlayer.AttributesGetInt(ATTRIBUTE_LAST_COSMETIC_PET));
        tACDPlayer.AttributesSetInt(ATTRIBUTE_LAST_COSMETIC_PET, -1);
    }


    for (int32 i = 0; i < NUM_SKILL_SLOTS; i++)
    {
        SNO snoPower = SPlayerGetActiveSkillInSlot(tPlayer, i);
        if (snoPower != SNO_NONE)
        {
            sPVPPlayerRemoveBuffsAndPets(tACDPlayer, snoPower);
        }
    }

    for (int32 i = 0; i < NUM_TRAIT_SLOTS; i++)
    {
        SNO snoPower = PlayerGetPowerInTraitSlot(tPlayer, i);
        if (snoPower != SNO_NONE)
        {
            sPVPPlayerRemoveBuffsAndPets(tACDPlayer, snoPower);
        }
    }


    sPVPPlayerRemoveBuffsAndPets(tACDPlayer, GlobalSNOGet(GLOBALSNO_POWERS_DESTRUCTIONSTREAK_BUFF_RUN_SPEED));
    sPVPPlayerRemoveBuffsAndPets(tACDPlayer, GlobalSNOGet(GLOBALSNO_POWERS_ENVIRONMENTKILL_BUFF_RESOURCE_REGEN));


    POS pos = tACDPlayer.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);

        PowerAPIBuffInstance *ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            if (!ptBuff->bDeleted &&
                ptBuff->bPositiveEffect &&
                ptBuff->idACDSource != tPlayer.idPlayerACD &&
                ACDIsPlayer(ptBuff->idACDSource))
            {
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, FALSE, TRUE);
            }
        }
    }


    pos = tACDPlayer.listBuffs.GetHeadPosition();
    while (pos)
    {
        BuffID idBuff = tACDPlayer.listBuffs.GetNext(pos);

        PowerAPIBuffInstance *ptBuff = PowerAPI_BuffTryToGet(idBuff);
        if (ptBuff)
        {
            if (!ptBuff->bDeleted &&
                PowerIsPassive(AutoPowerDefinitionR(ptBuff->snoPower)) &&
                (ptBuff->bRemoveOnPassiveApplication || ptBuff->bRemoveOnGreaterRiftEntry))
            {
                PowerAPI_BuffFlagForDeletion(ptBuff, FALSE, TRUE, FALSE, TRUE);
            }
        }
    }


    ACDInventoryEquipmentOrSkillChanged(tACDPlayer);

    int32 nPos;
    ACDID idACDPet = PlayerGetFirstPet(tACDPlayer.id, -1, nPos);
    while (idACDPet != ACDID_INVALID)
    {
        sPVPConfigureFollowerPetForDueling(idACDPet, bEnteringDuel);

        idACDPet = PlayerGetNextPet(tACDPlayer.id, -1, nPos);
    }

    if (!bEnteringDuel)
    {
        PowerAPI_FreeFromRoot(tACDPlayer.id);
        PowerAPI_FreeFromSnare(tACDPlayer.id);
        PowerAPI_FreeFromStun(tACDPlayer.id);
        PowerAPI_FreeFromFear(tACDPlayer.id);

        PowerAPI_BuffRemoveAllNegativeBuffs(tACDPlayer);

        if (tACDPlayer.AttributesGetInt(ATTRIBUTE_QUEUE_DEATH))
        {
            tACDPlayer.AttributesSetInt(ATTRIBUTE_QUEUE_DEATH, FALSE);
        }

        if (tACDPlayer.IsAlive())
        {
            tACDPlayer.ResetLifeStats(TRUE, FALSE, TRUE, TRUE, FALSE);
        }

        tACDPlayer.SetCombatStatus(FALSE);
    }


    tACDPlayer.SetInvulnerable(FALSE);
}




void PVPDuelingWarpPlayerToRandomRespawnLocation(Player & tPlayer)
{

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    int32 nLocalPlayers = 0;

    WorldPlace tResPlace;
    tResPlace.idSWorld = tACDPlayer.GetSWorldID();
    tResPlace.wp = Vector3D(0.0f, 0.0f, 0.0f);


    for (Player * ptOtherPlayer = PlayerGetFirstAll(); ptOtherPlayer; ptOtherPlayer = PlayerGetNextAll(ptOtherPlayer))
    {
        if (PlayersAreLocalToEachOther(tPlayer, *ptOtherPlayer) && tPlayer.nPlayerIndex != ptOtherPlayer->nPlayerIndex)
        {

            ActorCommonData & tACDOtherPlayer = ACDGet(ptOtherPlayer->idPlayerACD);
            tResPlace.wp += tACDOtherPlayer.GetWorldPlaceCenter().wp;
            nLocalPlayers++;
            break;
        }
    }


    if (nLocalPlayers > 0)
    {
        tResPlace.wp *= 1.0f / (float)nLocalPlayers;

        SPlayerFindGoodSpawnLocation(tResPlace, tResPlace, 100.0f);

        Target tResTargetLoc;
        tResTargetLoc.InitializeFromPlace(tResPlace);
        ACDWarpToTarget(tACDPlayer, tResTargetLoc, 15, 0.0f, FALSE);
    }
    else
    {

        uint32 uEntranceGUID = PVPGetDuelingEntranceGUID(tPlayer, tACDPlayer.GetSWorldID(), TRUE);
        if (uEntranceGUID != (uint32)-1)
        {
            ACDID idACD = SWorldGetPortalDestinationACD(tACDPlayer.GetSWorldID(), uEntranceGUID);
            if (idACD != ACDID_INVALID)
            {
                ACDWarpToACD(tACDPlayer, ACDGet(idACD), 10, 0.0f);
            }
        }
    }
}



uint32 PVPGetDuelingEntranceGUID(
    const Player & tPlayer,
    SWorldID idSWorld,
    BOOL bPickFar)
{
    uint32 uEntranceGUID = (uint32)-1;

    const int32 nMaxPVPEntrances = 32;
    uint32 arPossibleGUID[nMaxPVPEntrances];
    int32 nPossibleGUID = 0;
    ActorCommonData *ptACDPlayer = ACDTryToGet(tPlayer.idPlayerACD);
    if (!ptACDPlayer)
        return uEntranceGUID;  // no row of its own: the -1 return is shared with ACDTryToGet's failure paths



    // 30 yards, squared (the fcmp constant is 900.0f)
    const float MIN_DISTANCE_SQUARED = 30.0f * 30.0f;

    SWorld *ptSWorld = SWorldGet(idSWorld);
    POS pos = ptSWorld->tEntranceMap.GetStartAssoc();
    while (pos)
    {
        uint32 uPortalGUID;
        EntranceLocation tEntranceLocation;
        ptSWorld->tEntranceMap.GetNextAssoc(pos, uPortalGUID, tEntranceLocation);
        ACDID idACDPortal = SWorldGetPortalDestinationACD(ptSWorld->id, uPortalGUID);
        ActorCommonData *ptACDPortal = ACDTryToGet(idACDPortal);
        if (ptACDPortal)
        {
            if (bPickFar)
            {
                if (ptACDPortal->GetSWorldID() == ptACDPortal->GetSWorldID() &&

                    WorldPlaceDistanceSquared(ptACDPlayer->GetPlace(), ptACDPortal->GetPlace()) < MIN_DISTANCE_SQUARED)
                {
                    continue;
                }
            }


            arPossibleGUID[nPossibleGUID] = uPortalGUID;
            nPossibleGUID++;

            if (nPossibleGUID >= nMaxPVPEntrances)
                break;
        }
    }


    // the original attributes SGameGlobalsGet to column 36 with RandRange_Internal: probably a macro src lacks
    uEntranceGUID = arPossibleGUID[RandRange_Internal(SGameGlobalsGet().rng, 0, nPossibleGUID - 1)];

    return uEntranceGUID;
}



void sDuelingBuffOnPetAddEffect(PowerAPIBuffInstance * ptBuff, ACDID idACDSummoner, ACDID idACDPet, PetType ePetType, SNO snoPet)
{











    sPVPConfigureFollowerPetForDueling(idACDPet, TRUE);
}



static const int32 DUELING_BUFF_INDEX = 0;

void PVPApplyDuelingBuff(Player & tPlayer)
{


    if (PlayerIsHardcoreAndDead(tPlayer))
    {
        SPlayerDisconnect(tPlayer, QUIT_REASON_HARDCORE_DEATH, FALSE);
        return;
    }

    SPlayerRecalculateUpscalingLevels();

    ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);

    NPCDismiss(tACDPlayer, FALSE, TRUE);

    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUEL_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bRemoveOnDeath = FALSE;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.idACDSource = tACDPlayer.id;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.nBuffIndex = DUELING_BUFF_INDEX;
    tBuffStartData.pOnPetAddEffect = sDuelingBuffOnPetAddEffect;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_TEAM_OVERRIDE, tPlayer.nPlayerIndex + 2);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_DUELING, TRUE);

    PowerAPI_BuffAddToACD(tBuffStartData, tACDPlayer);

    PVPResetPowersForDueling(tPlayer, TRUE, FALSE);
}



void PVPApplyFollowerDuelingBuff(ActorCommonData & tACD)
{

    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUEL_BUFF);
    PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
    tBuffStartData.bInfinite = TRUE;
    tBuffStartData.bRemoveOnDeath = FALSE;
    tBuffStartData.bUndispellable = TRUE;
    tBuffStartData.idACDSource = tACD.id;
    tBuffStartData.snoPower = snoBuff;
    tBuffStartData.eStackType = 0;
    tBuffStartData.bPositiveEffect = TRUE;
    tBuffStartData.nBuffIndex = DUELING_BUFF_INDEX;
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_DUELING, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_HIDDEN, TRUE);
    FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_UNTARGETABLE, TRUE);

    ActorID idActorPet = (ActorID)ACDGetOwner(tACD.id);

    if (ActorTryToGetAI(idActorPet))
    {
        BrainAPIDoIdle(idActorPet);
        ActorAISetStatusFlag(idActorPet, ACTOR_AI_DISABLE_UPDATES_BIT, TRUE);
    }

    PowerAPI_BuffAddToACD(tBuffStartData, tACD);
}



void PVPRemoveDuelingBuff(Player & tPlayer)
{

    ActorCommonData & tACD = ACDGet(tPlayer.idPlayerACD);

    if (!tACD.IsAlive())
    {
        BrainAPIDoResurrect((ActorID)ACDGetOwner(tPlayer.idPlayerACD), FALSE);
    }

    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUEL_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoBuff, DUELING_BUFF_INDEX, tACD, ACDID_INVALID, FALSE);
    if (idBuff != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
    }

    PVPResetPowersForDueling(tPlayer, FALSE, FALSE);
    tPlayer.tLastCheckpoint.Initialize();
}



void PVPRemoveFollowerDuelingBuff(ActorCommonData & tACD)
{

    SNO snoBuff = GlobalSNOGet(GLOBALSNO_POWERS_DUEL_BUFF);
    BuffID idBuff = PowerAPI_BuffFindOnACD(snoBuff, DUELING_BUFF_INDEX, tACD, ACDID_INVALID, FALSE);
    if (idBuff != BUFFID_INVALID)
    {
        PowerAPI_BuffFlagForDeletion(idBuff, TRUE, TRUE, FALSE, TRUE);
    }

    ActorID idActorPet = (ActorID)ACDGetOwner(tACD.id);

    if (ActorTryToGetAI(idActorPet))
    {
        BrainAPIDoIdle(idActorPet);
        ActorAISetStatusFlag(idActorPet, ACTOR_AI_DISABLE_UPDATES_BIT, FALSE);
    }
}




void PVPDuelingHandlePlayerDelete(ACDID idACDPlayer)
{


    int32 nPos;
    for (ACDID idACDPet = PlayerGetFirstPet(idACDPlayer, -1, nPos); idACDPet != ACDID_INVALID; idACDPet = PlayerGetNextPet(idACDPlayer, -1, nPos))
    {
        sPVPConfigureFollowerPetForDueling(idACDPet, FALSE);
    }
}
