// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIHirelingCommon.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 HirelingClass;

class ActorCommonData;

// UIHirelingCommon.cpp:53.  The reference header records only the HirelingClass
// typedef; the declaration is kept here so the file's callers can reach it.
namespace UIHireling {

float GetBaseDamageScalarForClass(const ActorCommonData & tACDHireling);

}  // namespace UIHireling
