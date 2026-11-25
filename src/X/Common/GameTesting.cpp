// Original file: X/Common/GameTesting.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).
#include "Quests/Quests.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Game.h"
#include "GameCommonData.h"
#include "Shell/CmdLine.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RefString.h"
#include "GlobalSNO.h"
#include "../Server/SScene.h"
#include <stdio.h>
#include <new>
#include <stdlib.h>
#include <string.h>
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "SNO/SNOName.h"
#include "SNO/SNOFiles.h"
#include "SNO/SymbolicValue.h"
#include "../Server/SGame.h"
#include "../Server/SGameManager.h"
#include "../Server/PlayerList.h"
#include "Networking/GameMessage.h"
#include "ActorCommonData.h"
#include "Attributes/FastAttribute.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Definitions/SkillKitDefinition.h"
#include "Definitions/PowerDefinition.h"
#include "Powers/PowersCommon.h"
#include "../Sigma/GamePerf.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include <float.h>
#include "Player.h"
#include "../Client/CPlayer.h"
#include "../Client/RActors/RActors.h"
#include "ThreadGlobals.h"
#include "../Sigma/StringManip.h"
#include "../Server/Actors/Actors.h"
#include "../Sigma/Files/XFiles.h"
#include "../Sigma/DataStructures/XGrowableArray.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../Server/SWorld.h"
struct SScene;
struct WorldTestEntry {  // size 0x14
    SNO snoWorld;  // 0x00
    Act eAct;  // 0x04
    SNO snoQuest;  // 0x08
    int32 nStepUID;  // 0x0c
    SNO snoDungeonFinder;  // 0x10
};
struct TestScene {  // size 0xc
    SNO snoAct;  // 0x00
    SNO snoWorld;  // 0x04
    SNO snoScene;  // 0x08
};
typedef blz::vector<WorldTestEntry> WorldTestArray;
typedef blz::vector<TestScene> TestSceneArray;
typedef XGrowableArray<int, int> SNOArray;
#include "../Server/SPlayer.h"
// GameTesting.cpp:161 (members past the ones used here kept as padding).
struct GameTestingGlobals {  // size 0x1a8
    TestType eTestType;  // 0x00
    BOOL fFirstAdvance;  // 0x04
    CRefString sFileName;  // 0x08
    Blizzard::Time::Timestamp nTestStartTime;  // 0x20
    TDataArrayIterator<SScene> * ptIterator;  // 0x28
    WorldTestArray tListWorlds;  // 0x30
    TestSceneArray tListTestScenes;  // 0x48
    int32 eCurrentAct;  // 0x60 Act
    SNO snoCurrentWorld;  // 0x64
    int nCurrentWorld;  // 0x68
    SNO actFilter, worldFilter, sceneFilter;  // 0x6c
    SNO snoScenePause;  // 0x78
    int32 classFilter;  // 0x7c
    SNO powerFilter;  // 0x80
    int32 runeIndexFilter;  // 0x84
    SNO actEntryPoint;  // 0x88
    SNO worldEntryPoint;  // 0x8c
    SNO sceneEntryPoint;  // 0x90
    int32 classEntryPoint;  // 0x94
    SNO powerEntryPoint;  // 0x98
    int32 runeIndexEntryPoint;  // 0x9c
    uint32 sampleTime;  // 0xa0
    uint32 worldAdvanceWaitTime;  // 0xa4
    uint32 nDumpFPSFreq;  // 0xa8
    SNOArray tListMonsters;  // 0xb0
    int nMonsterIndex;  // 0xe8
    int32 eSkillTestState;  // 0xec SkillTestState
    SNOArray tListPowers;  // 0xf0
    int nPowerIndex;  // 0x128
    int nRuneIndex;  // 0x12c
    int nClassIndex;  // 0x130
    int nMessageIndex;  // 0x134
    int nMessageIterations;  // 0x138
    FILE * fileErrors;  // 0x140
    CRefString sCurrentAssetGroup;  // 0x148
    CRefString sCurrentAsset;  // 0x160
    CRefString sCurrentReportFilename;  // 0x178
    uint32 dwMinFPSToTriggerScreenshot;  // 0x190
    BOOL bHeroCreated;  // 0x194
    uint64 uHeroID;  // 0x198 OnlineService::HeroId
    BOOL bCommandSent;  // 0x1a0
};
static_assert(sizeof(GameTestingGlobals) == 0x1a8, "GameTestingGlobals");
static_assert(__builtin_offsetof(GameTestingGlobals, fileErrors) == 0x140, "fileErrors");
#include "Definitions/GameBalanceDefinition.h"
BOOL GameCommonDataExists();
extern CmdLineParams gtCmdLineParams;
void sRunBadLinksTest();
int32 LevelAreaGetCount();
CRefString sGetFPSLogFolder();
CRefString SNOFileGetName(const SNOName & snoname);  // X/Common/SNO/SNOFiles.h
LPCSTR SNOGroupGetName(const SNOGroup snoGroup);
void WritePerformanceLogHeader(GamePerfFile * pFile);
void WritePerformanceLogStartCollectionWorld(GamePerfFile * pFile, SNO snoWorld);
void WritePerformanceLogStartCollectionSkill(GamePerfFile * pFile, SNO snoPower, int32 nRuneIndex);
CRefString CPlayerGetScreenshotComment(BOOL bForInspectorField);  // CPlayer.cpp:3432
void GfxScreenshot(LPCSTR szFolder, LPCSTR szFilename, LPCSTR szComment, int32 eFormat, uint32 nQuality);
BOOL GameActiveGameCommonDataIsClient();
FILE * fopen_ex(const char * szPath, const char * szMode);
static inline bool sFilterAct(const SNO snoActFilter, const TestSceneArray & tListTestScenes, const Act eAct);
static inline bool sFilterWorld(const SNO snoWorldFilter, const TestSceneArray & tListTestScenes, const SNO snoWorld, const Act eAct);
LPCSTR ErrorManagerGetReportingFolder();
void FileReferenceInit(FileReference & tFileRef, LPCSTR szPath);
BOOL FileDirectoryCreate(FileReference & tFileRef, const BOOL fCreateSubDirs);
extern const SymbolicEntry gPlayerClasses[9];  // X/Common/SNO/SymbolicValue.cpp:4732
void ExperienceSetLevel(const ACDID idACD, int32 nLevels, const BOOL fUpdateClient);  // X/Server/Experience.h
ACDID HeroReset(ACDID idACDPlayer, const GBID gbidHeroClass, const uint32 dwSaveOptionBits, const BOOL bFemale, const BOOL bResetAllHeroData); /* X/Server/SHero.cpp:2285 */ ActorID GBAddWithEncounterAddStruct(ActorAddStruct & tActorAdd, EncounterAddStruct & tEncounterAdd); /* Actors.cpp:5915 */ uint32 MarkerStringHash(LPCSTR pszName); /* Marker.cpp:204 */ SNO SNOFileFindBestMatch(const SNOGroup snoGroup, LPCSTR szPath); /* SNOFiles.cpp */ struct SkillTestClass { GlobalGBID eGBIDClass; BOOL fFemale; }; static SkillTestClass sgtTestClasses[7] = { { GLOBALGBID_PLAYER_DEMONHUNTER, TRUE }, { GLOBALGBID_PLAYER_BARBARIAN, FALSE }, { GLOBALGBID_PLAYER_WIZARD, TRUE }, { GLOBALGBID_PLAYER_WITCHDOCTOR, FALSE }, { GLOBALGBID_PLAYER_MONK, FALSE }, { GLOBALGBID_PLAYER_CRUSADER, FALSE }, { GLOBALGBID_PLAYER_NECROMANCER, FALSE } };  // GameTesting.cpp:125; values from the image at 0x7100e48a6c
namespace Console {
namespace Online {
// Online.h:1325 (not in the adopted header yet).
struct DummySetupParams {  // size 0x20
    inline DummySetupParams()
    {
        m_snoWorld = INVALID_SNO;
        m_dwCreationFlags = 0;
        m_dwLocalPlayerCount = 1;
        m_nHandicap = 0;
        for (int32 i = 0; i < 4; i++)
            m_playerClasses[i] = -1;
    }

