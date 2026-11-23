#pragma once

// blz string streams. basic_stringbuf is a streambuf over a basic_string
// (m_string at 0x38 after the 0x38 streambuf, m_mode at 0x60; 0x68 in all).
// The string is the storage: the get area and put area point into it and
// _sync() (const, in the original) first grows the string's recorded size to
// cover what has been written through the put area. basic_ostringstream and
// basic_stringstream embed a stringbuf (m_sb) and hand its address to their
// stream base before it is constructed; their destructors are implicit.
// Line numbers are the DWARF's.

#include "config.h"
#include "char_traits.h"
#include "ios.h"
#include "istream.h"
#include "ostream.h"
#include "streambuf.h"
#include "string.h"

namespace blz {

// line 18
template <typename T, typename Traits, typename Alloc>
class basic_stringbuf : public basic_streambuf<T, Traits> {
    basic_stringbuf(const basic_stringbuf&);
    basic_stringbuf& operator=(const basic_stringbuf&);

public:
    typedef basic_string<T, Traits, Alloc> string_type;
    typedef T char_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    // line 38
    explicit basic_stringbuf(ios_base::openmode which) : m_mode(which) {}

    explicit basic_stringbuf(const string_type& s, ios_base::openmode which)
        : m_string(s), m_mode(which) {
        _set_pointers(0, (which & ios_base::ate) ? m_string.size() : 0);
    }

    explicit basic_stringbuf(string_type&& s, ios_base::openmode which)
        : m_string(blz::move(s)), m_mode(which) {
        _set_pointers(0, (which & ios_base::ate) ? m_string.size() : 0);
    }

    // line 76
    virtual ~basic_stringbuf() {
        _sync();
    }

    // line 103
    string_type str() const {
        _sync();
        return m_string;
    }

    void str(const string_type& s) {
        m_string = s;
        _set_pointers(0, (m_mode & ios_base::ate) ? m_string.size() : 0);
    }

    void str(string_type&& s) {
        m_string = blz::move(s);
        _set_pointers(0, (m_mode & ios_base::ate) ? m_string.size() : 0);
    }

    const char_type* c_str() const {
        _sync();
        return m_string.c_str();
    }

    const string_type& str_nocopy() const {
        _sync();
        return m_string;
    }

    string_type&& str_move() {
        _sync();
        return blz::move(m_string);
    }

protected:
    // line 147
    virtual int_type underflow() {
        _sync();
        this->m_gend = this->m_gbegin + m_string.length();
        if (this->m_gnext < this->m_gend)
            return Traits::to_int_type(*this->m_gnext);
        return Traits::eof();
    }

    // line 158
    virtual int_type pbackfail(int_type) {
        return Traits::eof();
    }

    // line 165
    virtual int_type overflow(int_type c) {
        if (!Traits::eq_int_type(c, Traits::eof())) {
            _sync();
            m_string.push_back(Traits::to_char_type(c));
            _set_pointers(this->m_gnext - this->m_gbegin, this->m_pnext + 1 - this->m_pbegin);
        }
        return c;
    }

    // line 179
    virtual basic_streambuf<T, Traits>* setbuf(char*, streamsize) {
        return this;
    }

    // NON_MATCHING: same instructions and tests as the binary (148 bytes against 152),
    // but clang places the in / out / end blocks of the `cur` case in a different
    // order (the binary tests in, then out, then jumps to the join and puts the end
    // case first); switch and if/else spellings tried give the same layout. (line 185)
    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which) {
        if (way == ios_base::cur && (which & (ios_base::in | ios_base::out)) == (ios_base::in | ios_base::out))
            return pos_type(off_type(-1));
        _sync();
        pos_type sp = off;
        if (way == ios_base::beg) {
        } else if (way == ios_base::end) {
            sp = off_type(m_string.size()) + off;
        } else if (way == ios_base::cur) {
            if (which & ios_base::in)
                sp = (this->m_gnext - this->m_gbegin) + off;
            else if (which & ios_base::out)
                sp = (this->m_pnext - this->m_pbegin) + off;
        } else {
            return pos_type(off_type(-1));
        }
        return seekpos(sp, which);
    }

