// Original file: X/Common/Definitions/AnimationDefinition.cpp

#include "AnimationDefinition.h"

// For cgplaceNull, whose initializer the original runs in this file's static initializer.
#include "../WorldPlace.h"
#include "../../Client/Gfx/GfxVB.h"
#include "../D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Contrib/Contrib/blz/include/blz/tuple.h, which the original reaches through an include chain
// not adopted here. Its header-level static blz::ignore sits in front of this file's own statics.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; no adopted header declares it yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_SNO;
extern const XTypeDescriptor * DT_TIME;
extern const XTypeDescriptor * DT_VELOCITY;
extern const XTypeDescriptor * DT_VELOCITYVECTOR3D;

// X/Client/Definitions/AppearanceDefinition.h (not adopted). Only the members read here are
#include "../../Client/Definitions/AppearanceDefinition.h"












enum {
    ANIMATION_COMMON_SNO_VERSION = 117,
    ANIMATION_XML_VERSION = 4,
    ANIMATION_PC_SNO_VERSION = 118,
    ANIMATION_CONSOLE_SNO_VERSION = 128,
    ANIMATION_PS4_SNO_VERSION = 130,
    ANIMATION_AXE_SNO_VERSION = 126,
    ANIMPERMUTATION_XML_VERSION = 6,
};
static const Vector3D scgwpDefaultContactOffset(0.0f, 0.0f, 4.0f);

static const float cgnBoneBlendWeightsDefault[2] = { 1.0f, 1.0f };




















