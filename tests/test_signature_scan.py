from capstone import CS_ARCH_X86, CS_MODE_32, Cs

import signature_scan

MD = Cs(CS_ARCH_X86, CS_MODE_32)
MD.detail = True


def sig(code):
    return signature_scan.signature(MD, bytes.fromhex(code), 0x1000)


def test_register_arguments_are_reads_before_writes():
    """`mov eax, ecx` reads ECX before writing EAX, so ECX is an argument; EAX
    and EDX are written first and are not."""
    ret, registers = sig('8bc1'      # mov eax, ecx
                         '8b542404'  # mov edx, [esp + 4]
                         'c20400')   # ret 4
    assert ret == 4 and registers == ('ecx',)
    assert signature_scan.total(ret, registers) == 2


def test_stack_arguments_only():
    """Reading the stack into EAX and ECX writes both registers first."""
    ret, registers = sig('8b442404'  # mov eax, [esp + 4]
                         '8b4c2408'  # mov ecx, [esp + 8]
                         'c20800')   # ret 8
    assert (ret, registers) == (8, ())
    assert signature_scan.total(ret, registers) == 2


def test_push_counts_as_a_register_read():
    """`push eax` reads EAX, so it is an argument even though nothing writes it."""
    assert sig('50c3') == (0, ('eax',))  # push eax; ret


def test_xmm_arguments_count():
    """LTCG passes floats in XMM registers; `movaps xmm0, xmm5` reads XMM5
    before writing XMM0, so XMM5 is an argument."""
    ret, registers = sig('0f28c5'    # movaps xmm0, xmm5
                         'c3')       # ret
    assert (ret, registers) == (0, ('xmm5',))


def test_zeroing_idioms_are_writes_not_reads():
    """`xorps xmm6, xmm6` and `or edx, -1` discard their destination, and
    `xor ecx, ecx` is a write; none of them makes an argument."""
    assert sig('0f57f6c3') == (0, ())          # xorps xmm6, xmm6; ret
    assert sig('83caffc3') == (0, ())          # or edx, -1; ret
    assert sig('33c9c3') == (0, ())            # xor ecx, ecx; ret


def test_first_ret_wins():
    """Trailing data can decode as a different ret; the first ret is the
    function's own and is the one whose pop count matters."""
    ret, registers = sig('8bc1'      # mov eax, ecx
                         'c20400'    # ret 4
                         'c21800')   # ret 0x18 (data after the function)
    assert (ret, registers) == (4, ('ecx',))


def test_register_read_after_many_instructions_counts():
    """A register argument may be read late, after lots of set-up."""
    ret, registers = sig('8b442404'  # mov eax, [esp + 4]
                         + '90' * 30  # 30 nops
                         + '03d1'     # add edx, ecx
                         + 'c3')      # ret
    assert (ret, registers) == (0, ('ecx', 'edx'))


def test_self_move_is_a_nop():
    """`mov edi, edi` is MSVC's two-byte NOP: it names neither an argument nor
    a definition."""
    assert sig('8bffc3') == (0, ())  # mov edi, edi; ret


def test_first_push_ecx_is_a_frame_slot():
    """A leading `push ecx` allocates a four-byte local; ECX is not an
    argument. A later `push ecx` still counts."""
    assert sig('518bc0c3') == (0, ())        # push ecx; mov eax, eax; ret
    assert sig('8bc051c3') == (0, ('ecx',))  # mov eax, eax; push ecx; ret


def test_sub_registers_map_to_their_parent():
    """`cmp di, -1` reads the 16-bit DI, which is the EDI argument."""
    assert sig('6683ffffc3') == (0, ('edi',))  # cmp di, -1; ret
    assert sig('f6c401c3') == (0, ('eax',))    # test ah, 1; ret


def test_total_adds_stack_words_and_register_arguments():
    assert signature_scan.total(0xc, ('eax', 'ecx', 'edx')) == 6
    assert signature_scan.total(8, ('eax',)) == 3
    assert signature_scan.describe(8, ('eax',)) == 'ret 0x8 regs eax'
    assert signature_scan.describe(0, ()) == 'ret 0x0 regs -'
