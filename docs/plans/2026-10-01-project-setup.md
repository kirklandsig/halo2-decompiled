# Project Set-up Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Turn the LTCG feasibility spike into a project that can match the retail Halo 2 XBE function by function: an inventory, a whole-game build, a checker, and a ready queue for workers.

**Architecture:** Plain Python tools under `tools/`, sharing two readers: `xbe.py` for the retail image, and `pe.py` with `linkmap.py` for the test image.
- `functions.py` finds every retail function.
- `libsig.py` recognises Xbox SDK library code by byte signature.
- `inventory.py` writes `config/functions.csv`.
- `build.py` compiles `src/` with per-file flags into one LTCG image, with generated stand-in callers.
- `check.py` compares that image with retail, function by function, using exact extents and exact address masks.
- `ready.py` lists the functions whose callees all match.

**Tech Stack:**
- Python 3.10+ with capstone 5.x and pytest.
- The Xbox SDK 5849 compiler (VC 7.1: `cl` 13.10.3077, `link` 7.10.3077), Windows only.

**Spec:** `docs/specs/2026-10-01-project-setup-design.md`

## Global Constraints

- **Never commit proprietary files.** That covers game files, XBEs, SDK files, Havok and Bink code, and leaked material. Retail lives in `orig/default.xbe` and the SDK in `sdk/xbox` (or `$XDK_DIR`); both are git-ignored.
- **The retail XBE's SHA-256 is fixed:** `03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d`. Tools that read retail check it and stop with a clear message on a mismatch.
- **Every compile uses the SDK's `bin/vc71` tools,** with `/GL /Gr` plus per-file flags. Every link uses `/LTCG`.
- **Credit:** code adapted from the Halo CE decompilation is CC0.
- **Tests must not contain retail or SDK bytes.**
  - Unit tests use hand-assembled snippets.
  - Integration tests are marked `retail` or `sdk`, and skip when those files are missing.
- **Environment on this machine** (Git Bash):
  - `export MSYS_NO_PATHCONV=1 XDK_DIR=<the SDK's xbox folder>`.
  - Run from the repository root.
  - Python is `python`.
- **Subagents do not commit or publish.** The coordinator commits after review and publishes with `bash local/publish.sh "<message>"`.

## Review Focus

1. **A different XBE** (another region or a title update) in `orig/`: the tools must stop with "not the retail XBE this project matches (sha256 …)", not produce a wrong inventory. *Task 4 owns `test_inventory_rejects_wrong_xbe`.*
2. **The SDK is missing, or `XDK_DIR` is wrong:** `build.py` must say which path it tried and how to set `XDK_DIR`, not show a Python traceback. *Task 5 owns `test_build_reports_missing_sdk`.*
3. **Two functions with the same plain name** (C++ overloads) in `src/`: `check.py` must report the ambiguity and name both decorations, not compare the wrong one. *Task 6 owns `test_resolve_symbol_ambiguous`.*
4. **Rerunning the inventory** after functions are matched must keep each row's `source` and `status`. *Task 4 owns `test_rows_keep_source_and_status`.*
5. **A function whose source is longer or shorter than retail's:** `check.py` must report DIFF and the first differing offset, not crash on unequal lengths. *Task 6 owns `test_compare_unequal_lengths`.*

---

### Task 1: Test harness and the test-image readers

**Files:**
- Create: `requirements-dev.txt`, `pytest.ini`, `tests/conftest.py`, `tools/linkmap.py`, `tools/pe.py`, `tests/test_linkmap.py`, `tests/test_pe.py`
- Modify: `tools/match.py` (use `pe.Pe` instead of its own class)

**Interfaces:**
- Produces:
  - `linkmap.LinkMap.read(path) -> LinkMap`, with:
    - `.base: int`
    - `.symbols: list[MapSymbol]`
    - `.rel_fixups: set[int]`
    - `.section_end(section: int) -> int`
    - `.extent(symbol) -> tuple[int, int]`
    - `.find(plain_name) -> list[MapSymbol]`
  - `MapSymbol(name: str, va: int, section: int, static: bool)`
  - `linkmap.plain_name(decorated: str) -> str`
  - `pe.Pe(path)`, with `.base`, `.sections` (`list[xbe.Section]`), `.fixups: set[int]` and `.read(va, size) -> bytes`
  - The test fixtures `retail_xbe` (a path) and `xdk_dir` (a path), which skip when the file or folder is missing.

- [ ] **Step 1: Dev dependencies and pytest config**

`requirements-dev.txt`:
```
capstone>=5,<6
pytest>=8
```
`pytest.ini`:
```ini
[pytest]
testpaths = tests
markers =
    retail: needs orig/default.xbe (the retail XBE)
    sdk: needs the Xbox SDK 5849 (sdk/xbox or XDK_DIR)
```
Run: `python -m pip install -r requirements-dev.txt`

- [ ] **Step 2: Write `tests/conftest.py`**

```python
import os
import sys

import pytest

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))


@pytest.fixture
def retail_xbe():
    path = os.environ.get('RETAIL_XBE', os.path.join(ROOT, 'orig', 'default.xbe'))
    if not os.path.exists(path):
        pytest.skip('retail XBE not present')
    return path


@pytest.fixture
def xdk_dir():
    path = os.environ.get('XDK_DIR', os.path.join(ROOT, 'sdk', 'xbox'))
    if not os.path.exists(os.path.join(path, 'bin', 'vc71', 'CL.Exe')):
        pytest.skip('Xbox SDK 5849 not present')
    return path
```

- [ ] **Step 3: Write the failing tests for `linkmap`**

`tests/test_linkmap.py`:
```python
from linkmap import LinkMap, plain_name

MAP = """ test

 Preferred load address is 00400000

 Start         Length     Name                   Class
 0001:00000000 00000139H .text                   CODE
 0002:00000000 00000511H .bss                    DATA

  Address         Publics by Value              Rva+Base     Lib:Object

 0000:00000000       ___safe_se_handler_table   00000000     <absolute>
 0001:00000000       ?function_163ba0@@YIXPAKPBXJ@Z 00401000 f   crc.obj
 0001:000000a0       @entry@0                   004010a0 f   crc_test.obj
 0002:00000008       ?g_buffer@@3PAEA           00402008     crc_test.obj

 entry point at        0000:00000000

 Static symbols

 0001:00000060       ?function_163c00@@YIXPAK@Z 00401060 f   crc.obj

FIXUPS: 1017 a4 2a
FIXUPS: 2000 fffffff0
"""


def test_symbols_and_static():
    m = LinkMap(MAP)
    assert m.base == 0x400000
    names = {s.name: (s.va, s.section, s.static) for s in m.symbols}
    assert names['?function_163ba0@@YIXPAKPBXJ@Z'] == (0x401000, 1, False)
    assert names['?function_163c00@@YIXPAK@Z'] == (0x401060, 1, True)
    assert '___safe_se_handler_table' not in names  # section 0 is absolute, not code


def test_fixups_each_line_starts_absolute_then_deltas():
    m = LinkMap(MAP)
    assert m.rel_fixups == {0x401017, 0x4010bb, 0x4010e5, 0x402000, 0x401ff0}


def test_extent_runs_to_next_symbol_or_section_end():
    m = LinkMap(MAP)
    crc = m.find('function_163ba0')[0]
    table = m.find('function_163c00')[0]
    entry = m.find('entry')[0]
    assert m.extent(crc) == (0x401000, 0x401060)
    assert m.extent(table) == (0x401060, 0x4010a0)
    assert m.extent(entry) == (0x4010a0, 0x401139)


def test_plain_name():
    assert plain_name('?function_163ba0@@YIXPAKPBXJ@Z') == 'function_163ba0'
    assert plain_name('?remove_all@c_world@@QAAXXZ') == 'c_world::remove_all'
    assert plain_name('@entry@0') == 'entry'
    assert plain_name('_strncmp') == 'strncmp'
    assert plain_name('_RtlSizeHeap@12') == 'RtlSizeHeap'
```

- [ ] **Step 4: Run them to see them fail**

Run: `python -m pytest tests/test_linkmap.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'linkmap'`

- [ ] **Step 5: Write `tools/linkmap.py`**

```python
"""Reads an MSVC linker map: its symbols (public and static), the extent of
each section, and the relative fields /MAPINFO:FIXUPS lists.

FIXUPS lines give one absolute RVA, then up to seven signed 32-bit deltas;
each field is the previous one plus the delta.
"""
import re
from dataclasses import dataclass

SYMBOL = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\b')
SECTION = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+([0-9a-f]{8})H\s+\S+\s+\S+\s*$')
BASE = re.compile(r'Preferred load address is ([0-9a-f]{8})')


@dataclass(frozen=True)
class MapSymbol:
    name: str
    va: int
    section: int
    static: bool


def plain_name(decorated):
    """A decorated name without its decoration: 'name' or 'class::name'."""
    if decorated.startswith('?'):
        parts = decorated[1:].split('@@', 1)[0].split('@')
        return '::'.join(reversed(parts))
    name = decorated.lstrip('_@')
    return name.split('@', 1)[0]


class LinkMap:
    def __init__(self, text):
        self.base = 0
        self.symbols = []
        self.rel_fixups = set()
        lengths = {}  # section -> end offset
        static = False
        for line in text.splitlines():
            m = BASE.search(line)
            if m:
                self.base = int(m.group(1), 16)
                continue
            if line.strip() == 'Static symbols':
                static = True
                continue
            if line.startswith('FIXUPS:'):
                values = [int(v, 16) for v in line.split()[1:]]
                rva = values[0]
                self.rel_fixups.add(self.base + rva)
                for delta in values[1:]:
                    rva = (rva + delta) & 0xFFFFFFFF
                    self.rel_fixups.add(self.base + rva)
                continue
            m = SECTION.match(line)
            if m:
                section, start, length = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16)
                lengths[section] = max(lengths.get(section, 0), start + length)
                continue
            m = SYMBOL.match(line)
            if m and int(m.group(1), 16) != 0:
                self.symbols.append(MapSymbol(m.group(3), int(m.group(4), 16), int(m.group(1), 16), static))
        # each section's start, from any symbol in it (va - offset)
        starts = {}
        for line in text.splitlines():
            m = SYMBOL.match(line)
            if m and int(m.group(1), 16) != 0:
                starts.setdefault(int(m.group(1), 16), int(m.group(4), 16) - int(m.group(2), 16))
        self._ends = {s: starts[s] + lengths[s] for s in starts if s in lengths}
        self._sorted = sorted(self.symbols, key=lambda s: (s.section, s.va))

    @classmethod
    def read(cls, path):
        with open(path, encoding='latin-1') as f:
            return cls(f.read())

    def section_end(self, section):
        return self._ends[section]

    def extent(self, symbol):
        """[start, end): up to the next symbol in its section, or the section's end."""
        later = [s.va for s in self._sorted if s.section == symbol.section and s.va > symbol.va]
        return symbol.va, min(later) if later else self.section_end(symbol.section)

    def find(self, plain):
        return [s for s in self.symbols if plain_name(s.name) == plain]
```