    SNO m_snoWorld;
    uint32 m_dwCreationFlags;
    uint32 m_dwLocalPlayerCount;
    int32 m_nHandicap;
    int32 m_playerClasses[4];  // PlayerClass
};
void CreateDummySetup(const DummySetupParams & tParams);
}  // namespace Online
}  // namespace Console
void MessageSendToServer(int32 nMessageID, void * pMessage, uint32 dwSize, const Player * ptPlayer);  // X/Client/Network/CGameMessage.cpp
namespace GameTesting {
void SetTestType(TestType eTestType);
void TestSkill(SNO snoPower, int32 nRuneIndex);
inline void OutputErrorEntry(FILE * fileOut, LPCSTR szType, LPCSTR szAct, LPCSTR szAssetGroup, LPCSTR szAsset,
                             LPCSTR szLastTouchedBy, LPCSTR szError);
inline BOOL TypeDumpsToErrorFile(const TestType eTestType);






















































































void Initialize()
{
    Console::Online::DummySetupParams tParams;
    tParams.m_snoWorld = GlobalSNOGet(GLOBALSNO_WORLD_FLATWORLD);
    tParams.m_dwLocalPlayerCount = CmdLineGetParams()->nTestPlayerNum;
    tParams.m_nHandicap = 19;
    if (CmdLineGetParams()->eTestClassOverride != -1)  // original line 256: override stores (258-261) first, 0..3 defaults (265-266) in the else
    {
        tParams.m_playerClasses[0] = CmdLineGetParams()->eTestClassOverride;
        tParams.m_playerClasses[1] = CmdLineGetParams()->eTestClassOverride;
        tParams.m_playerClasses[2] = CmdLineGetParams()->eTestClassOverride;
        tParams.m_playerClasses[3] = CmdLineGetParams()->eTestClassOverride;
    }
    else
    {
        tParams.m_playerClasses[0] = 0;
        tParams.m_playerClasses[1] = 1;
        tParams.m_playerClasses[2] = 2;
        tParams.m_playerClasses[3] = 3;
    }
    Console::Online::CreateDummySetup(tParams);

    if (gtCmdLineParams.eTestType == TESTTYPE_BADLINKS)
    {
        SetTestType(TESTTYPE_BADLINKS);
        sRunBadLinksTest();
        SetTestType(TESTTYPE_NONE);
    }
}




























void OnGameBNetSetup()
{
}

BOOL IsAutomatedBNetTesting();  // defined below (DWARF line 3339)
void OnHeartBeat(uint32 dwHeartBeatTime)
{

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (dwHeartBeatTime > 900 && IsAutomatedBNetTesting() && !ptGameTestingGlobals->bCommandSent)
    {
        ptGameTestingGlobals->bCommandSent = TRUE;


        TryConsoleCommand tMessage;
        SigmaMemoryClear(&tMessage, sizeof(tMessage));

        char worldFilter[256];
        worldFilter[0] = '\0';
        if (ptGameTestingGlobals->actFilter != SNO_NONE)
        {
            if (ptGameTestingGlobals->worldFilter == SNO_NONE)
            {
                Blizzard::String::Format(worldFilter, sizeof(worldFilter), "%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actFilter, FALSE).str());
            }
            else if (ptGameTestingGlobals->sceneFilter == SNO_NONE)
            {
                Blizzard::String::Format(worldFilter, sizeof(worldFilter), "%s.%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actFilter, FALSE).str(), SNOToString(WORLD_SNO, ptGameTestingGlobals->worldFilter, FALSE).str());
            }
            else
            {
                Blizzard::String::Format(worldFilter, sizeof(worldFilter), "%s.%s.%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actFilter, FALSE).str(), SNOToString(WORLD_SNO, ptGameTestingGlobals->worldFilter, FALSE).str(), SNOToString(SCENE_SNO, ptGameTestingGlobals->sceneFilter, FALSE).str());
            }
        }




        char worldEntryPoint[256];
        worldEntryPoint[0] = '\0';
        if (ptGameTestingGlobals->actEntryPoint != SNO_NONE)
        {
            if (ptGameTestingGlobals->worldEntryPoint == SNO_NONE)
            {
                Blizzard::String::Format(worldEntryPoint, sizeof(worldEntryPoint), "%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actEntryPoint, FALSE).str());
            }
            else if (ptGameTestingGlobals->sceneEntryPoint == SNO_NONE)
            {
                Blizzard::String::Format(worldEntryPoint, sizeof(worldEntryPoint), "%s.%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actEntryPoint, FALSE).str(), SNOToString(WORLD_SNO, ptGameTestingGlobals->worldEntryPoint, FALSE).str());
            }
            else
            {
                Blizzard::String::Format(worldEntryPoint, sizeof(worldEntryPoint), "%s.%s.%s", SNOToString(ACT_SNO, ptGameTestingGlobals->actEntryPoint, FALSE).str(), SNOToString(WORLD_SNO, ptGameTestingGlobals->worldEntryPoint, FALSE).str(), SNOToString(SCENE_SNO, ptGameTestingGlobals->sceneEntryPoint, FALSE).str());
            }
        }




        Blizzard::String::Format(tMessage.usMessage, sizeof(tMessage.usMessage), "testperf %u %u %s %s", ptGameTestingGlobals->sampleTime, ptGameTestingGlobals->worldAdvanceWaitTime, worldFilter, worldEntryPoint);


        MessageSendToServer(CMSG_TRY_CONSOLE_COMMAND, &tMessage, sizeof(tMessage), NULL);
    }
}





void InitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    GameTestingGlobals *& ptGameTestingGlobals = tGCD.ptGameTestingGlobals;

    ptGameTestingGlobals = (GameTestingGlobals *)SigmaMemoryNew(sizeof(GameTestingGlobals), 4, ptMemoryInterface, TRUE);
    ptGameTestingGlobals->ptIterator = NULL;
    new (&ptGameTestingGlobals->tListWorlds) WorldTestArray();
    new (&ptGameTestingGlobals->tListTestScenes) TestSceneArray();
    ptGameTestingGlobals->eCurrentAct = -1;
    ptGameTestingGlobals->snoCurrentWorld = SNO_NONE;
    ptGameTestingGlobals->actFilter = SNO_NONE;
    ptGameTestingGlobals->worldFilter = SNO_NONE;
    ptGameTestingGlobals->sceneFilter = SNO_NONE;
    ptGameTestingGlobals->snoScenePause = SNO_NONE;
    ptGameTestingGlobals->classFilter = -1;
    ptGameTestingGlobals->powerFilter = SNO_NONE;
    ptGameTestingGlobals->runeIndexFilter = 5;
    ptGameTestingGlobals->actEntryPoint = SNO_NONE;
    ptGameTestingGlobals->worldEntryPoint = SNO_NONE;
    ptGameTestingGlobals->sceneEntryPoint = SNO_NONE;
    ptGameTestingGlobals->classEntryPoint = -1;
    ptGameTestingGlobals->powerEntryPoint = SNO_NONE;
    ptGameTestingGlobals->runeIndexEntryPoint = 5;
    ptGameTestingGlobals->sampleTime = 7;
    ptGameTestingGlobals->worldAdvanceWaitTime = 3;
    ptGameTestingGlobals->nDumpFPSFreq = 1;
    ptGameTestingGlobals->nCurrentWorld = -1;
    ptGameTestingGlobals->nMonsterIndex = 0;
    ptGameTestingGlobals->eSkillTestState = 0;
    ptGameTestingGlobals->nPowerIndex = -1;
    ptGameTestingGlobals->nRuneIndex = -1;
    ptGameTestingGlobals->nClassIndex = 0;
    ptGameTestingGlobals->tListPowers.Initialize();
    ptGameTestingGlobals->tListMonsters.Initialize();
    ptGameTestingGlobals->eTestType = TESTTYPE_NONE;
    ptGameTestingGlobals->fFirstAdvance = TRUE;
    ptGameTestingGlobals->nTestStartTime = 0;
    ptGameTestingGlobals->nMessageIndex = 0;
    ptGameTestingGlobals->nMessageIterations = 0;
    ptGameTestingGlobals->fileErrors = NULL;
    ptGameTestingGlobals->bHeroCreated = FALSE;
    ptGameTestingGlobals->bCommandSent = FALSE;
}





















void DisposeForGame()
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    if (TypeDumpsToErrorFile(ptGameTestingGlobals->eTestType) && ptGameTestingGlobals->fileErrors)
    {
        fclose(ptGameTestingGlobals->fileErrors);
        ptGameTestingGlobals->fileErrors = NULL;
    }

    delete ptGameTestingGlobals->ptIterator;
    ptGameTestingGlobals->ptIterator = NULL;

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    SigmaMemoryFree(ptGameTestingGlobals, ptMemoryInterface);
    ptGameTestingGlobals = NULL;
}
inline void OutputErrorHeader(FILE * fileOut);  // defined at 1057
inline const char * TestTypeToString(TestType eTestType);  // defined at 3308
void ErrorCallback(LPCSTR szString);  // defined at 834

}  // namespace GameTesting
#include "../Server/LuaScript/LuaScript.h"  // LuaScriptAttach (SetTestType 551-560)
namespace GameTesting {

void SetTestType(
    const TestType eTestType)
{






    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;


    if (ptGameTestingGlobals->eTestType != TESTTYPE_NONE)
        return;



    ErrorManagerSetTestingErrorCallback(ErrorCallback);

    ptGameTestingGlobals->nCurrentWorld = 0;
	ptGameTestingGlobals->nMessageIndex = 1;
	ptGameTestingGlobals->nMessageIterations = 0;
	ptGameTestingGlobals->eTestType = eTestType;
    ptGameTestingGlobals->fFirstAdvance = TRUE;
	ptGameTestingGlobals->nTestStartTime = Blizzard::Time::GetTimestamp();


    if (GameActiveGameCommonDataIsServer())
    {
        switch (eTestType)
        {
            case TESTTYPE_SKILLS:
            {
                ptGameTestingGlobals->tListPowers.EmptyArray();
                break;
            }

            case TESTTYPE_WORLDS:
            case TESTTYPE_OPEN_WORLD:
                ptGameTestingGlobals->tListWorlds.clear();
                break;
















            case TESTTYPE_MONSTERS:
            {
                PooledSNONameList listSNOFiles(GetSnoNameListPool());
                SNOFileEnumerateSNOsForGroup(ACTOR_SNO, &listSNOFiles);
                POS posSNOs = listSNOFiles.GetHeadPosition();
                while (posSNOs)
                {
                    SNOName & snonameFile = listSNOFiles.GetNext(posSNOs);
                    ScopedActorDefinitionLockR pdefActor(snonameFile.snoHandle);
                    if (pdefActor->eType == ACTORTYPE_MONSTER)
                    {
                        ptGameTestingGlobals->tListMonsters.Add_Tracked(snonameFile.snoHandle);
                    }
                }
            }
            break;
        }
    }




    if (GameActiveGameCommonDataIsServer())
    {
		BOOL betaBuild = CmdLineGetParams()->tBitArray.Test(CMDLINE_BETA_BNET_SERVER);
		BOOL quickTest = CmdLineGetParams()->tBitArray.Test(CMDLINE_QUICK_TEST_BIT);

        switch (eTestType)
        {
            case TESTTYPE_SKILLS: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_SKILLSTEST), 0xFFFF); break;
            case TESTTYPE_SKILL_PERF: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_PERFSKILLSTEST), 0xFFFF); break;
            case TESTTYPE_WORLD_PERF: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(quickTest ? GLOBALSNO_SCRIPT_QUICK_PERFTEST : (betaBuild ? GLOBALSNO_SCRIPT_PERFTESTBETA : GLOBALSNO_SCRIPT_PERFTEST)), 0xFFFF); break;
            case TESTTYPE_WORLDS: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_WORLDTEST), 0xFFFF); break;
            case TESTTYPE_OPEN_WORLD: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_OPENWORLDTEST), 0xFFFF); break;
            case TESTTYPE_NETWORKING: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_NETWORKINGTEST), 0xFFFF); break;



            case TESTTYPE_MONSTERS: LuaScriptAttach(SCRIPT_ATTACH_NONE, -1, GlobalSNOGet(GLOBALSNO_SCRIPT_MONSTERTEST), 0xFFFF); break;
        }
    }







    if (TypeDumpsToErrorFile(eTestType))
    {
        CHAR szFullPathTempFile[260];























        CRefString sFileNamePrefix = TestTypeToString(eTestType);

        if (!GameActiveGameCommonDataIsClient())
        {
            sFileNamePrefix += "Server";
        }

        ptGameTestingGlobals->sFileName = CRefString().Format("%s.txt", sFileNamePrefix.str());
        sprintf(szFullPathTempFile, "%s\\%s", ErrorManagerGetReportingFolder(), ptGameTestingGlobals->sFileName.str());

        ptGameTestingGlobals->fileErrors = fopen(szFullPathTempFile, "w");
        if (!ptGameTestingGlobals->fileErrors)
        {


            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }


        OutputErrorHeader(ptGameTestingGlobals->fileErrors);
    }



}




