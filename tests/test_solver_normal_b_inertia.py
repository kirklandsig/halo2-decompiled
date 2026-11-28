"""First normal-pair SIMD copies preserve B inertia at the shared stack word."""

import pytest
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES,
    solver_images, solver_case, put, address, execute, words,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_first_normal_b_inertia_preserves_shared_word(solver_images, precision):
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
            data = solver_case(3, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 12, '2I', body_a, body_b)
            put(data, OPS + 24, '3I', 3 | (8 << 8) | (5 << 16), 0, 0x400)
            for row in range(2):
                put(data, TERMS + row * 48, '12f',
                    0, 0, 0, float(row + 1), 0, 0, 0, 1, 0, 0, 0, 1)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_a + offset, '4f', 0, 0, 0, 0)
            put(data, body_a + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '2f', 0, 0)
            # This positive pair leaves frame + 0x1c untouched. B inertia W
            # aliases it; an extra compiler copy there changes the body update.
            # Its linear vector overlaps later call arguments below the frame.
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

            # Unicorn can split MOVAPS into smaller writes. Observe the entire
            # completed vector on the next instruction, before later calls
            # reuse its storage. These hooks change no machine state.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   precision=precision | (rounding << 10))
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, precision, rounding, trace)
            assert len(trace) == 2, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                assert all(value[3] == 0 for value in trace), context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context
