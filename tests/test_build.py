import os
import subprocess

import pytest

import build
from build import file_flags, stub_sources, scan, source_flags

SOURCE = '''#include "unknown_11c920.h"
#include "crc.h"

// @retail 0x163ba0
void function_163ba0(
	unsigned long *crc_reference,
	void const *buffer,
	long buffer_size)
{
}

// @retail 0x163c00
PRIVATE void function_163c00(unsigned long *crc_table)
{
}

// @retail 0x259d0
real function_259d0(unsigned long *seed, char const *file, long line, real lower_bound, real upper_bound)
{
	return lower_bound;
}
'''


def test_scan_reads_markers_and_signatures():
    marked = scan(SOURCE, 'src/crc.cpp')
    assert [(m.retail, m.name, m.returns, m.params) for m in marked] == [
        (0x163ba0, 'function_163ba0', 'void', ['unsigned long *', 'void const *', 'long']),
        (0x163c00, 'function_163c00', 'void', ['unsigned long *']),
        (0x259d0, 'function_259d0', 'real', ['unsigned long *', 'char const *', 'long', 'real', 'real']),
    ]


def tu(text=SOURCE, classes=()):
    return build.tu_source('/abs/src/crc.cpp', scan(text, 'src/crc.cpp'), 'crc', classes)


def test_tu_includes_the_source_then_turns_inlining_off():
    text = tu()
    assert text.index('#include "/abs/src/crc.cpp"') < text.index('#pragma auto_inline(off)')
    assert text.index('#pragma auto_inline(off)') < text.index('#pragma inline_depth(0)')
    assert text.index('#pragma inline_depth(0)') < text.index('standin_crc_0')


def test_tu_calls_directly_with_volatile_arguments_and_no_prototypes():
    text = tu()
    assert 'void function_163ba0(unsigned long *, void const *, long);' not in text
    assert 'function_163ba0(*(unsigned long * volatile *)(standin_crc_arguments + 0), ' in text
    assert 'static real volatile standin_crc_result_2;' in text
    assert 'standin_crc_result_2 = function_259d0(' in text
    assert '&function_163ba0' not in text  # never through a pointer


MEMBERS = '''
// @retail 0x1000
void __stdcall callback(int x, widget w, widget &r, enum mode m)
{
}

// @retail 0x1010
static inline long *__fastcall find(char c) { return 0; }

// @retail 0x1020
long widget::size(int n) const
{
}

// @retail 0x1030
widget::widget(int a)
{
}

// @retail 0x1040
widget::~widget()
{
}

// @retail 0x1050
void ns::widget::go()
{
}
'''


def test_scan_strips_conventions_and_reads_members():
    callback, find, size, ctor, dtor, go = scan(MEMBERS, 'src/w.cpp')
    assert (callback.returns, callback.kind) == ('void', 'function')
    assert callback.params == ['int', 'widget', 'widget &', 'enum mode']
    assert (find.returns, find.kind) == ('long *', 'function')
    assert (size.name, size.cls, size.kind, size.returns) == ('widget::size', 'widget', 'method', 'long')
    assert (ctor.cls, ctor.kind, ctor.returns) == ('widget', 'constructor', '')
    assert (dtor.cls, dtor.kind, dtor.returns) == ('widget', 'destructor', '')
    assert (go.cls, go.kind) == ('ns::widget', 'method')


def test_tu_member_constructor_and_destructor_standins():
    text = build.tu_source('/abs/src/w.cpp', scan(MEMBERS, 'src/w.cpp'), 'w', {'widget'})
    assert ('standin_w_result_2 = ((widget *)standin_w_arguments)->widget::size('
            '*(int volatile *)(standin_w_arguments + 0));') in text
    assert '::new ((void *)standin_w_arguments) widget(*(int volatile *)(standin_w_arguments + 0));' in text
    assert '((widget *)standin_w_arguments)->widget::~widget();' in text
    assert '((ns::widget *)standin_w_arguments)->ns::widget::go();' in text
    assert 'vtable' not in text
    assert 'static long volatile standin_w_result_2;' in text
    assert '#include <new>' in text
    assert 'standin_w_result_3' not in text and 'standin_w_result_4' not in text


