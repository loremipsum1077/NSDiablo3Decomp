Template checks for DemonWare (not part of the build). Same idea as tests/blz: each file
explicitly instantiates a bd* template with arguments the binary uses, and
`python3 tools/try_file.py tests/demonware/<file>.cpp` compares every emitted function whose
mangled name is in `data/2.6.2/functions.csv`.

Find instantiations to target with `grep _ZN11bdReference data/2.6.2/functions.csv`.
