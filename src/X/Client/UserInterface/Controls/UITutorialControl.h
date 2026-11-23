// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UITutorialControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

struct Player;
struct IVector2D;

class UITutorial : public UIWindow {  // size 0x208
public:
    UITutorial(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UITutorial(const WindowCreationParams & tParams, const Rect2D * pRect);
    virtual ~UITutorial();  // vtable slot 0
    const UIID & GetParentID() const { return m_idActiveOwner; }  // line 32
    void GetParentRect(Rect2D & rectParent) { rectParent = m_ParentRect; }  // line 38
    BOOL OwnerIsMissing() const;
    void Update();
    void SetTutorial(const UIID & idParent, SNO snoTutorial, const Player * ptPlayer);
    void SetTutorial(const Rect2D & parentRect, const RActorID idRActorOwner, SNO snoTutorial, const Player * ptPlayer);
    SNO GetTutorial() const { return m_snoTutorial; }  // line 68
    BOOL IsFreestanding() { return m_fIsFreestanding; }  // line 74
    BOOL IsChatBalloon() { return m_fIsChatBalloon; }  // line 80
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 89
        return GetUIIDNone();
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 96
        if (eType == UIT_TUTORIAL)
            return TRUE;
        return UIWindow::IsType(eType);
    }
    virtual void Draw(const uint8, float & flZSlice);  // vtable slot 19
private:
    void sInit();
    void sClearChatBalloonTutorial();
    void SetParentID(const UIID & idParent);
    void sUpdatePosition();
    uint8 sGetAlphaLevel();
public:
    UIID m_idActiveOwner;  // 0x1d0
    RActorID m_idRActorOwner;  // 0x1d8
    uint8 m_bAlpha;  // 0x1dc
    SNO m_snoTutorial;  // 0x1e0
    uint32 m_dwStartTime;  // 0x1e4
    uint32 m_dwTimeBeforeFading;  // 0x1e8
    BOOL m_fIsFreestanding;  // 0x1ec
    BOOL m_fIsChatBalloon;  // 0x1f0
    Rect2D m_ParentRect;  // 0x1f4
    TiledStyle m_eStyleOverride;  // 0x204
};
static_assert(sizeof(UITutorial) == 0x208, "UITutorial");
