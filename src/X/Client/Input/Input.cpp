// Original file: X/Client/Input/Input.cpp
#include "../../Sigma/SigmaTiming.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Input.h"
#include "../Gfx/GfxVB.h"
#include "../../Sigma/DataStructures/DataStructuresExternal.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Common/Definitions/GlobalDefinition.h"
void ErrorManagerSetCapsDepressedCallback( PFNCAPSDEPRESSEDCALLBACK pfnCapsDepressedCallback );  // ErrorManager.cpp:486
// X/Client/Input/InputNX64.cpp
BOOL InputPlatformInitialize();
void InputPlatformCursorSet( CursorTypes nType );
BOOL InputPlatformAppHasCapture();
BOOL InputPlatformCapsLockIsDown();
BOOL InputPlatformIsCursorInWindow();
void InputPlatformShowPlatformCursor( const BOOL fShow, const BOOL fIgnoreState );
static const uint32 MAX_CONTROLLERS = 4;  // X/Client/Input/Input.h:21 (DWARF; src Input.h lacks it)
struct InputListenerList {  // size 0x208
    int32 nCount;  // 0x00
    InputListenerFunc pfunc[64];  // 0x08
};
static_assert(sizeof(InputListenerList) == 0x208, "InputListenerList");

struct InputContextGlobal {  // size 0x64d8
    InputContextGlobal();
    CursorTypes eCursor;  // 0x00
    InputState tInputState;  // 0x04
    InputListenerList tListenerLists[46];  // 0x768
};
static_assert(sizeof(InputContextGlobal) == 0x64d8, "InputContextGlobal");

struct InputGlobals {  // size 0xc9b8
    InputGlobals();
    InputContextGlobal arContext[2];  // 0x00
    BOOL bControllerPresent;  // 0xc9b0
};
static_assert(sizeof(InputGlobals) == 0xc9b8, "InputGlobals");

void InputPlatformUpdate( InputState & tInputState );  // InputNX64.cpp:715
Key InputPlatformKeyConvert( const uint16 wPlatformKey );  // InputNX64.cpp:1003
int32 InputPlatformGetControllerTriggerSlack();  // InputNX64.cpp:985


void InputReset();
inline InputContextGlobal::InputContextGlobal()
{
}





inline InputGlobals::InputGlobals()
{
}














inline BufferedControllerData::BufferedControllerData()
    : nControllerIndex( 0 )
    , eInputButton( -1 )
    , nRightTriggerPos( 0 )
    , nLeftTriggerPos( 0 )
{
}



BufferedInput::BufferedInput()
    : eInputMsg( -1 )
{
}
















inline ControllerFrameState::ControllerFrameState()
    : bIsConnected( FALSE ), uButtons( 0 ), nLeftTrigger( 0 ), nRightTrigger( 0 ), vecLeftStick( 0.0f, 0.0f ), vecRightStick( 0.0f, 0.0f ), nNumTouches( 0 )
{
}



BOOL ControllerFrameState::IsButtonDown( InputButton eButton ) const
{
    if ( eButton == -1 )
        return FALSE;
    return ( uButtons >> eButton ) & 1;
}

inline ControllerState::ControllerState()
    : tLastHeldDownCheckTime( 0 )
    , tLStickLastExtremeTime( 0 )
    , tRStickLastExtremeTime( 0 )
{
    SigmaMemoryClear( tButtonLastDownTimes, sizeof( tButtonLastDownTimes ) );
    SigmaMemoryClear( tButtonHeldDownTimes, sizeof( tButtonHeldDownTimes ) );
}




static InputGlobals sgtGlobals;

static BOOL sIsCapsLockDown();
BOOL InputCapsLockIsDown();






















BOOL InputInitialize()
{
    BOOL bResult = InputPlatformInitialize();
    InputReset();
    ErrorManagerSetCapsDepressedCallback( sIsCapsLockDown );
    return bResult;
}

























