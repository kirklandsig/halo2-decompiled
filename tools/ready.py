"""Lists the game functions ready to decompile: not decompiled yet (no source in
src/), and every function they call is decompiled already (matched or not: a
real callee, unlike a stub, gets the register convention LTCG gives it in
retail, so the caller can match), belongs to a library, or is part of the same
mutual recursion. Smallest first, one line per function: va, size, name (a
library signature's, else "-"), calls.

    python tools/ready.py [N] [--claims FILE]

--claims FILE is a saved copy of the Active claims table on issue #9
(markdown). A function is left out when its address sits in one of those
ranges. The Finished section is not claimed. A parenthetical "(except ...)"
is a hole, claimed only if another row lists it. Without --claims, the list
is unchanged. The tool does not read GitHub.
"""
import argparse
import bisect
import re
import sys

from inventory import read_rows
from xbe import FUNCTIONS_CSV

_RANGE = re.compile(r'`(0x[0-9a-fA-F]+)`\s*[–—-]\s*`(0x[0-9a-fA-F]+)`')
_SPAN_RANGE = re.compile(r'`(0x[0-9a-fA-F]+)\s*[–—-]\s*(0x[0-9a-fA-F]+)`')  # `0xa–0xb`
_ADDR = re.compile(r'`(0x[0-9a-fA-F]+)`')
_CODE = re.compile(r'`([^`]*)`')
_HEADING = re.compile(r'^ {0,3}(#{1,6})(?:\s|$)')
_EXCEPT = re.compile(r'\(([^)]*\bexcept\b[^)]*)\)', re.IGNORECASE)


def components(graph):
    """Strongly connected components (Tarjan), iteratively."""
    index, low, on, stack, out, counter = {}, {}, set(), [], [], [0]
    for root in graph:
        if root in index:
            continue
        work = [(root, iter(graph.get(root, ())))]
        index[root] = low[root] = counter[0]; counter[0] += 1
        stack.append(root); on.add(root)
        while work:
            node, edges = work[-1]
            for nxt in edges:
                if nxt not in graph:
                    continue
                if nxt not in index:
                    index[nxt] = low[nxt] = counter[0]; counter[0] += 1
                    stack.append(nxt); on.add(nxt)
                    work.append((nxt, iter(graph.get(nxt, ()))))
                    break
                if nxt in on:
                    low[node] = min(low[node], index[nxt])
            else:
                work.pop()
                if work:
                    low[work[-1][0]] = min(low[work[-1][0]], low[node])
                if low[node] == index[node]:
                    group = set()
                    while True:
                        n = stack.pop(); on.discard(n); group.add(n)
                        if n == node:
                            break
                    out.append(group)
    return out


def _span_list(text, warn=None):
    """Inclusive spans from `0xa`–`0xb`, `0xa–0xb` and lone `0xa` code spans.

    warn, if given, is called with each other code span that mentions 0x, so a
    range in a form this does not read is reported rather than dropped."""
    spans = []
    rest = text
    for pattern in (_RANGE, _SPAN_RANGE):
        for m in pattern.finditer(rest):
            a, b = int(m.group(1), 16), int(m.group(2), 16)
            if a > b:
                a, b = b, a
            spans.append((a, b))
        rest = pattern.sub(' ', rest)
    for m in _ADDR.finditer(rest):
        v = int(m.group(1), 16)
        spans.append((v, v))
    if warn:
        for m in _CODE.finditer(_ADDR.sub(' ', rest)):
            if '0x' in m.group(1).lower():
                warn(f'not read as an address or range: `{m.group(1)}`')
    return spans


def _merge(spans):
    spans = sorted(s for s in spans if s[0] <= s[1])
    out = []
    for a, b in spans:
        if out and a <= out[-1][1] + 1:
            out[-1] = (out[-1][0], max(out[-1][1], b))
        else:
            out.append((a, b))
    return out


def _subtract(claimed, holes):
    parts = list(claimed)
    for hlo, hhi in holes:
        nxt = []
        for a, b in parts:
            if hhi < a or hlo > b:
                nxt.append((a, b))
                continue
            if a < hlo:
                nxt.append((a, hlo - 1))
            if b > hhi:
                nxt.append((hhi + 1, b))
        parts = nxt
    return parts


