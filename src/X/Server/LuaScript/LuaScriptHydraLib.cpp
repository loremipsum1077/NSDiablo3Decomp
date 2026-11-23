#include "LuaScript.h"
#include "LuaScriptUtil.h"
#include "../../Sigma/StringManip.h"
#include "../../Common/TagMaps/MarkerTags.h"
#include "../../../Contrib/Contrib/lua/src/lua.h"
#include "../../../Contrib/Contrib/lua/src/lauxlib.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../Definitions/ScriptDefinition.h"

#include "../../Common/Game.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/GameConstants.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../Powers/Powers.h"
#include "ScriptEvent.h"
#include "ScriptEventParams.h"
#include "LuaDebugger.h"
#include "../../Common/Definitions/MarkerSetDefinition.h"
#include "../../Common/Hyperlink.h"
#include "../Actors/Actors.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Common/TagMaps/TagNumbers.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/Marker/Marker.h"
#include "../ConversationMgr.h"
#include "../../Common/Monsters.h"
#include "../AI/ActorAI.h"
extern "C" int Hydra_Init(lua_State * L);














static char gsTraceBuffer[4096];





static int32 LuaTraceScreen(lua_State * luaState)

{
    gsTraceBuffer[0] = 0;
    int32 n = lua_gettop(luaState);

    for (int32 i = 1; i <= n; i++)
    {
        const char * s = LuaScriptStackItemToString(luaState, i);
        if (i > 1)
        {
            strcat_safe(gsTraceBuffer, sizeof(gsTraceBuffer), " ");
        }
        strcat_safe(gsTraceBuffer, sizeof(gsTraceBuffer), s);
    }
    gsTraceBuffer[sizeof(gsTraceBuffer) - 1] = 0;

    return 0;
}






static int32 LuaTrace(lua_State * luaState)

{
    gsTraceBuffer[0] = 0;
    int32 n = lua_gettop(luaState);

    for (int32 i = 1; i <= n; i++)
    {
        const char * s = LuaScriptStackItemToString(luaState, i);
        if (i > 1)
        {
            strcat_safe(gsTraceBuffer, sizeof(gsTraceBuffer), " ");
        }
        strcat_safe(gsTraceBuffer, sizeof(gsTraceBuffer), s);
    }
    gsTraceBuffer[sizeof(gsTraceBuffer) - 1] = 0;

    return 0;
}



static int32 LuaDebugBreak(lua_State * luaState)

{
    CRefString sMessage("User Stop");
    luaL_checktype(luaState, -1, LUA_TSTRING);
    if (lua_isstring(luaState, -1))
    {
        sMessage = lua_tostring(luaState, -1);
    }
    sMessage.Format(" %s", sMessage.str());

    gpLuaDebugger->DebugBreak(luaState, LuaDebugger::DEBUG_BREAK_USER_STOP, CRefString(sMessage.str()));


    return 0;
}

// DWARF: bAssert starts as constant 1 (TRUE) and i has const_value 1, so the loop runs i = 1..nArgs. Columns: 125:13 `!`, 129:27, 133:33 `(BOOL)` cast, 141:14 `!bAssert`; nil else 136-139 inferred.
static int32 LuaAssert(lua_State * luaState)

{
    BOOL bAssert = TRUE;
    int32 nArgs = lua_gettop(luaState);

    for (int32 i = 1; i <= nArgs; i++)
    {
        if (!lua_isnil(luaState, i))
        {
            if (lua_isboolean(luaState, i))
            {
                bAssert = lua_toboolean(luaState, i);
            }
            else
            {
                bAssert = (BOOL)lua_tonumber(luaState, i);
            }
        }
        else
        {
            bAssert = FALSE;
        }

        if (!bAssert)
        {






            gpLuaDebugger->DebugBreak(luaState, LuaDebugger::DEBUG_BREAK_USER_STOP, CRefString("Assertion Failed!"));
            break;
        }
    }
    return 0;
}


static void sLuaErrorInternal(lua_State * L, LPCSTR szErrorMsg)
{



    LuaScriptReportError(L, LUA_ERRERR, TRUE);
    LuaDebugBreakError(L, szErrorMsg);
}


static BOOL sValidateScriptCanYield(lua_State * luaState, LPCSTR szFunction)


