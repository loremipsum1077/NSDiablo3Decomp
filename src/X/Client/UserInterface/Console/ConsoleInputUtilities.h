// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Console/ConsoleInputUtilities.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../../../Sigma/Math/Vector.h"
#include "../../../Sigma/SigmaTypes.h"

namespace Console {
class InputSpeedLimiter {  // size 0x4
public:
    InputSpeedLimiter();
    BOOL HasWaitTimeExpired(const uint32 nSpeedLimitInMS) const;
    void ResetLastInputTime();
    void UpdateLastInputTime();
private:
    Blizzard::Time::Millisecond m_nLastInputTime;  // 0x00
};
static_assert(sizeof(InputSpeedLimiter) == 0x4, "InputSpeedLimiter");

class AnalogStickInputFilter {  // size 0x28
public:
    struct Rule {  // size 0x14
        typedef int32 ID;
        static const Console::AnalogStickInputFilter::Rule::ID IN_DEAD_ZONE = -2;
        static const Console::AnalogStickInputFilter::Rule::ID IGNORE_INPUT = -1;
        static const Console::AnalogStickInputFilter::Rule::ID CATCH_ALL_INPUT = 0;
        Console::AnalogStickInputFilter::Rule::ID id;  // 0x00
        Vector2D vecMinStickPos;  // 0x04
        Vector2D vecMaxStickPos;  // 0x0c
        static Console::AnalogStickInputFilter::Rule CatchAllRule;
    };

    AnalogStickInputFilter();
    AnalogStickInputFilter(const Console::AnalogStickInputFilter &);
    Console::AnalogStickInputFilter & operator=(const Console::AnalogStickInputFilter & other);
    void SetDeadZone(const float flDeadZoneRadius);
    void SetMinimumInputDelay(const Blizzard::Time::Millisecond nDelay);
    Blizzard::Time::Millisecond GetMinimumInputDelay();
    void AddRule(const Console::AnalogStickInputFilter::Rule & tRule);
    Console::AnalogStickInputFilter::Rule::ID HandleInput(const Vector2D & vecStickPos, BOOL bIgnoreSpeedLimiter);
private:
    Blizzard::Time::Millisecond m_nInputDelayTime;  // 0x00
    float m_flDeadZoneRadius;  // 0x04
    blz::vector<Console::AnalogStickInputFilter::Rule, blz::allocator<Console::AnalogStickInputFilter::Rule> > m_arRules;  // 0x08
    Console::InputSpeedLimiter m_tSpeedLimiter;  // 0x20
    Console::AnalogStickInputFilter::Rule::ID m_idLastRuleUsed;  // 0x24
    BOOL IsInputInDeadZone(const Vector2D & vecInput) const;
    BOOL HasInputReversedDirection(const Vector2D &, const Vector2D &) const;
};
static_assert(sizeof(AnalogStickInputFilter) == 0x28, "AnalogStickInputFilter");

enum {
    RULE_STICK_UP = 0,
    RULE_STICK_DOWN = 1,
    RULE_STICK_LEFT = 2,
    RULE_STICK_RIGHT = 3,
};

enum {
    FILTER_DIRECTIONAL = 0,
    FILTER_HORIZONTAL = 1,
    FILTER_VERTICAL = 2,
    FILTER_STRICT_VERTICAL = 3,
};

typedef int32 DefaultStickFilterType;

AnalogStickInputFilter GetDefaultStickInputFilter(DefaultStickFilterType eType);

}  // namespace Console

// Functions defined in this file (bodies are not in the DWARF):
//   line    46  Console::AnalogStickInputFilter::~AnalogStickInputFilter()  [inlined into 70 functions]
//   line    50  Console::AnalogStickInputFilter::Rule & Console::AnalogStickInputFilter::Rule::operator=(const Console::AnalogStickInputFilter::Rule &)  [inlined into 2 functions]
