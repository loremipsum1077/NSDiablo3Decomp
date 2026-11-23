#include "Octree.h"

#include <stdlib.h>

#include "../Math/Intersect.h"
#include "../Math/Ray.h"

// Original descriptor tables are restored below using the adopted descriptor headers.

struct NodeOrder {
    float wdDistSqr;
    BOOL fLeaf;
    void* lpData;
};

// Stand-in for AABB::Set(const Vector3D wp0, const float wdSize) (X/Sigma/Math/Rect.inl:361), which
// ../Math/Rect.h does not declare yet.
static inline void sAABBSet(AABB& aabb, const Vector3D wp0, const float wdSize) {
    float wdExt = wdSize * 0.5f;
    aabb.wp.Set(wp0.x + wdExt, wp0.y + wdExt, wp0.z + wdExt);
    aabb.wvExt.Set(wdExt, wdExt, wdExt);
}





























































































































































































































































































































































































































static int32 sSortByDistance(const void* lpArg0, const void* lpArg1) {
    const NodeOrder* ptOrder0 = (const NodeOrder*)lpArg0;
    const NodeOrder* ptOrder1 = (const NodeOrder*)lpArg1;

    if (ptOrder0->wdDistSqr > ptOrder1->wdDistSqr) {
        return 1;
    }

    return -1;
}











void Octree::sWalkLinearRecursive(
	const LineSeg3D& wline,
	const OctreeNode* const ptNode,
	void* lpData,
	BOOL& fEndWalk,
	OctreeLeafCallback pfnLeafCallback) const
{
	if (fEndWalk)
	{
		return;
	}



















	float wdT;
	AABB aabbNode;
	sAABBSet(aabbNode, ptNode->tCube.wpOrigin, ptNode->tCube.wdSize);
	if (!Intersect::LineSeg3DAndAABB(wline, aabbNode, wdT))
	{
		return;
	}



	NodeOrder tNodeOrder[8];
	for (int32 i = 0; i < 8; i++)
	{
		uint32 dwChild = ptNode->pdwChildren[i];
		if (dwChild & 0x80000000)
		{
			OctreeLeaf * ptLeaf = &sGetLeaf(dwChild);


			float wdCenterOffset = ptLeaf->tCube.wdSize * 0.5f;
			Vector3D wp(
				ptLeaf->tCube.wpOrigin.x + wdCenterOffset,
				ptLeaf->tCube.wpOrigin.y + wdCenterOffset,
				ptLeaf->tCube.wpOrigin.z + wdCenterOffset);

			tNodeOrder[i].wdDistSqr = Vector3D::DistanceSquared(wp, wline.wr.wpOrigin);
			tNodeOrder[i].fLeaf = TRUE;
			tNodeOrder[i].lpData = ptLeaf;
		}
		else
		{
			OctreeNode * ptChild = &sGetNode(dwChild);


			float wdCenterOffset = ptChild->tCube.wdSize * 0.5f;
			Vector3D wp(
				ptChild->tCube.wpOrigin.x + wdCenterOffset,
				ptChild->tCube.wpOrigin.y + wdCenterOffset,
				ptChild->tCube.wpOrigin.z + wdCenterOffset);

			tNodeOrder[i].wdDistSqr = Vector3D::DistanceSquared(wp, wline.wr.wpOrigin);
			tNodeOrder[i].fLeaf = FALSE;
			tNodeOrder[i].lpData = ptChild;
		}
	}
	qsort(tNodeOrder, 8, sizeof(NodeOrder), sSortByDistance);

	for (int32 i = 0; i < 8; i++)
	{
		if (tNodeOrder[i].fLeaf)
		{
			OctreeLeaf * ptLeaf = (OctreeLeaf *)tNodeOrder[i].lpData;

			AABB aabb;
			sAABBSet(aabb, ptLeaf->tCube.wpOrigin, ptLeaf->tCube.wdSize);
			if (Intersect::LineSeg3DAndAABB(wline, aabb, wdT))
			{

				if (pfnLeafCallback && !pfnLeafCallback(ptLeaf, lpData, fEndWalk))
				{
					fEndWalk = TRUE;
				}
			}
		}
		else
		{
			OctreeNode * ptChild = (OctreeNode *)tNodeOrder[i].lpData;
			this->sWalkLinearRecursive(wline, ptChild, lpData, fEndWalk, pfnLeafCallback);
		}
	}
}






