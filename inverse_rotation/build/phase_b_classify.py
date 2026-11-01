"""Phase B: classify tainted identifiers and file names, propose replacements,
write the rename map. Analysis only: reads the tree and the scratchpad's private
name sets, and writes only to a per-worktree folder in the local temp directory
(outside the repository and outside OneDrive), so private-name data never sits in
the tree and concurrent batches never share or overwrite each other's files.

    python build/phase_b_classify.py <scratchpad> [--refresh]

Outputs (in OUT = <temp>/h2-phaseb/<worktree folder name>/, printed at the end):
    phaseb-gather.pkl        gathered data, reused by extra_scan.py
    phase-b-rename-map.tsv   kind, current_name, proposed_name, location(s), source_of_taint, reason, class
    phase-b-stats.json       counts used by the report
"""
import collections
import csv
import json
import os
import pickle
import re
import sys
import tempfile
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import phase_b_scan as P  # noqa: E402
from phase_b_layout import Model  # noqa: E402

OUT = os.path.join(tempfile.gettempdir(), 'h2-phaseb', os.path.basename(os.path.normpath(P.ROOT)))
os.makedirs(OUT, exist_ok=True)


def load(scratch, refresh=False):
    cache = os.path.join(OUT, 'phaseb-gather.pkl')
    if not refresh and os.path.exists(cache):
        return pickle.load(open(cache, 'rb'))
    data = P.gather(scratch)
    for k in ('names', 'types', 'objects', 'contrib', 'occ', 'markers'):
        data[k] = {a: (dict(b) if isinstance(b, dict) else b) for a, b in data[k].items()}
    pickle.dump(data, open(cache, 'wb'))
    return data


# ---------------------------------------------------------------- taint

def taint_of(t, d):
    out = []
    info = d['names'].get(t)
    if info and info['game'] and not info['library']:
        srcs = info['sources']
        tag = []
        if any(s.startswith(('atlas-csv', 'atlas-retail')) for s in srcs):
            tag.append('atlas')
        if any(s.startswith('map2003') for s in srcs):
            tag.append('map2003')
        role = 'qualifier' if all(s.endswith(':qualifier') for s in srcs) else 'name'
        objs = ','.join(sorted(info['objects']))[:60]
        out.append(f'{"+".join(tag)}:{role}' + (f'({objs})' if objs else ''))
    tsrc = d['types'].get(t)
    if tsrc and any(s.endswith(':game') for s in tsrc) and not (info and info['library']):
        out.append('atlas:decorated-type')
    if t in d['contrib']:
        out.append('contrib-2003-notes(' + ';'.join(sorted(d['contrib'][t]))[:100] + ')')
    if t in d.get('ce', ()) and t not in d.get('pre_ce', ()):
        out.append('ce2002-pdb(new since ' + P.PRE_CE_COMMIT + ')')
    if t in d['dbg'] and t not in d['pre']:
        out.append(f'debug2007-strings({d["dbg"][t]} hits; absent before {P.PRE_DEBUG_COMMIT})')
    return out


# XDK interface methods and other public names the maps list under game objects
PUBLIC_EXTRA = {'CommunicatorStatusUpdate', 'LocalChatDataReady', 'SpeechRecognized', 'VoiceMailDataReady',
                'VoiceMailStopped', 'ITitleXHV', 'XMemAlloc', 'XMemFree', 'XMemSize',
                '_XONLINE_USER'}  # the XDK's struct tag behind XONLINE_USER (forward-declared in a stub)
PUBLIC_PATTERN = re.compile(r'^(_?X[A-Z][A-Za-z0-9_]*|D3D\w*|IDirect\w*|I[A-Z][a-z]\w*|hk[A-Z]\w*|WSA\w*|[A-Z][A-Z0-9]*(_[A-Z0-9]+)+|E_[A-Z]+|S_OK|AF_INET)$')


def library_name(t, d):
    info = d['names'].get(t)
    if info and info['library']:
        return 'listed under a library object in the maps (public SDK/library name)'
    tsrc = d['types'].get(t)
    if tsrc and all(s.endswith(':library') for s in tsrc):
        return 'a library type in the decorations'
    if t in P.PUBLIC_NAMES or t in PUBLIC_EXTRA:
        return 'public C runtime / XDK name'
    return ''


# single tokens that are not English words or obvious terms (Bungie-specific spellings)
DISTINCTIVE_SINGLES = {'csstricmp', 'csnprintf', 'csmemcmp', 'csstrnzcat', 'csstrnzcpy', 'csstrtok', 'csstrnlen',
                       'ustrnzcpy', 'RGBToColor'}

