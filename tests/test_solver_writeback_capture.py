"""Differential coverage for aliased solver inputs and writeback storage."""
import struct

import pytest
from unicorn.x86_const import UC_X86_REG_ESP
from test_solver_dispatch import (
    HEAP, STACK, STOP, STEP, OPS, TERMS, IMPULSES, FRICTION_RESULTS, CALLBACK, solver_images,
    solver_case, put, address, execute, words, return_from_hook,
)


@pytest.mark.parametrize('kind', [7, 8])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('local_home', [False, True])
def test_friction_writeback_retains_time_at_each_output_boundary(
        solver_images, kind, precision, local_home):
    for rounding in range(4):
        data = solver_case(kind, 0)
        replacement_step = STEP + 0x100
        put(data, STEP, '7fi', 0, 0, 0, 0, 1, 0, 0, 0)
        put(data, replacement_step, '7fi', 0, 0, 0, 0, 2, 0, 0, 0)
        size, row_kind = (20, 5) if kind == 7 else (24, 6)
        # Retail aligns ESP, reserves 0xB4 bytes, then saves three registers.
        # Alias either the incoming step argument or its retained time home.
        output_address = STACK + 32768
        replacement = replacement_step
        if local_home:
            output_address += -0xd0 + 0xc
            replacement += 16
        put(data, OPS + 24, 'I', kind | (size << 8) | (row_kind << 16))
        put(data, OPS + 28, 'I', output_address)
        put(data, OPS + 40, 'i', 8)
        put(data, OPS + 24 + size, 'I', 0x400)
        # Preserve the return word in the incoming-argument case. The first
        # error stores a pointer's exact float representation into the alias.
        put(data, IMPULSES, '3I', STOP, 0x3f800000, 0x3f000000)
        put(data, TERMS + 12, 'I', replacement)
        put(data, TERMS + 60, 'f', 2)
        put(data, TERMS + 124, 'f', -3)
        expected = (STOP, replacement, 0x3f800000,
                    0x40800000 if local_home else 0x40000000)
        if kind == 8:
            expected += (0x3f000000, 0xc0c00000 if local_home else 0xc0400000)
        retail, compiled, symbols = solver_images
        snapshots = []
        for machine, entry, gate in (
                (retail, 0x31c940, 0x480c60),
                (compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))):
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               precision=precision | (rounding << 10))
            output = words(machine, output_address, len(expected))
            assert snapshot == (bytes(data), 1)
            assert output == expected, (kind, precision, rounding, local_home)
            snapshots.append((snapshot, output))
        assert snapshots[0] == snapshots[1]

