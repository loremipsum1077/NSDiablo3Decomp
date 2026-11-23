// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: NintendoSDK/Include/nn/diag/diag_LogTypes.h

#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

namespace nn {
namespace diag {
struct LogMessage {  // size 0x10
    const char * format;  // 0x00
    va_list * args;  // 0x08
};
static_assert(sizeof(LogMessage) == 0x10, "LogMessage");

// Added by hand from the stores in bdLogSubscriber::publish and the upstream SDK names.
enum LogSeverity {
    LogSeverity_Trace = 0,
    LogSeverity_Info = 1,
    LogSeverity_Warn = 2,
    LogSeverity_Error = 3,
    LogSeverity_Fatal = 4,
};

struct SourceInfo {  // size 0x18
    int lineNumber;  // 0x00
    const char * fileName;  // 0x08
    const char * functionName;  // 0x10
};

struct LogMetaData {  // size 0x40
    SourceInfo sourceInfo;  // 0x00
    const char * moduleName;  // 0x18
    LogSeverity severity;  // 0x20
    int verbosity;  // 0x24
    bool useDefaultLocaleCharset;  // 0x28
    void * pAdditionalData;  // 0x30
    size_t additionalDataBytes;  // 0x38
};
static_assert(sizeof(LogMetaData) == 0x40, "LogMetaData");

namespace detail {
void LogImpl(const LogMetaData & logMetaData, const char * format, ...);
}  // namespace detail

}  // namespace diag
}  // namespace nn

// Upstream SDK macro (nn/nn_Log.h), reduced to the fields the binary stores.
#define NN_LOG(...) \
    do \
    { \
        ::nn::diag::LogMetaData logMetaData; \
        logMetaData.sourceInfo.lineNumber = __LINE__; \
        logMetaData.sourceInfo.fileName = __FILE__; \
        logMetaData.sourceInfo.functionName = __func__; \
        logMetaData.moduleName = ""; \
        logMetaData.severity = ::nn::diag::LogSeverity_Info; \
        logMetaData.verbosity = 0; \
        logMetaData.useDefaultLocaleCharset = false; \
        logMetaData.pAdditionalData = NULL; \
        logMetaData.additionalDataBytes = 0; \
        ::nn::diag::detail::LogImpl(logMetaData, __VA_ARGS__); \
    } while (0)
