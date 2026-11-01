"""Phase B scan: find identifiers and file names in src/ and include/ that
match non-permitted sources (halo-symbol-atlas / 2003 maps, the contributor's
2003-map notes, the 2007 debug build), classify them and write a rename map.

Analysis only: reads src/, include/, config/, docs/ and the taint sources;
writes to the scratchpad given by --out.

    python build/phase_b_scan.py --out <dir>
"""
import argparse
import collections
import csv
import glob
import json
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))
from linkmap import plain_name  # noqa: E402
import build as project_build  # noqa: E402

MAIN = os.path.abspath(os.path.join(ROOT, '..', '..', '..')) if '.claude' in ROOT else ROOT
SCRATCH_DEFAULT = None
# Phase A emptied the name/object columns of config/functions.csv and removed the
# map citations from docs and file headers; the taint sources are read from this
# commit, which still has them.
TAINT_COMMIT = os.environ.get('PHASEB_TAINT_COMMIT', 'c25fc36')


def git_show(path, commit=TAINT_COMMIT):
    r = subprocess.run(['git', '-C', ROOT, 'show', f'{commit}:{path}'], capture_output=True,
                       encoding='utf-8', errors='replace')
    return r.stdout if r.returncode == 0 else None

CONTRIBUTOR_DOCS = ['actor_looking', 'crates', 'cseries', 'device_controls', 'device_light_fixtures',
                    'equipment', 'garbage', 'looping_sound_manager', 'path', 'real_math_rectangles',
                    'recorded_animation_playback', 'recorded_animation_playback_v1', 'sound_scenery']

IDENT = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')
CPP_WORDS = set('''alignas alignof and asm auto bool break case catch char class const const_cast continue
decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if
inline int long mutable namespace new noexcept not nullptr operator or private protected public register
reinterpret_cast return short signed sizeof static static_assert static_cast struct switch template this throw
true try typedef typeid typename union unsigned using virtual void volatile wchar_t while define include ifdef
ifndef endif pragma undef elif defined error warning line __cdecl __stdcall __fastcall __thiscall __declspec
__forceinline __inline __int64 __int32 __int16 __int8 __asm naked noinline NULL'''.split())
# names any C/C++ programmer or the public SDKs supply
PUBLIC_NAMES = set('''memcpy memmove memset memcmp strlen strcpy strncpy strcmp strncmp strcat strncat strchr strrchr
strstr sprintf vsprintf _snprintf _vsnprintf snprintf vsnprintf printf sscanf atoi atol atof strtol strtoul strtod
malloc free realloc calloc qsort bsearch abs fabs labs sqrt sqrtf sin cos tan asin acos atan atan2 floor ceil fmod
pow exp log log10 floorf ceilf sinf cosf atan2f fabsf powf expf logf tolower toupper isalpha isdigit isspace
isupper islower isalnum va_start va_end va_arg va_list size_t wchar_t _ftol _chkstk _alloca_probe alloca_probe
chkstk rand srand time clock _itoa itoa _stricmp stricmp _strnicmp strnicmp wcslen wcscpy wcsncpy wcscmp
swprintf _wcsicmp wcsicmp _purecall atexit _ftol2 _allmul _alldiv _allrem _aulldiv _aullrem _allshl _allshr
_aullshr'''.split())
TYPE_PREFIXES = ('s_', 'c_', 'e_', 't_', 'u_', 'g_', 'k_', 'm_', 'p_')


def scratch_path(out, name):
    return os.path.join(out, name)


# ---------------------------------------------------------------- taint sources

DECORATED_TYPE = re.compile(r'(?:[TUV]|W4)([A-Za-z_][A-Za-z0-9_]*)@@')


