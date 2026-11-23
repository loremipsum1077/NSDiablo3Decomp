// Original file: BlizzardCore/Source/System/Time/System_Time.cpp
// The original is a unity file that includes Time.cpp and Time_NX64.cpp (the DWARF decl_file of
// every function). Their bodies are written here in that order, each with a comment giving its
// original file and line.
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Time.h"
#include "../../../../NintendoSDK/Include/nn/nn_Result.h"
#include <limits.h>
#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>

// NintendoSDK declarations (nn/nn_TimeSpan.h, nn/os/os_Tick.h, nn/os/os_TickTypes.h,
// nn/time/time_CalendarTime.h, time_PosixTime.h, time_StandardUserSystemClock.h, time_Api.h).
namespace nn {
struct TimeSpanType {  // size 0x8
    int64_t GetNanoSeconds() const { return _nanoSeconds; }
    // nn_TimeSpan.h:213
    int64_t GetMicroSeconds() const { return DivideNanoSeconds(_nanoSeconds, 0x20C49BA5E353F7CFLL, 7); }
    // nn_TimeSpan.h:201
    int64_t GetMilliSeconds() const { return DivideNanoSeconds(_nanoSeconds, 0x431BDE82D7B634DBLL, 18); }
    // nn_TimeSpan.h:189
    int64_t GetSeconds() const { return DivideNanoSeconds(_nanoSeconds, 0x112E0BE826D694B3LL, 26); }
    int64_t _nanoSeconds;  // 0x00

private:
    // nn_TimeSpan.h:402
    static int64_t DivideNanoSeconds(const int64_t nanoSeconds, const int64_t magic, const int32_t rightShift)
    {
        int64_t n = MultiplyRightShift(nanoSeconds, magic);
        n >>= rightShift;
        return n + (static_cast<uint64_t>(nanoSeconds) >> 63);
    }
    // nn_TimeSpan.h:415
    static int64_t MultiplyRightShift(const int64_t x, const int64_t y)
    {
        const uint64_t x_lo = x & 0xffffffff;
        const int64_t x_hi = x >> 32;
        const int64_t y_hi = y >> 32;
        const uint64_t y_lo = y & 0xffffffff;
        const int64_t z = x_hi * y_lo + ((x_lo * y_lo) >> 32);
        return x_hi * y_hi + (static_cast<int64_t>(x_lo * y_hi + (z & 0xffffffff)) >> 32) + (z >> 32);  // asr #32 at 0x710000a324
    }
};

class TimeSpan {  // size 0x8
public:
    int64_t GetNanoSeconds() const { return m_TimeSpan.GetNanoSeconds(); }
    int64_t GetMicroSeconds() const { return m_TimeSpan.GetMicroSeconds(); }  // line 671
    int64_t GetMilliSeconds() const { return m_TimeSpan.GetMilliSeconds(); }  // line 659
    int64_t GetSeconds() const { return m_TimeSpan.GetSeconds(); }  // line 647

private:
    TimeSpanType m_TimeSpan;  // 0x00
};

namespace os {
struct Tick;
TimeSpan ConvertToTimeSpan(Tick tick);

struct Tick {  // size 0x8
    // os_TickTypes.h:112
    TimeSpan ToTimeSpan() const { return ConvertToTimeSpan(*this); }
    int64_t m_Tick;  // 0x00
};

Tick GetSystemTick();
}  // namespace os

namespace time {
struct CalendarTime {  // size 0x8
    int16_t year;  // 0x00
    int8_t month;  // 0x02
    int8_t day;  // 0x03
    int8_t hour;  // 0x04
    int8_t minute;  // 0x05
    int8_t second;  // 0x06
};

struct PosixTime {  // size 0x8
    int64_t value;  // 0x00
};

enum DayOfWeek {
    DayOfWeek_Sunday = 0,
};

class StandardUserSystemClock {
public:
    static Result GetCurrentTime(PosixTime * pOutPosixTime);
};

CalendarTime ToCalendarTimeInUtc(const PosixTime & posixTime);
PosixTime ToPosixTimeFromUtc(const CalendarTime & calendarTime);
DayOfWeek GetDayOfWeek(int year, int month, int day);
}  // namespace time
}  // namespace nn

// ---- Time.cpp

