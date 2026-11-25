#include "UITextControl.h"

#include <string.h>
#include <strings.h>

#include "../../../../BlizzardCore/Include/Blizzard/String.h"
#include "../../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../../Contrib/Contrib/blz/include/blz/algo_seq_const.h"
#include "../../../../Contrib/Contrib/blz/include/blz/string.h"
#include "../../../Common/Definitions/Definitions.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Common/SNO/SNOFiles.h"
#include "../../../Sigma/DataStructures/XChainedFixedBlockAllocator.h"
#include "../../../Sigma/ErrorManager/ErrorManager.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/UnicodeText.h"
#include "../../../Sigma/UnicodeTextBreakIterator.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Font/Font.h"
#include "../../Input/Input.h"
#include "../UI.h"
#include "../UIAttributes.h"
#include "UIEditControl.h"

// Declarations of the functions this file calls whose headers are not adopted yet; the DWARF
// gives their signatures (file noted on each) but not where they were declared.
CRefString StringListFetch(LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
CRefString StringListFetch(SNO snoStringList, LPCSTR szLabel);  // X/Common/Definitions/StringListDefinition.cpp
CRefString LocalizedSprintf(LPCSTR cpFmtStr, ...);  // X/Common/Definitions/StringListDefinition.cpp
int32 GetPreviousCharacterUTF8(LPCSTR pszText, const int32 nCursorPos);  // X/Client/Font/FormattedText.cpp
BOOL CHyperlinkGetURL(LPCSTR szHyperLink, CRefString & sURL);  // X/Common/Hyperlink.cpp
BOOL InputCtrlKeyIsDown(InputContext eContext);  // X/Client/Input/Input.cpp
int32 InputGetControllerType(int32 nControllerIndex);  // X/Client/Input/Input.cpp
void SetNextControllerTypeForIconAdjustment(int32 nControllerType);  // X/Client/Input/InputNX64.cpp
void InputPlatformSetClipboard(LPCSTR szText);  // X/Client/Input/InputNX64.cpp
UIEdit & UI_GetEditFromID(const UIID & id);  // X/Client/UserInterface/UI.cpp
int32 UI_GetPrimaryUIController();  // X/Client/UserInterface/UI.cpp
const UIID & UI_GetMousedOverControl();  // X/Client/UserInterface/UI.cpp
void UI_SetCapture(const UIID & idCapture);  // X/Client/UserInterface/UI.cpp
void ConvertUICToPixels(const Vector2D & vecUIC, Vector2D & vecPixels);  // X/Client/UserInterface/UIUtils.cpp
void ConvertUICToPixels(const Rect2D & rectUIC, Rect2D & rectPixels);  // X/Client/UserInterface/UIUtils.cpp
void ConvertPixelsToUIC(const IVector2D & vecPixels, Vector2D & vecUIC);  // X/Client/UserInterface/UIUtils.cpp
void GfxRectDrawDeferred(const Rect2D & wrRect, const RGBAColor & rgbaColor, const float flZ, const RenderLayer eRenderLayerOverride, BOOL fInRenderDraw);  // X/Client/Gfx/GfxPrimitives.cpp

// X/Client/OnlineService/CStore.h and X/Common/OnlineService/Store.h. Only the layout of StoreItem
// and its inline constructor and destructor (CStore.h:137-143) are declared.
namespace OnlineService {
typedef int32 StoreCurrency;
enum StoreCategories {
    STORE_CATEGORY_INVALID = -1,
};
typedef uint32 StoreProductId;
typedef int64 StoreItemPriceType;
typedef int32 StoreItemEntitlementType;
struct StoreItemPrice {  // size 0x18
    StoreCurrency m_eCurrency;  // 0x00
    StoreItemPriceType m_dwCurrentFixedPointPrice;  // 0x08
    StoreItemPriceType m_dwFullFixedPointPrice;  // 0x10
};
struct StoreItemEntitlement {  // size 0x10
    uint64 m_data;  // 0x00 (a union of GBID, ContentLicenseId and uint64 in the original)
    StoreItemEntitlementType m_eType;  // 0x08
    uint32 m_nMaxStackCount;  // 0x0c
};
class StoreItemPurchaseRestriction {  // size 0xc
public:
    ContentLicenseId m_idLicense;  // 0x00
    int32 m_nMinRequired;  // 0x04
    int32 m_nMaxAllowed;  // 0x08
};
struct StoreItem {  // size 0xa8
    StoreItem()  // line 139
        : m_eCategory(STORE_CATEGORY_INVALID),
          m_timeReleaseDate(0),
          m_dwDurationSeconds(0),
          m_productId(0) {}
    StoreCategories m_eCategory;  // 0x00
    blz::vector<StoreItemEntitlement, blz::allocator<StoreItemEntitlement> > m_arEntitlements;  // 0x08
    blz::vector<StoreItemPrice, blz::allocator<StoreItemPrice> > m_arPrices;  // 0x20
    blz::vector<int, blz::allocator<int> > m_arAttributes;  // 0x38
    blz::vector<StoreItemPurchaseRestriction, blz::allocator<StoreItemPurchaseRestriction> > m_arPurchaseRestrictions;  // 0x50
    Blizzard::Time::Timestamp m_timeReleaseDate;  // 0x68
    uint64 m_dwDurationSeconds;  // 0x70
    blz::string m_bundleLabel;  // 0x78
    StoreProductId m_productId;  // 0xa0
};
static_assert(sizeof(StoreItem) == 0xa8, "StoreItem");
}  // namespace OnlineService
BOOL CHyperlinkGetStoreItem(LPCSTR szHyperLink, OnlineService::StoreItem & tItem);  // X/Common/Hyperlink.cpp

#include "../Interface/UIToolTipManager.h"
class UIToolTip;




extern UIToolTipManager gtTooltipMgr;  // UIToolTipManager.cpp:24

// X/Client/Definitions/FontDefinition.h. Only the members used here are declared.
struct FontDefinition {  // size 0xd8
    uint8 _unadopted0[0xd8];
    FontPointSizeData * GetPointSizeData(int32 nFontSize) const;
};
class FontGroupDef : public GroupDefinition {
};
extern FontGroupDef * gptGroupDefFont;  // FontDefinition.cpp
// line 377. LockRFast takes no reference, so the unlock does nothing.
template <> inline const FontDefinition * DefinitionLockR<FontDefinition>(SNO sno) {
    return (const FontDefinition *)gptGroupDefFont->LockRFast(sno);
}
template <> inline void DefinitionUnlockR<FontDefinition>(const FontDefinition * pdef) {
}
typedef ScopedDefinitionLockR<FontDefinition> ScopedFontDefinitionLockR;







enum {
    MIN_TEXT_ALLOC_LEN = 31,
    MAX_NUM_WORDWRAP_CHARACTERS = 8,
    STRING_ALLOCATOR_BLOCK_SIZE = 40,
    STRING_ALLOCATOR_BLOCK_COUNT = 2048,
};

struct UITextControlGlobals {  // size 0x40
    ChainedFixedBlockAllocator tStringAllocator;  // 0x00
    int32 nAllocStrings;  // 0x30
    int32 nMaxAllocStrings;  // 0x34
    int32 nSysMemStrings;  // 0x38
    int32 nMaxSysMemStrings;  // 0x3c
};

int32 gnFontBoost;
void UITextControlInitializeForGame()
{
    UITextControlGlobals *& ptTextControlGlobals = CGameGet().ptUIGlobals->ptTextControlGlobals;
    ptTextControlGlobals = (UITextControlGlobals *)SigmaMemoryNew(sizeof(UITextControlGlobals), 4, NULL, TRUE);
    ptTextControlGlobals->tStringAllocator.Initialize(STRING_ALLOCATOR_BLOCK_SIZE, STRING_ALLOCATOR_BLOCK_COUNT, NULL);
}
void UITextControlDisposeForGame()
{
    UITextControlGlobals *& ptTextControlGlobals = CGameGet().ptUIGlobals->ptTextControlGlobals;
    ptTextControlGlobals->tStringAllocator.Dispose();
    SigmaMemoryFree(ptTextControlGlobals, NULL);
    ptTextControlGlobals = NULL;
}
inline CHAR * sAllocString(int32 nLen)
{
    UITextControlGlobals * ptTextControlGlobals = CGameGet().ptUIGlobals->ptTextControlGlobals;
    CHAR * szString;
    if (nLen <= STRING_ALLOCATOR_BLOCK_SIZE)
    {
        szString = (CHAR *)ptTextControlGlobals->tStringAllocator.Alloc_Tracked();
        ptTextControlGlobals->nAllocStrings++;
        ptTextControlGlobals->nMaxAllocStrings = Math::Max(ptTextControlGlobals->nAllocStrings, ptTextControlGlobals->nMaxAllocStrings);
    }
    else
    {
        szString = (CHAR *)SigmaMemoryNew(nLen, 4, NULL, FALSE);
        ptTextControlGlobals->nSysMemStrings++;
        ptTextControlGlobals->nMaxSysMemStrings = Math::Max(ptTextControlGlobals->nSysMemStrings, ptTextControlGlobals->nMaxSysMemStrings);
    }
    szString[0] = '\0';
    return szString;
}
void sFreeString(CHAR * szString, int32 nLen)
{
    UITextControlGlobals * ptTextControlGlobals = CGameGet().ptUIGlobals->ptTextControlGlobals;
    if (nLen <= STRING_ALLOCATOR_BLOCK_SIZE)
    {
        ptTextControlGlobals->tStringAllocator.Free(szString);
        ptTextControlGlobals->nAllocStrings--;
    }
    else
    {
        SigmaMemoryFree(szString, NULL);
        ptTextControlGlobals->nSysMemStrings--;
    }
}
CHAR * sResizeString(CHAR * szString, const int32 nLen, const int32 nNewLen)
{
    CHAR * szNewString = szString;
    if (nLen != nNewLen)
    {
        if (nLen > STRING_ALLOCATOR_BLOCK_SIZE || nNewLen > STRING_ALLOCATOR_BLOCK_SIZE)
        {
            if (nLen > STRING_ALLOCATOR_BLOCK_SIZE && nNewLen > STRING_ALLOCATOR_BLOCK_SIZE)
            {
                return (CHAR *)SigmaMemoryResize(szString, NULL, nNewLen, 4);
            }
            else
            {
                szNewString = sAllocString(nNewLen);
                strcpy(szNewString, szString);
                sFreeString(szString, nLen);
            }
        }
    }
    return szNewString;
}
inline void TextCreationParams::sInit()
{
    eJustifyHorizontal = UIJH_NONE;
    eJustifyVertical = UIJV_NONE;
    rgbaText = crgbaUIGold;
    rgbaDropShadow = crgbaBlack;
    rgbaTextDisabled = crgbaGray;
    rgbaDropShadowDisabled = crgbaBlack;
    snoFont = GlobalSNOGet(GLOBALSNO_FONT_EXOCETLIGHT);
    nFontSize = 14;
    nFontSizeFallback = 0;
    flAdvanceScalar = 1.0f;
    fAllCaps = FALSE;
    eResizeToFit = RESIZETOFIT_DISABLED;
    fHyperlinkAware = FALSE;
    fWordWrap = FALSE;
    fShrinkToFit = FALSE;
    fValidateHyperlinks = TRUE;
    vecTextButtonPressOffsetUIC.x = 0.0f;
    vecTextButtonPressOffsetUIC.y = 0.0f;
    fUseDropShadow = FALSE;
    nLinesBeforeTruncation = 0;
    fIsPassword = FALSE;
    fSelectable = FALSE;
    vecTextInsetUIC.Set(0.0f, 0.0f);
    rectTextPaddingUIC.left = rectTextPaddingUIC.top = rectTextPaddingUIC.right = rectTextPaddingUIC.bottom = 0.0f;
    sDecoratedLabel = "";
    sPlaceholderTextLabel = "";
    rgbaPlaceholderText = crgbaGray;
    rgbaPlaceholderTextDropShadow = crgbaBlack;
}




TextCreationParams::TextCreationParams(const UIType eType, const UIID & id, const UIID & idParent)



    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}




