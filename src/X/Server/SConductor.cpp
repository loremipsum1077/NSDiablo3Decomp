// Original file: X/Server/SConductor.cpp

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "SConductor.h"
#include "../Common/CommonTypes.h"
#include "../Common/Game.h"
#include "../Common/ActorCommonData.h"
#include "Actors/Actors.h"
#include "../Common/SNO/SNOFiles.h"
#include "SGame.h"
#include "SGameManager.h"
#include "Definitions/ConductorDefinition.h"
#include "Definitions/ScriptDefinition.h"
#include "LuaScript/LuaScript.h"
#include "LuaScript/LuaScriptUtil.h"
#include "LuaScript/ScriptEvent.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../../BlizzardCore/Include/Blizzard/Lock.h"

// Declared here from their definitions elsewhere (headers not adopted yet).
void ScriptDefinitionDisposeScript(SNO snoScript);  // X/Server/Definitions/ScriptDefinition.cpp:906
uint32 StringHashI(LPCSTR pszName);  // X/Sigma/TypeDescriptor/SymbolicValue.cpp
void LuaScriptConductorFlaggedForAdvancementThisUpdate();  // X/Server/LuaScript/LuaScript.cpp:5416
void LuaScriptAddRefACD(ACDID idACD);  // X/Server/LuaScript/LuaScript.cpp:4697
void LuaScriptReleaseRefACD(ACDID idACD);  // X/Server/LuaScript/LuaScript.cpp:4704
CRefString ConductorGetSequenceInterruptedFunctionName(int32 nPhaseIndex, int32 nSequenceIndex);  // X/Server/Definitions/ConductorDefinition.cpp
CRefString ConductorGetPhaseInitFunctionName(int32 nPhaseIndex);  // X/Server/Definitions/ConductorDefinition.cpp
void ScriptEventInitializeForConductorPhase(ConductorInstance & tConductorInstance, int32 nPhase);  // X/Server/LuaScript/ScriptEvent.cpp
void LuaScriptFreeStateWithScriptID(ScriptID idScript);  // X/Server/LuaScript/LuaScript.cpp
static const int32 CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES = 20;  // SConductor.h:33
static const int32 CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES = 20;  // SConductor.h:34
static const int32 CONDUCTOR_MAX_REFERENCED_ACDID = 128;  // SConductor.h:36

// ConductorGlobals (SConductor.h:58) is kept local: the shared header leaves it out.
struct ConductorGlobals {  // size 0x110
    UpdateData tConductorUpdateData;  // 0x00
    SNOArray tConductorCompletedArray;  // 0x20
    XListMemoryPool<ConductorInstance> tConductorInstancePool;  // 0x58
    XPooledList<ConductorInstance, ConductorInstance> tConductorInstanceList;  // 0x88
    XListMemoryPool<RepeatableScriptInstanceData> tRepeatableScriptInstanceDataPool;  // 0xb0
    XMapMemoryPool<ACDID, int> tMapRefACDsPool;  // 0xe0
};
static_assert(sizeof(ConductorGlobals) == 0x110, "ConductorGlobals");

static BOOL SConductorIsScriptBeingUsed(SNO snoScript);
void SConductorStopAllConductors(SNO snoConductor);
static void sSConductorStartPhase(ConductorInstance & tConductorInstance, uint32 nPhase);
static BOOL sSConductorCleanUpCurrentPhase(ConductorInstance & tConductorInstance);
static void sSConductorStopConductor(ConductorInstance * ptConductorInstance);




enum { BLOCK_COUNT_CONDUCTOR_REFERENCED_ACDID = 512 };






