// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIStoryboard.h

#pragma once

#include "Controls/UIControlControl.h"
#include "../../Sigma/DataStructures/XGrowableList.h"

struct UIControlDataExt;
struct Rect2D;
struct Vector2D;

enum {
    APROP_NONE = -1,
    APROP_LEFT = 0,
    APROP_TOP = 1,
    APROP_WIDTH = 2,
    APROP_HEIGHT = 3,
    APROP_LEFT_AND_TOP = 4,
    APROP_WIDTH_AND_HEIGHT = 5,
    APROP_ALPHA = 6,
    APROP_BLINKER_ALPHA_SPEED = 7,
    APROP_BLINKER_ROTATION_SPEED = 8,
};

typedef int32 UIAnimationProperty;

// Defined in UIStoryboard.cpp.
template <typename C, int eAnimType> class ManagedAnimation;

typedef ManagedAnimation<int, 0> UIInt32Animation;

typedef ManagedAnimation<float, 1> UIFloatAnimation;

typedef ManagedAnimation<Vector2D, 2> UIVector2DAnimation;

class StoryboardCreationParams : public ControlCreationParams {  // size 0xb8
public:
    StoryboardCreationParams(const UIType eType, const UIID & id, const UIID & idParent);
    StoryboardCreationParams(const UIType, LPCSTR, const UIID &);
    virtual ~StoryboardCreationParams() {}  // line 95
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    uint32 dwDuration;  // 0xa8
    SNO snoPlaybackSound;  // 0xac
    UIFunctionPtr pfnOnEnd;  // 0xb0
private:
    void sInit();
};
static_assert(sizeof(StoryboardCreationParams) == 0xb8, "StoryboardCreationParams");

class UIStoryboard : public UIControl {  // size 0xd8
    UIStoryboard(const UIStoryboard &);
    UIStoryboard & operator=(const UIStoryboard &);
public:
    UIStoryboard(const StoryboardCreationParams & tParams);
    virtual ~UIStoryboard();  // vtable slot 0
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 133
        return eType == UIT_STORYBOARD || UIControl::IsType(eType);
    }
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    BOOL Play(const BOOL bLoop, const float flInitialElapsedTime);
    BOOL Stop();
    BOOL IsPlaying() const { return m_bPlaying; }  // line 147
    BOOL IsLooping() const;
    void SkipToEnd();
    void Reset() {  // line 152
        m_flElapsedTime = 0.0f;
        Advance(0.0f);
    }
    void SetDuration(const float);
    float GetDuration() const { return m_flTotalDurationTime; }  // line 163
    void SetPlaybackSound(SNO);
    BOOL BindAnimation(const UIID & idControl, LPCSTR szAnimId, UIAnimationProperty eAnimProperty, float flScaleFactor);
    BOOL BindAnimation(const UIID & tUIControlId, const UIID & tAnimId, UIAnimationProperty eAnimProperty, float flScaleFactor);
    BOOL UnbindAnimation(const UIID &, LPCSTR, UIAnimationProperty);
    BOOL UnbindAnimation(const UIID &, const UIID &, UIAnimationProperty);
    void ClearAnimationBindings();
    void SetOnEndCallback(UIFunctionPtr pfnOnEnd) { m_pfnOnEnd = pfnOnEnd; }  // line 198
private:
    BOOL Advance(float flDeltaTime);
    void sLoadAnimations();
    SNO m_snoPlaybackSound;  // 0x60
    float m_flElapsedTime;  // 0x64
    float m_flTotalDurationTime;  // 0x68
    BOOL m_bLooping;  // 0x6c
    BOOL m_bPlaying;  // 0x70
    struct AnimationBinding {  // size 0x20
        uint64 nAnimationId;  // 0x00
        UIAnimationProperty eAnimatedProperty;  // 0x08
        UIID idAnimatedControl;  // 0x10
        float flScaleFactor;  // 0x18
    };
    XGrowableList<UIStoryboard::AnimationBinding *, UIStoryboard::AnimationBinding *> m_animationBindings;  // 0x78
    UIFunctionPtr m_pfnOnEnd;  // 0xd0
};
static_assert(sizeof(UIStoryboard) == 0xd8, "UIStoryboard");
