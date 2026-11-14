// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1D6560.CPP: per-object physics setup: which entries of an object's
   physics model hold (judged by its damage sections), and the layer each of
   its rigid bodies collides in */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"

/* a damage section an entry depends on (0xc bytes) */
struct s_physics_entry_section
{
	short key_a;
	short key_b;
	byte flags;
	byte unknown05[0xc - 5];
};

/* an entry of the physics model (0x18 bytes) */
struct s_physics_entry
{
	byte unknown00[8];
	long section_count;
	s_physics_entry_section *sections;
	byte unknown10[0x18 - 0x10];
};

/* a rigid body of the physics model (0x90 bytes) */
struct s_physics_rigid_body
{
	byte unknown00[0x18];
	byte flags;
	byte unknown19[0x90 - 0x19];
};

struct s_physics_model
{
	byte unknown00[0x30];
	long entry_count;
	s_physics_entry *entries;
	byte unknown38[4];
	s_physics_rigid_body *rigid_bodies;
};

struct s_physics_model_owner
{
	byte unknown00[0x48];
	s_physics_model *model;
};

struct s_physics_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

bool function_db540(long object_index, long key_a, long key_b);

/* sets the bit of each entry that has no sections, or one of whose flagged
   sections is gone; clears the others */
PRIVATE __forceinline bool function_1d6561(s_havok_component const *arg_0, s_physics_entry_section const *arg_1)
{
    long local_0 = *(volatile long const *)&arg_0->object_index;
    return function_db540(local_0, arg_1->key_a, arg_1->key_b);
}

// @retail 0x1d6560
void __stdcall function_1d6560(s_havok_component *component, s_physics_model_owner *owner, dword *mask)
{
	long i;

	for (i = 0; i < owner->model->entry_count; i++)
	{
		s_physics_entry *entry = &owner->model->entries[i];

		mask[i >> 5] &= ~(1 << (i & 31));
		if (entry->section_count == 0)
		{
			mask[i >> 5] |= 1 << (i & 31);
		}
		else
		{
			long j;

			for (j = 0; j < entry->section_count; j++)
			{
				s_physics_entry_section *section = &entry->sections[j];

				if ((section->flags & 1) && !function_1d6561(component, section))
				{
					mask[i >> 5] |= 1 << (i & 31);
					break;
				}
			}
		}
	}
}

