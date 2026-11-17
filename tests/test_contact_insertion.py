"""Optional retail comparisons for contact insertion and notification.

Requires Unicorn, the local retail XBE and a checker image. Array allocation,
world registration, motion queries and listeners use deterministic hooks;
insertion, shifting, ID allocation and the contact calculations execute normally.
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

MANAGER, WORLD, OWNER = HEAP + 0x1000, HEAP + 0x2000, HEAP + 0x2100
CONSTRAINT = MANAGER + 0x10
BODIES = HEAP + 0x3000, HEAP + 0x3100
ENTITIES = HEAP + 0x3600, HEAP + 0x3700
MOTIONS = HEAP + 0x4000, HEAP + 0x4100
MOTION_TABLE, OWNER_TABLE, MANAGER_TABLE = HEAP + 0x4200, HEAP + 0x4300, HEAP + 0x4400
INPUT, POINT, OUTPUT = HEAP + 0x4500, HEAP + 0x4600, HEAP + 0x4700
IDS, POINTS, PROPERTIES = HEAP + 0x4e00, HEAP + 0x5000, HEAP + 0x7000
VELOCITY, ADD_SIZES, REMOVE = STOP + 0x100, STOP + 0x110, STOP + 0x120
NAMES = {
    0x31be80: 'lane_b_contact_constraint_prefix::add',
    0x318c10: 'class_318e60::add',
    0x2d9160: 'function_2d9160', 0x2d91d0: 'function_2d91d0',
    0x30d2b0: 's_1d1541::function_30d2b0',
    0x318320: 'function_318320', 0x315d20: 'function_315d20',
}


def put(data, address, fmt, *values):
    struct.pack_into('<' + fmt, data, address - HEAP, *values)


def words(uc, address, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(address, count * 4))


def contact_case(seed):
    rng = random.Random(seed)
    data = bytearray(b'\xcd' * 65536)
    count = [0, 1, 2, 3, 7, 250, 251][seed % 7]
    property_count = count + [0, 1, 3][seed % 3]
    flag = (seed & 1) << 31
    put(data, MANAGER, 'I', MANAGER_TABLE)
    put(data, MANAGER_TABLE + 8, 'I', REMOVE)
    put(data, MANAGER + 8, 'I', WORLD)
    put(data, OWNER, 'I', OWNER_TABLE)
    put(data, OWNER_TABLE + 8, 'I', ADD_SIZES)
    put(data, CONSTRAINT + 0x14, 'I', OWNER)
    put(data, CONSTRAINT + 0x34, '3I', POINTS, count, flag | (count + seed % 2))
    put(data, CONSTRAINT + 0x90, '3I', PROPERTIES, property_count,
        flag | (property_count + seed // 2 % 2))
    put(data, CONSTRAINT + 0x50, 'I', seed << 4)
    id_count = count + 2
    put(data, CONSTRAINT + 0x20, '3I', IDS, id_count, flag | (id_count + seed % 2))
    for i in range(id_count):
        put(data, IDS + i, 'B', 0xff if seed % 3 and i % 5 == 0 else i % 250)
    data[POINTS - HEAP:POINTS - HEAP + count * 32] = rng.randbytes(count * 32)
    data[PROPERTIES - HEAP:PROPERTIES - HEAP + property_count * 28] = rng.randbytes(property_count * 28)
    for i in range(property_count):
        put(data, PROPERTIES + 28 * i + 24, 'H', (i + seed) % 3)
    put(data, MOTION_TABLE + 0x58, 'I', VELOCITY)
    for i in range(2):
        body = BODIES[i]
        for depth in range(seed % 3 + 1):
            parent = body + 0x200 if depth < seed % 3 else 0
            put(data, body + 0xc, 'I', parent)
            put(data, body + 0x20, 'I', ENTITIES[i])
            body = parent
        put(data, ENTITIES[i] + 0x3c, 'I', MOTIONS[i])
        put(data, ENTITIES[i] + 0x4c, '2f', rng.uniform(0.05, 1), rng.uniform(0.05, 1))
        put(data, ENTITIES[i] + 0x64, 'I', (seed >> i) & 1)
        put(data, MOTIONS[i], 'I', MOTION_TABLE)
        put(data, MOTIONS[i] + 0x10, '4f', *(rng.uniform(-3, 3) for _ in range(4)))
        put(data, MOTIONS[i] + 0x2c, 'f', rng.uniform(0.1, 2))
    put(data, POINT, '8f', *(rng.uniform(-1, 1) for _ in range(8)))
    put(data, INPUT + 0x30, 'f', 1 / 60)
    if seed % 17 == 0:
        special = [0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc12345][seed // 17 % 5]
        put(data, POINT + 16, 'I', special)
    if seed % 23 == 0:
        for motion in MOTIONS:
            put(data, motion + 0x2c, 'f', 0)
    # A motion query may change a child body's parent before material lookup.
    put(data, HEAP + 0x3a0c, 'I', 0)
    put(data, HEAP + 0x3a20, 'I', HEAP + 0x3b00)
    put(data, HEAP + 0x3b4c, '2f', 0.9, 0.7)
    return data


def run_contact(uc, addresses, va, data, seed, precision):
    uc.mem_write(HEAP, bytes(data))
    uc.mem_write(STACK, b'\xcd' * 65536)
    args = [OUTPUT, OUTPUT + 4] if va == 0x31be80 else [*BODIES, INPUT, POINT]
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    uc.reg_write(UC_X86_REG_ECX, CONSTRAINT if va == 0x31be80 else MANAGER)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    events, grows, velocities, sizes, removals, registrations = [], [], [], [], [], []
    remap = {address: retail for retail, address in addresses.items()}

    def grow(array, capacity, width):
        old, length, _ = words(uc, array, 3)
        target = {CONSTRAINT + 0x20: HEAP + 0xd000,
                  CONSTRAINT + 0x34: HEAP + 0x9000,
                  CONSTRAINT + 0x90: HEAP + 0xb000}[array]
        grows.append((array, capacity, width))
        uc.mem_write(target, bytes(uc.mem_read(old, length * width)))
        uc.mem_write(array, struct.pack('<3I', target, length, capacity))

    def hook(machine, address, size, _):
        kind = remap.get(address, address)
        call_sp = machine.reg_read(UC_X86_REG_ESP)
        ret, first, second = words(machine, call_sp, 3)
        cleanup = 4
        if kind == 0x2d9160:
            width = words(machine, call_sp + 12, 1)[0]
            grow(first, second, width)
        elif kind == 0x2d91d0:
            capacity = words(machine, first + 8, 1)[0] & 0x7fffffff
            grow(first, max(1, capacity * 2), second)
        elif kind == ADD_SIZES:
            assert machine.reg_read(UC_X86_REG_ECX) == OWNER and first == CONSTRAINT
            sizes.append(words(machine, second, 4))
            cleanup = 12
        elif kind == 0x30d2b0:
            assert machine.reg_read(UC_X86_REG_ECX) == WORLD and first == CONSTRAINT
            registrations.append(first)
            machine.reg_write(UC_X86_REG_EAX, first)
            cleanup = 8
        elif kind == VELOCITY:
            motion = machine.reg_read(UC_X86_REG_ECX)
            assert motion in MOTIONS and first == POINT
            velocities.append(motion)
            machine.mem_write(second, bytes(machine.mem_read(motion + 0x10, 16)))
            if motion == MOTIONS[1] and seed % 13 == 0:
                machine.mem_write(BODIES[0] + 12, struct.pack('<I', HEAP + 0x3a00))
            cleanup = 12
        elif kind in (0x318320, 0x315d20):
            machine.mem_write(second + 0x10, struct.pack('<I', 0 if kind == 0x318320 else first))
            # Bytes 2..3 are unused padding; the event's stack address is local.
            event = bytes(machine.mem_read(second, 2)) + bytes(machine.mem_read(second + 4, 32))
            events.append((kind, first, event))
            property_address = words(machine, second + 0x18, 1)[0]
            if seed % 4 == 0:
                machine.mem_write(property_address + 12, struct.pack('<f', 0.25))
                machine.mem_write(POINT + 12, struct.pack('<f', 0.125))
                machine.mem_write(MOTIONS[0] + 0x2c, struct.pack('<f', 1.5))
            if seed % 5 == 0:
                machine.mem_write(second + 0x20, struct.pack('<I', 1))
            elif seed % 11 == 0:
                machine.mem_write(second + 0x20, struct.pack('<I', 2))
        elif kind == REMOVE:
            assert machine.reg_read(UC_X86_REG_ECX) == MANAGER
            removals.append(first & 0xffff)
            cleanup = 8
        else:
            raise AssertionError(hex(kind))
        machine.reg_write(UC_X86_REG_ESP, call_sp + cleanup)
        machine.reg_write(UC_X86_REG_EIP, ret)

    hooks = [*list(addresses[v] for v in NAMES if v not in (0x31be80, 0x318c10)),
             VELOCITY, ADD_SIZES, REMOVE]
    handles = [uc.hook_add(UC_HOOK_CODE, hook, begin=entry, end=entry) for entry in hooks]
    try:
        uc.emu_start(addresses[va], STOP, count=100000)
    finally:
        for handle in handles:
            uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 + 4 * len(args)
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    result = uc.reg_read(UC_X86_REG_EAX) & (0xffff if va == 0x318c10 else 0xffffffff)
    return bytes(uc.mem_read(HEAP, 65536)), (result, events, grows, velocities, sizes, removals, registrations)


@pytest.mark.parametrize('va', [0x31be80, 0x318c10], ids=hex)
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_contact_insertion_against_retail(solver_images, va, precision):
    retail, compiled, symbols = solver_images
    addresses = {}
    for original, name in NAMES.items():
        hits = {symbol.va for symbol in symbols.find(name)}
        assert len(hits) == 1, ('rebuild the checker image', name, hits)
        addresses[original] = hits.pop()
    for seed in range(128):
        data = contact_case(seed)
        expected, expected_calls = run_contact(retail, {v: v for v in NAMES}, va, data, seed, precision)
        actual, actual_calls = run_contact(compiled, addresses, va, data, seed, precision)
        assert actual_calls == expected_calls, (hex(va), seed, precision, 'calls')
        allowed = set()
        if va == 0x318c10 and expected_calls[3]:
            index, = struct.unpack_from('<I', data, CONSTRAINT - HEAP + 0x38)
            properties, = struct.unpack_from('<I', expected, CONSTRAINT - HEAP + 0x90)
            allowed = {properties - HEAP + index * 28 + field for field in (0, 4, 8, 12, 20)}
        for offset in range(0, len(data), 4):
            rb, cb = expected[offset:offset + 4], actual[offset:offset + 4]
            if rb == cb:
                continue
            rf, cf = struct.unpack('<f', rb)[0], struct.unpack('<f', cb)[0]
            assert offset in allowed and (math.isclose(rf, cf, rel_tol=2e-6, abs_tol=2e-6)
                                          or math.isnan(rf) and math.isnan(cf)), (
                hex(va), seed, precision, hex(offset), rf, cf)
