"""Offline adapter contract tests with synthetic ELF/EH and genuine validator code."""
import json
from pathlib import Path
import struct
import tempfile
import unittest
from unittest.mock import patch

import contributor_adapter as a
import contributor_check as cc
from test_checker import write_elf, words, RET


def elf(path, object_file=False, size=4, extra=False):
    functions = [('fn', 0x1000, words(RET))]
    if extra:
        functions.append(('DO_NOT_EXPORT_PRIVATE_SYMBOL', 0x1004, words(RET)))
    write_elf(path, functions)
    blob = bytearray(path.read_bytes())
    shoff, count = struct.unpack_from('<Q', blob, 40)[0], struct.unpack_from('<H', blob, 60)[0]
    sections = [list(struct.unpack_from('<IIQQQQIIQQ', blob, shoff+i*64)) for i in range(count)]
    if object_file:
        struct.pack_into('<H', blob, 16, 1)  # ET_REL, with section-relative symbols
        sections[1][3], sections[1][5] = 0, 8 if extra else 4
        for offset in range(sections[2][4] + 24, sections[2][4] + sections[2][5], 24):
            value = struct.unpack_from('<Q', blob, offset + 8)[0]
            struct.pack_into('<Q', blob, offset + 8, value - 0x1000)
    old = sections[4]
    names = bytes(blob[old[4]:old[4]+old[5]])
    ehname = len(names); names += b'.eh_frame\0'
    relname = len(names); names += b'.rela.eh_frame\0'
    sections[4][4], sections[4][5] = len(blob), len(names)
    blob.extend(names)
    cie = b'\x01zR\0\x01\x78\x1e\x01\x1b\x0c\x1f\0'
    frame = struct.pack('<II', len(cie)+4, 0)+cie
    at = len(frame)
    pc = 0 if object_file else 0x1000-(0x6000+at+8)
    frame += struct.pack('<IIiI', 16, at+4, pc, size)+b'\0'*4
    offset = len(blob); blob.extend(frame)
    index = len(sections)
    sections.append([ehname, 1, 2, 0 if object_file else 0x6000, offset, len(frame), 0, 0, 8, 0])
    if object_file:
        offset = len(blob); blob.extend(struct.pack('<QQq', at+8, (1<<32)|261, 0))
        sections.append([relname, 4, 0, 0, offset, 24, 2, index, 8, 24])
    offset = len(blob)
    for section in sections:
        blob.extend(struct.pack('<IIQQQQIIQQ', *section))
    struct.pack_into('<Q', blob, 40, offset)
    struct.pack_into('<H', blob, 60, len(sections))
    path.write_bytes(blob)
    return path


