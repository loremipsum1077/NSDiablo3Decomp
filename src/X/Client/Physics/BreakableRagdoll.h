// Minimal layout adopted from X/Client/Physics/BreakableRagdoll.h DWARF.
#pragma once
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "Ragdoll.h"
#include "../../Common/CommonTypes.h"

struct ContactPoint;
struct ExplosionImpulse;
struct Structure;

class BreakableRagdoll : public Ragdoll {  // size 0x30a0
public:
    BreakableRagdoll(RActorID);
    virtual ~BreakableRagdoll();  // vtable slot 0
    virtual BOOL ShouldAnimate() const;  // vtable slot 3
    virtual BOOL ShouldUpdateBounds() const;  // vtable slot 4
    virtual void ApplyExplosion(const ExplosionImpulse & tExplosion, float flExplosionFactor);  // vtable slot 5
    virtual void ApplyCollision(const ContactPoint * ptPoint);  // vtable slot 6
    void Break();
    BOOL IsBroken() const;
private:
    BreakableRagdoll(const BreakableRagdoll &);
    void operator=(const BreakableRagdoll &);
    void Split(int32 nSplitIndex);
    void CreateConstraint(const Structure * ptStructure, int32 nBoneIndex);
    void CreateSubtreeBody(int32 nBoneIndex);
    void RecreateSubtreeBody(const Structure *, int32);
    BOOL m_bIsBroken;  // 0x3094
    BOOL m_bHasLeaves;  // 0x3098
    BOOL m_bHasDynamicBodies;  // 0x309c
};
static_assert(sizeof(BreakableRagdoll) == 0x30a0, "BreakableRagdoll");
