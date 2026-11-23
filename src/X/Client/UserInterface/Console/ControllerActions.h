// Adopted from the DWARF reference; hand-maintained.
// Original file: X/Client/UserInterface/Console/ControllerActions.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/RefString.h"
#include "../../../Sigma/SigmaTypes.h"
#include "../../../Sigma/Unicode.h"

namespace Console {
enum {
    CONTROLLER_INPUT_INVALID = -1,
    CONTROLLER_INPUT_LEFT_STICK = 0,
    CONTROLLER_INPUT_RIGHT_STICK = 1,
    CONTROLLER_INPUT_LEFT_STICK_HORIZONTAL = 2,
    CONTROLLER_INPUT_LEFT_STICK_VERTICAL = 3,
    CONTROLLER_INPUT_BUTTON_1 = 4,
    CONTROLLER_INPUT_BUTTON_3 = 5,
    CONTROLLER_INPUT_BUTTON_4 = 6,
    CONTROLLER_INPUT_BUTTON_2 = 7,
    CONTROLLER_INPUT_LEFT_SHOULDER = 8,
    CONTROLLER_INPUT_RIGHT_SHOULDER = 9,
    CONTROLLER_INPUT_LEFT_TRIGGER = 10,
    CONTROLLER_INPUT_RIGHT_TRIGGER = 11,
    CONTROLLER_INPUT_DPAD_UP = 12,
    CONTROLLER_INPUT_DPAD_DOWN = 13,
    CONTROLLER_INPUT_DPAD_LEFT = 14,
    CONTROLLER_INPUT_DPAD_RIGHT = 15,
    CONTROLLER_INPUT_DPAD_HORIZONTAL = 16,
    CONTROLLER_INPUT_DPAD_VERTICAL = 17,
    CONTROLLER_INPUT_BACK = 18,
    CONTROLLER_INPUT_START = 19,
    CONTROLLER_INPUT_OPTIONS = 20,
    CONTROLLER_INPUT_LEFT_STICK_BUTTON = 21,
    CONTROLLER_INPUT_RIGHT_STICK_BUTTON = 22,
    CONSOLE_INPUT_COUNT = 23,
};

typedef int32 ControllerInput;

typedef int32 ControllerActionId;

static const Console::ControllerActionId ACTION_NONE = -1;  // line 51

struct ControllerAction {  // size 0x40
    Console::ControllerActionId id;  // 0x00
    CRefString sText;  // 0x08
    CRefString sTextNoDelay;  // 0x20
    int32 nDelayUntil;  // 0x38
    ControllerAction()  // line 61
        : id(ACTION_NONE)
        , nDelayUntil(0)
    {
    }
};
static_assert(sizeof(ControllerAction) == 0x40, "ControllerAction");

class ControllerActionManager {  // size 0x5c8
public:
    static const int32 DEFAULT_SPACE_BETWEEN_LABELS = 6;
    ControllerActionManager();
    void SetAction(const Console::ControllerInput eInput, const Console::ControllerActionId idAction, const LPCSTR szText);
    void SetDelayedAction(const Console::ControllerInput eInput, const Console::ControllerActionId idAction, const LPCSTR szText, uint32 nDelay);
    void ClearAction(const Console::ControllerInput eInput);
    void ClearAllActions();
    Console::ControllerActionId GetActionForInput(const Console::ControllerInput eInput) const;
    CRefString ToString(const int32 nSpaceBetweenLabels) const;
    int32 GetActiveActionCount() const;
    static CRefString InputToString(const Console::ControllerInput eInput);
    BOOL AreActionsDirty() const;
    void SetActionsDirtyFlag(const BOOL bIsDirty);
    void OnUpdate();
    BOOL IsActionDelayed(const Console::ControllerInput eInput) const;
private:
    Console::ControllerAction m_tActions[23];  // 0x00
    BOOL m_bActionsDirtyFlag;  // 0x5c0
    ControllerActionManager(const Console::ControllerActionManager &);
};
static_assert(sizeof(ControllerActionManager) == 0x5c8, "ControllerActionManager");

}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    54  Console::ControllerAction::~ControllerAction()  [inlined into 1 functions]
//   line    61  Console::ControllerAction::ControllerAction()  [inlined into 1 functions]
//   line    70  Console::ControllerActionManager::~ControllerActionManager()  [0x7100105ef0]

// Variables defined in this file:
//   line    51  static const Console::ControllerActionId Console::ACTION_NONE
