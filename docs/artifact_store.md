# Verified artifact storage

`tools/artifact_store.py` provides opt-in compressed, content-addressed snapshots.
It reads actual file contents, preserves paths and permissions, verifies retained
`done.json` seals, and publishes immutable blobs and a hashed snapshot manifest.
It never deletes the source or replaces an existing restore destination.

From the repository root:

```sh
python3 tools/artifact_store.py create --source "$RECEIPT_DIR" --store build/exact-artifact-mode-pilot
python3 tools/artifact_store.py verify --store build/exact-artifact-mode-pilot --snapshot SNAPSHOT_SHA256
python3 tools/artifact_store.py restore --store build/exact-artifact-mode-pilot --snapshot SNAPSHOT_SHA256 --destination build/new-restored-mode
```

Use the snapshot hash returned by `create`; keep that identity with the original
review evidence. Verify before restoring. Creation and restoration use resource
admission; verification is read-only. A failed restore may leave a partial new
destination and does not make it acceptable evidence. Symlinks, hardlink aliases,
unsafe paths, missing/pruned files, changing inputs, corrupt blobs and conflicting
seals are refused. Version 1 also refuses a different compressed representation
at an existing content address; compressor upgrades may require a separate store.

## Measured trial, 2026-11-01

The curl review's baseline/normal and candidate/normal mode receipts contained
750 and 752 files, totaling 536,643,486 logical bytes. Their shared store used
80,604,762 logical bytes (83,062,784 allocated on this host). The candidate reused
736 prior blobs and added 16, growing the store by 19,765,656 logical bytes.
Creation took approximately 39 and 40 seconds; each restoration took under one
second. Independent review checked every restored path, byte and permission and
the existing retained-mode verifier passed with unchanged original receipt hashes.

These are measured archive sizes, not reclaimed disk space. After independent
verification, the two temporary restored copies were removed only after another
complete manifest check.

## Remaining integration work

This is storage verification, **not full receipt or source acceptance**. Existing
receipt bundles bind absolute paths. Restoring a mode receipt elsewhere does not
prove that an entire relocated review/integration bundle passes its original
policy. Do not rewrite binding strings to make relocation appear valid.

The fixed curl-pair rehearsal below completed full retained verification at its
original bound paths. It does not establish source acceptance or exact binary
certification. General recovery still requires extending the reviewed approach to
other bundles, closing over and pinning their external dependencies, and defining
retention roots, quotas, references and safe eviction. There is no automatic
archival, eviction or garbage collection. Keep original proof until that broader
lifecycle is tested.

## Compact, isolated bundle rehearsal

`tools/artifact_bundle.py` restores one independent file for each content hash and
permission combination, then hardlinks duplicates **within the new restore only**.
It verifies every restored path, digest, permission and link count, and rejects an
external hardlink alias. Supply the original root explicitly; the destination must
be disjoint from it and from the archive. This representation is for an isolated,
read-only verification rehearsal, not a writable build tree. Do not ingest it as
a source tree: ordinary archive creation refuses hardlink aliases.

```sh
python3 tools/artifact_bundle.py --store STORE --snapshot SNAPSHOT_SHA256 \
  --original-root ORIGINAL_RECEIPT_ROOT --destination NEW_PRIVATE_DIRECTORY
```

The compact restore estimates distinct content plus 16 KiB per path and refuses
estimates above 2 GiB. Archive creation can additionally enforce a logical ceiling
on all compressed staging writes and the manifest with `--write-limit-bytes N`.
These are planning limits, not filesystem reservations or guaranteed physical
allocation limits; callers must account for existing staged archives, metadata,
temporary audit files and all other concurrent outputs when setting a total budget.
Failures can retain a partial new restore, and neither operation deletes originals.

A full receipt remains path-sensitive. A valid rehearsal must explicitly pin the
trusted verifier and inputs, expose the verified restore at the exact
bound paths in an isolated namespace with read-only mounts, and exclude the live
original receipt tree. Restoring bytes or verifying archive seals alone does not
establish full-receipt acceptance or exact binary certification.

On 2026-11-01 a full curl-pair receipt root was archived as snapshot
`af25fd32287c8d0baefd5909ff5278f0d329f1dc5c33ff37fd45e09ad3a9623a`.
Its 17,513 files totaled 2,307,607,872 logical bytes; the archive, compact
restore and pinned tools used 722,684 KiB of reported allocation together.
The isolated container mounted the restore at the original bound receipt path,
the exact pinned tools and the pinned NSS, all read-only; the original receipt
root was not mounted. The unchanged group verifier returned
`retained_bytes_verified` and the restored tree passed a second complete manifest
check. This is retained-evidence verification with `accepted: false`, not a fresh
build or strict binary certification.
