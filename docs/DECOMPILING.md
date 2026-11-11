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
- **Pick a function:** take one from `python tools/ready.py` (or, for Havok,
  `python tools/ready.py --owner third:havok`; see [Havok](#havok)). Every function
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
     (punpckhdq/halo). It was CC0 until commit `8bde956` (2026-10-17) and is
     AGPL-3.0 since; its fork bnunu/halo-1 is still CC0. The AGPL revisions
     may be read to understand a function, but copy nothing from them: no
     code, names or structure;
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
     The 25 members of it that retail holds are linked as objects, in
     retail's order, before the libraries (`D3D_OBJECTS` in `tools/build.py`):
     LTCG gives the push-buffer functions their calling conventions by the
     order in which it reaches them. The first build extracts them from your
     SDK into `build/d3d8ltcg/`.
6. **Run** `python tools/check.py --fast <va>` after each edit (see
   [The fast check](#the-fast-check)), and `python tools/check.py <va>`
   before you keep a change. Repeat until it prints `MATCH`.
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
   - **the file's flags** (`/O1` against `/O2`, `/Ob1`);
   - **bit tests:**
     - Retail's `shr reg, N; test reg8, 1` before a branch comes from a
       one-bit value converted to `bool`: `TEST_FIELD_BIT` on a one-bit
       field (`include/unknown_11c920.h`), or `(bool)((x >> n) & 1)`.
     - A mask (`x & 0x1000`) or a bare `(x >> n) & 1` in a condition
       compiles to `test reg, mask` instead.
     - To find candidates, look for `shr reg, N` followed by `test reg8, 1`
       in a retail function whose source tests the same field with a mask.
7. **Stop after 20 tries,** or when only register choice or operand order
   differs. Leave the function with its marker. The checker records it as
   `near` or `todo` with the first difference, so someone can come back to
   it. `python tools/near.py` lists every near-miss in the csv by source
   file (count and size) and does not need the XBE. If what differs is the
   registers a call's arguments travel in, the callee is usually the problem,
   not your function: see
   [Calling conventions under LTCG](#calling-conventions-under-ltcg).

## The fast check

The plain check rebuilds and links the whole program, which takes one to
three minutes. While you iterate, check on a reduced image instead:

    python tools/check.py --fast <va> [<va> ...]

(or `@file`, one address per line). It builds one image holding each listed
function's source, the sources of its callees in retail all the way down, the
sources of the functions and globals its body uses by name (so the compiler
can inline them as the full build does), and, from your last full build, the
sources that take the address of the function or its callees, and the library
objects. Only those are compiled and linked. It prints the full check's
`MATCH` and `DIFF` lines and listing, then where the time went: on a busy
machine about 8 seconds for one function, 12 to 40 for twenty, and about
1.5 when nothing changed since the last run.

- It writes only `build/report-fast.json`. `config/functions.csv` and
  `build/report.json` only change with the plain check, which stays the
  judge: run `python tools/check.py <va>` before you keep a change, and the
  full `python tools/check.py` before you finish.
- It takes what it needs from the last full build in `build/` (any plain
  check makes one). Without one it says so, and leaves out the library objects
  and the sources that take addresses. After a branch switch or a header edit,
  run a plain check of the function first: the reduced image otherwise
  describes the tree of the last full build, and either scores the function
  differently (and refuses to search) or describes its callees wrongly.
- A function whose callees go a few hundred sources deep takes about as long
  as a full link (25 to 30 seconds). Several functions from one file or area
  share most of their image, so check them together.

A `CAVEAT` line follows a function when the reduced image may build it
differently from the full one:

- *builds differently here than in the last full build*: the function, or a
  function it calls, comes out different from the last full build although
  its source has not changed since. Code outside the reduced image decides
  it (such as a callee that takes a `float` in `xmm4` only in the full image);
  trust only the plain check for this function. In a 212-function comparison
  every disagreement with the full check carried this caveat.
- *calls or global references reach what the reduced image leaves out*: a
  function the full build may inline, or a global defined in a source the
  image leaves out. Usually harmless; run the plain check when a result
  surprises you.

The first caveat compares with the last full build, so it says nothing about
a function whose source you changed since (only its unchanged callees are
compared), and nothing at all after a header change until the next plain
check.

**The score.** Every `DIFF` line ends with a difference score: the
function's differing instructions (the listing's `!!` lines) plus the
difference in length, in bytes. A match scores 0. The score is stored in the
report entry, and `python tools/check.py --scores` lists every unmatched
function of the last full check with its score, as CSV. It only reports, and
no status depends on it, but it shows progress that "near" and "todo" hide:
a function that goes from 40 differing instructions to 3 is still `todo`.

- Keep whichever version of a near miss scores lowest, even if it reads less
  naturally, and note what still differs in a comment that starts
  `// score N (was M):`.
- Commit each improvement, not only finished matches, so a lower score is
  never lost.

## Calling conventions under LTCG

On this build, many near misses come down to how arguments travel rather
than to the body. This is what contributors and the maintainer's lanes have
learned so far:

- **A function that doesn't match yet decides its callers' code.** LTCG
  chooses each internal function's convention from its own body: which
  arguments go in registers and which stay on the stack. If our version of a
  callee differs from retail's, its convention can differ too, and then every
  caller that passes it arguments differs, however right the caller's own
  source is. Before you tune a near function, check whether the calls it gets
  wrong go to functions that don't match yet. Fixing that one callee often
  matches several callers without touching them. Many recent matches came
  this way.
- **Read retail's evidence.** `ret N` at the end of a function says how many
  bytes of arguments it takes from the stack. What each caller loads into
  `eax`, `ecx`, `edx`, `ebx`, `esi` or `edi` just before the call says which
  arguments travel in registers. Compare both with ours.
- **When an argument stays on the stack.** As far as we have seen, LTCG
  leaves an argument on the stack when the callee takes its address, uses it
  only once as a memory operand, or keeps it in `ebp` or a callee-saved
  register and also uses it elsewhere. To move an argument between a register
  and the stack, change how the body uses it, not how it is declared.
- **Idioms that have worked:**
  - reordering a function's parameters, with every call site changed to match;
  - removing a `volatile` that keeps parameters on the stack where retail has
    them in registers;
  - reading a parameter at its later use as `*(T const volatile *)&parameter`,
    or a field as `*(long volatile *)&pointer->field`.

  Try a plain-source fix first. A `volatile` access is acceptable only on a
  parameter, a local or data reached through a pointer, never on a global
  (see [What not to do](#what-not-to-do)), and it needs a comment saying why.
  These idioms change how a value travels, never which value; see the
  permuter's [false matches](#near-functions-the-permuter).
- **Aligned locals block register arguments.** A local declared
  `__declspec(align(N))`, an `__m128` local or an `_mm_*` store makes the
  compiler align the frame (`ebp` keeps the old `esp`, then `and esp, -N`).
  Under LTCG such a function stops taking arguments in `ebx`, even where the
  realignment is optimised away. If retail's prologue aligns the same way,
  keep it. If it doesn't, write the stores as plain `float` stores, or through
  a helper that takes a pointer, and the register arguments come back.
- **Link order.** Sources link in name order, so a file named after its
  subsystem links before every `unknown_*` file. The code LTCG generates for a
  few functions depends on the order in which it reaches them. When you rename
  or move a file, run the full check; if a match is lost, choose a name that
  keeps the file's place relative to its neighbours.

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
  - **When it is allowed:** only after the body matches retail byte for byte with the standard convention, and the ordinary idioms have failed. Write the function `__stdcall` (or as the method it is) as Bungie would have. Never use the marker to steer a function whose retail convention is a register one, and never on stubs or destructors; the build rejects those.
  - **Before the body matches (frame alignment):** the marker may go on an unmatched function when retail's own bytes show the case: its prologue self-aligns its frame (`and esp, -8`), which our build does for such a function only when its address escapes, and it takes no arguments, or retail's callers push every argument as the standard convention does. Its evidence comment says the body doesn't match yet and names the callers the marker lets match. 0x217c40 is the model case.
  - **Constructors:** C++ cannot take a constructor's address, so for a constructor marked `standard` the build instead compiles a caller without LTCG (as it compiles the stubs): `build/gen/<stem>_standard.cpp`, made of the source's `#include` lines and one function that constructs the class into a volatile buffer, linked as `standard_<stem>.obj` after the stubs. A function that code outside LTCG calls keeps the standard (`thiscall`) convention. The class must be declared in a header the source includes; the constructor's body stays in the source's own LTCG object, and nothing else changes for any other function. The evidence rules below apply as for any function. 0x2c4ea0 (`src/unknown_2c4e9c.cpp`) is the model case: in retail it lies on a call cycle through code this project has not written yet, and LTCG keeps the default convention for a cycle member it reaches through a back edge.
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

## Havok

Havok, the physics library linked into the game, is decompiled too: its rows
in `config/functions.csv` have the `owner` `third:havok` (1,436 functions,
mostly `0x2d0000`–`0x31ffff`). `python tools/check.py` counts them on their
own line, and `python tools/ready.py --owner third:havok` lists the ones ready
to write. It is easier than the game code in one way: Havok was a prebuilt
library, compiled without LTCG, so its conventions don't depend on the rest of
the program.

- **Flags:** every Havok file starts `// @flags /O2 /GL-`. Don't add
  `/arch:SSE`: Havok's own `__m128` code doesn't need it, and its scalar code
  is x87.
- **Conventions:** standard ones. Member functions are `thiscall`. Write free
  functions `__cdecl` explicitly, because the build adds `/Gr` to every file.
  A small struct returned by value comes back through a hidden pointer
  (`ret 4`).
- **Code shape:**
  - Vector code uses SSE intrinsics (`__m128`, `_mm_*`); functions with
    `__m128` locals realign their frame (`and esp, -16`), as retail does.
  - Scalars use plain `float`.
  - Every function is 16-byte aligned, with `int3` fill.
- **Files:** name a file `src/unknown_<lowest address>.cpp` and group
  neighbouring functions in it. Don't use a name that sorts before
  `src/unknown_058dd0.cpp`: code linked ahead of that file costs `0x73360` its
  match.
- **Calls:** Havok calls back into the game through ordinary `cdecl` calls and
  through a game-provided object's vtable. A Havok function's game callees
  never change its convention.
- **Provenance:** name Havok code only by what it does, as with the game's.
  Never use Havok SDK headers, documentation, samples or source, even versions
  that were once free to download (see
  [CONTRIBUTING.md](../CONTRIBUTING.md#contribution-provenance)).

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
temporaries, a repeated literal or constant held in one local, `if`/`else`
branches, `for`/`while`, `+=` forms, `++` forms, `a = b = 0` chains, a named
`bool` for an `&&` condition, `||` and `&&` conditions against `else if` and
nested `if`, `?:` against `if`/`else`, arguments of a call trading places or
rotating, `volatile` reads, stores and pointers), rebuilds, and keeps the
variant with the fewest differing instructions; 0 is a match. Rearranged arguments are tried most often: they
were the last change in about a quarter of the near functions that went on to
match, as far as git shows. It works on a copy of `src/` in a scratch
directory, so your tree is untouched; the search is deterministic for a given
`--seed`. It prints the best score, the tries per second and a unified diff.
With `--write` a strictly better variant is written back to the source file.
Read the diff before you keep it: a changed local type, a swapped argument or
a `volatile` access can change what the function means, and a variant that
matches by accident still has to read like Bungie's code.

For near misses, the fast search is usually the better choice:

    python tools/permute.py <va> --fast --anneal --jobs 4 --time-limit 600 --write-only-match

- `--fast` scores each variant on a reduced image (the function, its callees
  and what it needs), which takes seconds instead of a full build. Every
  improvement is confirmed by a full build. If the reduced image scores the
  original differently from the full image, the search stops at once and says
  to run it without `--fast`, because it would optimise the wrong score.
- `--jobs N` scores variants in N workers, each on its own scratch root; in a
  local run four workers covered about six times as many variants per second
  as one, since a worker's link overlaps the others' compiles. A prepared
  scratch compiles no sources (its copied objects and generated units are
  already current), so the first build is a link.
- `--anneal` lets the search accept some worse variants and restart, so it
  can get past a local minimum.
- `--write-only-match` writes a variant back only when a full build finds an
  exact match and every decompiled function it calls matches too. Any other
  match is reported for you to check by hand.
- `HALO2_TOOL_TIMEOUT` (seconds, default 600) bounds one compiler or linker
  call. Wine occasionally wedges a tool at 0% CPU; the call's whole process
  group is then killed and tried once more, so one wedge cannot stall a run.

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
are known. The check also follows each address field that leaves a function
(the `targets` of its entry in `build/report.json`, and a summary line): a
field that points at a marked function, a global named for its retail
address (`g_<address>`) or a string or floating-point constant is verified
against retail or found to differ; anything else, and a call between
functions neither side names, is unverified. The linker map gives no sizes,
so a field far into a global is only judged when its size is known or another
field verifies an offset as large. A field that differs counts against a
match: the function is `near` when one or two fields differ, otherwise
`todo`, and the `DIFF` names each field, our target and retail's. An
unverified field does not count.

## Naming and readability

Readability work changes names, types and file layout but never a byte of the
build: run a full check before and after, and every function that matched
must still match.

- **Where names come from:** only the code itself, the retail executable's own
  strings and constants, and SDK constants such as `XC_LANGUAGE_*`. See
  [PROVENANCE.md](../PROVENANCE.md) for the sources a name must never come
  from. Keep `function_<address>` for anything whose purpose the code doesn't
  show.
- **Functions:** `subsystem_object_verb` in lower snake case, with one prefix
  per subsystem: `saved_game_storage_update` acts on the whole store,
  `saved_game_type_get_storage_name` on one file type.
- **Types:** `s_name` for structs, `c_name` for classes, `e_name` for enums with
  `_name_value` members, `k_name` for constants and counts. Grep `src/` and
  `include/` for a name before adding it to a shared header.
- **Globals:** `g_name`. Rename a global only when the code you are working on
  is its only user; otherwise keep `g_<address>` and describe it in a comment
  beside its declaration.
- **Data layouts:** put a subsystem's structs in a header named after it.
  Check every struct's size, and any offset you are unsure of, with
  `typedef char assert_x[(cond) ? 1 : -1];` (the compiler has no
  `static_assert`). Name a member `unknownNN` (its hex offset) only when
  nothing reads it.
- **Files:** group a subsystem's functions in one file named after it, and
  move a function only into a file with the same `@flags`. The checker finds
  each function by its `@retail` marker, so a function moves with its marker.
- **Renames:** change the definition, every declaration and every call site,
  and nothing else on those lines. Leave `@retail` markers exactly as written.
- A one-line comment above each `@retail` marker saying what the function does
  helps the next reader most.
- **Near misses:** when you leave a function unmatched, keep a
  `// score N (was M):` note above its marker. It says what still differs and
  what you tried, as short forms with their scores, for example "a pointer
  local 87; the declarations swapped 91; a 600 s permuter run, none below 84".
  Add to the list rather than replacing it. Read the note before starting on
  a function, and don't repeat a listed form without a new reason.

## What not to do

- Do not add `__declspec(noinline)` or other attributes Bungie's code would
  not have had. Stand-ins keep functions out of line.
- Do not add `#pragma` lines inside a function body, for example an
  `inline_depth` pair around one call; they can change how the rest of the
  file compiles. A file-scope `#pragma inline_depth` around a whole function
  is fine.
- Do not steer code generation through globals. That means: no taking a
  global's address, or reading it through a `volatile` cast, as a trick (it
  stops the compiler hoisting loads of that global in every function that
  uses it); no reaching neighbouring globals by offsetting one global's
  address; and no `volatile` function pointers as anchors. Write a global as
  the data retail holds.
- Do not keep an argument swap at a call to a function that doesn't match yet
  (see [false matches](#near-functions-the-permuter)).
- Do not commit anything from `orig/`, `sdk/` or `build/`.
