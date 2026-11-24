"""Retail proof for both first-pair fallbacks' angular receiver epoch."""
import struct
import pytest
from test_solver_dispatch import HEAP,STACK,STEP,OPS,TERMS,IMPULSES,RESULTS,CALLBACK,solver_images,solver_case,put,address,execute,words,return_from_hook

def as_float(bits):return struct.unpack('<f',struct.pack('<I',bits))[0]

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('aliased',[False,True])
@pytest.mark.parametrize('row_index',[0,1])
def test_first_fallback_retains_angular_receiver_before_a_write(solver_images,precision,aliased,row_index):
    frame=STACK+32768-0xd0
    a=frame-0x20 if aliased else HEAP
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
            put(data,TERMS,'24f',*([0,0,0,1,0,0,0,1,1,0,0,1, 0,0,0,0,0,0,0,1,0,0,0,1] if row_index==0 else [0,0,0,0,0,0,0,1,0,0,0,1, 0,0,0,1,0,0,0,1,1,0,0,1]))
            put(data,HEAP,'B',3)
            for body in [HEAP,b,replacement]:
                for offset in [0x10,0x20]:put(data,body+offset,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,delta if body==a else 1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame-0x10,b'\0'*16)
            machine.mem_write(frame+0x10,b'\0'*160)
            callbacks=[]
            def callback(uc,va,size,user):
                if aliased:uc.mem_write(a+0x3c,struct.pack('<f',delta))
                callbacks.append(1)
                return_from_hook(uc)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            old=words(machine,b+0x20,4);new=words(machine,replacement+0x20,4)
            assert old==(0x3f800000,0,0,0x3f800000),(label,aliased,precision,rounding,old,new)
            assert new==(0,0,0,0),(label,aliased,precision,rounding,old,new)
            assert callbacks==[1]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,old,new,callbacks)
            else:assert snapshots[rounding]==(snapshot,old,new,callbacks)
