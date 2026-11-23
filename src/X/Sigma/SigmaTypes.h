#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"

typedef char CHAR;

typedef uint32 HINSTANCE;
typedef uint32 HWND;

typedef int32 BOOL;

#define FALSE 0
#define TRUE 1

typedef int64 __time64_t;

union _LARGE_INTEGER {
    struct {
        uint32 LowPart;
        int32 HighPart;
    };
    int64 QuadPart;
};
typedef _LARGE_INTEGER LARGE_INTEGER;

typedef CHAR TCHAR;
typedef const TCHAR* LPCTSTR;

// Layout checks generated from the DWARF by tools/layout_asserts.py.
static_assert(sizeof(_LARGE_INTEGER) == 0x8, "_LARGE_INTEGER");