def atlas_sources(scratch):
    """{base name: info} from config/functions.csv names, the atlas retail map
    and the 2003 maps; also class/namespace qualifiers and decorated types."""
    names = collections.defaultdict(lambda: {'sources': set(), 'vas': set(), 'library': False, 'game': False, 'objects': set()})
    types = collections.defaultdict(set)
    objects = collections.defaultdict(set)  # object base -> {source}

    def add(decorated, lib, source, va=None):
        if not decorated:
            return
        game = is_game_lib(lib)
        plain = plain_name(decorated)
        if '`' in plain or plain.startswith('?'):
            plain = plain.split('::`')[0]
        parts = [p for p in plain.split('::') if p]
        if not parts:
            return
        base = parts[-1].lstrip('~')
        for p in parts:
            p = p.lstrip('~')
            if not IDENT.fullmatch(p):
                continue
            info = names[p]
            info['sources'].add(source + ('' if p == base else ':qualifier'))
            if game:
                info['game'] = True
            else:
                info['library'] = True
            if va is not None and p == base:
                info['vas'].add(va)
            if lib:
                info['objects'].add(obj_base(lib))
        if decorated.startswith('?'):
            tail = decorated.split('@@', 1)[1] if '@@' in decorated else ''
            for t in DECORATED_TYPE.findall(tail):
                types[t].add(source + (':game' if game else ':library'))
        if lib:
            ob = obj_base(lib)
            if ob:
                objects[ob].add(source + (':game' if game else ':library'))

    import io
    with io.StringIO(git_show('config/functions.csv'), newline='') as f:
        for row in csv.DictReader(f):
            game = row['owner'] == 'game' or row['owner'].startswith('game')
            lib = row['object'] if game else ('lib:' + row['object'] if row['object'] else '')
            if row['name']:
                add(row['name'], lib or ('' if game else 'lib:'), 'atlas-csv', int(row['va'], 16))
            elif row['object']:
                ob = obj_base(row['object'])
                if ob:
                    objects[ob].add('atlas-csv' + (':game' if game else ':library'))
    for path in [os.path.join(MAIN, 'orig', 'atlas_retail.jsonl')] + glob.glob(os.path.join(scratch, 'atlas', '*.jsonl')):
        if not os.path.exists(path):
            continue
        source = 'atlas-retail' if path.endswith('atlas_retail.jsonl') or os.path.basename(path).startswith('0321') else 'map2003'
        with open(path, encoding='utf-8') as f:
            for line in f:
                d = json.loads(line)
                if 'name' not in d:
                    continue
                va = int(d['off'], 16) if source == 'atlas-retail' else None
                add(d['name'], d.get('lib', ''), source, va)
    return names, types, objects


LIBRARY_PREFIXES = ('xapilib', 'xboxkrnl', 'd3d8', 'd3dx8', 'dsound', 'xvoice', 'xnet', 'xonline', 'libc', 'libcmt',
                    'libcpmt', 'binkxbox', 'xgraphics', 'havok', 'hk', 'xmv', 'wmv', 'xhv', 'lib:', 'msvcrt')


def is_game_lib(lib):
    if not lib:
        return True
    lib = lib.strip()
    if lib.startswith('i '):
        lib = lib[2:]
    if lib.startswith('lib:'):
        return False
    if ':' in lib:
        prefix = lib.split(':', 1)[0].lower()
        return prefix.startswith('blamlib') or prefix.startswith('halo')
    return True


def obj_base(lib):
    lib = lib.strip()
    if lib.startswith('i '):
        lib = lib[2:]
    lib = lib.split(':')[-1].strip()
    if lib.startswith('i '):
        lib = lib[2:]
    base = os.path.splitext(os.path.basename(lib.replace('\\', '/')))[0]
    return base.lower() if base else ''


def contributor_names(texts_by_path):
    """Identifiers in the contributor's recovery notes and in the header comments
    of files that cite the 2003 maps."""
    names = collections.defaultdict(set)
    for d in CONTRIBUTOR_DOCS:
        text = git_show(f'docs/{d}.md')
        if text is None:
            continue
        for chunk in re.findall(r'`([^`]+)`', text) + re.findall(r'^\|([^|\n]+)\|', text, re.M):
            for t in IDENT.findall(chunk):
                if '_' in t.strip('_') or len(t) > 2:
                    names[t].add(f'contrib-doc:{d}.md')
    for path, text in texts_by_path.items():
        if 'May 2003' not in text:
            continue
        m = re.match(r'(?:\s*//[^\n]*\n)*\s*/\*(.*?)\*/', text, re.S)
        if m:
            for t in IDENT.findall(m.group(1)):
                if '_' in t.strip('_'):
                    names[t].add(f'contrib-header:{path}')
    return names


