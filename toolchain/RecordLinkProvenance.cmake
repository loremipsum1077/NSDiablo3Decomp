# Called only by the link rule, immediately after a successful linker invocation.
# Do not use this to attest an old ELF/map pair: relink to recreate provenance.
# This detects stale/mixed artifacts; it is not a signed ownership attestation.
if(NOT DEFINED ELF OR NOT EXISTS "${ELF}" OR NOT EXISTS "${ELF}.map")
    message(FATAL_ERROR "Link provenance requires the newly linked ELF and map")
endif()
file(SHA256 "${ELF}" ELF_SHA256)
file(SHA256 "${ELF}.map" MAP_SHA256)
file(WRITE "${ELF}.map.provenance.tmp"
    "{\"format\":1,\"elf_sha256\":\"${ELF_SHA256}\",\"map_sha256\":\"${MAP_SHA256}\"}\n")
file(RENAME "${ELF}.map.provenance.tmp" "${ELF}.map.provenance")
