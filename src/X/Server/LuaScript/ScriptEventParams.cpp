// X/Server/LuaScript/ScriptEventParams.cpp
// Line layout follows the original: the asserts in SetSNO (header), ACDIDParam::ParseParamFromScript,
// ACDIDListParam::ToGameParam and ACDIDListParam::ParseParamFromScript bake __LINE__.
#include "Contrib/Contrib/blz/include/blz/tuple.h"
#include "X/Server/LuaScript/ScriptEventParams.h"
#include "Contrib/Contrib/lua/src/lua.h"
#include "Contrib/Contrib/lua/src/lauxlib.h"
#include "X/Server/LuaScript/LuaScriptUtil.h"
#include "X/Server/Actors/Actors.h"
#include "X/Server/SScene.h"
#include "X/Server/System/Adventure.h"
#include "X/Server/System/LevelArea.h"
#include "X/Common/Attributes/FastAttribConstantsSwig.h"
#include "X/Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "X/Common/Definitions/GlobalDefinition.h"
#include "X/Common/Definitions/MarkerSetDefinition.h"
#include "X/Common/Game.h"
#include "X/Common/GameCommonData.h"
#include "X/Common/Marker/Marker.h"
#include "X/Common/Player.h"
#include "X/Common/TagMaps/AnimTags.h"
#include "X/Common/TeamCommon.h"
#include "X/Sigma/Math/MathGeneral.h"
#include "X/Sigma/MemoryPools.h"
#include "X/Sigma/RefString.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptor.h"
#include "X/Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <string.h>
#include <strings.h>

extern const SymbolicEntry gScriptEventVariantParamTypes[32];  // X/Common/SNO/SymbolicValue.cpp:4005

// X/Server/LuaScript/ScriptEventInternal.h:139 (that header is not adopted in src/ yet)
enum {
    TICKS_PER_HEARTBEAT = 15,
};

#include "X/Common/Definitions/GameBalanceDefinition.h"


















































