#include "RingWrapper.h"

#include "../ConsoleModalNotificationManager.h"
#include "../../Controls/UIWindowControl.h"
#include "../../UI.h"
#include "../../UIID.h"
#include "../../../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../../../Sigma/Math/MathGeneral.h"
#include "../../../../Sigma/Math/Vector.h"

UIWindow & UI_GetWindowFromID(const UIID & id);

// X/Client/Console/ControllerUtils.cpp
namespace Console {
namespace ControllerUtils {
BOOL LStickIsInDeadZone(int32 nControllerIndex, BOOL bUnused, float flDeadZone, float flReboundDeadZone);
Vector2D GetLStickPosition(int32 nControllerIndex, BOOL bUnused);
}  // namespace ControllerUtils
// X/Client/UserInterface/Console/ConsoleScreenManager.cpp
namespace Screens {
ConsoleModalNotificationManager & GetModalNotificationManager();
}  // namespace Screens
}  // namespace Console

// X/Client/UserInterface/Console/UIConsoleStackSplit.cpp
namespace UIStackSplit {
namespace Console {
BOOL IsVisible();
}  // namespace Console
}  // namespace UIStackSplit

float s_nSlotWidth = 60.0f;
float s_nTotalWidth = 500.0f;
int32 s_nSelectedSlot;
int32 s_nIconWidth;

