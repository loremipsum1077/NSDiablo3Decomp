#include "FormattedText.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../BlizzardCore/Include/Blizzard/Util/Hash.h"
#include "../../../Contrib/Contrib/blz/include/blz/tuple.h"
#include "../../Common/Hyperlink.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/UserInterface/FontDefinition.h"
#include "../../Common/WorldPlace.h"
#include "../Battlenet/Hub.h"
#include "../Gfx/GfxVB.h"
#include "../../Sigma/BlzExt/BlzVectorHelper.h"
#include "../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../Sigma/ErrorManager/ErrorManager.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Memory/SigmaMemory.h"
#include "../../Sigma/UnicodeText.h"
#include "../../Sigma/UnicodeTextBreakIterator.h"

XMemoryAllocatorInterface * AppGetCurrentMemoryInterface();

// Declarations of the functions this file calls whose headers are not adopted yet; the DWARF gives
// their signatures (file noted on each) but not where they were declared.
uint32 atoh(LPCSTR sz);  // X/Sigma/Hex.cpp
ImageHandle StringToImageHandle(LPCSTR pszName);  // X/Client/ImageCollection.cpp
LPCSTR AdjustUIButtonIconForSwitch(LPCSTR szDefaultIconName);  // X/Client/Input/InputNX64.cpp
BOOL FontSpecifierIsColor(FontSpecifier eFS);  // X/Common/UserInterface/Font.cpp
RGBAColor FontGetColorForFontSpecifier(FontSpecifier eFS, BOOL fIgnorePlayerOverrides);  // X/Common/UserInterface/Font.cpp
BOOL FontSpecifierIsFont(FontSpecifier eFS);  // X/Client/Font/Font.cpp
SNO FontGetSnoForFontSpecifier(FontSpecifier eFS);  // X/Client/Font/Font.cpp
CRefString GetUnknownHyperlinkText();  // X/Client/Battlenet/UI/Util/UIUtil.cpp
void Set_Text_Color(CRefString & usString, FontSpecifier efcDisplayColor);  // X/Common/UserInterface/UITextControl.cpp
CRefString StringListFetch(LPCSTR szSnoDecoratedLabel, BOOL & bSuccess);  // X/Common/Definitions/StringListDefinition.cpp
Hub * HubGet();  // X/Client/Battlenet/Hub.cpp

// X/Client/ImageCollection.h:20
enum {
    IMAGE_COLLECTION_INVALID = -1,
    IMAGE_COLLECTION_ITEM = 0,
    IMAGE_COLLECTION_MAP = 1,
    IMAGE_COLLECTION_UI = 2,
    IMAGE_COLLECTION_NUM_TYPES = 3,
};

// X/Common/OnlineService/Achievements.h and X/Client/OnlineService/ClientService.h. Only the
#include "../../Common/OnlineService/Achievements.h"
namespace OnlineService {









class ClientService {
public:
    // line 232. The member's name is not in the DWARF; invented.
    OnlineService::Achievements & Achievements() { return *m_pAchievements; }
private:
    uint8 _unadopted0[0x38];
    OnlineService::Achievements * m_pAchievements;  // 0x38
};
}  // namespace OnlineService

struct LabelTableEntry {  // size 0x10
    LPCSTR uszLabel;  // 0x00
    FontSpecifier eLabelType;  // 0x08
};
struct FormattedTextGlobals {  // size 0x150
    XChainedFixedBlockAllocator<FormattedHyperlinkDelta> poolHyperlinkDeltas;  // 0x00
    XChainedFixedBlockAllocator<FormattedTextDelta> poolBaseDeltas;  // 0x30
    XChainedFixedBlockAllocator<FormattedColorPushDelta> poolColorPushDeltas;  // 0x60
    XChainedFixedBlockAllocator<FormattedIconDelta> poolIconDeltas;  // 0x90
    XChainedFixedBlockAllocator<FormattedFontFacePushDelta> poolFontPushDeltas;  // 0xc0
    XChainedFixedBlockAllocator<FormattedCharacterDelta> poolCharacterDeltas;  // 0xf0
    XChainedFixedBlockAllocator<FormattedBulletIndentDelta> poolIndentDeltas;  // 0x120
};

enum {
    INITIAL_DELTA_POOL_COUNT = 32,
};

#include "../UserInterface/UI.h"




#include "../../Common/Game.h"




extern AppGlobals sgtAppGlobals;
const uint32 cguPasswordCharacter = 0x2022;
const uint32 cguInvalidGlyphCharacter = 0xfffd;
const uint32 cNBSP = 0xa0;
const uint32 cEllipsisChar = 0x2026;
LabelTableEntry tLabelTable[17] = {
    { "c_bonus", FONTCOLOR_BONUS },
    { "c_whisper", FONTCOLOR_WHISPER },
    { "c_questlog", FONTCOLOR_QUEST_LOG },
    { "c_white", FONTCOLOR_WHITE },
    { "c_red", FONTCOLOR_RED },
    { "c_darkred", FONTCOLOR_DARKRED },
    { "c_green", FONTCOLOR_GREEN },
    { "c_blue", FONTCOLOR_BLUE },
    { "c_gold", FONTCOLOR_GOLD },
    { "c_yellow", FONTCOLOR_YELLOW },
    { "c_legendary", FONTCOLOR_LEGENDARY },
    { "c_collectible", FONTCOLOR_COLLECTIBLE },
    { "c_epic", FONTCOLOR_EPIC },
    { "c_rare", FONTCOLOR_RARE },
    { "c_inferior", FONTCOLOR_GREY },
    { "c_magic", FONTCOLOR_BONUS },
    { "f_alpha_tiny", FONTTYPEFACE_ALPHA_TINY },
};
class SortLabelTable {  // size 0x1
public:
    SortLabelTable()
    {
        qsort(tLabelTable, sizeof(tLabelTable) / sizeof(tLabelTable[0]), sizeof(tLabelTable[0]), LabelComparator);
    }
    static int32 LabelComparator(const void * elem1, const void * elem2)
    {
        return strcmp(((const LabelTableEntry *)elem1)->uszLabel, ((const LabelTableEntry *)elem2)->uszLabel);
    }
};
static SortLabelTable s_labelTableSortAtStartup;

// The byte-wise (uint8) compares and case folding are what the binary does (ldrb, uxtb).
static int32 LabelComparatorForTagParsing(const void * elem1, const void * elem2)
{
    const CHAR * label1 = (const CHAR *)elem1;
    const CHAR * label2 = ((const LabelTableEntry *)elem2)->uszLabel;
    for (int32 index = 0;; index++) {
        CHAR c1 = ((uint8)label1[index] >= 'A' && (uint8)label1[index] <= 'Z') ? (uint8)label1[index] + ('a' - 'A') : (uint8)label1[index];
        CHAR c2 = ((uint8)label2[index] >= 'A' && (uint8)label2[index] <= 'Z') ? (uint8)label2[index] + ('a' - 'A') : (uint8)label2[index];


        if ((uint8)c1 == '}' || (uint8)c1 == ',') {
            c1 = '\0';
        }


        if ((uint8)c1 != (uint8)c2) {
            return c1 - c2;
        }


        if (c2 == '\0') {
            return 0;
        }
    }
}



BOOL sByteIsCodepointStart(const uint8 ch)
{







    return (ch & 0x80) == 0 ||
           (ch >= 0xc0 && ch <= 0xf7);
}




int32 GetPreviousCharacterUTF8(LPCSTR pszText, const int32 nCursorPos)
{





    if (nCursorPos <= 0) {
        return nCursorPos;
    }

    int32 nPos = nCursorPos - 1;

    while (nPos > 0 && !sByteIsCodepointStart(pszText[nPos])) {
        nPos--;
    }


    return nPos;
}



static void sCommonTextDeltaSetup(FormattedTextDelta * pDelta, LPCSTR szOriginalTextSaved, LPCSTR szText, int32 nFormattedIndex, int32 nNumCodepoints, int32 nFormattedTextLength)
{
    pDelta->nOriginalTextIndex = szOriginalTextSaved - szText;
    pDelta->nFormattedTextIndex = nFormattedIndex;
    pDelta->nCodepointIndex = nNumCodepoints;
    pDelta->nFormattedTextLength = nFormattedTextLength;
}

static FormattedTextDelta * sAllocFormattedTextDelta(FormattedTextDeltaType eType);
static void sDeleteFormattedTextDelta(FormattedTextDelta *& pDelta);
static FormattedTextDelta * sCopyFormattedTextDelta(FormattedTextDelta * pSrc);

