import random
import re

import pytest

import build
import permute
from linkmap import LinkMap

SOURCE = '''// header { not a body
// @retail 0x1234
long f(long a, long b)
{
	long x = a + b;   // "}" in a comment
	long y = 2;
	char const *s = "}{";
	if (x == y) {
		x += 1;
	} else {
		x = x - 1;
	}
	for (y = 0; y < 4; y++) {
		x ^= y;
	}
	return (x + y) & ~3;
}

long g(void) { return 0; }
'''


def body():
    start, end = permute.find_body(SOURCE, 0x1234)
    return SOURCE[start:end]


def rng():
    return random.Random(1)


def test_find_body_braces():
    b = body()
    assert b.startswith('\n\tlong x = a + b;') and b.endswith('~3;\n')
    assert 'g(void)' not in b


def test_find_body_after_a_standard_marker():
    text = SOURCE.replace('// @retail 0x1234', '// @retail 0x1234 standard')
    start, end = permute.find_body(text, 0x1234)
    assert text[start:end] == body()


def test_find_body_missing():
    with pytest.raises(ValueError):
        permute.find_body(SOURCE, 0x99)


def test_swap_operands_commutative():
    out = permute.mut_swap_operands('\n\tx = a + b;\n', rng())
    assert out == '\n\tx = b + a;\n'
    assert permute.mut_swap_operands('\n\tx = a - b;\n', rng()) is None
    assert permute.mut_swap_operands('\n\tx = c * a + b;\n', rng()) is None
    assert permute.mut_swap_operands('\n\tif (a < b) f();\n', rng()) == '\n\tif (b > a) f();\n'


def test_reorder_independent_only():
    out = permute.mut_reorder('\n\tlong a = 1;\n\tlong b = 2;\n', rng())
    assert out == '\n\tlong b = 2;\n\tlong a = 1;\n'
    assert permute.mut_reorder('\n\tlong a = 1;\n\tlong b = a;\n', rng()) is None


def test_retype_changes_one_declaration():
    out = permute.mut_retype('\n\tlong a = 1;\n\treturn a;\n', rng())
    assert out != '\n\tlong a = 1;\n\treturn a;\n' and out.endswith('return a;\n')
    assert re.match(r'\n\t(int|short|unsigned long|dword) a = 1;', out)
    assert re.match(r'\n\tbyte c;', permute.mut_retype('\n\tchar c;\n', rng()))


def test_introduce_and_inline_temp_roundtrip():
    src = '\n\tlong r = (a + b) & ~3;\n\treturn r;\n'
    out = permute.mut_introduce_temp(src, rng())
    assert re.search(r'tmp0 = a \+ b;', out) and 'tmp0 & ~3' in out
    inlined = permute.mut_inline_temp('\n\tlong t = a + b;\n\treturn t;\n', rng())
    assert inlined == '\n\treturn (a + b);\n'
    assert permute.mut_inline_temp('\n\tlong t = a + b;\n\tt += 1;\n\treturn t;\n', rng()) is None


def test_swap_if_else_negates():
    src = '\n\tif (a == b) {\n\t\tx = 1;\n\t} else {\n\t\tx = 2;\n\t}\n'
    out = permute.mut_swap_if_else(src, rng())
    assert 'if (a != b)' in out
    assert out.index('x = 2;') < out.index('x = 1;')
    assert permute.mut_swap_if_else('\n\tif (a) {\n\t\tx = 1;\n\t}\n', rng()) is None


def test_for_while_roundtrip():
    src = '\n\tfor (i = 0; i < 4; i++) {\n\t\tx += i;\n\t}\n'
    w = permute.mut_for_while(src, rng())
    assert 'while (i < 4) {' in w and w.count('i++;') == 1 and 'for' not in w
    assert permute.mut_for_while(w, rng()) == src


def test_compound_assignment_both_ways():
    assert permute.mut_compound('\n\tx += y;\n', rng()) == '\n\tx = x + y;\n'
    assert permute.mut_compound('\n\tx = x + y;\n', rng()) == '\n\tx += y;\n'
    assert permute.mut_compound('\n\tx += y + 1;\n', rng()) == '\n\tx = x + (y + 1);\n'


def test_increment_forms():
    assert permute.mut_increment('\n\ti++;\n', rng()) == '\n\t++i;\n'
    assert permute.mut_increment('\n\t--i;\n', rng()) == '\n\ti--;\n'
    assert 'for (i = 0; i < 4; ++i)' in permute.mut_increment('\n\tfor (i = 0; i < 4; i++) {\n', rng())


def test_split_decl_both_ways():
    assert permute.mut_split_decl('\n\tlong a = b;\n', rng()) == '\n\tlong a;\n\ta = b;\n'
    assert permute.mut_split_decl('\n\tlong a;\n\ta = b;\n', rng()) == '\n\tlong a = b;\n'


def test_logical_operators_respect_precedence():
    split = permute._split_logical
    assert split('a && f(b, c) && d[0]', '&&') == ['a', 'f(b, c)', 'd[0]']
    assert split('a >= b && c <= d', '&&') == ['a >= b', 'c <= d']
    assert split('a && b || c', '||') == ['a && b', 'c']
    assert split('a && b || c', '&&') is None
    assert split('x = a && b', '&&') is None
    assert split('x <<= 1 && b', '&&') is None
    assert split('a ? b : c && d', '&&') is None
    assert split('(a && b)', '&&') is None


def test_chain_assign_both_ways():
    out = permute.mut_chain_assign('\n\ta = b = 0;\n', rng())
    assert out in ('\n\tb = 0;\n\ta = 0;\n', '\n\ta = 0;\n\tb = 0;\n')
    assert permute.mut_chain_assign('\n\ta = 0;\n\tb = 0;\n', rng()) in ('\n\ta = b = 0;\n', '\n\tb = a = 0;\n')
    assert permute.mut_chain_assign('\n\ta = b = 300;\n', rng()) is None  # a narrow b could change what a gets
    assert permute.mut_chain_assign('\n\ta = b = 2;\n', rng()) is None  # a bool b would make a 1
    assert permute.mut_chain_assign('\n\ta = 0;\n\tb = 1;\n', rng()) is None
    assert permute.mut_chain_assign('\n\tp = 0;\n\tp->x = 0;\n', rng()) is None  # the second place depends on p


