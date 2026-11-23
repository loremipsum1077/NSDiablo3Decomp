# P02 claim reconciliation — 2026-10-18

All 20 claims from the initial claims.csv are reconciled below. Original owners, worktrees and branches were retained. No model run was started. Runnable-game work was not performed.

Baseline `fdceb90`: **13,641 exact functions / 3,184,796 original bytes**. Final source revision `2b79005`: **13,662 / 3,188,172**, a gain of **21 / 3,376**, with zero previously exact functions lost. Full inventory remains 36,559 functions / 12,467,556 bytes.

## Landing and validation

| Commit | Candidate | New exact functions | New original bytes | Residual |
|---|---|---:|---:|---|
| `94101bf` | UIItemTag | 8 | 1,332 | Two constructor relocation nonmatches |
| `5b4bcd1` | UISeasonJourney | 3 | 940 | None |
| `1947f86` | CVisualInventory | 1 | 80 | Five missing functions |
| `d21617d` | SmartParticleSystemMarker | 6 | 716 | None |
| `2b79005` | UIDyeItemCommon | 3 | 308 | GetAllDyes; existing insert_raw remains exact |

The existing `tools/build.py` and relocation-aware `tools/check` were run serially in the main checkout, under exclusive workflow/pilot/land locks. Every accepted stage passed the full checker and an address-keyed comparison preserving every prior O. Final `tools/check` returned OK; all five project-plan tests passed (`PYTHONPATH=tools python3 -m unittest tools.test_project_plan`). The validation audit confirms all 20 claims have dispositions, all 17 follow-ups cover their exact remaining symbol sets, and every archived candidate source file equaled its untouched original worktree copy. Local checks were not treated as relocation proof.

The stock land.py wrapper was not invoked: it requires a clean main checkout and unconditionally deletes accepted worktrees/branches. The main checkout had unrelated pre-existing tracker edits and the user explicitly required preservation. Candidate changes were integrated selectively, the same build/check gate was run, and only accepted source plus checker-generated inventory was committed. No hard reset, branch deletion, worktree deletion, bulk merge, or claim release occurred.

Rejected trials were restored only at the paths this task changed. UIActorTag reproduced a full-check regression in UIText::HitTest; UIConsoleTouch reproduced a 212/208-byte nonmatch against current dependencies. UIItemTag’s two constructor calls remained relocation mismatches and were explicitly marked NON_MATCHING. The dye enum move initially shifted assertion source lines by one in ScriptEventParams; original positions were restored and the full checker passed before landing.

Source review used archived prior reviews, fresh DWARF, original assembly/data and current headers. UIItemTag’s unexpected constructor aliases/inherited UIText emissions are not counted as gains. SeasonJourney’s AchievementRecord constructor is at Achievements.h:430 and its original caller shows the zero initialization and virtual slot 13. CVisualInventory’s five original calls use attachment IDs 671,669,670,672,673 and target RActorClearActorAttachment at 0x210790. Particle marker fields/types are checked at Particle.h:110,236–239; current Marker.h owns gtransformMarkerToScene, and the current constructor was preserved. Dye arIndices matches all 92 original bytes.

## Ownership and liveness

The legacy web runner reports paused; its matching jobs have terminal result messages. All 12 DeepSeek claims map to terminal blocked/retired/stopped tasks and null run-control PIDs. Process inspection found no claim runner, coordinator, build or competing landing process; the OpenCode serve daemon alone was present. Nonblocking acquisition of all three locks succeeded. This establishes no observed live claim worker, not abandonment by age. Ownership is retained, and any future resume must recheck liveness.

Six old web directories have broken .git registration paths even though their branches still exist. Their source files were compared directly against branch blobs; Critter and PowersSwig contain uncommitted source invisible to git status in those broken directories. The other four owner sources are byte-identical to HEAD. dmDistance has no worktree or branch; dmHomingFunction has a retained branch and prunable registration but no directory. Nothing was pruned. The eight auxiliary `.claude/worktrees` are clean and have no changes beyond their common ancestor with current dev; their heads are already incorporated tooling/documentation commits, not additional claim implementations.

## Every claim

