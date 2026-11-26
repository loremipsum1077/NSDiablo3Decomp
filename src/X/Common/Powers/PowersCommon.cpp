// Reconstructed from the original source ownership, DWARF inline trees and code.
#include "PowersCommon.h"
#include "../TagMaps/PowerTags.h"
#include "../TagMaps/Tags.h"
#include "../GameTemplate.h"
#include "../ThreadGlobals.h"
#include "../GlobalSNO.h"
#include "../Player.h"
#include "SpellEffectCommon.h"
#include "PowerAPISwig.h"
#include "../Target.h"
#include "../Attributes/FastAttribute.h"
#include "../Definitions/HeroDefinition.h"
#include "../Definitions/GlobalDefinition.h"
#include "../Definitions/SkillKitDefinition.h"
#include "../GameCommonData.h"
#include "../Monsters.h"
#include "../Gizmo.h"
#include "../WorldPlace.h"
#include "../SceneCommon.h"
#include "../Nav/NavigationMesh.h"
#include "../Definitions/StringListDefinition.h"
#include "../SNO/SNOFiles.h"
#include "../../Server/Actors/Actors.h"
#include <float.h>
#include "../Items.h"
#include "../AnimPlayer.h"
#include "../TagMaps/ActorTags.h"
#include "../TagMaps/AnimTagsSwig.h"
#include "../ActorInventory.h"
#include "../ActorCommonDataSwig.h"
#include "../../Server/Powers/Powers.h"

// X/Client/CGame.h:440 (client side of the shared cooldown checks).
uint32 CGameGetPredictedServerTime();

// ActorCommonData.cpp:4668 and NavigationMesh.cpp:134; their owner headers do not declare them yet.
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace);
BOOL NavMeshHandleCanEnter(const NavMeshHandle & tNavMeshHandle, const PassabilityType ePassabilityType);
BOOL PowerIsFacing(ACDID idACD, SNO snoPower, const Target * ptTarget);  // PowersCommon.cpp (defined later in this file)
BOOL WorldCollProjectileLineOfSight(ACDID idACD, const Target & tTarget);  // X/Common/WorldColl.cpp
BOOL PowerRollToTargetNavPlace(ACDID idACD, const Target & tTarget, PassabilityType ePassabilityType, BOOL bIgnoreCollision, NavPlace & tNavPlace);  // PowersCommon.cpp (defined later in this file)

uint32 PowerDetermineAnimation(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target & tTarget, BOOL bUseAlternateAnim);  // PowersCommon.cpp:8013
uint32 AnimPlayerGetCurrentAnimTag(ActorCommonData & tACD);  // X/Common/AnimPlayer.cpp



BOOL ItemIsBroken(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsGlyph(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsEquippedButDisabled(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
BOOL ItemIsTwoHanded(const ActorCommonData & tACDItem);  // X/Common/Items.cpp
ACDID ACDInventoryGetSecondaryWeaponOrShield(ACDID idACD);  // X/Common/ActorInventory.cpp

ActorID BrainAPIGetTauntTarget(ActorID idActor);  // X/Server/AI/BrainAPI.cpp
WorldDistance PowerGetDistanceViaWorldPlace(const WorldPlace & placeStart, const WorldPlace & placeTarget, WorldDistance wdStartHeight, const WorldDistance wdStartRadius, WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach);  // PowersCommon.cpp:8290
BOOL PowerIsCloseEnoughToPlace(ACDID idACD, const WorldPlace & placeSource, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance wdRange, WorldDistance wdLongReach, float * pflDistance, BOOL bIgnoreRange);  // PowersCommon.cpp:4904

// Tags.cpp:1287; owner header currently carries the other typed lookup declarations.
GBID TagMapGetGBID(const CTagMap * pTagMap, uint32 dwTag);
// Original owners: Tags.cpp:821 and Tags.cpp:1162, GameBalanceDefinition.cpp:4105.
TagVariantType TagGetVariantType(const CTagMap * pTagMap, const uint32 dwTag);
uint8 * TagMapGetBBEFormula(const CTagMap * pTagMap, uint32 dwTag, uint32 & dwSize);
float PowerGetFormulaValueAtLevel(AttribGroupID idFastAttrib, const ActorCommonData * ptACD,
    SNO snoPower, uint8 * pbFormula, int32 nFormulaSize, int32 nLevel);

// Original line 74.
BOOL PowerIsCompletedWhenWalkingStops(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_COMPLETED_WHEN_WALKING_STOPS);
}


// Original line 83.
BOOL PowerCanSteer(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAN_STEER);
}


// Original line 100.
BOOL PowerIsTranslate(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_TRANSLATE);
}


// Original line 108.
BOOL PowerChecksVerticalMovement(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return FALSE;
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CHECKS_VERTICAL_MOVEMENT);
}

// Original line 124.
BOOL PowerIsOffensive(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_OFFENSIVE);
}


// Original line 133.
BOOL PowerIsOffensive(SNO snoPower) {
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return PowerIsOffensive(pdefPower);
}


// Original line 143.
BOOL PowerIsPassive(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_PASSIVE);
}


// Original line 152.
BOOL PowerIsPassive(SNO snoPower) {
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return PowerIsPassive(pdefPower);
}


// Original line 179.
BOOL PowerCanPathDuringWalk(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAN_PATH_DURING_WALK);
}


// Original line 187.
BOOL PowerIsKnockbackMovementPower(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_KNOCKBACK_MOVEMENT);
}


// Original line 195.
ChargeSteeringType PowerGetChargeSteeringType(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return CHARGE_STEERING_NONE;
    return PowerGeneralTagGetInt(pdefPower, TAG_POWER_USE_CHARGE_STEERING);
}

// Original line 209.
PowerHotbarExclusiveType PowerGetHotbarExclusiveType(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return POWER_HOTBAR_EXCLUSIVE_TYPE_NONE;
    return PowerGeneralTagGetInt(pdefPower, TAG_POWER_HOTBAR_EXCLUSIVE_TYPE);
}

// Original line 223.
BOOL PowerPreplaysAnimation(AutoPowerDefinitionR pdefPower) {
    return !PowerGeneralTagGetBool(pdefPower, TAG_POWER_DOESNT_PREPLAY_ANIMATION);
}


// Original line 231.
BOOL PowerIsComboPower(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_COMBO_POWER);
}


// Original line 239.
BOOL PowerIsComboPower(SNO snoPower) {
    return PowerIsComboPower(AutoPowerDefinitionR(snoPower));
}


// Original line 247.
BOOL PowerRefreshesComboLevel(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_REFRESHES_COMBO_LEVEL);
}


// Original line 255.
BOOL PowerUsesMainHandOnly(AutoPowerDefinitionR pdefPower, int32 nComboLevel) {
    BOOL bUsesMainHandOnly = PowerGeneralTagGetBool(pdefPower, TAG_POWER_USES_MAINHAND_ONLY);
    if (PowerIsComboPower(pdefPower) && (uint32)nComboLevel < 3) {
        uint32 uTag;
        switch (nComboLevel) {
        case 0: uTag = TAG_POWER_USES_MAINHAND_ONLY_COMBO1; break;
        case 1: uTag = TAG_POWER_USES_MAINHAND_ONLY_COMBO2; break;
        case 2: uTag = TAG_POWER_USES_MAINHAND_ONLY_COMBO3; break;
        default: return bUsesMainHandOnly;
        }
        bUsesMainHandOnly = bUsesMainHandOnly || PowerGeneralTagGetBool(pdefPower, uTag);
    }
    return bUsesMainHandOnly;
}

// Original line 286.
BOOL PowerHasScaledAnimationTiming(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_SCALED_ANIMATION_TIMING);
}


// Original line 294.
BOOL PowerHasInfiniteAnimationTiming(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    if (PowerGetTemplate(pdefPower, ACDID_INVALID) == LOOPING_ANIMATION_POWER)
        return TRUE;
    if (PowerIsGizmo(pdefPower))
        return TRUE;
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_INFINITE_ANIMATION_TIMING, -1, -1, 0) > 0.000001f;
}

// Original line 313.
ContactFrameType PowerGetContactFrameType(AutoPowerDefinitionR pdefPower, int32 nContactFrame) {
    return PowerContactTagGetInt(pdefPower, TAG_POWER_CONTACT_FRAME_IN_COOLDOWN, nContactFrame);
}

// Original line 322.
PlayerClass PowerGetClassRestriction(AutoPowerDefinitionR pdefPower) {
    GBID gbidPlayerClass = PowerGeneralTagGetInt(pdefPower, TAG_POWER_CLASS_RESTRICTION);
    return PlayerClassFromGBID(gbidPlayerClass);
}

// Original line 331.
BOOL PowerRequiresActorTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_REQUIRES_ACTOR_TARGET, -1, -1, 0) > 0.000001f;
}

// Original line 340.
BOOL PowerRequiresTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    BOOL bReturn = PowerRequiresActorTarget(tACD, pdefPower) ||
        PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_REQUIRES_TARGET, -1, -1, 0) > 0.000001f;
    return bReturn;
}

// Original line 354.
SNO PowerConsumedItem(AutoPowerDefinitionR pdefPower) {
    SNO snoActorItem = PowerGeneralTagGetSNO(pdefPower, TAG_POWER_CONSUMES_ITEM);
    return snoActorItem;
}

// Original line 363.
BOOL PowerWalksCloserIfOutOfRange(AutoPowerDefinitionR pdefPower) {
    return !PowerGeneralTagGetBool(pdefPower, TAG_POWER_DONT_WALK_CLOSER_IF_OUT_OF_RANGE);
}


// Original line 372.
BOOL PowerIsAimedAtGround(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_AIMED_AT_GROUND);
}


// Original line 381.
BOOL PowerTargetGroundOnly(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_TARGET_GROUND_ONLY);
}


// Original line 390.
BOOL PowerTargetNavMeshOnly(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_TARGET_NAV_MESH_ONLY);
}


// Original line 398.
BOOL PowerTargetPathableOnly(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_TARGET_PATHABLE_ONLY);
}


// Original line 407.
BOOL PowerTargetsContactPlaneOnly(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_TARGET_CONTACT_PLANE_ONLY);
}


// Original line 416.
BOOL PowerPlayerCanUseWhenDead(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAN_USE_WHEN_DEAD);
}


// Original line 424.
GBID PowerGeneralTagGetGBID(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TagMapGetGBID(pdefPower->tPower.hPVPGeneralTagMap, dwTag);
    return TagMapGetGBID(pdefPower->tPower.hGeneralTagMap, dwTag);
}


// Original line 442.
BOOL PowerGeneralTagGetBool(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TagMapGetInt(pdefPower->tPower.hPVPGeneralTagMap, dwTag) != 0;
    return TagMapGetInt(pdefPower->tPower.hGeneralTagMap, dwTag) != 0;
}


// Original line 459.
int32 PowerGeneralTagGetInt(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TagMapGetInt(pdefPower->tPower.hPVPGeneralTagMap, dwTag);
    return TagMapGetInt(pdefPower->tPower.hGeneralTagMap, dwTag);
}


// Original line 476.
float PowerGeneralTagGetFloat(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TagMapGetFloat(pdefPower->tPower.hPVPGeneralTagMap, dwTag);
    return TagMapGetFloat(pdefPower->tPower.hGeneralTagMap, dwTag);
}


// Original line 493.
SNO PowerGeneralTagGetSNO(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TagMapGetSNO(pdefPower->tPower.hPVPGeneralTagMap, dwTag);
    return TagMapGetSNO(pdefPower->tPower.hGeneralTagMap, dwTag);
}


// Original line 510.
BOOL PowerContactTagGetBool(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame) {
    if (TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) && GameTypeIsPvP())
        return TagMapGetInt(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) != 0;
    return TagMapGetInt(pdefPower->tPower.hContactTagMap[nContactFrame], dwTag) != 0;
}

// Original line 529.
int32 PowerContactTagGetInt(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame) {
    if (TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) && GameTypeIsPvP())
        return TagMapGetInt(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag);
    return TagMapGetInt(pdefPower->tPower.hContactTagMap[nContactFrame], dwTag);
}

// Original line 548.
float PowerContactTagGetFloat(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame) {
    if (TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) && GameTypeIsPvP())
        return TagMapGetFloat(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag);
    return TagMapGetFloat(pdefPower->tPower.hContactTagMap[nContactFrame], dwTag);
}

// Original line 567.
SNO PowerContactTagGetSNO(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame) {
    if (TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) && GameTypeIsPvP())
        return TagMapGetSNO(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag);
    return TagMapGetSNO(pdefPower->tPower.hContactTagMap[nContactFrame], dwTag);
}

// Original line 605.
BOOL PowerContactTagExists(AutoPowerDefinitionR pdefPower, uint32 dwTag, uint32 nContactFrame) {
    if (TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwTag) && GameTypeIsPvP())
        return TRUE;
    return TagMapTagExists(pdefPower->tPower.hContactTagMap[nContactFrame], dwTag);
}

// Original line 623.
BOOL PowerGeneralTagExists(AutoPowerDefinitionR pdefPower, uint32 dwTag) {
    if (TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwTag) && GameTypeIsPvP())
        return TRUE;
    return TagMapTagExists(pdefPower->tPower.hGeneralTagMap, dwTag);
}

// Original line 639.
BOOL PowerIsGizmo(AutoPowerDefinitionR pdefPower) {
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, ACDID_INVALID);
    return eTemplate == GIZMO_POWER;
}

// Original line 649.
CRefString PowerTrace(SNO snoPower) {
    if (snoPower != SNO_NONE)
        return SNOToString(POWER_SNO, snoPower, FALSE);
    return CRefString("None");
}

// Original line 663.
CRefString PowerGetNameLabel(SNO snoPower) {
    return PowerTrace(snoPower) + CRefString("_name");
}

// Original line 671.
BOOL PowerIsPrimary(AutoPowerDefinitionR pdefPower, ACDID idACD) {
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, idACD);
    switch (eTemplate) {
    case WALK_POWER:
    case ANIMATION_POWER:
    case SPECIALWALK_POWER:
    case LOOPING_ANIMATION_POWER:
    case ATTACKMOVE_POWER:
        return TRUE;
    case GIZMO_POWER:
        return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_PRIMARY);
    default:
        return FALSE;
    }
}

// Original line 698.
BOOL PowerIsPrimary(SNO snoPower, ACDID idACD) {
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return PowerIsPrimary(pdefPower, idACD);
}

// Original line 709.
BOOL PowerIsPunch(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_PUNCH);
}


// Original line 718.
BOOL PowerIsDisplayed(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_DISPLAYED);
}


// Original line 735.
BOOL PowerIsBasicAttack(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_BASIC_ATTACK);
}


// Original line 744.
BOOL PowerIsToggleable(AutoPowerDefinitionR pdefPower) {
    BOOL bReturn = FALSE;
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower != SNO_NONE)
        bReturn = PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_TOGGLEABLE);
    return bReturn;
}

// Original line 759.
BOOL PowerShouldRollToDestination(AutoPowerDefinitionR pdefPower) {
    if (PowerCanMoveForwardBeforeContactFrame(pdefPower))
        return TRUE;
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_ROLL_TO_DESTINATION);
}

// Original line 775.
BOOL PowerIsChannelled(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_IS_CHANNELLED, -1, -1, 0) > 0.000001f;
}

// Original line 795.
BOOL PowerLocksActorsWhileSweeping(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_LOCKS_ACTORS_WHILE_SWEEPING) ||
        PowerStickyTargetsActorsWhileSweeping(pdefPower);
}

// Original line 805.
BOOL PowerStickyTargetsActorsWhileSweeping(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_STICKY_TARGETS_ACTORS_WHILE_SWEEPING);
}


// Original line 814.
float PowerGetStickyTargetRadius(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    if (!PowerStickyTargetsActorsWhileSweeping(pdefPower))
        return 0.0f;
    return PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_STICKY_TARGET_RADIUS, -1, -1, 0);
}

// Original line 828.
BOOL PowerIsEmote(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_EMOTE);
}


// Original line 846.
BOOL PowerAffectedByDualWield(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_AFFECTED_BY_DUAL_WIELD);
}


// Original line 862.
static int32 sgarRuneDeathTypeTags[5] = {
    TAG_POWER_SPECIAL_DEATH_TYPE_RUNE_A,
    TAG_POWER_SPECIAL_DEATH_TYPE_RUNE_B,
    TAG_POWER_SPECIAL_DEATH_TYPE_RUNE_C,
    TAG_POWER_SPECIAL_DEATH_TYPE_RUNE_D,
    TAG_POWER_SPECIAL_DEATH_TYPE_RUNE_E,
};

static int32 sgarRuneDeathChanceTags[5] = {
    TAG_POWER_SPECIAL_DEATH_CHANCE_RUNE_A,
    TAG_POWER_SPECIAL_DEATH_CHANCE_RUNE_B,
    TAG_POWER_SPECIAL_DEATH_CHANCE_RUNE_C,
    TAG_POWER_SPECIAL_DEATH_CHANCE_RUNE_D,
    TAG_POWER_SPECIAL_DEATH_CHANCE_RUNE_E,
};

// Original line 882.
void PowerGetSpecialDeathTags(AutoPowerDefinitionR pdefPower, RuneType eRune, int32 & nTagDeathType, int32 & nTagChance) {
    nTagDeathType = TAG_POWER_SPECIAL_DEATH_TYPE;
    nTagChance = TAG_POWER_SPECIAL_DEATH_CHANCE;
    if (eRune >= 0 && eRune < 5) {
        int32 nTagDeathTypeRune = sgarRuneDeathTypeTags[eRune];
        SpecialDeathType eDeathTypeRune = PowerGeneralTagGetInt(pdefPower, nTagDeathTypeRune);
        if (eDeathTypeRune != SPECIALDEATH_NONE) {
            nTagDeathType = nTagDeathTypeRune;
            nTagChance = sgarRuneDeathChanceTags[eRune];
        }
    }
}

// Original line 905.
SpecialDeathType PowerGetSpecialDeathType(AutoPowerDefinitionR pdefPower, RuneType eRune) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return SPECIALDEATH_NONE;
    int32 nTagDeathType;
    int32 nTagChance;
    PowerGetSpecialDeathTags(pdefPower, eRune, nTagDeathType, nTagChance);
    return PowerGeneralTagGetInt(pdefPower, nTagDeathType);
}

// Original line 925.
BOOL PowerContactFreezesFacing(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CONTACT_FREEZES_FACING);
}


// Original line 934.
BOOL PowerSnapsToFacing(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_SNAPS_TO_FACING);
}


// Original line 943.
BOOL PowerClientControlsFacing(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CLIENT_CONTROLS_FACING);
}


// Original line 986.
BOOL PowerIsValidForSlot(AutoPowerDefinitionR pdefPower, const int32 nSlot) {
    return TRUE;
}

// Original line 1047.
BOOL PowerIgnoresRangeOnShiftClick(AutoPowerDefinitionR pdefPower) {
    return TRUE;
}

// Original line 1061.
BOOL PowerClipsTargetToAttackRadius(AutoPowerDefinitionR pdefPower) {
    if (PowerCanMoveForwardBeforeContactFrame(pdefPower))
        return FALSE;
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CLIPS_TARGET_TO_ATTACK_RADIUS);
}

// Original line 1077.
BOOL PowerChecksPathAndCliptoPathableAttackRadius(SNO snoPower) {
    return PowerGeneralTagGetBool(AutoPowerDefinitionR(snoPower), TAG_POWER_CHECK_PATH_THEN_CLIP_TO_PATHABLE_ATTACK_RADIUS);
}

// Original line 1085.
BOOL PowerExtendsTargetToAttackRadius(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_EXTENDS_TARGET_TO_ATTACK_RADIUS);
}


// Original line 1094.
BOOL PowerWantsActorTarget(AutoPowerDefinitionR pdefPower) {
    if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_DOESNT_REQUIRE_ACTOR_TARGET))
        return FALSE;
    BOOL bReturn = PowerCastTargetNeutral(pdefPower) || PowerCastTargetEnemies(pdefPower) ||
        PowerCastTargetAllies(pdefPower);
    return bReturn;
}

// Original line 1117.
BOOL PowerCausesClosingCooldown(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAUSES_CLOSING_COOLDOWN);
}


// Original line 1134.
BOOL PowerNoInterruptTimer(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_NO_INTERRUPT_TIMER);
}


// Original line 1143.
PowerStartLOSType PowerGetStartLOS(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetInt(pdefPower, TAG_POWER_LOS_CHECK);
}

// Original line 1152.
BOOL PowerCastTargetAllies(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_ALLIES);
}


// Original line 1161.
BOOL PowerCastTargetEnemies(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_ENEMIES);
}


// Original line 1170.
BOOL PowerCastTargetNeutral(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_NEUTRAL);
}


// Original line 1188.
BOOL PowerCastTargetNecroCorpsesOnly(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NECROMANCER_CORPSE_FREE_CASTING)))
        return FALSE;
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CAST_TARGET_NECRO_CORPSES_ONLY, -1, -1, 0) > 0.000001f;
}

// Original line 1202.
BOOL PowerNoAffectedACD(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_NO_AFFECTED_ACD);
}


// Original line 1211.
BOOL PowerTracksAffectedACDInstanceWide(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_TRACKS_AFFECTED_ACD_INSTANCE_WIDE);
}


// Original line 1219.
BOOL PowerIsCancellable(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_CANCELLABLE);
}


// Original line 1227.
BOOL PowerIsCancellableByAnyPower(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_CANCELLABLE_BY_ANY_POWER);
}


// Original line 1235.
BOOL PowerIsCancellableByWalking(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_CANCELLABLE_BY_WALKING);
}


// Original line 1260.
BOOL PowerCancelsOtherPowers(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CANCELS_OTHER_POWERS);
}


// Original line 1300.
BOOL PowerIsNoRecastUntilLastAnimFrame(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_NO_RECAST_UNTIL_LAST_ANIM_FRAME);
}


// Original line 1308.
BOOL PowerCancelsOnInterruptFrame(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_CANCELS_AT_INTERRUPT_FRAME);
}


// Original line 1316.
BOOL PowerSnapsToGround(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_SNAPS_TO_GROUND);
}


// Original line 1324.
int32 PowerGetAutoPurchaseLevel(AutoPowerDefinitionR pdefPower) {
    int32 nAutoPurchaseLevel = 0;
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower != SNO_NONE)
        nAutoPurchaseLevel = PowerGeneralTagGetInt(pdefPower, TAG_POWER_AUTO_PURCHASE_LEVEL);
    return nAutoPurchaseLevel;
}

// Original line 1340.
BOOL PowerFacesTargetFacing(AutoPowerDefinitionR pdefPower, ACDID idACDTarget) {
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, ACDID_INVALID);
    if (idACDTarget != ACDID_INVALID && eTemplate == GIZMO_POWER) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget)
            return FALSE;
    }
    return FALSE;
}

