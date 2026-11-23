#pragma once

// blz::basic_external_string: a basic_string that starts out in a buffer
// the caller provides (m_external_buffer) and only allocates when it
// outgrows it. Layout and member list from the DWARF. Nothing in the binary
// has code from this header (the constructor at line 52 and _init at line
// 247 are recorded as inline only), so the bodies are a guess: the buffer is
// installed the way the base installs its own m_storage, with the embedded
// bit left set so that the base never frees it.

#include "config.h"
#include "mem_alloc.h"
#include "string.h"
#include "string_view.h"

namespace blz {

// line 15
template <typename T, typename Traits, typename Alloc>
class basic_external_string : public basic_string<T, Traits, Alloc> {
public:
    typedef basic_string<T, Traits, Alloc> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::size_type size_type;

    // line 52
    basic_external_string(value_type* buffer, size_type buffer_size, const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init(buffer, buffer_size);
    }

    basic_external_string(value_type* buffer, size_type buffer_size, const base_type& str)
    {
        _init(buffer, buffer_size);
        this->assign(str);
    }

    basic_external_string(value_type* buffer, size_type buffer_size, const value_type* s,
                          const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init(buffer, buffer_size);
        this->assign(s);
    }

    basic_external_string(value_type* buffer, size_type buffer_size, const value_type* s, size_type n,
                          const Alloc& alloc = Alloc())
        : base_type(alloc)
    {
        _init(buffer, buffer_size);
        this->assign(s, n);
    }

    basic_external_string& operator=(const base_type& str)
    {
        this->assign(str);
        return *this;
    }

    basic_external_string& operator=(const value_type* s)
    {
        this->assign(s);
        return *this;
    }

    void set_storage(value_type* buffer, size_type buffer_size)
    {
        this->_deallocate_elements();
        _init(buffer, buffer_size);
    }

private:
    basic_external_string(const basic_external_string&);

    // line 247
    void _init(value_type* buffer, size_type buffer_size)
    {
        m_external_buffer = buffer;
        this->m_elements = buffer;
        this->m_size = 0;
        this->m_capacity = buffer_size - 1;
        this->m_capacity_is_embedded = 1;
        buffer[0] = value_type();
    }

    value_type* m_external_buffer;  // 0x28
};

typedef basic_external_string<char, char_traits<char>, allocator<char> > external_string;

}  // namespace blz
