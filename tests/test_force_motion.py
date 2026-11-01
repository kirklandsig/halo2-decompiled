"""Optional retail differential checks for force and swept-pose callers.

Requires Unicorn, a retail XBE and a current full build. Both callers use the
retail math callees: this tests caller reconstruction, not the still-stubbed
quaternion-to-matrix implementation at 0x372500.
"""
import math
import random
import struct
from pathlib import Path

import pytest

uc = pytest.importorskip("unicorn", reason="optional x86 differential checks")
from unicorn import x86_const as reg
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86 import X86_OP_IMM, X86_OP_MEM

from linkmap import LinkMap
from pe import Pe
from xbe import Xbe

ROOT = Path(__file__).resolve().parents[1]
HEAP, STACK, STOP = 0x10000000, 0x20000000, 0x30000000
CALLERS, MASK = 0x40000000, 0x40003000
OBJECT, FORCE, POINT = HEAP + 0x1000, HEAP + 0x2000, HEAP + 0x2010
FUNCTIONS = {
    0x31acd0: ("force_motion_31acd0::add_force_at", 115),
    0x31b700: ("force_motion_31acd0::interpolate_sweep", 587),
}
CALLEES = [0x319300, 0x1d8d60, 0x372500, 0x2db7a0] * 2
CALLEE_NAMES = {
    0x319300: "quaternion_319300::function_319300",
    0x1d8d60: "s_1d8d60::function_1d8d60",
    0x372500: "rotation_372500::function_372500",
    0x2db7a0: "vector_2db7a0::function_2db7a0",
}


def _map_image(machine, image):
    start = min(s.va for s in image.sections) & ~4095
    end = (max(s.va + max(s.vsize, s.rsize) for s in image.sections) + 4095) & ~4095
    machine.mem_map(start, end - start)
    for section in image.sections:
        machine.mem_write(section.va, image.data[section.raw:section.raw + section.rsize])


@pytest.fixture(scope="module")
def images():
    from xbe import retail_xbe_path
    path = Path(retail_xbe_path())
    if not path.exists() or not (ROOT / "build/halo2.exe").exists():
        pytest.skip("retail XBE and full compiled image are required")
    retail = Xbe(path)
    compiled = Pe(ROOT / "build/halo2.exe")
    mapping = LinkMap.read(ROOT / "build/halo2.map")
    entries = {}
    for va, (name, _) in FUNCTIONS.items():
        hits = mapping.find(name)
        if len(hits) != 1:
            pytest.skip("full build does not contain this motion batch")
        entries[va] = hits[0].va
    machine = uc.Uc(uc.UC_ARCH_X86, uc.UC_MODE_32)
    _map_image(machine, retail)
    machine.mem_map(CALLERS, 0x10000)
    mask = retail.read(0x415120, 16)
    machine.mem_write(MASK, mask)
    for base in [HEAP, STACK]:
        machine.mem_map(base, 0x10000)
    machine.mem_map(STOP, 4096)

    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.detail = True
    # Relocate only these compiled callers, keeping retail globals intact.
    # Check every relocated reference against the actual linker targets/data.
    relocated = {}
    for index, (va, entry) in enumerate(entries.items()):
        size = FUNCTIONS[va][1]
        code = bytearray(compiled.read(entry, size))
        destination = CALLERS + index * 0x1000
        instructions = list(decoder.disasm(bytes(code), entry))
        assert instructions[-1].mnemonic == "ret"
        assert instructions[-1].address + instructions[-1].size == entry + size
        calls = [i for i in instructions if i.mnemonic == "call"]
        assert len(calls) == (len(CALLEES) if va == 0x31b700 else 0)
        for instruction, target in zip(calls, CALLEES):
            assert instruction.size == 5 and instruction.operands[0].type == X86_OP_IMM
            candidates = mapping.find(CALLEE_NAMES[target])
            assert instruction.operands[0].imm in {s.va for s in candidates}
            offset = instruction.address - entry
            struct.pack_into("<i", code, offset + 1, target - destination - offset - 5)
        for instruction in instructions:
            if instruction.mnemonic.startswith("j"):
                assert entry <= instruction.operands[0].imm < entry + size
            for operand in instruction.operands:
                if operand.type == X86_OP_MEM and not operand.mem.base and not operand.mem.index:
                    assert compiled.read(operand.mem.disp, 16) == mask
                    assert instruction.disp_size == 4
                    struct.pack_into("<I", code,
                                     instruction.address - entry + instruction.disp_offset, MASK)
        machine.mem_write(destination, bytes(code))
        relocated[va] = destination
    return machine, relocated


