// Adopted from the DWARF reference; hand maintained.
// Original file: X/Common/Actors/ActorEffectCommon.h
// Subset: the EffectType enum and the effect message structs.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../CommonTypes.h"
#include "../WorldPlace.h"
#include "../SNO/SNOFiles.h"
#include "../../Sigma/DataStructures/XOptional.h"

typedef int32 RopePosition;

typedef int32 DamageType;


enum {
    EFFECT_INVALID = -1,
    EFFECT_PLAY_HITSOUND = 0,
    EFFECT_DEATH = 1,
    EFFECT_PLAY_LANDING_SOUND = 2,
    EFFECT_PLAY_HALO_SOUND = 3,
    EFFECT_PLAY_PICKUP_SOUND = 4,
    EFFECT_PICKUP_GOLD = 5,
    EFFECT_LEVELUP = 6,
    EFFECT_HEAL = 7,
    EFFECT_REJUVE = 8,
    EFFECT_SHRINE = 9,
    EFFECT_IDENTIFY = 10,
    EFFECT_OPEN_CHEST = 11,
    EFFECT_HIT = 12,
    EFFECT_CREATE_PARTICLE = 13,
    EFFECT_PLAY_SOUND = 14,
    EFFECT_TRIGGER_ACTOR_ATTACHMENT = 15,
    EFFECT_ELECTRIC_CHARGE = 16,
    EFFECT_CANTPICKUP = 17,
    EFFECT_CANTPICKUP_NO_SUITABLE_SLOT = 18,
    EFFECT_CANTPICKUP_CURRENCY = 19,
    EFFECT_CANTPICKUP_SINGULAR = 20,
    EFFECT_CANTPICKUP_SOULBOUND = 21,
    EFFECT_RESURRECT_DEPRECATED = 22,
    EFFECT_PLAYER_RESURRECT = 23,
    EFFECT_RECREATE_DEAD = 24,
    EFFECT_CRITICAL_HIT = 25,
    EFFECT_KNOCK_BACK = 26,
    EFFECT_DEATH_DISINTEGRATE = 27,
    EFFECT_WARP_IN_PORTAL = 28,
    EFFECT_WARP_IN_WAYPOINT = 29,
    EFFECT_WARP_IN_COMPANION = 30,
    EFFECT_PLAYER_DETECTED = 31,
    EFFECT_REMOVE_COMPLEX_EFFECT = 32,
    EFFECT_PLAY_EFFECT_GROUP = 33,
    EFFECT_HIT_LOCAL = 34,
    EFFECT_BLOCKED_LOCAL = 35,
    EFFECT_DODGED_LOCAL = 36,
    EFFECT_NPC_OPERATE_LOCAL = 37,
    EFFECT_LOCAL_WIZARD_POWERED_IMPACT_MANA_FLASH = 38,
    EFFECT_DEAD = 39,
    EFFECT_ITEM_ELEMENTAL_EFFECT = 40,
    EFFECT_CRIT_DEATH_PHYSICAL = 41,
    EFFECT_CRIT_DEATH_FIRE = 42,
    EFFECT_CRIT_DEATH_POISON = 43,
    EFFECT_CRIT_DEATH_ARCANE = 44,
    EFFECT_CRIT_DEATH_HOLY = 45,
    EFFECT_CRIT_DEATH_LIGHTNING = 46,
    EFFECT_CRIT_DEATH_COLD = 47,
    EFFECT_DEATH_RARE_MONSTER = 48,
    EFFECT_DEATH_PULVERIZE = 49,
    EFFECT_DEATH_PLAGUED = 50,
    EFFECT_DEATH_ARCANE = 51,
    EFFECT_DEATH_HOLY = 52,
    EFFECT_DEATH_FIRE = 53,
    EFFECT_DEATH_COLD = 54,
    EFFECT_DEATH_LAVA = 55,
    EFFECT_DEATH_LIGHTNING = 56,
    EFFECT_DEATH_POISON = 57,
    EFFECT_DEATH_SPIRIT = 58,
    EFFECT_SHIELD_BLOCK = 59,
    EFFECT_CHANGE_DAMAGE_STATE = 60,
    EFFECT_RECREATE_DAMAGE_STATE = 61,
    EFFECT_INTERRUPTED = 62,
    EFFECT_HEAL_MANA = 63,
    EFFECT_GATE_OPENING = 64,
    EFFECT_GATE_CLOSING = 65,
    EFFECT_GATE_OPENED = 66,
    EFFECT_GATE_CLOSED = 67,
    EFFECT_SINK = 68,
    EFFECT_PROJECTILE_EFFECT = 69,
    EFFECT_PROJECTILE_BOUNCE = 70,
    EFFECT_SAVEPOINT_ACTIVATED = 71,
    EFFECT_ALT_LEVELUP = 72,
    EFFECT_CRAFTER_LEARN_RECIPE = 73,
    EFFECT_PLAY_POTION_AUTOPICKUP_SOUND = 74,
    EFFECT_COSMETIC_PET_WARP_OUT = 75,
    EFFECT_BLOOD_SHARD_ON_PICKUP = 76,
    EFFECT_NECRO_CORPSE_SPAWN = 77,
    EFFECT_NUM_EFFECTS = 78,
};

typedef int32 EffectType;

struct PlayEffectMessage {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    EffectType eEffectType;  // 0x0c
    XOptional<int> nEffectParameter;  // 0x10
    XOptional<ACDNetworkName> annPlayer;  // 0x18
};
static_assert(sizeof(PlayEffectMessage) == 0x20, "PlayEffectMessage");

struct PlayHitEffectMessage {  // size 0x18
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    ACDNetworkName annAttacker;  // 0x0c
    DamageType eDamageType;  // 0x10
    BOOL fCrit;  // 0x14
};
static_assert(sizeof(PlayHitEffectMessage) == 0x18, "PlayHitEffectMessage");

struct PlayHitEffectOverrideMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    ACDNetworkName ann;  // 0x08
    ACDNetworkName annAttacker;  // 0x0c
    SNO snoEffectGroup;  // 0x10
};
static_assert(sizeof(PlayHitEffectOverrideMessage) == 0x14, "PlayHitEffectOverrideMessage");

struct RopeEffectMessageACDToACD {  // size 0x20
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoRope;  // 0x08
    ACDNetworkName annFrom;  // 0x0c
    RopePosition ePositionFrom;  // 0x10
    ACDNetworkName annTo;  // 0x14
    RopePosition ePositionTo;  // 0x18
    BOOL bIgnoreOwnerAlpha;  // 0x1c
};
static_assert(sizeof(RopeEffectMessageACDToACD) == 0x20, "RopeEffectMessageACDToACD");

struct RopeEffectMessageACDToPlace {  // size 0x28
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoRope;  // 0x08
    ACDNetworkName annFrom;  // 0x0c
    RopePosition ePositionFrom;  // 0x10
    WorldPlace tPlaceTo;  // 0x14
    BOOL bIgnoreOwnerAlpha;  // 0x24
};
static_assert(sizeof(RopeEffectMessageACDToPlace) == 0x28, "RopeEffectMessageACDToPlace");

struct EffectGroupACDToACDMessage {  // size 0x14
    RequiredMessageHeader tHeader;  // 0x00
    SNO snoEffectGroup;  // 0x08
    ACDNetworkName annFrom;  // 0x0c
    ACDNetworkName annTo;  // 0x10
};
static_assert(sizeof(EffectGroupACDToACDMessage) == 0x14, "EffectGroupACDToACDMessage");
