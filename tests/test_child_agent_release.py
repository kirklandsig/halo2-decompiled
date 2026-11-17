"""Optional retail comparison of child release, including callback mutation."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS,
)
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

ENTRIES, CHILDREN, TABLES = HEAP+256, HEAP+1024, HEAP+4096
DESTROY, RELEASE, ALTERNATE_RELEASE, ALTERNATE_DESTROY = STOP+16, STOP+32, STOP+48, STOP+64

@pytest.mark.parametrize('mutate',[False,True])
def test_child_release_against_retail(solver_images,mutate):
    retail,compiled,symbols=solver_images
    hits={s.va for s in symbols.find('bounded_2e9530::function_2e90b0')}
    assert len(hits)==1, 'rebuild the checker image with the release routine'
    entry=hits.pop()
    for seed in range(96):
        rng=random.Random(seed)
        count=[0,1,2,3,8,17,24,32][seed%8]
        data=bytearray(b'\xcd'*65536)
        def put(at,fmt,*values): struct.pack_into('<'+fmt,data,at-HEAP,*values)
        put(HEAP,'I',TABLES)
        put(HEAP+12,'II',0 if count==0 and seed%2 else ENTRIES,count)
        put(TABLES,'I',DESTROY); put(TABLES+64,'I',ALTERNATE_DESTROY)
        put(TABLES+128+20,'I',RELEASE); put(TABLES+192+20,'I',ALTERNATE_RELEASE)
        for i in range(32):
            put(CHILDREN+16*i,'II',TABLES+128+(64 if i%2 else 0),0)
            child=0 if rng.randrange(4)==0 else CHILDREN+16*rng.randrange(32)
            put(ENTRIES+12*i,'III',rng.getrandbits(32),rng.getrandbits(32),child)
        results=[]; traces=[]
        for uc,start in [(retail,0x2e90b0),(compiled,entry)]:
            uc.mem_write(HEAP,bytes(data)); trace=[]; released=[]
            def hook(machine,pc,size,user):
                if pc not in (DESTROY,RELEASE,ALTERNATE_RELEASE,ALTERNATE_DESTROY): return
                sp=machine.reg_read(UC_X86_REG_ESP)
                this=machine.reg_read(UC_X86_REG_ECX)
                if pc in (RELEASE,ALTERNATE_RELEASE):
                    trace.append(('release',pc,this)); released.append(this)
                    value=struct.unpack('<I',machine.mem_read(this+4,4))[0]
                    machine.mem_write(this+4,struct.pack('<I',value+1))
                    if mutate:
                        machine.mem_write(HEAP+12,struct.pack('<II',HEAP+8192,0))
                        machine.mem_write(HEAP,struct.pack('<I',TABLES+64))
                        index=min(len(released)+1,31)
                        replacement=0 if seed%3==0 else CHILDREN+16*((seed+index)%32)
                        machine.mem_write(ENTRIES+12*index+8,struct.pack('<I',replacement))
                    cleanup=0
                else:
                    flag=struct.unpack('<I',machine.mem_read(sp+4,4))[0]
                    trace.append(('destroy',pc,this,flag)); assert flag==1 and this==HEAP
                    machine.mem_write(HEAP+8,struct.pack('<I',0xfeed1234)); cleanup=4
                machine.reg_write(UC_X86_REG_EAX,0xabcdef01)
                machine.reg_write(UC_X86_REG_ECX,0x87654321)
                machine.reg_write(UC_X86_REG_EDX,0x10203040)
                machine.reg_write(UC_X86_REG_EIP,struct.unpack('<I',machine.mem_read(sp,4))[0])
                machine.reg_write(UC_X86_REG_ESP,sp+4+cleanup)
            handle=uc.hook_add(UC_HOOK_CODE,hook)
            sp=STACK+32768
            uc.mem_write(STACK,b'\xcd'*65536); uc.mem_write(sp,struct.pack('<I',STOP))
            saved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
            for reg in saved: uc.reg_write(reg,0x12345678)
            uc.reg_write(UC_X86_REG_ECX,HEAP); uc.reg_write(UC_X86_REG_ESP,sp)
            uc.reg_write(UC_X86_REG_EFLAGS,2)
            try: uc.emu_start(start,STOP,count=10000)
            finally: uc.hook_del(handle)
            assert uc.reg_read(UC_X86_REG_EIP)==STOP
            assert uc.reg_read(UC_X86_REG_ESP)==sp+4
            assert all(uc.reg_read(reg)==0x12345678 for reg in saved)
            assert trace[-1][0]=='destroy'
            results.append(bytes(uc.mem_read(HEAP,65536))); traces.append(trace)
        assert traces[0]==traces[1],(seed,mutate,traces)
        assert results[0]==results[1],(seed,mutate)
