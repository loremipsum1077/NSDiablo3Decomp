// X/Client/UserInterface/Controls/UITimerControl.cpp

#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of this file's own sprite pool.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UITimerControl.h"
#include "UIBlinkerControl.h"
#include "../UI.h"
#include "../UICreation.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Gfx/GfxPrimitives.h"
#include "../../Sprite.h"
#include "../../UISpritePool.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Triangle.h"

class TimerUISprite : public UISprite {  // size 0x28
public:
    TimerUISprite() {}  // line 26
    virtual void Draw();  // vtable slot 0
private:
    RGBAColor GetColor(const float flElapsedFrac) const;
public:
    uint32 m_dwEndTime;  // 0x08
    uint32 m_dwStartTime;  // 0x0c
    BOOL m_fReverseColor;  // 0x10
    float m_flZ;  // 0x14
    Rect2D m_wrWindow;  // 0x18
};
static_assert(sizeof(TimerUISprite) == 0x28, "TimerUISprite");

static UISpritePool<TimerUISprite, 128> s_tTimerUISpritePool;  // line 44

inline void TimerCreationParams::sInit()  // line 54
{
    tImageTexFlash = cgtImageTextureFrameNull;
}

TimerCreationParams::TimerCreationParams(const UIType eType, const UIID & id, const UIID & idParent)  // line 63
    : WindowCreationParams(eType, id, idParent)
{
    sInit();
}

TimerCreationParams::TimerCreationParams(const UIType eType, LPCSTR szName, const UIID & idParent)  // line 75
    : WindowCreationParams(eType, szName, idParent)
{
    sInit();
}

void UITimer::sInit(const TimerCreationParams & tParams)  // line 87
{
    m_dwStartTime = 0;
    m_dwEndTime = 0;
    m_fReverseColor = FALSE;

    m_idBlinker = GetUIIDNone();
    if (tParams.tImageTexFlash.snoTexture != SNO_NONE)
    {
        BlinkerCreationParams tBlinkerParams(UIT_BLINKER, "blinker", GetID());
        tBlinkerParams.dwDuration = 0;
        tBlinkerParams.tImageTexBkgrnd = tParams.tImageTexFlash;
        m_idBlinker = CreateControl<UIBlinker>(tBlinkerParams, Vector2D(0.0f, 0.0f), m_vecDim);
    }
}

UITimer::UITimer(const TimerCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)  // line 107
    : UIWindow(tParams, vecTL, vecDim)
{
    sInit(tParams);
}

UITimer::UITimer(const TimerCreationParams & tParams, const Rect2D * pRect)  // line 119
    : UIWindow(tParams, pRect)
{
    sInit(tParams);
}

UITimer::~UITimer()  // line 130
{
}

void UITimer::SetTimer(const uint32 dwStartTime, const uint32 dwEndTime)  // line 136
{
    m_dwStartTime = dwStartTime;
    m_dwEndTime = dwEndTime;
}

void SpriteSubmitTimerControlSprite(const UIID & id, float flZ);

void UITimer::Draw(const uint8 bLayerAlpha, float & flZSlice)  // line 147
{
    UIWindow::Draw(bLayerAlpha, flZSlice);

    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0)
        return;

    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;

    if (m_dwEndTime == m_dwStartTime)
        return;

    uint32 dwTime = CGameGetPredictedServerTime();
    if (dwTime > m_dwEndTime)
    {
        if (m_idBlinker != GetUIIDNone())
        {
            UIBlinker & tBlinker = UI_GetBlinkerFromID(m_idBlinker);
            tBlinker.SetDuration(60);
            tBlinker.SetActive(TRUE);
        }
        m_dwStartTime = m_dwEndTime;
    }
    else
    {
        SpriteSubmitTimerControlSprite(GetID(), flZ);
    }
}

BOOL UITimer::IsFinished()  // line 192
{
    return m_dwEndTime == m_dwStartTime;
}

BOOL UITimer::HandleMessage(const UIMsg & msg, const uintptr dwParam)  // line 205
{
    switch (msg)
    {
    case UI_INPUT:
    case UI_REGISTER_INPUT_LISTENER:
        return FALSE;
    }

    return UIWindow::HandleMessage(msg, dwParam);
}

