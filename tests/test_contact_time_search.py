"""Execute the contact-time search, its bisection, and stack-array lifetime.

Only external collector, motion, handler and overflow-allocation callbacks are
synthetic. Timer instructions receive deterministic counter values. All helper
bodies, allocation bookkeeping, contact classification and timer writes run.
"""
import math
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

SEARCH, WORLD, STATE, INPUT = [HEAP + x for x in (0, 0x100, 0x400, 0x500)]
PAIRS, MEMBERS, BODIES, MOTIONS = [HEAP + x for x in (0x600, 0x1000, 0x2000, 0x3000)]
COLLECTORS, HANDLERS, TABLES, MEMORY = [HEAP + x for x in (0x4000, 0x4400, 0x4800, 0x5000)]
BLOCK, TIMERS = HEAP + 0x6000, HEAP + 0xa000
COLLECT, HANDLE, MOVE, ALLOCATE, RELEASE = [STOP + x for x in (0x10, 0x20, 0x30, 0x40, 0x50)]


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, overflow, mutate, difficult):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(SEARCH + 0x20, 'I', WORLD)
    count = [-1, 0, 1, 2, 4, 6][seed % 6]
    put(SEARCH + 0x68, 'Ii', PAIRS, count)
    put(WORLD + 0x244, '3f', [-0.25, 0, 0.125, 0.5][seed % 4],
        [0, 0.125, 0.25, -0.125][seed // 4 % 4], [0.03125, 0.125, 1][seed % 3])
    put(WORLD + 0x254, 'I', STATE)
    if difficult:
        put(WORLD + 0x244, 'f', [math.nan, math.inf, -math.inf, -0.0, 1e-30, 1e30][seed % 6])
    put(STATE + 12, 'If', 7, 123.5)
    put(INPUT + 16, 'I', 0x12340000 + seed)
    put(TABLES + 16, 'I', COLLECT)
    put(TABLES + 0x40 + 12, 'I', HANDLE)
    put(TABLES + 0x80 + 29 * 4, 'I', MOVE)
    put(TABLES + 0x100 + 9 * 4, '2I', ALLOCATE, RELEASE)
    put(MEMORY, '6I', TABLES + 0x100, 0, BLOCK, 0 if overflow else 8192,
        0, BLOCK if overflow else BLOCK + 8192)
    for index in range(6):
        member_a, member_b = MEMBERS + index * 0x80 + 0x10, MEMBERS + index * 0x80 + 0x50
        collector = COLLECTORS + index * 32
        put(PAIRS + index * 16, '3Ii', member_a, member_b,
            0 if (seed + index) % 7 == 0 else collector, [-3, -2, -1, 0, 1, 3][(seed + index) % 6])
        put(collector, '3I', TABLES, index, HANDLERS + index * 16)
        put(HANDLERS + index * 16, 'I', TABLES + 0x40)
        for side, member in enumerate((member_a, member_b)):
            body = BODIES + (index * 2 + side) * 0x80
            motion = MOTIONS + (index * 2 + side) * 0x100
            put(member + 16, 'I', body)
            put(body + 0x3c, 'I', motion)
            put(motion, 'I', TABLES + 0x80)
            put(motion + 0x7c, 'f', [0, 0.25, 0.5, 1.0][(seed + side * (1 + seed % 3)) % 4])
    return bytes(data), count


class Run:
    def __init__(self, machine, entry, globals_, helpers, seed, precision, overflow, mutate, difficult):
        self.uc, self.entry, self.globals = machine, entry, globals_
        self.seed, self.precision, self.overflow, self.mutate = seed, precision, overflow, mutate
        self.helpers, self.difficult = helpers, difficult
        self.helper_calls = {name: 0 for name in helpers.values()}
        self.events, self.timers, self.calls = [], [], [0] * 6
        self.moves = 0
        data, self.count = initial(seed, overflow, mutate, difficult)
        machine.mem_write(HEAP, data)
        try:
            machine.mem_map(0, 4096)
        except UcError:
            pass
        machine.mem_write(STOP, b'\xc3' * 4096)
        for at, value in zip(globals_, (MEMORY, TIMERS, TIMERS + [0, 1, 12, 13, 24, 48][seed % 6])):
            machine.mem_write(at, struct.pack('<I', value))
        self.hook_id = machine.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, count=1):
        return struct.unpack('<' + 'I' * count, self.uc.mem_read(at, count * 4))

    def put(self, at, fmt, *values):
        self.uc.mem_write(at, struct.pack('<' + fmt, *values))

    def done(self, sp, cleanup, value=0xabcdef01):
        for register, val in [(r.UC_X86_REG_EAX, value), (r.UC_X86_REG_ECX, 0xdead1234),
                              (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(register, val)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def hook(self, uc, pc, size, unused):
        if pc in self.helpers:
            self.helper_calls[self.helpers[pc]] += 1
        if size == 2 and bytes(uc.mem_read(pc, 2)) == b'\x0f\x31':
            mark = self.words(self.globals[1])[0]
            label = self.words(mark)[0]
            text = bytes(uc.mem_read(label, 32)).split(b'\0', 1)[0]
            assert text == ([b'TtNarrowPhase', b'Et'][len(self.timers)])
            stamp = (0xfffffff0 + self.seed + len(self.timers) * 97) & 0xffffffff
            self.timers.append((mark, text, stamp))
            uc.reg_write(r.UC_X86_REG_EAX, stamp)
            uc.reg_write(r.UC_X86_REG_EDX, 0x12345678)
            uc.reg_write(r.UC_X86_REG_EIP, pc + 2)
            return
        if pc not in (COLLECT, HANDLE, MOVE, ALLOCATE, RELEASE):
            return
        sp, this = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == ALLOCATE:
            assert self.overflow and this == MEMORY and self.words(sp + 4)[0] == 3072
            self.events.append(('allocate', 3072))
            if self.mutate:
                self.count = (self.seed + 3) % 7
                self.put(SEARCH + 0x6c, 'i', self.count)
            self.done(sp, 4, BLOCK)
        elif pc == RELEASE:
            assert self.overflow and this == MEMORY
            assert self.words(sp + 4, 2) == (BLOCK, 3072)
            self.events.append(('release', BLOCK, 3072))
            self.done(sp, 8)
        elif pc == MOVE:
            bits, context = self.words(sp + 4, 2)
            assert context == 0x12340000 + self.seed
            assert MOTIONS <= this < MOTIONS + 12 * 256 and (this - MOTIONS) % 256 == 0
            self.put(this + 0x7c, 'I', bits)
            self.moves += 1
            self.events.append(('move', this, bits, context))
            self.done(sp, 8)
        else:
            first, second, input_, output = self.words(sp + 4, 4)
            index = (this - (COLLECTORS if pc == COLLECT else HANDLERS)) // (32 if pc == COLLECT else 16)
            assert 0 <= index < 6
            pair_first, pair_second = self.words(PAIRS + index * 16, 2)
            assert (first, second, input_) == (pair_first - 16, pair_second - 16, INPUT)
            array = self.words(output)[0]
            block, count = self.words(array, 2)
            assert block == BLOCK
            if pc == COLLECT:
                assert count == 0
                tolerance_bits = self.words(STATE + 16)[0]
                tolerance = struct.unpack('<f', struct.pack('<I', tolerance_bits))[0]
                iteration = self.calls[index]
                self.calls[index] += 1
                mode = (self.seed + index + iteration) % 8
                values = [[], [-1.0], [0.0], [tolerance], [tolerance - 0.25, tolerance + 0.5],
                          [tolerance + 0.25], [math.nan, math.inf], [-math.inf]][mode]
                self.put(array + 4, 'i', len(values))
                for n, distance in enumerate(values):
                    payload = bytes(((self.seed + index * 13 + iteration * 7 + n * 19 + b) & 255) for b in range(48))
                    uc.mem_write(block + n * 48, payload)
                    self.put(block + n * 48 + 12, 'f', distance)
                if self.seed % 13 == 0:
                    self.put(STATE + 12, 'I', 0)
                # Calls must continue over the captured count, even if a callback changes it.
                if self.seed % 11 == 0:
                    self.put(SEARCH + 0x6c, 'I', 0)
                if self.seed % 17 == 0 and iteration == 0:
                    self.put(PAIRS + index * 16 + 12, 'i', -2)
                if self.difficult and iteration == 0:
                    # Later collections and the final handler must reload pair members.
                    self.put(PAIRS + index * 16, '2I', pair_second, pair_first)
                    # A callback may remove the next pair's collector.
                    if index < 5 and self.seed % 3 == 0:
                        self.put(PAIRS + (index + 1) * 16 + 8, 'I', 0)
                self.events.append(('collect', index, tolerance_bits, mode, len(values), first, second))
            else:
                self.events.append(('handle', index, self.words(PAIRS + index * 16 + 12)[0], first, second,
                                    bytes(uc.mem_read(block, count * 48))))
            self.done(sp, 16)

    def run(self):
        uc = self.uc
        sp = STACK + 0x8000 + (self.seed % 4) * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(0, 'I', 0xffffffff)
        self.put(sp, '2I', STOP, INPUT)
        preserved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                     r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for register, value in preserved.items():
            uc.reg_write(register, value)
        for register, value in [(r.UC_X86_REG_ESP, sp), (r.UC_X86_REG_ECX, SEARCH),
                                (r.UC_X86_REG_EAX, 0), (r.UC_X86_REG_EDX, 0),
                                (r.UC_X86_REG_EFLAGS, 2), (r.UC_X86_REG_FPCW, self.precision),
                                (r.UC_X86_REG_FPTAG, 0xffff), (r.UC_X86_REG_FPSW, 0),
                                (r.UC_X86_REG_MXCSR, 0x1f80)]:
            uc.reg_write(register, value)
        try:
            uc.emu_start(self.entry, STOP, count=200000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + 8
            assert self.words(0)[0] == 0xffffffff
            assert all(uc.reg_read(reg) == value for reg, value in preserved.items())
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            assert self.words(MEMORY + 8, 2) == (BLOCK, 0 if self.overflow else 8192)
            assert sum(event[0] == 'allocate' for event in self.events) == int(self.overflow)
            assert sum(event[0] == 'release' for event in self.events) == int(self.overflow)
            assert self.helper_calls['construct'] == self.helper_calls['destroy'] == 1
            data = bytearray(uc.mem_read(HEAP, 65536))
            # The only differing addresses in output are verified timer-label pointers.
            for mark, text, stamp in self.timers:
                assert self.words(mark + 4)[0] == stamp
                struct.pack_into('<I', data, mark - HEAP, 1 if text == b'TtNarrowPhase' else 2)
            return (bytes(data), tuple(self.events), tuple(self.timers), self.words(self.globals[1])[0],
                    self.moves, tuple(self.helper_calls.items()))
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('overflow,mutate', [(False, False), (True, False), (True, True)])
@pytest.mark.parametrize('difficult', [False, True])
def test_contact_time_search(solver_images, precision, overflow, mutate, difficult):
    retail, compiled, mapping = solver_images
    entries = [0x317950, symbol(mapping, 'narrow_phase_317c00::function_317950')]
    globals_ = [(0x480118, 0x480154, 0x480158),
                tuple(symbol(mapping, name) for name in ('g_480118', 'g_480154', 'g_480158'))]
    names = {'minimum': 'function_3163c0', 'bisect': 'time_search_316460::function_316460',
             'construct': 'stack_array_317060::stack_array_317060', 'destroy': 'stack_array_317060::~stack_array_317060'}
    helpers = [dict(zip((0x3163c0, 0x316460, 0x317060, 0x317100), names)),
               {symbol(mapping, name): kind for kind, name in names.items()}]
    coverage = set()
    moved = 0
    invoked = set()
    for seed in range(96):
        results = [Run(machine, entry, addresses, helper, seed, precision, overflow, mutate, difficult).run()
                   for machine, entry, addresses, helper in zip((retail, compiled), entries, globals_, helpers)]
        assert results[0] == results[1], (seed, precision, overflow, mutate, difficult)
        coverage.update(event[3] for event in results[0][1] if event[0] == 'collect')
        moved += results[0][-2]
        invoked.update(name for name, count in results[0][-1] if count)
    assert coverage == set(range(8))
    assert moved > 0
    assert invoked == set(names)