def test_bool_flag_both_ways():
    src = '\n\tif (a && b) {\n\t\tf();\n\t}\n'
    out = permute.mut_bool_flag(src, rng())
    assert out == '\n\tbool flag0 = false;\n\tif (a)\n\t\tflag0 = b;\n\tif (flag0) {\n\t\tf();\n\t}\n'
    assert permute.mut_bool_flag(out, rng()) == src
    named = permute.mut_bool_flag('\n\tbool flag0 = false;\n\tx = 1;\n\tif (a && b) {\n\t}\n', rng())
    assert 'bool flag1 = false;' in named
    folded = '\n\tbool flag0 = false;\n\tif (a || b)\n\t\tflag0 = c;\n\tif (flag0) {\n\t}\n'
    assert permute.mut_bool_flag(folded, rng()) == '\n\tif ((a || b) && c) {\n\t}\n'
    assert permute.mut_bool_flag('\n\tif (a && b || c) {\n\t}\n', rng()) is None
    assert permute.mut_bool_flag('\n\telse\n\tif (a && b) {\n\t}\n', rng()) is None  # the else would own the declaration


def test_or_split_both_ways():
    src = '\n\tif (a || b) {\n\t\tf();\n\t}\n'
    out = permute.mut_or_split(src, rng())
    assert out == '\n\tif (a) {\n\t\tf();\n\t} else if (b) {\n\t\tf();\n\t}\n'
    assert permute.mut_or_split(out, rng()) == src
    kept = permute.mut_or_split('\n\tif (a || b) {\n\t\tf();\n\t} else {\n\t\tg();\n\t}\n', rng())
    assert kept.endswith('} else if (b) {\n\t\tf();\n\t} else {\n\t\tg();\n\t}\n')
    assert permute.mut_or_split('\n\tif (a || b) {\n\t\tstatic long n;\n\t}\n', rng()) is None  # a copy would be a new n


def test_and_split_both_ways():
    src = '\n\tif (a && b) {\n\t\tf();\n\t}\n'
    out = permute.mut_or_split(src, rng())
    assert out == '\n\tif (a) {\n\t\tif (b) {\n\t\t\tf();\n\t\t}\n\t}\n'
    assert permute.mut_or_split(out, rng()) == src
    assert permute.mut_or_split('\n\tif (a && b) {\n\t\tf();\n\t} else {\n\t\tg();\n\t}\n', rng()) is None
    nested_else = '\n\tif (a) {\n\t\tif (b) {\n\t\t\tf();\n\t\t} else {\n\t\t\tg();\n\t\t}\n\t}\n'
    assert permute.mut_or_split(nested_else, rng()) is None


def test_ternary_both_ways():
    src = '\n\tx = a < b ? 1 : 0;\n'
    out = permute.mut_ternary(src, rng())
    assert out == '\n\tif (a < b)\n\t\tx = 1;\n\telse\n\t\tx = 0;\n'
    assert permute.mut_ternary(out, rng()) == src
    braced = '\n\tif (a) {\n\t\tx = NONE;\n\t} else {\n\t\tx = 2;\n\t}\n'
    assert permute.mut_ternary(braced, rng()) == '\n\tx = a ? NONE : 2;\n'
    assert permute.mut_ternary('\n\ty = x = a ? 1 : 0;\n', rng()) is None  # the condition would be 'x = a'
    assert permute.mut_ternary('\n\tx = a ? 1 : b;\n', rng()) is None
    assert permute.mut_ternary('\n\tx = a ? -1 : 0x80000000;\n', rng()) is None  # -1 would become unsigned
    assert permute.mut_ternary('\n\tif (a)\n\t\tx = 1;\n\telse\n\t\ty = 2;\n', rng()) is None


def test_swap_ternary_negates():
    assert permute.mut_swap_ternary('\n\tx = a == b ? c : d;\n', rng()) == '\n\tx = a != b ? d : c;\n'
    assert permute.mut_swap_ternary('\n\treturn !f ? 1 : 2;\n', rng()) == '\n\treturn f ? 2 : 1;\n'
    assert permute.mut_swap_ternary('\n\ty = x = a ? c : d;\n', rng()) is None


def test_swap_args_trades_two_arguments():
    swap = permute.mut_swap_args
    seen = {swap('\n\tf(a, 0x2a, b);\n', random.Random(s)) for s in range(60)}
    swaps = {'\n\tf(0x2a, a, b);\n', '\n\tf(b, 0x2a, a);\n', '\n\tf(a, b, 0x2a);\n'}
    rotations = {'\n\tf(0x2a, b, a);\n', '\n\tf(b, a, 0x2a);\n'}      # an argument moved two places
    assert seen == swaps | rotations
    assert swap('\n\tf( a , b );\n', rng()) == '\n\tf( b , a );\n'
    assert swap('\n\tf("a,b", c);\n', rng()) == '\n\tf(c, "a,b");\n'
    nested = {swap('\n\tf(g(a, b), c);\n', random.Random(s)) for s in range(60)}
    assert nested == {'\n\tf(c, g(a, b));\n', '\n\tf(g(b, a), c);\n'}
    assert swap('\n\tf(a, a);\n', rng()) is None and swap('\n\tf(a);\n', rng()) is None
    assert swap('\n\tif (a, b) return (c, d);\n', rng()) is None
    assert swap('\n\t// f(a, b)\n\tx = 1;\n', rng()) is None  # a comment is not a call


def test_volatile_forms_both_ways():
    read = permute.mut_volatile_read
    assert read('\n\tlong x = p->f;\n', rng()) == '\n\tlong x = *(volatile long *)&p->f;\n'
    assert read('\n\tlong x = *(volatile long *)&p->f;\n', rng()) == '\n\tlong x = p->f;\n'
    assert read('\n\treal v = g_4cf478;\n', rng()) == '\n\treal v = *(volatile real *)&g_4cf478;\n'
    assert read('\n\te->pending = true;\n', rng()) == '\n\t*(volatile bool *)&e->pending = true;\n'
    assert read('\n\t*(volatile bool *)&e->pending = true;\n', rng()) == '\n\te->pending = true;\n'
    assert read('\n\tlong x = f(p);\n', rng()) is None
    pointer = permute.mut_volatile_pointer
    assert pointer('\n\ts_slot *h = g[type];\n', rng()) == '\n\ts_slot volatile *h = g[type];\n'
    assert pointer('\n\ts_slot volatile *h = g[type];\n', rng()) == '\n\ts_slot *h = g[type];\n'
    assert pointer('\n\ts_slot const *h = g[type];\n', rng()) == '\n\ts_slot volatile const *h = g[type];\n'
    assert pointer('\n\treturn *p = 3;\n', rng()) is None


