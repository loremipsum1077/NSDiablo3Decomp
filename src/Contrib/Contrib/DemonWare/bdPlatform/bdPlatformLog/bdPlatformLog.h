// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h

#pragma once

#include "../bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdPlatformPatterns/bdLinkable.h"

enum bdLogMessageType {
    BD_LOG_INFO = 0,
    BD_LOG_WARNING = 1,
    BD_LOG_ERROR = 2,
};

class bdLogSubscriber : public bdLinkable {  // size 0x48
public:
    bdLogSubscriber();
    virtual ~bdLogSubscriber();  // vtable slot 0
    bdBool addChannel(const bdNChar8 *const channel);
    bdBool removeChannel(const bdNChar8 *const);
    void removeAllChannels();
    virtual void publish(const bdLogMessageType type, const bdNChar8 *const, const bdNChar8 *const file, const bdNChar8 *const, const bdUInt line, const bdNChar8 *const msg) const;  // vtable slot 2
    void logMessage(const bdLogMessageType, const bdNChar8 *const, const bdNChar8 *const, const bdNChar8 *const, const bdUInt, const bdNChar8 *const) const;
protected:
    const bdNChar8 * m_channels[5];  // 0x20
};
static_assert(sizeof(bdLogSubscriber) == 0x48, "bdLogSubscriber");

// Release logging: every bdLog* site is a call to this variadic no-op with the format
// arguments only (bdPlatformLog.cpp line 236).
void bdUseVAArgs(...);

// Macro names and shape invented (not in the DWARF): channel and format are dropped.
#define bdLogInfo(channel, format, ...) bdUseVAArgs(__VA_ARGS__)
#define bdLogWarn(channel, format, ...) bdUseVAArgs(__VA_ARGS__)
#define bdLogError(channel, format, ...) bdUseVAArgs(__VA_ARGS__)
