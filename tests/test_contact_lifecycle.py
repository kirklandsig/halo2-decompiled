"""Optional retail execution of contact construction, allocation and teardown.

Entity registration, world notification and allocation use synthetic callbacks.
The shared constructor, setters and contact-array destructor execute normally.
"""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_ESP
from test_solver_rows import HEAP, STOP, solver_images  # noqa: F401
from test_constraint_families import address, put, words, execute, return_from_hook

OBJECT, FACTORY = HEAP + 0x1000, HEAP + 0x2000
A, B, WORLD, WORLD2 = HEAP + 0x3000, HEAP + 0x3200, HEAP + 0x4000, HEAP + 0x4200
FIRST, SECOND = HEAP + 0x5000, HEAP + 0x5100
ALLOCATOR, TABLE = HEAP + 0x6000, HEAP + 0x6100
ALLOCATE, RELEASE = STOP + 0x100, STOP + 0x110


def member(symbols, prefix):
    hits = {s.va for s in symbols.symbols if s.name.startswith(prefix)}
    assert len(hits) == 1, (prefix, hits)
    return hits.pop()


def normalize_table(data, offset, original, symbols, class_name, retail_value):
    value, = struct.unpack_from('<I', data, offset)
    if original:
        assert value == retail_value
    else:
        suffix = {0x4153c8: '6Bconstraint_reference_31b9e0@@@', 0x4153b8: '6Blane_a_constraint_interface@@@'}.get(retail_value, '')
        tables = {s.va for s in symbols.symbols if s.name.startswith('??_7' + class_name + '@@')
                  and (not suffix or s.name.endswith(suffix))}
        assert value in tables, (class_name, hex(value), tables)
    struct.pack_into('<I', data, offset, retail_value)


def clobber(machine):
    machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
    machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)


def test_contact_storage_constructor(solver_images):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'contact_default_31c000::contact_default_31c000')
    for seed in range(96):
        data = bytearray(random.Random(seed).randbytes(65536))
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            result = bytearray(execute(uc, 0x31c000 if original else target, data, [], OBJECT))
            assert uc.reg_read(UC_X86_REG_EAX) == OBJECT
            for offset, table in [(0, 0x4153c8), (8, 0x4153b8)]:
                normalize_table(result, OBJECT - HEAP + offset, original, symbols, 'contact_default_31c000', table)
            results.append(result)
        assert results[0] == results[1], seed


@pytest.mark.parametrize('factory', [False, True])
def test_contact_manager_construction(solver_images, factory):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'class_319080::create') if factory else member(symbols, '??0class_318e60@@QAE@PAVc_1d1261@@')
    register = address(symbols, 'class_30bdc0::function_30bdc0')
    for seed in range(96):
        data = bytearray(random.Random(seed + 1000).randbytes(65536))
        first, second = [(0, 0), (A, 0), (0, B), (A, B), (A, A), (B, A)][seed % 6]
        put(data, A + 6, 'H', 10 + seed)
        put(data, B + 6, 'H', 20 + seed)
        put(data, FIRST + 0x20, 'I', first)
        put(data, SECOND + 0x20, 'I', second)
        put(data, FACTORY + 8, 'I', WORLD)
        put(data, ALLOCATOR, 'I', TABLE)
        put(data, TABLE + 0x10, 'I', ALLOCATE)
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            calls = []
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))

            def allocate(machine, va, size, user):
                args = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                calls.append(('allocate', machine.reg_read(UC_X86_REG_ECX), args))
                # Inputs are captured before allocation, but owner is read after it.
                if seed & 1:
                    machine.mem_write(FIRST + 0x20, struct.pack('<I', B))
                    machine.mem_write(SECOND + 0x20, struct.pack('<I', A))
                    machine.mem_write(FACTORY + 8, struct.pack('<I', WORLD2))
                clobber(machine)
                machine.reg_write(UC_X86_REG_EAX, OBJECT)
                return_from_hook(machine, 8)

            def registration(machine, va, size, user):
                entity = machine.reg_read(UC_X86_REG_ECX)
                constraint, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                refs, = struct.unpack('<H', machine.mem_read(entity + 6, 2))
                calls.append(('register', entity, constraint, refs))
                clobber(machine)
                return_from_hook(machine, 4)

            args = [FIRST, SECOND, HEAP + 0x7000] if factory else [WORLD, first, second]
            result = bytearray(execute(uc, (0x3191c0 if factory else 0x319110) if original else target,
                                      data, args, FACTORY if factory else OBJECT,
                                      [(ALLOCATE, allocate), (0x30bdc0 if original else register, registration)]))
            assert uc.reg_read(UC_X86_REG_EAX) == OBJECT
            normalize_table(result, OBJECT - HEAP, original, symbols, 'class_318e60', 0x415074)
            for offset, table in [(0x10, 0x4153c8), (0x18, 0x4153b8)]:
                normalize_table(result, OBJECT - HEAP + offset, original, symbols, 'contact_default_31c000', table)
            results.append((result, calls))
        assert results[0] == results[1], (factory, seed)
        assert len(results[0][1]) == int(factory) + bool(first) + bool(second)


