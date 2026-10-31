"""Optional retail comparison of bootstrap ordering and service replacement."""
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

ALLOCATOR, OLD_ALLOCATOR, OLD_SERVICE, TABLE, FINAL = [HEAP + n for n in (0x1000, 0x1200, 0x1400, 0x2000, 0x3000)]
ALLOCATE, DELETE, FINISH = STOP + 0x100, STOP + 0x110, STOP + 0x120


def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def words(uc, address_, count=1):
    return struct.unpack('<' + 'I' * count, uc.mem_read(address_, 4 * count))


def returning(uc, size=0):
    sp = uc.reg_read(UC_X86_REG_ESP)
    ret, = words(uc, sp)
    uc.reg_write(UC_X86_REG_ESP, sp + 4 + size)
    uc.reg_write(UC_X86_REG_EIP, ret)


@pytest.mark.parametrize('real_streams', [False, True])
def test_service_bootstrap(solver_images, real_streams):
    retail, compiled, symbols = solver_images
    global_names = {0x480118: 'g_480118', 0x480148: 'g_480148', 0x48014c: 'g_48014c',
                    0x480178: 'g_480178', 0x48017c: 'g_48017c', 0x480180: 'g_480180'}
    for seed in range(96):
        rng = random.Random(seed)
        data = bytearray(rng.randbytes(65536))
        def put(va, fmt, *values):
            struct.pack_into('<' + fmt, data, va - HEAP, *values)
        put(ALLOCATOR, 'I', TABLE)
        put(OLD_ALLOCATOR, 'II', TABLE, 1 + seed % 2)
        put(TABLE, 'I', DELETE)
        put(TABLE + 0x10, 'I', ALLOCATE)
        put(TABLE + 0x20, 'I', DELETE)
        put(FINAL, 'I', TABLE + 0x100)
        put(TABLE + 0x104, 'I', FINISH)
        for i in range(3):
            put(OLD_SERVICE + 0x100 * i, 'IHH', TABLE, 8, 1 + (seed >> i & 1))
        guard = [0, 0, 0, 1, 2, 255][seed % 6]
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            globals_ = {va: va if original else address(symbols, name) for va, name in global_names.items()}
            uc.mem_write(HEAP, bytes(data))
            for va, value in [(0x480118, OLD_ALLOCATOR if seed & 4 else 0), (0x480148, FINAL),
                              (0x480178, OLD_SERVICE if seed & 8 else 0),
                              (0x48017c, OLD_SERVICE + 0x100 if seed & 16 else 0),
                              (0x480180, OLD_SERVICE + 0x200 if seed & 32 else 0)]:
                uc.mem_write(globals_[va], struct.pack('<I', value))
            uc.mem_write(globals_[0x48014c], bytes([guard]))
            calls, allocations = [], []

            def acquire(machine, va, size, user):
                calls.append(('acquire',))
                machine.reg_write(UC_X86_REG_EAX, ALLOCATOR)
                returning(machine)

            def allocate(machine, va, size, user):
                length, tag = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert machine.reg_read(UC_X86_REG_ECX) == ALLOCATOR
                block = HEAP + 0x4000 + 0x100 * len(allocations)
                allocations.append((block, length, tag))
                calls.append(('allocate', length, tag))
                machine.reg_write(UC_X86_REG_EAX, block)
                machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
                machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)
                returning(machine, 8)

            def deleting(machine, va, size, user):
                calls.append(('delete', machine.reg_read(UC_X86_REG_ECX),
                              words(machine, machine.reg_read(UC_X86_REG_ESP) + 4)[0]))
                returning(machine, 4)

            def retained(machine, va, size, user):
                calls.append(('retain', words(machine, machine.reg_read(UC_X86_REG_ESP) + 4)[0]))
                machine.reg_write(UC_X86_REG_EAX, seed % 3 == 0)
                returning(machine)

            def registry(machine, va, size, user):
                calls.append(('registry', *(words(machine, globals_[v])[0] for v in (0x480180, 0x48017c, 0x480178))))
                returning(machine)

            def finish(machine, va, size, user):
                calls.append(('finish', machine.reg_read(UC_X86_REG_ECX), bytes(machine.mem_read(globals_[0x48014c], 1))))
                machine.mem_write(globals_[0x48014c], b'\x80')
                returning(machine)

            def stream_pair(machine, va, size, user):
                block = machine.reg_read(UC_X86_REG_ECX)
                calls.append(('stream_pair', block))
                machine.mem_write(block + 8, b'\x5a' * 24)
                machine.reg_write(UC_X86_REG_EAX, block)
                returning(machine)

            hooks = [(0x1473b0 if original else address(symbols, 'function_1473b0'), acquire),
                     (ALLOCATE, allocate), (DELETE, deleting), (FINISH, finish),
                     (0x279740 if original else address(symbols, 'function_279740'), retained),
                     (0x2d7ca0 if original else address(symbols, 'initialize_registered_services_2d7ca0'), registry)]
            if not real_streams:
                hooks.append((0x2d7570 if original else address(symbols, 'c_stream_pair_2d7650::c_stream_pair_2d7650'), stream_pair))
            if not any(lo == 0 for lo, hi, permissions in uc.mem_regions()):
                uc.mem_map(0, 4096)
            uc.mem_write(0, b'\xff' * 4)
            sp = STACK + 32768
            uc.mem_write(sp, struct.pack('<I', STOP))
            saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
            for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
                uc.reg_write(reg, 0x12345678)
            uc.reg_write(UC_X86_REG_ESP, sp)
            uc.reg_write(UC_X86_REG_EFLAGS, 2)
            uc.reg_write(UC_X86_REG_FPCW, 0x37f)
            uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
            uc.reg_write(UC_X86_REG_FPSW, 0)
            uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
            handles = [uc.hook_add(UC_HOOK_CODE, callback, begin=va, end=va) for va, callback in hooks]
            try:
                uc.emu_start(0x2d8780 if original else address(symbols, 'function_2d8780'), STOP, count=100000)
            finally:
                for handle in handles:
                    uc.hook_del(handle)
            assert uc.reg_read(UC_X86_REG_EIP) == STOP and uc.reg_read(UC_X86_REG_EAX) == 0
            assert uc.reg_read(UC_X86_REG_ESP) == sp + 4
            assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
            assert bytes(uc.mem_read(0, 4)) == b'\xff' * 4
            if guard == 0:
                factory, = words(uc, globals_[0x480180])
                handler, = words(uc, globals_[0x480178])
                dispatches = [
                    (factory, 1, 0x2d76f0, 'debug_stream_factory_2d76f0::open', [2]),
                    (factory, 2, 0x2d7750, 'reader_factory_2d7750::open_reader', [HEAP + 0x8000]),
                    (factory, 3, 0x2d79c0, 'stdio_stream_factory_2d79c0::open_writer', [HEAP + 0x8000]),
                ]
                for slot in (6, 7, 8):
                    dispatches.append((handler, slot, 0x2d8640, 'error_handler_2d8030::count_location', [HEAP + 0x8000, seed]))
                for slot in (9, 10, 11):
                    dispatches.append((handler, slot, 0x2d86a0, 'error_handler_2d8030::uncount_location', [HEAP + 0x8000, seed]))
                dispatches.extend([
                    (handler, 12, 0x2d7fb0, 'path_2d7f70::function_2d7fb0', [HEAP + 0x8000]),
                    (handler, 13, 0x2d7f70, 'path_2d7f70::function_2d7f70', [HEAP + 0x8000]),
                ])
                for receiver, slot, retail_target, name, args in dispatches:
                    def operation(machine, va, size, user):
                        received = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, len(args))
                        assert machine.reg_read(UC_X86_REG_ECX) == receiver and received == tuple(args)
                        calls.append(('slot', receiver, slot, received))
                        machine.reg_write(UC_X86_REG_EAX, HEAP + 0x8000)
                        machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
                        machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)
                        returning(machine, len(args) * 4)
                    target = retail_target if original else address(symbols, name)
                    hook = uc.hook_add(UC_HOOK_CODE, operation, begin=target, end=target)
                    table, = words(uc, receiver)
                    entry, = words(uc, table + 4 * slot)
                    uc.mem_write(sp, struct.pack('<' + 'I' * (1 + len(args)), STOP, *args))
                    uc.reg_write(UC_X86_REG_ESP, sp)
                    uc.reg_write(UC_X86_REG_ECX, receiver)
                    try:
                        uc.emu_start(entry, STOP, count=1000)
                    finally:
                        uc.hook_del(hook)
                    assert uc.reg_read(UC_X86_REG_EIP) == STOP
                    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 * (1 + len(args))
                    assert uc.reg_read(UC_X86_REG_EAX) == HEAP + 0x8000
                    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
            result = bytearray(uc.mem_read(HEAP, 65536))
            if not original:
                tables = {'debug_stream_factory_2d76f0': 0x411d2c, 'c_stream_pair_2d7650': 0x411d20,
                          'debug_writer_2d7c00': 0x411d3c, 'default_error_handler_2d8780': 0x411da4,
                          'stream_holder_2d8e70': 0x411e2c}
                remap = {s.va: value for name, value in tables.items() for s in symbols.symbols if s.name.startswith('??_7' + name + '@@')}
                for offset in range(0, len(result), 4):
                    value, = struct.unpack_from('<I', result, offset)
                    if value in remap:
                        struct.pack_into('<I', result, offset, remap[value])
            state = [bytes(uc.mem_read(globals_[va], 1 if va == 0x48014c else 4)) for va in global_names]
            results.append((result, calls, allocations, state))
        assert results[0] == results[1], (real_streams, seed)
        assert bool(results[0][1]) == (guard == 0)
