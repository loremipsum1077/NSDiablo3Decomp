#include "OnlineServiceLogStream.h"
#include "../../../BlizzardCore/Include/Blizzard/String.h"

namespace OnlineService {











// OUTPUTSTREAM_DEFAULT and format are evidenced by the original call operands.
void LogStream::WriteLog() const
{





    TraceInternal_Log(m_eSeverityLevel, m_uFlags, OUTPUTSTREAM_DEFAULT, "%s\n",








                      str().c_str());
}







LogStream & LogStream::operator<<(int val) { blz::ostringstream::operator<<(val); return *this;
}


LogStream & LogStream::operator<<(uint64 val) { blz::ostringstream::operator<<(val); return *this;
}










// The original ostream helper handles null; the shared helper currently lacks that branch.
LogStream & LogStream::operator<<(const char * val) { blz::ostringstream & s = *this; if (val) s << val; else s << "(null)"; return *this;
}












LogStream & operator<<(LogStream & s, const ostringstream_stringtype & c) { blz::ostringstream & ss = s; ss << c; return s;
}



























blz::ostream & operator<<(blz::ostream & s, const GameAccountHandle & a)
{


    char szProgram[5];
    a.m_program.ToString(szProgram, sizeof(szProgram));
    s << "{ " << a.m_id << "," << szProgram << "," << a.m_region << " }";
    return s;
}




















































LogStream & operator<<(LogStream & s, const GameIDLabel & label)
{


    char szBuffer[32];
    Blizzard::String::Format(szBuffer, sizeof(szBuffer), "[GameID: 0x%x]", label.m_id);
    return s << szBuffer;
}

}
