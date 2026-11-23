// Subset of X/Client/ParticleUpdate.h, plus the declarations of the X/Client/ParticleUpdate.cpp
// functions that Particle.cpp calls (the reference header does not list them).
// Original file: X/Client/ParticleUpdate.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Sigma/SigmaTypes.h"

struct ParticleSystem;
struct Particle;
struct KeyedMessage;

typedef void (*DeletePartCallback)(ParticleSystem *, Particle *, BOOL);

struct ParticleParams;
struct Vector3D;

// X/Client/ParticleUpdate.cpp:954
BOOL ParticleSystemUpdateParams(ParticleSystem * ptParticleSystem, const BOOL fInitialUpdate, const DataID idInstance, ParticleParams & tParams, Vector3D & wpParticle, BOOL fFlagDeletionOnly, BOOL bUpdateVisibleParams, float flDeltaTime);

// X/Client/ParticleUpdate.cpp:33
void ParticleAttachmentsOnMsg(ParticleSystem & tParticleSystem, const DataID idAttachedTo, const KeyedMessage & tMsg);
