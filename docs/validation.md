# Provider-free validation and CPU receipts

Manual and model-assisted changes use the same source/evidence requirements.
Independent source review is required. Compilation, a model verdict or stored-byte
equality alone does not establish acceptance; object matching may mask relocations.
Preserve actual call/data targets, ownership, emitted extras, unwind programs and
every prior checked identity and every non-selected status in fresh normal and Lua links.

The commands below describe **strict certification**, the unchanged default.
For reviewed source progress with pinned existing proof debt, use the explicit
[recovery-only policy](recovery_only.md). Its receipts cannot certify exactness.

## Core commands

Setup/build remain `tools/bootstrap.py`, `tools/setup.py`, `tools/reproduce.py` and
`tools/build.py`. Strict CPU primitives and trusted build/receipt machinery live in
`tools/validation/`. They import no provider orchestration code.
Public CI uses
asset-free fixtures; it has no originals or matching-service credentials.

`tools/receipts.py` supplies the source/object/mode receipts consumed by
`contributor_check.py`. Run it from trusted tooling against explicitly trusted
commits and existing local inputs/images. Fork code still requires the disposable
compile/comparison boundary described in [contributor checks](contributor_ci.md).
No installation, image pull, model call or service startup is performed.

```sh
python3 tools/receipts.py build --repo /path/to/core \
  --baseline <full-baseline-commit> --candidate <full-candidate-commit> \
  --original-elf /private/main.elf --original-sha256 <sha256> \
  --checker /trusted/check --checker-sha256 <sha256> \
  --lua-cache /private/lua-toolchain --lua-receipt-sha256 <receipt-sha256> \
  --targets /private/targets.json --output /private/new-receipt-directory
```

The default is read-only preflight of local input pins and committed source. Existing
Docker-image identities are queried only with `--run`. Add that flag only for an
authorized foreground CPU build. It exports
exact committed source and support files into isolated private roots and runs four
normal/Lua baseline/candidate builds with the existing trusted machinery. It binds
source commits/manifests, original/checker/Lua backend receipt hashes, tool hashes
and immutable local image IDs. Generated game code is read as data, never executed.

Targets use `{"format":1,"functions":[{"address":"0x...","name":"exact-qualified-name"}]}`,
with one to 64 exact committed identities. Target selection does not waive whole-owner
emission accounting, shared-header checks or prior non-target preservation. Both
receipt construction and contributor reports use the same selected-target comparison:
selected targets must finish `ok` in each mode, including intended `mismatch` → `ok`
gains; unselected identities must retain their exact status, even for improvements.
A selected U target may be absent from the baseline non-U receipt request, but no
unselected identity may be added or any previously checked identity lost. Empty,
duplicate or unknown scopes and duplicate result identities are rejected. Unchanged
unselected `missing`/`not-checkable` evidence can be retained in receipts, but reports
remain partial; checker errors still prevent receipt construction.

## Receipts and interruption

Outputs include `binding.json`, `run.json`, source manifests, per-mode `done.json`
and adapter-compatible `evidence.json`. Retain the full private directory; do not
hand-stamp or repair hashes. `--run --resume` verifies completed checkpoints and
uses new retained attempt directories for interrupted modes; changed bindings,
stale/tampered artifacts and unsupported evidence fail closed.

```sh
python3 tools/receipts.py verify --output /private/retained-receipts
```

`verify` checks retained bytes only and always reports `fresh_build_executed=false`.
Resuming completed checkpoints is not a new build. A fresh run result reports only
what executed in that invocation. Receipts remain contributor-supplied evidence,
with `accepted=false`; independent review, strict object/data/extras/unwind gates
and postintegration checks remain pending until separately completed.

Pass `evidence.json` and its independently recorded SHA-256 to the existing
`contributor_check.py --run-all` command described in [contributor CI](contributor_ci.md).
It does not build or grant trusted approval. Keep originals, raw objects/maps/logs,
receipts and local paths private; export only its sanitized report as appropriate.

This extraction was exercised with synthetic/mocked trusted builders and negative
binding/stale-artifact tests, not fresh game builds or a clean-clone trial. The later
true fresh-clone trial remains separate.

## After landing

`contributor_check.py` is a **pre-landing** gate: candidate changes to inventories
or checker configuration remain forbidden. `land.py` legitimately updates the
normal inventory, so do not run that contributor comparison against the landed
commit or relax its inventory restriction.

Use `receipts.py build` above with a **new output directory**, the original baseline
and the exact landed commit as `--candidate`, then `--run` for fresh both-mode
post-landing evidence. It checks selected targets and preservation of every prior
non-target identity without changing inventories. Verify that retained evidence
against the landed commit with:

```sh
python3 tools/receipts.py verify --output /private/post-landing-receipts \
  --repo . --candidate <full-landed-commit>
```

This additionally binds source/support manifests to the exact committed landed
state. An old pre-landing receipt cannot pass as a new landed build. Verification
reports retained bytes, never a fresh build or independent approval. Source review,
strict relocation/data/extras/unwind checks remain separate required gates.
