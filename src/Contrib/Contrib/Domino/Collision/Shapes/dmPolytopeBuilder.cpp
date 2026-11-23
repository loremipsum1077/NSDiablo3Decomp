#include "../../Include/dmPolytopeData.h"
#include "../../Include/dmBuffer.h"
#include "../../Common/dmPool.h"
#include "../ConvexHull/dmHullBuilder.h"
#include "../ConvexHull/dmSimpleHull.h"

struct dmHashEdge
{
    dmSubEdge m_subEdges[2];
    dm_uint32 m_key;
    dm_int32 m_next;
    static dm_uint32 BuildKey(dm_uint32 vertex1, dm_uint32 vertex2)
    {
        return (vertex1 << 16) | vertex2;
    }
};
static_assert(sizeof(dmHashEdge) == 16, "dmHashEdge");

template<> dmPool<dmHashEdge>::dmPool() : m_pData(0), m_count(0), m_capacity(0), m_ceiling(0), m_freeList(-1), m_sortedHead(-1)
{
}
template<> dmPool<dmHashEdge>::~dmPool()
{
    dmFree(m_pData);
}
// Owner-local adaptation: the original HashEdge instantiation stores its index in m_next.
// DWARF member 07d13f46 is at +0xc; original bc0a54 stores the index there.
template<> dm_int32 dmPool<dmHashEdge>::Allocate()
{
    if (m_freeList == -1)
    {
        dm_int32 newCapacity = dmMax(2, m_capacity + (m_capacity >> 1));
        dmHashEdge* pNewData = (dmHashEdge*)dmAllocFunction(newCapacity * sizeof(dmHashEdge), 16, "../../..\\Domino/Common/dmPool.h", 174, "Allocate");
        memset(pNewData, 0, newCapacity * sizeof(dmHashEdge));
        memcpy(pNewData, m_pData, m_capacity * sizeof(dmHashEdge));
        m_freeList = m_capacity;
        for (dm_int32 i = m_capacity; i < newCapacity - 1; ++i) pNewData[i].m_next = i + 1;
        pNewData[newCapacity - 1].m_next = -1;
        m_sortedHead = m_capacity;
        dmFree(m_pData);
        m_pData = pNewData;
        m_capacity = newCapacity;
    }
    dm_int32 index = m_freeList;
    m_freeList = m_pData[index].m_next;
    m_pData[index].m_next = index;
    ++m_count;
    m_ceiling = dmMax(m_ceiling, index + 1);
    m_sortedHead = dmMax(m_sortedHead, index + 1);
    return index;
}

inline dm_uint32 dmHash(dm_uint32 key)  // inline: DWARF abstract DIE only, no concrete DIE (r06 test)
{
    key = ~key + (key << 15);
    key = (key ^ (key >> 12)) * 5;
    key = (key ^ (key >> 4)) * 2057;
    return key ^ (key >> 16);
}
template<class T, class K> class dmTableIterator;
template<class T, class K> class dmTable
{
public:
    dmTable() : m_pTable(0), m_capacity(0), m_count(0)
    {
    }
    ~dmTable()
    {
        dmFree(m_pTable);
    }
    void Initialize(dm_int32 capacity);
    T* Find(K key)
    {
        dm_uint32 mask = m_capacity - 1;
        dm_uint32 hash = dmHash(key) & mask;
        dm_int32 index = m_pTable[hash];
        while (index != -1)
        {
            if (m_pool.Get(index)->m_key == key) return m_pool.Get(index);
            index = m_pool.Get(index)->m_next;
        }
        return 0;
    }
    T* Create(K key)
    {
        T* p = Find(key);
        if (p) return p;
        dm_uint32 mask = m_capacity - 1;
        dm_uint32 hash = dmHash(key) & mask;
        dm_int32 index = m_pool.Allocate();
        p = m_pool.Get(index);
        p->m_key = key;
        p->m_next = m_pTable[hash];
        m_pTable[hash] = index;
        ++m_count;
        return p;
    }
    dm_int32 GetCount() const
    {
        return m_count;
    }
private:
    friend class dmTableIterator<T, K>;
    dmPool<T> m_pool;
    dm_int32* m_pTable;
    dm_int32 m_capacity;
    dm_int32 m_count;
};
template<class T, class K> void dmTable<T, K>::Initialize(dm_int32 capacity)
{
#line 128 "../../..\\Domino/Common/dmTable.h"
    dmAssert(m_pTable == NULL);
    dmAssert(capacity > 0);
    if ((capacity & (capacity - 1)) != 0)
    {
        dm_int32 x = capacity;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        capacity = x + 1;
    }
    m_capacity = capacity;
#line 149
    m_pTable = (dm_int32*)dmAlloc(m_capacity * sizeof(dm_int32));
    for (dm_int32 i = 0; i < m_capacity; ++i) m_pTable[i] = -1;
    m_pool.Create(m_capacity);
}
template<class T, class K> class dmTableIterator
{
public:
    dmTableIterator(dmTable<T,K>* pTable) : m_pPool(&pTable->m_pool), m_pTable(pTable->m_pTable), m_pCount(&pTable->m_count), m_capacity(pTable->m_capacity), m_index(0), m_item(-1)
    {
    }
    T* GetFirst()
    {
        m_index = -1;
        m_item = -1;
        return GetNext();
    }
    T* GetNext()
    {
        if (m_item != -1)
        {
            m_item = m_pPool->Get(m_item)->m_next;
            if (m_item != -1) return m_pPool->Get(m_item);
        }
        for (++m_index; m_index < m_capacity; ++m_index)
        {
            if (m_pTable[m_index] != -1) break;
        }
        if (m_index == m_capacity) return 0;
        m_item = m_pTable[m_index];
        return m_pPool->Get(m_item);
    }
private:
    dmPool<T>* m_pPool;
    dm_int32* m_pTable;
    dm_int32* m_pCount;
    dm_int32 m_capacity;
    dm_int32 m_index;
    dm_int32 m_item;
};

