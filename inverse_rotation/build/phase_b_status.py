"""Save or compare the status column of config/functions.csv.

    python build/phase_b_status.py save <out.tsv>
    python build/phase_b_status.py diff <baseline.tsv>
"""
import collections
import csv
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def current():
    with open(os.path.join(ROOT, 'config', 'functions.csv'), encoding='utf-8') as f:
        return {r['va']: r['status'] for r in csv.DictReader(f)}


cmd, path = sys.argv[1], sys.argv[2]
now = current()
print('counts:', dict(collections.Counter(now.values())))
if cmd == 'save':
    with open(path, 'w', encoding='utf-8', newline='') as f:
        for va, s in sorted(now.items()):
            f.write(f'{va}\t{s}\n')
else:
    base = dict(line.rstrip('\n').split('\t') for line in open(path, encoding='utf-8'))
    changes = [(va, base.get(va), now.get(va)) for va in sorted(set(base) | set(now)) if base.get(va) != now.get(va)]
    print(f'status changes: {len(changes)}')
    for va, b, a in changes[:200]:
        print(f'  {va}: {b} -> {a}')
    sys.exit(1 if changes else 0)
