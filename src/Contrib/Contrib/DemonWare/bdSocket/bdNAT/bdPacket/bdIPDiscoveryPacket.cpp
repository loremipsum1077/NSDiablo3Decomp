#include "bdIPDiscoveryPacket.h"
#include "../../../bdCore/bdUtilities/bdBytePacker.h"
bdIPDiscoveryPacket::bdIPDiscoveryPacket(const bdUInt16 version):m_type(30),m_protocolVersion(version) {}
bdBool bdIPDiscoveryPacket::serialize(void *data,const bdUInt size,const bdUInt offset,bdUInt &newOffset) const {
 newOffset=offset;
 bdBool status=bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_type)
 && bdBytePacker::appendBasicType(data,size,newOffset,newOffset,m_protocolVersion);
 if (!status) newOffset=offset;
 return status;
}