def test_mutate_favours_the_edits_that_worked_before():
    body = '\n\tlong a = 1;\n\tlong b = 2;\n\tx = a + b;\n\tf(a, b);\n'
    firsts = [permute.mutate(body, random.Random(s), count=1) for s in range(300)]
    swapped = sum(1 for out in firsts if out is not None and 'f(b, a);' in out)
    assert swapped > 300 * 0.25


IDIOMS = '''
	if (a && b) {
		x = c ? 1 : 0;
	}
	if (a || d) {
		y = z = 0;
	}
	return x ? y : z;
'''


def test_idiom_mutations_keep_the_body_balanced():
    seen = set()
    for seed in range(200):
        out = permute.mutate(IDIOMS, random.Random(seed))
        if out is not None:
            seen.add(out)
            assert out.count('{') == out.count('}') and out.count('(') == out.count(')')
    assert len(seen) > 20


def test_mutate_is_deterministic_and_valid_looking():
    b = body()
    first = [permute.mutate(b, random.Random(7)) for _ in range(3)]
    assert first[0] == first[1] == first[2] and first[0] != b
    for seed in range(30):
        out = permute.mutate(b, random.Random(seed))
        assert out is None or all(out.count(c) == b.count(c) for c in '{}()')


@pytest.mark.sdk
@pytest.mark.retail
def test_permute_runs_on_game_state_malloc_aligned(retail_xbe, xdk_dir):
    result = permute.run(0x123d80, tries=3, seed=1, log=lambda *_: None)
    # once the function matches (baseline 0) there is nothing to try
    assert result.tries >= 1 or result.baseline == 0
    assert 0 <= result.best <= result.baseline


def test_missing_symbol_is_a_failed_variant_not_a_crash(monkeypatch, tmp_path):
    """A variant the linker folds out of the image scores as a failure.

    check.resolve returns None for that. Scoring used to call extent() on it
    and raise AttributeError, which aborts the whole search.
    """
    scorer = permute.Scorer.__new__(permute.Scorer)
    scorer.va = 0x1000
    scorer.root = str(tmp_path)
    scorer.xdk = None
    scorer.image = None
    scorer.path = str(tmp_path / 'f.cpp')
    scorer.row = {'size': '4'}
    scorer.rows = {}
    (tmp_path / 'f.cpp').write_text('original')

    monkeypatch.setattr(permute.build, 'build', lambda *a, **k: 'map')
    monkeypatch.setattr(permute.LinkMap, 'read', staticmethod(lambda path: object()))
    monkeypatch.setattr(permute, 'Pe', lambda path: object())
    monkeypatch.setattr(permute.build, 'marked_sources', lambda root: [type('M', (), {'retail': 0x1000})()])
    monkeypatch.setattr(permute.build, 'stub_sources', lambda root: [])
    monkeypatch.setattr(permute.check, 'StandinCalls', lambda *a, **k: object())
    monkeypatch.setattr(permute.check, 'Identity', lambda *a, **k: object())
    monkeypatch.setattr(permute.check, 'resolve', lambda *a, **k: None)

    assert scorer.score_text('variant') == (permute.WORST, 'not in the image')
    assert (tmp_path / 'f.cpp').read_text() == 'variant'


# ------------------------------------------------------------ --fast

def marker(va, path, stub=False):
    return build.Marked(path, va, f'function_{va:x}', 'void', [], stub)


# 0x100 calls 0x200 (decompiled) and 0x300 (a stub); 0x200 calls 0x400, which
# calls 0x500; 0x300's own callee 0x600 must not be followed (stubs are built
# without LTCG); 0x700 (library code) has no marker; 0x800 calls 0x100.
CALLS = {0x100: '00000200 00000300 00000700', 0x200: '00000400', 0x300: '00000600', 0x400: '00000500',
         0x500: '', 0x600: '', 0x700: '', 0x800: '00000100'}
ROWS = {va: {'va': f'{va:08x}', 'calls': calls} for va, calls in CALLS.items()}
MARKERS = [marker(0x100, 'src/a.cpp'), marker(0x200, 'src/b.cpp'), marker(0x300, 'src/stubs/s.cpp', stub=True),
           marker(0x400, 'src/c.cpp'), marker(0x500, 'src/d.cpp'), marker(0x600, 'src/e.cpp'),
           marker(0x800, 'src/f.cpp')]


def test_reduced_units_follow_retail_callees_to_the_given_depth():
    assert permute.reduced_units(0x100, ROWS, MARKERS, depth=0) == {'src/a.cpp'}
    assert permute.reduced_units(0x100, ROWS, MARKERS, depth=1) == {'src/a.cpp', 'src/b.cpp', 'src/stubs/s.cpp'}
    assert permute.reduced_units(0x100, ROWS, MARKERS, depth=2) == {'src/a.cpp', 'src/b.cpp', 'src/stubs/s.cpp',
                                                                    'src/c.cpp'}
    assert permute.reduced_units(0x100, ROWS, MARKERS) == {'src/a.cpp', 'src/b.cpp', 'src/stubs/s.cpp',
                                                           'src/c.cpp', 'src/d.cpp'}


def test_reduced_units_stop_at_stubs_and_leave_out_callers():
    units = permute.reduced_units(0x100, ROWS, MARKERS)
    assert 'src/e.cpp' not in units and 'src/f.cpp' not in units


def test_reduced_units_survive_call_cycles():
    rows = {0x100: {'calls': '00000200'}, 0x200: {'calls': '00000100'}}
    markers = [marker(0x100, 'src/a.cpp'), marker(0x200, 'src/b.cpp')]
    assert permute.reduced_units(0x100, rows, markers) == {'src/a.cpp', 'src/b.cpp'}


MAP = '''  Address         Publics by Value              Rva+Base     Lib:Object

 0000:00000000       ___safe_se_handler_table   00000000     <absolute>
 0001:00000000       ?sqrt@@YAMM@Z              00401000 f i ai.obj
 0001:00000010       ??1CLocalTalker@XHVNamespace@@QAE@XZ 00401010 f   xvoice:localtalker.obj
 0001:00000020       ?Talk@CLocalTalker@XHVNamespace@@QAEXXZ 00401020 f   xvoice:localtalker.obj
 0001:00000030       __fpclear                  00401030 f   libcmt:fpinit.obj
 0001:00000040       _D3DDevice_Clear@24        00401040 f   d3d8ltcg:device.obj

 Static symbols

 0001:00000050       _helper                    00401050 f   libcmt:other.obj
'''


