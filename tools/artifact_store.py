#!/usr/bin/env python3
"""Opt-in, content-addressed archive of complete sealed receipt trees.

This tool preserves source files. A seal checks internal byte consistency; it
is not an independent receipt approval or a substitute for receipts.verify.

    python3 tools/artifact_store.py create --source /path/to/receipt --store /path/to/store
    python3 tools/artifact_store.py verify --store /path/to/store --snapshot <id>
    python3 tools/artifact_store.py restore --store /path/to/store --snapshot <id> --destination /new/path

Restore exclusively creates a new destination. A failed restore retains that
new partial directory and never reports success. Snapshots retain original
absolute binding strings; relocating a full receipt does not make it pass a
path-sensitive policy verifier.
"""
import argparse
from contextlib import contextmanager
import fcntl
import gzip
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import tempfile
import unicodedata
import zlib

import resource_admission

KIND = 'sealed-receipt-archive-v1'
MAX_FILES = 100000
MAX_FILE = 2 * 1024**3
MAX_TOTAL = 32 * 1024**3
MAX_MANIFEST = 32 * 1024**2
MAX_SEAL = 8 * 1024**2
CHUNK = 1024**2
HEX = re.compile(r'[0-9a-f]{64}\Z')


class ArchiveError(ValueError):
    pass


def require(ok, message):
    if not ok:
        raise ArchiveError(message)


def unique(pairs):
    out = {}
    for key, value in pairs:
        require(key not in out, 'duplicate JSON field')
        out[key] = value
    return out


def load_json(blob):
    def invalid(value):
        raise ArchiveError('invalid JSON number '+value)
    try:
        return json.loads(blob, object_pairs_hook=unique, parse_constant=invalid)
    except (UnicodeError, ValueError, RecursionError) as exc:
        raise ArchiveError('invalid JSON') from exc


def valid_path(path):
    return (isinstance(path, str) and bool(path) and len(path.encode()) <= 4096
            and not PurePosixPath(path).is_absolute() and '\\' not in path and ':' not in path
            and '\0' not in path and len(path.split('/')) <= 128
            and all(x not in ('', '.', '..') for x in path.split('/')))


def metadata(info):
    return (info.st_dev, info.st_ino, info.st_mode, info.st_size,
            info.st_mtime_ns, info.st_ctime_ns)


def reject_symlink_ancestors(path):
    path=Path(path).absolute()
    for part in reversed((path, *path.parents)):
        try:info=part.lstat()
        except FileNotFoundError:break
        require(not stat.S_ISLNK(info.st_mode), 'symlink path component: '+str(part))


def safe_open(rootfd, relative, flags=os.O_RDONLY):
    require(valid_path(relative), 'unsafe path')
    parts = relative.split('/')
    current = os.dup(rootfd)
    try:
        for part in parts[:-1]:
            new = os.open(part, os.O_RDONLY | os.O_DIRECTORY | os.O_NOFOLLOW, dir_fd=current)
            os.close(current)
            current = new
        return os.open(parts[-1], flags | os.O_NOFOLLOW | os.O_NONBLOCK, dir_fd=current)
    finally:
        os.close(current)


def regular_open(path, *, immutable=False):
    fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW | os.O_NONBLOCK)
    info=os.fstat(fd)
    if (not stat.S_ISREG(info.st_mode) or
            (immutable and (info.st_mode & 0o222 or info.st_uid != os.getuid() or info.st_nlink != 1))):
        os.close(fd)
        raise ArchiveError('expected owned immutable regular file' if immutable else 'expected regular file')
    return fd


def directory(path, create=False):
    path = Path(path)
    if create:
        try:
            path.mkdir(mode=0o700)
        except FileExistsError:
            pass
    info = path.lstat()
    require(stat.S_ISDIR(info.st_mode), 'expected nonsymlink directory: '+str(path))
    return path


def sync_dir(path):
    fd=os.open(path,os.O_RDONLY|os.O_DIRECTORY|os.O_NOFOLLOW)
    try:os.fsync(fd)
    finally:os.close(fd)


