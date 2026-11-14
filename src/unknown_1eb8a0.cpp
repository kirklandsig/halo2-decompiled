#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_123b30.h"
#include "kill_volumes.h"
#include "normalize3d.h"
#include "vector_products.h"
#include <math.h>
#include <string.h>

// @flags /O2 /Ob1 /arch:SSE /Gr

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* a timed follow/ease state: counts ticks (counter) towards a duration */
struct s_ease_state
{
	long ticks_left;
	long counter;
	long duration;
	bool fast;
	bool active;
	bool limited;
	byte pad0f;
	point3f start;
	vector3f impulse;
	real alignment;
	real distance;
	point3f target;
	vector3f direction;
	real limit;
	bool settled;
	byte pad4d[3];
	real settle_distance;
	real settle_minimum;
	vector3f field58;
};

// @retail 0x1eb640
void function_1eb640(s_ease_state *state)
{
	state->distance = 0.0f;
	state->counter = 0;
	state->duration = 0;
	state->ticks_left = 0;
	state->fast = false;
	state->active = false;
	state->limited = false;
	state->impulse = *g_4687a4;
	state->alignment = 0.0f;
	state->target = *g_468788;
	state->direction = *g_4687a4;
	state->settled = false;
	state->settle_distance = 0.0f;
	state->settle_minimum = 0.0f;
	state->start = *g_468788;
	state->limit = 0.0f;
	state->field58 = *g_4687a8;
}

struct s_ease_output
{
	dword flags;
	byte unknown04[8];
	vector3f position;
};

real function_30bf0(vector3f *v);

struct s_direction_rotation_input
{
	byte field_0[0x14];
	dword flags;
	byte field_18[0xac - 0x18];
	vector3f direction;
	byte field_b8[0xf4 - 0xb8];
	vector3f forward;
	vector3f up;
};

PRIVATE __forceinline void rotate_direction(vector3f *vector, const vector3f *axis, real sine, real cosine)
{
	real dot = dot3f(vector, axis);
	real projected = dot * (1.0f - cosine);
	real cross_i = vector->j * axis->k - vector->k * axis->j;
	real cross_j = vector->k * axis->i - vector->i * axis->k;
	real cross_k = vector->i * axis->j - vector->j * axis->i;
	vector->i = vector->i * cosine + projected * axis->i - cross_i * sine;
	vector->j = vector->j * cosine + projected * axis->j - cross_j * sine;
	vector->k = vector->k * cosine + projected * axis->k - cross_k * sine;
}

// @retail 0x1ecf50
void function_1ecf50(vector3f *forward, vector3f *up, s_direction_rotation_input *input)
{
	if (!(input->flags & 0x40))
	{
		real dot = dot3f(&input->up, &input->direction);
		dot = PIN(dot, -1.0f, 1.0f);
		if (!(fabs(dot - 1.0f) < 0.0001f))
		{
			real cosine = PIN(dot, -1.0f, 1.0f);
			real angle = (real)acos(cosine);
			if (angle != 0.0f)
			{
				vector3f axis;
				cross3f(&input->up, &input->direction, &axis);
				if (function_30bf0(&axis) != 0.0f)
				{
					*up = input->up;
					*forward = input->forward;
					rotate_direction(up, &axis, (real)sin(angle), (real)cos(angle));
					rotate_direction(forward, &axis, (real)sin(angle), (real)cos(angle));
					function_30bf0(up);
					function_30bf0(forward);
				}
			}
		}
	}
	else
	{
		*forward = input->forward;
		*up = input->up;
	}
}

struct s_ease_input
{
	byte field_0[0xc];
	long object_index;
	byte field_10[0xbc - 0x10];
	bool update;
	byte field_bd[3];
	point3f target;
	vector3f velocity;
	real time;
	byte field_dc[0xe8 - 0xdc];
	point3f position;
	byte field_f4[0x118 - 0xf4];
	vector3f direction;
	vector3f reference_velocity;
};

bool function_109fd0(long object_index, vector3f *velocity);
long function_1eb8a0(s_ease_state *s, bool update, vector3f *a, point3f *b,
	s_ease_output *out, real dt, vector3f *d, vector3f *q, point3f *pos);

// @retail 0x1eb6f0
void function_1eb6f0(s_ease_input *input, s_ease_state *state, s_ease_output *output)
{
	s_ease_state *const *state_reference = &state;
	s_ease_output *const *output_reference = &output;
	vector3f velocity = input->velocity;
	vector3f reference = input->reference_velocity;
	vector3f object_velocity;
	bool relative = function_109fd0(input->object_index, &object_velocity);
	if (relative == true)
	{
		reference.i -= object_velocity.i;
		reference.j -= object_velocity.j;
		reference.k -= object_velocity.k;
		velocity.i -= object_velocity.i;
		velocity.j -= object_velocity.j;
		velocity.k -= object_velocity.k;
	}
	real squared = velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i;
	if (squared > 6.25f)
	{
		double scale = 2.5f / sqrt(squared);
		velocity.i = (real)(velocity.i * scale);
		velocity.j = (real)(velocity.j * scale);
		velocity.k = (real)(velocity.k * scale);
	}
	function_1eb8a0(*state_reference, input->update, &velocity, &input->target,
		output, input->time, &input->direction, &reference, &input->position);
	if (relative)
	{
		s_ease_output *result = *output_reference;
		result->position.i += object_velocity.i;
		result->position.j += object_velocity.j;
		result->position.k += object_velocity.k;
	}
}

