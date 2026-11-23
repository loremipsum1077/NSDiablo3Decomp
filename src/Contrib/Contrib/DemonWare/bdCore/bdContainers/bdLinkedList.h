// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdLinkedList.h
#pragma once
#include "../bdMemory/bdMemory-std.h"
#include "../../bdPlatform/bdPlatformCoreTypes/bdPlatformCoreTypes.h"

// Source-backed subset used by bdRemoteTaskManager. Layout comes from DWARF;
// bodies are reconstructed from the original task manager call sites.
template <class T> class bdLinkedList {
public:
    struct Node {
        BD_CLASS_ALLOCATOR(Node);
        T m_data;
        Node *m_next;
        Node *m_prev;
        Node(const T &data) : m_data(data) {}
    };
    typedef void *Position;
    bdLinkedList();
    ~bdLinkedList();
    bdBool isEmpty() const;
    bdUInt getSize() const;
    void clear();
    T &getHead();
    const T &getHead() const;
    const T &getAt(const Position position) const;
    T &forward(Position &position);
    const T &forward(Position &position) const;
    Position getHeadPosition() const;
    Position getTailPosition() const;
    void addTail(const T &value);
    void removeHead();
    void removeAt(Position &position);
    void insertAfter(const Position position, const T &value);
protected:
    Node *m_head;
    Node *m_tail;
    bdUInt m_size;
};

#include "bdLinkedList.inl"
