// Original file: X/Common/Definitions/AnimSetDefinition.cpp

#include "AnimSetDefinition.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer.
#include "../SNO/SNOName.h"
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../ActorCommonData.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../TagMaps/AnimTags.h"
#include "../TagMaps/Tags.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SNO;

// Defined in X/Common/SNO/SNOFiles.cpp:5565; no adopted header declares it yet.
BOOL SNOFileExists(const SNOName & snoname);






enum {
    ANIMSET_COMMON_SNO_VERSION = 24,
    ANIMSET_XML_VERSION = 0,
    ANIMSET_PC_SNO_VERSION = 24,
    ANIMSET_CONSOLE_SNO_VERSION = 27,
    ANIMSET_AXE_SNO_VERSION = 26,
};

static const TypeDescriptorField AnimSetTagMap_Fields[3] = {  // line 36
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimSetTagMap, serTagMap), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(AnimSetTagMap, hTagMap), 0x21,
                              (int32)offsetof(AnimSetTagMap, serTagMap) - (int32)offsetof(AnimSetTagMap, hTagMap), 0xc0000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimSetTagMap), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimSetTagMap("AnimSetTagMap", 0, AnimSetTagMap_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 38
const XTypeDescriptor * DT_ANIMSETTAGMAP = &TypeDescriptor_AnimSetTagMap;

static const TypeDescriptorField AnimSetDefinition_Fields[5] = {  // line 40
    FilterTypeDescriptorField("", DT_SNO, offsetof(AnimSetDefinition, snoParentAnimSet), "", ANIMSET_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimSetDefinition, serTagMapAnimDefault), NULL, 0),
    TagMapTypeDescriptorField("", offsetof(AnimSetDefinition, hTagMapAnimDefault), 0x21,
                              (int32)offsetof(AnimSetDefinition, serTagMapAnimDefault) - (int32)offsetof(AnimSetDefinition, hTagMapAnimDefault),
                              0xc0000),
    FixedArrayTypeDescriptorField("", DT_ANIMSETTAGMAP, offsetof(AnimSetDefinition, tWeaponAnimTagMap), NULL, 1, 28, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimSetDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimSetDefinition("AnimSet", ANIMSET_XML_VERSION, AnimSetDefinition_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 45
const XTypeDescriptor * DT_ANIMSET = &TypeDescriptor_AnimSetDefinition;

AnimSetGroupDef * gptGroupDefAnimSet;  // line 51

// line 57
AnimSetGroupDef::AnimSetGroupDef()
    : GroupDefinition("AnimSet", ANIMSET_SNO,
                      SNOVersions(ANIMSET_COMMON_SNO_VERSION, 28, ANIMSET_CONSOLE_SNO_VERSION, ANIMSET_CONSOLE_SNO_VERSION, 28),
                      SNODATATYPE_TITLE, sizeof(AnimSetDefinition), DT_ANIMSET, 0x4043, 0x100e) {
}

// line 82
uint32 AnimSetGroupDef::GetMaxCount() {
    return 256;
}

// line 94
void AnimSetGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(AnimSetDefinition));
    AnimSetDefinition * pdefAnimSet = (AnimSetDefinition *)ptDefaultDef;
    pdefAnimSet->snoParentAnimSet = SNO_NONE;
}

// line 106
SNO AnimSetGetAnim(SNO snoAnimSet, const WeaponClass eWeaponClass, const uint32 dwAnimTagId) {
    SNO snoAnim = SNO_NONE;

    if (!TagMapGetTagInfo(TAG_ANIMATION, dwAnimTagId))
        return SNO_NONE;

    if (snoAnimSet != SNO_NONE) {
        ScopedAnimSetDefinitionLockR pdefAnimSet(snoAnimSet);

        if (eWeaponClass != WEAPONCLASS_NONE)
            snoAnim = TagMapGetSNO(pdefAnimSet->tWeaponAnimTagMap[eWeaponClass].hTagMap, dwAnimTagId);

        if (snoAnim == SNO_NONE && TagMapTagExists(pdefAnimSet->hTagMapAnimDefault, dwAnimTagId))
            snoAnim = TagMapGetSNO(pdefAnimSet->hTagMapAnimDefault, dwAnimTagId);

        if (snoAnim == SNO_NONE && pdefAnimSet->snoParentAnimSet != SNO_NONE) {
            if (pdefAnimSet->snoParentAnimSet == snoAnimSet)
                return SNO_NONE;
            return AnimSetGetAnim(pdefAnimSet->snoParentAnimSet, eWeaponClass, dwAnimTagId);
        }
    }

    return snoAnim;
}

// line 155
static SNO sGetAnimSNO(const CTagMap * hTagMap, SNO snoAnimSet, uint32 uTag) {
    SNO snoAnim = SNO_NONE;

    if (TagMapTagExists(hTagMap, uTag)) {
        snoAnim = TagMapGetSNO(hTagMap, uTag);
        SNOName snoname(ANIMATION_SNO, snoAnim);
        if (snoAnim != SNO_NONE && !SNOFileExists(snoname)) {
            // The original reports the missing animation here; only the tag lookup survives.
            TagMapGetTagInfo(hTagMap, uTag);
            snoAnim = SNO_NONE;
        }
    }

    return snoAnim;
}

// line 182
void AnimSetGetSpecificAnimSet(const SNO snoAnimSet, const WeaponClass eWeaponClass, const uint32 dwTag, SNO * psnoAnimSet,
                               uint32 * pdwAnimSetTag) {
    CTagMap * hTagMapToUse;
    SNO snoAnim;
    uint32 dwTagStart;
    uint32 dwTagEnd;
    ScopedAnimSetDefinitionLockR pdefAnimSet(snoAnimSet);

    hTagMapToUse = pdefAnimSet->hTagMapAnimDefault;

    if (eWeaponClass != WEAPONCLASS_NONE) {
        SNO snoAnim = TagMapGetSNO(pdefAnimSet->tWeaponAnimTagMap[eWeaponClass].hTagMap, dwTag);
        if (snoAnim != SNO_NONE)
            hTagMapToUse = pdefAnimSet->tWeaponAnimTagMap[eWeaponClass].hTagMap;
    }

    snoAnim = TagMapGetSNO(hTagMapToUse, dwTag);
    if (snoAnim == SNO_NONE && pdefAnimSet->snoParentAnimSet != SNO_NONE) {
        AnimSetGetSpecificAnimSet(pdefAnimSet->snoParentAnimSet, eWeaponClass, dwTag, psnoAnimSet, pdwAnimSetTag);
        return;
    }

    AnimTagGetStartAndEndTags(dwTag, dwTagStart, dwTagEnd);

    psnoAnimSet[0] = sGetAnimSNO(hTagMapToUse, snoAnimSet, dwTag);
    pdwAnimSetTag[0] = dwTag;
    psnoAnimSet[1] = sGetAnimSNO(hTagMapToUse, snoAnimSet, dwTagStart);
    pdwAnimSetTag[1] = dwTagStart;
    psnoAnimSet[2] = sGetAnimSNO(hTagMapToUse, snoAnimSet, dwTagEnd);
    pdwAnimSetTag[2] = dwTagEnd;
}
