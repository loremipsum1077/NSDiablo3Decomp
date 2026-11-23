// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreMetadata.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "bdObjectStoreACL.h"
#include "bdObjectStoreObjectID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONDeserializer;
class bdJSONSerializer;
class bdObjectStoreHTTPHeader;

class bdObjectStoreMetadata {  // size 0x3d8
public:
    BD_CLASS_ALLOCATOR(bdObjectStoreMetadata);
    bdObjectStoreMetadata(const bdObjectStoreObjectID &);
    bdObjectStoreMetadata(const bdObjectStoreObjectID &, const bdNChar8 *const, const bdInt64, const bdObjectStoreACL &, const bdUByte8 *, bdUInt);
    bdObjectStoreMetadata();
    void reset();
    bdBool deserializeFromJSON(bdJSONDeserializer *, bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType);
    bdBool serializeToHTTPHeader(bdObjectStoreHTTPHeader *) const;
    bdBool serializeToJSON(bdJSONSerializer *, const bdNChar8 *) const;
    const bdNChar8 * getContext() const;
    const bdObjectStoreObjectID & getObjectID() const;
    const bdNChar8 * getContentChecksum() const;
    const bdNChar8 * getObjectVersion() const;
    const bdNChar8 * getCategory() const;
    const bdNChar8 * getContentURL() const;
    const bdUByte8 * getExtraData() const;
    bdUInt getExtraDataSize() const;
    bdInt64 getExpiresOn() const;
    bdUInt64 getContentLength() const;
    const bdObjectStoreACL & getACL() const;
    bdInt64 getCreated() const;
    bdInt64 getModified() const;
    bdBool setObjectID(const bdObjectStoreObjectID &);
    bdBool setCategory(const bdNChar8 *);
protected:
    bdNChar8 m_context[16];  // 0x00
    bdObjectStoreObjectID m_objectID;  // 0x10
    bdNChar8 m_contentChecksum[17];  // 0x60
    bdNChar8 m_objectVersion[33];  // 0x71
    bdInt64 m_expiresOn;  // 0x98
    bdUInt64 m_contentLength;  // 0xa0
    bdObjectStoreACL m_acl;  // 0xa8
    bdInt64 m_created;  // 0xb0
    bdInt64 m_modified;  // 0xb8
    bdNChar8 m_contentURL[512];  // 0xc0
    bdNChar8 m_category[20];  // 0x2c0
    bdUByte8 m_extraData[256];  // 0x2d4
    bdUInt m_extraDataSize;  // 0x3d4
};
static_assert(sizeof(bdObjectStoreMetadata) == 0x3d8, "bdObjectStoreMetadata");

