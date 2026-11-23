import unittest
import hashlib
from pathlib import Path
from tempfile import TemporaryDirectory
from unittest.mock import patch

from runtime_readiness import audit, classify


class ClassificationTests(unittest.TestCase):
    def test_hashes_identify_parsed_inputs(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            contents = [b'Address,Name\r\n0x1,target\r\n', b'Address,Name\n',
                        b'Address,File\n0x1,owner.cpp\n']
            paths = [root / name for name in ('functions.csv', 'data.csv', 'owners.csv')]
            for path, blob in zip(paths, contents):
                path.write_bytes(blob)
            symbol = dict(name='target', binding=1, type=0, defined=False,
                          table_section=4, index=1)
            # No ELF file is needed: metadata must come from the parser's snapshot.
            with patch('runtime_readiness.inspect_elf', return_value={
                    'sha256': 'parser-snapshot', 'symbols': [symbol]}):
                result = audit(root / 'elf', *paths)
            self.assertEqual(result['inputs']['elf']['sha256'], 'parser-snapshot')
            for key, blob in zip(('functions', 'data', 'owners'), contents):
                self.assertEqual(result['inputs'][key]['sha256'], hashlib.sha256(blob).hexdigest())
            self.assertEqual(result['imports'][0]['function_candidates'][0]['owners'], ['owner.cpp'])

    def test_exact_names_ambiguity_bindings_and_deduplication(self):
        def symbol(name, binding=1, typ=0, defined=False, index=1):
            return dict(name=name, binding=binding, type=typ, defined=defined,
                        table_section=4, index=index)
        rows = classify([symbol('same'), symbol('same', index=2), symbol('weak', 2),
                         symbol('defined', defined=True), symbol('local', 0), symbol('')],
                        [dict(Name='same', Address='0x1'), dict(Name='same', Address='0x2')],
                        [dict(Name='same', Address='0x3')],
                        [dict(Address='0x1', File='one.cpp')])
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0]['category'], 'function_and_data_inventory')
        self.assertEqual(len(rows[0]['occurrences']), 2)
        self.assertEqual(rows[0]['function_candidates'][0]['owners'], ['one.cpp'])
        self.assertEqual(rows[0]['function_candidates'][1]['owners'], [])
        self.assertEqual(rows[1]['category'], 'unclassified')
        self.assertEqual(rows[1]['binding'], 2)


if __name__ == '__main__':
    unittest.main()
