// Original file: X/Client/Definitions/ParticleDefinition.cpp

#include "ParticleDefinition.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include "../../Common/D3TypeDescriptor/D3TypeDescriptor.h"
#include "../../Common/Definitions/AttachmentDefinition.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"

// Declared here because their headers do not carry them yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;  // X/Sigma/Files/XFilesCommon.cpp:20
extern const XTypeDescriptor * DT_SNO;  // X/Common/D3TypeDescriptor/D3DescribedType.cpp
extern const XTypeDescriptor * DT_MASS;
extern const XTypeDescriptor * DT_INTERPOLATION_SCALAR;
extern const XTypeDescriptor * DT_INT_PATH;
extern const XTypeDescriptor * DT_TIME_PATH;
extern const XTypeDescriptor * DT_COLOR_PATH;
extern const XTypeDescriptor * DT_ANGLE_PATH;
extern const XTypeDescriptor * DT_VELOCITY_PATH;
extern const XTypeDescriptor * DT_VELOCITYVECTOR_PATH;
extern const XTypeDescriptor * DT_ANGULARVELOCITY_PATH;
extern const XTypeDescriptor * DT_ACCELVECTOR_PATH;
extern const XTypeDescriptor * DT_EXTENDED_PATH;
extern const XTypeDescriptor * DT_PARTICLE_QUAD;

// lines 44-51. Only the values are recorded (the field defaults and the GetDefaultData stores).
static const float cflDefaultMass = 1.0f / 32.2f;

static const float cflDefaultFoliageFrequency = 1.0f;
static const float cflDefaultFoliageDampingRatio = 0.3f;
static const float cflDefaultFoliageMaxOffsetFactor = 1.0f;
static const float cflDefaultFoliageOscWindFactor = 1.25f;
static const float cflDefaultFoliagePrevailingWindFactor = 0.0f;
static const float cflDefaultFoliageExplosionFactor = 1.0f;

