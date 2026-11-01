#include "unknown_11c920.h"
#include "effects.h"
#include "vector_products.h"

// @flags /O2 /arch:SSE /Gr

struct s_247fd0
{
	real field_00[17];
	dword field_44;
	void *field_48;
	void *field_4c;
	void const *field_50;
};

struct s_247fd3
{
	word field_00;
	word field_02;
	long field_04;
};

struct s_248000
{
	word field_00;
	union
	{
		word field_02;
		struct
		{
			word : 13;
			word field_02_13 : 3;
		};
	};
	long field_04;
	real field_08;
	real field_0c;
	real field_10;
	word field_14;
	word field_16;
	word field_18;
	word field_1a;
	point3f field_1c;
	vector3f field_28;
	real field_34;
	real field_38;
	dword field_3c;
};

class c_248228
{
public:
	virtual void function_248228() = 0;
	virtual void function_248229() = 0;
	virtual void function_24822a() = 0;
	virtual void function_24822b() = 0;
	virtual void function_24822c() = 0;
	virtual void function_24822d() = 0;
	virtual long function_24822e() = 0;
	virtual s_effect_particle_system_definition *function_24822f(word arg_0) = 0;
};

struct s_particle_spawn_definition;
struct s_particle_spawn_state;
void function_249aa0(s_particle_spawn_definition const *arg_0, s_particle_spawn_state *arg_1,
	point3f const *arg_2, real const *arg_3, vector3f const *arg_4);
void __stdcall function_173ba0(dword arg_0, void *arg_1, void *arg_2, void const *arg_3, real *arg_4);

PRIVATE __forceinline void function_248076(dword *arg_0, real &arg_1)
{
	arg_1 = function_x82e52f(arg_0, 0, 0);
}

