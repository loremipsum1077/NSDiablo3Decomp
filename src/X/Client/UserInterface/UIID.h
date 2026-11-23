// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/UserInterface/UIID.h

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Sigma/Unicode.h"

class CRefString;

struct UIID {  // size 0x8
    UIID() {}
    UIID(const UIID& other);
    UIID(const uint64 nIDHash, LPCSTR szID);
    UIID(const uint64 nIDHash);
    UIID& operator=(const UIID& arg);
    bool operator==(const UIID& arg) const;
    bool operator!=(const UIID& arg) const;
    operator unsigned long() const;
    CRefString GetIDAsString() const;
    void SetFromParent(const UIID& idParent, LPCSTR szName);
    void SetFromParentHash(uint64 dwHash, LPCSTR szName);
    void FromLabel(LPCSTR szLabel);
    uint64 nIDHash;  // 0x00

private:
    void SetInternal(const uint64 _nIDHash, LPCSTR _szID);
};
static_assert(sizeof(UIID) == 0x8, "UIID");

inline UIID::UIID(const UIID& other) {  // line 31
    SetInternal(other.nIDHash, NULL);
}

inline UIID::UIID(const uint64 nIDHash, LPCSTR szID) {
    SetInternal(nIDHash, szID);
}

inline UIID::UIID(const uint64 nIDHash) {  // line 47
    SetInternal(nIDHash, NULL);
}

inline UIID& UIID::operator=(const UIID& arg) {  // line 53
    if (this != &arg) {
        SetInternal(arg.nIDHash, NULL);
    }
    return *this;
}

inline bool UIID::operator==(const UIID& arg) const {
    return nIDHash == arg.nIDHash;
}

inline bool UIID::operator!=(const UIID& arg) const {
    return nIDHash != arg.nIDHash;
}

inline UIID::operator unsigned long() const {
    return nIDHash;
}

// The primary template lives in X/Sigma/DataStructures/XBaseMap.h.
template <typename ARG_KEY>
inline uint32 XHashKey(ARG_KEY key);

// The body is not in the DWARF; written from the copies inlined into
// X/Client/UserInterface/Console/UIConsoleEquipmentManagerDetails.cpp.
template <>
inline uint32 XHashKey<const UIID &>(const UIID & key) {  // line 124
    return (uint32)(key.nIDHash >> 32) ^ (uint32)key.nIDHash;
}

const UIID& GetUIIDNone();
uint64 UIHashIDName(LPCSTR pszName);
UIID UIIDCreate(LPCSTR szLabel);
UIID UIIDCreateFromParent(const UIID& idParent, LPCSTR szAppendedLabel);

// Defined in UIID.cpp (lines 25-28); UIInit.cpp refers to them by symbol.
extern const UIID cgUIIDRoot;
extern const UIID cgUIIDNormalLayer;
extern const UIID cgUIIDWindowLayer;
extern const UIID cgUIIDTopLayer;
extern const UIID cgUIIDMainScreen;  // data symbol 0x7100e436b8; UIConsoleDeathMenu.cpp compares against it through the GOT
