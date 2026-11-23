#include "bdCounterValue.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"










bdCounterValue::bdCounterValue()
    : m_counterID(0), m_counterValue(0)
{
}


bdBool bdCounterValue::serialize(bdByteBuffer &buffer) const
{
    bdBool writeOk = buffer.writeUInt32(m_counterID);
    writeOk = writeOk && buffer.writeInt64(m_counterValue);
    return writeOk;
}

bdBool bdCounterValue::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt32(m_counterID);
    ok = ok && buffer->readInt64(m_counterValue);
    return ok;
}


bdUInt bdCounterValue::sizeOf() { return sizeof(bdCounterValue); }
