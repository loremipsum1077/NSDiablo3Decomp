// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Input/Input.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Player.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"

enum {
    KEY_NONE = -1,
    KEY_ESCAPE = 0,
    KEY_1 = 1,
    KEY_2 = 2,
    KEY_3 = 3,
    KEY_4 = 4,
    KEY_5 = 5,
    KEY_6 = 6,
    KEY_7 = 7,
    KEY_8 = 8,
    KEY_9 = 9,
    KEY_0 = 10,
    KEY_MINUS = 11,
    KEY_EQUALS = 12,
    KEY_BACK = 13,
    KEY_TAB = 14,
    KEY_Q = 15,
    KEY_W = 16,
    KEY_E = 17,
    KEY_R = 18,
    KEY_T = 19,
    KEY_Y = 20,
    KEY_U = 21,
    KEY_I = 22,
    KEY_O = 23,
    KEY_P = 24,
    KEY_LBRACKET = 25,
    KEY_RBRACKET = 26,
    KEY_RETURN = 27,
    KEY_LCONTROL = 28,
    KEY_A = 29,
    KEY_S = 30,
    KEY_D = 31,
    KEY_F = 32,
    KEY_G = 33,
    KEY_H = 34,
    KEY_J = 35,
    KEY_K = 36,
    KEY_L = 37,
    KEY_SEMICOLON = 38,
    KEY_APOSTROPHE = 39,
    KEY_GRAVE = 40,
    KEY_LSHIFT = 41,
    KEY_BACKSLASH = 42,
    KEY_Z = 43,
    KEY_X = 44,
    KEY_C = 45,
    KEY_V = 46,
    KEY_B = 47,
    KEY_N = 48,
    KEY_M = 49,
    KEY_COMMA = 50,
    KEY_PERIOD = 51,
    KEY_SLASH = 52,
    KEY_RSHIFT = 53,
    KEY_MULTIPLY = 54,
    KEY_LMENU = 55,
    KEY_SPACE = 56,
    KEY_CAPITAL = 57,
    KEY_F1 = 58,
    KEY_F2 = 59,
    KEY_F3 = 60,
    KEY_F4 = 61,
    KEY_F5 = 62,
    KEY_F6 = 63,
    KEY_F7 = 64,
    KEY_F8 = 65,
    KEY_F9 = 66,
    KEY_F10 = 67,
    KEY_NUMLOCK = 68,
    KEY_SCROLL = 69,
    KEY_NUMPAD7 = 70,
    KEY_NUMPAD8 = 71,
    KEY_NUMPAD9 = 72,
    KEY_SUBTRACT = 73,
    KEY_NUMPAD4 = 74,
    KEY_NUMPAD5 = 75,
    KEY_NUMPAD6 = 76,
    KEY_ADD = 77,
    KEY_NUMPAD1 = 78,
    KEY_NUMPAD2 = 79,
    KEY_NUMPAD3 = 80,
    KEY_NUMPAD0 = 81,
    KEY_DECIMAL = 82,
    KEY_OEM_102 = 83,
    KEY_F11 = 84,
    KEY_F12 = 85,
    KEY_F13 = 86,
    KEY_F14 = 87,
    KEY_F15 = 88,
    KEY_KANA = 89,
    KEY_ABNT_C1 = 90,
    KEY_CONVERT = 91,
    KEY_NOCONVERT = 92,
    KEY_YEN = 93,
    KEY_ABNT_C2 = 94,
    KEY_NUMPADEQUALS = 95,
    KEY_PREVTRACK = 96,
    KEY_AT = 97,
    KEY_COLON = 98,
    KEY_UNDERLINE = 99,
    KEY_KANJI = 100,
    KEY_STOP = 101,
    KEY_AX = 102,
    KEY_UNLABELED = 103,
    KEY_NEXTTRACK = 104,
    KEY_NUMPADENTER = 105,
    KEY_RCONTROL = 106,
    KEY_MUTE = 107,
    KEY_CALCULATOR = 108,
    KEY_PLAYPAUSE = 109,
    KEY_MEDIASTOP = 110,
    KEY_VOLUMEDOWN = 111,
    KEY_VOLUMEUP = 112,
    KEY_WEBHOME = 113,
    KEY_NUMPADCOMMA = 114,
    KEY_DIVIDE = 115,
    KEY_SYSRQ = 116,
    KEY_RMENU = 117,
    KEY_PAUSE = 118,
    KEY_HOME = 119,
    KEY_UP = 120,
    KEY_PRIOR = 121,
    KEY_LEFT = 122,
    KEY_RIGHT = 123,
    KEY_END = 124,
    KEY_DOWN = 125,
    KEY_NEXT = 126,
    KEY_INSERT = 127,
    KEY_DELETE = 128,
    KEY_LWIN = 129,
    KEY_RWIN = 130,
    KEY_APPS = 131,
    KEY_POWER = 132,
    KEY_SLEEP = 133,
    KEY_WAKE = 134,
    KEY_WEBSEARCH = 135,
    KEY_WEBFAVORITES = 136,
    KEY_WEBREFRESH = 137,
    KEY_WEBSTOP = 138,
    KEY_WEBFORWARD = 139,
    KEY_WEBBACK = 140,
    KEY_MYCOMPUTER = 141,
    KEY_MAIL = 142,
    KEY_MEDIASELECT = 143,
    KEY_MOUSE1 = 144,
    KEY_MOUSE2 = 145,
    KEY_MOUSE3 = 146,
    KEY_MOUSE4 = 147,
    KEY_MOUSE5 = 148,
    KEY_MOUSEWHEELUP = 149,
    KEY_MOUSEWHEELDOWN = 150,
    KEY_F16 = 151,
    KEY_F17 = 152,
    KEY_F18 = 153,
    KEY_F19 = 154,
    KEY_OEM_8 = 155,
    NUM_KEYS = 156,
};

