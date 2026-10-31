"""Retail overlap filtering and notification through actual temporary arrays.

Broadphase, filter, listener and allocator virtuals are synthetic. Array
construction, growth, copying and destruction execute from each real image.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

WORLD, OBJECT, BROAD, FILTER_OWNER = [HEAP + n for n in (0, 0x300, 0x400, 0x500)]
OBJECT_TABLE, BROAD_TABLE, FILTER_TABLE = [HEAP + n for n in (0x600, 0x700, 0x800)]
OTHERS = [HEAP + 0x1000 + 0x100 * i for i in range(12)]
LISTENERS = [HEAP + 0x2000 + 0x100 * i for i in range(12)]
MEMORY, MEMORY_TABLE, STACK_BLOCK = [HEAP + n for n in (0x3000, 0x3100, 0x4000)]
ALLOCATE, RELEASE, OVERFLOW, FREE_OVERFLOW, BOX, QUERY, ACCEPT, ADD, REMOVE, CHANGED = [STOP + 16 * i for i in range(1, 11)]


def symbol(mapping, name):
    values = {s.va for s in mapping.find(name)}
    assert len(values) == 1, (name, values)
    return values.pop()


def initial(seed, overflow):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(WORLD + 0xc4, 'I', BROAD)
    put(WORLD + 0xd0, 'I', FILTER_OWNER)
    put(WORLD + 0x234, 'I', 16)
    put(OBJECT, 'I', OBJECT_TABLE)
    put(OBJECT_TABLE + 24, '4I', BOX, ADD, REMOVE, CHANGED)
    put(BROAD, 'I', BROAD_TABLE)
    put(BROAD_TABLE + 36, 'I', QUERY)
    put(FILTER_OWNER + 8, 'I', FILTER_TABLE)
    put(FILTER_TABLE, 'I', ACCEPT)
    put(MEMORY, '6I', MEMORY_TABLE, 0, STACK_BLOCK, 0 if overflow else 1024, 0,
        STACK_BLOCK if overflow else 0x12340000)
    put(MEMORY_TABLE + 16, '2I', ALLOCATE, RELEASE)
    put(MEMORY_TABLE + 36, '2I', OVERFLOW, FREE_OVERFLOW)
    put(OBJECT + 12 + 24, 'I', 0)
    put(OBJECT + 12 + 32, 'I', OBJECT)
    for index, (other, listener) in enumerate(zip(OTHERS, LISTENERS)):
        put(other + 24, 'I', [0, 2, 7][(seed + index) % 3])
        put(other + 32, 'I', listener)
        put(listener, 'I', OBJECT_TABLE)
    return bytes(data)


class Run:
    def __init__(self, uc, entry, allocator, helpers, seed, overflow, mutate, precision):
        self.uc, self.entry, self.helpers = uc, entry, helpers
        self.seed, self.overflow, self.mutate, self.precision = seed, overflow, mutate, precision
        self.events, self.calls, self.live = [], {name: 0 for name in helpers.values()}, {}
        self.next_block, self.filters, self.pairs = HEAP + 0x6000, 0, None
        self.count = [-2, 0, 1, 2, 3, 5, 7, 9][seed % 8]
        self.notify = [0, 1, 0x80, 0xff][seed % 4]
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
        for reg, value in [(r.UC_X86_REG_EAX, result), (r.UC_X86_REG_ECX, 0xdead1234),
                           (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(reg, value)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def hook(self, uc, pc, size, unused):
        if pc in self.helpers:
            self.calls[self.helpers[pc]] += 1
        if pc < ALLOCATE or pc > CHANGED or (pc - STOP) % 16:
            return
        sp, receiver = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == BOX:
            assert receiver == OBJECT
            box = self.words(sp + 4)[0]
            bits = [0x3f800000, 0xbf800000, 0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345, self.seed]
            self.put(box, '8I', *bits)
            self.events.append(('box', tuple(bits)))
            self.done(sp, 4)
        elif pc == QUERY:
            assert receiver == BROAD
            box, self.pairs = self.words(sp + 4, 2)
            assert tuple(self.words(box, 8)) == self.events[-1][1]
            data, count, flags, block, capacity = self.words(self.pairs, 5)
            assert (data, count, flags, block, capacity) == (STACK_BLOCK, 0, 0x80000010, STACK_BLOCK, 16)
            self.put(self.pairs + 4, 'i', self.count)
            for index in range(max(0, self.count)):
                other = OBJECT + 12 if (index + self.seed) % 5 == 0 else OTHERS[(index + self.seed) % 12]
                self.put(data + index * 8, '2I', 0x12340000 + index, other + 16)
            self.events.append(('query', self.count))
            self.done(sp, 8)
        elif pc == ACCEPT:
            assert receiver == FILTER_OWNER + 8
            result, own, other = self.words(sp + 4, 3)
            assert own == OBJECT + 12 and other in OTHERS
            accepted = [0, 1, 0x80, 0xff][(self.seed + self.filters) % 4]
            self.events.append(('filter', own, other, accepted))
            self.put(result, 'B', accepted)
            self.filters += 1
            if self.mutate:
                data, count = self.words(self.pairs, 2)
                if self.mutate == 'relocate' and self.filters == 1:
                    replacement = HEAP + 0x4500
                    uc.mem_write(replacement, bytes(uc.mem_read(data, 128)))
                    self.put(self.pairs, 'I', replacement)
                    data = replacement
                # The candidate is loaded again after filtering; replace all
                # remaining entries with a valid different handle.
                for index in range(count):
                    self.put(data + index * 8 + 4, 'I', OTHERS[11] + 16)
                if self.filters == 1 and count > 3:
                    self.put(self.pairs + 4, 'I', 3)
            self.done(sp, 12, result)
        elif pc in (ADD, REMOVE):
            other = self.words(sp + 4)[0]
            assert receiver == OBJECT or receiver in LISTENERS
            assert other in OTHERS or other == OBJECT + 12
            flag = uc.mem_read(sp + 8, 1)[0] if pc == ADD else None
            if pc == ADD:
                assert flag == 1
            self.events.append(('add' if pc == ADD else 'remove', receiver, other, flag))
            if self.mutate and receiver == OBJECT:
                # Mirrored dispatch must observe listener-kind changes made
                # by the object's own callback.
                self.put(other + 24, 'I', 0 if self.words(other + 24)[0] == 2 else 2)
                self.put(other + 32, 'I', LISTENERS[11])
            self.done(sp, 8 if pc == ADD else 4)
        elif pc == CHANGED:
            assert receiver == OBJECT and self.notify
            self.events.append(('changed',))
            self.done(sp, 0)
        elif pc == OVERFLOW:
            assert receiver == MEMORY and self.overflow and self.words(sp + 4)[0] == 128
            self.events.append(('overflow', STACK_BLOCK, 128))
            self.done(sp, 4, STACK_BLOCK)
        elif pc == FREE_OVERFLOW:
            assert receiver == MEMORY and self.overflow and self.words(sp + 4, 2) == (STACK_BLOCK, 128)
            self.events.append(('free_overflow', STACK_BLOCK, 128))
            self.done(sp, 8)
        elif pc == ALLOCATE:
            assert receiver == MEMORY
            size, tag = self.words(sp + 4, 2)
            assert size in (4, 8, 16, 32, 64) and tag == 0x12
            block = self.next_block
            self.next_block += 128
            self.live[block] = size
            self.events.append(('allocate', block, size, tag))
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
        self.put(sp, '3I', STOP, OBJECT, self.notify)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: WORLD,
                           r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
                           r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0,
                           r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=150000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 12
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(0)[0] == 0x12345678
            assert self.words(MEMORY + 8, 2) == (STACK_BLOCK, 0 if self.overflow else 1024)
            assert not self.live
            assert self.calls['construct'] == self.calls['destroy'] == 1
            assert sum(event[0] == 'changed' for event in self.events) == bool(self.notify)
            return bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), tuple(self.calls.items())
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('overflow,mutate', [(False, False), (True, False), (False, True),
    (True, True), (False, 'relocate'), (True, 'relocate')])
def test_overlap_updates(solver_images, precision, overflow, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x30dce0, symbol(mapping, 'world_30dce0::function_30dce0')]
    allocators = [0x480118, symbol(mapping, 'g_480118')]
    names = {0x30d870: 'construct', 0x30d900: 'destroy', 0x2d91d0: 'grow', 0x2d8db0: 'copy'}
    compiled_helpers = {symbol(mapping, name): label for name, label in [
        ('pairs_30d870::pairs_30d870', 'construct'), ('pairs_30d870::~pairs_30d870', 'destroy'),
        ('function_2d91d0', 'grow'), ('function_2d8db0', 'copy')]}
    for seed in range(128):
        results = [Run(machine, entry, allocator, helper, seed, overflow, mutate, precision).run()
                   for machine, entry, allocator, helper in zip((retail, compiled), entries, allocators, (names, compiled_helpers))]
        assert results[0] == results[1], (seed, overflow, mutate, precision)
