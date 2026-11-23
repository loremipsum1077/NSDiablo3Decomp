# Contributing

Project-authored tooling contributions in the paths listed in [REUSE.toml](REUSE.toml)
use the [MIT License](LICENSES/MIT.txt). For third-party code, identify the upstream
source and retain its notices. For recovered game code, describe the evidence used
to write the implementation.

The unit of recovery is one original source owner: its exact path in
`data/2.6.2/function_files.csv` and stable `SRC-…` project-plan ID. Functions are
identified by **address plus qualified name**, including any `@file` suffix.
The goal is authentic C/C++ with the maintained compiler and strict validation.

The path is **fork/clone → prerequisites → bootstrap check → choose/claim owner
→ evidence → recovery → strict validation/review → PR → release**.
Start with the [startup guide](docs/contributor_startup.md). For source locations,
use the [documentation index](docs/index.md).
Acceptance requires authentic source and evidence, deterministic checks and
independent source review.
The [validation guide](docs/validation.md) describes CPU receipts.

The current direction is full exact decompilation before a desktop port; see
[the exact operating plan](docs/exact_decompilation.md).

## Choose and coordinate

Inspect the committed plan without a server, then replace these example IDs and
paths with the agreed owner:

```sh
git status --short
git rev-parse HEAD
python3 tools/project_plan.py --actionable
python3 tools/project_plan.py --job SRC-REPLACE_WITH_REAL_ID  # metadata as JSON
python3 tools/next.py --file X/Sigma/Math/Rect.cpp
python3 tools/claim.py list
```

Copy the real `SRC-…` ID from the plan; it is derived from the original path.
Save the full baseline commit and address/name/size/quality owner membership.
Split large owners into named function subsets; still inspect whole-owner
missing/extra emissions. Priority and `ready` are hints, not complete dependency
or ownership proof. Reconcile shared-header work as well as source ownership.

Use an agreed Discord channel manually, one thread per owner. No server, bot,
account or integration is provided. Check overlapping claims before starting:

```text
CLAIM owner=SRC-<id> path=<original/path.cpp> baseline=<full-commit>
scope=<address + qualified-name list or membership manifest hash>
branch=<fork:branch> evidence=<safe reference> next-update=<UTC time>

BLOCKED owner=SRC-<id> baseline=<commit> reason=<specific missing evidence>
evidence=<command/status + safe hashes> resume-when=<concrete condition>

PR owner=SRC-<id> baseline=<commit> candidate=<full-commit> url=<PR>
checks=<object / normal / Lua / data-unwind / review statuses>
evidence=<sanitized report reference + SHA-256> reviewer=<public handle>

RELEASE owner=SRC-<id> baseline=<commit> reason=<merged / withdrawn / handoff>
retained=<PR or saved commit> remaining=<unresolved identities/blocker>
```

Keep updates in the same thread. Resolve duplicate or inactive claims manually;
there is no global expiry/takeover rule. Use public handles, not private paths,
credentials, tokens or raw evidence. Discord coordinates effort; it is **not
proof of correctness**.

`claims.csv` is ignored local coordination state. Locks coordinate sessions sharing
one main checkout, not forks, machines or Discord. Manual claim changes fail closed
on an active or invalid queue registered in the main checkout's private
`.git/d3-coordination.json`. Do not bypass locks or publish claim-file resume tokens.

## Local workspace and evidence

Run this from your **own primary clone**, at the agreed baseline, after setup
has produced reference headers and compile metadata (after the wizard, run
`python3 tools/build.py` and `python3 tools/gen_headers.py` once):

```sh
python3 tools/claim.py take X/Sigma/Math/Rect.cpp --who contributor-handle
```

Work only in the printed directory on `work/<owner_slug>`. The helper finds the
primary clone through Git's common directory and bases work on that clone's
current branch. Calling it from a linked worktree does not select that worktree's
baseline. Inspect any existing branch/worktree; verify `git rev-parse HEAD`.
It shares original ELF/NSS/reference trees through symlinks and copies
`compile_commands.json`; don't write through links. These workspaces are not
self-contained clean builds. Don't run setup, full builds, full checker or landing
in `build/work/`.

Use [decomp_workflow.md](docs/decomp_workflow.md) for detailed evidence tools:

