// X/Client/UserInterface/Controls/UIPieChartControl.cpp

#include "../../../../Contrib/Contrib/blz/include/blz/tuple.h"
// SNOName.h, WorldPlace.h and GfxVB.h only for their statics: the original's static initialiser
// runs them ahead of this file's own sprite pool.
#include "../../../Common/SNO/SNOName.h"
#include "../../../Common/WorldPlace.h"
#include "../../Gfx/GfxVB.h"
#include "UIPieChartControl.h"
#include "../UI.h"
#include "../UIUtils.h"
#include "../../CGame.h"
#include "../../Definitions/UIDialogDefinition.h"
#include "../../Gfx/GfxPrimitives.h"
#include "../../Sprite.h"
#include "../../UISpritePool.h"
#include "../../../Sigma/DataStructures/XArray.h"
#include "../../../Sigma/Math/MathGeneral.h"
#include "../../../Sigma/Math/Rect.h"
#include "../../../Sigma/Math/Triangle.h"

static const Angle MAX_DRAWABLE_ANGLE = PI / 24.0f;  // line 22

class PieChartUISprite : public UISprite {  // size 0x230, line 24
public:
    virtual void Draw();  // vtable slot 0
    float m_flZ;  // 0x08
    float m_flSampleTotal;  // 0x0c
    Rect2D m_tRectWindowUIC;  // 0x10
    XArray<UIPieChartSample, const UIPieChartSample &, 16> m_artSamples;  // 0x20
};
static_assert(sizeof(PieChartUISprite) == 0x230, "PieChartUISprite");



static UISpritePool<PieChartUISprite, 16> s_tPieChartUISpritePool;

static void sCalculatePiePieceTriangles(const Angle angleStart, const Angle angleSize, Vector2D wpCenter, float flRadius,
                                        const float flZ, const RGBAColor & rgbaColor, TriangleList2D & triList);
































UIPieChartControl::UIPieChartControl(const WindowCreationParams & tParams, const Rect2D * pRect)
    : UIWindow(tParams, pRect)
{
    sInit();
}






UIPieChartControl::~UIPieChartControl()
{
}




inline void UIPieChartControl::sInit()
{
    m_flSampleTotal = 0.0f;
    m_nTooltipSampleIndex = -1;
}






























UIID UIPieChartControl::HitTestForTooltips(const IVector2D & pt, BOOL &)
{
    if (!IsActive() ||
        !IsInside(pt))
    {
        m_nTooltipSampleIndex = -1;
        return GetUIIDNone();
    }








    Vector2D flPt((float)pt.x, (float)pt.y);
    Rect2D rectWindowUIC;
    GetWindowRect(rectWindowUIC);
    Rect2D rectWindowPixels;
    ConvertUICToPixels(rectWindowUIC, rectWindowPixels);
    const Vector2D wpCenter((rectWindowPixels.left + rectWindowPixels.right) * 0.5f, (rectWindowPixels.bottom + rectWindowPixels.top) * 0.5f);
    const float flRadius = Math::Min(wpCenter.x - rectWindowPixels.left, rectWindowPixels.top - wpCenter.y);
    Angle angleStart = 0.0f;
    for (int32 i = 0; i < m_artSamples.GetArraySize(); i++)
    {
        UIPieChartSample & tSample = m_artSamples[i];
        Angle sampleAngle = tSample.flSampleValue / m_flSampleTotal * FULL_CIRCLE;
        TriangleList2D * ptTriList = GfxPrimitivesGetSharedTriList2D();
        TriangleList2D & triList = *ptTriList;
        triList.Clear();
        sCalculatePiePieceTriangles(angleStart, sampleAngle, wpCenter, flRadius, 1.0f, tSample.rgbaPieColor, triList);
        for (int32 j = 0; j < triList.GetCount(); j++)
        {
            Triangle2D tri;
            if (triList.GetTriangle(j, tri) && tri.ContainsPoint(flPt))
            {
                m_nTooltipSampleIndex = i;
                return GetID();
            }
        }
        angleStart += sampleAngle;
    }

    m_nTooltipSampleIndex = -1;
    return GetUIIDNone();
}








































