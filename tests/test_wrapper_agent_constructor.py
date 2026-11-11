"""Retail constructor/forwarding sequences with mutating shape callbacks."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OBJECT, A, B, WRAPPER, INNER, OTHER, INNER2 = [HEAP + x for x in (0, 0x100, 0x140, 0x200, 0x240, 0x280, 0x2c0)]
INPUT, TABLES, CHILD, MANAGER, OUTPUT, START_OUTPUT = [HEAP + x for x in (0x300, 0x400, 0x800, 0x900, 0xc00, 0xc40)]
DISPATCH, ALTERNATE, HANDLERS = HEAP + 0x2000, HEAP + 0x6000, STOP + 0x1000
TYPE, QUERY1, QUERY2, QUERY3, QUERY4, QUERY7 = [STOP + x for x in (0x10, 0x20, 0x30, 0x40, 0x50, 0x60)]
TARGETS = {
    0x2e63a0: 'lane_b_wrapper_agent::lane_b_wrapper_agent',
    0x2e6450: 'lane_b_wrapper_agent::get_penetrations',
    0x2e6510: 'lane_b_wrapper_agent::get_closest_points',
    0x2e65d0: 'lane_b_wrapper_agent::linear_cast',
    0x2e6690: 'lane_b_wrapper_agent::process_collision',
    0x2e6740: 'lane_b_wrapper_agent::update_filter',
}


def entries(symbols):
    result = {}
    for va, name in TARGETS.items():
        found = {s.va for s in symbols.find(name) if va != 0x2e63a0 or 'PBUbody_2ea940@@' in s.name}
        assert len(found) == 1, (name, found)
        result[va] = found.pop()
    return result


@pytest.mark.parametrize('precision', [0x007f, 0x027f, 0x037f])
def test_wrapper_constructor_and_forwarding(solver_images, precision):
    retail, compiled, symbols = solver_images
    compiled_entries = entries(symbols)
    compiled_table = next(s.va for s in symbols.symbols if s.name == '??_7lane_b_wrapper_agent@@6B@')
    for seed in range(96):
        rng = random.Random(seed)
        data = bytearray(b'\xcd' * 65536)
        def put(at, fmt, *values):
            struct.pack_into('<' + fmt, data, at - HEAP, *values)
        put(OBJECT + 4, 'HH', 16 + seed, 1 + seed * 13)
        key = rng.getrandbits(32)
        put(A, '4I', WRAPPER, key, HEAP + 0x1000, 0)
        put(B, '4I', OTHER, rng.getrandbits(32), HEAP + (0x1000 if seed % 2 else 0x1100), A)
        put(WRAPPER + 12, 'I', INNER)
        put(INPUT, 'I', DISPATCH)
        put(TABLES + 20, 'I', TYPE)
        for i, shape in enumerate((WRAPPER, INNER, OTHER, INNER2)):
            put(shape, '2I', TABLES, (seed + i * 7) % 32)
        put(CHILD, 'I', TABLES + 64)
        for i, callback in [(1, QUERY1), (2, QUERY2), (3, QUERY3), (4, QUERY4), (7, QUERY7)]:
            put(TABLES + 64 + i * 4, 'I', callback)
        put(OUTPUT, 'I', 0)
        for table_index, table in enumerate((DISPATCH, ALTERNATE)):
            for pair in range(1024):
                put(table + 0x18c + pair * 4, 'I', HANDLERS + 4096 * table_index + 4 * pair)
        other = A if seed % 5 == 0 else B
        child_result = 0 if seed % 8 == 0 else CHILD
        results = []
        for uc, starts, vtable in [(retail, {v: v for v in TARGETS}, 0x412988),
                                    (compiled, compiled_entries, compiled_table)]:
            for address, length in [(0, 4096), (HANDLERS, 8192)]:
                try:
                    uc.mem_map(address, length)
                except UcError:
                    pass
            uc.mem_write(HANDLERS, b'\xc3' * 8192)
            uc.mem_write(HEAP, bytes(data))
            trace = []
            def words(at, n=1):
                return struct.unpack('<' + 'I' * n, uc.mem_read(at, n * 4))
            def hook(machine, pc, size, unused):
                if pc not in (TYPE, QUERY1, QUERY2, QUERY3, QUERY4, QUERY7) and not HANDLERS <= pc < HANDLERS + 8192:
                    return
                sp = machine.reg_read(r.UC_X86_REG_ESP)
                this = machine.reg_read(r.UC_X86_REG_ECX)
                result, cleanup = 0xabcdef01, 0
                if pc == TYPE:
                    result = words(this + 4)[0]
                    trace.append(('type', this, result))
                    if seed % 2:
                        # The constructor has already captured the child body and dispatcher.
                        machine.mem_write(A + 4, struct.pack('<2I', seed, HEAP + 0x1200))
                        machine.mem_write(WRAPPER + 12, struct.pack('<I', INNER2))
                        machine.mem_write(INPUT, struct.pack('<I', ALTERNATE))
                elif HANDLERS <= pc < HANDLERS + 8192:
                    body, b, query_input, manager = words(sp + 4, 4)
                    assert (b, query_input, manager) == (other, INPUT, MANAGER)
                    body_data = words(body, 4)
                    assert body_data == (INNER, key, HEAP + 0x1000, A)
                    first, second = words(INNER + 4)[0], words(words(other)[0] + 4)[0]
                    assert pc == HANDLERS + 4 * (32 * first + second)
                    trace.append(('create', body_data, words(other, 4), words(INPUT)[0]))
                    result = child_result
                else:
                    assert this == CHILD
                    n = 5 if pc == QUERY3 else 3 if pc == QUERY7 else 4
                    args = words(sp + 4, n)
                    assert args[1:3] == (other, INPUT)
                    assert words(args[0], 4) == (words(WRAPPER + 12)[0], words(A + 4)[0], words(A + 8)[0], A)
                    if n >= 4:
                        assert args[3] == OUTPUT
                    if n == 5:
                        assert args[4] == (START_OUTPUT if seed % 3 else 0)
                    trace.append(('query', pc, words(args[0], 4), words(other, 4), args[3:]))
                    machine.mem_write(OUTPUT, struct.pack('<I', words(OUTPUT)[0] + 1))
                    cleanup = n * 4
                machine.reg_write(r.UC_X86_REG_EAX, result)
                machine.reg_write(r.UC_X86_REG_ECX, 0x87654321)
                machine.reg_write(r.UC_X86_REG_EDX, 0x10203040)
                machine.reg_write(r.UC_X86_REG_EIP, words(sp)[0])
                machine.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)
            handle = uc.hook_add(UC_HOOK_CODE, hook)
            try:
                targets = [0x2e63a0] + (list(TARGETS)[1:] if child_result else [])
                for va in targets:
                    args = [A, other, INPUT]
                    if va == 0x2e63a0:
                        args.append(MANAGER)
                    elif va != 0x2e6740:
                        args.append(OUTPUT)
                    if va == 0x2e65d0:
                        args.append(START_OUTPUT if seed % 3 else 0)
                    sp = STACK + 32768
                    uc.mem_write(STACK, b'\xcd' * 65536)
                    uc.mem_write(0, struct.pack('<I', 0xffffffff))
                    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
                    saved = (r.UC_X86_REG_EBX, r.UC_X86_REG_ESI, r.UC_X86_REG_EDI, r.UC_X86_REG_EBP)
                    for reg in saved:
                        uc.reg_write(reg, 0x12345678)
                    uc.reg_write(r.UC_X86_REG_ECX, OBJECT)
                    uc.reg_write(r.UC_X86_REG_ESP, sp)
                    uc.reg_write(r.UC_X86_REG_EFLAGS, 2)
                    uc.reg_write(r.UC_X86_REG_FPCW, precision)
                    uc.emu_start(starts[va], STOP, count=10000)
                    assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
                    assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 4 + len(args) * 4
                    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
                    assert uc.reg_read(r.UC_X86_REG_FPCW) == precision
                    assert words(0)[0] == 0xffffffff
                    assert words(OBJECT)[0] == vtable
                    if va == 0x2e63a0:
                        assert uc.reg_read(r.UC_X86_REG_EAX) == OBJECT
                        assert words(OBJECT + 8, 2) == (MANAGER, child_result)
                assert words(OUTPUT)[0] == (5 if child_result else 0)
                result = bytearray(uc.mem_read(HEAP, 65536))
                struct.pack_into('<I', result, 0, 0)  # verified class-table identity above
                results.append((bytes(result), trace))
            finally:
                uc.hook_del(handle)
        assert results[0] == results[1], (seed, precision)