VIRTUALS = '''
class c_base { public: virtual void update(); };
class c_child : public c_base { public: void update(); long x; };
class c_abstract { public: virtual void draw() const = 0; };
struct s_plain { long x; };
'''


def test_polymorphic_classes_inherit_virtual_and_skip_abstract():
    assert build.polymorphic_classes([VIRTUALS]) == {'c_base', 'c_child'}


def test_tu_copy_constructs_classes_with_a_vtable():
    marked = scan('// @retail 0x1000\nvoid c_child::update()\n{\n}\n', 'src/c.cpp')
    text = build.tu_source('/abs/src/c.cpp', marked, 'c', {'c_child'}, {'c_base', 'c_child'})
    assert '((c_child *)standin_c_arguments)->c_child::update();' in text
    assert ('void standin_c_vtable_0(void) { ::new ((void *)standin_c_arguments) '
            'c_child(*(c_child *)(void *)(standin_c_arguments + 16)); }') in text
    assert '#include <new>' in text


def test_outside_callees_are_game_functions_called_from_library_code():
    rows = {0x10: {'owner': 'game', 'calls': '00000030'}, 0x20: {'owner': 'third:havok', 'calls': '00000010 00000040'},
            0x30: {'owner': 'game', 'calls': ''}, 0x40: {'owner': 'other:library', 'calls': ''}}
    assert build.outside_callees(rows) == {0x10}


def test_outside_callees_ignore_callers_decompiled_in_src():
    rows = {0x10: {'owner': 'game', 'calls': ''}, 0x20: {'owner': 'xdk:xonlines', 'calls': '00000010',
                                                          'source': 'src/widgets.cpp'}}
    assert build.outside_callees(rows) == set()


def test_tu_stores_addresses_of_functions_called_from_outside():
    text = ('// @retail 0x1000\nlong __stdcall find(long a)\n{\n}\n'
            '// @retail 0x1010\nbool c_list::has(long a, short *b) const\n{\n}\n'
            '// @retail 0x1020\nvoid c_list::clear()\n{\n}\n')
    text = build.tu_source('/abs/src/l.cpp', scan(text, 'src/l.cpp'), 'l', set(), set(), {0x1000, 0x1010})
    assert 'void *volatile standin_l_outside_0 = (void *)&find;' in text
    assert 'bool (c_list::*volatile standin_l_outside_1)(long, short *) const = &c_list::has;' in text
    assert 'standin_l_outside_2' not in text


def test_standard_marker_gives_the_outside_callers_address_escape():
    text = ('// @retail 0x218850 standard\ndword __stdcall request(long owner, chunk *c, dword flags)\n{\n}\n'
            '// @retail 0x1010   standard  \nbool c_list::has(long a) const\n{\n}\n'
            '// @retail 0x1020\nlong plain(long a)\n{\n}\n')
    marked = scan(text, 'src/s.cpp')
    assert [(m.retail, m.name, m.standard) for m in marked] == [
        (0x218850, 'request', True), (0x1010, 'c_list::has', True), (0x1020, 'plain', False)]
    assert marked[0].params == ['long', 'chunk *', 'dword']
    tu = build.tu_source('/abs/src/s.cpp', marked, 's', {'chunk'}, set(), set())
    assert 'void *volatile standin_s_outside_0 = (void *)&request;' in tu
    assert 'bool (c_list::*volatile standin_s_outside_1)(long) const = &c_list::has;' in tu
    assert 'standin_s_outside_2' not in tu
    # exactly the escape a function called from library code gets
    plain = scan(text.replace(' standard', ''), 'src/s.cpp')
    assert not any(m.standard for m in plain)
    assert tu == build.tu_source('/abs/src/s.cpp', plain, 's', {'chunk'}, set(), {0x218850, 0x1010})
    assert tu.count('standin_s_outside_0') == 1