| Record | Source | Original owner / state | Final disposition | Follow-up |
|---|---|---|---|---|
| P02-R01 | Contrib/Contrib/BlizzardProtoBuf/2.6.0/src/src/google/protobuf/descriptor.cc | web / done | Already incorporated; no source delta. (141/161 O) | P02-F01 |
| P02-R02 | X/Common/OnlineService/OnlineService.cpp | web / done | Already incorporated; no source delta. (34/36 O) | P02-F02 |
| P02-R03 | X/Sigma/DataStructures/XVar.cpp | web / done | Already incorporated; no source delta. (76/78 O) | P02-F03 |
| P02-R04 | X/Client/Behavior/Critter.cpp | web / stuck | Unlanded source recovered from a directory with broken Git registration; preserved. (0/8 O) | P02-F04 |
| P02-R05 | X/Server/Powers/PowersSwig.cpp | web / stuck | Large unfinished reconstruction preserved, including nine split source parts. (0/523 O) | P02-F05 |
| P02-R06 | X/Sigma/UnicodeTextBreakIterator.cpp | web / done | Already incorporated; all 16 assigned functions exact. (16/16 O) | None |
| P02-R07 | X/Client/UserInterface/Interface/UIBind.cpp | deepseek-7e2697c95361 / claimed | Retain evidence hold; two provisional local matches are unlanded. (0/2 O) | P02-F07 |
| P02-R08 | X/Client/UserInterface/Console/UIConsoleTouch.cpp | deepseek-372e0805e066 / claimed | Fresh current-HEAD trial rejected as nonexact; original candidate retained. (0/1 O) | P02-F08 |
| P02-R09 | X/Common/Marker/SmartParticleSystemMarker.cpp | deepseek-ca484dccfe1d / claimed | Recovered and landed 6/6 exact; current Transform fixes superseded old GetAABB blocker. (6/6 O) | None |
| P02-R10 | X/Client/UserInterface/Controls/UIActorTagControl.cpp | deepseek-652e6f3912e5 / claimed | Fresh trial rejected: all five assigned functions match but inherited UIText::HitTest regresses. (0/5 O) | P02-F10 |
| P02-R11 | X/Server/Network/SNetLounge.cpp | deepseek-af8194d9a3fa / claimed | Unlanded candidate retained for source/type evidence repair. (0/2 O) | P02-F11 |
| P02-R12 | X/Client/UserInterface/Interface/UIDyeItemCommon.cpp | deepseek-afa3e6bc812f / claimed | Recovered useful partial: 4/5 assigned exact, including one previously exact helper. (4/5 O) | P02-F12 |
| P02-R13 | X/Common/Definitions/ConversationListDefinition.cpp | deepseek-045d062f6c30 / claimed | Unlanded candidate retained for condition-helper reconstruction. (0/11 O) | P02-F13 |
| P02-R14 | X/Client/CVisualInventory.cpp | deepseek-7ca8b7923587 / claimed | Recovered one exact function; five missing functions remain. (1/6 O) | P02-F14 |
| P02-R15 | X/Client/UserInterface/Controls/UIItemTagControl.cpp | deepseek-a8149f4d7035 / claimed | Recovered partial: 8/10 exact; both constructors remain M. (8/10 O) | P02-F15 |
| P02-R16 | X/Client/UserInterface/Interface/UISeasonJourneyUtil.cpp | deepseek-bf02ffc22947 / claimed | Recovered and landed all 3 functions exact. (3/3 O) | None |
| P02-R17 | X/Client/Network/NetStat.cpp | deepseek-89794bd45fb1 / claimed | Stopped worker; no source implementation or source delta. Prepared task retained. (0/7 O) | P02-F17 |
| P02-R18 | X/Client/CTimedEvents.cpp | deepseek-e10006fc03fd / claimed | Coordinator-deferred task; no source implementation or source delta. (0/8 O) | P02-F18 |
| P02-R19 | Contrib/Contrib/Domino/Collision/Functions/dmDistance.cpp | earlier contributor / stuck | Prior failed probes preserved; HEAD already has 3/4 exact. (3/4 O) | P02-F19 |
| P02-R20 | Contrib/Contrib/Domino/Collision/Functions/dmHomingFunction.cpp | earlier contributor / stuck | Prior failed probes preserved; worktree directory absent, branch retained; HEAD has 3/4 exact. (3/4 O) | P02-F20 |

## Ranked follow-ups

Priority is shared-prerequisite/evidence risk; effort is scope, not elapsed time. All retain their original owner. Exact symbols and acceptance criteria are in project_jobs.json.

