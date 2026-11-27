"""Scalar-row inputs observe the rounded SIMD publication before updates."""

import struct

import pytest
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_FPCW

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, OP_LAYOUT,
    solver_images, solver_case, put, address, execute, words,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('kind', [4, 6, 9, 12, 13])
def test_scalar_row_b_inertia_observes_rounded_input(solver_images, precision, kind):
    frame = STACK + 32768 - 0xd0
    body_a, body_b = HEAP, frame - 0x20
    retail, compiled, symbols = solver_images
    snapshots = {}
    targets = [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]
    for label, machine, entry, gate in targets:
        for rounding in range(4):
            data = solver_case(kind, 0)
            size, row_kind = OP_LAYOUT[kind]
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 12, '2I', body_a, body_b)
            put(data, OPS + 24, 'I', kind | (size << 8) | (row_kind << 16))
            if size >= 8:
                put(data, OPS + 28, 'f', 100)
            if size >= 12:
                put(data, OPS + 32, 'f', -100)
            put(data, OPS + 24 + size, 'I', 0x400)
            if row_kind == 3:
                put(data, TERMS, '12f',
                    0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1)
            else:
                put(data, TERMS, '8f', 0, 0, 0, 1, 0, 0, 0, 1)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_a + offset, '4f', 0, 0, 0, 0)
            put(data, body_a + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '4f', 0, 0, 0, 0)

            # B inertia W overlaps the rounded input at frame + 0x1c.
            # The three angular paths expose it through B angular W; the
            # two linear paths expose it through B linear W. Later cursor
            # calls can overwrite these stack locations, so retain the
            # complete vector immediately after its MOVAPS has finished.
            for offset in [-0x10, 0, 0x10]:
                machine.mem_write(frame + offset, b'\0' * 16)
            machine.mem_write(gate, b'\x01')
            target = body_b + (0x10 if row_kind == 3 else 0x20)
            trace, pending = [], [False]

            def written(uc, access, va, size, value, user):
                # Exclude four-byte prologue pushes into B angular's XYZ.
                # Unicorn may report a MOVAPS as two eight-byte writes.
                if va == target and size > 4:
                    pending[0] = True

            def observe(uc, va, size, user):
                if pending[0]:
                    trace.append(words(uc, target, 4))
                    pending[0] = False

            # Both hooks only observe memory; no solver instruction, vector
            # lane, source value, or machine register is changed by a hook.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=control_word)
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, kind, precision, rounding, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert len(trace) == 1, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                expected_w = 0xbf800000 if row_kind == 3 else 0x3f800000
                assert trace[0][3] == expected_w, context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_unilateral_row_b_axis_observes_distinct_rounded_input(solver_images, precision):
    frame = STACK + 32768 - 0xd0
    body_a, body_b = HEAP, HEAP + 0x80
    row = frame - 0x10
    retail, compiled, symbols = solver_images
    snapshots = {}
    targets = [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]
    for label, machine, entry, gate in targets:
        for rounding in range(4):
            data = solver_case(2, 0)
            body_word = struct.unpack('<f', struct.pack('<I', body_b))[0]
            put(data, STEP, '7fi', 0, 1 / body_word, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 4, 'I', row)
            put(data, OPS + 12, '2I', body_a, body_b)
            put(data, OPS + 24, '2I', 0x30402, 0x400)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_a + offset, '4f', 0, 0, 0, 0)
                put(data, body_b + offset, '4f', 0, 0, 0, 0)
            put(data, body_a + 0x30, '4f', 1, 1, 1, 1)
            put(data, body_b + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '4f', 0, 0, 0, 0)

            # The aligned row's B angular axis is frame +0x10. Its Z
            # overlaps the unilateral rounded input at +0x18; W is +0x1c.
            # The row's mass at +0x0c sees the real header's saved B word.
            # Scale its finite float encoding to make a positive impulse.
            # The prologue supplies A's axis from real saved registers.
            # A later cursor call reuses this frame, so observe the completed
            # B vector before that call rather than only the final memory.
            for offset in [-0x10, 0, 0x10]:
                machine.mem_write(frame + offset, b'\0' * 16)
            machine.mem_write(frame - 4, struct.pack('<f', 1))
            machine.mem_write(gate, b'\x01')
            target = body_b + 0x20
            trace, pending = [], [False]

            def written(uc, access, va, size, value, user):
                # Unicorn may report a MOVAPS as two eight-byte writes.
                if va == target and size > 4:
                    pending[0] = True

            def observe(uc, va, size, user):
                if pending[0]:
                    trace.append((words(uc, target, 4), words(uc, frame + 0x18, 2)))
                    pending[0] = False

            # Both hooks only observe memory; no solver instruction, vector
            # lane, source value, or machine register is changed by a hook.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=control_word)
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, precision, rounding, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert len(trace) == 1, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                assert trace[0][1][1] == 0, context
                scalar = struct.unpack('<f', struct.pack('<I', trace[0][1][0]))[0]
                assert 0.5 < scalar < 2, context
                square = struct.unpack('<I', struct.pack('<f', scalar * scalar))[0]
                assert trace[0][0][2] == square, context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context
