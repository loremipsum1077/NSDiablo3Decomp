// Original file: X/Client/UserInterface/Controls/UIColorPickerControl.cpp
#include "../../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIColorPickerControl.h"
#include "../UI.h"
#include "../UIID.h"
#include "../UIAttributes.h"
#include "../UIUtils.h"
#include "../../Sprite.h"
#include "../../Input/Input.h"
#include "../../Definitions/TextureDefinition.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RGBAColor.h"
#include "../../../Sigma/GfxColorFormats.h"
#include "../../../Sigma/Memory/SigmaMemory.h"
#include "../../../Sigma/Math/MathGeneral.h"
// Declared here because their headers are not adopted yet (UI.cpp, TextureDefinition.cpp).
UIID UIIDCreateFromControlAttribute(const UIControlSourceKey & tSrcKey, const UIID & id, const UIAttributeType eAttributeType);
const UIID & UI_GetCapture(); void UI_SetCapture(const UIID & idCapture); void * TextureDefinitionLockTexture(const SNO sno, uint32 & dwPitch, uint32 & dwHeight, const uint32 dwMipLevel, const uint32 dwFlags); void TextureDefinitionUnlockTexture(const SNO sno, void * lpBuf, const uint32 dwMipLevel);
static Pixel32 * sGenerateSaturationValuePixels(double flHue, uint32 nWidth, uint32 nHeight, TextureFormat eTextureFormat); static RGBAColor sHSVtoRGB(double flHue, double flSaturation, double flValue); static void sBlitPixel32(Pixel32 * pDst, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwPitch, const Pixel32 * pSrc);

ColorPickerCreationParams::ColorPickerCreationParams(const UIType eType, const UIID & id, const UIID & idParent)
    : WindowCreationParams(eType, id, idParent)
{

    m_pfnOnColorChange = NULL;
}



















void ColorPickerCreationParams::Load(const SNO snoUI, const UIControlDataExt * pControlData)
{

    WindowCreationParams::Load(snoUI, pControlData);
    m_pfnOnColorChange = UIFunction_Lookup(pControlData->GetAttributeValue(ATT_ONCHANGE));
}









































UIColorPicker::UIColorPicker(const ColorPickerCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{

    sInit(tParams);
}




UIColorPicker::~UIColorPicker()
{

    DeleteSaturationValueTexture();
}



inline void UIColorPicker::sInit(const ColorPickerCreationParams & tParams)
{


    m_dSelectedHue = 0.0; m_dSelectedSaturation = 0.0; m_dSelectedValue = 0.0;

    m_snoSaturationValueTexture = SNO_NONE;
    m_bSVTextureDirty = FALSE;

    m_pfnOnColorChange = tParams.m_pfnOnColorChange;



    CreateSaturationValueTexture();
}




















void UIColorPicker::SetSaturationAndValue(double dSaturation, double dValue)
{

    if (m_dSelectedSaturation == dSaturation && m_dSelectedValue == dValue) { return; }

    m_dSelectedSaturation = dSaturation;
    m_dSelectedValue = dValue;
    if (UI_TestID(m_idSelectionCursor))
    {
        const Rect2D rectPicker = GetWindowRect();
        UIWindow & tCursor = UI_GetWindowFromID(m_idSelectionCursor);
        const Rect2D rectCursor = tCursor.GetWindowRect();

        Vector2D vecPos(rectPicker.left - rectPicker.Width() * (float)dSaturation + rectPicker.Width(),
                        rectPicker.top - rectPicker.Height() * (float)dValue + rectPicker.Height());
        vecPos.x -= rectCursor.Width() * 0.5f;
        vecPos.y -= rectCursor.Height() * 0.5f;
        tCursor.MoveTo(vecPos);
    }

    if (m_pfnOnColorChange)
    {
        m_pfnOnColorChange(GetID());
    }
}


























































// LINKER-REMOVED: DWARF concrete DIE at cpp:234 (low_pc 0, 0x178 bytes, `this` only parameter, sHSVtoRGB inlined at 237 with flHue/flSaturation in B0/B1); line rows 237:5/22/61 put the return and the three member arguments at those columns. Inferred: the body is exactly this call. Compiles to 0x178 (the DWARF size) with the generic Math::Abs template. Needed as the second caller of sHSVtoRGB so Draw and CreateSaturationValueTexture inline sGenerateSaturationValuePixels as the original does.
RGBAColor UIColorPicker::GetSelectedColor() const
{

    return sHSVtoRGB(m_dSelectedHue, m_dSelectedSaturation, m_dSelectedValue);
}



void UIColorPicker::PostCreate()
{


    m_idSelectionCursor = UIIDCreateFromControlAttribute(GetUISrcKey(), GetID(), ATT_ID_GENERIC_01);
}



void UIColorPicker::Draw(const uint8 bLayerAlpha, float & flZSlice)
{


    RGBAColor rgbaWindowColor = GetWindowColor();
    const uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, rgbaWindowColor.a);
    if (!bAlphaCascaded) { return; }




    const float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;
    rgbaWindowColor.a = bAlphaCascaded;

    const Rect2D & rectClippingUIC = GetCascadedClippingRect(); Rect2D rectClippingPixels;


    ConvertUICToPixels(rectClippingUIC, rectClippingPixels);


    Rect2D rectWindowUIC; GetWindowRect(rectWindowUIC); Rect2D rectWindowPixels;
    ConvertUICToPixels(rectWindowUIC, rectWindowPixels);


    SNO snoShaderToUse = SNO_NONE;
    if (bAlphaCascaded != 0xff)
    {
        snoShaderToUse = GlobalSNOGet(GLOBALSNO_SHADER_SPRITE_TRANSPARENT);
    }

    if (m_snoSaturationValueTexture != SNO_NONE)
    {
        if (m_bSVTextureDirty)
        {







            uint32 dwPitch, dwHeight;
            Pixel32 * ptSVTexturePixels = (Pixel32 *)TextureDefinitionLockTexture(m_snoSaturationValueTexture, dwPitch, dwHeight, 0, 0);
            Pixel32 * ptNewColorSwatch = sGenerateSaturationValuePixels(m_dSelectedHue, (uint32)rectWindowPixels.Width(), dwHeight, TEXTUREFORMAT_ARGB8888);
            sBlitPixel32(ptSVTexturePixels, (uint32)rectWindowPixels.Width(), dwHeight, dwPitch, ptNewColorSwatch);
            TextureDefinitionUnlockTexture(m_snoSaturationValueTexture, ptSVTexturePixels, 0);
            SigmaMemoryFree(ptNewColorSwatch, NULL);
            m_bSVTextureDirty = FALSE;
        }
        ImageTextureFrame tSVFrame(m_snoSaturationValueTexture, -1);
        ImageTextureFrame tNullMask;
        SpriteDrawDeferred(tSVFrame, tNullMask, snoShaderToUse, rectWindowPixels, rgbaWindowColor, crgbaBlack, flZ,
                           GetStereoDepth(), &rectClippingPixels, m_eRenderLayerOverride, FALSE);
    }
}