void SpriteSubmitTimerControlSprite(const UIID & id, float flZ)  // line 229
{
    TimerUISprite * ptSprite = s_tTimerUISpritePool.Get();

    if (ptSprite)
    {
        const UIControl & tControl = UI_GetControlFromID(id);
        const UITimer & tTimer = (const UITimer &)tControl;

        ptSprite->m_flZ = flZ;
        ptSprite->m_dwEndTime = tTimer.m_dwEndTime;
        ptSprite->m_dwStartTime = tTimer.m_dwStartTime;
        ptSprite->m_fReverseColor = tTimer.m_fReverseColor;
        ptSprite->m_wrWindow = tTimer.m_rect;

        SpriteDrawUISprite(ptSprite, flZ, RENDERLAYER_UI);
    }
}



void TimerUISprite::Draw()  // line 255
{
    if (m_dwEndTime == m_dwStartTime)
        return;

    uint32 dwTime = CGameGetPredictedServerTime();
    int32 nElapsedTime = Math::Max<int32>(dwTime - m_dwStartTime, 0);
    int32 nTimeRange = m_dwEndTime - m_dwStartTime;
    float flElapsedFrac = (float)nElapsedTime / (float)nTimeRange;
    float flDeg = flElapsedFrac * 360.0f;

    Rect2D wrWindowScreen;
    ConvertUICToPixels(m_wrWindow, wrWindowScreen);

    Vector2D wpCenter;
    wpCenter.x = (wrWindowScreen.left + wrWindowScreen.right) * 0.5f;
    wpCenter.y = (wrWindowScreen.bottom + wrWindowScreen.top) * 0.5f;

    RGBAColor rgbaColor = GetColor(flElapsedFrac);

    {
        const float DIVSIONS_PER_DEGREE = 1.0f / 3.0f;
        const float DEGREES_PER_DIVISION = 3.0f;

        TriangleList2D * ptTriList = GfxPrimitivesGetSharedTriList2D();
        TriangleList2D & tTriList = *ptTriList;
        tTriList.Clear();

        float flRadius = wrWindowScreen.Height() * 0.5f;

        static BOOL bSinCosComputed = FALSE;
        static float sflSinCos[121][2];
        if (!bSinCosComputed)
        {
            bSinCosComputed = TRUE;
            for (int32 i = 0; i <= 120; ++i)
            {
                float flDivisionDeg = i * DEGREES_PER_DIVISION;
                Angle angDivision = Math::DegToAngle(flDivisionDeg);
                sflSinCos[i][0] = Math::Sin(angDivision);
                sflSinCos[i][1] = Math::Cos(angDivision);
            }
        }

        Vector2D wpPrev;
        {
            int32 nFirstDiv = 0;
            if (!m_fReverseColor)
                nFirstDiv = Math::Pin((int32)(flDeg * DIVSIONS_PER_DEGREE), 0, 120);
            wpPrev.Set(wpCenter.x + flRadius * sflSinCos[nFirstDiv][0], wpCenter.y - flRadius * sflSinCos[nFirstDiv][1]);
        }

        {
            int32 nDiv = Math::Pin((int32)(flDeg * DIVSIONS_PER_DEGREE), 0, 120);
            for (int32 i = 1; i <= 120; ++i)
            {
                if ((m_fReverseColor && nDiv >= i) ||
                    (!m_fReverseColor && nDiv <= i))
                {
                    Vector2D wpCur;
                    wpCur.Set(wpCenter.x + flRadius * sflSinCos[i][0], wpCenter.y - flRadius * sflSinCos[i][1]);

                    Triangle2D tri;
                    tri.Set(wpCenter, wpPrev, wpCur);
                    if (!tTriList.Add(tri, m_flZ, rgbaColor))
                    {
                        tTriList.Draw();
                        tTriList.Clear();
                        tTriList.Add(tri, m_flZ, rgbaColor);
                    }
                    wpPrev = wpCur;
                }
            }
        }

        tTriList.Draw();
    }
}

RGBAColor TimerUISprite::GetColor(const float flElapsedFrac) const  // line 497
{
    const RGBAColor rgbaColorInit = crgbaBlack;
    const RGBAColor rgbaColorEnd = crgbaUIOrange;
    RGBAColor rgbaColor;
    const float cflTransitionFrac = 0.3f;
    float flColorFrac = m_fReverseColor ? 1.0f - flElapsedFrac : flElapsedFrac;

    if (flColorFrac < cflTransitionFrac)
    {
        rgbaColor = rgbaColorInit;
    }
    else
    {
        rgbaColor = Math::Lerp(rgbaColorInit, rgbaColorEnd, (flColorFrac - cflTransitionFrac) / (1.0f - cflTransitionFrac));
    }
    rgbaColor.a = 0xc0;
    return rgbaColor;
}
