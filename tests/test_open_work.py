import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'tools'))

from open_work import claim_text, open_files, person_claims  # noqa: E402
from ready import claim_rows, parse_claims  # noqa: E402

TABLE = """## Active claims

| Who | Retail range | What |
| --- | --- | --- |
| @someone | `0x1000`–`0x1fff` (#5, draft) | a person's claim |
| lane Z (maintainers, Codex) | `0x2000`–`0x2fff` (except `0x2800`) | a maintainer lane |

## Finished, not claimed
- `0x5000`–`0x5fff`
"""


def row(va, size, status, source, calls=''):
    return {'va': '%08x' % va, 'size': str(size), 'status': status, 'source': source,
            'owner': 'game', 'calls': calls, 'name': ''}


def test_claim_rows_keeps_who_and_holes():
    rows = claim_rows(TABLE)
    assert rows == [('@someone', [(0x1000, 0x1fff)]),
                    ('lane Z (maintainers, Codex)', [(0x2000, 0x27ff), (0x2801, 0x2fff)])]


def test_parse_claims_unchanged():
    assert parse_claims(TABLE) == [(0x1000, 0x27ff), (0x2801, 0x2fff)]


def test_person_claims_only():
    assert person_claims(TABLE) == [(0x1000, 0x1fff)]


def test_open_files_skips_persons_and_finished_files():
    rows = {
        0x1100: row(0x1100, 10, 'todo', 'src/a.cpp'),       # a person's claim
        0x2100: row(0x2100, 30, 'near', 'src/b.cpp'),       # maintainer lane: open
        0x2200: row(0x2200, 50, 'matched', 'src/b.cpp'),
        0x2300: row(0x2300, 20, 'matched', 'src/c.cpp'),    # all matched
        0x2400: row(0x2400, 5, 'todo', 'src/stubs/x.cpp'),  # stub file
        0x2500: row(0x2500, 8, 'todo', 'src/d.cpp'),
    }
    files = open_files(rows, person_claims(TABLE))
    assert [f[0] for f in files] == ['src/d.cpp', 'src/b.cpp']
    assert files[1][2] == 30


def test_claim_text_range_or_list():
    near = {0x2100: row(0x2100, 30, 'near', 'src/b.cpp'), 0x2200: row(0x2200, 50, 'matched', 'src/b.cpp')}
    assert claim_text(near, ('src/b.cpp', [0x2100, 0x2200], 30, None)) == \
        'Retail range claimed: `0x2100`-`0x2200` (src/b.cpp)'
    far = {0x2100: row(0x2100, 30, 'near', 'src/b.cpp'), 0x9000: row(0x9000, 50, 'matched', 'src/b.cpp')}
    assert claim_text(far, ('src/b.cpp', [0x2100, 0x9000], 30, None)) == \
        'Retail range claimed: `0x2100` (the unmatched functions of src/b.cpp)'