static const TypeDescriptorField ScriptEventVariantParamLW_Fields[31] = {
    EnumTypeDescriptorField("", offsetof(ScriptEventVariantParamLW, eParamType), NULL, gScriptEventVariantParamTypes),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_DWORD),
    VariantTypeDescriptorField("", DT_FLOAT, offsetof(ScriptEventVariantParamLW, flValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_FLOAT),
    VariantTypeDescriptorField("", DT_FLOAT, offsetof(ScriptEventVariantParamLW, flMinValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED),
    VariantTypeDescriptorField("", DT_FLOAT, offsetof(ScriptEventVariantParamLW, flMaxValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED),
    VariantTypeDescriptorField("", DT_FLOAT, offsetof(ScriptEventVariantParamLW, flMinValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE),
    VariantTypeDescriptorField("", DT_FLOAT, offsetof(ScriptEventVariantParamLW, flMaxValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", ACTOR_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", MONSTER_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", LEVELAREA_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", SCENE_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", CONVERSATION_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", TIMEDEVENT_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", WORLD_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", POWER_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", BOSS_ENCOUNTER_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ScriptEventVariantParamLW, snoValue), "", QUESTRANGE_SNO, 0, SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwHash), NULL, SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_PLAYER),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY),
    VariantTypeDescriptorField("", DT_ANIM_TAG, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwHash), NULL, SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME),
    VariantTypeDescriptorField("", DT_TIME, offsetof(ScriptEventVariantParamLW, dwTimeValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL),
    VariantTypeDescriptorField("", DT_TIME, offsetof(ScriptEventVariantParamLW, dwTimeValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME),
    VariantTypeDescriptorField("", DT_ACTOR_GROUP, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP),
    VariantTypeDescriptorField("", DT_ACTOR_GROUP, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP),
    FilterTypeDescriptorField("", DT_GBID, offsetof(ScriptEventVariantParamLW, gbidValue), "", 2, 0, SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwHash), NULL, SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwHash), NULL, SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME),
    VariantTypeDescriptorField("", DT_INT, offsetof(ScriptEventVariantParamLW, dwValue), NULL, SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ScriptEventVariantParamLW), NULL, 0) }; VariantTypeDescriptor TypeDescriptor_ScriptEventVariantParamLW("ScriptEventVariantParamLW", 0, ScriptEventVariantParamLW_Fields, 31, NULL, 0x80, NULL, NULL, NULL, offsetof(ScriptEventVariantParamLW, eParamType)); const XTypeDescriptor * DT_SCRIPT_EVENT_VARIANT_PARAM_LW = &TypeDescriptor_ScriptEventVariantParamLW;

// 0x710086c340  BOOL ScriptEventVariantParam::FilterAgainst(const ScriptEventVariantParam & rhs, DataID idSelf, ScriptID idScript, ScriptInstanceID idScriptInstance) const




























































BOOL ScriptEventVariantParam::FilterAgainst(const ScriptEventVariantParam & rhs, DataID idSelf, ScriptID idScript, ScriptInstanceID idScriptInstance) const
{
    BOOL bPassesFilter = FALSE;

    if (eParamType == SCRIPT_EVENT_VARIANT_PARAM_NONE)
    {
        return TRUE;
    }

    if (rhs.eParamType == SCRIPT_EVENT_VARIANT_PARAM_SELF)
    {
        if (idSelf == DATAID_INVALID)
        {
            return rhs.idValue == idValue;
        }
        return rhs.idValue == idSelf;
    }

    switch (eParamType)
    {
    case SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL:
        if (dwValue == 0)
        {
            return TRUE;
        }
        {
            uint32 dwTime = rhs.dwValue;
            uint32 dwLastInterval = dwTime - TICKS_PER_HEARTBEAT;
            uint32 dwIntervalNum = dwTime / dwValue;
            uint32 dwLastIntervalNum = dwLastInterval / dwValue;
            return dwIntervalNum != dwLastIntervalNum;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME:
        {
            uint32 dwSpawnerNameHash = rhs.dwValue;
            uint32 uHashToUse = GetAdventureScopedStringHash(idScriptInstance);
            return uHashToUse == dwSpawnerNameHash;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_SELF:
        {
            ActorCommonData * ptACD = ACDTryToGet((ACDID)idSelf);
            if (ptACD == NULL)
            {
                return FALSE;
            }
            {
                uint32 uHashToUse = ptACD->tName;
                uint32 dwSpawnerNameHash = rhs.dwValue;
                return uHashToUse == dwSpawnerNameHash;
            }
        }
    default:
        break;
    }

    if (eParamType != rhs.eParamType)
    {
        return FALSE;
    }
    switch (eParamType)
    {
    case SCRIPT_EVENT_VARIANT_PARAM_FLOAT:
        return Math::Abs(flValue - rhs.flValue) < EPSILON;
    case SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED:
        {
            BOOL bMaxInRange = (rhs.flMinValue >= flMinValue) && (rhs.flMinValue <= flMaxValue);
            BOOL bMinInRange = (rhs.flMaxValue >= flMinValue) && (rhs.flMaxValue <= flMaxValue);
            return bMaxInRange != bMinInRange;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE:
        return rhs.flValue <= flValue;
    case SCRIPT_EVENT_VARIANT_PARAM_DWORD:
    case SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_PLAYER:
    case SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY:
    case SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION:
    case SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG:
    case SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME:
    case SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP:
    case SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP:
    case SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID:
    case SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO:
    case SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE:
    case SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO:
        return dwValue == rhs.dwValue;
    case SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE:
        {
            BOOL bStartInRange = QuestRangeEvaluate(snoValue, rhs.tQuestStart.snoQuest, rhs.tQuestStart.nStepUID, NULL);
            BOOL bEndInRange = QuestRangeEvaluate(snoValue, rhs.tQuestEnd.snoQuest, rhs.tQuestEnd.nStepUID, NULL);
            return bStartInRange != bEndInRange;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH:
        {
            uint32 uHashToUse = GetAdventureScopedStringHash(idScriptInstance);
            return uHashToUse == rhs.dwHash;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME:
        if (rhs.eFilterInequality == LESS_THAN_THIS_VALUE)
        {
            return dwTimeValue <= rhs.dwTimeValue;
        }
        else
        {
            return dwTimeValue >= rhs.dwTimeValue;
        }
    case SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY:
    case SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP:
        {
            ACDID idACD = (ACDID)rhs.idValue;
            ACDID idACDSelf = (ACDID)idSelf;
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (idSelf == DATAID_INVALID)
            {
                idACDSelf = (ACDID)idValue;
            }
            ActorCommonData * ptACDSelf = ACDTryToGet(idACDSelf);
            if (ptACD == NULL || ptACDSelf == NULL)
            {
                return FALSE;
            }
            {
                ActorID idActor = (ActorID)ACDGetOwner(idACD);
                Actor * ptActor = ActorGet(idActor);
                ActorID idActorSelf = (ActorID)ACDGetOwner(idACDSelf);
                Actor * ptActorSelf = ActorGet(idActorSelf);

                if (ptActorSelf->tMarkerOwner.snoMarkerSet != SNO_NONE &&
                    ptActorSelf->tMarkerOwner.snoMarkerSet == ptActor->tMarkerOwner.snoMarkerSet &&
                    ptActorSelf->tMarkerOwner.nAdventureParentIndex == ptActor->tMarkerOwner.nAdventureParentIndex &&
                    ptActorSelf->tMarkerOwner.nMarkerSetInstanceIndex == ptActor->tMarkerOwner.nMarkerSetInstanceIndex)
                {
                    ScopedMarkerSetDefinitionLockR pdefMarkerSet(ptActorSelf->tMarkerOwner.snoMarkerSet);
                    const Marker & tMarkerSelf = pdefMarkerSet->tMarkerSet[ptActorSelf->tMarkerOwner.nMarkerIndex];
                    const Marker & tMarker = pdefMarkerSet->tMarkerSet[ptActor->tMarkerOwner.nMarkerIndex];

                    if (tMarkerSelf.nLinkCount > 0)
                    {
                        uint32 tParamName = dwHash;
                        for (int32 i = 0; i < tMarkerSelf.nLinkCount; i++)
                        {
                            MarkerLink & tLink = tMarkerSelf.ptMarkerLinks[i];
                            if (tLink.dwHash == tMarker.dwHash)
                            {
                                if (eParamType == SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY)
                                {
                                    bPassesFilter = TRUE;
                                    break;
                                }
                                if (tParamName == tLink.dwGroupHash)
                                {
                                    bPassesFilter = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                    return bPassesFilter;
                }
            }
            return FALSE;
        }
    default:
        break;
    }

    return bPassesFilter;
}

// 0x710086c8e0  uint32 ScriptEventVariantParam::GetAdventureScopedStringHash(ScriptInstanceID idScriptInstance) const












































uint32 ScriptEventVariantParam::GetAdventureScopedStringHash(ScriptInstanceID idScriptInstance) const
{
    uint32 uHashToUse = dwHash;
    uint32 nTokenLength = strlen(LOCAL_MARKER_SCOPE_TOKEN);
    if (strncmp(LOCAL_MARKER_SCOPE_TOKEN, szString, nTokenLength) == 0)
    {
        uint16 nAdventureParentIndex = LuaScriptGetScriptAdventureParentIndex(idScriptInstance);
        CRefString sStringToUse;
        if (nAdventureParentIndex != 0xffff)
        {

            LPCSTR pszScriptAdvParentName = AdventureGetParentName(nAdventureParentIndex);
            sStringToUse.Format("%s:%s", pszScriptAdvParentName, &szString[nTokenLength]);
        }
        else
        {
            sStringToUse = &szString[nTokenLength];
        }

        uHashToUse = MarkerStringHash(sStringToUse.str());

    }

    return uHashToUse;
}

// 0x710086c9e0  void ScriptEventParam::RefObjects(ScriptInstanceID idScriptInstance)































































































































































































































































































































void ScriptEventParam::RefObjects(ScriptInstanceID idScriptInstance)
{
}

// 0x710086c9f0  FloatRangeParam::FloatRangeParam(float flMin, float flMax)



FloatRangeParam::FloatRangeParam(float flMin, float flMax)
    : flMinVal(flMin)
    , flMaxVal(flMax)
{
}

// 0x710086ca10  int32 FloatRangeParam::PushParam(lua_State * ptLuaState)
int32 FloatRangeParam::PushParam(lua_State * ptLuaState)
{
    lua_pushnumber(ptLuaState, flMinVal);
    lua_pushnumber(ptLuaState, flMaxVal);

    return 2;
}

// 0x710086ca60  BOOL FloatRangeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL FloatRangeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED].SetFloatRangeEntered(flMinVal, flMaxVal);

    return TRUE;
}

// 0x710086ca80  BOOL FloatRangeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL FloatRangeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_istable(ptLuaState, -1))
    {

        lua_pushinteger(ptLuaState, 1);
        lua_gettable(ptLuaState, -2);
        float flMin = (float)lua_tonumber(ptLuaState, -1);
        lua_pop(ptLuaState, 1);

        lua_pushinteger(ptLuaState, 2);
        lua_gettable(ptLuaState, -2);
        float flMax = (float)lua_tonumber(ptLuaState, -1);
        lua_pop(ptLuaState, 1);

        if (flMin > flMax) { float flTemp = flMin; flMin = flMax; flMax = flTemp; }






        tParam.SetFloatRangeEntered(flMin, flMax);

        bParsed = TRUE;
    }


    return bParsed;
}

// 0x710086cb60  ACDIDParam::ACDIDParam(ACDID idACDActor)


ACDIDParam::ACDIDParam(ACDID idACDActor)
    : idACD(idACDActor)
{
}

// 0x710086cb80  int32 ACDIDParam::PushParam(lua_State * ptLuaState)
int32 ACDIDParam::PushParam(lua_State * ptLuaState)
{
    lua_pushinteger(ptLuaState, idACD);
    return 1;
}

// 0x710086cbb0  void ACDIDParam::RefObjects(ScriptInstanceID idScriptInstance)

void ACDIDParam::RefObjects(ScriptInstanceID idScriptInstance)
{
    LuaScriptInstanceAddRefACD(idScriptInstance, idACD);
}

// 0x710086cbc0  BOOL ACDIDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL ACDIDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    BOOL bFilled = FALSE;
    ActorCommonData * ptACD = ACDTryToGet(idACD);

    if (ptACD)
    {
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO].SetSNO(ACTOR_SNO, ptACD->snoActor);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO].SetSNO(MONSTER_SNO, ptACD->tSNOName.snoHandle);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH].SetString(MarkerNameGetString(ptACD->tName));
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SELF].SetSelfACDID(idACD);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PLAYER].SetPlayer(ACDIsPlayer(idACD));
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY].SetPlayerAlly(TeamsAreFriendly(ptACD->GetTeam(), TEAM_COLOR_NEUTRAL));
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP].SetPrimaryGroup(ptACD->uGroupPrimary);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP].SetSecondaryGroup(ptACD->uGroupSecondary);
        if (ptACD->hGB.eType == GB_ITEMS)
        {
            arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID].SetItemGBID(ptACD->hGB.gbid);
        }
        else
        {
            arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID].SetItemGBID(GBID_NONE);
        }

        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP].SetLinkedGroup("", idACD);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY].SetLinkedGroup(NULL, idACD);

        uint32 nSpawnerNameHash = 0;
        ActorCommonData * ptACDSpawner = ptACD->HasSpawner() ? ACDTryToGet((ACDID)ptACD->AttributesGetInt(ATTRIBUTE_SPAWNED_BY_ACDID)) : NULL;
        if (ptACDSpawner)
        {

            nSpawnerNameHash = ptACDSpawner->tName;
        }



        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME].SetDWORD(nSpawnerNameHash);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_SELF].SetDWORD(nSpawnerNameHash);

        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE].SetGizmoType(ptACD->GetGizmoType());

        bFilled = TRUE;
    }
    else
    {

        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO].SetSNO(ACTOR_SNO, -1);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO].SetSNO(MONSTER_SNO, -1);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH].SetString("");
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SELF].SetSelfACDID(ACDID_INVALID);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PLAYER].SetPlayer(FALSE);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY].SetPlayerAlly(FALSE);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP].SetPrimaryGroup(-1);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP].SetSecondaryGroup(-1);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID].SetItemGBID(GBID_NONE);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP].SetLinkedGroup("", ACDID_INVALID);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY].SetLinkedGroup(NULL, ACDID_INVALID);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME].SetDWORD(0);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_SELF].SetDWORD(0);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE].SetDWORD(0);
    }

    return bFilled;
}

