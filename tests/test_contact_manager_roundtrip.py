"""Construct a manager and exercise its eight-slot interface against retail.

Allocation, entity ownership, motion queries and the final listener callbacks
are synthetic. Contact storage and the world/entity listener dispatchers run.
"""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP
from test_solver_rows import HEAP, STOP, solver_images  # noqa: F401
from test_constraint_families import address, put, words, execute, return_from_hook
from test_contact_lifecycle import member, normalize_table, clobber

MANAGER, WORLD, OWNER = HEAP + 0x1000, HEAP + 0x2000, HEAP + 0x2400
CONSTRAINT = MANAGER + 0x10
ENTITIES = HEAP + 0x3000, HEAP + 0x3200
BODIES = HEAP + 0x3400, HEAP + 0x3500
MOTIONS = HEAP + 0x3600, HEAP + 0x3800
ALLOCATOR, MEMORY_TABLE, OWNER_TABLE, MOTION_TABLE = [HEAP + v for v in (0x4000, 0x4100, 0x4200, 0x4300)]
LISTENERS, LISTENER_TABLE, ARRAYS = HEAP + 0x4400, HEAP + 0x4600, HEAP + 0x4700
POINT, INPUT, RESULTS, OUTPUT = [HEAP + v for v in (0x5000, 0x5100, 0x6000, 0x7000)]
ID_OUTPUT, ID_BUFFER = HEAP + 0x7100, HEAP + 0x7200
ALLOCATE, RELEASE, ADD, REMOVE, VELOCITY = [STOP + v for v in (0x100, 0x110, 0x120, 0x130, 0x140)]
EVENTS = STOP + 0x150, STOP + 0x160, STOP + 0x170


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
def test_constructed_contact_manager_interface(solver_images, precision):
    retail, compiled, symbols = solver_images
    constructor = member(symbols, '??0class_318e60@@QAE@PAVc_1d1261@@')
    names = {0x30bdc0: 'class_30bdc0::function_30bdc0',
             0x30bcc0: 'entity_30bcc0::function_30bcc0',
             0x30d2b0: 's_1d1541::function_30d2b0',
             0x30ca30: 'c_1d1261::function_30ca30'}
    for seed in range(32):
        data = bytearray(65536)
        put(data, MANAGER + 4, 'H', 0x120)
        put(data, CONSTRAINT + 4, 'H', 0x110)
        put(data, ALLOCATOR, 'I', MEMORY_TABLE)
        put(data, MEMORY_TABLE + 0x10, 'II', ALLOCATE, RELEASE)
        put(data, OWNER, 'I', OWNER_TABLE)
        put(data, OWNER_TABLE + 8, 'II', ADD, REMOVE)
        put(data, MOTION_TABLE + 0x58, 'I', VELOCITY)
        put(data, LISTENER_TABLE, 'III', *EVENTS)
        put(data, INPUT + 0x30, 'f', 1 / 60)
        put(data, ID_OUTPUT, 'III', ID_BUFFER, 0, 0x80000000 | 32)
        sources = [WORLD, *ENTITIES]
        for i, entity in enumerate(ENTITIES):
            put(data, entity + 6, 'H', 20 + i)
            put(data, entity + 0x3c, 'I', MOTIONS[i])
            put(data, entity + 0x4c, 'ff', 0.3 + i * 0.1, 0.2 + i * 0.3)
            put(data, entity + 0x5c, 'H', [0, 1, 7, 65535][(seed + i) % 4])
            put(data, BODIES[i] + 0x20, 'I', entity)
            put(data, MOTIONS[i], 'I', MOTION_TABLE)
            put(data, MOTIONS[i] + 0x10, '4f', 1 + i, 2 - i, 0.5 * i, 0)
            put(data, MOTIONS[i] + 0x2c, 'f', 0.5 + i)
        for i, source in enumerate(sources):
            array = ARRAYS + i * 0x20
            put(data, source + (0xb4 if i == 0 else 0x60), 'III', array, 3, 3)
            for j in range(3):
                listener = LISTENERS + (i * 3 + j) * 0x10
                put(data, listener, 'III', LISTENER_TABLE, i, j)
                put(data, array + 4 * j, 'I', 0 if j == 1 else listener)
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))
            heap = data
            cursor = HEAP + 0x9000
            calls, snapshots, active = [], [], []

            def allocate(machine, va, size, user):
                nonlocal cursor
                length, tag = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert tag == 0x12
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
                constraint, sizes = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert constraint == CONSTRAINT and machine.reg_read(UC_X86_REG_ECX) == OWNER
                calls.append(('add_size' if va == ADD else 'remove_size', words(machine, sizes, 4)))
                clobber(machine)
                return_from_hook(machine, 8)

            def velocity(machine, va, size, user):
                point, output = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert point == POINT
                motion = machine.reg_read(UC_X86_REG_ECX)
                machine.mem_write(output, bytes(machine.mem_read(motion + 0x10, 16)))
                clobber(machine)
                return_from_hook(machine, 8)

            def entity_ownership(machine, va, size, user):
                constraint, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                calls.append(('entity', machine.reg_read(UC_X86_REG_ECX), constraint))
                clobber(machine)
                return_from_hook(machine, 4)

            def register(machine, va, size, user):
                constraint, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                assert constraint == CONSTRAINT
                machine.mem_write(constraint + 0x14, struct.pack('<I', OWNER))
                calls.append(('world_add', constraint))
                clobber(machine)
                return_from_hook(machine, 4)

            def unregister(machine, va, size, user):
                output, constraint = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                calls.append(('world_remove', constraint))
                machine.mem_write(output, b'\x01')
                machine.reg_write(UC_X86_REG_EAX, output)
                clobber(machine)
                return_from_hook(machine, 8)

            def event(machine, va, size, user):
                kind = EVENTS.index(va)
                listener = machine.reg_read(UC_X86_REG_ECX)
                source_index, index = words(machine, listener + 4, 2)
                event_, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                payload = bytes(machine.mem_read(event_, 20 if kind else 36))
                if kind != 2:
                    payload = payload[:2] + payload[4:]
                calls.append(('event', kind, source_index, index, payload))
                expected_world = 0 if source_index == 0 else sources[source_index]
                assert words(machine, event_ + 0x10, 1)[0] == expected_world
                # Real dispatch must compact a listener that removes itself.
                if kind == 2 and index == 2 and seed & 1:
                    array = ARRAYS + source_index * 0x20
                    for j in range(3):
                        if words(machine, array + 4 * j, 1)[0] == listener:
                            machine.mem_write(array + 4 * j, bytes(4))
                clobber(machine)
                return_from_hook(machine, 4)

            hooks = [(ALLOCATE, allocate), (RELEASE, release), (ADD, accounting),
                     (REMOVE, accounting), (VELOCITY, velocity)] + [(v, event) for v in EVENTS]
            for va, callback in [(0x30bdc0, entity_ownership), (0x30bcc0, entity_ownership),
                                 (0x30d2b0, register), (0x30ca30, unregister)]:
                hooks.append((va if original else address(symbols, names[va]), callback))

            def run(entry, args, destroyed=False):
                nonlocal heap
                heap = bytearray(execute(uc, entry, heap, args, MANAGER, hooks, precision))
                normalized = bytearray(heap)
                tables = [(0, 'class_318e60', 0x415074),
                          (0x10, 'contact_default_31c000', 0x4153c8),
                          (0x18, 'contact_default_31c000', 0x4153b8)]
                if destroyed:
                    tables = [(0, 'referenced_2d88a0', 0x452c30), (0x10, 'c_sized_reference', 0x452c30),
                              (0x18, 'lane_a_constraint_interface', 0x415340)]
                for offset, name, table in tables:
                    normalize_table(normalized, MANAGER - HEAP + offset, original, symbols, name, table)
                snapshots.append(normalized)
                return uc.reg_read(UC_X86_REG_EAX)

            def virtual(slot, args):
                table, = struct.unpack_from('<I', heap, MANAGER - HEAP)
                entry, = words(uc, table + slot * 4, 1)
                return run(entry, args, slot == 4)

            run(0x319110 if original else constructor, [WORLD, *ENTITIES])
            count = [1, 4, 9, 17][seed % 4]
            for i in range(count):
                put(heap, POINT, '8f', 0.1 * i, 0.2, -0.3, 0.4, 0.5, -0.6, 0.7, 0)
                contact_id = virtual(1, [*BODIES, INPUT, POINT]) & 0xffff
                assert contact_id != 0xffff and contact_id not in active
                active.append(contact_id)
                point = virtual(6, [contact_id])
                prop = virtual(5, [contact_id])
                assert point == struct.unpack_from('<I', heap, MANAGER - HEAP + 0x44)[0] + i * 32
                assert prop == struct.unpack_from('<I', heap, MANAGER - HEAP + 0xa0)[0] + i * 28
            for cycle in range(3):
                for i, contact_id in enumerate(reversed(active)):
                    payload = random.Random(seed * 1000 + cycle * 100 + i).randbytes(32)
                    heap[RESULTS - HEAP + i * 48:RESULTS - HEAP + i * 48 + 32] = payload
                    put(heap, RESULTS + i * 48 + 32, 'H', contact_id)
                put(heap, OUTPUT, 'II', RESULTS, len(active))
                put(heap, INPUT, 'I', OUTPUT)
                virtual(3, [*BODIES, POINT, INPUT])
                put(heap, ID_OUTPUT + 4, 'I', 0)
                virtual(7, [ID_OUTPUT])
                size, = struct.unpack_from('<I', heap, ID_OUTPUT - HEAP + 4)
                collected = list(struct.unpack_from('<' + 'H' * size, heap, ID_BUFFER - HEAP))
                assert collected == sorted(active)
                if active:
                    virtual(2, [active.pop((seed + cycle) % len(active))])
            while active:
                virtual(2, [active.pop()])
            virtual(4, [])
            allocated = sorted(c[1:] for c in calls if c[0] == 'allocate')
            released = sorted(c[1:] for c in calls if c[0] == 'release' and c[1] != MANAGER)
            assert allocated == released
            assert ('release', MANAGER, 0x120, 0x1d) in calls
            results.append((snapshots, calls))
        assert results[0] == results[1], (precision, seed)
