# Contributor startup

Start with a fork PR and the existing setup/claim/per-file/checker tools. The
campaign stays paused; arrange a selected owner and read [CONTRIBUTING.md](../CONTRIBUTING.md)
before recovery. Documentation and synthetic tests need no original game inputs.

## Guided terminal setup

For a game build, place the verified files at `build/exefs/main` and
`build/romfs/DiabloIIINX64r.nss`. The wizard verifies the generated bindings and
tables and offers to bootstrap missing or stale files. For a separate dump directory
or an offline Protobuf compiler, use the [standalone bootstrap](../README.md#generate-private-build-inputs).
Documentation and synthetic tooling tests can skip this step.

Run `python3 setup.py` from the repository root (Python 3.10+). It uses only the
standard library and existing project tools: no model, paid service or AI package.
Each numbered stage explains a failure and offers retry or exit. Downloads,
project dependency installation and compiler/build work require typing `yes`
after the command and purpose are shown. Host software is never installed.

`python3 setup.py --check` is read-only and never prompts, downloads or builds;
`--plan` prints the route without probing. Non-TTY invocation without these flags
exits instead of assuming consent. Missing tools and originals are normal on a
new checkout. Put installed tools on this terminal's PATH, verify their `--version`,
and retry. The platform must be macOS/Linux ARM64 or x86_64; see the table below.

Interactive runs create private, gitignored `.onboarding/state.json` and a local
process lock. The atomic checkpoint records stage outcomes, input hashes and an
opaque fingerprint, never full environment variables, commands from saved state,
or raw subprocess logs. Raw tool diagnostics can contain credentials or private
paths, so failures record an exit code and the command to rerun locally. Ctrl-C,
EOF and declined actions remain incomplete. Rerunning revalidates all current
facts, including compiler provenance; old passes never grant consent or prove a
new build. Malformed checkpoints are ignored and existing outputs retained.

The wizard uses `tools/setup.py --preserve-inventories`: reviewed tracked inventories
and headers are consumed without regeneration. A tracked-file hash guard stops
on unexpected drift; inspect `git diff` and preserve changes before reconciling,
never blindly reset your work. Supply your own verified dump at `build/exefs/main`
and `build/romfs/DiabloIIINX64r.nss`; these stay ignored. No originals are downloaded.
Preserved inventories also skip reference-header generation, and the wizard builds
only into its mode-specific directories. Before claiming an owner, run
`python3 tools/build.py` once for the shared `build/2.6.2` compile metadata used by
`claim.py` and `try_file.py`, and `python3 tools/gen_headers.py` for
`build/dwarf/headers`.

Use `--lua` to prepare the Lua backend and run a Lua baseline. The wizard uses
separate `build/onboarding-normal` and `build/onboarding-lua` directories, reconfiguring
and rebuilding all objects on retry so changed compilers cannot reuse stale objects. Existing `build/2.6.2` is preserved. After each build, the wizard runs the existing
strict inventory regression checker against that mode’s ELF/map in a disposable
snapshot and saves `.onboarding/regression-normal.json` or `regression-lua.json`.
All non-U identities are checked; accepted O rows must remain exact. A failed
regression reports **setup/build ready, strict match validation BLOCKED**, with
exit code **3**. Only the mode built in that invocation is reported ready; old
checkpoints never certify the other mode. Failure, decline or EOF at validation
never produces a passing validation status. Inspect its
private JSON report and `docs/checker_policy.md`; existing O labels can lack
proof under the current strict checker. Preserve that evidence and request a
scoped reconciliation rather than changing inventories or weakening checks. This does
not promote inventory rows or replace candidate-specific data/unwind review. Compiler preparation
may take hours. A successful readiness check is not an actual baseline build,
and a baseline build is not proof of a byte-identical game or matching functions.

At revision `2610d8e1`, both clean onboarding builds linked successfully. The
strict checker verified 31,043 of 33,729 O identities in normal mode and
31,674 in Lua mode. All 631 identities verified only in Lua mode belong to the
Lua library. The 2,055 identities unverified in Lua mode were also unverified
in normal mode. The earlier ledger recorded 1,525 failures per mode and is a
historical comparison point. Some current failures involve instruction size or
data targets, so inspect the reason for each identity before attributing it to
a missing proof. See [checker policy](checker_policy.md) and the separate
[per-contribution landing checks](decomp_workflow.md#landing).

CLI exit codes (also shown by `--help`):

| Code | Meaning |
| --- | --- |
| 0 | Requested workflow passed. `--help`/`--plan` perform no validation; `--check` proves readiness only. Interactive success includes inventory regression, not full contribution acceptance. |
| 1 | Setup/build failure, unsupported platform, or failed tracked-file integrity verification; readiness is not certified. |
| 2 | Invalid arguments or unattended invocation without `--check`/`--plan`. |
| 3 | Environment, setup and selected baseline build passed; strict validation failed, was unavailable or was declined. Never treat this as acceptance. |
| 130 | Interrupted; validation remains incomplete even if setup/build already passed. |

Interactive `.onboarding/state.json` records separate `outcomes` for the selected
mode after its build, including `strict_validation` and the final exit code.
Reports are retained; a report from an earlier attempt is not evidence of a new
check. CI must inspect the command mode and exit status, not just readiness text.
Strict contribution requirements are unchanged.

The remaining sections document the explicit advanced route. Those commands stay
available; invoking `tools/setup.py` without the preservation flag still requests
legacy inventory regeneration and should only be used deliberately.

## 1. Fork, clone and check prerequisites

Fork through your Git host, then select the agreed baseline rather than assuming
a default branch:

```sh
git clone <your-fork-url> NSD3Decomp
cd NSD3Decomp
git checkout <agreed-baseline-commit>
git rev-parse HEAD
python3 tools/bootstrap.py --check
```

The first check normally exits **1** with missing tools/submodules/inputs. It is
read-only: no venv, reports, downloads, worktrees, builds, models or servers are
created. Exit **0** means only the described prerequisite checks passed.
Provision prerequisites explicitly; the script never installs them:

| Requirement | Actual use |
| --- | --- |
| Git with worktree/submodule support | Pinned `tools/common` and nested `asm-differ`; Unix locks in claims |
| Python 3.10+ with venv and ensurepip/pip | Standard-library bootstrap; setup installs `tools/common/pip-requirements.txt` in a project venv |
| Rust/Cargo on `PATH` | Patched checker built with `cargo build --locked`; dependencies/cache still required |
| Host LLVM `llvm-nm`, `llvm-dwarfdump`, `llvm-objdump` on `PATH` | NSS inventory, DWARF and original evidence; record versions/AArch64 support |
| Docker Engine/Desktop + Compose v2 | Local checkout mounted at `/work`; clang 5.0.1, linker, CMake/Ninja in Linux |
| Disk/time budget | Dump extraction can need roughly 30 GB, plus space/time for clones, checker/compiler builds and both-mode links |

macOS/Linux ARM64 and x86_64 are the existing code paths. On **x86_64**, set
`export D3_DOCKER_PLATFORM=linux/amd64`; Compose defaults to `linux/arm64`.
On ARM64 use `linux/arm64`. Both cross-compile to AArch64. Native Windows is not
supported by Unix lock/venv paths. A Linux checkout inside WSL2 with working local
Docker integration is a possible path, **untested here**. Remote daemons need the
same checkout available for their bind mount; these instructions assume a local
Engine/Desktop context. Do not expose personal files or a Docker socket to fork code.

Several older helpers fall back to a Homebrew-only LLVM path; put host tools on
`PATH` instead. Host LLVM differs from the container compiler. The original
Nintendo fork/SDK is not distributed; builds use upstream clang 5.0.1 plus an
optional pinned Lua backend patch. It does not match every original function.
After provisioning tools, fetch recorded **public tooling** explicitly:

```sh
git submodule update --init --recursive
```

Don't copy a patched submodule/executable/cache as proof of fresh setup. Preserve
local changes before reconciling a pin mismatch; do not reset them blindly.

### Prepare both compilers explicitly

Before supplying originals, preview the existing container preparation path:

```sh
python3 tools/bootstrap.py --toolchain-plan
# Explicit opt-in, when ready to download/build the compiler image:
python3 tools/bootstrap.py --prepare-toolchain --allow-downloads
python3 tools/backends.py --prepare-lua --allow-downloads
python3 tools/backends.py --check
```

These commands prepare the normal image, Lua image and pinned Lua backend without a game build.
`--check` reads actual recipe/configuration and compiler-byte provenance and probes
the Lua executable in its image; it never downloads, builds or changes caches.
It reports missing, stale, corrupted, unsupported or ready, with repair commands.
`bootstrap.py --check` includes both backend checks. Task inventory listing remains
compiler-independent. Read-only Docker probes use disposable offline containers. It
requires installed Docker/Compose and an already-running daemon for execution;
it installs no host packages and starts no services. LLVM 5.0.1 archive size and
SHA-256 checks run before extraction for both supported platforms. Read the
[canonical lock/provenance limits](../toolchain/llvm-downloads.md): these pins
were observed at LLVM's original HTTPS URLs; legacy signature authentication
remains incomplete. Ubuntu/apt inputs remain mutable. The Lua backend uses the same archive lock. For offline preparation pass
`--archive /path/to/llvm-5.0.1.src.tar.xz` and omit `--allow-downloads`;
the normal and derivative images must already be ready. An existing backend can
be copied with `python3 tools/backends.py --adopt-lua /path/to/existing/cache`
only when current recipe, configuration, host architecture and binary checks pass.
This preserves its original receipt and does not claim a fresh build. Host LLVM tools,
Rust, Python dependencies, submodules and your own originals are still separate
prerequisites. This is preparation, not a completed fresh-clone trial.

## 2. Supply local originals

The clone supplies recovered source, inventories and tooling. It does **not**
supply the game, original NSO/NSS/ExeFS, RomFS/assets, console keys, a playable
game, proprietary SDK/compiler, model credentials/subscriptions or a prepared
build environment. Ignored files on an existing machine are not in a clone.

Supply your own legally obtained matching inputs and required toolchain/assets
access. Bootstrap never extracts/downloads proprietary materials, reads keys,
modifies credentials/security settings or launches a model. Extraction is a
separate user-managed step; [technical reference](technical_reference.md) describes tools/layout. Never
upload inputs via Git, Discord, PR attachments, CI caches/artifacts or registries.
Owning a copy does not itself establish permission for every use/distribution.

Distribution/licensing remains unresolved: there is no root license granting
rights to the game/SDK, middleware has separate notices and incomplete P10
provenance/generation work, and some generated declarations derive from original
DWARF/SDK metadata. Preserve notices and review rights before redistribution.
Hashes/matching are technical checks, not legal clearance. The broader public
snapshot privacy/distribution audit is separate from this onboarding work.

Conventional private paths are `build/exefs/main` and
`build/romfs/DiabloIIINX64r.nss`. Or supply explicit external paths:

```sh
python3 tools/bootstrap.py --check \
  --nso /path/to/own/exefs/main \
  --nss /path/to/own/romfs/DiabloIIINX64r.nss \
  --hash-inputs --check-docker
```

Verify both required originals with the provider-free, standard-library command:

```sh
python3 -B tools/verify_original_inputs.py
```

Use `--input-root /path/to/own/dump`, or `--nso FILE --nss FILE`, to check existing
external originals without copying them here. The [original-input manifest and
provenance notes](original_inputs.md) give exact SHA-256/size identities, accepted
executable equivalents, optional evidence and local-provenance limits. Missing,
wrong-size or wrong-hash required inputs block bootstrap and setup; setup rejects
them before installation, downloads, generation or builds. Present optional files
are checked by the dedicated command; absent optional files are allowed.

The corresponding required NSS is `r`; `ra` is a different program. Bootstrap
always hashes both required files. `--hash-inputs` displays those hashes;
`--check-docker` queries the existing daemon and never starts it. Conversion still
checks generated `main.elf` SHA-256
`9c246371f89c55c2f3bbacc69d0be2c1c60f6a070d66284ce5869171f88be90d`.
Identity verification does not establish ownership or redistribution clearance.

## 3. Bootstrap explicitly

Preview a concrete existing reproduction command with a **new** outside checkout
and **new** private report path:

```sh
python3 tools/bootstrap.py --dry-run \
  --nso /path/to/own/exefs/main --nss /path/to/own/romfs/DiabloIIINX64r.nss \
  --work-dir ../d3-local-reproduction --output build/onboarding/run-01.json
```

After accepting dependency downloads, disk/CPU/Docker use, replace `--dry-run`
with `--run-reproduce --allow-downloads`. This delegates to `tools/reproduce.py
--run`, with no model calls. It snapshots **committed HEAD**, imports only supplied
raw inputs, clones pinned submodules from locally present Git repos, runs setup,
compares five inventories, builds and runs strict regression. Working changes
are excluded. It retains the new checkout/logs on failure and refuses existing
checkout/report paths. Reports contain local paths/private evidence; keep them
ignored. Do not put reports inside the new checkout.

Opt-in may fetch pinned public converters, Python/Rust packages, base container
packages and LLVM archives. Installation uses project venvs, Cargo caches and
Docker, not a bootstrap system package installer. The image recipe's Ubuntu and
packages are not fully locked. Offline use requires pre-provisioned dependencies,
converter checkout and images; `--binaries <local-pinned-checkout>` verifies the
converter against `tools/setup_lock.json`. See [P20](p20_reproducibility.md) for
locking limits/known strict-checker failures. Local cache paths there
are examples, not portable prerequisites.

After inspecting the report, use the retained reproduction checkout as your own
primary clone. Successful setup supplies `main.elf`, reference headers and checker;
only a completed build supplies `compile_commands.json`. An earlier failure can
leave these absent. Stop at inventory drift/failure; don't copy stale artifacts
or relabel inventory to claim setup success. A fully provisioned contributor may
run `tools/setup.py` and `tools/build.py` directly in their own clone, after reading
their regeneration/download/install side effects. Never run these in a claim worktree.

Both-mode review additionally needs isolated baseline/candidate builds with
`python3 tools/build.py --lua-compiler`; bootstrap's reproduction is **normal only**.
Backend preparation can be expensive; see [Lua provisioning](../toolchain/lua/README.md).
Keep both modes' artifacts separate and normal inventory normal only.
[Private matching and the local report command](contributor_ci.md) describe PR proof.

## 4. Coordinate and recover one owner

Follow [CONTRIBUTING](../CONTRIBUTING.md) to choose the real `SRC-…` ID, post a
manual Discord claim, create a local claim workspace and authenticate evidence.

## 5. Check locally and submit a fork PR

Commit only the intended source/header changes. Run the [local report command](contributor_ci.md)
with your originals and retained both-mode artifacts/receipts (`--run-all`);
missing source/object receipts stay pending. Attach its sanitized report and
concise rationale to a PR using the template. Mark skipped/failed gates explicitly.
Maintainers reproduce CPU checks and obtain independent source review;
no recovery rerun is implied. Keep the manual claim through review, then release
it on merge, withdrawal or handoff. A report never means landed/merged.

## Troubleshooting

| Symptom | Next action |
| --- | --- |
| Missing/wrong NSO/NSS | Supply your own correct 2.6.2 inputs; no download fallback |
| Identity verifies but generation drifts | Check tool versions/debug parsing; retain drift, don't relabel |
| Missing/wrong submodule | Explicitly initialize pins; preserve local patches first |
| LLVM missing / no AArch64 support | Put suitable host tools on `PATH`, record versions |
| Cargo/venv resolution fails | Provision dependencies/cache explicitly; `--locked` supplies no cache |
| Docker/Compose fails | Check context/daemon yourself; don't change socket permissions |
| x86 host selects ARM64 | Set `D3_DOCKER_PLATFORM=linux/amd64` |
| Output already exists | Pick a new name; inspect retained evidence, no automatic cleanup |
| Missing compile commands/headers | Resolve the failed setup/build stage in your primary clone |
| Claim busy/stale | Reconcile local ledger and Discord; don't delete state/bypass locks |
| Object OK, linked/data/unwind fails | Keep pending; masked object equality is incomplete proof |
| Public CI green, matching absent | Expected: synthetic CI has no originals; request exact-commit private review |

No fresh dependency provisioning, proprietary-input conversion, Linux/x86_64 or
WSL2 startup, hosted CI run, or full fresh normal/Lua validation was performed for
this onboarding change. A later read-only [cached-artifact probe](contributor_gate_coverage.md)
exercised actual checker/data/object/emission/unwind validators against earlier
receipts and preserved failed/partial statuses.
Existing private artifacts are not clean-clone proof.