#line 24 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytopeBuilder.cpp"
static dm_vec4 sConvert(const dmFloat3& f)
{
    return dmVecSet(f.x, f.y, f.z);
}
static dmFloat3 sConvert(dm_vec4Arg v)
{
    dmFloat3 f;
    f.x = dmGetX(v);
    f.y = dmGetY(v);
    f.z = dmGetZ(v);
    return f;
}
void dmPolytopeData::Initialize()
{
    m_aVertices = 0;
    m_aPlanes = 0;
    m_aSubEdges = 0;
    m_aFaceSubEdges = 0;
    m_centroid.x = 0;
    m_centroid.y = 0;
    m_centroid.z = 0;
    m_vertexCount = 0;
    m_faceCount = 0;
    m_subEdgeCount = 0;
    m_volume = 0;
    m_area = 0;
}
dmPolytopeData::dmPolytopeData()
{
    Initialize();
}
void dmPolytopeData::Destroy()
{
    dmFree(m_aVertices);
    dmFree(m_aPlanes);
    dmFree(m_aSubEdges);
    dmFree(m_aFaceSubEdges);
    Initialize();
}
dmPolytopeData::~dmPolytopeData()
{
    Destroy();
}
dm_int32 dmPolytopeData::GetTwinIndex(dm_int32 edgeIndex) const
{
    dm_int32 offset = m_aSubEdges[edgeIndex].twinOffset;
    return edgeIndex + offset;
}
dm_int32 dmPolytopeData::GetNextEdgeIndex(dm_int32 edgeIndex) const
{
    return m_aSubEdges[edgeIndex].next;
}
dm_int32 dmPolytopeData::GetTailIndex(dm_int32 edgeIndex) const
{
    return m_aSubEdges[edgeIndex].tail;
}
dm_vec4 dmPolytopeData::GetTailVertex(dm_int32 edgeIndex) const
{
    return sConvert(m_aVertices[m_aSubEdges[edgeIndex].tail]);
}
#line 173
void dmPolytopeData::SetAsBox(const dmTransform& xf, dm_vec4Arg extents)
{
    dm_vec4 vertices[8];
    // Original minimum half-extent, bits 0x3d866219.
    dm_vec4 minh = dmV4(0.065616794f);
    dm_vec4 h = dmMax(extents, minh);
    dm_float32 hx = dmGetX(h);
    dm_float32 hy = dmGetY(h);
    dm_float32 hz = dmGetZ(h);

    vertices[0] = dmVecSet(-hx, -hy, -hz);
    vertices[1] = dmVecSet(hx, -hy, -hz);
    vertices[2] = dmVecSet(hx, -hy, hz);
    vertices[3] = dmVecSet(-hx, -hy, hz);
    vertices[4] = dmVecSet(-hx, hy, -hz);
    vertices[5] = dmVecSet(hx, hy, -hz);
    vertices[6] = dmVecSet(hx, hy, hz);
    vertices[7] = dmVecSet(-hx, hy, hz);

    for (dm_int32 i = 0; i < 8; ++i)
    {
        vertices[i] = dmMul(xf, vertices[i]);
    }

    bool success = SetAsHull(vertices, 8, 0, 0, 0);


}

