// X/Server/LuaScript/LuaScriptUtil.cpp -- reconstructed from the 2.6.2 DWARF and instruction listings.
// Functions sit on their original lines (asserts embed __LINE__).

#include "LuaScript.h"
#include "LuaScriptUtil.h"
#include "LuaDebugger.h"
#include "../SGameManager.h"
#include "../SGame.h"
#include "../SWorld.h"
#include "../../Common/Game.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/SceneCommon.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/Marker/Marker.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Stopwatch.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include <stdio.h>
#include <string.h>

// LuaScript.cpp (not declared in src LuaScript.h yet).
ScriptID LuaScriptGetScriptID(SNO snoScript);  // LuaScript.cpp:3153
void LuaScriptPushExecutingFunction(LPCSTR szFuncName);  // LuaScript.cpp:2045
void LuaScriptPopExecutingFunction();  // LuaScript.cpp:2082
void LuaScriptSetInstanceNumArgsAndRet(ScriptInstanceID idScriptInstance, int32 nArgs, int32 nRet);  // LuaScript.cpp:2244
void LuaScriptResetTimeoutTimer();  // LuaScript.cpp:5407
BOOL SNOFileTestSNOElement(const SNOName & snoname);  // X/Common/SNO/SNOFiles.cpp:4789 (not in src SNOFiles.h yet)
// LuaScriptDebug.cpp:44
BOOL LuaScriptGetStringForDebugStackLevel(lua_State * L, int32 nLevel, CRefString * sContext, CRefString * sLine, CRefString * sFile);

struct LuaMemoryStats {  // size 0x14
    int32 max_mem_alloced;  // 0x00
    int32 total_mem_alloced;  // 0x04
    int32 total_allocs;  // 0x08
    int32 total_reallocs;  // 0x0c
    int32 total_frees;  // 0x10
};

struct LuaScriptAllocator {  // size 0x28
    LuaMemoryStats tAllocStats;  // 0x00
    ChainedFixedBlockAllocator * m_arFixedBlockAllocators;  // 0x18
    int32 m_nNumBins;  // 0x20
    BOOL m_bShuttingDown;  // 0x24
    void Initialize();
    void Dispose();
    ChainedFixedBlockAllocator * FindAllocator(size_t nAllocSize);
    void * AllocBlock(size_t nSize);
    void * ReallocBlock(void * pMem, size_t nOldSize, size_t nNewSize);
    void FreeBlock(void * pMem, size_t nSize);
};




















static uint32 gsarBlockSizes[5] =
{
    16,
    24,
    32,
    36,
    64,
};














static uint32 gsarBlockCounts[5] =
{
    64,
    1024,
    512,
    256,
    512,
};
























































































void LuaScriptAllocator::Initialize()
{
    XMemoryAllocatorInterface * ptMemoryInterface;
    int32 nBytesAlloced;

    m_nNumBins = 5;
    m_bShuttingDown = FALSE;
    nBytesAlloced = m_nNumBins * sizeof(ChainedFixedBlockAllocator);
    m_arFixedBlockAllocators = (ChainedFixedBlockAllocator *)SigmaMemoryNew(nBytesAlloced, 4, NULL, FALSE);

    ptMemoryInterface = AppGetCurrentMemoryInterface();


    for (int32 i = 0; i < m_nNumBins; i++)
    {
        int32 nBlockSize = gsarBlockSizes[i];
        int32 nBlockCount = gsarBlockCounts[i];
        m_arFixedBlockAllocators[i].Initialize(nBlockSize, nBlockCount, ptMemoryInterface);

        m_arFixedBlockAllocators[i].SetGrow(XALLOC_GROW_LINEAR, nBlockCount);
        m_arFixedBlockAllocators[i].SetShrinkable(TRUE);
    }
}


















































void LuaScriptAllocator::Dispose()
{





    for (int32 i = 0; i < m_nNumBins; i++)
    {









        m_arFixedBlockAllocators[i].Dispose();
    }






    SigmaMemoryFree(m_arFixedBlockAllocators, NULL);
}




