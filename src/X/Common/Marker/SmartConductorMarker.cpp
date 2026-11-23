// Original file: X/Common/Marker/SmartConductorMarker.cpp

#include "SmartConductorMarker.h"
#include "../GlobalSNO.h"

SNO ConductorMarker::GetActorForEdit(SNO snoSource)
{
    return GlobalSNOGet(GLOBALSNO_ACTOR_CONDUCTOR);
}
