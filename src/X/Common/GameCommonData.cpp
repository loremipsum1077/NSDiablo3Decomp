// Original file: X/Common/GameCommonData.cpp
#include "GameCommonData.h"
#include "Definitions/FlagSetDefinition.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "Shell/CmdLine.h"
#include "ThreadGlobals.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"

// Declared here until their headers carry them (signatures from the DWARF).
BOOL GameTypeIsPvP();  // GameTemplate.cpp:255
void PlayersInitializeGameCommonData( GameCommonData & tGCD );  // Player.cpp:1020
void PlayersDisposeGameCommonData( GameCommonData & tGCD );  // Player.cpp:1092
void ReplayInitializeForGame( const GameParams & tParams );  // X/Server/Replay.cpp:430
void ReplayDisposeForGame();  // X/Server/Replay.cpp:511
void AnimPlayerInitializeForGame();  // AnimPlayer.cpp:121
void AnimPlayerDisposeForGame();  // AnimPlayer.cpp:135
void ActCacheWaypoints( const Act eAct );  // Act.cpp:83
void SNOFilesSetCurrentAct( Act eAct );  // SNOFiles.cpp:8497
RNG & SGameGetRNG();  // X/Server/SGame.cpp:6157
namespace GameTesting { void InitializeForGame(); void DisposeForGame(); }
namespace CombatLog { void InitializeForGame(); void DisposeForGame(); }
namespace ChallengeRift { void InitializeForGame(); void DisposeForGame(); }

typedef int32 PVPGameState;  // GameCommonData.h
enum { PVP_GAME_STATE_UNSTARTED = 0, PVP_GAME_STATE_PREGAME = 1, PVP_GAME_STATE_GAME = 2, PVP_GAME_STATE_GAME_OVER = 3 };
typedef int32 PVPGameResult;  // GameCommonData.h
typedef int32 Team;
extern CmdLineParams gtCmdLineParams;  // Shell/CmdLine.cpp

struct ActEntry {  // size 0x8
    Act eAct;  // 0x00
    GlobalSNO eGlobalSNOAct;  // 0x04
};
// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer (see X/Common/NPC.cpp, X/Common/Hireling.cpp).
#include "SNO/SNOName.h"
#include "WorldPlace.h"
#include "../Client/Gfx/GfxVB.h"
#include "D3TypeDescriptor/D3TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>
// X/Common/GameCommonData.h:108 (not adopted into src/X/Common/GameCommonData.h yet; copies live in SGame.cpp etc.)
struct GameSyncedDataMessage {  // size 0xd8
    RequiredMessageHeader tHeader;  // 0x00
    GameSyncedData tSyncedData;  // 0x08
};
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER;








static const ActEntry gptActs[] = {
    { ACT_1, GLOBALSNO_ACT_1 },
    { ACT_2, GLOBALSNO_ACT_2 },
    { ACT_3, GLOBALSNO_ACT_3 },
    { ACT_4, GLOBALSNO_ACT_4 },
    { ACT_5, GLOBALSNO_ACT_5 },
    { ACT_OPEN_WORLD, GLOBALSNO_ACT_OPEN_WORLD },
    { ACT_TEST, GLOBALSNO_ACT_TEST },
};




    // bound 0..15 from the static initializer: (1 << GAME_SYNCED_FLAGS_COUNT) - 1 is the name guess