static inline void sSConductorPurgeUnusedScripts()
{
    Blizzard::Lock::ScopedMutex scopedMutex(sgtAppGlobals.tConductorDefPurgeMutex);




    int32 nCount = sgtAppGlobals.ptConductorDefsPurged->GetArraySize();
    for (int32 i = 0; i < nCount; i++)
    {
        SNO snoScript = (*sgtAppGlobals.ptConductorDefsPurged)[i];


        if (!SConductorIsScriptBeingUsed(snoScript)) ScriptDefinitionDisposeScript(snoScript);
    }
    sgtAppGlobals.ptConductorDefsPurged->EmptyArray();
}























void SConductorInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;
    ptConductorGlobals = (ConductorGlobals *)SigmaMemoryNew(sizeof(ConductorGlobals), 4, AppGetCurrentMemoryInterface(), TRUE);

    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    ptConductorGlobals->tConductorCompletedArray.Initialize();
    ptConductorGlobals->tConductorInstancePool.Initialize();
    ptConductorGlobals->tConductorInstancePool.SetMemoryInterface(ptMemoryInterface);
    ptConductorGlobals->tConductorInstanceList.SetPool(&ptConductorGlobals->tConductorInstancePool);

    ptConductorGlobals->tRepeatableScriptInstanceDataPool.Initialize();
    ptConductorGlobals->tRepeatableScriptInstanceDataPool.SetMemoryInterface(ptMemoryInterface);

    ptConductorGlobals->tMapRefACDsPool.Initialize();
    ptConductorGlobals->tMapRefACDsPool.SetMemoryInterface(ptMemoryInterface);
    ptConductorGlobals->tMapRefACDsPool.SetBlockCount_Tracked(BLOCK_COUNT_CONDUCTOR_REFERENCED_ACDID);

    UpdateFunctionInitialize(UPDATE_CONDUCTORS, DATAID_INVALID, ptConductorGlobals->tConductorUpdateData);
    ServerQueueUpdateFunction(1, ptConductorGlobals->tConductorUpdateData);
}







void SConductorDisposeForGame()
{
    SConductorStopAllConductors(SNO_NONE);
    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;



    if (!GameActiveGameCommonDataIsClient())
    {
        ServerRemoveUpdateFunction(ptConductorGlobals->tConductorUpdateData);
    }




    ptConductorGlobals->tConductorCompletedArray.Dispose();
    ptConductorGlobals->tConductorInstanceList.Dispose();
    ptConductorGlobals->tConductorInstancePool.Dispose();
    ptConductorGlobals->tRepeatableScriptInstanceDataPool.Dispose();
    ptConductorGlobals->tMapRefACDsPool.Dispose();


    sSConductorPurgeUnusedScripts();


    XFREE(ptConductorGlobals, AppGetCurrentMemoryInterface());
}

















































void SConductorUpdate()
{

    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;

    ServerUpdateWasCalled(ptConductorGlobals->tConductorUpdateData);
    ServerQueueUpdateFunction(1, ptConductorGlobals->tConductorUpdateData);


    sSConductorPurgeUnusedScripts();


    POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);
        const ConductorDefinition * ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(tConductorInstance.snoConductor, FALSE);

        BOOL bRefreshNeeded = FALSE;
        BOOL bPhaseDone = TRUE;
        (void)bRefreshNeeded;
        POS posRepeatableInstances = tConductorInstance.tRepeatableScriptInstanceDataList.GetHeadPosition();
        while (posRepeatableInstances)
        {
            POS posCurrentRepeatableInstance = posRepeatableInstances;
            RepeatableScriptInstanceData & tData = tConductorInstance.tRepeatableScriptInstanceDataList.GetNext(posRepeatableInstances);
            lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(tData.idScriptInstance);

            if (ptLuaState)
            {
                if (tData.nPhase == tConductorInstance.nCurrentPhase)
                    bPhaseDone = FALSE;
            }
            else
            {
                tConductorInstance.tRepeatableScriptInstanceDataList.RemoveAt(posCurrentRepeatableInstance);
            }
        }






        for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
        {
            ScriptInstanceID idSequence = tConductorInstance.arPhaseScripts[i];
            if (idSequence != -1)
            {
                if (LuaScriptTryToGetLuaStateWithScriptInstanceID(idSequence))
                {
                    bPhaseDone = FALSE;
                }
                else
                {


                    tConductorInstance.arPhaseScripts[i] = -1;
                }
            }



            if (tConductorInstance.arPhaseEventHandlers[i] != -1)
                bPhaseDone = FALSE;
        }





        lua_State * ptLuaStatePhaseInit = LuaScriptTryToGetLuaStateWithScriptInstanceID(tConductorInstance.idPhaseInitScript);











        if (bPhaseDone && ptLuaStatePhaseInit == NULL)
        {



            if (tConductorInstance.nCurrentPhase + 1 < ptConductorDef->nNumPhases)
            {
                SGameGlobalsGet();








                tConductorInstance.nCurrentPhase++;
                sSConductorStartPhase(tConductorInstance, tConductorInstance.nCurrentPhase);
            }
            else
            {
                SGameGlobalsGet();








                tConductorInstance.bFlaggedForDeletion = TRUE;
            }
        }





        if (tConductorInstance.bFlaggedForDeletion)
        {




            sSConductorStopConductor(&tConductorInstance);
            ptConductorGlobals->tConductorInstanceList.RemoveAt(posCurrent);
        }



        gptGroupDefConductor->UnlockR((void *&)ptConductorDef);
    }
}


