// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdLinkedList.inl
#pragma once

template <class T>
bdLinkedList<T>::bdLinkedList() : m_head(0), m_tail(0), m_size(0) {}

template <class T>
bdLinkedList<T>::~bdLinkedList() { clear(); }

// Original owner: bdLinkedList.inl:37.
template <class T>
bdBool bdLinkedList<T>::isEmpty() const { return m_size == 0; }

template <class T>
bdUInt bdLinkedList<T>::getSize() const { return m_size; }

template <class T>
void bdLinkedList<T>::clear()
{
    while (m_head) {
        Node *const node = m_head->m_next;
        delete m_head;
        m_head = node;
    }
    m_tail = 0;
    m_size = 0;
}

template <class T>
T &bdLinkedList<T>::getHead() { return m_head->m_data; }

template <class T>
void bdLinkedList<T>::addTail(const T &value)
{
    const Position position = getTailPosition();
    insertAfter(position, value);
}

template <class T>
void bdLinkedList<T>::removeHead()
{
    Position position = getHeadPosition();
    removeAt(position);
}

template <class T>
typename bdLinkedList<T>::Position bdLinkedList<T>::getHeadPosition() const
{
    return m_head;
}

template <class T>
typename bdLinkedList<T>::Position bdLinkedList<T>::getTailPosition() const
{
    return m_tail;
}

template <class T>
void bdLinkedList<T>::removeAt(Position &position)
{
    Node *const node = static_cast<Node *>(position);
    if (node) {
        if (m_head == node) m_head = node->m_next;
        else node->m_prev->m_next = node->m_next;
        if (m_tail == node) m_tail = node->m_prev;
        else node->m_next->m_prev = node->m_prev;
        delete node;
        --m_size;
    }
}

template <class T>
void bdLinkedList<T>::insertAfter(const Position position, const T &value)
{
    Node *const node = static_cast<Node *>(position);
    Node *const newNode = new Node(value);
    if (node) {
        newNode->m_next = node->m_next;
        newNode->m_prev = node;
        if (node->m_next) node->m_next->m_prev = newNode;
        else m_tail = newNode;
        node->m_next = newNode;
    } else {
        newNode->m_next = 0;
        newNode->m_prev = m_tail;
        if (m_tail) m_tail->m_next = newNode;
        else m_head = newNode;
        m_tail = newNode;
    }
    ++m_size;
}

template<class T> const T &bdLinkedList<T>::getHead() const { return m_head->m_data; }
template<class T> T &bdLinkedList<T>::forward(Position &position) {
    Node *const node = static_cast<Node *>(position);
    position = node->m_next;
    return node->m_data;
}
template<class T> const T &bdLinkedList<T>::forward(Position &position) const {
    const Node *const node = static_cast<const Node *>(position);
    position = node->m_next;
    return node->m_data;
}

template<class T> const T &bdLinkedList<T>::getAt(const Position position) const { return static_cast<const Node *>(position)->m_data; }
