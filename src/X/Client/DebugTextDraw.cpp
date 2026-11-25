// X/Client/DebugTextDraw.cpp
// Decompiled from the Diablo III 2.6.2 (Switch) binary (clang 5.0.1).
//
// Evidence: DWARF + line-attributed listings of
//   _Z19DebugTextInitializev                          0x7100089f10   68 bytes
//   _Z16DebugTextDisposev                             0x7100089f60   80 bytes
//   _Z15DebugTextSubmitPKcRK8Vector3DRK7RWindow       0x7100089fb0  312 bytes
//   _ZN3blz6vectorI9DebugTextNS_9allocatorIS1_EEE9push_backERKS1_
//                                                     0x710008a0f0  308 bytes
//
// DebugText, DebugTextList and DebugTextDrawGlobals are file-local: the DWARF
// attributes them to this file and there is no DebugTextDraw.h.
//
// NON_MATCHING: the DWARF skeleton also lists two file-local statics,
//   line 123  sCheckCollideWithList(Rect2D&, RenderContext, int32, int32, const Rect2D&, float, float)
//   line 185  sFixOverlap(RenderContext)
// Their bodies are not in the DWARF and they have no entry in the inventory, so
// no bodies are invented for them here.
//
// NON_MATCHING (resolved by inspection, unverified here): the tail of
// DebugTextSubmit (lines 111-113) reads the freshly stored tNewText.* fields
// rather than vecTextSizeUIC, and the line-113 Set() operands are ordered
// "flX + width" / "height + flY" per the specialist guidance and the original
// operand order (fadd s3,s2,s3 ; fadd s0,s0,s1).
//
// NON_MATCHING (needs_evidence): the explicit instantiation of
// blz::vector<DebugText>::push_back was removed. push_back is a non-inline
// out-of-class member template (vector.h:528) that the call at line 117 forces
// out-of-line; if the checker finds 0x710008a0f0 missing or DebugTextSubmit
// inlined, this needs inlining research (do not simply re-add the instantiation).

#include <new>

#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Gfx/GfxCommon.h"
#include "../Sigma/Math/Rect.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"
#include "../Sigma/Unicode.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Common/GlobalSNO.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "CGame.h"
#include "RWindow.h"
#include "Font/Font.h"
#include "Font/FormattedText.h"
#include "Camera/CameraManager.h"

// line 26
enum {
    MAX_OVERLAP_TRIALS = 12,
};

struct DebugText {  // size 0x128
    // line 30. The inlined copy in push_back proves this is a memberwise copy
    // (memcpy for uszText, Vector2D::operator=, Rect2D::operator= in declaration
    // order); the implicit copy assignment is the DWARF's line-30 operator=.
    CHAR uszText[256];  // 0x00
    Vector2D vecOriginalPosUIC;  // 0x100
    float flWidthUIC;  // 0x108
    float flHeightUIC;  // 0x10c
    Rect2D rectUIC;  // 0x110
    BOOL fDraw;  // 0x120
    RenderContext eContext;  // 0x124
};
static_assert(sizeof(DebugText) == 0x128, "DebugText");

typedef blz::vector<DebugText, blz::allocator<DebugText> > DebugTextList;

struct DebugTextDrawGlobals {  // size 0x18
    DebugTextList listText;  // 0x00
};
static_assert(sizeof(DebugTextDrawGlobals) == 0x18, "DebugTextDrawGlobals");

static const float TEXT_HOR_PAD = 10.0f;  // line 49
static const float TEXT_VERT_PAD = 2.0f;  // line 50
static const GlobalSNO scgeFont = 645;  // line 51
static const int32 scgnFontSize = 9;  // line 52

// line 62
void DebugTextInitialize() {
    DebugTextDrawGlobals *& ptGlobals = CGameGet().ptDebugTextDrawGlobals;  // line 65
    ptGlobals = (DebugTextDrawGlobals *)SigmaMemoryNew(sizeof(DebugTextDrawGlobals), 4, NULL, TRUE);  // line 66
    new (ptGlobals) DebugTextDrawGlobals;  // line 67
}

// line 73
void DebugTextDispose() {
    DebugTextDrawGlobals *& ptGlobals = CGameGet().ptDebugTextDrawGlobals;
    ptGlobals->listText.~DebugTextList();  // line 77
    SigmaMemoryFree(ptGlobals, NULL);  // line 78
    ptGlobals = NULL;  // line 78
}

// line 84
void DebugTextSubmit(LPCSTR szText, const Vector3D & wp, const RWindow & tRWindow) {
    Vector2D vecUIC;
    DebugTextDrawGlobals * ptGlobals;
    BOOL bResult;
    DebugText tNewText;

    ptGlobals = CGameGet().ptDebugTextDrawGlobals;  // line 89
    bResult = WorldPointToScreenPointUIC(wp, vecUIC, tRWindow.GetCameraIndex());  // line 92
    if (!bResult)  // line 93
        return;

    Blizzard::String::Copy(tNewText.uszText, szText, sizeof(tNewText.uszText));  // line 99
    tNewText.vecOriginalPosUIC = vecUIC;  // line 101
    tNewText.eContext = tRWindow.GetRenderContext();  // line 102

    Vector2D vecTextSizeUIC;
    TextRenderProperties tProps(GlobalSNOGet(scgeFont), scgnFontSize, 1.0f, 1.0f);  // line 105
    FontStringGetRenderedSizeUIC(tProps, FormattedText(tNewText.uszText, PARSE_DEFAULT, FALSE), vecTextSizeUIC);  // line 106

    tNewText.flWidthUIC = vecTextSizeUIC.x;  // line 108
    tNewText.flHeightUIC = vecTextSizeUIC.y;  // line 109

    float flX = vecUIC.x - tNewText.flWidthUIC * 0.5f;  // line 111
    float flY = vecUIC.y - tNewText.flHeightUIC * 0.5f;  // line 112
    tNewText.rectUIC.Set(flX, flY, flX + tNewText.flWidthUIC, tNewText.flHeightUIC + flY);  // line 113
    tNewText.fDraw = FALSE;  // line 115

    ptGlobals->listText.push_back(tNewText);  // line 117
}
