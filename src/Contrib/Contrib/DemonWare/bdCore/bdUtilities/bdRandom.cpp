#include "Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdRandom.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformTiming/bdPlatformTiming.h"
















bdRandom::bdRandom()
{
    setSeed(bdPlatformTiming::getHiResTimeStamp());
}






bdRandom::~bdRandom()
{
}

bdUInt bdRandom::nextUInt()
{
    bdInt t = a * (m_val % q) - r * (m_val / q);
    if (t <= 0)
    {
        t += m;
    }
    m_val = t;

    return m_val;
}













void bdRandom::setSeed(const bdUInt seed)
{
    m_val = seed;

    if (m_val == 0)
    {
        bdLogWarn("bdCore/bdRandom", "Random seed was zero, using default seed.");  // text inferred; the release macro keeps only variadic arguments
        m_val = 0xba15b9;
    }
}