# single-token file names that are Bungie's own coinages rather than words
DISTINCTIVE_FILE_SINGLES = {'cseries'}

# compounds that read as generic idioms; they still match the source exactly, so they stay RENAME
# per the rule, but the reason column marks them for a possible downgrade on review
GENERIC_COMPOUND = re.compile(r'^(?:[a-z]+_)?[a-z]+_(index|count|indices|size|flags|type|mask|name|data|globals|bits|id)$|^(SET|TEST|CLEAR)_BIT$')

# Coordinator decision (phase B review): purely generic programming idioms are kept.
# A field, local, param or macro whose name is common words plus a generic suffix
# (index/indices/count/flags/size/type/mask/bits) or a bit macro is KEEP-GENERIC.
# Distinctive compounds stay RENAME: functions, types, classes and globals (shaped
# like exact symbol names), the *_name/*_data/*_globals/*_id idioms, and anything
# with a Halo-specific term in it.
GENERIC_IDIOM_KEEP = re.compile(r'^(?:[a-z]+_)?[a-z]+_(index|indices|count|flags|size|type|mask|bits)$|^(SET|TEST|CLEAR)_BIT$')
GENERIC_IDIOM_KINDS = {'field', 'local', 'param', 'macro'}
HALO_TERMS = {'bsp', 'biped', 'swarm', 'prop', 'simulation', 'engine', 'tag', 'datum', 'synch', 'vocalization'}


def generic_idiom_keep(t, kind):
    return (kind in GENERIC_IDIOM_KINDS and GENERIC_IDIOM_KEEP.match(t) is not None
            and not HALO_TERMS & set(t.lower().split('_')))

# behaviour-based replacements for the most-used names, chosen from what the code does
# (math value types, the salted-handle record pool); everything else gets a placeholder
CURATED = {
    'real_point3d': 'point3f', 'real_vector3d': 'vector3f', 'real_point2d': 'point2f', 'real_vector2d': 'vector2f',
    'real_vector4d': 'vector4f', 'real_quaternion': 'quaternionf', 'real_matrix4x3': 'transform4x3f',
    'real_plane3d': 'plane3f', 'real_plane2d': 'plane2f', 'real_rgb_color': 'color3f', 'real_argb_color': 'color4f',
    'real_hsv_color': 'hsv3f', 'real_euler_angles2d': 'yaw_pitch2f', 'real_rectangle2d': 'box2f',
    'real_rectangle3d': 'box3f', 'real_orientation': 'rigid_transform_scaled',
    'string_id': 'string_handle',
    's_data_array': 's_record_pool', 's_data_iterator': 's_record_pool_iterator', 's_datum_header': 's_record_header',
    'datum_new': 'record_pool_allocate', 'datum_delete': 'record_pool_release', 'datum_get': 'record_pool_lookup',
    'datum_try_and_get': 'record_pool_lookup_checked', 'data_next_index': 'record_pool_next_used',
    'data_delete_all': 'record_pool_release_all', 'data_iterator_next': 'record_pool_iterator_step',
    'dot_product3d': 'dot3f', 'cross_product3d': 'cross3f', 'magnitude_squared3d': 'length_sq3f',
    'magnitude_squared2d': 'length_sq2f', 'distance_squared3d': 'distance_sq3f', 'scale_vector3d': 'scale3f',
    'set_real_vector3d': 'make_vector3f', 'vector_from_points3d': 'subtract_points3f',
    'matrix4x3_transform_point': 'transform4x3f_apply_point', 'matrix4x3_transform_normal': 'transform4x3f_apply_normal',
    'matrix4x3_identity': 'transform4x3f_set_identity',
    'pixel32_to_real_rgb_color': 'unpack_color3f', 'real_rgb_color_to_pixel32': 'pack_color3f',
    'pixel32_to_real_argb_color': 'unpack_color4f', 'real_argb_color_to_pixel32': 'pack_color4f',
    'real_to_long_round': 'float_to_int_nearest', 'real_to_long_floor': 'float_to_int_down',
}

KIND_NAMES = {'function': 'function', 'method': 'method', 'type': 'type', 'global': 'global', 'field': 'field',
              'macro': 'macro', 'enumerator': 'enumerator', 'param': 'param', 'local': 'local'}


def short_hash(t):
    return f'{zlib.crc32(t.encode()) & 0xffffff:06x}'