// Original line 1361.
BOOL PowerHasBuffIcon(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_ICON; break;
    case 1: nTag = TAG_POWER_BUFF_1_ICON; break;
    case 2: nTag = TAG_POWER_BUFF_2_ICON; break;
    case 3: nTag = TAG_POWER_BUFF_3_ICON; break;
    case 4: nTag = TAG_POWER_BUFF_4_ICON; break;
    case 5: nTag = TAG_POWER_BUFF_5_ICON; break;
    case 6: nTag = TAG_POWER_BUFF_6_ICON; break;
    case 7: nTag = TAG_POWER_BUFF_7_ICON; break;
    case 8: nTag = TAG_POWER_BUFF_8_ICON; break;
    case 9: nTag = TAG_POWER_BUFF_9_ICON; break;
    case 10: nTag = TAG_POWER_BUFF_10_ICON; break;
    case 11: nTag = TAG_POWER_BUFF_11_ICON; break;
    case 12: nTag = TAG_POWER_BUFF_12_ICON; break;
    case 13: nTag = TAG_POWER_BUFF_13_ICON; break;
    case 14: nTag = TAG_POWER_BUFF_14_ICON; break;
    case 15: nTag = TAG_POWER_BUFF_15_ICON; break;
    case 16: nTag = TAG_POWER_BUFF_16_ICON; break;
    case 17: nTag = TAG_POWER_BUFF_17_ICON; break;
    case 18: nTag = TAG_POWER_BUFF_18_ICON; break;
    case 19: nTag = TAG_POWER_BUFF_19_ICON; break;
    case 20: nTag = TAG_POWER_BUFF_20_ICON; break;
    case 21: nTag = TAG_POWER_BUFF_21_ICON; break;
    case 22: nTag = TAG_POWER_BUFF_22_ICON; break;
    case 23: nTag = TAG_POWER_BUFF_23_ICON; break;
    case 24: nTag = TAG_POWER_BUFF_24_ICON; break;
    case 25: nTag = TAG_POWER_BUFF_25_ICON; break;
    case 26: nTag = TAG_POWER_BUFF_26_ICON; break;
    case 27: nTag = TAG_POWER_BUFF_27_ICON; break;
    case 28: nTag = TAG_POWER_BUFF_28_ICON; break;
    case 29: nTag = TAG_POWER_BUFF_29_ICON; break;
    case 30: nTag = TAG_POWER_BUFF_30_ICON; break;
    case 31: nTag = TAG_POWER_BUFF_31_ICON; break;
    default: return FALSE;
    }
    return PowerGeneralTagGetBool(pdefPower, nTag);
}

// Original line 1415.
BOOL PowerBuffIsDisplayed(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_IS_DISPLAYED; break;
    case 1: nTag = TAG_POWER_BUFF_1_IS_DISPLAYED; break;
    case 2: nTag = TAG_POWER_BUFF_2_IS_DISPLAYED; break;
    case 3: nTag = TAG_POWER_BUFF_3_IS_DISPLAYED; break;
    case 4: nTag = TAG_POWER_BUFF_4_IS_DISPLAYED; break;
    case 5: nTag = TAG_POWER_BUFF_5_IS_DISPLAYED; break;
    case 6: nTag = TAG_POWER_BUFF_6_IS_DISPLAYED; break;
    case 7: nTag = TAG_POWER_BUFF_7_IS_DISPLAYED; break;
    case 8: nTag = TAG_POWER_BUFF_8_IS_DISPLAYED; break;
    case 9: nTag = TAG_POWER_BUFF_9_IS_DISPLAYED; break;
    case 10: nTag = TAG_POWER_BUFF_10_IS_DISPLAYED; break;
    case 11: nTag = TAG_POWER_BUFF_11_IS_DISPLAYED; break;
    case 12: nTag = TAG_POWER_BUFF_12_IS_DISPLAYED; break;
    case 13: nTag = TAG_POWER_BUFF_13_IS_DISPLAYED; break;
    case 14: nTag = TAG_POWER_BUFF_14_IS_DISPLAYED; break;
    case 15: nTag = TAG_POWER_BUFF_15_IS_DISPLAYED; break;
    case 16: nTag = TAG_POWER_BUFF_16_IS_DISPLAYED; break;
    case 17: nTag = TAG_POWER_BUFF_17_IS_DISPLAYED; break;
    case 18: nTag = TAG_POWER_BUFF_18_IS_DISPLAYED; break;
    case 19: nTag = TAG_POWER_BUFF_19_IS_DISPLAYED; break;
    case 20: nTag = TAG_POWER_BUFF_20_IS_DISPLAYED; break;
    case 21: nTag = TAG_POWER_BUFF_21_IS_DISPLAYED; break;
    case 22: nTag = TAG_POWER_BUFF_22_IS_DISPLAYED; break;
    case 23: nTag = TAG_POWER_BUFF_23_IS_DISPLAYED; break;
    case 24: nTag = TAG_POWER_BUFF_24_IS_DISPLAYED; break;
    case 25: nTag = TAG_POWER_BUFF_25_IS_DISPLAYED; break;
    case 26: nTag = TAG_POWER_BUFF_26_IS_DISPLAYED; break;
    case 27: nTag = TAG_POWER_BUFF_27_IS_DISPLAYED; break;
    case 28: nTag = TAG_POWER_BUFF_28_IS_DISPLAYED; break;
    case 29: nTag = TAG_POWER_BUFF_29_IS_DISPLAYED; break;
    case 30: nTag = TAG_POWER_BUFF_30_IS_DISPLAYED; break;
    case 31: nTag = TAG_POWER_BUFF_31_IS_DISPLAYED; break;
    default: return FALSE;
    }
    if (!PowerGeneralTagGetBool(pdefPower, nTag))
        return FALSE;
    return PowerHasBuffIcon(pdefPower, nBuffIndex);
}

// Original line 1470.
BOOL PowerBuffMergesTooltip(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_MERGES_TOOLTIP; break;
    case 1: nTag = TAG_POWER_BUFF_1_MERGES_TOOLTIP; break;
    case 2: nTag = TAG_POWER_BUFF_2_MERGES_TOOLTIP; break;
    case 3: nTag = TAG_POWER_BUFF_3_MERGES_TOOLTIP; break;
    case 4: nTag = TAG_POWER_BUFF_4_MERGES_TOOLTIP; break;
    case 5: nTag = TAG_POWER_BUFF_5_MERGES_TOOLTIP; break;
    case 6: nTag = TAG_POWER_BUFF_6_MERGES_TOOLTIP; break;
    case 7: nTag = TAG_POWER_BUFF_7_MERGES_TOOLTIP; break;
    case 8: nTag = TAG_POWER_BUFF_8_MERGES_TOOLTIP; break;
    case 9: nTag = TAG_POWER_BUFF_9_MERGES_TOOLTIP; break;
    case 10: nTag = TAG_POWER_BUFF_10_MERGES_TOOLTIP; break;
    case 11: nTag = TAG_POWER_BUFF_11_MERGES_TOOLTIP; break;
    case 12: nTag = TAG_POWER_BUFF_12_MERGES_TOOLTIP; break;
    case 13: nTag = TAG_POWER_BUFF_13_MERGES_TOOLTIP; break;
    case 14: nTag = TAG_POWER_BUFF_14_MERGES_TOOLTIP; break;
    case 15: nTag = TAG_POWER_BUFF_15_MERGES_TOOLTIP; break;
    case 16: nTag = TAG_POWER_BUFF_16_MERGES_TOOLTIP; break;
    case 17: nTag = TAG_POWER_BUFF_17_MERGES_TOOLTIP; break;
    case 18: nTag = TAG_POWER_BUFF_18_MERGES_TOOLTIP; break;
    case 19: nTag = TAG_POWER_BUFF_19_MERGES_TOOLTIP; break;
    case 20: nTag = TAG_POWER_BUFF_20_MERGES_TOOLTIP; break;
    case 21: nTag = TAG_POWER_BUFF_21_MERGES_TOOLTIP; break;
    case 22: nTag = TAG_POWER_BUFF_22_MERGES_TOOLTIP; break;
    case 23: nTag = TAG_POWER_BUFF_23_MERGES_TOOLTIP; break;
    case 24: nTag = TAG_POWER_BUFF_24_MERGES_TOOLTIP; break;
    case 25: nTag = TAG_POWER_BUFF_25_MERGES_TOOLTIP; break;
    case 26: nTag = TAG_POWER_BUFF_26_MERGES_TOOLTIP; break;
    case 27: nTag = TAG_POWER_BUFF_27_MERGES_TOOLTIP; break;
    case 28: nTag = TAG_POWER_BUFF_28_MERGES_TOOLTIP; break;
    case 29: nTag = TAG_POWER_BUFF_29_MERGES_TOOLTIP; break;
    case 30: nTag = TAG_POWER_BUFF_30_MERGES_TOOLTIP; break;
    case 31: nTag = TAG_POWER_BUFF_31_MERGES_TOOLTIP; break;
    default: return FALSE;
    }
    return PowerGeneralTagGetBool(pdefPower, nTag);
}

// Original line 1519.
BOOL PowerBuffGetMergeTooltipIndex(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_MERGE_TOOLTIP_INDEX; break;
    case 1: nTag = TAG_POWER_BUFF_1_MERGE_TOOLTIP_INDEX; break;
    case 2: nTag = TAG_POWER_BUFF_2_MERGE_TOOLTIP_INDEX; break;
    case 3: nTag = TAG_POWER_BUFF_3_MERGE_TOOLTIP_INDEX; break;
    case 4: nTag = TAG_POWER_BUFF_4_MERGE_TOOLTIP_INDEX; break;
    case 5: nTag = TAG_POWER_BUFF_5_MERGE_TOOLTIP_INDEX; break;
    case 6: nTag = TAG_POWER_BUFF_6_MERGE_TOOLTIP_INDEX; break;
    case 7: nTag = TAG_POWER_BUFF_7_MERGE_TOOLTIP_INDEX; break;
    case 8: nTag = TAG_POWER_BUFF_8_MERGE_TOOLTIP_INDEX; break;
    case 9: nTag = TAG_POWER_BUFF_9_MERGE_TOOLTIP_INDEX; break;
    case 10: nTag = TAG_POWER_BUFF_10_MERGE_TOOLTIP_INDEX; break;
    case 11: nTag = TAG_POWER_BUFF_11_MERGE_TOOLTIP_INDEX; break;
    case 12: nTag = TAG_POWER_BUFF_12_MERGE_TOOLTIP_INDEX; break;
    case 13: nTag = TAG_POWER_BUFF_13_MERGE_TOOLTIP_INDEX; break;
    case 14: nTag = TAG_POWER_BUFF_14_MERGE_TOOLTIP_INDEX; break;
    case 15: nTag = TAG_POWER_BUFF_15_MERGE_TOOLTIP_INDEX; break;
    case 16: nTag = TAG_POWER_BUFF_16_MERGE_TOOLTIP_INDEX; break;
    case 17: nTag = TAG_POWER_BUFF_17_MERGE_TOOLTIP_INDEX; break;
    case 18: nTag = TAG_POWER_BUFF_18_MERGE_TOOLTIP_INDEX; break;
    case 19: nTag = TAG_POWER_BUFF_19_MERGE_TOOLTIP_INDEX; break;
    case 20: nTag = TAG_POWER_BUFF_20_MERGE_TOOLTIP_INDEX; break;
    case 21: nTag = TAG_POWER_BUFF_21_MERGE_TOOLTIP_INDEX; break;
    case 22: nTag = TAG_POWER_BUFF_22_MERGE_TOOLTIP_INDEX; break;
    case 23: nTag = TAG_POWER_BUFF_23_MERGE_TOOLTIP_INDEX; break;
    case 24: nTag = TAG_POWER_BUFF_24_MERGE_TOOLTIP_INDEX; break;
    case 25: nTag = TAG_POWER_BUFF_25_MERGE_TOOLTIP_INDEX; break;
    case 26: nTag = TAG_POWER_BUFF_26_MERGE_TOOLTIP_INDEX; break;
    case 27: nTag = TAG_POWER_BUFF_27_MERGE_TOOLTIP_INDEX; break;
    case 28: nTag = TAG_POWER_BUFF_28_MERGE_TOOLTIP_INDEX; break;
    case 29: nTag = TAG_POWER_BUFF_29_MERGE_TOOLTIP_INDEX; break;
    case 30: nTag = TAG_POWER_BUFF_30_MERGE_TOOLTIP_INDEX; break;
    case 31: nTag = TAG_POWER_BUFF_31_MERGE_TOOLTIP_INDEX; break;
    default: return FALSE;
    }
    return PowerGeneralTagGetInt(pdefPower, nTag);
}

// Original line 1569.
BOOL PowerBuffVisuallyAppliesToPets(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_DONT_APPLY_VISUAL_TO_PETS; break;
    case 1: nTag = TAG_POWER_BUFF_1_DONT_APPLY_VISUAL_TO_PETS; break;
    case 2: nTag = TAG_POWER_BUFF_2_DONT_APPLY_VISUAL_TO_PETS; break;
    case 3: nTag = TAG_POWER_BUFF_3_DONT_APPLY_VISUAL_TO_PETS; break;
    case 4: nTag = TAG_POWER_BUFF_4_DONT_APPLY_VISUAL_TO_PETS; break;
    case 5: nTag = TAG_POWER_BUFF_5_DONT_APPLY_VISUAL_TO_PETS; break;
    case 6: nTag = TAG_POWER_BUFF_6_DONT_APPLY_VISUAL_TO_PETS; break;
    case 7: nTag = TAG_POWER_BUFF_7_DONT_APPLY_VISUAL_TO_PETS; break;
    case 8: nTag = TAG_POWER_BUFF_8_DONT_APPLY_VISUAL_TO_PETS; break;
    case 9: nTag = TAG_POWER_BUFF_9_DONT_APPLY_VISUAL_TO_PETS; break;
    case 10: nTag = TAG_POWER_BUFF_10_DONT_APPLY_VISUAL_TO_PETS; break;
    case 11: nTag = TAG_POWER_BUFF_11_DONT_APPLY_VISUAL_TO_PETS; break;
    case 12: nTag = TAG_POWER_BUFF_12_DONT_APPLY_VISUAL_TO_PETS; break;
    case 13: nTag = TAG_POWER_BUFF_13_DONT_APPLY_VISUAL_TO_PETS; break;
    case 14: nTag = TAG_POWER_BUFF_14_DONT_APPLY_VISUAL_TO_PETS; break;
    case 15: nTag = TAG_POWER_BUFF_15_DONT_APPLY_VISUAL_TO_PETS; break;
    case 16: nTag = TAG_POWER_BUFF_16_DONT_APPLY_VISUAL_TO_PETS; break;
    case 17: nTag = TAG_POWER_BUFF_17_DONT_APPLY_VISUAL_TO_PETS; break;
    case 18: nTag = TAG_POWER_BUFF_18_DONT_APPLY_VISUAL_TO_PETS; break;
    case 19: nTag = TAG_POWER_BUFF_19_DONT_APPLY_VISUAL_TO_PETS; break;
    case 20: nTag = TAG_POWER_BUFF_20_DONT_APPLY_VISUAL_TO_PETS; break;
    case 21: nTag = TAG_POWER_BUFF_21_DONT_APPLY_VISUAL_TO_PETS; break;
    case 22: nTag = TAG_POWER_BUFF_22_DONT_APPLY_VISUAL_TO_PETS; break;
    case 23: nTag = TAG_POWER_BUFF_23_DONT_APPLY_VISUAL_TO_PETS; break;
    case 24: nTag = TAG_POWER_BUFF_24_DONT_APPLY_VISUAL_TO_PETS; break;
    case 25: nTag = TAG_POWER_BUFF_25_DONT_APPLY_VISUAL_TO_PETS; break;
    case 26: nTag = TAG_POWER_BUFF_26_DONT_APPLY_VISUAL_TO_PETS; break;
    case 27: nTag = TAG_POWER_BUFF_27_DONT_APPLY_VISUAL_TO_PETS; break;
    case 28: nTag = TAG_POWER_BUFF_28_DONT_APPLY_VISUAL_TO_PETS; break;
    case 29: nTag = TAG_POWER_BUFF_29_DONT_APPLY_VISUAL_TO_PETS; break;
    case 30: nTag = TAG_POWER_BUFF_30_DONT_APPLY_VISUAL_TO_PETS; break;
    case 31: nTag = TAG_POWER_BUFF_31_DONT_APPLY_VISUAL_TO_PETS; break;
    default: return TRUE;
    }
    return !PowerGeneralTagGetBool(pdefPower, nTag);
}

// Original line 1619.
BOOL PowerBuffShowsActiveOnSkillButton(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 1: nTag = TAG_POWER_BUFF_1_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 2: nTag = TAG_POWER_BUFF_2_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 3: nTag = TAG_POWER_BUFF_3_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 4: nTag = TAG_POWER_BUFF_4_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 5: nTag = TAG_POWER_BUFF_5_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 6: nTag = TAG_POWER_BUFF_6_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 7: nTag = TAG_POWER_BUFF_7_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 8: nTag = TAG_POWER_BUFF_8_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 9: nTag = TAG_POWER_BUFF_9_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 10: nTag = TAG_POWER_BUFF_10_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 11: nTag = TAG_POWER_BUFF_11_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 12: nTag = TAG_POWER_BUFF_12_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 13: nTag = TAG_POWER_BUFF_13_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 14: nTag = TAG_POWER_BUFF_14_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 15: nTag = TAG_POWER_BUFF_15_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 16: nTag = TAG_POWER_BUFF_16_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 17: nTag = TAG_POWER_BUFF_17_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 18: nTag = TAG_POWER_BUFF_18_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 19: nTag = TAG_POWER_BUFF_19_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 20: nTag = TAG_POWER_BUFF_20_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 21: nTag = TAG_POWER_BUFF_21_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 22: nTag = TAG_POWER_BUFF_22_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 23: nTag = TAG_POWER_BUFF_23_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 24: nTag = TAG_POWER_BUFF_24_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 25: nTag = TAG_POWER_BUFF_25_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 26: nTag = TAG_POWER_BUFF_26_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 27: nTag = TAG_POWER_BUFF_27_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 28: nTag = TAG_POWER_BUFF_28_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 29: nTag = TAG_POWER_BUFF_29_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 30: nTag = TAG_POWER_BUFF_30_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    case 31: nTag = TAG_POWER_BUFF_31_SHOW_ACTIVE_ON_SKILL_BUTTON; break;
    default: return TRUE;
    }
    return PowerGeneralTagGetInt(pdefPower, nTag);
}

// Original line 1719.
BOOL PowerBuffShowsDurationOnSkillButton(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 1: nTag = TAG_POWER_BUFF_1_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 2: nTag = TAG_POWER_BUFF_2_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 3: nTag = TAG_POWER_BUFF_3_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 4: nTag = TAG_POWER_BUFF_4_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 5: nTag = TAG_POWER_BUFF_5_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 6: nTag = TAG_POWER_BUFF_6_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 7: nTag = TAG_POWER_BUFF_7_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 8: nTag = TAG_POWER_BUFF_8_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 9: nTag = TAG_POWER_BUFF_9_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 10: nTag = TAG_POWER_BUFF_10_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 11: nTag = TAG_POWER_BUFF_11_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 12: nTag = TAG_POWER_BUFF_12_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 13: nTag = TAG_POWER_BUFF_13_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 14: nTag = TAG_POWER_BUFF_14_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 15: nTag = TAG_POWER_BUFF_15_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 16: nTag = TAG_POWER_BUFF_16_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 17: nTag = TAG_POWER_BUFF_17_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 18: nTag = TAG_POWER_BUFF_18_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 19: nTag = TAG_POWER_BUFF_19_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 20: nTag = TAG_POWER_BUFF_20_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 21: nTag = TAG_POWER_BUFF_21_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 22: nTag = TAG_POWER_BUFF_22_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 23: nTag = TAG_POWER_BUFF_23_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 24: nTag = TAG_POWER_BUFF_24_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 25: nTag = TAG_POWER_BUFF_25_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 26: nTag = TAG_POWER_BUFF_26_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 27: nTag = TAG_POWER_BUFF_27_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 28: nTag = TAG_POWER_BUFF_28_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 29: nTag = TAG_POWER_BUFF_29_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 30: nTag = TAG_POWER_BUFF_30_SHOW_DURATION_ON_SKILL_BUTTON; break;
    case 31: nTag = TAG_POWER_BUFF_31_SHOW_DURATION_ON_SKILL_BUTTON; break;
    default: return TRUE;
    }
    return PowerGeneralTagGetInt(pdefPower, nTag);
}

// Original line 1768.
BOOL PowerIsScripted(AutoPowerDefinitionR pdefPower) {
    return pdefPower->bScripted;
}

// Original line 1775.
BOOL PowerAutoTargets(AutoPowerDefinitionR pdefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    BOOL bAutoTargets = (int32)PowerGetFormulaValue(ptACDPlayer, pdefPower, TAG_POWER_CONSOLE_AUTO_TARGET_ENABLED, -1, -1, 0) != 0;
    return bAutoTargets;
}

// Original line 1813.
BOOL PowerPrefersRadialTargeting(AutoPowerDefinitionR pDefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    return (int32)PowerGetFormulaValue(ptACDPlayer, pDefPower, TAG_POWER_CONSOLE_PREFERS_RADIAL_TARGETING, -1, -1, 0) != 0;
}

// Original line 1840.
BOOL PowerFiresOnButtonUp(AutoPowerDefinitionR pDefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    return (int32)PowerGetFormulaValue(ptACDPlayer, pDefPower, TAG_POWER_CONSOLE_FIRES_ON_BUTTON_UP_ONLY, -1, -1, 0) != 0;
}

// Original line 1869.
BOOL PowerUsesTargetingReticleWhenHeld(AutoPowerDefinitionR pDefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    return (int32)PowerGetFormulaValue(ptACDPlayer, pDefPower, TAG_POWER_CONSOLE_USES_TARGET_RETICLE_WHEN_HELD, -1, -1, 0) != 0;
}

// Original line 1898.
BOOL PowerUseAtMaxRangeWhenHeld(AutoPowerDefinitionR pDefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    return (int32)PowerGetFormulaValue(ptACDPlayer, pDefPower, TAG_POWER_CONSOLE_USE_AT_MAX_RANGE_WHEN_HELD, -1, -1, 0) != 0;
}

// Original line 1928.
BOOL PowerRefundsResourcesWithoutDamage(AutoPowerDefinitionR pDefPower, const Player * ptPlayer) {
    if (!ptPlayer)
        return FALSE;
    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
    if (!ptACDPlayer)
        return FALSE;
    return (int32)PowerGetFormulaValue(ptACDPlayer, pDefPower, TAG_POWER_CONSOLE_REFUNDS_RESOURCES_WITHOUT_DAMAGE, -1, -1, 0) != 0;
}

// Original line 1957.
BOOL PowerBuffIconDisplayDuration(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_SHOW_DURATION; break;
    case 1: nTag = TAG_POWER_BUFF_1_SHOW_DURATION; break;
    case 2: nTag = TAG_POWER_BUFF_2_SHOW_DURATION; break;
    case 3: nTag = TAG_POWER_BUFF_3_SHOW_DURATION; break;
    case 4: nTag = TAG_POWER_BUFF_4_SHOW_DURATION; break;
    case 5: nTag = TAG_POWER_BUFF_5_SHOW_DURATION; break;
    case 6: nTag = TAG_POWER_BUFF_6_SHOW_DURATION; break;
    case 7: nTag = TAG_POWER_BUFF_7_SHOW_DURATION; break;
    case 8: nTag = TAG_POWER_BUFF_8_SHOW_DURATION; break;
    case 9: nTag = TAG_POWER_BUFF_9_SHOW_DURATION; break;
    case 10: nTag = TAG_POWER_BUFF_10_SHOW_DURATION; break;
    case 11: nTag = TAG_POWER_BUFF_11_SHOW_DURATION; break;
    case 12: nTag = TAG_POWER_BUFF_12_SHOW_DURATION; break;
    case 13: nTag = TAG_POWER_BUFF_13_SHOW_DURATION; break;
    case 14: nTag = TAG_POWER_BUFF_14_SHOW_DURATION; break;
    case 15: nTag = TAG_POWER_BUFF_15_SHOW_DURATION; break;
    case 16: nTag = TAG_POWER_BUFF_16_SHOW_DURATION; break;
    case 17: nTag = TAG_POWER_BUFF_17_SHOW_DURATION; break;
    case 18: nTag = TAG_POWER_BUFF_18_SHOW_DURATION; break;
    case 19: nTag = TAG_POWER_BUFF_19_SHOW_DURATION; break;
    case 20: nTag = TAG_POWER_BUFF_20_SHOW_DURATION; break;
    case 21: nTag = TAG_POWER_BUFF_21_SHOW_DURATION; break;
    case 22: nTag = TAG_POWER_BUFF_22_SHOW_DURATION; break;
    case 23: nTag = TAG_POWER_BUFF_23_SHOW_DURATION; break;
    case 24: nTag = TAG_POWER_BUFF_24_SHOW_DURATION; break;
    case 25: nTag = TAG_POWER_BUFF_25_SHOW_DURATION; break;
    case 26: nTag = TAG_POWER_BUFF_26_SHOW_DURATION; break;
    case 27: nTag = TAG_POWER_BUFF_27_SHOW_DURATION; break;
    case 28: nTag = TAG_POWER_BUFF_28_SHOW_DURATION; break;
    case 29: nTag = TAG_POWER_BUFF_29_SHOW_DURATION; break;
    case 30: nTag = TAG_POWER_BUFF_30_SHOW_DURATION; break;
    case 31: nTag = TAG_POWER_BUFF_31_SHOW_DURATION; break;
    default: return FALSE;
    }
    return TagMapGetInt(pdefPower->tPower.hGeneralTagMap, nTag) != 0;
}

