// Original file: X/Client/Physics/DominoPolytope.cpp
#include "../../../Contrib/Contrib/Domino/Include/dmPolytopeData.h"
#include "../../Sigma/Files/XSerializeData.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptor.h"
#include "../../Sigma/TypeDescriptor/TypeDescriptorField.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../Gfx/GfxVB.h"
#include <stddef.h>

// DWARF DominoPolytope.h:9; adopted shared header is not present.
struct PolytopeData : public dmPolytopeData {
    SerializeData serVertices;
    SerializeData serPlanes;
    SerializeData serSubEdges;
    SerializeData serFaceSubEdges;
};
static_assert(sizeof(PolytopeData) == 0x60, "PolytopeData");
// PhysMeshDefinition.cpp:105.
extern const XTypeDescriptor * DT_DOMINO_FLOAT3;

#line 16
static const TypeDescriptorField dmPlane_Fields[3] = {
    TypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(dmPlane, normal), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(dmPlane, offset), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(dmPlane), NULL, 0),
};
#line 19
ComplexTypeDescriptor TypeDescriptor_dmPlane("dmPlane", 0, dmPlane_Fields, 3, NULL, 0x80, NULL, NULL, NULL, NULL);
#line 19
const XTypeDescriptor * DT_DOMINO_PLANE = &TypeDescriptor_dmPlane;
#line 21
static const TypeDescriptorField dmSubEdge_Fields[5] = {
    TypeDescriptorField("", DT_BYTE, offsetof(dmSubEdge, twinOffset), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(dmSubEdge, tail), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(dmSubEdge, left), NULL, 1),
    TypeDescriptorField("", DT_BYTE, offsetof(dmSubEdge, next), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(dmSubEdge), NULL, 0),
};
#line 26
ComplexTypeDescriptor TypeDescriptor_dmSubEdge("dmSubEdge", 0, dmSubEdge_Fields, 5, NULL, 0x80, NULL, NULL, NULL, NULL);
#line 26
const XTypeDescriptor * DT_DOMINO_SUB_EDGE = &TypeDescriptor_dmSubEdge;
#line 28
static const TypeDescriptorField PolytopeData_Fields[15] = {
    TypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(PolytopeData, m_centroid), NULL, 1),
#line 31
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PolytopeData, serVertices), NULL, 0), VariableArrayTypeDescriptorField("", DT_DOMINO_FLOAT3, offsetof(PolytopeData, m_aVertices), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), (int32)offsetof(PolytopeData, serVertices) - (int32)offsetof(PolytopeData, m_aVertices)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PolytopeData, serPlanes), NULL, 0), VariableArrayTypeDescriptorField("", DT_DOMINO_PLANE, offsetof(PolytopeData, m_aPlanes), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), (int32)offsetof(PolytopeData, serPlanes) - (int32)offsetof(PolytopeData, m_aPlanes)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PolytopeData, serSubEdges), NULL, 0), VariableArrayTypeDescriptorField("", DT_DOMINO_SUB_EDGE, offsetof(PolytopeData, m_aSubEdges), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), (int32)offsetof(PolytopeData, serSubEdges) - (int32)offsetof(PolytopeData, m_aSubEdges)),
    TypeDescriptorField("", DT_SERIALIZEDATA, offsetof(PolytopeData, serFaceSubEdges), NULL, 0), VariableArrayTypeDescriptorField("", DT_BYTE, offsetof(PolytopeData, m_aFaceSubEdges), (1 << TDF_XML_SERIALIZATION) | (1 << TDF_XPTR64), (int32)offsetof(PolytopeData, serFaceSubEdges) - (int32)offsetof(PolytopeData, m_aFaceSubEdges)),
    TypeDescriptorField("", DT_INT, offsetof(PolytopeData, m_vertexCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PolytopeData, m_faceCount), NULL, 1),
    TypeDescriptorField("", DT_INT, offsetof(PolytopeData, m_subEdgeCount), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PolytopeData, m_volume), NULL, 1),
    TypeDescriptorField("", DT_FLOAT, offsetof(PolytopeData, m_area), NULL, 1),
    TypeDescriptorField(NULL, DT_NULL, sizeof(PolytopeData), NULL, 0),
};
#line 42
ComplexTypeDescriptor TypeDescriptor_PolytopeData("PolytopeData", 0, PolytopeData_Fields, 15, NULL, 0x80, NULL, NULL, NULL, NULL);
#line 42
const XTypeDescriptor * DT_POLYTOPE_DATA = &TypeDescriptor_PolytopeData;
