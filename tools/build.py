"""Builds every source in src/ into one LTCG image, build/halo2.exe, with its
map build/halo2.map, so tools/check.py can compare its functions with retail.

Each source is compiled with its own flags (a "// @flags" line in the source,
else config/files.json). A source with functions marked "// @retail 0x..." is
compiled as build/gen/<stem>_tu.cpp: the source itself, then a stand-in caller
for each marked function, with inlining off. The stand-ins share the source's
translation unit, so its own types and declarations are in scope. A stand-in
keeps its function in the image and out of line, as retail's own callers do.
Methods are called by qualified name, so a virtual one is called directly; a
class with a vtable is also copy-constructed once, which emits its vtable, so
its virtual functions' addresses escape as they do in retail. A function that
retail calls from library code (built without LTCG, so it needs the standard
convention) has its address stored in a generated global, which keeps that
convention here too; so does one marked "// @retail 0x... standard". Sources
in src/stubs/ are compiled without /GL, so they keep the standard calling
conventions retail uses for code outside the project. A generated ballast
object, which the linker discards, keeps inlining decisions stable as the
program grows (see INLINE_THRESHOLD). The members of the SDK's d3d8ltcg.lib
that retail holds are linked as objects, in retail's order, before the
libraries (see D3D_OBJECTS); d3d_objects extracts them into build/d3d8ltcg.

build(units=...) builds a reduced image instead, build/reduced.img with its
map build/reduced.map: only the listed sources (with their stand-ins), the
ballast and the libraries, linked with /FORCE:UNRESOLVED so that what the
left-out sources define does not stop the link. tools/permute.py --fast scores
variants on it, and tools/check.py --fast checks functions on it (see
tools/reduced.py); nothing else may use it. It is never named .exe: it does
not run, and antivirus software flags such images. build(image=...) links to
the given path instead.

    python tools/build.py
"""
import json
import os
import pickle
import re
import signal
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass

from inventory import read_rows
from xbe import ROOT, xdk_dir

# "deleting" marks the deleting destructor the compiler builds around the
# destructor below it, or, followed by a class name, around that class's
# implicit destructor (a marker with no function under it); "destructor"
# followed by a class name marks that class's implicit destructor itself;
# "standard" gives the function below the address escape of a function called
# from library code, so it keeps the standard convention (a last resort, see
# docs/DECOMPILING.md; a constructor, whose address C++ cannot take, gets a
# caller built without LTCG instead, see standard_source); "vcall" followed by a vtable offset marks the thunk the
# compiler emits for a pointer to a virtual member function, which calls
# through that vtable slot (a marker with no function under it, and no
# stand-in: the code that takes the pointer emits the thunk); "adjustor"
# followed by a name and an offset marks the this-adjustor thunk the compiler
# emits for a virtual member reached through a base at that offset: a class
# name alone means its deleting destructor, class::member that member (a
# marker with no function under it, and no stand-in of its own: the class's
# vtable, which the vtable stand-in emits, brings the thunk in)
MARKER = re.compile(r'^\s*//\s*@(retail|stub)\s+(0x[0-9a-fA-F]+)'
                    r'(?:\s+(deleting)(?:\s+([A-Za-z_][\w:]*))?|\s+destructor\s+([A-Za-z_][\w:]*)|\s+(standard)'
                    r'|\s+vcall\s+(0x[0-9a-fA-F]+)|\s+adjustor\s+([A-Za-z_][\w:]*)\s+(0x[0-9a-fA-F]+|\d+))?\s*$')
DELETING = "`deleting destructor'"  # the compiler's deleting destructor (??_G or ??_E), as linkmap names it
FLAGS = re.compile(r'^\s*//\s*@flags\s+(.+?)\s*$')
FLAGS_LINES = 30
SIZE_FLAGS = ['/O1', '/Ob0', '/Gr']
ARGUMENT_STRIDE = 16
LIBRARIES = ['libcmt.lib', 'libcpmt.lib', 'xapilib.lib', 'xboxkrnl.lib', 'dsound.lib', 'xonlines.lib',
             'xvoice.lib', 'xnet.lib', 'd3d8ltcg.lib', 'xgraphicsltcg.lib', 'd3dx8.lib']
# The members of the SDK's d3d8ltcg.lib that retail holds, in retail's layout
# order: each of retail's Direct3D functions was matched to the same function
# in our image, whose map names the member it comes from, and the members are
# listed by the retail addresses of their functions. build() links them as
# objects, in this order, after the program's objects and before the libraries
# (the library's other members still come from the library). Link-time code
# generation gives the functions of a call cycle their conventions by the
# order in which it reaches them; with these members left in the library it
# reaches the push-buffer cycle (SetFence, MakeRequestedSpace, BlockOnTime)
# differently from retail's link, and gives BlockOnTime, not
# MakeRequestedSpace, its argument in a register. Linked this way the split
# is retail's (a full check gained 10 functions and lost none). Leaving any of
# several members to the library undoes it, so keep the list whole.
D3D_LIBRARY = 'd3d8ltcg.lib'
D3D_OBJECTS = ['d3ddev', 'state', 'vshader', 'd3dbase', 'present', 'clear', 'pshader', 'pusher', 'drawprim',
               'resource', 'surface', 'dxgcreate', 'floatmath', 'math', 'pixeljar', 'mpcore', 'mpintr', 'mpdac',
               'mpmode', 'lazy', 'shadersnapshot', 'mphal', 'pushres', 'debug', 'combiner']
