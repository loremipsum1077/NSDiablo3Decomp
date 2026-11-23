# Touch-input recovery and queued integration

## Landed recovery, 2026-11-02

Commit `6fa258bf5fc16065d101628a58965c4e118b5781` lands the four functions
and changes exactly their inventory rows from U to M. The prepared-source queue
completed validation, accepted a separate independent review and recorded the
successful guarded landing. No O row changed.

The recovered code handles controller touch sampling, swipe direction and left/right
thresholds. Its four functions contain 712 original instruction bytes. The global
`int32 s_nTouchDelay = 50` is reconstructed from original initialized storage,
NSS identity and debug ownership. No assembly, guessed offsets or code-generation
workarounds were introduced.

Three functions pass the linked checker in both normal and Lua modes. The
488-byte single-controller `ProcessInput` retains two reversed instructions:
`STR X8,[X19,#8]` and `ORR W0,WZR,#1` at function offsets 476 and 480.
Review establishes the same final results under ordinary entry and valid writable
vector storage; exact instruction order and fault/interruption state remain debt.
The raw checker mismatch at original address `0x36ea9c` remains. All four
rows remain M, with no O credit or whole-binary equivalence claim.

## Reusable pipeline changes

Tool commit `a33ecc414307bc331ab5d912c801dc1b46056379` adds three independently
reviewed capabilities, covered by 256 passing regression tests:

- A generic bounded proof for a named four-byte initialized writable object and
  its adjacent GOT load. It authenticates original identity, debug ownership,
  bytes, permissions, object consumers, linked bindings and old-object census.
  Unsupported forms are rejected; this is not unrestricted mutable-data support.
- An opt-in single-owner integration policy that reuses the authenticated review
  baseline and still requires two fresh integrated candidate builds. Final links,
  maps, provenance, results and every retained object must reproduce the reviewed
  artifacts. Default and grouped integration policies remain unchanged.
- A provider-free durable queue for prepared source commits, validation,
  independent review and guarded landing. It records state and parks stale,
  failed or uncertain jobs; it does not generate source, author its own review,
  rebase candidates or launch paid model workers.

The [queue workflow](recovery_queue.md) and [recovery policy](recovery_only.md)
describe the supported profiles and preservation requirements.

After landing, follow-up commit `216d216` prevents queue state from being placed
inside a pinned Lua cache. All 17 focused queue tests pass, including rejection
before either read-only preflight or execution can write into that cache. The
Touch manifest and receipts were bound to the `a33ecc4` tools; no
earlier evidence was rebound to this later fix.

## Actual validation

The unchanged source/header candidate was rebased onto the frozen tools as
`3e6515c593336b77cda82c30e870dcf9b21607d3`. Four fresh review builds and the
production audit completed in 416.883 seconds. Independent replay completed all
19 obligations: 12 verified and seven safe-uncertified, with no blocking findings.

Each mode contains 733 old objects and one new owner object. Every old object's
link-relevant content is unchanged. Seven shared-header consumers have only
permitted debug metadata differences. The new object covers exactly four
functions and all 712 executable bytes, with no extra function or constructor.
Original and rebuilt unwind programs pass. The named delay is the only data
status transition: verified identities increase from 402 to 403, unverified
identities decrease from 10,649 to 10,648, and 12 preexisting differences persist.
Twenty-five unselected diagnostic changes per mode are authenticated
address-only changes in unchanged callers.

The ordinary strict object checker still lists four unsupported operands in the
488-byte function. Supplemental original import evidence authenticates the
`memcpy` and `memmove` calls, and the separate named-data witness authenticates
the delay GOT load. These witnesses do not relabel the strict result as exact.
The reconstructed default constructor is justified only for authenticated
observed uses with complete overwrite before read or escape; its unknown
implementation and unseen clients are not certified.

## Final transaction

The review SHA-256 is
`b74b7a4b8f421bc62e88be8b3d212f38a6bd45f9ff293900d262fc317911a4c2`.

The final transaction's `landed.json` recorded two fresh integration builds and two reused baseline
builds. Including the four fresh review builds, this run used **six fresh builds
instead of eight**. Integration took 531.604 seconds, including repeated evidence
checks. This is a measured reduction in build count, not a claim of a 25% reduction
in total elapsed time. Both final candidate artifact sets reproduced the reviewed
ELF, map, provenance, results and every retained object before the main branch
advanced. `queue-state/state.json` recorded `touch-input: landed` and zero provider
calls. Available disk space after landing was approximately 68 GiB.

Earlier private diagnostics and failed attempts were not promoted into
source-build receipts.

## Next limits to remove

This demonstrates prepared game source going through the queue to a reviewed
landing. Broader data forms and grouped new-data recovery remain separate work. Exact matching, including the two-word
Touch difference, remains the ultimate completion requirement.

The next source draft is the [console touch handler](ui_touch_recovery.md). Its
single-file compile exposes a local zero-storage proof gap and an unexpected
tracker initializer; those blockers are retained before any full builds.