static const TypeDescriptorField TranslationKey_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(TranslationKey, nFrame), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(TranslationKey, wp), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TranslationKey), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TranslationKey("TranslationKey", 0, TranslationKey_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRANSLATIONKEY = &TypeDescriptor_TranslationKey;

static const TypeDescriptorField RotationKey_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(RotationKey, nFrame), NULL, 1),
    TypeDescriptorField("", DT_QUATERNION16, offsetof(RotationKey, q16), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RotationKey), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RotationKey("RotationKey", 0, RotationKey_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ROTATIONKEY = &TypeDescriptor_RotationKey;

static const TypeDescriptorField ScaleKey_Fields[3] = {
    TypeDescriptorField("", DT_INT, offsetof(ScaleKey, nFrame), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ScaleKey, flScale), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ScaleKey), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ScaleKey("ScaleKey", 0, ScaleKey_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SCALEKEY = &TypeDescriptor_ScaleKey;

static const TypeDescriptorField TranslationCurve_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(TranslationCurve, serKeysComp), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(TranslationCurve, ptKeysComp), 0x700060,
                                     (int32)offsetof(TranslationCurve, serKeysComp) - (int32)offsetof(TranslationCurve, ptKeysComp)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TranslationCurve), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TranslationCurve("TranslationCurve", 0, TranslationCurve_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_TRANSLATIONCURVE = &TypeDescriptor_TranslationCurve;

static const TypeDescriptorField RotationCurve_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(RotationCurve, serKeysComp), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(RotationCurve, ptKeysComp), 0x700060,
                                     (int32)offsetof(RotationCurve, serKeysComp) - (int32)offsetof(RotationCurve, ptKeysComp)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(RotationCurve), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_RotationCurve("RotationCurve", 0, RotationCurve_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ROTATIONCURVE = &TypeDescriptor_RotationCurve;

static const TypeDescriptorField ScaleCurve_Fields[3] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ScaleCurve, serKeysComp), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(ScaleCurve, ptKeysComp), 0x700060,
                                     (int32)offsetof(ScaleCurve, serKeysComp) - (int32)offsetof(ScaleCurve, ptKeysComp)),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ScaleCurve), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ScaleCurve("ScaleCurve", 0, ScaleCurve_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_SCALECURVE = &TypeDescriptor_ScaleCurve;

static const TypeDescriptorField KeyframedAttachment_Fields[3] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(KeyframedAttachment, flKeyframeIndex), NULL, 1),
    TypeDescriptorField("", DT_TRIGGEREVENT, offsetof(KeyframedAttachment, tTriggeredEvent), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(KeyframedAttachment), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_KeyframedAttachment("KeyframedAttachment", 2, KeyframedAttachment_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_KEYFRAMED_ATTACHMENT = &TypeDescriptor_KeyframedAttachment;

static const TypeDescriptorField BoneName_Fields[2] = {
    TypeDescriptorField("", DT_INT, offsetof(BoneName, dwHash), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BoneName), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BoneName("BoneName", 0, BoneName_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BONENAME = &TypeDescriptor_BoneName;

static const TypeDescriptorField BoneBlendWeights_Fields[2] = {
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(BoneBlendWeights, aflWeight), cgnBoneBlendWeightsDefault, 1, ABW_MAX, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(BoneBlendWeights), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_BoneBlendWeights("BoneBlendWeights", 0, BoneBlendWeights_Fields, 2, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_BONEBLENDWEIGHTS = &TypeDescriptor_BoneBlendWeights;

static const TypeDescriptorField AnimPermutation_Fields[45] = {
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, dwFlags), NULL, 0x80001),
    CharArrayTypeDescriptorField("", offsetof(AnimPermutation, szPermutationName), "", 1, sizeof(((AnimPermutation *)0)->szPermutationName), -1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(AnimPermutation, flFrameRate), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flCompression), (const void *)0x40a00000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flTranslationCompressionRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flRotationCompressionRatio), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flScaleCompressionRatio), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AnimPermutation, dwBlendTime), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(AnimPermutation, dwFromPermBlendTime), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flSpeedMultMin), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flSpeedMultDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flRagdollVelocityFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flRagdollMomentumFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, dwWeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, nBoneCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serBoneNames), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_BONENAME, offsetof(AnimPermutation, ptBoneNames), 0x60,
                                     (int32)offsetof(AnimPermutation, serBoneNames) - (int32)offsetof(AnimPermutation, ptBoneNames)),
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, nNumBoneBlendWeights), NULL, 0x700001),
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, nPad), NULL, 0x700000),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serBoneBlendWeights), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BONEBLENDWEIGHTS, offsetof(AnimPermutation, ptBoneBlendWeights), 0x700061,
                                     (int32)offsetof(AnimPermutation, serBoneBlendWeights) - (int32)offsetof(AnimPermutation, ptBoneBlendWeights)),
    TypeDescriptorField("", DT_INT, offsetof(AnimPermutation, nKeyframeCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serTranslationCurves), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_TRANSLATIONCURVE, offsetof(AnimPermutation, ptTranslationCurves), 0x60,
                                     (int32)offsetof(AnimPermutation, serTranslationCurves) - (int32)offsetof(AnimPermutation, ptTranslationCurves)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serRotationCurves), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ROTATIONCURVE, offsetof(AnimPermutation, ptRotationCurves), 0x60,
                                     (int32)offsetof(AnimPermutation, serRotationCurves) - (int32)offsetof(AnimPermutation, ptRotationCurves)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serScaleCurves), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_SCALECURVE, offsetof(AnimPermutation, ptScaleCurves), 0x60,
                                     (int32)offsetof(AnimPermutation, serScaleCurves) - (int32)offsetof(AnimPermutation, ptScaleCurves)),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flContactKeyframe[0]), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flContactKeyframe[1]), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flContactKeyframe[2]), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flContactKeyframe[3]), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AnimPermutation, wpContactOffset[0]), &scgwpDefaultContactOffset, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AnimPermutation, wpContactOffset[1]), &scgwpDefaultContactOffset, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AnimPermutation, wpContactOffset[2]), &scgwpDefaultContactOffset, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(AnimPermutation, wpContactOffset[3]), &scgwpDefaultContactOffset, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(AnimPermutation, flEarliestInterruptKeyFrame), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serKeyedAttachments), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_KEYFRAMED_ATTACHMENT, offsetof(AnimPermutation, ptKeyedAttachments), 0x61,
                                     (int32)offsetof(AnimPermutation, serKeyedAttachments) - (int32)offsetof(AnimPermutation, ptKeyedAttachments)),
    VariableArrayCountTypeDescriptorField("", offsetof(AnimPermutation, nKeyedAttachmentCount), 0x10000, DT_KEYFRAMED_ATTACHMENT,
                                          (int32)offsetof(AnimPermutation, serKeyedAttachments) - (int32)offsetof(AnimPermutation, nKeyedAttachmentCount)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimPermutation, serNonlinearOffset), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VECTOR3D, offsetof(AnimPermutation, pwvNonlinearOffset), 0x61,
                                     (int32)offsetof(AnimPermutation, serNonlinearOffset) - (int32)offsetof(AnimPermutation, pwvNonlinearOffset)),
    TypeDescriptorField("", DT_VELOCITYVECTOR3D, offsetof(AnimPermutation, wvAvgVel), NULL, 1),
    TypeDescriptorField("", DT_HARDPOINTLINK, offsetof(AnimPermutation, tHardpointLink), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimPermutation), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimPermutation("AnimPermutation", ANIMPERMUTATION_XML_VERSION, AnimPermutation_Fields, 45, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ANIMPERMUTATION = &TypeDescriptor_AnimPermutation;

static const TypeDescriptorField AnimationDefinition_Fields[8] = {
    TypeDescriptorField("", DT_INT, offsetof(AnimationDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(AnimationDefinition, ePlaybackMode), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(AnimationDefinition, snoAppearance), "", APPEARANCE_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(AnimationDefinition, serPermutations), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANIMPERMUTATION, offsetof(AnimationDefinition, ptPermutations), 0x21,
                                     (int32)offsetof(AnimationDefinition, serPermutations) - (int32)offsetof(AnimationDefinition, ptPermutations)),
    VariableArrayCountTypeDescriptorField("", offsetof(AnimationDefinition, nPermutationCount), 0x10000, DT_ANIMPERMUTATION,
                                          (int32)offsetof(AnimationDefinition, serPermutations) - (int32)offsetof(AnimationDefinition, nPermutationCount)),
    TypeDescriptorField("", DT_INT, offsetof(AnimationDefinition, dwMachineTime), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(AnimationDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_AnimationDefinition("Anim", ANIMATION_XML_VERSION, AnimationDefinition_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_ANIMATION = &TypeDescriptor_AnimationDefinition;
AnimationGroupDef * gptGroupDefAnimation;
AnimationGroupDef::AnimationGroupDef()
    : GroupDefinition("Anim", ANIMATION_SNO,
                      SNOVersions(ANIMATION_PC_SNO_VERSION, 128, ANIMATION_CONSOLE_SNO_VERSION, ANIMATION_PS4_SNO_VERSION, 129),
                      SNODATATYPE_TITLE, sizeof(AnimationDefinition), DT_ANIMATION, 0x401, 0x944e) {
}
AnimationGroupDef::~AnimationGroupDef() {
}
uint32 AnimationGroupDef::GetMaxCount() {
    return 256;
}
void AnimationGroupDef::GetDefaultData(void * ptDefaultDef) const {
    SigmaMemoryClear(ptDefaultDef, sizeof(AnimationDefinition));

    AnimationDefinition * pdef = (AnimationDefinition *)ptDefaultDef;
    pdef->ePlaybackMode = PLAYBACKMODE_NORMAL;
    pdef->snoAppearance = SNO_NONE;
}




















































































































































































































































































































































































































































































































































































































































































































































































































































































































































BOOL AnimationGroupDef::Write(void * lpDefinition, FileReference & tRef) const {





    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}






void AnimationGroupDef::Postprocess(void * lpDefinition) {
    GroupDefinition::Postprocess(lpDefinition);

    AnimationDefinition * pdefAnim = (AnimationDefinition *)lpDefinition;

    for (int32 nPermutationIndex = 0; nPermutationIndex < pdefAnim->nPermutationCount; nPermutationIndex++) {


        AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[nPermutationIndex];




























        tAnimPerm.pnAnimToStructure = NULL;
    }
}




void AnimationGroupDef::Delete(void * lpDefinition) const {
    AnimationDefinition * pdefAnim = (AnimationDefinition *)lpDefinition;

    for (int32 nPermutationIndex = 0; nPermutationIndex < pdefAnim->nPermutationCount; nPermutationIndex++) {



        AnimPermutation & tAnimPerm = pdefAnim->ptPermutations[nPermutationIndex];
        XFREE(tAnimPerm.pnAnimToStructure, NULL);
    }

    GroupDefinition::Delete(lpDefinition);
}
















void AnimationBuildAnimToStructureMapping(const Structure & tStructure, const int32 nBoneCount, const BoneName * ptBoneNames, int16 * pnAnimToStructure, BOOL * pfOneToOne) {
    BOOL fOneToOne = TRUE;

    for (int16 i = 0; i < nBoneCount; i++) {
        BOOL fFound = FALSE;
        pnAnimToStructure[i] = -1;

        for (int16 j = 0; j < tStructure.nBoneCount; j++) {
            if (tStructure.ptBoneStructure[j].dwHash == ptBoneNames[i].dwHash) {
                if (i != j)
                    fOneToOne = FALSE;
                pnAnimToStructure[i] = j;
                fFound = TRUE;
                break;
            }
        }

        if (!fFound)
            fOneToOne = FALSE;
    }

    if (pfOneToOne)
        *pfOneToOne = fOneToOne;
}
