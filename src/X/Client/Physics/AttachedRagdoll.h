// Adopted from DWARF; hand-maintained.
// Original file: X/Client/Physics/AttachedRagdoll.h

#pragma once

#include "Ragdoll.h"
#include "../../Common/CommonTypes.h"

class AttachedRagdoll : public Ragdoll {  // size 0x3098
public:
    AttachedRagdoll(RActorID);
    virtual ~AttachedRagdoll();  // vtable slot 0
    void Detach();
private:
    AttachedRagdoll(const AttachedRagdoll &);
    void operator=(const AttachedRagdoll &);
};
static_assert(sizeof(AttachedRagdoll) == 0x3098, "AttachedRagdoll");

