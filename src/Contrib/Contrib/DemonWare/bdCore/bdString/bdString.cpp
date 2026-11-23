// Original file: Contrib/Contrib/DemonWare/bdCore/bdString/bdString.cpp

#include "bdString.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"
#include "../bdMemory/bdMemory.inl"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString.inl"
#include "../../bdPlatform/bdPlatformString/bdPlatformString-unix.inl"

// Allocations are rounded up to whole blocks of this many bytes (the `tst #0x3f` / `and #~0x3f`
// in allocateBuffer). The name is invented.
#define BD_STRING_BLOCK_SIZE 64

struct bdEmptyStringStatic {  // size 0x20
    bdEmptyStringStatic()
    {
        m_stringData.m_referenceCount = 1;
        m_stringData.m_length = 0;
        m_stringData.m_capacity = 0;
        m_stringByte = '\0';
    }

    bdStringData m_stringData;  // 0x00
    bdNChar8 m_stringByte;  // 0x18
};

// line 44
static bdEmptyStringStatic g_emptyStringData;
// line 46
static bdNChar8* g_emptyString = g_emptyStringData.m_stringData.getString();

// line 50
inline bdStringData* getEmptyStringData()
{
    g_emptyStringData.m_stringByte = '\0';
    return &g_emptyStringData.m_stringData;
}

// line 56
bdString::bdString()
{
    initialize();
}

// line 61
bdString::bdString(const bdString& s)
{
    m_string = s.m_string;
    addReference(getStringData());
}

// line 67
bdString::bdString(const bdNChar8* const s)
{
    const bdUWord length = bdStrlen(s);
    if (length)
    {
        allocateBuffer(length);
        bdMemcpy(m_string, s, length + 1);
    }
    else
    {
        initialize();
    }
}

// line 81
bdString::bdString(const bdNChar8* const s, const bdUWord length)
{
    bdUWord sLength = bdStrnlen(s, length);
    if (sLength)
    {
        allocateBuffer(sLength);
        bdMemcpy(m_string, s, sLength);
        m_string[sLength] = '\0';
    }
    else
    {
        initialize();
    }
}

// line 97
bdString::~bdString()
{
    removeReference(getStringData());
}

// line 102
bdString& bdString::operator=(const bdString& s)
{
    if (m_string != s.m_string)
    {
        removeReference(getStringData());
        m_string = s.m_string;
        addReference(getStringData());
    }
    return *this;
}

// line 116
bdString& bdString::operator=(const bdNChar8* const s)
{
    const bdUWord length = bdStrlen(s);

    if (getStringData()->m_referenceCount > 1 || !enoughCapacity(length))
    {
        removeReference(getStringData());
        allocateBuffer(length);
    }
    else
    {
        getStringData()->m_length = length;
    }

    bdMemcpy(m_string, s, length + 1);
    return *this;
}

// line 137
bdBool bdString::operator==(const bdString& s) const
{
    const bdBool areEqual = bdStrcmp(m_string, s.m_string) == 0;
    return areEqual;
}

// line 319
bdUWord bdString::getLength() const
{
    return getStringData()->m_length;
}

// line 324
const bdNChar8* bdString::getBuffer() const
{
    return m_string;
}

// line 329
inline bdStringData* bdString::getStringData() const
{
    return reinterpret_cast<bdStringData*>(m_string) - 1;
}

// line 334
inline void bdString::initialize()
{
    m_string = getEmptyStringData()->getString();
    addReference(getStringData());
}

// line 340
inline void bdString::addReference(bdStringData* stringData) const
{
    ++stringData->m_referenceCount;
}

// line 345
inline void bdString::removeReference(bdStringData* stringData) const
{
    --stringData->m_referenceCount;
    if (stringData->m_referenceCount == 0)
    {
        freeBuffer(stringData);
    }
}

// line 354
inline void bdString::allocateBuffer(const bdUWord length)
{
    const bdUWord requiredSize = length + 1;
    bdUWord blocks = requiredSize / BD_STRING_BLOCK_SIZE;
    if (requiredSize % BD_STRING_BLOCK_SIZE)
    {
        ++blocks;
    }
    const bdUWord bufferSize = blocks * BD_STRING_BLOCK_SIZE;

    bdStringData* stringData = reinterpret_cast<bdStringData*>(bdAllocate<bdNChar8>(sizeof(bdStringData) + bufferSize));
    stringData->m_referenceCount = 1;
    stringData->m_length = length;
    stringData->m_capacity = bufferSize;

    bdNChar8* const buffer = stringData->getString();
    m_string = buffer;
}

// line 378
inline void bdString::freeBuffer(bdStringData* stringData) const
{
    if (stringData != &g_emptyStringData.m_stringData)
    {
        bdNChar8* data = reinterpret_cast<bdNChar8*>(stringData);
        bdDeallocate(data);
    }
}

// line 387
inline bdBool bdString::enoughCapacity(const bdUWord length) const
{
    return getStringData()->m_capacity >= length + 1;
}
