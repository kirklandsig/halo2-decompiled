"""Optional differential execution of ragdoll and hinge solver builders.

Requires the local retail XBE, a full checker build, and Unicorn. All joint solver callees run normally. No retail executable bytes are stored in the tests.
"""
import random
import struct

import pytest

pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from test_solver_rows import (
    HEAP, STACK, STOP, PAIR, OUTPUT, TERMS, OPS, case, solver_images,  # noqa: F401
)

JOINT, ENTITY_A, ENTITY_B = HEAP + 0x4000, HEAP + 0x5000, HEAP + 0x5200


def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, ('rebuild the full checker image', name, hits)
    return hits.pop()


def put(data, va, fmt, *values):
    struct.pack_into('<' + fmt, data, va - HEAP, *values)


def words(uc, va, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, 4 * count))


def return_from_hook(uc, argument_bytes=0):
    sp = uc.reg_read(UC_X86_REG_ESP)
    ret, = words(uc, sp, 1)
    uc.reg_write(UC_X86_REG_ESP, sp + 4 + argument_bytes)
    uc.reg_write(UC_X86_REG_EIP, ret)


def execute(uc, entry, data, args, this=None, hooks=(), precision=0x37f):
    if not any(lo == 0 for lo, hi, permissions in uc.mem_regions()):
        uc.mem_map(0, 4096)
    uc.mem_write(0, b'\xff' * 4)
    uc.mem_write(HEAP, bytes(data))
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    if this is not None:
        uc.reg_write(UC_X86_REG_ECX, this)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    handles = [uc.hook_add(UC_HOOK_CODE, callback, begin=va, end=va) for va, callback in hooks]
    try:
        uc.emu_start(entry, STOP, count=100000)
    finally:
        for handle in handles:
            uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 + (4 * len(args) if this is not None else 0)
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    assert bytes(uc.mem_read(0, 4)) == b'\xff' * 4
    return bytes(uc.mem_read(HEAP, len(data)))


@pytest.mark.parametrize('va,name', [
    (0x312fe0, 'lane_a_ragdoll::build'),
    (0x313810, 'lane_a_limited_hinge::build'),
    (0x313ef0, 'lane_a_hinge::build'),
])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_joint_builders_against_retail(solver_images, va, name, precision):
    retail, compiled, symbols = solver_images
    target = address(symbols, name)
    for seed in range(64):
        data, _ = case(0x31f160, seed)
        rng = random.Random(seed)
        put(data, JOINT + 12, 'II', ENTITY_A, ENTITY_B)
        put(data, ENTITY_A + 0x3c, 'I', HEAP + 2048 - 0x80)
        put(data, ENTITY_B + 0x3c, 'I', HEAP + 2304 - 0x80)
        for offset in range(0x20, 0x90, 16):
            put(data, JOINT + offset, '4f', *(rng.uniform(-2, 2) for _ in range(3)), 0)
        friction = [0.0, -1.0, 0.25, 4.0][seed % 4]
        if va == 0x312fe0:
            put(data, JOINT + 0x80, '6f', 0.25, -0.5, 0.7, -0.8, 0.9, friction)
            result_offset, result_count = 0x98, 9
        elif va == 0x313810:
            put(data, JOINT + 0xc8, '4f', -1.5, 2, friction, [-7, -3.14, 0, 3.14, 7][seed % 5])
            result_offset, result_count = 0x90, 7
        else:
            result_offset, result_count = 0x70, 5
        for i in range(result_count * 2):
            put(data, JOINT + result_offset + 4 * i, 'f', rng.uniform(-1, 1))
        expected = execute(retail, va, data, [PAIR, OUTPUT], JOINT, precision=precision)
        actual = execute(compiled, target, data, [PAIR, OUTPUT], JOINT, precision=precision)
        assert actual == expected, (name, seed, precision)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_swing_limits_and_degenerate_axes(solver_images, precision):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'lane_a_ragdoll::build_swing_limits')
    vectors = [
        (0, 0, 0, 0), (1, 0, 0, 0), (-1, 0, 0, 0),
        (1, 0.0001, 0, 0), (1, 0.001, 0, 0),
        (0, 1, 0, 0), (0.2, -0.4, 0.8, 0),
        (0, 2**-12, 2**-12, 0),  # Squared norm equals the threshold exactly.
        (float('nan'), 0, 0, 0), (float('inf'), 1, 0, 0),
    ]
    for seed in range(len(vectors)**2):
        data, _ = case(0x31f000, seed)
        put(data, JOINT + 0x80, '3f', 0.25, -0.5, 0.7)
        put(data, JOINT + 0x200, '4f', 1, 0, 0, 0)
        put(data, JOINT + 0x210, '4f', *vectors[seed % len(vectors)])
        put(data, JOINT + 0x220, '4f', *vectors[seed // len(vectors)])
        args = [JOINT + 0x200, JOINT + 0x210, JOINT + 0x220, PAIR, OUTPUT]
        expected = execute(retail, 0x312e00, data, args, JOINT, precision=precision)
        actual = execute(compiled, target, data, args, JOINT, precision=precision)
        assert actual == expected, (seed, precision)