#line 263
bool dmPolytopeData::SetAsHull(const dm_vec4* pPoints, dm_int32 count, dm_float32 weldTolerance, dm_float32 edgeTolerance, dm_float32 faceTolerance)
{
    Destroy();
    dmSimpleHullInput input;
    input.pPoints = pPoints;
    input.count = count;
    input.maxCount = 32;
    input.weldTolerance = weldTolerance;
    input.edgeTolerance = edgeTolerance;
    input.faceTolerance = faceTolerance;
    dmHullBuilder hull;
    bool success = dmCreateSimpleHull(&hull, &input);
    if (!success) return false;
    m_vertexCount = hull.GetVertexCount();
    m_faceCount = hull.GetFaceCount();
    dm_int32 edgeCount = m_vertexCount + m_faceCount - 2;
#line 289
    dmAssert(m_vertexCount <= e_maxVertices);
    dmAssert(m_faceCount <= e_maxFaces);
    dmAssert(edgeCount <= e_maxEdges);
#line 293
    m_aVertices = (dmFloat3*)dmAlloc(m_vertexCount * sizeof(dmFloat3));
    for (dm_int32 i = 0; i < m_vertexCount; ++i) m_aVertices[i] = sConvert(hull.GetVertex(i)->m_point);
    dm_int32 totalFaceVertexCount = 0;
    for (const dmHullFace* pFace = hull.GetFaceList(); pFace; pFace = pFace->m_pNext) totalFaceVertexCount += pFace->m_vertexCount;
#line 306
    m_aPlanes = (dmPlane*)dmAlloc(m_faceCount * sizeof(dmPlane));
#line 310
    dm_int32* faceVertexCounts = (dm_int32*)dmAlloc(m_faceCount * sizeof(dm_int32));
    dm_int32* faceVertexIndices = (dm_int32*)dmAlloc(totalFaceVertexCount * sizeof(dm_int32));
    dm_int32 faceVertexIndex = 0;
    dm_int32 faceIndex = 0;
    for (const dmHullFace* pFace = hull.GetFaceList(); pFace; pFace = pFace->m_pNext)
    {
        m_aPlanes[faceIndex].normal = sConvert(pFace->m_normal);
        m_aPlanes[faceIndex].offset = dmF32(pFace->m_planeOffset);
#line 318
        dmAssert(pFace->m_vertexCount <= dmPolytopeData::e_maxVertices);
        faceVertexCounts[faceIndex] = pFace->m_vertexCount;
        const dmHullHalfEdge* pEdge = pFace->m_pEdge;
        do
        {
#line 323
            dmAssert(pEdge->m_pVertex->m_index <= dmPolytopeData::e_maxVertices);
            faceVertexIndices[faceVertexIndex++] = pEdge->m_pVertex->m_index;
            pEdge = pEdge->m_pNext;
        } while (pEdge != pFace->m_pEdge);
        ++faceIndex;
    }
#line 332
    dmAssert(faceIndex == m_faceCount);
    ComputeEdges(faceVertexCounts, faceVertexIndices);
    dmFree(faceVertexCounts);
    dmFree(faceVertexIndices);
    ComputeCentroid();
    return true;
}

