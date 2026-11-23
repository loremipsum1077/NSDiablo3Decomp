// X/Client/UserInterface/Controls/UICutsceneControl.cpp

#include "UICutsceneControl.h"

#include "../../Cutscene.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Gfx/GfxCommon.h"
#include "../../Sprite.h"
#include "../UIUtils.h"
#include "../../../Common/Game.h"
#include "../../../Common/GlobalSNO.h"
#include "../../../Sigma/RefString.h"

UICutscene::UICutscene(const WindowCreationParams & tParams, const Rect2D * pRect)  // line 37
    : UIWindow(tParams, pRect)
{
    m_szCutscenePath[0] = '\0';
    m_idCutscene = CUTSCENEID_INVALID;
    m_fLoop = FALSE;
    m_fCutsceneAudioMuted = FALSE;
}

UICutscene::~UICutscene()  // line 51
{
    Stop();
}


#line 58
void UICutscene::Draw(
    const uint8 bLayerAlpha,
    float & flZSlice)
{
    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
	if (bAlphaCascaded == 0)
    {
        return;
    }

    SNO snoTex = CutsceneGetTexture(m_idCutscene);
    if (snoTex == SNO_NONE)
    {
        return;
    }

    if (AppRenderLayerIsHidden(RENDERLAYER_UI))
    {
        return;
    }








    UIWindow::Draw(bLayerAlpha, flZSlice);

    float flLayerZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;


    IVector2D tMovieFrameSize;
    if (CutsceneGetFrameSize(m_idCutscene, tMovieFrameSize))
    {
        Rect2D rectWindow;
        GetWindowRect(rectWindow);

        float flWindowWidth = rectWindow.right - rectWindow.left;
        float flWindowHeight = rectWindow.bottom - rectWindow.top;

        float flWindowLeft = rectWindow.left;
        float flWindowTop = rectWindow.top;

        float flWindowAspectRatio = flWindowWidth / flWindowHeight;
        float flMovieAspectRatio = (float)tMovieFrameSize.x / (float)tMovieFrameSize.y;

        if (flWindowAspectRatio > flMovieAspectRatio)
        {
            float flExtensionValue = flWindowWidth / flMovieAspectRatio - flWindowHeight;
            flWindowTop += flExtensionValue * -0.5f;
            flWindowHeight += flExtensionValue;
        }
        else
        {
            float flExtensionValue = flWindowHeight * flMovieAspectRatio - flWindowWidth;
            flWindowLeft += flExtensionValue * -0.5f;
            flWindowWidth += flExtensionValue;
        }

        Rect2D extendedWindow(flWindowLeft, flWindowTop, flWindowLeft + flWindowWidth, flWindowTop + flWindowHeight);

        Rect2D extendedWindowScreen;
        ConvertUICToPixels(extendedWindow, extendedWindowScreen);

        Rect2D rectWindowScreen;
        ConvertUICToPixels(rectWindow, rectWindowScreen);



        RGBAColor rgbaColor = crgbaWhite;
        rgbaColor.a = bAlphaCascaded;
        SNO snoShader = GlobalSNOGet(GLOBALSNO_SHADER_CUTSCENE);
        ImageTextureFrame tImageTexFrame(snoTex, -1);

        SpriteDrawDeferred(tImageTexFrame, snoShader, extendedWindowScreen, rgbaColor, crgbaBlack, flLayerZ, &rectWindowScreen, GlobalSNOGet(GLOBALSNO_OPAQUE_WHITE_TEXTURE), NULL, TRUE, RENDERLAYER_INVALID, FALSE);
    }
}
#line 93
BOOL UICutscene::HandleMessage(const UIMsg & msg, const uintptr dwParam)  // line 142
{
    switch (msg)
    {
    case UI_UPDATE:
        if (m_fLoop &&
            m_idCutscene != CUTSCENEID_INVALID &&
            !IsPlaying())
        {
            Play(m_szCutscenePath, m_fCutsceneAudioMuted);
        }
        break;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}

void UICutscene::SetActive(const BOOL fActive)  // line 168
{
    if (!fActive)
    {
        Stop();
    }

    UIWindow::SetActive(fActive);
}

void UICutscene::Play(LPCSTR szPath, BOOL bAudioMuted)  // line 182
{
    CutsceneStop(m_idCutscene, FALSE);
    if (m_szCutscenePath != szPath)
    {
        Blizzard::String::Copy(m_szCutscenePath, szPath, sizeof(m_szCutscenePath));
    }

    CutsceneParams tParams(szPath, CutsceneGetSubtitleName(CRefString(szPath), 0));
    m_fCutsceneAudioMuted = tParams.m_bAudioMuted = bAudioMuted;
    tParams.m_fFullscreen = FALSE;

    m_idCutscene = CutscenePlay(tParams);
}

inline BOOL UICutscene::IsPlaying() const  // line 203
{
    return CutsceneIsPlaying(m_idCutscene);
}

void UICutscene::Stop()  // line 212
{
    CutsceneStop(m_idCutscene, FALSE);
    m_idCutscene = CUTSCENEID_INVALID;
}