D3D_FOLDER = 'd3d8ltcg'  # build/d3d8ltcg: the D3D_OBJECTS, extracted from the SDK (never committed)
EXE_NAME = 'halo2.exe'
MAP_NAME = 'halo2.map'
REDUCED_NAME = 'reduced'  # build(units=...): build/reduced.img, build/reduced.map, build/gen/entry_reduced.cpp
# The compiler's link-time inliner behaves differently as the program grows:
# adding about 30 KB of LTCG code anywhere changed inlining decisions across
# the image. The ballast object (unreferenced functions the linker discards)
# keeps the program size well past that point, and is compiled with the
# compiler's /d2inlT inlining-threshold option at the value the matched code
# was found at, so adding code no longer moves the result. See
# docs/DECOMPILING.md.
INLINE_THRESHOLD = 81
BALLAST_FUNCTIONS = 9000  # comfortably past the size where inlining changed
TOOL_TIMEOUT = max(1, int(os.environ.get('HALO2_TOOL_TIMEOUT') or 600))
# ^ seconds one cl/link/lib run may take before it is killed and retried once;
#   Wine occasionally wedges a tool at 0% CPU, and one wedge must not stall a build


@dataclass
class Marked:
    path: str
    retail: int
    name: str
    returns: str
    params: list
    stub: bool = False
    cls: str = ''
    kind: str = 'function'  # 'function', 'method', 'constructor', 'destructor', 'deleting', 'vcall' or 'adjustor'
    const: bool = False  # a const method
    standard: bool = False  # "standard": its address escapes (a constructor: a caller outside LTCG calls it)


def _split_params(text):
    parts, depth, current = [], 0, ''
    for ch in text:
        if ch in '(<[':
            depth += 1
        elif ch in ')>]':
            depth -= 1
        if ch == ',' and depth == 0:
            parts.append(current)
            current = ''
        else:
            current += ch
    if current.strip():
        parts.append(current)
    return [p.strip() for p in parts]


def _param_type(param):
    """'unsigned long *crc_reference' -> 'unsigned long *'."""
    param = param.split('=')[0].strip()
    m = re.match(r'^(.*?)([A-Za-z_]\w*)\s*(\[\s*\d*\s*\])?$', param)
    if m and m.group(1).strip() and not m.group(1).strip().endswith(('struct', 'union', 'enum')):
        kind = m.group(1).strip()
        return kind + ' *' if m.group(3) else kind
    return param


QUALIFIERS = re.compile(r'\b(?:__stdcall|__cdecl|__fastcall|__thiscall|__clrcall|__inline|__forceinline|'
                        r'inline|PRIVATE|static|extern|virtual)\b')
HEADER = re.compile(r'^(.*?)\s*((?:[A-Za-z_]\w*::)*~?[A-Za-z_]\w*)\s*\(')
CLASS_DECLARATION = re.compile(r'\b(?:struct|class|union)\s+([A-Za-z_]\w*)')
CLASS_TYPEDEF = re.compile(r'\btypedef\s+(?:struct|class|union)\b[^;{]*(?:\{.*?\})?\s*([A-Za-z_]\w*)\s*;', re.S)
CLASS_BODY = re.compile(r'\b(?:struct|class)\s+([A-Za-z_]\w*)\s*(?::\s*([^{;]*))?\{')
PURE_VIRTUAL = re.compile(r'\)\s*(?:const\s*)?=\s*0\s*;')


def _balanced(text, start, opening='(', closing=')'):
    """The text inside the brackets that open at text[start], or None."""
    depth = 0
    for i in range(start, len(text)):
        depth += text[i] == opening
        depth -= text[i] == closing
        if not depth:
            return text[start + 1:i]
    return None


def _kind(name):
    """(class, kind) of a possibly qualified function name."""
    cls, _, last = name.rpartition('::')
    if not cls:
        return '', 'function'
    if last.startswith('~'):
        return cls, 'destructor'
    return (cls, 'constructor') if last == cls.rpartition('::')[2] else (cls, 'method')


