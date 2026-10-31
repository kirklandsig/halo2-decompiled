"""Sorted pair insertion through real append helpers and synthetic virtuals."""
import struct
import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

MERGER, WORLD, PAIR, A, B, EA, EB, XA, XB = [HEAP+x for x in (0x100,0x400,0x800,0x900,0xa00,0xb00,0xc00,0xd00,0xe00)]
IA, IB, SHAPEA, SHAPEB, MA, MB, MX, MY = [HEAP+x for x in (0x1000,0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700)]
DISPATCH, NEXT_DISPATCH, TABLES, NEXT_TABLES = [HEAP+x for x in (0x1800,0x1900,0x2000,0x3400)]
CREATORS, OLD, OUTPUT, HANDLES, INFO, AGENT, FILTER, VT = [HEAP+x for x in (0x4800,0x5800,0x5c00,0x6000,0x6800,0x6900,0x6a00,0x6c00)]
CREATE, SHAPE, MOTION, FILTER_CALL, RELEASE, HANDLER, NEXT_HANDLER = [STOP+x for x in range(16,128,16)]


def symbol(mapping,name):
    hits={s.va for s in mapping.find(name)}
    assert len(hits)==1,(name,hits)
    return hits.pop()


def initial(seed, overlap):
    data=bytearray(b'\xcd'*65536)
    def put(at,fmt,*values):struct.pack_into('<'+fmt,data,at-HEAP,*values)
    put(MERGER+4,'I',WORLD)
    put(MERGER+0x50,'I',seed%3)
    count=seed%8
    put(MERGER+0x7c,'4I',DISPATCH,OLD,OLD+count*16,OLD if overlap else OUTPUT)
    put(PAIR,'2I',A+16,B+16)
    high=0x80000000 if seed%5==0 else 0
    key=seed//8%8
    put(A,'I',SHAPEA);put(B,'I',SHAPEB)
    put(A+16,'2I',0,high+key);put(B+16,'2I',0,seed%4)
    put(A+32,'I',EA);put(B+32,'I',EB)
    for i in range(8):
        h=HANDLES+i*32
        put(h,'4I',0,high+i,0,i%4)
        put(OLD+i*16,'4I',h,h+8,0x12340000+i,0xabcd0000+i)
    for entity,motion,island,kind in [(EA,MA,IA,seed%12),(EB,MB,IA if seed%4==0 else IB,seed//12%8),(XA,MX,IA,0),(XB,MY,IB,0)]:
        put(entity+0x3c,'2I',motion,seed//3%2)
        put(entity+0x44,'Ib',island,kind)
    if seed%9==0:put(EA+0x44,'I',MERGER-0x1c)
    for island in (IA,IB,MERGER-0x1c):
        put(island+0x24,'i',-1 if seed%7==0 else 3)
        put(island+0x29,'B',seed%2)
        put(island+0x30,'B',seed//2%2)
        if island!=MERGER-0x1c:put(island+0x74,'3I',HEAP+0x7000+(island-IA)*2,0,0x80000010)
    put(WORLD+0x20,'3I',HEAP+0x7500,0,0x80000010)
    put(WORLD+0x250,'I',FILTER)
    put(DISPATCH,'I',TABLES);put(NEXT_DISPATCH,'I',NEXT_TABLES)
    for i in range(96):
        creator=CREATORS+i*16
        put(creator,'2I',VT,i)
        put(TABLES+12+i*4,'I',creator);put(NEXT_TABLES+12+i*4,'I',creator)
    for i in range(1024):
        put(TABLES+0x18c+i*4,'I',HANDLER);put(NEXT_TABLES+0x18c+i*4,'I',NEXT_HANDLER)
    for obj,table in [(SHAPEA,VT+0x40),(SHAPEB,VT+0x40),(MA,VT+0x80),(MB,VT+0x80),(MX,VT+0x80),(MY,VT+0x80),(INFO,VT+0xc0),(FILTER,VT+0x100)]:put(obj,'I',table)
    put(VT+4,'I',CREATE);put(VT+0x40+20,'I',SHAPE);put(VT+0x80+24,'I',MOTION)
    put(VT+0xc0+16,'I',RELEASE);put(VT+0x100+4,'I',FILTER_CALL)
    put(AGENT+8,'I',int(seed%5!=1))
    return bytes(data)


class Run:
    def __init__(self,uc,entry,activate,helpers,seed,mutate,precision,overlap):
        self.uc,self.entry,self.activate,self.helpers=uc,entry,activate,helpers
        self.seed,self.mutate,self.precision=seed,mutate,precision
        self.events=[];self.calls={x:0 for x in helpers.values()}
        self.saved_dispatch=DISPATCH;self.saved_tables=TABLES
        uc.mem_write(HEAP,initial(seed,overlap));uc.mem_write(STOP,b'\xc3'*4096)
        self.hook_id=uc.hook_add(UC_HOOK_CODE,self.hook)
    def words(self,at,n=1):return struct.unpack('<'+'I'*n,self.uc.mem_read(at,n*4))
    def put(self,at,fmt,*values):self.uc.mem_write(at,struct.pack('<'+fmt,*values))
    def done(self,sp,cleanup,result=0xabcdef01):
        for reg,val in [(r.UC_X86_REG_EAX,result),(r.UC_X86_REG_ECX,0xdead1234),(r.UC_X86_REG_EDX,0xbeef5678)]:self.uc.reg_write(reg,val)
        self.uc.reg_write(r.UC_X86_REG_EIP,self.words(sp)[0]);self.uc.reg_write(r.UC_X86_REG_ESP,sp+4+cleanup)
    def hook(self,uc,pc,size,unused):
        if pc in self.helpers:self.calls[self.helpers[pc]]+=1
        if pc not in (self.activate,CREATE,SHAPE,MOTION,FILTER_CALL,RELEASE,HANDLER,NEXT_HANDLER):return
        sp,this=uc.reg_read(r.UC_X86_REG_ESP),uc.reg_read(r.UC_X86_REG_ECX)
        if pc==self.activate:
            island=self.words(sp+4)[0];assert this==WORLD and island in (IA,IB)
            self.events.append(('activate',island));self.put(island+0x29,'B',1)
            if self.mutate:self.put(island+0x30,'B',self.seed//4%2)
            self.done(sp,4)
        elif pc==CREATE:
            assert this==CREATORS+((self.seed%12)*8+self.seed//12%8)*16
            assert self.words(sp+4,3)==(A,B,DISPATCH)
            self.events.append(('create',this,A,B,DISPATCH))
            if self.mutate:self.put(MERGER+0x7c,'I',NEXT_DISPATCH);self.saved_dispatch=NEXT_DISPATCH;self.saved_tables=NEXT_TABLES
            self.done(sp,12,INFO)
        elif pc==SHAPE:
            assert this in (SHAPEA,SHAPEB)
            kind=self.seed%32 if this==SHAPEA else (self.seed//32+self.seed*7)%32
            self.events.append(('shape',this,kind))
            if self.mutate:
                self.put(MERGER+0x7c,'I',DISPATCH)
                self.put(self.saved_dispatch,'I',TABLES)
                index=(self.seed%32)*32+(self.seed//32+self.seed*7)%32
                self.put(self.saved_tables+0x18c+index*4,'I',NEXT_HANDLER)
            self.done(sp,0,kind)
        elif pc in (HANDLER,NEXT_HANDLER):
            args=self.words(sp+4,4);assert args==(A,B,self.saved_dispatch,INFO)
            assert pc==(NEXT_HANDLER if self.mutate else HANDLER)
            self.events.append(('handler',pc,args))
            if self.mutate:self.put(A+32,'I',XA);self.put(B+32,'I',XB)
            self.done(sp,0,0 if self.seed%5==0 else AGENT)
        elif pc==MOTION:
            assert this in (MA,MB,MX,MY)
            kind=[0,1,6,7][(self.seed+(1 if this in (MB,MY) else 0))%4]
            self.events.append(('motion',this,kind))
            if self.mutate and this==MX:
                self.put(B+32,'I',EB)
                self.put(XA+0x40,'B',self.seed//7%2)
            self.done(sp,0,kind)
        elif pc==FILTER_CALL:
            output,a,b=self.words(sp+4,3);assert this==FILTER and (a,b)==(A,B)
            value=[0,1,127,128,255][self.seed%5]
            self.put(output,'B',value);self.events.append(('filter',A,B,value));self.done(sp,12,output)
        else:
            assert this==INFO;self.events.append(('release',INFO));self.done(sp,0)
    def run(self):
        uc,sp=self.uc,STACK+0x8000;uc.mem_write(STACK,b'\xcc'*65536);self.put(sp,'2I',STOP,PAIR)
        saved={r.UC_X86_REG_EBX:0x11223344,r.UC_X86_REG_ESI:0x22334455,r.UC_X86_REG_EDI:0x33445566,r.UC_X86_REG_EBP:0x44556677}
        for reg,val in {**saved,r.UC_X86_REG_ESP:sp,r.UC_X86_REG_ECX:MERGER,r.UC_X86_REG_EAX:0,r.UC_X86_REG_EDX:0,r.UC_X86_REG_EFLAGS:2,r.UC_X86_REG_FPCW:self.precision,r.UC_X86_REG_FPTAG:0xffff,r.UC_X86_REG_FPSW:0,r.UC_X86_REG_MXCSR:0x1f80}.items():uc.reg_write(reg,val)
        try:
            uc.emu_start(self.entry,STOP,count=100000)
            assert uc.reg_read(r.UC_X86_REG_EIP)==STOP and uc.reg_read(r.UC_X86_REG_ESP)==sp+8
            assert all(uc.reg_read(reg)==val for reg,val in saved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW)==self.precision and uc.reg_read(r.UC_X86_REG_MXCSR)==0x1f80
            return bytes(uc.mem_read(HEAP,65536)),tuple(self.events),tuple(self.calls.items())
        finally:uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision',[0x7f,0x27f,0x37f])
@pytest.mark.parametrize('mutate',[False,True])
@pytest.mark.parametrize('overlap',[False,True])
def test_sorted_agent_insertion(solver_images,precision,mutate,overlap):
    retail,compiled,mapping=solver_images
    entries=[0x316dd0,symbol(mapping,'merger_316dd0::function_316dd0')]
    activations=[0x30ccd0,symbol(mapping,'hkWorld::function_30ccd0')]
    names=[(0x316c50,'class_316c50::function_316c50','defer'),(0x30d160,'class_30d160::function_30d160','connect')]
    helpers=[{va:label for va,name,label in names},{symbol(mapping,name):label for va,name,label in names}]
    coverage=set()
    for seed in range(256):
        results=[Run(uc,entry,activate,helper,seed,mutate,precision,overlap).run() for uc,entry,activate,helper in zip((retail,compiled),entries,activations,helpers)]
        assert results[0]==results[1],(seed,mutate,precision,overlap)
        coverage.update(event[0] for event in results[0][1])
    assert {'create','shape','handler','motion','filter','release','activate'}<=coverage
