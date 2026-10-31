"""Owner gathering through real insertion, filtering, growth and cleanup.

Only allocator virtuals and the part-kind virtual are synthetic boundaries.
The fixed member always supplies the first-list helper's required initial entry.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OWNER, OUT, FIXED = HEAP, HEAP + 0x200, HEAP + 0x400
MEMBERS = [HEAP + 0x500 + 0x100 * i for i in range(4)]
FIRST, SECOND, OWN, OUTPUT = [HEAP + n for n in (0x1000, 0x1100, 0x1800, 0x2000)]
ENTRIES, PARTS, PART_TABLE = [HEAP + n for n in (0x3000, 0x4000, 0x4800)]
MEMORY, MEMORY_TABLE = HEAP + 0x5000, HEAP + 0x5100
ALLOCATE, RELEASE, KIND = STOP + 16, STOP + 32, STOP + 48


def symbol(mapping, name):
    values = {s.va for s in mapping.find(name)}
    assert len(values) == 1, (name, values)
    return values.pop()


def initial(seed, grow, alias):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(MEMORY, 'I', MEMORY_TABLE)
    put(MEMORY_TABLE + 16, '2I', ALLOCATE, RELEASE)
    put(PART_TABLE + 32, 'I', KIND)
    for i in range(32):
        put(ENTRIES + i * 16, '4I', seed, i, 0, (seed + i) % 2)
        put(PARTS + i * 16, '4I', PART_TABLE, [0, 11, 3, 11, 0xffffffff][(seed + i) % 5], i, seed)
    for i, member in enumerate([FIXED] + MEMBERS):
        block = HEAP + 0x1200 + i * 0x100
        counts = [1 + seed % 4 if i == 0 else (seed + i) % 4,
                  (seed // 2 + i) % 4, (seed // 3 + i) % 5]
        for off, delta, count in zip((0x3c, 0x4c, 0x58), (0, 0x40, 0x80), counts):
            put(member + off, '3I', block + delta, count, 0x80000010)
            for j in range(8):
                value = PARTS + ((i * 5 + j) % 32) * 16 if off == 0x58 else ENTRIES + ((i * 7 + j) % 32) * 16
                put(block + delta + 4 * j, 'I', value)
    put(FIRST, '4I', MEMBERS[0], MEMBERS[1], MEMBERS[0], MEMBERS[3])
    put(SECOND, '4I', MEMBERS[2], MEMBERS[3], MEMBERS[1], MEMBERS[2])
    put(OWNER + 8, '3I', FIRST, seed % 4, 0x80000004)
    put(OWNER + 0x14, '3I', SECOND, seed // 4 % 4, 0x80000004)
    put(OWNER + 0x2c, 'I', FIXED)
    put(OWNER + 0xd0, 'I', 0 if seed % 3 == 0 else ENTRIES + 31 * 16)
    own_count = [-2, 0, 1, 3, 5][seed % 5]
    put(OWNER + 0x120, 'Ii', OUTPUT if alias else OWN, own_count)
    for i in range(64):
        put(OUTPUT + i * 4, 'I', ENTRIES + (i % 32) * 16)
        if i < 8:
            put(OWN + i * 4, 'I', ENTRIES + (31 - i) * 16)
    put(OUT, '3I', OUTPUT, seed % 3, 2 if grow else 0x80000080)
    return bytes(data)


class Run:
    def __init__(self, uc, entry, allocator, helpers, seed, grow, alias, mutate, precision):
        self.uc, self.entry, self.helpers = uc, entry, helpers
        self.seed, self.grow, self.mutate, self.precision = seed, grow, mutate, precision
        self.events, self.calls, self.live = [], {name: 0 for name in helpers.values()}, {}
        self.next_block, self.kinds = HEAP + 0x6000, 0
        if grow:
            self.live[OUTPUT] = 8
        uc.mem_write(HEAP, initial(seed, grow, alias))
        uc.mem_write(STOP, b'\xc3' * 4096)
        try:
            uc.mem_map(0, 4096)
        except Exception:
            pass
        self.put(0, 'I', 0x12345678)
        self.put(allocator, 'I', MEMORY)
        self.hook_id = uc.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, count=1):
        return struct.unpack('<' + 'I' * count, self.uc.mem_read(at, 4 * count))

    def put(self, at, fmt, *values):
        self.uc.mem_write(at, struct.pack('<' + fmt, *values))

    def done(self, sp, cleanup, result=0xabcdef01):
        for reg, value in [(r.UC_X86_REG_EAX, result), (r.UC_X86_REG_ECX, 0xdead1234),
                           (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(reg, value)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def hook(self, uc, pc, size, unused):
        if pc in self.helpers:
            self.calls[self.helpers[pc]] += 1
        if pc not in (ALLOCATE, RELEASE, KIND):
            return
        sp, receiver = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == KIND:
            assert PARTS <= receiver < PARTS + 512 and receiver % 16 == 0
            kind = self.words(receiver + 4)[0]
            self.events.append(('kind', receiver, kind))
            self.kinds += 1
            if self.mutate and self.kinds == 1:
                self.put(OWNER + 0xd0, 'I', ENTRIES + 13 * 16)
                # Later member traversals reload their limits and part pointers.
                if self.seed % 4 and (self.seed // 3 + 1) % 5:
                    self.put(OWNER + 0x18, 'I', 0)
                self.put(MEMBERS[0] + 0x5c, 'I', 0)
                self.put(HEAP + 0x1384, 'I', PARTS + 27 * 16)
            self.done(sp, 0, kind)
        elif pc == ALLOCATE:
            assert receiver == MEMORY
            size, tag = self.words(sp + 4, 2)
            assert 0 < size <= 1024 and tag == 0x12
            block = self.next_block
            self.next_block += (size + 63) & ~63
            assert self.next_block < HEAP + 65536
            self.live[block] = size
            self.events.append(('allocate', block, size, tag))
            if self.mutate and len(self.events) == 1:
                self.put(OWNER + 0x124, 'I', 2)
                self.put(OWN, '2I', ENTRIES + 16, ENTRIES + 32)
            self.done(sp, 8, block)
        else:
            assert receiver == MEMORY
            block, size, tag = self.words(sp + 4, 3)
            assert tag == 0x12 and self.live.pop(block) == size
            self.events.append(('release', block, size, tag))
            self.done(sp, 12)

    def run(self):
        uc, sp = self.uc, STACK + 0x8000 + self.seed % 4 * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(sp, '3I', STOP, OWNER, OUT)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: 0,
                           r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
                           r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0,
                           r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=150000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 4
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(0)[0] == 0x12345678
            data, count, flags = self.words(OUT, 3)
            assert count < 128
            assert self.live == ({} if flags & 0x80000000 else {data: flags * 4})
            assert self.calls['first'] == self.calls['parts'] == self.calls['actions'] == 1
            assert self.calls['insert'] >= 4
            return bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), tuple(self.calls.items()), tuple(self.live.items())
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('grow,alias,mutate', [(False, False, False), (True, False, False),
    (False, True, False), (True, True, False), (False, False, True), (True, False, True)])
def test_owner_gathering(solver_images, precision, grow, alias, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x30d480, symbol(mapping, 'function_30d480')]
    allocators = [0x480118, symbol(mapping, 'g_480118')]
    names = {0x30c650: 'insert', 0x30ced0: 'first', 0x30cf70: 'parts', 0x30d0c0: 'actions',
             0x2d9160: 'reserve', 0x2d91d0: 'grow', 0x2d8db0: 'copy'}
    helpers = [names, {symbol(mapping, ('pointers_30c650::' if va == 0x30c650 else '') +
                             'function_%x' % va): name for va, name in names.items()}]
    for seed in range(96):
        results = [Run(machine, entry, allocator, helper, seed, grow, alias, mutate, precision).run()
                   for machine, entry, allocator, helper in zip((retail, compiled), entries, allocators, helpers)]
        assert results[0] == results[1], (seed, grow, alias, mutate, precision)