static void sSConductorStartNonYieldingScript(ConductorInstance & tConductor, const char * szFunctionName)
{





    lua_State * ptLuaState = LuaScriptGetLuaStateWithScriptID(tConductor.idGlobalScript);


    if (ptLuaState == NULL)
        return;


    if (LuaScriptGetFunction(ptLuaState, szFunctionName))
    {
        int32 nNumParams = 2;
        int32 nNumReturnValues = 0;
        lua_pushinteger(ptLuaState, tConductor.idAttachment);
        lua_pushstring(ptLuaState, LuaScriptAttachmentTypeToString(tConductor.eAttachType));



        ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(tConductor.idGlobalScript, szFunctionName, NULL, -2, -1, 0);
        LuaCall(idScriptInstance, ptLuaState, nNumParams, nNumReturnValues, szFunctionName);
        LuaScriptCloseInstance(idScriptInstance);
    }
}






static ScriptInstanceID sSConductorStartScript(ConductorInstance * ptConductor, ScriptID idScript, SNO snoScript, const char * szFunctionName, lua_State * ptParent)
{











    ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(idScript, szFunctionName, ptParent, snoScript, -1, 0);

    if (idScriptInstance != -1)
    {
        lua_State * ptLuaState = LuaScriptGetInstanceState(idScriptInstance);







        if (LuaScriptGetFunction(ptLuaState, szFunctionName))
        {
            int32 nNumParams = 2;

            lua_pushinteger(ptLuaState, ptConductor->idAttachment);
            lua_pushstring(ptLuaState, LuaScriptAttachmentTypeToString(ptConductor->eAttachType));

            LuaScriptSetInstanceResumeTick(idScriptInstance, GameGetActiveGameCommonData().dwServerTime, nNumParams);
        }
    }
    return idScriptInstance;
}






