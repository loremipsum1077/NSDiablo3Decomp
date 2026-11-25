// Original file: X/Client/Definitions/UIDialogDefinition.cpp

#include "X/Client/Definitions/UIDialogDefinition.h"

#include "../Gfx/GfxCommon.h"
#include "../ImageCollection.h"
#include "../UserInterface/UIAttributes.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/Unicode.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include <stdlib.h>
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

uint32 atoh(LPCSTR sz);  // X/Sigma/Hex.cpp

// X/Client/UserInterface/UI.cpp; UI.h is not included here.
UIStyle * UI_GetStyleDataClone(LPCSTR szStyleName, SNO snoImpliedFile, const UIStyleMap * pmapStylesFromImpliedSno);
void UI_SetupStyles(UIControlData & pControl, const UIControlDataArray * pChildren, SNO snoImpliedFile, const UIStyleMap * pmapStylesFromImpliedSno);

// Inline helpers defined at the end of this file (lines 2582 and 2608).
inline int32 UI_ParseAttributeInt32(UIDataType eDataType, LPCSTR szValue);
inline UIDataType UI_GetAttributeDataType(const UIAttributeType eAttributeType);

#line 35
enum {
    UI_XML_VERSION = 1,
    UI_COMMON_SNO_VERSION = 471,
    UI_PC_SNO_VERSION = 471,
    UI_CONSOLE_SNO_VERSION = 479,
    UI_AXE_SNO_VERSION = 472,
};

struct AttributeInfo {
    UIDataType eDataType;
    LPCSTR szDefaultValue;
};

