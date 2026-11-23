// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Controls/UIControlControl.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIInit.h"
#include "../UIMessage.h"
#include "../UITypes.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/DataStructures/XGrowableMap.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

struct IVector2D;
struct UIControlDataExt;
struct Vector2D;

typedef XGrowableMap<unsigned long, const unsigned long &, int, int> CUIStateMap;

struct UIControlSourceKey {  // size 0x10
    UIControlSourceKey() {}
    UIControlSourceKey(const SNO snoUI, LPCSTR szSubpath) : snoUI(snoUI), nKey(UIHashIDName(szSubpath)) {}  // line 34
    static UIControlSourceKey Parse(LPCSTR szSnoAndPath, const SNO snoImpliedFile, LPCSTR * szPath, LPCSTR * szName);
    SNO snoUI;  // 0x00
    uint64 nKey;  // 0x08
};
static_assert(sizeof(UIControlSourceKey) == 0x10, "UIControlSourceKey");

class ControlCreationParams {  // size 0xa8
    // vtable pointer
public:
    ControlCreationParams(const UIType, const UIID &, const UIID &);
    ControlCreationParams(const UIType, LPCSTR, const UIID &);
    virtual ~ControlCreationParams() {}  // vtable slot 0; line 65
    virtual void Load(const SNO snoUI, const UIControlDataExt * pControlData);  // vtable slot 2
    UIType eType;  // 0x08
    UIID id;  // 0x10
    UIID idParent;  // 0x18
    BOOL fCodeConstructed;  // 0x20
    BOOL fIsInstantiatedTemplate;  // 0x24
    BOOL fOnlyAcceptPrimaryControllerInput;  // 0x28
    bool fAcceptsInitialKeyboardFocus;  // 0x2c
    UIControlSourceKey tSrcKey;  // 0x30
    BOOL fActive;  // 0x40
    UIFunctionPtr pfnOnOpen;  // 0x48
    UIFunctionPtr pfnOnClose;  // 0x50
    UIFunctionPtr pfnOnUpdate;  // 0x58
    UIFunctionPtr pfnOnDispose;  // 0x60
    UIFunctionPtr pfnOnInit;  // 0x68
    SNO snoOnOpenSound;  // 0x70
    SNO snoOnCloseSound;  // 0x74
    CRefString sGenericLink[2];  // 0x78
    static BOOL s_fDuringInitialization;
    BOOL HasAnyFunctionsPtr() const;
private:
    void sInit(const UIType _eType, const UIID & _id, const UIID & _idParent, const BOOL _fCodeConstructed);
};
static_assert(sizeof(ControlCreationParams) == 0xa8, "ControlCreationParams");

struct UIControlFunctionsPtr {  // size 0x28
    UIFunctionPtr pfnOnOpen;  // 0x00
    UIFunctionPtr pfnOnClose;  // 0x08
    UIFunctionPtr pfnOnUpdate;  // 0x10
    UIFunctionPtr pfnOnDispose;  // 0x18
    UIFunctionPtr pfnOnInit;  // 0x20
};
static_assert(sizeof(UIControlFunctionsPtr) == 0x28, "UIControlFunctionsPtr");

