#include "bdAuthenticationTag.h"
#include "../../bdPlatform/bdPlatformLog/bdPlatformLog.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"











bdAuthenticationTag::bdAuthenticationTag()
{
    bdMemset(m_tag, 0, BD_TAG_SIZE_MAX);
    m_tagLength = BD_TAG_SIZE_MAX;
}

bdAuthenticationTag::bdAuthenticationTag(const bdUByte8 * tag, const bdUByte8 tagLength)
{
    setTag(tag, tagLength);
}


bdAuthenticationTag::~bdAuthenticationTag()
{
}

bdBool bdAuthenticationTag::setTag(const bdUByte8 * tag, const bdUByte8 tagLength)
{
    bdBool ok = false;



    if (tagLength >= BD_TAG_SIZE_MIN && tagLength <= BD_TAG_SIZE_MAX)
    {
        m_tagLength = tagLength;
        bdMemcpy(m_tag, tag, tagLength);
        bdMemset(m_tag + m_tagLength, 0, BD_TAG_SIZE_MAX - m_tagLength);
        ok = true;
    }
    else
    {
        bdLogError("crypto", "Invalid tag length %u, must be between %u and %u", tagLength, BD_TAG_SIZE_MIN, BD_TAG_SIZE_MAX);
    }
    return ok;
}


bdUByte8 * bdAuthenticationTag::getTag()
{
    return m_tag;
}

bdUByte8 bdAuthenticationTag::getTagLength() const
{
    return m_tagLength;
}