#line 53
static const AttributeInfo arAttributeInfos[ATT_COUNT] = {
    { UIDT_INT32, "ASSERT" },  // ATT_TYPE
    { UIDT_STRING, "0" },  // ATT_TEMPLATE
    { UIDT_BOOL, "1" },  // ATT_ACTIVE
    { UIDT_INT32, "0" },  // ATT_L_ABS
    { UIDT_INT32, "0" },  // ATT_T_ABS
    { UIDT_INT32, "0" },  // ATT_R_ABS
    { UIDT_INT32, "0" },  // ATT_B_ABS
    { UIDT_INT32, "0" },  // ATT_L
    { UIDT_INT32, "0" },  // ATT_T
    { UIDT_INT32, "0" },  // ATT_WIDTH
    { UIDT_INT32, "0" },  // ATT_HEIGHT
    { UIDT_INT32, "-1" },  // ATT_MAX_WIDTH
    { UIDT_INT32, "-1" },  // ATT_MAX_HEIGHT
    { UIDT_INT32, "-1" },  // ATT_MIN_WIDTH
    { UIDT_INT32, "-1" },  // ATT_MIN_HEIGHT
    { UIDT_INT32, "255" },  // ATT_ALPHA
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_MOUSEOVER
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_PRESSED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_DISABLED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_ICON
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_ICON_MOUSEOVER
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_ICON_PRESSED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_ICON_DISABLED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_CHECKED_NORMAL
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_CHECKED_MOUSEOVER
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_CHECKED_DISABLED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_UNCHECKED_NORMAL
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_UNCHECKED_MOUSEOVER
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_UNCHECKED_DISABLED
    { UIDT_STRING, "" },  // ATT_IMAGEFRAME_FLASH
    { UIDT_STRING, "-1" },  // ATT_BG_ANIMSNO
    { UIDT_STRING, "" },  // ATT_MASKIMAGE
    { UIDT_INT32, "0" },  // ATT_BUTTON_ICON_W
    { UIDT_INT32, "0" },  // ATT_BUTTON_ICON_H
    { UIDT_INT32, "0" },  // ATT_BUTTON_ICON_ALIGNMENT
    { UIDT_INT32, "0" },  // ATT_BUTTON_ICON_TEXT_PADDING_X
    { UIDT_STRING, "-1" },  // ATT_BUTTON_ANIMSNO_NORMAL
    { UIDT_STRING, "-1" },  // ATT_BUTTON_ANIMSNO_MOUSEOVER
    { UIDT_STRING, "-1" },  // ATT_BUTTON_ANIMSNO_PRESSED
    { UIDT_STRING, "-1" },  // ATT_BUTTON_ANIMSNO_DISABLED
    { UIDT_STRING, "-1" },  // ATT_ONCLICK
    { UIDT_STRING, "-1" },  // ATT_CLICKPREDICATE
    { UIDT_STRING, "-1" },  // ATT_ONDOUBLECLICK
    { UIDT_STRING, "-1" },  // ATT_DRAWHOOK
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_INSET_X
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_INSET_Y
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_PADDING_L
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_PADDING_T
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_PADDING_R
    { UIDT_FLOAT32, "0" },  // ATT_TEXT_PADDING_B
    { UIDT_UICOLOR, "0xFFCD9D51" },  // ATT_FOREGROUND
    { UIDT_UICOLOR, "0xFF000000" },  // ATT_DROPSHADOW
    { UIDT_UICOLOR, "0xFF7F7F7F" },  // ATT_FOREGROUND_DISABLED
    { UIDT_UICOLOR, "0xFFFFFFFF" },  // ATT_FOREGROUND_HIGHLIGHT
    { UIDT_UICOLOR, "0XFFFF691E" },  // ATT_DROPSHADOW_HIGHLIGHT
    { UIDT_UICOLOR, "0XFFFFFFFF" },  // ATT_FOREGROUND_PUSHED
    { UIDT_UICOLOR, "0XFFFF691E" },  // ATT_DROPSHADOW_PUSHED
    { UIDT_STRING, "ExocetLight" },  // ATT_FONT
    { UIDT_INT32, "24" },  // ATT_FONT_SIZE
    { UIDT_INT32, "0" },  // ATT_FONT_SIZE_FALLBACK
    { UIDT_INT32, "" },  // ATT_TEXTJUSTIFY
    { UIDT_INT32, "" },  // ATT_TEXTJUSTIFY_VERTICAL
    { UIDT_FLOAT32, "1" },  // ATT_TEXT_ADVANCE_SCALAR
    { UIDT_BOOL, "0" },  // ATT_TEXT_ALL_CAPS
    { UIDT_STRING, "" },  // ATT_TEXT
    { UIDT_INT32, "0" },  // ATT_TEXT_LINES_BEFORE_TRUNCATION
    { UIDT_STRING, "-1" },  // ATT_TEXTCURSOR_ANIMSNO
    { UIDT_STRING, "-1" },  // ATT_TEXTSELECTION_ANIMSNO
    { UIDT_INT32, "-1" },  // ATT_HOTKEY
    { UIDT_INT32, "-1" },  // ATT_HOTKEY_ALT
    { UIDT_STRING, "-1" },  // ATT_ONKEY
    { UIDT_STRING, "-1" },  // ATT_ONCONTROLLER
    { UIDT_BOOL, "0" },  // ATT_WIN_DRAGGABLE
    { UIDT_BOOL, "0" },  // ATT_WIN_IGNORE_CLIPPING
    { UIDT_BOOL, "0" },  // ATT_DRAGWIN_CONSTRAIN_VERTICAL
    { UIDT_BOOL, "0" },  // ATT_DRAGWIN_CONSTRAIN_HORIZONTAL
    { UIDT_STRING, "-1" },  // ATT_ON_DRAG_START
    { UIDT_STRING, "-1" },  // ATT_ON_DRAG
    { UIDT_FLOAT32, "0" },  // ATT_BUTTON_REPEAT
    { UIDT_BOOL, "0" },  // ATT_BUTTON_DO_ACTION_ON_MOUSEDOWN
    { UIDT_INT32, "0" },  // ATT_BUTTON_STYLE
    { UIDT_UICOLOR, "0XFF888888" },  // ATT_BUTTON_DISABLED_COLOR
    { UIDT_FLOAT32, "0" },  // ATT_PROGRESS_MIN
    { UIDT_FLOAT32, "0" },  // ATT_PROGRESS_MAX
    { UIDT_BOOL, "0" },  // ATT_VERTICAL
    { UIDT_BOOL, "0" },  // ATT_PROGRESS_REVERSED
    { UIDT_STRING, "-1" },  // ATT_PROGRESS_LEFT_ENDCAP
    { UIDT_STRING, "-1" },  // ATT_PROGRESS_RIGHT_ENDCAP
    { UIDT_STRING, "-1" },  // ATT_PROGRESS_FOREGROUND
    { UIDT_STRING, "-1" },  // ATT_PROGRESS_TARGET
    { UIDT_STRING, "" },  // ATT_PROGRESS_FOREGROUND_IMAGE
    { UIDT_STRING, "" },  // ATT_PROGRESS_BACKGROUND_IMAGE
    { UIDT_STRING, "-1" },  // ATT_HIGHLIGHT_ANIMSNO
    { UIDT_STRING, "-1" },  // ATT_HOVER_ANIMSNO
    { UIDT_STRING, "-1" },  // ATT_ONINIT
    { UIDT_STRING, "-1" },  // ATT_ONOPEN
    { UIDT_STRING, "-1" },  // ATT_ONCLOSE
    { UIDT_STRING, "-1" },  // ATT_ONDISPOSE
    { UIDT_STRING, "-1" },  // ATT_ONUPDATE
    { UIDT_STRING, "-1" },  // ATT_ONCHANGE
    { UIDT_STRING, "-1" },  // ATT_ONSELCHANGE
    { UIDT_INT32, "-1" },  // ATT_WINDOW_EATS_CLICKS
    { UIDT_BOOL, "0" },  // ATT_WINDOW_IS_INPUT_FOCUS_GROUP
    { UIDT_INT32, "-1" },  // ATT_INPUT_FOCUS_ORDER
    { UIDT_STRING, "UI_TIP_NULL" },  // ATT_TOOLTIP
    { UIDT_INT32, "0" },  // ATT_TOOLTIP_OFFSET_X
    { UIDT_INT32, "0" },  // ATT_TOOLTIP_OFFSET_Y
    { UIDT_INT32, "-1" },  // ATT_TOOLTIP_ALIGN_HORIZONTAL
    { UIDT_INT32, "-1" },  // ATT_TOOLTIP_ALIGN_VERTICAL
    { UIDT_STRING, "" },  // ATT_TOOLTIP_TEXT
    { UIDT_INT32, "Center" },  // ATT_TOOLTIP_TEXT_JUSTIFY
    { UIDT_STRING, "-1" },  // ATT_ONRIGHTCLICK
    { UIDT_STRING, "-1" },  // ATT_ONMOUSEMOVE
    { UIDT_BOOL, "0" },  // ATT_WORDWRAP
    { UIDT_BOOL, "0" },  // ATT_SHRINK_TO_FIT
    { UIDT_INT32, "-1" },  // ATT_RESIZE_TO_FIT
    { UIDT_BOOL, "0" },  // ATT_HYPERLINKAWARE
    { UIDT_INT32, "-1" },  // ATT_TOOLTIP_WIDTH
    { UIDT_BOOL, "0" },  // ATT_IME_DISABLED
    { UIDT_BOOL, "0" },  // ATT_NUMBERS_ONLY
    { UIDT_BOOL, "0" },  // ATT_NUMBER_FORMATTING_ENABLED
    { UIDT_INT32, "0" },  // ATT_NUM_DECIMAL_PLACES
    { UIDT_STRING, "-1" },  // ATT_ONRETURNENDEDIT
    { UIDT_STRING, "-1" },  // ATT_ONGAINEDFOCUS
    { UIDT_STRING, "-1" },  // ATT_ONLOSTFOCUS
    { UIDT_BOOL, "0" },  // ATT_ACCEPTSFOCUS
    { UIDT_BOOL, "1" },  // ATT_ACCEPT_INITIAL_FOCUS
    { UIDT_STRING, "-1" },  // ATT_ONLBUTTONSTILLDOWN
    { UIDT_INT32, "3" },  // ATT_BUTTON_PRESS_HORIZONTAL
    { UIDT_INT32, "3" },  // ATT_BUTTON_PRESS_VERTICAL
    { UIDT_STRING, "-1" },  // ATT_ID_GENERIC_01
    { UIDT_STRING, "-1" },  // ATT_ID_GENERIC_02
    { UIDT_STRING, "-1" },  // ATT_ID_GENERIC_03
    { UIDT_INT32, "-1" },  // ATT_STYLE
    { UIDT_INT32, "5" },  // ATT_NUM_COMBOBOX_ITEMS
    { UIDT_STRING, "" },  // ATT_COMBOBOX_PLACEHOLDER_TEXT
    { UIDT_STRING, "-1" },  // ATT_ID_NEXT
    { UIDT_INT32, "-1" },  // ATT_TILED_STYLE
    { UIDT_STRING, "-1" },  // ATT_SHADER
    { UIDT_STRING, "" },  // ATT_ANCHOR_POINT
    { UIDT_STRING, "" },  // ATT_VERTICAL_ANCHOR_POINT
    { UIDT_BOOL, "0" },  // ATT_PASSWORD
    { UIDT_BOOL, "0" },  // ATT_SELECTABLE
    { UIDT_INT32, "0" },  // ATT_RADIOBUTTON_VALUE
    { UIDT_BOOL, "0" },  // ATT_RADIOBUTTON_DEFAULT
    { UIDT_STRING, "" },  // ATT_RADIOBUTTON_IMAGEFRAME
    { UIDT_STRING, "" },  // ATT_RADIOBUTTON_IMAGEFRAME_MOUSEOVER
    { UIDT_STRING, "" },  // ATT_RADIOBUTTON_IMAGEFRAME_PRESSED
    { UIDT_STRING, "" },  // ATT_RADIOBUTTON_IMAGEFRAME_DISABLED
    { UIDT_INT32, "-1" },  // ATT_RENDERLAYER_OVERRIDE
    { UIDT_BOOL, "0" },  // ATT_CLEAR_HIGHLIGHT_LAYER
    { UIDT_BOOL, "0" },  // ATT_USE_DROP_SHADOW
    { UIDT_BOOL, "0" },  // ATT_HORIZONTAL
    { UIDT_BOOL, "0" },  // ATT_REVERSE_ORDER
    { UIDT_INT32, "0" },  // ATT_STACK_SPACING
    { UIDT_STRING, "LT" },  // ATT_ANCHOREDGES
    { UIDT_STRING, "" },  // ATT_XML_STYLE
    { UIDT_STRING, "" },  // ATT_XML_TEMPLATE
    { UIDT_STRING, "" },  // ATT_CONTEXTMENU
    { UIDT_STRING, "" },  // ATT_TARGETPARENT_CONTEXTMENU
    { UIDT_STRING, "-1" },  // ATT_ONCONTEXTMENU
    { UIDT_BOOL, "0" },  // ATT_CONTEXTMENU_LEFTCLICK
    { UIDT_BOOL, "0" },  // ATT_CONTEXTMENU_TOGGLE
    { UIDT_STRING, "0,0,0,0" },  // ATT_MARGINS_TRBL
    { UIDT_BOOL, "0" },  // ATT_ROLLEDUP
    { UIDT_BOOL, "0" },  // ATT_CENTER_IN_STACK
    { UIDT_BOOL, "0" },  // ATT_DIALOGBOX_RESIZABLE
    { UIDT_BOOL, "0" },  // ATT_DIALOGBOX_TITLE_BAR
    { UIDT_INT32, "0" },  // ATT_DIALOGBOX_MINWIDTH
    { UIDT_INT32, "0" },  // ATT_DIALOGBOX_MINHEIGHT
    { UIDT_BOOL, "0" },  // ATT_ENCLOSE_CHILDREN
    { UIDT_STRING, "-1" },  // ATT_ACTOR
    { UIDT_STRING, "-1" },  // ATT_OBSERVER
    { UIDT_STRING, "-1" },  // ATT_LIGHT
    { UIDT_STRING, "-1" },  // ATT_CLICK_SOUND
    { UIDT_STRING, "-1" },  // ATT_RIGHT_CLICK_SOUND
    { UIDT_STRING, "-1" },  // ATT_MOUSEOVER_SOUND
    { UIDT_STRING, "-1" },  // ATT_ONOPEN_SOUND
    { UIDT_STRING, "-1" },  // ATT_ONCLOSE_SOUND
    { UIDT_INT32, "0" },  // ATT_RENDERCONTEXT
    { UIDT_BOOL, "0" },  // ATT_CLEAR_Z_BUFFER
    { UIDT_BOOL, "0" },  // ATT_ROTATION_ENABLED
    { UIDT_BOOL, "0" },  // ATT_STATIC_ACTOR
    { UIDT_FLOAT32, "0" },  // ATT_DURATION
    { UIDT_STRING, "-1" },  // ATT_PLAYBACK_SOUND
    { UIDT_STRING, "-1" },  // ATT_ONEND
    { UIDT_STRING, "" },  // ATT_LISTITEM_TEMPLATE
    { UIDT_BOOL, "0" },  // ATT_VIRTUALSCROLLSPACE_ENABLED
    { UIDT_BOOL, "0" },  // ATT_TOGGLE_SELECTION
    { UIDT_INT32, "0" },  // ATT_TILELIST_SPACING
    { UIDT_STRING, "-1" },  // ATT_ID_SCROLLBAR
    { UIDT_INT32, "0" },  // ATT_TREE_FOLDER_INDENT
    { UIDT_INT32, "0" },  // ATT_TREE_FOLDER_SPACING
    { UIDT_BOOL, "1" },  // ATT_CHATLIST_TEXT_FADE_ENABLED
    { UIDT_STRING, "" },  // ATT_CHATLIST_PLAYERCONTEXTMENU
    { UIDT_STRING, "" },  // ATT_CHATLIST_ONPLAYERCONTEXTMENU
    { UIDT_BOOL, "0" },  // ATT_IS_DRAG_TARGET
    { UIDT_INT32, "0" },  // ATT_SCROLLBAR_VISIBILITY
    { UIDT_INT32, "-1" },  // ATT_CHATEDIT_TITLE_MAXWIDTH
    { UIDT_BOOL, "0" },  // ATT_BLINK_FOREVER
    { UIDT_FLOAT32, "0" },  // ATT_BLINKER_MIN_ALPHA
    { UIDT_FLOAT32, "0" },  // ATT_BLINKER_ROTATION_SPEED
    { UIDT_FLOAT32, "1.0" },  // ATT_BLINKER_ALPHA_SPEED
    { UIDT_STRING, "" },  // ATT_EDIT_PLACEHOLDER_TEXT_LABEL
    { UIDT_UICOLOR, "0xFF7F7F7F" },  // ATT_EDIT_PLACEHOLDER_TEXT_FOREGROUND
    { UIDT_UICOLOR, "0xFF000000" },  // ATT_EDIT_PLACEHOLDER_TEXT_DROPSHADOW
    { UIDT_INT32, "512" },  // ATT_MAX_EDIT_CHARS
    { UIDT_INT32, "-1" },  // ATT_MAX_EDIT_BYTES
    { UIDT_INT32, "0" },  // ATT_CENTER_ON_PARENT
    { UIDT_FLOAT32, "0" },  // ATT_ROTATION
    { UIDT_BOOL, "0" },  // ATT_MIRRORX
    { UIDT_BOOL, "0" },  // ATT_MIRRORY
    { UIDT_INT32, "0" },  // ATT_HSPACING
    { UIDT_INT32, "0" },  // ATT_VSPACING
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL1
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL2
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL3
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR1
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR2
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR3
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON1
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON2
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON3
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON4
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADUP
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADRIGHT
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADLEFT
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADDOWN
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBACK
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONSTART
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONTOUCHPADBUTTON
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONOPTIONSBUTTON
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_LSTICKHANDLER
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_RSTICKHANDLER
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL1_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL2_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONL3_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR1_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR2_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONR3_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON1_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON2_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON3_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBUTTON4_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADUP_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADRIGHT_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADLEFT_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONDPADDOWN_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONBACK_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONSTART_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONTOUCHPADBUTTON_HELD
    { UIDT_STRING, "UI_CONSOLE_NULL" },  // ATT_ONOPTIONSBUTTON_HELD
    { UIDT_BOOL, "1" },  // ATT_FULLSCREEN_ELEMENT
    { UIDT_FLOAT32, "2" },  // ATT_SCROLLER_INITIAL_DELAY
    { UIDT_FLOAT32, "2" },  // ATT_SCROLLER_END_DELAY
    { UIDT_FLOAT32, "1" },  // ATT_SCROLLER_RATE
    { UIDT_BOOL, "0" },  // ATT_SCROLLER_HORIZONTAL
    { UIDT_BOOL, "0" },  // ATT_SCROLLER_REPEAT
    { UIDT_BOOL, "0" },  // ATT_REQUIRES_CONTROLLER_FOCUS
    { UIDT_BOOL, "0" },  // ATT_ONLY_SCROLL_WITH_DPAD
    { UIDT_BOOL, "0" },  // ATT_SCROLL_WITH_RSTICK
    { UIDT_INT32, "200" },  // ATT_STICK_SPEED_LIMIT
    { UIDT_BOOL, "0" },  // ATT_PRIMARY_CONTROLLER_ONLY
    { UIDT_BOOL, "0" },  // ATT_PC_ONLY
    { UIDT_FLOAT32, "0" },  // ATT_SCROLLINDICATOR_PADDING_TOP
    { UIDT_FLOAT32, "0" },  // ATT_SCROLLINDICATOR_PADDING_LEFT
    { UIDT_FLOAT32, "0" },  // ATT_SCROLLINDICATOR_PADDING_RIGHT
    { UIDT_FLOAT32, "0" },  // ATT_SCROLLINDICATOR_PADDING_BOTTOM
    { UIDT_STRING, "" },  // ATT_SCROLLDIMMER_TOP
    { UIDT_STRING, "" },  // ATT_SCROLLDIMMER_BOTTOM
    { UIDT_BOOL, "0" },  // ATT_HIDE_STACK_COUNT
    { UIDT_INT32, "PC" },  // ATT_RESOLUTION
    { UIDT_BOOL, "1" },  // ATT_DRAW_BACKGROUND
    { UIDT_BOOL, "1" },  // ATT_DRAW_SOCKETS
    { UIDT_INT32, "0" },  // ATT_STANDARD_DEF_OFFSET_X
    { UIDT_INT32, "0" },  // ATT_STANDARD_DEF_OFFSET_Y
    { UIDT_BOOL, "0" },  // ATT_ACCEPTS_RETURN
    { UIDT_BOOL, "1" },  // ATT_FORCE_LARGE_BACKGROUND
    { UIDT_STRING, "" },  // ATT_ITEM_NAME
    { UIDT_INT32, "0" },  // ATT_ITEM_STACK_COUNT
    { UIDT_BOOL, "0" },  // ATT_ITEM_DRAW_STACK_COUNT
    { UIDT_INT32, "0" },  // ATT_LIST_STYLE
    { UIDT_BOOL, "0" },  // ATT_SUPPORTS_TOUCH
};

