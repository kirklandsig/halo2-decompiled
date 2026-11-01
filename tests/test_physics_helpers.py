"""Optional retail differential checks for the shared physics support batch.

Needs Unicorn, a retail XBE and a current full build. Allocator, admission and
pair callbacks are mocked at their observed ABI; the helper bodies and their
direct dependencies execute from each image without patching their code.
"""
import json
import math
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
OBJECT, INPUT, OUTPUT = HEAP + 0x1000, HEAP + 0x2000, HEAP + 0x2400
ADDED, REMOVED, NODES = HEAP + 0x2800, HEAP + 0x3000, HEAP + 0x4000
HANDLERS, FILTER, ALLOCATOR, BLOCK = HEAP + 0x5000, HEAP + 0x5500, HEAP + 0x5600, HEAP + 0x8000
ALLOC, FREE, ACCEPT, ADD, REMOVE, FLOAT_RETURN = [MOCK + x for x in (0, 0x10, 0x20, 0x30, 0x40, 0x100)]
NOTIFY_ADD, NOTIFY_REMOVE = MOCK + 0x50, MOCK + 0x60
NOTIFIERS, NOTIFY_TABLE = HEAP + 0x6500, HEAP + 0xf300
ADD_TABLE, ALLOC_TABLE, FILTER_TABLE = HEAP + 0xf000, HEAP + 0xf100, HEAP + 0xf200
TARGETS = [0x315e70, 0x315ef0, 0x372500, 0x3725e0, 0x372680, 0x3726f0, 0x372730, 0x372760,
           0x372790, 0x3727d0, 0x372800, 0x3728b0, 0x3728c0, 0x372a40, 0x372a80, 0x372b50]
RETAIL_TABLES = {"pointer": 0x43e528, "records": 0x43e52c, "reference": 0x452c30,
                 "handler": 0x43e534, "handler_base": 0x41387c, "world_handler": 0x413888}
CLASSES = {"pointer": "pointer_storage_372680", "records": "record_storage_372800",
           "reference": "referenced_2d88a0", "handler": "empty_slot_372a40",
           "handler_base": "pair_handler_30dc80", "world_handler": "empty_entry_3101c0"}


def put(memory, address, fmt, *values):
    struct.pack_into("<" + fmt, memory, address - HEAP, *values)


def fresh():
    memory = bytearray(b"\xcd" * 0x10000)
    put(memory, ALLOCATOR, "I", ALLOC_TABLE)
    put(memory, ALLOC_TABLE, "6I", 0, 0, 0, 0, ALLOC, FREE)
    put(memory, FILTER, "I", FILTER_TABLE)
    put(memory, FILTER_TABLE, "I", ACCEPT)
    put(memory, ADD_TABLE, "3I", 0, ADD, REMOVE)
    put(memory, NOTIFY_TABLE, "9I", *([0] * 7 + [NOTIFY_ADD, NOTIFY_REMOVE]))
    for i in range(24):
        put(memory, NOTIFIERS + i * 16, "I", NOTIFY_TABLE)
    for i in range(64):
        put(memory, HANDLERS + i * 16, "I", ADD_TABLE)
        put(memory, OBJECT + i * 4, "I", HANDLERS + i * 16)
    return memory


