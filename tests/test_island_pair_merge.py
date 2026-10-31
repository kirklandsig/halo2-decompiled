"""Retail island pair merging and real island transfer, with virtual boundaries.

Array growth/copy and stack lifetimes execute normally. Only allocator,
activation, agent, and island-deletion callbacks are synthetic.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

WORLD, FIRST, SECOND, LAST = [HEAP + x for x in (0, 0x200, 0x400, 0x600)]
DEST, SOURCE = FIRST + 0x68, SECOND + 0x68
ACTIVE, PAIRS_A, PAIRS_B, ALTERNATE = [HEAP + x for x in (0x800, 0x1000, 0x1400, 0x1800)]
BODIES, AGENTS, MEMBERS, TABLE, MEMORY, BLOCK = [HEAP + x for x in (0x2000, 0x3000, 0x4000, 0x6000, 0x6200, 0x7000)]
ALLOCATE, RELEASE, OVERFLOW, UNSTACK, AGENT_FIRST, AGENT_SECOND, DELETE = [STOP + x for x in range(16, 128, 16)]


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, overflow, grow):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    counts = [0, 1, 2, 4, 7, 14]
    na, nb = counts[seed % 6], counts[seed // 6 % 6]
    put(WORLD + 8, '3I', ACTIVE, 3, 0x80000004)
    put(ACTIVE, '3I', FIRST, SECOND, LAST)
    ids = [0, 1, 2, 3, 0x7fffffff, 0x80000000, 0xfffffffe, 0xffffffff]
    for side, island, pairs, count in [(0, FIRST, PAIRS_A, na), (1, SECOND, PAIRS_B, nb)]:
        put(island, 'I', TABLE)
        put(island + 8, '5I', seed + side, 0xffffffff - side, seed * 3, side + 9, 0xfffffffe)
        put(island + 0x24, 'I', side)
        put(island + 0x28, '2B', [0, 1, 127, 255][(seed + side) % 4], (seed >> side) & 1)
        put(island + 0xae, 'B', seed // 2 % 2)
        put(island + 0x68, '3I', pairs, count, 0x80000000 | (count if grow else 32))
        # Sorted but independently allocated bodies allow equal IDs at different addresses.
        ranks = [(i * 2 + side) if seed % 4 == 0 else i + (side * 6 if seed % 4 == 1 else 0) for i in range(16)]
        if seed % 8 >= 4:
            ranks = [31 - rank for rank in ranks]
        keys = sorted((ids[rank // 4], ids[rank % 4 + (4 if seed % 4 == 3 else 0)]) for rank in ranks)
        for i, (first_id, second_id) in enumerate(keys):
            body = BODIES + side * 0x400 + i * 32
            agent = AGENTS + side * 0x400 + i * 32
            put(body, '4I', 0x12340000 + i, first_id, 0x56780000 + i, second_id)
            put(agent, '3I', TABLE + 0x40, side * 16 + i, agent + 16)
            put(agent + 16, '2I', TABLE + 0x80, side * 16 + i)
            put(pairs + i * 16, '4I', body, body + 8, agent, 0x10000 * side + i)
        for n, offset in enumerate((0x3c, 0x4c, 0x58, 0x74)):
            backing = HEAP + 0x5000 + side * 0x400 + n * 0x100
            count = (seed + side + n) % 4
            put(island + offset, '3I', backing, count, 0x80000000 | (count if grow else 16))
            for i in range(4):
                member = MEMBERS + side * 0x800 + n * 0x200 + i * 0x80
                if n == 3:
                    put(backing + i * 8, '2I', member, i + side * 100)
                else:
                    put(backing + i * 4, 'I', member)
                    put(member + (0x44 if n == 0 else 0x10 if n == 1 else 0x14), 'I', island)
                    if n == 0:
                        put(member + 0x58, 'I', i)
    data[ALTERNATE - HEAP:ALTERNATE - HEAP + 256] = data[PAIRS_B - HEAP:PAIRS_B - HEAP + 256]
    put(LAST + 0x24, 'I', 2)
    put(TABLE, 'I', DELETE)
    put(TABLE + 0x40 + 20, 'I', AGENT_FIRST)
    put(TABLE + 0x80 + 16, 'I', AGENT_SECOND)
    put(TABLE + 0x100 + 16, '2I', ALLOCATE, RELEASE)
    put(TABLE + 0x100 + 36, '2I', OVERFLOW, UNSTACK)
    put(MEMORY, '6I', TABLE + 0x100, 0, BLOCK, 0 if overflow else 4096, 0, BLOCK if overflow else BLOCK + 4096)
    return bytes(data)


class Run:
    def __init__(self, uc, entries, allocator, activate, helpers, seed, mode, overflow, mutate, precision):
        self.uc, self.entries, self.activate, self.helpers = uc, entries, activate, helpers
        self.seed, self.mode, self.overflow, self.mutate, self.precision = seed, mode, overflow, mutate, precision
        self.events, self.live, self.expected_second = [], {}, []
        self.calls = {name: 0 for name in helpers.values()}
        self.next_block, self.changed = HEAP + 0x8000, False
        uc.mem_write(HEAP, initial(seed, overflow, seed % 2 == 0))
        uc.mem_write(STOP, b'\xc3' * 4096)
        try:
            uc.mem_map(0, 4096)
        except UcError:
            pass
        self.put(0, 'I', 0x12345678)
        self.put(allocator, 'I', MEMORY)
        self.original_size = self.words(DEST + 4)[0]
        self.hook_id = uc.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, n=1):
        return struct.unpack('<' + 'I' * n, self.uc.mem_read(at, n * 4))

    def put(self, at, fmt, *values):
        self.uc.mem_write(at, struct.pack('<' + fmt, *values))

    def done(self, sp, cleanup, result=0xabcdef01):
        for reg, value in [(r.UC_X86_REG_EAX, result), (r.UC_X86_REG_ECX, 0xdead1234), (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(reg, value)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def hook(self, uc, pc, size, unused):
        if pc in self.helpers:
            self.calls[self.helpers[pc]] += 1
        if pc not in (self.activate, ALLOCATE, RELEASE, OVERFLOW, UNSTACK, AGENT_FIRST, AGENT_SECOND, DELETE):
            return
        sp, this = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == self.activate:
            island = self.words(sp + 4)[0]
            assert this == WORLD and island in (FIRST, SECOND)
            self.events.append(('activate', island))
            self.put(island + 0x29, 'B', 1)
            self.done(sp, 4)
        elif pc == DELETE:
            assert this == SECOND and self.words(sp + 4)[0] == 1
            self.events.append(('delete', this, 1))
            self.put(this + 4, 'I', 0xdead0001)
            self.done(sp, 4)
        elif pc == AGENT_FIRST:
            second = self.words(this + 8)[0]
            self.expected_second.append(second)
            self.events.append(('agent_first', this, second))
            if self.mutate:
                self.put(this + 8, 'I', AGENTS + 16)
                if not self.changed:
                    self.changed = True
                    self.put(SOURCE, 'I', ALTERNATE)
                    count = self.words(SOURCE + 4)[0]
                    if count:
                        self.put(ALTERNATE + (count - 1) * 16 + 12, 'I', 0xabcdef77)
                    self.put(DEST + 4, 'I', 1)
            self.done(sp, 0)
        elif pc == AGENT_SECOND:
            assert self.expected_second.pop(0) == this
            self.events.append(('agent_second', this))
            self.done(sp, 0)
        elif pc in (ALLOCATE, OVERFLOW):
            amount = self.words(sp + 4)[0]
            assert this == MEMORY and 0 < amount < 2048
            if pc == OVERFLOW:
                assert self.overflow and amount == self.original_size * 16
                self.events.append(('overflow', BLOCK, amount))
                if self.mutate:
                    # Constructor capacity is captured before this callback; resize reloads size.
                    self.put(DEST + 4, 'I', self.original_size + 1)
                self.done(sp, 4, BLOCK)
            else:
                assert self.words(sp + 8)[0] == 0x12
                block = self.next_block
                self.next_block += (amount + 31) & ~31
                assert self.next_block < HEAP + 65536
                self.live[block] = amount
                self.events.append(('allocate', block, amount, 0x12))
                self.done(sp, 8, block)
        else:
            block, amount = self.words(sp + 4, 2)
            assert this == MEMORY
            if pc == UNSTACK:
                assert self.overflow and (block, amount) == (BLOCK, self.original_size * 16)
                self.events.append(('unstack', block, amount))
                self.done(sp, 8)
            else:
                assert self.words(sp + 12)[0] == 0x12 and self.live.pop(block) == amount
                self.events.append(('release', block, amount, 0x12))
                self.done(sp, 12)

    def run(self):
        uc, sp = self.uc, STACK + 0x8000
        uc.mem_write(STACK, b'\xcc' * 65536)
        same = self.mode == 'island' and self.seed % 13 == 0
        args = [SOURCE] if self.mode == 'pairs' else [FIRST, FIRST if same else SECOND]
        self.put(sp, 'I' * (len(args) + 1), STOP, *args)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: DEST if self.mode == 'pairs' else 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: WORLD, r.UC_X86_REG_EAX: 0,
            r.UC_X86_REG_EDX: 0, r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
            r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0, r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entries[self.mode], STOP, count=250000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + (4 if self.mode == 'pairs' else 12)
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision and uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(0)[0] == 0x12345678 and not self.expected_second
            assert self.words(MEMORY + 8, 2) == (BLOCK, 0 if self.overflow else 4096)
            assert self.calls['construct'] == self.calls['destroy'] == int(not same)
            if self.mode == 'island':
                assert uc.reg_read(r.UC_X86_REG_EAX) == FIRST
                assert self.words(WORLD + 12)[0] == (3 if same else 2)
                if not same:
                    assert self.words(ACTIVE, 2) == (FIRST, LAST) and self.words(LAST + 0x24)[0] == 1
            # Persistent allocations belong exactly to the surviving destination arrays.
            offsets = (0x68,) if self.mode == 'pairs' else (0x3c, 0x4c, 0x58, 0x68, 0x74)
            owned = {self.words(FIRST + off)[0]: (self.words(FIRST + off + 8)[0] & 0x7fffffff) * (16 if off == 0x68 else 8 if off == 0x74 else 4)
                     for off in offsets if not self.words(FIRST + off + 8)[0] & 0x80000000}
            assert self.live == owned
            return bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), tuple(self.calls.items())
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('mode', ['pairs', 'island'])
@pytest.mark.parametrize('overflow,mutate', [(False, False), (True, False), (True, True)])
def test_island_pair_merge(solver_images, precision, mode, overflow, mutate):
    retail, compiled, mapping = solver_images
    entries = [{'pairs': 0x30ed40, 'island': 0x30f030},
               {'pairs': symbol(mapping, 'function_30ed40'), 'island': symbol(mapping, 'class_30f030::function_30f030')}]
    names = [(0x30d9a0, 'vectors_30d9a0::vectors_30d9a0', 'construct'),
             (0x30da30, 'vectors_30d9a0::~vectors_30d9a0', 'destroy'),
             (0x2d9160, 'function_2d9160', 'grow'), (0x2d8db0, 'function_2d8db0', 'copy')]
    helpers = [{va: label for va, name, label in names}, {symbol(mapping, name): label for va, name, label in names}]
    for seed in range(144):
        result = [Run(uc, entry, allocator, activate, helper, seed, mode, overflow, mutate, precision).run()
                  for uc, entry, allocator, activate, helper in zip((retail, compiled), entries,
                      (0x480118, symbol(mapping, 'g_480118')),
                      (0x30ccd0, symbol(mapping, 'hkWorld::function_30ccd0')), helpers)]
        assert result[0] == result[1], (seed, mode, overflow, mutate, precision)
