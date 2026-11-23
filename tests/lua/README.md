# Recovered Lua object behavior checks

Run from the integrated repository root:

```sh
clang -std=c11 -g -fsanitize=address,undefined -fno-sanitize=null -I src \
  tests/lua/objects_test.c \
  src/Contrib/Contrib/lua/src/lfunc.c src/Contrib/Contrib/lua/src/lstring.c \
  -o /tmp/d3-lua-objects-test
/tmp/d3-lua-objects-test
```

The allocation and GC-link functions are explicit test doubles. These checks exercise
real recovered string/closure/upvalue/prototype bodies; they do not test collector
scheduling, VM execution, allocation-failure long jumps, or byte matching.

The null sanitizer is disabled because upstream Lua 5.1.1's `ngcotouv` converts a
possibly null union pointer to its offset-zero member with `&((o)->uv)`. Modern
UBSan flags that idiom even though this path does not load from null.
AddressSanitizer and the remaining UndefinedBehaviorSanitizer checks pass.