def test_library_symbols_take_one_public_symbol_per_non_ltcg_library_object():
    assert permute.library_symbols(LinkMap(MAP)) == ['??1CLocalTalker@XHVNamespace@@QAE@XZ', '__fpclear']


PINNING_MAP = '''  Preferred load address is 00400000

 0001:00000000 00001000H .text                   CODE
 0002:00000000 00000100H .data                   DATA

  Address         Publics by Value              Rva+Base     Lib:Object

 0001:00000000       @target@4                  00401000 f   a.obj
 0001:00000020       @callee@4                  00401020 f   b.obj
 0001:00000040       @other@4                   00401040 f   c.obj
 0001:00000060       @stub_caller@4             00401060 f   stubs_s.obj
 0001:00000080       @ltcg_caller@4             00401080 f   d.obj
 0002:00000000       ?table@@3PAXA              00402000     e.obj

FIXUPS: 1005 60 20
'''


class Image:
    """A Pe stand-in: absolute fixups and the dwords stored in the image."""

    def __init__(self, fixups, dwords):
        self.fixups, self.dwords = fixups, dwords

    def read(self, va, size):
        return self.dwords[va].to_bytes(4, 'little', signed=self.dwords[va] < 0)


def test_pinning_units_take_address_takers_and_stub_callers():
    """target calls callee; e.obj's table holds target's address and c.obj
    takes callee's; a stub calls target; d.obj's LTCG code calls target."""
    linkmap = LinkMap(PINNING_MAP)
    image = Image({0x402000, 0x401045}, {
        0x401005: 0x401020 - 0x401009, 0x401065: 0x401000 - 0x401069, 0x401085: 0x401000 - 0x401089,
        0x402000: 0x401000, 0x401045: 0x401020})
    sources = {'a.obj': 'src/a.cpp', 'b.obj': 'src/b.cpp', 'c.obj': 'src/c.cpp', 'd.obj': 'src/d.cpp',
               'e.obj': 'src/e.cpp', 'stubs_s.obj': 'src/stubs/s.cpp'}
    target = linkmap.find('target')[0]
    assert permute.pinning_units(linkmap, image, [target], sources) == {'src/c.cpp', 'src/e.cpp', 'src/stubs/s.cpp'}


def search(full, log=None):
    """A FastSearch over body 'B' (full score 4) whose confirming build scores with full (a dict)."""
    confirmed = []

    def confirm(text):
        confirmed.append(text)
        return full[text], f'full {text}'

    s = permute.FastSearch('<', 'B', '>', confirm, 4, log or (lambda *_: None))
    s.start(5, 'fast baseline')
    return s, confirmed


def test_fast_improvement_is_confirmed_by_a_full_build():
    lines = []
    s, confirmed = search({'<C>': 2}, lines.append)
    s.offer('C', 3, 'fast C', 1)
    assert confirmed == ['<C>']
    assert (s.baseline, s.best, s.best_score, s.walk, s.walk_score) == (4, 'C', 2, 'C', 3)
    assert s.confirmations == 1
    assert any('3 on the reduced image' in l and '2 in the full image' in l and 'new best' in l for l in lines)


def test_fast_improvement_the_full_build_rejects_moves_nothing():
    lines = []
    s, confirmed = search({'<C>': 4}, lines.append)
    s.offer('C', 3, 'fast C', 1)
    assert (s.best, s.best_score, s.walk, s.walk_score) == ('B', 4, 'B', 5)
    assert any('3 on the reduced image' in l and '4 in the full image' in l and 'rejected' in l for l in lines)


def test_fast_search_warns_when_the_baselines_disagree():
    lines = []
    search({}, lines.append)  # full baseline 4, reduced 5
    assert any(l.startswith('warning: the reduced image scores the original 5, the full image 4') for l in lines)


def test_fast_plateau_moves_the_walk_without_a_full_build():
    s, confirmed = search({})
    s.offer('C', 5, 'same', 1)
    s.offer('D', 6, 'worse', 2)
    assert (s.walk, s.walk_score, s.best, s.best_score, confirmed) == ('C', 5, 'B', 4, [])


def test_fast_search_stops_at_a_full_match():
    s, confirmed = search({'<C>': 3, '<D>': 0})
    s.offer('C', 3, '', 1)
    s.offer('D', 1, '', 2)
    assert confirmed == ['<C>', '<D>']
    assert (s.best, s.best_score, s.done) == ('D', 0, True)
    assert s.propose(random.Random(0), 100) == (None, None)


def test_fast_search_best_always_has_a_full_build_score():
    """A fast scorer that claims every variant improves: each is confirmed,
    and the best is always a variant whose full score is the lowest seen."""
    class Fast:
        n = 100

        def score_text(self, text):
            Fast.n -= 1
            return Fast.n, 'fast'

    rng = random.Random(5)
    full = {}

    def confirm(text):
        full.setdefault(text, rng.randrange(1, 20))  # never 0, which ends the search
        return full[text], 'full'

    start, end = permute.find_body(SOURCE, 0x1234)
    s = permute.FastSearch(SOURCE[:start], SOURCE[start:end], SOURCE[end:], confirm, confirm(SOURCE)[0],
                           lambda *_: None)
    permute._work(s, Fast(), random.Random(3), 8, None)
    assert s.tries == 8 and s.confirmations == 8
    assert s.best_score == min(full.values()) and full[s.text(s.best)] == s.best_score
    assert s.text(s.best) in full


def test_jobs_needs_fast(monkeypatch, capsys):
    monkeypatch.setattr(permute.sys, 'argv', ['permute.py', '0x1234', '--jobs', '2'])
    with pytest.raises(SystemExit):
        permute.main()
    assert '--jobs' in capsys.readouterr().err


# ------------------------------------------------------- typed mutations

HEADER = '''typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef float real;
#define NONE -1

struct s_point
{
	long x;
	short y;
};

struct s_thing
{
	s_point *point;
	long value;
	word flags;
	short count;
	bool ready;
};

extern s_thing *g_thing;
long helper(long a, short b);
bool test(long a);
void store(long a, bool b);
#define TEST_FIELD_BIT(field) ((bool)(field))
static inline long doubled(long a) { return a + a; }
static inline bool has_flags(s_thing *t, word mask) { return (t->flags & mask) != 0; }
'''
PARAMS = 's_thing *thing, long index, short count'


def context():
    return permute.Context(PARAMS, 'long', HEADER)


def outputs(mut, body, seeds=40):
    """The distinct results of a typed mutation over many seeds; each must still parse."""
    found = set()
    for seed in range(seeds):
        out = mut(body, random.Random(seed), context())
        if out is not None:
            assert parses(out), out
            found.add(out)
    return found


