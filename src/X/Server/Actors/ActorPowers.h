// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Actors/ActorPowers.h
// Subset.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Powers/PowersStruct.h"

PowerID ActorPowersGetPrimary(ActorID idActor);  // ActorPowers.cpp:216

SNO ActorPowersGetPrimaryPower(ActorID idActor);  // ActorPowers.cpp:292
PowerID ActorPowersGetOwnerPower(ActorID idActor);  // ActorPowers.cpp:436

struct Actor;
typedef int32 SpellEffect;
void ActorPowersSetOwnerPower(ActorID idActor, PowerID idPower);

void ActorPowersCancelForDeath(ActorID idActor);  // ActorPowers.cpp:467
