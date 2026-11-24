"""Differential coverage for aliased solver inputs and writeback storage."""
import struct

import pytest
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