@pytest.mark.parametrize('text', [
    '// @retail 0x1000 standard\nc_list::~c_list()\n{\n}\n',
    '// @stub 0x1000 standard\nvoid __stdcall hk(long a)\n{\n}\n',
])
def test_standard_marker_is_only_for_decompiled_functions_and_methods(text):
    with pytest.raises(SystemExit, match='"standard" marks'):
        scan(text, 'src/x.cpp')


STANDARD_CONSTRUCTOR = ('#include <string.h>\n#include "lists.h"\n  # include "more.h"\n'
                        '// @retail 0x2000 standard\nc_list::c_list(word flags, s_item const &item, bool alternate) :\n'
                        '\tc_base(flags)\n{\n}\n'
                        '// @retail 0x2010\nc_other::c_other(long a)\n{\n}\n'
                        '// @retail 0x2020 standard\nvoid __stdcall plain(long a)\n{\n}\n')


def test_standard_marker_on_a_constructor_gets_a_caller_built_without_ltcg():
    marked = scan(STANDARD_CONSTRUCTOR, 'src/l.cpp')
    assert [(m.retail, m.kind, m.standard) for m in marked] == [
        (0x2000, 'constructor', True), (0x2010, 'constructor', False), (0x2020, 'function', True)]
    assert build.standard_constructors(marked) == [marked[0]]
    text = build.standard_source(STANDARD_CONSTRUCTOR, marked, 'l', {'s_item'})
    lines = text.splitlines()
    # the source's includes, not its code: the constructor's body stays in the LTCG object
    assert lines[1:5] == ['#include <string.h>', '#include "lists.h"', '# include "more.h"', '#include <new>']
    assert 'c_base(flags)' not in text and 'c_other' not in text and 'plain' not in text
    assert ('void standard_l_0(void) { ::new ((void *)standin_l_standard_arguments) c_list('
            '*(word volatile *)(standin_l_standard_arguments + 0), '
            '**(s_item const * volatile *)(standin_l_standard_arguments + 16), '
            '*(bool volatile *)(standin_l_standard_arguments + 32)); }') in lines
    assert build.standard_object('l') == 'standard_l.obj'


def test_standard_marker_on_a_constructor_leaves_its_ltcg_object_alone():
    marked = scan(STANDARD_CONSTRUCTOR, 'src/l.cpp')
    plain = scan(STANDARD_CONSTRUCTOR.replace('0x2000 standard', '0x2000'), 'src/l.cpp')
    assert build.standard_constructors(plain) == []
    assert (build.tu_source('/abs/src/l.cpp', marked, 'l', {'s_item'}, set(), set())
            == build.tu_source('/abs/src/l.cpp', plain, 'l', {'s_item'}, set(), set()))


def test_standard_must_be_the_whole_trailing_word():
    assert scan('// @retail 0x1000 standardish\nvoid f()\n{\n}\n', 'src/f.cpp') == []
    assert scan('// @retail 0x1000 standard extra\nvoid f()\n{\n}\n', 'src/f.cpp') == []


def test_tu_reads_class_values_through_a_non_volatile_pointer():
    text = build.tu_source('/abs/src/w.cpp', scan(MEMBERS, 'src/w.cpp'), 'w', {'widget'})
    assert '*(widget *)(void *)(standin_w_arguments + 16)' in text      # by value
    assert '**(widget * volatile *)(standin_w_arguments + 32)' in text  # reference
    assert '*(enum mode volatile *)(standin_w_arguments + 48)' in text  # enums are scalars
    assert '*(int volatile *)(standin_w_arguments + 0)' in text


def test_class_names_come_from_declarations():
    found = build.class_names(['struct a { int x; };', 'typedef struct { int y; } b;', 'class c;', 'int d;',
                               'enum e { f };'])
    assert found == {'a', 'b', 'c'}


def test_build_reports_missing_sdk(tmp_path):
    with pytest.raises(SystemExit) as e:
        build.build(root=str(tmp_path), xdk=str(tmp_path / 'nowhere'))
    message = str(e.value)
    assert str(tmp_path / 'nowhere') in message and 'XDK_DIR' in message


