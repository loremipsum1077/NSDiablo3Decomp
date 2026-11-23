// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdByteBuffer.inl
#pragma once
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"

// Lines 25/47, reconstructed from original caller inline trees.
inline bdByteBuffer::bdByteBuffer(const bdUInt size, bdBool isTypeChecked)
    : m_data(NULL), m_typeChecked(isTypeChecked), m_typeCheckedCopy(isTypeChecked), m_allocatedData(false)
{
    init(size);
}
inline void bdByteBuffer::init(const bdUInt size)
{
    m_size = size;
    if (m_size) allocateBuffer();
    m_readPtr = m_data;
    m_writePtr = m_data;
}
// Lines 36/61, reconstructed from bdLobbyConnection::sendFramedMessage's inline tree.
inline bdByteBuffer::bdByteBuffer(void * bytesToCopy, const bdUInt size, bdBool isTypeChecked)
    : m_size(size), m_data(NULL), m_typeChecked(isTypeChecked), m_typeCheckedCopy(isTypeChecked), m_allocatedData(false)
{
    if (bytesToCopy) {
        init(bytesToCopy, size);
    }
}
inline void bdByteBuffer::init(void * bytesToCopy, const bdUInt size)
{
    init(size);
    bdMemcpy(m_data, bytesToCopy, size);
}
inline bdUInt bdByteBuffer::getSize() const { return m_size; }
inline bdUByte8 * bdByteBuffer::getData() { return m_data; }
inline bdUInt bdByteBuffer::getMaxWriteSize() const { return m_size - bdUInt(m_writePtr - m_data); }
inline bdUInt bdByteBuffer::getMaxReadSize() const { return m_size - bdUInt(m_readPtr - m_data); }
