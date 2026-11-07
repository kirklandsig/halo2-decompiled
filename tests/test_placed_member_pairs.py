"""Placed-member bounds/pairs with real recorder virtuals and array lifetime.

Shape bounds, broadphase, dispatcher and group collector are explicit synthetic
boundaries. The dispatcher feeds real add/remove virtuals on the temporary
recorder; the contributor-owned group collector implementation is untouched.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

WORLD, MEMBER, SHAPE, MOTION, FINDER, TOLERANCE = [HEAP + x for x in (0, 0x300, 0x400, 0x500, 0x600, 0x700)]
SHAPE_TABLE, FINDER_TABLE, DISPATCH, SECOND_DISPATCH, LISTENER = [HEAP + x for x in (0x800, 0x900, 0xa00, 0xc00, 0xe00)]
OTHERS = [HEAP + 0x1000 + i * 0x40 for i in range(12)]
OWNERS = [HEAP + 0x1800 + i * 0x80 for i in range(12)]
GROUPS = [HEAP + 0x2800 + i * 0x100 for i in range(6)]
MEMORY, MEMORY_TABLE, STACK_BLOCK = [HEAP + x for x in (0x3800, 0x3900, 0x4000)]
ALLOCATE, RELEASE, OVERFLOW, FREE_OVERFLOW, BOUNDS, QUERY, NEXT = [STOP + i * 16 for i in range(1, 8)]


def symbol(mapping, name):
    matches = {s.va for s in mapping.find(name)}
    assert len(matches) == 1, (name, matches)
    return matches.pop()


def initial(seed, overflow):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(WORLD + 0xc4, 'I', FINDER)
    put(WORLD + 0xcc, '2I', TOLERANCE, LISTENER if seed % 3 else 0)
    put(WORLD + 0x138, 'I', DISPATCH)
    put(WORLD + 0x234, 'I', 16)
    put(MEMBER + 12, '3I', 0 if seed % 17 == 0 else SHAPE, 0, MOTION)
    put(MEMBER + 0x40, 'B', seed % 2)
    put(MEMBER + 0x44, 'I', GROUPS[0])
    put(SHAPE, 'I', SHAPE_TABLE)
    put(SHAPE_TABLE + 24, 'I', BOUNDS)
    put(FINDER, 'I', FINDER_TABLE)
    put(FINDER_TABLE + 4, 'I', QUERY)
    put(TOLERANCE + 8, 'I', [0, 0x3f800000, 0xbf800000, 0x7f800000, 0xff800000, 0x7fc12345][seed % 6])
    put(DISPATCH + 0x24, 'I', DISPATCH + 0x100)
    put(SECOND_DISPATCH + 0x24, 'I', SECOND_DISPATCH + 0x100)
    put(MEMORY, '6I', MEMORY_TABLE, 0, STACK_BLOCK, 0 if overflow else 1024, 0,
        STACK_BLOCK if overflow else 0x12340000)
    put(MEMORY_TABLE + 16, '2I', ALLOCATE, RELEASE)
    put(MEMORY_TABLE + 36, '2I', OVERFLOW, FREE_OVERFLOW)
    for i, (other, owner) in enumerate(zip(OTHERS, OWNERS)):
        put(other + 32, 'I', owner)
        put(owner + 0x40, 'B', int((seed + i) % 7 == 0))
        put(owner + 0x44, 'I', GROUPS[i % 6])
    for i, group in enumerate(GROUPS):
        put(group + 0x80, '3I', HEAP + 0x5000 + i * 0x100, 0, 0x80000010)
        put(group + 0x8c, '3I', HEAP + 0x5080 + i * 0x100, 0, 0x80000010)
    return bytes(data)


class Run:
    def __init__(self, uc, entry, allocator, dispatch, collect, helpers, seed, overflow, mutate, precision):
        self.uc, self.entry, self.dispatch, self.collect, self.helpers = uc, entry, dispatch, collect, helpers
        self.seed, self.overflow, self.mutate, self.precision = seed, overflow, mutate, precision
        self.events, self.calls, self.live = [], {name: 0 for name in helpers.values()}, {}
        self.next_block, self.queue, self.recorder, self.dispatch_sp = HEAP + 0x6000, [], None, None
        self.count = [-2, 0, 1, 3, 5, 9, 12][seed % 7]
        self.absent = seed % 17 == 0
        uc.mem_write(HEAP, initial(seed, overflow))
        uc.mem_write(STOP, b'\xc3' * 4096)
        try:
            uc.mem_map(0, 4096)
        except Exception:
            pass
        self.put(0, 'I', 0x12345678)
        self.put(allocator, 'I', MEMORY)
        self.hook_id = uc.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, count=1):
        return struct.unpack('<' + 'I' * count, self.uc.mem_read(at, count * 4))

    def put(self, at, fmt, *values):
        self.uc.mem_write(at, struct.pack('<' + fmt, *values))

    def done(self, sp, cleanup, result=0xabcdef01):
        for reg, value in [(r.UC_X86_REG_EAX, result), (r.UC_X86_REG_ECX, 0xdead1234), (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(reg, value)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def next_callback(self):
        if self.queue:
            slot, pair = self.queue.pop(0)
            table = self.words(self.recorder)[0]
            entry = self.words(table + slot * 4)[0]
            sp = self.dispatch_sp - 0x200
            self.put(sp, '2I', NEXT, pair)
            self.uc.reg_write(r.UC_X86_REG_ESP, sp)
            self.uc.reg_write(r.UC_X86_REG_ECX, self.recorder)
            self.uc.reg_write(r.UC_X86_REG_EIP, entry)
        else:
            assert self.calls['record_add'] == self.count
            if self.seed % 3 == 1:
                assert self.calls['record_remove'] == 1
            data, count, flags = self.words(self.recorder + 8, 3)
            self.events.append(('recorded', tuple(self.words(data, count)), flags))
            if self.mutate:
                self.put(WORLD + 0x138, 'I', SECOND_DISPATCH)
            self.done(self.dispatch_sp, 20)

    def hook(self, uc, pc, size, unused):
        if pc in self.helpers:
            self.calls[self.helpers[pc]] += 1
        if pc == NEXT:
            self.next_callback()
            return
        if pc not in (ALLOCATE, RELEASE, OVERFLOW, FREE_OVERFLOW, BOUNDS, QUERY, self.dispatch, self.collect):
            return
        sp, receiver = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == BOUNDS:
            transform, tolerance, bounds = self.words(sp + 4, 3)
            assert receiver == SHAPE and transform == MOTION + 32
            self.events.append(('bounds', transform, tolerance, bytes(uc.mem_read(transform, 64))))
            self.put(bounds, '8I', *[self.seed * 100 + i for i in range(8)])
            self.done(sp, 12)
        elif pc == QUERY:
            handle, bounds, added = self.words(sp + 4, 3)
            assert receiver == FINDER and handle == MEMBER + 28
            assert self.words(bounds, 8) == tuple(self.seed * 100 + i for i in range(8))
            assert self.words(added, 5) == (STACK_BLOCK, 0, 0x80000010, STACK_BLOCK, 16)
            self.put(added + 4, 'i', self.count)
            for i in range(max(self.count, 0)):
                pair = (MEMBER + 28, OTHERS[i % 12] + 16)
                if i % 2:
                    pair = tuple(reversed(pair))
                self.put(STACK_BLOCK + i * 8, '2I', *pair)
            if self.mutate:
                self.put(MEMBER + 0x40, 'B', 1)
            self.events.append(('query', self.count))
            self.done(sp, 12)
        elif pc == self.dispatch:
            assert receiver == DISPATCH
            data, count, removed, removed_count, listener = self.words(sp + 4, 5)
            assert (data, count, removed, removed_count, listener) == (STACK_BLOCK, self.count, 0, 0, LISTENER + 8 if self.seed % 3 else 0)
            self.dispatch_sp = sp
            self.recorder = self.words(DISPATCH + 0x24)[0]
            assert self.words(self.recorder + 4, 4) == (MEMBER + 12, self.recorder + 20, 0, 0x80000004)
            self.events.append(('dispatch', tuple(self.words(data, count * 2)), listener))
            self.queue = [(1, data + i * 8) for i in range(count)]
            if self.seed % 3 == 1:
                self.queue.append((2, data))
            self.next_callback()
        elif pc == self.collect:
            data, count, removed, removed_count = self.words(sp + 4, 4)
            assert receiver in GROUPS and removed == removed_count == 0
            self.events.append(('collect', receiver, count, tuple(self.words(data, count * 2))))
            if self.mutate:
                self.put(receiver + 0x84, 'I', 7)
            self.done(sp, 16)
        elif pc == OVERFLOW:
            assert receiver == MEMORY and self.overflow and self.words(sp + 4)[0] == 128
            self.events.append(('overflow', STACK_BLOCK, 128))
            self.done(sp, 4, STACK_BLOCK)
        elif pc == FREE_OVERFLOW:
            assert receiver == MEMORY and self.overflow and self.words(sp + 4, 2) == (STACK_BLOCK, 128)
            self.events.append(('free_overflow', STACK_BLOCK, 128))
            self.done(sp, 8)
        elif pc == ALLOCATE:
            size, tag = self.words(sp + 4, 2)
            assert receiver == MEMORY and tag == 0x12 and 0 < size <= 512
            block = self.next_block
            self.next_block += 512
            self.live[block] = size
            self.events.append(('allocate', block, size, tag))
            self.done(sp, 8, block)
        else:
            block, size, tag = self.words(sp + 4, 3)
            assert receiver == MEMORY and tag == 0x12 and self.live.pop(block) == size
            self.events.append(('release', block, size, tag))
            self.done(sp, 12)

    def run(self):
        uc, sp = self.uc, STACK + 0x8000 + self.seed % 4 * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(sp, '2I', STOP, MEMBER)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455, r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: WORLD, r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision, r.UC_X86_REG_FPTAG: 0xffff,
                           r.UC_X86_REG_FPSW: 0, r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=150000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP and uc.reg_read(r.UC_X86_REG_ESP) == sp + 8
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision and uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(0)[0] == 0x12345678 and not self.live
            assert self.words(MEMORY + 8, 2) == (STACK_BLOCK, 0 if self.overflow else 1024)
            assert self.calls['construct'] == self.calls['destroy'] == int(not self.absent)
            if self.recorder:
                current = SECOND_DISPATCH if self.mutate else DISPATCH
                assert self.words(current + 0x24)[0] == current + 0x100
                for event in self.events:
                    if event[0] == 'collect':
                        assert self.words(event[1] + 0x84)[0] == 0
            return bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), tuple(self.calls.items())
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('overflow,mutate', [(False, False), (True, False), (False, True), (True, True)])
def test_placed_member_pairs(solver_images, precision, overflow, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x30df80, symbol(mapping, 'owner_30c8e0::function_30df80')]
    allocators = [0x480118, symbol(mapping, 'g_480118')]
    dispatches = [0x3728c0, symbol(mapping, 'dispatcher_3728c0::function_3728c0')]
    collectors = [0x3173f0, symbol(mapping, 'collector_3173f0::function_3173f0')]
    names = {0x30d870: 'construct', 0x30d900: 'destroy', 0x30dad0: 'record_add', 0x30dba0: 'record_remove',
             0x30dc30: 'removed_pair', 0x2d91d0: 'grow', 0x2d8db0: 'copy', 0x30dc80: 'recorder_destroy'}
    compiled_helpers = {symbol(mapping, name): label for name, label in [
        ('pairs_30d870::pairs_30d870', 'construct'), ('pairs_30d870::~pairs_30d870', 'destroy'),
        ('class_30dad0::function_30dad0', 'record_add'), ('class_30dad0::function_30dba0', 'record_remove'),
        ('class_30dc30::function_30dc30', 'removed_pair'), ('function_2d91d0', 'grow'), ('function_2d8db0', 'copy'),
        ('holder_30dc80::~holder_30dc80', 'recorder_destroy')]}
    for seed in range(128):
        results = [Run(machine, entry, allocator, dispatch, collect, helper, seed, overflow, mutate, precision).run()
                   for machine, entry, allocator, dispatch, collect, helper in zip((retail, compiled), entries, allocators,
                       dispatches, collectors, (names, compiled_helpers))]
        assert results[0] == results[1], (seed, overflow, mutate, precision)