@pytest.mark.parametrize('kind', [7, 8])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_friction_writeback_reads_first_error_before_time_home(
        solver_images, kind, precision):
    for rounding in range(4):
        data = solver_case(kind, 0)
        put(data, STEP, '7fi', 0, 0, 0, 0, 1, 0, 0, 0)
        frame = STACK + 32768 - 0xd0
        # Only scalar writeback runs. Its first coefficient aliases the time
        # home. Seed the defined stack inputs before the function prologue.
        put(data, OPS + 4, 'I', frame + 4)
        size, row_kind = (20, 5) if kind == 7 else (24, 6)
        put(data, OPS + 24, 'I', kind | (size << 8) | (row_kind << 16))
        put(data, OPS + 28, 'I', FRICTION_RESULTS)
        put(data, OPS + 40, 'i', 8)
        put(data, OPS + 24 + size, 'I', 0x400)
        put(data, IMPULSES, '3f', 1.25, -2, 0.5)
        expected = (0x3fa00000, 0x40000000, 0xc0000000, 0x40400000)
        if kind == 8:
            expected += (0x3f000000, 0x40800000)
        expected_heap = bytearray(data)
        put(expected_heap, FRICTION_RESULTS, str(len(expected)) + 'I', *expected)
        retail, compiled, symbols = solver_images
        snapshots = []
        for machine, entry, gate in (
                (retail, 0x31c940, 0x480c60),
                (compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))):
            for offset, value in [(0x10, 2), (0x40, 3), (0x80, 4)]:
                machine.mem_write(frame + offset, struct.pack('<f', value))
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               precision=precision | (rounding << 10))
            output = words(machine, FRICTION_RESULTS, len(expected))
            assert output == expected, (kind, precision, rounding)
            assert snapshot == (bytes(expected_heap), 1)
            snapshots.append((snapshot, output))
        assert snapshots[0] == snapshots[1]


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_unilateral_retains_b_angular_receiver_before_a_write(
        solver_images, precision):
    def as_float(bits):
        return struct.unpack('<f', struct.pack('<I', bits))[0]

    frame = STACK + 32768 - 0xd0
    body_a = frame - 0x10
    original_b = HEAP + 0xa0
    replacement_b = original_b + 0x100
    axis = (as_float(replacement_b) - as_float(original_b)) / as_float(body_a)
    for rounding in range(4):
        data = solver_case(2, 0)
        put(data, STEP, '7fi', 0, 1, 0, 0, 1, 0, 0, 1)
        put(data, OPS + 12, '2I', body_a, HEAP + 0x80)
        put(data, OPS + 24, '2I', 2 | (4 << 8) | (3 << 16), 0x400)
        put(data, TERMS, '12f', 0, 0, 0, 1, 0, axis, 0, 1, 1, 0, 0, 0)
        put(data, HEAP, 'B', 3)
        put(data, HEAP + 0x90, '4f', 0, 0, 0, 0)
        put(data, original_b, '4f', 0, 0, 0, 0)
        put(data, HEAP + 0xb0, '4f', 1, 1, 1, 0)
        put(data, replacement_b, '4f', 0, 0, 0, 0)
        put(data, IMPULSES, 'f', 0)
        retail, compiled, symbols = solver_images
        snapshots = []
        for machine, entry, gate in (
                (retail, 0x31c940, 0x480c60),
                (compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))):
            # A's linear vector covers the saved registers but has zero delta.
            # Its angular Y component replaces the B-angular pointer at +0x14.
            machine.mem_write(frame + 0x10, b'\0' * 32)
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, IMPULSES],
                               precision=precision | (rounding << 10))
            actual_pointer = words(machine, frame + 0x14, 1)[0]
            assert actual_pointer == replacement_b, (precision, rounding)
            old_output = words(machine, original_b, 4)
            new_output = words(machine, replacement_b, 4)
            assert old_output == (0x3f800000, 0, 0, 0)
            assert new_output == (0, 0, 0, 0)
            assert snapshot[1] == 1
            snapshots.append((snapshot, old_output, new_output, actual_pointer))
        assert snapshots[0] == snapshots[1]


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


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_paired_writeback_preserves_impulse_bits_with_partial_overlap(
        solver_images, precision):
    patterns = [0, 0x80000000, 1, 0x80000001, 0x7f800001,
                0xff800001, 0x7fc12345, 0xffc56789]
    for rounding in range(4):
        for bits in patterns:
            for output in [FRICTION_RESULTS, IMPULSES + 1, IMPULSES - 3]:
                data = solver_case(3, 0)
                put(data, STEP, '7fi', 0, 0, 0, 0, 2, 0, 0, 0)
                put(data, OPS + 8, 'I', output)
                put(data, OPS + 20, 'I', 8)
                put(data, OPS + 24, '3I', 3 | 8 << 8 | 5 << 16, 0, 0x400)
                put(data, TERMS + 12, 'f', 1.25)
                put(data, TERMS + 60, 'f', -2)
                put(data, IMPULSES, '2I', bits, 0x7f800123)
                # Only writeback runs. Impulses retain their raw payloads,
                # including signaling NaNs; coefficients use numeric x87.
                # The second impulse is read after both first-result writes.
                expected_heap = bytearray(data)
                put(expected_heap, output, 'I', bits)
                put(expected_heap, output + 4, 'I', 0x40200000)
                second = struct.unpack_from('<I', expected_heap,
                                            IMPULSES + 4 - HEAP)[0]
                put(expected_heap, output + 8, 'I', second)
                put(expected_heap, output + 12, 'I', 0xc0800000)
                expected_output = (bits, 0x40200000, second, 0xc0800000)
                retail, compiled, symbols = solver_images
                snapshots = []
                for machine, entry, gate in (
                        (retail, 0x31c940, 0x480c60),
                        (compiled, address(symbols, 'solve_operations_31c940'),
                         address(symbols, 'g_480c60'))):
                    machine.mem_write(gate, b'\x01')
                    snapshot = execute(machine, entry, data,
                                       [STEP, OPS, HEAP, IMPULSES],
                                       precision=precision | (rounding << 10))
                    observed = words(machine, output, 4)
                    assert observed == expected_output, (precision, rounding,
                                                         bits, output)
                    assert snapshot == (bytes(expected_heap), 1)
                    snapshots.append((snapshot, observed))
                assert snapshots[0] == snapshots[1]


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_angular_error_reloads_step_between_rows(solver_images, precision):
    data = solver_case(12, 2)
    replacement_step = STEP + 0x100

    def as_float(bits):
        return struct.unpack('<f', struct.pack('<I', bits))[0]

    # Actual impulse accumulation replaces the incoming step pointer after
    # the first row. The next row must use the replacement's error scale.
    delta = as_float(replacement_step) - as_float(STEP)
    put(data, STEP, '7fi', 0, 1, 0, 0, 1, 0, 0, 1)
    put(data, replacement_step, '7fi', 0, 2, 0, 0, 1, 0, 0, 1)
    put(data, OPS + 24, '3I', 0x4040c, 0x4040c, 0x400)
    for body in (HEAP, HEAP + 0x80):
        for offset in (0x10, 0x20, 0x30, 0x40, 0x50):
            put(data, body + offset, '4f', 0, 0, 0, 0)
    for index, target in enumerate((delta, 3)):
        put(data, TERMS + index * 32, '8f', 0, 0, 0, 1, 0, 0, 0, target)

    stack_arguments = STACK + 32768 + 4
    retail, compiled, symbols = solver_images
    snapshots = []
    for machine, entry, gate in (
            (retail, 0x31c940, 0x480c60),
            (compiled, address(symbols, 'solve_operations_31c940'),
             address(symbols, 'g_480c60'))):
        # The second accumulated impulse occupies the operations argument.
        # Supply valid empty streams at both possible float encodings so a
        # stale-step implementation also returns normally and can be compared.
        for empty_stream in (0x40400000, 0x40c00000):
            if not any(lo <= empty_stream <= hi
                       for lo, hi, _ in machine.mem_regions()):
                machine.mem_map(empty_stream, 4096)
            machine.mem_write(empty_stream, b'\0' * 28)
        machine.mem_write(gate, b'\x01')
        snapshot = execute(machine, entry, data,
                           [STEP, OPS, HEAP, stack_arguments], precision=precision)
        output = words(machine, stack_arguments, 4)
        assert output == (replacement_step, 0x40c00000, HEAP, stack_arguments)
        assert snapshot == (bytes(data), 1)
        snapshots.append((snapshot, output))
    assert snapshots[0] == snapshots[1]


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_linear_limit_reloads_step_between_bounds(solver_images, precision):
    def as_float(bits):
        return struct.unpack('<f', struct.pack('<I', bits))[0]

    for rounding in range(4):
        data = solver_case(11, 2)
        replacement_step = STEP + 0x100
        final_step = STEP - 0x100
        delta = as_float(replacement_step) - as_float(STEP)
        for location, gain in ((STEP, 1), (replacement_step, 2), (final_step, 1)):
            put(data, location, '7fi', 0, gain, 0, 0, 1, 0, 0, 1)
        put(data, OPS + 24, 'I2fI', 11 | 12 << 8 | 3 << 16, 0, 2 * delta, 0x400)
        for body in (HEAP, HEAP + 0x80):
            for offset in (0x10, 0x20, 0x30, 0x40, 0x50):
                put(data, body + offset, '4f', 0, 0, 0, 0)
        put(data, TERMS, '12f', 0, 0, 0, delta, 0, 0, 0, 1, 0, 0, 0, 0)

        # Accumulating the lower-bound impulse replaces the step argument.
        # The upper bound must reload its scale: gain 2 moves the argument to
        # final_step, whereas retaining gain 1 would restore the original STEP.
        stack_arguments = STACK + 32768 + 4
        retail, compiled, symbols = solver_images
        snapshots = []
        for machine, entry, gate in (
                (retail, 0x31c940, 0x480c60),
                (compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))):
            observed = []

            def after_lower_bound(uc, address_, size_, user):
                observed.append(words(uc, stack_arguments, 1)[0])

            hooks = [(0x31de83, after_lower_bound)] if machine is retail else ()
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data,
                               [STEP, OPS, HEAP, stack_arguments], hooks=hooks,
                               precision=precision | (rounding << 10))
            output = words(machine, stack_arguments, 4)
            assert output == (final_step, OPS, HEAP, stack_arguments)
            if machine is retail:
                assert observed == [replacement_step]
            snapshots.append((snapshot, output))
        assert snapshots[0] == snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_bounded_linear_retains_b_receivers_before_a_write(solver_images,precision):
    from unicorn.x86_const import UC_X86_REG_ESP

    def as_float(bits):
        return struct.unpack('<f',struct.pack('<I',bits))[0]

    frame=STACK+32768-0xd0
    body_a=frame-0x10
    original_b=HEAP+0x80
    replacement_b=original_b+0x100
    delta=as_float(replacement_b)-as_float(original_b)
    for rounding in range(4):
        data=solver_case(5,0)
        put(data,STEP,'7fi',0,1/delta,0,0,1,0,0,1)
        put(data,OPS+12,'2I',body_a,original_b)
        put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
        put(data,OPS+36,'I2fI',5|(16<<8)|(3<<16),2,-2,0)
        put(data,OPS+52,'I',0x400)
        put(data,TERMS,'12f',0,0,0,delta,0,0,0,1,1,0,0,0)
        put(data,HEAP,'B',3)
        put(data,original_b+0x10,'4f',0,0,0,0)
        put(data,original_b+0x20,'4f',0,0,0,0)
        put(data,original_b+0x30,'4f',1,1,1,0)
        put(data,replacement_b+0x10,'4f',0,0,0,0)
        put(data,replacement_b+0x20,'4f',0,0,0,0)
        put(data,IMPULSES,'f',0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in (
                (retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                callbacks.append((terms,payload))
                # A's inertia W aliases the iteration word. The callback is
                # the sole synthetic boundary; the solver arithmetic is real.
                uc.mem_write(frame+0x2c,struct.pack('<I',0x3f800000))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            actual_pointer=words(machine,frame+0xc,1)[0]
            assert callbacks==[(TERMS,OPS+32)]
            assert actual_pointer==replacement_b,('pointer',precision,rounding,hex(actual_pointer),hex(replacement_b))
            old_output=words(machine,original_b+0x20,4)
            new_output=words(machine,replacement_b+0x20,4)
            assert old_output==(0x3f800000,0,0,0),('retail'if machine is retail else'compiled',precision,rounding,old_output,new_output)
            assert new_output==(0,0,0,0)
            assert snapshot[1]==1
            snapshots.append((snapshot,old_output,new_output,actual_pointer,callbacks))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('first_row',[False,True])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_normal_pair_keeps_linear_receiver_before_a_write(solver_images,precision,first_row):
    from unicorn.x86_const import UC_X86_REG_ESP

    def as_float(bits):
        return struct.unpack('<f',struct.pack('<I',bits))[0]

    def as_bits(value):
        return struct.unpack('<I',struct.pack('<f',value))[0]

    frame=STACK+32768-0xd0
    body_a=frame-0x10
    original_b=HEAP+0x80
    replacement_b=original_b+0x100
    delta=as_float(replacement_b)-as_float(original_b)
    for rounding in range(4):
        data=solver_case(3,0)
        put(data,STEP,'7fi',0,1,1,0,1,0,0,1)
        put(data,OPS+12,'2I',body_a,original_b)
        put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
        put(data,OPS+36,'IfI',3|(8<<8)|(5<<16),0,0x400)
        put(data,TERMS,'24f',*(
            (0,0,0,delta, 0,0,0,0, 0,0,0,1/delta,
             0,0,0,0, 0,1,0,0, 0,0,0,1) if first_row else
            (0,0,0,0, 0,1,0,0, 0,0,0,1,
             0,0,0,delta, 0,0,0,0, 0,0,0,1/delta)))
        put(data,HEAP,'B',3)
        for body in (original_b,replacement_b):
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'2f',0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                callbacks.append((terms,payload))
                uc.mem_write(frame+0x14,struct.pack('<f',-1))
                uc.mem_write(frame+0x2c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            assert callbacks==[(TERMS,OPS+32)]
            actual_pointer=words(machine,frame+0xc,1)[0]
            assert actual_pointer==replacement_b,('pointer',precision,rounding,hex(actual_pointer))
            old_linear=words(machine,original_b+0x10,4)
            new_linear=words(machine,replacement_b+0x10,4)
            assert old_linear==(0,0,0,as_bits(-delta)),('retail'if machine is retail else'compiled',precision,rounding,old_linear,new_linear)
            assert new_linear==(0,0,0,0)
            assert words(machine,IMPULSES,2)==(0x3f800000,0x3f800000)
            assert snapshot[1]==1
            snapshots.append((snapshot,old_linear,new_linear,actual_pointer,callbacks))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_keeps_angular_receiver_before_a_write(solver_images,precision):
    from unicorn.x86_const import UC_X86_REG_ESP

    def as_float(bits):
        return struct.unpack('<f',struct.pack('<I',bits))[0]

    frame=STACK+32768-0xd0
    body_a=frame-0x20
    original_b=HEAP+0x80
    next_b=original_b+0x100
    final_b=next_b+0x100
    delta=as_float(next_b)-as_float(original_b)
    for rounding in range(4):
        data=solver_case(3,0)
        put(data,STEP,'7fi',0,1,0,0,1,0,0,1)
        put(data,OPS+12,'2I',body_a,original_b)
        put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
        put(data,OPS+36,'IfI',3|(8<<8)|(5<<16),0,0x400)
        put(data,TERMS,'24f',
            0,0,0,1, 0,0,0,delta, 0,0,0,1,
            0,0,0,1, 0,0,0,delta, 0,0,0,1)
        put(data,HEAP,'B',3)
        for body in (original_b,next_b,final_b):
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'2f',0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                callbacks.append((terms,payload))
                uc.mem_write(frame+0x1c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            assert callbacks==[(TERMS,OPS+32)]
            actual_pointer=words(machine,frame+0xc,1)[0]
            assert actual_pointer==final_b,('pointer',precision,rounding,hex(actual_pointer))
            outputs=[words(machine,body+0x20,4)for body in(original_b,next_b,final_b)]
            assert outputs==[(0,0,0,0x3f800000),(0,0,0,0x3f800000),(0,0,0,0)],('retail'if machine is retail else'compiled',precision,rounding,outputs)
            assert words(machine,IMPULSES,2)==(0x3f800000,0x3f800000)
            assert snapshot[1]==1
            snapshots.append((snapshot,outputs,actual_pointer,callbacks))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('alias_kind',['inverse_b','linear_axis'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_linear_error_captures_axis_and_reloads_inertia(solver_images,precision,alias_kind):
    frame=STACK+32768-0xd0
    body_b=frame+0x20 if alias_kind=='inverse_b' else HEAP+0x80
    for rounding in range(4):
        data=solver_case(13,0)
        put(data,STEP,'7fi',0,1,1,0,1,0,0,1)
        put(data,OPS+12,'2I',HEAP,body_b)
        if alias_kind=='linear_axis':put(data,OPS+4,'I',frame+0x50)
        put(data,OPS+24,'2I',13|(4<<8)|(3<<16),0x400)
        put(data,TERMS,'12f',-1,0,0,2, 0,0,0,1, 0,0,0,0)
        put(data,HEAP,'B',3)
        put(data,HEAP+0x10,'4f',0,0,0,0)
        put(data,HEAP+0x20,'4f',0,0,0,0)
        put(data,HEAP+0x30,'4f',1,1,1,1)
        if alias_kind=='linear_axis':
            put(data,body_b+0x10,'4f',1,0,0,0)
            put(data,body_b+0x20,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'f',0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            machine.mem_write(frame+0x30,b'\0'*0x90)
            if alias_kind=='linear_axis':
                machine.mem_write(frame+0x50,struct.pack('<12f',-1,0,0,2,0,0,0,1,0,0,0,0))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            projection=words(machine,frame+0x5c,1)[0]
            result=words(machine,body_b+0x10,4)
            if alias_kind=='inverse_b':assert projection==0x3f800000
            assert result==(0x40000000,0,0,0xc0000000),('retail'if machine is retail else'compiled',precision,rounding,result)
            assert words(machine,IMPULSES,1)==(0x3f800000,)
            assert snapshot[1]==1
            snapshots.append((snapshot,projection,result))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_circular_second_projection_observes_angular_home(solver_images,precision):
    frame=STACK+32768-0xd0
    body_a=frame-0x10
    body_b=HEAP+0x80
    for rounding in range(4):
        data=solver_case(7,0)
        put(data,STEP,'7fi',0,1,1,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,body_b)
        put(data,OPS+24,'6I',7|(20<<8)|(5<<16),FRICTION_RESULTS,0,0,8,0x400)
        put(data,TERMS,'24f',
            0,0,0,0, 0,0,0,1, 0,0,0,1,
            0,0,0,0, 0,1,0,1, 0,0,0,1)
        put(data,HEAP,'B',3)
        put(data,body_b+0x10,'4f',0,0,0,0)
        put(data,body_b+0x20,'4f',0,0,0,0)
        put(data,body_b+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'2f',0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            machine.mem_write(frame+0x10,b'\0'*0xb0)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            projection=words(machine,frame+0xb8,1)
            assert projection==(body_b+0x20,),('retail'if machine is retail else'compiled',precision,rounding,projection)
            assert snapshot[1]==1
            snapshots.append((snapshot,projection))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_first_projection_precedes_linear_home(solver_images,precision):
    frame=STACK+32768-0xd0
    body_b=HEAP+0x80
    for rounding in range(4):
        data=solver_case(8,0)
        put(data,STEP,'7fi',0,1,1,1,1,0,0,1)
        put(data,OPS+12,'2I',frame,body_b)
        put(data,OPS+24,'7I',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,0,8,0x3f800000,0x400)
        put(data,TERMS,'32f',
            0,0,0,0, 1,0,0,1, 0,0,0,1,
            0,0,0,0, 0,0,0,1, 0,0,0,1,
            0,0,0,1, 0,0,0,0)
        put(data,HEAP,'B',3)
        put(data,body_b+0x10,'4f',0,0,0,0)
        put(data,body_b+0x20,'4f',0,0,0,0)
        put(data,body_b+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'3f',0,0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            machine.mem_write(frame+0x10,b'\0'*0xb0)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            projection=words(machine,frame+0x40,1)
            assert projection==(0,),('retail'if machine is retail else'compiled',precision,rounding,projection)
            assert snapshot[1]==1
            snapshots.append((snapshot,projection))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_linear_error_a_inertia_precedes_scalar_copy(solver_images,precision):
    frame=STACK+32768-0xd0
    # A inertia W overlaps the rounded SIMD input at frame+0x1c.
    # Retail reads inertia before writing that input. With the old zero W,
    # A angular W must preserve the retained B pointer at frame+0x0c.
    body_a=frame-0x20
    body_b=HEAP+0x80
    for rounding in range(4):
        data=solver_case(13,0)
        put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,body_b)
        put(data,OPS+24,'2I',13|(4<<8)|(3<<16),0x400)
        put(data,TERMS,'12f',0,0,0,1,0,0,0,1,0,0,0,0)
        put(data,HEAP,'B',3)
        put(data,body_b+0x10,'4f',0,0,0,0)
        put(data,body_b+0x20,'4f',0,0,0,0)
        put(data,body_b+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'f',0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            b_home=words(machine,frame+0xc,1)
            assert b_home==(body_b,),('retail'if machine is retail else'compiled',precision,rounding,b_home)
            assert words(machine,IMPULSES,1)==(0x3f800000,)
            assert snapshot[1]==1
            snapshots.append((snapshot,b_home))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_angular_error_a_inertia_precedes_scalar_copy(solver_images,precision):
    # A's inverse-inertia W aliases the rounded SIMD-input word.
    # Retail captures its old zero before that word becomes float one.
    case='a_inertia'
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    for rounding in range(4):
        data=solver_case(12,0)
        put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,body_b)
        put(data,OPS+24,'2I',12|(4<<8)|(4<<16),0x400)
        rows=(0,0,0,1,0,0,0,1)
        put(data,TERMS,'8f',*rows)
        put(data,HEAP,'B',3)
        for body in [body_b]:
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'f',0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in [(retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            label='retail'if machine is retail else'compiled'
            observed=words(machine,frame+0xc,1)
            assert observed==(body_b,),(label,case,precision,rounding,observed)
            assert snapshot[1]==1
            snapshots.append((snapshot,observed))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('case',['projection','receiver'])
def test_angular_error_published_receiver_capture_epochs(solver_images,precision,case):
    def as_float(v):return struct.unpack('<f',struct.pack('<I',v))[0]
    frame=STACK+32768-0xd0
    body_a=frame-0x10
    body_b=HEAP+0x80
    other_b=body_b+0x100
    delta=as_float(other_b+0x20)-as_float(body_b+0x20)
    for rounding in range(4):
        data=solver_case(12,0)
        put(data,STEP,'7fi',0,1,1 if case=='projection'else 0,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,body_b)
        put(data,OPS+24,'2I',12|(4<<8)|(4<<16),0x400)
        if case=='projection':rows=(0,1,0,0,0,0,0,0)
        else:rows=(0,delta/as_float(body_a),0,1,0,0,0,1)
        put(data,TERMS,'8f',*rows)
        put(data,HEAP,'B',3)
        for body in [body_b,other_b]:
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'f',0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in [(retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            label='retail'if machine is retail else'compiled'
            if case=='projection':
                observed=words(machine,frame+0x54,1)
                assert observed==(body_b+0x20,),(label,case,precision,rounding,observed)
            else:
                observed=words(machine,frame+0x14,1)
                assert observed==(other_b+0x20,),(label,case,precision,rounding,observed)
                original=words(machine,body_b+0x20,4)
                redirected=words(machine,other_b+0x20,4)
                assert original==(0,0,0,0x3f800000),(label,case,precision,rounding,original,redirected)
                assert redirected==(0,0,0,0)
            assert snapshot[1]==1
            snapshots.append((snapshot,observed))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('writer',['body','result'])
def test_angular_error_reloads_published_receiver_for_next_row(solver_images,precision,writer):
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    frame=STACK+32768-0xd0
    body_a=frame-0x10 if writer=='body'else HEAP
    body_b=HEAP+0x80
    other_b=body_b+0x100
    delta=as_float(other_b+0x20)-as_float(body_b+0x20)
    output=IMPULSES if writer=='body'else frame+0x14
    for rounding in range(4):
        data=solver_case(12,0)
        put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,body_b)
        put(data,OPS+24,'3I',0x4040c,0x4040c,0x400)
        rows=(0,delta/as_float(body_a),0,1,0,0,0,1) if writer=='body'else (0,0,0,1,0,0,0,delta)
        put(data,TERMS,'16f',*rows,0,0,0,1,0,0,0,1)
        put(data,HEAP,'B',3)
        for body in [HEAP,body_b,other_b]:
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'2f',0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in [(retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            label='retail'if machine is retail else'compiled'
            actual_home=words(machine,frame+0x14,1)
            original=words(machine,body_b+0x20,4)
            redirected=words(machine,other_b+0x20,4)
            assert actual_home==(other_b+0x20,),(label,writer,precision,rounding,actual_home)
            assert original==(0,0,0,0x3f800000 if writer=='body'else 0),(label,writer,precision,rounding,original,redirected)
            assert redirected==(0,0,0,0x3f800000),(label,writer,precision,rounding,original,redirected)
            assert snapshot[1]==1
            snapshots.append((snapshot,actual_home,original,redirected))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_circular_pair_retains_linear_and_angular_receivers(solver_images,precision):
    from unicorn.x86_const import UC_X86_REG_ESP
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    frame=STACK+32768-0xd0
    body_a=frame-0x10
    original_b=HEAP+0x80
    replacement_b=original_b+0x100
    final_b=original_b+0x200
    delta=as_float(replacement_b)-as_float(original_b)
    for rounding in range(4):
        data=solver_case(7,0)
        put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
        put(data,OPS+12,'2I',body_a,original_b)
        put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
        put(data,OPS+36,'IIffII',7|(20<<8)|(5<<16),FRICTION_RESULTS,0,10,8,0x400)
        put(data,TERMS,'24f',0,0,0,delta,0,0,0,0,0,0,0,1/delta,
                           0,0,0,delta,0,0,0,0,0,0,0,1/delta)
        put(data,HEAP,'B',3)
        for body in (original_b,replacement_b,final_b):
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        put(data,IMPULSES,'2f',0,0)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in ((retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))):
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                callbacks.append((terms,payload))
                uc.mem_write(frame+0x2c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            actual_pointer=words(machine,frame+0xc,1)[0]
            assert callbacks==[(TERMS,OPS+32)]
            assert actual_pointer==final_b,('retail'if machine is retail else'compiled',precision,rounding,hex(actual_pointer),hex(final_b),words(machine,IMPULSES,2),words(machine,frame+0x10,8),words(machine,original_b+0x10,8))
            original_linear=words(machine,original_b+0x10,4)
            replacement_linear=words(machine,replacement_b+0x10,4)
            assert original_linear==(0,0,0,as_bits(-2*delta)),('retail'if machine is retail else'compiled',precision,rounding,original_linear,replacement_linear)
            assert replacement_linear==(0,0,0,0)
            assert words(machine,IMPULSES,2)==(0x3f800000,0x3f800000)
            assert snapshot[1]==1
            snapshots.append((snapshot,original_linear,replacement_linear,actual_pointer,callbacks))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_circular_next_row_publishes_after_accumulation_input(solver_images,precision):
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    frame=STACK+32768-0xd0
    output=frame+0x18
    redirected_row=TERMS+0x400
    for rounding in range(4):
        data=solver_case(7,0)
        put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
        put(data,OPS+24,'IIffII',7|(20<<8)|(5<<16),FRICTION_RESULTS,0,10,8,0x400)
        put(data,TERMS,'24f',0,0,0,as_float(redirected_row),0,0,0,0,0,0,0,0,
                           1,0,0,1,0,0,0,0,0,0,0,1)
        put(data,redirected_row,'12f',2,0,0,0,0,0,0,0,0,0,0,0)
        put(data,HEAP,'B',3)
        for body in [HEAP,HEAP+0x80]:
            put(data,body+0x10,'4f',0,0,0,0)
            put(data,body+0x20,'4f',0,0,0,0)
            put(data,body+0x30,'4f',1,1,1,1)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in [(retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            label='retail'if machine is retail else'compiled'
            row_pointer=words(machine,output,1)
            linear_a=words(machine,HEAP+0x10,4)
            linear_b=words(machine,HEAP+0x90,4)
            assert row_pointer==(redirected_row,),(label,precision,rounding,row_pointer)
            assert linear_a==(0x40000000,0,0,0),(label,precision,rounding,linear_a,linear_b)
            assert linear_b==(0xc0000000,0,0,0),(label,precision,rounding,linear_a,linear_b)
            assert snapshot[1]==1
            snapshots.append((snapshot,row_pointer,linear_a,linear_b))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_header_captures_terms_before_publishing_a_home(solver_images,precision):
    from unicorn.x86_const import UC_X86_REG_ESP
    from test_solver_dispatch import RESULTS
    def as_float(v):return struct.unpack('<f',struct.pack('<I',v))[0]
    frame=STACK+32768-0xd0
    header=frame+0x20
    for rounding in range(4):
        data=solver_case(12,0)
        put(data,HEAP,'B',3)
        put(data,STEP,'7fi',0,as_float(HEAP+0x80),as_float(RESULTS),0,1,0,0,1)
        retail,compiled,symbols=solver_images
        snapshots=[]
        for machine,entry,gate in [(retail,0x31c940,0x480c60),
                (compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                callbacks.append((terms,payload))
                return_from_hook(uc)
            machine.mem_write(header,struct.pack('<6I',0x11801,TERMS,RESULTS,0,HEAP+0x80,8))
            machine.mem_write(header+24,struct.pack('<4I',0x20c0e,CALLBACK,0x12345678,0x400))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,header,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            label='retail'if machine is retail else'compiled'
            assert callbacks==[(TERMS,header+32)],(label,precision,rounding,callbacks)
            assert snapshot==(bytes(data),1)
            snapshots.append((snapshot,callbacks))
        assert snapshots[0]==snapshots[1]


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_bounded_angular_a_inertia_precedes_scalar_copy(solver_images,precision):
    # A inertia spans the inverse-pointer and rounded-input words. Its W lane
    # must retain the value read before the rounded impulse is published.
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(4,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'4I',4|(16<<8)|(4<<16),0x40000000,0xc0000000,0)
            put(data,OPS+40,'I',0x400)
            put(data,TERMS,'8f',0,0,0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'f',0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            b_home=words(machine,frame+0xc,1)
            assert b_home==(body_b,),(label,precision,rounding,b_home)
            assert words(machine,IMPULSES,1)==(0x3f800000,),(label,precision,rounding,words(machine,IMPULSES,1))
            assert words(machine,body_b+0x20,4)==(0,0,0,0x3f800000)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,b_home)
            else:assert snapshots[rounding]==(snapshot,b_home)


@pytest.mark.parametrize('kind',[2,6,9,11])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_linear_and_friction_a_inertia_precedes_scalar_copy(solver_images,precision,kind):
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(kind,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            size,classification={2:(4,3),6:(8,3),9:(8,4),11:(12,3)}[kind]
            put(data,OPS+24,'I',kind|(size<<8)|(classification<<16))
            if kind in [6,9]:put(data,OPS+28,'f',2)
            if kind==11:put(data,OPS+28,'2f',0,1)
            put(data,OPS+24+size,'I',0x400)
            if kind==9:put(data,TERMS,'8f',0,0,0,1,0,0,0,1)
            else:put(data,TERMS,'12f',0,0,0,1,0,0,1 if kind==2 else 0,1,0,0,0,0)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'f',0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            b_home=words(machine,frame+0xc,1)
            assert b_home==(body_b,),(label,kind,precision,rounding,b_home)
            assert words(machine,IMPULSES,1)==(0x3f800000,),(label,kind,precision,rounding,words(machine,IMPULSES,1))
            assert words(machine,body_b+0x20,4)==(0,0,0,0x3f800000 if kind==9 else 0)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,b_home)
            else:assert snapshots[rounding]==(snapshot,b_home)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_linear_friction_retains_angular_receiver(solver_images,precision):
    frame=STACK+32768-0xd0
    body_a=frame-0x10
    body_b=HEAP+0x80
    other_b=body_b+0x100
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    delta=as_float(other_b+0x20)-as_float(body_b+0x20)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(6,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'3I',6|(8<<8)|(3<<16),0x40000000,0x400)
            put(data,TERMS,'12f',0,0,0,1,0,delta/as_float(body_a),0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,other_b+0x20,'4f',0,0,0,0)
            put(data,IMPULSES,'f',0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            b_home=words(machine,frame+0xc,1)
            assert b_home==(body_b,),(label,precision,rounding,b_home)
            assert words(machine,IMPULSES,1)==(0x3f800000,),(label,precision,rounding,words(machine,IMPULSES,1))
            assert words(machine,body_b+0x20,4)==(0,0,0,0x3f800000),(label,precision,rounding,words(machine,body_b+0x20,4),words(machine,other_b+0x20,4))
            assert words(machine,other_b+0x20,4)==(0,0,0,0)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,b_home)
            else:assert snapshots[rounding]==(snapshot,b_home)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_first_normal_positive_a_inertia_precedes_copy(solver_images,precision):
    frame=STACK+32768-0xd0
    body_a=frame+0x10
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'3I',3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',0,0,0,1,0,0,1,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame+0x30,b'\0'*48)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,frame+0x38,1)
            assert observed==(0,),(label,precision,rounding,observed)
            assert words(machine,IMPULSES,2)==(0x3f800000,0x3f800000)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('selected',['first','second'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_first_normal_fallback_a_inertia_precedes_copy(solver_images,precision,selected):
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'3I',3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',0,0,0,1 if selected=='first'else -1,0,0,1 if selected=='first'else 0,1,0,0,0,1,0,0,0,1 if selected=='second'else -1,0,0,1 if selected=='second'else 0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,frame+0xc,1)
            assert observed==(body_b,),(label,precision,rounding,observed)
            assert words(machine,IMPULSES,2)==((0x3f800000,0)if selected=='first'else(0,0x3f800000))
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('selected',['first','second'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_fallback_a_inertia_precedes_copy(solver_images,precision,selected):
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,1 if selected=='first'else 0,0,0,1 if selected=='first'else 0,1,0,0,0,1,0,0,0,1 if selected=='second'else 0,0,0,1 if selected=='second'else 0,1,0,0,0,0)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,frame+8,1)
            assert observed==(0x12345678,),(label,precision,rounding,observed)
            assert words(machine,IMPULSES,4)==((0,0,0x3f800000,0)if selected=='first'else(0,0,0,0x3f800000))
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('writer',['input18','spin14','inverse10'])
def test_spin_first_inertia_capture_precedes_publications(solver_images,precision,writer):
    frame=STACK+32768-0xd0
    body_a=frame-0x20
    body_b=HEAP+0x80
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',body_a,body_b)
            put(data,OPS+24,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,2,8,1e38 if writer=='spin14'else 1,0x400)
            target=1e-38 if writer=='inverse10'else 1
            mass=1e38 if writer=='inverse10'else 1
            axis=[0,0,0,0];axis[{'inverse10':0,'spin14':1,'input18':2}[writer]]=1
            spin_target=1e-38 if writer=='spin14'else 0
            spin_mass=1 if writer=='spin14'else 0
            put(data,TERMS,'32f',0,0,0,target,*axis,0,0,0,mass,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,spin_mass,0,0,0,spin_target)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,body_b+offset,'4f',0,0,0,0)
            put(data,body_b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'3f',0,0,0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*16)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            assert snapshot[1]==1
            assert words(machine,IMPULSES,1)!=(0,),(label,writer,precision,rounding)
            if label=='retail':snapshots[rounding]=snapshot
            else:assert snapshots[rounding]==snapshot,(writer,precision,rounding)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_first_retains_linear_axis_before_publications(solver_images,precision):
    frame=STACK+32768-0xd0
    rows=frame+0x10
    a,b=HEAP+0x100,HEAP+0x200
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS,'6I',0x11801,rows,HEAP+0x4000,a,b,8)
            put(data,OPS+24,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,2,8,1,0x400)
            put(data,HEAP,'B',3)
            for body in [a,b]:
                for offset in [0x10,0x20]:put(data,body+offset,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000
            put(data,output,'3f',0,0,0)
            machine.mem_write(rows,struct.pack('<32f',.125,.25,.5,1,0,0,0,0,0,0,0,1,*([0.0]*20)))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            linear_a=words(machine,a+0x10,4);linear_b=words(machine,b+0x10,4)
            assert linear_a==(0x3f800000,0,0,0x3f800000),(label,precision,rounding,linear_a,linear_b)
            assert linear_b==(0xbf800000,0,0,0xbf800000),(label,precision,rounding,linear_a,linear_b)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,linear_a,linear_b)
            else:assert snapshots[rounding]==(snapshot,linear_a,linear_b)

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_second_row_follows_published_result_alias(solver_images,precision):
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    frame=STACK+32768-0xd0
    output=frame+0x18
    redirected=TERMS+0x400
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+24,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,2,8,1,0x400)
            put(data,TERMS,'32f',0,0,0,as_float(redirected)/2,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1,*([0.0]*8))
            put(data,redirected,'12f',1,0,0,0,*([0.0]*8))
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                for offset in [0x10,0x20]:put(data,body+offset,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            linear_a=words(machine,HEAP+0x10,4);linear_b=words(machine,HEAP+0x90,4)
            assert words(machine,output,1)==(redirected,),(label,precision,rounding,words(machine,output,3))
            assert linear_a[0]==0x3f800000 and linear_b[0]==0xbf800000,(label,precision,rounding,linear_a,linear_b)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,linear_a,linear_b)
            else:assert snapshots[rounding]==(snapshot,linear_a,linear_b)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_rows_retain_each_linear_receiver(solver_images,precision):
    from unicorn.x86_const import UC_X86_REG_ESP
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    frame=STACK+32768-0xd0
    a=frame+0x10
    original=HEAP+0x80;replacement=original+0x100;final=original+0x200
    delta=as_float(replacement+0x10)-as_float(original+0x10)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
            put(data,OPS+36,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,10,8,1,0x400)
            put(data,TERMS,'32f',delta,0,0,delta,0,0,0,0,0,0,0,1/delta,delta,0,0,delta,0,0,0,0,0,0,0,1/delta,*([0.0]*8))
            put(data,HEAP,'B',3)
            for b in [original,replacement,final]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000;put(data,output,'3f',0,0,0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                uc.mem_write(frame+0x2c,struct.pack('<f',1))
                uc.mem_write(frame+0x4c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*80)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            expected=(as_bits(-delta),0,0,as_bits(-delta))
            original_value=words(machine,original+0x10,4);replacement_value=words(machine,replacement+0x10,4)
            assert original_value==expected,(label,precision,rounding,original_value,replacement_value)
            assert replacement_value==expected,(label,precision,rounding,original_value,replacement_value)
            assert words(machine,final+0x10,4)==(0,0,0,0)
            assert callbacks==[(TERMS,OPS+32)]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_final_simd_bits_precede_second_result_store(solver_images,precision):
    frame=STACK+32768-0xd0;output=frame+0x10
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+24,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,2,8,1,0x400)
            put(data,TERMS,'32f',*([0.0]*12),0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for b in [HEAP,HEAP+0x80]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            machine.mem_write(frame+0x10,b'\0'*32);machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            angular=words(machine,HEAP+0x20,4)
            assert angular[0]==0x3f800000,(label,precision,rounding,angular,words(machine,output,3))
            assert words(machine,frame+0x14,1)==(0x40000000,)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,angular)
            else:assert snapshots[rounding]==(snapshot,angular)

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_spin_final_inertia_precedes_simd_copy(solver_images,precision):
    frame=STACK+32768-0xd0;a=frame+0x60;b=HEAP+0x80
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(8,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,b)
            put(data,OPS+24,'IIffIfI',8|(24<<8)|(6<<16),FRICTION_RESULTS,0,2,8,1,0x400)
            put(data,TERMS,'32f',*([0.0]*24),0,0,1,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
            put(data,b+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000;put(data,output,'3f',0,0,0)
            machine.mem_write(frame+0x70,b'\0'*48);machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            observed=words(machine,frame+0x88,1)
            assert observed==(0,),(label,precision,rounding,observed)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('row',[0,1])
def test_second_normal_positive_inertia_precedes_copy(solver_images,precision,row):
    frame=STACK+32768-0xd0
    a=frame+(0x80 if row==0 else 0x30)
    b=HEAP+0x80
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,b)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,1,1 if row==0 else 0,0,0,0,0,0,0,1,0,0,0,1,1 if row==1 else 0,0,0,0,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
            put(data,b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(a+0x10,b'\0'*48)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,a+0x20,1)
            assert observed==(0,),(label,row,precision,rounding,observed)
            assert words(machine,IMPULSES,4)==(0,0,0x3f800000,0x3f800000)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('writer',['second_impulse10','simd_copya0'])
def test_first_normal_second_simd_capture_precedes_first_result(solver_images,precision,writer):
    frame=STACK+32768-0xd0
    output=frame+(0x10 if writer=='second_impulse10'else 0xa0)
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+24,'3I',3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',0,0,0,1 if writer=='second_impulse10'else 2,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,1)
            put(data,HEAP,'B',3)
            for b in [HEAP,HEAP+0x80]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(frame+0xa0,b'\0'*8)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            observed=words(machine,output,2)
            expected=(0x40000000,0x40400000 if writer=='second_impulse10'else 0x3f800000)
            assert observed==expected,(label,writer,precision,rounding,observed,expected)
            assert words(machine,HEAP+0x10,1)==(0x3f800000,),(label,writer,precision,rounding,words(machine,HEAP+0x10,4))
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_second_normal_simd_capture_precedes_first_result(solver_images, precision):
    """The second SIMD input predates first FST; its numeric input follows FST."""
    frame = STACK + 32768 - 0xd0
    retail, compiled, symbols = solver_images
    snapshots = {}
    for label, machine, entry, gate in [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'), address(symbols, 'g_480c60')),
    ]:
        for rounding in range(4):
            data = solver_case(3, 0)
            put(data, STEP, '7fi', 0, 1, 0, 1, 1, 0, 0, 1)
            put(data, OPS + 24, '5I', 3 | (8 << 8) | (5 << 16), 0,
                3 | (8 << 8) | (5 << 16), 0, 0x400)
            put(data, TERMS, '24f', *([0.0] * 24))
            put(data, TERMS + 96, '24f',
                0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1)
            put(data, HEAP, 'B', 3)
            for body in [HEAP, HEAP + 0x80]:
                for offset in [0x10, 0x20]:
                    put(data, body + offset, '4f', 0, 0, 0, 0)
                put(data, body + 0x30, '4f', 1, 1, 1, 1)
            machine.mem_write(frame + 0x10, b'\0' * 32)
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, frame + 0x10],
                               precision=precision | (rounding << 10))
            assert words(machine, frame + 0x60, 1) == (0x3f800000,)
            assert words(machine, HEAP + 0x10, 4)[0] == 0x3f800000
            assert snapshot[1] == 1
            if label == 'retail':
                snapshots[rounding] = snapshot
            else:
                assert snapshots[rounding] == snapshot


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_fallback_captures_receiver_before_a_linear(solver_images,precision):
    """Retain B before the A linear store can change the shared pointer word."""
    from unicorn.x86_const import UC_X86_REG_ESP
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    frame=STACK+32768-0xd0
    a=frame
    original=HEAP+0x80;replacement=original+0x100;final=original+0x200
    delta=as_float(replacement+0x10)-as_float(original+0x10)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
            put(data,OPS+36,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,delta,0,0,0,1/delta,0,0,0,1/delta,0,0,0,0,0,0,0,0,0,0,0,0)
            put(data,HEAP,'B',3)
            for b in [original,replacement,final]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000;put(data,output,'4f',0,0,0,0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                uc.mem_write(frame+0x3c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*80)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            expected=(0,0,0,as_bits(-delta))
            original_value=words(machine,original+0x10,4);replacement_value=words(machine,replacement+0x10,4)
            assert original_value==expected,(label,precision,rounding,original_value,replacement_value)
            assert replacement_value==(0,0,0,0),(label,precision,rounding,original_value,replacement_value)
            assert words(machine,final+0x10,4)==(0,0,0,0)
            assert callbacks==[(TERMS,OPS+32)]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'receivers four modes passed',hex(precision),flush=True)



@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('scale_alias',['error','velocity'])
@pytest.mark.parametrize('repeat',[1,2])
def test_linear_error_scale_is_read_after_projection_publication(solver_images,precision,scale_alias,repeat):
    """Both scales observe projection5c, including on consecutive rows."""
    frame=STACK+32768-0xd0
    scale_step=frame+(0x58 if scale_alias=="error"else 0x54)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(13,0)
            put(data,OPS+24,str(repeat+1)+'I',*([13|(4<<8)|(3<<16)]*repeat+[0x400]))
            for row_index in range(repeat):
                put(data,TERMS+row_index*48,'12f',1,0,0,2,0,0,0,1,0,0,0,0)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',1 if body==HEAP else 0,0,0,0)
                put(data,body+0x20,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame+0x10,b'\0'*160)
            machine.mem_write(scale_step,struct.pack('<7fi',0,2,1 if scale_alias=='error'else 2,0,1,0,0,1))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[scale_step,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,HEAP+0x10,1)
            expected={'error':{1:2.0,2:5.0},'velocity':{1:4.0,2:-41.0}}[scale_alias][repeat]
            assert observed==struct.unpack('<I',struct.pack('<f',expected)),(label,precision,rounding,scale_alias,repeat,observed)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=snapshot
            else:assert snapshots[rounding]==snapshot
        print(label,'C13 scale after projection four modes passed',hex(precision),flush=True)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_second_fallback_captures_receiver_before_a_linear(solver_images,precision):
    """The second-row fallback also retains B before A replaces the pointer home."""
    from unicorn.x86_const import UC_X86_REG_ESP
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    frame=STACK+32768-0xd0
    a=frame
    original=HEAP+0x80;replacement=original+0x100;final=original+0x200
    delta=as_float(replacement+0x10)-as_float(original+0x10)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
            put(data,OPS+36,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,delta,0,0,0,1/delta,0,0,0,1/delta)
            put(data,HEAP,'B',3)
            for b in [original,replacement,final]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000;put(data,output,'4f',0,0,0,0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                uc.mem_write(frame+0x3c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*80)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            expected=(0,0,0,as_bits(-delta))
            original_value=words(machine,original+0x10,4);replacement_value=words(machine,replacement+0x10,4)
            assert original_value==expected,(label,precision,rounding,original_value,replacement_value)
            assert replacement_value==(0,0,0,0),(label,precision,rounding,original_value,replacement_value)
            assert words(machine,final+0x10,4)==(0,0,0,0)
            assert callbacks==[(TERMS,OPS+32)]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'receivers four modes passed',hex(precision),flush=True)



@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('kind',[6,9])
def test_friction_limit_publication_follows_captured_inputs(solver_images,precision,kind):
    """Publishing a limit preserves the target/projection inputs captured before it."""
    from unicorn.x86_const import UC_X86_REG_ESP
    from test_solver_dispatch import OP_LAYOUT
    frame=STACK+32768-0xd0
    terms=frame+(0x30 if kind==6 else 0x20)
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(kind,0)
            put(data,STEP,'7fi',0,2,1,2,1,0,0,1)
            put(data,OPS+4,'I',terms)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
            put(data,OPS+36,'IfI',kind|(8<<8)|(OP_LAYOUT[kind][1]<<16),100,0x400)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',0,0,1 if kind==6 and body==HEAP else 0,0)
                put(data,body+0x20,'4f',0,0,1 if kind==9 and body==HEAP else 0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'f',0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,arg,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((arg,payload))
                if kind==6:
                    uc.mem_write(frame+0x38,struct.pack('<f',1));uc.mem_write(frame+0x4c,struct.pack('<f',1))
                else:uc.mem_write(frame+0x2c,struct.pack('<f',1))
                uc.mem_write(frame+0x3c,struct.pack('<f',2))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*176)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            observed=words(machine,HEAP+(0x18 if kind==6 else 0x28),1)
            expected=301.0 if kind==6 else 4.0
            assert observed==struct.unpack('<I',struct.pack('<f',expected)),(label,kind,precision,rounding,observed)
            assert callbacks==[(terms,OPS+32)]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'limit publication4RM',kind,hex(precision),flush=True)



@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('writer',['row20','second_impulse18'])
def test_second_normal_row_and_simd_capture_precede_first_result(solver_images,precision,writer):
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    frame=STACK+32768-0xd0
    output=frame+(0x18 if writer=='row20'else 0x10)
    redirected=TERMS+0x400
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            first_target=as_float(redirected)/2 if writer=='row20'else 1
            put(data,TERMS+96,'24f',0,0,0,first_target,0,0,0,0,0,0,0,1,1 if writer=='second_impulse18'else 0,0,0,1,0,0,0,0,0,0,0,1)
            put(data,redirected,'12f',1,0,0,0,*([0.0]*8))
            put(data,HEAP,'B',3)
            for b in [HEAP,HEAP+0x80]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            machine.mem_write(frame+0x10,b'\0'*32)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],precision=precision|(rounding<<10))
            if writer=='row20':assert words(machine,frame+0x20,1)==(redirected,),(label,writer,precision,rounding,words(machine,frame+0x20,1))
            else:assert words(machine,frame+0x60,1)==(0x3f800000,),(label,writer,precision,rounding,words(machine,frame+0x60,1))
            linear=words(machine,HEAP+0x10,4)
            assert linear[0]==0x3f800000,(label,writer,precision,rounding,linear)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,linear)
            else:assert snapshots[rounding]==(snapshot,linear)
        print(label,'second normal epochs four modes passed',writer,hex(precision),flush=True)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_retains_linear_receiver_through_both_rows(solver_images,precision):
    def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    frame=STACK+32768-0xd0
    a=frame
    original=HEAP+0x80;replacement=original+0x100;final=original+0x200
    delta=as_float(replacement+0x10)-as_float(original+0x10)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,0)
            put(data,OPS+36,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,delta,0,0,0,0,0,0,0,1/delta,0,0,0,delta,0,0,0,0,0,0,0,1/delta)
            put(data,HEAP,'B',3)
            for b in [original,replacement,final]:
                for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
                put(data,b+0x30,'4f',1,1,1,1)
            output=HEAP+0x5000;put(data,output,'4f',0,0,0,0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                uc.mem_write(frame+0x3c,struct.pack('<f',1))
                return_from_hook(uc)
            machine.mem_write(frame+0x10,b'\0'*80)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,output],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            expected=(0,0,0,as_bits(-2*delta))
            original_value=words(machine,original+0x10,4);replacement_value=words(machine,replacement+0x10,4)
            assert original_value==expected,(label,precision,rounding,original_value,replacement_value)
            assert replacement_value==(0,0,0,0),(label,precision,rounding,original_value,replacement_value)
            assert words(machine,final+0x10,4)==(0,0,0,0)
            assert callbacks==[(TERMS,OPS+32)]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'receivers four modes passed',hex(precision),flush=True)



@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_inverse_publication_follows_a_angular(solver_images,precision):
    frame=STACK+32768-0xd0
    a=frame-0x10
    b=HEAP+0x80
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,b)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1)
            put(data,HEAP,'B',3)
            for offset in [0x10,0x20]:put(data,b+offset,'4f',0,0,0,0)
            put(data,b+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(a+0x10,b'\0'*48)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,a+0x20,1)
            assert observed==(b+0x30,),(label,precision,rounding,observed)
            assert words(machine,IMPULSES,4)==(0,0,0x3f800000,0x3f800000)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)
        print(label,'inverse publication four modes passed',hex(precision),flush=True)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_angular_home_publication_follows_first_projection(solver_images,precision):
    frame=STACK+32768-0xd0;a=frame-0x10
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,0,0,0,1,0,0,1)
            put(data,OPS+12,'2I',a,HEAP+0x80)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'48f',*([0.0]*48))
            put(data,TERMS+96+16,'4f',0,1,0,0)
            for offset in [0x10,0x20]:put(data,HEAP+0x80+offset,'4f',0,0,0,0)
            put(data,HEAP+0x80+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame-0x10,b'\0'*208)
            machine.mem_write(a,b'\x03')
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,frame+0x88,1)
            assert observed==(0,),(label,precision,rounding,observed)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=snapshot
            else:assert snapshots[rounding]==snapshot
        print(label,'second normal angular home4RM',hex(precision),flush=True)


@pytest.mark.parametrize('initial_count',[-1,0,1,3])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_initial_iteration_count_precedes_counter_publication(solver_images,precision,initial_count):
    frame=STACK+32768-0xd0;step=frame+0x10
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,OPS+24,'4I',14|(12<<8)|(2<<16),CALLBACK,0,0x400)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                return_from_hook(uc)
            machine.mem_write(frame,b'\0'*208)
            machine.mem_write(step,struct.pack('<7fi',0,0,0,0,1,0,0,initial_count))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[step,OPS,HEAP,HEAP+0x4000],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            assert callbacks==([(TERMS,OPS+32)]if initial_count>0 else[]),(label,precision,rounding,callbacks)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'initial counter capture4RM',hex(precision),flush=True)


@pytest.mark.parametrize('pair_index',[0,1])
@pytest.mark.parametrize('coefficient',['error','velocity'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_coupled_scales_reload_after_first_error(solver_images,precision,pair_index,coefficient):
    frame=STACK+32768-0xd0;slot=0x14 if pair_index==0 else 0x10
    step=frame+slot-{'error':4,'velocity':8}[coefficient]
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0);n=pair_index+1
            for i in range(n):
                put(data,OPS+24+8*i,'If',3|(8<<8)|(5<<16),0)
                put(data,TERMS+96*i,'24f',1,0,0,3,0,0,0,1,0,0,0,1,0,1,0,4,0,0,0,1,0,0,0,1)
            callback_op=OPS+24+8*n
            put(data,callback_op,'4I',14|(12<<8)|(2<<16),CALLBACK,0,0x400)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',1 if body==HEAP else 0,1 if body==HEAP else 0,0,0)
                put(data,body+0x20,'4f',0,0,0,0);put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3);callbacks.append((terms,payload))
                # The synthetic boundary only caps iteration count: its word
                # can overlap the retail counter, saved scale, or A home.
                uc.mem_write(step+28,struct.pack('<i',1));return_from_hook(uc)
            machine.mem_write(frame,b'\0'*208);machine.mem_write(step,struct.pack('<7fi',0,2,1,2,1,0,0,1));machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[step,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            assert callbacks==[(TERMS+96*n,callback_op+8)],(label,precision,rounding,pair_index,coefficient,callbacks)
            observed=words(machine,HEAP+0x10,4);assert observed[:2]!=(0x3f800000,0x3f800000),(label,'inactive',observed)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks,observed)
            else:assert snapshots[rounding]==(snapshot,callbacks,observed),(precision,rounding,pair_index,coefficient,observed)
        print(label,'coupled first-error scale4RM',pair_index,coefficient,hex(precision),flush=True)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_second_fallback_publishes_row_before_linear_update(solver_images,precision):
    frame=STACK+32768-0xd0;a=frame+0x10;expected_row=TERMS+144
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,1,1,1,0,0,1)
            put(data,OPS+12,'2I',a,HEAP+0x80)
            put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            put(data,TERMS+96,'24f',0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,3,0,0,0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',0,0,0,0);put(data,body+0x20,'4f',0,0,0,0);put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame,b'\0'*208);machine.mem_write(a+0x30,struct.pack('<4f',1,1,1,1));machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,a+0x10,4)
            assert observed[0]==expected_row,(label,precision,rounding,observed,hex(expected_row))
            assert words(machine,HEAP+0x90,4)[2]==0xc0000000
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed)
            else:assert snapshots[rounding]==(snapshot,observed)
        print(label,'second fallback row20 publication4RM',hex(precision),flush=True)


@pytest.mark.parametrize('inertia_owner',['a','b'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_second_fallback_inertia_straddles_row_publication(solver_images,precision,inertia_owner):
    frame=STACK+32768-0xd0;aliased=frame-0x10
    a=aliased if inertia_owner=='a'else HEAP;b=aliased if inertia_owner=='b'else HEAP+0x80
    row=TERMS+144;expected_b=struct.unpack('<I',struct.pack('<f',3*struct.unpack('<f',struct.pack('<I',row))[0]))[0]
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0);put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,b);put(data,OPS+24,'4I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0)
            put(data,OPS+40,'4I',14|(12<<8)|(2<<16),CALLBACK,0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            axis_a=(1 if inertia_owner=='a'else 0,0,0,1);axis_b=(1 if inertia_owner=='b'else 0,0,0,1)
            put(data,TERMS+96,'24f',0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,3,*axis_a,*axis_b)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',0,0,0,0);put(data,body+0x20,'4f',0,0,0,0);put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0);callbacks=[]
            def callback(uc,va,size,user):
                ret,terms,payload=words(uc,uc.reg_read(UC_X86_REG_ESP),3)
                observed=words(uc,aliased+0x20,1)[0];published=words(uc,frame+0x20,1)[0]
                callbacks.append((terms,payload,observed,published));return_from_hook(uc)
            machine.mem_write(frame-0x10,b'\0'*224);machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            assert len(callbacks)==1,(label,precision,rounding,inertia_owner,callbacks)
            terms,payload,observed,published=callbacks[0]
            assert(terms,payload,published)==(TERMS+192,OPS+48,row),(label,precision,rounding,inertia_owner,callbacks)
            if inertia_owner=='a':assert observed&0x7fffffff==0,(label,precision,rounding,inertia_owner,callbacks)
            else:assert observed==expected_b,(label,precision,rounding,inertia_owner,callbacks,hex(expected_b))
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,callbacks)
            else:assert snapshots[rounding]==(snapshot,callbacks)
        print(label,'second fallback inertia/publication4RM',inertia_owner,hex(precision),flush=True)


@pytest.mark.parametrize('writer',['linear','angular'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_first_fallback_retains_angular_receiver(solver_images,precision,writer):
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    def as_float(value):return struct.unpack('<f',struct.pack('<I',value))[0]
    frame=STACK+32768-0xd0;a=frame if writer=='linear'else frame-0x10
    original=HEAP+0x80
    replacement=0x40000000 if writer=='linear'else as_bits(as_float(original+0x20)+as_float(as_bits(2*as_float(a))))
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        page=replacement&~4095
        if not any(lo<=page and page+4095<=hi for lo,hi,_ in machine.mem_regions()):machine.mem_map(page,8192)
        for rounding in range(4):
            data=solver_case(3,0);put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original);put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            linear_axis=(0,1 if writer=='linear'else 0,0,2)
            angular_axis=(1 if writer=='linear'else 0,1 if writer=='angular'else 0,0,1)
            put(data,TERMS+96,'24f',*linear_axis,*angular_axis,1,0,0,1,0,0,0,-1,0,0,0,1,0,0,0,1)
            put(data,HEAP,'B',3)
            for body in [HEAP,original]:
                put(data,body+0x10,'4f',0,0,0,0);put(data,body+0x20,'4f',0,0,0,0);put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame-0x10,b'\0'*224)
            if writer=='linear':machine.mem_write(a+0x30,struct.pack('<4f',1,1,1,1))
            machine.mem_write(replacement,b'\0'*16);machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,original+0x20,4);unexpected=words(machine,replacement,4)
            assert observed[0]==0x40000000,(label,precision,rounding,writer,observed,unexpected,hex(replacement))
            assert unexpected==(0,0,0,0),(label,precision,rounding,writer,observed,unexpected)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed,unexpected)
            else:assert snapshots[rounding]==(snapshot,observed,unexpected)
        print(label,'first fallback angular receiver4RM',writer,hex(precision),flush=True)