// Original line 2006.
BOOL PowerBuffIsHarmful(AutoPowerDefinitionR pdefPower, int32 nBuffIndex) {
    int32 nTag;
    switch (nBuffIndex) {
    case 0: nTag = TAG_POWER_BUFF_0_HARMFUL_BUFF; break;
    case 1: nTag = TAG_POWER_BUFF_1_HARMFUL_BUFF; break;
    case 2: nTag = TAG_POWER_BUFF_2_HARMFUL_BUFF; break;
    case 3: nTag = TAG_POWER_BUFF_3_HARMFUL_BUFF; break;
    case 4: nTag = TAG_POWER_BUFF_4_HARMFUL_BUFF; break;
    case 5: nTag = TAG_POWER_BUFF_5_HARMFUL_BUFF; break;
    case 6: nTag = TAG_POWER_BUFF_6_HARMFUL_BUFF; break;
    case 7: nTag = TAG_POWER_BUFF_7_HARMFUL_BUFF; break;
    case 8: nTag = TAG_POWER_BUFF_8_HARMFUL_BUFF; break;
    case 9: nTag = TAG_POWER_BUFF_9_HARMFUL_BUFF; break;
    case 10: nTag = TAG_POWER_BUFF_10_HARMFUL_BUFF; break;
    case 11: nTag = TAG_POWER_BUFF_11_HARMFUL_BUFF; break;
    case 12: nTag = TAG_POWER_BUFF_12_HARMFUL_BUFF; break;
    case 13: nTag = TAG_POWER_BUFF_13_HARMFUL_BUFF; break;
    case 14: nTag = TAG_POWER_BUFF_14_HARMFUL_BUFF; break;
    case 15: nTag = TAG_POWER_BUFF_15_HARMFUL_BUFF; break;
    case 16: nTag = TAG_POWER_BUFF_16_HARMFUL_BUFF; break;
    case 17: nTag = TAG_POWER_BUFF_17_HARMFUL_BUFF; break;
    case 18: nTag = TAG_POWER_BUFF_18_HARMFUL_BUFF; break;
    case 19: nTag = TAG_POWER_BUFF_19_HARMFUL_BUFF; break;
    case 20: nTag = TAG_POWER_BUFF_20_HARMFUL_BUFF; break;
    case 21: nTag = TAG_POWER_BUFF_21_HARMFUL_BUFF; break;
    case 22: nTag = TAG_POWER_BUFF_22_HARMFUL_BUFF; break;
    case 23: nTag = TAG_POWER_BUFF_23_HARMFUL_BUFF; break;
    case 24: nTag = TAG_POWER_BUFF_24_HARMFUL_BUFF; break;
    case 25: nTag = TAG_POWER_BUFF_25_HARMFUL_BUFF; break;
    case 26: nTag = TAG_POWER_BUFF_26_HARMFUL_BUFF; break;
    case 27: nTag = TAG_POWER_BUFF_27_HARMFUL_BUFF; break;
    case 28: nTag = TAG_POWER_BUFF_28_HARMFUL_BUFF; break;
    case 29: nTag = TAG_POWER_BUFF_29_HARMFUL_BUFF; break;
    case 30: nTag = TAG_POWER_BUFF_30_HARMFUL_BUFF; break;
    case 31: nTag = TAG_POWER_BUFF_31_HARMFUL_BUFF; break;
    default: return FALSE;
    }
    return TagMapGetInt(pdefPower->tPower.hGeneralTagMap, nTag) != 0;
}

// Original line 2055.
BOOL PowerHasContactFrame(AutoPowerDefinitionR pdefPower) {
    for (int32 nContactFrame = 0; nContactFrame < 4; nContactFrame++) {
        if (PowerIsContactFrameEnabled(pdefPower, nContactFrame))
            return TRUE;
    }
    return FALSE;
}

// Original line 2071.
BOOL PowerReuseScriptState(AutoPowerDefinitionR pdefPower) {
    return TRUE;
}

// Original line 2170.
// Original line 2170.
BOOL PowerCanTurnIntoBasicAttack(AutoPowerDefinitionR pdefPower, ACDID idACD) {
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    BOOL fCanBecomeRanged = PowerGeneralTagGetBool(pdefPower, TAG_POWER_TURNS_INTO_BASIC_ATTACK_RANGED);
    BOOL fCanBecomeMelee = PowerGeneralTagGetBool(pdefPower, TAG_POWER_TURNS_INTO_BASIC_ATTACK_MELEE);
    if (!fCanBecomeMelee && !fCanBecomeRanged)
        return FALSE;
    if (fCanBecomeRanged && fCanBecomeMelee)
        return TRUE;
    SNO snoFallBackPower = ptACD->AttributeGetBasicAttack();
    BOOL bFallBackMelee = fCanBecomeMelee &&
        (snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_REACH_INSTANT) ||
         snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT) ||
         snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_OFFHAND) ||
         snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_BOTHHAND) ||
         snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_OBSTRUCTION));
    BOOL bFallBackRanged = fCanBecomeRanged &&
        (snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_RANGED_WAND) ||
         snoFallBackPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_RANGED_PROJECTILE));
    return bFallBackMelee || bFallBackRanged;
}

// Original line 2185.
BOOL PowerPlayerCanPayChannelCost(const ActorCommonData & tACDPlayer, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (tACDPlayer.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, snoPower)) ||
        tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)))
        return TRUE;
    ResourceType eType = PowerGetResourceType(tACDPlayer, pdefPower);
    float flCost = PowerGetFormulaValue(&tACDPlayer, pdefPower, TAG_POWER_COST_CONTINUAL_RESOURCE, -1, -1, 0);
    float flCurrentResource = tACDPlayer.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eType);
    return flCurrentResource > flCost || flCost < 0.000001f;
}

// Original line 2206.
SNO PowerGetNormalAttackReplacement(ACDID idACD) {
    ActorCommonData & tACD = ACDGet(idACD);
    SNO snoPower = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NORMAL_ATTACK_REPLACEMENT_POWER_SNO));
    return snoPower;
}

// Original line 2217.
// On DWARF lines 2217-2244 (decl lines of params/locals from the CU). Columns: 2227:40/2229:13 give 4-space indent,
// 2231:17/38 give `if(wpTarget.idSWorld != wpPlayer.idSWorld`; the INVALID test (row 2231:47, after the ||) is inferred;
// 2236:9/31/33/47/50 give `    if (Math::Abs(wpPlayer.wp.z - wpTarget.wp.z) > 30.0f)`. The return statements have no rows (inferred).
#line 2217
BOOL PowerIsTargetWithinValidZRange(
    const ACDID idACDPlayer,
    const Target & tTarget)
{
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer == NULL)
    {
        return FALSE;
    }

    WorldPlace wpPlayer = ptACDPlayer->GetPlace();
    WorldPlace wpTarget;
    tTarget.GetPlace(wpTarget);

    if(wpTarget.idSWorld != wpPlayer.idSWorld || wpTarget.idSWorld == SWORLDID_INVALID)
    {
        return FALSE;
    }

    if (Math::Abs(wpPlayer.wp.z - wpTarget.wp.z) > 30.0f)
    {
        return FALSE;
    }

    return TRUE;


}
#line 1186

// Original line 2248.
// Original line 2248.
BOOL PowerExtendTargetToPowerAttackRadius(const ACDID idACDPlayer, AutoPowerDefinitionR pdefPower, WorldPlace & placeTarget) {
    ActorCommonData * ptACDPlayer = ACDTryToGet(idACDPlayer);
    if (ptACDPlayer) {
        WorldPlace placePlayer = ptACDPlayer->GetPlace();
        float flPowerRadius = PowerGetRange(ptACDPlayer, pdefPower, TRUE);
        float flDistSqToTarget = Vector3D::DistanceSquared(placeTarget.wp, placePlayer.wp);
        float flPowerRadiusSq = flPowerRadius * flPowerRadius;
        if (flPowerRadius > 0.000001f && flDistSqToTarget < flPowerRadiusSq) {
            Vector3D wvToTarget = placeTarget.wp - placePlayer.wp;
            if (wvToTarget.Normalize() > 0.0f) {
                placeTarget.idSWorld = placePlayer.idSWorld;
                placeTarget.wp = placePlayer.wp + flPowerRadius * wvToTarget;
                return TRUE;
            }
        }
    }
    return FALSE;
}

// Original line 2283.
BOOL PowerPlayerAttackOnlyOncePerClick(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO) ||
        snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_NPC) ||
        snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_HELPER_ATTACH))
        return TRUE;
    if (PowerGetTemplate(pdefPower, ACDID_INVALID) == POTION_POWER)
        return TRUE;
    return FALSE;
}

// Original line 2304.
float PowerGetClassBonusPercent(AttribGroupID idFastAttrib, PlayerClass eClass) {
    float flBonusPercent = 0.0f;
    switch (eClass) {
    case PLAYER_CLASS_DEMONHUNTER:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_DEMONHUNTER));
        break;
    case PLAYER_CLASS_BARBARIAN:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_BARBARIAN));
        break;
    case PLAYER_CLASS_WIZARD:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_WIZARD));
        break;
    case PLAYER_CLASS_WITCHDOCTOR:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_WITCHDOCTOR));
        break;
    case PLAYER_CLASS_MONK:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_MONK));
        break;
    case PLAYER_CLASS_CRUSADER:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_CRUSADER));
        break;
    case PLAYER_CLASS_NECROMANCER:
        flBonusPercent = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_BONUS_NECROMANCER));
        break;
    }
    return flBonusPercent;
}

// Original line 2323.
// Original line 2323.
void PowerPayloadGetBonusMultipliers(AttribGroupID idFastAttrib, SNO snoPower, PowerID idPower, PlayerClass eClass, float & flPrimaryAttributeBonusMultiplier, float & flClassBonusMultiplier, float & flSkillsBonusMultiplier) {
    flSkillsBonusMultiplier = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS)) + 1.0f +
        FastAttribGetValueFloat(idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS, snoPower));
    flPrimaryAttributeBonusMultiplier = PowerComputeDamageScalarFromPrimaryAttribute(idFastAttrib) + 1.0f;
    flClassBonusMultiplier = PowerGetClassBonusPercent(idFastAttrib, eClass) + 1.0f;
    float flPowerInstanceDamagePercentBonus = FastAttribGetValueFloat(idFastAttrib, MakeAttribKey(ATTRIBUTE_POWER_INSTANCE_DAMAGE_PERCENT_BONUS, idPower));
    if (Math::Abs(flPowerInstanceDamagePercentBonus) > 0.000001f)
        flSkillsBonusMultiplier *= flPowerInstanceDamagePercentBonus + 1.0f;
    float flNGBonus = FastAttribGetValueFloat(idFastAttrib, FastAttribKey(ATTRIBUTE_DAMAGE_BONUS_FROM_NEPHALEM_GLORY));
    if (flNGBonus > 1.0f)
        flSkillsBonusMultiplier *= flNGBonus;
}

// Original line 2880.
float PowerGetAttackRadiusAtLevel(const ActorCommonData * ptACDOwner, AutoPowerDefinitionR pdefPower, const int32 nSkillLevel, const BOOL bUseBonusRange) {
    float flRadius = PowerGetFormulaValue(ptACDOwner, pdefPower, TAG_POWER_ATTACK_RADIUS, nSkillLevel, -1, 0);
    if (ptACDOwner) {
        if (PowerIsComboPower(pdefPower)) {
            int32 nComboLevel = ptACDOwner->AttributesGetInt(FastAttribKey(ATTRIBUTE_COMBO_LEVEL));
            switch (nComboLevel) {
            case 0:
                flRadius = PowerGetFormulaValue(ptACDOwner, pdefPower, TAG_POWER_COMBO_ATTACK_RADIUS_1, nSkillLevel, -1, 0);
                break;
            case 1:
                flRadius = PowerGetFormulaValue(ptACDOwner, pdefPower, TAG_POWER_COMBO_ATTACK_RADIUS_2, nSkillLevel, -1, 0);
                break;
            case 2:
                flRadius = PowerGetFormulaValue(ptACDOwner, pdefPower, TAG_POWER_COMBO_ATTACK_RADIUS_3, nSkillLevel, -1, 0);
                break;
            }
        }
        if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_USES_WEAPON_RANGE)) {
            SNO snoWeaponPower = ptACDOwner->AttributeGetBasicAttack();
            ScopedPowerDefinitionLockR pdefPrimaryWeaponPower(snoWeaponPower);
            flRadius = PowerGetFormulaValue(ptACDOwner, pdefPrimaryWeaponPower, TAG_POWER_ATTACK_RADIUS, nSkillLevel, -1, 0);
        }
        if (bUseBonusRange) {
            float flBonusRadius = ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_BONUS_ATTACK_RADIUS, pdefPower->tHeader.snoHandle));
            if (flBonusRadius > 0.000001f)
                flRadius += flBonusRadius;
        }
    }
    if (flRadius < 0.000001f)
        flRadius = FLT_MAX;
    return flRadius;
}

// Original line 2937.

float PowerGetRange(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower, const BOOL bUseBonusRange) {
    SNO snoPower = pdefPower.GetSNO();
    return PowerGetAttackRadiusAtLevel(ptACD, pdefPower, PowerGetLevel(ptACD, snoPower), bUseBonusRange);
}

// Original line 2949.
// Original line 2949.
void PowerGetTargetPlace(AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldPlace & tPlace) {
    if (tTarget.eType == 1) {
        ActorCommonData & tACDTarget = ACDGet(tTarget.idACDTarget);
        WorldLocation & tLocation = tACDTarget.GetWorldLocation();
        tPlace = tACDTarget.GetPlace();
        if (!PowerIsAimedAtGround(pdefPower))
            tPlace.wp.z += Math::Min(4.0f, tLocation.wcylBounds.wdHeight * 0.7f);
        return;
    }
    tTarget.GetPlace(tPlace);
}

// Original line 3246.
// Original line 3246.
BOOL PowerCanTarget(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, const Target * ptTarget) {
    if (!PowerIsPrimary(pdefPower, idACDOwner))
        return TRUE;
    if (!ptTarget)
        return FALSE;
    if (ptTarget->eType == 0)
        return TRUE;
    if (PowerIsGizmo(pdefPower) && ptTarget->eType != 1)
        return FALSE;
    if (PowerRequiresActorTarget(ACDGet(idACDOwner), pdefPower)) {
        if (!ptTarget->IsTargetingACD())
            return FALSE;
        if (ACDGet(ptTarget->idACDTarget).IsItem())
            return FALSE;
    }
    if (!ptTarget->bForcedActorTarget) {
        if (ptTarget->eType == 1 && ACDGet(ptTarget->idACDTarget).IsStealthed())
            return FALSE;
        if (ptTarget->eType != 2) {
            ActorCommonData * ptACDTarget = ACDTryToGet(ptTarget->idACDTarget);
            if (!ptACDTarget)
                return FALSE;
            return ptACDTarget->IsTargetable() != 0;
        }
    }
    return TRUE;
}

// Original line 3303.
// Original line 3303.
BOOL PowerIsValidCastTarget(AutoPowerDefinitionR pdefPower, ACDID idACDAttacker, ACDID idACDTarget, BOOL bAllowSelfTarget) {
    if (idACDTarget == idACDAttacker && !bAllowSelfTarget)
        return FALSE;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    ActorCommonData & tACDAttacker = ACDGet(idACDAttacker);
    if (WorldPlaceDistanceSquared(tACDAttacker.GetPlace(), tACDTarget.GetPlace()) > 4225.0f)
        return FALSE;
    if (tACDTarget.IsStealthed())
        return FALSE;
    if (tACDTarget.IsItem() && ItemIsGlyph(tACDTarget))
        return FALSE;
    if (!tACDTarget.IsTargetable())
        return FALSE;
    BOOL bCastTargetAllies = PowerCastTargetAllies(pdefPower);
    BOOL bCastTargetEnemies = PowerCastTargetEnemies(pdefPower);
    if (tACDTarget.GetTeam() == TEAM_INVALID) {
        if (!PowerCastTargetNeutral(pdefPower))
            return FALSE;
    } else {
        BOOL bFriendly = tACDTarget.IsWreckableObject() ? FALSE : ACDIsFriendly(tACDAttacker, tACDTarget, TRUE);
        if (!bCastTargetAllies && bFriendly)
            return FALSE;
        if (!bFriendly && !bCastTargetEnemies)
            return FALSE;
    }
    if (ACDIsPlayer(idACDTarget) && !tACDTarget.IsAlive())
        return FALSE;
    if (tACDTarget.IsMonster()) {
        if (PowerGeneralTagGetInt(pdefPower, TAG_POWER_CAST_TARGET_IGNORE_LARGE_MONSTERS) && tACDTarget.IsLargeMonster())
            return FALSE;
        if (PowerGeneralTagGetInt(pdefPower, TAG_POWER_CAST_TARGET_NORMAL_MONSTERS_ONLY) && !tACDTarget.IsNormalMonster())
            return FALSE;
        if (PowerGeneralTagGetInt(pdefPower, TAG_POWER_CAST_TARGET_UNDEAD_MONSTERS_ONLY) && !tACDTarget.IsUndeadMonster())
            return FALSE;
        if (!PowerGeneralTagGetInt(pdefPower, TAG_POWER_CAST_TARGET_ALLOW_DEAD_TARGETS) && !tACDTarget.IsAlive())
            return FALSE;
    }
    BOOL bNecroCorpsesOnly = PowerCastTargetNecroCorpsesOnly(tACDAttacker, pdefPower);
    if (bNecroCorpsesOnly != (tACDTarget.IsNecroCorpse() != FALSE))
        return FALSE;
    if (tACDTarget.IsNecroCorpse() && !tACDAttacker.CanTargetNecroCorpse(idACDTarget))
        return FALSE;
    return TRUE;
}

// Original line 3476.
BOOL PowerApplyPassiveAfterItemPassives(AutoPowerDefinitionR pdefPower) {
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_APPLY_PASSIVE_AFTER_ITEM_PASSIVES);
}

// Original line 3484.
BOOL PowerIsInvulnerableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IS_INVULNERABLE_DURING, -1, -1, 0));
}

// Original line 3493.
BOOL PowerIsInvisibleDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IS_INVISIBLE_DURING, -1, -1, 0));
}

// Original line 3502.
BOOL PowerIsUntargetableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IS_UNTARGETABLE_DURING, -1, -1, 0));
}

// Original line 3511.
BOOL PowerIsUninterruptableDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IS_UNINTERRUPTABLE_DURING, -1, -1, 0));
}

// Original line 3519.
BOOL PowerCannotDieDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_CANNOT_DIE_DURING, -1, -1, 0));
}

// Original line 3528.
BOOL PowerCanUseWhenFeared(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_CAN_USE_WHEN_FEARED, -1, -1, 0));
}

// Original line 3537.
BOOL PowerBreaksStun(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_BREAKS_STUN, -1, -1, 0));
}

// Original line 3546.
BOOL PowerBreaksFear(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_BREAKS_FEAR, -1, -1, 0));
}

// Original line 3555.
BOOL PowerBreaksSnare(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_BREAKS_SNARE, -1, -1, 0));
}

// Original line 3564.
BOOL PowerBreaksRoot(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_BREAKS_ROOT, -1, -1, 0));
}

// Original line 3574.
BOOL PowerBreaksImmobilize(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_BREAKS_IMMOBILIZE, -1, -1, 0));
}

// Original line 3611.
BOOL PowerIsImmuneToStunDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IMMUNE_TO_STUN_DURING, -1, -1, 0));
}

// Original line 3620.
BOOL PowerIsImmuneToSnareDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IMMUNE_TO_SNARE_DURING, -1, -1, 0));
}

// Original line 3629.
BOOL PowerIsImmuneToRootDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IMMUNE_TO_ROOT_DURING, -1, -1, 0));
}

// Original line 3638.
BOOL PowerIsImmuneToFearDuring(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_IMMUNE_TO_FEAR_DURING, -1, -1, 0));
}

// Original line 3647.
float PowerComputeDamageScalarFromPrimaryAttributeValue(float flPrimaryAttributeValue) {
    return flPrimaryAttributeValue * 0.01f;
}

// Original line 3655.
float PowerComputeDamageScalarFromPrimaryAttribute(AttribGroupID idFastAttrib) {
    Attrib eAttribute;
    float flResult = 0.0f;
    PrimaryDamageAttribute ePrimaryDamageAttribute = FastAttribGetValueInt(idFastAttrib, FastAttribKey(ATTRIBUTE_PRIMARY_DAMAGE_ATTRIBUTE));
    switch (ePrimaryDamageAttribute) {
    case PRIMARY_DAMAGE_ATTRIBUTE_STRENGTH:
        eAttribute = ATTRIBUTE_STRENGTH_TOTAL;
        break;
    case PRIMARY_DAMAGE_ATTRIBUTE_DEXTERITY:
        eAttribute = ATTRIBUTE_DEXTERITY_TOTAL;
        break;
    case PRIMARY_DAMAGE_ATTRIBUTE_INTELLIGENCE:
        eAttribute = ATTRIBUTE_INTELLIGENCE_TOTAL;
        break;
    default:
        return flResult;
    }
    flResult = PowerComputeDamageScalarFromPrimaryAttributeValue(FastAttribGetValueFloat(idFastAttrib, FastAttribKey(eAttribute)));
    return flResult;
}

// Original line 3688.
float PowerComputeCritChance(const ActorCommonData * ptACDAttacker, BOOL bUsedForDPS) {
    float flCritChance = 0.0f;
    if (ptACDAttacker) {
        flCritChance = ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_PERCENT_BASE)) +
            ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_PERCENT_BONUS_CAPPED)) +
            ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_WEAPON_CRIT_CHANCE_CURRENTHAND));
        if (!bUsedForDPS)
            flCritChance += ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_PERCENT_BONUS_HIDE_FROM_DPS));
        flCritChance = Math::Min(ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_PERCENT_CAP)), flCritChance);
        flCritChance += ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_PERCENT_BONUS_UNCAPPED));
    }
    return Math::Pin(flCritChance, 0.0f, 1.0f);
}

