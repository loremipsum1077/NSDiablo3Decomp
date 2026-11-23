#pragma once

// blz type traits. The DWARF records integral_constant, bool_constant,
// conditional, remove_const, add_lvalue_reference, remove_extent, _is_signed,
// is_signed, is_trivially_copy_constructible, is_trivially_move_constructible,
// is_trivially_destructible, restore_cv, aligned_storage, decay and enable_if
// (in that line order). Every other trait here is unrecorded (it leaves no
// trace in the binary) and written the conventional way, in the same relative
// order libc++ uses. is_fundamental, is_convertible, is_function, is_base_of
// and decay are named by mangled blz helpers (_construct, pair, invoke).

#include "config.h"

namespace blz {

// line 166
template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    typedef T value_type;
    typedef integral_constant type;
    constexpr operator value_type() const { return value; }
    constexpr value_type operator()() const { return value; }
};

template <typename T, T v>
constexpr T integral_constant<T, v>::value;

// line 184
template <bool B>
using bool_constant = integral_constant<bool, B>;
typedef bool_constant<true> true_type;
typedef bool_constant<false> false_type;

// line 204
template <bool, typename T, typename F> struct conditional { typedef F type; };
template <typename T, typename F> struct conditional<true, T, F> { typedef T type; };

template <typename T, typename U> struct is_same : false_type {};
template <typename T> struct is_same<T, T> : true_type {};

