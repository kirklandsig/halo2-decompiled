"""Independent retail proofs for the first normal pair's fallback axis reads."""
import struct
import pytest
from test_solver_dispatch import HEAP,STACK,STEP,OPS,TERMS,IMPULSES,RESULTS,CALLBACK,solver_images,solver_case,put,address,execute,words,return_from_hook

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('aliased',[False,True])
@pytest.mark.parametrize('row_index',[0,1])
def test_first_pair_fallback_linear_axis_read_epoch(solver_images,precision,aliased,row_index):
    frame=STACK+32768-0xd0
    rows=frame+(0x10 if row_index==0 else -0x20) if aliased else TERMS
    # First error, coupled impulse and rounded input overwrite the aliased
    # axis at homes14/10/18. Retail retains (1,0,0,1) from projection.
    if row_index==0:
        values=[1,0,0,1,0,0,0,1,0,0,0,1, 0,0,0,0,0,0,0,1,0,0,0,1]
    else:
        # The second row reloads its axis after the rounded input changes Z.
        values=[0,0,0,0,0,0,0,1,0,0,0,1, 1,0,0,1,0,0,0,1,0,0,0,1]
    retail,compiled,symbols=solver_images
    snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(3,0)
            put(data,STEP,'7fi',0,1,0,1,1,0,0,1)
            put(data,OPS,'6I',0x11801,rows,RESULTS,HEAP,HEAP+0x80,8)
            put(data,OPS+24,'3I',14|(12<<8)|(2<<16),CALLBACK,3)
            put(data,OPS+36,'3I',3|(8<<8)|(5<<16),0,0x400)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                for offset in [0x10,0x20]:put(data,body+offset,'4f',0,0,0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'2f',0,0)
            machine.mem_write(frame+0x10,b'\0'*160)
            if aliased:machine.mem_write(rows,struct.pack('<24f',*values))
            else:put(data,rows,'24f',*values)
            callbacks=[]
            def callback(uc,va,size,user):
                # This mass shares the iteration home, which entry cleared.
                # The existing callback supplies it before projection runs.
                if aliased:uc.mem_write(frame+0x2c,struct.pack('<f',1))
                callbacks.append(1)
                return_from_hook(uc)
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[STEP,OPS,HEAP,IMPULSES],hooks=[(CALLBACK,callback)],precision=precision|(rounding<<10))
            observed=words(machine,HEAP+0x10,4)
            assert observed==(0x3f800000,0,0x3f800000 if aliased and row_index==1 else 0,0x3f800000),(label,aliased,precision,rounding,observed)
            assert words(machine,HEAP+0x90,4)==(0xbf800000,0,0xbf800000 if aliased and row_index==1 else 0,0xbf800000)
            assert callbacks==[1]
            assert snapshot[1]==1
            if label=='retail':snapshots[rounding]=(snapshot,observed,callbacks)
            else:assert snapshots[rounding]==(snapshot,observed,callbacks)
