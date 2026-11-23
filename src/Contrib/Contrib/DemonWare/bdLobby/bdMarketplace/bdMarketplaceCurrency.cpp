#include "bdMarketplaceCurrency.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"

bdMarketplaceCurrency::bdMarketplaceCurrency() { reset(); }
bdBool bdMarketplaceCurrency::serialize(bdByteBuffer &buffer) const
{
    bdBool writeOk = buffer.writeUByte8(m_currencyId);
    writeOk = writeOk && buffer.writeUInt32(m_value);
    return writeOk;
}
bdBool bdMarketplaceCurrency::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUByte8(m_currencyId);
    ok = ok && buffer->readUInt32(m_value);
    return ok;
}
bdUInt bdMarketplaceCurrency::sizeOf() { return sizeof(*this); }
bdInt bdMarketplaceCurrency::serializedSizeOf() const { return 7; }
void bdMarketplaceCurrency::reset() { m_currencyId = 0; m_value = 0; }