BOOL UIColorPicker::HandleMessage(const UIMsg & msg, const uintptr dwParam)
{



    switch (msg)
    {
    case UI_INPUT:
        {
            BOOL bRetval = sInput(*(const BufferedInput *)dwParam);
            if (bRetval)
            {
                return bRetval;
            }
        }
        break;
    case UI_GFX_INIT:

        CreateSaturationValueTexture();
        break;
    case UI_GFX_DISPOSE:

        DeleteSaturationValueTexture();
        break;
    }



    return UIWindow::HandleMessage(msg, dwParam);
}




BOOL UIColorPicker::sInput(const BufferedInput & tInput)
{

    switch (tInput.eInputMsg)
    {
    case INPUTMSG_MOUSEMOVE:


        if (UI_GetCapture() != GetID()) { break; }





        SampleColorAtPoint(tInput.tBufMouseData.p);
        break;
    case INPUTMSG_LEFTMOUSEDOWN:
    case INPUTMSG_LEFTMOUSEDOUBLECLICK:


        if (IsInside(tInput.tBufMouseData.p)) {
            SampleColorAtPoint(tInput.tBufMouseData.p);

            UI_SetCapture(GetID());
            return TRUE;
        }
        break;
    case INPUTMSG_LEFTMOUSEUP:


        if (UI_GetCapture() == GetID())
        {
            UI_SetCapture(GetUIIDNone());
        }
        break;
    }


    return FALSE;
}

























static Pixel32 * sGenerateSaturationValuePixels(
    double flHue,
    uint32 nWidth,
    uint32 nHeight,
    TextureFormat eTextureFormat)
{
    uint32 nNumPixels = nWidth * nHeight;
    Pixel32 * ptColorSwatch = (Pixel32 *)SigmaMemoryNew(TextureFormatSize(eTextureFormat) * nNumPixels, 4, NULL, TRUE);
    for (uint32 i = 0; i < nNumPixels; ++i)
    {
        uint32 nRow = i / nWidth;

        double flSaturation = 1.0 - (double)(i % nWidth) / (double)nWidth;
        double flValue = 1.0 - (double)nRow / (double)nHeight;
        RGBAColor rgbaPixelColor = sHSVtoRGB(flHue, flSaturation, flValue);
        ptColorSwatch[i] = rgbaPixelColor.ToTexturePixel(eTextureFormat);
    }

    return ptColorSwatch;
}