TextCreationParams::TextCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)



    : WindowCreationParams(eType, szName, idParent)
{
    sInit();
}




void TextCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{


    WindowCreationParams::Load(snoUI, pControlData);

    eJustifyHorizontal = pControlData->GetInt32Value(ATT_TEXTJUSTIFY);
    eJustifyVertical = pControlData->GetInt32Value(ATT_TEXTJUSTIFY_VERTICAL);
    flAdvanceScalar = pControlData->GetFloat32Value(ATT_TEXT_ADVANCE_SCALAR);
    fAllCaps = pControlData->GetBoolValue(ATT_TEXT_ALL_CAPS);
    nLinesBeforeTruncation = pControlData->GetInt32Value(ATT_TEXT_LINES_BEFORE_TRUNCATION);

    UIColor uicolor = pControlData->GetUint32Value(ATT_FOREGROUND);
    rgbaText.Set(uicolor);
    uicolor = pControlData->GetUint32Value(ATT_DROPSHADOW);
    rgbaDropShadow.Set(uicolor);
    uicolor = pControlData->GetUint32Value(ATT_FOREGROUND_DISABLED);
    rgbaTextDisabled.Set(uicolor);

    snoFont = SNOFileLookupGUID(FONT_SNO, pControlData->GetAttributeValue(ATT_FONT));
    nFontSize = pControlData->GetInt32Value(ATT_FONT_SIZE);
    nFontSizeFallback = pControlData->GetInt32Value(ATT_FONT_SIZE_FALLBACK);
    fWordWrap = pControlData->GetBoolValue(ATT_WORDWRAP);
    fShrinkToFit = pControlData->GetBoolValue(ATT_SHRINK_TO_FIT);
    eResizeToFit = pControlData->GetInt32Value(ATT_RESIZE_TO_FIT);
    if (eResizeToFit == -1)
    {
        eResizeToFit = RESIZETOFIT_DISABLED;
    }
    fHyperlinkAware = pControlData->GetBoolValue(ATT_HYPERLINKAWARE);
    fIsPassword = pControlData->GetBoolValue(ATT_PASSWORD);
    fSelectable = pControlData->GetBoolValue(ATT_SELECTABLE);
    fUseDropShadow = pControlData->GetBoolValue(ATT_USE_DROP_SHADOW);

    sDecoratedLabel = pControlData->GetAttributeValue(ATT_TEXT);


    float flInsetX = pControlData->GetFloat32Value(ATT_TEXT_INSET_X);
    float flInsetY = pControlData->GetFloat32Value(ATT_TEXT_INSET_Y);
    vecTextInsetUIC.Set(flInsetX, flInsetY);

    float flPaddingL = pControlData->GetFloat32Value(ATT_TEXT_PADDING_L);
    float flPaddingR = pControlData->GetFloat32Value(ATT_TEXT_PADDING_R);
    float flPaddingT = pControlData->GetFloat32Value(ATT_TEXT_PADDING_T);
    float flPaddingB = pControlData->GetFloat32Value(ATT_TEXT_PADDING_B);
    rectTextPaddingUIC.Set(flPaddingL, flPaddingT, flPaddingR, flPaddingB);
}




void UIText::sInit(const TextCreationParams & tParams)
{

    m_uTCFlags = 0;
    m_nTextAllocLen = 0;
    m_fTextIsTruncated = FALSE;
    m_uszText = NULL;

    SetJustification(tParams.eJustifyHorizontal, tParams.eJustifyVertical);

    m_rgbaText = tParams.rgbaText;
    m_rgbaDropShadow = tParams.rgbaDropShadow;
    m_rgbaTextDisabled = tParams.rgbaTextDisabled;
    m_rgbaDropShadowDisabled = tParams.rgbaDropShadowDisabled;
    m_snoFont = tParams.snoFont;
    m_nFontSize = m_nFontSizeOriginal = tParams.nFontSize;


    m_nFontBoostSize = 0;
    unk_3bc = 0;

    if (tParams.fHyperlinkAware)
    {

        m_uTCFlags |= (1 << UITCF_HYPERLINKAWARE_BIT);
    }
    if (tParams.fValidateHyperlinks)
    {
        m_uTCFlags |= (1 << UITCF_VALIDATE_HYPERLINKS_BIT);
    }
    m_nLinesBeforeTruncation = tParams.nLinesBeforeTruncation;

    m_uTCFlags = !tParams.fAllCaps ? (m_uTCFlags & ~(1 << UITCF_ALL_CAPS_BIT)) : (m_uTCFlags | (1 << UITCF_ALL_CAPS_BIT));
    m_flAdvanceScalar = tParams.flAdvanceScalar;

    m_wrapLineScalar = 1.0f;

    SetSelectable(tParams.fSelectable);
    SetUseDropShadow(tParams.fUseDropShadow);

    m_nFontSizeFallback = tParams.nFontSizeFallback;
    if (tParams.nFontSizeFallback)
    {
        SetWordWrap(FALSE);
    }
    else
    {
        SetWordWrap(tParams.fWordWrap);
        m_uTCFlags = !tParams.fShrinkToFit ? (m_uTCFlags & ~(1 << UITCF_SHRINK_TO_FIT_BIT)) : (m_uTCFlags | (1 << UITCF_SHRINK_TO_FIT_BIT));
    }
    SetResizeToFit(tParams.eResizeToFit);
    m_uTCFlags = !tParams.fIsPassword ? (m_uTCFlags & ~(1 << UITCF_ISPASSWORD_BIT)) : (m_uTCFlags | (1 << UITCF_ISPASSWORD_BIT));

    SetEnabled(TRUE);
    m_nTopLine = 0;


    m_vecTextButtonPressOffsetUIC = tParams.vecTextButtonPressOffsetUIC;


    m_flTextScrollVerticalUIC = 0.0f;
    m_flTextScrollHorizontalUIC = 0.0f;

    m_vecTextInsetUIC = tParams.vecTextInsetUIC;
    m_rectTextPaddingUIC = tParams.rectTextPaddingUIC;

    SetText(StringListFetch(tParams.sDecoratedLabel), TRUE);

    m_nSelectionBegin = -1;
    m_nSelectionEnd = -1;

    m_bDragging = FALSE;
    m_nDragStartCursorPos = -1;

    m_nControllerIndexOverride = -1;
    m_nControllerType = -1;
}




UIText::UIText(const TextCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)



    : UIWindow(tParams, vecTL, vecDim)
{
    sInit(tParams);
}




UIText::UIText(const TextCreationParams & tParams, const Rect2D * pRect)



    : UIWindow(tParams, pRect)
{
    sInit(tParams);
}


UIText::~UIText()
{
    sFreeString(m_uszText, m_nTextAllocLen);
    m_uszText = NULL;
    m_nTextAllocLen = 0;
}




const FormattedText & UIText::GetText() const
{

    return m_tText;
}




void UIText::sApplyFormattingForCurrentDimensions()
{

    if (m_nFontSizeFallback != 0)
    {


        m_nFontSize = m_nFontSizeOriginal;
        m_uTCFlags &= ~(1 << UITCF_WORDWRAP_BIT);

        Rect2D rectTextExtentsUIC;
        GetTextRegionRectUIC(rectTextExtentsUIC);

        Vector2D tTextSizeUIC;
        GetTextRenderedSizeUIC(tTextSizeUIC, FALSE);
        if (tTextSizeUIC.x > rectTextExtentsUIC.Width())
        {
            m_nFontSize = m_nFontSizeFallback;
            m_uTCFlags |= (1 << UITCF_WORDWRAP_BIT);
        }
    }

    ApplyFormatting();
    ShrinkToFit();

    if (m_uTCFlags & ((1 << UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT) | (1 << UITCF_RESIZE_TO_FIT_VERTICAL_BIT)))
    {

        ResizeToFitText();
    }

    sTruncateIfNecessary();
}



// The icon scan updates the 32-bit index with the copied byte count.
void UIText::SetText(
    LPCSTR szNewText,
    const BOOL fDisallowUpcalls)
{
    if (m_uszText &&
        strcmp(m_uszText, szNewText) == 0)
    {
        return;
    }


    m_nControllerType = -1;

    int32 nTextLength = strlen(szNewText);
    int32 nAllocLen = Math::Max(nTextLength + 1, (int32)MIN_TEXT_ALLOC_LEN);

    if (m_nTextAllocLen == 0)
    {


        m_uszText = sAllocString(nAllocLen);
        m_nTextAllocLen = nAllocLen;
    }




    else if (m_nTextAllocLen < nAllocLen)
    {
        m_uszText = sResizeString(m_uszText, m_nTextAllocLen, nAllocLen);
        m_nTextAllocLen = nAllocLen;
    }



    Blizzard::String::Copy(m_uszText, szNewText, m_nTextAllocLen);

    BOOL bAllCaps = (m_uTCFlags >> UITCF_ALL_CAPS_BIT) & 1;


    if (nTextLength > 0 && UnicodeText::GetStringDisplayLocale() == XLOCALE_GERMAN)
    {
        if (m_snoFont == GlobalSNOGet(GLOBALSNO_FONT_EXOCETLIGHT))
            bAllCaps = TRUE;
    }


    if (bAllCaps)
    {
        UnicodeText::ToUpper(m_uszText, nTextLength + 1);

        const int32 cnIconTagLen = 6;
        LPCSTR cszIconTag = "{icon:";
        if (nTextLength >= cnIconTagLen)
        {

            for (int32 i = 0; i <= nTextLength; i++)
            {
                if (strncasecmp(&m_uszText[i], cszIconTag, cnIconTagLen) == 0)
                {
                    LPCSTR pClose = strchr(&m_uszText[i + cnIconTagLen], '}');
                    if (pClose)
                    {
                        size_t nLen = pClose - &m_uszText[i];
                        SigmaMemoryCopy(&m_uszText[i], &szNewText[i], nLen);
                        i += (int32)nLen;
                    }
                }
            }
        }
    }


    uint32 dwFlags = GetTextParseFlags();
    m_tText = FormattedText(m_uszText, dwFlags, FALSE);

    if ((m_uTCFlags >> UITCF_ISPASSWORD_BIT) & 1)
    {
        m_tText.SetIsPassword(TRUE);
    }

    sApplyFormattingForCurrentDimensions();

    SetText_Internal(szNewText, fDisallowUpcalls);

    if (!fDisallowUpcalls)
    {
        const UIID & id = GetID();
        if (UI_GetControlFromID(id).IsType(UIT_EDIT))
        {
            UIEdit & tEdit = UI_GetEditFromID(id);
            tEdit.SetTextUpcall();
        }
    }
}



