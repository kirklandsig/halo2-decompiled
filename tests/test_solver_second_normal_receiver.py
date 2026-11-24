"""Retail second-pair fallback angular receiver capture boundaries."""
import struct

import pytest
from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, RESULTS, CALLBACK,
    solver_images, solver_case, put, address, execute, words, return_from_hook,
)


def as_float(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('aliased', [False, True])
@pytest.mark.parametrize('row_index', [0, 1])
def test_second_pair_fallback_angular_receiver(solver_images, precision, aliased, row_index):
    frame = STACK + 32768 - 0xd0
    a = frame if aliased else HEAP
    b, replacement = HEAP + 0x80, HEAP + 0x180
    delta = as_float(replacement + 0x20) - as_float(b + 0x20)
    retail, compiled, symbols = solver_images
    snapshots = {}
    for label, machine, entry, gate in [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]:
        for rounding in range(4):
            data = solver_case(3, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS, '6I', 0x11801, TERMS, RESULTS, a, b, 8)
            put(data, OPS + 24, '3I', 14 | (12 << 8) | (2 << 16), CALLBACK, 3)
            put(data, OPS + 36, '5I', 3 | (8 << 8) | (5 << 16), 0,
                3 | (8 << 8) | (5 << 16), 0, 0x400)
            put(data, TERMS, '48f', *([0.0] * 24 +
                [0, 1, 0, float(row_index == 0), 0, 0, 0, 1, 1, 0, 0, 1,
                 0, 1, 0, float(row_index == 1), 0, 0, 0, 1, 1, 0, 0, 1]))
            put(data, HEAP, 'B', 3)
            for body in [HEAP, b, replacement]:
                put(data, body + 0x10, '4f', 0, 0, 0, 0)
                put(data, body + 0x20, '4f', 1 if body == b else 5 if body == replacement else 0,
                    0, 0, 0)
                put(data, body + 0x30, '4f', 1, 1, 1, 1)
            put(data, IMPULSES, '4f', 0, 0, 0, 0)
            machine.mem_write(frame + 0x10, b'\0' * 160)
            callbacks, visits = [], []

            def callback(uc, va, size, user):
                # A's linear Y write replaces the published angular pointer.
                uc.mem_write(a + 0x30, struct.pack('<4f', 0, 0, 0, delta))
                callbacks.append(1)
                return_from_hook(uc)

            def visited(uc, va, size, user):
                visits.append(va)

            hooks = [(CALLBACK, callback)]
            if label == 'retail':
                hooks.append((0x31d0dc if row_index == 0 else 0x31d15a, visited))
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               hooks=hooks, precision=precision | rounding << 10)
            old = words(machine, b + 0x20, 4)
            new = words(machine, replacement + 0x20, 4)
            published = words(machine, frame + 0x14, 1)[0]
            assert published == (replacement + 0x20 if aliased else b + 0x20), (
                label, row_index, precision, rounding, published)
            redirected = aliased and row_index == 1
            assert old == ((0x3f800000, 0, 0, 0) if redirected else
                           (0x40000000, 0, 0, 0x3f800000)), (label, row_index, precision, rounding, old)
            assert new == ((0x40c00000, 0, 0, 0x3f800000) if redirected else
                           (0x40a00000, 0, 0, 0)), (label, row_index, precision, rounding, new)
            assert callbacks == [1]
            assert snapshot[1] == 1
            if label == 'retail':
                assert len(visits) == 1, (row_index, visits)
                snapshots[rounding] = (snapshot, old, new, published, callbacks)
            else:
                assert snapshots[rounding] == (snapshot, old, new, published, callbacks)
