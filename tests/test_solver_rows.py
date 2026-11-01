"""Optional differential execution of the solver builders against retail.

Requires Unicorn and a full checker build. HALO2_SOLVER_TEST_IMAGE can select
an alternate PE image with a matching .map file. No executable bytes are kept
in the tests; inputs are deterministic synthetic body/contact configurations.
On Windows, use pytest -p no:faulthandler for Unicorn's native JIT mapping.
"""
import math
import os
import random
import struct
from pathlib import Path
import pytest
pytest.importorskip('unicorn')
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import (
    UC_X86_REG_EAX, UC_X86_REG_EBX, UC_X86_REG_ECX, UC_X86_REG_EDX,
    UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_FPSW, UC_X86_REG_MXCSR,
)
from xbe import Xbe, ROOT, retail_xbe_path
from pe import Pe
from linkmap import LinkMap
HEAP, STACK, STOP = (0x10000000, 0x20000000, 0x30000000)
PAIR, INPUT, OUTPUT = (HEAP + 1024, HEAP + 1280, HEAP + 1792)
TERMS, OPS = (HEAP + 4096, HEAP + 8192)

def machine(img):
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    lo = min((s.va for s in img.sections)) & ~4095
    hi = max((s.va + max(s.vsize, s.rsize) for s in img.sections)) + 4095 & ~4095
    uc.mem_map(lo, hi - lo)
    for s in img.sections:
        uc.mem_write(s.va, img.data[s.raw:s.raw + s.rsize])
    uc.mem_map(HEAP, 65536)
    uc.mem_map(STACK, 65536)
    uc.mem_map(STOP, 4096)
    return uc
NAMES = {
    0x31e7b0: 'build_angular_error_31e7b0',
    0x31e900: 'build_angular_array_31e900',
    0x31ea90: 'build_linear_velocity_31ea90',
    0x31ec30: 'build_linear_axis_31ec30',
    0x31ee10: 'build_linear_range_31ee10',
    0x31f000: 'build_angular_range_31f000',
    0x31f160: 'build_transformed_points_31f160',
    0x31f3e0: 'build_point_axes_31f3e0',
    0x31f620: 'lane_b_contact_build',
}


def case(va, seed):
    rng = random.Random(seed)
    data = bytearray(65536)

    def put(address, fmt, *values):
        struct.pack_into('<' + fmt, data, address - HEAP, *values)

    def vector(address, values=None):
        put(address, '4f', *(values if values is not None else [rng.uniform(-3, 3) for _ in range(4)]))
    def rotation():
        first = [rng.uniform(-1, 1) for _ in range(3)]
        norm = math.sqrt(sum(v*v for v in first))
        first = [v/norm for v in first]
        second = [rng.uniform(-1, 1) for _ in range(3)]
        projection = sum(a*b for a,b in zip(first,second))
        second = [b-projection*a for a,b in zip(first,second)]
        norm = math.sqrt(sum(v*v for v in second))
        second = [v/norm for v in second]
        third = [first[1]*second[2]-first[2]*second[1],
                 first[2]*second[0]-first[0]*second[2],
                 first[0]*second[1]-first[1]*second[0]]
        return [column+[0] for column in [first,second,third]]
    for j in range(2):
        body = HEAP + j * 256
        put(body + 12, 'B', seed >> j & 1)
        vector(body + 48, [rng.uniform(0.05, 2) for _ in range(4)] if seed % 11 else [0] * 4)
        vector(body + 64)
        for k,column in enumerate(rotation()):
            vector(body+0x50+16*k,column)
    put(PAIR, 'II2f', HEAP, HEAP + 256, rng.uniform(0.1, 3), rng.uniform(0.1, 3))
    for off in range(0, 256, 16):
        vector(INPUT + off)
    put(OUTPUT, 'II', TERMS, OPS)
    data[TERMS - HEAP:TERMS - HEAP + 4096] = b'\xcd' * 4096
    data[OPS - HEAP:OPS - HEAP + 4096] = b'\xcd' * 4096
    args = [INPUT, PAIR, OUTPUT]
    if va == 0x31e900:
        count = [-2, 0, 1, 2, 3, 6][seed % 6]
        put(INPUT, 'IIfl', HEAP + 12288, HEAP + 12800, rng.uniform(-3, 3), count)
        for j in range(max(0, count)):
            vector(HEAP + 12288 + j * 16)
            put(HEAP + 12800 + j * 8, '2f', rng.uniform(-3, 3), rng.uniform(-3, 3))
    if va == 0x31ea90:
        put(INPUT + 36, 'I', HEAP + 3072)
        put(HEAP + 3072, '2f', rng.uniform(-3, 3), rng.uniform(-3, 3))
    if va == 0x31f160:
        for j in range(2):
            transform = HEAP + 2048 + j * 256
            for k,column in enumerate(rotation()):
                vector(transform+16*k,column)
            vector(transform + 48, [rng.uniform(-3, 3) for _ in range(3)] + [1])
        args = [HEAP + 2048, HEAP + 2304, INPUT, INPUT + 16, PAIR, OUTPUT]
    if va == 0x31f3e0:
        args = [INPUT, INPUT + 16, PAIR, OUTPUT]
    if va == 0x31f620:
        count = [0, 1, 2, 3, 4, 6][seed % 6]
        points, properties, state = (HEAP + 12288, HEAP + 12800, HEAP + 13568)
        put(PAIR + 16, '4f', [-0.05, 0, 0.001, 0.5][seed % 4], 0, rng.uniform(0, 0.7), rng.uniform(0.1, 2))
        put(state, 'I', 4 if seed % 3 else 0)
        for off in [8, 16, 24]:
            put(state + off, 'f', rng.uniform(-1, 1))
        put(state + 28, 'f', rng.uniform(0.1, 2))
        vector(state + 32, [0, 0, 0, 0] if seed % 2 else [0, 1, 0, 0])
        vector(state + 48, [0, 0, 0, 0])
        direction = [rng.uniform(-1, 1) for _ in range(3)]
        length = math.sqrt(sum((v * v for v in direction)))
        direction = [v / length for v in direction]
        for i in range(count):
            vector(points + 32 * i, [0, 0, 0, 1] if seed % 17 == 0 else [rng.uniform(-2, 2) for _ in range(4)])
            normal = direction[:]
            if seed % 13 == 0:
                normal = [0, 0, 0]
            elif seed % 7 == 0:
                normal = [v * (-1 if i % 2 else 1) for v in normal]
            elif seed % 5 == 0:
                normal = [rng.uniform(-1, 1) for _ in range(3)]
                length = math.sqrt(sum((v * v for v in normal)))
                normal = [v / length for v in normal]
            vector(points + 32 * i + 16, normal + [rng.uniform(-1, 1)])
            impulse = 0 if seed % 11 == 0 else rng.uniform(0.1, 3)
            friction = -0.3 if seed % 19 == 0 else rng.uniform(0.1, 1)
            put(properties + 28 * i, '6fHH', impulse, rng.uniform(-1, 1), friction, 0, 0, rng.uniform(-1, 0.1), 1 if i > 0 and i % 2 and seed % 2 else 0, 0)
        args = [points, count, properties, state, PAIR, OUTPUT]
    return (data, args)

