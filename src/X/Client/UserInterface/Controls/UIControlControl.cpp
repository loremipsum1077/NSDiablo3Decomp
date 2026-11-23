#include "UIControlControl.h"

#include <stdlib.h>
#include <string.h>

#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../Interface/UIMinimap.h"
#include "../UIAttributes.h"
#include "../UIStoryboard.h"
#include "UIActorTagControl.h"
#include "UIAutoScrollingTextControl.h"
#include "UIBlinkerControl.h"
#include "UIButtonControl.h"
#include "UICheckBoxControl.h"
#include "UIColorPickerControl.h"
#include "UIConsoleBuffIndicatorControl.h"
#include "UIConsoleEditControl.h"
#include "UIConsoleListboxControl.h"
#include "UIConsoleScrollBoxControl.h"
#include "UIConsoleSelectorControl.h"
#include "UIControlGroup.h"
#include "UICutsceneControl.h"
#include "UIDPadHandlerControl.h"
#include "UIDrawHookControl.h"
#include "UIEditControl.h"
#include "UIFloatBubbleControl.h"
#include "UIHotKeyControl.h"
#include "UIHotbarButtonControl.h"
#include "UIItemButtonControl.h"
#include "UIItemTagControl.h"
#include "UIKeyMappingButtonControl.h"
#include "UIListItemControl.h"
#include "UIPieChartControl.h"
#include "UIProgressBarControl.h"
#include "UIRActorControl.h"
#include "UIRadioButtonControl.h"
#include "UIRadioButtonGroupControl.h"
#include "UIScrollableTextControl.h"
#include "UIScrollbarControl.h"
#include "UIStackPanelControl.h"
#include "UITextControl.h"
#include "UIThumbButtonControl.h"
#include "UITimerControl.h"
#include "UIToolTipControl.h"
#include "UITreeControl.h"
#include "UITutorialControl.h"
#include "UIWindowControl.h"
#include "UIWrapPanelControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
#include "../../Input/Input.h"







struct PlaySoundOptions;

// X/Client/Input/Input.cpp
BOOL InputIsController(const BufferedInput & tInput);
// X/Client/SNOPreload.cpp
namespace SNOPreload {
BOOL AddClientUIChildren(SNO snoUI);
BOOL AreAllClientUIChildrenLoaded(SNO snoUI);
void WaitForClientUIChildren(SNO snoUI);
void RemoveClientUIChildren(SNO snoUI);
}  // namespace SNOPreload
// X/Client/Sound/Sound.cpp
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
#include "../UI.h"



void UI_AddControl(const UIID & id, UIControl * pControl);
void UI_FlagInputListenerArrayDirty();
BOOL UI_TestID(const UIID & id);
const UIID & UI_GetCapture();
void UI_SetCapture(const UIID & idCapture);
int32 UI_GetPrimaryUIController();
void UI_SetKeyboardFocus(const UIID & idFocus);
UIID UI_DeriveFullUIID(const UIID & idControl, LPCSTR szIncompletePath, BOOL bUseCaretForTemplateInstances);
UIStoryboard & UI_GetStoryboardFromID(const UIID & id);
// X/Client/UserInterface/UIID.cpp
extern const UIID cgUIIDMainScreen;
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIFunctionPtr UIFunction_Lookup(LPCSTR szFunction);

BOOL ControlCreationParams::s_fDuringInitialization;

UIControlSourceKey UIControlSourceKey::Parse(LPCSTR szSnoAndPath, const SNO snoImpliedFile, LPCSTR * szPath, LPCSTR * szName)
{
    UIControlSourceKey key;

    key.snoUI = snoImpliedFile;

    LPCSTR pszColon = strchr(szSnoAndPath, ':');

    if (pszColon != NULL)
    {
        LPCSTR pszPath = pszColon + 1;

        key.nKey = UIHashIDName(pszPath);

        CHAR szSnoName[512];
        Blizzard::String::Copy(szSnoName, szSnoAndPath, pszColon - szSnoAndPath + 1);
        key.snoUI = SNOFileLookupGUID(UI_SNO, szSnoName);

        if (szPath != NULL)
        {
            *szPath = pszPath;
        }
        if (szName != NULL)
        {
            LPCSTR posDot = strrchr(pszPath, '.');
            *szName = (posDot != NULL) ? posDot + 1 : pszPath;
        }
    }
    else
    {
        key.nKey = UIHashIDName(szSnoAndPath);

        if (szPath != NULL)
        {
            *szPath = szSnoAndPath;
        }
        if (szName != NULL)
        {
            LPCSTR posDot = strrchr(szSnoAndPath, '.');
            *szName = (posDot != NULL) ? posDot + 1 : szSnoAndPath;
        }
    }

    return key;
}

