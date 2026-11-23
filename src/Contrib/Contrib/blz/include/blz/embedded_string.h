#pragma once

// blz::basic_embedded_string<T, N>: a basic_string whose in-object buffer is
// N chars instead of BUF_SIZE. The base's m_storage (16 chars at 0x18) runs
// on into m_extra_storage (N - 16 chars at 0x28), so _init only has to set
// the capacity to N - 1 after the base _init (line 282 in the binary: the
// capacity word becomes N - 1 with the embedded bit still set). Member list
// and names from the DWARF; the bodies from the out-of-line
// basic_embedded_string<char, 128>(const char*, const allocator&) and
// operator=(const char*) and the inlined copies of the others.

#include "config.h"
#include "mem_alloc.h"
#include "string.h"
#include "string_view.h"

namespace blz {

// line 15
template <typename T, unsigned long N, typename Traits, typename Alloc>
class basic_embedded_string : public basic_string<T, Traits, Alloc> {
public:
    typedef basic_string<T, Traits, Alloc> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::size_type size_type;

    // line 46
    basic_embedded_string()
    {
        _init();
    }

    explicit basic_embedded_string(const Alloc& alloc)
        : base_type(alloc)
    {
        _init();
    }

    // line 59
    basic_embedded_string(const base_type& str)
    {
        _init();
        this->assign(str, 0, base_type::npos);
    }

    basic_embedded_string(const basic_embedded_string& str)
    {
        _init();
        this->assign(str, 0, base_type::npos);
    }

    basic_embedded_string(base_type&& str)
    {
        _init();
        this->_assign_rv(blz::move(str));
    }

    basic_embedded_string(basic_embedded_string&& str)
    {
        _init();
        this->_assign_rv(blz::move(str));
    }

    basic_embedded_string(const value_type* s, size_type n, const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init();
        this->assign(s, n);
    }

    // line 115
    basic_embedded_string(const value_type* s, const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init();
        size_type n = Traits::length(s);
        this->assign(s, n);
    }

    basic_embedded_string(size_type n, value_type c, const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init();
        this->assign(n, c);
    }

    explicit basic_embedded_string(const basic_string_view<T, Traits>& sv)
    {
        _init();
        this->assign(sv.data(), sv.size());
    }

    basic_embedded_string& operator=(const basic_embedded_string& str)
    {
        if (this != &str)
            this->assign(str);
        return *this;
    }

    // line 205
    basic_embedded_string& operator=(basic_embedded_string&& str)
    {
        if (this != &str)
            this->_assign_rv(blz::move(str));
        return *this;
    }

    // line 214
    basic_embedded_string& operator=(const value_type* s)
    {
        this->assign(s);
        return *this;
    }

    basic_embedded_string& operator=(value_type c)
    {
        this->assign(1, c);
        return *this;
    }

private:
    // line 279
    void _init()
    {
        this->m_capacity = N - 1;
    }

    value_type m_extra_storage[N - base_type::BUF_SIZE];  // 0x28
};

template <unsigned long N>
using embedded_string = basic_embedded_string<char, N, char_traits<char>, allocator<char> >;

}  // namespace blz
