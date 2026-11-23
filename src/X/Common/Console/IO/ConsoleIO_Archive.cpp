// Original file: X/Common/Console/IO/ConsoleIO_Archive.cpp

#include "ConsoleIO_Archive.h"

#include <string.h>

#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Util/Hash.h"

namespace Console {
namespace IO {

CPKArchive::CPKArchive(const char * szFilename)
    : m_dwFlags(0),
      m_nResult(IORESULT_SUCCESS)
{
    strncpy(m_szFilename, szFilename, sizeof(m_szFilename) - 1);
}

CPKArchive::~CPKArchive()
{
}

uint64 CPKArchive::GetFileHash(const char * szFilename)
{
    CHAR szLoweredName[4096];
    strncpy(szLoweredName, szFilename, sizeof(szLoweredName) - 1);
    Blizzard::String::ToLower(szLoweredName);
    return Blizzard::Util::Hash64(szLoweredName);
}

uint64 CPKArchive::GetSubFileHash(uint64 dwParentHash, const char * szFilename)
{
    CHAR szLoweredName[4096];
    strncpy(szLoweredName, szFilename, sizeof(szLoweredName) - 1);
    Blizzard::String::ToLower(szLoweredName);
    return Blizzard::Util::Hash64More(szLoweredName, (uint)strlen(szLoweredName), dwParentHash);
}

uint32 CPKArchive::sHighestBit(uint64 dwValue)
{
    int32 nHighestBit = 0;
    while (dwValue)
    {
        dwValue >>= 1;
        nHighestBit++;
    }
    return nHighestBit;
}

}  // namespace IO
}  // namespace Console
