// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/SNO/SNOName.h

// SNOFiles.h and this header need each other: this one uses SNO, SNOGroup and the SNO_NONE /
// SNOGROUP_NONE enumerators, and SNOFiles.h has SNOName members. SNOFiles.h therefore includes
// this header after it has declared those, and the include below is deliberately outside the
// guard so that including SNOName.h first also works (SNOFiles.h re-enters this file and the
// outer copy is then skipped).
#include "SNOFiles.h"

#ifndef X_COMMON_SNO_SNONAME_H
#define X_COMMON_SNO_SNONAME_H

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/SigmaTypes.h"
#include "../../Sigma/Unicode.h"
// blz::ignore (header-level static in blz/tuple.h): every original compile unit that has
// SNONAME_NONE also has it, at the front of its merged .bss block.
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"

class CRefString;
class XTypeDescriptor;

struct SNOName {  // size 0x8
    SNOGroup snoGroup;  // 0x00
    SNO snoHandle;  // 0x04
    SNOName() {}  // empty: no inlined copy in the DWARF, no out-of-line copy in the binary
    SNOName(const SNOGroup snoGroup, const SNO snoHandle) {  // line 26
        Set(snoGroup, snoHandle);
    }
    SNOName(const SNOName & snoname) {  // line 33; calls Set at line 36
        Set(snoname.snoGroup, snoname.snoHandle);
    }
    SNOName(const SNOGroup snoGroup, LPCSTR sSNOName) {  // line 40
        Set(snoGroup, sSNOName);
    }
    void Set(const SNOGroup _snoGroup, const SNO _snoHandle) {  // line 47
        snoGroup = _snoGroup;
        snoHandle = _snoHandle;
    }
    void Set(const SNOGroup _snoGroup, LPCSTR sSNOName);
    bool operator==(const SNOName & snoname) const;
    BOOL operator!=(const SNOName & snoname) const;
    bool operator<(const SNOName &) const;
    CRefString GetDebugString() const;
};
static_assert(sizeof(SNOName) == 0x8, "SNOName");

// line 59; always inlined, no out-of-line copy in the binary.
inline bool SNOName::operator==(const SNOName & snoname) const {
    return ((snoGroup - snoname.snoGroup) | (snoHandle - snoname.snoHandle)) == 0;
}

// line 70; always inlined, no out-of-line copy in the binary (the DWARF attributes the copy
// inlined into LuaScriptDebug.cpp's caller to this line). Written with a bitwise & of the two
// member tests so that a comparison against an all-ones name folds into the single `and`/`cmn #1`
// test the original binary shows (SNOName.h:70 inlined at the caller).
inline BOOL SNOName::operator!=(const SNOName & snoname) const {
    return !((snoGroup == snoname.snoGroup) & (snoHandle == snoname.snoHandle));
}

struct CompressedSNOName {  // size 0x4
    SNOGroup snoGroup : 8;  // 0x00
    SNO snoHandle : 24;  // 0x00
    CompressedSNOName(const SNOName &);
    CompressedSNOName(uint32);
    CompressedSNOName();
    operator SNOName();
    operator unsigned int();
    BOOL operator==(const CompressedSNOName & snoname) const;
};
static_assert(sizeof(CompressedSNOName) == 0x4, "CompressedSNOName");

// SNOName.h:97-136. The bodies are not in the DWARF; they are written from the inlined copies in
// X/Client/SNOPreload.cpp (bfi of the handle over the group byte, a whole-word compare, and a
// hash that is the sign-extended handle).
inline CompressedSNOName::CompressedSNOName(const SNOName & snoname) {
    snoGroup = snoname.snoGroup;
    snoHandle = snoname.snoHandle;
}

// SNOName.h:112: inlined into 14 functions and leaves no code in them (the key local of
// XBaseMap::RemoveAll in ~XGrowableMap<CompressedSNOName,...>), so the body is empty.
// The body sets the invalid name: the allocated XMapAssoc's key store survives in
// X/Client/SNOManager.cpp's AddSNOTree (SNOName.h:114).
inline CompressedSNOName::CompressedSNOName() {
    snoGroup = SNOGROUP_NONE; snoHandle = SNO_NONE;
}

inline BOOL CompressedSNOName::operator==(const CompressedSNOName & snoname) const {
    return *(const uint32 *)&snoname == *(const uint32 *)this;
}

template <typename ARG_KEY>
inline uint32 XHashKey(ARG_KEY key);  // X/Sigma/DataStructures/XBaseMap.h

template <>
inline uint32 XHashKey<const CompressedSNOName &>(const CompressedSNOName & snoname) {
    return snoname.snoHandle;
}

typedef XListMemoryPool<SNOName> TSnoNameListPool;

// Defined in SNOName.cpp:23 and SNOName.cpp:63.
extern const XTypeDescriptor * DT_SNO_NAME;
TSnoNameListPool * GetSnoNameListPool();
void SNONameDisposeThreadGlobals();

static const SNOName SNONAME_NONE(SNOGROUP_NONE, SNO_NONE);  // line 142

#endif  // X_COMMON_SNO_SNONAME_H


// Functions defined in this file (bodies are not in the DWARF):
//   line    26  SNOName::SNOName(const SNOGroup snoGroup, const SNO snoHandle)  [inlined into 164 functions]
//   line    33  SNOName::SNOName(const SNOName & snoname)  [inlined into 51 functions]
//   line    40  SNOName::SNOName(const SNOGroup snoGroup, LPCSTR sSNOName)  [inlined into 18 functions]
//   line    47  void SNOName::Set(const SNOGroup _snoGroup, const SNO _snoHandle)  [inlined into 215 functions]
//   line    59  bool SNOName::operator==(const SNOName & snoname) const  [inlined into 19 functions]
//   line    70  BOOL SNOName::operator!=(const SNOName & snoname) const  [inlined into 4 functions]
//   line    97  CompressedSNOName::CompressedSNOName(const SNOName & snoname)  [inlined into 29 functions]
//   line   112  CompressedSNOName::CompressedSNOName()  [inlined into 14 functions]
//   line   118  CompressedSNOName::operator SNOName()  [inlined into 16 functions]
//   line   128  BOOL CompressedSNOName::operator==(const CompressedSNOName & snoname) const  [inlined into 24 functions]
//   line   136  uint32 XHashKey<const CompressedSNOName &>(const CompressedSNOName & snoname)  [inlined into 15 functions]
//   line   142  static void __cxx_global_var_init()  [inline]
//   line   142  static void __cxx_global_var_init.1()  [inline]
