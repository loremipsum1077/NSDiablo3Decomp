# Local reports and PR checks

Contributors supply their own matching originals locally. No asset hosting,
private image provisioning, provider calls or matching service is deployed here.
Public CI and private matching have different trust boundaries.

## One local report command

Use `contributor_check.py` from a **trusted tooling checkout**, passing the candidate
repository as Git data. It never imports candidate Python, runs a build/game,
changes quality labels or lands anything. With retained artifacts and receipts:

```sh
python3 /path/to/trusted-tools/tools/contributor_check.py \
  --repo /path/to/contributor-clone \
  --baseline <full-agreed-baseline> --candidate <full-candidate-commit> \
  --owner <exact/original/path.cpp> \
  --nso /path/to/own/exefs/main \
  --nss /path/to/own/romfs/DiabloIIINX64r.nss \
  --original-elf /path/to/own/converted/main.elf \
  --evidence /path/to/private/local-evidence.json \
  --evidence-sha256 <independently-recorded-manifest-sha256> \
  --checker /path/to/trusted/check \
  --checker-sha256 <independently-established-checker-sha256> \
  --run-all --output build/contributor-evidence/pr-report-01.json
```

`--run-all` explicitly enables local CPU checks: linked byte/relocation and prior
status preservation, per-identity data preservation, whole-owner strict object
checks, emission accounting and selected original/object/both-link unwind. It
reuses `strict.py`, `unwind.py`, `integration.py::compare` and `validate_data.py`.
It does not build, download, install or call a model. Unknown/unsupported evidence
stays pending. Unchanged pre-existing not-checkable/error entries remain pending,
not new regressions. Degraded or unexpected outside-target status changes fail;
selected target gains are reported without promoting inventory labels.

The default target set is the whole owner, limited to 64 functions. For a bounded
subset, add `--targets /path/to/private/targets.json` containing
`{"format":1,"functions":[{"address":"0x...","name":"exact-qualified-name"}]}`.
Every selection must belong to the committed owner. Both-mode linked/object
accounting still includes the whole owner and baseline non-U identities. Qualified
object names without an authenticated mapping stay pending; never strip `@file`.
Earlier checked identities outside that inventory and shared-header consumers
still require explicit maintainer reconciliation.

### Retain artifacts before running

Use `tools/receipts.py` for the [provider-free source/both-mode receipt workflow](validation.md),
or separately retained outputs from the existing trusted foreground integration
build. Both supply source manifests and per-mode `done.json` receipts in the existing
adapter format. A plain `build.py` link directory alone lacks the complete required
objects/source receipts. Retained verification is not fresh construction. Keep missing
evidence pending; never hand-stamp success, repair link provenance or invent a batch.
Bootstrap reproduction still builds normal mode only.

The **private local** adapter manifest has this structure (replace every value):

```json
{
  "format": 1,
  "baseline": {
    "commit": "<full-baseline>",
    "source": {
      "root": "<retained-baseline-source>",
      "manifest": "<existing-source-manifest.json>",
      "sha256": "<manifest-sha256>"
    },
    "normal": {"root": "<baseline-normal-receipt-root>", "receipt_sha256": "<done.json-sha256>"},
    "lua": {"root": "<baseline-lua-receipt-root>", "receipt_sha256": "<done.json-sha256>"}
  },
  "candidate": {
    "commit": "<full-candidate>",
    "source": {
      "root": "<retained-candidate-source>",
      "manifest": "<existing-source-manifest.json>",
      "sha256": "<manifest-sha256>"
    },
    "normal": {"root": "<candidate-normal-receipt-root>", "receipt_sha256": "<done.json-sha256>"},
    "lua": {"root": "<candidate-lua-receipt-root>", "receipt_sha256": "<done.json-sha256>"}
  },
  "toolchain": {}
}
```

Paths are local, absolute or relative to this manifest; keep it and raw receipts
private. Source manifests are the existing relative-file-to-SHA-256 map, optionally
wrapped in `{"files": ...}`. Source contents must equal the exact committed
`src/`, `include/`, `toolchain/` and `CMakeLists.txt` file set. Only explicitly
counted `.DS_Store` metadata is tolerated. A mode receipt must contain exactly
`{"files": {"relative/path": "sha256"}, "full_check_rc": 0}` from a completed
trusted check. Its root retains `build/2.6.2/{d3,d3.map,d3.map.provenance}` and the
unique `objects/.../src/<owner>.obj`. The baseline object may be absent only when
the baseline commit lacks that owner source and all its inventory members are U;
candidate object proof is always required. Missing inputs fail closed.

Optional `toolchain` keys are `normal_compiler`, `lua_backend`, `linker`, each
`{"path":"<local-tool>","sha256":"<expected-sha256>","version":"5.0.1"}`.
Hashes are observed; the numeric version is a recorded claim, explicitly unverified.
Tools are never executed for version discovery. Use a trusted checker build, not
an executable supplied by a PR. A self-supplied hash does not establish trust.

Receipt/file hash consistency and source snapshots **do not prove construction**.
Fresh build lineage and actual runtime compiler/checker versions stay pending
unless independently reproduced by a trusted build; this command never upgrades
them from supplied JSON. It rejects inventory/checker-config edits, replacements/
grafts, traversal, symlinks, changed copied inputs and mismatched commit bindings.
Only committed source counts; working edits are excluded.

### Read and share the sanitized result

Omit `--run-all` for a lightweight scope/input/provenance report with skipped gates.
Individual opt-ins are `--run-checker`, `--run-data`, `--run-owner-gates`. Without
adapter evidence, explicit `--normal-baseline`, `--normal-candidate`,
`--lua-baseline`, `--lua-candidate` directories can supply linked/data-only checks.
Each such directory contains `d3`, `d3.map`, `d3.map.provenance`.

