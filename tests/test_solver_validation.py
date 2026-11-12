"""Optional retail comparisons using synthetic validation data, never retail keys."""
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


def address(symbols, name):
    hits = {symbol.va for symbol in symbols.find(name)}
    assert len(hits) == 1, ('rebuild the full checker image', name, hits)
    return hits.pop()


def execute(uc, entry, args=()):
    sp = STACK + 32768
    uc.mem_write(STACK, b'\xcd' * 65536)
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.emu_start(entry, STOP, count=200000)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    return uc.reg_read(UC_X86_REG_EAX)


def test_validation_hash_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    entry = address(symbols, 'function_31c540')
    salt_address = address(symbols, 'g_415448')
    for seed in range(256):
        rng = random.Random(seed)
        length = [0, 1, 2, 31, 63, 64, 65, 96][seed % 8]
        text = bytes(rng.randrange(1, 256) for _ in range(length)) + b'\0'
        salt = bytes(rng.randrange(1, 256) for _ in range(seed % 65)).ljust(64, b'\0')
        data = text.ljust(65536, b'\xcd')
        retail.mem_write(HEAP, data)
        compiled.mem_write(HEAP, data)
        retail.mem_write(0x415448, salt)
        compiled.mem_write(salt_address, salt)
        expected = execute(retail, 0x31c540, [HEAP])
        actual = execute(compiled, entry, [HEAP])
        assert actual == expected, (seed, hex(expected), hex(actual))
        assert bytes(compiled.mem_read(HEAP, 65536)) == bytes(retail.mem_read(HEAP, 65536)) == data


def test_validation_gate_against_retail(solver_images):
    retail, compiled, symbols = solver_images
    globals_ = [0x445470, 0x46fbf0, 0x415440, 0x415444, 0x415448, 0x480c60]
    mapped = {va: address(symbols, 'g_' + hex(va)[2:]) for va in globals_}
    function = address(symbols, 'function_31c7d0')
    notice = address(symbols, 'function_2d97d0')
    clock = address(symbols, 'function_3724f0')
    hash_entry = address(symbols, 'function_31c540')
    warnings = {address(symbols, 'g_415460'): 0x415460,
                address(symbols, 'g_4154d8'): 0x4154d8}
    for seed in range(256):
        rng = random.Random(seed)
        salt = rng.randbytes(63) + b'\0'
        mask = rng.getrandbits(32)
        now = [1600000000, 0, -86400, 0x7ffffffe][seed % 4]
        days = [-1, 0, 1, 2, 179, 180, 181][seed % 7]
        expiry = (now + days * 86400) & 0xffffffff
        encoded = expiry ^ mask
        suffix = bytes(65 + ((encoded >> (4 * i)) & 15) for i in range(8))
        label = [b'TST', b'', b'T', b'TXT'][seed % 4]
        text = b'example-' + label + b'-' + suffix + b'\0'
        if seed % 11 == 0: text = b'no separators\0'
        if seed % 13 == 0: text = b'one-separator\0'
        if seed % 17 == 0: text = b'example-TST-\x80\xffABCDEFG\0'
        data = text.ljust(65536, b'\xcd')
        initial_gate = [0, 1, 7][seed % 3]
        values = {0x46fbf0: struct.pack('<I', HEAP), 0x415440: struct.pack('<I', mask),
                  0x415444: b'TST\0', 0x415448: salt, 0x480c60: bytes([initial_gate])}
        for uc, entries, h in [(retail, {va: va for va in globals_}, 0x31c540),
                               (compiled, mapped, hash_entry)]:
            uc.mem_write(HEAP, data)
            for va, value in values.items(): uc.mem_write(entries[va], value)
            digest = execute(uc, h, [HEAP])
            if uc is retail:
                retail_digest = digest
                expected_hash = 0 if seed % 3 == 0 else digest ^ (seed % 3 == 2)
            else:
                assert digest == retail_digest, (seed, 'validation input hash')
            uc.mem_write(entries[0x445470], struct.pack('<I', expected_hash))
        outcomes = []
        for uc, entry, time_entry, warning_entry, gate, remap in [
                (retail, 0x31c7d0, 0x3724f0, 0x2d97d0, 0x480c60, {}),
                (compiled, function, clock, notice, mapped[0x480c60], warnings)]:
            calls = []

            def hook(machine, target, size, _):
                sp = machine.reg_read(UC_X86_REG_ESP)
                ret, arg = struct.unpack('<II', machine.mem_read(sp, 8))
                if target == time_entry:
                    calls.append(('clock',))
                    machine.reg_write(UC_X86_REG_EAX, now & 0xffffffff)
                else:
                    calls.append(('notice', remap.get(arg, arg)))
                machine.reg_write(UC_X86_REG_ESP, sp + 4)
                machine.reg_write(UC_X86_REG_EIP, ret)

            hooks = [uc.hook_add(UC_HOOK_CODE, hook, begin=a, end=a) for a in (time_entry, warning_entry)]
            try: execute(uc, entry)
            finally:
                for handle in hooks: uc.hook_del(handle)
            outcomes.append((bytes(uc.mem_read(gate, 1)), calls, bytes(uc.mem_read(HEAP, 65536))))
        assert outcomes[0] == outcomes[1], (seed, outcomes[0][:2], outcomes[1][:2])