def debug_tokens(scratch, retail_tokens):
    """Identifiers in the debug build's strings (assertions, expressions), minus
    any the retail XBE's own strings carry, and its source file names."""
    data = open(os.path.join(MAIN, 'orig', 'debug', 'halo2debug.xbe'), 'rb').read()
    strings = [m.group().decode('ascii') for m in re.finditer(rb'[\x20-\x7e]{5,}', data)]
    tokens = collections.Counter()
    files = set()
    for s in strings:
        for p in re.findall(r'[A-Za-z0-9_\\/.]+\.(?:cpp|c|h|inl)\b', s):
            files.add(os.path.splitext(os.path.basename(p.replace('\\', '/')))[0].lower())
        for t in IDENT.findall(s):
            tokens[t] += 1
    debug = {t: n for t, n in tokens.items() if t not in retail_tokens}
    with open(os.path.join(scratch, 'debug15', 'debug15-files.tsv'), encoding='utf-8') as f:
        for row in csv.DictReader(f, delimiter='\t'):
            files.add(os.path.splitext(os.path.basename(row['source_file'].replace('\\', '/')))[0].lower())
    return debug, files, strings


def retail_string_tokens():
    data = open(os.path.join(MAIN, 'orig', 'default.xbe'), 'rb').read()
    tokens = set()
    for m in re.finditer(rb'[\x20-\x7e]{4,}', data):
        tokens.update(IDENT.findall(m.group().decode('ascii')))
    return tokens


# ---------------------------------------------------------------- source scan

def strip_code(text):
    """Comments and string/char literals blanked out (newlines kept)."""
    out = []
    i, n = 0, len(text)
    comments = []
    while i < n:
        c = text[i]
        if text.startswith('//', i):
            j = text.find('\n', i)
            j = n if j < 0 else j
            comments.append((i, text[i:j]))
            out.append(' ' * (j - i))
            i = j
        elif text.startswith('/*', i):
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            comments.append((i, text[i:j]))
            out.append(''.join(ch if ch == '\n' else ' ' for ch in text[i:j]))
            i = j
        elif c in '"\'':
            j = i + 1
            while j < n and text[j] != c and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            j = min(j + 1, n)
            # keep #include "x.h" visible separately
            out.append(c + ' ' * (j - i - 2) + (c if j - i >= 2 else ''))
            i = j
        else:
            out.append(c)
            i += 1
    return ''.join(out), comments


def brace_kind(prefix):
    p = prefix.strip()
    if re.search(r'\benum\b', p) and '(' not in p:
        return 'enum'
    if re.search(r'\b(struct|class|union)\b', p) and not p.endswith(')') and not re.search(r'\)\s*(const)?\s*$', p):
        return 'record'
    if re.search(r'\bnamespace\b', p):
        return 'ns'
    if re.search(r'\)\s*(const)?\s*$', p) or re.search(r'\b(else|do|try)\s*$', p):
        return 'code'
    if p.endswith('=') or p.endswith(','):
        return 'init'
    return 'code' if p else 'block'


TYPE_WORDS = set('bool char const double float int long short signed unsigned void volatile wchar_t __int64 __int8 __int16 __int32 struct class union enum'.split())


def _typeish(tok):
    return bool(IDENT.fullmatch(tok)) and (tok not in CPP_WORDS or tok in TYPE_WORDS)


def decl_prev(prev, prev2):
    """Whether the tokens before an identifier read as a declaration's type."""
    if _typeish(prev):
        return True
    return prev == '*' and (_typeish(prev2) or prev2 == '*')