void UIText::GetTextRegionRectUIC(Rect2D & rectTextExtentsUIC) const
{

    GetWindowRect(rectTextExtentsUIC);


    rectTextExtentsUIC.left += m_vecTextInsetUIC.x + m_rectTextPaddingUIC.left;
    rectTextExtentsUIC.top += m_vecTextInsetUIC.y + m_rectTextPaddingUIC.top;
    rectTextExtentsUIC.right -= m_vecTextInsetUIC.x + m_rectTextPaddingUIC.right;
    rectTextExtentsUIC.bottom -= m_vecTextInsetUIC.y + m_rectTextPaddingUIC.bottom;

    rectTextExtentsUIC.top += m_flTextScrollVerticalUIC;
    rectTextExtentsUIC.bottom += m_flTextScrollVerticalUIC;
}



void UIText::SetWordWrap(const BOOL bWrap)
{

    if (((m_uTCFlags >> UITCF_WORDWRAP_BIT) & 1) == bWrap)
    {
        return;
    }


    m_uTCFlags = !bWrap ? (m_uTCFlags & ~(1 << UITCF_WORDWRAP_BIT)) : (m_uTCFlags | (1 << UITCF_WORDWRAP_BIT));
    ApplyFormatting();
}




static BOOL sSkipEscapeSequence(LPCSTR & uszCurrentLine, const CRefString & sSeqStart)
{


    if (strncmp(uszCurrentLine, sSeqStart.str(), sSeqStart.Length()) == 0)
    {
        uszCurrentLine += sSeqStart.Length();

        LPCSTR pszCloseBrace = strchr(uszCurrentLine, '}');
        if (pszCloseBrace)
        {
            uszCurrentLine = pszCloseBrace + 1;
            return TRUE;
        }
    }
    return FALSE;
}



static int32 sCountLeadingBulletizedSpace(LPCSTR uszCurrentLine)
{

    const CRefString sBullet("{icon:bullet");
    const CRefString sCheck("{icon:check");
    const CRefString sSetDungeonBullet("{icon:Marker_Set");
    const CRefString sSetDungeonRedBullet("{icon:SetDungeon_RedSetIcon");
    const CRefString sColorSeqStartA("{c_");
    const CRefString sColorSeqStartB("{c:");

    int32 nCount = 0;
    BOOL fBulletFound = FALSE;
    while (uszCurrentLine && *uszCurrentLine)
    {
        if (*uszCurrentLine == ' ' || (uint8)*uszCurrentLine == (uint8)cNBSP)
        {


            nCount++;
            uszCurrentLine++;
        }
        else if (sSkipEscapeSequence(uszCurrentLine, sColorSeqStartA) ||
                 sSkipEscapeSequence(uszCurrentLine, sColorSeqStartB))
        {
        }
        else if (strncmp(uszCurrentLine, sBullet.str(), sBullet.Length()) == 0 ||
                 strncmp(uszCurrentLine, sCheck.str(), sCheck.Length()) == 0 ||
                 strncmp(uszCurrentLine, sSetDungeonBullet.str(), sSetDungeonBullet.Length()) == 0 ||
                 strncmp(uszCurrentLine, sSetDungeonRedBullet.str(), sSetDungeonRedBullet.Length()) == 0)
        {



            uszCurrentLine = strchr(uszCurrentLine, '}');


            fBulletFound = TRUE;
            if (uszCurrentLine)
            {
                uszCurrentLine++;
            }
        }
        else
        {
            break;
        }
    }
    if (!fBulletFound)
        nCount = 0;
    return nCount;
}
void UIText::ApplyFormatting()

{
    m_artTextLines.clear();
    if (m_nTextAllocLen == 0)
    {
        return;
    }

    BOOL fWordWrap = (m_uTCFlags >> UITCF_WORDWRAP_BIT) & 1;
    if (!fWordWrap)
    {
        m_tText.SplitIntoLines(m_artTextLines);
        return;
    }



    // NON_MATCHING: 1780 vs 1792 bytes. The two original fallback stores at 820 and 827 merge after the punctuation loop.
    uint32 dwFlags = GetTextParseFlags();
    m_tText = FormattedText(m_uszText, dwFlags, FALSE);
    if ((m_uTCFlags >> UITCF_ISPASSWORD_BIT) & 1)
    {
        m_tText.SetIsPassword(TRUE);
    }

    if (m_tText.Length() != 0)
    {
        BreakStyle eBreakStyle = BREAKSTYLE_LINE;
        if (m_eTextJustify == UIJH_CENTER && UnicodeText::GetStringDisplayLocale() == XLOCALE_KOREAN)
            eBreakStyle = BREAKSTYLE_WORD;




        UTBreakIterator tIterator(eBreakStyle);

        CRefString strStripped((LPCSTR)m_tText);
        UIText_RemoveFormattingCharacters(strStripped, TRUE, IsValidatingHyperlinks());
        tIterator.SetText(strStripped.str());

        int32 nBulletIndentCountForNewLines = 0;
        BOOL fCheckForBullet = TRUE;

        LPCSTR uszCurrentLine = m_uszText;
        while (*uszCurrentLine)
        {

            int32 nBulletIndentsForThisLine = nBulletIndentCountForNewLines;
            if (fCheckForBullet)
            {
                nBulletIndentCountForNewLines = sCountLeadingBulletizedSpace(uszCurrentLine);
                nBulletIndentsForThisLine = 0;
            }



            int32 nLineBytes;
            float flCurrentLineWidthUIC;
            sGetLineOfText(uszCurrentLine, nBulletIndentsForThisLine, nLineBytes, flCurrentLineWidthUIC);

            if (uszCurrentLine[nLineBytes] == '\n')
            {






                uszCurrentLine += nLineBytes + 1;
                fCheckForBullet = TRUE;
            }
            else if (uszCurrentLine[nLineBytes] == ' ')
            {


                int32 nLineStartPos = uszCurrentLine - m_uszText;
                int32 nLineEnd = nLineStartPos + nLineBytes;

                FormattedTextDelta * pNewLineDelta = m_tText.AddCharacterDelta(nLineEnd, '\n', TRUE);
                int32 nInsertionIndex = m_tText.GetDeltaInsertionIndex(pNewLineDelta);
                m_tText.InsertDelta(pNewLineDelta, nInsertionIndex++);
                if (nBulletIndentCountForNewLines)
                {
                    m_tText.InsertDelta(m_tText.AddBulletIndentDelta(nLineEnd, nBulletIndentCountForNewLines), nInsertionIndex);
                }
                uszCurrentLine = m_uszText + nLineEnd + 1;
                fCheckForBullet = FALSE;
            } else if (uszCurrentLine[nLineBytes] != '\0')
            {
                BOOL fCheckLineBreaksForPunctuation = eBreakStyle == BREAKSTYLE_WORD || UnicodeText::GetStringDisplayLocale() == XLOCALE_JAPANESE;

                int32 nLineStartPos = uszCurrentLine - m_uszText;
                int32 nLineEnd = nLineStartPos + nLineBytes; // The original keeps the initial line end for the fallback.
                int32 nLineBreakPos;
                BOOL fFound = FALSE;
                int32 nBreakHintIndexUsed = -1;
                while (m_tText.FindNearestLineBreak(tIterator, strStripped.str(), nLineBreakPos, nLineStartPos, nLineEnd, nBreakHintIndexUsed))
                {


                    if (fCheckLineBreaksForPunctuation && nLineBreakPos > 0)
                    {
                        if (m_uszText[nLineBreakPos - 1] == '(' || m_uszText[nLineBreakPos - 1] == '"')
                        {


                            nLineBreakPos--;
                        }
                    }
                    LPCSTR uszChar = m_uszText + nLineBreakPos;
                    nLineEnd = nLineBreakPos - 1;
                    if (fCheckLineBreaksForPunctuation)
                    {
                        uint32 uChar = Blizzard::Unicode::GetCodepointFromUTF8(&uszChar);
                        switch (uChar)
                        {
                        case '!':
                        case '"':
                        case '\'':
                        case '(':
                        case ')':
                        case ',':
                        case '.':
                        case ':':
                        case ';':
                        case '?':
                            continue;
                        default:
                            break;
                        }
                    }
                    fFound = TRUE;
                    break;
                }
                // An accepted candidate changes fFound from zero to one.




                if (fFound)
                {
                    if (nLineBreakPos == nLineStartPos)
                    {
                        nLineBreakPos += nLineBytes;
                    }
                }
                else
                {


                    nLineBreakPos = nLineStartPos + nLineBytes;
                }

                FormattedTextDelta * pNewLineDelta = m_tText.AddCharacterDelta(nLineBreakPos, '\n', m_uszText[nLineBreakPos] == ' ');
                int32 nInsertionIndex;
                if (nBreakHintIndexUsed != -1)
                {
                    nInsertionIndex = nBreakHintIndexUsed + 1;

                    const FormattedTextDelta * ptHintDelta = m_tText.m_arTextDeltas[nBreakHintIndexUsed];
                    if (ptHintDelta->eType == TEXTDELTA_HYPHEN_HINT)
                    {

                        m_tText.InsertDelta(m_tText.AddCharacterDelta(nLineBreakPos, '-', FALSE), nInsertionIndex++);
                    }
                }
                else
                {

                    nInsertionIndex = m_tText.GetDeltaInsertionIndex(pNewLineDelta);
                }


                m_tText.InsertDelta(pNewLineDelta, nInsertionIndex++);
                if (nBulletIndentCountForNewLines)
                {
                    m_tText.InsertDelta(m_tText.AddBulletIndentDelta(nLineBreakPos, nBulletIndentCountForNewLines), nInsertionIndex);
                }
                uszCurrentLine = m_uszText + nLineBreakPos;
                fCheckForBullet = FALSE;
            }
            else
            {
                break;
            }
        }
        m_tText.SplitIntoLines(m_artTextLines);
    }
}

void UIText::SetSnapToPixelCenter(const BOOL bVal)
{

    m_uTCFlags = bVal ? (m_uTCFlags & ~(1 << UITCF_DISABLE_SNAP_TO_PIXEL_BIT)) : (m_uTCFlags | (1 << UITCF_DISABLE_SNAP_TO_PIXEL_BIT));
}




void UIText::ShrinkToFit()
{

    if ((m_uTCFlags >> UITCF_SHRINK_TO_FIT_BIT) & 1)
    {



        Rect2D rectTextExtentsUIC;
        GetTextRegionRectUIC(rectTextExtentsUIC);
        Vector2D vecRegionExtents(rectTextExtentsUIC.Width(), rectTextExtentsUIC.Height());
        Vector2D vecTextSizeUIC;

        m_nFontSize = m_nFontSizeOriginal;

        const int32 STEPS = 10;
        for (int32 i = 1; i < STEPS; i++)
        {

            GetTextRenderedSizeUIC(vecTextSizeUIC, FALSE);

            if (vecTextSizeUIC.x > vecRegionExtents.x ||
                vecTextSizeUIC.y > vecRegionExtents.y)
            {



                m_nFontSize = FontGetNextSmallestFontSize(GetFont(), m_nFontSize);

                ApplyFormatting();
            }
            else
            {
                break;
            }
        }
    }
}


