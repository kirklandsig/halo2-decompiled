"""Coupled normal impulses at float underflow and cancellation boundaries."""
import itertools
import math

import pytest

from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import put, STEP, OPS
from test_solver_normal_storage_precision import normal_case, compare_normal


@pytest.mark.parametrize('pair', [0, 1])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_coupled_impulses_compare_stored_float(solver_images, pair, precision):
    count = 0
    seen = set()
    for rounding, small_row, exponent, coefficient, sign, cancel in itertools.product(
            range(4), range(2), [-149, -148, -126, -125],
            [.25, .5, .75, 1, 1.5], [-1, 1], [False, True]):
        data, a, b, row = normal_case(pair)
        put(data, STEP, '7fi', 0, 1, 0, .3, 60, 0, 0, 1)
        for body in (a, b):
            put(data, body + 16, '4f', 0, 0, 0, 0)
            put(data, body + 32, '4f', 0, 0, 0, 0)
            put(data, body + 48, '4f', 1, 1, 1, 1)
        small = math.ldexp(1, exponent)
        for index in range(2):
            current = row + index * 48
            target = sign * coefficient if index == small_row else .5
            diagonal = small if index == small_row else 1
            put(data, current, '4f', 1, 0, 0, target)
            put(data, current + 16, '4f', 0, 0, 0, 1)
            put(data, current + 32, '4f', 0, 0, 0, diagonal)
        put(data, OPS + 28 + pair * 8, 'f', -small if cancel else 0)
        control = precision | rounding << 10
        key = control, bytes(data)
        assert key not in seen
        seen.add(key)
        context = ('coupled float boundary', pair, control, small_row,
                   exponent, coefficient, sign, cancel)
        compare_normal(solver_images, data, control, pair, context)
        count += 1
    assert count == len(seen) == 640