// Original line 3718.
float PowerComputeCritDamageScalar(const ActorCommonData * ptACDAttacker) {
    if (!ptACDAttacker)
        return 1.0f;
    BOOL bAttackerIsMonster = ptACDAttacker->IsMonster();
    float flBonusDamageScalar;
    {
        ScopedGlobalDefinitionLockR pdefGlobal;
        flBonusDamageScalar = bAttackerIsMonster ? pdefGlobal->flMonsterCritDamageScalar : pdefGlobal->flPlayerCritDamageScalar;
    }
    flBonusDamageScalar += ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_DAMAGE_PERCENT));
    float flCap = ptACDAttacker->AttributesGetFloat(FastAttribKey(ATTRIBUTE_CRIT_DAMAGE_CAP));
    if (flCap > 0.000001f) {
        flBonusDamageScalar = Math::Min(flCap, flBonusDamageScalar);
    }
    return flBonusDamageScalar;
}

// Original line 3760.
float PowerComputeResistanceDamageMitigation(float flResistanceTotal, int32 nAttackerLevel) {
    flResistanceTotal = Math::Max(0.0f, flResistanceTotal);
    float flDenom = flResistanceTotal + nAttackerLevel * 5.0f;
    float flPercentMitigation = 0.0f;
    if (flDenom > 0.000001f)
        flPercentMitigation = flResistanceTotal / flDenom;
    return flPercentMitigation;
}

// Original line 3776.
float PowerComputeDodgeChance(const ActorCommonData & tACDDefender, int32 nLevel, PayloadDeliveryMechanism eDeliveryType, BOOL bUsedForToughness) {
    BOOL bCannotDodge = tACDDefender.AttributesGetInt(FastAttribKey(ATTRIBUTE_CANNOT_DODGE));
    if (bCannotDodge)
        return 0.0f;
    float flChanceBonus = tACDDefender.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS));
    float flChancePayloadType = tACDDefender.AttributesGetFloat(FastAttribKey(eDeliveryType == PDM_RANGED ? ATTRIBUTE_DODGE_CHANCE_BONUS_RANGED : ATTRIBUTE_DODGE_CHANCE_BONUS_MELEE));
    float flDodgeChance = 1.0f - (1.0f - flChanceBonus) * (1.0f - flChancePayloadType);
    if (tACDDefender.AttributesAttributeExists(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_ADDITIVE))) {
        flDodgeChance = Math::Min(1.0f, flDodgeChance + tACDDefender.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_ADDITIVE)));
    }
    if (!bUsedForToughness && tACDDefender.AttributesAttributeExists(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS))) {
        flDodgeChance = Math::Min(1.0f, flDodgeChance + tACDDefender.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DODGE_CHANCE_BONUS_HIDE_FROM_TOUGHNESS)));
    }
    return flDodgeChance;
}

// Original line 3818.
static const float ARMOR_DENOMINATOR_LEVEL_COEFFICIENT = 50.0f;
float PowerComputeArmorDamageMitigation(const ActorCommonData & tACDTarget, const int32 nAttackerLevel) {
    float flArmor = Math::Max(0.0f, tACDTarget.AttributesGetFloat(FastAttribKey(ATTRIBUTE_ARMOR_TOTAL)));
    const float flConstant = ARMOR_DENOMINATOR_LEVEL_COEFFICIENT * nAttackerLevel;
    float flDenom = flConstant + flArmor;
    float flPercentMitigation = 0.0f;
    if (flDenom > 0.000001f)
        flPercentMitigation = flArmor / flDenom;
    return flPercentMitigation;
}

// Original line 3840.
BOOL PowerCanActorTrainActiveSkill(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return FALSE;
    if (!PowerActorMeetsActiveSkillMasteryRequirement(tACD, pdefPower))
        return FALSE;
    return TRUE;
}

// Original line 3861.
BOOL PowerCanActorTrainTrait(const ActorCommonData & tACD, SNO snoPower) {
    if (snoPower == SNO_NONE)
        return FALSE;
    if (!PowerActorMeetsTraitMasteryRequirement(tACD, snoPower))
        return FALSE;
    return TRUE;
}

// Original line 3881.
BOOL PowerActorMeetsActiveSkillMasteryRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    SNO snoSkillKit = tACD.GetSkillKit();
    const ActiveSkillEntry * ptActiveSkillEntry = SkillKitGetMatchingActiveSkillEntry(snoSkillKit, snoPower);
    if (!ptActiveSkillEntry)
        return FALSE;
    int32 nMasteryLevelRequirement = ptActiveSkillEntry->nMasteryReq;
    if (nMasteryLevelRequirement <= 0)
        return TRUE;
    int32 nMasteryLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    return nMasteryLevel >= nMasteryLevelRequirement;
}

// Original line 3908.
BOOL PowerActorMeetsSkillRuneRequirement(const ActorCommonData & tACD, SNO snoPower, const RuneType eRuneType) {
    int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    int32 nRuneUnlockLevel = tACD.GetRuneUnlockLevel(snoPower, eRuneType);
    return nLevel >= nRuneUnlockLevel;
}

// Original line 3921.
BOOL PowerActorMeetsTraitMasteryRequirement(const ActorCommonData & tACD, SNO snoPower) {
    SNO snoSkillKit = tACD.GetSkillKit();
    const TraitEntry * ptTraitEntry = SkillKitGetMatchingTraitEntry(snoSkillKit, snoPower);
    if (!ptTraitEntry)
        return FALSE;
    int32 nMasteryLevelRequirement = ptTraitEntry->nMasteryReq;
    if (nMasteryLevelRequirement <= 0)
        return TRUE;
    int32 nMasteryLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    return nMasteryLevel >= nMasteryLevelRequirement;
}

// Original line 3946.
BOOL PowerActorMeetsSkillPointRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (ACDIsPlayer(tACD.id) && PowerGeneralTagGetBool(pdefPower, TAG_POWER_REQUIRES_SKILLPOINT)) {
        if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower)) <= 0)
            return FALSE;
    }
    return TRUE;
}

// Original line 3968.
// Original line 3968.
static BOOL sItemPowerSatisfiesItemRequirements(ACDID idACDItem, GBID gbidRequirement, const PowerDefinition * pdefPower) {
    GBHandle hgbPower = cghgbNone;
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDItem) {
        hgbPower = ptACDItem->hGB;
        if (ItemIsBroken(*ptACDItem))
            return FALSE;
        if (ItemIsEquippedButDisabled(*ptACDItem))
            return FALSE;
    }
    BOOL bRequires1H = PowerGeneralTagGetInt(pdefPower, TAG_POWER_REQUIRES_1H_ITEM);
    BOOL bRequires2H = PowerGeneralTagGetInt(pdefPower, TAG_POWER_REQUIRES_2H_ITEM);
    if (!ItemIsOfType(hgbPower, gbidRequirement))
        return FALSE;
    if (!ptACDItem && (bRequires1H || bRequires2H))
        return FALSE;
    if (bRequires1H && ItemIsTwoHanded(*ptACDItem))
        return FALSE;
    if (bRequires2H && !ItemIsTwoHanded(*ptACDItem))
        return FALSE;
    return TRUE;
}

// Original line 4020.
// Original line 4020.
BOOL PowerActorMeetsWeaponRequirement(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return FALSE;
    GBID gbidItemRequirement = PowerGeneralTagGetGBID(pdefPower, TAG_POWER_ITEM_TYPE_REQUIREMENT);
    if (gbidItemRequirement == GBID_NONE || !tACD.HasInventory())
        return TRUE;
    ACDID idACDPrimaryItem = ACDInventoryGetPrimaryWeapon(tACD.id);
    ACDID idACDSecondaryItem = ACDInventoryGetSecondaryWeaponOrShield(tACD.id);
    return sItemPowerSatisfiesItemRequirements(idACDPrimaryItem, gbidItemRequirement, pdefPower) ||
           sItemPowerSatisfiesItemRequirements(idACDSecondaryItem, gbidItemRequirement, pdefPower);
}

// Original line 4059.
// Original line 4059.
BOOL PowerActorCanFreeCastIfNecessary(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (GameActiveGameCommonDataIsServer() && ActorFlagIsSet((ActorID)ACDGetOwner(tACD.id), ACTOR_GOD_MODE_BIT))
        return TRUE;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, snoPower)) ||
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)))
        return TRUE;
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_ONLY_FREE_CAST, -1, -1, 0) == 0.0f;
}

// Original line 4090.
float PowerGetCostReductionPercent(const ActorCommonData & tACD, const ResourceType eResourceType, const DamageType eDamageType, const SNO snoPower) {
    float flCostReductionPercent = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_PERCENT_TOTAL, eResourceType);
    float flDamageTypeReductionPercent = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_COST_REDUCTION_PERCENT, eDamageType));
    float flPowerCostReductionPercent = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION_PERCENT, snoPower));
    float flTotal = (1.0f - flCostReductionPercent) * (1.0f - flDamageTypeReductionPercent) * (1.0f - flPowerCostReductionPercent);
    return flTotal;
}

// Original line 4108.
BOOL PowerActorCanPayResourceCost(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (GameActiveGameCommonDataIsServer() && ActorFlagIsSet((ActorID)ACDGetOwner(tACD.id), ACTOR_GOD_MODE_BIT))
        return TRUE;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, snoPower)) || tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)))
        return TRUE;
    float flResourceCost = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_RESOURCE, -1, -1, 0);
    if (PowerIsChannelled(tACD, pdefPower))
        flResourceCost = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_CONTINUAL_RESOURCE, -1, -1, 0);
    // positive block: DWARF lexical block (4134-4175) holds eType..flCurrentResource
    if (flResourceCost > 0.000001f) {
        ResourceType eType = PowerGetResourceType(tACD, pdefPower);
        RuneType eRuneType = PowerGetRuneTypeForACD(tACD, snoPower);
        float flPrimaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_PRIMARY_RESOURCE_COST_OVERRIDE, snoPower));
        float flSecondaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_SECONDARY_RESOURCE_COST_OVERRIDE, snoPower));
        if (flPrimaryResourceOverridenCost < FLT_MAX && flPrimaryResourceOverridenCost > 0.0f)
            eType = tACD.GetPrimaryResourceType();
        else if (flPrimaryResourceOverridenCost > 0.0f && flSecondaryResourceOverridenCost < FLT_MAX)
            eType = tACD.GetSecondaryResourceType();
        if (flPrimaryResourceOverridenCost < FLT_MAX) {
            if (eType == tACD.GetPrimaryResourceType())
                flResourceCost = flPrimaryResourceOverridenCost;
        }
        if (flSecondaryResourceOverridenCost < FLT_MAX) {
            if (eType == tACD.GetSecondaryResourceType())
                flResourceCost = flSecondaryResourceOverridenCost;
        }
        float flCostReductionPercent = PowerGetCostReductionPercent(tACD, eType, PowerGetDamageType(AutoPowerDefinitionR(snoPower), eRuneType), snoPower);
        flResourceCost *= flCostReductionPercent;
        float flCostReduction = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eType) + tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, snoPower));
        flResourceCost = Math::Max(1.0f, flResourceCost - flCostReduction);
        float flCurrentResource = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eType);
        if (flResourceCost > flCurrentResource)
            return FALSE;
    }
    return TRUE;
}

// Original line 4187.
ResourceType PowerGetCurrentResourceType(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, float * pflPrimaryResourceOverridenCost, float * pflSecondaryResourceOverridenCost) {
    ResourceType eType = PowerGetResourceType(tACD, pdefPower);
    float flPrimaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_PRIMARY_RESOURCE_COST_OVERRIDE, pdefPower->tHeader.snoHandle));
    float flSecondaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_SECONDARY_RESOURCE_COST_OVERRIDE, pdefPower->tHeader.snoHandle));
    if (flPrimaryResourceOverridenCost < FLT_MAX && flPrimaryResourceOverridenCost > 0.0f)
        eType = tACD.GetPrimaryResourceType();
    else if (flPrimaryResourceOverridenCost > 0.0f && flSecondaryResourceOverridenCost < FLT_MAX)
        eType = tACD.GetSecondaryResourceType();
    if (pflPrimaryResourceOverridenCost)
        *pflPrimaryResourceOverridenCost = flPrimaryResourceOverridenCost;
    if (pflSecondaryResourceOverridenCost)
        *pflSecondaryResourceOverridenCost = flSecondaryResourceOverridenCost;
    return eType;
}

// Original line 4224.
// Original line 4224.
BOOL PowerActorHasResourceMinCostToStart(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, pdefPower->tHeader.snoHandle)) ||
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)))
        return TRUE;
    float flPrimaryResourceOverridenCost;
    float flSecondaryResourceOverridenCost;
    ResourceType eType = PowerGetCurrentResourceType(tACD, pdefPower, &flPrimaryResourceOverridenCost, &flSecondaryResourceOverridenCost);
    float flMinToCast = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_RESOURCE_MIN_TO_CAST, -1, -1, 0);
    if (flPrimaryResourceOverridenCost < FLT_MAX && eType == tACD.GetPrimaryResourceType())
        flMinToCast = flPrimaryResourceOverridenCost;
    if (flSecondaryResourceOverridenCost < FLT_MAX && eType == tACD.GetSecondaryResourceType())
        flMinToCast = flSecondaryResourceOverridenCost;
    RuneType eRuneType = PowerGetRuneTypeForACD(tACD, pdefPower->tHeader.snoHandle);
    float flCostReductionPercent = PowerGetCostReductionPercent(tACD, eType, PowerGetDamageType(pdefPower, eRuneType), pdefPower->tHeader.snoHandle);
    flMinToCast *= flCostReductionPercent;
    if (flMinToCast > 0.000001f) {
        float flCurrentResource = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eType);
        if (flMinToCast > flCurrentResource)
            return FALSE;
    }
    return TRUE;
}

// Original line 4273.
// Original line 4273.
BOOL PowerActorCanPayHealthCost(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, BOOL bFreeCast) {
    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)) ||
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)) ||
        tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, pdefPower->tHeader.snoHandle)) || bFreeCast)
        return TRUE;
    float flHealthCostPercentOfMaxHealth = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_HEALTH, -1, -1, 0);
    float flHealthCost = flHealthCostPercentOfMaxHealth * tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_MAX_TOTAL));
    flHealthCost *= 1.0f - tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_HEALTH_COST_DISCOUNT));
    flHealthCost *= tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_HEALTH_COST_SCALAR, pdefPower->tHeader.snoHandle)) + 1.0f;
    if (flHealthCost > 0.000001f) {
        float flHPCurrent = tACD.GetHitPointsCurrent();
        if (flHPCurrent - flHealthCost < 1.0f)
            return FALSE;
    }
    return TRUE;
}

// Original line 4311.
// Original line 4311.
BOOL PowerActorCanPayChargeCost(const ActorCommonData & tACD, SNO snoPower) {
    if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOD)))
        return TRUE;
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    float flChargeCost = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_CHARGES, -1, -1, 0);
    int32 nChargeCost = Math::FloatRoundToInt(flChargeCost);
    if (nChargeCost > 0) {
        int32 nCurrentCharges = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_CHARGES, snoPower));
        return nCurrentCharges >= nChargeCost;
    }
    return TRUE;
}

// Original line 4339.
int32 PowerActorGetChargeCost(const ActorCommonData & tACD, SNO snoPower) {
    if (snoPower == SNO_NONE)
        return 0;
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return Math::FloatRoundToInt(PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_CHARGES, -1, -1, 0));
}

// Original line 4356.
int32 PowerActorGetMaxCharges(const ActorCommonData & tACD, SNO snoPower) {
    if (snoPower == SNO_NONE)
        return 0;
    ScopedPowerDefinitionLockR pdefPower(snoPower);
    return Math::FloatRoundToInt(PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_MAX_CHARGES, -1, -1, 0));
}

// Original line 4373.
int32 PowerActorGetRechargeTime(const ActorCommonData & tACD, SNO snoPower) {
    int32 nRechargeTime = 0;
    if (snoPower != SNO_NONE) {
        ScopedPowerDefinitionLockR pdefPower(snoPower);
        float flRechargeTime = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_RECHARGE_TIME, -1, -1, 0);
        nRechargeTime = Math::FloatRoundToInt(flRechargeTime);
    }
    return nRechargeTime * 60;
}

// Original line 4391.
void PowerActorGetWeaponDamageAndAttackSpeed(const ActorCommonData & tACD, float & flDamage, float & flAttackSpeed, const BOOL bWantBonuses, const PowerID idPower, const BOOL bUsedForDPS) {
    SNO snoBasicAttack = tACD.AttributeGetBasicAttack();
    if (snoBasicAttack == SNO_NONE) {
        flDamage = 0.0f;
        flAttackSpeed = 0.0f;
        return;
    }
    ScopedPowerDefinitionLockR pdefPrimaryWeaponPower(snoBasicAttack);
    flAttackSpeed = PowerGetFormulaValue(&tACD, pdefPrimaryWeaponPower, TAG_POWER_ATTACK_SPEED, -1, -1, 0);
    PowerDamageData tData;
    PowerGetDamageData(AutoPowerDefinitionR(snoBasicAttack), &tACD, idPower, tData, NULL, TRUE);
    float flDamageMin = 0.0f;
    float flDamageDelta = 0.0f;
    for (int32 nDamageType = 0; nDamageType < 7; ++nDamageType) {
        ElementDamage & tDmg = tData.tElementDamage[nDamageType];
        flDamageMin += tDmg.flTotalMin;
        flDamageDelta += tDmg.flTotalDelta;
    }
    flDamage = flDamageMin + flDamageDelta * 0.5f;
    if (bWantBonuses) {
        float flSkillsBonusMultiplier = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_DAMAGE_PERCENT_ALL_FROM_SKILLS)) + 1.0f;
        if (idPower != POWERID_INVALID) {
            Power * ptPower = PowerTryToGet(idPower);
            if (ptPower) {
                flSkillsBonusMultiplier += tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_DAMAGE_PERCENT_BONUS, ptPower->snoPower));
                float flPowerInstanceDamagePercentBonus = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_INSTANCE_DAMAGE_PERCENT_BONUS, idPower));
                if (Math::Abs(flPowerInstanceDamagePercentBonus) > 0.000001f)
                    flSkillsBonusMultiplier *= flPowerInstanceDamagePercentBonus + 1.0f;
            }
        }
        float flPrimaryAttributeBonusMultiplier = PowerComputeDamageScalarFromPrimaryAttribute(tACD.AttributesGetAttributeGroup()) + 1.0f;
        float flClassBonusMultiplier = PowerGetClassBonusPercent(tACD.AttributesGetAttributeGroup(), PlayerGetPlayerClass(tACD.id)) + 1.0f;
        flDamage *= flPrimaryAttributeBonusMultiplier; // three statements: rows 4455/4456/4457 at column 18
        flDamage *= flClassBonusMultiplier;
        flDamage *= flSkillsBonusMultiplier;
        float flCritChance = PowerComputeCritChance(&tACD, bUsedForDPS);
        float flBonusDamageScalar = PowerComputeCritDamageScalar(&tACD);
        flDamage = (flDamage * (1 - flCritChance)) + (flDamage * flCritChance * flBonusDamageScalar);
    }
}

// Original line 4470.
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower) {
    uint32 dwNow = GameGetServerTime();
    if (snoPower != SNO_NONE) {
        uint32 dwPowerCoolDown = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN, snoPower));
        if (dwPowerCoolDown != (uint32)-1 && dwNow < dwPowerCoolDown)
            return FALSE;
    }
    return TRUE;
}

// Original line 4506.
BOOL PowerIsEmoteCoolDownDone(const ActorCommonData & tACD) {
    uint32 uNow = GameActiveGameCommonDataIsClient() ? CGameGetPredictedServerTime() : GameGetServerTime();
    uint32 uCoolDown = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_EMOTE_COOLDOWN));
    return uNow >= uCoolDown;
}

// Original line 4532.
uint32 PowerGetCoolDown(const ActorCommonData & tACD) {
    return tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GENERAL_COOLDOWN));
}

// Original line 4541.
float PowerGetCoolDownReductionPercent(SNO snoPower, const ActorCommonData & tACD) {
    float flReductionPercent = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_PERCENT, snoPower));
    float flReductionPercentAll = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_PERCENT_ALL_CAPPED));
    float flReductionPercentTotal = 1.0f - (1.0f - flReductionPercent) * (1.0f - flReductionPercentAll);
    return flReductionPercentTotal;
}

// Original line 4558.
// Original line 4558.
uint32 PowerGetCastingCooldown(AutoPowerDefinitionR pdefPower, const ActorCommonData & tACD) {
    SNO snoPower = pdefPower.GetSNO();
    BOOL bAllowCDR = !PowerGeneralTagGetBool(snoPower, TAG_POWER_DONT_ALLOW_COOLDOWN_REDUCTION);
    float flCooldownTime = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COOLDOWN, -1, -1, 0);
    float flCooldownMinTime = flCooldownTime > 0.000001f ? tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_POWER_COOLDOWN_MIN_TIME)) : 0.0f;
    if (bAllowCDR) {
        flCooldownTime -= tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION, snoPower));
        flCooldownTime -= tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_POWER_COOLDOWN_REDUCTION_ALL));
        flCooldownTime *= 1.0f - PowerGetCoolDownReductionPercent(snoPower, tACD);
    }
    flCooldownTime = Math::Max(flCooldownTime, flCooldownMinTime);
    if (flCooldownTime > 0.000001f)
        return (int32)(flCooldownTime * 60.0f);
    return 0;
}

// Original line 4595.
float PowerGetAttackSpeedAtLevel(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, const int32 nSkillLevel, const int32 nComboLevelToUse) {
    uint32 dwAttackSpeedTag = TAG_POWER_ATTACK_SPEED;
    if (PowerIsComboPower(pdefPower)) {
        int32 nComboLevel = nComboLevelToUse;
        if (nComboLevel == -1)
            nComboLevel = tACD.GetComboLevel();
        switch (nComboLevel) {
        case 0: dwAttackSpeedTag = TAG_POWER_COMBO_ATTACK_SPEED_1; break;
        case 1: dwAttackSpeedTag = TAG_POWER_COMBO_ATTACK_SPEED_2; break;
        case 2: dwAttackSpeedTag = TAG_POWER_COMBO_ATTACK_SPEED_3; break;
        }
    }
    float flResult = PowerGetFormulaValue(&tACD, pdefPower, dwAttackSpeedTag, nSkillLevel, -1, 0);
    if (flResult < 0.000001f)
        flResult = 0.3f;
    if (PowerIsTranslate(pdefPower)) {
        const float flMinAllowedReduction = 0.75f;
        float flMovementScalar = Math::Max(tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR_TOTAL)), flMinAllowedReduction);
        flResult *= flMovementScalar;
    }
    return flResult;
}

// Original line 4656.
float PowerGetBaseDamageScalar(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_BASE_DAMAGE_SCALAR, -1, -1, 0);
}

// Original line 4665.
// Original line 4665.
ResourceCheckResult PowerCheckResource(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, BOOL bCurrentlyChannelingPower) {
    SNO snoPower = pdefPower.GetSNO();
    ResourceType eCostType = PowerGetResourceType(tACD, pdefPower);
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_FREE_CAST, snoPower)) || tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FREE_CAST_ALL)))
        return 0;
    if (PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_ONLY_FREE_CAST, -1, -1, 0) != 0.0f)
        return 2;
    float flCost = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_RESOURCE, -1, -1, 0);
    if (PowerIsChannelled(tACD, pdefPower))
        flCost = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_CONTINUAL_RESOURCE, -1, -1, 0);
    RuneType eRuneType = PowerGetRuneTypeForACD(tACD, snoPower);
    float flCostReductionPercent = PowerGetCostReductionPercent(tACD, eCostType, PowerGetDamageType(AutoPowerDefinitionR(snoPower), eRuneType), snoPower);
    flCost *= flCostReductionPercent;
    float flCostReduction = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_COST_REDUCTION_TOTAL, eCostType) +
                            tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_RESOURCE_REDUCTION, snoPower));
    flCost = Math::Max(0.0f, flCost - flCostReduction);
    float flMinToCast = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_COST_RESOURCE_MIN_TO_CAST, -1, -1, 0);
    float flPrimaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_PRIMARY_RESOURCE_COST_OVERRIDE, pdefPower->tHeader.snoHandle));
    float flSecondaryResourceOverridenCost = tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_POWER_SECONDARY_RESOURCE_COST_OVERRIDE, pdefPower->tHeader.snoHandle));
    if (flPrimaryResourceOverridenCost < FLT_MAX && eCostType == tACD.GetPrimaryResourceType()) {
        flCost = flPrimaryResourceOverridenCost;
        flMinToCast = flPrimaryResourceOverridenCost;
    }
    if (flSecondaryResourceOverridenCost < FLT_MAX && eCostType == tACD.GetSecondaryResourceType()) {
        flCost = flSecondaryResourceOverridenCost;
        flMinToCast = flSecondaryResourceOverridenCost;
    }
    float flCurrentValue = tACD.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, eCostType);
    if (flCurrentValue < flCost)
        return 1;
    if (!bCurrentlyChannelingPower && flCurrentValue < flMinToCast)
        return 1;
    return 0;
}

