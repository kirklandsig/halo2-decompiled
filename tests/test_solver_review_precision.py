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