// MiniMax G (11-14): no new form, no lower score (636): retail keeps q (the fourth stack argument) in EBX from entry
// and reads d from its stack slot at each use, this build keeps d in EBX; the first clamp still computes t before the
// 12/8 factor is selected into xmm1 (retail selects the factor first) and the xmm homes and coefficient schedules
// still differ.
// MiniMax G (11-12): no new form, no lower score (636): nothing new this round; the first clamp still computes t before the
// 12/8 factor is selected into xmm1 (retail selects the factor first), as the earlier note describes.
// MiniMax G (11-12): no new form, no lower score (636): the first clamp still computes t before the 12/8 factor is
// selected into xmm1 (retail selects the factor first), and the q/d EBX weight and the impulse's fast flag are as
// the earlier note describes.
// MiniMax G (11-11): no new form, no lower score (636); retail keeps q (the fourth stack argument) in EBX from entry and reads d
// from its stack slot at each use while this build keeps d in EBX; the frame size 0x24 and the clock register agree, but the xmm
// homes and the coefficient schedules still differ, and retail builds each zero with its own xorps where this build shares one
// function-wide zero.
// score 636 (was 636): earlier 636 (was 642); retail keeps q (the fourth stack argument) in EBX from entry and reads d from its stack
// earlier 642; retail keeps q (the fourth stack argument) in EBX from entry and reads d from its stack
// slot at each use; ours keeps d in EBX. Frame 0x24 and clock EDI agree; xmm homes and coefficient schedules remain.
// retail keeps q in EBX from entry (reloading it after the activation copies, where EBX serves
// a copy) and reads d from its stack slot at each use; ours keeps d in EBX. Frame 0x24 and clock EDI agree; the
// speed-factor selection order, direction/sample homes and coefficient schedules remain.
// MiniMax I (11-10): copying q into qr field by field (q->i, q->j, q->k) instead of one vector copy gives 636 (was 646)
// at 2880 bytes; the d slot still takes EBX where retail keeps q, and the coefficient schedule still differs.
// MiniMax h (11-05): the argument order is right (d in the third stack slot, q in the fourth, position in the fifth); retail copies the q slot into EBX and reads d from its own slot at each use, ours copies the d slot, and assigning the fallback pointer with a ternary instead of an if gives 642 with the same code, so no new form is kept.
// MiniMax A (11-04): no new form, no lower score; retail branches on the fast flag before the rate multiply and loads the
// constant there, ours multiplies first (642, same code).
// Earlier 646 (was 642; the settling-ratio fix took 642 to 646): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier 636 (was 642): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier 646 (was 642; the settling-ratio fix took 642 to 646): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier at 636 (was 642): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier at 646 (was 642; the settling-ratio fix took 642 to 646): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier at 642: retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// Earlier (642): retail keeps q (the fourth stack argument) in EBX from entry (reloading it after the
// activation copies) and reads d from its stack slot at each use; ours keeps d in EBX from entry. Frame 0x24
// and clock EDI agree; direction/sample homes and coefficient schedules remain.
// MiniMax E run 26: retail passes the state in ESI and the update flag in AL and reads its first argument from a different stack slot than the fastcall form ours compiles to; the declaration and the /Gr convention are fixed, so the prologue and every stack-relative access stay four bytes off and the direction/sample homes and the coefficient schedule still differ.
// Earlier: frame 0x24 and clock EDI agree; direction/sample homes and coefficient schedules remain.
// frame 0x24 and clock EDI agree; direction/sample homes and coefficient schedules remain.
// Earlier convention form: retail passes the state in ESI and the update flag in AL and reads its first argument from a different stack slot than the fastcall form ours compiles to; the declaration and the /Gr convention are fixed, so the prologue and every stack-relative access stay four bytes off and the direction/sample homes and the coefficient schedule still differ.
// Earlier: frame 0x24 and clock EDI agree; direction/sample homes and coefficient schedules remain.
// The stack layout agrees with the matched caller 0x1eb6f0 (out, dt, d, q, pos; state ESI, update AL, a ECX,
// b EDX); retail loads q into EBX at entry (and again after the first normalize), ours d.
// Earlier (merged stacked note, was 642 -> 642), MiniMax E run 26: retail passes the state in ESI and the update flag in AL and reads its first argument from a different stack slot than the fastcall form ours compiles to; the declaration and the /Gr convention are fixed, so the prologue and every stack-relative access stay four bytes off and the direction/sample homes and the coefficient schedule still differ.
// Earlier: Frame 0x24 and clock EDI agree; d/q homes and scalar schedules remain. Native
// Frame 0x24 and clock EDI agree; direction/sample homes and coefficient schedules remain.
// Frame 0x24 and clock EDI agree; direction/sample homes and scalar schedules remain.
// Frame 0x24 and clock EDI agree; d/q homes and scalar schedules remain. Native
// frame 0x24 and clock EDI agree; vector argument homes, scalar schedules and counter return differ.
// clock register recovered by capped-use coefficient lifetimes; vector argument homes,
// Frame 0x24 and clock EDI agree; d/q homes and scalar schedules remain. Native
// selected vector 689; selected vector arms 674; workspace role names 660; wide failure on guarded
// coefficients 671; native counter increment 665.
// Earlier: clock register recovered by capped-use coefficient lifetimes; vector argument homes,
// scalar schedules and counter return still differ; tried initial output native copy 745; normalizer
// workspace native copy 709; existing displacement scale 750; native target displacement 709; native output
// scale loops 733; native qr post clamp coefficient 719; native qr guarded coefficients 660; coefficient
// reference lifetime 739; coefficient direction lifetime 666; coefficient both input lifetimes 743.
// Earlier vector argument homes and clock differed in the 0x24 frame.
// Void return 708; native failure 716; input role names 709;
// native extra ticks 709; vector memmove 730.
// vector argument homes still differ; selected velocity pointer 685;
// Earlier: vector argument homes still differ; selected velocity pointer 685;
// activation velocity snapshot 786 (caller lost); contiguous velocity scale 660;
// fast-byte observations 660; integer remaining-tick test 699.
// Ordinary counter conversion on guarded-coefficient body 710; indexed activation displacement 667;
// interleaved displacement/sample loop 696; target-coordinate loop 699; impulse-coordinate loop 661.
// Frame 0x24 and clock EDI agree; vector argument homes and scalar schedules remain.
// Frame 0x24 and clock EDI agree; vector argument homes and scalar schedules remain. Shared output scale 660; shared sample scale 680; capped-length initializer 660; shared limit predicate 690; conditional flag store 674 (fast).
// Frame 0x24 and clock EDI agree; vector homes and scalar schedules remain. Shared
// Earlier: Frame 0x24 and clock EDI agree; vector argument homes and scalar schedules remain. Shared output scale 660; shared sample scale 680; capped-length initializer 660; shared limit predicate 690; conditional flag store 674 (fast).
// Earlier: Frame 0x24 and clock EDI agree; vector homes and scalar schedules remain. Shared
// output counter 706; computed activation alignment 710; impulse-add loop 687; active target advance loop
// 698; active target extension loop 700 (fast); matched caller preserved.
// Round 171 reduced/plain control 660: target cursor 708; direction cursor 701; impulse cursor 666;
// target/direction 735; target/impulse 711; direction/impulse 698; all state cursors 722; activation limit
// once 648 (plain, kept).
// Consistent live clock 662; normalized/remaining/both native vector values 698/660/698;
// live clock with normalized/remaining/both values 700/662/700; entry normalizer workspace 660.
// Native fallback vector assignment 642 kept; activation mode once 684; conditional coefficients 824;
// copy/mode 670; copy/coefficients 824; mode/coefficients 812; all three 807; fallback memcpy 642.
// Shared activation/direction/sample/settling dot products 642/643/676/640; all products 667;
// complete shared squared lengths 664; products with lengths 665, with initial point difference 719.
// Named acceleration terms 716; three clamp assignments 664; normalization length first 675;
// terms/clamps 763; terms/length 738; clamps/length 695; all 781; clamped headroom 751 (fast).
// Round 134: final flag-word workspace with early coefficient masks 0/1/2/3/4/5/6/7
// 648/694/716/695/648/692/716/695; original counter and caller preserved.
// Readonly input d/q/both/a/b/pos/b-pos/all contracts: eight forms 642 each.
// Initial component orders ikj/jik/jki/kij/kji 639/642/637/644/642; scale orders ikj/jik/jki 642/642/642.
// Round 324: shared ramp time 665; third time 642; ramp/third 665; remaining distance 683; ramp/remaining 682;
// third/remaining 683; all scalar workspaces 682; displacement workspace 665.
// Round 376: Continuation status at entry in bool/byte/short/long 643/643/648/647; at output
// phase 687/687/687/686. Original failure status, counter and caller preserved.
// Round 237: One rate observation everywhere 742; native integer ticks 642, both 742;
// ticks at each field use 663, with reused rate 718; rate with all left/right factors
// 742/742, native ticks and all left factors 742. Original clock scalar forms restored.
// Output displacement/scale schedules ikj/jik/jki/kij/kji 641/643/642/644/644;
// displacement-first, scale-first and scale-first per axis 642/642/642 (fast); caller matched.
// Round 78 (fast; aligned rows in brackets, baseline 757): retail keeps q in EBX (`cmove ebx` for the
// fallback) and reads d from the stack at each use, and selects the 12/8 factor before t in all three
// clamps. `*(counter == 0 ? g_4687a4 : q)` 686 [787] (gives the cmove, d keeps EBX); factor before t in
// all three clamps 743 (the first one loads s->fast into CL in the first pass, b leaves the register
// arguments and the matched caller 0x1eb6f0 breaks); in the third only 642 [749]; in the second and
// third 670 [559] (2864 bytes, caller kept); third plus the pointer select 686 [779].
// Round 380: Three-use capped-time helper with scaled/source input and state-field/Boolean mode: joined result 642/680/680/680; terminal results 685/684/684/684. Original phases restored; caller matched in every trial.
// Round 384: complete output-phase direction/target/duration/direction-target/direction-duration/
// target-duration/all state observations 690/693/667/731/704/711/736; all with the reused counter 756.
// Every snapshot and original body restored; exact caller preserved throughout.
// Round 168: Shared initial difference/impulse addition/direction refresh in seven subsets
// 679/644/665/664/662/677/648; all plus initial vector assignment 657. Assignment alone
// and with initial difference/addition/refresh 644/676/659/664. Original snapshots, math,
// counter result, qualification and exact caller restored.
// Round 268: Shared two-phase rise/fall/count workspaces, seven subsets, then all with the remaining-
// time workspace: 642/642/642/642/642/642/642/683. Original body restored.
// Round 189: original velocity observed at the first output producer, native at-use/entry
// value and output readback with conditional/native zero 661/661/687/676; each with the
// shared output dot 670/670/672/668. Original observations, math and counter result restored.
// Round 290: Native dword/word/byte/signed-byte completion fields with original result arms all
// 642; direct ordered completion stores 744/648/646/646. Original executable source restored.
// Round 427: reuse the existing capped length for distance publication, with shared-cap
// predicate/complete step arms/complete count arms masks 0..7: 667/705/671/711/701/722/704/708.
// Original selection, predicates, count producers, observations and complete body restored;
// exact caller preserved in all eight trials; direction/sample homes remain.
// DL202: Shared duration-count producer with joined/terminal/conditional results and remaining-distance
// initializers 703/708/681/703/681/708; native output arguments 642/708.
// Complete supplied-direction native/const/setter/CRT observations 731/731/743/731,
// frame 0x2c and caller loss in each. Original 0x24 frame, inputs, qualifiers and caller restored;
// direction/sample residence and coefficient schedules remain.
// DL210: native active/limited/settled byte declarations, masks 1-7, then all with fast; existing completion bit helper in value/arm forms;
// scores 642/649/649/642/642/649/649/649/642/642. Original source forms restored.
// Output sample/displacement/normalizer declarations before output acquisition, seven subsets 642 each;
// all with the activation workspace declared at entry 642. Original initializers and complete body restored.
// Round 259: ordered complete activation/direction/sample dot accumulators, subsets 1..7,
// then all with the settling accumulator: 671/642/681/665/683/642/681/681. Original sums,
// inherited counter qualification, body and exact caller restored; direction/sample homes remain.
// Round 129 (lane 5, fast, caller checked): both activation length sums written j, k, i 642 (same x87 order:
// it follows the delta components' first assignment); the first clamp's 12/8 factor before t 718 (2861 B,
// caller lost); the first clamp as PIN with the cap inline 761 (2914 B); the activation target through a
// point-along-vector helper 668 (2883 B), with the direction refresh through vector3d_from_points3d 670
// (both blocks then store through eax/edi as retail, EBX unchanged); both direction refreshes through
// vector3d_from_points3d 665 (2888 B).
// Round 326: complete sampled input at its projection producer, working/separate values and
// branch/conditional zero fallback 685/706/689/689; those graphs with early second/third factors
// 651/663/678/678. Original observations, expression trees and body restored; direction/sample homes remain.
// Round N274: Existing scaled vector addition at ramp/first-settling/second-settling output
// producers, subsets 1..7, 677/642/677/642/677/642/677; all with shared output displacement 677.
// Original sample observations, output math, inherited counter qualification and body restored.
// Round 66 (lane 9, fast, caller 0x1eb6f0 matched in each): d read through `vector3f *const *` at every use 673
// (d still takes EBX); q left unchanged with the fallback selected into a separate pointer for the copy 662; the
// output position copied through `out->position` before the position pointer 642; the w dot before the counter
// read 642; the counter conversion without `volatile` 671 (2860 bytes).
// Round 25 (lane 18, fast; aligned counts by difflib in brackets, original [757]): the first clamp's speed
// factor selected before t 718 [630], caller 0x1eb6f0 0 -> 22 (the factor byte takes CL, so a leaves ECX and
// b goes to the stack); the first two clamps so 744 [601], caller 22 (our s->fast load is CSE'd from the first
// clamp to the duration; retail loads it three times, AL/CL/AL); s->limit read at each use (retail reads it
// four times) 689 [734]; with the second clamp's factor first 719 [581]; with the output clamp's too 719
// [573]. Callers kept in the last three. Original body restored.
// Round 277: standard minimum/maximum/alignment-pin families, subsets 1..7,
// 718/752/751/829/868/895/880; all with failure constant first 880. Alignment-pin
// graphs also lose the exact caller; original body, helpers and 0x24 frame restored.
// Round N284: Ramp-admission orders settled-ticks-distance/distance-settled-ticks/
// distance-ticks-settled/ticks-settled-distance/ticks-distance-settled 642/696/696/695/696;
// nested settled/distance/ticks gates 642/696/695. Original sample, comparisons,
// counter result and complete body restored; exact caller preserved throughout.
// Round N274: Existing scaled vector addition at ramp/first-settling/second-settling output
// producers, subsets 1..7, 677/642/677/642/677/642/677; all with shared output displacement 677.
// Original sample observations, output math, inherited counter qualification and body restored.
// DL225: alias-safe library direction copies at activation/refresh/angular fallback,
// masks 1..7: 700/716/718/700/718/706/728; all with initial output copy 755. All restored;
// frame 0x24 and clock EDI agree, but direction rather than sample still takes EBX at +9.
// Round 235: Sample value observed before update, native/const/library/components
// 758/758/758/774; conditional zero fallback 750/750/750/776.
// All lose caller 0x1eb6f0; frame 0x24 remains, incoming sample/update and clock roles change.
// Original observation points, counter result, helpers and complete body restored.
// DL228: native output-record return with joined/terminal exits, direct output and field-counter
// subsets 641/641/696/696/643/643/699/699; direct-output graphs lose the exact caller.
// One-point reductions rejected; original counter result, output pointer and body restored.
// DL230: native complete squared lengths at activation/remaining displacement/sample, seven subsets: 680/642/668/642/680/642/666/670.
// Last form also names the complete direction dot at its comparison; original expressions restored.
// DH277: complete native component-square products at activation/displacement/sample sites, subsets 1-7,
// 683/642/671/642/683/642/671; complete ordered sums 668. Unlike whole squared-length names, all product
// terms were explicit; all restored with original 0x24 frame, inputs, counter qualifier and exact caller.
// Round 60 (m3 lane 1, fast, caller 0x1eb6f0 checked): dot3f for the three dot products with d 641 (2872 bytes, d
// keeps EBX), with the operands reversed 641, with q's too 667; alone at the activation alignment / the 0.866 test /
// the settling align 642/643/640; the speed factor selected at the top of the activation block 671, before t with the
// clamp as one ternary chain 716, before t with the cap at both uses 718 (all three lose the caller: the fast byte takes
// CL, so a leaves ECX); diagnostics (reverted forms): every use of d through a `vector3f *const &` 673, d's pointee
// through a const reference 686 (d keeps EBX in both). Retail reloads q into EBX after the first normalize call (EBX
// is a copy scratch in the activation block) and reads d from its slot at each use.
// DL242: Actual input-reference contracts d/q/d+q/a/b/position/d+a+b+position/all
// 642/686/686/642/642/642/642/686; q initializes the real zero-counter sample value. Canonical
// update == true 643, paired with first coefficient before its time product 719 and matched caller
// loss. Original formals, sole call, fields, coefficients and complete body restored;
// direction/sample residence remains.
// M320: Existing vector setter at activation/refresh/angular fallback subsets
// 1/2/4/3/5/6/7: 659/667/645/651/643/651/652; all plus initial output setter 667.
// All restored; original direction/sample homes and 0x24 frame remain; exact caller preserved.
// Round 382: Complete angular direction-admission predicate, bool/byte/short/long
// outside truth then inside complement: 642/642/642/642/642/642/642/642. Original dot/test, fallback and
// full body restored; direction/sample homes and coefficient schedule remain.
// Round 42 (m3 lane 2, fast, caller 0x1eb6f0 checked): the output copy from q moved to the entry 732 (2884
// bytes, caller 0 -> 31); only the position pointer declared at the entry 718 (2895 bytes, caller 0 -> 11); the
// 0.866 test's products written d first 642 (same code); the alignment dot written a first 642 (same code); the
// zero-counter fallback as `vector3f qr = counter == 0 ? *g_4687a4 : *q;` (q not reassigned) 686 (2857 bytes,
// caller kept). Original body restored.
// Round 81 (m3 lane 12, fast, caller checked): the first length sum written i, j, k 642; the delta computed before
// the scaled alignment 641 (one point); the 12/8 factor after t, before the 0.001 test 666 (2871 bytes, caller
// 0x1eb6f0 22); v as `PIN(s->alignment * 1.5f, 0.f, 2.5f) * 0.4f` after the delta 681; the step clamp as one
// ternary 645. Original body restored.
// Round 129 (m3 lane 8, fast; caller 0x1eb6f0 checked each time): retail reloads q into EBX after the
// update block's direction normalize (q is its register variable, clobbered and reloaded), d from the stack.
// The first clamp as `if (0.001f > t) e = 0.001f; else ...` 704 (2880 bytes); the first step as one ternary
// 645; the output position copy moved to function entry 732 (caller 31); a separate source pointer for the
// counter-0 fallback 662 (2871); the alignment dot written i, j, k 642 (same code). Original body restored.
// Round 83 (m3 lane 12, fast, caller 0x1eb6f0 MATCH in each): the step clamp as one ternary again 645 (a repeat);
// the cap as `k * globals->rate` 642; the duration test as `t >= 0.001f` 642; `fail` declared before `globals`
// 642; the first count as one ternary 683 (2877 bytes); both 12/8 factors as `k = 12.f; if (!s->fast) k = 8.f;`
// 642. Original body restored.
// Round 59 (m3 lane 13, fast, caller 0x1eb6f0 MATCH in each): retail's q is EBX from entry but unused until the update
// block's integer copies borrow EBX, then reloaded after the refresh normalize and kept for the output copy, the dot
// and `cmove ebx, [g_4687a4]` for the counter-zero fallback (ours stores the fallback to q's stack slot). The whole
// `if (update)` body as a forceinline helper taking d 655 (2872 bytes; d still EBX, the parameter is coalesced); the
// fallback as `q = counter == 0 ? g_4687a4 : q;` 642 (same code); the failure flag initialised after the update block
// 687; d and q as const pointees (definition and declaration) 642 (same code); the settle alignment dot before the
// first-settle block 653; the fallback dot with q's components first 642 (same code). Original body restored.
// Round 50 (m3 lane 15, fast, caller 0x1eb6f0 matched in each): the distance stored as the clamped length
// local 667 (2889 bytes); the step factor as `n * v * rate` 642 and `n * rate * v` 642 (same code); the limited
// flag computed first and the clamped length taken from it 722 (2858 bytes); the duration sum written count
// first 642 (same code). Retail's EBX holds the next stack parameter at entry (ours the one before it).
// DL258: Shared completed-activation admission in bool/byte/word/long form
// 642/642/663/665; complementary admission 652/652/683/684; joint direction/sample
// formal role names 642/642. Native sample EBX ownership at entry still differs;
// all other calibrated scores unchanged. Original body restored.
// DH353: Existing double-factor publishers at displacement/impulse/all tick scales,
// subsets 1/2/4/3/5/6/7: 716/716/724/783/783/766/817; complete sample graph 830.
// NOT kept: native SSE/frame/extent disagree; original products and caller restored.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 642 [757]): rate constant selected before the multiply at the first
// site 718 (2861) [630]; both sites 744 (2855) [601]; cap computed ahead too, first site 710 (2858) [622], both sites 668
// (2864) [597]. Each of these loses the matched caller 0x1eb6f0 (22). `t >= 0.001f` for `!(0.001f > t)` 642, same code.
// AC538: Complete settling-alignment admission, positive/complementary bool/byte/short/long:
// 642/642/642/642/642/642/642/642. NOT kept: unmatched decision/storage forms; original complete body
// restored.
// Native sample/direction homes and coefficient schedules remain.
// Round 148 (m3 lane 6, fast; aligned in brackets, control 642 [757]): rate constant selected before the multiply at the first
// site 718 (2861) [630]; both sites 744 (2855) [601]; cap computed ahead too, first site 710 (2858) [622], both sites 668
// (2864) [597]. Each of these loses the matched caller 0x1eb6f0 (22). `t >= 0.001f` for `!(0.001f > t)` 642, same code.
// Round 166 (m3 lane 4, one-file LTCG harness with 0x30bf0's and 0x109fd0's files, exact at 642 and caller 0; aligned/shape in
// brackets, control [482/185]): the 12/8 factor selected first loses the caller only because that first-site flag read is CSEd
// with the later reads and so takes ECX, pushing a/b out of ECX/EDX. The impulse written through a pointer (`vector3f *impulse =
// &s->impulse`, or vector3f_scale) kills the CSE: retail's al/cl/al reads, caller 0. On that: all three sites as PIN 656 (2873)
// [387/167]; site forms write/pin2/capfirst (best 1/2/3), `s->limit` read at each use, delta before v, target pointer 704
// (2870) [371/135] (the current body 642 has the wrong shape; keep this as the lead for a whole rewrite); the factor first with no
// pointer write: tern/if/if-else with write/PIN/capfirst, a bool local, `== true`, `!= 0`, `!`, factor at the block top, all
// 671-757, caller 22. Up/down products named or with a shared half 699-737; a shared direction helper 646, with target 662;
// direction pointer locals and dot3f at the 0.866 test no gain. Without the pointer write a descent on the positional score gets
// only delta before v 641 (same shape), `q = counter == 0 ? g_4687a4 : q` 641, w as one expression 641. Diagnostic: 8 extra q
// reads flip EBX from d to q, so that home is a matter of weight; retail's reference counts for it are not yet explained.
// Round 74 (m3 lane 10, fast, caller 0x1eb6f0 MATCH in each): second step count as a ternary 640 (2881 bytes, a gain of 2, not kept); both as ternaries 681 (2880). A one-file harness screen of 127 plain spellings (ternary and if both ways, nested tests,
// compare forms, commutative operands, constant declarations swapped) found none lower; commutative operand swaps change nothing, the sums and products are canonicalised.
// Same round, fast: `s->distance` as if/else 671 (2884); duration with its 7/1 as if/else 680 (2891); limit test as nested ifs 642 (2878); `rem = rem < 0.f ? 0.f : rem` 642 (2878); `s->limited = false; if (limit != 0.f) s->limited = len2 > limit;` 687 (2883).
// Round 65 (m3 lane 2, fast, caller 0x1eb6f0 MATCH in each; control 646 at 2875 bytes after the settling-ratio fix,
// difflib aligned/register-masked in brackets [754/305]): delta scaled through vector3f_scale(&delta, dt / len, &delta) 678
// (2893) [662/285]; the impulse through vector3f_scale(a, f, &s->impulse) 704 (2896) [636/269]; both 669 (2886) [700/288];
// impulse helper + the 12/8 factor selected before t at the first site 700 (2886) [629/259], first two sites 702 (2873)
// [547/255], all three 702 (2873) [545/247]. Lead: that last form has retail's k-before-t head and keeps the caller, but the
// zero's xorps moves to the entry (a one-line shift) and the limit is spilled; d still takes EBX (those three re-measure round
// 166's write + factor-first forms on this base; delta through vector3f_scale is the listed existing displacement scale).
// `if (!s->active && dt > 0.001f)` as one test 646, `s->active == false` 646 (same code each); the final flag test's arms
// swapped 646 (same code); `s->ticks_left = 0` before `fail = true` 646; the counter bumped in place and read back
// (`s->counter++; long new_counter = s->counter;`) 643 (2877 bytes: retail's `inc edx; mov [esi+4], edx; mov eax, edx`);
// void return alone 646 (same code); void with the read-back 642 (2877; `out` then goes to EAX before the flag update, as
// retail, whose EAX at `ret` is `out`: retail returns nothing, and the only caller ignores the result); that with `s->duration
// + 6 > new_counter` 642; the listed second-count ternary on this base 644, with void and the read-back 640 (2880 bytes).
// Lead: the void/read-back tail is retail's but for duration + 6 in ECX (retail EDX); the gain is under the keep threshold.
// MiniMax I (11-10): the fallback through a named pointer local (the parameter left unwritten) 666, the same fallback spelled at each component 649, the position copy written field by field 648 and the w*rate multiply moved after the qr reads 638; both builds lay the arguments out the same (out E+0x04, dt E+0x08, d E+0x0c, q E+0x10, pos E+0x14) and both reuse the pos slot for the fail byte, so the whole head difference is that the prologue copies d into EBX where retail copies q, and the fallback can only be retail's cmove once EBX holds q.
// M3 lane 8 r150 (fast, caller 0x1eb6f0 checked; base 636 here, aligned 685, register-masked 278): retail keeps the limit in
// no frame slot (ours stores it at +0x10) and copies `s->start = *pos` through EBX and EDI before reloading q. Tried: the
// impulse through the existing vector3f_scale 675 (2896 bytes, aligned 588, masked 233; re-measure of round 166's lead, the
// positional count rises); `vector3f qr = *q` as one aggregate copy 646 (aligned 760); the output position written
// through `out->position` with no pointer local 705 (caller 0x1eb6f0 lost, 103); `s->limit` read at every use with no
// limit local 667 (2882 bytes, aligned 670), with the vector3f_scale impulse 694 (aligned 588) (both re-measures of round
// 166's list); the inactive and duration tests as one `!s->active && dt > 0.001f` 636 (same code); the 0.866 test with
// its sides swapped 636 (same code); the counter read from the state at each use (no counter local) 636 (same code).
// Round 181 (m3 lane 4, fast, caller 0x1eb6f0 MATCH in each): the impulse through the existing vector3f_scale 675
// (2896 bytes); with the 12/8 factor selected before t at the first site 679 (retail's al/test/movss/jne/movss head,
// caller kept); at all three sites 666 (2883; step, the 0.001 constant and zero still in other xmm registers, d still
// EBX); on that, s->limit read at each use (no limit local) 705 (the constants and b move to the prologue), alone on
// the current body 667; the three clamps as PIN(t, 0.001f, rate * k) 664, the first only 717.
// Round 68 (m3 lane 15, fast, caller 0x1eb6f0 checked): the 12/8 factor selected before t in all three clamps 695
// (2867 B, frame 0x20, caller 22); the time globals read at each use (no globals local; retail reloads g_510c54 in
// the non-update part) 634 (2883 B, caller matched; gain 2, not kept); on that, the third clamp's factor before t3
// 651, the counter conversion without `volatile` 703, the counter-stage dot with q first in the j and k products 634.
// Round 168 (m3 lane 5, fast, caller 0x1eb6f0 checked in each): the output stage's three `goto done` and the label
// written as nested if/else with `fail = true; s->ticks_left = 0;` in each failing path 636 (byte-identical code, kept
// as a goto removal); qr computed as `q->i * globals->rate` at its scaling statements with no separate copy 694 (2851
// B). Retail loads d from its slot at five sites (alignment dot, activation fallback, active fallback, the 0.866 test,
// settle align) and q into EBX at entry; diagnostics: the settle align without d 753 (frame 0x1c), the post-0.866
// fallback from `a` 689 (b then takes EBX, caller 2), the alignment without d 757 (d keeps EBX). Already listed (not
// re-run): factor-first clamps, the impulse through vector3f_scale, the void return with the read-back,
// the counter conversion, qr copies, dot operand orders, the distance from len3 and the limited flag first.
// Later the same round: the settle copy `nq` written field by field 628 at 2886 B, positional only (`!!` lines 635 ->
// 623 with the length 5 bytes long, aligned 478 -> 479, masked 173 -> 174; retail copies nq with integer moves; not
// kept); the target fallback `s->target = *pos` field by field 692 (2878 B), `*position = qr` field by field 687 (2866
// B). On that 628: `s->start` field by field 669 (2881 B); the output copy from q field by field 653 (2882 B, the listed
// form); the settle length from nq 628, the 0.866 test with its sides swapped 628, `lo = MAX(0.f, lo)` 628, `rem =
// MAX(0.f, rem)` 628 (same code each; the last repeats round 74's ternary clamp). On the 636 body: the output flag
// through the existing setter `function_0bfe20(&out->flags, 2, fail || ...)` 636 (different code, same score).
// M3 lane 16 r108 (fast, caller 0x1eb6f0 checked; m = register-masked aligned count, this body m 296): on the time
// globals read at each use (no globals local, round 68's 634 re-measured: retail reloads g_510c54 in the output stage):
// `vector3f qr = *q;` as one aggregate copy (retail copies q with integer moves) 642 (2878 B, m 306); the 12/8 factor
// selected before t at all three clamps 712 (2861 B, caller 22); that with the impulse through the existing
// vector3f_scale 703 (2883 B, m 272, caller 22); the same with the first clamp back to t first 722 (2881 B, caller 22);
// the impulse through vector3f_scale alone 680 (2899 B, m 261, caller kept). With the globals read at each use, the
// impulse helper no longer keeps the caller with factor-first clamps (it did on round 166's globals-local base).
// Round 170 (m3 lane 5; plain unless marked fast, caller 0x1eb6f0 MATCH in each; base 636, `!!` 635, aligned 478,
// masked 173): `dist` through the existing inline distance3d(pos, &s->target) 637 (aligned 485; ours already computes
// dist on x87 as retail, and distance3d sums (i2 + j2) + k2 with an fxch, its out-of-line 0x3ea30's order, where
// retail keeps (k2 + j2) + i2); the e3 test as `0.001f < e3` 636 (fast, same code); diagnostic, arms swapped (not
// keepable): `if (0.001f >= e3)` with the settled block first, retail's constant-first compare and fall-through,
// 643 (fast, 2883 B; aligned 465, masked 170); the two activation lengths through the existing magnitude3d 672
// (2883 B; aligned 440, masked 157; the zero's xorps moves to the entry), all three lengths so 671 (2882 B; aligned
// 458, masked 175). d keeps EBX in every one.
// Round 154 (m3 lane 8, fast with the file's 17 matched functions; base 636, difflib aligned 478, register-masked
// 173): the two stacked `// score` lines below the first were relabelled "Earlier at" (text and numbers kept). Read
// again against retail: the first clamp selects the 12/8 factor into xmm1 before t and keeps e in the 0.001 register
// (xmm3); ours computes t first. Every natural form for that (factor first, PIN, the impulse pointer that kills the
// fast-flag CSE) and for the q/d EBX weight (aggregate and field copies of q, per-use reads, dot helpers, the fallback
// spellings) is listed above. Combinations of listed forms (fast, aligned/masked in brackets): the time globals read
// at each use (no globals local) re-measured 634 [489/173]; that with the 12/8 factor selected first at all three
// clamps and the impulse through vector3f_scale 703 (caller 0x1eb6f0 lost, 22) [432/135], plus `s->limit` read at
// each use 720 (caller lost) [424/133]; factor first + vector3f_scale alone re-measured 666 [405/137] (caller kept),
// with `s->limit` at each use 705 [403/139]. In the output arm retail expands the nested MIN/MAX for the step
// fraction without CSE, as ours does, but builds each zero with its own xorps (ours shares a function-wide zero in
// xmm6) and stores one intermediate (e3 - MAX(0, w)) to the stack. Body unchanged.
// M3 lane 12 r120 (fast with the file's 17 other functions and 0x30bf0, control 636, aligned 677, register/stack-masked 278,
// caller 0x1eb6f0 MATCH): the capped-time block (t, the 0.001 floor, the rate cap) as a forceinline helper taking the 12/8 factor
// as an argument, so each call site selects the factor ahead of t as retail does: all three sites 697 (2865 bytes, aligned 625,
// masked 254, frame 0x20, `ret 0x18`, caller 22); the first site only 716; the first two 698; the output site only 636
// (same score, masked 270, caller kept); arguments as (factor, distance) at all three 697 (same code). It is the listed
// factor-first form in another spelling: the first site's flag read still takes CL and moves `a` out of ECX. None kept.
// Round 101 (m3 lane 1, fast, caller 0x1eb6f0 MATCH in each): retail spills q around the activation block (EBX serves
// the `s->start = *pos` copy there and q is reloaded from its slot after the 0x30bf0 call), so its EBX q is a split
// live range. The output pointer declared after `out->position = *q` 636 (same code); the activation alignment as a
// named local for the store and `v` 672 (2883 bytes); the first step count as a conditional expression 658 (2879
// bytes); the duration sum as `n2 + (fast ? 7 : 1)` 636; the active block's `w` pin and scale as one expression 636
// (same code); re-measured round 168's first `rem = MAX(0.f, rem)` 636.
// m3 lane 6 r178 (fast with 0x1eb6f0, matched throughout): what decides d over q for EBX, by diagnostics (reverted):
// six fewer d reads (settle and 0.866 dots off d) d still EBX (641, 0x20 frame); no reassignment of q 678 and six
// extra top-level q reads 718, d still EBX in both; the parameters renamed to sort the other way (aq, zd) 636 (same
// code: names do not break it); no d in the activation block: no EBX load at all (726). So neither reference
// weight, names nor q's reassignment gives q the register. Forms: `q = counter == 0 ? g_4687a4 : q;` 636;
// `if (!counter)` 636 (same code: ours branches and stores q's slot where retail does `cmove ebx`); the output
// copy from q written per component 648 (2876 bytes); the 0.866 test with the constant on the right 636; `fail`
// declared before the time globals 636. Body unchanged.
// @retail 0x1eb8a0
long function_1eb8a0(
	s_ease_state *s,
	bool update,
	vector3f *a,
	point3f *b,
	s_ease_output *out,
	real dt,
	vector3f *d,
	vector3f *q,
	point3f *pos)
{
	s_game_time_globals *globals = g_510c54;
	bool fail = false;

	if (update)
	{
		if (!s->active)
		{
			if (dt > 0.001f)
			{
				real limit = s->limit;
				real step = dt;
				if (limit != 0.f && step > limit)
					step = limit;

				real t = step * 0.375f;
				real e = 0.001f;
				if (!(0.001f > t))
				{
					real k = s->fast ? 12.f : 8.f;
					real cap = globals->rate * k;
					e = t > cap ? cap : t;
				}
				real third = e * 0.33333334f;
				real up = e * 4.f;
				real down = (e - third) * 3.f;
				real rem = step - up * 0.5f - down * 0.5f;
				if (rem < 0.f)
					rem = 0.f;
				real n = 6.f;
				if (e > 0.001f)
					n = rem / e + 6.f;

				s->alignment = d->k * a->k + d->j * a->j + a->i * d->i;
				real v = s->alignment * 1.5f;
				vector3f delta;
				delta.i = b->x - pos->x;
				delta.j = b->y - pos->y;
				delta.k = b->z - pos->z;
				v = PIN(v, 0.f, 2.5f);
				v = v * 0.4f;

				real len = (real)sqrt(delta.i * delta.i + delta.k * delta.k + delta.j * delta.j);
				if (len > 0.001f)
				{
					real scale = dt / len;
					delta.i = delta.i * scale;
					delta.j = delta.j * scale;
					delta.k = delta.k * scale;
				}

				real f = globals->rate * n * v;
				s->impulse.i = a->i * f;
				s->impulse.j = a->j * f;
				s->impulse.k = a->k * f;
				delta.i = delta.i + s->impulse.i;
				delta.j = delta.j + s->impulse.j;
				delta.k = delta.k + s->impulse.k;

				real len2 = (real)sqrt(delta.i * delta.i + delta.k * delta.k + delta.j * delta.j);
				real len3;
				if (limit != 0.f && len2 > limit)
					len3 = limit;
				else
					len3 = len2;

				real t2 = len3 * 0.375f;
				real e2 = 0.001f;
				if (!(0.001f > t2))
				{
					real k2 = s->fast ? 12.f : 8.f;
					real cap2 = globals->rate * k2;
					e2 = t2 > cap2 ? cap2 : t2;
				}
				real third2 = e2 * 0.33333334f;
				real up2 = e2 * 4.f;
				real down2 = (e2 - third2) * 3.f;
				real rem2 = len3 - up2 * 0.5f - down2 * 0.5f;
				if (rem2 < 0.f)
					rem2 = 0.f;
				real n2 = 6.f;
				if (e2 > 0.001f)
					n2 = rem2 / e2 + 6.f;

				s->limited = limit != 0.f && len2 > limit;
				s->start = *pos;
				s->distance = s->limited ? limit : len2;
				s->active = true;
				s->duration = (long)((s->fast ? 7 : 1) + n2);
				if (len2 > 0.001f)
				{
					real r = s->distance / len2;
					s->target.x = delta.i * r + pos->x;
					s->target.y = delta.j * r + pos->y;
					s->target.z = delta.k * r + pos->z;
				}
				else
				{
					s->target = *pos;
				}
				s->direction.i = s->target.x - pos->x;
				s->direction.j = s->target.y - pos->y;
				s->direction.k = s->target.z - pos->z;
				if (function_30bf0(&s->direction) == 0.f)
					s->direction = *d;
			}
		}

		if (s->active)
		{
			if (!s->limited)
			{
				s->target.x = s->direction.i * dt + pos->x;
				s->target.y = s->direction.j * dt + pos->y;
				s->target.z = s->direction.k * dt + pos->z;
				real w = PIN(s->alignment, 0.f, 2.5f);
				w = w * 0.2f;
				s->target.x = s->direction.i * w + s->target.x;
				s->target.y = s->direction.j * w + s->target.y;
				s->target.z = s->direction.k * w + s->target.z;
			}
			s->direction.i = s->target.x - pos->x;
			s->direction.j = s->target.y - pos->y;
			s->direction.k = s->target.z - pos->z;
			if (function_30bf0(&s->direction) == 0.f)
				s->direction = *d;
		}

		if (0.8660254f > s->direction.k * d->k + s->direction.j * d->j + d->i * s->direction.i)
			s->direction = *d;
	}

	vector3f *position = &out->position;
	*position = *q;

	if (s->active)
	{
		real t3 = s->distance * 0.375f;
		real e3 = 0.001f;
				if (!(0.001f > t3))
				{
					real k3 = s->fast ? 12.f : 8.f;
					real cap3 = globals->rate * k3;
					e3 = t3 > cap3 ? cap3 : t3;
				}
		long counter = s->counter;
		real volatile cnt = (real)counter;
		real w = s->direction.k * q->k + s->direction.j * q->j + q->i * s->direction.i;
		if (counter == 0)
			q = g_4687a4;
		w = w * globals->rate;
		vector3f qr;
		qr.i = q->i;
		qr.j = q->j;
		qr.k = q->k;
		real third3 = e3 * 0.33333334f;
		real lo = (w - third3) * 3.f * 0.5f;
		real hi = (e3 - third3) * 3.f * 0.5f;
		if (lo < 0.f)
			lo = 0.f;
		vector3f dl;
		dl.i = s->target.x - pos->x;
		qr.i = qr.i * globals->rate;
		dl.j = s->target.y - pos->y;
		qr.j = qr.j * globals->rate;
		dl.k = s->target.z - pos->z;
		qr.k = qr.k * globals->rate;
		real dist = (real)sqrt(dl.k * dl.k + dl.j * dl.j + dl.i * dl.i);

		if (e3 > 0.001f)
		{
			if (!s->settled && !(lo > dist) && 4.f < (real)(s->duration - counter))
			{
				if (5.f > cnt)
				{
					real f = MIN(third3, MAX(0.f, e3 - MAX(0.f, w)));
					position->i = s->direction.i * f + qr.i;
					position->j = s->direction.j * f + qr.j;
					position->k = s->direction.k * f + qr.k;
					real ticks = (real)globals->field_2_3;
					position->i = position->i * ticks;
					position->j = position->j * ticks;
					position->k = position->k * ticks;
					s->ticks_left = (long)((dist - (((3.f - cnt) - 0.5f) * e3 * 0.5f + hi)) / e3 + (3.f - cnt) + 3.f + 0.5f);
				}
				else
				{
					*position = qr;
					real ticks = (real)globals->field_2_3;
					position->i = ticks * position->i;
					position->j = position->j * ticks;
					position->k = position->k * ticks;
					s->ticks_left = (long)((dist - hi) / e3 + 3.f - 0.5f);
				}
			}
			else
			{
				real v = PIN((double)s->alignment * 1.5, 0.75, 3.5);
				real step = (real)(globals->rate * v);
				vector3f nq = qr;
				real len = (real)sqrt(qr.k * qr.k + qr.j * qr.j + qr.i * qr.i);
				real nlen = function_30bf0(&nq);
				if (!s->settled)
				{
					s->settled = true;
					s->settle_distance = MAX(step, len - step);
					s->settle_minimum = MAX(0.001f, dist);
				}
				real align = d->j * nq.j + d->k * nq.k + d->i * nq.i;
				if (align > len * 0.087155744f && nlen != 0.f)
				{
					real m = s->settle_distance * 0.25f;
					if (len > 0.001f)
					{
						real len4 = MIN(m, len);
						real ratio = dist / hi - 0.5f;
						real back = 0.f - len4;
						position->j = nq.j * back + qr.j;
						position->i = nq.i * back + qr.i;
						position->k = nq.k * back + qr.k;
						real ticks = (real)globals->field_2_3;
						position->i = position->i * ticks;
						position->j = position->j * ticks;
						position->k = position->k * ticks;
						s->ticks_left = (long)ratio;
					}
					else
					{
						fail = true;
						s->ticks_left = 0;
					}
				}
				else
				{
					real m = (s->settle_distance + step) * 0.33333334f;
					real m2 = MIN(m, len);
					real back = 0.f - m2;
					position->i = back * nq.i + qr.i;
					position->j = nq.j * back + qr.j;
					position->k = nq.k * back + qr.k;
					if (step > len - m + 0.001f)
					{
						fail = true;
						s->ticks_left = 0;
					}
				}
			}
		}
		else
		{
			s->settled = true;
			fail = true;
			s->ticks_left = 0;
		}
	}

	long new_counter = s->counter + 1;
	s->counter = new_counter;
	if (!fail && s->ticks_left > 0 && new_counter < s->duration + 6)
		out->flags &= ~4;
	else
		out->flags |= 4;
	return new_counter;
}

