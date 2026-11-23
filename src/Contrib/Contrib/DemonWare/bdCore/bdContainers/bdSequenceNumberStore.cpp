#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSequenceNumber.h"
#include "Contrib/Contrib/DemonWare/bdCore/bdContainers/bdSequenceNumberStore.h"
#include "Contrib/Contrib/DemonWare/bdPlatform/bdPlatformLog/bdPlatformLog.h"











bdSequenceNumberStore::bdSequenceNumberStore(const bdSequenceNumber & inital)
    : m_bitmap(0)
    , m_lastSeq(inital)
{
}

bdSequenceNumberStore::bdSequenceStatus bdSequenceNumberStore::check(const bdSequenceNumber & thisSeq)
{
    const bdUInt windowSize = 32;
    bdUInt diff;



    if (thisSeq > m_lastSeq)
    {

        bdSequenceNumber sdiff = thisSeq - m_lastSeq;
        diff = sdiff.getValue();


        if (diff < windowSize)
        {

            m_bitmap = (m_bitmap << diff) | 1;
            m_lastSeq = thisSeq;
            return BD_SN_VALID_LARGER;
        }
        else
        {

            m_lastSeq = thisSeq;
            m_bitmap = 1;
            return BD_SN_VALID_MUCH_LARGER;
        }
    }
    else
    {

        bdSequenceNumber sdiff = m_lastSeq - thisSeq;
        diff = sdiff.getValue();

        if (diff >= windowSize)
        {
            return BD_SN_INVALID_SMALLER;
        }


        if (m_bitmap & (1 << diff))
        {
            return BD_SN_INVALID_DUPLICATE;
        }



        m_bitmap |= (1 << diff);
        return BD_SN_VALID_SMALLER;
    }
}

const bdSequenceNumber & bdSequenceNumberStore::getLastSequenceNumber() const
{
    return m_lastSeq;
}