// NON_MATCHING: 5312 bytes vs 5260. Statements are on the DWARF lines; the '{' tag test is an if/else-if
// chain (line table: 262:17/262:42 is `if (... == 'b' || ... == 'B')`), which SimplifyCFG turns into the
// original's ldrb jump table. Remaining: block layout (original puts the loop header at the bottom) and the
// hyperlink failure paths' constant cleanup flag (`mov w8,#0/1; cbnz w8`) after ~FormattedText, not reproduced.
// Inferred from the line table: the 312 condition, `pDelta = pX;` right after each alloc, the 636 brace.
static void sParseText(
    LPCSTR szText,
    uint32 nLength,
    const uint32 dwParseFlags,
    FormattedText::FormattedTextDeltaArray & arReturnArray)
{
    if (szText == NULL || dwParseFlags == PARSE_NONE) {
        return;
    }



    const uint32 PRESIZE_VALUE = 8;
    BOOL fParseSpecialTags = dwParseFlags & PARSE_SPECIAL_TAGS;
    BOOL fParseHyperlinks = dwParseFlags & PARSE_HYPERLINKS;
    BOOL fValidateHyperlinks = dwParseFlags & PARSE_DO_VALIDATION;
    LPCSTR szOriginalText = szText;
    uint32 nFormattedIndex = 0;
    uint32 nNumCodepoints = 0;

    while (szOriginalText < szText + nLength) {

        LPCSTR szOriginalTextSaved = szOriginalText;
        uint32 c = Blizzard::Unicode::GetCodepointFromUTF8(&szOriginalText);
        nNumCodepoints++;


        if (c == 0) {
            break;
        }

        if (c != '{' && c != '\\' && c != '|') {
            nFormattedIndex += szOriginalText - szOriginalTextSaved;
            continue;
        }

        if (fParseSpecialTags && c == '{') {

            FormattedTextDelta * pDelta = NULL;


            if (szOriginalText[0] == 'b' || szOriginalText[0] == 'B') {


                pDelta = sAllocFormattedTextDelta(TEXTDELTA_BREAK_HINT);
                sCommonTextDeltaSetup(pDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);


            } else if (szOriginalText[0] == 'd' || szOriginalText[0] == 'D') {


                pDelta = sAllocFormattedTextDelta(TEXTDELTA_HYPHEN_HINT);
                sCommonTextDeltaSetup(pDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);


            } else if (szOriginalText[0] == '/') {

                switch (szOriginalText[1]) {
                case 'c':
                case 'C':



                    pDelta = sAllocFormattedTextDelta(TEXTDELTA_FONTCOLOR_POP);
                    sCommonTextDeltaSetup(pDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);
                    break;
                case 'f':
                case 'F':



                    pDelta = sAllocFormattedTextDelta(TEXTDELTA_FONTFACE_POP);
                    sCommonTextDeltaSetup(pDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);
                    break;
                default:
                    nFormattedIndex += szOriginalText - szOriginalTextSaved;
                    continue;
                }




            } else {

                LPCSTR cszIconTag = "icon:";
                const int32 cnIconTagLength = 5;

                LPCSTR cszItemIconTag = "item:";
                const int32 cnItemIconTagLength = 5;


                if ((szOriginalText[0] == 'c' || szOriginalText[0] == 'C') && szOriginalText[1] == ':') {


                    if (strchr(szOriginalText + 2, '}') != szOriginalText + 2 + 8) {
                        nFormattedIndex += szOriginalText - szOriginalTextSaved;
                        continue;
                    }


                    FormattedColorPushDelta * pColorPushDelta = (FormattedColorPushDelta *)sAllocFormattedTextDelta(TEXTDELTA_FONTCOLOR_PUSH);
                    pDelta = pColorPushDelta;
                    sCommonTextDeltaSetup(pColorPushDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);


                    CHAR szColor[9];
                    Blizzard::String::Copy(szColor, szOriginalText + 2, sizeof(szColor));
                    UIColor color = atoh(szColor);
                    pColorPushDelta->rgbaColor.Set(color);

                } else if (strncasecmp(szOriginalText, cszIconTag, cnIconTagLength) == 0) {


                    LPCSTR pszFrameStart = szOriginalText + cnIconTagLength;
                    LPCSTR pszCloseBrace = strchr(pszFrameStart, '}');
                    if (pszCloseBrace != NULL) {

                        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)sAllocFormattedTextDelta(TEXTDELTA_FONTICON);
                        pDelta = pIconDelta;
                        sCommonTextDeltaSetup(pIconDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 1);


                        CRefString sFrame;
                        LPCSTR pszModifiers = strchr(pszFrameStart, ',');
                        if (pszModifiers != NULL && pszModifiers < pszCloseBrace) {


                            sscanf(pszModifiers + 1, "%f", &pIconDelta->flScale);
                            sFrame = CRefString(pszFrameStart, pszModifiers - pszFrameStart);
                        } else {


                            sFrame = CRefString(pszFrameStart, pszCloseBrace - pszFrameStart);
                        }



                        sFrame = AdjustUIButtonIconForSwitch(sFrame);

























                        pIconDelta->hImage = StringToImageHandle(sFrame);
                        pIconDelta->eCollection = IMAGE_COLLECTION_UI;
                    }

                } else if (strncasecmp(szOriginalText, cszItemIconTag, cnItemIconTagLength) == 0) {


                    LPCSTR pszFrameStart = szOriginalText + cnItemIconTagLength;
                    LPCSTR pszCloseBrace = strchr(pszFrameStart, '}');
                    if (pszCloseBrace != NULL) {

                        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)sAllocFormattedTextDelta(TEXTDELTA_FONTICON);
                        pDelta = pIconDelta;
                        sCommonTextDeltaSetup(pIconDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 1);


                        CRefString sFrame;
                        LPCSTR pszModifiers = strchr(pszFrameStart, ',');
                        if (pszModifiers != NULL && pszModifiers < pszCloseBrace) {


                            sscanf(pszModifiers + 1, "%f", &pIconDelta->flScale);
                            sFrame = CRefString(pszFrameStart, pszModifiers - pszFrameStart);
                        } else {


                            sFrame = CRefString(pszFrameStart, pszCloseBrace - pszFrameStart);
                        }

                        pIconDelta->hImage = atoh(sFrame.str());
                        pIconDelta->eCollection = IMAGE_COLLECTION_ITEM;
                    }
                } else {



                    LabelTableEntry * entry = (LabelTableEntry *)bsearch(szOriginalText, tLabelTable, sizeof(tLabelTable) / sizeof(tLabelTable[0]), sizeof(tLabelTable[0]), LabelComparatorForTagParsing);
                    if (entry == NULL) {
                        nFormattedIndex += szOriginalText - szOriginalTextSaved;
                        continue;
                    }


                    if (FontSpecifierIsColor(entry->eLabelType)) {

                        FormattedColorPushDelta * pColorPushDelta = (FormattedColorPushDelta *)sAllocFormattedTextDelta(TEXTDELTA_FONTCOLOR_PUSH);
                        pDelta = pColorPushDelta;
                        sCommonTextDeltaSetup(pColorPushDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);
                        pColorPushDelta->rgbaColor = FontGetColorForFontSpecifier(entry->eLabelType, FALSE);

                    } else if (FontSpecifierIsFont(entry->eLabelType)) {

                        FormattedFontFacePushDelta * pFontDelta = (FormattedFontFacePushDelta *)sAllocFormattedTextDelta(TEXTDELTA_FONTFACE_PUSH);
                        pDelta = pFontDelta;
                        sCommonTextDeltaSetup(pFontDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, 0);
                        pFontDelta->snoFont = FontGetSnoForFontSpecifier(entry->eLabelType);
                    } else {
                        nFormattedIndex += szOriginalText - szOriginalTextSaved;
                        continue;
                    }
                }
            }




            LPCSTR pCloseBrace = strchr(szOriginalText + 1, '}');
            if (pCloseBrace == NULL) {

                pCloseBrace = szOriginalText + strlen(szOriginalText) - 1;
            }
            uint32 nBytesSpanned = pCloseBrace + 1 - szOriginalTextSaved;
            size_t nCodepointsSpanned = 0;
            Blizzard::Unicode::CountUTF8Chars(szOriginalTextSaved, nBytesSpanned, nCodepointsSpanned);
            nNumCodepoints += nCodepointsSpanned - 1;

            if (pDelta != NULL) {
                pDelta->nOriginalBytesReplaced = nBytesSpanned;
                pDelta->nCodepointsReplaced = nCodepointsSpanned;

                if (arReturnArray.size() == 0) {
                    arReturnArray.reserve(PRESIZE_VALUE);
                }
                arReturnArray.push_back(pDelta);
            }




            szOriginalText = pCloseBrace + 1;
            if (pDelta != NULL) {

                nFormattedIndex += pDelta->nFormattedTextLength;
            }





        } else if (c == '\\') {

            if (*szOriginalText == '|') {

                FormattedHyperlinkDelta * pDelta = (FormattedHyperlinkDelta *)sAllocFormattedTextDelta(TEXTDELTA_HYPERLINK);
                sCommonTextDeltaSetup(pDelta, szOriginalTextSaved, szText, nFormattedIndex, nNumCodepoints - 1, szOriginalText - szOriginalTextSaved);

                uint32 nBytesSpanned = szOriginalText + 1 - szOriginalTextSaved;
                size_t nCodepointsSpanned = 0;
                Blizzard::Unicode::CountUTF8Chars(szOriginalTextSaved, nBytesSpanned, nCodepointsSpanned);
                nNumCodepoints += nCodepointsSpanned - 1;

                FormattedText tFormattedText;
                tFormattedText = FormattedText(CRefString().Format("%s", "|"), PARSE_ALL, TRUE);
                pDelta->tFormattedText = tFormattedText;

                pDelta->nOriginalBytesReplaced = nBytesSpanned;
                pDelta->nCodepointsReplaced = nCodepointsSpanned;
                if (arReturnArray.size() == 0) {



                    arReturnArray.reserve(PRESIZE_VALUE);
                }
                arReturnArray.push_back(pDelta);

                szOriginalText++;
                nFormattedIndex += pDelta->nFormattedTextLength;

            } else {


                nFormattedIndex += szOriginalText - szOriginalTextSaved;
            }



        } else if (fParseHyperlinks && c == '|' && (*szOriginalText == 'h' || *szOriginalText == 'H')) {


            CHAR szTemp[1024];
            LPCSTR pszHyperlinkStart = szOriginalTextSaved;
            LPCSTR pszHyperlinkValidate = szOriginalTextSaved;
            BOOL fValidated = !fValidateHyperlinks || HyperlinkValidate(pszHyperlinkValidate) != HLT_NONE;
            if (fValidated &&
                HyperlinkGetPrettyName(szTemp, pszHyperlinkStart, sizeof(szTemp), FALSE)) {

                LPCSTR pszHyperlinkEnd = pszHyperlinkStart;
                pszHyperlinkStart = szOriginalTextSaved;
                uint32 nCurrentOriginalTextIndex = szOriginalTextSaved - szText;
                HyperlinkType eType = HyperlinkParseType(pszHyperlinkStart, FALSE);

                FormattedText tHyperlinkDeltaText;
                if (eType == HLT_ACHIEVEMENT) {

                    HyperlinkAchievementData tAchievementData;
                    if (CHyperlinkGetAchievementData(pszHyperlinkStart, tAchievementData)) {


                        const OnlineService::AchievementDefinition * ptDef =
                            HubGet()->ClientServiceGet().Achievements().GetAchievementDefinition(tAchievementData.tAchievementId);
                        BOOL bSuccess = FALSE;
                        CRefString sAchievementName = ptDef ? CRefString("[") + StringListFetch(ptDef->m_pszNameKey, bSuccess) + CRefString("]") : GetUnknownHyperlinkText();


                        if (!bSuccess) {


                            sAchievementName = GetUnknownHyperlinkText();
                        }

                        Set_Text_Color(sAchievementName, FONTCOLOR_LINKS);
                        tHyperlinkDeltaText = FormattedText(sAchievementName.str(), PARSE_ALL, TRUE);
                    }


                } else if (eType == HLT_ITEM) {
                    CRefString sItemName;
                    if (CHyperlinkGetItemName(pszHyperlinkStart, sItemName, TRUE)) {

                        tHyperlinkDeltaText = FormattedText(sItemName.str(), PARSE_ALL, TRUE);
                    }

                } else {



                    tHyperlinkDeltaText = FormattedText(szTemp, strlen(szTemp), PARSE_ALL, TRUE);
                }
                int32 nPrettyNameLength = tHyperlinkDeltaText.ToFormattedIndex(tHyperlinkDeltaText.Length());

                FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)sAllocFormattedTextDelta(TEXTDELTA_HYPERLINK);
                pHyperlinkDelta->nFormattedTextLength = nPrettyNameLength;
                pHyperlinkDelta->nOriginalTextIndex = nCurrentOriginalTextIndex;
                pHyperlinkDelta->nCodepointIndex = nNumCodepoints - 1;
                pHyperlinkDelta->nOriginalBytesReplaced = pszHyperlinkEnd - pszHyperlinkStart;
                size_t nNumUTF8Chars = 0;
                Blizzard::Unicode::CountUTF8Chars(pszHyperlinkStart, pHyperlinkDelta->nOriginalBytesReplaced, nNumUTF8Chars);
                pHyperlinkDelta->nCodepointsReplaced = nNumUTF8Chars;
                pHyperlinkDelta->nFormattedTextIndex = nFormattedIndex;
                pHyperlinkDelta->tFormattedText = tHyperlinkDeltaText;
                if (arReturnArray.size() == 0) {



                    arReturnArray.reserve(PRESIZE_VALUE);
                }
                arReturnArray.push_back(pHyperlinkDelta);

                szOriginalText = pszHyperlinkEnd;
                nNumCodepoints += nNumUTF8Chars - 1;
                nFormattedIndex += nPrettyNameLength;
                continue;
            } else if (!fValidated) {


                HyperlinkType eType = HyperlinkParseType(szOriginalTextSaved, FALSE);
                LPCSTR pszHyperlinkEnd = szOriginalTextSaved;
                if (eType != HLT_NONE &&
                    HyperlinkAdvancePastHyperlink(pszHyperlinkEnd)) {



                FormattedText tHyperlinkDeltaText(GetUnknownHyperlinkText(), PARSE_DEFAULT, TRUE);
                int32 nPrettyNameLength = tHyperlinkDeltaText.ToFormattedIndex(tHyperlinkDeltaText.Length());

                FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)sAllocFormattedTextDelta(TEXTDELTA_HYPERLINK);
                pHyperlinkDelta->nFormattedTextLength = nPrettyNameLength;
                pHyperlinkDelta->nOriginalTextIndex = szOriginalTextSaved - szText;
                pHyperlinkDelta->nCodepointIndex = nNumCodepoints - 1;
                pHyperlinkDelta->nOriginalBytesReplaced = pszHyperlinkEnd - szOriginalTextSaved;
                size_t nNumUTF8Chars = 0;
                Blizzard::Unicode::CountUTF8Chars(szOriginalTextSaved, pHyperlinkDelta->nOriginalBytesReplaced, nNumUTF8Chars);
                pHyperlinkDelta->nCodepointsReplaced = nNumUTF8Chars;
                pHyperlinkDelta->nFormattedTextIndex = nFormattedIndex;
                pHyperlinkDelta->tFormattedText = tHyperlinkDeltaText;
                if (arReturnArray.size() == 0) {



                    arReturnArray.reserve(PRESIZE_VALUE);
                }
                arReturnArray.push_back(pHyperlinkDelta);

                szOriginalText = pszHyperlinkEnd;
                nNumCodepoints += nNumUTF8Chars - 1;
                nFormattedIndex += nPrettyNameLength;
                continue;
                }
            }
            nFormattedIndex += szOriginalText - szOriginalTextSaved;
        } else
            nFormattedIndex += szOriginalText - szOriginalTextSaved;
    }
}




