// Original file: X/Common/Definitions/AttachmentDefinition.cpp

#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../SNO/SNOName.h"
#include "AttachmentDefinition.h"

#include "Definitions.h"
#include "../WorldPlace.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

#include "../../Client/Gfx/GfxVB.h"

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_TIME;
extern const XTypeDescriptor * DT_IMPULSE;
extern const XTypeDescriptor * DT_VELOCITY;

// Defined in X/Sigma/TypeDescriptor/SymbolicValue.cpp:117; no adopted header declares it yet.
uint32 StringHashI(LPCSTR pszName);

// Defined in X/Common/Definitions/Definitions.cpp; no adopted header declares it yet.
GroupDefinition * GameDefinitionGet(const SNOGroup snoGroup);

// X/Client/CGame.h:145. Only the member read here is declared; the rest of the class is not
// adopted. CGameGlobalsGet is defined in X/Client/CGame.cpp:6338.
#include "../../Client/CGame.h"




CGameGlobals & CGameGlobalsGet();

// X/Common/Actors/ActorEffectCommon.h (not adopted).
typedef int32 RopePosition;

enum {
    TRIGGER_EVENT_XML_VERSION = 5,
    TRIGGER_CONDITIONS_XML_VERSION = 2,
};

static const int32 cnDefaultSpawnedActorLife = 600;  // line 31
static const int32 cnDefaultHitFlashDuration = 15;  // line 32

static const RGBAColor crgbaDefaultColor(128, 128, 128, 255);  // line 34

