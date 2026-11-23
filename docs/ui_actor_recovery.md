# Actor-tag recovery and early dependency checks

The isolated source draft is `03ccfe14f888af5a4182c9cad722f15baba6ca22`.
It reconstructs five `UIActorTag` functions and eleven original inline methods
in `UIControl`, `UIControlGroup`, and `UIWindow`. These are reviewed source
drafts, not landed game code or exact-certification credit.

The actor's drawing routine resolves the actor and aura hardpoint, projects its
position to screen coordinates, sizes and positions the text, and draws it.
The shared methods are nine original empty callbacks, a false-returning focus
query, and the 168-byte child-layout lookup/dispatch method. Original code and
DWARF support these bodies; they are not placeholder implementations.

## Measured progress

The formal v4 attempt completed four fresh normal/Lua builds. Its five actor
targets passed the checker without previously passing functions regressing,
but its data gate rejected eleven undefined vtable targets. That attempt remains
failed and immutable.

The v5 investigation recovered those methods, then compiled only the 68 existing
header consumers plus the actor. Canonical compiler commands were unchanged.
Compilation and initial object inspection took 22.79 seconds; snapshot export
and preflight time are additional. Of 68 existing objects, 51 retained their
runtime fingerprints. Across all 68, all 2,047 pre-existing function bodies and
relocation masks remained unchanged.

Detailed inspection found 163 existing vtable relocations becoming references to
the new weak definitions. All 163 emitted copies of the eleven methods passed
strict object checks against the original. Nine existing unwind records changed
only in parsed trailing `DW_CFA_nop` instructions. The decoder preserved complete
operands, including zero-valued operands and augmentation lengths; it did not
strip raw zero bytes. These observations do not waive consumer or unwind gates.
The original CIEs for the new methods also contain four additional decoded NOPs;
their raw unwind bytes are not identical to the rebuilt records.

A private relink used those 69 objects and 666 retained stock objects. Its
experimental registry preserved all 149 production provider choices and the
relative order of all 734 stock objects, placing the actor at index 444. The
eleven additional methods selected the first available copies in that order,
from `UIConsoleBuffIndicatorControl.cpp`. This does not establish original link
order and does not change the production registry. All sixteen targets passed
the linked checker; none of the previously passing functions regressed.

The first raw data report exceeded its 64 MiB file limit. A separate CPU-only continuation used the unchanged validator's
deterministic gzip output, without compiling or linking again. Among 11,063 data
identities, four validation statuses changed:

| Vtable | Result after supplying the methods |
| --- | --- |
| `UIScrollbar` | Verified equal |
| `UIFloatBubble` | Different: `UIText::HandleMessage` target extent |
| `UIText` | Different: the same target extent |
| `UIActorTag` | Different: the same target extent |

The three failing tables point at offset 112 to the existing M-quality
`UIText::HandleMessage`: 1,044 rebuilt bytes versus 1,008 original bytes. The
stock FloatBubble/Text tables already contained this mismatch. Resolving missing
targets exposed it; the header change did not alter the handler's code.

## Run the early diagnostic before requesting full builds

`tools/dependency_preflight.py` reads a candidate owner object and pinned original,
debug and baseline files. It reports missing providers, extent discrepancies,
aliases and unresolved provider selection. It neither builds nor grants approval.
Supply a JSON request with this shape, replacing each path and digest:

```json
{
  "inputs": {
    "owner": {"path": "/private/owner.obj", "sha256": "<sha256>"},
    "original": {"path": "/private/main.elf", "sha256": "<sha256>"},
    "nss": {"path": "/private/DiabloIIINX64r.nss", "sha256": "<sha256>"},
    "baseline": {"path": "/private/baseline/d3", "sha256": "<sha256>"},
    "functions": {"path": "/private/functions.csv", "sha256": "<sha256>"}
  },
  "vtable": "_ZTV10UIActorTag"
}
```

```sh
python3 tools/dependency_preflight.py /private/request.json > /private/report.json
python3 -m unittest discover -s tools -p test_dependency_preflight.py
```

The supported case is the current NX64 original/debug pair and the bounded vtable
layout already understood by recovery tooling. Unsupported shapes remain unknown.
Matching a name or extent does not prove function equivalence. Candidate definitions
remain selection-pending, including weak copies whose sizes differ from the
original. For example, the actor's 148-byte `UIText::HitTest` copy must not displace
the selected 288-byte implementation; see [provider preservation](weak_providers.md).
The diagnostic is not automatically invoked by the recovery queue.

Nine tests passed, including both actor objects and a rehashed malformed
relocation. The earlier object reports eleven missing providers and the handler
extent discrepancy; the closure object reports the newly supplied methods as
selection-pending and retains the handler discrepancy.

## Remaining work before admission

1. Resolve the existing `UIText::HandleMessage` mismatch. Its 57-function owner
   fits the existing bounded validation scope. Independent CFG inspection found
   the same examined dispatch/keyboard behavior in original and current source.
   The current optimized IR marks the left-mouse helper as a tail call and the
   base handler as an ordinary call, although both feed one return block. This
   identifies a compiler-investigation boundary, not a justified source edit.
   Do not manufacture flag assignments, lifetimes or compiler attributes.
2. Define and independently review a narrow shared-header contract for the
   measured new functions, existing relocations and unwind records. The current
   gate correctly rejects changed unrelated runtime objects. Also resolve the
   eleven registry entries without weakening the existing required providers.
3. Prove affected old vtable refinements and the new actor table against original
   data. Preserve the actual failed outcomes; never label a size-mismatched target
   `verified_equal` to continue the pipeline.
4. Only then create a new handoff and run the required fresh normal/Lua review and
   integration checks. The private normal-mode experiments are not fresh receipts,
   and the failed v4 queue must not be resumed or relabeled.
