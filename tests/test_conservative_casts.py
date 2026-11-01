"""Execute both conservative casts with synthetic closest-point providers.

The cast loops, placement copy, SIMD/x87 arithmetic and collector lifetimes
execute normally. Shape type, closest-query and final-result callbacks are
explicit boundaries; this does not validate a complete collision dispatcher.
"""
import math
import struct

import pytest
pytest.importorskip('unicorn')
from unicorn import UC_HOOK_CODE, UcError
from unicorn import x86_const as r
from test_solver_rows import HEAP, STACK, STOP, solver_images  # noqa: F401

A, B, SA, SB, INPUT, CONFIG = [HEAP + n for n in (0, 0x20, 0x100, 0x120, 0x200, 0x280)]
PA, PB, MAIN, SECOND, QUERY, TABLES, DISPATCH = [HEAP + n for n in (0x400, 0x500, 0x600, 0x620, 0x640, 0x700, 0x1000)]
TYPE, METHOD, REPORT, ACCEPT_RETURN = [STOP + n for n in (16, 48, 64, 80)]
HIT = HEAP + 0x5000


def symbol(mapping, name):
    hits = {s.va for s in mapping.find(name)}
    assert len(hits) == 1, (name, hits)
    return hits.pop()


def initial(seed, difficult, alias):
    data = bytearray(b'\xcd' * 65536)
    def put(at, fmt, *values):
        struct.pack_into('<' + fmt, data, at - HEAP, *values)
    put(A, '4I', SA, 0x12340000 + seed, PA, 0)
    put(B, '4I', SA if alias else SB, 0x56780000 + seed, PA if alias else PB, 0)
    put(SA, '2I', TABLES, seed % 32)
    put(SB, '2I', TABLES, seed // 4 % 32)
    put(TABLES + 5 * 4, 'I', TYPE)
    put(TABLES + 0x40 + 4, 'I', REPORT)
    put(TABLES + 0x80 + 8, 'I', METHOD)
    put(QUERY, 'I', TABLES + 0x80)
    for at in (MAIN, SECOND):
        put(at, 'If', TABLES + 0x40, [0, 0.125, 0.5, 1, 2, math.nan][seed // 3 % 6])
    put(INPUT, 'IIfI', DISPATCH, 0x12345678, [0, 0.125, 1, 4][seed // 5 % 4], CONFIG)
    put(INPUT + 16, '4f', -4, 0.25 if seed % 7 == 0 else 0, 0, 7)
    put(INPUT + 32, '2f', [0, 1, 4, -1][seed // 7 % 4], [0, 0.125, -0.125][seed % 3])
    put(CONFIG + 24, 'fi', [0, 0.125, 0.5, 4][seed // 11 % 4], [-1, 0, 1, 2, 4, 7][seed // 2 % 6])
    if difficult:
        location = [INPUT + 16, INPUT + 32, INPUT + 36, CONFIG + 24, MAIN + 4][seed // 8 % 5]
        put(location, 'f', [math.nan, math.inf, -math.inf, -0.0, 1e-30, 1e30][seed % 6])
    for placement in (PA, PB):
        for row in range(6):
            put(placement + row * 16, '4f', row + 0.25, seed * 0.125, -row - 0.5, row * 0.75)
    for index in range(1024):
        put(DISPATCH + 0x218c + index * 4, 'I', STOP + 0x100 + index * 2)
    return data


class Run:
    def __init__(self, machine, entry, copy, accept, seed, precision, method, difficult, alias, mutate):
        self.uc, self.entry, self.copy, self.accept = machine, entry, copy, accept
        self.seed, self.precision, self.method = seed, precision, method
        self.difficult, self.mutate = difficult, mutate
        self.events, self.query_count, self.copy_count = [], 0, 0
        self.pending = None
        self.accept_calls = 0
        machine.mem_write(HEAP, bytes(initial(seed, difficult, alias)))
        machine.mem_write(STOP, b'\xc3' * 4096)
        try:
            machine.mem_map(0, 4096)
        except UcError:
            pass
        self.hook_id = machine.hook_add(UC_HOOK_CODE, self.hook)

    def words(self, at, count=1):
        return struct.unpack('<' + 'I' * count, self.uc.mem_read(at, count * 4))

    def put(self, at, fmt, *values):
        self.uc.mem_write(at, struct.pack('<' + fmt, *values))

    def done(self, sp, cleanup, value=0xabcdef01):
        for reg, val in [(r.UC_X86_REG_EAX, value), (r.UC_X86_REG_ECX, 0xdead1234),
                         (r.UC_X86_REG_EDX, 0xbeef5678)]:
            self.uc.reg_write(reg, val)
        self.uc.reg_write(r.UC_X86_REG_EIP, self.words(sp)[0])
        self.uc.reg_write(r.UC_X86_REG_ESP, sp + 4 + cleanup)

    def hook(self, uc, pc, size, unused):
        if pc == self.copy:
            self.copy_count += 1
        if pc == self.accept:
            self.accept_calls += 1
        if pc == ACCEPT_RETURN:
            sp, cleanup, collector, expected, second = self.pending
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp
            assert bytes(uc.mem_read(collector + 8, 1)) == b'\x01'
            assert bytes(uc.mem_read(collector + 16, 32)) == expected
            if second:
                # A farther candidate (or a tie/nonfinite value) must leave the
                # first hit intact, including all its payload lanes.
                distance = struct.unpack('<f', expected[12:16])[0]
                self.put(HIT, '4f', -111, -222, -333, distance + 16)
                self.put(HIT + 16, '4f', 9, 8, 7, 6)
                self.pending = (sp, cleanup, collector, expected, False)
                self.put(sp - 8, '2I', ACCEPT_RETURN, HIT)
                uc.reg_write(r.UC_X86_REG_ESP, sp - 8)
                uc.reg_write(r.UC_X86_REG_ECX, collector)
                uc.reg_write(r.UC_X86_REG_EIP, self.words(self.words(collector)[0] + 4)[0])
                return
            self.pending = None
            self.done(sp, cleanup)
            return
        dispatch_call = STOP + 0x100 <= pc < STOP + 0x900 and (pc - STOP) % 2 == 0
        if pc not in (TYPE, METHOD, REPORT) and not dispatch_call:
            return
        sp, this = uc.reg_read(r.UC_X86_REG_ESP), uc.reg_read(r.UC_X86_REG_ECX)
        if pc == TYPE:
            assert this in (SA, SB)
            value = self.words(this + 4)[0]
            self.events.append(('type', this, value))
            self.done(sp, 0, value)
        elif dispatch_call or pc == METHOD:
            if pc == METHOD:
                assert this == QUERY
            else:
                first_shape, second_shape = self.words(A)[0], self.words(B)[0]
                index = self.words(first_shape + 4)[0] * 32 + self.words(second_shape + 4)[0]
                assert pc == STOP + 0x100 + index * 2
            first, second, header, collector = self.words(sp + 4, 4)
            assert second == B and (pc == METHOD) == self.method
            shape, key, placement, parent = self.words(first, 4)
            if self.query_count == 0:
                assert first == A
            else:
                assert parent == A and first != A
                assert (shape, key) == self.words(A, 2)
            # Compare actual temporary-body content without comparing stack addresses.
            body = (shape, key, parent, bytes(uc.mem_read(placement, 96)))
            self.events.append(('query', self.query_count, body, bytes(uc.mem_read(header, 16))))
            assert bytes(uc.mem_read(collector + 8, 1)) == b'\0'
            assert self.words(collector + 4)[0] == self.words(collector + 28)[0] == 0x7f7fffff
            iteration = self.query_count
            self.query_count += 1
            pattern = self.seed % 16
            missing = pattern == 0 or (iteration > 0 and pattern == 1)
            if not missing:
                if iteration == 0:
                    distance = [-1, 0, 0.03125, 0.25, 1, 2, 5, 16][pattern % 8]
                else:
                    distance = [0, 0.0625, 0.25, 1, 8][(pattern + iteration) % 5]
                nx = -1 if pattern == 3 and iteration else 1
                if self.difficult and pattern in (6, 7, 8):
                    distance = [math.nan, math.inf, -math.inf][pattern - 6]
                self.put(HIT, '4f', iteration + 0.25, self.seed * 0.5, -iteration - 0.75, distance)
                self.put(HIT + 16, '4f', nx, 0, 0, 3.5)
            if self.mutate and iteration == 0:
                self.put(A + 4, 'I', 0xabcd0000 + self.seed)
                self.put(PA + 0x50, '4f', 11, 12, 13, 14)
                self.put(CONFIG + 28, 'i', 4)
            elif self.mutate and iteration == 1:
                self.put(CONFIG + 24, 'f', 0.125)
                self.put(MAIN + 4, 'f', 0.75)
            if missing:
                self.done(sp, 16 if self.method else 0)
            else:
                # Invoke the temporary collector's actual virtual operation.
                # It must call the existing closest-hit implementation, not a stub.
                self.pending = (sp, 16 if self.method else 0, collector,
                                bytes(uc.mem_read(HIT, 32)), True)
                table = self.words(collector)[0]
                self.put(sp - 8, '2I', ACCEPT_RETURN, HIT)
                uc.reg_write(r.UC_X86_REG_ESP, sp - 8)
                uc.reg_write(r.UC_X86_REG_ECX, collector)
                uc.reg_write(r.UC_X86_REG_EIP, self.words(table + 4)[0])
        else:
            assert this in (MAIN, SECOND)
            hit = self.words(sp + 4)[0]
            assert self.words(hit + 32, 2) == (A, B)
            self.events.append(('report', this, bytes(uc.mem_read(hit, 40))))
            if self.mutate and this == SECOND:
                self.put(MAIN + 4, 'f', 0.5)
                self.put(INPUT + 32, 'f', -0.25)
            self.done(sp, 4)

    def run(self):
        uc = self.uc
        sp = STACK + 0x8000 + self.seed % 4 * 4
        uc.mem_write(STACK, b'\xcc' * 65536)
        self.put(0, 'I', 0xffffffff)
        self.put(sp, '6I', STOP, A, B, INPUT, MAIN, SECOND if self.seed % 3 else 0)
        saved = {r.UC_X86_REG_EBX: 0x11223344, r.UC_X86_REG_ESI: 0x22334455,
                 r.UC_X86_REG_EDI: 0x33445566, r.UC_X86_REG_EBP: 0x44556677}
        for reg, value in {**saved, r.UC_X86_REG_ESP: sp, r.UC_X86_REG_ECX: QUERY,
                           r.UC_X86_REG_EAX: 0, r.UC_X86_REG_EDX: 0,
                           r.UC_X86_REG_EFLAGS: 2, r.UC_X86_REG_FPCW: self.precision,
                           r.UC_X86_REG_FPTAG: 0xffff, r.UC_X86_REG_FPSW: 0,
                           r.UC_X86_REG_MXCSR: 0x1f80}.items():
            uc.reg_write(reg, value)
        try:
            uc.emu_start(self.entry, STOP, count=50000)
            assert uc.reg_read(r.UC_X86_REG_EIP) == STOP
            assert uc.reg_read(r.UC_X86_REG_ESP) == sp + (24 if self.method else 4)
            assert all(uc.reg_read(reg) == value for reg, value in saved.items())
            assert self.words(0)[0] == 0xffffffff
            assert uc.reg_read(r.UC_X86_REG_FPCW) == self.precision
            assert uc.reg_read(r.UC_X86_REG_MXCSR) == 0x1f80
            return (bytes(uc.mem_read(HEAP, 65536)), tuple(self.events), self.query_count,
                    self.copy_count, self.accept_calls)
        finally:
            uc.hook_del(self.hook_id)


@pytest.mark.parametrize('precision', [0x7f, 0x27f, 0x37f])
@pytest.mark.parametrize('method', [False, True])
@pytest.mark.parametrize('difficult,alias,mutate', [(False, False, False), (False, True, True), (True, False, True)])
def test_conservative_casts(solver_images, precision, method, difficult, alias, mutate):
    retail, compiled, mapping = solver_images
    entries = [0x2fec10 if method else 0x2fe830,
               symbol(mapping, 'class_2fec10::function_2fec10' if method else 'function_2fe830')]
    copies = [0x2e7d80, symbol(mapping, 'class_2e7d80::class_2e7d80')]
    accepts = [0x301a00, symbol(mapping, 'class_301a00::add_hit')]
    queried, reported, copied, accepted = set(), set(), 0, 0
    for seed in range(128):
        results = [Run(machine, entry, copy, accept, seed, precision, method, difficult, alias, mutate).run()
                   for machine, entry, copy, accept in zip((retail, compiled), entries, copies, accepts)]
        assert results[0] == results[1], (seed, precision, method, difficult, alias, mutate)
        queried.add(results[0][2])
        reported.update(e[1] for e in results[0][1] if e[0] == 'report')
        copied += results[0][3]
        accepted += results[0][4]
    assert 1 in queried and max(queried) > 2
    assert reported == {MAIN, SECOND}
    assert copied > 0
    assert accepted > 0