@pytest.mark.parametrize('writer',['linear','angular'])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_second_normal_second_fallback_captures_receiver_between_writes(solver_images,precision,writer):
    def as_bits(value):return struct.unpack('<I',struct.pack('<f',value))[0]
    def as_float(value):return struct.unpack('<f',struct.pack('<I',value))[0]
    frame=STACK+32768-0xd0;a=frame if writer=='linear'else frame-0x10
    original=HEAP+0x80
    replacement=0x40000000 if writer=='linear'else as_bits(as_float(original+0x20)+as_float(as_bits(2*as_float(a))))
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        page=replacement&~4095
        if not any(lo<=page and page+4095<=hi for lo,hi,_ in machine.mem_regions()):machine.mem_map(page,8192)
        for rounding in range(4):
            data=solver_case(3,0);put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS+12,'2I',a,original);put(data,OPS+24,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0.0]*24))
            linear_axis=(0,1 if writer=='linear'else 0,0,2)
            angular_axis=(1 if writer=='linear'else 0,1 if writer=='angular'else 0,0,1)
            put(data,TERMS+96,'24f',0,0,0,0,0,0,0,1,0,0,0,1,*linear_axis,*angular_axis,1,0,0,1)
            put(data,HEAP,'B',3)
            for body in [HEAP,original]:
                put(data,body+0x10,'4f',0,0,0,0);put(data,body+0x20,'4f',0,0,0,0);put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame-0x10,b'\0'*224)
            if writer=='linear':machine.mem_write(a+0x30,struct.pack('<4f',1,1,1,1))
            machine.mem_write(replacement,b'\0'*16);machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            observed=words(machine,original+0x20,4);unexpected=words(machine,replacement,4)
            assert observed[0]==(0 if writer=='linear'else 0x40000000),(label,precision,rounding,writer,observed,unexpected,hex(replacement))
            assert unexpected==((0x40000000,0,0,0x40000000)if writer=='linear'else(0,0,0,0)),(label,precision,rounding,writer,observed,unexpected)
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed,unexpected)
            else:assert snapshots[rounding]==(snapshot,observed,unexpected)
        print(label,'second fallback middle receiver4RM',writer,hex(precision),flush=True)