- [ ] **Step 6: Run the tests to see them pass**

Run: `python -m pytest tests/test_linkmap.py -v`
Expected: 4 passed

- [ ] **Step 7: Move `Pe` into `tools/pe.py`, with a failing integration test first**

`tests/test_pe.py` builds the spike's crc test with the SDK and checks that `Pe` finds base relocations:
```python
import os
import subprocess
import sys

import pytest

from pe import Pe

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


@pytest.mark.sdk
def test_pe_reads_base_relocations(xdk_dir, tmp_path):
    vc = os.path.join(xdk_dir, 'bin', 'vc71')
    src = os.path.join(ROOT, 'spike')
    for name in ('crc', 'crc_test'):
        subprocess.run([os.path.join(vc, 'CL.Exe'), '/nologo', '/c', '/GL', '/O2', '/Gr',
                        os.path.join(src, name + '.cpp'), f'/Fo{tmp_path / name}.obj'], check=True)
    subprocess.run([os.path.join(vc, 'Link.Exe'), '/nologo', '/LTCG', '/NODEFAULTLIB', '/ENTRY:entry',
                    '/SUBSYSTEM:CONSOLE', '/FIXED:NO', f'/OUT:{tmp_path / "t.exe"}',
                    str(tmp_path / 'crc.obj'), str(tmp_path / 'crc_test.obj')], check=True)
    image = Pe(str(tmp_path / 't.exe'))
    assert image.base == 0x400000
    assert image.fixups, 'a /FIXED:NO image has base relocations'
    assert all(image.base <= f < image.base + 0x100000 for f in image.fixups)
    assert len(image.read(image.base + 0x1000, 16)) == 16
```
Run: `python -m pytest tests/test_pe.py -v` → FAIL (`No module named 'pe'`).

Then create `tools/pe.py` with the `Pe` class exactly as it is in `tools/match.py` today (header parse, `Section('', base + va, vs, ra, rs, 0)`, base relocation parse into `self.fixups`, `read = Xbe.read`). Its docstring: `"""Reads a linked test image (PE): sections and base relocations."""`. In `tools/match.py`, delete the class and add `from pe import Pe`.

Run: `python -m pytest tests -v` → all pass (the sdk test runs on this machine).
Run: `python tools/match.py "/O2 /Gr" spike/crc.cpp spike/crc_test.cpp -- "?function_163c00@@YIXPAK@Z=163c00" "?function_163ba0@@YIXPAKPBXJ@Z=163ba0"` → `2/2 match`.

- [ ] **Step 8: Hand back.** Report the files changed and the test output. Do not commit.

---

### Task 2: Function discovery (`tools/functions.py`)

**Files:**
- Create: `tools/functions.py`, `tests/test_functions.py`

**Interfaces:**
- Consumes: an image object with `.sections` (`xbe.Section` list), `.section(name)`, `.section_bytes(section)` and `.entry` (as `xbe.Xbe` has).
- Produces:
  - `functions.Function(start: int, end: int, calls: set[int], tail_jumps: set[int], tables: list[tuple[int, int, int]])`. A table is `(va, entry_size, count)`; `end` is exclusive.
  - `functions.discover(image, seeds=(), text='.text') -> dict[int, Function]`
  - `functions.is_filler(ins) -> bool`

- [ ] **Step 1: Write the failing unit tests with hand-assembled code**

`tests/test_functions.py`:
```python
from dataclasses import dataclass

import pytest

from functions import discover
from xbe import Section, Xbe


@dataclass
class FakeImage:
    """A one-section image: code at 0x1000, optional data section."""
    code: bytes
    data: bytes = b''
    entry: int = 0x1000

    def __post_init__(self):
        self.sections = [Section('.text', 0x1000, len(self.code), 0, len(self.code), 0)]
        if self.data:
            self.sections.append(Section('.data', 0x8000, len(self.data), len(self.code), len(self.data), 0))
        self._bytes = self.code + self.data

    def section(self, name):
        return next(s for s in self.sections if s.name == name)

    def section_bytes(self, s):
        return self._bytes[s.raw:s.raw + s.rsize]


def pad(code, to):
    return code + b'\xcc' * (to - len(code))


def test_call_and_padding():
    # 0x1000: call 0x1010 ; ret ; int3 padding ; 0x1010: xor eax, eax ; ret
    code = pad(bytes.fromhex('e80b000000c3'), 0x10) + bytes.fromhex('33c0c3')
    found = discover(FakeImage(code))
    assert sorted(found) == [0x1000, 0x1010]
    assert found[0x1000].end == 0x1006 and found[0x1000].calls == {0x1010}
    assert found[0x1010].end == 0x1013


def test_jump_table_bounded_by_cmp():
    code = bytes.fromhex(
        '83f802'          # 1000 cmp eax, 2
        '7713'            # 1003 ja 1018
        'ff24851c100000'  # 1005 jmp [eax*4 + 0x101c]
        'b801000000c3'    # 100c case 0
        'b802000000c3'    # 1012 case 1
        '33c0c3'          # 1018 default
        '90'              # 101b alignment
        '0c100000' '12100000' '18100000')  # 101c table, 3 entries
    found = discover(FakeImage(code))
    assert list(found) == [0x1000]
    assert found[0x1000].end == 0x1028
    assert found[0x1000].tables == [(0x101c, 4, 3)]


def test_tail_jump_to_known_start():
    # 0x1000: jmp 0x1010 (another function, seeded) ; 0x1010: ret
    code = pad(bytes.fromhex('e90b000000'), 0x10) + bytes.fromhex('c3')
    found = discover(FakeImage(code), seeds=[0x1010])
    assert found[0x1000].end == 0x1005 and found[0x1000].tail_jumps == {0x1010}
    assert 0x1010 in found


def test_data_pointer_finds_callback():
    # 0x1000: ret ; padding ; 0x1010: callback only reached through .data
    code = pad(bytes.fromhex('c3'), 0x10) + bytes.fromhex('b801000000c3')
    found = discover(FakeImage(code, data=(0x1010).to_bytes(4, 'little')))
    assert 0x1010 in found and found[0x1010].end == 0x1016


def test_gap_after_packed_functions_becomes_a_function():
    # two packed functions, the second reached by nothing: ret ; mov eax,1 ; ret
    code = bytes.fromhex('c3b801000000c3')
    found = discover(FakeImage(code))
    assert sorted(found) == [0x1000, 0x1001]


@pytest.mark.retail
def test_retail_known_functions(retail_xbe):
    found = discover(Xbe(retail_xbe))
    assert found[0x163ba0].end == 0x163bf4      # function_163ba0, ends with ret 4
    assert found[0x163c00].end == 0x163c35      # function_163c00
    assert found[0x163ba0].calls == {0x163c00}
    assert found[0x1782a0].tail_jumps == {0x17add0}
    assert len(found) > 9954                    # more than the direct call targets alone
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_functions.py -v` → FAIL (`No module named 'functions'`).

- [ ] **Step 3: Write `tools/functions.py`**

