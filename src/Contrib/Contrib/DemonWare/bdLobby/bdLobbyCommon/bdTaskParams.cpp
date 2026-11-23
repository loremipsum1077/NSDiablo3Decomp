// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdTaskParams.cpp
#include "bdTaskParams.h"
#include "bdUserAccountID.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

bdTaskParams::bdTaskParams(bdUByte8 serviceID, bdUByte8 taskID, bdUInt capacityIncreaseHint, bdUInt maxBufferSize)
    : bdBufferParams(capacityIncreaseHint, maxBufferSize, false), m_serviceID(serviceID), m_taskID(taskID),
      m_taskResults(0), m_taskResultRefs(0), m_maxResults(0)
{
    bdUInt minimumBufferSize = 76;
    if (maxBufferSize < minimumBufferSize) {
        bdLogWarn("bdTaskParams", "Minimum buffer size %u", minimumBufferSize);
        m_maxBufferSize = minimumBufferSize;
    }
    bdUInt initBufferSize = capacityIncreaseHint > minimumBufferSize ? capacityIncreaseHint : minimumBufferSize;
    initBufferSize = initBufferSize > maxBufferSize ? maxBufferSize : initBufferSize;
    m_taskBuffer = new bdTaskByteBuffer(initBufferSize, true);
    m_buffer = m_taskBuffer;
    if (m_buffer) bdRemoteTaskManager::initTaskBuffer(m_taskBuffer, m_serviceID, m_taskID);
    else {
        bdLogError("bdTaskParams", "Failed to allocate task buffer");
        m_serializeOk = false;
    }
}
bdTaskParams::~bdTaskParams() {}
void bdTaskParams::addContextUser(const bdUserAccountID & userID)
{
    if (!m_isWritingArray) {
        bdBool capacityOk = ensureCapacity(ContextSerialization::getUserIDSize(userID), true);
        if (capacityOk) m_serializeOk = m_serializeOk && ContextSerialization::writeUserID(m_buffer, userID);
    } else {
        bdLogError("bdTaskParams", "Cannot serialize a user inside an array");
        m_serializeOk = false;
    }
}
void bdTaskParams::addContext(const bdNChar8 * context)
{
    if (!m_isWritingArray) {
        bdBool capacityOk = ensureCapacity(16, true);
        if (capacityOk) m_serializeOk = m_serializeOk && ContextSerialization::writeContext(m_buffer, context);
    } else {
        bdLogError("bdTaskParams", "Cannot serialize context inside an array");
        m_serializeOk = false;
    }
}
