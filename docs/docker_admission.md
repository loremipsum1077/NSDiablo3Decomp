# Docker interruption handling

Heavy cooperating entry points now combine the existing resource lease and disk
estimate with `docker_admission.session()`: receipt builds, project builds,
single-object compilation, Lua backend preparation and single/group integration.
The generic storage lease remains Docker-free. Receipt preflight and retained
verification do not enter a Docker session.

Before staging work, the guard checks the current Docker daemon and refuses known
active project containers, including recognized legacy names/images and Compose
build labels. It then creates and syncs a private per-user pending-session journal
under `/tmp`. Successful completion clears only that same journal, after checking
the daemon identity and project containers again. Nested calls share the journal.

An exception, interrupted process or unresolved cleanup leaves the journal behind.
The next session refuses admission even when no container is currently visible:
an earlier create request may still be in flight. This first implementation also
retains the journal after ordinary compilation failures. That is conservative and
requires review before retrying; it is not an instruction to repeat a failed build.
Forked children cannot clear a parent's journal, and caught nested failures prevent
the outer session from silently declaring success.

A failure in the final resource check does not undo completed source integration.
For landing commands, inspect the retained transaction and current Git commit
before retrying: source may already have been fast-forwarded when the guard reports
an unresolved session. Never interpret that error as a rollback.

## Inspecting an interruption

```sh
python3 tools/docker_admission.py status
```

Status takes no build lease and never grants admission. It reports whether its
journal observation remained stable while checking Docker. Inspect the recorded
session, process, retained command/logs and actual Docker state. The tool never
stops or removes a container. Do not act on an unrelated service just because it
uses a similar image or name.

After reviewing an abandoned session, an explicit override is available:

```sh
python3 tools/docker_admission.py reconcile --journal-sha256 JOURNAL_SHA256 --acknowledge-reviewed-abandoned-session
```

This requires the exact observed journal hash, an exited owner PID, the original
Docker daemon and no recognized active project containers. It refuses changed,
malformed or unsafe journals. The output labels the action as an operator-reviewed
override; it does **not** prove the absence of queued daemon work. Preserve that
output with the failure investigation. Never clear the marker merely to make the
next job start. An AI coordinator can perform reconciliation after inspecting the
same evidence; this is separate from exact-match acceptance.

## Coverage limits

The guard detects cooperating sessions and known legacy container identities, not
all possible Docker work. Other users/hosts, renamed legacy images, arbitrary manual
commands and earlier BuildKit jobs can escape classification. It does not prove
global VM idleness, reserve RAM/disk, or replace the capacity checks. New Lua image
preparation remains inside its pending session; an ambiguous failure blocks later
sessions, while earlier untracked image builds still need manual assessment.

The optional warm `docker exec` path in `try_file.py` is refused because its
externally owned container has no matching session lifecycle. Use the isolated
compiler path until warm-container ownership is explicitly implemented. Diagnostic
tools must opt into this guard; importing the resource lease alone is insufficient.