ChainedFixedBlockAllocator * LuaScriptAllocator::FindAllocator(size_t nAllocSize)
{




    for (int32 i = 0; i < m_nNumBins; i++)
    {
        if (nAllocSize <= gsarBlockSizes[i])
        {
            return &m_arFixedBlockAllocators[i];
        }
    }
    return NULL;
}





































void * LuaScriptAllocator::AllocBlock(size_t nSize)
{
    void * pMem;


    ChainedFixedBlockAllocator * pAlloc = FindAllocator(nSize);
    if (pAlloc)
    {
        pMem = pAlloc->Alloc_Tracked();
    }
    else
    {

        pMem = SigmaMemoryNew(nSize, 4, NULL, FALSE);
    }
    return pMem;
}









void * LuaScriptAllocator::ReallocBlock(void * pMem, size_t nOldSize, size_t nNewSize)
{
    size_t nCopySize;







    ChainedFixedBlockAllocator * pAllocOld = FindAllocator(nOldSize);



    nCopySize = (nOldSize <= nNewSize) ? nOldSize : nNewSize;




    if (pAllocOld && pAllocOld->ContainsMem(pMem))
    {
        ChainedFixedBlockAllocator * pAllocNew = FindAllocator(nNewSize);

        if (pAllocNew)
        {
            if (pAllocNew == pAllocOld) return pMem;

            void * pMemNew = AllocBlock(nNewSize);

            SigmaMemoryCopy(pMemNew, pMem, nCopySize);

            pAllocOld->Free(pMem);
            return pMemNew;
        }
        else
        {


            void * pMemNew = SigmaMemoryNew(nNewSize, 4, NULL, FALSE);

            SigmaMemoryCopy(pMemNew, pMem, nCopySize);

            pAllocOld->Free(pMem);
            return pMemNew;
        }
    }
    else
    {





        ChainedFixedBlockAllocator * pAllocNew = FindAllocator(nNewSize);

        if (pAllocNew)
        {
            void * pMemNew = AllocBlock(nNewSize);

            SigmaMemoryCopy(pMemNew, pMem, nCopySize);

            SigmaMemoryFree(pMem, NULL);
            return pMemNew;
        }
        else
        {

            return SigmaMemoryResize(pMem, NULL, nNewSize, 4);
        }
    }
}







void LuaScriptAllocator::FreeBlock(void * pMem, size_t nSize)
{







    ChainedFixedBlockAllocator * pAlloc = FindAllocator(nSize);
    if (pAlloc && pAlloc->ContainsMem(pMem))
    {
        if (!m_bShuttingDown)
        {
            pAlloc->Free(pMem);
        }
    }
    else
    {

        SigmaMemoryFree(pMem, NULL);
    }
}

















LuaScriptAllocator * LuaScriptAllocInitializeForGame()
{
    LuaScriptAllocator * ptLuaAllocator = (LuaScriptAllocator *)SigmaMemoryNew(sizeof(LuaScriptAllocator), 4, NULL, TRUE);

    ptLuaAllocator->Initialize();

    return ptLuaAllocator;
}




void LuaScriptAllocDisposeForGame(LuaScriptAllocator * ptLuaAllocator)
{
    ptLuaAllocator->Dispose();

    delete ptLuaAllocator;
}





void LuaScriptAllocSetToShutdown(LuaScriptAllocator * ptLuaAllocator)
{
    if (ptLuaAllocator)
    {
        ptLuaAllocator->m_bShuttingDown = TRUE;
    }
}




