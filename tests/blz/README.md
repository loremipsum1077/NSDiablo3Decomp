Template checks for blz (not part of the build).

A class template has no code of its own, so it is checked through instantiations the binary
contains. Each file here includes a blz header, declares the argument types it needs, and
explicitly instantiates a template (or calls a function template) with the arguments the binary
uses. `python3 tools/try_file.py tests/blz/<file>.cpp` compares every emitted function whose
mangled name is in `data/2.6.2/functions.csv`.

Find instantiations to target with `grep _ZN3blz6vector data/2.6.2/functions.csv`.

`set_subscription.cpp` includes the production `SubscriptionEvent.h` and checks the
zero- and one-argument notification/removal templates and their callback trees. Run it
without `--json`: the text output includes matching symbols owned by other original
source files. This fixture is separate from the real achievements/party UI caller proofs
and does not promote functions in the full-build inventory.

## Conventions in src/Contrib/Contrib/blz

- Headers include each other by bare name (`#include "type_traits.h"`), each starts with
  `config.h`; the `.cpp` files use `"../include/blz/x.h"`; tests use
  `"Contrib/Contrib/blz/include/blz/x.h"`.
- Allocation goes through `blzext::alloc(size_t)` / `blzext::free(void*)` (declared in
  `mem_alloc.h`, defined by the game). `allocator::deallocate` has no null test.
- Containers inherit from their allocator (vector and basic_string `protected`, list and deque
  default access) rather than holding it.
- Trait dispatch tags are passed by the caller, e.g.
  `_destroy(f, l, typename is_trivially_destructible<T>::type())`.
- Call `blz::swap` qualified inside classes that have a member `swap`.
- `llvm-objdump -d -l --start-address=... --stop-address=... build/romfs/DiabloIIINX64r.nss`
  prints the original file:line of every instruction. For an inlined helper this shows how its
  body was laid out; it is the best evidence there is for code that has no out-of-line copy.
- clang 5 rejects explicit template arguments for function templates with parameter packs;
  let them be deduced in tests.
