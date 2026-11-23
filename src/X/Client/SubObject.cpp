// Original file: X/Client/SubObject.cpp

#include "SubObject.h"
#include "Definitions/AppearanceDefinition.h"
#include "Gfx/GfxVB.h"
#include "../Sigma/GfxColorFormats.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Sigma/RGBAColor.h"

#define PACK_GEOMETRY (1 << SUBOBJECT_PACK_GEOMETRY_BIT)
#define PACK_SKIN (1 << SUBOBJECT_PACK_SKIN_BIT)
#define UNSTRIPIFIED (1 << SUBOBJECT_UNSTRIPIFIED_BIT)



































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































void SubObjectPostprocessDisposeFaceAdjacency(SubObject & tSubObject)
{
    SubObjectPostData * ptPostData = tSubObject.ptPostprocessed;


    if (ptPostData)
    {




        SigmaMemoryFree(ptPostData->ptFaceList, NULL); ptPostData->ptFaceList = NULL;
        SigmaMemoryFree(ptPostData->ptVertexOwnerList, NULL); ptPostData->ptVertexOwnerList = NULL;
    }

}



int32 SubObjectGetNumTriangles(const SubObject & tSubObject)
{



    if (tSubObject.dwFlags & PACK_SKIN)
    {
        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
        int32 nNumTriangles = 0;
        for (int32 i = 0; i < nNumSegments; i++)
        {
            if (tSubObject.dwFlags & UNSTRIPIFIED)
            {
                nNumTriangles += tSubObject.ptSkinningInfo[i].nPackIndexCount / 3;
            }
            else
            {
                nNumTriangles += tSubObject.ptSkinningInfo[i].nPackIndexCount - 2;
            }
        }
        return nNumTriangles;
    }
    else
    {

        if (tSubObject.dwFlags & UNSTRIPIFIED)
        {
            return tSubObject.nIndexCount / 3;
        }
        else
        {

            return tSubObject.nIndexCount - 2;
        }
    }
}

void SubObjectGetTriangleSegment(const SubObject & tSubObject,
                                 const int32 nTriangleIndex,
                                 int32 & nSegmentIndex,
                                 int32 & nIndexListIndex)
{
    int32 nSegment = 0;
    int32 nTriangle = nTriangleIndex;

    if (tSubObject.dwFlags & PACK_SKIN)
    {
        int32 nNumSegments = SubObjectGetNumSegments(tSubObject);
        int32 nTriangleBase = 0;
        for (nSegment = 0; nSegment < nNumSegments; nSegment++)
        {
            int32 nSegmentTriangles;
            if (tSubObject.dwFlags & UNSTRIPIFIED)
            {
                nSegmentTriangles = tSubObject.ptSkinningInfo[nSegment].nPackIndexCount / 3;
            }
            else
            {
                nSegmentTriangles = tSubObject.ptSkinningInfo[nSegment].nPackIndexCount - 2;
            }
            nTriangle = nTriangleIndex - nTriangleBase;
            if (nTriangle < nSegmentTriangles)
            {
                break;
            }
            nTriangleBase += nSegmentTriangles;
        }
        if (nSegment >= nNumSegments)
        {
            return;
        }
    }

    nSegmentIndex = nSegment;
    nIndexListIndex = (tSubObject.dwFlags & UNSTRIPIFIED) ? nTriangle * 3 : nTriangle;
}

















int32 SubObjectGetNumSegments(const SubObject & tSubObject)
{




    if (tSubObject.dwFlags & PACK_SKIN)
    {
        return tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);
    }




    return 1;
}



int32 SubObjectGetNumSegmentTriangles(const SubObject & tSubObject,
                                      int32 nSegmentIndex)
{
    if (tSubObject.dwFlags & PACK_SKIN)
    {
        const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];
        if (tSubObject.dwFlags & UNSTRIPIFIED)
        {
            return tSkinningInfo.nPackIndexCount / 3;
        }
        return tSkinningInfo.nPackIndexCount - 2;
    }
    else
    {
        if (tSubObject.dwFlags & UNSTRIPIFIED)
        {
            return tSubObject.nIndexCount / 3;
        }
        return tSubObject.nIndexCount - 2;
    }
}

















int32 SubObjectGetNumSegmentVertices(const SubObject & tSubObject,
                                     int32 nSegmentIndex)
{


    if (tSubObject.dwFlags & PACK_GEOMETRY)
    {
        if (tSubObject.dwFlags & PACK_SKIN)
        {
            return tSubObject.ptSkinningInfo[nSegmentIndex].nPackVertCount;
        }
        else
        {





            return tSubObject.nPackVertCount;
        }
    }
    else
    {

        return tSubObject.nVertCount;
    }
}



