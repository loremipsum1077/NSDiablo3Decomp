// Original file: X/Common/OnlineService/Callback/d3boost/type_traits/function_traits.hpp
// boost::function_traits under Blizzard's d3boost namespace. The DWARF has only the
// instantiations; arities 0 and 1 are what this tree uses so far.

#pragma once

namespace d3boost {
namespace detail {

template <typename Function> struct function_traits_helper;

template <typename R> struct function_traits_helper<R (*)(void)> {
    static const unsigned arity = 0;
    typedef R result_type;
};

template <typename R, typename T1> struct function_traits_helper<R (*)(T1)> {
    static const unsigned arity = 1;
    typedef R result_type;
    typedef T1 arg1_type;
    typedef T1 argument_type;
};

template <typename R, typename T1, typename T2> struct function_traits_helper<R (*)(T1, T2)> {
    static const unsigned arity = 2;
    typedef R result_type;
    typedef T1 arg1_type;
    typedef T2 arg2_type;
    typedef T1 first_argument_type;
    typedef T2 second_argument_type;
};

template <typename R, typename T1, typename T2, typename T3>
struct function_traits_helper<R (*)(T1, T2, T3)> {
    static const unsigned arity = 3;
    typedef R result_type;
    typedef T1 arg1_type;
    typedef T2 arg2_type;
    typedef T3 arg3_type;
};

template <typename R, typename T1, typename T2, typename T3, typename T4>
struct function_traits_helper<R (*)(T1, T2, T3, T4)> {
    static const unsigned arity = 4;
    typedef R result_type;
    typedef T1 arg1_type;
    typedef T2 arg2_type;
    typedef T3 arg3_type;
    typedef T4 arg4_type;
};

}  // namespace detail

template <typename Function>
struct function_traits : public d3boost::detail::function_traits_helper<Function *> {};

}  // namespace d3boost
