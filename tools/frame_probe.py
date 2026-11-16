"""Probes a function's frame and register allocation by inserting small dead
locals at the top of its body, or (with --widths) by narrowing/widening one of
its simple local declarations, and scores each variant with tools/check.py.

Register allocation and frame size are decided together: a 4-byte local moves
the stack arguments by four and can change which register a value gets, the
same lever that made 0x685f0 match with an unused array. The probe reports
which insertion changes the target's score, so the next search starts from a
frame that acts like retail's. The source is always restored.

    python tools/frame_probe.py 0x251f8d [--widths] [--full] [--dry-run]

--fast (the default) scores a reduced image; --full builds the whole image for
each probe and is much slower. Every probe is a diagnostic: a dead local or a
probe width only shows where the frame differs. It is not a form to commit; the
body still needs a real local with real uses.

The file must have no uncommitted changes before probing: a hard stop (SIGKILL
or an outside timeout) skips the restore, and this keeps a probe from being
left in a source unnoticed. The probes are listed first so one can also be put
back by hand.
"""
import argparse
import os
import re
import subprocess
import sys

from inventory import read_rows
from reduced import find_body
from xbe import FUNCTIONS_CSV, ROOT

RESULT = re.compile(r'^(MATCH|DIFF)\s+(?P<va>[0-9a-f]{8})\b')
SCORE = re.compile(r'score (\d+)')

DEFAULT_DECLS = (
    ('4 bytes', 'byte probe[4];'),
    ('8 bytes', 'byte probe[8];'),
    ('12 bytes', 'byte probe[12];'),
    ('16 bytes', 'byte probe[16];'),
    ('32 bytes', 'byte probe[32];'),
    ('long', 'long probe;'),
)

# simple top-level declarations whose width a probe can change
DECL = re.compile(r'(?m)^(?P<indent>[ \t]*)(?P<qual>(?:(?:const|volatile)[ \t]+)*)'
                  r'(?P<type>long|dword|int|short|word|bool|byte|char)'
                  r'(?P<rest>[ \t]+\w+[ \t]*(?:=[^;\n]*)?;)')
NARROW = {'long': 'short', 'dword': 'word', 'int': 'short', 'short': 'byte', 'word': 'byte'}
WIDEN = {'short': 'long', 'word': 'dword', 'byte': 'short', 'char': 'short', 'bool': 'long'}


def parse_result(output):
    """(MATCH|DIFF, score) from a tools/check.py output, or None."""
    for line in output.splitlines():
        m = RESULT.match(line)
        if m:
            score = SCORE.search(line)
            return m.group(1), int(score.group(1)) if score else 0
    return None


def failure(output):
    """A short reason a probe did not build."""
    for line in output.splitlines():
        if 'error C' in line or 'fatal error' in line or 'CL.Exe failed' in line:
            return line.strip()[:110]
    return 'no result'


def dirty(path):
    """Whether git reports uncommitted changes to path: a probe must start from
    a clean file so a hard stop cannot leave one behind."""
    done = subprocess.run(['git', 'status', '--porcelain', '--', path],
                          cwd=ROOT, capture_output=True, text=True)
    return bool(done.stdout.strip())


def dead_probes(text, va, decls=DEFAULT_DECLS):
    """[(label, text)] with each declaration inserted at the top of the body."""
    start, _ = find_body(text, va)
    return [(label, text[:start] + '\n\t' + decl + text[start:]) for label, decl in decls]


def width_probes(text, va, lines=30):
    """[(label, text)] narrowing and widening the simple declarations near the
    top of the body. These change types, so the body's meaning changes too."""
    start, end = find_body(text, va)
    body = text[start:end]
    out = []
    for m in DECL.finditer(body):
        if body.count('\n', 0, m.start()) > lines:
            continue
        kind = m.group('type')
        for direction, table in (('narrow', NARROW), ('widen', WIDEN)):
            if kind in table:
                replacement = table[kind]
                changed = body[:m.start('type')] + replacement + body[m.end('type'):]
                out.append((f'{direction} {kind} -> {replacement}: {m.group(0).strip()}',
                            text[:start] + changed + text[end:]))
    return out


def probe(path, va, variants, full, timeout, stream=sys.stdout):
    """Run every variant, restoring the source after each. Returns
    [(label, (status, score) or a failure string)]."""
    original = open(path, encoding='utf-8', newline='').read()
    check = os.path.join(ROOT, 'tools', 'check.py')
    rows = []
    for label, text in variants:
        open(path, 'w', encoding='utf-8', newline='').write(text)
        try:
            command = [sys.executable, check] + ([] if full else ['--fast']) + [f'{va:08x}']
            try:
                done = subprocess.run(command, capture_output=True, text=True, timeout=timeout)
                output = done.stdout + done.stderr
            except subprocess.TimeoutExpired:
                rows.append((label, 'timed out'))
                stream.write(f'{label:44}  timed out\n')
                stream.flush()
                continue
            result = parse_result(output)
            if result is None:
                reason = failure(output)
                rows.append((label, reason))
                stream.write(f'{label:44}  {reason}\n')
            else:
                rows.append((label, result))
                stream.write(f'{label:44}  {result[0]} {result[1]}\n')
            stream.flush()
        finally:
            open(path, 'w', encoding='utf-8', newline='').write(original)
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('address', help='the function\'s retail address')
    ap.add_argument('--widths', action='store_true', help='also probe simple local widths (semantic)')
    ap.add_argument('--full', action='store_true', help='build the whole image for each probe (slow)')
    ap.add_argument('--timeout', type=int, default=600, help='seconds for one check (default 600)')
    ap.add_argument('--dry-run', action='store_true', help='print the probes without running them')
    args = ap.parse_args()
    va = int(args.address, 16)
    rows = read_rows(FUNCTIONS_CSV)
    source = rows.get(va, {}).get('source')
    if not source:
        raise SystemExit('no source file for that address in config/functions.csv')
    path = os.path.join(ROOT, source)
    relative = os.path.relpath(path, ROOT)
    if dirty(relative):
        raise SystemExit(f'{path} has uncommitted changes: commit or restore it '
                         'before probing, so a hard stop cannot leave a probe behind')
    original = open(path, encoding='utf-8', newline='').read()
    variants = [('baseline', original)] + dead_probes(original, va)
    if args.widths:
        variants += width_probes(original, va)
    print(f'{path}: {len(variants) - 1} probe(s)')
    print(f'  restore with: git checkout -- {relative}')
    print('  dead locals inserted at the top of the body: '
          + ', '.join(decl for _, decl in DEFAULT_DECLS))
    if args.dry_run:
        for label, _ in variants:
            print(f'  {label}')
        return 0
    rows = probe(path, va, variants, args.full, args.timeout)
    def key(row):
        label, result = row
        if label == 'baseline':
            return (-1, 0)
        if isinstance(result, str):
            return (10**6, 0)
        return (result[1], 0 if result[0] == 'MATCH' else 1)
    print('\nprobe' + ' ' * 39 + 'result')
    for label, result in sorted(rows, key=key):
        mark = ' *' if label == 'baseline' else ''
        print(f'{label:44}' + (result if isinstance(result, str) else f'{result[0]} {result[1]}') + mark)
    return 0


if __name__ == '__main__':
    sys.exit(main())