{
    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();
    int32 nRet = LuaScriptGetInstanceNumRet(idScriptInstance);
    if (nRet > 0)
    {
        CRefString sError;
        sError.Format("Error in function '%s': script is attempting to yield in a method that has return values (this should be prevented by offline checking in Axe)!\n", szFunction); lua_pushstring(luaState, sError.str()); return FALSE;
    }


    DataID idAttachment;
    ScriptAttachmentType eAttachmentType;

    LuaScriptInstanceGetAttachmentIDAndType(idScriptInstance, idAttachment, eAttachmentType);
    if (eAttachmentType == SCRIPT_ATTACH_POWER)
    {
        Power * ptPower = PowerTryToGet(idAttachment);
        if (ptPower && (ptPower->dwFlags & (1 << PF_DISALLOW_SCRIPT_WAIT)))
        {
            CRefString sError;
            sError.Format("Error in function '%s': power script is attempting to yield in a buff callback that doesn't support it!\n", szFunction); lua_pushstring(luaState, sError.str()); return FALSE;
        }
    }



    return TRUE;
}





static int32 LuaWait(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    double flTimeSeconds = lua_tonumber(luaState, -1);

    if (flTimeSeconds < 0.0) flTimeSeconds = 0.0;




    uint32 dwDelayTicks = (uint32)(flTimeSeconds * TICKS_PER_SECOND);

    if (dwDelayTicks == 0) dwDelayTicks = 1;




    uint32 dwResumeTick = GameGetActiveGameCommonData().dwServerTime + dwDelayTicks;

    LuaScriptSetInstanceResumeTick(idScriptInstance, dwResumeTick, 0);

    return lua_yield(luaState, 0);
}


static int32 LuaQueueWaitEvent(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();



    ScriptEventFilters tFilters;

    ScriptEventType eEventType = (ScriptEventType)lua_tointeger(luaState, -2);

    ScriptEventParseEventFilters(luaState, eEventType, tFilters);

    ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, eEventType, tFilters);

    BOOL bQueued = LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

    lua_pushboolean(luaState, bQueued);

    return 1;
}


static int32 LuaWaitForEvent(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();



    ScriptEventFilters tFilters;

    ScriptEventType eEventType = (ScriptEventType)lua_tointeger(luaState, -2);

    if (ScriptEventParseEventFilters(luaState, eEventType, tFilters))
    {
        ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, eEventType, tFilters);

        BOOL bQueued = LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

        if (bQueued)
        {
            LuaScriptWaitForAnyEvent(idScriptInstance);
        }
    }







    return lua_yield(luaState, 0);
}


static int32 LuaWaitForAnyEvent(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (lua_gettop(luaState) >= 1 && lua_isnumber(luaState, -1))
    {
        TimeParam tTimeParam((uint32)-1, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
        ScriptEventFilters tFilters;

        tFilters.nNumParams = 1;

        if (tTimeParam.ParseParamFromScript(tFilters.tParam1, luaState))
        {
            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_GAME_TIME, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);
        }
    }

    LuaScriptWaitForAnyEvent(idScriptInstance);

    return lua_yield(luaState, 0);
}


static int32 LuaWaitForAllEvents(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (lua_gettop(luaState) >= 1 && lua_isnumber(luaState, -1))
    {
        TimeParam tTimeParam((uint32)-1, ScriptEventVariantParam::LESS_THAN_THIS_VALUE);
        ScriptEventFilters tFilters;

        tFilters.nNumParams = 1;

        if (tTimeParam.ParseParamFromScript(tFilters.tParam1, luaState))
        {
            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_GAME_TIME, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);
        }
    }

    LuaScriptWaitForAllEvents(idScriptInstance);

    return lua_yield(luaState, 0);
}





static BOOL sCheckParam(lua_State * ptLuaState, int32 nIndex, int32 nType, LPCSTR szFunc)

{
    BOOL bOk = TRUE;
    int32 nStackType = lua_type(ptLuaState, nIndex);
    if (nStackType != nType)
    {

        bOk = FALSE;
    }

    return bOk;
}