```python
"""Finds the functions of an XBE's code: where each starts and ends, what it
calls, and the jump tables inside it.

Starting points are the entry point, direct call targets, seeds (e.g. the
rows of an existing inventory), and pointers into the code from data and from
code immediates that land on a function boundary. Each is disassembled recursively. Code that
nothing reaches is picked up from the gaps between functions.

    python tools/functions.py <default.xbe>      print a summary
"""
import struct
import sys
from dataclasses import dataclass, field

from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from capstone import x86

MAX_TABLE = 512


@dataclass
class Function:
    start: int
    end: int
    calls: set = field(default_factory=set)
    tail_jumps: set = field(default_factory=set)
    tables: list = field(default_factory=list)


def is_filler(ins):
    """Alignment filler: nop, int3, mov r, r, or lea r, [r + 0]."""
    if ins.mnemonic in ('nop', 'int3'):
        return True
    if len(ins.operands) != 2:
        return False
    a, b = ins.operands
    if ins.mnemonic == 'mov':
        return a.type == b.type == x86.X86_OP_REG and a.reg == b.reg
    if ins.mnemonic == 'lea':
        return (a.type == x86.X86_OP_REG and b.type == x86.X86_OP_MEM and b.mem.base == a.reg
                and b.mem.index == 0 and b.mem.disp == 0)
    return False


class _Code:
    def __init__(self, image, text):
        self.section = image.section(text)
        self.bytes = image.section_bytes(self.section)
        self.lo = self.section.va
        self.hi = self.section.va + len(self.bytes)
        self.md = Cs(CS_ARCH_X86, CS_MODE_32)
        self.md.detail = True

    def inside(self, va):
        return self.lo <= va < self.hi

    def at(self, va):
        """The instruction at va, or None."""
        o = va - self.lo
        return next(self.md.disasm(self.bytes[o:o + 16], va), None)

    def dword(self, va):
        o = va - self.lo
        return struct.unpack_from('<I', self.bytes, o)[0] if 0 <= o <= len(self.bytes) - 4 else None

    def byte(self, va):
        o = va - self.lo
        return self.bytes[o] if 0 <= o < len(self.bytes) else None

    def boundary(self, va):
        """Whether va looks like a function start: the start of the code, after
        int3 padding or a ret, or aligned to 16."""
        if va == self.lo or va % 16 == 0:
            return True
        o = va - self.lo
        before = self.bytes[max(0, o - 3):o]
        return before[-1:] in (b'\xcc', b'\xc3') or (len(before) == 3 and before[0] == 0xC2)


def _table_count(code, block, jump):
    """How many entries the jump table of `jump` has, from the bound check
    before it. Returns (count, byte_table) where byte_table is (va, count) for
    MSVC's two-level switches, or None."""
    bound, byte_table = None, None
    for ins in reversed(block):
        if (ins.mnemonic == 'movzx' and ins.operands[1].type == x86.X86_OP_MEM
                and ins.operands[1].size == 1 and code.inside(ins.operands[1].mem.disp)):
            byte_table = ins.operands[1].mem.disp
        if ins.mnemonic == 'cmp' and ins.operands[1].type == x86.X86_OP_IMM:
            bound = ins.operands[1].imm + 1
            break
    if bound is None or bound > MAX_TABLE:
        return None, None
    if byte_table is not None:
        indices = [code.byte(byte_table + k) for k in range(bound)]
        return max(indices) + 1, (byte_table, bound)
    return bound, None


def _trace(code, start, starts):
    """Recursive descent from start. Returns the Function."""
    fn = Function(start, start)
    seen, work = set(), [start]
    while work:
        va = work.pop()
        block = []
        while code.inside(va) and va not in seen:
            ins = code.at(va)
            if ins is None:
                break
            seen.add(va)
            block.append(ins)
            fn.end = max(fn.end, va + ins.size)
            m = ins.mnemonic
            op = ins.operands[0] if ins.operands else None
            if m == 'call' and op.type == x86.X86_OP_IMM:
                fn.calls.add(op.imm)
            elif m == 'jmp':
                if op.type == x86.X86_OP_IMM:
                    if op.imm in starts and op.imm != start:
                        fn.tail_jumps.add(op.imm)
                    elif code.inside(op.imm):
                        work.append(op.imm)
                elif (op.type == x86.X86_OP_MEM and op.mem.base == 0 and op.mem.index != 0
                      and op.mem.scale == 4 and code.inside(op.mem.disp)):
                    count, byte_table = _table_count(code, block, ins)
                    if count:
                        table = op.mem.disp
                        fn.tables.append((table, 4, count))
                        fn.end = max(fn.end, table + 4 * count)
                        if byte_table:
                            fn.tables.append((byte_table[0], 1, byte_table[1]))
                            fn.end = max(fn.end, byte_table[0] + byte_table[1])
                        for k in range(count):
                            target = code.dword(table + 4 * k)
                            if target is not None and code.inside(target):
                                work.append(target)
                break
            elif m.startswith('j') or m.startswith('loop'):
                if op.type == x86.X86_OP_IMM and code.inside(op.imm):
                    work.append(op.imm)
            elif m in ('ret', 'int3', 'hlt'):
                break
            va += ins.size
    fn.tables.sort()
    return fn


def _pointer_starts(image, code):
    """Pointers into the code from data sections, at function boundaries."""
    found = set()
    for s in image.sections:
        if s.name == code.section.name:
            continue
        data = image.section_bytes(s)
        for o in range(0, len(data) - 3, 4):
            v = struct.unpack_from('<I', data, o)[0]
            if code.inside(v) and code.boundary(v):
                found.add(v)
    return found


def _sweep_starts(code):
    """Direct call targets, and code immediates (push/mov imm) at boundaries."""
    found = set()
    lite = Cs(CS_ARCH_X86, CS_MODE_32)
    lite.skipdata = True
    for _, _, mnemonic, op_str in lite.disasm_lite(code.bytes, code.lo):
        if mnemonic == 'call' and op_str.startswith('0x'):
            v = int(op_str, 16)
            if code.inside(v):
                found.add(v)
        elif mnemonic in ('push', 'mov') and op_str.split(', ')[-1].startswith('0x'):
            v = int(op_str.split(', ')[-1], 16)
            if code.inside(v) and code.boundary(v) and v % 16 == 0:
                found.add(v)
    return found


def _gaps(code, functions):
    """Starts of code that no function covers, after skipping filler."""
    spans = sorted((f.start, f.end) for f in functions.values())
    found, cursor = set(), code.lo
    for start, end in spans + [(code.hi, code.hi)]:
        va = cursor
        while va < start:
            ins = code.at(va)
            if ins is None:
                va += 1
                continue
            if is_filler(ins):
                va += ins.size
                continue
            found.add(va)
            break
        cursor = max(cursor, end)
    return found


def discover(image, seeds=(), text='.text'):
    code = _Code(image, text)
    starts = {image.entry} if code.inside(image.entry) else set()
    starts |= {s for s in seeds if code.inside(s)}
    starts |= _sweep_starts(code) | _pointer_starts(image, code)
    functions = {}
    pending = set(starts)
    while pending:
        for start in sorted(pending):
            functions[start] = _trace(code, start, starts)
        pending = _gaps(code, functions) - set(functions)
        starts |= pending
    return dict(sorted(functions.items()))


def main():
    from xbe import Xbe
    found = discover(Xbe(sys.argv[1]))
    total = sum(f.end - f.start for f in found.values())
    tables = sum(len(f.tables) for f in found.values())
    print(f'{len(found)} functions, {total} bytes, {tables} jump tables')


if __name__ == '__main__':
    main()
```

- [ ] **Step 4: Run the unit tests until they pass**

Run: `python -m pytest tests/test_functions.py -v -m "not retail"`
Expected: 5 passed. If `test_jump_table_bounded_by_cmp` fails, print `found[0x1000]` and check `_table_count` against the hand-assembled bytes in the test comments.

- [ ] **Step 5: Run the retail test, and check the retail numbers**

Run: `python -m pytest tests/test_functions.py -v` and `python tools/functions.py orig/default.xbe`
Expected:
- the retail test passes;
- the summary prints more than 9,954 functions and finds jump tables;
- the run takes under 5 minutes.

If a retail assertion fails, disassemble the address with `python -c "..."` (as in `tools/match.py`), and fix the rule at fault, not the test. Record any retail-specific rule you added in a comment.

- [ ] **Step 6: Hand back.** Report the files, the test output, the summary line, and the run time. Do not commit.

---

### Task 3: SDK library signatures (`tools/libsig.py`)

**Files:**
- Create: `tools/libsig.py`, `tests/test_libsig.py`

**Interfaces:**
- Produces:
  - `libsig.Signature(library: str, member: str, name: str, code: bytes, mask: bytes)`. In `mask`, 1 means compare and 0 means ignore.
  - `libsig.archive_members(data: bytes) -> list[tuple[str, bytes]]`
  - `libsig.object_signatures(obj: bytes, library='', member='') -> list[Signature]`
  - `libsig.library_signatures(path: str) -> list[Signature]`
  - `libsig.find(signatures, text: bytes, text_va: int) -> dict[int, Signature]`, which keeps unique hits only.
  - `libsig.COFF_LIBRARIES = ('libcmt', 'libcpmt', 'xapilib', 'dsound', 'xonlines', 'xvoice', 'xnet', 'd3dx8')`

- [ ] **Step 1: Write the failing tests**

`tests/test_libsig.py`:
```python
import os
import struct

import pytest

from libsig import Signature, find, library_signatures, object_signatures


def coff(code, relocs, symbols):
    """A minimal i386 COFF object: one .text section.
    relocs: [(offset, type)], symbols: [(name, offset)] (external functions)."""
    nsyms = len(symbols)
    header_size, section_size = 20, 40
    raw = header_size + section_size
    rel = raw + len(code)
    symtab = rel + 10 * len(relocs)
    head = struct.pack('<HHIIIHH', 0x14C, 1, 0, symtab, nsyms, 0, 0)
    sect = struct.pack('<8sIIIIIIHHI', b'.text', 0, 0, len(code), raw, rel, 0, len(relocs), 0, 0x60000020)
    body = code + b''.join(struct.pack('<IIH', off, 0, kind) for off, kind in relocs)
    syms = b''.join(struct.pack('<8sIhHBB', n.encode().ljust(8, b'\0'), off, 1, 0x20, 2, 0) for n, off in symbols)
    return head + sect + body + syms + struct.pack('<I', 4)


def test_object_signature_masks_relocations():
    code = bytes.fromhex('55 8bec a1 00000000 5d c3'.replace(' ', ''))
    sigs = object_signatures(coff(code, [(4, 0x06)], [('_f', 0)]))
    assert len(sigs) == 1 and sigs[0].name == '_f'
    assert sigs[0].mask == bytes([1, 1, 1, 1, 0, 0, 0, 0, 1, 1])


def test_find_unique_hit_with_different_address():
    code = bytes.fromhex('558beca100000000a1000000005dc3')
    sig = object_signatures(coff(code, [(4, 0x06), (9, 0x06)], [('_g', 0)]))[0]
    text = b'\xcc' * 32 + bytes.fromhex('558beca178563412a1ddccbbaa5dc3') + b'\xcc' * 8
    assert find([sig], text, 0x10000) == {0x10020: sig}


def test_find_ignores_ambiguous():
    sig = Signature('lib', 'm', '_h', bytes.fromhex('8b442404c3909090'), b'\1' * 8)
    text = sig.code + sig.code
    assert find([sig], text, 0x10000) == {}


@pytest.mark.sdk
@pytest.mark.retail
def test_retail_libcmt(xdk_dir, retail_xbe):
    from xbe import Xbe
    image = Xbe(retail_xbe)
    text = image.section('.text')
    hits = find(library_signatures(os.path.join(xdk_dir, 'lib', 'libcmt.lib')),
                image.section_bytes(text), text.va)
    names = {va: s.name for va, s in hits.items()}
    assert names.get(0x321340) == '_strncmp'
    assert names.get(0x320bd0) == '__allmul'
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_libsig.py -v` → FAIL (`No module named 'libsig'`).

- [ ] **Step 3: Write `tools/libsig.py`**

