// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIItemTagControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "UIButtonControl.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../../../Common/CommonTypes.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/SigmaTypes.h"

struct BufferedInput;
struct IVector2D;
struct Rect2D;
struct UIID;
struct Vector2D;

class UIItemTag : public UIButton {  // size 0x4d8
public:
    UIItemTag(const ButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIItemTag(const ButtonCreationParams &, const Rect2D *);
    virtual ~UIItemTag() {}  // vtable slot 0; line 33
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual UIID HitTest(const IVector2D & pt);  // vtable slot 15
    virtual UIID HitTestForTooltips(const IVector2D &, BOOL &) {  // vtable slot 16; line 42
        return GetUIIDNone();
    }
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 49
        return eType == UIT_ITEMTAG || UIButton::IsType(eType);
    }
    RActorID GetRActor() const { return m_idRActor; }  // line 63
    void SetRActor(const RActorID idRActor) { m_idRActor = idRActor; }  // line 70
    void SetItemQualityColor(const RGBAColor & rgbaQuality) { m_rgbaQuality = rgbaQuality; }  // line 76
    void SetItemOwnerIndex(const int32 nPlayerOwnerIndex) { m_nPlayerOwnerIndex = nPlayerOwnerIndex; }  // line 82
    void SetDrawIcon(const BOOL);
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
private:
    void sInit();
    BOOL sInput(const BufferedInput & tInput);
    RActorID m_idRActor;  // 0x4c8
    void DrawItemRarity(const uint8, float & flZSlice);
    RGBAColor m_rgbaQuality;  // 0x4cc
    BOOL m_fDrawIcon;  // 0x4d0
    int32 m_nPlayerOwnerIndex;  // 0x4d4
};
static_assert(sizeof(UIItemTag) == 0x4d8, "UIItemTag");
