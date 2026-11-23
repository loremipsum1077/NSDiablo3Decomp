// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdHashMap.inl
#pragma once
// On the original's lines: each member's name, every DWARF local and the statements with line-table rows
// sit on their DWARF lines (build/try/h32/hm_locals.txt; rows from the out-of-line put/remove/resize/clear
// copies and the inlined users in bdSocketRouter.cpp / bdNATTravClient.cpp). Blank lines pad the rest.
// getValue (both) and releaseIterator leave no DWARF record; their lines are not known.








template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::bdHashMap(bdUInt capacity, bdFloat32 load)
{
    createMap(capacity, load);
}





template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::~bdHashMap()
{
    clear();
    bdDeallocate(m_map);
}







template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdInt bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getSize() const
{
    return m_size;
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline void bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::clear()
{






    for (bdUInt i = 0; i < m_capacity; ++i)
    {
        Node* n = m_map[i];
        // Original rows: 57 while, 59 lastNode, 60 advance, 64 slot reset, 66 m_size.
        while (n)
        {
            Node* const lastNode = n;
            n = n->m_next;
            delete lastNode;
        }

        m_map[i] = 0;
    }
    m_size = 0;
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::get(const KEY_TYPE& key, VALUE_TYPE& value) const
{

    const Iterator iterator = getIterator(key);
    if (iterator)
    {
        value = getValue(iterator);
        return true;
    }
    return false;
}





template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::put(const KEY_TYPE& key, const VALUE_TYPE& value)
{






    const bdUInt hash = m_hashClass.getHash(key);
    bdUInt i = getHashIndex(hash);
    Node* n = m_map[i];


    while (n)
    {

        if (key == n->m_key)
            return false;
        n = n->m_next;
    }




    if (m_size + 1 > m_threshold)
    {
        resize(m_capacity * 2);
        i = getHashIndex(hash);
    }
    ++m_size;
    m_map[i] = new Node(key, value, m_map[i]);
    return true;
}
































template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::remove(const KEY_TYPE& key)
{





    const bdUInt hash = m_hashClass.getHash(key);
    const bdUInt i = getHashIndex(hash);

    Node* n = m_map[i];
    Node* prevNode = 0;
    while (n)
    {
        if (key == n->m_key)
        {

            if (prevNode)
                prevNode->m_next = n->m_next;
            else

                m_map[i] = n->m_next;
            delete n;
            --m_size;
            return true;
        }
        prevNode = n;
        n = n->m_next;
    }
    return false;
}










template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::remove(const KEY_TYPE& key, VALUE_TYPE& value)
{

    const bdUInt hash = m_hashClass.getHash(key);
    const bdUInt i = getHashIndex(hash);
    Node* n = m_map[i];
    Node* prevNode = 0;






    while (n)
    {
        if (key == n->m_key)
        {

            if (prevNode)
                prevNode->m_next = n->m_next;
            else

                m_map[i] = n->m_next;
            value = n->m_data;
            delete n;
            --m_size;
            return true;
        }
        prevNode = n;
        n = n->m_next;
    }
    return false;
}










template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::remove(

    Iterator& iterator)
{
    // DWARF form (bdQoSProbe CU abstract DIE: iterator 241, iter 246, n 248, result 255; no guard;
    // remove(key) out of line at 255:19, only next() inlined). h17 check over 129 includers: no match lost;
    // sizes ours->new (original): bdQoSProbe::handleReply 708->720 (720), bdSocketRouter::disconnect 544->512 (520),
    Iterator iter = iterator;
    next(iterator);
    Node* n = (Node*)iter;
    // onNATAddrDiscovery 1540->1576 (1536), bdRemoteTaskManager::cleanUpAsyncState 876->908 (848); none was matching. handleReply then matched.





    bdBool result = remove(n->m_key);
    return result;
}




template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdBool bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::containsKey(const KEY_TYPE& key) const
{
    const Iterator iterator = getIterator(key);
    return iterator != 0;
}

























template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline typename bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::Iterator bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getIterator() const
{
    if (m_size == 0)
    {
        return 0;
    }


    bdUInt i = 0;
    while ((i < m_capacity) && (m_map[i] == 0))
    {
        i++;
    }







    return m_map[i];
}


template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline typename bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::Iterator bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getIterator(const KEY_TYPE& key) const
{
    if (m_size == 0)
    {
        return 0;
    }

    const bdUInt hash = m_hashClass.getHash(key);
    const bdUInt i = getHashIndex(hash);
    Node* n = m_map[i];

    while (n)
    {
        if (key == n->m_key)
            return n;
        n = n->m_next;
    }
    return 0;
}










































template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline void bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::next(Iterator& iterator) const
{
    const Node* const n = static_cast<const Node*>(iterator);
    if (n->m_next)
    {
        iterator = n->m_next;
    }
    else
    {

        const bdUInt hash = m_hashClass.getHash(n->m_key);
        bdUInt i = getHashIndex(hash) + 1;
        while (i < m_capacity)
        {
            if (m_map[i])
            {
                iterator = m_map[i];
                return;
            }
            ++i;
        }
        iterator = 0;
    }
}









template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline const KEY_TYPE& bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getKey(Iterator iterator) const
{
    const Node* node = static_cast<const Node*>(iterator);
    return node->m_key;
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline KEY_TYPE& bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getKey(Iterator i)
{
    return ((Node*)i)->m_key;
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline VALUE_TYPE& bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getValue(Iterator i)
{
    return ((Node*)i)->m_data;
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline const VALUE_TYPE& bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getValue(Iterator i) const
{
    return ((Node*)i)->m_data;
}



























template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline void bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::createMap(bdUInt capacity, bdFloat32 load)
{
    // Rows 476-480 (bdNATTravClient ctor): m_size reset after member construction, then the members in order.







    m_size = 0;
    m_capacity = getNextCapacity(capacity);
    m_loadFactor = load;
    m_threshold = static_cast<bdUInt>(m_capacity * m_loadFactor);
    m_map = bdAllocate<Node*>(m_capacity);


    bdMemset(m_map, 0, m_capacity * sizeof(Node*));
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdUInt bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getNextCapacity(const bdUInt capacity)
{

















    const bdUInt nextCapacity = bdBitOperations::nextPowerOf2(capacity);
    return nextCapacity;
}



template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline void bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::resize(bdUInt newSize)
{
    Node** oldmap = m_map;

    const bdUInt oldCapacity = m_capacity;


    const bdUInt targetCapacity = getNextCapacity(newSize);


    if (targetCapacity > m_capacity)
    {



        m_threshold = (bdUInt)(m_loadFactor * targetCapacity);

        m_capacity = targetCapacity;
        m_map = bdAllocate<Node*>(targetCapacity);
        m_size = 0;

        bdMemset(m_map, 0, m_capacity * sizeof(Node*));




        for (bdUInt i = 0; i < oldCapacity; ++i)
        {
            Node* n = oldmap[i];
            while (n)
            {
                put(n->m_key, n->m_data);
                Node* prev = n;
                n = n->m_next;
                delete prev;
            }
        }
        bdDeallocate(oldmap);
    }
}

template <class KEY_TYPE, class VALUE_TYPE, class HASHING_CLASS>
inline bdUInt bdHashMap<KEY_TYPE, VALUE_TYPE, HASHING_CLASS>::getHashIndex(bdUInt hash) const
{



    return hash & (m_capacity - 1);
}
