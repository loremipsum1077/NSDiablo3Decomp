// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UICutsceneControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "../../Cutscene.h"
#include "UIWindowControl.h"

struct UIControlDataExt;
struct Rect2D;

class UICutscene : public UIWindow {  // size 0x258
public:
    UICutscene(const WindowCreationParams &, const Vector2D &, const Vector2D &);
    UICutscene(const WindowCreationParams &, const Rect2D *);
    virtual ~UICutscene();  // vtable slot 0
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 33
        return eType == UIT_CUTSCENE || UIWindow::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    void Play(LPCSTR szPath, BOOL bAudioMuted);
    BOOL IsPlaying() const;
    void Stop();
    BOOL IsLooping() const;
    void SetLooping(BOOL fLoop) { m_fLoop = fLoop; }  // line 75
    CHAR m_szCutscenePath[128];  // 0x1cc
    CutsceneID m_idCutscene;  // 0x24c
    BOOL m_fLoop;  // 0x250
    BOOL m_fCutsceneAudioMuted;  // 0x254
};
static_assert(sizeof(UICutscene) == 0x258, "UICutscene");
