"""A small MSVC (x86) struct layout model, enough to give fields their byte
offsets for field_<offset> placeholders. It fails (returns None) rather than
guess when a member type or array bound is unknown."""
import re

import phase_b_scan as P

PRIMITIVES = {
    'char': 1, 'bool': 1, 'byte': 1, 'BYTE': 1, 'UCHAR': 1, 'CHAR': 1, 'BOOLEAN': 1, '__int8': 1, 'int8': 1,
    'uint8': 1, 'short': 2, 'word': 2, 'WORD': 2, 'USHORT': 2, 'SHORT': 2, 'wchar_t': 2, 'wchar': 2, 'WCHAR': 2,
    '__int16': 2, 'int16': 2, 'uint16': 2, 'int': 4, 'long': 4, 'dword': 4, 'DWORD': 4, 'ULONG': 4, 'LONG': 4,
    'UINT': 4, 'INT': 4, 'BOOL': 4, 'HRESULT': 4, 'HANDLE': 4, 'float': 4, 'real': 4, 'FLOAT': 4, '__int32': 4,
    'int32': 4, 'uint32': 4, 'size_t': 4, 'double': 8, '__int64': 8, 'qword': 8, 'LONGLONG': 8, 'ULONGLONG': 8,
    'int64': 8, 'uint64': 8, 'XUID': 8, 'LARGE_INTEGER': 8, 'FILETIME': 8,
}
QUALIFIERS = {'const', 'volatile', 'unsigned', 'signed', 'struct', 'union', 'class', 'enum', 'mutable', 'static'}
RECORD_START = re.compile(r'\b(struct|union|class)\s+(?:__declspec\([^)]*\)\s*)?([A-Za-z_]\w*)?\s*(?::\s*(?:public\s+|private\s+|protected\s+)?([A-Za-z_][\w:<>, ]*?))?\s*\{')
PACK = re.compile(r'#\s*pragma\s+pack\s*\(\s*(push\s*,\s*(\d+)|pop|(\d+))?\s*\)')


def balanced(code, start):
    depth = 0
    for i in range(start, len(code)):
        if code[i] == '{':
            depth += 1
        elif code[i] == '}':
            depth -= 1
            if depth == 0:
                return i
    return None


