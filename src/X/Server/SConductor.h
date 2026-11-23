// Adopted from the DWARF skeleton (build/dwarf/headers/X/Server/SConductor.h);
// hand-maintained from here on. Original file: X/Server/SConductor.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
// The original includes ../../Contrib/Contrib/lua/src/lstate.h here, but that header is not
// adopted in src/ (no copy anywhere under src/ or include/). Every use below is through a
// pointer (lua_State * ptConductorGlobalState), so a forward declaration is sufficient.
struct lua_State;
#include "../Common/CommonTypes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Common/ThreadGlobals.h"
#include "LuaScript/LuaScript.h"
#include "Powers/Powers.h"
#include "SGame.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "../Sigma/SigmaTypes.h"

typedef int32 ScriptInstanceID;

typedef int32 ScriptID;

typedef int32 ScriptEventListenerDefinitionID;

typedef int32 ScriptAttachmentType;

struct RepeatableScriptInstanceData {  // size 0xc
    uint32 nPhase;  // 0x00
    uint32 nSequence;  // 0x04
    ScriptInstanceID idScriptInstance;  // 0x08
};
static_assert(sizeof(RepeatableScriptInstanceData) == 0xc, "RepeatableScriptInstanceData");

struct ConductorInstance {  // size 0x4c8
    SNO snoConductor;  // 0x00
    ScriptID idGlobalScript;  // 0x04
    ScriptAttachmentType eAttachType;  // 0x08
    DataID idAttachment;  // 0x0c
    ScriptInstanceID arGlobalScripts[20];  // 0x10
    ScriptInstanceID arPhaseScripts[20];  // 0x60
    ScriptInstanceID idGlobalInitScript;  // 0xb0
    ScriptInstanceID idPhaseInitScript;  // 0xb4
    XPooledList<RepeatableScriptInstanceData, RepeatableScriptInstanceData> tRepeatableScriptInstanceDataList;  // 0xb8
    ACDID arRefACDID[128];  // 0xe0
    XPooledMap<ACDID, ACDID, int, int, 16> tMapRefACDs;  // 0x2e0
    BOOL bFailedToFindAndPurgeOldRefACDs;  // 0x388
    uint32 nCurrentPhase;  // 0x38c
    lua_State * ptConductorGlobalState;  // 0x390
    BOOL bFlaggedForDeletion;  // 0x398
    BOOL bFlaggedForAdvancement;  // 0x39c
    BOOL bFlaggedForPhaseSkip;  // 0x3a0
    char szPhaseSkipName[128];  // 0x3a4
    int32 nNumEventHandlers;  // 0x424
    ScriptEventListenerDefinitionID arGlobalEventHandlers[20];  // 0x428
    ScriptEventListenerDefinitionID arPhaseEventHandlers[20];  // 0x478
};
static_assert(sizeof(ConductorInstance) == 0x4c8, "ConductorInstance");

// ConductorGlobals (DWARF line 58, size 0x110) is not reproduced here: its first member is
// UpdateData, which no adopted src/ header declares (see the obligation ledger). Nothing in
// LuaScriptDebug.cpp refers to it, so leaving it out cannot affect this file's bytes.


// Declared here because SConductorFindConductorByScriptID is defined in X/Server/SConductor.cpp
// and called by LuaScriptDebug.cpp (_Z33SConductorFindConductorByScriptIDi).
ConductorInstance * SConductorFindConductorByScriptID(ScriptID idScript);

// Functions defined in this file (bodies are not in the DWARF):
//   line    45  ConductorInstance::ConductorInstance()  [inlined into 1 functions]
//   line    45  ConductorInstance::~ConductorInstance()  [0x7100966350]

// Variables defined in this file:
//   line    33  static const int32 CONDUCTOR_MAX_SCRIPT_GLOBAL_INSTANCES
//   line    34  static const int32 CONDUCTOR_MAX_SCRIPT_PHASE_INSTANCES
//   line    36  static const int32 CONDUCTOR_MAX_REFERENCED_ACDID

BOOL SConductorActive(SNO snoConductor);
void SConductorInstanceStart(SNO snoConductor);