static const TypeDescriptorField GameSyncedData_Fields[32] = {
    BoundedTypeDescriptorField("", DT_INT, offsetof(GameSyncedData, dwGameSyncedFlags), NULL, (const void *)0, (const void *)((1 << GAME_SYNCED_FLAGS_COUNT) - 1)),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, eAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nInitialMonsterLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nMonsterLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nRandomWeatherSeed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, eOpenWorldMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, eOpenWorldModeAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nOpenWorldModeParam), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, dwOpenWorldTransitionTime), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, eOpenWorldDefaultAct), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, eOpenWorldBonusAct), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(GameSyncedData, snoDungeonFinderLevelArea), "", LEVELAREA_SNO, 0, -1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bLootRunOpen), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nOpenLootRunLevel), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bLootRunBossDead), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bSetDungeonActive), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nHunterPlayerIdx), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bLootRunBossActive), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bTieredLootRunFailed), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bLootRunChallengeCompleted), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, dwPregameEnd), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, dwRoundStart), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, dwRoundEnd), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bPregame), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bPVPGameOver), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(GameSyncedData, bPVPAwardsAssigned), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nTeamWins), NULL, 1, 2, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(GameSyncedData, nTeamScore), NULL, 1, 2, 0),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(GameSyncedData, ePVPGameResult), NULL, 1, 2, 0),
    TypeDescriptorField("", DT_INT64, offsetof(GameSyncedData, uPartyGuideHeroId), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT64, offsetof(GameSyncedData, arTieredRiftParticipatingHeroID), NULL, 1, 4, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameSyncedData), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_GameSyncedData("GameSyncedData", 0, GameSyncedData_Fields, 32, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GAME_SYNCED_DATA = &TypeDescriptor_GameSyncedData;


static const TypeDescriptorField GameSyncedDataMessage_Fields[3] = {
    TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(GameSyncedDataMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_GAME_SYNCED_DATA, offsetof(GameSyncedDataMessage, tSyncedData), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(GameSyncedDataMessage), NULL, 0), }; ComplexTypeDescriptor TypeDescriptor_GameSyncedDataMessage("GameSyncedDataMessage", 0, GameSyncedDataMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_GAME_SYNCED_DATA_MESSAGE = &TypeDescriptor_GameSyncedDataMessage;






void GameCommonData::Initialize( const GameParams & tParams )
{

    GameModeSet( GAMEMODE_NONE );
    eDifficultyLevel = 5;
    nHandicapLevel = tParams.nHandicapLevel;
    dwDifficultyChangedFlags = 0;
    nNumPlayers = 0;
    tTemplate = tParams.tTemplate;
    dwServerTime = 1;
    GameClockInitialize( tGameClock );
    eGameType = tParams.eGameType;
    szLootTraceBuffer = NULL;

    {
        SNO snoGameFlags = GlobalSNOGet( GLOBALSNO_FLAGSET_GAME_FLAGS );
        int nFlagArraySize;
        int nFlagCount;


        ScopedFlagSetDefinitionLockR ptGameFlags( snoGameFlags );
        nFlagCount = ptGameFlags->serFlagLookups.dwSize / sizeof( FlagLookup );
        nFlagArraySize = nFlagCount / 8 + 1;
        for ( int i = 0; i < nFlagCount; i++ )
        {
            FlagLookup & tFlagLookup = ptGameFlags->arFlagLookups[i];
            if ( tFlagLookup.nIndex > nFlagCount )
            {
                nFlagArraySize = tFlagLookup.nIndex / 8 + 1;
            }
        }
        nGameFlagsSize = nFlagArraySize;
        arGameFlags = (uint8 *)SigmaMemoryNew( nFlagArraySize, 4, NULL, TRUE );
    }

    PlayersInitializeGameCommonData( *this );
    ReplayInitializeForGame( tParams );
    AnimPlayerInitializeForGame();

    if ( tParams.dwCreationFlags & 0x40000 )
    {
        tGameSyncedData.dwGameSyncedFlags |= ( 1 << GAME_SYNCED_FLAGS_SEASONAL );
    }

    tGameSyncedData.eAct = tParams.eAct;
    tGameSyncedData.nInitialMonsterLevel = tParams.nMonsterLevel;
    tGameSyncedData.nMonsterLevel = tParams.nMonsterLevel;
    tGameSyncedData.eOpenWorldMode = 0;
    tGameSyncedData.eOpenWorldModeAct = ACT_INVALID;
    tGameSyncedData.nOpenWorldModeParam = -1;
    tGameSyncedData.dwOpenWorldTransitionTime = (uint32)-1;
    tGameSyncedData.eOpenWorldDefaultAct = ACT_INVALID;
    tGameSyncedData.eOpenWorldBonusAct = ACT_INVALID;
    tGameSyncedData.snoDungeonFinderLevelArea = SNO_NONE;
    tGameSyncedData.bLootRunOpen = FALSE;
    tGameSyncedData.nOpenLootRunLevel = -1;
    tGameSyncedData.bLootRunBossDead = FALSE;
    tGameSyncedData.nHunterPlayerIdx = -1;
    tGameSyncedData.bLootRunBossActive = FALSE;
    tGameSyncedData.bTieredLootRunFailed = FALSE;
    tGameSyncedData.bLootRunChallengeCompleted = FALSE;
    tGameSyncedData.bSetDungeonActive = FALSE;
    tGameSyncedData.nRandomWeatherSeed = 0;
    if ( GameActiveGameCommonDataIsServer() )
    {
        tGameSyncedData.nRandomWeatherSeed = RandRange_Internal( SGameGetRNG(), 0, 0x7fffffff );
    }

    Act eActToLoad = tParams.eAct;
    SNOFilesSetCurrentAct( tParams.eAct );
    ActCacheWaypoints( tParams.eAct );

    tGameSyncedData.bPregame = FALSE;
    tGameSyncedData.dwPregameEnd = 0;
    tGameSyncedData.dwRoundStart = 0;
    tGameSyncedData.dwRoundEnd = 0;
    tGameSyncedData.uPartyGuideHeroId = OnlineService::INVALID_HERO_ID;
    for ( int32 i = 0; i < 4; i++ )
    {
        tGameSyncedData.arTieredRiftParticipatingHeroID[i] = 0;
    }
    for ( int32 i = 0; i < 2; i++ )
    {
        tGameSyncedData.nTeamScore[i] = 0;
        tGameSyncedData.ePVPGameResult[i] = -1;
        tGameSyncedData.nTeamWins[i] = 0;
    }
    arItemWeights = NULL;
    tGameSyncedData.bPVPGameOver = FALSE;
    tGameSyncedData.bPVPAwardsAssigned = FALSE;

    GameTesting::InitializeForGame();
    CombatLog::InitializeForGame();
    ChallengeRift::InitializeForGame();
}





























void GameCommonData::Dispose()
{
    XFREE( arGameFlags, NULL );
    AnimPlayerDisposeForGame();
    ReplayDisposeForGame();
    PlayersDisposeGameCommonData( *this );
    GameTesting::DisposeForGame();
    CombatLog::DisposeForGame();
    ChallengeRift::DisposeForGame();
}












GameMode GameCommonData::GameModeGet() const
{
    return eMode;
}





void GameCommonData::GameModeSet( const GameMode _eMode )
{
    eMode = _eMode;
}





void GameEndOfCrash()
{
}






uint32 GameGetServerTime()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.dwServerTime;
}




Act GameGetCurrentAct()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.tGameSyncedData.eAct;
}



