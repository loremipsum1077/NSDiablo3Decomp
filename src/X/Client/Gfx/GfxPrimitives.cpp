// X/Client/Gfx/GfxPrimitives.cpp
// Functions sit on their original line numbers (2.6.2 DWARF). Generated from
// build/try/p2-render/prim/frag.cpp by build/try/p2-render/gen.py.

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "GfxPrimitives.h"
#include "Gfx.h"
#include "GfxCommon.h"
#include "GfxInternal.h"
#include "GfxVB.h"
#include "../CGame.h"
#include "../Sprite.h"
#include "../Camera/CameraManager.h"
#include "../Camera/Camera.h"
#include "../RWindow.h"
#include "../Definitions/ShaderDefinition.h"
#include "../../Common/GlobalSNO.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Math/Matrix.h"
#include "../../Sigma/Math/Ray.h"
#include "../../Sigma/Math/Rect.h"
#include "../../Sigma/Math/Triangle.h"
#include "../../Sigma/Math/Vector.h"
#include "../../Sigma/RGBAColor.h"

// X/Client/Gfx/Gfx.cpp:81 and GfxVB.cpp
extern VBFormatInfo cgtVBFormatInfo[NUM_VBFORMATS];
VBID GfxVBAddRef(const VBID vbid);
int32 GfxVBGetMaxCount(const VBID vbid);
void GfxWriteToVB(const VBID vbid, const uint32 dwVertCount, uint32 & dwStartIndex, const void * pSrc,
                  const uint32 dwSize);
// X/Client/Gfx/Gfx.h, GfxShader.h
void GfxWorldMatrixSet(const Matrix4x4 & m);
void GfxViewMatrixSet(const Matrix4x4 & m);
void GfxProjectionMatrixSet(const Matrix4x4 & m);
void GfxBuild2DUIProjectionMatrix(Matrix4x4 & mProj);
void GfxShaderInit(const SNO sno, const int32 nPassIndex);
void GfxShaderDraw(const DrawMode eDrawMode, const uint32 dwBaseIndexOffset, const uint32 dwMinVertexIndex, const uint32 dwNumVertices, const uint32 dwStartIndex, const uint32 dwPrimCount, const PerfBin ePerfBin);
void GfxShaderSetStream(const StreamIndex eStreamIndex, const uint32 dwStreamOffset, const VBID vbid);
void GfxShaderSetIB(const IBID ibid, const uint32 dwIndexOffset);
void GfxShaderSetCullMode(const CullMode eCullMode);

void GfxPrimitivesFlush();
static void sTriOverlayAdd(const Vector3D & wp0, const Vector3D & wp1, const Vector3D & wp2, const RGBAColor & rgbaColor, GlobalSNO snoShader, const RenderContext eContext);
void GfxSolidCircleDrawDeferred(Vector3D wpCentre, Vector3D vAxis, WorldDistance wdRadius, int nCircleSamplePointCount, RGBAColor rgbaColor, GlobalSNO snoShader, const RenderContext eContext);
void GfxDrawSphereAxis(Vector3D wpCentre, Vector3D vAxis, WorldDistance wdRadius, const Camera & tCamera, RGBAColor rgbaColor, int nCircleSamplePointCount, GlobalSNO snoShader, const RenderContext eContext);
void GfxLineCircleDrawDeferred(Vector3D wpCentre, Vector3D vAxis, WorldDistance wdRadius, int nCircleSamplePointCount, RGBAColor rgbaColor, GlobalSNO snoShader, const RenderContext eContext);
void GfxLineHalfCircleDrawDeferred(Vector3D wpCentre, Vector3D vAxis, Vector3D vFacing, WorldDistance wdRadius, int nCircleSamplePointCount, RGBAColor rgbaColor, bool bSolid, GlobalSNO snoShader, const RenderContext eContext);
static void sLine3DAdd(const Vector3D & wp0, const Vector3D & wp1, const VertexColor vcColor, GlobalSNO snoShader, const RenderContext eContext);
void GfxSphereDrawDeferred(const Sphere & ws, const Camera & tCamera, RGBAColor rgbaLineColor, RGBAColor rgbaFillColor, int nCircleSamplePointCount, GlobalSNO snoShader, const RenderContext eContext);
void GfxDashedLine3DDrawDeferred(const Vector3D & wp0, const Vector3D & wp1, const RGBAColor & rgbaColor, int nNumDashes, GlobalSNO snoShader, const RenderContext eContext);
static void sLineSegs3DAdd(LineSeg3D * wvEdges, int32 nEdgeCount, const RGBAColor & rgbaColor, GlobalSNO snoShader, GfxLineStyle eLineStyle, const RenderContext eContext);
// X/Client/Gfx/GfxVB.cpp:414 and :437; no adopted header declares them yet.
UByte4 Vector3DNormalToUByte4(const Vector3D & wvNormal);
UByte4 Vector2DTexCoordToUByte4(const Vector2D & uv);
void GfxDrawTriOverlayDeferred( const Triangle3D & wtri, const RGBAColor & rgbaColor, GlobalSNO snoShader, const RenderContext eContext );






void GfxPrimitivesGfxInit() {
    gptGfxData->vbidLineList3D = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_NOTEX_L].vbidPooled);
    gptGfxData->vbidPointList3D = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_NOTEX_L].vbidPooled);
    gptGfxData->vbidLineList2D = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_NOTEX_L].vbidPooled);
    gptGfxData->vbidTriOverlay = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_1TEX_L].vbidPooled);
    gptGfxData->vbidTriList2D = GfxVBAddRef(cgtVBFormatInfo[VBFORMAT_NOTEX_L].vbidPooled);

    gptGfxData->ptTriList2D = new TriangleList2D;

    GfxPrimitivesFlush();
}





void GfxPrimitivesGfxDispose() {
    GfxVBRelease(gptGfxData->vbidTriOverlay);
    GfxVBRelease(gptGfxData->vbidLineList3D);
    GfxVBRelease(gptGfxData->vbidPointList3D);
    GfxVBRelease(gptGfxData->vbidLineList2D);
    GfxVBRelease(gptGfxData->vbidTriList2D);

    delete gptGfxData->ptTriList2D;
    gptGfxData->ptTriList2D = NULL;
}





