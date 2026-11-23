#include "TrailUpdate.h"
#include "Trails.h"
#include "Definitions/TrailDefinition.h"
#include "../Sigma/Math/MathGeneral.h"




















static void sTrailCurveEvaluate(
    const TrailUpdateParams & tTrailUpdateParams,
    const int32 nCurveIndex,
    const int32 nKnotIndex,
    const float flT)
{
    int32 nMaxPointIndex = tTrailUpdateParams.nLength - 1;
    int32 nFirstIndex = Math::Max(nKnotIndex - 1, 0);
    int32 nSecondIndex = nKnotIndex;
    int32 nThirdIndex = Math::Min(nMaxPointIndex, nKnotIndex + 1);
    int32 nFourthIndex = Math::Min(nMaxPointIndex, nKnotIndex + 2);

    const TrailKnotPoint & tFirstTrailknotPoint = tTrailUpdateParams.ptTrailKnotPoints[nFirstIndex];
    const TrailKnotPoint & tSecondTrailknotPoint = tTrailUpdateParams.ptTrailKnotPoints[nSecondIndex];
    const TrailKnotPoint & tThirdTrailknotPoint = tTrailUpdateParams.ptTrailKnotPoints[nThirdIndex];
    const TrailKnotPoint & tFourthTrailknotPoint = tTrailUpdateParams.ptTrailKnotPoints[nFourthIndex];
    TrailCurvePoint & tTrailCurvePoint = tTrailUpdateParams.ptTrailCurvePoints[nCurveIndex];


    float flTSqr = flT * flT;
    float flTCub = flT * flTSqr;

    float flC0 = (2.0f * flTSqr - flTCub - flT) * 0.5f;
    float flC1 = (3.0f * flTCub - 5.0f * flTSqr + 2.0f) * 0.5f;
    float flC2 = (4.0f * flTSqr - 3.0f * flTCub + flT) * 0.5f;
    float flC3 = (flTCub - flTSqr) * 0.5f;

    tTrailCurvePoint.wpCurvePoint0 = tFirstTrailknotPoint.wpKnotPoint0 * flC0 + tSecondTrailknotPoint.wpKnotPoint0 * flC1 + tThirdTrailknotPoint.wpKnotPoint0 * flC2 + tFourthTrailknotPoint.wpKnotPoint0 * flC3;
    tTrailCurvePoint.wpCurvePoint1 = tFirstTrailknotPoint.wpKnotPoint1 * flC0 + tSecondTrailknotPoint.wpKnotPoint1 * flC1 + tThirdTrailknotPoint.wpKnotPoint1 * flC2 + tFourthTrailknotPoint.wpKnotPoint1 * flC3;
}




