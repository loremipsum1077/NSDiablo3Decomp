#include "RActors.h"
#include "RActorAnimation.h"
#include "../Physics/DominoBody.h"
#include "../Physics/DominoClient.h"
#include "../Physics/Ragdoll.h"
#include "../Physics/BreakableRagdoll.h"
#include "../Physics/AttachedRagdoll.h"
#include "../Definitions/AppearanceDefinition.h"
#include "../Definitions/PhysicsDefinition.h"
#include "../Definitions/PreferencesDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/AnimTreeDefinition.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Marker/Marker.h"
#include "../LookAt.h"
#include "../SubObjectGfx.h"
#include "../Cloth.h"
#include "../CPlayer.h"
#include "../../Sigma/Math/Sphere.h"

// DWARF FullRagdoll.h:26, matching the local definition in FullRagdoll.cpp.
class FullRagdoll : public Ragdoll {
public:
    FullRagdoll(RActorID, int32);
    virtual ~FullRagdoll();
private:
    FullRagdoll(const FullRagdoll&);
    void operator=(const FullRagdoll&);
};
// Existing original functions whose shared declarations are not adopted yet.
void RActorSetOrientation(RActor*, const Quaternion&, const ActorDefinition*, const ActorCommonData*);
BOOL RActorAnimationIsAnimTreeEnabled(const RActor&);
uint32 RActorGetAnimTreeLayerHash(AnimLayer);
void AnimTreeLayerEnableFullRagdoll(AnimTree&, const AnimTreeDefinition&, uint32);

// Ragdoll.h:147, body evidenced by this CU's SetAnimationOverride caller.
inline void Ragdoll::SetAnimationOverride(BOOL bEnable)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    m_bAnimationOverride = bEnable;
}
// NON_MATCHING: 1224 vs 1228 bytes; on DWARF lines; the physics lock is a direct LockRFast (no DefinitionLockR in the DWARF inline tree). Remaining: the two cgwvNull copies, original str/str/stp, ours stp/stp.
#line 43 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsInitialize(
    RActorID idRActor)
{
    RActor* ptRActor = RActorGet(idRActor);


    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    WorldLocation& loc = RActorGetWorldLocation(ptRActor);
    if (ptRActor->snoPhysics != -1)
    {
        RigidBodyParams tParams;

        const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
        const GeoSet& tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
        RActorGeoSetData& tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);

        tParams.m_bStatic = FALSE;
        tParams.m_materialOverride = pdefActor->eMaterial;
        tParams.m_transform = loc.transform;
        tParams.m_scale = loc.flScale;

        tParams.m_transform.q.Normalize();

        const PhysicsDefinition* pdefPhysics = (const PhysicsDefinition*)gptGroupDefPhysics->LockRFast(ptRActor->snoPhysics);
        tParams.m_friction = pdefPhysics->flFriction;
        tParams.m_rollingResistance = pdefPhysics->flRollingResistance;
        tParams.m_restitution = pdefPhysics->flRestitution;
        tParams.m_angularDamping = pdefPhysics->flAngularDamping;
        tParams.m_linearDamping = pdefPhysics->flLinearDamping;
        tParams.m_angularVel = cgwvNull;
        tParams.m_linearVel = cgwvNull;
        tParams.m_collisionCategory = 2;
        tParams.m_collisionMask = (pdefPhysics->wCollisionMask & ~0x40) | 0x20;
        tParams.m_bCollideWithSameActor = (pdefPhysics->dwFlags >> 17) & 1;
        tParams.m_bDisableSleep = (pdefPhysics->dwFlags >> 1) & 1;


        BOOL bFoundShape = FALSE;
        for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
        {
            SubObjectState& tSubObjectState = tGeoSetData.ptSubObjectState[i];

            if (!(tSubObjectState.dwFlags & 2))
                continue;



            SubObject* ptSubObject = &tGeoSet.ptSubObjects[i];
            if (ptSubObject->nShapeCount > 0)
            {
                bFoundShape = TRUE;
                break;
            }
        }

        if (bFoundShape)
        {
            ptRActor->ptRigidBody = gptPhysicsAPIImpl->CreateBodyFromSubObjects(tParams, idRActor);
        }
        else
        {



            AABB aabb;
            aabb.Set(cgwpNull, 0.0f);
            for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i)
            {
                SubObjectState& tSubObjectState = tGeoSetData.ptSubObjectState[i];

                if (tSubObjectState.dwFlags & 2)
                {



                    AABB::Union(aabb, tGeoSet.ptSubObjects[i].aabbBounds, aabb);
                }
            }
            if (aabb.wvExt.MagnitudeSquared() > 1e-6f)
            {
                ptRActor->ptRigidBody = gptPhysicsAPIImpl->CreateBodyFromAABB(tParams, aabb, idRActor);
            }
        }



    }


    else if(ptRActor->snoPhysMesh != -1)
    {
            ptRActor->ptRigidBody = gptPhysicsAPIImpl->CreateBodyFromPhysMesh(ptRActor->snoPhysMesh,
                loc.transform,
                    loc.flScale,
                MarkerNameGetString(ptRActor->tName));
    }

}
#line 145 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsDispose(RActor* ptRActor)
{
    if (ptRActor->ptRigidBody)
        gptPhysicsAPIImpl->DestroyBody(ptRActor->ptRigidBody);
}