Exit **1** means failure/output error; exit **2** means incomplete evidence.
The command always leaves independent source review and postintegration validation
required, with accepted/landed false. Inspect JSON even on failure. Output is
exclusive: use a new filename for each run.

The shareable JSON allowlists exact commits, owner/target hashes, input hashes and
version pins, checker/validator/settings/tool hashes, per-mode artifact bindings,
finite status counts/deltas, data/unwind/emission results, elapsed wall time and
gaps. It excludes original bytes/assets, raw reasons/logs, disassembly, local
paths, usernames, credentials and arbitrary free text. Review it before sharing.
Only this sanitized report belongs in a PR; never attach the local manifest,
source manifests, receipts, original logs, maps or binaries.

Reports are contributor-supplied assertions and can be forged. Maintainers must
independently reproduce deterministic CPU gates, authenticate construction and
review changed source/evidence with a narrowly scoped independent reviewer.
AI assistance is optional; independent source review is required for every method.
The [coverage table](contributor_gate_coverage.md)
lists remaining gates. Artifact parsers are trusted code reading hostile data;
this script is not a hardened sandbox. Use the separate disposable comparison
boundary below for untrusted PR artifacts, rather than a personal machine with
secrets or originals alongside fork scripts.

## Public asset-free CI (implemented, not deployed)

`.github/workflows/contributor-checks.yml` runs the existing unittest style against
synthetic bootstrap/report/setup/reproduction fixtures on GitHub-hosted Ubuntu.
It checks out without persistent credentials/submodules, uses read-only repository
permissions and a pinned checkout action, and has no repository secrets, original
inputs, private caches, model calls, artifact upload or deployment step. It uses
`pull_request`, never `pull_request_target`, and never a personal persistent runner.
GitHub may require approval for fork workflows. No repository settings were changed;
this branch has not been pushed, and hosted execution has not been exercised.
The local equivalent is:

```sh
PYTHONPATH=tools python3 -m unittest -v \
  test_bootstrap test_contributor_check test_contributor_adapter test_setup test_reproduce \
  test_data_provenance test_validate_data
```

A green fixture job verifies tooling behavior only. It cannot prove recovery,
source ownership or original byte matching. PR changes can alter their own public
checks, so inspect CI/tooling changes independently. There is no automatic private
follow-on, secret-bearing `workflow_run`, shared cache or privileged PR callback.

## Private maintainer MVP (design only)

1. Freeze the agreed baseline, exact candidate commit, owner/function scope and
   concise contributor rationale/review packet. Authenticate originals/toolchain
   independently. A new commit invalidates prior review/results.
2. In a disposable VM, compile source with a **maintainer-owned frozen recipe and
   tools**, no originals, credentials, network, host mounts, Docker socket, remote
   cache or personal files. Generate needed declarations separately from trusted
   baseline metadata and review their distribution status. Don't run fork setup,
   workflow, CMake custom commands, tests or built game binaries near originals.
   If candidate build changes are needed, review separately and still isolate.
   Use resource/time limits and retain source-to-object-to-link construction hashes
   for normal/Lua baseline/candidate. A plain container/network flag or maintainer
   approval alone does not neutralize malicious code/build scripts/compiler input.
3. Transfer bounded output files and a hash manifest through a controller, rejecting
   symlinks, device files, traversal/archives, unexpected formats/counts/sizes.
   Treat ELF/map/object payloads as hostile parser inputs, never execute them.
4. In a **separate fresh disposable comparison VM**, run trusted pinned checker/
   data/unwind tools against read-only originals and validated artifacts. No PR
   source scripts, credentials, network or personal mounts. Keep original evidence
   and raw logs private. Limit resources and audit parser failures. The report
   command can aggregate the supported checks there, but does not create this VM
   boundary or establish fresh source-to-build lineage.
5. Export only a fixed schema summary through a trusted controller. Don't let PR
   code select arbitrary report fields/paths. Review sanitized output: raw diffs,
   bytes, disassembly, full maps, crash dumps and logs can disclose originals.
   Destroy disposable workers; keep private audit evidence under access control.
   Post a summary manually for the MVP, without service credentials in either VM.
6. Independently reproduce CPU gates, obtain independent source review of changed
   functions/headers plus authenticated evidence, and escalate
   ambiguity only. Do not repeat recovery or trust contributor verdicts. Track
   deterministic wall time, review time and any model calls/tokens/cost when measurable.
   AI assistance is optional. Preserve all regression gates and run
   postintegration validation after authorized named landing.

This separation needs engineering/provisioning before automated private PR
matching is safe. No such service/runner/image/credential setup is implemented.
Until available, retain a draft/pending result rather than run untrusted fork
scripts on the maintainer's workstation with originals mounted. Publication is
also gated on the separately parked snapshot/privacy and licensing review.

## Primary-source examples checked 2026-10-21

[Ocarina of Time CI](https://raw.githubusercontent.com/zeldaret/oot/main/.github/workflows/ci.yml)
uses an upstream-repository gate, a private build container and `/orig` baserom
links, then builds/uploads maps. [Melee CI](https://raw.githubusercontent.com/doldecomp/melee/master/.github/workflows/build.yml)
also uses an upstream-gated build container, `/orig`, diff/completion checks and
map/report uploads. These demonstrate originals outside the public Git tree;
they do not establish legal clearance or safe fork isolation. We do not copy
their asset/image distribution or privilege boundaries.

[GitHub's secure-use guidance](https://docs.github.com/en/actions/reference/security/secure-use)
warns about untrusted checkout in privileged workflows and recommends least
privilege and pinned actions. The checked-in checkout pin is verified against
[actions/checkout v6.0.2](https://github.com/actions/checkout/releases/tag/v6.0.2).