// The field names are "" in the binary.
static const TypeDescriptorField TriggerConditions_Fields[10] = {  // line 63
    TypeDescriptorField("", DT_PERCENT, offsetof(TriggerConditions, bChance), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerConditions, dwDelayMin), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerConditions, dwDelayDelta), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerConditions, dwRepeatTimeMin), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerConditions, dwRepeatTimeDelta), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(TriggerConditions, flImpulseMin), NULL, 1),
    TypeDescriptorField("", DT_IMPULSE, offsetof(TriggerConditions, flImpulseDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerConditions, eMaterial), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerConditions, dwConditionFlags), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TriggerConditions), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TriggerConditions("TriggerConditions", TRIGGER_CONDITIONS_XML_VERSION, TriggerConditions_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 73
const XTypeDescriptor * DT_TRIGGERCONDITIONS = &TypeDescriptor_TriggerConditions;

static const TypeDescriptorField HardpointHash_Fields[3] = {  // line 76
    TypeDescriptorField("", DT_INT, offsetof(HardpointHash, dwHash), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(HardpointHash, dwHashFlag), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HardpointHash), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HardpointHash("HardpointHash", 0, HardpointHash_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 79
const XTypeDescriptor * DT_HARDPOINTHASH = &TypeDescriptor_HardpointHash;

static const TypeDescriptorField Hardpoint_Fields[4] = {  // line 82
    TypeDescriptorField("", DT_HARDPOINTHASH, offsetof(Hardpoint, tHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(Hardpoint, nBoneIndex), NULL, 1),
    TypeDescriptorField("", DT_PRTRANSFORM, offsetof(Hardpoint, transform), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Hardpoint), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Hardpoint("Hardpoint", 0, Hardpoint_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 87
const XTypeDescriptor * DT_HARDPOINT = &TypeDescriptor_Hardpoint;

static const TypeDescriptorField HardpointLink_Fields[3] = {  // line 89
    TypeDescriptorField("", DT_HARDPOINTHASH, offsetof(HardpointLink, tHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(HardpointLink, eSource), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(HardpointLink), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_HardpointLink("HardpointLink", 0, HardpointLink_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 93
const XTypeDescriptor * DT_HARDPOINTLINK = &TypeDescriptor_HardpointLink;

static const TypeDescriptorField LookLink_Fields[3] = {  // line 95
    TypeDescriptorField("", DT_INT, offsetof(LookLink, dwLookHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(LookLink, dwLookHashPadding), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(LookLink), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_LookLink("LookLink", 0, LookLink_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 99
const XTypeDescriptor * DT_LOOKLINK = &TypeDescriptor_LookLink;

static const TypeDescriptorField ConstraintLink_Fields[3] = {  // line 101
    TypeDescriptorField("", DT_INT, offsetof(ConstraintLink, dwHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(ConstraintLink, dwHashPadding), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConstraintLink), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConstraintLink("ConstraintLink", 0, ConstraintLink_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 105
const XTypeDescriptor * DT_CONSTRAINTLINK = &TypeDescriptor_ConstraintLink;

static const TypeDescriptorField MsgTriggeredEvent_Fields[3] = {  // line 107
    TypeDescriptorField("", DT_INT, offsetof(MsgTriggeredEvent, dwMsgKey), NULL, 1),
    TypeDescriptorField("", DT_TRIGGEREVENT, offsetof(MsgTriggeredEvent, tTriggerEvent), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(MsgTriggeredEvent), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_MsgTriggeredEvent("MsgTriggeredEvent", 2, MsgTriggeredEvent_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 110
const XTypeDescriptor * DT_MSGTRIGGEREDEVENT = &TypeDescriptor_MsgTriggeredEvent;

static const TypeDescriptorField TriggerEvent_Fields[32] = {  // line 112
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eType), NULL, 1),
    TypeDescriptorField("", DT_TRIGGERCONDITIONS, offsetof(TriggerEvent, tConditions), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eAttachmentType), NULL, 1),
    TypeDescriptorField("", DT_SNO_NAME, offsetof(TriggerEvent, snoname), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, dwID), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, dwPinFlags), NULL, 0x80001),
    FixedArrayTypeDescriptorField("", DT_HARDPOINTLINK, offsetof(TriggerEvent, tHardpointLinks), NULL, 1, MAX_EVENT_HARDPOINT_LINKS, 0),
    TypeDescriptorField("", DT_LOOKLINK, offsetof(TriggerEvent, tLookLink), NULL, 1),
    TypeDescriptorField("", DT_CONSTRAINTLINK, offsetof(TriggerEvent, tConstraintLink), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, dwAnimTag), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TriggerEvent, flAlpha), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eMsgPassMethod), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, dwMsgKey), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, dwTriggerFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, fIgnoreOwnerScale), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, fIgnoreOwnerAlpha), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TriggerEvent, flScale), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TriggerEvent, flFadeTime), (const void *)0x40000000, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eFootstepType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eOutroAnimType), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, fUseParticleScalar), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TriggerEvent, flParticleScalar), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, fUseMinimumSpeed), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(TriggerEvent, flMinimumSpeed), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerEvent, dwSpawnedActorLife), (const void *)cnDefaultSpawnedActorLife, 1),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, eRuneType), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(TriggerEvent, bUseRuneType), NULL, 0),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(TriggerEvent, rgbaColorAdd), &crgbaDefaultColor, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerEvent, nHitFlashAddDuration), (const void *)cnDefaultHitFlashDuration, 1),
    TypeDescriptorField("", DT_RGBACOLOR, offsetof(TriggerEvent, rgbaColorMul), &crgbaDefaultColor, 1),
    TypeDescriptorField("", DT_TIME, offsetof(TriggerEvent, nHitFlashMulDuration), (const void *)cnDefaultHitFlashDuration, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TriggerEvent), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TriggerEvent("TriggerEvent", TRIGGER_EVENT_XML_VERSION, TriggerEvent_Fields, 32, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 144
const XTypeDescriptor * DT_TRIGGEREVENT = &TypeDescriptor_TriggerEvent;

// Each hardpoint name is a string and its hash, defined on one line. The macro name is invented.
#define DEFINE_HARDPOINT_NAME(name, str) \
    const CHAR * name##STR = str;        \
    const HardpointHash name(str)
#define DEFINE_STATIC_HARDPOINT_NAME(name, str) \
    const CHAR * name##STR = str;               \
    static const HardpointHash name(str)

DEFINE_HARDPOINT_NAME(DEFAULT_HARDPOINT_NAME, "Default");  // line 154
DEFINE_HARDPOINT_NAME(AURA_HARDPOINT_NAME, "HP_aura");
DEFINE_HARDPOINT_NAME(CURSE_HARDPOINT_NAME, "HP_curse");
DEFINE_HARDPOINT_NAME(CHEST_HARDPOINT_NAME, "HP_chest");
DEFINE_HARDPOINT_NAME(LEFT_FOOT_HARDPOINT_NAME, "HP_leftFoot");
DEFINE_HARDPOINT_NAME(RIGHT_FOOT_HARDPOINT_NAME, "HP_rightFoot");
DEFINE_HARDPOINT_NAME(MOUTH_HARDPOINT_NAME, "HP_mouth");
DEFINE_HARDPOINT_NAME(LEFT_HAND_HARDPOINT_NAME, "HP_leftHand");
DEFINE_HARDPOINT_NAME(MANA_GATHER_HARDPOINT_NAME, "HP_ManaGather");
DEFINE_HARDPOINT_NAME(RIGHT_HAND_HARDPOINT_NAME, "HP_rightHand");
DEFINE_HARDPOINT_NAME(BACK_HARDPOINT_NAME, "HP_back");
DEFINE_HARDPOINT_NAME(LEFT_WEAPON_HARDPOINT_NAME, "HP_leftWeapon");
DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_HARDPOINT_NAME, "HP_rightWeapon");
DEFINE_HARDPOINT_NAME(LEFT_FIST_HARDPOINT_NAME, "HP_leftFist");
DEFINE_HARDPOINT_NAME(RIGHT_FIST_HARDPOINT_NAME, "HP_rightFist");
DEFINE_HARDPOINT_NAME(LIGHT_HARDPOINT_NAME, "HP_light");
DEFINE_HARDPOINT_NAME(SHIELD_HARDPOINT_NAME, "HP_shield");
DEFINE_HARDPOINT_NAME(HEAD_HARDPOINT_NAME, "HP_head");
DEFINE_HARDPOINT_NAME(HELM_HARDPOINT_NAME, "HP_helm");
DEFINE_HARDPOINT_NAME(LEFT_SHOULDER_HARDPOINT_NAME, "HP_left_shoulderPad");
DEFINE_HARDPOINT_NAME(RIGHT_SHOULDER_HARDPOINT_NAME, "HP_right_shoulderPad");
DEFINE_HARDPOINT_NAME(CENTROID_HARDPOINT_NAME, "HP_centroid");
DEFINE_HARDPOINT_NAME(TRAIL_1_NAME, "HP_Trail1");
DEFINE_HARDPOINT_NAME(TRAIL_2_NAME, "HP_Trail2");
DEFINE_HARDPOINT_NAME(TRAIL_3_NAME, "HP_Trail3");
DEFINE_HARDPOINT_NAME(TRAIL_4_NAME, "HP_Trail4");
DEFINE_HARDPOINT_NAME(CENTER_HARDPOINT_NAME, "HP_center");
DEFINE_HARDPOINT_NAME(LEFT_SECONDARY_WEAPON_HARDPOINT_NAME, "HP_left_scabbard");
DEFINE_HARDPOINT_NAME(RIGHT_SECONDARY_WEAPON_HARDPOINT_NAME, "HP_right_scabbard");
DEFINE_HARDPOINT_NAME(PELVIS_HARDPOINT_NAME, "HP_pelvis");
DEFINE_HARDPOINT_NAME(SHEATH_LEFT_HARDPOINT_NAME, "HP_sheath_left_Back");
DEFINE_HARDPOINT_NAME(SHEATH_RIGHT_HARDPOINT_NAME, "HP_sheath_right_Back");
DEFINE_HARDPOINT_NAME(SHEATH_LEFT_HIP_HARDPOINT_NAME, "HP_sheath_left_Hip");
DEFINE_HARDPOINT_NAME(SHEATH_RIGHT_HIP_HARDPOINT_NAME, "HP_sheath_right_Hip");
DEFINE_HARDPOINT_NAME(SHEATH_SHIELD_HARDPOINT_NAME, "HP_sheath_Shield");