def test_every_source_gets_ltcg_and_register_calls():
    assert source_flags(['/O1']) == ['/GL', '/Gr', '/O1']
    assert source_flags([])[:2] == ['/GL', '/Gr']


CONFIG = {'default': ['/O2', '/Gr'], 'files': {'old.cpp': ['/O1', '/Gr']}}


def test_flags_line_in_the_source_wins():
    text = '/* a file */\n// @flags /O1 /Gr\n#include "x.h"\n'
    assert file_flags(text, 'old.cpp', CONFIG) == ['/O1', '/Gr']
    assert file_flags('// @flags /O2 /Ob1 /Gr\n', 'new.cpp', CONFIG) == ['/O2', '/Ob1', '/Gr']


def test_flags_fall_back_to_files_json_then_default():
    assert file_flags('int x;\n', 'old.cpp', CONFIG) == ['/O1', '/Gr']
    assert file_flags('int x;\n', 'new.cpp', CONFIG) == ['/O2', '/Gr']
    assert file_flags('int x;\n', 'new.cpp', {'default': ['/O2']}) == ['/O2']


def test_flags_line_after_the_first_thirty_lines_is_ignored():
    text = '\n' * 30 + '// @flags /O1\n'
    assert file_flags(text, 'new.cpp', CONFIG) == ['/O2', '/Gr']


STUBS = '''#include "unknown_11c920.h"

// @stub 0x2f0a40
real hkVector4_length(real const *v)
{
	return 0;
}
'''


def test_scan_reads_stub_markers():
    (stub,) = scan(STUBS, 'src/stubs/havok.cpp')
    assert (stub.retail, stub.name, stub.returns, stub.params, stub.stub) == (
        0x2f0a40, 'hkVector4_length', 'real', ['real const *'], True)
    assert not scan(SOURCE, 'src/crc.cpp')[0].stub


def test_deleting_marker_names_the_compilers_deleting_destructor():
    text = ('// @retail 0x1a1c20 deleting\n// @retail 0x1a1c40\nc_page_heap::~c_page_heap()\n{\n}\n')
    deleting, destructor = scan(text, 'src/p.cpp')
    assert (deleting.retail, deleting.name, deleting.kind) == (0x1a1c20, "c_page_heap::`deleting destructor'", 'deleting')
    assert (destructor.retail, destructor.name, destructor.kind) == (0x1a1c40, 'c_page_heap::~c_page_heap', 'destructor')
    tu = build.tu_source('/abs/src/p.cpp', [deleting], 'p', {'c_page_heap'}, {'c_page_heap'})
    assert 'void standin_p_0(void) { delete (c_page_heap *)standin_p_arguments; }' in tu
    with pytest.raises(SystemExit):
        scan('// @retail 0x1000 deleting\nvoid f()\n{\n}\n', 'src/f.cpp')


def test_variadic_functions_get_their_fixed_parameters_only():
    (printf_like,) = scan('// @retail 0x13fb50\nlong __cdecl print(char *buffer, char const *format, ...)\n{\n}\n',
                          'src/p.cpp')
    assert printf_like.params == ['char *', 'char const *']


def test_deleting_marker_with_a_class_needs_no_function():
    (deleting,) = scan('// @retail 0x1a1c20 deleting c_page_heap\n', 'src/p.cpp')
    assert (deleting.name, deleting.cls, deleting.kind, deleting.params) == (
        "c_page_heap::`deleting destructor'", 'c_page_heap', 'deleting', [])


def test_destructor_marker_with_a_class_marks_its_implicit_destructor():
    (destructor,) = scan('// @retail 0x1473d0 destructor c_screen\n', 'src/s.cpp')
    assert (destructor.name, destructor.cls, destructor.kind, destructor.params) == (
        'c_screen::~c_screen', 'c_screen', 'destructor', [])
    tu = build.tu_source('/abs/src/s.cpp', [destructor], 's', {'c_screen'}, set())
    assert 'void standin_s_0(void) { ((c_screen *)standin_s_arguments)->c_screen::~c_screen(); }' in tu


