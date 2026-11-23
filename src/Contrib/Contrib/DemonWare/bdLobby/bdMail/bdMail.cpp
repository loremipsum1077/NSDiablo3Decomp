// Original file: Contrib/Contrib/DemonWare/bdLobby/bdMail/bdMail.cpp
#include "bdMail.h"
#include "bdMailBody.h"
#include "bdMailInfo.h"
#include "../bdLobbyCommon/bdUserAccountID.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"

// Release logging retains only arguments; channel and format text are descriptive.

bdMail::bdMail(bdRemoteTaskManager *const remoteTaskManager, const bdNChar8 * context)
    : m_remoteTaskManager(remoteTaskManager)
{
    bdStrlcpy(m_context, context, sizeof(m_context));
}
bdMail::~bdMail() { m_remoteTaskManager = 0; }

bdRemoteTaskRef bdMail::getMailInfoForCategories(bdMailInfo * mailInfos, const bdUInt32 startIndex, const bdUInt32 numMails, const bdUInt16 * mailCategories, const bdUInt32 numCategories, const bdBool newestFirst, const bdUserAccountID * userID)
{
    bdRemoteTaskRef task;
    const bdUInt taskSize = (84 + bdStrnlen(m_context, 16)) + (numCategories * (sizeof(bdUInt16) + 1) + (userID ? ContextSerialization::getUserIDSize(*userID) : 0)) + sizeof(bdBool) + 1;
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 29, 11);
    bdBool wroteParam = buffer->writeString(m_context, 16);
    wroteParam = wroteParam && buffer->writeUInt32(startIndex);
    wroteParam = wroteParam && buffer->writeUInt32(numMails);
    buffer->writeArrayStart(6, numCategories, 2);
    for (bdUInt i = 0; i < numCategories; ++i) {
        wroteParam = wroteParam && buffer->writeUInt16(mailCategories[i]);
    }
    buffer->writeArrayEnd();
    wroteParam = wroteParam && buffer->writeBool(newestFirst);
    if (userID) wroteParam = wroteParam && ContextSerialization::writeUserID(buffer, *userID);
    if (wroteParam) {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdMail", "Failed to start task: %u", startTaskResult);
        else task->setTaskResult(mailInfos, numMails);
    } else bdLogWarn("bdMail", "Failed to serialize task parameters");
    return task;
}

bdRemoteTaskRef bdMail::getMail(const bdUInt64 * mailIDs, const bdUInt32 numMailIDs, bdMailBody * mails, const bdBool deleteOnRead, const bdUserAccountID * userID)
{
    bdRemoteTaskRef task;
    const bdUInt taskSize = 76 + bdStrnlen(m_context, 16) + numMailIDs * (sizeof(bdUInt64) + 1) + (userID ? ContextSerialization::getUserIDSize(*userID) : 0);
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 29, 4);
    bdBool wroteParam = buffer->writeString(m_context, 16);
    wroteParam = wroteParam && buffer->writeBool(deleteOnRead);
    buffer->writeArrayStart(10, numMailIDs, 8);
    for (bdUInt i = 0; i < numMailIDs; ++i) {
        wroteParam = wroteParam && buffer->writeUInt64(mailIDs[i]);
    }
    buffer->writeArrayEnd();
    if (userID) wroteParam = wroteParam && ContextSerialization::writeUserID(buffer, *userID);
    if (wroteParam) {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdMail", "Failed to start task: %u", startTaskResult);
        else task->setTaskResult(mails, numMailIDs);
    } else bdLogWarn("bdMail", "Failed to serialize task parameters");
    return task;
}