typedef int32 Key;

enum {
    NO_MOUSE_BUTTON = -1,
    LEFT_MOUSE_BUTTON = 0,
    MIDDLE_MOUSE_BUTTON = 1,
    RIGHT_MOUSE_BUTTON = 2,
    NUM_MOUSE_BUTTONS = 3,
};

typedef int32 MouseButton;

enum KeyModifier {
    KEY_MODIFIER_ALT = 0,
    KEY_MODIFIER_CTRL = 1,
    KEY_MODIFIER_SHIFT = 2,
};

enum {
    INPUTMSG_CHARACTER = 0,
    INPUTMSG_KEYDOWNONCE = 1,
    INPUTMSG_KEYDOWN = 2,
    INPUTMSG_KEYSTILLDOWN = 3,
    INPUTMSG_KEYUP = 4,
    INPUTMSG_MOUSEMOVE = 5,
    INPUTMSG_LEFTMOUSEDOWN = 6,
    INPUTMSG_LEFTMOUSEUP = 7,
    INPUTMSG_LEFTMOUSEDOUBLECLICK = 8,
    INPUTMSG_LEFTMOUSESTILLDOWN_NO_DELAY = 9,
    INPUTMSG_LEFTMOUSESTILLDOWN = 10,
    INPUTMSG_MIDDLEMOUSEDOWN = 11,
    INPUTMSG_MIDDLEMOUSEUP = 12,
    INPUTMSG_MIDDLEMOUSEDOUBLECLICK = 13,
    INPUTMSG_MIDDLEMOUSESTILLDOWN_NO_DELAY = 14,
    INPUTMSG_MIDDLEMOUSESTILLDOWN = 15,
    INPUTMSG_RIGHTMOUSEDOWN = 16,
    INPUTMSG_RIGHTMOUSEUP = 17,
    INPUTMSG_RIGHTMOUSEDOUBLECLICK = 18,
    INPUTMSG_RIGHTMOUSESTILLDOWN_NO_DELAY = 19,
    INPUTMSG_RIGHTMOUSESTILLDOWN = 20,
    INPUTMSG_BUTTONDOWNONCE = 21,
    INPUTMSG_BUTTONDOWN = 22,
    INPUTMSG_BUTTONUP = 23,
    INPUTMSG_MOUSEWHEEL = 24,
    INPUTMSG_CONTROLLER_START = 25,
    INPUTMSG_LEFTSTICK = 25,
    INPUTMSG_RIGHTSTICK = 26,
    INPUTMSG_LEFTTRIGGER = 27,
    INPUTMSG_RIGHTTRIGGER = 28,
    INPUTMSG_CONTROLLERBUTTONDOWN = 29,
    INPUTMSG_CONTROLLERBUTTONSTILLDOWN = 30,
    INPUTMSG_CONTROLLERBUTTONHELDDOWN = 31,
    INPUTMSG_CONTROLLERBUTTONREPEAT = 32,
    INPUTMSG_CONTROLLERBUTTONUP = 33,
    INPUTMSG_CONTROLLER_END = 33,
    INPUTMSG_GESTURE_START = 34,
    INPUTMSG_GESTUREFLICK = 34,
    INPUTMSG_GESTURE_END = 34,
    INPUTMSG_IME_START_COMPOSITION = 35,
    INPUTMSG_IME_END_COMPOSITION = 36,
    INPUTMSG_IME_SHOW_COMPOSITION = 37,
    INPUTMSG_IME_INSERT_RESULT = 38,
    INPUTMSG_IME_LANGUAGE_CHANGE = 39,
    INPUTMSG_IME_OPEN_CANDIDATES = 40,
    INPUTMSG_IME_CHANGE_CANDIDATES = 41,
    INPUTMSG_IME_CLOSE_CANDIDATES = 42,
    INPUTMSG_IME_SHOWREADING = 43,
    INPUTMSG_IME_HIDEREADING = 44,
    INPUTMSG_IME_LAST = 45,
    NUM_INPUTMSG_TYPES = 46,
};

