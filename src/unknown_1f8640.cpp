// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_26b230.h"
#include "globals.h"
#include "slot_handler.h"
#include "unknown_1e3920.h"
#include "unknown_2626b0.h"
#include "unknown_0259a0.h"
#include "unknown_1f9240.h"
#include <math.h>
#include <string.h>

#define OWNER_STATE(index) ((s_slot_owner_entry *)(g_4f55f0->data + ((index) & 0xffff) * sizeof(s_slot_owner_entry)))

// @retail 0x1f8640
byte function_1f8640(long index)
{
	return OWNER_STATE(index)->unknown50c;
}

// @retail 0x1f8660
bool function_1f8660(long index)
{
	bool result = false;
	s_slot_owner_entry *s = OWNER_STATE(index);
	byte flag = s->unknown50c;
	if (flag && s->unknown504 == 1)
		result = true;
	return result;
}

// @retail 0x1f86a0
void function_1f86a0(long index)
{
	s_slot_owner_entry *s = OWNER_STATE(index);
	s->unknown50c = 0;
	s->unknown5ac = NONE;
	s->unknown5b0 = NONE;
	s->unknown5b4 = 0;
	s->unknown5b6 = 0;
	s->unknown4ac = 0;
	s->unknown504 = 0;
}

// @retail 0x1f86f0
bool function_1f86f0(long index)
{
	return OWNER_STATE(index)->unknown504 == 2;
}

// @retail 0x1f8720
bool function_1f8720(long index)
{
	return OWNER_STATE(index)->unknown504 == 3;
}

// @retail 0x1f8750
void function_1f8750(long index)
{
	s_slot_owner_entry *s = OWNER_STATE(index);
	s->unknown508++;
	s->unknown50c = 0;
	s->unknown504 = 3;
}

/* a request the movement code passes to the actor's unit (function_e6900) */
struct s_moving_unit_request
{
	long type;
	union
	{
		struct
		{
			point3f point;
			vector3f facing;
		} face;
		struct
		{
			short unknown4;
			byte unknown6[2];
			point3f point;
			vector3f vector;
		} turn;
	};
};

bool function_262590(long actor_index, s_reference reference, bool unknown);
long function_1e4990(long index);

/* the actor has arrived */
// @retail 0x1f8780
void function_1f8780(long actor_index, bool unknown)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	long type = 0;

	actor->unknown50c = true;
	actor->unknown504 = 2;
	if (!actor->unknown227)
	{
		short goal = actor->unknown4ac;

		if ((goal == 4 || goal == 6 || goal == 5) &&
			function_262590(actor_index, actor->unknown4b8_reference, false))
		{
			real radius = function_1e3920(actor_index);

			s_type_c3b527 *local_1 = (s_type_c3b527 *)function_262b40(actor->unknown4b8_reference);
			if (function_210ac0(local_1, &actor->position) <= radius + 0.2)
				actor->unknown227 = true;
		}
		else if (actor->unknown4e8)
		{
			actor->unknown227 = true;
		}
	}

	if (unknown)
	{
		s_moving_object_header *header = &((s_moving_object_header *)g_4e0300->data)[actor->unit_index & 0xffff];

		if (header->type == 0)
			type = header->object[0x3dc];
		switch (type)
		{
		case 2:
		{
			s_moving_unit_request request;

			if (actor->unknown4ac == 6)
			{
				request.type = 0x2d;
				request.turn.unknown4 = 0;
				if (!function_262a90(actor->unknown4b8_reference, &request.turn.point, &request.turn.vector))
					goto local_0;
			}
			else if (actor->unknown4ac == 4)
			{
				long character = function_1e4990(actor->tag_index);

				if (!character || (*(byte *)character & 2) ||
					!(function_262b40(actor->unknown4b8_reference)->flags & 0x40))
				{
					break;
				}
				request.type = 0x2f;
				if (!function_262af0(actor->unknown4b8_reference, &request.face.point, &request.face.facing))
					goto local_0;
				request.face.facing = actor->unknown290;
			}
			else
			{
				break;
			}

			if (function_e6900(actor->unit_index, (s_unit_request *)&request))
			{
				actor->unknown227 = true;
				break;
			}
local_0:
			{
				actor->unknown50c = false;
				actor->unknown504 = 3;
			}
			break;
		}
		}
	}
}
/* the length of the rest of the actor's path */
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x1f8940
real function_1f8940(long actor_index)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real length = 0.0f;

	if (actor->unknown50c && actor->unknown504 == 1)
	{
		real total = 0.0f;
		point3f previous = actor_moving_get(actor_index)->position;
		short i = actor->path_index;

		if (i < actor->path_count)
		{
			do
			{
				s_actor_path_point *point = &actor->path[i];
				point3f position;

				if (point->node.output_index == NONE ||
					!function_2104b0(point->node.output_index, &point->node.point, &position))
				{
					position = point->node.point;
				}
				vector3f offset;
				offset.i = previous.x - position.x;
				offset.j = previous.y - position.y;
				offset.k = previous.z - position.z;
				real squared = offset.k * offset.k + offset.i * offset.i;
				squared += offset.j * offset.j;
				total += (real)sqrt(squared);
				previous = position;
				i++;
			}
			while (i < actor->path_count);
			length = total;
		}
	}

	return length;
}
#pragma function(_ReadWriteBarrier)
/* the parts of the actor's character block and of its vehicle's entry that
   set the radius of the actor's path source */
struct s_path_radius_block
{
	byte unknown00[4];
	real radius;
};

struct s_path_radius_element
{
	byte unknown00[0x14];
	real radius;
};

long function_1e4a50(long index);

static inline void path_source_initialize(s_path_source *source, real radius, byte unknown04, long object_index, real unknown4c)
{
	memset(source, 0, sizeof(*source));
	source->radius = radius;
	source->unknown04 = unknown04;
	source->object_index = object_index;
	source->unknown0c = NONE;
	source->unknown4c = unknown4c;
}

static inline void path_source_set_point(s_path_source *source, s_path_point const *point, long unknown24)
{
	source->has_point = true;
	source->point = *point;
	source->unknown24 = unknown24;
}

/* where the actor's paths start: its position, or its vehicle's */
// @retail 0x1f90f0
void function_1f90f0(long actor_index, s_path_source *source)
{
	s_actor_moving *actor = actor_moving_get(actor_index);
	real radius = ((s_path_radius_block *)function_1e4a50(actor->tag_index))->radius;
	long object_index = actor->unit_index;
	real unknown4c;

	if (actor->unknown266)
	{
		s_path_radius_element *element;

		element = (s_path_radius_element *)function_1e5450(actor_index, moving_object_get(actor->unknown26c)->tag_index);
		object_index = actor->unknown26c;
		if (element && element->radius > 0.0f)
			radius = element->radius;
	}

	if (actor->unknown086 >= 3 && actor->unknown328 >= 3)
		unknown4c = 1.0f;
	else
		unknown4c = 5.0f;

	path_source_initialize(source, radius, actor->unknown3e4, object_index, unknown4c);
	if (actor->unknown229)
	{
		s_path_point point;

		point.point = actor->position;
		point.cluster_index = NONE;
		path_source_set_point(source, &point, NONE);
	}
	else
	{
		function_26c180(actor_index);
		path_source_set_point(source, &actor->location, actor->unknown28c);
	}
}

