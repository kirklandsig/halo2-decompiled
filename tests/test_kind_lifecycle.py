"""Real kind-object construction/factory and teardown, with synthetic services.

The reference destructor executes normally. The registry-name setter and
allocator are boundary hooks; the incomplete kind-object table is not modeled
as a complete runtime interface by these tests.
"""
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
from test_solver_rows import HEAP, STACK, STOP, solver_images
from test_solver_validation import address

OBJECT, ALLOCATOR, TABLE = [HEAP+n for n in (0x100,0x200,0x300)]
ALLOCATE, RELEASE = STOP+0x100, STOP+0x110


def invoke(uc, target, args=(), receiver=0):
    sp = STACK+0x8000
    uc.mem_write(STACK,b'\xa7'*65536)
    uc.mem_write(sp,struct.pack('<'+'I'*(len(args)+1),STOP,*args))
    saved=[UC_X86_REG_EBX,UC_X86_REG_ESI,UC_X86_REG_EDI,UC_X86_REG_EBP]
    for i,reg in enumerate(saved): uc.reg_write(reg,0x123400+i)
    uc.reg_write(UC_X86_REG_ECX,receiver)
    uc.reg_write(UC_X86_REG_ESP,sp)
    uc.reg_write(UC_X86_REG_EFLAGS,2)
    uc.emu_start(target,STOP,count=10000)
    assert uc.reg_read(UC_X86_REG_EIP)==STOP
    assert uc.reg_read(UC_X86_REG_ESP)==sp+4+(len(args)*4 if receiver else 0)
    assert [uc.reg_read(r) for r in saved]==[0x123400+i for i in range(4)]
    return uc.reg_read(UC_X86_REG_EAX)


def run(uc, entries, seed, factory, deleting):
    data=bytearray(random.Random(seed).randbytes(65536))
    struct.pack_into('<I',data,ALLOCATOR-HEAP,TABLE)
    struct.pack_into('<2I',data,TABLE+16-HEAP,ALLOCATE,RELEASE)
    uc.mem_write(HEAP,bytes(data))
    uc.mem_write(entries['allocator'],struct.pack('<I',ALLOCATOR))
    events=[]
    def words(p,n): return struct.unpack('<'+'I'*n,uc.mem_read(p,4*n))
    def hook(machine,pc,size,user):
        sp=uc.reg_read(UC_X86_REG_ESP);ret=words(sp,1)[0]
        receiver=uc.reg_read(UC_X86_REG_ECX)
        result=0
        if pc==ALLOCATE:
            assert receiver==ALLOCATOR and words(sp+4,2)==(8,0x26)
            events.append(('allocate',8,0x26));pop=8;result=OBJECT
        elif pc==RELEASE:
            assert receiver==ALLOCATOR
            obj,length,tag=words(sp+4,3)
            assert obj==OBJECT and tag==0x26
            assert length==struct.unpack('<H',uc.mem_read(OBJECT+4,2))[0]
            events.append(('release',length,tag));pop=12
        else:
            assert pc==entries['unregister'] and receiver==OBJECT
            assert words(sp+4,1)==(0,)
            flags=struct.unpack('<H',uc.mem_read(OBJECT+4,2))[0]
            assert not flags & 0x8000
            events.append(('unregister',flags));pop=4
        uc.reg_write(UC_X86_REG_EAX,result)
        uc.reg_write(UC_X86_REG_ECX,0xbad0)
        uc.reg_write(UC_X86_REG_EDX,0xbad1)
        uc.reg_write(UC_X86_REG_ESP,sp+4+pop)
        uc.reg_write(UC_X86_REG_EIP,ret)
    hooks=[uc.hook_add(UC_HOOK_CODE,hook,begin=p,end=p)
           for p in (ALLOCATE,RELEASE,entries['unregister'])]
    try:
        result=invoke(uc,entries['factory'] if factory else entries['constructor'],
                      receiver=0 if factory else OBJECT)
        assert result==OBJECT and words(OBJECT,1)[0]==entries['table']
        assert struct.unpack('<H',uc.mem_read(OBJECT+6,2))[0]==1
        constructed=bytearray(uc.mem_read(HEAP,65536))
        struct.pack_into('<I',constructed,OBJECT-HEAP,0x415100)
        flags=[0,8,0x123,0x7fff][seed//2%4] | (0x8000 if seed & 1 else 0)
        count=[0,1,2,0xffff][seed//8%4]
        uc.mem_write(OBJECT+4,struct.pack('<2H',flags,count))
        delete_flags=[0,1,2,3,0xffffffff,0x80000000][seed//16%6]
        if deleting:
            # Reach the compiler-generated deleting wrapper through the
            # constructed object's real table.
            entry=words(entries['table'],1)[0]
            assert entry==entries['deleting']
            assert invoke(uc,entry,[delete_flags],OBJECT)==OBJECT
        else:
            invoke(uc,entries['destructor'],receiver=OBJECT)
    finally:
        for handle in hooks: uc.hook_del(handle)
    assert words(OBJECT,1)[0]==entries['base_table']
    assert struct.unpack('<2H',uc.mem_read(OBJECT+4,4))==(flags & 0x7fff,count)
    expected=[]
    if factory: expected.append(('allocate',8,0x26))
    if flags & 0x8000: expected.append(('unregister',flags & 0x7fff))
    if deleting and delete_flags & 1: expected.append(('release',flags & 0x7fff,0x26))
    assert events==expected
    destroyed=bytearray(uc.mem_read(HEAP,65536))
    struct.pack_into('<I',destroyed,OBJECT-HEAP,0x452c30)
    return constructed,destroyed,events


@pytest.mark.parametrize('factory',[False,True],ids=['constructor','factory'])
@pytest.mark.parametrize('deleting',[False,True],ids=['destructor','delete'])
def test_kind_object_lifetime_matches_retail(solver_images,factory,deleting):
    retail,compiled,symbols=solver_images
    ra=dict(constructor=0x3192c0,factory=0x31c4f0,destructor=0x3192b0,
            deleting=0x3192d0,allocator=0x480118,unregister=0x2d7240,
            table=0x415100,base_table=0x452c30)
    def table(name):
        hits={s.va for s in symbols.symbols if s.name=='??_7'+name+'@@6B@'}
        assert len(hits)==1
        return hits.pop()
    ca=dict(constructor=address(symbols,'kind_object_3192b0::kind_object_3192b0'),
            factory=address(symbols,'function_31c4f0'),
            destructor=address(symbols,'kind_reference_3192b0::~kind_reference_3192b0'),
            deleting=address(symbols,"kind_object_3192b0::`deleting destructor'"),
            allocator=address(symbols,'g_480118'),unregister=address(symbols,'c_a::v2'),
            table=table('kind_object_3192b0'),base_table=table('c_sized_reference'))
    for seed in range(96):
        expected=run(retail,ra,seed,factory,deleting)
        actual=run(compiled,ca,seed,factory,deleting)
        assert actual==expected,(seed,factory,deleting)
