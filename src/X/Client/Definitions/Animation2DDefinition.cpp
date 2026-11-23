// Original file: X/Client/Definitions/Animation2DDefinition.cpp

#include "Animation2DDefinition.h"

// For SNONAME_NONE, cgplaceNull and the RGBAColor normals, whose initializers the original runs in
// this file's static initializer; it reaches them through include chains the DWARF does not record.
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include <stddef.h>

extern const XTypeDescriptor * DT_SNO; extern const XTypeDescriptor * DT_FRAMEANIM;  // D3DescribedType.cpp, GfxCommon.cpp

enum {
    ANIMATION2D_SNO_VERSION = 17,
    ANIMATION2D_XML_VERSION = 1,
};






static const TypeDescriptorField Anim2DFrame_Fields[3] = {
    CharArrayTypeDescriptorField("", offsetof(Anim2DFrame, szName), "", 1, ANIM_FRAME_NAME_LENGTH, -1), TypeDescriptorField("", DT_RGBACOLOR, offsetof(Anim2DFrame, rgbaColor), &crgbaWhite, 1), TypeDescriptorField(NULL, DT_NULL, sizeof(Anim2DFrame), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Anim2DFrame("Anim2DFrame", 0, Anim2DFrame_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ANIM2D_FRAME = &TypeDescriptor_Anim2DFrame;







static const TypeDescriptorField Animation2DDefinition_Fields[8] = {
    TypeDescriptorField("", DT_INT, offsetof(Animation2DDefinition, dwFlags), NULL, 0x80001), TypeDescriptorField("", DT_INT, offsetof(Animation2DDefinition, eCollection), NULL, 1),
    TypeDescriptorField("", DT_FRAMEANIM, offsetof(Animation2DDefinition, tFrameAnim), NULL, 1), FilterTypeDescriptorField("", DT_SNO, offsetof(Animation2DDefinition, snoSound), "", SOUND_SNO, 0, -1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Animation2DDefinition, serFrames), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANIM2D_FRAME, offsetof(Animation2DDefinition, ptFrames), 0x21, (int32)offsetof(Animation2DDefinition, serFrames) - (int32)offsetof(Animation2DDefinition, ptFrames)),
    VariableArrayCountTypeDescriptorField("", offsetof(Animation2DDefinition, nFrameCount), 0x10000, DT_ANIM2D_FRAME, (int32)offsetof(Animation2DDefinition, serFrames) - (int32)offsetof(Animation2DDefinition, nFrameCount)), TypeDescriptorField(NULL, DT_NULL, sizeof(Animation2DDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Animation2DDefinition("Anim2D", ANIMATION2D_XML_VERSION, Animation2DDefinition_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_ANIMATION2D = &TypeDescriptor_Animation2DDefinition;





Animation2DGroupDef * gptGroupDefAnimation2D;




































Animation2DGroupDef::Animation2DGroupDef()
    : GroupDefinition("Anim2D", ANIMATION2D_SNO,
                      SNOVersions(ANIMATION2D_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(Animation2DDefinition), DT_ANIMATION2D, 0x2005, 8)
{
}











uint32 Animation2DGroupDef::GetMaxCount()
{
    return 16;
}








void Animation2DGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    Animation2DDefinition * pdef = static_cast<Animation2DDefinition *>(ptDefaultDef);
    SigmaMemoryClear(pdef, sizeof(Animation2DDefinition));
    pdef->dwFlags = 0;
    pdef->eCollection = 2;
    pdef->tFrameAnim.ePlaybackMode = PLAYBACKMODE_LOOP;
    pdef->tFrameAnim.flFrameRateMin = 0.0f;
    pdef->tFrameAnim.flFrameRateDelta = 0.0f;
    pdef->tFrameAnim.dwStartFrameMin = 0;
    pdef->tFrameAnim.dwStartFrameDelta = 0;
    pdef->snoSound = SNO_NONE;
}
