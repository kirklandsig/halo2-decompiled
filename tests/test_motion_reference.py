"""Exercise constructed motion helpers through their registry and delete slots.

The existing reference destructor and name virtuals run normally. Only the
registry's copy/set boundary and allocator release use synthetic callbacks.
"""
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
from test_constraint_initialization import CASES, OBJECT, symbol_address, execute as construct

REGISTRY, ALLOCATOR, TABLE, BUFFER, NAME = [HEAP + n for n in (0x3000, 0x3100, 0x3200, 0x3300, 0x3400)]
RELEASE = STOP + 0x100


def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def words(uc, va, count=1):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, count * 4))


def returning(uc, consumed):
    sp = uc.reg_read(UC_X86_REG_ESP)
    ret, = words(uc, sp)
    uc.reg_write(UC_X86_REG_ESP, sp + 4 + consumed)
    uc.reg_write(UC_X86_REG_EIP, ret)


@pytest.mark.parametrize('kind', ['entity', 'serialized'])
def test_motion_registry_and_deleting_destructor(solver_images, kind):
    retail, compiled, symbols = solver_images
    retail_ctor, prefix = CASES[kind]
    for seed in range(96):
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            uc.mem_write(HEAP, bytes(65536))
            construct(uc, retail_ctor if original else symbol_address(symbols, prefix), kind, seed)
            uc.mem_write(OBJECT + 4, struct.pack('<H', 0x60 | (0x8000 if seed & 1 else 0)))
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))
            uc.mem_write(0x48011c if original else address(symbols, 'g_48011c'), struct.pack('<I', REGISTRY))
            uc.mem_write(ALLOCATOR, struct.pack('<I', TABLE))
            uc.mem_write(TABLE + 0x14, struct.pack('<I', RELEASE))
            uc.mem_write(NAME, b'motion-test\0')
            calls, snapshots = [], []

            def copy_name(machine, va, size, user):
                object_, buffer, length = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3)
                assert object_ == OBJECT and machine.reg_read(UC_X86_REG_ECX) == REGISTRY
                calls.append(('copy', object_, buffer, length))
                if length:
                    text = b'motion-test'[:length - 1] + b'\0'
                    machine.mem_write(buffer, text)
                machine.reg_write(UC_X86_REG_EAX, 11)
                machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
                machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)
                returning(machine, 12)

            def set_name(machine, va, size, user):
                object_, name = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert object_ == OBJECT and machine.reg_read(UC_X86_REG_ECX) == REGISTRY
                flag, = struct.unpack('<H', machine.mem_read(OBJECT + 4, 2))
                calls.append(('set', object_, name, flag))
                machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
                machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)
                returning(machine, 8)

            def release(machine, va, size, user):
                args = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3)
                assert args == (OBJECT, 0x60, 0x26)
                assert machine.reg_read(UC_X86_REG_ECX) == ALLOCATOR
                calls.append(('release', args))
                machine.reg_write(UC_X86_REG_ECX, 0xcccccccc)
                machine.reg_write(UC_X86_REG_EDX, 0xdddddddd)
                returning(machine, 12)

            hooks = [(0x2d7120 if original else address(symbols, 'c_reference_name_registry::copy_name'), copy_name),
                     (0x2d7180 if original else address(symbols, 'c_reference_name_registry::set_name'), set_name),
                     (RELEASE, release)]

            def call(slot, args, result=False):
                sp = STACK + 32768
                uc.mem_write(sp, struct.pack('<' + 'I' * (1 + len(args)), STOP, *args))
                saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
                for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_EDX]:
                    uc.reg_write(reg, 0x12345678)
                uc.reg_write(UC_X86_REG_ECX, OBJECT)
                uc.reg_write(UC_X86_REG_ESP, sp)
                uc.reg_write(UC_X86_REG_EFLAGS, 2)
                table, = words(uc, OBJECT)
                entry, = words(uc, table + slot * 4)
                handles = [uc.hook_add(UC_HOOK_CODE, callback, begin=va, end=va) for va, callback in hooks]
                try:
                    uc.emu_start(entry, STOP, count=10000)
                finally:
                    for handle in handles:
                        uc.hook_del(handle)
                assert uc.reg_read(UC_X86_REG_EIP) == STOP
                assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 * (1 + len(args))
                assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
                heap = bytearray(uc.mem_read(HEAP, 65536))
                expected = 0x452c30 if slot == 0 else 0x414abc
                if original:
                    assert words(uc, OBJECT)[0] == expected
                else:
                    name = 'c_sized_reference' if slot == 0 else 'c_314710'
                    assert words(uc, OBJECT)[0] == symbol_address(symbols, '??_7' + name + '@@6B@')
                struct.pack_into('<I', heap, OBJECT - HEAP, expected)
                value = uc.reg_read(UC_X86_REG_EAX)
                snapshots.append((heap, value if result else None))
                return value

            length = [0, 1, 5, 32][seed % 4]
            assert call(1, [BUFFER, length], True) == (11 if seed & 1 else 0)
            name = NAME if seed % 3 else 0
            call(2, [name])
            assert call(1, [BUFFER, length], True) == (11 if name else 0)
            registration = call(3, [])
            assert registration == (0x4806f8 if original else address(symbols, 'g_4806f8'))
            call(4, [seed])
            assert call(7, [], True) == 2
            flags = [0, 1, 2, 3, 0x80000001, 0xffffffff][seed % 6]
            assert call(0, [flags], True) == OBJECT
            assert len([c for c in calls if c[0] == 'release']) == bool(flags & 1)
            if name:
                assert ('set', OBJECT, 0, 0x60) in calls
            results.append((snapshots, calls))
        assert results[0] == results[1], (kind, seed)