void * LuaScriptAlloc(void * ud, void * ptr, size_t osize, size_t nsize)
{
    void * pret = NULL;
    LuaScriptAllocator * ptLuaScriptAlloc = (LuaScriptAllocator *)ud;




    if (ptLuaScriptAlloc)
    {
        LuaMemoryStats & tMemStats = ptLuaScriptAlloc->tAllocStats;

        tMemStats.total_mem_alloced += (int32)(nsize - osize);

        if (osize == 0)
            tMemStats.total_allocs++;

        if (nsize == 0) tMemStats.total_frees++; if (nsize != 0 && osize != 0) tMemStats.total_reallocs++;





        if (tMemStats.total_mem_alloced > tMemStats.max_mem_alloced)
            tMemStats.max_mem_alloced = tMemStats.total_mem_alloced;
    }

    if (ptLuaScriptAlloc == NULL)
    {


        if (nsize == 0)
        {
            if (ptr)
            {
                SigmaMemoryFree(ptr, NULL);
            }
        }
        else
        {
            if (ptr == NULL)
            {
                pret = SigmaMemoryNew(nsize, 4, NULL, FALSE);
            }
            else
            {
                pret = SigmaMemoryResize(ptr, NULL, nsize, 4);
            }
        }
    }
    else
    {
        if (nsize == 0)
        {
            if (ptr)
            {
                ptLuaScriptAlloc->FreeBlock(ptr, osize);
            }
        }
        else
        {

            if (ptr == NULL)
            {
                pret = ptLuaScriptAlloc->AllocBlock(nsize);
            }
            else
            {

                pret = ptLuaScriptAlloc->ReallocBlock(ptr, osize, nsize);
            }
        }
    }


    return pret;
}


CRefString LuaACDToString(ActorCommonData & tAcd)
{

    CRefString sRet;
    if (tAcd.tName)
    {
        sRet = MarkerNameGetString(tAcd.tName);
    }
    else
    {

        sRet = SNOToString(ACTOR_SNO, tAcd.snoActor, FALSE);
    }
    return sRet;
}













int32 LuaScriptReportError(lua_State * L, int32 status, BOOL printStackTrace)
{
    if (status && !lua_isnil(L, -1))
    {
        CHAR sFileInfoBuf[512];
        CHAR sErrorBuff[512];
        const char * msg = lua_tostring(L, -1);
        int32 nLineBuf;
        LPCSTR errorStr;

        switch (status)
        {
        case LUA_ERRRUN: errorStr = "Lua Runtime Error"; break;
        case LUA_ERRMEM: errorStr = "Lua Memory Error"; break;
        case LUA_ERRERR: errorStr = "Lua Error Handler Error"; break;
        default: errorStr = "Lua Error"; break;
        }




        if (msg == NULL)
            msg = "(error object is not a string)";
        else
        {
            if (sscanf(msg, "%[^]\t\n]]:%d:%[^\t\n]", sFileInfoBuf, &nLineBuf, sErrorBuff) == 3)
                msg = sErrorBuff;
        }



        lua_pop(L, 1);

        if (printStackTrace)
        {
            CRefString sTraceStr;
            sTraceStr.Format(" %s: %s\n", errorStr, msg);


            CRefString sContext; CRefString sLine; CRefString sFile; int32 nLevel = 0;
            BOOL bGotStack = LuaScriptGetStringForDebugStackLevel(L, nLevel, &sContext, &sLine, &sFile);

            while (bGotStack)
            {
                CRefString sStackString;
                if (sLine.Length())
                {
                    sStackString.Format(" [%d] Context: %s Line: %s\n", nLevel, sContext.str(), sLine.str());
                }
                else
                {
                    sStackString.Format(" [%d] Context: %s \n", nLevel, sContext.str());
                }

                sTraceStr += sStackString;
                nLevel++;
                bGotStack = LuaScriptGetStringForDebugStackLevel(L, nLevel, &sContext, &sLine, &sFile);
            }

            sTraceStr += "\n";

        }
    }





    return status;
}

