def test_vcall_marker_names_the_compilers_thunk_and_gets_no_standin():
    text = ('// @retail 0x234c5f vcall 0x5c\n'
            '// @retail 0x1010\nvoid f(long a)\n{\n}\n')
    thunk, f = scan(text, 'src/l.cpp')
    assert (thunk.retail, thunk.name, thunk.kind, thunk.params, thunk.stub) == (
        0x234c5f, "`vcall'{0x5c}", 'vcall', [], False)
    tu = build.tu_source('/abs/src/l.cpp', [thunk, f], 'l')
    # the source's own code emits the thunk; the other stand-ins keep their numbers
    assert 'standin_l_0' not in tu and 'void standin_l_1(void) { f(' in tu
    assert build.standin_names([thunk, f])[('src/l.cpp', 0x1010)] == 'standin_l_1'


def test_adjustor_marker_names_the_compilers_this_adjustor_thunk_and_gets_no_standin():
    text = ('// @retail 0x278b30 adjustor c_object 8' + chr(10) +
            '// @retail 0x1c58d0 adjustor c_1c3410::function_1c3410 0xc' + chr(10) +
            '// @retail 0x1010' + chr(10) + 'void f(long a)' + chr(10) + '{' + chr(10) + '}' + chr(10))
    deleting, method, f = scan(text, 'src/a.cpp')
    assert (deleting.retail, deleting.name, deleting.cls, deleting.kind, deleting.params, deleting.stub) == (
        0x278b30, "c_object::`deleting destructor'`adjustor{0x8}'", 'c_object', 'adjustor', [], False)
    assert (method.name, method.cls, method.kind) == (
        "c_1c3410::function_1c3410`adjustor{0xc}'", 'c_1c3410', 'adjustor')
    tu = build.tu_source('/abs/src/a.cpp', [deleting, method, f], 'a', {'c_object'}, {'c_object'})
    # the class's vtable stand-in emits the thunk; the other stand-ins keep their numbers
    assert 'standin_a_0' not in tu and 'standin_a_1(' not in tu and 'void standin_a_2(void) { f(' in tu
    assert 'standin_a_vtable_0' in tu
    assert build.standin_names([deleting, method, f])[('src/a.cpp', 0x1010)] == 'standin_a_2'


def test_adjustor_marker_is_not_for_stubs():
    with pytest.raises(SystemExit, match='"adjustor" marks'):
        scan('// @stub 0x278b30 adjustor c_object 8' + chr(10), 'src/stubs/s.cpp')


def test_vcall_marker_is_not_for_stubs():
    with pytest.raises(SystemExit, match='"vcall" marks'):
        scan('// @stub 0x234c5f vcall 0x5c\n', 'src/stubs/s.cpp')


def test_no_vtable_standin_for_a_class_with_a_marked_constructor():
    text = ('// @retail 0x1000\nc_child::c_child()\n{\n}\n'
            '// @retail 0x1010\nvoid c_child::update()\n{\n}\n')
    marked = scan(text, 'src/c.cpp')
    assert build.vtable_classes(marked, {'c_child'}) == []


def test_vtable_standins_come_before_inlining_is_turned_off():
    marked = scan('// @retail 0x1000\nvoid c_child::update()\n{\n}\n', 'src/c.cpp')
    text = build.tu_source('/abs/src/c.cpp', marked, 'c', {'c_child'}, {'c_child'})
    assert text.index('standin_c_vtable_0') < text.index('#pragma auto_inline(off)') < text.index('standin_c_0(')


def test_stub_sources_reads_only_the_stubs_folder(tmp_path):
    (tmp_path / 'src' / 'stubs').mkdir(parents=True)
    (tmp_path / 'src' / 'stubs' / 'havok.cpp').write_text(STUBS)
    (tmp_path / 'src' / 'crc.cpp').write_text(SOURCE)
    assert [m.name for m in stub_sources(str(tmp_path))] == ['hkVector4_length']
    assert [m.name for m in build.marked_sources(str(tmp_path))][0] == 'function_163ba0'
    assert all(not m.stub for m in build.marked_sources(str(tmp_path)))


