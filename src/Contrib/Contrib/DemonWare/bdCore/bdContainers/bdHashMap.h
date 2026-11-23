// Adopted from the DWARF reference; hashing and task-manager container subset.
// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdHashMap.h
#pragma once
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

class bdHashingClass {
public:
    // line 42. Body reconstructed from bdEndpoint::getHash; the original starts
    // at zero and multiplies by 16777619 before XORing each byte (line 51).
    bdUInt getHash(const bdUByte8 * key, const bdUWord size) const
    {
        const bdUByte8 * val = key;
        bdUInt off = 0;
        bdUInt h = 0;
        for (bdUWord i = 0; i < size; ++i)
            h = h * 16777619U ^ val[off++];
        return h;
    }

    // line 61. The DWARF records this instantiation for bdSecurityID.
    template <typename T>
    bdUInt getHash(const T & key) const
    {
        return getHash(reinterpret_cast<const bdUByte8 *>(&key), sizeof(key));
    }
};
static_assert(sizeof(bdHashingClass) == 1, "bdHashingClass");

#include "../bdString/bdString.h"
// line 86. bdString keys hash their characters, not the object (bdJSONObject callers).
template <>
inline bdUInt bdHashingClass::getHash(const bdString & key) const
{
    return getHash(reinterpret_cast<const bdUByte8 *>(key.getBuffer()), key.getLength());
}

#include "../bdMemory/bdMemory.inl"
#include "../bdUtilities/bdBitOperations.h"
#include "../../bdPlatform/bdPlatformMemory/bdPlatformMemory.inl"
// Source-backed subset reconstructed from the task manager's DWARF and code.
template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS = bdHashingClass> class bdHashMap  // default inferred: bdQoSProbe.cpp column 501:97 fits the two-argument spelling; 24 of the 28 DWARF instance types use bdHashingClass; the other 4 name their hasher
{
public:
    typedef void* Iterator;
    struct Node
    {
        BD_CLASS_ALLOCATOR(Node);
        VALUE_TYPE m_data;
        KEY_TYPE m_key;
        Node* m_next;
        Node(const KEY_TYPE& key, const VALUE_TYPE& value, Node* const next)
            : m_data(value)
            , m_key(key)
            , m_next(next)
        {
        }
    };
    bdHashMap(bdUInt capacity, bdFloat32 load);
    inline ~bdHashMap();  // inferred: in-class `inline` gives clang 5 the inline hint (see bdArray.h); the original inlines it into ~bdNATTravClient (bdNATTravClient.cpp:67) and 6 other functions. Declaring every member inline breaks 47 matches, so only this one.
    bdInt getSize() const;
    bdBool containsKey(const KEY_TYPE& key) const;
    void clear();
    bdBool put(const KEY_TYPE& key, const VALUE_TYPE& value);
    bdBool remove(const KEY_TYPE& key);
    bdBool remove(const KEY_TYPE& key, VALUE_TYPE& value);
    bdBool remove(Iterator& iterator);
    bdBool get(const KEY_TYPE& key, VALUE_TYPE& value) const;
    Iterator getIterator(const KEY_TYPE& key) const;
    Iterator getIterator() const;
    void next(Iterator& iterator) const;
    void releaseIterator(Iterator) const;
    const KEY_TYPE& getKey(Iterator i) const;
    KEY_TYPE& getKey(Iterator i);
    VALUE_TYPE& getValue(Iterator i);
    const VALUE_TYPE& getValue(Iterator i) const;

protected:
    void createMap(bdUInt capacity, bdFloat32 load);
    static bdUInt getNextCapacity(const bdUInt capacity);
    void resize(bdUInt newSize);
    bdUInt getHashIndex(bdUInt hash) const;
    bdUInt m_size, m_capacity;
    bdFloat32 m_loadFactor;
    bdUInt m_threshold;
    Node** m_map;
    HASHING_CLASS m_hashClass;
};
#include "bdHashMap.inl"