```python
"""Signatures of Xbox SDK library functions, for finding them in the retail
XBE. Each function's bytes come from its library object, with the fields its
relocations fill in masked, so a match ignores addresses.

Only libraries of ordinary objects work this way. D3D8 and XGRAPHICS ship as
LTCG intermediate code and are found by section instead.

    python tools/libsig.py <xdk lib dir> <default.xbe>   count the hits
"""
import os
import struct
import sys
from dataclasses import dataclass

COFF_LIBRARIES = ('libcmt', 'libcpmt', 'xapilib', 'dsound', 'xonlines', 'xvoice', 'xnet', 'd3dx8')
MIN_COMPARED = 6  # shorter signatures match too much by chance
CODE = 0x20  # IMAGE_SCN_CNT_CODE
MASKED = {0x06: 4, 0x07: 4, 0x0A: 2, 0x0B: 4, 0x14: 4}  # DIR32, DIR32NB, SECTION, SECREL, REL32


@dataclass(frozen=True)
class Signature:
    library: str
    member: str
    name: str
    code: bytes
    mask: bytes


def archive_members(data):
    """(name, bytes) for each object in a COFF archive (.lib)."""
    if data[:8] != b'!<arch>\n':
        raise ValueError('not a COFF archive')
    members, longnames, o = [], b'', 8
    while o + 60 <= len(data):
        name = data[o:o + 16].decode('latin-1').rstrip()
        size = int(data[o + 48:o + 58].decode().strip())
        body = data[o + 60:o + 60 + size]
        if name == '//':
            longnames = body
        elif name != '/':
            if name.startswith('/') and name[1:].isdigit():
                k = int(name[1:])
                name = longnames[k:longnames.index(b'\0', k)].decode('latin-1')
            members.append((name.rstrip('/'), body))
        o += 60 + size + (size & 1)
    return members


def object_signatures(obj, library='', member=''):
    machine, nsec, _, symtab, nsyms, optsize, _ = struct.unpack_from('<HHIIIHH', obj, 0)
    if machine != 0x14C:
        return []  # import stubs and LTCG intermediate code
    strings = symtab + 18 * nsyms
    sections = []
    for i in range(nsec):
        o = 20 + optsize + 40 * i
        _, _, _, rsize, rptr, relptr, _, nrel, _, chars = struct.unpack_from('<8sIIIIIIHHI', obj, o)
        sections.append((rsize, rptr, relptr, nrel, chars))

    names = {}  # section index -> [(offset, name)]
    k = 0
    while k < nsyms:
        o = symtab + 18 * k
        raw, value, section, kind, storage, aux = struct.unpack_from('<8sIhHBB', obj, o)
        if raw[:4] == b'\0\0\0\0':
            start = strings + struct.unpack_from('<I', raw, 4)[0]
            name = obj[start:obj.index(b'\0', start)].decode('latin-1')
        else:
            name = raw.rstrip(b'\0').decode('latin-1')
        if section > 0 and (storage == 2 or (storage == 3 and kind == 0x20 and aux == 0)):
            names.setdefault(section - 1, []).append((value, name))
        k += 1 + aux

    signatures = []
    for index, offsets in names.items():
        rsize, rptr, relptr, nrel, chars = sections[index]
        if not chars & CODE or rptr == 0:
            continue
        code = obj[rptr:rptr + rsize]
        mask = bytearray(b'\1' * rsize)
        for r in range(nrel):
            at, _, kind = struct.unpack_from('<IIH', obj, relptr + 10 * r)
            width = MASKED.get(kind, 0)
            mask[at:at + width] = bytes(width)
        offsets = sorted(set(offsets))
        for n, (start, name) in enumerate(offsets):
            end = offsets[n + 1][0] if n + 1 < len(offsets) else rsize
            body, bits = code[start:end], bytes(mask[start:end])
            while body and body[-1] in (0xCC, 0x90):  # trailing alignment fill
                body, bits = body[:-1], bits[:-1]
            if sum(bits) >= MIN_COMPARED:
                signatures.append(Signature(library, member, name, body, bits))
    return signatures


def library_signatures(path):
    library = os.path.splitext(os.path.basename(path))[0].lower()
    with open(path, 'rb') as f:
        members = archive_members(f.read())
    return [s for member, body in members for s in object_signatures(body, library, member)]


def _anchor(sig):
    """The longest run of compared bytes: (offset, bytes)."""
    best, start = (0, b''), None
    for i, bit in enumerate(sig.mask + b'\0'):
        if bit and start is None:
            start = i
        elif not bit and start is not None:
            if i - start > len(best[1]):
                best = (start, sig.code[start:i])
            start = None
    return best


def _equal(sig, text, at):
    window = text[at:at + len(sig.code)]
    return len(window) == len(sig.code) and all(
        not m or a == b for a, b, m in zip(sig.code, window, sig.mask))


def find(signatures, text, text_va):
    """{va: signature} for the signatures found exactly once in text."""
    hits = {}
    for sig in signatures:
        offset, anchor = _anchor(sig)
        if len(anchor) < 4:
            continue
        places, pos = [], text.find(anchor)
        while pos >= 0 and len(places) < 2:
            at = pos - offset
            if at >= 0 and _equal(sig, text, at):
                places.append(at)
            pos = text.find(anchor, pos + 1)
        if len(places) == 1:
            hits[text_va + places[0]] = sig
    return hits


def main():
    from xbe import Xbe
    lib_dir, image = sys.argv[1], Xbe(sys.argv[2])
    text = image.section('.text')
    code = image.section_bytes(text)
    for library in COFF_LIBRARIES:
        path = os.path.join(lib_dir, library + '.lib')
        if os.path.exists(path):
            print(f'{library:<10} {len(find(library_signatures(path), code, text.va))} functions')


if __name__ == '__main__':
    main()
```

- [ ] **Step 4: Run the unit tests**

Run: `python -m pytest tests/test_libsig.py -v -m "not retail"`
Expected: 3 passed

- [ ] **Step 5: Run the retail test and the summary**

Run: `python -m pytest tests/test_libsig.py -v` and `python tools/libsig.py "$XDK_DIR/lib" orig/default.xbe`
Expected:
- the retail test passes;
- libcmt finds well over 100 functions;
- dsound, xonlines and xvoice may find few, since retail used later releases (QFEs) of them.

Report the counts.

- [ ] **Step 6: Hand back.** Report the files, the test output and the counts. Do not commit.

---

### Task 4: The inventory (`tools/inventory.py` → `config/functions.csv`)

**Files:**
- Create: `tools/inventory.py`, `tests/test_inventory.py`, `config/functions.csv` (generated)
- Modify: `docs/specs/2026-10-01-project-setup-design.md`: the CSV columns now include `evidence` and `calls`

**Interfaces:**
- Consumes:
  - `functions.discover`, `functions.Function`, `functions.is_filler`
  - `libsig.library_signatures`, `libsig.find`, `libsig.COFF_LIBRARIES`
  - `xbe.Xbe`
- Produces:
  - `inventory.RETAIL_SHA256` (str)
  - `inventory.COLUMNS = ['va', 'size', 'owner', 'style', 'evidence', 'name', 'calls', 'source', 'status']`
  - `inventory.check_retail(path)`, which raises `SystemExit` with a message on a wrong hash
  - `inventory.style(code_bytes_before: bytes, fn, first_instructions) -> tuple[str, str]`, returning `(style, evidence)`
  - `inventory.owner(section_name, lib_hit) -> str`
  - `inventory.CODE_SECTIONS`, a set of section names
  - `inventory.read_rows(path) -> dict[int, dict]` and `inventory.write_rows(path, rows)`
  - `inventory.merge(new_rows, old_rows) -> list[dict]`, which keeps `source` and `status`

- [ ] **Step 1: Write the failing tests**

`tests/test_inventory.py`:
```python
import hashlib

import pytest

from inventory import COLUMNS, check_retail, merge, owner, read_rows, write_rows


def test_inventory_rejects_wrong_xbe(tmp_path):
    fake = tmp_path / 'default.xbe'
    fake.write_bytes(b'XBEH' + bytes(100))
    with pytest.raises(SystemExit) as e:
        check_retail(str(fake))
    assert 'not the retail XBE this project matches' in str(e.value)
    assert hashlib.sha256(fake.read_bytes()).hexdigest() in str(e.value)


def test_rows_keep_source_and_status(tmp_path):
    old = {0x163ba0: dict(va='00163ba0', size='84', owner='game', style='speed', evidence='a16 pad',
                          name='x', calls='', source='src/crc.cpp', status='matched')}
    new = [dict(va='00163ba0', size='84', owner='game', style='speed', evidence='a16 pad',
                name='?function_163ba0@@YIXPAKPBXJ@Z', calls='00163c00', source='', status='todo')]
    rows = merge(new, old)
    assert rows[0]['source'] == 'src/crc.cpp' and rows[0]['status'] == 'matched'
    assert rows[0]['name'] == '?function_163ba0@@YIXPAKPBXJ@Z'
    path = tmp_path / 'f.csv'
    write_rows(str(path), rows)
    assert read_rows(str(path))[0x163ba0]['status'] == 'matched'
    assert path.read_text().splitlines()[0] == ','.join(COLUMNS)


def test_owner_rules():
    assert owner('D3D', None) == 'xdk:d3d8'
    assert owner('BINK', None) == 'third:bink'
    assert owner('.text', 'libcmt') == 'xdk:libcmt'
    assert owner('.text', None) == 'game'
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_inventory.py -v` → FAIL (`No module named 'inventory'`).

- [ ] **Step 3: Write `tools/inventory.py`**

