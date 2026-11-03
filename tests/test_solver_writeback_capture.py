"""Differential coverage for paired writeback with aliased input arguments."""
import pytest
from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, IMPULSES, solver_images,
    solver_case, put, address, execute, words,
)

@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_paired_writeback_captures_step_after_first_output(solver_images, precision):
    data = solver_case(3, 0)
    replacement_step = STEP + 0x80
    put(data, STEP + 28, 'i', 0)
    put(data, replacement_step, '7fi', 0, 0, 0, 0, 2, 0, 0, 0)
    # The first result replaces the incoming step argument. The existing
    # operation/body/result cursors are already captured in registers.
    stack_arguments = STACK + 32768 + 4
    put(data, OPS + 8, 'I', stack_arguments)
    put(data, OPS + 20, 'I', 8)
    put(data, OPS + 24, '3I', 3 | 8 << 8 | 5 << 16, 0, 0x400)
    put(data, TERMS + 12, 'f', 1.25)
    put(data, TERMS + 60, 'f', -2)
    put(data, IMPULSES, '2I', replacement_step, 0x3f800000)
    retail, compiled, symbols = solver_images
    snapshots = []
    for machine, entry, gate in (
            (retail, 0x31c940, 0x480c60),
            (compiled, address(symbols, 'solve_operations_31c940'),
             address(symbols, 'g_480c60'))):
        machine.mem_write(gate, b'\x01')
        snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES], precision=precision)
        output = words(machine, stack_arguments, 4)
        assert output == (replacement_step, 0x40200000, 0x3f800000, 0xc0800000)
        assert snapshot == (bytes(data), 1)
        snapshots.append((snapshot, output))
    assert snapshots[0] == snapshots[1]