DEFINE_HARDPOINT_NAME(PHYSICS_1_HARDPOINT_NAME, "HP_physics1");  // line 190
DEFINE_HARDPOINT_NAME(PHYSICS_2_HARDPOINT_NAME, "HP_physics2");
DEFINE_HARDPOINT_NAME(PHYSICS_3_HARDPOINT_NAME, "HP_physics3");
DEFINE_HARDPOINT_NAME(PHYSICS_4_HARDPOINT_NAME, "HP_physics4");
DEFINE_HARDPOINT_NAME(LEGENDARY_FX_HARDPOINT_NAME, "HP_uniqueFX");
DEFINE_HARDPOINT_NAME(EXPLOSION_HARDPOINT_NAME, "HP_Explosion");

DEFINE_HARDPOINT_NAME(LEFT_WEAPON_PHYSICS_1_HARDPOINT_NAME, "HP_leftWeapon:HP_physics1");  // line 197
DEFINE_HARDPOINT_NAME(LEFT_WEAPON_PHYSICS_2_HARDPOINT_NAME, "HP_leftWeapon:HP_physics2");
DEFINE_HARDPOINT_NAME(LEFT_WEAPON_PHYSICS_3_HARDPOINT_NAME, "HP_leftWeapon:HP_physics3");
DEFINE_HARDPOINT_NAME(LEFT_WEAPON_PHYSICS_4_HARDPOINT_NAME, "HP_leftWeapon:HP_physics4");
DEFINE_HARDPOINT_NAME(LEFT_WEAPON_LEGENDARY_FX_HARDPOINT_NAME, "HP_leftWeapon:HP_uniqueFX");

DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_PHYSICS_1_HARDPOINT_NAME, "HP_rightWeapon:HP_physics1");  // line 203
DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_PHYSICS_2_HARDPOINT_NAME, "HP_rightWeapon:HP_physics2");
DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_PHYSICS_3_HARDPOINT_NAME, "HP_rightWeapon:HP_physics3");
DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_PHYSICS_4_HARDPOINT_NAME, "HP_rightWeapon:HP_physics4");
DEFINE_HARDPOINT_NAME(RIGHT_WEAPON_LEGENDARY_FX_HARDPOINT_NAME, "HP_rightWeapon:HP_uniqueFX");

DEFINE_HARDPOINT_NAME(ATTACH_HARDPOINT_NAME, "HP_attach");  // line 209
DEFINE_STATIC_HARDPOINT_NAME(EMITTER_HARDPOINT_NAME, "HP_Emitter");
DEFINE_STATIC_HARDPOINT_NAME(EMITTER_FLOOR_HARDPOINT_NAME, "HP_Emitter_Floor");

DEFINE_HARDPOINT_NAME(BEAM_CENTER_HARDPOINT_NAME, "HP_beamCenter");  // line 213
DEFINE_HARDPOINT_NAME(BEAM_LEFT_HARDPOINT_NAME, "HP_beamLeft");
DEFINE_HARDPOINT_NAME(BEAM_RIGHT_HARDPOINT_NAME, "HP_beamRight");

DEFINE_HARDPOINT_NAME(ANIM_PERMUTATION_HARDPOINT_NAME, "HP_from_anim_permutation");  // line 217

// line 379
void TriggerEventIterateSnoReferences(const TriggerEvent & tTriggerEvent, SNOIterateFunc pfuncIterate, void * pData) {
    if (tTriggerEvent.snoname.snoGroup != SNOGROUP_NONE)
        pfuncIterate(tTriggerEvent.snoname, 0, pData);
}

// line 393
void TriggerEventChangeSNOReference(TriggerEvent & tTriggerEvent, const SNOName & snonameOld, const SNO snoNew) {
    if (tTriggerEvent.snoname.snoHandle != SNO_NONE && tTriggerEvent.snoname.snoGroup != SNOGROUP_NONE &&
        snonameOld == tTriggerEvent.snoname)
        tTriggerEvent.snoname.snoHandle = snoNew;
}