// Original line 4736.
PowerTemplate PowerGetTemplate(AutoPowerDefinitionR pdefPower, ACDID idACDOwner) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return UNDEFINED_POWER_TEMPLATE;
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))
        return WALK_POWER;
    if (ptACDOwner) {
        if (PowerIsComboPower(pdefPower)) {
            int32 nComboLevel = ptACDOwner->GetComboLevel();
            switch (nComboLevel) {
            case 0: return PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_TEMPLATE_1);
            case 1: return PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_TEMPLATE_2);
            case 2: return PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_TEMPLATE_3);
            }
        }
        if (ACDIsPlayer(idACDOwner)) {
            RuneType eRuneType = PowerGetRuneTypeForACD(*ptACDOwner, snoPower);
            if (eRuneType != RUNE_TYPE_INVALID) {
                uint32 dwTag = 0;
                switch (eRuneType) {
                case RUNE_TYPE_A: dwTag = TAG_POWER_TEMPLATE_RUNE_A; break;
                case RUNE_TYPE_B: dwTag = TAG_POWER_TEMPLATE_RUNE_B; break;
                case RUNE_TYPE_C: dwTag = TAG_POWER_TEMPLATE_RUNE_C; break;
                case RUNE_TYPE_D: dwTag = TAG_POWER_TEMPLATE_RUNE_D; break;
                case RUNE_TYPE_E: dwTag = TAG_POWER_TEMPLATE_RUNE_E; break;
                }
                PowerTemplate eTemplateOverride = PowerGeneralTagGetInt(pdefPower, dwTag);
                if (eTemplateOverride != UNDEFINED_POWER_TEMPLATE)
                    return eTemplateOverride;
            }
        }
    }
    return PowerGeneralTagGetInt(pdefPower, TAG_POWER_TEMPLATE);
}
// Original line 4807.
BOOL PowerIsRecoilImmune(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_RECOIL_IMMUNE, -1, -1, 0));
}

// Original line 4817.
BOOL PowerIsKnockbackImmune(ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower) {
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_KNOCKBACK_IMMUNE, -1, -1, 0));
}

// Original line 4827.
BOOL PowerNeverUpdatesFacingDuring(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget) {
    BOOL bNeverUpdatesFacingDuring = PowerGeneralTagGetBool(pdefPower, TAG_POWER_NEVER_UPDATES_FACING_DURING);
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, idACDOwner);
    if (bNeverUpdatesFacingDuring)
        return TRUE;
    if (idACDTarget != ACDID_INVALID && eTemplate == GIZMO_POWER) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (!ptACDTarget || !GizmoShouldFaceToOperate(*ptACDTarget))
            return TRUE;
    }
    return FALSE;
}

// Original line 4857.
// Original line 4857.
BOOL PowerNeverUpdatesFacingStarting(AutoPowerDefinitionR pdefPower, ACDID idACDOwner, ACDID idACDTarget) {
    SNO snoPower = pdefPower.GetSNO();
    BOOL bNeverUpdatesFacingStarting = PowerGeneralTagGetBool(pdefPower, TAG_POWER_NEVER_UPDATES_FACING_STARTING);
    PowerTemplate eTemplate = PowerGetTemplate(pdefPower, idACDOwner);
    if (bNeverUpdatesFacingStarting)
        return TRUE;
    if (idACDTarget != ACDID_INVALID && eTemplate == GIZMO_POWER) {
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && !GizmoShouldFaceToOperate(*ptACDTarget))
            return TRUE;
    }
    ActorCommonData & tACDOwner = ACDGet(idACDOwner);
    if (!ACDIsDualWielding(tACDOwner, TRUE) && ACDIsPlayer(tACDOwner.id) &&
        (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT) || snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_OFFHAND)) &&
        AnimPlayerAnimExists(tACDOwner, TAG_ACTOR_ANIMATION_MELEE_ATTACK_LEFT) && AnimPlayerAnimExists(tACDOwner, TAG_ACTOR_ANIMATION_MELEE_ATTACK_RIGHT))
        return TRUE;
    return FALSE;
}

// Original line 4904.
// Original line 4904.
// locals declared in DWARF order (decl lines 4950-4954)
BOOL PowerIsCloseEnoughToPlace(ACDID idACD, const WorldPlace & placeSource, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance wdRange, WorldDistance wdLongReach, float * pflDistance, BOOL bIgnoreRange) {
    if (pflDistance)
        *pflDistance = 0.0f;
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget && !ptACDTarget->IsInWorld())
        return FALSE;
    if (tTarget.IsInvalid())
        return TRUE;
    WorldPlace placeTarget;
    tTarget.GetPlace(placeTarget);
    if (!WorldPlaceInSameWorld(placeSource, placeTarget))
        return FALSE;
    if (bIgnoreRange)
        return TRUE;
    WorldDistance wdAttackRadius;
    WorldDistance wdSourceRadius = 0.0f;
    WorldDistance wdSourceHeight = tACD.GetHeight();
    WorldDistance wdTargetRadius = 0.0f;
    WorldDistance wdTargetHeight = 6.0f;
    if (PowerIsGizmo(pdefPower)) {
        if (!ptACDTarget)
            return FALSE;
        GizmoGetOperationRadius(*ptACDTarget, &tACD, wdAttackRadius);
    } else {
        if (!(wdRange > 0.0f))
            wdRange = PowerGetRange(&tACD, pdefPower, TRUE);
        wdAttackRadius = wdRange;
        wdSourceRadius = tACD.GetRadius();
        if (ptACDTarget) {
            wdTargetRadius = ptACDTarget->GetRadius();
            wdTargetHeight = ptACDTarget->GetHeight();
        }
    }
    WorldDistance wdDistance = PowerGetDistanceViaWorldPlace(placeSource, placeTarget, wdSourceHeight, wdSourceRadius, wdTargetHeight, wdTargetRadius, wdLongReach);
    if (wdDistance > wdAttackRadius) {
        if (pflDistance)
            *pflDistance = wdDistance - wdAttackRadius;
        return FALSE;
    }
    if (GameActiveGameCommonDataIsServer()) {
        ActorID idActorTauntTarget = BrainAPIGetTauntTarget((ActorID)ACDGetOwner(idACD));
        if (idActorTauntTarget != ACTORID_INVALID) {
            Actor * ptActor = ActorTryToGet(idActorTauntTarget);
            if (ptActor) {
                ACDID idACDTauntTarget = ptActor->idACD;
                ActorCommonData * ptACDTauntTarget = ACDTryToGet(idACDTauntTarget);
                if (ptACDTauntTarget) {
                    BOOL bCloseEnough = !ptACDTauntTarget->IsAlive() || !(wdDistance > 6.0f);
                    if (bCloseEnough)
                        return bCloseEnough;
                    if (pflDistance)
                        *pflDistance = wdDistance - 6.0f;
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

// Original line 5025.
// Original line 5025.
BOOL PowerIsFacing(ACDID idACD, SNO snoPower, const Target * ptTarget) {
    ActorID idActor = (ActorID)ACDGetOwner(idACD);
    Actor * ptActor = ActorGet(idActor);
    ScopedActorDefinitionLockR pdefActor(ptActor->snoActor);
    Angle aTurningSpeed = TagMapGetFloat(pdefActor->hTagMap, TAG_ACTOR_SERVER_TURNING_SPEED);
    if (aTurningSpeed != 0.0f) {
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace placeCurrent = tACD.GetPlace();
        WorldPlace placeDest;
        ptTarget->GetPlace(placeDest);
        Angle aToTarget = Math::ATan(placeDest.wp.y - placeCurrent.wp.y, placeDest.wp.x - placeCurrent.wp.x);
        Angle aFacing = tACD.GetOrientation().ToYaw();
        Angle aDifference = Math::AngularDifference(aToTarget, aFacing);
        if (aDifference > EIGHTH_CIRCLE * 0.5f)
            return FALSE;
    }
    return TRUE;
}

// Original line 5061.

BOOL PowerIsCloseEnough(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, BOOL bIgnoreRange) {
    return PowerIsCloseEnough(idACD, pdefPower, ptTarget, 0.0f, 0.0f, NULL, bIgnoreRange);
}

// Original line 5072.
// Original line 5072.
BOOL PowerIsCloseEnough(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target * ptTarget, WorldDistance wdRange, WorldDistance wdLongReach, float * pflDistance, BOOL bIgnoreRange) {
    if (pflDistance)
        *pflDistance = 0.0f;
    if (!ptTarget)
        return FALSE;
    ActorCommonData & tACD = ACDGet(idACD);
    if (!tACD.IsInWorld())
        return FALSE;
    if (GameActiveGameCommonDataIsServer()) {
        SNO snoPower;  // never assigned: the original passes an undefined register here
        if (!PowerIsFacing(idACD, snoPower, ptTarget))
            return FALSE;
    }
    if (ACDIsPlayer(idACD) && wdLongReach < 0.000001f)
        wdLongReach = 1.0f;
    WorldPlace placeSource = tACD.GetPlace();
    BOOL bCloseEnough = PowerIsCloseEnoughToPlace(idACD, placeSource, pdefPower, *ptTarget, wdRange, wdLongReach, pflDistance, bIgnoreRange);
    return bCloseEnough;
}

// Original line 5129.
void PowerGetNavPlaceFromACD(const ActorCommonData * ptACD, NavPlace & tNavPlace) {
    ACDGetNavMeshHandleWithPlace(*ptACD, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);
}

// Original line 5266.
BOOL PowerPlaceToNavMeshHandle(const WorldPlace & tPlace, NavMeshHandle & tNavMeshHandle) {
    BOOL bOnMesh = ScenesCommonNavMeshNodeFromPlace(tPlace, &tNavMeshHandle, FALSE, FALSE);
    if (bOnMesh) {
        BOOL bCanEnter = NavMeshHandleCanEnter(tNavMeshHandle, PASSABILITY_UNKNOWN_OBJECT);
        if (bCanEnter)
            return TRUE;
    }
    tNavMeshHandle = NAVMESH_HANDLE_NONE;
    return FALSE;
}

// Original line 5297.
// Original line 5297.
BOOL PowerCheckStartLOS(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target & tTarget) {
    SNO snoPower = pdefPower.GetSNO();
    PowerStartLOSType eLOSType = PowerGetStartLOS(pdefPower);
    if (eLOSType == 0 || eLOSType == 4)
        return TRUE;
    ActorCommonData & tACD = ACDGet(idACD);
    if (!tACD.IsInWorld())
        return FALSE;
    ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
    if (ptACDTarget) {
        BOOL bSkipLOSChecks;
        {
            ScopedActorDefinitionLockR pdefActor(ptACDTarget->snoActor);
            bSkipLOSChecks = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_LOS_CHECKS_TO_SELF_ALWAYS_SUCCEED);
        }
        if (bSkipLOSChecks)
            return TRUE;
    }
    PassabilityType ePassabilityType = eLOSType == 1 ? PASSABILITY_PROJECTILE : PASSABILITY_MONSTER;
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_OPERATE_GIZMO))
        ePassabilityType = PASSABILITY_GIZMO_OPERATE;
    WorldPlace wpTarget;
    if (tTarget.GetPlace(wpTarget)) {
        WorldPlace wpACD = tACD.GetPlace();
        if (wpTarget.idSWorld != wpACD.idSWorld)
            return FALSE;
        float x = wpACD.wp.x - wpTarget.wp.x;
        float y = wpACD.wp.y - wpTarget.wp.y;
        if (x * x + y * y > 57600.0f)
            return FALSE;
    }
    BOOL bLOS;
    if (eLOSType == 1) {
        bLOS = WorldCollProjectileLineOfSight(idACD, tTarget);
    } else {
        NavPlace tNavPlace;
        bLOS = PowerRollToTargetNavPlace(idACD, tTarget, ePassabilityType, FALSE, tNavPlace);
    }
    return bLOS;
}

// Original line 5379.
// Original line 5379.
void PowerGetStartRange(ACDID idACD, AutoPowerDefinitionR pdefPower, const Target & tTarget, WorldDistance & wdAttackRange) {
    const float POWER_START_RANGE_HYSTERESIS_FEET = 0.5f;
    ActorCommonData & tACD = ACDGet(idACD);
    if (PowerIsGizmo(pdefPower)) {
        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (ptACDTarget)
            GizmoGetOperationRadius(*ptACDTarget, &tACD, wdAttackRange);
        else
            wdAttackRange = 0.0f;
    } else {
        WorldDistance wdAttackRadius = PowerGetRange(&tACD, pdefPower, TRUE);
        WorldDistance wdSourceRadius = tACD.GetRadius();
        ACDID idACDTarget = tTarget.GetACD();
        WorldDistance wdTargetRadius = 0.0f;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget) {
            if (!ptACDTarget->IsInWorld()) {
                wdAttackRange = 0.0f;
                return;
            }
            wdTargetRadius = ptACDTarget->GetRadius();
        }
        wdAttackRange = wdAttackRadius + wdSourceRadius + wdTargetRadius;
    }
    wdAttackRange = Math::Max(0.0f, wdAttackRange - POWER_START_RANGE_HYSTERESIS_FEET);
    if (!GameActiveGameCommonDataIsServer())
        return;
    ActorID idActorTauntTarget = BrainAPIGetTauntTarget((ActorID)ACDGetOwner(idACD));
    if (idActorTauntTarget != ACTORID_INVALID) {
        Actor * ptActor = ActorTryToGet(idActorTauntTarget);
        if (ptActor) {
            ACDID idACDTauntTarget = ptActor->idACD;
            ActorCommonData * ptACDTauntTarget = ACDTryToGet(idACDTauntTarget);
            if (ptACDTauntTarget && ptACDTauntTarget->IsAlive())
                wdAttackRange = 3.2f;
        }
    }
}

// Original line 5444.
ResourceType PowerGetResourceType(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    if (pdefPower->bUseSecondaryResource)
        return tACD.GetSecondaryResourceType();
    return tACD.GetPrimaryResourceType();
}

// Original line 5454.
// Original line 5454.
void PowerFaceTarget(ActorCommonData * ptACD, const Target & tTarget, const uint32 dwAnimationTag, BOOL bSnapToFacing) {
    if (!ptACD)
        return;
    if (!tTarget.IsInWorld())
        return;
    if (tTarget.IsInvalidForSource(ptACD))
        return;
    WorldPlace placeSource = ptACD->GetPlace();
    WorldPlace placeTarget;
    tTarget.GetPlace(placeTarget);
    if (!WorldPlaceInSameWorld(placeSource, placeTarget))
        return;
    WorldDistance wdSeparationSquared = WorldPlaceDistanceSquared(placeSource, placeTarget);
    if (wdSeparationSquared < 0.1f)
        return;
    switch (dwAnimationTag) {
    case TAG_ACTOR_EVADE_BACK:
    case TAG_ACTOR_TOWN_EVADE_BACK: {
        Vector3D vToTarget = placeTarget.wp - placeSource.wp;
        placeTarget.wp = placeSource.wp - vToTarget;
        break;
    }
    case TAG_ACTOR_EVADE_RIGHT:
    case TAG_ACTOR_TOWN_EVADE_RIGHT: {
        Vector3D vToTarget = placeTarget.wp - placeSource.wp;
        Vector3D::Rotate(cgwvZAxis, QUARTER_CIRCLE, vToTarget);
        placeTarget.wp = placeSource.wp + vToTarget;
        break;
    }
    case TAG_ACTOR_EVADE_LEFT:
    case TAG_ACTOR_TOWN_EVADE_LEFT: {
        Vector3D vToTarget = placeTarget.wp - placeSource.wp;
        Vector3D::Rotate(cgwvZAxis, -QUARTER_CIRCLE, vToTarget);
        placeTarget.wp = placeSource.wp + vToTarget;
        break;
    }
    }
    ptACD->SetFacing(placeTarget, bSnapToFacing);
}

// Original line 5531.
int32 PowerGetLevel(const ActorCommonData * ptACD, SNO snoPower) {
    if (!ptACD)
        return 0;
    return ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoPower));
}

// Original line 5546.
float PowerGetFormulaValue(const ActorCommonData * ptACD, AutoPowerDefinitionR pdefPower,
    uint32 dwPowerTag, int32 nLevel, AttribGroupID idAttribGroup, uint32 nContactFrame) {
    float flValue = 0.0f;
    BOOL bIsPVP = GameTypeIsPvP();
    const CTagMap * pTagMap;
    const CTagMap * pPVPTagMap;
    if (TagMapGetTagInfo(pdefPower->tPower.hGeneralTagMap, dwPowerTag)) {
        pTagMap = pdefPower->tPower.hGeneralTagMap;
        pPVPTagMap = pdefPower->tPower.hPVPGeneralTagMap;
    } else {
        pTagMap = pdefPower->tPower.hContactTagMap[nContactFrame];
        pPVPTagMap = pdefPower->tPower.hPVPContactTagMap[nContactFrame];
    }
    const CTagMap * pTagMapToUse = pTagMap;
    if (bIsPVP && TagMapTagExists(pPVPTagMap, dwPowerTag))
        pTagMapToUse = pPVPTagMap;
    switch (TagGetVariantType(pTagMap, dwPowerTag)) {
    case TAGVARIANTTYPE_FORMULA: {
        uint32 dwSize;
        uint8 * pbFormula = TagMapGetBBEFormula(pTagMapToUse, dwPowerTag, dwSize);
        if (idAttribGroup == -1)
            idAttribGroup = ptACD->AttributesGetAttributeGroup();
        if (dwSize) {
            int & rnPowerFormulaStackCount = ThreadGlobals::Get().m_nPowerFormulaStackCount;
            if (rnPowerFormulaStackCount >= POWER_FORMULA_STACK_OVERFLOW_LIMIT) {
                // The original build retains this diagnostic lookup without logging.
                const AxeTagInfo * ptInfo = TagMapGetTagInfo(pTagMap, dwPowerTag);
                (void)ptInfo;
            } else {
                ++rnPowerFormulaStackCount;
                flValue = PowerGetFormulaValueAtLevel(idAttribGroup, ptACD,
                    pdefPower->tHeader.snoHandle, pbFormula, dwSize, nLevel);
                --rnPowerFormulaStackCount;
            }
        }
        break;
    }
    case TAGVARIANTTYPE_INT:
        flValue = (float)TagMapGetInt(pTagMapToUse, dwPowerTag);
        break;
    case TAGVARIANTTYPE_FLOAT:
        flValue = TagMapGetFloat(pTagMapToUse, dwPowerTag);
        break;
    default: {
        // The original build retains this diagnostic lookup without logging.
        const AxeTagInfo * pTagInfo = TagMapGetTagInfo(pTagMapToUse, dwPowerTag);
        (void)pTagInfo;
        break;
    }
    }
    return flValue;
}
// Original line 5649.
BOOL PowerShouldUseClientWalking(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower != SNO_NONE && ACDIsPlayer(tACD.id)) {
        float flShouldUse = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_USE_SPECIALWALK_STEERING, -1, -1, 0);
        if (flShouldUse > 0.000001f)
            return TRUE;
    }
    return FALSE;
}

// Original line 5671.
BOOL PowerCanMoveForwardBeforeContactFrame(AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return FALSE;
    return (pdefPower->tPower.uFlags & (1 << POWERBIT_CONSOLE_MOVES_FORWARD_NO_TARGET)) != 0;
}



// Original line 5688.
float PowerGetOffsetFromTarget(const ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    SNO snoPower = pdefPower.GetSNO();
    if (snoPower == SNO_NONE)
        return 0.0f;
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CONTROLLER_OFFSET_FROM_AUTO_TARGET, -1, -1, 0);
}

// Original line 5835.
RuneType PowerGetRuneTypeForACD(const ActorCommonData & tACD, SNO snoPower) {
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_A, snoPower)) > 0)
        return 0;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_B, snoPower)) > 0)
        return 1;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_C, snoPower)) > 0)
        return 2;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_D, snoPower)) > 0)
        return 3;
    if (tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_RUNE_E, snoPower)) > 0)
        return 4;
    return -1;
}

// Original line 5900.
// Original line 5900.
CRefString PowerGetRuneName(SNO snoPower, RuneType eRuneType) {
    if (eRuneType == RUNE_TYPE_INVALID)
        return StringListFetch("SkillPane:Rune0");
    CRefString sLabel("NameRune_");
    switch (eRuneType) {
    case RUNE_TYPE_A: sLabel += "A#"; break;
    case RUNE_TYPE_B: sLabel += "B#"; break;
    case RUNE_TYPE_C: sLabel += "C#"; break;
    case RUNE_TYPE_D: sLabel += "D#"; break;
    case RUNE_TYPE_E: sLabel += "E#"; break;
    }
    sLabel += SNOToString((SNOGroup)0x1d, snoPower, FALSE);
    CRefString sName = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_ATTRIBUTE_DESCRIPTIONS), sLabel.str());
    if (sgtAppGlobals.dwDebugFlags2 & 2) {
        sName += " - ";
        sName += CRefString().Format("%c", 'A' + eRuneType);
    }
    return sName;
}

// Original line 5919.
CRefString PowerGetRuneStoneTextureFrame(const RuneType eRuneType) {
    switch (eRuneType) {
    case RUNE_TYPE_A:
        return CRefString("UI:Console_SkillSelectionRune01_Selected");
    case RUNE_TYPE_B:
        return CRefString("UI:Console_SkillSelectionRune02_Selected");
    case RUNE_TYPE_C:
        return CRefString("UI:Console_SkillSelectionRune03_Selected");
    case RUNE_TYPE_D:
        return CRefString("UI:Console_SkillSelectionRune04_Selected");
    case RUNE_TYPE_E:
        return CRefString("UI:Console_SkillSelectionRune05_Selected");
    default:
        return CRefString("");
    }
}

// Original line 5944.
DamageType PowerGetDamageType(AutoPowerDefinitionR pdefPower, const RuneType eRuneType) {
    uint32 dwTag;
    switch (eRuneType) {
    case RUNE_TYPE_A: dwTag = TAG_POWER_RUNEA_DAMAGE_TYPE; break;
    case RUNE_TYPE_B: dwTag = TAG_POWER_RUNEB_DAMAGE_TYPE; break;
    case RUNE_TYPE_C: dwTag = TAG_POWER_RUNEC_DAMAGE_TYPE; break;
    case RUNE_TYPE_D: dwTag = TAG_POWER_RUNED_DAMAGE_TYPE; break;
    case RUNE_TYPE_E: dwTag = TAG_POWER_RUNEE_DAMAGE_TYPE; break;
    default: dwTag = TAG_POWER_NORUNE_DAMAGE_TYPE; break;
    }
    return TagMapGetInt(pdefPower->tPower.hGeneralTagMap, dwTag);
}