// score 70 (was 70): frame 0x10 and length agree. Retail stores field_10 before the field_38
// sample; written in that order (133) everything from the 0x173ba0 arguments to the end matches,
// but the cached-record compare splits into `mov eax,[ebx+0x50]; cmp ebp,eax` after the sample's
// x87 store (+2 bytes, adds the child-loop pad) where retail folds `cmp ebp, [ebx+0x50]`. With
// field_10 last (kept) the compare folds but the 0x173ba0/0x249aa0 argument registers and the
// child loop rotate (its unknown04 temporary takes ECX once the seed pointer is dead; retail's takes
// EDX while it is live). Retail also loads the parent twice for the 0x175070 list addresses where
// ours reuses one register. This compiler never folds a load into an ALU operand right after an x87
// fstp inside one block (compile-only: cmp reg/imm, test byte and add all split, also with an
// integer store between; /G7, /Os or /arch:SSE2 fold it compile-only, the file's flags don't; /G7 on
// this file in the real build still splits); retail folds at three sites (here, 0x39a80, 0x186ab0)
// and the one matched fold (0x2c7410) is at a jump target. A plain fild store or an SSE product folds.
// Kept: in-loop lookup through the inline particle_object_for_tag with the child getter nested in
// 0x173fd0's call 133; record-first reset without the split temporary 133; field_38 sample before
// the field_10 store 70.
// Tried: nested getter alone 160; inline/forceinline redeclarations of 0x137bd0 142; sample through
// the existing helper 70; the 0x173c10 method called 70; sample into the shared local before
// field_10 146; sample local declared first 70; sample local at function scope 70, with field_10
// first 133; field_10 first with the sample stored after the compare 137, or drawn into a local and
// stored after field_10 146; field_10 and the fifth draw in a forced-inline helper 133; fifth sample
// through a field pointer 133; DATUM child 70; size optimization 525; inline depth one/two 142/142;
// global optimization off 433; frame pointer 314; current-record update argument 142; field_10
// first 133, with a compiled-out statement before the compare 133, with the cache-update stores
// swapped 129, with the sample through the existing reference helper into field_38 or into local_9
// 133/133, with the compare as a bool local 133, with the compare operands swapped 133; compile-only,
// all split: compare in an inline/forceinline helper or bool local, reversed or == compare,
// typed/int compare, cache pointer copy or member, loop/switch/goto wrappers, sample through an
// inline initialiser, a named-global or divided scale, helper or direct word samples,
// field_08/local_3/link/flag statement orders, record-typed cache field, 0x175610's cache store
// order, an inline cache-update helper, the scale through the shared word wrapper or a named
// temporary; inline type lookup at the loop re-fetch 134, with the child-settings query in its
// argument list 133 (a copy of 0x137bd0's body); comparison operands swapped 133; child/record datum
// through DATUM 133/133; 0x175610's cache-update body 133; complete reconciliation before final
// draw 133; final sample store after reconciliation 140, with early predicate/flags/both
// 138/149/160; early cache predicate/masked flags/both across final draw 157/153/177; sample output
// 142; age/time after sample 239/153; sample late age/time/both 239/153/236; sample value 142;
// child-input scope 142; metadata read order 142; direct child getter 160; early result declaration
// 142; word flag update 244; reciprocal default first 142; cache slot/early flags/both/cache
// reference 142; reciprocal input 143; read-only settings view 142; spawn type at use 254; shared
// parent/child/both datum accessors 246/146/298; native child ordinal 142; immutable creation inputs
// 142; reciprocal before child link 143; native cache pointer 142; changed-record predicate 142;
// actual head field 142; unsigned child handle 142; shared bounded selector 142; word flags 142,
// with bounded selector 142; child zero-age before enable 142; cache mask field/compare 142/142;
// existing cache evaluator/native receiver 142/142; refreshed interface lifetime 142; child-result
// declaration home 142; live lifetime field 142; mask/lifetime fields 142; common pointer/flags/both
// stores 159/140/158; conditional mask 282; immutable allocated handle/settings 142; common
// current-record stores and combined immutable bindings 158; common flags with current receiver 158;
// external parent/allocated record/settings/child references, singly and in pairs, 142 each;
// parent/settings/both metadata fields, first/last/both list fields, metadata with list fields,
// metadata with cache slot 161/160/167/142/142/142/167/167; declared sample array/loop 142/435;
// immutable record/spawn/both 142; child identity order 142; earlier enable 142; earlier age 143;
// child effect/tag/settings home names, pairs, all three, allocation/spawn names 142 each; inline
// link-child helper (parent pointer argument), with the parent store inside 70/70; cache-update
// inline helper with early return in retail store order 133; sample through the existing helper in
// retail store order 133; child result / loop index declared before the loop 70/70; tag index local
// 245; child definition local 76; first lookup through the inline particle_object_for_tag 69;
// list-address pointer locals 70; retail order through the shared sample local 133; declared
// cache-record type, direct four random returns, both, combined initial flags, flags/type and
// flags/direct draws 70 each; declared type with retail timestamp order 133, with all three forms
// 133; named seed pointer for the five draws 88 (863 bytes), with field_10 first 166; field_10 first
// with the compare as a bool local 133; compare operands swapped 71; count increment before the
// timestamp store 71; field_10 after the field_14/16/18 sample 184/168/152; field_50 read into a
// named pointer for the compare 70; retail order with the compare operands swapped 133; field_10
// stored right after the record link/record pointer/field_08/flag bits/seed-type bitfield
// 227/178/200/217/204.
// With field_10 last its unknown04 temporary takes ECX (the seed pointer is dead by then; retail's
// takes EDX while the seed pointer is live), which starts the scratch rotation after the compare.
// Retail's fold after an fstp occurs at only one other site (0x39a80); no matched function has
// either form. Count incremented before the field_3c store 71; short child index 76; with
// field_10 first 137; sample drawn into a local before the field_10 store 146; field_10 stored
// after field_08 200.
// Round 96: a minimal one-file LTCG test (build/r96/hfold) reproduces the split: `fstp [r+0x38]` directly
// followed by `if (rec != cache->last)` always gives `mov eax, [m]; cmp r, eax` (operands swapped, a named
// cache local, double or signed draws, an if-else, callee-saved receivers: all split), and any instruction
// between them (another field store) gives the folded `cmp r, [m]`. Child test as `continue` 70; owner fields
// stored in the other order 70; count increment before the field_3c store 71; parent store before the link
// call 99 (864 bytes); field_10 stored before the four draws 200.
// Round 83: field_10 first 133 (aligned 21: the split compare and the child-loop registers);
// loop test written count-first 71; child ordinal passed as word 70; record/parent stores swapped
// 70; child definition read into a named local 76; -1 literal for the last 0x173fd0 argument 70.
// Round 18: compile-only, the split is this compiler's default tuning after an x87 store that
// follows x87 arithmetic in one block; it folds after a call's fstp, when the compare starts a
// block (a jump target), or with /G7 or /Os. Screens of inline sample/compare/update helpers,
// do/while, a one-pass loop, an empty if and identical store arms all split. Full builds with
// field_10 first: 133 (aligned 21, 876 bytes); with a label before the compare 140; the sample
// through the reference helper 133; the cache update as an early-return inline helper 133;
// typed compare 133.
// 133; named seed pointer for the five draws 88 (863 bytes), with field_10 first 166; compare
// operands swapped 71; count increment before the timestamp store (or the field_3c store) 71; short
// child index 76, with field_10 first 137; field_10 after the field_14/16/18 sample 184/168/152;
// field_50 read into a named pointer for the compare 70; field_10 stored right after the record
// link/record pointer/field_08/flag bits/seed-type bitfield 227/178/200/217/204.
// Round 41 (fast): parent's unknown04 read into a local before the draws and stored last 243 (876
// bytes), stored in retail's place 234; a parent pointer local for the first child-list address
// 70 (copy propagated), with field_10 first 133; the four word samples' local in its own block 70,
// with field_10 first 133.
// Round 314: Independent sample outputs with direct/named scales and computation/entry declarations:
// 70/70/70/70; in retail timestamp order 133/133/133/133. Original sample workspace restored; folded
// compare and downstream register rotation remain.
// Round 31 (lane 10): retail's field_10 copy sits ahead of the fifth draw's seed load and store,
// which no scheduler could cross (both are stores through unrelated pointers), so retail's source
// order is field_10 first, and the fold after the fstp is the build's, not the source's. On field_10
// first: the seed through a named pointer 133 (876 bytes); the cache update storing the flags before
// the record 129 (863); the sample as `(real)_random(...) / 65535.f` 133; the field_10 copy cast to
// real 133; the cached record read into a typed local for the compare 133.
// Lane 15 r19: field_10's value read into a real local before the fifth draw and stored after it 144
// (876 bytes, through XMM0); the cache update's two stores swapped 141 (864 bytes), with field_10 first
// 129 (863 bytes); the child datum or the pool record address by byte arithmetic 70/70 (same code).
// Round 112 (fast): field_10 copied after the cache update 147 (878 bytes: the fstp then meets the compare
// and splits it), with the compare operands swapped 147; the fifth sample as `(real)(dword)_random(...) *
// (1.0f / 65535.0f)` 70 (same code as 0x82e52f's), with field_10 after the cache update 147, with field_10
// first 133. Everything after the compare is a scratch-register rotation that starts at field_10's temp
// (ECX here once the seed pointer is dead, EDX in retail's order while it is live).
// Round 30 (lane 13, fast): field_10 copied between the fourth draw and its field_1a store 141 (876
// bytes); the 0x173ba0 mask as `0xf80f & ~field_44` 70; the child's unknown08 as one ternary store 98;
// `field_02 += 1` 70; `NONE != child` 70. Compile-only with field_10 first, all split: a label before
// the compare, a flags read between, the sample through a named local, operands swapped, a cache local.
// Round 51 (lane 16, fast with 0x248cc0): field_10 first with the fifth draw in its own brace scope 133
// (876 bytes), with the cache compare block in its own brace scope 133 (both still split the compare).
// Round 102 (m3 lane 8; one-link LTCG harness, exact): in the kept form field_10's temporary takes ECX where retail's
// takes EDX (ECX then holds the live seed pointer) and every later scratch register follows; the child loop also
// reuses EAX for the second list address where retail reloads the parent. Fast: field_10 first 133; field_10 first with
// `arg_0->field_50 != local_1` and the cache stores swapped 129 (863 bytes); with a named record copy for the compare
// 133; child unknown4c stored before parent 70 (same code); count increment before the field_3c store 71. Harness:
// field_38 through 0x248076's reference, through local_9 or a named sample local, a named cache pointer: all as T1.
// Round 9 (m3 lane 18, fast): retail's compare is not a jump target (nothing branches to it), so its fold right after
// the fstp is the build's. field_10 first with another store moved between the field_38 sample and the compare:
// the field_08 clear 230 (876 bytes), the count increment 200 (860 bytes; 0x248cc0 then 15 in the reduced image),
// the list-head link 243 (878 bytes); field_10 last with the fifth sample through 0x248076's reference straight
// into field_38 70 (same code); the count increment moved before the 0x173ba0 call 159 (860 bytes; 0x248cc0 15).
// Round 186 (fast): aligned against retail, field_10 first differs only in the split compare (+2 bytes, which
// adds the child-loop pad) and the child loop's registers (the parent loaded once for both list addresses,
// the effect/tag/vtable loads rotated), the kept form in the field_10 order and the same child-loop rotation
// (aligned 51). The fifth draw split into its seed step and its scaling with field_10 between 143 (876
// bytes); field_10 first with the compare as `== ... ; else` 133; re-measured: field_10 first 133, with the
// cache flags stored first 129 (863 bytes), with the sample in a named local 133. On field_10 first, the child
// loop: the effect index read into a local before the loop 131 (875 bytes), the parent stored before the link
// and both list addresses taken through the child's parent 128 (872 bytes), the child test as `continue` 133.
// Round 57 (m3 lane 9, fast): field_10 first with the fifth sample in a local stored in both arms of the cache
// test 156 (879 bytes, not hoisted); field_10 first with the compare against the record re-derived from the pool
// 190 (878). Retail reloads arg_2 from its stack slot at every child-loop use (twice for the two list addresses);
// the second list address, or every child-loop use, read through `&arg_2` 70/70 (copy-propagated, same code);
// with field_10 first 133.
// Round 142 (lane 4, fast with 0x248cc0): field_10 first with the cache test as an empty `if (==) {} else`
// 133 (876 bytes), through a `bool record_changed` 133, as `field_50 != local_1` with the mask cleared first
// 129 (863 bytes), through a `parent` pointer for arg_2 133; the count increment next to the list link 208
// (860 bytes); field_3c stored before the 0x249aa0 call 67 (a reorder past a call that takes the record).
// @retail 0x247fd0
void function_247fd0(s_247fd3 *arg_1, s_particle_system_datum *arg_2,
	point3f const *arg_3, vector3f const *arg_4, dword arg_5, s_247fd0 *arg_0)
{
	long local_0 = record_pool_allocate(g_51ec84);
	if (local_0 != NONE)
	{
		s_248000 *local_1 = &((s_248000 *)g_51ec84->data)[local_0 & 0xffff];
		s_effect_particle_system_definition *local_2 = arg_2->function_1751d0();
		s_particle_spawn_definition const *local_3 = *(s_particle_spawn_definition const **)local_2->unknown34;
		local_1->field_04 = arg_1->field_04;
		arg_1->field_04 = local_0;
		local_1->field_02 &= 0xe004;
		local_1->field_02 |= 4;
		local_1->field_02_13 = (_random(&g_4e7408->seed, 0, 0) * 5) >> 16;
		local_1->field_08 = 0.f;
		real local_9;
		function_248076(&g_4e7408->seed, local_9);
		local_1->field_14 = (word)real_truncate(local_9 * 65535.f);
		function_248076(&g_4e7408->seed, local_9);
		local_1->field_16 = (word)real_truncate(local_9 * 65535.f);
		function_248076(&g_4e7408->seed, local_9);
		local_1->field_18 = (word)real_truncate(local_9 * 65535.f);
		function_248076(&g_4e7408->seed, local_9);
		local_1->field_1a = (word)real_truncate(local_9 * 65535.f);
		local_1->field_38 = function_x82e52f(&g_4e7408->seed, 0, 0);
		local_1->field_10 = arg_2->unknown04;
		if (local_1 != arg_0->field_50)
		{
			arg_0->field_50 = local_1;
			arg_0->field_44 &= ~0xf80f;
		}
		function_173ba0(~arg_0->field_44 & 0xf80f, arg_0->field_48, arg_0->field_4c, arg_0->field_50, arg_0->field_00);
		arg_0->field_44 |= 0xf80f;
		function_249aa0(local_3, (s_particle_spawn_state *)local_1, arg_3, arg_0->field_00, arg_4);
		local_1->field_3c = arg_5;
		++arg_1->field_02;
		c_248228 *local_4 = (c_248228 *)function_137bd0(local_2->tag_index);
		for (long local_5 = 0; local_5 < local_4->function_24822e(); ++local_5)
		{
			long local_6 = function_173fd0(local_4->function_24822f((word)local_5), arg_2->effect_index,
				local_2->tag_index, (short)local_5, NONE);
			local_4 = (c_248228 *)particle_object_for_tag(local_2->tag_index);
			if (local_6 != NONE)
			{
				s_particle_system_datum *local_7 = &((s_particle_system_datum *)g_510c74->data)[local_6 & 0xffff];
				particle_system_link(local_7, &arg_2->first_child_index, &arg_2->last_child_index);
				local_7->parent = arg_2;
				local_7->unknown4c = local_0;
				real local_8 = local_1->field_0c > 0.f ? 1.f / local_1->field_0c : 0.f;
				local_7->flag0 = true;
				local_7->unknown04 = 0.f;
				if (local_8 > 0.f) local_7->unknown08 = 1.f / local_8;
				else local_7->unknown08 = 1.f;
			}
		}
	}
}