#line 387 "D:/BuildServer/2/work/code/branches/2_6_2/Contrib/Contrib/Domino/Collision/Shapes/dmPolytopeBuilder.cpp"
void dmPolytopeData::ComputeEdges(const dm_int32* faceVertexCounts, const dm_int32* pFaceVertexIndices)
{

	dm_int32 edgeCount = m_vertexCount + m_faceCount - 2;


	m_subEdgeCount = 2 * edgeCount;

	m_aSubEdges = (dmSubEdge*)dmAlloc(m_subEdgeCount * sizeof(dmSubEdge));
	m_aFaceSubEdges = (dm_uint8*)dmAlloc(m_faceCount * sizeof(dm_uint8));


	memset(m_aFaceSubEdges, 255, m_faceCount);


	dmTable<dmHashEdge, dm_uint32> table;
	table.Initialize(edgeCount);
	dm_int32 vertexIndex = 0;
	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{
		dm_int32 faceVertexCount = faceVertexCounts[i];
		const dm_int32* pFace = pFaceVertexIndices + vertexIndex;

		for (dm_int32 j = 0; j < faceVertexCount; ++j)
		{
			dm_int32 index1 = pFace[j];
			dm_int32 index2 = pFace[(j + 1) % faceVertexCount];

			dm_uint32 flippedKey = dmHashEdge::BuildKey(index2, index1);
			dmHashEdge* pEdge = table.Find(flippedKey);
			if (!pEdge)
			{
				dm_uint32 key = dmHashEdge::BuildKey(index1, index2);
				pEdge = table.Create(key);
				pEdge->m_key = key;


				pEdge->m_subEdges[0].next = 255;
				pEdge->m_subEdges[0].tail = index1;
				pEdge->m_subEdges[0].left = i;
				pEdge->m_subEdges[0].twinOffset = 1;


				pEdge->m_subEdges[1].next = 255;
				pEdge->m_subEdges[1].tail = index2;
				pEdge->m_subEdges[1].left = 255;
				pEdge->m_subEdges[1].twinOffset = -1;
			}
			else
			{

				pEdge->m_subEdges[1].left = i;
			}
		}

		vertexIndex += faceVertexCount;
	}

	dmAssert(table.GetCount() == edgeCount);


	dmTableIterator<dmHashEdge, dm_uint32> iterator(&table);
	dmHashEdge* pEdge = iterator.GetFirst();
	dm_int32 subEdgeIndex = 0;
	while (pEdge)
	{

		m_aSubEdges[subEdgeIndex++] = pEdge->m_subEdges[0];
		m_aSubEdges[subEdgeIndex++] = pEdge->m_subEdges[1];
		pEdge = iterator.GetNext();

	}



	for (dm_int32 i = 0; i < m_faceCount; ++i)
	{

		dmBuffer<dm_int32, 32> faceEdges;
		for (dm_int32 j = 0; j < edgeCount; ++j)
		{
			dm_int32 edgeIndex = 2 * j;
			dm_int32 twinIndex = edgeIndex + 1;
			dmSubEdge* pSubEdge = m_aSubEdges + edgeIndex;
			dmSubEdge* pTwinEdge = m_aSubEdges + twinIndex;

			if (pSubEdge->left == i)
			{
				faceEdges.PushBack(edgeIndex);
			}
			else if (pTwinEdge->left == i)
			{
				faceEdges.PushBack(twinIndex);
			}
		}

		dmAssert(faceEdges.GetCount() >= 3);


		dm_int32 edge1 = faceEdges[0];
		m_aFaceSubEdges[i] = edge1;
		dm_int32 maxCount = faceEdges.GetCount(), count = 1;
		while (count < maxCount)
		{
			dm_int32 twinEdge = GetTwinIndex(edge1);
			dm_int32 headVertex = GetTailIndex(twinEdge);
			for (dm_int32 j = 0; j < maxCount; ++j)
			{
				dm_int32 edge2 = faceEdges[j];
				dm_int32 tailVertex = GetTailIndex(edge2);
				if (tailVertex == headVertex)
				{
					m_aSubEdges[edge1].next = edge2;
					edge1 = edge2;
					++count;
					break;
				}
			}
		}


		m_aSubEdges[edge1].next = faceEdges[0];
	}
}
void dmPolytopeData::ComputeCentroid()
{
#line 516
    dmAssert(3 <= m_vertexCount && 1 <= m_faceCount);
    dm_vec4 integral1 = dmV4(0.0f);
    dm_vec4 integral2 = dmV4(0.0f);
    dm_vec4 area = dmV4(0.0f);
    const dm_vec4 oneOver6 = dmV4(1.0f / 6.0f);
    for (dm_int32 i = 0; i < m_faceCount; ++i)
    {
        dm_int32 faceEdge1 = m_aFaceSubEdges[i];
        dm_vec4 v1 = GetTailVertex(faceEdge1);
        dm_int32 faceEdge = GetNextEdgeIndex(faceEdge1);
        dm_vec4 v2 = GetTailVertex(faceEdge);
        faceEdge = GetNextEdgeIndex(faceEdge);
        do
        {
            dm_vec4 v3 = GetTailVertex(faceEdge);
            dm_vec4 e1 = v2 - v1;
            dm_vec4 e2 = v3 - v1;
            dm_vec4 d = dmCross(e1, e2);
            area += dmLength3(d);
            dm_vec4 f1 = v1 + v2 + v3;
            dm_vec4 f2 = v1 * v1 + v2 * (v1 + v2) + v3 * f1;
            integral1 += dmSplatX(d * f1);
            integral2 += d * f2;
            v2 = v3;
            faceEdge = GetNextEdgeIndex(faceEdge);
        } while (faceEdge != faceEdge1);
    }
    integral1 *= oneOver6;
    m_volume = dmF32(integral1);
    m_area = dmF32(area * dm_vec4_oneHalf);
    if (dmGTZ(integral1))
    {
        integral2 *= (dm_vec4_oneHalf * dm_vec4_oneHalf) * oneOver6;
        m_centroid = sConvert(integral2 / integral1);
    }
    else
    {
        m_centroid = sConvert(dmV4(0.0f));
    }
}
