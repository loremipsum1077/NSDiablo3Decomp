# Original input identity

The [versioned manifest](../config/original_inputs.json) records local reference
SHA-256 hashes and exact byte sizes for **Switch 2.6.2**. Paths are relative to
your private input root (`build/` by default). No original files are supplied.

From the repository root, with Python 3.10+ and your own extracted inputs:

```sh
python3 -B tools/verify_original_inputs.py
```

This one command needs no provider, companion, submodule, extra Python package,
LLVM, Docker or network. For an existing private dump elsewhere, use
`--input-root /path/to/own/dump` containing `exefs/` and `romfs/`, or select
individual originals with `--nso FILE --nss FILE`. Explicit paths still require
the exact manifest identity. No originals need to be copied into the checkout.

| Input relative to input root | Use |
| --- | --- |
| `exefs/main` | Required original compressed NSO executable |
| `romfs/DiabloIIINX64r.nss` | Required original unstripped ELF64 AArch64 debug/type/owner evidence |
| `romfs/DiabloIIINX64r.nso` | Optional original executable evidence, identical bytes to `exefs/main` |
| `romfs/DiabloIIINX64ra.nss` | Optional debug evidence for a different program; never substitutes for `r.nss` |

The required executable also occurs at
`romfs/DiabloIIINX64r.nspd/program0.ncd/code/main`. This and the RomFS `r.nso`
are recorded as equivalent filenames, confirmed byte-identical locally. Use
`--nso FILE` to select one explicitly; the verifier never searches for fallback
files. There is one accepted size/hash per role, with no relaxed, decompressed,
renamed `ra`, different-version or arbitrary ELF variant. A filename change alone
does not establish identity.

Exit **0** means all required files match, and every present optional file matches.
Absent optional evidence is reported as `missing_optional` and is allowed. A
present optional mismatch fails this full verification command. Exit **1** reports
`missing`, `wrong_size`, `wrong_hash`, unsupported version/platform, an unsafe
path, an unreadable/special file, or a file that changed during hashing. Diagnostics
use manifest-relative filenames, not supplied private paths. Regular files are
streamed in 1 MiB blocks; device/inode, mode, size, mtime and ctime are checked
before opening, on the open descriptor before/after reading and at the pathname
after reading. Symlink files and symlink directories inside the input layout are
rejected. This detects ordinary concurrent replacement/modification, not a
forensic proof against a hostile filesystem.

`tools/bootstrap.py --check` always verifies both required hashes and sizes;
`--hash-inputs` now only displays their verified hashes. `tools/setup.py` enforces
the same required-input check before installing a venv, downloading tools,
converting, regenerating inventories or compiling the checker. Optional evidence
does not gate setup/bootstrap. This also gates the setup stage of
`tools/reproduce.py --run`; that runner's separate preflight report alone is not
input acceptance. Run the command above before using it directly. No game build
is needed to verify originals. Setup/build side effects still require the
explicit steps in [contributor startup](contributor_startup.md).

The `tools/setup.py` literal `NSO_SHA256` remains a compatibility mirror
for existing contributor-check AST readers. Manifest loading rejects any mismatch
between it and the manifest. The converted ELF pin in setup is a separate
**generated-output** check, not another accepted original. Expectations are
committed/reviewed; there is no command to regenerate them from arbitrary files.

## Local provenance and limits

On 2026-10-21, the locally held extraction-layout files were read without copying,
converting, modifying or uploading them. Streaming SHA-256 and sizes were stable
across before/after pathname/descriptor stats for all four manifest entries and
the packaged `r` executable. The observed ExeFS `main` matched the pre-existing
setup pin at baseline `a1309b67896690dac9842cbb00e4607882fa1cf5`, as did both
original-layout executable equivalents. This identifies a compressed original
NSO rather than the generated `main.uncompressed.nso` or `main.elf`.

The locally held `r.nss` is an ELF64 little-endian AArch64 file with `.debug_info`
and `.debug_line`. Its GNU build ID is `eafc2cef9b161d20514260d11266b5ae`, matching
the NSO build ID after its zero padding. Each of its three loadable text/rodata/data
segments also hashes to the corresponding uncompressed segment digest stored in
the pinned NSO. These checks corroborate the pairing; a plausible filename or ELF
header alone was not used to declare it the reference. `ra.nss` has the distinct
build ID `7247a8c62a57a0fe030f19a3d717bde4`.

The NSS and optional-file hashes are **observed local reference identities**.
There is no independently verified publisher checksum, extraction receipt or
chain of custody proving every debug byte was untouched before this
observation. No publisher-certified canonical hash or full debug completeness
claim is made. A different legitimate encoding is unsupported until its original
provenance and exact variant are separately reviewed; do not rename a substitute
or replace expected hashes to make verification pass.

`data/2.6.2/main.elf`, `data/2.6.2/main.uncompressed.nso`, DWARF caches, recovered
headers, inventories, checker executables and rebuilt objects/ELFs are generated
intermediates, not original inputs. Other ExeFS/RomFS assets, `rtld`, `sdk`, NPDM,
keys and the proprietary SDK/compiler are not consumed by this setup's original
input checks; they are not additional requirements for this verification command.
No game material was downloaded and no original was added to Git.

Hash consistency is an identity check, not proof of ownership or permission to
use, redistribute or publish anything. Keep originals private and do not attach
them to issues, PRs, CI caches or reports.
