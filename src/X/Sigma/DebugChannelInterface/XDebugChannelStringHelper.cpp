// Original file: X/Sigma/DebugChannelInterface/XDebugChannelStringHelper.cpp
#include "../DataStructures/XGrowableMap.h"
#include "XDebugChannelInterface.h"
#include "../RefString.h"

// Class layout and member ownership from this file's DWARF, original line 14.
class XDebugChannelStringIdGenerator {
public:
    uint32 GetStaticId(const char * szString);
    XDebugChannelStringIdGenerator();
private:
    BOOL m_bGetStaticIdInProgress;
    class StringEntry {
    public:
        uint32 m_dwId;
        const char * m_szString;
    };
    XGrowableMap<const char *, const char *, StringEntry, StringEntry> m_staticStringMap;
    uint32 m_idCounter;
};
static_assert(sizeof(XDebugChannelStringIdGenerator) == 0x90, "XDebugChannelStringIdGenerator");

// Original line 35.
static XDebugChannelStringIdGenerator s_tIdGenerator;

// Original line 47. Hash capacity 1000 and initial string from 0x710097223c
// and original .rodata at 0x7100e244e7 respectively.
XDebugChannelStringIdGenerator::XDebugChannelStringIdGenerator()
    : m_bGetStaticIdInProgress(FALSE), m_staticStringMap(16, NULL), m_idCounter(0)
{
    m_staticStringMap.SetHashTableSize_Tracked(1000);
    uint32 dwUnknownId = GetStaticId("#Unknown#");
}

// Original line 58; inline chain uses the existing owning map/allocator headers.
uint32 XDebugChannelStringIdGenerator::GetStaticId(const char * szString)
{
    StringEntry tStringEntry;
    if (m_staticStringMap.Lookup(szString, tStringEntry))
        return tStringEntry.m_dwId;
    if (!m_bGetStaticIdInProgress && g_pDebugChannel) {
        m_bGetStaticIdInProgress = TRUE;
        tStringEntry.m_dwId = m_idCounter;
        tStringEntry.m_szString = szString;
        m_staticStringMap.SetAt_Tracked(szString, tStringEntry);
        XDebugMessage tMsg(XDM_STRING_NEW);
        tMsg.AddUInt32(m_idCounter++);
        tMsg.AddString(szString);
        m_bGetStaticIdInProgress = FALSE;
        return m_idCounter - 1;
    }
    return 0;
}