TestType GetTestType()
{
    if (!GameCommonDataExists())
    {
        return 0;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        return tGCD.ptGameTestingGlobals->eTestType;
    return 0;
}









void SetDumpFPSFequency(uint32 freq)
{
    if (!GameCommonDataExists())
    {
        return;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        tGCD.ptGameTestingGlobals->nDumpFPSFreq = freq;
}










uint32 GetDumpFPSFequency()
{
    if (!GameCommonDataExists())
    {
        return 0;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        return tGCD.ptGameTestingGlobals->nDumpFPSFreq;
    return 0;
}









void SetMinFPSToTriggerScreenshot(uint32 minFPS)
{
    if (!GameCommonDataExists())
    {
        return;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        tGCD.ptGameTestingGlobals->dwMinFPSToTriggerScreenshot = minFPS;
}










void SetSampleTime(uint32 sampleTimeSecs)
{
    if (!GameCommonDataExists())
    {
        return;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        tGCD.ptGameTestingGlobals->sampleTime = sampleTimeSecs;
}










uint32 GetSampleTime()
{
    if (!GameCommonDataExists())
    {
        return 0;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (!tGCD.ptGameTestingGlobals)
        return 0;
    if (!tGCD.ptGameTestingGlobals->ptIterator)
        return 0;
    SScene * ptSScene = tGCD.ptGameTestingGlobals->ptIterator->ptElement;
    if (ptSScene && tGCD.ptGameTestingGlobals->snoScenePause == ptSScene->tSCD.snoScene)
        return 5000;
    return tGCD.ptGameTestingGlobals->sampleTime;
}

















void SetWorldAdvanceWaitTime(uint32 waitTimeSecs)
{
    if (!GameCommonDataExists())
    {
        return;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.ptGameTestingGlobals)
        tGCD.ptGameTestingGlobals->worldAdvanceWaitTime = waitTimeSecs;
}











uint32 GetWorldAdvanceWaitTime()
{
    if (!GameCommonDataExists())
    {
        return 0;
    }

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (!tGCD.ptGameTestingGlobals)
        return 0;
    if (!tGCD.ptGameTestingGlobals->ptIterator)
        return 0;
    return tGCD.ptGameTestingGlobals->worldAdvanceWaitTime;
}














inline BOOL TypeDumpsToErrorFile(const TestType eTestType)
{
    switch (eTestType)
    {
    case TESTTYPE_WORLDS:
    case TESTTYPE_MONSTERS:
    case TESTTYPE_SKILLS:
    case TESTTYPE_BADLINKS:
    case TESTTYPE_OPEN_WORLD:
        return TRUE;
    default:
        return FALSE;
    }
}






void ErrorCallback(LPCSTR szString)
{
    if (!ThreadGlobals::TryToGet())
        return;
    if (!GameCommonDataExists())
        return;
    if (GetTestType() == TESTTYPE_NONE)
        return;

    BOOL fAnyGamePresent = SGameManager::GetGameCount() != 0;
    if (!fAnyGamePresent)
        return;

    BOOL bReturnToClient = ServerIsLocal() ? (GameServerCodeEnter_Track() || _GameServerCodeEnter()) : FALSE;
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (ServerIsLocal())
        GameServerCodeLeave_Tracked(bReturnToClient);

    CRefString sType;
    CRefString sAct;
    switch (ptGameTestingGlobals->eTestType)
    {
    case TESTTYPE_WORLDS:
    {
        sType = "Worlds";
        Act eAct = GameGetCurrentAct();
        sAct = SymbolToName(eAct, gActs);
        break;
    }
    case TESTTYPE_MONSTERS:
        sType = "Monsters";
        break;
    case TESTTYPE_SKILLS:
        sType = "Skills";
        break;
    case TESTTYPE_BADLINKS:
        sType = "BadLinks";
        break;
    case TESTTYPE_OPEN_WORLD:
        sType = "OpenWorld";
        break;
    default:
        break;
    }

    CHAR szTemp[1024];
    Blizzard::String::Copy(szTemp, szString, sizeof(szTemp));
    strreplace(szTemp, "\n\r", ' ');
    strcat(szTemp, "\n");
    OutputErrorEntry(ptGameTestingGlobals->fileErrors, sType, sAct, ptGameTestingGlobals->sCurrentAssetGroup,
                     ptGameTestingGlobals->sCurrentAsset, "Unknown", szTemp);
}






































































}  // namespace GameTesting
static void sEmailDebugFile(LPCSTR szEmailSubject)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    if (GameTesting::TypeDumpsToErrorFile(ptGameTestingGlobals->eTestType) && ptGameTestingGlobals->fileErrors)
    {
        fclose(ptGameTestingGlobals->fileErrors);
        ptGameTestingGlobals->fileErrors = NULL;
    }
}

// File-scope includes and declarations for the Advance helpers (lines 1449-2511, 3416-3443).
#include "Definitions/SceneDefinition.h"
#include "Definitions/MarkerSetDefinition.h"
#include "Marker/SmartMarker.h"
#include "Nav/NavigationMesh.h"
#include "Nav/NavCellHandle.h"
#include "Definitions/MonsterDefinition.h"
#include "ActorCommonDataSwig.h"
#include "TagMaps/Tags.h"
#include "TagMaps/ActorTags.h"
#include "Networking/Network.h"
#include "../Server/ActorScript.h"
#include "../Server/SGameConstants.h"
#include "../Server/DRLGDungeonTester.h"
#include "../Server/DRLGResults.h"
BOOL WorldCollGetArbitraryPointInScene(const SSceneID idSScene, uint16 uOneRequiredStaticFlags, uint32 uObjectNoOverlapFlags, Vector3D & wpResult, NavCellHandle & hNavCellResult);  // X/Common/WorldCommonCollision.cpp:624
void ScenePrintDeactivationReasonsString();  // X/Common/SceneCommon.cpp:3175
BOOL SPlayerGetStartLocation(const SWorldID idSWorld, const SSceneID idSScene, const Team eTeam, const uint32 uEntranceGUID, WorldPlace & tPlace, Quaternion & q);  // X/Server/SPlayer.cpp:7938
BOOL GetGameMessageTypeAndSize(GameMessageType eMessageType, const XTypeDescriptor *& pTypeDescriptor, uint32 & dwSize, const char *& pszMessageLabel);  // X/Common/Networking/ByteswapMessage.cpp:74
void ServerMessageReceive(int32 nPlayerIndex, void * pMessage);  // X/Server/Network/ServerMessageReceive.cpp:1339
Act ActEnumFromActSNO(const SNO snoAct);  // X/Common/GameCommonData.cpp:433
BOOL SNOFileExistsOnDisk(const SNOName & snoname, const BOOL fCompileIfNeccessary);  // X/Common/SNO/SNOFiles.cpp:5233
void SWorldDungeonFinderCreate(SNO snoDungeonFinder);  // X/Server/SWorld.cpp:2751
void SGameChangeActs(const Act eAct, SNO snoForceToWorld, BOOL bMovePlayers);  // X/Server/SGame.cpp:6816
void SQuestsCompleteMainlineUpToQuest(SNO snoQuest);  // X/Server/Quests/SQuests.cpp:940
void SQuestHandleQuestAdvancement(QuestInstance & tQuestInstance, int32 nCompletedStepUID, int32 nStepUID, BOOL bStartup);  // X/Server/Quests/SQuests.cpp:3012
void SQuestsSyncPlayerState(QuestInstance & tQuestInstance, Player * ptPlayerToSync);  // X/Server/Quests/SQuests.cpp:4356
BOOL SSceneDataArrayIterate(TDataArrayIterator<SScene> & tIterator);  // X/Server/SScene.cpp:380
void SceneCommonDataActivate(const SSceneID idSScene);  // X/Common/SceneCommon.cpp:729
void SWorldResetDungeonFinder();  // X/Server/SWorld.cpp:3153
static bool sFilterScene(const SNO snoSceneFilter, const TestSceneArray & tListTestScenes, const Act eAct, const SNO snoWorld, const SNO snoScene);  // defined below (DWARF line 3417)







void sQuit(LPCSTR szEmailSubject)
{
    sEmailDebugFile(szEmailSubject);

    if (GameActiveGameCommonDataIsClient())
    {
        exit(0);
    }
    else
    {
        SimpleMessage tMessage;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        MessageSendToClient(listDestPlayers, (GameMessageType)379 /* SMSG_GAME_TESTING_DONE */, &tMessage, sizeof(tMessage));
        SGameGlobalsGet().dwFlags |= 4;
    }
}
namespace GameTesting {


















void ReceivedTestingDone()
{
}












inline void OutputErrorHeader(
    FILE * fileOut)
{
    fprintf(fileOut, "Type\tAct\tGroup\tAsset\tLast Touched By\tError\n");
}




inline void OutputErrorEntry(FILE * fileOut, LPCSTR szType, LPCSTR szAct, LPCSTR szAssetGroup, LPCSTR szAsset,
                             LPCSTR szLastTouchedBy, LPCSTR szError)
{
    if (fileOut)
        fprintf(fileOut, "%s\t%s\t%s\t%s\t%s\t%s", szType, szAct, szAssetGroup, szAsset, szLastTouchedBy, szError);
}















void OutputInfoEntry(LPCSTR szMsg)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (ptGameTestingGlobals->fileErrors)
        fputs(szMsg, ptGameTestingGlobals->fileErrors);
}











void SetFilter(LPCSTR szFilter)
{
    char szFilterToTokenize[384];
    Blizzard::String::Copy(szFilterToTokenize, szFilter, sizeof(szFilterToTokenize));

    char szParam1_VariableArray[128];
    char szParam2_VariableArray[128];
    char szParam3[128];
    szParam1_VariableArray[0] = 0;
    szParam2_VariableArray[0] = 0;
    szParam3[0] = 0;

    LPCSTR pszScenePauseName = NULL;
    LPCSTR cszTokens = "|";
    LPCSTR pszFilter = strtok(szFilterToTokenize, cszTokens);
    if (pszFilter)
    {
        sscanf(pszFilter, "%127[^.].%127[^.].%127s", szParam1_VariableArray, szParam2_VariableArray, szParam3);
        pszScenePauseName = strtok(NULL, cszTokens);
    }

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    switch (ptGameTestingGlobals->eTestType)
    {
    case TESTTYPE_WORLDS:
    case TESTTYPE_WORLD_PERF:
    case TESTTYPE_OPEN_WORLD:
    {
        SNOName actFilterName(56, szParam1_VariableArray);
        SNOName worldFilterName(48, szParam2_VariableArray);
        SNOName sceneFilterName(33, szParam3);
        ptGameTestingGlobals->actFilter = actFilterName.snoHandle;
        ptGameTestingGlobals->worldFilter = worldFilterName.snoHandle;
        ptGameTestingGlobals->sceneFilter = sceneFilterName.snoHandle;
        if (pszScenePauseName)
        {
            SNOName scenePauseName(33, pszScenePauseName);
            ptGameTestingGlobals->snoScenePause = scenePauseName.snoHandle;
        }
        break;
    }
    case TESTTYPE_SKILLS:
    case TESTTYPE_SKILL_PERF:
        if (szParam1_VariableArray[0])
        {
            ptGameTestingGlobals->classFilter = NameToSymbol(szParam1_VariableArray, gPlayerClasses);
            if (szParam2_VariableArray[0])
            {
                SNOName powerFilterName(29, szParam2_VariableArray);
                ptGameTestingGlobals->powerFilter = powerFilterName.snoHandle;
                if (szParam3[0])
                    ptGameTestingGlobals->runeIndexFilter = atoi(szParam3);
            }
        }
        break;
    }
}








void SetListPath(LPCSTR szListPath)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    const int MAX_LINE_SIZE = 1024;

    CHAR szListFullPath[260];
    sprintf(szListFullPath, "%s\\%s", SNOFilesGetConsoleDataPathOverride(), szListPath);

    FILE * pListFile = fopen_ex(szListFullPath, "r");
    if (!pListFile)
        return;

    CHAR szLine[MAX_LINE_SIZE];
    while (fgets(szLine, MAX_LINE_SIZE, pListFile))
    {
        char actName[128];
        char worldName[128];
        char sceneName[128];
        memset(actName, 0, sizeof(actName));
        memset(worldName, 0, sizeof(worldName));
        memset(sceneName, 0, sizeof(sceneName));
        sscanf(szLine, "%127[^.].%127[^.].%127s", actName, worldName, sceneName);
        actName[127] = 0;
        worldName[127] = 0;
        sceneName[127] = 0;

        SNOName actSNOName(56, actName);
        SNOName worldSNOName(48, worldName);
        SNOName sceneSNOName(33, sceneName);
        if (actSNOName.snoHandle != SNO_NONE && worldSNOName.snoHandle != SNO_NONE && sceneSNOName.snoHandle != SNO_NONE)
        {
            TestScene tScene;
            tScene.snoAct = actSNOName.snoHandle;
            tScene.snoWorld = worldSNOName.snoHandle;
            tScene.snoScene = sceneSNOName.snoHandle;
            ptGameTestingGlobals->tListTestScenes.push_back(tScene);
        }
    }
    fclose(pListFile);
}




















void SetEntryPoint(LPCSTR entryPoint)
{
    char szParam1_VariableArray[128];
    char szParam2_VariableArray[128];
    char szParam3[128];
    szParam1_VariableArray[0] = 0;
    szParam2_VariableArray[0] = 0;
    szParam3[0] = 0;

    sscanf(entryPoint, "%127[^.].%127[^.].%127s", szParam1_VariableArray, szParam2_VariableArray, szParam3);

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    switch (ptGameTestingGlobals->eTestType)
    {
    case TESTTYPE_WORLDS:
    case TESTTYPE_WORLD_PERF:
    case TESTTYPE_OPEN_WORLD:
    {
        SNOName actEntryPointName(56, szParam1_VariableArray);
        SNOName worldEntryPointName(48, szParam2_VariableArray);
        SNOName sceneEntryPointName(33, szParam3);
        ptGameTestingGlobals->actEntryPoint = actEntryPointName.snoHandle;
        ptGameTestingGlobals->worldEntryPoint = worldEntryPointName.snoHandle;
        ptGameTestingGlobals->sceneEntryPoint = sceneEntryPointName.snoHandle;
        break;
    }
    case TESTTYPE_SKILLS:
    case TESTTYPE_SKILL_PERF:
        if (szParam1_VariableArray[0])
        {
            ptGameTestingGlobals->classEntryPoint = NameToSymbol(szParam1_VariableArray, gPlayerClasses);
            if (szParam2_VariableArray[0])
            {
                SNOName powerEntryPointName(29, szParam2_VariableArray);
                ptGameTestingGlobals->powerEntryPoint = powerEntryPointName.snoHandle;
                if (szParam3[0])
                    ptGameTestingGlobals->runeIndexEntryPoint = atoi(szParam3);
            }
        }
        break;
    }
}






void AddDungeonFinder(LPCSTR szDungeonFinder, const Act eAct)


{



    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;





    if (!sFilterAct(ptGameTestingGlobals->actFilter, ptGameTestingGlobals->tListTestScenes, eAct))
        return;


    SNOName snoname(69, szDungeonFinder);
    if (snoname.snoHandle == SNO_NONE)
        return;
    {
        WorldTestEntry tEntry;
        tEntry.eAct = eAct;
        tEntry.snoQuest = SNO_NONE;
        tEntry.nStepUID = 0;
        tEntry.snoDungeonFinder = snoname.snoHandle;
        ScopedDungeonFinderDefinitionLockR pdefDungeonFinder(snoname.snoHandle);

        int32 nCount = pdefDungeonFinder->serDungeonFinderLevelTables.ElementCount(sizeof(DungeonFinderLevelTable));
        for (int32 i = 0; i < nCount; i++)
        {
            const DungeonFinderLevelTable & tLevelTable = pdefDungeonFinder->arDungeonFinderLevelTables[i];
            if (tLevelTable.flWeight == 0.0f)
                continue;



            if (tLevelTable.snoWorldTileset == SNO_NONE)
                continue;



            if (!sFilterWorld(ptGameTestingGlobals->worldFilter, ptGameTestingGlobals->tListTestScenes, tLevelTable.snoWorldTileset, eAct))
                continue;


            tEntry.snoWorld = tLevelTable.snoWorldTileset;
            ptGameTestingGlobals->tListWorlds.push_back(tEntry);
        }
    }

}

void AddWorld(LPCSTR szWorld, const Act eAct, SNO snoQuest, int32 nStepUID)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (!sFilterAct(ptGameTestingGlobals->actFilter, ptGameTestingGlobals->tListTestScenes, eAct))
        return;

    SNOName snoname(48, szWorld);
    if (snoname.snoHandle == SNO_NONE)
        return;

    {
        WorldTestEntry tEntry;
        if (sFilterWorld(ptGameTestingGlobals->worldFilter, ptGameTestingGlobals->tListTestScenes, snoname.snoHandle, eAct))
        {
            tEntry.snoWorld = snoname.snoHandle;
            tEntry.eAct = eAct;
            tEntry.snoQuest = snoQuest;
            tEntry.nStepUID = nStepUID;
            tEntry.snoDungeonFinder = SNO_NONE;
            ptGameTestingGlobals->tListWorlds.push_back(tEntry);
        }
    }
}














void StartSampling()
{



    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF)
    {
        if (ptGameTestingGlobals->ptIterator)
        {



            if (ptGameTestingGlobals->nCurrentWorld >= 0 && ptGameTestingGlobals->nCurrentWorld < (int32)ptGameTestingGlobals->tListWorlds.size())
            {




                SScene * ptSScene = ptGameTestingGlobals->ptIterator->ptElement;
                WorldTestEntry & tEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld];

                GameTestingWorldSamplingStartMessage tMessage;
                tMessage.snoScene = ptSScene->tSCD.snoScene;
                tMessage.snoWorld = tEntry.snoWorld;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
                MessageSendToClient(listDestPlayers, SMSG_GAME_TESTING_START_WORLD_SAMPLING, &tMessage, sizeof(tMessage));
            }
        }
    }
    else if (ptGameTestingGlobals->eTestType == TESTTYPE_SKILL_PERF)
    {



        if (ptGameTestingGlobals->tListPowers.GetArraySize() && ptGameTestingGlobals->nPowerIndex >= 0 && ptGameTestingGlobals->nPowerIndex < ptGameTestingGlobals->tListPowers.GetArraySize())
        {




            SNO snoPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex];

            GameTestingSkillSamplingStartMessage tMessage;
            tMessage.snoPower = snoPower;
            tMessage.nRuneIndex = ptGameTestingGlobals->nRuneIndex;
            PlayerList listDestPlayers(PlayerListGetElementPool());
            PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
            MessageSendToClient(listDestPlayers, SMSG_GAME_TESTING_START_SKILL_SAMPLING, &tMessage, sizeof(tMessage));
        }
    }
}




void StopSampling()
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF || ptGameTestingGlobals->eTestType == TESTTYPE_SKILL_PERF)
    {
        SimpleMessage tMessage;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
        MessageSendToClient(listDestPlayers, (GameMessageType)382 /* SMSG_GAME_TESTING_STOP_SAMPLING */, &tMessage, sizeof(tMessage));
    }
}












}  // namespace GameTesting

static void sCheckMarkerBounds(
    SScene & tSScene)
{
    ScopedSceneDefinitionLockR ptSceneDef(tSScene.tSCD.snoScene);
    for (int32 i = 0; i < ptSceneDef->arMarkerSets.GetArraySize(); i++)
    {
        ScopedMarkerSetDefinitionLockR ptMarkerSetDef(ptSceneDef->arMarkerSets[i]);
        for (int32 nMarker = 0; nMarker < ptMarkerSetDef->tMarkerSet.GetArraySize(); nMarker++)
        {
            SmartMarker sMarker(ptMarkerSetDef, nMarker);
            sMarker.VerifyMarkerBounds(*ptMarkerSetDef, tSScene.tSCD.snoScene);
        }
    }
}