def _case(seed):
    rng = random.Random(seed)
    memory = bytearray(b"\xcd" * 0x10000)

    def put(offset, fmt, *values):
        struct.pack_into("<" + fmt, memory, OBJECT - HEAP + offset, *values)

    for offset in range(0x20, 0x110, 16):
        put(offset, "4f", *(rng.uniform(-4, 4) for _ in range(4)))
    put(0xc, "f", [0.0, 0.1, 1.0, 10.0][seed % 4])
    quaternion = [rng.uniform(-1, 1) for _ in range(4)]
    length = math.sqrt(sum(x*x for x in quaternion))
    put(0x30, "4f", *(x/length for x in quaternion))
    if seed % 11 == 0:
        put(0xd0, "4f", 0.0, 0.0, 0.0, 0.0)
    if seed % 7 == 0:
        put(0xc0, "4f", 0.0, 0.0, 0.0, 0.0)
    put(0xcc, "f", 0.0)
    put(0xdc, "f", 0.0)
    previous = [0.0, 1.0, 0.5, -0.25, 1.25, rng.random()][seed % 6]
    put(0x7c, "f", previous)
    struct.pack_into("<4f", memory, FORCE - HEAP,
                     rng.uniform(-5, 5), rng.uniform(-5, 5), rng.uniform(-5, 5), 0.0)
    struct.pack_into("<4f", memory, POINT - HEAP,
                     rng.uniform(-5, 5), rng.uniform(-5, 5), rng.uniform(-5, 5), 1.0)
    fraction = [0.0, 1.0, previous, 0.25, 1.5, -0.5][seed % 6]
    duration = [0.0, 1/60, 1/30, -1/60, rng.uniform(0.001, 0.5)][seed % 5]
    return memory, fraction, duration


def _execute(machine, entry, memory, args, precision):
    machine.mem_write(HEAP, bytes(memory))
    machine.mem_write(STACK, b"\xcc" * 0x10000)
    sp = STACK + 0x8000
    machine.mem_write(sp, struct.pack("<III", STOP, *args))
    preserved = {reg.UC_X86_REG_EBX: 0x12345678, reg.UC_X86_REG_ESI: 0x23456789,
                 reg.UC_X86_REG_EDI: 0x3456789a, reg.UC_X86_REG_EBP: 0x456789ab}
    for register, value in preserved.items():
        machine.reg_write(register, value)
    for register in [reg.UC_X86_REG_EAX, reg.UC_X86_REG_EDX]:
        machine.reg_write(register, 0)
    machine.reg_write(reg.UC_X86_REG_ECX, OBJECT)
    machine.reg_write(reg.UC_X86_REG_ESP, sp)
    machine.reg_write(reg.UC_X86_REG_EFLAGS, 2)
    machine.reg_write(reg.UC_X86_REG_FPCW, precision)
    machine.reg_write(reg.UC_X86_REG_FPTAG, 0xffff)
    machine.reg_write(reg.UC_X86_REG_FPSW, 0)
    machine.reg_write(reg.UC_X86_REG_MXCSR, 0x1f80)
    machine.emu_start(entry, STOP, count=20000)
    assert machine.reg_read(reg.UC_X86_REG_EIP) == STOP
    assert machine.reg_read(reg.UC_X86_REG_ESP) == sp + 12
    for register, value in preserved.items():
        assert machine.reg_read(register) == value
    return bytes(machine.mem_read(HEAP, len(memory)))


@pytest.mark.parametrize("va", FUNCTIONS)
@pytest.mark.parametrize("precision", [0x7f, 0x27f, 0x37f])
def test_motion_against_retail(images, va, precision):
    machine, entries = images
    for seed in range(128):
        memory, fraction, duration = _case(seed)
        if va == 0x31acd0:
            args = (FORCE, POINT)
            changed = range(0xf0, 0x110)
        else:
            args = struct.unpack("<II", struct.pack("<ff", fraction, duration))
            changed = set(range(0x30, 0x40)) | set(range(0x60, 0xc0))
        expected = _execute(machine, va, memory, args, precision)
        actual = _execute(machine, entries[va], memory, args, precision)
        assert expected == actual, (hex(va), precision, seed)
        allowed = {OBJECT - HEAP + offset for offset in changed}
        assert all(a == b or i in allowed for i, (a, b) in enumerate(zip(memory, actual)))