void InputReset()
{
    for ( int32 i = 0; i < NUM_INPUTCONTEXTS; i++ )
    {
        SigmaMemoryClear( &sgtGlobals.arContext[i].tInputState, sizeof( InputState ) );
        sgtGlobals.arContext[i].tInputState.dwFlags = 3;
    }
}
























BOOL InputRegisterListener( const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext, int32 nIndex )
{
    InputListenerList & tList = sgtGlobals.arContext[eContext].tListenerLists[eInputMsg];






    XASSERT( tList.nCount < MAX_INPUT_LISTENERS );
    if ( nIndex == -1 || nIndex >= tList.nCount )
    {
        tList.pfunc[tList.nCount++] = pfunc;
    }
    else
    {
        SigmaMemoryMove( &tList.pfunc[nIndex + 1], &tList.pfunc[nIndex], ( tList.nCount - nIndex ) * sizeof( InputListenerFunc ) );
        tList.pfunc[nIndex] = pfunc;
        tList.nCount++;
    }
    return TRUE;
}





















void InputUnregisterListener( const InputMsg eInputMsg, const InputListenerFunc pfunc, InputContext eContext )
{
    InputListenerList & tList = sgtGlobals.arContext[eContext].tListenerLists[eInputMsg];
    for ( int32 i = 0; i < tList.nCount; i++ )
    {
        if ( tList.pfunc[i] == pfunc )
        {
            if ( i < tList.nCount - 1 )
            {
                SigmaMemoryMove( &tList.pfunc[i], &tList.pfunc[i + 1], ( tList.nCount - i - 1 ) * sizeof( InputListenerFunc ) );
            }
            tList.nCount--;
        }
    }
}






















static BOOL sCallListeners(
    const BufferedInput & tInput,
    InputContext eContext )
{


    InputListenerList & tListenerList = sgtGlobals.arContext[eContext].tListenerLists[tInput.eInputMsg];
    BOOL fHandled = FALSE;
    for ( int32 i = 0; i < tListenerList.nCount; ++i )
    {
        fHandled = tListenerList.pfunc[i]( tInput );
        if ( fHandled ) break;
    }
    return fHandled;
}








static BOOL sIsCapsLockDown()
{
    return InputCapsLockIsDown();
}





