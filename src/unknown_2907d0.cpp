#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1428b0.h"
#include "object_queries.h"
#include "slot_handler.h"
#include "unknown_11cc90.h"
#include <math.h>
#include <new>
// @flags /O2 /arch:SSE /Gr

struct s_actor_position_state
{
	s_actor_position_state();
	long flags;
	point3f position;
	short field10;
	byte unknown12[2];
	vector2f field14;
	long field1c;
	short field20;
	short field22;
	short field24;
	byte unknown26[2];
	short field28;
	short field2a;
	short field2c;
	short field2e;
	long field30;
	long field34;
	real field38;
	short field3c;
	short field3e;
	byte field40;
	byte unknown41[0x60 - 0x41];
	short field60;
};

__forceinline void function_290852(transform4x3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = arg_1->i;
	real local_1 = arg_1->j;
	real local_2 = arg_1->k;
	if (arg_0->scale != 1.0f)
	{
		local_0 = arg_0->scale * local_0;
		local_1 = arg_0->scale * local_1;
		local_2 = arg_0->scale * local_2;
	}
	arg_2->i = arg_0->up.i * local_2 + arg_0->left.i * local_1 + arg_0->forward.i * local_0;
	arg_2->j = arg_0->up.j * local_2 + arg_0->left.j * local_1 + arg_0->forward.j * local_0;
	arg_2->k = arg_0->up.k * local_2 + arg_0->left.k * local_1 + arg_0->forward.k * local_0;
}

vector2f *function_11df30(vector2f *angles, vector3f const *v);
transform4x3f *function_b8c00(long object_index, long *node_count);

