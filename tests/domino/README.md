# Domino geometry checks

`distance.cpp` checks analytic point, segment, triangle and cube distances, overlap,
radius adjustment, warm-cache reuse and strict support-point tie handling. On an
ARM64 macOS host with Clang:

```sh
clang++ -std=c++11 -O2 -ffp-contract=off -Wl,-dead_strip -Isrc \
  tests/domino/distance.cpp \
  src/Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp \
  src/Contrib/Contrib/Domino/Common/dmMath.cpp -o /tmp/domino-distance-test
/tmp/domino-distance-test
```

The fixture supplies only the assertion hook. It is a supplemental behavioral test,
not evidence of matching Nintendo compiler output. Use `tools/try_file.py` for
provisional instruction comparisons and the main checkout's serial full build plus
`tools/check` for relocation-aware exact-match validation.

`convex_contact.cpp` checks shared convex-contact quaternion composition, inverse
transforms, manifold initialization and cache flags. Run with the command above,
substituting `convex_contact.cpp` and omitting `dmDistance.cpp`. These analytic
checks supplement original-caller validation; they do not prove binary matches.
