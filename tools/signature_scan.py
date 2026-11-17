"""Compares the calling signature of each unmatched decompiled function with
retail's: the stack bytes its `ret` pops and which of EAX, ECX, EDX, EBX, ESI,
EDI and XMM0-XMM7 it reads before writing.

A different total of stack bytes plus register arguments means the source's
parameter list is wrong. The same total split differently between registers and
the stack is only how LTCG passes the arguments: the callers decide it, not the
function's own source.

Registers are followed from the entry to the first `ret`, and the usual
zeroing and constant idioms are not counted as reads; an argument can be read
late, after a long set-up.

One shape it cannot see: a struct returned by value passes a hidden result
pointer as a stack argument, so its total matches retail's when retail takes the
result pointer as an explicit parameter (0x319230: a `const` method returning
one byte against retail's free function taking the result pointer, both three
words). Read the source at any row whose total differs; a byte match still
needs review.

It reads the last full build (build/halo2.exe and build/halo2.map) and
config/functions.csv, and uses build/report.json for the scores when present.

    python tools/signature_scan.py [<retail va> ...] [--all]

Without addresses it scans every marked function that does not match and is
owned by game or Havok. --all prints the split-only differences too.
"""
import argparse
import json
import os
import sys

from capstone import CS_ARCH_X86, CS_MODE_32, Cs
from capstone.x86 import (
    X86_OP_IMM, X86_OP_REG,
    X86_REG_AH, X86_REG_AL, X86_REG_AX, X86_REG_BH, X86_REG_BL, X86_REG_BX,
    X86_REG_CH, X86_REG_CL, X86_REG_CX, X86_REG_DH, X86_REG_DI, X86_REG_DL,
    X86_REG_DX, X86_REG_EAX, X86_REG_EBX, X86_REG_ECX, X86_REG_EDI,
    X86_REG_EDX, X86_REG_ESI, X86_REG_SI,
    X86_REG_XMM0, X86_REG_XMM1, X86_REG_XMM2, X86_REG_XMM3,
    X86_REG_XMM4, X86_REG_XMM5, X86_REG_XMM6, X86_REG_XMM7,
)

import build
import check
from inventory import check_retail, read_rows
from linkmap import LinkMap
from pe import Pe
from xbe import FUNCTIONS_CSV, ROOT, Xbe, retail_xbe_path

# LTCG passes float arguments in XMM registers as readily as integer ones in
# EAX/ECX/EDX, and callee-saved registers as well (0x1d0cf0 gets its component
# in EBX). A push of a callee-saved register is its save, not a read of an
# incoming value, so pushes are excluded for those three.
REGISTERS = (X86_REG_EAX, X86_REG_ECX, X86_REG_EDX,
             X86_REG_EBX, X86_REG_ESI, X86_REG_EDI,
             X86_REG_XMM0, X86_REG_XMM1, X86_REG_XMM2, X86_REG_XMM3,
             X86_REG_XMM4, X86_REG_XMM5, X86_REG_XMM6, X86_REG_XMM7)
CALLEE_SAVED = (X86_REG_EBX, X86_REG_ESI, X86_REG_EDI)
REPORT = os.path.join(ROOT, 'build', 'report.json')
# 8- and 16-bit names map to their 32-bit parent, so `cmp di, -1` marks EDI as
# the argument and `test al, al` marks EAX once.
ALIASES = {
    X86_REG_AX: X86_REG_EAX, X86_REG_AL: X86_REG_EAX, X86_REG_AH: X86_REG_EAX,
    X86_REG_CX: X86_REG_ECX, X86_REG_CL: X86_REG_ECX, X86_REG_CH: X86_REG_ECX,
    X86_REG_DX: X86_REG_EDX, X86_REG_DL: X86_REG_EDX, X86_REG_DH: X86_REG_EDX,
    X86_REG_BX: X86_REG_EBX, X86_REG_BL: X86_REG_EBX, X86_REG_BH: X86_REG_EBX,
    X86_REG_SI: X86_REG_ESI, X86_REG_DI: X86_REG_EDI,
}


def _parent(register):
    return ALIASES.get(register, register)


# Register zeroing/constant idioms that capstone reports as a read of the
# destination although the old value is discarded.
SELF_ZERO = ('xor', 'xorps', 'xorpd', 'pxor', 'sub')