typedef int32 InputMsg;

enum {
    MOUSESTATE_LEFT_BUTTON_DOWN_BIT = 0,
    MOUSESTATE_MIDDLE_BUTTON_DOWN_BIT = 1,
    MOUSESTATE_RIGHT_BUTTON_DOWN_BIT = 2,
};

enum {
    INPUT_KEYBOARD_VALID_BIT = 0,
    INPUT_MOUSE_VALID_BIT = 1,
    INPUT_CONTROLLER0_VALID_BIT = 2,
    INPUT_CONTROLLER1_VALID_BIT = 3,
    INPUT_CONTROLLER2_VALID_BIT = 4,
    INPUT_CONTROLLER3_VALID_BIT = 5,
};

enum CursorTypes {
    CURSOR_EDITSELECT = 0,
    CURSOR_EDITSELECT_CYCLE = 1,
    CURSOR_EDITSELECT_CYCLE_VARIATION_1 = 2,
    CURSOR_EDITSELECT_CYCLE_VARIATION_2 = 3,
    CURSOR_EDITSELECT_ADD = 4,
    CURSOR_EDIT_AXIS_X = 5,
    CURSOR_EDIT_AXIS_Y = 6,
    CURSOR_EDIT_AXIS_Z = 7,
    CURSOR_EDIT_AXES_XY = 8,
    CURSOR_EDIT_AXES_XZ = 9,
    CURSOR_EDIT_AXES_YZ = 10,
    CURSOR_EDIT_AXES_XYZ = 11,
    CURSOR_EDITROTATE = 12,
    CURSOR_EDITSCALE = 13,
    CURSOR_LIGHT = 14,
    CURSOR_LIGHTDISTANCE = 15,
    CURSOR_CANTWALK = 16,
    CURSOR_INVENTORY = 17,
    CURSOR_MEASURING_TAPE_ADD = 18,
    CURSOR_MEASURING_TAPE_SUBTRACT = 19,
};

typedef int32 InputButton;

enum {
    MAX_INPUT_LISTENERS = 64,
    MAX_IME_COMPOSITION_TEXT_LEN = 64,
};

enum InputContext {
    INPUTCONTEXT_DEFAULT = 0,
    INPUTCONTEXT_HOLODECK = 1,
    NUM_INPUTCONTEXTS = 2,
};

struct KeyboardState {  // size 0x30c
    uint8 bCurrentKeys[156];  // 0x00
    uint32 dwClickTime[156];  // 0x9c
};
static_assert(sizeof(KeyboardState) == 0x30c, "KeyboardState");

struct MouseState {  // size 0x20
    uint32 dwClickTime[3];  // 0x00
    uint32 dwFlags;  // 0x0c
    IVector2D p;  // 0x10
    int32 nMouseWheelPos;  // 0x18
    int32 nMouseWheelDeltaSinceLastUpdate;  // 0x1c
};
static_assert(sizeof(MouseState) == 0x20, "MouseState");

struct TouchState {  // size 0x8
    uint16 uX;  // 0x00
    uint16 uY;  // 0x02
    uint32 uID;  // 0x04
    TouchState() : uX(0), uY(0), uID(0) {}  // inline; body from the Input.cpp static initialiser
};
static_assert(sizeof(TouchState) == 0x8, "TouchState");

