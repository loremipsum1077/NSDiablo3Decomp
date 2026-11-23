#pragma once

// blz::basic_streambuf: the buffer layer under the iostreams. Layout (0x38):
// vptr, then the get area (m_gbegin, m_gnext, m_gend at 0x08..0x18) and the put
// area (m_pbegin, m_pnext, m_pend at 0x20..0x30). Virtual slots, from the
// DWARF: 0/1 destructor, 2 setbuf, 3 seekoff, 4 seekpos, 5 sync, 6 showmanyc,
// 7 xsgetn, 8 underflow, 9 uflow, 10 pbackfail, 11 xsputn, 12 overflow. The
// out-of-line members (showmanyc, xsgetn, uflow, xsputn) are the ones the
// binary emits (xsputn calls memcpy itself, not char_traits::copy); sbumpc (80, body 82-84) and sputn (129, body 131) are
// inlined into callers. Line numbers are the DWARF's.

#include "config.h"
#include "char_traits.h"
#include "ios.h"

#include <string.h>

namespace blz {

// line 16
template <typename T, typename Traits>
class basic_streambuf {
public:
    typedef T char_type;
    typedef typename Traits::int_type int_type;
    typedef typename Traits::pos_type pos_type;
    typedef typename Traits::off_type off_type;

    virtual ~basic_streambuf() {}

    basic_streambuf* pubsetbuf(char_type* s, streamsize n) {
        return setbuf(s, n);
    }

    pos_type pubseekoff(off_type off, ios_base::seekdir way,
                        ios_base::openmode which = ios_base::in | ios_base::out) {
        return seekoff(off, way, which);
    }

    pos_type pubseekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out) {
        return seekpos(sp, which);
    }

    int pubsync() {
        return sync();
    }

    streamsize in_avail() {
        streamsize n = m_gend - m_gnext;
        if (n > 0)
            return n;
        return showmanyc();
    }

    // line 71
    int_type snextc() {
        if (Traits::eq_int_type(sbumpc(), Traits::eof()))
            return Traits::eof();
        return sgetc();
    }

    // line 80
    int_type sbumpc() {
        if (m_gnext == m_gend)
            return uflow();
        return Traits::to_int_type(*m_gnext++);
    }

    // line 88
    int_type sgetc() {
        if (m_gnext == m_gend)
            return underflow();
        return Traits::to_int_type(*m_gnext);
    }

    streamsize sgetn(char_type* s, streamsize n) {
        return xsgetn(s, n);
    }

    int_type sputbackc(char_type c) {
        if (m_gbegin == m_gnext || !Traits::eq(c, m_gnext[-1]))
            return pbackfail(Traits::to_int_type(c));
        --m_gnext;
        return Traits::to_int_type(c);
    }

    int_type sungetc() {
        if (m_gbegin == m_gnext)
            return pbackfail(Traits::eof());
        --m_gnext;
        return Traits::to_int_type(*m_gnext);
    }

    // line 120
    int_type sputc(char_type c) {
        if (m_pnext == m_pend)
            return overflow(Traits::to_int_type(c));
        *m_pnext++ = c;
        return Traits::to_int_type(c);
    }

    // line 129
    streamsize sputn(const char_type* s, streamsize n) {
        return xsputn(s, n);
    }

protected:
    // line 136
    basic_streambuf() : m_gbegin(nullptr), m_gnext(nullptr), m_gend(nullptr), m_pbegin(nullptr), m_pnext(nullptr), m_pend(nullptr) {}

    basic_streambuf(const basic_streambuf& x)
        : m_gbegin(x.m_gbegin), m_gnext(x.m_gnext), m_gend(x.m_gend), m_pbegin(x.m_pbegin), m_pnext(x.m_pnext), m_pend(x.m_pend) {}

    basic_streambuf& operator=(const basic_streambuf& x) {
        m_gbegin = x.m_gbegin;
        m_gnext = x.m_gnext;
        m_gend = x.m_gend;
        m_pbegin = x.m_pbegin;
        m_pnext = x.m_pnext;
        m_pend = x.m_pend;
        return *this;
    }

    void swap(basic_streambuf& x) {
        blz::swap(m_gbegin, x.m_gbegin);
        blz::swap(m_gnext, x.m_gnext);
        blz::swap(m_gend, x.m_gend);
        blz::swap(m_pbegin, x.m_pbegin);
        blz::swap(m_pnext, x.m_pnext);
        blz::swap(m_pend, x.m_pend);
    }

    char_type* eback() const { return m_gbegin; }
    char_type* gptr() const { return m_gnext; }
    char_type* egptr() const { return m_gend; }
    void gbump(int n) { m_gnext += n; }
    void setg(char_type* gbegin, char_type* gnext, char_type* gend) {
        m_gbegin = gbegin;
        m_gnext = gnext;
        m_gend = gend;
    }

    char_type* pbase() const { return m_pbegin; }
    char_type* pptr() const { return m_pnext; }
    char_type* epptr() const { return m_pend; }
    void pbump(int n) { m_pnext += n; }
    void setp(char_type* pbegin, char_type* pend) {
        m_pbegin = pbegin;
        m_pnext = pbegin;
        m_pend = pend;
    }

    virtual basic_streambuf* setbuf(char_type*, streamsize) {
        return this;
    }

    virtual pos_type seekoff(off_type, ios_base::seekdir, ios_base::openmode) {
        return pos_type(off_type(-1));
    }

    virtual pos_type seekpos(pos_type, ios_base::openmode) {
        return pos_type(off_type(-1));
    }

    virtual int sync() {
        return 0;
    }

    // line 275
    virtual streamsize showmanyc() {
        return 0;
    }

    // line 281
    virtual streamsize xsgetn(char_type* s, streamsize n) {
        char_type* begin = s;
        char_type* end = s + n;
        while (s < end) {
            int_type c = sbumpc();
            if (Traits::eq_int_type(c, Traits::eof()))
                break;
            *s++ = Traits::to_char_type(c);
        }
        return s - begin;
    }

    virtual int_type underflow() {
        return Traits::eof();
    }

    // line 305
    virtual int_type uflow() {
        int_type c = underflow();
        if (Traits::eq_int_type(c, Traits::eof()))
            return c;
        return Traits::to_int_type(*m_gnext++);
    }

    virtual int_type pbackfail(int_type) {
        return Traits::eof();
    }

    // line 322
    virtual streamsize xsputn(const char_type* s, streamsize n) {
        const char_type* begin = s;
        const char_type* end = s + n;
        while (n != 0) {
            size_t buf_size = m_pend - m_pnext;
            if (buf_size == 0) {
                int_type c = overflow(Traits::to_int_type(*s));
                if (Traits::eq_int_type(c, Traits::eof()))
                    break;
                ++s;
                buf_size = m_pend - m_pnext;
                --n;
            }
            size_t write_size = n;
            if (write_size > buf_size)
                write_size = buf_size;
            memcpy(m_pnext, s, write_size);
            m_pnext += write_size;
            s += write_size;
            n = end - s;
        }
        return s - begin;
    }

    virtual int_type overflow(int_type) {
        return Traits::eof();
    }

    char_type* m_gbegin;
    char_type* m_gnext;
    char_type* m_gend;
    char_type* m_pbegin;
    char_type* m_pnext;
    char_type* m_pend;
};

}  // namespace blz
