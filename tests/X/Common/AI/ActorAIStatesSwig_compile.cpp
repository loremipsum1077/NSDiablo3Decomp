// Compile check for X/Common/AI/ActorAIStatesSwig.h (enumerators only, nothing to compare).
#include "X/Common/AI/ActorAIStatesSwig.h"

static_assert(sizeof(AIStateType) == 4, "AIStateType");
static_assert(sizeof(AIBehaviorType) == 4, "AIBehaviorType");
