"""Optional differential execution of joint builders and contact modifiers.

Requires the local retail XBE, a full checker build, and Unicorn. External
owner/allocator callbacks use synthetic responses; joint solver callees run
normally. No retail executable bytes are stored in the tests.
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
from test_solver_rows import (
    HEAP, STACK, STOP, PAIR, OUTPUT, TERMS, OPS, case, solver_images,  # noqa: F401
)

JOINT, ENTITY_A, ENTITY_B = HEAP + 0x4000, HEAP + 0x5000, HEAP + 0x5200
MANAGER, OWNER, TABLE, CALLBACK = HEAP + 0x6000, HEAP + 0x7000, HEAP + 0x7100, STOP + 128


def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, ('rebuild the full checker image', name, hits)
    return hits.pop()


def put(data, va, fmt, *values):
    struct.pack_into('<' + fmt, data, va - HEAP, *values)


def words(uc, va, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, 4 * count))


def return_from_hook(uc, argument_bytes=0):
    sp = uc.reg_read(UC_X86_REG_ESP)
    ret, = words(uc, sp, 1)
    uc.reg_write(UC_X86_REG_ESP, sp + 4 + argument_bytes)
    uc.reg_write(UC_X86_REG_EIP, ret)


def execute(uc, entry, data, args, this=None, hooks=(), precision=0x37f):
    if not any(lo == 0 for lo, hi, permissions in uc.mem_regions()):
        uc.mem_map(0, 4096)
    uc.mem_write(0, b'\xff' * 4)
    uc.mem_write(HEAP, bytes(data))
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    if this is not None:
        uc.reg_write(UC_X86_REG_ECX, this)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    handles = [uc.hook_add(UC_HOOK_CODE, callback, begin=va, end=va) for va, callback in hooks]
    try:
        uc.emu_start(entry, STOP, count=100000)
    finally:
        for handle in handles:
            uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 + (4 * len(args) if this is not None else 0)
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    assert bytes(uc.mem_read(0, 4)) == b'\xff' * 4
    return bytes(uc.mem_read(HEAP, len(data)))


@pytest.mark.parametrize('va,name', [
    (0x314220, 'lane_a_ball_socket::build'),
    (0x3149e0, 'lane_a_spring::build'),
    (0x314fc0, 'lane_a_prismatic::build'),
])
def test_joint_builders_against_retail(solver_images, va, name):
    retail, compiled, symbols = solver_images
    target = address(symbols, name)
    for seed in range(64):
        data, _ = case(0x31f160, seed)
        rng = random.Random(seed)
        put(data, JOINT + 12, 'II', ENTITY_A, ENTITY_B)
        put(data, ENTITY_A + 0x3c, 'I', HEAP + 2048 - 0x80)
        put(data, ENTITY_B + 0x3c, 'I', HEAP + 2304 - 0x80)
        for offset in range(0x20, 0xa0, 16):
            put(data, JOINT + offset, '4f', *(rng.uniform(-2, 2) for _ in range(4)))
        if va == 0x3149e0:
            put(data, JOINT + 0x40, 'f', rng.uniform(0, 10))
        if va == 0x314fc0:
            put(data, JOINT + 0x50, '4f', 1, 0, 0, 0)
            put(data, JOINT + 0x60, '4f', 0, 1, 0, 0)
            for i in range(3):
                put(data, JOINT + 0x70 + 16 * i, '4f', *(int(j == i) for j in range(4)))
            put(data, JOINT + 0xa0, '3f', -2, 3, [0, -1, 0.25, 4][seed % 4])
        expected = execute(retail, va, data, [PAIR, OUTPUT], JOINT)
        actual = execute(compiled, target, data, [PAIR, OUTPUT], JOINT)
        assert actual == expected, (name, seed)


def test_velocity_bias_callback_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'add_velocity_bias_315440')
    for seed in range(64):
        data = bytearray(b'\xcd' * 65536)
        rng = random.Random(seed)
        record = JOINT + 4 * (seed % 4)  # Deliberately includes unaligned input.
        put(data, record, '4fI', *(rng.uniform(-4, 4) for _ in range(4)), TERMS)
        for offset in range(0, 0x60, 4):
            put(data, TERMS + offset, 'f', rng.uniform(-10, 10))
        precision = [0x7f, 0x27f, 0x37f][seed % 3]
        expected = execute(retail, 0x315440, data, [0, record], precision=precision)
        actual = execute(compiled, target, data, [0, record], precision=precision)
        assert actual == expected, seed


@pytest.mark.parametrize('va,name', [(0x315340, 'function_315340'), (0x3153c0, 'function_3153c0')])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_modifier_installation_against_retail(solver_images, va, name, precision):
    retail, compiled, symbols = solver_images
    compiled_va = address(symbols, name)
    for seed in range(64):
        data = bytearray(b'\xcd' * 65536)
        contact = MANAGER + 0x10
        put(data, contact, 'I', TABLE if seed % 3 else TABLE + 0x100)
        put(data, contact + 12, 'III', ENTITY_A, ENTITY_B, OWNER)
        put(data, OWNER, 'I', TABLE)
        put(data, TABLE + 8, 'I', CALLBACK)
        body = ENTITY_A if seed % 2 else ENTITY_B
        # Include zeros, infinities and NaN payloads: scalar copies preserve bits.
        scale_a = [0, 0x80000000, 0x3e800000, 0x7f800000, 0x7fc12345, 0x7f812345][seed % 6]
        scale_b = 0x3f400000
        args = [MANAGER, body, ENTITY_B, scale_a, scale_b] if va == 0x315340 else [MANAGER, body, TERMS]
        results = []
        for uc, entry, original in [(retail, va, True), (compiled, compiled_va, False)]:
            for retail_global, name_global, value in [(0x480990, 'g_480990', TABLE),
                                                       (0x480770, 'g_480770', TABLE + 0x200),
                                                       (0x480880, 'g_480880', TABLE + 0x300)]:
                uc.mem_write(retail_global if original else address(symbols, name_global), struct.pack('<I', value))
            calls = []

            def owner_add(machine, address_, size, user):
                sp = machine.reg_read(UC_X86_REG_ESP)
                receiver, sizes = words(machine, sp + 4, 2)
                assert words(machine, receiver, 1) == (TABLE,)
                expected_scales = ((scale_a, scale_b) if body == ENTITY_A else (scale_b, scale_a))
                expected_values = (expected_scales if va == 0x315340 else
                                   (TERMS, 0xbf800000 if body == ENTITY_A else 0x3f800000))
                assert words(machine, receiver + 0x40, 2) == expected_values
                calls.append((machine.reg_read(UC_X86_REG_ECX), receiver,
                              bytes(machine.mem_read(sizes, 16)), bytes(machine.mem_read(receiver + 0x40, 8))))
                # Publishing the replacement table must observe the owner callback.
                if seed & 4:
                    global_va = (0x480770 if va == 0x315340 else 0x480880) if original else address(
                        symbols, 'g_480770' if va == 0x315340 else 'g_480880')
                    machine.mem_write(global_va, struct.pack('<I', TABLE + 0x400))
                for reg in [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
                    machine.reg_write(reg, 0x76543210)
                return_from_hook(machine, 8)

            result = execute(uc, entry, data, args, hooks=[(CALLBACK, owner_add)], precision=precision)
            if seed % 3:
                table = TABLE + (0x400 if seed & 4 else 0x200 if va == 0x315340 else 0x300)
                assert words(uc, contact, 1) == (table,)
            # Once its table changes, installing the same modifier again does nothing.
            repeat = execute(uc, entry, result, args, hooks=[(CALLBACK, owner_add)], precision=precision)
            assert repeat == result
            results.append((result, calls))
        assert results[0] == results[1], (name, seed)
        assert len(results[0][1]) == int(seed % 3 != 0)


@pytest.mark.parametrize('va,name', [(0x315620, 'contact_sizes_3155f0::build'),
                                    (0x315740, 'contact_sizes_315720::build')])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_contact_modifier_builders_against_retail(solver_images, va, name, precision):
    retail, compiled, symbols = solver_images
    for seed in range(128 if va == 0x315620 else 64):
        data, _ = case(0x31f160, seed)
        put(data, JOINT + 12, 'II', ENTITY_A, ENTITY_B)
        put(data, JOINT + 0x38, 'I', seed % 5)
        put(data, JOINT + 0x40, '2f', [0, 0.5, 2][seed % 3], [-1, 0, 1, 3][seed % 4])
        put(data, JOINT + 0x100, '4f', 1.5, -2.25, 0.25, 4)
        if seed >= 64:
            # Include signed zeros, subnormals, overflow and both kinds of NaN.
            # Odd seeds have the property; use seed // 2 so they cover every scale.
            scales = [0, 0x80000000, 1, 0x3f000001, 0x7f7fffff,
                      0x7f800000, 0x7fc12345, 0x7f812345]
            components = [0x3f800001, 0xbf800001, 1, 0x80000001,
                          0x7f7fffff, 0xff800000, 0x7fc54321, 0x80000000]
            put(data, JOINT + 0x44, 'I', scales[(seed // 2) % len(scales)])
            put(data, JOINT + 0x100, '4I',
                *(components[(seed // 16 + i) % len(components)] for i in range(4)))
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            calls = []

            def contact_build(machine, address_, size, user):
                sp = machine.reg_read(UC_X86_REG_ESP)
                pair, output = words(machine, sp + 4, 2)
                a, b = words(machine, pair, 2)
                calls.append((machine.reg_read(UC_X86_REG_ECX),
                              bytes(machine.mem_read(a + 0x30, 16)), bytes(machine.mem_read(b + 0x30, 16))))
                start, = words(machine, output, 1)
                count = (seed % 5) * 0x30 + (0x60 if seed % 3 else 0)
                machine.mem_write(start, bytes([seed]) * count)
                machine.mem_write(output, struct.pack('<I', start + count))
                return_from_hook(machine, 8)

            def property_get(machine, address_, size, user):
                machine.reg_write(UC_X86_REG_EAX, JOINT + 0x100 if seed % 2 else 0)
                return_from_hook(machine)

            hooks = [(0x31bdf0 if original else address(symbols, 'lane_b_contact_constraint_prefix::build'), contact_build),
                     (0x1cf380 if original else address(symbols, 'havok_entity_property_2003_get'), property_get)]
            result = bytearray(execute(uc, va if original else address(symbols, name), data,
                                       [PAIR, OUTPUT], JOINT, hooks, precision=precision))
            if not original:
                replacements = {address(symbols, n): r for r, n in [(0x315280, 'lane_a_scale_masses'),
                               (0x3152f0, 'lane_a_restore_masses'), (0x315440, 'add_velocity_bias_315440')]}
                for offset in range(OPS - HEAP, OPS - HEAP + 4096, 4):
                    word, = struct.unpack_from('<I', result, offset)
                    if word in replacements:
                        struct.pack_into('<I', result, offset, replacements[word])
            results.append((result, calls))
        assert results[0] == results[1], (name, seed, precision)


def test_contact_array_destruction_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    for seed in range(8):
        data = bytearray(b'\xcd' * 65536)
        put(data, OWNER, 'I', TABLE)
        put(data, TABLE + 0x14, 'I', CALLBACK)
        for i, offset in enumerate([0x20, 0x34, 0x90]):
            put(data, JOINT + offset, 'III', TERMS + 0x100 * i, 0, 3 | (0x80000000 if seed & (1 << i) else 0))
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', OWNER))
            calls = []

            def release(machine, address_, size, user):
                calls.append(words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3))
                return_from_hook(machine, 12)

            def base_destroy(machine, address_, size, user):
                calls.append(('base', machine.reg_read(UC_X86_REG_ECX)))
                return_from_hook(machine)

            hooks = [(CALLBACK, release), (0x31b9e0 if original else address(symbols, 'lane_a_constraint::~lane_a_constraint'), base_destroy)]
            output = execute(uc, 0x315510 if original else address(symbols, 'contact_constraint_315510::~contact_constraint_315510'),
                             data, [], JOINT, hooks)
            results.append((output, calls))
        assert results[0] == results[1], seed
        assert results[0][1][-1] == ('base', JOINT)
        assert len(results[0][1]) == 4 - seed.bit_count()