// Lines 415-532: the type descriptors (_GLOBAL__sub_I_UIDialogDefinition.cpp). Generated with
// build/try/solo/desc.py from the original static initializer; the field names are "" in the binary.
#line 415
static const TypeDescriptorField UIControlElement_Fields[4] = {
    TypeDescriptorField("", DT_INT64, offsetof(UIControlElement, nKey), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIControlElement, serAttrib_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIControlElement, szAttrib), 0x700020, -8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIControlElement), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIControlElement("UIControlElement", 0, UIControlElement_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UICONTROLELEMENT = &TypeDescriptor_UIControlElement;

static const TypeDescriptorField UIDataElementInt32_Fields[3] = {
    TypeDescriptorField("", DT_INT64, offsetof(UIDataElementInt32, nKey), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UIDataElementInt32, nValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIDataElementInt32), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIDataElementInt32("UIDataElementInt32", 0, UIDataElementInt32_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIDATAELEMENTINT32 = &TypeDescriptor_UIDataElementInt32;

static const TypeDescriptorField UIDataElementFloat32_Fields[3] = {
    TypeDescriptorField("", DT_INT64, offsetof(UIDataElementFloat32, nKey), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(UIDataElementFloat32, flValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIDataElementFloat32), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIDataElementFloat32("UIDataElementFloat32", 0, UIDataElementFloat32_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIDATAELEMENTFLOAT32 = &TypeDescriptor_UIDataElementFloat32;

static const TypeDescriptorField DataElements_Fields[10] = {
    TypeDescriptorField("", DT_INT, offsetof(DataElements, dwNumStringElements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DataElements, serStringElements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UICONTROLELEMENT, offsetof(DataElements, ptStringElements), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(DataElements, dwNumInt32Elements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DataElements, serInt32Elements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIDATAELEMENTINT32, offsetof(DataElements, ptInt32Elements), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(DataElements, dwNumFloat32Elements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(DataElements, serFloat32Elements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIDATAELEMENTFLOAT32, offsetof(DataElements, ptFloat32Elements), 0x20, -0xc),
    TypeDescriptorField(NULL, DT_NULL, sizeof(DataElements), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_DataElements("DataElements", 0, DataElements_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_DATAELEMENTS = &TypeDescriptor_DataElements;

static const TypeDescriptorField UIStyleChildSpec_Fields[4] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIStyleChildSpec, serPathToChild_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIStyleChildSpec, szPathToChild), 0x700020, -8),
    TypeDescriptorField("", DT_DATAELEMENTS, offsetof(UIStyleChildSpec, tAttributes), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIStyleChildSpec), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIStyleChildSpec("UIStyleChildSpec", 0, UIStyleChildSpec_Fields, 4, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UISTYLECHILDSPEC = &TypeDescriptor_UIStyleChildSpec;

static const TypeDescriptorField UIStyle_Fields[8] = {
    TypeDescriptorField("", DT_INT64, offsetof(UIStyle, nKey), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIStyle, serName_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIStyle, szName), 0x700020, -8),
    TypeDescriptorField("", DT_DATAELEMENTS, offsetof(UIStyle, tAttributes), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIStyle, serChildSpecs), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UISTYLECHILDSPEC, offsetof(UIStyle, ptChildSpecs), 0x20, -0xc),
    VariableArrayCountTypeDescriptorField("", offsetof(UIStyle, dwNumChildSpecs), 0x10000, DT_UISTYLECHILDSPEC, 4),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIStyle), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIStyle("UIStyle", 0, UIStyle_Fields, 8, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UISTYLE = &TypeDescriptor_UIStyle;

static const TypeDescriptorField UIAnimationElementInt32_Fields[3] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(UIAnimationElementInt32, flT), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElementInt32, nValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIAnimationElementInt32), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIAnimationElementInt32("UIAnimationElementInt32", 0, UIAnimationElementInt32_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIANIMATIONELEMENTINT32 = &TypeDescriptor_UIAnimationElementInt32;

static const TypeDescriptorField UIAnimationElementFloat_Fields[3] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(UIAnimationElementFloat, flT), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(UIAnimationElementFloat, flValue), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIAnimationElementFloat), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIAnimationElementFloat("UIAnimationElementFloat", 0, UIAnimationElementFloat_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIANIMATIONELEMENTFLOAT = &TypeDescriptor_UIAnimationElementFloat;

static const TypeDescriptorField UIAnimationElementVector2D_Fields[3] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(UIAnimationElementVector2D, flT), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_FLOAT, offsetof(UIAnimationElementVector2D, flVector), NULL, 1, 2, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIAnimationElementVector2D), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIAnimationElementVector2D("UIAnimationElementVector2D", 0, UIAnimationElementVector2D_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIANIMATIONELEMENTVECTOR2D = &TypeDescriptor_UIAnimationElementVector2D;

static const TypeDescriptorField UIAnimationDescription_Fields[6] = {
    TypeDescriptorField("", DT_INT64, offsetof(UIAnimationDescription, nKey), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationDescription, serName_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIAnimationDescription, szName), 0x700020, -8),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationDescription, eType), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(UIAnimationDescription, nNodeRange), NULL, 1, 2, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIAnimationDescription), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIAnimationDescription("UIAnimationDescription", 0, UIAnimationDescription_Fields, 6, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIANIMATIONDESCRIPTION = &TypeDescriptor_UIAnimationDescription;

static const TypeDescriptorField UIControlAnimationBinding_Fields[5] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIControlAnimationBinding, serAnimationName_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIControlAnimationBinding, szAnimationName), 0x700020, -8),
    TypeDescriptorField("", DT_INT, offsetof(UIControlAnimationBinding, eProperty), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(UIControlAnimationBinding, flScaleFactor), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIControlAnimationBinding), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIControlAnimationBinding("UIControlAnimationBinding", 0, UIControlAnimationBinding_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UICONTROLANIMATIONBINDING = &TypeDescriptor_UIControlAnimationBinding;

static const TypeDescriptorField UIAnimationElements_Fields[16] = {
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElements, dwNumInt32Elements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationElements, serInt32Elements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIANIMATIONELEMENTINT32, offsetof(UIAnimationElements, ptInt32Elements), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElements, dwNumFloatElements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationElements, serFloatElements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIANIMATIONELEMENTFLOAT, offsetof(UIAnimationElements, ptFloatElements), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElements, dwNumVector2DElements), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationElements, serVector2DElements), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIANIMATIONELEMENTVECTOR2D, offsetof(UIAnimationElements, ptVector2DElements), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElements, dwNumAnimationDescriptions), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationElements, serAnimationDescriptions), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UIANIMATIONDESCRIPTION, offsetof(UIAnimationElements, ptAnimationDescriptions), 0x20, -0xc),
    TypeDescriptorField("", DT_INT, offsetof(UIAnimationElements, dwNumAnimationBindings), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIAnimationElements, serAnimationBindings), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UICONTROLANIMATIONBINDING, offsetof(UIAnimationElements, ptAnimationBindings), 0x20, -0xc),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIAnimationElements), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIAnimationElements("UIAnimationElements", 0, UIAnimationElements_Fields, 16, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UIANIMATIONELEMENTS = &TypeDescriptor_UIAnimationElements;

static const TypeDescriptorField UIControlData_Fields[11] = {
    TypeDescriptorField("", DT_INT, offsetof(UIControlData, nOrdinal), NULL, 1),
    TypeDescriptorField("", DT_INT64, offsetof(UIControlData, nKey), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIControlData, serName_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIControlData, szName), 0x700020, -8),
    TypeDescriptorField("", DT_INT64, offsetof(UIControlData, nParentKey), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIControlData, serParentKey_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(UIControlData, szParentKey), 0x700020, -8),
    TypeDescriptorField("", DT_INT, offsetof(UIControlData, eControlType), NULL, 1),
    TypeDescriptorField("", DT_UISTYLE, offsetof(UIControlData, tInlineStyle), NULL, 1),
    TypeDescriptorField("", DT_UIANIMATIONELEMENTS, offsetof(UIControlData, tAnimationElements), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIControlData), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIControlData("UIControlData", 0, UIControlData_Fields, 11, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UICONTROLDATA = &TypeDescriptor_UIControlData;

static const TypeDescriptorField UIDialogDefinition_Fields[9] = {
    TypeDescriptorField("", DT_INT, offsetof(UIDialogDefinition, dwMachineTime), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIDialogDefinition, serControlList), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UICONTROLDATA, offsetof(UIDialogDefinition, ptControlList), 0x20, -0xc),
    VariableArrayCountTypeDescriptorField("", offsetof(UIDialogDefinition, dwNumControls), 0x10000, DT_UICONTROLDATA, 4),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(UIDialogDefinition, serStyleList), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_UISTYLE, offsetof(UIDialogDefinition, ptStyleList), 0x20, -0xc),
    VariableArrayCountTypeDescriptorField("", offsetof(UIDialogDefinition, dwNumStyles), 0x10000, DT_UISTYLE, 4),
    TypeDescriptorField("", DT_DATAELEMENTS, offsetof(UIDialogDefinition, tConstants), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(UIDialogDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_UIDialogDefinition("UI", UI_XML_VERSION, UIDialogDefinition_Fields, 9, NULL, 0x80, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_UI = &TypeDescriptor_UIDialogDefinition;

#line 534
UIDialogGroupDef * gptGroupDefUI;

#line 539
UIDialogGroupDef::UIDialogGroupDef()
    : GroupDefinition("UI", UI_SNO,
                      SNOVersions(UI_PC_SNO_VERSION,
                                  UI_AXE_SNO_VERSION + AXE_BASE_SNO_VERSION,
                                  UI_CONSOLE_SNO_VERSION,
                                  UI_CONSOLE_SNO_VERSION,
                                  UI_CONSOLE_SNO_VERSION + NX64_BASE_SNO_VERSION),
                      SNODATATYPE_TITLE, sizeof(UIDialogDefinition), DT_UI, 0x2041, 0x400a) {
}

#line 559
uint32 UIDialogGroupDef::GetMaxCount() {
    return 100;
}

#line 571
void UIDialogGroupDef::Delete(void * lpDefinition) const {
    UIDialogDefinition * pdef = (UIDialogDefinition *)lpDefinition;

    if (pdef->pmapControlData) {
        POS pos = pdef->pmapControlData->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIControlData * pControl;
            pdef->pmapControlData->GetNextAssoc(pos, nKey, pControl);

            delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)pControl->tInlineStyle.pmapControlElements;
            delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)pControl->tInlineStyle.pmapInt32Elements;
            delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)pControl->tInlineStyle.pmapFloat32Elements;

            if (pControl->tInlineStyle.plistChildSpecs) {
                for (uint32 i = 0; i < pControl->tInlineStyle.plistChildSpecs->size(); i++) {
                    delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)(*pControl->tInlineStyle.plistChildSpecs)[i]->pmapControlElements;
                    (*pControl->tInlineStyle.plistChildSpecs)[i]->pmapControlElements = NULL;
                    delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)(*pControl->tInlineStyle.plistChildSpecs)[i]->pmapInt32Elements;
                    (*pControl->tInlineStyle.plistChildSpecs)[i]->pmapInt32Elements = NULL;
                    delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)(*pControl->tInlineStyle.plistChildSpecs)[i]->pmapFloat32Elements;
                    (*pControl->tInlineStyle.plistChildSpecs)[i]->pmapFloat32Elements = NULL;
                }
                delete (blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > *)pControl->tInlineStyle.plistChildSpecs;
                pControl->tInlineStyle.plistChildSpecs = NULL;
            }

            delete (UIControlDataArray *)pControl->plistChildren;

            UIStyle::DeleteClone(pControl->pBaseStyle, FALSE);
            SigmaMemoryFree(pControl->pBaseStyle, NULL);
            pControl->pBaseStyle = NULL;

            if (pControl->plistStylesFromAncestors) {
                for (uint32 i = 0; i < pControl->plistStylesFromAncestors->size(); i++) {
                    UIStyleChildSpec::DeleteClone((*pControl->plistStylesFromAncestors)[i], FALSE);
                }
                delete (blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > *)pControl->plistStylesFromAncestors;
            }
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIControlData *, UIControlData *> *)pdef->pmapControlData;
    }

    if (pdef->pmapStyles) {
        POS pos = pdef->pmapStyles->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIStyle * pStyle;
            pdef->pmapStyles->GetNextAssoc(pos, nKey, pStyle);

            delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)pStyle->pmapControlElements;
            pStyle->pmapControlElements = NULL;
            delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)pStyle->pmapInt32Elements;
            pStyle->pmapInt32Elements = NULL;
            delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)pStyle->pmapFloat32Elements;
            pStyle->pmapFloat32Elements = NULL;

            if (pStyle->plistChildSpecs) {
                for (uint32 i = 0; i < pStyle->plistChildSpecs->size(); i++) {
                    delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)(*pStyle->plistChildSpecs)[i]->pmapControlElements;
                    (*pStyle->plistChildSpecs)[i]->pmapControlElements = NULL;
                    delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)(*pStyle->plistChildSpecs)[i]->pmapInt32Elements;
                    (*pStyle->plistChildSpecs)[i]->pmapInt32Elements = NULL;
                    delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)(*pStyle->plistChildSpecs)[i]->pmapFloat32Elements;
                    (*pStyle->plistChildSpecs)[i]->pmapFloat32Elements = NULL;
                }
                delete (blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > *)pStyle->plistChildSpecs;
                pStyle->plistChildSpecs = NULL;
            }

            UIStyle::DeleteClone(pStyle->pBaseStyle, FALSE);
            SigmaMemoryFree(pStyle->pBaseStyle, NULL);
            pStyle->pBaseStyle = NULL;
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIStyle *, UIStyle *> *)pdef->pmapStyles;
        pdef->pmapStyles = NULL;
    }

    delete (XGrowableMap<unsigned long, unsigned long, CRefString, CRefString> *)pdef->tConstantsMaps.pmapStrings;
    pdef->tConstantsMaps.pmapStrings = NULL;
    delete (XGrowableMap<unsigned long, unsigned long, int, int> *)pdef->tConstantsMaps.pmapInt32s;
    pdef->tConstantsMaps.pmapInt32s = NULL;
    delete (XGrowableMap<unsigned long, unsigned long, float, float> *)pdef->tConstantsMaps.pmapFloat32s;
    pdef->tConstantsMaps.pmapFloat32s = NULL;

    GroupDefinition::Delete(lpDefinition);
}