static void sTestDRLGForWorld(
    const SNO snoWorld)
{
    const float flErrorTolerance = 0.03f;
    const int32 nNumTries = 1000;

    DRLGDungeonTester dungeonTester(snoWorld);
    const DRLGTestResults & results = dungeonTester.TestDungeons(nNumTries);
    // GetErrorCount is inlined here (its body was added to X/Server/DRLGResults.h:139, DWARF DRLGResults.h:254).
    int32 nNumErrors = results.GetErrorCount();
    // Inference: nNumErrors / flErrorPercentage leave no code and no concrete DIE; the formula, and their use on 1482-1485 (compiled out), are unknown.
    float flErrorPercentage = (float)nNumErrors / (float)nNumTries;




}





static void sTestSceneForDuplicateMarkerSets(
    const SNO snoScene)
{
    ScopedSceneDefinitionLockR pdefScene(snoScene);
    int32 nNumSets = pdefScene->arMarkerSets.GetArraySize();
    for (int32 i = 0; i < nNumSets; i++)
    {
        SNO snoMarkerSetToCheck = pdefScene->arMarkerSets[i];
        for (int32 j = i + 1; j < nNumSets; j++)
        {

            // Inference: 1496-1513 leave no instruction (only LockR 1495 / UnlockR 1514 remain); the j start and the compiled-out duplicate check on 1505-1511 are unknown.
            SNO snoMarkerSetCurr = pdefScene->arMarkerSets[j];







        }
    }
}






static BOOL sWarpToValidPointInScene(
    SScene * ptSScene,
    const Actor * ptActor)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    WorldPlace tPlace;
    tPlace.idSWorld = ptSScene->tSCD.idSWorld;

    NavCellHandle hNavCell;
    uint16 uOneRequiredStaticFlags = 1;
    uint32 uDynamicFlagsNoOverlap = NavMeshGetDynamicFlagsForPassability(PASSABILITY_PLAYER);

    ActorCommonData &tACD = ACDGet(ptActor->idACD);
    WorldLocation &loc = tACD.GetWorldLocation();

    if (loc.idSWorld == tPlace.idSWorld &&
        WorldCollGetArbitraryPointInScene(ptSScene->id, uOneRequiredStaticFlags, uDynamicFlagsNoOverlap, tPlace.wp, hNavCell))
    {

        WorldTestEntry &tEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld];


        ptGameTestingGlobals->sCurrentAssetGroup = "";
        ptGameTestingGlobals->sCurrentAsset = CRefString().Format("%s:%s",
            SNOToString(WORLD_SNO, tEntry.snoWorld).str(),
            SNOToString(SCENE_SNO, ptSScene->tSCD.snoScene).str());
        ActorSnapToPosition(ptActor->id, tPlace, TRUE, TRUE);

        ScenePrintDeactivationReasonsString();
        // return TRUE: no row of its own; the caller's flag store `str w8(=1), [sp, #0x8]` sits in line 1548's rows (col 0)
        return TRUE;
    }
    // return FALSE: no row of its own; both failed conditions branch straight to the caller (+0x6b4)
    return FALSE;
}









static void sPrintCounts()
{
    SGame & tSGame = SGameManager::GameGet();
    int32 nTotalWorldCount = tSGame.ptSWorlds->GetTotalCount();
    int32 nTotalSceneCount = tSGame.ptSScenes->GetTotalCount();
    int32 nTotalLevelAreaCount = LevelAreaGetCount();

    CRefString sMsg;
    sMsg.Format("World count %d, Scene count %d, Level Area count %d\n", nTotalWorldCount, nTotalSceneCount,
                nTotalLevelAreaCount);
    GameTesting::OutputInfoEntry(sMsg);
}




static CRefString sGetAutomatedStatusFileName()
{
    return CRefString().Format("AutomatedStatus%s.txt", gtCmdLineParams.szInstanceName);
}

static void sAdvanceWorlds()
{


    CGameServerLoadingPartRequired(LSC_PLAYER_WARPED);


    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    const Actor * ptActor = NULL;
    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        if (ptActor)
        {
            SGameGlobals & tSGameGlobals = SGameGlobalsGet();
            tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_SLOW_KILL_BIT);

            if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF)
            {
                ActorBoostToLevel(
                    ACDGet(ptPlayerCurr->idPlayerACD),
                    70,
                    "",
                    "",
                    (1 << PLAYER_BOOST_PICKUP_ITEMS_BIT) |
                    (1 << PLAYER_BOOST_NOTIFY_CLIENT_BIT) |
                    (1 << PLAYER_BOOST_REMOVE_EQUIPMENT_FIRST_BIT) |
                    (1 << PLAYER_BOOST_SHOW_LEVEL_UP_UI_BIT));
            }
        }
    }
    // Inference: cbz at +0x1f4 goes to +0x1378, a CGameServerLoadingPartCompleted(4) tail at line 0 (merged with 1649 and 1856).
    if (!ptActor)
    {
        CGameServerLoadingPartCompleted(LSC_PLAYER_WARPED);
        return;
    }



    BOOL fTestingWorlds = ptGameTestingGlobals->eTestType == TESTTYPE_WORLDS;
    BOOL fCountWorlds = ptGameTestingGlobals->eTestType == TESTTYPE_OPEN_WORLD;

    ActorCommonData & tACD = ACDGet(ptActor->idACD);
    WorldLocation & loc = tACD.GetWorldLocation();

    if (!ptGameTestingGlobals->ptIterator)
    {
        if (ptGameTestingGlobals->nCurrentWorld >= (int32)ptGameTestingGlobals->tListWorlds.size())
        {
            if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF)
            {
                CRefString sFileName = sGetFPSLogFolder() + sGetAutomatedStatusFileName();
                Blizzard::File::Delete(sFileName.str());
            }




            sPrintCounts();
            sQuit(NULL);  // Inference: no instruction sets x0 before the bl at +0x1374; NULL is a stand-in, as in sRunBadLinksTest
            CGameServerLoadingPartCompleted(LSC_PLAYER_WARPED);  // Inference: tail-merged into the line-0 call at +0x1378 (col 4, as sQuit's)
            return;
        }
        else
        {



            if (ptGameTestingGlobals->actEntryPoint != SNO_NONE)
            {
                Act eActEntryPoint = ActEnumFromActSNO(ptGameTestingGlobals->actEntryPoint);
                while (ptGameTestingGlobals->nCurrentWorld < (int32)ptGameTestingGlobals->tListWorlds.size())
                {
                    WorldTestEntry & tWorldEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld];
                    if (tWorldEntry.eAct == eActEntryPoint && (tWorldEntry.snoWorld == ptGameTestingGlobals->worldEntryPoint || ptGameTestingGlobals->worldEntryPoint == SNO_NONE))
                    {
                        break;
                    }

                    ptGameTestingGlobals->nCurrentWorld++;
                }

                ptGameTestingGlobals->actEntryPoint = SNO_NONE;
                ptGameTestingGlobals->worldEntryPoint = SNO_NONE;
            }

            WorldTestEntry & tEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld];
            ptGameTestingGlobals->eCurrentAct = tEntry.eAct;
            ptGameTestingGlobals->snoCurrentWorld = tEntry.snoWorld;

            if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF)
            {

                CRefString sFileName = sGetFPSLogFolder() + sGetAutomatedStatusFileName();
                FILE * pAutomatedStatus = fopen(sFileName.str(), "w");
                fprintf(pAutomatedStatus, "CURRENT:%s.%s\n", SymbolToName(tEntry.eAct, gActs), SNOToString(SNOName(WORLD_SNO, tEntry.snoWorld), FALSE).str());
                if (ptGameTestingGlobals->nCurrentWorld + 1 < (int32)ptGameTestingGlobals->tListWorlds.size())
                {
                    WorldTestEntry & tNextEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld + 1];
                    fprintf(pAutomatedStatus, "NEXT:%s.%s\n", SymbolToName(tNextEntry.eAct, gActs), SNOToString(SNOName(WORLD_SNO, tNextEntry.snoWorld), FALSE).str());
                }
                fclose(pAutomatedStatus);
            }











            SNO snoWorld = tEntry.snoWorld;
            SNOName snonameFile(WORLD_SNO, snoWorld);
            if (!SNOFileExistsOnDisk(snonameFile, TRUE))
            {
                // Inference: the cbz at +0x1190 leaves the helper (to +0x1380); the return has no instruction of its own.
                return;
            }


            ptGameTestingGlobals->sCurrentAssetGroup = SNOGroupGetName(snonameFile.snoGroup);
            ptGameTestingGlobals->sCurrentAsset = SNOFileGetName(snonameFile);
            if (tEntry.snoDungeonFinder != SNO_NONE)
            {






                SWorldDungeonFinderCreate(tEntry.snoDungeonFinder);

                SGameChangeActs(tEntry.eAct, snoWorld, TRUE);
            }
            else
            {
                SGameChangeActs(tEntry.eAct, snoWorld, TRUE);




                if (tEntry.snoQuest != SNO_NONE)
                {
                    QuestInstance * ptQuest = QuestsGetQuestInstance(tEntry.snoQuest, ACDID_INVALID);
                    SQuestsCompleteMainlineUpToQuest(tEntry.snoQuest);


                    int32 nCurrentStepUID = ptQuest->nCurrentStepUID;
                    if (tEntry.nStepUID != QUEST_UNASSIGNED_STEP_UID && nCurrentStepUID != tEntry.nStepUID)
                    {
                        if (QuestStepIsReachableByStep(tEntry.snoQuest, nCurrentStepUID, tEntry.nStepUID))
                        {
                            QuestsAdvanceToStep(*ptQuest, tEntry.nStepUID, FALSE);
                            SQuestHandleQuestAdvancement(*ptQuest, nCurrentStepUID, tEntry.nStepUID, TRUE);
                            SQuestsSyncPlayerState(*ptQuest, NULL);
                        }
                    }
                }
            }








            if (fTestingWorlds)
            {
                sTestDRLGForWorld(snoWorld);
            }

            ptGameTestingGlobals->ptIterator = new TDataArrayIterator<SScene>;
        }
    }
    // Inference: endless loop (no instruction); its exits are the return at 1791 and the bWarpSuccess test at 1855; the filter jumps (+0x6dc) go straight back to 1770.
    while (TRUE)
    {
        BOOL fKeepGoing = !fCountWorlds && SSceneDataArrayIterate(*ptGameTestingGlobals->ptIterator);
        if (!fKeepGoing)
        {
            delete ptGameTestingGlobals->ptIterator; ptGameTestingGlobals->ptIterator = NULL;
            WorldTestEntry & tEntry = ptGameTestingGlobals->tListWorlds[ptGameTestingGlobals->nCurrentWorld];
            ptGameTestingGlobals->nCurrentWorld++;

            if (fCountWorlds)
            {
                sPrintCounts();
            }
            else
            {
                DeleteAllWorldsAndTransferPlayersToLimbo();
                if (tEntry.snoDungeonFinder != SNO_NONE)
                {
                    SWorldResetDungeonFinder();
                }
            }

            CGameServerLoadingPartCompleted(LSC_PLAYER_WARPED);
            return;  // Inference: b +0x1384 leaves the helper; no instruction of its own
        }


        SScene * ptSScene = ptGameTestingGlobals->ptIterator->ptElement;
        if (ptSScene->tSCD.idSWorld != loc.idSWorld)
        {
            continue;
        }

        if (!sFilterScene(ptGameTestingGlobals->sceneFilter, ptGameTestingGlobals->tListTestScenes,
            ptGameTestingGlobals->eCurrentAct, ptGameTestingGlobals->snoCurrentWorld, ptSScene->tSCD.snoScene))
        {
            continue;
        }


        if (ptGameTestingGlobals->sceneEntryPoint != SNO_NONE)
        {
            if (ptSScene->tSCD.snoScene != ptGameTestingGlobals->sceneEntryPoint)
            {
                continue;
            }

            ptGameTestingGlobals->sceneEntryPoint = SNO_NONE;
        }

        {
            ScopedSceneDefinitionLockR ptSceneDef(ptSScene->tSCD.snoScene);

            if (ptSceneDef->tHeader.dwFlags & (1 << DH_IS_BAD_DATA_BIT))
            {
                continue;
            }
        }

        SceneCommonDataActivate(ptSScene->id);
        if (fTestingWorlds)
        {
            sCheckMarkerBounds(*ptSScene);
            sTestSceneForDuplicateMarkerSets(ptSScene->tSCD.snoScene);
        }


        Player * ptPlayerCurr = PlayerGetFirstInGame();
        ACDTryToGet(ptPlayerCurr->idPlayerACD);  // Inference: only cmn/b.eq/GetLocalStorage of an inlined ACDTryToGet remain (+0x470-+0x484), result unused, before the null test of ptPlayerCurr; the enclosing statement is unknown

        BOOL bWarpSuccess = FALSE;
        for (; ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
        {
            Actor * ptThisActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
            if (ptThisActor)
            {
                if (sWarpToValidPointInScene(ptSScene, ptThisActor))
                {
                    bWarpSuccess = TRUE;
                }
            }
        }



        if (bWarpSuccess)
            break;  // Inference: no instruction of its own; the cbnz at +0x6d8 carries the loop closing-brace line 1855
    }
    CGameServerLoadingPartCompleted(LSC_PLAYER_WARPED);  // Inference: tail-merged into the line-0 call at +0x1378 (with 1622 and 1649)
}