bdRemoteTaskRef bdMail::deleteMail(const bdUInt64 * mailIDs, const bdUInt32 numMailIDs, const bdUserAccountID * userID)
{
    bdRemoteTaskRef task;
    const bdUInt taskSize = 74 + bdStrnlen(m_context, 16) + numMailIDs * (sizeof(bdUInt64) + 1) + (userID ? ContextSerialization::getUserIDSize(*userID) : 0);
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 29, 5);
    bdBool wroteParam = buffer->writeString(m_context, 16);
    buffer->writeArrayStart(10, numMailIDs, 8);
    for (bdUInt i = 0; i < numMailIDs; ++i) {
        wroteParam = wroteParam && buffer->writeUInt64(mailIDs[i]);
    }
    buffer->writeArrayEnd();
    if (userID) wroteParam = wroteParam && ContextSerialization::writeUserID(buffer, *userID);
    if (wroteParam) {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdMail", "Failed to start task: %u", startTaskResult);
    } else bdLogWarn("bdMail", "Failed to serialize task parameters");
    return task;
}

bdRemoteTaskRef bdMail::setMessageReadStatus(const bdUInt64 mailID, const bdBool status, const bdUserAccountID * userID)
{
    bdRemoteTaskRef task;
    const bdUInt taskSize = 93 + bdStrnlen(m_context, 16) + (userID ? ContextSerialization::getUserIDSize(*userID) : 0);
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 29, 8);
    bdBool wroteParam = buffer->writeString(m_context, 16);
    wroteParam = wroteParam && buffer->writeUInt64(mailID);
    wroteParam = wroteParam && buffer->writeUInt32(status);
    wroteParam = wroteParam && buffer->writeUInt32(0);
    if (userID) wroteParam = wroteParam && ContextSerialization::writeUserID(buffer, *userID);
    if (wroteParam) {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdMail", "Failed to start task: %u", startTaskResult);
    } else bdLogWarn("bdMail", "Failed to serialize task parameters");
    return task;
}

bdRemoteTaskRef bdMail::sendMailWithDuration(const void *const message, const bdUInt32 messageSize, const bdUInt16 mailCategory, const bdUInt32 messageDuration, const bdBool returnOnExpiration, const bdUserAccountID * userIDs, const bdUInt32 numUserIDs, const bdUserAccountID * senderID)
{
    bdRemoteTaskRef task;
    const bdUByte8 taskID = senderID ? 7 : 9;
    const bdUInt actualMessageSize = messageSize < 1024 ? messageSize : 1024;
    if (messageSize > 1024) bdLogWarn("bdMail", "Mail message exceeds maximum size");
    bdUInt taskSize = 74 + bdStrnlen(m_context, 16) + numUserIDs * (sizeof(bdUInt64) + 1) + actualMessageSize + 21;
    if (senderID) taskSize += ContextSerialization::getUserIDSize(*senderID);
    for (bdUInt i = 0; i < numUserIDs; ++i) {
        taskSize += ContextSerialization::getUserIDSize(userIDs[i]);
    }
    bdTaskByteBufferRef buffer = new bdTaskByteBuffer(taskSize, true);
    bdRemoteTaskManager::initTaskBuffer(buffer, 29, taskID);
    bool wroteParam = true;
    if (senderID) wroteParam = ContextSerialization::writeUserID(buffer, *senderID);
    wroteParam = wroteParam && buffer->writeString(m_context, 16);
    wroteParam = wroteParam && buffer->writeBlob(message, actualMessageSize);
    wroteParam = wroteParam && buffer->writeUInt16(mailCategory);
    wroteParam = wroteParam && buffer->writeUInt32(numUserIDs);
    for (bdUInt i = 0; i < numUserIDs; ++i) {
        wroteParam = wroteParam && ContextSerialization::writeUserID(buffer, userIDs[i]);
    }
    wroteParam = wroteParam && buffer->writeUInt32(messageDuration);
    wroteParam = wroteParam && buffer->writeBool(returnOnExpiration);
    if (wroteParam) {
        const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, buffer);
        if (startTaskResult != BD_NO_ERROR) bdLogWarn("bdMail", "Failed to start task: %u", startTaskResult);
    } else bdLogWarn("bdMail", "Failed to serialize task parameters");
    return task;
}
