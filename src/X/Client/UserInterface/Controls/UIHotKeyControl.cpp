// X/Client/UserInterface/Controls/UIHotKeyControl.cpp -- the XML <Hotkey>
// control: a key or controller button bound to a user-interface action.
//
// `HotkeyCreationParams` is the descriptor an authored control loads from its
// XML attributes (an on-key function, an on-controller function and a UI
// action); `UIHotkey` is the live control.  Two helpers in here have no symbol
// in the binary and survive only in the 2.6.2 line table:
// `HotkeyCreationParams::sInit` (UIHotKeyControl.cpp:27, inlined into the
// constructor) and `sUIActionAllowsHeldRepeats` (:98, inlined into
// `sHandleInput`).

#include "UIHotKeyControl.h"

#include "../../../Common/Player.h"
#include "../../Definitions/ControllerSchemeDefinition.h"
#include "../../Definitions/PreferencesDefinition.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Input/Input.h"
#include "../Interface/UIFunctions.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/SigmaTiming.h"

// UIHotKeyControl.cpp:27 -- the authored default a hotkey descriptor starts
// from: no callbacks and no bound action until `Load` finds them.
void HotkeyCreationParams::sInit()
{
    m_pfnOnKey = NULL;
    m_pfnOnController = NULL;
    m_pfnHotkeyPredicate = NULL;

    m_nUIAction = ACTION_INVALID;
}

// UIHotKeyControl.cpp:39 -- the 0x71002ad6e0 listing stores the predicate
// (0xb8) before the merged onKey/onController pair (str xzr,[x19,#0xb8];
// stp xzr,xzr,[x19,#0xa8]); the separate-statement order in `sInit` is the
// natural form and the store merger + scheduler reach that order.
HotkeyCreationParams::HotkeyCreationParams(
    const UIType eType,
    const UIID & id,
    const UIID & idParent )
    : ControlCreationParams( eType, id, idParent )
{
    sInit();
}

// UIHotKeyControl.cpp:63 -- base `Load`, then the three attributes that make a
// hotkey: the on-key function, the on-controller function and the action.
void HotkeyCreationParams::Load(
    const SNO snoUI,
    const UIControlDataExt * pControlData )
{
    ControlCreationParams::Load( snoUI, pControlData );

    m_pfnOnKey = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONKEY ) );
    m_pfnOnController = UIConsoleFunction_Lookup( pControlData->GetAttributeValue( ATT_ONCONTROLLER ) );
    m_nUIAction = pControlData->GetInt32Value( ATT_HOTKEY );
}

// UIHotKeyControl.cpp:77 -- copy the descriptor into the control.  The action
// is pinned into `[ACTION_INVALID, ACTION_UI_LAST]`; `KEY_NONE` is "no key seen
// yet" and the held-repeat timestamp is zero so the first repeat fires.
UIHotkey::UIHotkey( const HotkeyCreationParams & tParams )
    : UIControl( tParams )
{
    m_pfnOnKey = tParams.m_pfnOnKey;
    m_pfnOnController = tParams.m_pfnOnController;
    m_pfnHotkeyPredicate = tParams.m_pfnHotkeyPredicate;
    m_nUIAction = Math::Pin( tParams.m_nUIAction, ACTION_INVALID, ACTION_UI_LAST );
    m_eLastInputKey = KEY_NONE;
    m_dwTickLastProcessedKeyStillDown = 0;
}

// UIHotKeyControl.cpp:92 -- how long a held hotkey waits between repeats.
static const uint32 HELD_HOTKEY_REPEAT_INTERVAL = 500;

// UIHotKeyControl.cpp:98 -- only the chat scroll actions are useful to repeat
// while the key stays down.
static BOOL sUIActionAllowsHeldRepeats( UIAction eAction )
{
    return eAction == ACTION_UI_CHAT_PAGE_UP || eAction == ACTION_UI_CHAT_PAGE_DOWN;
}

