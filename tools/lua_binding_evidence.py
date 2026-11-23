"""Recover binding metadata from the local DWARF and project-reviewed wrapper recipes."""
import json
from pathlib import Path
import re

from dwarf_fn import attr, load_unit, type_name

ROOT = Path(__file__).resolve().parents[1]


def wrapper_records(dies):
    records = {}
    for die in dies.values():
        if die["tag"] != "subprogram":
            continue
        name = attr(dies, die, "name").strip('"')
        if not name.startswith("_wrap_"):
            continue
        locals_ = []
        for child in die["kids"]:
            if child["tag"] == "variable":
                var = attr(dies, child, "name").strip('"')
                if re.fullmatch(r"arg\d+|result|SWIG_arg", var):
                    locals_.append([type_name(dies, child), var])
        if locals_:
            records[name[6:]] = (attr(dies, die, "decl_line"), locals_)
    return records


def native_records(owners):
    records = {}
    for owner in owners:
        path = ROOT / "build/dwarf/headers" / owner
        if not path.is_file():
            raise ValueError("Run python3 tools/gen_headers.py to generate the local DWARF headers first.")
        for line, signature, name in re.findall(
            r"^//   line +(\d+)  (.*?\b(\w+)\([^\n]*?\))  \[", path.read_text(), re.M
        ):
            records.setdefault(name, []).append((owner, line, signature))
    return records


def recover():
    recipes = json.loads((ROOT / "config/lua_binding_recipes.json").read_text())
    natives = native_records(recipes["owners"])
    wrappers = wrapper_records(load_unit(0x7f6e80))

    def row(recipe, overloaded):
        name, shape, *defaults = recipe
        native = name.split("__SWIG_")[0]
        choices = natives[native]
        if native in recipes["native_lines"]:
            choices = [c for c in choices if c[1] == recipes["native_lines"][native]]
        if len(choices) != 1:
            raise ValueError(f"ambiguous native function: {native}")
        owner, native_line, signature = choices[0]
        override = recipes["exceptions"].get(name, {})
        line, locals_ = override.get("wrapper", wrappers.get(name, (None, None)))
        if not locals_:
            raise ValueError(f"missing DWARF locals for {name}")
        if overloaded:
            # Reviewed overload recipes order SWIG_arg first, then arguments and result.
            def local_order(local):
                key = local[1]
                return int(key[3:]) if key.startswith("arg") else (-1 if key == "SWIG_arg" else 100)
            locals_ = sorted(locals_, key=local_order)
        result = dict(ShortName=name, Shape=shape, signature=signature, locals=locals_, line=line,
                      NativeTU=owner, native_line=native_line)
        if overloaded:
            result["defaults"] = defaults
        if override.get("reconstructed_arm"):
            result["reconstructed_arm"] = True
        return result

    fixed = {batch: [row(r, False) for r in rows] for batch, rows in recipes["fixed"].items()}
    dispatch = {}
    for batch, groups in recipes["dispatch"].items():
        dispatch[batch] = []
        for name, recipes_for_arms in groups:
            arms = [row([f"{name}__SWIG_{i}", *recipe], True) for i, recipe in enumerate(recipes_for_arms)]
            dispatch[batch].append(dict(ShortName=name, arms=arms))
    return dict(fixed=fixed, dispatch=dispatch)
