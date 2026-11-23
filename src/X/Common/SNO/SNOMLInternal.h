// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOMLInternal.h

#pragma once

namespace Blizzard { namespace Lock { struct RMutex; } }
class GroupDefinition;
class XTypeDescriptor;

// Preserve the original map alias without requiring its complete definition here.
template <typename Key, typename ARG_KEY, typename VALUE, typename ARG_VALUE> class XGrowableMap;
struct SNOName;
struct XMLFileData;
typedef XGrowableMap<SNOName, const SNOName &, XMLFileData, const XMLFileData &> MapSNOtoXMLFileData;

struct SNOMLGlobals {  // size 0x20
    MapSNOtoXMLFileData * pmapSNOtoXMLFileData;  // 0x00
    Blizzard::Lock::RMutex * ptXMLLock;  // 0x08
    GroupDefinition ** ptGameDefinitions;  // 0x10
    const XTypeDescriptor ** ptXMLGameDefinitions;  // 0x18
};
static_assert(sizeof(SNOMLGlobals) == 0x20, "SNOMLGlobals");