static void sLine2DAdd( const float flX0, const float flY0, const float flX1, const float flY1, const VertexColor vcColor, const float flZ, const RenderContext eContext ) {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_UPDATE )];
    // Line table 0x71001d5690..: z/vcDiffuse at 106/107, x0 at 109, x1/y1 at 113; no Vector3D::Set inline in the DWARF.
    VertexNoTexL tVert;
    tVert.wpPos.z = flZ;
    tVert.vcDiffuse = vcColor;
    tVert.wpPos.x = flX0;
    tVert.wpPos.y = flY0;
    tData.tLineList2DVertices.push_back( tVert );
    tVert.wpPos.x = flX1;
    tVert.wpPos.y = flY1;
    tData.tLineList2DVertices.push_back( tVert );

    if ( !tData.tLine2DShaderBatches.IsEmpty() ) {
        Line2DShaderBatch & tLastBatch = tData.tLine2DShaderBatches.GetTail();
        if ( tLastBatch.eContext == eContext ) {
            tLastBatch.nEndLineVertexIndex += 2;
        } else {
            int nLineVertexIndex = tLastBatch.nEndLineVertexIndex + 1;
            Line2DShaderBatch tNewBatch;
            tNewBatch.nStartLineVertexIndex = nLineVertexIndex;
            tNewBatch.nEndLineVertexIndex = nLineVertexIndex + 1;
            tNewBatch.eContext = eContext;
            tData.tLine2DShaderBatches.AddTail_Tracked( tNewBatch );
        }
    } else {
        Line2DShaderBatch tFirstBatch;
        tFirstBatch.nStartLineVertexIndex = 0;
        tFirstBatch.nEndLineVertexIndex = 1;
        tFirstBatch.eContext = eContext;
        tData.tLine2DShaderBatches.AddTail_Tracked( tFirstBatch );
    }
}





























void GfxLine2DDrawDeferred( const float flX0, const float flY0, const float flX1, const float flY1, const RGBAColor & rgbaColor, const float flZ, const RenderContext eContext ) {
    sLine2DAdd( flX0, flY0, flX1, flY1, VertexColor( rgbaColor ), flZ, eContext );
}













void GfxRectDrawDeferred( const Rect2D & wrRect,
                          const RGBAColor & rgbaColor,
                          const float flZ,
                          const RenderLayer eRenderLayerOverride,
                          BOOL fInRenderDraw ) {
    SNO snoShader = GlobalSNOGet( rgbaColor.a != 255 ? GLOBALSNO_SHADER_2D_PRIMS_TRANSPARENT : GLOBALSNO_SHADER_2D_PRIMS );

    Rect2D rectNew = wrRect;
    if ( rectNew.right < rectNew.left ) {
        float flTemp = rectNew.right;
        rectNew.right = rectNew.left;
        rectNew.left = flTemp;
    }
    if ( rectNew.bottom < rectNew.top ) {
        float flTemp = rectNew.bottom;
        rectNew.bottom = rectNew.top;
        rectNew.top = flTemp;
    }

    SpriteDrawDeferred( cgtImageTextureFrameNull, snoShader, rectNew, rgbaColor, crgbaBlack, flZ, NULL, -1, NULL, 0, eRenderLayerOverride, fInRenderDraw );
}













void GfxRectFrameDrawDeferred( const Rect2D & wrRect, const RGBAColor & rgbaColor, const float flZ, const RenderLayer eRenderLayerOverride ) {
    float flX0 = wrRect.left - 1.0f;
    float flY0 = wrRect.top - 1.0f;
    float flX1 = wrRect.right;
    float flY1 = wrRect.bottom;

    Rect2D rectTop( flX0, flY0, flX1, flY0 + 1.0f );
    Rect2D rectLeft( flX0, flY0, flX0 + 1.0f, flY1 );
    Rect2D rectBottom( flX0, flY1, flX1, flY1 + 1.0f );
    Rect2D rectRight( flX1, flY0, flX1 + 1.0f, flY1 + 1.0f );

    GfxRectDrawDeferred( rectTop, rgbaColor, flZ, eRenderLayerOverride, FALSE );
    GfxRectDrawDeferred( rectLeft, rgbaColor, flZ, eRenderLayerOverride, FALSE );
    GfxRectDrawDeferred( rectBottom, rgbaColor, flZ, eRenderLayerOverride, FALSE );
    GfxRectDrawDeferred( rectRight, rgbaColor, flZ, eRenderLayerOverride, FALSE );
}










































static void sLine3DAdd( const Vector3D & wp0, const Vector3D & wp1, const VertexColor vcColor, GlobalSNO snoShader, const RenderContext eContext ) {
    VertexNoTexL tVert;
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_UPDATE )];

    tVert.wpPos = wp0;
    tVert.vcDiffuse = vcColor;
    tData.tLineList3DVertices.push_back( tVert );
    tVert.wpPos = wp1;
    tData.tLineList3DVertices.push_back( tVert );

    if ( !tData.tLine3DShaderBatches.IsEmpty() ) {
        Line3DShaderBatch & tLastBatch = tData.tLine3DShaderBatches.GetTail();
        if ( tLastBatch.snoShader == snoShader && tLastBatch.eContext == eContext ) {
            tLastBatch.nEndLineVertexIndex += 2;
        } else {
            int nLineVertexIndex = tLastBatch.nEndLineVertexIndex + 1;
            Line3DShaderBatch tNewBatch;
            tNewBatch.nStartLineVertexIndex = nLineVertexIndex;
            tNewBatch.nEndLineVertexIndex = nLineVertexIndex + 1;
            tNewBatch.snoShader = snoShader;
            tNewBatch.eContext = eContext;
            tData.tLine3DShaderBatches.AddTail_Tracked( tNewBatch );
        }
    } else {
        Line3DShaderBatch nFirstBatch;
        nFirstBatch.nStartLineVertexIndex = 0;
        nFirstBatch.nEndLineVertexIndex = 1;
        nFirstBatch.snoShader = snoShader;
        nFirstBatch.eContext = eContext;
        tData.tLine3DShaderBatches.AddTail_Tracked( nFirstBatch );
    }
}



























static void sLineSegs3DAdd( LineSeg3D * wvEdges, int32 nEdgeCount, const RGBAColor & rgbaColor, GlobalSNO snoShader, GfxLineStyle eLineStyle, const RenderContext eContext ) {
    const WorldDistance DASH_SIZE = 3.0f;

    for ( int i = 0; i < nEdgeCount; i++ ) {
        Vector3D wpStart( wvEdges[i].wr.wpOrigin );
        Vector3D wpEnd = wvEdges[i].GetEndPoint();

        if ( eLineStyle == GFX_LINE_STYLE_SOLID ) {
            GfxLine3DDrawDeferred( wpStart, wpEnd, rgbaColor, snoShader, eContext );
        } else {
            int nNumDashes = (int)( Vector3D::Distance(wpEnd, wpStart) / DASH_SIZE );
            if ( ( nNumDashes & 1 ) == 0 ) {
                nNumDashes++;
            }
            GfxDashedLine3DDrawDeferred( wpStart, wpEnd, rgbaColor, nNumDashes, snoShader, eContext );
        }
    }
}

















void GfxLine3DDrawDeferred( const Vector3D & wp0, const Vector3D & wp1, const RGBAColor & rgbaColor, GlobalSNO snoShader, const RenderContext eContext ) {
    sLine3DAdd( wp0, wp1, VertexColor( rgbaColor ), snoShader, eContext );
}










