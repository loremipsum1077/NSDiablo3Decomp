#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../SigmaTypes.h"
#include "../Unicode.h"

enum SeverityLevel {
    SLVL_INTERNAL = 0,
    SLVL_DEBUG = 1,
    SLVL_INFO = 2,
    SLVL_WARNING = 3,
    SLVL_CRASH_INFO = 4,
    SLVL_ERROR = 5,
    SLVL_SERVICE_ERROR = 6,
    SLVL_FATAL = 7,
};

enum OutputStreamType {
    OUTPUTSTREAM_DEFAULT = 0,
    OUTPUTSTREAM_BOOT = 1,
    OUTPUTSTREAM_SYNC = 2,
    OUTPUTSTREAM_STREAMING = 3,
    OUTPUTSTREAM_BATTLENET = 4,
    OUTPUTSTREAM_LOCKS = 5,
    OUTPUTSTREAM_NOTIFICATIONS = 6,
    OUTPUTSTREAM_TRADE = 7,
    OUTPUTSTREAM_LEADERBOARD = 8,
    OUTPUTSTREAM_RIFTSTATS = 9,
    NUM_OUTPUTSTREAMS = 10,
};

enum {
    LF_ECHO_TO_OUTPUT_WINDOW = 1,
    LF_ECHO_TO_STREAM = 2,
    LF_ECHO_TO_SCREEN = 4,
    LF_ECHO_TO_CONSOLE = 8,
    LF_LIMIT_OUTPUT = 16,
    LF_CAPSLOCK_ONLY = 32,
    LF_ECHO_TO_CLIENTS = 64,
    LF_ECHO_TO_RING_BUFFER = 128,
    LF_ECHO_TO_SCREEN_AND_CONSOLE = 12,
    LF_DEFAULT = 3,
};

struct RingBufferLogInfo {  // size 0x10
    CHAR * pMemoryRingBuffer;  // 0x00
    uint32 dwCurrentLocation;  // 0x08
    uint32 dwRingBufferSize;  // 0x0c
};
static_assert(sizeof(RingBufferLogInfo) == 0x10, "RingBufferLogInfo");

typedef void (*PFNLOGSINKCALLBACK)(const char *, int32, SeverityLevel, const OutputStreamType, LPCSTR, LPCSTR);

void ErrorManagerLoggingInitialize();
void ErrorManagerLoggingRegisterLogSink(PFNLOGSINKCALLBACK pfnLogSinkCallback);
void ErrorManagerLoggingUnregisterLogSink(PFNLOGSINKCALLBACK pfnLogSinkCallback);
void ErrorManagerEnableLocalLogging(BOOL bEnabled);
SeverityLevel ErrorManagerGetStreamSeverityLevel(OutputStreamType eStream);
void ErrorManagerSetLogStreamDirectory(LPCSTR pszDirectory);
void ErrorManagerSetLogStreamFilename(const OutputStreamType eStream, LPCSTR pszFileName);
void ErrorManagerLogToStream(const SeverityLevel eLevel, const OutputStreamType eStream, LPCSTR szMessage, BOOL fUseRingBuffer);
BOOL TraceInternal_ShouldLog(const SeverityLevel eLevel, const uint32 dwFlags, const OutputStreamType eStream);
void TraceInternal_Log(const SeverityLevel eLevel, const uint32 dwFlags, const OutputStreamType eStream, LPCSTR szFormat, ...);
void TraceBootStop();
