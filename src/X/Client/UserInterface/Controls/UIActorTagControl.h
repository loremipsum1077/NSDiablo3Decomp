// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIActorTagControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"
#include "../../../Common/CommonTypes.h"

struct UIControlDataExt;
struct Rect2D;

class UIActorTag : public UIText {  // size 0x3f8
public:
    UIActorTag(const TextCreationParams &, const Vector2D &, const Vector2D &);
    UIActorTag(const TextCreationParams &, const Rect2D *);
    virtual ~UIActorTag() {}  // original inline definition, header line 30
    virtual BOOL IsType(const UIType eType) const { return eType == UIT_ACTORTAG || UIText::IsType(eType); }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);
    uint32 GetExpirationTime() const { return m_dwExpirationTime; }  // line 46
    void SetExpirationTime(const uint32 dwTime) { m_dwExpirationTime = dwTime; }  // line 53
    void SetACD(const ACDID idACD) { m_idACD = idACD; }  // line 67
    void SetOffset(const float x, const float y) { m_offset.Set(x, y); }  // line 73
private:
    void sInit();
    ACDID m_idACD;  // 0x3e8
    uint32 m_dwExpirationTime;  // 0x3ec
    Vector2D m_offset;  // 0x3f0
};
static_assert(sizeof(UIActorTag) == 0x3f8, "UIActorTag");