inline void GfxDashedLine3DDrawDeferred( const Vector3D & wp0, const Vector3D & wp1, const RGBAColor & rgbaColor, int nNumDashes, GlobalSNO snoShader, const RenderContext eContext ) {
    Vector3D wvDash = ( wp1 - wp0 ) * ( 1.0f / nNumDashes );
    for ( int i = 0; i < nNumDashes; i += 2 ) {
        Vector3D wpStart = wp0 + wvDash * (float)i;
        Vector3D wpEnd = wpStart + wvDash;
        sLine3DAdd( wpStart, wpEnd, VertexColor( rgbaColor ), snoShader, eContext );
    }
}














































void GfxArrow3DDrawDeferred(
    const Vector3D & wp0,
    const Vector3D & wp1,
    const RGBAColor & rgbaColor,
    const float flHeadLength,
    GlobalSNO snoShader,
    const RenderContext eContext)
{
    VertexColor vcColor( rgbaColor );

    sLine3DAdd( wp0, wp1, vcColor, snoShader, eContext );

    Vector3D wvTemp = wp0 - wp1;
    wvTemp.Normalize();
    wvTemp *= flHeadLength;

    Angle aHead = Math::DegToAngle(20.0f);
    Vector3D wv;
    Matrix3x3 mRot;
    mRot.SetRotationZ( aHead );
    wv = wvTemp * mRot;
    sLine3DAdd(wp1, wp1 + wv, vcColor, snoShader, eContext);

    mRot.SetRotationZ(Math::NormalizeAngle(-aHead));
    wv = wvTemp * mRot;
    sLine3DAdd(wp1, wp1 + wv, vcColor, snoShader, eContext);
}





void GfxAABB3DDrawDeferred( const AABB & aabb, const RGBAColor & rgbaColor, GlobalSNO snoShader, GfxLineStyle eLineStyle, const RenderContext eContext ) {
    LineSeg3D wvEdges[12];
    aabb.ComputeEdges( wvEdges );
    sLineSegs3DAdd( wvEdges, 12, rgbaColor, snoShader, eLineStyle, eContext );
}










void GfxOBB3DDrawDeferred( const OBB & obb, const RGBAColor & rgbaColor, GlobalSNO snoShader, GfxLineStyle eLineStyle, const RenderContext eContext ) {
    LineSeg3D wvEdges[12];
    obb.ComputeEdges( wvEdges );
    sLineSegs3DAdd( wvEdges, 12, rgbaColor, snoShader, eLineStyle, eContext );
}










void GfxSimpleConeDrawDeferred( const Vector3D & wpTip, const Vector3D & vDir, const float flRadius, const Angle aConeAngle, const Camera & tCamera, const RGBAColor & rgbaColor, const int32 nNumDivisions, const int32 nNumSections, GlobalSNO snoShader, const RenderContext eContext )










{
    Vector3D wvNearestPtOnLine = wpTip + vDir * Vector3D::DotProduct( tCamera.cmState.transform.wp - wpTip, vDir );
    Vector3D wvCamToLine = wvNearestPtOnLine - tCamera.cmState.transform.wp;
    wvCamToLine.Normalize();

    Quaternion qRotation1, qRotation2;
    qRotation1.SetRotation( wvCamToLine, aConeAngle );
    qRotation2.SetRotation( wvCamToLine, -aConeAngle );

    Vector3D vecEdge1 = qRotation1.Rotate( vDir );
    Vector3D vecEdge2 = qRotation2.Rotate( vDir );

    Vector3D wpFar1 = wpTip + vecEdge1 * flRadius;
    Vector3D wpFar2 = wpTip + vecEdge2 * flRadius;

    GfxLine3DDrawDeferred( wpTip, wpFar1, rgbaColor, snoShader, eContext );
    GfxLine3DDrawDeferred( wpTip, wpFar2, rgbaColor, snoShader, eContext );

    Vector3D * vecCircle = (Vector3D *)alloca( sizeof( Vector3D ) * nNumDivisions );
    for ( int32 d = 0; d < nNumDivisions; d++ ) {

        float aAngle = (float)d * FULL_CIRCLE / (float)nNumDivisions;

        Quaternion qRot;
        qRot.SetRotation( vDir, aAngle );

        vecCircle[d] = qRot.Rotate( vecEdge1 );
    }

    for ( int32 s = 1; s <= nNumSections; s++ ) {

        float flSectionRadius = (float)s * flRadius / (float)nNumSections;

        for ( int32 d = 0; d < nNumDivisions - 1; d++ ) {

            GfxLine3DDrawDeferred( vecCircle[d] * flSectionRadius + wpTip, wpTip + vecCircle[d + 1] * flSectionRadius, rgbaColor, snoShader, eContext );
        }
        GfxLine3DDrawDeferred( vecCircle[nNumDivisions - 1] * flSectionRadius + wpTip, wpTip + vecCircle[0] * flSectionRadius, rgbaColor, snoShader, eContext );

    }
}


