// Layout check for X/Client/RActors/RActors.h against the DWARF (byte_size and
// data_member_location of every type in it). Compiled, not compared: it defines no function
// the binary has. RActorGet / RActorTryToGet are the same text as the stand-ins the matched
// files (Gizmo.cpp, CGame.cpp, InterpolationScalar.cpp, ...) already carry.
#include "X/Client/RActors/RActors.h"

#include <cstddef>

#define CHECK(T, m, off) static_assert(offsetof(T, m) == off, #T "::" #m)

static_assert(sizeof(CollisionCapsuleInstance) == 0x30, "CollisionCapsuleInstance");
CHECK(CollisionCapsuleInstance, ptSphereNode, 0x20);
CHECK(CollisionCapsuleInstance, idRActor, 0x28);
CHECK(CollisionCapsuleInstance, nCollisionCapsuleIndex, 0x2c);

static_assert(sizeof(RActorTilt) == 0x24, "RActorTilt");
static_assert(sizeof(HitJitter) == 0x4, "HitJitter");
static_assert(sizeof(RActorSink) == 0x8, "RActorSink");
static_assert(sizeof(RActorGeoSetData) == 0x10, "RActorGeoSetData");
CHECK(RActorGeoSetData, ptSubObjectState, 0x08);

static_assert(sizeof(RActor) == 0x410, "RActor");
CHECK(RActor, idSScene, 0x04);
CHECK(RActor, idACD, 0x08);
CHECK(RActor, snoActor, 0x0c);
CHECK(RActor, snoMonster, 0x10);
CHECK(RActor, tLocation, 0x18);
CHECK(RActor, transformClient, 0x88);
CHECK(RActor, mWorld, 0xa8);
CHECK(RActor, snoAppearance, 0xe8);
CHECK(RActor, nLookNative, 0xec);
CHECK(RActor, nLookActive, 0xf0);
CHECK(RActor, snoPhysMesh, 0xf4);
CHECK(RActor, tParentData, 0xf8);
CHECK(RActor, ptParticleParams, 0x128);
CHECK(RActor, ptEnvAppSphereNode, 0x130);
CHECK(RActor, wpWalkingPos, 0x138);
CHECK(RActor, ptActorAnim, 0x148);
CHECK(RActor, ptCActorMovement, 0x150);
CHECK(RActor, aYaw, 0x158);
CHECK(RActor, flLife, 0x15c);
CHECK(RActor, flTriggerFadePerTick, 0x160);
CHECK(RActor, nFadeGroupSubObjectIndex, 0x164);
CHECK(RActor, flAlpha, 0x168);
CHECK(RActor, flFadeAlpha, 0x16c);
CHECK(RActor, flTriggerFadeAlpha, 0x170);
CHECK(RActor, eOverlayType, 0x174);
CHECK(RActor, dwOverlayFadeTimeCurrent, 0x178);
CHECK(RActor, dwOverlayFadeTimeOriginal, 0x17c);
CHECK(RActor, tName, 0x180);
CHECK(RActor, dwLastFrameSubmitted, 0x184);
CHECK(RActor, dwCreationTick, 0x188);
CHECK(RActor, flDeathTime, 0x18c);
CHECK(RActor, dwSActorCreationTick, 0x190);
CHECK(RActor, flTimeLastHit, 0x194);
CHECK(RActor, flTimeLastFlinch, 0x198);
CHECK(RActor, flHitFlashAddTimeRemaining, 0x19c);
CHECK(RActor, flHitFlashAddDuration, 0x1a0);
CHECK(RActor, rgbavalHitFlashAdd, 0x1a4);
CHECK(RActor, flHitFlashMulTimeRemaining, 0x1b4);
CHECK(RActor, flHitFlashMulDuration, 0x1b8);
CHECK(RActor, rgbavalHitFlashMul, 0x1bc);
CHECK(RActor, dwFlags, 0x1cc);
CHECK(RActor, dwFlagsEx, 0x1d0);
CHECK(RActor, dwLightFilterFlags, 0x1d4);
CHECK(RActor, dwVisibilityFlags, 0x1d8);
CHECK(RActor, flLastLOSCheckTime, 0x1dc);
CHECK(RActor, fLastLOSCheckResult, 0x1e0);
CHECK(RActor, snoPhysics, 0x1e4);
CHECK(RActor, ptRigidBody, 0x1e8);
CHECK(RActor, flForceMultiplier, 0x1f0);
CHECK(RActor, tRMsgList, 0x1f8);
CHECK(RActor, tAttachmentList, 0x230);
CHECK(RActor, idTalkSound, 0x258);
CHECK(RActor, nTrackedEffectGroupID, 0x25c);
CHECK(RActor, eContext, 0x260);
CHECK(RActor, wdSimpleShadowRadius, 0x264);
CHECK(RActor, snoIrradianceTex, 0x268);
CHECK(RActor, snoIrradianceTexHighlight, 0x26c);
CHECK(RActor, snoAmbientOcclusionOverrideTex, 0x270);
CHECK(RActor, flRand, 0x274);
CHECK(RActor, tGeoSetData, 0x278);
CHECK(RActor, ptRigidAnim, 0x298);
CHECK(RActor, tTilt, 0x2a0);
CHECK(RActor, tHitJitter, 0x2c4);
CHECK(RActor, tSink, 0x2c8);
CHECK(RActor, tCollisionCapsuleList, 0x2d0);
CHECK(RActor, bBoundingBoxCreated, 0x2f8);
CHECK(RActor, wvMinBounds, 0x2fc);
CHECK(RActor, wvMaxBounds, 0x308);
CHECK(RActor, rgbaColor, 0x314);
CHECK(RActor, eItemEffectType, 0x318);
CHECK(RActor, nItemEffectLevel, 0x31c);
CHECK(RActor, ePickStyle, 0x320);
CHECK(RActor, eHairStyle, 0x324);
CHECK(RActor, rgbavalSceneCurrent, 0x328);
CHECK(RActor, rgbavalSceneTarget, 0x338);
CHECK(RActor, ptEffectAttachments, 0x348);
CHECK(RActor, ptLookOverrideMap, 0x350);
CHECK(RActor, idMarker, 0x358);
CHECK(RActor, ptBehavior, 0x360);
CHECK(RActor, tAimTarget, 0x368);
CHECK(RActor, wvVelocity, 0x384);
CHECK(RActor, wpLastUpdate, 0x390);
CHECK(RActor, dwRActorLastSeen, 0x39c);
CHECK(RActor, tMarkerOwner, 0x3a0);
CHECK(RActor, tMinimapInfo, 0x3ac);
CHECK(RActor, idRopeAttachment, 0x3b4);
CHECK(RActor, flShear, 0x3b8);
CHECK(RActor, flStretch, 0x3bc);
CHECK(RActor, flBaseScale, 0x3c0);
CHECK(RActor, flTimeUntilFade, 0x3c4);
CHECK(RActor, flFadeTime, 0x3c8);
CHECK(RActor, flFloatingNumberHealAccumulator, 0x3cc);
CHECK(RActor, uNextFloatingNumberHealTime, 0x3d0);
CHECK(RActor, tObjTexAnimParam, 0x3d4);
CHECK(RActor, dwDebugFlags, 0x3d8);
CHECK(RActor, flLookAtTargetIntensity, 0x3dc);
CHECK(RActor, uLookAtTargetExpireTime, 0x3e0);
CHECK(RActor, flLastRegisterTime, 0x3e4);
CHECK(RActor, nSimTreeDepth, 0x3e8);
CHECK(RActor, posMonsterCorpseList, 0x3f0);
CHECK(RActor, ptBannerRenderData, 0x3f8);
CHECK(RActor, ptOcclusionHighlighting, 0x400);
CHECK(RActor, bShowLocatorBeam, 0x408);

// X/Client/TexAnimation.h
static_assert(sizeof(TexAnimState) == 0x48, "TexAnimState");
CHECK(TexAnimState, aRot, 0x14);
CHECK(TexAnimState, snoTex, 0x1c);
CHECK(TexAnimState, tAnimState, 0x20);
CHECK(TexAnimState, ptRActor, 0x38);
CHECK(TexAnimState, nBone, 0x40);
static_assert(sizeof(ObjectTexAnimParam) == 0x4, "ObjectTexAnimParam");
static_assert(sizeof(TextureFramesInfo) == 0x18, "TextureFramesInfo");
CHECK(TextureFramesInfo, ptTexFrames, 0x10);
