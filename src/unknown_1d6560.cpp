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
// score 96 (was 96): no local frame; retail reads the type once before the kind test and keeps
// the body array/body in EBP/ESI. Current component EAX, rigid index ECX, kind EDI and ret 8
// agree with the matched caller. A single type read must preserve that incoming convention;
// every prior one-read type form flips the component/index pair (the caller
// 0x1d5bf0 then differs). Type read after the rigid body 94 at 256 bytes (caller 13), first
// 117, after headers/body/index 94 (caller 13), body then type 136, kind 7 as a ternary 104;
// case 0 through the object_index local 91 at 256 bytes (caller 146), with the one-read type
// 132-133; parameters reordered (rigid index first, component last) 96 / 94 (caller 13).
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

/* a flag field read as word bits: retail reads these flags with a load, a
   shift and a test of the low bit */
struct s_flag_word_bits
{
	word bit0 : 1;
	word bit1 : 1;
	word bit2 : 1;
	word bit3 : 1;
	word bit4 : 1;
	word bit5 : 1;
	word bit6 : 1;
	word bit7 : 1;
	word bit8 : 1;
	word bit9 : 1;
	word bit10 : 1;
	word bit11 : 1;
	word bit12 : 1;
	word bit13 : 1;
	word bit14 : 1;
	word bit15 : 1;
};

