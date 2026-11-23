# Supplemental sweep and time-of-impact checks

`time_of_impact.cpp` checks transform endpoints and interpolation, local-center
subtraction, advancing a sweep repeatedly (including non-increasing times),
quaternion antipodes, non-unit four-lane quaternion interpolation/advancement, stationary separated points, approaching points with the
recovered contact tolerance, and coincident points.

After the collision sources have been integrated, run on ARM64 macOS with Clang:

```sh
clang++ -std=c++11 -O2 -ffp-contract=off -Wl,-dead_strip -Isrc \
  tests/domino/time_of_impact.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmTimeOfImpact.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmHomingFunction.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp \
  src/Contrib/Contrib/Domino/Common/dmMath.cpp -o /tmp/domino-toi-test
/tmp/domino-toi-test
```

These analytic checks establish basic behavior only. They are not exact-match
proof and do not replace the Nintendo-target full build and relocation-aware checker.
