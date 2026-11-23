# TypeExtensions evidence checks

Compile the integrated source, then audit its fresh object:

```sh
COMPOSE_PROJECT_NAME=d3decomp python3 tools/try_file.py src/X/Sigma/TypeDescriptor/TypeExtensions.cpp
python3 tests/type_extensions/audit_tables.py build/try/X_Sigma_TypeDescriptor_TypeExtensions.cpp/obj.o
python3 tests/type_extensions/audit_calls.py build/try/X_Sigma_TypeDescriptor_TypeExtensions.cpp/obj.o
```

The vtable audit requires a fixed inventory of 18 tables: 17 `TypeDescriptorClass_DT_*` families and `VariantTypeDescriptor`. Each table has 344 bytes: two ABI prefix words followed by 41 virtual-method pointers. The audit verifies all 738 relocated targets against the original ELF and compares every other byte. Missing tables, extra tables, wrong sizes, unexpected or missing relocations, incorrect targets, and differing ordinary bytes fail the command. The inventory is independent of the object being checked, so an omitted table cannot silently reduce coverage.

`DT_ENUM` and `ComplexTypeDescriptor` have key functions owned elsewhere; their vtables are not expected from this source. No descriptor objects or public descriptor globals belong to this source. The original three source string globals lack surviving ELF data and are omitted rather than assigned guessed initializers.

The direct-call audit checks named calls and tail calls at corresponding instruction offsets in functions that match after relocation masking. It includes matching inline helper copies emitted by this object, including copies attributed to other source files. It reports external PLT targets separately and fails on incorrect resolved targets. This does not validate calls inside nonmatching functions, indirect-call instructions, or general instruction data references. The vtable audit separately validates virtual dispatch targets. Both scripts write detailed reports under `build/try/type_evidence/`.

These checks supplement the per-file comparison; the complete build and `tools/check` are still required for landing. The reconstruction covers all 381 functions attributed to `TypeExtensions.cpp`. Inline descriptor bodies remain in their original `DescribedType.h` or `TypeDescriptor.h` owners, while source methods follow the original DWARF order. Original DWARF, ELF bytes, and inlined caller evidence are authoritative; sibling implementations were leads only. Any remaining nonmatches are marked directly above their definitions.

Final landing validation: the isolated object matches 379/381 attributed functions; the full build matches 377/381 and passes the complete checker without losing previous matches. In addition to the two source-marked instruction-order differences, the full build selects copies of `ComplexTypeDescriptor::SizeOf` and `LoadFromDefaultData` that inline `GetField`, whereas the original calls it. Those shared-header methods are marked accordingly; isolated-object success does not establish their linked-copy match.
