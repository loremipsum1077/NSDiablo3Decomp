// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/ScriptEventParams.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/QuestRangeDefinition.h"
#include "../../Common/GameBalance.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/ThreadGlobals.h"
#include "LuaScript.h"
#include "../Powers/Powers.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../Common/Marker/Marker.h"

class CRefString;
struct lua_State;

typedef int32 SNO;

typedef int32 ScriptID;

typedef int32 ScriptInstanceID;

enum {
    MAX_NUM_SCRIPT_EVENT_PARAMS = 4,
    MAX_SCRIPT_EVENT_STRING_LENGTH = 96,
    SCRIPT_EVENT_VARIANT_PARAM_SNO_VERSION = 24,
};

enum ScriptEventVariantParamType {
    SCRIPT_EVENT_VARIANT_PARAM_NONE = 0,
    SCRIPT_EVENT_VARIANT_PARAM_DWORD = 1,
    SCRIPT_EVENT_VARIANT_PARAM_FLOAT = 2,
    SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED = 3,
    SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO = 4,
    SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO = 5,
    SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO = 6,
    SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH = 7,
    SCRIPT_EVENT_VARIANT_PARAM_PLAYER = 8,
    SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY = 9,
    SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO = 10,
    SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION = 11,
    SCRIPT_EVENT_VARIANT_PARAM_SELF = 12,
    SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME = 13,
    SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG = 14,
    SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME = 15,
    SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL = 16,
    SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP = 17,
    SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP = 18,
    SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID = 19,
    SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY = 20,
    SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP = 21,
    SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE = 22,
    SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO = 23,
    SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_NAME = 24,
    SCRIPT_EVENT_VARIANT_PARAM_SPAWNER_PARENT_SELF = 25,
    SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO = 26,
    SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO = 27,
    SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE = 28,
    SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE = 29,
    SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO = 30,
    SCRIPT_EVENT_NUM_VARIANT_PARAM_TYPES = 31,
};

struct ScriptEventVariantParamLW {  // size 0x18
    enum FilterInequality {
        LESS_THAN_THIS_VALUE = 0,
        GREATER_THAN_THIS_VALUE = 1,
    };
    union {  // size 0x10
        uint32 dwValue;  // 0x00
        struct {  // size 0x8
            uint32 dwTimeValue;  // 0x00
            ScriptEventVariantParamLW::FilterInequality eFilterInequality;  // 0x04
        };  // 0x00
        float flValue;  // 0x00
        SNO snoValue;  // 0x00
        GBID gbidValue;  // 0x00
        DataID idValue;  // 0x00
        struct {  // size 0x8
            float flMinValue;  // 0x00
            float flMaxValue;  // 0x04
        };  // 0x00
        struct {  // size 0x10
            uint32 dwHash;  // 0x00
            uint32 dwPadding;  // 0x04
            union {  // size 0x8
                LPCSTR szString;  // 0x00
                uint64 dwString64;  // 0x00
            };  // 0x08
        };  // 0x00
        struct {  // size 0x10
            QuestTime tQuestStart;  // 0x00
            QuestTime tQuestEnd;  // 0x08
        };  // 0x00
    };  // 0x00
    ScriptEventVariantParamType eParamType;  // 0x10
};
static_assert(sizeof(ScriptEventVariantParamLW) == 0x18, "ScriptEventVariantParamLW");

