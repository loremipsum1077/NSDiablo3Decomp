#pragma once

#include "SigmaTypes.h"
#include "Unicode.h"

class BreakIteratorData;

enum {
    BREAKSTYLE_CHAR = 0,
    BREAKSTYLE_WORD = 1,
    BREAKSTYLE_SENTENCE = 2,
    BREAKSTYLE_LINE = 3,
};

typedef int32 BreakStyle;

class UTBreakIterator {
public:
    UTBreakIterator(BreakStyle eBreakStyle);
    ~UTBreakIterator();
    void SetText(LPCSTR szText);
    int32 Next();
    int32 Previous();
    int32 Following(int32 nOffset);
    int32 Preceding(int32 nOffset);
    BOOL IsBoundary(int32 nOffset);
    BreakStyle GetBreakStyle() const;

protected:
    BreakStyle m_eBreakStyle;
    BreakIteratorData* m_data;

private:
    UTBreakIterator(const UTBreakIterator&);
    UTBreakIterator& operator=(const UTBreakIterator&);
};

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(UTBreakIterator) == 0x10, "UTBreakIterator");
