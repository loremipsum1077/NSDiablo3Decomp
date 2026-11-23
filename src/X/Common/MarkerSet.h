// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/MarkerSet.h

#pragma once

#include "../Sigma/DataStructures/XGrowableArray.h"
#include "Marker/Marker.h"

struct Circle;

typedef XGrowableArray<Marker, const Marker &> MarkerSet;

typedef XGrowableArray<Circle, const Circle &> NoSpawnSet;

typedef BOOL (*MarkerIterateCb)(const Marker *, void *);