def parses(body):
    """Balanced braces, parentheses and brackets, and find_body takes the body back whole."""
    if any(body.count(a) != body.count(b) for a, b in ('{}', '()', '[]')):
        return False
    text = f'// @retail 0x1234\nlong f({PARAMS})\n{{{body}}}\n'
    start, end = permute.find_body(text, 0x1234)
    return text[start:end] == body


# (mutation, body, the results it must give): each body is valid C++ in
# long f(s_thing *thing, long index, short count) after HEADER, and so is each result
CASES = [
    (permute.mut_reorder_fields, '\n\tthing->value = 0;\n\tthing->count = 1;\n\treturn 0;\n',
     {'\n\tthing->count = 1;\n\tthing->value = 0;\n\treturn 0;\n'}),
    (permute.mut_move_statement, '\n\tthing->value = 0;\n\tthing->count = 1;\n\tindex = 2;\n\treturn 0;\n',
     {'\n\tthing->count = 1;\n\tindex = 2;\n\tthing->value = 0;\n\treturn 0;\n',
      '\n\tindex = 2;\n\tthing->value = 0;\n\tthing->count = 1;\n\treturn 0;\n'}),
    (permute.mut_hoist_argument, '\n\tstore(thing->value, index != 0);\n\treturn 0;\n',
     {'\n\tlong tmp0 = thing->value;\n\tstore(tmp0, index != 0);\n\treturn 0;\n',
      '\n\tbool tmp0 = index != 0;\n\tstore(thing->value, tmp0);\n\treturn 0;\n'}),
    (permute.mut_hoist_argument, '\n\tif (index)\n\t\tstore(helper(index, count), true);\n\treturn 0;\n',
     {'\n\tif (index)\n\t{\n\t\tlong tmp0 = helper(index, count);\n\t\tstore(tmp0, true);\n\t}\n\treturn 0;\n'}),
    (permute.mut_inline_local, '\n\tshort s = thing->count;\n\tstore(s, true);\n\treturn 0;\n',
     {'\n\tstore(thing->count, true);\n\treturn 0;\n'}),
    (permute.mut_inline_local, '\n\tshort s = thing->value;\n\tstore(s, true);\n\treturn 0;\n',
     {'\n\tstore((short)thing->value, true);\n\treturn 0;\n'}),
    (permute.mut_cache_field,
     '\n\tstore(thing->value, true);\n\tstore(thing->value + 1, false);\n\tthing->value = 0;\n\treturn thing->value;\n',
     {'\n\tlong tmp0 = thing->value;\n\tstore(tmp0, true);\n\tstore(tmp0 + 1, false);\n\tthing->value = 0;\n'
      '\treturn thing->value;\n'}),
    (permute.mut_uncache_field, '\n\tword flags = thing->flags;\n\tif (flags & 1)\n\t\tstore(index, (flags & 2) != 0);\n'
                                '\treturn 0;\n',
     {'\n\tif (thing->flags & 1)\n\t\tstore(index, (thing->flags & 2) != 0);\n\treturn 0;\n'}),
    (permute.mut_zero_compare, '\n\tif (index && !count)\n\t\treturn 1;\n\treturn 0;\n',
     {'\n\tif (index != 0 && !count)\n\t\treturn 1;\n\treturn 0;\n',
      '\n\tif (index && count == 0)\n\t\treturn 1;\n\treturn 0;\n'}),
    (permute.mut_zero_compare, '\n\tif (index != 0 || (count & 3) == 0)\n\t\treturn 1;\n\treturn 0;\n',
     {'\n\tif (index || (count & 3) == 0)\n\t\treturn 1;\n\treturn 0;\n',
      '\n\tif (index != 0 || !(count & 3))\n\t\treturn 1;\n\treturn 0;\n'}),
    (permute.mut_zero_compare, '\n\tbool ok = test(index);\n\tthing->ready = test(count) != 0;\n\treturn ok;\n',
     {'\n\tbool ok = test(index) != 0;\n\tthing->ready = test(count) != 0;\n\treturn ok;\n',
      '\n\tbool ok = test(index);\n\tthing->ready = test(count);\n\treturn ok;\n'}),
    (permute.mut_early_return, '\n\tlong result = 0;\n\tif (index)\n\t{\n\t\tresult = 1;\n\t}\n\treturn result;\n',
     {'\n\tlong result = 0;\n\tif (index)\n\t{\n\t\treturn 1;\n\t}\n\treturn result;\n'}),
    (permute.mut_early_return, '\n\tlong result;\n\tif (index)\n\t{\n\t\tresult = 1;\n\t\tgoto done;\n\t}\n\tresult = 2;\n'
                               'done:\n\treturn result;\n',
     {'\n\tlong result;\n\tif (index)\n\t{\n\t\treturn 1;\n\t}\n\tresult = 2;\ndone:\n\treturn result;\n',
      '\n\tlong result;\n\tif (index)\n\t{\n\t\tresult = 1;\n\t\tgoto done;\n\t}\n\treturn 2;\ndone:\n\treturn result;\n'}),
    (permute.mut_early_return, '\n\tlong result = 0;\n\tif (index)\n\t{\n\t\treturn 1;\n\t}\n\tresult = 2;\n\treturn result;\n',
     {'\n\tlong result = 0;\n\tif (index)\n\t{\n\t\tresult = 1;\n\t}\n\telse\n\t{\n\t\tresult = 2;\n\t}\n'
      '\treturn result;\n',
      '\n\tlong result = 0;\n\tif (index)\n\t{\n\t\treturn 1;\n\t}\n\treturn 2;\n\treturn result;\n'}),
    (permute.mut_volatile_local, '\n\tlong a = index;\n\ts_point *p = thing->point;\n\treturn a + p->x;\n',
     {'\n\tvolatile long a = index;\n\ts_point *p = thing->point;\n\treturn a + p->x;\n',
      '\n\tlong a = index;\n\ts_point *volatile p = thing->point;\n\treturn a + p->x;\n'}),
    # the local's own qualifier comes off; what a pointer points to stays volatile (mut_volatile_pointer's)
    (permute.mut_volatile_local, '\n\tvolatile long a = index;\n\ts_point volatile *volatile p = 0;\n\treturn a;\n',
     {'\n\tlong a = index;\n\ts_point volatile *volatile p = 0;\n\treturn a;\n',
      '\n\tvolatile long a = index;\n\ts_point volatile *p = 0;\n\treturn a;\n'}),
    (permute.mut_retype_wide, '\n\tshort s = count;\n\treturn s;\n',
     {'\n\tword s = count;\n\treturn s;\n', '\n\tchar s = count;\n\treturn s;\n'}),
    (permute.mut_retype_wide, '\n\tword w = count;\n\treturn w;\n',
     {'\n\tshort w = count;\n\treturn w;\n', '\n\tbyte w = count;\n\treturn w;\n', '\n\tdword w = count;\n\treturn w;\n'}),
    (permute.mut_recast, '\n\treturn *(short *)&index;\n',
     {'\n\treturn *(word *)&index;\n', '\n\treturn *(char *)&index;\n', '\n\treturn *(long *)&index;\n'}),
    (permute.mut_redundant_cast, '\n\tstore((long)index, true);\n\treturn 0;\n', {'\n\tstore(index, true);\n\treturn 0;\n'}),
    (permute.mut_redundant_cast, '\n\tstore(index, true);\n\treturn 0;\n', {'\n\tstore((long)index, true);\n\treturn 0;\n'}),
    (permute.mut_do_while, '\n\twhile (index > 0)\n\t{\n\t\tindex--;\n\t}\n\treturn index;\n',
     {'\n\tif (index > 0)\n\t{\n\t\tdo\n\t\t{\n\t\t\tindex--;\n\t\t} while (index > 0);\n\t}\n\treturn index;\n'}),
    (permute.mut_single_exit, '\n\tif (index)\n\t\treturn 1;\n\treturn index + 2;\n',
     {'\n\tlong result;\n\tif (index)\n\t\treturn 1;\n\tresult = index + 2;\n\treturn result;\n'}),
    (permute.mut_inline_helper, '\n\treturn doubled(index) + has_flags(thing, 4);\n',
     {'\n\treturn ((long)(index + index)) + has_flags(thing, 4);\n',
      '\n\treturn doubled(index) + ((bool)((thing->flags & 4) != 0));\n'}),
    (permute.mut_test_field_bit, '\n\tif (thing->ready && !g_thing->ready)\n\t\treturn 1;\n\treturn 0;\n',
     {'\n\tif (TEST_FIELD_BIT(thing->ready) && !g_thing->ready)\n\t\treturn 1;\n\treturn 0;\n',
      '\n\tif (thing->ready && !TEST_FIELD_BIT(g_thing->ready))\n\t\treturn 1;\n\treturn 0;\n'}),
    (permute.mut_test_field_bit, '\n\twhile (TEST_FIELD_BIT(thing->ready))\n\t\tindex++;\n\treturn index;\n',
     {'\n\twhile (thing->ready)\n\t\tindex++;\n\treturn index;\n'}),
]


