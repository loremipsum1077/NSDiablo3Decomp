// bdLobby/bdRemoteTaskManager/bdTaskByteBuffer.cpp -- the byte buffer a
// lobby task is serialised into, with header room in front of the payload.
// Diagnostic strings are inferred: this release preserves only format arguments.
#include "bdTaskByteBuffer.h"

#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdCore/bdMemory/bdMemory.inl"

bdTaskByteBuffer::bdTaskByteBuffer(const bdUInt size, const bdBool isTypeChecked)
	: bdByteBuffer(0, isTypeChecked), m_taskData(NULL), m_taskDataSize(0), m_validHeaderSize(0), m_paddingSize(0)
{
	setupTaskData(size);
}

bdTaskByteBuffer::~bdTaskByteBuffer()
{
	if (m_taskData)
	{
		bdDeallocate(m_taskData);
	}
	m_taskData = NULL;
	m_data = m_readPtr = m_writePtr = NULL;
}

void bdTaskByteBuffer::allocateBuffer()
{
	if (m_taskData)
	{
		bdLogWarn("bdtaskbytebuffer", "Buffer already allocated.");
		return;
	}
	setupTaskData(m_size);
}

void bdTaskByteBuffer::setHeaderSize(bdUInt size)
{
    m_validHeaderSize = size < maxHeaderSize ? size : maxHeaderSize;
}

bdUByte8 *bdTaskByteBuffer::getHeaderStart() const
{
	return m_taskData ? m_taskData + (maxHeaderSize - m_validHeaderSize) : NULL;
}

void bdTaskByteBuffer::adjustPayload(void *payloadStart, bdUInt payloadSize)
{
	const bdBool hasPayload = m_taskData != NULL;
	const bdBool payloadStartInBounds =
		(bdUByte8 *)payloadStart > m_taskData && (bdUByte8 *)payloadStart < m_taskData + m_taskDataSize;
	const bdBool payloadEndInBounds = (bdUByte8 *)payloadStart + payloadSize > m_taskData &&
		(bdUByte8 *)payloadStart + payloadSize <= m_taskData + m_taskDataSize;

	if (hasPayload && payloadStartInBounds && payloadEndInBounds)
	{
		m_data = (bdUByte8 *)payloadStart;
		m_size = payloadSize;
		m_readPtr = m_writePtr = (bdUByte8 *)payloadStart;
	}
	else
	{
		bdLogError("bdtaskbytebuffer", "Payload out of bounds.");
	}
}

// Original behavior: retain header/footer only and reset payload cursors.
void bdTaskByteBuffer::resizeToFit(bdUInt32 newDataSize)
{
	const bdUInt32 payloadOffset = m_data - m_taskData;
	const bdUInt32 oldTaskFooterOffset = m_size + payloadOffset;
	const bdUInt32 oldTaskFooterSize = m_taskDataSize - oldTaskFooterOffset;
	bdUByte8 *oldTaskData = m_taskData;
	const bdUInt32 newPayloadSize = newDataSize;
	const bdUInt32 newTaskDataSize = m_taskDataSize - m_size + newPayloadSize;

	m_taskData = bdAllocate<bdUByte8>(newTaskDataSize);
	m_taskDataSize = newTaskDataSize;
	bdMemcpy(m_taskData, oldTaskData, payloadOffset);
	bdMemcpy(m_taskData + payloadOffset + newPayloadSize, oldTaskData + oldTaskFooterOffset, oldTaskFooterSize);

	m_data = m_readPtr = m_taskData + payloadOffset;
	m_writePtr = m_data;
	m_size = newPayloadSize;
	bdDeallocate(oldTaskData);
}

// Original cpp line 178; kept here even though all three callers inline it.
inline void bdTaskByteBuffer::setupTaskData(bdUInt size)
{
    if (size)
    {
        m_paddingSize = signatureSize;
        m_taskDataSize = size + maxHeaderSize + m_paddingSize;
        m_taskData = bdAllocate<bdUByte8>(m_taskDataSize);
        m_data = m_taskData + maxHeaderSize;
        m_validHeaderSize = maxHeaderSize;
        m_size = size;
    }
    m_readPtr = m_data;
    m_writePtr = m_data;
}

bdBool bdTaskByteBuffer::expand(bdUInt increaseSize)
{
	bdBool expandOk = false;

	if (m_taskData)
	{
		if (m_readPtr >= m_data && m_writePtr >= m_data)
		{
			bdUByte8 *oldTaskData = m_taskData;
			const bdUInt64 readOffset = m_readPtr - m_data;
			const bdUInt64 writeOffset = m_writePtr - m_data;
			const bdUInt oldTaskSize = m_taskDataSize;
			bdUByte8 *oldData = m_data;
			bdUByte8 *oldWritePtr = m_writePtr;
			bdUByte8 *oldReadPtr = m_readPtr;
			const bdUInt oldPaddingSize = m_paddingSize;
			const bdUInt oldValidHeaderSize = m_validHeaderSize;
			const bdUInt oldSize = m_size;

			m_size = oldSize + increaseSize;
			m_taskData = NULL;
			allocateBuffer();

			if (!m_taskData)
			{
				m_paddingSize = oldPaddingSize;
				m_taskDataSize = oldTaskSize;
				m_taskData = oldTaskData;
				m_data = oldData;
				m_validHeaderSize = oldValidHeaderSize;
				m_size = oldSize;
				m_readPtr = oldReadPtr;
				m_writePtr = oldWritePtr;
				bdLogError("bdtaskbytebuffer", "Failed to expand buffer to %u bytes.", oldSize + increaseSize);
			}
			else
			{
				bdMemcpy(m_taskData, oldTaskData, oldTaskSize);
				m_readPtr += readOffset;
				m_writePtr += writeOffset;
				bdDeallocate(oldTaskData);
				expandOk = true;
			}
		}
	}
	else
	{
		bdLogWarn("bdtaskbytebuffer", "Expanding unallocated buffer to %u bytes.", increaseSize);
		m_size = increaseSize;
		setupTaskData(increaseSize);
		if (m_taskData)
		{
			expandOk = true;
		}
		else
		{
			bdLogError("bdtaskbytebuffer", "Failed to allocate %u bytes.", increaseSize);
		}
	}
	return expandOk;
}
