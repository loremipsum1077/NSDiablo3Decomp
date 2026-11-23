// X/Client/UserInterface/Controls/UIConsoleSkillsTimerControl.cpp

#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of this file's own colours and sprite pool.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIConsoleSkillsTimerControl.h"
#include "../UI.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Gfx/GfxPrimitives.h"
#include "../../Sprite.h"
#include "../../UISpritePool.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Triangle.h"

static const RGBAColor crgbaUITimerMainColor(0xff, 0x7c, 0x25, 0xff);  // line 27
static const RGBAColor crgbaUITimerHighlightColor(0xfc, 0xff, 0x00, 0xff);  // line 28

class ConsoleSkillsTimerUISprite : public UISprite {  // size 0x30
public:
    ConsoleSkillsTimerUISprite() {}  // line 33
    virtual void Draw();  // vtable slot 0
    RGBAColor GetColor(uint32 nTotalSteps, uint32 nCurrentStep);
    uint32 m_dwEndTime;  // 0x08
    uint32 m_dwStartTime;  // 0x0c
    float m_flZ;  // 0x10
    float m_flBrighten;  // 0x14
    Rect2D m_wrWindow;  // 0x18
    Vector2D * m_pLeadingEdgePos;  // 0x28
};
static_assert(sizeof(ConsoleSkillsTimerUISprite) == 0x30, "ConsoleSkillsTimerUISprite");

static UISpritePool<ConsoleSkillsTimerUISprite, 128> s_tTimerUISpritePool;  // line 50

inline void UIConsoleSkillsTimer::Init()  // line 55
{
    m_eFlashState = FLASH_STATE_OFF;
    m_flFlashScale = 0.0f;
    m_dwFlashStartTime = 0;
    m_LeadingEdgePos.x = 0.0f;
    m_LeadingEdgePos.y = 0.0f;
}

UIConsoleSkillsTimer::UIConsoleSkillsTimer(const TimerCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)  // line 67
    : UITimer(tParams, vecTL, vecDim)
{
    Init();
}

UIConsoleSkillsTimer::~UIConsoleSkillsTimer()  // line 90
{
}

void ConsoleTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ, float flBrighten, Vector2D * pLeadingEdgePos);

void UIConsoleSkillsTimer::Draw(const uint8 bLayerAlpha, float & flZSlice)  // line 98
{
    UIWindow::Draw(bLayerAlpha, flZSlice);

    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0)
        return;

    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;

    if (m_dwEndTime == m_dwStartTime && m_eFlashState == FLASH_STATE_OFF)
        return;

    uint32 dwTime = CGameGetPredictedServerTime();
    if (dwTime > m_dwEndTime)
    {
        Flash();
        m_dwStartTime = m_dwEndTime;
    }

    const float FLASH_RAMP_UP_PER_TICK = 0.2f;
    const float FLASH_RAMP_DOWN_PER_TICK = 0.1f;
    const uint32 FLASH_STATE_TICKS_AT_MAX = 20;

    if (!GameIsSoftPaused())
    {
        uint32 dwTimeElapsed = dwTime - m_dwFlashStartTime;
        switch (m_eFlashState)
        {
        case FLASH_STATE_RAMP_UP:
            m_flFlashScale = dwTimeElapsed * FLASH_RAMP_UP_PER_TICK;
            if (m_flFlashScale >= 1.0f)
            {
                m_eFlashState = FLASH_STATE_RAMP_MAX;
                m_flFlashScale = 1.0f;
                m_dwFlashStartTime = dwTime;
            }
            break;

        case FLASH_STATE_RAMP_MAX:
            if (dwTimeElapsed >= FLASH_STATE_TICKS_AT_MAX)
            {
                m_eFlashState = FLASH_STATE_RAMP_DOWN;
                m_dwFlashStartTime = dwTime;
            }
            break;

        case FLASH_STATE_RAMP_DOWN:
            m_flFlashScale = 1.0f - dwTimeElapsed * FLASH_RAMP_DOWN_PER_TICK;
            if (m_flFlashScale <= 0.0f)
            {
                m_eFlashState = FLASH_STATE_OFF;
                m_flFlashScale = 0.0f;
            }
            break;

        default:
            m_flFlashScale = 0.0f;
            break;
        }
    }
    else
    {
        m_flFlashScale = 0.0f;
    }

    ConsoleTimerSpriteSubmitTimerControlSprite(GetID(), flZ, m_flFlashScale, &m_LeadingEdgePos);
}

void UIConsoleSkillsTimer::Flash()  // line 182
{
    if (m_eFlashState == FLASH_STATE_OFF)
    {
        m_dwFlashStartTime = CGameGetPredictedServerTime();
        m_eFlashState = FLASH_STATE_RAMP_UP;
        m_flFlashScale = 0.0f;
    }
}

void ConsoleTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ, float flBrighten, Vector2D * pLeadingEdgePos)  // line 198
{
    ConsoleSkillsTimerUISprite * ptSprite = s_tTimerUISpritePool.Get();

    if (ptSprite)
    {
        const UIControl & tControl = UI_GetControlFromID(id);
        const UIConsoleSkillsTimer & tTimer = (const UIConsoleSkillsTimer &)tControl;

        ptSprite->m_flZ = flZ;
        ptSprite->m_dwEndTime = tTimer.m_dwEndTime;
        ptSprite->m_dwStartTime = tTimer.m_dwStartTime;
        ptSprite->m_wrWindow = tTimer.m_rect;
        ptSprite->m_flBrighten = flBrighten;
        ptSprite->m_pLeadingEdgePos = pLeadingEdgePos;

        SpriteDrawUISprite(ptSprite, flZ, RENDERLAYER_UI);
    }
}