class UIControl {  // size 0x60
    // vtable pointer
    // UIControlGroup reads m_bCachedActiveState, m_id, m_idParent and m_ptPrevActiveSibling directly
    // (no accessor is inlined into its functions).
    friend class UIControlGroup;
    // UI.cpp:4791 reads the private m_bIsInstantiatedTemplate directly (no accessor is inlined there); the friend line is not in the DWARF.
    friend UIID UI_DeriveFullUIID(const UIID & idControl, LPCSTR szIncompletePath, BOOL bUseCaretForTemplateInstances);
    UIControl(const UIControl &);
    UIControl & operator=(const UIControl &);
public:
    UIControl(const ControlCreationParams &);
    virtual ~UIControl();  // vtable slot 0
    const UIID & GetParent() const { return m_idParent; }  // line 132
    void SwitchParent(const UIID & idParent);
    const UIControlSourceKey & GetUISrcKey() const { return m_tSrcKey; }  // line 141
    BOOL HasParent(const UIID & idParent) const;
    virtual BOOL CanAcceptKeyboardFocus() const { return FALSE; }  // vtable slot 2; line 170
    virtual const UIID & GetInitialKeyboardFocus() const {  // vtable slot 3; line 180
        if (CanAcceptKeyboardFocus() && m_bAcceptsInitialKeyboardFocus) {
            return GetID();
        }
        return GetUIIDNone();
    }
    virtual void LostKeyboardFocus() {}  // vtable slot 4; line 193
    virtual void GainedKeyboardFocus() {}  // vtable slot 5; line 201
    virtual BOOL CanAcceptControllerFocus() const { return FALSE; }  // vtable slot 6; line 207
    virtual void LostControllerFocus() {}  // vtable slot 7; line 216
    virtual void GainedControllerFocus() {}  // vtable slot 8; line 224
    virtual void Move(const Vector2D & vecDelta) {}  // vtable slot 9; line 234
    virtual void SetActive(const BOOL fActive);  // vtable slot 10
    virtual BOOL UpdateCachedActiveState();  // vtable slot 11
    BOOL IsActive() const { return m_bCachedActiveState; }  // line 257
    BOOL IsItselfActive() const { return m_bActive; }  // line 264
    BOOL IsInstatiatedTemplate() const;
    void AllocateFunctionsPtrIfNeeded() {  // line 277
        if (m_pControlFunctionsPtr == NULL) {
            m_pControlFunctionsPtr = (UIControlFunctionsPtr *)SigmaMemoryNew(sizeof(UIControlFunctionsPtr), 4, NULL, TRUE);
        }
    }
    void SetUserData(void * pUserData);
    void * GetUserData() const;
    void SaveUIState(CUIStateMap & tMap);
    void RestoreUIState(CUIStateMap & tMap);
    void SetOnOpenCallback(UIFunctionPtr);
    void SetOnCloseCallback(UIFunctionPtr);
    UIFunctionPtr GetOnCloseCallback() const;
    const UIID & GetID() const { return m_id; }  // line 323
    void SetOnUpdateCallback(UIFunctionPtr);
    BOOL CanHandleMessage(const UIMsg & msg, const uintptr dwParam) const;
    UIControl * GetNextActiveSibling() const { return m_ptNextActiveSibling; }  // line 342
    void SetNextActiveSibling(UIControl * ptNextActiveSibling) { m_ptNextActiveSibling = ptNextActiveSibling; }  // line 348
    UIControl * GetPrevActiveSibling() const { return m_ptPrevActiveSibling; }  // line 354
    void SetPrevActiveSibling(UIControl * ptPrevActiveSibling) { m_ptPrevActiveSibling = ptPrevActiveSibling; }  // line 360
    virtual BOOL HandleMessage(const UIMsg & msg, const uintptr);  // vtable slot 12
    virtual BOOL HandlesInput() { return FALSE; }  // vtable slot 13; line 377
    void SetFilterControllerIndex(int32 nControllerIndex) { m_nFilterControllerIndex = nControllerIndex; }  // line 383
    virtual void PeekChildMessage(const UIMsg & msg, const uintptr dwParam);  // vtable slot 14
    virtual UIID HitTest(const IVector2D & pt) { return GetUIIDNone(); }  // vtable slot 15; line 405
    virtual UIID HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting) { return GetUIIDNone(); }  // vtable slot 16; line 418
    virtual BOOL IsType(const UIType eType) const { return eType == UIT_CONTROL; }  // vtable slot 17
    UIType GetType() const { return m_eType; }  // line 446
    virtual void PostCreate();  // vtable slot 18
    virtual void Draw(const uint8 bAlpha, float & flZSlice) {}  // vtable slot 19; line 465
    virtual void EnteredHitRegion() {}  // vtable slot 20; line 474
    virtual void LeftHitRegion() {}  // vtable slot 21; line 478
    virtual void UpdateAbsolutePositions() {}  // vtable slot 22; line 482
    virtual BOOL ContainsUIElement(const UIID & idRecipient) const { return FALSE; }  // vtable slot 23; line 486
    static UIControl * CreateFromXMLData(const SNO snoUI, const UIControlDataExt * pControlData, const UIID & id, const UIID & idParent, const BOOL fInstantiatingTemplate);
protected:
    BOOL SetActiveInternal(const BOOL fActive) {  // line 517
        m_bActive = fActive;
        return UpdateCachedActiveState();
    }
    UIControlFunctionsPtr * m_pControlFunctionsPtr;  // 0x08
    SNO m_snoOnOpenSound;  // 0x10
    SNO m_snoOnCloseSound;  // 0x14
    bool m_bAcceptsInitialKeyboardFocus : 1;  // 0x18
private:
    void sProcessAnimationBindings();
    bool m_bActive : 1;  // 0x18
    bool m_bCachedActiveState : 1;  // 0x18
    bool m_bIsInstantiatedTemplate : 1;  // 0x18
    bool m_bChildrenPreloaded : 1;  // 0x18
    bool m_bOnlyAcceptPrimaryControllerInput : 1;  // 0x18
    int32 m_nFilterControllerIndex : 3;  // 0x18
    UIControl * m_ptNextActiveSibling;  // 0x20
    UIControl * m_ptPrevActiveSibling;  // 0x28
    UIID m_id;  // 0x30
    UIID m_idParent;  // 0x38
    UIType m_eType;  // 0x40
    void * m_pUserData;  // 0x48
    UIControlSourceKey m_tSrcKey;  // 0x50
};
static_assert(sizeof(UIControl) == 0x60, "UIControl");