void FormattedTextInitializeForApp()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    FormattedTextGlobals *& ptFormattedTextGlobals = sgtAppGlobals.ptUIAppGlobals->ptFormattedTextGlobals;

    ptFormattedTextGlobals = new (SigmaMemoryNew(sizeof(FormattedTextGlobals), 4, ptMemoryInterface, FALSE)) FormattedTextGlobals;

    ptFormattedTextGlobals->poolBaseDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolBaseDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolBaseDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolColorPushDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolColorPushDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolColorPushDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolHyperlinkDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolHyperlinkDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolHyperlinkDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolIconDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolIconDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolIconDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolFontPushDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolFontPushDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolFontPushDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolCharacterDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolCharacterDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolCharacterDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);

    ptFormattedTextGlobals->poolIndentDeltas.Initialize(INITIAL_DELTA_POOL_COUNT, ptMemoryInterface);
    ptFormattedTextGlobals->poolIndentDeltas.SetBlockCount_Tracked(INITIAL_DELTA_POOL_COUNT);
    ptFormattedTextGlobals->poolIndentDeltas.SetGrow(XALLOC_GROW_LINEAR, INITIAL_DELTA_POOL_COUNT);
}




void FormattedTextDisposeForApp()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    FormattedTextGlobals *& ptFormattedTextGlobals = sgtAppGlobals.ptUIAppGlobals->ptFormattedTextGlobals;

    if (ptFormattedTextGlobals != NULL) {

        ptFormattedTextGlobals->~FormattedTextGlobals();
        SigmaMemoryFree(ptFormattedTextGlobals, ptMemoryInterface);
        ptFormattedTextGlobals = NULL;
    }
}