@pytest.mark.parametrize('coefficient', ['error', 'velocity'])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_linear_error_reloads_step_between_rows(solver_images, precision, coefficient):
    def as_float(bits):
        return struct.unpack('<f', struct.pack('<I', bits))[0]

    replacement_step = STEP + 0x100
    delta = as_float(replacement_step) - as_float(STEP)
    stack_arguments = STACK + 32768 + 4
    retail, compiled, symbols = solver_images
    snapshots = {}
    for label, machine, entry, gate in (
            ('retail', retail, 0x31c940, 0x480c60),
            ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
             address(symbols, 'g_480c60'))):
        for rounding in range(4):
            data = solver_case(13, 2)
            put(data, STEP, '7fi', 0, 1, 1 if coefficient == 'velocity' else 0, 0, 1, 0, 0, 1)
            put(data, replacement_step, '7fi', 0, 2 if coefficient == 'error' else 1,
                2 if coefficient == 'velocity' else 0, 0, 1, 0, 0, 1)
            put(data, OPS + 24, '3I', 0x3040d, 0x3040d, 0x400)
            for body in (HEAP, HEAP + 0x80):
                for offset in (0x10, 0x20, 0x30, 0x40, 0x50):
                    put(data, body + offset, '4f', 0, 0, 0, 0)
            for index, target in enumerate((delta, 3 if coefficient == 'error' else 0)):
                put(data, TERMS + index * 48, '12f',
                    1 if index == 1 and coefficient == 'velocity' else 0, 0, 0, target,
                    0, 0, 0, 1, 0, 0, 0, 0)
            if coefficient == 'velocity':
                put(data, HEAP + 0x10, '4f', 3, 0, 0, 0)
            # First accumulation replaces the incoming step pointer. Second
            # accumulation replaces the operations argument with +/-6 in retail
            # or +/-3 with a stale step. Both continuations return normally.
            for empty_stream in (0x40400000, 0x40c00000, 0xc03ff000, 0xc0bff000):
                for page in (empty_stream, empty_stream + 4096):
                    if not any(lo <= page <= hi for lo, hi, _ in machine.mem_regions()):
                        machine.mem_map(page, 4096)
                machine.mem_write(empty_stream, b'\0' * 8192)
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data,
                               [STEP, OPS, HEAP, stack_arguments],
                               precision=precision | (rounding << 10))
            output = words(machine, stack_arguments, 4)
            # FADD includes the tiny positive float encoding of the old OPS
            # argument; directed rounding can retain its one-ulp contribution.
            expected = (0x40c00000 + (rounding == 2) if coefficient == 'error'
                        else 0xc0c00000 - (rounding in (2, 3)))
            assert output == (replacement_step, expected, HEAP, stack_arguments), (
                label, precision, rounding, coefficient, output)
            assert snapshot[1] == 1
            if label == 'retail':
                snapshots[rounding] = (snapshot, output)
            else:
                assert snapshots[rounding] == (snapshot, output), (precision, rounding, coefficient)
        print(label, 'linear-error step reload 4RM', coefficient, hex(precision), flush=True)


