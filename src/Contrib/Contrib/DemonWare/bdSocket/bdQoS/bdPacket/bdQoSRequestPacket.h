// Adopted from DWARF; declarations checked against the original layout.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdPacket/bdQoSRequestPacket.h

#pragma once

#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdQoSRequestPacket {  // size 0x20
public:
    bdQoSRequestPacket(bdUInt32, bdUInt32);
    bdQoSRequestPacket();
    void setRequestingData(bdBool requestingData);
    bdUByte8 getType() const;
    bdUInt64 getTimestamp() const;
    bdUInt32 getId() const;
    bdUInt32 getSecId() const;
    bdBool getRequestingData() const;
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
protected:
    bdUByte8 m_type;  // 0x00
    bdUInt64 m_timestamp;  // 0x08
    bdUInt32 m_id;  // 0x10
    bdUInt32 m_secid;  // 0x14
    bdBool m_requestingData;  // 0x18
};
static_assert(sizeof(bdQoSRequestPacket) == 0x20, "bdQoSRequestPacket");

