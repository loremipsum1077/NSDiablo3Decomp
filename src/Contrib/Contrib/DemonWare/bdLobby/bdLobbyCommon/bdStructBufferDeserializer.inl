#pragma once

inline bdBool bdStructBufferDeserializer::setLastReadResult(bdReadResult result)
{
    m_lastReadResult = result;
    // The original treats only unsigned status values 0 and 1 as successful reads.
    return static_cast<bdUInt32>(result) < BD_READ_FAILED_STREAM_ERROR;
}

// bdStructBufferDeserializer.inl:112. Only instantiated where bdStructFixedSizeString.h is included.
template <bdUWord MaxLength>
inline bdBool bdStructBufferDeserializer::readString(const bdUInt32 tag, bdStructFixedSizeString<MaxLength> & string)
{
    return readString(tag, string.getBuffer(), static_cast<bdUInt32>(string.getBufferSize()));
}

// bdStructBufferDeserializer.inl:17. Only instantiated where bdArray.h and the element type are included.



template <typename T>
inline bdBool bdStructBufferDeserializer::readObjectArray(const bdUInt32 tag, bdArray<T> & objects)
{
    bool ok = true;
    bool finished = false;
    objects.clear();
    while (ok && !finished)
    {
        T object;
        ok = readObject(tag, object);
        if (ok)
        {
            if (getLastReadResult() == BD_READ_SUCCESS_TAG_NOT_FOUND)
            {
                finished = true;
            }
            else
            {
                objects.pushBack(object);
            }
        }
    }
    return ok;
}
