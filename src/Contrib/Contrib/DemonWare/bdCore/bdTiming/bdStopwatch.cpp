// Original file: Contrib/Contrib/DemonWare/bdCore/bdTiming/bdStopwatch.cpp
#include "bdStopwatch.h"
#include "../../bdPlatform/bdPlatformTiming/bdPlatformTiming.h"

bdStopwatch::bdStopwatch()
{
    reset();
}

void bdStopwatch::start()
{
    m_start = bdPlatformTiming::getHiResTimeStamp();
}

void bdStopwatch::reset()
{
    m_start = 0;
}

bdFloat32 bdStopwatch::getElapsedTimeInSeconds() const
{
    if (m_start) {
        const bdUInt64 nowTimeStamp = bdPlatformTiming::getHiResTimeStamp();
        return bdPlatformTiming::getElapsedTime(m_start, nowTimeStamp);
    }
    return 0.0f;
}