// The field names are "" in the binary; float defaults are stored as bit patterns.
static const TypeDescriptorField EmitterParams_Fields[10] = {
    TypeDescriptorField("", DT_INT, offsetof(EmitterParams, eType), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EmitterParams, serPathRadius_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(EmitterParams, pathRadius), 0x700020, 0x18),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EmitterParams, serPathHeight_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(EmitterParams, pathHeight), 0x700020, 0x18),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(EmitterParams, serPathBoxExtents_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VECTOR_PATH, offsetof(EmitterParams, pathBoxExtents), 0x700020, 0x18),
    TypeDescriptorField("", DT_INT, offsetof(EmitterParams, dwSubObjectHash), NULL, 0x700000),
    TypeDescriptorField("", DT_INT, offsetof(EmitterParams, dwPad), NULL, 0x700000),
    TypeDescriptorField(NULL, DT_NULL, sizeof(EmitterParams), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_EmitterParams("EmitterParams", 1, EmitterParams_Fields, 10, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_EMITTER_PARAMS = &TypeDescriptor_EmitterParams;

static const TypeDescriptorField ExtendedPath_Console_Fields[25] = {
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathInitialSourceRelativeVelocity_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(ExtendedPath_Console, pathInitialSourceRelativeVelocity), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathInitialWorldRelativeVelocity_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(ExtendedPath_Console, pathInitialWorldRelativeVelocity), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathSystemSpeedSpawnRate_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ExtendedPath_Console, pathSystemSpeedSpawnRate), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathParticleAlphaAux_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ExtendedPath_Console, pathParticleAlphaAux), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathOrientationVelocity_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANGULARVELOCITY_PATH, offsetof(ExtendedPath_Console, pathOrientationVelocity), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathOrientationPosition_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ANGLE_PATH, offsetof(ExtendedPath_Console, pathOrientationPosition), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionCylinderRadialPos_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ExtendedPath_Console, pathMotionCylinderRadialPos), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionSphereRadialPos_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ExtendedPath_Console, pathMotionSphereRadialPos), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionSphereRadialVel_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ExtendedPath_Console, pathMotionSphereRadialVel), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionRelativeCartesianAcc_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_ACCELVECTOR_PATH, offsetof(ExtendedPath_Console, pathMotionRelativeCartesianAcc), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionAttractorVel_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ExtendedPath_Console, pathMotionAttractorVel), 0x20, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ExtendedPath_Console, serPathMotionAttractorPos_Console), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ExtendedPath_Console, pathMotionAttractorPos), 0x20, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ExtendedPath_Console), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ExtendedPath_Console("ExtendedPath_Console", 0, ExtendedPath_Console_Fields, 25, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_EXTENDED_PATH = &TypeDescriptor_ExtendedPath_Console;

static const TypeDescriptorField ParticleQuad_Fields[5] = {
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleQuad, flShrinkMinX), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleQuad, flShrinkMaxX), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleQuad, flShrinkMinY), NULL, 0),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleQuad, flShrinkMaxY), NULL, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ParticleQuad), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ParticleQuad("ParticleQuad", 0, ParticleQuad_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PARTICLE_QUAD = &TypeDescriptor_ParticleQuad;

static const TypeDescriptorField ParticleDefinition_Fields[78] = {
    TypeDescriptorField("", DT_INT, offsetof(ParticleDefinition, eType), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ParticleDefinition, dwFlags), NULL, 0x80001),
    TypeDescriptorField("", DT_TIME, offsetof(ParticleDefinition, tSystemDuration), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(ParticleDefinition, tOutroDuration), NULL, 1),
    TypeDescriptorField("", DT_TIME, offsetof(ParticleDefinition, tPrepumpDuration), NULL, 1),
    TypeDescriptorField("", DT_INTERPOLATION_SCALAR, offsetof(ParticleDefinition, tSystemDurationScalar), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(ParticleDefinition, dwPad), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathSystemScale_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathSystemScale), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathSystemCount_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_INT_PATH, offsetof(ParticleDefinition, pathSystemCount), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathSystemAlpha_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathSystemAlpha), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathParticleDuration_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_TIME_PATH, offsetof(ParticleDefinition, pathParticleDuration), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathInitialScale_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathInitialScale), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathInitialSourceRelativeSpread_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_ANGLE_PATH, offsetof(ParticleDefinition, pathInitialSourceRelativeSpread), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathSystemSpawnRate_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ParticleDefinition, pathSystemSpawnRate), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathSystemSpeedDurationFactor_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ParticleDefinition, pathSystemSpeedDurationFactor), 0x700020, 8),
    TypeDescriptorField("", DT_UBERMATERIAL, offsetof(ParticleDefinition, tUberMaterial), NULL, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ParticleDefinition, snoPhysics), "", 0x1c, 0, -1),
    TypeDescriptorField("", DT_MASS, offsetof(ParticleDefinition, flMass), (const void *)0x3cfe68f1, 1),
    TypeDescriptorField("", DT_INT, offsetof(ParticleDefinition, dwMaxSystems), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flClutterZOffset), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliageFrequency), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliageDampingRatio), (const void *)0x3e99999a, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliageMaxOffsetFactor), (const void *)0x3f800000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliageOscWindFactor), (const void *)0x3fa00000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliagePrevailingWindFactor), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flFoliageExplosionFactor), (const void *)0x3f800000, 1),
    FilterTypeDescriptorField("", DT_SNO, offsetof(ParticleDefinition, snoActor), "", 1, 0, -1),
    TypeDescriptorField("", DT_EMITTER_PARAMS, offsetof(ParticleDefinition, tEmitterParams), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathParticleAlpha_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathParticleAlpha), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathParticleColor_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_COLOR_PATH, offsetof(ParticleDefinition, pathParticleColor), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathParticleScale_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathParticleScale), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathParticleStretch_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_FLOAT_PATH, offsetof(ParticleDefinition, pathParticleStretch), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathAngularPosition_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_ANGLE_PATH, offsetof(ParticleDefinition, pathAngularPosition), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathAngularVelocity_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_ANGULARVELOCITY_PATH, offsetof(ParticleDefinition, pathAngularVelocity), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathOrientationAxis_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VECTOR_PATH, offsetof(ParticleDefinition, pathOrientationAxis), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCylinderRadialVel_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VELOCITY_PATH, offsetof(ParticleDefinition, pathMotionCylinderRadialVel), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCylinderAngleVel_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_ANGULARVELOCITY_PATH, offsetof(ParticleDefinition, pathMotionCylinderAngleVel), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCylinderAxis_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VECTOR_PATH, offsetof(ParticleDefinition, pathMotionCylinderAxis), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCartesianPos_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VECTOR_PATH, offsetof(ParticleDefinition, pathMotionCartesianPos), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCartesianVel_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(ParticleDefinition, pathMotionCartesianVel), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionCartesianAcc_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_ACCELVECTOR_PATH, offsetof(ParticleDefinition, pathMotionCartesianAcc), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionRelativeCartesianPos_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VECTOR_PATH, offsetof(ParticleDefinition, pathMotionRelativeCartesianPos), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serPathMotionRelativeCartesianVel_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_VELOCITYVECTOR_PATH, offsetof(ParticleDefinition, pathMotionRelativeCartesianVel), 0x700020, 8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serExtendedPath_Console), NULL, 0x700000),
    VariableArrayTypeDescriptorField("", DT_EXTENDED_PATH, offsetof(ParticleDefinition, ptExtendedPath_Console), 0x700020, 8),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, flJitterApproachSpeed), (const void *)0x3c23d70a, 1),
    TypeDescriptorField("", DT_INT, offsetof(ParticleDefinition, nFacing), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, wdBoundingRadius), (const void *)0x41200000, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ParticleDefinition, fAttachedToCameraDistanceFactor), (const void *)0x3f4ccccd, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serMsgTriggeredEvents), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_MSGTRIGGEREDEVENT, offsetof(ParticleDefinition, ptMsgTriggeredEvents), 0x21, -0xc),
    VariableArrayCountTypeDescriptorField("", 0x2b4, 0x10000, DT_MSGTRIGGEREDEVENT, -8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ParticleDefinition, serParticleQuadStructure), NULL, 0x400000),
    VariableArrayTypeDescriptorField("", DT_PARTICLE_QUAD, offsetof(ParticleDefinition, ptParticleQuadStructure), 0x400020, 8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ParticleDefinition), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ParticleDefinition("Particle", 11, ParticleDefinition_Fields, 78, NULL, 0x80, NULL, NULL, NULL, NULL);