// 0x710086d010  BOOL ACDIDParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)

BOOL ACDIDParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;
    int32 nType = lua_type(ptLuaState, -1);

    if (nType == LUA_TNIL)
    {
        bParsed = TRUE;
    }
    else if (nType == LUA_TNUMBER)
    {

        int32 nValue = (int32)lua_tointeger(ptLuaState, -1);

        ActorCommonData * ptACD = ACDTryToGet((ACDID)nValue);
        if (ptACD == NULL)
        {
            return FALSE;
        }
        tParam.SetString(MarkerNameGetString(ptACD->tName));
        bParsed = TRUE;
    }
    else if (nType == LUA_TSTRING)
    {
        size_t nLen;
        LPCSTR szStringValScoped = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            LPCSTR szSeparator = strstr(szStringValScoped, ":");

            if (szSeparator)
            {
                LPCSTR szSpecifiation = szSeparator + 1;
                if (strncasecmp(szStringValScoped, "GBID", 4) == 0)
                {
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                }



                else if (strncasecmp(szStringValScoped, "SNO", 3) == 0)
                {
                    tParam.snoValue = SNOFileLookupGUID(ACTOR_SNO, szSpecifiation); tParam.eParamType = SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO; bParsed = TRUE;
                }

                else if (strncasecmp(szStringValScoped, "NAME", 4) == 0)
                {
                    tParam.SetString(szSpecifiation); bParsed = TRUE;
                }
            }
            else
            {
                if (strncasecmp(szStringValScoped, "SELF", 4) == 0)
                {
                    tParam.SetSelf(DATAID_INVALID); bParsed = TRUE;
                }

                else if (strcasecmp(szStringValScoped, "PLAYER") == 0)
                {
                    tParam.SetPlayer(TRUE); bParsed = TRUE;
                }
            }
        }
        else
        {
            bParsed = TRUE;
        }
    }

    return bParsed;
}