@pytest.mark.parametrize('routine', ['manager', 'delete_manager', 'delete_contact', 'adjust_contact'])
def test_contact_teardown(solver_images, routine):
    retail, compiled, symbols = solver_images
    retail_va = {'manager': 0x3190a0, 'delete_manager': 0x319190,
                 'delete_contact': 0x31c0d0, 'adjust_contact': 0x31c0c0}[routine]
    if routine == 'manager':
        target = address(symbols, 'class_318e60::~class_318e60')
    elif routine == 'delete_manager':
        target = member(symbols, '??_Gclass_318e60@@')
    elif routine == 'delete_contact':
        target = member(symbols, '??_Gcontact_default_31c000@@UAEPAX')
    else:
        target = member(symbols, '??_Econtact_default_31c000@@W7')
    is_manager = routine in ('manager', 'delete_manager')
    constraint = OBJECT + (0x10 if is_manager else 0)
    for seed in range(96):
        data = bytearray(random.Random(seed + 2000).randbytes(65536))
        put(data, OBJECT + 4, 'H', 0x120 if is_manager else 0x110)
        put(data, OBJECT + 8, 'I', WORLD)
        put(data, ALLOCATOR, 'I', TABLE)
        put(data, TABLE + 0x14, 'I', RELEASE)
        for i, offset in enumerate([0x20, 0x34, 0x90]):
            put(data, constraint + offset, 'III', HEAP + 0x8000 + 0x400 * i,
                seed // 32, [0, 1, 5, 0x7fffffff][seed % 4] | (0x80000000 if seed & (1 << i) else 0))
        flags = [0, 1, 2, 3][seed // 8 % 4]
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            calls = []
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))

            def release(machine, va, size, user):
                calls.append(('release', machine.reg_read(UC_X86_REG_ECX),
                              words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3)))
                clobber(machine)
                return_from_hook(machine, 12)

            def notify(machine, va, size, user):
                result, entry = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                calls.append(('notify', machine.reg_read(UC_X86_REG_ECX), entry))
                machine.mem_write(result, bytes([seed & 1]))
                machine.reg_write(UC_X86_REG_EAX, result)
                clobber(machine)
                return_from_hook(machine, 8)

            def base_destroy(machine, va, size, user):
                calls.append(('base', machine.reg_read(UC_X86_REG_ECX)))
                clobber(machine)
                return_from_hook(machine)

            hooks = [(RELEASE, release),
                     (0x30ca30 if original else address(symbols, 'c_1d1261::function_30ca30'), notify),
                     (0x31b9e0 if original else address(symbols, 'lane_a_constraint::~lane_a_constraint'), base_destroy)]
            args = [] if routine == 'manager' else [flags]
            result = bytearray(execute(uc, retail_va if original else target, data, args,
                                      OBJECT + (8 if routine == 'adjust_contact' else 0), hooks))
            if routine != 'manager':
                assert uc.reg_read(UC_X86_REG_EAX) == OBJECT
            if is_manager:
                normalize_table(result, OBJECT - HEAP, original, symbols, 'referenced_2d88a0', 0x452c30)
            results.append((result, calls))
        assert results[0] == results[1], (routine, seed)
        expected_calls = 1 + 3 - (seed & 7).bit_count()
        expected_calls += bool(is_manager and seed // 32)
        expected_calls += bool(routine != 'manager' and flags & 1)
        assert len(results[0][1]) == expected_calls


def test_construct_destroy_roundtrip(solver_images):
    """Run the real shared base teardown as well as both entity setters."""
    retail, compiled, symbols = solver_images
    constructor = member(symbols, '??0class_318e60@@QAE@PAVc_1d1261@@')
    destructor = address(symbols, 'class_318e60::~class_318e60')
    for seed in range(96):
        data = bytearray(random.Random(seed + 3000).randbytes(65536))
        first, second = [(0, 0), (A, 0), (0, B), (A, B), (A, A), (B, A)][seed % 6]
        put(data, OBJECT + 4, 'H', 0x120)
        put(data, OBJECT + 0x14, 'H', 0x110)
        put(data, A + 6, 'H', 10 + seed)
        put(data, B + 6, 'H', 20 + seed)
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            calls = []

            def registration(machine, va, size, user):
                calls.append(('add', machine.reg_read(UC_X86_REG_ECX),
                              words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)[0]))
                clobber(machine)
                return_from_hook(machine, 4)

            def removal(machine, va, size, user):
                calls.append(('remove', machine.reg_read(UC_X86_REG_ECX),
                              words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)[0]))
                clobber(machine)
                return_from_hook(machine, 4)

            hooks = [(0x30bdc0 if original else address(symbols, 'class_30bdc0::function_30bdc0'), registration),
                     (0x30bcc0 if original else address(symbols, 'entity_30bcc0::function_30bcc0'), removal)]
            constructed = execute(uc, 0x319110 if original else constructor, data,
                                  [WORLD, first, second], OBJECT, hooks)
            result = bytearray(execute(uc, 0x3190a0 if original else destructor, constructed, [], OBJECT, hooks))
            normalize_table(result, OBJECT - HEAP, original, symbols, 'referenced_2d88a0', 0x452c30)
            normalize_table(result, OBJECT - HEAP + 0x10, original, symbols, 'c_sized_reference', 0x452c30)
            normalize_table(result, OBJECT - HEAP + 0x18, original, symbols, 'lane_a_constraint_interface', 0x415340)
            assert result[A - HEAP + 6:A - HEAP + 8] == data[A - HEAP + 6:A - HEAP + 8]
            assert result[B - HEAP + 6:B - HEAP + 8] == data[B - HEAP + 6:B - HEAP + 8]
            results.append((result, calls))
        assert results[0] == results[1], seed
        assert [call[0] for call in results[0][1]] == ['add'] * (bool(first) + bool(second)) + ['remove'] * (bool(first) + bool(second))
