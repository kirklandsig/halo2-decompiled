"""Exact three-row friction regressions after removing volatile tuning.

The real solver executes with competing NaN payloads, infinities and signed
zero. No tolerance or floating-point-status equivalence is claimed.
"""
import itertools

import pytest

from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import (
    solver_case, put, compare_case, HEAP, TERMS, STEP, OPS,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_contact_friction_nonfinite_rounding(solver_images, precision):
    payloads = [
        (0x7fc00001, 0x7fc54321), (0xffc54321, 0x7fc00001),
        (0x7f800001, 0xffc12345), (0xff800123, 0x7fc54321),
        (0x7fc12345, 0xffc12345), (0x7fc54321, 0x7f800002),
        (0x7f800000, 0xff800000), (0x80000000, 0),
    ]
    seen = set()
    for rounding, shared, lane, field, payload in itertools.product(
            range(4), [False, True], range(4), range(8), payloads):
        control = precision | rounding << 10
        data = solver_case(8, 2)
        put(data, STEP, '7fi', 0, .6, .8, .3, 60, 0, 0, 1)
        put(data, OPS + 48, 'I', 0x400)
        a, b = HEAP, HEAP if shared else HEAP + 0x80
        put(data, OPS + 16, 'I', b)
        offset = lane * 4
        first, second = [
            (TERMS + 12, TERMS + 44), (TERMS + 60, TERMS + 92),
            (TERMS + 108, TERMS + 124), (OPS + 32, STEP + 12),
            (a + 32 + offset, TERMS + 16 + offset),
            (b + 16 + offset, TERMS + 48 + offset),
            (OPS + 36, TERMS + 44), (STEP + 8, STEP + 12),
        ][field]
        put(data, first, 'I', payload[0])
        put(data, second, 'I', payload[1])
        key = control, bytes(data)
        if key in seen:
            continue
        seen.add(key)
        context = ('three-row friction', precision, rounding, shared, lane,
                   field, payload)
        snapshots = compare_case(solver_images, data, control, context)
        assert snapshots[0] == snapshots[1], context
    assert len(seen) == 896


@pytest.mark.parametrize('kind', [4, 5])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_bounded_impulse_captured_inputs_rounding(solver_images, precision, kind):
    """Compare captured scalars through projection, both clamps and writeback."""
    payloads = [
        (0x7fc00001, 0x7fc54321), (0xffc54321, 0x7fc00001),
        (0x7f800001, 0xffc12345), (0xff800123, 0x7fc54321),
        (0x7f800000, 0xff800000), (0x80000000, 0),
        (1, 0x80000001), (0x7f7fffff, 0x3f800001),
        (0x40400000, 0x3f800000), (0xc0400000, 0x3f800000),
        (0x3f000001, 0x3f7fffff), (0xbf000001, 0x3f800001),
    ]
    target = TERMS + (28 if kind == 4 else 12)
    mass = TERMS + (12 if kind == 4 else 44)
    fields = [
        (target, mass),
        (target, STEP + 4),
        (target, OPS + 28),
        (target, OPS + 32),
        (target, HEAP + 32),
        (mass, TERMS + 16),
    ]
    for rounding, shared, field, payload in itertools.product(
            range(4), [False, True], fields, payloads):
        data = solver_case(kind, 2)
        put(data, STEP, '7fi', 0, .6, .8, .3, 60, 0, 0, 1)
        put(data, OPS + 16, 'I', HEAP if shared else HEAP + 0x80)
        put(data, OPS + 24, 'I2fI', kind | 16 << 8 | (4 if kind == 4 else 3) << 16, 2, -2, 0)
        put(data, OPS + 40, 'I', 0x400)
        put(data, TERMS, '12f', .25, -.5, .75, .5,
            .125, .25, -.125, 1, -.25, .125, .25, 1)
        put(data, field[0], 'I', payload[0])
        put(data, field[1], 'I', payload[1])
        context = ('bounded impulse captured inputs', kind, precision, rounding,
                   shared, field, payload)
        snapshots = compare_case(solver_images, data,
                                 precision | rounding << 10, context)
        assert snapshots[0] == snapshots[1], context


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_spin_target_scale_nonfinite_products(solver_images, precision):
    payloads = [
        (0x7fc12345, 0xffc12345), (0xffc12345, 0x7fc12345),
        (0x7f812345, 0xffc12345), (0xff812345, 0x7fc12345),
        (0x7fc00001, 0x7fc54321), (0x7fc54321, 0x7fc00001),
        (0x7f800000, 0), (0xff800000, 0x80000000),
        (0, 0x80000000), (0x80000000, 0),
        (1, 0x3f800001), (0x80000001, 0x3f7fffff),
        (0x7f7fffff, 0x40000000), (0xff7fffff, 0x40000000),
        (0x3f000001, 0x3f7fffff), (0xbf000001, 0x3f800001),
    ]
    # Observe the real first tangential product; the hook only counts visits.
    coverage = {0x31d2fe: 0}
    for rounding, shared, target, payload in itertools.product(
            range(4), [False, True], [TERMS + 12, TERMS + 60, TERMS + 124],
            payloads):
        data = solver_case(8, 2)
        put(data, STEP, '7fi', 0, .6, .8, .3, 60, 0, 0, 1)
        put(data, OPS + 16, 'I', HEAP if shared else HEAP + 0x80)
        put(data, OPS + 48, 'I', 0x400)
        put(data, target, 'I', payload[0])
        put(data, STEP + 12, 'I', payload[1])
        context = ('spin target-scale products', precision, rounding,
                   shared, target, payload)
        snapshots = compare_case(solver_images, data,
                                 precision | rounding << 10, context, coverage)
        assert snapshots[0] == snapshots[1], context
    assert coverage[0x31d2fe] == 384


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('rounding', range(4))
@pytest.mark.parametrize('aliased', [False, True])
def test_spin_limit_division_reloads_operation_record(
        solver_images, precision, rounding, aliased):
    """The saved comparison limit and later division operand have distinct epochs."""
    import struct
    from test_solver_dispatch import STACK, RESULTS, IMPULSES, execute, address, words

    frame = STACK + 32768 - 0xd0
    a, b, output = 0x02000000, 0x08001000, 0x10300000
    operation = frame + 0xa if aliased else OPS + 24
    header = operation - 24
    # The caller's saved values make the overlapping header survive the real
    # prologue: A/B remain aligned, and the operation starts with bytes 08/18.
    # No register or instruction is changed while the solver executes.
    saved_values = (0x18080000, 0x00000800, 0x10000200, 0x12345678)
    retail, compiled, symbols = solver_images
    snapshots = []
    visits = []
    for label, machine, entry, gate in [
            ('retail', retail, 0x31c940, 0x480c60),
            ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
             address(symbols, 'g_480c60'))]:
        for start, size in [(a, 65536), (b & ~65535, 65536), (output, 131072)]:
            if not any(lo <= start and start + size - 1 <= hi
                       for lo, hi, permissions in machine.mem_regions()):
                machine.mem_map(start, size)
            machine.mem_write(start, b'\0' * size)
        data = bytearray(65536)
        put(data, HEAP, 'B', 3)
        put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
        put(data, TERMS, '32f',
            1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1,
            0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
            0, 0, 1, 1, 0, 0, 0, 0)
        # Cancels the tiny coupled cross term exactly in the nearest mode.
        put(data, TERMS + 0x3c, 'I', 0x80008000)
        for body in [a, b]:
            machine.mem_write(body + 0x30, struct.pack('<4f', 1, 1, 1, 1))
        machine.mem_write(frame - 256, b'\0' * 512)
        header_bytes = struct.pack('<6I', 0x11801, TERMS, RESULTS, a, b, 0)
        # Classification zero skips the residual pass, keeping this regression
        # focused on the real three-row update, clamp and result writeback.
        operation_bytes = struct.pack('<IIIfIfI', 0x1808, output + 0x800,
                                      0x00004000, 1, 8, 1, 0x400)
        if aliased:
            machine.mem_write(header, header_bytes)
            machine.mem_write(operation, operation_bytes)
        else:
            data[header - HEAP:header - HEAP + 24] = header_bytes
            data[operation - HEAP:operation - HEAP + 28] = operation_bytes
        machine.mem_write(gate, b'\1')

        def observe_limit(current, address_, size_, user):
            # Read-only observer at retail's real FSQRT, before its FDIVR.
            assert words(current, header + 12, 2) == (a, b)
            assert bytes(current.mem_read(operation, 2)) == b'\x08\x18'
            visits.append((words(current, frame + 0x34, 1)[0],
                           words(current, operation + 12, 1)[0]))

        hooks = [(0x31d375, observe_limit)] if label == 'retail' else ()
        snapshot = execute(machine, entry, data, [STEP, header, HEAP, IMPULSES],
                           hooks=hooks, precision=precision | rounding << 10,
                           callee_saved_values=saved_values)
        assert snapshot[1] == 1
        snapshots.append((snapshot, bytes(machine.mem_read(a, 65536)),
                          bytes(machine.mem_read(b & ~65535, 65536)),
                          bytes(machine.mem_read(output, 131072))))
        if label == 'retail' and rounding == 0:
            assert words(machine, IMPULSES, 1) == ((0,) if aliased else (0x3f800000,))
    expected = ((0x3f808000, 0x80008000) if rounding == 1
                else (0x3f800000, 0x80000000)) if aliased else (0x3f800000, 0x3f800000)
    assert visits == [expected], (precision, rounding, aliased, visits)
    assert snapshots[0] == snapshots[1], (precision, rounding, aliased)