real function_30bf0(vector3f *v);
void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);

/* the vector in the frame of the forward direction (forward, left, up), as
   a unit vector; flat frames ignore the vertical */
// @retail 0x1f8510
void __stdcall function_1f8510(bool full_frame, vector3f const *vector, vector3f const *forward, vector3f *out)
{
	if (full_frame)
	{
		vector3f left;
		vector3f up;
		vector3f normalized_forward;

		function_11d180(forward, g_4687b0, &normalized_forward, &left, &up);
		out->i = dot3f(forward, vector);
		out->j = dot3f(vector, &left);
		out->k = dot3f(vector, &up);
		function_30bf0(out);
	}
	else
	{
		vector3f left;

		left.i = 0.0f - forward->j;
		left.j = forward->i;
		out->i = vector->i * forward->i + forward->j * vector->j;
		out->j = vector->i * left.i + left.j * vector->j;
		out->k = 0.0f;
		function_30bf0(out);
	}
}

struct s_1f8a70_frame
{
	short mode;
	byte unknown02[2];
	real unknown04;
	real unknown08;
	union
	{
		s_type_c3b527 point;
		s_reference reference;
		long index;
	} target;
	long source_node;
	real unknown20;
	real radius;
	byte unknown28[0x38 - 0x28];
	long root_index;
	byte unknown3c[4];
	s_type_c3b527 destination;
	long destination_node;
	real destination_radius;
};

struct s_actor_move_request;
struct s_path_result_view;
struct s_29e730;
struct s_29de21;
struct s_29e0f9;
/* a unit action state, returned by value through the caller's temporary */
struct s_unit_action_state
{
	short value;
	s_unit_action_state(short state) : value(state) {}
};
s_unit_action_state function_e70b0(long unit_index);
long function_1e4a50(long character_index);
void __stdcall function_26c2d0(long prop_index);
bool function_1f9490(long actor_index, s_reference reference, s_actor_move_request *request);
bool function_1f9580(long actor_index, s_reference reference);
void function_1caa40(long object_index, point3f *position);
long function_baf80(long object_index);
bool __stdcall function_29e730(s_29e730 *request, long actor_index, point3f const *position,
    s_type_c3b527 const *target, s_reference reference, s_29de21 *context,
    long root_index, s_29e0f9 *result);
void function_2713c0(s_type_f17a25 *state, s_type_c3b527 const *point, long node_index, real radius);
bool function_270930(long actor_index, s_type_f17a25 *state, s_path_result_view *result);
void function_2705c0(s_path_source *source, point3f const *point, real radius,
    long object_index, real weight, bool unknown);

