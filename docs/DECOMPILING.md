# Decompiling a function

This is the procedure for one function, written for a person or a subagent.

## Before you start

- **Provenance:** follow [PROVENANCE.md](../PROVENANCE.md). Nothing from
  leaked symbols, linker maps or internal builds (names, strings, comments or
  code) may be copied into the repository; name things by what they do.
- **The toolchain:** the development toolchain's `xbox` folder at `sdk/xbox`,
  or set `XDK_DIR`. The project does not distribute it; see the README's
  [Requirements](../README.md#requirements).
- **The retail XBE:** `orig/default.xbe`. Extract it with
  `python tools/xiso_extract.py "<your Halo 2 image>" orig default.xbe`.
- **Python:** `pip install -r requirements-dev.txt`.
- **Pick a function:** take one from `python tools/ready.py`. Every function
  it calls is already matched, is library code, or is in the same recursion
  group, so the function's own code is the only unknown. Pass
  `--claims` a saved copy of the Active claims table to skip ranges that
  issue already lists.
- **In a git worktree,** `orig/` and `sdk/` are not there: set `RETAIL_XBE`
  to the retail XBE and `XDK_DIR` to the SDK's `xbox` folder.

## Steps

1. **Read the retail code** with `python tools/disasm.py <va>`.
   - `config/functions.csv` gives the function's size and what it calls.
     Its `name` column holds only library functions' names, from their
     signatures in the SDK libraries; the `object` column is empty.
   - Note which arguments arrive in registers. LTCG gives internal functions
     custom conventions; write normal C++, and the compiler will choose the
     same registers.
2. **Find related source.** It may be:
   - the function of the same name in the Halo CE decompilation
     (punpckhdq/halo, CC0);
   - a neighbouring matched function;
   - the structures in `include/`.
3. **Write the function** in the `src/` file it belongs to. Group functions by
   what you can see in the executable: neighbouring addresses, shared data,
   and type or callback tables. Name a file after what its code does, or use
   `src/unknown_<va>.cpp`.
   - Put `// @retail 0x<va>` on the line above it.
   - Write `static` functions as `PRIVATE`.
4. **Set the file's flags** with a `// @flags <flags>` line among the first 30
   lines of the source (`config/files.json` is only a fallback for files
   without one; the default is `/O2 /Gr`). `/GL` and `/Gr` are always added.
   Choose by the `style` column:
   - `speed`: `// @flags /O2 /Gr`
   - `size`: `// @flags /O1 /Gr`
   - `unknown`: try `/O2` first; if it does not match and the function is
     packed or ebp-framed, try `/O1`.
   If a callee gets inlined that retail calls, add `/Ob1` (`// @flags /O2 /Ob1 /Gr`).
5. **Callees.** Every call out of the function must reach the right function:
   - *Matched or in-progress game functions:* declare them (a header in
     `include/`) and call them; their `@retail` markers tie them to retail.
   - *Third-party code* (Havok, Bink, ...) and game functions not decompiled
     yet: write a stub in `src/stubs/<owner>.cpp` with a plausible signature,
     with `// @stub 0x<va>` (retail's address) above its definition. The body
     does not matter; the signature and calling convention do. Stubs are
     compiled without `/GL`, so they keep the standard convention retail uses
     for code outside the project. A stub is not compared with retail and gets
     no stand-in.
   - *Library functions* (CRT, XAPI, D3D, ...): nothing to write. Declare
     them and call them; the SDK libraries are linked. `tools/libsig.py`
     identifies library functions by their byte signatures.
   - *Direct3D:* a function that calls the public D3D API must call the public
     D3D API. `d3d8ltcg.lib` is linked, and LTCG inlines parts of it into the
     caller just as it did in retail. Never write a stub for a D3D internal.
6. **Run** `python tools/check.py <va>`. Repeat until it prints `MATCH`.
   When it does not match, it prints the first difference, and
   `build/report.json` stores it too.
   Things that usually decide a match:
   - **types:** `short` against `long`, signed against unsigned;
   - **the order of terms** in floating-point expressions;
   - **whether a value goes through a local variable,** and whether a
     parameter's address is taken (that keeps it on the stack);
   - **an argument or local that retail keeps on the stack:** take its address
     (`T const *x_reference = &x;`) so our build keeps it there too, and say
     why in a comment;
   - **the file's flags** (`/O1` against `/O2`, `/Ob1`).
7. **Stop after 20 tries,** or when only register choice or operand order
   differs. Leave the function with its marker. The checker records it as
   `near` or `todo` with the first difference, so someone can come back to
   it. `python tools/near.py` lists every near-miss in the csv by source
   file (count and size) and does not need the XBE.

## Stand-ins: marking functions

Each function marked `// @retail 0x<va>` gets a stand-in caller that keeps it
in the image and out of line. The build compiles the source as
`build/gen/<stem>_tu.cpp`: an `#include` of the source, then
`#pragma auto_inline(off)`, `#pragma inline_depth(0)` and the stand-ins. They
share the source's translation unit, so:

- types defined inside the `.cpp` are fine as parameters;
- explicit `__stdcall` / `__cdecl` / `__fastcall` declarations are fine (the
  stand-in uses the declaration already in scope; taking the function's
  address is fine too);
- member functions, constructors and destructors (`T::m`, `T::T`, `T::~T`) can
  be marked. The stand-in calls them on, or constructs into, a volatile buffer.
  Methods are called by qualified name (`p->T::m()`), so a virtual method is
  called directly.
- **Virtual methods:** declare them `virtual`, as Bungie did, with the class
  deriving from its interface.
  - For each class with a vtable, a stand-in also copy-constructs the class, which emits its vtable. The virtual functions' addresses then escape as they do in retail, and LTCG keeps their standard `thiscall` convention.
  - Every virtual function in such a class must have a body. Slots that are not decompiled yet may have empty inline bodies in the interface.
  - Never take addresses through globals to imitate a vtable.
- **Deleting destructors:** a vtable's destructor slot holds the compiler's deleting destructor. It calls the destructor, then `operator delete` when `flags & 1`. Don't write it by hand. Mark it instead:
  - with `// @retail 0x<va> deleting` above the class's destructor;
  - or, when the destructor is implicit, with a standalone `// @retail 0x<va> deleting <class>` line.

  The checker compares it with the compiler's `??_G`/`??_E`.
- **Implicit destructors:** when retail keeps a class's implicit (non-deleting) destructor out of line, mark it with a standalone `// @retail 0x<va> destructor <class>` line. The marker only names the compiler-generated `<class>::~<class>`; the checker compares its bytes like any other function.
- **Pointers to virtual member functions:** taking `&T::method` of a virtual method makes the compiler emit a thunk that calls through its vtable slot (`mov eax, [ecx] ; jmp [eax + 0x5c]`). No source function corresponds to it. When retail holds such a thunk, mark it with a standalone `// @retail 0x<va> vcall <vtable offset>` line in the source that takes the pointer. The checker compares it with the compiler's own thunk for that offset (`??_9`); it gets no stand-in, so if nothing in the source takes such a pointer the checker reports it missing. 0x234c5f (`src/unknown_232d43.cpp`) is the model case.
- **Functions called from library code:** when Havok, the C runtime or an XDK library calls a game function directly, retail keeps that function's standard convention, because those callers were built without LTCG. The build reads those callers from `config/functions.csv` and stores the function's address in a generated global, so the source needs nothing special.
- **Standard convention with no visible reason (last resort):** a few retail functions keep the standard stack convention (`__stdcall`, `ret N`) although nothing in retail's image holds their address and every caller is LTCG code. Under LTCG such a function gets a register convention unless its address escapes. For these, and only these, mark the function with `// @retail 0x<va> standard`. The build then gives it the same generated address global as a function called from library code. That global is data, so the checker never compares it.
  - **When it is allowed:** only after the body matches retail byte for byte with the standard convention, and the ordinary idioms have failed. Write the function `__stdcall` (or as the method it is) as Bungie would have. Never use the marker to steer a function whose retail convention is a register one, and never on stubs, constructors or destructors; the build rejects those.
  - **Evidence:** put a comment above the function that records three things:
    1. Retail's body matches only with the standard convention: the build with the marker matches, and without it LTCG moves the arguments into registers.
    2. Retail holds no reference to the function's address: no data or code in retail holds it, and its callers (list them) are all LTCG code that pushes every argument.
    3. Which idioms were tried and what each gave, for example taking a parameter's address (which keeps the arguments on the stack but changes the body) or `/GL-`.

  0x218850 (the sound cache request, `src/unknown_218850.cpp`) is the model case.
- **Function pointers in data:** a table or struct of function pointers belongs in the source only when retail's data holds those addresses. Write it as that data, named by its retail address. Never use a global, or a dummy parameter, only to steer a calling convention; the `standard` marker above is the only way to do that, and the build, not the source, holds the address.

A class or struct passed by value is read through a plain pointer into that
buffer, since a volatile object cannot be copied.

## The Bungie-code boundary

Bungie's code ends where the Xbox SDK's D3DX zlib code begins (`0x2cb510`,
zlib's `compress2`, which `d3dx8.lib` defines; the last game function is
`0x2cb4e2`). Everything above it in `.text` is libraries and third-party
code (zlib, XAPI, Havok, CRT, Rockall, voice, WMA, Bink, DSOUND, compiler stubs).
`config/owners.json` records this: `tools/inventory.py` applies it last, so
`game` rows in `.text` at or above `game_end` become `other:library`, and each
entry of `ranges` (`{"start", "end", "owner", "note"}`) sets its rows' owner
outright. The checker's game totals count only `game` rows.

## The whole-program inlining threshold

The compiler's link-time inliner makes some decisions differently once the
whole program passes a certain size. While developing the build we observed
that adding about 30 KB of LTCG code anywhere in `src/`, in any link order,
made small helpers stop inlining across the image. Pull request #28 lost 18
matches that way, such as `function_6b910`'s callers and Bink's allocator.

`tools/build.py` keeps the result stable as code is added:
- It links a generated ballast object, `build/gen/ltcg_ballast.cpp`: 9,000
  small functions that nothing calls, so the linker drops them. They keep the
  program comfortably past that size.
- The ballast is compiled with the compiler's `/d2inlT` inlining-threshold
  option (`INLINE_THRESHOLD`), set to the value at which the matched code was
  found. An option on any one object applies to the whole link.

That value is a sharp optimum for the matched code: a full check one step
lower loses 4 matches and gains 1, and one step higher loses 8. Don't change
`INLINE_THRESHOLD` or the ballast without a full `python tools/check.py` run.
A function that still inlines differently from retail needs a source fix (an
`inline` helper, `/Ob1`), not a different threshold. The ballast adds roughly
15 seconds to each link.

## Near functions: the permuter

When a function differs from retail by a few instructions and the obvious
rewrites have not helped, let `tools/permute.py` search for you:

    python tools/permute.py <va> [--tries N] [--seed S] [--time-limit SECONDS] [--write]

It mechanically rewrites the body of the function marked `// @retail 0x<va>`
(operand swaps, statement and declaration order, integer types of locals,
temporaries, `if`/`else` branches, `for`/`while`, `+=` forms, `++` forms,
`a = b = 0` chains, a named `bool` for an `&&` condition, `||` and `&&`
conditions against `else if` and nested `if`, `?:` against `if`/`else`,
arguments of a call trading places or rotating, `volatile` reads, stores and
pointers), rebuilds, and keeps the variant with the fewest differing
instructions; 0 is a match. Rearranged arguments are tried most often: they
were the last change in about a quarter of the near functions that went on to
match, as far as git shows. It works on a copy of `src/` in a scratch
directory, so your tree is untouched; the search is deterministic for a given
`--seed`. It prints the best score, the tries per second and a unified diff.
With `--write` a strictly better variant is written back to the source file.
Read the diff before you keep it: a changed local type, a swapped argument or
a `volatile` access can change what the function means, and a variant that
matches by accident still has to read like Bungie's code.

For near misses, the fast search is usually the better choice:

    python tools/permute.py <va> --fast --anneal --jobs 1 --time-limit 600 --write-only-match

- `--fast` scores each variant on a reduced image (the function, its callees
  and what it needs), which takes seconds instead of a full build. Every
  improvement is confirmed by a full build.
- `--anneal` lets the search accept some worse variants and restart, so it
  can get past a local minimum.
- `--write-only-match` writes a variant back only when a full build finds an
  exact match and every decompiled function it calls matches too. Any other
  match is reported for you to check by hand.

**False matches.** Under LTCG a function that doesn't match yet takes its
argument registers from its own body. So swapping the arguments at a call to
such a function can reproduce retail's bytes while passing the wrong values,
and the checker counts it as a gain. Never keep an argument swap at a call to
a function that doesn't match, unless you change that function's parameters at
its definition and every call site the same way.

## What a match means

A match compares the bytes, and for each address field only its kind: whether
it is a call, a global or a jump table entry (jump table entries must map to
the same cases). Call targets are verified: a call to a function with an
`@retail` or `@stub` marker must reach that marker's retail address; a call to
a function retail reaches at an address some marker claims must reach that
marker's function, unless ours is the compiler's own copy of the helper
that marker recreates (`??_H` for `vector_constructor_iterator`); otherwise
both sides' names (the map symbol, the `name` column) must agree when both
are known. Globals are still checked by eye.

## What not to do

- Do not add `__declspec(noinline)` or other attributes Bungie's code would
  not have had. Stand-ins keep functions out of line.
- Do not commit anything from `orig/`, `sdk/` or `build/`.
