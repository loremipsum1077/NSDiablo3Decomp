# Native Lua API checks

The harness exercises recovered API behavior without requiring an operational
Lua VM. It covers stack order and growth, positive/negative/invalid indices,
pseudo-indices, numeric and boolean conversions, cross-thread moves, C closure
upvalue bounds and updates, the abort field, and GC control operations.

On macOS, after integrating the Lua owner headers, run from the repository root:

```sh
mkdir -p build/try/native_lapi
clang -std=c11 -fsanitize=address,undefined \
  -Isrc/Contrib/Contrib/lua/src -Wl,-dead_strip \
  tests/lua/lapi_native.c src/Contrib/Contrib/lua/src/lapi.c \
  -o build/try/native_lapi/check
build/try/native_lapi/check
```

Dead stripping removes untested API sections. The few remaining VM/allocator
stubs abort if reached; GC stubs record invocations and complete a collection
step deterministically. This tests API control behavior and memory access, not
the underlying collector or VM, and is separate from original-byte comparison.