@pytest.mark.parametrize('mut, body, expected', CASES, ids=[f'{c[0].__name__}-{k}' for k, c in enumerate(CASES)])
def test_typed_mutation_gives_the_intended_rewrites(mut, body, expected):
    assert parses(body)
    assert outputs(mut, body) == expected


def test_typed_mutations_cover_every_one():
    assert {c[0] for c in CASES} == set(permute.TYPED_MUTATIONS)
    assert not set(permute.TYPED_MUTATIONS) & set(permute.MUTATIONS)


def test_do_while_round_trip():
    body = '\n\twhile (index > 0)\n\t{\n\t\tindex--;\n\t}\n\treturn index;\n'
    there = permute.mut_do_while(body, rng(), context())
    assert permute.mut_do_while(there, rng(), context()) == body


def test_typed_mutations_leave_alone_what_they_must():
    ctx = context()
    # reads the field the other writes; two calls; pairs mut_reorder already takes
    assert permute.mut_reorder_fields('\n\tthing->value = 0;\n\tindex = thing->value;\n', rng(), ctx) is None
    assert permute.mut_reorder_fields('\n\tstore(thing->value, true);\n\tstore(thing->value, false);\n', rng(),
                                      ctx) is None
    assert permute.mut_reorder_fields('\n\tlong a = 1;\n\tlong b = 2;\n', rng(), ctx) is None
    # the middle statement reads what the first writes, so neither end can pass it; two calls
    assert permute.mut_move_statement('\n\tindex = 1;\n\tthing->value = index;\n\tindex = 2;\n', rng(), ctx) is None
    assert permute.mut_move_statement('\n\tstore(index, true);\n\tthing->value = 0;\n\tstore(index, false);\n', rng(),
                                      ctx) is None
    # an argument whose type nothing declares, and trivial ones
    assert permute.mut_hoist_argument('\n\tstore(unknown->field, true);\n', rng(), ctx) is None
    assert permute.mut_hoist_argument('\n\tstore(index, true);\n', rng(), ctx) is None
    # mut_inline_temp's own types
    assert permute.mut_inline_local('\n\tlong t = index + 1;\n\treturn t;\n', rng(), ctx) is None
    # a loop between the assignment and the return
    assert permute.mut_early_return('\n\tlong result = 0;\n\twhile (index)\n\t{\n\t\tresult = 1;\n\t}\n'
                                    '\treturn result;\n', rng(), ctx) is None
    # a's address is taken, so only p can be volatile
    assert permute.mut_volatile_local('\n\tlong a = 0;\n\tlong *p = &a;\n', rng(), ctx) == \
        '\n\tlong a = 0;\n\tlong *volatile p = &a;\n'
    # a local read once is the inline mutations' business
    assert permute.mut_uncache_field('\n\tword flags = thing->flags;\n\treturn flags;\n', rng(), ctx) is None
    # one exit already; the last return is a local (mut_early_return's case); a void function
    assert permute.mut_single_exit('\n\treturn index + 2;\n', rng(), ctx) is None
    assert permute.mut_single_exit('\n\tlong r = 0;\n\tif (index)\n\t\treturn 1;\n\treturn r;\n', rng(), ctx) is None
    assert permute.mut_single_exit('\n\tif (index)\n\t\treturn 1;\n\treturn 2;\n', rng(),
                                   permute.Context(PARAMS, 'void', HEADER)) is None
    # an argument the helper reads twice is evaluated once only when it is a name or a literal
    assert permute.mut_inline_helper('\n\treturn doubled(index + 1);\n', rng(), ctx) is None
    # a helper from another file (an included header) stays a call
    assert permute.mut_inline_helper('\n\treturn doubled(index);\n', rng(),
                                     permute.Context(PARAMS, 'long', HEADER, own='')) is None
    # no TEST_FIELD_BIT in the file, or a field that is not the whole condition
    assert permute.mut_test_field_bit('\n\tif (thing->ready)\n\t\treturn 1;\n\treturn 0;\n', rng(),
                                      permute.Context(PARAMS, 'long', '')) is None
    assert permute.mut_test_field_bit('\n\tif (thing->value > 2)\n\t\treturn 1;\n\treturn 0;\n', rng(), ctx) is None