#line 667
static int32 sSortUIControls(const void * arg1, const void * arg2) {
    const UIControlData * pControl1 = *(const UIControlData **)arg1;
    const UIControlData * pControl2 = *(const UIControlData **)arg2;
    return pControl1->nOrdinal - pControl2->nOrdinal;
}

// NON_MATCHING: 8404 vs 8460 bytes; register allocation and spill differences spread through the body
#line 679
void UIDialogGroupDef::Postprocess(void * lpDefinition) {
    UIDialogDefinition * ptDefinition = (UIDialogDefinition *)lpDefinition;

    GroupDefinition::Postprocess(lpDefinition);

    if (ptDefinition->dwNumControls == 0) {
        ptDefinition->pmapControlData = NULL;
    } else {
        ptDefinition->pmapControlData = new XGrowableMap<unsigned long, unsigned long, UIControlData *, UIControlData *>();
        ptDefinition->pmapControlData->SetBlockCount_Tracked(ptDefinition->dwNumControls);
        ptDefinition->pmapControlData->SetHashTableSize_Tracked(ptDefinition->dwNumControls / 2);
    }

    for (uint32 nControlIndex = 0; nControlIndex < ptDefinition->dwNumControls; nControlIndex++) {
        UIControlData & tControlData = ptDefinition->ptControlList[nControlIndex];
        ptDefinition->pmapControlData->SetAt_Tracked(tControlData.nKey, &tControlData);

        tControlData.pBaseStyle = NULL;
        tControlData.plistStylesFromAncestors = NULL;
        tControlData.plistChildren = new UIControlDataArray();
        tControlData.tInlineStyle.pBaseStyle = NULL;

        if (tControlData.tInlineStyle.tAttributes.dwNumStringElements == 0) {
            tControlData.tInlineStyle.pmapControlElements = NULL;
        } else {
            tControlData.tInlineStyle.pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
            tControlData.tInlineStyle.pmapControlElements->SetBlockCount_Tracked(tControlData.tInlineStyle.tAttributes.dwNumStringElements);
            tControlData.tInlineStyle.pmapControlElements->SetHashTableSize_Tracked(tControlData.tInlineStyle.tAttributes.dwNumStringElements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tControlData.tInlineStyle.tAttributes.dwNumStringElements; nElementIndex++) {
                UIControlElement & tControlElement = tControlData.tInlineStyle.tAttributes.ptStringElements[nElementIndex];
                tControlData.tInlineStyle.pmapControlElements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        if (tControlData.tInlineStyle.tAttributes.dwNumInt32Elements == 0) {
            tControlData.tInlineStyle.pmapInt32Elements = NULL;
        } else {
            tControlData.tInlineStyle.pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
            tControlData.tInlineStyle.pmapInt32Elements->SetBlockCount_Tracked(tControlData.tInlineStyle.tAttributes.dwNumInt32Elements);
            tControlData.tInlineStyle.pmapInt32Elements->SetHashTableSize_Tracked(tControlData.tInlineStyle.tAttributes.dwNumInt32Elements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tControlData.tInlineStyle.tAttributes.dwNumInt32Elements; nElementIndex++) {
                UIDataElementInt32 & tControlElement = tControlData.tInlineStyle.tAttributes.ptInt32Elements[nElementIndex];
                tControlData.tInlineStyle.pmapInt32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        if (tControlData.tInlineStyle.tAttributes.dwNumFloat32Elements == 0) {
            tControlData.tInlineStyle.pmapFloat32Elements = NULL;
        } else {
            tControlData.tInlineStyle.pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
            tControlData.tInlineStyle.pmapFloat32Elements->SetBlockCount_Tracked(tControlData.tInlineStyle.tAttributes.dwNumFloat32Elements);
            tControlData.tInlineStyle.pmapFloat32Elements->SetHashTableSize_Tracked(tControlData.tInlineStyle.tAttributes.dwNumFloat32Elements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tControlData.tInlineStyle.tAttributes.dwNumFloat32Elements; nElementIndex++) {
                UIDataElementFloat32 & tControlElement = tControlData.tInlineStyle.tAttributes.ptFloat32Elements[nElementIndex];
                tControlData.tInlineStyle.pmapFloat32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        tControlData.tInlineStyle.plistChildSpecs = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
        for (uint32 nChildSpecIndex = 0; nChildSpecIndex < tControlData.tInlineStyle.dwNumChildSpecs; nChildSpecIndex++) {
            UIStyleChildSpec & tSpec = tControlData.tInlineStyle.ptChildSpecs[nChildSpecIndex];
            tControlData.tInlineStyle.plistChildSpecs->push_back(&tSpec);

            if (tSpec.tAttributes.dwNumStringElements == 0) {
                tSpec.pmapControlElements = NULL;
            } else {
                tSpec.pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
                tSpec.pmapControlElements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumStringElements);
                tSpec.pmapControlElements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumStringElements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumStringElements; nElementIndex++) {
                    UIControlElement & tControlElement = tSpec.tAttributes.ptStringElements[nElementIndex];
                    tSpec.pmapControlElements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }

            if (tSpec.tAttributes.dwNumInt32Elements == 0) {
                tSpec.pmapInt32Elements = NULL;
            } else {
                tSpec.pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
                tSpec.pmapInt32Elements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumInt32Elements);
                tSpec.pmapInt32Elements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumInt32Elements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumInt32Elements; nElementIndex++) {
                    UIDataElementInt32 & tControlElement = tSpec.tAttributes.ptInt32Elements[nElementIndex];
                    tSpec.pmapInt32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }

            if (tSpec.tAttributes.dwNumFloat32Elements == 0) {
                tSpec.pmapFloat32Elements = NULL;
            } else {
                tSpec.pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
                tSpec.pmapFloat32Elements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumFloat32Elements);
                tSpec.pmapFloat32Elements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumFloat32Elements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumFloat32Elements; nElementIndex++) {
                    UIDataElementFloat32 & tControlElement = tSpec.tAttributes.ptFloat32Elements[nElementIndex];
                    tSpec.pmapFloat32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }
        }
    }

    ptDefinition->pControlDataRoot = NULL;
    for (uint32 nControlIndex = 0; nControlIndex < ptDefinition->dwNumControls; nControlIndex++) {
        UIControlData & tControlData = ptDefinition->ptControlList[nControlIndex];
        if (tControlData.nParentKey == 0) {
            ptDefinition->pControlDataRoot = &tControlData;
        } else {
            BOOL bRetval = FALSE;
            UIControlData * ptControlDataParent;
            if (ptDefinition->pmapControlData)
                bRetval = ptDefinition->pmapControlData->Lookup(tControlData.nParentKey, ptControlDataParent);
            if (bRetval)
                ptControlDataParent->plistChildren->push_back(&tControlData);
        }
    }

    for (uint32 nControlIndex = 0; nControlIndex < ptDefinition->dwNumControls; nControlIndex++) {
        UIControlData & tControlData = ptDefinition->ptControlList[nControlIndex];
        if (tControlData.plistChildren->size())
            blzext::vector_sort(*tControlData.plistChildren, sSortUIControls);
    }

    if (ptDefinition->dwNumStyles == 0) {
        ptDefinition->pmapStyles = NULL;
    } else {
        ptDefinition->pmapStyles = new XGrowableMap<unsigned long, unsigned long, UIStyle *, UIStyle *>();
        ptDefinition->pmapStyles->SetBlockCount_Tracked(ptDefinition->dwNumStyles);
        ptDefinition->pmapStyles->SetHashTableSize_Tracked(ptDefinition->dwNumStyles / 2);
    }

    for (uint32 nStyleIndex = 0; nStyleIndex < ptDefinition->dwNumStyles; nStyleIndex++) {
        UIStyle & tStyle = ptDefinition->ptStyleList[nStyleIndex];
        ptDefinition->pmapStyles->SetAt_Tracked(tStyle.nKey, &tStyle);
        tStyle.pBaseStyle = NULL;

        if (tStyle.tAttributes.dwNumStringElements == 0) {
            tStyle.pmapControlElements = NULL;
        } else {
            tStyle.pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
            tStyle.pmapControlElements->SetBlockCount_Tracked(tStyle.tAttributes.dwNumStringElements);
            tStyle.pmapControlElements->SetHashTableSize_Tracked(tStyle.tAttributes.dwNumStringElements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tStyle.tAttributes.dwNumStringElements; nElementIndex++) {
                UIControlElement & tControlElement = tStyle.tAttributes.ptStringElements[nElementIndex];
                tStyle.pmapControlElements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        if (tStyle.tAttributes.dwNumInt32Elements == 0) {
            tStyle.pmapInt32Elements = NULL;
        } else {
            tStyle.pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
            tStyle.pmapInt32Elements->SetBlockCount_Tracked(tStyle.tAttributes.dwNumInt32Elements);
            tStyle.pmapInt32Elements->SetHashTableSize_Tracked(tStyle.tAttributes.dwNumInt32Elements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tStyle.tAttributes.dwNumInt32Elements; nElementIndex++) {
                UIDataElementInt32 & tControlElement = tStyle.tAttributes.ptInt32Elements[nElementIndex];
                tStyle.pmapInt32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        if (tStyle.tAttributes.dwNumFloat32Elements == 0) {
            tStyle.pmapFloat32Elements = NULL;
        } else {
            tStyle.pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
            tStyle.pmapFloat32Elements->SetBlockCount_Tracked(tStyle.tAttributes.dwNumFloat32Elements);
            tStyle.pmapFloat32Elements->SetHashTableSize_Tracked(tStyle.tAttributes.dwNumFloat32Elements / 2);

            for (uint32 nElementIndex = 0; nElementIndex < tStyle.tAttributes.dwNumFloat32Elements; nElementIndex++) {
                UIDataElementFloat32 & tControlElement = tStyle.tAttributes.ptFloat32Elements[nElementIndex];
                tStyle.pmapFloat32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
            }
        }

        tStyle.plistChildSpecs = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
        for (uint32 nChildSpecIndex = 0; nChildSpecIndex < tStyle.dwNumChildSpecs; nChildSpecIndex++) {
            UIStyleChildSpec & tSpec = tStyle.ptChildSpecs[nChildSpecIndex];
            tStyle.plistChildSpecs->push_back(&tSpec);

            if (tSpec.tAttributes.dwNumStringElements == 0) {
                tSpec.pmapControlElements = NULL;
            } else {
                tSpec.pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
                tSpec.pmapControlElements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumStringElements);
                tSpec.pmapControlElements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumStringElements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumStringElements; nElementIndex++) {
                    UIControlElement & tControlElement = tSpec.tAttributes.ptStringElements[nElementIndex];
                    tSpec.pmapControlElements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }

            if (tSpec.tAttributes.dwNumInt32Elements == 0) {
                tSpec.pmapInt32Elements = NULL;
            } else {
                tSpec.pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
                tSpec.pmapInt32Elements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumInt32Elements);
                tSpec.pmapInt32Elements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumInt32Elements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumInt32Elements; nElementIndex++) {
                    UIDataElementInt32 & tControlElement = tSpec.tAttributes.ptInt32Elements[nElementIndex];
                    tSpec.pmapInt32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }

            if (tSpec.tAttributes.dwNumFloat32Elements == 0) {
                tSpec.pmapFloat32Elements = NULL;
            } else {
                tSpec.pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
                tSpec.pmapFloat32Elements->SetBlockCount_Tracked(tSpec.tAttributes.dwNumFloat32Elements);
                tSpec.pmapFloat32Elements->SetHashTableSize_Tracked(tSpec.tAttributes.dwNumFloat32Elements / 2);

                for (uint32 nElementIndex = 0; nElementIndex < tSpec.tAttributes.dwNumFloat32Elements; nElementIndex++) {
                    UIDataElementFloat32 & tControlElement = tSpec.tAttributes.ptFloat32Elements[nElementIndex];
                    tSpec.pmapFloat32Elements->SetAt_Tracked(tControlElement.nKey, &tControlElement);
                }
            }
        }

        UIControlElement * pElement;
        if (tStyle.GetAttribute(ATT_XML_STYLE, pElement)) {
            if (pElement->szAttrib[0])
                tStyle.pBaseStyle = UI_GetStyleDataClone(pElement->szAttrib, ptDefinition->tHeader.snoHandle, ptDefinition->pmapStyles);
        }
    }

    UI_SetupStyles(*ptDefinition->pControlDataRoot, ptDefinition->pControlDataRoot->plistChildren, ptDefinition->tHeader.snoHandle, ptDefinition->pmapStyles);

    if (ptDefinition->tConstants.dwNumStringElements == 0) {
        ptDefinition->tConstantsMaps.pmapStrings = NULL;
    } else {
        ptDefinition->tConstantsMaps.pmapStrings = new XGrowableMap<unsigned long, unsigned long, CRefString, CRefString>();
        ptDefinition->tConstantsMaps.pmapStrings->SetBlockCount_Tracked(ptDefinition->tConstants.dwNumStringElements);
        ptDefinition->tConstantsMaps.pmapStrings->SetHashTableSize_Tracked(ptDefinition->tConstants.dwNumStringElements / 2);

        for (uint32 nConstantIndex = 0; nConstantIndex < ptDefinition->tConstants.dwNumStringElements; nConstantIndex++)
            ptDefinition->tConstantsMaps.pmapStrings->SetAt_Tracked(ptDefinition->tConstants.ptStringElements[nConstantIndex].nKey, CRefString(ptDefinition->tConstants.ptStringElements[nConstantIndex].szAttrib));
    }

    if (ptDefinition->tConstants.dwNumInt32Elements == 0) {
        ptDefinition->tConstantsMaps.pmapInt32s = NULL;
    } else {
        ptDefinition->tConstantsMaps.pmapInt32s = new XGrowableMap<unsigned long, unsigned long, int, int>();
        ptDefinition->tConstantsMaps.pmapInt32s->SetBlockCount_Tracked(ptDefinition->tConstants.dwNumInt32Elements);
        ptDefinition->tConstantsMaps.pmapInt32s->SetHashTableSize_Tracked(ptDefinition->tConstants.dwNumInt32Elements / 2);

        for (uint32 nConstantIndex = 0; nConstantIndex < ptDefinition->tConstants.dwNumInt32Elements; nConstantIndex++)
            ptDefinition->tConstantsMaps.pmapInt32s->SetAt_Tracked(ptDefinition->tConstants.ptInt32Elements[nConstantIndex].nKey, ptDefinition->tConstants.ptInt32Elements[nConstantIndex].nValue);
    }

    if (ptDefinition->tConstants.dwNumFloat32Elements == 0) {
        ptDefinition->tConstantsMaps.pmapFloat32s = NULL;
    } else {
        ptDefinition->tConstantsMaps.pmapFloat32s = new XGrowableMap<unsigned long, unsigned long, float, float>();
        ptDefinition->tConstantsMaps.pmapFloat32s->SetBlockCount_Tracked(ptDefinition->tConstants.dwNumFloat32Elements);
        ptDefinition->tConstantsMaps.pmapFloat32s->SetHashTableSize_Tracked(ptDefinition->tConstants.dwNumFloat32Elements / 2);

        for (uint32 nConstantIndex = 0; nConstantIndex < ptDefinition->tConstants.dwNumFloat32Elements; nConstantIndex++)
            ptDefinition->tConstantsMaps.pmapFloat32s->SetAt_Tracked(ptDefinition->tConstants.ptFloat32Elements[nConstantIndex].nKey, ptDefinition->tConstants.ptFloat32Elements[nConstantIndex].flValue);
    }
}


#line 1564
BOOL UIDialogGroupDef::Write(void * lpDefinition, FileReference & tRef) const {
    DisplayInternalError(NULL, "", 1570, ERROR_INTERNAL_FATAL);
    ExitProgram(ERROR_INTERNAL_FATAL);
}

#line 1579
UIStyleChildSpec * UIStyleChildSpec::Clone() const {
    UIStyleChildSpec * pClone = (UIStyleChildSpec *)SigmaMemoryNew(sizeof(UIStyleChildSpec), 4, NULL, FALSE);
    SigmaMemoryCopy(pClone, this, sizeof(UIStyleChildSpec));

    if (pmapControlElements) {
        pClone->pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
        POS pos = pmapControlElements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIControlElement * pElement;
            pmapControlElements->GetNextAssoc(pos, nKey, pElement);
            UIControlElement * pNewElement = (UIControlElement *)SigmaMemoryNew(sizeof(UIControlElement), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIControlElement));
            pClone->pmapControlElements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    if (pmapInt32Elements) {
        pClone->pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
        POS pos = pmapInt32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementInt32 * pElement;
            pmapInt32Elements->GetNextAssoc(pos, nKey, pElement);
            UIDataElementInt32 * pNewElement = (UIDataElementInt32 *)SigmaMemoryNew(sizeof(UIDataElementInt32), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIDataElementInt32));
            pClone->pmapInt32Elements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    if (pmapFloat32Elements) {
        pClone->pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
        POS pos = pmapFloat32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementFloat32 * pElement;
            pmapFloat32Elements->GetNextAssoc(pos, nKey, pElement);
            UIDataElementFloat32 * pNewElement = (UIDataElementFloat32 *)SigmaMemoryNew(sizeof(UIDataElementFloat32), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIDataElementFloat32));
            pClone->pmapFloat32Elements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    return pClone;
}


#line 1635
void UIStyleChildSpec::DeleteClone(UIStyleChildSpec * pSpec, const BOOL fAllocatedDuringCompilation) {
    if (!pSpec)
        return;

    POS pos;
    if (pSpec->pmapControlElements) {
        pos = pSpec->pmapControlElements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIControlElement * pElement;
            pSpec->pmapControlElements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)pSpec->pmapControlElements;
        pSpec->pmapControlElements = NULL;
    }

    if (pSpec->pmapInt32Elements) {
        pos = pSpec->pmapInt32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementInt32 * pElement;
            pSpec->pmapInt32Elements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)pSpec->pmapInt32Elements;
        pSpec->pmapInt32Elements = NULL;
    }

    if (pSpec->pmapFloat32Elements) {
        pos = pSpec->pmapFloat32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementFloat32 * pElement;
            pSpec->pmapFloat32Elements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)pSpec->pmapFloat32Elements;
        pSpec->pmapFloat32Elements = NULL;
    }

    delete pSpec;
}

#line 1704
UIStyle * UIStyle::Clone() const {
    UIStyle * pClone = (UIStyle *)SigmaMemoryNew(sizeof(UIStyle), 4, NULL, FALSE);
    CopyTo(pClone);
    return pClone;
}

#line 1714
void UIStyle::CopyTo(UIStyle * pDest) const {
    SigmaMemoryCopy(pDest, this, sizeof(UIStyle));

    if (pmapControlElements) {
        pDest->pmapControlElements = new XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *>();
        POS pos = pmapControlElements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIControlElement * pElement;
            pmapControlElements->GetNextAssoc(pos, nKey, pElement);
            UIControlElement * pNewElement = (UIControlElement *)SigmaMemoryNew(sizeof(UIControlElement), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIControlElement));
            pDest->pmapControlElements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    if (pmapInt32Elements) {
        pDest->pmapInt32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *>();
        POS pos = pmapInt32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementInt32 * pElement;
            pmapInt32Elements->GetNextAssoc(pos, nKey, pElement);
            UIDataElementInt32 * pNewElement = (UIDataElementInt32 *)SigmaMemoryNew(sizeof(UIDataElementInt32), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIDataElementInt32));
            pDest->pmapInt32Elements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    if (pmapFloat32Elements) {
        pDest->pmapFloat32Elements = new XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *>();
        POS pos = pmapFloat32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementFloat32 * pElement;
            pmapFloat32Elements->GetNextAssoc(pos, nKey, pElement);
            UIDataElementFloat32 * pNewElement = (UIDataElementFloat32 *)SigmaMemoryNew(sizeof(UIDataElementFloat32), 4, NULL, FALSE);
            SigmaMemoryCopy(pNewElement, pElement, sizeof(UIDataElementFloat32));
            pDest->pmapFloat32Elements->SetAt_Tracked(nKey, pNewElement);
        }
    }

    if (plistChildSpecs) {
        pDest->plistChildSpecs = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
        for (uint32 i = 0; i < plistChildSpecs->size(); i++) {
            UIStyleChildSpec * pSpec = (*plistChildSpecs)[i];
            pDest->plistChildSpecs->push_back(pSpec->Clone());
        }
    }

    if (pBaseStyle)
        pDest->pBaseStyle = pBaseStyle->Clone();
}


#line 1786
void UIStyle::DeleteClone(UIStyle * pStyle, const BOOL fAllocatedDuringCompilation) {
    if (!pStyle)
        return;

    POS pos;
    if (pStyle->pmapControlElements) {
        pos = pStyle->pmapControlElements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIControlElement * pElement;
            pStyle->pmapControlElements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIControlElement *, UIControlElement *> *)pStyle->pmapControlElements;
    }

    if (pStyle->pmapInt32Elements) {
        pos = pStyle->pmapInt32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementInt32 * pElement;
            pStyle->pmapInt32Elements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIDataElementInt32 *, UIDataElementInt32 *> *)pStyle->pmapInt32Elements;
    }

    if (pStyle->pmapFloat32Elements) {
        pos = pStyle->pmapFloat32Elements->GetStartAssoc();
        while (pos) {
            uint64 nKey;
            UIDataElementFloat32 * pElement;
            pStyle->pmapFloat32Elements->GetNextAssoc(pos, nKey, pElement);
            SigmaMemoryFree(pElement, NULL);
        }
        delete (XGrowableMap<unsigned long, unsigned long, UIDataElementFloat32 *, UIDataElementFloat32 *> *)pStyle->pmapFloat32Elements;
    }

    if (pStyle->plistChildSpecs) {
        for (uint32 i = 0; i < pStyle->plistChildSpecs->size(); i++) {
            UIStyleChildSpec * pSpec = (*pStyle->plistChildSpecs)[i];
            UIStyleChildSpec::DeleteClone(pSpec, fAllocatedDuringCompilation);
        }
        delete (blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > *)pStyle->plistChildSpecs;
    }

    if (pStyle->pBaseStyle) {
        DeleteClone(pStyle->pBaseStyle, fAllocatedDuringCompilation);
        SigmaMemoryFree(pStyle->pBaseStyle, NULL);
        pStyle->pBaseStyle = NULL;
    }
}

