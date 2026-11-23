"""Opt-in pilot compiler: warm container, preprocessed-input cache, SQLite timings."""
import hashlib
import json
import os
from pathlib import Path
import sqlite3
import subprocess
import time


def connect(path):
    db = sqlite3.connect(path, timeout=60)
    db.execute('PRAGMA journal_mode=WAL')
    db.executescript('''
      CREATE TABLE IF NOT EXISTS objects (key TEXT PRIMARY KEY, object BLOB, stderr TEXT);
      CREATE TABLE IF NOT EXISTS validations (
        id INTEGER PRIMARY KEY, task TEXT, started REAL, preprocessing REAL,
        compilation REAL, total REAL, cache_hit INTEGER, returncode INTEGER, mode TEXT);
    ''')
    return db


def compile(root, cmd, obj):
    started = time.time()
    db = connect(os.environ['D3_PILOT_DB'])
    container = os.environ['D3_VALIDATION_CONTAINER']
    image = subprocess.check_output(['docker','inspect','--format','{{.Image}}',container], text=True).strip()
    # Preprocessing follows the actual include graph, including newly introduced
    # headers, macros and compiler builtins. Never cache by source mtime alone.
    pre = obj.with_suffix('.ii')
    pre_cmd = list(cmd)
    pre_cmd[pre_cmd.index('-c')] = '-E'
    pre_cmd[pre_cmd.index('-o') + 1] = '/work/' + pre.relative_to(root).as_posix()
    t = time.monotonic()
    run = subprocess.run(['docker','exec',container,*pre_cmd],capture_output=True,text=True)
    preprocessing = time.monotonic()-t
    hit, compilation = False, 0.0
    if run.returncode == 0:
        key = hashlib.sha256(json.dumps([image,cmd]).encode()+pre.read_bytes()).hexdigest()
        row = db.execute('SELECT object,stderr FROM objects WHERE key=?',(key,)).fetchone()
        if row:
            obj.write_bytes(row[0])
            run = subprocess.CompletedProcess(cmd,0,'',row[1])
            hit = True
        else:
            t = time.monotonic()
            run = subprocess.run(['docker','exec',container,*cmd],capture_output=True,text=True)
            compilation = time.monotonic()-t
            if run.returncode == 0 and obj.is_file():
                db.execute('INSERT OR REPLACE INTO objects VALUES (?,?,?)',(key,obj.read_bytes(),run.stderr))
        pre.unlink(missing_ok=True)
    db.execute('INSERT INTO validations(task,started,preprocessing,compilation,total,cache_hit,returncode,mode) VALUES (?,?,?,?,?,?,?,?)',
               (os.environ.get('D3_PILOT_TASK',''),started,preprocessing,compilation,time.time()-started,int(hit),run.returncode,'warm'))
    db.commit()
    db.close()
    return run
