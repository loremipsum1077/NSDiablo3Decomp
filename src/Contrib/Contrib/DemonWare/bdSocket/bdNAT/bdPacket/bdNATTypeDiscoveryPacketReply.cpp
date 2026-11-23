#include "bdNATTypeDiscoveryPacketReply.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
bdNATTypeDiscoveryPacketReply::bdNATTypeDiscoveryPacketReply():m_type(0),m_protocolVersion(0) {}
bdBool bdNATTypeDiscoveryPacketReply::deserialize(const void *data,const bdUInt size,const bdUInt offset,bdUInt &newOffset) {
 newOffset=offset;
 bdBool status=bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_type);
 if (m_type <20 || m_type >29) {newOffset=offset;return false;}
 {
 status=status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 if (m_protocolVersion!=2) {
 status=status && bdBytePacker::rewindBytes(data,size,newOffset,newOffset,2);
 status=status && bdBytePacker::skipBytes(data,size,newOffset,newOffset,1);
 status=status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 if (m_protocolVersion!=1) {newOffset=offset;return false;}
 }
 status=status && m_mappedAddr.deserialize(data,size,newOffset,newOffset);
 if (m_protocolVersion==1) status=status && bdBytePacker::skipBytes(data,size,newOffset,newOffset,2);
 status=status && m_secAddr.deserialize(data,size,newOffset,newOffset);
 }
 if (!status) newOffset=offset;
 return status;
}
bdUByte8 bdNATTypeDiscoveryPacketReply::getType() const {return m_type;}
const bdAddr &bdNATTypeDiscoveryPacketReply::getMappedAddr() const {return m_mappedAddr;}
const bdAddr &bdNATTypeDiscoveryPacketReply::getSecAddr() const {return m_secAddr;}
