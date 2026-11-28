"""Retail proofs for the distinct normal-pair fallback linear receivers."""
import struct
import pytest
from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import UC_X86_REG_FPCW
from test_solver_dispatch import HEAP,STACK,STEP,OPS,TERMS,IMPULSES,RESULTS,CALLBACK,solver_images,solver_case,put,address,execute,words,return_from_hook

def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('aliased',[False,True])
@pytest.mark.parametrize('row_index',[0,1])
def test_first_pair_fallback_linear_source_and_destination(solver_images,precision,aliased,row_index):
    frame=STACK+32768-0xd0
    a=frame-0x10 if aliased else HEAP
    b,replacement=HEAP+0x80,HEAP+0x180
    delta=as_float(replacement)-as_float(b)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS,'6I',0x11801,TERMS,RESULTS,a,b,8)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,3)
            put(data,OPS+36,'3I',3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'24f',*([0,0,0,1,0,0,0,1,0,0,0,1, 0,0,0,0,0,0,0,1,0,0,0,1] if row_index==0 else [0,0,0,0,0,0,0,1,0,0,0,1, 0,0,0,1,0,0,0,1,0,0,0,1]))
            put(data,HEAP,'B',3)
            for body in [HEAP,b,replacement]:
                put(data,body+0x10,'4f',1 if body==b else 5 if body==replacement else 0,0,0,0)
                put(data,body+0x20,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,delta if body==a else 1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame+0x10,b'\0'*160)
            callbacks=[]
            def callback(uc,va,size,user):
                if aliased:uc.mem_write(a+0x3c,struct.pack('<f',delta))
                callbacks.append(1)
                return_from_hook(uc)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            old=words(machine,b+0x10,4);new=words(machine,replacement+0x10,4)
            old_expected=(0x3f800000,0,0,0) if aliased and row_index==0 else (0x3f800000,0,0,0xbf800000)
            new_expected=(0x3f800000,0,0,0xbf800000) if aliased and row_index==0 else (0x40a00000,0,0,0)
            assert old==old_expected,(label,aliased,row_index,precision,rounding,old,new)
            assert new==new_expected,(label,aliased,row_index,precision,rounding,old,new)
            assert callbacks==[1]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,old,new,callbacks)
            else:assert snapshots[rounding]==(snapshot,old,new,callbacks)


