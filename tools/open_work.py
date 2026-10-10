"""Lists work a contributor can claim, from a saved copy of issue #9.

The maintainers' automated lanes cover the whole game, but they yield to
contributors: a contributor may claim one source file, or an address range of
up to 0x2000 bytes, inside any maintainer lane, and the lanes then leave it
alone. Only a person's claim (an "@user" row of the Active claims table) is
off limits. This tool lists:

- source files with unmatched functions, none of them in a person's claim,
  fewest unmatched bytes first, with the claim text to put in a draft pull
  request;
- functions not decompiled yet whose callees are all done (as tools/ready.py
  lists them), outside every person's claim.

    python tools/open_work.py --claims claims.md [N]

claims.md is a saved copy of issue #9's body (for example
`gh issue view 9 -R kirklandsig/halo2-decompiled --json body -q .body > claims.md`).
The tool does not read GitHub itself.
"""
import argparse
import collections
import sys

from inventory import read_rows
from ready import claim_rows, covers, ready, _merge
from xbe import FUNCTIONS_CSV

REPO_CLAIMS = 'https://github.com/kirklandsig/halo2-decompiled/issues/9'


def person_claims(text, warn=None):
    """Merged spans of the rows a person holds ("@user"), not a maintainer lane."""
    return _merge([s for who, spans in claim_rows(text, warn)
                   if who.startswith('@') and 'maintainer' not in who.lower() for s in spans])


def open_files(rows, claimed):
    """[(file, vas, unmatched bytes, counts)] for source files with unmatched
    game functions and none in a person's claim, fewest unmatched bytes first."""
    by_file = collections.defaultdict(list)
    for va, r in rows.items():
        src = r.get('source') or ''
        if r['owner'] == 'game' and src.startswith('src/') and not src.startswith('src/stubs/'):
            by_file[src].append(va)
    out = []
    for src, vas in by_file.items():
        if any(covers(claimed, va) for va in vas):
            continue
        counts = collections.Counter(rows[va]['status'] for va in vas)
        left = sum(int(rows[va]['size']) for va in vas if rows[va]['status'] != 'matched')
        if left:
            out.append((src, sorted(vas), left, counts))
    return sorted(out, key=lambda f: (f[2], f[0]))


def claim_text(rows, entry):
    """The claim line for a draft pull request: the file's whole range when its
    functions sit within 0x2000 bytes, else just its unmatched functions."""
    src, vas = entry[0], entry[1]
    if vas[-1] - vas[0] <= 0x2000:
        return f'Retail range claimed: `0x{vas[0]:x}`-`0x{vas[-1]:x}` ({src})'
    todo = [va for va in vas if rows[va]['status'] != 'matched']
    return 'Retail range claimed: ' + ', '.join(f'`0x{va:x}`' for va in todo) + f' (the unmatched functions of {src})'


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('count', nargs='?', type=int, default=15)
    ap.add_argument('--claims', metavar='FILE', required=True,
                    help='saved copy of issue #9 (its Active claims table)')
    args = ap.parse_args()
    with open(args.claims, encoding='utf-8') as fh:
        text = fh.read()
    if not claim_rows(text):
        ap.error('no claims table found (expected a saved copy of issue #9)')
    claimed = person_claims(text, warn=lambda msg: print(f'warning: {msg}', file=sys.stderr))
    rows = read_rows(FUNCTIONS_CSV)

    files = open_files(rows, claimed)
    print(f'Open source files: {len(files)} with unmatched functions outside every person\'s claim.')
    print('Fewest unmatched bytes first. near = differs by at most two instructions.\n')
    print(f'{"file":38} {"retail range":24} {"funcs":>5} {"match":>5} {"near":>4} {"todo":>4} {"bytes left":>10}')
    for src, vas, left, c in files[:args.count]:
        span = f'0x{vas[0]:x}-0x{vas[-1]:x}'
        print(f'{src:38} {span:24} {len(vas):5} {c["matched"]:5} {c["near"]:4} {c["todo"]:4} {left:10}')
    if files:
        print('\nTo claim the first, open a draft pull request whose description says:\n  ' + claim_text(rows, files[0]))

    fresh = [r for r in ready(rows) if not covers(claimed, int(r['va'], 16))]
    print(f'\nNot decompiled yet, every callee done: {len(fresh)} functions outside every person\'s claim.')
    print('Smallest first: va, size, calls. Claim one, or a range of up to 0x2000 bytes around it.')
    for r in fresh[:args.count]:
        print(f'  {r["va"]} {r["size"]:>5}  {r["calls"] or "-"}')
    print(f'\nCheck the open pull requests too, and record your claim on {REPO_CLAIMS}.')


if __name__ == '__main__':
    main()
