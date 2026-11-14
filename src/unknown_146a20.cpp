// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_146A20.CPP: the memory Havok allocates from. One block of virtual
   memory (g_479888) holds the allocators, built in place: a fixed buffer, a
   Havok pool, and two fixed buffers made when the pool runs short, one in
   the ai's scratch buffers (g_47989c) and one in physical memory
   (g_4798a0). The game's own c_havok_abstract_allocator (vtable 0x45378c, 0x147110 and on)
   allocates from them. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_146a20.h"
#include "physical_memory.h"
#include "unknown_223b60.h"
#include "unknown_080e10.h"
#include <xtl.h>
#include <new.h>

/* the texture cache's physical memory (unknown_12c0d0.cpp) */
extern s_physical_object *g_4e6464;

long __stdcall function_12d2f0(long a, long b, long c, long d);
void function_12c600(void);
void function_12d520(long a);
double timing_ticks_to_seconds(__int64 ticks);

/* the havok system (unknown_1c25a0.cpp and later) */
void function_1c27a0(void);
void function_1c2690(void);
void __stdcall function_1c4590(long unknown);
bool function_1c4040(long attempt, bool active, bool any_object, bool even_if_unknown, long excluded_component_index);

/* the block the allocators live in (unknown_1c25a0.cpp) */
extern void *g_479888;

/* the ai's scratch buffers (ai.cpp) */
extern byte *g_510c44;
extern bool g_510c48;

/* the fixed buffer in physical memory (unknown_147090.cpp) */
extern c_havok_fixed_memory *g_4798a0;

#define HAVOK_SCRATCH_MEMORY_SIZE 0x452e8

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void function_146b80(void);
void function_146da0(void);
void function_146de0(void);
long function_146fd0(void);

bool g_479880;
long g_479884;
long g_47988c;
long g_479890;
c_havok_abstract_allocator *g_479894;
c_havok_abstract_allocator *g_479898;
long g_4798b4;
bool g_47f05a;
bool g_47f05b;