class Machine:
    def __init__(self, image, entries, tables, allocator):
        self.image, self.entries, self.tables = image, entries, tables
        self.uc = unicorn.Uc(unicorn.UC_ARCH_X86, unicorn.UC_MODE_32)
        lo = min(s.va for s in image.sections) & ~4095
        hi = (max(s.va + max(s.vsize, s.rsize) for s in image.sections) + 4095) & ~4095
        self.uc.mem_map(lo, hi - lo)
        for section in image.sections:
            self.uc.mem_write(section.va, image.data[section.raw:section.raw + section.rsize])
        for base, size in [(0, 4096), (HEAP, 0x10000), (STACK, 0x10000), (STOP, 0x3000)]:
            self.uc.mem_map(base, size)
        self.uc.mem_write(allocator, struct.pack("<I", ALLOCATOR))
        for address, cleanup in [(ALLOC, 8), (FREE, 12), (ACCEPT, 12), (ADD, 4), (REMOVE, 4),
                                 (NOTIFY_ADD, 8), (NOTIFY_REMOVE, 4)]:
            self.uc.mem_write(address, b"\xc2" + struct.pack("<H", cleanup))
        self.uc.mem_write(FLOAT_RETURN, b"\xd9\x1d" + struct.pack("<I", OUTPUT) +
                          b"\xe9" + struct.pack("<i", STOP - FLOAT_RETURN - 11))
        self.uc.hook_add(unicorn.UC_HOOK_CODE, self.callback)

    def words(self, address, count):
        return struct.unpack("<" + "I" * count, self.uc.mem_read(address, count * 4))

    def callback(self, machine, address, size, unused):
        if address not in (ALLOC, FREE, ACCEPT, ADD, REMOVE, NOTIFY_ADD, NOTIFY_REMOVE):
            return
        sp = machine.reg_read(reg.UC_X86_REG_ESP)
        this = machine.reg_read(reg.UC_X86_REG_ECX)
        result = 0xbad0cafe
        if address == ALLOC:
            assert this == ALLOCATOR
            length, tag = self.words(sp + 4, 2)
            self.events.append(("allocate", length, tag))
            result = BLOCK
        elif address == FREE:
            assert this == ALLOCATOR
            self.events.append(("release", *self.words(sp + 4, 3)))
        elif address == ACCEPT:
            assert this == FILTER
            result, first, second = self.words(sp + 4, 3)
            allowed = False
            if first and second:
                a, b = self.words(first + 0x14, 1)[0], self.words(second + 0x14, 1)[0]
                allowed = ((a ^ b ^ self.seed) & 3) != 0
                if allowed and self.seed % 3 == 0:
                    kind = self.words(first + 0x18, 1)[0]
                    machine.mem_write(first + 0x18, struct.pack("<I", (kind + 1) & 7))
            self.events.append(("accept", first, second, allowed))
            machine.mem_write(result, bytes([allowed]))
        elif address in (NOTIFY_ADD, NOTIFY_REMOVE):
            other = self.words(sp + 4, 1)[0]
            assert NOTIFIERS <= this < NOTIFIERS + 24 * 16 and (this - NOTIFIERS) % 16 == 0
            if address == NOTIFY_ADD:
                flag = self.words(sp + 8, 1)[0] & 0xff
                self.events.append(("notify_add", this, other, flag))
            else:
                self.events.append(("notify_remove", this, other))
        else:
            pair = self.words(sp + 4, 1)[0]
            first, second = self.words(pair, 2)
            a, b = self.words(first + 8, 1)[0], self.words(second + 8, 1)[0]
            assert this == HANDLERS + (a * 8 + b) * 16
            self.events.append(("add" if address == ADD else "remove", a * 8 + b, pair, first, second))
        machine.reg_write(reg.UC_X86_REG_EAX, result)
        machine.reg_write(reg.UC_X86_REG_ECX, 0xfeed1234)
        machine.reg_write(reg.UC_X86_REG_EDX, 0xface5678)

    def run(self, entry, memory, args=(), this=OBJECT, cleanup=0, precision=0x37f,
            seed=0, capture_float=False):
        self.events, self.seed = [], seed
        self.uc.mem_write(HEAP, bytes(memory))
        self.uc.mem_write(STACK, b"\xcc" * 0x10000)
        self.uc.mem_write(0, struct.pack("<I", 0xffffffff))
        sp = STACK + 0x8000 + (seed & 3) * 4
        target = FLOAT_RETURN if capture_float else STOP
        self.uc.mem_write(sp, struct.pack("<" + "I" * (len(args) + 1), target, *args))
        preserved = {reg.UC_X86_REG_EBX: 0x12345678, reg.UC_X86_REG_ESI: 0x23456789,
                     reg.UC_X86_REG_EDI: 0x3456789a, reg.UC_X86_REG_EBP: 0x456789ab}
        for register, value in preserved.items():
            self.uc.reg_write(register, value)
        for register in [reg.UC_X86_REG_EAX, reg.UC_X86_REG_EDX]:
            self.uc.reg_write(register, 0)
        self.uc.reg_write(reg.UC_X86_REG_ECX, this)
        self.uc.reg_write(reg.UC_X86_REG_ESP, sp)
        self.uc.reg_write(reg.UC_X86_REG_EFLAGS, 2)
        self.uc.reg_write(reg.UC_X86_REG_FPCW, precision)
        self.uc.reg_write(reg.UC_X86_REG_FPTAG, 0xffff)
        self.uc.reg_write(reg.UC_X86_REG_FPSW, 0)
        self.uc.reg_write(reg.UC_X86_REG_MXCSR, 0x1f80)
        self.uc.emu_start(entry, STOP, count=200000)
        assert self.uc.reg_read(reg.UC_X86_REG_EIP) == STOP
        assert self.uc.reg_read(reg.UC_X86_REG_ESP) == sp + 4 + cleanup
        assert self.words(0, 1)[0] == 0xffffffff
        for register, value in preserved.items():
            assert self.uc.reg_read(register) == value
        return (bytearray(self.uc.mem_read(HEAP, len(memory))), tuple(self.events),
                self.uc.reg_read(reg.UC_X86_REG_EAX))

    def normalize(self, memory, fields):
        for address, kind in fields:
            pointer = struct.unpack_from("<I", memory, address - HEAP)[0]
            assert pointer == self.tables[kind], (kind, hex(pointer), hex(self.tables[kind]))
            put(memory, address, "I", 0)
        return bytes(memory)