static void sSConductorStartPhase(ConductorInstance & tConductorInstance, uint32 nPhase)
{



    const ConductorDefinition * ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(tConductorInstance.snoConductor, FALSE);

    SNO snoScript = ptConductorDef->snoScript;
    gptGroupDefConductor->UnlockR((void *&)ptConductorDef);



    sSConductorStartNonYieldingScript(tConductorInstance,
        (const char *)

        ConductorGetPhaseInitFunctionName(nPhase));
    BOOL arStartSequences[20];

























    if (tConductorInstance.nCurrentPhase != nPhase)
        return;



    if (tConductorInstance.bFlaggedForAdvancement || tConductorInstance.bFlaggedForPhaseSkip)
        return;



    if (nPhase == 0)
    {
        for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES; i++)
        {
            tConductorInstance.arGlobalScripts[i] = -1;
        }
    }

    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
    {
        tConductorInstance.arPhaseScripts[i] = -1;
    }

    ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(tConductorInstance.snoConductor, FALSE);

    ConductorPhase & tPhase = ptConductorDef->arPhases[nPhase];


    for (uint32 i = 0; i < 20; i++)
    {
        arStartSequences[i] = FALSE;
    }


    for (uint32 i = 0; i < tPhase.nNumSequences; i++)
    {


        if (tPhase.arSequences[i].arScriptMessageMap[0].eEventType == EVENT_INVALID)
        {
            arStartSequences[i] = TRUE;
        }
    }


    gptGroupDefConductor->UnlockR((void *&)ptConductorDef);

    for (uint32 i = 0; i < 20; i++)
    {
        if (arStartSequences[i])
        {

            ScriptInstanceID idSequence = sSConductorStartScript(&tConductorInstance, tConductorInstance.idGlobalScript, snoScript,
                (const char *)



                ConductorGetSequenceStartFunctionName(nPhase, i),
                tConductorInstance.ptConductorGlobalState);
            if (nPhase == 0)
                tConductorInstance.arGlobalScripts[i] = idSequence;
            else
                tConductorInstance.arPhaseScripts[i] = idSequence;
        }
    }



    ScriptEventInitializeForConductorPhase(tConductorInstance, nPhase);

    if (nPhase == 0)
    {



        for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES; i++)
        {
            if (tConductorInstance.arGlobalEventHandlers[i] != -1)
                ScriptEventActivateListener(tConductorInstance.arGlobalEventHandlers[i], tConductorInstance.idGlobalScript);
        }
    }
    else
    {



        for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
        {
            if (tConductorInstance.arPhaseEventHandlers[i] != -1)
                ScriptEventActivateListener(tConductorInstance.arPhaseEventHandlers[i], tConductorInstance.idGlobalScript);
        }
    }
}




