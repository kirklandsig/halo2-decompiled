"""Apply the phase B rename map (phase-b-rename-map.tsv) to src/ and include/.

Identifier renames are whole-identifier, single-pass and case-sensitive, in code
and comments; string and character literals are left alone and reported (they
are data, and the #include lines among them are handled by the file pass).
File renames use `git mv`, then rewrite #include lines, the upper-case file
banner in each renamed file's first comment, and the source column of
config/functions.csv.

    python build/apply_renames.py --map <tsv> [--phase identifiers|files|text|all]
                                  [--apply] [--verify] [--allow-dirty]
                                  [--exclude name,name] [--include-review]

The text phase applies the same renames to docs, tests and spike/ (whole word,
literals included) and moves files there named after a renamed source file.
The map can be run on any branch: map files the branch lacks are skipped, files
already moved are left, and names the branch never uses are reported.

Without --apply it only reports what it would change (a dry run). --verify
runs `python tools/check.py` afterwards and fails if any retail function that
was matched (or near) in config/functions.csv before the run lost that status.
Rows whose reason marks them "reviewer may downgrade" are applied unless
--exclude names them; --include-review is accepted for symmetry and is the default.
"""
import argparse
import collections
import csv
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_DIRS = ('src', 'include')
SOURCE_EXT = ('.cpp', '.h', '.c', '.inl')
IDENT = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')
CODE_TOKEN = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')


def git(*args, check=True):
    return subprocess.run(['git', '-C', ROOT, *args], capture_output=True, text=True, check=check).stdout


def read_map(path, exclude):
    idents, files = {}, {}
    with open(path, encoding='utf-8') as f:
        for row in csv.DictReader(f, delimiter='\t'):
            if row['class'] != 'RENAME' or row['current_name'] in exclude:
                continue
            cur, new = row['current_name'], row['proposed_name']
            if not new or new == cur:
                raise SystemExit(f'bad row: {cur!r} -> {new!r}')
            if row['kind'] == 'file':
                files[cur] = new
            else:
                if not IDENT.match(cur) or not IDENT.match(new):
                    raise SystemExit(f'not an identifier: {cur!r} -> {new!r}')
                idents[cur] = new
    return idents, files


def source_files():
    out = []
    for d in SOURCE_DIRS:
        for dirpath, _, names in os.walk(os.path.join(ROOT, d)):
            for n in sorted(names):
                if n.endswith(SOURCE_EXT):
                    out.append(os.path.relpath(os.path.join(dirpath, n), ROOT).replace('\\', '/'))
    return sorted(out)


def segments(text):
    """[(kind, text)] with kind 'code', 'comment' or 'literal', covering text exactly."""
    out = []
    i, n, start = 0, len(text), 0

    def flush(upto):
        if upto > start:
            out.append(('code', text[start:upto]))

    while i < n:
        if text.startswith('//', i) or text.startswith('/*', i):
            flush(i)
            if text[i + 1] == '/':
                j = text.find('\n', i)
                j = n if j < 0 else j
            else:
                j = text.find('*/', i + 2)
                j = n if j < 0 else j + 2
            out.append(('comment', text[i:j]))
            i = start = j
        elif text[i] in '"\'':
            # a digit separator or an apostrophe inside a number never occurs in this C++03 code
            flush(i)
            q, j = text[i], i + 1
            while j < n and text[j] != q and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            j = min(j + 1, n)
            out.append(('literal', text[i:j]))
            i = start = j
        else:
            i += 1
    flush(n)
    return out


def check_collisions(idents, files_text):
    """Every new name must be new: not already an identifier anywhere in src/include
    (unless it is itself being renamed away) and not the target of two renames."""
    present = set()
    for text in files_text.values():
        for kind, seg in segments(text):
            if kind == 'code':
                present.update(CODE_TOKEN.findall(seg))
    targets = collections.Counter(idents.values())
    problems = [f'{new} is the target of {k} renames' for new, k in targets.items() if k > 1]
    # a target already present is a collision only while the old name is still used
    # (on a branch that has the renames already, the old name is simply gone)
    problems += [f'{cur} -> {new}: {new} already exists in src/include' for cur, new in idents.items()
                 if new in present and new not in idents and cur in present]
    chained = [f'{cur} -> {new} -> {idents[new]}' for cur, new in idents.items() if new in idents]
    problems += [f'chained rename {c}' for c in chained]
    return problems