@pytest.fixture(scope="module")
def machines():
    path = Path(retail_xbe_path())
    if not path.exists() or not (ROOT / "build/halo2.exe").exists():
        pytest.skip("retail XBE and full build required")
    report = json.loads((ROOT / "build/report.json").read_text())
    mapping = LinkMap.read(ROOT / "build/halo2.map")
    entries = {}
    for va in TARGETS:
        name = report.get(f"{va:08x}", {}).get("name", "")
        candidates = [s for s in mapping.find(name) if "ABV0@@Z" not in s.name] if name else []
        if not candidates:
            pytest.skip("full build does not contain the physics support batch")
        assert len(candidates) == 1
        entries[va] = candidates[0].va
    tables = {}
    for kind, name in CLASSES.items():
        matches = {s.va for s in mapping.symbols if s.name.startswith("??_7" + name + "@@")}
        assert len(matches) == 1, (kind, matches)
        tables[kind] = matches.pop()
    allocator = mapping.find("g_480118")
    assert len(allocator) == 1
    retail = Machine(Xbe(path), {v: v for v in TARGETS}, RETAIL_TABLES, 0x480118)
    compiled = Machine(Pe(ROOT / "build/halo2.exe"), entries, tables, allocator[0].va)
    # The factories must install the actual emitted deleting destructors.
    for machine in [retail, compiled]:
        for kind, va in [("pointer", 0x372730), ("records", 0x3727d0)]:
            assert machine.words(machine.tables[kind], 1)[0] == machine.entries[va]
        assert machine.words(machine.tables["world_handler"] + 4, 2) == (
            machine.entries[0x315e70], machine.entries[0x315ef0])
    return retail, compiled


def compare(machines, va, memory, args=(), cleanup=0, precision=0x37f, seed=0,
            fields=(), return_value=False, capture_float=False):
    results = []
    for machine in machines:
        initial = bytearray(memory)
        if va in (0x372680, 0x372730):
            put(initial, OBJECT, "I", machine.tables["pointer"])
        elif va in (0x372800, 0x3727d0):
            put(initial, OBJECT, "I", machine.tables["records"])
        elif va == 0x3728b0:
            put(initial, OBJECT + 0x100, "I", machine.tables["handler"])
        result, events, eax = machine.run(machine.entries[va], initial, args, cleanup=cleanup,
                                          precision=precision, seed=seed, capture_float=capture_float)
        results.append((machine.normalize(result, fields), events, eax if return_value else None))
    assert results[0] == results[1], (hex(va), seed, precision)