struct ScriptEventVariantParam {  // size 0x68
    ScriptEventVariantParamType eParamType;  // 0x00
    BOOL FilterAgainst(const ScriptEventVariantParam & rhs, DataID idSelf, ScriptID idScript, ScriptInstanceID idScriptInstance) const;
    enum FilterInequality {
        LESS_THAN_THIS_VALUE = 0,
        GREATER_THAN_THIS_VALUE = 1,
    };
    union {  // size 0x64
        uint32 dwValue;  // 0x00
        struct {  // size 0x8
            uint32 dwTimeValue;  // 0x00
            ScriptEventVariantParam::FilterInequality eFilterInequality;  // 0x04
        };  // 0x00
        float flValue;  // 0x00
        SNO snoValue;  // 0x00
        GBID gbidValue;  // 0x00
        DataID idValue;  // 0x00
        struct {  // size 0x8
            float flMinValue;  // 0x00
            float flMaxValue;  // 0x04
        };  // 0x00
        struct {  // size 0x64
            uint32 dwHash;  // 0x00
            CHAR szString[96];  // 0x04
        };  // 0x00
        struct {  // size 0x10
            QuestTime tQuestStart;  // 0x00
            QuestTime tQuestEnd;  // 0x08
        };  // 0x00
    };  // 0x04
    ScriptEventVariantParam & operator=(const ScriptEventVariantParamLW & tSource);
    uint32 GetAdventureScopedStringHash(ScriptInstanceID idScriptInstance) const;
    CRefString ToString();
    CRefString ToValueString();
    void SetDWORD(uint32 dwVal);
    void SetFloat(float flVal);
    void SetSNO(const SNOGroup snogroup, const SNO sno);
    void SetSelfACDID(ACDID idACDVal);
    void SetPlayer(BOOL bPlayer);
    void SetPlayerAlly(BOOL bAlly);
    void SetSelf(DataID idVal);
    void SetFloatRangeEntered(float flMin, float flMax);
    void SetFloatRangeInside(float flVal);
    void SetString(LPCSTR szSrcString);
    void SetGameTime(uint32 dwGameTime);
    void SetTimeInterval(float flIntervalSeconds);
    void SetTimeInterval(uint32 dwIntervalTicks);
    void SetAnimTag(uint32 dwAnimTag);
    void SetQuestName(LPCSTR szQuestName);
    void SetPrimaryGroup(uint32 uHash);
    void SetSecondaryGroup(uint32 uHash);
    void SetItemGBID(GBID gbidItem);
    void SetLinkedGroup(LPCSTR szGroupName, ACDID idACDVal);
    void SetGizmoType(uint32);
    void SetQuestRange(QuestTime tStart, QuestTime tEnd);
};
static_assert(sizeof(ScriptEventVariantParam) == 0x68, "ScriptEventVariantParam");

struct ScriptEventParam {  // size 0x8
    // vtable pointer
    virtual ~ScriptEventParam() {}  // vtable slot 0, line 242 (implicit in the original)
    virtual int32 PushParam(lua_State *) = 0;  // vtable slot 2
    virtual void RefObjects(ScriptInstanceID idScriptInstance);  // vtable slot 3
    virtual BOOL ToGameParam(ScriptEventVariantParam *) = 0;  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam &, lua_State *) = 0;  // vtable slot 5
};
static_assert(sizeof(ScriptEventParam) == 0x8, "ScriptEventParam");

struct FloatRangeParam : public ScriptEventParam {  // size 0x10
    float flMinVal;  // 0x08
    float flMaxVal;  // 0x0c
    FloatRangeParam(float, float);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(FloatRangeParam) == 0x10, "FloatRangeParam");

struct ACDIDParam : public ScriptEventParam {  // size 0x10
    ACDID idACD;  // 0x08
    ACDIDParam(ACDID);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual void RefObjects(ScriptInstanceID idScriptInstance);  // vtable slot 3
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(ACDIDParam) == 0x10, "ACDIDParam");

struct ACDIDListParam : public ScriptEventParam {  // size 0x30
    ACDIDListPooled mIdAcdList;  // 0x08
    ACDIDListParam(ACDIDListPooled &);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual void RefObjects(ScriptInstanceID idScriptInstance);  // vtable slot 3
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(ACDIDListParam) == 0x30, "ACDIDListParam");

struct SceneIDParam : public ScriptEventParam {  // size 0x10
    SSceneID m_idSScene;  // 0x08
    SceneIDParam(SSceneID);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(SceneIDParam) == 0x10, "SceneIDParam");

struct LevelAreaParam : public ScriptEventParam {  // size 0x10
    SNO m_snoLevelArea;  // 0x08
    LevelAreaParam(SNO);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(LevelAreaParam) == 0x10, "LevelAreaParam");

struct StringParam : public ScriptEventParam {  // size 0x10
    LPCSTR m_szString;  // 0x08
    StringParam(LPCSTR);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(StringParam) == 0x10, "StringParam");

struct TimeParam : public ScriptEventParam {  // size 0x10
    uint32 m_dwGameTime;  // 0x08
    ScriptEventVariantParam::FilterInequality m_eFilterInequality;  // 0x0c
    TimeParam(uint32, ScriptEventVariantParam::FilterInequality);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(TimeParam) == 0x10, "TimeParam");

struct TimeIntervalParam : public ScriptEventParam {  // size 0x10
    uint32 m_dwIntervalTicks;  // 0x08
    TimeIntervalParam(uint32);
    TimeIntervalParam(float);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(TimeIntervalParam) == 0x10, "TimeIntervalParam");

