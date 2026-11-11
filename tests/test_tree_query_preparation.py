"""Retail differential tests of plane/sphere preparation and tree traversal.

Inputs are synthetic command streams and geometry. The preparation tests
intercept only the walker entry; traversal tests execute both walkers and
their offset collectors normally. No retail executable bytes are stored here.
"""
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

TREE, QUERY, PLANES = HEAP + 0x100, HEAP + 0x800, HEAP + 0x1000
SECOND, FIRST = HEAP + 0x2000, HEAP + 0x2020


def address(symbols, name):
    found = {s.va for s in symbols.find(name)}
    assert len(found) == 1, (name, found)
    return found.pop()


def put(data, va, fmt, *values):
    struct.pack_into('<' + fmt, data, va - HEAP, *values)


def words(uc, va, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, count * 4))


def inputs(kind, seed, traversal):
    rng = random.Random(seed)
    data = bytearray(rng.randbytes(65536))
    origin = [rng.uniform(-3, 3) for _ in range(3)] + [rng.choice([0.25, 1, 3, 65536])]
    if not traversal:
        origin[3] = [0.0, -0.0, 0.5, -2, 3, float('inf'), float('nan')][seed % 7]
    put(data, TREE, '4f', *origin)
    # Generous fixed backing storage keeps allocation outside this test's scope.
    for array, storage in [(SECOND, HEAP + 0x3000), (FIRST, HEAP + 0x4000)]:
        put(data, array, 'III', storage, seed % 3, 0x80000100)
    count = seed % 33
    if kind == 'plane':
        put(data, QUERY, 'iI', count, PLANES)
        for index in range(32):
            values = [rng.uniform(-2, 2) for _ in range(4)]
            if traversal:
                # Fully inside, outside and straddling the same small box.
                values = [1, 0, 0, [-1e9, 1e9, -1][seed % 3]]
            elif seed % 11 == 0:
                values[index % 4] = [float('nan'), float('inf'), -0.0][index % 3]
            put(data, PLANES + 16 * index, '4f', *values)
    else:
        values = [rng.uniform(-10, 10) for _ in range(3)] + [rng.uniform(-2, 20)]
        if traversal:
            values = [[0, 0, 0, 1e9], [1e9, 0, 0, 1], [0, 0, 0, 1]][seed % 3]
        elif seed % 11 == 0:
            values[seed % 4] = [float('nan'), float('inf'), -0.0][seed % 3]
        put(data, QUERY, '4f', *values)
    streams = [
        b'\x00', b'\x37', b'\x50\xa1', b'\x51\x01\x02',
        b'\x09\x17\x60\x12\x52\x00\x01\x02',
        b'\x0a\x01\x00\x64\x01\x02\x53\x00\x00\x01\x04',
        b'\x01\x01\x02\x03\x0b\x00\x00\x01\x00\x39',
        b'\x05\x02\xff\xff\x3a',
        b'\x29\x00\x00\x00\x00\x00\x02\x2a\x00\x00\x00\x00\x00\x02\x2b\x00\x00\x00\x00\x00\x02\x3b',
        b'\x26\x00\x02\x27\x00\x02\x28\x00\x02\x3c',
        b'\x10\x01\x01\x01\x3d\x3e',
        b'\x13\x00\x00\x01\x3d\x3e',
    ]
    stream = streams[(seed // 3) % len(streams)] if traversal else b'\x00'
    data[TREE - HEAP + 0x30:TREE - HEAP + 0x30 + len(stream)] = stream
    return data, count


def execute(uc, entry, walker, data, kind, precision, traversal, alias):
    uc.mem_write(HEAP, bytes(data))
    uc.mem_write(STACK, b'\xcc' * 65536)
    sp = STACK + 0x8000
    query = TREE if alias and kind == 'sphere' else QUERY
    uc.mem_write(sp, struct.pack('<5I', STOP, TREE, query, SECOND, FIRST))
    preserved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in preserved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    for reg, value in [(UC_X86_REG_ESP, sp), (UC_X86_REG_EFLAGS, 2),
                       (UC_X86_REG_FPCW, precision), (UC_X86_REG_FPTAG, 0xffff),
                       (UC_X86_REG_FPSW, 0), (UC_X86_REG_MXCSR, 0x1f80)]:
        uc.reg_write(reg, value)
    snapshots = []

    def capture(machine, va, size, user):
        if snapshots:
            return  # Recursion belongs to the real walk, not preparation.
        base = machine.reg_read(UC_X86_REG_ECX)
        stack = machine.reg_read(UC_X86_REG_ESP)
        ret, pending, root, fixed = words(machine, stack, 4)
        assert root == TREE + 0x30
        read = lambda offset, length: bytes(machine.mem_read(base + offset, length))
        fields = [read(0, 4), read(0x10, 4), read(0x20, 4),
                  bytes(machine.mem_read(pending, 24)), bytes(machine.mem_read(fixed, 32))]
        if kind == 'plane':
            groups, = words(machine, base + 0x30, 1)
            assert groups <= 8
            fields += [groups, read(0x40, groups * 64), read(0x240, groups * 64)]
        else:
            assert fixed == base + 0x40
            fields += [read(0x30, 16)]
        snapshots.append(tuple(fields))
        if not traversal:
            machine.reg_write(UC_X86_REG_EAX, 0xcafebabe)
            machine.reg_write(UC_X86_REG_ECX, 0xbadbad01)
            machine.reg_write(UC_X86_REG_EDX, 0xbadbad02)
            machine.reg_write(UC_X86_REG_ESP, stack + 16)
            machine.reg_write(UC_X86_REG_EIP, ret)

    handle = uc.hook_add(UC_HOOK_CODE, capture, begin=walker, end=walker)
    try:
        uc.emu_start(entry, STOP, count=100000)
    finally:
        uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4
    assert all(uc.reg_read(reg) == 0x12345678 for reg in preserved)
    assert uc.reg_read(UC_X86_REG_FPCW) == precision
    assert len(snapshots) == 1
    return bytes(uc.mem_read(HEAP, 65536)), snapshots[0]


@pytest.mark.parametrize('kind,entry,walker', [
    ('plane', 0x2dd930, 0x2dc220), ('sphere', 0x2ddbd0, 0x2dce60),
])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('traversal', [False, True], ids=['prepared-state', 'real-traversal'])
@pytest.mark.parametrize('alias', [False, True], ids=['separate', 'read-only-alias'])
def test_tree_queries_against_retail(solver_images, kind, entry, walker, precision, traversal, alias):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'function_%x' % entry)
    walk_target = address(symbols, 'class_%x::function_%x' % (walker, walker))
    outcomes = set()
    groups_seen = set()
    for seed in range(132):
        data, count = inputs(kind, seed, traversal)
        if alias and kind == 'plane':
            # First output and second output may be the same accumulator. Plane
            # input may also alias the tree origin in the preparation-only test.
            put(data, FIRST, 'I', HEAP + 0x3000)
            if not traversal:
                put(data, QUERY, 'iI', count, TREE)
        actual = execute(compiled, target, walk_target, data, kind, precision, traversal, alias)
        expected = execute(retail, entry, walker, data, kind, precision, traversal, alias)
        assert actual == expected, (kind, precision, traversal, seed)
        if kind == 'plane':
            assert actual[1][5] == (count + 3) // 4
        groups_seen.add((count + 3) // 4)
        outcomes.add(tuple(struct.unpack_from('<I', actual[0], array - HEAP + 4)[0]
                           - seed % 3 for array in (FIRST, SECOND)))
    assert groups_seen == set(range(9))
    if traversal:
        assert (0, 0) in outcomes and any(a or b for a, b in outcomes)