#line 1874
inline BOOL UIStyle::GetAttribute(const UIAttributeType eAttributeType, UIControlElement *& pControlElement) const {
    BOOL found = FALSE;
    const UIStyle * pStyle = this;
    while (pStyle) {
        if (pStyle->pmapControlElements && pStyle->pmapControlElements->Lookup(eAttributeType, pControlElement))
            return TRUE;
        pStyle = pStyle->pBaseStyle;
    }
    return found;
}

#line 1891
inline BOOL UIStyle::GetAttribute(const UIAttributeType eAttributeType, UIDataElementInt32 *& pDataElement) const {
    BOOL found = FALSE;
    const UIStyle * pStyle = this;
    while (pStyle) {
        if (pStyle->pmapInt32Elements && pStyle->pmapInt32Elements->Lookup(eAttributeType, pDataElement))
            return TRUE;
        pStyle = pStyle->pBaseStyle;
    }
    return found;
}

#line 1908
inline BOOL UIStyle::GetAttribute(const UIAttributeType eAttributeType, UIDataElementFloat32 *& pDataElement) const {
    BOOL found = FALSE;
    const UIStyle * pStyle = this;
    while (pStyle) {
        if (pStyle->pmapFloat32Elements && pStyle->pmapFloat32Elements->Lookup(eAttributeType, pDataElement))
            return TRUE;
        pStyle = pStyle->pBaseStyle;
    }
    return found;
}


