"""Render the proto2 declaration subset used by the pinned 2.6.2 FileDescriptorProtos."""
import json


def varint(data, offset):
    value = 0
    for shift in range(0, 70, 7):
        byte = data[offset]
        offset += 1
        value |= (byte & 127) << shift
        if byte < 128:
            return value, offset
    raise ValueError("invalid descriptor varint")


def fields(data, allowed):
    values = {}
    offset = 0
    while offset < len(data):
        tag, offset = varint(data, offset)
        value, offset = varint(data, offset)
        number, wire = divmod(tag, 8)
        if number not in allowed or wire not in (0, 2):
            raise ValueError(f"unsupported descriptor field {number} (wire type {wire})")
        if wire == 2:
            end = offset + value
            if end > len(data):
                raise ValueError("truncated descriptor")
            value, offset = data[offset:end], end
        values.setdefault(number, []).append(value)
    return values


def name(item, field=1):
    return item[field][0].decode("utf-8")


def enum(raw):
    item = fields(raw, {1, 2})
    lines = [f"enum {name(item)} {{"]
    for raw_value in item[2]:
        value = fields(raw_value, {1, 2})
        number = value[2][0]
        if number >= 1 << 63:
            number -= 1 << 64
        lines.append(f"  {name(value)} = {number};")
    return [*lines, "}"]


TYPES = ("", "double", "float", "int64", "uint64", "int32", "fixed64", "fixed32",
         "bool", "string", "group", "message", "bytes", "uint32", "enum", "sfixed32",
         "sfixed64", "sint32", "sint64")


def message(raw):
    item = fields(raw, {1, 2, 3, 4})
    lines = [f"message {name(item)} {{"]
    for field, render in ((3, message), (4, enum)):
        for nested in item.get(field, []):
            lines.extend("  " + line for line in render(nested))
    for raw_field in item.get(2, []):
        f = fields(raw_field, {1, 3, 4, 5, 6, 7, 8})
        label = ("", "optional", "required", "repeated")[f[4][0]]
        kind = name(f, 6) if 6 in f else TYPES[f[5][0]]
        options = []
        if 7 in f:
            default = name(f, 7)
            if kind in ("string", "bytes"):
                default = json.dumps(default)
            options.append(f"default = {default}")
        if 8 in f:
            for number, value in fields(f[8][0], {2, 3}).items():
                key = {2: "packed", 3: "deprecated"}[number]
                options.append(f"{key} = {str(bool(value[0])).lower()}")
        suffix = " [" + ", ".join(options) + "]" if options else ""
        lines.append(f"  {label} {kind} {name(f)} = {f[3][0]}{suffix};")
    return [*lines, "}"]


def schema(raw):
    item = fields(raw, {1, 2, 3, 4, 5})
    lines = ['syntax = "proto2";', f"package {name(item, 2)};"]
    lines.extend(f'import "{dep.decode()}";' for dep in item.get(3, []))
    for field, render in ((4, message), (5, enum)):
        for declaration in item.get(field, []):
            lines.extend(render(declaration))
    return "\n".join(lines) + "\n"