// 0x710086d240  ACDIDListParam::ACDIDListParam(ACDIDListPooled & idAcdList)


ACDIDListParam::ACDIDListParam(ACDIDListPooled & idAcdList)
    : mIdAcdList(MemoryPoolsGetDATAIDListPool())
{
    POS pos = idAcdList.GetHeadPosition();
    while (pos)
    {
        DataID idAcd = idAcdList.GetNext(pos);
        mIdAcdList.AddTail_Tracked(idAcd);
    }
}

// 0x710086d300  int32 ACDIDListParam::PushParam(lua_State * ptLuaState)
int32 ACDIDListParam::PushParam(lua_State * ptLuaState)
{
    lua_createtable(ptLuaState, mIdAcdList.Count(), 0);
    int nTableIndex = lua_gettop(ptLuaState);

    int32 nIndex = 1;
    POS pos = mIdAcdList.GetHeadPosition();
    while (pos)
    {
        ACDID idAcd = (ACDID)mIdAcdList.GetNext(pos);
        lua_pushinteger(ptLuaState, nIndex);
        lua_pushinteger(ptLuaState, idAcd);
        lua_settable(ptLuaState, nTableIndex);
        nIndex++;
    }

    return 1;
}

// 0x710086d3a0  void ACDIDListParam::RefObjects(ScriptInstanceID idScriptInstance)

void ACDIDListParam::RefObjects(ScriptInstanceID idScriptInstance)
{
    POS pos = mIdAcdList.GetHeadPosition();
    while (pos)
    {
        ACDID idAcd = (ACDID)mIdAcdList.GetNext(pos);
        LuaScriptInstanceAddRefACD(idScriptInstance, idAcd);
    }
}

// 0x710086d3e0  BOOL ACDIDListParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL ACDIDListParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{

    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    return FALSE;
}

// 0x710086d410  BOOL ACDIDListParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL ACDIDListParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{


    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    return FALSE;
}

// 0x710086d440  SceneIDParam::SceneIDParam(SSceneID idSScene)


SceneIDParam::SceneIDParam(SSceneID idSScene)
    : m_idSScene(idSScene)
{
}

// 0x710086d460  int32 SceneIDParam::PushParam(lua_State * ptLuaState)
int32 SceneIDParam::PushParam(lua_State * ptLuaState)
{
    lua_pushnumber(ptLuaState, m_idSScene);

    return 1;
}

// 0x710086d490  BOOL SceneIDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL SceneIDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    SScene * ptSScene = SSceneTryToGet(m_idSScene);

    if (ptSScene)
    {
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO].SetSNO(SCENE_SNO, ptSScene->tSCD.snoScene);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SELF].SetSelf(m_idSScene);
    }
    else
    {
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO].SetSNO(SCENE_SNO, SNO_NONE);
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SELF].SetSelf(SSCENEID_INVALID);
    }

    return TRUE;
}