// NON_MATCHING: 1732 bytes. The empty GetLineCount path follows the reset block in this build; the original places it before.
void UIText::sTruncateIfNecessary()

{


    BOOL fResizeToFit = (m_uTCFlags & ((1 << UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT) | (1 << UITCF_RESIZE_TO_FIT_VERTICAL_BIT))) && m_vecMaxDim == Vector2D(-1.0f, -1.0f);
    BOOL fShrinkToFit = (m_uTCFlags >> UITCF_SHRINK_TO_FIT_BIT) & 1;

    if (!fShrinkToFit && !fResizeToFit && m_nLinesBeforeTruncation > 0 &&


        !IsType(UIT_EDIT) &&
        m_nTextAllocLen > 0)
    {
        int32 nLineCount = GetLineCount();





        // The original resets the truncation state and skips the remaining work when the text fits.
        if (nLineCount < m_nLinesBeforeTruncation ||
            (nLineCount == m_nLinesBeforeTruncation && ((m_uTCFlags >> UITCF_WORDWRAP_BIT) & 1)))
        {
            m_fTextIsTruncated = FALSE;
            m_tTruncatedFinalLine = FormattedText();
            m_tTruncatedText = FormattedText();
            return;
        }

        FormattedText tLastLine = GetLine(m_nLinesBeforeTruncation - 1);

        int32 nBytes;
        float flCurrentLineWidthUIC;
        sGetLineOfText(tLastLine, 0, nBytes, flCurrentLineWidthUIC);

        int32 nLastLineLength = tLastLine.Length();
        nBytes = Math::Min(nBytes, nLastLineLength);

        TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());

        Rect2D rectTextExtentsUIC;
        GetTextRegionRectUIC(rectTextExtentsUIC);
        float flExtentsWidth = rectTextExtentsUIC.Width();

        if (flCurrentLineWidthUIC > flExtentsWidth && tLastLine.m_arTextDeltas.size() > 0)
        {



            FormattedTextDelta * const * iter = blz::find_if(tLastLine.m_arTextDeltas.begin(), tLastLine.m_arTextDeltas.end(), [](const FormattedTextDelta * ptTextDelta) {

                if (ptTextDelta->eType == TEXTDELTA_HYPERLINK)
                {
                    const FormattedHyperlinkDelta * ptHyperlinkDelta = (const FormattedHyperlinkDelta *)ptTextDelta;
                    return ptHyperlinkDelta->tFormattedText.Length() > 0 &&
                           ((LPCSTR)ptHyperlinkDelta->tFormattedText)[0] == '[' &&
                           ((LPCSTR)ptHyperlinkDelta->tFormattedText)[ptHyperlinkDelta->tFormattedText.Length() - 1] == ']';
                }

                return false;
            });


            if (iter != tLastLine.m_arTextDeltas.end())
            {
                FormattedHyperlinkDelta * ptHyperlinkDelta = (FormattedHyperlinkDelta *)*iter;
                CRefString sEllipsisReplacement;
                sEllipsisReplacement.Format("%s]", Blizzard::Unicode::GetUTF8FromCodepoint(cEllipsisChar).c_str());

                Vector2D vecEllipsisSize;
                FontStringGetRenderedSizeUIC(tProps, FormattedText(sEllipsisReplacement.str(), PARSE_DEFAULT, FALSE), vecEllipsisSize);
                float flEllipsisWidth = vecEllipsisSize.x;

                int32 nHyperlinkBytes = ptHyperlinkDelta->tFormattedText.Length();

                LPCSTR szLastLine = ptHyperlinkDelta->tFormattedText;
                while (flEllipsisWidth + flCurrentLineWidthUIC > flExtentsWidth && nHyperlinkBytes > 0)
                {
                    int32 nPrevCharIndex = GetPreviousCharacterUTF8(szLastLine, nHyperlinkBytes);
                    LPCSTR uszText = szLastLine + nPrevCharIndex;
                    uint32 nPrevChar = Blizzard::Unicode::GetCodepointFromUTF8(&uszText);
                    Blizzard::Unicode::GetUTF8FromCodepoint tPrevChar(nPrevChar);
                    Vector2D vecCharSize;
                    FontStringGetRenderedSizeUIC(tProps, FormattedText(tPrevChar, PARSE_DEFAULT, FALSE), vecCharSize);
                    flCurrentLineWidthUIC -= vecCharSize.x;
                    nHyperlinkBytes -= strlen(tPrevChar);
                }
                ptHyperlinkDelta->tFormattedText.Truncate(nHyperlinkBytes, sEllipsisReplacement.str());
                m_fTextIsTruncated = TRUE;
                m_tTruncatedFinalLine = tLastLine;


                m_tTruncatedText = m_tText;
                return;
            }
        }




        if (m_nLinesBeforeTruncation == nLineCount &&
            (nBytes == nLastLineLength ||
             (nBytes == nLastLineLength - 1 && ((LPCSTR)tLastLine)[nBytes] == '\n')))
        {
            m_fTextIsTruncated = FALSE;
            m_tTruncatedFinalLine = FormattedText();
            m_tTruncatedText = FormattedText();
            return;
        }

        CRefString sEllipsisReplacement;
        sEllipsisReplacement.Format(" %s", Blizzard::Unicode::GetUTF8FromCodepoint(cEllipsisChar).c_str());

        Vector2D vecEllipsisSize;
        FontStringGetRenderedSizeUIC(tProps, FormattedText(sEllipsisReplacement.str(), PARSE_DEFAULT, FALSE), vecEllipsisSize);
        float flEllipsisWidth = vecEllipsisSize.x;


        FormattedText tLastLineNew(tLastLine, nBytes);
        Vector2D vecLastLineSize;
        FontStringGetRenderedSizeUIC(tProps, tLastLineNew, vecLastLineSize);
        flCurrentLineWidthUIC = vecLastLineSize.x;

        LPCSTR szLastLine = tLastLine;
        while (flEllipsisWidth + flCurrentLineWidthUIC > flExtentsWidth && nBytes > 0)
        {
            int32 nPrevCharIndex = GetPreviousCharacterUTF8(szLastLine, nBytes);
            LPCSTR uszText = szLastLine + nPrevCharIndex;
            uint32 nPrevChar = Blizzard::Unicode::GetCodepointFromUTF8(&uszText);
            Blizzard::Unicode::GetUTF8FromCodepoint tPrevChar(nPrevChar);
            Vector2D vecCharSize;
            FontStringGetRenderedSizeUIC(tProps, FormattedText(tPrevChar, PARSE_DEFAULT, FALSE), vecCharSize);
            flCurrentLineWidthUIC -= vecCharSize.x;
            nBytes -= strlen(tPrevChar);
        }

        m_fTextIsTruncated = TRUE;
        int32 nPreviousBytes = 0;
        for (int32 i = 0; i < m_nLinesBeforeTruncation - 1; i++)
        {
            nPreviousBytes += GetLine(i).Length();
        }

        m_tTruncatedFinalLine = tLastLine;
        m_tTruncatedText = m_tText;

        if (nBytes > 0 &&
            nBytes == nLastLineLength)
        {

            nBytes = nLastLineLength - 1;
        }

        if (nBytes > 0 &&
            (szLastLine[nBytes - 1] == ' ' || (uint8)szLastLine[nBytes - 1] == (uint8)cNBSP))
        {

            nBytes--;
        }

        m_tTruncatedFinalLine.Truncate(nBytes, NULL);
        m_tTruncatedText.Truncate(nBytes + nPreviousBytes, NULL);

        return;

    }
    m_fTextIsTruncated = FALSE;
    m_tTruncatedFinalLine = FormattedText();
    m_tTruncatedText = FormattedText();
}



void UIText::sGetLineOfText(LPCSTR uszLineStart, const int32 nBulletIndentCount, int32 & nBytes, float & flCurrentLineWidthUIC)
{



    LPCSTR uszCurrentChar = uszLineStart;
    nBytes = 0;
    flCurrentLineWidthUIC = 0.0f;

    Rect2D rectTextExtentsUIC;
    GetTextRegionRectUIC(rectTextExtentsUIC);
    const float WRAP_EPSILON = 0.0001f;
    float flExtentsWidth = rectTextExtentsUIC.Width();


    uint32 dwFlags = GetTextParseFlags();
    FormattedText tFormattedText(uszLineStart, dwFlags, FALSE);
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());


    if (nBulletIndentCount)
    {
        flCurrentLineWidthUIC += FontGetBulletIndentAdvanceUIC(tProps, nBulletIndentCount);
    }


    uint32 nCurrentDeltaIndex = 0;
    const FormattedText::FormattedTextDeltaArray & tDeltas = tFormattedText.m_arTextDeltas;
    for (;;)
    {
        LPCSTR uszNextChar;

        if (nCurrentDeltaIndex < tDeltas.size() &&
            tDeltas[nCurrentDeltaIndex]->nOriginalTextIndex == (uint32)(uszCurrentChar - uszLineStart))
        {
            uszNextChar = uszCurrentChar + tDeltas[nCurrentDeltaIndex]->nOriginalBytesReplaced;
            nCurrentDeltaIndex++;
        }
        else
        {
            uszNextChar = uszCurrentChar;
            uint32 nCodepoint = Blizzard::Unicode::GetCodepointFromUTF8(&uszNextChar);
            if (nCodepoint == 0 || nCodepoint == '\n')
            {
                break;
            }
        }



        Vector2D vecSizeFontUIC;
        FormattedText tFormattedLine(uszCurrentChar, uszNextChar - uszCurrentChar, dwFlags, FALSE);
        FontStringGetRenderedSizeUIC(tProps, tFormattedLine, vecSizeFontUIC);
        if (flCurrentLineWidthUIC + vecSizeFontUIC.x > flExtentsWidth + WRAP_EPSILON && uszCurrentChar != uszLineStart)
        {
            break;
        }
        flCurrentLineWidthUIC += vecSizeFontUIC.x;

        uszCurrentChar = uszNextChar;
    }


    nBytes = uszCurrentChar - uszLineStart;
}



int32 UIText::GetLineCount() const

{
    if (m_tText.Length() == 0)
    {
        return 0;
    }
    int32 nLines = m_artTextLines.size();
    return nLines;
}




inline const FormattedText & UIText::GetLine(const int32 nLine) const
{

    int32 nIndex = Math::Pin(nLine, 0, (int32)m_artTextLines.size() - 1);

    return m_artTextLines[nIndex];
}












































int32 UIText::GetNumVisibleLines() const
{


    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    float flFontHeight = FontGetLineAdvanceUIC(tProps);
    if (flFontHeight <= 0.0f)
    {
        return 0;
    }


    Rect2D rectTextRegionUIC;
    GetTextRegionRectUIC(rectTextRegionUIC);
    float flListHeightUIC = rectTextRegionUIC.Height();

    int32 nVisibleLines = Math::FloatToInt(flListHeightUIC / flFontHeight);
    return Math::Max(nVisibleLines, 1);
}



void UIText::GetTextRenderedSizeUIC(Vector2D & vecTextSizeUIC, BOOL fGetTruncatedTextSize) const
{


    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    FontStringGetRenderedSizeUIC(tProps, (m_fTextIsTruncated && fGetTruncatedTextSize) ? m_tTruncatedText : m_tText, vecTextSizeUIC);
}







































