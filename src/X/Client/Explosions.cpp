// Original file: X/Client/Explosions.cpp
// Functions sit on their original line numbers (2.6.2 DWARF).
#include <float.h>
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "Explosions.h"
#include "AttachmentParent.h"
#include "CGame.h"
#include "Cloth.h"
#include "Particle.h"
#include "Rope.h"
#include "RActors/RActors.h"
#include "RActors/RActorAnimation.h"
#include "Behavior/Behavior.h"
#include "Physics/Ragdoll.h"
#include "Physics/BreakableRagdoll.h"
#include "Physics/DominoBody.h"
#include "Definitions/AppearanceDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameConstants.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/MemoryPools.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/Random.h"
// Declared here because no header in src declares them yet (owners: RActorCollision.cpp:1041, Rope.cpp:1952/1982, Particle.cpp:6519, Particle.cpp ParticleSystemExplodeFoliage).
void RActorsIntersectSphere(const Sphere & tSphere, const RActorID idIgnore, RActorIDListPooled & listActors); void RopeIntersectSphere(const Sphere & tSphere, XPooledDataIDList & listRope); void RopeApplyExplosion(const RopeID idRope, const ExplosionImpulse & tExplosion);
void ParticleSystemsIntersectSphere(const Sphere & tSphere, XPooledDataIDList & listParticleSystems); void ParticleSystemExplodeFoliage(const ExplosionImpulse & tExplosion);
static const int32 cnExplosionEntryDuration = 20;



