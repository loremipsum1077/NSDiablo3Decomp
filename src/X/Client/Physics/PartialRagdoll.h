// Minimal layout adopted from X/Client/Physics/PartialRagdoll.h DWARF.
#pragma once
#include "Ragdoll.h"
#include "../../Common/CommonTypes.h"

class PartialRagdoll : public Ragdoll {  // size 0x3098
public:
    PartialRagdoll(RActorID);
    virtual ~PartialRagdoll();  // vtable slot 0
    virtual BOOL ShouldUpdateBounds() const;  // vtable slot 4
private:
    PartialRagdoll(const PartialRagdoll &);
    void operator=(const PartialRagdoll &);
};
static_assert(sizeof(PartialRagdoll) == 0x3098, "PartialRagdoll");
