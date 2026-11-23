#include "bdNATTypeDiscoveryPacket.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
bdNATTypeDiscoveryPacket::bdNATTypeDiscoveryPacket(bdNATTypeDiscoveryPacketRequest request):m_type(20),m_protocolVersion(2),m_request(request) {}
bdBool bdNATTypeDiscoveryPacket::serialize(void *data,const bdUInt size,const bdUInt offset,bdUInt &newOffset) const {
 newOffset=offset;
 bdBool status=bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_type)
 && bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_protocolVersion)
 && bdBytePacker::appendBasicType(data,size,newOffset,newOffset,static_cast<signed char>(m_request));
 if (!status) newOffset=offset;
 return status;
}