def rename_identifiers(idents, files_text, comments=True):
    if not idents:
        return {}, collections.Counter(), []
    pattern = re.compile(r'(?<![A-Za-z0-9_])(' + '|'.join(sorted(map(re.escape, idents), key=len, reverse=True)) +
                         r')(?![A-Za-z0-9_])')
    changed, counts, literal_hits = {}, collections.Counter(), []

    def sub(m):
        counts[m.group(1)] += 1
        return idents[m.group(1)]

    for path, text in files_text.items():
        parts = []
        for kind, seg in segments(text):
            if kind == 'code' or (kind == 'comment' and comments):
                seg = pattern.sub(sub, seg)
            elif kind == 'literal' and pattern.search(seg) and not text_is_include(text, seg):
                literal_hits.append((path, seg))
            parts.append(seg)
        new = ''.join(parts)
        if new != text:
            changed[path] = new
    return changed, counts, literal_hits


def text_is_include(text, seg):
    return re.search(r'#\s*include\s*' + re.escape(seg), text) is not None


def _word(names):
    return re.compile(r'(?<![A-Za-z0-9_])(' + '|'.join(sorted(map(re.escape, names), key=len, reverse=True)) +
                      r')(?![A-Za-z0-9_])')


def file_reference_rewriters(idents, path_map):
    """(refs, stems): functions that rewrite mentions of moved files in prose.
    refs handles paths and file names (also a name whose stem the identifier pass
    already rewrote, as in "MACRO_0A8D68.cpp"); stems handles bare stems that are
    not identifiers themselves."""
    stems = {}
    for old, new in path_map.items():
        stems.setdefault(os.path.splitext(os.path.basename(old))[0], os.path.splitext(os.path.basename(new))[0])
    names = {os.path.basename(o): os.path.basename(n) for o, n in path_map.items()}
    for stem, new_stem in list(stems.items()):
        names.setdefault(stem + '.md', new_stem + '.md')
        if stem in idents:
            for ext in ('.cpp', '.h', '.md'):
                names.setdefault(idents[stem] + ext, new_stem + ext)
    paths_rx = re.compile(r'(?<![A-Za-z0-9_/.])(' + '|'.join(sorted(map(re.escape, path_map), key=len, reverse=True)) +
                          r')(?![A-Za-z0-9_])')
    names_rx = _word(names)
    bare = {s: n for s, n in stems.items() if s not in idents}
    bare_rx = _word(bare) if bare else None
    counts = collections.Counter()

    def refs(text):
        text = paths_rx.sub(lambda m: (counts.update(['path']), path_map[m.group(1)])[1], text)
        return names_rx.sub(lambda m: (counts.update(['file name']), names[m.group(1)])[1], text)

    def bare_stems(text):
        return bare_rx.sub(lambda m: (counts.update(['stem']), bare[m.group(1)])[1], text) if bare_rx else text
    return refs, bare_stems, counts


