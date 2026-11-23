# Manifold geometry checks

Run `sh tests/domino/run_manifolds.sh` from the repository root on ARM64 macOS.

The suite exercises production PS/TS/CS/CC manifolds, GJK, polytope triangle setup
and destruction, shape initializers, and convex contact dispatch. It covers:

- Separated, warm-cache and deep-interior polytope–sphere paths, first-face ties,
  near-surface fallback, non-first face selection, and fourth-lane preservation.
- Triangle front/back rejection, coplanar contact, triangle IDs, and external-buffer
  lifetime through the actual polytope constructor/destructor.
- Capsule separation/contact/coincidence, both capsule–sphere degeneracy branches.
- Rotated and translated polytope contact output, persisted/new point impulses,
  aggregate impulses, capsule dispatch and separated-contact rejection.

`shape_fixture.cpp` supplies trapping definitions for unimplemented virtual shape
operations and unexercised PC/PP manifold branches. Any call to these aborts. The
fixture triangle input constructor initializes only type/radius; tested geometry
fields are set explicitly. Production geometry algorithms are not replaced.
These tests do not exercise allocator-backed teardown or duplicate-ID consumption
across multiple new points. They establish behavior, not binary/helper proof.