def secure_dir(path, create=False):
    path=directory(path,create=create);info=path.lstat()
    require(info.st_uid==os.getuid() and not info.st_mode & 0o022,
            'archive directory has unsafe owner or permissions')
    return path


@contextmanager
def input_lock(root):
    # Receipts keep an advisory run lock; do not ingest a running build.
    lock = root / '.lock'
    if not os.path.lexists(lock):
        yield
        return
    fd = regular_open(lock)
    try:
        try:
            fcntl.flock(fd, fcntl.LOCK_SH | fcntl.LOCK_NB)
        except BlockingIOError as exc:
            raise ArchiveError('receipt tree is active') from exc
        yield
    finally:
        os.close(fd)


def enumerate_tree(root):
    entries, total = [], 0
    def onerror(exc):
        raise exc
    for base, dirs, files in os.walk(root, followlinks=False, onerror=onerror):
        dirs.sort(); files.sort()
        for name in dirs + files:
            path = Path(base)/name
            relative = path.relative_to(root).as_posix()
            require(valid_path(relative), 'unsupported source path')
            info = path.lstat()
            require(stat.S_ISDIR(info.st_mode) or stat.S_ISREG(info.st_mode),
                    'source contains symlink or special file: '+relative)
            require(not info.st_mode & 0o7000, 'special permission bits unsupported')
            if stat.S_ISREG(info.st_mode):
                require(info.st_nlink == 1, 'hardlinked source file unsupported: '+relative)
                require(info.st_size <= MAX_FILE, 'file exceeds bound')
                total += info.st_size
            entries.append((relative, info))
            require(len(entries) <= MAX_FILES and total <= MAX_TOTAL, 'tree exceeds bound')
    return entries, total