def _active_section(lines):
    """The lines of the Active claims section: after its heading, up to the
    next heading of the same or a higher level (so a sub-heading inside it
    does not end it, and `###` headings work as well as `##`). The last such
    heading wins, so a saved issue title ("# Active claims: ...") above the
    section does not swallow the Finished table. Without that heading, the
    lines up to the first heading that follows a table row."""
    start, level, found = 0, 6, False
    for i, line in enumerate(lines):
        m = _HEADING.match(line)
        if m and 'active claims' in line.lower():
            start, level, found = i + 1, len(m.group(1)), True
    seen_row = False
    for i in range(start, len(lines)):
        m = _HEADING.match(lines[i])
        if m and len(m.group(1)) <= level and (found or seen_row):
            return lines[start:i]
        if lines[i].lstrip().startswith('|'):
            seen_row = True
    return lines[start:]


def parse_claims(text, warn=None):
    """Merged inclusive ranges from an issue #9 Active claims table.

    Stops at the next heading of the same or a higher level, so the Finished
    section is ignored. Returns [] when the table has no addresses. warn, if
    given, is called for each code span that mentions 0x but is not read as
    an address or range."""
    text = text.replace('\r\n', '\n').replace('\r', '\n')
    spans = []
    for line in _active_section(text.split('\n')):
        raw = line.strip()
        if not raw.startswith('|'):
            continue
        cells = [c.strip() for c in raw.strip('|').split('|')]
        if len(cells) < 2:
            continue
        head = cells[1].lower().replace(' ', '')
        if head in ('retailrange', '---') or set(head) <= set('-:'):
            continue
        cell = cells[1]
        holes = []
        for m in _EXCEPT.finditer(cell):
            holes += _span_list(m.group(1), warn)
        # A hole applies only to this row. Another row may claim the same
        # addresses (lane I's "except the UI screens", which the UI lane lists).
        spans += _subtract(_span_list(_EXCEPT.sub(' ', cell), warn), holes)
    return _merge(spans)


def covers(claims, va):
    """True when va lies in one of the merged inclusive ranges."""
    i = bisect.bisect_right(claims, (va, 1 << 64)) - 1
    return i >= 0 and claims[i][0] <= va <= claims[i][1]


def without_claims(rows, claims):
    return [r for r in rows if not covers(claims, int(r['va'], 16))]


def _by_size(r):
    return int(r['size']), r['va']


def ready(rows):
    def calls(va):
        text = rows[va]['calls']
        return {int(c, 16) for c in text.split()} if text else set()

    def done(va):  # decompiled: matched, or has source the checker re-tests
        return rows[va]['status'] == 'matched' or bool(rows[va].get('source'))

    game = {va for va, r in rows.items() if r['owner'] == 'game'}
    graph = {va: calls(va) & game for va in game}
    result = []
    for group in components(graph):
        if all(done(va) for va in group):
            continue
        outside = set().union(*(calls(va) for va in group)) - group
        if all(c not in rows or rows[c]['owner'] != 'game' or done(c) for c in outside):
            result += [rows[va] for va in group if not done(va)]
    return sorted(result, key=_by_size)


def line(r):
    return ' '.join((r['va'], r['size'], r['name'] or '-', r['calls'] or '-'))


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('count', nargs='?', type=int, default=20)
    ap.add_argument('--claims', metavar='FILE',
                    help='markdown copy of the issue #9 Active claims table')
    args = ap.parse_args()
    rows = read_rows(FUNCTIONS_CSV)
    found = ready(rows)
    if args.claims:
        with open(args.claims, encoding='utf-8') as fh:
            warnings = []
            claims = parse_claims(fh.read(), warn=warnings.append)
        if warnings:
            ap.error('cannot safely filter incomplete claims: ' + '; '.join(warnings))
        if not claims:
            ap.error('no address claims found (expected the Active claims table from issue #9)')
        kept = without_claims(found, claims)
        print(f'hiding {len(found) - len(kept)} of {len(found)} ready functions '
              f'in {len(claims)} claimed ranges', file=sys.stderr)
        found = kept
    for r in found[:args.count]:
        print(line(r))


if __name__ == '__main__':
    main()
