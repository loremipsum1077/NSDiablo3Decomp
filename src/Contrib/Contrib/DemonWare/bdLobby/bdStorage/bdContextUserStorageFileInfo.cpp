#include "bdContextUserStorageFileInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include <string.h>









bdContextUserStorageFileInfo::bdContextUserStorageFileInfo()
    : m_createTime(0), m_modifedTime(0), m_visibility(bdFileInfo::BD_MAX_VISIBILITY_TYPE), m_fileSize(0)
{
    memset(m_fileName, 0, sizeof(m_fileName));
}




bdContextUserStorageFileInfo::~bdContextUserStorageFileInfo() {}



bdBool bdContextUserStorageFileInfo::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readUInt32(m_fileSize);
    ok = ok && buffer->readUInt32(m_createTime);
    ok = ok && buffer->readUInt32(m_modifedTime);
    bdBool visibilityPrivate = false;
    ok = ok && buffer->readBool(visibilityPrivate);
    m_visibility = static_cast<bdFileInfo::bdVisibility>(visibilityPrivate);
    ok = ok && ContextSerialization::readUserID(buffer, m_ownerID);
    ok = ok && buffer->readString(m_fileName, sizeof(m_fileName));
    return ok;
}











bdUInt bdContextUserStorageFileInfo::sizeOf() { return sizeof(bdContextUserStorageFileInfo); }
