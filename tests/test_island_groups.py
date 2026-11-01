"""Optional retail execution checks for island grouping and component export."""
import json
import random
import struct
from pathlib import Path

import pytest

unicorn = pytest.importorskip("unicorn", reason="optional x86 differential checks")
from unicorn import x86_const as reg

from linkmap import LinkMap
from pe import Pe
from xbe import Xbe, retail_xbe_path

ROOT = Path(__file__).resolve().parents[1]
HEAP, STACK, STOP, MOCK = 0x10000000, 0x20000000, 0x30000000, 0x30001000
OBJECT, PARENTS, SCRATCH, PAIRS, GROUPS = [HEAP + x for x in (0x100, 0x200, 0x220, 0x240, 0x260)]
P_DATA, S_DATA, E_DATA, G_DATA = [HEAP + x for x in (0x3000, 0x4000, 0x5000, 0x6000)]
ALLOCATOR, TABLE, ARENA = HEAP + 0xf00, HEAP + 0xf40, HEAP + 0x10000
TARGETS = [0x3720a0, 0x3720c0, 0x372180, 0x372200, 0x372300, 0x372390]
PRESERVED = {reg.UC_X86_REG_EBX: 0x12345678, reg.UC_X86_REG_ESI: 0x23456789,
             reg.UC_X86_REG_EDI: 0x3456789a, reg.UC_X86_REG_EBP: 0x456789ab}


def put(memory, address, fmt, *values):
    struct.pack_into("<" + fmt, memory, address - HEAP, *values)


def read(memory, address, count, signed=False):
    return struct.unpack_from("<" + ("i" if signed else "I") * count, memory, address - HEAP)


def fresh():
    memory = bytearray(b"\xcd" * 0x40000)
    put(memory, OBJECT, "3I", PARENTS, SCRATCH, 0x55667788)
    put(memory, ALLOCATOR, "I", TABLE)
    put(memory, TABLE, "6I", MOCK, MOCK + 0x10, 0, 0, MOCK + 0x20, MOCK + 0x30)
    return memory


class Machine:
    def __init__(self, image, entries, allocator):
        self.entries = entries
        self.uc = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
        lo = min(s.va for s in image.sections) & ~4095
        hi = (max(s.va + max(s.vsize, s.rsize) for s in image.sections) + 4095) & ~4095
        self.uc.mem_map(lo, hi - lo)
        for section in image.sections:
            self.uc.mem_write(section.va, image.data[section.raw:section.raw + section.rsize])
        for base, size in [(0, 4096), (HEAP, 0x40000), (STACK, 0x10000), (STOP, 0x2000)]:
            self.uc.mem_map(base, size)
        self.uc.mem_write(allocator, struct.pack("<I", ALLOCATOR))
        for offset, cleanup in [(0, 8), (0x10, 4), (0x20, 8), (0x30, 12)]:
            self.uc.mem_write(MOCK + offset, b"\xc2" + struct.pack("<H", cleanup))
        self.uc.hook_add(unicorn.UC_HOOK_CODE, self.callback)
        self.uc.hook_add(unicorn.UC_HOOK_MEM_WRITE, self.write)

    def words(self, address, count):
        return struct.unpack("<" + "I" * count, self.uc.mem_read(address, count * 4))

    def callback(self, machine, address, size, unused):
        if address not in (MOCK, MOCK + 0x10, MOCK + 0x20, MOCK + 0x30):
            return
        assert machine.reg_read(reg.UC_X86_REG_ECX) == ALLOCATOR
        sp = machine.reg_read(reg.UC_X86_REG_ESP)
        result = 0xbad0cafe
        slot = {MOCK: 0, MOCK + 0x10: 1, MOCK + 0x20: 4, MOCK + 0x30: 5}[address]
        if slot in (0, 4):
            length, tag = self.words(sp + 4, 2)
            result = self.cursor
            self.cursor += (max(length, 1) + 15) & ~15
            assert self.cursor <= HEAP + 0x40000
            self.events.append(("allocate", slot, length, tag, result))
        elif slot == 1:
            self.events.append(("release", slot, *self.words(sp + 4, 1)))
        else:
            self.events.append(("release", slot, *self.words(sp + 4, 3)))
        machine.reg_write(reg.UC_X86_REG_EAX, result)
        machine.reg_write(reg.UC_X86_REG_ECX, 0xfeed1234)
        machine.reg_write(reg.UC_X86_REG_EDX, 0xface5678)

    def write(self, machine, access, address, size, value, unused):
        if any(lo <= address < hi for lo, hi in self.trace):
            self.writes.append((address, size, value & ((1 << (size * 8)) - 1)))

    def reset(self, memory):
        self.uc.mem_write(HEAP, bytes(memory))
        self.cursor = ARENA

    def run(self, va, args=(), this=OBJECT, cleanup=0, seed=0, trace=()):
        self.events, self.writes, self.trace = [], [], trace
        self.uc.mem_write(STACK, b"\xcc" * 0x10000)
        self.uc.mem_write(0, struct.pack("<I", 0xffffffff))
        sp = STACK + 0x8000 + (seed & 3) * 4
        self.uc.mem_write(sp, struct.pack("<" + "I" * (len(args) + 1), STOP, *args))
        for register, value in PRESERVED.items():
            self.uc.reg_write(register, value)
        for register in [reg.UC_X86_REG_EAX, reg.UC_X86_REG_EDX]:
            self.uc.reg_write(register, 0)
        self.uc.reg_write(reg.UC_X86_REG_ECX, this)
        self.uc.reg_write(reg.UC_X86_REG_ESP, sp)
        self.uc.reg_write(reg.UC_X86_REG_EFLAGS, 2)
        self.uc.reg_write(reg.UC_X86_REG_FPCW, 0x37f)
        self.uc.reg_write(reg.UC_X86_REG_FPTAG, 0xffff)
        self.uc.reg_write(reg.UC_X86_REG_FPSW, 0)
        self.uc.reg_write(reg.UC_X86_REG_MXCSR, 0x1f80)
        self.uc.emu_start(self.entries[va], STOP, count=1000000)
        assert self.uc.reg_read(reg.UC_X86_REG_EIP) == STOP
        assert self.uc.reg_read(reg.UC_X86_REG_ESP) == sp + 4 + cleanup
        assert self.words(0, 1)[0] == 0xffffffff
        for register, value in PRESERVED.items():
            assert self.uc.reg_read(register) == value
        return (bytes(self.uc.mem_read(HEAP, 0x40000)), tuple(self.events), tuple(self.writes),
                self.uc.reg_read(reg.UC_X86_REG_EAX))


