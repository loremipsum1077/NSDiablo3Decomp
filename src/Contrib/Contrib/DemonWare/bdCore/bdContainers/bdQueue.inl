// Original file: Contrib/Contrib/DemonWare/bdCore/bdContainers/bdQueue.inl
#pragma once
template<class T> inline void bdQueue<T>::enqueue(const T &value) { m_list.addTail(value); }
template<class T> inline void bdQueue<T>::dequeue() { m_list.removeHead(); }
template<class T> inline T &bdQueue<T>::peek() { return m_list.getHead(); }
template<class T> inline const T &bdQueue<T>::peek() const { return m_list.getHead(); }
template<class T> inline bdUInt bdQueue<T>::getSize() const { return m_list.getSize(); }
template<class T> inline bdBool bdQueue<T>::isEmpty() const { return m_list.isEmpty(); }