// Original line 5977.
BOOL PowerGetBuffAttributes(int32 nBuffIndex, Attrib & eAttribStart, Attrib & eAttribEnd, Attrib & eAttribCount) {
    switch (nBuffIndex) {
    case 0:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_0;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_0;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_0;
        return TRUE;
    case 1:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_1;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_1;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_1;
        return TRUE;
    case 2:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_2;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_2;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_2;
        return TRUE;
    case 3:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_3;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_3;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_3;
        return TRUE;
    case 4:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_4;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_4;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_4;
        return TRUE;
    case 5:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_5;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_5;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_5;
        return TRUE;
    case 6:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_6;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_6;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_6;
        return TRUE;
    case 7:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_7;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_7;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_7;
        return TRUE;
    case 8:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_8;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_8;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_8;
        return TRUE;
    case 9:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_9;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_9;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_9;
        return TRUE;
    case 10:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_10;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_10;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_10;
        return TRUE;
    case 11:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_11;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_11;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_11;
        return TRUE;
    case 12:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_12;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_12;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_12;
        return TRUE;
    case 13:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_13;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_13;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_13;
        return TRUE;
    case 14:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_14;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_14;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_14;
        return TRUE;
    case 15:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_15;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_15;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_15;
        return TRUE;
    case 16:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_16;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_16;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_16;
        return TRUE;
    case 17:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_17;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_17;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_17;
        return TRUE;
    case 18:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_18;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_18;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_18;
        return TRUE;
    case 19:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_19;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_19;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_19;
        return TRUE;
    case 20:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_20;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_20;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_20;
        return TRUE;
    case 21:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_21;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_21;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_21;
        return TRUE;
    case 22:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_22;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_22;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_22;
        return TRUE;
    case 23:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_23;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_23;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_23;
        return TRUE;
    case 24:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_24;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_24;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_24;
        return TRUE;
    case 25:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_25;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_25;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_25;
        return TRUE;
    case 26:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_26;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_26;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_26;
        return TRUE;
    case 27:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_27;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_27;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_27;
        return TRUE;
    case 28:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_28;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_28;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_28;
        return TRUE;
    case 29:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_29;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_29;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_29;
        return TRUE;
    case 30:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_30;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_30;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_30;
        return TRUE;
    case 31:
        eAttribStart = ATTRIBUTE_BUFF_ICON_START_TICK_31;
        eAttribEnd = ATTRIBUTE_BUFF_ICON_END_TICK_31;
        eAttribCount = ATTRIBUTE_BUFF_ICON_COUNT_31;
        return TRUE;
    }
    eAttribStart = ATTRIBUTE_NONE;
    eAttribEnd = ATTRIBUTE_NONE;
    eAttribCount = ATTRIBUTE_NONE;
    return FALSE;
}

// Original line 6161.
Attrib PowerRuneTypeAndBuffIndexToAttribute(SNO snoPower, RuneType eType, int32 nBuffIndex) {
    if (eType < RUNE_TYPE_INVALID || eType >= RUNE_TYPE_COUNT)
        return ATTRIBUTE_NONE;
    if (nBuffIndex < 0 || nBuffIndex >= 32)
        return ATTRIBUTE_NONE;
    switch (nBuffIndex) {
    case 0:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 1:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 2:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 3:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 4:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 5:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 6:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 7:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 8:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 9:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 10:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 11:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 12:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 13:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 14:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 15:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 16:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 17:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 18:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 19:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 20:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 21:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 22:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 23:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 24:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 25:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 26:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 27:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 28:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 29:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 30:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_E;
        }
        break;
    case 31:
        switch (eType) {
        case RUNE_TYPE_INVALID: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_NONE;
        case RUNE_TYPE_A: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_A;
        case RUNE_TYPE_B: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_B;
        case RUNE_TYPE_C: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_C;
        case RUNE_TYPE_D: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_D;
        case RUNE_TYPE_E: return ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_E;
        }
        break;
    }
    return ATTRIBUTE_NONE;
}

// Original line 6543.
void PowerAttributeToRuneTypeAndBuffIndex(Attrib eAttrib, RuneType & eType, int32 & nBuffIndex) {
    switch (eAttrib) {
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_0_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 0;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_1_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 1;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_2_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 2;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_3_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 3;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_4_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 4;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_5_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 5;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_6_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 6;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_7_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 7;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_8_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 8;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_9_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 9;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_10_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 10;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_11_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 11;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_12_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 12;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_13_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 13;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_14_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 14;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_15_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 15;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_16_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 16;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_17_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 17;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_18_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 18;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_19_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 19;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_20_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 20;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_21_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 21;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_22_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 22;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_23_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 23;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_24_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 24;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_25_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 25;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_26_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 26;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_27_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 27;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_28_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 28;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_29_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 29;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_30_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 30;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_NONE:
        eType = RUNE_TYPE_INVALID;
        nBuffIndex = 31;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_A:
        eType = RUNE_TYPE_A;
        nBuffIndex = 31;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_B:
        eType = RUNE_TYPE_B;
        nBuffIndex = 31;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_C:
        eType = RUNE_TYPE_C;
        nBuffIndex = 31;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_D:
        eType = RUNE_TYPE_D;
        nBuffIndex = 31;
        break;
    case ATTRIBUTE_POWER_BUFF_31_VISUAL_EFFECT_RUNE_E:
        eType = RUNE_TYPE_E;
        nBuffIndex = 31;
        break;
    }
}

// Original line 7792.
// Original line 7792.
static uint32 sDetermineAnimationForRune(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower, uint32 dwTagNone, uint32 dwTagRuneA, uint32 dwTagRuneB, uint32 dwTagRuneC, uint32 dwTagRuneD, uint32 dwTagRuneE) {
    SNO snoPower = pdefPower.GetSNO();
    RuneType eRuneType = PowerGetRuneTypeForACD(tACD, snoPower);
    uint32 dwAnimTagNone = PowerGeneralTagGetInt(pdefPower, dwTagNone);
    uint32 dwAnimTagRuneA = PowerGeneralTagGetInt(pdefPower, dwTagRuneA);
    uint32 dwAnimTagRuneB = PowerGeneralTagGetInt(pdefPower, dwTagRuneB);
    uint32 dwAnimTagRuneC = PowerGeneralTagGetInt(pdefPower, dwTagRuneC);
    uint32 dwAnimTagRuneD = PowerGeneralTagGetInt(pdefPower, dwTagRuneD);
    uint32 dwAnimTagRuneE = PowerGeneralTagGetInt(pdefPower, dwTagRuneE);
    uint32 dwAnimationTag = dwAnimTagNone;
    if (eRuneType == RUNE_TYPE_A && dwAnimTagRuneA != (uint32)-1)
        dwAnimationTag = dwAnimTagRuneA;
    else if (eRuneType == RUNE_TYPE_B && dwAnimTagRuneB != (uint32)-1)
        dwAnimationTag = dwAnimTagRuneB;
    else if (eRuneType == RUNE_TYPE_C && dwAnimTagRuneC != (uint32)-1)
        dwAnimationTag = dwAnimTagRuneC;
    else if (eRuneType == RUNE_TYPE_D && dwAnimTagRuneD != (uint32)-1)
        dwAnimationTag = dwAnimTagRuneD;
    else if (eRuneType == RUNE_TYPE_E && dwAnimTagRuneE != (uint32)-1)
        dwAnimationTag = dwAnimTagRuneE;
    return dwAnimationTag;
}

// Original line 7986.
// Original line 7986.

BOOL PowerAlternatesAnims(ActorCommonData & tACD, AutoPowerDefinitionR pdefPower) {
    RuneType eRuneType = PowerGetRuneTypeForACD(tACD, pdefPower->tHeader.snoHandle);
    if (eRuneType != RUNE_TYPE_INVALID) {
        uint32 dwAnimationTag = sDetermineAnimationForRune(tACD, pdefPower, TAG_POWER_ANIMATION_TAG, TAG_POWER_ANIMATION_TAG_RUNE_A,
                                                           TAG_POWER_ANIMATION_TAG_RUNE_B, TAG_POWER_ANIMATION_TAG_RUNE_C,
                                                           TAG_POWER_ANIMATION_TAG_RUNE_D, TAG_POWER_ANIMATION_TAG_RUNE_E);
        if (dwAnimationTag != (uint32)PowerGeneralTagGetInt(pdefPower, TAG_POWER_ANIMATION_TAG))
            return FALSE;
    }
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_ALTERNATES_ANIMS, -1, -1, 0) > 0.000001f;
}

// Original line 8234.
// Original line 8234.
float PowerGetAnimationSpeedForPower(const ACDID idACD, AutoPowerDefinitionR pdefPower, const int32 nComboLevel) {
    float fSpeedFactor;
    SNO snoPower = pdefPower.GetSNO();
    ActorCommonData & tACD = ACDGet(idACD);
    if (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WALK))
        fSpeedFactor = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_MOVEMENT_SCALAR_TOTAL));
    else
        fSpeedFactor = PowerGetAttackSpeedAtLevel(tACD, pdefPower, -1, nComboLevel);
    return Math::Max(fSpeedFactor, 0.1f);
}

// Original line 8259.
// Original line 8259.

BOOL PowerShouldPlayAnimForPower(ACDID idACD, const Target & tTarget, SNO snoNewPower, SNO snoCurrentPower) {
    if (idACD == ACDID_INVALID)
        return TRUE;
    if (snoNewPower != snoCurrentPower)
        return TRUE;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
        return TRUE;
    uint32 dwAnimationTag = PowerDetermineAnimation(AutoPowerDefinitionR(snoNewPower), idACD, tTarget, FALSE);
    if (AnimPlayerGetCurrentAnimTag(*ptACD) != dwAnimationTag)
        return TRUE;

    return !PowerHasInfiniteAnimationTiming(*ptACD, AutoPowerDefinitionR(snoNewPower));
}

// Original line 8290.
WorldDistance PowerGetDistanceViaWorldPlace(const WorldPlace & placeStart, const WorldPlace & placeTarget, WorldDistance wdStartHeight, const WorldDistance wdStartRadius, WorldDistance wdTargetHeight, const WorldDistance wdTargetRadius, const WorldDistance wdLongReach) {
    WorldDistance wdZDist;
    wdStartHeight *= 0.8f;
    wdTargetHeight *= 0.8f;
    if (placeStart.wp.z < placeTarget.wp.z) {
        wdZDist = placeTarget.wp.z - placeStart.wp.z - wdStartHeight;
    } else {
        wdZDist = placeStart.wp.z - placeTarget.wp.z - wdTargetHeight;
    }
    wdZDist = Math::Max(wdZDist, 0.0f);
    WorldDistance wdSeparation = WorldPlaceXYPlaneDistance(placeStart, placeTarget);
    WorldDistance wdSpace = Math::Max(wdSeparation - wdStartRadius - wdTargetRadius - wdLongReach, 0.0f);
    return wdSpace > wdZDist ? wdSpace : wdZDist;
}

// Original line 8349.
SNO PowerSNOFromAttributeKey(const FastAttribKey & tKey) {
    SNO snoParam = KeyGetParam(tKey);
    if (snoParam == 0xfffff || !SNOFileExists(POWER_SNO, snoParam))
        snoParam = SNO_NONE;
    return snoParam;
}

// Original line 8554.
Attrib PowerAttributeFromRuneType(RuneType eRuneType) {
    switch (eRuneType) {
    case RUNE_TYPE_A: return ATTRIBUTE_RUNE_A;
    case RUNE_TYPE_B: return ATTRIBUTE_RUNE_B;
    case RUNE_TYPE_C: return ATTRIBUTE_RUNE_C;
    case RUNE_TYPE_D: return ATTRIBUTE_RUNE_D;
    case RUNE_TYPE_E: return ATTRIBUTE_RUNE_E;
    default: return ATTRIBUTE_NONE;
    }
}

#include "../WorldCommonCollision.h"
#include "../ActorCommonCollision.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"

#line 7831
uint32 PowerGetPassabilityBit(
    PowerPassabilityType ePassability)
{
    uint32 uBit;

    switch (ePassability)
    {
    case POWER_PASSABILITY_DURING:
        uBit = POWERBIT_PASSABILITY_DURING_ENABLED;
        break;
    case POWER_PASSABILITY_DESTINATION:
        uBit = POWERBIT_PASSABILITY_DEST_ENABLED;
        break;
    default:


        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
    return uBit;
}

#line 7713
BOOL PowerGetCollFlags(
    AutoPowerDefinitionR pdefPower,
    PowerPassabilityType ePassability,
    ActorCollisionFlags &tCollFlags)
{
    BOOL bSuccess = FALSE;
    if (pdefPower)
    {



        uint32 uBit = PowerGetPassabilityBit(ePassability);
        if (pdefPower->tPower.uFlags & (1 << uBit))
        {
            tCollFlags = pdefPower->tPower.arCollFlags[ePassability];
            bSuccess = TRUE;
        }
    }

    return bSuccess;
}

#line 7737
BOOL PowerFillCollParams(
    ACDID idACD,
    AutoPowerDefinitionR pdefPower,
    PowerPassabilityType ePassability,
    WorldCollParams &tCollParams)
{
    ActorCommonData *ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    ActorCollisionFlags tCollFlags;
    BOOL bSuccess = PowerGetCollFlags(pdefPower, ePassability, tCollFlags);
    if (bSuccess)
    {
        tCollParams.uObjectNoOverlapFlags = tCollFlags.uDynamicFlagsNoOverlap;
        tCollParams.uOneRequiredStaticFlags = tCollFlags.uStaticOneRequiredFlags;
        tCollParams.eTeam = ptACD->GetTeam();
        tCollParams.uSpecialFlags |= 1 << COLL_PARAMS_FROM_POWER;
    }

    return bSuccess;
}

// Raw DWARF records only Player.cpp:5247/5248; no recovered header declaration.
SNO PlayerGetPowerFallbackPower(const Player &tPlayer);

#line 3468
BOOL PowerUsesPowerFallbackQueue(
    AutoPowerDefinitionR pdefPower)
{
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_USES_POWER_FALLBACK_QUEUE);
}

#line 2102
static GameError sGameErrorFromResourceType(
    const ResourceType eResourceType)
{
    switch (eResourceType)
    {
    case RESOURCE_TYPE_HATRED:
        return GAMEERROR_POWER_NOT_ENOUGH_HATRED;
    case RESOURCE_TYPE_ARCANUM:
        return GAMEERROR_POWER_NOT_ENOUGH_ARCANUM;
    case RESOURCE_TYPE_FURY:
        return GAMEERROR_POWER_NOT_ENOUGH_FURY;
    case RESOURCE_TYPE_SPIRIT:
        return GAMEERROR_POWER_NOT_ENOUGH_SPIRIT;
    case RESOURCE_TYPE_DISCIPLINE:
        return GAMEERROR_POWER_NOT_ENOUGH_DISCIPLINE;
    case RESOURCE_TYPE_FAITH:
        return GAMEERROR_POWER_NOT_ENOUGH_FAITH;
    case RESOURCE_TYPE_ESSENCE:
        return GAMEERROR_POWER_NOT_ENOUGH_ESSENCE;
    default:
        return GAMEERROR_POWER_NOT_ENOUGH_MANA;
    }
}

#line 2136
SNO PowerPlayerFallbackIfNecessary(
    AutoPowerDefinitionR pdefPower,
    const ACDID idACDPlayer,
    GameError &eGameError,
    BOOL bCurrentlyChannelingPower)
{
    SNO snoPower = pdefPower.GetSNO();
    ActorCommonData &tACD = ACDGet(idACDPlayer);

    eGameError = GAMEERROR_NONE;
    if (!PowerIsCoolDownDone(tACD, snoPower) && !bCurrentlyChannelingPower)
    {
        eGameError = GAMEERROR_POWER_IN_COOLDOWN;
    }
    else
    {
        ResourceCheckResult eResult = PowerCheckResource(tACD, pdefPower, bCurrentlyChannelingPower);
        if (eResult == RESOURCE_CHECK_NEEDS_FREECAST)
        {
            eGameError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        }
        else if (eResult != RESOURCE_CHECK_OK)
        {
            eGameError = sGameErrorFromResourceType(PowerGetResourceType(ACDGet(idACDPlayer), pdefPower));
        }
    }

    if (eGameError != GAMEERROR_NONE)
    {
        if (PowerUsesPowerFallbackQueue(pdefPower))
        {
            SNO snoFallbackPower = PlayerGetPowerFallbackPower(*PlayerGetByACD(idACDPlayer));
            if (snoFallbackPower != SNO_NONE)
            {
                return snoFallbackPower;
            }
        }

        if (PowerCanTurnIntoBasicAttack(pdefPower, idACDPlayer))
        {
            return tACD.AttributeGetBasicAttack();
        }
    }

    return snoPower;
}

#include "../../Server/Powers/PowerAPI.h"
#include "../../Sigma/Memory/SigmaMemory.h"
// Raw DWARF records only PowerTags.cpp:1691/1701; no recovered header declarations.
uint32 PowerTagGetDamageMinTag(DamageType eDamageType);
uint32 PowerTagGetDamageDeltaTag(DamageType eDamageType);
// Raw DWARF records only PowerAttributes.cpp:17; no recovered header declaration.
float PowerAttributesGetFloat(PowerID idPower, FastAttribKey tKey);

#line 2367
void PowerGetDamageData(
    AutoPowerDefinitionR pdefPower,
    const ActorCommonData *ptACDOwner,
    PowerID idPower,
    PowerDamageData &tData,
    PowerAPIAttackPayload *ptPayload,
    BOOL bForceUseACDAttributes)
{




    SNO snoPower = pdefPower.GetSNO();




    uint32 nContactFrame = 0;
    AttribGroupID idFastAttrib = ATTRIBGROUPID_INVALID;


    Power *ptPower = NULL;
    if (idPower != POWERID_INVALID)
    {
        ptPower = PowerTryToGet(idPower);
    }

    if (ptACDOwner && bForceUseACDAttributes)
    {
        idFastAttrib = ptACDOwner->idFastAttrib;
    }
    else if (ptPower)
    {
        nContactFrame = ptPower->dwCurrentContactFrame;
        idFastAttrib = ptPower->idFastAttrib;
    }
    else if (ptACDOwner)
    {
        idFastAttrib = ptACDOwner->idFastAttrib;
    }






    float flBaseDamageScalar;

    if (ptPayload)
    {

        flBaseDamageScalar = ptPayload->flBaseDamageScalar;
    }
    else if (!bForceUseACDAttributes && ptPower)
    {
        flBaseDamageScalar = PowerGetFormulaValue(idPower, TAG_POWER_BASE_DAMAGE_SCALAR);
    }
    else
    {
        if (ptACDOwner)
        {
            flBaseDamageScalar = PowerGetFormulaValue(ptACDOwner, pdefPower, TAG_POWER_BASE_DAMAGE_SCALAR, -1, ATTRIBGROUPID_INVALID, nContactFrame);

            float flSidekickBonus = ptACDOwner->AttributesGetFloat(ATTRIBUTE_DAMAGE_SIDEKICK_MULTIPLIER);
            if (flSidekickBonus > 0.0f)
            {
                flBaseDamageScalar *= flSidekickBonus;
            }
        }
        else { flBaseDamageScalar = 1.0f; }
    }
    SigmaMemoryClear(&tData, sizeof(tData));

    for (int32 nDamageType = 0; nDamageType < NUM_DAMAGE_TYPES; ++nDamageType)
    {
        ElementDamage &tDmg = tData.tElementDamage[nDamageType];


        if (ptPower && !bForceUseACDAttributes)
        {
            tDmg.flTotalMin = flBaseDamageScalar * PowerAttributesGetFloat(idPower, MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, nDamageType));
            tDmg.flTotalDelta = flBaseDamageScalar * PowerAttributesGetFloat(idPower, MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA_TOTAL, nDamageType));

            tDmg.flTotalMin += PowerGetFormulaValue(idPower, PowerTagGetDamageMinTag(nDamageType));
            tDmg.flTotalDelta += PowerGetFormulaValue(idPower, PowerTagGetDamageDeltaTag(nDamageType));

            tDmg.flTotalMin += tData.tElementDamage[DAMAGE_TYPE_PHYSICAL].flTotalMin * PowerAttributesGetFloat(idPower, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType));
            tDmg.flTotalDelta += tData.tElementDamage[DAMAGE_TYPE_PHYSICAL].flTotalDelta * PowerAttributesGetFloat(idPower, MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType));
        }

        else
        {

            tDmg.flTotalMin = flBaseDamageScalar * ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_MIN_TOTAL, nDamageType));
            tDmg.flTotalDelta = flBaseDamageScalar * ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_DELTA_TOTAL, nDamageType));

            tDmg.flTotalMin += PowerGetFormulaValue(ptACDOwner, pdefPower, PowerTagGetDamageMinTag(nDamageType), -1, ATTRIBGROUPID_INVALID, nContactFrame);
            tDmg.flTotalDelta += PowerGetFormulaValue(ptACDOwner, pdefPower, PowerTagGetDamageDeltaTag(nDamageType), -1, ATTRIBGROUPID_INVALID, nContactFrame);

            tDmg.flTotalMin += tData.tElementDamage[DAMAGE_TYPE_PHYSICAL].flTotalMin * ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType));
            tDmg.flTotalDelta += tData.tElementDamage[DAMAGE_TYPE_PHYSICAL].flTotalDelta * ptACDOwner->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_DAMAGE_TYPE_PERCENT_BONUS, nDamageType));
        }
    }







































    PowerPayloadGetBonusMultipliers(idFastAttrib, snoPower, idPower, PlayerGetPlayerClass(ptACDOwner), tData.flPrimaryAttributeBonusMultiplier, tData.flClassBonusMultiplier, tData.flSkillsBonusMultiplier);
}

#line 5708
void GameErrorFromPowerFailureReasons(
    GameErrorData &tData,
    const uint32 dwFailureReason,
    const ACDID idACD,
    AutoPowerDefinitionR pdefPower,
    const BOOL fItemPower)
{
    if (dwFailureReason & ((1 << PS_NEED_COOLDOWN_BIT) | (1 << PS_NEED_GENERAL_COOLDOWN_BIT)))
    {
        if (fItemPower)
        {
            tData.eError = GAMEERROR_ITEM_NOT_READY;
        }

        else
        {
            tData.eError = GAMEERROR_POWER_IN_COOLDOWN;


            if (PowerGetTemplate(pdefPower, ACDID_INVALID) == SECONDARY_ANIMATION_POWER)
            {
                tData.dwVal = PowerGetCastingCooldown(pdefPower, ACDGet(idACD));
            }
        }
        return;
    }

    if (dwFailureReason & ((1 << PS_ONLY_IN_TOWN_PORTAL_AREA) | (1 << PS_TARGET_NOT_IN_TELEPORTABLE_AREA)))
    {

        tData.eError = GAMEERROR_POWER_UNUSABLE_IN_AREA;
        return;
    }
    if (dwFailureReason & (1 << PS_UNUSABLE_DURING_BOSS_ENCOUNTER))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_DURING_BOSS_ENCOUNTER;
        return;
    }
    if (dwFailureReason & (1 << PS_NOT_IN_COMBAT))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_IN_COMBAT;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_TARGET))
    {
        tData.eError = GAMEERROR_POWER_NEED_TARGET;
        return;
    }
    if (dwFailureReason & (1 << PS_INVALID_TARGET))
    {
        tData.eError = GAMEERROR_POWER_INVALID_TARGET;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_HEALTH_BIT))
    {
        tData.eError = GAMEERROR_POWER_NOT_ENOUGH_HEALTH;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_CHARGES_BIT))
    {
        tData.eError = GAMEERROR_POWER_NOT_ENOUGH_CHARGES;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_RESOURCE_BIT))
    {
        tData.eError = sGameErrorFromResourceType(PowerGetCurrentResourceType(ACDGet(idACD), pdefPower, NULL, NULL));
        return;
    }
    if (dwFailureReason & (1 << PS_UNABLE_TO_FREECAST_BIT))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_SKILLPOINT))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        return;
    }
    if (dwFailureReason & (1 << PS_NEED_CORPSES))
    {
        tData.eError = GAMEERROR_POWER_NEED_CORPSES;
        return;
    }
    if (dwFailureReason & (1 << PS_WRONG_WEAPON))
    {
        tData.eError = GAMEERROR_POWER_WRONG_WEAPON;
        tData.dwVal = PowerGeneralTagGetGBID(pdefPower, TAG_POWER_ITEM_TYPE_REQUIREMENT);
        return;
    }
    if (dwFailureReason & (1 << PS_MISSING_ITEM))
    {
        tData.eError = GAMEERROR_POWER_MISSING_ITEM;
        return;
    }
    if (dwFailureReason & (1 << PS_NOT_IN_TOWN))
    {
        tData.eError = GAMEERROR_POWER_IN_TOWN;
        return;
    }
    if (dwFailureReason & (1 << PS_NOT_CLOSE_ENOUGH_BIT))
    {
        tData.eError = GAMEERROR_POWER_TARGET_TOO_FAR;
        return;
    }
    if (dwFailureReason & (1 << PS_SILENCED))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        return;
    }
    if (dwFailureReason & (1 << PS_IMMOBILIZED))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        return;
    }
    if (dwFailureReason & (1 << PS_CASTER_IS_STUNNED))
    {
        tData.eError = GAMEERROR_POWER_UNUSABLE_GENERIC;
        return;
    }

    tData.eError = GAMEERROR_NONE;

}

