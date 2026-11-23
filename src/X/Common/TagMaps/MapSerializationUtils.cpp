#include "X/Common/TagMaps/MapSerializationUtils.h"
#include "X/Common/TagMaps/Tags.h"
#include "X/Common/BBE.h"
#include "X/Sigma/Memory/SigmaMemory.h"













void MapSerializationUtils::SerializeInt(
    int32 nValue,
    void *& pBlockPtr,
    int32 & nBlockSize)
{
    if (pBlockPtr)
    {
        *(int32 *)pBlockPtr = nValue;
        pBlockPtr = (char *)pBlockPtr + sizeof(int32);
    }

    nBlockSize += sizeof(int32);
}




void MapSerializationUtils::UnserializeInt(
    void *& pBlockPtr,
    int32 & nValue)
{

    nValue = *(int32 *)pBlockPtr;
    pBlockPtr = (char *)pBlockPtr + sizeof(int32);
}




void MapSerializationUtils::SerializeInt(
    uint32 dwValue,
    void *& pBlockPtr,
    int32 & nBlockSize)
{
    if (pBlockPtr)
    {
        *(uint32 *)pBlockPtr = dwValue;
        pBlockPtr = (char *)pBlockPtr + sizeof(uint32);
    }

    nBlockSize += sizeof(uint32);
}




void MapSerializationUtils::UnserializeInt(
    void *& pBlockPtr,
    uint32 & dwValue)
{

    dwValue = *(uint32 *)pBlockPtr;
    pBlockPtr = (char *)pBlockPtr + sizeof(uint32);
}




void MapSerializationUtils::SerializeFormula(
    XPtr64<unsigned char> & pData,
    void *& pBlockPtr,
    int32 & nBlockSize)
{
    StringFormula * pStringFormula = (StringFormula *)(unsigned char *)pData;

    pStringFormula->Compile();


    SerializeInt(0, pBlockPtr, nBlockSize);
    SerializeInt(0, pBlockPtr, nBlockSize);
    SerializeInt(0, pBlockPtr, nBlockSize);
    SerializeInt(0, pBlockPtr, nBlockSize);
    SerializeInt(pStringFormula->serFormulaString.dwOffset, pBlockPtr, nBlockSize);
    SerializeInt(pStringFormula->serFormulaString.dwSize, pBlockPtr, nBlockSize);
    SerializeInt(pStringFormula->serFormula.dwOffset, pBlockPtr, nBlockSize);
    SerializeInt(pStringFormula->serFormula.dwSize, pBlockPtr, nBlockSize);

    const uint32 dwAlignBytes = 4;
    uint32 dwPadString = pStringFormula->serFormulaString.dwSize % dwAlignBytes;
    uint32 dwPadFormula = pStringFormula->serFormula.dwSize % dwAlignBytes;

    if (dwPadString)
    {
        dwPadString = dwAlignBytes - dwPadString;
    }

    if (dwPadFormula)
    {
        dwPadFormula = dwAlignBytes - dwPadFormula;
    }

    if (pBlockPtr)
    {
        LPCSTR szString = pStringFormula->szFormulaString;

        if (pStringFormula->serFormulaString.dwSize)
        {

            SigmaMemoryClear(pBlockPtr, pStringFormula->serFormulaString.dwSize + dwPadString);
            if (szString)
            {
                LPSTR szDest = (LPSTR)pBlockPtr;
                strcpy(szDest, szString);
            }
            pBlockPtr = (uint8 *)pBlockPtr + pStringFormula->serFormulaString.dwSize + dwPadString;
        }

        if (pStringFormula->serFormula.dwSize)
        {

            SigmaMemoryClear(pBlockPtr, pStringFormula->serFormula.dwSize + dwPadFormula);
            uint8 * pbFormula = pStringFormula->pbFormula;
            if (pbFormula)
            {
                SigmaMemoryCopy(pBlockPtr, pbFormula, pStringFormula->serFormula.dwSize);
                BBEByteswap((uint8 *)pBlockPtr, pStringFormula->serFormula.dwSize);
            }
            pBlockPtr = (uint8 *)pBlockPtr + pStringFormula->serFormula.dwSize + dwPadFormula;
        }
    }

    nBlockSize += pStringFormula->serFormula.dwSize + dwPadFormula + pStringFormula->serFormulaString.dwSize + dwPadString;
}





























































































