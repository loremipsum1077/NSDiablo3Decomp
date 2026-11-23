// Adopted from the DWARF: bdLobbyCommon/bdTaskResult.h.
#pragma once
#include "../../bdCore/bdReference/bdReferencable.h"
#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
class bdTaskResult : public virtual bdReferencable {
public:
    BD_CLASS_ALLOCATOR(bdTaskResult);
    bdTaskResult();
    virtual ~bdTaskResult();
    virtual bdBool deserialize(bdByteBufferRef) = 0;
    virtual bdUInt sizeOf() = 0;
protected:
    bdTaskResult(const bdTaskResult &);
};
static_assert(sizeof(bdTaskResult) == 0x18, "bdTaskResult");