```sh
mkdir -p build/contributor-evidence
python3 tools/dwarf_fn.py '<qualified-name>' > build/contributor-evidence/prototype.txt
python3 tools/line_evidence.py '0x<original-address>' > build/contributor-evidence/original.txt
python3 tools/data.py '<named-data-symbol>' > build/contributor-evidence/data.txt
python3 tools/try_file.py src/X/Sigma/Math/Rect.cpp --json > build/contributor-evidence/object.json
```

Use separate filenames per identity. `line_evidence.py` is bounded to 16 KiB
functions; larger functions need bounded reviewed extraction. Authenticate types,
ABI/offsets/signatures, inline trees, header/helper closure, constants, exact
relocation targets, data and unwind before declaring readiness. DWARF provides
no bodies or every initializer. Ghidra and sibling versions are leads that need
proof against this binary. Keep raw evidence private under ignored `build/`.

- Use original owner paths and DWARF names. Adopt needed headers using
  `tools/gen_headers.py --adopt <original/header.h>`; it never overwrites.
- Write maintainable source; preserve line-sensitive layout and middleware
  notices/upstream provenance. Don't paste decompiler bodies.
- No inline assembly, register pinning, forced emission, guessed layouts/stubs
  for credit, global flag changes or checker weakening.
- Don't hand-edit inventories, quality labels, vendored `tools/common/` or link
  provenance. Shared-header additions require affected callers to be checked.
- After about eight evidence-based attempts, retain useful source with
  `// NON_MATCHING: <specific difference>` and record the blocker.

## Validation and review

`python3 tools/land.py <original/path.cpp>` is the gate. It runs one incremental
build and one checker run, then relabels and commits
([details](docs/decomp_workflow.md#landing)):

- the full build must succeed;
- no function that was `O` may now differ from the original;
- a function is `O` when the checker accepts it, or when the checker's only
  objection is a reference it cannot prove yet and `try_file.py` finds every
  instruction equal;
- a compiled function that differs is `M`.

`try_file.py` masks relocation operands, so its OK alone does not verify call
or data targets; landing does that for every reference the checker can resolve.
Run `land.py --no-commit` before opening a PR and include its summary line.
Reviewers read the source for authenticity (names, helpers, no forced bytes).

Both-mode receipts, proof-debt ledgers, data/unwind identity and whole-image
comparison ([validation](docs/validation.md), [checker policy](docs/checker_policy.md),
[whole-image comparison](docs/exact_image.md)) are periodic audits of the
whole tree. They are not required to land a file.

## Fork PR and claim release

Fork contributions need explicit commits, even though the local helper says
`land.py` can commit automatically. Review and stage intended paths only:

```sh
git diff --check
git diff --stat
git add -- src/X/Sigma/Math/Rect.cpp
git commit -m 'Recover selected owner functions'
```

Add each intended header/test path explicitly. Inspect staged content for game
bytes, generated headers, keys/tokens, logs, private paths and unrelated edits.
When ready, push the branch to your fork and open a PR against the agreed
upstream base. Onboarding tools never push or open PRs. Use the
[PR template](.github/pull_request_template.md); include owner/scope, exact commits,
concise source rationale, contributor review, sanitized report, reproducible
commands, affected callers and pending gates. Keep originals, disassembly/full
maps, private logs and resume tokens out of public Git/PR/Discord/artifacts.
Hashes bind evidence; authorized reviewers still need underlying private proof.

Post the PR update manually. Keep the claim until review/merge, withdrawal or
explicit handoff, then post `RELEASE`. Maintainers land one **named** owner
serially after review; never run unscoped `python3 tools/land.py` against old
done claims. Postintegration checks remain required.

`claim.py done` only marks a local row. `stuck` can remove a clean worktree/branch;
`release` removes the worktree and deletes its branch, including committed work
that may not be merged. Retain work in a PR or verified backup branch first.
Never use `release --force` for onboarding. Discord release requires no cleanup,
and local cleanup sends no Discord notification.

## Recovery without exact credit

The opt-in [recovery-only workflow](docs/recovery_only.md) permits independently
reviewed source progress with fresh both-mode differential evidence and pinned
baseline debt. It never grants O credit or substitutes for strict certification.
The guide includes explicit receipt/landing commands.