void InputBufferInput(
    const BufferedInput & tInput,
    InputContext eContext,
    BOOL bForce )
{


    if ( SigmaCurrentlyHandlingError() )
        return;




    static int32 nStopReenteringCount = 0;

    if ( nStopReenteringCount > 0 && !bForce )
        return;





    nStopReenteringCount++;

    MouseState & tMouseState = sgtGlobals.arContext[eContext].tInputState.tMouseState;
    KeyboardState & tKeyboardState = sgtGlobals.arContext[eContext].tInputState.tKeyboardState;
    BufferedInput tListenerInput = tInput;

    BOOL fHandled = FALSE;

    switch ( tInput.eInputMsg )
    {
    case INPUTMSG_MOUSEMOVE:

        tMouseState.p = tInput.tBufMouseData.p;
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_BUTTONDOWNONCE:
    case INPUTMSG_BUTTONDOWN:
    case INPUTMSG_BUTTONUP:
        sCallListeners( tListenerInput, eContext );
        break;
    case INPUTMSG_CHARACTER:
        sCallListeners( tListenerInput, eContext );
        break;


    case INPUTMSG_KEYSTILLDOWN:
        {
            BufferedInput tListenerInputExact = tListenerInput;
            tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
            fHandled = sCallListeners( tListenerInputExact, eContext );
            if ( !fHandled )
            {

                sCallListeners( tListenerInput, eContext );
            }
        }
        break;

    case INPUTMSG_KEYDOWN:
        {
            Key eKey = InputPlatformKeyConvert( tInput.tBufKeyData.wPlatformKey );
            if ( eKey != KEY_NONE )
            {
                tListenerInput.tBufKeyData.eKey = eKey;

                BOOL bPreviouslyDown = tKeyboardState.bCurrentKeys[eKey];

                tKeyboardState.bCurrentKeys[eKey] = TRUE;
                if ( !bPreviouslyDown )
                {













































                    tListenerInput.eInputMsg = INPUTMSG_KEYDOWNONCE;


                    BufferedInput tListenerInputExact = tListenerInput;
                    tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
                    fHandled = sCallListeners( tListenerInputExact, eContext );
                    if ( !fHandled )
                    {

                        sCallListeners( tListenerInput, eContext );
                    }
                }


                tListenerInput.eInputMsg = INPUTMSG_KEYDOWN;
                sCallListeners( tListenerInput, eContext );
            }
        }
        break;

    case INPUTMSG_KEYUP:
        {
            Key eKey = InputPlatformKeyConvert( tInput.tBufKeyData.wPlatformKey );
            if ( eKey != KEY_NONE )
            {
                tListenerInput.tBufKeyData.eKey = eKey;

                tKeyboardState.bCurrentKeys[eKey] = FALSE;
























                BufferedInput tListenerInputExact = tListenerInput;
                tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
                fHandled = sCallListeners( tListenerInputExact, eContext );
                if ( !fHandled )
                {

                    sCallListeners( tListenerInput, eContext );
                }
            }
        }
        break;

    case INPUTMSG_LEFTMOUSEDOWN:
    case INPUTMSG_LEFTMOUSEDOUBLECLICK:
        tMouseState.dwClickTime[LEFT_MOUSE_BUTTON] = GetMachineTicks();
        // fall through
    case INPUTMSG_LEFTMOUSESTILLDOWN:
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags |= ( 1 << MOUSESTATE_LEFT_BUTTON_DOWN_BIT );
        // fall through
    case INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY:
        {
            BOOL bPreviouslyDown = tKeyboardState.bCurrentKeys[KEY_MOUSE1];

            tKeyboardState.bCurrentKeys[KEY_MOUSE1] = TRUE;
            if ( !bPreviouslyDown )
            {




                BufferedInput tListenerInputExact;
                tListenerInputExact.eInputMsg = INPUTMSG_KEYDOWNONCE;
                tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE1;
                tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
                fHandled = sCallListeners( tListenerInputExact, eContext );
            }
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_LEFTMOUSEUP:
        tMouseState.dwClickTime[LEFT_MOUSE_BUTTON] = 0;
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags &= ~( 1 << MOUSESTATE_LEFT_BUTTON_DOWN_BIT );

        tKeyboardState.bCurrentKeys[KEY_MOUSE1] = FALSE;

        {
            BufferedInput tListenerInputExact;
            tListenerInputExact.eInputMsg = INPUTMSG_KEYUP;
            tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE1;
            tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
            fHandled = sCallListeners( tListenerInputExact, eContext );
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_MIDDLEMOUSEDOWN:
    case INPUTMSG_MIDDLEMOUSEDOUBLECLICK:
        tMouseState.dwClickTime[MIDDLE_MOUSE_BUTTON] = GetMachineTicks();
        // fall through
    case INPUTMSG_MIDDLEMOUSESTILLDOWN:
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags |= ( 1 << MOUSESTATE_MIDDLE_BUTTON_DOWN_BIT );
        // fall through
    case INPUTMSG_MIDDLEMOUSESTILLDOWN_NO_DELAY:
        {
            BOOL bPreviouslyDown = tKeyboardState.bCurrentKeys[KEY_MOUSE3];

            tKeyboardState.bCurrentKeys[KEY_MOUSE3] = TRUE;
            if ( !bPreviouslyDown )
            {




                BufferedInput tListenerInputExact;
                tListenerInputExact.eInputMsg = INPUTMSG_KEYDOWNONCE;
                tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE3;
                tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
                fHandled = sCallListeners( tListenerInputExact, eContext );
            }
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_MIDDLEMOUSEUP:
        tMouseState.dwClickTime[MIDDLE_MOUSE_BUTTON] = 0;
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags &= ~( 1 << MOUSESTATE_MIDDLE_BUTTON_DOWN_BIT );

        tKeyboardState.bCurrentKeys[KEY_MOUSE3] = FALSE;

        {
            BufferedInput tListenerInputExact;
            tListenerInputExact.eInputMsg = INPUTMSG_KEYUP;
            tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE3;
            tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
            fHandled = sCallListeners( tListenerInputExact, eContext );
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_RIGHTMOUSEDOWN:
    case INPUTMSG_RIGHTMOUSEDOUBLECLICK:
        tMouseState.dwClickTime[RIGHT_MOUSE_BUTTON] = GetMachineTicks();
        // fall through
    case INPUTMSG_RIGHTMOUSESTILLDOWN:
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags |= ( 1 << MOUSESTATE_RIGHT_BUTTON_DOWN_BIT );
        // fall through
    case INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY:
        {
            BOOL bPreviouslyDown = tKeyboardState.bCurrentKeys[KEY_MOUSE2];

            tKeyboardState.bCurrentKeys[KEY_MOUSE2] = TRUE;
            if ( !bPreviouslyDown )
            {




                BufferedInput tListenerInputExact;
                tListenerInputExact.eInputMsg = INPUTMSG_KEYDOWNONCE;
                tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE2;
                tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
                fHandled = sCallListeners( tListenerInputExact, eContext );
            }
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_RIGHTMOUSEUP:
        tMouseState.dwClickTime[RIGHT_MOUSE_BUTTON] = 0;
        tMouseState.p = tInput.tBufMouseData.p;
        tMouseState.dwFlags &= ~( 1 << MOUSESTATE_RIGHT_BUTTON_DOWN_BIT );

        tKeyboardState.bCurrentKeys[KEY_MOUSE2] = FALSE;

        {
            BufferedInput tListenerInputExact;
            tListenerInputExact.eInputMsg = INPUTMSG_KEYUP;
            tListenerInputExact.tBufKeyData.eKey = KEY_MOUSE2;
            tListenerInputExact.tBufKeyData.bExactMatch = TRUE;
            fHandled = sCallListeners( tListenerInputExact, eContext );
        }

        if ( !fHandled )
        {
            sCallListeners( tListenerInput, eContext );
        }
        break;

    case INPUTMSG_MOUSEWHEEL:
        tListenerInput.tBufMouseData.p = tMouseState.p;
        tMouseState.nMouseWheelPos += tInput.tBufMouseData.nDeltaMouseWheel;
        tMouseState.nMouseWheelDeltaSinceLastUpdate += tInput.tBufMouseData.nDeltaMouseWheel;

        if ( !sCallListeners( tListenerInput, eContext ) )
        {
            Key eKeySimulated = KEY_MOUSEWHEELUP;
            if ( tInput.tBufMouseData.nDeltaMouseWheel < 0 )
                eKeySimulated = KEY_MOUSEWHEELDOWN;




            tKeyboardState.bCurrentKeys[eKeySimulated] = TRUE;

            BufferedInput tListenerInputSimulatedKey;
            tListenerInputSimulatedKey.eInputMsg = INPUTMSG_KEYDOWNONCE;
            tListenerInputSimulatedKey.tBufKeyData.eKey = eKeySimulated;
            tListenerInputSimulatedKey.tBufKeyData.bExactMatch = TRUE;
            if ( !sCallListeners( tListenerInputSimulatedKey, eContext ) )
            {
                tListenerInputSimulatedKey.tBufKeyData.bExactMatch = FALSE;
                sCallListeners( tListenerInputSimulatedKey, eContext );
            }

            tListenerInputSimulatedKey.eInputMsg = INPUTMSG_KEYUP;
            tListenerInputSimulatedKey.tBufKeyData.eKey = eKeySimulated;
            tListenerInputSimulatedKey.tBufKeyData.bExactMatch = TRUE;
            if ( !sCallListeners( tListenerInputSimulatedKey, eContext ) )
            {
                tListenerInputSimulatedKey.tBufKeyData.bExactMatch = FALSE;
                sCallListeners( tListenerInputSimulatedKey, eContext );
            }
            
            tKeyboardState.bCurrentKeys[eKeySimulated] = FALSE;
        }
        break;


    case INPUTMSG_LEFTSTICK:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_RIGHTSTICK:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_RIGHTTRIGGER:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_LEFTTRIGGER:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_CONTROLLERBUTTONDOWN:
    case INPUTMSG_CONTROLLERBUTTONSTILLDOWN:
    case INPUTMSG_CONTROLLERBUTTONHELDDOWN:
    case INPUTMSG_CONTROLLERBUTTONREPEAT:
    case INPUTMSG_CONTROLLERBUTTONUP:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_GESTUREFLICK:
        sCallListeners( tListenerInput, eContext );
        break;

    case INPUTMSG_IME_START_COMPOSITION:
    case INPUTMSG_IME_END_COMPOSITION:
    case INPUTMSG_IME_SHOW_COMPOSITION:
    case INPUTMSG_IME_INSERT_RESULT:
    case INPUTMSG_IME_LANGUAGE_CHANGE:
    case INPUTMSG_IME_OPEN_CANDIDATES:
    case INPUTMSG_IME_CHANGE_CANDIDATES:
    case INPUTMSG_IME_CLOSE_CANDIDATES:
    case INPUTMSG_IME_SHOWREADING:
    case INPUTMSG_IME_HIDEREADING:
        sCallListeners( tListenerInput, eContext );
        break;
    }

    nStopReenteringCount = ( nStopReenteringCount > 0 ) ? nStopReenteringCount - 1 : 0;

}




const InputState & InputGetCurrentState( InputContext eContext )
{
    return sgtGlobals.arContext[eContext].tInputState;
}





BOOL InputShiftKeyIsDown( InputContext eContext )
{
    const KeyboardState & tKeyboardState = sgtGlobals.arContext[eContext].tInputState.tKeyboardState;
    return tKeyboardState.bCurrentKeys[KEY_LSHIFT] || tKeyboardState.bCurrentKeys[KEY_RSHIFT];
}









BOOL InputCtrlKeyIsDown( InputContext eContext )
{
    const KeyboardState & tKeyboardState = sgtGlobals.arContext[eContext].tInputState.tKeyboardState;
    return tKeyboardState.bCurrentKeys[KEY_LCONTROL] || tKeyboardState.bCurrentKeys[KEY_RCONTROL];
}









BOOL InputAltKeyIsDown( InputContext eContext )
{
    const KeyboardState & tKeyboardState = sgtGlobals.arContext[eContext].tInputState.tKeyboardState;
    return tKeyboardState.bCurrentKeys[KEY_LMENU] || tKeyboardState.bCurrentKeys[KEY_RMENU];
}








inline BOOL InputCapsLockIsDown()
{


    return InputPlatformCapsLockIsDown();
}






















BOOL InputAppHasCapture()
{


    return InputPlatformAppHasCapture();
}





// Defined below (X/Client/Input/Input.cpp:1457, 1469, 1481); forward declarations inferred, the
// original calls them from InputUpdate before their definitions.
static void sUpdatePrevFrameControllerState( InputState & tInputState );
static void sProcessControllerInputs( InputState & tInputState );
static void sProcessGestureInputs( InputState & tInputState );


void InputUpdate()
{





    sUpdatePrevFrameControllerState( sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState );
    InputPlatformUpdate( sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState );
    sProcessControllerInputs( sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState );
    sProcessGestureInputs( sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState );


























































































}




void InputResetMouseWheelDelta()
{
    sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState.tMouseState.nMouseWheelDeltaSinceLastUpdate = 0;
}





















BOOL InputIsKeyboard( const BufferedInput & tInput )
{
    return tInput.eInputMsg >= INPUTMSG_CHARACTER && tInput.eInputMsg <= INPUTMSG_KEYUP;
}








BOOL InputIsGesture( const BufferedInput & tInput )
{
    return tInput.eInputMsg >= INPUTMSG_GESTURE_START && tInput.eInputMsg <= INPUTMSG_GESTURE_END;
}










BOOL InputIsController( const BufferedInput & tInput )
{
    return tInput.eInputMsg >= INPUTMSG_CONTROLLER_START && tInput.eInputMsg <= INPUTMSG_CONTROLLER_END;
}











BOOL InputIsIME( const BufferedInput & tInput )
{
    return tInput.eInputMsg >= INPUTMSG_IME_START_COMPOSITION && tInput.eInputMsg <= INPUTMSG_IME_LAST;
}





















IVector2D InputGetCursorPos()
{
    return sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState.tMouseState.p;
}





BOOL InputIsCursorInWindow()
{

    return InputPlatformIsCursorInWindow();
}




void InputShowPlatformCursor( const BOOL fShow, const BOOL fIgnoreState )
{


    InputPlatformShowPlatformCursor( fShow, fIgnoreState );
}





void InputCursorSet( CursorTypes eCursorType )
{
    sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].eCursor = eCursorType;
    InputPlatformCursorSet( eCursorType );
}







































void InputSetControllerPresent( BOOL bPresent )
{
    sgtGlobals.bControllerPresent = bPresent;
}





BOOL InputIsControllerPresent()
{
    return sgtGlobals.bControllerPresent;
}





BOOL InputIsControllerConnected( int32 nControllerIndex )
{
    return sgtGlobals.arContext[INPUTCONTEXT_DEFAULT].tInputState.tControllerState[nControllerIndex].tCurrentFrame.bIsConnected;
}







uint32 InputGetNumConnectedControllers()
{
    uint32 nCount = 0;
    for ( int32 i = 0; i < 4; i++ )
    {
        if ( InputIsControllerConnected( i ) )
        {
            nCount++;
        }
    }
    return nCount;
}










static void sSendLeftAnalogStickEvent(
    const ControllerFrameState & tControllerFrameState,
    int32 nControllerIndex )
{
    BufferedInput tInput;
    tInput.tBufControllerData.nControllerIndex = nControllerIndex;
    tInput.tBufControllerData.vLeftStick = tControllerFrameState.vecLeftStick;
    tInput.eInputMsg = INPUTMSG_LEFTSTICK;
    InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );
}




static void sSendRightAnalogStickEvent(
    const ControllerFrameState & tControllerFrameState,
    int32 nControllerIndex )
{
    BufferedInput tInput;
    tInput.tBufControllerData.nControllerIndex = nControllerIndex;
    tInput.tBufControllerData.vRightStick = tControllerFrameState.vecRightStick;
    tInput.eInputMsg = INPUTMSG_RIGHTSTICK;
    InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );
}




static void sUpdateStickReboundingTime(
    ControllerState & tControllerState )
{



    ScopedGlobalDefinitionLockR pdefGlobal;
    float flReboundDetectionRange = pdefGlobal->flControllerReboundDetectionZone;

    if ( tControllerState.tCurrentFrame.vecLeftStick.MagnitudeSquared() > flReboundDetectionRange * flReboundDetectionRange )
    {
        tControllerState.tLStickLastExtremeTime = Blizzard::Time::Milliseconds();
    }

    if ( tControllerState.tCurrentFrame.vecRightStick.MagnitudeSquared() > flReboundDetectionRange * flReboundDetectionRange )
    {
        tControllerState.tRStickLastExtremeTime = Blizzard::Time::Milliseconds();
    }
}





static void sSendStickEvents(
    ControllerState & tControllerState,
    int32 nControllerIndex )
{

    sUpdateStickReboundingTime( tControllerState );


    ControllerFrameState & tControllerCurrentState = tControllerState.tCurrentFrame;
    sSendLeftAnalogStickEvent( tControllerCurrentState, nControllerIndex );
    sSendRightAnalogStickEvent( tControllerCurrentState, nControllerIndex );
}




static void sSetButtonHeldStart(
    ControllerState & tControllerState,
    InputButton eButton )
{
    if ( tControllerState.tButtonHeldDownTimes[eButton] == 0 )
    {
        tControllerState.tButtonHeldDownTimes[eButton] = Blizzard::Time::Milliseconds();
    }
}




static void sSendButtonEvents(
    ControllerState & tControllerState,
    int32 nControllerIndex )
{
    BufferedInput tButtonInput;
    tButtonInput.tBufControllerData.nControllerIndex = nControllerIndex;


    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();

    for ( uint32 i = 0; i < CONSOLE_BUTTON_COUNT; ++i )
    {

        if ( i == BUTTON_LEFT_TRIGGER || i == BUTTON_RIGHT_TRIGGER )
        {
            continue;
        }

        InputButton eButton = (InputButton)i;
        BOOL bWasDown = tControllerState.tPrevFrame.IsButtonDown( eButton );
        BOOL bIsDown = tControllerState.tCurrentFrame.IsButtonDown( eButton );

        if ( !bWasDown && !bIsDown )
        {
            continue;
        }


        tButtonInput.tBufControllerData.eInputButton = eButton;
        if ( !bWasDown && bIsDown )
        {
            tButtonInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONDOWN;
        }
        else if ( bWasDown && !bIsDown )
        {
            tButtonInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONUP;
            tControllerState.tButtonHeldDownTimes[eButton] = 0;
            tControllerState.tButtonLastDownTimes[eButton] = Blizzard::Time::Milliseconds();
        }
        else if ( bWasDown && bIsDown )
        {

            tButtonInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONSTILLDOWN;
            sSetButtonHeldStart( tControllerState, eButton );
            tControllerState.tButtonLastDownTimes[eButton] = Blizzard::Time::Milliseconds();
        }

        InputBufferInput( tButtonInput, INPUTCONTEXT_DEFAULT, FALSE );

        if ( bIsDown )
        {

            if ( tControllerState.tButtonHeldDownTimes[eButton] != 0 &&
                now - tControllerState.tButtonHeldDownTimes[eButton] > 1000 &&
                tControllerState.tLastHeldDownCheckTime - tControllerState.tButtonHeldDownTimes[eButton] <= 1000 )
            {
                tButtonInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONHELDDOWN;
                InputBufferInput( tButtonInput, INPUTCONTEXT_DEFAULT, FALSE );
            }
        }
        tControllerState.tLastHeldDownCheckTime = now;
    }
}




enum Trigger {
    LEFT_TRIGGER = 0,
    RIGHT_TRIGGER = 1,
};

static void sHandleTriggerInput(
    ControllerState & tControllerState,
    int32 nControllerIndex,
    Trigger eTrigger )
{
    int32 nTriggerValue = (eTrigger == LEFT_TRIGGER) ? tControllerState.tCurrentFrame.nLeftTrigger : tControllerState.tCurrentFrame.nRightTrigger;
    InputButton eButton = (eTrigger == LEFT_TRIGGER) ? BUTTON_LEFT_TRIGGER : BUTTON_RIGHT_TRIGGER;

    if ( nTriggerValue > InputPlatformGetControllerTriggerSlack() )
    {
        BufferedInput tInput;
        tInput.eInputMsg = (eTrigger == LEFT_TRIGGER) ? INPUTMSG_LEFTTRIGGER : INPUTMSG_RIGHTTRIGGER;
        tInput.tBufControllerData.nControllerIndex = nControllerIndex;
        if ( eTrigger == LEFT_TRIGGER )
        {
            tInput.tBufControllerData.nLeftTriggerPos = tControllerState.tCurrentFrame.nLeftTrigger;
        }
        else
        {
            tInput.tBufControllerData.nRightTriggerPos = tControllerState.tCurrentFrame.nRightTrigger;
        }
        InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );

        if ( tControllerState.tButtonLastDownTimes[eButton] != 0 )
        {
            // 50: literal from the binary (cmp #0x32), no named constant found in the CU.
            if ( tControllerState.tButtonHeldDownTimes[eButton] < 50 )
            {
                tInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONSTILLDOWN;
            }
            else
            {
                tInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONREPEAT;
            }
        }
        else
        {
            tInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONDOWN;
        }
        tInput.tBufControllerData.eInputButton = eButton;
        InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );

        sSetButtonHeldStart( tControllerState, eButton );
        tControllerState.tButtonLastDownTimes[eButton] = Blizzard::Time::Milliseconds();
    }
    else
    {
        BufferedInput tInput;
        tInput.eInputMsg = (eTrigger == LEFT_TRIGGER) ? INPUTMSG_LEFTTRIGGER : INPUTMSG_RIGHTTRIGGER;
        tInput.tBufControllerData.nControllerIndex = nControllerIndex;
        int32 nLastFrameValue;
        if ( eTrigger == LEFT_TRIGGER )
        {
            tInput.tBufControllerData.nLeftTriggerPos = 0;
            nLastFrameValue = tControllerState.tPrevFrame.nLeftTrigger;
        }
        else
        {
            tInput.tBufControllerData.nRightTriggerPos = 0;
            nLastFrameValue = tControllerState.tPrevFrame.nRightTrigger;
        }
        if ( nLastFrameValue != 0 )
        {
            InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );

            tInput.eInputMsg = INPUTMSG_CONTROLLERBUTTONUP;
            tInput.tBufControllerData.eInputButton = eButton;
            InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );

            tControllerState.tButtonLastDownTimes[eButton] = 0;
            tControllerState.tButtonHeldDownTimes[eButton] = 0;
        }
    }
}