void GfxConeDrawDeferred(
    const Vector3D & wpTip,
    const Vector3D & vDir,
    const float flLength,
    const Angle aConeAngle,
    const RGBAColor & rgbaColor,
    const int32 nNumDivisions,
    const int32 nNumSections,
    GfxFillStyle eFillStyle,
    GlobalSNO snoShader,
    const RenderContext eContext)
{


    Vector3D vUp( cgwvZAxis );
    if ( Math::Abs( Vector3D::DotProduct( -vDir, vUp ) ) > 0.99f ) {

        vUp = cgwvYAxis;
    }

    Vector3D vRight = Vector3D::CrossProduct( -vDir, vUp );
    vRight.Normalize();


    vUp = Vector3D::CrossProduct( -vDir, vRight );
    vUp.Normalize();

    Matrix3x3 mRot;
    mRot.SetBasisVectors( vRight, vUp, -vDir );
    Matrix4x4 mTransform;
    mTransform.Set( mRot, cgwpNull );

    Angle flRadius = Math::Tan( aConeAngle ) * flLength;

    int32 nNumCylinderVerts = ( nNumSections + 1 ) * nNumDivisions;
    int32 nNumEndVerts = nNumDivisions + 1;
    int32 nNumVerts = nNumCylinderVerts + nNumEndVerts;

    int32 nNumCylinderTris = nNumSections * (nNumDivisions * 2);
    int32 nNumEndTris = nNumDivisions;

    int32 nNumCylinderIndicies = nNumCylinderTris * 3;
    int32 nNumEndIndicies = nNumEndTris * 3;
    int32 nNumIndicies = nNumCylinderIndicies + nNumEndIndicies;




    Vector3D * VB = new Vector3D[nNumVerts];

    int32 i = 0;
    for ( int32 s = 0; s <= nNumSections; s++ ) {

        float flSectionRadius = flRadius / (float)nNumSections * (float)s;

        for ( int32 d = 0; d < nNumDivisions; d++ ) {

            float aAngle = (float)d * FULL_CIRCLE / (float)nNumDivisions;

            VB[i].x = flSectionRadius * Math::Cos( aAngle );
            VB[i].y = -flSectionRadius * Math::Sin( aAngle );
            VB[i].z = -(float)s * flLength / (float)nNumSections;
            i++;
        }
    }




    for ( int32 d = 0; d < nNumDivisions; d++ ) {

        float aAngle = (float)d * FULL_CIRCLE / (float)nNumDivisions;
        VB[i].x = flRadius * Math::Cos( aAngle );
        VB[i].y = -flRadius * Math::Sin( aAngle );
        VB[i].z = -flLength;
        i++;
    }




    VB[i] = cgwpNull;




    for ( i = 0; i < nNumVerts; i++ ) {

        VB[i] = VB[i] * mTransform;
        VB[i] += wpTip;
    }




    int32 * IB = new int32[nNumIndicies];


    int32 nIndex = 0;
    i = 0;
    for ( int32 s = 0; s < nNumSections; s++ ) {

        int32 nStartIndex = nIndex;

        for ( int32 d = 0; d < nNumDivisions - 1; d++ ) {

            IB[i++] = nIndex;
            IB[i++] = nIndex + nNumDivisions;
            IB[i++] = nIndex + nNumDivisions + 1;

            IB[i++] = nIndex + nNumDivisions + 1;
            IB[i++] = nIndex + 1;
            IB[i++] = nIndex;
            nIndex++;
        }


        IB[i++] = nIndex;
        IB[i++] = nIndex + nNumDivisions;
        IB[i++] = nStartIndex + nNumDivisions;

        IB[i++] = nStartIndex + nNumDivisions;
        IB[i++] = nStartIndex;
        IB[i++] = nIndex;
        nIndex++;
    }






    for ( int32 d = 0; d < nNumDivisions; d++ ) {

        IB[i++] = nNumCylinderVerts + d;
        IB[i++] = nNumCylinderVerts + ((d + 1 == nNumDivisions) ? 0 : d + 1);
        IB[i++] = nNumCylinderVerts + nNumDivisions;
    }









    for ( i = 0; i < nNumIndicies; ) {

        Triangle3D wTri;
        wTri.wp0 = VB[IB[i++]];
        wTri.wp1 = VB[IB[i++]];
        wTri.wp2 = VB[IB[i++]];

        if ( eFillStyle == GFX_FILL_SOLID ) {

            GfxDrawTriOverlayDeferred( wTri, rgbaColor, snoShader, eContext );
        } else {


            GfxLine3DDrawDeferred( wTri.wp0, wTri.wp1, rgbaColor, snoShader, eContext );
            GfxLine3DDrawDeferred( wTri.wp0, wTri.wp2, rgbaColor, snoShader, eContext );
            GfxLine3DDrawDeferred( wTri.wp1, wTri.wp2, rgbaColor, snoShader, eContext );
        }
    }

    delete[] VB;
    delete[] IB;
}










Matrix3x3 UnitMatrixFromUnitVector(const Vector3D & vUnitZ)
{
    Vector3D vZ(vUnitZ);


    Vector3D vX;
    vX.Set(vZ.z, 0.0f, -vZ.x);

    float flSqMag = vX.MagnitudeSquared();

    Vector3D vY;
    if (flSqMag > EPSILON)
    {
        vX *= 1.0f / Math::Sqrt(flSqMag);
        vY = Vector3D::CrossProduct(vZ, vX);
    }
    else
    {
        if (vZ.y > 0.0f)
        {

            vX = cgwvXAxis;
            vY = -cgwvZAxis;
            vZ = cgwvYAxis;
        }
        else
        {

            vX = cgwvXAxis;
            vY = cgwvZAxis;
            vZ = -cgwvYAxis;
        }
    }

    Matrix3x3 result;
    result.SetBasisVectors(vX, vY, vZ);
    return result;
}




void GfxLineCircleDrawDeferred(
    Vector3D wvCenter,
    Vector3D vFacing,
    float wdRadius,
    int nNumPoints,
    RGBAColor rgbaColor,
    GlobalSNO snoShader,
    const RenderContext eContext)
{
    VertexColor vcColor( rgbaColor );

    float flRadsPerPoint = FULL_CIRCLE / (float)nNumPoints;



    Matrix4x4 transform;
    transform.Set( UnitMatrixFromUnitVector( vFacing ), wvCenter );

    Vector3D * wps = new Vector3D[nNumPoints];


    for ( int32 i = 0; i < nNumPoints; i++ )
    {
        Angle a = flRadsPerPoint * (float)i;
        wps[i].Set(Math::Cos(a) * wdRadius, Math::Sin(a) * wdRadius, 0.0f);
        wps[i] = wps[i] * transform;
    }


    for ( int32 i = 0; i < nNumPoints - 1; i++ )
    {
        sLine3DAdd( wps[i], wps[i + 1], vcColor, snoShader, eContext );
    }


    sLine3DAdd( wps[nNumPoints - 1], wps[0], vcColor, snoShader, eContext );

    delete[] wps;
}





void GfxLineHalfCircleDrawDeferred( Vector3D wvCenter, Vector3D vN, Vector3D vFacing, float wdRadius, int nNumPoints, RGBAColor rgbaColor, bool bConnectBase, GlobalSNO snoShader, const RenderContext eContext )









{
    if ( nNumPoints < 2 ) {
        return;
    }


    VertexColor vcColor( rgbaColor );

    Vector3D vX( vFacing );
    Vector3D vZ( vN );
    Vector3D vY = Vector3D::CrossProduct( vZ, vX );

    vX.Normalize();
    vY.Normalize();
    vZ.Normalize();

    Matrix3x3 tRotation;
    tRotation.SetBasisVectors( vX, vY, vZ );

    Matrix4x4 transform;
    transform.Set( tRotation, wvCenter );

    Vector3D * wps = new Vector3D[nNumPoints];

    float flRadsPerPoint = HALF_CIRCLE / (float)( nNumPoints - 1 );


    for ( int32 i = 0; i < nNumPoints; i++ ) {

        Angle a = flRadsPerPoint * (float)i - QUARTER_CIRCLE;
        wps[i].Set( Math::Cos( a ) * wdRadius, Math::Sin( a ) * wdRadius, 0.0f );
        wps[i] = wps[i] * transform;
    }


    for ( int32 i = 0; i < nNumPoints - 1; i++ ) {

        sLine3DAdd( wps[i], wps[i + 1], vcColor, snoShader, eContext );
    }


    if ( bConnectBase ) {

        sLine3DAdd( wps[nNumPoints - 1], wps[0], vcColor, snoShader, eContext );
    }

    delete[] wps;

}























































