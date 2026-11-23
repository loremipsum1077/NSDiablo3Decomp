// Original file: X/Common/OnlineService/Callback/d3boost/detail/call_traits.hpp
// boost::call_traits under Blizzard's d3boost namespace: small arithmetic and pointer types
// are passed by const value, everything else by const reference, references as they are.

#pragma once

namespace d3boost {

template <typename T> struct is_pointer { static const bool value = false; };
template <typename T> struct is_pointer<T *> { static const bool value = true; };

template <typename T> struct is_arithmetic { static const bool value = false; };
template <> struct is_arithmetic<bool> { static const bool value = true; };
template <> struct is_arithmetic<char> { static const bool value = true; };
template <> struct is_arithmetic<signed char> { static const bool value = true; };
template <> struct is_arithmetic<unsigned char> { static const bool value = true; };
template <> struct is_arithmetic<wchar_t> { static const bool value = true; };
template <> struct is_arithmetic<short> { static const bool value = true; };
template <> struct is_arithmetic<unsigned short> { static const bool value = true; };
template <> struct is_arithmetic<int> { static const bool value = true; };
template <> struct is_arithmetic<unsigned int> { static const bool value = true; };
template <> struct is_arithmetic<long> { static const bool value = true; };
template <> struct is_arithmetic<unsigned long> { static const bool value = true; };
template <> struct is_arithmetic<long long> { static const bool value = true; };
template <> struct is_arithmetic<unsigned long long> { static const bool value = true; };
template <> struct is_arithmetic<float> { static const bool value = true; };
template <> struct is_arithmetic<double> { static const bool value = true; };
template <> struct is_arithmetic<long double> { static const bool value = true; };

template <typename T> struct is_enum { static const bool value = __is_enum(T); };

namespace detail {

template <typename T, bool small_> struct ct_imp2 {
    typedef const T & param_type;
};
template <typename T> struct ct_imp2<T, true> {
    typedef const T param_type;
};

template <typename T, bool isp, bool b1, bool b2> struct ct_imp {
    typedef const T & param_type;
};
template <typename T, bool isp, bool b2> struct ct_imp<T, isp, true, b2> {
    typedef typename ct_imp2<T, sizeof(T) <= sizeof(void *)>::param_type param_type;
};
// Enums go by const reference: the original mangles callback_base<1, Callback<void (MailResult)>>::execute
// as execute(MailResult const &).
template <typename T, bool isp, bool b1> struct ct_imp<T, isp, b1, true> {
    typedef const T & param_type;
};
template <typename T, bool b1, bool b2> struct ct_imp<T, true, b1, b2> {
    typedef const T param_type;
};

}  // namespace detail

template <typename T> struct call_traits {
    typedef T value_type;
    typedef T & reference;
    typedef const T & const_reference;
    typedef typename d3boost::detail::ct_imp<T, d3boost::is_pointer<T>::value, d3boost::is_arithmetic<T>::value,
                                             d3boost::is_enum<T>::value>::param_type param_type;
};

template <typename T> struct call_traits<T &> {
    typedef T & value_type;
    typedef T & reference;
    typedef const T & const_reference;
    typedef T & param_type;
};

}  // namespace d3boost