namespace TimeConst {
const int64 TimestampsPerSecond = 1000000000;  // line 44
static const int64 TimestampsPerUnixtime = TimestampsPerSecond;  // line 49
static const int64 TimestampsPerFILETIME = 100;  // line 52
static const Blizzard::Time::Timestamp TimestampUnixBias = -946684800LL * TimestampsPerUnixtime;  // line 63
static const Blizzard::Time::UnixTime UnixTimestampBias = 946684800;  // line 72
static const Blizzard::Time::Timestamp TimestampUnixMin = (int64)INT_MIN * TimestampsPerUnixtime + TimestampUnixBias;  // line 88
static const Blizzard::Time::Timestamp TimestampUnixMax = (int64)INT_MAX * TimestampsPerUnixtime + TimestampUnixBias;  // line 89
static const Blizzard::Time::UnixTime64 UnixTime64TimestampMin = -8276687237LL;  // line 93
static const Blizzard::Time::UnixTime64 UnixTime64TimestampMax = 10170056836LL;  // line 94
}  // namespace TimeConst

namespace Blizzard {
namespace Time {

// Time.cpp:118
uint32 GetTimeElapsed(uint32 startTime, uint32 endTime)
{
    if (endTime < startTime)
        return (0xFFFFFFFF - startTime) + endTime;
    return endTime - startTime;
}

// Time.cpp:128
int32 GetDayOfTheYear(int32 nDay, int32 nMonth, int32 nYear)
{
    static const int addPerMonth[13] = {0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334, 365};
    bool leap;
    if (nYear % 400 == 0) leap = true;
    else if (nYear % 100 == 0) leap = false;
    else leap = nYear % 4 == 0;

    int32 nDayOfTheYear = addPerMonth[nMonth - 1] + nDay - 1;
    if (leap && nMonth > 2)
        nDayOfTheYear++;
    return nDayOfTheYear;
}

// Time.cpp:177
UnixTime ToUnixTime(Timestamp stamp)
{
    UnixTime unixtime;
    if (stamp <= TimeConst::TimestampUnixMin)
        unixtime = INT_MIN;
    else if (stamp >= TimeConst::TimestampUnixMax)
        unixtime = INT_MAX;
    else
        unixtime = (UnixTime)((stamp - TimeConst::TimestampUnixBias) / TimeConst::TimestampsPerUnixtime);
    return unixtime;
}

// Time.cpp:197
Timestamp FromUnixTime(UnixTime unixtime)
{
    Timestamp time = (Timestamp)unixtime * TimeConst::TimestampsPerUnixtime + TimeConst::TimestampUnixBias;
    return time;
}

// Time.cpp:208
UnixTime64 ToUnixTime64(Timestamp stamp)
{
    UnixTime64 unixtime = stamp / TimeConst::TimestampsPerSecond;
    if (stamp % TimeConst::TimestampsPerSecond == 0 || stamp >= 0)
        unixtime += TimeConst::UnixTimestampBias;
    else
        unixtime += TimeConst::UnixTimestampBias - 1;
    return unixtime;
}

// Time.cpp:221
Timestamp FromUnixTime64(UnixTime64 unixtime)
{
    Timestamp time;
    if (unixtime <= TimeConst::UnixTime64TimestampMin)
        time = LLONG_MIN;
    else if (unixtime > TimeConst::UnixTime64TimestampMax)
        time = LLONG_MAX;
    else
        time = unixtime * TimeConst::TimestampsPerSecond + TimeConst::TimestampUnixBias;
    return time;
}

Timestamp MakeTime(TimeRec & rec);

// Time.cpp:258
Timestamp FromHTTPDate(const char * httpDate)
{
    const char * const k_month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char * dayIndex;
    const char * monthIndex;
    const char * yearIndex;
    const char * timeIndex;
    TimeRec rec;

    if (httpDate[3] == ',')
    {
        // RFC 1123: "Sun, 06 Nov 1994 08:49:37 GMT"
        dayIndex = httpDate + 5;
        monthIndex = httpDate + 8;
        yearIndex = httpDate + 12;
        timeIndex = httpDate + 17;
    }
    else if (httpDate[3] == ' ')
    {
        // ANSI C asctime(): "Sun Nov  6 08:49:37 1994"
        dayIndex = httpDate + 8;
        monthIndex = httpDate + 4;
        yearIndex = httpDate + 20;
        timeIndex = httpDate + 11;
    }
    else
    {
        // RFC 850: "Sunday, 06-Nov-94 08:49:37 GMT"
        const char * commaOffset = String::Find(httpDate, ',');
        if (commaOffset == NULL || String::Length(commaOffset) != 24)
            return 0;
        dayIndex = commaOffset + 2;
        monthIndex = commaOffset + 5;
        yearIndex = commaOffset + 9;
        timeIndex = commaOffset + 12;
    }

    rec.day = atoi(dayIndex);
    rec.month = -1;
    for (int k = 0; k < 12; ++k)
    {
        if (String::EqualI(monthIndex, k_month[k], 3))
        {
            rec.month = k + 1;
            break;
        }
    }
    rec.hour = atoi(timeIndex);
    rec.minute = atoi(timeIndex + 3);
    rec.second = atoi(timeIndex + 6);
    rec.nanosecond = 0;
    rec.year = atoi(yearIndex);
    if (!isdigit(yearIndex[2]))
        rec.year += (rec.year > 68) ? 1900 : 2000;

    return MakeTime(rec);
}

}  // namespace Time
}  // namespace Blizzard

