"""Reference teardown after real contact construction, compared with retail."""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_ESP
from test_solver_rows import HEAP, STOP, solver_images  # noqa: F401
from test_constraint_families import address, put, words, execute, return_from_hook
from test_contact_lifecycle import normalize_table, clobber

OBJECT, A, B, ALLOCATOR, REGISTRY = [HEAP + v for v in (0x1000, 0x3000, 0x3200, 0x4000, 0x4200)]
ENTITY_TABLE, MEMORY_TABLE = HEAP + 0x5000, HEAP + 0x5100
DELETE, RELEASE = STOP + 0x100, STOP + 0x110


@pytest.mark.parametrize('derived', [False, True])
def test_constraint_reference_teardown(solver_images, derived):
    retail, compiled, symbols = solver_images
    constructor = address(symbols, 'contact_default_31c000::contact_default_31c000')
    destructor = address(symbols, 'contact_constraint_315510::~contact_constraint_315510'
                         if derived else 'lane_a_constraint::~lane_a_constraint')
    coverage = set()
    for seed in range(96):
        data = bytearray(random.Random(seed + 6000).randbytes(65536))
        first, second = [(0, 0), (A, 0), (0, B), (A, B), (A, A), (B, A)][seed % 6]
        put(data, OBJECT + 4, 'H', 0x110 | (0x8000 if seed & 1 else 0))
        put(data, A, 'I', ENTITY_TABLE)
        put(data, B, 'I', ENTITY_TABLE)
        put(data, A + 6, 'H', (2 if first == second == A else 1) + (seed >> 1 & 1))
        put(data, B + 6, 'H', 1 + (seed >> 2 & 1))
        put(data, ENTITY_TABLE, 'I', DELETE)
        put(data, ALLOCATOR, 'I', MEMORY_TABLE)
        put(data, MEMORY_TABLE + 0x14, 'I', RELEASE)
        results = []
        for uc, original in [(retail, True), (compiled, False)]:
            uc.mem_write(0x480118 if original else address(symbols, 'g_480118'), struct.pack('<I', ALLOCATOR))
            uc.mem_write(0x48011c if original else address(symbols, 'g_48011c'), struct.pack('<I', REGISTRY))
            heap = bytearray(execute(uc, 0x31c000 if original else constructor, data, [], OBJECT))
            put(heap, OBJECT + 0xc, 'II', first, second)
            if derived:
                # Independently exercise each owned array's release bit.
                for i, (offset, width) in enumerate([(0x20, 1), (0x34, 32), (0x90, 28)]):
                    put(heap, OBJECT + offset, 'III', HEAP + 0x6000 + i * 0x1000, 0,
                        (seed + 1) | (0x80000000 if seed & (1 << i) else 0))
            calls = []

            def unlink(machine, va, size, user):
                constraint, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                entity = machine.reg_read(UC_X86_REG_ECX)
                assert constraint == OBJECT
                calls.append(('unlink', entity, constraint))
                clobber(machine)
                return_from_hook(machine, 4)

            def retained(machine, va, size, user):
                entity, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                keep = (seed >> 3) & 1
                calls.append(('retain', entity, keep))
                coverage.add('retain' if keep else 'delete')
                machine.reg_write(UC_X86_REG_EAX, keep)
                clobber(machine)
                return_from_hook(machine)

            def deleting(machine, va, size, user):
                entity = machine.reg_read(UC_X86_REG_ECX)
                flags, = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 1)
                assert flags == 1
                calls.append(('delete', entity))
                machine.reg_write(UC_X86_REG_EAX, entity)
                clobber(machine)
                return_from_hook(machine, 4)

            def release(machine, va, size, user):
                assert machine.reg_read(UC_X86_REG_ECX) == ALLOCATOR
                calls.append(('release', *words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 3)))
                clobber(machine)
                return_from_hook(machine, 12)

            def unregister(machine, va, size, user):
                object_, name = words(machine, machine.reg_read(UC_X86_REG_ESP) + 4, 2)
                assert machine.reg_read(UC_X86_REG_ECX) == REGISTRY
                assert (object_, name) == (OBJECT, 0)
                assert struct.unpack('<H', machine.mem_read(OBJECT + 4, 2))[0] == 0x110
                calls.append(('unregister', object_))
                coverage.add('registered')
                clobber(machine)
                return_from_hook(machine, 8)

            hooks = [(DELETE, deleting), (RELEASE, release),
                     (0x30bcc0 if original else address(symbols, 'entity_30bcc0::function_30bcc0'), unlink),
                     (0x279740 if original else address(symbols, 'function_279740'), retained),
                     (0x2d7180 if original else address(symbols, 'c_reference_name_registry::set_name'), unregister)]
            retail_va = 0x315510 if derived else 0x31b9e0
            result = bytearray(execute(uc, retail_va if original else destructor, heap, [], OBJECT, hooks))
            normalize_table(result, OBJECT - HEAP, original, symbols, 'c_sized_reference', 0x452c30)
            normalize_table(result, OBJECT - HEAP + 8, original, symbols, 'lane_a_constraint_interface', 0x415340)
            assert [c[1] for c in calls if c[0] == 'unlink'] == [p for p in (first, second) if p]
            assert len([c for c in calls if c[0] == 'unregister']) == (seed & 1)
            if derived:
                assert len([c for c in calls if c[0] == 'release']) == 3 - (seed & 7).bit_count()
            results.append((result, calls))
        assert results[0] == results[1], (derived, seed)
    assert coverage == {'retain', 'delete', 'registered'}
