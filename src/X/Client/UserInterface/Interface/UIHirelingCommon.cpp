// X/Client/UserInterface/Interface/UIHirelingCommon.cpp
//
// Helpers the client UI shares for the hireling (follower) classes.  Given a
// follower's ACD this resolves the base-attack power of its class and asks
// PowersCommon for the damage scalar that power carries; anything that is not
// one of the three followers has no scalar, so 1.0 is returned unchanged.

#include "UIHirelingCommon.h"

#include "../../../Common/ActorCommonData.h"
#include "../../../Common/Definitions/PowerDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/Powers/PowersCommon.h"

namespace UIHireling {

// Follower class ids are 1..3.  Each has a base-attack power, and the three sit
// consecutively in the GlobalSNO powers starting at the templar's melee instant.
float GetBaseDamageScalarForClass(const ActorCommonData & tACDHireling)
{
    HirelingClass eClass = ACDGetHirelingClass(tACDHireling);

    if (eClass >= 1 && eClass <= 3)
    {
        SNO snoPowerBaseAttack = GlobalSNOGet(GLOBALSNO_POWERS_TEMPLAR_MELEE_INSTANT + (eClass - 1));

        return PowerGetBaseDamageScalar(tACDHireling, AutoPowerDefinitionR(snoPowerBaseAttack));
    }

    return 1.0f;
}

}  // namespace UIHireling