struct s_1f8a70_actor
{
	byte unknown00[0x4ac];
	s_1f8a70_frame frame;
};
// MiniMax G (11-12): no new form, no lower score (182): retail's case 2 arm runs the switch's value down through the
// compare-and-decrement pair and calls 0x262b40, where ours tests the value in place and calls 0x410730, so the arm's
// shape differs before the twenty bytes of extra code that shift every later branch.
// MiniMax B (11-13): no new form, no lower score (182): retail's route dispatch is laid out between mode case 5 and case 6,
// its failure block stores false into the frame byte, calls 0x1f8750 and reloads the byte, and its merged exit returns that
// byte in AL where ours goes through BL; the permuter (fast, 5 tries in 394 s) found nothing below the 167 differing
// instructions of the reduced image, so the body is the committed one.
// score 182 (was 182; 182 before): earlier 188; Frame 0x80, prologue, first exit and the unit-mode slot agree; the route test's layout, case 7's stores and the result's home differ.
// MiniMax S (11-10): no new form, no lower score (182): the route dispatch still follows case 7 where retail puts it after case 5 with cases 6 and 7 behind it, and the final result still goes through BL where retail returns its frame byte.
// MiniMax E (11-09): no new form, no lower score (188).
// MiniMax E (11-06): 188 then (was 188): no new form lowered it. The byte locals sit two bytes lower
// than retail's (ours at esp+0x10 where retail has esp+0x12), retail switches on the word at
// esp+0x14 with a jump table where ours tests the mode with branches, and ours is twenty bytes
// shorter than retail (1632 against 1652).
// Earlier at 228: Frame 0x84 versus 0x80; pool/data entry order and unit-output/route/result homes differ.
// MiniMax S (10-31): no new form, no lower score. Deferring the result's initializer reproduces retail's
// 0x80 frame and prologue exactly (the first difference moves from +0x0 to +0x33), but the aligned body
// then differs in ~417 instructions, so the 228 body is kept.
// Earlier: frame 0x84 versus retail 0x80; pool/data entry schedule and unit-output/route/result homes differ.
// Round 46 leads (aligned diff 59 at 228): retail's failure block stores result = false itself and the
// route switch has a real default storing false (`default: result = false; break;` and `failed:
// result = false;` give frame 0x80, aligned 50, positional 415). VALUE BUG: retail's case 3 jumps into
// case 5's tail and also sets route_mode = 1; ours never does (with the frame fix, aligned 44,
// positional 270; alone 423). Retail's first exit keeps false in BL across 0x1f86a0 (`mov al, bl`):
// one final `return result;` reached from the first path by a goto stops the constant fold (length
// 1652 = retail, aligned 45/46, positional 367/371) but the value goes to a stack byte, not BL. Retail
// packs unit_mode/result at frame +2 (ours +0) and reads route_mode as a short at +4; short route on
// these graphs 404/430/431 (it takes EDI). Returned unit-mode test 418; result from the disabled test
// 435 (sete). All restored; next round: apply the case 3 fix with whatever recovers BL and the +2 slot.
// Earlier forms (scores on the graph of their time, frame 0x80 at 399/408): global optimization off
// 1030, frame pointer 500, inline depth 1 414, no route range guard 434, result after first exit 419.
// Returned immediate/arrival/mode tests 437/437/450; pairs 441/432/458; all tests 439; initial decision
// ladder 436. Fast control 415; signed-char result 425; entry workspaces 415; common target query 440;
// shared cases four/five 448; position-owner local 415. Reduced trials: typed scratch only 415; byte
// scratch only 415; immutable path 415; read-only tags 415; reference expression 419; chained reset 415;
// context branches 415; reference branches 427. Native active flag 411; native reset member 411; native
// mode member 411; native source metadata 411; native point source 411; all native actor members 411;
// paired metadata null guards 410 (plain 413); case order two/six/three/seven/four/five 438. Point node
// first 433; point radius first 399 plain; target node first 434; target radius first 399; moving node
// first 428; immediate records last 412; target records last 407; moving record last 428.
// Destination orders 234675 437; 235674 440; 452367 419; 243567 418; 234657 412; 235746 453; 456723 415;
// 246357 433. Entry parameter-only path conversion, uninitialized unit-mode/route homes and their four
// combinations each 399; complete entry/reference/root/position declaration homes 399. Destination
// receiver at switch/entry/unit boundary/route phase 435/396/396/412; node/radius receiver 399/399; all
// three receivers at switch/route 435/412. Early disabled/active/both getters 407/407/428; result-first
// interactions 407/407/428; actor after disabled/active exits 407/407. Reset/cleanup/arrival/property/
// movement/target/predicate/route inline-noinline declarations 399 each. Initial mode/reference mode/root
// override snapshots 399/399/399; named and separate route admission 399/399; scratch-location memcpy
// 399; readonly query targets 399; combined native mode/root values 399. Five independent reset orders
// 399 each; initial/success/paired status publication 399/399/399. Character-index/destination-mode/
// root-override field references, singly and in all combinations, 399 each; existing actor query at every
// use 787. Request/location/reference receivers 449/399/449/450/449/450/449; route-only request 449.
// Movement flags byte/dword/long 396/399/399; state const pointer/reference 399/399; state reference with
// flags byte/dword/long 396/399/399; long result/long route/both on byte flags and state reference
// 421/410/427. Round 142: separate noinline declarations for radius/position/root/directed route/path
// source/path settings/scratch get/scratch release all 399. Round 169: nonzero route producer with
// initial/reference/both bounded mode predicates 399/414/406/413; canonical zero tests 399/414/406/413.
// Round 160: native refreshed reset/direct owner observations 402/464; coherent cleanup inline alone/with
// native reset 402/402; native reset with single exit/escaped mode name/both/cleanup 408/402/408/408.
// Round 251: unit-output phase scope, with source/settings early end, wide route, both, late result,
// source+late, wide+late, all: 402/402/410/410/429/429/439/439. Round 373: existing returned unit-output
// test with deferred result, output scope, both, long route, long+deferred, long+scope and all lifetime
// forms 404/433/404/433/412/440/412/440. Round 280: scratch-admission-owned final result,
// bool/byte/short/long with entry default 402/410/448/444; without the default and with literal failed
// return 410/411/426/411. Round 239: direct three-route decision chains 210/201/120/102/021/012
// 417/416/420/398/420/416; 210 with long route/returned unit output 423/414. Round 412: branch-owned
// disabled false and ordinary false after unit, bool/byte/short/long 419/427/441/432; after resets
// 417/425/441/432. Round 193 (dword-selector graph): output/selector/result role names, seven subsets 228
// each; all with unit phase scope 228; signed long 411, returned mode output 412; three-choice enum
// 411/412, scoped enum 411/412. Round 215: route short/word/signed-byte/byte 420/366/416/336; each with
// returned unit-output consumption 416/371/422/350.
// Lane 18 round 6: route_mode looks like a spilled short (dword stores, movsx word reads) with EDI holding NONE.
// On the frame-0x80 graph (case 3 also setting route_mode = 1 after the destination and node, in case 5's
// order, so the two tails merge as retail; `default: result = false;` and `failed: result = false;`) 270,
// aligned 44: positions slip by one from the first exit (no `xor bl, bl` / `mov al, bl`) and by up to 13 in the
// mode switch, where retail places the route dispatch after case 5's tail and cases 6-7 after it. Value fix and
// false stores without the case 3 reorder 423; with short route_mode 431 (route takes EDI, NONE does not);
// first-exit `result = false;` in its branch without an initializer 270 (still folded); unit mode tested through
// the returned pointer 333; on that graph case 5 moved last in the mode switch 451, and the failure-label
// store dropped (default store kept) 380 (frame 0x84 again). Original body restored.
// Round 208: Scratch path receiver reuse with unit-output/source-settings/default-lifetime masks
// 0..7: 228/232/228/232/258/262/258/262. Original receiver, scopes and defaults restored; frame 0x84
// and result/output homes remain.
// Round 27: the route switch's real default and the failure block's own false reach retail's frame
// 0x80 but score 415; dropping the result initializer matches retail's prologue order and its frame
// yet loses retail's `xor bl,bl` before the 0x1f86a0 call (416); retail's mode 3 also sets route_mode
// = 1 and our case 3 does not (423 alone, 405 with the actor pointer hoisted); the original is kept.
// Round 109 (lane 6, fast; the reduced image scores the original 228 too): case 3 fixed to retail's
// order (point, node NONE, radius 0, route_mode = 1) 423 (1616 bytes; route_mode then takes EDI and
// NONE EBX); that with `default: result = false; break;` and `failed: result = false;` reached by the
// `!result` exit as well 270 (1636 bytes; frame 0x80 and the prologue are retail's, the first
// difference is the missing `xor bl, bl` before 0x1f86a0, which shifts every later row), with the
// `!result` exit jumping past that store 398; on the 270 form: short route_mode 431, the returned
// unit-mode test 333, both 432, `result = false;` inside the first exit with no initializer 270 (same
// code), `return false;` there 270; the fix with the real default only 380; t2 with the returned test
// 436. Retail's first exit keeps the constant in BL; every form here folds it to `xor al, al`.
// Round 53 (value bug confirmed from retail's jump table: case 3 jumps into case 5's tail, which also sets
// route_mode = 1): the early exits as one if/else-if chain to a shared `return result;` with `result = true`
// in the true paths 430, with the fix 440; that chain with no initializer and `result = false;` in the first
// branch 401, with the fix 406 (the first exit becomes retail's `xor bl, bl` / `mov al, bl` exactly, but the
// true paths then return through BL); only the first branch sharing the failure return 429/437 (folded), the
// success return 426/432 (EDI pushed before the first test); the fix alone 270.
// Round 246: early active/arrival/mode test-result subsets on the dword-selector
// graph 430/308/434/442/449/442/449; all with explicit failed false 435. The mode
// result recovers frame 0x80 and pool/data entry order at 442, first +0x33, but the
// first false still lacks BL and length grows to 1672. Original body restored.
// Round 120 (lane 6, fast), on the value-fixed graph (case 3 as case 5's tail with route_mode = 1, a
// real route default storing false, `failed: result = false;`: 270, aligned 70 against the original's
// 82): unit mode tested through e70b0's returned pointer with the first exit's `result = false;` in
// its branch and no initializer 333 (route_mode then takes EDI and the properties pointer spills,
// frame 0x84), that with a short route_mode 432; the whole body after the first exit in an `else`
// with one final `return result;` 437, without the initializer 437 (the first false still folds and
// EDI is pushed before the first test); unit_mode renamed a_mode/z_mode 270 each (same code: the slot
// stays at +0, so retail's +2 is not name order). Retail's route_mode is a dword slot read as a word
// (movsx/cmp word) and its result a memory byte everywhere but the first exit (BL).
// Round 476: Disabled admission-produced bool/byte/short/long return with ordinary false after unit 432/442/466/466; after actor resets 432/442/462/460.
// Frame 0x80 and entry pool/data order recover, first +0x27; extra predicate materialization and result homes regress. Original body restored.
// Round 51 (lane 11, fast): VALUE BUG still in this body (retail case 3 shares case 5's tail: node NONE,
// radius 0, route_mode = 1). On the value-fixed graph (case 3 in case 5's order with route_mode = 1, a real
// route default storing false, `failed: result = false;`) 270 (1636 B, aligned 70), the same without the
// result initializer 270; short route_mode 431 (it takes EDI from NONE); short route_mode with a named
// `reachable` (radius != 0 || node != NONE) test 400, with the negated `||` written inline 431; the named
// test alone 404; the first exit through a shared final `return result;` (if/else tail, one goto) 367
// (1652 B), with the initializer 371. Retail tests route_mode as a word (`movsx ecx, word`), materialises
// `node != NONE` with setne before its test, and keeps the first exit's false in BL.
// Round 476: Disabled admission-produced bool/byte/short/long return with ordinary false after unit 432/442/466/466; after actor resets 432/442/462/460.
// Frame 0x80 and entry pool/data order recover, first +0x27; extra predicate materialization and result homes regress. Original body restored.
// Round 19 (lane 18): retail's result byte shares unit_mode's slot (+0x12), so it has no definition
// before the unit-mode test; `failed:` stores false and the success test jumps past that store.
// Compile-only screen: `result = false;` inside the first arm with the rest of the body in an `else`
// and one final `return result;` gives retail's `xor bl, bl` / call / `mov al, bl` and keeps the early
// `return true` exits as `mov al, 1`, but only while the result stays in a register; in the real
// build it is memory-homed and the first false folds again. Real (fast), all on that else graph:
// `failed: result = false;` 429 (1648 B); with the route switch's real default 428 (frame 0x80, aligned
// 91 vs 82); with the case 3 fix too 446; that with the unit mode tested through 0xe70b0's returned
// pointer 451; the default form with the returned pointer 428. All restored.
// Round 476: Disabled admission-produced bool/byte/short/long return with ordinary false after unit 432/442/466/466; after actor resets 432/442/462/460.
// Frame 0x80 and entry pool/data order recover, first +0x27; extra predicate materialization and result homes regress. Original body restored.
// Round 68 (lane 12): the value-fixed 270 form rebuilt (exact LCS 44 unmatched against the original's 59;
// EDI holds NONE and route_mode is a memory dword, as retail). Retail evidence on it: the unit mode is
// tested through 0xe70b0's returned pointer (`cmp word ptr [eax], 5`), route_mode is read as a signed
// short (`movsx`, jle/jg), the zero-route node test is a materialized bool (`setne al; test al, al`),
// and 0x1f86a0 takes no BL argument (so the first exit's BL is the result itself; 0x262800 does take
// its bool in BL). On 270: returned unit-mode test 333 (route_mode then takes EDI, NONE EBX); first
// exit by goto to a shared final `return result;` 437 (still folded, EDI pushed early); actor fetched
// once before the first test 277; short route_mode with the returned test 432. Original body kept.
// Round 25 (lane 18, fast, on the value-fixed 270 graph): the zero-route and range tests as one ternary
// condition 427; the zero-route test nested under `if (route_mode == 0)` 270 (same code); the early exits as an
// if/else-if chain to one final `return result;` (false before 0x1f86a0, true after the calls) 406: the first
// exit is retail's `xor bl, bl` / `mov al, bl` exactly, but the true exits then return through BL; only the
// first exit reaching that final return 270 (folds again: it needs a second value at the join); result
// initialized from the first test 435. Retail's true exits are `mov al, 1`, so the first exit's join partner
// is still unknown. Original body restored.
// Round N284: Missing-destination conjunction orders route-node-radius/radius-route-node/
// radius-node-route/node-route-radius/node-radius-route 228/377/254/307/308; nested
// route/radius/node admission 228/377/307. Original gates, values and complete body restored.
// Round 476: Disabled admission-produced bool/byte/short/long return with ordinary false after unit 432/442/466/466; after actor resets 432/442/462/460.
// Frame 0x80 and entry pool/data order recover, first +0x27; extra predicate materialization and result homes regress. Original body restored.
// DL225: distinct scratch-conversion result, bool/byte/short/long, published before/after
// release 424/424/424/424/432/432/433/433. All restored; frame 0x84, early false and
// unit-output/route/result homes remain.
// Round 50 (lane 14, fast; aligned counts from a difflib listing, base 82). Value fix for mode 3 (route_mode = 1, tail
// order point, node, radius), `default: result = false;` and `failed: result = false;` with the `!result` exit past that
// store 398 (1644 bytes, aligned 69, frame 0x80); with no result initializer 398; with short route_mode 426 (1636,
// aligned 94: route takes EDI, the properties pointer spills); unit mode through the returned pointer, dword route 436
// (1628), scoped 434, long and scoped 430, short and scoped 423; scope alone 398, the unit index as the call's
// argument 398. A long copy of a short route_mode for the zero test, range test and route switch 267 (1640, aligned
// 66): route then spills with dword stores and word reads as retail. On that: radius set through a helper 267; mode 7's
// radius before its node 440 (1632, aligned 59: cases 2/4 and 3/5 then merge their tails as retail); mode 2 or 4 radius
// first 268, both 269, modes 2 and 7 412 (1648). On the 59 form: the zero and range tests nested if/else 440 (aligned
// 68), one test route-zero first 440 (59), range first 420 (1640, aligned 71), as a ternary 440, a valid flag 440, a
// has-node flag 440; default after case 5 or first 440; source order 6,7 first 449 (1620), 7,6 last 458 (1612), 5
// last 459 (1612). Left on that form: retail lays the route test and compare chain directly after case 5, with
// cases 6 and 7, the zero test and the failure block after it; ours has them after case 7. Also left: `xor bl, bl` kept
// across 0x1f86a0, unit mode at frame +2 read through the returned pointer, and `setne al` in the zero test.
// DL227: Complete result carriers on the dword selector graph: byte entry/late 376/379,
// signed-byte 376/379, short 410/413, word 410/413. Original result and body restored; frame
// 0x84, early false and unit-output/route/result homes remain; recorded mode-three value question
// unapplied.
// DL229: complete destination-case orders 246753/247653/264753/267453/274653/276453/245763/254763
// 379/375/379/379/375/379/379/379. Original case graph, dword selector and body restored;
// frame, early pool/data staging and unit-output/result homes remain.
// DL234: complete complementary route outcomes in bool/byte/short/long, initialized at
// entry 411/411/418/419 or at the route phase 438/438/445/446. All exact-only forms rejected;
// original result and body restored. Frame 0x84, early false and unit-output/route/result homes remain.
// J239: native active/pending/route-request fields on the unsigned selector, seven subsets [228, 228, 228, 228, 228, 228, 228];
// all with existing returned unit-mode output 257. Original external view, observations and body restored.
// B381: complete existing path-query record begun at scratch/case/route/destination/reset/unit/actor/entry
// phases 228/228/228/228/228/228/228/228. All tie; NOT kept: grouped homes require an exact match. Original frame 0x84,
// pool/data scheduling and unit-output/result packing remain.
// Round 45 (m3 lane 2, fast, base 228; aligned counts from a difflib listing, base 59): retail's failure block stores
// result = false and the `!result` exit enters after that store. `failed: result = false;` with the `!result` exit
// as its own `function_1f8750` + `return result;` block 397 (1648 bytes, frame 0x80; that exit folds to `xor al,
// al`); the `!result` exit to a second label after the store (diagnostic, gotos only) 250 (1652 bytes, frame 0x80,
// aligned 36: result and unit mode then share one slot as retail, at +0x10 for retail's +0x12). On that: short
// route_mode 425; `default: result = false;` 425 (1664); signed long route_mode 417. Left on it: retail's first
// exit keeps false in BL across 0x1f86a0, the returned unit-mode pointer, word reads of the dword-stored
// route_mode, the route test and switch laid out after case 5, and the zero test's `setne al`.
// M3 lane 4 r157 (fast): the unit index passed straight to 0xe70b0 232; the three resets through `actor` (no second
// actor_get) 431; the 0x5d2 flag stored before unknown504 230; the movement getter before the properties getter 232;
// the zero-route node test before the radius test 228 (same code).
// Round 139 (m3 lane 6, fast, control 228): retail's result byte and unit_mode share frame +2 (the result is
// the low byte of unit_mode's slot), so their scopes are disjoint in retail. The reset stores through `actor`
// instead of a fresh lookup 431; the unit index read inside the 0xe70b0 call 232; the route initializer as
// `actor->unknown229 != 0` 228 (same code); as `route_mode = 0; if (...) route_mode = 1;` 265 (1660 B);
// the route initializer before the unit index 232.
// Round 141 (m3 lane 6, fast, control 228; aligned 82): unit_mode declared at the function's top 228 (same code);
// the path cast at the top 228; case 2's radius stored first 228; retail's failure block (`mov byte [esp+0x12], 0`,
// the call, `mov al, [esp+0x12]`) read as a cross-jumped tail: `if (result) { ...; return result; }
// function_1f8750(actor_index); return result; failed: result = false; function_1f8750(actor_index); return
// result;` with no initializer and the first exit `return false` 417 (1644 bytes, frame 0x80, prologue as retail,
// aligned 69), with `result = false;` before the first exit's call 417 (still folded to `xor al, al`).
// Round 52 (m3 lane 15, fast; aligned counts from a difflib listing, base 65): VALUE BUG fixed (case 3 stores node NONE, radius 0
// and route_mode = 1, as case 5's tail). A new cause: retail's first exit (`xor bl, bl`, call, `mov al, bl`) comes from two
// variables, an outer result (false in the first arm, assigned from the route result at the end of the `else` arm, one
// final return) and an inner route result; first difference +0x0 -> +0x55 and frame 0x80, with early `return true`s kept.
// On that graph: the tail joined `result = route_found` once after the if/else 382 (aligned 51), in each arm with a goto past
// the failure block 382 (82); a flag for the failures, tests `<0 || >2` 409 (57) / plain 409; short route_mode with a long
// copy for the tests and the route switch 402 at aligned 53 (route spills with dword stores and word reads, EDI holds NONE,
// as retail); mode 7's radius before its node 343 (cases 2/4 and 3/5 merge their tails as retail; node first 361, aligned 39,
// with case 2, case 3 or a radius local reordered 363-371); a failure flag with the result set in each arm 199 (aligned 46); with
// the unit mode tested through 0xe70b0's returned pointer in its own block 188 (aligned 37, masked 34; without the block frame
// 0x84, 206); the flag replaced by `route_found = false;` under an else on the route test, reached by the existing gotos, 188 (kept,
// same code). Case 6 nested or with breaks 188/188;
// the route switch's `__assume(0)` default 327 (retail stores false there); `result = route_found;` once after the
// if/else 327; a `bool route_found = false;` initializer 427 (frame 0x84); the failure block after the final return 453
// (first exit folds); case 6's `route_mode = 1` jumping to case 5's store (goto, diagnostic) 361, VC copies the store back;
// cases 6, 7 first in the switch 383 (laid out first); 0x1caa40 called in both arms of the owner test 185 (retail's `lea esi` before the
// branch; a gain of 3, not kept); address-taken or volatile route result (diagnostics) 188/322/337: no slot change.
// Left: retail lays the route test and chain directly after case 5 (cases 6, 7, zero test and failure block after it, `jne`
// to case 5's store from case 6); mode 7's node store before the radius; `setne al; test al, al` in the zero test; the failure
// block's memory store and reload of the result (ours folds to BL) and the unit mode's slot (+0x10 for retail's +0x12).
// Round 143 (m3 lane 6, fast; aligned base 82): no initializer, the first exit `result = false;` + 0x1f86a0 + `goto
// done;`, `!result` jumping to a `release:` label past `failed: result = false;`, one `done: return result;`, success
// `return result;` 429 (1656 B, aligned 71); success `goto done;` 366 (1656 B, frame 0x80, aligned 66: the first exit
// stays its own tail and no longer folds to `xor al, al`, but keeps the result in a stack byte, not BL; result and unit
// mode share a slot at +0x10 for retail's +0x12, route's byte neighbour +0x14 for +0x16); `result = false;` after the
// 0x1f86a0 call 366; with the route default storing false 388 (1672 B); with case 3 in case 5's order and route_mode = 1
// 413 (1624 B, aligned 99).
// M368: Terminal outer completion before success/cleanup, bool/byte/short/long,
// guarded by inner/outer result: 318/318/462/462/488/486/491/484.
// NOT kept: all regress or grow beyond retail; original late assignments restored.
// Unit/result +0x10 versus +0x12 and route-tail placement remain.
// AC521: Independent inner route outcome on the two-result/native-word-selector graph,
// byte/signed-byte/short/long with converted or test-arm outer results: 191/191/191/191/300/300/306/306.
// NOT kept: no exact memory-result home; original outer/inner results, unit scope and full body restored.
// AC530: Uniform complete frame-member pointer at active/route/unit/reset phase boundaries
// 506/505/505/505; corresponding real frame references 506/505/505/505 (fast control 188).
// NOT kept: all regress and lose native entry order; original field observations and full body restored.
// AC532: Shared destination case-family producers 2/3, 4/5, both: conditional nodes
// 373/384/409, branch nodes 370/380/411; both with wide selected mode 441/411.
// NOT kept: no match; original cases, live observations, results and body restored.
// Y576: Full-record/fallback/failure-first branch subsets 187/211/305/210/304/305/304;
// all with shared modes-two/three destination prefix 369. NOT kept: one-point best gain;
// route dispatch, expired unit/result home and failure-result storage still differ. Original body restored.
// P325: On the two-result graph, inner-result declarations after unit/reset/destination,
// at entry and at route selection all tie 188; unit-output declarations at active phase/entry
// gave 206/206; outer failure result assigned before cleanup 188. All restored;
// unit/result packing and the native memory-result cleanup remain.
// N367: Early active/arrival/mode success joins on the two-result word-selector graph, seven subsets
// 424/418/425/446/443/441/316; complete joins with terminal literals 306. NOT kept: all regress; original
// early exits and result assignments restored. Unit/result +0x10/+0x12 and late route-result home remain.
// Round 129 (m3 lane 7, fast, control 188 = full; aligned = difflib count on normalized lines, control 37): causes found, none keepable alone.
// (1) One result variable (no route_found) gives retail's failure block with `mov [m], 0` / reload: 340 (1680 B, aligned 47); its first exit
// folds to memory or `xor al, al`. Retail's `xor bl, bl` .. `mov al, bl` needs a second value at the final return: `result = true; goto done;`
// on the 0x480 test and the mode 0/1 exit, the mode-2 exit `return true;`, the route phase `return result;`, one `done: return result;` 312
// (1628 B, 434 lines as retail, aligned 37, masked 28): first exit exact, one exit still `mov bl, 1`. All three exits by goto 288 (1632 B, aligned
// 42); mode-2 only 286 (41); x480 only 429; x480 + mode-2 293; mode-2 + mode 0/1 429. Route phase in `do {} while (false)` from the route_mode or
// the switch 442/327 (first exit exact but the returns read BL; the former inlines 0xe70b0); returns inside that region 447 (first false folds);
// a tested `result = !flag` 466/450, flag-then-test forms 447.
// (2) `bool valid` set in if/else on the route test gives retail's `setne al; test al, al` 309 (1636 B, aligned 39); as `if (route != 0)` 311; initialised
// true or false, or with the failing arm first, the same; a named has-node flag folds 188; the ternary reversed 188 (aligned 46); `route > 0 ?` 307.
// (3) Layout: when the join follows case 5, case 6's store cross-jumps to case 5's by itself (`jne`, cases 6,7 first in source 383 shows it), so the
// store is an effect. On matched 0x2ad5f0 (scratch perturbations, restored) all stores distinct puts the join behind the first case; a shared store
// pulls it behind that case. Here, on the 312 form: the join stays after case 7 with case 2 or 4 radius-before-node 357/356, case 3 or 5 likewise 345,
// shared `goto direct` tails 343/345/312, a larger shared tail 397, the failure block as the mode switch's default 312, no default 389, short route_mode
// volatile, address-taken, in an array or in a one-member struct 312/312/396/327/327 (all same stores: a memory-homed short is retail's dword store
// with word reads), any shape of case 6's nesting 312 (five forms, same code). Only dropping `if (!movement) goto failed;`, or all three target tests,
// moves the join (behind case 6's then-arm, 348-435, not the same behaviour, diagnostic). A `valid` flag through the mode switch with `break`s also moves
// it behind case 6's then-arm 450-467 (flag in BL, not threaded to the failure block). A long route_mode with its address escaped and (short) reads 396, a 10-byte field store after each `route_mode = 1` 305: join stays.
// Left: why retail's join follows case 5; then the unit/result slot +2.
// Y585: Scratch admission owns inner route outcome, bool/byte/short/long, success-first
// 226/226/302/291; failure-first 249/249/302/291. NOT kept: all regress;
// original two results, scratch default, scopes and full body restored. Unit/result packing and dispatch remain.
// B437: Actual radius-first route admission in bool/byte/short/long valid storage
// 309/309/308/308, complementary invalid storage 309/309/308/308. NOT kept:
// all regress; unit/result packing, dispatch adjacency and memory-result cleanup remain.
// P337: Current two-result word-selector lexical unit/selector/inner-result home subsets
// 1/2/4/3/5/6/7, then all with outer-result name, each 188. NOT kept: no byte match;
// native unit/result +0x12 and late memory result remain. Original full body restored.
// M381: Independent outer byte/short/long/dword completion after terminal stores/cleanup,
// route-result copies then branch facts: 435/429/432/432/435/429/432/432. NOT kept: result-home
// diagnostics regress; original outer/inner results, native-word selector and full body restored.
// DL273: Outer completion at directed/reference/path call producers, seven subsets and
// all with existing false producers: eight 188s. NOT kept: no byte match; original
// two-result body restored. Unit/result homes, dispatch and late memory result remain.
// M3 lane 11 r71 (fast, control 188 at 1632 bytes). Retail: cases through 5, the route dispatch, cases 6 and 7, then
// the zero test and a failure block with its own 0x1f8750 call and return. Route test nonzero side first: `route != 0
// ? ... : ...` 188 (retail's `je` to the zero test), `route ? ... : ...` 188, assigned bool 188, `(route != 0 && ...)
// || (route == 0 && ...)` 188, `... || (route == 0 && ...)` 309, `(route == 0 && ...) || ...` 188 (register-blind
// aligned 44 for 56). Failure block in the default arm with its own call: returning there 453, with the zero test
// first 475; through `result` and a goto to the only return 370, with the zero test first 319.
// Round 133 (m3 lane 7, fast; aligned = difflib count, control 55): route_mode assigned inside the unit-mode block 188
// (same code); the route result declared just before the mode switch 188 (same code); the path case's result as its own
// local copied out after the release 327 (1616 bytes, aligned 63); the route test and switch on route_mode directly, no
// long copy, 398 (aligned 125); case 7 placed before case 6 in the mode switch 335 (1608 bytes, aligned 107). Retail
// also keeps 0x10ff40's 0xe70b0 short at frame +2, so the +2 slot is not particular to this body. Body unchanged at 188.
// Round 218 (m3 lane 3, fast; masked = register/stack-masked aligned line pairs x2, control 53): the route test inverted
// with the failure arm (and its label) first 188 (same code); case 6's inner test with the `route_mode = 1` arm last
// (`if (!(*movement & 0x20)) { ... = 2; } else route_mode = 1;`) 327 at 1616 bytes, masked 50: case 6 then jumps into
// case 5's `route_mode = 1` store as retail does, but the join still follows case 7; on that, case 7 ahead of case 6
// 304 (masked 136), the default label first 327 (same code), the route admission through an if/else `valid` flag 327
// (same code). Left: the join after case 5, then the result byte's +0x12 home and the failure block's memory result.
// T72: Complete six-destination conditional chains, one captured selection, branch orders
// 234567/765432/236745/456723/452367/672345/674523/325476:
// 369/371/377/385/369/388/387/397. NOT kept: no native dispatch/home equality;
// original switch, all case bodies, two results and complete body restored.
// P393: Both route root-override admissions, positive/complementary bool/byte/short/long:
// 188/188/188/188/188/188/188/188. NOT kept: one-use outcomes do not match; unit/result +0x12,
// route dispatch and late memory result remain. Original complete two-result body restored.
// M3 lane 16 r97: kept 188 -> 182 (plain): 0xe70b0 returns its two-byte action state by value (a record with a
// converting constructor; retail's callee takes the result pointer in ECX and returns it in EAX), so the unit mode
// here is the call's temporary and sits at frame +0x12 as retail, sharing the route result's byte. Measured first
// with a one-caller inline wrapper returning such a record 182 (diagnostic), a two-short array with the mode second
// 206 (frame 0x84). With a default constructor and a named local returned, matched 0x1afb30 (which inlines the getter)
// read 114; the converting-constructor form keeps it matched. On the 182 body (fast; aligned = difflib count with
// targets masked, 31 here, 37 before): case 7 storing the destination node before the radius (retail's store order)
// 360 (1640 bytes, aligned 32); the route admission in a named bool 182 (same code), both 360; the route test nonzero
// side first 182 (aligned 39); case 6's inner test with the `route_mode = 1` arm last 326 (1616 bytes, aligned 30),
// both 326 (aligned 38). Left: the route dispatch follows case 7 (retail: after case 5, with cases 6 and 7 behind it,
// then the zero test and the failure block), and the final result goes through BL where retail returns its frame byte.
// Round 143 (m3 lane 7, fast): the mode cases in source order 672345 382, 234675 372, 234576 334,
// 567234 388, the default label first 182 (block layout follows source order); `result = route_found;`
// once after the final if/else 326, first with the test on it 317, the failing arm first 369.
// M3 lane 9 r110 (fast with 0x1f86a0, 0x1f8750, 0x1f8780, 0x1f8940, 0x1f90f0, 0x1f8510, 0x1f9580, 0x1f9490, 0xe70b0, all
// matched; control 182): the mode switch on the actor view's unknown4ac instead of the frame view's mode 182, the route
// case's mode tests through unknown4ac 182, both 182, `long route = route_mode;` as one declaration 182 (all the same
// code). Retail's order: cases 2-5, then the route test head, its range checks and the route dispatch (with its false
// default), then cases 6 and 7 (both jumping back to the head), the zero-route test, the failure block (false stored,
// 0x1f8750, the result reloaded), route cases 2, 1, 0 and the final block, whose not-found branch enters the failure
// block after its store.
// M3 lane 3 r231 (fast, control 182). Re-measured on this body: the failure block as the mode switch's default (round 129's
// form; here `default: failed: route_found = false; goto finish;` with `finish:` before the final test) 182 (same code: the
// block is threaded away). New: the success arm returning `route_found` itself 310 (the first exit folds); the route admission
// as its own switch setting a bool (case 0 the zero test, cases 1-2 true, default false) 182 (same code as the nonzero-first
// ternary); the zero test as `!(radius == 0.0f && node == NONE)` 182 (same code); the unit action state in a named local 199
// (homed at +0x10, compared from the frame); the route initializer as `actor->unknown229 ? 1 : 0` 182 (same code). Layout lead
// (diagnostic on a listed form): with the default as its own 0x1f8750 call and `return false;` (453, a duplicated failure
// block) the route test moves behind case 5's shared `route_mode = 1` tail with cases 6 and 7 after it, as retail; so the join
// leaves case 7 only when a block that survives threading follows case 7 in source. Retail's zero test, failure store and
// failure call after case 7 fit that if the failure block (its false store and the memory reload after 0x1f8750) is that
// block; ours folds the false, so the block vanishes. A compile-only /Gr harness keeps this layout rule.
// m3 lane 11 r90 (fast with the file and the eight callers, control 182): re-measured lane 3 r231's success arm returning route_found,
// written with the success test nested in the route block and `failed:` just before the shared 0x1f8750 tail, 310 (the first exit folds
// to `xor al, al`). Retail's tail stores false, calls 0x1f8750 and reloads the result from its frame byte, so the result's value is
// unknown there in retail. Diagnostics: route_found cleared through memset (address taken) 326 at 1680 bytes (store and reload survive,
// the route test still follows case 7); the failure block as the mode switch's default with `failed:`/`release:` labels and the end's
// `!route_found` jumping to `release:` 438 at 1640 bytes (first exit folds). No existing helper covers the three-field reset
// (actor_reset_state and 0x1f86a0 also clear 0x5b4/0x5b6/0x4ac); a scan of include/ found no other helper this body writes out.
// M3 lane 2 r72 (fast with the file's ten matched functions, all matched throughout; aligned/masked in brackets, base
// [49/47]): `long route = route_mode;` as one declaration 182 (same code); the failure block (false store, 0x1f8750 call,
// `return route_found;`) as the mode switch's default with the route test's else as `goto failed;` 453 (1668 bytes)
// [109/89], that with the join's failing arm also `return route_found;` after its own call 435 (1644 bytes) [99/79] (the
// two calls are not cross-jumped and the default's false still folds); case 6's `route_mode = 1` arm last together with
// case 7's node stored before its radius 366 (1624 bytes) [54/52]. Diagnostic: `(void)&route_found;` 182 (same code:
// taking the address alone does not keep the false store or the reload after 0x1f8750).
// T90: Destination case orders 324567/342567/423567/432567/245367/253467/352467/425367: 350/366/378/378/359/362/350/378.
// NOT kept: No native dispatch/result equality; original case order, every case body and full function restored.
// M3 lane 10 r97 (fast with the file's ten other functions and 0xe70b0, all MATCH throughout; base 182): nothing kept. Ours keeps
// route_found in the shared +0x12 frame byte as retail does (a store after each route call) but folds the failure block's false
// across 0x1f8750; retail reloads the byte there, and its zero-route test materializes the node test (`setne al; test al, al`).
// route_found typed byte 184; declared at function scope 182; case 6's `route_mode == 0` test nested around the 0x1f9580 call 182
// (same code); the zero-route test through a new inline bool helper (diagnostic) 381 (no setne, the layout moves); the first exit's
// 0x1f86a0 call before `result = false;` 447 (1624 bytes).
// P434: complete adjacent-family orders 236457/237456/237645/452376/456237/457236/457623/623457
// gave 211/364/357/388/234/387/383/370. NOT kept: all regress; original cases and full body restored.
// Native frame, first false BL and unit/result +0x12 home agree; dispatch and late false reload remain.
// T96: Destination case orders 235467/254367/325467/345267/354267/435267/453267/523467
// on the native returned-unit graph: 355/366/367/369/225/233/373/378. NOT kept:
// no native dispatch/result equality; original six cases, two results, shared unit
// temporary and full body restored.
// M3 lane 1 r99 (fast with the file's ten other functions and 0xe70b0, all MATCH throughout; base 182; block order read off the listing).
// New cause for the layout: retail's `route_mode = 1` store at 0x1f8c9d is one statement behind the mode switch, reached by cases 3 and
// 5 and by case 6's 0x20 test and jumped over by cases 2, 4 and 7. With a `direct` flag set in those cases and `if (direct) route_mode
// = 1;` behind the switch, the route test and what follows it land behind case 5 with cases 6 and 7 after them, at retail's position
// (the first 154 lines agree): 315 at 1624 bytes, because the rest still differs (ours lays the failure call behind the first tested
// operand and the route dispatch behind the second; retail has the dispatch behind the range test and the zero test, false store and
// failure call behind case 7). A flag set in the cases and tested once is kept only with a match, so it is recorded, not kept. On
// the flag form: nonzero route first 322; the route test inverted with the failure block (false store, 0x1f8750, the result) behind
// it and the final test's failing path entering after the store (two gotos) 309 (store and reload then as retail, but through BL);
// that as an if/else-if with the zero route first 302; as its own switch ahead of the route switch (three case orders) 309; as an
// or of two and-chains 309; with a validity flag (four spellings) 309/315/322, none gives `setne`; the zero test's result in a named
// bool 309; through an inline bool helper 393; the range failure storing its own false 309; the range test as its own statement
// first 292 (dispatch behind the zero test, failure block last); the zero test and failure block written in the mode switch's
// default arm 302; case 7's node stored before its radius 315 (same code); the failure block and the success path returning the
// route result itself 454 (retail's `mov al, [esp + 0x12]` at both, but the first exit folds). Every spelling of the route test
// comes out as one of three layouts, so the compiler normalises it; what still decides dispatch-or-failure behind the range test
// is open. Without the flag: an empty inline taking the route result's address 182 (same code); its address passed to a real
// call (diagnostic) 498 (frame 0x84, store and reload survive); the inverted test with a shared release tail 306 (four spellings,
// same code); the mode switch's default as its own 0x1f8750 call and return 453 (the route test then sits behind case 6), with the
// nonzero route first 477; validity flags 182 (same code). Body unchanged at 182.
// P466: Destination orders 263475/273645/273465/627345/362745/372645/632745/637245:
// 367/380/380/387/368/363/373/378. NOT kept: all regress and shorten away from
// native extent; original cases, shared unit output, two results and full body restored.
// T107: Overlap-safe library transfers at direct/reference/state/fallback phases
// 395/392/379/215; paired direct-reference/reference-state 417/408, all destinations
// 448, with fallback 457. NOT kept: native transfer/layout mismatch; original body restored.
// @retail 0x1f8a70
bool __stdcall function_1f8a70(long actor_index, long unknown)
{
	bool result;
	if (actor_get(actor_index)->unknown267)
	{
		result = false;
		function_1f86a0(actor_index);
	}
	else
	{
		bool route_found;
		s_actor_view *actor = actor_get(actor_index);
		if (*((byte *)actor + 0x480))
			return true;
		if (actor->unknown504 == 2 && function_1f3230(actor_index, function_1e3920(actor_index) * 3.0f))
		{
			function_1f8780(actor_index, false);
			return true;
		}
		if (actor->unknown4ac == 0 || actor->unknown4ac == 1)
		{
			function_1f86a0(actor_index);
			return true;
		}

		short route_mode = actor->unknown229 != 0;
		{
			if (function_e70b0(actor->unknown018).value == 5)
			{
				byte *properties = (byte *)function_1e4990(actor->unknown054);
				byte *movement = (byte *)function_1e4a50(actor->unknown054);
				if (properties && !(*properties & 2) && movement && (*movement & 0x20))
					route_mode = 1;
			}
		}
		{
			s_actor_view *reset = actor_get(actor_index);
			reset->unknown50c = false;
			reset->unknown5ac = NONE;
			reset->unknown5b0 = NONE;
		}
		actor->unknown504 = 0;
		*((bool *)actor + 0x5d2) = false;
		s_type_f17a25 *path = (s_type_f17a25 *)unknown;
		switch (((s_1f8a70_actor *)actor)->frame.mode)
		{
		case 2:
			((s_1f8a70_actor *)actor)->frame.destination = ((s_1f8a70_actor *)actor)->frame.target.point;
			((s_1f8a70_actor *)actor)->frame.destination_node = ((s_1f8a70_actor *)actor)->frame.source_node;
			((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
			break;
		case 3:
			((s_1f8a70_actor *)actor)->frame.destination = ((s_1f8a70_actor *)actor)->frame.target.point;
			((s_1f8a70_actor *)actor)->frame.destination_node = NONE;
			((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
			route_mode = 1;
			break;
		case 4:
		{
			s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
			if (!target)
				goto failed;
			((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
			((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)target + 0x14);
			((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
			break;
		}
		case 5:
		{
			s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
			if (!target)
				goto failed;
			((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
			((s_1f8a70_actor *)actor)->frame.destination_node = NONE;
			((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
			route_mode = 1;
			break;
		}
		case 6:
		{
			s_262b40_result *target = function_262b40(((s_1f8a70_actor *)actor)->frame.target.reference);
			if (!target)
				goto failed;
			byte *movement = (byte *)function_1e4a50(actor->unknown054);
			if (!movement)
				goto failed;
			if (*movement & 0x10)
			{
				((s_1f8a70_actor *)actor)->frame.destination = *(s_type_c3b527 *)target;
				((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)target + 0x14);
				((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
				if (*movement & 0x20)
					route_mode = 1;
				else if (route_mode == 0 && function_1f9580(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference))
					route_mode = 2;
			}
			break;
		}
		case 7:
		{
			long prop_index = ((s_1f8a70_actor *)actor)->frame.target.index;
			s_prop_node_view *node = prop_node_get(prop_index);
			function_26c2d0(prop_index);
			s_prop_state_view *state = prop_node_state(node);
			if (route_mode == 1)
			{
				((s_1f8a70_actor *)actor)->frame.destination.point = *(point3f *)((byte *)state + 0x10);
				((s_1f8a70_actor *)actor)->frame.destination.output_index = NONE;
			}
			else
				((s_1f8a70_actor *)actor)->frame.destination = state->unknown48;
			((s_1f8a70_actor *)actor)->frame.destination_radius = ((s_1f8a70_actor *)actor)->frame.radius;
			((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)state + 0x44);
			break;
		}
		default:
			goto failed;
		}
		long route;
		route = route_mode;
		if (route == 0 ? (((s_1f8a70_actor *)actor)->frame.destination_radius != 0.0f || ((s_1f8a70_actor *)actor)->frame.destination_node != NONE) : (route > 0 && route <= 2))
		{
			switch (route)
			{
			case 2:
				route_found = function_1f9490(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, (s_actor_move_request *)((byte *)actor + 0x50c));
				break;
			case 1:
			{
				s_reference reference = g_470fa0;
				long root_index = NONE;
				if (((s_1f8a70_actor *)actor)->frame.mode == 4 || ((s_1f8a70_actor *)actor)->frame.mode == 5 || ((s_1f8a70_actor *)actor)->frame.mode == 6)
					reference = ((s_1f8a70_actor *)actor)->frame.target.reference;
				else if (((s_1f8a70_actor *)actor)->frame.mode == 7)
					root_index = function_baf80(prop_node_get(((s_1f8a70_actor *)actor)->frame.target.index)->object_index);
				if (((s_1f8a70_actor *)actor)->frame.root_index != NONE)
					root_index = ((s_1f8a70_actor *)actor)->frame.root_index;
				point3f position;
				function_1caa40(actor->unknown26c == NONE ? actor->unknown018 : actor->unknown26c, &position);
				s_29de21 *context = path ? (s_29de21 *)((byte *)path + 0x140d4) : NULL;
				route_found = function_29e730((s_29e730 *)&((s_1f8a70_actor *)actor)->frame, actor_index, &position, &((s_1f8a70_actor *)actor)->frame.destination,
					reference, context, root_index, (s_29e0f9 *)((byte *)actor + 0x50c));
				break;
			}
			case 0:
				if (path)
				{
					function_2713c0(path, &((s_1f8a70_actor *)actor)->frame.destination, ((s_1f8a70_actor *)actor)->frame.destination_node, ((s_1f8a70_actor *)actor)->frame.destination_radius);
					route_found = function_270930(actor_index, path, (s_path_result_view *)((byte *)actor + 0x50c));
				}
				else
				{
					s_path_source source;
					s_path_settings settings;
					route_found = false;
					function_1f90f0(actor_index, &source);
					if (((s_1f8a70_actor *)actor)->frame.root_index != NONE)
						source.unknown0c = ((s_1f8a70_actor *)actor)->frame.root_index;
					if (actor->unknown358 > 0 && !actor->unknown35e)
						function_2705c0(&source, &actor->unknown370, actor->unknown36c, actor->unknown360, 10.0f, true);
					function_1f9240(actor_index, &settings);
					byte *buffer = ai_scratch_buffer_get();
					s_type_f17a25 *state = (s_type_f17a25 *)buffer;
					function_271300(state, (s_path_location *)((byte *)actor + 0x5b4), &settings, &source, 0);
					function_2713c0(state, &((s_1f8a70_actor *)actor)->frame.destination, ((s_1f8a70_actor *)actor)->frame.destination_node, ((s_1f8a70_actor *)actor)->frame.destination_radius);
					if (function_2715a0(buffer))
					{
						route_found = function_270930(actor_index, state, (s_path_result_view *)((byte *)actor + 0x50c));
						if (!route_found && actor->unknown5b2[0] && ((s_1f8a70_actor *)actor)->frame.mode == 4)
							function_262800(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, false);
					}
					else
						*(s_path_location *)((byte *)actor + 0x5b4) = *(s_path_location *)(buffer + 0x70);
					ai_scratch_buffer_release(buffer);
				}
				break;
			default:
				route_found = false;
				break;
			}
			actor->unknown506 = true;
		}
		else
		{
failed:
			route_found = false;
		}
		if (route_found)
		{
			actor->unknown504 = 1;
			actor->unknown5e8 = 0;
			result = route_found;
		}
		else
		{
			function_1f8750(actor_index);
			result = route_found;
		}
	}
	return result;
}

