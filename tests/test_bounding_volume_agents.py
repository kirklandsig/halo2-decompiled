"""Retail differential checks for bounds-first agent dispatch and lifetimes.

Shape/child-agent callbacks are synthetic. The query bodies and their actual
temporary collectors execute from both images, including enabled timer marks.
"""
import random
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

AGENT, A, B = HEAP, HEAP + 0x100, HEAP + 0x120
WRAPPER, BOUND, INNER, OTHER, INNER2 = [HEAP + x for x in (0x200, 0x240, 0x280, 0x2c0, 0x2e0)]
INPUT, TABLES, BOUND_AGENT, MANAGER = [HEAP + x for x in (0x300, 0x400, 0x800, 0x900)]
CHILDREN, DISPATCH, TIMERS, OUTPUT, START_OUTPUT, POINT = [HEAP + x for x in (0xa00, 0x2000, 0xa000, 0xc000, 0xc100, 0xe000)]
TYPE, CREATE, QUERY, CAST, CLEAN, FILTER, RESUME = [STOP + x for x in (0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70)]
FREE_QUERY, FREE_CAST = STOP + 0x80, STOP + 0x90
QUERY_CLOSEST, QUERY_PROCESS, HANDLERS = STOP + 0xa0, STOP + 0xb0, STOP + 0x1000
ENTRIES = {
    0x2e6d70: 'lane_b_bv_agent::lane_b_bv_agent',
    0x2e6e90: 'lane_b_bv_agent::process_collision',
    0x2e7030: 'lane_b_bv_agent::linear_cast',
    0x2e71f0: 'function_2e71f0',
    0x2e7370: 'lane_b_bv_agent::get_closest_points',
    0x2e7510: 'function_2e7510',
    0x2e7670: 'lane_b_bv_agent::get_penetrations',
    0x2e7710: 'function_2e7710',
    0x2e77d0: 'lane_b_bv_agent::update_filter',
}


def symbol(symbols, name):
    found = {s.va for s in symbols.find(name)}
    if name == 'lane_b_bv_agent::lane_b_bv_agent':
        found = {s.va for s in symbols.find(name) if 'PBUbody_2ea940@@' in s.name}
    assert len(found) == 1, (name, found)
    return found.pop()


def fixture_bytes(seed):
    rng = random.Random(seed)
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(AGENT + 4, 'HH', 20, seed + 1)
    put(A, '4I', WRAPPER, rng.getrandbits(32), HEAP + 0x1000, 0)
    put(B, '4I', OTHER, rng.getrandbits(32), HEAP + (0x1000 if seed % 2 else 0x1100), A if seed % 3 else 0)
    put(WRAPPER + 12, '2I', BOUND, INNER)
    put(TABLES + 20, 'I', TYPE)
    for i, shape in enumerate((BOUND, INNER, OTHER, INNER2)):
        put(shape, '2I', TABLES, (seed + i * 7) % 32)
    for i, callback in enumerate((0, QUERY, QUERY_CLOSEST, CAST, QUERY_PROCESS, CLEAN, 0, FILTER)):
        put(TABLES + 64 + i * 4, 'I', callback)
    put(BOUND_AGENT, '2I', TABLES + 64, 0)
    put(INPUT, 'I', DISPATCH)
    for index in range(4):
        for pair in range(32 * 32):
            put(DISPATCH + 0x18c + index * 4096 + pair * 4, 'I', HANDLERS + index * 4096 + pair * 4)
    put(OUTPUT, 'I', 0)
    put(START_OUTPUT, 'I', 0)
    return bytes(data)


