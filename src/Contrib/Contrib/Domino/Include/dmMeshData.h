// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: Contrib/Contrib/Domino/Include/dmMeshData.h
// Subset: the mesh node, triangle and data layouts.

#pragma once

#include "dmSerialize.h"

struct dmMeshTriangle {  // size 0x1c
    dm_int32 i1;  // 0x00
    dm_int32 i2;  // 0x04
    dm_int32 i3;  // 0x08
    dm_int32 w1;  // 0x0c
    dm_int32 w2;  // 0x10
    dm_int32 w3;  // 0x14
    dm_uint16 materialIndex;  // 0x18
    dm_uint16 collisionFlags;  // 0x1a
};
static_assert(sizeof(dmMeshTriangle) == 0x1c, "dmMeshTriangle");

struct dmMeshNode {  // size 0x10
    const dmMeshNode * GetChild1() const;
    const dmMeshNode * GetChild2() const;
    dm_int16 lowerX;  // 0x00
    dm_int16 lowerY;  // 0x02
    dm_int16 lowerZ;  // 0x04
    dm_int16 upperX;  // 0x06
    dm_int16 upperY;  // 0x08
    dm_int16 upperZ;  // 0x0a
    union {  // size 0x4
        struct {  // size 0x4
            dm_uint32 axis : 2;  // 0x00
            dm_uint32 triangleCount : 4;  // 0x00
            dm_uint32 index : 26;  // 0x00
        };  // 0x00
        dm_uint32 data;  // 0x00
    };  // 0x0c
};
static_assert(sizeof(dmMeshNode) == 0x10, "dmMeshNode");

struct dmMeshData {  // size 0x50
    dmMeshNode * m_pRoot;  // 0x00
    dmFloat4 * m_aVertices;  // 0x08
    dmMeshTriangle * m_aTriangles;  // 0x10
    dmFloat3 m_center;  // 0x18
    dmFloat3 m_extents;  // 0x24
    dmFloat3 m_unquantize;  // 0x30
    dm_int32 m_nodeCount;  // 0x3c
    dm_int32 m_vertexCount;  // 0x40
    dm_int32 m_triangleCount;  // 0x44
    dm_int32 m_height;  // 0x48
    dm_int32 m_padding;  // 0x4c
};
static_assert(sizeof(dmMeshData) == 0x50, "dmMeshData");