@pytest.fixture(scope="module")
def machines():
    path = Path(retail_xbe_path())
    if not path.exists() or not (ROOT / "build/halo2.exe").exists():
        pytest.skip("retail XBE and a current full build required")
    report = json.loads((ROOT / "build/report.json").read_text())
    mapping = LinkMap.read(ROOT / "build/halo2.map")
    entries = {}
    for va in TARGETS:
        name = report.get(f"{va:08x}", {}).get("name", "")
        candidates = [s for s in mapping.find(name) if "ABV0@@Z" not in s.name] if name else []
        if not candidates:
            pytest.skip("full build does not contain the island grouping batch")
        assert len(candidates) == 1
        entries[va] = candidates[0].va
    allocator = mapping.find("g_480118")
    assert len(allocator) == 1
    return [Machine(Xbe(path), {v: v for v in TARGETS}, 0x480118),
            Machine(Pe(ROOT / "build/halo2.exe"), entries, allocator[0].va)]


def compare_step(machines, va, args=(), this=OBJECT, cleanup=0, seed=0,
                 trace=(), returned=False):
    results = [m.run(va, args, this, cleanup, seed, trace) for m in machines]
    assert results[0][:3] == results[1][:3], (hex(va), seed)
    if returned:
        assert results[0][3] == results[1][3], (hex(va), seed, "return")
    return results[0][0], results[0][1], results[0][3]


def forest(seed):
    rng = random.Random(seed)
    count = 1 + seed % 48
    parents = [-1] * count
    for i in range(1, count):
        if seed % 5 == 0:
            parents[i] = i - 1
        elif seed % 5 == 1:
            parents[i] = 0
        elif rng.randrange(3):
            parents[i] = rng.randrange(i)
    roots = []
    for i in range(count):
        root = i
        while parents[root] >= 0:
            root = parents[root]
        roots.append(root)
    for root in set(roots):
        parents[root] = -roots.count(root)
    memory = fresh()
    scratch = [rng.randrange(count) for _ in range(seed % 37)]
    put(memory, PARENTS, "3I", P_DATA, count, count | 0x80000000)
    put(memory, SCRATCH, "3I", S_DATA, len(scratch), len(scratch) | 0x80000000)
    put(memory, P_DATA, "i" * count, *parents)
    put(memory, S_DATA, "I" * len(scratch), *scratch)
    return rng, memory, count