const XTypeDescriptor * DT_PARTICLESYSTEM = &TypeDescriptor_ParticleDefinition;


ParticleGroupDef * gptGroupDefParticle;  // line 216

// line 222
ParticleGroupDef::ParticleGroupDef()
    : GroupDefinition("Particle", PARTICLE_SNO,
                      SNOVersions(180, 213, 192, 192, 213),
                      SNODATATYPE_TITLE, sizeof(ParticleDefinition), DT_PARTICLESYSTEM, 0x2041, 0x500a)
{
}

// line 246
uint32 ParticleGroupDef::GetMaxCount()
{
    return 384;
}

// line 258
static float sComputeMaxParticleSize(ParticleDefinition * pdefParticle)
{
    float flSystemScaleMax = 1.0f;
    if (pdefParticle->pathSystemScale)
    {
        float flSystemScaleMin;
        pdefParticle->pathSystemScale->GetMinMaxValues(flSystemScaleMin, flSystemScaleMax);
    }
    float flInitialScaleMax = 1.0f;
    if (pdefParticle->pathInitialScale)
    {
        float flInitialScaleMin;
        pdefParticle->pathInitialScale->GetMinMaxValues(flInitialScaleMin, flInitialScaleMax);
    }
    float flParticleScaleMax = 1.0f;
    if (pdefParticle->pathParticleScale)
    {
        float flParticleScaleMin;
        pdefParticle->pathParticleScale->GetMinMaxValues(flParticleScaleMin, flParticleScaleMax);
    }
    return flSystemScaleMax * Math::Max(flInitialScaleMax, flParticleScaleMax);
}

// line 285
static float sGetParticleSystemBoundingRadius(ParticleDefinition * pdefParticle)
{
    float flBoundingRadius;
    switch (pdefParticle->eType)
    {
    case PSTYPE_ACTOR:
    case PSTYPE_PHYS_ACTOR:
    case PSTYPE_FOLIAGE_ACTOR:
        return pdefParticle->wdBoundingRadius;
    }

    switch (pdefParticle->tEmitterParams.eType)
    {
    case 1:
        flBoundingRadius = 1.0f;
        break;
    case 4:
    case 10:
    {
        float flRadiusMax = 0.0f;
        if (pdefParticle->tEmitterParams.pathRadius)
        {
            float flRadiusMin;
            pdefParticle->tEmitterParams.pathRadius->GetMinMaxValues(flRadiusMin, flRadiusMax);
        }
        flBoundingRadius = flRadiusMax;
        break;
    }
    case 5:
    case 9:
    {
        float flRadiusMax = 0.0f;
        if (pdefParticle->tEmitterParams.pathRadius)
        {
            float flRadiusMin;
            pdefParticle->tEmitterParams.pathRadius->GetMinMaxValues(flRadiusMin, flRadiusMax);
        }
        float flHeightMax = 0.0f;
        if (pdefParticle->tEmitterParams.pathHeight)
        {
            float flHeightMin;
            pdefParticle->tEmitterParams.pathHeight->GetMinMaxValues(flHeightMin, flHeightMax);
        }
        flBoundingRadius = Math::Max(flRadiusMax, flHeightMax);
        break;
    }
    default:
        return pdefParticle->wdBoundingRadius;
    }

    flBoundingRadius += sComputeMaxParticleSize(pdefParticle);

    return Math::Min(flBoundingRadius, pdefParticle->wdBoundingRadius);
}