def scan_source(path, text):
    """[(ident, line, kind)] for one source."""
    code, comments = strip_code(text)
    results = []
    stack = []  # brace kinds
    paren = 0
    last_stmt = 0
    line = 1
    i, n = 0, len(code)
    line_starts = [0]
    for k, ch in enumerate(code):
        if ch == '\n':
            line_starts.append(k + 1)
    import bisect
    tokens = list(re.finditer(r'[A-Za-z_][A-Za-z0-9_]*|0[xX][0-9a-fA-F]+|\d+\w*|->|::|[{}();,=\[\]:.*&#<>]', code))
    for idx, m in enumerate(tokens):
        t = m.group()
        if t == '{':
            stack.append(brace_kind(code[last_stmt:m.start()]))
            last_stmt = m.end()
            continue
        if t == '}':
            if stack:
                stack.pop()
            last_stmt = m.end()
            continue
        if t == ';':
            last_stmt = m.end()
            continue
        if t == '(':
            paren += 1
            continue
        if t == ')':
            paren = max(0, paren - 1)
            continue
        if not IDENT.fullmatch(t) or t in CPP_WORDS:
            continue
        prev = tokens[idx - 1].group() if idx else ''
        prev2 = tokens[idx - 2].group() if idx > 1 else ''
        nxt = tokens[idx + 1].group() if idx + 1 < len(tokens) else ''
        ctx = stack[-1] if stack else 'top'
        ln = bisect.bisect_right(line_starts, m.start())
        linetext = code[line_starts[ln - 1]:m.start()]
        if re.match(r'\s*#\s*define\s*$', linetext):
            kind = 'macro'
        elif linetext.lstrip().startswith('#'):
            kind = 'preproc'
        elif prev in ('struct', 'class', 'union', 'enum'):
            kind = 'type'
        elif prev == '*' and prev2 == '(' and nxt == ')':  # a function pointer declarator
            kind = 'field' if ctx == 'record' else ('global' if ctx in ('top', 'ns') else 'local')
        elif nxt == ':' and prev in (';', '{', '}') and ctx == 'code':
            kind = 'local'  # a label
        elif prev == 'goto':
            kind = 'local'
        elif prev in ('->', '.'):
            kind = 'method-call' if nxt == '(' else 'member-access'
        elif prev == '::':
            kind = 'method' if nxt == '(' and paren == 0 and ctx in ('top', 'ns') else ('method-call' if nxt == '(' else 'member-access')
        elif ctx == 'enum':
            kind = 'enumerator' if prev in ('{', ',') else 'ref'
        elif ctx == 'record' and paren == 0 and nxt in (';', '[', ':', ',') and prev not in ('(', ',', '=') or \
                (ctx == 'record' and paren == 0 and nxt == ';'):
            kind = 'field'
        elif ctx == 'record' and paren == 0 and nxt == '(':
            kind = 'method'
        elif nxt == '(' and paren == 0 and ctx in ('top', 'ns') and decl_prev(prev, prev2):
            kind = 'function'
        elif nxt == '(':
            kind = 'call'
        elif code[last_stmt:m.start()].lstrip().startswith('typedef') and nxt in (';', ')', '['):
            kind = 'type'
        elif paren >= 1 and nxt in (',', ')', '[') and decl_prev(prev, prev2):
            kind = 'param'
        elif ctx in ('top', 'ns') and paren == 0 and nxt in (';', '=', '[') and decl_prev(prev, prev2):
            kind = 'global'
        elif ctx == 'code' and nxt in (';', '=', '[', ',') and decl_prev(prev, prev2) and paren == 0:
            kind = 'local'
        elif t.startswith(('s_', 'c_', 'e_', 't_')) or (IDENT.fullmatch(nxt) or nxt in ('*', '&')) and nxt not in CPP_WORDS:
            kind = 'typeref'
        else:
            kind = 'ref'
        results.append((t, ln, kind))
    return results, comments


def source_texts(root):
    texts = {}
    for d in ('src', 'include'):
        for dirpath, _, files in os.walk(os.path.join(root, d)):
            for f in files:
                if f.endswith(('.cpp', '.h', '.c', '.inl')):
                    p = os.path.join(dirpath, f)
                    rel = os.path.relpath(p, root).replace('\\', '/')
                    texts[rel] = open(p, encoding='utf-8', errors='replace').read()
    return texts