// MiniMax H (11-14): no new form, no lower score (34): the permuter tried 80 variants of the body and none came below the kept form (30 differing instructions in both).
// score 34 (was 34): frameless entry agrees; intermediate vtable store and successful-arm backing reload remain.
// MiniMax R (11-11): no new form, no lower score (34); the abstract allocator's base vtable store
// (`mov [eax], 0x453760`) is still dropped, because it comes from the shared constructors in
// include/unknown_146a20.h, which this lane may not edit (BLOCKED-BY), and the pool placement still reloads
// g_479888 at the join where retail reloads it on the constructed arm; the no-local form puts that reload in
// retail's place but loses the 1 in EDX (36), and seven source forms all score 34 or more.
// MiniMax E (11-11): no new form, no lower score (34); the base vtable store retail keeps comes from the inlined
// constructors in include/unknown_146a20.h, which this lane may not edit, so the gap stands (BLOCKED-BY that header).
// MiniMax C (11-09): no new form, no lower score (34); the abstract allocator's base vtable store is still elided (BLOCKED-BY include/unknown_146a20.h).
// MiniMax D (11-06): no new form, no lower score (34); retail's base vtable store (0x453760) and its
// derived vtable (0x4576a4) come from the shared constructors in include/unknown_146a20.h, which this lane may
// not edit, so the gap stays (BLOCKED-BY include/unknown_146a20.h).
// Earlier at 34: MiniMax D (11-05): no new form, no lower score (34); the abstract allocator's base vtable store comes from the shared constructor in include/unknown_146a20.h, which I may not edit, so the gap stays (BLOCKED-BY include/unknown_146a20.h).
// frameless prefix agrees; intermediate vtable store and successful-arm backing reload remain.
// Earlier: MiniMax D (11-05): no new form, no lower score (34); the abstract allocator's base vtable store comes from the shared constructor in include/unknown_146a20.h, which I may not edit, so the gap stays (BLOCKED-BY include/unknown_146a20.h).
// MiniMax b (11-04): no new form, no lower score; the abstract allocator's base vtable store is still dropped (it comes from the header's inline constructor) and the pool placement still reloads g_479888 at the join.
// Earlier at 34: unchanged that round: retail still stores the abstract allocator's vtable at
// the object before the two following stores (our build drops that store, which comes from the
// header's inline constructor and so is out of reach from this file) and still reloads g_479888 on
// the constructed arm where ours reloads it at the join.
// Earlier: the base vtable store is still dropped and the pool placement
// still reloads g_479888 at the join where retail reloads it on the constructed arm.
// Earlier headers, merged: unchanged that round (34); a one-file LTCG probe keeps the
// store only for a load through an unknown pointer between the two stores, which
// belongs to the abstract allocator's constructor in the header and so is out of reach
// from this file (34); the vtable slot at [eax] is never filled in, so the two stores
// that follow start one field too late; retail stores the vtable there first (34).
// Retail keeps the abstract allocator's vtable store only here (0x146b80, 0x146e30 and 0x22c3e0
// drop it), as if something read memory between the two vtable stores; that also keeps 1 in edx.
// Without `block` (g_479888 read at both placements) the reload sits on the constructed arm as in
// retail but the 1 becomes immediates: 38. Tried: the pool placed at `block + 0x15040` 40; the
// buffer argument through g_479888 38; `(c_havok_fixed_memory *)g_479888 + 1` 38; a typed
// `fixed` pointer local with `fixed + 1` 34; `block` read after the three global stores 34; the
// fixed buffer in a local before storing g_479894 38; `block` reassigned from g_479888 for the pool 34.
// A constructor that takes only the size and puts the buffer at `this + 1` (in the fixed buffer or
// its base, with or without `block`), or a subclass holding the 0x15000 bytes, gives retail's reload
// and the 1 in edx, leaving only the dead vtable store: 35 each (140 bytes). A one-file LTCG harness
// keeps the store for none of member reads, constant-folded branches, a cross-file constructor or the
// globals as one struct.
// Force-inline abstract constructor 34; scoped block 34; abstract-typed block 34; refcount-typed block 34;
// intermediate/derived/abstract result reused for the pool 44/44/44; intermediate result published 38;
// inline target 34.
// Round 7 (lane 13): retail's matched functions that keep a base vtable store (0x2784c0, 0x2dec70 and the
// other Havok constructors) all read memory through another pointer between the two stores; here nothing
// does. On the size-only constructor: intermediate class novtable 35, virtual destructor in the fixed class
// 35, base stores through an inline initialize member 35, abstract loop split 3 + NONE 34 (0x146b80 and
// 0x22c3e0 lose their matches), a buffer/size description struct read by the base constructor 36 (146
// bytes; the reads are forwarded before the store is dropped). Harness only: memset in the abstract
// constructor, size by reference bound to a local, buffer by reference: store dropped each time.
// inline target 34. Round 5 (g_479888 read at both placements, which puts the reload on the
// constructed arm, while only the `block` form keeps the 1 in edx): chained flag assignment 38;
// pool address through a byte pointer named after the first construction 38; if/else on the
// placement result 38; bool flag first 38; NONE flag last 38. Four explicit base-constructor
// stores in place of the loop still drop the vtable store (30, 0x146b80 lost). Round 20 (size-only
// constructor, g_479888 at both placements): a class placement operator new 35; the size by const
// reference 35; `m_size = size; m_size &= ...` 35; the base's size by const reference 35; the
// construction copied from a temporary 34 (elided). A one-file LTCG harness also keeps the store for
// none of: the abstract constructor out of line in another file (still inlined, store dropped), a
// virtual call in the derived constructor (devirtualised after the final vtable store), out-params
// by reference in the base, a pointer-walk loop.
// stores in place of the loop still drop the vtable store (30, 0x146b80 lost).
// Round 103 (fast, with 0x146b80/0x146e30/0x22c3e0): the fixed base storing the count before the
// size 33 (0x146e30 121); the abstract loop reading a const initial table 34; the fixed base's
// fields as a member initializer list 34; `__declspec(novtable)` fixed base 34; size-only
// constructor whose body reads the buffer into the count before clearing it 34 (140 bytes).
// Harness only: a field read in the fixed base (not forwarded, store still dropped) 23 but
// 0x146b80 15; a description struct by const reference 36 (146 bytes, 1 as immediates).
// Round 37: the fixed, fixed-base and abstract constructors each defined out of line in another file (LTCG
// inlines them) 34/34/34, no other function changes; the block typed as its layout (fixed allocator, buffer,
// pool) 34; the count flag stored first 38. With the vtable store forced (diagnostic only) 12 at 147 bytes:
// the rest is the pool placement's reload of g_479888 at the join, where retail reloads on the constructed arm.
// Round 3 (lane 18, header constructors, with 0x146b80/0x146e30/0x22c3e0): the count header's 1 as a member
// initializer 34; the abstract loop counting down 33 (0x146b80 2, 0x22c3e0 lost); its NONE test as `i < 3`
// 34; the fixed base storing the count first 32 (0x146e30 120 -> 121); the size before the buffer 33
// (0x146b80 2). The base vtable store is dropped in every form.
// Round 235: publish fixed identity before construction, saved/published placement
// and published buffer 35/35/46; pool identity with saved/published placement 52/52;
// both identities 53/53/64. All original publications and executable code restored.
// Round 22 (lane 13, fast with 0x146b80/0x22c3e0 matched): the three global stores after the
// first construction 41 (143 bytes); g_479880 stored first 39; the block typed `byte *` 34;
// the first placement as an explicit if/else on the block 34; the block typed as the fixed
// allocator with the buffer at `block + 1` 34 (close to the listed `fixed + 1` form); g_479890
// stored first 39.
// Round 97 (lane 8, fast with 0x146b80/0x146e30/0x22c3e0/0x1c2690): retail's 0x4798b4 is the reference
// count (+4) of the game's allocator object at 0x4798b0 (0x1473b0 returns it); the increment through
// 0x1473b0 34 (same code), with g_479888 at both placements 38. The fixed class without the
// intermediate base 31 (its vtable store moves up beside the abstract stores; 0x146b80 3). g_479888 at
// both placements re-measured 38: retail's reload and layout, the 1 as three immediates, no base store.
// An explicit `block ? new (block) ... : NULL` after the global stores 34 (same code).
// Round 32 (lane 11, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0; retail's 0x453760 is the abstract class's
// own vtable, eight pure slots, and 0x146b80 builds the same class at 0x105410 with the store dropped): the
// fixed class flattened onto the abstract one 31 (0x146b80 3); `::new` 34; the refcount header as a member
// in place of a base 34; the abstract constructor storing NONE before its zero loop 31 (0x146b80 4); the
// masked size in a named local 34. The store is dropped in all (141 bytes).
// Round 7 (m3 lane 18, fast with 0x146b80/0x22c3e0 matched): the pool address read from g_479888
// before the fixed construction 43 (140 bytes); the block typed as one layout struct (fixed
// allocator, buffer, pool) read from g_479888 at each placement 38 (144 bytes); the fixed placement
// from g_479888 with the buffer from `block` 36 (146 bytes: reload on the constructed arm, but the 1s
// become immediates, no base store); the layout struct through `block` with the pool through
// g_479888 34 (same code). Diagnostic only (header): a global read in the fixed base constructor,
// between the two vtable stores, still drops the base store.
// Round 113 (lane 6, fast with 0x146b80/0x22c3e0 matched): a typed fixed pointer from g_479888 with the
// buffer at `fixed + 1` 34 (same code), read after the three global stores 34; the first placement through
// g_479888 with the buffer through `block` 36 (146 bytes by chance: block copied to EDX, the 1s immediates,
// no vtable store); neither local (diagnostic, listed) 38 (reload on the constructed arm as retail, 1s
// immediates); a buffer local read from g_479888 before the global stores 38.
// Round 53: `block` for the placement and the pool with the buffer from g_479888 43; `block` read after
// the three stores with the placement from g_479888 36 (146 bytes), with the buffer from g_479888 38;
// `block` (at the top or after the stores) used only for the pool 34/34 (149 bytes). Compile-only, all
// store orders and `block`/g_479888 mixes for placement, buffer and pool: the 1 in a register comes only
// with both from `block`, the arm reload only when the arm reads g_479888; never both, no base store.
// Round 100 (lane 8, one-file harness exact with unknown_22c0a0.cpp): diagnostic only, a compiler barrier at the end of
// the abstract constructor keeps the base vtable store (12 at 147 bytes; 0x146b80/0x22c3e0 lose), and with the size-only
// constructor it is an exact 0, so the kept store is the last difference. Forms: the size-only buffer as
// `(byte *)this + sizeof(*this)` 35; the size-only constructor setting the buffer in its body after a NULL base argument
// 35; an explicit upcast at the publish 35; `byte *block` 34 (same code); the fixed allocator in a typed local first 38.
// Round 22 (m3 lane 2, fast with 0x146b80/0x22c3e0 matched): `_ReadWriteBarrier()` after the three global stores 41,
// the same with g_479888 at both placements 41 (144 bytes); `g_4798b4++` before the constructions 48 (137 bytes); the
// size-only constructor 35, with the barrier after / before the global stores 41/34.
// Round 43 (fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0): a byte block for the buffer and the pool 40; the
// buffer named before the global stores 36; the construction in a typed local published after 38; the size as a
// named const 34 (same code); the pool address named after the first construction 34 (same code).
// Round 125 (lane 6, fast with 0x146b80/0x22c3e0 matched): the fixed allocator in a typed local published
// after the pool's construction 38 (154 bytes); `g_4798b4++` between the two constructions 38 (137 bytes);
// the three global stores after both constructions 40 (144 bytes); `g_479880 = true` after the first
// construction 39, `g_479890 = 1` there 40 (141 bytes each). The base vtable store stays dropped in all.
// Round 122 (lane 5, fast with 0x146b80/0x22c3e0 matched): the size-only constructor re-measured 35
// (140 bytes, only the abstract vtable store missing); on it, the fixed base's buffer/size stores under
// a constant-folded `if (size > 0)` 35 (a folded branch does not keep the store); the buffer as `block +
// sizeof(c_havok_fixed_memory)` 34 (same code); the buffer named after the three global stores 36 (146
// bytes, the 1s become immediates); `g_479894 = NULL; if (block) g_479894 = new (block) ...` 32 (139
// bytes, the NULL store moves above the test, the vtable store still dropped).
// Round 106 (lane 7, fast with every function of the files using the header): the size-only
// constructor (buffer `this + 1`) 35 (140 bytes; 0x146b80/0x22c3e0 kept); the abstract constructor
// clearing its four words with memset and then storing NONE 38 (152 bytes), clearing three words 37
// (149), each with the size-only constructor 46/27 (153/150): the base vtable store is still dropped
// and 0x146b80/0x146e30/0x22c3e0 lose (memset keeps a pointer to the words). Compile-only is not
// exact here (the constants' registers differ).
// Round 105 (lane 8, fast with 0x146b80/0x22c3e0/0x146ac0): retail 0x146b80 and 0x146e30 build the
// same class at g_479888 + an offset and drop the base store; only here (object at g_479888 itself,
// buffer at this + 0x30) is it kept. Header forms: the abstract array value-initialised with NONE set
// in the body 42 (0x146b80 21, 0x22c3e0 219); the fixed base storing buffer, count, size 33 (Round
// 103's count-before-size form re-measured); the abstract loop zeroing all four slots, then NONE 21
// (144 bytes; the plain dead zero store at +0x14 survives the loop, but the vtable store is still
// dropped; 0x146b80 15, 0x22c3e0 207). Diagnostics (not kept): the pool constructed first 52; the
// pool construction removed 75: the base store is dropped either way, so nothing after the
// construction keeps it.
// Lane 12 r66 (fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0; a /GL- compile-only build also drops the store):
// g_479880 stored before g_479890 36; `g_4798b4 += 1` 34 (same code); the buffer at `block +
// sizeof(c_havok_fixed_memory)` 34 (same code); the pool placed through `block` too 40 (135 bytes); the
// construction in a typed local published after the pool construction 38 (154 bytes).
// Round 19 (lane 17, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0/0x146de0/0x146da0/0x147090/0x2d6fc0):
// the size-only constructor re-measured 35 (140 bytes, only the abstract vtable store missing). On it: a
// constructor taking the whole block size (buffer `block_size - sizeof(*this)`) 35; the pool placed at
// `(byte *)g_479894 + 0x15040` 44 (131 bytes); the fixed allocator and its 0x15000-byte buffer as one
// struct whose constructor passes its buffer member 35; `g_4798b4++` between the two constructions 39
// (136 bytes); the three global stores after the fixed construction 42 (142 bytes).
// Round 212: named complete allocator copied into placement, direct/copy initialization:
// whole-function lifetime 89/89; guarded production 88/88;
// lifetime ending before the pool 89/89; outer producer with guarded
// placement 89/89. All restored: a 0x30 frame and an out-of-line copy call.
// Round N281: complete fixed/pool construction result locals, derived/base type
// pairs at production 38/38/38/38; entry declarations with the derived fixed
// result and derived/base pool result 38/38. Original complete body restored.
// Round 52 (lane 1, fast with 0x146b80/0x146e30/0x22c3e0/0x22c330): the size-only constructor (buffer at
// `this + 1`) declared in the header and defined out of line in unknown_22c0a0.cpp, placed at g_479888, 35
// (140 bytes); the size-only constructor on the fixed base, out of line, placed through `block` 35; that with
// the abstract constructor also out of line 35 (an out-of-line definition inlined by LTCG drops the store as
// the header one does); the refcount 1 as a member initializer 34; the abstract loop value as
// `i < 3 ? 0 : NONE` 34 (same code).
// Round 113 (lane 8, fast with 0x146b80/0x22c3e0 matched): `g_4798b4++` between the two constructions 38
// (137 bytes); `g_479880 = true` alone after the first construction 39 (its store sinks to the join); the
// pool through a typed local published after 34 (same code); the pool placed at `(byte *)g_479894 + 0x15040`
// 44 (131 bytes, no reload at all); the pool address computed into `block` first 34 (same code).
// Round 151 (lane 4, fast with 0x146b80/0x22c3e0 matched): the pool placed at `&((byte *)g_479888)[0x15040]` 34
// (same code); the buffer as `&((byte *)block)[0x30]` 34 (same code); the fixed allocator constructed in a
// statement of its own and `block` published after it 42 (133 bytes); the pool constructed in a statement of its
// own and published from its address 43 (132 bytes); both fixed constructors `__forceinline` in the header 34 (same
// code; 0x146e30 unchanged at 120). Diagnostic only: global optimizations off for this function 91 (187 bytes, no
// inlining at all). No source form on this function's side that blocks the dead-store removal is known.
// Round 24 (lane 17, fast with every function of this file and 0x22c3e0): `g_479880 = g_479890 = 1` 34 (same
// code); g_479884's NONE stored after the two 1s 39; the abstract loop with a short index 54 (0x146b80 and 0x22c3e0
// lose); the refcount header named in the abstract constructor's initializer list 34 (same code). Diagnostic only:
// /Ob1 for this file 34 (0x146b30 74), so the inlining mode does not keep the store. Retail's vtables fold (the
// abstract and fixed vtables share the destructor 0x1470f0), so a polymorphic fixed base with no overrides would also
// store 0x453760: the kept store may be that base's rather than the abstract's.
// Round 19 (lane 17, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0/0x146de0/0x146da0/0x147090/0x2d6fc0):
// the size-only constructor re-measured 35 (140 bytes, only the abstract vtable store missing). On it: a
// constructor taking the whole block size (buffer `block_size - sizeof(*this)`) 35; the pool placed at
// `(byte *)g_479894 + 0x15040` 44 (131 bytes); the fixed allocator and its 0x15000-byte buffer as one
// struct whose constructor passes its buffer member 35; `g_4798b4++` between the two constructions 39
// (136 bytes); the three global stores after the fixed construction 42 (142 bytes).
// Round 50 (lane 14, fast with 0x146b80/0x22c3e0): g_479888 read at each use instead of `block` 38 (144 bytes: retail's
// reload inside the non-null arm and its `lea` at the join, but 1 no longer sits in EDX); `block` for the placement
// with the buffer from g_479888 38 (144), the reverse 36 (146 bytes, no reload of its own shape), `block` declared
// after the three global stores 34, with the buffer from g_479888 38; a buffer local at the top or after the stores
// 38/38; the three stores after the fixed construction 39; a chained 1 for g_479880 and g_479890 38; a local holding
// the 1 38. Compile-only, about 45 forms, the abstract store dropped in all but the last two groups: placement through
// force-inline wrappers (one and two levels) and a static create; an explicit constructor call; operator new then
// the constructor; the abstract's stores in an init method; a defaulted NONE parameter; arguments in a by-value or
// by-reference struct, const references, locals and address-taken locals read in the constructor; `this + 1` and a
// member array for the buffer; `__assume` on fields, the vtable and globals; dead or forwarded reads of fields; a
// second polymorphic base. Only a real load of a global between the two vtable stores keeps the store (the load
// stays in the code, so retail's reason is another read that disappears later).
// Round 298: Native member-array fill/fill_n, first-three/full-four ranges,
// each with body/member-list fixed-base initialization: 56 each (eight forms).
// All add a constructor call and worsen other users: 0x146b80 57,
// 0x146e30 169, 0x22c3e0 230/201. Entire original shared header restored;
// the intermediate vtable store and successful-arm reload remain.
// Round 300: Passive refcount header initialized by its abstract owner before/after the
// native reverse four-word pass, long/int/short/signed-char indices:
// 33/33/54/54/35/35/54/54. All worsen other constructor users; original header/body restored. Intermediate vtable store and arm reload remain.
// J239: intermediate fixed-base storage with byte/next-object/sizeof buffer before reset [34, 34, 34];
// after reset [34, 34, 34]; scoped next-object/sizeof with chained state [34, 34].
// Original placements and body restored; intermediate vtable store and successful-arm reload remain.
// Round 124 (m3 lane 8, fast with 0x146b80/0x22c3e0 matched): `void *const block` 34 (same code); `block` read
// after the NONE store 34; the stores ordered g_479890, g_479884, g_479880 38; the pool constructed with `()`
// 34; `g_479880 = true` moved to the end 48 (155 bytes).
// B381: complete four-word constructor element result in long/int/short/signed-char,
// terminal/nonterminal assignment: 34/34/34/34/34/34/34/34. NOT kept: one-use element results
// require an exact match. Original intermediate-vtable store and arm reload gap remain.
// m3 lane 11 r62 (fast with 0x146b80/0x146e30/0x22c3e0; header trials restored): the abstract loop with a
// dword index 34 (same code); its test as `i != 4` 54 (loop kept; 0x146b80 49, 0x146e30 169, 0x22c3e0 231);
// as `while (i < 4) { ...; i++; }` 34 (same code); the fixed constructors taking `byte *` 34 (same code);
// `++g_4798b4` 34 (same code). The base vtable store stays dropped in all.
// Round 62 (m3 lane 10): `byte *block` 34; placement through `(c_havok_fixed_memory *)block` 34;
// `void *const block` 34; the pool address named before the first construction 43 (140 bytes);
// `block ? new ... : NULL` 34. Original body restored.
// Lane 18 round 36 (fast with 0x146b80/0x146e30/0x22c3e0): the abstract loop index `unsigned long` 34 (same
// code); the three global stores first, then a `fixed` result cleared to NULL and constructed under `if (block)`
// 39 (142 bytes); `g_479894 = block ? new (block) ... : NULL;` 34 (same code); the fixed base's size parameter as
// `dword` 34 (same code); the count cleared in the derived fixed constructor's body 33 (0x146e30 120 -> 121; not kept).
// Round 124 (m3 lane 7, fast with the file and 0x22c3e0): a `c_havok_fixed_memory *` block with the buffer as
// `block + 1` 34; the fixed base's fields through a member-initializer list 34; the size as `long const &` in
// both fixed constructors 34; the count cleared first in the fixed base body 32 (0x146e30 120 -> 121, not kept);
// the abstract loop as `i < 3 ? 0 : NONE` 34. Header and body restored.
// m3 lane 11 r62 (fast with 0x146b80/0x146e30/0x22c3e0; header trials restored): the abstract loop with a
// dword index 34 (same code); its test as `i != 4` 54 (loop kept; 0x146b80 49, 0x146e30 169, 0x22c3e0 231);
// as `while (i < 4) { ...; i++; }` 34 (same code); the fixed constructors taking `byte *` 34 (same code);
// `++g_4798b4` 34 (same code). The base vtable store stays dropped in all.
// Lane 12 r86 (fast with 0x146ac0/0x146b80/0x146e30/0x22c3e0): g_479884's NONE stored after both
// constructions 44 (140 bytes); the buffer as `(void *)((dword)block + 0x30)` 34 (same code); a byte
// block with `&block[0x30]` and the pool built with `()` 34 (same code); the stores ordered g_479880,
// g_479884, g_479890 39; header: the size mask moved from the fixed base into the derived constructor's
// base argument 34 (all 57 functions of the header's ten files unchanged).
// DL257: Called pool constructor ordinary/forced inline, alone and with forced reference-
// count/fixed-base/fixed-derived constructor expansion: 34/1058/34/34/34/1058/1058/1058. Ordinary
// policies retain the omitted intermediate vtable store; forced policies expand the native call.
// Original declarations and complete body restored; the store and successful-arm reload remain.
// Round 42 (m3 lane 18, fast with 0x146b80/0x146e30/0x22c3e0/0x1c2690): testing the global while placing through
// `block`, `g_479894 = g_479888 ? new (block) ... : NULL;`, gives retail's arm reload and the 1 in EDX together, so
// only the vtable store differs: 35 (140 bytes, positional); as `if (g_479888) ... else g_479894 = NULL;` 35; on it the
// method6/method7 overrides declared in the fixed base 35, `block` typed as the fixed allocator with `block + 1` 35,
// the test inverted 36. Compile-only: a local placement always gives the 1 in a register and the join reload, a
// g_479888 placement the arm reload and immediate 1s (13 spellings); the store is dropped in every form.
// F377: Direct exhaustive constructor stores, terminal/zero-first arms and long/int/short/
// signed-char ordinals: 34/34/34/34/43/54/43/54. NOT kept: narrow forms worsen other
// constructor users; shared header and full body restored; base-vtable/arm reload remain.
// Round 86 (m3 lane 16): retail keeps the store only here; 0x146b80, 0x146e30 and 0x22c3e0 inline the same constructors and
// drop it. The fixed constructor defined in src/unknown_22c0a0.cpp 34 (plain, same code, siblings unchanged), the abstract one in
// src/unknown_1470f0.cpp 34 (fast, same code); a raw block struct with fixed/buffer/pool members 34, all through the global 38
// (144 bytes); placement from the global with the buffer from `block` 36 (146 bytes, still no store); the global tested
// with placement through `block` 35 (140 bytes).
// N384: Actual fixed/pool placement admission in bool/byte/short/long: eight 34.
// NOT kept: unmatched admission carriers; original complete body restored. Shared
// constructors untouched; intermediate vtable store and successful-arm reload remain.
// Lane 4 r166 (held, coordinator): MATCH only with a second c_havok_fixed_memory_base constructor taking the size alone
// (buffer = this + 1, the same stores as the (buffer, size) one) that ends in _ReadWriteBarrier(), plus __declspec(novtable)
// on the base: a same-behaviour constructor twin for this one caller carrying a barrier steer, not kept (twin ruling, as
// 0x45560 in lane 4 r165). Retail keeps the base's abstract vtable store only in this construction (not 0x146b80 or 0x146e30).
// build/held/lane4-r166-146a20-ctor-twin.patch is the whole change, a lead toward the match.
// F404: Pool/count outputs, derived/abstract pool result, current/next count
// before/after publication: 34/34/34/34/34/34/34/34. NOT kept: no exact result-storage form.
// Original placements, publications and body restored; intermediate vtable store and arm reload remain.
// B451: Per-element default then terminal override in the existing four-word
// constructor pass, long/int/short/signed-char ordinals: 34/34/43/43/34/34/43/54. NOT kept:
// no exact, nonregressing shared form. Entire constructor/header and target restored;
// intermediate vtable store and successful-arm reload remain.
// Round 70 (m3 lane 14, fast with 0x146b80/0x146e30/0x22c3e0, on `g_479894 = g_479888 ? new (block) ... : NULL;` 35):
// the abstract's zeros as one chained assignment and a NONE store 33 (0x146b80 6, 0x22c3e0 6); a novtable fixed base 35;
// the refcount 1 as a member initializer 35. Diagnostic only: the abstract constructor storing the refcount again 15
// (143 bytes: the duplicate +4 store survives where retail stores the vtable, so plain dead-store removal keeps it and
// the vtable store goes in a separate pass; 0x146b80 24, 0x22c3e0 202).
// P360: Nonthrowing abstract/fixed-base/fixed-derived constructor contracts, seven subsets, then the reference-count constructor contract: 34/34/34/34/34/34/34/34.
// NOT kept: no exact native source recovery; original complete body restored; intermediate vtable store and successful-arm backing reload remain.
// P368: Inactive-sentinel default followed by active-prefix zero in the existing four-word pass,
// long/int/short/signed-char, forward 34/34/43/54; reverse 33/33/43/54. NOT kept:
// no exact, nonregressing shared form; original header, constructor pass and target restored.
// Intermediate vtable store and successful-arm backing reload remain.
// P372: Four-word contents carried through the real array loop, long/int/short/signed-char
// ordinals: forward 34/34/54/54, reverse 33/33/54/54. NOT kept: no qualifying gain;
// reverse full-width forms lose two exact users, narrow forms also regress users.
// Entire shared constructor/header and original body restored; base-vtable/arm reload remain.
// T68: Starting/next reference-count captures before backing read/reset/fixed/
// pool construction:
// 36/36/36/28/39/39/39/26. NOT kept: early one-use counter observations require equality. Original executable body restored.
// Intermediate vtable store and successful-arm reload remain.
// T70: Explicit abstract-parent lifetime start, explicit count parent, count/member initializer
// boundaries: eight 34s; all 57 marked shared users unchanged. NOT kept: no intermediate-vtable
// or successful-arm reload repair. Original complete shared header and executable body restored.
// P388: Nonuniform four-word constructor range from automatic/static constant
// values via memcpy/copy/backward/uninitialized copy: 61/56/46/56/60/56/69/56.
// NOT kept: no exact intermediate-vtable/arm-reload recovery and shared-user
// regressions; complete original header, constructor chain and body restored.
// P398: Actual four-word constructor cursor, forward/reverse conditional 48/56;
// forward-prefix/sentinel-reverse-prefix 54/54; exhaustive forward/reverse arms 48/56;
// forward zero-default/reverse sentinel-default overrides 43/43.
// NOT kept: all regress, several outline the constructor, and both exact constructor peers lose.
// Original complete shared constructor and initializer restored; intermediate vtable/arm reload remain.
// T79: Existing four-word member lifetime via uninitialized_fill/uninitialized_fill_n,
// prefix/full zero ranges then sentinel, body/member-list fixed-base initialization: eight 56s.
// NOT kept: extent 129/146 and four shared-user regressions per form, losing both exact
// constructor peers. Complete header and body restored; intermediate vtable/arm reload remain.
// Round 81 (m3 lane 10, fast with this file, 0x22c3e0 and 0x1c2690): placement through g_479888 with the
// buffer and the pool through `block` 41; placement and buffer through g_479888 with the pool through `block`
// 34 (149 bytes: the 1s become immediates and `block` takes a pushed EDI); `block` read after the three
// stores with `block ? new ... : NULL` 34 (same code); `g_479890 = g_479880 = true` 36; the stores ordered
// g_479880, g_479890, g_479884 38. The base vtable store stays dropped in all.
// m3 lane 12 r103 (fast, 0x146b80/0x22c3e0 matched throughout): the buffer at `block + sizeof(c_havok_fixed_memory)` 34
// (same code); with g_479888 at both placements (38, 144 B, retail's code but for the base vtable store and 1 as
// immediates): an explicit `if (g_479888)` / `else g_479894 = NULL` 38 and the pool through a named pool pointer 38 (near
// repeats of the block-form if/else and published-result forms above); the placement through g_479888 with the buffer
// through `block` 36 (146 B, `mov edx, eax; add edx, 0x30`); placement and pool through `block`, buffer through
// g_479888 43 (139 B). 0x453760 is
// stored in retail only here and in the abstract allocator's deleting destructor 0x1470f0.
// M3 lane 14 r79 (fast with all 57 functions of the header's ten files): a virtual destructor declared in the fixed base
// 34 (the store still dropped, no other function changes). Diagnostic (header, removed at once): `if (buffer) m_count = 0;
// else m_count = 1;` in the fixed base constructor keeps the fixed base's own vtable store beside its field stores (61 at
// 162 bytes, 0x146b80 27): a branch present when dead stores are removed, between one vtable store and the next, keeps
// the earlier store even after the branch becomes a `sete`. So retail's kept 0x453760 store points at a branch or block
// boundary that this construction alone had there and that later vanished; in the shared constructors it would also
// reach 0x146b80, 0x146e30 and 0x22c3e0, which drop the store. No other untried form on this function's side is known.
// M3 lane 2 r67 (fast with 0x146b80/0x146e30/0x22c3e0): the fixed base's three member stores in reverse order 34 (same
// code here; matched 0x146b80 goes to 2), so not kept. Every constructor change reaches 0x146b80 too, which drops the
// abstract vtable store in retail, so the cause has to be one this call site alone has (retail's construction here keeps
// the store and reloads g_479888 in the arm, 0x146b80's does neither); the call-site forms above cover the placement,
// buffer and result spellings.
// m3 lane 10 r87 (fast with 0x146b80/0x146e30/0x22c3e0/0x1470f0/0x147110/0x22c0a0, header trials restored): an empty
// virtual destructor in the fixed base 34, in the fixed allocator 34 (retail's vtables give both the abstract's deleting
// destructor 0x1470f0, so neither derived allocator has its own); the fixed base's count cleared through an inline member call 34;
// the abstract's four words set through an inline member call 34 (near the listed compile-only init-method form),
// through a word cursor 34. The store is dropped after
// inlining whatever calls or loop shape the constructors have.
// Round 80 (m3 lane 13, fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0/0x146bf0): `byte *block` re-measured 34 (same
// code; listed above). No untried call-site form found: register-masked aligned 3, only the abstract vtable store and
// the arm reload. The store comes from the shared constructors, which matched 0x146b80 and 0x22c3e0 also inline (they
// drop it in retail); lane 4 r166's held constructor twin is the known match. Body unchanged at 34.
// Lane 6 r167 (fast with 0x146b80/0x146e30/0x22c3e0/0x146ac0, all unchanged; base 34, aligned 4 = the base vtable store
// and the g_479888 reload at the join): `void *const block` 34, the pool constructed with `()` 34, `g_4798b4 += 1` 34,
// `byte *const block` with `block + 0x30` 34 (all same code); re-measure: the fixed allocator in an abstract-typed local
// published on the next line 38 (137 bytes; the listed typed-local-published form). The lever left is the shared
// constructors in include/unknown_146a20.h (included by include/globals.h and contributor files) or the held twin.
// Lane 4 r186: no untried call-site or header form left, so no trial ran. The call-site lists above cover the placement, buffer,
// result, store-order and typed-block forms. The header lists above (rounds 7, 20, 37, 103, 300, T70, lane 14 r79, lane 2 r67,
// lane 10 r87) cover initializer lists, out-of-line definitions, novtable, virtual destructors, memset, explicit and chained
// stores, split and counted-down loops, member calls, a word cursor, a const table and the refcount initializer. The base vtable
// store comes from the shared constructors in include/unknown_146a20.h: BLOCKED-BY 0x146b80, 0x22c3e0 (matched; retail drops that
// store at both). The header reaches no contributor body: its contributor includers build none of these allocators.
// Round 155 (m3 lane 8): retail 0x146b80 (same constructor, base store dropped) stores the fixed base's count before
// its buffer and size; retail here keeps declaration order and keeps the store, so the two inlined copies differ in
// scheduling, not in source. No untried form on either side; body unchanged.
// MiniMax A (11-12): no new form, no lower score (34); the base vtable store still comes from the shared constructor in include/unknown_146a20.h, which this lane may not edit (BLOCKED-BY), and the successful arm still reloads the backing store.
// MiniMax G (11-13, later): no new form, no lower score (34): with the pool's placement read from g_479888 the
// reload sinks into the successful arm and the join matches retail's lea, but the constant one then becomes an
// immediate (38; 36 with a byte pointer), and with the placement read from the local the constant stays in edx and
// the reload stays at the join (34); the abstract allocator's dead vtable store still comes from the shared
// constructor in include/unknown_146a20.h (BLOCKED-BY), so the body stands.
// MiniMax G (11-13): no new form, no lower score (34): the abstract allocator's dead vtable store and the
// reload of the backing pointer on the successful arm are still missing; the shared constructor bodies they
// follow are outside this queue, so the body stands.
// score 34 (was 34): unchanged this round: the abstract allocator's dead vtable store and the reload of the backing
// pointer on the successful arm are still missing; the shared constructor bodies they follow are in another file.
// score 34 (was 34): unchanged this round: the abstract allocator's vtable store and the backing pointer's reload
// on the successful arm are still missing; both come from the shared constructors in include/unknown_146a20.h,
// which this lane may not edit.
// @retail 0x146a20
void function_146a20(void)
{
	void *block = g_479888;

	g_479884 = NONE;
	g_479890 = 1;
	g_479880 = true;
	g_479894 = new (block) c_havok_fixed_memory((byte *)block + 0x30, 0x15000);
	g_479898 = new ((byte *)g_479888 + 0x15040) c_havok_page_pool_allocator;
	g_4798b4++;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x146ac0
void function_146ac0(void)
{
	if (g_47989c)
	{
		function_146de0();
	}
	if (g_4798a0)
	{
		((c_havok_abstract_allocator *)g_4798a0)->~c_havok_abstract_allocator();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
	g_479894->~c_havok_abstract_allocator();
	c_havok_abstract_allocator *last = g_479898;
	_ReadWriteBarrier();
	g_479894 = NULL;
	_ReadWriteBarrier();
	last->~c_havok_abstract_allocator();
	g_479898 = NULL;
	g_479890 = 0;
	g_479880 = false;
}

#pragma function(_ReadWriteBarrier)

/* a page aligned block of virtual memory: its size and the offset back to
   the allocation sit before it */
static void virtual_memory_allocate_aligned(void **block_out, long size)
{
	void *result = NULL;
	byte *block = (byte *)VirtualAlloc(result, size + 0x1000, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);

	if (!block)
	{
		GetLastError();
	}
	else
	{
		byte *aligned = (byte *)(((dword)block + 0x1000) & ~0xfff);

		((long *)aligned)[-2] = size;
		((long *)aligned)[-1] = aligned - block;
		result = aligned;
	}
	*block_out = result;
}

// @retail 0x146b30
void function_146b30(void)
{
	virtual_memory_allocate_aligned(&g_479888, 0x105470);
}

// @retail 0x146b80
void function_146b80(void)
{
	byte *buffer = g_510c44;
	long size = HAVOK_SCRATCH_MEMORY_SIZE;

	g_510c48 = true;
	if ((dword)buffer & 0xf)
	{
		long adjustment = -(long)buffer & 0xf;

		buffer += adjustment;
		size -= adjustment;
	}
	g_47989c = new ((byte *)g_479888 + 0x105410) c_havok_fixed_memory(buffer, size);
}

// score 5 (was 5): retail's function_1c4040 takes the active flag in AL and reads
// it after the other arguments are pushed; here the callee takes it in DL, so the
// byte is loaded into DL before the other byte is stored. The callee lives in
// src/unknown_1c25a0.cpp and is far from retail.
// @retail 0x146bf0
void function_146bf0(void)
{
	if (!g_47f05b)
	{
		long level = function_146fd0();
		long last_time = g_479884;
		long attempt;

		g_47f05b = true;
		for (attempt = 0; attempt < 3 && level >= 2; attempt++)
		{
			long count = 0;
			long pass;

			if (level >= 3)
			{
				function_1c4590(1);
				last_time = g_479884;
			}
			level = function_146fd0();
			for (pass = 0; pass < 3 && level >= 2; pass++)
			{
				bool active = pass == 2;
				bool any_object = pass > 1;
				long tries = 0;

				do
				{
					bool recent = last_time != NONE && g_510c54->game_time - last_time < 10;

					if (!function_1c4040(attempt, active, any_object, recent, NONE))
					{
						if (!attempt && !recent)
						{
							function_146da0();
							level = function_146fd0();
							g_479884 = last_time = g_510c54->game_time;
							break;
						}
						last_time = g_479884;
						break;
					}
					level = function_146fd0();
					if (level < 2)
					{
						last_time = g_479884;
						break;
					}
					tries++;
					if (++count % 8 == 0)
					{
						function_146da0();
						level = function_146fd0();
						g_479884 = last_time = g_510c54->game_time;
					}
					else
					{
						last_time = g_479884;
					}
				}
				while (tries <= 50);
			}
		}
		g_47f05b = false;
	}
}

// @retail 0x146da0
void function_146da0(void)
{
	bool scratch = g_47989c != NULL;

	function_1c27a0();
	g_47f05a = true;
	function_1c2690();
	g_47f05a = false;
	if (scratch && !g_47989c)
	{
		function_146b80();
	}
}

// @retail 0x146de0
void function_146de0(void)
{
	if (g_47989c)
	{
		((c_havok_abstract_allocator *)g_47989c)->~c_havok_abstract_allocator();
		g_47989c = NULL;
		g_510c48 = false;
	}
	if (g_4798a0)
	{
		((c_havok_abstract_allocator *)g_4798a0)->~c_havok_abstract_allocator();
		g_4798a0 = NULL;
		function_12d520(g_47988c);
		g_47988c = 0;
	}
}

// MiniMax B (11-06) round 3: no new form, no lower score (120); the padding as the round-up of
// the block (the block plus 0xf masked with 0xfffffff0, minus the block) measured as a long
// adjustment inside the loop scores 143 and the same padding computed after the loop scores 149
// (149 again when the loop is respelled as a while with a break), all of them already in the
// earlier list; with the padding after the loop the size moves into EBP as retail keeps it, but
// the frame grows to 0x18 and the first rdtsc's high half moves into a slot of its own, so the
// body is unchanged.
// MiniMax B (11-06) round 2: no new form, no lower score (120); the block is unchanged: retail
// inlines the texture cache update (0x12c600) at this one call site and inlines the fixed-memory
// constructor, and both of those live outside the files this lane may edit, so the body stays 22
// bytes short with our size spill and counter registers.
// MiniMax B (11-06): no new form, no lower score (120); retail inlines the texture cache
// update (0x12c600) at this one call site and inlines the fixed-memory constructor, and both
// of those live outside the files this lane may edit, so the body stays 22 bytes short with
// our size spill and counter registers; the padding after the retry loop, which the earlier
// round measured at 151, stays worse.
// MiniMax b (11-04): no new form, no lower score; retail inlines the texture cache update (0x12c600) at this call site only, so the body stays 22 bytes short and keeps our size spill and counter registers.
// score 120 (was 120): the function still misses retail's inline of the texture cache
// update (0x12c600) at this one call site and stays 22 bytes short; unchanged this
// round
// score 120 (was 143): the function still misses retail's inline of the texture
// cache update (0x12c600) at this one call site, so it stays 22 bytes short;
// this round's trials in this file found no lower form of it
// score 120 (was 143): BLOCKED-BY 0x12c600: retail inlines the texture cache update (0x12c600)
// at this one call site only (its other 13 callers call it), ours calls it; the padding is consumed
// only when the allocation succeeds and the handle is published inside the retry loop.
// Lane 10 r15: `inline` on 0x12c600 inlines it at every caller (all 14 worse, matched 0x8c790,
// 0x1241b0, 0x194260 lost; reverted). Retail computes the padding after the loop:
// `long padding = (((dword)block + 0xf) & ~0xf) - (dword)block;` then `c_havok_fixed_memory(block,
// size - padding)` gives retail's prologue, frame entry, registers (size EBP, block EDI) and tail
// instruction for instruction, 151 here only because the missing inline leaves it 55 bytes short;
// the same as `size -= ...` 149, as a pointer difference in the argument 163, as `-(long)block & 0xf`
// 152. A local forceinline copy of the update (diagnostic) also inlines its callees 207.
// Earlier: global analysis off 120 (0x50 frame); frame pointer 161; size/speed 281/143; local
// analysis with frame/size 120/158; frame with size/speed 281/161.
// Constructed allocator result 164; conditional usable capacity 140 (frame 0x14); conditional
// padding 141; block/padding outer scopes 143; current block-first homes 143; construction scope
// 143; zero-first padding 143 at 377 bytes. Per-attempt padding after/before publication 149/149;
// consumed padding 139/151; guarded consumption 145; rounded padding/consumption 163/162; bounded
// search 149. Update defining-source inline/force-inline declarations 139/139; separate
// request/usable capacity 139; per-attempt request 139; split capacity plus inline/force-inline
// update 139/139; request plus inline 139; native handle plus split capacity 139. Byte/word
// alignment padding 150/140; allocation success result 143; explicit placement guard 143; post
// retry capacity 143. Constructor mask 139; repaired native construction result with/without mask
// 164/164; entry capacity plus mask 141; native buffer/address/padding-scope plus mask 139/139/139;
// actual masked-capacity local 138. Final rounded/low-bit/conditional/word padding (inside the
// loop's structure) 149/152/143/145; signed-word per-attempt padding 129 (kept then);
// capacity/allocation/joint home names 139/139/139. Signed-short delta 139; attempt/final short
// scope 129/129; short remainder consumption 136; entry capacity 131; unsigned capacity/native
// handle/separate usable with short padding 129/129/129. Wrapped/full boundary distances 164/149;
// direct consumption 164; counted distance 153; remainder update 140; byte padding 144; unsigned
// capacity/native allocation 139/139. Short-padding failure/body, counted and capacity loops
// 129/131/129; zero-default/select padding 134/129; allocator result 129; full short distance 135;
// success-only padding consumption 120 (held: the guard repeats the loop's own test). Separate
// byte/char/word/dword/short/long remainders with short padding 134/134/136/146/136/146; byte/word
// conditional padding 134/136.
// @retail 0x146e30
c_havok_abstract_allocator *function_146e30(void)
{
	if (!g_4798a0)
	{
		long size = 0x800000;
		byte *block;
		short adjustment;

		do
		{
			block = (byte *)function_80e14(size, 0, 0, 0, 90, 1.0f);
			g_47988c = (long)block;
			if (block)
			{
				adjustment = (dword)block & 0xf;
				if (adjustment)
					adjustment = 0x10 - adjustment;
				size -= adjustment;
			}
		}
		while (!block && ((size >>= 1) >= 0x100000));

		g_4798a0 = new ((byte *)g_479888 + 0x105440) c_havok_fixed_memory(block, size);
		function_1c4590(1);
	}
	return g_4798a0;
}

// @retail 0x146fd0
long function_146fd0(void)
{
	long physical_level = 0;
	long pool_level;

	if (g_4798a0 && g_4798a0->m_count > 0)
	{
		physical_level = 3;
	}
	long scratch_level = 0;
	if (g_47989c && g_47989c->m_count > 0)
	{
		scratch_level = 3;
	}
	if (g_479898)
	{
		long level;
		c_havok_page_pool_allocator *pool = (c_havok_page_pool_allocator *)g_479898;

		level = 3;
		if ((real)pool->page_count * (1.0f / 118.0f) < 0.8f)
		{
			real used = pool->get_used_fraction();

			if (used < 0.85f)
			{
				level = 2;
				if (used < 0.75f)
				{
					level = 1;
					if (used < 0.65f)
					{
						level = 0;
					}
				}
			}
		}
		pool_level = level;
	}
	else
	{
		pool_level = 0;
	}
	return MAX(scratch_level, MAX(pool_level, physical_level));
}