int32 LuaWaitForActorKilled(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TNUMBER, __FUNCTION__))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -1);
        ActorCommonData * ptACD = ACDTryToGet(idACD);

        if (ptACD)
        {
            ScriptEventFilters tFilters;

            tFilters.tParam1.SetString(MarkerNameGetString(ptACD->tName));


            tFilters.nNumParams = 2;

            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_ACTOR_KILLED, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

            LuaScriptWaitForAllEvents(idScriptInstance);
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForActorKilled(): invalid ACDID given: 0x%08X\n", idACD));
        }
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForActorKilledByName(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szMarkerName = lua_tolstring(luaState, -1, &nLen);
        ScriptEventFilters tFilters;

        tFilters.tParam1.SetString(szMarkerName);


        tFilters.nNumParams = 2;

        ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_ACTOR_KILLED, tFilters);

        LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

        LuaScriptWaitForAllEvents(idScriptInstance);
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForActorKilledByMonsterName(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szMonsterName = lua_tolstring(luaState, -1, &nLen);
        SNOName tSNOName(ACTOR_SNO, szMonsterName);

        if (MonsterExists(tSNOName))
        {
            ScriptEventFilters tFilters;

            tFilters.tParam1.SetSNO(ACTOR_SNO, tSNOName.snoHandle);


            tFilters.nNumParams = 2;

            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_ACTOR_KILLED, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

            LuaScriptWaitForAllEvents(idScriptInstance);
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForActorKilledByMonsterName(): invalid actor monster name given: %s\n", szMonsterName));
        }
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForConversationFinished(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szConversation = lua_tolstring(luaState, -1, &nLen);
        SNO snoConv = SNOFileLookupGUID(CONVERSATION_SNO, szConversation);

        if (snoConv != SNO_NONE)
        {


            if (ConversationMgr::ConversationIsActive(snoConv))
            {







                ScriptEventFilters tFilters;

                tFilters.tParam3.eParamType = SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION;
                tFilters.tParam3.dwValue = snoConv;


                tFilters.nNumParams = 3;

                ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_CONVERSATION_FINISHED, tFilters);

                LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

                LuaScriptWaitForAllEvents(idScriptInstance);
            }
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForConversationFinished(): invalid conversation sno given: %s\n", szConversation));
        }
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForAnyPlayerInBounds(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TNUMBER, __FUNCTION__))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -1);
        ActorCommonData * ptACD = ACDTryToGet(idACD);

        if (ptACD)
        {
            ScriptEventFilters tFilters;

            tFilters.tParam1.SetString(MarkerNameGetString(ptACD->tName));


            tFilters.nNumParams = 2;

            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_TRIGGER_ON_ACTOR_ENTERED, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

            LuaScriptWaitForAllEvents(idScriptInstance);
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForAnyPlayerInBounds(): invalid ACDID given: 0x%08X\n", idACD));
        }
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForAnyPlayerInBoundsByName(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szMarkerName = lua_tolstring(luaState, -1, &nLen);
        ScriptEventFilters tFilters;

        tFilters.tParam1.SetString(szMarkerName);


        tFilters.nNumParams = 2;

        ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_TRIGGER_ON_ACTOR_ENTERED, tFilters);

        LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

        LuaScriptWaitForAllEvents(idScriptInstance);
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForGizmoOperated(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TNUMBER, __FUNCTION__))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -1);
        ActorCommonData * ptACD = ACDTryToGet(idACD);

        if (ptACD)
        {
            ScriptEventFilters tFilters;

            tFilters.tParam1.SetString(MarkerNameGetString(ptACD->tName));


            tFilters.nNumParams = 2;

            ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_GIZMO_OPERATED, tFilters);

            LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

            LuaScriptWaitForAllEvents(idScriptInstance);
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForGizmoOperated(): invalid ACDID given: 0x%08X\n", idACD));
        }
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForGizmoOperatedByName(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szMarkerName = lua_tolstring(luaState, -1, &nLen);
        ScriptEventFilters tFilters;

        tFilters.tParam1.SetString(szMarkerName);


        tFilters.nNumParams = 2;

        ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_GIZMO_OPERATED, tFilters);

        LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

        LuaScriptWaitForAllEvents(idScriptInstance);
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForUserEvent(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TSTRING, __FUNCTION__))
    {
        size_t nLen;
        LPCSTR szEventName = lua_tolstring(luaState, -1, &nLen);
        ScriptEventFilters tFilters;

        tFilters.tParam1.SetString(szEventName);

        tFilters.nNumParams = 1;

        ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_USER_EVENT, tFilters);

        LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

        LuaScriptWaitForAllEvents(idScriptInstance);
    }

    return lua_yield(luaState, 0);
}


int32 LuaWaitForQEmpty(lua_State * luaState)

