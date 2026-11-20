"""Zero-opcode dispatch must not dereference unused header pointers."""
import pytest

from test_solver_rows import solver_images  # noqa: F401
from test_solver_dispatch import compare_case, put, solver_case, OPS, STEP


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_empty_stream_with_unused_header_pointers(solver_images, precision):
    for iterations in (0, 1, 3):
        for unused in ('terms', 'bodies', 'all'):
            for sentinel in (0, 0x400):
                data = solver_case(2, 2)
                put(data, STEP + 28, 'i', iterations)
                put(data, OPS + 24, 'I', sentinel)
                if unused in ('terms', 'all'):
                    put(data, OPS + 4, 'I', 0)
                if unused in ('bodies', 'all'):
                    put(data, OPS + 12, '2I', 0, 0)
                context = ('empty stream', iterations, unused, sentinel)
                snapshots = compare_case(solver_images, data, precision, context)
                assert snapshots[0] == snapshots[1], (context, precision)