# The second unrolled pair keeps both destinations, unlike the first pair.
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('aliased',[False,True])
@pytest.mark.parametrize('row_index',[0,1])
def test_second_pair_fallback_retains_original_linear_receiver(solver_images,precision,aliased,row_index):
    frame=STACK+32768-0xd0
    a=frame-0x10 if aliased else HEAP
    b,replacement=HEAP+0x80,HEAP+0x180
    delta=as_float(replacement)-as_float(b)
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS,'6I',0x11801,TERMS,RESULTS,a,b,8)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,3)
            put(data,OPS+36,'5I',3|(8<<8)|(5<<16),0,3|(8<<8)|(5<<16),0,0x400)
            put(data,TERMS,'48f',*([0.0]*24+([0,0,0,1,0,0,0,1,0,0,0,1, 0,0,0,0,0,0,0,1,0,0,0,1] if row_index==0 else [0,0,0,0,0,0,0,1,0,0,0,1, 0,0,0,1,0,0,0,1,0,0,0,1])))
            put(data,HEAP,'B',3)
            for body in [HEAP,b,replacement]:
                put(data,body+0x10,'4f',1 if body==b else 5 if body==replacement else 0,0,0,0)
                put(data,body+0x20,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,delta if body==a else 1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame+0x10,b'\0'*160)
            callbacks=[]
            def callback(uc,va,size,user):
                if aliased:uc.mem_write(a+0x3c,struct.pack('<f',delta))
                callbacks.append(1)
                return_from_hook(uc)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            old=words(machine,b+0x10,4);new=words(machine,replacement+0x10,4)
            old_expected=(0x3f800000,0,0,0xbf800000)
            new_expected=(0x40a00000,0,0,0)
            assert old==old_expected,(label,aliased,row_index,precision,rounding,old,new)
            assert new==new_expected,(label,aliased,row_index,precision,rounding,old,new)
            assert callbacks==[1]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,old,new,callbacks)
            else:assert snapshots[rounding]==(snapshot,old,new,callbacks)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_first_pair_fallback_preserves_unrelated_scalar_word(solver_images, precision):
    frame = STACK + 32768 - 0xd0
    body_a, body_b = HEAP + 0x80, HEAP
    body_word = as_float(body_b)
    gain = 1 / body_word
    assert body_word * gain == 1
    assert frame % 16 == 0
    retail, compiled, symbols = solver_images
    targets = [
        ('retail', retail, 0x31c940, 0x480c60),
        ('compiled', compiled, address(symbols, 'solve_operations_31c940'),
         address(symbols, 'g_480c60')),
    ]
    snapshots = {}
    for label, machine, entry, gate in targets:
        for rounding in range(4):
            data = solver_case(3, 0)
            put(data, STEP, '7fi', 0, gain, 0, 1, 1, 0, 0, 1)
            put(data, OPS, '6I', 0x11801, frame, RESULTS, body_a, body_b, 8)
            put(data, OPS + 24, '3I', 0x50803, 0, 0x400)
            put(data, RESULTS, '4f', 0, 0, 0, 0)
            for body in (body_a, body_b):
                put(data, body, 'B', 3)
                for offset in (0x10, 0x20, 0x40, 0x50):
                    put(data, body + offset, '4f', 0, 0, 0, 0)
                put(data, body + 0x30, '4f', 1, 1, 1, 1)

            # The aligned row's A angular axis occupies frame +0x10.
            # Z observes the fallback's rounded input at +0x18. W is the
            # row's original mass at +0x1c, which this path must preserve.
            # The real header supplies the row target through B's word at
            # +0x0c. Its power-of-two float encoding and reciprocal scale
            # give an exact unit fallback impulse. The real saved iteration
            # and scale words overlap other row lanes; no hook supplies them.
            geometry = (
                0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 1,
            )
            machine.mem_write(frame, b'\0' * 192)
            machine.mem_write(frame, struct.pack('<24f', *geometry))
            machine.mem_write(gate, b'\x01')
            target = body_a + 0x20
            trace, pending = [], [False]

            def written(uc, access, va, size, value, user):
                # A MOVAPS may be reported as two eight-byte writes.
                if va == target and size > 4:
                    pending[0] = True

            def observe(uc, va, size, user):
                if pending[0]:
                    trace.append((words(uc, target, 4),
                                  words(uc, frame + 0x10, 4),
                                  words(uc, frame + 0x18, 2)))
                    pending[0] = False

            # Observe the complete vector before later passes reuse its
            # scalar inputs. Both hooks only read; execute checks the ABI.
            handles = [machine.hook_add(UC_HOOK_MEM_WRITE, written),
                       machine.hook_add(UC_HOOK_CODE, observe)]
            control_word = precision | (rounding << 10)
            try:
                snapshot = execute(machine, entry, data, [STEP, OPS, HEAP, RESULTS],
                                   precision=control_word)
            finally:
                for handle in handles:
                    machine.hook_del(handle)
            context = (label, precision, rounding, trace)
            assert machine.reg_read(UC_X86_REG_FPCW) == control_word, context
            assert snapshot[1] == 1, context
            assert len(trace) == 1, context
            if label == 'retail':
                expected = (0xef000000, 0x3f800000, 0x3f800000, 0x3f800000)
                assert trace[0] == (expected, expected,
                                    (0x3f800000, 0x3f800000)), context
                snapshots[rounding] = (snapshot, tuple(trace))
            else:
                assert snapshots[rounding] == (snapshot, tuple(trace)), context
