import frame_probe

SAMPLE = """// @retail 0x1234
void f(long a, long b)
{
\tlong x = a;
\treturn x;
}
"""


def test_dead_probes_insert_at_the_top_of_the_body():
    out = frame_probe.dead_probes(SAMPLE, 0x1234, (('4 bytes', 'byte probe[4];'),))
    assert len(out) == 1
    label, text = out[0]
    assert label == '4 bytes'
    assert '{\n\tbyte probe[4];\n\tlong x = a;' in text


def test_width_probes_narrow_and_widen_simple_locals():
    narrow = frame_probe.width_probes(SAMPLE, 0x1234)
    assert any(label.startswith('narrow long -> short') for label, _ in narrow)
    widened = frame_probe.width_probes('// @retail 0x1234\nvoid g()\n{\n\tshort s;\n}\n', 0x1234)
    assert any(label.startswith('widen short -> long') for label, _ in widened)


def test_width_probes_keep_qualifiers():
    text = '// @retail 0x1234\nvoid g()\n{\n\tvolatile long id = 0;\n}\n'
    out = frame_probe.width_probes(text, 0x1234)
    label, changed = next((l, t) for l, t in out if l.startswith('narrow long'))
    assert 'volatile short id = 0;' in changed
    assert 'volatile long id = 0;' in label


def test_parse_result_reads_match_and_diff():
    assert frame_probe.parse_result('MATCH  0011e5e0 function_11e5e0\n') == ('MATCH', 0)
    assert frame_probe.parse_result('DIFF   000a6430 f at +0x1 (549 bytes), score 14\n') == ('DIFF', 14)
    assert frame_probe.parse_result('nothing here\n') is None


def test_failure_reads_a_compiler_error():
    assert 'error C' in frame_probe.failure('x.cpp(1) : error C2059: syntax error\n')
