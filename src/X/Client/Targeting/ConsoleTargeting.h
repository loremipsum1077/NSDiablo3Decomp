// Original file: X/Client/Targeting/ConsoleTargeting.h

#pragma once

#include "../../Common/WorldPlace.h"
#include "../../Sigma/Math/MathGeneral.h"

struct Player;
struct PowerDefinition;

namespace Targeting { namespace Console {











enum TargetingType { TARGETING_PRIMARY = 0, TARGETING_SECONDARY = 1, TARGETING_STICKY = 2 };






struct TargetingInformationInput
{
    const Player * ptPlayer;
    const PowerDefinition * pdefPower;
    TargetingType eTargetingType;
    // Inference: constructor member initialization follows stores in Player.cpp:1148.
    TargetingInformationInput(const Player * player, const PowerDefinition * ptPower, const TargetingType targetingType)
        : ptPlayer(player), pdefPower(ptPower), eTargetingType(targetingType) {}
};

static_assert(sizeof(TargetingInformationInput) == 0x18, "TargetingInformationInput");

struct TargetingInformationOutput
{
    Angle aSearchAngle;
    float flSearchRadius;
    float flControllerAutoTargetingCloseRange;
    float flControllerAutoTargetingCloseAngle;
    WorldPlace wpConeStart;
};

static_assert(sizeof(TargetingInformationOutput) == 0x20, "TargetingInformationOutput");



} }
