#include "bdSessionParams.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"







bdSessionParams::bdSessionParams() {}



bdSessionParams::~bdSessionParams() {}



bdBool bdSessionParams::serialize(bdByteBuffer &) const { return true; }





bdUInt bdSessionParams::sizeOf()
{
    bdUseVAArgs();
    return sizeof(bdSessionParams);
}