struct GestureState {  // size 0x4
    union {  // size 0x4
        bool bFlick : 1;  // 0x00
        uint32 nPadding;  // 0x00
    };  // 0x00
};
static_assert(sizeof(GestureState) == 0x4, "GestureState");

struct ControllerFrameState {  // size 0x38
    BOOL bIsConnected;  // 0x00
    uint32 uButtons;  // 0x04
    int32 nLeftTrigger;  // 0x08
    int32 nRightTrigger;  // 0x0c
    Vector2D vecLeftStick;  // 0x10
    Vector2D vecRightStick;  // 0x18
    int32 nNumTouches;  // 0x20
    TouchState arTouches[2];  // 0x24
    GestureState tGestures;  // 0x34
    ControllerFrameState();
    BOOL IsButtonDown(InputButton eButton) const;
};
static_assert(sizeof(ControllerFrameState) == 0x38, "ControllerFrameState");

struct ControllerState {  // size 0x10c
    ControllerFrameState tPrevFrame;  // 0x00
    ControllerFrameState tCurrentFrame;  // 0x38
    Blizzard::Time::Millisecond tButtonLastDownTimes[18];  // 0x70
    Blizzard::Time::Millisecond tButtonHeldDownTimes[18];  // 0xb8
    Blizzard::Time::Millisecond tLastHeldDownCheckTime;  // 0x100
    Blizzard::Time::Millisecond tLStickLastExtremeTime;  // 0x104
    Blizzard::Time::Millisecond tRStickLastExtremeTime;  // 0x108
    ControllerState();
};
static_assert(sizeof(ControllerState) == 0x10c, "ControllerState");

struct InputState {  // size 0x760
    uint32 dwFlags;  // 0x00
    KeyboardState tKeyboardState;  // 0x04
    MouseState tMouseState;  // 0x310
    ControllerState tControllerState[4];  // 0x330
};
static_assert(sizeof(InputState) == 0x760, "InputState");

struct BufferedCharData {  // size 0x6
    CHAR szUTF8[6];  // 0x00
};
static_assert(sizeof(BufferedCharData) == 0x6, "BufferedCharData");

struct BufferedKeyData {  // size 0xc
    uint16 wPlatformKey;  // 0x00
    Key eKey;  // 0x04
    BOOL bExactMatch;  // 0x08
};
static_assert(sizeof(BufferedKeyData) == 0xc, "BufferedKeyData");

struct BufferedMouseData {  // size 0x10
    IVector2D p;  // 0x00
    int32 nDeltaMouseWheel;  // 0x08
    MouseButton eMouseButton;  // 0x0c
};
static_assert(sizeof(BufferedMouseData) == 0x10, "BufferedMouseData");

struct BufferedControllerData {  // size 0x20
    BufferedControllerData();
    int32 nControllerIndex;  // 0x00
    InputButton eInputButton;  // 0x04
    Vector2D vLeftStick;  // 0x08
    Vector2D vRightStick;  // 0x10
    int32 nRightTriggerPos;  // 0x18
    int32 nLeftTriggerPos;  // 0x1c
};
static_assert(sizeof(BufferedControllerData) == 0x20, "BufferedControllerData");

struct BufferedInput {  // size 0x48
    BufferedInput();
    BufferedInput(InputMsg);
    InputMsg eInputMsg;  // 0x00
    BufferedCharData tBufCharData;  // 0x04
    BufferedKeyData tBufKeyData;  // 0x0c
    BufferedMouseData tBufMouseData;  // 0x18
    BufferedControllerData tBufControllerData;  // 0x28
};
static_assert(sizeof(BufferedInput) == 0x48, "BufferedInput");

typedef BOOL (*InputListenerFunc)(const BufferedInput &);

// X/Client/Input/Input.cpp
const InputState & InputGetCurrentState(InputContext eContext);
BOOL InputCtrlKeyIsDown(InputContext eContext);  // Input.cpp:785


// Functions defined in this file (bodies are not in the DWARF):
//   line   390  ControllerFrameState & ControllerFrameState::operator=(const ControllerFrameState &)  [inlined into 1 functions]
//   line   431  InputState::InputState()  [inline]

// Variables defined in this file:
//   line    21  static const uint32 MAX_CONTROLLERS
//   line   388  static const int32 MAX_TRIGGER_VALUE

// Input.cpp (X/Common/Game.cpp AppIdle; X/Client/CGame.cpp declares these locally too).
BOOL InputIsCursorInWindow();
BOOL InputAppHasCapture();
void InputCursorSet(CursorTypes eCursorType);
