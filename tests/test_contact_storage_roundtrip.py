"""Exercise constructed contact storage through real growth and ID reuse."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP
from test_solver_rows import HEAP, STOP, solver_images  # noqa: F401
from test_constraint_families import address, put, words, execute, return_from_hook
from test_contact_lifecycle import normalize_table, clobber

OBJECT, OWNER, ALLOCATOR = HEAP + 0x1000, HEAP + 0x3000, HEAP + 0x4000
OWNER_TABLE, MEMORY_TABLE, OUTPUT = HEAP + 0x5000, HEAP + 0x5100, HEAP + 0x6000
ALLOCATE, RELEASE, ADD, REMOVE = STOP + 0x100, STOP + 0x110, STOP + 0x120, STOP + 0x130


def test_contact_storage_growth_and_reuse(solver_images):
    retail, compiled, symbols = solver_images
    names = {0x31c000: 'contact_default_31c000::contact_default_31c000',
             0x31be80: 'lane_b_contact_constraint_prefix::add',
             0x31bc90: 'lane_b_contact_constraint_prefix::remove',
             0x315510: 'contact_constraint_315510::~contact_constraint_315510'}
    for seed in range(64):
        data = bytearray(random.Random(seed + 4000).randbytes(65536))
        put(data, OBJECT + 4, 'H', 0x110)
        put(data, OWNER, 'I', OWNER_TABLE)
        put(data, OWNER_TABLE + 8, 'II', ADD, REMOVE)
        put(data, ALLOCATOR, 'I', MEMORY_TABLE)
        put(data, MEMORY_TABLE + 0x10, 'II', ALLOCATE, RELEASE)
        count = [0, 1, 4, 8, 9, 12, 17, 24][seed % 8]
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))
            heap = data
            cursor = HEAP + 0x9000
            calls, snapshots, active = [], [], []

            def allocate(machine, va, size, user):
                nonlocal cursor
                length, tag = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert tag == 0x12 and machine.reg_read(UC_X86_REG_ECX) == ALLOCATOR
                block = cursor
                cursor += (length + 31) & ~15
                assert cursor < HEAP + 65536
                calls.append(('allocate', block, length, tag))
                machine.reg_write(UC_X86_REG_EAX, block)
                clobber(machine)
                return_from_hook(machine, 8)

            def release(machine, va, size, user):
                calls.append(('release', *words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3)))
                clobber(machine)
                return_from_hook(machine, 12)

            def accounting(machine, va, size, user):
                contact, sizes = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert machine.reg_read(UC_X86_REG_ECX) == OWNER and contact == OBJECT
                calls.append(('add' if va == ADD else 'remove', words(machine, sizes, 4)))
                clobber(machine)
                return_from_hook(machine, 8)

            hooks = [(ALLOCATE, allocate), (RELEASE, release), (ADD, accounting), (REMOVE, accounting)]

            def run(va, args):
                nonlocal heap
                heap = bytearray(execute(uc, va if original else address(symbols, names[va]), heap, args, OBJECT, hooks))
                normalized = bytearray(heap)
                if va == 0x315510:
                    tables = [(0, 'c_sized_reference', 0x452c30), (8, 'lane_a_constraint_interface', 0x415340)]
                else:
                    tables = [(0, 'contact_default_31c000', 0x4153c8), (8, 'contact_default_31c000', 0x4153b8)]
                for offset, name, table in tables:
                    normalize_table(normalized, OBJECT - HEAP + offset, original, symbols, name, table)
                snapshots.append(normalized)

            def add_contact(index):
                run(0x31be80, [OUTPUT, OUTPUT + 4])
                contact_id = uc.reg_read(UC_X86_REG_EAX)
                assert contact_id not in active
                active.append(contact_id)
                point, prop = struct.unpack_from('<II', heap, OUTPUT - HEAP)
                # Populate caller-owned payload, including non-finite bit patterns.
                payload = random.Random(seed * 100 + index).randbytes(32)
                heap[point - HEAP:point - HEAP + 32] = payload
                heap[prop - HEAP + 8:prop - HEAP + 24] = payload[:16]

            run(0x31c000, [])
            put(heap, OBJECT + 0x14, 'I', OWNER)
            for index in range(count):
                add_contact(index)
            for _ in range(min(3, len(active))):
                index = (seed + len(active) // 2) % len(active)
                run(0x31bc90, [active.pop(index)])
            for index in range(count, count + 3):
                add_contact(index)
            while active:
                index = (seed * 7 + len(active) // 2) % len(active)
                run(0x31bc90, [active.pop(index)])
            assert struct.unpack_from('<I', heap, OBJECT - HEAP + 0x94)[0] == 0
            run(0x315510, [])
            # Every owned allocation, including replacements, is released once.
            allocated = sorted((call[1], call[2], call[3]) for call in calls if call[0] == 'allocate')
            released = sorted(call[1:] for call in calls if call[0] == 'release')
            assert allocated == released
            results.append((snapshots, calls))
        assert results[0] == results[1], seed