@pytest.mark.parametrize('scale_words', [
    (0x40000000, 0x40400000),
    (0x7fa12345, 0x7f800001),
    (0x80000000, 0xff800001),
], ids=['finite', 'nan_payloads', 'signed_zero_nan'])
@pytest.mark.parametrize('aliased', [False, True])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_iteration_scales_are_captured_before_saved_error_publication(
        solver_images, precision, aliased, scale_words):
    frame = STACK + 32768 - 0xd0
    step = frame + 0x28 if aliased else STEP
    error_word, velocity_word = scale_words
    retail, compiled, symbols = solver_images
    snapshots = {}
    for label, machine, entry, gate in (
            ('retail', retail, 0x31c940, 0x480c60),
            ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
             address(symbols, 'g_480c60'))):
        for rounding in range(4):
            data = solver_case(13, 0)
            put(data, STEP, '8I', 0, error_word, velocity_word, 0, 0x3f800000, 0, 0, 1)
            put(data, OPS + 24, '5I', 0x20410, 0x20c0e, CALLBACK, 123, 0x400)
            put(data, HEAP, 'B', 3)
            machine.mem_write(frame + 0x10, b'\0' * 176)
            if aliased:
                # The counter zeroes the input error. Saving that zero to +30
                # then overlaps the velocity input, which retail captured first.
                machine.mem_write(step, struct.pack(
                    '<8I', 0, error_word, velocity_word, 0, 0x3f800000, 0, 0, 1))
            calls = []

            def observer(uc, va, size, user):
                terms, payload = words(uc, uc.reg_read(UC_X86_REG_ESP) + 4, 2)
                calls.append((terms, payload, *words(uc, step + 4, 2)))
                return_from_hook(uc)

            # The real restore-scales opcode executes before the observer.
            # The callback only observes its arguments and restored input words.
            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [step, OPS, HEAP, IMPULSES],
                               hooks=[(CALLBACK, observer)],
                               precision=precision | (rounding << 10))
            assert calls == [(TERMS, OPS + 36,
                              0 if aliased else error_word, velocity_word)], (
                                  label, precision, rounding, aliased, calls)
            assert snapshot[1] == 1
            if label == 'retail':
                snapshots[rounding] = (snapshot, calls)
            else:
                assert snapshots[rounding] == (snapshot, calls), (precision, rounding, aliased)
        print(label, 'saved-scale capture 4RM', hex(precision), aliased, scale_words, flush=True)