def primary_kind(c, t, d):
    if c['type'] or ('#class:' + t) in d['markers'] or (t.startswith(('s_', 'c_')) and (c['typeref'] or c['type'])):
        return 'class' if ('#class:' + t) in d['markers'] else 'type'
    if t in d['markers'] or c['function'] or c['method']:
        return 'function'
    for k in ('global', 'field', 'macro', 'enumerator', 'param', 'local'):
        if c[k]:
            return k
    if c['typeref']:
        return 'type'
    return 'reference'


def locations(occs, kinds=None, limit=4):
    pref = [o for o in occs if kinds and o[2] in kinds] or occs
    files = collections.OrderedDict()
    for path, ln, _ in pref:
        files.setdefault(path, ln)
    shown = [f'{p}:{ln}' for p, ln in list(files.items())[:limit]]
    allfiles = {p for p, _, _ in occs}
    extra = f' (+{len(allfiles) - len(shown)} files)' if len(allfiles) > len(shown) else ''
    return '; '.join(shown) + extra + f' [{len(occs)} uses]'


def function_va(t, d):
    marks = d['markers'].get(t)
    if marks:
        vas = sorted({va for va, _, _ in marks})
        return vas[0], len(vas)
    info = d['names'].get(t)
    if info and info['vas']:
        vas = sorted(info['vas'])
        return vas[0], len(vas)
    return None, 0


