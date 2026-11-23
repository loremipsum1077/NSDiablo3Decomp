// X/Server/LuaScript/LuaScript.cpp -- reconstructed from the 2.6.2 DWARF and instruction listings.
// Functions sit on their original lines (asserts and logs embed __LINE__).

#include "LuaScript.h"
#include "LuaScriptUtil.h"
#include "ScriptEvent.h"
#include "LuaAbortThreadWatcher.h"
#include "../SGameManager.h"
#include "../../Common/Game.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/WorldPlace.h"
#include "../Actors/Actors.h"
#include "../SGame.h"
#include "../SScene.h"
#include "ScriptEventParams.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Common/Hyperlink.h"
#include "../SConductor.h"
#include "../../Common/SavedData.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../Sigma/SigmaTiming.h"
#include "LuaHook.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../Definitions/ScriptDefinition.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Stopwatch.h"
#include "../../Sigma/DataStructures/ChainedDataArrayTemplated.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/DataStructures/XGrowableMap.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include "../../Sigma/DataStructures/XVar.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"

struct LuaScriptAllocator;
#include "../Definitions/ConductorDefinition.h"
#include "ScriptEventSwig.h"
#include "../AI/ActorAI.h"
#include "../../Common/Shell/CmdLine.h"
#include "../SGameConstants.h"

// DWARF SConductor.h:78; shared header omits this type (also defined locally in SConductor.cpp).
struct ConductorGlobals {
    UpdateData tConductorUpdateData;
    SNOArray tConductorCompletedArray;
    XListMemoryPool<ConductorInstance> tConductorInstancePool;
    XPooledList<ConductorInstance, ConductorInstance> tConductorInstanceList;
    XListMemoryPool<RepeatableScriptInstanceData> tRepeatableScriptInstanceDataPool;
    XMapMemoryPool<ACDID, int> tMapRefACDsPool;
};
static_assert(sizeof(ConductorGlobals) == 0x110, "ConductorGlobals");
// DWARF SConductor.cpp:904/947.
void SConductorAdvancePhase(ConductorInstance &);
void SConductorSkipToPhase(ConductorInstance &, LPCSTR);
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../../Contrib/Contrib/lua/src/lualib.h"
// DWARF LuaScriptUtil.cpp:495 and LuaScriptHydraLib.cpp:1291.
LuaScriptAllocator * LuaScriptAllocInitializeForGame();
void RegisterHydraLib(lua_State *);
#include "../Powers/Powers.h"
#include "../../Common/Shell/Shell.h"
// DWARF LuaScriptUtil.cpp:507/518.
void LuaScriptAllocDisposeForGame(LuaScriptAllocator *);
void LuaScriptAllocSetToShutdown(LuaScriptAllocator *);
// DWARF LuaScript.cpp:3793.
static void sScriptCloseAndFreeLuaStateProfiled(lua_State *, lua_State *, LPCSTR);





static XVarScope s_scope("LuaScript", NULL);
static XVarBool s_varEnableHaltOnExpensivePowers(s_scope, "EnableHaltOnExpensivePowers", false);

enum {
    MAX_SCRIPTSTATES = 2048,
    MAX_SCRIPTINSTANCES = 256,
    HIGH_SCRIPT_INSTANCE_THRESHOLD = 128,
    MAX_TRACE_SCRIPT_CALLBACKS = 16,
    MAX_WAITING_EVENTS = 8,
    MAX_SCRIPT_EVENT_HANDLERS = 64,
    MAX_ENTRY_POINT_NAME_LENGTH = 64,
    MAX_REFERENCED_ACDID = 64,
    COMPILED_SCRIPT_GROWTH_RATE = 256,
    BLOCK_COUNT_GAME_REFERENCED_ACDID = 512,
};

struct ScriptState {  // size 0x130
    ScriptID id;  // 0x00
    lua_State * ptLuaState;  // 0x08
    lua_State * ptParentLuaState;  // 0x10
    SNO snoScript;  // 0x18
    int32 nNumEventHandlers;  // 0x1c
    ScriptEventListenerDefinitionID arEventHandlers[64];  // 0x20
    uint32 dwLoadMachineTime;  // 0x120
    uint16 nAdventureParentIndex;  // 0x124
    BOOL bConductor;  // 0x128
};
static_assert(sizeof(ScriptState) == 0x130, "ScriptState");

typedef TDataArrayIterator<ScriptState> ScriptStateIterator;
typedef TDataArray<ScriptState, int> ScriptStateDataArray;
typedef blz::vector<ScriptState *, blz::allocator<ScriptState *> > ScriptStatesPointerArray;

struct CompiledScriptData {  // size 0x10
    size_t nSize;  // 0x00
    uint8 * pCompiledScript;  // 0x08
};
static_assert(sizeof(CompiledScriptData) == 0x10, "CompiledScriptData");

typedef XGrowableMap<int, int, CompiledScriptData, CompiledScriptData> SNOCompiledScriptMap;
typedef XGrowableMap<int, int, int, int> SNOScriptIDMap;

enum ScriptInstanceWaitType {
    ScriptInstanceWaitNone = 0,
    ScriptInstanceWaitForAnyEvent = 1,
    ScriptInstanceWaitForAllEvents = 2,
};

typedef XPooledMap<ACDID, ACDID, int, int, 16> MapRefACDs;

struct ScriptInstance {  // size 0x148
    ScriptInstanceID id;  // 0x00
    ScriptID idScript;  // 0x04
    lua_State * ptLuaState;  // 0x08
    uint32 dwStartedTick;  // 0x10
    uint32 dwResumeTick;  // 0x14
    int32 nResumeArgCnt;  // 0x18
    int32 nResumeRetValCnt;  // 0x1c
    ScriptInstanceWaitType eWaitType;  // 0x20
    CHAR szEntryPoint[64];  // 0x24
    AIQueuedActionID idLastQueuedAIAction;  // 0x64
    MapRefACDs mapRefACDs;  // 0x68
    BOOL bFailedToFindAndPurgeOldRefACDs;  // 0x110
    DataID idAttachment;  // 0x114
    ScriptAttachmentType eAttachmentType;  // 0x118
    BOOL bAttachmentDeleted;  // 0x11c
    int32 nNumWaitingEvents;  // 0x120
    ScriptEventWaitListenerID arListeners[8];  // 0x124
};
static_assert(sizeof(ScriptInstance) == 0x148, "ScriptInstance");

typedef TDataArray<ScriptInstance, int> ScriptInstanceDataArray;
typedef TDataArrayIterator<ScriptInstance> ScriptInstanceIterator;

struct ScriptAttachment {  // size 0x18
    ScriptAttachmentType eAttachType;  // 0x00
    DataID idAttachment;  // 0x04
    uint64 dwDisabledEvents;  // 0x08
    BOOL bFlaggedForDeletion;  // 0x10
    BOOL operator==(const ScriptAttachment &);
};
static_assert(sizeof(ScriptAttachment) == 0x18, "ScriptAttachment");

typedef XPooledList<ScriptAttachment, ScriptAttachment &> PooledScriptAttachmentList;
typedef XListMemoryPool<ScriptAttachment> ScriptAttachmentListPool;
typedef XGrowableMap<int, int, XPooledList<ScriptAttachment, ScriptAttachment &> *, XPooledList<ScriptAttachment, ScriptAttachment &> *> SNOScriptAttachmentListMap;
typedef XGrowableMap<ACDID, ACDID, int, int> ACDIDIntMap;
typedef XGrowableMap<unsigned int, unsigned int, double, double> ScriptGlobalVarsMap;
typedef XMapMemoryPool<ACDID, int> MapRefACDPool;

typedef void (*TraceScriptCallback)(LPCSTR, SNOName, void *);  // LuaScript.h

struct LuaScriptAppGlobals {  // size 0x108
    int32 nNumTraceScriptCallbacks;  // 0x00
    TraceScriptCallback arTraceScriptCallbacks[16];  // 0x08
    void * arTraceScriptCallbacksData[16];  // 0x88
};
static_assert(sizeof(LuaScriptAppGlobals) == 0x108, "LuaScriptAppGlobals");

struct DetachQueueEntry {  // size 0xc
    ScriptAttachmentType eType;  // 0x00
    DataID idAttachment;  // 0x04
    SNO snoScript;  // 0x08
};
static_assert(sizeof(DetachQueueEntry) == 0xc, "DetachQueueEntry");

typedef blz::vector<DetachQueueEntry, blz::allocator<DetachQueueEntry> > DetachQueue;
typedef XGrowableMap<ACDID, ACDID, lua_State *, lua_State *> ACDIDLuaStateMap;

struct LuaScriptGameGlobals {  // size 0xa0
    lua_State * ptHydraState;  // 0x00
    lua_State * ptConsoleState;  // 0x08
    ScriptStateDataArray * ptScriptStates;  // 0x10
    SNOScriptIDMap * mapSNOScriptID;  // 0x18
    SNOCompiledScriptMap * mapSNOCompiledScripts;  // 0x20
    LuaScriptAllocator * ptLuaScriptAlloc;  // 0x28
    ScriptInstanceDataArray * ptScriptInstances;  // 0x30
    ScriptAttachmentListPool * ptScriptAttachmentPool;  // 0x38
    SNOScriptAttachmentListMap * ptScriptAttachmentMap;  // 0x40
    ACDIDIntMap * ptInUseACDs;  // 0x48
    ScriptGlobalVarsMap * ptScriptGlobalVars;  // 0x50
    DetachQueue tDetachQueue;  // 0x58
    ACDIDLuaStateMap * ptACDScriptStates;  // 0x70
    ScriptStatesPointerArray * ptRangeEventScriptStates;  // 0x78
    TChainedDataArray<SaveBlock, int> * ptInactiveSaveBlockArray;  // 0x80
    uint32 dwStartOfCurrentScript;  // 0x88
    MapRefACDPool * ptMapRefACDsPool;  // 0x90
    BOOL bConductorFlaggedForAdvancementThisUpdate;  // 0x98
};
static_assert(sizeof(LuaScriptGameGlobals) == 0xa0, "LuaScriptGameGlobals");

struct MyLuaReader {  // size 0x10
    const ScriptDefinition * pScriptDef;  // 0x00
    BOOL bReadCompleted;  // 0x08
};
static_assert(sizeof(MyLuaReader) == 0x10, "MyLuaReader");

static ScriptID sScriptLookupScriptID(SNO snoScript);
static ScriptState * sGetScriptStateWithScriptID(ScriptID idScript);
static ScriptState * sGetScriptStateWithScriptInstanceID(ScriptInstanceID idScriptInstance);
static ScriptInstance * sGetScriptInstance(ScriptInstanceID idScriptInstance);
static ScriptState * sTryGetScriptStateWithScriptID(ScriptID idScript);
static lua_State * sAllocNewLuaState(lua_State * ptParentState, LPCSTR szStateName);
static void sServerLuaHook(lua_State * L, lua_Debug * ar);
void LuaSetInitializingScript(LPCSTR sScriptName);
void LuaClearInitializingScript();
void * LuaScriptAlloc(void * ud, void * ptr, size_t osize, size_t nsize);  // LuaScriptUtil.cpp:529
extern LuaHook * gpLuaHook;  // LuaHook.cpp:18
static void sDetachQueuedEntries();
ScriptID LuaScriptGetScriptID(SNO snoScript);
void ScriptDefinitionDisposeScript(SNO snoScript);  // X/Server/Definitions/ScriptDefinition.cpp:906
namespace GameTesting { int32 GetTestType(); }  // X/Common/GameTesting.cpp:625 (returns TestType)
static void sScriptCloseAndFreeScriptState(ScriptState * ptScriptState);
static void sRemoveRefACDID(ACDID idACD);
static void sAddRefACDID(ACDID idACD);
void SConductorInstanceAddRefACD(ConductorInstance * ptConductorInstance, ACDID idACD);  // X/Server/SConductor.cpp:1242
void SConductorInstanceRemoveRefACD(ConductorInstance * ptConductorInstance, ACDID idACD);  // X/Server/SConductor.cpp:1285
static void sScriptRemoveFromMap(ScriptState * ptScriptState);
static void sScriptCloseAndFreeLuaState(lua_State * ptLuaState, lua_State * ptLuaParentState, LPCSTR szStateName);
void LuaScriptGetExecutingStackString(CRefString & sMsg);
void LuaScriptDetach(ScriptAttachmentType eType, DataID idAttachment, SNO snoScript);
SNO LuaScriptGetSNOForLuaState(lua_State * ptLuaState);
BOOL SQuestsIsQuestTriggerTargetingActor(ACDID idActor);  // X/Server/Quests/SQuests.cpp
BOOL IsScriptTriggerEventTargetingActor(ACDID idActor);  // X/Server/LuaScript/ScriptEvent.cpp:2160
static void sLuaTickScriptInstance(ScriptInstance * ptScriptInstance);
static void sScriptCloseInstance(ScriptInstance * ptScriptInstance);
BOOL LuaScriptScriptIsExecuting(ScriptInstanceID idScript);
void LuaScriptFreeStateWithScriptID(ScriptID idScript);

