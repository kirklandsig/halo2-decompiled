"""Retail contact queries with real transformed-segment geometry."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

A, B, SHAPES, PLACEMENTS, INPUT, COLLECTOR, CALLBACK = [
    HEAP, HEAP + 0x100, HEAP + 0x200, HEAP + 0x400,
    HEAP + 0x800, HEAP + 0x900, STOP + 16]


@pytest.mark.parametrize('geometry', ['crossed', 'clamped', 'parallel', 'point', 'nonfinite', 'alias'])
@pytest.mark.parametrize('precision', [0x007f, 0x027f, 0x037f])
@pytest.mark.parametrize('va,name', [(0x2f3920, 'capsule_pair_2f3920::function_2f3920'),
                                   (0x2f3d00, 'function_2f3d00')])
def test_capsule_contacts_against_retail(solver_images, precision, va, name, geometry):
    retail, compiled, symbols = solver_images
    entry = {s.va for s in symbols.find(name)}
    assert len(entry) == 1
    globals_ = [0x480154, next(s.va for s in symbols.find('g_480154'))]
    limits = [0x480158, next(s.va for s in symbols.find('g_480158'))]
    paths, axes, admissions = set(), set(), set()
    for seed in range(96):
        rng = random.Random(seed)
        data = bytearray(b'\xcd' * 65536)
        def put(at, fmt, *values):
            struct.pack_into('<' + fmt, data, at - HEAP, *values)
        for index, body in enumerate((A, B)):
            shape, placement = SHAPES + 0x100 * index, PLACEMENTS + 0x100 * index
            put(body, '4I', shape, seed + index, placement, 0)
            put(shape + 12, 'f', [0.0, 0.1, 0.5, 2.0][(seed + index) % 4])
            ends = [(-1, 0, 0, 1), (1, 0, 0, 1)] if index == 0 else [(0, -1, 0, 1), (0, 1, 0, 1)]
            if geometry in ('parallel', 'alias'):
                direction = [(0, 2, 3), (2, 0, 3), (2, 3, 0)][seed % 3]
                ends = [tuple(-v for v in direction) + (1,), direction + (1,)]
            elif geometry == 'point':
                point = (0, 0, 0, 1)
                ends = [point, point] if index == 0 or seed % 2 else ends
            elif geometry == 'nonfinite':
                ends = [list(point) for point in ends]
                ends[seed % 2][(seed // 2) % 4] = [float('nan'), float('inf'), -float('inf')][seed % 3]
            for j, point in enumerate(ends):
                put(shape + 16 + 16 * j, '4f', *point)
            for j, axis in enumerate([(1, 0, 0, 0), (0, 1, 0, 0), (0, 0, 1, 0)]):
                put(placement + 32 + 16 * j, '4f', *axis)
            put(placement + 80, '4f', rng.uniform(-0.5, 0.5), rng.uniform(-0.5, 0.5),
                (0 if index == 0 else [0.01, 0.5, 3.0][seed % 3]), 0)
            if geometry == 'clamped':
                put(placement + 80, '4f', index * [1.0, 2.0, 8.0][seed % 3],
                    index * [2.0, 0.0, -3.0][(seed // 3) % 3], 0, 0)
            elif geometry in ('parallel', 'point', 'alias'):
                put(placement + 80, '4f', 0, 0, index * [0.0, 0.125, 8.0, 0.0][seed % 4], 0)
            if geometry != 'crossed' and seed % 8 == 7:
                # Rigid rotations, shared by parallel bodies, with nontrivial translations.
                for j, axis in enumerate([(0, 1, 0, 0), (-1, 0, 0, 0), (0, 0, 1, 0)]):
                    put(placement + 32 + 16 * j, '4f', *axis)
        if geometry == 'alias':
            if seed % 3 != 0:
                put(B, 'I', SHAPES)
            if seed % 3 != 1:
                put(B + 8, 'I', PLACEMENTS)
        put(INPUT + 8, 'f', [0.0, 0.125, -0.25, float('nan')][seed % 4])
        put(COLLECTOR, 'I', COLLECTOR + 16)
        put(COLLECTOR + 20, 'I', CALLBACK)
        results = []
        for index, (uc, start) in enumerate([(retail, va), (compiled, entry.copy().pop())]):
            uc.mem_write(HEAP, bytes(data))
            uc.mem_write(globals_[index], struct.pack('<I', 0))
            uc.mem_write(limits[index], struct.pack('<I', 0))
            events = []
            def hook(machine, pc, size, unused):
                if index == 0:
                    offset = pc - va
                    if offset in (0x1bf, 0x1cc, 0x282):
                        paths.add(offset)
                    if offset == 0x2d4:
                        axes.add(machine.reg_read(r.UC_X86_REG_ECX))
                if pc != CALLBACK:
                    return
                sp = machine.reg_read(r.UC_X86_REG_ESP)
                ret, contact = struct.unpack('<2I', machine.mem_read(sp, 8))
                assert machine.reg_read(r.UC_X86_REG_ECX) == COLLECTOR
                payload = bytes(machine.mem_read(contact, 40))
                assert struct.unpack('<2I', payload[32:]) == (A, B)
                events.append(payload)
                machine.reg_write(r.UC_X86_REG_EAX, 0xabcdef01)
                machine.reg_write(r.UC_X86_REG_ECX, 0x87654321)
                machine.reg_write(r.UC_X86_REG_EDX, 0x10203040)
                machine.reg_write(r.UC_X86_REG_EIP, ret)
                machine.reg_write(r.UC_X86_REG_ESP, sp + 8)
            handle = uc.hook_add(UC_HOOK_CODE, hook)
            try:
                sp = STACK + 32768
                uc.mem_write(STACK, b'\xcd' * 65536)
                uc.mem_write(sp, struct.pack('<5I', STOP, A, B, INPUT, COLLECTOR))
                saved = [r.UC_X86_REG_EBX, r.UC_X86_REG_ESI, r.UC_X86_REG_EDI, r.UC_X86_REG_EBP]
                for reg in saved:
                    uc.reg_write(reg, 0x12345678)
                uc.reg_write(r.UC_X86_REG_ECX, HEAP + 0xf000)
                uc.reg_write(r.UC_X86_REG_ESP, sp)
                uc.reg_write(r.UC_X86_REG_EFLAGS, 2)
                uc.reg_write(r.UC_X86_REG_FPCW, precision)
                uc.reg_write(r.UC_X86_REG_FPTAG, 0xffff)
                uc.reg_write(r.UC_X86_REG_FPSW, 0)
                uc.reg_write(r.UC_X86_REG_MXCSR, 0x1f80)
                uc.emu_start(start, STOP, count=100000)
                assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
                assert uc.reg_read(r.UC_X86_REG_ESP) == sp + (20 if va == 0x2f3920 else 4)
                assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
                assert uc.reg_read(r.UC_X86_REG_FPCW) == precision
                results.append((bytes(uc.mem_read(HEAP, 65536)), events))
            finally:
                uc.hook_del(handle)
        assert results[0] == results[1], (hex(va), seed, precision, results[0][1], results[1][1])
        admissions.add(bool(results[0][1]))
    if geometry == 'crossed':
        assert admissions == {False, True}
        assert 0x1cc in paths
    elif geometry == 'clamped':
        assert admissions == {False, True}
        assert 0x1bf in paths
    elif geometry == 'parallel':
        assert 0x282 in paths
        assert axes == {0, 1, 2}
    elif geometry == 'point':
        assert 0x282 in paths