FormattedText::FormattedText()

    : m_sOriginalText(""),
      m_nOriginalLength(0),
      m_dwHash(0),
      m_fPassword(FALSE),
      m_fContainsBreakHints(FALSE),
      m_fContainsIcons(FALSE)
{
    m_szOriginalText = m_sOriginalText;
    ComputeHash();
}





FormattedText::FormattedText(CRefString sText, const uint32 dwParseFlags)
    : m_szOriginalText(NULL),

      m_sOriginalText(sText),
      m_nOriginalLength(sText.Length()),
      m_dwHash(0),
      m_fPassword(FALSE),
      m_fContainsBreakHints(FALSE),
      m_fContainsIcons(FALSE)
{


    m_szOriginalText = m_sOriginalText;
    sParseText(m_szOriginalText, m_nOriginalLength, dwParseFlags, m_arTextDeltas);
    m_fContainsBreakHints = ContainsBreakHints();
    m_fContainsIcons = ContainsIcons();
    ComputeHash();
}





FormattedText::FormattedText(LPCSTR szText, const uint32 dwParseFlags, BOOL fDeepCopy)
    : m_szOriginalText(NULL),
      m_nOriginalLength(0),
      m_dwHash(0),
      m_fPassword(FALSE),
      m_fContainsBreakHints(FALSE),
      m_fContainsIcons(FALSE)
{



    if (szText == NULL) {
        m_szOriginalText = m_sOriginalText;
    } else {






        m_nOriginalLength = strlen(szText);
        if (fDeepCopy) {
            m_sOriginalText = szText;
            szText = m_sOriginalText;
        }
        m_szOriginalText = szText;





        sParseText(szText, m_nOriginalLength, dwParseFlags, m_arTextDeltas);
        m_fContainsBreakHints = ContainsBreakHints();
        m_fContainsIcons = ContainsIcons();
    }
    ComputeHash();
}




FormattedText::FormattedText(LPCSTR szText, uint32 nLength, const uint32 dwParseFlags, BOOL fDeepCopy)
    : m_szOriginalText(NULL),
      m_nOriginalLength(0),
      m_dwHash(0),

      m_fPassword(FALSE),
      m_fContainsBreakHints(FALSE),
      m_fContainsIcons(FALSE)
{



    if (szText == NULL) {
        m_szOriginalText = m_sOriginalText;

        ComputeHash();
        return;
    }


    m_nOriginalLength = Math::Min((uint32)strlen(szText), nLength);
    if (fDeepCopy) {

        m_sOriginalText = CRefString(szText, m_nOriginalLength);
        szText = m_sOriginalText;
    }




    m_szOriginalText = szText;
    sParseText(szText, m_nOriginalLength, dwParseFlags, m_arTextDeltas);
    m_fContainsBreakHints = ContainsBreakHints();
    m_fContainsIcons = ContainsIcons();

    ComputeHash();
}




FormattedText::FormattedText(const FormattedText & tFormattedText)
    : m_szOriginalText(NULL),
      m_nOriginalLength(tFormattedText.m_nOriginalLength),
      m_dwHash(0),
      m_fPassword(tFormattedText.m_fPassword),
      m_fContainsBreakHints(FALSE),
      m_fContainsIcons(FALSE)
{

    if (tFormattedText.m_sOriginalText.Length()) {
        m_sOriginalText = tFormattedText.m_sOriginalText;
        m_szOriginalText = m_sOriginalText;
    } else {
        m_szOriginalText = tFormattedText.m_szOriginalText;
    }


    m_arTextDeltas.resize(tFormattedText.m_arTextDeltas.size());
    for (uint32 i = 0; i < tFormattedText.m_arTextDeltas.size(); i++) {
        m_arTextDeltas[i] = sCopyFormattedTextDelta(tFormattedText.m_arTextDeltas[i]);
    }




    m_fContainsBreakHints = tFormattedText.m_fContainsBreakHints;
    m_fContainsIcons = tFormattedText.m_fContainsIcons;
    ComputeHash();
}





FormattedText::FormattedText(const FormattedText & tFormattedText, uint32 nLength)
    : m_dwHash(0)
{


    *this = tFormattedText;
    m_nOriginalLength = Math::Min(m_nOriginalLength, nLength);
    ComputeHash();
}





FormattedText & FormattedText::operator=(const FormattedText & tFormattedText)
{
    if (&tFormattedText == this) {
        return *this;
    }


    Cleanup();

    m_nOriginalLength = tFormattedText.m_nOriginalLength;
    m_fPassword = tFormattedText.m_fPassword;

    if (tFormattedText.m_sOriginalText.Length()) {
        m_sOriginalText = tFormattedText.m_sOriginalText;
        m_szOriginalText = m_sOriginalText;
    } else {
        m_szOriginalText = tFormattedText.m_szOriginalText;
    }






    m_arTextDeltas.resize(tFormattedText.m_arTextDeltas.size());
    for (uint32 i = 0; i < tFormattedText.m_arTextDeltas.size(); i++) {
        m_arTextDeltas[i] = sCopyFormattedTextDelta(tFormattedText.m_arTextDeltas[i]);
    }

    m_fContainsBreakHints = tFormattedText.m_fContainsBreakHints;
    m_fContainsIcons = tFormattedText.m_fContainsIcons;
    ComputeHash();



    return *this;
}
FormattedText::FormattedText(FormattedText && tFormattedText)
{
    m_nOriginalLength = tFormattedText.m_nOriginalLength;
    m_fPassword = tFormattedText.m_fPassword;

    if (tFormattedText.m_sOriginalText.Length()) {
        m_sOriginalText = tFormattedText.m_sOriginalText;
        m_szOriginalText = m_sOriginalText.str();
    } else {
        m_szOriginalText = tFormattedText.m_szOriginalText;
    }




    m_arTextDeltas = blz::move(tFormattedText.m_arTextDeltas);
    m_fContainsBreakHints = tFormattedText.m_fContainsBreakHints;
    m_fContainsIcons = tFormattedText.m_fContainsIcons;
    ComputeHash();
}


FormattedText & FormattedText::operator=(FormattedText && tFormattedText)
{
    if (&tFormattedText != this) {




        Cleanup();

        m_nOriginalLength = tFormattedText.m_nOriginalLength;
        m_fPassword = tFormattedText.m_fPassword;

        if (tFormattedText.m_sOriginalText.Length()) {
            m_sOriginalText = tFormattedText.m_sOriginalText;
            m_szOriginalText = m_sOriginalText.str();
        } else {
            m_szOriginalText = tFormattedText.m_szOriginalText;
        }





        m_arTextDeltas = blz::move(tFormattedText.m_arTextDeltas);
        m_fContainsBreakHints = tFormattedText.m_fContainsBreakHints;
        m_fContainsIcons = tFormattedText.m_fContainsIcons;
        ComputeHash();
    }


    return *this;
}



FormattedText::~FormattedText()
{

    Cleanup();
}




void FormattedText::ComputeHash()
{
    const FormattedText::FormattedTextDeltaArray & tDeltas = m_arTextDeltas;
    uint64 v = Blizzard::Util::Hash64(GetText(), Length());


    for (uint32 i = 0; i < tDeltas.size(); i++) {

        v = Blizzard::Util::Hash64More(tDeltas[i], sizeof(FormattedTextDelta), v);
        if (tDeltas[i]->eType == TEXTDELTA_HYPERLINK) {
            FormattedHyperlinkDelta * ptHyperlinkDelta = (FormattedHyperlinkDelta *)tDeltas[i];

            if (ptHyperlinkDelta->tFormattedText.GetHash() == 0) {

                ptHyperlinkDelta->tFormattedText.ComputeHash();
            }
            v = Blizzard::Util::Hash64More(ptHyperlinkDelta->tFormattedText.GetHash(), v);
        }
    }

    m_dwHash = v;
}