void GfxSolidCircleDrawDeferred(
    Vector3D wvCenter,
    Vector3D vFacing,
    float wdRadius,
    int nNumPoints,
    RGBAColor rgbaColor,
    GlobalSNO snoShader,
    const RenderContext eContext)
{
    VertexColor vcColor( rgbaColor );

    float flRadsPerPoint = FULL_CIRCLE / (float)nNumPoints;



    Matrix4x4 transform;
    transform.Set( UnitMatrixFromUnitVector( vFacing ), wvCenter );

    Vector3D * wps = new Vector3D[nNumPoints];


    for ( int32 i = 0; i < nNumPoints; i++ )
    {
        Angle a = flRadsPerPoint * (float)i;
        wps[i].Set(Math::Cos(a) * wdRadius, Math::Sin(a) * wdRadius, 0.0f);
        wps[i] = wps[i] * transform;
    }

    int nNumTris = nNumPoints - 2;

    Triangle3D wTri;
    for ( int32 i = 0; i < nNumTris; i++ )
    {
        wTri.wp0 = wps[0];
        wTri.wp1 = wps[i + 1];
        wTri.wp2 = wps[i + 2];

        sTriOverlayAdd(wTri.wp0, wTri.wp1, wTri.wp2, rgbaColor, snoShader, eContext);
    }

    delete[] wps;

}




void GfxDrawSolidWedgeDeferred( LineSeg3D wStartWedgeEdgeLine, Angle aWedgeAngle, int nWedgeTriCount, Vector3D vRotAxis, RGBAColor rgbaWedgeFillColor, RGBAColor rgbaWedgeEdgeColor, GlobalSNO snoShader, const RenderContext eContext )








{
    Vector3D wpWedgeOrigin( wStartWedgeEdgeLine.wr.wpOrigin );
    Vector3D vCentreToWedgeStart = wStartWedgeEdgeLine.GetEndPoint() - wStartWedgeEdgeLine.wr.wpOrigin;
    if ( vCentreToWedgeStart.Normalize() <= 0.0f ) {
        return;
    }



    GfxLine3DDrawDeferred( wpWedgeOrigin, wStartWedgeEdgeLine.GetEndPoint(), rgbaWedgeEdgeColor, snoShader, eContext );

    if ( Math::Abs( aWedgeAngle ) > EPSILON ) {

        Triangle3D wTri;
        float aAngleIncrement = aWedgeAngle / (float)nWedgeTriCount;


        for ( int32 i = 0; i < nWedgeTriCount; i++ ) {

            Quaternion qRot;
            qRot.SetRotation( vRotAxis, aAngleIncrement * (float)i );
            wTri.wp0 = qRot.Rotate( vCentreToWedgeStart ) * wStartWedgeEdgeLine.wdLength + wpWedgeOrigin;


            qRot.SetRotation( vRotAxis, aAngleIncrement * (float)( i + 1 ) );
            wTri.wp1 = qRot.Rotate( vCentreToWedgeStart ) * wStartWedgeEdgeLine.wdLength + wpWedgeOrigin;

            wTri.wp2 = wpWedgeOrigin;
            GfxDrawTriOverlayDeferred( wTri, rgbaWedgeFillColor, snoShader, eContext );
        }

        Quaternion qRot;
        qRot.SetRotation( vRotAxis, aWedgeAngle );


        Vector3D wvWedgeEnd = wpWedgeOrigin + qRot.Rotate( vCentreToWedgeStart ) * wStartWedgeEdgeLine.wdLength;
        GfxLine3DDrawDeferred( wStartWedgeEdgeLine.wr.wpOrigin, wvWedgeEnd, rgbaWedgeEdgeColor, snoShader, eContext );
    }
}




void GfxSphereDrawDeferred( const Sphere & ws, const Camera & tCamera, RGBAColor rgbaLineColor, RGBAColor rgbaFillColor, int nCircleSamplePointCount, GlobalSNO snoShader, const RenderContext eContext ) {
    Matrix4x4 mCamera;
    tCamera.cmState.transform.ToMatrix( mCamera );
    Vector3D vFacingCamera = -mCamera.ExtractBasisVectorZ();

    GfxLineCircleDrawDeferred( ws.wpCenter, vFacingCamera, ws.wdRadius, nCircleSamplePointCount, rgbaLineColor, snoShader, eContext );
    GfxDrawSphereAxis( ws.wpCenter, cgwvXAxis, ws.wdRadius, tCamera, rgbaLineColor, nCircleSamplePointCount, snoShader, eContext );
    GfxDrawSphereAxis( ws.wpCenter, cgwvYAxis, ws.wdRadius, tCamera, rgbaLineColor, nCircleSamplePointCount, snoShader, eContext );
    GfxDrawSphereAxis( ws.wpCenter, cgwvZAxis, ws.wdRadius, tCamera, rgbaLineColor, nCircleSamplePointCount, snoShader, eContext );

    if ( rgbaFillColor.a ) {
        GfxSolidCircleDrawDeferred( ws.wpCenter, vFacingCamera, ws.wdRadius, nCircleSamplePointCount, rgbaFillColor, snoShader, eContext );
    }
}
















void GfxDrawSphereAxis( Vector3D wpCentre, Vector3D vAxis, WorldDistance wdRadius, const Camera & tCamera, RGBAColor rgbaColor, int nCircleSamplePointCount, GlobalSNO snoShader, const RenderContext eContext ) {
    Matrix4x4 mCamera;
    tCamera.cmState.transform.ToMatrix( mCamera );
    Vector3D vFacingCamera = -mCamera.ExtractBasisVectorZ();
    float flDotValue = Vector3D::DotProduct( vAxis, vFacingCamera );

    if ( Math::Abs( flDotValue ) >= 0.99f ) {
        GfxLineCircleDrawDeferred( wpCentre, vAxis, wdRadius, nCircleSamplePointCount, rgbaColor, snoShader, eContext );
    } else {
        Vector3D vFacing = vFacingCamera - vAxis * flDotValue;
        if ( vFacing.Normalize() > EPSILON ) {
            GfxLineHalfCircleDrawDeferred( wpCentre, vAxis, vFacing, wdRadius, nCircleSamplePointCount, rgbaColor, false, snoShader, eContext );
        }
    }
}






















void GfxSphereDrawDeferred( const Sphere & ws, RGBAColor rgbaColor, GlobalSNO snoShader, const RenderContext eContext ) {
    const Camera & cam = CameraManagerGetCamera( -1 );
    GfxSphereDrawDeferred( ws, cam, rgbaColor, RGBAColor( 0, 0, 0, 0 ), 20, snoShader, eContext );
}






