void UIText::ResizeToFitText()
{

    Vector2D vecDimNew = CalculateTextSize();
    SetRelativePosition(m_vecTL, vecDimNew);
}




Vector2D UIText::CalculateTextSize() const
{

    BOOL fHorizontal = (m_uTCFlags >> UITCF_RESIZE_TO_FIT_HORIZONTAL_BIT) & 1;
    BOOL fVertical = (m_uTCFlags >> UITCF_RESIZE_TO_FIT_VERTICAL_BIT) & 1;

    Vector2D vecDim = m_vecDim;
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    if (fVertical)
    {


        Vector2D vecRenderedDim;
        FontStringGetRenderedSizeUIC(tProps, m_tText, vecRenderedDim);
        float flRenderedHeight = vecRenderedDim.y;
        const float flHeightPadding = 4.0f;
        float flGeneralLineHeight = FontGetHeightUIC(tProps, GetLineCount());


        float flFullHeight = Math::Max(flRenderedHeight, flGeneralLineHeight) + flHeightPadding;


        vecDim.y = flFullHeight + (m_vecTextInsetUIC.y * 2.0f + (m_rectTextPaddingUIC.top + m_rectTextPaddingUIC.bottom) + m_vecTextButtonPressOffsetUIC.y * 2.0f);
    }


    BOOL fWordWrap = (m_uTCFlags >> UITCF_WORDWRAP_BIT) & 1;
    if ((fHorizontal || fVertical) && !fWordWrap)
    {
        if (fHorizontal)
        {

            Vector2D vecRenderedDim;
            FontStringGetRenderedSizeUIC(tProps, m_tText, vecRenderedDim);
            vecDim.x = vecRenderedDim.x + (m_vecTextInsetUIC.x * 2.0f + (m_rectTextPaddingUIC.left + m_rectTextPaddingUIC.right) + m_vecTextButtonPressOffsetUIC.x * 2.0f);
        }
        else
        {




            vecDim.x = m_vecDim.x;
        }
    }


    return vecDim;
}



uint32 UIText::GetTextParseFlags() const
{

    BOOL fDoValidation = (m_uTCFlags >> UITCF_VALIDATE_HYPERLINKS_BIT) & 1;
    uint32 dwFlags = fDoValidation ? PARSE_DO_VALIDATION : PARSE_NONE;
    if (!IsType(UIT_CHATEDIT) && !IsType(UIT_EDIT))
    {
        dwFlags |= PARSE_SPECIAL_TAGS;
    }
    if (IsHyperlinkAware())
    {
        dwFlags |= PARSE_HYPERLINKS;
    }
    return dwFlags;
}



int32 UIText::ScreenPtToCursorPos(const Vector2D & vecMouseUIC)
{

    Rect2D rectTextExtentsUIC;
    GetTextRegionRectUIC(rectTextExtentsUIC);

    int32 nStartingLine = m_nTopLine;
    if (nStartingLine > 0 && m_eTextJustifyVertical == UIJV_BOTTOM)
    {
        nStartingLine--;
    }



    int32 nLineCount = m_artTextLines.size();

    Vector2D vecAdjustUIC;
    if ((m_uTCFlags >> UITCF_USE_BUTTON_PRESS_OFFSET_BIT) & 1)
    {
        vecAdjustUIC = m_vecTextButtonPressOffsetUIC;
    }
    else
    {
        vecAdjustUIC.x = 0.0f;
        vecAdjustUIC.y = 0.0f;
    }
    vecAdjustUIC.x += m_flTextScrollHorizontalUIC;
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    ScopedFontDefinitionLockR pdefFont(tProps.snoFont);
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData(tProps.nFontSize);
    float flLineHeightUIC = FontGetLineAdvanceUIC(tProps, ptPointSize);

    float flVerticalOffset = vecAdjustUIC.y + sJustifyVertical(ptPointSize, rectTextExtentsUIC, m_fTextIsTruncated ? m_tTruncatedText : m_tText);

    for (int32 nCurrentLine = nStartingLine; nCurrentLine < nLineCount; nCurrentLine++)
    {
        Vector2D vecLineTextSizeUIC;
        BOOL fOnThisLine = FALSE;
        FontStringGetRenderedSizeUIC(tProps, m_artTextLines[nCurrentLine], vecLineTextSizeUIC);


        float flHorizontalOffset = vecAdjustUIC.x + sJustifyHorizontal(rectTextExtentsUIC, vecLineTextSizeUIC.x);

        Rect2D rectLineExtentsUIC;
        rectLineExtentsUIC.left = rectTextExtentsUIC.left + flHorizontalOffset;
        rectLineExtentsUIC.top = flVerticalOffset + rectTextExtentsUIC.top;
        rectLineExtentsUIC.right = rectLineExtentsUIC.left + vecLineTextSizeUIC.x;
        rectLineExtentsUIC.bottom = flLineHeightUIC + rectLineExtentsUIC.top;






        if (nLineCount == 1)
        {
            fOnThisLine = TRUE;
        }
        else if (vecMouseUIC.y < rectLineExtentsUIC.top)
        {



            return m_tText.ToFormattedIndex(m_artTextLines[nCurrentLine].GetText() - m_tText.GetText());
        }


        else if (vecMouseUIC.y >= rectLineExtentsUIC.top && vecMouseUIC.y < rectLineExtentsUIC.bottom)
        {
            fOnThisLine = TRUE;
        }


        if (fOnThisLine)
        {
            if (vecMouseUIC.x <= rectLineExtentsUIC.left)
            {



                return m_tText.ToFormattedIndex(m_artTextLines[nCurrentLine].GetText() - m_tText.GetText());
            }
            else if (vecMouseUIC.x >= rectLineExtentsUIC.right)
            {


                return m_tText.ToFormattedIndex(m_artTextLines[nCurrentLine].Length() + (uint32)(m_artTextLines[nCurrentLine].GetText() - m_tText.GetText()));
            }
            else
            {

                Vector2D vecMousePixels;
                ConvertUICToPixels(vecMouseUIC, vecMousePixels);
                Vector2D vecLineUIC(rectLineExtentsUIC.left, rectLineExtentsUIC.top);
                Vector2D vecLinePixels;
                ConvertUICToPixels(vecLineUIC, vecLinePixels);


                Vector2D vecPosLineRelative = vecMousePixels - vecLinePixels;

                int32 nPositionWithinLine = FontStringGetPosWithinLine(tProps, m_artTextLines[nCurrentLine], vecPosLineRelative);
                return m_tText.ToFormattedIndex(m_artTextLines[nCurrentLine].GetText() - m_tText.GetText()) + nPositionWithinLine;
            }
        }


        flVerticalOffset += flLineHeightUIC;
    }


    return m_tText.ToFormattedIndex(m_tText.Length());
}



