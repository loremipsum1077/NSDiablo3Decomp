// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/ScriptEventInternal.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "ScriptEvent.h"
#include "ScriptEventParams.h"
#include "ScriptEventSwig.h"

#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

typedef int32 ScriptEventListenerDefinitionID;

class ScriptEventDefinition {  // size 0x30
public:
    ScriptEventType m_eEventType;  // 0x00
    ScriptEventParamList m_tEventDefParams;  // 0x08
    ScriptEventDefinition(ScriptEventType, int32, ScriptEventParam **, LPCSTR *);
    BOOL ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState);
};
static_assert(sizeof(ScriptEventDefinition) == 0x30, "ScriptEventDefinition");

enum {
    SCRIPT_EVENT_LISTENER_DEF_NO_ATTACH_PARAMETERS = 0,
    SCRIPT_EVENT_LISTENER_DEF_ONE_SHOT = 1,
    SCRIPT_EVENT_LISTENER_IS_CONDUCTOR = 2,
};

struct ScriptEventListenerDefinition {  // size 0x1c8
    ScriptEventListenerDefinitionID id;  // 0x00
    ScriptEventType eEventType;  // 0x04
    ScriptEventFilters tFilters;  // 0x08
    CHAR * szFuncName;  // 0x1b0
    uint32 dwFlags;  // 0x1b8
    int32 nIndex;  // 0x1bc
    int32 nEventCount;  // 0x1c0
    int32 nEventCountRequired;  // 0x1c4
};
static_assert(sizeof(ScriptEventListenerDefinition) == 0x1c8, "ScriptEventListenerDefinition");

typedef TDataArray<ScriptEventListenerDefinition, int> ScriptEventListenerDefinitionDataArray;

enum ScriptEventVariableType {
    INVALID_PARAM_TYPE = 0,
    ACDID_PARAM_TYPE = 1,
    FLOAT_RANGE_PARAM_TYPE = 2,
    FLOAT_RANGE_INSIDE_PARAM_TYPE = 3,
    CONV_PARAM_TYPE = 4,
    SCENEID_PARAM_TYPE = 5,
    LEVELAREA_PARAM_TYPE = 6,
    STRING_PARAM_TYPE = 7,
    TIME_PARAM_TYPE = 8,
    ANIMTAG_PARAM_TYPE = 9,
    QUEST_NAME_PARAM_TYPE = 10,
    TIME_INTERVAL_PARAM_TYPE = 11,
    DWORD_PARAM_TYPE = 12,
    ACTOR_GROUP_PARAM_TYPE = 13,
    TIMED_EVENT_PARAM_TYPE = 14,
    WORLD_PARAM_TYPE = 15,
    POWER_PARAM_TYPE = 16,
    QUESTRANGE_PARAM_TYPE = 17,
    BOSS_ENCOUNTER_PARAM_TYPE = 18,
};

struct ScriptEventDescription {  // size 0x88
    ScriptEventType eEventType;  // 0x00
    LPCSTR szDescription;  // 0x08
    LPCSTR szGrammar;  // 0x10
    ScriptEventVariableType arParamTypes[4];  // 0x18
    LPCSTR szParamNames[4];  // 0x28
    LPCSTR szFuncParamNames[4];  // 0x48
    LPCSTR szParamDescriptions[4];  // 0x68
};
static_assert(sizeof(ScriptEventDescription) == 0x88, "ScriptEventDescription");

enum {
    TICKS_PER_HEARTBEAT = 15,
};


// Functions defined in this file (bodies are not in the DWARF):
//   line    61  ScriptEventDefinition::ScriptEventDefinition(ScriptEventType eType, int32 nNumParams, ScriptEventParam ** arParams, LPCSTR * arParamNames)  [inlined into 1 functions]
//   line    68  BOOL ScriptEventDefinition::ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState)  [inlined into 1 functions]
//   line    87  ScriptEventListenerDefinition::ScriptEventListenerDefinition()  [inlined into 1 functions]

// ScriptEventInternal.h:61 and 68, recovered from app init and ParseEventFilters.
inline ScriptEventDefinition::ScriptEventDefinition(ScriptEventType eType, int32 nNumParams, ScriptEventParam ** arParams, LPCSTR * arParamNames) : m_eEventType(eType), m_tEventDefParams(nNumParams, arParams, arParamNames) {}
inline BOOL ScriptEventDefinition::ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState) { return m_tEventDefParams.ParseFiltersFromScript(tParams, ptLuaState); }
