// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdInitChunk.cpp
#include "bdInitChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
extern void bdUseVAArgs(...);

bdInitChunk::bdInitChunk() : bdChunk(BD_CT_INIT), m_initTag(0), m_flags(BD_IC_NO_FLAGS), m_windowCredit(0) {}
bdInitChunk::bdInitChunk(bdUInt32 initTag, const bdWord windowCredit) : bdChunk(BD_CT_INIT), m_initTag(initTag), m_flags(BD_IC_NO_FLAGS), m_windowCredit(windowCredit) {}
bdInitChunk::~bdInitChunk() {}
bdUInt32 bdInitChunk::getInitTag() const { return m_initTag; }
bdUInt bdInitChunk::serialize(bdUByte8* data, const bdUInt size) const {
    bdUInt offset = bdChunk::serialize(data, size);
    bdUInt16 length = 0;
    const bdUByte8 flags = (bdUByte8)m_flags;
    bdBool ok = bdBytePacker::appendBasicType(data, size, offset, offset, flags);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, length);
    ok = ok && bdBytePacker::appendBasicType(data, size, offset, offset, m_initTag);
    return offset;
}
bdBool bdInitChunk::deserialize(const bdUByte8* const data, const bdUInt size, bdUInt& offset) {
    bdUInt bytesRead = offset;
    bdBool ok = true;
    if (size - bytesRead > 4) {
        bdUByte8 flags = 0;
        bdUInt16 length;
        ok = bdChunk::deserialize(data, size, bytesRead);
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, flags);
        m_flags = (bdInitChunkFlags)flags;
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, length);
        ok = ok && bdBytePacker::removeBasicType(data, size, bytesRead, bytesRead, m_initTag);
    }
    if (ok) offset = bytesRead;
    return ok;
}
