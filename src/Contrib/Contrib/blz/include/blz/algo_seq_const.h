#pragma once

// Non-mutating sequence algorithms. for_each (line 53), find (line 73) and
// find_if (line 85; loop condition at line 87) are recorded.

#include "config.h"
#include "iter_types.h"

namespace blz {

template <typename InputIt, typename Pred>
inline bool all_of(InputIt first, InputIt last, Pred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            return false;
    return true;
}

template <typename InputIt, typename Pred>
inline bool any_of(InputIt first, InputIt last, Pred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            return true;
    return false;
}

template <typename InputIt, typename Pred>
inline bool none_of(InputIt first, InputIt last, Pred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            return false;
    return true;
}

// line 53
template <typename InputIt, typename Function>
inline Function for_each(InputIt first, InputIt last, Function f) {
    for (; first != last; ++first)
        f(*first);
    return f;
}

// line 73. One exit, like find_if below: a second `return last` exit leaves the
// iterator on the stack in Console::Online::TaskManager::RemoveTask.
template <typename InputIt, typename T>
inline InputIt find(InputIt first, InputIt last, const T& value) {
    for (; first != last; ++first)
        if (*first == value)
            break;
    return first;
}

// line 85. One exit: returning `last` from a second exit leaves the iterators on the stack in
// XVarRegistry::_AddScope and XVarScopeInternal::FindVar.
template <typename InputIt, typename Pred>
inline InputIt find_if(InputIt first, InputIt last, Pred pred) {
    for (; first != last; ++first)
        if (pred(*first))
            break;
    return first;
}

template <typename InputIt, typename Pred>
inline InputIt find_if_not(InputIt first, InputIt last, Pred pred) {
    for (; first != last; ++first)
        if (!pred(*first))
            return first;
    return last;
}

template <typename InputIt, typename T>
inline typename iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last,
                                                                const T& value) {
    typename iterator_traits<InputIt>::difference_type n = 0;
    for (; first != last; ++first)
        if (*first == value)
            ++n;
    return n;
}

template <typename InputIt, typename Pred>
inline typename iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last,
                                                                   Pred pred) {
    typename iterator_traits<InputIt>::difference_type n = 0;
    for (; first != last; ++first)
        if (pred(*first))
            ++n;
    return n;
}

template <typename InputIt1, typename InputIt2>
inline bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for (; first1 != last1; ++first1, ++first2)
        if (!(*first1 == *first2))
            return false;
    return true;
}

template <typename InputIt1, typename InputIt2, typename Pred>
inline bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, Pred pred) {
    for (; first1 != last1; ++first1, ++first2)
        if (!pred(*first1, *first2))
            return false;
    return true;
}

template <typename InputIt1, typename InputIt2>
inline bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                    InputIt2 last2) {
    for (; first2 != last2; ++first1, ++first2) {
        if (first1 == last1 || *first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;
    }
    return false;
}

}  // namespace blz