Vector3D ExplosionImpulse::ComputeImpulse(
    const Vector3D & wp) const
{
    Vector3D wvImpulse;

    float flScale;
    float flDistance = Vector3D::Distance(wp, m_wpCenter);
    if (flDistance < m_flMinRadius)
    {
        flScale = 1.0f;
    }
    else if (m_flDeltaRadius > FLT_EPSILON && flDistance < m_flMinRadius + m_flDeltaRadius)
    {
        flScale = 1.0f - (flDistance - m_flMinRadius) / m_flDeltaRadius;
    }
    else
    {
        return cgwvNull;
    }

    switch (m_eType)
    {
    case EXPLOSION_TYPE_PARENT_DIRECTION:
    case EXPLOSION_TYPE_WORLD_DIRECTION:
        wvImpulse = flScale * m_flMagnitude * m_wvDirection;
        break;

    case EXPLOSION_TYPE_RADIAL:
        {
            Vector3D wvRadial = wp - m_wpCenter;
            wvRadial.Normalize();
            wvImpulse = flScale * m_flMagnitude * wvRadial;
        }
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    return wvImpulse;
}
// X/Client/Physics/BreakableRagdoll.h:86 (body line 89); the adopted header lacks the inline body. Written from its two inlined copies in this file (both load m_bIsBroken at 0x3094).
inline BOOL BreakableRagdoll::IsBroken() const { return m_bIsBroken; }
// Owner-local recovery of Explosions.h:55-92 (the adopted header omits these; Attachments.cpp does the same).
struct Explosion {  // size 0x80
    void Initialize(SNO snoExplosion, const PRTransform & transform, const RenderContext eContext); void UpdatePropertiesFromParent(); void Update();
    ExplosionID id;  // 0x00
    SNO m_snoExplosion;  // 0x04
    float m_flMinScale; float m_flDeltaScale; float m_flChance; float m_flDuration;  // 0x08-0x14
    Vector3D m_wvLocalDirection;  // 0x18
    ExplosionImpulse m_tImpulse;  // 0x24
    ParentData m_tParentData;  // 0x54
};
static_assert(sizeof(Explosion) == 0x80, "Explosion");
class ExplosionManager {  // size 0xa818
public:
    static void InitializeForGame(); static void DisposeForGame();
    ExplosionID Create(SNO snoExplosion, const PRTransform & transform, const RenderContext eContext);
    void Destroy(ExplosionID id); void SetParent(ExplosionID id, const ParentData & tParentData);
    Explosion * Get(ExplosionID id); Explosion * TryToGet(ExplosionID id);
    void Update(float flDeltaTime); void DrawDebugView(RWindow &);
    enum { MAX_EXPLOSIONS = 128, };
private:
    TDataArray<Explosion, int> * m_ptExplosions;  // 0x00
    ExplosionEntry m_pEntryPool[128];  // 0x08
    ExplosionEntry * m_pFreeEntryList;  // 0xa408
    ExplosionEntry * m_pEntries[128];  // 0xa410
    int32 m_nEntryCount;  // 0xa810
};
static_assert(sizeof(ExplosionManager) == 0xa818, "ExplosionManager");


void ExplosionEntry::Initialize(
    SNO snoExplosion,
    const ExplosionImpulse & tExplosion,
    float flScale)
{
    m_snoExplosion = snoExplosion;
    m_nDuration = cnExplosionEntryDuration;

    m_tImpulse = tExplosion;
    m_tImpulse.m_flMagnitude *= flScale;


    AppGlobals & tAppGlobals = sgtAppGlobals;
    if (tAppGlobals.flExplosionMagnitude > 0.0f)
    {
        m_tImpulse.m_flMagnitude = tAppGlobals.flExplosionMagnitude;
    }

    m_nActorCount = 0;


    Sphere ws;
    ws.wpCenter = m_tImpulse.m_wpCenter;
    ws.wdRadius = m_tImpulse.m_flMinRadius + m_tImpulse.m_flDeltaRadius;

    RActorIDListPooled listRActors(MemoryPoolsGetDATAIDListPool());
    RActorsIntersectSphere(ws, RACTORID_INVALID, listRActors);


    POS pos = listRActors.GetHeadPosition();
    while (pos)
    {
        RActorID idRActor = (RActorID)listRActors.GetNext(pos);
        RActor * ptRActor = RActorGet(idRActor);
        Vector3D wpActor = ptRActor->tLocation.tWorldLocation.wsBounds.wpCenter;
        Vector3D wvImpulse = m_tImpulse.ComputeImpulse(wpActor);
        float flImpulse;  // DWARF local (line 142) that leaves no code; its use is not recoverable (wvImpulse is likewise unused in the binary).







        BehaviorApplyExplosion(ptRActor->id, m_tImpulse);




        ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
        float flExplosionFactor = ptRActor->flForceMultiplier * pdefActor->flExplosionFactor;

        if (ptRActor->ptRigidBody)
        {
            if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)))
            {
                const float cflMaxRigidBodyVelocity = 50.0f;
                ptRActor->ptRigidBody->ApplyExplosion(m_tImpulse, flExplosionFactor, cflMaxRigidBodyVelocity);
            }
        }
        else if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
        {
            Ragdoll * ptRagdoll = ptRActor->ptActorAnim->ptRagdoll;
            switch (ptRagdoll->GetType())
            {
            case FULL_RAGDOLL_TYPE:
                if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)) || !(ptRActor->dwFlags & 0x2000))  // 0x2000: RActor dwFlags bit with no recovered name
                {
                    ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                    if (m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT))
                    {
                        ptRActor->dwFlags |= 0x2000;
                    }
                }
                break;

            case PARTIAL_RAGDOLL_TYPE:
                if (!m_tImpulse.IsMelee())
                {
                    ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                }


                if (!m_tImpulse.IsMelee() || !(ptRActor->dwFlags & 0x2000))
                {
                    if (m_nActorCount < ACTOR_BUFFER_SIZE)
                    {
                        m_idActorBuffer[m_nActorCount] = idRActor;
                        m_nActorCount++;
                    }
                }
                break;

            case BREAKABLE_RAGDOLL_TYPE:
                if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)) || !(ptRActor->dwFlags & 0x2000))
                {
                    BreakableRagdoll * ptBreakable = (BreakableRagdoll *)ptRagdoll;

                    if (ptBreakable->IsBroken())
                    {
                        ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                        if (m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT))
                        {
                            ptRActor->dwFlags |= 0x2000;
                        }
                    }
                    else
                    {
                        if (m_nActorCount < ACTOR_BUFFER_SIZE)
                        {
                            m_idActorBuffer[m_nActorCount] = idRActor;
                            m_nActorCount++;
                        }
                    }
                }
                break;
            case ATTACHED_RAGDOLL_TYPE:
                if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)))
                {
                    ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                }
                break;
            }
        }
        else if (ptRActor->snoAppearance != SNO_NONE && m_nActorCount < ACTOR_BUFFER_SIZE)
        {


            ScopedAppearanceDefinitionLockR pdefAppearance(ptRActor->snoAppearance);
            if (pdefAppearance->tStructure.dwFlags & (1 << STRUCTURE_CAN_RAGDOLL_BIT))
            {
                m_idActorBuffer[m_nActorCount] = idRActor;
                m_nActorCount++;
            }
        }
    }










    XPooledDataIDList listCloth(MemoryPoolsGetDATAIDListPool());
    ClothManager::IntersectSphere(ws, listCloth);

    pos = listCloth.GetHeadPosition();
    while (pos)
    {
        Cloth * ptCloth = ClothManager::Get(listCloth.GetNext(pos));
        ptCloth->ApplyExplosion(m_tImpulse);
    }


    XPooledDataIDList listRope(MemoryPoolsGetDATAIDListPool());
    RopeIntersectSphere(ws, listRope);

    pos = listRope.GetHeadPosition();
    while (pos)
    {
        RopeApplyExplosion(listRope.GetNext(pos), m_tImpulse);
    }


    XPooledDataIDList listParticleSystems(MemoryPoolsGetDATAIDListPool());
    ParticleSystemsIntersectSphere(ws, listParticleSystems);

    pos = listParticleSystems.GetHeadPosition();
    while (pos)
    {
        ParticleSystem * ptParticleSystem = ParticleSystemGet(listParticleSystems.GetNext(pos));
        ptParticleSystem->ApplyExplosion(m_tImpulse);
    }


    ParticleSystemExplodeFoliage(m_tImpulse);
}