static void sScriptAllocAndLoadScriptState(ScriptState * ptScriptState); // Same-file static definition at original line 4782.
static void sScriptLoadScriptState(ScriptState * ptScriptState); // Same-file static definition at original line 5099.
// Exhaustive raw search finds CPP records only: LuaScriptUtil.cpp:1084 and ScriptEvent.cpp:2818, with no recovered header declaration.
int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName);
int32 ScriptEventInitializeForState(SNO snoScript, ScriptID idScript, lua_State * ptLuaState, ScriptEventListenerDefinitionID * arEventHandlers, int32 nMaxEventHandlers);

































static void sServerLuaHook(lua_State * L, lua_Debug * ar)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;


    ScriptInstanceID idExecutingScript = LuaScriptGetExecutingScriptInstance();
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idExecutingScript);
    if (ptScriptInstance != NULL && ptScriptInstance->bAttachmentDeleted)
    {
        CRefString sError;
        sError.Format("Aborting LUA script: Attachment has been deleted!\n");


        lua_pushstring(L, sError.str());
        lua_error(L);
        return;
    }



    uint32 dwMaxLuaTickTime = GameTesting::GetTestType() != TESTTYPE_NONE ? 300000 : 10000;




    if (GetMachineTicks() - ptLuaScriptGlobals->dwStartOfCurrentScript > dwMaxLuaTickTime)
    {



        lua_pushstring(L,
            ar->event == LUA_HOOKRET ? "Aborting LUA script: Infinite loop detected!  Event = Returning from function\n" : "Aborting LUA script: Infinite loop detected!  Event = Lua instruction count\n");
        lua_error(L);
    }
}

































































































































































































































float sGetRangeMaxForScriptSno(SNO snoScript)
{
    ScopedScriptDefinitionLockR ptScriptDef(snoScript);
    float flMax = 0.0f;

    for (int32 i = 0; i < ptScriptDef->nMessageMapCount; i++)
    {
        ScriptMessageMap * tMessageMap = &ptScriptDef->arScriptMessageMap[i];
        if (tMessageMap->eEventType == EVENT_PLAYER_IN_RANGE_OF_ACTOR)
        {
            if (tMessageMap->arMessageFilters[2].flValue > flMax)
                flMax = tMessageMap->arMessageFilters[2].flValue;
        }
    }


    return flMax;

}

static BOOL sScriptSNOHasRangeMessages(SNO snoScript)
{
    ScopedScriptDefinitionLockR ptScriptDef(snoScript);

    for (int32 i = 0; i < ptScriptDef->nMessageMapCount; i++)
    {
        ScriptMessageMap * tMessageMap = &ptScriptDef->arScriptMessageMap[i];
        if (tMessageMap->eEventType == EVENT_PLAYER_IN_RANGE_OF_ACTOR)
            return TRUE;
    }


    return FALSE;
}


float sGetRangeMaxForConductorSno(
    SNO snoConductor,
    uint32 nCurrentPhase)
{
    ScopedConductorDefinitionLockR ptConductorDef(snoConductor);


    float flMax = 0.0f;

    for (uint32 i = 0; i < ptConductorDef->nNumPhases; i++)
    {
        if (i == 0 || i == nCurrentPhase)
        {
            ConductorPhase & tPhase = ptConductorDef->arPhases[i];
            for (uint32 j = 0; j < tPhase.nNumSequences; j++)
            {
                ConductorSequence & tSequence = tPhase.arSequences[j];
                ScriptMessageMap * tMessageMap = tSequence.arScriptMessageMap;
                if (tMessageMap->eEventType == EVENT_PLAYER_IN_RANGE_OF_ACTOR)
                {
                    if (tMessageMap->arMessageFilters[2].flValue > flMax)
                        flMax = tMessageMap->arMessageFilters[2].flValue;
                }
            }
        }
    }


    return flMax;

}

float LuaGetMaxPlayerInRangeRadiusInScriptMessageArray()
{
    float flMax = 0.0f;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

    ScriptStatesPointerArray & aScriptStateArray = *ptLuaScriptGlobals->ptRangeEventScriptStates;
    size_t nScriptStates = aScriptStateArray.size();
    for (uint32 i = 0; i < nScriptStates; i++)
    {
        ScriptState * pScriptState = aScriptStateArray[i];
        float flScriptRangeMax = sGetRangeMaxForScriptSno(pScriptState->snoScript);
        if (flScriptRangeMax > flMax)
            flMax = flScriptRangeMax;



        if (pScriptState->bConductor)
        {
            ConductorInstance * ptConductor = SConductorFindConductorByScriptID(pScriptState->id);
            float flConductorRangeMax = sGetRangeMaxForConductorSno(ptConductor->snoConductor, ptConductor->nCurrentPhase);
            if (flConductorRangeMax > flMax)
                flMax = flConductorRangeMax;
        }
    }


    return flMax;
}


BOOL IsTriggerEventTargetingActor(ActorID idActor)
{
    ACDID acdidActor = ACDFindByActor(idActor);
    return SQuestsIsQuestTriggerTargetingActor(acdidActor) || IsScriptTriggerEventTargetingActor(acdidActor);
}































































































































































































































































void LuaScriptInitializeForApp()
{
    LuaScriptAppGlobals *& ptLuaScriptGlobals = sgtAppGlobals.ptLuaScriptAppGlobals;
    ptLuaScriptGlobals = new LuaScriptAppGlobals;
    ptLuaScriptGlobals->nNumTraceScriptCallbacks = 0;
}





void LuaScriptDisposeForApp()
{
    LuaScriptAppGlobals *& ptLuaScriptGlobals = sgtAppGlobals.ptLuaScriptAppGlobals;
    delete ptLuaScriptGlobals;
    ptLuaScriptGlobals = NULL;
}



void LuaScriptInitializeForGame()

{
    LuaScriptGameGlobals *& ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();

    ptLuaScriptGlobals = new LuaScriptGameGlobals;

    ptLuaScriptGlobals->ptScriptInstances = TDataArrayCreateTracked<ScriptInstance, int>("ScriptInstances", ptMemoryInterface, MAX_SCRIPTINSTANCES);

    ptLuaScriptGlobals->ptScriptAttachmentPool = new ScriptAttachmentListPool();
    ptLuaScriptGlobals->ptScriptAttachmentPool->SetBlockCount_Tracked(MAX_SCRIPTSTATES);

    ptLuaScriptGlobals->ptScriptAttachmentMap = new SNOScriptAttachmentListMap(16, ptMemoryInterface);
    ptLuaScriptGlobals->ptScriptAttachmentMap->SetHashTableSize_Tracked(512);

    ptLuaScriptGlobals->ptInUseACDs = new ACDIDIntMap(16, ptMemoryInterface);
    ptLuaScriptGlobals->ptInUseACDs->SetHashTableSize_Tracked(1024);

    ptLuaScriptGlobals->ptScriptGlobalVars = new ScriptGlobalVarsMap(16, ptMemoryInterface);

    ptLuaScriptGlobals->ptACDScriptStates = new ACDIDLuaStateMap;
    ptLuaScriptGlobals->ptRangeEventScriptStates = new ScriptStatesPointerArray;

    ptLuaScriptGlobals->dwStartOfCurrentScript = GetMachineTicks();

    ptLuaScriptGlobals->bConductorFlaggedForAdvancementThisUpdate = FALSE;


    ptLuaScriptGlobals->ptMapRefACDsPool = new MapRefACDPool();
    ptLuaScriptGlobals->ptMapRefACDsPool->SetBlockCount_Tracked(BLOCK_COUNT_GAME_REFERENCED_ACDID);
    // DWARF constant location is32.
    const int32 INITIAL_INACTIVE_SAVE_COUNT = 32;
    SGame & tSGame = SGameManager::GameGet();
    ptLuaScriptGlobals->ptInactiveSaveBlockArray = TChainedDataArrayCreateTracked<SaveBlock, int>("ptInactiveSaveBlockArray", tSGame.ptMemoryInterface, INITIAL_INACTIVE_SAVE_COUNT);
    {

        ScopedGlobalDefinitionLockR ptGlobals;


        int nGlobalVarCount = ptGlobals->ptServerData->serScriptGlobalVars.dwSize / sizeof(GlobalScriptVariable);
        // Inference: hash size starts at64 and selects the larger variable count in the folded branch.
        int nHashTableSize = 64;

        if (nGlobalVarCount > 64)
            nHashTableSize = nGlobalVarCount;



        ptLuaScriptGlobals->ptScriptGlobalVars->SetHashTableSize_Tracked(nHashTableSize);

        for (int i = 0; i < nGlobalVarCount; ++i)
        {
            GlobalScriptVariable & tGlobalVar = ptGlobals->ptServerData->arScriptGlobalVars[i];
            ptLuaScriptGlobals->ptScriptGlobalVars->SetAt_Tracked(tGlobalVar.uHash, tGlobalVar.flDefaultValue);
        }
    }

    ptLuaScriptGlobals->ptScriptStates = TDataArrayCreateTracked<ScriptState, int>("ScriptState", NULL, MAX_SCRIPTSTATES);



    ptLuaScriptGlobals->mapSNOScriptID = new SNOScriptIDMap(16, ptMemoryInterface);
    ptLuaScriptGlobals->mapSNOCompiledScripts = new SNOCompiledScriptMap;
    ptLuaScriptGlobals->mapSNOCompiledScripts->SetGrow(XALLOC_GROW_LINEAR, COMPILED_SCRIPT_GROWTH_RATE);

    ptLuaScriptGlobals->ptLuaScriptAlloc = LuaScriptAllocInitializeForGame();


    ptLuaScriptGlobals->ptHydraState = sAllocNewLuaState(NULL, "Hydra");


    LuaSetInitializingScript("Hydra");
    luaL_openlibs(ptLuaScriptGlobals->ptHydraState);
    RegisterHydraLib(ptLuaScriptGlobals->ptHydraState);
    LuaClearInitializingScript();

    ptLuaScriptGlobals->ptConsoleState = sAllocNewLuaState(ptLuaScriptGlobals->ptHydraState, "Console");

    lua_gc(ptLuaScriptGlobals->ptHydraState, LUA_GCSTOP, 0); }



void LuaScriptDisposeForGame()