void SConductorInstanceStart(SNO snoConductor)
{

    SGame & tSGame = SGameManager::GameGet();




    const ConductorDefinition * ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(snoConductor, FALSE);
    int32 nNumPhases = ptConductorDef->nNumPhases;
    SNO snoScript = ptConductorDef->snoScript;
    BOOL bRunOnlyOnce = ptConductorDef->bRunOnlyOnce;
    gptGroupDefConductor->UnlockR((void *&)ptConductorDef);

    if (nNumPhases == 0)
        return;





    if (bRunOnlyOnce)
    {
        for (int32 i = 0; i < tSGame.ptConductorGlobals->tConductorCompletedArray.GetArraySize(); i++)
        {
            SNO snoCompletedConductor = tSGame.ptConductorGlobals->tConductorCompletedArray[i];
            if (snoCompletedConductor == snoConductor)
                return;
        }





        tSGame.ptConductorGlobals->tConductorCompletedArray.Add_Tracked(snoConductor);
    }









    ConductorInstance tConductorInstance;
    SigmaMemoryClear(&tConductorInstance, sizeof(tConductorInstance));
    tConductorInstance.snoConductor = snoConductor;
    tConductorInstance.nCurrentPhase = 0;
    tConductorInstance.bFlaggedForDeletion = FALSE;
    tConductorInstance.bFlaggedForAdvancement = FALSE;
    tConductorInstance.bFlaggedForPhaseSkip = FALSE;
    tConductorInstance.tRepeatableScriptInstanceDataList.SetPool(&tSGame.ptConductorGlobals->tRepeatableScriptInstanceDataPool);

    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES; i++)
    {
        tConductorInstance.arGlobalEventHandlers[i] = -1;
        tConductorInstance.arPhaseEventHandlers[i] = -1;
    }
    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
    {
        tConductorInstance.arGlobalScripts[i] = -1;
        tConductorInstance.arPhaseScripts[i] = -1;
    }

    ScriptInstanceID idCurrentScriptInstance = LuaScriptGetExecutingScriptInstance();
    if (idCurrentScriptInstance != -1)
    {

        LuaScriptInstanceGetAttachmentIDAndType(idCurrentScriptInstance, tConductorInstance.idAttachment, tConductorInstance.eAttachType);
    }

    uint16 nAdventureParentIndex = 0xffff;
    if (tConductorInstance.eAttachType == SCRIPT_ATTACH_ACTOR)
    {

        ACDID idACDAttachment = (ACDID)tConductorInstance.idAttachment;
        ActorID idActor = (ActorID)ACDGetOwner(idACDAttachment);
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
        {

            nAdventureParentIndex = ptActor->tMarkerOwner.nAdventureParentIndex;
        }
    }
    tConductorInstance.idGlobalScript = LuaScriptCreateState(snoScript, nAdventureParentIndex, &tConductorInstance);
    tConductorInstance.ptConductorGlobalState = LuaScriptGetLuaStateWithScriptID(tConductorInstance.idGlobalScript);

    if (tConductorInstance.eAttachType == SCRIPT_ATTACH_ACTOR)
    {
        LuaScriptAddRefACD((ACDID)tConductorInstance.idAttachment);
    }
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;
    ptConductorGlobals->tConductorInstanceList.AddTail_Tracked(tConductorInstance);

    ConductorInstance & tNewConductorInstance = ptConductorGlobals->tConductorInstanceList.GetTail();

    tNewConductorInstance.tMapRefACDs.Initialize(&ptConductorGlobals->tMapRefACDsPool);

    tNewConductorInstance.bFailedToFindAndPurgeOldRefACDs = FALSE;


    sSConductorStartPhase(tNewConductorInstance, tConductorInstance.nCurrentPhase);

    if (nNumPhases > 1)
    {

        tConductorInstance.nCurrentPhase++;
        sSConductorStartPhase(tNewConductorInstance, tConductorInstance.nCurrentPhase);
    }
}





BOOL SConductorActive(SNO snoConductor)
{

    if (GameCommonDataExists())
    {




        SGame & tSGame = SGameManager::GameGet();
        ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;


        POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
        while (pos)
        {
            ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);
            if (tConductorInstance.snoConductor == snoConductor)
                return TRUE;
        }
    }
    return FALSE;
}


































static BOOL sSConductorCleanUpCurrentPhase(ConductorInstance & tConductorInstance)
{

    if (tConductorInstance.nCurrentPhase == 0)
        return FALSE;










    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
    {
        ScriptInstanceID idSequence = tConductorInstance.arPhaseScripts[i];
        if (idSequence != -1)
        {
            lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(idSequence);
            if (ptLuaState)
            {


                if (idSequence != LuaScriptGetExecutingScriptInstance())
                {
                    LuaScriptCloseInstance(idSequence);
                    sSConductorStartNonYieldingScript(tConductorInstance,
                        (const char *)

                        ConductorGetSequenceInterruptedFunctionName(tConductorInstance.nCurrentPhase, i));
                }
            }
        }






        ScriptEventListenerDefinitionID idEventListener = tConductorInstance.arPhaseEventHandlers[i];
        if (idEventListener != -1)
        {


            ScriptEventDeActivateListener(idEventListener);
            ScriptEventRemoveListener(idEventListener);
        }
    }



    if (tConductorInstance.idPhaseInitScript != -1)
    {
        lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(tConductorInstance.idPhaseInitScript);
        if (ptLuaState)
        {
            LuaScriptCloseInstance(tConductorInstance.idPhaseInitScript);
        }
        tConductorInstance.idPhaseInitScript = -1;
    }


    POS pos = tConductorInstance.tRepeatableScriptInstanceDataList.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        RepeatableScriptInstanceData & tRepeatableScriptInstanceData = tConductorInstance.tRepeatableScriptInstanceDataList.GetNext(pos);
        if (tRepeatableScriptInstanceData.nPhase != 0)
        {
            if (tRepeatableScriptInstanceData.nPhase != tConductorInstance.nCurrentPhase)
            {

                lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(tRepeatableScriptInstanceData.idScriptInstance);
                (void)ptLuaState;
            }







            lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(tRepeatableScriptInstanceData.idScriptInstance);
            if (ptLuaState)
            {

                if (tRepeatableScriptInstanceData.idScriptInstance != LuaScriptGetExecutingScriptInstance())
                {
                    LuaScriptCloseInstance(tRepeatableScriptInstanceData.idScriptInstance);
                    sSConductorStartNonYieldingScript(tConductorInstance,
                        (const char *)
                        ConductorGetSequenceInterruptedFunctionName(tRepeatableScriptInstanceData.nPhase, tRepeatableScriptInstanceData.nSequence));
                }
            }

            if (tRepeatableScriptInstanceData.idScriptInstance != LuaScriptGetExecutingScriptInstance())
            {
                tConductorInstance.tRepeatableScriptInstanceDataList.RemoveAt(posCurrent);
            }
        }
    }



    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
    {
        tConductorInstance.arPhaseEventHandlers[i] = -1;
        tConductorInstance.arPhaseScripts[i] = -1;
    }

    return TRUE;
}

