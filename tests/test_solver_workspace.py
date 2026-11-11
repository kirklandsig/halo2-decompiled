"""Optional retail execution of workspace allocation and virtual dispatch paths."""
import random
import struct

import pytest

pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP, UC_X86_REG_EIP
from test_solver_rows import HEAP, STOP, solver_images  # noqa: F401
from test_solver_validation import address, execute

SCRATCH, ALLOCATOR, TABLE = HEAP + 0x1000, HEAP + 0x1100, HEAP + 0x1180
ENTITIES, CONSTRAINTS = HEAP + 0x1200, HEAP + 0x1280
MOTION_TABLE, CONSTRAINT_TABLE = HEAP + 0x2800, HEAP + 0x2900
STEP, SETTINGS, PAIR, WORLD, SIZES = [HEAP + x for x in (0x3500, 0x3540, 0x35c0, 0x3600, 0x3700)]
BUFFER, TIMERS, STACK_BUFFER, OVERFLOW = [HEAP + x for x in (0x4000, 0x8000, 0x9000, 0xb000)]
ALLOCATE, RELEASE, EXPORT, IMPORT, BUILD = [STOP + x for x in (0x100, 0x110, 0x120, 0x130, 0x140)]


def put(data, target, fmt, *values):
    struct.pack_into('<' + fmt, data, target - HEAP, *values)


def words(uc, target, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(target, count * 4))