inline void ControlCreationParams::sInit(const UIType _eType, const UIID & _id, const UIID & _idParent, const BOOL _fCodeConstructed)
{
    eType = _eType;
    id = _id;
    idParent = _idParent;
    fCodeConstructed = _fCodeConstructed;
    fIsInstantiatedTemplate = FALSE;

    fOnlyAcceptPrimaryControllerInput = FALSE;

    fAcceptsInitialKeyboardFocus = TRUE;

    tSrcKey.snoUI = -1;
    tSrcKey.nKey = (uint64)-1;

    fActive = TRUE;

    pfnOnOpen = pfnOnClose = pfnOnUpdate = pfnOnDispose = pfnOnInit = NULL;
    snoOnOpenSound = snoOnCloseSound = -1;
}

ControlCreationParams::ControlCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
{
    sInit(eType, id, idParent, FALSE);
}

ControlCreationParams::ControlCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)
{
    UIID _id;
    _id.SetFromParent(idParent, szName);

    sInit(eType, _id, idParent, TRUE);
}

void ControlCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{
    if (snoUI == -1)
    {
        return;
    }

    tSrcKey.snoUI = snoUI;
    tSrcKey.nKey = pControlData->nKey;

    fActive = pControlData->GetBoolValue(ATT_ACTIVE);
    fIsInstantiatedTemplate = pControlData->GetTemplate() != NULL;

    fOnlyAcceptPrimaryControllerInput = pControlData->GetBoolValue(ATT_PRIMARY_CONTROLLER_ONLY);

    fAcceptsInitialKeyboardFocus = pControlData->GetBoolValue(ATT_ACCEPT_INITIAL_FOCUS);

    pfnOnOpen = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONOPEN));
    pfnOnClose = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONCLOSE));
    pfnOnUpdate = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONUPDATE));
    pfnOnDispose = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONDISPOSE));
    pfnOnInit = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONINIT));

    snoOnOpenSound = SNOFileLookupGUID(SOUND_SNO, pControlData->GetAttributeValue(ATT_ONOPEN_SOUND));
    snoOnCloseSound = SNOFileLookupGUID(SOUND_SNO, pControlData->GetAttributeValue(ATT_ONCLOSE_SOUND));

    sGenericLink[0] = pControlData->GetAttributeValue(ATT_ID_GENERIC_01);
    sGenericLink[1] = pControlData->GetAttributeValue(ATT_ID_GENERIC_02);
}

inline BOOL ControlCreationParams::HasAnyFunctionsPtr() const
{
    return pfnOnOpen != NULL || pfnOnClose != NULL || pfnOnUpdate != NULL || pfnOnDispose != NULL || pfnOnInit != NULL;
}

UIControl::UIControl(const ControlCreationParams & tParams)
{
    m_eType = tParams.eType;

    m_id = tParams.id;

    m_idParent = tParams.idParent;
    m_tSrcKey = tParams.tSrcKey;
    m_bIsInstantiatedTemplate = (tParams.fIsInstantiatedTemplate == TRUE);

    m_bOnlyAcceptPrimaryControllerInput = (tParams.fOnlyAcceptPrimaryControllerInput == TRUE);
    m_nFilterControllerIndex = -1;

    m_bAcceptsInitialKeyboardFocus = tParams.fAcceptsInitialKeyboardFocus;

    m_pUserData = NULL;

    UI_AddControl(m_id, this);

    m_pControlFunctionsPtr = NULL;

    if (tParams.HasAnyFunctionsPtr())
    {
        AllocateFunctionsPtrIfNeeded();

        m_pControlFunctionsPtr->pfnOnOpen = tParams.pfnOnOpen;
        m_pControlFunctionsPtr->pfnOnClose = tParams.pfnOnClose;
        m_pControlFunctionsPtr->pfnOnUpdate = tParams.pfnOnUpdate;
        m_pControlFunctionsPtr->pfnOnDispose = tParams.pfnOnDispose;
        m_pControlFunctionsPtr->pfnOnInit = tParams.pfnOnInit;
    }

    m_snoOnOpenSound = tParams.snoOnOpenSound;
    m_snoOnCloseSound = tParams.snoOnCloseSound;

    m_bChildrenPreloaded = FALSE;

    m_ptNextActiveSibling = m_ptPrevActiveSibling = NULL;
    m_bCachedActiveState = m_bActive = FALSE;
    SetActiveInternal(tParams.fActive == TRUE);

    if (tParams.fCodeConstructed)
    {
        if (m_idParent != GetUIIDNone())
        {
            UI_GetControlGroupFromID(m_idParent).AddChild(m_id);
        }

        if (tParams.fActive)
        {
            if (CanAcceptKeyboardFocus())
            {
                UI_SetKeyboardFocus(m_id);
            }
        }
    }
}

