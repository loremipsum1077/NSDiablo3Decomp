# Desktop subsystem harness

This is a separate native host build for exercising a small recovered Lua
subsystem. It does not use the root project's Nintendo Switch matching flags or
require the original game binary. From the repository root, with CMake and a
native C compiler installed:

```sh
cmake -S desktop -B /tmp/nsd3-desktop
cmake --build /tmp/nsd3-desktop
ctest --test-dir /tmp/nsd3-desktop --output-on-failure
```

The two deterministic tests compile the actual recovered sources directly:

- `lua_object_behavior` uses `lobject.c` to check numeric encodings, parsing,
  raw equality and chunk names. Its fixture defines fail-fast stand-ins for
  string allocation, stack growth and concatenation; those VM services are not
  exercised. See `tests/lua_runtime/object_test.c`.
- `lua_allocation_behavior` uses `lfunc.c` and `lstring.c` to check string
  interning and resizing, userdata, closures, upvalues and prototypes. Its
  fixture supplies allocation and GC-link test doubles; it is not a garbage
  collector or VM integration test. See `tests/lua/README.md` and
  `tests/lua/objects_test.c`.

Assertions remain enabled in these fixtures even in release builds. Passing them
shows that these selected cases work on the host; it does not establish full Lua
or game behavioral equivalence. This harness does not boot the game, provide a
desktop game executable, or implement platform services.
