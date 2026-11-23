# D3 descriptor evidence checks

After compiling `src/X/Common/D3TypeDescriptor/D3DescribedType.cpp` with `tools/try_file.py`, run:

```sh
python3 tests/d3_descriptors/audit_tables.py build/try/X_Common_D3TypeDescriptor_D3DescribedType.cpp/obj.o
python3 tests/d3_descriptors/audit_calls.py build/try/X_Common_D3TypeDescriptor_D3DescribedType.cpp/obj.o
```

The table audit reads the original ELF and DWARF variable inventory. It requires all 20 descriptor objects, their 20 public pointers, and 20 vtables. It compares non-relocated bytes and 882 semantic pointer targets, including descriptor names and virtual methods. `DT_TIME_SHORT` intentionally retains the original `DT_TIME` class-name string.

The call audit verifies named direct calls and tail calls at corresponding instruction offsets in exact functions. Calls to external PLT entries are reported separately; the full build/check remains required. It does not audit direct calls in the three nonmatching functions, indirect calls, or general code data references. The table audit separately checks virtual dispatch targets.

The source includes all 370 attributed functions. Three have explicit `NON_MATCHING` comments after bounded natural-source attempts: TAGMAP binary deserialization (same size, flag loads/register allocation), ACTOR_ATTRIBUTE XML conversion (equivalent range-test encoding), and STARTLOC_NAME variant conversion (equivalent branch layout). Shared inline bodies are kept in their DWARF header owners. The source and original binary are the authority; sibling implementations served only as leads.
