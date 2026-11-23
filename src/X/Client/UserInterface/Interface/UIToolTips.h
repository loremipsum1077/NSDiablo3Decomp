// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIToolTips.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"

typedef int32 AttribGroupID;

typedef int32 FontSpecifier;

typedef int32 ResourceType;

struct UIID;

// The tooltip callbacks UIToolTips.cpp defines; each fills the shared tooltip for the control `id`.
void UIToolTips_Nothing(const UIID & id);
void UIToolTips_HeroStrength(const UIID & id);
void UIToolTips_HirelingStrength(const UIID & id);
void UIToolTips_HeroStrengthBonus(const UIID & id);
void UIToolTips_HirelingStrengthBonus(const UIID & id);
void UIToolTips_HeroDexterity(const UIID & id);
void UIToolTips_HirelingDexterity(const UIID & id);
void UIToolTips_HeroDexterityBonus(const UIID & id);
void UIToolTips_HirelingDexterityBonus(const UIID & id);
void UIToolTips_HeroIntelligence(const UIID & id);
void UIToolTips_HirelingIntelligence(const UIID & id);
void UIToolTips_HeroIntelligenceBonus(const UIID & id);
void UIToolTips_HirelingIntelligenceBonus(const UIID & id);
void UIToolTips_HeroVitality(const UIID & id);
void UIToolTips_HirelingVitality(const UIID & id);
void UIToolTips_HeroVitalityBonus(const UIID & id);
void UIToolTips_HirelingVitalityBonus(const UIID & id);
void UIToolTips_HeroSurvivability(const UIID & id);
void UIToolTips_HeroDPS(const UIID & id);
void UIToolTips_HeroHealing(const UIID & id);
void UIToolTips_HirelingArmor(const UIID & id);
void UIToolTips_HirelingSurvivability(const UIID & id);
void UIToolTips_HirelingDPS(const UIID & id);