namespace GameTesting {
void KillAllMonsters(const ActorID idActorKiller, const BOOL fNoLoot, const BOOL fNoHealthDrop, const float flMaxDistToPlayer)
{
}




































































}  // namespace GameTesting

static void sSpawnMonster(
    SNO snoMonster)
{
    if (snoMonster == SNO_NONE)
        return;



    Player *ptPlayer = PlayerGetByPlayerIndex(0);
    // Inferred: cbz on PlayerGetByPlayerIndex's result (line 0 row at +0xbac) jumps to the helper's end.
    if (!ptPlayer)
        return;


    ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
    if (pdefMonster)  // Inferred: cbz on the LockR result (line 0 row at +0xbcc) skips to ~pdefMonster at 1983; tActorAdd sits in a nested lexical block closing at 1982.
    {
        ActorAddStruct tActorAdd;
        tActorAdd.snoActor = pdefMonster->snoActor;
        tActorAdd.tSNOName.snoGroup = ACTOR_SNO;
        tActorAdd.tSNOName.snoHandle = pdefMonster->snoActor;
        tActorAdd.eMonsterRarity = MR_NORMAL;
        tActorAdd.eTeam = TEAM_MONSTER_0;
        tActorAdd.tMarkersName = MarkerStringHash("TestingSpawnMonster");
        tActorAdd.dwActorAddFlags |= (1 << ACTORADD_ADD_AT_SPAWNER_LOCATION_IF_SEARCH_FAILS);

        ScopedActorDefinitionLockR pdefActor(tActorAdd.snoActor);
        if (pdefActor->eType != ACTORTYPE_MONSTER)
            return;




        ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        // Inferred: cbz x0 on the ACDTryToGet result (+0xcb4, line 0) jumps to the 1982 cleanup; 1979 is at column 9, so no nested if-body.
        if (!ptACDPlayer)
            return;

        ActorFindAddLocation(*ptACDPlayer, PASSABILITY_MONSTER, tActorAdd);

        GBAdd(tActorAdd);
    }
}






static void sAdvanceMonsters()
{

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    Player *ptPlayer = PlayerGetByPlayerIndex(0);
    const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayer->idPlayerACD));
    if (!ptActor)
        return;



    ActorFlagSet(ptActor->id, ACTOR_GOD_MODE_BIT, TRUE);

    ActorCommonData *ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    // Inferred: the ACDTryToGet miss (ccmp/b.eq at +0x91c, inlined row) skips the 2007 call to line 2010.
    if (ptACDPlayer)
        ptACDPlayer->AttributesSetInt(ATTRIBUTE_GOD, 1);


    if (ptGameTestingGlobals->nMonsterIndex >= ptGameTestingGlobals->tListMonsters.GetArraySize())
    {
        // Inferred: b.ge at +0x940 goes to the shared line-0 `bl sQuit` at +0xf38 (tail-merged with line 2500), then the function exit; the argument is dead in sQuit (x0 is never set), value unknown.
        sQuit(NULL);
        return;
    }


    for (Player *ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        ActorCommonData *ptACDPlayerCurr = ACDTryToGet(ptPlayerCurr->idPlayerACD);
        const Actor *ptActorCurr = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        if (!ptACDPlayerCurr || !ptActorCurr)
            return;



        WorldPlace tPlaceStart;
        Quaternion qStart;
        if (SPlayerGetStartLocation(ptACDPlayerCurr->GetWorldLocation().idSWorld, SSCENEID_INVALID, TEAM_INVALID, (uint32)-1, tPlaceStart, qStart))
        {
            ActorSnapToPosition(ptActorCurr->id, tPlaceStart, TRUE, TRUE);
        }
    }

    SNO &snoActor = ptGameTestingGlobals->tListMonsters[ptGameTestingGlobals->nMonsterIndex];
    SNOName snonameActor(ACTOR_SNO, snoActor);
    ptGameTestingGlobals->nMonsterIndex++;
    if (!MonsterExists(snonameActor))
    {
        ptGameTestingGlobals->sCurrentAssetGroup = SNOGroupGetName(snonameActor.snoGroup);
        ptGameTestingGlobals->sCurrentAsset = SNOFileGetName(snonameActor);
        return;
    }



    SNOName snonameMonster;
    MonsterGetSNONameFromActor(snonameActor.snoHandle, snonameMonster);

    ptGameTestingGlobals->sCurrentAssetGroup = SNOGroupGetName(snonameMonster.snoGroup);
    ptGameTestingGlobals->sCurrentAsset = SNOFileGetName(snonameMonster);

    CRefString sMonsterName;
    if (!MonsterGetBaseName(snonameMonster.snoHandle, sMonsterName))
    {
        ScopedActorDefinitionLockR pdefActor(snoActor);
        BOOL bRenderOnlyInEditMode = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_RENDER_ONLY_IN_EDIT_MODE);




    }


    sSpawnMonster(snonameMonster.snoHandle);
}




namespace GameTesting {
void LearnAllSkills(ActorCommonData & tACDPlayer, const int nSkillLevel)
{
    SNO snoSkillKit = tACDPlayer.GetSkillKit();

    ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
    int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
    for (int32 i = 0; i < nCount; i++)
    {
        ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
        SNO snoCurrentPower = tEntry.snoPower;
        if (snoCurrentPower != SNO_NONE)
        {
            if (PowerIsDisplayed(AutoPowerDefinitionR(snoCurrentPower)))
            {
                int32 nCurrentLevel = tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoCurrentPower));
                if (nCurrentLevel < nSkillLevel)
                    tACDPlayer.AttributesSetInt(MakeAttribKey(ATTRIBUTE_SKILL, snoCurrentPower), nSkillLevel);
            }
        }
        tACDPlayer.ApplyPassive(snoCurrentPower, FALSE);
    }
}










void SetLevel(const ACDID idACDPlayer, const int32 nLevel, const BOOL fUpdateClient)
{
    ExperienceSetLevel(idACDPlayer, nLevel, fUpdateClient);
}

}  // namespace GameTesting
#include "../Server/Powers/Powers.h"  // PowerStartData, PowerStartDataInit, PowerStartEx (TestSkill)
#include "Hyperlink.h"  // HyperlinkCreateFromSNO (TestSkill 2195/2205)
namespace GameTesting {









void TestSkill(
    const SNO snoSkillPower,
    int32 nRuneIndex)
{




    if (snoSkillPower == SNO_NONE)
        return;



    if (PlayerGetFirstInGame() == NULL)
        return;



    for (Player *ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        if (!ptActor)
            return;



        ActorCommonData &tACD = ACDGet(ptPlayerCurr->idPlayerACD);
        PowerID idPower;
        if (PowerRequiresTarget(tACD, AutoPowerDefinitionR(snoSkillPower)))
        {
            Target tTarget;
            if (PowerRequiresActorTarget(tACD, AutoPowerDefinitionR(snoSkillPower)))
            {

                return;
            }
            else
            {
                WorldPlace tPlace;
                WorldLocation &loc = tACD.GetWorldLocation();
                tPlace.idSWorld = loc.idSWorld;
                tPlace.wp = loc.transform.wp;
                tPlace.wp.x += 10.0f;

                tTarget.InitializeFromPlace(tPlace);

                PowerStartData tPowerStartData;
                PowerStartDataInit(tPowerStartData);
                tPowerStartData.idACD = ptPlayerCurr->idPlayerACD;
                tPowerStartData.idACDAlternateSource = tPowerStartData.idACD;
                tPowerStartData.snoPower = snoSkillPower;
                tPowerStartData.tTarget = tTarget;
                tPowerStartData.eRuneTypeOverride = nRuneIndex;
                idPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
            }
        }
        else
        {

            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = ptPlayerCurr->idPlayerACD;
            tPowerStartData.idACDAlternateSource = tPowerStartData.idACD;
            tPowerStartData.snoPower = snoSkillPower;
            tPowerStartData.tTarget = cgTargetNone;
            tPowerStartData.eRuneTypeOverride = nRuneIndex;
            idPower = PowerStartEx(tPowerStartData, ATTRIBGROUPID_INVALID, FALSE, FALSE);
        }


        if (idPower == POWERID_INVALID)
        {
            CRefString sMessage;
            sMessage.Format("%s unable to start", HyperlinkCreateFromSNO(SNOName(POWER_SNO, snoSkillPower), NULL).str());  // columns (.str() at 109) fit a call without the NULL argument (a default argument src's Hyperlink.h lacks)

            WorldPlace wpOwner = tACD.GetPlace();
            SWorld *ptSWorld = SWorldTryToGet(wpOwner.idSWorld);
            if (ptSWorld)
            {
                sMessage += CRefString().Format(";  Location: [%0.2f, %0.2f, %0.2f] in %s.",
                    wpOwner.wp.x,
                    wpOwner.wp.y,
                    wpOwner.wp.z,
                    HyperlinkCreateFromSNO(SNOName(WORLD_SNO, ptSWorld->tWCD.snoWorld), NULL).str());  // see 2195 on the NULL argument
            }

            sMessage += "\n";


        }
    }

}



}  // namespace GameTesting

static void sAdvanceSkills()
{

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    if (!PlayerGetFirstInGame())
    {
        return;
    }

    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        // Inference: the null test leaves only line-0 cbz/cbnz after ActorTryToGet (+0x7bc..+0x7f0) to the helper exit; same shape as SetupPerfSkillTest.
        if (!ptActor)
            return;
    }


    SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_SLOW_KILL_BIT);
    SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_AI_BLIND_BIT);

    if (ptGameTestingGlobals->eSkillTestState == 2)
    {
        // Inference: x0 is never set before bl sQuit (+0xe84), so the argument is unknowable; NULL as in sRunBadLinksTest.
        sQuit(NULL);
        if (ptGameTestingGlobals->eTestType == TESTTYPE_SKILL_PERF)
        {
            CRefString sFileName = sGetFPSLogFolder() + sGetAutomatedStatusFileName();
            Blizzard::File::Delete(sFileName.str());
        }
        return;  // Inference: the state-2 path branches to the helper exit (+0xf08 -> +0x1938) instead of line 2359.
    }
    if (ptGameTestingGlobals->eSkillTestState == 0)  // Inference: no row of its own; its test is the cbz folded into line 2242's compare.
    {
        if (ptGameTestingGlobals->classFilter != -1)
        {
            if (ptGameTestingGlobals->nClassIndex < ptGameTestingGlobals->classFilter)
            {
                ptGameTestingGlobals->nClassIndex = ptGameTestingGlobals->classFilter;
            }
            else if (ptGameTestingGlobals->nClassIndex > ptGameTestingGlobals->classFilter)
            {
                // Inference: b.gt (+0x142c) goes to the line-0 "eSkillTestState = 2" at +0x1bfc (shared with line 2361), then the helper exit.
                ptGameTestingGlobals->eSkillTestState = 2;
                return;
            }
        }

        if (ptGameTestingGlobals->classEntryPoint != -1)
        {
            ptGameTestingGlobals->nClassIndex = ptGameTestingGlobals->classEntryPoint;
            ptGameTestingGlobals->classEntryPoint = -1;
        }

        SkillTestClass & tTestClass = sgtTestClasses[ptGameTestingGlobals->nClassIndex];

        GBID gbidClass = GlobalGBIDGet(tTestClass.eGBIDClass);
        CRefString szNewHero;
        szNewHero.Format("%s %s", GBIDToString(gbidClass, GB_HEROS, FALSE).str(), tTestClass.fFemale ? "Female" : "Male");


        ptGameTestingGlobals->sCurrentAssetGroup = "Player";
        ptGameTestingGlobals->sCurrentAsset = szNewHero;
        // Inference: initializer leaves no row; w25 = -1 (+0x14f4) is the value used when there is no player (+0x1680).
        SNO snoSkillKit = SNO_NONE;

        for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
        {
            const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
            ActorCommonData & tACDPlayer = ACDGet(ptPlayerCurr->idPlayerACD);

            HeroReset(ptPlayerCurr->idPlayerACD, gbidClass, 0, tTestClass.fFemale, TRUE);


            GameTesting::SetLevel(tACDPlayer.id, 70, FALSE);
            GameTesting::LearnAllSkills(tACDPlayer, 1);

            ActorFlagSet(ptActor->id, ACTOR_GOD_MODE_BIT, TRUE);
            tACDPlayer.AttributesSetInt(ATTRIBUTE_GOD, 1);
            tACDPlayer.ResetLifeStats(TRUE, TRUE, TRUE, TRUE, FALSE);

            snoSkillKit = tACDPlayer.GetSkillKit();
        }

        ptGameTestingGlobals->tListPowers.EmptyArray();
        ptGameTestingGlobals->nPowerIndex = 0;
        ptGameTestingGlobals->nRuneIndex = ptGameTestingGlobals->runeIndexFilter == 5 ? -1 : ptGameTestingGlobals->runeIndexFilter;

        ScopedSkillKitDefinitionLockR pdefSkillKit(snoSkillKit);
        int32 nCount = pdefSkillKit->serActiveSkillEntries.dwSize / sizeof(ActiveSkillEntry);
        for (int32 i = 0; i < nCount; i++)
        {
            const ActiveSkillEntry & tEntry = pdefSkillKit->arActiveSkillEntries[i];
            if ((ptGameTestingGlobals->powerFilter == SNO_NONE) || (ptGameTestingGlobals->powerFilter == tEntry.snoPower))
            {
                ptGameTestingGlobals->tListPowers.Add_Tracked(tEntry.snoPower);
            }
        }


        if (ptGameTestingGlobals->powerEntryPoint != SNO_NONE)
        {
            while (ptGameTestingGlobals->nPowerIndex < ptGameTestingGlobals->tListPowers.GetArraySize())
            {
                SNO snoPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex];
                if (snoPower == ptGameTestingGlobals->powerEntryPoint)
                {
                    break;  // Inference: no row; the b.eq at line 2326 goes straight to line 2334.
                }

                ptGameTestingGlobals->nPowerIndex++;
            }

            ptGameTestingGlobals->powerEntryPoint = SNO_NONE;
        }

        if (ptGameTestingGlobals->eTestType == TESTTYPE_SKILL_PERF && ptGameTestingGlobals->nPowerIndex < ptGameTestingGlobals->tListPowers.GetArraySize())
        {
            SNO snoPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex];

            CRefString sFileName = sGetFPSLogFolder() + sGetAutomatedStatusFileName();
            FILE * pAutomatedStatus = fopen(sFileName.str(), "w");
            fprintf(pAutomatedStatus, "CURRENT:%s.%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex, gPlayerClasses), SNOToString(SNOName(29, snoPower), FALSE).str());
            if (ptGameTestingGlobals->nPowerIndex + 1 < ptGameTestingGlobals->tListPowers.GetArraySize())
            {
                SNO snoNextPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex + 1];
                fprintf(pAutomatedStatus, "NEXT:%s.%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex, gPlayerClasses), SNOToString(SNOName(29, snoNextPower), FALSE).str());
            }
            else if (ptGameTestingGlobals->nClassIndex + 1 < sizeof(sgtTestClasses) / sizeof(sgtTestClasses[0]))
            {
                fprintf(pAutomatedStatus, "NEXT:%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex + 1, gPlayerClasses));
            }
            fclose(pAutomatedStatus);
        }

        ptGameTestingGlobals->eSkillTestState = 1;
    }

    if (ptGameTestingGlobals->tListPowers.GetArraySize() == 0)
    {
        ptGameTestingGlobals->eSkillTestState = 2;  // Inference: no row; the line-0 store at +0x1bfc shared with line 2264.
    }
    else
    {

        if (ptGameTestingGlobals->runeIndexFilter != 5)
        {
            return;
        }


        ptGameTestingGlobals->nRuneIndex++;
        if (ptGameTestingGlobals->nRuneIndex == 5)
        {
            ptGameTestingGlobals->nRuneIndex = -1;
            ptGameTestingGlobals->nPowerIndex++;
            if (ptGameTestingGlobals->nPowerIndex >= ptGameTestingGlobals->tListPowers.GetArraySize())
            {
                ptGameTestingGlobals->nClassIndex++;
                if (ptGameTestingGlobals->nClassIndex >= sizeof(sgtTestClasses) / sizeof(sgtTestClasses[0]))
                {
                    ptGameTestingGlobals->eSkillTestState = 2;  // Inference: both stores merge into the cset/lsl/str select on line 2380.
                }
                else
                {
                    ptGameTestingGlobals->eSkillTestState = 0;
                }
            }
            else
            {
                if (ptGameTestingGlobals->eTestType == TESTTYPE_SKILL_PERF)
                {
                    SNO snoPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex];

                    CRefString sFileName = sGetFPSLogFolder() + sGetAutomatedStatusFileName();
                    FILE * pAutomatedStatus = fopen(sFileName.str(), "w");
                    fprintf(pAutomatedStatus, "CURRENT:%s.%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex, gPlayerClasses), SNOToString(SNOName(29, snoPower), FALSE).str());
                    if (ptGameTestingGlobals->nPowerIndex + 1 < ptGameTestingGlobals->tListPowers.GetArraySize())
                    {
                        SNO snoNextPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex + 1];
                        fprintf(pAutomatedStatus, "NEXT:%s.%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex, gPlayerClasses), SNOToString(SNOName(29, snoNextPower), FALSE).str());
                    }
                    else if (ptGameTestingGlobals->nClassIndex + 1 < sizeof(sgtTestClasses) / sizeof(sgtTestClasses[0]))
                    {
                        fprintf(pAutomatedStatus, "NEXT:%s\n", SymbolToName(ptGameTestingGlobals->nClassIndex + 1, gPlayerClasses));
                    }
                    fclose(pAutomatedStatus);
                }
            }
        }


        for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerCurr->idPlayerACD);
            const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
            if (!ptACDPlayer || !ptActor)
            {
                return;
            }

            WorldPlace tPlaceStart;
            Quaternion qStart;
            if (SPlayerGetStartLocation(ptACDPlayer->GetWorldLocation().idSWorld, SSCENEID_INVALID, TEAM_INVALID, (uint32)-1, tPlaceStart, qStart))
            {
                ActorSnapToPosition(ptActor->id, tPlaceStart, TRUE, TRUE);
            }
        }
    }






}

