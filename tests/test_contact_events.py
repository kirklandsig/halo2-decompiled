"""Differential execution of contact callbacks, with synthetic motion/listeners.

Requires the optional Unicorn dependency and a full checker image. Listener
dispatch is intercepted at its existing function boundary; the callbacks under
test execute normally. No retail executable bytes are stored here.
"""
import os
import random
import struct
from pathlib import Path

import pytest

pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from xbe import Xbe, ROOT, retail_xbe_path
from pe import Pe
from linkmap import LinkMap
from test_solver_rows import machine, HEAP, STACK, STOP

MANAGER, WORLD = HEAP + 0x1000, HEAP + 0x2000
BODIES = (HEAP + 0x3000, HEAP + 0x3100)
ENTITIES = (HEAP + 0x4000, HEAP + 0x4100)
MOTIONS = (HEAP + 0x5000, HEAP + 0x5100)
VTABLE, POINT = HEAP + 0x5200, HEAP + 0x5300
IDS, POINTS, RESULTS = HEAP + 0x6000, HEAP + 0x6100, HEAP + 0x6800
OUTPUT, INPUT = HEAP + 0x7000, HEAP + 0x7100
VELOCITY_CALL = STOP + 0x100


@pytest.fixture(scope='module')
def contact_images():
    retail = Path(retail_xbe_path())
    compiled = Path(os.environ.get('HALO2_SOLVER_TEST_IMAGE', str(Path(ROOT) / 'build/halo2.exe')))
    if not retail.exists() or not compiled.exists() or not compiled.with_suffix('.map').exists():
        pytest.skip('requires retail XBE and a full checker build')
    symbols = LinkMap.read(compiled.with_suffix('.map'))
    names = {
        0x318a90: 'lane_a_simple_contact_manager::add',
        0x318fb0: 'class_318e60::processed',
        0x318320: 'function_318320', 0x315d20: 'function_315d20',
        0x318440: 'function_318440', 0x315e00: 'function_315e00',
    }
    addresses = {}
    for va, name in names.items():
        hits = symbols.find(name)
        assert len(hits) == 1, 'rebuild the image with the contact sources: ' + name
        addresses[va] = hits[0].va
    return machine(Xbe(retail)), machine(Pe(compiled)), addresses


