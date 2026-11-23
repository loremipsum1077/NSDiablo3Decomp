#include "bdServiceBandwidthArbitrator.h"

bdServiceBandwidthArbitrator::bdServiceBandwidthArbitrator()
    : m_allowedSend(0), m_sentThisSlice(false) {}

void bdServiceBandwidthArbitrator::reset()
{
    m_allowedSend = 0;
    m_sentThisSlice = false;
}

void bdServiceBandwidthArbitrator::addSliceQuota(bdUInt bytes)
{
    if (m_allowedSend / (bdInt)bytes < 2 &&
        (m_allowedSend <= 0 || m_sentThisSlice))
        m_allowedSend += bytes;
    m_sentThisSlice = false;
}

bdBool bdServiceBandwidthArbitrator::allowedSend(bdUInt) const
{
    return m_allowedSend > 0;
}

void bdServiceBandwidthArbitrator::send(bdUInt bytes)
{
    m_allowedSend -= bytes;
    m_sentThisSlice = true;
}