void SConductorFlagForAdvancement(ConductorInstance & tConductorInstance)
{

    if (tConductorInstance.bFlaggedForAdvancement)
    {
    }


    else if (tConductorInstance.bFlaggedForPhaseSkip)
    {
    }




    else
    {
        tConductorInstance.bFlaggedForAdvancement = TRUE;
    }
    LuaScriptConductorFlaggedForAdvancementThisUpdate();
}



void SConductorFlagForPhaseSkip(ConductorInstance & tConductorInstance, LPCSTR szPhaseName)
{


    if (tConductorInstance.bFlaggedForAdvancement)
    {
    }


    else if (tConductorInstance.bFlaggedForPhaseSkip)
    {
    }




    else
    {
        tConductorInstance.bFlaggedForPhaseSkip = TRUE;
        Blizzard::String::Copy(tConductorInstance.szPhaseSkipName, szPhaseName, sizeof(tConductorInstance.szPhaseSkipName));
    }
    LuaScriptConductorFlaggedForAdvancementThisUpdate();
}



void SConductorAdvancePhase(ConductorInstance & tConductorInstance)
{




    BOOL bCleanedUp = sSConductorCleanUpCurrentPhase(tConductorInstance);













    SGameGlobalsGet();









    if (bCleanedUp)
    {
        const ConductorDefinition * ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(tConductorInstance.snoConductor, FALSE);
        uint32 nNumPhases = ptConductorDef->nNumPhases;
        gptGroupDefConductor->UnlockR((void *&)ptConductorDef);

        if (tConductorInstance.nCurrentPhase + 1 < nNumPhases)
        {
            tConductorInstance.nCurrentPhase = tConductorInstance.nCurrentPhase + 1;
            sSConductorStartPhase(tConductorInstance, tConductorInstance.nCurrentPhase);
        }
    }
}
void SConductorSkipToPhase(ConductorInstance & tConductorInstance, LPCSTR szPhaseName)
{




    const ConductorDefinition * ptConductorDef = (const ConductorDefinition *)gptGroupDefConductor->LockR(tConductorInstance.snoConductor, FALSE);
    uint32 nNumPhases = ptConductorDef->nNumPhases;

    uint32 nNextPhase = 0;
    uint32 dwPhaseHash = StringHashI(szPhaseName);

    for (uint32 i = 1; i < nNumPhases; i++)
    {

        ConductorPhase & tPhase = ptConductorDef->arPhases[i];





        if (tPhase.dwPhaseHash == dwPhaseHash)
        {
            nNextPhase = i;
            break;
        }
    }
    gptGroupDefConductor->UnlockR((void *&)ptConductorDef);


    if (nNextPhase == 0)
        return;







    if (nNextPhase <= tConductorInstance.nCurrentPhase)
        return;








    if (sSConductorCleanUpCurrentPhase(tConductorInstance))
    {




        SGameGlobalsGet();




        tConductorInstance.nCurrentPhase = nNextPhase;
        sSConductorStartPhase(tConductorInstance, nNextPhase);
    }
}