// UIHotKeyControl.cpp:111 -- the one place a hotkey decides whether the input
// is one of its own.  Keyboard key-down and key-still-down each carry their own
// copy of the "fire the on-key callback while the predicate admits the control"
// tail (the compiler then merges the two identical copies); the mouse-up path
// only answers whether the hotkey is currently active, and the controller
// paths dispatch through the on-controller callback.
BOOL UIHotkey::sHandleInput( const BufferedInput & tInputPassed )
{
    BufferedInput tInput = tInputPassed;
    const UIID & id = GetID();

    m_eLastInputKey = tInput.tBufKeyData.eKey;

    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_KEYDOWNONCE:
        if ( m_nUIAction != ACTION_INVALID )
        {
            m_dwTickLastProcessedKeyStillDown = GetMachineTicks();

            if ( Prefs::ActionKeyMatch( m_nUIAction, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch ) )
            {
                if ( m_pfnOnKey != NULL
                     && ( m_pfnHotkeyPredicate == NULL || m_pfnHotkeyPredicate( id ) ) )
                {
                    m_pfnOnKey( id );
                    return TRUE;
                }
            }
        }
        break;

    case INPUTMSG_KEYSTILLDOWN:
        if ( m_nUIAction != ACTION_INVALID && sUIActionAllowsHeldRepeats( m_nUIAction ) )
        {
            if ( tInput.tBufKeyData.eKey == KEY_ESCAPE || tInput.tBufKeyData.eKey == KEY_RETURN )
            {
                return FALSE;
            }

            uint32 dwNow = GetMachineTicks();

            if ( m_dwTickLastProcessedKeyStillDown + HELD_HOTKEY_REPEAT_INTERVAL < dwNow )
            {
                m_dwTickLastProcessedKeyStillDown = dwNow;

                if ( Prefs::ActionKeyMatch( m_nUIAction, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch ) )
                {
                    if ( m_pfnOnKey != NULL
                         && ( m_pfnHotkeyPredicate == NULL || m_pfnHotkeyPredicate( id ) ) )
                    {
                        m_pfnOnKey( id );
                        return TRUE;
                    }
                }
            }
        }
        break;

    case INPUTMSG_LEFTMOUSEUP:
        if ( m_nUIAction != ACTION_INVALID )
        {
            if ( Prefs::ActionKeyMatch( m_nUIAction, tInput.tBufKeyData.eKey, tInput.tBufKeyData.bExactMatch ) )
            {
                if ( m_pfnOnKey != NULL
                     && ( m_pfnHotkeyPredicate == NULL || m_pfnHotkeyPredicate( id ) ) )
                {
                    return TRUE;
                }
            }
        }
        break;

    case INPUTMSG_LEFTTRIGGER:
    case INPUTMSG_RIGHTTRIGGER:
    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
        if ( PlayerGetByControllerIndex( tInputPassed.tBufControllerData.nControllerIndex ) == NULL )
        {
            return FALSE;
        }

        if ( m_nUIAction != ACTION_INVALID )
        {
            if ( Controller::ActionButtonMatch( m_nUIAction, tInput ) )
            {
                if ( m_pfnOnController != NULL )
                {
                    m_pfnOnController( id, tInput.tBufControllerData.nControllerIndex );
                    return TRUE;
                }
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}

// UIHotKeyControl.cpp:230 -- an input goes through `sHandleInput` first; a
// control that can handle input asks to be registered when the UI announces
// the listener frame, and everything else falls through to the base class.
// The 0x71002ad970 listing tests UI_REGISTER_INPUT_LISTENER (cmp w8,#0x2)
// before UI_INPUT (cmp w8,#0x1) with both compares attributed to the same
// line 234, which is the switch dispatch shape used above.
BOOL UIHotkey::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    switch ( msg )
    {
    case UI_INPUT:
        {
            BOOL bRetval = sHandleInput( *(const BufferedInput *)dwParam );

            if ( bRetval )
            {
                return bRetval;
            }
        }
        break;

    case UI_REGISTER_INPUT_LISTENER:
        if ( HandlesInput() )
        {
            UI_RegisterInputListener( this );
        }
        break;

    default:
        break;
    }

    return UIControl::HandleMessage( msg, dwParam );
}

// UIHotKeyControl.cpp:268 -- only a hotkey with an on-controller callback
// listens to raw input.
BOOL UIHotkey::HandlesInput()
{
    return m_pfnOnController != NULL;
}