// line 216
template <typename T> struct remove_const { typedef T type; };
template <typename T> struct remove_const<const T> { typedef T type; };
template <typename T> struct remove_volatile { typedef T type; };
template <typename T> struct remove_volatile<volatile T> { typedef T type; };
template <typename T> struct remove_cv {
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template <typename T> struct add_const { typedef const T type; };
template <typename T> struct add_volatile { typedef volatile T type; };
template <typename T> struct add_cv { typedef const volatile T type; };

template <typename T> struct remove_reference { typedef T type; };
template <typename T> struct remove_reference<T&> { typedef T type; };
template <typename T> struct remove_reference<T&&> { typedef T type; };

// line 268
template <typename T> struct add_lvalue_reference { typedef T& type; };
template <> struct add_lvalue_reference<void> { typedef void type; };
template <> struct add_lvalue_reference<const void> { typedef const void type; };
template <> struct add_lvalue_reference<volatile void> { typedef volatile void type; };
template <> struct add_lvalue_reference<const volatile void> { typedef const volatile void type; };

template <typename T> struct add_rvalue_reference { typedef T&& type; };
template <> struct add_rvalue_reference<void> { typedef void type; };
template <> struct add_rvalue_reference<const void> { typedef const void type; };
template <> struct add_rvalue_reference<volatile void> { typedef volatile void type; };
template <> struct add_rvalue_reference<const volatile void> { typedef const volatile void type; };

template <typename T> struct remove_pointer { typedef T type; };
template <typename T> struct remove_pointer<T*> { typedef T type; };
template <typename T> struct remove_pointer<T* const> { typedef T type; };
template <typename T> struct remove_pointer<T* volatile> { typedef T type; };
template <typename T> struct remove_pointer<T* const volatile> { typedef T type; };

template <typename T> struct add_pointer { typedef typename remove_reference<T>::type* type; };

// line 296
template <typename T> struct remove_extent { typedef T type; };
template <typename T> struct remove_extent<T[]> { typedef T type; };
template <typename T, size_t N> struct remove_extent<T[N]> { typedef T type; };

template <typename T> struct remove_all_extents { typedef T type; };
template <typename T> struct remove_all_extents<T[]> { typedef typename remove_all_extents<T>::type type; };
template <typename T, size_t N> struct remove_all_extents<T[N]> {
    typedef typename remove_all_extents<T>::type type;
};

template <typename T> struct is_const : false_type {};
template <typename T> struct is_const<const T> : true_type {};
template <typename T> struct is_volatile : false_type {};
template <typename T> struct is_volatile<volatile T> : true_type {};

template <typename T> struct _is_void : false_type {};
template <> struct _is_void<void> : true_type {};
template <typename T> struct is_void : _is_void<typename remove_cv<T>::type> {};

template <typename T> struct _is_null_pointer : false_type {};
template <> struct _is_null_pointer<decltype(nullptr)> : true_type {};
template <typename T> struct is_null_pointer : _is_null_pointer<typename remove_cv<T>::type> {};

template <typename T> struct _is_integral : false_type {};
template <> struct _is_integral<bool> : true_type {};
template <> struct _is_integral<char> : true_type {};
template <> struct _is_integral<signed char> : true_type {};
template <> struct _is_integral<unsigned char> : true_type {};
template <> struct _is_integral<wchar_t> : true_type {};
template <> struct _is_integral<char16_t> : true_type {};
template <> struct _is_integral<char32_t> : true_type {};
template <> struct _is_integral<short> : true_type {};
template <> struct _is_integral<unsigned short> : true_type {};
template <> struct _is_integral<int> : true_type {};
template <> struct _is_integral<unsigned int> : true_type {};
template <> struct _is_integral<long> : true_type {};
template <> struct _is_integral<unsigned long> : true_type {};
template <> struct _is_integral<long long> : true_type {};
template <> struct _is_integral<unsigned long long> : true_type {};
template <typename T> struct is_integral : _is_integral<typename remove_cv<T>::type> {};

template <typename T> struct _is_floating_point : false_type {};
template <> struct _is_floating_point<float> : true_type {};
template <> struct _is_floating_point<double> : true_type {};
template <> struct _is_floating_point<long double> : true_type {};
template <typename T> struct is_floating_point : _is_floating_point<typename remove_cv<T>::type> {};

template <typename T> struct is_array : false_type {};
template <typename T> struct is_array<T[]> : true_type {};
template <typename T, size_t N> struct is_array<T[N]> : true_type {};

template <typename T> struct _is_pointer : false_type {};
template <typename T> struct _is_pointer<T*> : true_type {};
template <typename T> struct is_pointer : _is_pointer<typename remove_cv<T>::type> {};

template <typename T> struct is_lvalue_reference : false_type {};
template <typename T> struct is_lvalue_reference<T&> : true_type {};
template <typename T> struct is_rvalue_reference : false_type {};
template <typename T> struct is_rvalue_reference<T&&> : true_type {};
template <typename T> struct is_reference : false_type {};
template <typename T> struct is_reference<T&> : true_type {};
template <typename T> struct is_reference<T&&> : true_type {};

template <typename T> struct is_union : bool_constant<__is_union(T)> {};
template <typename T> struct is_class : bool_constant<__is_class(T)> {};
template <typename T> struct is_enum : bool_constant<__is_enum(T)> {};

// A function type is the only non-reference type that `const` does not change.
template <typename T>
struct is_function : bool_constant<!is_const<const T>::value && !is_reference<T>::value> {};

template <typename T> struct _is_member_pointer : false_type {};
template <typename T, typename C> struct _is_member_pointer<T C::*> : true_type {};
template <typename T> struct is_member_pointer : _is_member_pointer<typename remove_cv<T>::type> {};

template <typename T> struct _is_member_function_pointer : false_type {};
template <typename T, typename C> struct _is_member_function_pointer<T C::*> : is_function<T> {};
template <typename T>
struct is_member_function_pointer : _is_member_function_pointer<typename remove_cv<T>::type> {};

template <typename T>
struct is_member_object_pointer
    : bool_constant<is_member_pointer<T>::value && !is_member_function_pointer<T>::value> {};

template <typename T>
struct is_arithmetic : bool_constant<is_integral<T>::value || is_floating_point<T>::value> {};

template <typename T>
struct is_fundamental
    : bool_constant<is_arithmetic<T>::value || is_void<T>::value || is_null_pointer<T>::value> {};

template <typename T>
struct is_scalar
    : bool_constant<is_arithmetic<T>::value || is_enum<T>::value || is_pointer<T>::value ||
                    is_member_pointer<T>::value || is_null_pointer<T>::value> {};

template <typename T>
struct is_object
    : bool_constant<is_scalar<T>::value || is_array<T>::value || is_union<T>::value ||
                    is_class<T>::value> {};

template <typename T> struct is_compound : bool_constant<!is_fundamental<T>::value> {};

template <typename T> struct is_empty : bool_constant<__is_empty(T)> {};
template <typename T> struct is_polymorphic : bool_constant<__is_polymorphic(T)> {};
template <typename T> struct is_abstract : bool_constant<__is_abstract(T)> {};
template <typename T> struct is_final : bool_constant<__is_final(T)> {};
template <typename T> struct is_pod : bool_constant<__is_pod(T)> {};
template <typename T> struct is_trivial : bool_constant<__is_trivial(T)> {};
template <typename T> struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};
template <typename T> struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)> {};
template <typename T> struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};