void SubObjectGetSegmentIndexListWalkInfo(const SubObject & tSubObject,
                                          int32 nSegmentIndex,
                                          int32 & nIndexListIndexCount,
                                          int32 & nIndexListIndexIncrement)
{
    if (tSubObject.dwFlags & PACK_SKIN)
    {
        const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];
        if (tSubObject.dwFlags & UNSTRIPIFIED)
        {
            nIndexListIndexCount = tSkinningInfo.nPackIndexCount;
            nIndexListIndexIncrement = 3;
        }
        else
        {
            nIndexListIndexCount = tSkinningInfo.nPackIndexCount - 2;
            nIndexListIndexIncrement = 1;
        }
    }
    else
    {
        if (tSubObject.dwFlags & UNSTRIPIFIED)
        {
            nIndexListIndexCount = tSubObject.nIndexCount;
            nIndexListIndexIncrement = 3;
        }
        else
        {
            nIndexListIndexCount = tSubObject.nIndexCount - 2;
            nIndexListIndexIncrement = 1;
        }
    }
}













void SubObjectGetSegmentTriangle(const SubObject & tSubObject,
                                 int32 nSegmentIndex,
                                 int32 nIndexListIndex,
                                 uint16 & wIndex0,
                                 uint16 & wIndex1,
                                 uint16 & wIndex2)
{


    if (tSubObject.dwFlags & PACK_SKIN)
    {






        const uint16 * pwIndexList = (const uint16 *)tSubObject.ptSkinningInfo[nSegmentIndex].pPackIndexList.Get();

        wIndex0 = pwIndexList[nIndexListIndex];
        wIndex1 = pwIndexList[nIndexListIndex + 1];
        wIndex2 = pwIndexList[nIndexListIndex + 2];
    }
    else
    {

















        wIndex0 = tSubObject.pwIndexList[nIndexListIndex];
        wIndex1 = tSubObject.pwIndexList[nIndexListIndex + 1];
        wIndex2 = tSubObject.pwIndexList[nIndexListIndex + 2];
    }
}



void SubObjectGetSegmentVertexPos(const SubObject & tSubObject,
                                  int32 nSegmentIndex,
                                  uint16 wVertexIndex,
                                  Vector3D & wpPos)
{

    if (tSubObject.dwFlags & PACK_GEOMETRY)
    {
        int16 nX, nY, nZ;
        if (!(tSubObject.dwFlags & PACK_SKIN))
        {





            const Vertex2TexLCPos * ptVert = (const Vertex2TexLCPos *)tSubObject.pPackVertList.Get() + wVertexIndex;
            nX = ptVert->nX; nY = ptVert->nY; nZ = ptVert->nZ;
        }
        else
        {







            const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];
            const VertexSkinCPos * ptVert = (const VertexSkinCPos *)tSkinningInfo.pPackVertList.Get() + wVertexIndex;
            nX = ptVert->nX; nY = ptVert->nY; nZ = ptVert->nZ;
        }


        wpPos.x = (float)nX * tSubObject.wpFixedPointPosScale.x + tSubObject.wpFixedPointPosOffset.x;
        wpPos.y = (float)nY * tSubObject.wpFixedPointPosScale.y + tSubObject.wpFixedPointPosOffset.y;
        wpPos.z = (float)nZ * tSubObject.wpFixedPointPosScale.z + tSubObject.wpFixedPointPosOffset.z;
    }
    else
    {














        wpPos = tSubObject.ptVertList[wVertexIndex].wpPos;
    }
}



void SubObjectGetSegmentVertexUV(const SubObject & tSubObject,
                                 int32 nSegmentIndex,
                                 uint16 wVertexIndex,
                                 int32 nUVSetIndex,
                                 Vector2D & vecUV)
{
    if (tSubObject.dwFlags & PACK_GEOMETRY)
    {
        if (!(tSubObject.dwFlags & PACK_SKIN))
        {






            const Vertex2TexLCPos * ptVert = (const Vertex2TexLCPos *)tSubObject.pPackVertList.Get() + wVertexIndex;

            if (nUVSetIndex != 1)
            {
                vecUV = UByte4TexCoordToVector2D(ptVert->vpUV0);
            }
            else
            {

                vecUV = UByte4TexCoordToVector2DLM(ptVert->vpUV1);
            }
        }
        else
        {




            const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];
            const VertexSkinCPos * ptVert = (const VertexSkinCPos *)tSkinningInfo.pPackVertList.Get() + wVertexIndex;

            if (nUVSetIndex != 1)
            {
                vecUV = UByte4TexCoordToVector2D(ptVert->vpUV0);
            }
            else
            {

                vecUV = UByte4TexCoordToVector2DLM(ptVert->vpUV1);
            }
        }
    }
    else
    {

























        UByte4 vpUV = tSubObject.ptVertList[wVertexIndex].vpUVSet[nUVSetIndex];

        if (nUVSetIndex != 1)
        {
            vecUV = UByte4TexCoordToVector2D(vpUV);
        }
        else
        {

            vecUV = UByte4TexCoordToVector2DLM(vpUV);
        }
    }
}


