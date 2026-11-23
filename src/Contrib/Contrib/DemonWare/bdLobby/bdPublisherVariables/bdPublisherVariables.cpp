// Original file: Contrib/Contrib/DemonWare/bdLobby/bdPublisherVariables/bdPublisherVariables.cpp
#include "bdPublisherVariables.h"
#include "../../bdJSON/bdJSONObject.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"













static const bdNChar8 * BD_MAJOR_KEY = "MajorVersion";
static const bdNChar8 * BD_MINOR_KEY = "MinorVersion";

















bdPublisherVariables::~bdPublisherVariables()
{
}



























bdBool bdPublisherVariables::setData(const bdNChar8 *const nameSpace,
                                     bdJSONObject * jsonObj,
                                     const bdUInt16 majorVersion,
                                     const bdUInt16 minorVersion)
{
    const bdBool ok = (jsonObj != NULL);
    if (ok)
    {
        (*jsonObj)[BD_MAJOR_KEY] = static_cast<bdUInt64>(majorVersion);
        (*jsonObj)[BD_MINOR_KEY] = static_cast<bdUInt64>(minorVersion);
        (*m_jsonVariables)[nameSpace] = bdJSONValue(*jsonObj);
    }
    else
    {
        bdJSONValue empty(BD_JSON_OBJECT);
        (*m_jsonVariables)[nameSpace] = bdJSONValue(*empty.getObject());
        bdLogWarn("bdPublisherVariables", "No variables for namespace");
    }

    return ok;
}