// 0x710086d500  BOOL SceneIDParam::ParseParamFromScript(...)  -- "SNO:" arm: lookup, snoValue, eParamType, bParsed all on line 1093; plain-name arm: SetSNO (tail-merged)
BOOL SceneIDParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        size_t nLen;
        LPCSTR szStringValScoped = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            LPCSTR szSeparator = strstr(szStringValScoped, ":");
            LPCSTR szSpecifiation = szSeparator + 1;

            if (szSeparator)
            {
                if (strncasecmp(szStringValScoped, "SNO", 3) == 0)
                {
                    tParam.snoValue = SNOFileLookupGUID(SCENE_SNO, szSpecifiation); tParam.eParamType = SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO; bParsed = TRUE;
                }
            }
            else
            {
                if (strncasecmp(szStringValScoped, "SELF", 4) == 0)
                {
                    tParam.SetSelf(SSCENEID_INVALID);
                }
                else
                {
                    tParam.SetSNO(SCENE_SNO, SNOFileLookupGUID(SCENE_SNO, szStringValScoped));
                    bParsed = TRUE;
                }
            }
        }
    }

    return bParsed;
}

// 0x710086d5e0  LevelAreaParam::LevelAreaParam(SNO snoLevelArea)
LevelAreaParam::LevelAreaParam(SNO snoLevelArea)
    : m_snoLevelArea(snoLevelArea)
{
}

// 0x710086d600  int32 LevelAreaParam::PushParam(lua_State * ptLuaState)
int32 LevelAreaParam::PushParam(lua_State * ptLuaState)
{
    CRefString sLevelAreaName;

    LevelAreaGetName(m_snoLevelArea, sLevelAreaName);

    lua_pushstring(ptLuaState, sLevelAreaName.str());

    return 1;
}

// 0x710086d660  BOOL LevelAreaParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL LevelAreaParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO].SetSNO(LEVELAREA_SNO, m_snoLevelArea);

    return TRUE;
}

// 0x710086d680  BOOL LevelAreaParam::ParseParamFromScript(...)  -- as SceneIDParam: direct stores (snoValue first) on line 1160, SetSNO in the plain-name arm; the store-order difference stops the call sink
BOOL LevelAreaParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        size_t nLen;
        LPCSTR szStringValScoped = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            LPCSTR szSeparator = strstr(szStringValScoped, ":");
            LPCSTR szSpecifiation = szSeparator + 1;

            if (szSeparator)
            {
                if (strncasecmp(szStringValScoped, "LEVELAREA", 9) == 0)
                {
                    tParam.snoValue = SNOFileLookupGUID(LEVELAREA_SNO, szSpecifiation); tParam.eParamType = SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO; bParsed = TRUE;
                }
            }
            else
            {
                tParam.SetSNO(LEVELAREA_SNO, SNOFileLookupGUID(LEVELAREA_SNO, szStringValScoped));
                bParsed = TRUE;
            }
        }
    }

    return bParsed;
}

// 0x710086d730  StringParam::StringParam(LPCSTR szString)




StringParam::StringParam(LPCSTR szString)
    : m_szString(szString)
{
}

// 0x710086d750  int32 StringParam::PushParam(lua_State * ptLuaState)
int32 StringParam::PushParam(lua_State * ptLuaState)
{
    lua_pushstring(ptLuaState, m_szString);

    return 1;
}

// 0x710086d780  BOOL StringParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL StringParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH].SetString(m_szString);

    return TRUE;
}

// 0x710086d7d0  BOOL StringParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL StringParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        size_t nLen;
        LPCSTR szStringValScoped = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            tParam.SetString(szStringValScoped);
            bParsed = TRUE;
        }
    }

    return bParsed;
}

// 0x710086d860  TimeParam::TimeParam(uint32 dwGameTime, ScriptEventVariantParam::FilterInequality eFilterInequality)


TimeParam::TimeParam(uint32 dwGameTime, ScriptEventVariantParam::FilterInequality eFilterInequality)
    : m_dwGameTime(dwGameTime)
    , m_eFilterInequality(eFilterInequality)
{
}

// 0x710086d880  int32 TimeParam::PushParam(lua_State * ptLuaState)
int32 TimeParam::PushParam(lua_State * ptLuaState)
{
    double flTime = (double)m_dwGameTime / 60.0;

    lua_pushnumber(ptLuaState, flTime);

    return 1;
}

// 0x710086d8b0  BOOL TimeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL TimeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME].SetGameTime(m_dwGameTime);
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME].eFilterInequality = m_eFilterInequality;
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL].SetGameTime(m_dwGameTime);
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL].eFilterInequality = m_eFilterInequality;

    return TRUE;
}

// 0x710086d8f0  BOOL TimeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL TimeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isnumber(ptLuaState, -1))
    {
        double flTime = lua_tonumber(ptLuaState, -1);

        if (flTime < 0.0)
        {
            flTime = 0.0;
        }

        uint32 dwGameTime = GameGetActiveGameCommonData().dwServerTime + (uint32)(flTime * 60.0);

        tParam.SetGameTime(dwGameTime);
        bParsed = TRUE;
    }

    return bParsed;
}

// 0x710086d980  TimeIntervalParam::TimeIntervalParam(float flIntervalTicks)








