// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdQueue.h
#pragma once
#include "bdLinkedList.h"
template<class T> class bdQueue {
public:
    // Constructor and destructor are implicit (bdQueue.h:19 in the DWARF); value-initialising
    // a bdQueue zeroes its padding, which bdLobbyConnection::close relies on.
    void enqueue(const T &value);
    void dequeue();
    T &peek();
    const T &peek() const;
    bdUInt getSize() const;
    bdBool isEmpty() const;
protected:
    bdLinkedList<T> m_list;
};
#include "bdQueue.inl"
