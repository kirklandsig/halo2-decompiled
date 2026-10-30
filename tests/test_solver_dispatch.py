"""Optional differential execution of the Havok operation-stream solver.

Requires the local retail XBE, a full checker build, and Unicorn. The external
validation call and operation callback use synthetic hooks. No retail executable
bytes are stored in the tests.
"""
import math
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
from test_solver_rows import (
    HEAP, STACK, STOP, TERMS, OPS, solver_images,  # noqa: F401
    NAMES as BUILDER_NAMES, OUTPUT as BUILDER_OUTPUT, case as builder_case,
)


def address(symbols, name):
    hits = {s.va for s in symbols.find(name)}
    assert len(hits) == 1, ('rebuild the full checker image', name, hits)
    return hits.pop()


def put(data, va, fmt, *values):
    struct.pack_into('<' + fmt, data, va - HEAP, *values)


def words(uc, va, count):
    return struct.unpack('<' + 'I' * count, uc.mem_read(va, 4 * count))


def return_from_hook(uc, argument_bytes=0):
    sp = uc.reg_read(UC_X86_REG_ESP)
    ret, = words(uc, sp, 1)
    uc.reg_write(UC_X86_REG_ESP, sp + 4 + argument_bytes)
    uc.reg_write(UC_X86_REG_EIP, ret)


def execute(uc, entry, data, args, this=None, hooks=(), precision=0x37f):
    if not any(lo == 0 for lo, hi, permissions in uc.mem_regions()):
        uc.mem_map(0, 4096)
    uc.mem_write(0, b'\xff' * 4)
    uc.mem_write(HEAP, bytes(data))
    sp = STACK + 32768
    uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
    saved = [UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP]
    for reg in saved + [UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX]:
        uc.reg_write(reg, 0x12345678)
    if this is not None:
        uc.reg_write(UC_X86_REG_ECX, this)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, precision)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_FPSW, 0)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    handles = [uc.hook_add(UC_HOOK_CODE, callback, begin=va, end=va) for va, callback in hooks]
    try:
        uc.emu_start(entry, STOP, count=100000)
    finally:
        for handle in handles:
            uc.hook_del(handle)
    assert uc.reg_read(UC_X86_REG_EIP) == STOP
    assert uc.reg_read(UC_X86_REG_ESP) == sp + 4 + (4 * len(args) if this is not None else 0)
    assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
    assert bytes(uc.mem_read(0, 4)) == b'\xff' * 4
    return bytes(uc.mem_read(HEAP, len(data))), uc.reg_read(UC_X86_REG_EAX)


STEP, RESULTS, IMPULSES = HEAP + 0x3000, HEAP + 0x4000, HEAP + 0x5000
FRICTION_RESULTS, CALLBACK = HEAP + 0x6000, STOP + 128
OP_LAYOUT = {
    2: (4, 3), 3: (8, 5), 4: (16, 4), 5: (16, 3),
    6: (8, 3), 7: (20, 5), 8: (24, 6), 9: (8, 4),
    10: (16, 4), 11: (12, 3), 12: (4, 4), 13: (4, 3),
}