def test_a_declaration_in_a_switch_gets_braces():
    body = '\n\tswitch (index)\n\t{\n\tcase 1:\n\t\tstore(thing->value, true);\n\t\tbreak;\n\t}\n\treturn 0;\n'
    out = permute.mut_hoist_argument(body, rng(), context())
    assert out == ('\n\tswitch (index)\n\t{\n\tcase 1:\n\t{\n\t\tlong tmp0 = thing->value;\n\t\tstore(tmp0, true);\n'
                   '\t}\n\t\tbreak;\n\t}\n\treturn 0;\n')


def test_expr_type_reads_declarations():
    ctx = context()
    types = permute.declared_types('\n\tlong *arguments = 0;\n\ts_point *p = 0;\n', ctx)
    assert types == {'thing': 's_thing *', 'index': 'long', 'count': 'short', 'arguments': 'long *',
                     'p': 's_point *'}
    for expr, t in [('arguments[1]', 'long'), ('*(short *)&arguments[2]', 'short'), ('thing->point->y', 'short'),
                    ('&thing->value', 'long *'), ('helper(1, 2)', 'long'), ('g_thing->flags', 'word'),
                    ('index != 0', 'bool'), ('(word)index', 'word'), ('index + 1', 'long'), ('store(1, 0)', None),
                    ('mystery->x', None)]:
        assert permute.expr_type(expr, types, ctx) == t, expr


def test_function_context_reads_the_signature(tmp_path):
    (tmp_path / 'include').mkdir()
    (tmp_path / 'include' / 'thing.h').write_text(HEADER)
    text = '#include "thing.h"\n// @retail 0x1234\nbool __stdcall c_x::f(s_thing *thing, long index)\n{\n\treturn 0;\n}\n'
    start, _ = permute.find_body(text, 0x1234)
    ctx = permute.function_context(text, start, str(tmp_path))
    assert (ctx.params, ctx.returns) == ('s_thing *thing, long index', 'bool')
    assert ctx.field_type('s_thing', 'flags') == 'word'
    assert ctx.field_type(None, 'flags') is None  # an expression of unknown type


def test_mutate_survives_bodies_the_typed_mutations_cannot_parse():
    body = '\n\twhile (x) {\n\t\ty = (z;\n\tif (q) {\n'  # unbalanced
    for seed in range(20):
        permute.mutate(body, random.Random(seed), mutations=permute.TYPED_MUTATIONS, context=context())


@pytest.mark.sdk
def test_typed_mutation_results_compile(xdk_dir, tmp_path):
    """Every body and result in CASES compiles (cl /Zs) as a function after HEADER."""
    bodies = {b for _, b, expected in CASES for b in [b, *expected]}
    source = HEADER + ''.join(f'long f{k}({PARAMS})\n{{{b}}}\n\n' for k, b in enumerate(sorted(bodies)))
    path = tmp_path / 'typed.cpp'
    path.write_text(source)
    build.run_tool('CL.Exe', ['/Zs', '/TP', str(path)], str(tmp_path), xdk_dir)


# ------------------------------------------------------- --anneal

class Draws:
    """A stand-in rng whose random() returns the given values in turn."""

    def __init__(self, *values):
        self.values = list(values)

    def random(self):
        return self.values.pop(0)


def test_anneal_accepts_worse_with_falling_probability():
    a = permute.Anneal(start=0.1, end=0.005)
    assert a.accepts(0, 0.0, None) and a.accepts(-2, 1.0, None)  # never draws for no worse
    assert not a.accepts(permute.WORST - 2, 0.0, Draws(0.0)) and a.deltas == []  # a failed build: never, not counted
    # the first worse variant is the typical one: 0.1 at the start
    assert a.accepts(6, 0.0, Draws(0.09)) and not a.accepts(6, 0.0, Draws(0.11))
    assert a.deltas == [6, 6]
    # a variant half as bad as the typical one: 0.1 ** 0.5 = 0.32; twice as bad: 0.01
    assert a.accepts(3, 0.0, Draws(0.3)) and not a.accepts(12, 0.0, Draws(0.02))
    assert a.deltas == [3, 6, 6, 12]
    # at the end of the run a typical one: 0.005
    assert abs(a.probability(6, 1.0) - 0.005) < 1e-12 and abs(a.probability(6, 0.5) - 0.1 * 0.05 ** 0.5) < 1e-12
    assert not permute.Greedy().accepts(1, 0.0, Draws(0.0))


def test_anneal_probability_does_not_depend_on_the_score_scale():
    small, large = permute.Anneal(), permute.Anneal()
    for d in (1, 2, 3):
        small.accepts(d, 0.0, Draws(1.0))
        large.accepts(10 * d, 0.0, Draws(1.0))
    assert abs(small.probability(2, 0.3) - large.probability(20, 0.3)) < 1e-12


def anneal_search(full, strategy=None, log=None):
    """A FastSearch over body 'B' (full score 4, reduced 5) with Anneal; confirm scores with full (a dict)."""
    confirmed = []

    def confirm(text):
        confirmed.append(text)
        return full[text], 'full'

    s = permute.FastSearch('<', 'B', '>', confirm, 4, log or (lambda *_: None), strategy or permute.Anneal())
    s.start(5, 'fast baseline')
    return s, confirmed


def test_anneal_walk_takes_a_worse_variant_without_a_full_build():
    s, confirmed = anneal_search({})
    s.offer('C', 6, 'worse', 1, Draws(0.0))
    assert (s.walk, s.walk_score, s.uphill, s.best, s.best_fast, confirmed) == ('C', 6, 1, 'B', 5, [])
    s.offer('D', 7, 'worse', 2, Draws(0.99))
    assert (s.walk, s.walk_score, s.uphill) == ('C', 6, 1)


def test_anneal_confirms_only_what_beats_the_best_not_the_walk():
    """From a worse position, a variant better than the position but not than
    the best moves the walk; only one that beats the best costs a full build."""
    s, confirmed = anneal_search({'<E>': 3})
    s.offer('C', 7, 'worse', 1, Draws(0.0))
    s.offer('D', 6, 'better than the walk', 2, Draws())
    assert (s.walk, s.walk_score, confirmed) == ('D', 6, [])
    s.offer('E', 4, 'better than the best', 3, Draws())
    assert confirmed == ['<E>']
    assert (s.best, s.best_score, s.best_fast, s.walk, s.walk_score, s.stalled) == ('E', 3, 4, 'E', 4, 0)