{
    LuaScriptGameGlobals *& ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;




        sDetachQueuedEntries();




    {
        ScriptInstanceIterator tIter;


        while (ptLuaScriptGlobals->ptScriptInstances->Iterate(tIter))
        {
            ScriptInstance * ptScriptInstance = tIter.ptElement;
            sScriptCloseInstance(ptScriptInstance);
        }
    }



    {
        ScriptStateIterator tIt;


        while (ptLuaScriptGlobals->ptScriptStates->Iterate(tIt))
        {
            ScriptState * ptScriptState = tIt.ptElement;

            sScriptCloseAndFreeScriptState(ptScriptState);
        }
    }



    {
        POS pos = ptLuaScriptGlobals->ptScriptAttachmentMap->GetStartAssoc();
        while (pos)
        {
            SNO snoScriptKey;
            PooledScriptAttachmentList * listAttachments;
            ptLuaScriptGlobals->ptScriptAttachmentMap->GetNextAssoc(pos, snoScriptKey, listAttachments);

            if (listAttachments)
            {
                if (listAttachments->Count() > 0)
                {
                    CRefString sScript = SNOToString(SCRIPT_SNO, snoScriptKey, FALSE);
                    POS posList = listAttachments->GetHeadPosition();


                    // Inference: the original walks the remaining links; diagnostic use of each element was removed.
                    while (posList)
                    {
                        listAttachments->GetNext(posList);
                    }


                }

                delete listAttachments;
            }
        }
    }





        sScriptCloseAndFreeLuaState(ptLuaScriptGlobals->ptConsoleState, ptLuaScriptGlobals->ptHydraState, "Console");
        ptLuaScriptGlobals->ptConsoleState = NULL;





        sScriptCloseAndFreeLuaStateProfiled(ptLuaScriptGlobals->ptHydraState, NULL, NULL);
        ptLuaScriptGlobals->ptHydraState = NULL;

        delete ptLuaScriptGlobals->ptScriptAttachmentMap; ptLuaScriptGlobals->ptScriptAttachmentMap = NULL;
        delete ptLuaScriptGlobals->ptScriptGlobalVars; ptLuaScriptGlobals->ptScriptGlobalVars = NULL;
        delete ptLuaScriptGlobals->ptInUseACDs; ptLuaScriptGlobals->ptInUseACDs = NULL;
        delete ptLuaScriptGlobals->ptScriptAttachmentPool; ptLuaScriptGlobals->ptScriptAttachmentPool = NULL;
        delete ptLuaScriptGlobals->ptMapRefACDsPool; ptLuaScriptGlobals->ptMapRefACDsPool = NULL;



        TDataArrayDispose(ptLuaScriptGlobals->ptScriptInstances);
        ptLuaScriptGlobals->ptScriptInstances = NULL;




        TDataArrayDispose(ptLuaScriptGlobals->ptScriptStates);
        ptLuaScriptGlobals->ptScriptStates = NULL;

        delete ptLuaScriptGlobals->mapSNOScriptID; ptLuaScriptGlobals->mapSNOScriptID = NULL;




    {
        POS posCompiledScripts = ptLuaScriptGlobals->mapSNOCompiledScripts->GetStartAssoc();
        while (posCompiledScripts)
        {
            CompiledScriptData tData;
            SNO snoKey;
            ptLuaScriptGlobals->mapSNOCompiledScripts->GetNextAssoc(posCompiledScripts, snoKey, tData);

            SigmaMemoryFree(tData.pCompiledScript, NULL);
        }

        delete ptLuaScriptGlobals->mapSNOCompiledScripts; ptLuaScriptGlobals->mapSNOCompiledScripts = NULL;
    }




    {
        POS posACDScriptStates = ptLuaScriptGlobals->ptACDScriptStates->GetStartAssoc();
        while (posACDScriptStates)
        {
            ACDID idACD;
            lua_State * L;
            ptLuaScriptGlobals->ptACDScriptStates->GetNextAssoc(posACDScriptStates, idACD, L);
            sScriptCloseAndFreeLuaState(L, NULL, NULL);
        }
        delete ptLuaScriptGlobals->ptACDScriptStates; ptLuaScriptGlobals->ptACDScriptStates = NULL;
    }




    LuaScriptAllocDisposeForGame(ptLuaScriptGlobals->ptLuaScriptAlloc);
    ptLuaScriptGlobals->ptLuaScriptAlloc = NULL;


        delete ptLuaScriptGlobals->ptRangeEventScriptStates; ptLuaScriptGlobals->ptRangeEventScriptStates = NULL;


    TChainedDataArrayDispose(ptLuaScriptGlobals->ptInactiveSaveBlockArray);

        delete ptLuaScriptGlobals->ptScriptAttachmentMap; ptLuaScriptGlobals->ptScriptAttachmentMap = NULL;

    delete ptLuaScriptGlobals; ptLuaScriptGlobals = NULL;



    {
        Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tScriptDefPurgeMutex);
        sgtAppGlobals.ptScriptDefsPurged->EmptyArray();
    }

}


ActorIDArray & LuaScriptGetThreadSafeActorIDArray()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    return ptLuaScriptThreadGlobals->tActorIdArray;
}


void LuaScriptOnSGameUpdateStart()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        ptLuaScriptThreadGlobals->nExecutingScriptDepth = 0;
        ptLuaScriptThreadGlobals->nExecutingFunctionDepth = 0;
    }
}




BOOL LuaScriptIsScriptBeingUsed(SNO snoScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (ptLuaScriptGlobals == NULL)
        return FALSE;
    ScriptID idScript;
    ScriptInstanceIterator tIter;


    idScript = LuaScriptGetScriptID(snoScript);
    if (idScript == SCRIPTID_INVALID)
        return FALSE;



    while (ScriptInstance * ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->Iterate(tIter))
    {
        if (ptScriptInstance->idScript == idScript)
            return TRUE;
    }




    for (uint32 i = 0; i < ptLuaScriptGlobals->ptRangeEventScriptStates->size(); i++)
    {
        if ((*ptLuaScriptGlobals->ptRangeEventScriptStates)[i]->id == idScript)
            return TRUE;
    }
    return FALSE;
}








void LuaScriptUpdate()
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    uint32 dwGameTime;
    ScriptInstanceIterator tIter;
    if (ptLuaScriptGlobals == NULL)
        return;




    dwGameTime = GameGetActiveGameCommonData().dwServerTime;

    ScriptEventUpdate();

    sDetachQueuedEntries();






    while (ScriptInstance * ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->Iterate(tIter))
    {
        if (ptScriptInstance->dwResumeTick != 0xFFFFFFFF && ptScriptInstance->dwResumeTick <= dwGameTime)
            sLuaTickScriptInstance(ptScriptInstance);
    }













































    lua_gc(ptLuaScriptGlobals->ptHydraState, LUA_GCSTEP, 0);

    {
        Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tScriptDefPurgeMutex);

        int32 nCount = sgtAppGlobals.ptScriptDefsPurged->GetArraySize();
        for (int32 i = 0; i < nCount; i++)
        {
            SNO snoScript = (*sgtAppGlobals.ptScriptDefsPurged)[i];
            if (!LuaScriptIsScriptBeingUsed(snoScript))
            {
                ScriptDefinitionDisposeScript(snoScript);
            }
        }

        sgtAppGlobals.ptScriptDefsPurged->EmptyArray();
    }
}



static void sLuaTickScriptInstance(ScriptInstance * ptScriptInstance)
{

    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

    BOOL bBase;
    bBase = ptLuaScriptThreadGlobals->nExecutingScriptDepth == 0;




    if (ptScriptInstance->bAttachmentDeleted)
    {
        sScriptCloseInstance(ptScriptInstance);
        return;
    }

    if (bBase)
    {
        ptLuaScriptGlobals->bConductorFlaggedForAdvancementThisUpdate = FALSE;
    }
    else
    {
        if (ptLuaScriptGlobals->bConductorFlaggedForAdvancementThisUpdate)
        {
            ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementGet(ptScriptInstance->idScript);
            if (ptScriptState->bConductor)
            {
                ConductorInstance * ptConductor = SConductorFindConductorByScriptID(ptScriptInstance->idScript);
                if (ptConductor)
                {
                    if (ptConductor->bFlaggedForDeletion)
                        return;
                }
            }
        }
    }












    int32 nArgs = ptScriptInstance->nResumeArgCnt;

    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount++;
    uint32 dwExecuteUIDPrev = ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID;
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount;

    Stopwatch tStopwatch;













    ptLuaScriptGlobals->dwStartOfCurrentScript = GetMachineTicks();

    LuaScriptPushExecutingScript(ptScriptInstance->id);
    int32 nRet;


    nRet = lua_resume(ptScriptInstance->ptLuaState, nArgs);


    // Inference: both status flags start FALSE; success sets done and error cases set both.
    BOOL bScriptIsDone = FALSE;
    BOOL bScriptHadError = FALSE;

    switch (nRet)
    {
    case 0:
        bScriptIsDone = TRUE;
        break;
    case LUA_ERRRUN:
    case LUA_ERRSYNTAX:
    case LUA_ERRMEM:
    case LUA_ERRERR:
        {

            LuaScriptReportError(ptScriptInstance->ptLuaState, nRet, TRUE);
            // Inference: absent string error passes NULL to LuaDebugBreakError.
            LPCSTR szErrorMsg = NULL;
            if (lua_isstring(ptScriptInstance->ptLuaState, -1))
            {
                szErrorMsg = lua_tostring(ptScriptInstance->ptLuaState, -1);
            }
            LuaDebugBreakError(ptScriptInstance->ptLuaState, szErrorMsg);
            // Inference: both flags are set in the merged error tail.
            bScriptHadError = TRUE; bScriptIsDone = TRUE;
            break; }
    }
    LuaScriptPopExecutingScript();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = dwExecuteUIDPrev;

    // DWARF constant location0x3ca3d70a.
    const float flMaxLuaTimePerScriptUpdate = 0.02f;



    // DWARF constant location0x3e99999a.
    const float flExpensiveUpdateThreshold = 0.3f;
    float flTime = tStopwatch.Check();
    if (flTime > flMaxLuaTimePerScriptUpdate || (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        Power * ptPowerScoped = PowerTryToGet(ptScriptInstance->idAttachment);
        if (ptPowerScoped)
        {
            RuneType eRuneType = PowerGetRuneType(ptPowerScoped);














            if (s_varEnableHaltOnExpensivePowers.Get())
            {

                if (flTime > flExpensiveUpdateThreshold)
                {
                    ptPowerScoped->nExpensiveScriptCalls++;

                    if (ptPowerScoped->nExpensiveScriptCalls > 10)
                    {
                        CRefString sError = CRefString().Format(
                            "Power %s with rune (%d) had too many expensive script calls (%s).  Total Script Calls: %d Tracked ACDs: %d",
                            SNOToString(POWER_SNO, ptPowerScoped->snoPower, FALSE).str(),
                            eRuneType,
                            ptScriptInstance->szEntryPoint,
                            ptPowerScoped->nScriptCalls,
                            ptPowerScoped->listACDsTracked.Count());
                        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                    }
                }
            }
        }
    }







































    if (bScriptIsDone)
    {
        sScriptCloseInstance(ptScriptInstance);
    }

    if (bScriptHadError)
    {
        if (ptScriptInstance->eAttachmentType == SCRIPT_ATTACH_POWER)
        {
            PowerForceStop(ptScriptInstance->idAttachment, FALSE);
        }
    }

    if (bBase)
    {
        if (ptLuaScriptGlobals->bConductorFlaggedForAdvancementThisUpdate)
        {
            SGame & tSGameScoped = SGameManager::GameGet();
            ConductorGlobals *& ptConductorGlobals = tSGameScoped.ptConductorGlobals;
            POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
            while (pos)
            {
                ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);
                if (tConductorInstance.bFlaggedForAdvancement)
                {
                    tConductorInstance.bFlaggedForAdvancement = FALSE;
                    SConductorAdvancePhase(tConductorInstance);
                }
                else if (tConductorInstance.bFlaggedForPhaseSkip)
                {
                    tConductorInstance.bFlaggedForPhaseSkip = FALSE;
                    SConductorSkipToPhase(tConductorInstance, tConductorInstance.szPhaseSkipName);
                }
            }
        }
    }
}