const char * LuaScriptStackItemToString(lua_State * luaState, int32 i)
{
    static char sOutputBuffer[512];


    if (lua_isnumber(luaState, i) && SGameManager::GetGameCount())
    {
        DataID idData = lua_tointeger(luaState, i);

        ActorCommonData * ptACD = ACDTryToGet((ACDID)idData);
        SceneCommonData * ptSCD;
        if (ptACD)
        {
            CRefString sAcdToString = LuaACDToString(*ptACD);
            Blizzard::String::Copy(sOutputBuffer, sAcdToString.str(), sizeof(sOutputBuffer));

            return sOutputBuffer;
        }
        ptSCD = SceneCommonDataTryToGet(idData);

        if (ptSCD
            &&
            ptSCD->snoScene != SNO_NONE)
        {
            CRefString sSnoScene = SNOToString(SCENE_SNO, ptSCD->snoScene, FALSE);
            Blizzard::String::Copy(sOutputBuffer, sSnoScene.str(), sizeof(sOutputBuffer));

            return sOutputBuffer;
        }


        if (SNOFileTestSNOElement(SNOName(QUEST_SNO, idData)))
        {
            CRefString sSnoQuest = SNOToString(QUEST_SNO, idData, FALSE);
            Blizzard::String::Copy(sOutputBuffer, sSnoQuest.str(), sizeof(sOutputBuffer));

            return sOutputBuffer;
        }


        SWorld * ptSWorld = SWorldTryToGet(idData);
        if (ptSWorld)
        {

            CRefString sSnoWorld = SNOToString(WORLD_SNO, ptSWorld->tWCD.snoWorld, FALSE);
            Blizzard::String::Copy(sOutputBuffer, sSnoWorld.str(), sizeof(sOutputBuffer));

            return sOutputBuffer;
        }
    }




    lua_getglobal(luaState, "tostring");
    lua_pushvalue(luaState, i);
    lua_call(luaState, 1, 1);
    const char * s = lua_tostring(luaState, -1);

    if (s == NULL)
    {
        sprintf(sOutputBuffer, "Type: '%s' - no tostring function", lua_typename(luaState, lua_type(luaState, i)));
    }
    else
    {
        strncpy(sOutputBuffer, s, sizeof(sOutputBuffer) - 1);

        sOutputBuffer[sizeof(sOutputBuffer) - 1] = 0;
    }

    lua_pop(luaState, 1);


    return sOutputBuffer;
}






















































































































int32 LuaErrorHandler(lua_State * L)
{
    LPCSTR szErrorMsg = NULL;

    if (lua_isstring(L, -1))
    {
        szErrorMsg = lua_tostring(L, -1);
    }


    LuaScriptReportError(L, LUA_ERRERR, TRUE);
    LuaDebugBreakError(L, szErrorMsg);
    return 1;
}

void LuaDebugBreakError(lua_State * L, LPCSTR szErrorMsg)
{


    if (gpLuaDebugger)
    {
        gpLuaDebugger->DebugBreak(L, LuaDebugger::DEBUG_BREAK_SCRIPT_ERROR, CRefString(szErrorMsg));
    }
}





int32 LuaCall(ScriptInstanceID idScriptInstance, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName)
{







    Stopwatch tStopwatch;
    float flTime;


    int32 base = lua_gettop(L) - nArgumentsCnt;

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    lua_pushcfunction(L, LuaErrorHandler);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    lua_insert(L, base);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptPushExecutingScript(idScriptInstance);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptPushExecutingFunction(szFuncName);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptSetInstanceNumArgsAndRet(idScriptInstance, nArgumentsCnt, nReturnValCnt);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    int32 status = lua_pcall(L, nArgumentsCnt, nReturnValCnt, base);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    lua_remove(L, base);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    if (status != 0)
    {
        lua_gc(L, LUA_GCCOLLECT, 0);

        LuaScriptReportError(L, status, FALSE);
    }

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptPopExecutingScript();

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }


    LuaScriptPopExecutingFunction();


    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    return status;
}

int32 LuaSNOScriptCall(SNO snoScript, lua_State * L, int32 nArgumentsCnt, int32 nReturnValCnt, LPCSTR szFuncName)
{





    Stopwatch tStopwatch;
    float flTime;
    ScriptInstanceID idScriptInstance = LuaScriptNewInstanceWithScriptID(LuaScriptGetScriptID(snoScript), szFuncName, NULL, -2, -1, (ScriptAttachmentType)0);



    LuaScriptResetTimeoutTimer();

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    int32 nRet = LuaCall(idScriptInstance, L, nArgumentsCnt, nReturnValCnt, szFuncName);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    LuaScriptCloseInstance(idScriptInstance);

    if (SGameGlobalsGet().dwFlags & 0x100)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }
    return nRet;
}