template <typename B, typename D> struct is_base_of : bool_constant<__is_base_of(B, D)> {};
template <typename From, typename To>
struct is_convertible : bool_constant<__is_convertible_to(From, To)> {};

template <typename T> struct alignment_of : integral_constant<size_t, alignof(T)> {};

template <typename T> struct rank : integral_constant<size_t, 0> {};
template <typename T> struct rank<T[]> : integral_constant<size_t, rank<T>::value + 1> {};
template <typename T, size_t N> struct rank<T[N]> : integral_constant<size_t, rank<T>::value + 1> {};

template <typename T, unsigned I = 0> struct extent : integral_constant<size_t, 0> {};
template <typename T> struct extent<T[], 0> : integral_constant<size_t, 0> {};
template <typename T, unsigned I> struct extent<T[], I> : extent<T, I - 1> {};
template <typename T, size_t N> struct extent<T[N], 0> : integral_constant<size_t, N> {};
template <typename T, size_t N, unsigned I> struct extent<T[N], I> : extent<T, I - 1> {};

// line 711
template <typename T> struct _is_signed : false_type {};
template <> struct _is_signed<signed char> : true_type {};
template <> struct _is_signed<char> : bool_constant<(char)-1 < (char)0> {};
template <> struct _is_signed<wchar_t> : bool_constant<(wchar_t)-1 < (wchar_t)0> {};
template <> struct _is_signed<short> : true_type {};
template <> struct _is_signed<int> : true_type {};
template <> struct _is_signed<long> : true_type {};
template <> struct _is_signed<long long> : true_type {};
template <> struct _is_signed<float> : true_type {};
template <> struct _is_signed<double> : true_type {};
template <> struct _is_signed<long double> : true_type {};

// line 724
template <typename T> struct is_signed : _is_signed<typename remove_cv<T>::type> {};
template <typename T>
struct is_unsigned : bool_constant<is_arithmetic<T>::value && !is_signed<T>::value> {};

template <typename T, typename... Args>
struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};
template <typename T>
struct is_default_constructible : is_constructible<T> {};
template <typename T>
struct is_copy_constructible : is_constructible<T, typename add_lvalue_reference<const T>::type> {};
template <typename T>
struct is_move_constructible : is_constructible<T, typename add_rvalue_reference<T>::type> {};

template <typename T, typename U>
struct is_assignable : bool_constant<__is_assignable(T, U)> {};
template <typename T>
struct is_copy_assignable : is_assignable<typename add_lvalue_reference<T>::type,
                                          typename add_lvalue_reference<const T>::type> {};
template <typename T>
struct is_move_assignable : is_assignable<typename add_lvalue_reference<T>::type,
                                          typename add_rvalue_reference<T>::type> {};

template <typename T, typename... Args>
struct is_trivially_constructible : bool_constant<__is_trivially_constructible(T, Args...)> {};
template <typename T>
struct is_trivially_default_constructible : is_trivially_constructible<T> {};

// line 850. The DWARF shows the base as integral_constant<bool, X>::type.
template <typename T>
struct is_trivially_copy_constructible
    : integral_constant<bool, __is_trivially_constructible(T, typename add_lvalue_reference<const T>::type)>::type {};

