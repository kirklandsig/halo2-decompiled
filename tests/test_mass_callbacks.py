"""Retail mass callbacks, including sequential aliasing and x87 special values."""
import random
import struct

import pytest

pytest.importorskip('unicorn')
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

RECORD, FIRST, SECOND = HEAP + 0x1000, HEAP + 0x2000, HEAP + 0x3000
SPECIAL = [0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x00800000,
           0x3f000000, 0x3f800000, 0xbf800000, 0x40000000, 0x7f7fffff,
           0xff7fffff, 0x7f800000, 0xff800000, 0x7fc12345, 0xffc54321,
           0x7f812345, 0xff812346]


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, layout):
    rng = random.Random(0x315280 + seed)
    data = bytearray(b'\xcd' * 65536)
    def put(at, values):
        struct.pack_into('<' + 'I' * len(values), data, at - HEAP, *values)
    def values(count, offset):
        return [SPECIAL[(seed + i * 7 + offset) % len(SPECIAL)]
                if seed < 72 else rng.getrandbits(32) for i in range(count)]
    # The last two layouts overlap saved values, then both scale factors;
    # the record's accumulator pointers remain valid throughout the sequence.
    a, b = [(FIRST, SECOND), (FIRST, FIRST), (FIRST, FIRST + 4),
            (FIRST + 4, FIRST), (RECORD - 0x2c, RECORD - 0x20),
            (RECORD - 0x18, SECOND)][layout]
    put(FIRST, values(20, 0))
    put(SECOND, values(20, 5))
    put(RECORD, values(10, 11) + [a, b])
    return bytes(data)


def run(uc, entries, memory, control, mxcsr):
    uc.mem_write(HEAP, memory)
    snapshots = []
    for entry in entries:
        uc.mem_write(STACK, b'\xcc' * 65536)
        sp = STACK + 0x8000
        uc.mem_write(sp, struct.pack('<3I', STOP, 0x12345678, RECORD))
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        registers = {**saved, r.UC_X86_REG_EAX: 0x12341234,
                     r.UC_X86_REG_ECX: 0x23452345, r.UC_X86_REG_EDX: 0x34563456,
                     r.UC_X86_REG_ESP: sp, r.UC_X86_REG_EFLAGS: 2,
                     r.UC_X86_REG_FPCW: control, r.UC_X86_REG_FPTAG: 0xffff,
                     r.UC_X86_REG_FPSW: 0, r.UC_X86_REG_MXCSR: mxcsr}
        for reg, value in registers.items():
            uc.reg_write(reg, value)
        uc.emu_start(entry, STOP, count=10000)
        assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
        assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 4
        assert all(uc.reg_read(reg) == value for reg, value in saved.items())
        assert uc.reg_read(r.UC_X86_REG_FPCW) == control
        assert uc.reg_read(r.UC_X86_REG_MXCSR) == mxcsr
        assert uc.reg_read(r.UC_X86_REG_FPTAG) == 0xffff
        snapshots.append(bytes(uc.mem_read(HEAP, 65536)))
    return tuple(snapshots)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('layout', range(6))
@pytest.mark.parametrize('operation', ['scale', 'restore', 'sequence'])
def test_mass_callbacks(solver_images, precision, layout, operation):
    retail, compiled, mapping = solver_images
    scale, restore = symbol(mapping, 'lane_a_scale_masses'), symbol(mapping, 'lane_a_restore_masses')
    entries = {'scale': [(0x315280,), (scale,)],
               'restore': [(0x3152f0,), (restore,)],
               'sequence': [(0x315280, 0x3152f0), (scale, restore)]}[operation]
    for seed in range(128):
        memory = initial(seed, layout)
        control = precision | ((seed % 4) << 10)
        mxcsr = 0x1f80 | ((seed % 4) << 13) | (0x8040 if seed % 2 else 0)
        expected = run(retail, entries[0], memory, control, mxcsr)
        actual = run(compiled, entries[1], memory, control, mxcsr)
        assert actual == expected, (seed, precision, layout, operation)