void GfxAxialCylinderDrawDeferred(
    const AxialCylinder & wcyl,
    const RGBAColor & rgbaColor,
    GlobalSNO snoShader,
    const RenderContext eContext )
{
    VertexColor vcColor( rgbaColor );

    int32 i;
    const int32 NUM_CYLINDER_POINTS = 10;
    const float DEG_PER_POINT = 360.0f / (float)NUM_CYLINDER_POINTS;
    Vector3D wpTop[NUM_CYLINDER_POINTS];
    Vector3D wpBottom[NUM_CYLINDER_POINTS];

    for ( i = 0; i < NUM_CYLINDER_POINTS; i++ )
    {
        Angle a = Math::DegToAngle( (float)i * DEG_PER_POINT );
        wpBottom[i].Set( wcyl.wpBase.x + Math::Cos( a ) * wcyl.wdRadius, wcyl.wpBase.y + Math::Sin( a ) * wcyl.wdRadius, wcyl.wpBase.z );
        wpTop[i].Set( wcyl.wpBase.x + Math::Cos( a ) * wcyl.wdRadius, wcyl.wpBase.y + Math::Sin( a ) * wcyl.wdRadius, wcyl.wpBase.z + wcyl.wdHeight );
    }

    for ( i = 0; i < NUM_CYLINDER_POINTS - 1; i++ )
    {
        sLine3DAdd( wpBottom[i], wpBottom[i + 1], vcColor, snoShader, eContext );
        sLine3DAdd( wpTop[i], wpTop[i + 1], vcColor, snoShader, eContext );
        sLine3DAdd( wpBottom[i], wpTop[i], vcColor, snoShader, eContext );
    }
    sLine3DAdd( wpBottom[NUM_CYLINDER_POINTS - 1], wpBottom[0], vcColor, snoShader, eContext );
    sLine3DAdd( wpTop[NUM_CYLINDER_POINTS - 1], wpTop[0], vcColor, snoShader, eContext );
}




void GfxCapsuleDrawDeferred(
    const Capsule & c,
    const PRTransform & transform,
    const RGBAColor & rgbaColor,
    GlobalSNO snoShader,
    const RenderContext eContext )
{
    VertexColor vcColor( rgbaColor );

    Vector3D u = c.wp2 - c.wp1;
    if ( u.MagnitudeSquared() < EPSILON * EPSILON )
    {
        Sphere s;
        s.Set( transform * c.wp1, c.flRadius );
        GfxSphereDrawDeferred( s, rgbaColor, snoShader, eContext );
        return;
    }

    float L = u.Normalize();
    float h = L * 0.5f;
    float r = c.flRadius;
    PRTransform axialTransform;
    axialTransform.q.FromArc( cgwvZAxis, u );
    axialTransform.wp = 0.5f * ( c.wp1 + c.wp2 );

    PRTransform finalTransform = transform * axialTransform;

    const int32 cnPointCount = 12;
    const int32 cnHalfPointCount = cnPointCount / 2;
    const float cflRadPerPoint = FULL_CIRCLE / (float)cnPointCount;
    Vector3D wp[cnPointCount];


    for ( int32 i = 0; i < cnPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( r * Math::Cos( a ), r * Math::Sin( a ), -h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnPointCount - 1; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }
    sLine3DAdd( wp[cnPointCount - 1], wp[0], vcColor, snoShader, eContext );


    for ( int32 i = 0; i <= cnHalfPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( 0.0f, r * Math::Cos( a ), -r * Math::Sin( a ) - h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnHalfPointCount; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }


    for ( int32 i = 0; i <= cnHalfPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( r * Math::Cos( a ), 0.0f, -r * Math::Sin( a ) - h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnHalfPointCount; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }


    for ( int32 i = 0; i < cnPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( r * Math::Cos( a ), r * Math::Sin( a ), h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnPointCount - 1; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }
    sLine3DAdd( wp[cnPointCount - 1], wp[0], vcColor, snoShader, eContext );


    for ( int32 i = 0; i <= cnHalfPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( 0.0f, r * Math::Cos( a ), r * Math::Sin( a ) + h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnHalfPointCount; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }


    for ( int32 i = 0; i <= cnHalfPointCount; i++ )
    {
        float a = (float)i * cflRadPerPoint;
        wp[i].Set( r * Math::Cos( a ), 0.0f, r * Math::Sin( a ) + h );
        wp[i] = finalTransform * wp[i];
    }
    for ( int32 i = 0; i < cnHalfPointCount; i++ )
    {
        sLine3DAdd( wp[i], wp[i + 1], vcColor, snoShader, eContext );
    }


    Vector3D wp1, wp2;
    wp1 = finalTransform * Vector3D( -r, 0.0f, -h );
    wp2 = finalTransform * Vector3D( -r, 0.0f, h );
    sLine3DAdd( wp1, wp2, vcColor, snoShader, eContext );
    wp1 = finalTransform * Vector3D( r, 0.0f, -h );
    wp2 = finalTransform * Vector3D( r, 0.0f, h );
    sLine3DAdd( wp1, wp2, vcColor, snoShader, eContext );
    wp1 = finalTransform * Vector3D( 0.0f, -r, -h );
    wp2 = finalTransform * Vector3D( 0.0f, -r, h );
    sLine3DAdd( wp1, wp2, vcColor, snoShader, eContext );
    wp1 = finalTransform * Vector3D( 0.0f, r, -h );
    wp2 = finalTransform * Vector3D( 0.0f, r, h );
    sLine3DAdd( wp1, wp2, vcColor, snoShader, eContext );
}







































































































































































































































































































































































































































































static void sTriOverlayAdd(
    const Vector3D & wp0,
    const Vector3D & wp1,
    const Vector3D & wp2,
    const RGBAColor & rgbaColor,
    GlobalSNO snoShader,
    const RenderContext eContext )
{
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_UPDATE )];

    Vector3D wvNormal = Vector3D::CrossProduct( wp1 - wp0, wp2 - wp0 );
    wvNormal.Normalize();


    int32 nStart = (int32)tData.tTriOverlayVertices.size() / 3;
    Vertex1TexL tVert;
    tVert.vcDiffuse = VertexColor( rgbaColor );
    tVert.vcAuxColor = tVert.vcDiffuse;
    tVert.wvpNormal = Vector3DNormalToUByte4( wvNormal );
    tVert.vpUV0 = Vector2DTexCoordToUByte4( Vector2D( 0.0f, 0.0f ) );

    tVert.wpPos = wp0;
    tData.tTriOverlayVertices.push_back( tVert );

    tVert.wpPos = wp1;
    tData.tTriOverlayVertices.push_back( tVert );

    tVert.wpPos = wp2;
    tData.tTriOverlayVertices.push_back( tVert );

    if ( !tData.tTriOverlayShaderBatches.IsEmpty() ) {

        TriOverlayShaderBatch & tLastBatch = tData.tTriOverlayShaderBatches.GetTail();


        if ( tLastBatch.snoShader != snoShader ||
             tLastBatch.eContext != eContext ) {

            TriOverlayShaderBatch tNewBatch;
            tNewBatch.nStart = nStart;
            tNewBatch.nEnd = nStart + 1;
            tNewBatch.snoShader = snoShader;
            tNewBatch.eContext = eContext;

            tData.tTriOverlayShaderBatches.AddTail_Tracked( tNewBatch );
        } else {



            tLastBatch.nEnd += 1;
        }
    } else {




        TriOverlayShaderBatch nFirstBatch;
        nFirstBatch.nStart = nStart;
        nFirstBatch.nEnd = nStart + 1;
        nFirstBatch.snoShader = snoShader;
        nFirstBatch.eContext = eContext;

        tData.tTriOverlayShaderBatches.AddTail_Tracked( nFirstBatch );
    }
}





void GfxDrawTriOverlayDeferred( const Triangle3D & wtri, const RGBAColor & rgbaColor, GlobalSNO snoShader, const RenderContext eContext )




{
    sTriOverlayAdd( wtri.wp0, wtri.wp1, wtri.wp2, rgbaColor, snoShader, eContext );
}


















inline TriangleList2D::TriangleList2D() {
    m_nCount = 0;
    m_tVerts = new VertexNoTexL[256 * 3];
}





inline TriangleList2D::~TriangleList2D() {
    delete [] m_tVerts;
}





BOOL TriangleList2D::Add( const Triangle2D & wtri, const float flZ, const RGBAColor & rgbaColor ) {
    if ( m_nCount >= 256 ) {
        return FALSE;
    }

    VertexColor vcColor( rgbaColor );
    VertexNoTexL * ptVert = &m_tVerts[m_nCount * 3];

    ptVert[0].wpPos.Set( wtri.wp0.x, wtri.wp0.y, flZ );
    ptVert[0].vcDiffuse = vcColor;
    ptVert[1].wpPos.Set( wtri.wp1.x, wtri.wp1.y, flZ );
    ptVert[1].vcDiffuse = vcColor;
    ptVert[2].wpPos.Set( wtri.wp2.x, wtri.wp2.y, flZ );
    ptVert[2].vcDiffuse = vcColor;

    m_nCount++;
    return TRUE;
}










BOOL TriangleList2D::GetTriangle( int32 nIndex, Triangle2D & wtri ) const {
    if ( nIndex < 0 || nIndex >= m_nCount * 3 ) {
        return FALSE;
    }

    const VertexNoTexL * ptVert = &m_tVerts[nIndex * 3];
    wtri.wp0.Set( ptVert[0].wpPos.x, ptVert[0].wpPos.y );
    wtri.wp1.Set( ptVert[1].wpPos.x, ptVert[1].wpPos.y );
    wtri.wp2.Set( ptVert[2].wpPos.x, ptVert[2].wpPos.y );
    return TRUE;
}









void TriangleList2D::Draw() const {
    int32 nPassIndex = 0;
    SNO snoShader = GlobalSNOGet( GLOBALSNO_SHADER_2D_PRIMS_TRANSPARENT );
    GfxShaderInit( snoShader, nPassIndex );

    GfxWorldMatrixSet( cgm4x4Identity );
    GfxViewMatrixSet( cgm4x4Identity );
    Matrix4x4 mProj;
    GfxBuild2DUIProjectionMatrix( mProj );
    GfxProjectionMatrixSet( mProj );

    int32 nIndex = 0;
    uint32 dwVertsRemaining = m_nCount * 3;
    while ( dwVertsRemaining ) {
        uint32 dwVertCount = Math::Min<uint32>( dwVertsRemaining, GfxVBGetMaxCount( gptGfxData->vbidTriList2D ) );
        dwVertCount = ( dwVertCount / 3 ) * 3;
        uint32 dwStartIndex;
        GfxWriteToVB( gptGfxData->vbidTriList2D, dwVertCount, dwStartIndex, &m_tVerts[nIndex], dwVertCount * sizeof( VertexNoTexL ) );

        GfxShaderSetCullMode( CULLMODE_NONE );
        GfxShaderSetIB( -1, 0 );
        GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, gptGfxData->vbidTriList2D );
        GfxShaderDraw( DRAWMODE_TRIANGLELIST, dwStartIndex, 0, dwVertCount, 0, dwVertCount / 3, PERFBIN_UI );

        dwVertsRemaining -= dwVertCount;
        nIndex += dwVertCount;
    }

    CameraManagerRestoreCurrentCameraToMainCamera();
}








static Line2DShaderBatch * sGfxGetNextLine2DBatchToRender( POS & batchPOS ) {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    RenderContext eContext = GetRWindowMgr()->GetCurrentRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )].dispList.GetRenderContext();

    while ( batchPOS ) {
        Line2DShaderBatch & tCurrentBatch = tData.tLine2DShaderBatches.GetNext( batchPOS );
        if ( tCurrentBatch.eContext == eContext ) {
            return &tCurrentBatch;
        }
    }

    return NULL;
}








