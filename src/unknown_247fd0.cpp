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

// score 334 (was 334): earlier 356; frame 0xb0, 1652 bytes and entry EBP agree; effect addressing still folds (retail never
// combines base+index or folds a memory operand at the gate, the definition short or the bit-4 test), homes differ.
// MiniMax G (11-19): no new form, no lower score (334): the gate byte is still read from the folded address
// (`mov al, byte [eax+edx+2]`) into AL and stored at [esp+0x1f], where retail adds the base to the index first, reads
// the byte into DL and stores it at [esp+0x1b]; a pointer local for the datum (418), the shift chain in one statement
// (334) and the pointer local with a combined shift (334) leave the code as it is. The flags word is still loaded into
// AX (`mov ax, word [ebp+2]`) where retail zeroes EDX and loads DX before the mask, and reading the flags into a dword
// local first (338) or writing the mask as one expression (334) does not change it. Body unchanged.
// MiniMax G (11-19): no new form, no lower score (334): retail materializes the datum address with `and eax,0xffff;
// imul eax,eax,0x190; add eax,edx` and reads the flag byte at `[eax+2]` where ours uses the two-register address
// `[eax+edx+2]`, and the flag mask chain starts from `xor edx,edx; mov dx, word [ebp+2]` where ours zeroes EAX;
// binding the datum to a pointer local of its own does not change the code (334). Body unchanged.
// MiniMax E (11-15): no form kept; the permuter's best variant (333 in the reduced image and in the full one)
// only adds a local holding a constant, which is on the reverted list, and one point is under the bar anyway; an
// annealed run (19 tries) found nothing below the 334 of the baseline in either image.
// Still differs: the gate byte is read with the base and the index in one operand where retail adds the index to
// the base first and reads the byte from it (0x247985-0x247987), and the flag byte and the settings sit in the
// frame's later slots where retail keeps them at +0x1b and +0x1c.
// M3 lane 11 r123 (fast with 0x247fd0/0x248cc0, both MATCH): the cache block as matched 0x247fd0 writes it (pointer test
// `local_2 != arg_0->field_50`, pointer stored before the mask clear) 346 (aligned 246 -> 242, masked 106 -> 102: the
// load/and/store is retail's); the store order alone 347; the test order alone 333. Compile-only screens: /G5, /G6, /G7,
// /GB, /O1, /Ox, /Os, /Oy-, /Ob0, /Ob2, /Op all fold the gate's base+index and the 0x30 field address as ours does; an
// inline record getter, a byte-pointer record helper (sizeof or 0x190) and an inline flag test fold too; only a record
// pointer kept alive past the byte read (a volatile copy, diagnostic) gives retail's `add eax, edx; mov dl, [eax+2]`,
// so retail's record pointer outlives the gate read. An inline real getter or a named real for the age copy keeps
// the integer move (retail copies it through XMM0). The first block realigns at the first sample, which ours also
// stores to the grouped local (`fst [esp+0x70]`, absent in retail).
// MiniMax G (11-14): no new form, no lower score (334): the effect datum's address still folds into the scaled index in
// the flag byte's load and in the definition pointer's load, where retail keeps the address in a register; the flag
// byte's and the definition pointer's frame homes still differ.
// M3 lane 8 r159 (fast with 0x247fd0 and 0x248cc0, both stay matched): kept, the probe direction through the existing
// vector3f_assign (0.f, 0.f, -1.f) 334 and the 3-bit draw through the existing random_index(&seed, 5) 334 (same code each;
// they replace those helpers' bodies written out). The scale step as three `*=` 334 (same code); re-measured on main, the
// bit-4 test as TEST_FIELD_BIT(local_3->flag4) 352 (masked 69 -> 65, the shr/test is retail's but the definition pointer
// moves); the header mask and set as one statement 334 (same code); the final
// call's tag index and both getters written in the call (no one-use locals) 337 (first difference +0x12).
// Diagnostic (reverted): the file built with /G5 334, same code, so the unfolded addressing is not the processor target.
// MiniMax H (11-11): no new form, no lower score (334); the flag byte read through a named datum pointer 334, through
// its bitfield 334 and through a named word of flags 335 give the same code, so the base still folds into the scaled index.
// Earlier status line (334, was 334): earlier 356; frame 0xb0, 1652 bytes and entry EBP agree; effect addressing still folds,
// gate/settings homes and DL differ, blend values escape and child-descriptor allocation rotates.
// MiniMax C (11-10): the score is 334 (was 334); the gate flag keeps its dead store where retail folds the address, and the settings pointer, the effect address and the random draws still live in different homes.
// MiniMax E (11-09): no new form, no lower score (334).
// Earlier (334, was 356): frame 0xb0, 1652 bytes and entry EBP agree; effect addressing still folds,
// Earlier 334 (was 356): frame 0xb0, 1652 bytes and entry EBP agree; effect addressing still folds,
// Earlier first line: score 334 (was 356): frame 0xb0, 1652 bytes and entry EBP agree; effect addressing still folds,
// MiniMax E (11-09): no new form, no lower score (334).
// gate/settings homes and DL differ, blend values escape and child-descriptor allocation rotates.
// MiniMax E (11-09): no new form, no lower score (334).
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
// MiniMax B (11-06) round 2: no new form, no lower score (334); the permuter ran for 411 s and
// tried two forms, the only candidate exchanging the last two arguments of the 0x173fd0 call
// (which changes what the callee receives, and its declaration is in another file), so it is not
// kept (334 -> 333, one point); the first random value moved from the local record's field_98 to
// a plain real local, as the sibling 0x247fd0 does it, measures 392 at retail's 1652 bytes; the
// frame stays 0xb0 with retail's entry EBP save, and the four random-result stores and the
// escaped blend, sample and getter homes are unchanged, so the body is unchanged.
// MiniMax B (11-06): no new form, no lower score (334); the function keeps our frame 0xb0, the
// late EBP save and the escaped blend, sample, random and getter homes, and the grouped
// workspace order that lands the point, direction and collision result on retail's offsets
// stays at 355 while the 16-byte local that holds the blend and the sample together drops
// the frame to 0xa0; the body is unchanged.
// F404: Actual child-creation success/failure outcome in bool/byte/short/long,
// after descriptor refresh: 334/334/334/334/334/334/334/334. NOT kept: no exact outcome-storage form.
// Original handle admission, loop, random stream, geometry and whole body restored; lookup/blend/getter allocation remains.
// T64: Real byte-span element addressing at effect/allocation/velocity/child lookups,
// singles/effect pair/particle pair/all: seven 334s; all with named offset producers 338.
// NOT kept: separate native address adds and scalar homes are not recovered; complete
// original typed lookups, geometry, random stream, qualifier and body restored.
// P365: Real position/direction/both matrix-output workspaces, field/native then shared
// setter publication, then one sequential workspace in both forms:
// 386/384/401/386/386/430/399/415 (1681-1729 bytes). NOT kept: native calls use
// in-place outputs; frame and length move away. Original complete body restored.
// P367: Complete contact-normal value for both offset/direction consumers,
// producer/contact/geometry/entry declarations and two vector-home names:
// eight 366s, frame 0xbc and 1665/1652 bytes. NOT kept: record-copy and frame
// shape miss native 0xb0; original normal pointer and complete body restored.
// P371: Raw effect-flag input in char/signed-char/short/unsigned-short/int/unsigned-int/long/unsigned-long:
// eight 334s on the native entry-save graph. NOT kept: folded address and gate/sample/
// descriptor homes remain; original byte carrier, inherited Boolean store and full body restored.
// T68: Actual effect-presence/absence snapshots in bool/byte/short/long, reused
// for admission and inherited velocity:
// 421/421/426/429/421/421/426/429. NOT kept: no exact ownership/home repair. Original executable body restored.
// Early address/flag home and blend/getter allocation remain.
// T70: Actual flag word/bit-field extraction, staged/direct Boolean producers on native-entry graph:
// 334/334/334/334/344/334/334/334. NOT kept: no exact address/DL/flag-home recovery; original field observations,
// inherited qualifier, random stream, geometry, helpers and complete body restored.
// T72: Native-entry gate/settings/sample lexical home subsets 1/2/4/3/5/6/7,
// then all plus the raw-flag name: eight 334s. NOT kept: native DL/gate/descriptor
// homes and separate address add remain; all original names and the whole body restored.
// P392: Ordinary admission storage on the retained native-entry-save graph,
// bool/byte/short/word/int/long/dword/signed-char: 395/395/395/395/395/395/395/395. NOT kept: no native
// effect-address/gate-home equality. Original inherited local qualifier, random stream, geometry and
// complete body restored.
// M3 lane 4 r176 (fast with 0x247fd0, which stays matched; base 334 at 1652): retail's dead flag byte sits at +0x1b,
// below the definition pointer's +0x1c, ours at +0x1f above it at +0x18. The flag as a bool member at the end of the
// local record, no volatile, 351 (the member keeps its store but lands high in the frame); as the record's first member
// 351; the volatile flag renamed to sort first or last among the locals 334/334 (same code); the flag declared after
// the effect index 334 (same code). Re-measured: all four samples through one plain real 392; the flag as a plain bool
// set and then tested 395 (store gone, first difference +0x2). All restored.
// P401: Initial settings pointer reserved before collision/effect/allocation/admission,
// each with distinct refreshed terminal owner/direct terminal tag: eight 334s at 1652 bytes.
// NOT kept: no native gate/settings home or split effect-address recovery. Original
// producer lifetimes, refreshed settings, random stream and complete body restored.
// M3 lane 9 r101 (fast with 0x247fd0, which stays matched; base 334 at 1652, aligned 246, masked 106): the gate with
// no volatile and no assignment in the test keeps its dead store when the flag starts false and is set under the
// index test (`bool flag = false; if (index != NONE) flag = ...; if (flag) return;`): 351 at 1652 (aligned 249,
// masked 118; the store stays at +0x1f in AL with the folded address, so it is a score rise and not kept), with the
// bit read as the word bit-field 351 (same code), with the flag declared before the index 351 (same code); the same
// gate as a file-local inline helper returning its `result`, byte read / bit-field read 351/351; the flag as
// `index != NONE && bit` 406; the flag set in both arms of the index test 411. Retail's `add eax, edx` then
// `mov dl, [eax + 2]` and the +0x1b home (under the definition pointer at +0x1c) are still open.
// M3 lane 14 r79 (fast with 0x247fd0/0x248cc0, both kept): only the two effect-record lookups fold base+index here
// (`[eax + edx + 2]` for the flag, `lea [ecx + eax + 0x30]` for the velocity); retail computes both with a separate
// `add eax, edx`. The velocity as a default `g_4687a4` set to `&effect->velocity` from a named effect record under
// `if (local_0 != NONE)` 348 (frame 0xb4; the lea still folds).
// Retail's matched 0x249170 builds the same `index != NONE ? &record->field : default` pointer from the same pool with
// a folded `lea [eax + ecx + 0x74]`, so retail's split `add eax, edx; add eax, 0x30` here is this function's source, not
// the pool or the flags: a boundary between the record pointer and the field offset that a same-file inline getter
// does not keep (listed above, 356). Retail's 0x18cbc0 folds the same velocity lookup too (`lea [eax + edx + 0x30]`).
// m3 lane 1 r85 (fast; 0x247fd0 stays matched): an inline effect getter at both effect lookups 334 (same code, the flag
// address still folds); the definition pointer named `definition` or `system_definition` 334/334 (same code); the flag
// through an inline reader of the effect pointer returning bool 417 at 1665 B (the flag takes retail's +0x1b and the
// definition pointer retail's +0x1c, by the extra read-back of the volatile; still folded); the effect pointer from
// integer arithmetic 334 (same code); the reader returning the byte, `!= 0` in the caller, 334 (same code); the child
// reciprocal as 0x247fd0's if/else alone 334 (aligned 188 -> 179). Retail's
// sibling 0x247fd0 folds `[eax + edx]`; this one never folds base+index, so the cause is in the function, not the spelling.
// Round 228 (m3 lane 3, fast; aligned counts exact/stack-masked/register-masked in brackets, base 334 [246/231/104]):
// the 0x1697c0 query's result buffer is 0x5c bytes (matched 0x0b8ca0 declares it so); at that size retail's frame
// reads probe 0x14, direction 0x20, result 0x2c, placement 0x88, with the blend and the first draw in registers. On
// that shape (in-loop particle_object_for_tag, blend and first draw as plain locals, the result padded to 0x5c in an
// aggregate wrapper; diagnostic): workspace {probe, direction, placement} 419 (1664) [180/177/98]; all plain locals
// 419 [199/177/98] (the result renamed, declared last or as a 0x5c byte array, the placement renamed: same code);
// probe, direction and result declared in the trace block 422 [175/170/87], the placement in the allocation block
// as well 422 (same code); the placement's NONE store at the allocation 360 (same frame). Plain address-taken locals
// here sort by size, smallest lowest: a 0x20-byte result lands below the placement (frame 0x78), and six extra
// placement stores move nothing (455), so retail's result-below-placement order does not come from plain locals of
// these sizes. The else arm's sum as three field statements (a helper body by hand; diagnostic) 343 (1652)
// [241/215/93]: that gives retail's scaled-position block exactly (three products held in XMM2-4, the stores after
// the age update, the else arm adding the held values), because the stored fields are then forwarded; through
// vector3f_add_scaled, or a point-typed forceinline copy of it at both point sites, 334 (same code): an inlined
// helper's pointer reads are not forwarded. Scaling through vector3f_scale 354 [272/246/113]. On main: a named
// `point3f *position` for the record's position at every use 335 [269/236/116], with the point-typed helper at both
// point sites 335 (same code); the else arm's scale folded into the call 334 (same code). All restored.
// Round 80 (m3 lane 13, fast with 0x247fd0/0x248cc0, both stay matched): the flag byte's shift and mask as one
// statement 334, in its initializer 334 (same code each); the velocity lookup through `local_0` 358 (1634 bytes); the
// velocity defaulted to g_4687a4 and set under `if (local_0 != NONE)` without a named record 348 (the frame changes,
// first difference +0x2); the collision result declared in the trace block 334 (stack-masked aligned 169 -> 167).
// Body unchanged at 334.
// Lane 6 r167 (fast with 0x247fd0, which stays matched; base 334 at 1652, aligned 246, masked 106). The child loop written
// as matched 0x247fd0 writes it (`tag_index`/`effect_index` read into locals ahead of the 0x173fd0 call, the in-loop lookup
// through particle_object_for_tag, the link call given the DATUM sum, the reciprocal as an if/else) 311 (1692 B, aligned
// 185, masked 75); with 0x247fd0's cache test and store order as well 311 (masked 69). Match-only: the gain needs the two
// one-use locals. Grid of the four loop parts (lookup, locals, sum, if/else): 1000 371, 0100 334, 0010 335, 0001 334,
// 1001 372, 1100 333, 1010 375, 1110 321, 1101 323, 1011 376, 0111 335, 0110 335, 0101 334, 0011 335. On the 311 (masked
// 69) form: the bit-4 test through TEST_FIELD_BIT(flag4) 387, all four samples through `last_sample` 376, the record
// pointer as 0x247fd0's `&((s_248000 *)data)[index & 0xffff]` 311 (same code), the velocity as a g_4687a4 default
// overridden under `if (local_0 != NONE)` 291 (frame 0xb4, masked 78: retail tests first and stores each arm into
// local_0's slot +0x14), the velocity as an if/else 311 (same code as the ternary), the probe z as
// `local_2->field_1c.z + 1.f` 385, the blend's x row written as the y and z rows (`40.x * (1 - a) + 34.x * a`) 311
// (same code; on main's body 334, same code). Keepable parts alone: the velocity default 348 (frame 0xb4; a re-measure
// of lane 14 r79's form); with the in-loop lookup and the if/else reciprocal 331 (1696 B); plus 0x247fd0's cache test
// and store order 327.
// M3 lane 12 r113 (fast with 0x247fd0 and 0x248cc0, both kept; aligned 169, register-masked 71, ours 420 instructions
// against retail's 430 at 1652 bytes: retail folds no base+index or memory operand here). Lane 6 r167's keepable parts
// (V the velocity default, L the in-loop particle_object_for_tag, R the if/else reciprocal) with C, the cache test as
// `local_2 != arg_0->field_50` without 0x247fd0's store order: C 333, V+C 347, V+L+R+C 330 (1696 B), L+R+C 371 (1696),
// V+L+C 342 (1696); without L: V+R 348, V+R+C 347, R+C 333 (1652 each); on V+L+R+C: all four draws through one plain real
// (the record's draw member dropped) 385, the gate without the volatile or the assignment in its test 441 (1696 each).
// None reaches the 8-point bar; body unchanged.
// M3 lane 16 r106 (fast with 0x247fd0 and 0x248cc0, both stay matched; base 334, register-masked aligned 134): the cache clear
// as `field_44 = field_44 & ~0xf80f` 334 (same code); that with matched 0x247fd0's order (`local_2 != arg_0->field_50`, field_50
// stored first; aligned 131, the clear then load/and/store as retail) 346, and round 71's compound form in that order re-measured
// 346; the contact normal read from the result at both uses (no pointer local) 337; the bit-4 velocity test as `((byte >> 4) & 1)
// != 0` 334 (same code); the effect gate byte shifted and masked in one expression 334 (same code); the velocity's i row spelled as
// the j and k rows (`i = velocity->i + i`) 334 (same code). Retail also inlines the in-loop tag lookup (the tag-group compares)
// where ours calls it. Body unchanged at 334.
// m3 lane 14 r88 (fast with 0x247fd0 and 0x248cc0, both MATCH): `if (!local_3->unknown0c)` 334 (same code); the else arm's
// factor as `arg_6 * (1.f - arg_5)` 334 (same code); the field_3c store ahead of the motion branch 354; re-measured round 32's
// inline effect-record getter at both effect sites 334 (same code: the split `add eax, edx` stays folded). Diagnostics, not
// kept: global optimisation off for this body 1686 (2770 B), so the unfolded address adds and loads are not that setting; in
// the compile-only screen, local_0 or local_3 renamed to sort last moves no home. Body unchanged.
// P436: Fixed existing inherited-velocity/contact-normal receivers, child handle/record,
// reciprocal, motion weight and terminal tag/handle bindings: eight fast 334s, control 334.
// NOT kept: no native split-address/gate/settings home repair or byte change. Original
// complete body restored; frame 0xb0, entry EBP and 1652-byte extent remain correct.
// P439: Positive child rate then inverse test results in bool/byte/word/long:
// 334/334/334/334/334/334/334/334. NOT kept: one-use predicates require equality;
// split effect addressing and gate/settings homes remain. Original comparisons,
// reciprocal results, all random steps and complete body restored.
// P441: Original-site masked/copied header word/dword/short/long accumulators: 334/338/334/338/334/338/334/338.
// NOT kept: Separate effect-base addition, native DL/gate +0x1b and settings +0x1c remain. Original complete body restored.
// P444: physical lower header field 334f; physical contact bits 334f; physical failure bit 334f; pooled particle lvalue 334f; cache-change producer 334f; native terminal-slot result 334f; double-factor initial scale 377f; allocator-admission producer 334f.
// NOT kept: split effect addressing and gate/settings homes remain; original complete body restored.
// P446: Native objects on the integrated entry-save/helper graph: blend, first sample, both, probes/placement, geometry, probes/placement/sample, all, all at consumers;
// 406/395/446/465/446/446/446/447. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P448: Immutable incoming system/settings/publication/fraction/duration/scale/placement/cache bindings:
// 334/334/334/334/334/334/334/334. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// M3 lane 10 r101 (fast with 0x247fd0 and 0x248cc0, both stay matched; aligned/register-masked in brackets): nothing kept.
// Round 73's recipe rebuilt on this tree 415 [79/45] at 1652 bytes, 430 instructions as retail. On it: the gate through a named
// effect pointer read twice 243 [102/52] (431 instructions: the record address is then formed with a separate `add`, as retail,
// but with the base in EBP and the byte in AL); the motion kind read into its named short ahead of the field_08 statement 409
// [88/54] (retail's `mov ax, [edi+0xc]; test ax, ax` exactly; the field_08 product turns), ahead of the scaled stores 423
// (1666 bytes); with that, the gate through a one-byte wrapper type returned by value from an inline helper taking the record
// pointer 315 [107/62] (dead store with no volatile, address still folded, flag still at +0x1f); the effect pointer declared
// at function scope for the velocity 415 (same code); 0 to 7 unused longs ahead of the body 409 each [88-90/54-56] (the
// symbol-count phase moves two float rows and none of the address sites). Lead, not a source form: retail's body holds five
// `add reg, reg` and no base+index operand at all, while the matched sibling 0x247fd0 folds `[eax + edx]` in the very same
// inlined tag-group lookup (same three registers live) and `lea esi, [ebx + edx]` for the same child record; so the unfolded
// sites (gate, velocity, tag lookup), the short loaded before its test and the `movss` copy of field_10 are a property of how
// this one function was compiled, not of its spelling. Its caller 0x247470 only matches built without optimisation (its own
// file, on main), so the original source had an optimisation pragma region next to this function. Diagnostic (compiler
// setting, reverted at once): an `optimize("", off)` / `optimize("", on)` pair placed ahead of this function 334, same code,
// so a plain restore is exact; a state with global optimisation and frame-pointer omission on but no speed preference was
// not measured (one setting diagnostic per round) and is the next thing to try, as a build-flag question for the maintainer.
// P450: Original contact-mode request decision, positive/complementary bool/byte/short/long:
// eight 334s at 1652 bytes. NOT kept: one-consumer results lack byte equality; native
// split effect addressing and gate/settings homes remain. Original complete body restored.
// P452: Real 0x5c collision extent on native-entry code, query scope, first sample, blend, probes, placement, all entry and all producer scopes:
// 337/337/395/406/336/337/444/445. NOT kept: all regress (best 336 versus 334); no native gate/address repair. Original shared type, complete body, grouped storage, random steps, qualifier and helpers restored.
// P454: Initial scale, blend setter, inherited-velocity sum, probe copy, contact-normal setter,
// post-transform sum, age product and offset-probe constructor on native-entry code:
// 354/334/341/344/359/335/334/343. NOT kept: none improves; original grouped storage, samples,
// helpers, field observations and full body restored; native split-address/gate/settings homes remain.
// m3 lane 7 r159 (fast; aligned difflib count in brackets, control 334 [246]): the grouped workspace replaced by plain
// locals (blend, probe point, direction, placement), all four draws through `last_sample`, the result padded to 0x5c
// 444 [231] (frame 0xb0, blend and first draw in registers as retail); probe point, direction and result declared in the
// trace block 447 [207] (probe +0x24 and direction +0x30 on retail's slots); the result's point and normal read as fields
// 447 (same code); the placement declared in the final block with its NONE store there 443 [199] (result +0x3c and
// placement +0x98: retail's offsets); with the in-loop particle_object_for_tag 416 [167] at 1648 bytes. Cause found with a
// /GL harness of this file alone: real_truncate's inline asm switches VC7.1's slot ordering for the whole function (an
// `__asm nop` alone does it), and in that mode the 0x28 placement sorts below the 0x5c result whatever the declaration
// order, names, scopes or result references (only the final-block placement escapes it, but retail stores the NONE before
// the loop). The same source without asm gives retail's offsets exactly. A plain cast of these x87 values compiles to
// __ftol2 and the SSE intrinsics force an `and esp, -16` frame, so no spelling without asm reproduces retail's
// fstp/movss/cvttss2si truncation yet. A no-asm truncation form is the lead (each form above was added to the previous).
// P478: Standard link/time spans, inherited/blend producers, reverse normal/probe copies,
// complete sample multiplies and age product: 440/373/379/456/335/370/548/385.
// NOT kept: all regress; separate effect add, gate/settings homes and descriptor expansion remain; restored.
// M3 lane 10 r104 (fast with 0x247fd0 and 0x248cc0, both stay matched; diagnostics only, all restored): the one
// compiler-setting diagnostic named above, the speed preference turned off around this body, 711 (1368 B), so retail is
// speed-optimised. The truncations: in a compile-only screen VC7.1 /arch:SSE turns `(word)(long)x` and every other
// narrowing cast into __ftol2, `(long)x` stored as a long into `cvttss2si eax, [mem]` (one instruction, memory
// operand) or the SSE path when its input is already in a register, and a value already on the x87 stack into __ftol2
// even when it is stored into a 16-bit `long` bit-field. Retail's two-instruction `movss xmm0, [tmp]; cvttss2si eax,
// xmm0` after an x87 `fstp` is the asm helper's own sequence, so retail had the asm here too and the asm itself is not
// what moves this frame. The truncation as a plain C function in another file (inlined at link time) gives __ftol2
// calls 444 (1619 B); the asm helper as an out-of-line function in another file stays a call 393 (1633 B), and as a
// `__forceinline` one there is not emitted (unresolved calls). No new source form found; the forms the note lists
// cover the open leads (gate addressing, result/placement homes, in-loop tag lookup). Body unchanged.
// MiniMax I (11-14): no new form, no lower score (334): the gate's byte is still read with the base and the index
// in one operand where retail adds the index to the base first and reads the byte from it, and the flag byte and the
// settings still sit in the frame's later slots where retail keeps them at +0x1b and +0x1c.
// M3 lane 13 r93 (fast with 0x247fd0/0x248cc0, both stay matched; control 334): the gate flag renamed to sort first or last
// 334/334, with the definition pointer renamed the other way 334/334 (same code: names do not swap the +0x1b flag and +0x1c
// definition homes); dead-local phase scan on this body (n = 0..7, removed) 334/334/338/338/335/334/334/334. Diagnostic
// (compiler setting, reverted at once): float consistency on around this body 480 (1713 B), the gate and velocity
// addresses still fold, so that setting is not the cause either. Body unchanged.
// P500: Standard reciprocal/interpolation/inherited-velocity arithmetic, seven subsets, then initial-position
// products: 384/573/352/625/421/575/627/650. NOT kept: All regress; native separate effect address, gate/settings
// homes and extent are not recovered. Three matched controls remain exact. Original complete executable body
// restored.
// M3 lane 10 r108 (fast with 0x247fd0 and 0x248cc0, both stay matched; control 334): the motion kind read into a named short
// ahead of the age step 354; the child reciprocal computed ahead of particle_system_link 334 (1654 bytes); the probe's NONE
// store ahead of the probe copies 334 (other code); the parent count increment ahead of the field_3c store 335. Same code:
// the flag word update as one `(field & 0xe004) | 4` statement, the gate through a named effect pointer read once, the scale
// rows as `*= arg_7` or `field * arg_7`, the age step with the sum's terms swapped, the motion-kind short read just before
// its test, the definition index cast to short, the velocity's j and k rows as `+=`, the child's flag stored after its time.
// Retail's `xor edx, edx; mov dx, [ebp + 2]` for the flag word is also in matched 0x247fd0 (same source), so it is schedule.
// m3 lane 14 r96 (fast with 0x247fd0 and 0x248cc0, both stay matched; masked = registers and stack offsets masked,
// control 334 masked 70, 420 instructions against retail's 430): the in-loop tag lookup through particle_object_for_tag
// (retail inlines it there, as matched 0x247fd0 writes it) 371 (1696 B, 431 instructions, masked 56: the extra bytes are
// 4-byte stack displacements, the collision result and placement sitting higher than retail's); field_10's copy with a
// `(real)` cast 334 (same code; retail copies it through XMM0, 0x247fd0 through EDX); the gate without the volatile
// flag 394; with the in-loop lookup, the placement's NONE store moved into the final block 366 (1687 B); that with the
// placement as a plain local of the final block 322 (1680 B, masked 56: placement_set's `lea ecx, [esp + 0x9c]` is then
// retail's, the collision result 0x10 above retail's 0x3c because the blend and first draw still sit in the grouped
// workspace) -- not kept: retail stores that NONE before the child loop (0x247e3a), so the gain needs a statement moved
// against retail's order; the plain placement with the NONE store before the loop 371, declared beside the collision
// result 371. Retail's frame: probe 0x24, direction 0x30, collision result 0x3c (0x5c bytes; the shared type is 0x4c),
// placement 0x98, blend and first draw only in registers.
// M3 lane 13 r97 (fast with 0x247fd0 and 0x248cc0, both stay matched; control 334): `inline` on 0x137bd0's definition in
// its /Ob1 file (retail expands that lookup in the child loop) 371 at 1696 B, and link-time code generation then expands
// it at every caller: 16 of the 20 matched callers in the fast image stop matching, so not usable. The workspace, the
// collision result, the gate flag and the effect index declared in other orders (collision first, flag first, flag
// last) 334/334/334 (same code); collision first with the in-loop particle_object_for_tag 371 (1696 B). This function
// is on a link-time call cycle (through 0x246e60, 0x247070, 0x188180, 0x175bd0's file, 0x175270, 0x248970 and 0x247470),
// but its only call into the cycle is its last one, which already has retail's register convention.
// P546: Full-width effect/allocation/child-ordinal/child-handle/tag/event/flags
// domains, then unsigned effect identity: 334/334/334/334/334/334/334/334.
// NOT kept: no exact native address/home repair; separate effect-base additions,
// gate/settings homes and escaped blend/getter allocation remain. Full body restored.
// Round 125 (m3 lane 16, fast with 0x247fd0 and 0x248cc0, both stay matched; control 334): the effect index, allocated
// index, definition and gate flag named effect_index/particle_index/definition/hidden 334 (same code: names do not move the
// flag's +0x1f slot); the gate's record as `s_effect_datum const &` 334 (same code); the child loop's rate as `real const &`
// 334; the blend's two endpoints as `point3f const &` 337; the gate as one `local_0 != NONE && (flag = ...)` test 334.
// Diagnostic (compiler setting, reverted at once; the state the note above names): `optimize("", off)` then `optimize("gy",
// on)` around this function, `optimize("", on)` after, 334 with the same code, so dropping the speed preference does not
// unfold the addressing either.
// P565: Effect/allocation/child ordinal/child handle/tag/event/flags eight-byte
// logical storage, each then all: 445/450/378/338/356/354/357/540. NOT kept: native values
// are narrow and no split-address/home equality; original complete body restored.
// M3 lane 5 r191 (fast with 0x247fd0/0x248cc0, both matched throughout): the gate test without the volatile flag
// local 394 (frame moves); the first random sample in the shared `last_sample` local re-measured 392 (it only lines up
// once the gate's unfolded `add eax, edx` is also there, retail being one instruction ahead from the gate on); the
// velocity pointer through a named effect pointer inside an `if` 348 (frame moves); one function-scope effect pointer
// for the gate and the velocity 414 (1633 bytes); the gate flag as the named effect's `flag3` bitfield 334 (same code).
// Retail forms both effect addresses unfolded (gate `add eax, edx; mov dl, [eax + 2]`, velocity `add eax, edx; add
// eax, 0x30`) where ours folds them into one operand or `lea`; no pointer-returning getter exists in retail to call.
// P589: Original incoming system/settings/flags/blend/step/scale/link/cache parameter
// identities, eight independent forms: 334/334/334/334/334/334/334/334. NOT kept:
// no split effect-address or gate/settings-home recovery; complete original body restored.
// MiniMax S (11-16): no new form, no lower score (334): the gate's byte is still read with the base and the
// index in one operand where retail adds the index to the base first and reads the byte from it, and the flag
// byte and the settings still sit in the frame's later slots where retail keeps them at +0x1b and +0x1c.
// b7H (11-17) walk against retail, fast build on r830: all of 0x247950-0x247fc1 read against ours, no behaviour
// difference. Matched: gate 0x247971-0x247994, five seed steps (0x2479f6 0x247a2a 0x247a6e 0x247ab2 0x247af6),
// constants (0x45dc50, 0x45dca4 65535, 0x45dbc0 1.0, 0x45dbcc -1.0, 0x45e21c 0.005), cache test 0x247b44-0x247b74,
// scale and age 0x247baf-0x247be6, velocity default 0x247bf8-0x247c2e, blend sums 0x247c32-0x247c86 (v34*a+v40*(1-a)),
// probe 0x247cbf-0x247d2b (z + 1.0 at 0x247d15, call args), offset 0x247d34-0x247d7d, else arm 0x247dd7-0x247e17,
// child loop 0x247e4b/0x247f4b-0x247f54, reciprocals 0x247f10-0x247f3d, final dispatch order 0x247f5e-0x247fb2.
// Differences left: retail inlines the tag lookup in the child loop (0x247e81-0x247ebb) where ours calls 0x137bd0
// (same body as particle_object_for_tag): the inline form 371 at 1696 B, 378 with the two helpers below, not kept;
// retail stores the placement's NONE at 0x247e3a after that call, ours before it (no effect). Not kept: the
// velocity and position sums through vector3f_add, which this file doesn't use yet: both 342, velocity alone 341,
// position alone 335, operands swapped 342 each (no behaviour change). The scale step has no helper without
// normalize3d.h (a new include; 354 per r159). The raw reads (+0x16 bit 4, +0x78 == 8) and the volatile gate flag are lanes' score forms, left as is.
// M3 lane 8 r180 (fast with 0x247fd0 and 0x248cc0, both matched throughout; control 334; all restored): the placement's NONE
// store after the 0x137bd0 call, retail's order (0x247e3a), 334 (its slot +0x6c against retail's +0xbc). Diagnostics,
// reverted at once: the gate record's address formed in integer arithmetic 334 (same code, still folded); the file with
// /G7 in the link-time build 334 (same code; the compile-only test above was not the linked image); the gate byte
// through a volatile read 378 (1665 B, still folded). Re-measured listed forms: the flag as a member of the local
// record 351, the gate bit in one expression 334, the velocity as an if/else 334, the collision record first 334.
// Optional effect record 421, with the named flag 424, short-circuit flag write
// 334, with allocation-failure return 334. Staged evaluator mask 347.
// Shared scale for the first endpoint 340, second 338, both 342.
// NOT kept: the tied guards leave the folded effect address and the gate slot.
// MiniMax E (11-17): no new form, no lower score (334): retail computes the blend's x term as the (1 - step) product first
// and the step product second (0x247c49-0x247c58), so the 1 - step value is made before the products, while this build
// makes the step product first and the 1 - step value in between; writing the x term the other way round (the (1 - step)
// product first) 334 (the same score with other code: the three products then spill into the frame and the register
// numbers move), and the 1 - step value as a named local of its own 335. What still differs: the gate's byte is read
// with the base and the index in one operand where retail
// adds the index to the base first and reads the byte from it into a second register, the gate flag's home is at +0x1f
// here against retail's +0x1b, and the effect definition's home is at +0x18 here against retail's +0x1c.
// M3 lane 16 r134 (fast with 0x247fd0/0x248cc0, both matched throughout; control 334, aligned 246, 420 instructions against
// retail's 430): the gate through a named const effect pointer with the flag set from TEST_FIELD_BIT(flag3) as its own
// statement 344; the flag from `(effect->flags >> 3) & 1` assigned then tested 417 (1665 B); the velocity pointer defaulted to
// the global and replaced in an `if` 348 (first difference +0x2); as an if/else 334 (same code); the age step written
// `field_08 = arg_5 * arg_6 * field_0c + field_08` 334 (same code).
// AC572: Gate flag transferred by memcpy, memmove, copy or copy_backward.
// Low byte 334/460/446/446. Full word 366/442/442/442.
// NOT kept: byte memcpy folds as before. The other transfers widen the read
// or leave a call. Separate base addition and gate/definition homes remain.
// Enclosing bit-field and raw-byte admission 344/334; staged gate 418;
// plain positive gate 395; enclosing gate with combined header word 344,
// allocator-failure exit 344, query-scoped collision 344 or separate refreshed
// settings 344. The staged gate gains the two slots through an extra readback;
// the split effect address and DL load remain. Restored.
// Spatial workspace alone or with collision after the effect gate 334/334. Workspace alone or
// with collision in the allocation arm 334/334. First-draw workspace and probe collision 334.
// Post-gate workspace and probe collision 334. Allocated workspace and motion collision 334.
// Settings declared after the gate 334. NOT kept: folded address and flag/settings homes remain.
// The effect address still folds into the gate load and the gate and definition use different stack slots
// from retail. unsigned-word effect slot 349; gate byte stride 334; both typed strides 350; both unsigned-
// word slots 365; cache entry before mask clear 346; signed placement mode 334; collision miss first 334;
// child duration default first 334. NOT kept: The gate-only typed stride and the three consumer-branch
// forms tie at the retail extent. The word slot and two-lookup forms regress. The matched-neighbour cache
// order also regresses when applied here.
// Retained child count 334, delayed descriptor update 345, both in for/while 348/348;
// inline refresh with retained count 371, delayed update 338, both in for/while
// 341/341. NOT kept: delayed inline refresh recovers the gate's +0x1b slot, while the
// address remains folded, the handle/settings homes move and the body grows.
// M3 lane 7 r180 (fast with 0x247fd0 and 0x248cc0; control 334, aligned 246, register-masked 172): 0x247fd0's cache test and
// store order 346 (aligned 242: that block is then retail's, but the count rises), with the record pointer in 0x247fd0's form
// as well 346 (same code), with the scale through vector3f_scale 360 (1665 B); the age step ahead of the scale 351 (the
// scale products then store one by one); 0x247fd0's `(_random(seed, 0, 0) * 5) >> 16` draw 334 (same code). Retail copies
// the system's age with movss where this build (and matched 0x247fd0) moves it through an integer register.
// Effect addressing still folds and the gate and settings homes remain displaced.
// Word index for particle 406, child 333, gate with particle 419, gate with child 363,
// particle with child 422, velocity with particle 428, velocity with particle and child 428,
// and all four slots 428. NOT kept: child alone gains one at retail length and leaves the gate unchanged.
// Named flags low byte 334; separate pool base and slot 338; narrow named flags word 338;
// gate storage scoped to the effect arm 338; separate velocity record 338; both effect addresses 334;
// both with a short motion-mode read 334 or one prior-position weight 334.
// NOT kept: the effect address still folds and the gate and settings slots do not move to retail's homes.
// M3 lane 4 r215 (fast, control 334): the field_50 test as the matched sibling 0x247fd0 writes it (`if (local_2 !=
// arg_0->field_50) { field_50 = local_2; field_44 &= ~0xf80f; }`) 346 (that block is then exactly retail's, the count rises
// from realignment); on it: the effect datum through an inline getter function at the gate and the velocity 346 (same
// code: the gate still folds base+index); the four scalar locals renamed (effect_index/particle_index/definition/hidden
// or gated) 346 (the gate byte stays at +0x1f: names do not move it); the blend as its own point3f summed through the
// existing vector3f_add 371 (1617 B, frame 0xa4: the helper taking its address does not keep retail's slot); all four
// draws through the one sample local (the sibling's form) 412.
// M3 lane 16 r140 (fast with 0x247fd0/0x248cc0, both MATCH throughout; control 334, register-masked aligned 106): the
// parent test written `local_2 != arg_0->field_50` 333 (retail's `cmp ebp, [esi+0x50]`; one under the bar); that with
// field_50 stored before the field_44 clear 346 (masked 102: the clear becomes retail's load/and/store); the blend as its
// own `point3f` local outside the grouped workspace 406 (1601 B, frame 0xa4, masked 94: the blend then stays in XMM2-4
// across the first 0x143070 call as in retail, and the probe and direction land at retail's +0x24/+0x30, but the frame
// loses the blend's 12 bytes); the same with the position sum through vector3f_add 388 (1617 B, frame 0xa4, masked 108:
// the inlined address does not reserve a slot); the workspace split into plain locals 446 (frame 0xa0); the workspace
// as probe, direction, collision result and placement with the blend and first sample as plain locals 456 (frame 0xa0).
// Retail's 16 unreferenced bytes between the collision result (+0x3c..+0x87) and the placement (+0x98) are the open lead.
// Shared seed across real draws or through the header 351/351; shared random-state
// receiver 334; seed with a named packed header or separate sample scopes 351/351;
// with first or all real draws from the value producer 351/351; separate seed
// lifetimes for category and real draws 351. The folded effect address and gate slots remain.
// M3 lane 16 r142 (fast with 0x247fd0/0x248cc0, both MATCH; control 334), all the same code as the control: the volatile
// gate flag declared inside the gate block 334; the end block's definition re-read as `arg_2->function_1751d0()->tag_index`
// (no reassignment of local_3) 334; the three scale rows as `*= arg_7` 334; the y and z blend rows written step product
// first like the x row 334; local_3 declared at the function top and assigned in place 334. Retail also copies the
// system's age (+4 into field_10) through xmm0 where ours uses an integer register, and stores the three scale products
// after the age step through a pointer to field_1c (ESI) with y and z at [ebp+0x20]/[ebp+0x24].
// M3 lane 11 r125 (fast with 0x247fd0 and 0x248cc0, both MATCH; control 334, aligned 246): the field_02 update as one
// `(field_02 & 0xe004) | 4` store 334 (same code); the gate read, shift and mask in one expression inside the volatile
// assignment 334 (same code); the byte read, shifted and masked in its declaration 334 (same code); the gate stored to the
// volatile and the byte tested 405; the list head read into a named local, the head store, then the element's link 365
// (aligned 237: after the 0x1751d0 call the scratch registers are then retail's, ECX for the allocated index first, EAX for
// the old head and EDX for field_02, but the link store follows the head store; retail stores the link first).
// M3 lane 15 r109 (compile-only screen of this file reproduces the folded gate; each then fast with 0x247fd0/0x248cc0,
// both MATCH): the gate's byte read through a new inline helper writing the flag through a pointer, through a named effect
// pointer, through a pointer to the byte, through a named `effects` table pointer indexed, and as byte arithmetic on the
// table with `sizeof`: 334 each (same code: `[eax + edx + 2]` stays folded); an effect pointer with the flag from
// `(effect->flags >> 3) & 1` (compile-only only: an extra reload of the flag). Body unchanged.
// Native effect bitfield direct or through a record reference gives 344 in all four forms.
// Native flags masked with either velocity selection gives 334/334. Word flags with a
// bit-position test gives 417/417. NOT kept: the masks reproduce the folded address,
// and the field and word forms regress. Effect address and gate/settings homes remain.
// M3 lane 4 r229 (fast with 0x247fd0/0x248cc0, both MATCH; control 334, aligned 188, register-masked 72). The gate's
// stored bool needs no qualifier: a bool with two definitions gives retail's `shr; and 1; mov [home]; jne` by itself
// (the store is not removed). Measured: the gate through an inline helper with a result variable (`bool result = false;
// if (effect_index != NONE) result = effect->flag3; return result;`) 351; that helper with an effect pointer local, with
// a bool local for the bit as well, with the bit through the file's bool cast 351 each (same code); the gate bool as a
// plain local started false, set from the effect's bit under the index test and tested after it 351; the same with the
// byte read, shift and mask kept as on main 351; declared after the index 351 (same code); typed byte 351; with the
// blend's `1.f - arg_5` named 351. In all of these the gate's instructions are main's exactly; the 17 points come from
// six later products and sums whose operand sides flip (`movss xmm1, [mem]; mulss xmm1, xmm0` for main's and retail's
// `movaps xmm1, xmm0; mulss xmm1, [mem]`, and the three position sums loading the field first), so the qualifier on
// main acts on those sides, not on the gate. Also: the bit tested directly under `local_0 != NONE &&` 394 (no stored
// bool, first difference +0x2); a block-scoped single-definition bool 395; the age copied through a named real 415
// (1665 B). The effect address still folds in every form. Body unchanged.
// M3 lane 14 r123 (fast with 0x247fd0/0x248cc0, both MATCH; control 334): one effect pointer local assigned at the gate and
// again at the velocity site 334 (same code: both still fold base+index). A plain (not inline) effect getter folds the same in
// this file (334), is not inlined from 0x249170's /Ob1 file (453, 1617 B), and folds again when LTCG inlines it from a /O2
// file (334, plain): the unfolded address is no cross-file inline. Lead, match-only: each kept value read through a const
// reference binding gives retail's unfolded shapes one by one: the gate pointer (`s_effect_datum const *const &`) 355 (the
// gate is then retail's `add eax, edx; mov dl, [eax + 2]`, only the flag's home differs, +0x1f vs +0x1b); with the velocity
// pointer bound in an if/else 357 (`add; add 0x30`); with the definition short bound 348 (`mov ax, [reg + 0xc]; test ax,
// ax`, the definition reloaded into EAX where retail keeps it in EDI); with the bit-4 byte bound 364 (1665 B, `shr al, 4;
// test al, 1`). Retail's four sites read as values kept apart from the test or load that uses them. Diagnostic (reverted at
// once): a volatile view of the gate byte still folds base+index (378, 1665 B), so it is the binding, not volatility.
// Full 0x5c collision output with opening scratch 337, separate blend 406, separate first sample 395,
// both 444, branch-local collision/ray objects 336, branch-local result plus separate blend/sample 446,
// independent objects 444, and independent objects with a branch-local query 447.
// NOT kept: the full result alone enlarges the frame. The retail-frame forms shorten and regress.
// The effect lookup still folds and the gate byte stays off retail's slot. Original body restored.
// M3 lane 1 r135 (fast with 0x247fd0/0x248cc0, both MATCH; control 334): the cache test as `local_2 != arg_0->field_50` 333
// (retail's `cmp ebp, [esi + 0x50]`; 1 lower, under the bar); that with 0x247fd0's store order (field_50, then field_44) 346;
// with the first draw in `last_sample` (no store of it, as retail) 392. Match-only lead: the gate's effect pointer bound to a
// `const &` plus the first draw in `last_sample` 317 (1665 B, first difference +0x43: through the gate and the flag update
// only the flag byte's slot (+0x1f against +0x1b) and the definition's (+0x18 against +0x1c) differ), with the compare order
// 316, with 0x247fd0's store order too 311 (1665 B; the field_44 AND is then split as retail but the later OR splits too).
// Round 444. Position/direction/matrix receiver pointers 335/369/355; position-direction,
// position-matrix, direction-matrix 371/355/359; all pointers or references 359/359.
// Reduced control 334. NOT kept: regressions. The effect address remains folded and the
// matrix receiver exchanges the opening EBX and ESI roles.
// M3 lane 13 r129 (fast with 0x247fd0 and 0x248cc0, both matched; control 334): the effect index local `long const` 334;
// the gate byte as one `byte const` of the shifted, masked flags 334; the gate byte read as the low byte of the record's
// flags word 334 (all the same code); the velocity's effect record through the index local 358 (1634 B: the record
// address is then shared with the gate, retail re-derives it); the allocation's old head read into a local before the
// head update 365. All restored.
// M3 lane 15 r116 (fast with 0x247fd0 and 0x248cc0, both MATCH; control 334, aligned 250): the in-loop lookup through the
// existing particle_object_for_tag re-measured 371 (1696 B, aligned 215: placement and result slots sit low, so 4-byte
// displacements); the child reciprocal's two tests as `0.f < x` 334 (same code: the zero still takes XMM2 for retail's XMM1);
// the scale rows through the existing scale_particle_vector_2b 338 (aligned 275); the velocity pointer as an if/else 334
// (same code), as `local_0 == NONE ? g_4687a4 : ...` 334 (aligned 266). Compile-only: `_mm_cvtt_ss2si(_mm_load_ss(&x))` and
// `_mm_cvtt_ss2si(_mm_set_ss(x))` both give an `and esp, -16` frame, so no intrinsic replaces the asm truncation.
// m3 lane 4 r238 (fast with 0x247fd0, 0x248cc0, both MATCH; control 334, aligned / register-masked 188 / 101): the gate
// flag without the volatile, as a bool started false and set under `if (local_0 != NONE)` then tested, 351 (the gate
// code is main's exactly, store to +0x1f included: a bool with two definitions keeps its store; the rise is the scale
// rows' operand sides, which flip); the same keeping main's two local names, declared before or after the index, 351
// and 351; the bool set inside the NONE block with its own test 394; the effect record bound to a const reference with
// a plain bool 396 (`test cl, 8` form); the impact's definition looked up into its own local 334 (same code).
// Diagnostics, reverted: one, two, four probe reads of the definition after the motion branch 353 / 360 / 376; the
// impact's second definition lookup dropped 345 (1641 B). Read from retail: the definition's home (+0x1c) has four
// references (store, the motion branch's load into EDI that serves both the +0xc and the +0x16 tests, the loop's two
// loads), the flag byte's one: equal references per byte, so the two sort as a tie in this 0xb0 frame; ours reads the
// definition six times (+0xc and +0x16 tests reload it), which puts its home below the byte's.
// Named flags byte bound as an lvalue 338; with effect reacquisition, separate impact settings, sibling particle indexing,
// particle lvalue, child lvalue, or all record lvalues 338 each; all lvalues with separate impact settings 335.
// The gate still folds the effect address, and the gate and settings spill slots remain displaced. Body restored.
// Entry declarations with original assignments: allocation handle, record, both, reused sample,
// handle/sample, record/sample, all three, or those plus settings, each 334.
// NOT kept: the folded gate address, AL and the two stack slots are unchanged.
// M3 lane 4 r240 (fast with 0x247fd0, 0x248cc0; control 334): the gate through a file-local inline taking the flags word
// by const reference, the record by const reference or the record pointer by const reference, bool result: 417 each
// (1665 B; still folded, but the flag lands at retail's +0x1b once main's byte temporary is gone); the byte by const
// reference with main's steps 334 (same code); the record pointer bound by const reference at the gate 355 (with one
// expression 355), plus the first draw into `last_sample` 317 (lane 1 r135's lead), with the bit-4 test as
// TEST_FIELD_BIT(flag4) or through a definition local 317 (same code). Retail keeps the definition in EDI from the scale
// rows to the bit-4 test (four home references against six here), which is what orders its home above the flag's.
// M3 lane 15 r123 (h2m fast screen with 0x247fd0 and 0x248cc0, both MATCH each time; control 334, 245 aligned rows). Reading
// of retail: after the 0x1751d0 call it loads the allocated index (ECX) ahead of the old list head (EAX), copies the
// system's age through XMM0, and computes the three scale products and the age sum before any of their four stores (x
// through ESI, y and z at [ebp + 0x20] / [ebp + 0x24], then the age). Forms: the list link through a file-local inline
// taking the index by value (`*link = *head; *head = index;`) 334 (same code: the head is still loaded first); the cache
// test in 0x247fd0's form re-measured 346 (241 aligned), with the link helper 346 (same code); the age copy through a
// file-local inline setter taking the real by value 337 (still an integer move); all three 346; the scale through the
// existing by-value vector3f_assign 336 (the x store still leads the y and z products); the age step ahead of that 350;
// the age sum in a named real ahead of the scale rows, stored after them 334 (same code as the control), with
// vector3f_assign 350; the first draw through the same sample local as the other three re-measured 392. Body unchanged.
// With the split gate address, direct first draw 356, low-byte member and shared
// draw 317, separate gate publication 421, and bitfield with direct draw 365.
// Shared first draw with separate impact settings 317 or probe-local collision 315.
// Split gate and velocity with shared draw 307, also with separate impact settings
// 307. Removing the gate binding, velocity binding or both gives 405/317/392.
// NOT kept: the 307 forms require reference bindings and still differ at the gate
// and settings stack slots. They reach retail length and move the first difference
// to +0x43. The larger 315 and 317 forms also retain match-only bindings.
// M3 lane 8 r219 (h2m fast with 0x247fd0 and 0x248cc0, both MATCH; control 334). Reading: retail gives EDI to the definition
// pointer (loaded once after the 0x249170 call and used by the +0xc and +0x16 tests across the motion branch); ours gives EDI to
// the address of the record's +0x28 vector, shared from the second rotation call through the collision copy to the bit-4 adds,
// and reloads the definition at each test; retail re-forms that address at each site. Forms: one function-scope effect pointer
// assigned at the gate and at the velocity 334 (same code; gate only 334, velocity only 334); the bit-4 adds through the
// existing vector3f_add 341; the collision copy through the existing vector3f_assign rows 359 (1665 B); the second rotation's
// output through a named vector pointer used for both arguments 334 (same code); the kind test as a switch with case 0 and
// default 349. Body unchanged.
// Separate initial settings and impact observations with shared shift,
// byte, Boolean and field gate producers 418/404/378/378. Bound effect
// address 423, retained allocation pool 378, impact receiver with byte
// or address binding 404/423. NOT kept: all regress. The gate still
// needs the separate address, DL and its slot beside the settings.
// Scale each random sample in its own storage before native word/long/dword/int
// conversion. Separate first draw 430/430/430/430, shared draw 431/431/431/431.
// The cast still selects an x87 runtime conversion; the frame stays 0xb0,
// but the early register saves and folded gate address differ.
// M3 lane 13 r140 (H2M_FAST; control 334, difflib-aligned 188 with call targets masked): the blended position as its own
// `point3f position` local (out of the workspace aggregate, whose member stays) 390 at 1617 B (aligned 173: the blend is then
// held in XMM2-4 across the first 0x143070 call and added from the registers, as retail, so retail's 1652 B carries 35 bytes
// this body lacks elsewhere); with the workspace member dropped 406 (1601 B, frame moves). On the 390 form: plus the in-loop
// particle_object_for_tag (the sibling 0x247fd0's spelling) 359 (1648 B, aligned 150); plus the if/else reciprocal and the
// cache test as `local_2 != arg_0->field_50` 358 (1648 B, aligned 139); plus the velocity default 357 (1652 B, aligned 155);
// the if/else reciprocal and cache test without the in-loop lookup 389 (1617 B). Diagnostic (a helper written by hand, not
// kept): the else arm's add-scaled as three field statements on the 358 form 356 (1652 B, aligned 121), alone on main 343.
// Left on the 358 form: the scale rows' stores ahead of the y/z products, the 0x173ba0 argument setup, the collision call's
// argument order and homes, and the gate's folded address. Body unchanged.
// M3 lane 4 r247 (fast, 0x247fd0/0x248cc0 matched in each): the blended position as its own point3f (out of the grouped
// workspace) 406 (1601 B, frame shrinks), with the workspace member left in place (unused, diagnostic) 390 (1617 B, the blend
// sums then stay in XMM2-4 as retail); the in-loop lookup through the inline particle_object_for_tag 371 (1696 B), both lookups
// through it 371 (same code, the first stays a call); in-loop lookup plus the separate position 361, with the unused member 359
// (1648 B). Body unchanged.
// M3 lane 2 r134 (H2M_FAST with 0x247fd0 and 0x248cc0, both MATCH throughout; main 334): diagnostic, reverted at once:
// `optimize("", off)` then `optimize("gy", on)` around this function 334 (same code: no speed preference does not unfold
// the base+index sites). Forms: the gate's effect record named and read through a `const &` 355 (first +0x43: the entry is
// retail's to the flag update but for the flag byte's and the definition's homes, and the first draw's store into the
// record member then shifts the rest); the age copy through a `real const &` to the field 334 (integer move); the 5-way
// random pick in 0x247fd0's spelling 334; the allocated record indexed as 0x247fd0 does 334; on lane 1 r135's 317 lead
// (re-measured 317, 1665 B) the velocity's effect record bound the same way under an if 322 (the prologue changes). Body
// unchanged.
// M3 lane 5 r235 (h2m fast with 0x247fd0 and 0x248cc0, both MATCH each time; control 334, aligned 246): the first draw into
// its own scalar real with the workspace member kept 392 (the `fst` to the member goes, retail's draw, but the later
// homes shift; aligned 248), with the member dropped 395; that scalar draw plus the cache test as `local_2 != arg_0->field_50`
// 392; that cache test alone 333 (retail's `cmp ebp, [esi + 0x50]`, one point, under the bar); with the clear also written
// `arg_0->field_44 = arg_0->field_44 & ~0xf80f` 333 (still the read-modify-write `and [esi + 0x44]`, where retail loads,
// masks and stores). Body unchanged.
// M3 lane 15 r136 (h2m fast with 0x247fd0/0x248cc0, both MATCH each run; control 334): the three later draws each into
// a block-scoped sample local 338; the allocation list linked through a `long *` to the list head 334 (same code); the gate
// and the bit-4 test both through the bitfields (`flag3` of the effect, `flag4` of the definition) 361; the child link
// call re-deriving the child record as 0x247fd0 does 335 (1654 B); the velocity selection testing the system's effect
// index field instead of `local_0` 411; re-measured, the collision result declared ahead of the workspace 334. Body
// unchanged.
// M3 lane 6 r234 (h2m fast with 0x247fd0/0x248cc0, both MATCH each run; control 334): the scale rows as compound
// `*= arg_7` 334; the kind test as `!local_3->unknown0c` 334 (retail loads the word into AX from the definition held
// in EDI and tests it; ours compares memory through a reload); the bit-4 test as a plain `& 0x10` mask 334 (still
// `shr al, 4; test al, 1`); the blend's x row written like y and z 334; y and z written like x 334 (same code in every
// one). Body unchanged.
// M3 lane 6 r237 (h2m fast with 0x247fd0/0x248cc0, both MATCH each run; control 334, aligned/masked 188/72 by a
// difflib count): the blend as its own point3f re-measured 406 (as lane 4 r247); the 0x173ba0 call and the
// mask OR through the existing method 0x173c10 (`function_173c10(0xf80f)` on the value cache) 334 (same code: it expands
// to main's instructions); the 3-bit draw stored as an explicit `& 0x1fff | draw << 13` 334 (same code). The sibling
// 0x247fd0's spellings together (in-loop particle_object_for_tag, if/else reciprocal, `local_2 != field_50` with field_50
// stored first) with the blend as a point3f and the first draw in `last_sample`: 415 at 1664 B, frame 0xa0 [145/62]; plus
// that sibling's named tag/effect index locals and `(short)` ordinal in the 0x173fd0 call 414 at 1660 B [140/56]; plus the
// collision result padded to 0x5c (diagnostic wrapper) 412 at 1660 B, frame 0xb0 [133/54], the definition then in EDI as
// retail (home +0x18 vs +0x1c). Left on that shape: retail keeps the three scaled rows and the three blend sums in
// XMM2-4 and stores them after (the else arm's add-scaled reads the scaled rows from XMM2-4), ours stores each row at
// once; the blend sums as `position.x += field` then one aggregate store 461 (1692 B). Body unchanged.
// M3 lane 12 r183 (h2m fast with 0x247fd0/0x248cc0, both MATCH each run; control 334; body unchanged). Reading: retail
// loads the three real arguments once at the top of the scale step (XMM0/1/5) with the definition in EDI, computes the
// three scaled rows and the age sum before any of their four stores and keeps the rows in XMM2-4 for the else arm. Forms:
// the gate record indexed by `arg_2->effect_index` 335; the velocity record indexed by the local index 358 (1634 B); the
// velocity pointer started at the default and set under `if (local_0 != NONE)` 348; the velocity in an if/else with a
// named record pointer 334 (same code); the gate through a named record pointer and the bit-3 view 344; three named
// scaled reals and a named age sum stored after all four 335; the scale step through the existing vector3f_scale in place
// 354; vector3f_scale into a point local assigned as a whole after a named age 404 (1697 B); the age step ahead of the
// in-place vector3f_scale 354.
// M3 lane 2 r146 (h2m fast with 0x247fd0/0x248cc0, both MATCH each run; control 334, aligned 188, 72 with registers and
// stack offsets masked; body unchanged). Read against retail again: the age copy goes through an XMM register (`movss`
// load and store, an integer move here and in the matched sibling), the three real arguments are all loaded right after the
// 0x249170 call, and the blend's three sums stay in XMM2-4 until they are added to the position (ours keeps them in the
// workspace record). Forms: the cache block in the sibling's spelling (`local_2 != arg_0->field_50`, the owner stored
// first, then the clear) 346 (aligned 184, masked 68); the cleared flags named, the owner stored, then the flags stored
// (retail's load, mask, owner store, flags store) 346 (same code as the sibling's spelling: retail's order appears, the
// rows after it shift); the blend as three scalar reals 390 (1617 B, aligned 173: the workspace record loses its first
// member's use and the frame shrinks); the blend written inside the three sums after the rotation call 392 (1617 B,
// aligned 197); the sibling's cache spelling with the scalar blend 383 (1617 B, aligned 170, masked 65), with the blend
// inside the sums 383 (aligned 194); the named cleared flags with the scalar blend 383 (same code).
// Byte and word gate projections gave quotient parity 441/441, remainder threshold 441/441, masked quotient 334/334,
// and shift to high bit 403/403. The effect address still folds into the flag load.
// Direct C word conversions on draw one/two/three/four 403/392/381/357;
// last three 416; long-to-word on the first 403. Staging the product before a word
// conversion on the first or last three 404/416. The gate address and gate/settings
// homes still differ. The fourth-only form preserves length and raises the count.
// M3 lane GB r2 (12 compiled trials, fast control 334 at 1652 B, aligned 271 / masked 177, 0x247470/0x247fd0/0x248cc0 MATCH throughout; body restored):
// Member function s_247950::function_247950 (with caller 0x247470 calling via this) 334 (same code); dropping local_10 normal pointer 334 (aligned 271), with normal stored before |= 0xa and value24 = NONE after function_137bd0 334 (aligned 268); compound += on position/velocity + no (bool) cast + local_2 != arg_0->field_50 333 (1652 B, under bar), with blend x flipped 333; sibling 0x247fd0 child loop without one-use locals (particle_object_for_tag, (short)local_13, DATUM in link, if/else reciprocal, value24 after lookup) 376 (1696 B, aligned 207, masked 127), plus 0x247fd0 cache test/order 393 (1696 B, aligned 203, masked 120), plus compound +=/no_local10/combined header 393; dropping local_19/20/21 in final call 338; age copy after cache block 380 (1665 B); inline s_248000::set_age/scale_pos methods 337 (field_10 still integer mov); scaled point3f + next_age stored after both 386 (1681 B).
// Combined effect rejection and allocation failure in the handle, with conditional/branch/default
// assignment or a terminal failure guard. Low-byte projection 417/417/417/417; bitfield
// 344/344/344/344. NOT kept: the low-byte forms gain the two homes through a readback,
// and the bitfield forms retain the folded address and raise the score.
// M3 lane 16 r187 (h2m fast with 0x247fd0, 0x248cc0 and 0x247470, all MATCH each run; control 334 at 1652 B, aligned 246,
// register-masked 106; body unchanged). Ranking read: the definition pointer never gets a register piece here (its
// piece from the 0x249170 call to the child loop has three costed uses and comes out at a negative cost in the seventh
// pass), while the shared `&local_2->field_28` address is coloured in the third pass (49) and takes EDI from the second
// rotation call to the velocity add; retail keeps the definition in EDI over that span (the unknown0c load, the bit-4
// load, the tag index) and forms the field's address again at each use. Forms (G the bit-4 test as
// TEST_FIELD_BIT(local_3->flag4), L the in-loop particle_object_for_tag, R the if/else reciprocal, C the sibling's cache
// test and store order, V the velocity default, D the link call given the DATUM sum as the sibling writes it): G 352,
// GR 352, GV 354, GRV 354, GC 358 (1665 B), GRC 357, GCV 359, GL 390 (1696 B), GLR 391, GLC 391, GLV 393, GLRCV 390; with
// G the definition still does not take EDI. LRV 331, LRCV 327, LRCVD 318 (1696 B each), but with V the frame is 0xb4
// (first difference +0x2) and the aligned count rises (LRC 196, LRCV 221), so those drops are positional only and V is
// not retail's test-first shape: not kept. Without V: D 335 (1654 B), RCD 353 (1658 B), LD 375, LRD 376, LCD 393, LRCD
// 393 (1696 B, aligned 187, register-masked 78: the closest natural structure so far, 44 bytes long from the blend and
// draw stores). LRCV with the `(short)` ordinal, `!local_3->unknown0c` or the sibling's record index 327 each (same code).
// Typed cursor advance at the gate, velocity, particle and child lookups 443/367/442/354.
// Gate/velocity, gate/particle, velocity/particle and all three 459/446/460/462.
// Direct cursor updates at the gate, velocity, particle and both effect reads 334/348/334/350.
// NOT kept: the library advance adds calls and bytes. Direct updates retain 1652 bytes,
// but the gate still folds the address and uses AL, with its spill at +0x1f.
// The effect load still folds the base addition. Mask then shift (byte/word) 334/334;
// low-nibble threshold 403/403; rejection-path flag store 426/426; admission-first
// flag stores 440/440. NOT kept: the best forms tie and the split stores grow.
// Nullable velocity receiver by ternary/branch 353/353. Nullable gate by ternary/branch
// 421/421, each with the two nullable velocity forms 424/424. NOT kept: the nullable
// gate gives the separate address add and DL but adds a null branch, grows to 1665
// bytes and leaves the gate byte at +0x1f.
// Combined short-circuit gate/allocation test and conditional allocation result:
// byte 334/334, word mask 334/334, word shift 334/334, bitfield 344/344.
// NOT kept: the effect load and gate spill remain in AL and ESP+0x1f.
// Single failure exit with a typed byte shift, word mask, named bit or named
// low byte: 334/334/344/334. Enclosing admission with the same first three
// projections: 334/334/344. Both admission-result arms before allocation 425.
// NOT kept: the address remains folded. The result arms add 13 bytes.
// M3 lane 1 r168 (fast with 0x247fd0 and 0x248cc0, both MATCH each run; control 334): the final call's tag index read
// from a fresh getter call without re-assigning the definition pointer 334 (same code); one tag index local read before
// the child loop for both lookups and the spawn call 375 (1658 B); the age copy moved up beside the field_08 clear 390
// (still an integer move); the second rotation call through a file-local inline helper taking the record 334 (same
// code: the +0x28 address stays shared in EDI); the collision normal bound as a `vector3f const &` 334 (same code). A
// forced-ranking search over 40 single decisions found nothing under 334. Body unchanged.
// Gemini lane GB r10 (2026-11-25, 16 source trials + 1 diagnostic, fast control 334, al=164, mk=69; 0x247fd0/0x248cc0/0x247470 MATCH):
// direct `local_2->field_1c.{x,y,z} += local_2->field_28.{i,j,k} * local_11;` in the else arm recovers retail's exact 19-insn
// staged scale/age sequence and keeps all three scaled components in xmm registers into the else arm (343 at 1652 B, mk 69->59);
// with staged scale/age locals 343/343, component normal copy 352 (1665 B), compound velocity adds 343, no normal pointer 343,
// reversed else add + `local_2 != field_50` 342 (mk 58), plain 2-def gate 346, `point3f *pos` 332 (1652 B, under bar), short child index 343,
// plain blend + shared `last_sample` 425 (1633 B); combined with sibling 0x247fd0 forms (`particle_object_for_tag`, cache order, if/else
// reciprocal, short ordinal, DATUM in link) 386 (1696 B, al 122, mk 41), + plain `point3f blend` 342 (1664 B, al 86, mk 34, 428/430 insns),
// + shared `last_sample` 416 (al 102, mk 48), + unpacked workspace 411 (fr=0xa0). Diagnostic (reverted): `#pragma optimize("a", on)` 334 (same code). Body unchanged.
// M3 lane GA r12 (18 real trials, fast control 334 at 1652 B, 0x247fd0/0x248cc0/0x247470 MATCH throughout; body restored):
// `TEST_FIELD_BIT(local_3->flag4)` + plain `point3f blend` + compound `+=` + direct `else +=` + sibling `0x247fd0` forms (`particle_object_for_tag`, cache order, if/else reciprocal, `(short)` ordinal, DATUM in link) + no `local_10` hits 271 (exact 430/430 insns, al 134, mk 47, lines 346-434 lockstep) — NOT kept: frame shrinks 0xb0->0xa4 (+0x2) and length grows 1652->1664 B without 0x5c collision; + sibling loop locals 331 (al 130, mk 40, recovers `local_3` in EDI), + `switch (unknown0c)` 341, + `if/else if` `TAG_GROUP` 275 (still folds `[edx+eax]`). Keeping `fr=0xb0` on main with those clean subsets: 350/352/386. Plain locals + `s_placement` ctor at 0x247e3a + `0x5c` collision ctor recover `fr=0xb0` (+0x20), 3-load probe copy and early normal loads (396/399/368, al 135, mk 52), but `real_truncate` asm still sorts 0x28 placement (+0x3c) below 0x5c collision (+0x64). Direct `function_x82e52f` without `function_248076` constant-folds `(1/65535)*65535` (427/408/419/415); standalone `s_placement` ctor on main 334; `point3f *pos` + direct `else +=` 335; reversed blend x + direct `else +=` + NONE after 0x137bd0 343.
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
		local_2->field_02_13 = random_index(&g_4e7408->seed, 5);
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
				vector3f_assign(&local_23.field_18, 0.f, 0.f, -1.f);
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
