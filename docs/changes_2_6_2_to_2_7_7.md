# Change tracking: Switch 2.6.2 → 2.7.7

Prepared 2026-11-24 for this repository's decompilation work. This is a research and recovery guide, not proof that a feature has been recovered or that every historical hotfix has been catalogued. It covers major shipped systems, representative balance and bug-fix changes, local asset evidence, and executable search anchors. Exact per-item numerical histories remain in the linked release notes.

## Target identity comes before patch attribution

| Item | Observed identity |
| --- | --- |
| Baseline | Switch `C2.6.2.52309`; see `src/X/Sigma/buildver.cpp` and the baseline original-input manifest |
| Repository/build namespace | Branch `dev` (ported on `port-2.7.7`; 2.6.2 kept on `2.6.2`); `data/2.7.7/`, `build/2.7.7/`; `tools/config.toml` selects `2.7.7` |
| Original target input inspected | `data/2.7.7/main.elf`, 18,204,656 bytes |
| Embedded target version | `2.7.7`, `C2.7.7.92380`, `C2.7.7.92380 (135863-1751824)` |
| Embedded code branch | `branches/2_7_7`, also present in source-path strings |
| Target ELF SHA-256 | `b462b3f68d0afc80eb2331b2b18c96a86046ef5a459bedc2439aa7959cfa13c5` |
| Compiled project output | `build/2.7.7/d3`; do not mistake this for the original target input |

The target binary identifies itself as **2.7.7**. The project's version labels, data and build paths were renamed from an earlier mistaken `2.7.9` to `2.7.7`. Do not apply public 2.7.8/2.7.9 behavior to this target. Conversely, a 2.7.7 label alone does not identify which later hotfixes are included; retain the full build number and hash.

## Evidence and status rules

- **P — published:** Blizzard release notes or a Blizzard staff release announcement describes the change. Respect platform exclusions, later amendments, and crossed-out PTR text.
- **A — archive:** a named entry was parsed from a local CPK. Presence proves an entry exists, not when its payload changed, whether it is active, or what its values mean.
- **E — executable:** a string or other fact was observed in the hashed target ELF. A string can identify a search location but is not proof of reachable behavior.
- **H — hypothesis:** proposed source owner, implementation mechanism, or feature-to-asset association. Verify through references, data layouts and control flow.
- **V — verified implementation:** reserve for a documented target address, validated mapping and checked implementation. **No feature in this guide is promoted to V.**

Archive contents, patch-note claims, and code hypotheses must stay distinct. A balance adjustment can be entirely data driven. A changed function can instead reflect compiler changes, altered structure offsets, generated code, or a bad old-to-new match. A retained seasonal feature can be disabled by runtime configuration.

## Chronological change register

Each ID is stable for follow-up evidence. Unless stated otherwise, published cross-platform notes include Switch, but the implementation is still unverified in this target. “Seasonal” describes activation, not whether code/assets remain in later binaries. Sources are linked per patch; use the released values rather than superseded PTR values.

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-264-01 | 2.6.4 | Broad class-set increases; Legacy of Nightmares bonus increased. | Persistent; check actual Switch values. | Set-bonus tables and generic modifier evaluation before native power code. |
| CH-264-02 | 2.6.4 | Season 16 grants the Royal Grandeur effect; first seasonal solo GR70 completion grants a Primal. | Theme seasonal; reward condition must be checked against later fixes. | Season activation, set-count adjustment, completion/reward predicates. |
| CH-264-03 | 2.6.4 | Primal presentation, inventory/material handling, Armory and Greater Rift restrictions changed. | Source is framed as PC/Mac and includes mouse-specific UI; Switch applicability of individual UI changes remains unresolved. | Compare existing console behavior first; do not import PC UI assumptions. |

