// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/Wrappers/RingWrapper.h

#pragma once

#include "../../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Console/TouchGestureTracker.h"
#include "../../Controls/UIControlGroup.h"
#include "../../UIID.h"
#include "../../../../Sigma/Math/MathGeneral.h"
#include "../../../../Sigma/SigmaTypes.h"

struct Vector2D;

namespace Console {
class RingWrapper {  // size 0x68
public:
    typedef BOOL (*IncludeControlPredicate)(const UIID &);
    typedef void (*IconSelectionChanged)(const Console::RingWrapper &, const UIID &, const BOOL);
public:
    RingWrapper();
    ~RingWrapper();
    void WrapUsingContainerControl(const UIID & idContainer, const UIID & idCursor, Console::RingWrapper::IncludeControlPredicate fnPredicate, BOOL bForceUpdate);
    const UIID & GetContainerControlID() const;
    void GetTargetControlIDs(UIIDArray & aridTargets) const;
    // 2.7.7 added the fnCanSelect argument (unnamed in the binary): when non-NULL, a ring icon is
    // selected only if it returns true (UpdateRingSelection 0x710038a270, UpdateTouchpad 0x7100389dd0).
    void UpdateInput(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged, bool (*fnCanSelect)(const Console::RingWrapper &, const UIID &));
    const UIID & GetSelectedControlID() const;
    void SelectControl(const UIID & id, Console::RingWrapper::IconSelectionChanged fnSelectionChanged);
    void SelectControlIndex(int32, Console::RingWrapper::IconSelectionChanged);
    void SelectNextClockwise(Console::RingWrapper::IconSelectionChanged);
    void SelectNextCounterClockwise(Console::RingWrapper::IconSelectionChanged);
    BOOL IsEnabled() const { return m_bIsEnabled; }  // line 102
    void SetEnabled(const BOOL bEnabled);
    typedef uint32 InputSources;
    void SetInputSources(Console::RingWrapper::InputSources);
private:
    struct IconInfo {  // size 0x10
        Angle aToCenter;  // 0x00
        UIID id;  // 0x08
    };
    blz::vector<Console::RingWrapper::IconInfo, blz::allocator<Console::RingWrapper::IconInfo> > m_arIcons;  // 0x00
    UIID m_idContainer;  // 0x18
    UIID m_idCursor;  // 0x20
    int32 m_nSelectedIndex;  // 0x28
    BOOL m_bIsEnabled;  // 0x2c
    Console::RingWrapper::InputSources m_nInputSources;  // 0x30
    Console::TouchGestureTracker m_tTouchGestureTracker;  // 0x34
    int32 m_nTouchStartIcon;  // 0x64
    void UpdateAnalogStickInput(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged, bool (*fnCanSelect)(const Console::RingWrapper &, const UIID &));
    void UpdateRingSelection(const Vector2D & vecUnitDirection, Console::RingWrapper::IconSelectionChanged fnSelectionChanged, bool (*fnCanSelect)(const Console::RingWrapper &, const UIID &));
    void Reset();
    BOOL IsInDeadZone(const int32 nControllerIndex);
    void CalculateIconRegions(Console::RingWrapper::IncludeControlPredicate fnPredicate);
    int32 GetControlIndexAt(const Vector2D & vecUnitDirection) const;
    int32 GetIndexOfControlID(const UIID & id) const;
    void SetSelectedControl(const UIID & id, Console::RingWrapper::IconSelectionChanged fnSelectionChanged);
    void PointCursorAtControl(const UIID & id);
    void UpdateCursorVisibility();
    static int32 sSortByAngle(const void * pLHS, const void * pRHS);
    BOOL UpdateTouchpad(const int32 nControllerIndex, Console::RingWrapper::IconSelectionChanged fnSelectionChanged, bool (*fnCanSelect)(const Console::RingWrapper &, const UIID &));
};
static_assert(sizeof(RingWrapper) == 0x68, "RingWrapper");

}  // namespace Console