struct s_unknown_object
{
	byte unknown00[0xbc];
	bool flag_bc;
	byte unknownbd[0x1b];
	real value_d8;
};

struct s_unknown_output
{
	long unknown00;
	long mode;
};

// @retail 0x1ec3f0
void function_1ec3f0(s_unknown_object *object, s_unknown_output *output)
{
	if (object->flag_bc && object->value_d8 >= 0.1f)
		output->mode = 4;
}

s_kill_volume_globals *g_51e9c8;

// @retail 0x1ec420
void function_1ec420(void)
{
	s_kill_volume_globals *globals = (s_kill_volume_globals *)function_123d40("unknown", "unknown", sizeof(s_kill_volume_globals));
	g_51e9c8 = globals;
	globals->enabled = false;
}

// @retail 0x1ec470
void function_1ec470(void)
{
	g_51e9c8 = 0;
}

// @retail 0x1ec480
void function_1ec480(void)
{
	s_kill_volume_globals *globals = g_51e9c8;
	memset(globals->bits, 0, sizeof(globals->bits));
	globals->enabled = true;
}

// @retail 0x1ec4b0
void function_1ec4b0(void)
{
	g_51e9c8->enabled = false;
}

struct s_unknown_entry
{
	byte unknown00[0x40];
	short bit_index;
	byte unknown42[2];
};