TimeIntervalParam::TimeIntervalParam(float flIntervalTicks)
    : m_dwIntervalTicks((uint32)(flIntervalTicks * 60.0f))
{
}

// 0x710086d9b0  int32 TimeIntervalParam::PushParam(lua_State * ptLuaState)
int32 TimeIntervalParam::PushParam(lua_State * ptLuaState)
{
    double flInterval = (double)m_dwIntervalTicks / 60.0;

    lua_pushnumber(ptLuaState, flInterval);

    return 1;
}

// 0x710086d9e0  BOOL TimeIntervalParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL TimeIntervalParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL].SetTimeInterval(m_dwIntervalTicks);

    return TRUE;
}

// 0x710086da00  BOOL TimeIntervalParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL TimeIntervalParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isnumber(ptLuaState, -1))
    {
        double flIntervalSeconds = lua_tonumber(ptLuaState, -1);

        if (flIntervalSeconds < 0.0)
        {
            flIntervalSeconds = 0.0;
        }

        tParam.SetTimeInterval((float)flIntervalSeconds);
        bParsed = TRUE;
    }

    return bParsed;
}

// 0x710086da70  AnimTagParam::AnimTagParam(uint32 dwAnimTag)



AnimTagParam::AnimTagParam(uint32 dwAnimTag)
    : m_dwAnimTag(dwAnimTag)
    , m_szTagName(NULL)
{
}

// 0x710086da90  int32 AnimTagParam::PushParam(lua_State * ptLuaState)
int32 AnimTagParam::PushParam(lua_State * ptLuaState)
{
    if (!m_szTagName)
    {
        m_szTagName = AnimTagGetName(m_dwAnimTag);
    }

    lua_pushstring(ptLuaState, m_szTagName);

    return 1;
}

// 0x710086dae0  BOOL AnimTagParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL AnimTagParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG].SetAnimTag(m_dwAnimTag);

    return TRUE;
}

// 0x710086db00  BOOL AnimTagParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL AnimTagParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        LPCSTR szAnimTagName = lua_tolstring(ptLuaState, -1, NULL);

        if (szAnimTagName && szAnimTagName[0])
        {
            uint32 dwTagId = AnimTagGetID(szAnimTagName);

            tParam.SetAnimTag(dwTagId);
            bParsed = TRUE;
        }
    }

    return bParsed;
}

// 0x710086db70  QuestNameParam::QuestNameParam(LPCSTR szQuestName)







QuestNameParam::QuestNameParam(LPCSTR szQuestName)
    : m_szQuestName(szQuestName)
    , m_snoQuest(SNOFileLookupGUID(QUEST_SNO, szQuestName))
{
}

// 0x710086dbb0  int32 QuestNameParam::PushParam(lua_State * ptLuaState)
int32 QuestNameParam::PushParam(lua_State * ptLuaState)
{
    lua_pushstring(ptLuaState, m_szQuestName);

    return 1;
}

// 0x710086dbe0  BOOL QuestNameParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL QuestNameParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME].SetQuestName(m_szQuestName);
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SELF].SetSelf(m_snoQuest);

    return TRUE;
}

// 0x710086dc50  BOOL QuestNameParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL QuestNameParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        LPCSTR szQuestName = lua_tolstring(ptLuaState, -1, NULL);

        tParam.SetQuestName(szQuestName);


        SNO snoQuest = SNOFileLookupGUID(QUEST_SNO, szQuestName);

        bParsed = TRUE;
    }

    return bParsed;
}

// 0x710086dce0  QuestRangeParam::QuestRangeParam(QuestTime tStart, QuestTime tEnd)




QuestRangeParam::QuestRangeParam(QuestTime tStart, QuestTime tEnd)
    : m_snoQuestRange(-1)
{
    for (int32 i = 0; i < 4; i++)
    {
        m_arStart[i].Initialize();
        m_arEnd[i].Initialize();
    }
    m_arStart[0] = tStart;
    m_arEnd[0] = tEnd;
}

// 0x710086dd70  QuestRangeParam::QuestRangeParam(SNO snoQuestRange)
QuestRangeParam::QuestRangeParam(SNO snoQuestRange)
    : m_snoQuestRange(snoQuestRange)
{
    for (int32 i = 0; i < 4; i++)
    {
        m_arStart[i].Initialize();
        m_arEnd[i].Initialize();
    }
    if (snoQuestRange != -1)
    {
        ScopedQuestRangeDefinitionLockR pdefQuestRange(snoQuestRange);
        for (int32 i = 0; i < pdefQuestRange->nQuestRangeCount; i++)
        {
            m_arStart[i] = pdefQuestRange->arQuestRanges[i].tStart;
            m_arEnd[i] = pdefQuestRange->arQuestRanges[i].tEnd;
        }
    }
}

// 0x710086de50  int32 QuestRangeParam::PushParam(lua_State * ptLuaState)

int32 QuestRangeParam::PushParam(lua_State * ptLuaState)
{
    CRefString sSnoString = SNOToString(QUESTRANGE_SNO, m_snoQuestRange, FALSE);

    lua_pushstring(ptLuaState, sSnoString.str());

    return 1;
}

// 0x710086deb0  BOOL QuestRangeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL QuestRangeParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    if (m_snoQuestRange != -1)
    {
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE].SetSNO(QUESTRANGE_SNO, m_snoQuestRange);
    }
    else
    {
        arGameParams[SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE].SetQuestRange(m_arStart[0], m_arEnd[0]);
    }
    return TRUE;
}