#include "../Nav/NavZone.h"
// Raw DWARF records only NavigationMesh.cpp:958/1937/2000/3180/3383; no recovered header declarations.
void NavMeshGetCenterWorldSpace(const NavMeshHandle &tNavMeshHandle, Vector3D &wpCenter);
void NavMeshRollDataInitialize(NavMeshRollData &tRollData);
BOOL NavMeshRollMarble(NavMeshRollData &tRollData);
uint32 NavMeshGetStaticFlagsForPassability(PassabilityType ePassability);
BOOL NavMeshCallBackPassabilityTypeCanEnter(void *pCallbackData, const NavMeshHandle &tNavMeshHandle);

#line 5205
BOOL PowerRollToTargetNavPlace(
    ACDID idACD,
    const Target &tTarget,
    const PassabilityType ePassabilityType,
    const BOOL fIgnoreDestinationHeight,
    NavPlace &tNavPlace)
{
    ActorCommonData &tACD = ACDGet(idACD);
    if (!tACD.IsInWorld())
    {
        return FALSE;
    }

    SWorldID idSWorldSource = tACD.GetSWorldID();




    WorldPlace placeStart = tACD.GetPlace();
    NavCellHandle hNavCellTest;
    if (NavCellHandleFromWorldPlace(placeStart, hNavCellTest))
    {
        const NavZoneInstance *ptNavZoneInstance = NavCellHandleToZoneInstance(hNavCellTest);
        uint32 uStaticOneRequiredFlags = NavMeshGetStaticFlagsForPassability(ePassabilityType);
        NavCellModifyForRoundedCorners(ptNavZoneInstance, hNavCellTest, placeStart.wp.As2D(), uStaticOneRequiredFlags);
    }

    NavMeshHandle tNavMeshHandleSource;
    ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandleSource, FALSE, FALSE);

    if (!NavMeshHandleIsActivated(tNavMeshHandleSource))
    {


        PowerGetNavPlaceFromACD(&tACD, tNavPlace);
        return FALSE;
    }

    WorldPlace placeRollDestination;

    switch (tTarget.eType)
    {
    case TT_NONE:
        PowerGetNavPlaceFromACD(&tACD, tNavPlace);
        return FALSE;

    case TT_POINT:
        placeRollDestination = tTarget.tPlace;
        break;

    case TT_ACTOR:
    {
        ActorCommonData *ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (!ptACDTarget)
        {
            return FALSE;
        }

        NavMeshHandle tNavMeshHandleTarget;
        ACDGetNavMeshHandle(*ptACDTarget, tNavMeshHandleTarget);

        if (NavMeshHandleIsNone(tNavMeshHandleTarget))
        {
            return FALSE;
        }


        if (!NavMeshHandleIsActivated(tNavMeshHandleTarget))
        {
            PowerGetNavPlaceFromACD(ptACDTarget, tNavPlace);
            return FALSE;
        }



        NavMeshGetCenterWorldSpace(tNavMeshHandleTarget, placeRollDestination.wp);

        placeRollDestination.idSWorld = ptACDTarget->GetSWorldID();
        break;
    }

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    if (placeRollDestination.idSWorld != idSWorldSource)
    {
        PowerGetNavPlaceFromACD(&tACD, tNavPlace);
        return FALSE;
    }



    Vector3D wpRollStart;
    NavMeshGetCenterWorldSpace(tNavMeshHandleSource, wpRollStart);


    uint32 dwMarbleRollFlags = 1 << MARBLE_ROLL_FAT;
    if (fIgnoreDestinationHeight)
        dwMarbleRollFlags |= 1 << MARBLE_ROLL_IGNORE_HEIGHT_ON_DEST;




    NavMeshRollData tRollData;
    NavMeshRollDataInitialize(tRollData);

    tRollData.idACD = idACD;
    tRollData.pCallbackFunction = NavMeshCallBackPassabilityTypeCanEnter;
    tRollData.pCallbackData = (void *)(intptr_t)ePassabilityType;
    tRollData.tNavMeshHandleStartCenter = tNavMeshHandleSource;
    tRollData.wpStart = wpRollStart;
    tRollData.wpDest = placeRollDestination.wp;
    tRollData.dwMarbleRollFlags = dwMarbleRollFlags;

    BOOL bPassable = NavMeshRollMarble(tRollData);

    tNavPlace.tNavMeshHandle = tRollData.tNavMeshHandleEndCenter;
    tNavPlace.tPlace.wp = tRollData.wpEnd;
    tNavPlace.tPlace.idSWorld = idSWorldSource;

    return bPassable;
}

#include "../VisualInventory.h"
#include <string.h>
// Raw DWARF records only FastAttribute.cpp:2484; no recovered header declaration.
void FastAttributeGetAttributeAndParamFromString(const char *szString, Attrib &eAttrib, int32 &nParam);

// NON_MATCHING: 244 bytes both; 6/61 differ: the original keeps two strlen calls (the zero test folded to an early ldrb at 8408:22), ours CSEs them into one; no extra statement found that separates them.
#line 8364
static int32 sGetGenericAttributeParameter(
    const PowerDefinition *pdefPower,
    uint32 dwPowerTag,
    Attrib eAttrib,
    uint32 nContactFrame)
{

    if (eAttrib == ATTRIBUTE_NONE)
    {
        return -1;
    }

    Attrib eAttribFound = ATTRIBUTE_NONE;
    int32 nParameter = -1;

    const CTagMap *pTagMap;
    BOOL bIsPVP = GameTypeIsPvP();

    if (TagMapGetTagInfo(pdefPower->tPower.hGeneralTagMap, dwPowerTag))
    {
        if (bIsPVP && TagMapTagExists(pdefPower->tPower.hPVPGeneralTagMap, dwPowerTag))
        {
            pTagMap = pdefPower->tPower.hPVPGeneralTagMap;
        }
        else
        {
            pTagMap = pdefPower->tPower.hGeneralTagMap;
        }
    }

    else
    {
        if (bIsPVP && TagMapTagExists(pdefPower->tPower.hPVPContactTagMap[nContactFrame], dwPowerTag))
        {
            pTagMap = pdefPower->tPower.hPVPContactTagMap[nContactFrame];
        }
        else
        {
            pTagMap = pdefPower->tPower.hContactTagMap[nContactFrame];
        }
    }

    LPCSTR szFormula = TagMapGetFormulaString(pTagMap, dwPowerTag);
    const char firstChar = szFormula[0];

    if (strlen(szFormula) > 1)
    {
        FastAttributeGetAttributeAndParamFromString(szFormula, eAttribFound, nParameter);
    }

    if (KeyGetParamType(eAttrib) == APT_NONE)
    {








        return -1;
    }

    else if (firstChar == '\0')
    {




        return -1;
    }

    else if (eAttrib != eAttribFound) // columns 8437:21/24
    {




        return -1;
    }
    else { // return nParameter at 8446:16 (nested)
        return nParameter;
    }
}

#line 8452
void PowerApplyGenericAttributeBuffAttributes(
    ActorCommonData &tACD,
    AutoPowerDefinitionR pdefPower,
    AttribGroupID idFastAttrib)
{
    Attrib eAttrib0 = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_0, 0);
    Attrib eAttrib1 = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_1, 0);
    Attrib eAttrib2 = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_2, 0);
    Attrib eAttrib3 = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_3, 0);
    float flAttributeValue0 = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_0, -1, ATTRIBGROUPID_INVALID, 0);
    float flAttributeValue1 = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_1, -1, ATTRIBGROUPID_INVALID, 0);
    float flAttributeValue2 = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_2, -1, ATTRIBGROUPID_INVALID, 0);
    float flAttributeValue3 = PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_FORMULA_3, -1, ATTRIBGROUPID_INVALID, 0);

    int32 nParam0 = sGetGenericAttributeParameter(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_0_WITH_PARAM, eAttrib0, 0);
    int32 nParam1 = sGetGenericAttributeParameter(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_1_WITH_PARAM, eAttrib1, 0);
    int32 nParam2 = sGetGenericAttributeParameter(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_2_WITH_PARAM, eAttrib2, 0);
    int32 nParam3 = sGetGenericAttributeParameter(pdefPower, TAG_POWER_GENERIC_BUFF_ATTRIBUTE_3_WITH_PARAM, eAttrib3, 0);

    ItemLabel eItemLabelRequired = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_WEAPON_CLASS_REQUIREMENT, 0);
    ItemLabel eItemLabelRequiredOffhand = PowerContactTagGetInt(pdefPower, TAG_POWER_GENERIC_BUFF_WEAPON_CLASS_REQUIREMENT_OFFHAND, 0);

    if (eItemLabelRequired != ITL_INVALID || eItemLabelRequiredOffhand != ITL_INVALID)
    {
        if (ACDIsPlayer(tACD.id))
        {
            VisualInventorySlot eVisualSlotLeftHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_LEFT_HAND);
            VisualInventorySlot eVisualSlotRightHand = VisualInventorySlotFromInventorySlot(SLOT_PLAYER_RIGHT_HAND);

            GBID gbidItemLeft = GBID_NONE;
            GBID gbidItemRight = GBID_NONE;

            const VisualInventory *ptVisualInventory = VisualInventoryGet(tACD.id);
            if (ptVisualInventory)
            {
                const VisualItem &tVisualItemLeftHand = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotLeftHand];
                const VisualItem &tVisualItemRightHand = ptVisualInventory->tEquipmentCurrent.tVisualItem[eVisualSlotRightHand];
                gbidItemLeft = tVisualItemLeftHand.gbidItem;
                gbidItemRight = tVisualItemRightHand.gbidItem;

                if ((eItemLabelRequired != ITL_INVALID && !ItemHasLabel(gbidItemLeft, eItemLabelRequired)) ||
                    (eItemLabelRequiredOffhand != ITL_INVALID && !ItemHasLabel(gbidItemRight, eItemLabelRequiredOffhand)))
                {
                    flAttributeValue0 = flAttributeValue1 = flAttributeValue2 = flAttributeValue3 = 0.0f;
                }
            }

        }
    }



    if (eAttrib0 > AXEBADDATA && eAttrib0 < NUM_ATTRIBUTES)
    {
        if (KeyGetDataType(eAttrib0) == ATTRIBUTEDT_FLOAT)
        {
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(eAttrib0, nParam0), flAttributeValue0);
        }
        else
        {
            FastAttribSetValueInt(idFastAttrib, MakeAttribKey(eAttrib0, nParam0), Math::FloatToInt(flAttributeValue0));
        }
    }

    if (eAttrib1 > AXEBADDATA && eAttrib1 < NUM_ATTRIBUTES)
    {
        if (KeyGetDataType(eAttrib1) == ATTRIBUTEDT_FLOAT)
        {
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(eAttrib1, nParam1), flAttributeValue1);
        }
        else
        {
            FastAttribSetValueInt(idFastAttrib, MakeAttribKey(eAttrib1, nParam1), Math::FloatToInt(flAttributeValue1));
        }
    }
    if (eAttrib2 > AXEBADDATA && eAttrib2 < NUM_ATTRIBUTES)
    {
        if (KeyGetDataType(eAttrib2) == ATTRIBUTEDT_FLOAT)
        {
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(eAttrib2, nParam2), flAttributeValue2);
        }
        else
        {
            FastAttribSetValueInt(idFastAttrib, MakeAttribKey(eAttrib2, nParam2), Math::FloatToInt(flAttributeValue2));
        }
    }
    if (eAttrib3 > AXEBADDATA && eAttrib3 < NUM_ATTRIBUTES)
    {
        if (KeyGetDataType(eAttrib3) == ATTRIBUTEDT_FLOAT)
        {
            FastAttribSetValueFloat(idFastAttrib, MakeAttribKey(eAttrib3, nParam3), flAttributeValue3);
        }
        else
        {
            FastAttribSetValueInt(idFastAttrib, MakeAttribKey(eAttrib3, nParam3), Math::FloatToInt(flAttributeValue3));
        }
    }
}

#include "../../Server/AI/ActorAI.h"
#include "../TeamCommon.h"
// Exhaustive cached DWARF records only CPP homes for these four functions, not header declarations.
BOOL ACDPetCanTarget(ActorCommonData & tACDPet, ActorCommonData & tACDTarget); // ActorAI.cpp:1731
BOOL ACDCanBeAffectedByPowers(const ACDID idACD); // ActorAI.cpp:1147
BOOL PowerHasAffectedACD(PowerID idPower, const PowerDefinition * pdefPower, const ACDID idACDTarget, const ACDID idACDSource); // Powers.cpp:4694
BOOL PowerTargetNeutral(PowerID idPower); // Powers.cpp:1193

#line 2976 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerCanTarget(
    const PowerID idPower,
    const ACDID idACDTarget,
    const ACDID idACDSource)
{



    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (!ptACDTarget)
        return FALSE;




    if (!ptACDTarget->IsTargetable())
        return FALSE;



    if (idPower == POWERID_INVALID)
        return TRUE;





    const Power * ptPower = PowerGet(idPower);
    const TargetCriteria & tTargetCriteria = ptPower->tTargetCriteria;


    ActorCommonData * ptACD = PowerGetOwner(ptPower);
    if (ptACD) {



        if (ptACD->IsPet()) {

            if (!ACDPetCanTarget(*ptACD, *ptACDTarget))
                return FALSE;
        }
    }





    if (!ACDCanBeAffectedByPowers(ptACDTarget->id))
        return FALSE;



    Team eTeamPower = PowerGetTeam(idPower);
    Team eTeamTarget = ptACDTarget->GetTeam();


    if (ptACDTarget->IsWreckableObject()) {

        if (ActorHasBeenKilled((ActorID)ACDGetOwner(ptACDTarget->id)))
            return FALSE;


        {

            ScopedActorDefinitionLockR pdefActor(ptACDTarget->snoActor);
            SNO snoRequiredPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_DESTROYED_BY_POWER);
            if (snoRequiredPower != SNO_NONE &&
                snoRequiredPower != ptPower->snoPower) {
                return FALSE;
            }

        }


        if (tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_WRECKABLES))
            return FALSE;





        if ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_ENEMIES)) && eTeamPower != eTeamTarget)
            return TRUE;





        if (!(tTargetCriteria.dwTeamRelationshipFlags & ((1 << TCTR_AFFECTS_ENEMIES) | (1 << TCTR_AFFECTS_ALLIES))) && eTeamPower != eTeamTarget)
            return TRUE;
    }





    BOOL bIsPlayer = ACDIsPlayer(ptACDTarget->id);
    if (!bIsPlayer && (tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_NON_PLAYERS)))
        return FALSE;



    if (tTargetCriteria.eLifeStatus == TCLS_NOT_DEAD && ActorHasBeenKilled((ActorID)ACDGetOwner(ptACDTarget->id)))
        return FALSE;



    BOOL bAlive = FALSE;
    if (ptACDTarget->IsMonster() || bIsPlayer || ptACDTarget->IsWreckableObject() || ptACDTarget->IsNecroCorpse()) {

        bAlive = ptACDTarget->IsAlive();
    }

    if (tTargetCriteria.eLifeStatus == TCLS_ALIVE_ONLY && !bAlive)
        return FALSE;



    if (tTargetCriteria.eLifeStatus == TCLS_DEAD_ONLY && !ActorHasBeenKilled((ActorID)ACDGetOwner(ptACDTarget->id)))
        return FALSE;



    {
        ScopedPowerDefinitionLockR pdefPower(ptPower->snoPower);


        if (PowerHasAffectedACD(idPower, pdefPower, ptACDTarget->id, idACDSource))
            return FALSE;



        if (PowerContactTagGetBool(pdefPower, TAG_POWER_TARGET_NORMAL_MONSTERS_ONLY, ptPower->dwCurrentContactFrame) && !ptACDTarget->IsNormalMonster())
            return FALSE;



        if (PowerContactTagGetBool(pdefPower, TAG_POWER_TARGET_UNDEAD_MONSTERS_ONLY, ptPower->dwCurrentContactFrame) && !ptACDTarget->IsUndeadMonster())
            return FALSE;



        if (PowerContactTagGetBool(pdefPower, TAG_POWER_TARGET_IGNORE_LARGE_MONSTERS, ptPower->dwCurrentContactFrame) && ptACDTarget->IsLargeMonster())
            return FALSE;



        if (PowerContactTagGetBool(pdefPower, TAG_POWER_TARGET_NECRO_CORPSES_ONLY, ptPower->dwCurrentContactFrame) && !ptACDTarget->IsNecroCorpse())
            return FALSE;



        if (ptACDTarget->IsNecroCorpse()) {

            ACDID idACDCorpseSkillSource = idACDSource;
            if (idACDCorpseSkillSource == ACDID_INVALID) {

                idACDCorpseSkillSource = ptPower->idACD;
            }
            ActorCommonData * ptACDSource = ACDTryToGet(idACDCorpseSkillSource);
            if (!ptACDSource || !ptACDSource->CanTargetNecroCorpse(idACDTarget))
                return FALSE;
        }


    }


    if (eTeamPower == TEAM_INVALID)
        return FALSE;



    if (eTeamTarget != TEAM_INVALID) {

        BOOL bTeamsAreAllied = FALSE;
        if (ptACD) {

            bTeamsAreAllied = ACDIsFriendly(*ptACD, *ptACDTarget, TRUE);
        }
        else {

            bTeamsAreAllied = !ptACDTarget->IsCharmed() && TeamsAreFriendly(eTeamPower, eTeamTarget);
        }

        if (ptPower->dwFlags & (1 << PF_WAS_REFLECTED)) {


            if (ptACD && ACDIsPlayer(ptACD->id) && ptACDTarget->id != ptACD->id)
                return FALSE;





            bTeamsAreAllied = FALSE;
            ActorCommonData * ptACDSource = ACDTryToGet(idACDSource);
            if (ptACDSource) {



                bTeamsAreAllied = ACDIsFriendly(*ptACDSource, *ptACDTarget, TRUE);
            }
        }


        if (tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_NECRO_CORPSES_ONLY)) {

            if (!ptACDTarget->IsNecroCorpse() || ActorFlaggedForDeletion((ActorID)ACDGetOwner(ptACDTarget->id)))
                return FALSE;
        }
        else {

            if (!(((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_SCENERY)) && (eTeamTarget == TEAM_SCENERY || eTeamTarget == TEAM_INVALID)) ||
                  ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_ENEMIES)) && !bTeamsAreAllied) ||
                  ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_AFFECTS_ALLIES)) && bTeamsAreAllied)))
                return FALSE;
        }


        if ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_CASTER)) && ptACDTarget == ptACD)
            return FALSE;




        if ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_PETS)) &&
            ptACDTarget->IsPet() &&
            ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_PLAYERS)) ||
             !ptACDTarget->AttributesGetInt(ATTRIBUTE_IS_PLAYER_DECOY)))
            return FALSE;



        if ((tTargetCriteria.dwTeamRelationshipFlags & (1 << TCTR_IGNORES_PLAYERS)) && bIsPlayer)
            return FALSE;
    }
    else {





        if (!PowerTargetNeutral(idPower))
            return FALSE;
    }




    NavMeshHandle tTargetHandle;
    ACDGetNavMeshHandle(*ptACDTarget, tTargetHandle);
    if (NavMeshHandleIsNone(tTargetHandle)) {

        ActorCommonData * ptACDPowerOwner = PowerGetOwner(idPower);
        // The owner lookup survives; any erased diagnostic consuming this local is unknown.
        return FALSE;
    }






    return TRUE;
}

#line 7840 "X/Common/Powers/PowersCommon.cpp"
static void sDetermineAnimationForTurning(
    ActorCommonData & tACD,
    const PowerDefinition * pdefPower,
    const Target & tTarget,
    uint32 & dwAnimTag,
    uint32 dwTagLeft,
    uint32 dwTagRight)
{
    if (!tTarget.IsInWorld() || tTarget.IsInvalidForSource(&tACD))
        return;



    Angle aThreshold = Math::DegToAngle(PowerGeneralTagGetFloat(pdefPower, TAG_POWER_ANIMATION_TURN_THRESHOLD));
    if (aThreshold > 0.0f) {

        WorldPlace placeCurrent = tACD.GetPlace();
        WorldPlace placeTarget;
        tTarget.GetPlace(placeTarget);

        Vector3D myFacing = tACD.GetForward();
        Vector3D meToTarget = placeTarget.wp - placeCurrent.wp;
        meToTarget.Normalize();
        float flDotProduct = Vector3D::DotProduct(meToTarget, myFacing);


        if (flDotProduct < Math::Cos(aThreshold)) {

            float flSign = Vector3D::CrossProduct(myFacing, meToTarget).z;
            uint32 dwAnimTagLeft = PowerGeneralTagGetInt(pdefPower, dwTagLeft);
            uint32 dwAnimTagRight = PowerGeneralTagGetInt(pdefPower, dwTagRight);

            uint32 dwAnimTagResult = flSign > 0.0f ? dwAnimTagLeft : dwAnimTagRight;
            if (AnimPlayerAnimExists(tACD, dwAnimTagResult))
                dwAnimTag = dwAnimTagResult;
        }
    }
}