    // line 219
    virtual pos_type seekpos(pos_type sp, ios_base::openmode which) {
        _sync();
        if (which & ios_base::in) {
            char_type* gnext = this->m_gbegin + sp;
            if (gnext > this->m_gend)
                return pos_type(off_type(-1));
            this->m_gnext = gnext;
        }
        if (which & ios_base::out) {
            char_type* pnext = this->m_pbegin + sp;
            if (pnext > this->m_pend)
                return pos_type(off_type(-1));
            this->m_pnext = pnext;
        }
        return sp;
    }

    // line 246
    virtual int sync() {
        _sync();
        return 0;
    }

private:
    // line 254
    void _set_pointers(size_t goff, size_t poff) {
        char_type* gbegin = nullptr;
        char_type* gnext = nullptr;
        char_type* gend = nullptr;
        if (m_mode & ios_base::in) {
            gbegin = m_string.data();
            gnext = gbegin + goff;
            gend = gbegin + m_string.length();
        }
        this->m_gbegin = gbegin;
        this->m_gnext = gnext;
        this->m_gend = gend;
        char_type* pbegin = nullptr;
        char_type* pnext = nullptr;
        char_type* pend = nullptr;
        if (m_mode & ios_base::out) {
            pbegin = m_string.data();
            pnext = pbegin + poff;
            pend = pbegin + m_string.capacity();
        }
        this->m_pbegin = pbegin;
        this->m_pnext = pnext;
        this->m_pend = pend;
    }

    // line 284
    void _sync() const {
        size_t n = this->m_pnext - this->m_pbegin;
        if (n > m_string.size())
            const_cast<string_type&>(m_string).set_size(n);
    }

    string_type m_string;
    ios_base::openmode m_mode;
};

// line 420
template <typename T, typename Traits, typename Alloc>
class basic_ostringstream : public basic_ostream<T, Traits> {
    basic_ostringstream(const basic_ostringstream&);
    basic_ostringstream& operator=(const basic_ostringstream&);

public:
    typedef basic_string<T, Traits, Alloc> string_type;
    typedef T char_type;

    // line 440
    explicit basic_ostringstream(ios_base::openmode which = ios_base::out)
        : basic_ostream<T, Traits>(&m_sb), m_sb(which | ios_base::out) {}

    basic_stringbuf<T, Traits, Alloc>* rdbuf() const {
        return const_cast<basic_stringbuf<T, Traits, Alloc>*>(&m_sb);
    }

    // line 491
    string_type str() const {
        return m_sb.str();
    }

    void str(const string_type& s) {
        m_sb.str(s);
    }

    void str(string_type&& s) {
        m_sb.str(blz::move(s));
    }

    const char_type* c_str() const {
        return m_sb.c_str();
    }

    const string_type& str_nocopy() const {
        return m_sb.str_nocopy();
    }

    string_type&& str_move() {
        return m_sb.str_move();
    }

private:
    basic_stringbuf<T, Traits, Alloc> m_sb;
};

// line 533
template <typename T, typename Traits, typename Alloc>
class basic_stringstream : public basic_iostream<T, Traits> {
    basic_stringstream(const basic_stringstream&);
    basic_stringstream& operator=(const basic_stringstream&);

public:
    typedef basic_string<T, Traits, Alloc> string_type;
    typedef T char_type;

    // line 553
    explicit basic_stringstream(ios_base::openmode which = ios_base::in | ios_base::out)
        : basic_iostream<T, Traits>(&m_sb), m_sb(which) {}

    basic_stringbuf<T, Traits, Alloc>* rdbuf() const {
        return const_cast<basic_stringbuf<T, Traits, Alloc>*>(&m_sb);
    }

    string_type str() const {
        return m_sb.str();
    }

    void str(const string_type& s) {
        m_sb.str(s);
    }

    void str(string_type&& s) {
        m_sb.str(blz::move(s));
    }

    const char_type* c_str() const {
        return m_sb.c_str();
    }

    const string_type& str_nocopy() const {
        return m_sb.str_nocopy();
    }

    string_type&& str_move() {
        return m_sb.str_move();
    }

private:
    basic_stringbuf<T, Traits, Alloc> m_sb;
};

}  // namespace blz
