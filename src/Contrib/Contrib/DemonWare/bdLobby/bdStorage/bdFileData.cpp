#include "bdFileData.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"










bdFileData::bdFileData() : m_fileData(NULL), m_fileSize(0) {}











bdFileData::~bdFileData() {}



bdBool bdFileData::deserialize(bdByteBufferRef buffer)
{
    bdBool ok = buffer->readBlob(static_cast<bdUByte8 *>(m_fileData), m_fileSize);
    return ok;
}


bdUInt bdFileData::sizeOf() { return sizeof(bdFileData); }