RGBAColor ConsoleSkillsTimerUISprite::GetColor(uint32 nTotalSteps, uint32 nCurrentStep)  // line 228
{
    RGBAColor rgbaColor;
    const int32 nDivisionsToHighlight = 13;
    const RGBAColor rgbaColorInit = crgbaUITimerMainColor;
    const RGBAColor rgbaColorEnd = crgbaUITimerHighlightColor;

    int32 nRemainingSteps = nTotalSteps - nCurrentStep;

    if (m_flBrighten > 0.0f)
    {
        rgbaColor = Math::Lerp(rgbaColorInit, rgbaColorEnd, m_flBrighten);
    }
    else
    {
        if (nRemainingSteps > nDivisionsToHighlight || nRemainingSteps == 0)
            rgbaColor = rgbaColorInit;
        else if (nRemainingSteps > 0)
            rgbaColor = Math::Lerp(rgbaColorInit, rgbaColorEnd, (float)(nDivisionsToHighlight - nRemainingSteps) / (float)nDivisionsToHighlight);
        else
            rgbaColor = rgbaColorEnd;

        rgbaColor.a = 0xc4;
    }

    return rgbaColor;
}

void ConsoleSkillsTimerUISprite::Draw()  // line 272
{
    if (m_dwEndTime == m_dwStartTime && m_flBrighten <= 0.0f)
        return;

    uint32 dwTime = CGameGetPredictedServerTime();
    int32 nElapsedTime = Math::Max<int32>(dwTime - m_dwStartTime, 0);
    int32 nTimeRange = m_dwEndTime - m_dwStartTime;
    float flElapsedFrac = (float)nElapsedTime / (float)nTimeRange;
    float flDeg = flElapsedFrac * 360.0f;

    if (m_flBrighten > 0.0f && m_dwEndTime == m_dwStartTime)
        flDeg = 360.0f;

    Rect2D wrWindowScreen;
    ConvertUICToPixels(m_wrWindow, wrWindowScreen);

    Vector2D wpCenter;
    wpCenter.x = (wrWindowScreen.left + wrWindowScreen.right) * 0.5f;
    wpCenter.y = (wrWindowScreen.bottom + wrWindowScreen.top) * 0.5f;

    const float DIVSIONS_PER_DEGREE = 1.0f / 3.0f;
    const float DEGREES_PER_DIVISION = 3.0f;
    const int32 DIVISIONS = 120;

    TriangleList2D * ptTriList = GfxPrimitivesGetSharedTriList2D();
    TriangleList2D & tTriList = *ptTriList;
    tTriList.Clear();
    float flRadius = wrWindowScreen.Height() * 0.5f;
    float flWidth = flRadius / 5.0f;

    static BOOL bSinCosComputed = FALSE;
    static float sflSinCos[DIVISIONS + 1][2];
    if (!bSinCosComputed)
    {
        bSinCosComputed = TRUE;
        for (int32 i = 0; i <= DIVISIONS; ++i)
        {
            float flDivisionDeg = i * DEGREES_PER_DIVISION;
            Angle angDivision = Math::DegToAngle(flDivisionDeg);
            sflSinCos[i][0] = Math::Sin(angDivision);
            sflSinCos[i][1] = Math::Cos(angDivision);
        }
    }

    Vector2D wpPrev[2];
    wpPrev[0].x = wpCenter.x + flRadius * sflSinCos[0][0]; wpPrev[0].y = wpCenter.y - flRadius * sflSinCos[0][1];
    wpPrev[1].x = wpCenter.x + (flRadius + flWidth) * sflSinCos[0][0]; wpPrev[1].y = wpCenter.y - (flRadius + flWidth) * sflSinCos[0][1];

    int32 nDiv = Math::Pin((int32)(flDeg * DIVSIONS_PER_DEGREE), 0, DIVISIONS);

    int32 i;
    for (i = 1; i <= nDiv && i <= DIVISIONS; ++i)
    {
        float flExtra = (i & 1) ? flWidth : 0.0f;
        Vector2D wpCur;
        wpCur.x = wpCenter.x + (flRadius + flExtra) * sflSinCos[i][0]; wpCur.y = wpCenter.y - (flRadius + flExtra) * sflSinCos[i][1];

        RGBAColor rgbaColor = GetColor(nDiv, i);

        Triangle2D tri;
        tri.Set(wpPrev[0], wpPrev[1], wpCur);
        if (!tTriList.Add(tri, m_flZ, rgbaColor))
        {
            tTriList.Draw();
            tTriList.Clear();
            tTriList.Add(tri, m_flZ, rgbaColor);
        }

        wpPrev[0] = wpPrev[1];
        wpPrev[1] = wpCur;
    }

    int32 nFinalPoint = (i + 2) % DIVISIONS;
    Vector2D wpEnd;
    wpEnd.x = wpCenter.x + (flRadius + flWidth * 0.5f) * sflSinCos[nFinalPoint][0]; wpEnd.y = wpCenter.y - (flRadius + flWidth * 0.5f) * sflSinCos[nFinalPoint][1];

    Triangle2D tri;
    tri.Set(wpPrev[0], wpPrev[1], wpEnd);

    *m_pLeadingEdgePos = wpEnd;

    RGBAColor rgbaColor = GetColor(nDiv, i);

    if (!tTriList.Add(tri, m_flZ, rgbaColor))
    {
        tTriList.Draw();
        tTriList.Clear();
        tTriList.Add(tri, m_flZ, rgbaColor);
    }

    tTriList.Draw();
}