static void sSConductorStopConductor(ConductorInstance * ptConductorInstance)
{

    SGameGlobalsGet();







    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;



    if (ptConductorInstance->idGlobalInitScript != -1)
    {
        lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(ptConductorInstance->idGlobalInitScript);
        if (ptLuaState)
        {
            LuaScriptCloseInstance(ptConductorInstance->idGlobalInitScript);
        }

        ptConductorInstance->idGlobalInitScript = -1;
    }


    if (ptConductorInstance->idPhaseInitScript != -1)
    {
        lua_State * ptLuaState = LuaScriptTryToGetLuaStateWithScriptInstanceID(ptConductorInstance->idPhaseInitScript);
        if (ptLuaState)
        {
            LuaScriptCloseInstance(ptConductorInstance->idPhaseInitScript);
        }
        ptConductorInstance->idPhaseInitScript = -1;
    }



    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES; i++)
    {
        ScriptInstanceID idSequence = ptConductorInstance->arPhaseScripts[i];
        LuaScriptCloseInstance(idSequence);

        ScriptEventListenerDefinitionID idEventListener = ptConductorInstance->arPhaseEventHandlers[i];
        if (idEventListener != -1)
        {


            ScriptEventDeActivateListener(idEventListener);
            ScriptEventRemoveListener(idEventListener);
        }
    }
    for (int32 i = 0; i < CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES; i++)
    {
        ScriptInstanceID idSequence = ptConductorInstance->arGlobalScripts[i];
        LuaScriptCloseInstance(idSequence);

        ScriptEventListenerDefinitionID idEventListener = ptConductorInstance->arGlobalEventHandlers[i];
        if (idEventListener != -1)
        {

            ScriptEventDeActivateListener(idEventListener);
            ScriptEventRemoveListener(idEventListener);
        }
    }
    POS pos = ptConductorInstance->tRepeatableScriptInstanceDataList.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        RepeatableScriptInstanceData & tRepeatableScriptInstanceData = ptConductorInstance->tRepeatableScriptInstanceDataList.GetNext(pos);
        LuaScriptCloseInstance(tRepeatableScriptInstanceData.idScriptInstance);
        ptConductorInstance->tRepeatableScriptInstanceDataList.RemoveAt(posCurrent);
    }
    ptConductorInstance->tRepeatableScriptInstanceDataList.Dispose();

    LuaScriptFreeStateWithScriptID(ptConductorInstance->idGlobalScript);

    if (ptConductorInstance->eAttachType == SCRIPT_ATTACH_ACTOR)
    {
        LuaScriptReleaseRefACD((ACDID)ptConductorInstance->idAttachment);
    }


    ACDID idACDRef;
    int32 nRefValue;
    POS posRef = ptConductorInstance->tMapRefACDs.GetStartAssoc();
    while (posRef)
    {
        ptConductorInstance->tMapRefACDs.GetNextAssoc(posRef, idACDRef, nRefValue);
        LuaScriptReleaseRefACD(idACDRef);
    }


    ScopedConductorDefinitionLockR pdefConductor(ptConductorInstance->snoConductor);
    SNO snoScript = LuaScriptGetSNO(ptConductorInstance->idGlobalScript);
    if (snoScript != SNO_NONE && pdefConductor->snoScript != snoScript)
    {





        BOOL bFound = FALSE;
        pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
        while (pos)
        {
            ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);
            if (tConductorInstance.idGlobalScript != ptConductorInstance->idGlobalScript &&
                LuaScriptGetSNO(tConductorInstance.idGlobalScript) == snoScript)
            {
                bFound = TRUE;
            }
        }
        if (!bFound)
        {
            ScriptDefinitionDisposeScript(snoScript);
        }
    }
}