BOOL GameIsOpenWorld()
{
    return GameGetCurrentAct() == ACT_OPEN_WORLD
        && ( GameGetGameType() == GT_DIABLO3
          || gtCmdLineParams.eTestType == TESTTYPE_WORLDS
          || gtCmdLineParams.eTestType == TESTTYPE_WORLD_PERF
          || gtCmdLineParams.eTestType == TESTTYPE_OPEN_WORLD );
}







BOOL GameIsWeeklyChallenge()
{
    return GameGetGameType() == GT_CHALLENGE_RIFTS;
}




OpenWorldMode GameGetOpenWorldMode()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.tGameSyncedData.eOpenWorldMode;
}



Act GameGetOpenWorldModeAct()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.tGameSyncedData.eOpenWorldModeAct;
}



int32 GameGetOpenWorldModeParam()
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return tGCD.tGameSyncedData.nOpenWorldModeParam;
}






















Act ActEnumFromActIndex( const int32 nIndex )
{
    int32 nCount = sizeof( gptActs ) / sizeof( gptActs[0] );
    if ( nIndex < 0 || nIndex >= nCount )
    {
        return ACT_INVALID;
    }
    return gptActs[nIndex].eAct;
}






int32 ActIndexFromActEnum( const Act eAct )
{
    int32 nCount = sizeof( gptActs ) / sizeof( gptActs[0] );
    for ( int32 i = 0; i < nCount; i++ )
    {
        if ( gptActs[i].eAct == eAct )
        {
            return i;
        }
    }


    XASSERT( FALSE );
    return 0;
}