// @retail 0x1ec4c0
void function_1ec4c0(long index)
{
	if (index != NONE)
	{
		s_unknown_entry *entry = g_4e0350->entries + index;
		if (entry->bit_index != NONE)
		{
			g_51e9c8->bits[entry->bit_index >> 5] &= ~(1 << (entry->bit_index & 0x1f));
		}
	}
}

struct s_state_c570
{
	point3f position;
	byte field_0c;
	byte field_0d;
	byte field_0e;
	byte field_0f;
	byte field_10;
	byte field_11;
	byte field_12;
	byte field_13;
	long field_14;
	long field_18;
	byte field_1c[0x50 - 0x1c];
	vector3f field_50;
	long time;
	long field_60;
	long field_64;
	vector3f field_68;
};

// @retail 0x1ec570
void function_1ec570(s_state_c570 *state)
{
	state->position = *(point3f *)g_4687b0;
	state->field_0d = 0;
	state->field_0c = 0;
	state->field_0e = 0;
	state->field_0f = 0;
	state->field_10 = 0;
	state->field_11 = 0;
	state->field_13 = 0;
	state->field_14 = NONE;
	state->field_18 = NONE;
	state->field_50 = *g_4687a4;
	state->field_12 = 0;
	state->field_60 = NONE;
	state->field_64 = NONE;
	state->field_68 = *g_4687a4;
	state->time = NONE;
}