void ExplosionEntry::Update()
{

    for (int32 i = 0; i < m_nActorCount; ++i)
    {
        BOOL bRemove = FALSE;

        RActor * ptRActor = RActorTryToGet(m_idActorBuffer[i]);
        if (!ptRActor)
            bRemove = TRUE;
        else if (ptRActor->ptActorAnim && ptRActor->ptActorAnim->ptRagdoll)
        {
            Ragdoll * ptRagdoll = ptRActor->ptActorAnim->ptRagdoll;

            switch (ptRagdoll->GetType())
            {
            case FULL_RAGDOLL_TYPE:
                if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)) || !(ptRActor->dwFlags & 0x2000))
                {
                    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
                    float flExplosionFactor = ptRActor->flForceMultiplier * pdefActor->flExplosionFactor;

                    ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                    if (m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT))
                    {
                        ptRActor->dwFlags |= 0x2000;
                    }
                }
                bRemove = TRUE;
                break;

            case BREAKABLE_RAGDOLL_TYPE:
                {


                    BreakableRagdoll * ptBreakable = (BreakableRagdoll *)ptRagdoll;
                    if (ptBreakable->IsBroken() == TRUE)
                    {
                        if (!(m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT)) || !(ptRActor->dwFlags & 0x2000))
                        {
                            ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
                            float flExplosionFactor = ptRActor->flForceMultiplier * pdefActor->flExplosionFactor;

                            ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                            if (m_tImpulse.m_dwFlags & (1 << EXPLOSION_MELEE_BIT))
                            {
                                ptRActor->dwFlags |= 0x2000;
                            }
                        }
                        bRemove = TRUE;
                    }
                }
                break;



            case ATTACHED_RAGDOLL_TYPE:
                {
                    ScopedActorDefinitionLockR pdefActor(ptRActor->snoActor);
                    float flExplosionFactor = ptRActor->flForceMultiplier * pdefActor->flExplosionFactor;
                    ptRagdoll->ApplyExplosion(m_tImpulse, flExplosionFactor);
                }
                break;
            }
        }








        if (bRemove)
        {
            m_idActorBuffer[i] = m_idActorBuffer[m_nActorCount - 1];
            i--;

            m_nActorCount--;
        }
    }

    m_nDuration--;
}



void Explosion::Initialize(
    SNO snoExplosion,
    const PRTransform & transform,
    const RenderContext eContext)
{
    m_snoExplosion = snoExplosion;

    const ExplosionDefinition * pdef = (const ExplosionDefinition *)gptGroupDefExplosion->LockRFast(snoExplosion);

    m_tImpulse.m_eContext = eContext;
    m_tImpulse.m_eType = pdef->eType;
    m_tImpulse.m_dwFlags = pdef->dwFlags;
    m_wvLocalDirection = pdef->wvDirection;
    if (pdef->eType == EXPLOSION_TYPE_PARENT_DIRECTION)
    {
        m_tImpulse.m_wvDirection = transform.q.Rotate(pdef->wvDirection);
    }
    else
    {
        m_tImpulse.m_wvDirection = pdef->wvDirection;
    }
    m_tImpulse.m_flMagnitude = pdef->flMagnitude;
    m_tImpulse.m_flMinRadius = pdef->flMinRadius;
    m_tImpulse.m_flDeltaRadius = pdef->flDeltaRadius;
    m_tImpulse.m_wpCenter = transform.wp;

    m_flMinScale = pdef->flMinScale;
    m_flDeltaScale = pdef->flDeltaScale;
    m_flDuration = (float)pdef->nDuration * OO_TICKS_PER_SECOND;
    m_flChance = pdef->flChance;

    m_tParentData.Init();
}





void Explosion::UpdatePropertiesFromParent()
{

    if (m_tParentData.m_eParentType != PARENT_NONE)
    {
        if (m_tParentData.ParentIsValid())
        {
            PRTransform transform;
            m_tParentData.GetTransform(transform);

            if (m_tImpulse.m_eType == EXPLOSION_TYPE_PARENT_DIRECTION)
            {
                m_tImpulse.m_wvDirection = transform.q.Rotate(m_wvLocalDirection);
            }
            m_tImpulse.m_wpCenter = transform.wp;
        }
    }
}