inline float UIText::sJustifyHorizontal(const Rect2D & rectTextExtentsUIC, const float flLineWidthUIC)
{


    switch (m_eTextJustify)
    {
    case UIJH_NONE:
    case UIJH_LEFT:
        return 0.0f;

    case UIJH_CENTER:
        return (rectTextExtentsUIC.Width() - flLineWidthUIC) * 0.5f;

    case UIJH_RIGHT:
        return rectTextExtentsUIC.Width() - flLineWidthUIC;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}


float UIText::sJustifyVertical(FontPointSizeData * ptPointSize, const Rect2D & rectTextExtentsUIC, const FormattedText & tText)
{



    Vector2D vecFullTextSizeUIC;
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());


    switch (m_eTextJustifyVertical)
    {
    case UIJV_NONE:
    case UIJV_LINE_TOP:
    {
        float flAscenderGlyphTopGapUIC;
        float flMaxAscenderGlyphTopGapUIC;
        FontStringGetRenderedSizeUIC(tProps, tText, vecFullTextSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);


        return flMaxAscenderGlyphTopGapUIC - flAscenderGlyphTopGapUIC;
    }

    case UIJV_TOP:
    {
        float flAscenderGlyphTopGapUIC;
        float flMaxAscenderGlyphTopGapUIC;
        FontStringGetRenderedSizeUIC(tProps, tText, vecFullTextSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);


        return -flAscenderGlyphTopGapUIC;
    }

    case UIJV_MIDDLE:
    {
        float flAscenderGlyphTopGapUIC;
        float flMaxAscenderGlyphTopGapUIC;
        FontStringGetRenderedSizeUIC(tProps, tText, vecFullTextSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);
        const float flFullTextHeightUIC = vecFullTextSizeUIC.y;

        float flTopOffset = (rectTextExtentsUIC.Height() - flFullTextHeightUIC) * 0.5f;

        return flTopOffset - flAscenderGlyphTopGapUIC;
    }

    case UIJV_ASCENDER_MIDDLE:
    {






        float flTopOffset;
        float flAscender = FontGetAscenderUIC(tProps, ptPointSize);


        if (m_artTextLines.size() <= 1)
        {
            flTopOffset = (rectTextExtentsUIC.Height() - flAscender) * 0.5f;
        }
        else
        {



            float flAscenderGlyphTopGapUIC;
            float flMaxAscenderGlyphTopGapUIC;
            Vector2D vecLastLineSizeUIC;
            FontStringGetRenderedSizeUIC(tProps, tText, vecFullTextSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);



            FontStringGetRenderedSizeUIC(tProps, m_artTextLines.back(), vecLastLineSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);

            flTopOffset = (rectTextExtentsUIC.Height() - (flAscender + (vecFullTextSizeUIC.y - vecLastLineSizeUIC.y))) * 0.5f;
        }





        return flTopOffset;
    }
    case UIJV_LINE_MIDDLE:
    {
        float flFullHeight = FontGetHeightUIC(tProps, 1);
        float flTopOffset = (rectTextExtentsUIC.Height() - flFullHeight) * 0.5f;
        return flTopOffset;
    }



    case UIJV_BOTTOM:
    {
        float flAscenderGlyphTopGapUIC;
        float flMaxAscenderGlyphTopGapUIC;
        FontStringGetRenderedSizeUIC(tProps, tText, vecFullTextSizeUIC, &flAscenderGlyphTopGapUIC, &flMaxAscenderGlyphTopGapUIC);

        const float flFullTextHeightUIC = vecFullTextSizeUIC.y + 1.0f;

        float flTopOffset = rectTextExtentsUIC.Height() - flFullTextHeightUIC;

        return flTopOffset - flAscenderGlyphTopGapUIC;
    }




    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL);
        ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




























inline void UIText::SetColorStackForLine(FontColorStack & tColorStack, const RGBAColor & rgbaTextCurrent, const FormattedText & tText)
{



    tColorStack.Empty();
    tColorStack.Push(rgbaTextCurrent);


    if ((m_uTCFlags >> UITCF_ENABLED_BIT) & 1)
    {


        uint32 nLineOriginalIndex = (LPCSTR)tText - (LPCSTR)m_tText;
        for (uint32 i = 0; i < m_tText.m_arTextDeltas.size(); i++)
        {
            if (m_tText.m_arTextDeltas[i]->nOriginalTextIndex >= nLineOriginalIndex)
            {
                break;
            }

            switch (m_tText.m_arTextDeltas[i]->eType)
            {
            case TEXTDELTA_FONTCOLOR_PUSH:
            {
                FormattedColorPushDelta * pColorPush = (FormattedColorPushDelta *)m_tText.m_arTextDeltas[i];
                tColorStack.Push(pColorPush->rgbaColor);
                break;
            }
            case TEXTDELTA_FONTCOLOR_POP:
                tColorStack.Pop();
                break;
            }
        }
    }
}

void UIText::DrawText(const uint8 bLayerAlpha, float & flZSlice)
{


    if (sNeedsToRefreshIcons())
    {
        sRefreshIcons();
    }

    FormattedText & tTargetText = m_tText.Length() ? m_tText : m_tPlaceholderText;
    if (tTargetText.Length() == 0)
    {
        return;
    }

    const Rect2D & rectClipping = GetCascadedClippingRect();

    RGBAColor rgbaTextCurrent;
    RGBAColor rgbaDropShadow;
    if (&tTargetText == &m_tPlaceholderText)
    {
        rgbaTextCurrent = m_rgbaPlaceholderText;
        rgbaDropShadow = m_rgbaPlaceholderTextDropShadow;
    }
    else if (IsEnabled())
    {
        rgbaTextCurrent = m_rgbaText;
        rgbaDropShadow = m_rgbaDropShadow;
    }
    else
    {
        rgbaTextCurrent = m_rgbaTextDisabled;
        rgbaDropShadow = m_rgbaDropShadowDisabled;
    }

    uint8 bAlpha = MultiplyAlpha(MultiplyAlpha(bLayerAlpha, rgbaTextCurrent.a), GetWindowColor().a);
    if (bAlpha == 0)
    {
        return;
    }

    rgbaDropShadow.a = MultiplyAlpha(MultiplyAlpha(bLayerAlpha, rgbaDropShadow.a), GetWindowColor().a);

    int32 nStartingLine = m_nTopLine;
    if (nStartingLine > 0 && m_eTextJustifyVertical == UIJV_BOTTOM)
    {
        nStartingLine--;
    }





    const float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;

    Rect2D rectTextExtentsUIC;
    GetTextRegionRectUIC(rectTextExtentsUIC);

    Rect2D rectClippedExtentsUIC;
    Rect2D::Intersect(rectClipping, rectTextExtentsUIC, &rectClippedExtentsUIC);
    if (rectClippedExtentsUIC.IsEmpty())
    {
        return;
    }


    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    tProps.fSnapToPixelCenterX = TRUE;
    tProps.fSnapToPixelCenterY = !((m_uTCFlags >> UITCF_DISABLE_SNAP_TO_PIXEL_BIT) & 1);
    tProps.flStereoDepth = GetStereoDepth();

    ScopedFontDefinitionLockR pdefFont(tProps.snoFont);
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData(tProps.nFontSize);

    float flLineHeightUIC = FontGetLineAdvanceUIC(tProps, ptPointSize);

    flLineHeightUIC *= m_wrapLineScalar;

    Vector2D vecAdjustUIC;
    if ((m_uTCFlags >> UITCF_USE_BUTTON_PRESS_OFFSET_BIT) & 1)
    {
        vecAdjustUIC = m_vecTextButtonPressOffsetUIC;
    }
    else
    {
        vecAdjustUIC.x = 0.0f;
        vecAdjustUIC.y = 0.0f;
    }
    vecAdjustUIC.x += m_flTextScrollHorizontalUIC;
    float flVerticalOffset = vecAdjustUIC.y + sJustifyVertical(ptPointSize, rectTextExtentsUIC, m_fTextIsTruncated ? m_tTruncatedText : tTargetText);
    BOOL fColorNeedsToBeSetForLine = TRUE;
    BOOL fAllowColor = IsEnabled();
    for (int32 nCurrentLine = nStartingLine; nCurrentLine < (int32)m_artTextLines.size(); nCurrentLine++)
    {
        if (m_fTextIsTruncated && nCurrentLine >= m_nLinesBeforeTruncation)
        {
            break;
        }
        const FormattedText * ptCurrentLine;
        if (m_fTextIsTruncated && nCurrentLine == m_nLinesBeforeTruncation - 1)
        {

            ptCurrentLine = &m_tTruncatedFinalLine;
        }
        else if (&tTargetText == &m_tPlaceholderText)
        {
            ptCurrentLine = &m_tPlaceholderText;
        }
        else
        {
            ptCurrentLine = &m_artTextLines[nCurrentLine];
        }

        Vector2D vecLineTextSizeUIC;
        FontStringGetRenderedSizeUIC(tProps, *ptCurrentLine, vecLineTextSizeUIC);


        float flHorizontalOffset = vecAdjustUIC.x + sJustifyHorizontal(rectTextExtentsUIC, vecLineTextSizeUIC.x);

        Rect2D rectLineExtentsUIC;
        rectLineExtentsUIC.left = flHorizontalOffset + rectTextExtentsUIC.left;
        rectLineExtentsUIC.top = flVerticalOffset + rectTextExtentsUIC.top;
        rectLineExtentsUIC.right = rectLineExtentsUIC.left + vecLineTextSizeUIC.x;
        rectLineExtentsUIC.bottom = rectLineExtentsUIC.top + vecLineTextSizeUIC.y;

        if (Rect2D::Intersect(rectLineExtentsUIC, rectClippedExtentsUIC, NULL))
        {
            if (fColorNeedsToBeSetForLine)
            {

                SetColorStackForLine(cgtColorStack, rgbaTextCurrent, *ptCurrentLine);
            }



            FontSubstringDrawUIC(tProps, ptPointSize, pdefFont, *ptCurrentLine,




                                 Vector2D(rectLineExtentsUIC.left, rectLineExtentsUIC.top), bAlpha, &rgbaDropShadow,


                                 IsUsingDropShadow(), flZ, fAllowColor, &rectClippedExtentsUIC,



                                 m_eRenderLayerOverride);

            fColorNeedsToBeSetForLine = FALSE;
        }
        else
        {
            fColorNeedsToBeSetForLine = TRUE;
        }
        flVerticalOffset += flLineHeightUIC;
    }


    cgtColorStack.Empty();
}




void UIText::SetTopLine(const int32 nLine)
{


    if (m_nTopLine == nLine)
    {
        return;
    }

    m_nTopLine = nLine;
}





void UIText::SetScrollOffsetUIC(const UITextOffset eOffset, const float flOffsetAmount)
{


    switch (eOffset)
    {
    case UITEXT_OFFSET_HORIZONTAL:
        m_flTextScrollHorizontalUIC = flOffsetAmount;
        break;
    case UITEXT_OFFSET_VERTICAL:
        m_flTextScrollVerticalUIC = flOffsetAmount;
        break;
    }
}





float UIText::GetScrollOffsetUIC(const UITextOffset eOffset) const
{

    switch (eOffset)
    {
    case UITEXT_OFFSET_HORIZONTAL:
        return m_flTextScrollHorizontalUIC;

    case UITEXT_OFFSET_VERTICAL:
        return m_flTextScrollVerticalUIC;
    }

    return 0.0f;
}



void UIText::Draw(const uint8 bLayerAlpha, float & flZSlice)
{









    if (GetStereoDepth() == 1.0f)
    {
        UIWindow::Draw(bLayerAlpha, flZSlice);
    }

    if ((m_uTCFlags >> UITCF_SELECTABLE_BIT) & 1)
    {
        const Rect2D & rectClippingUIC = GetCascadedClippingRect();


        Rect2D rectTextRegionUIC;
        GetTextRegionRectUIC(rectTextRegionUIC);

        DrawSelection(bLayerAlpha, flZSlice, rectClippingUIC, rectTextRegionUIC, NULL);
    }

    DrawText(bLayerAlpha, flZSlice);
}




void UIText::DrawSelection(const uint8 bLayerAlpha, float & flZSlice, const Rect2D & rectDrawClippingUIC, const Rect2D & rectTextExtentsUIC, Rect2D * pRectSelectionUIC)
{






    if (pRectSelectionUIC)
    {
        pRectSelectionUIC->Set(0.0f, 0.0f, 0.0f, 0.0f);
    }

    if (!((m_uTCFlags >> UITCF_SELECTABLE_BIT) & 1) ||
        (m_nSelectionBegin == -1 && m_nSelectionEnd == -1) ||
        m_tText.Length() == 0)
    {
        return;
    }
    RGBAColor rgbaSelection(0x55, 0x55, 0x55, 0xff);
    rgbaSelection.a = MultiplyAlpha(MultiplyAlpha(bLayerAlpha, rgbaSelection.a), GetWindowColor().a);
    if (rgbaSelection.a == 0)
    {
        return;
    }

    int32 nStartingLine = m_nTopLine;
    if (nStartingLine > 0 && m_eTextJustifyVertical == UIJV_BOTTOM)
    {
        nStartingLine--;
    }





    Vector2D vecAdjustUIC;
    if ((m_uTCFlags >> UITCF_USE_BUTTON_PRESS_OFFSET_BIT) & 1)
    {
        vecAdjustUIC = m_vecTextButtonPressOffsetUIC;
    }
    else
    {
        vecAdjustUIC.x = 0.0f;
        vecAdjustUIC.y = 0.0f;
    }
    vecAdjustUIC.x += m_flTextScrollHorizontalUIC;
    const float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;
    TextRenderProperties tProps(GetFont(), GetFontSize(), 1.0f, GetAdvanceScalar());
    ScopedFontDefinitionLockR pdefFont(tProps.snoFont);
    FontPointSizeData * ptPointSize = pdefFont->GetPointSizeData(tProps.nFontSize);
    float flLineAdvanceUIC = FontGetLineAdvanceUIC(tProps, ptPointSize);
    float flSelectionHeightUIC = FontGetCursorHeightUIC(tProps);

    Rect2D rectClippedExtentsUIC;
    Rect2D::Intersect(rectDrawClippingUIC, rectTextExtentsUIC, &rectClippedExtentsUIC);
    float flVerticalOffset = vecAdjustUIC.y;

    if (m_eTextJustifyVertical == UIJV_MIDDLE ||
        m_eTextJustifyVertical == UIJV_LINE_MIDDLE ||
        m_eTextJustifyVertical == UIJV_ASCENDER_MIDDLE)
    {
        flVerticalOffset += (rectTextExtentsUIC.Height() - flSelectionHeightUIC) * 0.5f;
    }

    for (int32 nCurrentLine = nStartingLine; nCurrentLine < (int32)m_artTextLines.size(); nCurrentLine++)
    {
        const FormattedText & tCurrentLine = m_artTextLines[nCurrentLine];
        int32 nFormattedIndexStart = m_tText.ToFormattedIndex(tCurrentLine.GetText() - m_tText.GetText());
        int32 nFormattedIndexEnd = nFormattedIndexStart + tCurrentLine.ToFormattedIndex(tCurrentLine.Length());
        int32 nSelectionSpanBegin = Math::Max(nFormattedIndexStart, m_nSelectionBegin) - nFormattedIndexStart;
        int32 nSelectionSpanEnd = Math::Min(nFormattedIndexEnd, m_nSelectionEnd) - nFormattedIndexStart;

        if (nSelectionSpanEnd > nSelectionSpanBegin)
        {
            Vector2D vecLineTextSizeUIC;
            Vector2D vecSelectionBeginUIC;
            Vector2D vecSelectionEndUIC;
            FontStringGetSelectedRegionUIC(tProps, m_artTextLines[nCurrentLine], nSelectionSpanBegin, nSelectionSpanEnd, vecLineTextSizeUIC, vecSelectionBeginUIC, vecSelectionEndUIC);

            float flHorizontalOffset = vecAdjustUIC.x + sJustifyHorizontal(rectTextExtentsUIC, vecLineTextSizeUIC.x);

            Rect2D rectSelectionUIC;
            Rect2D rectClippedSelectionUIC;
            rectSelectionUIC.left = flHorizontalOffset + (rectTextExtentsUIC.left + vecSelectionBeginUIC.x);
            rectSelectionUIC.right = rectSelectionUIC.left + vecSelectionEndUIC.x - vecSelectionBeginUIC.x;
            rectSelectionUIC.top = flVerticalOffset + rectTextExtentsUIC.top;
            rectSelectionUIC.bottom = flSelectionHeightUIC + rectSelectionUIC.top;

            BOOL bVisible = !rectClippedExtentsUIC.IsEmpty() && Rect2D::Intersect(rectSelectionUIC, rectClippedExtentsUIC, &rectClippedSelectionUIC);
            if (bVisible)
            {
                Rect2D rectSelectionPixels;
                ConvertUICToPixels(rectClippedSelectionUIC, rectSelectionPixels);

                GfxRectDrawDeferred(rectSelectionPixels, rgbaSelection, flZ, m_eRenderLayerOverride, FALSE);


                if (pRectSelectionUIC)
                {
                    *pRectSelectionUIC = rectClippedSelectionUIC;
                }
            }
        }


        flVerticalOffset += flLineAdvanceUIC;
    }
}




int32 UIText::IsInHyperlink(const int32 nPosition, const BOOL bStartInclusive, const BOOL bEndInclusive) const
{



    if (!((m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1))
    {
        return -1;
    }
    const FormattedText::FormattedTextDeltaArray & tarDeltas = m_tText.m_arTextDeltas;
    for (uint32 i = 0; i < tarDeltas.size(); i++)
    {

        if (tarDeltas[i]->eType == TEXTDELTA_HYPERLINK)
        {
            FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)tarDeltas[i];
            BOOL bInStart = bStartInclusive ? (int32)pHyperlinkDelta->nFormattedTextIndex <= nPosition : (int32)pHyperlinkDelta->nFormattedTextIndex < nPosition;
            BOOL bInEnd = bEndInclusive ? (int32)(pHyperlinkDelta->nFormattedTextIndex + pHyperlinkDelta->nFormattedTextLength) >= nPosition : (int32)(pHyperlinkDelta->nFormattedTextIndex + pHyperlinkDelta->nFormattedTextLength) > nPosition;
            if (bInStart && bInEnd)
            {
                return i;
            }
        }
    }






















    return -1;
}






















































inline BOOL UIText::sGetHyperlink(const BufferedMouseData & tBufMouseData, UITextHyperlinkRecord & tRecord, HyperlinkType & eHyperlinkType)
{



    if (((m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1) &&
        m_nSelectionBegin == m_nSelectionEnd &&
        IsInside(tBufMouseData.p))
    {
        return sIsInHyperlink(tBufMouseData.p, &tRecord, &eHyperlinkType);
    }

    return FALSE;
}



BOOL UIText::sIsInHyperlink(const IVector2D & vecMousePosition, UITextHyperlinkRecord * ptRecord, HyperlinkType * peHyperlinkType)
{



    if ((m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1)
    {
        Vector2D vecMouseUIC;
        ConvertPixelsToUIC(vecMousePosition, vecMouseUIC);

        int32 nCursorPos = ScreenPtToCursorPos(vecMouseUIC);
        int32 nHyperlinkIndex = IsInHyperlink(nCursorPos, TRUE, FALSE);
        if (nHyperlinkIndex != -1)
        {
            if (ptRecord)
            {

                const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
                *ptRecord = UITextHyperlinkRecord();
                ptRecord->sRawHyperlinkText = CRefString(m_tText.GetText() + pDelta->nOriginalTextIndex, pDelta->nOriginalBytesReplaced);
                ptRecord->uHyperlinkStartChar = pDelta->nOriginalTextIndex;
                ptRecord->uHyperlinkCharacterLength = pDelta->nOriginalBytesReplaced;

                if (peHyperlinkType)
                {
                    const char * pszHyperlink = ptRecord->sRawHyperlinkText;
                    *peHyperlinkType = HyperlinkParseType(pszHyperlink, FALSE);
                }
            }
            return TRUE;
        }
    }

    return FALSE;
}


// NON_MATCHING: 1028 vs 1008 bytes. The original shares the return block for HandleLeftMouseDown and UIWindow::HandleMessage; this build tail-calls the former.
BOOL UIText::HandleMessage(
    const UIMsg & msg,
    const uintptr dwParam)
{
    BOOL fPassToBaseClass = TRUE;
    switch (msg)
    {
    case UI_INPUT:
    {
        const BufferedInput * ptInput = (const BufferedInput *)dwParam;
        switch (ptInput->eInputMsg)
        {
        case INPUTMSG_KEYDOWN:
        {





            BOOL fCtrlDown = InputCtrlKeyIsDown(INPUTCONTEXT_DEFAULT);
            switch (ptInput->tBufKeyData.eKey)
            {
            case KEY_INSERT:
            case KEY_C:
                if (fCtrlDown)
                {
                    CopySelectionToClipboard();
                    return TRUE;
                }
            }
            fPassToBaseClass = FALSE;
            break;
        }











        case INPUTMSG_MOUSEMOVE:
        {
            if (m_bDragging)
            {



                Vector2D vecMouseUIC;
                ConvertPixelsToUIC(ptInput->tBufMouseData.p, vecMouseUIC);
                int32 nNewCursorPos = ScreenPtToCursorPos(vecMouseUIC);
                SetSelection(m_nDragStartCursorPos, nNewCursorPos);
                return TRUE;
            }
            fPassToBaseClass = FALSE;
            break;
        }
        case INPUTMSG_LEFTMOUSEDOWN:
        case INPUTMSG_LEFTMOUSEDOUBLECLICK:
        {
            BOOL bRetVal = HandleLeftMouseDown(*ptInput);
            return bRetVal;
        }





        case INPUTMSG_RIGHTMOUSEDOWN:
        {
            if ((m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1)
            {
                Vector2D vecMouseUIC;
                ConvertPixelsToUIC(ptInput->tBufMouseData.p, vecMouseUIC);

                UITextHyperlinkRecord tRecord;
                HyperlinkType eHyperlinkType = HLT_NONE;
                if (sGetHyperlink(ptInput->tBufMouseData, tRecord, eHyperlinkType) &&
                    eHyperlinkType == HLT_ONLINE_USER)
                {

                    return TRUE;
                }
            }

            break;
        }


        case INPUTMSG_LEFTMOUSEUP:
        {
            BOOL fHandled = FALSE;


            UITextHyperlinkRecord tRecord;
            HyperlinkType eHyperlinkType = HLT_NONE;
            if (sGetHyperlink(ptInput->tBufMouseData, tRecord, eHyperlinkType))
            {
                switch (eHyperlinkType)
                {
                case HLT_ITEM:
                    gtTooltipMgr.SetHyperlinkTooltip(tRecord);
                    fHandled = TRUE;
                    break;



                case HLT_ACHIEVEMENT:
                    fHandled = TRUE;
                    break;





                case HLT_URL:
                {
                    CRefString sURL;
                    fHandled = CHyperlinkGetURL(tRecord.sRawHyperlinkText, sURL) != FALSE;
                    break;
                }





                case HLT_STORE_PRODUCT:
                {
                    OnlineService::StoreItem tItem;
                    fHandled = CHyperlinkGetStoreItem(tRecord.sRawHyperlinkText, tItem) != FALSE;
                    break;
                }













































                default:
                    break;
                }
            }



            if (m_bDragging)
            {
                m_bDragging = FALSE;
                UI_SetCapture(GetUIIDNone());
                fHandled = TRUE;
            }

            return fHandled;
        }



        case INPUTMSG_RIGHTMOUSEUP:
        {
            BOOL fHandled = FALSE;


            UITextHyperlinkRecord tRecord;
            HyperlinkType eHyperlinkType = HLT_NONE;
            if (sGetHyperlink(ptInput->tBufMouseData, tRecord, eHyperlinkType))
            {
            }






            return fHandled;
        }
        }
        break;
    }


    case UI_GFX_INIT:

        sApplyFormattingForCurrentDimensions();
        break;
    }


    if (!fPassToBaseClass)
    {
        return FALSE;
    }



    return UIWindow::HandleMessage(msg, dwParam);
}



LPCSTR UIText::GetTooltipText()
{
    LPCSTR szTooltipText = UIWindow::GetTooltipText();
    if (szTooltipText && *szTooltipText)
    {
        return szTooltipText;
    }


    if (m_fTextIsTruncated && !((m_uTCFlags >> UITCF_DISABLE_TRUNCATION_TOOLTIP_BIT) & 1))
    {
        return m_tText;
    }

    return NULL;
}



void UIText::SetRelativePosition(const Vector2D & vecTL, const Vector2D & vecDim)
{


    Vector2D vecCurrentDim = m_vecDim;
    UIWindow::SetRelativePosition(vecTL, vecDim);

    if (Math::Abs(vecCurrentDim.x - m_vecDim.x) > EPSILON ||
        Math::Abs(vecCurrentDim.y - m_vecDim.y) > EPSILON)
    {
        sApplyFormattingForCurrentDimensions();
    }
}




CRefString UIText_GetTimeString(const uint32 uSeconds)
{

    const uint32 uSecPerYear = 31536000;
    const uint32 uSecPerDay = 86400;
    const uint32 uSecPerHour = 3600;
    const uint32 uSecPerMin = 60;

    uint32 uSec = uSeconds % uSecPerMin;
    uint32 uMin = (uSeconds % uSecPerHour) / uSecPerMin;
    uint32 uHrs = (uSeconds % uSecPerDay) / uSecPerHour;
    uint32 uDays = (uSeconds % uSecPerYear) / uSecPerDay;

    CRefString sDayLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralDays");
    CRefString sHourLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralHours");
    CRefString sMinuteLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralMinutes");
    CRefString sSecondLabel = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralSeconds");
    CRefString sResultTwoStrings = StringListFetch(GlobalSNOGet(GLOBALSNO_STRINGLIST_BNET_STRINGS), "GeneralTimeTwoStrings");

    CRefString sDay;
    CRefString sHour;
    CRefString sMinute;
    CRefString sSecond;
    CRefString sResult;





















    if (uHrs > 0 || uDays > 0)
    {
        sHour = LocalizedSprintf(sHourLabel.str(),
                                 UnicodeText::FormatNumber((int32)(uDays * 24 + uHrs), TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                 NULL);

        sMinute = LocalizedSprintf(sMinuteLabel.str(),
                                   UnicodeText::FormatNumber((int32)uMin, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   NULL);

        sResult = LocalizedSprintf(sResultTwoStrings.str(),
                                   sHour.str(),
                                   sMinute.str(),
                                   NULL);
        return sResult;
    }



    else if (uMin >= 10)
    {
        sMinute = LocalizedSprintf(sMinuteLabel.str(),
                                   UnicodeText::FormatNumber((int32)uMin, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   NULL);

        return sMinute;
    }


    else if (uMin > 0)
    {
        sMinute = LocalizedSprintf(sMinuteLabel.str(),
                                   UnicodeText::FormatNumber((int32)uMin, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   NULL);

        sSecond = LocalizedSprintf(sSecondLabel.str(),
                                   UnicodeText::FormatNumber((int32)uSec, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   NULL);

        sResult = LocalizedSprintf(sResultTwoStrings,
                                   sMinute.str(),
                                   sSecond.str(),
                                   NULL);
        return sResult;
    }
    else
    {

        sSecond = LocalizedSprintf(sSecondLabel.str(),
                                   UnicodeText::FormatNumber((int32)uSec, TRUE, FNS_NORMAL, XLOCALE_INVALID).str(),
                                   NULL);


        return sSecond;
    }
}


uint32 UIText_RemoveFormattingCharacters(LPSTR puszString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks)
{


    LPCSTR puszInput = puszString;
    LPSTR puszOutput = puszString;




    uint32 dwFlags = fValidateHyperlinks ? (PARSE_ALL | PARSE_DO_VALIDATION) : PARSE_ALL;
    uint32 dwNewSize = 0;


    FormattedText tTemp(puszString, dwFlags, FALSE);
    const FormattedText::FormattedTextDeltaArray & arDeltas = tTemp.m_arTextDeltas;
    uint32 nCurrentDelta = 0;
    while (*puszInput)
    {
        if (nCurrentDelta < arDeltas.size() &&
            arDeltas[nCurrentDelta]->nOriginalTextIndex == (uint32)(puszInput - puszString))
        {
            FormattedTextDelta * pCurrentDelta = arDeltas[nCurrentDelta];
            int32 nBytesToAdvance;
            if (!fPreserveHyperlinks && pCurrentDelta->eType == TEXTDELTA_HYPERLINK)
            {
                FormattedHyperlinkDelta * pHyperlinkDelta = (FormattedHyperlinkDelta *)pCurrentDelta;
                CRefString sDeltaInput = pHyperlinkDelta->tFormattedText;
                UIText_RemoveFormattingCharacters(sDeltaInput, FALSE, TRUE);
                for (size_t i = 0; i < sDeltaInput.Length(); i++)
                {
                    *puszOutput++ = ((LPCSTR)sDeltaInput)[i];
                    dwNewSize++;
                }
            }
            else if (pCurrentDelta->eType == TEXTDELTA_CHARACTER)
            {
                FormattedCharacterDelta * ptCharacterDelta = (FormattedCharacterDelta *)pCurrentDelta;
                Blizzard::Unicode::GetUTF8FromCodepoint tUtf8(ptCharacterDelta->uChar);
                LPCSTR uszChar = tUtf8;
                while (uszChar)
                {
                    *puszOutput++ = *uszChar++;
                }
            }

            pCurrentDelta = arDeltas[nCurrentDelta++];
            if (!fPreserveHyperlinks || pCurrentDelta->eType != TEXTDELTA_HYPERLINK)
            {
                nBytesToAdvance = pCurrentDelta->nOriginalBytesReplaced;
                puszInput += nBytesToAdvance;
                continue;
            }
        }

        *puszOutput++ = *puszInput++;
        dwNewSize++;
    }

    *puszOutput = '\0';
    return dwNewSize;
}




void UIText_RemoveFormattingCharacters(CRefString & usString, BOOL fPreserveHyperlinks, BOOL fValidateHyperlinks)
{




    if (usString.Length() < 1024)
    {
        CHAR uszTemp[1024];
        Blizzard::String::Copy(uszTemp, usString, 1024);
        UIText_RemoveFormattingCharacters(uszTemp, fPreserveHyperlinks, fValidateHyperlinks);
        usString = uszTemp;
    }
    else
    {
        CHAR * uszTemp = new CHAR[usString.Length() + 1];
        strcpy(uszTemp, usString);
        UIText_RemoveFormattingCharacters(uszTemp, fPreserveHyperlinks, fValidateHyperlinks);
        usString = uszTemp;
        delete[] uszTemp;
    }
}




void UIText::ClearSelection()
{

    m_nSelectionBegin = -1;
    m_nSelectionEnd = -1;
}




void UIText::SetSelection(const int32 nSelectionBegin, const int32 nSelectionEnd)
{


    int32 nFormattedLength = m_tText.ToFormattedIndex(m_tText.Length());
    int32 nBegin = Math::Min(nSelectionBegin, nSelectionEnd);
    int32 nEnd = Math::Max(nSelectionBegin, nSelectionEnd);

    nBegin = Math::Pin(nBegin, 0, nFormattedLength);
    nEnd = Math::Pin(nEnd, 0, nFormattedLength);

    if (nBegin == nEnd)
    {
        nBegin = -1;
        nEnd = -1;
    }

    m_nSelectionBegin = nBegin;
    m_nSelectionEnd = nEnd;




    if (((m_uTCFlags >> UITCF_HYPERLINKAWARE_BIT) & 1) && nBegin != nEnd)
    {
        int32 nHyperlinkIndex = IsInHyperlink(m_nSelectionBegin, TRUE, FALSE);
        if (nHyperlinkIndex != -1)
        {

            const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
            m_nSelectionBegin = pDelta->nFormattedTextIndex;
        }

        nHyperlinkIndex = IsInHyperlink(m_nSelectionEnd, FALSE, TRUE);
        if (nHyperlinkIndex != -1)
        {

            const FormattedTextDelta * pDelta = m_tText.m_arTextDeltas[nHyperlinkIndex];
            m_nSelectionEnd = pDelta->nFormattedTextIndex + pDelta->nFormattedTextLength;
        }
    }
}














BOOL UIText::HasSelection() const
{

    return m_nSelectionBegin != -1 && m_nSelectionEnd != -1;
}




void UIText::GetSelection(int32 & nSelectionBegin, int32 & nSelectionEnd) const
{


    nSelectionBegin = m_nSelectionBegin;
    nSelectionEnd = m_nSelectionEnd;
}




inline void UIText::GetSelectedText(LPSTR lpuszRetrievedText) const
{

    int32 nBegin;
    int32 nConvertedBegin;
    int32 nConvertedEnd;
    GetSelection(nBegin, nConvertedEnd);
    if (nBegin == -1)
    {
        lpuszRetrievedText[0] = '\0';
        return;
    }

    ConvertFormattedSelectionToOriginalTextSelection(nConvertedBegin, nConvertedEnd);





    if ((m_uTCFlags >> UITCF_SELECTS_RAW_TEXT_BIT) & 1)
    {
        Blizzard::String::Copy(lpuszRetrievedText, &m_uszText[nConvertedBegin], nConvertedEnd - nConvertedBegin + 1);
    }
    else
    {
        Blizzard::String::Copy(lpuszRetrievedText, &m_uszText[nConvertedBegin], nConvertedEnd - nConvertedBegin + 1);
        UIText_RemoveFormattingCharacters(lpuszRetrievedText, FALSE, TRUE);
    }
}















































BOOL UIText::HandleLeftMouseDown(const BufferedInput & tInput)
{

    if (!(m_uTCFlags & ((1 << UITCF_HYPERLINKAWARE_BIT) | (1 << UITCF_SELECTABLE_BIT))))
    {
        return FALSE;
    }

    const UIID & id = GetID();
    UIID idTest = UI_GetMousedOverControl();
    if (idTest == GetUIIDNone() ||
        idTest != id)
    {
        return FALSE;
    }


    Vector2D vecMouseUIC;
    ConvertPixelsToUIC(tInput.tBufMouseData.p, vecMouseUIC);
    if (IsHyperlinkAware())
    {

        UITextHyperlinkRecord tRecord;
        HyperlinkType eHyperlinkType = HLT_NONE;
        if (sGetHyperlink(tInput.tBufMouseData, tRecord, eHyperlinkType))
        {



            return TRUE;
        }
    }
    if (IsSelectable())
    {
        int32 nNewCursorPos = ScreenPtToCursorPos(vecMouseUIC);
        if (nNewCursorPos != -1)
        {
            ClearSelection();

            UI_SetCapture(id);
            m_bDragging = TRUE;
            m_nDragStartCursorPos = nNewCursorPos;
        }

        return TRUE;
    }

    return FALSE;
}



int32 UIText::TextPosToOriginalTextPos(const int32 nPos) const
{

    return m_tText.ToOriginalIndex(nPos);
}




int32 UIText::OriginalTextPosToTextPos(const int32 nPos) const
{

    return m_tText.ToFormattedIndex(nPos);
}




void UIText::CopySelectionToClipboard()
{

    BOOL fIsPassword = (m_uTCFlags >> UITCF_ISPASSWORD_BIT) & 1;
    if (m_nTextAllocLen <= MAX_UI_TEXT_LEN)
    {
        CHAR uzText[MAX_UI_TEXT_LEN];
        SigmaMemoryClear(uzText, sizeof(uzText));
        if (fIsPassword)
        {

            if (m_tText.Length())
            {
                SigmaMemoryFill(uzText, cguPasswordCharacter, m_tText.Length());
            }
        }
        else
        {
            GetSelectedText(uzText);
        }
        InputPlatformSetClipboard(uzText);
    }
    else
    {
        CHAR * uzText = (CHAR *)SigmaMemoryNew(m_nTextAllocLen, 4, NULL, FALSE);
        SigmaMemoryClear(uzText, m_nTextAllocLen);
        if (fIsPassword)
        {

            if (m_tText.Length())
            {
                SigmaMemoryFill(uzText, cguPasswordCharacter, m_tText.Length());
            }
        }
        else
        {
            GetSelectedText(uzText);
        }
        InputPlatformSetClipboard(uzText);
        SigmaMemoryFree(uzText, NULL);
    }
}





void UIText::ConvertFormattedSelectionToOriginalTextSelection(int32 & nConvertedSelectionBegin, int32 & nConvertedSelectionEnd) const
{


    nConvertedSelectionBegin = TextPosToOriginalTextPos(m_nSelectionBegin);
    nConvertedSelectionEnd = TextPosToOriginalTextPos(m_nSelectionEnd);
}

























































































































































inline BOOL UIText::sNeedsToRefreshIcons()
{


    if (m_tText.GetContainsIcons())
    {



        int32 nControllerIndex = UI_GetPrimaryUIController();
        if (m_nControllerIndexOverride != -1)
        {
            nControllerIndex = m_nControllerIndexOverride;
        }

        const int32 nControllerType = InputGetControllerType(nControllerIndex);
        if (nControllerType != m_nControllerType)
        {
            m_nControllerType = nControllerType;
            return TRUE;
        }
    }

    return FALSE;
}



void UIText::sRefreshIcons()
{


    SetNextControllerTypeForIconAdjustment(m_nControllerType);
    uint32 dwFlags = GetTextParseFlags();
    m_tText = FormattedText(m_tText, dwFlags, FALSE);

    if ((m_uTCFlags >> UITCF_ISPASSWORD_BIT) & 1)
    {
        m_tText.SetIsPassword(TRUE);
    }

    sApplyFormattingForCurrentDimensions();

    SetText_Internal(m_tText, FALSE);
    SetNextControllerTypeForIconAdjustment(-1);
}
