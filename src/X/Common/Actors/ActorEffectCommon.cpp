#include "../SNO/SNOName.h"
#include "ActorEffectSwig.h"
#include "../Attributes/FastAttribConstantsSwig.h"
#include "ActorEffectCommon.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include <stddef.h>
// DWARF GameMessage.cpp:65 and D3DescribedType.cpp descriptor definitions.
extern const XTypeDescriptor * DT_REQUIRED_MESSAGE_HEADER, * DT_SNO, * DT_WORLD_PLACE;
// DWARF ActorEffectCommon.h:136/143, absent from the adopted header.
struct PlayNonPositionalSoundMessage { RequiredMessageHeader tHeader; SNO snoSound;
};
struct PlayMusicMessage { RequiredMessageHeader tHeader; SNO snoMusic;
};
// Original inline tree includes SNOName, WorldPlace and GfxVB header initializers.

static const TypeDescriptorField PlayEffectMessage_Fields[6] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayEffectMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayEffectMessage, ann), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayEffectMessage, eEffectType), NULL, (const void *)(EFFECT_INVALID), (const void *)(EFFECT_NUM_EFFECTS - 1)),
    OptionalTypeDescriptorField("", DT_INT, offsetof(PlayEffectMessage, nEffectParameter), NULL, 1),
    OptionalTypeDescriptorField("", DT_INT, offsetof(PlayEffectMessage, annPlayer), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayEffectMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_PlayEffectMessage("PlayEffectMessage", 0, PlayEffectMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAY_EFFECT_MESSAGE = &TypeDescriptor_PlayEffectMessage;

static const TypeDescriptorField PlayHitEffectMessage_Fields[6] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayHitEffectMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayHitEffectMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayHitEffectMessage, annAttacker), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayHitEffectMessage, eDamageType), NULL, (const void *)(DAMAGE_TYPE_NONE), (const void *)(NUM_DAMAGE_TYPES - 1)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(PlayHitEffectMessage, fCrit), NULL, (const void *)(0), (const void *)(1)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayHitEffectMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_PlayHitEffectMessage("PlayHitEffectMessage", 0, PlayHitEffectMessage_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAY_HIT_EFFECT_MESSAGE = &TypeDescriptor_PlayHitEffectMessage;

static const TypeDescriptorField PlayHitEffectOverrideMessage_Fields[5] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayHitEffectOverrideMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayHitEffectOverrideMessage, ann), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PlayHitEffectOverrideMessage, annAttacker), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PlayHitEffectOverrideMessage, snoEffectGroup), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayHitEffectOverrideMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_PlayHitEffectOverrideMessage("PlayHitEffectOverrideMessage", 0, PlayHitEffectOverrideMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAY_HIT_EFFECT_OVERRIDE_MESSAGE = &TypeDescriptor_PlayHitEffectOverrideMessage;

static const TypeDescriptorField PlayNonPositionalSoundMessage_Fields[3] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayNonPositionalSoundMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(PlayNonPositionalSoundMessage, snoSound), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayNonPositionalSoundMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_PlayNonPositionalSoundMessage("PlayNonPositionalSoundMessage", 0, PlayNonPositionalSoundMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAY_NONPOSITIONAL_SOUND_MESSAGE = &TypeDescriptor_PlayNonPositionalSoundMessage;

static const TypeDescriptorField PlayMusicMessage_Fields[3] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(PlayMusicMessage, tHeader), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(PlayMusicMessage, snoMusic), "", MUSIC_SNO, 0, -1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PlayMusicMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_PlayMusicMessage("PlayMusicMessage", 0, PlayMusicMessage_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_PLAY_MUSIC_MESSAGE = &TypeDescriptor_PlayMusicMessage;

static const TypeDescriptorField RopeEffectMessageACDToACD_Fields[8] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RopeEffectMessageACDToACD, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RopeEffectMessageACDToACD, snoRope), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToACD, annFrom), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToACD, ePositionFrom), NULL, (const void *)(0), (const void *)(ROPE_POSITION_COUNT)),
    TypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToACD, annTo), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToACD, ePositionTo), NULL, (const void *)(0), (const void *)(ROPE_POSITION_COUNT)),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToACD, bIgnoreOwnerAlpha), NULL, (const void *)(0), (const void *)(1)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RopeEffectMessageACDToACD), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_RopeEffectMessageACDToACD("RopeEffectMessageACDToACD", 0, RopeEffectMessageACDToACD_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ROPE_EFFECT_MESSAGE_ACD_TO_ACD = &TypeDescriptor_RopeEffectMessageACDToACD;

static const TypeDescriptorField RopeEffectMessageACDToPlace_Fields[7] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(RopeEffectMessageACDToPlace, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(RopeEffectMessageACDToPlace, snoRope), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToPlace, annFrom), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToPlace, ePositionFrom), NULL, (const void *)(0), (const void *)(ROPE_POSITION_COUNT)),
    TypeDescriptorField("", DT_WORLD_PLACE, offsetof(RopeEffectMessageACDToPlace, tPlaceTo), NULL, 1),
    BoundedTypeDescriptorField("", DT_INT, offsetof(RopeEffectMessageACDToPlace, bIgnoreOwnerAlpha), NULL, (const void *)(0), (const void *)(1)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RopeEffectMessageACDToPlace), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_RopeEffectMessageACDToPlace("RopeEffectMessageACDToPlace", 0, RopeEffectMessageACDToPlace_Fields, 7, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ROPE_EFFECT_MESSAGE_ACD_TO_PLACE = &TypeDescriptor_RopeEffectMessageACDToPlace;

static const TypeDescriptorField EffectGroupACDToACDMessage_Fields[5] = { TypeDescriptorField("", DT_REQUIRED_MESSAGE_HEADER, offsetof(EffectGroupACDToACDMessage, tHeader), NULL, 1),
    TypeDescriptorField("", DT_SNO, offsetof(EffectGroupACDToACDMessage, snoEffectGroup), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupACDToACDMessage, annFrom), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(EffectGroupACDToACDMessage, annTo), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EffectGroupACDToACDMessage), NULL, 0) };
ComplexTypeDescriptor TypeDescriptor_EffectGroupACDToACDMessage("EffectGroupACDToACDMessage", 0, EffectGroupACDToACDMessage_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_EFFECT_GROUP_ACD_TO_ACD_MESSAGE = &TypeDescriptor_EffectGroupACDToACDMessage;