// Not called anywhere in the binary; the linker dropped it (the DWARF keeps its local, sDeepCopy).

void FormattedText::ConvertToDeepCopy(
    FormattedText & tText)
{
    CRefString sDeepCopy(tText.m_szOriginalText, tText.m_nOriginalLength);
    tText.m_sOriginalText = sDeepCopy;
    tText.m_szOriginalText = tText.m_sOriginalText;
}




void FormattedText::Cleanup()
{

    m_sOriginalText = "";
    m_szOriginalText = NULL;
    m_nOriginalLength = 0;

    ClearDeltas();
}




LPCSTR FormattedText::GetText() const
{

    return m_szOriginalText;
}




uint32 FormattedText::Length() const
{

    return m_nOriginalLength;
}




uint32 FormattedText::ToFormattedIndex(uint32 nOriginalIndex) const
{

    int32 nPosDelta = 0;

    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        const FormattedTextDelta * pDelta = m_arTextDeltas[i];
        if (pDelta->nOriginalTextIndex + pDelta->nOriginalBytesReplaced <= nOriginalIndex) {

            nPosDelta += pDelta->nOriginalBytesReplaced - pDelta->nFormattedTextLength;
        }
    }


    return nOriginalIndex - nPosDelta;
}




uint32 FormattedText::ToOriginalIndex(uint32 nFormattedIndex) const
{

    uint32 nPosDelta = 0;

    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        const FormattedTextDelta * pDelta = m_arTextDeltas[i];
        if (pDelta->nFormattedTextIndex + pDelta->nFormattedTextLength <= nFormattedIndex) {

            nPosDelta += pDelta->nOriginalBytesReplaced - pDelta->nFormattedTextLength;
        }
    }

    return nFormattedIndex + nPosDelta;
}




static int32 sUTF8BytesToCodepoints(LPCSTR szUTF8, const int32 nBytes)
{



    size_t nLineCodepoints = 0;
    Blizzard::Unicode::CountUTF8Chars(szUTF8, nBytes, nLineCodepoints);
    return nLineCodepoints;
}




static int32 sCodepointsToUTF8Bytes(LPCSTR szUTF8, const int32 nCodepoints)
{

    int32 nCodepointIndex = 0;

    LPCSTR uszChar = szUTF8;
    while (*uszChar != '\0' && nCodepointIndex != nCodepoints) {

        Blizzard::Unicode::GetCodepointFromUTF8(&uszChar);
        nCodepointIndex++;
    }

    int32 nBytes = uszChar - szUTF8;
    return nBytes;
}




uint32 FormattedText::GetPrevBreak(const uint32 nFormattedIndex, const BreakStyle eBreakStyle) const
{

    UTBreakIterator tIterator(eBreakStyle);
    tIterator.SetText(m_szOriginalText);



    int32 nOriginalIndex = ToOriginalIndex(nFormattedIndex);
    int32 nCodepointIndex = sUTF8BytesToCodepoints(m_szOriginalText, nOriginalIndex);

    int32 nNewCodepointIndex = nCodepointIndex;
    do {

        nNewCodepointIndex = tIterator.Preceding(nNewCodepointIndex);

    } while (IndexIsInsideTextDelta(nNewCodepointIndex, 0xff));

    int32 nNewIndexInBytes;
    if (nNewCodepointIndex == -1) {
        nNewIndexInBytes = 0;
    } else {




        nNewIndexInBytes = sCodepointsToUTF8Bytes(m_szOriginalText, nNewCodepointIndex);
    }

    return ToFormattedIndex(nNewIndexInBytes);
}




uint32 FormattedText::GetNextBreak(const uint32 nFormattedIndex, const BreakStyle eBreakStyle) const
{

    UTBreakIterator tIterator(eBreakStyle);
    tIterator.SetText(m_szOriginalText);



    int32 nOriginalIndex = ToOriginalIndex(nFormattedIndex);
    int32 nCodepointIndex = sUTF8BytesToCodepoints(m_szOriginalText, nOriginalIndex);

    int32 nNewCodepointIndex = nCodepointIndex;
    do {

        nNewCodepointIndex = tIterator.Following(nNewCodepointIndex);

    } while (IndexIsInsideTextDelta(nNewCodepointIndex, 0xff));

    int32 nNewIndexInBytes;
    if (nNewCodepointIndex == -1) {
        nNewIndexInBytes = m_nOriginalLength;
    } else {




        nNewIndexInBytes = sCodepointsToUTF8Bytes(m_szOriginalText, nNewCodepointIndex);
    }

    return ToFormattedIndex(nNewIndexInBytes);
}




uint32 FormattedText::GetPrevFormattedChar(uint32 nFormattedIndex) const
{

    return GetPrevBreak(nFormattedIndex, 0);
}




uint32 FormattedText::GetNextFormattedChar(uint32 nFormattedIndex) const
{

    return GetNextBreak(nFormattedIndex, 0);
}




uint32 FormattedText::GetPrevFormattedWord(uint32 nFormattedIndex) const
{

    return GetPrevBreak(nFormattedIndex, 3);
}





uint32 FormattedText::GetNextFormattedWord(uint32 nFormattedIndex) const
{

    return GetNextBreak(nFormattedIndex, 3);
}




BOOL FormattedText::FindNearestLineBreak(UTBreakIterator & tIterator, LPCSTR szStrippedText, int32 & nLineBreakOriginalIndex, const int32 nOriginalIndexMin, const int32 nOriginalIndexStart, int32 & nBreakHintIndexUsed) const
{


    uint32 uIgnoreFlags = (1 << TEXTDELTA_HYPERLINK) | (1 << TEXTDELTA_CHARACTER);


    nBreakHintIndexUsed = -1;
    int32 nOriginalIndexStartStripped = ConvertIdxToStrippedIdx(nOriginalIndexStart, uIgnoreFlags);





    int32 nLineBreakCodepoint = sUTF8BytesToCodepoints(m_szOriginalText, nOriginalIndexStart);
    int32 nLineBreakCodepointStripped = sUTF8BytesToCodepoints(szStrippedText, nOriginalIndexStartStripped);


    BOOL bForceUseOfBreakHints = UnicodeText::GetCurrentLocale() == XLOCALE_JAPANESE && m_fContainsBreakHints;



    if (!bForceUseOfBreakHints && tIterator.IsBoundary(nLineBreakCodepointStripped) &&

        !IndexIsInsideTextDelta(nLineBreakCodepoint, 0xff)) {



        nLineBreakOriginalIndex = nOriginalIndexStart;

        while (!sByteIsCodepointStart(m_szOriginalText[nLineBreakOriginalIndex])) {
            nLineBreakOriginalIndex--;
        }
        return TRUE;
    }


    int32 nCodepointMin = sUTF8BytesToCodepoints(m_szOriginalText, nOriginalIndexMin);

    do {
        int32 nLineBreakCodepointStrippedFromIterator = tIterator.Preceding(nLineBreakCodepointStripped);
        int32 nLineBreakCodepointFromIterator = -1;
        if (nLineBreakCodepointStrippedFromIterator != -1) {
            int32 nLineBreakOriginalStripped = sCodepointsToUTF8Bytes(szStrippedText, nLineBreakCodepointStrippedFromIterator);
            int32 nLineBreakOriginal = ConvertIdxFromStrippedIdx(nLineBreakOriginalStripped, uIgnoreFlags);

            nLineBreakCodepointFromIterator = sUTF8BytesToCodepoints(m_szOriginalText, nLineBreakOriginal);
        }





        if (nLineBreakCodepoint >= 0 && m_fContainsBreakHints) {
            int32 nHintIndex = FindPrecedingBreakHintIndex(nLineBreakCodepoint, !bForceUseOfBreakHints);


            if (nHintIndex != -1) {

                FormattedTextDelta * ptHintDelta = m_arTextDeltas[nHintIndex];
                if (bForceUseOfBreakHints || (int32)ptHintDelta->nCodepointIndex > nLineBreakCodepointFromIterator) {


                    nLineBreakCodepoint = ptHintDelta->nCodepointIndex + ptHintDelta->nCodepointsReplaced;
                    int32 nLineBreakOriginal = sCodepointsToUTF8Bytes(m_szOriginalText, nLineBreakCodepoint);
                    int32 nLineBreakOriginalStripped = ConvertIdxToStrippedIdx(nLineBreakOriginal, uIgnoreFlags);
                    nLineBreakCodepointStripped = sUTF8BytesToCodepoints(szStrippedText, nLineBreakOriginalStripped);
                    nBreakHintIndexUsed = nHintIndex;
                }
            }
        }

        if (nBreakHintIndexUsed == -1) {
            nLineBreakCodepointStripped = nLineBreakCodepointStrippedFromIterator;
            nLineBreakCodepoint = nLineBreakCodepointFromIterator;
        }



    } while (IndexIsInsideTextDelta(nLineBreakCodepoint, 0xff) && nLineBreakCodepoint > nCodepointMin);

    if (nLineBreakCodepoint == -1 || nLineBreakCodepoint <= nCodepointMin) {


        nBreakHintIndexUsed = -1;
        return FALSE;
    }



    nLineBreakOriginalIndex = sCodepointsToUTF8Bytes(m_szOriginalText, nLineBreakCodepoint);


    return TRUE;
}