/* the collision layer of one of the object's rigid bodies, by object type */
// MiniMax G (11-14): no new form kept, no lower score (96): a single type read shared by both kind routes reaches 68
// only with a one-use absolute-index local and a score-picked declaration order, and the one-use-free form scores 98
// and demotes the matched caller 0x1d5bf0 to 213; retail keeps the object index in ecx (re-masking it in case 0) and
// reads the type once.
// score 96 (was 96): no local frame; retail reads the type once before the kind test (one type read shared by
// both kind routes, and case 0 through the same header data) and keeps the body array/body in EBP/ESI. Current
// component EAX, rigid index ECX, kind EDI and ret 8 agree with the matched caller. A single type read must
// preserve that incoming convention; every prior one-read type form flips the component/index pair (the caller
// 0x1d5bf0 then differs). Type read after the rigid body 94 at 256 bytes (caller 13), first
// 117, after headers/body/index 94 (caller 13), body then type 136, kind 7 as a ternary 104;
// case 0 through the object_index local 91 at 256 bytes (caller 146), with the one-read type
// 132-133; parameters reordered (rigid index first, component last) 96 / 94 (caller 13).
// MiniMax B (11-12): no new form kept, no lower score (96, was 96): case 0 through the object_index local (the object
// read through the index already in hand instead of a second read of the component) gives 91 at 256 bytes, but it
// moves the component off the register the caller passes it in, and the caller 0x1d5bf0 then differs at 146 (it
// matches with this body), so the original body is kept. The one type read at the entry (a byte local) gives 117 at
// 236 bytes; the header array local gives 92; the headers, rigid body and object index declared in that order gives
// 92 at 256 bytes. Retail's own convention is component EAX, rigid index ECX, kind EDI, owner and level on the stack.
// MiniMax I (11-09): no new form, no lower score; retail takes the owner and the level on the stack and returns with `ret 8`, ours keeps the owner in ECX and returns with `ret 4`, so the body cannot match while the caller keeps its matched form: BLOCKED-BY the caller 0x1d5bf0 (src/unknown_1d5bf0.cpp).
// MiniMax R (11-09): no new form, no lower score (96); retail reads the object type once at the entry and dispatches it through a byte-indexed table, ours reads the type again in each branch, and the one-read forms move the caller's argument setup.
// MiniMax A (11-06): no new form, no lower score; retail loads the object-header pool and its data at the
// entry, keeps the level in EBX, the component in EBP and the rigid body in ESI (three pushes against our
// one) and reads the type byte once into EAX before the kind test, using it for both routes (the kind
// route as a decrement and a test, the switch as a table dispatch); ours reads the type in each branch.
// The recorded one-read forms (111 and 127) move the caller's argument setup and leave the matched caller
// 0x1d5bf0 at 194 and 209.
// MiniMax A (11-06): no new form, no lower score; retail reads the object type once before the kind test and uses
// it for both kind routes, ours reads it in each branch; the one-read forms give 127 (a byte type at the entry),
// 111 (the header array, the object index and a byte type) and 111 (the component read twice) with the matched
// caller 0x1d5bf0 at 194, 209 and 209, so the one-read shape is not eligible while the caller is matched.
// MiniMax H (11-06): no new form, no lower score; the incoming register pair is fixed by the matched caller
// 0x1d5bf0 (component in eax, rigid body index in ecx, kind in edi, ret 8), and every body that changes that
// promotion flips the pair and leaves the caller differing, so the type read and the kind 7 route stay where
// they are: the type hoisted into a byte local 94 with the caller at 13, the data pointer in a local 97 with the
// caller unchanged, the data local declared first 117 with the caller at 160, the type read through the header
// object in case 0 129 with the caller at 213, direct returns instead of the result local 98 with the caller
// unchanged, the object index read again for case 0 91 with the caller at 146, and the flags read through the
// owner chain without a pointer local 117 with the caller at 122.
// MiniMax E (11-05): no new form, no lower score: the 24 form needs the object index and the absolute index, each used once (recorded not kept); the header array local with the type hoisted and case 0 through the object index 96 with the caller 0x1d5bf0 at 13.
// Earlier 96 (was 89): retail pushes ebp and holds the base pointer there, ours keeps the record in esi and ecx;
// the whole register allocation differs.
// Round 183: shared header getter at both dispatches 96; shared byte/long type after body 94/96; early byte 106; direct-component long 105; header/typed object lookup 136/136; shared multi-use handle 105. Single-type forms lose 1d5bf0; all restored.
// MiniMax R: both flag results computed as ((flags & 8) | 0x30) >> 2 and >> 3 instead of branching: 95 (not kept).
// Round 200: byte/char/short/word layer result 92/91/119/119; with one direct full-width type
// 141/141/121/121, all four lose the matched caller. Original layer and dispatch restored.
// Round r4 (lane 17): one header pointer for type and object 132 (220 bytes, kind moves to ESI, caller 213); header
// array local with one object_index read and one type read 124 (228, component on the stack, caller 209); one type
// read keeping the component read in case 0 94 (256, component/index swap EAX/ECX, caller 13), the same with result
// declared first 94 (caller 13); header array local with the component read at both uses 127 (224, caller 209).
// The register parameters follow how often the body reads component: one read puts it on the stack.
// Round 320: both rigid-body flags read live through the model; repeated/shared byte type
// and original/explicit object-bit result arms 117/111/114/117; explicit level tests
// 117/111/114/117. Every form loses the matched caller; complete original body restored.
// Round 457: selected-body const reference with repeated/shared type, original/explicit
// object-bit arms and original/explicit upper-level arms: 96/136/93/136/96/136/93/136.
// Shared-type forms lose the matched caller; the three-point forms are below the keep gate.
// Original body and caller convention restored.
// Round 10 (lane 17): a /Gd first-pass harness loads the rigid-body index into EAX for every one-read-type form
// (208 orderings, byte/long type, case 0 through the local or the component, header local), into ECX only with the
// two reads, which is the flip. One read with case 0 through the object_index local 136 (216 bytes, caller 213); the
// kind-7 test as a plain if 100 (260 bytes, sbb select); parameters (component, owner, level, kind, index) as the
// caller's setup order implies 96 (same code, caller kept), with the one-read body 136, with a single-exit kind-7
// path 100 (caller 13).
// Round 46 (lane 1): parameters in retail's register order (component, index, kind, owner, level) 96 (same
// code, caller kept), with the one-read type after the body 94 (caller 13), with case 0 also through the
// object_index local 133 (220 bytes, caller 213), with case 0 through the header data at that index 133
// (caller 213). A /Gd screen of 72 one-read forms (type/body/result orders, byte/long type, case 0 through the
// component or the local, switch/result kind-7 path, header pointer) and two with the body formed at its uses
// loads the index into EAX in every one. Retail loads the header data first and keeps it live into case 0
// (EDX), which costs EBP for the body array; no form here keeps it.
// Round 56 (lane 16): a /Gd compile-only screen predicts the register parameters (it agrees with the build for the
// two-read body, the one-read flip and the form below). Of ~150 one-read forms (orders of object_index, type,
// rigid body and result; long/byte/short/const type; case 0 through the component or object_index; a header
// pointer; a split rigid-body pointer; the kind-7 test as a switch, switch with default, single-exit switch,
// ternary or if/return), only the if/ternary kind-7 tests keep component EAX / index ECX, and they compile to
// `neg; sbb; and; add` where retail has the switch's `dec eax; je`. object_index, type, then rigid body with the
// kind-7 ternary 96 at 256 bytes with the caller still matching (retail's three pushes); with case 0 through
// object_index 141 (212 bytes, caller 213); one-read switch form 96 (256, caller 13); one header pointer read by
// Earlier: one type read shared by both kind routes and case 0 through the same header data in retail.
// one type read shared by both kind routes and case 0 through the same header data in retail.
// both switches 125 (284 bytes, caller 194).
// Round 63 (lane 12, fast): one type read with case 0 through component (retail's unshared index arithmetic, which
// a case 0 through the object_index local loses to CSE) 94 (caller 13); the type read indexed by `(word)object_index`
// 98 (caller 13, movzx); the kind-7 test as `switch (kind) { case 7: ... }` 89 (caller 13); the current body with
// case 0 through the object_index local 91 (caller 146); the type read straight from component, long 105 (caller 9,
// index arrives in EDX). An env link with 0x1d5940/0x1d5bf0 (exact for the current body): the type through a
// forceinline getter in both branches, or straight from component in both, never hoists the two reads (268/264
// bytes); every order of the object_index/rigid-body/type/result declarations with byte or long type keeps the flip.
// Round 31 (lane 15, fast; o/t/r = object_index, type, rigid body in declaration order, caller in brackets):
// type through the byte-array header idiom o,t,r 105 [9], r,o,t 137, o,r,t 136, byte 117/132; o,t,r with case 0
// through the component 124 at 276 [194] (retail's entry and case 0 exactly, but the type takes EDI and kind goes
// to the stack); on that form the body array first 118, the model first 122, a byte shift in case 0 124, int type
// 124, result first 124, the rigid body at each use 97 at 260 [122], the rigid body after the kind-7 path 103.
// Round 32 (m3 lane 2, fast, caller 0x1d5bf0 in brackets): one byte type read with the kind test inverted (main
// switch first, the kind-7 switch in the else) 89 at 256 bytes [13] (component ECX, index EAX); the header array read
// first and shared by the type and case 0 124 at 228 [209]; one read with the kind-7 path as an if/else single exit
// 104 at 240 [13] (sbb select); the two-read body with the kind test inverted 103 at 268; one read for the main switch
// with the kind-7 switch re-reading the header expression 94 at 256 [13] (CSE'd into the one-read form).
// Round 33 (lane 15, fast): o,t,r with the kind-7 ternary re-measured 96 at 256 bytes [0] (rigid body computed
// in place in ECX where retail copies the index to ESI, header data in ESI for retail's EDX, branchless kind 7);
// on it the kind-7 path as `if (type == 1) return 0xc; return 1;` 96 (folded to the same select), case 0 as
// `(byte >> 2) & 1` through the component 96 (still `shr eax, 2`), through object_index 141 [213], the kind-7
// path as a one-case switch 124 [194]; o,r,t 105 and r,o,t 107 (240 bytes [13]).
// Round 35 (lane 15, fast), on o,t,r with the kind-7 switch and case 0 through the component (124 at 276 [194]:
// entry and case 0 as retail, but the type takes EDI and kind drops to the stack): type renamed object_type /
// type_value, rigid_body renamed body 124/124/124; kind, level or component renamed 124/124/124 (names order no
// register); the main switch inside `if (kind != 7)` with the kind-7 switch after it 115 at 272 [194]; per-case
// returns with no result 124. On this body: case 0 as `(byte >> 2) & 1` or `(byte)(byte >> 2) & 1` 96/96 [0].
// Round 64 (lane 16, fast; caller 0x1d5bf0 in brackets): one type read as a byte 94 [13], as a long 96 [13];
// on the one-read body the owner read once through `&owner` 96 [13], the level through `&level` 96 [13], the kind
// through `&kind` 65 at 260 bytes [194: kind on the stack, ret 0xc], the rigid body as `base + index * sizeof`
// byte arithmetic 96 [13]. Every one-read body takes component in ECX and the index in EAX.
// Round 39 (lane 15, fast): one type read with a `dword` object_index 94 at 256 [13] (component ECX, index EAX);
// one read with the kind-7 switch listing cases 0/7/11 and an `__assume(0)` default 121 (284) [13]; two reads with
// the main switch's through `component->object_index` 91 at 256 [146] (component on the stack, kind EDX), with
// the kind-7 read through it instead 96 [0] (same code); one read with the body array named first 94 [13].
// DL226: One completed native layer result across both kind routes, byte/char/short/word,
// kind-arm defaults 97/99/125/125 and entry defaults 97/97/122/122. All lose 0x1d5bf0;
// original result flow, repeated type observations and incoming convention restored.
// Round 240: Primary body result default, crossed with secondary body default, one byte type
// and native object-byte shift: 100/108/91/91/100/108/91/91. Original body restored;
// shared-type forms lose the matched caller (13); the five-point gain is below the keep gate.
// Round 243: shared primary/secondary flag-selection cases, main type byte/short/long/dword
// 132/136/128/128; the group after the object case 131/136/126/126. All grow and lose
// the matched caller (13). Original separate flag cases, both type reads and body restored.
// Round 504: actual byte flag3 with repeated/shared type and primary/secondary
// result defaults 96/94/100/91/104/91/108/91. Shared-type forms lose 1d5bf0 (13);
// original byte layout, both type reads, result arms and complete body restored.
// Round 118 (one-file harness, plain fast where marked; caller 0x1d5bf0 tracked): a /Gd compile-only screen of 1200
// statement orders predicts the register parameters; of 72 forms that load the component in EAX and the index in ECX
// the best is `long object_index = component->object_index; long absolute_index = object_index & 0xffff;` then the
// rigid body, a `headers` local and a byte type, with case 0 reading `headers[component->object_index & 0xffff]`:
// 28 (fast; 256 B, caller matches, jump table and all case bodies retail's). Not kept: the field local is used once.
// Left in it: rigid body ECX for ESI, object index EDX for ECX, data ESI for EDX, and `shr eax, 2` for `shr al, 2`.
// On it: `absolute_index` from the field directly 102, case 0 through the object_index local 102, a long type 97.
// `object_index`, `headers`, `absolute_index`, rigid body, byte type with case 0 through `headers[object_index & 0xffff]`
// or the object getter 68 (fast; 256 B, caller matches), not kept: only one of the two lookups uses `absolute_index`
// (or `headers` is used once); `absolute_index` at both lookups, fast with the caller in brackets: object_index,
// headers, absolute 99 [9], the default-case kind 7 switch 99 [9], long type 64 [9]; object_index, absolute, headers,
// rigid body 101 [9]; object_index, headers, rigid body, absolute 128 [213]; object_index, absolute, rigid body,
// headers 129 [213]; headers first 101 [11]. `headers` first with one component read 35 (harness; caller lost, all 60
// parameter orders the same code); rigid body as `bodies + index`,
// `(byte *)` arithmetic either way round or from a copied index 38
// (same code); index modified in place 96; byte-shift spellings of the case 0 flag 38 (same code), the mask 57; inline
// getters for the rigid body and the type 126 (224 B, caller lost).
// Round 333: Shared twice-used body flag byte/predicate before or after kind policy,
// with repeated/shared type: 101/97/106/115/92/92/91/83. Single-type and early flag
// forms lose the matched caller; original observations, body and convention restored.
// F344: Actual object-getter acquisition before handle/body/result/main dispatch,
// repeated type 125/72/95/83; shared byte type 129/88/81/92. NOT kept:
// one-use getter pointer and early object-pointer read; several forms also lose
// the matched caller. Original body, both observations and caller convention restored.
// B388: Case-zero decoded-bit bool/byte/short/long producers, low/high result defaults,
// 106/106/106/106/106/106/106/106 (276 bytes), caller 13. Original bit expression,
// repeated dispatch, body acquisition and caller-compatible convention restored.
// DH314: Cached model/body-array producer roots, native/readonly, repeated type:
// 117/117/113/113; shared byte type: 104/104/98/98. All lose the matched caller
// (215 or 14); original entry acquisition, selected body, type observations and body restored.
// m3 lane 14 r56 (fast, caller in brackets), on round 118's 28 form (re-measured 28 [0]): a long type 89 [0];
// headers declared first 83 [2]; the type read before the rigid body 111 (268 B) [9]; case 0 through
// `headers[object_index & 0xffff]` 101 [0]; `long result` declared first 28 (same code); the kind-7 path as
// `if (type == 1) return 0xc;` 94 [0]; the rigid body read at each use 97 (260 B) [122].
// DH317: Borrowed native type-byte/header-record dispatch bindings, const/mutable,
// after handle/body: 122/122/94/94/125/125/94/94. NOT kept: every form loses
// matched 0x1d5bf0 (194 or 13); the 94 forms also gain only two. Original body,
// both type observations and incoming convention restored.
// Round 65 (m3 lane 11, fast; caller 0x1d5bf0 in brackets): round 118's 28 form rebuilt 28 [0]; 57 random declaration
// orders, byte/long/dword/char/word type and three case-0 lookups on it: headers, object_index, rigid body, byte type
// with the index masked at the type read 25 [13] (left: rigid body ECX for ESI, object index EDX for ECX, data ESI for
// EDX; the index still arrives in EAX), `char` type 28 [0], others 64-137. On the 28 form: case 0 byte-typed shift 28,
// the body array named first 96, a header element pointer 123 at 272 [194], the kind-7 switch with a default return 28,
// the case 0 object through a pointer in its own block 28.
// Retail's `xor eax, eax; mov al; shr al, 2; and eax, 1` comes from none of 27 compile-only spellings (byte and dword
// bitfields, byte/bool locals, byte results); only 0x9d250's `not al` form keeps `shr al` next to `and eax, 1`.
// DL256: Full type-first dispatch with result-first level/body/object policy groups, masks 0-7:
// 121/117/123/121/96/97/112/97 (232/236/228/232/256/256/240/256 bytes). All move the
// component to the stack, use ret 4 and lose the matched caller to 213; 0x1d5940 rises 9 to 116.
// NOT kept: ingress/caller regressions; native-length forms do not improve score. Original full body restored.
// B401: Level-layer preclassification before handle/after selected body, byte/short/long/dword:
// 105/107/107/107/93/128/125/125; caller 6/6/6/6/6/194/194/194. NOT kept: unmatched
// one-use outcomes and every caller loss; three-point reduction also below gate. Full body restored.
// B403: Independent level/primary/object/secondary case returns and level-primary,
// level-object, level-secondary, primary-secondary return pairs: 96 each. Original
// result joins and full body restored; caller and all file controls retain their scores.
// Round 60 (m3 lane 13, fast; caller 0x1d5bf0 in brackets): case 0 as 0x9d250 reads the bit, through the existing
// word bitfield `TEST_FIELD_BIT(((s_object_view *)...)->flag2) ? 11 : 9` (unknown_0a58d0.h), gives retail's exact
// `xor eax, eax; mov al; shr al, 2; ... and eax, 1; shl eax, 1; or eax, 9`: on the current body 96 [0]; on round 118's
// form (object_index, absolute_index, rigid body, headers, byte type; case 0 through headers[component->object_index &
// 0xffff]) 24 at 256 B [0], only the opening left: retail loads the header array first into EDX, moves the index to
// ESI for the rigid body and keeps the object index in ECX; ours gives the object index EDX, the headers ESI and the
// rigid body ECX, and loads the type `mov al` then `movzx` (not kept: the object_index local is used once). On 24:
// headers first 83 [2] (the header load takes EAX, so the component arrives in EDX), headers second 83, rigid body
// after absolute_index 88, after the type 111 (268 B) [9], before absolute_index 94 [13]; long type 89, char type 24;
// no object_index local 101, case 0 through the object_index local 101, `(word)` absolute index 98 [208]; case 0
// through havok_object_get 24; the existing object_header_get/object_get/havok_object_get getters for the type and
// case 0 (with/without the object_index local, either order) 94-136 [13-213]. The other declaration orders of the
// 24 form (o object_index, h headers, r rigid body, a absolute_index, t type): o,h,r,a,t and h,o,r,a,t 21 [13] (the
// component arrives in ECX and the index in EAX), o,r,h,a,t 94 [13], h,r,o,a,t 94 [13], r first (r,o,a,h,t /
// r,o,h,a,t / r,h,o,a,t) 96 [13], o,h,a,t,r 106 (264 B) [9], h,o,a,t,r 106 (264 B) [11].
// N369: Native handle-field references readonly/mutable, repeated/shared type,
// direct/captured header base: 97/97/136/136/93/93/95/95. NOT kept: every form loses
// the matched caller (146/213/13); best gain three is below the gate.
// Original handle capture, dispatch observations and complete body restored.
// DH373: Original-site secondary-kind equality/inequality outcomes in bool/byte/short/long:
// eight at 96; matched caller and file controls unchanged. NOT kept: no native entry repair
// or equality; original repeated dispatch, body flags, result and full convention restored.
// M3 lane 4 r164 (fast, caller in brackets): round 60's 24 form rebuilt 24 [0]; headers, object index, rigid body, type through the
// object index with case 0 through it 124 (228 B) [209]; the 21 form (headers, object index, rigid body, absolute index, type) in 8
// parameter orders, definition, declaration and call together, 21 [13] each (identical code: the component stays in EDX and the
// index in EAX); a named object index for case 0 with the type read through the field, byte type in six declaration orders 99/102
// [13], long type 98 [171] or 112 (236 B) [13].
// AC559: Original primary-kind-seven equality/inequality outcomes, bool/byte/short/long:
// 109/109/102/102/109/109/102/102. NOT kept: all regress and grow to 272/276
// versus native 256; original dispatch, body acquisition and caller convention restored.
// Round 153 (fast, caller 0x1d5bf0 in brackets): round 60's 24 form rebuilt 24 [0]; on it the header-array local renamed to sort after the
// object index 24 (same code); the type local as short 35 (260 B) [0], word 35 (260 B) [0], dword 89 [0]; the header-array local as a pointer to
// const 24 (same code). The 21 form re-measured 21 [13]: it has the 24 form's colouring (headers ESI, object index EDX, rigid body ECX against
// retail's EDX, ECX, ESI) and only flips the incoming pair. The fast check shows 0x1d5940 at 221 for every body, the current one included
// (a reduced-image artifact; 9 in the full build).
// Round 141 (m3 lane 7; one-file LTCG harness with 0x1d5bf0 and its callee files, harness counts, the caller's aligned count in
// brackets, [2] = matched): round 60's 24 form 34 [2], the 21 form 31 [15]; the rigid body through a forceinline (model, index) or
// (owner, index) helper in nine statement orders, same code as without it (31-96); the type as long/int/dword/unsigned long on the
// 24 form 97 (retail's `movzx` load, scheduled after the kind test), short/word 44 at 260; rigid body first 96 (component on the
// stack); 96 random forms (orders, long/dword/no object index, absolute local or not, byte/long type, case 0 through the component,
// the object index or the absolute index, kind-7 switch with or without a default, `&[]` or `+` rigid body): best 31 [15], best
// with the caller kept 75 [2]. Fast, caller in brackets: the 24 form with the rigid body through an (owner, index) helper 24 [0] (same
// code), with `+` for the rigid body and a default in the kind-7 switch 24 [0]; no object index local, absolute index, headers,
// `+` rigid body, byte type, case 0 through the component, kind-7 default 68 [0]; object index, absolute, headers, rigid body, long
// type, case 0 through headers[absolute] 68 [0]; the same with a dword object index, long type, case 0 through the absolute index 67 [9].
// M3 lane 14 r74 (fast, caller 0x1d5bf0 in brackets, 0 = still matched): round 60's 24 form rebuilt 24 [0], with a long type 89 [0]. A grid of
// declaration orders (o object_index, a absolute_index, r rigid body, h headers, t type through absolute_index, T type through `object_index & 0xffff`
// with no absolute_index), type long or byte, and case 0 through hc `headers[component->object_index & 0xffff]`, ho `headers[object_index & 0xffff]`,
// ha `headers[absolute_index]` or gc havok_object_get(component->object_index), all with the flag2 bitfield read and one type read (94 builds):
// hroat hc/ho/ha/gc: long 93 [13], 98 [171], 133 [209], 93 [13]; byte 94 [13], 99 [13], 127 [209], 94 [13]
// hoart hc/ho/ha/gc: long 91 [2], 93 [160], 67 [11], 91 [2]; byte 83 [2], 72 [2], 102 [11], 83 [2]
// ohrat hc/ho/ha/gc: long 86 [13], 102 [0], 133 [213], 86 [13]; byte 21 [13], 68 [0], 128 [213], 21 [13]
// ohart hc/ho/ha/gc: long 91 [0], 102 [0], 65 [9], 91 [0]; byte 83 [0], 68 [0], 100 [9], 83 [0]
// orhat hc/ho/ha/gc: long 96 [13], 108 [0], 136 [213], 96 [13]; byte 94 [13], 101 [0], 133 [213], 94 [13]
// roaht hc/ho/ha/gc: long 95 [13], 108 [13], 137 [213], 95 [13]; byte 96 [13], 101 [13], 132 [213], 96 [13]
// horat hc/ho/ha/gc: long 86 [13], 92 [160], 127 [209], 86 [13]; byte 21 [13], 72 [2], 124 [209], 21 [13]
// rhoat hc/ho/ha/gc: long 98 [13], 112 [13], 137 [213], 98 [13]; byte 96 [13], 102 [13], 132 [213], 96 [13]
// hroT hc/ho/gc: long 93 [13], 133 [209], 93 [13]; byte 94 [13], 127 [209], 94 [13]
// ohrT hc/ho/gc: long 86 [13], 133 [213], 86 [13]; byte 21 [13], 128 [213], 21 [13]
// orhT hc/ho/gc: long 96 [13], 136 [213], 96 [13]; byte 94 [13], 133 [213], 94 [13]
// rohT hc/ho/gc: long 98 [13], 137 [213], 98 [13]; byte 96 [13], 132 [213], 96 [13]
// horT hc/ho/gc: long 86 [13], 127 [209], 86 [13]; byte 21 [13], 124 [209], 21 [13]
// Nothing beats 21 [13] / 24 [0]; no form without a one-use local keeps the caller and gains (ohart ho byte 68 [0] is the best with every local
// used twice: object index EDX, case 0 CSE'd with the type read, 0x7 first difference). Left on 24: retail loads the header data first into EDX, copies
// the incoming index to ESI for the rigid body and reuses ECX for the object index, and loads the type with one movzx. Body unchanged at 96.
// Round 68 (m3 lane 2, fast; caller 0x1d5bf0 in brackets): round 60's 24 form rebuilt 24 [0] (left: object index EDX, headers ESI,
// rigid body ECX in place, and `mov al` + `movzx` for the type); on it a model local for the rigid body 24 [0] (same code), the type
// through a header element pointer 121 [194] (272 bytes). On the current body: the main switch's type in a byte local 96 [0] (same
// code); case 11 as `level <= 1 ? 4 : (level >= 5 ? 5 : 3)` 96 [0] (same code); the rigid body computed after the kind-7 block 100 [0].
// m3 lane 1 r85 (fast, caller 0x1d5bf0 in brackets): round 60's 24 form rebuilt 24 [0]; on it a top-level const header
// pointer, a const rigid-body pointer, the type declared after `result`, the rigid body as `bodies + index`, a const type
// 24 each (same code); with no type local and both switches on `headers[absolute_index].type` 86 at 264 B [13] (flip).
// m3 lane 12 r106 (fast, caller 0x1d5bf0 in brackets, control 96 [0]): one byte `type` read at entry shared by both kind
// routes with case 0 through havok_object_get(object_index) (every local used twice) 133 at 220 B [213]; that with the
// kind-7 route as `return type == 1 ? 0xc : 1;` 136 at 216 B [213]; with the rigid body declared first 132 [213]; with
// the type read before the rigid body 117 at 236 B [213]; the current body with a `headers` local for its two type
// reads 117 at 272 B [160].
// m3 lane 14 r82 (fast, caller 0x1d5bf0 in brackets): round 60's 24 form re-measured 24 at 256 B [0], aligned 21: retail loads the
// header data first into EDX, copies the incoming index to ESI and reuses ECX for the object index, where ours gives the object
// index EDX, the headers ESI and computes the rigid body in place in ECX; and ours loads the type as `mov al` plus a `movzx` at the
// uses for retail's one `movzx` (not kept: the object_index local is used once). A /Gd first pass of the current body loads the
// component into EAX and the index into ECX, retail's pair.
// Also r82: permuter on the 24 form (600 s, 13 tries): best 24; its reduced image holds only this file, scores the form 101
// (the caller's convention is missing), so it is no guide here.
// Also r82 on the 24 form (fast, caller in brackets): the kind-7 route as `return type == 1 ? 0xc : 1;` 94 [0]; per-case returns 24
// (same code); no `__assume(0)` default 125 at 228 B [213]; the kind test inverted (main switch first) 94 [0]; `signed char` type 24
// (the listed char); case 11's level test with its arms the other way round 35 [0].
// Round 71 (m3 lane 15, fast; caller 0x1d5bf0 matched throughout): round 60's 24 form re-measured 24 at 256 bytes
// (register-masked aligned 8); on it the main switch's four cases in all 23 other source orders: best 42 (11,0,1,7 and
// 1,11,0,7), the rest 45-67, all 256 bytes. Retail lays the case bodies out 11, 1, 0, 7, the order already used.
// m3 lane 15 r74 (fast, caller 0x1d5bf0 in brackets): both bodies return with `ret 8` (owner and level on the stack,
// component EAX, rigid index ECX, kind EDI); the MiniMax I line above that says ours returns `ret 4` is wrong. A long
// type read once at entry with `if (type == 1)` for kind 7 141 at 212 B [213] (kind arrives in ESI and the level in EDI,
// `ret 4`); with the one-case switch for kind 7 136 [213] (re-measure of round 10's form). Retail computes the absolute
// index twice (`and eax, 0xffff` for the type, `and ecx, 0xffff` again in case 0) and keeps the object index in ECX for
// both, so its case 0 is not CSE'd with the type read the way every one-read form here is.
// One type read with no rigid-body local (both flag tests through the owner chain), kind 7 as the one-case switch: byte
// type, case 0 through object_index 113 at 240 B [210]; long type 117 at 236 B [210]; byte or long type with case 0
// re-reading the component 97/97 at 260 B [122].
// Round 173 (m3 lane 6, fast, caller 0x1d5bf0 in brackets): round 60's 24 form rebuilt 24 [0] (left: object index EDX, headers
// ESI, rigid body ECX in place, `mov al` + `movzx` type); on it the absolute index from the component with no object-index local
// 101 [0]; the type read through the global with the header local for case 0 only 24 [0] (same code); `result` declared first 24
// [0] (same code); declaration order o,a,h,r,t 88 [0]; o,a,h,t,r 111 at 268 B [9]; a dword absolute index 24 [0] (same code); a
// dword object index 24 [0] (same code); the kind-7 route setting `result` (1, or 0xc in its one-case switch) with the main
// switch as its `else` and one return 95 [0]. Body unchanged at 96.
// M3 lane 3 r238 (fast, caller 0x1d5bf0 matched throughout): case 11 as an if / else-if chain 96 (same code); the two
// flag selections as ternaries 98 (272 B). Re-measured: round 60's 24 form 24 (256 B), with a long type 89 (aligned 24,
// masked 12: the type then loads with retail's one movzx; left is the entry's register choice, headers ESI loaded after
// the bodies, object index EDX, rigid body in ECX, where retail loads the headers first into EDX and uses ECX, ESI).
// Round 194 (m3 lane 4, fast, caller 0x1d5bf0 in brackets), on round 56's one-read base (object_index, type, rigid body, the
// kind-7 ternary; re-measured 96 at 256 B [0]): a `headers` local for the type read with case 0 through
// `headers[component->object_index & 0xffff]` 96 at 256 B [0] (headers loaded after the bodies into ESI, object index EDX);
// with case 0 through `headers[object_index & 0xffff]` 141 at 212 B [213]; a byte type 95 at 256 B [0]; the rigid body
// declared first 107 at 240 B [13]; `headers` declared first with case 0 through the component 96 at 256 B [2] (the component
// arrives in EDX). Body unchanged.
// @retail 0x1d69d0
long function_1d69d0(s_havok_component *component, long kind, s_physics_model_owner *owner, long level, long rigid_body_index)
{
	long object_index = component->object_index;
	s_physics_rigid_body *rigid_body = &owner->model->rigid_bodies[rigid_body_index];
	long result;

	if (kind == 7)
	{
		switch (((s_physics_object_header *)g_4e0300->data)[object_index & 0xffff].type)
		{
		case 1:
			return 0xc;
		}
		return 1;
	}
	switch (((s_physics_object_header *)g_4e0300->data)[object_index & 0xffff].type)
	{
	case 11:
		if (level <= 1)
		{
			result = 4;
		}
		else
		{
			result = level >= 5 ? 5 : 3;
		}
		break;
	case 1:
		if (rigid_body->flags & 8)
			result = 14;
		else
			result = 12;
		break;
	case 0:
		/* retail looks the object up again from the component here */
		result = (bool)(((dword)((byte *)havok_object_get(component->object_index))[0x10a] >> 2) & 1) ? 11 : 9;
		break;
	case 7:
		if (kind == 6)
		{
			result = 8;
		}
		else
		{
			if (rigid_body->flags & 8)
				result = 7;
			else
				result = 6;
		}
		break;
	default:
		__assume(0);
	}
	return result;
}

