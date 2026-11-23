#include "bdIPDiscoveryPacketReply.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
bdIPDiscoveryPacketReply::bdIPDiscoveryPacketReply():m_type(0),m_protocolVersion(0) {}
bdBool bdIPDiscoveryPacketReply::deserialize(const void *data,const bdUInt size,const bdUInt offset,bdUInt &newOffset) {
 newOffset=offset;
 bdBool status=bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_type);
 if (m_type <30 || m_type >39) {newOffset=offset;return false;}
 {
 status=status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 if (m_protocolVersion!=2 && m_protocolVersion!=3) {
 status=status && bdBytePacker::rewindBytes(data,size,newOffset,newOffset,2);
 status=status && bdBytePacker::skipBytes(data,size,newOffset,newOffset,1);
 status=status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 if (m_protocolVersion!=1) {newOffset=offset;return false;}
 }
 status=status && m_addr.deserialize(data,size,newOffset,newOffset);
 if (m_protocolVersion>2) {
 status=status && m_protectedAddr.deserialize(data,size,newOffset,newOffset);
 applyXorToAddr(m_protectedAddr);
 }
 if (m_protocolVersion==1) status=status && bdBytePacker::skipBytes(data,size,newOffset,newOffset,2);
 }
 if (!status) newOffset=offset;
 return status;
}
const bdAddr &bdIPDiscoveryPacketReply::getAddr() const {return m_addr;}
const bdAddr &bdIPDiscoveryPacketReply::getProtectedAddr() const {return m_protectedAddr;}
bdUInt16 bdIPDiscoveryPacketReply::getProtocolVersion() const {return m_protocolVersion;}
inline void bdIPDiscoveryPacketReply::applyXorToAddr(bdAddr &addr) const {
 addr.getHash();
 bdInetAddr temp_addr(addr.getAddress());
 temp_addr.set(temp_addr.toUInt32() ^ 0x2112a442);
 addr.set(temp_addr,addr.getPort() ^ 0x2112);
}
