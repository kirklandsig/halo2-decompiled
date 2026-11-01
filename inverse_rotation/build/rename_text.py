"""Whole-word identifier renames in docs, tests, README and spike (apply_renames --phase text
crashes when the map has no file renames).  Same globs as apply_renames.

    python build/rename_text.py <map.tsv> [--apply]
"""
import csv
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GLOBS = ('README.md', 'CONTRIBUTING.md', 'docs/**/*.md', 'tests/**/*.py', 'spike/*')
idents = {}
for r in csv.DictReader(open(sys.argv[1], encoding='utf-8'), delimiter='\t'):
    if r['class'] == 'RENAME' and r['kind'] != 'file':
        idents[r['current_name']] = r['proposed_name']
rx = re.compile(r'(?<![A-Za-z0-9_])(' + '|'.join(sorted(map(re.escape, idents), key=len, reverse=True)) + r')(?![A-Za-z0-9_])')
paths = sorted({os.path.relpath(p, ROOT).replace('\\', '/') for g in GLOBS
                for p in glob.glob(os.path.join(ROOT, g), recursive=True) if os.path.isfile(p)})
total = 0
for path in paths:
    full = os.path.join(ROOT, path)
    text = open(full, encoding='utf-8', errors='surrogateescape', newline='').read()
    n = 0

    def sub(m):
        global n
        n += 1
        return idents[m.group(1)]
    new = rx.sub(sub, text)
    if new != text:
        total += n
        print(f'{path}: {n}')
        if '--apply' in sys.argv:
            with open(full, 'w', encoding='utf-8', errors='surrogateescape', newline='') as f:
                f.write(new)
print('total', total)