// @retail 0x1ec5f0
real function_1ec5f0(void *data)
{
	s_state_c570 *state = (s_state_c570 *)data;
	real result = 1.0f;
	if (state->time != NONE)
	{
		real elapsed = (g_510c54->game_time - state->time) * g_510c54->rate * 0.8f;
		result = PIN(elapsed, 0.0f, 1.0f);
	}
	return result;
}

// @retail 0x1ec640
real function_1ec640(void *data)
{
	s_state_c570 *state = (s_state_c570 *)data;
	real result = 0.0f;
	if (state->time != NONE)
	{
		result = (g_510c54->game_time - state->time) * g_510c54->rate * 6.6666665f;
		result = PIN(1.0f - result, 0.0f, 1.0f);
	}
	return result;
}

struct s_movement_query_state
{
	byte field_0[0xc];
	bool flag_c;
	byte field_d;
	bool flag_e;
	byte field_f;
	byte ticks;
	bool active;
	bool disabled;
	byte field_13[0x60 - 0x13];
	long time;
	long value;
	vector3f direction;
};

__forceinline long movement_round_ticks(real value)
{
	long result;
	__asm
	{
		fld value
		fistp result
	}
	return result;
}

// @retail 0x1ecef0
bool function_1ecef0(void *ragdoll)
{
	s_movement_query_state *state = (s_movement_query_state *)ragdoll;
	return state->active && !state->disabled &&
		((state->flag_c && state->flag_e) ||
		state->ticks > movement_round_ticks(g_510c54->field_2_3 * 0.2f));
}

