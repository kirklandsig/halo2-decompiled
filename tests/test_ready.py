from ready import components, covers, line, parse_claims, ready, without_claims


def row(va, calls='', owner='game', status='todo', size=10, name=''):
    return dict(va=f'{va:08x}', size=str(size), owner=owner, status=status, name=name, object='',
                calls=' '.join(f'{c:08x}' for c in calls))


def test_components_groups_mutual_recursion():
    groups = components({1: {2}, 2: {1}, 3: {1}})
    assert {frozenset(g) for g in groups} == {frozenset({1, 2}), frozenset({3})}


def test_ready_needs_matched_or_library_callees():
    rows = dict([
        (0x10, row(0x10, calls=[0x20])),                       # calls a todo game function
        (0x20, row(0x20, size=5)),                             # leaf
        (0x30, row(0x30, calls=[0x40], size=7)),               # calls a library function
        (0x40, row(0x40, owner='xdk:libcmt')),
        (0x50, row(0x50, calls=[0x60], size=3)),
        (0x60, row(0x60, status='matched')),
        (0x70, row(0x70, status='matched')),                   # done already
    ])
    assert [r['va'] for r in ready(rows)] == ['00000050', '00000020', '00000030']


def test_ready_counts_decompiled_callees_and_skips_decompiled_functions():
    rows = dict([
        (0x10, row(0x10, calls=[0x20])),                       # callee has source, not matched yet
        (0x20, dict(row(0x20, status='near'), source='src/a.cpp')),
        (0x30, row(0x30, calls=[0x40])),                       # callee has no source: not ready
        (0x40, row(0x40, calls=[0x50])),                       # callee of 0x40 not decompiled
        (0x50, row(0x50, calls=[0x60])),
        (0x60, row(0x60, calls=[0x50])),                       # 0x50/0x60 recursion: ready as a unit
    ])
    assert {r['va'] for r in ready(rows)} == {'00000010', '00000050', '00000060'}


def test_ready_treats_recursion_as_one_unit():
    rows = {0x10: row(0x10, calls=[0x20]), 0x20: row(0x20, calls=[0x10])}
    assert {r['va'] for r in ready(rows)} == {'00000010', '00000020'}


def test_ready_line():
    assert line(row(0x20, size=9)) == '00000020 9 - -'
    assert line(row(0x20, size=9, name='_strncmp', calls=[0x30])) == '00000020 9 _strncmp 00000030'

CLAIMS = """
The example in the intro is not a claim: `0xd5990`-`0xd9fff`.

## Active claims

| Who | Retail range | What |
| --- | --- | --- |
| @Banshee64 (#28, draft) | `0xdc370`–`0xe503f` | bipeds |
| lane K (maintainers) | `0x218000`–`0x21ffff` (except @Banshee64's `0x2198f0`–`0x21d10f`), `0x220000`-`0x22bfff` | sound |
| lane A (maintainers) | `0x2a0000`–`0x2affff` + `0x273f30`, `0x10a660` | script |
| lane I (maintainers) | `0x250000`–`0x25ffff` (except the UI screens at `0x250155`–`0x2541b2`) | props |
| UI lane | `0x250155`—`0x2541b2` | screens |

## Finished, not claimed

- `0x180000`–`0x18ffff`: lane F, open for follow-up
"""


def test_parse_claims_keeps_active_table_and_drops_finished():
    claims = parse_claims(CLAIMS)
    assert covers(claims, 0xdc370) and covers(claims, 0xe503f)
    assert not covers(claims, 0xdc36f) and not covers(claims, 0xe5040)
    assert covers(claims, 0x2198ef) and covers(claims, 0x21d110)
    assert not covers(claims, 0x2198f0) and not covers(claims, 0x21d10f)
    assert covers(claims, 0x220000) and covers(claims, 0x22bfff)
    assert covers(claims, 0x273f30) and not covers(claims, 0x273f31)
    assert covers(claims, 0x10a660)
    # lane I's except hole is still claimed, because the UI row lists it
    assert covers(claims, 0x250154) and covers(claims, 0x250155)
    assert covers(claims, 0x2541b2) and covers(claims, 0x2541b3)
    assert not covers(claims, 0x180000)   # Finished section
    assert not covers(claims, 0xd5990)    # prose before the table


