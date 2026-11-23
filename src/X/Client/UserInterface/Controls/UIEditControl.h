// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIEditControl.h
// Trimmed to what UIControl::CreateFromXMLData needs; members whose types are not
// adopted yet are kept as _unadopted padding at their DWARF offsets.

#pragma once

#include "UITextControl.h"
#include "../../AnimationCommon.h"

struct UIControlDataExt;
struct Rect2D;
struct BufferedInput;
struct IRect2D;

typedef int32 Key;

enum EditFlagBits {
    EDIT_FLAG_NUMBERS_ONLY_BIT = 0,
    _DEPRECATED_EDIT_FLAG_IME_ACTIVE_BIT = 1,
    EDIT_FLAG_IME_DISABLED = 2,
    EDIT_FLAG_NUMBER_FORMATTING_ENABLED = 3,
    EDIT_FLAG_ACCEPTS_RETURN = 4,
};

enum EditBoxStyle {
    EDITSTYLE_DEFAULT = -1,
    EDITSTYLE_NO_BACKGROUND = 0,
};

typedef BOOL (*UIEditFunction)(LPCSTR);

class EditBoxCreationParams : public TextCreationParams {  // size 0x820
public:
    EditBoxCreationParams(const UIType, const UIID &, const UIID &);
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    SNO snoCursorAnim;  // 0x7d8
    EditBoxStyle eEditBoxStyle;  // 0x7dc
    UIFunctionPtr pfnOnReturnEndEdit;  // 0x7e0
    UIFunctionPtr pfnOnChange;  // 0x7e8
    UIFunctionPtr pfnOnGainedFocus;  // 0x7f0
    UIFunctionPtr pfnOnLostFocus;  // 0x7f8
    BOOL fNumbersOnly;  // 0x800
    BOOL fAutoFormatNumbers;  // 0x804
    BOOL fIMEDisabled;  // 0x808
    int32 nMaxAllowedCharacters;  // 0x80c
    int32 nMaxAllowedBytes;  // 0x810
    int32 nNumDecimalPlaces;  // 0x814
    BOOL fAcceptsReturn;  // 0x818
};
static_assert(sizeof(EditBoxCreationParams) == 0x820, "EditBoxCreationParams");

