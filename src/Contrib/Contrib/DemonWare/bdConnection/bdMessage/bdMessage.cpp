// Original file: Contrib/Contrib/DemonWare/bdConnection/bdMessage/bdMessage.cpp
#include "bdMessage.h"

bdMessage::bdMessage(const bdMessageType type, const bdBool typeChecked)
    : m_type(type), m_payloadTypeChecked(typeChecked)
{
}

bdMessage::bdMessage(const bdMessageType type, const bdUByte8 *const payload,
                     const bdUInt32 size, const bdBool hasTypeCheckedBit,
                     const bdUByte8 *const unencPayload, const bdUInt32 unencSize)
    : m_type(type), m_payloadTypeChecked(false)
{
    if (payload && size) {
        m_payload = new bdBitBuffer(payload, size * 8, hasTypeCheckedBit);
        m_payloadTypeChecked = m_payload->getTypeCheck();
    }
    if (unencPayload && unencSize) {
        m_unencPayload = new bdByteBuffer(unencSize, false);
        m_unencPayload->write(unencPayload, unencSize);
    }
}

bdMessage::~bdMessage() {}

bdMessageType bdMessage::getType() const
{
    return m_type;
}

bdBitBufferRef bdMessage::getPayload()
{
    if (m_payload.isNull()) {
        m_payload = new bdBitBuffer(0, m_payloadTypeChecked);
    }
    return m_payload;
}

bdBool bdMessage::hasPayload() const
{
    const bdBool hasPlayload = m_payload.notNull();
    return hasPlayload;
}

bdByteBufferRef bdMessage::getUnencryptedPayload() const
{
    return m_unencPayload;
}

bdBool bdMessage::hasUnencryptedPayload() const
{
    const bdBool hasUnencryptedPayload = m_unencPayload.notNull();
    return hasUnencryptedPayload;
}