struct s_physics_graph_body
{
    byte unknown00[0x18];
    byte flags;
    byte unknown19[3];
    short replacement;
    byte unknown1e[0x90 - 0x1e];
};

struct s_physics_graph_entry
{
    byte unknown00[4];
    short groups[2];
    long section_count;
    s_physics_entry_section *sections;
    byte unknown10[8];
};

struct s_physics_graph_model
{
    byte unknown00[0x30];
    long entry_count;
    s_physics_graph_entry *entries;
    long body_count;
    s_physics_graph_body *bodies;
};

struct s_physics_body_group
{
    char bodies[64];
    long count;
};

struct s_physics_body_groups
{
    long unknown00;
    s_physics_body_group groups[257];
};

bool function_db4c0(long object_index, long key_a, long key_b);

/* 0x1d6630 is a method of the component: retail passes it in ecx, the rest on the stack */
struct s_physics_component_walk : s_havok_component
{
    void function_1d6630(s_physics_model_owner *owner, char const *kinds,
        dword const *entry_mask, s_physics_body_groups *groups, void *context, long parent, long current,
        char *output, long *output_count, void *other_context, dword *visited, bool *active);
};

static __forceinline long component_object_index(s_havok_component *component)
{
    return component->object_index;
}

static __forceinline bool entry_section_open(s_havok_component *component, s_physics_graph_entry *entry)
{
    for (long j = 0; j < entry->section_count; ++j)
    {
        s_physics_entry_section *section = &entry->sections[j];
        bool section_open = !function_db540(component_object_index(component), section->key_a, section->key_b) &&
            !function_db4c0(component_object_index(component), section->key_a, section->key_b);
        if (section_open)
            return true;
    }
    return false;
}

