// Original file: Contrib/Contrib/DemonWare/bdHTTP/bdHTTP.cpp

#include "bdHTTP.h"
#include "../bdPlatform/bdPlatformString/bdPlatformString.inl"







bdHTTP::bdHTTP()
    : m_status(BD_UNINITIALIZED), m_timeout(0)
{
}








bdBool bdHTTP::setUploadData(const bdNChar8 * buffer, bdUInt32 size)
{
    return setUploadData(reinterpret_cast<const bdByte8 *>(buffer), size);
}








bdBool bdHTTP::setDownloadBuffer(bdNChar8 * buffer, bdUInt32 size)
{
    return setDownloadBuffer(reinterpret_cast<bdByte8 *>(buffer), size);
}


bdBool bdHTTP::isState(bdHTTP::bdRequestState state) const
{
    bdBool ok = false;
    switch (state)
    {
    case BD_IDLE:
    {
        ok = m_status == BD_EMPTY;
        break;
    }
    case BD_CREATING:
    {
        ok = m_status == BD_INITIALIZING;
        break;
    }
    case BD_RUNNING:
    {
        ok = m_status == BD_PENDING;
        break;
    }
    case BD_STOPPING:
    {
        ok = m_status == BD_ABORTING;
        break;
    }
    case BD_FINISHED:
    {
        ok = (m_status == BD_DONE) || (m_status == BD_FAILED) || (m_status == BD_ABORTED) || (m_status == BD_ABORTING);
        break;
    }
    case BD_DESTROYED:
    {   // Always true; the binary keeps the short-circuit shape of a tautological condition.
        ok = (m_status != BD_INITIALIZING) || (m_status != BD_PENDING);
        break;
    }
    default:
    {
        ok = false;
        break;
    }
    }
    return ok;
}


bdBool bdHTTP::setHeader(const bdNChar8 * key, const bdUInt64 value)
{
    bdNChar8 integerString[24];

    bdSnprintf(integerString, sizeof(integerString), "%llu", value);
    return setHeader(key, integerString);
}
