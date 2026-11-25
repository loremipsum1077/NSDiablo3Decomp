// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIItemButtonControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UIButtonControl.h"
#include "../../../Common/GameBalance.h"

struct UIControlDataExt;
struct Rect2D;
class ActorCommonData;
namespace D3 { namespace Items { class Generator; } }

typedef int32 ItemQualityLevel;

typedef int32 UIItemOverlay;

class ItemButtonCreationParams : public ButtonCreationParams {  // size 0x8a8
public:
    ItemButtonCreationParams(const UIType, const UIID &, const UIID &);
    ItemButtonCreationParams(const UIType, LPCSTR, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    BOOL fDrawBackground;  // 0x890
    BOOL fDrawSockets;  // 0x894
    BOOL fForceLargeBackground;  // 0x898
    GBID gbidItem;  // 0x89c
    BOOL fDrawStackCount;  // 0x8a0
    int32 nStackCount;  // 0x8a4
private:
    void sInit();
};
static_assert(sizeof(ItemButtonCreationParams) == 0x8a8, "ItemButtonCreationParams");

class UIItemButton : public UIButton {  // size 0x4f0
public:
    UIItemButton(const ItemButtonCreationParams &, const Vector2D &, const Vector2D &);
    UIItemButton(const ItemButtonCreationParams &, const Rect2D *);
    virtual ~UIItemButton() {}  // vtable slot 0; line 67
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17; line 69
        if (eType == UIT_ITEMBUTTON)
            return TRUE;
        return UIButton::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    void SetGBID(const GBID gbid);
    GBID GetGBID() const;
    void SetACDID(const ACDID idACD);
    ACDID GetACDID() const;
    void SetQualityLevel(const ItemQualityLevel eIQL);
    void SetOverlay(const UIItemOverlay eOverlay) { m_eOverlay = eOverlay; }  // line 113
    void SetStackCount(uint64 nStackCount) { m_nStackCount = nStackCount; }  // line 119
    uint64 GetStackCount() { return m_nStackCount; }  // line 125
    void SetFromRecipe(const SNO snoRecipe);
    void SetFromItemGenerator(const D3::Items::Generator & tItem);
    void SetDrawStackCount(BOOL fDrawStackCount);
    void DrawItemSockets(const ActorCommonData * ptACD, const Rect2D & rectItemImage, const BOOL fEquipped, float & flZ);
    static void GetItemRect(const ImageTextureFrame & tImageTexFrame, const Rect2D & rectSlot, Rect2D & rectItem);
private:
    void sInit(const ItemButtonCreationParams & tParams);
    enum {
        IBF_DRAW_HIGHLIGHT_BIT = 0,
        IBF_EQUIPPED_BIT = 1,
        IBF_NO_BACKGROUND = 2,
        IBF_NO_SOCKETS = 3,
        IBF_FORCE_LARGE_BACKGROUND = 4,
        IBF_DELAYED_TEXTURE_INIT = 5,
        IBF_DRAW_STACK_COUNT = 6,
        IBF_DRAW_LEGENDARY_GEM_RANK = 7,
    };
    ItemQualityLevel m_eIQL;  // 0x4c8
    uint32 m_dwItemButtonFlags;  // 0x4cc
    UIItemOverlay m_eOverlay;  // 0x4d0
    GBID m_gbid;  // 0x4d4
    ACDID m_idACDItem;  // 0x4d8
    uint64 m_nStackCount;  // 0x4e0
    int32 unk_4e8;  // 0x4e8 (2.7.7: ctor zeroes 0x4e8/0x4ec, SetACDID stores two call results)
    int32 unk_4ec;  // 0x4ec
};
static_assert(sizeof(UIItemButton) == 0x4f0, "UIItemButton");

inline void UIItemButton::SetDrawStackCount(BOOL fDrawStackCount) {  // line 157
    if (fDrawStackCount)
        m_dwItemButtonFlags |= (1 << IBF_DRAW_STACK_COUNT);
    else
        m_dwItemButtonFlags &= ~(1 << IBF_DRAW_STACK_COUNT);
}