@pytest.mark.parametrize('kind', [2, 4, 5, 6, 9, 10, 11, 12, 13])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('overwrite', ['error', 'impulse'])
def test_single_writeback_captures_stride_between_outputs(
        solver_images, kind, precision, overwrite):
    from test_solver_dispatch import OP_LAYOUT

    # Retail reads stride after publishing the impulse, before publishing the
    # residual error. Either output can overlap its physical stack home.
    frame = STACK + 32768 - 0xd0
    retail, compiled, symbols = solver_images
    for rounding in range(4):
        for new_stride in (16, 32):
            data = solver_case(kind, 2)
            put(data, STEP, '7fi', 0, 0, 0, 0, 1, 0, 0, 0)
            size, row_kind = OP_LAYOUT[kind]
            linear = kind in (2, 5, 6, 11, 13)
            row_size, coefficient = (48, 12) if linear else (32, 28)
            destination = frame + (0x20 if overwrite == 'error' else 0x24)
            put(data, OPS + 8, 'I', destination)
            put(data, OPS + 20, 'I', 8)
            for row in range(2):
                put(data, OPS + 24 + row * size, 'I',
                    kind | size << 8 | row_kind << 16)
            put(data, OPS + 24 + 2 * size, 'I', 0x400)
            put(data, TERMS + coefficient, 'I',
                new_stride if overwrite == 'error' else kind)
            put(data, TERMS + row_size + coefficient, 'f', 1)
            put(data, IMPULSES, '2I',
                0x11223344 if overwrite == 'error' else new_stride, kind)

            expected = bytearray(0x38)
            struct.pack_into('<I', expected, 4, new_stride)
            if overwrite == 'error':
                struct.pack_into('<I', expected, 0, 0x11223344)
                struct.pack_into('<I', expected, 12, 0x3f800000)
            else:
                struct.pack_into('<2I', expected, 4 + new_stride,
                                 kind, 0x3f800000)
            # The sentinel publishes kind zero after both output records.
            struct.pack_into('<I', expected, 8, 0)

            for label, machine, entry, gate in (
                    ('retail', retail, 0x31c940, 0x480c60),
                    ('compiled', compiled,
                     address(symbols, 'solve_operations_31c940'),
                     address(symbols, 'g_480c60'))):
                machine.mem_write(frame + 12, b'\0' * 176)
                machine.mem_write(gate, b'\x01')
                snapshot = execute(machine, entry, data,
                                   [STEP, OPS, HEAP, IMPULSES],
                                   precision=precision | (rounding << 10))
                context = label, kind, precision, rounding, overwrite, new_stride
                assert snapshot == (bytes(data), 1), context
                assert bytes(machine.mem_read(frame + 0x20, len(expected))) == expected, context


