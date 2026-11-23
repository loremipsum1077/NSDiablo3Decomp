#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/iosfwd.h"
#include "../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../Sigma/ErrorManager/ErrorManagerLogging.h"
#include "OnlineService.h"

namespace OnlineService {

// line 31 (the implicit destructor carries the class line)
class LogStream : public blz::ostringstream {  // size 0x98
public:
    LogStream & operator<<(bool);
    LogStream & operator<<(short);
    LogStream & operator<<(unsigned short);
    LogStream & operator<<(int val);
    LogStream & operator<<(unsigned int);
    LogStream & operator<<(int64);
    LogStream & operator<<(uint64 val);
    LogStream & operator<<(long int &);
    LogStream & operator<<(const long int &);
    LogStream & operator<<(float);
    LogStream & operator<<(double &);
    LogStream & operator<<(const double &);
    LogStream & operator<<(const char * val);
    LogStream & operator<<(blz::streambuf *);
    LogStream & operator<<(blz::ostream &(*)(blz::ostream &));
    LogStream & operator<<(blz::ios &(*)(blz::ios &));

    // line 61
    LogStream(SeverityLevel eSL, uint32 uFlags, OutputStreamType eStream)
        : m_eSeverityLevel(eSL), m_uFlags(uFlags), m_eStream(eStream) {}

    const char * Get() const;
    void SetSeverityLevel(SeverityLevel);
    void WriteLog() const;

protected:
    SeverityLevel m_eSeverityLevel;  // 0x70
    uint32 m_uFlags;  // 0x74
    OutputStreamType m_eStream;  // 0x78
};
static_assert(sizeof(LogStream) == 0x98, "LogStream");

typedef blz::string ostringstream_stringtype;

// OnlineServiceLogStream.cpp
LogStream & operator<<(LogStream & s, const ostringstream_stringtype & c);
blz::ostream & operator<<(blz::ostream & s, const GameAccountHandle & a);
LogStream & operator<<(LogStream & s, const GameIDLabel & label);

}  // namespace OnlineService

// The logging macro. Its name is not in the DWARF (only the local it declares,
// __xLocalVarName_, is); the shape is from the callers in X/Server/SGame.cpp:
// TraceInternal_ShouldLog(level, flags, stream), then a LogStream local built
// with the same three values, the inserted expression, WriteLog().
#define ONLINE_SERVICE_LOG(eLevel, uFlags, eStream, expr)                              \
    if (TraceInternal_ShouldLog(eLevel, uFlags, eStream)) {                           \
        OnlineService::LogStream __xLocalVarName_(eLevel, uFlags, eStream);           \
        __xLocalVarName_ << expr;                                                     \
        __xLocalVarName_.WriteLog();                                                  \
    }
