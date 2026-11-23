// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/Act.h
// Subset: the Act type only.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 Act;

struct ActDefinition;
int32 ActGetNumQuests(const ActDefinition * ptActDef);  // Act.cpp:452
