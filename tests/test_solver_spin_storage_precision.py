"""Stored spinning impulses across x87 precision and rounding modes."""
import pytest

from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import (
    compare_case, put, solver_case, HEAP, IMPULSES, OPS, STEP, TERMS,
    FRICTION_RESULTS,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('stage', ['initial', 'scaled'])
def test_spin_storage_rounding(solver_images, precision, stage):
    for rounding in (0, 0x400, 0x800, 0xc00):
        for seed in range(32):
            data = solver_case(8, 2)
            put(data, STEP, '7fi', 0, 1, 1, 1, 60, 0, 0, 1)
            put(data, OPS + 24, 'IIffIf', 0x61808, FRICTION_RESULTS,
                0, 1e30, 12, 1)
            put(data, OPS + 48, 'I', 0x400)
            if seed & 4:
                put(data, OPS + 16, 'I', HEAP)
            for body in (HEAP, HEAP + 0x80):
                for offset in (0x10, 0x20, 0x40, 0x50):
                    put(data, body + offset, '4f', 0, 0, 0, 0)
                put(data, body + 0x30, '4f', .25, .5, .75, 1)
            put(data, TERMS, '32f', *([0] * 32))
            axis = seed % 3
            put(data, TERMS + 0x60 + axis * 4, 'f', 1)
            sign = -1 if seed & 1 else 1
            if stage == 'initial':
                mass = (2.0 ** -126, 2.0 ** -100, 2.0 ** -20, 1)[seed % 4]
                target = sign * (2.0 ** -24, 2.0 ** -25,
                                 1 + 2.0 ** -23, 1 - 2.0 ** -24)[seed // 8]
                scale = 1.25 if seed & 2 else .75
                accumulated = 0
            else:
                exponent = 4 + seed % 20
                mass, target = 1, sign * (2.0 ** exponent + 1)
                scale = 1 + 2.0 ** -exponent
                accumulated = -sign * (2.0 ** exponent + 2)
            put(data, TERMS + 0x6c, 'f', mass)
            put(data, TERMS + 0x7c, 'f', target)
            put(data, OPS + 44, 'f', scale)
            put(data, IMPULSES, '3f', 0, 0, accumulated)
            context = ('spin storage', stage, seed, rounding)
            snapshots = compare_case(solver_images, data, precision | rounding, context)
            assert snapshots[0] == snapshots[1], (context, precision)
