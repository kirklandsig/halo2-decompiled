"""Retail circular rows retain receivers and inertia across a saved-pointer write."""
import struct

import pytest
import unicorn.x86_const as ux

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, RESULTS, FRICTION_RESULTS,
    CALLBACK, solver_images, solver_case, put, address, execute, words,
    return_from_hook,
)


def as_float(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def as_bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('aliased', [False, True])
def test_circular_saved_body_pointer_capture(solver_images, precision, aliased):
    frame = STACK + 32768 - 0xd0
    a = frame - 0x10 if aliased else HEAP + 0x7000
    b = HEAP + 0x80
    delta = as_float(b + 0x100) - as_float(b)
    retail, compiled, symbols = solver_images
    targets = [(1., 1.), (-1., 1.), (2., -1.), (.5, 1.), (1., .5), (.5, -1.)]
    for rounding in range(4):
        for ta, tb in targets:
            for coupling in [0., .5]:
                impulse_a, impulse_b = ta + tb * coupling, tb + ta * coupling
                first = as_bits(as_float(b) + delta * impulse_a * ta) if aliased else b
                last = as_bits(as_float(first) + delta * impulse_b * tb) if aliased else b
                replacements = sorted({first, last} - {b})
                snapshots = []
                for label, machine, entry, gate in [
                    ('retail', retail, 0x31c940, 0x480c60),
                    ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
                     address(symbols, 'g_480c60')),
                ]:
                    data = solver_case(7, 2)
                    put(data, STEP, '7fi', 0, 1, 0, 1, 60, 0, 0, 1)
                    put(data, OPS, '6I', 0x11801, TERMS, RESULTS, a, b, 8)
                    put(data, OPS + 24, '3I', 0x20c0e, CALLBACK, 3)
                    put(data, OPS + 36, 'IIffI', 0x51407, FRICTION_RESULTS, coupling, 100, 12)
                    put(data, OPS + 56, 'I', 0x400)
                    put(data, HEAP, 'B', 3)
                    put(data, TERMS, '24f',
                        0, 0, 0, ta, 0, 0, 0, 0, 1, 0, 0, 1,
                        0, 0, 0, tb, 0, 0, 0, 0, 0, 1, 0, 1)
                    put(data, b + 0x10, '12f', *([0.] * 8 + [1.] * 4))
                    for replacement in replacements:
                        # Different weights expose reloading inertia through the
                        # redirected body pointer, even if its receiver was cached.
                        put(data, replacement + 0x10, '12f',
                            *([0.] * 8 + [3., 5., 7., 11.]))
                    if not aliased:
                        put(data, a + 0x10, '12f', *([0.] * 11 + [delta]))
                    put(data, IMPULSES, '2f', 0, 0)
                    machine.mem_write(frame - 0x10, b'\0' * 16)
                    machine.mem_write(frame + 0x10, b'\0' * 176)
                    machine.mem_write(gate, b'\x01')
                    callbacks, observations = [], []

                    def callback(uc, va, size, user):
                        # An actual external-op boundary supplies A's W weight.
                        # In the aliased case A's subsequent linear W publication
                        # changes the saved B body pointer, twice.
                        uc.mem_write(a + 0x3c, struct.pack('<f', delta))
                        callbacks.append(1)
                        for reg in [ux.UC_X86_REG_EAX, ux.UC_X86_REG_ECX, ux.UC_X86_REG_EDX]:
                            uc.reg_write(reg, 0x13579bdf)
                        return_from_hook(uc)

                    def observed(uc, va, size, user):
                        observations.append((va, words(uc, frame + 0xc, 1)[0],
                                             words(uc, frame + 0x14, 1)[0],
                                             uc.reg_read(ux.UC_X86_REG_EAX)))

                    hooks = [(CALLBACK, callback)]
                    if label == 'retail':
                        # Read-only retail observations: first inertia capture,
                        # then completion of both circular body publications.
                        hooks += [(0x31d6e0, observed), (0x31d783, observed)]
                    snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                       hooks=hooks, precision=precision | rounding << 10)
                    context = (label, precision, rounding, aliased, ta, tb, coupling)
                    assert callbacks == [1] and snapshot[1] == 1, context
                    assert bytes(machine.mem_read(b + 0x20, 16)) == struct.pack(
                        '<4f', impulse_a, impulse_b, 0, impulse_a + impulse_b), context
                    for replacement in replacements:
                        assert words(machine, replacement + 0x20, 4) == (0, 0, 0, 0), context
                    if label == 'retail':
                        assert len(observations) == 2, (context, observations)
                        assert observations[0] == (0x31d6e0, first, b + 0x20, b + 0x30), context
                        assert observations[1][:3] == (0x31d783, last, b + 0x20), context
                    snapshots.append(snapshot)
                # Exact heap bytes and return value, including replacement bodies.
                # execute also checks stack cleanup, saved registers and FP control.
                assert snapshots[0] == snapshots[1], (precision, rounding, aliased, ta, tb, coupling)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('active_row', [0, 1])
def test_circular_a_inertia_precedes_scalar_copy(solver_images, precision, active_row):
    frame = STACK + 32768 - 0xd0
    # A's inertia W overlaps the rounded SIMD input at frame+0x1c. Both
    # circular rows must capture it before publishing their scalar copy.
    # Reading the new impulse instead corrupts the retained B pointer when
    # A's angular W is written back into frame+0x0c.
    body_a = frame - 0x20
    body_b = HEAP + 0x80
    retail, compiled, symbols = solver_images
    snapshots = {}
    for label, machine, entry, gate in [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]:
        for rounding in range(4):
            data = solver_case(7, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 12, '2I', body_a, body_b)
            put(data, OPS + 24, '6I', 7 | (20 << 8) | (5 << 16),
                FRICTION_RESULTS, 0, 0x42c80000, 8, 0x400)
            for row in range(2):
                put(data, TERMS + row * 48, '12f',
                    0, 0, 0, float(row == active_row), 0, 0, 0, 1, 0, 0, 0, 1)
            put(data, HEAP, 'B', 3)
            for offset in [0x10, 0x20]:
                put(data, body_b + offset, '4f', 0, 0, 0, 0)
            put(data, body_b + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '2f', 0, 0)
            machine.mem_write(frame - 0x10, b'\0' * 16)
            machine.mem_write(frame + 0x10, b'\0' * 16)
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               precision=precision | (rounding << 10))
            context = (label, precision, rounding, active_row)
            b_home = words(machine, frame + 0xc, 1)
            assert b_home == (body_b,), (context, b_home)
            # Exact signed zero: downward rounding produces -0 for the inactive row.
            inactive = 0x80000000 if rounding == 1 else 0
            expected = (0x3f800000, inactive) if active_row == 0 else (inactive, 0x3f800000)
            assert words(machine, IMPULSES, 2) == expected, context
            assert snapshot[1] == 1, context
            if label == 'retail':
                snapshots[rounding] = (snapshot, b_home)
            else:
                assert snapshots[rounding] == (snapshot, b_home), context
