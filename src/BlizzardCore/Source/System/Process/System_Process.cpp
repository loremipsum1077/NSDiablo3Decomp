// Original file: BlizzardCore/Source/System/Process/System_Process.cpp
// The original is a unity file that includes Process.cpp and Process_NX64.cpp.
#include "../../../Include/Blizzard/String.h"
#include "../../../Include/Blizzard/Time.h"
#include "../../../Include/Blizzard/Util.h"

#include <new>
#include <stdint.h>
#include <string.h>

// NintendoSDK declarations (nn/nn_TimeSpan.h, nn/os/os_Thread.h).
namespace nn {
struct TimeSpanType {  // size 0x8
    static TimeSpanType FromMilliSeconds(int64_t milliSeconds)
    {
        TimeSpanType timeSpan = { milliSeconds * 1000 * 1000 };
        return timeSpan;
    }
    int64_t _nanoSeconds;  // 0x00
};

class TimeSpan {  // size 0x8
public:
    TimeSpan(const TimeSpanType & timeSpan) : m_TimeSpan(timeSpan) {}
    static TimeSpan FromMilliSeconds(int64_t milliSeconds)
    {
        return TimeSpan(TimeSpanType::FromMilliSeconds(milliSeconds));
    }

private:
    TimeSpanType m_TimeSpan;  // 0x00
};

namespace os {
void SleepThread(nn::TimeSpan time);
}  // namespace os
}  // namespace nn

namespace Blizzard {
namespace Process {
bool GetProcessPath(char * buf, int bufMax);

// Process.cpp:27
bool GetProcessDirectory(char * buf, int bufMax)
{
    Util::Buffer<1024, char> path(bufMax);
    if (!GetProcessPath(path, bufMax))
    {
        return false;
    }
    String::SplitPath(path, buf, bufMax, NULL, 0);
    return true;
}

// Process_NX64.cpp:25
bool Sleep(Time::Millisecond milliseconds)
{
    nn::os::SleepThread(nn::TimeSpan::FromMilliSeconds(milliseconds));
    return true;
}

// Process_NX64.cpp:37
char * GetComputerName(char * computerName, int maxNameSize)
{
    String::Copy(computerName, "NX64", maxNameSize);
    return computerName;
}

// Process_NX64.cpp:47
inline bool GetProcessPath(char * buf, int bufMax)
{
    if (bufMax <= 0)
    {
        return false;
    }
    strncpy(buf, "data:/", bufMax);
    return true;
}
}  // namespace Process
}  // namespace Blizzard