// ---- Time_NX64.cpp

// Time_NX64.cpp:20
static nn::TimeSpan GetSystemTimeSpan()
{
    const nn::TimeSpan timespan = nn::os::GetSystemTick().ToTimeSpan();
    return timespan;
}

Blizzard::Time::Timestamp stBootTimeTs;  // line 27
Blizzard::Time::Nanosecond stBootTimeNs;  // line 28

// Time_NX64.cpp:34
void InitializeTimeNX64()
{
    nn::time::PosixTime posixTime;
    {
        nn::Result nn_result = nn::time::StandardUserSystemClock::GetCurrentTime(&posixTime);
        (void)nn_result;
    }
    stBootTimeTs = Blizzard::Time::FromUnixTime64(posixTime.value);
    stBootTimeNs = Blizzard::Time::Nanoseconds();
}

namespace Blizzard {
namespace Time {

// Time_NX64.cpp:49
Timestamp GetTimestamp()
{
    const Nanosecond tCurrTimeNs = Nanoseconds();
    const Timestamp tCurrTimeTs = stBootTimeTs + (tCurrTimeNs - stBootTimeNs);
    return tCurrTimeTs;
}

// Time_NX64.cpp:61
Timestamp LocalBias(Timestamp time)
{
    return time;
}

// Time_NX64.cpp:82
Nanosecond Nanoseconds()
{
    const Nanosecond nanoseconds = GetSystemTimeSpan().GetNanoSeconds();
    return nanoseconds;
}

// Time_NX64.cpp:89
Microsecond Microseconds()
{
    const nn::TimeSpan timespan = GetSystemTimeSpan();
    const Microsecond microseconds = timespan.GetMicroSeconds();
    return microseconds;
}

// Time_NX64.cpp:96


Millisecond Milliseconds()
{
    const nn::TimeSpan timespan = GetSystemTimeSpan();
    const Millisecond milliseconds = timespan.GetMilliSeconds();
    return milliseconds;
}

// Time_NX64.cpp:103
Second Seconds()
{
    const nn::TimeSpan timespan = GetSystemTimeSpan();
    const Second seconds = timespan.GetSeconds();
    return seconds;
}

// Time_NX64.cpp:116
// Store order from the line rows: hour 123, minute 124, second 127, year 128 (stp with month);
// month and nanosecond on the row-less lines 125/126 (inferred, the order that matches).
void BreakTime(Timestamp stamp, TimeRec & rec)
{
    UnixTime64 nPosixTime = ToUnixTime64(stamp);
    nn::time::CalendarTime time = nn::time::ToCalendarTimeInUtc(reinterpret_cast<nn::time::PosixTime &>(nPosixTime));
    rec.day = time.day;
    rec.dayOfWeek = nn::time::GetDayOfWeek(time.year, time.month, time.day);
    rec.dayOfYear = GetDayOfTheYear(time.day, time.month, time.year);
    rec.hour = time.hour;
    rec.minute = time.minute;
    rec.month = time.month;
    rec.nanosecond = 0;
    rec.second = time.second;
    rec.year = time.year;
}

// Time_NX64.cpp:137
Timestamp MakeTime(TimeRec & rec)
{
    nn::time::CalendarTime calendarTime;
    calendarTime.year = rec.year;
    calendarTime.month = rec.month;
    calendarTime.day = rec.day;
    calendarTime.hour = rec.hour;
    calendarTime.minute = rec.minute;
    calendarTime.second = rec.second;
    nn::time::PosixTime posixTime = nn::time::ToPosixTimeFromUtc(calendarTime);
    return FromUnixTime64(posixTime.value) + rec.nanosecond;
}

}  // namespace Time
}  // namespace Blizzard