def contact_case(va, seed):
    rng = random.Random(seed)
    data = bytearray(65536)

    def put(address, fmt, *values):
        struct.pack_into('<' + fmt, data, address - HEAP, *values)

    def vector(address):
        put(address, '4f', *(rng.uniform(-10, 10) for _ in range(4)))

    put(MANAGER + 8, 'I', WORLD)
    put(VTABLE + 0x58, 'I', VELOCITY_CALL)
    for i in range(2):
        put(BODIES[i] + 0x20, 'I', ENTITIES[i])
        put(ENTITIES[i] + 0x3c, 'I', MOTIONS[i])
        put(ENTITIES[i] + 0x5c, 'H', [0, 1, 7, 65535][(seed + i) % 4])
        put(ENTITIES[i] + 0x64, 'I', (seed >> i) & 1)
        put(MOTIONS[i], 'I', VTABLE)
        vector(MOTIONS[i] + 0x10)
    vector(POINT)
    vector(POINT + 0x10)
    # Exercise signed zero, infinities and NaNs as well as ordinary inputs.
    if seed % 8 == 0:
        special = [0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345][seed // 8 % 5]
        put(MOTIONS[0] + 0x10, 'I', special)
        put(POINT + 0x10, 'I', special)
    if va == 0x318a90:
        put(MANAGER + 0xc, 'II', *ENTITIES)
        args = [*BODIES, INPUT, POINT]
    else:
        put(MANAGER + 0xc, 'i', [0, 1, 2, -1][seed // 4 % 4])
        put(MANAGER + 0x30, 'I', IDS)
        put(MANAGER + 0x44, 'I', POINTS)
        count = [0, 1, 2, 7][seed % 4]
        put(OUTPUT, 'II', RESULTS, count)
        put(INPUT, 'I', OUTPUT)
        for i in range(8):
            put(IDS + i, 'B', (7 - i) if seed & 1 else i % 3)
            vector(POINTS + i * 32)
            vector(POINTS + i * 32 + 16)
            vector(RESULTS + i * 48)
            vector(RESULTS + i * 48 + 16)
            put(RESULTS + i * 48 + 32, 'H', i)
        args = [*BODIES, POINT, INPUT]
    return data, args


def run_contact(uc, entry, dispatch, va, data, args):
    uc.mem_write(HEAP, bytes(data))
    uc.mem_write(STACK, b'\xcd' * 65536)
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<5I', STOP, *args))
    preserved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in preserved + [UC_X86_REG_EAX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    uc.reg_write(UC_X86_REG_ECX, MANAGER)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, 0x37f)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    events = []
    velocities = []

    def hook(uc, address, size, _):
        if address != VELOCITY_CALL and address not in dispatch:
            return
        call_sp = uc.reg_read(UC_X86_REG_ESP)
        ret, first, second = struct.unpack('<3I', uc.mem_read(call_sp, 12))
        if address == VELOCITY_CALL:
            motion = uc.reg_read(UC_X86_REG_ECX)
            assert motion in MOTIONS and first == POINT
            velocities.append(motion)
            uc.mem_write(second, bytes(uc.mem_read(motion + 0x10, 16)))
            cleanup = 12
        else:
            kind = dispatch[address]
            world = 0 if kind == 'world' else first
            uc.mem_write(second + 0x10, struct.pack('<I', world))
            if va == 0x318a90:
                point = struct.unpack('<I', uc.mem_read(second + 0x14, 4))[0]
                event = (bytes(uc.mem_read(second, 20)), bytes(uc.mem_read(point, 32)),
                         bytes(uc.mem_read(second + 0x18, 8)))
                # Later listeners must observe changes to the temporary event
                # and copied point, without changing the caller's point.
                uc.mem_write(point, struct.pack('<f', len(events) + 10.0))
                uc.mem_write(second + 0x1c, struct.pack('<f', len(events) + 20.0))
            else:
                event = bytes(uc.mem_read(second, 20))
            events.append((kind, first, event))
            cleanup = 4
        uc.reg_write(UC_X86_REG_ESP, call_sp + cleanup)
        uc.reg_write(UC_X86_REG_EIP, ret)

    handle = uc.hook_add(UC_HOOK_CODE, hook)
    try:
        uc.emu_start(entry, STOP, count=10000)
    finally:
        uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 20
    assert all(uc.reg_read(reg) == 0x12345678 for reg in preserved)
    result = uc.reg_read(UC_X86_REG_EAX) & 0xffff if va == 0x318a90 else None
    return bytes(uc.mem_read(HEAP, 65536)), events, velocities, result


@pytest.mark.parametrize('va', [0x318a90, 0x318fb0], ids=hex)
def test_contact_callbacks_agree_with_retail(contact_images, va):
    retail, compiled, addresses = contact_images
    world, entity = (0x318320, 0x315d20) if va == 0x318a90 else (0x318440, 0x315e00)
    for seed in range(128):
        data, args = contact_case(va, seed)
        expected = run_contact(retail, va, {world: 'world', entity: 'entity'}, va, data, args)
        actual = run_contact(compiled, addresses[va],
                             {addresses[world]: 'world', addresses[entity]: 'entity'}, va, data, args)
        assert actual == expected, (hex(va), seed)
        if va == 0x318a90:
            assert actual[2] == list(MOTIONS)
            assert actual[3] == 0
            assert actual[0][POINT - HEAP:POINT - HEAP + 32] == data[POINT - HEAP:POINT - HEAP + 32]