class Run:
    def __init__(self, uc, entries, cursor, limit, table, seed, precision):
        self.uc, self.entries, self.cursor, self.limit, self.table = uc, entries, cursor, limit, table
        self.seed, self.precision = seed, precision
        self.trace, self.created, self.released, self.timer_events = [], [], [], []
        self.hit, self.step, self.pending = False, 0, None
        uc.mem_write(HEAP, fixture_bytes(seed))
        uc.mem_write(cursor, struct.pack('<I', TIMERS))
        # Retail tests cursor < limit, even if less than a complete mark remains.
        self.capacity = [0, 1, 11, 12, 24, 36, 120, 2048][seed % 8]
        uc.mem_write(limit, struct.pack('<I', TIMERS + self.capacity))
        try:
            uc.mem_map(0, 4096)
        except UcError:
            pass
        try:
            uc.mem_map(HANDLERS, 0x4000)
        except UcError:
            pass
        uc.mem_write(HANDLERS, b'\xc3' * 0x4000)
        self.handle = uc.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, n=1):
        return struct.unpack('<' + 'I' * n, self.uc.mem_read(at, n * 4))

    def finish(self, sp, cleanup, result=0xabcdef01):
        self.uc.reg_write(r.UC_X86_REG_EAX, result)
        self.uc.reg_write(r.UC_X86_REG_ECX, 0x87654321)
        self.uc.reg_write(r.UC_X86_REG_EDX, 0x10203040)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def next_contact(self):
        sp, cleanup, collector, points, is_cast = self.pending
        if not points:
            if is_cast and self.hit:
                self.trace.append(('closest-result', self.words(collector + 4)[0],
                                   bytes(self.uc.mem_read(collector + 16, 32))))
            self.pending = None
            self.finish(sp, cleanup)
            return
        distance = points.pop(0)
        if is_cast:
            self.uc.mem_write(POINT, struct.pack('<8f2I', 1.25, -2.5, 4, distance,
                                               -1, 0.5, 2, 0, A, B))
            args = [POINT]
        else:
            args = [A, B]
        nested = sp - 128
        self.uc.mem_write(nested, struct.pack('<' + 'I' * (len(args) + 1), RESUME, *args))
        self.uc.reg_write(r.UC_X86_REG_ESP, nested)
        self.uc.reg_write(r.UC_X86_REG_ECX, collector)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(self.words(collector)[0] + 4)[0])

    def hook(self, machine, pc, size, unused):
        if size == 2 and bytes(machine.mem_read(pc, 2)) == b'\x0f\x31':
            # Supply a deterministic hardware counter; all writes and cursor updates are real.
            mark = self.words(self.cursor)[0]
            label = self.words(mark)[0]
            text = bytes(machine.mem_read(label, 32)).split(b'\0', 1)[0]
            value = (0xfffffff0 + self.seed + 97 * len(self.timer_events)) & 0xffffffff
            self.timer_events.append((text, value))
            machine.reg_write(r.UC_X86_REG_EAX, value)
            machine.reg_write(r.UC_X86_REG_EDX, 0xdeadbeef)
            machine.reg_write(r.UC_X86_REG_EIP, pc + 2)
            return
        if pc == RESUME:
            self.next_contact()
            return
        table_index = None
        if HANDLERS <= pc < HANDLERS + 0x4000:
            table_index, pair_offset = divmod(pc - HANDLERS, 4096)
            sp = machine.reg_read(r.UC_X86_REG_ESP)
            a, b = self.words(sp + 4, 2)
            first = self.words(self.words(a)[0] + 4)[0]
            second = self.words(self.words(b)[0] + 4)[0]
            assert pair_offset == 4 * (32 * first + second)
            self.trace.append(('dispatch', table_index, first, second))
            pc = [CREATE, FREE_QUERY, FREE_QUERY, FREE_CAST][table_index]
        if pc not in (TYPE, CREATE, QUERY, QUERY_CLOSEST, QUERY_PROCESS, CAST, CLEAN, FILTER, FREE_QUERY, FREE_CAST):
            return
        sp = machine.reg_read(r.UC_X86_REG_ESP)
        this = machine.reg_read(r.UC_X86_REG_ECX)
        if pc == TYPE:
            self.trace.append(('type', this))
            self.finish(sp, 0, self.words(this + 4)[0])
            return
        if pc == CLEAN:
            assert this not in self.released
            self.released.append(this)
            self.trace.append(('cleanup', this))
            machine.mem_write(this + 4, struct.pack('<I', self.words(this + 4)[0] + 1))
            self.finish(sp, 0)
            return
        is_cast = pc in (CAST, FREE_CAST)
        count = 5 if is_cast else 3 if pc == FILTER else 4
        args = self.words(sp + 4, count)
        body, other, query_input = args[:3]
        body_data = self.words(body, 4)
        assert other == B and query_input == INPUT
        assert body_data[2] == self.words(A + 8)[0] and body_data[3] == A
        if pc == CREATE:
            assert args[3] == MANAGER
            if body_data[0] == BOUND:
                result = BOUND_AGENT
            else:
                result = CHILDREN + 32 * len(self.created)
                self.created.append(result)
                machine.mem_write(result, struct.pack('<2I', TABLES + 64, 0))
            self.trace.append(('create', body_data, result))
            self.finish(sp, 0, result)
            return
        if pc == FILTER:
            self.trace.append(('filter', this, body_data))
            self.finish(sp, 12)
            return
        cleanup = count * 4 if pc in (QUERY, QUERY_CLOSEST, QUERY_PROCESS, CAST) else 0
        collector = args[3]
        bounding = body_data[0] == BOUND
        self.trace.append(('bounds' if bounding else 'child', pc, is_cast, body_data,
                           self.words(other, 4), args[4] == collector if is_cast else False))
        if bounding:
            if is_cast:
                assert self.words(collector + 4)[0] == 0x7f7fffff
                assert machine.mem_read(collector + 8, 1) == b'\0'
                assert self.words(collector + 28)[0] == 0x7f7fffff
            else:
                assert machine.mem_read(collector + 4, 1) == b'\0'
            # The child body must reload its parent's key after the callback.
            if self.seed % 2:
                machine.mem_write(A + 4, struct.pack('<I', (self.seed * 65537 + self.step) & 0xffffffff))
                machine.mem_write(WRAPPER + 16, struct.pack('<I', INNER2 if self.step % 2 else INNER))
            distances = ([2.0, -1.0, 0.0, float('nan'), -3.0] if self.seed % 3 else
                         [float('nan'), -2.0, float('inf')]) if self.hit else []
            self.pending = [sp, cleanup, collector, distances if is_cast else ([0] if self.hit else []), is_cast]
            self.next_contact()
        else:
            assert collector == OUTPUT
            if is_cast:
                assert args[4] == (START_OUTPUT if self.seed % 2 else 0)
            assert body_data[1] == self.words(A + 4)[0]
            machine.mem_write(OUTPUT, struct.pack('<I', self.words(OUTPUT)[0] + 1))
            if is_cast and args[4]:
                machine.mem_write(START_OUTPUT, struct.pack('<I', self.words(START_OUTPUT)[0] + 1))
            self.finish(sp, cleanup)

    def call(self, va, args, this=0):
        uc = self.uc
        sp = STACK + 32768
        uc.mem_write(STACK, b'\xcd' * 65536)
        uc.mem_write(0, struct.pack('<I', 0xffffffff))
        uc.mem_write(sp, struct.pack('<' + 'I' * (len(args) + 1), STOP, *args))
        saved = (r.UC_X86_REG_EBX, r.UC_X86_REG_ESI, r.UC_X86_REG_EDI, r.UC_X86_REG_EBP)
        for reg in saved:
            uc.reg_write(reg, 0x12345678)
        uc.reg_write(r.UC_X86_REG_ECX, this)
        uc.reg_write(r.UC_X86_REG_ESP, sp)
        uc.reg_write(r.UC_X86_REG_EFLAGS, 2)
        uc.reg_write(r.UC_X86_REG_FPCW, self.precision)
        uc.reg_write(r.UC_X86_REG_FPTAG, 0xffff)
        uc.reg_write(r.UC_X86_REG_FPSW, 0)
        uc.reg_write(r.UC_X86_REG_MXCSR, 0x1f80)
        try:
            uc.emu_start(self.entries[va], STOP, count=30000)
        except UcError as error:
            raise AssertionError((hex(va), self.seed, self.step,
                                  hex(uc.reg_read(r.UC_X86_REG_EIP)), self.trace[-8:])) from error
        assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
        assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 4 + (4 * len(args) if this else 0)
        assert all(uc.reg_read(reg) == 0x12345678 for reg in saved)
        assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
        assert self.words(0)[0] == 0xffffffff
        if va == 0x2e6d70:
            assert self.words(AGENT)[0] == self.table
            assert uc.reg_read(r.UC_X86_REG_EAX) == AGENT

    def result(self, constructed):
        data = bytearray(self.uc.mem_read(HEAP, 65536))
        if constructed:
            struct.pack_into('<I', data, 0, 0)  # already verified the actual class table
        cursor = self.words(self.cursor)[0]
        assert cursor == TIMERS + 12 * len(self.timer_events)
        for i, (_, timestamp) in enumerate(self.timer_events):
            assert self.words(TIMERS + 12 * i + 4)[0] == timestamp
            struct.pack_into('<I', data, TIMERS - HEAP + 12 * i, 0)  # compare label contents in trace
        return bytes(data), self.trace, self.timer_events, cursor


