"""Consecutive linear rows reload a receiver replaced by result publication."""

import struct

import pytest
from unicorn import UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_FPCW

from test_solver_dispatch import (
    HEAP, STACK, STEP, OPS, TERMS, RESULTS,
    solver_images, solver_case, put, address, execute,
)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('rounding', range(4))
def test_linear_rows_reload_receiver_after_result_store(
        solver_images, precision, rounding):
    frame = STACK + 32768 - 0xd0
    body_b = HEAP + 0x80
    first_receiver = body_b + 0x10
    output = frame + 0x20
    retail, compiled, symbols = solver_images
    control_word = precision | (rounding << 10)

    def float_bits(bits):
        return struct.unpack('<f', struct.pack('<I', bits))[0]

    # Exercise both directions. The backward receiver is A's otherwise unused
    # accumulated-linear vector; body kind 3 leaves that storage untouched.
    for second_receiver in [HEAP + 0x40, HEAP + 0x220]:
        delta = float_bits(second_receiver) - float_bits(first_receiver)
        data = solver_case(13, 2)
        put(data, STEP, '7fi', 0, 1, 1, 1, 1, 0, 0, 1)
        put(data, OPS, '6I', 0x11801, TERMS, RESULTS, HEAP, body_b, 8)
        put(data, OPS + 24, '3I', 0x3040d, 0x3040d, 0x400)
        for body in [HEAP, body_b]:
            put(data, body, 'B', 3)
            for offset in [0x10, 0x20, 0x40, 0x50]:
                put(data, body + offset, '4f', 0, 0, 0, 0)
            put(data, body + 0x30, '4f', 1, 1, 1, 1)
        put(data, TERMS, '24f',
            1, 0, 0, delta, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0)
        put(data, second_receiver, '4f', 0, 2, 0, 0)

        snapshots = []
        traces = []
        for label, machine, entry, gate in [
                ('retail', retail, 0x31c940, 0x480c60),
                ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))]:
            machine.mem_write(frame + 0x10, b'\0' * 160)
            machine.mem_write(gate, b'\x01')
            pointer_writes = []
            receiver_writes = []

            def observe(current, access, va, size, value, user):
                if va == output and size == 4:
                    pointer_writes.append(value & 0xffffffff)
                if va in [first_receiver, second_receiver] and size > 4:
                    # MOVAPS may be reported as two eight-byte writes.
                    receiver_writes.append(va)

            # The observer never changes memory, registers, or instructions.
            # The first ordinary FADD/FSTP changes the saved receiver itself.
            handle = machine.hook_add(UC_HOOK_MEM_WRITE, observe)
            try:
                snapshot = execute(
                    machine, entry, data, [STEP, OPS, HEAP, output],
                    precision=control_word)
            finally:
                machine.hook_del(handle)
            context = (label, precision, rounding, second_receiver)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert snapshot[1] == 1, context
            assert pointer_writes == [first_receiver, second_receiver], context
            assert receiver_writes == [first_receiver, second_receiver], context
            snapshots.append(snapshot)
            traces.append((pointer_writes, receiver_writes))
        assert snapshots[0] == snapshots[1], (precision, rounding, second_receiver)
        assert traces[0] == traces[1], (precision, rounding, second_receiver)
