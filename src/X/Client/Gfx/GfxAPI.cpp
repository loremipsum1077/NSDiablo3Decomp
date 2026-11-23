// X/Client/Gfx/GfxAPI.cpp
// Byte-matching decompilation of Diablo III 2.6.2 (Switch, clang 5.0.1).
//
// Evidence:
//   dwarf   _Z16GFXAPIInitializei  X/Client/Gfx/GfxAPI.cpp:59
//   dwarf   _Z13GFXAPIDisposev     X/Client/Gfx/GfxAPI.cpp:117 (inlined into 1 fn)
//   dwarf   variable gptGFXAPI     X/Client/Gfx/GfxAPI.cpp:41
//   lines   _Z16GFXAPIInitializei  0x71001d23e0 size 124
//   lines   _Z13GFXAPIDisposev     0x71001d2460 size 60

#include "X/Client/Gfx/GfxAPI.h"

#include "X/Client/Gfx/NX64/NX64NVN.h"
#include "X/Sigma/ErrorManager/ErrorManager.h"

// X/Client/Gfx/GfxAPI.cpp:41
// The single back-end object.  Every reader reaches it through the GOT
// (`ldr` the slot at _GLOBAL_OFFSET_TABLE_+0x2a0, then `ldr` the pointer out
// of it), so it is a pointer variable at file scope, not the object.
// .bss: 0x71017c37c0 gptGFXAPI -- no file contents, no value to cite.
GFXNX64NVN *gptGFXAPI;

// X/Client/Gfx/GfxAPI.cpp:59
// 0x71001d23e0  size 124  BOOL GFXAPIInitialize(GfxAPI eGraphicsSystem)
BOOL GFXAPIInitialize(GfxAPI eGraphicsSystem)
{
    // X/Client/Gfx/GfxAPI.cpp:62 -- GFXAPIDispose is inlined here
    // (dwarf: "_Z13GFXAPIDisposev ... inlined at line 62").
    GFXAPIDispose();

    switch (eGraphicsSystem)
    {
    case GFX_NONE:
        // X/Client/Gfx/GfxAPI.cpp:67 -- the id the shipped build never sets.
        // `mov w2, #0x43` is the source line number 67; the string argument is
        // an empty string (adrp/add to 0x7100d6a4a3, whose first byte is 0x00),
        // and the error code immediate is 1 (ERROR_INTERNAL_FATAL).
        DisplayInternalError(nullptr, "", 67, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
        break;

    case GFX_NX64NVN:
        // X/Client/Gfx/GfxAPI.cpp:107 -- the only back-end this platform
        // builds.  `mov w0, #0x1048` is sizeof(GFXNX64NVN); no constructor
        // call follows _Znwm, i.e. the type is trivially default constructible.
        gptGFXAPI = new GFXNX64NVN;
        break;
    }

    // X/Client/Gfx/GfxAPI.cpp:112
    return TRUE;
}

// X/Client/Gfx/GfxAPI.cpp:117
// 0x71001d2460  size 60  void GFXAPIDispose()
void GFXAPIDispose()
{
    // X/Client/Gfx/GfxAPI.cpp:120
    if (gptGFXAPI != nullptr)
    {
        // X/Client/Gfx/GfxAPI.cpp:122
        gptGFXAPI->Dispose();

        // X/Client/Gfx/GfxAPI.cpp:123 -- the inner `cbz` before _ZdlPv is the
        // delete-expression's own null guard; `str xzr, [x19]` is the
        // assignment right after it.
        delete gptGFXAPI;
        gptGFXAPI = nullptr;
    }

    // X/Client/Gfx/GfxAPI.cpp:125
}