struct AnimTagParam : public ScriptEventParam {  // size 0x18
    uint32 m_dwAnimTag;  // 0x08
    LPCSTR m_szTagName;  // 0x10
    AnimTagParam(uint32);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(AnimTagParam) == 0x18, "AnimTagParam");

struct QuestNameParam : public ScriptEventParam {  // size 0x18
    LPCSTR m_szQuestName;  // 0x08
    SNO m_snoQuest;  // 0x10
    QuestNameParam(LPCSTR);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(QuestNameParam) == 0x18, "QuestNameParam");

struct QuestRangeParam : public ScriptEventParam {  // size 0x50
    SNO m_snoQuestRange;  // 0x08
    QuestTime m_arStart[4];  // 0x0c
    QuestTime m_arEnd[4];  // 0x2c
    QuestRangeParam(QuestTime, QuestTime);
    QuestRangeParam(SNO);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(QuestRangeParam) == 0x50, "QuestRangeParam");

struct DWORDParam : public ScriptEventParam {  // size 0x10
    uint32 m_dwValue;  // 0x08
    DWORDParam(uint32);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(DWORDParam) == 0x10, "DWORDParam");

struct ActorGroupParam : public ScriptEventParam {  // size 0x10
    uint32 m_dwValue;  // 0x08
    ActorGroupParam(uint32);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(ActorGroupParam) == 0x10, "ActorGroupParam");

struct FloatRangeInsideParam : public ScriptEventParam {  // size 0x10
    float m_fValue;  // 0x08
    FloatRangeInsideParam(float);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
};
static_assert(sizeof(FloatRangeInsideParam) == 0x10, "FloatRangeInsideParam");

class SNOParam : public ScriptEventParam {  // size 0x18
public:
    SNOParam(const SNOGroup, const SNO, const ScriptEventVariantParamType);
    virtual int32 PushParam(lua_State * ptLuaState);  // vtable slot 2
    virtual BOOL ToGameParam(ScriptEventVariantParam * arGameParams);  // vtable slot 4
    virtual BOOL ParseParamFromScript(ScriptEventVariantParam & tParam, lua_State * ptLuaState);  // vtable slot 5
protected:
    SNOGroup m_snogroup;  // 0x08
    SNO m_snoVal;  // 0x0c
    ScriptEventVariantParamType m_eParamType;  // 0x10
};
static_assert(sizeof(SNOParam) == 0x18, "SNOParam");

struct ConversationParam : public SNOParam {  // size 0x18
    ConversationParam(SNO snoVal) : SNOParam(CONVERSATION_SNO, snoVal, SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION) {}  // line 572 (body is a guess: unproven helper)
};
static_assert(sizeof(ConversationParam) == 0x18, "ConversationParam");

struct TimedEventParam : public SNOParam {  // size 0x18
    TimedEventParam(SNO snoVal) : SNOParam(TIMEDEVENT_SNO, snoVal, SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO) {}  // line 581 (body is a guess: unproven helper)
};
static_assert(sizeof(TimedEventParam) == 0x18, "TimedEventParam");

struct WorldParam : public SNOParam {  // size 0x18
    WorldParam(SNO snoVal) : SNOParam(WORLD_SNO, snoVal, SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO) {}  // line 590 (body is a guess: unproven helper)
};
static_assert(sizeof(WorldParam) == 0x18, "WorldParam");

struct BossEncounterParam : public SNOParam {  // size 0x18
    BossEncounterParam(SNO snoVal) : SNOParam(BOSS_ENCOUNTER_SNO, snoVal, SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO) {}  // line 599 (body is a guess: unproven helper)
};
static_assert(sizeof(BossEncounterParam) == 0x18, "BossEncounterParam");

struct PowerParam : public SNOParam {  // size 0x18
    PowerParam(SNO snoVal) : SNOParam(POWER_SNO, snoVal, SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO) {}  // line 608 (body is a guess: unproven helper)
};
static_assert(sizeof(PowerParam) == 0x18, "PowerParam");

struct ScriptEventGameParams {  // size 0x3264
    int32 nNumParams;  // 0x00
    ScriptEventVariantParam arGameParams[4][31];  // 0x04
    ScriptEventGameParams();
};
static_assert(sizeof(ScriptEventGameParams) == 0x3264, "ScriptEventGameParams");