struct s_248cc0
{
	long field_00;
	long field_04;
	long field_08;
};

struct s_248ce2
{
	short field_00;
	word field_02;
	long field_04;
	long field_08;
	real field_0c;
	byte field_10[0x4c - 0x10];
};

struct s_particle_emitter_datum;
void function_2483f0(s_particle_emitter_datum *arg_0, long *arg_1, long *arg_2);

// @retail 0x248cc0
void function_248cc0(s_248cc0 *arg_0, s_247fd0 *arg_1, s_particle_system_datum *arg_2,
	point3f const *arg_3, vector3f const *arg_4)
{
	s_record_pool *local_0 = g_51ec88;
	if (arg_0->field_04 == NONE)
	{
		long local_1 = record_pool_allocate(local_0);
		if (local_1 == NONE)
			return;
		s_248ce2 *local_2 = &((s_248ce2 *)local_0->data)[local_1 & 0xffff];
		local_2->field_08 = NONE;
		local_2->field_02 = 0;
		local_2->field_04 = NONE;
		local_2->field_0c = 0.f;
		function_2483f0((s_particle_emitter_datum *)&((s_248ce2 *)local_0->data)[local_1 & 0xffff], &arg_0->field_04, &arg_0->field_08);
	}
	if (arg_0->field_04 != NONE)
		function_247fd0((s_247fd3 *)&((s_248ce2 *)local_0->data)[arg_0->field_04 & 0xffff], arg_2, arg_3, arg_4, arg_2->color, arg_1);
}

