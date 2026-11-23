#include "UIID.h"

#include <string.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"

uint64 sUIAppendHashIDName(uint64 dwHash, LPCSTR pszName);

const UIID& GetUIIDNone() {
    static const UIID tReturnValue((uint64)-1, "None");
    return tReturnValue;
}

// The hash values are not in the DWARF; these are the values the switch in
// sUIAppendHashIDName tests for.
const UIID cgUIIDRoot((uint64)-2, "Root");
const UIID cgUIIDNormalLayer(1, "Root.NormalLayer");
const UIID cgUIIDWindowLayer(2, "Root.WindowLayer");
const UIID cgUIIDTopLayer(3, "Root.TopLayer");
const UIID cgUIIDMainScreen((uint64)-3, "Main-Screen");

static BOOL sStartWithValidUIIDHash(LPCSTR szLabel, uint64& dwHash) {
    BOOL bLegalValue = TRUE;
    uint64 dwLocalHash = 0;
    for (uint32 i = 0; i < 16; ++i) {
        CHAR cCharValue = szLabel[i];
        if (cCharValue >= 'a') {
            bLegalValue = cCharValue <= 'f';
            cCharValue = cCharValue - 'a' + 10;
        } else {
            bLegalValue = cCharValue >= '0' && cCharValue <= '9';
            cCharValue = cCharValue - '0';
        }
        if (!bLegalValue)
            return FALSE;
        dwLocalHash = (dwLocalHash << 4) + cCharValue;
    }
    dwHash = dwLocalHash;
    return TRUE;
}

uint64 sUIAppendHashIDName(uint64 dwHash, LPCSTR pszName) {
    CHAR szLoweredName[513];
    Blizzard::String::Copy(szLoweredName, pszName, sizeof(szLoweredName));
    Blizzard::String::ToLower(szLoweredName);

    LPCSTR pszParentName = NULL;
    if (dwHash == GetUIIDNone().nIDHash)
        pszParentName = "None";
    else if (dwHash == cgUIIDRoot.nIDHash)
        pszParentName = "Root";
    else if (dwHash == cgUIIDNormalLayer.nIDHash)
        pszParentName = "Root.NormalLayer";
    else if (dwHash == cgUIIDWindowLayer.nIDHash)
        pszParentName = "Root.WindowLayer";
    else if (dwHash == cgUIIDTopLayer.nIDHash)
        pszParentName = "Root.TopLayer";
    else if (dwHash == cgUIIDMainScreen.nIDHash)
        pszParentName = "Main-Screen";
    if (pszParentName)
        dwHash = UIHashIDName(pszParentName);

    return Blizzard::Util::Hash64More(szLoweredName, (uint)strlen(szLoweredName), dwHash);
}

uint64 UIHashIDName(LPCSTR pszName) {
    if (!*pszName)
        return 0;
    CHAR szLoweredName[513];
    Blizzard::String::Copy(szLoweredName, pszName, sizeof(szLoweredName));
    Blizzard::String::ToLower(szLoweredName);
    return Blizzard::Util::Hash64(szLoweredName);
}

void UIID::SetInternal(const uint64 _nIDHash, LPCSTR _szID) {
    nIDHash = _nIDHash;
}

void UIID::SetFromParent(const UIID& idParent, LPCSTR szName) {
    CRefString sAppendedLabel = CRefString().Format(".%s", szName);
    SetFromParentHash(idParent.nIDHash, sAppendedLabel.str());
}

inline void UIID::SetFromParentHash(uint64 dwHash, LPCSTR szName) {
    uint64 nIDHash = sUIAppendHashIDName(dwHash, szName);
    SetInternal(nIDHash, szName);
}

CRefString UIID::GetIDAsString() const {
    char szLocalID[17];
    uint64 nLocalIDHash = nIDHash;
    for (int32 i = 15; i >= 0; --i) {
        uint8 nLastValue = nLocalIDHash & 0xF;
        char cValue = '0' + nLastValue;
        if (nLastValue > 9)
            cValue += 'a' - '0' - 10;
        szLocalID[i] = cValue;
        nLocalIDHash >>= 4;
    }
    szLocalID[16] = 0;
    return CRefString(szLocalID);
}

void UIID::FromLabel(LPCSTR szLabel) {
    int32 nLabelLen = strlen(szLabel);
    uint64 dwHash;
    if (nLabelLen == 16) {
        if (sStartWithValidUIIDHash(szLabel, dwHash)) {
            SetInternal(dwHash, szLabel);
            return;
        }
    } else if (nLabelLen > 17 && szLabel[16] == '.' && sStartWithValidUIIDHash(szLabel, dwHash)) {
        LPCSTR pszID = szLabel + 16;
        SetFromParentHash(dwHash, pszID);
        return;
    }
    uint64 _nIDHash = UIHashIDName(szLabel);
    SetInternal(_nIDHash, szLabel);
}

UIID UIIDCreate(LPCSTR szLabel) {
    UIID idRetval;
    idRetval.FromLabel(szLabel);
    return idRetval;
}

UIID UIIDCreateFromParent(const UIID& idParent, LPCSTR szAppendedLabel) {
    UIID idRetval;
    idRetval.SetFromParentHash(idParent.nIDHash, szAppendedLabel);
    return idRetval;
}
