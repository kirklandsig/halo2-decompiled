"""Retail differential tests for island constraint/action registration.

The two world setup operations and object virtual callbacks are boundaries.
Array growth, byte copying, action transfer and stack-array lifetime execute
their real bodies; only the allocator's virtual operations are synthetic.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

WORLD, MEMBER, ISLAND, FIXED, ADDED = [HEAP + x for x in (0, 0x200, 0x400, 0x500, 0x600)]
PEERS, CONSTRAINTS, ACTIONS = [HEAP + x for x in (0x1000, 0x2000, 0x3000)]
ADDED_DATA, LINKS, ACTION_DATA, FIXED_DATA = [HEAP + x for x in (0x800, 0x900, 0xa00, 0xb00)]
PAIR_DATA, ISLAND_LINKS, ISLAND_ACTIONS = [HEAP + x for x in (0x4000, 0x4400, 0x4800)]
TABLES, MEMORY, BLOCK, ALLOCATIONS = [HEAP + x for x in (0x5000, 0x5800, 0x6000, 0x8000)]
TYPE, INFO, ADD, MEMBERS, ALLOCATE, RELEASE, OVERFLOW, UNSTACK = [STOP + x for x in range(16, 144, 16)]


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, grow, overflow):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    counts = [-2, 0, 1, 2, 4, 6]
    put(WORLD + 0x20, '3I', PAIR_DATA, seed % 3, (seed % 3 if grow else 128) | 0x80000000)
    put(WORLD + 0x2c, 'I', FIXED)
    put(WORLD + 0x12c, 'B', [0, 1, 127, 128, 255][seed % 5])
    put(MEMBER + 0x40, 'B', int(seed % 11 == 0))
    put(MEMBER + 0x44, 'I', ISLAND)
    put(MEMBER + 0x88, 'Ii', LINKS, counts[seed % 6])
    put(MEMBER + 0x94, 'Ii', ACTION_DATA, counts[seed // 3 % 6])
    put(ADDED, 'IiI', ADDED_DATA, counts[seed // 2 % 6], 0x80000008)
    for index in range(8):
        put(PEERS + index * 0x100 + 0x40, 'B', int((index + seed) % 3 == 0))
        constraint, action = CONSTRAINTS + index * 0x40, ACTIONS + index * 0x40
        first = MEMBER if (index + seed) % 2 else PEERS + index * 0x100
        second = PEERS + ((index + 1) % 8) * 0x100
        put(constraint, 'I', TABLES)
        put(constraint + 8, 'I', index)
        put(constraint + 12, '3I', first, second, FIXED)
        put(action, 'I', TABLES + 0x80)
        put(action + 8, 'I', index)
        put(action + 16, 'I', FIXED)
        put(ADDED_DATA + index * 4, 'I', constraint)
        put(LINKS + index * 4, 'I', constraint)
        put(ACTION_DATA + index * 4, 'I', action)
        put(FIXED_DATA + index * 4, 'I', ACTIONS + (7 - index) * 0x40)
    for island in (ISLAND, FIXED):
        put(island, 'I', TABLES + 0xc0)
        put(island + 0x28, 'B', 0)
    put(ISLAND + 0x4c, '3I', ISLAND_ACTIONS, 0, 0x80000000 | (0 if grow else 32))
    put(ISLAND + 0x58, '3I', ISLAND_LINKS, 0, 0x80000000 | (0 if grow else 32))
    put(FIXED + 0x4c, '3I', FIXED_DATA, 8, 0x80000008)
    put(TABLES + 8 * 4, 'I', TYPE)
    put(TABLES + 10 * 4, 'I', INFO)
    put(TABLES + 0x80 + 6 * 4, 'I', MEMBERS)
    put(TABLES + 0xc0 + 2 * 4, 'I', ADD)
    put(TABLES + 0x100 + 4 * 4, '2I', ALLOCATE, RELEASE)
    put(TABLES + 0x100 + 9 * 4, '2I', OVERFLOW, UNSTACK)
    put(MEMORY, '6I', TABLES + 0x100, 0, BLOCK, 0 if overflow else 4096,
        0, BLOCK if overflow else BLOCK + 4096)
    return data


class Run:
    def __init__(self, machine, entry, allocator, boundaries, helpers, seed, grow, overflow, mutate, precision):
        self.uc, self.entry, self.boundaries, self.helpers = machine, entry, boundaries, helpers
        self.seed, self.overflow, self.mutate, self.precision = seed, overflow, mutate, precision
        self.events, self.live, self.freed = [], {}, set()
        self.helper_calls = {name: 0 for name in helpers.values()}
        self.next_block = ALLOCATIONS
        self.flag = [0, 1, 127, 128, 255][seed % 5]
        machine.mem_write(HEAP, bytes(initial(seed, grow, overflow)))
        machine.mem_write(STOP, b'\xc3' * 4096)
        try:
            machine.mem_map(0, 4096)
        except UcError:
            pass
        self.put(allocator, 'I', MEMORY)
        self.hook_id = machine.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, count=1):
        return struct.unpack('<' + 'I' * count, self.uc.mem_read(at, count * 4))

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
            self.helper_calls[self.helpers[pc]] += 1
        if pc not in self.boundaries and pc not in (TYPE, INFO, ADD, MEMBERS, ALLOCATE, RELEASE, OVERFLOW, UNSTACK):
            return
        sp, this = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc in self.boundaries:
            assert this == WORLD and self.words(sp + 4)[0] == MEMBER
            assert bytes(uc.mem_read(WORLD + 0x12c, 1)) == b'\x01'
            self.events.append((self.boundaries[pc], this, MEMBER))
            # Registration must load the island after both setup calls.
            self.put(MEMBER + 0x44, 'I', ISLAND)
            self.done(sp, 4)
        elif pc in (ALLOCATE, OVERFLOW):
            assert this == MEMORY
            amount = self.words(sp + 4)[0]
            if pc == ALLOCATE:
                assert self.words(sp + 8)[0] == 0x12 and 0 < amount <= 4096
                result = self.next_block
                self.next_block += (amount + 31) & ~31
                assert self.next_block < HEAP + 65536
                self.live[result] = amount
                self.events.append(('allocate', result, amount, 0x12))
                self.done(sp, 8, result)
            else:
                assert self.overflow and amount == 64
                self.events.append(('overflow', BLOCK, amount))
                if self.mutate and self.seed % 2 == 0:
                    self.put(MEMBER + 0x98, 'I', 4)
                self.done(sp, 4, BLOCK)
        elif pc in (RELEASE, UNSTACK):
            assert this == MEMORY
            block, amount = self.words(sp + 4, 2)
            if pc == RELEASE:
                assert self.words(sp + 12)[0] == 0x12
                assert self.live.pop(block) == amount and block not in self.freed
                self.freed.add(block)
                self.events.append(('release', block, amount, 0x12))
                self.done(sp, 12)
            else:
                assert self.overflow and (block, amount) == (BLOCK, 64)
                self.events.append(('unstack', block, amount))
                self.done(sp, 8)
        elif pc == TYPE:
            index = (this - CONSTRAINTS) // 0x40
            assert 0 <= index < 8
            kind = [0, 1, 10, 11, 12, 0xffffffff][(index + self.seed) % 6]
            self.events.append(('type', index, kind))
            if self.mutate and index == 0:
                # The tested body must reload this slot after the type callback.
                self.put(LINKS, 'I', CONSTRAINTS + 7 * 0x40)
                self.put(MEMBER + 0x8c, 'I', 4)
            self.done(sp, 0, kind)
        elif pc == INFO:
            index = (this - CONSTRAINTS) // 0x40
            output = self.words(sp + 4)[0]
            assert 0 <= index < 8
            values = (index, self.seed, 0x12345678, 0x87654321)
            self.put(output, '4I', *values)
            self.events.append(('info', index, values))
            if self.mutate and index == 0:
                self.put(ADDED_DATA, 'I', CONSTRAINTS + 7 * 0x40)
            self.done(sp, 4)
        elif pc == ADD:
            constraint, info = self.words(sp + 4, 2)
            assert this == ISLAND
            self.events.append(('add', constraint, self.words(info, 4)))
            if self.mutate and self.seed % 3 == 0:
                self.put(ADDED + 4, 'I', 1)
            # In particular the owner store must reload after this call.
            if self.mutate and constraint == CONSTRAINTS + 7 * 0x40:
                self.put(ADDED_DATA, 'I', CONSTRAINTS + 6 * 0x40)
            self.done(sp, 8)
        else:
            index = (this - ACTIONS) // 0x40
            assert 0 <= index < 8
            output = self.words(sp + 4)[0]
            block, old_count, capacity = self.words(output, 3)
            assert block == BLOCK and capacity == 0x80000010
            # Some callbacks append to the previous list instead of resetting it.
            values = [MEMBER, PEERS + ((self.seed + index) % 8) * 0x100,
                      PEERS + ((self.seed + index + 3) % 8) * 0x100]
            start = old_count if self.seed % 5 == 0 and old_count <= 9 else 0
            for n, peer in enumerate(values):
                self.put(block + (start + n) * 4, 'I', peer)
            self.put(output + 4, 'I', start + len(values))
            self.events.append(('members', index, old_count, tuple(values), start))
            if self.mutate and index == 0:
                self.put(MEMBER + 0x98, 'I', 3)
                self.put(ACTION_DATA + 4, 'I', ACTIONS + 7 * 0x40)
            self.done(sp, 4)

    def run(self):
        uc = self.uc
        sp = STACK + 0x8000 + self.seed % 4 * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(0, 'I', 0xffffffff)
        self.put(sp, '3I', STOP, MEMBER, ADDED)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: WORLD,
                           r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
                           r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0,
                           r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=200000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 12
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(0)[0] == 0xffffffff
            assert bytes(uc.mem_read(WORLD + 0x12c, 1)) == bytes([self.flag])
            assert self.words(MEMORY + 8, 2) == (BLOCK, 0 if self.overflow else 4096)
            assert self.helper_calls['construct'] == self.helper_calls['destroy'] == 1
            assert self.helper_calls['attach'] == self.helper_calls['detach']
            assert sum(e[0] == 'overflow' for e in self.events) == int(self.overflow)
            assert sum(e[0] == 'unstack' for e in self.events) == int(self.overflow)
            owned = {self.words(at)[0] for at in (WORLD + 0x20, ISLAND + 0x4c, ISLAND + 0x58)
                     if self.words(at + 8)[0] & 0x80000000 == 0}
            assert owned == set(self.live)
            pairs, count = self.words(WORLD + 0x20, 2)
            pair_values = tuple(self.words(pairs + n * 8, 2) for n in range(count))
            return (bytes(uc.mem_read(HEAP, 65536)), tuple(self.events),
                    tuple(self.helper_calls.items()), pair_values)
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('grow,overflow', [(False, False), (True, False), (True, True)])
@pytest.mark.parametrize('mutate', [False, True])
def test_island_registration(solver_images, precision, grow, overflow, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x30f380, symbol(mapping, 'owner_30c8e0::function_30f380')]
    allocators = [0x480118, symbol(mapping, 'g_480118')]
    boundaries = [{0x30c8e0: 'setup_first', 0x30df80: 'setup_second'},
                  {symbol(mapping, 'owner_30c8e0::function_30c8e0'): 'setup_first',
                   symbol(mapping, 'owner_30c8e0::function_30df80'): 'setup_second'}]
    names = {'grow': 'function_2d9160', 'double': 'function_2d91d0',
             'copy': 'function_2d8db0', 'attach': 'class_316c50::function_316d60',
             'detach': 'list_owner_3169c0::function_3169c0',
             'construct': 'longs_30d740::longs_30d740', 'destroy': 'longs_30d740::~longs_30d740'}
    helpers = [dict(zip((0x2d9160, 0x2d91d0, 0x2d8db0, 0x316d60, 0x3169c0, 0x30d740, 0x30d7d0), names)),
               {symbol(mapping, name): kind for kind, name in names.items()}]
    invoked, kinds, self_pair = set(), set(), False
    for seed in range(96):
        results = [Run(machine, entry, allocator, boundary, helper, seed, grow, overflow, mutate, precision).run()
                   for machine, entry, allocator, boundary, helper in
                   zip((retail, compiled), entries, allocators, boundaries, helpers)]
        assert results[0] == results[1], (seed, grow, overflow, mutate, precision)
        invoked.update(name for name, count in results[0][2] if count)
        kinds.update(e[2] for e in results[0][1] if e[0] == 'type')
        self_pair |= (MEMBER, MEMBER) in results[0][3]
    assert kinds == {0, 1, 10, 11, 12, 0xffffffff}
    assert self_pair
    assert {'attach', 'detach', 'construct', 'destroy'} <= invoked
    if grow:
        assert invoked == set(names)