void LuaTickScriptInstanceId(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        sLuaTickScriptInstance(ptScriptInstance);
}





static void sActivateListeners(
    ScriptID idScript)
{
    if (idScript != SCRIPTID_INVALID)
    {
        ScriptState * ptScriptState = sGetScriptStateWithScriptID(idScript);

        for (int32 i = 0; i < ptScriptState->nNumEventHandlers; i++)
        {
            ScriptEventActivateListener(ptScriptState->arEventHandlers[i], idScript);
        }
    }
}


POS LuaScriptAttach(
    ScriptAttachmentType eType,
    DataID idAttachment,
    SNO snoScript,
    uint16 nAdventureParentIndex)
{
    if (!SNOFileExists(SCRIPT_SNO, snoScript) && !gptGroupDefScript->WasDynamicallyCreated(snoScript))
        return NULL;




    POS pos;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    PooledScriptAttachmentList * listAttachments;

    if (!ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(snoScript, listAttachments))
    {




        listAttachments = new PooledScriptAttachmentList(ptLuaScriptGlobals->ptScriptAttachmentPool);

        ptLuaScriptGlobals->ptScriptAttachmentMap->SetAt_Tracked(snoScript, listAttachments);
    }

    ScriptAttachment tAttachment;
    tAttachment.eAttachType = eType;
    tAttachment.idAttachment = idAttachment;
    tAttachment.dwDisabledEvents = 0;
    tAttachment.bFlaggedForDeletion = FALSE;
















    listAttachments->AddTail_Tracked(tAttachment);

    if (listAttachments->Count() == 1)
    {

        ScriptID idScript = LuaScriptGetScriptID(snoScript);

        if (idScript == SCRIPTID_INVALID)
        {
            idScript = LuaScriptCreateState(snoScript, nAdventureParentIndex, NULL);
        }

        sActivateListeners(idScript);
    }

    pos = listAttachments->GetTailPosition();

    return pos;
}


static void sDeActivateListeners(ScriptID idScript)
{
    if (idScript == SCRIPTID_INVALID)
        return;


    ScriptState * ptScriptState = sGetScriptStateWithScriptID(idScript);
    for (int32 i = 0; i < ptScriptState->nNumEventHandlers; i++)
    {
        ScriptEventDeActivateListener(ptScriptState->arEventHandlers[i]);
    }
}




void LuaScriptDetach(ScriptAttachmentType eType, DataID idAttachment, SNO snoScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    DetachQueueEntry tEntry;
    PooledScriptAttachmentList * listAttachments;

    if (ptLuaScriptGlobals == NULL)
        return;

    tEntry.eType = eType;
    tEntry.idAttachment = idAttachment;
    tEntry.snoScript = snoScript;









    if (ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(snoScript, listAttachments))
    {
        ScriptAttachment tAttachment;
        tAttachment.eAttachType = eType;
        tAttachment.idAttachment = idAttachment;

        POS pos = listAttachments->GetHeadPosition();
        while (pos)
        {

            ScriptAttachment & tAttachmentCmp = listAttachments->GetNext(pos);
            if (tAttachmentCmp.eAttachType == tAttachment.eAttachType && tAttachmentCmp.idAttachment == tAttachment.idAttachment)
            {
                tAttachmentCmp.bFlaggedForDeletion = TRUE;
                break;
            }
        }
    }


    ptLuaScriptGlobals->tDetachQueue.push_back(tEntry);
}
static void sHandleQueuedDetachEntry(
    DetachQueueEntry & tEntry)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    PooledScriptAttachmentList * listAttachments;

    if (ptLuaScriptGlobals &&
        ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(tEntry.snoScript, listAttachments))
    {
        ScriptAttachment tAttachment;
        tAttachment.eAttachType = tEntry.eType;
        tAttachment.idAttachment = tEntry.idAttachment;

        POS pos = listAttachments->GetHeadPosition();

        while (pos)
        {
            POS posCur = pos;
            ScriptAttachment & tAttachmentCmp = listAttachments->GetNext(pos);

            if (tAttachmentCmp.eAttachType == tAttachment.eAttachType &&
                tAttachmentCmp.idAttachment == tAttachment.idAttachment)
            {
                listAttachments->RemoveAt(posCur);
                break;
            }
        }

        if (!listAttachments->Count())
        {

            sDeActivateListeners(LuaScriptGetScriptID(tEntry.snoScript));
        }
    }
}


static void sDetachQueuedEntries()

{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    size_t nCount = ptLuaScriptGlobals->tDetachQueue.size();
    for (uint i = 0; i < nCount; ++i)
    {
        sHandleQueuedDetachEntry(ptLuaScriptGlobals->tDetachQueue[i]);
    }
    ptLuaScriptGlobals->tDetachQueue.clear();
}


void LuaScriptDeactivateInScene(const SSceneID idSScene)
{
    SScene * ptSScene = SSceneTryToGet(idSScene);
    if (ptSScene != NULL)
    {

        POS pos = ptSScene->listScriptsInScene.GetHeadPosition();
        while (pos)
        {
            SceneScript & tSceneScript = ptSScene->listScriptsInScene.GetNext(pos);
            LuaScriptDetach(SCRIPT_ATTACH_SCENE, idSScene, tSceneScript.snoScript);
        }
    }
}




void LuaScriptReActivateInScene(const SSceneID idSScene)
{
    if (GameActiveGameCommonDataIsServer())
    {
        SScene * ptSScene = SSceneTryToGet(idSScene);
        if (ptSScene != NULL)
        {

            POS pos = ptSScene->listScriptsInScene.GetHeadPosition();
            while (pos)
            {
                SceneScript & tSceneScript = ptSScene->listScriptsInScene.GetNext(pos);
                LuaScriptAttach(SCRIPT_ATTACH_SCENE, idSScene, tSceneScript.snoScript, tSceneScript.nAdventureParentIndex);
            }
        }
    }
}







LPCSTR LuaScriptAttachmentTypeToString(ScriptAttachmentType eAttachType)
{
    switch (eAttachType)
    {
    case SCRIPT_ATTACH_NONE:
        return "None";
    case SCRIPT_ATTACH_ACTOR:
        return "Actor";
    case SCRIPT_ATTACH_QUEST:
        return "Quest";
    case SCRIPT_ATTACH_SCENE:
        return "Scene";
    case SCRIPT_ATTACH_WORLD:
        return "World";
    case SCRIPT_ATTACH_POWER:
        return "Power";
    }
    return "Unknown";
}
















































void LuaScriptErrorHandlerEnter()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = 0;
}




void LuaScriptErrorHandlerExit()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount++;
    ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount;
}




void LuaScriptPushExecutingScript(ScriptInstanceID idScriptInstance)
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        if (ptLuaScriptThreadGlobals->nExecutingScriptDepth == 0)
        {
            ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount++;
            ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteCount;
        }
        ptLuaScriptThreadGlobals->arExecutingScriptStack[ptLuaScriptThreadGlobals->nExecutingScriptDepth] = idScriptInstance;
        ptLuaScriptThreadGlobals->nExecutingScriptDepth++;
    }
}

























































SNO LuaScriptGetExecutingScriptSNO()
{
    ScriptInstanceID idExecutingScript = LuaScriptGetExecutingScriptInstance();
    if (idExecutingScript != SCRIPTINSTANCEID_INVALID)
    {
        ScriptState * ptScriptState = sGetScriptStateWithScriptInstanceID(idExecutingScript);
        return ptScriptState->snoScript;
    }
    return -1;
}




lua_State * LuaScriptGetExecutingScriptState()
{
    ScriptInstanceID idExecutingScript = LuaScriptGetExecutingScriptInstance();
    if (idExecutingScript != SCRIPTINSTANCEID_INVALID)
    {
        ScriptInstance * ptScriptInstance = sGetScriptInstance(idExecutingScript);
        if (ptScriptInstance != NULL)
            return ptScriptInstance->ptLuaState;
    }
    return NULL;
}





BOOL LuaScriptExecutingScriptIsConductor()
{
    ScriptInstanceID idExecutingScript = LuaScriptGetExecutingScriptInstance();
    if (idExecutingScript != SCRIPTINSTANCEID_INVALID)
    {
        ScriptInstance * ptScriptInstance = sGetScriptInstance(idExecutingScript);
        if (ptScriptInstance != NULL)
        {
            LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
            ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementGet(ptScriptInstance->idScript);
            return ptScriptState->bConductor;
        }
    }
    return FALSE;
}



void LuaScriptPopExecutingScript()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        ScriptInstanceID * ptOldScript = &ptLuaScriptThreadGlobals->arExecutingScriptStack[ptLuaScriptThreadGlobals->nExecutingScriptDepth - 1];
        ScriptInstanceID idScriptToPop = *ptOldScript;
        if (idScriptToPop != SCRIPTINSTANCEID_INVALID)
        {
            ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptToPop);
            if (ptScriptInstance != NULL)
            {
                int32 * pAbort = lua_abortaddress(ptScriptInstance->ptLuaState);
                if (*pAbort == 1)
                    *pAbort = 0;
            }
        }
        ptLuaScriptThreadGlobals->nExecutingScriptDepth--;
        *ptOldScript = SCRIPTINSTANCEID_INVALID;
        if (ptLuaScriptThreadGlobals->nExecutingScriptDepth == 0)
            ptLuaScriptThreadGlobals->ptDeadlockGameThreadData->dwExecuteUID = 0;
    }
}

















void LuaScriptPushExecutingFunction(LPCSTR szFuncName)
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        if (ptLuaScriptThreadGlobals->nExecutingFunctionDepth < 10)
            ptLuaScriptThreadGlobals->arExecutingFunctionStack[ptLuaScriptThreadGlobals->nExecutingFunctionDepth] = szFuncName;
        ptLuaScriptThreadGlobals->nExecutingFunctionDepth++;
    }
}







LPCSTR LuaScriptGetExecutingFunction()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        int32 nIndex = ptLuaScriptThreadGlobals->nExecutingFunctionDepth - 1;
        if (nIndex >= 0 && nIndex < 10)
            return ptLuaScriptThreadGlobals->arExecutingFunctionStack[nIndex];
    }
    return NULL;
}









void LuaScriptPopExecutingFunction()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
        ptLuaScriptThreadGlobals->nExecutingFunctionDepth--;
}






void LuaSetInitializingScript(LPCSTR sScriptName)
{

    Blizzard::String::Copy(LuaScriptThreadGlobalsGet()->sInitializingScriptStateName, sScriptName, 128);
}
void LuaClearInitializingScript()
{

    Blizzard::String::Copy(LuaScriptThreadGlobalsGet()->sInitializingScriptStateName, "none", 128);
}
static void sPrintOutOfScriptInstances()
{


    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    ScriptInstanceIterator tIter;
    int32 nCount = 0;
    while (ptLuaScriptGlobals->ptScriptInstances->Iterate(tIter))
    {
        ScriptInstance * ptScriptInstance = tIter.ptElement;
        SNOName snoNameSource = LuaScriptGetSNONameForLuaState(ptScriptInstance->ptLuaState);





    }

}