def configurations(solver_images):
    retail, compiled, symbols = solver_images
    entries = {va: symbol(symbols, name) for va, name in ENTRIES.items()}
    table = next(s.va for s in symbols.symbols if s.name == '??_7lane_b_bv_agent@@6B@')
    return [(retail, {va: va for va in ENTRIES}, 0x480154, 0x480158, 0x4129dc),
            (compiled, entries, symbol(symbols, 'g_480154'), symbol(symbols, 'g_480158'), table)]


@pytest.mark.parametrize('precision', [0x007f, 0x027f, 0x037f])
def test_bounding_agent_stateful_queries(solver_images, precision):
    for seed in range(64):
        results = []
        for config in configurations(solver_images):
            run = Run(*config, seed, precision)
            try:
                run.call(0x2e6d70, [A, B, INPUT, MANAGER], AGENT)
                for step, hit in enumerate([False, True, True, False, False, True, True, False]):
                    run.step, run.hit = step, hit
                    va = [0x2e6e90, 0x2e7030, 0x2e7370][(seed + step) % 3]
                    args = [A, B, INPUT, OUTPUT]
                    if va == 0x2e7030:
                        args.append(START_OUTPUT if seed % 2 else 0)
                    run.call(va, args, AGENT)
                    if step == 2:
                        run.call(0x2e77d0, [A, B, INPUT], AGENT)
                assert len(run.created) == 2 and run.released == run.created
                assert run.words(AGENT + 16)[0] == 0
                assert run.words(OUTPUT)[0] == 4
                results.append(run.result(True))
            finally:
                run.uc.hook_del(run.handle)
        assert results[0] == results[1], (seed, precision)