UIControl::~UIControl()
{
    UI_FlagInputListenerArrayDirty();

    if (m_tSrcKey.snoUI != -1)
    {
        if (m_bChildrenPreloaded)
        {
            SNOPreload::RemoveClientUIChildren(m_tSrcKey.snoUI);
        }
    }

    if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnDispose != NULL)
    {
        m_pControlFunctionsPtr->pfnOnDispose(GetID());
    }

    SigmaMemoryFree(m_pControlFunctionsPtr, NULL);
}

void UIControl::SwitchParent(const UIID & idParent)
{
    if (idParent == m_idParent)
    {
        return;
    }

    if (UI_TestID(m_idParent))
    {
        UI_GetControlGroupFromID(m_idParent).RemoveChild(GetID());
    }

    m_idParent = idParent;
    UI_GetControlGroupFromID(idParent).AddChild(GetID());
    UpdateCachedActiveState();
}

void UIControl::sProcessAnimationBindings()
{
    if (m_tSrcKey.snoUI == -1)
    {
        return;
    }

    ScopedUIDialogDefinitionLockR pdefUI(m_tSrcKey.snoUI);

    UIControlData * pControlData;
    BOOL fSuccess = UI_GetControlData(m_tSrcKey, pdefUI, pControlData);
    if (!fSuccess)
    {
        return;
    }

    UIAnimationElements & tAnimElements = pControlData->tAnimationElements;
    const UIID & id = GetID();

    for (uint32 i = 0; i < tAnimElements.dwNumAnimationBindings; ++i)
    {
        UIControlAnimationBinding & tBinding = tAnimElements.ptAnimationBindings[i];

        const char * pAnimName = strrchr(tBinding.szAnimationName, '.');
        if (pAnimName == NULL)
        {
            continue;
        }

        char szStoryboardName[512];
        Blizzard::String::Copy(szStoryboardName, tBinding.szAnimationName, pAnimName - tBinding.szAnimationName + 1);

        UIID idStoryboard = UI_DeriveFullUIID(id, szStoryboardName, TRUE);
        if (!UI_TestID(idStoryboard))
        {
            idStoryboard = UI_DeriveFullUIID(id, szStoryboardName, FALSE);
        }

        if (UI_TestID(idStoryboard))
        {
            UIID idAnimation = UI_DeriveFullUIID(idStoryboard, tBinding.szAnimationName, TRUE);
            UI_GetStoryboardFromID(idStoryboard).BindAnimation(id, idAnimation, tBinding.eProperty, tBinding.flScaleFactor);
        }
    }
}

void UIControl::PostCreate()
{
    sProcessAnimationBindings();

    if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnInit != NULL)
    {
        m_pControlFunctionsPtr->pfnOnInit(GetID());
    }
}

BOOL UIControl::HasParent(const UIID & idParent) const
{
    BOOL fHitParent = FALSE;

    UIID idTarget = GetID();

    while (idTarget != GetUIIDNone() && idTarget != cgUIIDMainScreen && !fHitParent)
    {
        if (idParent == idTarget)
        {
            fHitParent = TRUE;
        }
        else
        {
            idTarget = UI_GetControlFromID(idTarget).GetParent();
        }
    }

    return fHitParent;
}

void UIControl::SetActive(const BOOL fActive)
{
    if ((fActive == TRUE) != m_bActive)
    {
        if (!fActive)
        {
            if (UI_GetCapture() == GetID())
            {
                UI_SetCapture(GetUIIDNone());
            }
        }

        SetActiveInternal(fActive == TRUE);

        if (m_idParent != GetUIIDNone())
        {
            UIControlGroup & tParentControl = UI_GetControlGroupFromID(m_idParent);
            tParentControl.RecalculateLayoutForElement(GetID());
        }
    }
}