def scan(text, path):
    lines = text.splitlines()
    found = []
    for i, line in enumerate(lines):
        m = MARKER.match(line)
        if not m:
            continue
        if m.group(4):
            found.append(Marked(path, int(m.group(2), 16), f'{m.group(4)}::{DELETING}', '', [],
                                m.group(1) == 'stub', m.group(4), 'deleting'))
            continue
        if m.group(5):
            cls = m.group(5)
            found.append(Marked(path, int(m.group(2), 16), f'{cls}::~{cls.rpartition("::")[2]}', '', [],
                                m.group(1) == 'stub', cls, 'destructor'))
            continue
        if m.group(7):
            if m.group(1) == 'stub':
                raise SystemExit(f'{path}:{i + 1}: "vcall" marks a thunk the build emits, not a stub')
            found.append(Marked(path, int(m.group(2), 16), vcall_name(int(m.group(7), 16)), '', [],
                                kind='vcall'))
            continue
        if m.group(8):
            if m.group(1) == 'stub':
                raise SystemExit(f'{path}:{i + 1}: "adjustor" marks a thunk the build emits, not a stub')
            cls, _, member = m.group(8).rpartition('::')
            if not cls:
                cls, member = member, DELETING
            found.append(Marked(path, int(m.group(2), 16), adjustor_name(cls, member, int(m.group(9), 0)), '', [],
                                False, cls, 'adjustor'))
            continue
        header = ''
        for later in lines[i + 1:]:
            header += ' ' + later.split('//')[0]
            if '{' in later:
                break
        header = ' '.join(header.split('{')[0].split())
        h = HEADER.match(header)
        inner = h and _balanced(header, h.end() - 1)
        if inner is None:
            raise SystemExit(f'{path}:{i + 2}: cannot read the function after @{m.group(1)} {m.group(2)}')
        params = [_param_type(p) for p in _split_params(inner) if p != '...']  # a stand-in passes no variadic arguments
        if params == ['void']:
            params = []
        name = h.group(2)
        cls, kind = _kind(name)
        returns = ' '.join(QUALIFIERS.sub(' ', h.group(1)).split())
        if kind in ('constructor', 'destructor'):
            returns = ''
        if m.group(3):  # "deleting": the deleting destructor the compiler builds around this destructor
            if kind != 'destructor':
                raise SystemExit(f'{path}:{i + 1}: "deleting" marks a destructor, not {name}')
            name, kind = f'{cls}::{DELETING}', 'deleting'
        standard = bool(m.group(6))
        if standard and (m.group(1) == 'stub' or kind not in ('function', 'method', 'constructor')):
            raise SystemExit(f'{path}:{i + 1}: "standard" marks a decompiled function, method or constructor, '
                             f'not {name}')
        trailer = header[h.end() + len(inner) + 1:]
        found.append(Marked(path, int(m.group(2), 16), name, returns or ('int' if kind == 'function' else ''),
                            params, m.group(1) == 'stub', cls, kind, bool(re.match(r'\s*const\b', trailer)),
                            standard))
    return found


def vcall_name(offset):
    """The plain name (as linkmap names it) of the compiler's thunk that calls
    through the vtable slot at offset."""
    return f"`vcall'{{{offset:#x}}}"


def adjustor_name(cls, member, offset):
    """The plain name (as linkmap names it) of the compiler's this-adjustor
    thunk for cls's member reached through a base at offset."""
    return f"{cls}::{member}`adjustor{{{offset:#x}}}'"


def _declared(text):
    return set(CLASS_DECLARATION.findall(text)) | set(CLASS_TYPEDEF.findall(text))


def class_names(texts):
    """Names declared as struct, class or union in the given sources."""
    return set().union(*map(_declared, texts))


def _class_bodies(text):
    """(name, has a virtual member, bases, has a pure virtual member) of each class defined in text."""
    out = []
    for m in CLASS_BODY.finditer(text):
        body = _balanced(text, m.end() - 1, '{', '}') or ''
        bases = [part.split()[-1] for part in (m.group(2) or '').split(',') if part.split()]
        out.append((m.group(1), bool(re.search(r'\bvirtual\b', body)), bases, bool(PURE_VIRTUAL.search(body))))
    return out


def polymorphic_classes(texts):
    """Classes with a vtable (a virtual member of their own or of a base) that
    can be instantiated (no pure virtual member of their own)."""
    return _polymorphic(map(_class_bodies, texts))


def _polymorphic(bodies):
    """polymorphic_classes, from each text's _class_bodies."""
    found = {}
    for found_in_text in bodies:
        for name, own, bases, pure in found_in_text:
            found[name] = (own, bases, pure)
    virtual = {name for name, (own, _, _) in found.items() if own}
    grew = True
    while grew:
        inherited = {name for name, (_, bases, _) in found.items() if virtual.intersection(bases)}
        grew = not inherited <= virtual
        virtual |= inherited
    return {name for name in virtual if not found[name][2]}


def vtable_classes(marked, polymorphic):
    """The classes, in order, whose vtables a source's stand-ins emit. A class
    with a marked constructor needs none: that constructor's stand-in emits the
    vtable, and copy-constructing would add a second, implicit constructor with
    the same plain name."""
    constructed = {m.cls for m in marked if m.kind == 'constructor'}
    out = []
    for m in marked:
        if (m.cls and m.cls.rpartition('::')[2] in polymorphic and m.cls not in out
                and m.cls not in constructed):
            out.append(m.cls)
    return out


def _is_class(kind, classes):
    words = re.findall(r'[A-Za-z_]\w*', kind)
    return bool(words) and (words[0] in ('struct', 'class', 'union') or any(w in classes for w in words))


def _argument(kind, offset, prefix, classes):
    """An expression reading one parameter of type `kind` from the argument buffer.
    Scalars and pointers are read through a volatile pointer, so the optimizer
    cannot fold them. A class or struct passed by value cannot be copied from
    volatile storage (no volatile copy constructor), so it is read through a
    plain pointer: the buffer itself stays volatile-declared, and the value
    still comes from memory the optimizer knows nothing about."""
    place = f'standin_{prefix}_arguments + {offset}'
    if kind.endswith('&'):
        return f'**({kind[:-1].strip()} * volatile *)({place})'
    if '*' not in kind and _is_class(kind, classes):
        return f'*({kind} *)(void *)({place})'
    return f'*({kind} volatile *)({place})'


