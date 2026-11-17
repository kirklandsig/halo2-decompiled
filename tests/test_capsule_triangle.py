"""Differential capsule/triangle queries using independent synthetic geometry.

The geometry helpers execute normally. Only collector/owner callbacks and
array growth are synthetic; timing is disabled through the existing cursor.
Requires the local retail executable and a checker-built image, not checked-in
machine code. Use -p no:faulthandler for Unicorn on Windows.
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
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW,
    UC_X86_REG_FPTAG, UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from test_solver_rows import solver_images, HEAP, STACK, STOP

A, B, SHAPE_A, SHAPE_B = [HEAP + n for n in (0x100, 0x140, 0x200, 0x300)]
POSE_A, POSE_B, INPUT, AGENT = [HEAP + n for n in (0x400, 0x500, 0x600, 0x700)]
COLLECTOR, OWNER, TABLE, ARRAY, ITEMS = [HEAP+n for n in (0x800, 0x840, 0x880, 0x900, 0x1000)]
CALLBACK = STOP + 0x100
TARGETS = [0x2f4440, 0x2f4f00, 0x2f5960, 0x2f63d0, 0x2f6db0]
MEMBERS = {0x2f4440, 0x2f4f00, 0x2f63d0}
SAVED = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]


def inputs(seed):
    rng = random.Random(0x2f4f00 + seed)
    data = bytearray(65536)
    def put(address, fmt, *values):
        struct.pack_into('<'+fmt, data, address-HEAP, *values)
    def vec(address, xyz):
        put(address, '4f', *xyz, 0)
    triangle = [(-2, -1, 0), (2, -1, 0), (0, 2, 0)]
    ends = [(-0.5, 0, 0.1), (0.5, 0, 0.1)]
    mode = seed % 16
    if mode == 1: ends = [(0, 0, -1), (0, 0, 1)]
    elif mode == 2: ends = [(-3, -1, 0.1), (3, -1, 0.1)]
    elif mode == 3: ends = [(3, 3, 0), (4, 4, 0)]
    elif mode == 4: ends = [(-0.5, 0, 6), (0.5, 0, 6)]
    elif mode == 5: ends = [(0, 0, -0.1), (0, 0, -0.1)]
    elif mode == 6: ends = [(-3, 0, -0.5), (3, 0, 0.5)]
    elif mode == 7: ends = [(-2, -1, -0.0), (0, 2, 0.0)]
    elif mode == 8: triangle = [(0, 0, 0)] * 3
    elif mode == 9: triangle = [(-2, 0, 0), (0, 0, 0), (2, 0, 0)]
    elif mode == 10: ends = [(math.nan, 0, 0), (0, 0, math.inf)]
    elif mode == 11: triangle = [(math.nan, 0, 0), (1, 0, 0), (0, 1, 0)]
    elif mode >= 12: ends = [tuple(rng.uniform(-3, 3) for _ in range(3)) for _ in range(2)]
    if seed & 16: triangle = triangle[::-1]
    radius = [0, 0.15, 0.5, 1.0][seed // 16 % 4]
    put(SHAPE_A + 12, 'f', radius)
    put(SHAPE_B + 12, 'f', radius / 3)
    for i, v in enumerate(ends): vec(SHAPE_A + 16 + 16*i, v)
    for i, v in enumerate(triangle): vec(SHAPE_B + 16 + 16*i, v)
    # Equal rotations and translations preserve the analytic cases; the last
    # four modes also exercise independent rigid transformations.
    for index, pose in enumerate((POSE_A, POSE_B)):
        angle = (seed // 32) * 0.37 + (index * 0.51 if mode >= 12 else 0)
        c, s = math.cos(angle), math.sin(angle)
        for i, v in enumerate(((c,s,0),(-s,c,0),(0,0,1))): vec(pose+32+16*i, v)
        vec(pose+80, (0.25, -0.5, 0.125) if seed & 64 else (0,0,0))
    put(A, '4I', SHAPE_A, 3, POSE_A, 0)
    put(B, '4I', SHAPE_B, 7, POSE_B, 0)
    if seed % 31 == 0:
        # Shared read-only shape/pose storage is valid even when the roles
        # use different numbers of vertices.
        put(A, '4I', SHAPE_B, 3, POSE_B, 0)
    put(INPUT + 8, 'f', [-0.1, 0, 0.05, 0.5][seed // 4 % 4])
    put(AGENT + 8, 'I', OWNER)
    put(AGENT + 12, '3H', *([0xffff] * 3 if seed % 3 else [10, 11, 12]))
    put(OWNER, 'I', TABLE + 16)
    put(TABLE, '3I', 0, CALLBACK, CALLBACK + 16)
    put(TABLE + 16, '3I', 0, CALLBACK + 32, CALLBACK + 48)
    put(COLLECTOR, 'I', TABLE)
    put(ARRAY, '3I', ITEMS, 0, 0x80000000 | (1 if seed & 1 else 8))
    data[ITEMS-HEAP:ITEMS-HEAP+0x400] = b'\xcd' * 0x400
    return data


def invoke(uc, address, args, receiver, precision):
    sp = STACK + 0x8000
    uc.mem_write(STACK, b'\xa7' * 65536)
    uc.mem_write(sp, struct.pack('<'+'I'*(len(args)+1), STOP, *args))
    for i, reg in enumerate(SAVED): uc.reg_write(reg, 0xabc000+i)
    uc.reg_write(UC_X86_REG_ECX, receiver)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    uc.emu_start(address, STOP, count=300000)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 + (len(args)*4 if receiver else 0)
    assert [uc.reg_read(r) for r in SAVED] == [0xabc000+i for i in range(4)]
    assert uc.reg_read(UC_X86_REG_FPCW) == precision


def run(uc, addresses, va, seed, precision):
    uc.mem_write(HEAP, bytes(inputs(seed)))
    for g in addresses['timer']: uc.mem_write(g, b'\0' * 4)
    invoke(uc, addresses['measures'], [SHAPE_B+16, AGENT+20], 0, precision)
    events = []
    helpers = set()
    def words(p, n): return struct.unpack('<'+'I'*n, uc.mem_read(p, 4*n))
    def finish(pop, result=0):
        sp = uc.reg_read(UC_X86_REG_ESP)
        destination = words(sp, 1)[0]
        uc.reg_write(UC_X86_REG_EAX, result)
        uc.reg_write(UC_X86_REG_ECX, 0x12345678)
        uc.reg_write(UC_X86_REG_EDX, 0x87654321)
        uc.reg_write(UC_X86_REG_ESP, sp+4+pop)
        uc.reg_write(UC_X86_REG_EIP, destination)
    def callback(machine, pc, size, user):
        sp = uc.reg_read(UC_X86_REG_ESP)
        if pc in (addresses['segments'], addresses['capsule_edge']):
            helpers.add('segments' if pc == addresses['segments'] else 'capsule_edge')
        elif pc == addresses['grow']:
            array, element_size = words(sp+4, 2)
            assert array == ARRAY and element_size == 48
            pointer, count, cap = words(array, 3)
            events.append(('grow', count, cap & 0x7fffffff))
            uc.mem_write(array+8, struct.pack('<I', 0x80000000 | max(1,(cap & 0x7fffffff)*2)))
            finish(0)
        elif pc == CALLBACK and va in (0x2f63d0, 0x2f6db0):
            assert uc.reg_read(UC_X86_REG_ECX) == COLLECTOR
            assert words(sp+4, 2) == (A, B)
            events.append(('pair', A, B)); finish(8)
        elif pc == CALLBACK:
            assert uc.reg_read(UC_X86_REG_ECX) == COLLECTOR
            point = words(sp+4, 1)[0]
            events.append(('contact', bytes(uc.mem_read(point, 40)))); finish(4)
        elif pc == CALLBACK+32:
            assert uc.reg_read(UC_X86_REG_ECX) == OWNER
            a, b, inp, point = words(sp+4, 4)
            assert (a,b,inp) == (A,B,INPUT)
            events.append(('add', bytes(uc.mem_read(point, 32))))
            finish(16, 0xffff if seed % 7 == 0 else 20+len(events))
        elif pc == CALLBACK+48:
            assert uc.reg_read(UC_X86_REG_ECX) == OWNER
            events.append(('release', words(sp+4,1)[0])); finish(4)
    hooks = [uc.hook_add(UC_HOOK_CODE, callback, begin=CALLBACK, end=CALLBACK+64),
             *[uc.hook_add(UC_HOOK_CODE, callback, begin=addresses[name], end=addresses[name])
               for name in ('grow','segments','capsule_edge')]]
    try:
        if va == 0x2f4440: uc.mem_write(COLLECTOR, struct.pack('<I', ARRAY))
        invoke(uc, addresses[va], [A,B,INPUT,COLLECTOR], AGENT if va in MEMBERS else 0, precision)
    finally:
        for hook in hooks: uc.hook_del(hook)
    return bytes(uc.mem_read(HEAP,65536)), events, helpers


@pytest.mark.parametrize('va', TARGETS, ids=hex)
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_capsule_triangle_queries_match_retail(solver_images, va, precision):
    retail, compiled, symbols = solver_images
    def symbol(name):
        hits = symbols.find(name)
        assert len(hits) == 1, name
        return hits[0].va
    ra = {v:v for v in TARGETS}
    ra.update(measures=0x308a50, grow=0x2d91d0, segments=0x308590,
              capsule_edge=0x2f28f0, timer=[0x480154,0x480158])
    ca = {v:symbol(('class_'+format(v,'x')+'::' if v in MEMBERS else '')+
                   'function_'+format(v,'x')) for v in TARGETS}
    ca.update(measures=symbol('function_308a50'), grow=symbol('function_2d91d0'),
              segments=symbol('function_308590'), capsule_edge=symbol('function_2f28f0'),
              timer=[symbol('g_480154'),symbol('g_480158')])
    kinds = set(); helper_paths = set(); empty = False
    for seed in range(128):
        expected = run(retail,ra,va,seed,precision)
        actual = run(compiled,ca,va,seed,precision)
        assert expected == actual, (hex(va),seed,hex(precision),expected[1],actual[1])
        kinds.update(event[0] for event in expected[1])
        helper_paths.update(expected[2])
        empty |= not expected[1]
    assert empty
    assert 'segments' in helper_paths
    if va == 0x2f4440:
        assert {'add','release','grow'} <= kinds
        assert 'capsule_edge' in helper_paths
    elif va in (0x2f63d0,0x2f6db0): assert 'pair' in kinds
    else: assert 'contact' in kinds
