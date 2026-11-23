"""Offline regressions for generation patterns and the public Lua fixture."""
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from bootstrap_game_data import adapt


class GenerationTests(unittest.TestCase):
    def test_repeated_string_validated_when_read(self):
        source = ("// Generated\n// source\n\n#define HEADER\n#include <algorithm>\n\n\n\n"
                  "  GOOGLE_CHECK(file != NULL);\n"
                  "  ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(\n"
                  "    values(i).data(),\n    values(i).length(),\n"
                  '    ::google::protobuf::internal::WireFormat::PARSE,\n    "values");\n')
        text = adapt(Path("Example.pb.cc"), source)
        self.assertIn("DO_(::google::protobuf::internal::IsStructurallyValidUTF8(\n"
                      "    values(i).data(),\n    values(i).length()));", text)
        self.assertNotIn("VerifyUTF8StringNamedField", text)

    def test_lua_fixture_works_without_private_files(self):
        root = Path(__file__).resolve().parents[1]
        with tempfile.TemporaryDirectory() as tmp:
            subprocess.run([sys.executable, root / "tools/recover_lua_bindings.py", "--manifest",
                            root / "tests/fixtures/lua_bindings.json", "--output-dir", tmp],
                           cwd=tmp, stdout=subprocess.PIPE, check=True)
            generated = (Path(tmp) / "example.inc").read_text()
            self.assertIn("result = ExampleCount(arg1);", generated)
