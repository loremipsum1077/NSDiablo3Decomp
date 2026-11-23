// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/LuaScript/LuaScript.h
// Subset: the script and instance API the power scripts call.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
typedef int32 ScriptReferenceType; // Original LuaScript.h
#include "../../Common/CommonTypes.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/DataStructures/XTemplate.h"

struct lua_State;
struct ScriptDefinition;
struct ConductorInstance;

typedef int32 ScriptID;

typedef int32 DataID;

enum {
    SCRIPTID_INVALID = -1,
};

enum {
    SCRIPTINSTANCEID_INVALID = -1,
};

typedef int32 ScriptInstanceID;

enum {
    SCRIPT_ATTACH_NONE = 0,
    SCRIPT_ATTACH_ACTOR = 1,
    SCRIPT_ATTACH_QUEST = 2,
    SCRIPT_ATTACH_SCENE = 3,
    SCRIPT_ATTACH_WORLD = 4,
    SCRIPT_ATTACH_POWER = 5,
};

typedef int32 ScriptAttachmentType;

enum {
    ACTOR_ID_ARRAY_MAX_ACTORS = 64,
};

struct ActorIDArray {  // size 0x108
    ACDID arActorIDs[64];  // 0x00
    int32 nNumActorIDs;  // 0x100
    BOOL bWantAddRef;  // 0x104
};
static_assert(sizeof(ActorIDArray) == 0x108, "ActorIDArray");
ActorIDArray & LuaScriptGetThreadSafeActorIDArray();  // LuaScript.cpp:1166

void LuaTickScriptInstanceId(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:1554
ScriptInstanceID LuaScriptNewInstanceWithScriptID(ScriptID idScript, LPCSTR szEntryPoint, lua_State * ptParentState, SNO snoParentScript, DataID idAttachment, ScriptAttachmentType eAttachmentType);  // LuaScript.cpp:2124
lua_State * LuaScriptGetInstanceState(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:2264
void LuaScriptSetInstanceResumeTick(ScriptInstanceID idScriptInstance, uint32 dwResumeTick, int32 nResumeArgCnt);  // LuaScript.cpp:2345
ScriptID LuaScriptCreateState(SNO snoScript, uint16 nAdventureParentIndex, ConductorInstance * ptConductor);  // LuaScript.cpp:3046
lua_State * LuaScriptTryToGetLuaStateWithScriptID(ScriptID idScript);  // LuaScript.cpp:3393
lua_State * LuaScriptTryToGetLuaStateWithScriptInstanceID(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:3431
int32 LuaScriptGetExecutingStackDepth();  // LuaScript.cpp:5339
void LuaVerifySnosAreReferenced(const SNO param0, const SNO param1, const SNO param2, const SNO param3,
                                const SNO param4, const SNO param5, const SNO param6);  // LuaScript.cpp:5632
void LuaScriptPrintExecutingStackString();  // LuaScript.cpp:5397

POS LuaScriptAttach(ScriptAttachmentType eType, DataID idAttachment, SNO snoScript,
                    uint16 nAdventureParentIndex);  // LuaScript.cpp:1580

// Declared here because the DWARF attributes these definitions (X/Server/LuaScript/LuaScript.cpp)
// to this header; the puzzle file LuaScriptDebug.cpp is their caller.
BOOL LuaScriptExecutingScriptIsConductor();  // LuaScript.cpp
ScriptInstanceID LuaScriptGetExecutingScriptInstance();  // LuaScript.cpp
int32 LuaScriptGetInstanceNumRet(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:2255
BOOL LuaScriptAddInstanceWaitListener(ScriptInstanceID idScriptInstance, int32 idWaitListener);  // LuaScript.cpp:2361
void LuaScriptWaitForAnyEvent(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:2380
void LuaScriptWaitForAllEvents(ScriptInstanceID idScriptInstance);  // LuaScript.cpp
void LuaScriptInstanceGetAttachmentIDAndType(ScriptInstanceID idScriptInstance, DataID & idAttachment, ScriptAttachmentType & eAttachmentType);  // LuaScript.cpp:3516
LPCSTR LuaScriptGetExecutingFunction();  // LuaScript.cpp
SNOName LuaScriptGetSNONameForLuaState(lua_State * ptLuaState);  // LuaScript.cpp
ScriptID LuaScriptGetInstanceParent(ScriptInstanceID idScriptInstance);  // LuaScript.cpp
LPCSTR LuaScriptGetInstanceEntryPoint(ScriptInstanceID idScriptInstance);  // LuaScript.cpp

void LuaScriptInstanceAddRefACD(ScriptInstanceID idScriptInstance, ACDID idACD);  // LuaScript.cpp:2556
void LuaScriptGetACDVariable(ACDID idACD, LPCSTR sVarName, lua_State * lDest);  // LuaScript.cpp:3728
void LuaScriptSetACDVariable(ACDID idACD, LPCSTR sVarName, lua_State * lSrc);  // LuaScript.cpp:3749
int32 CustomLuaLoader(const ScriptDefinition * ptScriptDef, lua_State * ptLuaState);  // LuaScript.cpp:4941
uint16 LuaScriptGetScriptAdventureParentIndex(ScriptInstanceID idScriptInstance);  // LuaScript.cpp:2726

SNO LuaScriptGetSNO(ScriptID idScript);
lua_State * LuaScriptGetLuaStateWithScriptID(ScriptID idScript);

struct WorldPlace;
struct ScriptEventParamList;
LPCSTR LuaScriptAttachmentTypeToString(ScriptAttachmentType eAttachType);
void LuaScriptPushExecutingScript(ScriptInstanceID idScriptInstance);
void LuaScriptPopExecutingScript();
POS LuaScriptGetFirstAttachment(ScriptID idScript);
void LuaScriptGetNextAttachment(POS & pos, ScriptAttachmentType & eType, DataID & idAttachment, BOOL & bFlaggedForDeletion);
void LuaScriptEnableListener(POS pos, int32 nListenerIdx, BOOL bEnabled);
BOOL LuaScriptIsListenerEnabled(POS pos, int32 nListenerIdx);
BOOL LuaScriptIsListenerInRange(POS pos, WorldPlace & tPlace, float flRange);
lua_State * LuaScriptTryGetLuaState(ScriptID idScript);
void LuaScriptCloseInstance(ScriptInstanceID idScriptInstance);
void LuaScriptInstanceNotifyEvent(ScriptInstanceID idScriptInstance, int32 idListener, int32 eEventType, ScriptEventParamList * ptEventDefParams);

SNO LuaScriptInstanceTryToGetSNO(ScriptInstanceID idScriptInstance);

uint16 LuaScriptGetExecutingScriptAdventureParentIndex();