static void sSendTriggerEvents(
    ControllerState & tControllerState,
    int32 nControllerIndex )
{
    sHandleTriggerInput( tControllerState, nControllerIndex, LEFT_TRIGGER );
    sHandleTriggerInput( tControllerState, nControllerIndex, RIGHT_TRIGGER );
}




static void sProcessControllerInput(
    InputState & tInputState,
    int32 nControllerIndex )
{

    sSendStickEvents( tInputState.tControllerState[nControllerIndex], nControllerIndex );


    sSendButtonEvents( tInputState.tControllerState[nControllerIndex], nControllerIndex );


    sSendTriggerEvents( tInputState.tControllerState[nControllerIndex], nControllerIndex );
}




static void sProcessGestureInput(
    InputState & tInputState,
    int32 nControllerIndex )
{



    if ( tInputState.tControllerState[nControllerIndex].tCurrentFrame.tGestures.bFlick )
    {
        BufferedInput tInput;
        tInput.tBufControllerData.nControllerIndex = nControllerIndex;
        tInput.eInputMsg = INPUTMSG_GESTUREFLICK;
        InputBufferInput( tInput, INPUTCONTEXT_DEFAULT, FALSE );
    }
}




static void sUpdatePrevFrameControllerState(
    InputState & tInputState )
{
    for ( uint32 i = 0; i < MAX_CONTROLLERS; ++i )
    {
        tInputState.tControllerState[i].tPrevFrame = tInputState.tControllerState[i].tCurrentFrame;
    }
}




static void sProcessControllerInputs(
    InputState & tInputState )
{
    for ( int32 i = 0; i < MAX_CONTROLLERS; ++i )
    {
        sProcessControllerInput( tInputState, i );
    }
}




static void sProcessGestureInputs(
    InputState & tInputState )
{
    for ( int32 i = 0; i < MAX_CONTROLLERS; ++i )
    {
        sProcessGestureInput( tInputState, i );
    }
}

