#include "UIControlGroup.h"

#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../CGame.h"

// Declarations of the functions this file calls whose headers are not adopted yet;
// the DWARF gives their signatures but not where they were declared.
// X/Client/Input/Input.h
struct BufferedInput;
BOOL InputIsKeyboard(const BufferedInput & tInput);
BOOL InputIsIME(const BufferedInput & tInput);
// X/Client/Sound/Sound.cpp
struct PlaySoundOptions;
typedef uint32 SoundID;
namespace SoundManager {
SoundID PlaySound(const SNO snoSound, const PlaySoundOptions & tOptions);
}  // namespace SoundManager
extern const PlaySoundOptions cgtPSODefaults;
// X/Client/UserInterface/UI.cpp
#include "../UI.h"



BOOL UI_TestID(const UIID & id);
void UI_DeleteControl(UIID & id, BOOL fImmediate);
void UI_SetKeyboardFocus(const UIID & idFocus);

UIControlGroup::UIControlGroup(const ControlCreationParams & tParams)
    : UIControl(tParams), m_ptFirstActiveChild(NULL), m_ptLastActiveChild(NULL)
{
}

UIControlGroup::~UIControlGroup()
{
    for (uint32 i = 0; i < m_listChildren.size(); ++i)
    {
        UIID idChild = m_listChildren[i].id;
        UI_DeleteControl(idChild, TRUE);
    }

    m_listChildren.clear();
}

UIID UIControlGroup::HitTestChildren(const IVector2D & pt) const
{
    for (int32 i = m_listChildren.size(); i > 0; --i)
    {
        UIControl & tControlChild = *m_listChildren[i - 1].ptControl;
        UIID idRetval = tControlChild.HitTest(pt);
        if (idRetval != GetUIIDNone())
        {
            return idRetval;
        }
    }

    return GetUIIDNone();
}

UIID UIControlGroup::HitTestChildrenForTooltips(const IVector2D & pt, BOOL & bContinueTesting) const
{
    for (int32 i = m_listChildren.size(); i > 0; --i)
    {
        UIControl & tControlChild = *m_listChildren[i - 1].ptControl;
        UIID idRetval = tControlChild.HitTestForTooltips(pt, bContinueTesting);
        if (idRetval != GetUIIDNone() || !bContinueTesting)
        {
            return idRetval;
        }
    }

    return GetUIIDNone();
}

UIID UIControlGroup::HitTest(const IVector2D & pt)
{
    if (!m_bCachedActiveState)
    {
        return GetUIIDNone();
    }

    UIID idHit = HitTestChildren(pt);
    if (idHit != GetUIIDNone())
    {
        return idHit;
    }

    return GetUIIDNone();
}

UIID UIControlGroup::HitTestForTooltips(const IVector2D & pt, BOOL & bContinueTesting)
{
    if (!m_bCachedActiveState)
    {
        return GetUIIDNone();
    }

    UIID idHit = HitTestChildrenForTooltips(pt, bContinueTesting);
    if (idHit != GetUIIDNone() || !bContinueTesting)
    {
        return idHit;
    }

    return GetUIIDNone();
}

BOOL UIControlGroup::AddChild(const UIID & idControl)
{
    if (blz::find(m_listChildren.begin(), m_listChildren.end(), idControl) != m_listChildren.end())
    {
        return FALSE;
    }

    UIControlItem item;
    item.id = idControl;
    item.ptControl = &UI_GetControlFromID(idControl);
    item.ptActiveControl = item.ptControl->IsActive() ? item.ptControl : NULL;
    m_listChildren.push_back(item);
    OnChildAdded(idControl);
    RecalculateLayout(m_listChildren.size() - 1);
    UpdateActiveLinkedList();
    return TRUE;
}

BOOL UIControlGroup::RemoveChild(const UIID & idControl)
{
    int32 i = blzext::vector_find(m_listChildren, UIControlItem(idControl));
    if (i >= 0)
    {
        blzext::vector_erase_at(m_listChildren, i);
        RecalculateLayout(i);
        UpdateActiveLinkedList();
        return TRUE;
    }

    return FALSE;
}

