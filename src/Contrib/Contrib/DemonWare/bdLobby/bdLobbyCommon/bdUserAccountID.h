// Generated from the DWARF by tools/gen_headers.py -- reference only.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdUserAccountID.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "bdLobbyServicesCommon.h"
#include "bdSerializable.h"
#include "bdTaskResult.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>

class bdByteBuffer;
class bdJSONDeserializer;
class bdStructBufferDeserializer;
class bdStructBufferSerializer;

class bdUserAccountID : public bdTaskResult, public bdSerializable {  // size 0x38
public:
    BD_CLASS_ALLOCATOR(bdUserAccountID);
    bdUserAccountID();
    bdUserAccountID(const bdUserAccountID &);
    bdUserAccountID(const bdOnlineUserID &);
    void reset();
    bdUserAccountID(const bdOnlineUserID &, const bdNChar8 *const);
    virtual ~bdUserAccountID();  // vtable slot 0
    bdUserAccountID & operator=(const bdUserAccountID & other);
    bool operator==(const bdUserAccountID & other) const;
    bool operator!=(const bdUserAccountID & other) const;
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    bdNChar8 m_accountType[10];  // 0x10
    bdOnlineUserID m_userID;  // 0x20
};
static_assert(sizeof(bdUserAccountID) == 0x38, "bdUserAccountID");

typedef bdUserAccountID bdUserID;

class ContextSerialization {  // size 0x1
public:
    static void * operator new(size_t);
    static void operator delete(void *);
    static void * operator new[](size_t);
    static void operator delete[](void *);
    static void * operator new(size_t, void *);
    static void operator delete(void *, void *);
    static void * operator new[](size_t, void *);
    static void operator delete[](void *, void *);
    static bdUInt getUserIDSize(const bdUserAccountID & userID);
    static bdUInt getUserIDSize(const bdOnlineUserID &);
    static bdUInt getContextSize(const bdNChar8 *);
    static bdBool writeUserID(bdByteBufferRef buffer, const bdUserAccountID & userID);
    static bdBool writeUserID(bdByteBufferRef, const bdOnlineUserID &);
    static bdBool writeContext(bdByteBufferRef buffer, const bdNChar8 * context);
    static bdBool readContext(bdByteBufferRef buffer, bdNChar8 * context, const bdUInt32 contextBufferSize);
    static bdBool readUserID(bdByteBufferRef buffer, bdUserAccountID & userID);
    static bdBool readUserID(bdByteBufferRef, bdOnlineUserID &);
    static bdBool readUserID(bdJSONDeserializer &, bdUserAccountID &, const bdNChar8 *const, const bdNChar8 *const);
    static bdBool readUserID(bdJSONDeserializer &, bdOnlineUserID &, const bdNChar8 *const, const bdNChar8 *const);
};
static_assert(sizeof(ContextSerialization) == 0x1, "ContextSerialization");


// Functions defined in this file (bodies are not in the DWARF):
//   line    50  void bdUserAccountID::operator delete(void * p)  [inlined into 1 functions]
//   line    77  bdStructUserAccountID & bdStructUserAccountID::operator=(const bdStructUserAccountID &)  [inline]
//   line    77  bdStructUserAccountID::~bdStructUserAccountID()  [inline]
//   line    80  void bdStructUserAccountID::operator delete(void * p)  [inline]
