"""Which RENAME functions does tools/check.py's call identity match by the
atlas name? Identity.ok compares an unmarked callee's plain name with the
retail row's atlas name, so renaming an unmarked function that some matched
code calls can turn a match into a mismatch.

    python build/phase_b_identity_risk.py <scratchpad>
"""
import csv, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P
sys.path.insert(0, os.path.join(P.ROOT, 'tools'))
from linkmap import plain_name

S = sys.argv[1]
rows = list(csv.DictReader(open(os.path.join(S, 'phase-b-rename-map.tsv'), encoding='utf-8'), delimiter='\t'))
ren = {r['current_name'] for r in rows if r['class'] == 'RENAME' and r['kind'] in ('function', 'method', 'class', 'type')}
texts = P.source_texts(P.ROOT)
marked = set()
for path, text in texts.items():
    if path.startswith('src/'):
        try:
            for m in P.project_build.scan(text, path):
                marked.add(m.name.rpartition('::')[2])
        except SystemExit:
            pass
stripped = {p: P.strip_code(t)[0] for p, t in texts.items()}
defined_unmarked = {}
for name in ren:
    if name in marked:
        continue
    rx = re.compile(r'\b' + re.escape(name) + r'\s*\([^;{}]*\)\s*(?:const\s*)?\{')
    hits = [p for p, c in stripped.items() if rx.search(c)]
    if hits:
        defined_unmarked[name] = hits
# atlas rows whose plain name (or its last part) is one of these, unclaimed by markers
atlas = {}
with open(os.path.join(P.ROOT, 'config', 'functions.csv'), encoding='utf-8') as f:
    for r in csv.DictReader(f):
        if r['name']:
            pn = plain_name(r['name'])
            atlas.setdefault(pn.rpartition('::')[2], []).append((r['va'], pn, r['status'], r['source']))
out = []
for name, paths in sorted(defined_unmarked.items()):
    rows_ = atlas.get(name, [])
    unclaimed = [r for r in rows_ if not r[3]]
    out.append((name, ','.join(paths)[:80], ';'.join(f'{va}:{st}' for va, _, st, _ in rows_), 'RISK' if unclaimed else 'ok'))
for o in out:
    print('\t'.join(o))
print('defined-but-unmarked RENAME functions:', len(out), 'with an unclaimed atlas row (identity fallback risk):',
      sum(1 for o in out if o[3] == 'RISK'))
