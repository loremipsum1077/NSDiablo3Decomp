// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/Interface/UIFunctions.h

#pragma once

#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Sigma/Unicode.h"

struct UIID;

typedef int32 UIResult;

typedef int32 PlayerClass;

typedef UIResult (*UIMsgFunctionPtr)(const UIID &);

typedef void (*UIFunctionPtr)(const UIID &);

typedef void (*UIScrollbarChangeFunctionPtr)(const UIID &, const int32, const int32);

typedef void (*UIConsoleFunctionPtr)(const UIID &, const int32);

typedef void (*UIListBoxChangeFunctionPtr)(const UIID &, const int32, const int32);

// Declarations of the functions UIFunctions.cpp defines; the DWARF gives their
// signatures (owners: X/Client/UserInterface/Interface/UIFunctions.cpp) but not
// where they were declared.
UIFunctionPtr UIFunction_Lookup(LPCSTR szFunction);  // UIFunctions.cpp
UIConsoleFunctionPtr UIConsoleFunction_Lookup(LPCSTR szFunction);  // UIFunctions.cpp
