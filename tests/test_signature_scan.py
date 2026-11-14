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


def test_total_adds_stack_words_and_register_arguments():
    assert signature_scan.total(0xc, ('eax', 'ecx', 'edx')) == 6
    assert signature_scan.total(8, ('eax',)) == 3
    assert signature_scan.describe(8, ('eax',)) == 'ret 0x8 regs eax'
    assert signature_scan.describe(0, ()) == 'ret 0x0 regs -'
