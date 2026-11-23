// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/RWindow.h
// Subset: RWindow, MainRWindow and RWindowMgr with the inline accessors X/Client/CGame.cpp uses.
// The accessor bodies are not in the DWARF; they are written from the copies inlined there.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "Gfx/GfxCommon.h"
#include "SubObjectGfx.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/SigmaTypes.h"

struct ShakeRWindowData;
struct VariableResRWindowData;
struct WeatherRWindowData;

enum RWindowID {
    RWindowInvalid = -1,
    RWindowMainWindow = 0,
};

class AsyncRWindowData {  // size 0xd8
public:
    RenderContext GetRenderContext() const;
    Vector3D wpLocalPlayerPosition;  // 0x00
    BOOL fLocalPlayerValid;  // 0x0c
    DisplayList dispList;  // 0x10
    DisplayList dispListReflections[2];  // 0x40
    DisplayList dispListProjected;  // 0xa0
    float flClientTime;  // 0xd0
};
static_assert(sizeof(AsyncRWindowData) == 0xd8, "AsyncRWindowData");

inline RenderContext AsyncRWindowData::GetRenderContext() const {  // line 47
    return dispList.GetRenderContext();
}

class RWindow {  // size 0x2c8
public:
    RWindow();
    virtual ~RWindow();  // vtable slot 0
    void Initialize(const RWindowID id, const RenderContext eContext, const int32 nMainCameraIndex, const int32 nPMACameraIndex);
    void Dispose();
    BOOL PostFXGfxInit();
    void PostFXGfxDispose();
    void DisplayListsCompact();
    void DisplayListsBegin();
    void DisplayListsSort();
    void DisplayListsEnd();
    virtual void Present() const = 0;  // vtable slot 2
    virtual BOOL ShouldRender() const = 0;  // vtable slot 3
    SNO GetDefaultColorTarget() const;
    SNO GetDefaultDepthTarget() const;
    SNO GetPMATarget() const;
    void SetRenderAndDepthTargetToDefaults();
    void CopyBackBuffer(const SNO);
    virtual IVector2D GetPixelDimensions() const = 0;  // vtable slot 4
    virtual Vector2D GetPixelDimensionsFloat() const = 0;  // vtable slot 5
    virtual void BuildCameras() = 0;  // vtable slot 6
    virtual SNO GetCurrentWorld() const = 0;  // vtable slot 7
    int32 GetCameraIndex() const;
    int32 GetPMACameraIndex() const;
    RenderContext GetRenderContext() const;
    RWindowID GetRWindowID() const;
    void SetLocalPlayerWorldPosition(const Vector3D & wpPosition, const BOOL fValid);
    BOOL LocalPlayerWorldPositionIsValid() const;
    const Vector3D & GetLocalPlayerWorldPosition() const;
    WeatherRWindowData & GetWeatherRWindowData() const;
    ShakeRWindowData & GetShakeRWindowData() const;
    VariableResRWindowData & GetVariableResRWindowData() const;
    AsyncRWindowData tAsyncData[2];  // 0x08
    SNO snoTexDefaultColorTarget;  // 0x1b8
    SNO snoTexDefaultDepthTarget;  // 0x1bc
    SNO snoEffectBuffer[28];  // 0x1c0
    SNO snoEffectDepthBuffer[28];  // 0x230
protected:
    SNO m_snoTexPMA;  // 0x2a0
    RWindowID m_id;  // 0x2a4
    int32 m_nCameraIndex;  // 0x2a8
    int32 m_nPMACameraIndex;  // 0x2ac
    WeatherRWindowData * m_ptWeatherRWindowData;  // 0x2b0
    ShakeRWindowData * m_ptShakeRWindowData;  // 0x2b8
    VariableResRWindowData * m_ptVariableResRWindowData;  // 0x2c0
};
static_assert(sizeof(RWindow) == 0x2c8, "RWindow");

inline SNO RWindow::GetDefaultColorTarget() const {  // line 120
    return snoTexDefaultColorTarget;
}