// line 855
template <typename T>
struct is_trivially_move_constructible
    : integral_constant<bool, __is_trivially_constructible(T, typename add_rvalue_reference<T>::type)>::type {};

template <typename T, typename U>
struct is_trivially_assignable : bool_constant<__is_trivially_assignable(T, U)> {};
template <typename T>
struct is_trivially_copy_assignable
    : is_trivially_assignable<typename add_lvalue_reference<T>::type,
                              typename add_lvalue_reference<const T>::type> {};
template <typename T>
struct is_trivially_move_assignable
    : is_trivially_assignable<typename add_lvalue_reference<T>::type,
                              typename add_rvalue_reference<T>::type> {};

template <typename T, typename... Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(T, Args...)> {};

// line 905. The DWARF shows the base spelled bool_constant<__has_trivial_destructor(T)>.
template <typename T>
struct is_trivially_destructible : bool_constant<__has_trivial_destructor(T)> {};

template <typename T> struct underlying_type { typedef __underlying_type(T) type; };

template <typename T> struct _make_unsigned { typedef T type; };
template <> struct _make_unsigned<char> { typedef unsigned char type; };
template <> struct _make_unsigned<signed char> { typedef unsigned char type; };
template <> struct _make_unsigned<short> { typedef unsigned short type; };
template <> struct _make_unsigned<int> { typedef unsigned int type; };
template <> struct _make_unsigned<long> { typedef unsigned long type; };
template <> struct _make_unsigned<long long> { typedef unsigned long long type; };

template <typename T> struct _make_signed { typedef T type; };
template <> struct _make_signed<char> { typedef signed char type; };
template <> struct _make_signed<unsigned char> { typedef signed char type; };
template <> struct _make_signed<unsigned short> { typedef short type; };
template <> struct _make_signed<unsigned int> { typedef int type; };
template <> struct _make_signed<unsigned long> { typedef long type; };
template <> struct _make_signed<unsigned long long> { typedef long long type; };

// line 988: copies the cv-qualifiers of U onto T.
template <typename T, typename U>
struct restore_cv {
    typedef typename conditional<is_const<U>::value, const T, T>::type _const_type;
    typedef typename conditional<is_volatile<U>::value, volatile _const_type, _const_type>::type type;
};

template <typename T>
struct make_unsigned {
    typedef typename restore_cv<typename _make_unsigned<typename remove_cv<T>::type>::type, T>::type type;
};

template <typename T>
struct make_signed {
    typedef typename restore_cv<typename _make_signed<typename remove_cv<T>::type>::type, T>::type type;
};

// line 1084
template <size_t Len, size_t Align>
struct aligned_storage {
    struct type {
        alignas(Align) char data[Len];
    };
};

// line 1114
template <typename T>
struct decay {
    typedef typename remove_reference<T>::type U;
    typedef typename conditional<
        is_array<U>::value, typename remove_extent<U>::type*,
        typename conditional<is_function<U>::value, typename add_pointer<U>::type,
                             typename remove_cv<U>::type>::type>::type type;
};

// line 1134
template <bool, typename T = void> struct enable_if {};
template <typename T> struct enable_if<true, T> { typedef T type; };

// Not recorded in the DWARF (they leave no code); used by name in mangled
// blz helpers as blz::forward and blz::declval.
template <typename T>
typename add_rvalue_reference<T>::type declval();

template <typename T>
inline constexpr T&& forward(typename remove_reference<T>::type& t) {
    return static_cast<T&&>(t);
}

template <typename T>
inline constexpr T&& forward(typename remove_reference<T>::type&& t) {
    return static_cast<T&&>(t);
}

template <typename T>
inline constexpr typename remove_reference<T>::type&& move(T&& t) {
    return static_cast<typename remove_reference<T>::type&&>(t);
}

template <typename T>
inline T* addressof(T& x) {
    return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(x)));
}

}  // namespace blz