ScriptInstanceID LuaScriptNewInstanceWithScriptID(
    ScriptID idScript,
    LPCSTR szEntryPoint,
    lua_State * ptParentState,
    SNO snoParentScript,
    DataID idAttachment,
    ScriptAttachmentType eAttachmentType)
{
    if (idScript != SCRIPTID_INVALID)
    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

        ScriptInstanceID idScriptInstance = ptLuaScriptGlobals->ptScriptInstances->ElementNew();

        if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
        {

















            ScriptInstance * ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->ElementGet(idScriptInstance);

            lua_State * ptParentLuaState = ptParentState;
            SNO snoScript = snoParentScript;

            if (ptParentState == NULL || snoParentScript == INVALID_SNO)
            {





                ScriptState * ptParentScriptState = sGetScriptStateWithScriptID(idScript);
                ptParentLuaState = ptParentScriptState->ptLuaState;
                snoScript = ptParentScriptState->snoScript;
            }





            const int32 nBufferSize = 34;
            CHAR sStateName[nBufferSize];
            Blizzard::String::Format(sStateName, nBufferSize, "ScriptInstance_%08X_%08X", snoScript, ptScriptInstance);

            ptScriptInstance->idScript = idScript;



            ptScriptInstance->ptLuaState = sAllocNewLuaState(ptParentLuaState, sStateName);

            ptScriptInstance->dwStartedTick = GameGetServerTime();
            ptScriptInstance->dwResumeTick = 0;
            ptScriptInstance->nResumeArgCnt = ptScriptInstance->nResumeRetValCnt = 0;
            ptScriptInstance->eWaitType = ScriptInstanceWaitNone;

            ptScriptInstance->idLastQueuedAIAction = QUEUED_ACTION_INVALID;
            Blizzard::String::Copy(ptScriptInstance->szEntryPoint, szEntryPoint, MAX_ENTRY_POINT_NAME_LENGTH);
            ptScriptInstance->mapRefACDs.SetPool(ptLuaScriptGlobals->ptMapRefACDsPool);
            ptScriptInstance->mapRefACDs.RemoveAll();
            ptScriptInstance->bFailedToFindAndPurgeOldRefACDs = FALSE;
            ptScriptInstance->eAttachmentType = eAttachmentType;
            ptScriptInstance->idAttachment = idAttachment;
            ptScriptInstance->bAttachmentDeleted = FALSE;
            ptScriptInstance->nNumWaitingEvents = 0;
            for (int32 i = 0; i < MAX_WAITING_EVENTS; i++)
            {
                ptScriptInstance->arListeners[i] = SCRIPT_EVENT_WAIT_LISTENER_INVALID;
            }
        }
        else
        {
            sPrintOutOfScriptInstances();

            SNOName snoNameSource;
            lua_State * ptLuaState = LuaScriptTryGetLuaState(idScript);
            if (ptLuaState != NULL)
            {
                snoNameSource = LuaScriptGetSNONameForLuaState(ptLuaState);
            }




        }
        return idScriptInstance;
    }

    return SCRIPTINSTANCEID_INVALID;
}

















void LuaScriptSetInstanceNumArgsAndRet(ScriptInstanceID idScriptInstance, int32 nArgs, int32 nRet)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);

    ptScriptInstance->nResumeArgCnt = nArgs;
    ptScriptInstance->nResumeRetValCnt = nRet;
}




int32 LuaScriptGetInstanceNumRet(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);

    return ptScriptInstance->nResumeRetValCnt;
}



lua_State * LuaScriptGetInstanceState(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        return ptScriptInstance->ptLuaState;
    return NULL;
}








ScriptID LuaScriptGetInstanceParent(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        return ptScriptInstance->idScript;
    return SCRIPTID_INVALID;
}









LPCSTR LuaScriptGetInstanceEntryPoint(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        return ptScriptInstance->szEntryPoint;
    return "INVALID";
}











































void LuaScriptSetInstanceResumeTick(ScriptInstanceID idScriptInstance, uint32 dwResumeTick, int32 nResumeArgCnt)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
    {
        ptScriptInstance->dwResumeTick = dwResumeTick;
        ptScriptInstance->nResumeArgCnt = nResumeArgCnt;
        ptScriptInstance->nResumeRetValCnt = 0;
    }
}






BOOL LuaScriptAddInstanceWaitListener(ScriptInstanceID idScriptInstance, ScriptEventWaitListenerID idWaitListener)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL && ptScriptInstance->nNumWaitingEvents < MAX_WAITING_EVENTS)
    {
        ptScriptInstance->arListeners[ptScriptInstance->nNumWaitingEvents] = idWaitListener;
        ptScriptInstance->nNumWaitingEvents++;
        return TRUE;
    }
    return FALSE;
}








void LuaScriptWaitForAnyEvent(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
    {
        ptScriptInstance->dwResumeTick = 0xFFFFFFFF;
        ptScriptInstance->nResumeArgCnt = 0;
        ptScriptInstance->nResumeRetValCnt = 0;
        ptScriptInstance->eWaitType = ScriptInstanceWaitForAnyEvent;
    }
}




void LuaScriptWaitForAllEvents(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
    {
        ptScriptInstance->dwResumeTick = 0xFFFFFFFF;
        ptScriptInstance->nResumeArgCnt = 0;
        ptScriptInstance->nResumeRetValCnt = 0;
        ptScriptInstance->eWaitType = ScriptInstanceWaitForAllEvents;
    }
}




static void sScriptInstanceRemoveAllWaitingEvents(ScriptInstance * ptScriptInstance)
{

    for (int32 i = 0; i < ptScriptInstance->nNumWaitingEvents; i++)
    {
        ScriptEventWaitListenerID idWaitListener = ptScriptInstance->arListeners[i];

        if (idWaitListener != SCRIPT_EVENT_WAIT_LISTENER_INVALID)
        {
            ScriptEventRemoveGameWaitListener(idWaitListener);
        }
    }

    ptScriptInstance->nNumWaitingEvents = 0;
}


void LuaScriptInstanceNotifyEvent(ScriptInstanceID idScriptInstance, ScriptEventWaitListenerID idListener, ScriptEventType eEventType, ScriptEventParamList * ptEventDefParams)
{




    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
    {

        BOOL bEventsActive = FALSE;
        BOOL bFoundEvent = FALSE;

        for (int32 i = 0; i < ptScriptInstance->nNumWaitingEvents; i++)
        {
            if (ptScriptInstance->arListeners[i] == idListener)
            {
                ScriptEventRemoveGameWaitListener(idListener);
                ptScriptInstance->arListeners[i] = SCRIPT_EVENT_WAIT_LISTENER_INVALID;
                bFoundEvent = TRUE;
            }
            // Inference: a separate if, not else: original row 2449 tests the listener only on the not-found path (the INVALID store forwards) and keeps the branch.
            if (ptScriptInstance->arListeners[i] != SCRIPT_EVENT_WAIT_LISTENER_INVALID)
            {
                bEventsActive = TRUE;
            }

            if (bFoundEvent && bEventsActive)
                break;

        }


        switch (ptScriptInstance->eWaitType)
        {
        case ScriptInstanceWaitNone:
            if (bFoundEvent)
            {



                SNOName snoNameSource = LuaScriptGetSNONameForLuaState(ptScriptInstance->ptLuaState);
                CRefString sError(CRefString().Format("ptScriptInstance->eWaitType == ScriptInstanceWaitNone in SNO %s, function %s, num waiting events %d, found event %d, events active %d", SNOToString(snoNameSource, FALSE).str(), ptScriptInstance->szEntryPoint, ptScriptInstance->nNumWaitingEvents, bFoundEvent, bEventsActive));
                DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
            }
            break;


        case ScriptInstanceWaitForAllEvents:



            if (eEventType == EVENT_GAME_TIME)
            {
                sScriptInstanceRemoveAllWaitingEvents(ptScriptInstance);
                bEventsActive = FALSE;
            }


            if (!bEventsActive)
            {
                int32 nParams;

                lua_pushinteger(ptScriptInstance->ptLuaState, eEventType);
                nParams = ptEventDefParams->PushScriptParams(ptScriptInstance->ptLuaState) + 1;

                ptScriptInstance->dwResumeTick = GameGetActiveGameCommonData().dwServerTime;
                ptScriptInstance->nResumeArgCnt = nParams;
                ptScriptInstance->nResumeRetValCnt = 0;
                ptScriptInstance->nNumWaitingEvents = 0;
                ptScriptInstance->eWaitType = ScriptInstanceWaitNone;
            }
            break;

        case ScriptInstanceWaitForAnyEvent:
            {
                int32 nParams;
                sScriptInstanceRemoveAllWaitingEvents(ptScriptInstance);



                lua_pushinteger(ptScriptInstance->ptLuaState, eEventType);
                nParams = ptEventDefParams->PushScriptParams(ptScriptInstance->ptLuaState) + 1;

                ptScriptInstance->dwResumeTick = GameGetActiveGameCommonData().dwServerTime;
                ptScriptInstance->nResumeArgCnt = nParams;
                ptScriptInstance->nResumeRetValCnt = 0;
                ptScriptInstance->nNumWaitingEvents = 0;
                ptScriptInstance->eWaitType = ScriptInstanceWaitNone;
            }
            break;
        }
    }
}






void LuaScriptInstanceSetLastAIQAction(ScriptInstanceID idScriptInstance, AIQueuedActionID idQAction)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        ptScriptInstance->idLastQueuedAIAction = idQAction;
}























void LuaScriptInstanceAddRefACD(
    ScriptInstanceID idScriptInstance,
    ACDID idACD)
{


    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance == NULL)
        return;






    if (GameActiveGameCommonDataIsServer())
    {
        ConductorInstance * ptConductorInstance = SConductorFindConductorByScriptID(ptScriptInstance->idScript);
        if (ptConductorInstance != NULL)
        {
            SConductorInstanceAddRefACD(ptConductorInstance, idACD);
            return;
        }
    }

    int32 nLookupValue;
    if (!ptScriptInstance->mapRefACDs.Lookup(idACD, nLookupValue))
    {
        if (ptScriptInstance->mapRefACDs.Count() == MAX_REFERENCED_ACDID)
        {
            if (!ptScriptInstance->bFailedToFindAndPurgeOldRefACDs)
            {
                ACDID idACDRef;
                int32 nRefValue;
                POS posRef = ptScriptInstance->mapRefACDs.GetStartAssoc();
                while (posRef)
                {
                    ptScriptInstance->mapRefACDs.GetNextAssoc(posRef, idACDRef, nRefValue);
                    if (ACDTryToGet(idACDRef) == NULL)
                        ptScriptInstance->mapRefACDs.RemoveAt(idACDRef);
                }



                if (ptScriptInstance->mapRefACDs.Count() == MAX_REFERENCED_ACDID)
                {
                    ptScriptInstance->bFailedToFindAndPurgeOldRefACDs = TRUE;
                }
            }

            if (ptScriptInstance->bFailedToFindAndPurgeOldRefACDs)
                return;
        }



        sAddRefACDID(idACD);

        ptScriptInstance->mapRefACDs.SetAt_Tracked(idACD, 1);
    }
}


void LuaScriptInstanceRemoveRefACD(ScriptInstanceID idScriptInstance, ACDID idACD)
{
    ScriptInstance * ptScriptInstance;
    int32 nLookupValue;



    ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance == NULL)
        return;

    if (GameActiveGameCommonDataIsServer())
    {
        ConductorInstance * ptConductorInstance = SConductorFindConductorByScriptID(ptScriptInstance->idScript);
        if (ptConductorInstance != NULL)
        {
            SConductorInstanceRemoveRefACD(ptConductorInstance, idACD);
            return;
        }
    }





    if (ptScriptInstance->mapRefACDs.Lookup(idACD, nLookupValue))
    {
        sRemoveRefACDID(idACD);
        ptScriptInstance->mapRefACDs.RemoveAt(idACD);
    }
}


