// Reconstructed from original callers; owner: bdLobbyCommon/bdTaskParams.inl.
#pragma once
// line 14
inline bdTaskByteBufferRef bdTaskParams::getTaskBuffer() { return m_taskBuffer; }
// line 85
inline bdBool bdTaskParams::ensureCapacity(bdUInt paramSize, bdBool typeChecked)
{
    bdUInt bytesRequired = paramSize + 73;
    bdBool expandOk = bdBufferParams::ensureCapacity(bytesRequired, typeChecked);
    return expandOk;
}

// line 27
inline void bdTaskParams::bindResults(bdTaskResult * results, bdUInt maxResults)
{
    if (!m_taskResults && !m_taskResultRefs) {
        m_taskResults = results;
        m_maxResults = maxResults;
    } else {
        bdLogError("bdTaskParams", "Results already bound");
        m_serializeOk = false;
    }
}
// line 43
inline void bdTaskParams::bindResultRefs(bdTaskResult ** results, bdUInt maxResults)
{
    if (!m_taskResults && !m_taskResultRefs) {
        m_taskResultRefs = results;
        m_maxResults = maxResults;
    } else {
        bdLogError("bdTaskParams", "Results already bound");
        m_serializeOk = false;
    }
}

#line 59
inline bdBool bdTaskParams::getResultBindings(bdTaskResult *& resultList,
                                              bdTaskResult **& resultRefsList,
                                              bdUInt & maxResults) const
{
    resultList = m_taskResults;
    resultRefsList = m_taskResultRefs;
    maxResults = m_maxResults;

    return m_serializeOk &&
        ((m_taskResults || m_taskResultRefs) ? m_maxResults : 0);
}
