// X/Client/UserInterface/Controls/UIRechargeTimerControl.cpp

#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of this file's own sprite pool.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIRechargeTimerControl.h"
#include "../UI.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Gfx/GfxPrimitives.h"
#include "../../Sprite.h"
#include "../../UISpritePool.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Triangle.h"

class RechargeTimerUISprite : public UISprite {  // size 0x28
public:
    RechargeTimerUISprite() {}  // line 24
    virtual void Draw();  // vtable slot 0
    RGBAColor GetColor(uint32, uint32);
    uint32 m_dwEndTime;  // 0x08
    uint32 m_dwStartTime;  // 0x0c
    float m_flZ;  // 0x10
    Rect2D m_wrWindow;  // 0x14
};
static_assert(sizeof(RechargeTimerUISprite) == 0x28, "RechargeTimerUISprite");

static UISpritePool<RechargeTimerUISprite, 128> s_tTimerUISpritePool;  // line 39

UIRechargeTimer::UIRechargeTimer(const TimerCreationParams & tParams, const Vector2D & vecTL, const Vector2D & vecDim)  // line 45
    : UITimer(tParams, vecTL, vecDim)
{
}

UIRechargeTimer::~UIRechargeTimer()  // line 68
{
}

void RechargeTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ);

void UIRechargeTimer::Draw(const uint8 bLayerAlpha, float & flZSlice)  // line 76
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
        m_dwStartTime = m_dwEndTime;
    }

    RechargeTimerSpriteSubmitTimerControlSprite(GetID(), flZ);
}

void RechargeTimerSpriteSubmitTimerControlSprite(const UIID & id, float flZ)  // line 115
{
    RechargeTimerUISprite * ptSprite = s_tTimerUISpritePool.Get();

    if (ptSprite)
    {
        const UIControl & tControl = UI_GetControlFromID(id);
        const UIRechargeTimer & tTimer = (const UIRechargeTimer &)tControl;

        ptSprite->m_flZ = flZ;
        ptSprite->m_dwEndTime = tTimer.m_dwEndTime;
        ptSprite->m_dwStartTime = tTimer.m_dwStartTime;
        ptSprite->m_wrWindow = tTimer.m_rect;

        SpriteDrawUISprite(ptSprite, flZ, RENDERLAYER_UI);
    }
}
























































void RechargeTimerUISprite::Draw()
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

    Vector2D wpCenter(
        (wrWindowScreen.left + wrWindowScreen.right) * 0.5f,
        (wrWindowScreen.bottom + wrWindowScreen.top) * 0.5f);

    Vector2D wpEdgePoints[4];



    float flRadius = wrWindowScreen.Height() * 0.5f;

    for (int32 n = 0; n < 4; ++n)
    {
        Vector2D & wpCurrent = wpEdgePoints[n];

        Angle angDivision = Math::DegToAngle(flDeg);
        wpCurrent.x = wpCenter.x + flRadius * Math::Sin(angDivision); wpCurrent.y = wpCenter.y - flRadius * Math::Cos(angDivision);

        flDeg -= 8.0f;
    }









































    TriangleList2D * ptTriList = GfxPrimitivesGetSharedTriList2D();
    TriangleList2D & tTriList = *ptTriList;
    tTriList.Clear();

    Triangle2D triTrail;

    triTrail.Set(wpCenter, wpEdgePoints[1], wpEdgePoints[0]);
    tTriList.Add(triTrail, m_flZ, crgbaWhite);

    triTrail.Set(wpCenter, wpEdgePoints[2], wpEdgePoints[1]);
    tTriList.Add(triTrail, m_flZ, RGBAColor(0xa2, 0xf5, 0xff, 0xff));

    triTrail.Set(wpCenter, wpEdgePoints[3], wpEdgePoints[2]);
    tTriList.Add(triTrail, m_flZ, RGBAColor(0xa2, 0xf5, 0xff, 0xc0));


    tTriList.Draw();
}