// @retail 0x1ed430
bool function_1ed430(void *ragdoll, vector3f *direction, long *value)
{
	bool result = false;
	s_movement_query_state *state = (s_movement_query_state *)ragdoll;
	long *const *value_reference = &value;
	if (state->time != NONE)
	{
		long current_time = g_510c54->game_time;
		long limit = movement_round_ticks(g_510c54->field_2_3 * 0.1f);
		if (current_time - state->time <= limit)
		{
			**value_reference = state->value;
			*direction = state->direction;
			result = true;
		}
	}
	return result;
}

struct s_volume_list_view
{
 byte field_0[0x230];
 long count;
 short *volumes;
};
bool function_11c5b0(long object_index, long trigger_volume_index);

// @retail 0x1ec500
bool function_1ec500(long object_index)
{
 s_volume_list_view *scenario = (s_volume_list_view *)g_4e0350;
 bool result = false;
 if (scenario->count > 0)
 {
  long index = (g_510c54->game_time + (object_index & 0xffff)) % scenario->count;
  if (!(g_51e9c8->bits[index >> 5] & (1 << (index & 31))))
  {
   short const *const volume_reference = &scenario->volumes[index];
   short const *const *reference = &volume_reference;
   word volume = **reference;
   if (volume != 0xffff)
    result = function_11c5b0(object_index, (short)volume);
  }
 }
 return result;
}