BOOL UIControl::UpdateCachedActiveState()
{
    if (HandlesInput())
    {
        UI_FlagInputListenerArrayDirty();
    }

    if (m_idParent != GetUIIDNone())
    {
        UIControlGroup & parent = UI_GetControlGroupFromID(m_idParent);

        if (m_bActive)
        {
            parent.AddActiveChild(this);
        }
        else
        {
            parent.RemoveActiveChild(this);
        }
    }

    const BOOL bWasActive = m_bCachedActiveState;

    m_bCachedActiveState = m_bActive && (m_idParent == GetUIIDNone() || UI_GetControlFromID(m_idParent).IsActive());

    if (!m_bCachedActiveState && UI_GetCapture() == GetID())
    {
        UI_SetCapture(GetUIIDNone());
    }

    if (ControlCreationParams::s_fDuringInitialization == FALSE && bWasActive != m_bCachedActiveState)
    {
        if (m_bCachedActiveState)
        {
            BOOL fIsControlGroup = IsType(UIT_CONTROLGROUP);
            if (!fIsControlGroup)
            {
                SoundManager::PlaySound(m_snoOnOpenSound, cgtPSODefaults);
                if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnOpen != NULL)
                {
                    m_pControlFunctionsPtr->pfnOnOpen(GetID());
                }
            }
        }
        else
        {
            if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnClose != NULL)
            {
                SoundManager::PlaySound(m_snoOnCloseSound, cgtPSODefaults);
                m_pControlFunctionsPtr->pfnOnClose(GetID());
            }
        }
    }

    if (m_tSrcKey.snoUI != -1)
    {
        UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;

        if (ControlCreationParams::s_fDuringInitialization == FALSE && ptUIGlobals->bSkipPreloadChange == FALSE && IsActive() != m_bChildrenPreloaded)
        {
            if (!m_bChildrenPreloaded)
            {
                if (SNOPreload::AddClientUIChildren(m_tSrcKey.snoUI) && !SNOPreload::AreAllClientUIChildrenLoaded(m_tSrcKey.snoUI))
                {
                    SNOPreload::WaitForClientUIChildren(m_tSrcKey.snoUI);
                }

                m_bChildrenPreloaded = TRUE;
            }
            else
            {
                SNOPreload::RemoveClientUIChildren(m_tSrcKey.snoUI);
                m_bChildrenPreloaded = FALSE;
            }
        }
    }

    return bWasActive;
}

void UIControl::SetUserData(void * pUserData)
{
    m_pUserData = pUserData;
}

void * UIControl::GetUserData() const
{
    return m_pUserData;
}

void UIControl::SaveUIState(CUIStateMap & tMap)
{
    tMap.SetAt_Tracked(m_id, m_bActive);
}

void UIControl::RestoreUIState(CUIStateMap & tMap)
{
    BOOL bActive;

    if (tMap.Lookup(m_id, bActive))
    {
        m_bActive = (bActive == TRUE);
    }
}