def solver_case(kind, seed):
    rng = random.Random(seed)
    data = bytearray(b'\xcd' * 65536)
    for i in range(2):
        body = HEAP + 0x80 * i
        put(data, body, 'B', 2)
        for offset in [0x10, 0x20, 0x40, 0x50]:
            put(data, body + offset, '4f', *(rng.uniform(-0.5, 0.5) for _ in range(4)))
        put(data, body + 0x30, '4f', *(rng.uniform(0.1, 1.0) for _ in range(4)))
    put(data, HEAP + 0x100, 'B', 3)
    put(data, STEP, '7fi', 0, 0.6, 0.8, 0.3, 60, 0, 0, [-1, 0, 1, 2, 4][seed % 5])
    put(data, OPS, '6I', 0x11801, TERMS, RESULTS, HEAP, HEAP + 0x80, [8, 12, 28][seed % 3])
    op = OPS + 24
    # Exercise temporary scales and restoring the previous settings.
    if seed % 2:
        put(data, op, 'I2f', 0x20c0f, 0.4, 0.75)
        op += 12
    if seed % 7 == 0:
        put(data, op, '3I', 0x20c0e, CALLBACK, 3)
        op += 12
    terms, result_index = TERMS, 0
    size, row_kind = OP_LAYOUT[kind]
    repeat = 1 + seed % 3
    for j in range(repeat):
        put(data, op, 'I', kind | size << 8 | row_kind << 16)
        if size >= 8:
            put(data, op + 4, 'f', 0.2 if kind == 3 else [0.02, 1.0, 100.0][seed % 3])
        if size >= 12:
            put(data, op + 8, 'f', -0.4 if kind not in (7, 8) else 0.2)
        if kind in (10, 11):
            lower, upper = [(-0.4, 0.7), (0, 0), (0.7, -0.4)][seed % 3]
            put(data, op + 4, '2f', lower, upper)
        if kind == 10:
            put(data, op + 12, 'f', 0.5)
        if kind in (7, 8):
            put(data, op + 4, 'I', FRICTION_RESULTS + j * 64)
            put(data, op + 12, 'f', [0.02, 1, 100][seed % 3])
            put(data, op + 16, 'I', 12)
        if kind == 8:
            put(data, op + 20, 'f', 0.7)
        row_types = ['linear'] if row_kind == 3 else ['angular'] if row_kind == 4 else ['linear', 'linear']
        if row_kind == 6:
            row_types.append('angular')
        for row_type in row_types:
            vectors = 3 if row_type == 'linear' else 2
            for v in range(vectors):
                put(data, terms + v * 16, '4f', *(rng.uniform(-0.5, 0.5) for _ in range(4)))
            if row_type == 'linear':
                put(data, terms + 12, 'f', rng.uniform(-2, 2))
                put(data, terms + 28, 'f', 0.5)
                put(data, terms + 44, 'f', 0.6)
            else:
                put(data, terms + 12, 'f', 0.5)
                put(data, terms + 28, 'f', rng.uniform(-2, 2))
            put(data, IMPULSES + result_index * 4, 'f', rng.uniform(-0.1, 0.1))
            terms += vectors * 16
            result_index += 1
        op += size
    if seed % 2:
        put(data, op, 'I', 0x20410)
        op += 4
    put(data, op, 'I', 0x400)
    return data


@pytest.mark.parametrize('kind', list(OP_LAYOUT))
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_dispatch_operation_paths(solver_images, kind, precision):
    for seed in range(48):
        compare_case(solver_images, solver_case(kind, seed), precision, (kind, seed))


def float_outputs(data):
    """Only these words may differ by rounding; all other bytes must match."""
    offsets = set()
    for body in (0, 0x80):
        for field in (0x10, 0x20, 0x40, 0x50):
            offsets.update(range(body + field, body + field + 16, 4))
    op, impulse = OPS - HEAP, IMPULSES - HEAP
    while data[op]:
        kind, size, row_kind = data[op:op + 3]
        if kind == 1:
            terms, destination, _, _, stride = struct.unpack_from('<5I', data, op + 4)
            terms -= HEAP
            destination -= HEAP
        elif kind == 14:
            # The synthetic callback modifies the next row's first component.
            offsets.add(terms)
        elif kind in OP_LAYOUT:
            target, target_stride = destination, stride
            if kind in (7, 8):
                target = struct.unpack_from('<I', data, op + 4)[0] - HEAP
                target_stride = struct.unpack_from('<I', data, op + 16)[0]
            rows = (3,) if row_kind == 3 else (2,) if row_kind == 4 else (3, 3)
            if row_kind == 6:
                rows += (2,)
            for vectors in rows:
                offsets.update((terms + (12 if vectors == 3 else 28), impulse,
                                target, target + 4))
                terms += vectors * 16
                impulse += 4
                target += target_stride
            if kind not in (7, 8):
                destination = target
        op += size
    return offsets