def outside_callees(rows):
    """Game functions that retail calls from library or third-party code. A
    caller decompiled in src/ is game code whatever its owner says (the
    inventory can mistake a small game function for a library one, as with
    0x22ec84), and src/ builds it with LTCG, so it does not count."""
    out = set()
    for r in rows.values():
        if r['owner'] not in ('game', 'eh') and not r.get('source'):
            out.update(t for t in (int(c, 16) for c in r['calls'].split())
                       if rows.get(t, {}).get('owner') == 'game')
    return out


def _escape(m, name):
    """A global that holds a marked function's address, or None when it has none."""
    if m.kind == 'function':
        return f'void *volatile {name} = (void *)&{m.name};'
    if m.kind == 'method':
        member = m.name.rpartition('::')[2]
        const = ' const' if m.const else ''
        return f'{m.returns} ({m.cls}::*volatile {name})({", ".join(m.params)}){const} = &{m.cls}::{member};'
    return None


def tu_source(source_abs, marked, prefix, classes=(), polymorphic=(), outside=(), keep_results=False):
    """The translation unit that holds a source and its stand-ins: the source
    itself; one function per class with a vtable that copy-constructs it,
    which emits the vtable (before the pragmas below, so the compiler-made
    members it brings in, such as implicit and deleting destructors, are
    inlined as in retail); with inlining off, a global holding the address of
    each marked function retail calls from outside LTCG or whose marker says
    "standard" (data, which the checker never compares); and one function per
    marked function that calls it with values read from a volatile buffer, so
    the call stays out of line."""
    buffer = f'standin_{prefix}_arguments'
    vtables = vtable_classes(marked, polymorphic)
    out = [f'/* generated by tools/build.py: {os.path.basename(source_abs)} with its stand-in callers */',
           f'#include "{source_abs}"']
    if vtables or any(m.kind == 'constructor' for m in marked):
        out.append('#include <new>')
    out.append(f'static __declspec(align(16)) unsigned char volatile {buffer}[{ARGUMENT_STRIDE * 16}];')
    for j, cls in enumerate(vtables):
        copied = f'*({cls} *)(void *)({buffer} + {ARGUMENT_STRIDE})'
        out.append(f'void standin_{prefix}_vtable_{j}(void) {{ ::new ((void *){buffer}) {cls}({copied}); }}')
    out += ['#pragma auto_inline(off)', '#pragma inline_depth(0)']
    for k, m in enumerate(marked):
        escape = (m.retail in outside or m.standard) and not m.stub and _escape(m, f'standin_{prefix}_outside_{k}')
        if escape:
            out.append(escape)
    for k, m in enumerate(marked):
        if m.kind in ('vcall', 'adjustor'):
            continue  # the source's own code, or the class's vtable, emits the thunk
        args = ', '.join(_argument(p, ARGUMENT_STRIDE * n, prefix, classes) for n, p in enumerate(m.params))
        member = m.name.rpartition('::')[2]
        if m.kind == 'constructor':
            call = f'::new ((void *){buffer}) {m.cls}({args})'
        elif m.kind == 'destructor':
            call = f'(({m.cls} *){buffer})->{m.cls}::{member}()'
        elif m.kind == 'deleting':
            call = f'delete ({m.cls} *){buffer}'
        elif m.kind == 'method':
            call = f'(({m.cls} *){buffer})->{m.cls}::{member}({args})'
        else:
            call = f'{m.name}({args})'
        stored = (m.kind in ('function', 'method') and m.returns != 'void'
                  and ('*' in m.returns or not _is_class(m.returns, classes)))
        if stored:
            out.append(f'static {m.returns} volatile standin_{prefix}_result_{k};')
            call = f'standin_{prefix}_result_{k} = {call}'
        elif keep_results and m.kind in ('function', 'method') and m.returns != 'void':
            # without /GL the compiler drops a call to a side-effect-free function
            # whose class result is unused, so store the result in the buffer
            call = f'*({m.returns} *)(unsigned char *){buffer} = {call}'
        out.append(f'void standin_{prefix}_{k}(void) {{ {call}; }}')
    return '\n'.join(out) + '\n'


def standard_constructors(marked):
    """The constructors among a source's markers whose marker says "standard"."""
    return [m for m in marked if m.kind == 'constructor' and m.standard]


def standard_source(source_text, marked, prefix, classes=()):
    """The source of the object that keeps the standard (thiscall) convention
    of each constructor marked "standard" (standard_constructors): C++ cannot
    take a constructor's address, so instead a caller built without LTCG, as
    the stubs are, calls it, and link-time code generation keeps the standard
    convention of a function that code outside it calls. The object holds that
    caller alone: the source's #include lines (the class must be declared in a
    header the source includes; the constructor's body stays in the source's
    own LTCG object) and one function per constructor that constructs into a
    volatile buffer. Nothing calls those functions; the linker keeps them, as
    it keeps the stubs, since they are not packaged as COMDATs."""
    buffer = f'standin_{prefix}_standard_arguments'
    out = ['/* generated by tools/build.py: callers built without LTCG that keep the standard convention of the '
           'constructors marked "standard" */']
    out += [line.strip() for line in source_text.splitlines() if re.match(r'\s*#\s*include\b', line)]
    out += ['#include <new>',
            f'static __declspec(align(16)) unsigned char volatile {buffer}[{ARGUMENT_STRIDE * 16}];']
    for k, m in enumerate(standard_constructors(marked)):
        args = ', '.join(_argument(p, ARGUMENT_STRIDE * n, f'{prefix}_standard', classes)
                         for n, p in enumerate(m.params))
        out.append(f'void standard_{prefix}_{k}(void) {{ ::new ((void *){buffer}) {m.cls}({args}); }}')
    return '\n'.join(out) + '\n'