def test_parse_claims_leaves_an_except_hole_open_when_nobody_else_claims_it():
    text = """
## Active claims
| Who | Retail range | What |
| --- | --- | --- |
| lane K | `0x218000`-`0x21ffff` (except `0x2198f0`-`0x21d10f`) | sound |
"""
    claims = parse_claims(text)
    assert covers(claims, 0x218000) and not covers(claims, 0x2198f0)
    assert not covers(claims, 0x21d10f) and covers(claims, 0x21d110)


def test_without_claims_drops_ready_rows_inside_a_range():
    rows = dict([
        (0x10, row(0x10, size=4)),
        (0x20, row(0x20, size=8)),
        (0x30, row(0x30, size=2)),
    ])
    claims = parse_claims('| Who | Retail range | What |\n| --- | --- | --- |\n| lane | `0x20`-`0x2f` | x |\n')
    assert [r['va'] for r in without_claims(ready(rows), claims)] == ['00000030', '00000010']


def test_parse_claims_of_prose_is_empty():
    assert parse_claims('no table here, just `0x10`-`0x20` in a sentence\n') == []


TABLE_HEAD = '| Who | Retail range | What |\n| --- | --- | --- |\n'


def test_parse_claims_reads_a_range_in_one_code_span():
    claims = parse_claims('## Active claims\n' + TABLE_HEAD + '| a | `0x2198f0–0x21d10f`, `0x30`-`0x3f` | x |\n')
    assert claims == [(0x30, 0x3f), (0x2198f0, 0x21d10f)]


def test_parse_claims_warns_about_an_address_it_cannot_read():
    warnings = []
    claims = parse_claims('## Active claims\n' + TABLE_HEAD + '| a | `0x10..0x20`, `0x30` | x |\n',
                          warn=warnings.append)
    assert claims == [(0x30, 0x30)]
    assert warnings == ['not read as an address or range: `0x10..0x20`']


def test_parse_claims_stops_at_a_level_three_heading():
    text = ('### Active claims\n' + TABLE_HEAD + '| a | `0x100`-`0x1ff` | x |\n\n'
            '### Finished, not claimed\n' + TABLE_HEAD + '| b | `0x800`-`0x8ff` | y |\n')
    claims = parse_claims(text)
    assert covers(claims, 0x100) and not covers(claims, 0x800)


def test_main_rejects_partially_unreadable_claims(tmp_path, monkeypatch, capsys):
    import pytest
    import ready as module

    inventory = tmp_path / 'functions.csv'
    inventory.write_text('va,size,owner,status,name,calls\n00000018,10,game,todo,,\n',
                         encoding='utf-8')
    claims = tmp_path / 'claims.md'
    claims.write_text('## Active claims\n' + TABLE_HEAD
                      + '| lane | `0x10..0x20`, `0x30` | example |\n', encoding='utf-8')
    monkeypatch.setattr(module, 'FUNCTIONS_CSV', str(inventory))
    monkeypatch.setattr(module.sys, 'argv', ['ready.py', '--claims', str(claims)])

    with pytest.raises(SystemExit) as stopped:
        module.main()
    assert stopped.value.code == 2
    output = capsys.readouterr()
    assert output.out == ''
    assert 'not read as an address or range: `0x10..0x20`' in output.err


def test_parse_claims_skips_the_issue_title_and_keeps_sub_headings():
    text = ('# Active claims: who is working on which address ranges\n\n'
            '## Active claims\n\n### Lanes\n' + TABLE_HEAD + '| a | `0x100`-`0x1ff` | x |\n\n'
            '## Finished, not claimed\n' + TABLE_HEAD + '| b | `0x800`-`0x8ff` | y |\n')
    claims = parse_claims(text)
    assert covers(claims, 0x180) and not covers(claims, 0x880)


def test_parse_claims_without_the_heading_stops_at_the_heading_after_the_table():
    text = ('# My saved copy\n' + TABLE_HEAD + '| a | `0x100`-`0x1ff` | x |\n\n'
            '## Finished, not claimed\n' + TABLE_HEAD + '| b | `0x800`-`0x8ff` | y |\n')
    claims = parse_claims(text)
    assert covers(claims, 0x100) and not covers(claims, 0x800)
