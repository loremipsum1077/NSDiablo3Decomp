// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAchievementsEngine/bdAchievementsEngineService.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdCore/bdReference/bdReference.h"
#include "../bdRemoteTaskManager/bdRemoteTask.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdActivateAchievementRequest;
class bdClaimAchievementRequest;
class bdDeactivateAchievementRequest;
class bdGetAchievementDefinitionsRequest;
class bdGetAchievementDefinitionsResponse;
class bdGetAchievementStatesRequest;
class bdGetAchievementStatesResponse;
class bdRemoteTaskManager;
class bdReportEventsRequest;
class bdReportUserEventsRequest;
class bdStructBufferSerializable;

typedef bdReference<bdRemoteTask> bdRemoteTaskRef;

class bdAchievementsEngineService {  // size 0x8
public:
    BD_CLASS_ALLOCATOR(bdAchievementsEngineService);
    bdAchievementsEngineService(bdRemoteTaskManager *const);
    bdRemoteTaskRef reportEvents(const bdReportEventsRequest &);
    bdRemoteTaskRef reportUserEvents(const bdReportUserEventsRequest &);
    bdRemoteTaskRef getAchievementStates(const bdGetAchievementStatesRequest &, bdGetAchievementStatesResponse *);
    bdRemoteTaskRef getAchievementDefinitions(const bdGetAchievementDefinitionsRequest &, bdGetAchievementDefinitionsResponse *);
    bdRemoteTaskRef activateAchievement(const bdActivateAchievementRequest &);
    bdRemoteTaskRef deactivateAchievement(const bdDeactivateAchievementRequest &);
    bdRemoteTaskRef claimAchievement(const bdClaimAchievementRequest &);
protected:
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdRemoteTaskRef startTask(const bdNChar8 * taskName, bdUByte8 taskID, const bdStructBufferSerializable & request, bdStructBufferSerializable * response);
};
static_assert(sizeof(bdAchievementsEngineService) == 0x8, "bdAchievementsEngineService");


// Functions defined in this file (bodies are not in the DWARF):
//   line    38  void bdAchievementsEngineService::operator delete(void * p)  [inlined into 1 functions]
//   line    38  void * bdAchievementsEngineService::operator new(size_t nbytes)  [inline]
