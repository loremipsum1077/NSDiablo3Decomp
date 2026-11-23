// Adopted from the DWARF reference; hand maintained.
// Original file: X/Server/Actors/ActorEffect.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"

typedef int32 DamageType;

typedef int32 RopePosition;

typedef int32 EffectType;

typedef int32 CosmeticItemType;


typedef int32 GBID;  // X/Common/GameBalance.h
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h
struct WorldPlace;
class PlayerList;

DamageType GetPredominantDamageType(const float * flDamageDone, BOOL bExcludePhysical);  // ActorEffect.cpp:44
void ACDPlayHitEffect(const ACDID idACD, const ACDID idACDAttacker, const DamageType eDamageType, const BOOL fCrit);  // ActorEffect.cpp:71
void ACDPlayHitEffectOverride(const ACDID idACD, const ACDID idACDAttacker, const SNO snoEffectGroup);  // ActorEffect.cpp:89
void ACDPlayEffectForPlayer(const ACDID idACDPlayer, const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter);  // ActorEffect.cpp:137
void ACDPlayEffectForPlayers(const PlayerList & listDestPlayers, const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter);  // ActorEffect.cpp:150
void ACDPlayEffectChangeActor(const ACDID idACD, const SNO snoActor);  // ActorEffect.cpp:202
void ACDPlayEffectRopeACDToACD(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const ACDID idACDTo, const RopePosition ePositionTo, const BOOL bIgnoreOwnerAlpha);  // ActorEffect.cpp:242
void ACDPlayEffectRopeACDToPlace(const SNO snoRope, const ACDID idACDFrom, const RopePosition ePositionFrom, const WorldPlace & tPlaceTo, const BOOL bIgnoreOwnerAlpha);  // ActorEffect.cpp:265

void ACDActivateCosmeticEffect(ACDID idACD, GBID gbidItem, CosmeticItemType eType);  // ActorEffect.cpp:293
void ACDPlayEffectGroupACDToACD(const SNO snoEffectGroup, const ACDID idACDFrom, const ACDID idACDTo);  // ActorEffect.cpp:217
void ACDPlayEffect(const ACDID idACD, const EffectType eEffectType, const int32 nEffectParameter,
                   const ACDID idACDOwner);  // ActorEffect.cpp:104