struct ScriptEventFilters {  // size 0x1a4
    int32 nNumParams;  // 0x00
    union {  // size 0x1a0
        ScriptEventVariantParam arParams[4];  // 0x00
        struct {  // size 0x1a0
            ScriptEventVariantParam tParam1;  // 0x00
            ScriptEventVariantParam tParam2;  // 0x68
            ScriptEventVariantParam tParam3;  // 0xd0
            ScriptEventVariantParam tParam4;  // 0x138
        };  // 0x00
    };  // 0x04
    ScriptEventFilters();
    CRefString ToString();
};
static_assert(sizeof(ScriptEventFilters) == 0x1a4, "ScriptEventFilters");

struct ScriptEventParamList {  // size 0x28
    ScriptEventParam * m_arParams[4];  // 0x00
    int32 m_nNumParams;  // 0x20
    ScriptEventParamList();
    ScriptEventParamList(int32, ScriptEventParam **, LPCSTR *);
    ScriptEventParamList(ScriptEventParam *);
    ScriptEventParamList(ScriptEventParam *, ScriptEventParam *);
    ScriptEventParamList(ScriptEventParam *, ScriptEventParam *, ScriptEventParam *);
    ScriptEventParamList(ScriptEventParam *, ScriptEventParam *, ScriptEventParam *, ScriptEventParam *);
    BOOL ToGameParams(ScriptEventGameParams & tGameParams);
    int32 PushScriptParams(lua_State * ptLuaState);
    void RefObjects(ScriptInstanceID idScriptInstance);
    BOOL ParseFiltersFromScript(ScriptEventFilters & tParams, lua_State * ptLuaState);
};
static_assert(sizeof(ScriptEventParamList) == 0x28, "ScriptEventParamList");

struct ScriptEventReturnValue {  // size 0x10
    // vtable pointer
    int32 m_nNumReturnValues;  // 0x08
    ScriptEventReturnValue(int32 nNumReturnValues) : m_nNumReturnValues(nNumReturnValues) {}  // line 731
    virtual ~ScriptEventReturnValue() {}  // vtable slot 0, line 736
    virtual BOOL HandleScriptReturnValues(lua_State *);  // vtable slot 2
};
static_assert(sizeof(ScriptEventReturnValue) == 0x10, "ScriptEventReturnValue");

struct ScriptEventReturnValueBool : public ScriptEventReturnValue {  // size 0x10
    BOOL m_bValue;  // 0x0c
    ScriptEventReturnValueBool(BOOL);
    virtual BOOL HandleScriptReturnValues(lua_State * ptLuaState);  // vtable slot 2
};
static_assert(sizeof(ScriptEventReturnValueBool) == 0x10, "ScriptEventReturnValueBool");

struct ScriptEventReturnValueInt : public ScriptEventReturnValue {  // size 0x10
    int32 m_nValue;  // 0x0c
    ScriptEventReturnValueInt(int32);
    virtual BOOL HandleScriptReturnValues(lua_State *);  // vtable slot 2
};
static_assert(sizeof(ScriptEventReturnValueInt) == 0x10, "ScriptEventReturnValueInt");

// ---------------------------------------------------------------------------
// Inline helpers. The original header defines them at lines 769-974, and
// SetSNO's assert bakes __LINE__ 820 into every caller, so this block is kept
// on its original lines: the blank run below pads it there. Do not insert or
// remove lines anywhere above the "APPEND BELOW" marker at the end of the file.
// ---------------------------------------------------------------------------
































































































































































































































































































































































inline void ScriptEventVariantParam::SetDWORD(uint32 dwVal)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_DWORD;
    dwValue = dwVal;
}

inline void ScriptEventVariantParam::SetFloat(float flVal)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_FLOAT;
    flValue = flVal;
}

inline void ScriptEventVariantParam::SetSNO(const SNOGroup snogroup, const SNO sno)
{
    ScriptEventVariantParamType eType;

    switch (snogroup)
    {
    case CONVERSATION_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_CONVERSATION;
        break;
    case MONSTER_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_MONSTER_SNO;
        break;
    case LEVELAREA_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_LEVEL_AREA_SNO;
        break;
    case SCENE_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_SCENE_SNO;
        break;
    case ACTOR_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_ACTOR_SNO;
        break;
    case TIMEDEVENT_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_TIMED_EVENT_SNO;
        break;
    case WORLD_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_WORLD_SNO;
        break;
    case POWER_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_POWER_SNO;
        break;
    case QUESTRANGE_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE;
        break;
    case BOSS_ENCOUNTER_SNO:
        eType = SCRIPT_EVENT_VARIANT_PARAM_BOSS_ENCOUNTER_SNO;
        break;
    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    eParamType = eType;
    snoValue = sno;
}
inline void ScriptEventVariantParam::SetSelfACDID(ACDID idACDVal)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_SELF;
    idValue = idACDVal;
}

