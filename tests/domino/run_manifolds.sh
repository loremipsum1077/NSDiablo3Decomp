#!/bin/sh
set -eu
# Run from the repository root on ARM64 macOS.
clang++ -std=c++11 -O2 -ffp-contract=off -Wl,-dead_strip -Isrc \
  tests/domino/manifolds.cpp tests/domino/shape_fixture.cpp \
  src/Contrib/Contrib/Domino/Dynamics/Contacts/dmConvexContact.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmSSManifold.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmCSManifold.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmCCManifold.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmPSManifold.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmTSManifold.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp \
  src/Contrib/Contrib/Domino/Collision/Shapes/dmShapes.cpp \
  src/Contrib/Contrib/Domino/Collision/Shapes/dmPolytope.cpp \
  src/Contrib/Contrib/Domino/Common/dmMath.cpp -o /tmp/domino-manifold-test
/tmp/domino-manifold-test