#include "unknown_0259a0.h"

struct s_247950
{
	word field_0;
	word field_2;
	long field_4;
	byte field_8[8];
	matrix3x3 field_10;
	point3f field_34;
	point3f field_40;
};

struct s_placement
{
	point3f position;
	vector3f direction;
	vector3f up;
	short value24;
};

class c_247951
{
public:
	virtual void function_247952() = 0;
	virtual void function_247953() = 0;
	virtual void function_247954() = 0;
	virtual void function_247955() = 0;
	virtual dword function_247956() = 0;
	virtual long function_247957() = 0;
	virtual long function_247958() = 0;
	virtual s_effect_particle_system_definition *function_247959(word arg_0) = 0;
};

struct s_object_246eb0;
struct s_particle_impact;
void function_249170(real const *arg_0, s_particle_spawn_state *arg_1, s_particle_spawn_definition const *arg_2, s_particle_system_datum const *arg_3);
vector3f *function_143070(matrix3x3 const *arg_1, vector3f const *arg_0, vector3f *arg_2);
void placement_set(s_placement *arg_0, vector3f const *arg_1, point3f const *arg_2, vector3f const *arg_3, short arg_4);
void function_246e60(long arg_0, dword arg_1, s_particle_impact const *arg_2, s_object_246eb0 const *arg_3, long arg_4);

