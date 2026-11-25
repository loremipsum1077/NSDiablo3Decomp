#include "UIToolTips.h"
#include "UITextControl.h"
#include "../BBE.h"
#include "../GlobalSNO.h"
#include "../Definitions/StringListDefinition.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Sigma/LanguageRules.h"
// DWARF GameBalanceDefinition.cpp:4105.
float PowerGetFormulaValueAtLevel(AttribGroupID idFastAttrib, const ActorCommonData * ptACD, SNO snoPower, uint8 * pbFormula, int32 nFormulaSize, int32 nLevel);













static BOOL sGetTagResult(
    const ActorCommonData * ptACDItem,
    AttribGroupID idFastAttribMin,
    AttribGroupID idFastAttribMax,
    SNO snoPower,
    int32 nLevel,
    LPCSTR szTagStart,
    float & flResultMin,
    float & flResultMax,
    CRefString & strResult,
    uint32 & uTagLength,
    TagResolutionCallback pTagCallback,
    void * pTagData0,
    void * pTagData1,
    FontSpecifier & eFontColor)
{
    // Line 41 column 15 (a postfix ++ on a 10-character name) and the callback argument szTagStart + 1 give this advance.
    szTagStart++;
    CHAR * pcCurrChar = (CHAR *)szTagStart;
    // Loop-back row at 44:5 and entry test at 47:13 give for (;;); counting uTagLength in the header (caller starts it at 0, line 311) is inferred from the original's index IV (strb [x21,x23]).
    for (;; ++pcCurrChar, ++uTagLength)
    {
        CHAR cChar = *pcCurrChar;
        if (cChar == 0 || cChar == '{')
        {
            return FALSE;
        }
        if (cChar == '}')
        {
            // Inferred default: absent callback restores the delimiter and reports no match.





            *pcCurrChar = 0;
            BOOL bMatch = FALSE;
            if (pTagCallback)
            {
                bMatch = pTagCallback(ptACDItem, idFastAttribMin, idFastAttribMax, szTagStart, flResultMin, &flResultMax, strResult, snoPower, nLevel, pTagData0, pTagData1, eFontColor);
            }
            *pcCurrChar = '}';

            if (bMatch)
            {
                // Inferred: count of characters before '}' plus the two braces; the original stores index + 1 from the unadvanced tag start.
                uTagLength += 2;
                return TRUE;
            }
            return FALSE;
        }
    }

}







static float sEvaluateFormula(CRefString & usFormula,
    const ActorCommonData * ptACDItem,
    const AttribGroupID idFastAttrib,
    const SNO snoPower,
    const int32 nLevel)
{
    // Original translation context stores two -1 values.
    BBETranslationData bbeTranslationData = { -1, GBID_NONE };
    uint8 arbFormula[1024];

    uint32 dwFormulaSize = BBEFromString(usFormula.str(), arbFormula, sizeof(arbFormula), bbeTranslationData);





    float flValue = PowerGetFormulaValueAtLevel(idFastAttrib, ptACDItem, snoPower, arbFormula, dwFormulaSize, nLevel);
    return flValue;
}



static CRefString sPrintValue(float flValue,
    BOOL bUseSign,
    uint32 uDecimalPlaces,
    BOOL bInFormula)
{





    XLocale eXLocale = bInFormula ? XLOCALE_ENGLISH_US : XLOCALE_INVALID;
    BOOL bUseGrouping = !bInFormula;
    CRefString usValue = UnicodeText::FormatNumber(flValue, bUseGrouping, FNS_NORMAL, uDecimalPlaces, uDecimalPlaces, eXLocale);
    if (bUseSign && flValue > 0.0f)
        usValue = CRefString("+") + usValue;
    return usValue;
}










static CRefString sPrintValueRange(float flValue1,
    float flValue2,
    BOOL bUseSign,
    uint32 uDecimalPlaces,
    BOOL bInFormula)

{



    XLocale eXLocale = bInFormula ? XLOCALE_ENGLISH_US : XLOCALE_INVALID;

    CRefString sValue1 = UnicodeText::FormatNumber(flValue1, TRUE, FNS_NORMAL, uDecimalPlaces, uDecimalPlaces, eXLocale);

    CRefString sValue;
    if (flValue1 != flValue2)
    {
        CRefString sValue2 = UnicodeText::FormatNumber(flValue2, TRUE, FNS_NORMAL, uDecimalPlaces, uDecimalPlaces, eXLocale);
        sValue = LocalizedSprintf(StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_GENERAL), "AffixRangeFormat"),
            sValue1.str(), sValue2.str(), NULL);
    }
    else
    {
        sValue = sValue1;
    }

    if (flValue1 > 0.0f && bUseSign)
    {



        sValue = CRefString("+") + sValue;
    }
    return sValue;
}






static void sOutputSingleFormulaTagValue(CRefString * psOutput,
    CRefString & strResult,
    const float flValue,
    LPCSTR szStringStart,
    CHAR * pcCurrChar)

{
    *pcCurrChar = 0;
    *psOutput += szStringStart;
    *pcCurrChar = '{';

    CRefString usValue;
    if (strResult.Length() == 0)
    {

        usValue = sPrintValue(flValue, FALSE, 5, TRUE);
    }
    else
    {
        usValue = strResult;
    }

    *psOutput += usValue;
}




