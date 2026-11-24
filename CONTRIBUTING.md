# Contributing

This is a personal project, shared as it is. **It is not maintained.** Issues and
pull requests will not be reviewed or merged, and there is no chat server or other
place where work is coordinated.

If you want to take it further, you are encouraged to do so in your own fork. You
do not need to ask: fork it, change it, and run it however you like, within the
licence terms below and the law where you live.

## If you fork it

The tools and documents that were used to build this decompilation are all here
and still work on their own:

- [Contributor startup](docs/contributor_startup.md): prerequisites and the
  `python3 setup.py` wizard.
- [Decompilation workflow](docs/decomp_workflow.md): evidence tools, `try_file.py`,
  and landing a file with `python3 tools/land.py`.
- [Validation](docs/validation.md) and [checker policy](docs/checker_policy.md):
  what "matching" means and how it is checked.
- [Remaining functions](docs/remaining_functions.md): every function that does not
  match byte for byte, with its reason.
- [Documentation index](docs/index.md) for everything else.

The local claim tool (`tools/claim.py`) only coordinates sessions sharing one
checkout. Coordinating work across people is up to whoever runs the fork.

## Licence and game material

Project-authored tooling in the paths listed in [REUSE.toml](REUSE.toml) is under
the [MIT License](LICENSES/MIT.txt). Third-party code keeps its own notices. The
licence grants no rights in the game.

Never commit or publish game files, keys, dumps, disassembly or anything else taken
from the game. Each person supplies their own legally obtained copy (see
[Getting the game files](README.md#getting-the-game-files)).