void UIPieChartControl::Draw(const uint8 bLayerAlpha, float & flZSlice)
{
    if (!IsActive())
        return;

    uint8 bAlphaCascaded = MultiplyAlpha(bLayerAlpha, GetWindowColor().a);
    if (bAlphaCascaded == 0)
        return;

    float flZ = UIDC_DIALOG_Z_START - UIDC_DIALOG_Z_INCREMENT * flZSlice;
    flZSlice += 1.0f;

    PieChartUISprite * ptSprite = s_tPieChartUISpritePool.Get();
    if (ptSprite == NULL)
        return;

    ptSprite->m_flSampleTotal = m_flSampleTotal;
    ptSprite->m_tRectWindowUIC = m_rect;
    ptSprite->m_artSamples = m_artSamples;
    ptSprite->m_flZ = flZ;

    SpriteDrawUISprite(ptSprite, flZ, RENDERLAYER_UI);
}

















static Vector2D sCalculatePiePiecePoint(const Angle angle, const float radius, const Vector2D vecCenter)
{
    Vector2D pt;
    pt.x = vecCenter.x + Math::Cos(angle) * radius;
    pt.y = vecCenter.y + Math::Sin(angle) * radius;
    return pt;
}






static void sCalculatePiePieceTriangles(const Angle angleStart, const Angle angleSize, Vector2D wpCenter, float flRadius,
                                        const float flZ, const RGBAColor & rgbaColor, TriangleList2D & triList)
{
    if (angleSize == 0.0f)
        return;

    Angle angleDrawStart = angleStart;
    Angle angleDrawEnd = angleStart + angleSize;
    while (angleDrawEnd - angleDrawStart > MAX_DRAWABLE_ANGLE)
    {
        Triangle2D tri;
        Vector2D pt0 = sCalculatePiePiecePoint(angleDrawStart, flRadius, wpCenter);
        angleDrawStart += MAX_DRAWABLE_ANGLE;
        Vector2D pt1 = sCalculatePiePiecePoint(angleDrawStart, flRadius, wpCenter);
        tri.Set(wpCenter, pt0, pt1);
        triList.Add(tri, flZ, rgbaColor);
    }

    if (angleDrawEnd - angleDrawStart > 0.0f)
    {
        Triangle2D tri;
        Vector2D pt0 = sCalculatePiePiecePoint(angleDrawStart, flRadius, wpCenter);
        Vector2D pt1 = sCalculatePiePiecePoint(angleDrawEnd, flRadius, wpCenter);
        tri.Set(wpCenter, pt0, pt1);
        triList.Add(tri, flZ, rgbaColor);
    }
}
















void PieChartUISprite::Draw()
{
    TriangleList2D * ptTriList = GfxPrimitivesGetSharedTriList2D();
    TriangleList2D & triList = *ptTriList;
    triList.Clear();

    Rect2D rectWindowPixels;
    ConvertUICToPixels(m_tRectWindowUIC, rectWindowPixels);
    const Vector2D wpCenter((rectWindowPixels.left + rectWindowPixels.right) * 0.5f, (rectWindowPixels.bottom + rectWindowPixels.top) * 0.5f);
    const float flRadius = Math::Min(wpCenter.x - rectWindowPixels.left, rectWindowPixels.top - wpCenter.y);
    Angle angleStart = 0.0f;
    for (int32 i = 0; i < m_artSamples.GetArraySize(); i++)
    {
        UIPieChartSample & tSample = m_artSamples[i];
        Angle sampleAngle = tSample.flSampleValue / m_flSampleTotal * FULL_CIRCLE;
        sCalculatePiePieceTriangles(angleStart, sampleAngle, wpCenter, flRadius, m_flZ, tSample.rgbaPieColor, triList);
        angleStart += sampleAngle;
    }

    triList.Draw();
}
