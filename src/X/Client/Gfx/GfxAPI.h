// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Gfx/GfxAPI.h
//
// Trimmed to the back-end pointer. On NX it is a GFXNX64NVN, so every call through it is a
// direct call; the abstract GFX interface the reference header declares is left out.

#pragma once

#include "NX64/NX64NVN.h"

// X/Client/Gfx/GfxAPI.h:28 (DWARF reference) -- the back-end selector.
enum {
    GFX_NONE = 0,
    GFX_NULL = 1,
    GFX_D3D = 2,
    GFX_D3D11 = 3,
    GFX_OPENGL = 4,
    GFX_PS4GNM = 5,
    GFX_XBOXONED3D = 6,
    GFX_NX64NVN = 7,
};

typedef int32 GfxAPI;

// X/Client/Gfx/GfxAPI.cpp:59 / :117
// The two entry points. GFXAPIInitialize calls GFXAPIDispose (DWARF: inlined at
// GfxAPI.cpp:62), so it must be declared before its use; both bodies are in GfxAPI.cpp.
BOOL GFXAPIInitialize(GfxAPI eGraphicsSystem);
void GFXAPIDispose();

// X/Client/Gfx/GfxAPI.cpp:41
extern GFXNX64NVN * gptGFXAPI;