#line 1973
static BOOL sUIControlData_GetAttribute(const UIControlData * pControl, const UIAttributeType eAttributeType, UIControlElement *& pControlElement) {
    BOOL found = FALSE;
    if (eAttributeType != ATT_XML_STYLE && pControl->plistStylesFromAncestors) {
        for (int32 i = (int32)pControl->plistStylesFromAncestors->size() - 1; i >= 0 && !found; i--) {
            if ((*pControl->plistStylesFromAncestors)[i]->pmapControlElements && (*pControl->plistStylesFromAncestors)[i]->pmapControlElements->Lookup(eAttributeType, pControlElement))
                found = TRUE;
        }
    }
    if (!found && pControl->tInlineStyle.pmapControlElements && pControl->tInlineStyle.pmapControlElements->Lookup(eAttributeType, pControlElement))
        found = TRUE;
    if (!found && eAttributeType != ATT_XML_STYLE && pControl->pBaseStyle)
        found = pControl->pBaseStyle->GetAttribute(eAttributeType, pControlElement);
    return found;
}


#line 2008
static BOOL sUIControlData_GetAttribute(const UIControlData * pControl, const UIAttributeType eAttributeType, UIDataElementInt32 *& pElement) {
    BOOL found = FALSE;
    if (eAttributeType != ATT_XML_STYLE && pControl->plistStylesFromAncestors) {
        for (int32 i = (int32)pControl->plistStylesFromAncestors->size() - 1; i >= 0 && !found; i--) {
            if ((*pControl->plistStylesFromAncestors)[i]->pmapInt32Elements && (*pControl->plistStylesFromAncestors)[i]->pmapInt32Elements->Lookup(eAttributeType, pElement))
                found = TRUE;
        }
    }
    if (!found && pControl->tInlineStyle.pmapInt32Elements && pControl->tInlineStyle.pmapInt32Elements->Lookup(eAttributeType, pElement))
        found = TRUE;
    if (!found && eAttributeType != ATT_XML_STYLE && pControl->pBaseStyle)
        found = pControl->pBaseStyle->GetAttribute(eAttributeType, pElement);
    return found;
}



