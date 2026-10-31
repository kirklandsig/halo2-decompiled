"""Differential settings copies, including overlapping pair storage.

Array growth and byte copying execute normally. Allocator virtuals supply
deterministic storage; no function body or copy operation is intercepted.
"""
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OBJECT, DESCRIPTION, SOURCE, DESTINATION, ALTERNATE = [HEAP + n for n in (0, 0x100, 0x300, 0x500, 0x800)]
MEMORY, TABLE, BLOCK = HEAP + 0x1000, HEAP + 0x1100, HEAP + 0x2000
ALLOCATE, RELEASE = STOP + 16, STOP + 32


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, mode, grow):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    count = [-2, 0, 1, 2, 3, 7][seed % 6]
    source, destination = SOURCE, DESTINATION
    if mode == 'same':
        destination = source
    elif mode == 'forward':
        destination = source + 4
    elif mode == 'backward':
        destination = source - 4
    elif mode == 'header':
        count, destination = 1, OBJECT + 0x30
    actual_growth = grow and mode == 'separate' and count > 1
    put(MEMORY, 'I', TABLE)
    put(TABLE + 4 * 4, '2I', ALLOCATE, RELEASE)
    put(OBJECT + 0x28, 'I', seed if seed % 2 else 0)
    put(OBJECT + 0x30, '3I', destination, 1, 1 if actual_growth else 0x80000040)
    put(DESCRIPTION, 'I', seed)
    put(DESCRIPTION + 12, 'IiI', source, count, 0x80000040)
    for index in range(10):
        put(source + index * 8, '2I', (0x87654321 + seed * 77 + index * 19) & 0xffffffff,
            (0xfedcba98 - seed * 17 - index * 31) & 0xffffffff)
    if mode == 'header':
        put(source, '2I', ALTERNATE, seed + 1)
    return bytes(data), actual_growth


class Run:
    def __init__(self, machine, entry, allocator, helpers, seed, mode, grow, mutate, precision):
        self.uc, self.entry, self.helpers = machine, entry, helpers
        self.seed, self.mode, self.mutate, self.precision = seed, mode, mutate, precision
        self.events, self.calls = [], {name: 0 for name in helpers.values()}
        data, self.grows = initial(seed, mode, grow)
        machine.mem_write(HEAP, data)
        machine.mem_write(STOP, b'\xc3' * 4096)
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
            self.calls[self.helpers[pc]] += 1
        if pc not in (ALLOCATE, RELEASE):
            return
        sp = uc.reg_read(r.UC_X86_REG_ESP)
        assert self.grows and uc.reg_read(r.UC_X86_REG_ECX) == MEMORY
        if pc == ALLOCATE:
            size, tag = self.words(sp + 4, 2)
            assert 16 <= size <= 56 and tag == 0x12
            self.events.append(('allocate', size, tag))
            if self.mutate:
                self.put(DESCRIPTION + 16, 'I', 1)
                self.put(SOURCE, '2I', 0x12340000 + self.seed, 0x56780000 + self.seed)
            self.done(sp, 8, BLOCK)
        else:
            assert self.words(sp + 4, 3) == (DESTINATION, 8, 0x12)
            self.events.append(('release', DESTINATION, 8, 0x12))
            self.done(sp, 12)

    def run(self):
        uc = self.uc
        sp = STACK + 0x8000 + self.seed % 4 * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(sp, '2I', STOP, DESCRIPTION)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: OBJECT,
                           r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
                           r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0,
                           r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=30000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 8
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert len(self.events) == int(self.grows) * 2
            assert self.calls['grow'] == self.calls['copy'] == int(self.grows)
            return bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), tuple(self.calls.items())
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('mode,grow,mutate', [('separate', False, False), ('separate', True, False),
    ('separate', True, True), ('same', False, False), ('forward', False, False),
    ('backward', False, False), ('header', False, False)])
def test_settings_copy(solver_images, precision, mode, grow, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x3124c0, symbol(mapping, 'class_312430::function_3124c0')]
    allocators = [0x480118, symbol(mapping, 'g_480118')]
    helpers = [{0x2d9160: 'grow', 0x2d8db0: 'copy'},
               {symbol(mapping, 'function_2d9160'): 'grow', symbol(mapping, 'function_2d8db0'): 'copy'}]
    for seed in range(96):
        results = [Run(machine, entry, allocator, helper, seed, mode, grow, mutate, precision).run()
                   for machine, entry, allocator, helper in zip((retail, compiled), entries, allocators, helpers)]
        assert results[0] == results[1], (seed, mode, grow, mutate, precision)