def test_markers_are_read_again_when_a_source_changes(tmp_path):
    (tmp_path / 'src').mkdir()
    source = tmp_path / 'src' / 'crc.cpp'
    source.write_text(SOURCE)
    assert [m.retail for m in build.marked_sources(str(tmp_path))] == [0x163ba0, 0x163c00, 0x259d0]
    source.write_text(SOURCE.replace('0x163c00', '0x163c10'))
    stat = source.stat()
    os.utime(source, ns=(stat.st_atime_ns, stat.st_mtime_ns + 10 ** 9))  # a later write, whatever the clock's resolution
    assert [m.retail for m in build.marked_sources(str(tmp_path))] == [0x163ba0, 0x163c10, 0x259d0]


def test_source_objects_name_objects_as_the_build_does(tmp_path):
    (tmp_path / 'src' / 'stubs').mkdir(parents=True)
    (tmp_path / 'src' / 'crc.cpp').write_text(SOURCE)
    (tmp_path / 'src' / 'stubs' / 'havok.cpp').write_text(STUBS)
    assert build.source_objects(str(tmp_path)) == {'crc.obj': 'src/crc.cpp', 'standard_crc.obj': 'src/crc.cpp',
                                                   'stubs_havok.obj': 'src/stubs/havok.cpp'}


def test_entry_source_leaves_fltused_to_libcmt():
    text = build.entry_source(['void standin_a_0(void);'], ['	standin_a_0();'])
    assert 'fltused' not in text
    assert 'standin_a_0();' in text and 'int entry(void)' in text


def test_ballast_source_is_unreferenced_code_only():
    text = build.ballast_source(3)
    assert [line.split('(')[0] for line in text.splitlines() if line.startswith('void ')] == [
        'void ltcg_ballast_0', 'void ltcg_ballast_1', 'void ltcg_ballast_2']
    # nothing in it is kept in the image: its only global is static, and no
    # function calls another or takes an address
    assert 'static volatile long ltcg_ballast_v[64];' in text
    assert 'ltcg_ballast_0(' not in text.replace('void ltcg_ballast_0(', '')
    assert '&ltcg' not in text and 'pragma' not in text


@pytest.mark.skipif(build.sys.platform in ('win32', 'cygwin', 'msys'), reason='paths pass through unchanged on Windows')
def test_tool_arg_gives_wine_absolute_paths_as_drive_paths(tmp_path):
    path = str(tmp_path / 'crc.cpp')
    win = 'Z:' + path.replace('/', '\\')
    assert build.tool_arg(path) == win
    assert build.tool_arg('/Fo' + path) == '/Fo' + win
    assert build.tool_arg('/OUT:' + path) == '/OUT:' + win
    for switch in ('/c', '/O2', '/GL', '/I', '/MAPINFO:FIXUPS', 'libcmt.lib'):
        assert build.tool_arg(switch) == switch


@pytest.mark.parametrize('platform', ['win32', 'cygwin', 'msys'])
def test_tool_arg_leaves_arguments_alone_on_windows(monkeypatch, tmp_path, platform):
    monkeypatch.setattr(build.sys, 'platform', platform)
    path = str(tmp_path / 'crc.cpp')
    for arg in (path, '/Fo' + path, '/OUT:' + path, '/c', '/O2'):
        assert build.tool_arg(arg) == arg


def test_standin_names_number_each_sources_markers_in_order():
    marked = [build.Marked('src/ui.cpp', 0x10, 'a', 'void', []), build.Marked('src/hud.cpp', 0x20, 'b', 'void', []),
              build.Marked('src/ui.cpp', 0x30, 'c', 'void', [])]
    assert build.standin_names(marked) == {('src/ui.cpp', 0x10): 'standin_ui_0', ('src/hud.cpp', 0x20): 'standin_hud_0',
                                           ('src/ui.cpp', 0x30): 'standin_ui_1'}