@pytest.mark.parametrize('precision', [0x007f, 0x027f, 0x037f])
@pytest.mark.parametrize('va', [0x2e71f0, 0x2e7510, 0x2e7710])
def test_bounding_agent_free_queries(solver_images, precision, va):
    for seed in range(64):
        results = []
        for config in configurations(solver_images):
            run = Run(*config, seed, precision)
            try:
                run.hit = bool(seed % 2)
                args = [A, B, INPUT, OUTPUT]
                # The penetration query forwards the user's collector directly.
                if va == 0x2e7710:
                    run.uc.mem_write(OUTPUT, struct.pack('<IB', TABLES + 128, 0))
                    run.uc.mem_write(TABLES + 128 + 4, struct.pack('<I', TYPE))
                    # Use the real flag collector operation, not a synthetic implementation.
                    actual_flag_table = 0x4129cc if config[0] is solver_images[0] else next(
                        s.va for s in solver_images[2].symbols if s.name == '??_7lane_b_flag_body_pair_collector@@6B@')
                    run.uc.mem_write(OUTPUT, struct.pack('<I', actual_flag_table))
                if va == 0x2e71f0:
                    args.append(START_OUTPUT if seed % 2 else 0)
                run.call(va, args)
                if va == 0x2e7710:
                    run.uc.mem_write(OUTPUT, struct.pack('<I', 0))
                else:
                    assert run.words(OUTPUT)[0] == int(run.hit)
                results.append(run.result(False))
            finally:
                run.uc.hook_del(run.handle)
        assert results[0] == results[1], (seed, precision, hex(va))