SNO ActSNOFromActEnum( const Act eAct )
{
    int32 nCount = sizeof( gptActs ) / sizeof( gptActs[0] );
    for ( int32 i = 0; i < nCount; i++ )
    {
        if ( gptActs[i].eAct == eAct )
        {
            return GlobalSNOGet( gptActs[i].eGlobalSNOAct );
        }
    }
    return SNO_NONE;
}






Act ActEnumFromActSNO( const SNO snoAct )
{
    int32 nCount = sizeof( gptActs ) / sizeof( gptActs[0] );
    for ( int32 i = 0; i < nCount; i++ )
    {
        if ( GlobalSNOGet( gptActs[i].eGlobalSNOAct ) == snoAct )
        {
            return gptActs[i].eAct;
        }
    }
    return ACT_INVALID;
}





uint32 PlayerFlagFromActEnum( const Act eAct )
{
    switch ( eAct )
    {
    case ACT_1: return 25;
    case ACT_2: return 26;
    case ACT_3: return 27;
    case ACT_4: return 28;
    case ACT_5: return 30;
    }
    return 0;
}




BOOL ActEnumIsValid( const Act eAct, BOOL bActTestIsLegal )
{
    switch ( eAct )
    {
    case ACT_1:
    case ACT_2:
    case ACT_3:
    case ACT_4:
    case ACT_5:
    case ACT_OPEN_WORLD:
        return TRUE;
    case ACT_TEST:
        return bActTestIsLegal;
    }
    return FALSE;
}


















BOOL GameCommonGameFlagIsSet( LPCSTR szFlagLabel )
{
    SNO snoGameFlags = GlobalSNOGet( GLOBALSNO_FLAGSET_GAME_FLAGS );
    BOOL bValue = FALSE;
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    int32 nIndex = FlagSetLookup( snoGameFlags, szFlagLabel );
    if ( nIndex >= 0 && nIndex < tGCD.nGameFlagsSize * 8 )
    {
        int32 nByteIndex = nIndex / 8;
        int32 nBitIndex = nIndex % 8;
        bValue = ( tGCD.arGameFlags[nByteIndex] >> nBitIndex ) & 1;
    }
    return bValue;
}














PVPGameState GameGetPVPGameState()
{
    if ( !GameCommonDataExists() )
        return PVP_GAME_STATE_UNSTARTED;
    if ( !GameTypeIsPvP() )
        return PVP_GAME_STATE_UNSTARTED;








    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if ( tGCD.tGameSyncedData.bPregame )
        return PVP_GAME_STATE_PREGAME;
    if ( !tGCD.tGameSyncedData.dwRoundStart )
        return PVP_GAME_STATE_UNSTARTED;
    if ( tGCD.tGameSyncedData.bPVPGameOver )
        return PVP_GAME_STATE_GAME_OVER;
    return PVP_GAME_STATE_GAME;
}











PVPGameResult GameGetPVPGameResultForTeam( Team eTeam )
{
    if ( !GameCommonDataExists() )
        return -1;
    if ( !GameTypeIsPvP() )
        return -1;








    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return ( eTeam == 2 ) ? tGCD.tGameSyncedData.ePVPGameResult[0] : tGCD.tGameSyncedData.ePVPGameResult[1];
}










BOOL GameCommonPlayerIsPartyGuide( const Player & tPlayer )
{
































    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if ( tGCD.tGameSyncedData.uPartyGuideHeroId == OnlineService::INVALID_HERO_ID )
        return FALSE;
    return tPlayer.uHeroId == tGCD.tGameSyncedData.uPartyGuideHeroId;
}
