// Original file: X/Client/ParticleCommon.cpp

#include "ParticleInternal.h"
#include "Definitions/ParticleDefinition.h"

void ParticleGetInterpTime(
    const ParticleSystem * ptParticleSystem,
    const ParticleParams & tParams,
    const Vector3D & wpParticle,
    InterpTime & tInterpTime) {
    tInterpTime.Init();
    tInterpTime.flTime = tParams.flSeconds;
    tInterpTime.flScale = tParams.flDuration;

    if (ptParticleSystem->eType == PSTYPE_CLUTTER_BILLBOARD ||
        ptParticleSystem->eType == PSTYPE_CLUTTER_DYNAMIC) {
        tInterpTime.tCachedScalarParams.flMsgScalar = tParams.tCreateParams.flDensity;
    } else {
        tInterpTime.tCachedScalarParams.flMsgScalar = ptParticleSystem->flMsgScalar;
    }

    if (ptParticleSystem->dwDefFlags & (1 << PARTICLE_DEF_DELETE_PARTICLE_AT_DURATION_END_BIT)) {
        tInterpTime.nTimeType = TICK_TIME_NON_LOOPING;
    } else if (ptParticleSystem->bOutro) {
        tInterpTime.flRatio = ptParticleSystem->flOutroCrossFade;
        tInterpTime.flOutro = ptParticleSystem->flOutroCrossFade;
        tInterpTime.nTimeType = TICK_TIME_OUTRO_CROSSFADE;
    } else {
        tInterpTime.nTimeType = TICK_TIME_LOOPING;
    }

    if (EPSILON < ptParticleSystem->flEmitterRadiusMax) {
        tInterpTime.tCachedScalarParams.flRadialDistance =
            Vector3D::Distance(ptParticleSystem->wp, wpParticle) / ptParticleSystem->flEmitterRadiusMax;
    }
    if (EPSILON < ptParticleSystem->flEmitterHeightMax) {
        tInterpTime.tCachedScalarParams.flFractionalHeight =
            (wpParticle.z - ptParticleSystem->wp.z) / ptParticleSystem->flEmitterHeightMax;
    }
}
