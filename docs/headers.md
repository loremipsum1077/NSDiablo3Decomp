# Recovered declarations and helper closure

Declarations live beside recovered source under `src/`; `include/` contains
compatibility headers. Setup generates private reference headers under
`build/dwarf/headers`, which are evidence, not automatically accepted source.

Use `python3 tools/gen_headers.py --adopt <original/header.h>` only for an authentic
owner needed by the selected recovery. It does not overwrite existing files.
Check original DWARF names/types, ABI/field offsets, signatures, constants and
transitive helper/template bodies against the matching binary. A direct include
heuristic does not prove transitive closure, emission or source ownership.

Every header that defines a struct/class/union carries `sizeof`/`offsetof`
static_asserts from the DWARF: `python3 tools/layout_asserts.py --write <header>`
appends them, `tools/test_layout_asserts.py` enforces it, and
`config/layout_assert_allowlist.txt` / `config/layout_assert_omit.txt` record
justified exceptions and known layout differences.

Shared-header changes require all affected consumers to be reconciled and checked.
Do not invent private substitutes, guessed layouts, synthetic callers or forced
emission to obtain a match. Keep original owner paths and exact address/qualified
symbol identities. Missing proof is a blocker, not permission to fabricate source.

Current technical holds and dependencies are preserved in `config/project_jobs.json`
and exposed by `tools/project_plan.py --job ID`.
