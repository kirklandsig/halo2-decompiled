"""Optional retail differential tests with synthetic convex features and shapes.

The separately owned tetrahedron helper and penetration-query boundary are
intercepted. Other feature helpers execute normally in both images.
"""
import math
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

SEARCH, FIRST, SECOND, TRANSFORM, STATE, RESULT = [HEAP+x for x in (0,1024,1280,1536,1792,2048)]
VTABLE, SUPPORT, VERTICES = HEAP+2304, STOP+16, STOP+32

def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, ('rebuild the checker image', name, hits)
    return hits.pop()

def words(uc, address_, count):
    return struct.unpack('<'+'I'*count, uc.mem_read(address_,4*count))

def vector(values, identity=0):
    return struct.pack('<3fI', *values[:3], identity)

def base_case(seed):
    rng = random.Random(seed)
    data = bytearray(b'\xcd'*65536)
    def put(at, value): data[at-HEAP:at-HEAP+len(value)] = value
    for offset in range(0,0x170,16):
        put(SEARCH+offset,vector([rng.uniform(-3,3) for _ in range(3)],offset//16))
    put(FIRST,struct.pack('<I',VTABLE)); put(SECOND,struct.pack('<I',VTABLE))
    put(VTABLE+48,struct.pack('<II',SUPPORT,VERTICES))
    angle = seed*0.37
    rotation = [(math.cos(angle),math.sin(angle),0),(-math.sin(angle),math.cos(angle),0),(0,0,1)]
    for i, row in enumerate(rotation): put(TRANSFORM+16*i,vector(row))
    put(TRANSFORM+48,vector([rng.uniform(-2,2) for _ in range(3)]))
    put(STATE,bytes(16))
    return rng,data,put

def execute(uc, entry, args, precision):
    sp=STACK+32768
    uc.mem_write(STACK,b'\xcd'*65536)
    uc.mem_write(sp,struct.pack('<'+'I'*(len(args)+1),STOP,*args))
    preserved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
    for reg in preserved+[UC_X86_REG_EAX,UC_X86_REG_EDX]: uc.reg_write(reg,0x12345678)
    uc.reg_write(UC_X86_REG_ECX,SEARCH)
    uc.reg_write(UC_X86_REG_ESP,sp); uc.reg_write(UC_X86_REG_EFLAGS,2)
    uc.reg_write(UC_X86_REG_FPCW,precision); uc.reg_write(UC_X86_REG_FPTAG,0xffff)
    uc.reg_write(UC_X86_REG_FPSW,0); uc.reg_write(UC_X86_REG_MXCSR,0x1f80)
    uc.emu_start(entry,STOP,count=500000)
    assert uc.reg_read(UC_X86_REG_EIP)==STOP
    assert uc.reg_read(UC_X86_REG_ESP)==sp+4+len(args)*4
    assert all(uc.reg_read(reg)==0x12345678 for reg in preserved)
    return uc.reg_read(UC_X86_REG_EAX),bytes(uc.mem_read(HEAP,65536))

def return_from(uc, value, cleanup):
    sp=uc.reg_read(UC_X86_REG_ESP)
    uc.reg_write(UC_X86_REG_EAX,value & 0xffffffff)
    uc.reg_write(UC_X86_REG_EIP,words(uc,sp,1)[0])
    uc.reg_write(UC_X86_REG_ESP,sp+4+cleanup)

def penetration_hook(uc, at, seed, output, trace):
    first,second,transform,simplex,differences,result=words(uc,uc.reg_read(UC_X86_REG_ESP)+4,6)
    count=words(uc,simplex+128,1)[0]
    trace.append(('penetration',first,second,transform,count,
                  bytes(uc.mem_read(simplex,count*16)),
                  bytes(uc.mem_read(simplex+64,count*16)),
                  bytes(uc.mem_read(differences,count*16))))
    a,b=output
    for i in range(3):
        uc.mem_write(simplex+16*i,a[i]); uc.mem_write(simplex+64+16*i,b[i])
    uc.mem_write(simplex+128,struct.pack('<I',1 if seed%13==0 else 3))
    uc.mem_write(result,struct.pack('<4f',0.5,-0.25,0.75,0.0))
    return_from(uc,[0,1,2,0xffffffff][seed%4],0)

def returned_features(rng,seed):
    sides=[]
    for side in range(2):
        count=1+(seed//(3**side))%3
        points=[vector([rng.uniform(-3,3) for _ in range(3)],side*16+i) for i in range(count)]
        # Identical XYZ vertices retain different identity lanes in some cases.
        sides.append([points[i%count] for i in range(3)])
    return sides

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_penetration_fallback_against_retail(solver_images,precision):
    retail,compiled,symbols=solver_images
    entry=address(symbols,'class_3048a0::function_303dd0')
    pen=address(symbols,'function_30a6b0')
    zero=address(symbols,'g_4801e0')
    for seed in range(144):
        rng,data,put=base_case(seed)
        a,b=1+seed%4,1+(seed//4)%4
        put(SEARCH,struct.pack('<3i',a,b,seed%3))
        output=returned_features(rng,seed)
        results=[]; traces=[]
        for uc,start,boundary,zero_address in [(retail,0x303dd0,0x30a6b0,0x4801e0),(compiled,entry,pen,zero)]:
            uc.mem_write(HEAP,bytes(data)); uc.mem_write(zero_address,bytes(16)); trace=[]
            def hook(machine,pc,size,user):
                if pc==boundary: penetration_hook(machine,pc,seed,output,trace)
            handle=uc.hook_add(UC_HOOK_CODE,hook)
            try: results.append(execute(uc,start,[FIRST,SECOND,TRANSFORM,STATE],precision))
            finally: uc.hook_del(handle)
            traces.append(trace)
        assert traces[0]==traces[1],(seed,precision,'calls',traces)
        assert results[0]==results[1],(seed,precision,'first difference',next((i for i,(a,b) in enumerate(zip(results[0][1],results[1][1])) if a!=b),None))

@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_closest_search_against_retail(solver_images,precision):
    retail,compiled,symbols=solver_images
    entry=address(symbols,'class_3048a0::function_3048a0'); pen=address(symbols,'function_30a6b0')
    tetra=address(symbols,'convex_search_tetrahedron_boundary'); zero=address(symbols,'g_4801e0')
    visited=set()
    for seed in range(320):
        rng,data,put=base_case(seed+10000)
        initial=[(1,1),(1,2),(2,1),(1,3),(3,1),(2,2)][seed%6]
        put(STATE,struct.pack('<4H4b',0,1,2,3,*initial,4,4))
        put(SEARCH+8,struct.pack('<i',seed%3))
        put(SEARCH+12,bytes([seed%2]))
        hulls=[[vector([rng.uniform(-2,2)+(3 if side else 0) for _ in range(3)],i) for i in range(4)] for side in range(2)]
        if seed%17==0: hulls[1]=hulls[0][:]
        if seed%19==0: hulls[0]=[hulls[0][0]]*4
        if seed%23==0: hulls=[[vector([0.0,0.0,0.0],i) for i in range(4)] for _ in range(2)]
        if seed%29==0: hulls[0]=[vector([-0.0,0.0,-0.0],i) for i in range(4)]
        if seed%31==0: hulls[0]=[vector([float('inf'),1.0,0.0],i) for i in range(4)]
        if seed%37==0: hulls[0]=[vector([float('nan'),0.0,1.0],i) for i in range(4)]
        if seed%41==0: hulls[0]=[vector([i*1e-38,-i*1e-38,1e-38],i) for i in range(4)]
        if seed%43==0: hulls[0]=[vector([i*1e30,-i*1e30,1e30],i) for i in range(4)]
        if seed%53==0:
            # A point above the interior of a triangle forces a fourth support
            # vertex, exercising the first-side tetrahedron reduction.
            hulls[0]=[vector(p,i) for i,p in enumerate([(0,0,0),(2,0,0),(0,2,0),(.25,.25,2)])]
            hulls[1]=[vector((.25,.25,.25),i) for i in range(4)]
            put(STATE,struct.pack('<4H4b',0,1,2,3,3,1,4,4))
            for i,row in enumerate([(1,0,0),(0,1,0),(0,0,1),(0,0,0)]): put(TRANSFORM+16*i,vector(row))
        output=returned_features(rng,seed)
        results=[]; traces=[]
        for uc,start,pen_at,tetra_at,zero_at,is_retail in [(retail,0x3048a0,0x30a6b0,0x303540,0x4801e0,True),(compiled,entry,pen,tetra,zero,False)]:
            uc.mem_write(HEAP,bytes(data)); uc.mem_write(zero_at,bytes(16)); trace=[]
            def hook(machine,pc,size,user):
                if is_retail: visited.add(pc)
                sp=machine.reg_read(UC_X86_REG_ESP)
                if pc==VERTICES:
                    ids,count,out=words(machine,sp+4,3)
                    shape=machine.reg_read(UC_X86_REG_ECX); side=int(shape==SECOND)
                    indices=struct.unpack('<'+'H'*count,machine.mem_read(ids,count*2))
                    trace.append(('vertices',side,indices,out))
                    for i,index in enumerate(indices): machine.mem_write(out+16*i,hulls[side][index%4])
                    return_from(machine,0,12)
                elif pc==SUPPORT:
                    direction,out=words(machine,sp+4,2)
                    values=struct.unpack('<4f',machine.mem_read(direction,16))
                    shape=machine.reg_read(UC_X86_REG_ECX); side=int(shape==SECOND)
                    index=max(range(4),key=lambda i:sum(a*b for a,b in zip(struct.unpack('<3f',hulls[side][i][:12]),values[:3])))
                    trace.append(('support',side,index,out,bytes(machine.mem_read(direction,16))))
                    machine.mem_write(out,hulls[side][index]); return_from(machine,0,8)
                elif pc==pen_at: penetration_hook(machine,pc,seed,output,trace)
                elif pc==tetra_at:
                    if is_retail: point=machine.reg_read(UC_X86_REG_EBX); corners=machine.reg_read(UC_X86_REG_EAX); sign=words(machine,sp+4,1)[0]
                    else: point,corners,sign=words(machine,sp+4,3)
                    trace.append(('tetra',point,corners,sign))
                    return_from(machine,-1 if seed%3==0 else seed%4,0)
            handle=uc.hook_add(UC_HOOK_CODE,hook)
            try: results.append(execute(uc,start,[FIRST,SECOND,TRANSFORM,STATE,RESULT],precision))
            finally: uc.hook_del(handle)
            traces.append(trace)
        assert traces[0]==traces[1],(seed,precision,'calls',traces)
        assert results[0]==results[1],(seed,precision,'first difference',next((i for i,(a,b) in enumerate(zip(results[0][1],results[1][1])) if a!=b),None))
    # Both edge/triangle orders, tetrahedron orders, and support-query orders.
    required={0x3049ae,0x3049cc,0x304aa1,0x304c12,0x304e0f,0x3051ff,0x3053ef,0x30550a,0x305515}
    assert required <= visited, [hex(pc) for pc in required-visited]