static void sAdvanceNetworking()
{

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    if (ptGameTestingGlobals->nMessageIndex < MSG_NUM_MESSAGES)
    {
        GameMessageType eMsgType = ptGameTestingGlobals->nMessageIndex;


        if (eMsgType == CMSG_TRY_CONSOLE_COMMAND || eMsgType == CMSG_START_LOGOUT_TIMER || eMsgType == CMSG_LOGOUT_NOW)
        {
            // return below: inferred from the unconditional `b +0x1384` (function exit) at line 2451's row
            ptGameTestingGlobals->nMessageIndex++;
            return;
        }

        const XTypeDescriptor *pTypeDescriptor = NULL;
        uint32 dwSize;
        const char *pszLabel;
        GetGameMessageTypeAndSize(eMsgType, pTypeDescriptor, dwSize, pszLabel);
        if (pTypeDescriptor)
        {

            if (pTypeDescriptor == DT_GENERIC_BLOB_MESSAGE)
            {
                dwSize += RandGetDWord(SGameGlobalsGet().rng) % (MAX_SINGLE_MESSAGE_SIZE - dwSize);
            }

            blz::vector<uint8> tMessage;
            tMessage.resize(dwSize);
            uint8 *pMessage = &tMessage[0];

            TypeDescriptorField field(pTypeDescriptor, NULL);
            pTypeDescriptor->Fuzz(pMessage, field, SGameGlobalsGet().rng);

            if (pTypeDescriptor == DT_GENERIC_BLOB_MESSAGE)
            {
                GenericBlobMessage *ptBlobMessage = (GenericBlobMessage *)pMessage;
                ptBlobMessage->uBlobSize = dwSize - sizeof(*ptBlobMessage);

                for (size_t i = sizeof(*ptBlobMessage); i < dwSize; ++i)
                {
                    pMessage[i] = (RandGetDWord(SGameGlobalsGet().rng) % 256);
                }
            }




            RequiredMessageHeader *ptHeader = (RequiredMessageHeader *)pMessage;
            ptHeader->eType = eMsgType;
            ptHeader->dwSize = dwSize;
            ServerMessageReceive(0, pMessage);
        }
        ptGameTestingGlobals->nMessageIndex++;
    }
    else
    {
        ptGameTestingGlobals->nMessageIndex = 1;
        ptGameTestingGlobals->nMessageIterations++;

        if (ptGameTestingGlobals->nMessageIterations >= 5000)
        {
            // Inference: sQuit ignores its subject argument and the call at +0xf38 passes none, so NULL is a stand-in.
            sQuit(NULL);
        }
    }
}






static void sGatherBadLinks(
    const SNOName & snoName,
    const uint32 dwFlags,
    void * pData)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    const SNOName & snoNameReferencer = *(const SNOName *)pData;
    if (snoName.snoHandle != SNO_NONE)
    {
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (snoName.snoHandle == ptGroupDef->GetFallbackDataSNO())
        {

            if (snoNameReferencer.snoHandle != GameDefinitionGet(snoNameReferencer.snoGroup)->GetFallbackDataSNO())
            {
                CRefString sError(CRefString().Format("Has a link to %s Axe Bad Data which is not allowed.\n", SNOGroupGetName(snoName.snoGroup)));
                GameTesting::OutputErrorEntry(ptGameTestingGlobals->fileErrors, "Link", "", SNOGroupGetName(snoNameReferencer.snoGroup), SNOFileGetName(snoNameReferencer), "Unknown", sError);
                return;
            }
        }

        if (!SNOFileExists(snoName))
        {
            CRefString sError(CRefString().Format("Missing link to %s SNO (%d)\n", SNOGroupGetName(snoName.snoGroup), snoName.snoHandle));
            GameTesting::OutputErrorEntry(ptGameTestingGlobals->fileErrors, "Link", "", SNOGroupGetName(snoNameReferencer.snoGroup), SNOFileGetName(snoNameReferencer), "Unknown", sError);
        }
    }
    else
    {

        if (dwFlags & 1)
            return;
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoName.snoGroup);
        if (snoName.snoHandle == ptGroupDef->GetFallbackDataSNO())
            return;
        CRefString sError(CRefString().Format("Has a link to %s SNO NONE which is not allowed.\n", SNOGroupGetName(snoName.snoGroup)));
        GameTesting::OutputErrorEntry(ptGameTestingGlobals->fileErrors, "Link", "", SNOGroupGetName(snoNameReferencer.snoGroup), SNOFileGetName(snoNameReferencer), "Unknown", sError);
    }


}
int32 SNOGroupGetGroups(SNOGroup * pGroups);  // X/Common/SNO/SNOGroups.cpp:111
void DefinitionIterateSnoReferencesFromFile(const SNOName & snoname, SNOIterateFunc pfuncIterate, void * pData);  // X/Common/Definitions/Definitions.cpp:6275

void sRunBadLinksTest()
{
    SNOGroup snoGroups[70];
    int32 nGroupCount = SNOGroupGetGroups(snoGroups);
    for (int32 i = 0; i < nGroupCount; i++)
    {
        SNOGroup snoGroup = snoGroups[i];
        GroupDefinition * ptGroupDef = GameDefinitionGet(snoGroup);
        if (ptGroupDef->CanReferenceOtherSNOs())
        {
            PooledSNONameList listSNOFiles(GetSnoNameListPool());
            SNOFileEnumerateSNOsForGroup(snoGroup, &listSNOFiles);

            POS pos = listSNOFiles.GetHeadPosition();
            while (pos)
            {
                SNOName snonameFile = listSNOFiles.GetNext(pos);
                DefinitionIterateSnoReferencesFromFile(snonameFile, sGatherBadLinks, &snonameFile);
            }
        }
    }
    // Inference: sQuit ignores its subject argument and the original call passes none (the compiler drops the unused argument), so NULL is a stand-in; the value is unknowable.
    sQuit(NULL);
}
#include "../Client/Definitions/PreferencesDefinition.h"
#include "../Client/Gfx/GfxAPI.h"
#include "../Client/Gfx/NX64/NX64NVN.h"
#include "../../BlizzardCore/Include/Blizzard/Hardware.h"
#include "../../BlizzardCore/Include/Blizzard/Sys/OS_API.h"
#include "../Sigma/buildver.h"
#include "Networking/XSocket.h"
namespace GameTesting {
void TriggerSkill()
{
    int32 nRuneIndex;
    SNO snoPower;

    if (!PlayerGetFirstInGame())
        return;

    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        if (!ptActor)
            return;
    }

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (ptGameTestingGlobals->tListPowers.GetArraySize() && ptGameTestingGlobals->nPowerIndex >= 0 &&
        ptGameTestingGlobals->nPowerIndex < ptGameTestingGlobals->tListPowers.GetArraySize())
    {
        snoPower = ptGameTestingGlobals->tListPowers[ptGameTestingGlobals->nPowerIndex];









        nRuneIndex = ptGameTestingGlobals->nRuneIndex;

        SNOName snonameFile(29, snoPower);
        CRefString szPowerAndRune;
        szPowerAndRune.Format("%s (Rune: %d)", SNOFileGetName(snonameFile).str(), nRuneIndex);
        ptGameTestingGlobals->sCurrentAssetGroup = SNOGroupGetName(snonameFile.snoGroup);
        ptGameTestingGlobals->sCurrentAsset = szPowerAndRune;
        TestSkill(snoPower, nRuneIndex);
    }
}









