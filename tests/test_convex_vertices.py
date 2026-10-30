"""Execute the convex block queries directly against retail, including aliases."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

OBJECT, DIRECTION, INDICES, OUTPUT, BLOCKS = [
    HEAP, HEAP + 0x100, HEAP + 0x200, HEAP + 0x400, HEAP + 0x1000]


@pytest.mark.parametrize('precision', [0x007f, 0x027f, 0x037f])
@pytest.mark.parametrize('alias', [False, True])
@pytest.mark.parametrize('va,name', [
    (0x2fd260, 'convex_vertices_2fd260::function_2fd260'),
    (0x2fd430, 'convex_vertices_2fd260::function_2fd430')])
def test_convex_block_queries(solver_images, precision, alias, va, name):
    retail, compiled, symbols = solver_images
    entries = {s.va for s in symbols.find(name)}
    assert len(entries) == 1
    masks, selected_lanes, counts = set(), set(), set()
    for seed in range(128):
        rng = random.Random(seed)
        data = bytearray(b'\xcd' * 65536)

        def put(at, fmt, *values):
            struct.pack_into('<' + fmt, data, at - HEAP, *values)

        block_count = 2 if seed < 16 else [-1, 0, 1, 2, 3, 4, 7, 8][seed % 8]
        put(OBJECT + 0x30, 'Ii', BLOCKS, block_count)
        for block in range(8):
            for coordinate in range(3):
                values = [rng.uniform(-10, 10) for _ in range(4)]
                if seed < 16:
                    values = [0.0] * 4
                    if block == 1 and coordinate == 0:
                        values = [1.0 if seed & (1 << lane) else -1.0 for lane in range(4)]
                elif seed % 11 == 0:
                    values = [0.0, -0.0, 1.0, 1.0]
                elif seed % 7 == 0:
                    values[(seed + block + coordinate) % 4] = [float('nan'), float('inf'), -float('inf')][block % 3]
                put(BLOCKS + block * 48 + coordinate * 16, '4f', *values)
        direction = [1.0, 0.0, 0.0, 17.0] if seed < 16 else [rng.uniform(-3, 3) for _ in range(4)]
        if seed >= 16 and seed % 13 == 0:
            direction = [0.0, -0.0, 0.0, float('nan')]
        elif seed >= 16 and seed % 17 == 0:
            direction[seed % 3] = float('nan')
        put(DIRECTION, '4f', *direction)
        count = [-3, 0, 1, 2, 3, 4, 5, 7, 8, 9, 12, 15][seed % 12]
        counts.add(count)
        for ordinal in range(max(0, count)):
            put(INDICES + ordinal * 2, 'H', rng.randrange(32))
        destination = OUTPUT
        if alias:
            if va == 0x2fd260:
                destination = [DIRECTION, BLOCKS, BLOCKS + 16, BLOCKS + 32][seed % 4]
            else:
                # Output replaces coordinates that later indexed reads may use.
                destination = BLOCKS + (seed % 3) * 16
        args = [DIRECTION, destination] if va == 0x2fd260 else [INDICES, count & 0xffffffff, destination]
        results = []
        for index, (machine, entry) in enumerate([(retail, va), (compiled, entries.copy().pop())]):
            machine.mem_write(HEAP, bytes(data))
            machine.mem_write(STACK, b'\xcd' * 65536)
            sp = STACK + 32768
            machine.mem_write(sp, struct.pack('<' + 'I' * (1 + len(args)), STOP, *args))
            saved = [r.UC_X86_REG_EBX, r.UC_X86_REG_ESI, r.UC_X86_REG_EDI, r.UC_X86_REG_EBP]
            for reg in saved:
                machine.reg_write(reg, 0x12345678)
            machine.reg_write(r.UC_X86_REG_ECX, OBJECT)
            machine.reg_write(r.UC_X86_REG_ESP, sp)
            machine.reg_write(r.UC_X86_REG_EFLAGS, 2)
            machine.reg_write(r.UC_X86_REG_FPCW, precision)
            machine.reg_write(r.UC_X86_REG_FPTAG, 0xffff)
            machine.reg_write(r.UC_X86_REG_FPSW, 0)
            machine.reg_write(r.UC_X86_REG_MXCSR, 0x1f80)

            def hook(uc, pc, size, unused):
                if index == 0 and pc == 0x2fd300:
                    masks.add(uc.reg_read(r.UC_X86_REG_EDX))

            handle = machine.hook_add(UC_HOOK_CODE, hook)
            try:
                machine.emu_start(entry, STOP, count=100000)
            finally:
                machine.hook_del(handle)
            assert machine.reg_read(r.UC_X86_REG_EIP) == STOP
            assert machine.reg_read(r.UC_X86_REG_ESP) == sp + 4 * (1 + len(args))
            assert all(machine.reg_read(reg) == 0x12345678 for reg in saved)
            assert machine.reg_read(r.UC_X86_REG_FPCW) == precision
            assert machine.reg_read(r.UC_X86_REG_MXCSR) & 0xffc0 == 0x1f80
            results.append(bytes(machine.mem_read(HEAP, 65536)))
        assert results[0] == results[1], (hex(va), seed, precision, alias)
        if va == 0x2fd260:
            tag = struct.unpack_from('<I', results[0], destination - HEAP + 12)[0]
            assert tag & 0xffffffe0 == 0x3f000000
            selected_lanes.add(tag & 3)
    if va == 0x2fd260:
        assert masks == set(range(16))
        assert selected_lanes == set(range(4))
    else:
        assert {-3, 0, 1, 3, 4, 5, 7, 8, 9, 15} <= counts
