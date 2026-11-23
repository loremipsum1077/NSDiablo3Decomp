#include "GameBalance.h"

#include <alloca.h>

#include "Protobuf/GameBalance.pb.h"
#include "../../BlizzardCore/Include/Blizzard/String.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Sigma/RefString.h"

// Defined in X/Common/Definitions/GlobalDefinition.cpp; that header is not in src/ yet.
typedef int32 AssetListType;
int32 AssetListIndexGet(AssetListType eAssetList, int32 nAssetID);
int32 AssetListArrayToBitfield(AssetListType eAssetList, const int32* ptAssetIDArray, int32 nAssets, uint8* ptByteArray, int32 nByteArrayLength, int32* pnLeadingNullBytes);
int32 AssetListBitfieldToArray(AssetListType eAssetList, const uint8* ptByteArray, int32 nByteArrayLength, int32* ptAssetIDArray, int32 nMaxAssets, int32 nLeadingNullBytes);

// line 18. The DWARF has no initializer; the bytes in .rodata are { -1, -1 }.
const GBHandle cghgbNone(-1, GBID_NONE);

// line 24
GBID StringToGBID(LPCSTR szGBID) {
    if (szGBID[0] == '\0')
        return GBID_NONE;

    CHAR szLoweredName[256];
    Blizzard::String::Copy(szLoweredName, szGBID, sizeof(szLoweredName));
    Blizzard::String::ToLower(szLoweredName);
    return XHashKey<LPCSTR>(szLoweredName);
}

// line 42
void PackAssetsIntoBitPackedGbidArray(D3::GameBalance::BitPackedGbidArray& tPackedArray, AssetListType eAssetList, int32* ptAssetIdArray, int32 nNumAssets, int32 nMaxAssets) {
    tPackedArray.Clear();

    size_t uBufferSize = (nMaxAssets + 7) / 8;
    uint8* arBitfieldBuffer = (uint8*)alloca(uBufferSize);
    int32 nLeadingNullBytes = 0;
    int32 nNumBytes = AssetListArrayToBitfield(eAssetList, ptAssetIdArray, nNumAssets, arBitfieldBuffer, uBufferSize, &nLeadingNullBytes);

    BOOL bUseBitfield = FALSE;
    if (nNumBytes > 0 && nNumBytes < nNumAssets * sizeof(int32)) {
        bUseBitfield = TRUE;
        tPackedArray.set_bitfield(arBitfieldBuffer, nNumBytes);
        if (nLeadingNullBytes > 0)
            tPackedArray.set_bitfield_leading_null_bytes(nLeadingNullBytes);
    }

    for (int32 i = 0; i < nNumAssets; ++i) {
        int32 nListIndex = AssetListIndexGet(eAssetList, ptAssetIdArray[i]);
        if (bUseBitfield && nListIndex != -1)
            continue;
        tPackedArray.add_elements(ptAssetIdArray[i]);
    }
}

// line 91
int32 UnpackAssetsFromBitPackedGbidArray(const D3::GameBalance::BitPackedGbidArray& tPackedArray, AssetListType eAssetList, int32* ptAssetIdArray, int32 nMaxAssets) {
    int32 nUnpackedAssets = 0;

    const blz::string& strData = tPackedArray.bitfield();
    if (strData.length() > 0) {
        int32 uBufferSize = (nMaxAssets + 7) / 8;
        int32 nBytes = Math::Min<int>(strData.length(), uBufferSize);
        if (nBytes != 0) {
            int32 nLeadingNullBytes = tPackedArray.has_bitfield_leading_null_bytes() ? tPackedArray.bitfield_leading_null_bytes() : 0;
            nUnpackedAssets = AssetListBitfieldToArray(eAssetList, (const uint8*)strData.data(), nBytes, ptAssetIdArray, nMaxAssets, nLeadingNullBytes);
        }
    }

    for (int32 i = 0; i < tPackedArray.elements_size() && nUnpackedAssets < nMaxAssets; ++i) {
        ptAssetIdArray[nUnpackedAssets] = tPackedArray.elements(i);
        ++nUnpackedAssets;
    }

    return nUnpackedAssets;
}
