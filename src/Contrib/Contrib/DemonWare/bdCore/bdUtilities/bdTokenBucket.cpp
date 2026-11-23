#include "Contrib/Contrib/DemonWare/bdCore/bdUtilities/bdTokenBucket.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformTiming/bdPlatformTiming.h"

















bdTokenBucket::bdTokenBucket(bdUInt32 minFragmentTokens)
    : m_tokensPerMs(0)
    , m_capacity(0)
    , m_minFragment(minFragmentTokens)
    , m_tokens(0)
    , m_lastUpdateTime(0)
    , m_isInfinite(true)
{
}

void bdTokenBucket::fillBucket()
{
    bdUInt64 timeNow = bdPlatformTiming::getHiResTimeStamp();


    bdUInt32 elapsedMs = 0;

    bdFloat32 elapsedMsFloat = bdPlatformTiming::getElapsedTime(m_lastUpdateTime, timeNow) * 1000.0f;
    if (elapsedMsFloat >= 65000.0f || elapsedMsFloat < 0.0f)
    {
        elapsedMs = 0;
    }
    else
    {
        elapsedMs = (bdUInt32)elapsedMsFloat;
    }

    m_tokens = (m_tokens + elapsedMs * m_tokensPerMs < m_capacity) ? m_tokens + elapsedMs * m_tokensPerMs : m_capacity;
    m_lastUpdateTime = timeNow;
}

bdUInt32 bdTokenBucket::getAvailTokens(bdUInt32 maxTokens) const
{
    if (m_isInfinite)
    {
        return maxTokens;
    }
    else
    {

        if (m_tokens >= maxTokens)
        {
            return maxTokens;
        }
        else
        {

            if (m_tokens >= m_minFragment)
            {
                return m_tokens;
            }
            else
            {
                return 0;
            }
        }
    }
}


void bdTokenBucket::removeTokens(bdUInt32 numTokens)
{
    if (m_tokens >= numTokens)
    {
        m_tokens -= numTokens;
    }
    else
    {
        m_tokens = 0;
    }
}

void bdTokenBucket::setTokensRestricted(bdBool isRestricted)
{
    m_isInfinite = !isRestricted;
}

void bdTokenBucket::setRates(bdUInt32 tokensPerMs,
                             bdUInt32 bucketSize)
{





    fillBucket();

    m_tokensPerMs = tokensPerMs;
    m_capacity = bucketSize;
    m_tokens = (m_tokens < bucketSize) ? m_tokens : bucketSize;
}

void bdTokenBucket::setMinFragment(bdUInt32 minFragmentTokens)
{




    m_minFragment = minFragmentTokens;
}