class AdapterTests(unittest.TestCase):
    def test_report_validator_original_bindings_are_isolated(self):
        first, second = a.validators()['strict'], a.validators()['strict']
        for label, module in (('first', first), ('second', second)):
            root = self.root / label
            data = root / 'data/2.6.2'
            data.mkdir(parents=True)
            for name, value in self.tables.items():
                (data / name).write_bytes(value)
            if label == 'first':
                (data / 'main.elf').write_bytes(self.original.read_bytes())
            else:
                write_elf(data / 'main.elf', [('fn', 0x1000, words(0xd503201f))])
            module.configure(root)
        obj = self.root / 'candidate/normal/objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        self.assertTrue(first.verify(obj, [self.target])['results'][0]['exact'])
        self.assertFalse(second.verify(obj, [self.target])['results'][0]['exact'])
        self.assertTrue(first.verify(obj, [self.target])['results'][0]['exact'])

    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.original = elf(self.root/'original')
        self.owner = 'X/Owner.cpp'
        self.target = {'Name': 'fn', 'Address': '0x7100001000', 'Size': '4', 'Quality': 'U'}
        self.tables = {'functions.csv': b'Address,Quality,Size,Name\n0x7100001000,U,4,fn\n',
                       'data_symbols.csv': b'Address,Name\n'}
        self.evidence_path = self.root/'evidence.json'
        self.commits = {'baseline': 'a'*40, 'candidate': 'b'*40}
        self.value = {'format': 1, 'toolchain': {}}
        self.expected = {}
        self.links = {}
        for version, commit in self.commits.items():
            parent = self.root/version
            source = parent/'source'
            source.mkdir(parents=True)
            hashes = {}
            for name, text in {'CMakeLists.txt': 'fixture recipe', 'src/X/Owner.cpp': 'fixture source'}.items():
                path = source/name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(text)
                hashes[name] = a.digest(path)
            manifest = parent/'source-manifest.json'
            manifest.write_text(json.dumps(hashes))
            self.expected[version] = hashes
            self.value[version] = {'commit': commit, 'source': {'root': str(source), 'manifest': str(manifest), 'sha256': a.digest(manifest)}}
            for mode in ('normal', 'lua'):
                root = parent/mode
                dest = root/'build/2.6.2'
                dest.mkdir(parents=True)
                elf(dest/'d3')
                (dest/'d3.map').write_text('fixture map')
                self.provenance(dest)
                obj = root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
                obj.parent.mkdir(parents=True)
                elf(obj, True)
                self.value[version][mode] = {'root': str(root), 'receipt_sha256': self.receipt(root)}
                self.links[mode+'_'+version] = cc.verify_link_provenance(dest/'d3')
        self.write_evidence()

    def provenance(self, dest):
        (dest/'d3.map.provenance').write_text(json.dumps({'format': 1,
            'elf_sha256': a.digest(dest/'d3'), 'map_sha256': a.digest(dest/'d3.map')}))

    def receipt(self, root):
        files = {p.relative_to(root).as_posix(): a.digest(p) for p in root.rglob('*') if p.is_file() and p.name != 'done.json'}
        (root/'done.json').write_text(json.dumps({'files': files, 'full_check_rc': 0}))
        return a.digest(root/'done.json')

    def write_evidence(self):
        self.evidence_path.write_text(json.dumps(self.value))
        return a.load(self.evidence_path, a.digest(self.evidence_path), self.commits)

    def run_gates(self):
        evidence = self.write_evidence()
        return a.owner_gates(evidence, self.owner, [self.target], [self.target], self.expected,
                             self.original, a.digest(self.original), self.links, self.tables)

    def test_real_synthetic_object_extras_and_unwind_pass_without_approval(self):
        result = self.run_gates()
        self.assertEqual(result['object']['status'], 'pass')
        self.assertEqual(result['emitted_extras']['status'], 'pass')
        self.assertEqual(result['unwind']['status'], 'pass')
        self.assertEqual(result['fresh_build_lineage']['status'], 'partial')
        self.assertEqual(len(result['bindings']), 4)
        self.assertNotIn(str(self.root), json.dumps(result))
        self.assertNotIn('original_cie', json.dumps(result))

    def test_pin_and_commit_tamper_fail_closed(self):
        with self.assertRaises(ValueError):
            a.load(self.evidence_path, '0'*64, self.commits)
        self.value['candidate']['commit'] = 'c'*40
        with self.assertRaises(ValueError):
            self.write_evidence()

    def test_receipt_file_mutation_is_rejected(self):
        path = self.root/'candidate/normal/build/2.6.2/d3'
        path.write_bytes(b'changed')
        with self.assertRaisesRegex(ValueError, 'mode_receipt_file_mismatch'):
            self.run_gates()

    def test_source_manifest_cannot_claim_different_commit_content(self):
        self.expected['candidate']['src/X/Owner.cpp'] = '0'*64
        with self.assertRaisesRegex(ValueError, 'source_commit_content_mismatch'):
            self.run_gates()

    def test_traversal_symlinks_and_extra_receipt_fields_rejected(self):
        root = self.root/'candidate/normal'
        data = json.loads((root/'done.json').read_text())
        data['files']['../outside'] = '0'*64
        (root/'done.json').write_text(json.dumps(data))
        self.value['candidate']['normal']['receipt_sha256'] = a.digest(root/'done.json')
        with self.assertRaisesRegex(ValueError, 'invalid_evidence_path'):
            self.run_gates()
        del data['files']['../outside']
        data['fresh_build'] = True
        (root/'done.json').write_text(json.dumps(data))
        self.value['candidate']['normal']['receipt_sha256'] = a.digest(root/'done.json')
        with self.assertRaisesRegex(ValueError, 'invalid_integration_receipt'):
            self.run_gates()

    def test_missing_mode_or_owner_object_never_passes(self):
        obj = self.root/'candidate/normal/objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        obj.unlink()
        self.value['candidate']['normal']['receipt_sha256'] = self.receipt(self.root/'candidate/normal')
        with self.assertRaisesRegex(ValueError, 'unique_bound_owner_object'):
            self.run_gates()

    def test_new_owner_can_have_authenticated_unimplemented_baseline(self):
        source = self.root/'baseline/source'
        (source/'src/X/Owner.cpp').unlink()
        del self.expected['baseline']['src/X/Owner.cpp']
        manifest = self.root/'baseline/source-manifest.json'
        manifest.write_text(json.dumps(self.expected['baseline']))
        self.value['baseline']['source']['sha256'] = a.digest(manifest)
        for mode in ('normal', 'lua'):
            root = self.root/'baseline'/mode
            (root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj').unlink()
            self.value['baseline'][mode]['receipt_sha256'] = self.receipt(root)
        result = self.run_gates()
        self.assertEqual(result['object']['status'], 'pass')
        self.assertEqual(result['unwind']['status'], 'pass')
        self.assertIsNone(result['bindings']['normal_baseline']['object_sha256'])

    def test_unknown_extra_is_counted_without_exporting_its_name(self):
        root = self.root/'candidate/normal'
        elf(root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj', True, extra=True)
        self.value['candidate']['normal']['receipt_sha256'] = self.receipt(root)
        result = self.run_gates()
        self.assertEqual(result['emitted_extras']['status'], 'partial')
        self.assertEqual(result['emitted_extras']['modes']['normal']['extra_count'], 1)
        self.assertNotIn('DO_NOT_EXPORT', json.dumps(result))

    def test_unwind_degradation_fails(self):
        root = self.root/'candidate/lua'
        elf(root/'build/2.6.2/d3', size=8)
        self.provenance(root/'build/2.6.2')
        self.value['candidate']['lua']['receipt_sha256'] = self.receipt(root)
        self.links['lua_candidate'] = cc.verify_link_provenance(root/'build/2.6.2/d3')
        result = self.run_gates()
        self.assertEqual(result['unwind']['status'], 'fail')
        self.assertGreater(result['unwind']['regressions'], 0)
        self.assertGreater(result['unwind']['counts']['candidate']['pending'], 0)

    def test_unnamed_executable_bytes_stay_pending(self):
        root = self.root/'candidate/normal'
        path = root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        blob = bytearray(path.read_bytes())
        shoff = struct.unpack_from('<Q', blob, 40)[0]
        struct.pack_into('<Q', blob, shoff + 64 + 32, 8)
        path.write_bytes(blob)
        self.value['candidate']['normal']['receipt_sha256'] = self.receipt(root)
        result = self.run_gates()
        self.assertEqual(result['emitted_extras']['status'], 'partial')
        self.assertEqual(result['emitted_extras']['modes']['normal']['unattributed_executable_bytes'], 4)

    def test_invalid_function_ranges_fail_closed(self):
        path = self.root/'candidate/normal/objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        blob = bytearray(path.read_bytes())
        shoff = struct.unpack_from('<Q', blob, 40)[0]
        symoff = struct.unpack_from('<Q', blob, shoff + 2*64 + 24)[0]
        struct.pack_into('<Q', blob, symoff + 24 + 16, 8)
        path.write_bytes(blob)
        with self.assertRaisesRegex(ValueError, 'invalid_function_range'):
            a.executable_coverage(a.validators()['strict'], path)

    def test_original_cie_register_alignment_and_program_differences_never_pass(self):
        cie = b'\x01zR\0\x01\x78\x1e\x01\x1b\x0c\x1f\0'
        for offset, value in ((4, 2), (5, 0x70), (6, 29), (10, 30)):
            with self.subTest(field=offset):
                for mode in ('normal', 'lua'):
                    root = self.root/'candidate'/mode
                    for path in (root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj', root/'build/2.6.2/d3'):
                        elf(path, object_file=path.name == 'Owner.cpp.obj')
                        blob = bytearray(path.read_bytes())
                        at = blob.index(cie)
                        blob[at + offset] = value
                        path.write_bytes(blob)
                    self.provenance(root/'build/2.6.2')
                    self.value['candidate'][mode]['receipt_sha256'] = self.receipt(root)
                    self.links[mode+'_candidate'] = cc.verify_link_provenance(root/'build/2.6.2/d3')
                result = self.run_gates()
                self.assertEqual(result['unwind']['status'], 'fail')
                self.assertGreater(result['unwind']['counts']['candidate']['pending'], 0)

    def test_trailing_zero_operand_is_not_discarded_for_object_or_link_cie(self):
        mods = a.validators()
        strict, unwind = mods['strict'], mods['unwind']
        root = self.root/'candidate/normal'
        obj = root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        link = root/'build/2.6.2/d3'
        distinct_link = bytearray(link.read_bytes())
        distinct_link[9] = 1  # unused ELF ident padding distinguishes original
        link.write_bytes(distinct_link)
        self.assertTrue(a.exact_unwind_programs(strict, unwind, self.original, obj,
            {'normal': link, 'lua': link}, self.target))
        parse = unwind.frame
        object_bytes, linked_bytes = obj.read_bytes(), link.read_bytes()
        for altered in (object_bytes, linked_bytes):
            def frame(elf, linked):
                index, frames, cies = parse(elf, linked)
                if elf.d == altered:
                    cies = {key: body[:-1] for key, body in cies.items()}
                return index, frames, cies
            with patch.object(unwind, 'frame', side_effect=frame):
                self.assertFalse(a.exact_unwind_programs(strict, unwind, self.original, obj,
                    {'normal': link, 'lua': link}, self.target))

    def test_trailing_zero_fde_operand_is_not_discarded(self):
        mods = a.validators()
        strict, unwind = mods['strict'], mods['unwind']
        root = self.root/'candidate/normal'
        obj = root/'objects/CMakeFiles/fixture.dir/src/X/Owner.cpp.obj'
        link = root/'build/2.6.2/d3'
        blob = bytearray(link.read_bytes()); blob[9] = 1; link.write_bytes(blob)
        parse = unwind.frame
        for altered in (obj.read_bytes(), link.read_bytes()):
            def frame(elf, linked):
                index, frames, cies = parse(elf, linked)
                # DW_CFA_def_cfa_offset ULEB(0), or its truncated operand.
                frames = [dict(row, instr=b'\x0e' if elf.d == altered else b'\x0e\0') for row in frames]
                return index, frames, cies
            with patch.object(unwind, 'frame', side_effect=frame):
                self.assertFalse(a.exact_unwind_programs(strict, unwind, self.original, obj,
                    {'normal': link, 'lua': link}, self.target))

    def test_toolchain_claims_are_hash_checked_without_free_text_or_execution(self):
        tool = self.root/'compiler'
        tool.write_bytes(b'trusted fixture executable bytes; never executed')
        self.value['toolchain']['normal_compiler'] = {'path': str(tool), 'sha256': a.digest(tool), 'version': '5.0.1'}
        evidence = self.write_evidence()
        self.assertEqual(evidence['toolchain']['normal_compiler']['sha256'], a.digest(tool))
        self.value['toolchain']['normal_compiler']['version'] = 'PRIVATE_USER /home/name'
        with self.assertRaisesRegex(ValueError, 'invalid_toolchain_version'):
            self.write_evidence()

    def test_duplicate_keys_and_source_symlink_fail(self):
        self.evidence_path.write_text('{"format":1,"format":1}')
        with self.assertRaisesRegex(ValueError, 'duplicate_evidence_key'):
            a.read(self.evidence_path)
        (self.root/'candidate/source/unexpected').symlink_to(self.original)
        with self.assertRaisesRegex(ValueError, 'source_snapshot_symlink'):
            self.run_gates()

    def test_preexisting_uncheckable_is_pending_not_a_new_regression(self):
        request = [{'address': '0x10', 'name': 'prior'}, {'address': '0x20', 'name': 'target'}]
        target = [{'Address': '0x20', 'Name': 'target'}]
        result = cc.linked_summary(request, ['not-checkable', 'mismatch'], ['not-checkable', 'ok'], target)
        self.assertEqual(result['regressions'], 0)
        self.assertEqual(result['preexisting_unverifiable'], 1)
        self.assertEqual(result['status'], 'partial')
        bad = cc.linked_summary(request, ['ok', 'mismatch'], ['not-checkable', 'ok'], target)
        self.assertEqual(bad['status'], 'fail')
        missing = cc.linked_summary(request, ['not-checkable', 'not-checkable'], ['not-checkable', 'not-checkable'], target)
        self.assertEqual(missing['owner_not_ok'], 1)
        self.assertEqual(missing['status'], 'partial')


if __name__ == '__main__':
    unittest.main()