CRefString UIToolTips_ProcessTip(
    const ActorCommonData * ptACDItem,
    const AttribGroupID idFastAttribMin,
    const AttribGroupID idFastAttribMax,
    const SNO snoPower,
    const int32 nLevel,
    LPCSTR szTip,
    const ToolTipValueColoring eColoringType,
    TagResolutionCallback pTagCallback,
    void * pTagData0,
    void * pTagData1)
{
    CRefString usResult;
    CRefString usFormulaResultMin;
    CRefString usFormulaResultMax;
    // DWARF CHAR* locals and original delimiter stores require a mutable view of szTip.
    CHAR * pcCurrChar = (CHAR *)szTip;
    CHAR * szStringStart = pcCurrChar;
    BOOL bInFormula = FALSE;
    BOOL bUseSign = FALSE;
    BOOL bInFormat = FALSE;
    FontSpecifier eFontColor = FONTCOLOR_WHITE;
    uint32 uDecimalPlaces = 0;
    for (;; ++pcCurrChar)
    {
        if (*pcCurrChar == 0)
        {
            usResult += szStringStart;
            break;
        }
        else if (bInFormat)
        {
            if (*pcCurrChar == '+')
                bUseSign = TRUE;
            else if (*pcCurrChar == '|') {
                szStringStart = pcCurrChar + 1;
                bInFormat = FALSE;
            }



            else if (*pcCurrChar >= '0' && *pcCurrChar <= '9')
            {
                uDecimalPlaces = *pcCurrChar - '0';
            }
        }
        else if (*pcCurrChar == '[')
        {


            *pcCurrChar = 0;
            usResult += szStringStart;
            *pcCurrChar = '[';
            szStringStart = pcCurrChar + 1;
            usFormulaResultMin = "";
            usFormulaResultMax = "";
            bInFormula = TRUE;
        }
        else if (bInFormula && *pcCurrChar == ']')
        {
            *pcCurrChar = 0;
            usFormulaResultMin += szStringStart;
            usFormulaResultMax += szStringStart;
            *pcCurrChar = ']';

            CRefString usFormulaValue;
            float flFormulaValueMin = sEvaluateFormula(usFormulaResultMin, ptACDItem, idFastAttribMin, snoPower, nLevel);
            if (idFastAttribMax == -1)
            {

                usFormulaValue = sPrintValue(flFormulaValueMin, bUseSign, uDecimalPlaces, FALSE);
            }
            else
            {
                float flFormulaValueMax = sEvaluateFormula(usFormulaResultMax, ptACDItem, idFastAttribMax, snoPower, nLevel);
                usFormulaValue = sPrintValueRange(flFormulaValueMin, flFormulaValueMax, bUseSign, uDecimalPlaces, FALSE);
            }

            switch (eColoringType)
            {
            case TTVC_NORMAL_COLORING:
                if (eFontColor != FONTCOLOR_WHITE)
                {
                    Set_Text_Color(usFormulaValue, eFontColor);
                    eFontColor = FONTCOLOR_WHITE;
                }
                break;
            case TTVC_ENCHANT_RANGE_COLORING:

                Set_Text_Color(usFormulaValue, FONTCOLOR_LIGHTGREY);
                eFontColor = FONTCOLOR_WHITE;
                break;
            default: break;
            }



            usResult += usFormulaValue;
            bInFormula = FALSE;
            bUseSign = FALSE;
            bInFormat = FALSE;
            uDecimalPlaces = 0;
            szStringStart = pcCurrChar + 1;
        }
        else if (*pcCurrChar == '{')
        {
            float flValueMin = 0.0f;
            float flValueMax = 0.0f;
            uint32 uTagLength = 0;

            CRefString strResult;
            if (sGetTagResult(ptACDItem, idFastAttribMin, idFastAttribMax, snoPower, nLevel, pcCurrChar, flValueMin, flValueMax, strResult, uTagLength, pTagCallback, pTagData0, pTagData1, eFontColor))
            {
                if (bInFormula)
                {

                    sOutputSingleFormulaTagValue(&usFormulaResultMin, strResult, flValueMin, szStringStart, pcCurrChar);
                    if (idFastAttribMax != -1)
                    {
                        sOutputSingleFormulaTagValue(&usFormulaResultMax, strResult, flValueMax, szStringStart, pcCurrChar);
                    }
                }
                else
                {

                    *pcCurrChar = 0;
                    usResult += szStringStart;
                    *pcCurrChar = '{';

                    CRefString usValue;
                    if (strResult.Length() == 0)
                    {
                        if (idFastAttribMax == -1)
                        {

                            usValue = sPrintValue(flValueMin, FALSE, uDecimalPlaces, FALSE);
                        }
                        else
                        {
                            usValue = sPrintValueRange(flValueMin, flValueMax, FALSE, uDecimalPlaces, FALSE);
                        }
                    }
                    else
                    {
                        usValue = strResult;
                    }

                    switch (eColoringType)
                    {
                    case TTVC_NORMAL_COLORING:
                        if (eFontColor != FONTCOLOR_WHITE)
                        {
                            Set_Text_Color(usValue, eFontColor);
                            eFontColor = FONTCOLOR_WHITE;
                        }
                        break;
                    case TTVC_ENCHANT_RANGE_COLORING:

                        Set_Text_Color(usValue, FONTCOLOR_LIGHTGREY);
                        eFontColor = FONTCOLOR_WHITE;
                        break;
                    default: break;
                    }



                    usResult += usValue;
                }

                szStringStart = pcCurrChar + uTagLength;
            }
        }
        else if (bInFormula && *pcCurrChar == '|')
        {


            *pcCurrChar = 0;
            usFormulaResultMin += szStringStart;
            usFormulaResultMax += szStringStart;
            *pcCurrChar = '|';
            bInFormat = TRUE;
        }
    }


    CRefString usAfterLanguageRules = LanguageRules::ApplyLanguageRules(usResult.str());
    return usAfterLanguageRules;
}