def execute(uc, entry, data, args, precision):
    uc.mem_write(HEAP, bytes(data))
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    for reg in [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]:
        uc.reg_write(reg, 0)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    uc.emu_start(entry, STOP, count=100000)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4
    assert all((uc.reg_read(reg) == 0 for reg in [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]))
    return bytes(uc.mem_read(HEAP, len(data)))

@pytest.fixture(scope='module')
def solver_images():
    retail_path = retail_xbe_path()
    if not Path(retail_path).exists():
        pytest.skip('retail XBE not present')
    image_path = Path(os.environ.get('HALO2_SOLVER_TEST_IMAGE', str(Path(ROOT) / 'build/halo2.exe')))
    if not image_path.exists() or not image_path.with_suffix('.map').exists():
        pytest.skip('run the full checker to build the solver test image')
    return (machine(Xbe(retail_path)), machine(Pe(image_path)),
            LinkMap.read(image_path.with_suffix('.map')))

@pytest.mark.parametrize('retail_va,name', NAMES.items(), ids=lambda value: hex(value) if isinstance(value, int) else value)
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_solver_outputs_agree_with_retail(solver_images, retail_va, name, precision):
    retail_machine, compiled_machine, symbols = solver_images
    hits = symbols.find(name)
    assert len(hits) == 1, 'rebuild the image with the solver sources'
    for seed in range(64):
        data, args = case(retail_va, seed)
        retail = execute(retail_machine, retail_va, data, args, precision)
        compiled = execute(compiled_machine, hits[0].va, data, args, precision)
        ro = struct.unpack_from('<II', retail, OUTPUT - HEAP)
        co = struct.unpack_from('<II', compiled, OUTPUT - HEAP)
        assert ro == co, (hex(retail_va), seed, precision, 'cursors', ro, co)
        assert TERMS <= ro[0] <= TERMS + 4096
        assert OPS <= ro[1] <= OPS + 4096
        rb = retail[TERMS - HEAP:ro[0] - HEAP] + retail[OPS - HEAP:ro[1] - HEAP]
        cb = compiled[TERMS - HEAP:co[0] - HEAP] + compiled[OPS - HEAP:co[1] - HEAP]
        if retail_va != 0x31f620:
            assert rb == cb, (hex(retail_va), seed, precision)
        else:
            count = args[1]
            assert retail[13568:13572] == compiled[13568:13572]
            for i in range(count):
                start = 12800 + 28 * i
                assert retail[start:start + 20] == compiled[start:start + 20]
                assert retail[start + 24:start + 28] == compiled[start + 24:start + 28]
            rb += retail[12800:12800 + 28 * count] + retail[13568:13632]
            cb += compiled[12800:12800 + 28 * count] + compiled[13568:13632]
            for offset in range(0, len(rb), 4):
                if rb[offset:offset + 4] == cb[offset:offset + 4]:
                    continue
                rf = struct.unpack_from('<f', rb, offset)[0]
                cf = struct.unpack_from('<f', cb, offset)[0]
                assert math.isclose(rf, cf, rel_tol=3e-06, abs_tol=3e-06), (seed, precision, offset, rf, cf)
        cursor = OPS - HEAP
        while cursor < ro[1] - HEAP:
            opcode = struct.unpack_from('<I', retail, cursor)[0]
            assert struct.unpack_from('<I', compiled, cursor)[0] == opcode
            size = opcode >> 8 & 255
            assert size >= 4 and cursor + size <= ro[1] - HEAP
            for word in range(1, size // 4):
                value = struct.unpack_from('<I', retail, cursor + 4 * word)[0]
                if HEAP <= value < HEAP + 65536 or (opcode == 0x11801 and word == 5) or (opcode in (0x51407, 0x61808) and word == 4):
                    assert retail[cursor + 4 * word:cursor + 4 * word + 4] == compiled[cursor + 4 * word:cursor + 4 * word + 4]
            cursor += size
        for start, end in [(ro[0] - HEAP, TERMS - HEAP + 4096), (ro[1] - HEAP, OPS - HEAP + 4096)]:
            assert retail[start:end] == compiled[start:end] == data[start:end]
