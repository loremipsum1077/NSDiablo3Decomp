// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Powers/SpellEffect.h
// Subset.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Powers/PowersStruct.h"

class Target;
struct ImpactData;

void SpellEffect_GetProjectileImpactData(ACDID idACDProjectile, ACDID idACDIntersected, Target & tImpactTarget);  // SpellEffect.cpp:1351
BOOL SpellEffect_CalculatePierce(PowerID idPower, ACDID idACDProjectile, ACDID idACDIntersected);  // SpellEffect.cpp:1373
BOOL SpellEffect_ProjectileTryReflect(const PowerID idPower, ImpactData * ptImpact);  // SpellEffect.cpp:1576
void SpellEffectRunBeginEffect(PowerID idPower, void * pData);  // SpellEffect.cpp:19397
void SpellEffectRunInterruptedEffect(PowerID idPower, void * pData);  // SpellEffect.cpp:19427
void SpellEffect_InitializeForApp();  // SpellEffect.cpp:19452
SNO SpellEffect_GetWeaponPower(ACDID idACD);  // SpellEffect.cpp:11464