// score 241 (was 261): frame 0x18, current ESI, groups EBX and the body loop index EDI now agree. Left: the
// model pointer is in ESI where retail uses EDX, the output body index and the replaced flag swap BL/CL, the
// availability stays in memory where retail reloads it into DL (so ours hoists 0x80000 into EBP), the two
// flag bytes swap frame slots 2/3, and `next` is compared as a short where retail sign-extends it once.
// Round 30 (lane D): no better form found. The permuter ran 50 tries and its best was 258 against the
// 261 baseline (three points, below the retention margin), and two flag reads were tried: bit 5 through
// a byte bitfield view and the flag byte's mask tested directly both score 275, so the word bitfield
// view stays. What still differs: our frame is 0x1c against retail's 0x18, the current and entry roles
// are rotated between EDI and ESI, the next-group base is retained in an extra spill, and the policy
// and availability bytes take the two frame slots the other way round. Original body kept.
// Earlier: Frame 0x1c versus 0x18; current/entry/status homes and next-group spill remain.
// flag 1 read through a word bitfield view gives retail's load-and-shift;
// frame 0x1c versus 0x18 and the current/entry roles and next-group spill still differ.
// Round 9 (lane 11, fast): first body set in an if/else 300 (frame 0x18 and retail's out-of-line NONE store
// with the re-tested next, but current moves to EDX and the entry index to ESI where retail has ESI/EDX);
// with it: nested first-body skip test 300 (identical); `this` used directly for the component alias 300
// (identical); availability set after the replacement branch 318; model pointer read before the body
// index 294 (922 bytes).
// In the body loop retail keeps the policy byte at frame 2 and the availability byte at 3
// (available in dl on the replacement path, reloaded from its byte otherwise; replaced in cl);
// ours swaps the two bytes and keeps available in memory, replaced in dl. Round 57: availability
// named to sort after the policy, the policy named to sort first, both: 275 each (same layout);
// available declared before replaced 303 (898 bytes); both flags declared before the body lookup 300.
// Earlier: frame 0x1c versus 0x18; current/entry roles and next-group spill differ.
// Reduced trials: availability first 303; immutable policy 275; read-only model 275; read-only body 275;
// native availability 297; native application 305; byte first body 298; read-only sections 275.
// frame 0x1c versus 0x18, current/entry homes and group
// spill differ; direct receiver 275; signed-word next 310; byte body 319;
// direct model uses 275; conditional first body 300. Original body kept.
// Earlier: extra frame word and group/current homes remain;
// resolved group index 385; outer entry counter 275; promoted replacement 336;
// explicit output advance 275; branched body lookup 285.
// frame 0x1c versus 0x18; current/entry roles and group spill differ.
// Shared sections 357; next path first 276; replacement bool names 275;
// frame pointer 325; size preference 446; shared sections with in-loop store 352;
// availability last 298. Original kept.
// Body-byte cursor 323; edge-group array 275; separate body rejections 275;
// native section result 275; native replacement policy 307 (frame 0x18,
// 898 bytes); policy and cursor 300 (930 bytes). Original body kept.
// Earlier: extra frame word and current/entry/group homes remain.
// Combined status mask 307; direct body fields 320; section callback guards 275;
// entry eligibility guards 275; next-count snapshot 301. Original body restored.
// Earlier: frame 0x1c versus 0x18 and status/group homes remain.
// Byte availability/application/pair 275/275/275; body/entry references 275/275;
// ordinary body while scan 275 (plain).
// Signed-word policy/availability/application 298/297/293 (frame 0x18); repeated upper clamp bound
// 279; paired signed-word statuses 285.
// Round 215: signed-byte availability/application/policy 275/275/275;
// unsigned-word policy 298; policy field at each body use 325.
// Round 188 connection ordinal, neighbor group/body, section ordinals, their three
// pairs, all three, and all plus body ordinal renamed: 275 each; frame/group spill unchanged.
// Round 167: Signed-word first/current-side/next-side group body identities, individually
// and in all combinations, 275 each; signed-word output body index 317 at 898/922,
// frame 0x18 recovered but saved roles still differ. All eight forms restored.
// Round 150: Positive policy acquired inside nonempty admission, bool/short 290/300; complementary
// preserve-originals policy at entry/admission bool 323/324, char 323/324, short 306/310. Policy
// ownership does not recover current/entry/status allocation or the neighbor-group spill; complete
// original method restored.
// Round 375: Complete tested-status producers, guarded bool/byte/short/long
// 319/319/300/319; eager replacement/clamp 304/304/298/307. All recover frame 0x18;
// guarded short retains 922 bytes, others 898. Register roles still differ; restored.
// Round 210: terminal/neighbor-owned first-body result 292/304; conditional result 296/303;
// native word neighbors on those four graphs 317/312/299/307. First form recovers 0x18 and 922 bytes.
// Round 139: Separate published-body result long/short/signed-byte/byte 311/319/319/319; those four
// with application status from policy 319/304/304/304. All recover frame 0x18 but shorten to
// 898/922 and retain wrong current/entry roles. Original executable source and output/status/clamp
// observations restored.
// Round 350: shared entry-bit test, visited-bit set, neighbor-bit test and all combinations
// 275 each; complete shared bits with direct receiver 275. Frame/status/group spill unchanged.
// Round 96: replace flag renamed to sort first 275 (slots unchanged); next-group count/body through the
// current-style ternary 307; the file's section-test helper for the first query 283; first body as
// if/else or ternary 300/300 (frame 0x18 and retail's untreaded next test, but current lands in EDX).
// Retail stores -1 in a separate block and re-tests next, so it recomputes the group where ours spills it.
// Round 190: method-scope body identity, both statuses, identity/status pair, model/body
// receivers and each receiver combination 275 each; common native section ordinal 275.
// Every initializer and memory observation retained at its original point; all forms restored.
// Round 253: Existing component policy/application/availability bit helpers,
// all seven nonempty subsets 275 each; native upper-bitfield policy with both
// shared status stores 275. Original method and include set restored.
// Round 82: first body if/else (frame 0x18, group recomputed as retail, but current in edx where
// retail keeps esi and the index/next in esi) 300; with next/first body declared before the entry
// tests 300; inverted first-body test 300; nested first-body rejection 300; NONE first body declared
// before the entry tests 313. Original body restored.
// Earlier: flag 1 read through a word bitfield view gives retail's load-and-shift; the rest still differs.
// Round 201: Preserving the retained word-view admission: real word flags 261; branch-owned first
// body 298, also 298 with word flags; model-first 278, also 278 with word flags; branch/model 280,
// also 280 with word flags; directly declared existing word-bitfield type 261. Original body, flag
// declaration, type location and includes restored. Masked-context diagnostics 275/297/300/294 in
// both widths repeat the older allocation and are excluded from the fresh count.
// Round 208: Recursive inline with first-body/availability/model masks 0..7:
// 261/298/257/298/278/280/278/280; availability-first with later policy, complementary policy, both
// and direct bounds: 274/296/297/318. Four-point 257 rejected; original method, class declaration
// and clamp restored. Frame/status/group homes remain.
// Round 249: Complementary availability/application/both truths, bool
// 258/261/292 and byte 258/261/292; both with availability declared first
// 279/279. Three-point best rejected; original status domains and complete body restored.
// Round 31 (lane 15, fast): first body as if/else plus `short next = groups[0]; if (next == current) next =
// groups[1];` 241 (kept: retail's untreaded NONE store, no group spill); if/else alone 298 (909), mask flag
// test alone 275, short next alone 309 (898), short ternary 304 (898), long overwrite 292, mask + short next
// 314 (898); on the kept body: mask flag test 279 (aligned 111 -> 110), short ternary 298, long overwrite 298,
// short temp then long next 288 (925), long with field test 299 (935), long/short if/else 298/298, short with
// field test 319 (948); policy renamed allow_replacement, availability renamed, both flags or the availability
// declared at method scope, policy declared after them, byte/char flag types, byte policy: 241 (same code);
// availability declared first 270; flags before the body index 240; model before the body index 253; model
// through owner at each use 241; clamp inline 241; output as `replaced ? replacement : body_index` 252, with
// mask 242; a separate output index 241, with mask 279.
// Round 35 (lane 15, fast; aligned counts in brackets, base 241 [144]): `body_index = replacement;` before
// `replaced = true;` 241 [144]; `available = true;` after the replace block 252 [129] (lead: the policy and
// availability bytes then take retail's slots 0x12/0x13 and available gets retail's `mov dl, 1`; left: model in
// ESI for EDX, body index ECX for EBX, replaced BL for CL); on it the mask test `body->flags & 0x20` 279 [144],
// with both flags declared first 302 [185], the model read through owner at both uses 252 [129], the model local
// before the body index 275 [188]; the clamp bound named `last` 242 [154].
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
            bool replaced = false;
            bool available = false;
            if (TEST_FIELD_BIT(((s_flag_word_bits *)&body->flags)->bit5))
            {
                short replacement = body->replacement;
                long bounded = replacement < 0 ? 0 : replacement > model->body_count - 1 ? model->body_count - 1 : replacement;
                if (bounded == replacement)
                {
                    available = true;
                    if (replace)
                    {
                        replaced = true;
                        body_index = replacement;
                    }
                }
            }
            output[(*output_count)++] = (char)body_index;
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
            if ((entry_mask[entry_index >> 5] & (1 << (entry_index & 31))) &&
                (entry->groups[0] == current || entry->groups[1] == current))
            {
                short next = entry->groups[0];
                if (next == current)
                    next = entry->groups[1];
                long first_body;
                if (next != NONE && groups->groups[next].count > 0)
                    first_body = groups->groups[next].bodies[0];
                else
                    first_body = NONE;
                if (first_body != NONE && (kinds[first_body] == 6 || (visited[next >> 5] & (1 << (next & 31)))))
                    continue;
                if (next == NONE)
                {
                    for (long j = 0; j < entry->section_count; ++j)
                    {
                        s_physics_entry_section *section = &entry->sections[j];
                        if (!function_db540(component->object_index, section->key_a, section->key_b) &&
                            !function_db4c0(component->object_index, section->key_a, section->key_b))
                        {
                            *active = true;
                            break;
                        }
                    }
                }
                else
                {
                    if (groups->groups[next].count > 0)
                    {
                        if (entry->section_count == 0)
                        {
                            long this_body = (current == NONE ? groups->groups[256].bodies[0] : groups->groups[current].bodies[0]);
                            long next_body = groups->groups[next].bodies[0];
                            if (kinds[this_body] == 7 || kinds[next_body] == 7)
                                continue;
                        }
                        else
                        {
                            long j;
                            for (j = 0; j < entry->section_count; ++j)
                            {
                                s_physics_entry_section *section = &entry->sections[j];
                                if (!function_db540(component->object_index, section->key_a, section->key_b) &&
                                    !function_db4c0(component->object_index, section->key_a, section->key_b))
                                    break;
                            }
                            if (j >= entry->section_count)
                                continue;
                        }
                    }
                    if (first_body != NONE)
                        static_cast<s_physics_component_walk *>(component)->function_1d6630(owner, kinds, entry_mask, groups, context, current, next,
                            output, output_count, other_context, visited, active);
                }
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
    out->m_rotation.m_col0.m_quad = zero;
    out->m_rotation.m_col1.m_quad = zero;
    out->m_rotation.m_col2.m_quad = zero;
    ((real *)&out->m_rotation.m_col0)[0] = 1.0f;
    ((real *)&out->m_rotation.m_col1)[1] = 1.0f;
    ((real *)&out->m_rotation.m_col2)[2] = 1.0f;
    out->m_translation.m_quad = zero;
}

