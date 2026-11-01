"""Static checks on the phase B map (no tree changes): proposed names valid,
unique, new; no chained renames; file targets free; string-literal hits."""
import collections, csv, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P

S = sys.argv[1]
rows = list(csv.DictReader(open(os.path.join(S, 'phase-b-rename-map.tsv'), encoding='utf-8'), delimiter='\t'))
ren = [r for r in rows if r['class'] == 'RENAME']
idents = {r['current_name']: r['proposed_name'] for r in ren if r['kind'] != 'file'}
files = {r['current_name']: r['proposed_name'] for r in ren if r['kind'] == 'file'}
texts = P.source_texts(P.ROOT)
present = set()
literal = collections.Counter()
for path, text in texts.items():
    code, _ = P.strip_code(text)
    present.update(P.IDENT.findall(code))
    for m in re.finditer(r'"([^"\n]*)"', text):
        for t in P.IDENT.findall(m.group(1)):
            if t in idents and not re.match(r'\s*#\s*include', text[text.rfind('\n', 0, m.start()) + 1:m.start()]):
                literal[(path, t)] += 1
problems = []
tgt = collections.Counter(idents.values())
problems += [f'dup target {n}' for n, k in tgt.items() if k > 1]
problems += [f'exists {c}->{n}' for c, n in idents.items() if n in present and n not in idents]
problems += [f'chain {c}->{n}' for c, n in idents.items() if n in idents]
problems += [f'invalid {c}->{n}' for c, n in idents.items() if not re.fullmatch(r'[A-Za-z_]\w*', n) or n == c]
problems += [f'file target exists {c}->{n}' for c, n in files.items() if n in texts]
problems += [f'file dup target {n}' for n, k in collections.Counter(files.values()).items() if k > 1]
suffixed = sorted(n for n in idents.values() if re.search(r'_\d+$', n) and not re.search(r'_[0-9a-f]{4,}$', n))
print('rename identifiers', len(idents), 'files', len(files), 'problems', len(problems))
for p in problems[:30]:
    print(' ', p)
print('string-literal occurrences of renamed names (left alone by apply):', sum(literal.values()))
for (p, t), k in sorted(literal.items())[:20]:
    print('  ', p, t, k)
print('suffixed targets:', len(suffixed))