#line 2043
static BOOL sUIControlData_GetAttribute(const UIControlData * pControl, const UIAttributeType eAttributeType, UIDataElementFloat32 *& pElement) {
    BOOL found = FALSE;
    if (eAttributeType != ATT_XML_STYLE && pControl->plistStylesFromAncestors) {
        for (int32 i = (int32)pControl->plistStylesFromAncestors->size() - 1; i >= 0 && !found; i--) {
            if ((*pControl->plistStylesFromAncestors)[i]->pmapFloat32Elements && (*pControl->plistStylesFromAncestors)[i]->pmapFloat32Elements->Lookup(eAttributeType, pElement))
                found = TRUE;
        }
    }
    if (!found && pControl->tInlineStyle.pmapFloat32Elements && pControl->tInlineStyle.pmapFloat32Elements->Lookup(eAttributeType, pElement))
        found = TRUE;
    if (!found && eAttributeType != ATT_XML_STYLE && pControl->pBaseStyle)
        found = pControl->pBaseStyle->GetAttribute(eAttributeType, pElement);
    return found;
}

#line 2078
BOOL UIControlData::AttributeExists(const UIAttributeType eAttributeType) const {
    switch (UI_GetAttributeDataType(eAttributeType)) {
    case UIDT_INT32:
    case UIDT_UINT32:
    case UIDT_UICOLOR:
    case UIDT_BOOL: {
        UIDataElementInt32 * pElement;
        return sUIControlData_GetAttribute(this, eAttributeType, pElement);
    }
    case UIDT_FLOAT32: {
        UIDataElementFloat32 * pElement;
        return sUIControlData_GetAttribute(this, eAttributeType, pElement);
    }
    default: {
        UIControlElement * pElement;
        return sUIControlData_GetAttribute(this, eAttributeType, pElement);
    }
    }
}

