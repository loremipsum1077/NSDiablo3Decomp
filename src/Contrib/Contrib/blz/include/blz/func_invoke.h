#pragma once

// blz::invoke. The two recorded overloads have their exact shape in the
// mangled names:
//   line 57:  enable_if<is_function<F>::value && !is_base_of<C, decay<T0>::type>::value
//                       && !is_reference_wrapper<decay<T0>::type>::value,
//                       decltype(((*blz::forward<T0>(x)).*f)(blz::forward<Args>(args)...))>::type
//             invoke(F C::* f, T0&& x, Args&&... args)
//   line 119: decltype(blz::declval<F>()(blz::declval<Args>()...)) invoke(F&& f, Args&&... args)
// The other overloads (object reference, data members) are not recorded.

#include "config.h"
#include "type_traits.h"

namespace blz {

// Not recorded; blz has no reference_wrapper in this build, so nothing is one.
template <typename T>
struct is_reference_wrapper : false_type {};

// member function, object (or derived) by reference
template <typename F, typename C, typename T0, typename... Args>
inline auto invoke(F C::* f, T0&& x, Args&&... args)
    -> typename enable_if<is_function<F>::value && is_base_of<C, typename decay<T0>::type>::value,
        decltype((blz::forward<T0>(x).*f)(blz::forward<Args>(args)...))>::type {
    return (blz::forward<T0>(x).*f)(blz::forward<Args>(args)...);
}

// line 57: member function, object by pointer (or smart pointer)
template <typename F, typename C, typename T0, typename... Args>
inline auto invoke(F C::* f, T0&& x, Args&&... args)
    -> typename enable_if<is_function<F>::value && !is_base_of<C, typename decay<T0>::type>::value &&
            !is_reference_wrapper<typename decay<T0>::type>::value,
        decltype(((*blz::forward<T0>(x)).*f)(blz::forward<Args>(args)...))>::type {
    return ((*blz::forward<T0>(x)).*f)(blz::forward<Args>(args)...);
}

// data member, object (or derived) by reference
template <typename F, typename C, typename T0>
inline auto invoke(F C::* f, T0&& x)
    -> typename enable_if<!is_function<F>::value && is_base_of<C, typename decay<T0>::type>::value,
        decltype(blz::forward<T0>(x).*f)>::type {
    return blz::forward<T0>(x).*f;
}

// data member, object by pointer
template <typename F, typename C, typename T0>
inline auto invoke(F C::* f, T0&& x)
    -> typename enable_if<!is_function<F>::value && !is_base_of<C, typename decay<T0>::type>::value &&
            !is_reference_wrapper<typename decay<T0>::type>::value,
        decltype((*blz::forward<T0>(x)).*f)>::type {
    return (*blz::forward<T0>(x)).*f;
}

// line 119: any other callable
template <typename F, typename... Args>
inline auto invoke(F&& f, Args&&... args) -> decltype(blz::declval<F>()(blz::declval<Args>()...)) {
    return blz::forward<F>(f)(blz::forward<Args>(args)...);
}

}  // namespace blz