def make_case(seed):
    rng = random.Random(seed)
    data = bytearray(rng.randbytes(65536))
    bodies, constraints = [0, 1, 3][seed % 3], [0, 1, 3][seed // 3 % 3]
    body_bytes, term_bytes, op_bytes = bodies * 128, constraints * 64, constraints * 24
    count = constraints * 3
    terms = body_bytes + 0x90
    term_end = terms + term_bytes
    required = term_end + count * 4 + 8 + op_bytes + 4
    capacity = [0, terms - 16, terms, terms + 16, term_end, term_end + 8,
                required - 1, required, required + 64][seed // 9 % 9]
    put(data, SCRATCH + 8, '2IB', BUFFER, capacity, 0)
    put(data, ALLOCATOR, 'I', TABLE)
    put(data, ALLOCATOR + 8, '2I', STACK_BUFFER, 0x4000 if seed & 1 else 0)
    put(data, ALLOCATOR + 0x14, 'I', [0, STACK_BUFFER, OVERFLOW, HEAP][seed // 2 % 4])
    put(data, TABLE + 0x24, '2I', ALLOCATE, RELEASE)
    put(data, MOTION_TABLE + 12, '2I', EXPORT, IMPORT)
    put(data, CONSTRAINT_TABLE + 0x24, 'I', BUILD)
    put(data, SIZES + 8, '5I', 64, term_bytes, op_bytes, body_bytes, count)
    put(data, SETTINGS + 0x20, 'I', seed + 100)
    for i in range(4):
        entity, motion = HEAP + 0x2000 + i * 0x100, HEAP + 0x2400 + i * 0x100
        put(data, ENTITIES + i * 4, 'I', entity)
        put(data, entity + 0x3c, 'I', motion)
        put(data, motion, 'I', MOTION_TABLE)
        put(data, motion + 8, 'I', (i + 1) * 128 if i < bodies and seed & 1 else 0)
    for i in range(constraints):
        constraint = HEAP + 0x3000 + i * 0x100
        put(data, CONSTRAINTS + i * 4, 'I', constraint)
        put(data, constraint, 'I', CONSTRAINT_TABLE)
        put(data, constraint + 12, '2I', HEAP + 0x2000 + (i % max(1, bodies)) * 0x100,
            HEAP + 0x2000 + ((i + 1) % max(1, bodies)) * 0x100)
    return data, [STEP, SETTINGS, PAIR, WORLD, SIZES, ENTITIES, bodies, CONSTRAINTS, constraints]


def run_workspace(uc, entry, solver, globals_, seed, data, args):
    uc.mem_write(HEAP, bytes(data))
    for name, value in [('g_484b38', SCRATCH), ('g_480118', ALLOCATOR),
                        ('g_480154', TIMERS), ('g_480158', TIMERS + (16 if seed & 2 else 0))]:
        uc.mem_write(globals_[name], struct.pack('<I', value))
    calls = []

    def hook(machine, target, size, _):
        sp = machine.reg_read(UC_X86_REG_ESP)
        ret, a, b, c, d = words(machine, sp, 5)
        owner = machine.reg_read(UC_X86_REG_ECX)
        cleanup = 4
        result = 0
        if target == ALLOCATE:
            assert owner == ALLOCATOR
            calls.append(('allocate', a))
            result = OVERFLOW
            cleanup = 8
        elif target == RELEASE:
            assert owner == ALLOCATOR
            calls.append(('release', a, b))
            cleanup = 12
        elif target == EXPORT:
            assert a == STEP and b == WORLD
            calls.append(('export', owner, c, words(machine, owner + 8, 1)[0]))
            machine.mem_write(c, bytes([owner >> 8 & 255]) * 128)
            result = c + 128
            cleanup = 16
        elif target == IMPORT:
            assert a == STEP
            calls.append(('import', owner, b, c))
            machine.mem_write(owner + 0x50, struct.pack('<I', seed))
            result = c + 128
            cleanup = 16
        elif target == BUILD:
            assert a == PAIR
            terms, ops = words(machine, b, 2)
            calls.append(('build', owner, words(machine, PAIR, 2), terms, ops))
            machine.mem_write(terms, bytes([owner >> 8 & 255]) * 64)
            machine.mem_write(ops, bytes([owner >> 8 & 255]) * 24)
            machine.mem_write(b, struct.pack('<II', terms + 64, ops + 24))
            cleanup = 12
        elif target == solver:
            calls.append(('solve', a, b, c, d))
            assert a == SETTINGS and machine.mem_read(SCRATCH + 16, 1) == b'\x01'
            count = words(machine, SIZES + 24, 1)[0]
            assert bytes(machine.mem_read(d, count * 4)) == bytes(count * 4)
            for i in range(count): machine.mem_write(d + i * 4, struct.pack('<f', i + 0.5))
            machine.mem_write(SETTINGS + 0x20, struct.pack('<I', seed + 200))
            result = [0, 1, 2, 0xffffffff][seed % 4]
        else: raise AssertionError(hex(target))
        machine.reg_write(UC_X86_REG_EAX, result)
        machine.reg_write(UC_X86_REG_ESP, sp + cleanup)
        machine.reg_write(UC_X86_REG_EIP, ret)

    hooks = [uc.hook_add(UC_HOOK_CODE, hook, begin=a, end=a)
             for a in (ALLOCATE, RELEASE, EXPORT, IMPORT, BUILD, solver)]
    try: execute(uc, entry, args)
    finally:
        for handle in hooks: uc.hook_del(handle)
    actual = bytearray(uc.mem_read(HEAP, 65536))
    timer_cursor = words(uc, globals_['g_480154'], 1)[0]
    if seed & 2:
        assert words(uc, TIMERS, 1)[0] == globals_['g_4153f4']
        put(actual, TIMERS, 'I', 0x4153f4)
    assert uc.mem_read(SCRATCH + 16, 1) == b'\0'
    return actual, calls, timer_cursor


def test_solver_workspace_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    globals_ = {'g_484b38': 0x484b38, 'g_480118': 0x480118, 'g_480154': 0x480154,
                'g_480158': 0x480158, 'g_4153f4': 0x4153f4}
    mapped = {name: address(symbols, name) for name in globals_}
    entry = address(symbols, 'function_31c100')
    solver = address(symbols, 'solve_operations_31c940')
    for seed in range(324):
        data, args = make_case(seed)
        expected = run_workspace(retail, 0x31c100, 0x31c940, globals_, seed, data, args)
        actual = run_workspace(compiled, entry, solver, mapped, seed, data, args)
        assert actual[1:] == expected[1:], (seed, expected[1:], actual[1:])
        assert actual[0] == expected[0], (seed, [hex(HEAP + i) for i, (a, b)
                                               in enumerate(zip(actual[0], expected[0])) if a != b][:20])


def test_kind_object_factory_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    entry = address(symbols, 'function_31c4f0')
    allocator = address(symbols, 'g_480118')
    tables = {s.va for s in symbols.symbols if s.name == '??_7kind_object_3192b0@@6B@'}
    assert len(tables) == 1
    table = tables.pop()
    for seed in range(64):
        data = bytearray(random.Random(seed).randbytes(65536))
        put(data, ALLOCATOR, 'I', TABLE)
        put(data, TABLE + 16, 'I', ALLOCATE)
        outcomes = []
        for uc, function, global_, vtable in [(retail, 0x31c4f0, 0x480118, 0x415100),
                                              (compiled, entry, allocator, table)]:
            uc.mem_write(HEAP, bytes(data))
            uc.mem_write(global_, struct.pack('<I', ALLOCATOR))
            calls = []

            def hook(machine, target, size, _):
                assert machine.reg_read(UC_X86_REG_ECX) == ALLOCATOR
                sp = machine.reg_read(UC_X86_REG_ESP)
                ret, size, tag = words(machine, sp, 3)
                calls.append((size, tag))
                machine.reg_write(UC_X86_REG_EAX, BUFFER)
                machine.reg_write(UC_X86_REG_ESP, sp + 12)
                machine.reg_write(UC_X86_REG_EIP, ret)

            handle = uc.hook_add(UC_HOOK_CODE, hook, begin=ALLOCATE, end=ALLOCATE)
            try: result = execute(uc, function)
            finally: uc.hook_del(handle)
            assert result == BUFFER and calls == [(8, 0x26)]
            assert words(uc, BUFFER, 1)[0] == vtable
            actual = bytearray(uc.mem_read(HEAP, 65536))
            put(actual, BUFFER, 'I', 0x415100)
            outcomes.append(actual)
        assert outcomes[0] == outcomes[1], seed
