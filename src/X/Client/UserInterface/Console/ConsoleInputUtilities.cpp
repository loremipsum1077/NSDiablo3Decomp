#include "ConsoleInputUtilities.h"

#include <float.h>

namespace Console {
AnalogStickInputFilter::Rule AnalogStickInputFilter::Rule::CatchAllRule = { AnalogStickInputFilter::Rule::CATCH_ALL_INPUT, Vector2D(-FLT_MAX, -FLT_MAX), Vector2D(FLT_MAX, FLT_MAX) };
InputSpeedLimiter::InputSpeedLimiter()
    : m_nLastInputTime(0) {
}

BOOL InputSpeedLimiter::HasWaitTimeExpired(const uint32 nSpeedLimitInMS) const {
    Blizzard::Time::Millisecond now = Blizzard::Time::Milliseconds();
    return Blizzard::Time::GetTimeElapsed(m_nLastInputTime, now) > nSpeedLimitInMS;
}

void InputSpeedLimiter::ResetLastInputTime() {
    m_nLastInputTime = 0;
}

void InputSpeedLimiter::UpdateLastInputTime() {
    m_nLastInputTime = Blizzard::Time::Milliseconds();
}

AnalogStickInputFilter::AnalogStickInputFilter()
    : m_nInputDelayTime(0),
      m_flDeadZoneRadius(0.0f),
      m_arRules(),
      m_tSpeedLimiter(),
      m_idLastRuleUsed(Rule::IGNORE_INPUT) {
}

AnalogStickInputFilter& AnalogStickInputFilter::operator=(const AnalogStickInputFilter& other) {
    if (&other != this) {
        m_nInputDelayTime = other.m_nInputDelayTime;
        m_flDeadZoneRadius = other.m_flDeadZoneRadius;
        m_arRules = other.m_arRules;
        m_tSpeedLimiter = other.m_tSpeedLimiter;
        m_idLastRuleUsed = other.m_idLastRuleUsed;
    }

    return *this;
}

void AnalogStickInputFilter::SetDeadZone(const float flDeadZoneRadius) {
    m_flDeadZoneRadius = flDeadZoneRadius;
}

void AnalogStickInputFilter::SetMinimumInputDelay(const Blizzard::Time::Millisecond nDelay) {
    m_nInputDelayTime = nDelay;
}

void AnalogStickInputFilter::AddRule(const Rule& tRule) {
    m_arRules.push_back(tRule);
}

AnalogStickInputFilter::Rule::ID AnalogStickInputFilter::HandleInput(const Vector2D& vecStickPos,
                                                                    BOOL bIgnoreSpeedLimiter) {
    if (IsInputInDeadZone(vecStickPos)) {
        if (!bIgnoreSpeedLimiter) {
            m_tSpeedLimiter.ResetLastInputTime();
        }

        return Rule::IN_DEAD_ZONE;
    }

    Rule::ID idResult = Rule::IGNORE_INPUT;

    for (uint32 i = 0; i < m_arRules.size(); i++) {
        Rule& tRule = m_arRules[i];
        if (vecStickPos.x >= tRule.vecMinStickPos.x && vecStickPos.y >= tRule.vecMinStickPos.y &&
            vecStickPos.x <= tRule.vecMaxStickPos.x && vecStickPos.y <= tRule.vecMaxStickPos.y) {
            idResult = tRule.id;
            break;
        }
    }

    if (!bIgnoreSpeedLimiter) {
        if (!m_tSpeedLimiter.HasWaitTimeExpired(m_nInputDelayTime) && idResult == m_idLastRuleUsed) {
            return Rule::IGNORE_INPUT;
        }

        m_tSpeedLimiter.UpdateLastInputTime();
    }

    m_idLastRuleUsed = idResult;

    return idResult;
}

BOOL AnalogStickInputFilter::IsInputInDeadZone(const Vector2D& vecInput) const {
    if (m_flDeadZoneRadius > EPSILON) {
        return vecInput.MagnitudeSquared() <= m_flDeadZoneRadius * m_flDeadZoneRadius;
    }

    return FALSE;
}

AnalogStickInputFilter GetDefaultStickInputFilter(DefaultStickFilterType eType) {
    const float HORIZONTAL_RANGE = FLT_MAX;
    const float flDeadZoneVal = 0.3f;

    AnalogStickInputFilter tDefaultFilter;
    tDefaultFilter.SetDeadZone(flDeadZoneVal);

    switch (eType) {
        case FILTER_DIRECTIONAL: {
            AnalogStickInputFilter::Rule tStickUpRule;
            tStickUpRule.id = RULE_STICK_UP;
            tStickUpRule.vecMinStickPos.Set(-flDeadZoneVal, 0.0f);
            tStickUpRule.vecMaxStickPos.Set(flDeadZoneVal, FLT_MAX);
            tDefaultFilter.AddRule(tStickUpRule);

            AnalogStickInputFilter::Rule tStickDownRule;
            tStickDownRule.id = RULE_STICK_DOWN;
            tStickDownRule.vecMinStickPos.Set(-flDeadZoneVal, -FLT_MAX);
            tStickDownRule.vecMaxStickPos.Set(flDeadZoneVal, 0.0f);
            tDefaultFilter.AddRule(tStickDownRule);

            AnalogStickInputFilter::Rule tStickLeftRule;
            tStickLeftRule.id = RULE_STICK_LEFT;
            tStickLeftRule.vecMinStickPos.Set(-FLT_MAX, -flDeadZoneVal);
            tStickLeftRule.vecMaxStickPos.Set(0.0f, flDeadZoneVal);
            tDefaultFilter.AddRule(tStickLeftRule);

            AnalogStickInputFilter::Rule tStickRightRule;
            tStickRightRule.id = RULE_STICK_RIGHT;
            tStickRightRule.vecMinStickPos.Set(0.0f, -flDeadZoneVal);
            tStickRightRule.vecMaxStickPos.Set(FLT_MAX, flDeadZoneVal);
            tDefaultFilter.AddRule(tStickRightRule);
            break;
        }

        case FILTER_HORIZONTAL: {
            AnalogStickInputFilter::Rule tStickLeftRule;
            tStickLeftRule.id = RULE_STICK_LEFT;
            tStickLeftRule.vecMinStickPos.Set(-HORIZONTAL_RANGE, -FLT_MAX);
            tStickLeftRule.vecMaxStickPos.Set(-flDeadZoneVal, FLT_MAX);
            tDefaultFilter.AddRule(tStickLeftRule);

            AnalogStickInputFilter::Rule tStickRightRule;
            tStickRightRule.id = RULE_STICK_RIGHT;
            tStickRightRule.vecMinStickPos.Set(flDeadZoneVal, -FLT_MAX);
            tStickRightRule.vecMaxStickPos.Set(HORIZONTAL_RANGE, FLT_MAX);
            tDefaultFilter.AddRule(tStickRightRule);
            break;
        }

        case FILTER_VERTICAL: {
            AnalogStickInputFilter::Rule tStickDownRule;
            tStickDownRule.id = RULE_STICK_DOWN;
            tStickDownRule.vecMinStickPos.Set(-FLT_MAX, -FLT_MAX);
            tStickDownRule.vecMaxStickPos.Set(FLT_MAX, -flDeadZoneVal);
            tDefaultFilter.AddRule(tStickDownRule);

            AnalogStickInputFilter::Rule tStickUpRule;
            tStickUpRule.id = RULE_STICK_UP;
            tStickUpRule.vecMinStickPos.Set(-FLT_MAX, flDeadZoneVal);
            tStickUpRule.vecMaxStickPos.Set(FLT_MAX, FLT_MAX);
            tDefaultFilter.AddRule(tStickUpRule);
            break;
        }

        case FILTER_STRICT_VERTICAL: {
            AnalogStickInputFilter::Rule tStickDownRule;
            tStickDownRule.id = RULE_STICK_DOWN;
            tStickDownRule.vecMinStickPos.Set(-flDeadZoneVal, -FLT_MAX);
            tStickDownRule.vecMaxStickPos.Set(flDeadZoneVal, -flDeadZoneVal);
            tDefaultFilter.AddRule(tStickDownRule);

            AnalogStickInputFilter::Rule tStickUpRule;
            tStickUpRule.id = RULE_STICK_UP;
            tStickUpRule.vecMinStickPos.Set(-flDeadZoneVal, flDeadZoneVal);
            tStickUpRule.vecMaxStickPos.Set(flDeadZoneVal, FLT_MAX);
            tDefaultFilter.AddRule(tStickUpRule);
            break;
        }
    }

    return tDefaultFilter;
}

}  // namespace Console