@pytest.mark.parametrize('kind', [2, 4, 5, 6, 9, 10, 11, 12, 13])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_single_writeback_captures_time_pointer_before_impulse(
        solver_images, kind, precision):
    from test_solver_dispatch import OP_LAYOUT

    # The first impulse replaces the inverse-time pointer's stack home.
    # This row still uses the old pointer; the next row reads the replacement.
    frame = STACK + 32768 - 0xd0
    retail, compiled, symbols = solver_images
    for rounding in range(4):
        for replacement_time in (2, 3):
            data = solver_case(kind, 2)
            put(data, STEP, '7fi', 0, 0, 0, 0, 1, 0, 0, 0)
            replacement = STEP + 0x100
            put(data, replacement, 'f', replacement_time)
            size, row_kind = OP_LAYOUT[kind]
            linear = kind in (2, 5, 6, 11, 13)
            row_size, coefficient = (48, 12) if linear else (32, 28)
            put(data, OPS + 8, 'I', frame + 0x10)
            put(data, OPS + 20, 'I', 8)
            for row in range(2):
                put(data, OPS + 24 + row * size, 'I',
                    kind | size << 8 | row_kind << 16)
            put(data, OPS + 24 + 2 * size, 'I', 0x400)
            put(data, TERMS + coefficient, 'f', 2)
            put(data, TERMS + row_size + coefficient, 'f', 4)
            put(data, IMPULSES, '2I', replacement, 0x3f800000)
            expected = struct.pack('<IfIf', replacement, 2,
                                   0x3f800000, 4 * replacement_time)
            for label, machine, entry, gate in (
                    ('retail', retail, 0x31c940, 0x480c60),
                    ('compiled', compiled,
                     address(symbols, 'solve_operations_31c940'),
                     address(symbols, 'g_480c60'))):
                machine.mem_write(frame + 12, b'\0' * 176)
                machine.mem_write(gate, b'\x01')
                snapshot = execute(machine, entry, data,
                                   [STEP, OPS, HEAP, IMPULSES],
                                   precision=precision | (rounding << 10))
                context = label, kind, precision, rounding, replacement_time
                assert snapshot == (bytes(data), 1), context
                assert bytes(machine.mem_read(frame + 0x10, len(expected))) == expected, context


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('overlap', [None, -3, -2, -1, 0])
def test_iteration_limit_precedes_increment_publication(solver_images, precision, overlap):
    frame = STACK + 32768 - 0xd0
    counter = frame + 0x2c
    step = STEP if overlap is None else counter + overlap - 28
    retail, compiled, symbols = solver_images
    for rounding in range(4):
        data = solver_case(3, 0)
        put(data, OPS + 24, '4I', 14 | (12 << 8) | (2 << 16), CALLBACK, 0, 0x400)
        put(data, HEAP, 'B', 3)
        if overlap is None:
            put(data, step, '7fi', 0, 0, 0, 0, 1, 0, 0, 1)
        snapshots = []
        for label, machine, entry, gate in (
                ('retail', retail, 0x31c940, 0x480c60),
                ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
                 address(symbols, 'g_480c60'))):
            machine.mem_write(frame, b'\0' * 208)
            if overlap is not None:
                machine.mem_write(step, struct.pack('<7fi', 0, 0, 0, 0, 1, 0, 0, 1))
            callbacks = []

            def callback(uc, va, size, user):
                # This is the actual operation callback boundary. The second
                # value only bounds a reconstruction which incorrectly repeats.
                callbacks.append(1)
                assert len(callbacks) <= 2
                limit = 1 if len(callbacks) == 1 else (0x7fffffff if overlap == -3 else -1)
                uc.mem_write(step + 28, struct.pack('<i', limit))
                return_from_hook(uc)

            machine.mem_write(gate, b'\x01')
            snapshot = execute(machine, entry, data, [step, OPS, HEAP, IMPULSES],
                               hooks=[(CALLBACK, callback)],
                               precision=precision | (rounding << 10))
            context = label, precision, rounding, overlap
            assert callbacks == [1], (context, callbacks)
            assert words(machine, counter, 1) == (2 if overlap == 0 else 1,), context
            assert snapshot[1] == 1, context
            snapshots.append((snapshot, tuple(callbacks)))
        assert snapshots[0] == snapshots[1], (precision, rounding, overlap)