void MapSerializationUtils::UnserializeFormula(
    void *& pBlockPtr,
    XPtr64<unsigned char> & pData,
    BOOL bReadOnly)
{

    StringFormula * pStringFormula = (StringFormula *)pBlockPtr;
    int32 nPad;


    pStringFormula->szFormulaString = NULL;
    UnserializeInt(pBlockPtr, nPad);
    UnserializeInt(pBlockPtr, nPad);
    pStringFormula->pbFormula = NULL;
    UnserializeInt(pBlockPtr, nPad);
    UnserializeInt(pBlockPtr, nPad);
    UnserializeInt(pBlockPtr, pStringFormula->serFormulaString.dwOffset);
    UnserializeInt(pBlockPtr, pStringFormula->serFormulaString.dwSize);
    UnserializeInt(pBlockPtr, pStringFormula->serFormula.dwOffset);
    UnserializeInt(pBlockPtr, pStringFormula->serFormula.dwSize);

    const uint32 dwAlignBytes = 4;
    uint32 dwPadString = pStringFormula->serFormulaString.dwSize % dwAlignBytes;
    uint32 dwPadFormula = pStringFormula->serFormula.dwSize % dwAlignBytes;

    if (dwPadString)
    {
        dwPadString = dwAlignBytes - dwPadString;
    }

    if (dwPadFormula)
    {
        dwPadFormula = dwAlignBytes - dwPadFormula;
    }

    if (pStringFormula->serFormulaString.dwSize)
    {
        pStringFormula->szFormulaString = (char *)pBlockPtr;
    }

    pBlockPtr = (uint8 *)pBlockPtr + pStringFormula->serFormulaString.dwSize + dwPadString;

    if (pStringFormula->serFormula.dwSize)
    {
        pStringFormula->pbFormula = (uint8 *)pBlockPtr;
        BBEByteswap(pStringFormula->pbFormula, pStringFormula->serFormula.dwSize);
    }

    pBlockPtr = (uint8 *)pBlockPtr + pStringFormula->serFormula.dwSize + dwPadFormula;

    if (bReadOnly)
    {
        pData = (uint8 *)pStringFormula;
    }
    else
    {
        StringFormula * pNewStringFormula = (StringFormula *)SigmaMemoryNew(sizeof(StringFormula), 4, NULL, TRUE);

        pNewStringFormula->Copy(*pStringFormula);

        pData = (uint8 *)pNewStringFormula;
    }
}




void MapSerializationUtils::SerializeVariant(
    const TagVariantType eTagVariantType,
    TagVariant & variantTag,
    void *& pBlockPtr,
    int32 & nBlockSize)
{

    if (eTagVariantType == TAGVARIANTTYPE_FORMULA)
    {


        XPtr64<unsigned char> pTemp((unsigned char *)variantTag.m_pData);
        SerializeFormula(pTemp, pBlockPtr, nBlockSize);

    }
    else
    {

        SerializeInt(variantTag.m_TagData, pBlockPtr, nBlockSize);
    }


}




void MapSerializationUtils::UnserializeVariant(
    const TagVariantType eTagVariantType,
    void *& pBlockPtr,
    TagVariant & variantTag,
    BOOL bReadOnly)
{
    if (eTagVariantType == TAGVARIANTTYPE_FORMULA)
    {


        XPtr64<unsigned char> pTemp;
        UnserializeFormula(pBlockPtr, pTemp, bReadOnly);
        variantTag.m_pData = (uintptr)pTemp.Get();
    }
    else
    {



        UnserializeInt(pBlockPtr, variantTag.m_TagData);
    }


}




float MapSerializationUtils::VariantToFloat(
    const TagVariant & variantTag)
{
    return *(float *)&variantTag.m_TagData;
}




StringFormula * MapSerializationUtils::VariantToStringFormula(
    const TagVariant & variantTag)
{
    return (StringFormula *)variantTag.m_pData;
}




void MapSerializationUtils::PutIntInVariant(
    TagVariant & variantTag,
    int32 & nValue)
{
    variantTag.m_TagData = nValue;
}






















































void MapSerializationUtils::FreeVariant(
    TagVariantType eVariantType,
    TagVariant & variantTag)
{
    if (eVariantType == TAGVARIANTTYPE_FORMULA)
    {


        StringFormula * pStringFormula = (StringFormula *)variantTag.m_pData;

        pStringFormula->Dispose();

        SigmaMemoryFree(pStringFormula, NULL);
    }


}
