// X/Client/UserInterface/Controls/UIKeyMappingButtonControl.cpp -- the
// key-remapping button: a `UIButton` that, while the mouse is over it,
// swallows the next key-down, remembers the key and hands it to an authored
// callback.
//
// Two helpers in here have no symbol in the binary and survive only in the
// 2.6.2 line table: `KeyMappingButtonCreationParams::sInit`
// (UIKeyMappingButtonControl.cpp:22, inlined into both constructors) and
// `UIKeyMappingButton::sInit` (:66, inlined into the constructor).

#include "UIKeyMappingButtonControl.h"

#include "../UIAttributes.h"
#include "../../Input/Input.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../Interface/UIFunctions.h"
#include "../UIID.h"
#include "../UIMessage.h"

// Declarations of the functions this file calls whose headers are not adopted yet.
// X/Client/UserInterface/Interface/UIFunctions.cpp
UIFunctionPtr UIFunction_Lookup(LPCSTR szFunction);
// X/Client/UserInterface/UI.cpp
const UIID & UI_GetMousedOverControl();

// UIKeyMappingButtonControl.cpp:22 -- the authored default a key-mapping
// descriptor starts from: no callback until `Load` finds one.
void KeyMappingButtonCreationParams::sInit()
{
    pfnOnKey = NULL;
}

// UIKeyMappingButtonControl.cpp:31

KeyMappingButtonCreationParams::KeyMappingButtonCreationParams(
    const UIType eType,
    const UIID & id,
    const UIID & idParent )
    : ButtonCreationParams( eType, id, idParent )
{
    sInit();
}

// UIKeyMappingButtonControl.cpp:55 -- base `Load`, then one extra
// attribute-driven callback field.
void KeyMappingButtonCreationParams::Load(
    const SNO snoUI,
    const UIControlDataExt * pControlData )
{
    ButtonCreationParams::Load( snoUI, pControlData );

    pfnOnKey = UIFunction_Lookup( pControlData->GetAttributeValue( ATT_ONKEY ) );
}

// UIKeyMappingButtonControl.cpp:66 -- copy the descriptor into the control.
// `KEY_NONE` is "no key seen yet".
void UIKeyMappingButton::sInit( const KeyMappingButtonCreationParams & tParams )
{
    m_eLastKey = KEY_NONE;
    m_pfnOnKey = tParams.pfnOnKey;
}

// UIKeyMappingButtonControl.cpp:87

UIKeyMappingButton::UIKeyMappingButton( const KeyMappingButtonCreationParams & tParams, const Rect2D * pRect )
    : UIButton( tParams, pRect )
{
    sInit( tParams );
}

// UIKeyMappingButtonControl.cpp:98 -- the enabled button records the key and
// fires the callback only while this exact instance is the moused-over
// control, so a screenful of key rows can all be listening and only the one
// under the cursor reacts.  Any other message goes to the base class.
BOOL UIKeyMappingButton::HandleMessage( const UIMsg & msg, const uintptr dwParam )
{
    const UIID & id = GetID();

    if ( msg == UI_INPUT )
    {
        const BufferedInput & tInput = *(const BufferedInput *)dwParam;

        if ( tInput.eInputMsg == INPUTMSG_KEYDOWNONCE )
        {
            if ( !IsEnabled() )
            {
                return FALSE;
            }

            m_eLastKey = tInput.tBufKeyData.eKey;

            if ( m_pfnOnKey )
            {
                if ( UI_GetMousedOverControl() == id )
                {
                    m_pfnOnKey( id );
                    return TRUE;
                }
            }

            return FALSE;
        }
    }

    return UIButton::HandleMessage( msg, dwParam );
}