def _snapshot_manifest(blob):
    require(len(blob) <= MAX_MANIFEST, 'manifest exceeds bound')
    obj = load_json(blob)
    require(isinstance(obj, dict) and set(obj) == {'kind','root_mode','directories','files','seals'}
            and obj['kind'] == KIND, 'unsupported archive manifest')
    require(type(obj['root_mode']) is int and 0 <= obj['root_mode'] <= 0o777, 'invalid root mode')
    require(isinstance(obj['directories'], list) and isinstance(obj['files'], list)
            and isinstance(obj['seals'], list) and 1 <= len(obj['seals']) <= MAX_FILES
            and len(obj['directories']) + len(obj['files']) <= MAX_FILES, 'invalid archive entries')
    paths = set(); folded = set(); encodings = {}
    directories = set()
    filemap = {}
    for item in obj['directories']:
        require(isinstance(item,dict) and set(item)=={'path','mode'} and valid_path(item['path'])
                and type(item['mode']) is int and 0 <= item['mode'] <= 0o777, 'invalid directory entry')
        require(item['path'] not in paths, 'duplicate archive path')
        folded_name = unicodedata.normalize('NFC',item['path']).casefold()
        require(folded_name not in folded, 'case or Unicode path collision')
        folded.add(folded_name);paths.add(item['path']); directories.add(item['path'])
    total = 0
    for item in obj['files']:
        require(isinstance(item,dict) and set(item)=={'path','mode','size','sha256','compressed_size','compressed_sha256'}
                and valid_path(item['path']) and type(item['mode']) is int and 0 <= item['mode'] <= 0o777
                and type(item['size']) is int and 0 <= item['size'] <= MAX_FILE
                and isinstance(item['sha256'],str) and HEX.fullmatch(item['sha256'])
                and type(item['compressed_size']) is int and 0 < item['compressed_size'] <= item['size'] + item['size']//1000 + 65536
                and isinstance(item['compressed_sha256'],str) and HEX.fullmatch(item['compressed_sha256']), 'invalid file entry')
        require(item['path'] not in paths, 'duplicate archive path')
        folded_name = unicodedata.normalize('NFC',item['path']).casefold()
        require(folded_name not in folded, 'case or Unicode path collision')
        folded.add(folded_name)
        encoding=(item['size'],item['compressed_size'],item['compressed_sha256'])
        require(item['sha256'] not in encodings or encodings[item['sha256']]==encoding,
                'conflicting encoding for content digest')
        encodings[item['sha256']]=encoding
        paths.add(item['path']); filemap[item['path']] = item
        total += item['size']
        require(total <= MAX_TOTAL, 'archive exceeds total bound')
    compressed_total=sum(value[1] for value in encodings.values())
    require(compressed_total <= MAX_TOTAL + MAX_TOTAL//1000 + MAX_FILES*65536, 'compressed archive exceeds bound')
    for path in paths:
        parent = PurePosixPath(path).parent
        while str(parent) != '.':
            require(parent.as_posix() in directories, 'missing parent directory')
            parent = parent.parent
    require(all(isinstance(seal,str) for seal in obj['seals']) and
            len(set(obj['seals'])) == len(obj['seals']), 'duplicate or invalid seal')
    for seal in obj['seals']:
        require(isinstance(seal,str) and seal in filemap and PurePosixPath(seal).name=='done.json'
                and (seal == 'done.json' or PurePosixPath(seal).parent.name == 'receipt'), 'invalid seal path')
        require(filemap[seal]['size'] <= MAX_SEAL, 'seal too large')
    return obj, filemap, total


def seals_match(obj, filemap, read_file):
    for seal in obj['seals']:
        data = load_json(read_file(filemap[seal]))
        require(isinstance(data,dict) and isinstance(data.get('files'),dict) and data['files'], 'invalid receipt seal')
        parent = PurePosixPath(seal).parent
        for relative, digest in data['files'].items():
            require(valid_path(relative) and isinstance(digest,str) and HEX.fullmatch(digest), 'invalid sealed file entry')
            path = (parent/PurePosixPath(relative)).as_posix()
            require(path in filemap and filemap[path]['sha256']==digest, 'missing or changed sealed file: '+path)
    if 'run.json' in filemap:
        require(filemap['run.json']['size'] <= MAX_SEAL, 'run record too large')
        run = load_json(read_file(filemap['run.json']))
        require(isinstance(run,dict) and run.get('status') in ('complete','recovery_evidence_complete')
                and isinstance(run.get('modes'),dict) and set(run['modes']) ==
                {'baseline_normal','baseline_lua','candidate_normal','candidate_lua'},
                'receipt run is not complete')
        for name, key in (('binding.json','binding_sha256'),('evidence.json','evidence_sha256')):
            require(name in filemap and run.get(key)==filemap[name]['sha256'], 'receipt run binding changed')
        for mode, entry in run['modes'].items():
            require(isinstance(entry,dict) and isinstance(entry.get('root'),str)
                    and valid_path(entry['root']), 'invalid receipt checkpoint')
            label,variant=mode.split('_',1)
            require(re.fullmatch(re.escape(label+'/'+variant) + r'/attempt-[0-9]{4,}/receipt', entry['root'])
                    and int(entry['root'].split('/')[-2].split('-')[-1]) > 0,
                    'receipt checkpoint mode path mismatch')
            seal = entry['root'] + '/done.json'
            attempt_binding = str(PurePosixPath(entry['root']).parent/'binding.json')
            require(seal in obj['seals'] and entry.get('receipt_sha256')==filemap[seal]['sha256']
                    and attempt_binding in filemap and entry.get('attempt_binding_sha256')==filemap[attempt_binding]['sha256'],
                    'receipt checkpoint omitted or changed')
        if 'audit_sha256' in run:
            audits=[name for name in ('recovery-audit.json','recovery-group-audit.json') if name in filemap]
            require(len(audits)==1 and run['audit_sha256']==filemap[audits[0]]['sha256'],
                    'receipt audit binding changed')


def blob_path(store, digest):
    require(isinstance(digest,str) and HEX.fullmatch(digest), 'invalid blob digest')
    base = secure_dir(Path(store)/'blobs')
    shard = secure_dir(base/digest[:2])
    return shard/(digest+'.gz')


def read_blob(store, item, sink=None, capture=False):
    path = blob_path(store, item['sha256'])
    fd = regular_open(path,immutable=True)
    digest = hashlib.sha256(); size = 0; captured = bytearray()
    try:
        with os.fdopen(fd,'rb') as stream:
            before=os.fstat(stream.fileno())
            require(before.st_size==item['compressed_size'], 'compressed blob size mismatch')
            compressed=hashlib.sha256();remaining=item['compressed_size']
            while remaining:
                block=stream.read(min(CHUNK,remaining))
                require(bool(block),'compressed blob truncated')
                remaining-=len(block)
                compressed.update(block)
            require(not stream.read(1),'compressed blob grew')
            require(compressed.hexdigest()==item['compressed_sha256'], 'compressed blob digest mismatch')
            stream.seek(0)
            decoder=zlib.decompressobj(16+zlib.MAX_WBITS)
            remaining=item['compressed_size']
            while remaining:
                block=stream.read(min(CHUNK,remaining))
                require(bool(block),'compressed blob truncated')
                remaining-=len(block)
                pending=block
                while pending:
                    prior=len(pending)
                    chunk=decoder.decompress(pending,min(CHUNK,item['size']-size+1))
                    pending=decoder.unconsumed_tail
                    size += len(chunk)
                    require(size <= item['size'], 'blob expands beyond recorded size')
                    digest.update(chunk)
                    if sink is not None:sink.write(chunk)
                    if capture:
                        require(size <= MAX_SEAL, 'captured seal too large')
                        captured.extend(chunk)
                    if decoder.eof:
                        require(not decoder.unused_data and not pending and remaining==0 and not stream.read(1), 'trailing compressed data')
                        break
                    require(bool(chunk) or len(pending)<prior, 'compressed decoder made no progress')
                if decoder.eof:break
            require(decoder.eof, 'truncated compressed blob')
            require(metadata(os.fstat(stream.fileno()))==metadata(before),'compressed blob changed during read')
    except (OSError, EOFError, zlib.error) as exc:
        raise ArchiveError('invalid compressed blob') from exc
    require(size == item['size'] and digest.hexdigest() == item['sha256'], 'blob size or digest mismatch')
    return bytes(captured) if capture else None


def manifest_bytes(obj):
    blob = json.dumps(obj, sort_keys=True, separators=(',',':'), ensure_ascii=False).encode('utf-8')
    require(len(blob) <= MAX_MANIFEST, 'manifest exceeds bound')
    return blob


def load_snapshot(store, snapshot):
    require(isinstance(snapshot,str) and HEX.fullmatch(snapshot), 'invalid snapshot id')
    reject_symlink_ancestors(store)
    secure_dir(store); snapshots = secure_dir(Path(store)/'snapshots')
    fd=regular_open(snapshots/(snapshot+'.json'),immutable=True)
    with os.fdopen(fd,'rb') as f:blob=f.read(MAX_MANIFEST+1)
    require(len(blob)<=MAX_MANIFEST and hashlib.sha256(blob).hexdigest()==snapshot, 'snapshot id mismatch')
    return _snapshot_manifest(blob)


def verify(store, snapshot):
    obj, filemap, total = load_snapshot(store,snapshot)
    checked = set()
    for item in obj['files']:
        if item['sha256'] not in checked:
            read_blob(store,item); checked.add(item['sha256'])
    seals_match(obj,filemap,lambda item:read_blob(store,item,capture=True))
    return {'status':'archive_verified','snapshot':snapshot,'files':len(filemap),'bytes':total,
            'receipt_acceptance':False}


class _WriteBudget:
    """Bound logical bytes written to compressed staging files and manifest.

    This does not reserve filesystem blocks or include metadata overhead.
    """
    def __init__(self, limit):
        require(limit is None or (type(limit) is int and 0 < limit <= MAX_TOTAL),
                'invalid write limit')
        self.limit = limit
        self.used = 0

    def consume(self, count):
        require(self.limit is None or self.used + count <= self.limit,
                'archive write limit exceeded')
        self.used += count

    def wrap(self, stream):
        budget = self
        class Writer:
            def write(self, data):
                budget.consume(len(data))
                return stream.write(data)
            def flush(self):
                return stream.flush()
        return Writer()


def create(source, store, write_limit_bytes=None):
    budget=_WriteBudget(write_limit_bytes)
    source=Path(source).absolute(); store=Path(store).absolute()
    reject_symlink_ancestors(source);reject_symlink_ancestors(store)
    directory(source)
    source_info=source.lstat()
    require(not source_info.st_mode & 0o7000, 'special root permission bits unsupported')
    sr=source.resolve(); dest=store.resolve(strict=False)
    require(not dest.is_relative_to(sr) and not sr.is_relative_to(dest), 'store and source must be disjoint')
    with input_lock(source):
        initial,total=enumerate_tree(source)
        # Estimate worst-case compressed payload and filesystem overhead; this is
        # an admission estimate, not a guarantee against outside activity.
        with resource_admission.lease():
            growth = max(resource_admission.DEFAULT_GROWTH, total + total//1000 + len(initial)*8192 + MAX_MANIFEST)
            resource_admission.check(store,growth_bytes=growth)
            secure_dir(store,create=True)
            sync_dir(store.parent)
            blobs=secure_dir(store/'blobs',create=True);snapshots=secure_dir(store/'snapshots',create=True)
            sync_dir(store)
            rootfd=os.open(source,os.O_RDONLY|os.O_DIRECTORY|os.O_NOFOLLOW)
            try:
                with tempfile.TemporaryDirectory(prefix='.stage-',dir=store) as tmp:
                    staged={};files=[];dirs=[]; seals=[]
                    for relative, initial_info in initial:
                        mode=stat.S_IMODE(initial_info.st_mode)
                        if stat.S_ISDIR(initial_info.st_mode):
                            dirs.append({'path':relative,'mode':mode});continue
                        fd=safe_open(rootfd,relative)
                        before=os.fstat(fd)
                        try:
                            require(stat.S_ISREG(before.st_mode) and metadata(before)==metadata(initial_info), 'source changed before read')
                        except BaseException:
                            os.close(fd);raise
                        digest=hashlib.sha256();count=0
                        temp=Path(tmp)/str(len(files))
                        with os.fdopen(fd,'rb') as inp, temp.open('xb') as out:
                            with gzip.GzipFile(fileobj=budget.wrap(out),mode='wb',filename='',mtime=0) as comp:
                                while True:
                                    block=inp.read(CHUNK)
                                    if not block:break
                                    count+=len(block);require(count<=before.st_size,'source grew during read')
                                    digest.update(block);comp.write(block)
                            out.flush();os.fchmod(out.fileno(),0o400);os.fsync(out.fileno())
                            require(count==before.st_size and metadata(os.fstat(inp.fileno()))==metadata(before), 'source changed during read')
                        encoded=hashlib.sha256()
                        with temp.open('rb') as f:
                            while True:
                                block=f.read(CHUNK)
                                if not block:break
                                encoded.update(block)
                        hexdigest=digest.hexdigest();item={'path':relative,'mode':mode,'size':count,'sha256':hexdigest,
                            'compressed_size':temp.stat().st_size,'compressed_sha256':encoded.hexdigest()}
                        files.append(item);staged.setdefault(hexdigest,(temp,item))
                        if relative=='done.json' or (PurePosixPath(relative).name=='done.json' and PurePosixPath(relative).parent.name=='receipt'):
                            seals.append(relative)
                    obj={'kind':KIND,'root_mode':stat.S_IMODE(source_info.st_mode),
                         'directories':dirs,'files':files,'seals':sorted(seals)}
                    blob=manifest_bytes(obj);obj,filemap,_=_snapshot_manifest(blob)
                    budget.consume(len(blob))  # Before any immutable publication.
                    def staged_read(item):
                        path=staged[item['sha256']][0]
                        with gzip.open(path,'rb') as data:return data.read(MAX_SEAL+1)
                    seals_match(obj,filemap,staged_read)
                    final,_=enumerate_tree(source)
                    require(metadata(source.lstat())==metadata(source_info) and
                            [(p,metadata(s)) for p,s in initial] == [(p,metadata(s)) for p,s in final], 'source tree changed')
                    touched=set()
                    for digest,(temp,item) in staged.items():
                        shard=secure_dir(blobs/digest[:2],create=True); target=shard/(digest+'.gz')
                        try:os.link(temp,target,follow_symlinks=False)
                        except FileExistsError:read_blob(store,item)
                        temp.unlink()
                        touched.add(shard)
                    for shard in sorted(touched):sync_dir(shard)
                    sync_dir(blobs)
                    snapshot=hashlib.sha256(blob).hexdigest()
                    temp_manifest=Path(tmp)/'snapshot.json'
                    with temp_manifest.open('xb') as f:
                        f.write(blob);f.flush();os.fchmod(f.fileno(),0o400);os.fsync(f.fileno())
                    try:os.link(temp_manifest,snapshots/(snapshot+'.json'),follow_symlinks=False)
                    except FileExistsError:
                        existing=regular_open(snapshots/(snapshot+'.json'),immutable=True)
                        with os.fdopen(existing,'rb') as f:require(f.read(MAX_MANIFEST+1)==blob, 'snapshot collision')
                    temp_manifest.unlink()
                    sync_dir(snapshots)
            finally:os.close(rootfd)
    return {'status':'archive_created','snapshot':snapshot,'files':len(files),'bytes':total,'receipt_acceptance':False}


def restore(store,snapshot,destination):
    store=Path(store).absolute();destination=Path(destination).absolute()
    reject_symlink_ancestors(store);reject_symlink_ancestors(destination)
    require(not destination.resolve(strict=False).is_relative_to(store.resolve()), 'destination overlaps archive')
    with resource_admission.lease():
        obj,_,total=load_snapshot(store,snapshot)
        growth=max(resource_admission.DEFAULT_GROWTH,total+len(obj['files'])*8192)
        resource_admission.check(destination.parent,growth_bytes=growth)
        verify(store,snapshot)
        os.mkdir(destination,0o700)  # Exclusive: never replace even an empty directory.
        try:
            for item in sorted(obj['directories'],key=lambda x:(x['path'].count('/'),x['path'])):
                (destination/item['path']).mkdir(mode=0o700)
            for item in obj['files']:
                path=destination/item['path']
                fd=os.open(path,os.O_WRONLY|os.O_CREAT|os.O_EXCL|os.O_NOFOLLOW,0o600)
                with os.fdopen(fd,'wb') as out:read_blob(store,item,sink=out)
                os.chmod(path,item['mode'],follow_symlinks=False)
            for item in sorted(obj['directories'],key=lambda x:(-x['path'].count('/'),x['path'])):
                os.chmod(destination/item['path'],item['mode'],follow_symlinks=False)
            os.chmod(destination,obj['root_mode'],follow_symlinks=False)
        except BaseException as exc:
            raise ArchiveError('restore incomplete; new destination retained: '+str(destination)) from exc
    return {'status':'archive_restored','snapshot':snapshot,'destination':str(destination),'receipt_acceptance':False}


def main():
    p=argparse.ArgumentParser(description=__doc__);s=p.add_subparsers(dest='command',required=True)
    a=s.add_parser('create');a.add_argument('--source',required=True);a.add_argument('--store',required=True);a.add_argument('--write-limit-bytes',type=int)
    a=s.add_parser('verify');a.add_argument('--store',required=True);a.add_argument('--snapshot',required=True)
    a=s.add_parser('restore');a.add_argument('--store',required=True);a.add_argument('--snapshot',required=True);a.add_argument('--destination',required=True)
    a=p.parse_args()
    try:
        result=(create(a.source,a.store,write_limit_bytes=a.write_limit_bytes) if a.command=='create' else verify(a.store,a.snapshot) if a.command=='verify' else restore(a.store,a.snapshot,a.destination))
    except (ArchiveError,resource_admission.AdmissionError,OSError,ValueError) as exc:
        p.exit(2,'artifact-store: '+str(exc)+'\n')
    print(json.dumps(result,indent=2));return 0

if __name__=='__main__':raise SystemExit(main())