// 0x710086def0  BOOL QuestRangeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)

BOOL QuestRangeParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        size_t nLen;
        LPCSTR szName = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            tParam.snoValue = SNOFileLookupGUID(QUESTRANGE_SNO, szName);
            tParam.eParamType = SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE;
            bParsed = TRUE;
        }
    }

    return bParsed;
}

// 0x710086df60  DWORDParam::DWORDParam(uint32 dwValue)


DWORDParam::DWORDParam(uint32 dwValue)
    : m_dwValue(dwValue)
{
}

// 0x710086df80  int32 DWORDParam::PushParam(lua_State * ptLuaState)
int32 DWORDParam::PushParam(lua_State * ptLuaState)
// Pushes the value as an integer.
{
    lua_pushinteger(ptLuaState, m_dwValue);

    return 1;
}

// 0x710086dfb0  BOOL DWORDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL DWORDParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_DWORD].SetDWORD(m_dwValue);

    return TRUE;
}

// 0x710086dfd0  BOOL DWORDParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)

BOOL DWORDParam::ParseParamFromScript(ScriptEventVariantParam & tParam,
                                      lua_State * ptLuaState)

{
    BOOL bParsed = FALSE;

    if (lua_isnumber(ptLuaState, -1))
    {
        uint32 dwValue = (uint32)lua_tointeger(ptLuaState, -1);
        tParam.SetDWORD(dwValue);
        bParsed = TRUE;
    }

    return bParsed;
}

// 0x710086e020  ActorGroupParam::ActorGroupParam(uint32 dwValue)

ActorGroupParam::ActorGroupParam(uint32 dwValue)
    : m_dwValue(dwValue)
{
}

// 0x710086e040  int32 ActorGroupParam::PushParam(lua_State * ptLuaState)
int32 ActorGroupParam::PushParam(lua_State * ptLuaState)

{

    lua_pushstring(ptLuaState, ActorGroupHashToName(m_dwValue));

    return 1;
}

// 0x710086e080  BOOL ActorGroupParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL ActorGroupParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{

    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP].SetPrimaryGroup(m_dwValue);
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP].SetSecondaryGroup(m_dwValue);

    return TRUE;
}

// 0x710086e0b0  BOOL ActorGroupParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL ActorGroupParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isnumber(ptLuaState, -1))
    {
        uint32 dwValue = (uint32)lua_tointeger(ptLuaState, -1);
        tParam.SetPrimaryGroup(dwValue);
        bParsed = TRUE;
    }

    return bParsed;
}

// 0x710086e100  FloatRangeInsideParam::FloatRangeInsideParam(float fValue)




FloatRangeInsideParam::FloatRangeInsideParam(float fValue)
    : m_fValue(fValue)
{
}

// 0x710086e120  int32 FloatRangeInsideParam::PushParam(lua_State * ptLuaState)
int32 FloatRangeInsideParam::PushParam(lua_State * ptLuaState)
{
    lua_pushnumber(ptLuaState, m_fValue);
    return 1;
}

// 0x710086e150  BOOL FloatRangeInsideParam::ToGameParam(ScriptEventVariantParam * arGameParams)
BOOL FloatRangeInsideParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{
    arGameParams[SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE].SetFloatRangeInside(m_fValue);
    return TRUE;
}

// 0x710086e170  BOOL FloatRangeInsideParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL FloatRangeInsideParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    if (lua_isnumber(ptLuaState, -1))
    {
        double fValue = lua_tonumber(ptLuaState, -1);
        tParam.SetFloat((float)fValue);
        return TRUE;
    }

    return FALSE;
}

// 0x710086e1c0  SNOParam::SNOParam(const SNOGroup snogroup, const SNO snoVal, const ScriptEventVariantParamType eParamType)

SNOParam::SNOParam(
    const SNOGroup snogroup,
    const SNO snoVal,
    const ScriptEventVariantParamType eParamType)
    : m_snogroup(snogroup), m_snoVal(snoVal), m_eParamType(eParamType)
{
}

// 0x710086e1e0  int32 SNOParam::PushParam(lua_State * ptLuaState)
int32 SNOParam::PushParam(lua_State * ptLuaState)
{
    CRefString sSnoString = SNOToString(m_snogroup, m_snoVal, FALSE);

    lua_pushstring(ptLuaState, sSnoString.str());


    return 1;
}
// 0x710086e240  BOOL SNOParam::ToGameParam(ScriptEventVariantParam * arGameParams)

BOOL SNOParam::ToGameParam(ScriptEventVariantParam * arGameParams)
{

    arGameParams[m_eParamType].SetSNO(m_snogroup, m_snoVal);

    return TRUE;
}

// 0x710086e270  BOOL SNOParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
BOOL SNOParam::ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_isstring(ptLuaState, -1))
    {
        size_t nLen;
        LPCSTR szName;

        szName = lua_tolstring(ptLuaState, -1, &nLen);

        if (nLen > 0)
        {
            tParam.SetSNO(m_snogroup, SNOFileLookupGUID(m_snogroup, szName)); bParsed = TRUE;
        }
    }

    return bParsed;
}

// 0x710086e2f0  ScriptEventParamList::ScriptEventParamList(int32 nNumParams, ScriptEventParam ** arParams, LPCSTR *)





