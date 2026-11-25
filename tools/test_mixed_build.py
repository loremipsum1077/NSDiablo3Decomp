import contextlib
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

import build
import project_version


class LegacyPathTests(unittest.TestCase):
    def test_prebuilt_libraries_are_legacy_only_in_mixed_builds(self):
        with patch.object(project_version, 'MIXED', True):
            self.assertTrue(project_version.is_legacy('src/Contrib/Contrib/lua/src/lapi.c'))
            self.assertTrue(project_version.is_legacy('src/Contrib/Contrib/DemonWare/bdCore/x.cpp'))
            self.assertFalse(project_version.is_legacy('src/Contrib/Contrib/Domino/x.cpp'))
            self.assertFalse(project_version.is_legacy('src/X/Common/Items.cpp'))
        with patch.object(project_version, 'MIXED', False):
            self.assertFalse(project_version.is_legacy('src/Contrib/Contrib/lua/src/lapi.c'))

    def test_cmake_regex_matches_the_python_list(self):
        text = (Path(build.ROOT) / 'CMakeLists.txt').read_text()
        names = [d.rstrip('/').rsplit('/', 1)[1] for d in project_version.LEGACY_DIRS]
        self.assertIn('set(D3_LEGACY_REGEX "/src/Contrib/Contrib/(' + '|'.join(names) + ')/")', text)


class MixedRoutingTests(unittest.TestCase):
    def run_build(self, *argv):
        calls = []
        with tempfile.TemporaryDirectory() as temp, \
             patch.object(build, 'ROOT', Path(temp)), \
             patch.object(build, 'STOCK_501', False), \
             patch.object(build, 'LEGACY_RELATIVE', 'build/2.7.7-legacy'), \
             patch.object(build.project_version, 'LEGACY_BUILD', Path(temp) / 'build/2.7.7-legacy'), \
             patch.object(build.project_version, 'LATEPIPE', False), \
             patch.object(build.shutil, 'which', return_value='/docker'), \
             patch.object(build.resource_admission, 'lease', return_value=contextlib.nullcontext()), \
             patch.object(build.resource_admission, 'check'), \
             patch.object(build, 'check', return_value=[]), \
             patch.object(build.latepipe, 'build') as latepipe_build, \
             patch.object(build.subprocess, 'run') as docker, \
             patch.object(build, 'lua_container', side_effect=lambda *a: calls.append(('lua', a))), \
             patch.object(build, 'image_container', side_effect=lambda *a: calls.append(('main', a))), \
             patch.object(build, 'compose', side_effect=lambda *a: calls.append(('compose', a))), \
             patch.object(sys, 'argv', ['build.py', *argv]):
            docker.return_value.returncode = 0
            build.main()
            self.assertTrue(latepipe_build.called)
        return calls

    def test_legacy_phase_runs_first_with_the_2_6_2_recipe(self):
        calls = self.run_build('--jobs', '4')
        kinds = [k for k, _ in calls]
        self.assertEqual(kinds, ['lua', 'lua', 'main', 'main'])
        configure = calls[0][1]
        for flag in ('-DD3_LUA_COMPILER=ON', '-DD3_USE_LATEPIPE=ON', '-DD3_LIBRARY_SET=legacy-only'):
            self.assertIn(flag, configure)
        self.assertEqual(calls[1][1][-1], '-j4')
        main = calls[2][1]
        self.assertIn('-DD3_LIBRARY_SET=exclude-legacy', main)
        self.assertIn('-DD3_LEGACY_OBJECT_DIR=/work/build/2.7.7-legacy', main)
        self.assertIn('-DD3_USE_LATEPIPE=OFF', main)
        self.assertIn('-DD3_WEAK_PROVIDERS=OFF', main)

    def test_stock_lua_and_onboarding_are_refused(self):
        for argv in (['--no-lua-compiler'], ['--onboarding-output']):
            with self.assertRaises(SystemExit):
                self.run_build(*argv)


if __name__ == '__main__':
    unittest.main()