inline SNO RWindow::GetDefaultDepthTarget() const {  // line 126
    return snoTexDefaultDepthTarget;
}

inline SNO RWindow::GetPMATarget() const {  // line 132
    return m_snoTexPMA;
}

inline int32 RWindow::GetCameraIndex() const {  // line 156
    return m_nCameraIndex;
}

inline int32 RWindow::GetPMACameraIndex() const {  // line 162
    return m_nPMACameraIndex;
}

inline RWindowID RWindow::GetRWindowID() const {  // line 171
    return m_id;
}

inline WeatherRWindowData & RWindow::GetWeatherRWindowData() const {  // line 190
    return *m_ptWeatherRWindowData;
}

inline ShakeRWindowData & RWindow::GetShakeRWindowData() const {  // line 197
    return *m_ptShakeRWindowData;
}

inline VariableResRWindowData & RWindow::GetVariableResRWindowData() const {  // line 204
    return *m_ptVariableResRWindowData;
}

class MainRWindow : public RWindow {  // size 0x2c8
public:
    virtual BOOL ShouldRender() const;  // vtable slot 3
    virtual void Present() const;  // vtable slot 2
    virtual IVector2D GetPixelDimensions() const;  // vtable slot 4
    virtual Vector2D GetPixelDimensionsFloat() const;  // vtable slot 5
    virtual void BuildCameras();  // vtable slot 6
    virtual SNO GetCurrentWorld() const;  // vtable slot 7
};
static_assert(sizeof(MainRWindow) == 0x2c8, "MainRWindow");

class RWindowMgr {  // size 0x28
public:
    RWindowMgr();
    virtual ~RWindowMgr();  // vtable slot 0
    void AddRWindow(RWindow & tRWindow);
    void DisplayListsBegin();
    void DisplayListsSort();
    void DisplayListsEnd();
    void DisplayListsCompact();
    void RenderPresentAll();
    void SetCurrentRWindow(RWindow & tRWindow);
    void ResetCurrentRWindowToDefault();
    RWindow & GetCurrentRWindow();
    RenderContext GetCurrentRenderContext();
    RWindowID GetCurrentRWindowID();
    int32 GetRWindowCount();
    RWindow & GetRWindow(int32 nIndex);
    RWindow & GetMainRWindow();
    RWindow * GetActiveRWindowForAudio();
    RWindow * GetActiveRWindow();
    static void InitializeForGame();
    static void DisposeForGame();
private:
    blz::vector<RWindow *, blz::allocator<RWindow *> > m_tRWindowArray;  // 0x08
    RWindow * m_ptCurrentRWindow;  // 0x20
    static MainRWindow m_tMainRWindow;
};
static_assert(sizeof(RWindowMgr) == 0x28, "RWindowMgr");

inline void RWindowMgr::SetCurrentRWindow(RWindow & tRWindow) {  // line 294
    m_ptCurrentRWindow = &tRWindow;
}

inline void RWindowMgr::ResetCurrentRWindowToDefault() {  // line 301
    m_ptCurrentRWindow = &m_tMainRWindow;
}

inline RWindow & RWindowMgr::GetCurrentRWindow() {  // line 308
    return *m_ptCurrentRWindow;
}

inline RWindowID RWindowMgr::GetCurrentRWindowID() {  // line 321
    return GetCurrentRWindow().GetRWindowID();
}

inline int32 RWindowMgr::GetRWindowCount() {  // line 327
    return m_tRWindowArray.size();
}

inline RWindow & RWindowMgr::GetRWindow(int32 nIndex) {  // line 333
    return *m_tRWindowArray[nIndex];
}

// Not marked inline in the DWARF function list, but X/Client/CGame.cpp (sRenderDraw) reads
// m_tMainRWindow's address directly, so the body is written here.
inline RWindow & RWindowMgr::GetMainRWindow() {
    return m_tMainRWindow;
}

// Defined in X/Client/RWindow.cpp.
RWindowMgr * GetRWindowMgr();
