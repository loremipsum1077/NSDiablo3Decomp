// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIProgressBarControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Gfx/GfxCommon.h"
#include "UIWindowControl.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct IVector2D;
struct Rect2D;
struct UIControlDataExt;
struct Vector2D;

class ProgressBarCreationParams : public WindowCreationParams {  // size 0x750
public:
    ProgressBarCreationParams(const UIType, const UIID &, const UIID &);
    ProgressBarCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    BOOL fIsVertical;  // 0x730
    BOOL fDrawReversed;  // 0x734
    float flMinPos;  // 0x738
    float flMaxPos;  // 0x73c
    ImageTextureFrame tImageTexProgressBackground;  // 0x740
    ImageTextureFrame tImageTexProgressForeground;  // 0x748
private:
    void sInit();
};
static_assert(sizeof(ProgressBarCreationParams) == 0x750, "ProgressBarCreationParams");

class UIProgressBar : public UIWindow {  // size 0x218
public:
    UIProgressBar(const ProgressBarCreationParams &, const Vector2D &, const Vector2D &);
    UIProgressBar(const ProgressBarCreationParams &, const Rect2D *);
    virtual ~UIProgressBar();  // vtable slot 0
    float GetPos() const { return m_flCurrentPos; }  // line 64
    float GetTargetPos() const { return m_flTargetPos; }  // line 71
    void SetPos(const float flPos, const float flTarget);
    void SetExtents(const float flMin, const float flMax);
    void SetExtentsAndPosition(const float flMin, const float flMax, const float flPos, const float flTarget) {  // line 94
        SetExtents(flMin, flMax);
        SetPos(flPos, flTarget);
    }
    void SetForegroundRActor(const SNO snoActor, const RenderContext eContext);
    void SetTargetRActor(const SNO, const RenderContext);
    UIID GetForegroundChild() const { return m_idForegroundChild; }  // line 129
    UIID GetBackgroundChild() const;
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual UIID HitTest(const IVector2D & pt) {  // vtable slot 15; line 146
        if (IsActive() && IsInside(pt) && EatsClicks() != EC_IGNORE)
        {
            return GetID();
        }
        return GetUIIDNone();
    }
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL &);  // vtable slot 16
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 166
        return eType == UIT_PROGRESSBAR || UIWindow::IsType(eType);
    }
    virtual void PostCreate();  // vtable slot 18
    static UIID Create(const ProgressBarCreationParams &, const Vector2D &, const Vector2D &, LPCSTR, LPCSTR);
    static UIID Create(const ProgressBarCreationParams &, const Rect2D &, LPCSTR, LPCSTR);
    virtual BOOL UpdateCachedActiveState();  // vtable slot 11
private:
    void sInit(const ProgressBarCreationParams & tParams, const Vector2D &, const Vector2D & vecDim);
    static void sCreate(UIProgressBar &, LPCSTR, LPCSTR);
    float m_flCurrentPos;  // 0x1cc
    float m_flTargetPos;  // 0x1d0
    BOOL m_bUseCustomExtents;  // 0x1d4
    float m_flCustomMin;  // 0x1d8
    float m_flCustomMax;  // 0x1dc
    UIID m_idForegroundChild;  // 0x1e0
    UIID m_idBackgroundChild;  // 0x1e8
    UIID m_idLeftEndcapChild;  // 0x1f0
    UIID m_idRightEndcapChild;  // 0x1f8
    UIID m_idTargetChild;  // 0x200
    BOOL m_fIsVertical;  // 0x208
    BOOL m_fDrawReversed;  // 0x20c
    float m_flMinPos;  // 0x210
    float m_flMaxPos;  // 0x214
    float sGetMinPos();
    float sGetMaxPos();
    void sOnReposition();
    void sSetRActor(const UIID & id, const SNO snoActor, const RenderContext eContext);
};
static_assert(sizeof(UIProgressBar) == 0x218, "UIProgressBar");
