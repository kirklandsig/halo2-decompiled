"""Rounded fallback inputs must leave the adjacent B inertia word untouched."""

import pytest
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_FPCW

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES,
    solver_images, solver_case, put, address, execute, words,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('path', ['unilateral', 'normal-first', 'normal-second'])
def test_fallback_b_inertia_preserves_adjacent_scalar(solver_images, precision, path):
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
            kind = 2 if path == 'unilateral' else 3
            data = solver_case(kind, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 12, '2I', body_a, body_b)
            if kind == 2:
                put(data, OPS + 24, '2I', 2 | (4 << 8) | (3 << 16), 0x400)
                row_targets = [1]
            else:
                put(data, OPS + 24, '3I', 3 | (8 << 8) | (5 << 16), 0, 0x400)
                row_targets = [1, -1] if path == 'normal-first' else [-1, 1]
            for row, target in enumerate(row_targets):
                put(data, TERMS + row * 48, '12f',
                    0, 0, 0, target, 0, 0, 0, 1, 0, 0, 0, 1)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_a + offset, '4f', 0, 0, 0, 0)
            put(data, body_a + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '2f', 0, 0)
            # B inertia W aliases frame + 0x1c. These paths round at +0x18;
            # an extra conversion copy at +0x1c changes B's linear update.
            for offset in [-0x10, 0, 0x10]:
                machine.mem_write(frame + offset, b'\0' * 16)
            machine.mem_write(gate, b'\x01')
            trace, pending = [], [False]

            def written(uc, access, va, size, value, user):
                if va == body_b + 0x10:
                    pending[0] = True

            def observe(uc, va, size, user):
                if pending[0]:
                    trace.append(words(uc, body_b + 0x10, 4))
                    pending[0] = False

            # Read after the complete MOVAPS, before later calls reuse the
            # argument area. Both hooks observe memory without changing it.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=control_word)
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, precision, rounding, path, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert len(trace) == 1, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                assert trace[0][3] == 0, context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding][1] == tuple(trace), context
                assert snapshots[rounding] == (snapshot, tuple(trace)), context
