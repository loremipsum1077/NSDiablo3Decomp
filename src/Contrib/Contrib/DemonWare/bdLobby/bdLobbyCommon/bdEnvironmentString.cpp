// Original file: Contrib/Contrib/DemonWare/bdLobby/bdLobbyCommon/bdEnvironmentString.cpp

#include "bdEnvironmentString.h"

#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"

// Blank lines keep the original line numbers (DWARF line table).











bdEnvironmentString::bdEnvironmentString(const bdNChar8 * devString,
                                         const bdNChar8 * certString,
                                         const bdNChar8 * prodString)
{
    bdStrlcpy(m_devString, devString, sizeof(m_devString));
    bdStrlcpy(m_certString, certString, sizeof(m_certString));
    bdStrlcpy(m_prodString, prodString, sizeof(m_prodString));
}

bdEnvironmentString::bdEnvironmentString(const bdEnvironmentString & other)
{
    bdStrlcpy(m_devString, other.m_devString, sizeof(m_devString));
    bdStrlcpy(m_certString, other.m_certString, sizeof(m_certString));
    bdStrlcpy(m_prodString, other.m_prodString, sizeof(m_prodString));
}






























const bdNChar8 * bdEnvironmentString::get(const bdEnvironment environment) const
{
    const bdNChar8 * result;
    switch (environment)
    {
    case BD_ENVIRONMENT_PROD:
        result = m_prodString;
        break;
    case BD_ENVIRONMENT_CERT:
        result = m_certString;
        break;
    case BD_ENVIRONMENT_DEV:
        result = m_devString;
        break;
    default:
        bdUseVAArgs();
        result = NULL;
        break;
    }
    return result;
}
