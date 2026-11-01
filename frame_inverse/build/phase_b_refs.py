"""List files outside src/ and include/ that mention a RENAME identifier or file.

    python build/phase_b_refs.py <map.tsv> [path-glob ...]
"""
import csv
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
rows = [r for r in csv.DictReader(open(sys.argv[1], encoding='utf-8'), delimiter='\t') if r['class'] == 'RENAME']
ids = {r['current_name'] for r in rows if r['kind'] != 'file'}
files = {r['current_name'] for r in rows if r['kind'] == 'file'}
names = {os.path.basename(f) for f in files}
pat = re.compile(r'(?<![A-Za-z0-9_])(' + '|'.join(sorted(map(re.escape, ids), key=len, reverse=True)) + r')(?![A-Za-z0-9_])')
globs = sys.argv[2:] or ['tools/*.py', 'tests/**/*.py', 'config/*', '.github/**/*', 'spike/**/*', 'docs/**/*.md', '*.md']
for g in globs:
    for p in sorted(glob.glob(os.path.join(ROOT, g), recursive=True)):
        if not os.path.isfile(p) or p.endswith('functions.csv'):
            continue
        t = open(p, encoding='utf-8', errors='replace').read()
        hits = sorted(set(pat.findall(t)))
        fh = sorted(n for n in names if re.search(r'(?<![A-Za-z0-9_])' + re.escape(n), t))
        if hits or fh:
            print(os.path.relpath(p, ROOT), '| ids', len(hits), hits[:25], '| files', len(fh), fh[:25])