// score 19 (was 19): earlier start 21. Round 116 (m3 lane 9, fast): descriptive names for every parameter and local
// 19, the parameter reference declared after the point and the vector 19, a const reference to the transform in the
// else arm 19 (all same code); the node indexed at both calls with the reference kept 51; re-measured `field2e = true`
// 19. Nothing in our build calls this function: 0x2902b0's body is `#if 0` and its stub is empty, so the convention and
// the EBX/EBP choice come from this body alone. No untried natural form is left.
// MiniMax G (11-12): no new form, no lower score (19); retail keeps the transform in EBP and takes the record in EDI, our build
// takes the record from the stack and the transform in EBX, so the caller's register choice (in another file) decides the rest.
// MiniMax E (11-11): no new form, no lower score (19): the else arm's two reads swapped 68, the position stored field by field 141, the transform read directly in one call 19; the transform still takes EBX and the point's y EBP, and the constant 1 stays shared between the result and field2e.
// Earlier at 19 (was 19): Ours keeps the transform pointer in EBX and the point's y component in
// EBP, retail keeps them the other way round; retail also stores the last field with a
// direct word store where ours moves it through EAX. Tried: the last field through the
// plain output pointer 34, with field1c too 35; field2e as word 21; field2e stored before
// field1c 22, right after field10 34 (the constant 1 stays shared with the result); every
// store plain with arg_2 read directly 49 (arg_2 then arrives in ecx); local_5 not volatile
// 35; only field1c/field2e through local_5 35; the angles through an inlined helper taking
// real pointers 113; arg_2 read once through a volatile cast instead of local_4 35; the
// result in a local also stored to field2e 21.
// Round 99 (lane 16): the angle pair is the existing 0x11df30 (as matched 0x203780 fills the same
// record), with field14 a vector2f: 21 -> 19, and the record's flags member is no longer volatile.
// Left: the transform in EBX and the point's y in EBP (retail the other way) and the constant 1
// shared between the result and field2e. Without the two steering locals that form reads 33 (464
// bytes: the transform arrives in ECX; the stores then sit in retail's order on their own); seven
// parameter orders 33; field2e as word 33; local_1 declared first 33. The existing 0x142640 for the
// private rotate 312 (stays a call). With the angle function: no volatile pointer 113 (453 bytes);
// only field1c or only field2e through it 19; no address-of local 124; a byte result 19; a result
// local with field2e = true 19; field10 stored before the position 19.
// Round 69 (m3 lane 2, fast, plain-confirmed): the node lookup through the existing 0xb8c00
// (the count through its out-parameter, the returned pointer advanced past the bounds test)
// 19, the same code: kept, as the count/offset reads were that getter's body written out;
// on it the record no longer needs its volatile pointer (19, same code: removed). Also on it:
// no address-of local and no volatile 124 (447 bytes); the getter's pointer plus the node
// index before the bounds test 119; the getter's pointer in its own local 19. On the old
// base: local_1 declared first 19 (same code), the no-entity copies swapped 68, one exit with
// a `placed` flag 136; no steering locals re-measured 33 (the transform arrives in ECX and
// goes to EBX: in the stack first pass it must load into EBP to stay a stack argument).
// Round 71 (m3 lane 2, fast): the transform read once through local_4 into a pointer local 113 (453
// bytes); the if/else inverted (`arg_0 == NONE` first) 137 (451), with no steering local 128 (462); no
// steering local with the node indexed in both calls (`&local_3[arg_1]`) 63 (464); the bounds test as a
// positive block with the false return in its else 19 (same code); placement new for the record 51 (474
// bytes: a NULL test); the transform in a pointer local declared after the point and vector, read
// once through local_4 113 (453), copied from the parameter with no steering local 33 (464; either
// place). Permuter (600 s, annealing): reduced image 144 for this body, nothing better in 47 tries.
// Helper input by value 135, by const reference 19; helper output by
// reference 19, returned vector 110; named products in every row 19;
// output accumulation 19; node count at entry 19; node pointer at entry 19.
// All restored; the register pair and shared constant remain.
// Round 88 (m3 lane 13, fast). Diagnostic, restored at once: with the only caller 0x2902b0 compiled (its stub taken out, its
// `#if 0` opened) this body is still 19 and the no-steering body 33, so the missing caller does not decide the convention. One
// exit (`bool result = false`, `goto done` at the bounds failure, `result = true` at the end) 139 (458 bytes) with the steering
// local and 133 without it (the result takes CL from the top); a result local set at the end beside the direct false return 19
// (same code); the rotate helper as a plain static or plain inline function 19, and 33 without the steering local (same code as
// forced: the link expands it either way). Diagnostics on the no-steering body, the transform still arriving in ECX and kept in
// EBX: one more read of the point after the reset 65 (483 bytes); the no-object arm's forward from a global, so the transform has
// fewer uses, 77. The transform as a by-value one-pointer aggregate (diagnostic) 86 (its load is no longer hoisted to the entry);
// as a reference parameter 33 (the no-steering code); the initial point assigned after its declaration 19; the record-filling tail as an
// inline function returning true, or returning nothing with `return true` after it, 89 each (438 bytes: 0x11df30 is then called
// from it and no longer expanded). All restored.
// Round 103 (m3 lane 10, fast): the else arm reading the parameter directly (the address local only in the node arm) 19; the
// address local as a reference to the parameter (`transform4x3f const *const &`) 19; the node count declared at function scope
// ahead of the point 19; the rotate helper returning its output pointer, as 0x142640 does, 19; the helper's input as one vector
// copy 130 (494 bytes). Diagnostic, restored: size preference for this body 252 (344 bytes, frame pointer and string moves), so
// the shared constant 1 is not a size/speed choice. Retail inlines 0x142640 itself here (the scale test reads [esi]); that
// function sits in an /Ob1 file with no `inline`, so ours calls it (312) and the private copy stands in. All restored.
// Round 106 (m3 lane 1, fast): the record as a reference parameter 19; the address local non-const 19 (same code each);
// the transform read once through the address local into a named pointer used everywhere 113 (453 bytes, a repeat of
// round 71's form); the point and the vector grouped in one local aggregate 115 (455 bytes, diagnostic); `inline` on
// 0x142640 with this body calling it in place of the private copy 19 (the same code), but matched 0x16c2f0 then inlines
// it (107, 528 bytes): reverted. The call to 0x200e60 shows as differing only because ours reads `[ebx]` where retail
// reads `[ebp+0]`, one byte shorter, which moves the tail by one byte until the shared constant 1 moves it back.
// Round 108 (m3 lane 1, fast): the transform parameter as a const pointer 19 (same code); explicit `__stdcall` 19 (same
// code), `__cdecl` 26 (caller-pops); the forward vector's address named once ahead of the branch for both arms 142 (454
// bytes). Diagnostic: the result as `arg_0 != NONE || arg_3 != NULL` folds to the constant and still shares it with
// field2e (19), so the sharing is the constant's, not the return statement's.
// Round 107 (m3 lane 11, fast): `field2e = true` 19 (same code); field1c and field2e stored ahead of the angle call 35;
// the node arm's point call given `&arg_2->position` (the address local kept for the rest) 19 (same code); the angle call
// ahead of the field10 store 32; the bounds test as `!(arg_1 >= 0 && arg_1 < node_count)` 19 (same code). Readings:
// matched 0x1b1f70 shares its constant 1 between a word store and the bool return exactly as ours does here, so the
// sharing is the compiler's default and retail's separate `mov al, 1` needs another cause; 0x1414d0 shows the same
// EBX/EBP trade (a stack parameter loaded at entry takes EBX in ours, EBP in retail). Diagnostic, reverted at once:
// the standard marker without the address local puts every argument on the stack, 137 (479 bytes).
// Round 112 (m3 lane 10, fast): the bounds test split (`arg_1 < 0` ahead of the node lookup, `>= node_count` after it) 89
// (462 bytes); a byte return type 19 (same code, the constant 1 still shared); field1c and field2e stored right after the
// constructor, ahead of the position, 39; the vector declared ahead of the address local and the point 19 (same code); the
// else arm's point copied per component through a position pointer 143 (485 bytes). All restored.
// Round 132 (m3 lane 12, fast with 0xb8c00, 0x142570 and 0x200e60, matched throughout): the record now arrives in EDI
// as in retail; what is left is the transform in EBX (retail EBP), the point's y in EBP (retail EBX) and the shared
// constant 1. The parameter orders retail's caller setup suggests (EDX first, the push, EAX, EDI last): record,
// object, transform, node 19; record, object, node, transform 19; transform, record, object, node 19 (same code: the
// body alone picks the registers); the transform read through the address local into a pointer local in each arm 19;
// the node pointer declared at function scope 19; the record filled through a reference 19; the else arm through
// pointers to the position and forward 81. Diagnostic: plain parameters with `__stdcall` and the function's address
// taken by a probe global (no address local) 137 (479 bytes: every argument on the stack, the transform in EDI). All restored.
// Round 178 (m3 lane 8, fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, matched throughout): the point bound by reference
// (`point3f &local_0 = point;`) 113 (453 bytes); the vector bound by reference 19 (same code); the parameter bound by a
// non-const reference with the address local taken of it 113 (453); a const reference to the parameter in place of the
// address local 19 (same code); the record fill and `return true` in a forced-inline tail helper 23 (the constant 1 still
// shared, the root's two products swap); no address local re-measured 33 (464 bytes). Same reading as 0x1414d0: retail
// gives the stack-passed pointer EBP (the one register no register argument uses) and the other cross-call value EBX.
// Round 107 (m3 lane 14, fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, matched throughout), aimed at the shared
// constant 1: a `result` flag started false, set true in each arm, the fill under `if (result)` and `return result`
// 138 (458 bytes: field2e then takes retail's immediate word store, but the flag lives in CL/BL); an uninitialised
// result with one `goto done` to the only return 19 (same code: threaded back to constants); `return arg_3 != NULL`
// 19 and `return arg_3->field2e != 0` 19 (both fold, same code); field2e set through a forced-inline member setter 19.
// Matched 0x1b1f70 (`x = 1; return true;`) shares the constant as ours does; the only matched non-sharing word store
// found (0x26ed40) has EAX busy as the store's base.
// M3 lane 11 r114 (fast): the else arm's point and vector copies as memcpy 19, the point alone 19 (same code); the node
// matrix through the existing 0xb8bd0 after 0xb8c00 gives the count 146 (451 bytes); the bound test as `node_count <=
// arg_1` 21 (the compare's operands swap); the initial point copied through a const reference to the global 127 (452
// bytes). The transform still takes EBX and the point's y EBP, and the constant 1 stays shared.
// M3 lane 11 r116 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, matched throughout): the placement part
// (constructor, position, field10, the angle call) as a forced-inline helper returning true, the caller then storing
// field1c and field2e and returning that result, 23 (the constant 1 still shared, the root's two products swap); the same
// helper returning nothing with a literal `return true` 23 (same code); that part as a forced-inline member of the
// record called on arg_3, field1c/field2e stored by the caller, 19; the whole fill as a forced-inline member returning
// true, `return arg_3->place_facing(...)`, 19; field1c and field2e through an inline member setter (mode, count) 19 (the
// three 19s the same code). Reading: 0x2902b0 declares the record, calls this, then overrides field1c/field2e (2 and 3)
// when its count is at most 1, and 0x256bd0 fills a local record with the same position, NONE, facing, 4 and 1 sequence;
// in retail both exits set AL at the same place in the pop sequence (after `pop ebp`), ours moves the true exit's load
// ahead of `pop ebp` because it is the shared 32-bit 1.
// M3 lane 6 r194 (fast with 0x142570 and 0x200e60, both MATCH; control 19, aligned 17, masked 5). Diagnostics (reverted at
// once): field2e = 2 gives 18 (the separate `mov al, 1` then matches; the EBX/EBP swap stays), no field2e store 44, field1c
// = 5 20, field10 = 7 20, so the shared constant 1 is not what decides the swap; one more read of the transform through
// the address local in the else arm 55, in the node arm 108, at the top 128, in both arms 116 (the transform stays in EBX
// each time). Tried: `__assume(arg_2 != NULL)` at the top 19, `__assume(arg_3 != NULL)` 19 (same code each); the node
// count initialised to 0 113 (453 B); the address local declared only in the else arm, the node arm reading the parameter
// directly, 19 (same code); the else arm copying the forward before the position 68.
// M3 lane 5 r203 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, matched throughout; control 19): the bounds test as
// two separate `return false` tests after the lookup 83 (462 B); the helper's scale read once into a named local 58 (retail
// reads [esi] at each product); the node pointer named as `&nodes[arg_1]` after the test 19; field2e typed `word` 19; the
// scale test as `!(scale == 1.0f)` 19 (the 19s the same code). Diagnostics (reverted): 1-7 unused initialised locals at
// the top 113 (k = 1, 453 B) then 19; on the body without the address local (33) one extra read of the transform in the
// else arm, the node arm or at the top 127/77/137, the transform still arriving in ECX and kept in EBX each time.
// M3 lane 12 r143 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): 1-12 unused initialised locals
// at the top 19 except K = 1 and K = 9 (113, 453 B: the convention changes; the EBX/EBP pair never moves); the bounds
// test split into `arg_1 < 0` and `arg_1 >= node_count` after the lookup 83 (462 B); the rotate helper scaling in place
// (`local_0 *= arg_0->scale;`) 19 (same code); `return arg_3->field1c != 0;` 19 (folds, the 1 stays shared); re-measured:
// the else arm through a const reference to the transform 19, the position stored field by field 141 (485 B). Diagnostic,
// removed: the rotate call ahead of the point call 96 (471 B). Reading: the callee-saved registers go ESI, EDI, EBP, EBX
// by priority, so ours ranks the point's y above the transform (EBP/EBX) and retail the transform above the y.
// M3 lane 4 r220 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): `(void)&arg_2;` with the
// parameter read directly in place of the address local 33 (464 bytes: folds, the transform arrives in ECX); forced-inline
// accessors returning the transform's forward or position address, used in both arms, 19 / 19, both 19 (same code: the
// accessor's copy does not take a use from the transform); the address local and the branch in their own block ending
// before the record fill 19 (same code). Both builds give the transform and the point's y the same uses, so the pair is
// a tie-break the source has not reached.
// M3 lane 11 r129 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the whole function as a
// member of the record (`this` the record, the tail through `this`) 19 (same code); that member without the address
// local 33 (464 bytes, the transform arrives in ECX and goes to EBX as on the plain no-local body). Diagnostic on the
// no-local body: 1-12 unused initialised locals at the top 33 each except K = 2 and K = 10 (124, 447 bytes); the
// transform never stays a stack argument, so no symbol count puts it in EBP in the first pass. Also: the rotate helper
// `static __forceinline` 19; every parameter declared `const` (the address local unchanged) 19; the transform parameter
// non-const 19 (all the same code).
// M3 lane 4 r223 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the node arm rotating
// into a fresh vector then copied to the forward local 110 (494 bytes); the point call writing a fresh point then
// copied 131 (470 bytes: the initial copy of the global point is then dead and dropped); the angle pair computed into
// a local pair then stored 71 (486 bytes); the constructor called through a record pointer local 19; the point copied
// into a const local ahead of the constructor and stored from it 19 (same code each).
// M3 lane 15 r110 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the transform parameter
// non-const 19 (same code); the origin copy moved into the node arm (the else arm overwrites it) 33 (the copy follows the
// branch; the EBX/EBP pair stays); the whole function as a member of the record (`this` for arg_3) 113 (453 B, the
// convention changes); two stages, the node matrix chosen first (NULL when there is no object, false return on a bad
// node) and then `if (node)` transform else copy 134 (478 B), the same with `if (arg_0 != NONE)` as the second test 19
// (same code). In both builds the point's x outranks y and z (x ESI, z spilled), so the transform sits between x and
// y in ours and below y in retail.
// M3 lane 7 r194 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the forward copied into the
// vector local at its declaration with the else arm copying only the position 139 (451 B); the record filled through a
// pointer local 19 (same code); no address local, the position and forward pointers named once ahead of the branch for
// both arms 147 (451 B); the node bound to a const reference `local_3[arg_1]` for both calls 19 (same code); the node
// index widened into a named long for the test and the index 36.
// M3 lane 6 r209 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the node arm's forward address
// named in a pointer local after the point call 19, ahead of it 19, the node arm's position address named 19 (the three the
// same code); the else arm's forward copied through a pointer local 67, with the node arm's pointer as well 67.
// M3 lane 3 r279 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the record fill and `return true`
// written in the node arm as well, the else arm falling into the shared tail, 189 (509 B); the vector declared ahead of the
// point 19; the rotate helper's parameters as (out, transform, in) 19; the helper taking the transform by const reference 19;
// the address local itself `const` 19 (the 19s the same code).
// M3 lane 6 r213 (fast; control 19): the position address named once ahead of the branch for both arms (address local
// kept) 138 (452 B); the forward copied into the vector local at its declaration with the node arm rotating it in place
// 136 (454 B); `1.0f != arg_0->scale` in the rotate helper 103 (474 B: the scale load moves, the pair stays); `NONE !=
// arg_0` 19 (same code). Diagnostics (reverted at once): one extra store of the point's y into the record 52 (481 B, the
// transform still in EBX); the else arm's forward from a global, so the transform has fewer uses, 111 (still EBX).
// M3 lane 16 r155 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the loaded transform
// bound once as `transform4x3f const *const &` for both arms 113 (453 B); the rotate helper's scale bound by const
// reference 19; the address local itself const 19; the node arm's forward from the parameter directly 19; the else
// arm's forward from the parameter directly 19 (the 19s the same code).
// M3 lane 3 r281 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the transform through a
// pointer local scoped to the else arm only 19, to the node arm only 19; the node arm's point and forward calls in one
// forceinline helper taking (node, transform, out point, out vector) 19 (the three the same code); the position stored
// after field10 and the angle call 41 (472 B); field1c and field2e stored between the position and field10 35.
// M3 lane 3 r285 (h2m fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): `field2e = 1` stored
// ahead of the angle call 34; `field1c = 4` stored ahead of it 33; the record's position stored with memcpy 19; the else
// arm's two copies through a forceinline (transform, point out, vector out) helper 19 (the 19s the same code); field10
// stored ahead of the position with field2e ahead of field1c 20.
// M3 lane 16 r161 (fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the position stored from a
// `point3f const &` bound to the point 19 (same code); the position stored after the facing call, ahead of field1c, 41
// (472 B). Diagnostics (reverted at once): the rotate helper scaling unconditionally 135 (436 B), no bounds test 170
// (426 B), the point's y stored into four more fields 99 (496 B, y then in memory): the transform stays in EBX each time,
// so neither the blocks it spans nor the y's use count decides the pair.
// M3 lane 6 r220 (h2m fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): `bool result = true;`
// declared first and returned at the end (the bounds failure still `return false`) 113 (453 B, the convention changes as
// with one unused local); the node indexed in both calls (`&local_3[arg_1]`, no `+=`) with the address local kept 51 (the
// node index arrives in CX and the lookup reorders); field2e stored ahead of field1c after the angle call 20; the bounds
// test as `arg_1 >= node_count || arg_1 < 0` 132 (474 B); the else arm's copies from `point3f const &` / `vector3f const &`
// bound to the transform's position and forward 81.
// M3 lane 16 r163 (fast with 0x200e60, MATCH each time; control 19): the constructor as a placement new 51 (474 B: a NULL
// test of the record); the bounds test as `if (in range) { node work } else return false;` 19 (same code); the object test
// with its arms swapped (`== NONE` first) 137 (451 B); a redundant store of the forward at the head of the else arm 19 (same
// code); `__stdcall` on the definition 19 (same code), and with no address local 33 (the transform still arrives in ECX:
// the declared convention does not change the first pass).
// M3 lane 8 r216 (h2m fast; control 19): a `valid` flag started true, cleared on a bad node, the fill under `if (valid)` and
// `return valid` 107 (453 B); the rotate helper reading its input inside each scale arm (scaled or plain) 115 (456 B); the
// address local declared separately in each arm 19; the position stored through a `point3f &` to the record's field 19
// (both the same code); the angle call ahead of the position and field10 stores 39 (472 B). Diagnostics (reverted at
// once): a conditional store probe at nine places (entry, node arm top, after the bounds test, between the two node calls,
// else arm top and middle, before and after the constructor, before field1c) 66 to 150, and deleting in turn the point
// call, the rotate call, the constructor, the angle call, the two tail stores, the position store or one of the else arm's
// copies 51 to 297: the transform never takes EBP (EBX, or EDI when the convention changes), so neither one block nor one
// call decides the pair; the file without /Gr 19 (same code). Under the plain stack convention (no address local) the
// constant 1 is not shared (`mov word ptr [eax + 0x2e], 1` then `mov al, 1`: EAX holds the record there).
// M3 lane 15 r125 (h2m fast with 0x200e60 and 0x142570, MATCH; control 19): no address local re-measured 33 (464 B: ECX copied
// to EBX at entry); no address local with the else arm copying the forward first 80 (462 B); no address local with the
// transform declared last 33 (same code); a `bool result = true;` returned at the end with the node pointer named (`node =
// local_3 + arg_1`) 19, the named node pointer alone 19 (same code each).
// M3 lane 16 r169 (h2m fast with 0x200e60 and 0x142570, MATCH each time; control 19): the parameters as (record, object,
// node, transform) 19 and as (transform, object, node, record) 19 (same code: nothing compiled calls it); the node arm's
// point into its own `point3f` then copied to the shared point 131; the vector declared ahead of a bare point assigned from
// the global 19; the else arm's copies through a `transform4x3f const &` bound to `**local_4` 19, the node arm's two calls
// through that reference 19 (the 19s the same code: transform EBX, y EBP, the constant 1 shared).
// M3 lane 16 r171 (h2m fast with 0x200e60 and 0x142570, MATCH each time; control 19): the record fill in its own `{}`
// block 19; the address local declared after the point and the vector 19; the node count declared at function scope 19;
// the transform, point and vector renamed `transform`, `point` and `facing` 19; the address local declared inside the node
// arm only, the else arm copying from the parameter directly 19 (all the same code: transform EBX, y EBP, constant 1
// shared).
// M3 lane 1 r151 (h2m fast with 0xb8c00, 0x200e60 and 0x142570, all MATCH; control 19): one to three redundant paren
// pairs around `arg_0 != NONE` 19/19/19, around the rotate helper's first scale product 19/19/19, around each side of the
// bounds test 19/19. Diagnostics (reverted): 1-8 unused initialised locals inside the rotate helper 19 each; 16 and 17 at
// the top 19 / 113 (453 B: the convention changes at K = 1, 9, 17, a mod-8 state; the EBX/EBP pair never moves).
// M3 lane 6 r227 (h2m fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): the transform read through a
// new `__forceinline` identity inline returning its pointer argument at the node arm's point call, at its vector call, at the
// else arm's position copy, at its forward copy, at both node calls, at both copies, at all four, and around the node pointer
// passed to the rotate helper 19 each (the same code: no scratch or callee-saved shift here, the transform keeps EBX and the
// point's y EBP). Reading of the tail: retail sets the result with a byte `mov al, 1`, so AX is not 1 and field2e takes an
// immediate; ours materialises a 32-bit `mov eax, 1` shared by the result and the word store.
// M3 lane 7 r214 (h2m fast with 0xb8c00, 0x11df30, 0x142570 and 0x200e60, all MATCH; control 19): no address local, the
// node arm and the else arm's position reading the parameter, the else arm's forward through one volatile read of the
// parameter's address 87 (472 B); the bounds test's second compare commuted (`node_count <= arg_1`) 21; the bounds test
// split in place into two early returns 83 (462 B); the node pointer declared bare at function scope and assigned in the
// node arm 19 (same code); the node pointer formed in one statement ahead of the bounds test (`function_b8c00(...) + arg_1`)
// 119 (472 B). Re-measured: `field2e = true` 19, the transform parameter declared `const` 19, the else arm's copies as
// memcpy 19, the position stored field by field 141 (485 B).
// @retail 0x2907d0
bool function_2907d0(long arg_0, short arg_1, transform4x3f const *arg_2, s_actor_position_state *arg_3)
{
	transform4x3f const *const *local_4 = &arg_2;
	point3f local_0 = *g_468788;
	vector3f local_1;
	if (arg_0 != NONE)
	{
		long node_count;
		transform4x3f const *local_3 = function_b8c00(arg_0, &node_count);
		if (arg_1 < 0 || arg_1 >= node_count)
			return false;
		local_3 += arg_1;
		transform4x3f_apply_point(local_3, &(*local_4)->position, &local_0);
		function_290852(local_3, &(*local_4)->forward, &local_1);
	}
	else
	{
		local_0 = (*local_4)->position;
		local_1 = (*local_4)->forward;
	}
	arg_3->s_actor_position_state::s_actor_position_state();
	arg_3->position = local_0;
	arg_3->field10 = NONE;
	function_11df30(&arg_3->field14, &local_1);
	arg_3->field1c = 4;
	arg_3->field2e = 1;
	return true;
}