def standard_object(stem):
    """The name of the object build() builds from standard_source for src/<stem>.cpp."""
    return f'standard_{stem}.obj'


PATH_SWITCHES = ('/Fo', '/Fe', '/Fd', '/Fa', '/OUT:', '/MAP:')


def wine_path(path):
    """An absolute host path as Wine's Z: drive sees it."""
    return 'Z:' + path.replace('/', '\\')


def tool_arg(arg):
    """Under Wine, cl and link read a leading '/' as a switch, so absolute host
    paths, alone or after a switch such as /Fo, go over as Z: drive paths. On
    Windows, including MSYS2's and Cygwin's Python, arguments pass unchanged."""
    if sys.platform in ('win32', 'cygwin', 'msys'):
        return arg
    if os.path.dirname(arg) not in ('', '/') and os.path.isdir(os.path.dirname(arg)):
        return wine_path(arg)
    for switch in PATH_SWITCHES:
        if arg.startswith(switch + '/'):
            return switch + wine_path(arg[len(switch):])
    return arg


def _run_tool_once(command, cwd, env):
    """Runs command in its own process group, so a timeout can kill the whole
    tree. The SDK's tools are launched through a wrapper that execs Wine, and
    Wine leaves a wineserver behind; killing only the wrapper would leave that
    server wedged for the next attempt."""
    posix = os.name == 'posix'
    p = subprocess.Popen(command, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         text=True, start_new_session=posix)
    try:
        stdout, stderr = p.communicate(timeout=TOOL_TIMEOUT)
    except subprocess.TimeoutExpired:
        if posix:
            os.killpg(p.pid, signal.SIGKILL)
        else:
            p.kill()
        p.communicate()
        raise
    return p.returncode, stdout, stderr


def run_tool(tool, args, cwd, xdk):
    """Runs one of the SDK's tools and returns what it printed. A tool that
    produces nothing for TOOL_TIMEOUT seconds is killed and tried once more:
    Wine sometimes wedges cl.exe or link.exe at 0% CPU."""
    include, lib = (tool_arg(os.path.join(xdk, d)) for d in ('include', 'lib'))
    env = dict(os.environ, INCLUDE=include, LIB=lib)
    command = [os.path.join(xdk, 'bin', 'vc71', tool), '/nologo', *(tool_arg(a) for a in args)]
    for attempt in (1, 2):
        try:
            returncode, stdout, stderr = _run_tool_once(command, cwd, env)
            break
        except subprocess.TimeoutExpired:
            if attempt == 2:
                raise SystemExit(f'{tool} timed out after {TOOL_TIMEOUT}s; set HALO2_TOOL_TIMEOUT to change it')
            print(f'{tool} timed out after {TOOL_TIMEOUT}s; retrying once', file=sys.stderr)
    if returncode:
        raise SystemExit(f'{tool} failed:\n{stdout}{stderr}')
    return stdout


def d3d_objects(root, xdk):
    """The paths of the D3D_OBJECTS, in order, in build/d3d8ltcg. They are
    extracted from the SDK's d3d8ltcg.lib with its lib tool, and extracted
    again only when the library changes (its size and time, which a stamp
    file in the folder records)."""
    library = os.path.join(xdk, 'lib', D3D_LIBRARY)
    if not os.path.isfile(library):
        raise SystemExit(f'{library} not found: the build links {len(D3D_OBJECTS)} of its members as objects '
                         '(D3D_OBJECTS in tools/build.py). Set XDK_DIR to the xbox folder of Xbox SDK 5849.')
    folder = os.path.join(root, 'build', D3D_FOLDER)
    paths = [os.path.join(folder, f'{stem}.obj') for stem in D3D_OBJECTS]
    stat = os.stat(library)
    key, stamp = f'{D3D_LIBRARY} {stat.st_size} {stat.st_mtime_ns}\n', os.path.join(folder, 'extracted.txt')
    if os.path.exists(stamp) and all(map(os.path.isfile, paths)):
        with open(stamp, encoding='utf-8') as f:
            if f.read() == key:
                return paths
    os.makedirs(folder, exist_ok=True)
    members = {}  # a member's file name: its paths in the library
    for line in filter(None, map(str.strip, run_tool('Lib.Exe', ['/LIST', library], folder, xdk).splitlines())):
        members.setdefault(re.split(r'[\\/]', line)[-1].lower(), []).append(line)
    for stem, path in zip(D3D_OBJECTS, paths):
        found = members.get(f'{stem}.obj', [])
        if len(found) != 1:
            raise SystemExit(f'{library} has {"more than one member" if found else "no member"} named {stem}.obj, '
                             'which the build links as an object (D3D_OBJECTS in tools/build.py). Set XDK_DIR to '
                             'the xbox folder of Xbox SDK 5849.')
        run_tool('Lib.Exe', [f'/EXTRACT:{found[0]}', f'/OUT:{path}.tmp', library], folder, xdk)
        os.replace(path + '.tmp', path)
    with open(stamp, 'w', encoding='utf-8') as f:
        f.write(key)
    return paths


def _stale(target, sources, newest_header):
    if not os.path.exists(target):
        return True
    t = os.path.getmtime(target)
    return newest_header > t or any(os.path.getmtime(s) > t for s in sources)