BOOL LuaScriptInstanceIsACDInUse(ACDID idACD)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    int32 nCount;
    return ptLuaScriptGlobals->ptInUseACDs->Lookup(idACD, nCount);
}












ScriptInstanceID LuaScriptGetExecutingScriptInstance()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        int32 nIndex = ptLuaScriptThreadGlobals->nExecutingScriptDepth - 1;
        if (nIndex >= 0 && nIndex < 10)
        {
            ScriptInstanceID idScriptInstance = ptLuaScriptThreadGlobals->arExecutingScriptStack[nIndex];
            if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
            {
                ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
                return ptScriptInstance != NULL ? idScriptInstance : SCRIPTINSTANCEID_INVALID;
            }
        }
    }
    return SCRIPTINSTANCEID_INVALID;
}











BOOL LuaScriptScriptIsExecuting(ScriptInstanceID idScript)
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
    {
        for (int32 i = 0; i < ptLuaScriptThreadGlobals->nExecutingScriptDepth; i++)
        {
            if (ptLuaScriptThreadGlobals->arExecutingScriptStack[i] == idScript)
                return TRUE;
        }
    }
    return FALSE;
}






uint16 LuaScriptGetExecutingScriptAdventureParentIndex()
{
    ScriptInstanceID idExecutingScript = LuaScriptGetExecutingScriptInstance();
    return LuaScriptGetScriptAdventureParentIndex(idExecutingScript);
}



uint16 LuaScriptGetScriptAdventureParentIndex(ScriptInstanceID idScriptInstance)
{
    if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
    {
        ScriptState * ptScriptState = sGetScriptStateWithScriptInstanceID(idScriptInstance);
        return ptScriptState->nAdventureParentIndex;
    }
    return 0xFFFF;
}
















BOOL LuaScriptHasHighScriptInstanceCount()
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    return ptLuaScriptGlobals->ptScriptInstances->GetTotalCount() > HIGH_SCRIPT_INSTANCE_THRESHOLD;
}

































































































































































































































































































// NON_MATCHING: 828 versus 852 bytes; only the inlined blz::vector push_back growth/aliasing sequence at the end differs (shared header code).
ScriptID LuaScriptCreateState(
    SNO snoScript,
    uint16 nAdventureParentIndex,
    ConductorInstance * ptConductor)
{
    Stopwatch tStopwatch;
    float flTime;

    ScriptID idScript = SCRIPTID_INVALID;

    if (!SNOFileExists(SCRIPT_SNO, snoScript) && !gptGroupDefScript->WasDynamicallyCreated(snoScript))
        return idScript;




    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

    if (ptLuaScriptGlobals != NULL)
    {
        if (ptLuaScriptGlobals->ptScriptStates->AvailableCount() == 0)
        {

            ScriptStateIterator tIt;
            while (ptLuaScriptGlobals->ptScriptStates->Iterate(tIt))
            {
                ScriptState * ptScriptState = tIt.ptElement;
                ScopedScriptDefinitionLockR pDef(ptScriptState->snoScript);




            }

        }






        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        idScript = ptLuaScriptGlobals->ptScriptStates->ElementNew();



        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementGet(idScript);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        lua_State * ptParentLuaState = ptLuaScriptGlobals->ptHydraState;

        ptScriptState->ptLuaState = NULL;
        ptScriptState->snoScript = snoScript;
        ptScriptState->ptParentLuaState = ptParentLuaState;
        ptScriptState->dwLoadMachineTime = 0;
        ptScriptState->nAdventureParentIndex = nAdventureParentIndex;

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
        sScriptAllocAndLoadScriptState(ptScriptState);

        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        BOOL bHasRangeMessages = sScriptSNOHasRangeMessages(snoScript);

        if (ptConductor != NULL)
        {
            ptScriptState->bConductor = TRUE;
            bHasRangeMessages = TRUE;
        }
        if (bHasRangeMessages)
        {
            ptLuaScriptGlobals->ptRangeEventScriptStates->push_back(ptScriptState);
        }
    }

    return idScript;
}


ScriptID LuaScriptGetScriptID(SNO snoScript)
{
    return sScriptLookupScriptID(snoScript);
}
























































POS LuaScriptGetFirstAttachment(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    POS pos = NULL;
    if (ptLuaScriptGlobals != NULL)
    {
        ScriptState * ptScriptState = sTryGetScriptStateWithScriptID(idScript);
        if (ptScriptState != NULL)
        {
            PooledScriptAttachmentList * listAttachments;
            if (ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(ptScriptState->snoScript, listAttachments))
                pos = listAttachments->GetHeadPosition();
        }
    }
    return pos;
}








void LuaScriptGetNextAttachment(POS & pos, ScriptAttachmentType & eType, DataID & idAttachment, BOOL & bFlaggedForDeletion)
{
    ScriptAttachment & tAttachment = PooledScriptAttachmentList::sGetNext(pos);
    eType = tAttachment.eAttachType;
    idAttachment = tAttachment.idAttachment;
    bFlaggedForDeletion = tAttachment.bFlaggedForDeletion;
}







void LuaScriptEnableListener(POS pos, int32 nListenerIdx, BOOL bEnabled)
{
    ScriptAttachment & tAttachment = *(ScriptAttachment *)pos;
    if (bEnabled)
        tAttachment.dwDisabledEvents &= ~(1ULL << nListenerIdx);
    else
        tAttachment.dwDisabledEvents |= (1ULL << nListenerIdx);
}



BOOL LuaScriptIsListenerEnabled(POS pos, int32 nListenerIdx)
{
    ScriptAttachment & tAttachment = *(ScriptAttachment *)pos;
    return (tAttachment.dwDisabledEvents & (1ULL << nListenerIdx)) == 0;
}





BOOL LuaScriptIsListenerInRange(POS pos, WorldPlace & tPlace, float flRange)
{
    ScriptAttachment & tAttachment = *(ScriptAttachment *)pos;
    BOOL bInRange = FALSE;
    if (tAttachment.eAttachType == SCRIPT_ATTACH_ACTOR)
    {
        ActorCommonData * ptACD = ACDTryToGet((ACDID)tAttachment.idAttachment);
        if (ptACD != NULL && ptACD->tLocation.tWorldLocation.idSWorld == tPlace.idSWorld)
        {
            float flRadius = ptACD->tLocation.tWorldLocation.wsBounds.wdRadius + flRange;
            float flRadiusSqr = flRadius * flRadius;
            bInRange = Vector3D::DistanceSquared(ptACD->tLocation.tWorldLocation.transform.wp, tPlace.wp) < flRadiusSqr;
        }
    }
    return bInRange;
}



















void LuaScriptFreeStateWithScriptID(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (ptLuaScriptGlobals != NULL)
    {


        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
        if (ptScriptState != NULL)
        {



            ScriptStatesPointerArray & tRangeEventScriptStates = *ptLuaScriptGlobals->ptRangeEventScriptStates;
            size_t nRangeEventScriptStates = tRangeEventScriptStates.size();
            for (uint32 i = 0; i < nRangeEventScriptStates; i++)
            {
                if (tRangeEventScriptStates[i] == ptScriptState)
                {
                    blzext::vector_erase_at(tRangeEventScriptStates, i);
                    break;
                }
            }





            sScriptCloseAndFreeScriptState(ptScriptState);
            ptLuaScriptGlobals->ptScriptStates->ElementDelete(ptScriptState);
        }
    }
}

void LuaScriptFreeStateWithSNO(SNO snoScript)
{
    ScriptID idScript = sScriptLookupScriptID(snoScript);
    if (idScript != SCRIPTID_INVALID)
        LuaScriptFreeStateWithScriptID(idScript);
}






lua_State * LuaScriptGetLuaStateWithSNO(SNO snoScript)
{
    lua_State * ptLuaState = NULL;
    if (snoScript != -1)
    {
        ScriptID idScript = LuaScriptGetScriptID(snoScript);
        if (idScript == SCRIPTID_INVALID)
            idScript = LuaScriptCreateState(snoScript, 0xFFFF, NULL);
        if (idScript != SCRIPTID_INVALID)
            ptLuaState = LuaScriptTryGetLuaState(idScript);
    }
    return ptLuaState;
}












lua_State * LuaScriptGetLuaStateWithScriptID(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    lua_State * ptLuaState = NULL;



    ptLuaState = ptLuaScriptGlobals->ptScriptStates->ElementGet(idScript)->ptLuaState;

    return ptLuaState;
}




lua_State * LuaScriptTryToGetLuaStateWithScriptID(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (ptLuaScriptGlobals != NULL && ptLuaScriptGlobals->ptScriptStates != NULL)
    {
        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
        if (ptScriptState != NULL)
            return ptScriptState->ptLuaState;
    }
    return NULL;
}



























lua_State * LuaScriptTryToGetLuaStateWithScriptInstanceID(ScriptInstanceID idScriptInstance)
{
    if (!GameCommonDataExists())
        return NULL;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    lua_State * ptLuaState = NULL;
    if (ptLuaScriptGlobals != NULL)
    {
        ScriptInstance * ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->ElementTryToGet(idScriptInstance);
        if (ptScriptInstance != NULL)
            ptLuaState = ptScriptInstance->ptLuaState;
    }
    return ptLuaState;
}











lua_State * LuaScriptTryGetLuaState(ScriptID idScript)
{
    lua_State * ptLuaState = NULL;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (idScript != SCRIPTID_INVALID && ptLuaScriptGlobals != NULL)
    {
        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
        if (ptScriptState != NULL)
            ptLuaState = ptScriptState->ptLuaState;
    }
    return ptLuaState;
}
















SNO LuaScriptGetSNO(ScriptID idScript)
{
    SNO snoScript = -1;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (idScript != SCRIPTID_INVALID && ptLuaScriptGlobals != NULL)
    {
        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
        if (ptScriptState != NULL)
            snoScript = ptScriptState->snoScript;
    }
    return snoScript;
}







SNO LuaScriptInstanceTryToGetSNO(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        return LuaScriptGetSNO(ptScriptInstance->idScript);
    return -1;
}






void LuaScriptInstanceGetAttachmentIDAndType(ScriptInstanceID idScriptInstance, DataID & idAttachment, ScriptAttachmentType & eAttachmentType)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);

    idAttachment = ptScriptInstance->idAttachment;
    eAttachmentType = ptScriptInstance->eAttachmentType;
}





SNO LuaScriptGetSNOForLuaState(lua_State * ptLuaState)
{
    SNO snoScript = -1;
    if (!GameCommonDataExists())
        return snoScript;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (ptLuaScriptGlobals == NULL)
        return snoScript;

    {
        ScriptStateIterator tIt;
        while (ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->Iterate(tIt))
        {
            if (ptScriptState->ptLuaState == ptLuaState)
            {
                snoScript = ptScriptState->snoScript;
                break;
            }
        }
    }

    if (snoScript == -1)
    {
        ScriptInstanceIterator tIt;
        while (ScriptInstance * ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->Iterate(tIt))
        {
            if (ptScriptInstance->ptLuaState == ptLuaState)
            {
                ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(ptScriptInstance->idScript);
                if (ptScriptState != NULL)
                {
                    snoScript = ptScriptState->snoScript;
                    break;
                }
            }
        }
    }
    return snoScript;
}











SNOName LuaScriptGetSNONameForLuaState(lua_State * ptLuaState)
{

    SNO snoScript = LuaScriptGetSNOForLuaState(ptLuaState);
    SNOName snoName(SCRIPT_SNO, snoScript);


    if (snoScript != -1)
    {
        const ScriptDefinition * pScriptDef = (const ScriptDefinition *)gptGroupDefScript->LockR(snoScript, FALSE);

        if (pScriptDef->snoNameDynamic.snoHandle != -1)
        {
            snoName = pScriptDef->snoNameDynamic;
        }

        gptGroupDefScript->UnlockR((void *&)pScriptDef);
    }


    return snoName;
}






