inline void ScriptEventVariantParam::SetPlayer(BOOL bPlayer)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_PLAYER;
    dwValue = bPlayer;
}

inline void ScriptEventVariantParam::SetPlayerAlly(BOOL bAlly)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_PLAYER_ALLY;
    dwValue = bAlly;
}

inline void ScriptEventVariantParam::SetSelf(DataID idVal)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_SELF;
    idValue = idVal;
}

inline void ScriptEventVariantParam::SetFloatRangeEntered(float flMin, float flMax)
{


    eParamType = SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_ENTERED;
    flMinValue = flMin;
    flMaxValue = flMax;
}

inline void ScriptEventVariantParam::SetFloatRangeInside(float flVal)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_FLOAT_RANGE_INSIDE;
    flValue = flVal;
}

inline void ScriptEventVariantParam::SetString(LPCSTR szSrcString)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_STRING_HASH;
    Blizzard::String::Copy(szString, szSrcString, MAX_SCRIPT_EVENT_STRING_LENGTH);

    dwHash = MarkerStringHash(szString);
}




inline void ScriptEventVariantParam::SetGameTime(uint32 dwGameTime)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_GAME_TIME;
    dwTimeValue = dwGameTime;
}


inline void ScriptEventVariantParam::SetTimeInterval(float flIntervalSeconds)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL;
    dwValue = (uint32)(flIntervalSeconds * 60.0f);
}

inline void ScriptEventVariantParam::SetTimeInterval(uint32 dwIntervalTicks)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_TIME_INTERVAL;
    dwValue = dwIntervalTicks;
}

inline void ScriptEventVariantParam::SetAnimTag(uint32 dwAnimTag)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_ANIM_TAG;
    dwValue = dwAnimTag;
}

inline void ScriptEventVariantParam::SetQuestName(LPCSTR szQuestName)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_QUEST_NAME;
    Blizzard::String::Copy(szString, szQuestName, MAX_SCRIPT_EVENT_STRING_LENGTH);

    dwHash = MarkerStringHash(szString);
}




inline void ScriptEventVariantParam::SetPrimaryGroup(uint32 uHash)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_PRIMARY_GROUP;
    dwValue = uHash;
}

inline void ScriptEventVariantParam::SetSecondaryGroup(uint32 uHash)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_SECONDARY_GROUP;
    dwValue = uHash;
}

inline void ScriptEventVariantParam::SetItemGBID(GBID gbidItem)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_ITEM_GBID;
    gbidValue = gbidItem;
}

inline void ScriptEventVariantParam::SetLinkedGroup(LPCSTR szGroupName, ACDID idACDVal)
{

    if (szGroupName)
    {
        eParamType = SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_IN_GROUP;
    }
    else
    {
        eParamType = SCRIPT_EVENT_VARIANT_PARAM_LINKED_MARKER_ANY;
    }


    Blizzard::String::Copy(szString, szGroupName, MAX_SCRIPT_EVENT_STRING_LENGTH);
    idValue = idACDVal;
}

inline void ScriptEventVariantParam::SetGizmoType(uint32 dwGizmoType)
{

    eParamType = SCRIPT_EVENT_VARIANT_PARAM_GIZMO_TYPE;
    dwValue = dwGizmoType;
}

inline void ScriptEventVariantParam::SetQuestRange(QuestTime tStart, QuestTime tEnd)
{


    eParamType = SCRIPT_EVENT_VARIANT_PARAM_QUESTRANGE;
    tQuestStart = tStart;
    tQuestEnd = tEnd;
}

// ---- APPEND BELOW: declarations added for callers go here, never above ----

// ScriptEventParams.h:671; original ElementNew at 0x8684e8 clears the full filter.
inline ScriptEventFilters::ScriptEventFilters() { SigmaMemoryClear(this, sizeof(*this)); }

// Original dispatch has no initialization before ToGameParams; no emitted ctor exists.
inline ScriptEventGameParams::ScriptEventGameParams() {}
