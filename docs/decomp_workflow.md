# Decompiling a source file

Fork contributors start with the numbered [startup guide](contributor_startup.md)
and [contribution policy](../CONTRIBUTING.md). This reference describes local
shared-checkout helpers: claims are not shared across clones, fork PRs need
explicit commits, and only maintainers perform named landing after independent
review. The no-commit local shortcut below does not apply to fork submission.

The unit of work is one original source file. The goal is C++ that compiles, with clang 5.0.1
and the flags in `CMakeLists.txt`, to the same bytes as the original for every function in it.

## What you have for each file

For an original file such as `X/Sigma/Math/Rect.cpp`:

| What | Where |
|---|---|
| Its functions, addresses and sizes | `data/2.6.2/function_files.csv` joined with `functions.csv` |
| Ghidra's decompiled C for each of them | `build/ghidra/decomp/X/Sigma/Math/Rect.cpp` |
| Types, method declarations, includes, function list | `build/dwarf/headers/X/Sigma/Math/Rect.h` (and `.cpp`, `.inl`) |
| One function's prototype, locals and inlined-call tree | `python3 tools/dwarf_fn.py <mangled name>` |
| The same functions in the sibling decomp of the 1.0 build, if you have it | `python3 tools/sibling.py X/Sigma/Math/Rect.cpp` (a reference for bodies; port, do not paste) |
| Which inline helpers matter most | `data/2.6.2/inline_helpers.csv` |
| Whether each inline helper is proven yet, and which file would prove it | `data/2.6.2/helper_ledger.csv` (`python3 tools/helper_ledger.py`; `next.py --file` shows a file's rows) |
| Both disassemblies of a function, as a diff | `python3 tools/try_file.py src/<path>.cpp --diff <mangled name>` |
| The original source file and line of every instruction | `llvm-objdump -d -l --start-address=0x<vaddr> --stop-address=0x<end> build/romfs/DiabloIIINX64r.nss` (vaddr without the `0x71` prefix; give both addresses or it prints the whole binary) |
| The original bytes of a global, table or vtable | `python3 tools/data.py <symbol or 0x address>` (`--ptr` for pointers and strings) |
| All of the above for one function in one listing: file:line per instruction, named callees, resolved data references with their bytes, and the named constants its compile unit records | `python3 tools/line_evidence.py <mangled name or 0x address>` |

`build/dwarf/headers/` is a reference tree. It mirrors the original layout, including `.inl`
files, and every file ends with the list of functions defined in it with their original line
numbers. Class templates are only shown as one example instantiation and must be written by
hand.

## Loop

1. Create the source file under `src/` at its original path. Start headers from the reference:
   `python3 tools/gen_headers.py --adopt <path>` copies one into `src/` (it never overwrites),
   then trim or fix it by hand. Keep only declarations you can stand behind; it is fine to
   leave out methods you are not implementing yet.
2. Write functions in their original line order. Run `tools/dwarf_fn.py` on each first: use the
   recorded parameter and local names, and call the same inline helpers the original did, at
   the same places. A function often only matches when its helpers are structured the same way.
   Put each inline helper in the file and at the relative position the DWARF records for it.
3. `python3 tools/try_file.py src/<path>.cpp` compiles just that file and reports each function
   as `OK`, a size difference, or a count of differing instructions.
   `--diff <mangled name>` prints the two disassemblies as a diff; `--asm` prints them whole.
4. Iterate until `OK`. If a function still differs after about eight real attempts, leave your
   best version in place with a comment directly above it, `// NON_MATCHING: <what differs>`,
   and move on.

## What the DWARF does not tell you

- **Inline bodies.** A reference `.inl`, and the inline members of a reference header, have
  names and line numbers but no bodies. Write them from a caller: `dwarf_fn.py` on the caller
  shows which helpers it inlined, and `llvm-objdump -d -l` shows which instructions came from
  which line of the helper. Whether a function is inline at all matters (an empty inline
  destructor and an out-of-line one give different callers).
  A helper is only *proven* once a function it was inlined into matches; until then its body
  is a guess that every caller repeats. `data/2.6.2/helper_ledger.csv` (written by
  `python3 tools/helper_ledger.py`, from the DWARF index) has one row per inline helper: how
  many functions inline it, how many of those match (`MatchedCallers`, 0 = unproven), whether
  its file is under `src/` and whether that file has a body for it (`Bodied`: yes, no,
  absent, unknown), whether it is a static helper in a `.cpp`, and the smallest file whose
  unmatched function inlines it. `next.py --file` prints the rows for a file's own helpers.
  When a helper is unproven, write its body in the file the DWARF names and match one of its
  callers before leaning on it elsewhere.
- **Values.** Constants and initialisers are listed without values. `tools/data.py <symbol>`
  prints the bytes the original has. A global the linker dropped has no bytes; leave it out
  with a comment rather than guess.
- **Includes of a `.cpp`.** The reference `.cpp` does not list them reliably. `next.py` calls a
  file `ready` from its header's includes only, so a file can still need a header that is not
  in `src/` yet. Adopt it if it is plain; report it if it is not.
- **Static-initialiser files.** Some files' only function is `_GLOBAL__sub_I_<File>.cpp`. Its
  content is the file's global objects, in line order (the reference `.cpp` lists them). Type
  descriptor tables are the common case; `src/X/Sigma/TypeDescriptor/TypeDescriptorField.cpp`
  and the Math files show the pattern. Every header you include can add initialisers, so
  include what the original did and no more.
- **Inline functions with one out-of-line copy.** The copy is attributed to whichever compile
  unit the linker kept, which may be a file that never calls it. If your file has no reason to
  emit a function assigned to it, check it from a small file under `tests/` that takes its
  address (see `tests/blz/README.md`) and say so in your report.
- **Asserts and traps.** A call to a function that never returns is followed by a trap word
  (`udf #0xdefe`; ours is `brk`, and the tools treat them as equal). A trap word followed by
  more code on the same path is a breakpoint that returns: `__builtin_debugtrap()`.
- **Line numbers.** Asserts and log macros compile `__LINE__` into the code, so a function only
  matches when it sits on its original line (the reference `.cpp` gives each function's line).
  Keep the file's layout, including a licence header in third-party code.
- **Third-party source.** zlib, PCRE, Lua and protobuf are open source. The upstream text of the
  right version (protobuf 2.6.0, zlib 1.2.3) is the best starting point; Blizzard changed
  some of it (protobuf uses `blz::string`), and the binary decides.
- **Macros.** Several members declared on one source line in every class of a family were a
  macro. Invent one, name it plainly, and comment that the name is invented.

## Rules

- Do not paste Ghidra output. Write the source a programmer would have written; the decompiler
  output is only a guide to behaviour.
- No register pinning, inline assembly, or other tricks to force bytes.
- Names come from the DWARF. Do not invent a name where the DWARF has one. Where you have to
  invent something (the body of an inline helper, a macro), keep it minimal.
- `try_file.py` masks relocated bits, so it does not verify which function or global a call or
  load refers to. Take care to call the right callee; `tools/land.py` verifies it on the full build.

## Landing

```sh
python3 tools/land.py X/Sigma/Math/Rect.cpp   # build, check, relabel, commit
python3 tools/land.py --no-commit <file>      # the same, without the commit
python3 tools/land.py --sync                  # no source change: relabel from the checker
```

A file with a claim worktree is merged first; otherwise the main checkout is landed as
it stands. The gate is one incremental build and one `tools/check` run, usually well
under a minute:

- the full build must succeed;
- no function that was `O` may now differ from the original;
- every function the checker accepts becomes `O`;
- a function the checker cannot finish proving, because it refers to data or a jump table
  nobody has defined yet, becomes `O` when `try_file.py` finds every instruction equal;
- a function that is compiled but differs becomes `M`.

New strings, constants, globals and jump tables are allowed, and there is no limit on
the number of functions in a file. Lua-mode builds, receipts and review records are not
part of landing. `land.py` prints the number of open proof gaps; they close as the code
and data they refer to are written. On failure nothing is committed, `functions.csv` is
put back, and the checker output is in `build/land-failed.txt`.

## Working in parallel

`tools/claim.py` gives every claimed file its own checkout under `build/work/` on its own
branch, so sessions do not see each other's half-written files. `tools/land.py` merges them
into the main checkout one at a time and runs the full build and check.

- Work only in the directory `claim.py` printed for your file.
- Do not run `tools/build.py`, `tools/check` or `tools/setup.py` there, and do not edit anything
  under `data/`. `tools/try_file.py`, `tools/dwarf_fn.py`, `tools/data.py` and
  `tools/gen_headers.py --adopt` are what you use.
- Two files often need the same header. In a header that already exists, only add (a
  declaration, an inline body); never remove or reorder. Additions merge cleanly; rewrites
  conflict when the second file lands. An added constructor or inline body can change the code
  of other files that include the header: run `try_file.py` on those too.
- Run `claim.py` from your directory or the main checkout; both see the same claims.
  `build/dwarf`, `build/ghidra` and `build/romfs` in your directory are links to shared
  read-only inputs. Put scratch files under `build/try/`.
- If you need a header that does not exist in `src/` yet, adopt it when it is plain. When it is
  large or holds templates, declare the minimum you need locally if that is sound (a forward
  declaration, an `extern`) with a comment naming the header it belongs in; otherwise skip the
  function and record what was missing.
- You do not need to commit. `land.py` commits what is in the directory.
- Finish with `tools/claim.py done <file>`, or `stuck <file> "<why>"` if nothing could be written.

## Reporting

When you stop, report: files written; per file, how many functions are `OK` out of how many the
original file has; each `NON_MATCHING` function with what differs; anything you needed from
another area; and anything about the tools or this process that got in your way.
