"""Linear updates observe rounded inputs and preserve neighboring row words."""

import struct

import pytest
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_FPCW

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, RESULTS, OP_LAYOUT,
    solver_images, solver_case, put, address, execute, words,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('kind', [6, 11, 13])
@pytest.mark.parametrize('initial_w', [
    0x40a00000, 0xc0400000, 0x7f800000, 0x7fc12345,
])
def test_linear_a_observes_rounded_input(solver_images, precision, kind, initial_w):
    frame = STACK + 32768 - 0xd0
    body_a, body_b = frame, HEAP + 0x80
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
            if kind == 6:
                put(data, OPS + 28, 'f', 100)
            elif kind == 11:
                put(data, OPS + 28, '2f', 0, 0)
            put(data, OPS + 24 + size, 'I', 0x400)
            put(data, TERMS, '12f',
                0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_b + offset, '4f', 0, 0, 0, 0)
            put(data, body_b + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '4f', 0, 0, 0, 0)

            # A linear W shares frame + 0x1c with the rounded SIMD input.
            # Its old finite/nonfinite value participates in projection,
            # but the update must read the newly published value instead.
            machine.mem_write(frame, struct.pack('<32f', *([1.0] * 32)))
            machine.mem_write(frame + 0x1c, struct.pack('<I', initial_w))
            machine.mem_write(gate, b'\x01')
            target = body_a + 0x10
            trace, pending = [], [False]

            def written(uc, access, va, size, value, user):
                if va == target and size > 4:
                    pending[0] = True

            def observe(uc, va, size, user):
                if pending[0]:
                    trace.append(words(uc, target, 4))
                    pending[0] = False

            # Observe the completed vector before later cursor calls reuse
            # the frame. Neither hook changes code, memory, or registers.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=control_word)
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, kind, precision, rounding, initial_w, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert len(trace) == 1, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                assert trace[0][3] == 0x40800000, context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_unilateral_zero_impulse_preserves_neighboring_row_target(solver_images, precision):
    frame = STACK + 32768 - 0xd0
    body_a, body_b = HEAP + 0x80, HEAP + 0x100
    assert frame % 16 == 0
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
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS, '6I', 0x11801, frame + 0x10, RESULTS,
                body_a, body_b, 8)
            put(data, OPS + 24, '2I', 0x30402, 0x400)
            put(data, HEAP, 'B', 3)
            put(data, IMPULSES, 'f', 0)
            put(data, RESULTS, '4f', 0, 0, 0, 0)
            for body in (body_a, body_b):
                for offset in (0x10, 0x20, 0x40, 0x50):
                    put(data, body + offset, '4f', 0, 0, 0, 0)
                put(data, body + 0x30, '4f', 1, 1, 1, 1)

            # The real aligned row starts at native frame +0x10. Its
            # linear Z observes the SIMD input at +0x18; its target W at
            # +0x1c must stay 1. The real iteration counter overlaps A's
            # angular W at +0x2c and supplies zero mass. A positive error
            # therefore executes an update with zero impulse, rather than
            # skipping it. An extra argument spill at +0x1c destroys the
            # target and changes the subsequent real residual/writeback.
            machine.mem_write(frame, b'\0' * 192)
            machine.mem_write(frame + 0x1c, struct.pack('<f', 1))
            machine.mem_write(gate, b'\x01')
            trace = []

            def written(uc, access, va, size, value, user):
                if va == IMPULSES and size == 4:
                    trace.append((words(uc, frame + 0x1c, 1)[0],
                                  words(uc, frame + 0x18, 1)[0],
                                  words(uc, frame + 0x2c, 1)[0]))

            # Read the row at the first real impulse publication. This
            # observer changes no memory, registers, or instructions.
            handle = machine.hook_add(UC_HOOK_MEM_WRITE, written)
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=control_word)
            finally:
                machine.hook_del(handle)
            context = (label, precision, rounding, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert snapshot[1] == 1, context
            assert len(trace) == 1, context
            if label == 'retail':
                assert trace[0][0] == 0x3f800000 and trace[0][2] == 0, context
                assert words(machine, RESULTS + 4, 1) == (0x3f800000,), context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context