def pre_debug_tokens(commit):
    """Identifiers in src/ and include/ at a commit before the debug build arrived."""
    files = subprocess.run(['git', '-C', ROOT, 'ls-tree', '-r', '--name-only', commit, 'src', 'include'],
                           capture_output=True, text=True, check=True).stdout.split()
    tokens = set()
    for f in files:
        if not f.endswith(('.cpp', '.h')):
            continue
        text = subprocess.run(['git', '-C', ROOT, 'show', f'{commit}:{f}'], capture_output=True,
                              text=True, encoding='utf-8', errors='replace').stdout
        code, _ = strip_code(text)
        tokens.update(IDENT.findall(code))
    return tokens, {os.path.splitext(os.path.basename(f))[0].lower() for f in files}


# ---------------------------------------------------------------- classification

GENERIC_EXTRA = set()  # single-word names are generic by rule; extend here if a review says so


def words(name):
    core = name
    while core.startswith('_'):
        core = core[1:]
    for p in TYPE_PREFIXES:
        if core.startswith(p) and len(core) > len(p):
            core = core[len(p):]
            break
    if core.startswith('k_'):
        core = core[2:]
    parts = [w for w in re.split(r'_+', core) if w]
    if len(parts) == 1:  # CamelCase counts its humps
        humps = re.findall(r'[A-Z]+(?![a-z])|[A-Z]?[a-z]+|\d+', parts[0])
        if len(humps) > 1 and any(c.isupper() for c in parts[0][1:]):
            return humps
    return parts


PLACEHOLDER = re.compile(r'^(function|unknown|field|g|global|sub|nullsub|loc|byte|word|dword|data|standin|type|param)_[0-9a-f]+$|^unknown[0-9a-f]+$|^(g|k)_[0-9a-f]{5,}$')

DEF_KINDS = {'function', 'method', 'field', 'type', 'macro', 'global', 'param', 'local', 'enumerator'}
KIND_ORDER = ['function', 'method', 'type', 'global', 'field', 'macro', 'enumerator', 'param', 'local']
PRE_DEBUG_COMMIT = '43c9116'  # last commit before orig/debug and the debug15 hints arrived (2026-10-03 08:29)


def gather(scratch):
    names, types, objects = atlas_sources(scratch)
    retail = retail_string_tokens()
    dbg, dfiles, _ = debug_tokens(scratch, retail)
    texts = source_texts(ROOT)
    old_files = subprocess.run(['git', '-C', ROOT, 'ls-tree', '-r', '--name-only', TAINT_COMMIT, 'src', 'include'],
                               capture_output=True, text=True, check=True).stdout.split()
    cited = {f'{TAINT_COMMIT}:{f}': git_show(f) or '' for f in old_files if f.endswith(('.cpp', '.h'))}
    cited.update(texts)  # the current tree too (phase A removed the citations from it)
    contrib = contributor_names({k: v for k, v in cited.items() if 'May 2003' in v})
    occ = collections.defaultdict(list)
    comments = {}
    markers = collections.defaultdict(list)  # base name -> [(va, path, qualified)]
    for path, text in texts.items():
        r, c = scan_source(path, text)
        comments[path] = c
        for t, ln, k in r:
            occ[t].append((path, ln, k))
        if path.startswith('src/'):
            try:
                found = project_build.scan(text, path)
            except SystemExit:
                found = []
            for m in found:
                parts = m.name.split('::')
                base = parts[-1].lstrip('~')
                markers[base].append((m.retail, path, m.name))
                for q in parts[:-1]:
                    markers['#class:' + q].append((m.retail, path, m.name))
    pre, prefiles = pre_debug_tokens(PRE_DEBUG_COMMIT)
    return dict(names=names, types=types, objects=objects, retail=retail, dbg=dbg, dfiles=dfiles, texts=texts,
                contrib=contrib, occ=occ, comments=comments, markers=markers, pre=pre, prefiles=prefiles)