def fake_lib_tool(monkeypatch, members):
    """Stands in for the SDK's lib tool: /LIST prints the members, /EXTRACT
    writes the member's name to the /OUT file. Returns each run's first argument."""
    runs = []

    def run_tool(tool, args, cwd, xdk):
        runs.append(args[0])
        if args[0] == '/LIST':
            return ''.join(f'{m}\n' for m in members)
        with open(args[1][len('/OUT:'):], 'w') as f:
            f.write(args[0][len('/EXTRACT:'):])
        return ''
    monkeypatch.setattr(build, 'run_tool', run_tool)
    return runs


def test_d3d_objects_are_extracted_in_order_once_per_library(tmp_path, monkeypatch):
    assert len(build.D3D_OBJECTS) == len(set(build.D3D_OBJECTS)) == 25
    xdk = tmp_path / 'xdk'
    (xdk / 'lib').mkdir(parents=True)
    library = xdk / 'lib' / 'd3d8ltcg.lib'
    library.write_bytes(b'library')
    runs = fake_lib_tool(monkeypatch, [f'obj\\i386\\{stem}.obj' for stem in ['texture', *reversed(build.D3D_OBJECTS)]])
    paths = build.d3d_objects(str(tmp_path), str(xdk))
    assert paths == [str(tmp_path / 'build' / 'd3d8ltcg' / f'{stem}.obj') for stem in build.D3D_OBJECTS]
    assert open(paths[0]).read() == 'obj\\i386\\d3ddev.obj' and len(runs) == 26
    assert build.d3d_objects(str(tmp_path), str(xdk)) == paths and len(runs) == 26  # kept
    stat = library.stat()
    os.utime(library, ns=(stat.st_atime_ns, stat.st_mtime_ns + 10 ** 9))  # a new library
    build.d3d_objects(str(tmp_path), str(xdk))
    assert len(runs) == 52


def test_d3d_objects_need_the_library_and_every_member(tmp_path, monkeypatch):
    xdk = tmp_path / 'xdk'
    with pytest.raises(SystemExit, match='d3d8ltcg.lib not found'):
        build.d3d_objects(str(tmp_path), str(xdk))
    (xdk / 'lib').mkdir(parents=True)
    (xdk / 'lib' / 'd3d8ltcg.lib').write_bytes(b'library')
    fake_lib_tool(monkeypatch, [f'obj\\i386\\{stem}.obj' for stem in build.D3D_OBJECTS if stem != 'pusher'])
    with pytest.raises(SystemExit, match='no member named pusher.obj'):
        build.d3d_objects(str(tmp_path), str(xdk))


def test_run_tool_retries_once_after_a_timeout(tmp_path, monkeypatch):
    """A wedged Wine tool is killed after TOOL_TIMEOUT and tried once more,
    rather than stalling the whole build."""
    xdk = tmp_path / 'xdk'
    (xdk / 'bin' / 'vc71').mkdir(parents=True)
    calls = []

    class Result:
        returncode, stdout, stderr = 0, 'ok', ''

    def fake_run(command, **kwargs):
        calls.append(kwargs)
        if len(calls) == 1:
            raise subprocess.TimeoutExpired(command, kwargs['timeout'])
        return Result()

    monkeypatch.setattr(build.subprocess, 'run', fake_run)
    assert build.run_tool('CL.Exe', [], str(tmp_path), str(xdk)) == 'ok'
    assert len(calls) == 2 and calls[0]['timeout'] == build.TOOL_TIMEOUT


def test_build_refuses_a_source_named_like_a_d3d_member(tmp_path):
    (tmp_path / 'xdk' / 'bin' / 'vc71').mkdir(parents=True)
    (tmp_path / 'xdk' / 'bin' / 'vc71' / 'CL.Exe').write_bytes(b'')
    (tmp_path / 'src').mkdir()
    (tmp_path / 'src' / 'pusher.cpp').write_text('')
    with pytest.raises(SystemExit, match='src/pusher.cpp'):
        build.build(root=str(tmp_path), xdk=str(tmp_path / 'xdk'))
