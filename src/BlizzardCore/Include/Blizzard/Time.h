#pragma once

#include "Types.h"

namespace Blizzard {
namespace Time {
typedef int64 Timestamp;

typedef uint64 Win32FILETIME;

typedef int32 UnixTime;

typedef int64 UnixTime64;

struct TimeRec {  // size 0x24
    int year;  // 0x00
    int month;  // 0x04
    int day;  // 0x08
    int hour;  // 0x0c
    int minute;  // 0x10
    int second;  // 0x14
    int nanosecond;  // 0x18
    int dayOfWeek;  // 0x1c
    int dayOfYear;  // 0x20
};
static_assert(sizeof(TimeRec) == 0x24, "TimeRec");

typedef uint64 Nanosecond;

typedef uint64 Microsecond;

typedef uint32 Millisecond;

typedef uint32 Second;

Timestamp GetTimestamp();
Timestamp FromUnixTime(UnixTime unixtime);
UnixTime ToUnixTime(Timestamp stamp);  // Time.cpp:177
Timestamp FromUnixTime64(UnixTime64 unixtime);
Millisecond Milliseconds();
Microsecond Microseconds();
Second Seconds();
uint32 GetTimeElapsed(uint32 startTime, uint32 endTime);
Nanosecond Nanoseconds();
void BreakTime(Timestamp stamp, TimeRec& rec);
}  // namespace Time
}  // namespace Blizzard

// BlizzardCore/Source/System/Time/Time.cpp:128 (X/Common/Game.cpp GameGetAnniversaryEventStatus).
namespace Blizzard {
namespace Time {
int32 GetDayOfTheYear(int32 nDay, int32 nMonth, int32 nYear);
}  // namespace Time
}  // namespace Blizzard

// BlizzardCore/Source/System/Time/Time.cpp and Time_NX64.cpp.
namespace Blizzard {
namespace Time {
typedef int32 UnixTime;
UnixTime64 ToUnixTime64(Timestamp stamp);  // Time.cpp:208
Timestamp FromHTTPDate(const char * httpDate);  // Time.cpp:258
Timestamp LocalBias(Timestamp time);  // Time_NX64.cpp:61
Timestamp MakeTime(TimeRec & rec);  // Time_NX64.cpp:137
}  // namespace Time
}  // namespace Blizzard
