// Original file: Contrib/Contrib/DemonWare/bdLobby/bdAntiCheat/bdExtendedAuthInfo.cpp
#include "bdExtendedAuthInfo.h"
#include "../../bdCore/bdContainers/bdByteBuffer.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"




bdExtendedAuthInfo::bdExtendedAuthInfo() : m_initialized(false) {
    bdMemset(m_data, 0, sizeof(m_data));
}



bdExtendedAuthInfo::~bdExtendedAuthInfo() {}



bdBool bdExtendedAuthInfo::serialize(bdByteBuffer & buffer) const {
    bdBool ok = false;
    if (m_initialized) {
        ok = buffer.writeString(m_data, sizeof(m_data));
    }
    if (!ok) {
        bdLogWarn("extended auth info", "Failed to serialize extended auth info.");
    }
    return ok;
}










bdUInt bdExtendedAuthInfo::sizeOf() { return sizeof(bdExtendedAuthInfo); }














bdBool bdExtendedAuthInfo::setData(const bdNChar8 * data) {
    bdBool ok = false;
    bdUWord lenData = bdStrlen(data);
    if (lenData <= sizeof(m_data) - 1) {
        bdUWord lenWritten = bdStrlcpy(m_data, data, sizeof(m_data));
        (void)lenWritten;
        m_initialized = true;
        ok = true;
    } else {
        bdLogWarn("extended auth info", "Data too long (%u).", lenData);
    }
    return ok;
}
