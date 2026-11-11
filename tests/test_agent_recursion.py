"""Retail differential execution of recursive shape-pair registration.

The recursion and type-name resolver execute normally. Only the diagnostic
formatter is intercepted, recording its text/arguments and respecting its ABI.
All tables, alternate graphs and handler values are synthetic.
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
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OBJECT, HANDLERS, EDGES = HEAP + 0x100, HEAP + 0x6000, HEAP + 0x6100


def address(symbols, name):
    found = {s.va for s in symbols.find(name)}
    assert len(found) == 1, (name, found)
    return found.pop()


def put(data, va, fmt, *values):
    struct.pack_into('<' + fmt, data, va - HEAP, *values)


def words(uc, va, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, 4 * count))


def string(uc, va):
    result = bytearray()
    for i in range(512):
        value = uc.mem_read(va + i, 1)[0]
        if not value:
            return bytes(result)
        result.append(value)
    raise AssertionError('unterminated diagnostic string')


def inputs(seed, mode, alias):
    rng = random.Random(seed)
    data = bytearray(rng.randbytes(65536))
    types = [(0, 31), (2, 3), (2, 2), (31, 0), (6, 9), (1, 1)]
    if mode == 'wildcards':
        types = [(-1, 3), (2, -1), (-1, -1)]
    first, second = types[seed % len(types)]
    edges = [(6, 2), (7, 6), (8, 2), (9, 3), (10, 9), (11, 8)] if mode == 'alternates' else []
    if edges and seed % 5 == 0:
        edges += [(6, 2)]  # Repeated registrations remain ordered, not deduplicated.
    put(data, OBJECT + 0x4d90, 'III', EDGES, len(edges), 0x80000010)
    for i, edge in enumerate(edges):
        put(data, EDGES + i * 8, 'ii', *edge)
    diagnostic = seed % 2 if (first, second) != (-1, -1) else 0
    put(data, OBJECT + 0x4d8c, 'BB', 0x7f, diagnostic)
    for a in range(32):
        for b in range(32):
            put(data, OBJECT + 0x418c + 3 * (a * 32 + b), 'BBB',
                (a + seed) % 32, (b + seed * 3) % 32,
                [0, 1, 2, 127, 128, 255][(a + b + seed) % 6])
    handlers = HANDLERS
    if alias:
        # The handler input overlaps destination table 1, so later loads must
        # see any earlier stores to those same cells.
        handlers = OBJECT + 0x118c + 4 * (max(first, 1) * 32 + max(second, 1))
    put(data, handlers, '4I', *(rng.getrandbits(32) for _ in range(4)))
    args = [handlers, first & 0xffffffff, second & 0xffffffff,
            seed % 32, (seed * 3 + 7) % 32, [0, 1, 2, 126, 127, 128][seed % 6]]
    return data, args


def execute(uc, entry, resolver, formatter, data, args):
    uc.mem_write(HEAP, bytes(data))
    uc.mem_write(STACK, b'\xcc' * 65536)
    sp = STACK + 0xe000
    uc.mem_write(sp, struct.pack('<7I', STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    uc.reg_write(UC_X86_REG_ECX, OBJECT)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    trace = []
    calls = []

    def observe_call(machine, va, size, user):
        assert machine.reg_read(UC_X86_REG_ECX) == OBJECT
        calls.append(words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 6))

    def observe_name(machine, va, size, user):
        trace.append(('name', words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)[0]))

    def format_message(machine, va, size, user):
        stack = machine.reg_read(UC_X86_REG_ESP)
        ret, buffer, capacity, fmt, a, b, c, d = words(machine, stack, 8)
        assert capacity == 1000
        assert STACK <= buffer < STACK + 65536 - 1024
        trace.append(('format', string(machine, fmt), tuple(string(machine, x) for x in (a, b, c, d))))
        machine.mem_write(buffer, b'formatted\x00')
        machine.reg_write(UC_X86_REG_EAX, 9)
        machine.reg_write(UC_X86_REG_ECX, 0xbadbad01)
        machine.reg_write(UC_X86_REG_EDX, 0xbadbad02)
        machine.reg_write(UC_X86_REG_ESP, stack + 4)
        machine.reg_write(UC_X86_REG_EIP, ret)

    handles = [uc.hook_add(UC_HOOK_CODE, fn, begin=va, end=va) for va, fn in
               [(entry, observe_call), (resolver, observe_name), (formatter, format_message)]]
    try:
        uc.emu_start(entry, STOP, count=500000)
    finally:
        for handle in handles:
            uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 28
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    return bytes(uc.mem_read(HEAP, 65536)), calls, trace


@pytest.mark.parametrize('mode', ['pairs', 'alternates', 'wildcards'])
@pytest.mark.parametrize('alias', [False, True], ids=['separate-handlers', 'aliased-handlers'])
def test_agent_registration_against_retail(solver_images, mode, alias):
    retail, compiled, symbols = solver_images
    targets = [address(symbols, name) for name in
               ['s_1c3a40::function_2e5a90', 'function_2e6220', 'function_2d8ca0']]
    diagnostics = False
    recursive = False
    for seed in range(128):
        data, args = inputs(seed, mode, alias)
        actual = execute(compiled, *targets, data, args)
        expected = execute(retail, 0x2e5a90, 0x2e6220, 0x2d8ca0, data, args)
        assert actual == expected, (mode, alias, seed)
        diagnostics |= bool(actual[2])
        recursive |= len(actual[1]) > 1
        assert actual[0][OBJECT - HEAP + 0x4d8c] == 1
    assert diagnostics
    assert recursive == (mode == 'alternates')