| Priority | Effort | Job | Blocker / next action |
|---|---|---|---|
| high | medium | P02-F03 | Resolve RegisterCmdLine and missing emplace_back<const char*&> through original blz string move/assign and transfer inlining evidence. Eight-attempt history is retained; no noinline attributes or synthetic callers. |
| high | medium | P02-F07 | Obtain independent original-body or equivalent evidence for linker-stripped UIBind_Open, especially destination store and dialog-show effect. DWARF high_pc and a need to defeat GlobalOpt do not prove a body. Preserve the candidate; rerun two original functions and full relocations only after evidence review. |
| high | medium | P02-F08 | Resolve static-object emission from current Player.h dependencies: SNONAME_NONE and cgplaceNull precede stTouchGlobals; Update is 212 versus 208 bytes and adds a global-base +0x18 instruction. Audit original header/data ownership and initializer evidence before changing shared headers. Do not reorder globals solely to force bytes. An earlier 1/1 masked match used older headers. |
| high | medium | P02-F10 | Recover faithful emission of UIText::HitTest from its owning header and original inlined sIsInHyperlink/IsInHyperlink evidence. The original 288-byte body requires cpp helper inlining; candidate emits a shorter weak body. Validate all emitting TUs and preserve the original O before retrying five UIActorTag functions. No weak-symbol selection or checker exception workaround. |
| high | medium | P02-F15 | Resolve UIButton C1/C2 base-constructor relocation identity through actual original constructor definitions and ABI evidence. Both UIItemTag constructors are locally exact but full checker targets separate C2 stubs instead of original C1. Preserve 8 exact functions and related UIKeyMappingButton callers; no alias/checker workaround. |
| high | high | P02-F04 | Repair Critter ownership before matching: migrate private ActorMovement/CActorMovement/RActorAnimation subsets to original owners, compare against current shared headers, then split eight functions into constructor/update batches. The legacy permutation-sweep handoff is not a completed review. |
| high | high | P02-F05 | Reconcile PowersSwig.cpp, prelude and parts 1–9 against current shared power APIs; inventory duplicate/missing bodies and source ownership first. Begin with the original first-15-function batch and independently check it; enumerate further bounded batches over all 523 original symbols. Do not concatenate unreviewed parts or restart automated agents. |
| high | high | P02-F11 | Use current AppGlobals owner instead of candidate SNetLounge.h copy; replace invented SGameNameSource with SGameGlobals::szGameName and idSGame with original id. Recover NetGameAdvertItem/NetHeroAdvert layouts from original evidence, fix return/const types, source socket constants from owner, and use semantic public-bit test. Review stripped SNetLoungeSendLANGameParams independently. Two local matches do not establish these source facts. |
| high | high | P02-F13 | Reconstruct sRetvalFromConditionReqs with the original BOOL & fResult contract rather than invented tri-state -1/fDesiredVal behavior. Validate ConversationListEntryEvaluate (2828 bytes) before integrating the 10 provisional matches and 23-file shared-header candidate additively against HEAD. |
| medium | medium | P02-F02 | Resolve SetShellDisposing dead-store elimination and the missing string-vector push_back emitted by stripped GetHost. Obtain independent original-body evidence for stripped readers; do not invent callers to control optimization. |
| medium | medium | P02-F12 | Resolve GetAllDyes container/inlining differences against its 1080-byte original. All 23 arIndices constants now verified from 92 original bytes. Owned declarations and enum integration are landed; preserve three newly exact dye functions and the pre-existing insert_raw helper. |
| medium | medium | P02-F17 | Resume NetStat as a named manual source job only after taking ownership from deepseek-89794bd45fb1. Read the retained stopped task packet and reconstruct the seven listed symbols; there is no implementation to land. |
| medium | medium | P02-F18 | Resume CTimedEvents manually from its prepared task packet after resolving original ownership. Reconstruct the eight listed symbols in two bounded batches; no existing code can be landed. |
| medium | high | P02-F01 | Recover protobuf descriptor residuals without replacing game blz helpers. Inspect Trunk-era string comparisons, find_last_of clamp and rb_tree erase inlining; preserve 141 exact functions. Start by isolating one string-comparison caller proof. |
| medium | high | P02-F14 | Implement the five named remaining CVisualInventory functions in bounded batches, starting sCreateAttachmentForSlot (400 bytes) and VisualInventoryUpdateSheathed (396 bytes). Verify slot tables and Appearance/Actor layouts and required declarations. Retained GBHandle::Set proof is not needed by the landed clear-attachments function; revalidate it with a real downstream caller. |
| medium | high | P02-F19 | Investigate the one dmDistance residual using original listings and retained failed source-order probes. Do not repeat those probes. Preserve all three exact functions; document a new concrete compiler/algorithm hypothesis before editing. |
| medium | high | P02-F20 | Investigate dmHomingFunction::Initialize (original 2628 bytes, retained trial 2556) using retained conjunction/switch failures. Preserve 3/4 exact and the retained branch; do not repeat the three unsuccessful transformations. |

## Residual accounting

The residual ledger enumerated every remaining assigned address separately, including already-landed M functions and unimplemented U functions. No exception is counted exact. Follow-up scope is the exact remaining list, not the earlier masked match count. P02 reconciliation is complete; those source jobs remain open.

The existing tracker changes present at start were preserved. Only the requested backlog file was updated alongside new reconciliation records. claims.csv entries are retained with original owner/state/timestamp and a reconciliation note; no owner was reassigned and no claim was released.
