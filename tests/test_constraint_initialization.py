"""Optional retail differential checks for constraint initialization and import.

Uses the same local retail image, full build and Unicorn setup as the solver
tests. Inputs and callback responses are synthetic; no executable bytes are
stored here. Run on Windows with pytest -p no:faulthandler.
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

OBJECT, SOURCE, SHAPE, TABLE = (HEAP + 256, HEAP + 1024, HEAP + 1536, HEAP + 2048)
CALLBACK = STOP + 128
CASES = {
    'entity': (0x314710, '??0c_314710@@QAE@PAVhkEntity@@@Z'),
    'serialized': (0x3147c0, '??0c_314710@@QAE@PBUs_314320@@@Z'),
    'snapshot': (0x314bb0, '??0c_component_joint_snapshot_5@@QAE@XZ'),
    'import': (0x314cd0, '?import_fields@lane_a_prismatic@@'),
}


def symbol_address(symbols, prefix):
    hits = {s.va for s in symbols.symbols if s.name.startswith(prefix)}
    assert len(hits) == 1, ('rebuild the full image', prefix, hits)
    return hits.pop()


def execute(uc, entry, kind, seed):
    # The /GX constructor links its exception frame through fs:[0]. Unicorn's
    # flat segment base is zero; provide the thread's initial chain terminator.
    if not any(lo == 0 for lo, hi, permissions in uc.mem_regions()):
        uc.mem_map(0, 4096)
    uc.mem_write(0, struct.pack('<I', 0xffffffff))
    rng = random.Random(seed)
    data = bytearray(rng.getrandbits(8) for _ in range(4096))
    for offset in range(0, 160, 4):
        struct.pack_into('<f', data, SOURCE - HEAP + offset, rng.uniform(-20, 20))
    struct.pack_into('<I', data, SHAPE - HEAP, TABLE)
    struct.pack_into('<I', data, TABLE - HEAP + 0x18, CALLBACK)
    struct.pack_into('<I', data, TABLE - HEAP + 0x1c, CALLBACK)
    if kind == 'entity':
        struct.pack_into('<I', data, SOURCE - HEAP + 12, SHAPE if seed % 2 else 0)
    if kind == 'import':
        struct.pack_into('<I', data, OBJECT - HEAP, TABLE)
    uc.mem_write(HEAP, bytes(data))
    observed = []

    def callback(machine, address, size, user):
        sp = machine.reg_read(UC_X86_REG_ESP)
        ret = struct.unpack('<I', machine.mem_read(sp, 4))[0]
        this = machine.reg_read(UC_X86_REG_ECX)
        if kind == 'entity':
            transform, tolerance, output = struct.unpack('<III', machine.mem_read(sp + 4, 12))
            observed.append((this, tolerance, bytes(machine.mem_read(transform, 64))))
            # Include degenerate boxes and a nonzero fourth lane, which must
            # not contribute to the squared radius.
            low = [-seed, seed / 4, -2, 100]
            high = low if seed % 5 == 0 else [seed / 2, seed + 1, 7, 200]
            machine.mem_write(output, struct.pack('<8f', *(low + high)))
            consumed = 12
        else:
            assert kind == 'import'
            result = struct.unpack('<I', machine.mem_read(sp + 4, 4))[0]
            observed.append((this, bytes(machine.mem_read(OBJECT, 0xe0))))
            machine.mem_write(result, bytes([seed % 2]))
            machine.reg_write(UC_X86_REG_EAX, result)
            consumed = 4
        machine.reg_write(UC_X86_REG_ESP, sp + 4 + consumed)
        machine.reg_write(UC_X86_REG_EIP, ret)

    hook = uc.hook_add(UC_HOOK_CODE, callback, begin=CALLBACK, end=CALLBACK)
    args = [] if kind == 'snapshot' else [SOURCE]
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    uc.reg_write(UC_X86_REG_ECX, OBJECT)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, 0x37f)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    try:
        uc.emu_start(entry, STOP, count=10000)
    finally:
        uc.hook_del(hook)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert bytes(uc.mem_read(0, 4)) == b'\xff' * 4
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 * (len(args) + 1)
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    if kind != 'import':
        assert uc.reg_read(UC_X86_REG_EAX) == OBJECT
    assert len(observed) == (seed % 2 if kind == 'entity' else int(kind == 'import'))
    output = bytearray(uc.mem_read(HEAP, len(data)))
    if kind != 'import':
        # Image-relative vtable addresses differ; validate the motion table
        # separately below, then compare every other byte, including padding.
        output[OBJECT - HEAP:OBJECT - HEAP + 4] = b'\0' * 4
    return bytes(output), observed


@pytest.mark.parametrize('kind', CASES)
def test_constraint_initialization_agrees_with_retail(solver_images, kind):
    retail, compiled, symbols = solver_images
    retail_va, prefix = CASES[kind]
    compiled_va = symbol_address(symbols, prefix)
    for seed in range(32):
        expected = execute(retail, retail_va, kind, seed)
        actual = execute(compiled, compiled_va, kind, seed)
        assert actual == expected, (kind, seed)
    if kind in ('entity', 'serialized'):
        table = struct.unpack('<I', compiled.mem_read(OBJECT, 4))[0]
        assert table == symbol_address(symbols, '??_7c_314710@@6B@')
        assert struct.unpack('<I', retail.mem_read(OBJECT, 4))[0] == 0x414abc
        for slot, name in [(3, 'c_314710::v3'), (5, 'c_314710::function_3144b0'),
                           (6, 'c_314710::function_314580'), (7, 'c_314710::kind')]:
            target = struct.unpack('<I', compiled.mem_read(table + 4 * slot, 4))[0]
            assert target in {s.va for s in symbols.find(name)}, (slot, name)
