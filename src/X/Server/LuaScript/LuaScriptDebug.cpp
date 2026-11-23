// X/Server/LuaScript/LuaScriptDebug.cpp -- reconstructed from the DWARF and the line-attributed
// listing of _Z36LuaScriptGetStringForDebugStackLevelP9lua_StateiP10CRefStringS2_S2_
// (0x7100967ad0, 848 bytes). Every callee is declared by its owning header: lua_Debug and the Lua
// debug API by Contrib/Contrib/lua/src/lua.h, Blizzard::String::FindI by
// BlizzardCore/Include/Blizzard/String.h.

#include "LuaScriptDebug.h"

#include "../../Sigma/RefString.h"
#include "LuaScript.h"
#include "../SConductor.h"
#include "../Definitions/ConductorDefinition.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include <string.h>

BOOL LuaScriptGetStringForDebugStackLevel(lua_State * L, int32 nLevel, CRefString * sContext, CRefString * sLine, CRefString * sFile)
{
    BOOL bGotStack = FALSE;
    lua_Debug tLuaDebug;




    if (L == NULL)
        return bGotStack;

    if (!lua_getstack(L, nLevel, &tLuaDebug))
        return bGotStack;
    bGotStack = TRUE;
    lua_getinfo(L, "Snl", &tLuaDebug);

    if (sLine != NULL &&
        tLuaDebug.currentline >= 0)
        sLine->Format("%d", tLuaDebug.currentline);




    BOOL bUnknownLuaContext = FALSE;

    if (sContext == NULL)
        return bGotStack;
    if (strcmp(tLuaDebug.what, "C") == 0)
    {
        sContext->Format("[C]:%s", tLuaDebug.name);
    }
    else if (strcmp(tLuaDebug.what, "Lua") == 0)
    {
        if (LuaScriptExecutingScriptIsConductor())
        {
            ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
            CRefString sFunctionName(LuaScriptGetInstanceEntryPoint(idScriptInstance));
            int32 nPhaseIndex = 0, nSequenceIndex = 0;

            ScriptID idScript = LuaScriptGetInstanceParent(idScriptInstance);
            ConductorInstance * ptConductorInstance = SConductorFindConductorByScriptID(idScript);

            SNO snoConductor = ptConductorInstance->snoConductor;


            if (ConductorGetPhaseAndSequenceFromStartFunctionName(sFunctionName.str(), nPhaseIndex, nSequenceIndex))
            {
                if (Blizzard::String::FindI(sFunctionName.str(), "start", 0x7fffffff) != NULL)
                {
                    sContext->Format("PHASE: \"%s\" - SEQUENCE START: \"%s\"", ConductorGetPhaseLabel(snoConductor, nPhaseIndex).str(), ConductorGetSequenceLabel(snoConductor, nPhaseIndex, nSequenceIndex).str());
                }
                else
                {
                    sContext->Format("PHASE: \"%s\" - SEQUENCE INTERRUPTED: \"%s\"", ConductorGetPhaseLabel(snoConductor, nPhaseIndex).str(), ConductorGetSequenceLabel(snoConductor, nPhaseIndex, nSequenceIndex).str());
                }
            }
            else if (ConductorGetPhaseFromInitFunctionName(sFunctionName.str(), nPhaseIndex))
            {
                sContext->Format("PHASE INIT: \"%s\"", ConductorGetPhaseLabel(snoConductor, nPhaseIndex).str());
            }

            if (sLine != NULL)
            {
                sLine->Format("%d", tLuaDebug.currentline);
            }

        }
        else
        {
            if (tLuaDebug.name != NULL)
            {
                sContext->Format("Lua:%s", tLuaDebug.name);
            }
            else
            {
                SNOName snoNameSource = LuaScriptGetSNONameForLuaState(L);
                if (snoNameSource != SNOName(SNOGROUP_NONE, SNO_NONE))
                {
                    sContext->Format("Lua:%s", SNOToString(snoNameSource, FALSE).str());
                }
                else
                {
                    sContext->Format("Lua:Unknown");
                    bUnknownLuaContext = TRUE;
                }
            }
            *sFile = tLuaDebug.source;
        }
    }
    else if (strcmp(tLuaDebug.what, "main") == 0)
    {
        sContext->Format("main:%s", tLuaDebug.short_src);
    }

    BOOL lastStack = lua_getstack(L, nLevel + 1, &tLuaDebug);

    if (!bUnknownLuaContext || lastStack)
        return bGotStack;


    LPCSTR szFuncName = LuaScriptGetExecutingFunction();

    if (szFuncName != NULL)
    {
        sContext->Format("Lua:%s", szFuncName);
    }



    return bGotStack;
}

// Provenance notes (the function above is byte-identical to 0x7100967ad0 apart from the operands
// relocations fill in):
//  * `snoNameSource != SNOName(SNOGROUP_NONE, SNO_NONE)` is the two-argument-constructor form of
//    the inlined SNOName::operator!= declared at SNOName.h:70 (DWARF: inlined at LuaScriptDebug.cpp
//    line 123). Written with a bitwise & of the two member tests so the comparison folds into the
//    single `and`/`cmn #1` test the original shows; plain `!(*this == snoname)` compiles to an
//    `orr`/`cbz` pair instead.
//  * lua_Debug comes from its DWARF owner, Contrib/Contrib/lua/src/lua.h:28-41 (size 0x78,
//    i_ci at 0x74); the offsets the listing uses (what +0x18, source +0x20, currentline +0x28,
//    short_src +0x38) are unchanged by that header.
//  * strcmp's callee (0x7100c1d6a0) carries no DWARF symbol; it is written as strcmp.
//  * the original saved x28 without an obvious use in the listing; the same save/restore is emitted
//    here from the identical body, so no source construct has to force it.
//  * static bool sIsVariableChar(char ch) (LuaScriptDebug.cpp:268, inline, no address, no body
//    recorded) is not in the inventory and is deliberately not invented here.