BOOL FormattedText::IndexIsInsideTextDelta(int32 nCodepointIndex, uint32 dwDeltaSearchMask) const
{


    if (nCodepointIndex < 0) {
        return FALSE;
    }


    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];
        if (((ptDelta->nCodepointsReplaced ? (int32)ptDelta->nCodepointIndex < nCodepointIndex : (int32)ptDelta->nCodepointIndex <= nCodepointIndex) &
             nCodepointIndex < (int32)(ptDelta->nCodepointIndex + ptDelta->nCodepointsReplaced)) &&
            ((1 << ptDelta->eType) & dwDeltaSearchMask)) {
            return TRUE;
        }
    }

    return FALSE;
}








int32 FormattedText::FindPrecedingBreakHintIndex(int32 nCodepointIndex, BOOL bIgnoreNewLines) const
{







    for (int32 i = (int32)m_arTextDeltas.size() - 1; i >= 0; i--) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];
        if ((int32)ptDelta->nCodepointIndex < nCodepointIndex) {

            if (!bIgnoreNewLines && ptDelta->eType == TEXTDELTA_CHARACTER && ((FormattedCharacterDelta *)ptDelta)->uChar == '\n') {

                return -1;
            }




            if (ptDelta->IsBreakHint()) {
                return i;
            }
        }
    }


    return -1;
}




BOOL FormattedText::ContainsBreakHints() const
{

    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];
        if (ptDelta->IsBreakHint()) {
            return TRUE;
        }
    }

    return FALSE;
}





BOOL FormattedText::ContainsIcons() const
{

    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];
        if (ptDelta->eType == TEXTDELTA_FONTICON) {
            return TRUE;
        }
    }

    return FALSE;
}

// Not called anywhere in the binary; the linker dropped it. The DWARF keeps its locals
// (nFormattedOffset, i, ptDelta) and the inlined vector_erase_at; the rest of the body is a guess.
void FormattedText::ClearDeltas(
    FormattedTextDeltaType eDeltaType)
{
    int32 nFormattedOffset = 0;
    for (uint32 i = 0; i < m_arTextDeltas.size();) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];
        if (ptDelta->eType == eDeltaType) {
            nFormattedOffset += ptDelta->nFormattedTextLength;
            sDeleteFormattedTextDelta(ptDelta);

            blzext::vector_erase_at(m_arTextDeltas, i);
        } else {
            ptDelta->nFormattedTextIndex -= nFormattedOffset;
            i++;
        }
    }

    ComputeHash();
}





void FormattedText::ClearDeltas()
{
    if (m_arTextDeltas.size() > 0) {




        for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

            sDeleteFormattedTextDelta(m_arTextDeltas[i]);
        }
        m_arTextDeltas.clear();

        ComputeHash();
    }
}



int32 FormattedText::GetDeltaInsertionIndex(FormattedTextDelta * pDelta) const
{
    int32 nDeltaIndex;


    for (nDeltaIndex = 0; nDeltaIndex < (int32)m_arTextDeltas.size(); nDeltaIndex++) {

        const FormattedTextDelta * pDeltaInList = m_arTextDeltas[nDeltaIndex];
        if (pDelta->nOriginalTextIndex > pDeltaInList->nOriginalTextIndex) {
            continue;
        }

        if (pDelta->nOriginalTextIndex != pDeltaInList->nOriginalTextIndex) {
            break;
        }




        if (pDelta->nFormattedTextIndex > pDeltaInList->nFormattedTextIndex) {
            continue;
        }
        if (pDelta->nOriginalBytesReplaced == 0 || pDeltaInList->nOriginalBytesReplaced != 0) {
            break;
        }
    }



    return nDeltaIndex;
}




void FormattedText::InsertDelta(FormattedTextDelta * pDelta, const int32 nInsertionPoint)
{

    if (nInsertionPoint < (int32)m_arTextDeltas.size()) {


        blzext::vector_insert_at(m_arTextDeltas, nInsertionPoint, pDelta);
        if (pDelta->nFormattedTextLength > 0) {



            for (int32 i = nInsertionPoint + 1; i < (int32)m_arTextDeltas.size(); i++) {

                m_arTextDeltas[i]->nFormattedTextIndex += pDelta->nFormattedTextLength;
            }
        }
    } else {


        m_arTextDeltas.push_back(pDelta);
    }

    ComputeHash();
}




void FormattedText::Truncate(uint32 nIndex, LPCSTR szReplacementString)
{
    FormattedTextDelta * pDelta;
    LPCSTR uszOriginalTruncationStart;



    for (int32 i = (int32)m_arTextDeltas.size() - 1; i >= 0; i--) {

        int32 nStartIndexOfThisTextDelta = m_arTextDeltas[i]->nOriginalTextIndex;
        int32 nEndIndexOfThisTextDelta = nStartIndexOfThisTextDelta + m_arTextDeltas[i]->nOriginalBytesReplaced;
        if ((uint32)nEndIndexOfThisTextDelta < nIndex) {
            break;
        }
        nIndex = Math::Min(nIndex, (uint32)nStartIndexOfThisTextDelta);
        blzext::vector_erase_reorder(m_arTextDeltas, i);
    }







    nIndex = Math::Min(nIndex, Length());
    if (szReplacementString != NULL) {


        FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)sAllocFormattedTextDelta(TEXTDELTA_HYPERLINK);

        pHyperlinkDelta->tFormattedText = FormattedText(szReplacementString, PARSE_DEFAULT, TRUE);
        pHyperlinkDelta->nFormattedTextLength = pHyperlinkDelta->tFormattedText.Length();
        pDelta = pHyperlinkDelta;
    } else {

        FormattedCharacterDelta * pCharDelta = (FormattedCharacterDelta *)sAllocFormattedTextDelta(TEXTDELTA_CHARACTER);

        pCharDelta->uChar = cEllipsisChar;
        pCharDelta->nFormattedTextLength = strlen(Blizzard::Unicode::GetUTF8FromCodepoint(cEllipsisChar).c_str());
        pDelta = pCharDelta;
    }

    uszOriginalTruncationStart = m_szOriginalText + nIndex;
    pDelta->nFormattedTextIndex = ToFormattedIndex(nIndex);
    pDelta->nOriginalBytesReplaced = (m_szOriginalText + m_nOriginalLength) - uszOriginalTruncationStart;
    pDelta->nOriginalTextIndex = nIndex;
    pDelta->nCodepointIndex = sUTF8BytesToCodepoints(m_szOriginalText, nIndex);
    pDelta->nCodepointsReplaced = sUTF8BytesToCodepoints(uszOriginalTruncationStart, pDelta->nOriginalBytesReplaced);

    InsertDelta(pDelta);

    ComputeHash();
}




FormattedTextDelta * FormattedText::AddCharacterDelta(uint32 nIndex, uint32 uChar, BOOL fReplaceCharAtIndex)
{
    int32 nIndexCharSize;
    FormattedCharacterDelta * pDelta;

    nIndex = Math::Pin(nIndex, 0u, Length());


    if (fReplaceCharAtIndex) {
        LPCSTR uszString = m_szOriginalText + nIndex;
        LPCSTR uszStringSaved = uszString;
        Blizzard::Unicode::GetCodepointFromUTF8(&uszString);
        nIndexCharSize = uszString - uszStringSaved;
    } else {
        nIndexCharSize = 0;
    }

    pDelta = (FormattedCharacterDelta *)sAllocFormattedTextDelta(TEXTDELTA_CHARACTER);
    pDelta->nFormattedTextIndex = ToFormattedIndex(nIndex);
    pDelta->nFormattedTextLength = strlen(Blizzard::Unicode::GetUTF8FromCodepoint(uChar).c_str());
    pDelta->nOriginalBytesReplaced = nIndexCharSize;
    pDelta->nOriginalTextIndex = nIndex;
    pDelta->nCodepointIndex = sUTF8BytesToCodepoints(m_szOriginalText, nIndex);
    pDelta->nCodepointsReplaced = nIndexCharSize > 0 ? 1 : 0;
    pDelta->uChar = uChar;

    return pDelta;
}