def compare_case(solver_images, data, precision, context, retail_coverage=None):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'solve_operations_31c940')
    compiled_gate = address(symbols, 'g_480c60')
    snapshots, callbacks = [], []
    for uc, entry, gate in [(retail, 0x31c940, 0x480c60), (compiled, target, compiled_gate)]:
        uc.mem_write(gate, b'\x01')
        calls = []

        def callback(machine, address_, size_, user):
            sp = machine.reg_read(UC_X86_REG_ESP)
            ret, terms, payload = words(machine, sp, 3)
            calls.append((terms, payload, words(machine, payload, 1)[0]))
            value, = struct.unpack('<f', machine.mem_read(terms, 4))
            machine.mem_write(terms, struct.pack('<f', value * 0.75))
            return_from_hook(machine)

        hooks = [(CALLBACK, callback)]
        if uc is retail and retail_coverage is not None:
            def visit(machine, address_, size_, user):
                retail_coverage[address_] += 1
            hooks.extend((va, visit) for va in retail_coverage)
        snapshots.append(execute(uc, entry, data, [STEP, OPS, HEAP, IMPULSES],
                                 hooks=hooks, precision=precision))
        callbacks.append(calls)
    assert callbacks[0] == callbacks[1], (context, precision, 'callbacks')
    expected, actual = snapshots
    assert expected[1] == actual[1] == 1
    outputs = float_outputs(data)
    for offset in range(0, len(data), 4):
        rb, cb = expected[0][offset:offset + 4], actual[0][offset:offset + 4]
        if rb == cb:
            continue
        rf, cf = struct.unpack('<f', rb)[0], struct.unpack('<f', cb)[0]
        # Only float outputs admit rounding differences. Pointers, cursor fields,
        # unused record fields and every guard byte must be identical.
        close = math.isclose(rf, cf, rel_tol=2e-5, abs_tol=2e-6)
        assert offset in outputs and (close or math.isnan(rf) and math.isnan(cf)), (
            context, precision, hex(offset), rf, cf)
    return snapshots