BOOL UIControlGroup::AddActiveChild(UIControl * ptChild)
{
    int32 i = blzext::vector_find(m_listChildren, UIControlItem(ptChild->m_id));
    if (i >= 0)
    {
        m_listChildren[i].ptActiveControl = ptChild;
        UpdateActiveLinkedList();
        return TRUE;
    }

    return FALSE;
}

BOOL UIControlGroup::RemoveActiveChild(UIControl * ptChild)
{
    int32 i = blzext::vector_find(m_listChildren, UIControlItem(ptChild->m_id));
    if (i >= 0)
    {
        m_listChildren[i].ptActiveControl = NULL;
        UpdateActiveLinkedList();
    }

    return FALSE;
}

void UIControlGroup::RemoveAllChildren()
{
    m_listChildren.clear();
    RecalculateLayout(-1);
    UpdateActiveLinkedList();
}

BOOL UIControlGroup::InsertChild(const UIID & idControl, const int32 nIndex)
{
    if (blz::find(m_listChildren.begin(), m_listChildren.end(), idControl) != m_listChildren.end())
    {
        return FALSE;
    }

    UIControlItem item;
    item.id = idControl;
    item.ptControl = &UI_GetControlFromID(idControl);
    item.ptActiveControl = item.ptControl->IsActive() ? item.ptControl : NULL;
    m_listChildren.insert(m_listChildren.begin() + nIndex, item);
    OnChildAdded(idControl);
    RecalculateLayout(m_listChildren.size() - 1);
    UpdateActiveLinkedList();
    return TRUE;
}

BOOL UIControlGroup::UpdateCachedActiveState()
{
    const UIID & idThis = m_id;
    UIGlobals * ptUIGlobals = CGameGet().ptUIGlobals;
    BOOL fIsBaseLayer = idThis == cgUIIDRoot || idThis == cgUIIDNormalLayer || idThis == cgUIIDWindowLayer || idThis == cgUIIDTopLayer;

    if (fIsBaseLayer)
    {
        ptUIGlobals->bSkipPreloadChange = TRUE;
    }

    BOOL bWasActive = UIControl::UpdateCachedActiveState();

    for (uint32 i = 0; i < m_listChildren.size(); ++i)
    {
        m_listChildren[i].ptControl->UpdateCachedActiveState();
    }

    if (IsActive() && !bWasActive)
    {
        RecalculateLayout(-1);

        if (ControlCreationParams::s_fDuringInitialization == FALSE)
        {
            SoundManager::PlaySound(m_snoOnOpenSound, cgtPSODefaults);
            if (m_pControlFunctionsPtr != NULL && m_pControlFunctionsPtr->pfnOnOpen != NULL)
            {
                m_pControlFunctionsPtr->pfnOnOpen(idThis);
            }
        }
    }

    if (fIsBaseLayer)
    {
        ptUIGlobals->bSkipPreloadChange = FALSE;
    }

    return bWasActive;
}

