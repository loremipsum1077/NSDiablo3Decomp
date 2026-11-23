// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdPublisherVariables/bdPublisherVariables.h

#pragma once

#include "../../bdCore/bdReference/bdReference.h"
#include "../../bdJSON/bdJSONValue.h"
#include "bdPublisherVariablesInfo.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdRemoteTaskManager;
class bdJSONObject;

class bdPublisherVariables {  // size 0x58
public:
    BD_CLASS_ALLOCATOR(bdPublisherVariables);
    ~bdPublisherVariables();
    static const bdUInt32 BD_MAX_NAMESPACE_LEN = 16;
    static const bdUInt32 BD_MAX_VALUE_LEN = 32;
    enum bdPublisherVariablesStatus {
        BD_NOT_RETRIEVED = 0,
        BD_NOT_AVAILABLE = 1,
        BD_BAD_JSON_DATA = 2,
        BD_AVAILABLE = 3,
    };
    bdPublisherVariables::bdPublisherVariablesStatus getVariable(const bdNChar8 *const, const bdNChar8 *const, bdNChar8 *const, bdUInt32) const;
    bdPublisherVariables::bdPublisherVariablesStatus getVersions(const bdNChar8 *const, bdUInt16 &, bdUInt16 &);
    // Invented: bdPublisherVariablesInfo::deserialize calls the protected setData.
    friend class bdPublisherVariablesInfo;
protected:
    bdPublisherVariables(bdRemoteTaskManager *const, const bdNChar8 *);
    bdBool setData(const bdNChar8 *const nameSpace, bdJSONObject * jsonObj, const bdUInt16 majorVersion, const bdUInt16 minorVersion);
    bdRemoteTaskManager * m_remoteTaskManager;  // 0x00
    bdJSONValue m_json;  // 0x08
    bdJSONObject * m_jsonVariables;  // 0x18
    bdPublisherVariablesInfo m_refreshPublisherVariablesResult;  // 0x20
    bdNChar8 m_context[16];  // 0x40
    bdUInt16 m_currentMajorVersion;  // 0x50
    bdUInt16 m_currentMinorVersion;  // 0x52
};
static_assert(sizeof(bdPublisherVariables) == 0x58, "bdPublisherVariables");



// Functions defined in this file (bodies are not in the DWARF):
//   line    40  void bdPublisherVariables::operator delete(void * p)  [inlined into 1 functions]
//   line    40  void * bdPublisherVariables::operator new(size_t nbytes)  [inline]