static RGBAColor sHSVtoRGB(
    double flHue,
    double flSaturation,
    double flValue)
{
    double flChroma = flValue * flSaturation;
    double flHuePrime = flHue / (PI / 3.0f);
    double flColorComponent = flChroma * (1 - Math::Abs(fmod(flHuePrime, 2) - 1.0));

    double flR = 0.0;
    double flG = 0.0;
    double flB = 0.0;

    if (flHuePrime >= 0.0 && flHuePrime < 1.0)
    {
        flR = flChroma;
        flG = flColorComponent;
    }
    else if (flHuePrime >= 1.0 && flHuePrime < 2.0)
    {
        flR = flColorComponent;
        flG = flChroma;
    }
    else if (flHuePrime >= 2.0 && flHuePrime < 3.0)
    {
        flG = flChroma;
        flB = flColorComponent;
    }
    else if (flHuePrime >= 3.0 && flHuePrime < 4.0)
    {
        flG = flColorComponent;
        flB = flChroma;
    }
    else if (flHuePrime >= 4.0 && flHuePrime < 5.0)
    {
        flR = flColorComponent;
        flB = flChroma;
    }
    else if (flHuePrime >= 5.0 && flHuePrime <= 6.0)
    {
        flR = flChroma;
        flB = flColorComponent;
    }

    double flM = flValue - flChroma;
    return RGBAColor((uint8)((flR + flM) * 255 + 0.01f), (uint8)((flG + flM) * 255 + 0.01f), (uint8)((flB + flM) * 255 + 0.01f));
}



static void sBlitPixel32(Pixel32 * pDst, const uint32 dwWidth, const uint32 dwHeight, const uint32 dwPitch, const Pixel32 * pSrc)
{






    const uint32 mod = (dwPitch >> 2) - dwWidth;

    if (mod == 0)
    {
        SigmaMemoryCopy(pDst, pSrc, dwHeight * dwWidth * sizeof(Pixel32));
    }
    else
    {
        for (uint32 y = 0; y < dwHeight; ++y)
        {
            for (uint32 x = 0; x < dwWidth; ++x)
            {
                *pDst++ = *pSrc++;
            }

            pDst += mod;
        }
    }
}



void UIColorPicker::CreateSaturationValueTexture()
{

    Rect2D rectWindowUIC; GetWindowRect(rectWindowUIC); Rect2D rectWindowPixels;
    ConvertUICToPixels(rectWindowUIC, rectWindowPixels);
    TextureFormat eTextureFormat = TEXTUREFORMAT_ARGB8888;
    const uint32 nSVWidth = (uint32)rectWindowPixels.Width();
    const uint32 nSVHeight = (uint32)rectWindowPixels.Height();
    if (nSVWidth == 0 || nSVHeight == 0) { return; }






    m_snoSaturationValueTexture = TextureDefinitionCreateTexture("ColorPickerSaturationValueTexture", TEXTUREFORMAT_ARGB8888, eTextureFormat, nSVWidth, nSVHeight, RESOURCEUSAGE_STATIC, 0, 1, 0, 0);









    Pixel32 * ptColorSwatch = sGenerateSaturationValuePixels(m_dSelectedHue, nSVWidth, nSVHeight, eTextureFormat);

    uint32 nSVTexPitch = 0; uint32 nSVTexHeight = 0;
    Pixel32 * ptSVTexturePixels = (Pixel32 *)TextureDefinitionLockTexture(m_snoSaturationValueTexture, nSVTexPitch, nSVTexHeight, 0, 0);


    sBlitPixel32(ptSVTexturePixels, nSVWidth, nSVTexHeight, nSVTexPitch, ptColorSwatch);
    TextureDefinitionUnlockTexture(m_snoSaturationValueTexture, ptSVTexturePixels, 0);
    SigmaMemoryFree(ptColorSwatch, NULL);
}



inline void UIColorPicker::DeleteSaturationValueTexture()
{

    if (m_snoSaturationValueTexture != SNO_NONE)
    {
        gptGroupDefTexture->Purge(m_snoSaturationValueTexture, FALSE, FALSE, FALSE, GLOBALCACHE_MAIN);
        m_snoSaturationValueTexture = SNO_NONE;
    }
}



inline BOOL UIColorPicker::SampleColorAtPoint(IVector2D pt)
{


    Rect2D rectWindowUIC; GetWindowRect(rectWindowUIC); Rect2D rectWindowPixels;
    ConvertUICToPixels(rectWindowUIC, rectWindowPixels);
    pt.x = Math::Pin(pt.x, (int32)rectWindowPixels.left, (int32)rectWindowPixels.right);
    pt.y = Math::Pin(pt.y, (int32)rectWindowPixels.top, (int32)rectWindowPixels.bottom);
    const double flSaturation = 1.0 - ((float)pt.x - rectWindowPixels.left) / rectWindowPixels.Width();
    const double flValue = 1.0 - ((float)pt.y - rectWindowPixels.top) / rectWindowPixels.Height();
    SetSaturationAndValue(flSaturation, flValue);
    return TRUE;
}
