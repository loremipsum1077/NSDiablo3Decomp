// OnlineService::operator<<(ostream&, const GameAccountHandle&)
// (OnlineServiceLogStream.cpp:102) formats a handle with three numeric and
// three string writes, which exercises basic_ostream::write / sentry /
// setstate, operator<<(unsigned) and operator<<(const char*) inline.
#include "Contrib/Contrib/blz/include/blz/ostream.h"

class XFourCC {
public:
    void ToString(char* pBuf, unsigned long nBufSize) const;
    unsigned int m_value;
};

namespace blz {
typedef basic_ostream<char, char_traits<char> > ostream;
}

namespace OnlineService {
struct GameAccountHandle {
    unsigned int m_id;
    XFourCC m_program;
    unsigned int m_region;
};

blz::ostream& operator<<(blz::ostream& s, const GameAccountHandle& a) {
    char szProgram[5];
    a.m_program.ToString(szProgram, 5);
    s << "{ " << a.m_id << "," << szProgram << "," << a.m_region << " }";
    return s;
}
}  // namespace OnlineService
