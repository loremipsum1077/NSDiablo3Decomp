#include "bdLinkable.h"

// Blank lines keep the original line numbers







bdLinkable::bdLinkable() : m_previous(0), m_next(0), m_headPtr(0) {}






bdLinkable::~bdLinkable() { unlink(); }




void bdLinkable::link(bdLinkable** head)
{

    unlink();
    if (head)
    {
        bdLinkable* const headPtr = *head;
        m_previous = 0;
        m_next = headPtr;
        if (headPtr) headPtr->m_previous = this;
        *head = this;
        m_headPtr = head;
    }
}







void bdLinkable::unlink()
{
    if (m_previous) m_previous->m_next = m_next;
    if (m_next) m_next->m_previous = m_previous;
    if (m_headPtr && *m_headPtr == this) *m_headPtr = m_next;
    m_headPtr = 0;
    m_next = 0;
    m_previous = 0;
}






















inline bdLinkable* bdLinkable::getNext() const { return m_next; }




bdBool bdLinkable::find(const bdLinkable* const list, const bdLinkable* const link)
{
    bdBool found = false;
    const bdLinkable* currentLink = list;
    while (!found && currentLink)
    {
        found = (currentLink == link);

        currentLink = currentLink->getNext();
    }
    return found;
}
