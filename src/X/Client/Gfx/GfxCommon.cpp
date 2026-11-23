// Original file: X/Client/Gfx/GfxCommon.cpp

#include "GfxCommon.h"

// Not used directly: the original reaches these through an include chain, and their header-level
// statics (blz::ignore, cgplaceNull, the GfxVB.h normals) are part of this file's _GLOBAL__sub_I.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/WorldPlace.h"
#include "GfxVB.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

#include <stddef.h>

// Defined in X/Client/ImageCollection.cpp:949; src/ has no ImageCollection.h yet.
BOOL ImageCollectionGetImageInfo(LPCSTR szDecoratedLabel, ImageTextureFrame & tImageTexFrame);

// Defined in X/Common/D3TypeDescriptor/D3DescribedType.cpp; no adopted header declares them yet.
extern const XTypeDescriptor * DT_VELOCITY;
extern const XTypeDescriptor * DT_ANGULARVELOCITY;

enum {
    DISPLAYMODE_XML_VERSION = 1,
    TEX_ANIM_PARAMS_XML_VERSION = 1,
};

const TexAnimParams cgtTexAnimParamsDefault = {  // line 30
    TEXANIMTYPE_FIXED,
    Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
};

// The field names are "" in the binary.
static const TypeDescriptorField TexFrame_Fields[6] = {  // line 37
    TypeDescriptorField("", DT_INT, offsetof(TexFrame, hImageHandle), NULL, 0x700000),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexFrame, flU0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexFrame, flV0), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexFrame, flU1), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexFrame, flV1), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TexFrame), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TexFrame("TexFrame", 0, TexFrame_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 44
const XTypeDescriptor * DT_TEXFRAME = &TypeDescriptor_TexFrame;

const TexFrame cgtTexFrameDefault = { 0, 0.0f, 0.0f, 1.0f, 1.0f };  // line 49

const ImageTextureFrame cgtImageTextureFrameNull;  // line 54

static const TypeDescriptorField FrameAnim_Fields[6] = {  // line 71
    TypeDescriptorField("", DT_INT, offsetof(FrameAnim, ePlaybackMode), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(FrameAnim, flFrameRateMin), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(FrameAnim, flFrameRateDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FrameAnim, dwStartFrameMin), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(FrameAnim, dwStartFrameDelta), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(FrameAnim), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_FrameAnim("FrameAnim", 2, FrameAnim_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 77
const XTypeDescriptor * DT_FRAMEANIM = &TypeDescriptor_FrameAnim;

static const TypeDescriptorField TexAnimParams_Fields[18] = {  // line 79
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, eTexAnimType), NULL, 1),
    TypeDescriptorField("", DT_MATRIX4x4, offsetof(TexAnimParams, mTex), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexAnimParams, flUTransInitial), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(TexAnimParams, flUTransVel), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(TexAnimParams, flUTransVelDelta), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(TexAnimParams, flVTransInitial), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(TexAnimParams, flVTransVel), NULL, 1),
    TypeDescriptorField("", DT_VELOCITY, offsetof(TexAnimParams, flVTransVelDelta), NULL, 1),
    TypeDescriptorField("", DT_ANGLE, offsetof(TexAnimParams, aRotInitial), NULL, 1),
    TypeDescriptorField("", DT_ANGULARVELOCITY, offsetof(TexAnimParams, aRotVel), NULL, 1),
    TypeDescriptorField("", DT_ANGULARVELOCITY, offsetof(TexAnimParams, aRotVelDelta), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, fPinTranslation), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, fUseObjectValueForU), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, fUseObjectValueForV), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, fScaleUVsToFrame), NULL, 1),
    TypeDescriptorField("", DT_FRAMEANIM, offsetof(TexAnimParams, tFrameAnim), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(TexAnimParams, dwAnimFlags), NULL, 0x80001),
    TypeDescriptorField(NULL, DT_NULL, sizeof(TexAnimParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_TexAnimParams("TexAnimParams", TEX_ANIM_PARAMS_XML_VERSION, TexAnimParams_Fields, 18, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 98
const XTypeDescriptor * DT_TEXANIMPARAMS = &TypeDescriptor_TexAnimParams;

static const TypeDescriptorField DisplayMode_Fields[13] = {  // line 100
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwWindowMode), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, nWinLeft), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, nWinTop), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, nWinWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, nWinHeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwUIOptWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwUIOptHeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwWidth), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwHeight), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, nRefreshRate), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(DisplayMode, dwBitDepth), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DisplayMode), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DisplayMode("DisplayMode", DISPLAYMODE_XML_VERSION, DisplayMode_Fields, 13, NULL, 0x80, NULL, NULL, NULL, NULL);  // line 113
const XTypeDescriptor * DT_DISPLAYMODE = &TypeDescriptor_DisplayMode;

const DisplayMode cgtDisplayModeDefault = {  // line 115
    (1 << DISPLAYMODE_IS_DEFAULT_BIT) | (1 << DISPLAYMODE_SCREENSHOT_ENABLE_BIT),
    WINDOWMODE_FULLSCREEN,
    0,
    0,
    1024,
    768,
    1024,
    768,
    1024,
    768,
    60,
    32,
    0,
    0.0f,
};

ImageTextureFrame::ImageTextureFrame(LPCSTR szDecoratedLabel) {  // line 132
    snoTexture = SNO_NONE;
    nFrame = -1;
    ImageCollectionGetImageInfo(szDecoratedLabel, *this);
}