#line 158 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsUpdate(
    RActor* ptRActor,
    const ActorDefinition* pdefActor,
    const ActorCommonData* ptACD)
{

    WorldLocation& loc = RActorGetWorldLocation(ptRActor);

    ptRActor->ptRigidBody->GetTransform(loc.transform);
    if (loc.transform.wp.z < -1000.0f)
    {
        loc.transform.wp.z = -1000.0f;
        ptRActor->ptRigidBody->SetTransform(loc.transform);
        ptRActor->ptRigidBody->SetType(RigidBody::STATIC);
    }

    RActorSetOrientation(ptRActor, loc.transform.q, pdefActor, ptACD);
}
void RActorDisableClothPlanes(RActor* ptRActor)
{
    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    const GeoSetType eGeoSetType = GEOSET_HIGHPOLY;
    const GeoSet& tGeoSet = pdefAppearance->tStructure.tGeoSets[eGeoSetType];
    RActorGeoSetData& tGeoSetData = ptRActor->tGeoSetData[eGeoSetType];
    for (int32 i = 0; i < tGeoSet.nSubObjectCount; ++i) {
        if (tGeoSetData.ptSubObjectState[i].idCloth != -1) {
            Cloth* ptCloth = ClothManager::Get(tGeoSetData.ptSubObjectState[i].idCloth);
            ptCloth->DisablePlanes();
        }
    }
}
#line 180 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsEnableRagdoll(
    RActor* ptRActor)
{
    if (ptRActor->ptActorAnim == NULL)
        return;




























	if (ptRActor->ptActorAnim->ptLookAt)
	{
		::delete ptRActor->ptActorAnim->ptLookAt;
		ptRActor->ptActorAnim->ptLookAt = NULL;
	}

    if (ptRActor->ptActorAnim->ptRagdoll)
    {
        if (ptRActor->ptActorAnim->ptRagdoll->GetType() == 3)
        {
            ::delete ptRActor->ptActorAnim->ptRagdoll;
            ptRActor->ptActorAnim->ptRagdoll = NULL;
        }
        else if (ptRActor->ptActorAnim->ptRagdoll->GetType() == 4)
        {

            BreakableRagdoll* ptBreakable = static_cast<BreakableRagdoll*>(ptRActor->ptActorAnim->ptRagdoll);
            ptBreakable->Break();

            if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
            {
                AnimationChannelEnableFullRagdoll(ptRActor->ptActorAnim->tAnimChannel);
            }
            else
            {
                static const uint32 dwDeathLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_DEATH);

                ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);

                AnimTreeLayerEnableFullRagdoll(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwDeathLayerHash);
            }

            return;
        }
        else
        {
            return;
        }
    }


    ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
    if (!(pdefAppearance->tStructure.dwFlags & 2))
        return;




    GameMode eGameMode = CGameGetGameCommonData().GameModeGet();
    if (eGameMode != 1 && eGameMode != 4 && eGameMode != 5)
        return;




    if (ptRActor->snoPhysics == -1)
        return;




    if (ptRActor->tParentData.m_eParentType != -1)
    {
        ptRActor->ptActorAnim->ptRagdoll = new AttachedRagdoll(ptRActor->id);
    }
    else
    {

		int32 nPhysicsQuality = Prefs::GetVideoPrefs().GetPhysicsQuality();


		// Inferred: the original passes the constant 1; any use of nPhysicsQuality left no code.
		int32 nDegrade = 1;







        RActorDisableClothPlanes(ptRActor);

        ptRActor->ptActorAnim->ptRagdoll = new FullRagdoll(ptRActor->id, nDegrade);
    }

    if (!RActorAnimationIsAnimTreeEnabled(*ptRActor))
    {
        AnimationChannelEnableFullRagdoll(ptRActor->ptActorAnim->tAnimChannel);
    }
    else
    {
        static const uint32 dwDeathLayerHash = RActorGetAnimTreeLayerHash(ANIM_LAYER_DEATH);

        ScopedAnimTreeDefinitionLockR pdefAnimTree(ptRActor->ptActorAnim->snoAnimTree);

        AnimTreeLayerEnableFullRagdoll(*ptRActor->ptActorAnim->ptAnimTree, *pdefAnimTree, dwDeathLayerHash);
    }
}
#line 314 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsSlowTime(const Sphere& ws, float flSlowExplosionFactor)
{
    const int32 cnMaxBodies = 256;
    RigidBody* aptBodies[cnMaxBodies];
    AABB aabb;
    aabb.wp = ws.wpCenter;
    aabb.wvExt.Set(ws.wdRadius, ws.wdRadius, ws.wdRadius);
    int32 nCount = gptPhysicsAPIImpl->QueryAABB(aptBodies, cnMaxBodies, aabb);
    for (int32 i = 0; i < nCount; ++i) {
        RigidBody* ptBody = aptBodies[i];
        if (ptBody->GetType() != RigidBody::DYNAMIC) continue;
        Vector3D wp = ptBody->GetCenterOfMassPosition();
        if (!ws.ContainsPoint(wp)) continue;
        if (flSlowExplosionFactor - ptBody->m_slowExplosionFactor <= 1e-6f) continue;
        ptBody->SetSleeping(FALSE);
        ptBody->m_slowExplosionFactor = flSlowExplosionFactor;
        Vector3D wvVelocity = ptBody->GetCenterOfMassVelocity();
        Vector3D wvAngularVelocity = ptBody->GetAngularVelocity();
        const float cflVelSlowFactor = 0.95f;
        const float cflGravityFactor = 0.8f;
        wvVelocity *= cflVelSlowFactor;
        wvAngularVelocity *= cflVelSlowFactor;
        ptBody->SetCenterOfMassVelocity(wvVelocity);
        ptBody->SetAngularVelocity(wvAngularVelocity);
        float flMass = ptBody->GetMass();
        Vector3D wvImpulse(0.0f, 0.0f, flMass * (32.0f * cflGravityFactor * (1.0f / 60.0f)));
        ptBody->ApplyImpulse(wvImpulse);
    }
}
#line 372 "X/Client/RActors/RActorPhysics.cpp"
void RActorApplyCollision(RActor* ptRActor, const ContactPoint* ptPoint)
{
    if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
        ptRActor->ptActorAnim->ptRagdoll->ApplyCollision(ptPoint);
}
#line 385 "X/Client/RActors/RActorPhysics.cpp"
Constraint* RActorPhysicsGetConstraint(const RActor* ptRActor, uint32 dwConstraintHash)
{
    if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
        return ptRActor->ptActorAnim->ptRagdoll->GetConstraint(dwConstraintHash);
    return 0;
}
#line 416 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsDisturbRigidBodies(const AABB& aabb, const Vector3D& wvVelocity)
{
    float vx = wvVelocity.x;
    float vy = wvVelocity.y;
    const float cflMinVelocitySquared = 10.0f;
    const float cnMaxDistance = 70.0f;
    if (vx * vx + vy * vy < cflMinVelocitySquared) return;
    ACDID idACDPlayer = LocalPlayerACDGet();
    Vector3D wpPlayer;
    if (idACDPlayer != ACDID_INVALID) {
        wpPlayer = ACDGet(idACDPlayer).GetWorldLocation().transform.wp;
        float flDistanceSquared = Vector3D::DistanceSquared(wpPlayer, aabb.wp);
        if (flDistanceSquared > cnMaxDistance * cnMaxDistance) return;
    }
    const int32 cnMaxBodies = 32;
    RigidBody* apBodyBuffer[cnMaxBodies];
    int32 nCount = gptPhysicsAPIImpl->QueryAABB(apBodyBuffer, cnMaxBodies, aabb);
    float flFactor = 0.2f;
    Vector3D wvParallel(vx, vy, 0.0f);
    wvParallel.Normalize();
    Vector3D wvPerpendicular(wvParallel.y, -wvParallel.x, 0.0f);
    for (int32 i = 0; i < nCount; ++i) {
        RigidBody* ptBody = apBodyBuffer[i];
        if (!(ptBody->m_dwFlags & 2)) continue;
        float flWeight = ptBody->GetMass() * 32.0f;
        Vector3D wpBody = ptBody->GetCenterOfMassPosition();
        Vector3D wvHorizontalDelta = wpBody - aabb.wp;
        wvHorizontalDelta.z = 0.0f;
        if (Vector3D::DotProduct(wvPerpendicular, wvHorizontalDelta) > 0.0f) {
            Vector3D wvImpulse = (wvParallel + wvPerpendicular) * (flWeight * flFactor * (1.0f / 60.0f));
            ptBody->ApplyImpulseToClosestPoint(wvImpulse, aabb.wp);
        }
        else {
            Vector3D wvImpulse = (wvParallel - wvPerpendicular) * (flWeight * flFactor * (1.0f / 60.0f));
            ptBody->ApplyImpulseToClosestPoint(wvImpulse, aabb.wp);
        }
    }
}
#line 505 "X/Client/RActors/RActorPhysics.cpp"
void RActorPhysicsSetAnimationOverride(RActor* ptRActor, BOOL bEnable)
{
    if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
        ptRActor->ptActorAnim->ptRagdoll->SetAnimationOverride(bEnable);
}
