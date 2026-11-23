# LLVM 5.0.1 archive pins

`llvm.lock.json` is the canonical archive URL, size and SHA-256 lock. The normal
Docker image and optional Lua source preparation use `llvm_archive.py`; a missing
pin, unsupported platform/version, mismatched architecture/URL, truncated file or
checksum mismatch fails before extraction. Existing cache files receive the same
size/hash check and a corrupt cache is preserved for inspection, never silently
replaced. There is no checksum bypass or automatic pin update. The helper only
obtains/verifies archives; Docker and the existing Lua bootstrap do extraction.

| Selection | Exact original LLVM archive | Use |
| --- | --- | --- |
| `linux/arm64` | `clang+llvm-5.0.1-aarch64-linux-gnu.tar.xz` | ARM64 Linux compiler image (also used from ARM64 macOS) |
| `linux/amd64` | `clang+llvm-5.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz` | x86_64 Linux compiler image (cross-compiles AArch64) |
| `source` | `llvm-5.0.1.src.tar.xz` | Optional existing Lua backend bootstrap |

Empty `LLVM_ARCH` selects the mapped archive. A supplied value must match that
mapping exactly. `CLANG_VERSION` must remain `5.0.1`. Direct Docker builds may pass
`CLANG_URL` only if it equals that selection's original HTTPS URL. Compose does
not forward `CLANG_URL`; it always uses the canonical URL. Other platforms and
custom compilers require a separately reviewed policy/lock change, never a
runtime expected-hash override. The lock does not install host LLVM tools.

## Provenance and limits

The upstream [5.0.1 release directory](https://releases.llvm.org/5.0.1/) lists
archives, sizes and detached `.sig` files, **no SHA-256 checksum manifest**.
On 2026-10-22, the three exact archive URLs in the lock were downloaded over
certificate-verified HTTPS from `releases.llvm.org`, bounded to 270 MB per file
and five minutes per request, and hashed without extraction or execution.
The source result agrees with the project's previous Lua source pin.
These are **observed hashes of official HTTPS responses, not independently
authenticated vendor-published SHA-256 values**. They pin later downloads to the
reviewed bytes and detect changes/corruption; the observation still trusts the
LLVM HTTPS origin and its delivery infrastructure at that time.

Detached signatures came from each original URL plus `.sig`. Their observed
SHA-256 values are recorded in the lock, and their 287-byte original bytes are
retained in `llvm-signatures/`. Packet inspection identifies RSA signatures by
`8F0871F202119294`, created on December 19/20, 2017, using SHA-1 (OpenPGP digest
algorithm 2). LLVM's [contemporaneous mailing-list archive](https://lists.llvm.org/pipermail/llvm-dev/2017-August/116388.html)
records the full Tom Stellard fingerprint
`11E521D646982372EB577A1F8F0871F202119294`. This is corroborating identity evidence,
not verification of the three archive signatures.

**Signature authentication remains incomplete.** The current official
[release key bundle](https://releases.llvm.org/release-keys.asc) contains Stellard's
newer `474E22316ABF4785A88C6E8EA2C794A986419D8A` key, not the 2017 signer.
The LLVM website's 2022 key bundle also contains only the newer key.
The old download page's adjacent `345AD05D` link is Hans Wennborg's different key;
it must not be used as evidence that these Stellard signatures verified.
No keyserver, third-party archive/mirror, imported personal keyring, weak-digest
exception, unsigned checksum from another project or successful signature
verification was used to authenticate these pins.

To close this evidence gap, obtain the legacy public key from an authoritative
LLVM release-maintainer source and match its **full** fingerprint against the
contemporaneous record. In a new temporary GnuPG home, with automatic key
retrieval disabled, verify each retained signature against its exact original
archive, require the expected signer in `VALIDSIG`, and retain the complete
verification results. Review the legacy SHA-1 signature policy explicitly;
a vendor-authenticated SHA-256 statement for these exact bytes would provide
stronger evidence. If the key/signature cannot be accepted, ask the upstream
maintainers for authenticated archive digests. Do not mark these pins as
signature-verified, accept a different archive or update hashes on a failed check.
The runtime download verifier does not try to refresh this trust decision.

## Contributor entry point

From a contributor checkout, preview preparation without original game inputs:

```sh
# On x86_64, first: export D3_DOCKER_PLATFORM=linux/amd64
python3 tools/bootstrap.py --toolchain-plan
```

After reviewing the plan and the provenance limitation, an explicit invocation
prepares only the existing normal Linux compiler image:

```sh
python3 tools/bootstrap.py --prepare-toolchain --allow-downloads
```

This checks the supported host/platform, lock and Docker Compose. Execution also
probes the already-running Docker daemon, then runs `docker compose build build`.
It does not start Docker, change its context, use sudo, install host software,
read credentials, fetch game originals, generate inventories or compile the
game. Missing host tools and originals still require explicit user provisioning;
continue with `python3 tools/bootstrap.py --check` and the existing
[contributor startup steps](../docs/contributor_startup.md). The optional Lua
compiler is still prepared only through `tools/build.py --lua-compiler`.

Docker reuses compatible build layers/images; existing images are not a new
provisioning proof. LLVM archive pinning alone does **not** freeze `ubuntu:20.04`,
apt repository contents, packages, Python/Rust packages or host LLVM. Both image
recipes still use mutable Ubuntu/apt inputs. The optional Lua executable also
depends on GCC/package/architecture choices. No Docker build, dependency install,
original-input conversion, full game build or fresh-clone trial was run for this
change. Validation used offline synthetic downloads/caches and static recipe
checks; clean provisioning remains a later, explicitly scheduled task.
