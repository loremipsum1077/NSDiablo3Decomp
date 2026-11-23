#include "bdPublisherVariablesInfo.h"
#include "bdPublisherVariables.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdJSON/bdJSONValue.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"













bdPublisherVariablesInfo::~bdPublisherVariablesInfo() {}



bdBool bdPublisherVariablesInfo::deserialize(bdByteBufferRef buffer)
{
    bdNChar8 nameSpace[bdPublisherVariables::BD_MAX_NAMESPACE_LEN];
    bdUInt16 majorVersion = 0;
    bdUInt16 minorVersion = 0;

    bdJSONValue json;
    bdBool ok = buffer->readUInt16(majorVersion)
        && buffer->readUInt16(minorVersion);
    ok = ok && buffer->readString(nameSpace, sizeof(nameSpace));

    const bdNChar8 *ptr = (const bdNChar8 *)buffer->getReadPtr();
    if (!ok ||
        *ptr != 16 ||

        !json.parse(ptr + 1, NULL)) {


        bdLogWarn("bdPublisherVariablesInfo", "Failed to parse publisher variables.");
        return false;
    }




    if (m_requestingService) {


        bdJSONObject *jsonObj = json.getObject();
        return m_requestingService->setData(nameSpace, jsonObj, majorVersion, minorVersion);
    }
    return false;
}



bdUInt bdPublisherVariablesInfo::sizeOf() { return sizeof(bdPublisherVariablesInfo); }