```python
"""Writes config/functions.csv: every function in the retail XBE's code, with
who owns it (game, an Xbox SDK library, or third-party code), how it was
compiled (for speed or for size), its library name where known, and what it
calls. Rerunning keeps each row's source and status.

    python tools/inventory.py [--xbe orig/default.xbe] [--xdk sdk/xbox] [--out config/functions.csv]
"""
import argparse
import csv
import hashlib
import os
import sys

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import libsig
from functions import discover, is_filler
from xbe import Xbe

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RETAIL_SHA256 = '03215919bb7163259257d361f4c7bf802a7ab12aa85e2689436369b5c427935d'
COLUMNS = ['va', 'size', 'owner', 'style', 'evidence', 'name', 'calls', 'source', 'status']
# code sections (XBE section flags mark data sections executable too, so go by name)
CODE_SECTIONS = {'.text', 'D3D', 'XPP', 'DSOUND', 'WMADEC', 'XONLINE', 'XNET'}
SECTION_OWNERS = {'D3D': 'xdk:d3d8', 'XPP': 'xdk:xapi', 'DSOUND': 'xdk:dsound', 'WMADEC': 'xdk:wmadec',
                  'XONLINE': 'xdk:xonline', 'XNET': 'xdk:xnet'}


def check_retail(path):
    with open(path, 'rb') as f:
        digest = hashlib.sha256(f.read()).hexdigest()
    if digest != RETAIL_SHA256:
        raise SystemExit(f'{path} is not the retail XBE this project matches '
                         f'(sha256 {digest}, expected {RETAIL_SHA256})')


def owner(section, lib_hit):
    if section.startswith('BINK'):
        return 'third:bink'
    if section != '.text':
        return SECTION_OWNERS.get(section, 'xdk:' + section.lower())
    if lib_hit:
        return 'xdk:' + lib_hit
    return 'game'


def style(before, fn, first):
    """('speed' | 'size' | 'unknown', evidence). before: the bytes just before
    the function; first: its first two instructions."""
    evidence = []
    if fn.start % 16 == 0:
        evidence.append('a16')
    if before[-1:] == b'\xcc':
        evidence.append('pad')
    if (len(first) >= 2 and first[0].mnemonic == 'push' and first[0].op_str == 'ebp'
            and first[1].mnemonic == 'mov' and first[1].op_str == 'ebp, esp'):
        evidence.append('ebp')
    if 'a16' not in evidence and 'pad' not in evidence:
        evidence.append('packed')
    if 'a16' in evidence and 'pad' in evidence:
        kind = 'speed'
    elif 'packed' in evidence:
        kind = 'size'
    else:
        kind = 'unknown'
    return kind, ' '.join(evidence)


def read_rows(path):
    if not os.path.exists(path):
        return {}
    with open(path, newline='', encoding='utf-8') as f:
        return {int(r['va'], 16): r for r in csv.DictReader(f)}


def write_rows(path, rows):
    os.makedirs(os.path.dirname(path) or '.', exist_ok=True)
    with open(path, 'w', newline='', encoding='utf-8') as f:
        w = csv.DictWriter(f, COLUMNS, lineterminator='\n')
        w.writeheader()
        w.writerows(rows)


def merge(new_rows, old_rows):
    for row in new_rows:
        old = old_rows.get(int(row['va'], 16))
        if old:
            row['source'], row['status'] = old['source'], old['status']
    return new_rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('--xbe', default=os.path.join(ROOT, 'orig', 'default.xbe'))
    ap.add_argument('--xdk', default=os.environ.get('XDK_DIR', os.path.join(ROOT, 'sdk', 'xbox')))
    ap.add_argument('--out', default=os.path.join(ROOT, 'config', 'functions.csv'))
    args = ap.parse_args()

    check_retail(args.xbe)
    image = Xbe(args.xbe)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True

    rows = []
    for section in image.sections:
        if section.name not in CODE_SECTIONS and not (section.name.startswith('BINK') and section.name != 'BINKDATA'):
            continue
        found = discover(image, text=section.name)
        lib_hits = {}
        if section.name == '.text':
            code = image.section_bytes(section)
            for library in libsig.COFF_LIBRARIES:
                path = os.path.join(args.xdk, 'lib', library + '.lib')
                if os.path.exists(path):
                    for va, sig in libsig.find(libsig.library_signatures(path), code, section.va).items():
                        lib_hits.setdefault(va, sig)
        for fn in found.values():
            before = image.read(fn.start - 1, 1) if fn.start > section.va else b''
            first = list(md.disasm(image.read(fn.start, 16), fn.start, 2))
            kind, evidence = style(before, fn, first)
            sig = lib_hits.get(fn.start)
            rows.append(dict(
                va=f'{fn.start:08x}', size=str(fn.end - fn.start),
                owner=owner(section.name, sig and sig.library),
                style=kind, evidence=evidence, name=sig.name if sig else '',
                calls=' '.join(f'{c:08x}' for c in sorted(fn.calls | fn.tail_jumps)),
                source='', status='todo'))
    rows.sort(key=lambda r: r['va'])
    write_rows(args.out, merge(rows, read_rows(args.out)))
    counts = {}
    for r in rows:
        counts[r['owner']] = counts.get(r['owner'], 0) + 1
    print(f'{len(rows)} functions:', ', '.join(f'{k} {v}' for k, v in sorted(counts.items())))


if __name__ == '__main__':
    main()
```

- [ ] **Step 4: Run the unit tests until they pass**

Run: `python -m pytest tests/test_inventory.py -v`
Expected: 3 passed

- [ ] **Step 5: Generate the inventory and check it against known facts**

Run: `python tools/inventory.py`

Then check:
```
python -c "import sys; sys.path.insert(0,'tools'); from inventory import read_rows; r=read_rows('config/functions.csv'); [print(hex(v), r[v]['size'], r[v]['owner'], r[v]['style'], r[v]['evidence']) for v in (0x163ba0,0x163c00,0x123d40,0x24c819,0x165cc3,0x321340)]"
```
Expected:
| Address | Size | Owner | Style |
| --- | --- | --- | --- |
| `0x163ba0` | 84 | game | speed |
| `0x163c00` | 53 | game | speed |
| `0x123d40` | — | game | speed |
| `0x24c819` | — | game | size |
| `0x165cc3` | — | game | size |
| `0x321340` | — | xdk:libcmt | — |

If one is wrong, fix the rule, not the expectation. Report the owner counts.

- [ ] **Step 6: Update the spec's column list**

In `docs/specs/2026-10-01-project-setup-design.md`, in section 1, replace the columns line with:
`**Columns:** va, size, owner, style, evidence, name, calls, source, status. evidence is the style's clues (a16, pad, ebp, packed); calls is the space-separated addresses it calls or tail-jumps to.`

- [ ] **Step 7: Hand back.** Report the files, the test output, the owner counts and the check table. Do not commit.

---

### Task 5: The whole-game build (`tools/build.py`)

**Files:**
- Create: `tools/build.py`, `tests/test_build.py`, `include/unknown_11c920.h`, `config/files.json`, `src/.gitkeep`, `src/stubs/.gitkeep`

**Interfaces:**
- Produces:
  - `build.Marked(path: str, retail: int, name: str, returns: str, params: list[str], header: str)`. `name` is as written (it may contain `::`). `params` holds each parameter's type with its name removed.
  - `build.scan(text: str, path: str) -> list[Marked]`
  - `build.standin_source(source_path: str, includes: list[str], marked: list[Marked], prefix: str) -> str`
  - `build.build(root=ROOT, xdk=None) -> str`, which returns the map path. It raises `SystemExit` with a clear message when the SDK is missing.
  - The output files `build/halo2.exe` and `build/halo2.map`.
  - The `// @retail 0x<va>` marker convention, and the `PRIVATE` macro in `include/unknown_11c920.h`.

- [ ] **Step 1: Write the failing tests**

`tests/test_build.py`:
```python
import os

import pytest

import build
from build import scan, standin_source

SOURCE = '''#include "unknown_11c920.h"
#include "crc.h"

// @retail 0x163ba0
void function_163ba0(
	unsigned long *crc_reference,
	void const *buffer,
	long buffer_size)
{
}

// @retail 0x163c00
PRIVATE void function_163c00(unsigned long *crc_table)
{
}

// @retail 0x259d0
real function_259d0(unsigned long *seed, char const *file, long line, real lower_bound, real upper_bound)
{
	return lower_bound;
}
'''


def test_scan_reads_markers_and_signatures():
    marked = scan(SOURCE, 'src/crc.cpp')
    assert [(m.retail, m.name, m.returns, m.params) for m in marked] == [
        (0x163ba0, 'function_163ba0', 'void', ['unsigned long *', 'void const *', 'long']),
        (0x163c00, 'function_163c00', 'void', ['unsigned long *']),
        (0x259d0, 'function_259d0', 'real', ['unsigned long *', 'char const *', 'long', 'real', 'real']),
    ]


def test_standins_call_directly_with_volatile_arguments():
    text = standin_source('src/crc.cpp', ['#include "unknown_11c920.h"', '#include "crc.h"'],
                          scan(SOURCE, 'src/crc.cpp'), 'crc')
    assert 'void function_163ba0(unsigned long *, void const *, long);' in text
    assert 'function_163ba0(*(unsigned long * volatile *)(standin_crc_arguments + 0), ' in text
    assert 'static real volatile standin_crc_result_2;' in text
    assert 'standin_crc_result_2 = function_259d0(' in text
    assert '&function_163ba0' not in text  # never through a pointer


def test_build_reports_missing_sdk(tmp_path):
    with pytest.raises(SystemExit) as e:
        build.build(root=str(tmp_path), xdk=str(tmp_path / 'nowhere'))
    message = str(e.value)
    assert str(tmp_path / 'nowhere') in message and 'XDK_DIR' in message
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_build.py -v` → FAIL (`No module named 'build'`).

- [ ] **Step 3: Write `include/unknown_11c920.h` and `config/files.json`**

`include/unknown_11c920.h`:
```c
/* UNKNOWN_11C920.H: basic types and conventions shared by every source file */

#ifndef CSERIES_H
#define CSERIES_H

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef float real;

#define NONE -1

/* Bungie's static functions. Empty here, so the build's stand-in callers in
   other files can reach them; LTCG sees the whole program either way. */
#define PRIVATE

#endif
```
`config/files.json`:
```json
{
  "default": ["/O2", "/Gr"],
  "files": {}
}
```

- [ ] **Step 4: Write `tools/build.py`**

