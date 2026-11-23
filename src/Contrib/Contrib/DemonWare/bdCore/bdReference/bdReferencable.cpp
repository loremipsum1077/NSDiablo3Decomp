// Original file: Contrib/Contrib/DemonWare/bdCore/bdReference/bdReferencable.cpp

#include "bdReferencable.h"

// line 11
bdReferencable::~bdReferencable()
{
}

// line 16
bdReferencable::bdReferencable(const bdReferencable&)
    : m_refCount(0)
{
}

// line 22
const bdReferencable& bdReferencable::operator=(const bdReferencable&)
{
    return *this;
}
