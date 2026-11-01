"""Supplement to phase_b_classify: tokens that appear only in comments and docs (never as a code
identifier) and still match the private name sets exactly.  Same taint and class rules as the
classifier, kind 'reference'.  Output: TSV rows in the classifier's format, class RENAME only.

    python build/extra_scan.py <scratch> <out.tsv>
"""
import collections
import csv
import glob
import os
import pickle
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P  # noqa: E402
import phase_b_classify as C  # noqa: E402

ROOT = P.ROOT
scratch, out = sys.argv[1], sys.argv[2]
# the classifier leaves its gathered data in its per-worktree temp folder
d = pickle.load(open(os.path.join(C.OUT, 'phaseb-gather.pkl'), 'rb'))
code_tokens = set(d['occ'])

texts = {}
for path, comments in d['comments'].items():
    texts[path] = '\n'.join(c for _, c in comments)
files = subprocess.run(['git', '-C', ROOT, 'ls-files', 'README.md', 'CONTRIBUTING.md', 'docs', 'tests', 'spike', 'tools'],
                       capture_output=True, text=True).stdout.split()
for f in files:
    if f.endswith(('.md', '.py', '.txt', '.csv', '.json')) and not f.startswith('config/'):
        try:
            texts[f] = open(os.path.join(ROOT, f), encoding='utf-8', errors='replace').read()
        except OSError:
            pass

uses = collections.defaultdict(list)
for path, text in texts.items():
    for ln, line in enumerate(text.split('\n'), 1):
        for t in P.IDENT.findall(line):
            if t in code_tokens:
                continue
            uses[t].append((path, ln))

rows = []
for t in sorted(uses):
    if t in P.CPP_WORDS or P.PLACEHOLDER.match(t):
        continue
    taint = C.taint_of(t, d)
    if not taint:
        continue
    if C.library_name(t, d):
        continue
    nwords = len(P.words(t))
    if nwords <= 1 and t not in C.DISTINCTIVE_SINGLES:
        continue
    generic = C.generic_idiom_keep(t, 'field')
    where = uses[t]
    locs = '; '.join(f'{p}:{ln}' for p, ln in where[:4]) + f' [{len(where)} uses]'
    rows.append((t, locs, ' | '.join(taint), 'generic-idiom?' if generic else 'distinctive'))

with open(out, 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f, delimiter='\t', lineterminator='\n')
    w.writerow(['name', 'locations', 'taint', 'note'])
    w.writerows(rows)
print(len(rows), 'tokens;', sum(1 for r in rows if r[3] == 'distinctive'), 'distinctive')