@pytest.mark.parametrize("va", [0x372500, 0x3725e0])
@pytest.mark.parametrize("precision", [0x7f, 0x27f, 0x37f])
def test_physics_math(machines, va, precision):
    edges = [0.0, -0.0, 1.0, -1.0, 1e-20, -1e-20, 1e10, -1e10, math.inf, -math.inf, math.nan]
    for seed in range(128):
        rng = random.Random(seed)
        memory = fresh()
        if va == 0x372500:
            source = [INPUT, OBJECT, OBJECT + 16][seed % 3]
            values = [rng.uniform(-2, 2) for _ in range(4)]
            if seed < len(edges):
                values[seed % 4] = edges[seed]
            put(memory, source, "4f", *values)
            compare(machines, va, memory, (source,), 4, precision, seed)
        else:
            values = (edges[seed % len(edges)], edges[(seed // len(edges)) % len(edges)]) if seed < 121 else (
                rng.uniform(-100, 100), rng.uniform(-100, 100))
            args = struct.unpack("<II", struct.pack("<ff", *values))
            compare(machines, va, memory, args, precision=precision, seed=seed, capture_float=True)


def pair_case(seed):
    rng = random.Random(seed)
    memory = fresh()
    handles = []
    for i in range(24):
        handle = NODES + i * 64 + 16
        key = rng.choice([0, 1, 2, 7, 0x7fffffff, 0x80000000, 0xffffffff])
        put(memory, handle, "3I", 0, key, rng.randrange(8))
        put(memory, handle + 16, "I", NOTIFIERS + i * 16)
        handles.append((key, handle))
    pairs = []
    for i in range(seed % 25):
        a, b = rng.choice(handles), rng.choice(handles)
        pairs.append((a, b))
    return rng, memory, pairs


@pytest.mark.parametrize("va", [0x372a80, 0x372b50, 0x3728c0])
def test_pair_processing(machines, va):
    for seed in range(192):
        rng, memory, pairs = pair_case(seed)
        if va == 0x372b50:
            for i, (a, b) in enumerate(pairs):
                put(memory, ADDED + i * 8, "2I", a[1], b[1])
            count = len(pairs) if seed % 31 else -1
            compare(machines, va, memory, (ADDED, count & 0xffffffff), seed=seed)
        elif va == 0x372a80:
            if not pairs:
                pairs = [((0, NODES + 16), (0, NODES + 16))]
            for i, (a, b) in enumerate(pairs):
                put(memory, ADDED + i * 8, "2I", a[1], b[1])
            low = rng.randrange(len(pairs))
            high = rng.randrange(low, len(pairs))
            compare(machines, va, memory, (ADDED, low, high, 0), seed=seed)
        else:
            pairs = [(a, b) if a[0] <= b[0] else (b, a) for a, b in pairs]
            added = sorted([p for p in pairs if rng.randrange(3)], key=lambda p: (p[0][0], p[1][0]))
            removed = sorted([p for p in pairs if rng.randrange(3)], key=lambda p: (p[0][0], p[1][0]))
            for address, array in [(ADDED, added), (REMOVED, removed)]:
                for i, (a, b) in enumerate(array):
                    put(memory, address + i * 8, "2I", a[1], b[1])
            compare(machines, va, memory, (ADDED if added else 0, len(added),
                    REMOVED if removed else 0, len(removed), FILTER if added else 0), 20, seed=seed)


@pytest.mark.parametrize("va", [0x372680, 0x3726f0, 0x372730, 0x372760, 0x372790,
                               0x3727d0, 0x372800, 0x3728b0, 0x372a40])
def test_storage_and_tables(machines, va):
    for seed in range(64):
        memory = fresh()
        count, capacity = seed % 7, seed % 7 + seed % 3
        flags = capacity | (0x80000000 if seed & 1 else 0)
        fields, args, cleanup, returns = [], (), 0, False
        if va in (0x3726f0, 0x372790):
            fields = [(BLOCK, "pointer" if va == 0x3726f0 else "records")]
            returns = True
        elif va in (0x372680, 0x372730):
            put(memory, OBJECT + 4, "HH3I", 0x14, 1, INPUT, count, flags)
            fields = [(OBJECT, "reference")]
            if va == 0x372730:
                args, cleanup, returns = (seed & 3,), 4, True
        elif va == 0x372760:
            put(memory, OBJECT, "3I", INPUT, count, flags)
        elif va in (0x372800, 0x3727d0):
            put(memory, OBJECT + 4, "HHI3I", 0x18, 1, 0, INPUT, count, flags)
            uses = [0] * 3
            for i in range(count):
                uses[i % 3] += 1
                put(memory, INPUT + i * 12, "3I", NODES + (i % 3) * 128 + 12, 0, 0)
            for i, use_count in enumerate(uses):
                put(memory, NODES + i * 128, "3I", 0, 10 + i, max(0, use_count - 1) + seed % 3)
            fields = [(OBJECT, "reference")]
            if va == 0x3727d0:
                args, cleanup, returns = (seed & 3,), 4, True
        else:
            fields = [(OBJECT + 0x100, "handler" if va == 0x372a40 else "handler_base")]
        compare(machines, va, memory, args, cleanup, seed=seed, fields=fields, return_value=returns)


@pytest.mark.parametrize("slot", [0, 1, 2])
@pytest.mark.parametrize("kind", ["handler", "world_handler"])
def test_embedded_handler_contract(machines, slot, kind):
    for seed in range(64):
        results = []
        for machine in machines:
            memory = fresh()
            # A standalone instance of the same empty handler: deleting it
            # with flag 1 is valid, unlike deleting the embedded member.
            put(memory, INPUT, "I", machine.tables[kind])
            first, second = NODES + 16, NODES + 80
            put(memory, first, "3I", 0, 1, 2 if seed & 1 else 0)
            put(memory, second, "3I", 0, 2, 2 if seed & 2 else 0)
            put(memory, first + 16, "I", NOTIFIERS)
            put(memory, second + 16, "I", NOTIFIERS + 16)
            put(memory, ADDED, "2I", first, second)
            entry = machine.words(machine.tables[kind] + slot * 4, 1)[0]
            args = (seed & 3,) if slot == 0 else (ADDED,)
            output, events, eax = machine.run(entry, memory, args, this=INPUT, cleanup=4, seed=seed)
            final_kind = "handler_base" if slot == 0 else kind
            results.append((machine.normalize(output, [(INPUT, final_kind)]), events, eax if slot == 0 else None))
        assert results[0] == results[1], (kind, slot, seed)
