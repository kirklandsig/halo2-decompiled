"""Sweeps a source file's "// @flags" variants and reports each target's score.

Most unmatched functions keep a few compiler decisions that a different
translation-unit flag set changes: the register allocation, the inlining
budget, the frame pointer, or SSE. This tool rewrites the file's
"// @flags ..." line, runs tools/check.py on the target addresses, and
restores the source. It varies one flag axis at a time from the file's
current flags (plus a few combinations), so the result says which decision
the remaining differences depend on.

    python tools/flag_sweep.py 0xa6430 [0xa6440 ...] [--full] [--dry-run]

The addresses must share one source file. It reads config/functions.csv for
the file, and runs in the current environment (set XDK_DIR as for
tools/check.py). --fast sweeps a reduced image (the default); --full runs the
whole image for each variant and is much slower.

The file must have no uncommitted changes before a sweep: a hard stop (SIGKILL
or an outside timeout) skips the restore, and this keeps a source from being
left with a variant's flags unnoticed. The current flags line is printed first
so it can also be put back by hand.
"""
import argparse
import os
import re
import subprocess
import sys

from inventory import read_rows
from xbe import FUNCTIONS_CSV, ROOT

FLAGS_LINE = re.compile(r'^(?P<pre>\s*//\s*@flags\s+)(?P<flags>.*?)(?P<post>\s*)$')
RESULT = re.compile(r'^(MATCH|DIFF)\s+(?P<va>[0-9a-f]{8})\b')
SCORE = re.compile(r'score (\d+)')
FLAGS_LINES = 30  # tools/build.py reads the line among the first 30

# flag axis: (name, the tokens it owns, the spellings to try)
AXES = (
    ('optimization', re.compile(r'^/O[012dx]$'), ('/Od', '/O1', '/O2', '/Ox')),
    ('favor', re.compile(r'^/O[st]$'), ('/Os', '/Ot')),
    ('inlining', re.compile(r'^/Ob[012]$'), ('/Ob0', '/Ob1', '/Ob2')),
    ('frame pointer', re.compile(r'^/Oy-?$'), ('/Oy', '/Oy-')),
    ('intrinsics', re.compile(r'^/Oi-?$'), ('/Oi', '/Oi-')),
    ('exceptions', re.compile(r'^/GX-?$|^/EH\w*$'), ('/GX', '/GX-')),
    ('SSE', re.compile(r'^/arch:SSE2?$'), ('/arch:SSE', '/arch:SSE2')),
    # /Gr is what the build adds; /Gd and /Gz are the alternatives worth trying
    ('convention', re.compile(r'^/G[rdsz]$'), ('/Gd', '/Gz')),
)

# combinations that change two decisions the ties usually follow together
COMBINATIONS = (
    ('/O1', '/Ob1'),
    ('/O2', '/Ob1'),
    ('/Ob0',),
    ('/Ob1',),
    ('/Oy',),
    ('/GX-',),
)


def parse_flags(text):
    """The file's current "// @flags ..." tokens, or [] when it has none."""
    for line in text.splitlines()[:FLAGS_LINES]:
        m = FLAGS_LINE.match(line)
        if m:
            return m.group('flags').split()
    return []


def with_flags(text, flags):
    """text with its "// @flags ..." line set to flags (the line is inserted
    at the top when the file has none)."""
    lines = text.splitlines(keepends=True)
    for i, line in enumerate(lines[:FLAGS_LINES]):
        body = line.rstrip('\r\n')
        ending = line[len(body):]
        m = FLAGS_LINE.match(body)
        if m:
            lines[i] = m.group('pre') + ' '.join(flags) + (m.group('post') or '') + ending
            return ''.join(lines)
    return '// @flags ' + ' '.join(flags) + '\n' + text


def _has(flags, token):
    """Whether an axis token matching token's axis is already present."""
    axis = next((pattern for name, pattern, options in AXES if token in options), None)
    return axis is not None and any(axis.match(t) for t in flags)


def _with(flags, token):
    """flags with token in place of its axis's token, or appended."""
    axis = next((pattern for name, pattern, options in AXES if token in options), None)
    out = list(flags)
    if axis is not None:
        for i, t in enumerate(out):
            if axis.match(t):
                out[i] = token
                return tuple(out)
    if token not in out:
        out.append(token)
    return tuple(out)


def variants(flags):
    """The current flags, one-axis alternatives, and the combinations, deduped
    and with the baseline first."""
    base = tuple(flags)
    out = [base]
    seen = {base}
    for name, pattern, options in AXES:
        for token in options:
            v = _with(base, token)
            if v != base and v not in seen:
                out.append(v)
                seen.add(v)
    for combination in COMBINATIONS:
        v = base
        for token in combination:
            v = _with(v, token)
        if v != base and v not in seen:
            out.append(v)
            seen.add(v)
    return out


