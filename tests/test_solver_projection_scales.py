"""Retail differential coverage of step scales aliased with solver scalar outputs."""
import struct

import pytest
from test_solver_dispatch import HEAP,STACK,OPS,TERMS,IMPULSES,FRICTION_RESULTS,OP_LAYOUT,solver_images,solver_case,put,address,execute,words

CASES=[]
for kind,slot,coefficient in [(2,0x50,'error'),(4,0x4c,'error'),(5,0x74,'error'),(6,0x64,'friction'),(7,0x78,'friction'),(8,0x40,'friction'),(9,0x80,'friction'),(10,0x44,'velocity'),(11,0x6c,'error'),(12,0x54,'error'),(13,0x5c,'error')]:
    CASES.append((kind,coefficient,slot))
    if kind not in [4,5,10]:CASES.append((kind,'velocity',slot))

# The spinning-friction limit shares this home with an incoming scale. Both
# second-row products must observe that scale before the limit is published.
CASES.append((8,'friction',0x34))

@pytest.mark.parametrize('kind,coefficient,slot',CASES)
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_operation_scales_observe_retail_projection_epoch(solver_images,precision,kind,coefficient,slot):
    frame=STACK+32768-0xd0
    step=frame+slot-{'error':4,'velocity':8,'friction':12}[coefficient]
    retail,compiled,symbols=solver_images;snapshots={}
    for label,machine,entry,gate in [('retail',retail,0x31c940,0x480c60),('compiled',compiled,address(symbols,'solve_operations_31c940'),address(symbols,'g_480c60'))]:
        for rounding in range(4):
            data=solver_case(kind,0);size,row_kind=OP_LAYOUT[kind]
            put(data,OPS+24,'I',kind|(size<<8)|(row_kind<<16));put(data,OPS+24+size,'I',0x400)
            if size>=8:put(data,OPS+28,'f',100)
            if size>=12:put(data,OPS+32,'f',-100)
            if size>=16:put(data,OPS+36,'f',1)
            if kind==10:put(data,OPS+28,'3f',0,0,1)
            if kind==11:put(data,OPS+28,'2f',0,10)
            if kind in [7,8]:
                put(data,OPS+28,'I',FRICTION_RESULTS)
                put(data,OPS+32,'f',0);put(data,OPS+36,'f',100);put(data,OPS+40,'I',12)
            if kind==8:put(data,OPS+44,'f',1)
            if row_kind in [3,5,6]:
                put(data,TERMS,'12f',1,0,0,2,0,0,0,1,0,0,0,1)
                if row_kind in [5,6]:put(data,TERMS+48,'12f',0,1,0,2,0,0,0,1,0,0,0,1)
                if row_kind==6:put(data,TERMS+96,'8f',0,0,1,1,0,0,0,2)
            else:put(data,TERMS,'8f',1,0,0,1,0,0,0,2)
            put(data,HEAP,'B',3)
            for body in [HEAP,HEAP+0x80]:
                put(data,body+0x10,'4f',1 if body==HEAP else 0,1 if body==HEAP else 0,0,0)
                put(data,body+0x20,'4f',1 if body==HEAP else 0,0,1 if body==HEAP else 0,0)
                put(data,body+0x30,'4f',1,1,1,1)
            put(data,IMPULSES,'4f',0,0,0,0)
            machine.mem_write(frame+0x10,b'\0'*176)
            machine.mem_write(step,struct.pack('<7fi',0,2,2 if coefficient=='velocity'else 1,2,1,0,0,1))
            machine.mem_write(gate,b'\x01')
            snapshot=execute(machine,entry,data,[step,OPS,HEAP,IMPULSES],precision=precision|(rounding<<10))
            assert snapshot[1]==1
            before=bytes(data)[0x10:0x30]
            observed=bytes(machine.mem_read(HEAP+0x10,32))
            assert observed!=before,(label,'inactive input',kind,coefficient,precision,rounding)
            if label=='retail':snapshots[rounding]=snapshot
            else:assert snapshots[rounding]==snapshot,(kind,coefficient,precision,rounding)
        print(label,'projection scale matrix4RM',kind,coefficient,hex(precision),flush=True)
