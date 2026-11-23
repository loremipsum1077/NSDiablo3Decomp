import importlib.util
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import project_plan as p


def fn(address,quality,size=32):
    return dict(Address=hex(address),Quality=quality,Size=str(size),Name='fn'+str(address))


class PlanTests(unittest.TestCase):
    def test_exact_only_and_nonmatches_not_low_effort(self):
        job=p.source_job('X/A.cpp',[fn(1,'O'),fn(2,'M'),fn(3,'m'),fn(4,'W'),fn(5,'U')],{'source_rules':[]})
        self.assertEqual((job['exact'],job['remaining'],job['nonmatches']),(1,4,3))
        self.assertNotIn('effort',job)
        self.assertNotIn('priority',job)
        self.assertNotIn('claim',job)
        self.assertEqual(job['status'],'partial')
        self.assertEqual(p.source_job('X/A.cpp',[fn(1,'O')],{'source_rules':[]})['status'],'done')

    def test_coverage_includes_missing_owner_and_stable_ids(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'config').mkdir()
            (root/'config/project_jobs.json').write_text(json.dumps(dict(scope='scope',estimates={},source_rules=[],jobs=[])))
            functions=[fn(1,'O'),fn(2,'M'),fn(65540,'U'),fn(65560,'U')]
            owners=[{'Address':'0x1','File':'X/A.cpp'},{'Address':'0x2','File':'X/A.cpp'},{'Address':'0x10004','File':''}]
            with patch.object(p,'git',return_value='revision'),patch.object(p,'inventory',return_value=(functions,owners)):
                plan=p.snapshot(root)
            self.assertIn('no checker is run', plan['metric_evidence'])
            self.assertIn('fully linked byte-identical', plan['metric_evidence'])
            self.assertEqual(sum(j['functions'] for j in plan['jobs']),4)
            self.assertEqual(plan['totals']['unattributed'],2)
            self.assertEqual([j['id'] for j in plan['jobs'] if j['kind']=='binary'],['BIN-1'])
            self.assertEqual(len({j['id'] for j in plan['jobs']}),len(plan['jobs']))

    def test_exact_unattributed_code_still_needs_attribution(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'config').mkdir()
            (root/'config/project_jobs.json').write_text(json.dumps(dict(scope='scope',estimates={},source_rules=[],jobs=[])))
            with patch.object(p,'git',return_value='revision'),patch.object(p,'inventory',return_value=([fn(65540,'O')],[])):
                plan=p.snapshot(root)
            job=plan['jobs'][0]
            self.assertEqual((job['kind'],job['exact'],job['remaining']),('binary',1,0))
            self.assertEqual(job['status'],'partial')
            self.assertEqual(plan['totals']['unattributed'],1)
            self.assertEqual(plan['areas'][0]['done'],0)

    def test_component_leads_preserve_binary_scope_including_exact_functions(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            evidence = root / 'config'
            evidence.mkdir(parents=True)
            (root/'config/project_jobs.json').write_text(json.dumps(dict(
                scope='scope', estimates={}, source_rules=[], jobs=[])))
            (evidence/'component_rules.json').write_text(json.dumps({
                'schema_version':1, 'rules':[{
                    'id':'P03-C01', 'component':'Example component',
                    'name_patterns':['^fn'], 'confidence':'symbol-family',
                    'evidence':'Fixture only', 'follow_up':'P03-F01'}]}))
            with patch.object(p,'git',return_value='revision'), patch.object(
                    p,'inventory',return_value=([fn(65540,'O'), fn(65560,'U')], [])):
                plan = p.snapshot(root)
            job = plan['jobs'][0]
            self.assertEqual(job['status'], 'partial')
            self.assertEqual(job['component_summary'], {'Example component':2})
            self.assertEqual(len(job['attribution_records']), 2)
            self.assertEqual(len(job['remaining_functions']), 1)
            self.assertEqual(plan['totals']['unattributed'], 2)

    def test_real_inventory_reconciles_and_dependencies_exist(self):
        plan=p.snapshot()
        jobs=[j for j in plan['jobs'] if j['kind']!='milestone']
        self.assertEqual(sum(j['functions'] for j in jobs),plan['totals']['functions'])
        self.assertEqual(sum(j['bytes'] for j in jobs),plan['totals']['bytes'])
        self.assertEqual(sum(j['exact'] for j in jobs),plan['totals']['exact'])
        self.assertEqual(plan['format'],1)
        self.assertNotIn('estimates',plan)
        self.assertTrue(all('priority' not in j and 'effort' not in j and 'claim' not in j for j in plan['jobs']))
        ids={j['id'] for j in plan['jobs']}
        self.assertEqual(len(ids),len(plan['jobs']))
        graph={j['id']:j['depends_on'] for j in plan['jobs']}
        def visit(key,stack):
            self.assertNotIn(key,stack)
            for dependency in graph[key]:
                self.assertIn(dependency,ids)
                visit(dependency,stack|{key})
        for key in graph:visit(key,set())

    def test_deferral_keeps_scope_but_stops_repeat_work(self):
        job = p.source_job('X/A.cpp', [fn(1,'U')], {'source_rules':[]})
        job.update(kind='milestone', id='P99', deferred={
            'reason':'Unknown runtime target',
            'revisit_when':'A validated target-set producer exists',
            'do_not_repeat':'Treating a pointer load as proof of external targets'})
        self.assertFalse(p.actionable(job))
        ready = {**job, 'id':'P98', 'priority':'low', 'deferred':None}
        complete = {**ready, 'id':'P97', 'status':'done'}
        self.assertEqual([j['id'] for j in sorted([complete,job,ready], key=p.job_order)],
                         ['P98','P99','P97'])
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'config').mkdir()
            config=dict(scope='scope', estimates={}, source_rules=[], jobs=[job])
            path=root/'config/project_jobs.json';path.write_text(json.dumps(config))
            with patch.object(p,'git',return_value='abc'),patch.object(p,'inventory',return_value=([fn(1,'U')],[])):
                plan=p.snapshot(root)
                self.assertEqual(plan['totals']['functions'],1)
                self.assertIn('P99',[j['id'] for j in plan['jobs']])
                del config['jobs'][0]['deferred']['revisit_when']
                path.write_text(json.dumps(config))
                with self.assertRaisesRegex(ValueError,'revisit_when'):
                    p.snapshot(root)
                for invalid in ({}, None, {'reason':' ', 'revisit_when':'x', 'do_not_repeat':'x'}):
                    config['jobs'][0]['deferred'] = invalid
                    path.write_text(json.dumps(config))
                    with self.assertRaisesRegex(ValueError,'revisit_when'):
                        p.snapshot(root)

    def test_references_drop_paths_that_do_not_exist(self):
        with tempfile.TemporaryDirectory() as directory:
            (Path(directory)/'tools').mkdir()
            (Path(directory)/'tools/claim.py').write_text('')
            values=p.references(Path(directory),['tools/dashboard.py','docs/gone.md','tools/claim.py'])
            self.assertEqual(values,['tools/claim.py'])

    def test_next_never_treats_accepted_nonmatch_as_done(self):
        spec=importlib.util.spec_from_file_location('next_tool',Path(__file__).with_name('next.py'))
        module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
        for quality in ('M','m','W','U'):
            with patch.object(module,'load',return_value=({'a':quality},{'a':4},{'X/A.cpp':['a']},{})),patch.object(module,'missing_headers',return_value=[]):
                row=module.analyse()[0]
                self.assertEqual(row['o'],0)
                self.assertNotEqual(row['status'],'done')


if __name__=='__main__':unittest.main()