// @retail 0x1d6630
void s_physics_component_walk::function_1d6630(s_physics_model_owner *owner, char const *kinds,
    dword const *entry_mask, s_physics_body_groups *groups, void *context, long parent, long current,
    char *output, long *output_count, void *other_context, dword *visited, bool *active)
{
    s_havok_component *component = this;
    bool replace = !(bool)((component->unknown04 >> 13) & 1);
    if ((current == NONE ? groups->groups[256].count : groups->groups[current].count) > 0)
    {
        for (long i = 0; i < (current == NONE ? groups->groups[256].count : groups->groups[current].count); ++i)
        {
            long body_index = (current == NONE ? groups->groups[256].bodies[i] : groups->groups[current].bodies[i]);
            s_physics_graph_model *model = (s_physics_graph_model *)owner->model;
            s_physics_graph_body *body = &model->bodies[body_index];
            long output_index = body_index;
            bool available = false;
            bool replaced = false;
            if (body->flags & 0x20)
            {
                short replacement = body->replacement;
                long bounded = replacement < 0 ? 0 : replacement > model->body_count - 1 ? model->body_count - 1 : replacement;
                if (bounded == replacement)
                {
                    available = true;
                    if (replace)
                    {
                        replaced = true;
                        output_index = replacement;
                    }
                }
            }
            output[(*output_count)++] = (char)output_index;
            if (replaced)
                component->unknown04 |= 0x100000;
            if (available)
                component->unknown04 |= 0x80000;
        }
        if (current != NONE)
            visited[current >> 5] |= 1 << (current & 31);
        for (long entry_index = 0; entry_index < owner->model->entry_count; ++entry_index)
        {
            s_physics_graph_entry *entry = &((s_physics_graph_model *)owner->model)->entries[entry_index];
            if (!(entry_mask[entry_index >> 5] & (1 << (entry_index & 31))))
                continue;
            short next_group = entry->groups[0];
            if (next_group != current && entry->groups[1] != current)
                continue;
            long next = next_group == current ? entry->groups[1] : next_group;
            long first_body;
            if (next != NONE && groups->groups[next].count > 0)
                first_body = groups->groups[next].bodies[0];
            else
                first_body = NONE;
            if (first_body != NONE && (kinds[first_body] == 6 || (visited[next >> 5] & (1 << (next & 31)))))
                continue;
            bool open = true;
            if (next == NONE)
                open = entry_section_open(component, entry);
            else if (groups->groups[next].count > 0)
            {
                if (entry->section_count == 0)
                {
                    long this_body = (current == NONE ? groups->groups[256].bodies[0] : groups->groups[current].bodies[0]);
                    char next_body = groups->groups[next].bodies[0];
                    __assume(this_body != NONE);
                    open = kinds[this_body] != 7 && kinds[next_body] != 7;
                }
                else
                    open = entry_section_open(component, entry);
            }
            if (open)
            {
                if (next == NONE)
                    *active = true;
                else if (first_body != NONE)
                    static_cast<s_physics_component_walk *>(component)->function_1d6630(owner, kinds, entry_mask, groups, context, current, next,
                        output, output_count, other_context, visited, active);
            }
        }
    }
}