UIControl * UIControl::CreateFromXMLData(const SNO snoUI, const UIControlDataExt * pControlData, const UIID & id, const UIID & idParent, const BOOL fInstantiatingTemplate)
{
    BOOL fTemplate = atoi(pControlData->GetAttributeValue(ATT_TEMPLATE));
    if (!fInstantiatingTemplate && fTemplate)
    {
        return NULL;
    }

    UIType eType = pControlData->eControlType;
    switch (eType)
    {
        case UIT_CONTROL:
        {
            ControlCreationParams tParams(UIT_CONTROL, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIControl(tParams);
        }

        case UIT_CONTROLGROUP:
        {
            ControlCreationParams tParams(UIT_CONTROLGROUP, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIControlGroup(tParams);
        }

        case UIT_WINDOW:
        {
            WindowCreationParams tParams(UIT_WINDOW, id, idParent);
            tParams.Load(snoUI, pControlData);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            return new UIWindow(tParams, NULL);
        }

        case UIT_BUTTON:
        {
            ButtonCreationParams tParams(UIT_BUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIButton(tParams, NULL);
        }

        case UIT_TEXT:
        {
            TextCreationParams tParams(UIT_TEXT, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIText(tParams, NULL);
        }

        case UIT_EDIT:
        {
            EditBoxCreationParams tParams(UIT_EDIT, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIEdit(tParams, NULL);
        }

        case UIT_HOTKEY:
        {
            HotkeyCreationParams tParams(UIT_HOTKEY, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIHotkey(tParams);
        }

        case UIT_CONSOLEEDIT:
        {
            EditBoxCreationParams tParams(UIT_CONSOLEEDIT, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIConsoleEdit(tParams, NULL);
        }

        case UIT_LISTITEM:
        {
            ListItemCreationParams tParams(UIT_LISTITEM, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIListItem(tParams, NULL);
        }

        case UIT_CONSOLESCROLLBOX:
        {
            ConsoleScrollBoxCreationParams tParams(UIT_CONSOLESCROLLBOX, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIConsoleScrollBox(tParams, NULL);
        }

        case UIT_CONSOLELISTBOX:
        {
            ConsoleListBoxCreationParams tParams(UIT_CONSOLELISTBOX, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIConsoleListBox(tParams, NULL);
        }

        case UIT_CONSOLESELECTOR:
        {
            ConsoleSelectorCreationParams tParams(UIT_CONSOLESELECTOR, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIConsoleSelector(tParams, NULL);
        }

        case UIT_CONSOLEBUFFINDICATOR:
        {
            ConsoleBuffIndicatorCreationParams tParams(UIT_CONSOLEBUFFINDICATOR, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIConsoleBuffIndicator(tParams, NULL);
        }

        case UIT_DPADHANDLER:
        {
            WindowCreationParams tParams(UIT_DPADHANDLER, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIDPadHandlerControl(tParams, NULL);
        }

        case UIT_SCROLLABLETEXT:
        {
            ScrollableTextCreationParams tParams(UIT_SCROLLABLETEXT, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIScrollableText(tParams, NULL);
        }

        case UIT_TIMER:
        {
            TimerCreationParams tParams(UIT_TIMER, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UITimer(tParams, NULL);
        }

        case UIT_MINIMAP:
        {
            WindowCreationParams tParams(UIT_MINIMAP, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIMinimap(tParams, NULL);
        }

        case UIT_SCROLLBAR:
        {
            ScrollbarCreationParams tParams(UIT_SCROLLBAR, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIScrollbar(tParams, NULL, (ScrollbarVisualStyle)-1);
        }

        case UIT_PROGRESSBAR:
        {
            ProgressBarCreationParams tParams(UIT_PROGRESSBAR, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIProgressBar(tParams, NULL);
        }

        case UIT_TOOLTIP:
        {
            WindowCreationParams tParams(UIT_TOOLTIP, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIToolTip(tParams, NULL);
        }

        case UIT_TUTORIAL:
        {
            WindowCreationParams tParams(UIT_TUTORIAL, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UITutorial(tParams, NULL);
        }

        case UIT_DRAWHOOK:
        {
            DrawHookCreationParams tParams(UIT_DRAWHOOK, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIDrawHook(tParams);
        }

        case UIT_CHECKBOX:
        {
            CheckBoxCreationParams tParams(UIT_CHECKBOX, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UICheckBox(tParams, NULL);
        }

        case UIT_RACTOR:
        {
            RActorCreationParams tParams(UIT_RACTOR, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIRActor(tParams, NULL);
        }

        case UIT_AUTOSCROLLINGTEXT:
        {
            AutoScrollingTextCreationParams tParams(UIT_AUTOSCROLLINGTEXT, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIAutoScrollingText(tParams, NULL);
        }

        case UIT_RADIOBUTTON:
        {
            RadioButtonCreationParams tParams(UIT_RADIOBUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIRadioButton(tParams, NULL);
        }

        case UIT_RADIOBUTTONGROUP:
        {
            RadioButtonGroupCreationParams tParams(UIT_RADIOBUTTONGROUP, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIRadioButtonGroup(tParams);
        }

        case UIT_FLOATBUBBLE:
        {
            TextCreationParams tParams(UIT_FLOATBUBBLE, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIFloatBubble(tParams, NULL);
        }

        case UIT_ITEMTAG:
        {
            ButtonCreationParams tParams(UIT_ITEMTAG, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIItemTag(tParams, NULL);
        }

        case UIT_THUMBBUTTON:
        {
            ButtonCreationParams tParams(UIT_THUMBBUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIThumbButton(tParams, NULL);
        }

        case UIT_ITEMBUTTON:
        {
            ItemButtonCreationParams tParams(UIT_ITEMBUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIItemButton(tParams, NULL);
        }

        case UIT_HOTBARBUTTON:
        {
            HotbarButtonCreationParams tParams(UIT_HOTBARBUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIHotbarButton(tParams, NULL);
        }

        case UIT_BLINKER:
        {
            BlinkerCreationParams tParams(UIT_BLINKER, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIBlinker(tParams, NULL);
        }

        case UIT_CUTSCENE:
        {
            WindowCreationParams tParams(UIT_CUTSCENE, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UICutscene(tParams, NULL);
        }

        case UIT_ACTORTAG:
        {
            ButtonCreationParams tParams(UIT_ACTORTAG, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIActorTag(tParams, NULL);
        }

        case UIT_KEYMAPPINGBUTTON:
        {
            KeyMappingButtonCreationParams tParams(UIT_KEYMAPPINGBUTTON, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIKeyMappingButton(tParams, NULL);
        }

        case UIT_STACKPANEL:
        {
            StackPanelCreationParams tParams(UIT_STACKPANEL, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIStackPanel(tParams, NULL);
        }

        case UIT_WRAPPANEL:
        {
            WrapPanelCreationParams tParams(UIT_WRAPPANEL, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIWrapPanel(tParams, NULL);
        }

        case UIT_STORYBOARD:
        {
            StoryboardCreationParams tParams(UIT_STORYBOARD, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIStoryboard(tParams);
        }

        case UIT_TREE:
        {
            TreeCreationParams tParams(UIT_TREE, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UITree(tParams, NULL);
        }

        case UIT_TREEFOLDER:
        {
            TreeFolderCreationParams tParams(UIT_TREEFOLDER, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UITreeFolder(tParams, NULL);
        }

        case UIT_PIECHART:
        {
            WindowCreationParams tParams(UIT_PIECHART, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIPieChartControl(tParams, NULL);
        }

        case UIT_COLORPICKER:
        {
            ColorPickerCreationParams tParams(UIT_COLORPICKER, id, idParent);
            tParams.fIsInstantiatedTemplate |= fInstantiatingTemplate;
            tParams.Load(snoUI, pControlData);
            return new UIColorPicker(tParams, NULL);
        }

        default:
            return NULL;
    }
}
























































































































































































































// Shape (inference): the original keeps two IsActive() returns. The filter-matched path reuses the halfword
// loaded at 1108; the else-if exit and the default case share one reload (BranchFolder merges the tails).
BOOL UIControl::CanHandleMessage(const UIMsg & msg,
                                 const uintptr dwParam) const

{

    switch (msg)
    {
        case UI_GFX_INIT:
        case UI_GFX_DISPOSE:
        case UI_INIT_FOR_WORLD:
        case UI_DISPOSE_FOR_WORLD:
        case UI_INIT_FOR_GAME:
        case UI_DISPOSE_FOR_GAME:
            return TRUE;

        case UI_INPUT:
        {
            BufferedInput * ptInput = (BufferedInput *)dwParam;
            if (InputIsController(*ptInput)
                && !m_bOnlyAcceptPrimaryControllerInput
                && m_nFilterControllerIndex != -1)
            {
                if (m_nFilterControllerIndex != ptInput->tBufControllerData.nControllerIndex)
                {
                    return FALSE;
                }
                return IsActive();
            }





            else if (m_bOnlyAcceptPrimaryControllerInput
                && InputIsController(*ptInput)
                && UI_GetPrimaryUIController() != -1
                && ptInput->tBufControllerData.nControllerIndex != UI_GetPrimaryUIController())
            {
                return FALSE;
            }
            return IsActive();
        }

        default:
            break;
    }

    return IsActive();


}









BOOL UIControl::HandleMessage(const UIMsg & msg,
                              const uintptr)

{
    switch (msg)
    {
        case UI_UPDATE:


            if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnUpdate != NULL)
            {
                m_pControlFunctionsPtr->pfnOnUpdate(GetID());
            }
            break;

        case UI_INIT_FOR_GAME:



            if (m_bCachedActiveState)
            {
                SoundManager::PlaySound(m_snoOnOpenSound, cgtPSODefaults);
                if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnOpen != NULL)
                {
                    m_pControlFunctionsPtr->pfnOnOpen(GetID());
                }
            }
            break;

        default:
            break;
    }








    return FALSE;
}



void UIControl::PeekChildMessage(const UIMsg & msg,
                                 const uintptr dwParam)

{
    if (UI_TestID(m_idParent))
    {
        UI_GetControlFromID(m_idParent).PeekChildMessage(msg, dwParam);
    }
}