def source_flags(flags):
    """What every source in src/ is compiled with, whatever files.json says:
    LTCG, and the register calling convention retail's code uses."""
    return ['/GL', '/Gr', *flags]


def file_flags(text, name, config):
    """A source's own flags: its "// @flags ..." line among the first 30 lines,
    else its entry in config/files.json, else the default."""
    for line in text.splitlines()[:FLAGS_LINES]:
        m = FLAGS.match(line)
        if m:
            return m.group(1).split()
    return config.get('files', {}).get(name, config['default'])


def _source_names(root):
    src = os.path.join(root, 'src')
    return sorted(n for n in os.listdir(src) if n.endswith('.cpp')) if os.path.isdir(src) else []


# path -> ((modification time, size), text or None until read, {what: derived
# from the text}): a file is read and parsed again only when it changes (_stale
# trusts the same times), so repeated builds in one process (tools/permute.py)
# parse only the source that changed
_FILES = {}
_derived = False  # whether this process derived anything (save_parse_cache)


def _read(path, what=None, derive=None):
    """The file's text, or derive(text) cached under what."""
    stat = os.stat(path)
    key = (stat.st_mtime_ns, stat.st_size)
    entry = _FILES.get(path)
    if entry is None or entry[0] != key:
        entry = _FILES[path] = (key, None, {})
    if what is not None and what in entry[2]:
        return entry[2][what]
    if entry[1] is None:
        with open(path, encoding='utf-8') as f:
            entry = _FILES[path] = (key, f.read(), entry[2])
    if what is None:
        return entry[1]
    global _derived
    _derived = True
    entry[2][what] = derive(entry[1])
    return entry[2][what]


def _parse_version():
    """This module's own time and size: a parse cache is only as good as the code that wrote it."""
    stat = os.stat(__file__)
    return stat.st_mtime_ns, stat.st_size


def load_parse_cache(path):
    """Takes in what an earlier process derived from the sources and headers
    (save_parse_cache), for each file unchanged since. tools/check.py --fast
    runs a process per check, and parsing every source costs it a second."""
    try:
        with open(path, 'rb') as f:
            version, files = pickle.load(f)
    except Exception:  # missing, or written by another version of Python
        return
    if version == _parse_version():
        for name, (key, derived) in files.items():
            _FILES.setdefault(name, (key, None, derived))


def save_parse_cache(path):
    """Saves what this process derived from the files it read (see
    load_parse_cache), unless it derived nothing new."""
    if not _derived and os.path.exists(path):
        return
    files = {name: (key, derived) for name, (key, _, derived) in _FILES.items() if derived}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path + '.tmp', 'wb') as f:
        pickle.dump((_parse_version(), files), f)
    os.replace(path + '.tmp', path)


def marked_sources(root=ROOT):
    """Every function marked "// @retail 0x..." in src/*.cpp, in file order.
    Stubs ("// @stub 0x...") are not among them: they are not checked and get no stand-in."""
    marked = []
    for name in _source_names(root):
        found = _read(os.path.join(root, 'src', name), 'scan', lambda text: scan(text, f'src/{name}'))
        marked += [m for m in found if not m.stub]
    return marked


def stub_sources(root=ROOT):
    """Every function marked "// @stub 0x..." in src/stubs/*.cpp, in file order."""
    folder = os.path.join(root, 'src', 'stubs')
    marked = []
    if os.path.isdir(folder):
        for name in sorted(n for n in os.listdir(folder) if n.endswith('.cpp')):
            found = _read(os.path.join(folder, name), 'scan', lambda text: scan(text, f'src/stubs/{name}'))
            marked += [m for m in found if m.stub]
    return marked


def source_objects(root=ROOT):
    """{an object's name, as build() names it and the linker map shows it: its
    source ('src/<name>.cpp' or 'src/stubs/<name>.cpp')}."""
    out = {f'{os.path.splitext(n)[0]}.obj': f'src/{n}' for n in _source_names(root)}
    out.update({standard_object(os.path.splitext(n)[0]): f'src/{n}' for n in _source_names(root)})
    folder = os.path.join(root, 'src', 'stubs')
    if os.path.isdir(folder):
        out.update({f'stubs_{os.path.splitext(n)[0]}.obj': f'src/stubs/{n}' for n in os.listdir(folder)
                    if n.endswith('.cpp')})
    return out


def standin_names(marked):
    """{(path, retail address): the name of its stand-in} for markers listed as
    marked_sources() lists them: the k-th marker of src/<stem>.cpp has
    standin_<stem>_<k>."""
    names, counts = {}, {}
    for m in marked:
        k = counts[m.path] = counts.get(m.path, -1) + 1
        names[(m.path, m.retail)] = f'standin_{os.path.splitext(os.path.basename(m.path))[0]}_{k}'
    return names


def entry_source(decls, calls):
    # no _fltused here: float code pulls __fltused from libcmt (fpinit.obj), and a second one clashes
    return ('/* generated by tools/build.py: the image entry point */\n' + '\n'.join(decls) +
            '\nextern "C" int entry(void)\n{\n' + '\n'.join(calls) + '\n\treturn 0;\n}\n')