Source: [Blizzard 2.6.4 release notes](https://news.blizzard.com/en-us/article/22863534/patch-2-6-4-now-live). No separate 2.6.3 or 2.6.4 archive was found in the inspected directory. The 2.6.3 interval remains an explicit historical coverage gap, not an assertion that nothing changed.

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-265-01 | 2.6.5 | Torment XIV–XVI, revised rewards, and a special Ancient/Primal Puzzle Ring result. | Persistent. | Difficulty bounds/scaling, reward tables, Cube portal selection. |
| CH-265-02 | 2.6.5 | Season 17 grants Legacy of Nightmares; legendary powers are retuned. | Theme seasonal; item changes persistent. | Community-buff dispatcher, item/set effects. |
| CH-265-03 | 2.6.5 | Fixes Keywarden map icons and Taiwanese subtitle overflow on Switch; console Cube duplicate-tab fix; GR70 Primal reward now respects successful timing. | Explicit Switch/console fixes plus shared reward fix. | Map icon visibility, subtitle layout, Cube UI, timed completion. |

Source: [Blizzard 2.6.5 release notes](https://news.blizzard.com/en-us/article/22989462/patch-2-6-5-now-live).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-266-01 | 2.6.6 | Triune circles grant combat bonuses during Season 18. | Seasonal. | Hit-triggered spawning, area membership, temporary buffs. |
| CH-266-02 | 2.6.6 | Legacy of Dreams gem; new or revised powers including Flavor of Time, Executioner, Stone Gauntlets, Echoing Fury, Squirt's Necklace and Messerschmidt's Reaver. | Persistent; individual powers have follower/pet eligibility rules. | Damage/kill callbacks, equipment exclusions, duration and cooldown handling. |
| CH-266-03 | 2.6.6 | Crafted sets revised: Sage, Captain Crimson, Aughild and Cain; old crafted items/recipes are distinguished from new ones. | Persistent. | Recipe identity, legacy-item compatibility, attribute-derived modifiers. |

Source: [Blizzard 2.6.6 release notes](https://news.blizzard.com/en-us/article/23093788/patch-2-6-6-is-now-live).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-267-01 | 2.6.7 | Aegis of Valor and Patterns of Justice added; Barbarian power changes; Archon item-bonus persistence fixed. | Persistent. | New set identities, skill interactions, transformation cleanup. |
| CH-267-02 | 2.6.7 | Pandemonium kill-streak events; Greater Rift key vacuum pickup; console privacy menu notice. | Seasonal events; persistent pickup/UI changes. | Kill counters, event thresholds, pickup classification, console UI. |
| CH-267A-01 | 2.6.7a | Lamentation Rend bonus restored at 100–150%; Shield of Fury capped at 20 stacks; Fjord Cutter and Won Khim Lau fixes. | Persistent. | Bonus applicability, stack cap, teleport/channel interaction. |
| CH-267A-02 | 2.6.7a | Arcane Attunement GR-entry cleanup and Patterns of Justice Flurry cancellation fix were explicitly PC-only at this release. | **Do not date these console fixes to 2.6.7a.** Later arrival unresolved here. | Keep separate follow-up records for console landing. |

Sources: [Blizzard 2.6.7 release notes](https://news.blizzard.com/en-us/article/23220967/patch-2-6-7-now-live), [Blizzard staff 2.6.7a announcement](https://us.forums.blizzard.com/en/d3/t/now-live-patch-267a-update/7413).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-268-01 | 2.6.8 | Horde of the Ninety Savages, Mundunugu's Regalia and Typhon's Veil; associated skill/item changes. | Persistent. | Frenzy, Spirit Barrage and Hydra effect ownership. |
| CH-268-02 | 2.6.8 | Season 20 allows any item category in each Cube power slot without duplicate-power stacking. | Seasonal, with rollover reset. | Slot validation, duplicate suppression, save migration. |
| CH-268-03 | 2.6.8 | Machines/organs use vacuum pickup; bounty plans guaranteed until all learned; Fires of Heaven early animation cancellation prevented. | Persistent. PTR Spirit Barrage timing change was withdrawn. | Pickup filters, recipe knowledge, animation/cast timing. |

Source: [Blizzard 2.6.8 final notes](https://news.blizzard.com/en-gb/article/23319443/patch-2-6-8-now-live).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-269-01 | 2.6.9 | Gears of Dreadlands and Masquerade of the Burning Carnival; Momentum/Strafe interactions; Simulacrum and associated item changes. | Persistent. | Skill replay, timed stacks, pet damage sharing and duplication. |
| CH-269-02 | 2.6.9 | Season 21 periodically triggers elemental effects. | Seasonal. | Periodic event scheduling and effect spawning. |
| CH-2610-01 | 2.6.10 | Season 22 shrine/pylon shadow clones and an extra Cube slot; item retuning. | Seasonal mechanics. | Clone creation/lifetime/loadout, Cube slot serialization. |
| CH-2610-02 | 2.6.10 | Pet/Simulacrum bonus fixes, legacy item stacking fixes and Blessed Hammer target limit. | Shared fixes; Act IV crash fix had a separately delayed Switch rollout. | Damage-source attribution, compatibility rules, projectile lifetime. |

Sources: [Blizzard 2.6.9 final notes](https://news.blizzard.com/en-gb/article/23428368/season-21-trials-of-tempests-is-now-live), [Blizzard 2.6.10 release and amendments](https://news.blizzard.com/en-us/article/23548757/season-22-shades-of-the-nephalem-has-ended).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-270-01 | 2.7.0 | Followers receive 14 equipment slots, Emanate effects and reworked skills. | Persistent in seasonal and non-seasonal play. | Hireling inventory layout, equipment-to-player propagation, UI and save schemas. |
| CH-270-02 | 2.7.0 | Firebird and Rathma reworks and leaderboard changes. | Persistent; inspect console presentation separately. | Skill/set behavior and leaderboard filtering. |
| CH-271-01 | 2.7.1 | Ethereal weapons: fixed affixes plus randomized legendary/passive powers; one equipped; restricted modification, acquisition and season transfer. | Seasonal; collection unlock tracked separately. | Item quality/type, equipment validation, passive deduplication, collection state. |
| CH-271-02 | 2.7.1 | Inna/Mystic Ally rework and ally AI improvements. | Persistent. | Summon limits, duration refresh, targeting and active-form transitions. |
| CH-272-01 | 2.7.2 | Seven Soul Shards, helm/weapon eligibility, Hellforge Ember upgrades, salvage/augment use and season restrictions. | Seasonal. No standalone 2.7.2 CPK here. | Socket validation, rank upgrades, item-use transactions and rewards. |
| CH-272-02 | 2.7.2 | Arachyr/Corpse Spider, Marauder/Sentry, Firebird and associated item changes. | Persistent. | Pet classifications, automatic skill firing, damage eligibility. |

Sources: [Blizzard 2.7.0](https://news.blizzard.com/en-us/article/23647866/season-23-the-disciples-of-sanctuary-has-ended), [Blizzard 2.7.1](https://news.blizzard.com/en-us/article/23695733/season-24-ethereal-memory-has-ended), [Blizzard 2.7.2 released notes](https://news.blizzard.com/en-us/article/23746130/season-25-the-lords-of-hell-has-ended). The 2.7.1 page also records a later 2.7.1a update; its separate Switch binary/content boundary has not been established.

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-273-01 | 2.7.3 | Echoing Nightmare entered through a Petrified Scream; escalating encounter and Whisper of Atonement reward. | Initially seasonal; see CH-274-02 for permanence. | Cube entry, world/event state, wave scaling, termination and reward calculation. |
| CH-273-02 | 2.7.3 | Orek's Dream, curated Greater Rift layouts/monster pools, progression adjustments and solo early rift closure; Mystic Ally fixes. | Persistent. Xbox resolution changes in these notes are not Switch changes. | Dungeon selection, monster weights, closure state, pet movement. |
| CH-274-01 | 2.7.4 | Angelic Crucibles sanctify equipment with special class powers and perfect Ancient-level stats; one equipped. | Seasonal; items do not transfer at rollover. | Item conversion, power assignment, validation and cleanup. |
| CH-274-02 | 2.7.4 | Echoing Nightmare becomes permanent with reduced XP; Adventure/difficulty unlock changes; repeatable Uber realms; Guardian, Akkhan, Trag'Oul and other item changes. | Persistent. Some unlock behavior may already exist on baseline Switch. | Compare baseline before claiming new code; encounter reset, experience and set effects. |

Sources: [Blizzard 2.7.3](https://news.blizzard.com/en-us/article/23785083/season-26-the-fall-of-the-nephalem-has-ended), [Blizzard 2.7.4 released notes](https://news.blizzard.com/en-us/article/23816414/season-27-lights-calling-preview).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-275-01 | 2.7.5 | Altar of Rites: sacrifices, prerequisite nodes, account-wide seasonal powers and potion effects. Pet material pickup and item salvage are among its effects. | Seasonal unlock state; later retuned. | New console UI/controller, sacrifice transaction, persistence, power activation and pet ownership. |
| CH-275-02 | 2.7.5 | Primordial Ashes: Primal salvage yields 55; Cube upgrade costs 100; one upgraded Primal equipped. Natalya/Spike Trap and Monk changes. | Ashes/crafting persistent; distinguish crafted Primals from ordinary drops. | Currency type, recipe, item flags, equip limits; skill and trap handling. |
| CH-276-01 | 2.7.6 | Solo Self Found mode, dedicated leaderboards, hero creation/rebirth and multiplayer restrictions. | Mode-specific. | Hero flags, matchmaking guards, save/protocol fields and UI filters. |
| CH-276-02 | 2.7.6 | Visions of Enmity nested fissures and special monster affixes; Season 29 Paragon allocation changes. | Seasonal rules; do not assume the Season 29 cap remains enabled in target. | World transitions, portal recursion/cleanup, affix and Paragon configuration. |
| CH-276-03 | 2.7.6 | Orek's Dream composition changes; Crusader/Demon Hunter/Witch Doctor balance and quality-of-life fixes. | Shared unless platform-qualified; mouse-specific fixes require separate Switch evidence. | Data pools, item stack semantics, pet/skill conditions. |

Sources: [Blizzard 2.7.5 released notes](https://news.blizzard.com/en-us/article/23897180/season-28-rites-of-sanctuary-has-ended), [Blizzard 2.7.6 released notes](https://news.blizzard.com/en-us/article/23999068/season-29-visions-of-enmity-has-concluded).

| ID | Patch | Published behavior to investigate | Lifetime / platform qualification | Recovery lead (H) |
| --- | --- | --- | --- | --- |
| CH-277-01 | 2.7.7 | Returning season themes; Altar and Visions retained, with Altar retuning. | Persistent seasonal systems, not permission to enable everything in non-seasonal play. | Community-event gates and existing power data. |
| CH-277-02 | 2.7.7 | Chiltara gemstone chance 20%; Whisper salvage yields 1–3 Souls; Orek's Dream chance 4% rather than 1%; Echoing Nightmare shrine no longer spawns Gelatinous Sires. | Shared changes; candidate asset matches below. | Loot tables, salvage rewards, dungeon weights and shrine spawn lists. |
| CH-277-03 | 2.7.7 | All-class balance pass, including set multipliers, Blessed Shield chaining, Faithful Memory/Denial stacks, Wave of Light target threshold and pet effects. | Persistent; retain per-item details in source notes. | Data first; inspect native callbacks only where semantics require it. |
| CH-277-LATE-01 | Later 2.7.7 notes | Season 31 notes add Korean rating-logo and progress-orb eligibility fixes. | **Inclusion in build 92380 unresolved.** | Compare target data and conditions; do not equate version label with hotfix completeness. |

Sources: [Blizzard 2.7.7 release notes](https://news.blizzard.com/en-us/article/24046224/season-30-the-lords-of-hell-has-concluded), [seasonal scope explicitly described in the 2.7.7 PTR announcement](https://news.blizzard.com/en-us/article/24031819/diablo-iii-ptr-2-7-7-has-concluded) (scope context only, not authority for final balance values), [later 2.7.7 amendments](https://news.blizzard.com/en-us/article/24078323/season-31-season-of-the-forbidden-archives-has-concluded).

## Local archive inventory (A)

Inspected read-only under `Documents/diablo3/extracted_7_7_9/romfs/Update/CPKs/`, using the existing `Game/tools/D3CPKUnpack/cpklib.py`. All 14 tables parsed as CPK version 7 with header format constant 10. Entries below are table entries, **not unique changed assets or verified readable payloads**.

| Archive | Entries |
| --- | ---: |
| `Patch2_6_5.cpk` | 8,362 |
| `Patch2_6_6.cpk` | 1,296 |
| `Patch2_6_7.cpk` | 1,862 |
| `Patch2_6_7a.cpk` | 47 |
| `Patch2_6_8.cpk` | 1,986 |
| `Patch2_6_9.cpk` | 305 |
| `Patch2_6_10.cpk` | 428 |
| `Patch2_7_0.cpk` | 742 |
| `Patch2_7_1.cpk` | 6,516 |
| `Patch2_7_3.cpk` | 21,623 |
| `Patch2_7_4.cpk` | 21,998 |
| `Patch2_7_5.cpk` | 28,310 |
| `Patch2_7_6.cpk` | 29,048 |
| `Patch2_7_7.cpk` | 91 |

No separate 2.6.3, 2.6.4 or 2.7.2 CPK was found here. In particular, `Actor\P72_Soulshard_01.acr` and multiple `P72_ItemPassive_Soulshard_*` powers are present in `Patch2_7_3.cpk`. Missing archive names therefore do not imply missing features. Repeated names in later archives also do not prove unchanged or changed payloads.

### Useful asset associations

These entries were observed (A); their relationship to a published feature is a hypothesis (H) until data and references are inspected.

| Register IDs | Observed entries / archive | Suggested use |
| --- | --- | --- |
| CH-265-02 | `Power\Community_Buff_LegacyOfNightmares.pow` in 2.6.5 | Season buff identifier. |
| CH-266-01 | `Power\Community_Buff_TriunesWill.pow` in 2.6.6 | Area-buff dispatch. |
| CH-267-02 | `Power\Community_Buff_Pandemonium.pow` in 2.6.7 | Kill-streak event handler. |
| CH-269-02 | `Power\Community_Buff_Weather.pow` in 2.6.9 | Periodic seasonal effects. |
| CH-2610-01 | `Power\Community_Buff_ShadowClones.pow` in 2.6.10 | Clone spawning/lifetime. |
| CH-270-01 | `UI\Hireling.ui`, `Monster\Hireling_Templar.mon` in 2.7.0 | Follower layout and definitions. |
| CH-271-01 | `Power\P71_ItemPassive_Ethereal_025.pow` in 2.7.1 | Ethereal passive identifiers. |
| CH-272-01 | `Actor\P72_Soulshard_01.acr` in 2.7.3 | Carried-forward Soul Shard content. |
| CH-274-01 | `EffectGroup\master_itemFlippy_legend_switch_sanctified.efg` in 2.7.4 | Switch-specific presentation asset. |
| CH-275-01 | `Power\P75_ItemPassive_DarkAlchemyMinor_018_AI_PetPickup.pow` in 2.7.5 and 2.7.7 | Altar/pet automation candidate. |
| CH-276-02 | `Condition\P76_NestingWorldActive_15.cnd` in 2.7.6; `Power\Community_Buff_NestingPortalSpawn.pow` in 2.7.7 | Nested-world activation and portal spawning candidates. |
| CH-277-02 | `TreasureClass\Unique_Chiltara.trs` in 2.7.7 | Gemstone drop candidate. |
| CH-277-02 | `TreasureClass\WhisperofAtonementSalvage.trs` in 2.7.7 | Salvage reward candidate. |
| CH-273-02, CH-277-02 | `DungeonFinder\X1_DungeonFinderEngine_GreaterRift.dfn` in 2.7.7 | Rift selection/weight candidate. |
| CH-277-03 | `GameBalance\Items_Legendary.gam`, `GameBalance\SetItemBonuses.gam`, `Power\Monk_WaveOfLight.pow` in 2.7.7 | Item/set/skill comparison candidates. |

### Reader limitation that blocks semantic asset diffs

The current reader returned the declared byte length for all 91 entries in `Patch2_7_7.cpk`, but **44 payloads were entirely zero-filled**, including `Items_Legendary.gam`, `Unique_Chiltara.trs` and `WhisperofAtonementSalvage.trs`. The library documents possible companion-archive stubs, but that explanation has not been proven for these entries. Sparse data, archive resolution and decoder defects remain possibilities. Nonzero output is also not automatically valid decoded data.

Do not interpret a zero-filled payload as a removed feature or a zero drop rate. Resolve the effective asset through the real archive load/override rules, validate decompression and schema/SNO identity, then compare to the effective 2.6.2 asset. Lexical filename ordering is not an established load order. Do not concatenate the archives or count repeated entries as separate changes.

## Executable search anchors (E)

These exact printable strings are present in the target and absent from the baseline ELF's printable-string set. Offsets are **file offsets**, not virtual addresses or function addresses. Convert through the ELF program headers before using disassembler addresses; multiple copies or references may exist. Their feature associations remain H.

| Feature | Exact target string | First file offset |
| --- | --- | --- |
| Altar UI | `ConsoleDarkAlchemy` | `0xdc2a9f` |
| Visions | `CommunityEventNestingPortals` | `0xdf23a6` |
| Ethereals | `CommunityBuffEtherealItems` | `0xdde275` |
| Soul Shard upgrades | `ConsoleInventory:AddSoulShardRanksButton` | `0xdca4ca` |
| Sanctified items | `CommunityBuffSanctifiedItems` | `0xdd7b86` |
| Crafting currency | `Primordial Ashes` | `0xe37039` |
| Solo mode UI | `ConsoleHeroCreate:SoloSeasonalHardcoreModalWarningDescription` | `0xdeb941` |

Baseline ELF SHA-256 used for the string comparison: `9c246371f89c55c2f3bbacc69d0be2c1c60f6a070d66284ce5869171f88be90d`. Exact-string absence does not prove absence of an equivalent mechanism or translated text.

### Resolved anchor references (E; feature ownership still H)

`python3 tools/string_anchors.py` resolves each anchor to the functions whose code uses it
(addresses are `functions.csv` addresses; names and Quality as of this writing).

| Anchor | Referencing function | Notes |
| --- | --- | --- |
| `ConsoleDarkAlchemy` | `0x710017a700` U, 1,116 bytes; also `UI_InitializeForGame` (M) | Builds controls from the `ConsoleDarkAlchemy:DarkAlchemyUI_main` template; called by `0x7100178050` (U). |
| `CommunityEventNestingPortals` | `0x71004a7bb0` U, 1,424 bytes | Registers `XVar` bool/float variables: a static initializer for community-event settings. |
| `CommunityBuffEtherealItems`, `CommunityBuffSanctifiedItems` | `0x71004a61d0` U, 2,864 bytes | One function names both buffs; callers include `_GLOBAL__sub_I_LobbyService.cpp`. |
| `ConsoleInventory:AddSoulShardRanksButton` | `0x71003736c0` U, 448 bytes | Also uses `Inventory:SanctifyLegendaryItemConfirmation`; called by `0x71003739a0` (U). |
| `Primordial Ashes` | no direct code reference | Held in a pointer table at `0x10d1308` read by `_GLOBAL__sub_I_GameBalanceDefinition.cpp`, next to enum (`SymbolicEntry`) descriptors: likely a new currency enum value. |
| `ConsoleHeroCreate:SoloSeasonalHardcoreModalWarningDescription` | `UIHeroCreate::Console::OnButton1` (M) | An existing function that changed, not a new one. |

## Where to start in this repository

These existing source paths are candidates, not assignments of recovered target functions. Newly introduced implementations may need new files rather than being forced into baseline owners.

| Priority | Change cluster | Existing source entry points | First evidence task |
| --- | --- | --- | --- |
| 1 | Altar, Visions and seasonal gates | `src/X/Server/SPlayer.cpp`, `src/X/Server/Powers/PowerAPI.cpp`, `src/X/Server/LuaScript/ScriptUtil.cpp` | Find references to `ConsoleDarkAlchemy` and `CommunityEventNestingPortals`; follow UI registration and event dispatch to unnamed functions. |
| 1 | Ethereal/Soul Shard/Sanctified/Primal item rules | `src/X/Common/Items.cpp`, `src/X/Common/ActorInventory.cpp`, `src/X/Common/Protobuf/Items.pb.cc` | Compare item-use/equip validation and persistent fields; trace unique UI error/confirmation strings. |
| 1 | Followers and Emanate | `src/X/Common/Hireling.cpp`, `src/X/Server/SHireling.cpp`, `src/X/Server/AI/HirelingBrain.cpp`, `src/X/Client/UserInterface/Interface/UIHireling.cpp` | Identify inventory-slot layout and effective owner of propagated bonuses; inspect serialization before renaming fields. |
| 1 | Solo Self Found | `src/X/Client/UserInterface/Console/UIConsoleHeroCreate.cpp`, `src/X/Client/Console/LobbyService.cpp`, `src/X/Common/Protobuf/HeroCommon.pb.cc` | Trace solo-mode UI strings to validation and saved mode flags; distinguish solo leaderboard filtering from the new mode. |
| 2 | Echoing Nightmare, Orek's Dream and loot | `src/X/Server/Definitions/DungeonFinderDefinition.cpp`, `src/X/Server/Loot/Loot.cpp`, `src/X/Common/Definitions/TreasureClassDefinition.cpp` | Recover effective definitions, then world transition/closure/reward consumers. |
| 2 | Skill, set and pet behavior | `src/X/Server/Powers/SpellEffect.cpp`, `src/X/Server/Powers/PowerAPI.cpp`, `src/X/Common/Items.cpp` | Resolve SNO/asset IDs; distinguish declarative data, script logic and native callbacks. |
| 2 | Paragon and mode-dependent limits | `src/X/Client/UserInterface/Console/UIConsoleParagon.cpp`, `src/X/Client/UserInterface/Interface/UIParagonCommon.cpp`, `src/X/Server/SPlayer.cpp` | Follow mode/season predicates before interpreting constants as universal caps. |
| 3 | Display, localization and small balance corrections | `src/X/Client/UserInterface/Interface/UIToolTips.cpp`, relevant console UI and data definitions | Prefer small validated comparisons after archive decoding is reliable. |

### Use existing function mappings as leads

The inspected `build/analysis/2.7.7/version_diff/summary.md` reports:

| Classification | Count |
| --- | ---: |
| Baseline functions | 36,559 |
| Target functions | 37,005 |
| Address-masked identical pairs | 16,272 |
| Changed pairs | 17,829 |
| Unmatched target functions (report calls these “new”) | 2,904 |
| Unmatched baseline functions (report calls these “removed”) | 2,458 |

These are a pre-existing analysis snapshot, not freshly validated semantic changes. Unmatched does not establish new/removed functionality. Inlining, function splitting, compiler changes, missing boundaries and mapping errors can produce the same categories. The port guide reports a different 3,576 unnamed-function count; that measures a different artifact/state and must not be substituted for the mapping report's 2,904 unmatched target functions.

Observed candidate-owner counts from that mapping CSV:

| Baseline source owner | Changed pairs | Identical pairs | Unmatched baseline |
| --- | ---: | ---: | ---: |
| `X/Common/Items.cpp` | 222 | 68 | 12 |
| `X/Common/ActorInventory.cpp` | 170 | 35 | 7 |
| `X/Server/SPlayer.cpp` | 211 | 44 | 27 |
| `X/Server/Powers/PowerAPI.cpp` | 134 | 6 | 0 |
| `X/Server/Powers/SpellEffect.cpp` | 350 | 71 | 12 |
| `X/Client/Console/LobbyService.cpp` | 178 | 111 | 21 |

The [port guide](port_2_7_7.md) reports a mixed compiler setup and incorrectly paired look-alike wrappers. Follow that compiler policy and verify callee consistency before attributing a mismatch to a patch. Generated Protobuf/SWIG changes can create large diff clusters without corresponding to equally many gameplay changes. No builds or checker runs were performed for this documentation task. (The mapping figures above predate the look-alike repairs in the port guide; current counts are in `build/analysis/2.7.7/version_diff/summary.md`.)

## Recovery workflow without the target NSS

1. Select a change ID and resolve its shipped/platform-specific behavior from the linked source. Record whether it is a historical step or an expected final behavior.
2. Find an asset identifier or target string. Resolve its references in the original ELF, not the rebuilt output. Record the full input hash and the virtual addresses actually used.
3. Follow callers, callees, registration tables, RTTI/vtables, residual source-path strings and serialization fields. Compare against baseline names/types and the mapping CSV, treating all transferred identities as provisional.
4. Check the effective data before editing code. A numeric set change may need no native implementation change. Validate archive override order, nonzero payload/schema and baseline-vs-target values.
5. For genuine logic changes, document branch conditions, state transitions, field offsets, call targets and constants. Compare the native implementation under the repo's chosen compiler recipe.
6. Add a focused behavioral test only where it tests a meaningful rule (for example, one special item equipped, no duplicate passive, correct pet item ownership or correct season gate). Run the repository's required target checks before marking code verified.
7. Update the record with evidence and open questions. Patch-note similarity alone must never authorize a function rename, ABI change, guessed structure field or verification status.

Suggested per-change record for future work:

```text
Change ID:
Published source and relevant subsection:
Applicability: Switch / shared / other-platform / unresolved
Activation: permanent / seasonal / mode-gated / unknown
Target ELF hash and full build version:
Archive and entry; effective-load-order evidence:
Baseline address/name -> target address; mapping method/callee check:
Observed data or disassembly evidence:
Hypothesis and alternative explanation:
Source files touched:
Validation/checker receipt:
Status: P / P+A / P+E / H / V
Unresolved questions:
```

## Open coverage and verification work

- Historical 2.6.3 changes, intermediate console-only releases and lettered hotfixes are not exhaustively reconstructed. Do not infer version continuity from the CPK filenames.
- Confirm which post-launch 2.7.7 amendments are included in `C2.7.7.92380` using content/build evidence. Public articles may have been edited after a particular build shipped.
- Resolve the 44 zero-filled 2.7.7 payloads and validate the reader before generating a numeric asset-difference report. The inventory establishes names/counts only.
- Establish actual archive lookup precedence and effective baseline/target assets. Do not assume each retained archive is an independent complete update or preserves the historical payload unchanged.
- Expand each grouped balance entry into individual rows when that system becomes active decomp work. Cosmetics, reward rotations and every tooltip/numerical tweak are not exhaustively reproduced here.
- Confirm runtime activation separately from compiled support, especially for past season themes, the Season 29 Paragon cap and non-seasonal Altar/Visions assumptions.
- Keep PC-only UI, authentication, platform services and Xbox/PlayStation graphics fixes outside Switch expectations unless independent target evidence supports them.

This guide is ready to use for prioritization and naming hypotheses; its evidence rules are intended to keep those hypotheses from turning into unsupported recovered code.
