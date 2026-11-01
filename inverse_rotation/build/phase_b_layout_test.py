"""Check the layout model against the sources' own offset evidence: offset
comments (/* +0x.. */ before a field) and offsetof(...) == 0x.. checks."""
import os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P
from phase_b_layout import Model

texts = P.source_texts(P.ROOT)
m = Model(texts)
good = bad = unk = 0
for path, text in texts.items():
    for mm in re.finditer(r'offsetof\(\s*(\w+)\s*,\s*(\w+)\s*\)\s*==\s*(0x[0-9a-fA-F]+|\d+)', text):
        rec = m.records.get((path, mm.group(1))) or m.by_name.get(mm.group(1))
        lay = rec and m.layout(rec)
        got = lay['fields'].get(mm.group(2)) if lay else None
        want = int(mm.group(3), 0)
        if got is None:
            unk += 1
        elif got == want:
            good += 1
        else:
            bad += 1
            print('BAD', path, mm.group(1), mm.group(2), hex(got), hex(want))
    # offset comments inside records
    for (p, name), rec in m.records.items():
        if p != path:
            continue
        lay = m.layout(rec)
        if not lay:
            continue
        for fm in re.finditer(r'/\*\s*\+?(0x[0-9a-fA-F]+)\s*\*/\s*[^;/]*?\b(\w+)\s*(?:\[[^\]]*\])*\s*;', text[rec[5]:rec[5] + len(rec[1]) + 200]):
            got = lay['fields'].get(fm.group(2))
            if got is None:
                continue
            if got == int(fm.group(1), 16):
                good += 1
            else:
                bad += 1
                print('BAD-comment', path, name, fm.group(2), hex(got), fm.group(1))
# unknownNNN fields name their own offset
for (p, name), rec in m.records.items():
    lay = m.layout(rec)
    if not lay:
        continue
    for f, off in lay['fields'].items():
        u = re.fullmatch(r'unknown_?([0-9a-f]+)', f)
        if u:
            if int(u.group(1), 16) == off:
                good += 1
            else:
                bad += 1
total = len(m.records)
laid = sum(1 for r in m.records.values() if m.layout(r))
print('records', total, 'laid out', laid, 'checks good', good, 'bad', bad, 'unknown', unk)
