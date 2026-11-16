"""Retail normal-pair storage boundaries across precision, rounding and aliases."""
import itertools
import random

import pytest
from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import solver_case, put, compare_case, HEAP, TERMS, STEP, OPS


def normal_case(pair, shared=False, alias=None):
    data = solver_case(3, 2)
    put(data, STEP, '7fi', 0, .6, .8, .3, 60, 0, 0, 1)
    put(data, OPS + (32 if pair == 0 else 40), 'I', 0x400)
    a, b = HEAP, HEAP if shared else HEAP + 0x80
    first = TERMS
    if alias is not None:
        shift, owner, offset, seed = alias
        rng = random.Random(seed)
        a, b = HEAP + 0x200, HEAP + 0x200 + shift
        for at in range(a - 0xa0, a + 0x140, 4):
            put(data, at, 'f', rng.uniform(-1, 1))
        second = (a if owner == 0 else b) + offset
        first = second - pair * 96
    second = first + pair * 96
    put(data, OPS + 4, 'I', first)
    put(data, OPS + 12, '2I', a, b)
    if pair:
        # Keep the first pair finite and inactive unless aliasing changes it.
        for at in range(first, first + 96, 16):
            put(data, at, '4f', 0, 0, 0, 0)
        for row in (first, first + 48):
            put(data, row + 12, 'f', -1)
            put(data, row + 28, 'f', 1)
            put(data, row + 44, 'f', 1)
        put(data, OPS + 28, 'f', 0)
    put(data, a, 'B', 2)
    put(data, b, 'B', 2)
    return data, a, b, second


def compare_normal(images, data, control, pair, context):
    # These retail instructions publish the two projections, stored first
    # error, coupled impulses and comparison. Require the intended pair to run.
    required = ([0x31ca14, 0x31ca63, 0x31ca76, 0x31ca97, 0x31caab, 0x31cab3]
                if pair == 0 else
                [0x31cdad, 0x31cdfb, 0x31ce32, 0x31ce44, 0x31ce4c])
    visits = dict.fromkeys(required, 0)
    snapshots = compare_case(images, data, control, context, retail_coverage=visits)
    assert snapshots[0] == snapshots[1], context
    assert all(count == 1 for count in visits.values()), (context, visits)


@pytest.mark.parametrize('pair', [0, 1])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_normal_pair_nonfinite_storage(solver_images, pair, precision):
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
        data, a, b, row = normal_case(pair, shared)
        coupling = OPS + 28 + pair * 8
        offset = lane * 4
        locations = [
            (row + 12, STEP + 8), (row + 44, row + 28),
            (row + 92, coupling), (row + 12, row + 60),
            (STEP + 4, STEP + 8), (a + 16 + offset, row + offset),
            (b + 32 + offset, row + 16 + offset), (coupling, STEP + 4),
        ][field]
        for at, bits in zip(locations, payload):
            put(data, at, 'I', bits)
        key = control, bytes(data)
        if key in seen:
            continue
        seen.add(key)
        context = ('normal stored values', pair, precision, rounding,
                   shared, lane, field, payload)
        compare_normal(solver_images, data, control, pair, context)
    assert len(seen) == 896


@pytest.mark.parametrize('pair', [0, 1])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_normal_pair_partial_storage_alias(solver_images, pair, precision):
    seen = set()
    for rounding, shift, owner, offset, seed in itertools.product(
            range(4), [-32, -16, 0, 16, 32], range(2), [16, 32, 48], range(6)):
        control = precision | rounding << 10
        alias = shift, owner, offset, seed
        data, _, _, _ = normal_case(pair, alias=alias)
        put(data, OPS + 28 + pair * 8, 'f', [0, .001, .2, 100, -.25, .75][seed])
        key = control, bytes(data)
        if key in seen:
            continue
        seen.add(key)
        context = ('normal storage alias', pair, precision, rounding, alias)
        compare_normal(solver_images, data, control, pair, context)
    assert len(seen) == 504
