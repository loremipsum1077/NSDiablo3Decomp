// Original file: Contrib/Contrib/DemonWare/bdLobby/bdStorage/bdStorage.cpp
// Reconstructed from original instructions and DWARF; only retained functions.
#include "bdStorage.h"
#include "bdFileData.h"
#include "bdContextUserStorageFileInfo.h"
#include "../bdLobbyCommon/bdTaskParams.h"
#include "../bdRemoteTaskManager/bdRemoteTaskManager.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Blank lines preserve the original owner statement lines.










































bdStorage::bdStorage(bdRemoteTaskManager *const remoteTaskManager,
                     const bdNChar8 *context)
    : m_remoteTaskManager(remoteTaskManager)
{
    bdStrlcpy(m_context, context, sizeof(m_context));
}

bdStorage::~bdStorage()
{
    m_remoteTaskManager = NULL;
}

bdRemoteTaskRef bdStorage::uploadFile(const bdNChar8 *const fileName,
                                    const void *const fileData,
                                    const bdUInt fileSize,
                                    const bdFileInfo::bdVisibility fileVisibility,
                                    const bdUserID &ownerID,
                                    bdUserStorageFileInfo *const fileInfo)
{
    bdRemoteTaskRef task(NULL);


    bdTaskParams params(
        10,
        10,
        1024,
        m_remoteTaskManager->getTaskParamsMaxBufferSize());


    params.addContext(m_context);
    params.addString(fileName, 128);
    params.addBool(fileVisibility == bdFileInfo::BD_VISIBLE_PRIVATE);
    params.addBlob(fileData, fileSize);
    params.addContextUser(ownerID);



    params.bindResults(fileInfo, 1);


    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

    if (startTaskResult != BD_NO_ERROR)
    {
        bdUseVAArgs();
    }

    return task;
}
































































































































































































































































































































bdRemoteTaskRef bdStorage::getFile(const bdNChar8 *fileName,
                                 bdFileData *const fileData,
                                 const bdUserID &ownerUID)
{
    bdRemoteTaskRef task(NULL);


    bdTaskParams params(
        10,
        12,
        1024,
        m_remoteTaskManager->getTaskParamsMaxBufferSize());


    params.addContext(m_context);
    params.addString(fileName, 128);
    params.addContextUser(ownerUID);



    params.bindResults(fileData, 1);


    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

    if (startTaskResult != BD_NO_ERROR)
    {
        bdUseVAArgs();
    }

    return task;
}





































































































bdRemoteTaskRef bdStorage::listFilesByOwner(const bdUserID &ownerID,
                                          const bdUInt32 startDate,
                                          bdUserStorageFileInfo *fileList,
                                          const bdUInt16 maxNumResults,
                                          const bdUInt16 offset,
                                          const bdNChar8 *const fileName)
{
    bdRemoteTaskRef task(NULL);


    bdTaskParams params(
        10,
        13,
        1024,
        m_remoteTaskManager->getTaskParamsMaxBufferSize());


    params.addContext(m_context);
    params.addContextUser(ownerID);
    params.addUInt32(startDate);
    params.addUInt16(maxNumResults);
    params.addUInt16(offset);
    if (fileName)
    {
        params.addString(fileName, 128);
    }


    params.bindResults(fileList, maxNumResults);


    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

    if (startTaskResult != BD_NO_ERROR)
    {
        bdUseVAArgs();
    }

    return task;
}








































bdRemoteTaskRef bdStorage::getPublisherFile(const bdNChar8 *const fileName,
                                          bdFileData *const fileData)
{
    bdRemoteTaskRef task(NULL);


    bdTaskParams params(
        10,
        21,
        1024,
        m_remoteTaskManager->getTaskParamsMaxBufferSize());


    params.addContext(m_context);
    params.addString(fileName, 128);



    params.bindResults(fileData, 1);


    const bdLobbyErrorCode startTaskResult = m_remoteTaskManager->startTask(task, params);

    if (startTaskResult != BD_NO_ERROR)
    {
        bdUseVAArgs();
    }

    return task;
}