```python
"""Builds every source in src/ into one LTCG image, build/halo2.exe, with its
map build/halo2.map, so tools/check.py can compare its functions with retail.

Each source is compiled with its own flags (config/files.json). Each function
marked "// @retail 0x..." gets a stand-in caller, compiled for size without
inlining. The stand-in keeps the function in the image and out of line, as
retail's own callers do. Sources in src/stubs/ are compiled without /GL, so
they keep the standard calling conventions retail uses for code outside the
project.

    python tools/build.py
"""
import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MARKER = re.compile(r'^\s*//\s*@retail\s+(0x[0-9a-fA-F]+)\s*$')
INCLUDE = re.compile(r'^\s*#\s*include\s+"[^"]+"')
SIZE_FLAGS = ['/O1', '/Ob0', '/Gr']
ARGUMENT_STRIDE = 16


@dataclass
class Marked:
    path: str
    retail: int
    name: str
    returns: str
    params: list
    header: str


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


def scan(text, path):
    lines = text.splitlines()
    found = []
    for i, line in enumerate(lines):
        m = MARKER.match(line)
        if not m:
            continue
        header = ''
        for later in lines[i + 1:]:
            header += ' ' + later.split('//')[0]
            if '{' in later:
                break
        header = ' '.join(header.split('{')[0].split())
        h = re.match(r'^(?:PRIVATE\s+)?(?:inline\s+|__inline\s+)?(.*?)\s*([A-Za-z_][\w:]*)\s*\((.*)\)\s*$', header)
        if not h:
            raise SystemExit(f'{path}:{i + 2}: cannot read the function after @retail {m.group(1)}')
        params = [_param_type(p) for p in _split_params(h.group(3))]
        if params == ['void']:
            params = []
        found.append(Marked(path, int(m.group(1), 16), h.group(2), h.group(1).strip() or 'int', params, header))
    return found


def standin_source(source_path, includes, marked, prefix):
    out = [f'/* generated by tools/build.py from {source_path}: stand-in callers */']
    out += includes
    out.append(f'static unsigned char volatile standin_{prefix}_arguments[{ARGUMENT_STRIDE * 16}];')
    for k, m in enumerate(marked):
        if '::' in m.name:
            raise SystemExit(f'{source_path}: {m.name} is a member function; write its stand-in by hand '
                             'in src/standins/ (tools/build.py cannot make one)')
        out.append(f'{m.returns} {m.name}({", ".join(m.params)});')
    for k, m in enumerate(marked):
        args = ', '.join(
            f'*({p} volatile *)(standin_{prefix}_arguments + {ARGUMENT_STRIDE * n})'
            for n, p in enumerate(m.params))
        call = f'{m.name}({args})'
        if m.returns != 'void':
            out.append(f'static {m.returns} volatile standin_{prefix}_result_{k};')
            call = f'standin_{prefix}_result_{k} = {call}'
        out.append(f'void standin_{prefix}_{k}(void) {{ {call}; }}')
    return '\n'.join(out) + '\n'


def _run(tool, args, cwd, xdk):
    env = dict(os.environ, INCLUDE=os.path.join(xdk, 'include'), LIB=os.path.join(xdk, 'lib'))
    p = subprocess.run([os.path.join(xdk, 'bin', 'vc71', tool), '/nologo', *args], cwd=cwd, env=env,
                       capture_output=True, text=True)
    if p.returncode:
        raise SystemExit(f'{tool} failed:\n{p.stdout}{p.stderr}')


def _stale(target, sources):
    if not os.path.exists(target):
        return True
    t = os.path.getmtime(target)
    return any(os.path.getmtime(s) > t for s in sources)


def build(root=ROOT, xdk=None):
    xdk = xdk or os.environ.get('XDK_DIR', os.path.join(root, 'sdk', 'xbox'))
    if not os.path.exists(os.path.join(xdk, 'bin', 'vc71', 'CL.Exe')):
        raise SystemExit(f'Xbox SDK 5849 not found at {xdk}. Put its xbox folder at sdk/xbox, '
                         'or set XDK_DIR to it.')
    src, out = os.path.join(root, 'src'), os.path.join(root, 'build')
    obj_dir, gen = os.path.join(out, 'obj'), os.path.join(out, 'gen')
    os.makedirs(obj_dir, exist_ok=True)
    os.makedirs(gen, exist_ok=True)
    with open(os.path.join(root, 'config', 'files.json'), encoding='utf-8') as f:
        config = json.load(f)
    headers = [os.path.join(d, n) for base in ('include', 'src') for d, _, ns in os.walk(os.path.join(root, base))
               for n in ns if n.endswith('.h')]
    includes = ['/I', os.path.join(root, 'include'), '/I', src]

    objects, entry_calls, entry_decls = [], [], []
    sources = sorted(n for n in os.listdir(src) if n.endswith('.cpp')) if os.path.isdir(src) else []
    for name in sources:
        path = os.path.join(src, name)
        stem = os.path.splitext(name)[0]
        flags = config['files'].get(name, config['default'])
        obj = os.path.join(obj_dir, stem + '.obj')
        if _stale(obj, [path, os.path.join(root, 'config', 'files.json')] + headers):
            _run('CL.Exe', ['/c', '/GL', *flags, *includes, path, f'/Fo{obj}'], root, xdk)
        objects.append(obj)
        with open(path, encoding='utf-8') as f:
            text = f.read()
        marked = scan(text, f'src/{name}')
        if marked:
            gen_path = os.path.join(gen, f'standins_{stem}.cpp')
            source = standin_source(f'src/{name}', [l for l in text.splitlines() if INCLUDE.match(l)],
                                    marked, stem)
            if not os.path.exists(gen_path) or open(gen_path, encoding='utf-8').read() != source:
                with open(gen_path, 'w', encoding='utf-8') as f:
                    f.write(source)
            gen_obj = os.path.join(obj_dir, f'standins_{stem}.obj')
            if _stale(gen_obj, [gen_path] + headers):
                _run('CL.Exe', ['/c', '/GL', *SIZE_FLAGS, *includes, gen_path, f'/Fo{gen_obj}'], root, xdk)
            objects.append(gen_obj)
            for k in range(len(marked)):
                entry_decls.append(f'void standin_{stem}_{k}(void);')
                entry_calls.append(f'\tstandin_{stem}_{k}();')

    for sub, gl in (('stubs', []), ('standins', ['/GL'])):
        folder = os.path.join(src, sub)
        if os.path.isdir(folder):
            for name in sorted(n for n in os.listdir(folder) if n.endswith('.cpp')):
                path = os.path.join(folder, name)
                obj = os.path.join(obj_dir, f'{sub}_{os.path.splitext(name)[0]}.obj')
                flags = ['/O2', '/Gr'] if sub == 'stubs' else SIZE_FLAGS
                if _stale(obj, [path] + headers):
                    _run('CL.Exe', ['/c', *gl, *flags, *includes, path, f'/Fo{obj}'], root, xdk)
                objects.append(obj)

    entry = os.path.join(gen, 'entry.cpp')
    with open(entry, 'w', encoding='utf-8') as f:
        f.write('/* generated by tools/build.py: the image entry point */\n'
                'extern "C" int _fltused = 0;\n' + '\n'.join(entry_decls) +
                '\nextern "C" int entry(void)\n{\n' + '\n'.join(entry_calls) + '\n\treturn 0;\n}\n')
    entry_obj = os.path.join(obj_dir, 'entry.obj')
    _run('CL.Exe', ['/c', '/GL', *SIZE_FLAGS, entry, f'/Fo{entry_obj}'], root, xdk)
    objects.append(entry_obj)

    exe, map_path = os.path.join(out, 'halo2.exe'), os.path.join(out, 'halo2.map')
    _run('Link.Exe', ['/LTCG', '/NODEFAULTLIB', '/ENTRY:entry', '/SUBSYSTEM:CONSOLE', '/MAP:' + map_path,
                      '/MAPINFO:FIXUPS', '/FIXED:NO', f'/OUT:{exe}', *objects], root, xdk)
    return map_path


if __name__ == '__main__':
    print(build())
```

- [ ] **Step 5: Run the unit tests until they pass**

Run: `python -m pytest tests/test_build.py -v`
Expected: 3 passed. If the standin text assertions fail on spacing, fix `standin_source`, not the test. The test's strings are the format other tools and people will read.

- [ ] **Step 6: Check that the build runs with the spike's crc in `src/`, then remove it again**

Copy `spike/crc.cpp` to `src/crc.cpp`.
- Add `#include "unknown_11c920.h"` and the markers `// @retail 0x163ba0` and `// @retail 0x163c00` above the two functions.
- Add `PRIVATE` before `static void function_163c00` in both places, removing `static`.
- Remove the local `typedef unsigned char byte;`.

Then run `python tools/build.py`.
Expected: it prints `...build/halo2.map`, and the map contains `?function_163ba0@@YIXPAKPBXJ@Z` and `?function_163c00@@YIXPAK@Z` plus `FIXUPS:` lines.

Delete `src/crc.cpp` afterwards: Task 7 adds it properly.

- [ ] **Step 7: Hand back.** Report the files, the test output and the map excerpt. Do not commit.

---

### Task 6: The checker (`tools/check.py`)

**Files:**
- Create: `tools/check.py`, `tests/test_check.py`
- Modify: `tools/match.py` (its docstring points to `check.py` for project work)

**Interfaces:**
- Consumes:
  - `build.build`, `build.scan`
  - `linkmap.LinkMap`, `linkmap.plain_name`
  - `pe.Pe`
  - `xbe.Xbe`
  - `inventory.read_rows`, `inventory.write_rows`, `inventory.check_retail`
- Produces:
  - `check.resolve(linkmap, marked) -> MapSymbol`, which raises `SystemExit` when the name is missing or ambiguous.
  - `check.trim(code: bytes) -> bytes`, which strips the trailing `0xCC` fill.
  - `check.compare(ours: bytes, ours_va: int, theirs: bytes, theirs_va: int, masked: set[int]) -> int | None`. It returns the first differing offset, or `None` when they match.
  - `check.masked_offsets(ours_va, size, fixups) -> set[int]`, the offsets inside the function covered by any 4-byte fixup.
  - `check.relative_stays(theirs: bytes, theirs_va: int, offsets: list[int]) -> int | None`. It returns the first offset where retail's relative field targets the function itself, or `None`. Our linker only lists fields that leave the function.
  - The report file `build/report.json`.
  - The summary line `matched N of M game functions (B of T bytes, P%)`.
  - The `status` column, set to `matched`, `near` (same length, at most 2 differing instructions) or `todo`, and the `source` column.

- [ ] **Step 1: Write the failing tests**