// LINKER-REMOVED: DWARF concrete DIE 0x0671bc5d (low_pc 0, 0x224 bytes); body from locals, inline tree and line table; its out-of-line call at 3671 is the
// second use that keeps sScriptAllocAndLoadScriptState from being inlined into LuaScriptCreateState.
BOOL LuaScriptReloadScriptWithScriptID(
    ScriptID idScript)
{
    BOOL bReloaded = FALSE;
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

    if (ptLuaScriptGlobals != NULL)
    {
        ScriptState * ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
        // Inference: null check on the lexical block holding nAttachmentCount (line table 3655 only after 609).
        if (ptScriptState != NULL)
        {
            int32 nAttachmentCount = 0;

            PooledScriptAttachmentList * listAttachments;
            if (ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(ptScriptState->snoScript, listAttachments))
            {
                nAttachmentCount = listAttachments->Count();
            }

            lua_State * ptParentLuaState = ptScriptState->ptParentLuaState;

            if (nAttachmentCount > 0)
            {
                sDeActivateListeners(idScript);
            }

            sScriptCloseAndFreeScriptState(ptScriptState);

            ptScriptState->ptParentLuaState = ptParentLuaState;

            sScriptAllocAndLoadScriptState(ptScriptState);

            if (nAttachmentCount > 0)
            {
                sActivateListeners(idScript);
            }
            // Inference: no row of its own; bReloaded becomes w0 only at the 3682 return.
            bReloaded = TRUE;
        }
    }

    return bReloaded;
}

// LINKER-REMOVED: DWARF LuaScript.cpp:3686 (low_pc 0, 0xc0 bytes); body from locals, inline tree and line table; it is the caller of the function above. bReloaded handling inferred from loclist.
BOOL LuaScriptReloadScriptWithSNO(
    SNO snoScript)
{
    BOOL bReloaded = FALSE;
    ScriptID idScript = LuaScriptGetScriptID(snoScript);

    if (idScript != SCRIPTID_INVALID)
    {
        bReloaded = LuaScriptReloadScriptWithScriptID(idScript);
    }

    return bReloaded;
}





























void LuaScriptGetACDVariable(ACDID idACD, LPCSTR sVarName, lua_State * lDest)
{






    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    lua_State * ACDLuaState;
    if (ptLuaScriptGlobals->ptACDScriptStates->Lookup(idACD, ACDLuaState))
    {
        lua_getfield(ACDLuaState, LUA_GLOBALSINDEX, sVarName);
        LuaMove(ACDLuaState, lDest, 1);
    }
    else
    {
        lua_pushnil(lDest);
    }
}

void LuaScriptSetACDVariable(ACDID idACD, LPCSTR sVarName, lua_State * lSrc)
{






    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL)
    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
        lua_State * ACDLuaState;
        BOOL bFound = ptLuaScriptGlobals->ptACDScriptStates->Lookup(idACD, ACDLuaState);
        if (!bFound)
        {
            CRefString sStateName;
            sStateName.Format("%s_ACDVariables", MarkerNameGetString(ptACD->tName));
            ACDLuaState = sAllocNewLuaState(NULL, sStateName.str());
            ptLuaScriptGlobals->ptACDScriptStates->SetAt_Tracked(idACD, ACDLuaState);
        }
        LuaMove(lSrc, ACDLuaState, 1);
        lua_setfield(ACDLuaState, LUA_GLOBALSINDEX, sVarName);
    }
}

void LuaScriptDisposeACDVariableList(ACDID idACD)
{

    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    lua_State * ACDLuaState;
    if (ptLuaScriptGlobals->ptACDScriptStates->Lookup(idACD, ACDLuaState))
    {
        sScriptCloseAndFreeLuaState(ACDLuaState, NULL, NULL);
        ptLuaScriptGlobals->ptACDScriptStates->RemoveAt(idACD);
    }
}






// Recovered sole inline call uses NULL parent/name; no discarded parent-state branch is inferred.
static void sScriptCloseAndFreeLuaStateProfiled(
    lua_State * ptLuaState,
    lua_State * ptLuaParentState,
    LPCSTR szStateName)
{

    Stopwatch tStopwatch;
    float flTime;

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (gpLuaHook)
    {

        gpLuaHook->DetachFromLuaState(ptLuaState);
    }
    else
    {

        lua_sethook(ptLuaState, sServerLuaHook, 0, 0);
    }

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


































    lua_gc(ptLuaState, LUA_GCCOLLECT, 0);


    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
        LuaScriptAllocSetToShutdown(ptLuaScriptGlobals->ptLuaScriptAlloc);


        lua_close(ptLuaState);
    }

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

}


static void sScriptCloseAndFreeLuaState(
    lua_State * ptLuaState,
    lua_State * ptLuaParentState,
    LPCSTR szStateName)
{

    Stopwatch tStopwatch;
    float flTime;

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (gpLuaHook)
    {

        gpLuaHook->DetachFromLuaState(ptLuaState);
    }
    else
    {

        lua_sethook(ptLuaState, sServerLuaHook, 0, 0);
    }

    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (ptLuaParentState)
    {

        lua_pushnil(ptLuaParentState);
        if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }

        lua_setfield(ptLuaParentState, LUA_GLOBALSINDEX, szStateName);

        if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
    }
    else
    {









        lua_close(ptLuaState);
    }
    // Inference: final verbose timing block shares line0 instructions with the earlier timing blocks.
    if (SGameIsValid() && (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING)))
    {
        flTime = tStopwatch.Check();
        tStopwatch.Reset();
    }

}


static void sScriptCloseAndFreeScriptState(
    ScriptState * ptScriptState)
{
    if (GameCommonDataExists())
    {

        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

        if (ptLuaScriptGlobals)
        {
            ScriptInstanceIterator tIter;

            while (ptLuaScriptGlobals->ptScriptInstances->Iterate(tIter))
            {
                ScriptInstance * ptScriptInst = tIter.ptElement;

                if (ptScriptInst->idScript == ptScriptState->id)
                {
                    CRefString sScriptName = SNOToString(LuaScriptGetSNONameForLuaState(ptScriptState->ptLuaState), FALSE);




                    sScriptCloseInstance(ptScriptInst);
                }
            }
        }
    }

    sDeActivateListeners(ptScriptState->id);

    for (int32 i = 0; i < ptScriptState->nNumEventHandlers; ++i)
    {
        ScriptEventRemoveListener(ptScriptState->arEventHandlers[i]);
    }
    sScriptRemoveFromMap(ptScriptState);

    CRefString sScriptStateName;
    sScriptStateName.Format("Script_%08X_%08X", ptScriptState->snoScript, ptScriptState);

    sScriptCloseAndFreeLuaState(ptScriptState->ptLuaState, ptScriptState->ptParentLuaState, sScriptStateName.str());

    // Inference: the adjacent null stores share source line4001 in the original.
    ptScriptState->ptLuaState = ptScriptState->ptParentLuaState = NULL;
}


void LuaScriptCloseInstance(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        sScriptCloseInstance(ptScriptInstance);
}





static void sScriptCloseInstance(
    ScriptInstance * ptScriptInstance)
{
    Stopwatch tStopwatch;
    float flTime;


    ScriptInstanceID idScriptInstance = ptScriptInstance->id;
    // Inference: the unused executing-state check retains its observable TLS lookup (inline DIE at 2702).
    LuaScriptScriptIsExecuting(idScriptInstance);




    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    ScriptAttachmentType eAttachmentType = ptScriptInstance->eAttachmentType;
    DataID idAttachment = ptScriptInstance->idAttachment;

    ScriptState * ptScriptParentState = sGetScriptStateWithScriptID(ptScriptInstance->idScript);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    lua_State * ptLuaParentState = ptScriptParentState->ptLuaState;

    const int32 nBufferSize = 34;
    CHAR sScriptStateName[nBufferSize];
    Blizzard::String::Format(sScriptStateName, nBufferSize, "ScriptInstance_%08X_%08X", ptScriptParentState->snoScript, ptScriptInstance);


    if (!gptGroupDefScript->WasDynamicallyCreated(ptScriptParentState->snoScript) && SHELL_APP_STATE_GET() != APPSTATE_FINISHED)
    {
        ScopedScriptDefinitionLockR pdefScript(ptScriptParentState->snoScript);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    sScriptCloseAndFreeLuaState(ptScriptInstance->ptLuaState, ptLuaParentState, sScriptStateName);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ACDID idACDRef;
    int32 nRefValue;
    POS posRef = ptScriptInstance->mapRefACDs.GetStartAssoc();
    while (posRef)
    {
        ptScriptInstance->mapRefACDs.GetNextAssoc(posRef, idACDRef, nRefValue);
        sRemoveRefACDID(idACDRef);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    ptLuaScriptGlobals->ptScriptInstances->ElementDeleteByID(ptScriptInstance->id);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (eAttachmentType == SCRIPT_ATTACH_POWER)
    {
        Power * ptPower = PowerTryToGet(idAttachment);
        if (ptPower)
        {
            POS posInList = ptPower->listScriptInstanceIDs.Find(idScriptInstance);
            // Inference: Find failure bypasses RemoveAt; branch is attributed to the inlined Find.
            if (posInList)
                ptPower->listScriptInstanceIDs.RemoveAt(posInList);
        }


        PowerCheckForDone(idAttachment);
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
    }
}




























































































































































































































































































































































































































































































































































static void sAddRefACDID(ACDID idACD)
{
    int32 nRefCount;
    ACDIDIntMap * ptInUseACDs = SGameManager::GameGet().ptLuaScriptGlobals->ptInUseACDs;



    if (ptInUseACDs->Lookup(idACD, nRefCount))
    {
        ptInUseACDs->SetAt_Tracked(idACD, nRefCount + 1);
    }
    else
    {
        ptInUseACDs->SetAt_Tracked(idACD, 1);
    }
}


static void sRemoveRefACDID(ACDID idACD)
{
    int32 nRefCount;
    ACDIDIntMap * ptInUseACDs = SGameManager::GameGet().ptLuaScriptGlobals->ptInUseACDs;



    if (ptInUseACDs->Lookup(idACD, nRefCount))
    {
        if (nRefCount > 1)
        {
            ptInUseACDs->SetAt_Tracked(idACD, nRefCount - 1);
        }
        else
        {
            ptInUseACDs->RemoveAt(idACD);
        }
    }
}


void LuaScriptAddRefACD(ACDID idACD)
{

    sAddRefACDID(idACD);
}


void LuaScriptReleaseRefACD(ACDID idACD)
{

    sRemoveRefACDID(idACD);
}


static lua_State * sAllocNewLuaState(
    lua_State * ptParentState,
    LPCSTR szStateName)
{
    lua_State * ptNewState;
    LuaSetInitializingScript(szStateName);

    if (ptParentState != NULL)
    {


        lua_State * L = ptParentState;

        int32 nTop = lua_gettop(L);

        ptNewState = lua_newthread(L);

        lua_newtable(L);
        lua_newtable(L);

        lua_getfenv(L, nTop + 1);
        lua_setfield(L, -2, "__index");

        lua_setmetatable(L, -2);


        lua_setfenv(L, nTop + 1);



        lua_setglobal(L, szStateName);

        int32 nTopEnd = lua_gettop(L);
    }

    else
    {
        LuaScriptAllocator * ptLuaAlloc = NULL;

        if (ServerIsLocal())
        {
            BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter();
            LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

            if (ptLuaScriptGlobals != NULL)
            {
                ptLuaAlloc = ptLuaScriptGlobals->ptLuaScriptAlloc;
            }
            GameServerCodeLeave_Tracked(bReturnToClient);
        }

        ptNewState = lua_newstate(LuaScriptAlloc, ptLuaAlloc);
    }

    if (gpLuaHook != NULL)
    {

        gpLuaHook->AttachToLuaState(ptNewState);
    }
    else
    {

        lua_sethook(ptNewState, sServerLuaHook, LUA_MASKRET | LUA_MASKCOUNT, 100);
    }


    LuaClearInitializingScript();
    return ptNewState;
}


static void sScriptAllocAndLoadScriptState(
    ScriptState * ptScriptState)
{
    Stopwatch tStopwatch;
    float flTime;

    CRefString sScriptStateName;




    sScriptStateName.Format("Script_%08X_%08X", ptScriptState->snoScript, ptScriptState);


    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ptScriptState->ptLuaState = sAllocNewLuaState(ptScriptState->ptParentLuaState, sScriptStateName.str());

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(ptScriptState->id, "alloc script", NULL, INVALID_SNO, DATAID_INVALID, SCRIPT_ATTACH_NONE);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptPushExecutingScript(idScriptInstance);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    sScriptLoadScriptState(ptScriptState);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (ptScriptState->snoScript != SNO_NONE)
    {
        ptScriptState->nNumEventHandlers = ScriptEventInitializeForState(ptScriptState->snoScript, ptScriptState->id, ptScriptState->ptLuaState, ptScriptState->arEventHandlers, MAX_SCRIPT_EVENT_HANDLERS);
    }

    LuaScriptPopExecutingScript();

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptCloseInstance(idScriptInstance);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

}

































static const char * sLuaReader(lua_State * L, void * ud, size_t * sz)
{
    MyLuaReader * pLuaReader = (MyLuaReader *)ud;

    float flTime;
    Stopwatch tStopwatch;
    const char * pReturn = NULL;


    *sz = 0;

    if (!pLuaReader->bReadCompleted)
    {
        pLuaReader->bReadCompleted = TRUE;

        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
        CompiledScriptData tCompiledScriptData;
        BOOL bCached = ptLuaScriptGlobals->mapSNOCompiledScripts->Lookup(pLuaReader->pScriptDef->tHeader.snoHandle, tCompiledScriptData);
        if (bCached)
        {
            *sz = tCompiledScriptData.nSize;
            pReturn = (const char *)tCompiledScriptData.pCompiledScript;
        }
        else
        {
            uint8 * pData = pLuaReader->pScriptDef->pCompiledData;


            *sz = pLuaReader->pScriptDef->serCompiledData.dwSize;
            pReturn = (const char *)pData;
        }
    }


    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    return pReturn;
}

int32 CustomLuaLoader(const ScriptDefinition * ptScriptDef, lua_State * ptLuaState)
{


    Stopwatch tStopwatch;
    float flTime;



    MyLuaReader tLuaReader;
    tLuaReader.pScriptDef = ptScriptDef;
    tLuaReader.bReadCompleted = FALSE;


    CRefString sFileName("0000.lua");













    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    int32 nError = lua_load(ptLuaState, sLuaReader, &tLuaReader, sFileName.str());

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
























    return nError;
}
static uint32 sLoadScriptIntoLuaState(
    SNO snoScript,
    lua_State * ptLuaState)
{
    Stopwatch tStopwatch;
    float flTime;

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
    uint32 dwMachineTime;
    int32 nError;
    {
        ScopedScriptDefinitionLockR ptScriptDef(snoScript);
        dwMachineTime = ptScriptDef->dwMachineTime;
        nError = CustomLuaLoader(ptScriptDef, ptLuaState);
    }

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (nError == 0)
    {


        LuaSNOScriptCall(snoScript, ptLuaState, 0, LUA_MULTRET, "Global");






    }
    else
    {
        LPCSTR szCause;






        {
            ScopedScriptDefinitionLockR ptScriptDef(snoScript);





        }


        lua_isstring(ptLuaState, -1);
    }



    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    return dwMachineTime;
}















static void sScriptLoadScriptState(
    ScriptState * ptScriptState)
{
    Stopwatch tStopwatch;
    float flTime;

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    ptScriptState->dwLoadMachineTime = 0;

    if (ptScriptState->snoScript != SNO_NONE)
    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;

        ptLuaScriptGlobals->mapSNOScriptID->SetAt_Tracked(ptScriptState->snoScript, ptScriptState->id);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

        ptScriptState->dwLoadMachineTime = sLoadScriptIntoLuaState(ptScriptState->snoScript, ptScriptState->ptLuaState);

    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_VERBOSE_POWER_LOGGING))
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
    }
}