#include "havok_reference.h"
#include <new>

struct s_physics_mass
{
    real volume;
    real mass;
    byte unknown08[8];
    hkVector4 center;
    hkRotation inertia;
};

struct s_physics_mass_entry
{
    s_physics_mass mass;
    hkTransform transform;
};

struct s_physics_mass_array
{
    s_physics_mass_entry *data;
    long size;
    long capacity;
};

class c_physics_allocation_view
{
public:
    virtual void slot0() {}
    virtual void slot1() {}
    virtual void slot2() {}
    virtual void slot3() {}
    virtual void *allocate(long bytes, long category) { return NULL; }
    virtual void release(void *data, long bytes, long category) {}
};

class c_physics_shape_view : public c_havok_reference_counted
{
public:
    virtual void slot1() {}
    virtual void slot2() {}
    virtual void slot3() {}
    virtual void slot4() {}
    virtual long type() { return 0; }
};

struct c_child_transform : c_havok_reference_counted
{
    dword user;
    long field_c;
    hkTransform transform;
    c_child_transform(c_havok_reference_counted *child);
};

class c_physics_shape_list
{
public:
    c_physics_shape_list(c_havok_reference_counted **shapes, long count);
};

long __cdecl function_2dba80(s_physics_mass_array const *array, s_physics_mass *result);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *out);
extern transform4x3f *g_4687d0;