`tests/test_check.py`:
```python
import pytest

from build import Marked
from check import compare, masked_offsets, relative_stays, resolve, trim
from linkmap import LinkMap

MAP = """ Preferred load address is 00400000
 0001:00000000 00000100H .text                   CODE
 0001:00000000       ?f@@YIXH@Z                 00401000 f   a.obj
 0001:00000040       ?f@@YIXM@Z                 00401040 f   a.obj
 0001:00000080       ?g@@YIXXZ                  00401080 f   a.obj
"""


def marked(name):
    return Marked('src/a.cpp', 0x1000, name, 'void', [], '')


def test_resolve_symbol():
    assert resolve(LinkMap(MAP), marked('g')).va == 0x401080


def test_resolve_symbol_ambiguous():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(MAP), marked('f'))
    assert '?f@@YIXH@Z' in str(e.value) and '?f@@YIXM@Z' in str(e.value)


def test_resolve_symbol_missing():
    with pytest.raises(SystemExit) as e:
        resolve(LinkMap(MAP), marked('h'))
    assert 'not in the image' in str(e.value)


def test_trim_strips_fill():
    assert trim(bytes.fromhex('c3cccccc')) == bytes.fromhex('c3')


def test_compare_masks_only_listed_fields():
    ours = bytes.fromhex('a1' '00204000' 'c3')
    theirs = bytes.fromhex('a1' '88e75500' 'c3')
    assert compare(ours, 0x401000, theirs, 0x163ba0, masked_offsets(0x401000, 6, {0x401001})) is None
    assert compare(ours, 0x401000, theirs, 0x163ba0, set()) == 1


def test_compare_unequal_lengths():
    assert compare(bytes.fromhex('33c0c3'), 0x401000, bytes.fromhex('33c0'), 0x10000, set()) == 2


def test_relative_fields_must_leave_in_retail():
    call_out = bytes.fromhex('e8' '10000000' 'c3')     # call va+0x15, outside a 6-byte function
    jump_self = bytes.fromhex('e9' 'fbffffff' 'c3')    # jmp va, inside
    assert relative_stays(call_out, 0x1000, [1]) is None
    assert relative_stays(jump_self, 0x1000, [1]) == 1
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_check.py -v` → FAIL (`No module named 'check'`).

- [ ] **Step 3: Write `tools/check.py`**

```python
"""Builds the project (tools/build.py) and compares every function marked
"// @retail 0x..." with the retail XBE, byte for byte apart from address
fields. Our extent comes from the linker map, retail's from
config/functions.csv. The masked fields are exactly those our linker filled
in: base relocations and the relative fields /MAPINFO:FIXUPS lists.

Writes build/report.json, updates the status and source columns of
config/functions.csv, and prints a summary.

    python tools/check.py [<retail va> ...] [--no-build]
"""
import argparse
import json
import os
import sys

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import build
from inventory import check_retail, read_rows, write_rows
from linkmap import LinkMap, plain_name
from pe import Pe
from xbe import Xbe

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NEAR = 2  # differing instructions, at most, for "near"


def resolve(linkmap, marked):
    hits = linkmap.find(marked.name)
    if not hits:
        raise SystemExit(f'{marked.path}: {marked.name} (@retail {marked.retail:#x}) is not in the image')
    if len(hits) > 1:
        raise SystemExit(f'{marked.path}: {marked.name} is ambiguous: ' + ', '.join(h.name for h in hits))
    return hits[0]


def trim(code):
    return code.rstrip(b'\xcc')


def masked_offsets(va, size, fixups):
    offsets = set()
    for f in fixups:
        for k in range(4):
            if 0 <= f + k - va < size:
                offsets.add(f + k - va)
    return offsets


def relative_stays(theirs, theirs_va, offsets):
    for k in sorted(offsets):
        if k + 4 > len(theirs):
            continue
        target = (theirs_va + k + 4 + int.from_bytes(theirs[k:k + 4], 'little', signed=True)) & 0xFFFFFFFF
        if theirs_va <= target < theirs_va + len(theirs):
            return k
    return None


def compare(ours, ours_va, theirs, theirs_va, masked):
    for k in range(min(len(ours), len(theirs))):
        if k not in masked and ours[k] != theirs[k]:
            return k
    return None if len(ours) == len(theirs) else min(len(ours), len(theirs))


def differing_instructions(ours, ours_va, theirs, theirs_va, masked):
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    a = list(md.disasm(ours, ours_va))
    b = list(md.disasm(theirs, theirs_va))
    lines, count = [], 0
    for k in range(max(len(a), len(b))):
        x, y = (a[k] if k < len(a) else None), (b[k] if k < len(b) else None)
        same = x is not None and y is not None and x.size == y.size and all(
            (x.address - ours_va + i) in masked or x.bytes[i] == y.bytes[i] for i in range(x.size))
        count += not same
        lines.append(f'  {"  " if same else "!!"} '
                     f'{(x.mnemonic + " " + x.op_str) if x else "":<44} | '
                     f'{(y.mnemonic + " " + y.op_str) if y else ""}')
    return count, lines


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('addresses', nargs='*')
    ap.add_argument('--no-build', action='store_true')
    args = ap.parse_args()
    retail_path = os.environ.get('RETAIL_XBE', os.path.join(ROOT, 'orig', 'default.xbe'))
    check_retail(retail_path)
    map_path = os.path.join(ROOT, 'build', 'halo2.map') if args.no_build else build.build()

    linkmap = LinkMap.read(map_path)
    image = Pe(os.path.join(ROOT, 'build', 'halo2.exe'))
    retail = Xbe(retail_path)
    csv_path = os.path.join(ROOT, 'config', 'functions.csv')
    rows = read_rows(csv_path)
    fixups = image.fixups | linkmap.rel_fixups

    marked = []
    src = os.path.join(ROOT, 'src')
    for name in sorted(os.listdir(src)) if os.path.isdir(src) else []:
        if name.endswith('.cpp'):
            with open(os.path.join(src, name), encoding='utf-8') as f:
                marked += build.scan(f.read(), f'src/{name}')
    wanted = {int(a, 16) for a in args.addresses}
    report, failed = {}, 0
    for m in marked:
        if wanted and m.retail not in wanted:
            continue
        row = rows.get(m.retail)
        if row is None:
            raise SystemExit(f'{m.path}: @retail {m.retail:#x} is not a function start in config/functions.csv')
        symbol = resolve(linkmap, m)
        start, end = linkmap.extent(symbol)
        ours = trim(image.read(start, end - start))
        theirs = retail.read(m.retail, int(row['size']))
        masked = masked_offsets(start, len(ours), fixups)
        first = compare(ours, start, theirs, m.retail, masked)
        if first is None:
            first = relative_stays(theirs, m.retail, [f - start for f in linkmap.rel_fixups if start <= f < end])
        status = 'matched'
        if first is not None:
            count, lines = differing_instructions(ours, start, theirs, m.retail, masked)
            status = 'near' if len(ours) == len(theirs) and count <= NEAR else 'todo'
            failed += 1
            print(f'DIFF   {m.retail:08x} {m.name} at +{first:#x} ({len(ours)} bytes, retail {len(theirs)})')
            print('\n'.join(lines))
        else:
            print(f'MATCH  {m.retail:08x} {m.name}')
        row['status'], row['source'] = status, m.path
        report[f'{m.retail:08x}'] = dict(name=m.name, source=m.path, status=status, size=len(theirs),
                                         ours=len(ours), first_difference=first)
    write_rows(csv_path, list(rows.values()))
    with open(os.path.join(ROOT, 'build', 'report.json'), 'w', encoding='utf-8') as f:
        json.dump(report, f, indent=1, sort_keys=True)

    game = [r for r in rows.values() if r['owner'] == 'game']
    done = [r for r in game if r['status'] == 'matched']
    total, matched = sum(int(r['size']) for r in game), sum(int(r['size']) for r in done)
    print(f'matched {len(done)} of {len(game)} game functions '
          f'({matched} of {total} bytes, {100 * matched / max(total, 1):.2f}%)')
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
```

- [ ] **Step 4: Run the unit tests until they pass**

Run: `python -m pytest tests/test_check.py -v`
Expected: 7 passed

- [ ] **Step 5: Point `match.py` at `check.py`**

In `tools/match.py`'s docstring, add after the first paragraph:
`For project work, use tools/check.py, which builds src/ and checks every marked function. This script is for quick experiments like spike/.`

- [ ] **Step 6: Hand back.** Report the files and the test output. Do not commit.

---

### Task 7: The spike's functions in `src/`, matching in the whole-game build

**Files:**
- Create:
  - `src/crc.cpp`, `src/unknown_123b30.cpp`, `src/unknown_0259d0.cpp`
  - `src/unknown_1edbc0.cpp`, `src/unknown_24c819.cpp`, `src/unknown_165cc3.cpp`
  - `include/unknown_123b30.h`, `include/unknown_0259d0.h`, `include/crc.h`
- Modify: `config/files.json`

**Interfaces:**
- Consumes: `tools/build.py`, `tools/check.py`, `config/functions.csv`
- Produces: eight `matched` rows in `config/functions.csv`, and one `near` row (`0x123d80`).

- [ ] **Step 1: Write the sources from the spike**

Use the spike's function bodies unchanged: `spike/crc.cpp`, `spike/unknown_123b30.cpp`, `spike/unknown_123b30_o2.cpp`, `spike/unknown_123b30_os.cpp` and `spike/unknown_0259d0.cpp`.
- **Markers:** every function gets its `// @retail 0x...` line.
- **`static` functions** become `PRIVATE`.
- **Shared types** move into the headers: `byte` and `real` to `unknown_11c920.h`; `s_game_state_globals` and the prototypes to `unknown_123b30.h`; the `point3f` and `vector3f` unions to `unknown_0259d0.h`; the crc prototypes to `crc.h`. Every source includes `"unknown_11c920.h"` first.
- **In `src/unknown_0259d0.cpp`,** `distance3d` and `function_259d0` are ordinary functions: drop `__declspec(noinline)`, since the stand-ins are compiled `/Ob0`.
- **The three initializer functions:** take each from its spike file into its `src/unknown_<va>.cpp`, named `game_state_initialize_<va>`. Remove the pointer table and `entry`, which the build generates now.
- **Remove the harness code** (`crc_test.cpp` callers, `g_*` test globals). It is not needed.

`config/files.json`:
```json
{
  "default": ["/O2", "/Gr"],
  "files": {
    "crc.cpp": ["/O2", "/Ob1", "/Gr"],
    "unknown_24c819.cpp": ["/O1", "/Gr"],
    "unknown_165cc3.cpp": ["/O1", "/Gr"]
  }
}
```

