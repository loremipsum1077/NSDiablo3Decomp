// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/DebugChannelInterface/XDebugChannelInterface.cpp

#include "XDebugChannelInterface.h"
#include "../Memory/SigmaMemory.h"

#include <string.h>

// line 14
XDebugChannelInterface * g_pDebugChannel = NULL;

// line 19
// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
XDebugChannelInterface::~XDebugChannelInterface()
{
}

// line 28
XDebugMessage::XDebugMessage(const uint16 wType)
{
    m_wType = wType;
    m_dwDataSize = 0;
    m_pReadPtr = m_data;
    if (wType >= XDM_LARGE_MESSAGE_START)
        m_largeDataInfo.dwLargeDataCount = 0;
}

// line 109
// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
void XDebugMessage::Add(const void * pData, uint32 dwLen)
{
    SigmaMemoryCopy(&m_data[m_dwDataSize], pData, dwLen);
    m_dwDataSize += dwLen;
}

// line 119
// Stripped by the linker (DWARF low_pc 0); written from its DWARF and checked only against the recorded size.
void XDebugMessage::AddEndianSwap(const uint8 * pData, uint32 dwSize)
{
    Add(pData, dwSize);
}

// line 136
void XDebugMessage::AddString(const char * szStr)
{
    uint32 dwStrLen = strlen(szStr);
    ++dwStrLen;
    Add(szStr, dwStrLen);
}

// line 165
void XDebugMessage::AddUInt32(const uint32 dwData)
{
    AddEndianSwap((const uint8 *)&dwData, sizeof(dwData));
}
