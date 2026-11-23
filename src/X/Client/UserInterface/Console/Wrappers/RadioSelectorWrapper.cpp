// X/Client/UserInterface/Console/Wrappers/RadioSelectorWrapper.cpp
//
// The radio-button-list wrapper a console screen drives: it turns a container
// control's children into the list of option ids, tracks which one is
// selected, and reports a change through the two raw callbacks the owner
// installs.
//
// Byte-matching decompilation of the Diablo III 2.6.2 (Switch) build. Bodies
// are written from the DWARF line-attributed listing of the original binary
// (each function's address and size is in the comment) and from the inlined
// copies that listing shows; the original source line numbers are noted.

#include "X/Client/UserInterface/Console/Wrappers/RadioSelectorWrapper.h"

#include "X/Client/UserInterface/Controls/UIControlGroup.h"
#include "X/Client/UserInterface/UI.h"
#include "X/Client/UserInterface/UIID.h"

namespace Console {

// line 20 -- 0x710017b010, 68 bytes.
//
// The init list is in declaration order (container, selected index, both
// callbacks); m_aridOptions is default-constructed, so blz::vector() -> _init
// inlines to the three zero stores the listing shows at +0x08/+0x10/+0x18
// (vector.h:955).
RadioSelectorWrapper::RadioSelectorWrapper()
    : m_idContainer( GetUIIDNone() )
    , m_nSelectedIndex( -1 )
    , m_fnSelectionPredicate( NULL )
    , m_fnSelectionChanged( NULL )
{
}

// line 32 -- 0x710017b060, 100 bytes.
//
// Reset() (line 148) is inlined here: container = GetUIIDNone(), index = -1,
// options.clear().  The listing then assigns idContainer and tail-calls
// UpdateOptions().
void RadioSelectorWrapper::WrapUsingContainerControl( const UIID &idContainer )
{
    Reset();
    m_idContainer = idContainer;
    UpdateOptions();
}

// line 43 -- 0x710017b220, 4 bytes.
const UIID &RadioSelectorWrapper::GetContainerControlID() const
{
    return m_idContainer;
}

// line 51 -- 0x710017b230, 8 bytes.
const UIIDArray &RadioSelectorWrapper::GetOptionControlIDs() const
{
    return m_aridOptions;
}

// line 59 -- 0x710017b240, 8 bytes.
void RadioSelectorWrapper::SetCallbacks( SelectionPredicate fnSelectionPredicate, SelectionChanged fnSelectionChanged )
{
    m_fnSelectionPredicate = fnSelectionPredicate;
    m_fnSelectionChanged = fnSelectionChanged;
}

// line 69 -- 0x710017b250, 8 bytes.
int32 RadioSelectorWrapper::GetNumberOfOptions() const
{
    return static_cast<int32>( m_aridOptions.size() );
}

// line 77 -- 0x710017b260, 8 bytes.
int32 RadioSelectorWrapper::GetSelectedIndex() const
{
    return m_nSelectedIndex;
}

// line 85 -- 0x710017b270, 264 bytes.
BOOL RadioSelectorWrapper::SetSelectedIndex( const int32 nIndex, const BOOL bTriggeredByInput )
{
    BOOL bCanSelect = m_fnSelectionPredicate ? m_fnSelectionPredicate( nIndex ) : TRUE;

    if( bCanSelect )
    {
        if( m_nSelectedIndex != nIndex && NULL != m_fnSelectionChanged )
        {
            const int32 nOldSelectedIndex = m_nSelectedIndex;

            m_nSelectedIndex = nIndex;

            // The listing builds the "old" id first (sp+0x18, fed by
            // nOldSelectedIndex, line 100) and the "new" id second (sp+0x8,
            // fed by the new m_nSelectedIndex, line 101), so the two locals
            // are declared in that order even though the DWARF local list
            // prints idNew first.
            const UIID idOldSelectedOption = ( nOldSelectedIndex != -1 ) ? GetUIIDAtIndex( nOldSelectedIndex ) : GetUIIDNone();
            const UIID idNewSelectedOption = ( m_nSelectedIndex != -1 ) ? GetUIIDAtIndex( m_nSelectedIndex ) : GetUIIDNone();

            m_fnSelectionChanged( idOldSelectedOption, nOldSelectedIndex, FALSE, bTriggeredByInput );
            m_fnSelectionChanged( idNewSelectedOption, m_nSelectedIndex, TRUE, bTriggeredByInput );
        }
    }

    return bCanSelect;
}

// line 114 -- 0x710017b390, 44 bytes.
BOOL RadioSelectorWrapper::SelectNext( const BOOL bTriggeredByInput )
{
    if( m_nSelectedIndex < static_cast<int32>( m_aridOptions.size() ) - 1 )
    {
        return SetSelectedIndex( m_nSelectedIndex + 1, bTriggeredByInput );
    }

    return FALSE;
}

// line 127 -- 0x710017b3c0, 32 bytes.
BOOL RadioSelectorWrapper::SelectPrevious( const BOOL bTriggeredByInput )
{
    if( m_nSelectedIndex - 1 >= 0 )
    {
        return SetSelectedIndex( m_nSelectedIndex - 1, bTriggeredByInput );
    }

    return FALSE;
}

// line 140 -- 0x710017b380, 12 bytes (also inlined into SetSelectedIndex).
const UIID &RadioSelectorWrapper::GetUIIDAtIndex( const int32 nIndex ) const
{
    return m_aridOptions[nIndex];
}

// line 148 -- no out-of-line copy in the binary; inlined into
// WrapUsingContainerControl (line 35) and itself inlines UIID::operator= and
// vector::clear.
void RadioSelectorWrapper::Reset()
{
    m_idContainer = GetUIIDNone();
    m_nSelectedIndex = -1;
    m_aridOptions.clear();
}

// line 158 -- 0x710017b0d0, 332 bytes.
void RadioSelectorWrapper::UpdateOptions()
{
    if( m_idContainer == GetUIIDNone() )
    {
        return;
    }

    // line 168 (same line as the UI_GetControlGroupFromID call in the listing).
    // GetChildList() is the public inline accessor (UIControlGroup.h:149) that
    // returns &m_listChildren; its address computation folds into the two
    // vector loads below, so clang emits no inlined frame for it and the DWARF
    // records only this one binding, not an intermediate local.
    const UIControlItemArray &aridChildren = UI_GetControlGroupFromID( m_idContainer ).GetChildList();

    // The listing reloads the child vector's size every iteration because
    // push_back below can grow (and in the same object) m_aridOptions.
    for( uint32 i = 0; aridChildren.size() > i; ++i )
    {
        m_aridOptions.push_back( aridChildren[i].id );
    }
}

}  // namespace Console