void Gfx2DPrimitivesDraw( const RenderLayer eRenderLayer ) {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    if ( eRenderLayer != RENDERLAYER_UI ) {
        return;
    }

    int32 nCount = (int32)tData.tLineList2DVertices.size();
    if ( nCount == 0 ) {
        return;
    }

    int32 nPassIndex = 0;
    SNO snoShader;
    Matrix4x4 mProj;
    int32 nBatchLineVertsDrawnSoFar = 0;
    POS pos = tData.tLine2DShaderBatches.GetHeadPosition();
    Line2DShaderBatch * pCurrentLineBatch = sGfxGetNextLine2DBatchToRender( pos );

    snoShader = GlobalSNOGet( GLOBALSNO_SHADER_2D_PRIMS_TRANSPARENT );
    GfxShaderInit( snoShader, nPassIndex );
    GfxWorldMatrixSet( cgm4x4Identity );
    GfxViewMatrixSet( cgm4x4Identity );
    GfxBuild2DUIProjectionMatrix( mProj );
    GfxProjectionMatrixSet( mProj );

    while ( pCurrentLineBatch && nCount > 0 ) {
        int32 nDrawCount = pCurrentLineBatch->GetVertexCount() - nBatchLineVertsDrawnSoFar;
        uint32 dwVertCount = Math::Min<int>( nDrawCount, GfxVBGetMaxCount( gptGfxData->vbidLineList2D ) );
        uint32 dwStartIndex;
        GfxWriteToVB( gptGfxData->vbidLineList2D, dwVertCount, dwStartIndex, &tData.tLineList2DVertices[pCurrentLineBatch->nStartLineVertexIndex + nBatchLineVertsDrawnSoFar], dwVertCount * sizeof( VertexNoTexL ) );
        GfxShaderSetIB( -1, 0 );
        GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, gptGfxData->vbidLineList2D );
        GfxShaderDraw( DRAWMODE_LINELIST, dwStartIndex, 0, dwVertCount, 0, dwVertCount / 2, PERFBIN_UI );

        nBatchLineVertsDrawnSoFar += dwVertCount;
        if ( nBatchLineVertsDrawnSoFar >= pCurrentLineBatch->GetVertexCount() ) {
            pCurrentLineBatch = sGfxGetNextLine2DBatchToRender( pos );
            nBatchLineVertsDrawnSoFar = 0;
        }
        nCount -= dwVertCount;
    }

    CameraManagerRestoreCurrentCameraToMainCamera();
}

















static Line3DShaderBatch * sGfxGetNextLine3DBatchToRender( POS & batchPOS, const RenderLayer eRenderLayer ) {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    RenderContext eContext = GetRWindowMgr()->GetCurrentRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )].dispList.GetRenderContext();

    while ( batchPOS ) {
        Line3DShaderBatch & tCurrentBatch = tData.tLine3DShaderBatches.GetNext( batchPOS );
        if ( tCurrentBatch.eContext == eContext ) {
            const ShaderDefinition * pdefShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast( GlobalSNOGet( tCurrentBatch.snoShader ) );
            RenderLayer eShaderRenderLayer = ( (RenderPass *)pdefShader->ptPasses )->eRenderLayer;
            if ( eShaderRenderLayer == eRenderLayer ) {
                return &tCurrentBatch;
            }
        }
    }

    return NULL;
}