class UIEdit : public UIText {  // size 0x468
public:
    UIEdit(const EditBoxCreationParams &, const Vector2D &, const Vector2D &);
    UIEdit(const EditBoxCreationParams &, const Rect2D *);
    virtual ~UIEdit() {}  // vtable slot 0, line 76
    virtual BOOL UpdateCachedActiveState();  // vtable slot 11
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 12
    virtual BOOL HandlesInput() { return TRUE; }  // vtable slot 13, line 173
    virtual UIID HitTest(const IVector2D & pt) {  // vtable slot 15, line 180
        if (!IsActive())
            return GetUIIDNone();

        UIID idHit = HitTestChildren(pt);
        if (idHit == GetUIIDNone())
        {
            if (IsInside(pt))
                return GetID();
            return GetUIIDNone();
        }
        return idHit;
    }
    virtual BOOL CanAcceptKeyboardFocus() const {  // vtable slot 2, line 203
        return IsActive() && IsEnabled();
    }
    virtual void LostKeyboardFocus();  // vtable slot 4
    virtual void GainedKeyboardFocus();  // vtable slot 5
    virtual BOOL IsType(const UIType eType) const {  // vtable slot 17, line 215
        return eType == UIT_EDIT || UIText::IsType(eType);
    }
    virtual void Draw(const uint8 bLayerAlpha, float & flZSlice);  // vtable slot 19
    virtual void SetText_Internal(LPCSTR, const BOOL);  // vtable slot 37
    void Clear();
    void SetTextUpcall();
    void SetMaximumAllowedCharacters(const int32 nNewMaximum, BOOL fSetCursor);
    void SetMaximumAllowedBytes(const int32 nNewMaximum);
    BOOL DeleteSelection();
    BOOL InsertAtCursor(LPCSTR lpuszNewCharacters, BOOL fPasteHyperlink);
    BOOL AcceptsReturn() const;
    void KeepCursorVisible();
protected:
    void GetCursorRectUIC(Rect2D & rectCursorUIC);
    void SetCursorPosition(const int32 nFormattedCursorPosition);
private:
    void sInit(const EditBoxCreationParams & tParams);
    void sUpdate();
    void sDraw(const uint8 bLayerAlpha, float & flZSlice);
    void sDrawBackground(const uint8, const RGBAColor & rgbaColor, float flZ);
    void sDrawCursor(const uint8 bAlphaCascaded, const Rect2D & rectClippingUIC, float flZ);
    BOOL sInput(const BufferedInput & tInput);
    void sHandleKeyBackspace(const BOOL fCtrlDown);
    void sHandleKeyDelete(const BOOL fCtrlDown);
    void sHandleKeyLeft(const BOOL fShiftDown, const BOOL fCtrlDown);
    void sHandleKeyRight(const BOOL fShiftDown, const BOOL fCtrlDown);
    BOOL sHandleKeyUp(const BOOL fShiftDown, const BOOL fCtrlDown);
    BOOL sHandleKeyDown(const BOOL fShiftDown, const BOOL fCtrlDown);
    void sHandleKeyHome(const BOOL fShiftDown, const BOOL fCtrlDown);
    void sHandleKeyEnd(const BOOL fShiftDown, const BOOL fCtrlDown);
    void sHandleCharacter(LPCSTR szUTF8);
    BOOL sHandleMouseMove(const BufferedInput & tInput);
    BOOL sHandleLeftMouseDown(const BufferedInput & tInput);
    void sPasteFromClipboard();
    void sAdjustSelection(const int32 nOldCursorPos, const int32 nNewCursorPos, const BOOL fShiftDown, const BOOL fExpandingLeft);
    int32 sValidateString(LPSTR lpuszString, int32 nAvailableSpace, int32 nRemainingCharacters);
    void sGetCursorLineInfo(int32 nCursorPosition, int32 & nLineIndex, int32 & nLineCursorOriginalIndex, FormattedText & tLineAtCursor) const;
    BOOL sCodepointIsValid(const uint32 nCodepoint) const;
    BOOL sIsUnhandledKey(const Key eKey) const;
    BOOL HasKeyboardFocus() const;
    BOOL InsertAtCursorInternal(LPCSTR lpuszNewCharacters, BOOL fShowingComposition, BOOL fPasteHyperlink);
    void UpdateIMELocaleIndicator();
    void PerformNumberFormatting(LPSTR uszBuffer, uint32 nLength, int32 & nCursorPosition);
public:
    SNO m_snoCursor;  // 0x3e4
    AnimState m_tCursorAnimState;  // 0x3e8
    uint32 m_dwFlags;  // 0x400
    int32 m_nCursorPosition;  // 0x404
    int32 m_nMaximumAllowedCharacters;  // 0x408
    int32 m_nMaximumAllowedBytes;  // 0x40c
    int32 m_nUTF8Length;  // 0x410
    int32 m_nNumDecimalDigits;  // 0x414
    XLocale m_eLocaleOverride;  // 0x418
    BOOL m_bIsIMEComposing;  // 0x41c
    UIFunctionPtr m_pfnOnReturnEndEdit;  // 0x420
    UIFunctionPtr m_pfnOnChange;  // 0x428
    UIFunctionPtr m_pfnOnGainedFocus;  // 0x430
    UIFunctionPtr m_pfnOnLostFocus;  // 0x438
    UIFunctionPtr m_pfnMaximumAllowedCharactersReached;  // 0x440
    UIEditFunction m_pfnStringWouldBeValid;  // 0x448
    UIEditFunction m_pfnInputWouldBeValid;  // 0x450
    EditBoxStyle m_eEditBoxStyle;  // 0x458
    float m_flIndentLeft;  // 0x45c
    float m_flIndentRight;  // 0x460
    BOOL m_fIgnoreNextCharMessage;  // 0x464
};
static_assert(sizeof(UIEdit) == 0x468, "UIEdit");
