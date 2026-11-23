// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/DebugChannelInterface/XDebugChannelInterface.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"

enum XDebugMessageTypes {
    XDM_CLIENT_HEARTBEAT = 0,
    XDM_CLIENT_NAME = 1,
    XDM_STRING_NEW = 2,
    XDM_CONSOLE_ENABLE = 3,
    XDM_CONSOLE_OUTPUT = 4,
    XDM_CONSOLE_COMMAND = 5,
    XDM_CONSOLE_CLEAR = 6,
    XDM_MEMORY_ENABLE = 7,
    XDM_MEMORY_STATS = 8,
    XDM_MEMORY_ALLOC = 9,
    XDM_MEMORY_ALLOC_GROW = 10,
    XDM_MEMORY_STATS_XBOX = 11,
    XDM_MEMORY_STATS_PS3 = 12,
    XDM_BUG_REPORT_COMPLETE = 13,
    XDM_PROPERTY_SET = 14,
    XDM_PROPERTY_REQ_UPDATE = 15,
    XDM_PROPERTY_UPDATE = 16,
    XDM_PROPERTY_CHANGE = 17,
    XDM_PROPERTY_TREE_NODES = 18,
    XDM_LARGE_MESSAGE_START = 19,
    XDM_PROFILER_DATA = 19,
    XDM_MAX_TYPE = 20,
};

struct LargeData {  // size 0x10
    const uint8 * pLargeData;  // 0x00
    uint32 dwDataSize;  // 0x08
};
static_assert(sizeof(LargeData) == 0x10, "LargeData");

class XDebugMessage {  // size 0x318
public:
    XDebugMessage(const uint16);
    XDebugMessage(const XDebugMessage &);
    XDebugMessage(const uint8 *, uint32);
    void Clear();
    uint32 GetCapacity() const;
    uint32 GetCapacityLeft() const;
    void AddLargeData(const void *, uint32);
    void Add(const void * pData, uint32 dwLen);
    void AddString(const char * szStr);
    void AddUInt8(const uint8);
    void AddInt8(const int8);
    void AddUInt16(const uint16);
    void AddUInt32(const uint32 dwData);
    void AddUInt64(const uint64);
    void AddInt16(const int16);
    void AddInt32(const int32);
    void AddInt64(const int64);
    void AddFloat(const float);
    void AddBool(const BOOL);
    void Get(void * pData, uint32 dwLen);
    void GetString(char *, uint32);
    uint8 GetUInt8();
    int8 GetInt8();
    uint16 GetUInt16();
    uint32 GetUInt32();
    uint64 GetUInt64();
    int16 GetInt16();
    int32 GetInt32();
    int64 GetInt64();
    float GetFloat();
    BOOL GetBool();
    void ResetReadPtr();
    uint32 GetSize() const;
    uint32 GetDataLeft() const;
    uint16 GetType() const;
    uint32 GetLargeDataCount() const;
    const LargeData & GetLargeData(uint32) const;
    enum {
        MAX_MESSAGE_SIZE = 768,
        MAX_LARGE_DATA = 47,
    };
    const uint8 * GetDataPtr() const;
private:
    void operator=(const XDebugMessage &);
    void AddEndianSwap(const uint8 * pData, uint32 dwSize);
    void GetEndianSwap(void * pData, uint32 dwLen);
    struct LargeDataInfo {  // size 0x2f8
        uint32 dwLargeDataCount;  // 0x00
        LargeData tLargeData[47];  // 0x08
    };
    uint16 m_wType;  // 0x00
    union {  // size 0x300
        uint8 m_data[768];  // 0x00
        XDebugMessage::LargeDataInfo m_largeDataInfo;  // 0x00
    };  // 0x08
    uint32 m_dwDataSize;  // 0x308
    uint8 * m_pReadPtr;  // 0x310
};
static_assert(sizeof(XDebugMessage) == 0x318, "XDebugMessage");

class XDebugChannelInterface {  // size 0x8
    // vtable pointer
    typedef BOOL (*ProcessMessageCallback)(XDebugMessage &, void *);
public:
    virtual void Send(const XDebugMessage &, const BOOL) = 0;  // vtable slot 0
    virtual void RegisterListener(const uint16, XDebugChannelInterface::ProcessMessageCallback, void *) = 0;  // vtable slot 1
    virtual void DeregisterListener(const uint16, XDebugChannelInterface::ProcessMessageCallback) = 0;  // vtable slot 2
    virtual ~XDebugChannelInterface();  // vtable slot 0
};
static_assert(sizeof(XDebugChannelInterface) == 0x8, "XDebugChannelInterface");

// The one global channel, defined in XDebugChannelInterface.cpp:14. Declared here
// so the channel-interface files that post to it (XDebugChannelMemory.cpp,
// XDebugChannelStringHelper.cpp) share one declaration.
extern XDebugChannelInterface * g_pDebugChannel;

