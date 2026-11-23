// Shared waypoint helpers used by the waypoint list and the waypoint map.

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIWaypointsCommon.h"

#include "../../../Common/CommonTypes.h"
#include "../../../Common/GameBalance.h"
#include "../../../Common/Definitions/QuestDefinition.h"
#include "../../../Sigma/DataStructures/XPtr64.h"
#include "../../../Sigma/SigmaTypes.h"

// Declared here because their headers are not adopted yet. Each comment names the file that
// defines the function or object.
#include "../../../Common/Quests/Quests.h"











static_assert(sizeof(QuestInstance) == 0x168, "QuestInstance");
// X/Common/Quests/Quests.cpp
QuestInstance * GetLeadingMainLineQuest();
// X/Common/Definitions/QuestDefinition.cpp
uint32 QuestStepUIDToIndex(const QuestDefinition * ptQuestDef, int32 nStepUID);

namespace UIWaypoints {

void GetCurrentQuestLevelAreas(int32 & nLevelAreaCount, SNO * snoWaypointLevelAreas)
{
    nLevelAreaCount = 0;

    QuestInstance * ptQuestInstance = GetLeadingMainLineQuest();
    if (ptQuestInstance != NULL) {
        ScopedQuestDefinitionLockR pdefQuest(ptQuestInstance->snoQuest);

        int32 nStepIndex = QuestStepUIDToIndex(pdefQuest, ptQuestInstance->nCurrentStepUID);
        if (nStepIndex != -1) {
            QuestStep & tStep = pdefQuest->arQuestSteps[nStepIndex];
            for (int32 i = 0; i < NUM_WAYPOINT_LEVEL_AREAS; i++) {
                if (tStep.snoWaypointLevelAreas[i] != -1) {
                    snoWaypointLevelAreas[nLevelAreaCount++] = tStep.snoWaypointLevelAreas[i];
                }
            }
        }
    }
}

}  // namespace UIWaypoints