// line 409
SNOGroup TriggerEventGetSnoGroup(const TriggerEvent & tEvent) {
    switch (tEvent.eType) {
    case TRIGGEREDEVENTTYPE_ADD_OBJECT:
    case TRIGGEREDEVENTTYPE_SPAWN_OBJECT:
        switch (tEvent.eAttachmentType) {
        case ATTACHMENTTYPE_ACTOR:
            return ACTOR_SNO;
        case ATTACHMENTTYPE_PARTICLESYSTEM:
            return PARTICLE_SNO;
        case ATTACHMENTTYPE_LIGHT:
            return LIGHT_SNO;
        case ATTACHMENTTYPE_SOUND:
            return SOUND_SNO;
        case ATTACHMENTTYPE_AMBIENTSOUND:
            return AMBIENTSOUND_SNO;
        case ATTACHMENTTYPE_EXPLOSION:
            return EXPLOSION_SNO;
        case ATTACHMENTTYPE_SCENE:
            return SCENE_SNO;
        case ATTACHMENTTYPE_SHAKE:
            return SHAKE_SNO;
        case ATTACHMENTTYPE_VIBRATION:
            return VIBRATION_SNO;
        default:
            return SNOGROUP_NONE;
        }
    case TRIGGEREDEVENTTYPE_ADD_ROPE:
        return ROPE_SNO;
    case TRIGGEREDEVENTTYPE_ADD_TRAIL:
        return TRAIL_SNO;
    case TRIGGEREDEVENTTYPE_ADD_EFFECT_GROUP:
        return EFFECTGROUP_SNO;
    case TRIGGEREDEVENTTYPE_MUSIC:
        return MUSIC_SNO;
    default:
        return SNOGROUP_NONE;
    }
}

// line 581
void TriggerEventInit(TriggerEvent & tEvent) {
    SigmaMemoryClear(&tEvent, sizeof(TriggerEvent));
    tEvent.tConditions.bChance = 0xff;
    tEvent.flScale = 1.0f;
    tEvent.flFadeTime = 2.0f;
    tEvent.eRuneType = -1;
}

// line 596
void TriggerEventGetDefaultData(TriggerEvent & tEvent) {
    tEvent.eType = TRIGGEREDEVENTTYPE_SPAWN_OBJECT;
    tEvent.eAttachmentType = ATTACHMENTTYPE_PARTICLESYSTEM;
    tEvent.tConditions.bChance = 0xff;
    tEvent.dwAnimTag = 0x11000;
    tEvent.eMsgPassMethod = MSGPASS_TO_ACTORCHAIN;
    tEvent.dwMsgKey = MSGKEY_RESERVED_HIT;
    tEvent.dwTriggerFlags = (1 << TRIGGERFLAG_DELETE_ON_OWNER_DEATH_BIT);
    tEvent.fIgnoreOwnerScale = FALSE;
    tEvent.fIgnoreOwnerAlpha = FALSE;
    tEvent.flScale = 1.0f;
    tEvent.flFadeTime = 2.0f;
    tEvent.eOutroAnimType = OUTROANIMATIONTYPE_RUN_LEFT_FOOT;
    tEvent.dwSpawnedActorLife = cnDefaultSpawnedActorLife;
    tEvent.eRuneType = -1;
    tEvent.bUseRuneType = FALSE;

    for (int32 i = 0; i < MAX_EVENT_HARDPOINT_LINKS; i++) {
        tEvent.tHardpointLinks[i].eSource = HARDPOINTSOURCE_PRIMARY;
        tEvent.tHardpointLinks[i].tHash = DEFAULT_HARDPOINT_NAME;
    }

    tEvent.snoname.snoHandle = GameDefinitionGet(PARTICLE_SNO)->GetFallbackDataSNO();

    tEvent.rgbaColorMul.Set(255, 255, 255, 255);
    tEvent.rgbaColorAdd.Set(0, 0, 0, 255);
}

