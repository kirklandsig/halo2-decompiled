"""Retail clamp paths with competing payloads and directed x87 rounding."""
import itertools

import pytest

from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import solver_case, put, compare_case, HEAP, TERMS, STEP, OPS


@pytest.mark.parametrize('kind', [4, 5])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_bounded_clamp_payloads_and_rounding(solver_images, kind, precision):
    payloads = [
        (0x7fc00001, 0xffc54321), (0x7f800001, 0xffc12345),
        (0x7f800000, 0xff800000), (0, 0x80000000),
        (1, 0x80000001), (0x3fc00000, 0x3f800000),
        (0xbfc00000, 0x3f800000), (0x7f7fffff, 0x3f800000),
    ]
    # Projection, comparison, upper/lower selection, and common update in retail.
    sites = ([0x31d9c7, 0x31d9d9, 0x31d9e3, 0x31d9f2, 0x31d9f9]
             if kind == 4 else
             [0x31df92, 0x31dfa7, 0x31dfb1, 0x31dfc0, 0x31dfc7])
    paths = dict.fromkeys(['upper', 'lower', 'unclamped'], 0)
    seen = set()
    count = 0
    for rounding, shared, sign, field, payload in itertools.product(
            range(4), [False, True], [-1, 1], range(4), payloads):
        data = solver_case(kind, 2)
        put(data, STEP, '7fi', 0, 1, 1, 1, 60, 0, 0, 1)
        put(data, OPS + 24, 'I3fI', kind | 16 << 8 | (4 if kind == 4 else 3) << 16,
            .2, -.4, 0, 0x400)
        a, b = HEAP, HEAP if shared else HEAP + 0x80
        put(data, OPS + 16, 'I', b)
        for body in (a, b):
            put(data, body + 16, '8f', *([0] * 8))
            put(data, body + 48, '4f', 1, 1, 1, 1)
        if kind == 4:
            put(data, TERMS, '8f', 1, 0, 0, 1, 0, 1, 0, sign * 1.5)
            mass, target = TERMS + 12, TERMS + 28
        else:
            put(data, TERMS, '12f', 1, 0, 0, sign * 1.5,
                0, 1, 0, 1, 0, 0, 1, 0)
            mass, target = TERMS + 28, TERMS + 12
        locations = [(target, mass), (mass, STEP + 4),
                     (OPS + 28, OPS + 32), (target, OPS + 28)][field]
        for address, bits in zip(locations, payload):
            put(data, address, 'I', bits)
        control = precision | rounding << 10
        key = control, bytes(data)
        if key in seen:
            continue
        seen.add(key)
        visits = dict.fromkeys(sites, 0)
        context = ('bounded clamp', kind, control, shared, sign, field, payload)
        snapshots = compare_case(solver_images, data, control, context,
                                 retail_coverage=visits)
        assert snapshots[0] == snapshots[1], context
        assert all(visits[site] == 1 for site in (sites[0], sites[1], sites[4]))
        paths['upper'] += visits[sites[2]]
        paths['lower'] += visits[sites[3]]
        paths['unclamped'] += not (visits[sites[2]] or visits[sites[3]])
        count += 1
    assert count == len(seen) == 384
    assert all(paths.values()), paths