#line 2107
LPCSTR UIControlData::GetAttributeValue(const UIAttributeType eAttributeType) const {
    UIControlElement * pControlElement;
    BOOL bRetVal = sUIControlData_GetAttribute(this, eAttributeType, pControlElement);
    if (!bRetVal) {
        const AttributeInfo & info = arAttributeInfos[eAttributeType];
        return info.szDefaultValue;
    }
    return pControlElement->szAttrib;
}

#line 2127
inline int32 UIControlData::GetInt32Value(const UIAttributeType eAttributeType) const {
    UIDataElementInt32 * pElement;
    BOOL bRetVal = sUIControlData_GetAttribute(this, eAttributeType, pElement);
    if (!bRetVal) {
        const AttributeInfo & info = arAttributeInfos[eAttributeType];
        return UI_ParseAttributeInt32(info.eDataType, info.szDefaultValue);
    }
    return pElement->nValue;
}

#line 2147
inline float UIControlData::GetFloat32Value(const UIAttributeType eAttributeType) const {
    UIDataElementFloat32 * pElement;
    BOOL bRetVal = sUIControlData_GetAttribute(this, eAttributeType, pElement);
    if (!bRetVal) {
        const AttributeInfo & info = arAttributeInfos[eAttributeType];
        return (float)atof(info.szDefaultValue);
    }
    return pElement->flValue;
}

#line 2167
BOOL UIControlData::GetBoolValue(const UIAttributeType eAttributeType) const {
    UIDataElementInt32 * pElement;
    BOOL bRetVal = sUIControlData_GetAttribute(this, eAttributeType, pElement);
    const AttributeInfo & info = arAttributeInfos[eAttributeType];
    if (!bRetVal)
        return UI_ParseAttributeInt32(info.eDataType, info.szDefaultValue);
    return pElement->nValue;
}

#line 2187
inline uint32 UIControlData::GetUint32Value(const UIAttributeType eAttributeType) const {
    UIDataElementInt32 * pElement;
    BOOL bRetVal = sUIControlData_GetAttribute(this, eAttributeType, pElement);
    if (!bRetVal) {
        const AttributeInfo & info = arAttributeInfos[eAttributeType];
        return UI_ParseAttributeInt32(info.eDataType, info.szDefaultValue);
    }
    return pElement->nValue;
}

#line 2208
inline BOOL UIControlData::GetImageAttribute(const UIAttributeType eAttributeType, ImageTextureFrame & tImageTexFrame) const {
    LPCSTR szDecoratedLabel = GetAttributeValue(eAttributeType);
    BOOL fSuccess = FALSE;
    if (szDecoratedLabel[0])
        fSuccess = ImageCollectionGetImageInfo(szDecoratedLabel, tImageTexFrame);
    if (!fSuccess)
        tImageTexFrame = cgtImageTextureFrameNull;
    return fSuccess;
}

#line 2234
UIControlData * UIControlData::Clone() const {
    UIControlData * pClone = (UIControlData *)SigmaMemoryNew(sizeof(UIControlData), 4, NULL, FALSE);
    SigmaMemoryCopy(pClone, this, sizeof(UIControlData));
    tInlineStyle.CopyTo(&pClone->tInlineStyle);

    if (plistStylesFromAncestors) {
        pClone->plistStylesFromAncestors = new blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> >();
        for (uint32 i = 0; i < plistStylesFromAncestors->size(); i++) {
            UIStyleChildSpec * pSpec = (*plistStylesFromAncestors)[i];
            pClone->plistStylesFromAncestors->push_back(pSpec->Clone());
        }
    }

    pClone->plistChildren = new UIControlDataArray();
    for (uint32 i = 0; i < plistChildren->size(); i++) {
        UIControlData * pChild = (*plistChildren)[i];
        pClone->plistChildren->push_back(pChild->Clone());
    }

    if (pBaseStyle)
        pClone->pBaseStyle = pBaseStyle->Clone();

    return pClone;
}

#line 2329
void UIControlData::Delete(UIControlData * pClone, const BOOL fAllocatedDuringCompilation) {
    if (!pClone)
        return;

    UIStyle::DeleteClone(&pClone->tInlineStyle, fAllocatedDuringCompilation);
    UIStyle::DeleteClone(pClone->pBaseStyle, fAllocatedDuringCompilation);
    SigmaMemoryFree(pClone->pBaseStyle, NULL);
    pClone->pBaseStyle = NULL;

    if (pClone->plistStylesFromAncestors) {
        for (uint32 i = 0; i < pClone->plistStylesFromAncestors->size(); i++)
            UIStyleChildSpec::DeleteClone((*pClone->plistStylesFromAncestors)[i], fAllocatedDuringCompilation);
        delete (blz::vector<UIStyleChildSpec *, blz::allocator<UIStyleChildSpec *> > *)pClone->plistStylesFromAncestors;
    }

    if (pClone->plistChildren) {
        for (uint32 i = 0; i < pClone->plistChildren->size(); i++) {
            UIControlData * pChild = (*pClone->plistChildren)[i];
            Delete(pChild, fAllocatedDuringCompilation);
        }
        delete (UIControlDataArray *)pClone->plistChildren;
    }

    SigmaMemoryFree(pClone, NULL);
}

#line 2374
UIControlDataExt::UIControlDataExt(UIControlData & tControl)
    : m_control(tControl), m_template(NULL), m_templateSno(SNO_NONE) {
    nKey = tControl.nKey;
    eControlType = tControl.eControlType;
}

#line 2395
void UIControlDataExt::SetTemplate(UIControlData * pTemplate, SNO templateSno) {
    m_template = new UIControlDataExt(*pTemplate);
    m_templateSno = templateSno;
}

#line 2410
inline void UIControlDataExt::GetControlChildren(UIControlDataArray * plistChildren) const {
    for (uint32 i = 0; i < m_control.plistChildren->size(); i++)
        plistChildren->push_back((*m_control.plistChildren)[i]);
}

#line 2420
inline void UIControlDataExt::GetAllChildren(UIControlDataArray * plistChildren) const {
    GetControlChildren(plistChildren);
    if (m_template)
        m_template->GetAllChildren(plistChildren);
}

#line 2432
void UIControlDataExt::CloneTemplateChildren(UIControlDataArray * plistChildren) const {
    if (m_template) {
        UIControlDataArray templateChildren;
        m_template->GetAllChildren(&templateChildren);
        for (uint32 i = 0; i < templateChildren.size(); i++) {
            UIControlData * pControl = templateChildren[i]->Clone();
            plistChildren->push_back(pControl);
        }
    }
}

#line 2452
BOOL UIControlDataExt::AttributeExists(const UIAttributeType eAttributeType) const {
    if (m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE && m_template->AttributeExists(eAttributeType))
        return TRUE;

    return m_control.AttributeExists(eAttributeType);
}

#line 2466
LPCSTR UIControlDataExt::GetAttributeValue(const UIAttributeType eAttributeType) const {
    if (!m_control.AttributeExists(eAttributeType) && m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE)
        return m_template->GetAttributeValue(eAttributeType);

    return m_control.GetAttributeValue(eAttributeType);
}

#line 2484
int32 UIControlDataExt::GetInt32Value(const UIAttributeType eAttributeType) const {
    if (!m_control.AttributeExists(eAttributeType) && m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE)
        return m_template->GetInt32Value(eAttributeType);

    return m_control.GetInt32Value(eAttributeType);
}

#line 2502
float UIControlDataExt::GetFloat32Value(const UIAttributeType eAttributeType) const {
    if (!m_control.AttributeExists(eAttributeType) && m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE)
        return m_template->GetFloat32Value(eAttributeType);

    return m_control.GetFloat32Value(eAttributeType);
}

#line 2520
BOOL UIControlDataExt::GetBoolValue(const UIAttributeType eAttributeType) const {
    if (!m_control.AttributeExists(eAttributeType) && m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE)
        return m_template->GetBoolValue(eAttributeType);

    return m_control.GetBoolValue(eAttributeType);
}

#line 2538
uint32 UIControlDataExt::GetUint32Value(const UIAttributeType eAttributeType) const {
    if (!m_control.AttributeExists(eAttributeType) && m_template && eAttributeType != ATT_TEMPLATE && eAttributeType != ATT_XML_TEMPLATE)
        return m_template->GetUint32Value(eAttributeType);

    return m_control.GetUint32Value(eAttributeType);
}

#line 2556
// NON_MATCHING: the rebuild shares the zero-return block at +0x90 between the empty-label and failed-lookup paths.
BOOL UIControlDataExt::GetImageAttribute(const UIAttributeType eAttributeType, ImageTextureFrame & tImageTexFrame) const {
    if (m_control.AttributeExists(eAttributeType))
        return m_control.GetImageAttribute(eAttributeType, tImageTexFrame);

    if (m_template)
        return m_template->GetImageAttribute(eAttributeType, tImageTexFrame);

    tImageTexFrame = cgtImageTextureFrameNull;
    return FALSE;
}

#line 2582
inline int32 UI_ParseAttributeInt32(UIDataType eDataType, LPCSTR szValue) {
    if (eDataType == UIDT_UICOLOR) {
        uint32 nVal = atoh(szValue);
        return nVal;
    } else if (eDataType == UIDT_UINT32) {
        uint32 nVal = atol(szValue);
        return nVal;
    }
    return atoi(szValue);
}

#line 2608
inline UIDataType UI_GetAttributeDataType(const UIAttributeType eAttributeType) {
    return arAttributeInfos[eAttributeType].eDataType;
}
