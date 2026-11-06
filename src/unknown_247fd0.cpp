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

// allocates a particle for a particle system, seeds it from the random stream and spawns its child systems
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
		local_1->field_10 = arg_2->unknown04;
		local_1->field_38 = function_259d0(&g_4e7408->seed, 0, 0, 0.f, 1.f);
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
			long tag_index = local_2->tag_index;
			long effect_index = arg_2->effect_index;
			long local_6 = function_173fd0(local_4->function_24822f((word)local_5), effect_index,
				tag_index, (short)local_5, NONE);
			local_4 = (c_248228 *)particle_object_for_tag(local_2->tag_index);
			if (local_6 != NONE)
			{
				s_particle_system_datum *local_7 = DATUM(g_510c74, s_particle_system_datum, local_6);
				particle_system_link(DATUM(g_510c74, s_particle_system_datum, local_6), &arg_2->first_child_index, &arg_2->last_child_index);
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
/* the placement's word-sized value: callers build it in the stack argument slot */
class c_placement_value
{
public:
	c_placement_value(short value) : m_value(value) {}
	short m_value;
};

void placement_set(s_placement *arg_0, vector3f const *arg_1, point3f const *arg_2, vector3f const *arg_3, c_placement_value arg_4);
void function_246e60(s_particle_impact const *arg_2, s_object_246eb0 const *arg_3, long arg_4, dword arg_1, long arg_0);

// score 334 (was 356): frame 0xb0 and 1652 bytes agree; entry EBP is retail's now; effect/velocity/tag lookups keep retail's unfolded `add eax, edx`, blend and scaled position stay in memory, and the child loop rotates.
// Earlier first line: score 356 (was 356): frame 0xb0, 1643 bytes against 1652. Left: retail saves EBP at entry and
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
// Round 32 (m3 lane 2, fast): the early exit folded into one condition around the allocation (no return before the
// body) 356 (identical code, EBP still saved late); an inline effect-record getter at both effect sites 356 (folded
// back into one tree: retail's split `add eax, edx` at both sites stays absent); the motion branch as a switch on the
// definition short (case 0 / default) 370 (still `cmp word ptr [reg + 0xc], 0`); the final NONE test as an early
// return inside the body 356 (identical); the definition's bit-4 test as a shifted byte local 359 (still `test byte
// ptr [reg + 0x16], 0x10`). Retail 0x247fd0 saves EBP late after its one early exit; this one saves it at entry.
// Round 110 (m3 lane 7, fast): retail 0x247fd0 sinks its EBP push past the allocation test as ours does here, so
// retail's entry push comes from the effect-flag block. The early exit as `goto done;` 356 (same code, re-measured
// against round 137's goto form); the allocation failure as an early return with the body unnested 356 (same
// code); the flag's store and test as two statements (`local_18 = local_24 != 0; if (local_24 != 0) return;`) 425;
// the volatile flag declared in the effect block 356 (same code); the flag byte declared bare at function scope
// 356 (same code). Retail copies field_10 with `movss` (ours, and retail's 0x247fd0, with an integer move) and
// re-derives the effect record for the velocity (so a shared effect pointer would not fit): the copy through a
// `real` local 359 (still an integer move); with a `(real)` cast 356 (same code); through the reused `last_sample`
// 356 (integer move); through the grouped sample field 373 (1659 bytes: `movss` as retail, plus a store to the field).
// Round 233: Actual effect-pool header with allocated-pool/seed receiver masks 0-3:
// 387/401/387/401; conditional effect-data owner with those masks 389/394/390/405. Original pool
// observations, seed receivers, grouped geometry and complete body restored; early EBP save and
// random/blend/getter homes remain.
// Round 264: Existing bounded category helper with scalar-first-sample/selective-refresh/combined-header masks 0..7: 356/416/376/436/356/416/376/436. Original random producer, storage, header stores and descriptor refresh restored; late EBP save and random/blend/getter homes remain.
// Round 346: scaled position reused in publication and motion; scalar/point-set/vector-scale/vector-set,
// aggregate/setter publication: 395/395/397/395/375/375/376/375.
// Frame 0xb0 agrees; late EBP preservation, random/geometry homes and selective getter expansion remain.
// Original executable body restored.
// Round 353: First grouped draw retained with suffix-three/suffix-two output loops;
// indexed reference/direct producers 527/450/527/450, real-array pointer walks
// 517/469/517/469. Original record layout, draw sequence and complete body restored;
// late EBP preservation and random/blend/getter homes remain.
// AC474: Complete admission/failure results across the effect lookup and existing flag store,
// bool/byte/word/long: 436/436/448/448/425/425/432/431. Original direct exit and full body restored;
// frame 0xb0 agrees, late EBP save and random/blend/geometry/getter homes remain.
// Round 365: Plain native-array blend/contact/velocity traversals, subsets 1-7,
// 376/374/374/376/376/374/580; all with short component ordinals 528. Original
// complete body restored; late EBP save, escaped blend and random/getter homes remain.
// Round 367: Actual cache value array reused by evaluation/spawn, pointer/reference and entry/producer
// address, then missing-bit result reused for publication: 359/359/359/359/379/379/379/379.
// Original complete body restored; late EBP save and first-sample/blend/getter homes remain.
// Round 377: Original-RNG packed header, combined/named initial mask and native category/shifted-bit
// producers, eight forms 356/359/359/359/359/359/360/360. Original header publications, helper graph and
// complete body restored; late EBP save and random/geometry/getter homes remain.
// AC501: Read-only descriptor count/entry/terminal query declarations, masks 1/2/4/3/5/6/7,
// then all with read-only descriptor receiver: 356/356/356/356/356/356/356/356. Original interface and
// full body restored; late EBP save and random/blend/getter homes remain.
// B385: Record link/time scalar copies through memcpy/memcpy, memmove/memcpy,
// memcpy/memmove and memmove/memmove 356/448/383/450; each with the complete
// trace-direction setter 356/448/383/450. Original complete body/includes restored;
// early EBP preservation and random/blend/geometry/getter homes remain.
// B387: Read-only settings/immutable particle/immutable allocation identity, seven subsets,
// then all with immutable effect identity: 356 each. Original complete body restored;
// frame 0xb0 and late EBP save, escaped blend and random/getter homes remain.
// B393: Direct-expression initial scaled point aggregate/scalar parts, native/setter publication,
// age after/before publication: 393/393/358/363/357/376/358/376. NOT kept: no exact match;
// original complete body restored; late EBP save, escaped blend and random/getter homes remain.
// Round 66 (m3 lane 1, fast): grouped workspace members in retail's frame order (probe point, direction, the
// collision result moved in, blend, sample, placement) 355 at 1652 bytes (point/direction/collision/placement
// land on retail's offsets; retail's unused 16 bytes are the blend and sample slots, both kept in registers); on
// it the in-loop tag lookup through the inline particle_object_for_tag (retail inlines it here: the
// 'PRTM'/'prt3' compare) 383 (1691 bytes), plus the `(short)` child index and if/else reciprocal 383; blend as
// its own point3f 403 (frame 0xa4, blend in XMM2-4 as retail); and the first sample a plain real 432 (frame
// 0xa0). A named double in callee 0x249170 (diagnostic) leaves the frame at 0xa0. All restored.
// B393: Direct-expression initial scaled point aggregate/scalar parts, native/setter publication,
// age after/before publication: 393/393/358/363/357/376/358/376. NOT kept: no exact match;
// original complete body restored; late EBP save, escaped blend and random/getter homes remain.
// AC508 isolated trials: Existing random-output helper pointer/reference outputs, void/real/
// reference/pointer returns, ignored and complete returned-output consumers: eight scores 356.
// Original helper, every consumer and full body restored; late EBP save and sample/blend/getter homes remain.
// M344: Allocated-record pointer results at producer/entry, conditional/default-owned,
// positive guard/failure exit 409/410/409/410/409/410/409/410, all 1659/1652 bytes.
// NOT kept: unmatched nullable admission adds a pointer test; original handle admission,
// record observations, inherited qualifier, helpers and entire body restored.
// J247: Actual probe-point/contact-normal copy boundaries (memcpy/memmove pairs):
// 356/373/356/382/356/382/373/391; frame 0xb0 throughout. NOT kept: no gain;
// move forms are 1643 or 1627 bytes. Original body restored; delayed EBP and sample/geometry homes remain.
// B400: Geometry-kind/contact-query/terminal-dispatch decision results, masks 1/2/4/3/5/6/7,
// 359/359/359/359/359/359/356; complete graph with native loop predicate 356.
// NOT kept: no exact match or gain; original complete body restored; late EBP save and sample/blend/getter homes remain.
// B393: Direct-expression initial scaled point aggregate/scalar parts, native/setter publication,
// age after/before publication: 393/393/358/363/357/376/358/376. NOT kept: no exact match;
// original complete body restored; late EBP save, escaped blend and random/getter homes remain.
// DH334: Standard native-array scaling/translated-position/probe-copy algorithms, seven subsets
// then all with contact-normal array copy: 405/384/389/405/403/405/420/421.
// NOT kept: no exact match; original complete body restored; late EBP save, escaped blend and random/getter homes remain.
// F377: Complete contact-position/normal/inherited-velocity setters, single/pair/all graphs,
// then all with the probe-direction setter: 356/366/363/366/366/367/367/367. NOT kept: no gain;
// original complete body restored; late EBP save and sample/blend/getter homes remain.
// B436: Native constant-index blend/contact/velocity observations, seven subsets,
// then complete initial-scale coordinates: 356/356/356/356/356/356/356/356. NOT kept: no exact form or eligible gain;
// original complete body and helpers restored. Native frame/register differences remain.
// M3 lane 10 r73 (fast; aligned/masked in brackets, control 356 [256/142]). Kept: placement_set's last parameter as a by-value word class
// (retail's `push ecx; mov eax, esp; mov word ptr [eax], 0`) 337 [250/140], which also gives retail's entry EBP; the contact position through the
// existing vector3f_add_scaled 334 [246/135]. EXACT-MATCH RECIPE, reached [79/45] at retail's 1652 bytes and frame 0xb0, each part below kept only if
// the function then matches: the sum repeated in the link call (as matched 0x247fd0) 350; tag and effect index in one-use locals 353 [242/140];
// in-loop particle_object_for_tag 349 (1686) [211/119]; cache test as in 0x247fd0 367 [252/138], with the sum 358, with locals 358, with the lookup
// 388 (1686) [207/115]; the class on that 320 (1692) [193/103]; workspace ordered {probe, direction, collision, 16 unreferenced bytes, placement}
// 400 (1698) [176/83] (retail's offsets); blend and scaled position as locals stored by field with in-place blend sums 397 at 1652 [150/80]; aggregate
// store of the scaled position 452 (frame 0xbc); effect flag read twice unnamed 336 at 1652 [85/50], named pointer 334 (1634) [128/56]; bit 4 as a
// new one-bit field through TEST_FIELD_BIT gives retail's shr/test; blend and age in a 16-byte local struct {point, value} (struct-member scalars take
// retail's `movss` copy, plain locals never do) 415 [79/45], the struct alone drops the frame to 0xa0 417 [93/61]. Keepable subsets stay above this:
// best of 16 grid forms 355 (1654), effect hkBool with plain blend 426 (1638) [144/77], class plus each of bit field/cache test/lookup 352/345/374.
// Left on the recipe: `add eax, edx; mov dl, [eax + 2]`, `mov ax, [edi + 0xc]; test ax, ax`, `add eax, edx; add eax, 0x30`, the x row's load order, the
// probe copy's three loads, the normal's two early loads, the loop's table load with `add eax, edx; mov eax, [eax]`. Retail's body has no base+index
// address at all. Tried on it: hkBool flag helper by index or pointer 388 [100/55] (retail's dead byte store with no volatile, lookup still folded);
// flag3 through TEST_FIELD_BIT with volatile 407, plain or direct 382, named and tested twice 382/372, named with volatile 412 [100/64]; hkBool retiring
// the volatile on the old base 376; age as named real/sample/getter/double cast/const reference/`* 1.f`/`+ 0.f`/`true ? :` 336 each (integer move);
// stored twice 392; named remainder 334 no change; named short or velocity pointer no change; dead reads, duplicated pointers 415/410; 800 no-op inline
// calls 415; class with destructor 415, with copy constructor 415 (0x2b9060 197); empty else on the effect block 356; global optimization off, once as a
// diagnostic, 1853 (2916).
// Round 72 (m3 lane 1, harness with the callees' 8 files: control 386 [112] at 1636 bytes against the plain 356 at 1643, so ranked only; masked aligned in brackets): EBP's entry save on the early exit: empty else 386 [112], `do { } while (false)` in that
// else or after the block 386 [112], in the exit arm 386 [112] (same code each); `__assume(local_0 >= NONE)` after the block 418 [131] (1652 bytes, EBP still late); flag byte not volatile 440 [129]. Effect flag read: named datum pointer 402 [114], shifted byte local 425 [116],
// pointer and one-bit bool 414 [127]. Retail's flag byte store at [esp+0x1b] is never read back; no form tried here gives it without the volatile. Not kept; body unchanged.
// F404: Actual child-creation success/failure outcome in bool/byte/short/long,
// after descriptor refresh: 334/334/334/334/334/334/334/334. NOT kept: no exact outcome-storage form.
// Original handle admission, loop, random stream, geometry and whole body restored; lookup/blend/getter allocation remains.
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
					vector3f_add_scaled((vector3f const *)&collision.point, local_10, 0.005f, (vector3f *)&local_2->field_1c);
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
			function_246e60((s_particle_impact const *)&local_23.field_70, (s_object_246eb0 const *)local_2, local_19, local_21, local_20);
		}
	}
}