void SetupPerfSkillTest()
{

    if (!PlayerGetFirstInGame())
    {
        return;
    }


    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));
        if (!ptActor)
        {
            return;
        }
    }

    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (ptGameTestingGlobals->tListPowers.GetArraySize() == 0)
    {
        return;
    }

    if (ptGameTestingGlobals->nPowerIndex < 0 || ptGameTestingGlobals->nPowerIndex >= ptGameTestingGlobals->tListPowers.GetArraySize())
    {
        return;
    }



    CGameServerLoadingPartRequired(LSC_PLAYER_WARPED);


    SkillTestClass & tTestClass = sgtTestClasses[ptGameTestingGlobals->nClassIndex];

    DeleteAllWorldsAndTransferPlayersToLimbo();

    SWorldID idSWorldNew = SWorldCreate(GlobalSNOGet(GLOBALSNO_WORLD_FLATWORLD), TRUE, TRUE, NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, 0, 0);


    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        ActorCommonData * ptACDPlayerCurr = ACDTryToGet(ptPlayerCurr->idPlayerACD);
        if (ptACDPlayerCurr && SPlayerCanSwitchToNewWorld(ptACDPlayerCurr->id))
        {
            SWorldPlayerEnter(idSWorldNew,

                              ptPlayerCurr->idPlayerACD,
                              NULL,
                              0,
                              0,
                              D3::Hero::SavedDefinition::default_instance(),
                              D3::Account::SavedDefinition::default_instance(),
                              D3::Items::Mails::default_instance(),
                              NULL);
        }
    }
    GBID gbidClass = GlobalGBIDGet(tTestClass.eGBIDClass);





    WorldPlace tPlace;
    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayerCurr->idPlayerACD));

        ActorCommonData & tACDPlayer = ACDGet(ptPlayerCurr->idPlayerACD);

        HeroReset(ptPlayerCurr->idPlayerACD, gbidClass, 0, tTestClass.fFemale, TRUE);

        SetLevel(tACDPlayer.id, 70, FALSE);
        LearnAllSkills(tACDPlayer, 1);

        ActorFlagSet(ptActor->id, ACTOR_GOD_MODE_BIT, TRUE);
        tACDPlayer.AttributesSetInt(ATTRIBUTE_GOD, 1);

        tPlace = tACDPlayer.GetPlace();
    }


    MonsterRarity eRarity = MR_NORMAL;

    SNOName tSNOName(1, SNOFileFindBestMatch(1, "TargetDummy_Level61_NoArmor"));  // 1: the actor SNO group (no enumerator in the DWARF)
    if (tSNOName.snoHandle != SNO_NONE && MonsterExists(tSNOName))
    {
        ActorAddStruct tActorAdd;
        tActorAdd.tSNOName = tSNOName;
        tActorAdd.tPlace = tPlace;
        tActorAdd.tMarkersName = MarkerStringHash("DroppedMonster");
        tActorAdd.snoActor = tSNOName.snoHandle;
        tActorAdd.eMonsterRarity = eRarity;

        GBHandleList listGroupAffixes(GBGetHandlePool());
        EncounterAddStruct tEncounterAdd;
        EncounterAddInitialize(tEncounterAdd, listGroupAffixes);

        for (int32 i = 0; i < 50; i++)
        {
            GBAddWithEncounterAddStruct(tActorAdd, tEncounterAdd);
        }
    }


    CGameServerLoadingPartCompleted(LSC_PLAYER_WARPED);


}




void Advance()

{


    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;

    switch (ptGameTestingGlobals->eTestType)
    {
    case TESTTYPE_WORLDS:
    case TESTTYPE_WORLD_PERF:
    case TESTTYPE_OPEN_WORLD:
        sAdvanceWorlds();
        break;

    case TESTTYPE_SKILLS:
    case TESTTYPE_SKILL_PERF:
        sAdvanceSkills();
        break;

    case TESTTYPE_NETWORKING:
        sAdvanceNetworking();
        break;


    case TESTTYPE_MONSTERS:
        sAdvanceMonsters();
        break;

    // default: values 0, 7 (TESTTYPE_BADLINKS) and > 8 leave the jump table straight to line 2792 (+0x48 b.hi / table)
    default:
        break;
    }








    ptGameTestingGlobals->fFirstAdvance = FALSE;


}
}  // namespace GameTesting
#include "../Sigma/Networking/XNetAddress.h"
const char * GetCmdLine();  // X/Common/Shell/CmdLine.cpp:568
namespace Blizzard { namespace String { char * Translate(const char * source, char * dest, uint32 destSize, const char * from, const char * to); } }  // BlizzardCore/Source/System/String/String.cpp:762
namespace Blizzard { namespace Process { char * GetComputerName(char * computerName, int maxNameSize); } }  // BlizzardCore/Source/System/Process/Process_NX64.cpp:37


void WritePerformanceLogHeader(GamePerfFile * pFile)
{












    uint8 UTF8_BOM[3] = { 0xEF, 0xBB, 0xBF };
    pFile->Write(UTF8_BOM, sizeof(UTF8_BOM));


    pFile->Printf("{\n");

    char tempBuffer[2048];


    pFile->Printf("\t\"build_info\" : {\n");
    pFile->Printf("\t\t\"project\" : %d,\n", 36);
    pFile->Printf("\t\t\"build\" : %d,\n", D3BuildNum());
    pFile->Printf("\t\t\"version\" : { \"major\" : %d, \"minor\" : %d, \"revision\" : %d },\n", D3VersionMajor(), D3VersionMinor(), D3VersionRevision());
    pFile->Printf("\t\t\"code_revision\" : %d,\n", D3CodeVersion());
    pFile->Printf("\t\t\"data_revision\" : %d,\n", D3DataVersion());
    pFile->Printf("\t\t\"code_branch\" : \"%s\",\n", D3CodeBranch());
    pFile->Printf("\t\t\"data_branch\" : \"%s\",\n", D3DataBranch());
    pFile->Printf("\t\t\"configuration\" : \"%s\",\n", "Release");

    Blizzard::String::Translate(GetCmdLine(), tempBuffer, sizeof(tempBuffer), "\"", "\\\"");
    pFile->Printf("\t\t\"commandline\" : \"%s\"", tempBuffer);



    pFile->Printf("\n\t},\n\n");


    const VideoPrefs & tVideoPrefs = Prefs::GetVideoPrefs();
    const DisplayMode & displayMode = tVideoPrefs.tVideoPrefsToConfirm.tDisplayMode;
    pFile->Printf("\t\"preferences\" : {\n");
    pFile->Printf("\t\t\"resolution\" : { \"width\" : %d, \"height\" : %d },\n", displayMode.dwWidth, displayMode.dwHeight);
    switch (displayMode.dwWindowMode)
    {   case WINDOWMODE_FULLSCREEN: pFile->Printf("\t\t\"window_mode\" : \"fullscreen\",\n"); break;

    case WINDOWMODE_WINDOWED:
        pFile->Printf("\t\t\"window_mode\" : \"windowed\",\n");
        break;
    case WINDOWMODE_FULLSCREEN_WINDOWED:
        pFile->Printf("\t\t\"window_mode\" : \"windowed_fullscreen\",\n");
        break;
    }
    pFile->Printf("\t\t\"texture_quality\" : %d,\n", tVideoPrefs.tVideoPrefsToConfirm.nMipOffset);
    pFile->Printf("\t\t\"shadow_quality\" : %d,\n", tVideoPrefs.GetShadowQuality());
    pFile->Printf("\t\t\"physics_quality\" : %d,\n", tVideoPrefs.nPhysicsQuality);
    pFile->Printf("\t\t\"clutter_quality\" : %d,\n", tVideoPrefs.nClutterQuality);
    pFile->Printf("\t\t\"fsaa\" : %s,\n", tVideoPrefs.bAntialiasing ? "true" : "false");
    pFile->Printf("\t\t\"letterbox\" : %s,\n", tVideoPrefs.bLetterbox ? "true" : "false");
    pFile->Printf("\t\t\"color_correction\" : %s,\n", tVideoPrefs.bColorCorrection ? "true" : "false");
    pFile->Printf("\t\t\"vsync\" : %s,\n", tVideoPrefs.tVideoPrefsToConfirm.bVsync ? "true" : "false");
    pFile->Printf("\t\t\"gamma\" : %f\n", tVideoPrefs.flGamma);
    pFile->Printf("\t},\n\n");

    OS_API::OSInfo osInfo;
    Blizzard::Hardware::ProcessorFeatures cpuInfo;
    char cpuBrandString[64];
    char cpuVendorString[32];
    char systemModel[256];
    char systemManufacturer[256];
    char systemName[256];
    uint64 cpuSpeed;
    GfxDeviceDesc deviceDesc;
    XNetAddress tAddress;


    OS_API::GetOSInfo(&osInfo);




    Blizzard::Hardware::ExtendedProcessorFeatures(cpuInfo);
    Blizzard::Hardware::GetProcessorVendorString(cpuVendorString, sizeof(cpuVendorString));
    Blizzard::Hardware::GetProcessorBrandString(cpuBrandString, sizeof(cpuBrandString));
    cpuSpeed = Blizzard::Hardware::CpuSpeed();
    Blizzard::Hardware::GetSystemManufacturer(systemManufacturer, sizeof(systemManufacturer));
    Blizzard::Hardware::GetSystemModel(systemModel, sizeof(systemModel));
    Blizzard::Process::GetComputerName(systemName, sizeof(systemName));
    gptGFXAPI->GetDeviceDescription(deviceDesc);

    XSocketGetMyIPAddress(tAddress);
    CRefString sIPAdress = tAddress.ToString(FALSE);


    pFile->Printf("\t\"system_info\" : {\n");

    pFile->Printf("\t\t\"general\" : {\n");
    pFile->Printf("\t\t\t\"ip_address\" : \"%s\",\n", tAddress.ToString(FALSE).str());
    Blizzard::String::Translate(systemName, tempBuffer, sizeof(tempBuffer), "\"", "\\\"");
    pFile->Printf("\t\t\t\"system_name\" : \"%s\",\n", tempBuffer);
    Blizzard::String::Translate(systemModel, tempBuffer, sizeof(tempBuffer), "\"", "\\\"");
    pFile->Printf("\t\t\t\"system_model\" : \"%s\",\n", tempBuffer);
    Blizzard::String::Translate(systemManufacturer, tempBuffer, sizeof(tempBuffer), "\"", "\\\"");
    pFile->Printf("\t\t\t\"system_manufacturer\" : \"%s\"", tempBuffer);



    pFile->Printf("\n\t\t},\n");

    pFile->Printf("\t\t\"os\" : {\n");
    pFile->Printf("\t\t\t\"name\" : \"%s\",\n", osInfo.productName);
    pFile->Printf("\t\t\t\"major_version\" : %u,\n", osInfo.versionMajor);
    pFile->Printf("\t\t\t\"minor_version\" : %u,\n", osInfo.versionMinor);
    pFile->Printf("\t\t\t\"revision\" : %u,\n", osInfo.versionRevision);
    pFile->Printf("\t\t\t\"build\" : \"%s\",\n", osInfo.build);
    pFile->Printf("\t\t\t\"64bit\" : %s,\n", osInfo.is64Bit ? "true" : "false");
    pFile->Printf("\t\t\t\"service_pack_major\" : %u,\n", osInfo.servicePackMajor);
    pFile->Printf("\t\t\t\"service_pack_minor\" : %u\n", osInfo.servicePackMinor);
    pFile->Printf("\t\t},\n");

    pFile->Printf("\t\t\"cpu\" : {\n");
    pFile->Printf("\t\t\t\"vendor\" : \"%s\",\n", cpuVendorString);
    pFile->Printf("\t\t\t\"brand\" : \"%s\",\n", cpuBrandString);
    pFile->Printf("\t\t\t\"model\" : %u,\n", cpuInfo.model);
    pFile->Printf("\t\t\t\"extmodel\" : %u,\n", cpuInfo.extmodel);
    pFile->Printf("\t\t\t\"family\" : %u,\n", cpuInfo.family);
    pFile->Printf("\t\t\t\"extfamily\" : %u,\n", cpuInfo.extfamily);
    pFile->Printf("\t\t\t\"stepping\" : %u,\n", cpuInfo.stepping);
    pFile->Printf("\t\t\t\"signature\" : %u,\n", cpuInfo.signature);

    pFile->Printf("\t\t\t\"speed\" : \"%lu\",\n", cpuSpeed);



    pFile->Printf("\t\t\t\"num_processors\" : %d,\n", cpuInfo.packageCount);
    pFile->Printf("\t\t\t\"num_cores\" : %d,\n", cpuInfo.numCores);
    pFile->Printf("\t\t\t\"num_threads\" : %d\n", cpuInfo.numHWThreads);
    pFile->Printf("\t\t},\n");

    pFile->Printf("\t\t\"graphics\" : {\n");
    pFile->Printf("\t\t\t\"device\" : \"%s\",\n", deviceDesc.deviceDescription);
    pFile->Printf("\t\t\t\"vendor_id\" : %u,\n", deviceDesc.vendorID);
    pFile->Printf("\t\t\t\"device_id\" : %u,\n", deviceDesc.deviceID);
    pFile->Printf("\t\t\t\"api_description\" : \"%s\",\n", deviceDesc.apiDescription);
    pFile->Printf("\t\t\t\"driver_version\" : \"%s\",\n", deviceDesc.driverVersion);

    pFile->Printf("\t\t\t\"video_mem\" : \"%lu\",\n", deviceDesc.videoMemSize);
    pFile->Printf("\t\t\t\"agp_mem\" : \"%lu\"\n", deviceDesc.agpMemSize);




    pFile->Printf("\t\t}\n");

    pFile->Printf("\t},\n\n");


    pFile->Printf("\t\"collections\" : [\n");
    pFile->Flush();

}


inline void WritePerformanceLogFooter(GamePerfFile * pFile)
{
    pFile->Printf("\n\t]\n");
    pFile->Printf("}\n");
    pFile->Flush();
}

QuestInstance * GetLeadingMainLineQuest();  // X/Common/Quests/Quests.cpp:1020





