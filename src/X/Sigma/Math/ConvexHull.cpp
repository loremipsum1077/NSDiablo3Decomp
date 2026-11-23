// Original file: X/Sigma/Math/ConvexHull.cpp

#include "ConvexHull.h"

#include <stddef.h>
#include "Plane.h"
#include "../TypeDescriptor/TypeDescriptorField.h"
#include "../TypeDescriptor/TypeDescriptor.h"

// Defined in X/Sigma/Files/XFilesCommon.cpp:20; the header that declares it is not adopted yet.
extern const XTypeDescriptor * DT_SERIALIZEDATA;

static const TypeDescriptorField IndexedTriangle_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(IndexedTriangle, a), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(IndexedTriangle, b), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(IndexedTriangle, c), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(IndexedTriangle), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_IndexedTriangle("IndexedTriangle", 0, IndexedTriangle_Fields, 4, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_INDEXEDTRIANGLE = &TypeDescriptor_IndexedTriangle;

static const TypeDescriptorField ConvexHull_Fields[] = {
    TypeDescriptorField("", DT_INT, offsetof(ConvexHull, nVertexCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConvexHull, serVertices), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_VECTOR3D, offsetof(ConvexHull, ptVertices), 1, offsetof(ConvexHull, serVertices) - offsetof(ConvexHull, ptVertices)),
    TypeDescriptorField("", DT_INT, offsetof(ConvexHull, nTriangleCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConvexHull, serTriangles), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_INDEXEDTRIANGLE, offsetof(ConvexHull, ptTriangles), 1, offsetof(ConvexHull, serTriangles) - offsetof(ConvexHull, ptTriangles)),
    TypeDescriptorField("", DT_INT, offsetof(ConvexHull, nPlaneCount), NULL, 1),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(ConvexHull, serPlanes), NULL, 0),
    VariableArrayTypeDescriptorField("", DT_PLANE, offsetof(ConvexHull, ptPlanes), 1, offsetof(ConvexHull, serPlanes) - offsetof(ConvexHull, ptPlanes)),
    TypeDescriptorField("", DT_MATRIX3x3, offsetof(ConvexHull, mInertiaTensor), NULL, 1),
    TypeDescriptorField("", DT_VECTOR3D, offsetof(ConvexHull, wvCenterOfMass), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConvexHull, flArea), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(ConvexHull, flVolume), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(ConvexHull), NULL, 0),
};
ComplexTypeDescriptor TypeDescriptor_ConvexHull("ConvexHull", 0, ConvexHull_Fields, 14, NULL, 0, NULL, NULL, NULL, NULL); const XTypeDescriptor * DT_CONVEXHULL = &TypeDescriptor_ConvexHull;