struct s_havok_component;
void havok_component_rigid_bodies_activate(s_havok_component *component);
bool function_109e00(long object_index, vector3f *velocity, bool definition_flag_required);
void function_b7740(long object_index, vector3f const *linear_velocity, vector3f const *angular_velocity, bool skip_update);
void function_1c4b00(long object_index, void *linear, void *angular, long force);
void __stdcall function_b9b90(long object_index, bool disable);
void function_b7360(long object_index);
void function_bba20(long object_index);
void __stdcall function_b8890(long object_index);

// @retail 0x1ed340
void __stdcall function_1ed340(void *physics, long object_index)
{
 long const *input_reference = &object_index;
 struct s_release_object_view
 {
  byte unknown00[0xb4];
  long component_index;
  byte unknownb8[8];
  word flag0 : 1;
  word flag1 : 1;
  word flag2 : 1;
  word flag3 : 1;
  word flag4 : 1;
  word flag5 : 1;
  word flag6 : 1;
  word remaining : 9;
 };
 s_release_object_view *object = *(s_release_object_view **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
 *(long *)((byte *)physics + 0x14) = NONE;
 *(long *)((byte *)physics + 0x18) = NONE;
 if (!TEST_FIELD_BIT(object->flag6))
 {
  s_havok_component *component = (s_havok_component *)(g_51e9b8->data + (*(long volatile *)((byte *)object + 0xb4) & 0xffff) * 0xa0);
  vector3f velocity;
  if (function_109e00(*input_reference, &velocity, false))
  {
   function_b7740(object_index, &velocity, NULL, false);
   function_1c4b00(object_index, &velocity, NULL, 1);
   if (velocity.k * velocity.k + velocity.j * velocity.j + velocity.i * velocity.i > 0.0001f)
   {
    function_b9b90(object_index, false);
    function_b7360(object_index);
    function_bba20(object_index);
   }
  }
  function_b8890(object_index);
  havok_component_rigid_bodies_activate(component);
 }
}


#include "object_default_placement.h"
#include "object_queries.h"
#include <float.h>
struct s_small_index;
short function_0b67a0(s_small_index const *data);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_141590(transform4x3f const *matrix, transform4x3f *inverse);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
real function_1201a0(vector3f *vector, vector3f const *fallback);
bool havok_component_any_rigid_body_active(s_havok_component *component);

struct s_movement_contact_1ed4a0
{
 byte field_0[0x14];
 long field_14;
 long field_18;
 byte field_1c[0x28 - 0x1c];
 vector3f direction;
 byte field_34[0x46 - 0x34];
 signed char node;
 byte field_47;
};

PRIVATE __forceinline long movement_counter_increment(byte value)
{
 long next = value + 1;
 if (next > 254) next = 254;
 return next;
}

/* whether a direction lies within 60 degrees of the world up vector */
PRIVATE inline bool direction_faces_up(vector3f const *direction)
{
 vector3f const *up = g_4687b0;
 return dot3f(up, direction) > 0.5f;
}

// @retail 0x1ed4a0
void function_1ed4a0(s_state_c570 *state, dword *output, byte const *input)
{
 byte *component = g_51e9b8->data + (*(long *)(input + 0x40) & 0xffff) * 0xa0;
 bool disabled = *(byte *)(*(byte **)(component + 0x70) + 0x44) != 0;
 long selected = NONE;
 state->field_11 = 0;
 if (state->field_14 != NONE && !function_badc0(state->field_14, (dword)NONE))
  function_1ed340(state, *(long *)(g_51e9b8->data + (*(long *)(input + 0x40) & 0xffff) * 0xa0 + 8));
 if (state->field_14 != NONE)
 {
  component = g_51e9b8->data + (*(long *)(input + 0x40) & 0xffff) * 0xa0;
  long object_index = *(long *)(component + 8);
  byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
  if (!(bool)((*(word *)(object + 0xc0) >> 6) & 1))
  {
   byte *other = *(byte **)(g_4e0300->data + (state->field_14 & 0xffff) * 12 + 8);
   transform4x3f matrix;
   function_142a60((transform4x3f *)(other + *(short *)(other + 0x116) + (short)state->field_18 * 0x34),
    (transform4x3f *)state->field_1c, &matrix);
   state->field_0c = direction_faces_up(&matrix.up) ? movement_counter_increment(state->field_0c) : 0;
   if (state->field_0c)
   {
    real projection = dot3f(&matrix.forward, &matrix.up);
    vector3f projected_up;
    vector3f_scale(&matrix.up, projection, &projected_up);
    vector3d_from_points3d((point3f *)&projected_up, (point3f *)&matrix.forward, &matrix.forward);
    if (function_30bf0(&matrix.up) == 0.0f) matrix.up = *g_4687b0;
    if (function_30bf0(&matrix.forward) != 0.0f)
    {
     function_b75a0(object_index, &matrix.position, &matrix.forward, &matrix.up, NULL, false);
     state->position = *(point3f *)&matrix.up;
    }
    else
     function_1ed340(state, object_index);
   }
   else
    function_1ed340(state, object_index);
  }
  else
   function_1ed340(state, object_index);
 }
 if (state->field_14 == NONE)
 {
  dword flags = *(dword *)(input + 0x18);
  if (!(flags & 0x40))
  {
   component = g_51e9b8->data + (*(long *)(input + 0x40) & 0xffff) * 0xa0;
   long count = *(long *)(component + 0x8c);
   real best = -FLT_MAX;
   for (long i = 0; i < count; i++)
   {
    if ((*(s_movement_contact_1ed4a0 **)(component + 0x88))[i].direction.k > 0.70710677f &&
     (*(s_movement_contact_1ed4a0 **)(component + 0x88))[i].direction.k > best)
    {
     selected = i;
     best = (*(s_movement_contact_1ed4a0 **)(component + 0x88))[i].direction.k;
    }
   }
   bool found;
   s_movement_contact_1ed4a0 *contact;
   if (selected != NONE)
   {
    contact = &(*(s_movement_contact_1ed4a0 **)(component + 0x88))[selected];
    if (flags & 2) state->position = *(point3f *)&contact->direction;
    else
    {
     state->position.x += contact->direction.i;
     state->position.y += contact->direction.j;
     state->position.z += contact->direction.k;
    }
    function_1201a0((vector3f *)&state->position, g_4687b0);
    found = true;
    state->field_11 = contact->field_14 == NONE;
   }
   else { state->position = *(point3f *)g_4687b0; found = false; }
   if (havok_component_any_rigid_body_active((s_havok_component *)component) && !found) goto clear_counters;
   if (*(dword *)(input + 0x18) & 1)
   {
    state->field_0c = movement_counter_increment(state->field_0c);
    goto increment_duration;
   }
   state->field_0c = direction_faces_up((vector3f *)&state->position) ? movement_counter_increment(state->field_0c) : 0;
   if (!state->field_0c || state->field_0d < movement_round_ticks(g_510c54->field_2_3 * 0.15f)) goto increment_duration;
   long other_index = *(long *)(input + 0xc);
   if (other_index == NONE || selected == NONE) goto increment_duration;
   component = g_51e9b8->data + (*(long *)(input + 0x40) & 0xffff) * 0xa0;
   contact = &(*(s_movement_contact_1ed4a0 **)(component + 0x88))[selected];
   if (contact->field_18 != other_index) goto increment_duration;
   long object_index = *(long *)(component + 8);
   byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
   if (*(long *)(object + 0x14) != NONE) goto increment_duration;
   byte *other = *(byte **)(g_4e0300->data + (other_index & 0xffff) * 12 + 8);
   word definition_flags = *(word *)(g_4e3b44[*(long *)other & 0xffff].bytes + 2);
   if (!(bool)((definition_flags >> 6) & 1) || (bool)((definition_flags >> 11) & 1)) goto increment_duration;
   long other_component_index = *(long *)(other + 0xb4);
   if (other_component_index == NONE) goto increment_duration;
   byte *other_component = g_51e9b8->data + (other_component_index & 0xffff) * 0xa0;
   long node_index = *(signed char *)(other_component + 0x19);
   if (node_index == NONE || function_0b67a0((s_small_index *)other_component) != contact->node) goto increment_duration;
   transform4x3f const *node_matrix = function_b8bd0(other_index, (short)node_index);
   state->position = *(point3f *)&contact->direction;
   function_1201a0((vector3f *)&state->position, g_4687b0);
   transform4x3f matrix;
   function_ba160(object_index, &matrix);
   matrix.up = *(vector3f *)&state->position;
   matrix.forward.i -= matrix.up.i;
   matrix.forward.j -= matrix.up.j;
   matrix.forward.k -= matrix.up.k;
   if (function_30bf0(&matrix.forward) != 0.0f)
   {
    matrix.left.i = matrix.forward.k * matrix.up.j - matrix.up.k * matrix.forward.j;
    matrix.left.j = matrix.up.k * matrix.forward.i - matrix.forward.k * matrix.up.i;
    matrix.left.k = matrix.forward.j * matrix.up.i - matrix.up.j * matrix.forward.i;
    transform4x3f inverse;
    function_141590(node_matrix, &inverse);
    function_142a60(&inverse, &matrix, (transform4x3f *)state->field_1c);
    state->field_14 = *(long *)(input + 0xc);
    state->field_18 = node_index;
    function_b8840(object_index);
   }
increment_duration:
   state->field_0d = movement_counter_increment(state->field_0d);
   goto counters_done;
  }
clear_counters:
  state->field_0c = 0;
  state->field_0d = 0;
 }
counters_done:
 state->field_12 = disabled;
 if (!state->field_0c && state->field_10 <= movement_round_ticks(g_510c54->field_2_3 * 0.2f)) output[1] |= 1;
 else output[1] &= ~1;
}


#if 0
// Activating this draft loses matched 0x2901e0 and 0xfa100 through LTCG shifts.
#include "unknown_1efac0.h"
#include "unknown_1eb550.h"
#include <xmmintrin.h>
extern real g_47f05c;
void havok_component_rigid_body_point_velocity_get(vector3f *velocity, long rigid_body_index, s_havok_component *component, point3f const *point);
void __stdcall function_1c3770(long object_index, dword flags);
bool function_1c5210(transform4x3f const *matrix, void *shape, long excluded_component, long filter);

class c_2def90 : public c_a
{
public:
 c_2def90(real radius);
 long field_8;
 real radius;
};

// @retail 0x1ec690 disabled
void function_1ec690(byte const *input, s_state_c570 *state, byte *output)
{
 long component_index = *(long *)(input + 0x18);
 byte *component_data = g_51e9b8->data;
 byte *component = component_data + (component_index & 0xffff) * 0xa0;
 bool disabled = *(byte *)(*(byte **)(component + 0x70) + 0x44) != 0;
 vector3f *velocity = (vector3f *)(output + 0xc);
 *velocity = *(vector3f *)(input + 0x124);
 bool changed = false;
 struct { vector3f change, inherited, current; } scratch;
 vector3f &change = scratch.change;
 vector3f &inherited = scratch.inherited;
 if (disabled)
 {
  byte *source_velocity = *(byte **)(component + 0x70);
  inherited.i = *(real *)(source_velocity + 0x30);
  inherited.j = *(real *)(source_velocity + 0x34);
  inherited.k = *(real *)(source_velocity + 0x38);
  inherited.i *= 0.4f;
  inherited.j *= 0.4f;
  inherited.k *= 0.4f;
  velocity->i -= inherited.i;
  velocity->j -= inherited.j;
  velocity->k -= inherited.k;
 }
 if (state->field_14 != NONE)
 {
  if (!function_badc0(state->field_14, (dword)NONE))
   function_1ed340(state, *(long *)(g_51e9b8->data + (*(long *)(input + 0x18) & 0xffff) * 0xa0 + 8));
 }
 else if (*(long *)(input + 0xc) != NONE)
 {
  byte *object = *(byte **)(g_4e0300->data + (*(long *)(input + 0xc) & 0xffff) * 12 + 8);
  byte *other_component = g_51e9b8->data + (*(long *)(object + 0xb4) & 0xffff) * 0xa0;
  signed char index = *(signed char *)(other_component + 0x18);
  short rigid_body_index = function_0b67a0((s_small_index *)other_component);
  if (rigid_body_index != NONE)
  {
   vector3f &current = scratch.current;
   havok_component_rigid_body_point_velocity_get(&current, rigid_body_index, (s_havok_component *)other_component, (point3f *)(input + 0xe8));
   change.i = current.i - state->field_50.i;
   change.j = current.j - state->field_50.j;
   change.k = current.k - state->field_50.k;
   changed = state->field_50.i * state->field_50.i + state->field_50.j * state->field_50.j + state->field_50.k * state->field_50.k > 0.0f;
   state->field_50 = current;
  }
 }
 else state->field_50 = *g_4687a4;
 if (state->field_0c && (0.01f > *(real *)(output + 0x14) || 0.01f > *(real *)(input + 0x12c)))
 {
  double a = *(real *)(output + 0x10);
  double b = velocity->i;
  real magnitude = (real)sqrt(b * b + a * a);
  real decrement = g_510c54->rate * 2.0f;
  if (magnitude > decrement)
  {
   real scale = 1.0f - decrement / magnitude;
   velocity->i *= scale;
   *(real *)(output + 0x10) *= scale;
  }
  else { velocity->i = 0.0f; *(real *)(output + 0x10) = 0.0f; }
 }
 vector3f const *target = (vector3f *)(input + 0x138);
 if (target->i * target->i + target->j * target->j + target->k * target->k > 9.99999905e-9f)
 {
  if (*(dword *)output & 1) *(dword *)output |= 1;
  else *(dword *)output &= ~1;
  real x = *(real *)(input + 0xf4) * target->i - *(real *)(input + 0xf8) * target->j - *(real *)(input + 0x124);
  real y = target->i * *(real *)(input + 0xf8) + *(real *)(input + 0xf4) * target->j - *(real *)(input + 0x128);
  real length = (real)sqrt((double)y * y + (double)x * x);
  real acceleration = *(real *)(input + ((input[0x14] & 4) ? 0x148 : 0x144));
  real maximum = g_510c54->rate * acceleration;
  if (length > maximum) { real scale = maximum / length; x *= scale; y *= scale; }
  velocity->i += x;
  *(real *)(output + 0x10) += y;
 }
 if (!(input[0x14] & 2) && !(2.25f > velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k))
 {
  state->field_0e = 0;
  state->field_0f = movement_counter_increment(state->field_0f);
 }
 else
 {
  state->field_0e = movement_counter_increment(state->field_0e);
  state->field_0f = 0;
 }
 if (input[0xb9]) state->field_13 = 0;
 else if (input[0xb8] && state->field_13 < 2)
 {
  real extra = g_47f05c + 0.001f;
  long selected = NONE;
  __m128 rotation[3];
  rotation[0] = _mm_setzero_ps();
  rotation[1] = _mm_setzero_ps();
  rotation[2] = _mm_setzero_ps();
  for (long index = state->field_13 + 1; index < 3; index++)
  {
   byte *shape = *(byte **)(*(byte **)(input + 8) + 0x24) + index * 0x80;
   real radius = *(real *)(shape + 0x2c) - extra;
   if (g_47f05c > radius) radius = g_47f05c;
   c_2def90 sphere(radius);
   __m128 position = _mm_set_ps(0.0f, *(real *)(input + 0xf0), *(real *)(input + 0xec), *(real *)(input + 0xe8));
   position = _mm_add_ps(position, *(__m128 *)(shape + 0x70));
   transform4x3f matrix;
   matrix.scale = 1.0f;
   matrix.forward.i = 1.0f;
   matrix.forward.j = ((real *)&rotation[0])[1];
   matrix.forward.k = ((real *)&rotation[0])[2];
   matrix.left.i = ((real *)&rotation[1])[0];
   matrix.left.j = 1.0f;
   matrix.left.k = ((real *)&rotation[1])[2];
   matrix.up.i = ((real *)&rotation[2])[0];
   matrix.up.j = ((real *)&rotation[2])[1];
   matrix.up.k = 1.0f;
   matrix.position = *(point3f *)&position;
   if (!function_1c5210(&matrix, &sphere, *(long *)(input + 0x18), *(long *)(input + 0x24))) selected = index;
  }
  if (selected != NONE)
  {
   state->field_13 = (byte)selected;
   byte *current = g_51e9b8->data + (*(long *)(input + 0x18) & 0xffff) * 0xa0;
   function_1c3770(*(long *)(current + 8), 0);
  }
 }
 long count = *(long *)(component + 0x8c);
 if (count)
 {
  signed char index = *(signed char *)(component + 0x18);
  short rigid_body_index = function_0b67a0((s_small_index *)component);
  s_movement_contact_1ed4a0 *contacts = *(s_movement_contact_1ed4a0 **)(component + 0x88);
  long i;
  for (i = 0; i < count; i++)
   if (contacts[i].field_14 != NONE && contacts[i].field_18 != *(long *)(input + 0xc)) break;
  if (i == count && rigid_body_index != NONE)
  {
   real x = PIN(velocity->i, -0.05f, 0.05f);
   if (x == velocity->i)
   {
    real y = PIN(*(real *)(output + 0x10), -0.05f, 0.05f);
    if (y == *(real *)(output + 0x10))
    {
     real minimum = 0.0f - g_51e9c4->unknown0 - 0.05f;
     real z = PIN(*(real *)(output + 0x14), minimum, 0.05f);
     if (z == *(real *)(output + 0x14)) state->field_10 = movement_counter_increment(state->field_10);
    }
   }
  }
  else state->field_10 = 0;
 }
 else state->field_10 = 0;
 if (changed)
 {
  velocity->i += change.i;
  velocity->j += change.j;
  velocity->k += change.k;
 }
 if (disabled)
 {
  velocity->i += inherited.i;
  velocity->j += inherited.j;
  velocity->k += inherited.k;
 }
}

#endif