def results(output):
    """{va: (MATCH|DIFF, score)} from a tools/check.py run."""
    found = {}
    for line in output.splitlines():
        m = RESULT.match(line)
        if m:
            score = SCORE.search(line)
            found[int(m.group('va'), 16)] = (m.group(1), int(score.group(1)) if score else 0)
    return found


def _failure(output):
    """A short reason a variant did not build, for the report."""
    for line in output.splitlines():
        if 'error C' in line or 'fatal error' in line or 'CL.Exe failed' in line:
            return line.strip()[:120]
    return 'no result'


def dirty(path):
    """Whether git reports uncommitted changes to path: a sweep must start from
    a clean file so a hard stop cannot leave a variant's flags behind."""
    done = subprocess.run(['git', 'status', '--porcelain', '--', path],
                          cwd=ROOT, capture_output=True, text=True)
    return bool(done.stdout.strip())


def sweep(path, targets, flag_sets, full, timeout, dry, stream=sys.stdout):
    """Run every flag set against the targets, restoring the file each time.
    Returns [(flags, {va: (status, score)} or a failure string)]."""
    original = open(path, encoding='utf-8', newline='').read()
    check = os.path.join(ROOT, 'tools', 'check.py')
    rows = []
    try:
        for flags in flag_sets:
            if dry:
                stream.write('would run: ' + ' '.join(flags) + '\n')
                continue
            open(path, 'w', encoding='utf-8', newline='').write(with_flags(original, flags))
            command = [sys.executable, check] + ([] if full else ['--fast'])
            command += [f'{va:08x}' for va in targets]
            try:
                done = subprocess.run(command, capture_output=True, text=True, timeout=timeout)
                output = done.stdout + done.stderr
            except subprocess.TimeoutExpired:
                rows.append((flags, 'timed out'))
                stream.write(f'{" ".join(flags):44}  timed out\n')
                stream.flush()
                continue
            found = results(output)
            missing = [va for va in targets if va not in found]
            if missing:
                reason = _failure(output)
                rows.append((flags, reason))
                stream.write(f'{" ".join(flags):44}  {reason}\n')
            else:
                rows.append((flags, found))
                stream.write(f'{" ".join(flags):44}  ' + '  '.join(
                    f'{va:08x} {found[va][0]} {found[va][1]}' for va in targets) + '\n')
            stream.flush()
    finally:
        open(path, 'w', encoding='utf-8', newline='').write(original)
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('addresses', nargs='+', help='retail addresses, all in one source file')
    ap.add_argument('--full', action='store_true', help='build the whole image for each variant (slow)')
    ap.add_argument('--timeout', type=int, default=600, help='seconds for one check (default 600)')
    ap.add_argument('--dry-run', action='store_true', help='print the flag sets without running them')
    args = ap.parse_args()
    wanted = [int(a, 16) if a.lower().startswith('0x') else int(a, 16) for a in args.addresses]
    rows = read_rows(FUNCTIONS_CSV)
    paths = {rows[va].get('source') for va in wanted if va in rows}
    if len(paths) != 1 or None in paths:
        raise SystemExit('the addresses must all be in one source file recorded in config/functions.csv')
    path = os.path.join(ROOT, paths.pop())
    if dirty(os.path.relpath(path, ROOT)):
        raise SystemExit(f'{path} has uncommitted changes: commit or restore it '
                         'before a sweep, so a hard stop cannot leave variant flags behind')
    current = parse_flags(open(path, encoding='utf-8', newline='').read())
    print(f'{path}: current flags {" ".join(current) or "(default)"}')
    print(f'  restore with: git checkout -- {os.path.relpath(path, ROOT)}')
    flag_sets = variants(current)
    print(f'{len(flag_sets)} flag set(s)')
    rows = sweep(path, wanted, flag_sets, args.full, args.timeout, args.dry_run)
    if args.dry_run:
        return 0
    baseline = tuple(current)
    def key(row):
        flags, found = row
        if isinstance(found, str):
            return (10**6, 0)
        return (max(score for status, score in found.values()), 0 if flags == baseline else 1)
    print('\nflags' + ' ' * 39 + 'result')
    for flags, found in sorted(rows, key=key):
        mark = ' *' if flags == baseline else ''
        if isinstance(found, str):
            print(f'{" ".join(flags):44}{found}')
        else:
            print(f'{" ".join(flags):44}' + '  '.join(
                f'{va:08x} {status} {score}' for va, (status, score) in found.items()) + mark)
    return 0


if __name__ == '__main__':
    sys.exit(main())