PRIVATE __forceinline void physics_mass_copy(byte const *body, s_physics_mass_array *array, long i)
{
    array->data[i].mass.volume = 0;
    array->data[i].mass.mass = *(real const *)(body + 0x3c);
    array->data[i].mass.center = *(hkVector4 const *)(body + 0x40);
    array->data[i].mass.inertia = *(hkRotation const *)(body + 0x50);
}

// score 4 (was 4): retail loads the 1.0f constant after it stores the array pointer, and
// loads the allocator global before it stores the centre; ours does both the other way round.
// Round 26: g_480118 defined in a /GL- library file instead of src/globals.cpp fixes the second
// (LTCG counts an LTCG-defined global as aliased by pointer stores): 2, not kept (a global's
// definition moved for a gain without a match).
// Vector set 454; repeated zero intrinsic 4; explicit quad stores 197; direct lane stores 4;
// explicit centre store 4; centre memcpy 125; plain copy inline 4; const centre 4; transform set 4.
// Round 26: allocator local between the mass and centre copies 4; forceinline identity 4; mass
// array built by an inline constructor 4; on 2: count read before the allocator local 2, a first-entry
// pointer for the identity 2, aggregate declared before the array 2, zero intrinsic per identity
// store 2, identity columns set one at a time 2.
// Round 206: shared allocator interface at first/list/both/all sites 4/4/4/4;
// shared allocator getter at all sites 4; block allocator interface at first/list/all 4/4/4.
// Round 18 (lane 14, compile-only screens, which reproduce both swaps): size/capacity chained either way 4;
// aggregate zeroed by an s_physics_mass constructor 4; identity zero via an hkVector4 local 4, 1.0s via
// m128_f32 lanes 4; aggregate scalar zeros chained/swapped 4/4, one named __m128 zero 4; allocation without
// the allocator local 4; identity on array.data->transform 4; identity through a reference 4.
// Round 92 (plain): identity before size 23, between size/capacity 22; pointer-return identity 4;
// initial allocator for the list 339; list allocator for release 102; list allocator before the
// aggregate call 85; value-initialized array 345; one allocator throughout 340. Original restored.
// Round 31 (lane 15, fast): g_480118 defined in the allocator's own /GL- file (0x2d6fc0's) 2, with the
// allocator read between the mass and centre copies 2; that read alone 4; identity by reference 4; chained
// capacity/size 4. Compile-only screens (they reproduce both swaps) keep the 1.0 load first for: identity
// right after the allocation, size/capacity first, capacity before size, set_ss/named one/column-pointer/
// m128_f32/rotation-reference identities, a nested rotation identity, a zero-quad local, aggregate zeroed
// by a helper, a named data pointer, sizeof element size; only copy-before-identity (50 others differ) or a
// barrier after the data store (array.data reloaded) give retail's store-first order.
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