FormattedTextDelta * FormattedText::AddBulletIndentDelta(uint32 nIndex, uint32 nSpaces)
{
    FormattedBulletIndentDelta * pDelta;

    nIndex = Math::Pin(nIndex, 0u, Length());


    pDelta = (FormattedBulletIndentDelta *)sAllocFormattedTextDelta(TEXTDELTA_BULLET_INDENT);
    pDelta->nFormattedTextIndex = ToFormattedIndex(nIndex);
    pDelta->nOriginalTextIndex = nIndex;
    pDelta->nOriginalBytesReplaced = 0;
    pDelta->nFormattedTextLength = 1;
    pDelta->nCodepointIndex = sUTF8BytesToCodepoints(m_szOriginalText, nIndex);
    pDelta->nCodepointsReplaced = 0;
    pDelta->nSpaces = nSpaces;

    return pDelta;
}




int32 FormattedText::sAddSegment(blz::vector<FormattedText, blz::allocator<FormattedText> > & arFormattedTexts, LPCSTR uszSplitStart, const uint32 nSplitLength, const int32 nCurrentDeltaIndex, const BOOL fCopyCurrentDelta, const int32 nLastSplitDeltaIndex, const int32 nLastSplitOriginalIndex, const int32 nLastSplitCodepointIndex, const int32 nFormattedLength, const BOOL fDeepCopy) const
{









    FormattedText tSegment(uszSplitStart, nSplitLength, PARSE_NONE, fDeepCopy);
    tSegment.SetIsPassword(IsPassword());



    int32 nLastDeltaToCopy = fCopyCurrentDelta ? nCurrentDeltaIndex : nCurrentDeltaIndex - 1;
    nLastDeltaToCopy = Math::Min(nLastDeltaToCopy, (int32)m_arTextDeltas.size() - 1);

    int32 nNumToCopy = nLastDeltaToCopy - nLastSplitDeltaIndex;
    if (nNumToCopy) {

        tSegment.m_arTextDeltas.reserve(nNumToCopy);

        for (int32 i = nLastSplitDeltaIndex + 1; i <= nLastDeltaToCopy; i++) {

            FormattedTextDelta * pCopy = sCopyFormattedTextDelta(m_arTextDeltas[i]);






            pCopy->nOriginalTextIndex = Math::Max((int32)pCopy->nOriginalTextIndex - nLastSplitOriginalIndex, 0);
            pCopy->nFormattedTextIndex = Math::Max((int32)pCopy->nFormattedTextIndex - nFormattedLength, 0);
            pCopy->nCodepointIndex = Math::Max((int32)pCopy->nCodepointIndex - nLastSplitCodepointIndex, 0);

            tSegment.m_arTextDeltas.push_back(pCopy);
        }
    }
    if (arFormattedTexts.size() == 0) {
        const uint32 PRESIZE_VALUE = 4;
        arFormattedTexts.reserve(PRESIZE_VALUE);
    }
    arFormattedTexts.push_back(tSegment);
    tSegment.ComputeHash();

    return nLastDeltaToCopy;
}






void FormattedText::SplitIntoLines(blz::vector<FormattedText, blz::allocator<FormattedText> > & arFormattedTexts) const
{




    XASSERT(m_szOriginalText != NULL);



    const BOOL fDeepCopy = m_sOriginalText.Length() != 0;
    uint32 nCurrentDeltaIndex = 0;
    uint32 nFormattedLength = 0;
    uint32 nLastSplitOriginalIndex = 0;
    uint32 nLastSplitCodepointIndex = 0;
    uint32 nLastSplitFormattedLength = 0;
    int32 nLastSplitDeltaIndex = -1;
    LPCSTR uszString = m_szOriginalText;
    LPCSTR uszStringStart = m_szOriginalText;
    LPCSTR uszStringEnd = m_szOriginalText + m_nOriginalLength;

    while (uszString < uszStringEnd) {

        if (nCurrentDeltaIndex < m_arTextDeltas.size() &&
            m_arTextDeltas[nCurrentDeltaIndex]->nOriginalTextIndex == (uint32)(uszString - uszStringStart)) {

            int32 nTotalBytesReplaced = 0;
            while (nCurrentDeltaIndex < m_arTextDeltas.size() &&
                   m_arTextDeltas[nCurrentDeltaIndex]->nOriginalTextIndex == (uint32)(uszString - uszStringStart)) {

                FormattedTextDelta * pDelta = m_arTextDeltas[nCurrentDeltaIndex];

                if (pDelta->eType == TEXTDELTA_CHARACTER && ((FormattedCharacterDelta *)pDelta)->uChar == '\n') {

                    LPCSTR uszSplitStart = uszStringStart + nLastSplitOriginalIndex;
                    LPCSTR uszSplitEnd = uszString + pDelta->nOriginalBytesReplaced;
                    nLastSplitDeltaIndex = sAddSegment(arFormattedTexts, uszSplitStart, uszSplitEnd - uszSplitStart,
                                                       nCurrentDeltaIndex, TRUE, nLastSplitDeltaIndex,
                                                       nLastSplitOriginalIndex, nLastSplitCodepointIndex,
                                                       nLastSplitFormattedLength, fDeepCopy);
                    nLastSplitOriginalIndex = uszSplitEnd - uszStringStart;
                    nLastSplitCodepointIndex = pDelta->nCodepointIndex + pDelta->nCodepointsReplaced;
                    nLastSplitFormattedLength = nFormattedLength + pDelta->nFormattedTextLength;
                }
                nFormattedLength += pDelta->nFormattedTextLength;
                nTotalBytesReplaced += pDelta->nOriginalBytesReplaced;
                nCurrentDeltaIndex++;
            }
            uszString += nTotalBytesReplaced;
        } else {

            LPCSTR uszStringSaved = uszString;

            uint32 uStrChar = Blizzard::Unicode::GetCodepointFromUTF8(&uszString);
            uint32 nCharSize = uszString - uszStringSaved;
            if (uStrChar == '\n') {


                LPCSTR uszSplitStart = uszStringStart + nLastSplitOriginalIndex;
                nLastSplitDeltaIndex = sAddSegment(arFormattedTexts, uszSplitStart, uszString - uszSplitStart,
                                                   nCurrentDeltaIndex, FALSE, nLastSplitDeltaIndex,
                                                   nLastSplitOriginalIndex, nLastSplitCodepointIndex,
                                                   nFormattedLength, fDeepCopy);
                nLastSplitOriginalIndex = uszString - uszStringStart;
                nLastSplitCodepointIndex = sUTF8BytesToCodepoints(uszStringStart, nLastSplitOriginalIndex);
                nLastSplitFormattedLength = nFormattedLength + nCharSize;
            }
            nFormattedLength += nCharSize;
        }
        if (uszString == uszStringEnd && nLastSplitOriginalIndex != (uint32)(uszStringEnd - uszStringStart)) {

            LPCSTR uszSplitStart = uszStringStart + nLastSplitOriginalIndex;
            sAddSegment(arFormattedTexts, uszSplitStart, uszStringEnd - uszSplitStart, nCurrentDeltaIndex, TRUE,
                        nLastSplitDeltaIndex, nLastSplitOriginalIndex, nLastSplitCodepointIndex, nFormattedLength,
                        fDeepCopy);
        }
    }
    if (arFormattedTexts.size() == 0) {
        arFormattedTexts.push_back(*this);
    }
}





int32 FormattedText::ConvertIdxToStrippedIdx(int32 nIndex, uint32 uIgnoreFlags) const
{

    int32 nNewIndex = nIndex;


    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];


        if ((int32)ptDelta->nOriginalTextIndex >= nIndex) {
            break;
        }


        if (!((1 << ptDelta->eType) & uIgnoreFlags)) {

            nNewIndex = Math::Max(nNewIndex - (int32)ptDelta->nOriginalBytesReplaced, 0);
        }
    }


    return nNewIndex;
}