// score 356 (was 356): frame 0xb0, 1643 bytes against 1652. Left: retail saves EBP at entry and
// shares one epilogue (ours saves it after the early exits); retail keeps the blend in XMM2-4
// across the first 0x143070 call (here it goes through the grouped local's memory); retail's
// frame holds probe point f0x14, probe direction f0x20, collision result f0x2c, 16 unreferenced
// bytes at f0x78 and the placement at f0x88; the random-store and getter homes differ.
// Round 137: early exit as one goto to the end 368; probe point/direction/collision declared in
// the collision branch, the blend in its branch and the placement before its NONE store, with
// one sample local (no grouping struct) 469 (frame 0xa0: the blend gets no slot, the placement
// lands below the collision result), the collision result renamed to sort between them 469;
// effect datum pointer local for the flag byte 368; flag byte read as the low byte of the
// flags word 368.
// Earlier: interpolation remainder 415; child-duration default 411; failed-child continue 408;
// overwritten placement default removed 409; definition reads at use 449; entry scalar blend
// components 417; branch-scoped components 417; native kind snapshot 408; inline depth one 408;
// independent random outputs 435; random-word array loop 591; array loop with scalar draw 589;
// position scale loop 418; position-add loop 408; byte guard 408; separate trace point 408;
// separate direction 408; scoped flag 408; nonzero kind first 415; final scalar draw 399; last
// two 409; last three 389; entry record declaration 389; final getter field at use 389; external
// record reference 389; late placement default 389; child enable last 389; native effect
// bit-field, flag-word mask and shift 389/389/389; ordinary result on those native forms
// 436/436/436; native bit-field and word-mask single guards 389/389; scalar draw one/two/three
// 399/386/399 (two alone reaches 1652 bytes, rejected), draws one/two 409, one/three 396,
// one/four 409, two/four 396, one/two/four 415; entry/random-phase sample scopes and sample name
// 389 each; shared real scale 396; child reciprocal before link 383; age before position scale
// 391; separate blend point 447, with shared real scale 431; reciprocal lifetime 383 (a one-use
// read moved ahead of the link call, not kept); child tag/duration/both native values
// 391/390/392; inherited velocity value 380 kept; complete blend products 387; tag/products 392;
// duration/velocity 384; all producers 401; velocity at admission 395; after rotations 382;
// complete velocity arms 411; entry velocity declaration 380; both 411; per-use velocity fields
// 467; separate real spatial objects 433, with velocity arms/entry declaration/per-use fields
// 438/433/487; independent collision/placement/both 377/380/376; first sample alone/with
// collision/with placement/all 429/436/429/436; collision and placement with velocity source 368
// kept; placement/source 389; branch collision/source 372; late placement 399; shared effect
// receiver with velocity source/value 434/435; native flag four 393, with velocity
// pointer/admitted pointer/pointer arms/value arms 394/393/394/410, with complete blend
// products/duration/all-four scalar draws 393/392/428; shared current/prior/both blend-vector
// scales 375/375/368; collision-offset/motion producers 377/369; both blends with
// collision/motion/all 377/370/377; independent blend/probe-point/probe-direction objects
// 428/368/368; blend-point/probe-point, blend-point/direction, both probe objects, all three
// 428/428/368/428; full independent native objects 466; shared point constructor for blend
// 368, collision 371, motion 375, blend/collision 371, blend/motion 375, collision/motion 378,
// all three 378, all plus trace-start 368; guard bit word/long, collision declaration,
// workspace/guard names, combined flags and child while 368 each; common scalar for all four
// samples 423.
// Frame 0xb0 agrees; entry EBP preservation and random/geometry/getter homes remain.
// Frame 0xb0 agrees; late EBP save, escaped blend coordinates and selective getter expansion remain.
// Frame 0xbc/0xb0, entry EBP save and random/geometry/getter scheduling remain.
// Earlier: Frame 0xbc versus 0xb0; entry EBP preservation and random/geometry homes remain.
// Earlier form: exact 0xb0 frame; late EBP save, random stores/getter expansion differ.
// Interpolation remainder 415; child-duration default 411; failed-child continue 408;
// overwritten placement default removed 409; definition reads at use 449.
// frame 0xb0 agrees; EBP is saved after the early exits,
// random scratch stores and interpolation/collision homes differ. Entry scalar
// blend components 417; branch-scoped components 417; native kind snapshot 408;
// inline depth one 408; independent random outputs 435.
// random-word array loop 591; array loop with scalar draw 589;
// position scale loop 418; position-add loop 408; byte guard 408.
// separate trace point 408; separate direction 408; scoped flag 408;
// nonzero kind first 415; final scalar draw 399; last two 409; last three 389.
// Entry record declaration 389; final getter field at use 389; external record
// reference 389; late placement default 389; child enable last 389.
// Native effect bit-field, flag-word mask and shift 389/389/389;
// ordinary result on those native forms 436/436/436; native bit-field and
// word-mask single guards 389/389 (control 389). Entry EBP save remains late;
// original admission guard, qualifier and scalar random lifetimes restored.
// Earlier form: late entry EBP save and random/getter geometry scheduling remain. Fast trials:
// scalar draw one 399; scalar draw two 386; scalar draw three 399; scalar draws one/two 409; scalar
// draws one/three 396; scalar draws one/four 409; scalar draws two/four 396; scalar draws one/two/four
// 415. All restored.
// Draw two alone reaches 1652 bytes at fast 386; three-point reduction rejected.
// Entry/random-phase sample scopes and sample name 389 each; shared real scale 396;
// child reciprocal before link 383; age before position scale 391; separate blend point 447;
// separate blend with shared real scale 431. Reciprocal lifetime plain 383 at 1638/1652 (a one-use read moved ahead of the link call; not kept).
// Child tag/duration/both native values 391/390/392; inherited velocity value 380 kept (plain);
// complete blend products 387; tag/products 392; duration/velocity 384; all producers 401;
// velocity at admission 395; after rotations 382. Frame 0xbc versus 0xb0 for the retained native velocity.
// Complete velocity arms 411; entry velocity declaration 380; both 411; per-use velocity fields 467;
// separate real spatial objects 433; with velocity arms/entry declaration/per-use fields 438/433/487. All restored.
// Independent collision/placement/both 377/380/376; first sample alone/with collision/with placement/all
// 429/436/429/436; collision and placement with velocity source 368; collision/source alone 368
// kept (plain), placement/source 389; branch collision/source 372; late placement 399;
// shared effect receiver with velocity source/value 434/435. Frame 0xb0; body 1643/1652.
// Native flag four 393; with velocity pointer/admitted pointer/pointer arms/value arms
// 394/393/394/410; with complete blend products/duration/all-four scalar draws 393/392/428.
// Native flag declaration and every body form restored.
// Round 230: Shared current/prior/both blend-vector scales 375/375/368; collision-offset/motion
// producers 377/369; both blends with collision/motion/all 377/370/377.
// Round 197: independent blend/probe-point/probe-direction objects 428/368/368;
// blend-point/probe-point, blend-point/direction, both probe objects, all three 428/428/368/428;
// full independent native objects on the recovered collision/velocity-source frame 466. All restored.
// Shared point constructor: blend 368, collision 371, motion 375, blend/collision 371,
// blend/motion 375, collision/motion 378, all three 378, all plus trace-start 368. All restored.
// Guard bit word/long, collision declaration, workspace/guard names, combined flags and
// child while 368 each; common scalar for all four samples 423.
// First native sample on the collision/source frame: entry/producer 426/426;
// with native probe point/direction/both 426/426/426; entry/producer/both probes
// with shared loop lookup 443/443/443. Frame shrinks to 0xac; all restored.
// Round 223: Defining/caller inline with noinline spawn/matrix/placement/spawn+matrix/
// spawn+placement/matrix+placement/all 368/369/368/369/368/369/369; spawn noinline 368.
// Round 226: native signed particle header, unsigned effect/child handles, every pair/all
// three, and all with unsigned allocated handle 368 each. Frame 0xb0 and late EBP save remain.
// Round 32: plain locals for the grouped one 466 (frame 0xa0, the blend has no home); blend through
// an inline interpolation helper 466; a 0x5c-byte collision result (the size 0x0b8ca0 and 0x26bfa0
// give it) 369 alone, 463 with plain locals (frame 0xb0, but collision above the placement);
// placement renamed 463; trace locals in the trace block 466. All restored.
// Round 244: direct existing real producer assigned at original draws 1/2/3/4/last-three/
// all/first-third/second-fourth 368/368/368/368/368/368/368/368. Original output assignments
// and reference helper restored; late EBP save, first-sample store and geometry homes remain.
// Round 218: Shared vector sums blend/velocity/both/motion/blend-motion/velocity-motion/all
// 368/368/368/356/356/363/363; all plus trace-vector setter 366. Motion helper alone
// 356 kept plain; frame 0xb0 and 1643/1652 bytes remain, with late EBP save and random/blend/getter homes.
// Round 255: returned position/direction receivers 379/374; contact add-scale 356; both
// receivers 380; position/contact 378, direction/contact 371, both/contact 380;
// all plus shared blend addition 380. Original receivers and collision arithmetic restored.
// Round 222: Native initial-position/both-blend-source/both/normal/initial-normal/blend-normal/all inputs 396/420/478/372/412/432/478; all with point constructors 396. Every input snapshot and constructor restored; late EBP save and random/blend/getter homes remain.
// Round 224: Probe/normal/both memcpy, collision scaled sum, probe+sum/normal+sum/all 356 each;
// both memmove with the sum 390. Original copies, helper graph and complete body restored.
// Round 224 continuation: Existing allocator/placement ordinary-inline-noinline declaration pairs, eight non-original forms
// 356/356/409/409/409/356/356/356. Original defining and forward declarations, shared header and
// whole body restored; early EBP save and random/geometry/getter homes remain.
// Round 71: the 9 missing bytes are four forms retail keeps unfolded (effect record `add eax, edx`,
// the cache-flag and/store, `mov ax; test ax, ax` on the definition short, `add eax, edx; add eax,
// 0x30`). Cache test as `local_2 != arg_0->field_50` 355; field_50 stored before the field_44 clear
// 368 (retail's load/and/store), both 367 (both exact, later registers move); flag bit 4 through a
// word read 374 (retail's `shr al, 4; test al, 1`, but the definition is reloaded from the stack);
// all three 364 (1659 bytes). /G5, /G6 and /G7 compile-only all fold as ours does.
// System-getter/cache ordinary-inline-noinline declaration pairs, eight non-original
// forms, 356 each; getter-inline pairs lose a reduced matched caller. All declarations restored.
// Round 383: coherent live particle/live descriptor/retained allocation pool 709/367/359;
// particle+descriptor/particle+pool/descriptor+pool/all/all with live settings
// 731/752/367/755/807. Original record observations, helper graph and complete body restored.
// Existing cache update method 356; with initial shared scale/blend sum/loop lookup
// 376/356/376; scale+lookup/scale+blend/lookup+blend/all 397/376/376/398. All restored.
// Round 264: collision/terminal-dispatch ordinary-inline-noinline caller declaration
// pairs, eight combinations 356 each; consistent dispatcher inline/noinline in its
// defining unit and every forward declaration 423/356. Every declaration and body
// restored; late EBP preservation, random/blend/getter homes remain.
// Final-dispatch tag/handle/flags entry declarations, all seven subsets, and all three with
// descriptor entry declaration 356 each; original declaration phases and producers restored.
// Round 186: Independent native blend/probe/placement consumer-scope masks 0-7 on the retained
// motion graph 465/465/466/466/465/465/466/466. Original grouped workspace, native collision, source
// velocity and complete body restored.
// Round 188: joint reciprocal result stages, ternary/explicit/default-update pairs
// (1,0)/(2,0)/(0,1)/(0,2)/(1,1)/(1,2)/(2,1)/(2,2):
// 356/357/356/351/356/351/357/353. Five-point nonmatch leads rejected; original results restored.
// Round 403: entry previous-weight/age-step/motion-coefficient producers, masks 1-7,
// 429/447/451/449/449/448/449; all with combined header assignment 449.
// Original parameter producers, motion helper and complete body restored; late EBP remains.
// Round 293: Uninitialized child-handle/record-owner/reciprocal declarations before the loop, all seven subsets, and all at function entry: 356 each. All original producer phases, signed pretest, link calls, reciprocal expressions and complete body restored.
// Round 429: One native point for blend then probe start, with entry/consumer direction,
// placement and sample declarations: all eight 416 (frame 0xa0). Original body restored.
// Round 232: Rate read once, pending mask, both, named age increment, rate+age, mask+age, all, then all with allocation-failure exit
// 360, 359, 360, 359, 360, 359, 357, 357. Original executable source restored.
// Round 462: Complete initial scaled-position point-set/vector-scale producers, aggregate/setter
// publication and original/early age: 393/393/358/363/411/397/376/377. All original geometry,
// stores, helper graph and includes restored; frame and late EBP save remain.
// @retail 0x247950
void function_247950(s_particle_system_datum *arg_2, s_particle_spawn_definition const *arg_3,
	dword arg_4, real arg_5, real arg_6, real arg_7, s_247950 *arg_1, s_247fd0 *arg_0)
{
	struct s_247961
	{
		point3f field_0;
		point3f field_c;
		vector3f field_18;
		s_placement field_70;
		real field_98;
	} local_23;
	s_collision_result_1697c0 collision;
	volatile bool local_18;
	long local_0 = arg_2->effect_index;
	if (local_0 != NONE)
	{
		byte local_24 = *(byte const *)((byte const *)DATUM(g_4ea93c, s_effect_datum, local_0) + 2);
		local_24 >>= 3;
		local_24 &= 1;
		if (local_18 = local_24 != 0)
			return;
	}
	long local_1 = record_pool_allocate(g_51ec84);
	if (local_1 != NONE)
	{
		s_248000 *local_2 = DATUM(g_51ec84, s_248000, local_1);
		s_effect_particle_system_definition *local_3 = arg_2->function_1751d0();
		local_2->field_04 = arg_1->field_4;
		arg_1->field_4 = local_1;
		local_2->field_02 &= 0xe004;
		local_2->field_02 |= 4;
		local_2->field_02_13 = (_random(&g_4e7408->seed, 0, 0) * 5) >> 16;
		local_2->field_08 = 0.f;
		function_248076(&g_4e7408->seed, local_23.field_98);
		local_2->field_14 = (word)real_truncate(local_23.field_98 * 65535.f);
		real last_sample;
		function_248076(&g_4e7408->seed, last_sample);
		local_2->field_16 = (word)real_truncate(last_sample * 65535.f);
		function_248076(&g_4e7408->seed, last_sample);
		local_2->field_18 = (word)real_truncate(last_sample * 65535.f);
		function_248076(&g_4e7408->seed, last_sample);
		local_2->field_1a = (word)real_truncate(last_sample * 65535.f);
		local_2->field_10 = arg_2->unknown04;
		if (arg_0->field_50 != local_2)
		{
			arg_0->field_44 &= ~0xf80f;
			arg_0->field_50 = local_2;
		}
		function_173ba0(~arg_0->field_44 & 0xf80f, arg_0->field_48, arg_0->field_4c, arg_0->field_50, arg_0->field_00);
		arg_0->field_44 |= 0xf80f;
		function_249170(arg_0->field_00, (s_particle_spawn_state *)local_2, arg_3, arg_2);
		local_2->field_1c.x = arg_7 * local_2->field_1c.x;
		local_2->field_1c.y = arg_7 * local_2->field_1c.y;
		local_2->field_1c.z = arg_7 * local_2->field_1c.z;
		local_2->field_08 += arg_5 * arg_6 * local_2->field_0c;
		if (local_3->unknown0c == 0)
		{
			vector3f const *velocity = local_0 != NONE ? &DATUM(g_4ea93c, s_effect_datum, arg_2->effect_index)->velocity : g_4687a4;
			local_23.field_0.x = arg_1->field_34.x * arg_5 + arg_1->field_40.x * (1.f - arg_5);
			local_23.field_0.y = arg_1->field_40.y * (1.f - arg_5) + arg_1->field_34.y * arg_5;
			local_23.field_0.z = arg_1->field_40.z * (1.f - arg_5) + arg_1->field_34.z * arg_5;
			function_143070(&arg_1->field_10, (vector3f *)&local_2->field_1c, (vector3f *)&local_2->field_1c);
			local_2->field_1c.x = local_23.field_0.x + local_2->field_1c.x;
			local_2->field_1c.y = local_23.field_0.y + local_2->field_1c.y;
			local_2->field_1c.z = local_23.field_0.z + local_2->field_1c.z;
			function_143070(&arg_1->field_10, &local_2->field_28, &local_2->field_28);
			if (*(long *)((byte const *)arg_3 + 0x78) == 8)
			{
				local_23.field_c = local_2->field_1c;
				local_23.field_18.i = 0.f;
				local_23.field_18.j = 0.f;
				local_23.field_18.k = -1.f;
				local_23.field_c.z += 1.f;
				collision.unknown24 = NONE;
				if (function_1697c0(0x800005, &local_23.field_c, &local_23.field_18, NONE, NONE, &collision))
				{
					vector3f const *local_10 = (vector3f const *)((byte const *)&collision + 0x28);
					local_2->field_1c.x = local_10->i * 0.005f + collision.point.x;
					local_2->field_1c.y = local_10->j * 0.005f + collision.point.y;
					local_2->field_1c.z = local_10->k * 0.005f + collision.point.z;
					local_2->field_02 |= 0xa;
					local_2->field_28 = *local_10;
				}
				else
					local_2->field_02 |= 1;
			}
			if ((bool)((*(byte const *)((byte const *)local_3 + 0x16) >> 4) & 1))
			{
				local_2->field_28.i += velocity->i;
				local_2->field_28.j = velocity->j + local_2->field_28.j;
				local_2->field_28.k = velocity->k + local_2->field_28.k;
			}
		}
		else
		{
			real local_11 = (1.f - arg_5) * arg_6;
			vector3f_add_scaled((vector3f const *)&local_2->field_1c, &local_2->field_28, local_11, (vector3f *)&local_2->field_1c);
		}
		local_2->field_3c = arg_4;
		++arg_1->field_2;
		local_23.field_70.value24 = NONE;
		c_247951 *local_12 = (c_247951 *)function_137bd0(local_3->tag_index);
		for (long local_13 = 0; local_13 < local_12->function_247958(); ++local_13)
		{
			long local_14 = function_173fd0(local_12->function_247959((word)local_13), arg_2->effect_index, local_3->tag_index, local_13, NONE);
			local_12 = (c_247951 *)function_137bd0(local_3->tag_index);
			if (local_14 != NONE)
			{
				s_particle_system_datum *local_15 = DATUM(g_510c74, s_particle_system_datum, local_14);
				particle_system_link(local_15, &arg_2->first_child_index, &arg_2->last_child_index);
				local_15->parent = arg_2;
				local_15->unknown4c = local_1;
				real local_16 = local_2->field_0c > 0.f ? 1.f / local_2->field_0c : 0.f;
				local_15->flag0 = true;
				local_15->unknown04 = 0.f;
				local_15->unknown08 = local_16 > 0.f ? 1.f / local_16 : 1.f;
			}
		}
		if (local_12->function_247957() != NONE)
		{
			placement_set(&local_23.field_70, &local_2->field_28, &local_2->field_1c, g_4687b0, 0);
			local_3 = arg_2->function_1751d0();
			long local_19 = local_3->tag_index;
			long local_20 = local_12->function_247957();
			dword local_21 = local_12->function_247956();
			function_246e60(local_20, local_21, (s_particle_impact const *)&local_23.field_70, (s_object_246eb0 const *)local_2, local_19);
		}
	}
}
