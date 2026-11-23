// Host-only link fixtures for unimplemented virtual operations. Every body traps;
// a passing manifold test must never call any of these stand-ins.
#include "Contrib/Contrib/Domino/Include/dmPolytope.h"
#include "Contrib/Contrib/Domino/Include/dmCollision.h"
#include "Contrib/Contrib/Domino/Common/dmPoolAllocator.h"
#include <cstdlib>

dmAABB dmShape::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const { std::abort(); }
void dmShape::ComputeMass(dmMassData * pMassData, dm_float32 density) const { std::abort(); }
dm_vec4 dmShape::ComputeCentroid() const { std::abort(); }
dm_vec4 dmShape::ComputeSurfaceArea() const { std::abort(); }
dm_vec4 dmShape::ComputeVolume() const { std::abort(); }
dm_vec4 dmShape::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const { std::abort(); }
dm_vec4 dmShape::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const { std::abort(); }
void dmShape::ApplyScale(dm_vec4Arg scale) { std::abort(); }
dmAABB dmSphere::GetAABB(const dmMtx & xf) const { std::abort(); }
dmAABB dmSphere::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const { std::abort(); }
bool dmSphere::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const { std::abort(); }
void dmSphere::ComputeMass(dmMassData * pMassData, dm_float32 density) const { std::abort(); }
dm_vec4 dmSphere::ComputeCentroid() const { std::abort(); }
dm_vec4 dmSphere::ComputeSurfaceArea() const { std::abort(); }
dm_vec4 dmSphere::ComputeVolume() const { std::abort(); }
dm_vec4 dmSphere::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const { std::abort(); }
dm_vec4 dmSphere::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const { std::abort(); }
bool dmSphere::TestOverlap(const dmSphere & sphere) const { std::abort(); }
void dmSphere::ApplyScale(dm_vec4Arg scale) { std::abort(); }
dmAABB dmCapsule::GetAABB(const dmMtx & transform) const { std::abort(); }
dmAABB dmCapsule::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const { std::abort(); }
bool dmCapsule::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const { std::abort(); }
void dmCapsule::ComputeMass(dmMassData * pMassData, dm_float32 density) const { std::abort(); }
dm_vec4 dmCapsule::ComputeCentroid() const { std::abort(); }
dm_vec4 dmCapsule::ComputeSurfaceArea() const { std::abort(); }
dm_vec4 dmCapsule::ComputeVolume() const { std::abort(); }
dm_vec4 dmCapsule::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const { std::abort(); }
dm_vec4 dmCapsule::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & transform, dm_vec4Arg normal, dm_vec4Arg offset) const { std::abort(); }
bool dmCapsule::TestOverlap(const dmSphere & sphere) const { std::abort(); }
void dmCapsule::ApplyScale(dm_vec4Arg scale) { std::abort(); }
dmAABB dmTriangle::GetAABB(const dmMtx & transform) const { std::abort(); }
dmAABB dmTriangle::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const { std::abort(); }
dm_vec4 dmTriangle::ComputeCentroid() const { std::abort(); }
bool dmTriangle::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & transform) const { std::abort(); }
bool dmTriangle::TestOverlap(const dmSphere & sphere) const { std::abort(); }
void dmPolytope::ComputeMass(dmMassData * pMassData, dm_float32 density) const { std::abort(); }
dm_vec4 dmPolytope::ComputeCentroid() const { std::abort(); }
dm_vec4 dmPolytope::ComputeSurfaceArea() const { std::abort(); }
dm_vec4 dmPolytope::ComputeVolume() const { std::abort(); }
dm_vec4 dmPolytope::ComputeProjectedSurfaceArea(dm_vec4Arg dir) const { std::abort(); }
dm_vec4 dmPolytope::ComputeSubmergedVolume(dm_vec4 * pCentroid, const dmTransform & xf, dm_vec4Arg normal, dm_vec4Arg offset) const { std::abort(); }
dmAABB dmPolytope::GetAABB(const dmMtx & xf) const { std::abort(); }
dmAABB dmPolytope::GetSweptAABB(const dmMtx & xf1, const dmMtx & xf2) const { std::abort(); }
bool dmPolytope::RayCast(dmRayCastOutput * pOutput, const dmRayCastInput * pInput, const dmTransform & xf) const { std::abort(); }
bool dmPolytope::TestOverlap(const dmSphere & sphere) const { std::abort(); }
void dmPolytope::ApplyScale(dm_vec4Arg scale) { std::abort(); }
void dmPoolAllocator::Free(void*, dm_int32, bool) { std::abort(); }
void dmFree(void*) { std::abort(); }

// Unimplemented manifold branches are deliberately forbidden in this suite.
#include "Contrib/Contrib/Domino/Collision/Functions/dmManifold.h"
void dmPCManifold(dmManifold*, dmGJKCache*, const dmPolytope*, const dmCapsule*) { std::abort(); }
void dmPPManifold(dmManifold*, dmSATCache*, const dmPolytope*, const dmPolytope*, const dmTransform&) { std::abort(); }
