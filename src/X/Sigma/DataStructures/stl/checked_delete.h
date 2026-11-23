// Original file: X/Sigma/DataStructures/stl/checked_delete.h

#pragma once

#include "../../../../Contrib/Contrib/blz/include/blz/util_pair.h"








template <typename T>
inline void checked_delete(T * x)  // DWARF checked_delete.h:15
{
    typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
    (void)sizeof(type_must_be_complete);
    delete x;
}








template <typename P>
struct checked_second_deleter  // DWARF checked_delete.h:29
{
    void operator()(const P & pair) const { checked_delete(pair.second); }
};
