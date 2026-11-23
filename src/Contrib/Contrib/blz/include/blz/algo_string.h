#pragma once

// Search and compare helpers behind basic_string. Names, parameters and
// locals (npos, xpos, i, rlen, clen, result) from the DWARF; bodies from the
// line info of their inlined copies: string_compare clamps rlen at line 37
// (cmp rlen, n1; csel hi) and clen at line 41, calls
// Traits::compare at 44, tests at 45 and returns the length difference at 48;
// string_find bails out at 57 and 59, loops at 62/64 and compares at 66;
// find_first_of tests at 114 and loops at 119/121/123; find_last_of tests at
// 140, clamps at 143/144, compares at 150 and steps back at 154/156;
// find_first_not_of tests at 167 and loops at 172/174/176. Characters are
// compared with == (no Traits::eq shows up in the inlined copies).

#include "config.h"

namespace blz {

// line 32
template <typename Traits>
inline int string_compare(const typename Traits::char_type* s1, size_t s1_size, size_t pos1, size_t n1,
                          const typename Traits::char_type* s2, size_t n2)
{
    size_t rlen = s1_size - pos1;
    if (rlen > n1)
        rlen = n1;

    size_t clen = rlen > n2 ? n2 : rlen;

    int result = Traits::compare(s1 + pos1, s2, clen);
    if (result != 0)
        return result;

    // Both lengths are narrowed before the subtraction: an i64 `rlen - n2`
    // makes InstCombine swap the rlen clamp's compare (csel operands come out
    // reversed) and lets the DAG fold `== 0` into a cmp, where the binary keeps
    // `sub w, w` + cbnz (IsSubSymbol, FindAllExtensionNumbers).
    return static_cast<int>(rlen) - static_cast<int>(n2);
}

// line 53
template <typename Traits>
inline size_t string_find(const typename Traits::char_type* s1, size_t s1_size,
                          const typename Traits::char_type* s2, size_t pos, size_t n)
{
    const size_t npos = static_cast<size_t>(-1);

    if (pos > s1_size)
        return npos;
    if (n == 0)
        return pos;

    for (size_t xpos = pos; xpos + n <= s1_size; ++xpos)
    {
        size_t i;
        for (i = 0; i < n; ++i)
        {
            if (s1[xpos + i] != s2[i])
                break;
        }
        if (i >= n)
            return xpos;
    }

    return npos;
}

// line 110
template <typename Traits>
inline size_t string_find_first_of(const typename Traits::char_type* s1, size_t s1_size,
                                   const typename Traits::char_type* s2, size_t pos, size_t n)
{
    const size_t npos = static_cast<size_t>(-1);

    if (pos < s1_size)
    {
        for (size_t xpos = pos; xpos < s1_size; ++xpos)
        {
            for (size_t i = 0; i < n; ++i)
            {
                if (s1[xpos] == s2[i])
                    return xpos;
            }
        }
    }

    return npos;
}

// line 132
template <typename Traits>
inline size_t string_find_last_of(const typename Traits::char_type* s1, size_t s1_size,
                                  const typename Traits::char_type* s2, size_t pos, size_t n)
{
    const size_t npos = static_cast<size_t>(-1);

    if (s1_size != 0)
    {
        size_t xpos = s1_size - 1;
        if (xpos > pos)
            xpos = pos;

        for (;;)
        {
            for (size_t i = 0; i < n; ++i)
            {
                if (s1[xpos] == s2[i])
                    return xpos;
            }
            if (xpos == 0)
                break;
            --xpos;
        }
    }

    return npos;
}

// line 163
template <typename Traits>
inline size_t string_find_first_not_of(const typename Traits::char_type* s1, size_t s1_size,
                                       const typename Traits::char_type* s2, size_t pos, size_t n)
{
    const size_t npos = static_cast<size_t>(-1);

    if (pos < s1_size)
    {
        for (size_t xpos = pos; xpos < s1_size; ++xpos)
        {
            size_t i;
            for (i = 0; i < n; ++i)
            {
                if (s1[xpos] == s2[i])
                    break;
            }
            if (i >= n)
                return xpos;
        }
    }

    return npos;
}

}  // namespace blz