- [ ] **Step 2: Run the checker**

Run: `python tools/check.py`
Expected: `MATCH` for `00163ba0`, `00163c00`, `00123d40`, `001edbc0`, `0024c819`, `00165cc3`, `0003ea30` and `000259d0`, and `DIFF` for `00123d80` with the single `lea` line. The process exits 1 because of that DIFF.

If a function that matched in the spike now differs, the build differs from the spike. The likely causes are:
- **a stand-in changed a calling convention:** compare against the spike's `tools/match.py` command;
- **`PRIVATE`** (external linkage) changed `function_163c00`. If so, record it in `docs/PROGRESS.md`, and change `build.py` to compile each source's stand-ins inside that source's translation unit (`#include` the generated stand-in file at the end of the source) instead of relying on `PRIVATE`.

Fix the build, not the expectation.

- [ ] **Step 3: Run the whole test suite**

Run: `python -m pytest -v`
Expected: all pass.

- [ ] **Step 4: Hand back.** Report the checker output and the summary line. Do not commit.

---

### Task 8: The ready queue and the worker procedure

**Files:**
- Create: `tools/ready.py`, `tests/test_ready.py`, `tools/dis.py`, `docs/DECOMPILING.md`

**Interfaces:**
- Consumes: `inventory.read_rows`
- Produces:
  - `ready.components(graph: dict[int, set[int]]) -> list[set[int]]` (Tarjan's algorithm).
  - `ready.ready(rows: dict[int, dict]) -> list[dict]`: game rows not yet matched whose callees, outside their own component, are all matched or not `game`. Smallest first.
  - The command `python tools/ready.py [N]`, which prints the first N (default 20) as `va size name calls`.

- [ ] **Step 1: Write the failing tests**

`tests/test_ready.py`:
```python
from ready import components, ready


def row(va, calls='', owner='game', status='todo', size=10, name=''):
    return dict(va=f'{va:08x}', size=str(size), owner=owner, status=status, name=name,
                calls=' '.join(f'{c:08x}' for c in calls))


def test_components_groups_mutual_recursion():
    groups = components({1: {2}, 2: {1}, 3: {1}})
    assert {frozenset(g) for g in groups} == {frozenset({1, 2}), frozenset({3})}


def test_ready_needs_matched_or_library_callees():
    rows = {r: v for r, v in [
        (0x10, row(0x10, calls=[0x20])),                       # calls a todo game function
        (0x20, row(0x20, size=5)),                             # leaf
        (0x30, row(0x30, calls=[0x40], size=7)),               # calls a library function
        (0x40, row(0x40, owner='xdk:libcmt')),
        (0x50, row(0x50, calls=[0x60], size=3)),
        (0x60, row(0x60, status='matched')),
        (0x70, row(0x70, status='matched')),                   # done already
    ]}
    assert [r['va'] for r in ready(rows)] == ['00000050', '00000020', '00000030']


def test_ready_treats_recursion_as_one_unit():
    rows = {0x10: row(0x10, calls=[0x20]), 0x20: row(0x20, calls=[0x10])}
    assert {r['va'] for r in ready(rows)} == {'00000010', '00000020'}
```

- [ ] **Step 2: Run them to see them fail**

Run: `python -m pytest tests/test_ready.py -v` → FAIL (`No module named 'ready'`).

- [ ] **Step 3: Write `tools/ready.py`**

```python
"""Lists the game functions ready to decompile: not yet matched, and every
function they call is matched, belongs to a library, or is part of the same
mutual recursion. Smallest first.

    python tools/ready.py [N]
"""
import os
import sys

from inventory import read_rows

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


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


def ready(rows):
    def calls(va):
        text = rows[va]['calls']
        return {int(c, 16) for c in text.split()} if text else set()

    game = {va for va, r in rows.items() if r['owner'] == 'game'}
    graph = {va: calls(va) & game for va in game}
    result = []
    for group in components(graph):
        if all(rows[va]['status'] == 'matched' for va in group):
            continue
        outside = set().union(*(calls(va) for va in group)) - group
        if all(c not in rows or rows[c]['owner'] != 'game' or rows[c]['status'] == 'matched' for c in outside):
            result += [rows[va] for va in group if rows[va]['status'] != 'matched']
    return sorted(result, key=lambda r: (int(r['size']), r['va']))


def main():
    count = int(sys.argv[1]) if len(sys.argv) > 1 else 20
    for r in ready(read_rows(os.path.join(ROOT, 'config', 'functions.csv')))[:count]:
        print(r['va'], r['size'], r['name'] or '-', r['calls'] or '-')


if __name__ == '__main__':
    main()
```

- [ ] **Step 4: Run the tests until they pass**

Run: `python -m pytest tests/test_ready.py -v` → 3 passed.
Run: `python tools/ready.py 10` → prints ten ready functions. The list must not include the eight matched spike functions.

- [ ] **Step 5: Write `tools/dis.py`**

```python
"""Prints a retail function's disassembly, with its inventory row and the
names of what it calls. Jump tables inside the function print as data.

    python tools/dis.py <va>
"""
import os
import sys

from capstone import CS_ARCH_X86, CS_MODE_32, Cs

from inventory import read_rows
from xbe import Xbe

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    va = int(sys.argv[1], 16)
    rows = read_rows(os.path.join(ROOT, 'config', 'functions.csv'))
    row = rows.get(va)
    if row is None:
        raise SystemExit(f'{va:#x} is not a function start in config/functions.csv')
    image = Xbe(os.environ.get('RETAIL_XBE', os.path.join(ROOT, 'orig', 'default.xbe')))
    print(f"{row['va']} size {row['size']} {row['owner']} {row['style']} ({row['evidence']}) {row['name'] or '-'}")
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.skipdata = True
    for address, size, mnemonic, op_str in md.disasm_lite(image.read(va, int(row['size'])), va):
        note = ''
        if mnemonic in ('call', 'jmp') and op_str.startswith('0x'):
            callee = rows.get(int(op_str, 16))
            if callee:
                note = f"   ; {callee['name'] or callee['va']} [{callee['status']}]"
        print(f'  {address:08x}  {mnemonic} {op_str}{note}')


if __name__ == '__main__':
    main()
```

Run: `python tools/dis.py 163ba0`. It prints `function_163ba0`'s 30 instructions, and the `call 0x163c00` line notes `?function_163c00@@YAXPAK@Z [matched]`.

- [ ] **Step 6: Write `docs/DECOMPILING.md`**

```markdown
# Decompiling a function

This is the procedure for one function, written for a person or a subagent.

## Before you start

- **The SDK:** XDK 5849 at `sdk/xbox`, or set `XDK_DIR`.
- **The retail XBE:** `orig/default.xbe`. Extract it with
  `python tools/xiso_extract.py "<your Halo 2 image>" orig default.xbe`.
- **Python:** `pip install -r requirements-dev.txt`.
- **Pick a function:** take one from `python tools/ready.py`. Every function
  it calls is already matched, so its callers' code is the only unknown.

## Steps

1. **Read the retail code** with `python tools/dis.py <va>`.
   - `config/functions.csv` gives the function's size, its library name (if
     a signature finds it) and what it calls.
   - Note which arguments arrive in registers. LTCG gives internal functions
     custom conventions; write normal C++, and the compiler will choose the
     same registers.
2. **Find related source.** It may be:
   - the function of the same name in the Halo CE decompilation
     (punpckhdq/halo, CC0);
   - a neighbouring matched function;
   - the structures in `include/`.
3. **Write the function** in the `src/` file it belongs to, or
   `src/unknown_<va>.cpp`.
   - Put `// @retail 0x<va>` on the line above it.
   - Write `static` functions as `PRIVATE`.
4. **Set the file's flags** in `config/files.json` if it was compiled for
   size (the `style` column says `size`: use `/O1 /Gr`).
5. **Run** `python tools/check.py <va>`. Repeat until it prints `MATCH`.
   Things that usually decide a match:
   - **types:** `short` against `long`, signed against unsigned;
   - **the order of terms** in floating-point expressions;
   - **whether a value goes through a local variable,** and whether a
     parameter's address is taken (that keeps it on the stack);
   - **the file's flags** (`/O1` against `/O2`, `/Ob1`).
6. **Stop after 20 tries,** or when only register choice or operand order
   differs. Leave the function with its marker. The checker records it as
   `near` or `todo` with the first difference, so someone can come back to
   it.

## What not to do

- Do not add `__declspec(noinline)` or other attributes Bungie's code would
  not have had. Stand-ins keep functions out of line.
- Do not commit anything from `orig/`, `sdk/` or `build/`.
```

- [ ] **Step 7: Hand back.** Report the files, the test output and the ready list. Do not commit.

---

### Task 9: Publish the set-up

**Files:**
- Modify: `README.md` (status, layout, how to build), `docs/PROGRESS.md` (new entry), `.gitignore` (`/build/` is already ignored; check it)

**Interfaces:**
- Consumes: the summary line of `tools/check.py`, and the owner counts from `tools/inventory.py`.

- [ ] **Step 1: Rewrite the README's Tools section**

Replace it with "Build and check":
1. Install: `pip install -r requirements-dev.txt`.
2. Supply the SDK and the XBE.
3. Run `python tools/inventory.py` (only when the inventory is regenerated).
4. Run `python tools/check.py`.
5. Pick work with `python tools/ready.py`, following `docs/DECOMPILING.md`.

Keep the table of tools, adding `inventory.py`, `functions.py`, `libsig.py`, `build.py`, `check.py` and `ready.py`. Replace the status paragraph with the checker's summary line.

- [ ] **Step 2: Add a dated entry to `docs/PROGRESS.md`**

Write it in the style of the earlier entries. It covers:
- the inventory's owner counts;
- the checker's summary;
- what changed from the spike's tool;
- anything Task 7 found about `PRIVATE`.

- [ ] **Step 3: Run the full test suite and the checker once more**

Run: `python -m pytest -v` (all pass) and `python tools/check.py` (8 MATCH, 1 DIFF).

- [ ] **Step 4: Publish (coordinator)**

Run: `bash local/publish.sh "Project set-up: inventory, whole-game LTCG build, checker, ready queue"`
Expected: `published: https://github.com/kirklandsig/halo2-decompiled`
