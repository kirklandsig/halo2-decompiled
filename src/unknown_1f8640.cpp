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
short *function_e70b0(long unit_index, short *value);
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
// score 228 (was 228): Frame 0x84 versus 0x80; pool/data entry order and unit-output/route/result homes differ.
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
// @retail 0x1f8a70
bool __stdcall function_1f8a70(long actor_index, long unknown)
{
	bool result = false;
	if (actor_get(actor_index)->unknown267)
	{
		function_1f86a0(actor_index);
		return result;
	}
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

	short unit_mode;
	long unit_index = actor->unknown018;
	dword route_mode = actor->unknown229 ? 1 : 0;
	function_e70b0(unit_index, &unit_mode);
	if (unit_mode == 5)
	{
		byte *properties = (byte *)function_1e4990(actor->unknown054);
		byte *movement = (byte *)function_1e4a50(actor->unknown054);
		if (properties && !(*properties & 2) && movement && (*movement & 0x20))
			route_mode = 1;
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
		((s_1f8a70_actor *)actor)->frame.destination_radius = 0.0f;
		((s_1f8a70_actor *)actor)->frame.destination = ((s_1f8a70_actor *)actor)->frame.target.point;
		((s_1f8a70_actor *)actor)->frame.destination_node = NONE;
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
		((s_1f8a70_actor *)actor)->frame.destination_node = *(long *)((byte *)state + 0x44);
		((s_1f8a70_actor *)actor)->frame.destination_radius = ((s_1f8a70_actor *)actor)->frame.radius;
		break;
	}
	default:
		goto failed;
	}
	if (route_mode == 0 && ((s_1f8a70_actor *)actor)->frame.destination_radius == 0.0f && ((s_1f8a70_actor *)actor)->frame.destination_node == NONE)
		goto failed;
	if (route_mode < 0 || route_mode > 2)
		goto failed;
	switch (route_mode)
	{
	case 2:
		result = function_1f9490(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, (s_actor_move_request *)((byte *)actor + 0x50c));
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
		result = function_29e730((s_29e730 *)&((s_1f8a70_actor *)actor)->frame, actor_index, &position, &((s_1f8a70_actor *)actor)->frame.destination,
			reference, context, root_index, (s_29e0f9 *)((byte *)actor + 0x50c));
		break;
	}
	case 0:
		if (path)
		{
			function_2713c0(path, &((s_1f8a70_actor *)actor)->frame.destination, ((s_1f8a70_actor *)actor)->frame.destination_node, ((s_1f8a70_actor *)actor)->frame.destination_radius);
			result = function_270930(actor_index, path, (s_path_result_view *)((byte *)actor + 0x50c));
		}
		else
		{
			s_path_source source;
			s_path_settings settings;
			result = false;
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
				result = function_270930(actor_index, state, (s_path_result_view *)((byte *)actor + 0x50c));
				if (!result && actor->unknown5b2[0] && ((s_1f8a70_actor *)actor)->frame.mode == 4)
					function_262800(actor_index, ((s_1f8a70_actor *)actor)->frame.target.reference, false);
			}
			else
				*(s_path_location *)((byte *)actor + 0x5b4) = *(s_path_location *)(buffer + 0x70);
			ai_scratch_buffer_release(buffer);
		}
		break;
	default:
		__assume(0);
	}
	actor->unknown506 = true;
	if (!result)
		goto failed;
	actor->unknown504 = 1;
	actor->unknown5e8 = 0;
	return result;
failed:
	function_1f8750(actor_index);
	return result;
}