@pytest.mark.parametrize('kind', [4, 9, 10, 12])
@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_angular_impulse_retains_x87_remainder(solver_images, kind, precision):
    # SIMD consumes a float copy of the impulse, while accumulation keeps the
    # x87 value. Cancellation makes premature rounding observable without
    # changing the suite's existing tolerances.
    for exponent in (4, 12, 20, 28):
        for sign in (-1, 1):
            for axis in range(3):
                for active_body in range(2):
                    data = solver_case(kind, 2)
                    large, small = sign * 2.0 ** exponent, sign * 2.0 ** (exponent - 25)
                    put(data, STEP, '7fi', 0, 1, 1, 1, 60, 0, 0, 1)
                    size, row_kind = OP_LAYOUT[kind]
                    put(data, OPS + 24, 'I', kind | size << 8 | row_kind << 16)
                    if kind == 4:
                        put(data, OPS + 28, '2f', abs(large) * 2, -abs(large) * 2)
                    elif kind == 9:
                        put(data, OPS + 28, 'f', abs(large) * 2)
                    elif kind == 10:
                        put(data, OPS + 28, '3f', 0, 0, 1)
                    put(data, OPS + 24 + size, 'I', 0x400)
                    alias = (axis + exponent // 4) % 2
                    put(data, OPS + 16, 'I', HEAP if alias else HEAP + 0x80)
                    for body in (HEAP, HEAP + 0x80):
                        for offset in (0x10, 0x20, 0x30, 0x40, 0x50):
                            put(data, body + offset, '4f', 0, 0, 0, 0)
                        put(data, body + 0x30 + axis * 4, 'f', 0.25 if axis % 2 else 0)
                    put(data, TERMS, '8f', 0, 0, 0, 1, 0, 0, 0, large)
                    put(data, TERMS + active_body * 16 + axis * 4, 'f', 1)
                    body = HEAP if alias or not active_body else HEAP + 0x80
                    put(data, body + 0x20 + axis * 4, 'f', -small)
                    put(data, IMPULSES, 'f', -large)
                    context = ('angular cancellation', kind, exponent, sign, axis, active_body)
                    snapshots = compare_case(solver_images, data, precision, context)
                    expected = small if precision != 0x7f else 0.0
                    # The positive limit branch reloads its error from a float
                    # spill before multiplying by inverse mass in retail.
                    if kind == 10 and sign > 0:
                        expected = 0.0
                    for heap, _ in snapshots:
                        actual, = struct.unpack_from('<f', heap, IMPULSES - HEAP)
                        assert actual == expected, (context, precision, actual, expected)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_spin_impulse_rounds_after_scale(solver_images, precision):
    # Spin friction has a distinct boundary: its scaled impulse is rounded
    # before both the SIMD update and accumulation. Keeping it extended would
    # incorrectly leave a remainder in these exact-cancellation cases.
    for exponent in (12, 16, 20):
        for sign in (-1, 1):
            for axis in range(3):
                for alias in (False, True):
                    data = solver_case(8, 2)
                    put(data, STEP, '7fi', 0, 1, 1, 1, 60, 0, 0, 1)
                    put(data, OPS + 16, 'I', HEAP if alias else HEAP + 0x80)
                    put(data, OPS + 24, 'IIffIf',
                        0x61808, FRICTION_RESULTS, 0, 1e9, 12, 1 + 2.0 ** -exponent)
                    put(data, OPS + 48, 'I', 0x400)
                    for body in (HEAP, HEAP + 0x80):
                        for offset in (0x10, 0x20, 0x30, 0x40, 0x50):
                            put(data, body + offset, '4f', 0, 0, 0, 0)
                        put(data, body + 0x30 + axis * 4, 'f', 0.25)
                    put(data, TERMS, '32f', *([0] * 32))
                    put(data, TERMS + 0x60 + axis * 4, 'f', 1)
                    put(data, TERMS + 0x6c, 'f', 1)
                    put(data, TERMS + 0x7c, 'f', sign * (2.0 ** exponent + 1))
                    put(data, IMPULSES, '3f', 0, 0, -sign * (2.0 ** exponent + 2))
                    context = ('spin rounding', exponent, sign, axis, alias)
                    snapshots = compare_case(solver_images, data, precision, context)
                    for heap, _ in snapshots:
                        actual, = struct.unpack_from('<f', heap, IMPULSES - HEAP + 8)
                        assert actual == 0.0, (context, precision, actual)


def mixed_case(seed):
    data = solver_case(12, seed)
    for start, end in ((0x1000, 0x3000), (0x4000, 0x7000)):
        data[start:end] = b'\xcd' * (end - start)
    order = list(OP_LAYOUT)
    random.Random(seed).shuffle(order)
    op, terms, impulse = OPS, TERMS, IMPULSES
    for section, kind in enumerate(order):
        source = solver_case(kind, seed + section)
        if section == 0 or order[section - 1] == 8 or seed % 2:
            a, b = (HEAP, HEAP + 0x80) if section % 2 else (HEAP + 0x80, HEAP)
            put(data, op, '6I', 0x11801, terms, RESULTS + section * 256, a, b,
                [8, 12, 28][seed % 3])
            op += 24
        cursor, source_terms, source_impulse = OPS + 24, TERMS, IMPULSES
        while source[cursor - HEAP]:
            code, size, row_kind = source[cursor - HEAP:cursor - HEAP + 3]
            data[op - HEAP:op - HEAP + size] = source[cursor - HEAP:cursor - HEAP + size]
            if code in OP_LAYOUT:
                if code in (7, 8):
                    old_result, = struct.unpack_from('<I', source, cursor - HEAP + 4)
                    put(data, op + 4, 'I', old_result + section * 256)
                rows = (3,) if row_kind == 3 else (2,) if row_kind == 4 else (3, 3)
                if row_kind == 6:
                    rows += (2,)
                length = sum(rows) * 16
                data[terms - HEAP:terms - HEAP + length] = source[source_terms - HEAP:source_terms - HEAP + length]
                length_impulses = len(rows) * 4
                data[impulse - HEAP:impulse - HEAP + length_impulses] = source[source_impulse - HEAP:source_impulse - HEAP + length_impulses]
                terms += length
                source_terms += length
                impulse += length_impulses
                source_impulse += length_impulses
            cursor += size
            op += size
    assert terms < OPS and op < STEP
    put(data, op, 'I', 0x400)
    return data


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_mixed_streams_and_body_pairs(solver_images, precision):
    for seed in range(96):
        compare_case(solver_images, mixed_case(seed), precision, ('mixed', seed))


def contact_group_case(pair_count, seed):
    """Consecutive normal pairs share bodies and end in three-axis friction."""
    data = solver_case(3, seed)
    for start, end in ((0x1000, 0x3000), (0x4000, 0x7000)):
        data[start:end] = b'\xcd' * (end - start)
    put(data, STEP + 28, 'i', [1, 2, 4][seed % 3])
    put(data, OPS, '6I', 0x11801, TERMS, RESULTS, HEAP,
        HEAP if seed % 7 == 0 else HEAP + 0x80, 12)
    op, terms, impulse = OPS + 24, TERMS, IMPULSES
    errors = [(1, 1), (-1, 1), (1, -1), (-1, -1),
              (math.nan, 1), (1, math.nan), (0, 0), (1e-30, 1)]
    for index, kind in enumerate([3] * pair_count + [8]):
        source = solver_case(kind, 2)
        size, row_kind = OP_LAYOUT[kind]
        source_op = OPS + 24
        data[op - HEAP:op - HEAP + size] = source[source_op - HEAP:source_op - HEAP + size]
        length, count = (96, 2) if kind == 3 else (128, 3)
        data[terms - HEAP:terms - HEAP + length] = source[TERMS - HEAP:TERMS - HEAP + length]
        data[impulse - HEAP:impulse - HEAP + count * 4] = source[IMPULSES - HEAP:IMPULSES - HEAP + count * 4]
        if kind == 3:
            first, second = errors[(seed + index) % len(errors)]
            put(data, terms + 12, 'f', first)
            put(data, terms + 60, 'f', second)
            put(data, op + 4, 'f', [0, 0.2, -0.2, 1][seed % 4])
        else:
            put(data, op + 4, 'I', FRICTION_RESULTS)
            put(data, op + 12, 'f', [0, 0.02, 1, 100][seed % 4])
        op += size
        terms += length
        impulse += count * 4
    # A callback after the group observes the advanced term cursor.
    put(data, terms, 'f', 0.75)
    put(data, op, '3I', 0x20c0e, CALLBACK, pair_count)
    put(data, op + 12, 'I', 0x400)
    return data


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_contact_pair_groups_then_friction(solver_images, precision):
    # Local retail disassembly identifies the two pair entries, their four
    # outcomes (both, first, second, neither), and the following friction entry.
    # Coverage is measured only in retail; compiled outputs use compare_case.
    coverage = dict.fromkeys((0x31c9c6, 0x31cd52, 0x31d1fb,
                              0x31cadb, 0x31cc2b, 0x31ccb1, 0x31cd39,
                              0x31ce74, 0x31d0dc, 0x31d15a, 0x31d1e5), 0)
    for pair_count in range(1, 6):
        for seed in range(32):
            before = coverage.copy()
            compare_case(solver_images, contact_group_case(pair_count, seed),
                         precision, ('contact group', pair_count, seed), coverage)
            iterations = [1, 2, 4][seed % 3]
            assert coverage[0x31c9c6] - before[0x31c9c6] == (pair_count + 1) // 2 * iterations
            assert coverage[0x31cd52] - before[0x31cd52] == pair_count // 2 * iterations
            assert coverage[0x31d1fb] - before[0x31d1fb] == iterations
    assert all(coverage.values()), coverage


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_shared_body_in_both_header_slots(solver_images, precision):
    for kind in OP_LAYOUT:
        for seed in range(8):
            data = solver_case(kind, seed)
            put(data, OPS + 16, 'I', HEAP)
            compare_case(solver_images, data, precision, ('shared body', kind, seed))


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_nonfinite_and_zero_rows(solver_images, precision):
    for kind, (_, row_kind) in OP_LAYOUT.items():
        for value in (0.0, -0.0, math.inf, -math.inf, math.nan):
            data = solver_case(kind, 3)
            put(data, TERMS + (28 if row_kind == 4 else 12), 'f', value)
            compare_case(solver_images, data, precision, (kind, value))


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_step_scale_edge_values(solver_images, precision):
    # Seed 2 emits no temporary-scale override, so every operation consumes
    # the selected field directly. Keep one iteration to isolate that read.
    for kind in OP_LAYOUT:
        for offset in (4, 8, 12):
            for value in (0.0, -0.0, 1e-30, -0.5, math.inf, math.nan):
                data = solver_case(kind, 2)
                put(data, STEP + 0x1c, 'i', 1)
                put(data, STEP + offset, 'f', value)
                compare_case(solver_images, data, precision, ('step scale', kind, offset, value))


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_retail_builder_streams(solver_images, precision):
    retail, _, _ = solver_images
    for builder in BUILDER_NAMES:
        for seed in range(24):
            source, args = builder_case(builder, seed)
            generated, _ = execute(retail, builder, source, args, precision=precision)
            terms_end, ops_end = struct.unpack_from('<2I', generated, BUILDER_OUTPUT - HEAP)
            data = solver_case(12, seed)
            data[TERMS - HEAP:OPS - HEAP] = b'\xcd' * (OPS - TERMS)
            data[TERMS - HEAP:terms_end - HEAP] = generated[TERMS - HEAP:terms_end - HEAP]
            for body, builder_body in ((0, 0), (0x80, 0x100)):
                data[body + 0x30:body + 0x40] = source[builder_body + 0x30:builder_body + 0x40]
            put(data, OPS, '6I', 0x11801, TERMS, RESULTS, HEAP, HEAP + 0x80, 12)
            op, cursor, count = OPS + 24, OPS, 0
            while cursor < ops_end:
                code, size, row_kind = generated[cursor - HEAP:cursor - HEAP + 3]
                assert code == 1 or code in OP_LAYOUT
                if code != 1:
                    data[op - HEAP:op - HEAP + size] = generated[cursor - HEAP:cursor - HEAP + size]
                    if code in (7, 8):
                        put(data, op + 4, 'I', FRICTION_RESULTS)
                    count += 1 if row_kind in (3, 4) else 2 if row_kind == 5 else 3
                    op += size
                cursor += size
            put(data, op, 'I', 0x400)
            for i in range(count):
                put(data, IMPULSES + 4 * i, 'f', 0)
            compare_case(solver_images, data, precision, ('builder', hex(builder), seed))


@pytest.mark.parametrize('accepted', [False, True])
def test_validation_gate_preserves_retail_behavior(solver_images, accepted):
    retail, compiled, symbols = solver_images
    target = address(symbols, 'solve_operations_31c940')
    data = solver_case(12, 0)
    outputs = []
    for uc, entry, gate, validator in [
        (retail, 0x31c940, 0x480c60, 0x31c7d0),
        (compiled, target, address(symbols, 'g_480c60'), address(symbols, 'function_31c7d0')),
    ]:
        uc.mem_write(gate, b'\x00')
        calls = []

        def validate(machine, address_, size_, user):
            calls.append(1)
            machine.mem_write(gate, bytes([accepted]))
            return_from_hook(machine)

        output = execute(uc, entry, data, [STEP, OPS, HEAP, IMPULSES], hooks=[(validator, validate)])
        assert calls == [1]
        assert output[1] == int(accepted)
        if not accepted:
            assert output[0] == data
        outputs.append(output)
    assert outputs[0] == outputs[1]
