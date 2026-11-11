"""Retail checks for shared world-object records and their bounds caller."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OBJECT, SHAPE, WORLD, TOLERANCE, TABLE, OUTPUT = [HEAP + x for x in (0, 0x400, 0x800, 0xc00, 0xd00, 0xe00)]
BOUNDS = STOP + 0x10


def symbol(mapping, name):
    found = {s.va for s in mapping.find(name)
             if name != 'class_30b340::class_30b340' or 'PAVreferenced_2d88a0@@J@Z' in s.name}
    assert len(found) == 1, (name, found)
    return found.pop()


def execute(uc, ctor, bounds, table, seed, precision):
    rng = random.Random(seed)
    data = bytearray(rng.getrandbits(8) for _ in range(65536))
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    reference = [0, SHAPE, OBJECT][seed % 3]
    put(OBJECT + 4, '2H', rng.randrange(65536), rng.randrange(65536))
    put(SHAPE + 4, '2H', rng.randrange(65536), [0, 1, 2, 0x7fff, 0xfffe, 0xffff][seed // 3 % 6])
    value = [0, 1, 0xffffffff, 0x80000000, rng.getrandbits(32)][seed % 5]
    uc.mem_write(HEAP, bytes(data))
    uc.mem_write(STOP, b'\xc3' * 4096)
    events = []
    def words(at, n=1):
        return struct.unpack('<' + 'I' * n, uc.mem_read(at, n * 4))
    def hook(machine, pc, size, unused):
        if pc != BOUNDS:
            return
        sp, this = machine.reg_read(r.UC_X86_REG_ESP), machine.reg_read(r.UC_X86_REG_ECX)
        transform, tolerance, output = words(sp + 4, 3)
        assert this == SHAPE and transform == OBJECT + 0x80 and output == OUTPUT
        assert tolerance == words(TOLERANCE + 8)[0]
        payload = bytes(machine.mem_read(transform, 32))
        events.append((this, transform, tolerance, output, payload))
        machine.mem_write(output, payload)
        machine.reg_write(r.UC_X86_REG_EAX, 0xabcdef01)
        machine.reg_write(r.UC_X86_REG_ECX, 0x12345678)
        machine.reg_write(r.UC_X86_REG_EDX, 0x23456789)
        machine.reg_write(r.UC_X86_REG_EIP, words(sp)[0])
        machine.reg_write(r.UC_X86_REG_ESP, sp + 16)
    hook_id = uc.hook_add(UC_HOOK_CODE, hook)
    def call(entry, args):
        sp = STACK + 0x8000 + (seed % 4) * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        uc.mem_write(sp, struct.pack('<' + 'I' * (1 + len(args)), STOP, *args))
        kept = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for register, val in kept.items():
            uc.reg_write(register, val)
        for register, val in [(r.UC_X86_REG_ESP, sp), (r.UC_X86_REG_ECX, OBJECT),
                              (r.UC_X86_REG_EAX, 0), (r.UC_X86_REG_EDX, 0),
                              (r.UC_X86_REG_EFLAGS, 2), (r.UC_X86_REG_FPCW, precision),
                              (r.UC_X86_REG_FPTAG, 0xffff), (r.UC_X86_REG_FPSW, 0),
                              (r.UC_X86_REG_MXCSR, 0x1f80)]:
            uc.reg_write(register, val)
        uc.emu_start(entry, STOP, count=10000)
        assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
        assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 4 + 4 * len(args)
        assert all(uc.reg_read(register) == val for register, val in kept.items())
        assert uc.reg_read(r.UC_X86_REG_FPCW) == precision
        return uc.reg_read(r.UC_X86_REG_EAX)
    try:
        assert call(ctor, (reference, value)) == OBJECT
        assert words(OBJECT)[0] == table
        constructed = bytearray(uc.mem_read(HEAP, 65536))
        struct.pack_into('<I', constructed, 0, 0)
        assert constructed[4:6] == data[4:6]
        assert words(OBJECT + 8, 13) == (0, reference, 0xffffffff, 0, 0, 0, 0, value,
                                       0, OBJECT, 0, 0, 0x80000000)
        expected_count = 2 if reference == OBJECT else 1
        assert struct.unpack('<H', uc.mem_read(OBJECT + 6, 2))[0] == expected_count
        if reference == SHAPE:
            old = struct.unpack_from('<H', data, SHAPE - HEAP + 6)[0]
            assert struct.unpack('<H', uc.mem_read(SHAPE + 6, 2))[0] == (old + 1) & 0xffff
        # Run an existing exact descendant caller through the newly shared shape field.
        for at, val in [(OBJECT + 8, WORLD), (OBJECT + 12, SHAPE), (SHAPE, TABLE),
                        (TABLE + 24, BOUNDS), (WORLD + 0xcc, TOLERANCE)]:
            uc.mem_write(at, struct.pack('<I', val))
        call(bounds, (OUTPUT,))
        final = bytearray(uc.mem_read(HEAP, 65536))
        struct.pack_into('<I', final, 0, 0)
        assert len(events) == 1
        return bytes(constructed), bytes(final), tuple(events)
    finally:
        uc.hook_del(hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_world_object_constructor_and_bounds(solver_images, precision):
    retail, compiled, mapping = solver_images
    ctor = symbol(mapping, 'class_30b340::class_30b340')
    bounds = symbol(mapping, 'placed_shape_312800::function_312760')
    tables = {s.va for s in mapping.symbols if s.name.startswith('??_7class_30b340@@')}
    assert len(tables) == 1
    for seed in range(128):
        actual = execute(compiled, ctor, bounds, next(iter(tables)), seed, precision)
        expected = execute(retail, 0x30b340, 0x312760, 0x413790, seed, precision)
        assert actual == expected, (seed, precision)
