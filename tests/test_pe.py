import os
import struct

import pytest

from build import run_tool
from pe import Pe

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


@pytest.mark.sdk
def test_pe_reads_base_relocations(xdk_dir, tmp_path):
    src = os.path.join(ROOT, 'spike')
    for name in ('crc', 'crc_test'):
        run_tool('CL.Exe', ['/c', '/GL', '/O2', '/Gr', os.path.join(src, name + '.cpp'), f'/Fo{tmp_path / name}.obj'],
                 str(tmp_path), xdk_dir)
    run_tool('Link.Exe', ['/LTCG', '/NODEFAULTLIB', '/ENTRY:entry', '/SUBSYSTEM:CONSOLE', '/FIXED:NO',
                          f'/OUT:{tmp_path / "t.exe"}', str(tmp_path / 'crc.obj'), str(tmp_path / 'crc_test.obj')],
             str(tmp_path), xdk_dir)
    image = Pe(str(tmp_path / 't.exe'))
    assert image.base == 0x400000
    assert image.fixups, 'a /FIXED:NO image has base relocations'
    assert all(image.base <= f < image.base + 0x100000 for f in image.fixups)
    assert len(image.read(image.base + 0x1000, 16)) == 16


def synthetic_pe(tmp_path, blocks=(), image_base=0x400000):
    """Small generated PE32 image; no compiler or retail bytes required."""
    data = bytearray(0x600)
    data[:2] = b'MZ'
    struct.pack_into('<I', data, 0x3c, 0x80)
    data[0x80:0x84] = b'PE\0\0'
    struct.pack_into('<HHIIIHH', data, 0x84, 0x14c, 2, 0, 0, 0, 0xe0, 0x102)
    opt = 0x98
    struct.pack_into('<H', data, opt, 0x10b)
    struct.pack_into('<I', data, opt + 28, image_base)
    struct.pack_into('<II', data, opt + 32, 0x1000, 0x200)
    struct.pack_into('<II', data, opt + 56, 0x3000, 0x200)
    struct.pack_into('<I', data, opt + 92, 16)
    table = opt + 0xe0
    for i, (name, rva, raw) in enumerate([(b'.text', 0x1000, 0x200),
                                          (b'.reloc', 0x2000, 0x400)]):
        start = table + i * 40
        data[start:start + 8] = name.ljust(8, b'\0')
        struct.pack_into('<IIII', data, start + 8, 0x300, rva, 0x200, raw)
    data[0x200:0x400] = bytes(range(256)) * 2
    relocation_data = b''.join(struct.pack('<II', page, 8 + 2 * len(entries))
                               + struct.pack('<' + 'H' * len(entries), *entries)
                               for page, entries in blocks)
    assert len(relocation_data) <= 0x200
    if blocks:
        struct.pack_into('<II', data, opt + 96 + 5 * 8, 0x2000, len(relocation_data))
        data[0x400:0x400 + len(relocation_data)] = relocation_data
    path = tmp_path / 'synthetic.exe'
    path.write_bytes(data)
    return Pe(path)


def test_pe_without_relocations_reads_sections_and_zero_fill(tmp_path):
    image = synthetic_pe(tmp_path)
    assert image.base == 0x400000
    assert image.fixups == set()
    assert image.read(0x401010, 4) == bytes([16, 17, 18, 19])
    assert image.read(0x4011fc, 8) == bytes([252, 253, 254, 255, 0, 0, 0, 0])
    assert image.read(0x401210, 4) == bytes(4)
    with pytest.raises(ValueError, match='in no section'):
        image.read(0x403000, 4)


@pytest.mark.parametrize('image_base', [0x400000, 0x10000000])
def test_pe_relocations_keep_only_highlow_fields_across_blocks(tmp_path, image_base):
    image = synthetic_pe(tmp_path, [
        (0x1000, [0x3000, 0x3010, 0x0000, 0x1018]),
        (0x2000, [0x3024, 0x3024, 0x2028, 0x0000]),
    ], image_base)
    # HIGHLOW at offset zero is real; ABSOLUTE padding and HIGH/LOW are not
    # four-byte address fields. Duplicate entries must not multiply masks.
    assert image.fixups == {image_base + 0x1000, image_base + 0x1010,
                            image_base + 0x2024}