void SConductorStopAllConductors(SNO snoConductor)
{

    if (!GameCommonDataExists())
        return;




    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;


    if (ptConductorGlobals == NULL)
        return;






    POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
    while (pos)
    {
        POS posCurrent = pos;
        ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);

        if (snoConductor == SNO_NONE || tConductorInstance.snoConductor == snoConductor)
        {
            sSConductorStopConductor(&tConductorInstance);
            ptConductorGlobals->tConductorInstanceList.RemoveAt(posCurrent);
        }
    }
}



void SConductor_ChangeActs()
{

    if (GameCommonDataExists())
    {




        SConductorStopAllConductors(SNO_NONE);

        SGame & tSGame = SGameManager::GameGet();
        ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;

        ptConductorGlobals->tConductorCompletedArray.EmptyArray();
    }
}

ConductorInstance * SConductorFindConductorByScriptID(ScriptID idScript)
{

    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;


    POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
    while (pos)
    {
        ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);

        if (tConductorInstance.idGlobalScript == idScript)
            return &tConductorInstance;
    }





    return NULL;
}

static BOOL SConductorIsScriptBeingUsed(SNO snoScript)
{

    SGame & tSGame = SGameManager::GameGet();
    ConductorGlobals *& ptConductorGlobals = tSGame.ptConductorGlobals;


    POS pos = ptConductorGlobals->tConductorInstanceList.GetHeadPosition();
    while (pos)
    {
        ConductorInstance & tConductorInstance = ptConductorGlobals->tConductorInstanceList.GetNext(pos);


        if (LuaScriptGetSNO(tConductorInstance.idGlobalScript) == snoScript)
            return TRUE;
    }
    return FALSE;
}





void SConductorInstanceAddRefACD(ConductorInstance * ptConductorInstance, ACDID idACD)
{

    int32 nLookupValue;
    if (ptConductorInstance->tMapRefACDs.Lookup(idACD, nLookupValue))
        return;
    if (ptConductorInstance->tMapRefACDs.Count() == CONDUCTOR_MAX_REFERENCED_ACDID)
    {
        if (ptConductorInstance->bFailedToFindAndPurgeOldRefACDs)
            return;

        ACDID idACDRef;
        POS posRef = ptConductorInstance->tMapRefACDs.GetStartAssoc();
        while (posRef)
        {
            int32 nRefValue;
            ptConductorInstance->tMapRefACDs.GetNextAssoc(posRef, idACDRef, nRefValue);
            if (ACDTryToGet(idACDRef) == NULL)
            {
                ptConductorInstance->tMapRefACDs.RemoveAt(idACDRef);
            }
        }

        if (ptConductorInstance->tMapRefACDs.Count() == CONDUCTOR_MAX_REFERENCED_ACDID)
        {
            ptConductorInstance->bFailedToFindAndPurgeOldRefACDs = TRUE;
            return;
        }

        if (ptConductorInstance->bFailedToFindAndPurgeOldRefACDs)
            return;
    }




    LuaScriptAddRefACD(idACD);
    ptConductorInstance->tMapRefACDs.SetAt_Tracked(idACD, 1);
}




void SConductorInstanceRemoveRefACD(ConductorInstance * ptConductorInstance, ACDID idACD)
{

    int32 nLookupValue;
    if (ptConductorInstance->tMapRefACDs.Lookup(idACD, nLookupValue))
    {
        LuaScriptReleaseRefACD(idACD);
        ptConductorInstance->tMapRefACDs.RemoveAt(idACD);
    }
}