namespace Console {

RingWrapper::RingWrapper()
    : m_idContainer(GetUIIDNone())
    , m_idCursor(GetUIIDNone())
    , m_nSelectedIndex(-1)
    , m_bIsEnabled(TRUE)
    , m_nInputSources(1)
    , m_nTouchStartIcon(0)
{
}

RingWrapper::~RingWrapper()
{
}

void RingWrapper::WrapUsingContainerControl(const UIID & idContainer, const UIID & idCursor, Console::RingWrapper::IncludeControlPredicate fnPredicate, BOOL bForceUpdate)
{
    if (m_idContainer != idContainer || bForceUpdate) {
        m_idContainer = idContainer;
        m_idCursor = idCursor;

        Reset();

        CalculateIconRegions(fnPredicate);
    }
}

const UIID & RingWrapper::GetContainerControlID() const
{
    return m_idContainer;
}

void RingWrapper::GetTargetControlIDs(UIIDArray & aridTargets) const
{
    for (int32 i = 0; i < m_arIcons.size(); ++i) {
        aridTargets.push_back(m_arIcons[i].id);
    }
}

BOOL RingWrapper::IsInDeadZone(const int32 nControllerIndex)
{
    const float DEAD_ZONE = 0.9f;
    const float REBOUND_DEAD_ZONE = 0.9f;
    return ControllerUtils::LStickIsInDeadZone(nControllerIndex, 0, DEAD_ZONE, REBOUND_DEAD_ZONE);
}

void RingWrapper::UpdateInput(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    if (!m_bIsEnabled) {
        return;
    }

    if (Screens::GetModalNotificationManager().IsNotificationShowing() || UIStackSplit::Console::IsVisible()) {
        return;
    }

    UpdateTouchpad(nControllerIndex, fnSelectionChanged);

    if (m_nInputSources & 1) {
        UpdateAnalogStickInput(nControllerIndex, fnSelectionChanged);
    }
}

void RingWrapper::UpdateAnalogStickInput(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    static BOOL bInDeadZone = IsInDeadZone(nControllerIndex);
    const BOOL bWasInDeadZone = bInDeadZone;
    const BOOL bCurrentlyInDeadZone = IsInDeadZone(nControllerIndex);

    if (bCurrentlyInDeadZone) {
        bInDeadZone = TRUE;

        if (!bWasInDeadZone) {
            const UIID idSelected = GetSelectedControlID();
            if (idSelected != GetUIIDNone()) {
                PointCursorAtControl(idSelected);
            }
        }
    } else {
        bInDeadZone = FALSE;

        const Vector2D vecStick = ControllerUtils::GetLStickPosition(nControllerIndex, 0);
        UpdateRingSelection(vecStick, fnSelectionChanged);
    }
}

void RingWrapper::UpdateRingSelection(const Vector2D & vecUnitDirection, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    if (!m_bIsEnabled) {
        return;
    }

    const int32 nIndex = GetControlIndexAt(vecUnitDirection);
    if (nIndex == -1) {
        return;
    }

    const UIID idSelected = GetSelectedControlID();
    if (idSelected != m_arIcons[nIndex].id) {
        PointCursorAtControl(m_arIcons[nIndex].id);
        SetSelectedControl(m_arIcons[nIndex].id, fnSelectionChanged);
    }
}

const UIID & RingWrapper::GetSelectedControlID() const
{
    if (m_nSelectedIndex == -1) {
        return GetUIIDNone();
    }
    return m_arIcons[m_nSelectedIndex].id;
}

void RingWrapper::SelectControl(const UIID & id, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    if (m_bIsEnabled) {
        SetSelectedControl(id, fnSelectionChanged);
        PointCursorAtControl(id);
    }
}

void RingWrapper::UpdateCursorVisibility()
{
    if (m_idCursor == GetUIIDNone()) {
        return;
    }

    UIWindow & tWndCursor = UI_GetWindowFromID(m_idCursor);

    if (GetSelectedControlID() == GetUIIDNone()) {
        tWndCursor.SetActive(FALSE);
    } else {
        tWndCursor.SetActive(m_bIsEnabled);
    }
}

void RingWrapper::SetEnabled(const BOOL bEnabled)
{
    m_bIsEnabled = bEnabled;

    UpdateCursorVisibility();
}

void RingWrapper::PointCursorAtControl(const UIID & id)
{
    if (m_idCursor == GetUIIDNone()) {
        return;
    }

    if (m_nInputSources != 1) {
        return;
    }

    const int32 nIndex = GetIndexOfControlID(id);
    if (nIndex == -1) {
        return;
    }

    const Angle aAngle = Math::NormalizeAngle(PI - m_arIcons[nIndex].aToCenter);
    UIWindow & tWndCursor = UI_GetWindowFromID(m_idCursor);
    tWndCursor.SetWindowRotation(aAngle);
}

void RingWrapper::SetSelectedControl(const UIID & id, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    const UIID idSelected = GetSelectedControlID();

    if (id != idSelected && fnSelectionChanged != NULL) {
        fnSelectionChanged(*this, idSelected, FALSE);
    }

    m_nSelectedIndex = GetIndexOfControlID(id);

    if (fnSelectionChanged != NULL) {
        fnSelectionChanged(*this, id, TRUE);
    }

    UpdateCursorVisibility();
}

int32 RingWrapper::GetControlIndexAt(const Vector2D & vecUnitDirection) const
{
    const int32 nNumIcons = m_arIcons.size();

    if (nNumIcons == 0) {
        return -1;
    }

    if (nNumIcons == 1) {
        return 0;
    }

    int32 nNextIconIndex = 0;
    const Angle aAngle = Math::ATan(vecUnitDirection.y, vecUnitDirection.x);

    for (int32 i = 0; i < nNumIcons; ++i) {
        if (aAngle < m_arIcons[i].aToCenter) {
            nNextIconIndex = i;
            break;
        }
    }

    const int32 nPrevIconIndex = (nNextIconIndex > 0) ? (nNextIconIndex - 1) : (nNumIcons - 1);

    const Angle aDiffPrev = Math::AngularDifference(aAngle, m_arIcons[nPrevIconIndex].aToCenter);
    const Angle aDiffNext = Math::AngularDifference(aAngle, m_arIcons[nNextIconIndex].aToCenter);

    return (aDiffPrev < aDiffNext) ? nPrevIconIndex : nNextIconIndex;
}

int32 RingWrapper::GetIndexOfControlID(const UIID & id) const
{
    for (int32 i = 0; i < m_arIcons.size(); ++i) {
        if (m_arIcons[i].id == id) {
            return i;
        }
    }

    return -1;
}

void RingWrapper::Reset()
{
    m_nSelectedIndex = -1;
    m_arIcons.clear();
}

int32 RingWrapper::sSortByAngle(const void * pLHS, const void * pRHS)
{
    const IconInfo * pLeft = (const IconInfo *)pLHS;
    const IconInfo * pRight = (const IconInfo *)pRHS;

    return (pLeft->aToCenter < pRight->aToCenter) ? -1 : ((pLeft->aToCenter > pRight->aToCenter) ? 1 : 0);
}

void RingWrapper::CalculateIconRegions(Console::RingWrapper::IncludeControlPredicate fnPredicate)
{
    if (m_idContainer == GetUIIDNone()) {
        return;
    }

    Vector2D vecRingTL;
    Vector2D vecRingDim;
    UI_GetWindowFromID(m_idContainer).GetRelativePosition(vecRingTL, vecRingDim);

    const Vector2D vecRingCenter(vecRingDim.x * 0.5f, vecRingDim.y * 0.5f);

    const UIControlItemArray & arChildIDs = UI_GetControlGroupFromID(m_idContainer).GetChildList();

    for (uint32 i = 0; i < arChildIDs.size(); ++i) {
        if (fnPredicate != NULL && !fnPredicate(arChildIDs[i].id)) {
            continue;
        }

        if (arChildIDs[i].id == m_idCursor) {
            continue;
        }

        UIWindow & tIcon = UI_GetWindowFromID(arChildIDs[i].id);
        if (!tIcon.IsItselfActive()) {
            continue;
        }

        Vector2D vecIconTL;
        Vector2D vecIconDim;
        tIcon.GetRelativePosition(vecIconTL, vecIconDim);

        const Vector2D vecIconCenter(vecIconTL.x + vecIconDim.x * 0.5f, vecIconTL.y + vecIconDim.y * 0.5f);
        Vector2D vecDelta = vecIconCenter - vecRingCenter;
        vecDelta.y = -vecDelta.y;

        IconInfo iconInfo;
        iconInfo.id = arChildIDs[i].id;
        iconInfo.aToCenter = Math::ATan(vecDelta.y, vecDelta.x);

        m_arIcons.push_back(iconInfo);
    }

    blzext::vector_sort(m_arIcons, sSortByAngle);
}

BOOL RingWrapper::UpdateTouchpad(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged)
{
    if (GetSelectedControlID() == GetUIIDNone()) {
        return FALSE;
    }

    GestureArray arGestures;

    if (m_tTouchGestureTracker.ProcessInput(nControllerIndex, arGestures) > 0) {
        SwipeGesture & tSwipe = arGestures[0];

        const int32 nControlIDCount = m_arIcons.size();
        s_nIconWidth = (int32)(s_nTotalWidth / nControlIDCount + s_nSlotWidth);

        const int32 nSlotDelta = tSwipe.vecDirection.x / s_nIconWidth;
        int32 nNewSelection = m_nTouchStartIcon + nSlotDelta;

        while (nNewSelection < 0 || nNewSelection >= nControlIDCount) {
            if (nNewSelection < 0) {
                nNewSelection += nControlIDCount;
            }
            if (nNewSelection >= nControlIDCount) {
                nNewSelection -= nControlIDCount;
            }
        }

        if (nNewSelection != m_nSelectedIndex) {
            s_nSelectedSlot = nNewSelection;
            SelectControl(m_arIcons[nNewSelection].id, fnSelectionChanged);
            return TRUE;
        }
    } else {
        m_nTouchStartIcon = m_nSelectedIndex;
    }

    return FALSE;
}

}  // namespace Console
