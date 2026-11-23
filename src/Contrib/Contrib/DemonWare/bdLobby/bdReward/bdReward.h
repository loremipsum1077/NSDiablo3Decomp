// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdReward/bdReward.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdRewardEvent;
class bdRewardGameEvent;
class bdRewardUserGameEventsList;
class bdRulesDefinition;
class bdSessionID;
class bdUserAccountID;
class bdUserAchievement;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdReward {  // size 0x18
public:
    BD_CLASS_ALLOCATOR(bdReward);
    bdReward(bdRemoteTaskManager *const, const bdNChar8 *);
    ~bdReward();
    bdRemoteTaskRef incrementTime(const bdUInt32, const bdUInt32);
    bdRemoteTaskRef claimRewardRoll();
    bdRemoteTaskRef claimClientAchievements(const bdUserAchievement *, const bdUInt16, const bdSessionID);
    bdRemoteTaskRef reportRewardEvents(const bdRewardEvent **, const bdUInt16);
    bdRemoteTaskRef reportRewardEventsSync(const bdRewardEvent **, const bdUInt16, bdRewardEvent *, const bdUInt16);
    bdRemoteTaskRef reportRewardGameEvents(const bdRewardGameEvent **, const bdUInt16);
    bdRemoteTaskRef reportRewardGameEventsForUser(const bdUserAccountID &, const bdRewardGameEvent **, const bdUInt16);
    bdRemoteTaskRef reportRewardGameEventsForUsers(const bdRewardUserGameEventsList *, const bdUInt32);
    bdRemoteTaskRef getRulesDefinition(bdRulesDefinition *, const bdNChar8 *, const bdUInt16, const bdUInt16);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdNChar8 m_context[16];  // 0x08
};
static_assert(sizeof(bdReward) == 0x18, "bdReward");


// Functions defined in this file (bodies are not in the DWARF):
//   line    41  void bdReward::operator delete(void * p)  [inlined into 1 functions]
//   line    41  void * bdReward::operator new(size_t nbytes)  [inline]