def rename_files(files, apply, idents=None):
    """git mv each file; rewrite #include lines, file banners, mentions of the moved
    files in comments, and functions.csv sources."""
    refs, bare_stems, ref_counts = file_reference_rewriters(idents or {}, files)
    moves = {}
    for old, new in files.items():
        old_there, new_there = (os.path.exists(os.path.join(ROOT, p)) for p in (old, new))
        if old_there and new_there:
            raise SystemExit(f'{old} -> {new}: both exist')
        if old_there:
            moves[old] = new
        elif new_there:
            print(f'  already moved: {old} -> {new}')
        else:
            print(f'  not on this branch, skipped: {old}')
    # #include lines and the csv are rewritten for every map row, so a branch whose
    # files were moved already still gets its new includes fixed
    all_moves = dict(files)
    header_moves = {os.path.basename(o): os.path.basename(n) for o, n in all_moves.items() if o.endswith('.h')}
    include = re.compile(r'(#\s*include\s*")([^"]+)(")')
    edits = {}
    for path in source_files():
        text = open(os.path.join(ROOT, path), encoding='utf-8').read()

        def inc(m):
            target = m.group(2)
            base = os.path.basename(target)
            if base in header_moves:
                return m.group(1) + target[:len(target) - len(base)] + header_moves[base] + m.group(3)
            return m.group(0)
        new = include.sub(inc, text)
        # comments naming moved files; Halo CE's own file names are left alone
        new = ''.join(bare_stems(refs(seg)) if kind == 'comment' and 'Halo CE' not in seg else seg
                      for kind, seg in segments(new))
        if path in moves:  # the banner: "/* DAMAGE.CPP: ..." names the file in upper case
            old_up = os.path.basename(path).upper()
            new = new.replace(old_up, os.path.basename(moves[path]).upper(), 1)
        if new != text:
            edits[path] = new
    csv_path = os.path.join(ROOT, 'config', 'functions.csv')
    csv_text = open(csv_path, encoding='utf-8', newline='').read()
    csv_new = csv_text
    for old, new in all_moves.items():
        csv_new = re.sub(r'(?<=,)' + re.escape(old) + r'(?=,)', new, csv_new)
    print(f'files: {len(moves)} moves, {len(edits)} sources with #include, banner or comment edits '
          f'(comment mentions: {dict(ref_counts)}), '
          f'functions.csv {"changed" if csv_new != csv_text else "unchanged"}')
    if not apply:
        for old, new in sorted(moves.items()):
            print(f'  git mv {old} {new}')
        return
    for path, text in edits.items():
        with open(os.path.join(ROOT, path), 'w', encoding='utf-8', newline='') as f:
            f.write(text)
    for old, new in sorted(moves.items()):
        git('mv', old, new)
    if csv_new != csv_text:
        with open(csv_path, 'w', encoding='utf-8', newline='') as f:
            f.write(csv_new)


# PROVENANCE.md is left out: it quotes kept names (and the light_fixture string) on purpose
TEXT_GLOBS = ('README.md', 'CONTRIBUTING.md', 'docs/**/*.md', 'tests/**/*.py', 'spike/*')


def rename_text(idents, files, globs, apply):
    """Docs, tests and spike/: whole-word renames everywhere in the text (literals
    included), then renamed file paths and names, then bare file stems that are
    not identifiers. A file there whose stem is a renamed source stem (a doc named
    after its source file, a spike copy) is moved to the new stem as well."""
    import glob as globmod
    stems = {}
    for old, new in files.items():
        stems[os.path.splitext(os.path.basename(old))[0]] = os.path.splitext(os.path.basename(new))[0]
    paths = sorted({os.path.relpath(p, ROOT).replace('\\', '/') for g in globs
                    for p in globmod.glob(os.path.join(ROOT, g), recursive=True) if os.path.isfile(p)})
    # companion files: named after a renamed source stem, or that stem plus a suffix
    moves = {}
    for path in paths:
        stem, ext = os.path.splitext(os.path.basename(path))
        if path in files:
            continue
        hit = stem if stem in stems else max((s for s in stems if stem.startswith(s + '_')), key=len, default=None)
        if hit:
            moves[path] = os.path.dirname(path) + '/' + stems[hit] + stem[len(hit):] + ext
    # file references: full paths first, then names with an extension, then bare stems
    path_map = dict(files)
    path_map.update(moves)
    refs, bare_stems, counts = file_reference_rewriters(idents, path_map)
    ident_rx = _word(idents) if idents else None
    edits = {}
    for path in paths:
        text = open(os.path.join(ROOT, path), encoding='utf-8', errors='surrogateescape').read()
        new = refs(text)
        if ident_rx:
            new = ident_rx.sub(lambda m: (counts.update(['identifier']), idents[m.group(1)])[1], new)
        new = bare_stems(new)
        if new != text:
            edits[path] = new
    print(f'text: {len(edits)} files edited ({dict(counts)}), {len(moves)} companion files moved')
    for old, new in sorted(moves.items()):
        print(f'  git mv {old} {new}')
    if not apply:
        for p in sorted(edits):
            print(f'  edit {p}')
        return
    for path, text in edits.items():
        with open(os.path.join(ROOT, path), 'w', encoding='utf-8', errors='surrogateescape', newline='') as f:
            f.write(text)
    for old, new in sorted(moves.items()):
        if os.path.exists(os.path.join(ROOT, new)):
            raise SystemExit(f'{new} already exists')
        git('mv', old, new)


