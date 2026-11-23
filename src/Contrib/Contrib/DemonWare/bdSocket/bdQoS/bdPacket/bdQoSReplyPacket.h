// Adopted from DWARF; declarations checked against original layout.
// Original file: Contrib/Contrib/DemonWare/bdSocket/bdQoS/bdPacket/bdQoSReplyPacket.h

#pragma once

#include "../../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdQoSReplyPacket {  // size 0x38
public:
    bdQoSReplyPacket();
    bdQoSReplyPacket(bdUInt32, bdUInt64);
    ~bdQoSReplyPacket();
    bdBool setData(const bdUByte8 *, bdUInt);
    void setId(bdUInt32 id);
    void setTimestamp(bdUInt64 timestamp);
    void setEnabled(bdBool);
    void setBandwidth(bdUInt bandwidth);
    void setHostProcessingTime(bdFloat32 hostProcessingTime);
    void setHasData(bdBool hasData);
    bdUByte8 getType() const;
    bdFloat32 getElapsedTime() const;
    bdUInt32 getId() const;
    bdUByte8 * getData();
    bdUInt32 getDataSize() const;
    bdBool getEnabled() const;
    bdUInt32 getBandwidth() const;
    bdBool getHasData() const;
    bdUInt getSerializedSize();
    bdBool serialize(void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset) const;
    bdBool deserialize(const void * data, const bdUInt size, const bdUInt offset, bdUInt & newOffset);
protected:
    bdUInt getHeaderSize();
    bdUByte8 m_type;  // 0x00
    bdUInt32 m_id;  // 0x04
    bdUInt64 m_timestamp;  // 0x08
    bdBool m_enabledMode;  // 0x10
    bdUInt32 m_bandwidth;  // 0x14
    bdFloat32 m_hostProcessingTime;  // 0x18
    bdUByte8 * m_data;  // 0x20
    bdUInt32 m_dataSize;  // 0x28
    static bdUInt headerSize;
    bdUInt m_serializedSize;  // 0x2c
    bdBool m_hasData;  // 0x30
};
static_assert(sizeof(bdQoSReplyPacket) == 0x38, "bdQoSReplyPacket");

