// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIRActorControl.h
// Class layouts and method lines from the DWARF. m_tVisualEquipment stays as padding: four
// files that include this header define their own RActor stand-ins, so VisualInventory.h
// (which pulls in RActors.h) cannot be included here yet.

#pragma once

#include "UIWindowControl.h"
#include "../../SubObjectGfx.h"
#include "../../../Sigma/DataStructures/XOptional.h"

typedef int32 LightID;
typedef int32 PlayerClass;

struct UIControlDataExt;
struct Rect2D;
struct PRTransform;
struct FastAttribKey;

class AsynchRActorCreator {  // size 0x8; line 19
public:
    AsynchRActorCreator() {}  // line 22
    virtual ~AsynchRActorCreator() {}  // vtable slot 0; line 23
    virtual BOOL NeedsUpdate() const = 0;  // vtable slot 2; line 25
    virtual RActorID TryToCreateRActor(RenderContext eContext) const = 0;  // vtable slot 3; line 28
};

class RActorCreationParams : public WindowCreationParams {  // size 0x758; line 32
public:
    RActorCreationParams(const UIType eType, const UIID & id, const UIID & idParent);  // line 35
    RActorCreationParams(const UIType, LPCSTR, const UIID &);  // line 40
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2; line 45
    SNO snoActor;  // 0x730
    SNO snoObserver;  // 0x734
    SNO snoLight;  // 0x738
    RenderContext eRenderContext;  // 0x73c
    BOOL fClearZBuffer;  // 0x740
    BOOL fRotationEnabled;  // 0x744
    BOOL fIsStaticRActor;  // 0x748
    XOptional<int> fFullScreenOverride;  // 0x74c
private:
    void sInit();  // line 59
};
static_assert(sizeof(RActorCreationParams) == 0x758, "RActorCreationParams");

struct VisualEquipment;
struct RActor;

class UIRActor : public UIWindow {  // size 0x370; line 69
public:
    struct RenderData {  // size 0x60; line 72
        DisplayList m_tDisplayList;  // 0x00
        DisplayList m_tDisplayListHighlight;  // 0x30
    };
    UIRActor(const RActorCreationParams &, const Vector2D &, const Vector2D &);  // line 78
    UIRActor(const RActorCreationParams & tParams, const Rect2D * pRect);  // line 82
    virtual ~UIRActor();  // vtable slot 0; line 86
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 88
        return eType == UIT_RACTOR || UIWindow::IsType(eType);
    }
    virtual BOOL UpdateCachedActiveState();  // vtable slot 11; line 101
    BOOL ClickIsEaten(const IVector2D & p) const;  // line 108
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15; line 111
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting);  // vtable slot 16; line 114
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12; line 118
    void Cleanup();  // line 123
    void SetLight(const LightID idLight) { m_idLight = idLight; }  // line 127
    BOOL IsLightValid();  // line 133
    void CreateRActor(const RenderContext eContext, const SNO snoActor);  // line 141
    RActorID GetRActor() const { return m_idRActor; }  // line 145
    void SetRActor(RActorID idRActor);  // line 150
    void DeleteRActor();  // line 152
    void SetAsynchRActorCreator(AsynchRActorCreator * ptCreator);  // line 158
    const AsynchRActorCreator * GetAsynchRActorCreator() const { return m_ptAsynchRActorCreator; }  // line 161
    RActorID CreateRActorForCurrentContext(const SNO snoActor, const float flScale, const float flLife, const PRTransform & transform, const uint32 dwRActorAddFlags);  // line 164
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19; line 171
    static void PlayAnimForEquipment(RActor * ptRActor, const VisualEquipment & tVisualEquipment, const PlayerClass ePlayerClass, const uint32 dwAnimTag, BOOL bDisableBlending);  // line 175
    void AttachParticleSystem(const SNO snoParticle, const Vector2D & vecUIC, const FastAttribKey * ptAttribKey);  // line 182
    BOOL HasAttachedParticleSystem(const FastAttribKey & tKey) const;  // line 187
    void RemoveParticleSystem(const FastAttribKey & tKey);  // line 190
    void SetIsFullScreenElement(BOOL bFullScreen);  // line 193
    PlayerClass m_ePlayerClass;  // 0x1cc
    uint32 m_dwPlayerFlags;  // 0x1d0
    uint8 _unadopted1[0x268 - 0x1d4];  // 0x1d4 VisualEquipment m_tVisualEquipment (line 203), see the note at the top
    UIRActor::RenderData tRenderData[2];  // 0x268
    void SetObserver(const SNO snoObserver) { m_snoObserver = snoObserver; }  // line 207
    SNO GetObserver() const { return m_snoObserver; }  // line 213
    RenderContext GetRenderContext();  // line 220
private:
    void sInit(const RActorCreationParams & tParams);  // line 231
    BOOL IsFullScreenElement() const {  // line 236
        return fFullScreenOverRide.Exists() ? fFullScreenOverRide.Get() : m_fIsFullscreenUIElement;
    }
    void SetRenderContext(const RenderContext eRenderContext);  // line 243
    void sRecreateActor();  // line 246
public:  // DWARF has the rest private; other files in this tree still read some of them.
    RActorID m_idRActor;  // 0x328
    LightID m_idLight;  // 0x32c
    SNO m_snoObserver;  // 0x330
    IVector2D m_vecRotationPixels;  // 0x334
    BOOL m_fIsStaticRActor;  // 0x33c
    BOOL m_fIsFullscreenUIElement;  // 0x340
    BOOL m_fClearZBuffer;  // 0x344
    BOOL m_fRotationEnabled;  // 0x348
    int32 m_eEffectBuffer;  // 0x34c; EffectBuffer (PostFX.h) in the DWARF. PostFX.h pulls in GfxVB.h, whose static
                            // RGBAColor constants would then initialize ahead of other headers' in every includer.
    SNO m_snoActor;  // 0x350
    SNO m_snoLight;  // 0x354
    RenderContext m_eRenderContext;  // 0x358
    XOptional<int> fFullScreenOverRide;  // 0x35c
    AsynchRActorCreator * m_ptAsynchRActorCreator;  // 0x368
};
static_assert(sizeof(UIRActor) == 0x370, "UIRActor");