// line 358
void ParticleGroupDef::Postprocess(void * lpDefinition)
{
    GroupDefinition::Postprocess(lpDefinition);

    ParticleDefinition * ptParticleDef = (ParticleDefinition *)lpDefinition;
    if (ptParticleDef->tHeader.dwFlags & (1 << 5))
    {
        return;
    }
    ptParticleDef->wdBoundingRadius = sGetParticleSystemBoundingRadius(ptParticleDef);
}

// line 713
BOOL ParticleGroupDef::Write(void * lpDefinition, FileReference & tRef) const
{
    return GroupDefinition::Write(lpDefinition, tRef);
}

// line 745
void ParticleGroupDef::GetDefaultData(void * ptDefaultDef) const
{
    ParticleDefinition * pdef = (ParticleDefinition *)ptDefaultDef;
    SigmaMemoryClear(pdef, sizeof(ParticleDefinition));

    pdef->eType = PSTYPE_BILLBOARD;
    pdef->dwFlags = (1 << PARTICLE_DEF_INFINITE_DURATION_BIT) | (1 << PARTICLE_DEF_CREATE_SYSTEM_WHEN_OFFSCREEN) | (1 << PARTICLE_DEF_DELETE_PARTICLES_WHEN_OFFSCREEN);
    pdef->tSystemDuration = 60;
    pdef->tOutroDuration = 30;
    pdef->tPrepumpDuration = 0;
    pdef->tSystemDurationScalar.nScalarFunction = SCALAR_FUNCTION_NONE;
    pdef->tSystemDurationScalar.flScalarLerpZero = 0.0f;
    pdef->tSystemDurationScalar.flScalarLerpOne = 0.0f;
    pdef->pathSystemScale = NULL;
    pdef->pathSystemCount = NULL;
    pdef->pathSystemAlpha = NULL;
    pdef->pathParticleDuration = NULL;
    pdef->pathInitialScale = NULL;
    pdef->pathInitialSourceRelativeSpread = NULL;
    pdef->pathSystemSpawnRate = NULL;
    pdef->pathSystemSpeedDurationFactor = NULL;
    MaterialSetDefaultValues(pdef->tUberMaterial);
    pdef->snoPhysics = SNO_NONE;
    pdef->flMass = cflDefaultMass;
    pdef->flFoliageFrequency = cflDefaultFoliageFrequency;
    pdef->flFoliageDampingRatio = cflDefaultFoliageDampingRatio;
    pdef->flFoliageMaxOffsetFactor = cflDefaultFoliageMaxOffsetFactor;
    pdef->flFoliageOscWindFactor = cflDefaultFoliageOscWindFactor;
    pdef->flFoliagePrevailingWindFactor = cflDefaultFoliagePrevailingWindFactor;
    pdef->flFoliageExplosionFactor = cflDefaultFoliageExplosionFactor;
    pdef->snoActor = SNO_NONE;
    pdef->tEmitterParams.eType = 4;
    pdef->tEmitterParams.pathRadius = NULL;
    pdef->tEmitterParams.pathHeight = NULL;
    pdef->tEmitterParams.pathBoxExtents = NULL;
    pdef->pathParticleColor = NULL;
    pdef->pathParticleAlpha = NULL;
    pdef->pathParticleScale = NULL;
    pdef->pathParticleStretch = NULL;
    pdef->pathAngularPosition = NULL;
    pdef->pathAngularVelocity = NULL;
    pdef->pathOrientationAxis = NULL;
    pdef->pathMotionCylinderRadialVel = NULL;
    pdef->pathMotionCylinderAngleVel = NULL;
    pdef->pathMotionCylinderAxis = NULL;
    pdef->pathMotionCartesianPos = NULL;
    pdef->pathMotionCartesianVel = NULL;
    pdef->pathMotionCartesianAcc = NULL;
    pdef->pathMotionRelativeCartesianPos = NULL;
    pdef->pathMotionRelativeCartesianVel = NULL;
    pdef->ptExtendedPath_Console = NULL;
    pdef->serParticleQuadStructure.dwOffset = 0;
    pdef->serParticleQuadStructure.dwSize = 0;
    pdef->ptParticleQuadStructure = NULL;
    pdef->flJitterApproachSpeed = 0.01f;
    pdef->nFacing = PARTICLE_FACING_BILLBOARD;
    pdef->wdBoundingRadius = 10.0f;
    pdef->fAttachedToCameraDistanceFactor = 0.8f;
}