def main():
    scratch = sys.argv[1]
    d = load(scratch, refresh='--refresh' in sys.argv)
    texts = d['texts']
    model = Model(texts)
    existing = set(d['occ'])
    rows = []
    stats = collections.Counter()

    # ------------------------------------------------ (a)/(c) identifiers
    for t in sorted(d['occ']):
        occs = d['occ'][t]
        if t in P.CPP_WORDS or P.PLACEHOLDER.match(t):
            continue
        taint = taint_of(t, d)
        if not taint:
            continue
        c = collections.Counter(k for _, _, k in occs)
        defined = any(c[k] for k in P.DEF_KINDS) or t in d['markers'] or ('#class:' + t) in d['markers']
        kind = primary_kind(c, t, d)
        own_definition = any(c[k] for k in ('macro', 'enumerator', 'type', 'function', 'global', 'field', 'method'))
        lib = library_name(t, d)
        nwords = len(P.words(t))
        loc = locations(occs, {'function', 'method', 'type', 'field', 'global', 'macro', 'enumerator', 'param', 'local'})
        src = ' | '.join(taint)
        if lib:
            cls, reason, new = 'KEEP-PUBLIC', lib, ''
        elif not defined and (PUBLIC_PATTERN.match(t) or re.search(r'[A-Z]', t)):
            cls, reason, new = 'KEEP-PUBLIC', 'never declared in src/include and SDK-styled: supplied by SDK/CRT headers', ''
        elif PUBLIC_PATTERN.match(t) and not own_definition and not (t in d['names'] and d['names'][t]['game']):
            cls, reason, new = 'KEEP-PUBLIC', 'SDK/Win32 naming (X*, D3D*, ALL_CAPS constant) and not a game map symbol', ''
        elif nwords <= 1 and t not in DISTINCTIVE_SINGLES:
            cls, reason, new = 'KEEP-GENERIC', 'single common word or obvious term', ''
        elif generic_idiom_keep(t, kind):
            cls, reason, new = 'KEEP-GENERIC', 'generic idiom (common words + generic suffix or bit macro): kept by coordinator decision', ''
        else:
            cls = 'RENAME'
            reason = 'distinctive single token' if nwords <= 1 else f'{nwords}-word compound matching the source exactly'
            if GENERIC_COMPOUND.match(t):
                reason += '; generic idiom (noun_index/count/...): reviewer may downgrade to KEEP-GENERIC'
            new = None
        rows.append(dict(kind=kind, current=t, proposed=new, loc=loc, src=src, reason=reason, cls=cls, c=c))

    # proposals
    taken = set(existing)
    by_new = {}

    def claim(name, t):
        base, k = name, 2
        while name in taken and by_new.get(name) != t:
            name = f'{base}_{k}'
            k += 1
        taken.add(name)
        by_new[name] = t
        return name

    for r in rows:
        if r['cls'] != 'RENAME':
            continue
        t, kind = r['current'], r['kind']
        note = ''
        if t in CURATED:
            new = CURATED[t]
            note = 'behaviour-based (curated)'
            if new in d['names'] or new in d['dbg'] or new in d['types']:
                note += '; WARNING proposed name itself matches a source'
        elif kind in ('function', 'method'):
            va, n = function_va(t, d)
            if va is not None:
                new = f'function_{va:x}'
                note = f'placeholder from @retail/atlas va' + (f'; name shared by {n} functions, lowest va used' if n > 1 else '')
                if new in existing:
                    note += f'; {new} is already a name in src (a wrapper or another declaration of this va): suffixed to stay unique'
            else:
                new = f'function_x{short_hash(t)}'
                note = 'placeholder (no va known: an undecompiled callee or macro-made name)'
        elif kind == 'class':
            vas = sorted({va for va, _, _ in d['markers']['#class:' + t]})
            new = f'c_class_{vas[0]:x}'
            note = 'placeholder from the lowest marked method va'
        elif kind == 'type':
            new = ('s_' if not t.startswith('c_') else 'c_') + f'type_{short_hash(t)}'
            note = 'placeholder (types carry no va)'
        elif kind == 'field':
            offs = model.field_offsets(t)
            known = sorted({o for _, _, o in offs if o is not None})
            if len(known) == 1:
                new = f'field_{known[0]:x}'
                note = f'placeholder from computed offset 0x{known[0]:x} in ' + ','.join(sorted({n for _, n, o in offs if o is not None}))[:80]
            elif known:
                new = f'field_{known[0]:x}'
                note = 'placeholder; field sits at offsets ' + ','.join(f'0x{o:x}' for o in known) + ' in different records; lowest used'
            else:
                new = f'field_x{short_hash(t)}'
                note = 'placeholder; offset not computable by the layout model (member access only or unknown member type)'
        elif kind == 'global':
            new = f'g_global_{short_hash(t)}'
            note = 'placeholder (globals carry no known va here)'
        elif kind == 'macro':
            new = f'MACRO_{short_hash(t).upper()}'
            note = 'placeholder'
        elif kind == 'enumerator':
            new = f'_enum_value_{short_hash(t)}'
            note = 'placeholder'
        elif kind == 'param':
            new = f'arg_{short_hash(t)}'
            note = 'placeholder'
        elif kind == 'local':
            new = f'local_{short_hash(t)}'
            note = 'placeholder'
        else:
            new = f'name_{short_hash(t)}'
            note = 'placeholder'
        new = claim(new, t)
        assert new != t
        r['proposed'] = new
        r['reason'] += '; ' + note

    # ------------------------------------------------ (b) files
    file_rows = []
    debug_files = d['dfiles']
    objects = d['objects']
    file_markers = collections.defaultdict(list)
    for name, marks in d['markers'].items():
        if name.startswith('#class:'):
            continue
        for va, path, _ in marks:
            file_markers[path].append(va)
    stems = collections.defaultdict(list)
    for path in sorted(texts):
        stems[os.path.splitext(os.path.basename(path))[0]].append(path)
    new_stem = {}
    for stem, paths in sorted(stems.items()):
        low = stem.lower()
        if P.PLACEHOLDER.match(stem) or stem.startswith('unknown_'):
            continue
        srcs = []
        if low in objects and any(s.endswith(':game') for s in objects[low]):
            srcs.append('atlas/map2003 object ' + low + '.obj')
        elif low in objects:
            srcs.append('library object ' + low + '.obj (' + ','.join(sorted(objects[low]))[:40] + ')')
        if low in debug_files and low not in d['prefiles']:
            srcs.append('debug2007 source file ' + low + '.cpp/.h (absent before ' + P.PRE_DEBUG_COMMIT + ')')
        elif low in debug_files:
            srcs.append('debug2007 source file name, but the file predates the debug build')
        tainted = [s for s in srcs if not s.startswith(('library', 'debug2007 source file name, but'))]
        if not tainted:
            if srcs:
                stats['files-keep-predates-or-library'] += 1
                for p in paths:
                    file_rows.append(dict(kind='file', current=p, proposed='', loc=p, src=' | '.join(srcs),
                                          reason='name matches only a library object or a debug file name it predates',
                                          cls='KEEP-PUBLIC', c=None))
            continue
        nwords = len(P.words(stem))
        if nwords <= 1 and stem not in DISTINCTIVE_FILE_SINGLES:
            for p in paths:
                file_rows.append(dict(kind='file', current=p, proposed='', loc=p, src=' | '.join(srcs),
                                      reason='single common word', cls='KEEP-GENERIC', c=None))
            continue
        vas = sorted(va for p in paths for va in file_markers.get(p, []))
        if not vas:  # a header without its own .cpp: the lowest va of a function it declares
            for p in paths:
                for t, occs in d['occ'].items():
                    if any(o[0] == p and o[2] == 'function' for o in occs) and t in d['markers']:
                        vas += [va for va, _, _ in d['markers'][t]]
            vas.sort()
        if vas:
            ns = f'unknown_{vas[0]:06x}'
            why = f'placeholder from the lowest function va 0x{vas[0]:x} in the file'
        else:
            ns = f'unknown_x{short_hash(stem)}'
            why = 'placeholder; no marked function to take a va from'
        k = 2
        base_ns = ns
        while any(os.path.dirname(q) + '/' + ns + os.path.splitext(q)[1] in texts for q in paths) or ns in new_stem.values():
            ns = f'{base_ns}_{k}'
            k += 1
        new_stem[stem] = ns
        for p in paths:
            ext = os.path.splitext(p)[1]
            newp = os.path.dirname(p) + '/' + ns + ext
            file_rows.append(dict(kind='file', current=p, proposed=newp, loc=p, src=' | '.join(srcs),
                                  reason=f'{nwords}-word file name matching the source; {why}', cls='RENAME', c=None))

    # ------------------------------------------------ (d) comments
    comment_patterns = {
        'May 2003 / 2003 maps': re.compile(r'2003'),
        'halo-symbol-atlas / atlas': re.compile(r'atlas', re.I),
        '.obj object-file citation': re.compile(r'\b\w+\.obj\b'),
        'linker map / map file': re.compile(r'linker map|\bmap file|\bthe maps?\b', re.I),
        'debug build / debug15 / 2007': re.compile(r'debug build|debug15|halo2debug|\b2007\b|debug xbe|the debug\b', re.I),
        'assertion string citation': re.compile(r'\bassert(ion)?s?\b', re.I),
        'profile build': re.compile(r'profile build', re.I),
    }
    comment_counts = collections.Counter()
    comment_files = collections.defaultdict(set)
    any_comment = 0
    any_files = set()
    for path, comments in d['comments'].items():
        for _, ctext in comments:
            hit = False
            for label, rx in comment_patterns.items():
                if rx.search(ctext):
                    comment_counts[label] += 1
                    comment_files[label].add(path)
                    hit = True
            if hit:
                any_comment += 1
                any_files.add(path)

    # ------------------------------------------------ write
    out = os.path.join(OUT, 'phase-b-rename-map.tsv')
    allrows = file_rows + rows
    order = {'RENAME': 0, 'KEEP-GENERIC': 1, 'KEEP-PUBLIC': 2}
    allrows.sort(key=lambda r: (order[r['cls']], r['kind'], r['current'].lower()))
    with open(out, 'w', newline='', encoding='utf-8') as f:
        w = csv.writer(f, delimiter='\t', lineterminator='\n')
        w.writerow(['kind', 'current_name', 'proposed_name', 'location(s)', 'source_of_taint', 'reason', 'class'])
        for r in allrows:
            w.writerow([r['kind'], r['current'], r['proposed'] or '', r['loc'], r['src'], r['reason'], r['cls']])
    counts = collections.Counter((r['kind'], r['cls']) for r in allrows)
    src_counts = collections.Counter()
    for r in allrows:
        if r['cls'] != 'RENAME':
            continue
        for s in ('atlas', 'map2003', 'contrib-2003-notes', 'debug2007', 'ce2002'):
            if s in r['src']:
                src_counts[s] += 1
    generic_idiom = sum(1 for r in rows if r['cls'] == 'RENAME' and 'generic idiom' in r['reason'])
    curated = sum(1 for r in rows if r['cls'] == 'RENAME' and 'curated' in r['reason'])
    merges = [r['current'] + '->' + r['proposed'] for r in rows if r['cls'] == 'RENAME' and 'suffixed to stay unique' in r['reason']]
    rename_uses = sum(sum(r['c'].values()) for r in rows if r['cls'] == 'RENAME')
    stats.update({})
    result = {
        'by_kind_class': {f'{k}|{c}': n for (k, c), n in sorted(counts.items())},
        'rename_by_source': dict(src_counts),
        'generic_idiom_renames': generic_idiom,
        'curated_renames': curated,
        'function_placeholder_suffixed': merges,
        'rename_identifier_uses': rename_uses,
        'comments': {k: {'comments': comment_counts[k], 'files': len(comment_files[k])} for k in comment_patterns},
        'comments_any': {'comments': any_comment, 'files': len(any_files)},
        'files_keep_predates_or_library': stats['files-keep-predates-or-library'],
        'total_rows': len(allrows),
    }
    json.dump(result, open(os.path.join(OUT, 'phase-b-stats.json'), 'w'), indent=1)
    print(json.dumps(result, indent=1))
    print('outputs in', OUT)


if __name__ == '__main__':
    main()