PRIVATE __forceinline void physics_mass_frame(transform4x3f const *matrix, hkTransform *out)
{
    real *columns = (real *)out;
    columns[0] = matrix->forward.i; columns[1] = matrix->forward.j; columns[2] = matrix->forward.k; columns[3] = 0;
    columns[4] = matrix->left.i; columns[5] = matrix->left.j; columns[6] = matrix->left.k; columns[7] = 0;
    columns[8] = matrix->up.i; columns[9] = matrix->up.j; columns[10] = matrix->up.k; columns[11] = 0;
    hkVector4 translation;
    translation.set(matrix->position.x, matrix->position.y, matrix->position.z);
    out->m_translation = translation;
}

PRIVATE inline void physics_mass_identity(hkTransform *out)
{
    __m128 zero = _mm_setzero_ps();
    hkVector4 *column0 = &out->m_rotation.m_col0;
    hkVector4 *column1 = &out->m_rotation.m_col1;
    hkVector4 *column2 = &out->m_rotation.m_col2;
    column0->m_quad = zero;
    column1->m_quad = zero;
    column2->m_quad = zero;
    ((real *)column0)[0] = 1.0f;
    ((real *)column1)[1] = 1.0f;
    ((real *)column2)[2] = 1.0f;
    out->m_translation.m_quad = zero;
}