// Original copy was linker-removed (DWARF concrete DIE at low_pc 0, size 4); not declared inline (DW_INL_inlined).

void Explosion::Update()
{

    UpdatePropertiesFromParent();
}



void ExplosionManager::InitializeForGame()
{

    CGame & tGame = CGameGet();


    ExplosionManager * ptManager = (ExplosionManager *)SigmaMemoryNew(sizeof(ExplosionManager), 4, NULL, TRUE);

    tGame.ptExplosionManager = ptManager;

    ptManager->m_ptExplosions = TDataArrayCreateTracked<Explosion, ExplosionID>("Explosion", AppGetCurrentMemoryInterface(), MAX_EXPLOSIONS);

    for (int32 i = 0; i < MAX_EXPLOSIONS - 1; ++i)
    {
        ptManager->m_pEntryPool[i].m_pNext = ptManager->m_pEntryPool + i + 1;
    }
    ptManager->m_pEntryPool[MAX_EXPLOSIONS - 1].m_pNext = NULL;
    ptManager->m_pFreeEntryList = &ptManager->m_pEntryPool[0];
    ptManager->m_nEntryCount = 0;
}



void ExplosionManager::DisposeForGame()
{

    CGame & tGame = CGameGet();
    TDataArrayDispose(tGame.ptExplosionManager->m_ptExplosions);
    SigmaMemoryFree(tGame.ptExplosionManager, NULL); tGame.ptExplosionManager = NULL;
}



ExplosionID ExplosionManager::Create(
    SNO snoExplosion,
    const PRTransform & transform,
    const RenderContext eContext)
{
    ExplosionID id = m_ptExplosions->ElementNew();
    if (id == EXPLOSIONID_INVALID)
    {
        return id;  // id is EXPLOSIONID_INVALID here; returning id (not the constant) matches the original register use
    }



    Explosion * ptExplosion = m_ptExplosions->ElementGet(id);
    ptExplosion->Initialize(snoExplosion, transform, eContext);

    return id;
}


void ExplosionManager::SetParent(
    ExplosionID id,
    const ParentData & tParentData)
{
    Explosion * ptExplosion = m_ptExplosions->ElementGet(id);
    ptExplosion->m_tParentData = tParentData;
}



void ExplosionManager::Destroy(
    ExplosionID id)
{
    if (id == EXPLOSIONID_INVALID)
    {
        return;
    }

    m_ptExplosions->ElementDeleteByID(id);
}



Explosion * ExplosionManager::Get(
    ExplosionID id)
{
    return m_ptExplosions->ElementGet(id);
}



Explosion * ExplosionManager::TryToGet(
    ExplosionID id)
{
    return m_ptExplosions->ElementTryToGet(id);
}



void ExplosionManager::Update(
    float flDeltaTime)
{

    for (int32 i = 0; i < m_nEntryCount; ++i)
    {
        m_pEntries[i]->Update();
    }


    for (int32 i = 0; i < m_nEntryCount; ++i)
    {
        if (m_pEntries[i]->m_nDuration <= 0)
        {
            m_pEntries[i]->m_pNext = m_pFreeEntryList;
            m_pFreeEntryList = m_pEntries[i];
            m_pEntries[i] = m_pEntries[m_nEntryCount - 1];
            i--;
            m_nEntryCount--;
        }
    }



    RNG & rng = CGameGlobalsGet().rngLocal;

    TDataArrayIterator<Explosion> tIt;
    while (m_ptExplosions->Iterate(tIt))
    {
        Explosion * ptExplosion = tIt.ptElement;
        ExplosionID id = ptExplosion->id;
        ptExplosion->Update();
        if (id != ptExplosion->id)
        {
            continue;
        }


        if (RandFloat_Internal(rng) * 100.0f <= ptExplosion->m_flChance)
        {
            float flScale = RandRangeFloat_Internal(rng, ptExplosion->m_flMinScale, ptExplosion->m_flDeltaScale);

            if (m_pFreeEntryList)
            {
                ExplosionEntry * pEntry = m_pFreeEntryList;
                m_pFreeEntryList = pEntry->m_pNext;
                m_pEntries[m_nEntryCount] = pEntry;
                m_nEntryCount++;
                pEntry->m_pNext = NULL;

                pEntry->Initialize(ptExplosion->m_snoExplosion, ptExplosion->m_tImpulse, flScale);
            }
        }









        if (ptExplosion->m_flDuration > 0.0f)
        {
            ptExplosion->m_flDuration = Math::Max(ptExplosion->m_flDuration - flDeltaTime, 0.0f);
        }
        else
        {
            m_ptExplosions->ElementDelete(ptExplosion);
        }
    }
}