def ballast_source(functions=BALLAST_FUNCTIONS):
    """Unreferenced functions that only add to the program size the inliner sees
    (see INLINE_THRESHOLD). Nothing calls them, so the image never holds them."""
    out = ['/* generated by tools/build.py: LTCG ballast, discarded by the linker */',
           'static volatile long ltcg_ballast_v[64];',
           '#define B1(k) ltcg_ballast_v[(k) & 63] = ltcg_ballast_v[((k) * 7) & 63] * ((k) + 3) + a;',
           '#define B4(k) B1(k) B1((k) + 1) B1((k) + 2) B1((k) + 3)',
           '#define B32(k) B4(k) B4((k) + 4) B4((k) + 8) B4((k) + 12) B4((k) + 16) B4((k) + 20) B4((k) + 24) '
           'B4((k) + 28)']
    out += [f'void ltcg_ballast_{k}(long a) {{ B32({k}) }}' for k in range(functions)]
    return '\n'.join(out) + '\n'


def build(root=ROOT, xdk=None, units=None, image=None, include=(), jobs=1, reuse=False, timings=None):
    """Builds the image and returns its map's path. units, if given, is the set
    of sources ('src/<name>.cpp', 'src/stubs/<name>.cpp') of a reduced image
    (see the module docstring). image, if given, is the path to link the image
    to, instead of build/halo2.exe or build/reduced.img; its map goes next to
    it. include lists symbols the link must take in (/INCLUDE), with the
    library objects that define them. jobs compiles that many stale sources
    at once. reuse keeps the image of the last link when nothing it was
    linked from has changed since (its objects, their times, the link's
    arguments; <image>.inputs records them). timings, a dict if
    given, gets the seconds spent compiling ('compile', of 'compiled' sources)
    and linking ('link', None when the last image was kept)."""
    xdk = xdk or xdk_dir()
    if not os.path.exists(os.path.join(xdk, 'bin', 'vc71', 'CL.Exe')):
        raise SystemExit(f'Xbox SDK 5849 not found at {xdk}. Put its xbox folder at sdk/xbox, '
                         'or set XDK_DIR to it.')
    # the map names an object by its file name alone, so a source must not share one with a D3D member
    clash = sorted(n for n in _source_names(root) if os.path.splitext(n)[0].lower() in D3D_OBJECTS)
    if clash:
        raise SystemExit(f'src/{clash[0]} builds an object with the name of a {D3D_LIBRARY} member that the build '
                         'links as an object (D3D_OBJECTS in tools/build.py); rename the source.')
    d3d = d3d_objects(root, xdk)
    src, out = os.path.join(root, 'src'), os.path.join(root, 'build')
    obj_dir, gen = os.path.join(out, 'obj'), os.path.join(out, 'gen')
    os.makedirs(obj_dir, exist_ok=True)
    os.makedirs(gen, exist_ok=True)
    with open(os.path.join(root, 'config', 'files.json'), encoding='utf-8') as f:
        config = json.load(f)
    headers = [os.path.join(d, n) for base in ('include', 'src') for d, _, ns in os.walk(os.path.join(root, base))
               for n in ns if n.endswith('.h')]
    newest_header = max((os.path.getmtime(h) for h in headers), default=0)
    includes = ['/I', os.path.join(root, 'include'), '/I', src]
    timings = {} if timings is None else timings
    timings.update(compile=0.0, compiled=0, link=None)
    pending = []

    def compile_pending():
        started = time.perf_counter()
        if jobs > 1:
            with ThreadPoolExecutor(jobs) as pool:
                list(pool.map(lambda args: run_tool('CL.Exe', args, root, xdk), pending))
        else:
            for args in pending:
                run_tool('CL.Exe', args, root, xdk)
        timings['compile'] += time.perf_counter() - started
        timings['compiled'] += len(pending)
        pending.clear()

    def compile_(args):
        """Compiles now, or with jobs above 1 with the others, at once, before the link."""
        pending.append(args)
        if jobs == 1:
            compile_pending()

    marked_by_path = {}
    for m in marked_sources(root):
        marked_by_path.setdefault(m.path, []).append(m)

    parsed = [os.path.join(src, name) for name in _source_names(root)] + headers
    classes = set().union(*(_read(path, 'declared', _declared) for path in parsed))
    polymorphic = _polymorphic(_read(path, 'class bodies', _class_bodies) for path in parsed)
    outside = outside_callees(read_rows(os.path.join(root, 'config', 'functions.csv')))

    objects, entry_calls, entry_decls, standard = [], [], [], []
    for name in _source_names(root):
        if units is not None and f'src/{name}' not in units:
            continue
        path = os.path.join(src, name)
        stem = os.path.splitext(name)[0]
        text = _read(path)
        flags = file_flags(text, name, config)
        obj = os.path.join(obj_dir, stem + '.obj')
        marked = marked_by_path.get(f'src/{name}')
        compiled, depends = path, [path, os.path.join(root, 'config', 'files.json')]
        if marked:
            compiled = os.path.join(gen, f'{stem}_tu.cpp')
            source = tu_source(os.path.abspath(path), marked, stem, classes, polymorphic, outside,
                               keep_results='/GL-' in flags)
            if not os.path.exists(compiled) or open(compiled, encoding='utf-8').read() != source:
                with open(compiled, 'w', encoding='utf-8') as f:
                    f.write(source)
            depends.append(compiled)
            standins = [f'standin_{stem}_{k}' for k, m in enumerate(marked) if m.kind not in ('vcall', 'adjustor')]
            standins += [f'standin_{stem}_vtable_{j}' for j in range(len(vtable_classes(marked, polymorphic)))]
            for standin in standins:
                entry_decls.append(f'void {standin}(void);')
                entry_calls.append(f'\t{standin}();')
            if standard_constructors(marked):
                standard.append((stem, standard_source(text, marked, stem, classes)))
        if _stale(obj, depends, newest_header):
            compile_(['/c', *source_flags(flags), *includes, compiled, f'/Fo{obj}'])
        objects.append(obj)

    folder = os.path.join(src, 'stubs')
    if os.path.isdir(folder):
        for name in sorted(n for n in os.listdir(folder) if n.endswith('.cpp')):
            if units is not None and f'src/stubs/{name}' not in units:
                continue
            path = os.path.join(folder, name)
            obj = os.path.join(obj_dir, f'stubs_{os.path.splitext(name)[0]}.obj')
            if _stale(obj, [path], newest_header):
                # /O2 without /Gy (VC7.1 has no /Gy-): the stubs are not packaged as COMDATs, so the linker never folds identical bodies
                compile_(['/c', '/Og', '/Oi', '/Ot', '/Oy', '/Ob2', '/Gs', '/GF', '/Gr', *includes, path, f'/Fo{obj}'])
            objects.append(obj)

    # after the stubs: like them, built without LTCG (see standard_source)
    for stem, text in standard:
        generated, obj = os.path.join(gen, f'{stem}_standard.cpp'), os.path.join(obj_dir, standard_object(stem))
        if not os.path.exists(generated) or open(generated, encoding='utf-8').read() != text:
            with open(generated, 'w', encoding='utf-8') as f:
                f.write(text)
        if _stale(obj, [generated], newest_header):
            compile_(['/c', '/Og', '/Oi', '/Ot', '/Oy', '/Ob2', '/Gs', '/GF', '/Gr', *includes, generated,
                      f'/Fo{obj}'])
        objects.append(obj)

    ballast = os.path.join(gen, 'ltcg_ballast.cpp')
    ballast_text = ballast_source()
    ballast_obj = os.path.join(obj_dir, 'ltcg_ballast.obj')
    if not os.path.exists(ballast) or open(ballast, encoding='utf-8').read() != ballast_text:
        with open(ballast, 'w', encoding='utf-8') as f:
            f.write(ballast_text)
    if _stale(ballast_obj, [ballast], 0):
        compile_(['/c', '/GL', '/O2', '/Gr', f'/d2inlT{INLINE_THRESHOLD}', ballast, f'/Fo{ballast_obj}'])
    objects.append(ballast_obj)

    entry_name = 'entry' if units is None else f'entry_{REDUCED_NAME}'
    entry = os.path.join(gen, entry_name + '.cpp')
    entry_text = entry_source(entry_decls, entry_calls)
    entry_obj = os.path.join(obj_dir, entry_name + '.obj')
    if not os.path.exists(entry) or open(entry, encoding='utf-8').read() != entry_text:
        with open(entry, 'w', encoding='utf-8') as f:
            f.write(entry_text)
    if _stale(entry_obj, [entry], newest_header):
        compile_(['/c', '/GL', *SIZE_FLAGS, entry, f'/Fo{entry_obj}'])
    objects.append(entry_obj)
    compile_pending()

    if units is None:
        exe = image or os.path.join(out, EXE_NAME)
        response, force = os.path.join(out, 'link_objects.rsp'), []
    else:
        exe = image or os.path.join(out, REDUCED_NAME + '.img')
        response, force = os.path.join(out, f'link_{REDUCED_NAME}.rsp'), ['/FORCE:UNRESOLVED']
    map_path = os.path.splitext(exe)[0] + '.map'
    os.makedirs(os.path.dirname(exe), exist_ok=True)
    # the objects go in a response file: listed on the command line, their paths
    # outgrow Windows' 32K command-line limit
    with open(response, 'w', encoding='utf-8') as f:
        f.write(''.join(f'"{tool_arg(obj)}"\n' for obj in objects))
        f.write(''.join(f'/INCLUDE:{symbol}\n' for symbol in include))
    # the D3D members go after the program's objects and before the libraries (see D3D_OBJECTS)
    link = ['/LTCG', '/NODEFAULTLIB', '/ENTRY:entry', '/SUBSYSTEM:CONSOLE', '/MAP:' + map_path, '/MAPINFO:FIXUPS',
            '/FIXED:NO', *force, f'/OUT:{exe}', '@' + tool_arg(response), *d3d, *LIBRARIES]
    if reuse:
        stamp = exe + '.inputs'
        linked = objects + d3d
        inputs = ''.join(f'{line}\n' for line in [*link, *include, *(
            f'{obj} {stat.st_mtime_ns} {stat.st_size}' for obj, stat in zip(linked, map(os.stat, linked)))])
        if all(map(os.path.exists, (exe, map_path, stamp))):
            with open(stamp, encoding='utf-8') as f:
                if f.read() == inputs:
                    return map_path
            os.remove(stamp)
    started = time.perf_counter()
    run_tool('Link.Exe', link, root, xdk)
    timings['link'] = time.perf_counter() - started
    if reuse:
        with open(stamp, 'w', encoding='utf-8') as f:
            f.write(inputs)
    return map_path


if __name__ == '__main__':
    print(build())