static void sDetermineEvadeAnimation(
    ActorCommonData & tACD,
    const Target & tTarget,
    uint32 & dwAnimTag)
{

    if (dwAnimTag != TAG_ACTOR_EVADE)
        return;



    if (!tTarget.IsInWorld() || tTarget.IsInvalidForSource(&tACD))
        return;



    ScopedGlobalDefinitionLockR pdefGlobal;
    Angle aThreshold = Math::DegToAngle(pdefGlobal->flEvadeBackAngle);
    if (aThreshold > 0.0f) {

        float flCosAngle = Math::Cos(aThreshold);

        WorldPlace placeCurrent = tACD.GetPlace();
        WorldPlace placeTarget;
        tTarget.GetPlace(placeTarget);

        Vector3D myFacing = tACD.GetForward();
        Vector3D meToTarget = placeTarget.wp - placeCurrent.wp;
        meToTarget.Normalize();

        float flDotProduct = Vector3D::DotProduct(meToTarget, myFacing);
        if (flDotProduct < -flCosAngle) {
            dwAnimTag = TAG_ACTOR_EVADE_BACK;

            if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                dwAnimTag = TAG_ACTOR_EVADE;
        } else {




            float flCrossProduct = Vector3D::CrossProduct(myFacing, meToTarget).z;
            if (flCrossProduct < -flCosAngle) {
                dwAnimTag = TAG_ACTOR_EVADE_RIGHT;

                if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                    dwAnimTag = TAG_ACTOR_EVADE;
            }


            else if (flCrossProduct > flCosAngle) {
                dwAnimTag = TAG_ACTOR_EVADE_LEFT;

                if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                    dwAnimTag = TAG_ACTOR_EVADE;
            }
        }
    }




    if (ACDIsInTown(tACD)) {

        switch (dwAnimTag) {
        case TAG_ACTOR_EVADE:
            dwAnimTag = TAG_ACTOR_TOWN_EVADE;
            if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                dwAnimTag = TAG_ACTOR_EVADE;
            break;


        case TAG_ACTOR_EVADE_BACK:
            dwAnimTag = TAG_ACTOR_TOWN_EVADE_BACK;
            if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                dwAnimTag = TAG_ACTOR_EVADE_BACK;
            break;


        case TAG_ACTOR_EVADE_RIGHT:
            dwAnimTag = TAG_ACTOR_TOWN_EVADE_RIGHT;
            if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                dwAnimTag = TAG_ACTOR_EVADE_RIGHT;
            break;


        case TAG_ACTOR_EVADE_LEFT:
            dwAnimTag = TAG_ACTOR_TOWN_EVADE_LEFT;
            if (!AnimPlayerAnimExists(tACD, dwAnimTag))
                dwAnimTag = TAG_ACTOR_EVADE_LEFT;
            break;

        }
    }
}
































// SignedAngularDifference(aFacing, aToTarget): original fsub s8(aToTarget)-s0(aFacing); body is b - a (MathGeneral.inl)
uint32 PowerDetermineAnimation(
    AutoPowerDefinitionR pdefPower,
    ACDID idACDOwner,
    const Target & tTarget,
    BOOL bUseAlternateAnim)
{
    SNO snoPower = pdefPower.GetSNO();
    uint32 dwAnimationTag = TAG_ACTOR_ANIMATION_INVALID;
    BOOL fOperateGizmo = PowerIsGizmo(pdefPower);
    if (fOperateGizmo) {



        const ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (ptACDTarget) {

            dwAnimationTag = ACDGizmoGetAnimation(*ptACDTarget);
        }
    } else {






        ActorCommonData & tACDOwner = ACDGet(idACDOwner);


        dwAnimationTag = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ANIMATION_TAG);


        sDetermineAnimationForTurning(tACDOwner, pdefPower, tTarget, dwAnimationTag,
            TAG_POWER_ANIMATION_TAG_TURN_LEFT, TAG_POWER_ANIMATION_TAG_TURN_RIGHT);





        sDetermineEvadeAnimation(tACDOwner, tTarget, dwAnimationTag);




        RuneType eRuneType = PowerGetRuneTypeForACD(tACDOwner, snoPower);
        BOOL bHasRuneAnim = FALSE;
        if (eRuneType != RUNE_TYPE_INVALID) {

            dwAnimationTag = sDetermineAnimationForRune(tACDOwner, pdefPower,
                TAG_POWER_ANIMATION_TAG,
                TAG_POWER_ANIMATION_TAG_RUNE_A,
                TAG_POWER_ANIMATION_TAG_RUNE_B,
                TAG_POWER_ANIMATION_TAG_RUNE_C,
                TAG_POWER_ANIMATION_TAG_RUNE_D,
                TAG_POWER_ANIMATION_TAG_RUNE_E);

            bHasRuneAnim = dwAnimationTag != (uint32)PowerGeneralTagGetInt(pdefPower, TAG_POWER_ANIMATION_TAG);
        }




        if (bUseAlternateAnim) {

            dwAnimationTag = PowerGeneralTagGetInt(pdefPower, TAG_POWER_ANIMATION_TAG_2);
        }


        BOOL bUsesMainHandOnly = PowerUsesMainHandOnly(pdefPower, 0);
        if (PowerIsComboPower(pdefPower)) {

            int32 nComboLevel = tACDOwner.GetComboLevel();
            bUsesMainHandOnly = PowerUsesMainHandOnly(pdefPower, nComboLevel);
            if (nComboLevel == 0) {

                dwAnimationTag = sDetermineAnimationForRune(tACDOwner, pdefPower,
                    TAG_POWER_COMBO_ANIMATION_1,
                    TAG_POWER_COMBO_ANIMATION_1_RUNE_A,
                    TAG_POWER_COMBO_ANIMATION_1_RUNE_B,
                    TAG_POWER_COMBO_ANIMATION_1_RUNE_C,
                    TAG_POWER_COMBO_ANIMATION_1_RUNE_D,
                    TAG_POWER_COMBO_ANIMATION_1_RUNE_E);
                if (dwAnimationTag != (uint32)PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_ANIMATION_1))
                    bHasRuneAnim = TRUE;



            } else if (nComboLevel == 1) {

                dwAnimationTag = sDetermineAnimationForRune(tACDOwner, pdefPower,
                    TAG_POWER_COMBO_ANIMATION_2,
                    TAG_POWER_COMBO_ANIMATION_2_RUNE_A,
                    TAG_POWER_COMBO_ANIMATION_2_RUNE_B,
                    TAG_POWER_COMBO_ANIMATION_2_RUNE_C,
                    TAG_POWER_COMBO_ANIMATION_2_RUNE_D,
                    TAG_POWER_COMBO_ANIMATION_2_RUNE_E);
                if (dwAnimationTag != (uint32)PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_ANIMATION_2))
                    bHasRuneAnim = TRUE;




            } else if (nComboLevel == 2) {

                dwAnimationTag = sDetermineAnimationForRune(tACDOwner, pdefPower,
                    TAG_POWER_COMBO_ANIMATION_3,
                    TAG_POWER_COMBO_ANIMATION_3_RUNE_A,
                    TAG_POWER_COMBO_ANIMATION_3_RUNE_B,
                    TAG_POWER_COMBO_ANIMATION_3_RUNE_C,
                    TAG_POWER_COMBO_ANIMATION_3_RUNE_D,
                    TAG_POWER_COMBO_ANIMATION_3_RUNE_E);
                if (dwAnimationTag != (uint32)PowerGeneralTagGetInt(pdefPower, TAG_POWER_COMBO_ANIMATION_3))
                    bHasRuneAnim = TRUE;

            }
        }


        if (!bUsesMainHandOnly) {

            if (ACDIsDualWielding(tACDOwner, TRUE)) {



                if (PowerAffectedByDualWield(pdefPower) &&
                    snoPower != GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_BOTHHAND) &&
                    !PowerIsComboPower(pdefPower) &&
                    !PowerAlternatesAnims(tACDOwner, pdefPower)) {

                    DualWieldAttackType eAttackType = tACDOwner.AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HAND));

                    if (tACDOwner.AttributesGetInt(FastAttribKey(ATTRIBUTE_DUALWIELD_HANDS_SWAPPED))) {

                        eAttackType = eAttackType == DUALWIELD_ATTACK_MAIN ? DUALWIELD_ATTACK_OFFHAND : DUALWIELD_ATTACK_MAIN;
                    }









                    if (eAttackType == DUALWIELD_ATTACK_OFFHAND && !bHasRuneAnim) {

                        dwAnimationTag = PowerGeneralTagGetInt(pdefPower, TAG_POWER_OFFHAND_ANIMATION_TAG);
                    }
                }
            }
            else if (ACDIsPlayer(tACDOwner.id) &&
                     (snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT) || snoPower == GlobalSNOGet(GLOBALSNO_POWERS_WEAPON_MELEE_INSTANT_OFFHAND)) &&
                     tTarget.IsInWorld() && !tTarget.IsInvalidForSource(&tACDOwner)) {



                WorldPlace placeCurrent = tACDOwner.GetPlace();
                WorldPlace placeDest;
                tTarget.GetPlace(placeDest);
                Angle aToTarget = Math::ATan(placeDest.wp.y - placeCurrent.wp.y, placeDest.wp.x - placeCurrent.wp.x);
                Angle aFacing = tACDOwner.GetOrientation().ToYaw();
                Angle aSignedDifference = Math::SignedAngularDifference(aFacing, aToTarget);
                Angle aDifference = Math::Abs(aSignedDifference);


                if (aDifference > Math::DegToAngle(45.0f)) {



                    // No tag temporary is recorded; repeating this selection is inferred source spelling.






                    if (AnimPlayerAnimExists(tACDOwner, aSignedDifference > 0.0f ? TAG_ACTOR_ANIMATION_MELEE_ATTACK_LEFT : TAG_ACTOR_ANIMATION_MELEE_ATTACK_RIGHT))
                        dwAnimationTag = aSignedDifference > 0.0f ? TAG_ACTOR_ANIMATION_MELEE_ATTACK_LEFT : TAG_ACTOR_ANIMATION_MELEE_ATTACK_RIGHT;
                }
            }
        }
    }




    return dwAnimationTag;
}

#line 785 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerCannotActivate(
    const ActorCommonData & tACD,
    AutoPowerDefinitionR pdefPower)
{
    return PowerGetFormulaValue(&tACD, pdefPower, TAG_POWER_CANNOT_ACTIVATE, -1, -1, 0) > 0.000001f;
}

#line 836 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerFailsIfNoCorpsesAvailable(
    const ActorCommonData * ptACD,
    AutoPowerDefinitionR pdefPower)
{
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_NECRO_CORPSE_FAILS_IF_NO_CORPSES_AVAILABLE, -1, -1, 0));
}

#line 1013 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerIsUsableInTown(
    AutoPowerDefinitionR pdefPower)
{
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_USABLE_IN_TOWN);
}




BOOL PowerOnlyUsableInTownPortalAreas(
    AutoPowerDefinitionR pdefPower)
{
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_ONLY_USABLE_IN_TOWN_PORTAL_AREAS);
}




BOOL PowerIsUsableInCombat(
    AutoPowerDefinitionR pdefPower)
{
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_USABLE_IN_COMBAT);
}



BOOL PowerIsUsableInPVP(
    AutoPowerDefinitionR pdefPower)
{
    return PowerGeneralTagGetBool(pdefPower, TAG_POWER_IS_USABLE_IN_PVP);
}

#line 2083 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerIsDisabled(
    const ActorCommonData & tACD,
    SNO snoPower)
{
    if (snoPower != SNO_NONE) {


        uint32 dwDisabled = tACD.AttributesGetInt(MakeAttribKey(ATTRIBUTE_POWER_DISABLED, snoPower));
        if (dwDisabled) {
            return TRUE;
        }

    }

    return FALSE;
}

#line 3583 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerFailsIfImmobilized(
    const ActorCommonData * ptACD,
    AutoPowerDefinitionR pdefPower)
{
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_FAILS_IF_IMMOBILIZED, -1, -1, 0));
}




BOOL PowerFailsIfStunned(
    const ActorCommonData * ptACD,
    AutoPowerDefinitionR pdefPower)
{
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_FAILS_IF_STUNNED, -1, -1, 0));
}



BOOL PowerFailsIfFeared(
    const ActorCommonData * ptACD,
    AutoPowerDefinitionR pdefPower)
{
    return Math::FloatToInt(PowerGetFormulaValue(ptACD, pdefPower, TAG_POWER_FAILS_IF_FEARED, -1, -1, 0));
}

#line 4490 "X/Common/Powers/PowersCommon.cpp"
BOOL PowerIsGeneralCoolDownDone(
    const ActorCommonData & tACD)
{
    uint32 dwNow = GameGetServerTime();
    uint32 dwCoolDown = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GENERAL_COOLDOWN));

    if (dwCoolDown != UINT32_MAX && dwNow < dwCoolDown)
        return FALSE;
    return TRUE;
}

// Exhaustive cached-CU search records only ActorCommonData.cpp:4446, no header home.
BOOL ACDIsInTownPortalArea(const ActorCommonData & tACD);
#include "../Definitions/LevelAreaDefinition.h"
// Exhaustive cached-CU search records only NavigationMesh.cpp:3624, no header home.
BOOL NavMeshRollToTargetPlace(const ACDID idACDSource, const WorldPlace & tDesiredDestPlace, const BOOL & fIgnoreDestinationHeight, NavPlace & tNavPlaceResult);

#line 2513 "X/Common/Powers/PowersCommon.cpp"

uint32 PowerACDCanStartPowerFailureReasons(
    const ActorCommonData & tACD,
    AutoPowerDefinitionR pdefPower,
    const Target * ptTarget,
    BOOL bIsAI,
    BOOL bFreeCast)
{
    SNO snoPower = pdefPower.GetSNO();
    uint32 dwRetval = 0;



    if (!bIsAI) {

        if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_CANT_START_DISPLAYED_POWERS))) {

            if (ACDIsPlayer(tACD.id)) {

                if (PowerIsDisplayed(pdefPower) && !PowerIsPassive(pdefPower))
                    dwRetval |= 1u << PS_CASTER_CANT_START_DISPLAYED_POWERS;
            } else {




                if (PowerIsOffensive(pdefPower) && !PowerIsPassive(pdefPower))
                    dwRetval |= 1u << PS_CASTER_CANT_START_DISPLAYED_POWERS;
            }
        }




        if (ACDIsPlayer(tACD.id) && !tACD.IsAlive()) {

            dwRetval |= 1u << PS_CASTER_IS_DEAD;
        }

        if (tACD.IsObserver()) {

            dwRetval |= 1u << PS_SILENCED;
        }

        if (!PowerIsUsableInTown(pdefPower) &&
            ACDIsInTown(tACD)) {

            dwRetval |= 1u << PS_NOT_IN_TOWN;
        }

        if (!PowerIsUsableInCombat(pdefPower) &&
            tACD.IsInCombat()) {

            dwRetval |= 1u << PS_NOT_IN_COMBAT;
        }

        if (!PowerIsUsableInPVP(pdefPower) &&
            GameTypeIsPvP()) {

            dwRetval |= 1u << PS_NOT_IN_COMBAT;
        }

        if (PowerOnlyUsableInTownPortalAreas(pdefPower)) {

            if (GameGetGameType() == GT_PVP_TEAMDEATHMATCH) {

                dwRetval |= 1u << PS_ONLY_IN_TOWN_PORTAL_AREA;
            }

            if (!ACDIsInTownPortalArea(tACD)) {
                dwRetval |= 1u << PS_ONLY_IN_TOWN_PORTAL_AREA;
            }


            if (tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_BOSS_ENCOUNTER))) {

                dwRetval |= 1u << PS_UNUSABLE_DURING_BOSS_ENCOUNTER;
            }
        }
    }

    if (!PowerIsPassive(pdefPower)) {

        if (PowerFailsIfImmobilized(&tACD, pdefPower) &&
            tACD.IsImmobilized()) {

            dwRetval |= 1u << PS_IMMOBILIZED;
        }

        if (PowerFailsIfStunned(&tACD, pdefPower) &&
            (tACD.IsStunned() || tACD.IsBlind()) && !PowerBreaksStun(&tACD, pdefPower)) {

            dwRetval |= 1u << PS_CASTER_IS_STUNNED;
        }

        if (tACD.IsFeared() && PowerFailsIfFeared(&tACD, pdefPower) && !PowerBreaksFear(&tACD, pdefPower)) {

            dwRetval |= 1u << PS_FEARED;
        }

        if (PowerCannotActivate(tACD, pdefPower)) {

            dwRetval |= 1u << PS_POWER_DISABLED;
        }
    }

    if (ptTarget &&
        (ptTarget->eType == TT_ACTOR || ptTarget->eType == TT_ACTOR_WITH_FALLBACK_POINT)) {

        ActorCommonData * ptACDTarget = ACDTryToGet(ptTarget->idACDTarget);
        if (ptACDTarget) {

            BOOL fTargetAllies = PowerCastTargetAllies(pdefPower);
            BOOL fTargetEnemies = PowerCastTargetEnemies(pdefPower);
            BOOL fTargetNecroCorpsesOnly = PowerCastTargetNecroCorpsesOnly(tACD, pdefPower);

            Team eTeamTarget = ptACDTarget->GetTeam();
            if (fTargetNecroCorpsesOnly) {

                if (!ptACDTarget->IsNecroCorpse()) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }
            }
            else if (eTeamTarget != TEAM_INVALID) {

                BOOL fFriendly = FALSE;




                if (ptACDTarget->IsWreckableObject())
                    fFriendly = FALSE;
                else fFriendly = ACDIsFriendly(tACD, *ptACDTarget, TRUE);


                if (fFriendly && !fTargetAllies) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }

                if (!fFriendly && !fTargetEnemies) {
                    dwRetval |= 1u << PS_INVALID_TARGET;
                }
            }

            else if (!PowerCastTargetNeutral(pdefPower)) {

                dwRetval |= 1u << PS_INVALID_TARGET;
            }

            if (ptACDTarget->IsMonster()) {

                if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_IGNORE_LARGE_MONSTERS) && ptACDTarget->IsLargeMonster()) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }

                if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_NORMAL_MONSTERS_ONLY) && !ptACDTarget->IsNormalMonster()) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }

                if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_UNDEAD_MONSTERS_ONLY) && !ptACDTarget->IsUndeadMonster()) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }
            }


            if (PowerCastTargetNecroCorpsesOnly(tACD, pdefPower) != !!ptACDTarget->IsNecroCorpse()) {

                dwRetval |= 1u << PS_INVALID_TARGET;
            }

            if (ptACDTarget->IsNecroCorpse()) {

                if (!tACD.CanTargetNecroCorpse(ptACDTarget->id)) {

                    dwRetval |= 1u << PS_INVALID_TARGET;
                }
            }

            if (ptACDTarget->IsMonster() || ACDIsPlayer(ptACDTarget->id) || ptACDTarget->IsWreckableObject()) {

                if (!PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_ALLOW_DEAD_TARGETS) && !ptACDTarget->IsAlive()) {

                    dwRetval |= 1u << PS_TARGET_IS_DEAD;
                }
            }

            if (!ptACDTarget->IsTargetable() && !ptTarget->bForcedActorTarget) {

                dwRetval |= 1u << PS_INVALID_TARGET;
            }
        }
    }

    BOOL bTargetIsValid = ptTarget &&

        !ptTarget->IsNone();




    if (!bIsAI && PowerIsGizmo(pdefPower)) {

        if (!ptTarget || ptTarget->eType != TT_ACTOR) {

            dwRetval |= 1u << PS_INVALID_TARGET;
            bTargetIsValid = FALSE;
        }
    }

    if (PowerRequiresActorTarget(tACD, pdefPower)) {

        if (!ptTarget || !ptTarget->IsTargetingACD() || ACDGet(ptTarget->idACDTarget).IsItem()) {

            dwRetval |= 1u << PS_INVALID_TARGET;
            bTargetIsValid = FALSE;
        }
    }
    // Inference: operand order and tab indent from columns (original rows 2736:13 = &&, 2736:16 = bTargetIsValid).
	if (!bIsAI && bTargetIsValid) {

        NavPlace tNavPlace;
        BOOL bInInventory = FALSE;
        BOOL bGotNavPlace = FALSE;

        ActorCommonData * ptACDTarget = ACDTryToGet(ptTarget->idACDTarget);
        if (ptACDTarget) {

            bInInventory = ptACDTarget->IsInInventory();
        }

        if (!bInInventory) {

            bGotNavPlace = ptTarget->GetNavPlace(tNavPlace);



            if (!bGotNavPlace && ptTarget->eType == TT_POINT) {

                NavMeshRollToTargetPlace(tACD.id, ptTarget->tPlace, TRUE, tNavPlace);
                bGotNavPlace = TRUE;
            }
        }

        if (!(bGotNavPlace || bInInventory)) {

            dwRetval |= 1u << PS_INVALID_TARGET;
        } else {


            BOOL bUsableInTown = PowerIsUsableInTown(pdefPower);
            if (bGotNavPlace && !bUsableInTown) {

                SNO snoLevelArea = NavMeshHandleGetLevelArea(tNavPlace.tNavMeshHandle);
                if (LevelAreaIsTown(snoLevelArea)) {

                    dwRetval |= 1u << PS_NOT_IN_TOWN;
                }
            }

            if (PowerIsPrimary(pdefPower, tACD.id)) {

                if (!PowerCheckStartLOS(tACD.id, pdefPower, *ptTarget)) {

                    dwRetval |= 1u << PS_NO_LINE_OF_SIGHT;
                }
            }

            if (PowerGeneralTagGetBool(pdefPower, TAG_POWER_CAST_TARGET_MUST_BE_IN_TELEPORTABLE_AREA)) {

                SNO snoLevelArea = NavMeshHandleGetLevelArea(tNavPlace.tNavMeshHandle);
                if (!LevelAreaTeleportAllowed(snoLevelArea)) {

                    dwRetval |= 1u << PS_TARGET_NOT_IN_TELEPORTABLE_AREA;
                }
            }
        }
    }

    if (!PowerIsCoolDownDone(tACD, snoPower)) {

        dwRetval |= 1u << PS_NEED_COOLDOWN_BIT;
    }

    if (!PowerIsGeneralCoolDownDone(tACD)) {

        dwRetval |= 1u << PS_NEED_GENERAL_COOLDOWN_BIT;
    }

    if (!bIsAI) {

        if (PowerIsEmote(pdefPower) && !PowerIsEmoteCoolDownDone(tACD)) {

            dwRetval |= 1u << PS_NEED_EMOTE_COOLDOWN_BIT;
        }

        if (!PowerActorCanPayHealthCost(tACD, pdefPower, bFreeCast)) {

            dwRetval |= 1u << PS_NEED_HEALTH_BIT;
        }

        if (!PowerActorCanPayChargeCost(tACD, snoPower)) {

            dwRetval |= 1u << PS_NEED_CHARGES_BIT;
        }

        if (!PowerActorCanPayResourceCost(tACD, pdefPower)) {

            dwRetval |= 1u << PS_NEED_RESOURCE_BIT;
        }

        if (!PowerActorMeetsWeaponRequirement(tACD, pdefPower)) {

            dwRetval |= 1u << PS_WRONG_WEAPON;
        }

        if (PowerFailsIfNoCorpsesAvailable(&tACD, pdefPower) && ACDIsPlayer(tACD.id) && ptTarget && !tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NECROMANCER_CORPSE_FREE_CASTING))) {

            Player * ptPlayer = PlayerGetByACD(tACD.id);
            if (ptPlayer) {


                WorldPlace placeDest;
                ptTarget->GetPlace(placeDest);
                blz::vector<ACDID> listCorpsesACDs = ACDGetNecroCorpsesForPlayerAroundPosition(ptPlayer, placeDest.wp, snoPower);
                if (listCorpsesACDs.size() == 0) {

                    dwRetval |= 1u << PS_NEED_CORPSES;
                }
            }
        }
    }

    if (!PowerActorCanFreeCastIfNecessary(tACD, pdefPower)) {

        dwRetval |= 1u << PS_UNABLE_TO_FREECAST_BIT;
    }

    if (!PowerActorMeetsSkillPointRequirement(tACD, pdefPower)) {

        dwRetval |= 1u << PS_NEED_SKILLPOINT;
    }

    if (PowerIsPrimary(pdefPower, tACD.id)) {

        if (bTargetIsValid &&
            !PowerIsCloseEnough(tACD.id, pdefPower, ptTarget, FALSE)) {

            dwRetval |= 1u << PS_NOT_CLOSE_ENOUGH_BIT;
        }
    }

    if (PowerIsDisabled(tACD, snoPower)) {

        dwRetval |= 1u << PS_POWER_DISABLED;
    }

    return dwRetval;
}

extern "C" BOOL sub_7100733800(AutoPowerDefinitionR, int32)
{
    return FALSE;
}