PRIVATE __forceinline void physics_mass_copy(byte const *body, s_physics_mass_array *array, long i)
{
    array->data[i].mass.volume = 0;
    array->data[i].mass.mass = *(real const *)(body + 0x3c);
    array->data[i].mass.center = *(hkVector4 const *)(body + 0x40);
    array->data[i].mass.inertia = *(hkRotation const *)(body + 0x50);
}

// @retail 0x1d5ec0
c_havok_reference_counted *__stdcall function_1d5ec0(s_havok_component *component,
    s_physics_model_owner *owner, char const *kinds, char *output, long *output_count,
    s_physics_body_groups *groups, long current, dword *visited, real *mass,
    hkVector4 *center, hkRotation *inertia, bool *active, long *maximum,
    bool *mixed, bool *special)
{
    dword entry_mask[128];
    *output_count = 0;
    *active = false;
    *mixed = false;
    function_1d6560(component, owner, entry_mask);
    static_cast<s_physics_component_walk *>(component)->function_1d6630(owner, kinds, entry_mask, groups, (void *)current, NONE,
        current, output, output_count, (void *)64, visited, active);
    long count = *output_count;
    if (count > 0)
    {
        long first_index = output[0];
        bool preserve_wrapper = kinds[first_index] == 7 || kinds[first_index] == 6;
        c_havok_reference_counted *result;
        long flagged;
        if (count == 1)
        {
            byte *first = (byte *)owner->model->rigid_bodies + first_index * 0x90;
            *mass = *(real *)(first + 0x3c);
            *center = *(hkVector4 *)(first + 0x40);
            *inertia = *(hkRotation *)(first + 0x50);
            *maximum = *(short *)(first + 0x1e);
            flagged = ((dword)first[0x18] >> 2) & 1;
            result = *(c_havok_reference_counted **)(first + 0x38);
            result->reference_count++;
            if (first[0x8e] & 1)
                *special = true;
        }
        else
        {
            byte *first = (byte *)owner->model->rigid_bodies + first_index * 0x90;
            byte *model = *(byte **)((byte *)owner + 0x44);
            byte *render = g_4e3b44[*(long *)(model + 4) & 0xffff].bytes;
            transform4x3f const *root = *(short *)first != NONE ?
                (transform4x3f *)(*(byte **)(render + 0x4c) + *(short *)first * 0x60 + 0x28) : g_4687d0;
            c_havok_reference_counted *shapes[64];
            shapes[0] = *(c_havok_reference_counted **)(first + 0x38);
            if (!preserve_wrapper && ((c_physics_shape_view *)shapes[0])->type() == 0x13)
                shapes[0] = *(c_havok_reference_counted **)((byte *)shapes[0] + 0xc);
            shapes[0]->reference_count++;
            if (first[0x8e] & 1)
                *special = true;
            c_physics_allocation_view *allocator = (c_physics_allocation_view *)g_480118;
            s_physics_mass_array array;
            long entries = *output_count;
            array.data = (s_physics_mass_entry *)allocator->allocate(entries * 0x90, 0x12);
            array.size = entries;
            array.capacity = entries;
            s_physics_mass aggregate;
            aggregate.volume = 0;
            aggregate.mass = 0;
            aggregate.center.m_quad = _mm_setzero_ps();
            aggregate.inertia.m_col0.m_quad = _mm_setzero_ps();
            aggregate.inertia.m_col1.m_quad = _mm_setzero_ps();
            aggregate.inertia.m_col2.m_quad = _mm_setzero_ps();
            physics_mass_identity(&array.data[0].transform);
            physics_mass_copy(first, &array, 0);
            *maximum = *(short *)(first + 0x1e);
            flagged = ((dword)first[0x18] >> 2) & 1;
            for (long i = 1; i < *output_count; i++)
            {
                byte *body = (byte *)owner->model->rigid_bodies + output[i] * 0x90;
                c_physics_shape_view *child = *(c_physics_shape_view **)(body + 0x38);
                bool transformed = *(short *)first != *(short *)body;
                hkTransform transform;
                if (transformed)
                {
                    transform4x3f inverse, relative;
                    function_141590((transform4x3f *)(*(byte **)(render + 0x4c) + *(short *)body * 0x60 + 0x28), &inverse);
                    function_142a60(root, &inverse, &relative);
                    physics_mass_frame(&relative, &transform);
                    if (child->type() == 0x15)
                    {
                        transform.transform_compose_in_place(*(hkTransform *)((byte *)child + 0x10));
                        child = *(c_physics_shape_view **)((byte *)child + 0xc);
                    }
                }
                else
                    physics_mass_identity(&transform);
                if (!preserve_wrapper && child->type() == 0x13)
                    child = *(c_physics_shape_view **)((byte *)child + 0xc);
                if (transformed)
                {
                    void *storage = ((c_physics_allocation_view *)g_480118)->allocate(0x50, 0x22);
                    c_child_transform *wrapper = storage ? new (storage) c_child_transform(child) : NULL;
                    wrapper->transform = transform;
                    child = (c_physics_shape_view *)wrapper;
                }
                else
                    child->reference_count++;
                if (body[0x8e] & 1)
                    *special = true;
                shapes[i] = child;
                array.data[i].transform = transform;
                physics_mass_copy(body, &array, i);
                long value = *(short *)(body + 0x1e);
                *maximum = *maximum > value ? *maximum : value;
                flagged += ((dword)body[0x18] >> 2) & 1;
            }
            function_2dba80(&array, &aggregate);
            *mass = aggregate.mass;
            *center = aggregate.center;
            c_physics_allocation_view *list_allocator = (c_physics_allocation_view *)g_480118;
            *inertia = aggregate.inertia;
            void *storage = list_allocator->allocate(0x38, 0x22);
            *(word *)((byte *)storage + 4) = 0x38;
            result = (c_havok_reference_counted *)new (storage) c_physics_shape_list(shapes, *output_count);
            for (long i = 0; i < *output_count; i++)
                havok_reference_remove(shapes[i]);
            if (!(array.capacity & 0x80000000))
                ((c_physics_allocation_view *)g_480118)->release(array.data, (array.capacity & 0x7fffffff) * 0x90, 0x12);
        }
        *mixed = flagged != *output_count;
        return result;
    }
    return NULL;
}