void WritePerformanceLogStartCollectionWorld(GamePerfFile * pFile, SNO snoScene)
{





    CRefString sSceneName;
    CRefString sActName;
    CRefString sClass;
    CRefString sWorldName;
    int32 nQuestStepUID = -1;
    CRefString sQuestName;
    int32 dwSeed = 0;
    Vector3D worldPosition(0.0f, 0.0f, 0.0f);

    ACDID idACDPlayer = LocalPlayerACDGet();
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer)
    {
        RActorID idPlayerRActor = LocalPlayerRActorGet();
        if (idPlayerRActor != RACTORID_INVALID)
        {
            const RActor * ptRActor = RActorTryToGet(idPlayerRActor);
            if (ptRActor)
            {
                SNOName nameScene(SCENE_SNO, snoScene);
                sSceneName = SNOFileGetName(nameScene);
                Act eAct = GameGetCurrentAct();
                sActName = SymbolToName(eAct, gActs);
                sClass = SymbolToName(PlayerGetPlayerClass(ptACDPlayer), gPlayerClasses);
                WorldPlace tPlace = ptACDPlayer->GetPlace();
                worldPosition = tPlace.wp;
                sWorldName = SNOToString(WORLD_SNO, WorldCommonGetWorldSNO(tPlace.idSWorld), FALSE);
                dwSeed = ptACDPlayer->AttributesGetInt(FastAttribKey(ATTRIBUTE_WORLD_SEED));
                QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
                if (ptQuestInstance)
                {
                    sQuestName = SNOToString(QUEST_SNO, ptQuestInstance->snoQuest, FALSE);
                    nQuestStepUID = ptQuestInstance->nCurrentStepUID;
                }
            }
        }
    }
    Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(utcTime, timeRec);
    CRefString datetimeStr;
    datetimeStr.Format("%04d-%02d-%02d %02d:%02d:%02d.%09u UTC", timeRec.year, timeRec.month, timeRec.day,
                       timeRec.hour, timeRec.minute, timeRec.second, timeRec.nanosecond);
    pFile->Printf("\t\t{\n");
    pFile->Printf("\t\t\t\"datetime\" : \"%s\",\n", datetimeStr.str());
    pFile->Printf("\t\t\t\"seed\" : %d,\n", dwSeed);
    pFile->Printf("\t\t\t\"act\" : \"%s\",\n", sActName.str());
    pFile->Printf("\t\t\t\"world\" : \"%s\",\n", sWorldName.str());
    pFile->Printf("\t\t\t\"scene\" : \"%s\",\n", sSceneName.str());
    pFile->Printf("\t\t\t\"quest\" : \"%s\",\n", sQuestName.str());
    pFile->Printf("\t\t\t\"quest_step\" : %d,\n", nQuestStepUID);
    pFile->Printf("\t\t\t\"location\" : { \"x\" : %f, \"y\" : %f, \"z\" : %f },\n", worldPosition.x, worldPosition.y, worldPosition.z);
    pFile->Printf("\t\t\t\"class\" : \"%s\",\n", sClass.str());
    pFile->Printf("\t\t\t\"samples\" : [\n");

}

void WritePerformanceLogStartCollectionSkill(GamePerfFile * pFile, SNO snoPower, int32 nRuneIndex)
{
    CRefString sPowerName;
    CRefString sClass;

    ACDID idACDPlayer = LocalPlayerACDGet();
    if (idACDPlayer != ACDID_INVALID)
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
        if (ptACDPlayer)
        {
            RActorID idPlayerRActor = LocalPlayerRActorGet();
            if (idPlayerRActor != RACTORID_INVALID)
            {
                const RActor * ptRActor = RActorTryToGet(idPlayerRActor);
                if (ptRActor)
                {
                    SNOName namePower(29, snoPower);
                    sPowerName = SNOFileGetName(namePower);
                    sClass = SymbolToName(PlayerGetPlayerClass(ptACDPlayer), gPlayerClasses);
                }
            }
        }
    }

    Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(utcTime, timeRec);
    CRefString datetimeStr;
    datetimeStr.Format("%04d-%02d-%02d %02d:%02d:%02d.%09u UTC", timeRec.year, timeRec.month, timeRec.day,
                       timeRec.hour, timeRec.minute, timeRec.second, timeRec.nanosecond);

    pFile->Printf("\t\t{\n");
    pFile->Printf("\t\t\t\"datetime\" : \"%s\",\n", datetimeStr.str());
    pFile->Printf("\t\t\t\"power\" : \"%s\",\n", sPowerName.str());
    pFile->Printf("\t\t\t\"runeindex\" : \"%d\",\n", nRuneIndex);
    pFile->Printf("\t\t\t\"class\" : \"%s\",\n", sClass.str());
    pFile->Printf("\t\t\t\"samples\" : [\n");
}
namespace GameTesting {


}  // namespace GameTesting
inline void WritePerformanceLogStopCollection(GamePerfFile * pFile, BOOL lastCollection)
{
    pFile->Printf("\n\t\t\t]\n");
    if (lastCollection)
        pFile->Printf("\t\t}\n");
    else
        pFile->Printf("\t\t},\n");
}
namespace GameTesting {









}  // namespace GameTesting
CRefString sGetFPSLogFolder()
{
    CRefString sFolderPath(Blizzard::String::QuickFormat<1024>("%s\\FPS Logs", ErrorManagerGetReportingFolder()));
    FileReference tRef;
    FileReferenceInit(tRef, sFolderPath);
    FileDirectoryCreate(tRef, FALSE);
    return sFolderPath + CRefString("\\");
}
namespace GameTesting {












BOOL StartWorldFPSCollection(LPCSTR szFilename, SNO snoScene, uint32 nDumpFPSFreq, BOOL bDumpFPSFormatJSON)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    GamePerfStats & tPerfStats = GamePerfStatsGet();

    if (tPerfStats.fileDumpFPS)
    {
        if (tPerfStats.bDumpFPSFormatJSON)
            WritePerformanceLogStopCollection(tPerfStats.fileDumpFPS, FALSE);

        delete tPerfStats.fileDumpFPS;
        tPerfStats.fileDumpFPS = NULL;
        tPerfStats.nDumpFPSFreq = nDumpFPSFreq;
        tPerfStats.nDumpFPSCount = 0;
        tPerfStats.bFPSCollectionStart = FALSE;
        tPerfStats.bDumpFPSStart = FALSE;
        tPerfStats.bDumpFPSFormatJSON = FALSE;
    }

    CRefString sFileName = sGetFPSLogFolder() + CRefString().Format("%s.txt", szFilename);
    ptGameTestingGlobals->sCurrentReportFilename = szFilename;

    Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::Timestamp localTime = Blizzard::Time::LocalBias(utcTime);
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(localTime, timeRec);

    tPerfStats.fileDumpFPS = new GamePerfFile(sFileName.str());
    if (!tPerfStats.fileDumpFPS->IsOpen())
    {
        delete tPerfStats.fileDumpFPS;
        tPerfStats.fileDumpFPS = NULL;










        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    tPerfStats.fCollectPerfCounters = TRUE;
    tPerfStats.bFPSCollectionStart = TRUE;
    tPerfStats.bDumpFPSStart = TRUE;
    tPerfStats.bDumpFPSFormatJSON = bDumpFPSFormatJSON;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    tAppGlobals.eDebugDisplayMode = DDM_FPS_QA;
    tPerfStats.flMinAvgFPS = FLT_MAX;
    tPerfStats.nDumpFPSFreq = nDumpFPSFreq;
    tPerfStats.nDumpFPSCount = 0;

    if (tPerfStats.bDumpFPSFormatJSON)
    {
        WritePerformanceLogHeader(tPerfStats.fileDumpFPS);
        WritePerformanceLogStartCollectionWorld(tPerfStats.fileDumpFPS, snoScene);
    }
    return TRUE;
}




BOOL StartSkillFPSCollection(LPCSTR szFilename, SNO snoPower, int32 nRuneIndex, uint32 nDumpFPSFreq, BOOL bDumpFPSFormatJSON)
{
    GamePerfStats & tPerfStats = GamePerfStatsGet();

    if (tPerfStats.fileDumpFPS)
    {
        if (tPerfStats.bDumpFPSFormatJSON)
            WritePerformanceLogStopCollection(tPerfStats.fileDumpFPS, FALSE);

        delete tPerfStats.fileDumpFPS;
        tPerfStats.fileDumpFPS = NULL;
        tPerfStats.nDumpFPSFreq = nDumpFPSFreq;
        tPerfStats.nDumpFPSCount = 0;
        tPerfStats.bFPSCollectionStart = FALSE;
        tPerfStats.bDumpFPSStart = FALSE;
        tPerfStats.bDumpFPSFormatJSON = FALSE;
    }

    CRefString sFileName = sGetFPSLogFolder() + CRefString().Format("%s.txt", szFilename);

    Blizzard::Time::Timestamp utcTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::Timestamp localTime = Blizzard::Time::LocalBias(utcTime);
    Blizzard::Time::TimeRec timeRec;
    Blizzard::Time::BreakTime(localTime, timeRec);

    tPerfStats.fileDumpFPS = new GamePerfFile(sFileName.str());
    tPerfStats.fCollectPerfCounters = TRUE;
    tPerfStats.bFPSCollectionStart = TRUE;
    tPerfStats.bDumpFPSStart = TRUE;
    tPerfStats.bDumpFPSFormatJSON = bDumpFPSFormatJSON;

    AppGlobals & tAppGlobals = sgtAppGlobals;
    tAppGlobals.eDebugDisplayMode = DDM_FPS_QA;
    tPerfStats.nDumpFPSFreq = nDumpFPSFreq;
    tPerfStats.nDumpFPSCount = 0;

    if (tPerfStats.bDumpFPSFormatJSON)
    {
        WritePerformanceLogHeader(tPerfStats.fileDumpFPS);
        WritePerformanceLogStartCollectionSkill(tPerfStats.fileDumpFPS, snoPower, nRuneIndex);
    }
    return TRUE;
}
























void StopFPSCollection(BOOL finishLog)
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    GamePerfStats & tPerfStats = GamePerfStatsGet();
    AppGlobals & tAppGlobals = sgtAppGlobals;

    if (tPerfStats.fileDumpFPS)
    {
        if (tPerfStats.bDumpFPSFormatJSON)
        {
            WritePerformanceLogStopCollection(tPerfStats.fileDumpFPS, finishLog);
            if (finishLog)
                WritePerformanceLogFooter(tPerfStats.fileDumpFPS);
        }

        delete tPerfStats.fileDumpFPS;
        tPerfStats.bDumpFPSFormatJSON = FALSE;
        tPerfStats.nDumpFPSFreq = 0;
        tPerfStats.nDumpFPSCount = 0;
        tPerfStats.bFPSCollectionStart = FALSE;
        tPerfStats.bDumpFPSStart = FALSE;
        tPerfStats.fileDumpFPS = NULL;

        if (ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF &&
            (float)ptGameTestingGlobals->dwMinFPSToTriggerScreenshot > tPerfStats.flMinAvgFPS)
        {
            CRefString sFileName = CRefString().Format("%s.jpg", ptGameTestingGlobals->sCurrentReportFilename.str());
            GfxScreenshot(sGetFPSLogFolder(), sFileName, CPlayerGetScreenshotComment(FALSE), 4, 100);
        }
    }

    tAppGlobals.eDebugDisplayMode = DDM_DISABLED;
    tPerfStats.fCollectPerfCounters = FALSE;
}












inline const char * TestTypeToString(
    TestType eTestType)
{
    switch (eTestType)
    {
    case TESTTYPE_WORLDS:
        return "Worlds";
    case TESTTYPE_MONSTERS:
        return "Monsters";
    case TESTTYPE_SKILLS:
        return "Skills";
    case TESTTYPE_NETWORKING:
        return "Networking";
    case TESTTYPE_WORLD_PERF:
        return "WorldPerf";
    case TESTTYPE_SKILL_PERF:
        return "SkillPerf";
    case TESTTYPE_BADLINKS:
        return "BadLinks";
    case TESTTYPE_OPEN_WORLD:
        return "OpenWorld";
    default:
        return "INVALIDTESTTYPE";
    }
}






BOOL IsAutomatedBNetTesting()
{
    GameTestingGlobals *& ptGameTestingGlobals = GameGetActiveGameCommonData().ptGameTestingGlobals;
    if (gtCmdLineParams.eTestType == TESTTYPE_WORLD_PERF)
        return TRUE;
    return ptGameTestingGlobals->eTestType == TESTTYPE_WORLD_PERF;
}








}  // namespace GameTesting
static inline bool sFilterAct(const SNO snoActFilter, const TestSceneArray & tListTestScenes, const Act eAct)
{
    SNO snoAct = ActSNOFromActEnum(eAct);
    if (snoActFilter != SNO_NONE && snoActFilter != snoAct)
        return false;
    if (tListTestScenes.empty())
        return true;
    for (uint32 i = 0; i < tListTestScenes.size(); i++)
    {
        if (tListTestScenes[i].snoAct == snoAct)
            return true;
    }
    return false;
}
namespace GameTesting {














}  // namespace GameTesting
static inline bool sFilterWorld(const SNO snoWorldFilter, const TestSceneArray & tListTestScenes, const SNO snoWorld, const Act eAct)
{
    SNO snoAct = ActSNOFromActEnum(eAct);
    if (snoWorldFilter != SNO_NONE && snoWorldFilter != snoWorld)
        return false;
    if (tListTestScenes.empty())
        return true;
    for (uint32 i = 0; i < tListTestScenes.size(); i++)
    {
        if (tListTestScenes[i].snoAct == snoAct && tListTestScenes[i].snoWorld == snoWorld)
            return true;
    }
    return false;
}
namespace GameTesting {
}  // namespace GameTesting
// Position inferred: the DWARF has the ldn statics (ldn_Ipv4Address.h:153/158/163, ldn_MacAddress.h:101/106) last in _GLOBAL__sub_I_GameTesting.cpp.
#include "../../NintendoSDK/Include/nn/ldn/ldn_Ipv4Address.h"
#include "../../NintendoSDK/Include/nn/ldn/ldn_MacAddress.h"













static bool sFilterScene(
    const SNO snoSceneFilter,
    const TestSceneArray & tListTestScenes,
    const Act eAct,
    const SNO snoWorld,
    const SNO snoScene)
{
    SNO snoAct = ActSNOFromActEnum(eAct);
    if (snoSceneFilter != SNO_NONE && snoSceneFilter != snoScene)
        return false;



    if (tListTestScenes.empty())
        return true;



    for (uint32 i = 0; i < tListTestScenes.size(); i++)
    {
        const TestScene & tScene = tListTestScenes[i];
        if (tScene.snoAct == snoAct &&
            tScene.snoWorld == snoWorld &&
            tScene.snoScene == snoScene) return true;
    }
    return false;
}
