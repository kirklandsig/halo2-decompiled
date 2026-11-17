"""Optional retail checks of motion construction and the sibling constructor."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

@pytest.mark.parametrize('retail_va,class_name',[(0x31ad80,'class_31ad80'),(0x31a870,'class_31a870')])
@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
def test_motion_construction_against_retail(solver_images,retail_va,class_name,precision):
    retail,compiled,symbols=solver_images
    hits={s.va for s in symbols.find(class_name+'::'+class_name)}
    assert len(hits)==1, 'rebuild the checker image with the constructor'
    entry=hits.pop()
    tables={s.va for s in symbols.symbols if s.name.startswith('??_7'+class_name+'@@')}
    assert len(tables)==1
    table=tables.pop()
    # The retail base constructor installs a normal x86 exception frame.
    for uc in (retail,compiled):
        if not any(low==0 for low,high,permissions in uc.mem_regions()): uc.mem_map(0,4096)
    for seed in range(64):
        rng=random.Random(seed)
        data=bytearray(rng.randbytes(65536))
        position=HEAP+[1024,0xc0,0x20,1024][seed%4]
        orientation=HEAP+[1040,0xf0,0x30,0x100][seed%4]
        values=[rng.uniform(-4,4) for _ in range(8)]
        if seed%8==0: values=[0.0,-0.0,0.0,1.0]*2
        if seed%8==1: values=[float('inf'),0.0,float('-inf'),1.0]*2
        if seed%8==2: values=[float('nan'),-0.0,0.0,0.0]*2
        if seed%8==3: values=[1e-38,-1e-38,1e-38,-1e-38]*2
        if seed%8==4: values=[1e30,-1e30,1e30,-1e30]*2
        struct.pack_into('<4f',data,position-HEAP,*values[:4])
        struct.pack_into('<4f',data,orientation-HEAP,*values[4:])
        results=[]
        for uc,start in [(retail,retail_va),(compiled,entry)]:
            uc.mem_write(HEAP,bytes(data)); uc.mem_write(STACK,b'\xcd'*65536)
            uc.mem_write(0,struct.pack('<I',0xffffffff))
            sp=STACK+32768
            uc.mem_write(sp,struct.pack('<III',STOP,position,orientation))
            saved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
            for reg in saved+[UC_X86_REG_EAX,UC_X86_REG_EDX]: uc.reg_write(reg,0x12345678)
            uc.reg_write(UC_X86_REG_ECX,HEAP); uc.reg_write(UC_X86_REG_ESP,sp)
            uc.reg_write(UC_X86_REG_EFLAGS,2); uc.reg_write(UC_X86_REG_FPCW,precision)
            uc.reg_write(UC_X86_REG_FPTAG,0xffff); uc.reg_write(UC_X86_REG_FPSW,0)
            uc.reg_write(UC_X86_REG_MXCSR,0x1f80)
            uc.emu_start(start,STOP,count=10000)
            assert uc.reg_read(UC_X86_REG_EIP)==STOP and uc.reg_read(UC_X86_REG_EAX)==HEAP
            assert uc.reg_read(UC_X86_REG_ESP)==sp+12
            assert all(uc.reg_read(reg)==0x12345678 for reg in saved)
            assert bytes(uc.mem_read(0,4))==b'\xff'*4
            result=bytearray(uc.mem_read(HEAP,65536))
            if uc is compiled: assert struct.unpack_from('<I',result)[0]==table
            # The generated class-table address relocates between the images.
            result[:4]=bytes(4)
            results.append(result)
        assert results[0]==results[1],(seed,precision,class_name,next((i for i,(a,b) in enumerate(zip(*results)) if a!=b),None))
