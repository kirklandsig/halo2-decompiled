"""The spin residual reloads angular input after the pair's scalar publications."""
import struct

import pytest
import unicorn.x86_const as ux

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, RESULTS, FRICTION_RESULTS,
    CALLBACK, solver_images, solver_case, put, address, execute, words,
    return_from_hook,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('aliased', [False, True])
def test_spin_residual_angular_capture(solver_images, precision, aliased):
    retail, compiled, symbols = solver_images
    frame = STACK + 32768 - 0xd0
    a = frame + 0x80 if aliased else HEAP + 0x7000
    b = HEAP + 0x80
    changed = 0
    for rounding in range(4):
        for seed in range(8):
            data = solver_case(8, 2)
            put(data, STEP, '7fi', 0, 1, 1, 1, 60, 0, 0, 1)
            put(data, OPS, '6I', 0x11801, TERMS, RESULTS, a, b, 8)
            put(data, OPS + 24, '3I', 0x20c0e, CALLBACK, 3)
            put(data, OPS + 36, 'IIffIf', 0x61808, FRICTION_RESULTS,
                .2, [.02, 1., 100.][seed % 3], 12, .7)
            put(data, OPS + 60, 'I', 0x400)
            snapshots = []
            for label, machine, entry, gate in [
                ('retail', retail, 0x31c940, 0x480c60),
                ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60')),
            ]:
                machine.mem_write(frame + 0xc, bytes(180))
                machine.mem_write(gate, b'\x01')
                calls, observations = [], []

                def callback(uc, va, size, user):
                    # Supply body inputs at an actual external-operation boundary.
                    # In the aliased scenes A.angular.Y is also the second pair
                    # projection's scalar home at frame+A4.
                    values = [((seed + i) % 9 - 4) / 8 for i in range(8)]
                    uc.mem_write(a + 0x10, struct.pack('<12f',
                                 *(values + [.25, .5, .75, 1.])))
                    calls.append(1)
                    for reg in [ux.UC_X86_REG_EAX, ux.UC_X86_REG_ECX,
                                ux.UC_X86_REG_EDX]:
                        uc.reg_write(reg, 0x13579bdf)
                    return_from_hook(uc)

                def observed(uc, va, size, user):
                    observations.append((va, words(uc, a + 0x24, 1)[0]))

                hooks = [(CALLBACK, callback)]
                if label == 'retail':
                    # Read-only observations before the pair and after its two
                    # error publications, immediately before the spin projection.
                    hooks += [(0x31e2e9, observed), (0x31e3a8, observed)]
                control = precision | rounding << 10
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                   hooks=hooks, precision=control)
                context = (label, precision, rounding, aliased, seed)
                assert calls == [1] and snapshot[1] == 1, context
                assert machine.reg_read(ux.UC_X86_REG_FPCW) == control, context
                if label == 'retail':
                    assert [x[0] for x in observations] == [0x31e2e9, 0x31e3a8], context
                    if aliased:
                        changed += observations[0][1] != observations[1][1]
                    else:
                        assert observations[0][1] == observations[1][1], context
                snapshots.append(snapshot)
            # Exact full-heap outputs, including the residual targets and exported
            # errors. execute also checks stack cleanup, saved registers and FS.
            assert snapshots[0] == snapshots[1], (precision, rounding, aliased, seed)
    if aliased:
        assert changed > 0, 'the aliased angular input must actually change'