static void sScriptRemoveFromMap(ScriptState * ptScriptState)
{
    if (ptScriptState->snoScript != -1)
    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;



        ptLuaScriptGlobals->mapSNOScriptID->SetAt_Tracked(ptScriptState->snoScript, SCRIPTID_INVALID);
    }
}

static ScriptID sScriptLookupScriptID(SNO snoScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    if (ptLuaScriptGlobals != NULL)
    {
        ScriptID idScript;
        if (ptLuaScriptGlobals->mapSNOScriptID->Lookup(snoScript, idScript))
            return idScript;
    }
    return SCRIPTID_INVALID;
}




static ScriptState * sGetScriptStateWithScriptID(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;



    return ptLuaScriptGlobals->ptScriptStates->ElementGet(idScript);
}





static ScriptState * sGetScriptStateWithScriptInstanceID(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);

    return sGetScriptStateWithScriptID(ptScriptInstance->idScript);
}



static ScriptState * sTryGetScriptStateWithScriptID(ScriptID idScript)
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    ScriptState * ptScriptState = NULL;




    if (ptLuaScriptGlobals != NULL)
        ptScriptState = ptLuaScriptGlobals->ptScriptStates->ElementTryToGet(idScript);
    return ptScriptState;
}



















static ScriptInstance * sGetScriptInstance(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = NULL;


    if (idScriptInstance != SCRIPTINSTANCEID_INVALID)
    {
        LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
        ptScriptInstance = ptLuaScriptGlobals->ptScriptInstances->ElementTryToGet(idScriptInstance);
    }
    return ptScriptInstance;
}






void LuaScriptSaveForDeactivatingActor(ActorID idActor, SavedDataKey * ptSavedDataKey)
{
    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
        return;

    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;



    SigmaMemoryClear(ptSavedDataKey, sizeof(SavedDataKey));
    ptSavedDataKey->idFirstBlock = -1;

    SavedDataAdapter tInactiveSaveAdapter(ptSavedDataKey, ptLuaScriptGlobals->ptInactiveSaveBlockArray);

    int32 nScripts = ptActor->listSNOScripts.Count();
    if (nScripts > 0)
    {
        POS posScript = ptActor->listSNOScripts.GetHeadPosition();
        while (posScript)
        {


            SNO snoScript = ptActor->listSNOScripts.GetNext(posScript);
            PooledScriptAttachmentList * listAttachments;
            if (ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(snoScript, listAttachments))
            {
                POS pos = listAttachments->GetHeadPosition();
                while (pos)
                {
                    ScriptAttachment & tAttachment = listAttachments->GetNext(pos);
                    if (tAttachment.idAttachment == ptActor->idACD)
                    {
                        tInactiveSaveAdapter.SaveUInt64(tAttachment.dwDisabledEvents);
                        break;
                    }
                }
            }
        }
    }
}






void LuaScriptRestoreForActivatingActor(ActorID idActor, SavedDataKey * ptSavedDataKey)
{
    if (ptSavedDataKey == NULL || ptSavedDataKey->idFirstBlock == -1)
        return;

    Actor * ptActor = ActorTryToGet(idActor);
    if (ptActor == NULL)
        return;

    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;




    SavedDataAdapter tInactiveSaveAdapter(ptSavedDataKey, ptLuaScriptGlobals->ptInactiveSaveBlockArray);


    int32 nScripts = ptActor->listSNOScripts.Count();
    if (nScripts > 0)
    {
        POS posScript = ptActor->listSNOScripts.GetHeadPosition();
        while (posScript)
        {

            SNO snoScript = ptActor->listSNOScripts.GetNext(posScript);
            PooledScriptAttachmentList * listAttachments;
            if (ptLuaScriptGlobals->ptScriptAttachmentMap->Lookup(snoScript, listAttachments))
            {
                POS pos = listAttachments->GetHeadPosition();
                while (pos)
                {
                    ScriptAttachment & tAttachment = listAttachments->GetNext(pos);
                    if (tAttachment.idAttachment == ptActor->idACD)
                    {
                        tInactiveSaveAdapter.RestoreUInt64(tAttachment.dwDisabledEvents);
                        break;
                    }
                }
            }
        }
    }

    tInactiveSaveAdapter.FreeAllBlocks();
}







int32 LuaScriptGetExecutingStackDepth()
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals != NULL)
        return ptLuaScriptThreadGlobals->nExecutingScriptDepth;
    return 0;
}








void LuaScriptGetExecutingStackString(CRefString & sMsg)
{
    LuaScriptThreadGlobals * ptLuaScriptThreadGlobals = LuaScriptThreadGlobalsGet();
    if (ptLuaScriptThreadGlobals == NULL)
        return;

    for (int32 i = ptLuaScriptThreadGlobals->nExecutingScriptDepth - 1; i >= 0; i--)
    {
        ScriptInstance * ptScriptInstance;
        CRefString sSnoNameStackEntry;

        ptScriptInstance = sGetScriptInstance(ptLuaScriptThreadGlobals->arExecutingScriptStack[i]);
        if (ptScriptInstance != NULL)
        {
            ScriptState * ptScriptState = sGetScriptStateWithScriptInstanceID(ptLuaScriptThreadGlobals->arExecutingScriptStack[i]);
            SNOName snonameScript = LuaScriptGetSNONameForLuaState(ptScriptState->ptLuaState);
            lua_Debug tLuaStateDebug;

            sSnoNameStackEntry = HyperlinkCreateFromSNO(snonameScript, NULL).str();
            sSnoNameStackEntry += CRefString().Format(" - '%s'", ptScriptInstance->szEntryPoint);

            if (lua_getstack(ptScriptState->ptLuaState, 0, &tLuaStateDebug) && lua_getinfo(ptScriptState->ptLuaState, "Snl", &tLuaStateDebug))
            {
                sSnoNameStackEntry += CRefString().Format(", Line %d", tLuaStateDebug.currentline - 1);
            }
        }
        else
        {
            sSnoNameStackEntry = "[Invalid ScriptState]";
        }

        sMsg += CRefString().Format("%d: %s\n", i, sSnoNameStackEntry.str());
    }
}









void LuaScriptPrintExecutingStackString()
{
    CRefString sMsg;
    LuaScriptGetExecutingStackString(sMsg);

}




void LuaScriptResetTimeoutTimer()
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    ptLuaScriptGlobals->dwStartOfCurrentScript = Blizzard::Time::Milliseconds();
}




void LuaScriptConductorFlaggedForAdvancementThisUpdate()
{
    LuaScriptGameGlobals * ptLuaScriptGlobals = SGameManager::GameGet().ptLuaScriptGlobals;
    ptLuaScriptGlobals->bConductorFlaggedForAdvancementThisUpdate = TRUE;
}




void LuaScriptInstanceAttachmentDeleted(ScriptInstanceID idScriptInstance)
{
    ScriptInstance * ptScriptInstance = sGetScriptInstance(idScriptInstance);
    if (ptScriptInstance != NULL)
        ptScriptInstance->bAttachmentDeleted = TRUE;
}









































































































































































































void LuaVerifySnosAreReferenced(const SNO param0, const SNO param1, const SNO param2, const SNO param3, const SNO param4, const SNO param5, const SNO param6)
{
}