void Gfx3DPrimitivesDraw( const RenderLayer eRenderLayer ) {
    SNO pointShader;
    const ShaderDefinition * pdefShader;
    RenderLayer eShaderRenderLayer;
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    {
        int32 nCount = (int32)tData.tLineList3DVertices.size();
        if ( nCount > 0 ) {
            int32 nBatchLineVertsDrawnSoFar = 0;
            Line3DShaderBatch * pCurrentLineBatch;
            POS pos;

            pos = tData.tLine3DShaderBatches.GetHeadPosition();
            pCurrentLineBatch = sGfxGetNextLine3DBatchToRender( pos, eRenderLayer );

            while ( pCurrentLineBatch && nCount > 0 ) {
                GfxWorldMatrixSet( cgm4x4Identity );

                int32 nDrawCount = pCurrentLineBatch->GetVertexCount() - nBatchLineVertsDrawnSoFar;
                int32 nPassIndex = 0;
                uint32 dwVertCount = Math::Min<int>( nDrawCount, GfxVBGetMaxCount( gptGfxData->vbidLineList3D ) );
                uint32 dwStartIndex;
                GfxWriteToVB( gptGfxData->vbidLineList3D, dwVertCount, dwStartIndex, &tData.tLineList3DVertices[pCurrentLineBatch->nStartLineVertexIndex + nBatchLineVertsDrawnSoFar], dwVertCount * sizeof( VertexNoTexL ) );
                GfxShaderInit( GlobalSNOGet( pCurrentLineBatch->snoShader ), nPassIndex );
                GfxShaderSetIB( -1, 0 );
                GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, gptGfxData->vbidLineList3D );
                GfxShaderDraw( DRAWMODE_LINELIST, dwStartIndex, 0, dwVertCount, 0, dwVertCount / 2, PERFBIN_UI );

                nBatchLineVertsDrawnSoFar += dwVertCount;
                if ( nBatchLineVertsDrawnSoFar >= pCurrentLineBatch->GetVertexCount() ) {
                    pCurrentLineBatch = sGfxGetNextLine3DBatchToRender( pos, eRenderLayer );
                    nBatchLineVertsDrawnSoFar = 0;
                }
                nCount -= dwVertCount;
            }
        }
    }

    pointShader = GlobalSNOGet( GLOBALSNO_SHADER_3D_PRIMS );
    pdefShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast( pointShader );
    eShaderRenderLayer = ( (RenderPass *)pdefShader->ptPasses )->eRenderLayer;
    if ( eShaderRenderLayer == eRenderLayer ) {
        int32 nCount = (int32)tData.tPointList3DVertices.size();
        int32 nIndex = 0;
        while ( nCount ) {
            GfxWorldMatrixSet( cgm4x4Identity );

            uint32 dwVertCount = Math::Min<int>( nCount, GfxVBGetMaxCount( gptGfxData->vbidPointList3D ) );
            uint32 dwStartIndex;
            int32 nPassIndex = 0;
            GfxWriteToVB( gptGfxData->vbidPointList3D, dwVertCount, dwStartIndex, &tData.tPointList3DVertices[nIndex], dwVertCount * sizeof( VertexNoTexL ) );
            GfxShaderInit( pointShader, nPassIndex );
            GfxShaderSetIB( -1, 0 );
            GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, gptGfxData->vbidPointList3D );
            GfxShaderDraw( DRAWMODE_POINTLIST, dwStartIndex, 0, dwVertCount, 0, dwVertCount, PERFBIN_UI );

            nCount -= dwVertCount;
            nIndex += dwVertCount;
        }
    }
}





















void GfxPrimitivesFlush() {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    // reset all vertex arrays and batches
    tData.tLineList2DVertices.clear();
    tData.tLineList3DVertices.clear();
    tData.tPointList3DVertices.clear();
    tData.tTriOverlayVertices.clear();
    tData.tLine2DShaderBatches.RemoveAll();
    tData.tLine3DShaderBatches.RemoveAll();
    tData.tTriOverlayShaderBatches.RemoveAll();
}





void GfxTriOverlaysDraw( const RenderLayer eRenderLayer ) {
    GfxRenderData & tData = gptGfxData->tRenderData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )];

    RenderContext eContext = GetRWindowMgr()->GetCurrentRWindow().tAsyncData[CGameAsyncGetRenderDataIndex( ASYNC_RENDER_DATA_DRAW )].dispList.GetRenderContext();

    POS pos = tData.tTriOverlayShaderBatches.GetHeadPosition();
    while ( pos ) {
        TriOverlayShaderBatch & tBatch = tData.tTriOverlayShaderBatches.GetNext( pos );
        if ( tBatch.eContext != eContext ) {
            continue;
        }

        SNO snoShader = GlobalSNOGet( tBatch.snoShader );
        const ShaderDefinition * pdefShader = (const ShaderDefinition *)gptGroupDefShader->LockRFast( snoShader );
        RenderLayer eShaderRenderLayer = ( (RenderPass *)pdefShader->ptPasses )->eRenderLayer;
        if ( eShaderRenderLayer != eRenderLayer ) {
            continue;
        }

        uint32 dwVertsRemaining = ( tBatch.nEnd - tBatch.nStart ) * 3;
        int32 nIndex = tBatch.nStart * 3;
        while ( dwVertsRemaining ) {
            GfxWorldMatrixSet( cgm4x4Identity );

            uint32 dwVertCount = Math::Min<uint32>( dwVertsRemaining, GfxVBGetMaxCount( gptGfxData->vbidTriOverlay ) );
            dwVertCount = ( dwVertCount / 3 ) * 3;
            uint32 dwStartIndex;
            int32 nPassIndex = 0;
            GfxWriteToVB( gptGfxData->vbidTriOverlay, dwVertCount, dwStartIndex, &tData.tTriOverlayVertices[nIndex], dwVertCount * sizeof( Vertex1TexL ) );
            GfxShaderInit( snoShader, nPassIndex );
            GfxShaderSetCullMode( CULLMODE_NONE );
            GfxShaderSetIB( -1, 0 );
            GfxShaderSetStream( STREAMINDEX_DEFAULT, 0, gptGfxData->vbidTriOverlay );
            GfxShaderDraw( DRAWMODE_TRIANGLELIST, dwStartIndex, 0, dwVertCount, 0, dwVertCount / 3, PERFBIN_UI );

            dwVertsRemaining -= dwVertCount;
            nIndex += dwVertCount;
        }
    }
}















TriangleList2D * GfxPrimitivesGetSharedTriList2D() {
    return gptGfxData->ptTriList2D;
}
