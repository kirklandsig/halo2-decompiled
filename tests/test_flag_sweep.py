import flag_sweep


def test_parse_flags_reads_the_line():
    assert flag_sweep.parse_flags('// @flags /O2 /Gr\ntext\n') == ['/O2', '/Gr']
    assert flag_sweep.parse_flags('/* a note */\n') == []


def test_with_flags_replaces_or_inserts():
    text = '// @flags /O2 /Gr\ntext\n'
    assert flag_sweep.with_flags(text, ['/O1', '/Gr']) == '// @flags /O1 /Gr\ntext\n'
    assert flag_sweep.with_flags('text\n', ['/O1']).startswith('// @flags /O1\ntext\n')


def test_with_replaces_the_axis_or_appends():
    assert flag_sweep._with(('/O2', '/Gr'), '/O1') == ('/O1', '/Gr')
    assert flag_sweep._with(('/O2',), '/Ob1') == ('/O2', '/Ob1')


def test_variants_keep_the_baseline_and_vary_one_axis():
    out = flag_sweep.variants(['/O2', '/Gr'])
    assert out[0] == ('/O2', '/Gr')
    assert ('/O1', '/Gr') in out
    assert ('/Od', '/Gr') in out
    assert ('/O2', '/Gr', '/Ob1') in out
    assert ('/O2', '/Gr', '/Oy') in out
    assert len(out) == len(set(out))  # no duplicate flag sets


def test_variants_do_not_repeat_an_axis():
    out = flag_sweep.variants(['/O2', '/Ob1'])
    assert not any(flags.count(token) > 1 for flags in out for token in ('/O2', '/Ob1'))
    assert ('/O1', '/Ob1') in out


def test_results_reads_match_and_diff_lines():
    output = (
        'DIFF   000a6430 function_a6430 at +0x1 (549 bytes, retail 548), score 14\n'
        'MATCH  0011e5e0 function_11e5e0\n'
    )
    assert flag_sweep.results(output) == {0xa6430: ('DIFF', 14), 0x11e5e0: ('MATCH', 0)}


def test_dirty_reads_git_status(monkeypatch):
    class Done:
        stdout = ' M src/x.cpp\n'

    monkeypatch.setattr(flag_sweep.subprocess, 'run', lambda *a, **k: Done())
    assert flag_sweep.dirty('src/x.cpp')
    Done.stdout = ''
    assert not flag_sweep.dirty('src/x.cpp')
