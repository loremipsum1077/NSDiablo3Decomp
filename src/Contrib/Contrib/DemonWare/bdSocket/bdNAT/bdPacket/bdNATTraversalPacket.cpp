#include "bdNATTraversalPacket.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
#include "../../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
bdUInt bdNATTraversalPacket::serializedSize = 0;
bdNATTraversalPacket::bdNATTraversalPacket() : m_type(0), m_protocolVersion(0), m_ident(0) {}
bdNATTraversalPacket::bdNATTraversalPacket(const bdUByte8 &type, const bdUInt32 ident, const bdAddr &addrSrc, const bdAddr &addrDest)
 : m_type(type), m_protocolVersion(2), m_ident(ident), m_addrSrc(addrSrc), m_addrDest(addrDest) {}
bdBool bdNATTraversalPacket::serialize(void *data, const bdUInt size, const bdUInt offset, bdUInt &newOffset) const {
 newOffset = offset;
 bdBool status = bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_type)
 && bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_protocolVersion)
 && bdBytePacker::appendBuffer(data,size,newOffset,newOffset,m_hmac,10)
 && bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_ident)
 && m_addrSrc.serialize(data,size,newOffset,newOffset)
 && m_addrDest.serialize(data,size,newOffset,newOffset);
 if (!status) newOffset = offset;
 return status;
}
bdBool bdNATTraversalPacket::deserialize(const void *data, const bdUInt32 size, const bdUInt offset, bdUInt &newOffset) {
 newOffset = offset;
 bdBool status = true;
 status = status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_type);
 status = status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 // Original 71:5: the version compare is tested before status, then an else clears status (78-82).
 if ((m_protocolVersion >= 2) && status) {
  status = status && bdBytePacker::removeBuffer(data,size,newOffset,newOffset,m_hmac,10);
  status = status && bdBytePacker::removeBasicType(data,size,newOffset,newOffset,m_ident);
  status = status && m_addrSrc.deserialize(data,size,newOffset,newOffset);
  status = status && m_addrDest.deserialize(data,size,newOffset,newOffset);
 } else {
  status = false;
 }
 if (!status) newOffset = offset;
 return status;
}
bdUInt bdNATTraversalPacket::getSize() const { if (!serializedSize) serialize(0,65535,0,serializedSize); return serializedSize; }
bdUByte8 bdNATTraversalPacket::getType() const { return m_type; }
void bdNATTraversalPacket::setType(bdUByte8 type) { m_type=type; }
bdUInt32 bdNATTraversalPacket::getIdentifier() const { return m_ident; }
const bdAddr &bdNATTraversalPacket::getAddrDest() const { return m_addrDest; }
const bdAddr &bdNATTraversalPacket::getAddrSrc() const { return m_addrSrc; }
const bdUByte8 *bdNATTraversalPacket::getHMAC() const { return m_hmac; }
void bdNATTraversalPacket::setHMAC(const bdUByte8 *hmac,const bdUInt size) { bdMemcpy(m_hmac,hmac,10); }
