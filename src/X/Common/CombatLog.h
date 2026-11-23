// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/CombatLog.h
// Subset: the event enum and the functions CombatLog.cpp defines; CombatLog::Message is not
// adopted yet. SGameStats.cpp and PowerAPI.cpp still declare their own local subsets of this
// namespace (with invented enumerator names) and do not include this header.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"

class CRefString;
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h

namespace CombatLog {
enum {
    LOG_EVENT_INVALID = -1,
    LOG_EVENT_POWER_USE = 0,
    LOG_EVENT_DODGE = 1,
    LOG_EVENT_HIT_ROLL = 2,
    LOG_EVENT_TARGET_INVULNERABLE = 3,
    LOG_EVENT_CRITICAL_HIT = 4,
    LOG_EVENT_DAMAGE_REDUCTION = 5,
    LOG_EVENT_BLOCK_ROLL = 6,
    LOG_EVENT_RESISTANCE_APPLIED = 7,
    LOG_EVENT_DAMAGE_DONE = 8,
    LOG_EVENT_DEATH = 9,
    LOG_EVENT_HEALING_DONE = 10,
    LOG_EVENT_ITEM_PROC_ROLL = 11,
    LOG_EVENT_CRUSHING_BLOW = 12,
};

typedef int32 LogEventType;

void InitializeForGame();
void DisposeForGame();
void sGetActorName(CRefString & sName, ACDID idACD);
void LogString(CRefString & sLogItem);
void LogPowerUse(ACDID idACDCaster, SNO snoPower, ACDID idACDTarget);
void LogDamageReduction(ACDID idACDTarget, uint32 eDamageType, float flOriginalDamage, float flReducedDamage);
void LogDodge(ACDID idACDTarget, ACDID idACDCaster, SNO snoPower, float flDodge, float flDodgeRoll);
void LogHitRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flHitChance, float flHitRoll);
void LogTargetInvulernable(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower);
void LogCriticalHit(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flCritChance, float flCritRoll);
void LogCrushingBlow(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flProcChance, float flRoll);
void LogBlockRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flBlockChance, float flBlockRoll);
void LogItemProcRoll(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flProcChance, float flProcRoll);
void LogResistDamage(ACDID idACDCaster, ACDID idACDTarget, uint32 eDamageType, float flDamageTakenPercent, float flDamage);
void LogDamageDone(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flDamage, uint32 bCrit, uint32 eDamageType, int32 eDeliveryMechanism);
void LogHealingDone(ACDID idACDCaster, ACDID idACDTarget, SNO snoPower, float flHealing, uint32 eHealType);
void LogDeath(ACDID idACDKiller, ACDID idACDVictim);
void LogEvent(LogEventType eType, ACDID idACD1, ACDID idACD2, SNO snoData, float flParam1, float flParam2, uint32 dwParam1);
}  // namespace CombatLog
