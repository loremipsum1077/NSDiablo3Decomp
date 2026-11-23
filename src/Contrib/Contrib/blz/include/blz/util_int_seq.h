#pragma once

// Compile-time integer sequences (used to unpack tuples, e.g. by bind).

#include "config.h"

namespace blz {

// line 16
template <typename T, T... I>
struct integer_sequence {
    typedef T value_type;
    static size_t size() { return sizeof...(I); }
};

template <typename Seq1, typename Seq2> struct _int_seq_concat;
template <typename T, T... I1, T... I2>
struct _int_seq_concat<integer_sequence<T, I1...>, integer_sequence<T, I2...> > {
    typedef integer_sequence<T, I1..., (sizeof...(I1) + I2)...> type;
};

template <typename T, size_t N>
struct _make_int_seq {
    typedef typename _int_seq_concat<typename _make_int_seq<T, N / 2>::type,
                                     typename _make_int_seq<T, N - N / 2>::type>::type type;
};
template <typename T> struct _make_int_seq<T, 0> { typedef integer_sequence<T> type; };
template <typename T> struct _make_int_seq<T, 1> { typedef integer_sequence<T, 0> type; };

template <typename T, T N>
using make_integer_sequence = typename _make_int_seq<T, N>::type;

// line 41
template <size_t... I>
using index_sequence = integer_sequence<size_t, I...>;

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template <typename... T>
using index_sequence_for = make_index_sequence<sizeof...(T)>;

}  // namespace blz
