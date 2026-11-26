"""Retail linear bounds retain their axis across the scalar SIMD copy."""
import struct

import pytest
from unicorn.x86_const import (
    UC_X86_REG_ESP, UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_XMM4,
)

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, RESULTS, IMPULSES, CALLBACK,
    solver_images, solver_case, put, address, execute, words, return_from_hook,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('aliased', [False, True])
@pytest.mark.parametrize('upper_active', [False, True])
def test_linear_bound_axis_precedes_scalar_copy(
        solver_images, precision, aliased, upper_active):
    frame = STACK + 32768 - 0xd0
    rows = frame + 0x10 if aliased else HEAP + 0x6000
    body_a, body_b = HEAP + 0x7000, HEAP + 0x80
    retail, compiled, symbols = solver_images
    impulse_bits = 0xbf800000 if upper_active else 0x3f800000
    snapshots = {}
    for label, machine, entry, gate in [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]:
        for rounding in range(4):
            data = solver_case(11, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS, '6I', 0x11801, rows, RESULTS, body_a, body_b, 8)
            put(data, OPS + 24, '3I', 0x20c0e, CALLBACK, 3)
            bound = 2 if upper_active else 0
            put(data, OPS + 36, 'IffI', 11 | (12 << 8) | (3 << 16),
                bound, bound, 0x400)
            put(data, HEAP, 'B', 3)
            for body in [body_a, body_b]:
                put(data, body + 0x10, '12f', *([0.] * 8 + [1.] * 4))
            if not aliased:
                put(data, rows, '12f', *([0.] * 12))
            put(data, IMPULSES, 'f', 0)
            machine.mem_write(frame + 0x10, b'\0' * 0xb0)
            machine.mem_write(gate, b'\x01')
            callbacks, observations = [], []

            def callback(uc, va, size, user):
                esp = uc.reg_read(UC_X86_REG_ESP)
                actual_rows, payload = struct.unpack('<II', uc.mem_read(esp + 4, 8))
                assert actual_rows == rows
                # Supply real row inputs at an external-operation boundary.
                # In the alias case the linear axis W shares the SIMD copy
                # home, and angular-A W shares the iteration counter.
                uc.mem_write(rows + 12, struct.pack('<f', 1))
                uc.mem_write(rows + 28, struct.pack('<f', 1))
                callbacks.append((actual_rows, payload))
                for reg in [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
                    uc.reg_write(reg, 0x13579bdf)
                return_from_hook(uc)

            before_copy = 0x31deb0 if upper_active else 0x31de0e
            after_copy = 0x31deb4 if upper_active else 0x31de12

            def observe(uc, va, size, user):
                axis_w = (uc.reg_read(UC_X86_REG_XMM4) >> 96) & 0xffffffff
                observations.append((va, axis_w, words(uc, frame + 0x1c, 1)[0]))

            hooks = [(CALLBACK, callback)]
            if label == 'retail':
                # Read-only observations establish that the axis still has
                # W=1 after the rounded impulse overwrites its aliased input.
                hooks += [(before_copy, observe), (after_copy, observe)]
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               hooks=hooks, precision=precision | (rounding << 10))
            observed = (words(machine, body_a + 0x10, 4),
                        words(machine, body_b + 0x10, 4),
                        words(machine, IMPULSES, 1))
            context = (label, precision, rounding, aliased, upper_active, observed)
            assert callbacks == [(rows, OPS + 32)], context
            assert observed[0][3] == impulse_bits, context
            assert observed[1][3] == (impulse_bits ^ 0x80000000), context
            assert observed[2] == (impulse_bits,), context
            assert snapshot[1] == 1, context
            if label == 'retail':
                assert observations == [
                    (before_copy, 0x3f800000, 0x3f800000 if aliased else 0),
                    (after_copy, 0x3f800000, impulse_bits),
                ], (context, observations)
                snapshots[rounding] = (snapshot, callbacks, observed)
            else:
                # execute also checks stack cleanup, saved registers and FP control.
                assert snapshots[rounding] == (snapshot, callbacks, observed), context