BOOL LuaCheckReturnValue(SNO snoScript, lua_State * L, int32 nExpectedLuaType, LPCSTR szFuncName)
{






    int32 nLuaType = lua_type(L, -1);
    BOOL bValid = nLuaType == nExpectedLuaType;

    if (!bValid)
    {
        LPCSTR szReturnType = lua_typename(L, nLuaType);
        LPCSTR szExpectedType = lua_typename(L, nExpectedLuaType);
    }




    return bValid;
}



int32 LuaScriptGetTableSize(lua_State * L, int32 nTableIndex)
{
    int32 nTableSize = -1;
    LuaStackChecker tStackCheck(L);



    if (nTableIndex < 0)
    {
        nTableIndex = lua_gettop(L) + nTableIndex + 1;
    }

    if (lua_istable(L, nTableIndex))
    {
        nTableSize = 0;

        lua_pushnil(L);
        while (lua_next(L, nTableIndex))
        {
            nTableSize++;
            lua_pop(L, 1);
        }
    }

    return nTableSize;
}





BOOL LuaScriptGetFunction(lua_State * L, LPCSTR szFuncName)
{
    BOOL bGotFunction = FALSE;



    if (L && szFuncName)
    {






        lua_getglobal(L, szFuncName);

        if (lua_isfunction(L, -1))
        {
            bGotFunction = TRUE;
        }
        else
        {
            lua_pop(L, 1);
        }
    }

    return bGotFunction;
}



BOOL LuaScriptHasFunction(lua_State * L, LPCSTR szFuncName)
{
    BOOL bHasFunction = FALSE;



    if (L)
    {

        lua_getglobal(L, szFuncName);

        bHasFunction = lua_isfunction(L, -1);




        lua_pop(L, 1);
    }

    return bHasFunction;
}

void LuaCopyItemToState(lua_State * lSrc, lua_State * lDst, int32 nSrcIndex)
{
    int eType = lua_type(lSrc, nSrcIndex);
    switch (eType)
    {
    case LUA_TNIL:
        {
            lua_pushnil(lDst);
        } break;
    case LUA_TNUMBER:
        {

            double d = lua_tonumber(lSrc, nSrcIndex);
            lua_pushnumber(lDst, d);
        } break;
    case LUA_TBOOLEAN:
        {

            BOOL b = lua_toboolean(lSrc, nSrcIndex);
            lua_pushboolean(lDst, b);
        } break;
    case LUA_TSTRING:
        {

            const char * s = lua_tostring(lSrc, nSrcIndex);
            lua_pushstring(lDst, s);
        } break;
    case LUA_TLIGHTUSERDATA:
        {

            void * pUserData = lua_touserdata(lSrc, nSrcIndex);
            lua_pushlightuserdata(lDst, pUserData);
        } break;
    case LUA_TTABLE:
        {

            int32 nSrcAbsoluteIndex = nSrcIndex;
            if (nSrcIndex < 0)
            {
                nSrcAbsoluteIndex = lua_gettop(lSrc) + nSrcIndex + 1;
            }





            size_t nTableSize = lua_objlen(lSrc, nSrcAbsoluteIndex);
            int eTableType = lua_type(lSrc, nSrcAbsoluteIndex);


            lua_createtable(lDst, nTableSize, 0);

            lua_pushnil(lSrc);
            while (lua_next(lSrc, nSrcAbsoluteIndex))
            {
                LuaCopyItemToState(lSrc, lDst, -2);
                LuaCopyItemToState(lSrc, lDst, -1);
                lua_settable(lDst, -3);
                lua_pop(lSrc, 1);
            }
        } break;
    case LUA_TFUNCTION:
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        } break;
    case LUA_TUSERDATA:
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        } break;
    case LUA_TTHREAD:
        {

            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        } break;
    }
}


void LuaMove(lua_State * lSrc, lua_State * lDst, int32 nItems)
{



    for (int32 i = -nItems; i < 0; i++)
    {
        LuaCopyItemToState(lSrc, lDst, i);
    }

    lua_pop(lSrc, nItems);
}
