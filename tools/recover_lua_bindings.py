#!/usr/bin/env python3
"""Regenerate recovered Lua wrapper batches from local debug evidence and reviewed recipes.

The manifest records original DWARF local names, argument/result types, source
ownership, and overload defaults corroborated by instruction comparisons. This
script writes the recovered manifest and C++ fragments under build/try; it never edits adopted headers or
the hand-maintained LuaScriptInterface_wrap.cpp. No sibling checkout is needed.

Usage: python3 tools/recover_lua_bindings.py [--output-dir PATH]
After changing a recovered shape, compile a probe with tools/try_file.py and use
the full project checker before accepting the changed binding semantics.
"""
import argparse
import json
from pathlib import Path
import re
ROOT = Path(__file__).resolve().parent.parent

def emit(r):
    name = r['ShortName']
    shape = r['Shape']
    argshape, ret = shape.split('->')
    kinds = argshape[1:-1].split(',') if argshape != '()' else []
    loc = dict(((n, t) for t, n in r['locals']))
    args = [f'arg{i + 1}' for i in range(len(kinds))]
    comment = f"Recovered trailing-default arm; neighboring original helper at line {r['line']}." if r.get('reconstructed_arm') else f"Original LuaScriptInterface_wrap.cpp:{r['line']}."
    code = f'// {comment}\nstatic int _wrap_{name}(lua_State *L)\n{{\n    int SWIG_arg = 0;\n'
    for n in args:
        code += f'    {loc[n]} {n};\n'
    if 'result' in loc:
        code += f"    {loc['result']} result;\n"
    for i, (kind, n) in enumerate(zip(kinds, args), 1):
        if kind in ['flag', 'actor', 'player', 'vfloat']:
            continue
        check = f'lua_type(L, {i}) != LUA_TBOOLEAN' if kind == 'bool' else f"!{('lua_isstring' if kind == 'string' else 'lua_isnumber')}(L, {i})"
        code += f'    if ({check}) {{\n        lua_pushfstring(L, "argument %d incorrect/missing", {i});\n        goto fail;\n    }}\n'
    for i, (kind, n) in enumerate(zip(kinds, args), 1):
        if kind == 'vfloat':
            code += f'    if (lua_isnumber(L, {i})) {{\n        float flInput = (float)lua_tonumber(L, {i});\n        if (!isfinite(flInput) || isnan(flInput)) {{\n            {{\n                CRefString sError;\n                sError.Format("Argument %d is an invalid floating point number", {i});\n                lua_pushstring(L, sError.str());\n            }}\n            goto fail;\n        }}\n        {n} = flInput;\n    }} else {{\n        lua_pushfstring(L, "argument %d incorrect/missing", {i});\n        goto fail;\n    }}\n'
        elif kind in ['actor', 'player']:
            code += f'    if (!lua_isnumber(L, {i})) {{\n        lua_pushfstring(L, "argument %d incorrect/missing", {i});\n        goto fail;\n    }}\n'
            code += f"""    {{\n        ACDID idACD = (ACDID)lua_tointeger(L, {i});\n        if (!{('sCheckPlayerACDID' if kind == 'player' else 'sCheckACDID')}(idACD)) {{\n            {{\n                CRefString sError;\n                sError.Format("Argument %d (0x%08X) is an invalid {('player actor' if kind == 'player' else 'actor')} ID", {i}, idACD);\n                lua_pushstring(L, sError.str());\n            }}\n            goto fail;\n        }}\n        {n} = idACD;\n    }}\n"""
        elif kind == 'flag':
            code += f'    if (lua_type(L, {i}) == LUA_TBOOLEAN) {{\n        {n} = lua_toboolean(L, {i});\n    }} else {{\n        if (!lua_isnumber(L, {i})) {{\n            lua_pushfstring(L, "argument %d incorrect/missing", {i});\n            goto fail;\n        }}\n        {n} = ({loc[n]})lua_tonumber(L, {i});\n    }}\n'
        else:
            conv = f'lua_tostring(L, {i})' if kind == 'string' else f'lua_toboolean(L, {i})' if kind == 'bool' else f'lua_tonumber(L, {i})'
            code += f'    {n} = ({loc[n]}){conv};\n'
    native_name = re.search('([\\w:]+)\\(', r['signature'])[1]
    code += '    ' + ('result = ' if 'result' in loc else '') + native_name + '(' + ', '.join(args + r.get('defaults', [])) + ');\n'
    if ret == 'acdlist':
        code += '    {\n        lua_createtable(L, result->nNumActorIDs, 0);\n        int nIndex = lua_gettop(L);\n        ScriptInstanceID idScriptInstance = LuaScriptGetExecutingScriptInstance();\n        for (int i = 0; i < result->nNumActorIDs; i++) {\n            lua_pushnumber(L, (lua_Number)(i + 1));\n            lua_pushnumber(L, (lua_Number)result->arActorIDs[i]);\n            if (result->bWantAddRef) {\n                LuaScriptInstanceAddRefACD(idScriptInstance, result->arActorIDs[i]);\n            }\n            lua_settable(L, nIndex);\n        }\n        SWIG_arg++;\n    }\n'
    elif ret != 'void':
        push = 'lua_pushboolean' if ret == 'bool' else 'lua_pushstring' if ret in ['string', 'refstring'] else 'lua_pushnumber'
        code += f'    {push}(L, ' + ('result.str()' if ret == 'refstring' else 'result' if ret in ['bool', 'string'] else '(lua_Number)result') + ');\n    SWIG_arg++;\n'
    code += '    return SWIG_arg;\n'
    if args:
        code += 'fail:\n    lua_error(L);\n    return -1;\n'
    code += '}\n'
    return code

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--manifest', type=Path, help='use an existing private or synthetic manifest')
    parser.add_argument('--output-dir', type=Path, default=ROOT / 'build/try/lua_bindings_regenerated')
    args = parser.parse_args()
    if args.manifest:
        manifest = json.loads(args.manifest.read_text())
    else:
        from lua_binding_evidence import recover
        try:
            manifest = recover()
        except (OSError, ValueError) as error:
            parser.exit(1, f"Lua binding recovery failed: {error}\n")
    args.output_dir.mkdir(parents=True, exist_ok=True)
    if not args.manifest:
        (args.output_dir / "bindings.json").write_text(json.dumps(manifest, indent=2) + "\n")
    count = 0
    for batch, rows in manifest['fixed'].items():
        (args.output_dir / (batch + '.inc')).write_text('\n'.join((emit(r) for r in rows)))
        count += len(rows)
    # Older private manifests may carry the full dispatcher. Current manifests
    # refer to the maintained C++ definition to avoid shipping a second copy.
    dispatchers = {}
    for batch, rows in manifest['dispatch'].items():
        parts = []
        for row in rows:
            parts.extend((emit(arm) for arm in row['arms']))
            if 'dispatcher' in row:
                parts.append(row['dispatcher'])
                continue
            if not dispatchers:
                wrapper = ROOT / 'src/X/Server/LuaScript/LuaScriptInterface_wrap.cpp'
                pattern = r'^static int _wrap_(\w+)\(lua_State \*L\)\n\{\n.*?^\}\n'
                dispatchers = {m[1]: m[0] for m in re.finditer(pattern, wrapper.read_text(), re.M | re.S)}
            parts.append(dispatchers[row['ShortName']])
        (args.output_dir / (batch + '.inc')).write_text('\n'.join(parts))
        count += len(rows)
    print(f'Recovered {count} callbacks into {args.output_dir}')
if __name__ == '__main__':
    main()