int32 FormattedText::ConvertIdxFromStrippedIdx(int32 nIndex, uint32 uIgnoreFlags) const
{

    int32 nNewIndex = nIndex;


    for (uint32 i = 0; i < m_arTextDeltas.size(); i++) {

        FormattedTextDelta * ptDelta = m_arTextDeltas[i];


        if ((int32)ptDelta->nOriginalTextIndex > nNewIndex) {
            break;
        }


        if (!((1 << ptDelta->eType) & uIgnoreFlags)) {

            nNewIndex += ptDelta->nOriginalBytesReplaced;
        }
    }


    return nNewIndex;
}

// Not called anywhere in the binary; the linker dropped it. Only the inlined
// sByteIsCodepointStart (line 1834) is in the DWARF; the rest of the body is a guess.
BOOL FormattedText::IsValidOriginalTextPosition(
    int32 nOriginalTextIndex)
{
    if (nOriginalTextIndex < 0 || nOriginalTextIndex > (int32)m_nOriginalLength) {
        return FALSE;
    }



    return nOriginalTextIndex == (int32)m_nOriginalLength || sByteIsCodepointStart(m_szOriginalText[nOriginalTextIndex]);
}






static FormattedTextDelta * sAllocFormattedTextDelta(FormattedTextDeltaType eType)
{
    FormattedTextGlobals *& ptFormattedTextGlobals = sgtAppGlobals.ptUIAppGlobals->ptFormattedTextGlobals;
    switch (eType) {
    case TEXTDELTA_FONTCOLOR_PUSH: {




        FormattedColorPushDelta * ptColorPush = ptFormattedTextGlobals->poolColorPushDeltas.Alloc_Tracked();
        ptColorPush->eType = eType;
        return ptColorPush;
    }
    case TEXTDELTA_FONTFACE_PUSH: {

        FormattedFontFacePushDelta * ptFontPush = ptFormattedTextGlobals->poolFontPushDeltas.Alloc_Tracked();
        ptFontPush->eType = eType;
        return ptFontPush;
    }
    case TEXTDELTA_HYPERLINK: {

        FormattedHyperlinkDelta * ptHyperlinkDelta = ptFormattedTextGlobals->poolHyperlinkDeltas.Alloc_Tracked();
        ptHyperlinkDelta->eType = eType;
        return ptHyperlinkDelta;
    }
    case TEXTDELTA_FONTICON: {

        FormattedIconDelta * ptIconDelta = ptFormattedTextGlobals->poolIconDeltas.Alloc_Tracked();
        ptIconDelta->eType = eType;
        ptIconDelta->hImage = 0;
        ptIconDelta->eCollection = -1;
        ptIconDelta->flScale = 1.0f;
        return ptIconDelta;
    }
    case TEXTDELTA_CHARACTER: {

        FormattedCharacterDelta * ptCharacterDelta = ptFormattedTextGlobals->poolCharacterDeltas.Alloc_Tracked();
        ptCharacterDelta->eType = eType;
        ptCharacterDelta->uChar = 0;
        return ptCharacterDelta;
    }
    case TEXTDELTA_BULLET_INDENT: {

        FormattedBulletIndentDelta * ptIndentDelta = ptFormattedTextGlobals->poolIndentDeltas.Alloc_Tracked();
        ptIndentDelta->eType = eType;
        ptIndentDelta->nSpaces = 0;
        return ptIndentDelta;
    }
    default: {

        FormattedTextDelta * ptTextDelta = ptFormattedTextGlobals->poolBaseDeltas.Alloc_Tracked();
        ptTextDelta->eType = eType;
        return ptTextDelta;
    }
    }
}




static void sDeleteFormattedTextDelta(FormattedTextDelta *& pDelta)
{
    FormattedTextGlobals *& ptFormattedTextGlobals = sgtAppGlobals.ptUIAppGlobals->ptFormattedTextGlobals;
    if (pDelta != NULL) {



        switch (pDelta->eType) {
        case TEXTDELTA_FONTCOLOR_PUSH: {


            FormattedColorPushDelta * pColorPush = (FormattedColorPushDelta *)pDelta;


            ptFormattedTextGlobals->poolColorPushDeltas.Free(pColorPush);
            break;
        }
        case TEXTDELTA_FONTFACE_PUSH: {
            FormattedFontFacePushDelta * pFontPush = (FormattedFontFacePushDelta *)pDelta;

            ptFormattedTextGlobals->poolFontPushDeltas.Free(pFontPush);
            break;
        }
        case TEXTDELTA_FONTICON: {
            FormattedIconDelta * pIcon = (FormattedIconDelta *)pDelta;

            ptFormattedTextGlobals->poolIconDeltas.Free(pIcon);
            break;
        }
        case TEXTDELTA_HYPERLINK: {
            FormattedHyperlinkDelta * pHyperlink = (FormattedHyperlinkDelta *)pDelta;

            ptFormattedTextGlobals->poolHyperlinkDeltas.Free(pHyperlink);
            break;
        }
        case TEXTDELTA_CHARACTER: {
            FormattedCharacterDelta * pCharacter = (FormattedCharacterDelta *)pDelta;

            ptFormattedTextGlobals->poolCharacterDeltas.Free(pCharacter);
            break;
        }
        case TEXTDELTA_BULLET_INDENT: {
            FormattedBulletIndentDelta * pIndent = (FormattedBulletIndentDelta *)pDelta;

            ptFormattedTextGlobals->poolIndentDeltas.Free(pIndent);
            break;
        }
        default:

            ptFormattedTextGlobals->poolBaseDeltas.Free(pDelta);
            break;
        }

        pDelta = NULL;
    }
}




static FormattedTextDelta * sCopyFormattedTextDelta(FormattedTextDelta * pSrc)
{
    if (pSrc == NULL) {
        return NULL;
    }



    switch (pSrc->eType) {
    case TEXTDELTA_FONTCOLOR_PUSH: {
        FormattedColorPushDelta * pColorDelta = (FormattedColorPushDelta *)pSrc;
        FormattedColorPushDelta * pColorDeltaCopy = (FormattedColorPushDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pColorDeltaCopy = *pColorDelta;
        return pColorDeltaCopy;
    }
    case TEXTDELTA_FONTFACE_PUSH: {

        FormattedFontFacePushDelta * pFontDelta = (FormattedFontFacePushDelta *)pSrc;
        FormattedFontFacePushDelta * pFontDeltaCopy = (FormattedFontFacePushDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pFontDeltaCopy = *pFontDelta;
        return pFontDeltaCopy;
    }
    case TEXTDELTA_FONTICON: {

        FormattedIconDelta * pIconDelta = (FormattedIconDelta *)pSrc;
        FormattedIconDelta * pIconDeltaCopy = (FormattedIconDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pIconDeltaCopy = *pIconDelta;
        return pIconDeltaCopy;
    }
    case TEXTDELTA_HYPERLINK: {

        FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)pSrc;
        FormattedHyperlinkDelta * pHyperlinkDeltaCopy = (FormattedHyperlinkDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pHyperlinkDeltaCopy = *pHyperlinkDelta;
        return pHyperlinkDeltaCopy;
    }
    case TEXTDELTA_CHARACTER: {

        FormattedCharacterDelta * pCharacterDelta = (FormattedCharacterDelta *)pSrc;
        FormattedCharacterDelta * pCharacterDeltaCopy = (FormattedCharacterDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pCharacterDeltaCopy = *pCharacterDelta;
        return pCharacterDeltaCopy;
    }
    case TEXTDELTA_BULLET_INDENT: {
        FormattedBulletIndentDelta * pIndentDelta = (FormattedBulletIndentDelta *)pSrc;
        FormattedBulletIndentDelta * pIndentDeltaCopy = (FormattedBulletIndentDelta *)sAllocFormattedTextDelta(pSrc->eType);

        *pIndentDeltaCopy = *pIndentDelta;
        return pIndentDeltaCopy;
    }
    default: {

        FormattedTextDelta * pTextDelta = sAllocFormattedTextDelta(pSrc->eType);

        *pTextDelta = *pSrc;
        return pTextDelta;
    }
    }
}

// Truncate calls the out-of-line copy of vector_erase_reorder; without this instantiation clang
// inlines it there.
template void blzext::vector_erase_reorder<FormattedTextDelta *>(blz::vector<FormattedTextDelta *, blz::allocator<FormattedTextDelta *> > &, const size_t);
