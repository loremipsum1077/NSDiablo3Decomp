# Whole-image byte comparison

`tools/exact_image.py` compares a candidate against the original NSO pinned in
`config/original_inputs.json`. It compares every container byte without address,
trap, padding or header normalization and reports a bounded list of differing
offset ranges. It separately inspects the three declared NSO load segments,
their memory layout, BSS and header bytes. Malformed or unsupported candidates
cannot pass by omission. File and decoded sizes are bounded.

From the repository root:

```sh
python3 tools/exact_image.py --candidate path/to/reconstructed.nso
```

The JSON goes to standard output. Exit status is 0 for literal container equality,
1 for inequality and 2 for an unverified/error result. `--original` can select
another local copy but cannot change the required original hash. `--max-ranges`
controls reported ranges, not which bytes are compared.

Every report states `accepted: false` and
`source_and_build_lineage: not_assessed`. Even comparing the original to itself
cannot certify a decompilation. Source reconstruction, a reproducible build and
independent review remain separate mandatory obligations.

## Real-input checks, 2026-11-01

| Candidate | Container bytes | Declared segments/layout/BSS |
| --- | --- | --- |
| Original NSO itself | Equal | Equal |
| Retained uncompressed NSO | Different | Equal |
| Current comparison ELF | Different | Unsupported as NSO |

The self-comparison is a sanity
check; the other cases demonstrate why a matching function report or decompressed
image does not prove original packaging. No reconstructed final NSO exists yet.

The independent bounded LZ4 block decoder follows the
[upstream block format](https://github.com/lz4/lz4/blob/dev/doc/lz4_Block_format.md).
Original-range ownership, source-built layout/link order and packaging recovery,
and integration into the final source-bound acceptance gate remain P28 work.