def _reads_and_writes(insn, first=False):
    """Capstone's reads and writes, with sub-registers mapped to their parents
    and `xor edx, edx`, `xorps xmm6, xmm6`, `or edx, -1`, `and edx, 0` counted
    as writes only: they read their destination only to discard it. A leading
    `push ecx` is MSVC's four-byte frame slot, not a read of an argument."""
    raw_reads, raw_writes = insn.regs_access()
    reads = tuple(_parent(r) for r in raw_reads)
    writes = tuple(_parent(r) for r in raw_writes)
    ops = insn.operands
    if (first and insn.mnemonic == 'push' and len(ops) == 1
            and ops[0].type == X86_OP_REG and _parent(ops[0].reg) == X86_REG_ECX):
        reads = tuple(r for r in reads if r != X86_REG_ECX)
    if (insn.mnemonic == 'mov' and len(ops) == 2 and ops[0].type == X86_OP_REG
            and ops[1].type == X86_OP_REG and ops[0].reg == ops[1].reg):
        # `mov edi, edi` is MSVC's two-byte NOP: it reads and writes nothing.
        return (), ()
    if len(ops) == 2 and ops[0].type == X86_OP_REG:
        register = _parent(ops[0].reg)
        same = ops[1].type == X86_OP_REG and ops[0].reg == ops[1].reg
        constant = ops[1].type == X86_OP_IMM and (
            (insn.mnemonic == 'or' and (ops[1].imm & 0xffffffff) == 0xffffffff)
            or (insn.mnemonic == 'and' and ops[1].imm == 0))
        if (same and insn.mnemonic in SELF_ZERO) or constant:
            reads = tuple(r for r in reads if r != register)
    if (insn.mnemonic == 'push' and len(ops) == 1 and ops[0].type == X86_OP_REG
            and _parent(ops[0].reg) in CALLEE_SAVED):
        reads = tuple(r for r in reads if r != _parent(ops[0].reg))
    return reads, writes


def signature(md, data, base):
    """(stack bytes the first ret pops, base registers read before written
    anywhere in the body) of the machine code data, addressed at base."""
    ret = 0
    names = []
    written = set()
    for n, insn in enumerate(md.disasm(data, base)):
        if insn.mnemonic != 'call':
            reads, writes = _reads_and_writes(insn, n == 0)
            for r in REGISTERS:
                if r in reads and r not in written and md.reg_name(r) not in names:
                    names.append(md.reg_name(r))
            written.update(writes)
        if insn.mnemonic == 'ret':
            # A function's epilogues pop the same stack bytes, so the first ret
            # is representative. Stopping there also keeps jump tables and other
            # trailing data from being disassembled as instructions (they can
            # contain byte sequences that look like a different ret).
            ret = insn.operands[0].imm if insn.operands and insn.operands[0].type == X86_OP_IMM else 0
            break
    return ret, tuple(names)


def total(ret, registers):
    """The argument count a signature carries: stack words plus register ones."""
    return ret // 4 + len(registers)


def describe(ret, registers):
    regs = '+'.join(registers) if registers else '-'
    return f'ret {ret:#x} regs {regs}'


def scan(md, marked, rows, report, image, linkmap, retail, standin_calls, wanted):
    """Yield (marked, retail signature, our signature) for each function whose
    signature differs, or whose total differs when all is False."""
    for m in marked:
        if m.stub or (wanted and m.retail not in wanted):
            continue
        row = rows.get(m.retail)
        if row is None or row['status'] == 'matched':
            continue
        entry = report.get(f'{m.retail:08x}')
        if entry is not None and entry.get('status') == 'matched':
            continue
        theirs = signature(md, retail.read(m.retail, int(row['size'])), m.retail)
        symbol = check.resolve(linkmap, m, standin_calls)
        if symbol is None:
            continue
        lo, hi = linkmap.extent(symbol)
        ours = signature(md, image.read(lo, hi - lo), lo)
        if ours != theirs:
            yield m, row, entry, theirs, ours


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument('addresses', nargs='*', help='retail addresses to scan (default: every unmatched game and Havok function)')
    ap.add_argument('--all', action='store_true', help='print signatures that differ only in how arguments split between registers and the stack')
    args = ap.parse_args()
    retail_path = retail_xbe_path()
    if not os.path.exists(retail_path):
        raise SystemExit(f'retail XBE not found at {retail_path}: put it at orig/default.xbe or set RETAIL_XBE')
    check_retail(retail_path)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    wanted = set(check.parse_addresses(args.addresses))
    marked = build.marked_sources() + build.stub_sources()
    rows = read_rows(FUNCTIONS_CSV)
    report = json.load(open(REPORT)) if os.path.exists(REPORT) else {}
    image = Pe(os.path.join(ROOT, 'build', build.EXE_NAME))
    linkmap = LinkMap.read(os.path.join(ROOT, 'build', build.MAP_NAME))
    retail = Xbe(retail_path)
    standin_calls = check.StandinCalls(linkmap, image, marked)
    seen = 0
    for m, row, entry, theirs, ours in scan(md, marked, rows, report, image, linkmap, retail, standin_calls, wanted):
        same_total = total(*theirs) == total(*ours)
        if same_total and not args.all:
            continue
        seen += 1
        score = '?' if entry is None else str(entry.get('score'))
        shape = 'same total (split)' if same_total else 'DIFFERENT TOTAL'
        print(f'{m.retail:08x} score {score:>4} {row["owner"]:14} {shape:16} '
              f'retail {describe(*theirs):28} ours {describe(*ours):28} {m.path} {m.name}')
    print(f'{seen} signature difference(s)'
          + ('' if args.all else ' with a different argument total'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