ScriptEventParamList::ScriptEventParamList(int32 nNumParams, ScriptEventParam ** arParams, LPCSTR *)
{
    for (int32 i = 0; i < nNumParams; ++i)
    {
        m_arParams[i] = arParams[i];
    }
    m_nNumParams = nNumParams;
}
// 0x710086e390  ScriptEventParamList::ScriptEventParamList()
ScriptEventParamList::ScriptEventParamList()
    : m_nNumParams(0)
{
}
// 0x710086e3a0  ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1)
ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1)
    : m_nNumParams(1)
{
    m_arParams[0] = ptParam1;
}
// 0x710086e3b0  ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2)
ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2)
    : m_nNumParams(2)
{
    m_arParams[0] = ptParam1;
    m_arParams[1] = ptParam2;
}
// 0x710086e3c0  ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2, ScriptEventParam * ptParam3)
ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2, ScriptEventParam * ptParam3)
    : m_nNumParams(3)
{
    m_arParams[0] = ptParam1;
    m_arParams[1] = ptParam2;
    m_arParams[2] = ptParam3;
}
// 0x710086e3e0  ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2, ScriptEventParam * ptParam3, ScriptEventParam * ptParam4)
ScriptEventParamList::ScriptEventParamList(ScriptEventParam * ptParam1, ScriptEventParam * ptParam2, ScriptEventParam * ptParam3, ScriptEventParam * ptParam4)
    : m_nNumParams(4)
{
    m_arParams[0] = ptParam1;
    m_arParams[1] = ptParam2;
    m_arParams[2] = ptParam3;
    m_arParams[3] = ptParam4;
}

// 0x710086e400  BOOL ScriptEventParamList::ToGameParams(ScriptEventGameParams & tGameParams)
BOOL ScriptEventParamList::ToGameParams(ScriptEventGameParams & tGameParams)
{
    BOOL bFilled = TRUE;
    for (int32 i = 0; i < m_nNumParams; ++i)
    {
        bFilled &= m_arParams[i]->ToGameParam(tGameParams.arGameParams[i]);
    }
    tGameParams.nNumParams = m_nNumParams;
    return bFilled;
}

// 0x710086e490  int32 ScriptEventParamList::PushScriptParams(lua_State * ptLuaState)
int32 ScriptEventParamList::PushScriptParams(lua_State * ptLuaState)
{
    int32 nParamsPushed = 0;

    for (int32 i = 0; i < m_nNumParams; ++i)
    {
        nParamsPushed += m_arParams[i]->PushParam(ptLuaState);
    }

    return nParamsPushed;
}

// 0x710086e500  void ScriptEventParamList::RefObjects(ScriptInstanceID idScriptInstance)
void ScriptEventParamList::RefObjects(ScriptInstanceID idScriptInstance)
{
    if (idScriptInstance == SCRIPTINSTANCEID_INVALID)
        return;
    for (int32 i = 0; i < m_nNumParams; ++i)
    {
        m_arParams[i]->RefObjects(idScriptInstance);
    }

}

// 0x710086e570  BOOL ScriptEventParamList::ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState)
BOOL ScriptEventParamList::ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState)
{
    BOOL bParsed = FALSE;

    if (lua_type(ptLuaState, -1) == LUA_TTABLE)
    {
        int32 nParams;
        int32 nTableSize = LuaScriptGetTableSize(ptLuaState, -1);
        LuaStackChecker tStackCheck(ptLuaState);

        nParams = nTableSize > m_nNumParams ? m_nNumParams : nTableSize;

        bParsed = TRUE;





        for (int32 i = 0; i < nParams; ++i)
        {
            lua_pushnumber(ptLuaState, i + 1);
            lua_gettable(ptLuaState, -2);

            bParsed &= m_arParams[i]->ParseParamFromScript(tParams.arParams[i], ptLuaState);

            lua_pop(ptLuaState, 1);
        }

        tParams.nNumParams = nParams;
    }






    return bParsed;
}

// 0x710086e670  ScriptEventReturnValueBool::ScriptEventReturnValueBool(BOOL bDefault)
ScriptEventReturnValueBool::ScriptEventReturnValueBool(BOOL bDefault)
    : ScriptEventReturnValue(1)
    , m_bValue(bDefault)
{
}
// 0x710086e690  BOOL ScriptEventReturnValueBool::HandleScriptReturnValues(lua_State * ptLuaState)
BOOL ScriptEventReturnValueBool::HandleScriptReturnValues(lua_State * ptLuaState)
{
    int32 nTop = lua_gettop(ptLuaState);
    if (lua_type(ptLuaState, -1) != LUA_TNIL && nTop > 0)
    {
        m_bValue = lua_toboolean(ptLuaState, -1);
    }

    return TRUE;
}

// 0x710086e6f0  ScriptEventVariantParam & ScriptEventVariantParam::operator=(const ScriptEventVariantParamLW & tSource)









































ScriptEventVariantParam & ScriptEventVariantParam::operator=(const ScriptEventVariantParamLW & tSource)
{
    eParamType = tSource.eParamType;
    tQuestStart = tSource.tQuestStart;
    tQuestEnd = tSource.tQuestEnd;
    switch (tSource.eParamType)
    {
    case SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH:
    case SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME:
    case SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP:
    case SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME:
        Blizzard::String::Copy(szString, tSource.szString, MAX_SCRIPT_EVENT_STRING_LENGTH);
        break;
    default:
        break;
    }
    return *this;
}


