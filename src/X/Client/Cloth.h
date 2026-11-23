// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Client/Cloth.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "ClothUpdate.h"
#include "Gfx/GfxCommon.h"
#include "Gfx/GfxVB.h"
#include "../Common/CommonTypes.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"

struct Camera;
struct ClothDefinition;
struct ClothStaple;
struct ExplosionImpulse;
struct RActor;
class RWindow;
struct Sphere;
struct Vector3D;

enum {
    CLOTH_MAX_COUNT = 128,
    CLOTH_MAX_BONE_INFLUENCES = 3,
};

enum {
    CLOTHID_INVALID = -1,
};

typedef int32 ClothID;

struct ClothRenderData {  // size 0x18
    void * m_pAsyncVertices2TexL;  // 0x00
    void * m_pAsyncVerticesCloth;  // 0x08
    uint32 m_dwAsyncVertexCount;  // 0x10
};
static_assert(sizeof(ClothRenderData) == 0x18, "ClothRenderData");

class Cloth {  // size 0x328
    friend class ClothManager;
public:
    BOOL Create(RActor * ptRActor, int32 nSubObjectIndex, SNO snoCloth);
    void Destroy();
    static void Execute(void * pParam);
    void PreStep();
    void Step();
    void Settle();
    const Sphere & GetBoundingSphere() const;
    void DrawDebugView(RenderContext);
    const Vector3D & GetVertexPosition(uint16 wIndex) const;
    void CopyVertices(VBID, VBID, uint32 &);
    void CopyAsyncVertices(VBID vbid, VBID vbidModelSpacePos, uint32 & dwIBOffset);
    void CopyToAsyncVertices();
    void ApplyExplosion(const ExplosionImpulse & tExplosion);
    BOOL IsRendered() const;
    BOOL IsVisible(const Camera & camera) const;
    RenderContext GetRenderContext() const;
    BOOL IsPendingDeletion() const;
    void SetPendingDeletion();
    void DisablePlanes();
    float GetSoundData() const;
    ClothID id;  // 0x00
private:
    void CacheCollisionCapsules(const ClothDefinition * pdefCloth);
    void CacheCollisionPlanes(const ClothDefinition * pdefCloth);
    void CacheFatNormals();
    void Staple();
    uint32 dwLastFrameSubmitted;  // 0x04
    RActorID m_idRActor;  // 0x08
    int32 m_nSubObjectIndex;  // 0x0c
    SNO m_snoCloth;  // 0x10
    uint32 m_dwFlags;  // 0x14
    ClothUpdateParams m_tUpdateParams;  // 0x18
    int8 * m_pWorkSpace;  // 0x2b8
    ClothStaple * m_ptStaples;  // 0x2c0
    Vector3D * m_ptCachedFatNormals;  // 0x2c8
    ClothRenderData m_tRenderData[2];  // 0x2d0
    BOOL m_bAnimated;  // 0x300
    float m_flArea;  // 0x304
    float m_flPhaseShift;  // 0x308
    float m_flDeltaTime;  // 0x30c
    float m_flOldDeltaTime;  // 0x310
    RActor * m_ptRActor;  // 0x318
    uint32 m_dwDefFlags;  // 0x320
    AttachmentID m_idAttachment;  // 0x324
};
static_assert(sizeof(Cloth) == 0x328, "Cloth");

struct ClothCounters {  // size 0x14
    int32 nClothCount;  // 0x00
    int32 nVisibleCount;  // 0x04
    int32 nVertexCount;  // 0x08
    int32 nFaceCount;  // 0x0c
    int32 nEdgeCount;  // 0x10
};
static_assert(sizeof(ClothCounters) == 0x14, "ClothCounters");

class ClothManager {  // size 0x20
public:
    static void InitializeForGame();
    static void DisposeForGame();
    static ClothID Create(RActor * ptRActor, int32 nSubObjectIndex, SNO snoCloth);
    static void Destroy(ClothID id);
    static Cloth * Get(ClothID id);
    static Cloth * TryToGet(ClothID id);
    static void Update(float flDeltaTime, BOOL fFixedUpdateOnly);
    static void WaitForCompletion();
    static void CopyToAsyncVertices();
    static void DrawDebugView(RWindow &);
    static void IntersectSphere(const Sphere & tSphere, XPooledDataIDList & listCloth);
    static void GetCounters(ClothCounters *);
private:
    TDataArray<Cloth, int> * m_ptCloths;  // 0x00
    int32 m_nClothCount;  // 0x08
    int32 m_nVisibleCount;  // 0x0c
    int32 m_nVertexCount;  // 0x10
    int32 m_nFaceCount;  // 0x14
    int32 m_nEdgeCount;  // 0x18
};
static_assert(sizeof(ClothManager) == 0x20, "ClothManager");

// Line 278. Body from the copy inlined into AmbientSound::sGetJointVelocitySoundParams (ldr of 0x2a0).
inline float Cloth::GetSoundData() const { return m_tUpdateParams.tInputOutput.flAverageVelocity; }


// Functions defined in this file (bodies are not in the DWARF):
//   line   272  const Sphere & Cloth::GetBoundingSphere() const  [inlined into 1 functions]
//   line   278  float Cloth::GetSoundData() const  [inlined into 1 functions]

// Line 272. The Cloth.cpp CU's definition DIE carries no decl_file of its own, so it shares Cloth.h with the
// declaration; RActorGfx.cpp:975 reads the sphere at Cloth+0x2a4 with no call.
inline const Sphere & Cloth::GetBoundingSphere() const { return m_tUpdateParams.tInputOutput.wsBoundingSphere; }