def test_anneal_restarts_from_the_best_then_the_original_when_it_stalls():
    lines = []
    body = '\n\tx = a + b;\n\ty = c * d;\n'
    better, worse = '\n\tx = b + a;\n\ty = c * d;\n', '\n\tx = a + b;\n\ty = d * c;\n'
    s = permute.FastSearch('<', body, '>', lambda t: (2, 'full'), 4, lines.append, permute.Anneal(stall=2),
                           permute.MUTATIONS)
    s.start(5, 'fast')
    s.offer(better, 4, 'better', 1, Draws())  # confirmed: the new best
    s.offer(worse, 6, 'worse', 2, Draws(0.0))
    assert s.walk == worse and s.stalled == 1
    s.offer('\n\tx = a + b;\n\ty = c;\n', 9, 'worse', 3, Draws(0.0))
    s.propose(random.Random(0), 100)
    assert s.restarts == 1 and any('restart 1 from the best' in l for l in lines)
    s.stalled = 2
    s.propose(random.Random(0), 100)
    assert s.restarts == 2 and any('restart 2 from the original' in l for l in lines)
    assert (s.walk, s.walk_score) == (body, 5)


def test_anneal_restarts_when_the_position_runs_dry_then_ends():
    """When no new variant turns up, the walk restarts with more mutations at
    a time, and the search ends only after its rounds of that."""
    lines = []
    s = permute.FastSearch('<', '\n\tx = a + b;\n', '>', lambda t: (9, 'full'), 9, lines.append, permute.Anneal(),
                           [permute.mut_swap_operands])
    s.start(9, 'fast')
    rng = random.Random(4)
    proposals = []
    while True:
        c, n = s.propose(rng, 1000)
        if c is None:
            break
        proposals.append(c)
        s.offer(c, 9, '', n, rng)
    assert proposals == ['\n\tx = b + a;\n']
    assert s.end == 'no new mutations' and s.restarts == permute.Anneal().rounds - 1
    assert lines[-1] == 'no new mutations available'


def test_anneal_stub_search_keeps_the_best_confirmed():
    """With a random reduced score and a random full score, the best is always
    the confirmed variant with the lowest full score, and the walk wanders."""
    rng = random.Random(9)
    full = {}

    def confirm(text):
        full.setdefault(text, rng.randrange(1, 30))
        return full[text], 'full'

    class Fast:
        def score_text(self, text):
            return rng.randrange(1, 12), 'fast'

    start, end = permute.find_body(SOURCE, 0x1234)
    s = permute.FastSearch(SOURCE[:start], SOURCE[start:end], SOURCE[end:], confirm, 40, lambda *_: None,
                           permute.Anneal(), permute.MUTATIONS + permute.TYPED_MUTATIONS, context())
    proposals, propose = [], s.propose
    s.propose = lambda *a: proposals.append(propose(*a)[0]) or (proposals[-1], len(proposals))
    permute._work(s, Fast(), random.Random(3), 60, None)
    assert s.tries == 60 and s.uphill > 0
    assert len(set(proposals[:60])) == 60 and SOURCE[start:end] not in proposals  # no variant scored twice
    assert s.best_score == min([40, *full.values()])
    assert s.best == SOURCE[start:end] or full[s.text(s.best)] == s.best_score


def run_main(monkeypatch, tmp_path, best, *flags, callees='matched'):
    """main() with a stubbed search whose best scores best (full build), for 0x1234, which calls 0x2000 (decompiled,
    with status callees) and 0x3000 (library code); the source file's text afterwards."""
    result = permute.Result(2, best, 'variant', 'original', 5, 1.0, fast_baseline=2, fast_best=best)
    monkeypatch.setattr(permute, 'run_fast', lambda *a, **k: result)
    monkeypatch.setattr(permute, 'run', lambda *a, **k: result)
    monkeypatch.setattr(permute, 'ROOT', str(tmp_path))
    monkeypatch.setattr(permute.build, 'marked_sources', lambda *a: [type('M', (), {'retail': 0x1234, 'path': 'f.cpp'})(),
                                                                    type('M', (), {'retail': 0x2000, 'path': 'g.cpp'})()])
    monkeypatch.setattr(permute, 'read_rows', lambda path: {0x1234: {'calls': '00002000 00003000', 'status': 'near'},
                                                            0x2000: {'calls': '', 'status': callees},
                                                            0x3000: {'calls': '', 'status': 'todo'}})
    (tmp_path / 'f.cpp').write_text('original')
    monkeypatch.setattr(permute.sys, 'argv', ['permute.py', '0x1234', *flags])
    with pytest.raises(SystemExit):
        permute.main()
    return (tmp_path / 'f.cpp').read_text()


def test_write_only_match_writes_an_exact_match(monkeypatch, tmp_path):
    assert run_main(monkeypatch, tmp_path, 0, '--fast', '--anneal', '--write-only-match') == 'variant'
    assert run_main(monkeypatch, tmp_path, 0, '--write-only-match') == 'variant'


def test_write_only_match_leaves_a_match_that_calls_unmatched_code(monkeypatch, tmp_path, capsys):
    """Its calls can pass a callee that does not match yet its arguments in other registers than retail's."""
    assert run_main(monkeypatch, tmp_path, 0, '--fast', '--write-only-match', callees='todo') == 'original'
    assert 'not written: it calls 0x2000, which do not match retail yet' in capsys.readouterr().out
    assert run_main(monkeypatch, tmp_path, 1, '--fast', '--write', callees='todo') == 'variant'  # --write as before


def test_write_only_match_leaves_a_merely_better_variant(monkeypatch, tmp_path, capsys):
    assert run_main(monkeypatch, tmp_path, 1, '--fast', '--write-only-match') == 'original'
    assert 'not written' in capsys.readouterr().out


def test_write_keeps_its_meaning(monkeypatch, tmp_path):
    assert run_main(monkeypatch, tmp_path, 1, '--fast', '--write') == 'variant'


@pytest.mark.parametrize('flags, message', [(['--write', '--write-only-match'], 'not allowed with'),
                                            (['--anneal'], '--anneal needs --fast')])
def test_option_conflicts(monkeypatch, capsys, flags, message):
    monkeypatch.setattr(permute.sys, 'argv', ['permute.py', '0x1234', *flags])
    with pytest.raises(SystemExit):
        permute.main()
    assert message in capsys.readouterr().err


