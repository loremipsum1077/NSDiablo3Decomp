// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdLobby/bdRichPresence/bdRichPresenceData.h

#pragma once

#include "../../bdCore/bdSocket/bdThreadedStreamSocket.h"
#include "../bdLobbyCommon/bdSerializable.h"
#include "../bdLobbyCommon/bdTaskResult.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include <stddef.h>
#include <stdint.h>
class bdByteBuffer;

typedef bdUByte8 bdRichPresenceStatus;

typedef bdUByte8 bdRichPresenceSource;

static const bdUInt BD_MAX_USER_RICH_PRESENCE_DATA_LENGTH = 128;
enum bdRichPresenceStatuses {
    BD_RICH_PRESENCE_STATUS_OFFLINE = 0,
    BD_RICH_PRESENCE_STATUS_ONLINE = 1,
    BD_RICH_PRESENCE_STATUS_BUSY = 2,
    BD_RICH_PRESENCE_STATUS_IDLE = 3,
    BD_RICH_PRESENCE_STATUS_DO_NOT_DISTURB = 4,
    BD_RICH_PRESENCE_STATUS_HIDDEN = 5,
    BD_RICH_PRESENCE_STATUS_RESERVED_MAX = 31,
    BD_RICH_PRESENCE_STATUS_MAX = 255,
};

enum bdRichPresenceSources {
    BD_RICH_PRESENCE_SOURCE_SUBSCRIPTION = 0,
    BD_RICH_PRESENCE_SOURCE_FRIENDS = 1,
    BD_RICH_PRESENCE_SOURCE_TEAMS = 2,
};

class bdRichPresenceData : public bdTaskResult, public bdSerializable {  // size 0xe0
public:
    BD_CLASS_ALLOCATOR(bdRichPresenceData);
    bdRichPresenceData();
    bdRichPresenceData(const bdRichPresenceStatus, const bdUByte8 *const, const bdUInt);
    virtual bdUInt sizeOf();  // vtable slot 3
    virtual bdBool serialize(bdByteBuffer & buffer) const;  // vtable slot 4
    virtual bdBool deserialize(bdByteBufferRef buffer);  // vtable slot 2
    void setRichPresenceData(const bdUserAccountID * userAccountID, const bdRichPresenceStatus onlineStatus, const bdUByte8 *const presenceData, const bdUInt size);
    const bdUserAccountID & getUserAccountID() const;
    bdRichPresenceStatus getOnlineStatus() const;
    const bdUByte8 * getPresenceData() const;
    bdUInt getPresenceDataSize() const;
protected:
    bdUserAccountID m_userAccountID;  // 0x10
    bdRichPresenceStatus m_onlineStatus;  // 0x48
    bdUByte8 m_presenceData[128];  // 0x49
    bdUInt m_presenceDataSize;  // 0xcc
};
static_assert(sizeof(bdRichPresenceData) == 0xe0, "bdRichPresenceData");


// Functions defined in this file (bodies are not in the DWARF):
//   line    47  bdRichPresenceData::bdRichPresenceData(const bdRichPresenceData &)  [inlined into 1 functions]
//   line    47  bdRichPresenceData::~bdRichPresenceData()  [0x7100b40750 (+1 more), inlined into 8 functions]
//   line    50  void bdRichPresenceData::operator delete(void * p)  [inlined into 1 functions]

// Variables defined in this file:
//   line    19  static const bdUInt BD_MAX_USER_RICH_PRESENCE_DATA_LENGTH