{
    if (!sValidateScriptCanYield(luaState, __PRETTY_FUNCTION__)) return lua_error(luaState);

    ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();

    if (sCheckParam(luaState, -1, LUA_TNUMBER, __FUNCTION__))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -1);
        ActorCommonData * ptACD = ACDTryToGet(idACD);

        if (ptACD)
        {

            if (ActorIsUsingAIQueue((ActorID)ptACD->idOwner))
            {

                ScriptEventFilters tFilters;

                tFilters.tParam1.SetString(MarkerNameGetString(ptACD->tName));

                tFilters.nNumParams = 1;

                ScriptEventWaitListenerID idWaitListener = ScriptEventAddGameWaitListener(idScriptInstance, EVENT_AIQUEUE_EMPTY, tFilters);

                LuaScriptAddInstanceWaitListener(idScriptInstance, idWaitListener);

                LuaScriptWaitForAllEvents(idScriptInstance);
            }
        }






        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("LuaWaitForQEmpty(): invalid actor id (0x%08X) given\n", idACD));
        }
    }

    return lua_yield(luaState, 0);
}

void sLuaPushTagVariant(lua_State * luaState, const CTagMap * ptTagMap, uint32 dwTag)

{


    if (TagMapTagExists(ptTagMap, dwTag))
    {
        TagVariantType eType = TagGetVariantType(ptTagMap, dwTag);

        switch (eType)
        {
        case TAGVARIANTTYPE_INT:
        case TAGVARIANTTYPE_ACTOR_GROUP:
        case TAGVARIANTTYPE_ACTOR_ATTRIBUTE:
            lua_pushinteger(luaState, TagMapGetInt(ptTagMap, dwTag)); break;

        case TAGVARIANTTYPE_FLOAT:
            lua_pushnumber(luaState, TagMapGetFloat(ptTagMap, dwTag)); break;

        case TAGVARIANTTYPE_SNO:
        {
            SNO sno = TagMapGetSNO(ptTagMap, dwTag);
            const AxeTagInfo * ptAxeTagInfo = TagMapGetTagInfo(ptTagMap, dwTag);
            SNOGroup snoGroup = AxeTagTypeToSNOGroup(ptAxeTagInfo->eType);
            lua_pushstring(luaState, SNOToString(snoGroup, sno, FALSE).str()); break;
        }

        case TAGVARIANTTYPE_GBID:
            lua_pushinteger(luaState, TagMapGetGBID(ptTagMap, dwTag)); break;

        case TAGVARIANTTYPE_FORMULA:
        {
            sLuaErrorInternal(luaState, CRefString().Format("Attempted to retrieve tag of type TAGVARIANTTYPE_FORMULA.  This is not supported."));
            lua_pushnil(luaState); break;
        }
        default:
            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }
    else
    {

        lua_pushnil(luaState);
    }
}

int32 sPushMarkerTagValue(lua_State * luaState, uint32 dwTag)

{

    if (sCheckParam(luaState, -1, LUA_TNUMBER, __FUNCTION__))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -1);
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
        {
            Actor * pActor = ActorGet((ActorID)ptACD->idOwner);

            SNO snoMarkerSet = pActor->tMarkerOwner.snoMarkerSet;
            if (snoMarkerSet != SNO_NONE)
            {

                int16 nMarkerIndex = pActor->tMarkerOwner.nMarkerIndex;
                ScopedMarkerSetDefinitionLockR ptMarkerSetDef(snoMarkerSet);

                const Marker & tMarker = ptMarkerSetDef->tMarkerSet[nMarkerIndex];
                sLuaPushTagVariant(luaState, tMarker.hTagMap, dwTag);
                return 1;
            }
            else
            {
                sLuaErrorInternal(luaState, CRefString().Format("Tried to get marker tag from ACDID: 0x%08X , SNO: %s but owner market set was SNO_NONE.\n", idACD, HyperlinkCreateFromSNO(ptACD->tSNOName, NULL).str()));
            }
        }
        else
        {
            sLuaErrorInternal(luaState, CRefString().Format("Tried to get marker tag from invalid ACDID: 0x%08X\n", idACD));
        }
    }
    lua_pushnil(luaState);

    return 1;
}

int32 LuaGetMarkerTagGenericScriptConversation(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_CONVERSATION_SNO); return 1;
}


int32 LuaGetMarkerTagGenericScriptConversationList(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_CONVERSATION_LIST_SNO); return 1;
}


int32 LuaGetMarkerTagGenericScriptConductor(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_CONDUCTOR_SNO); return 1;
}


int32 LuaGetMarkerTagGenericScriptMusic(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_MUSIC_SNO); return 1;
}


int32 LuaGetMarkerTagGenericScriptPower(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_POWER_SNO); return 1;
}


int32 LuaGetMarkerTagGenericScriptActor(lua_State * luaState)

{
    sPushMarkerTagValue(luaState, TAG_MARKER_GENERIC_SCRIPT_ACTOR_SNO); return 1;
}