void SubObjectGetSegmentVertexColor(const SubObject & tSubObject,
                                    int32 nSegmentIndex,
                                    uint16 wVertexIndex,
                                    int32 nColorSetIndex,
                                    RGBAColor & rgbaColor)
{
    if (tSubObject.dwFlags & PACK_GEOMETRY)
    {
        const Vertex2TexLCPos * ptVert;
        if (!(tSubObject.dwFlags & PACK_SKIN))
        {






            ptVert = (const Vertex2TexLCPos *)tSubObject.pPackVertList.Get() + wVertexIndex;
        }
        else
        {














            const SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];
            ptVert = (const Vertex2TexLCPos *)((const VertexSkinCPos *)tSkinningInfo.pPackVertList.Get() + wVertexIndex);
        }






        VertexColor vcColor = (nColorSetIndex == 1) ? ptVert->vcAuxColor : ptVert->vcDiffuse;



        rgbaColor.r = vcColor.Red();
        rgbaColor.g = vcColor.Green();
        rgbaColor.b = vcColor.Blue();
        rgbaColor.a = vcColor.Alpha();
    }
    else
    {
































        const FatVertex & tVert = tSubObject.ptVertList[wVertexIndex];

        rgbaColor = tVert.rgbaColor[nColorSetIndex];
    }
}

// Two early-return ifs inferred from the line table (tests on rows 1655 and 1660, closing brace 1667);
// the single `||` return gave (plain | comp) instead of the original (comp | plain).
BOOL SubObjectHasVertexInfluences(const SubObject & tSubObject)
{








    if (tSubObject.ptInfluenceList != NULL)
    {
        return TRUE;
    }

    if (tSubObject.ptCompInfluenceList != NULL)
    {
        return TRUE;
    }

    return FALSE;

}




void SubObjectGetSegmentVertexInfluences(
    const SubObject & tSubObject,
    int32 nSegmentIndex,
    uint16 wVertexIndex,
    VertInfluences & tInfluences)
{

    if (tSubObject.dwFlags & PACK_GEOMETRY)
    {
        if (tSubObject.dwFlags & PACK_SKIN)
        {
            int32 nSkinningInfoCount = tSubObject.serSkinningInfo.dwSize / sizeof(SkinningInfo);  // DWARF local (1683), no code; initialiser inferred from SubObjectGfx.cpp:2208


            SkinningInfo & tSkinningInfo = tSubObject.ptSkinningInfo[nSegmentIndex];

            const VertexSkinCPos * ptVertList = (const VertexSkinCPos *)tSkinningInfo.pPackVertList.Get();
            const VertexSkinCPos & tVertex = ptVertList[wVertexIndex];

            int32 nIndex0 = tVertex.dwMatrixIndices & 0xff;

            int32 nIndex1 = (tVertex.dwMatrixIndices >> 8) & 0xff;

            int32 nIndex2 = (tVertex.dwMatrixIndices >> 16) & 0xff;


            tInfluences.tBoneInfluence[0].nBoneIndex = tSkinningInfo.pBoneIDs[nIndex0];
            tInfluences.tBoneInfluence[0].flWeight = (float)tVertex.vMatrixWeights.a * (1.0f / 255.0f);
            tInfluences.tBoneInfluence[1].nBoneIndex = tSkinningInfo.pBoneIDs[nIndex1];
            tInfluences.tBoneInfluence[1].flWeight = (float)tVertex.vMatrixWeights.b * (1.0f / 255.0f);
            tInfluences.tBoneInfluence[2].nBoneIndex = tSkinningInfo.pBoneIDs[nIndex2];
            tInfluences.tBoneInfluence[2].flWeight = (float)tVertex.vMatrixWeights.g * (1.0f / 255.0f);
        }
    }
    else
    {




        if (tSubObject.ptInfluenceList)
        {
            tInfluences = tSubObject.ptInfluenceList[wVertexIndex];
        }

        else if (tSubObject.ptCompInfluenceList)
        {
            const CompVertInfluences & tCompInfluence = tSubObject.ptCompInfluenceList[wVertexIndex];

            for (int i = 0; i < 3; i++)
            {
                tInfluences.tBoneInfluence[i].nBoneIndex = tCompInfluence.tBoneInfluence[i].dwBoneIndex;
                tInfluences.tBoneInfluence[i].flWeight = (float)tCompInfluence.tBoneInfluence[i].dwWeight;
            }
        }
    }
}