class Model:
    def __init__(self, texts):
        self.records = {}  # (path, name) -> (kind, body, base, pack)
        self.by_name = {}
        self.typedefs = {}
        self.enums = set()
        self.constants = {}
        self.cache = {}
        self.stripped = {}
        for path, text in texts.items():
            code, _ = P.strip_code(text)
            self.stripped[path] = code
            for m in re.finditer(r'#\s*define\s+([A-Za-z_]\w*)\s+\(?\s*(0[xX][0-9a-fA-F]+|\d+)[uUlL]*\s*\)?\s*$', code, re.M):
                self.constants.setdefault(m.group(1), int(m.group(2), 0))
            for m in re.finditer(r'\benum\s*([A-Za-z_]\w*)?\s*\{([^}]*)\}\s*([A-Za-z_]\w*)?', code):
                for n in (m.group(1), m.group(3)):
                    if n:
                        self.enums.add(n)
                value = -1
                for item in m.group(2).split(','):
                    item = item.strip()
                    if not item:
                        continue
                    name, _, expr = item.partition('=')
                    name = name.strip()
                    if expr.strip():
                        v = self.eval(expr)
                        value = v if v is not None else value + 1
                    else:
                        value += 1
                    self.constants.setdefault(name, value)
            for m in re.finditer(r'\btypedef\s+([^;{}()]+?)\s+(\**)\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*;', code):
                self.typedefs.setdefault(m.group(3), (m.group(1).strip(), m.group(2), m.group(4), path))
            for m in re.finditer(r'\btypedef\s+[^;{}]*\(\s*[^)]*\*\s*([A-Za-z_]\w*)\s*\)\s*\(', code):
                self.typedefs.setdefault(m.group(1), ('void', '*', '', path))
            packs = [(pm.start(), pm) for pm in PACK.finditer(code)]
            for m in RECORD_START.finditer(code):
                end = balanced(code, m.end() - 1)
                if end is None:
                    continue
                body = code[m.end():end]
                name = m.group(2)
                after = re.match(r'\s*([A-Za-z_]\w*)?', code[end + 1:])
                head = code[max(0, m.start() - 40):m.start()]
                if not name and re.search(r'\btypedef\s*$', head) and after.group(1):
                    name = after.group(1)
                if not name:
                    continue
                pack = self.pack_at(packs, m.start())
                rec = (m.group(1), body, (m.group(3) or '').strip(), pack, path, m.start())
                self.records.setdefault((path, name), rec)
                if path.startswith('include/') or name not in self.by_name:
                    self.by_name[name] = rec

    @staticmethod
    def pack_at(packs, pos):
        stack, cur = [], 8
        for p, m in packs:
            if p > pos:
                break
            if m.group(2):
                stack.append(cur)
                cur = int(m.group(2))
            elif m.group(1) == 'pop':
                cur = stack.pop() if stack else 8
            elif m.group(3):
                cur = int(m.group(3))
        return cur

    def eval(self, expr):
        expr = expr.strip()
        if not expr:
            return None

        def sub(m):
            t = m.group()
            if t in self.constants:
                return str(self.constants[t])
            if t == 'sizeof':
                return t
            raise KeyError(t)
        expr = re.sub(r'sizeof\s*\(\s*([A-Za-z_]\w*)\s*\)', lambda m: str((self.type_layout(m.group(1), '') or (None, None))[0]), expr)
        try:
            expr = re.sub(r'(?<![0-9xX])[A-Za-z_]\w*', sub, expr)
            expr = re.sub(r'(0[xX][0-9a-fA-F]+|\d+)[uUlL]+', r'\1', expr)
            if not re.fullmatch(r'[\d\sxXa-fA-F+\-*/()<>|&]+', expr):
                return None
            return int(eval(expr.replace('/', '//')))
        except Exception:
            return None

    def type_layout(self, typ, path):
        """(size, align) of a type name or None."""
        words = [w for w in re.sub(r'<.*>', '', typ).replace('::', ' ').split() if w not in QUALIFIERS]
        if not words:
            return None
        if words in (['long', 'long'],):
            return 8, 8
        name = words[-1]
        if name in ('short', 'int') and len(words) > 1 and words[-2] == 'long':
            name = 'long'
        if name in PRIMITIVES:
            s = PRIMITIVES[name]
            return s, min(s, 8)
        if name in self.enums:
            return 4, 4
        rec = self.records.get((path, name)) or self.by_name.get(name)
        if rec:
            lay = self.layout(rec)
            return (lay['size'], lay['align']) if lay else None
        if name in self.typedefs:
            base, stars, dims, tpath = self.typedefs[name]
            if base == name:
                return None
            if stars:
                s, a = 4, 4
            else:
                r = self.type_layout(base, tpath)
                if not r:
                    return None
                s, a = r
            for dim in re.findall(r'\[([^\]]*)\]', dims):
                n = self.eval(dim)
                if n is None:
                    return None
                s *= n
            return s, a
        return None

    def layout(self, rec):
        key = (rec[4], rec[5])
        if key in self.cache:
            return self.cache[key]
        self.cache[key] = None  # recursion guard
        kind, body, base, pack, path, _ = rec
        fields = {}
        offset, align = 0, 1
        virtual = 'virtual' in body
        if base:
            r = self.type_layout(base.split(',')[0], path)
            if not r:
                return None
            offset, align = r
        elif virtual:
            offset, align = 4, 4
        ok = self.members(body, path, kind == 'union', pack, fields, offset, align)
        if ok is None:
            return None
        end, align = ok
        size = (end + align - 1) // align * align if end else (1 if kind != 'union' else 0)
        result = {'size': size, 'align': align, 'fields': fields}
        self.cache[key] = result
        return result

    def members(self, body, path, union, pack, fields, start, align):
        """Lay out body's members from start; return (end, align) or None."""
        offset = start
        end = start
        bit_unit = None  # (unit size, unit offset, bits used)
        pos = 0
        decls = []
        depth = 0
        cur = ''
        i = 0
        while i < len(body):
            ch = body[i]
            if ch == '{':
                close = balanced(body, i)
                if close is None:
                    return None
                cur += '{' + body[i + 1:close] + '}'
                i = close + 1
                continue
            if ch == ';':
                decls.append(cur.strip())
                cur = ''
            else:
                cur += ch
            i += 1
        for decl in decls:
            decl = re.sub(r'^\s*(public|private|protected)\s*:', '', decl).strip()
            decl = re.sub(r'__declspec\([^)]*\)', '', decl).strip()
            if not decl or decl.startswith(('typedef', 'static ', 'friend ', 'using ', 'enum ', 'template')) or decl.startswith('#'):
                if decl.startswith('enum ') and '}' in decl and re.search(r'\}\s*[A-Za-z_]', decl):
                    pass
                else:
                    continue
            nested = re.match(r'^(struct|union)\s*([A-Za-z_]\w*)?\s*\{(.*)\}\s*(.*)$', decl, re.S)
            if nested:
                inner_union = nested.group(1) == 'union'
                names = [n.strip() for n in nested.group(4).split(',') if n.strip()]
                inner_fields = {}
                r = self.members(nested.group(3), path, inner_union, pack, inner_fields, 0, 1)
                if r is None:
                    return None
                iend, ialign = r
                isize = (iend + ialign - 1) // ialign * ialign
                ialign = min(ialign, pack)
                bit_unit = None
                if not union:
                    offset = (offset + ialign - 1) // ialign * ialign
                base_off = 0 if union else offset
                if not names:
                    for n, o in inner_fields.items():
                        fields.setdefault(n, base_off + o)
                for n in names:
                    nm = re.match(r'(\**)\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)', n)
                    if not nm:
                        return None
                    count = 1
                    for dim in re.findall(r'\[([^\]]*)\]', nm.group(3)):
                        v = self.eval(dim)
                        if v is None:
                            return None
                        count *= v
                    fields.setdefault(nm.group(2), base_off)
                    s = 4 if nm.group(1) else isize * count
                    if union:
                        end = max(end, s)
                    else:
                        offset += s
                        end = offset
                if not names:
                    if union:
                        end = max(end, isize)
                    else:
                        offset += isize
                        end = offset
                align = max(align, ialign)
                continue
            if '(' in decl and not re.search(r'\(\s*\*\s*[A-Za-z_]\w*\s*\)\s*\(', decl):
                continue  # a method
            fp = re.search(r'\(\s*\*\s*([A-Za-z_]\w*)\s*\)\s*\(', decl)
            if fp:
                items = [(fp.group(1), 4, 4, '', None)]
            else:
                parts = decl.split(',')
                tm = re.match(r'^(.*?)\s*((?:\*\s*)*[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*(?::\s*[^:]+)?)$', parts[0], re.S)
                if not tm or not tm.group(1).strip():
                    return None
                typ = tm.group(1).strip()
                items = []
                for d in [tm.group(2)] + parts[1:]:
                    dm = re.match(r'^\s*(\**)\s*&?\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)(?::\s*(.+))?\s*$', d)
                    if not dm:
                        return None
                    if dm.group(1):
                        s, a = 4, 4
                    else:
                        r = self.type_layout(typ, path)
                        if r is None:
                            return None
                        s, a = r
                    for dim in re.findall(r'\[([^\]]*)\]', dm.group(3)):
                        v = self.eval(dim)
                        if v is None:
                            return None
                        s *= v
                    bits = self.eval(dm.group(4)) if dm.group(4) else None
                    if dm.group(4) and bits is None:
                        return None
                    items.append((dm.group(2), s, a, typ, bits))
            for name, s, a, typ, bits in items:
                a = min(a, pack)
                align = max(align, a)
                if bits is not None:
                    if union:
                        fields.setdefault(name, 0)
                        end = max(end, s)
                        continue
                    if bit_unit and bit_unit[0] == s and bit_unit[2] + bits <= s * 8:
                        fields.setdefault(name, bit_unit[1])
                        bit_unit = (s, bit_unit[1], bit_unit[2] + bits)
                        continue
                    offset = (offset + a - 1) // a * a
                    fields.setdefault(name, offset)
                    bit_unit = (s, offset, bits)
                    offset += s
                    end = offset
                    continue
                bit_unit = None
                if union:
                    fields.setdefault(name, 0)
                    end = max(end, s)
                else:
                    offset = (offset + a - 1) // a * a
                    fields.setdefault(name, offset)
                    offset += s
                    end = offset
        return end, align

    def field_offsets(self, field):
        """[(path, record name, offset or None)] for every record declaring field."""
        out = []
        for (path, name), rec in self.records.items():
            if not re.search(r'\b' + re.escape(field) + r'\b', rec[1]):
                continue
            lay = self.layout(rec)
            if lay is None:
                out.append((path, name, None))
            elif field in lay['fields']:
                out.append((path, name, lay['fields'][field]))
        return out