int32 LuaGetActorVariable(lua_State * luaState)
{
    if (sCheckParam(luaState, -2, LUA_TNUMBER, "LuaGetActorVariable") &&
        sCheckParam(luaState, -1, LUA_TSTRING, "LuaGetActorVariable"))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -2);
        const char * sVarName = lua_tostring(luaState, -1);
        LuaScriptGetACDVariable(idACD, sVarName, luaState); return 1;
    }

    return 0;

}

int32 LuaSetActorVariable(lua_State * luaState)
{
    if (sCheckParam(luaState, -3, LUA_TNUMBER, "LuaSetActorVariable") &&
        sCheckParam(luaState, -2, LUA_TSTRING, "LuaSetActorVariable"))
    {
        ACDID idACD = (ACDID)lua_tointeger(luaState, -3);
        const char * sVarName = lua_tostring(luaState, -2);
        lua_pushvalue(luaState, -1);
        LuaScriptSetACDVariable(idACD, sVarName, luaState);
    }

    return 0;
}








static const luaL_Reg gLuaHydraReg[] =
{
    { "TraceScreen", LuaTraceScreen },
    { "Trace", LuaTrace },
    { "DebugBreak", LuaDebugBreak },
    { "Assert", LuaAssert },
    { "Wait", LuaWait },
    { "QueueWaitEvent", LuaQueueWaitEvent },
    { "WaitForEvent", LuaWaitForEvent },
    { "WaitForAnyEvent", LuaWaitForAnyEvent },
    { "WaitForAllEvents", LuaWaitForAllEvents },
    { "WaitForActorKilled", LuaWaitForActorKilled },
    { "WaitForActorKilledByName", LuaWaitForActorKilledByName },
    { "WaitForActorKilledByMonsterName", LuaWaitForActorKilledByMonsterName },
    { "WaitForConversationFinished", LuaWaitForConversationFinished },
    { "WaitForAnyPlayerInBounds", LuaWaitForAnyPlayerInBounds },
    { "WaitForAnyPlayerInBoundsByName", LuaWaitForAnyPlayerInBoundsByName },
    { "WaitForGizmoOperated", LuaWaitForGizmoOperated },
    { "WaitForGizmoOperatedByName", LuaWaitForGizmoOperatedByName },
    { "WaitForUserEvent", LuaWaitForUserEvent },
    { "WaitForQEmpty", LuaWaitForQEmpty },
    { "GetMarkerTagGenericScriptConversation", LuaGetMarkerTagGenericScriptConversation },
    { "GetMarkerTagGenericScriptConversationList", LuaGetMarkerTagGenericScriptConversationList },
    { "GetMarkerTagGenericScriptConductor", LuaGetMarkerTagGenericScriptConductor },
    { "GetMarkerTagGenericScriptMusic", LuaGetMarkerTagGenericScriptMusic },
    { "GetMarkerTagGenericScriptPower", LuaGetMarkerTagGenericScriptPower },
    { "GetMarkerTagGenericScriptActor", LuaGetMarkerTagGenericScriptActor },
    { "GetActorVariable", LuaGetActorVariable },
    { "SetActorVariable", LuaSetActorVariable },
    { NULL, NULL }
};



















































































































































































































































































































































































void RegisterHydraLib(lua_State * pLuaState)

{

    LuaStackChecker tStackCheck(pLuaState);



    luaL_register(pLuaState, "HydraUtil", gLuaHydraReg);
    lua_pop(pLuaState, 1);


    Hydra_Init(pLuaState);


    lua_getfield(pLuaState, LUA_GLOBALSINDEX, "Hydra");

    const luaL_Reg * ptLuaReg = gLuaHydraReg;

    while (ptLuaReg->func)
    {
        lua_pushstring(pLuaState, ptLuaReg->name);
        lua_pushcfunction(pLuaState, ptLuaReg->func);
        lua_rawset(pLuaState, -3);
        ptLuaReg++;
    }

    lua_pop(pLuaState, 1);

    SNOName snoName(SCRIPT_SNO, GlobalSNOGet(GLOBALSNO_SCRIPT_HYDRAINIT));

    ScopedScriptDefinitionLockR ptScriptDef(snoName.snoHandle);
    int32 errCode = CustomLuaLoader(ptScriptDef, pLuaState);
    if (errCode == 0)
    {
        errCode = lua_pcall(pLuaState, 0, 0, 0);
    }
    if (errCode != 0)
    {
        lua_pop(pLuaState, 1);
    }






}






















































































































































































































































































































































































































































































































































































































































