void TrailUpdate(
    const TrailUpdateParams & tTrailUpdateParams)
{
    WorldDistance wdTotalLength = 0.0f;
    WorldDistance wdCurLengths[MAX_TRAIL_CURVE_POINTS];
    wdCurLengths[0] = 0.0f;

    int32 nLength = tTrailUpdateParams.nLength;
    int32 nCurvePoints = tTrailUpdateParams.nCurvePoints;
    int32 nCurvePointsDivider = Math::Max(nCurvePoints - 1, 1);

    float flLength = (float)(nLength - 1);
    float fCurvePointsMinusOne = (float)(nCurvePoints - 1);
    float flIter = 0.0f;
    for (int32 i = 0; i < nCurvePoints; ++i, flIter += 1.0f)
    {
        float flFractionAlongTrail = flIter / fCurvePointsMinusOne;
        float flKnotPos = flFractionAlongTrail * flLength;
        int32 nKnotIndex = i >> 2;
        if (i == nCurvePoints - 1)
            ++nKnotIndex;



        float flKnotFraction = flKnotPos - floorf(flKnotPos);
        sTrailCurveEvaluate(tTrailUpdateParams, i, nKnotIndex, flKnotFraction);

        if (i > 0)
        {
            TrailCurvePoint & tCurvePoint0 = tTrailUpdateParams.ptTrailCurvePoints[i - 1];
            TrailCurvePoint & tCurvePoint1 = tTrailUpdateParams.ptTrailCurvePoints[i];
            WorldDistance wdCurLength = Vector3D::Distance(tCurvePoint0.wpCurvePoint0, tCurvePoint1.wpCurvePoint0) + Vector3D::Distance(tCurvePoint0.wpCurvePoint1, tCurvePoint1.wpCurvePoint1);
            wdTotalLength += wdCurLength;
            wdCurLengths[i] = wdTotalLength;
        }
    }

    ResolvedInterpolationNode<RGBAColor> resolvedColorNodes[64];
    ResolvedInterpolationPath<RGBAColor> resolvedPathColorOverLength(64, resolvedColorNodes);
    tTrailUpdateParams.ptPathColorOverLength->Resolve(tTrailUpdateParams.flColorOverLengthInterp, resolvedPathColorOverLength);

    ResolvedInterpolationNode<float> resolvedAlphaNodes[64];
    ResolvedInterpolationPath<float> resolvedPathAlphaOverLength(64, resolvedAlphaNodes);
    tTrailUpdateParams.ptPathAlphaOverLength->Resolve(tTrailUpdateParams.flAlphaOverLengthInterp, resolvedPathAlphaOverLength);

    InterpCursor colorCursor;
    InterpCursor alphaCursor;

    RGBAColor rgbaTrailColor = tTrailUpdateParams.rgbaColor;


    if (tTrailUpdateParams.fScaleTextureToLength)
    {
        WorldDistance wdInvTotalLength = 0.0f;
        if (wdTotalLength > EPSILON)
        {
            wdInvTotalLength = 1.0f / wdTotalLength;
        }

        for (int32 i = 0; i < nCurvePoints; ++i)
        {
            TrailCurvePoint & tCurvePoint = tTrailUpdateParams.ptTrailCurvePoints[i];
            tCurvePoint.flFractionAlongTrail = wdCurLengths[i] * wdInvTotalLength;


            InterpTickTime tCurveInterp;
            tCurveInterp.Init();

            const int32 SCALE_FACTOR = 1000;
            tCurveInterp.tTickTime = (i * SCALE_FACTOR) / nCurvePointsDivider;
            tCurveInterp.nTimeType = TICK_TIME_NON_LOOPING;
            tCurveInterp.tScale = SCALE_FACTOR;

            RGBAColor rgbaColorOverLength;
            resolvedPathColorOverLength.GetInterpolatedValueByTickTime(tCurveInterp, rgbaColorOverLength, colorCursor);

            float flAlphaOverLength;
            resolvedPathAlphaOverLength.GetInterpolatedValueByTickTime(tCurveInterp, flAlphaOverLength, alphaCursor);

            if (flAlphaOverLength < 1.0f - EPSILON)
            {
                rgbaColorOverLength.a = Math::FloatRoundToInt(flAlphaOverLength * 255.0f);
            }

            tCurvePoint.rgbaCurvePointColor = rgbaTrailColor * rgbaColorOverLength;
        }
    }
    else
    {   flIter = 0.0f; // Original floating iterator restarts for unscaled coordinates.
        for (int32 i = 0; i < nCurvePoints; ++i, flIter += 1.0f)
        {
            TrailCurvePoint & tCurvePoint = tTrailUpdateParams.ptTrailCurvePoints[i];
            tCurvePoint.flFractionAlongTrail = flIter / fCurvePointsMinusOne;


            InterpTickTime tCurveInterp;
            tCurveInterp.Init();

            const int32 SCALE_FACTOR = 1000;
            tCurveInterp.tTickTime = (i * SCALE_FACTOR) / nCurvePointsDivider;
            tCurveInterp.nTimeType = TICK_TIME_NON_LOOPING;
            tCurveInterp.tScale = SCALE_FACTOR;

            RGBAColor rgbaColorOverLength;
            resolvedPathColorOverLength.GetInterpolatedValueByTickTime(tCurveInterp, rgbaColorOverLength, colorCursor);

            float flAlphaOverLength;
            resolvedPathAlphaOverLength.GetInterpolatedValueByTickTime(tCurveInterp, flAlphaOverLength, alphaCursor);

            if (flAlphaOverLength < 1.0f - EPSILON)
            {
                rgbaColorOverLength.a = Math::FloatRoundToInt(flAlphaOverLength * 255.0f);
            }

            tCurvePoint.rgbaCurvePointColor = rgbaTrailColor * rgbaColorOverLength;
        }
    }
}





void TrailUpdateParams::FromTrailAndDefinition(
    Trail * ptTrail,
    const TrailDefinition * pdefTrail)
{
    nCurvePoints = ptTrail->nCurvePoints;
    nLength = ptTrail->nLength;
    flColorOverLengthInterp = ptTrail->flColorOverLengthInterp;
    flAlphaOverLengthInterp = ptTrail->flAlphaOverLengthInterp;
    fScaleTextureToLength = (pdefTrail->dwTrailDefFlags >> TRAILDEF_SCALE_TEXTURE_TO_LENGTH) & 1;
    rgbaColor = ptTrail->rgbaColor;
    ptPathColorOverLength = &pdefTrail->pathColorOverLength;
    ptPathAlphaOverLength = &pdefTrail->pathAlphaOverLength;
    ptTrailCurvePoints = ptTrail->tTrailCurvePoints;
    ptTrailKnotPoints = ptTrail->tTrailKnotPoints;
}









