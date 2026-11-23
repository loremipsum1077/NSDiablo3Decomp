// Original file: Contrib/Contrib/DemonWare/bdConnection/bdPacket/bdChunk.cpp
#include "bdChunk.h"
#include "../../bdCore/bdUtilities/bdBytePacker.h"
bdChunk::bdChunk(const bdChunkTypes type) : m_type(type) {}
bdChunk::~bdChunk() {}
bdChunkTypes bdChunk::getType() const { return m_type; }
bdChunkTypes bdChunk::getType(const bdUByte8 *const data, const bdUInt size)
{
    bdUInt offset = 0;
    bdUByte8 tmp;
    const bdBool result = bdBytePacker::removeBasicType(data, size, offset, offset, tmp);
    return result ? static_cast<bdChunkTypes>(tmp) : BD_CT_INVALID;
}
bdUInt bdChunk::serialize(bdUByte8 *data, const bdUInt32 size) const
{
    bdUInt offset = 0;
    bdUByte8 type = static_cast<bdUByte8>(m_type);
    bdBytePacker::appendBasicType(data, size, offset, offset, type);
    return offset;
}
bdBool bdChunk::deserialize(const bdUByte8 *const data, const bdUInt size, bdUInt &offset)
{
    bdUByte8 tmp;
    bdBool result = false;
    if (size - offset >= sizeof(m_headerData))
    {
        result = bdBytePacker::removeBasicType(data, size, offset, offset, tmp);
        if (result) m_type = static_cast<bdChunkTypes>(tmp);
    }
    return result;
}
bdUInt bdChunk::getSerializedSize() const { return serialize(0, 0); }
bdBool bdChunk::isControl() const { return m_type != BD_CT_DATA; }