def statuses():
    with open(os.path.join(ROOT, 'config', 'functions.csv'), encoding='utf-8') as f:
        return {r['va']: r['status'] for r in csv.DictReader(f)}


RANK = {'matched': 2, 'near': 1}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--map', required=True)
    ap.add_argument('--phase', choices=['identifiers', 'files', 'text', 'all'], default='all')
    ap.add_argument('--text-globs', default=','.join(TEXT_GLOBS),
                    help='comma-separated globs for the text phase (docs, tests, spike/)')
    ap.add_argument('--apply', action='store_true')
    ap.add_argument('--verify', action='store_true')
    ap.add_argument('--allow-dirty', action='store_true')
    ap.add_argument('--no-comments', action='store_true', help='leave identifiers in comments alone')
    ap.add_argument('--exclude', default='', help='comma-separated current names to skip')
    ap.add_argument('--include-review', action='store_true')
    args = ap.parse_args()

    if args.apply and not args.allow_dirty and git('status', '--porcelain', '--', 'src', 'include', 'config').strip():
        raise SystemExit('src/, include/ or config/ has uncommitted changes: commit or stash them first')
    idents, files = read_map(args.map, {x for x in args.exclude.split(',') if x})
    before = statuses()

    if args.phase in ('identifiers', 'all'):
        files_text = {p: open(os.path.join(ROOT, p), encoding='utf-8').read() for p in source_files()}
        problems = check_collisions(idents, files_text)
        if problems:
            print('\n'.join(problems))
            raise SystemExit(f'{len(problems)} collision(s): fix the map first')
        changed, counts, literal_hits = rename_identifiers(idents, files_text, comments=not args.no_comments)
        unused = sorted(set(idents) - set(counts))
        print(f'identifiers: {len(counts)} of {len(idents)} names found, {sum(counts.values())} replacements '
              f'in {len(changed)} files')
        if unused:
            print(f'  {len(unused)} map names not found (already renamed?): {", ".join(unused[:20])}')
        for path, seg in literal_hits:
            print(f'  left in a string literal (review by hand): {path}: {seg}')
        if args.apply:
            for path, text in changed.items():
                with open(os.path.join(ROOT, path), 'w', encoding='utf-8', newline='') as f:
                    f.write(text)

    if args.phase in ('files', 'all'):
        rename_files(files, args.apply, idents)

    if args.phase in ('text', 'all'):
        rename_text(idents, files, [g for g in args.text_globs.split(',') if g], args.apply)

    if args.verify:
        if not args.apply:
            raise SystemExit('--verify needs --apply')
        r = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'check.py')], cwd=ROOT)
        after = statuses()
        lost = [(va, before[va], after.get(va, '')) for va in before
                if RANK.get(before[va], 0) > RANK.get(after.get(va, ''), 0)]
        gained = sum(1 for va in after if RANK.get(after[va], 0) > RANK.get(before.get(va, ''), 0))
        print(f'check exit {r.returncode}; lost {len(lost)}, gained {gained}')
        for va, b, a in lost[:50]:
            print(f'  LOST {va}: {b} -> {a or "?"}')
        if r.returncode or lost:
            raise SystemExit(1)


if __name__ == '__main__':
    main()