// line 636
uint32 TriggerConditions::GetDelay(const BOOL bForRepeat) const {
    RNG & rng = CGameGlobalsGet().rngLocal;
    uint32 dwDelay;

    if (!bForRepeat) {
        uint8 bTrigger = bChance && Rand_Internal(rng, 254) <= bChance;
        if (!bTrigger)
            return (uint32)-1;
        dwDelay = RandRange_Internal(rng, dwDelayMin, dwDelayDelta);
    } else {
        dwDelay = RandRange_Internal(rng, dwRepeatTimeMin, dwRepeatTimeDelta);
    }

    return dwDelay;
}

// line 675
HardpointHash RopePositionToHardpointName(RopePosition ePosition) {
    switch (ePosition) {
    case 0: return DEFAULT_HARDPOINT_NAME;
    case 1: return CHEST_HARDPOINT_NAME;
    case 2: return MOUTH_HARDPOINT_NAME;
    case 3: return LEFT_HAND_HARDPOINT_NAME;
    case 4: return RIGHT_HAND_HARDPOINT_NAME;
    case 5: return MANA_GATHER_HARDPOINT_NAME;
    case 6: return BACK_HARDPOINT_NAME;
    case 7: return HEAD_HARDPOINT_NAME;
    case 8: return LEFT_FOOT_HARDPOINT_NAME;
    case 9: return RIGHT_FOOT_HARDPOINT_NAME;
    case 10: return PELVIS_HARDPOINT_NAME;
    case 11: return LEFT_WEAPON_HARDPOINT_NAME;
    case 12: return RIGHT_WEAPON_HARDPOINT_NAME;
    case 13: return EMITTER_HARDPOINT_NAME;
    case 14: return EMITTER_FLOOR_HARDPOINT_NAME;
    case 15: return EXPLOSION_HARDPOINT_NAME;
    case 16: return BEAM_CENTER_HARDPOINT_NAME;
    case 17: return BEAM_LEFT_HARDPOINT_NAME;
    case 18: return BEAM_RIGHT_HARDPOINT_NAME;
    default: {
        HardpointHash tNull;
        return tNull;
    }
    }
}

// line 710
HardpointHash::HardpointHash() {
    dwHash = 0;
    dwHashFlag = 0;
}

// line 718
HardpointHash::HardpointHash(const char * szName) {
    *this = szName;
}

// line 726
HardpointHash::HardpointHash(const HardpointHash & from) {
    *this = from;
}

// line 742
BOOL HardpointHash::IsDamage() const {
    return (dwHashFlag >> HARDPOINTHASH_FLAG_IS_DAMAGE) & 1;
}

// line 749
BOOL HardpointHash::IsDynamic() const {
    return (dwHashFlag >> HARDPOINTHASH_FLAG_IS_DYNAMIC) & 1;
}

// line 765
void HardpointHash::SetDynamic(uint32 dwIndex) {
    dwHash = dwIndex;
    dwHashFlag = (1 << HARDPOINTHASH_FLAG_IS_DYNAMIC);
}

// line 775
BOOL HardpointHash::operator==(const HardpointHash & rhs) const {
    return rhs.dwHash == dwHash && rhs.dwHashFlag == dwHashFlag;
}

// line 783
BOOL HardpointHash::operator==(const uint32 dwRHS) const {
    return dwHash == dwRHS && !IsDynamic();
}

// line 791
BOOL HardpointHash::operator==(const char * szRHS) const {
    return StringHashI(szRHS) == dwHash && !IsDynamic();
}

// line 799
HardpointHash & HardpointHash::operator=(const HardpointHash & from) {
    dwHash = from.dwHash;
    dwHashFlag = from.dwHashFlag;
    return *this;
}

// line 809
HardpointHash & HardpointHash::operator=(const char * szName) {
    dwHash = StringHashI(szName);
    dwHashFlag = 0;
    return *this;
}

// line 829
HardpointHash & HardpointHash::operator=(uint32 dwHashIn) {
    dwHash = dwHashIn;
    dwHashFlag = 0;
    return *this;
}
