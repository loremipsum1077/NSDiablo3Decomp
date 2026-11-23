// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Sigma/BlzExt/BlzUnorderedMapHelper.h
// Subset: unordered_map_find_ptr (line 31). The body is not in the DWARF; it is written from the
// copies inlined into X/Server/System/SNephalemRift.cpp (find at line 35).

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/unordered_map.h"

namespace blzext {

template <typename K, typename V>
inline V * unordered_map_find_ptr(blz::unordered_map<K, V> & tMap, const K & key)
{
    typename blz::unordered_map<K, V>::iterator it = tMap.find(key);
    if (it == tMap.end())
        return NULL;
    return &it->second;
}

}  // namespace blzext
