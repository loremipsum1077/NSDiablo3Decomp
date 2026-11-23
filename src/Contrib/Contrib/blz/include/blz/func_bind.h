#pragma once

// blz::bind / blz::_bind_object. The DWARF records, in line order: the
// constructor (171), operator()<>() (178), _call<0, 1, tuple<>> (186) and
// bind (233); _bind_result<tuple<>>::type is `void`. Members: m_f (the
// decayed callable, 0x00) and m_bound_args (a tuple of the decayed bound
// arguments, next). The result type of the call is not recorded beyond the
// void one; _bind_result is the general decltype(invoke(...)) form.
// Passing call-time arguments through is not recorded either (every use in
// the binary calls with none): they are accepted but not appended.

#include "config.h"
#include "func_invoke.h"
#include "tuple.h"
#include "type_traits.h"
#include "util_int_seq.h"

namespace blz {

// line 159
template <typename F, typename... BoundArgs>
class _bind_object {
    typedef typename decay<F>::type FD;
    typedef tuple<typename decay<BoundArgs>::type...> TD;

    template <typename CallArgs>
    struct _bind_result {
        typedef decltype(blz::invoke(blz::declval<FD&>(),
                                     blz::declval<typename decay<BoundArgs>::type&>()...)) type;
    };

public:
    // line 171
    _bind_object(F&& f, BoundArgs&&... bound_args)
        : m_f(blz::forward<F>(f)), m_bound_args(blz::forward<BoundArgs>(bound_args)...) {}

    // line 178
    template <typename... Args>
    typename _bind_result<tuple<Args&&...> >::type operator()(Args&&... args) {
        return _call(make_index_sequence<sizeof...(BoundArgs)>(),
                     tuple<Args&&...>(blz::forward<Args>(args)...));
    }

private:
    // line 186
    template <size_t... I, typename Tuple>
    typename _bind_result<Tuple>::type _call(index_sequence<I...>, Tuple&&) {
        return blz::invoke(m_f, blz::get<I>(m_bound_args)...);
    }

    FD m_f;
    TD m_bound_args;
};

// line 233
template <typename F, typename... BoundArgs>
inline _bind_object<F, BoundArgs...> bind(F&& f, BoundArgs&&... bound_args) {
    return _bind_object<F, BoundArgs...>(blz::forward<F>(f), blz::forward<BoundArgs>(bound_args)...);
}

}  // namespace blz