// NON_MATCHING: the original tests `msg <= 2` unsigned (`cmp #2; b.ls`) and lays the child walk (line 359) out
// first though the active walk (342) is earlier in source. UIMsg has no fixed underlying type in the DWARF (other
// enums carry DW_AT_type), so msg promotes to int: the unsigned test comes from the expression. Range forms
// (`msg >= UI_UPDATE && msg < UI_RESET`, `==` chains, switch) give cmp #2 but A-first layout and put the branch on
// the &&; the original has branch 338:9, msg load 338:37 (28 chars into the condition), not yet explained.
BOOL UIControlGroup::ForwardMessage(const UIMsg & msg, const uintptr dwParam)
{
    if (msg >= UI_RESET)
    {
        for (int32 i = m_listChildren.size(); i > 0; --i)
        {
            UIControl & tControl = *m_listChildren[i - 1].ptControl;
            if (tControl.CanHandleMessage(msg, dwParam))
            {
                BOOL bRetval = tControl.HandleMessage(msg, dwParam);
                if (bRetval)
                {
                    return TRUE;
                }
            }
        }
    }
    else
    {
        for (UIControl * ptControl = m_ptLastActiveChild; ptControl != NULL; ptControl = ptControl->m_ptPrevActiveSibling)
        {
            if (ptControl->CanHandleMessage(msg, dwParam))
            {
                BOOL bRetval = ptControl->HandleMessage(msg, dwParam);
                if (bRetval)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

const UIID & UIControlGroup::GetInitialKeyboardFocus() const
{
    for (int32 i = m_listChildren.size(); i > 0; --i)
    {
        UIControl & tControl = *m_listChildren[i - 1].ptControl;
        const UIID & idNewFocus = tControl.GetInitialKeyboardFocus();
        if (idNewFocus != GetUIIDNone())
        {
            return idNewFocus;
        }
    }

    if (CanAcceptKeyboardFocus() && m_bAcceptsInitialKeyboardFocus)
    {
        return GetID();
    }

    return GetUIIDNone();
}

void UIControlGroup::SetActive(const BOOL fActive)
{
    BOOL fWasActive = IsActive();

    UIControl::SetActive(fActive);

    if (fActive && !fWasActive)
    {
        const UIID & idKeyboardFocus = GetInitialKeyboardFocus();
        if (idKeyboardFocus != GetUIIDNone())
        {
            UI_SetKeyboardFocus(idKeyboardFocus);
        }
    }
}

void UIControlGroup::PostCreate()
{
    for (uint32 i = 0; i < m_listChildren.size(); ++i)
    {
        UIControl & tControl = *m_listChildren[i].ptControl;
        tControl.PostCreate();
    }

    UIControl::PostCreate();
}

BOOL UIControlGroup::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{
    switch (msg)
    {
    case UI_INPUT:
    {
        const BufferedInput & tInput = *(const BufferedInput *)dwParam;
        if (InputIsKeyboard(tInput))
        {
            return ForwardMessage(msg, dwParam);
        }

        if (InputIsIME(tInput))
        {
            return ForwardMessage(msg, dwParam);
        }
        break;
    }
    case UI_UPDATE:
    case UI_RESET:
    case UI_GFX_INIT:
    case UI_GFX_DISPOSE:
    case UI_INIT_FOR_WORLD:
    case UI_DISPOSE_FOR_WORLD:
    case UI_INIT_FOR_GAME:
    case UI_DISPOSE_FOR_GAME:
    case UI_AXE_DUMP:
    {
        BOOL bRetval = ForwardMessage(msg, dwParam);
        if (bRetval)
        {
            return TRUE;
        }
        break;
    }
    default:
        break;
    }

    return UIControl::HandleMessage(msg, dwParam);
}

BOOL UIControlGroup::ContainsUIElement(const UIID & idRecipient) const
{
    if (!UI_TestID(idRecipient))
    {
        return FALSE;
    }

    UIControl & tControl = UI_GetControlFromID(idRecipient);
    if (tControl.HasParent(GetID()))
    {
        return TRUE;
    }

    for (uint32 i = 0; i < m_listChildren.size(); ++i)
    {
        if (m_listChildren[i].ptControl->ContainsUIElement(idRecipient))
        {
            return TRUE;
        }
    }

    return FALSE;
}

inline void UIControlGroup::UpdateActiveLinkedList()
{
    m_ptFirstActiveChild = NULL;
    UIControl * ptCurrentActiveChild = NULL;

    for (uint32 i = 0; i < m_listChildren.size(); ++i)
    {
        UIControl * ptControl = m_listChildren[i].ptActiveControl;
        if (ptControl == NULL)
        {
            continue;
        }

        if (ptControl->m_idParent != m_id)
        {
            continue;
        }

        if (m_ptFirstActiveChild == NULL)
        {
            m_ptFirstActiveChild = ptControl;
            ptControl->SetPrevActiveSibling(NULL);
        }
        else
        {
            ptCurrentActiveChild->SetNextActiveSibling(ptControl);
            ptControl->SetPrevActiveSibling(ptCurrentActiveChild);
        }

        ptCurrentActiveChild = ptControl;
    }

    if (ptCurrentActiveChild != NULL)
    {
        ptCurrentActiveChild->SetNextActiveSibling(NULL);
    }

    m_ptLastActiveChild = ptCurrentActiveChild;
}
