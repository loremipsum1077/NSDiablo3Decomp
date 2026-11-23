// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdObjectStore/bdObjectStoreObjectID.h

#pragma once

#include "../../bdCore/bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdJSONDeserializer;
class bdJSONSerializer;
class bdUserAccountID;

class bdObjectStoreObjectID {  // size 0x50
public:
    enum bdObjectStoreObjectIDOwnerType {
        USER_OWNER_TYPE = 0,
        PUBLISHER_OWNER_TYPE = 1,
    };
public:
    BD_CLASS_ALLOCATOR(bdObjectStoreObjectID);
    bdObjectStoreObjectID(const bdNChar8 *const, const bdNChar8 *const, bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType);
    bdObjectStoreObjectID(const bdUserAccountID &, const bdNChar8 *const);
    bdObjectStoreObjectID();
    void reset();
    bdBool setOwner(const bdNChar8 *, bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType);
    bdBool getOwnerString(bdNChar8 * owner, bdUInt ownerSize) const;
    bdBool serializeToJSON(bdJSONSerializer *) const;
    bdBool deserializeFromMetadataJSON(bdJSONDeserializer *, bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType);
    const bdNChar8 * getOwner() const;
    const bdNChar8 * getObjectName() const;
    bdNChar8 * getObjectName();
    bdUInt64 getID() const;
    bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType getOwnerType() const;
protected:
    bdObjectStoreObjectID::bdObjectStoreObjectIDOwnerType m_ownerType;  // 0x00
    bdNChar8 m_owner[30];  // 0x04
    bdNChar8 m_objectName[32];  // 0x22
    bdUInt64 m_ID;  // 0x48
    bdBool hasAccountType(const bdUserAccountID &);
    bdBool validateOwnerString(const bdNChar8 * owner);
    bdBool validateUserIDOwnerString(const bdNChar8 *);
    bdBool setOwnerStringForPublisher(const bdNChar8 * owner);
    bdBool setOwnerStringFromUserAccountID(const bdUserAccountID &);
    bdBool setOwnerStringFromUserString(const bdNChar8 * owner);
};
static_assert(sizeof(bdObjectStoreObjectID) == 0x50, "bdObjectStoreObjectID");

