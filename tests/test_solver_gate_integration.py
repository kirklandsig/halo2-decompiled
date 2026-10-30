"""Exercise the reconstructed solver with its real hash and validation gate."""
import math
import random
import struct

import pytest

pytest.importorskip('unicorn')
from test_solver_rows import HEAP, OPS, solver_images  # noqa: F401
from test_solver_dispatch import (
    STEP, IMPULSES, address, execute, float_outputs, return_from_hook, solver_case,
)
from test_solver_validation import execute as execute_hash


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('accepted', [False, True])
def test_solver_with_real_validation(solver_images, accepted, precision):
    retail, compiled, symbols = solver_images
    globals_ = [0x445470, 0x46fbf0, 0x415448, 0x480c60]
    mapped = {va: address(symbols, 'g_' + hex(va)[2:]) for va in globals_}
    solver = address(symbols, 'solve_operations_31c940')
    hash_entry = address(symbols, 'function_31c540')
    notice = address(symbols, 'function_2d97d0')
    for seed in range(32):
        data = solver_case(12, seed if seed % 7 else seed + 1)
        rng = random.Random(seed)
        text = b'synthetic solver validation ' + str(seed).encode() + b'\0'
        data[0xe000:0xe000 + len(text)] = text
        salt = bytes(rng.randrange(1, 128) for _ in range(63)) + b'\0'
        snapshots, calls = [], []
        for uc, target, hash_, warning, addresses in [
                (retail, 0x31c940, 0x31c540, 0x2d97d0, {va: va for va in globals_}),
                (compiled, solver, hash_entry, notice, mapped)]:
            uc.mem_write(HEAP, bytes(data))
            uc.mem_write(addresses[0x415448], salt)
            uc.mem_write(addresses[0x46fbf0], struct.pack('<I', HEAP + 0xe000))
            uc.mem_write(addresses[0x480c60], b'\0')
            digest = execute_hash(uc, hash_, [HEAP + 0xe000])
            if uc is retail:
                expected_hash = digest if accepted else digest ^ 1
                assert expected_hash != 0
            uc.mem_write(addresses[0x445470], struct.pack('<I', expected_hash))
            notices = []

            def failure(machine, address_, size_, _):
                notices.append(1)
                return_from_hook(machine)

            result = execute(uc, target, data, [STEP, OPS, HEAP, IMPULSES],
                             hooks=[(warning, failure)], precision=precision)
            assert result[1] == int(accepted)
            assert bytes(uc.mem_read(addresses[0x480c60], 1)) == bytes([accepted])
            assert notices == ([] if accepted else [1])
            snapshots.append(result[0])
            calls.append(notices)
        expected, actual = snapshots
        if not accepted:
            assert expected == actual == data
            continue
        outputs = float_outputs(data)
        for offset in range(0, len(data), 4):
            rb, cb = expected[offset:offset + 4], actual[offset:offset + 4]
            if rb == cb: continue
            rf, cf = struct.unpack('<f', rb)[0], struct.unpack('<f', cb)[0]
            assert offset in outputs and (math.isclose(rf, cf, rel_tol=2e-5, abs_tol=2e-6)
                                          or math.isnan(rf) and math.isnan(cf)), (
                seed, precision, hex(offset), rf, cf)