void Octree::WalkLinear(const LineSeg3D& wline, void* lpData, OctreeLeafCallback pfnLeafCallback) const {
    BOOL fEndWalk = FALSE;




    sWalkLinearRecursive(wline, ptNodes, lpData, fEndWalk, pfnLeafCallback);
}

#include "../TypeDescriptor/TypeDescriptor.h"
#include "../TypeDescriptor/TypeDescriptorField.h"
extern const XTypeDescriptor* DT_SERIALIZEDATA;
static const TypeDescriptorField OctreeCube_Fields[] = {
    TypeDescriptorField("", DT_VECTOR3D, offsetof(OctreeCube, wpOrigin), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(OctreeCube, wdSize), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OctreeCube), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OctreeCube("OctreeCube", 0, OctreeCube_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL);
const XTypeDescriptor* DT_OCTREE_CUBE = &TypeDescriptor_OctreeCube;
static const TypeDescriptorField OctreeNode_Fields[] = {
    TypeDescriptorField("", DT_OCTREE_CUBE, offsetof(OctreeNode, tCube), NULL, 1),
    FixedArrayTypeDescriptorField("", DT_INT, offsetof(OctreeNode, pdwChildren), NULL, 1, 8, 0),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OctreeNode), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OctreeNode("OctreeNode", 0, OctreeNode_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL);
const XTypeDescriptor* DT_OCTREE_NODE = &TypeDescriptor_OctreeNode;
static const TypeDescriptorField OctreeLeaf_Fields[] = {
    TypeDescriptorField("", DT_OCTREE_CUBE, offsetof(OctreeLeaf, tCube), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OctreeLeaf, nPrimitiveIndex), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OctreeLeaf, nPrimitiveCount), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OctreeLeaf), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OctreeLeaf("OctreeLeaf", 0, OctreeLeaf_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL);
const XTypeDescriptor* DT_OCTREE_LEAF = &TypeDescriptor_OctreeLeaf;
static const TypeDescriptorField OctreePrimitive_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(OctreePrimitive, dwUserData), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(OctreePrimitive, dwIndexListIndex), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(OctreePrimitive), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_OctreePrimitive("OctreePrimitive", 0, OctreePrimitive_Fields, 3, NULL, 0, NULL, NULL, NULL, NULL);
const XTypeDescriptor* DT_OCTREE_PRIMITIVE = &TypeDescriptor_OctreePrimitive;
static const TypeDescriptorField Octree_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(Octree, nMaxNodes), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Octree, nMaxLeaves), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Octree, nMaxPrimitives), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Octree, nNodeCount), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Octree, nLeafCount), NULL, 0),
    TypeDescriptorField("", DT_INT, offsetof(Octree, nPrimitiveCount), NULL, 0),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Octree, serNodes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_OCTREE_NODE, offsetof(Octree, ptNodes), 0x20, -8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Octree, serLeaves), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_OCTREE_LEAF, offsetof(Octree, ptLeaves), 0x20, -8),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(Octree, serPrimitives), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_OCTREE_PRIMITIVE, offsetof(Octree, ptPrimitives), 0x20, -8),
    TypeDescriptorField(NULL, DT_NULL, sizeof(Octree), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_Octree("Octree", 0, Octree_Fields, 13, NULL, 0, NULL, NULL, NULL, NULL);
const XTypeDescriptor* DT_OCTREE = &TypeDescriptor_Octree;