def test_group_constructor(machines):
    for seed in range(64):
        memory = fresh()
        put(memory, OBJECT, "3I", 0xcdcdcdcd, 0xcdcdcdcd, seed * 12345)
        for machine in machines:
            machine.reset(memory)
        output, events, result = compare_step(machines, 0x3720a0, (PARENTS, SCRATCH), cleanup=8,
                                              seed=seed, returned=True)
        assert read(output, OBJECT, 3) == (PARENTS, SCRATCH, seed * 12345)
        assert not events and result == OBJECT


@pytest.mark.parametrize("va", [0x3720c0, 0x372180])
def test_forest_updates(machines, va):
    for seed in range(512):
        rng, memory, count = forest(seed)
        for machine in machines:
            machine.reset(memory)
        args = (rng.randrange(count), rng.randrange(count)) if va == 0x3720c0 else ()
        trace = [(P_DATA, P_DATA + count * 4), (S_DATA, S_DATA + read(memory, SCRATCH, 3)[1] * 4)]
        compare_step(machines, va, args, cleanup=8 if args else 0, seed=seed,
                     trace=trace, returned=bool(args))


def graph(seed):
    rng = random.Random(seed)
    count = (seed * 7) % 49
    if not count or seed % 5 == 0:
        edges = []
    elif seed % 5 == 1:
        edges = [(i, i - 1) for i in range(1, count)]
    elif seed % 5 == 2:
        edges = [(i, (i // 4) * 4) for i in range(count)]
    else:
        edges = [(rng.randrange(count), rng.randrange(count)) for _ in range(seed % 97)]
        if seed % 5 == 3:
            edges = edges[:len(edges)//2] * 2
    memory = fresh()
    for address, data, needed, salt in [(PARENTS, P_DATA, count, 0), (SCRATCH, S_DATA, len(edges), 1)]:
        capacity = [0, 1, needed, needed * 2][(seed + salt) % 4]
        previous_size = min(capacity, seed % 7)
        put(memory, address, "3I", data if capacity else 0, previous_size,
            capacity | (0x80000000 if (seed + salt) & 1 else 0))
    put(memory, PAIRS, "3I", E_DATA if edges else 0, len(edges), len(edges))
    for i, (a, b) in enumerate(edges):
        put(memory, E_DATA + i * 8, "2I", a, b)
    existing = seed % 3
    capacity = max(existing, [0, 1, 4, 64][seed % 4])
    put(memory, GROUPS, "3I", G_DATA if capacity else 0, existing,
        capacity | (0x80000000 if seed & 2 else 0))
    for i in range(existing):
        put(memory, G_DATA + i * 12, "3I", HEAP + 0x7000 + i * 64, 1, 1)
        put(memory, HEAP + 0x7000 + i * 64, "I", 1000 + i)
    return memory, count, edges, existing


def components(count, edges):
    """Independent graph traversal, without a union-find implementation."""
    neighbors = [set() for _ in range(count)]
    for a, b in edges:
        neighbors[a].add(b)
        neighbors[b].add(a)
    remaining, groups = set(range(count)), []
    while remaining:
        pending, found = [min(remaining)], set()
        while pending:
            node = pending.pop()
            if node not in found:
                found.add(node)
                pending.extend(neighbors[node] - found)
        remaining -= found
        groups.append(sorted(found))
    return groups


def test_component_pipeline(machines):
    for seed in range(256):
        memory, count, edges, existing = graph(seed)
        for machine in machines:
            machine.reset(memory)
        setup, _, _ = compare_step(machines, 0x372200, (PAIRS, count), cleanup=8, seed=seed)
        expected = components(count, edges)
        parent_header, scratch_header = read(setup, PARENTS, 3), read(setup, SCRATCH, 3)
        table = read(setup, parent_header[0], count, signed=True) if count else ()
        for group in expected:
            root = min(group)
            assert table[root] == -len(group)
            assert all(node == root or table[node] == root for node in group)
        exported, _, _ = compare_step(machines, 0x372390, (GROUPS,), cleanup=4, seed=seed)
        data, size, flags = read(exported, GROUPS, 3)
        assert size == existing + len(expected)
        for i, group in enumerate(expected, existing):
            indices, length, written = read(exported, data + i * 12, 3)
            assert length == written == len(group)
            assert list(read(exported, indices, length)) == group
        for address, before in [(PARENTS, parent_header), (SCRATCH, scratch_header)]:
            assert read(exported, address, 3) == (before[0], 0, before[2])
        assert read(exported, OBJECT + 8, 1)[0] == 0
        _, releases, _ = compare_step(machines, 0x372300, this=GROUPS, seed=seed)
        assert len([event for event in releases if event[:2] == ("release", 1)]) == size
