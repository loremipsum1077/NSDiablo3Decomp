# Preserve selected weak providers while recovering game source

## Concrete integration failure

The isolated `UIActorTagControl` draft (`48555a73dd055b0dbd7156bf5f9cb389fe9dd694`,
based on `e3f2d1c`) reconstructs five functions totaling 844 original bytes.
Its stock compiler output has the original sizes and no differences outside
relocation fields. One strict object result is exact; four remain unsupported.
Those observations are not full-owner or exact-binary acceptance.

Adding that object in the existing CMake lexical order makes its 148-byte weak
`UIText::HitTest` replace the previously selected 288-byte copy from
`UITextControl.cpp`. A retained-object relink reproduced the stock ELF first,
then checked all 36,559 original identities. All five new target rows reported
OK, but the old HitTest row regressed from OK to a size mismatch.

This is a legitimate difference between compilation units. Original actor DWARF
records a HitTest instance with `low_pc=0` and a 148-byte extent, consistent with
a discarded copy. The selected original 288-byte instance contains nested
inlining of helpers defined in `UITextControl.cpp`; the candidate actor copy
calls one of those helpers. The discarded original bytes are unavailable, so the
148-byte extent is not a byte-equality proof. Changing shared headers to force
these copies to look alike is not supported by this evidence.

## Build contract

`toolchain/weak-providers.json` records 149 observed stock group contributors.
There are 148 duplicated signatures in the stock inputs; HitTest is unique there
and becomes the 149th duplicate with the actor object. This registry preserves
reviewed build behavior. It does **not** assert original link order or resolve
owner-attribution debt.

`toolchain/weak_provider_link.py` parses the supplied relocatable objects, requires
the designated providers, derives precedence constraints and uses deterministic
stable topological ordering. It preserves the incoming order when the constraints
already hold. Unknown duplicate groups, absent providers, conflicting definitions,
unsupported aliases and cycles fail rather than silently select another body.
Ordinary competing definitions and initialization-bearing inputs also retain their
relative ordering. This does not establish binding preservation for unrelated
non-COMDAT definitions: a new strong definition can override an old weak one
regardless of ordering. Existing full validation must still detect such changes. The supported recipe remains the existing object-only
`ld.lld -shared -Bsymbolic --no-demangle` link; archive extraction, section garbage
collection and identical-code folding are outside this rule's supported scope.

CMake supplies the same compiler options and linker flags. It removes stale
success metadata before linking, runs the wrapper, then records the existing map
provenance. The helper verifies actual mapped contributors before publishing
`d3.order.json`. That file is diagnostic output: the existing receipt format does
not retain or independently replay it. Do not present it as sealed receipt evidence
or use it to waive checker, source, old-consumer, data, initializer, unwind or
linked-endpoint obligations. Existing receipts bind the committed CMake/toolchain
source and their normal artifacts; source recovery still uses the ordinary gates.

Changing a provider requires review of the new body, all affected callers and data,
including already recovered functions. A planner failure is not permission to
remove a constraint. The registry's reference hashes identify the
baseline and counterfactual experiment; they do not make those diagnostics receipts.

## Implementation validation

Eleven focused parser/planner tests and independent review cover unknown and missing
providers, conflicting definitions, aliases, cycles and preserved initializer order.
The actual wrapper then reproduced three reference ELF/map/provenance triples
byte for byte: stock normal, stock Lua and the actor counterfactual. Configure-only
controls produced the same 734 compiler commands in both modes, with only the
intended linker graph/dependency changes. No game compilation was needed for those
controls. Full source recovery still requires fresh ordinary receipts.

The actual-wrapper result SHA-256 is
`233d9f332b824230d255a6e7d877bf7675a725e807a39d98a6d0916608ec7725`;
configure result SHA-256 is
`5886987e34620ee95e138763371b184882d48ac1252475f1dce5b5e9300254ef`.
Final independent scoped clearance SHA-256 is
`f89585d943d9a72dd66c1053380d22190d6cd3528803adf8b612527daf1d04d9`.
These controls predate the separately reviewed `stuck` cleanup fix; that fix does
not change the tested compiler, linker, source or provider-policy files.

## Retained-object experiment

A complete census covered 2,696 COMDAT groups in 734 stock objects plus the actor
object. Keeping the stock contributors permits actor insertion immediately after
`UITextControl.cpp` (position 444 instead of lexical position 426), without changing
any old object's relative order. A fresh counterfactual relink took 1.165 seconds:
all five actor targets reported OK, no previously OK function regressed, and all
149 mapped contributors remained the same. This is a useful cheap preflight;
header consumers were not rebuilt and original linked vtable/data proof remains
necessary before source admission.

The initial private relink driver completed both links but repeatedly parsed a
large diagnostic during checker preparation. It was interrupted before the checker
ran. The exact session journal was
reconciled after confirming process exit and daemon/container state, and a separate
bounded CPU continuation reused the completed links. No failed attempt was
relabeled fresh or overwritten.

The compiler late-pass experiment is a separate, rejected approach; see
[console touch recovery](ui_touch_recovery.md). Neither this link rule nor those
experiments grant O credit or establish a runnable game.

The subsequent full data gate exposed missing inherited methods and a pre-existing
text-handler mismatch. [Actor-tag recovery](ui_actor_recovery.md) records that
source work, the focused consumer experiment, and the early dependency diagnostic.
Its experimental 160-provider registry has not replaced the production registry.

## Original initializer order

`weak-providers.json` also carries `initializer_order`: the order of the original binary's
`.init_array`, as object paths. The original has 350 entries, all `_GLOBAL__sub_I_` functions;
347 map to objects this build has. The list records an observation about the original link. It
is not checked against our link, and it does not make our initializer order equal to the
original's (the incoming CMake order already disagrees with it in about fifteen thousand pairs).

The link helper uses it for one thing. Initialization-bearing inputs still keep their incoming
relative order, and a registered provider that contradicts that order is still dropped by the
first pass. A dropped provider then gets a second chance: one of its two objects may move
within the initializer chain when all of these hold.

- The original initialized the designated provider's object first.
- Every provider the first pass honoured is still honoured, and definition order is unchanged.
- The chain's number of inversions against the original order does not increase.
- Any other COMDAT copy whose provider changes as a result goes to its registered provider or
  to an object the original linked earlier.

The helper prints each dropped and each honoured signature. The case that motivated this is
`X/Server/SGame.cpp`: once its static initializer was written, the incoming order put
`Network/Server.cpp` first, and Server's 672-byte copy of
`XGrowableMapTS<unsigned long, OnlineService::PlayerResolveData>::Copy` replaced SGame's
408-byte copy, which is the original's. In the original, SGame's initializer is entry 273 and
Server's is entry 293.

On the tree without that change the old and new helper produce a byte-identical ELF and map.
Sixteen registered providers are still dropped: seven agree with the original order but need
more than one object moved, six disagree with it, and three do not involve a direct pair of
initializer-bearing objects.
