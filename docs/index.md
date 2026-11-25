# Documentation index

Start with [README](../README.md), [contributor startup](contributor_startup.md) and
[CONTRIBUTING](../CONTRIBUTING.md). Core setup, builds and checks require no
companion repository.

| Need | Core reference |
| --- | --- |
| Expected changes from base Switch to target 2.7.7 | [Patch change tracking and recovery guide](changes_2_6_2_to_2_7_7.md) |
| Porting tools, compiler and mixed build for 2.7.7 | [Porting to 2.7.7](port_2_7_7.md) |
| Remaining 2.7.7 work, sized for agents | [2.7.7 decomp plan](decomp_plan_2_7_7.md) |
| Source provenance and third-party notices | [Provenance review](source_provenance.md) |
| Current exact-first priority | [Exact decompilation](exact_decompilation.md) |
| Compressed evidence snapshots and restore limits | [Artifact storage](artifact_store.md) |
| Interrupted Docker sessions and safe retry admission | [Docker admission](docker_admission.md) |
| Literal NSO comparison and packaging gaps | [Whole-image comparison](exact_image.md) |
| Original identities and owners | `data/2.6.2/`; exact address plus qualified name |
| Source and declarations | `src/`, `include/`; [header/helper closure](headers.md) |
| Local recovery and evidence tools | [Workflow](decomp_workflow.md), [technical reference](technical_reference.md) |
| Two-owner shared builds | [Grouped recovery](grouped_recovery.md) |
| Landing a file | [Workflow: landing](decomp_workflow.md#landing) |
| Periodic whole-tree audits and receipts | [Validation](validation.md), [contributor report/CI](contributor_ci.md) |
| Checker limits and compiler policy | [Checker policy](checker_policy.md), [compile flags](compile_flags.md), [late GlobalOpt plugin](late_globalopt.md) |
| Overall status snapshot | `python3 tools/status_report.py --format 1`; retained per-mode labels and explicit evidence availability |
| Native desktop subsystem tests | [Standalone harness](../desktop/README.md) |
| Scope and project metadata | [Project metadata](project_plan.md) |

`config/project_jobs.json` preserves technical scope, dependencies, deferrals and
acceptance criteria. Personal planning overlays and workflow state stay outside core. `config/component_rules.json` holds attribution rules;
`evidence/checker/reconciliation.json` holds the checker investigation set. These
are core machine inputs, not archived campaign logs.

Inventory O labels and byte totals are retained claims, not fresh verification, a
fully linked identical executable or a runnable game. Candidate acceptance requires
source review, relocations, data, extras, unwind and both linked modes.
