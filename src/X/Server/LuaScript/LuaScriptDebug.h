// Adopted from the DWARF skeleton (build/dwarf/headers/X/Server/LuaScript/LuaScriptDebug.h);
// hand-maintained from here on. Original file: X/Server/LuaScript/LuaScriptDebug.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
// The DWARF records only Blizzard/Types.h, but the adopted copy does not define LPCSTR, so pull
// it (and int32) from the headers that own it: Sigma/Unicode.h owns LPCSTR.
#include "../../Sigma/Unicode.h"

typedef void (*LuaScriptStackCallback)(LPCSTR, LPCSTR, LPCSTR, int32, void *);
