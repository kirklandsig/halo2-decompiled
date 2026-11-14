#include <string.h>
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1A9B50.CPP: the slot handlers of types 0x42, 0x18, 0x19, 0x10 and
   0x11 (0x47f7f0, 0x47da7c..0x47db48) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"

/* the actor's tag entry function_1e4f90 returns (0x40 bytes) */
struct s_actor_tag_entry
{
	byte flags;
	byte unknown01[3];
	real unknown04;
	real unknown08;
	real unknown0c;
	byte unknown10[0x18 - 0x10];
	real unknown18;
	real unknown1c;
	real unknown20;
	real unknown24;
	byte unknown28[0x34 - 0x28];
	real unknown34;
	byte unknown38[0x40 - 0x38];
};

/* the state of the slots of types 0x10 and 0x11 */
struct s_slot_10
{
	s_slot_header header;
	long time;
	short next_type;
	bool unknown12;
	byte unknown13[0x17 - 0x13];
	bool unknown17;
	bool unknown18;
	byte unknown19;
	short unknown1a;
	s_type_c3b527 unknown1c;
	byte unknown2c[0x40 - 0x2c];
};

struct s_prop_datum_view
{
	byte unknown00[0x27];
	char unknown27;
	real unknown28;
	byte unknown2c[0x3c - 0x2c];
};

struct s_prop_state_64
{
	byte unknown00[0x64];
	bool unknown64;
};

real function_259a0(dword *seed);
void *function_1e4f90(long actor_index);
real function_1c9ee0(real fraction);
bool __stdcall function_1aa750(long actor_index, s_slot *slot, bool flag);
short __stdcall function_1a9ec0(long actor_index);
bool __stdcall function_1aa0d0(long arg_0, s_slot *arg_1);
bool __stdcall function_1aab50(long arg_0, s_slot *arg_1);
void __stdcall function_1c04c0(long actor_index, s_slot *slot);

static inline bool actor_function_50c(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	return actor->unknown50c && actor->unknown504 == 1;
}

/* ---- slot type 0x42 ---- */

// @retail 0x1a9b50
short __stdcall function_1a9b50(long actor_index, s_slot *slot)
{
	function_1e4650(actor_index, true);
	return g_46fbe4;
}

/* ---- slot type 0x18 ---- */

// @retail 0x1a9bc0
short __stdcall function_1a9bc0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown07c != NONE)
	{
		long other_index = element_502420_get(actor->unknown07c)->first_actor_index;
		s_actor_view *other;
		do
		{
			if (other_index == NONE)
			{
				function_1e4650(actor_index, true);
				break;
			}
			other = actor_get(other_index);
			other_index = other->next_index;
		} while (other == actor || other->unknown004 != actor->unknown004);
	}
	return g_46fbe4;
}

/* ---- slot type 0x19 ---- */

// @retail 0x1a9c50
short __stdcall function_1a9c50(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown358 != 0 && actor->unknown35e)
	{
		function_1e4650(actor_index, true);
		if (!actor->unknown226)
		{
			dword *seed = &g_4e7408->unknown0;
			*seed = 1664525 * *seed + 1013904223;
			if (0.5f > (real)(*seed >> 16) * (1.f / 65535.f))
				actor->unknown226 = true;
		}
	}
	return g_46fbe4;
}

/* ---- slot types 0x10 and 0x11 ---- */

// @retail 0x1a9cf0
short __stdcall function_1a9cf0(long actor_index, s_slot *slot, bool active)
{
	short result = g_46fbe8;
	s_slot_10 *state = (s_slot_10 *)slot;
	short type = state->next_type;

	if (type >= 0 && type < k_slot_type_count)
	{
		s_slot_handler *handler = g_46eeb8[type];
		if (handler->unknown8 != g_46f348 &&
			(handler->mask & g_4ee4ec) == g_4ee4ec &&
			(g_557c40[type >> 5] & (1 << (type & 31))) != 0)
		{
			result = state->next_type;
			if (result != g_46fbe8)
				goto local_0;
		}
	}
	if (g_46eeb8[state->header.type]->unknown8 == g_46f348 || state->unknown12)
		result = g_46fbe4;
local_0:
	return result;
}

void function_1f86a0(long index);
byte function_1f8640(long index);
bool __stdcall function_1f4280(long actor_index);
void function_265c30(long prop_index, long actor_index, bool unknown);

/* stops the actor's prop approach once the prop is out of reach */
// @retail 0x1a9d80
void function_1a9d80(long actor_index, real distance, bool *approaching, bool *stopped)
{
	s_actor_view *actor = actor_get(actor_index);

	if (!actor->unknown007 && actor->unknown040)
	{
		*stopped = false;
		if (*approaching)
		{
			function_1f86a0(actor_index);
			if (!function_1f4810(actor_index, actor->prop_index, distance, 1))
			{
				*stopped = true;
				*approaching = false;
				function_1f4280(actor_index);
				actor_get(actor_index)->unknown040 = false;
			}
		}
	}
}

// @retail 0x1a9e00
void function_1a9e00(long actor_index, real distance, bool force)
{
	if (function_1e2030(actor_index))
	{
		s_actor_view *actor = actor_get(actor_index);

		if (!actor->unknown007 && actor->unknown040)
		{
			long prop_index = actor->prop_index;

			if (prop_index != NONE)
			{
				short *view = (short *)function_25d700(prop_index);

				if (view && *view >= 6)
				{
					bool close = false;

					if (prop_node_get(prop_index)->unknown28 > distance &&
						(force || !function_1f8640(actor_index) || actor->unknown524 > distance))
					{
						close = true;
					}
					function_265c30(prop_index, actor_index, close);
				}
			}
		}
	}
}
// @retail 0x1aa6e0
void __stdcall function_1aa6e0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	s_actor_view *local_0 = actor_get(actor_index);
	if (local_0->unknown50c)
	{
		if (local_0->unknown504 == 1)
		{
			actor->unknown3e0 = 0;
			actor->unknown44d = true;
		}
		else
			actor->unknown3e0 = 0;
	}
	else
		actor->unknown3e0 = 0;
}

// @retail 0x1aa990
bool __stdcall function_1aa990(long actor_index, s_slot *slot)
{
	bool result = false;

	if (!actor_get(actor_index)->unknown264)
	{
		result = function_1aa750(actor_index, slot, false);
		if (result)
		{
			s_actor_tag_entry *entry = (s_actor_tag_entry *)function_1e4f90(actor_index);
			if (entry && (entry->flags & 2))
				function_1e4650(actor_index, true);
		}
	}
	return result;
}

// @retail 0x1aa9f0
short __stdcall function_1aa9f0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_ai_object *unit = ai_object_get(actor->unknown018);
	s_actor_tag_entry *entry = (s_actor_tag_entry *)function_1e4f90(actor_index);
	short result = 0;

	if (!(unit->flags19 & 1) && entry && actor->prop_index != NONE &&
		entry->unknown24 >= g_45dbd8 && !actor->unknown229 &&
		(actor->times[4] == NONE || (real)(g_510c54->game_time - actor->times[4]) * g_510c54->rate > entry->unknown34))
	{
		long unit_index = actor->unknown018;
		if (!function_110ab0(unit_index))
		{
			s_prop_datum_view *prop = (s_prop_datum_view *)prop_node_get(actor->prop_index);
			s_prop_state_64 *state = (s_prop_state_64 *)function_25d690((s_prop_datum *)prop);
			if (prop->unknown27 >= 1)
			{
				if ((state->unknown64 || prop->unknown28 > entry->unknown1c) && entry->unknown20 > prop->unknown28)
				{
					if (state->unknown64 && !function_1e2030(actor_index))
						result = 4;
					else if (function_1c9ee0(entry->unknown24) > function_259a0(&g_4e7408->unknown0))
						result = 3;
				}
			}
		}
	}
	return result;
}

// @retail 0x1ab2c0
void __stdcall function_1ab2c0(long actor_index, s_slot *slot)
{
	s_slot_10 *state = (s_slot_10 *)slot;

	if (state->unknown17 && !state->unknown12 && !actor_get(actor_index)->unknown264)
		state->unknown1a++;
}

// @retail 0x1ab300
void __stdcall function_1ab300(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_10 *state = (s_slot_10 *)slot;

	function_1aa6e0(actor_index, slot);
	if (state->unknown18)
	{
		actor->unknown464 = true;
		actor->unknown465 = true;
		actor->unknown466 = true;
		((dword *)&actor->unknown468)[0] = ((dword *)&state->unknown1c)[0];
		((dword *)&actor->unknown468)[1] = ((dword *)&state->unknown1c)[1];
		((dword *)&actor->unknown468)[2] = ((dword *)&state->unknown1c)[2];
		((dword *)&actor->unknown468)[3] = ((dword *)&state->unknown1c)[3];
		state->unknown17 = true;
		state->unknown18 = false;
		state->unknown1a = 0;
	}
}

// @retail 0x1ab380
bool __stdcall function_1ab380(long actor_index, s_slot *slot)
{
	return function_1aa750(actor_index, slot, true);
}

// @retail 0x1ab3a0
void __stdcall function_1ab3a0(long actor_index, s_slot *slot)
{
	actor_get(actor_index)->times[4] = g_510c54->game_time;
}

/* ---- the handlers ---- */

s_slot_handler_0 g_47f7f0 =
{
	0x42, 0, 0, -2, 0, function_1a9b50
};

s_slot_handler_0 g_47da7c =
{
	0x18, 0, 0x7ff, -2, 0, function_1a9bc0
};

s_slot_handler_0 g_47da90 =
{
	0x19, 0, 0x7ff, -2, 0, function_1a9c50
};

s_slot_handler_2 g_47daa8 =
{
	{
		0x10, 2, 0, -2, 0,
		function_1a9ec0, function_1a9cf0, function_1aa990, function_1c04c0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1aa0d0, 0, function_1aa6e0
};

s_slot_handler_2 g_47daf8 =
{
	{
		0x11, 2, 0, -2, 0,
		function_1aa9f0, function_1a9cf0, function_1ab380, function_1ab3a0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1aab50, function_1ab2c0, function_1ab300
};

struct s_1aa0d0
{
	byte field_0[0xc];
	long field_c;
	short field_10;
	bool field_12;
	bool field_13;
	bool field_14;
	char field_15;
	bool field_16;
	bool field_17;
	bool field_18;
	byte field_19;
	short field_1a;
	point2f field_1c;
	real field_24;
	real field_28;
	bool field_2c;
	bool field_2d;
	bool field_2e;
	byte field_2f;
	short field_30;
	short field_32;
	real field_34;
	real field_38;
	byte field_3c[4];
};

struct s_1aa0d1
{
	byte field_0[0xc];
	real field_c;
	real field_10;
	real field_14;
};

struct s_1aa0d2
{
	byte field_0[4];
	point3f field_4;
	byte field_10[0x1c - 0x10];
	vector3f field_1c;
};

bool function_10f340(long arg_0, long arg_1, long arg_2);
bool unit_submit_entry_request(short fallback_index, short entry_index, long unit_index, long priority, void const *extra);
long function_1469f0(real arg_0);
real function_30bf0(vector3f *arg_0);
real normalize2d(point2f *arg_0);
extern point2f *g_468778;

// MiniMax H (11-13): no new form, no lower score (375): the frame agrees (0x40) but retail loads the actor's table
// entry before the settings call and keeps the slot in ebp where ours keeps the entry in ebx after the call, and the
// facing pointer is formed in esi from the view call here and late from eax in retail, which 0x25d740's inlining decides;
// taking the actor view before the settings call, as retail does, measures 413.
// MiniMax B (11-11): no new form, no lower score (375); retail loads the actor table before its register saves,
// keeps arg_0, the actor, the slot and the prop in edi, ebp, esi and ebx and computes the actor's table entry
// before the settings call, where ours keeps that entry in ebx after the call and the slot in ebp; the frame
// itself agrees (0x40), so only the carriers and the prediction homes differ.
// MiniMax G (11-12): no new form, no lower score (375): the frame agrees, but retail still loads the actor table before its register
// saves and holds the actor in ebp, the slot in esi and the prop in ebx, where ours computes the actor's table entry after the settings
// call and keeps the entry in ebx; moving the actor lookup ahead of the settings lookup still regresses (413), as the four entry
// declaration orders recorded below do.
// score 375 (was 375): Frame 0x40 agrees; early actor/settings/prop carriers and prediction homes remain.
// MiniMax H (11-04): no new form, no lower score; the four orders of the entry declarations (the actor first, the actor and its slot first, the actor second and the slot before the actor) give 413, 413, 375 and 413 against 375.
// b2G walk (r719): every block, field offset, constant and call argument checked against retail 0x1aa0d0-0x1aa6da; no logic or
// precision difference found (float sums in retail's k,j,i order, 0x24590 called, 0x2b400/0x30bf0 results as retail); the 20 matched callees and
// the file's matched siblings still match. What remains is allocation and scheduling only.
// Lane 6 r85: retail returns constants (`mov al, 1` / `xor al, al`, every push at entry, the early tests
// jumping to the false exit); `if (...) return true; ... return false;` without the result byte gives that
// 440 (frame 0x3c); with the actor looked up before the settings call 413 (retail's 0x40 frame and hoisted
// actor-table load; retail then keeps arg_0/actor/slot/prop in edi/ebp/esi/ebx and the settings pointer at
// [esp+0x10] only, ours keeps the settings pointer in esi and the prop in arg_1's home); prop pointer at
// function scope 417; slot pointer declared first 413; early `if (!settings) return false;` 424.
// Earlier at 375 (was 375): frame 0x40 agrees; early actor/settings/prop carriers and prediction scalar homes differ.
// frame 0x40 agrees; actor/settings/prop carriers and prediction homes remain.
// Earlier: frame 0x40 agrees; early actor/settings/prop carriers and prediction scalar homes differ.
// actor-table/result/prop carriers still differ; first
// actor-table/result/prop carriers still differ; first
// prediction loop 391; second prediction loop 391; planar reverse loop 391;
// repeated vector-field access 387; direct final result 379, plain verified.
// Round 290: result after lookups 411; read-only settings pointer 379; payload-only memset 378; native
// planar request workspace 391; byte result 399; actor first with direct final result 413.
// Frame 0x40 agrees; actor/settings/prop carriers differ. Guarded actor lookup
// Earlier at 379 (was 379): Frame 0x40 agrees; actor/settings/prop carriers differ. Guarded actor lookup
// 408; integer clock 413; conditional threshold 389; aggregate request 379; branch-derived
// prediction weight 385 (fast).
// Round 303: entry request 379; entry primary vector 379; sibling secondary vector 379;
// inner threshold 388; entry planar workspace 379. Actor/settings/prop carriers still differ.
// Round 310: eligibility result 432; double clock 413; prop handle snapshot 379;
// read-only prop payload 379; weight after magnitude 379. Early actor/settings/prop homes remain.
// actor/settings/prop carriers differ; request direction at use 379; request
// validity first 379; vector memcpy 379; normalization arm first 379; state interval 379.
// Frame 0x40 agrees; actor/settings/prop homes remain. Slot-first with direct
// Earlier at 379 (was 379): actor/settings/prop carriers differ; request direction at use 379; request
// Earlier at 379 (was 379): Frame 0x40 agrees; actor/settings/prop homes remain. Slot-first with direct
// result 379; unit field pointer 379; external state byte reference 390; position pointer 387;
// velocity pointer 424.
// Round 173: external actor / guarded slot / combined references 379/379/379;
// external facing reference 379; prediction-path velocity reference 395. Frame 0x40 agrees;
// early actor/settings/prop carriers remain. Original body retained.
// frame 0x40 agrees; early actor/settings/prop carriers remain.
// Failure guard with direct result 440; primary components i/j/k 379; earlier normalization
// distance 375 kept (plain); double distance 406; named facing projection 387.
// Early-distance context: distance/weight/time scalar names 375 each; combined scalar names 375; chained defaults 375.
// Distance at common direction scope 410; prediction-default order 375; complete weight result 388.
// Direct parent-to-direction handoff 375; direction after admission 378; named velocity alignment 375;
// branch-derived prop receiver 399; read-only actor 375; final rejection expression 375; nested admission 374; prop result with later direction 404.
// Actor lookup at use 524; velocity/position/both values 405/417/406;
// all projection products 375; products with live actor 516; clock at use 391; products with prediction values 406.
// Target velocity/position/both pointers at prediction 397/377/392; at receiver 434/383/425;
// position/both with separate second time 377/425.
// Earlier at 375 (was 379): frame 0x40 agrees; early actor/settings/prop carriers remain.
// Existing point-difference helper at primary/secondary/both predictions 375/375/375; native predicted
// point at primary/secondary/both 375/375/375; unary planar negation 390; earlier uninitialized
// prediction-time storage 375. Actor/settings/prop carriers remain.
// Native unit/state/rate values 393/405/380f; unit plus state/rate 406/395f;
// state plus rate 403f; all three 401f; actual distance value 403f.
// Early actor acquisition with the new unit/state/rate/all-three/distance values
// 395/424/406/410/410f. All native scalar captures and lookup changes restored.
// Later false default 409; rejected-arm false default 440; admission result 432;
// complete buffer names 375; direction names 375; earlier magnitude storage/name 375/375;
// later default with complete buffer names 409. Early actor/settings/prop homes remain.
// Native memcpy for the facing-test planar initializer 384; with request, actor
// fallback, both, constant fallback, request/constant, both fallbacks and all
// three request transfers 384 each. Original planar transfers restored.
// Round 177: Shared dot at facing/alignment/both/prediction/facing+prediction/alignment+prediction/
// all three 374/377/383/375/374/377/383; all three with opposite helper inputs 383.
// Exact-only grouping forms restored; actor/settings/prop carriers remain.
// Round 341: Successive vector-length storage before prediction, target velocity receiver and actor-first graphs 379/413/394/435; after prediction 379/413/395/437. Original body restored.
// Round 235: Scaled-displacement producers (O original scalar, H shared scale
// output, V native velocity value scaled in place), primary/secondary:
// O/H 389; O/V 420; H/O 389; H/H 403; H/V 448; V/O 406; V/H 406; V/V 376.
// Actor/settings/prop homes remain; original producers, include set and body restored.
// Round 140: One immutable request-distance observation with secondary threshold/shared prediction/native
// result interactions 406/395/440/431/430/421/455/445; original and actor-first lookup under coherent
// inline/noinline/both settings declarations 375/413/375/413/375/413. Both declarations lose matched 0x265d30
// in the calibrated peer image; all observations, vectors, result and declarations restored.
// Round 177: Shared dot at facing/alignment/both/prediction/facing+prediction/alignment+prediction/
// all three 374/377/383/375/374/377/383; all three with opposite helper inputs 383.
// Exact-only grouping forms restored; actor/settings/prop carriers remain.
// Round 158: Complete facing/alignment/prediction product operands commuted independently, in pairs, all
// three, then with both full prediction producers. Scores [375, 375, 375, 375, 375, 375, 375, 375]. Original
// executable body restored.
// Round 184: native admission/active/request byte fields, all seven nonempty
// groups, and the unsigned bounded request discriminator 375 each. Original
// Boolean fields, output-pointer contracts and request discriminator restored.
// Round 226: complete settings native/memcpy values with bool/byte result, original/actor-first
// acquisition 441, 441, 413, 413, 422, 422, 394, 394. All restored; frame and early carrier homes remain.
// Round 217: Prop-state inline/noinline, prop-view inline/noinline, both inline/noinline
// boundaries 375/375/375/375/375/375; state/both inline with actor-first acquisition 413/413
// fast. Original declarations, acquisitions, pointer homes and complete body restored.
// Round 205: Complete settings/state/direction getter receiver masks 1/2/3/4/5/6/7
// 433/487/516/506/574/663/657; all with constant return exits 660. Every original
// receiver, early distance, request transfer, expression tree and body restored.
// Round 399: Native slot callback input, original/actor-first lookup, with entry/late result defaults,
// constant-return exits and early rejection: 375/413/409/426/425/393/440/413. Body and declaration
// restored.
// Round 38: Common result set by independent final failure tests, all six field orders 375
// each fast; nested success assignment 380; failure-result ladder 373, all 1549 bytes.
// Two-point best rejected; original result expression, defaults and complete body restored.
// Round 428: native short/word/long/dword completion result 404/404/406/406;
// those result widths with actor-first acquisition 401/401/403/403. All 1549 bytes;
// result spill and actor/settings/prop allocation remain; original bool result and lookup order restored.
// Round 238: complementary rejection result bool/byte/short/long 394/394/403/401;
// those domains with actor-first acquisition 419/419/406/404. Original positive
// bool result, lookup order, early distance and complete body restored.
// Round 37 (lane 1, fast): no facing pointer, the facing read through the prop view 401; the settings
// threshold field in a named local 406; the actor before the settings call 413; that with
// `if (!settings || f13 || f14 || f2d) return false;` and a direct final return 393 (1549 bytes, frame
// 0x40 and all four pushes at entry as retail; ours keeps arg_0/actor/settings/slot in EBX/EDI/ESI/EBP,
// retail in EDI/EBP/memory/ESI with the prop in EBX); the slot pointer declared first there 393.
// Round 249: Repeated actor/request truth snapshots, singly/both 400/401/394;
// actor-first counterparts 402/425/415; both with late false default, original/actor-first
// acquisition 426/429. All three predicate calls, early distance and complete original body restored.
// Round 471: Complete persistent actor/settings/slot/prop lexical priorities in eight
// role-correct identity sets 375 each. Original pointer names, observations and body restored.
// Round 145: ignoring register names, ours differs from retail in about 100 instructions (entry order,
// result byte, prop home, facing reads, constant exits). Actor first with `if (...) {...; if (!f14 && !f13
// && !f2d) return true;} return false;` 413 (1523 bytes); actor first with the result byte 413 (1555);
// constant exits alone 440 (1517); actor first with `return !f14 && !f13 && !f2d;` in the arm 393 (1549:
// settings spilled at [esp+0x10] as retail, but arg_0 kept in EBX and the actor in EDI); that with the
// first facing dot read off the prop view 408 (1556).
// Round 55 (lane 10, fast): on the actor-first form with constant returns (393 re-measured): the slot pointer
// declared first 393; the settings rejection as its own early return 410; the prop pointer declared with the
// lookups at function scope 395; the prop pointer cleared in the prop-less else arm in place of its NULL
// initializer 385 (the settings pointer then lives only at [esp+0xc] as retail's [esp+0x10], but arg_0 stays in
// EBX and the prop goes to ESI; retail's `xor ebx, ebx` before the NONE test shows the NULL initializer is
// retail's); the prop index in a named local 393.
// Round 289: Complete facing value with actor-first/target position/target velocity values, masks
// 0..7, 473/467/514/520/505/524/522/556. All restored; original facing receiver,
// observations, lookup order, early distance and complete body retained.
// Round 456: Facing fallback/secondary direction/planar-test memmove transfers, subsets 1..7
// 378/381/410/381/386/387/420; all plus three request planar transfers 414.
// Original copies, early distance, lookup order, arithmetic trees and full body restored; persistent actor/settings/prop homes remain.
// Round 72 (lane 16, fast; aligned register-blind distance in brackets, control [109]): actor first 413 [136]; a late
// `&field_2d` pointer with constant exits 413 [109], with a result 413 [136], one expression exit 393 [116]; the facing
// pointer declared inside the second test 416, on the original order 378; a named velocity pointer 398 [112]; both 398
// [106] with retail's `xor ebx, ebx` prop and frame 0x44; dot3f in the three facing tests 407/398/398, all 407; early
// rejection 398; slot typed parameter 398 (same code); struct copy of the 2D facing 406. Ours keeps arg_0/actor/slot in
// ESI/EDI/EBP, retail in EDI/EBP/ESI, and that rotation dominates the positional score. Restored.
// Round 304: primary/secondary displacement production in all six uniform coordinate orders,
// then primary-only/secondary-only kji: 375 each. Original stores and complete body restored.
// Round 479: Shared add-scaled then point-difference prediction pipelines, original/native-point/in-place
// primary and secondary pairs: 376/375/375/377/376/375/376/377. Original producers and include set restored;
// early actor/settings/prop carriers and prediction homes remain.
// AC489: Absent-prop, unsubmitted-request and inactive final-processing phase ownership,
// seven subsets 388/397/375/410/388/397/410; complete graph with separate secondary admission
// guards 410. Original body restored; frame agrees and early actor/settings/prop carriers remain.
// m3 lane 11 r63 (fast), on the actor-first form with `return !f14 && !f13 && !f2d;` in the arm (393
// re-measured; retail passes arg_0 to the settings call in EBX from EDI, keeps the actor in EBP and spills
// the settings pointer, ours spills the actor): settings test nested apart from the flag tests 393; prop-less
// arm first 401; slot pointer before the settings call 393; prop lookup before the clock 393; a four-use
// `&field_2d` pointer in the tail 393. All restored to the 375 body.
// Round 139 (m3 lane 5, fast): actor first with constant exits 393; with a two-use settings distance local at the
// final calls 403 (1554 bytes); with the default threshold before the clock 393; that threshold order alone 375 (same
// code); the two-use distance local alone 425 (1567 bytes).
// Round 47 (lane 15, fast): actor first with `return` of the expression 393, constant returns 413 (1523 bytes); settings rejected
// early 410; all rejections early 393; prop declared at entry 395; slot read inside the settings arm 393. Retail keeps arg_0 in EDI,
// actor EBP, slot ESI, prop EBX with settings only at +0x10; every actor-first form keeps arg_0 in EBX.
// AC505: Branch-owned near/predicted request-kind and active-state stores, masks
// 1/2/4/3/5/6/7, 378/374/375/375/378/374/375; all with bounded request admission 375.
// NOT kept: best gain 1; original stores and complete body restored. Early carriers remain.
// Round 139 (m3 lane 5, fast): actor first with constant exits 393; with a two-use settings distance local at the
// final calls 403 (1554 bytes); with the default threshold before the clock 393; that threshold order alone 375 (same
// code); the two-use distance local alone 425 (1567 bytes).
// AC507: Active/inactive unit-admission call results in bool/byte/short/long storage,
// 375/375/375/375 and 375/375/375/375. NOT kept: one-use results do not match;
// original calls, observations and complete body restored. Early carriers remain.
// AC509: Admission rejection to the sole result return, compound/negative-result/positive-result/
// settings-owned gates: 374/407/407/374; actor-first versions 413/435/435/413. NOT kept:
// best gain 1; original acquisition, result, admission and complete body restored. Early carriers remain.
// AC511: Actual header-free slot-member record with entry/branch false result,
// direct completion and early admission, original/actor-first acquisition:
// 387/440/425/440/410/413/393/413. NOT kept: native receiver/return lifetime graphs
// regress; original record, lookups, result and complete body restored.
// AC513: Existing native actor-pool getter at original/readonly/actor-first producers
// 375/375/413; actor-first direct completion 393; metadata-owned admission and
// single failure-result join 374/374; direct request direction 375; every-use facing
// 401. NOT kept: best gain 1 and no exact match; original getter, include set and body restored.
// AC516: Actual request-submission call result, bool/byte/short/long at its producer
// or with the request-owned uninitialized declaration: 375/375/375/375/375/375/375/375.
// NOT kept: one-use call-result storage unmatched; original call, state stores and complete body restored.
// AC519: Planar normalization-return residence, facing/request/fallback subsets
// 1..7 and one shared planar-length carrier: 375 each. NOT kept: no byte match;
// original normalization calls, early distance, result and complete body restored.
// AC523: Partial-metadata returned result, bool/byte/short/long with
// original/actor-first acquisition pairs: 440/413/429/403/401/412/403/414.
// NOT kept: results regress and shorten away from retail; original admission,
// lookups, early distance, result, request and complete body restored.
// AC527: Complete distance-or-deadline stop result, positive/complementary bool/byte/short/long,
// 383/383/383/383/383/383/383/383. NOT kept: result residence unmatched; original short circuit,
// state-publication arms, clock/rounding observations and complete body restored; early carriers remain.
// M384: Complete prediction/heading ready/blocked request results, bool/byte/short/long:
// 375/375/375/375/375/375/375/375. NOT kept: no byte equality for the flag lifetime;
// original request admission and full body restored; early actor/settings/prop homes remain.
// m3 lane 1 r71 (--fast; masked = aligned count with registers and esp offsets masked, base 163): LEAD, not kept because the score rises: the actor looked up first with
// constant returns 413/1523 (masked 86); plus the velocity field as a named pointer used eight times 398/1549 (75); plus the direction pointer declared inside its test
// 398 (65); plus the second distance's zero assigned in its arm 406 (65); plus the planar pair copied whole 414 (59). Settings then sit in memory and the prop in EBX as
// retail; what is left: arg_0 ESI (retail EDI), actor EDI spilled (retail EBP, the extra slot makes frame 0x44 against 0x40), slot EBP (retail ESI), the first dot's term
// order, `comiss` against the memory zero after the normalize, and the request's word/dword store order. On that base the slot as the parameter itself 422/1532 (60), the
// prop at function scope 397/1560 (73). Same code, no change: slot declared first or middle, all six dot term orders, dot operands swapped, renamed locals (three sets).
// One-link harness not faithful here (control 442 at 1580 bytes).
// AC560: Proven metadata/returned state/view record references, seven subsets, then facing
// reference: 375/375/375/375/375/375/375/375. NOT kept: no byte equality; early actor/settings/prop homes remain.
// Original receivers, early distance, result, observations and full body restored.
// Round 90 (m3 lane 16, fast, on the 375 form): the prop pointer uninitialised and cleared in the prop-less arm 399
// (1558 bytes); the slot pointer declared first 375 (same code); the prop lookup before the clock 380; a velocity
// pointer for the magnitude call and the eight prediction reads 397 (1576 bytes); the request direction stored
// through its pointer 375 (same code). Aligned by sequence, 263 rows differ.
// Round 179 (m3 lane 4, fast, the file's 15 matched functions and 0x265d30 matched each time): actor first with
// `return !f14 && !f13 && !f2d;` in the arm re-measured 393 (arg_0 EBX, actor EDI, settings ESI, slot EBP, prop in
// arg_1's home); on the 375 body: the distance left uninitialised and zeroed in the copy arm 384 (1551 bytes); a
// separate local for the second prediction time 375 (same code); the prop index in a two-use local 375 (same code);
// the facing pointer taken straight from the 0x25d740 call, no view local, 375; the settings test as its own `if`
// around the flag tests 374 (one count, not kept); the facing-test planar pair copied whole 384. Retail's two
// `local_9 = *local_8` copies stay separate (the copy arm jumps past a local_6 reload); ours cross-jumps them.
// Round 106 (lane 9, fast; re-measures of lane 1 r71's chain on the current base): actor first with constant returns
// 393 (1549 bytes, masked 134), plus the velocity field through a named pointer 417 (1563), plus the direction
// pointer inside its test 412 (1560), plus the second distance's zero in its arm 420 (1560), plus the planar pair
// copied whole 412 (1552). Retail's homes: arg_0 EDI, actor EBP, slot ESI, prop EBX, settings in memory.
// Diagnostic: 0x110ab0 as an opaque stub leaves 375 (masked 134), so that unmatched callee is not the cause.
// Lane 12 r107 (fast; masked aligned count with registers and esp offsets masked in brackets, 124 at 375): lane 1 r71's lead
// rebuilt (actor first, constant exits, a velocity pointer for every field_1c read, the direction pointer declared inside the
// 0x110ab0 test, the second distance's zero in its arm, the planar pair copied whole) 422 [90] (1532 B, frame 0x44: ours arg_0
// ESI, actor EDI spilled around 0xe6900's EDI argument, slot EBP; retail arg_0 EDI, actor EBP, slot ESI). On it: a separate
// local for the second prediction time 422 (same code); the first prediction in i, j, k order 418 [91] (1535 B); without the
// velocity pointer 420 [111] (1525 B); `return !f14 && !f13 && !f2d;` in the arm 411 [99] (1549 B); that with the distance zero
// at its declaration 403 [102] (1549 B), and with the planar pair as `{i, j}` 407 [106] (1555 B).
// Round 169 (m3 lane 6, fast with the file's 15 matched functions and 0x1aab50): a forceinline facing-request helper shared
// with 0x1aab50 (memset, mode, type, planar direction, both normalize2d fallbacks, 0x0e6900 and the entry request) 375 (same
// code); a forceinline predicted-offset helper (target velocity times scale plus its point minus the origin, i, j, k: retail
// gives i, j, k xmm1/xmm2/xmm3 in both functions) for both predictions here and 0x1aab50's 375 (same code; 0x1aab50 444);
// a forceinline planar facing test shared with 0x1aab50 (cosine as a parameter) 384; the final result set by
// `if (...) result = true;` 380; the first facing dot read through the prop view (retail's `[edi + 0x34]`, then `add edi,
// 0x2c`) with the facing pointer kept for the rest 394 (1555 bytes).
// M3 lane 1 r90 (fast with the file's functions and 0x265d30; aligned/register-masked counts in brackets, control 375
// [263/104]): base B = the actor looked up first, `if (!local_1 || f13 || f14 || f2d) return false;` and `return !f14 &&
// !f13 && !f2d;` in the arm, re-measured 393 [266/104]; B with the -0.4 facing dot read off the view (round 145's form,
// re-measured 408 [179/99], 1556 B) = B2. New on B2: a pointer for the prop velocity's eight reads 451 [322/83] (frame
// 0x44, 1588 B); the planar pair copied whole 402 [281/89] (1536 B); the second distance left uninitialised and zeroed in
// its copy arm 395 [175/97] (1558 B, line count equal to retail's) = B3; on B3 the planar pair copied whole 399 [288/98]
// (1549 B), and a late pointer to field_2d for the tail 395 (same code). Retail keeps the prop in EBX, then the velocity
// pointer (`add ebx, 0x1c`) for magnitude3d, and the weight in memory across that call (our link keeps it in XMM4: the
// same magnitude3d register-summary lead as 0x1d24a0's). All restored.
// M3 lane 14 r80 (fast; masked = aligned count with registers masked, 106 at 375): retail keeps the view pointer in EDI across the
// second 0x110ab0 call and turns it into the facing pointer in place (`add edi, 0x2c`) in the middle of the first dot, reading
// facing.k/j off the view (+0x34/+0x30); ours forms the facing pointer in ESI straight after the view call, and so does every
// facing form tried. Retail likewise turns local_6 into its field_1c pointer in place (`add R, 0x1c`) for the magnitude call.
// New: the facing read through the view at each use plus a velocity pointer for the magnitude and all nine field_1c reads 450
// (1601 bytes, masked 163); that with the actor first 457 (1593, 264); the facing through the view with the actor first 389 (1556,
// 129); that with the result returned in the arm (`return !f14 && !f13 && !f2d;`, `return false;` last) 386 (1549, 154); the facing
// through the view with that return on the original order 408 (1549, 153). Re-measured: actor first 413 (1555, masked 134); the
// velocity pointer alone 397 (1576, masked 136).
// M3 lane 2 r73 (plain with the file's 17 functions and 0x265d30; aligned 263, masked 102 at 375): the 0.866 planar test
// through vector2f_products.h's dot_product2d 375 (masked 103: the -0.4 dot's j product takes the actor field first).
// Census of the other natural forms, all listed above with scores: dot3f at the three facing dots 407/398/398 (all 407),
// the one-use clock read at its use 391, the facing pointer straight from the 0x25d740 call 375, the actor first 413,
// constant exits 440, the prediction pipelines through the point helpers 375-377, a separate second prediction time 375,
// the slot pointer declared first 375, the prop lookup before the clock 380.
// M3 lane 1 r99 (fast with the file's 17 functions and 0x265d30, all MATCH throughout; base 375). Reading of the listing: retail
// gives EBX to the prop, so arg_0 takes EDI and is copied to EBX for 0x1e4f90, and the settings pointer is the one left in memory;
// ours loads arg_0 straight into EBX (the register 0x1e4f90 takes it in), keeps the settings in ESI and leaves the prop in arg_1's
// home, so the question is why the prop outranks the settings in retail (both have six reads). The prop pointer declared first at
// function scope and cleared where it was 375 (same code); that with the actor looked up first 413 (same code as actor first
// alone, re-measured 413); retail's entry read literally (actor, settings tested at once with `return false`, then the slot and
// its three flags, the result returned as the expression) 410 (1549 bytes); that with the prop declared first 410, with the prop
// declared and cleared at the very top 409; the slot and the actor ahead of the settings call 413; the settings distance read
// once into a local for its three later reads 406 (1533 bytes), with retail's entry 421 (1529 bytes). Declaration order and
// position do not decide the tie. Body unchanged at 375.
// T105: Progressive final-result logical/bitwise refinement, bool/byte/short/long:
// 375/395/404/406/375/373/406/407. NOT kept: two-point best is below the gate
// and no native result/carrier equality; original result and complete body restored.
// @retail 0x1aa0d0
bool __stdcall function_1aa0d0(long arg_0, s_slot *arg_1)
{
	bool result = false;
	s_1aa0d1 *local_1 = (s_1aa0d1 *)function_1e4f90(arg_0);
	s_1aa0d0 *local_2 = (s_1aa0d0 *)arg_1;
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_1 && !local_2->field_13 && !local_2->field_14 && !local_2->field_2d)
	{
		s_prop_node_view *local_3 = NULL;
		if (local_0->prop_index != NONE)
		{
			real local_4 = (real)g_510c54->game_time;
			real local_5 = 3.402823466e+38f;
			local_3 = prop_node_get(local_0->prop_index);
			if (!local_2->field_12)
			{
				if (!local_0->unknown225 && !function_1e2030(arg_0))
					local_5 = local_1->field_10;
				if (!local_0->unknown225 && function_1e2030(arg_0))
				{
					if ((local_3->unknown28 > local_5 && function_1e2030(arg_0)) ||
						local_4 >= (real)(local_2->field_c + function_1469f0(local_1->field_14)))
						local_2->field_14 = true;
					else
						local_2->field_2c = true;
				}
				else
				{
					local_2->field_14 = false;
					local_2->field_2c = true;
				}
			}
		}
		else
		{
			local_2->field_2c = false;
			local_2->field_14 = true;
		}
		if (local_2->field_12)
			local_2->field_13 = !function_110ab0(local_0->unknown018);
		else if (local_3)
		{
			s_1aa0d2 *local_6 = (s_1aa0d2 *)function_25d690((s_prop_datum *)local_3);
			s_prop_view_fields *local_7 = (s_prop_view_fields *)function_25d740((s_prop_node *)local_3);
			vector3f const *local_8 = &local_7->unknown2c;
			if (!function_110ab0(local_0->unknown018))
			{
				vector3f local_9;
				if (-0.4f > local_8->k * local_0->unknown290.k + local_8->j * local_0->unknown290.j + local_8->i * local_0->unknown290.i &&
					local_1->field_c > local_3->unknown28 && !local_0->unknown264 &&
					function_10f340(local_0->unknown018, 0x7000101, 0xa0005b9) && !local_2->field_16)
				{
					local_2->field_15 = 1;
					local_9.i = -1.0f * local_8->i;
					local_9.j = -1.0f * local_8->j;
				}
				else if (0.8f > local_3->unknown28)
				{
					local_9 = *local_8;
					local_2->field_15 = local_2->field_16 ? 2 : 0;
				}
				else
				{
					real local_13 = 0.0f;
					real local_10 = 0.0f;
					real local_11 = magnitude3d(&local_6->field_1c);
					if (local_11 > 0.0f)
						local_10 = (1.0f + (local_8->k * local_6->field_1c.k + local_8->j * local_6->field_1c.j + local_8->i * local_6->field_1c.i) / local_11) * 0.5f;
					real local_12 = (real)local_2->field_30 * g_510c54->rate * local_10;
					local_9.j = local_6->field_1c.j * local_12 + local_6->field_4.y - local_0->position.y;
					local_9.k = local_6->field_1c.k * local_12 + local_6->field_4.z - local_0->position.z;
					local_9.i = local_6->field_1c.i * local_12 + local_6->field_4.x - local_0->position.x;
					if (0.0f > local_8->k * local_9.k + local_8->j * local_9.j + local_8->i * local_9.i)
						local_9 = *local_8;
					else
					{
						local_13 = function_30bf0(&local_9);
						if (local_13 == 0.0f)
							local_9 = *local_8;
					}
					if (local_2->field_34 > local_13)
						local_2->field_15 = local_2->field_16 ? 2 : 0;
					else if (local_2->field_38 > 0.0f && !local_2->field_16)
					{
						vector3f local_14;
						local_12 = (real)local_2->field_32 * g_510c54->rate * local_10;
						local_14.i = local_6->field_1c.i * local_12 + local_6->field_4.x - local_0->position.x;
						local_14.j = local_6->field_1c.j * local_12 + local_6->field_4.y - local_0->position.y;
						local_14.k = local_6->field_1c.k * local_12 + local_6->field_4.z - local_0->position.z;
						if (!(local_2->field_38 > function_30bf0(&local_14)))
							goto local_15;
						local_2->field_15 = 3;
						local_9 = local_14;
					}
					else
						goto local_15;
				}
				if (!local_2->field_2e && (local_2->field_15 == 0 || local_2->field_15 == 2 || local_2->field_15 == 3))
				{
					point2f local_16 = {local_9.i, local_9.j};
					if (normalize2d(&local_16) > 0.0f && 0.866025388f > local_0->unknown290.j * local_16.y + local_0->unknown290.i * local_16.x)
						goto local_15;
				}
				s_unit_request local_17;
				memset(&local_17, 0, sizeof(local_17));
				local_17.type = 0x1a;
				local_17.type1a.unknown4 = local_2->field_15;
				*(point2f *)(local_17.arguments + 4) = *(point2f *)&local_9;
				local_17.type1a.unknown6 = true;
				point2f *local_18 = (point2f *)(local_17.arguments + 4);
				if (normalize2d(local_18) == 0.0f)
				{
					*local_18 = *(point2f *)&local_0->unknown290;
					if (normalize2d(local_18) == 0.0f)
						*local_18 = *g_468778;
				}
				if (function_e6900(local_0->unknown018, &local_17))
				{
					unit_submit_entry_request(0x12, NONE, local_0->unknown018, 0xd, NULL);
					local_2->field_12 = true;
				}
			}
		}
	local_15:
		if (local_2->field_12)
		{
			local_2->field_2d = false;
			function_1f4280(arg_0);
		}
		else
			function_1a9d80(arg_0, local_1->field_c, &local_2->field_2c, &local_2->field_2d);
		if (!local_2->field_12)
			function_1a9e00(arg_0, local_1->field_c, local_2->field_2d);
		result = !local_2->field_14 && !local_2->field_13 && !local_2->field_2d;
	}
	return result;
}

bool function_25d9b0(long arg_0);
bool function_fa1a0(real arg_0, real arg_1, point3f const *arg_2, point3f const *arg_3,
	real *arg_4, real const *arg_5, real const *arg_6, bool arg_7, vector3f *arg_8,
	real *arg_9, real *arg_10, real *arg_11, real *arg_12, real *arg_13);

struct s_1aab50
{
	byte field_0[0x10];
	real field_10;
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
	real field_28;
	real field_2c;
};

static __forceinline long real_to_long(real value);

// MiniMax H (11-13): no new form, no lower score (382): retail's early false exit (xor al, al; pop ebp; add esp,
// 0x58; ret 8) sits in the middle of the body where ours lands after the main exit, and in the velocity block retail
// keeps the view pointer in eax and forms the facing pointer late where ours forms it in esi from the view call; the
// single-exit bool spelled as a direct return true and return false measures 508.
// MiniMax B (11-12): no new form, no lower score (382); the normalised instruction streams differ in two ways: the
// early false exit (xor al, al; pop ebp; add esp, 0x58; ret 8) sits at retail's 0x1aac35, in the middle of the
// body, while here it lands after the main exit at the end of the function, so every later branch target is
// displaced, and the velocity block colours its registers differently (retail keeps the view pointer in EAX and
// the zero vector in XMM4, ours in ESI and XMM3). Writing the rejection as an early return (the entry tests
// returning false at once, the body outside them, or the three tests as separate else branches) gives 508 or 527
// at a 0x54 frame, since the result byte is then folded away and the frame loses four bytes, so the shared exit
// with the result byte in its stack slot stays as it is. The permuter (run with the 0x25d740 and 0x25d690
// contexts, since retail inlines those two callees) finds no rewrite that matches retail, so the body stays.
// MiniMax E (11-11): no new form, no lower score (382): the frame and the length agree and the early false exit's place is right, but the object/magnitude and query scalar homes still differ, and the helper retail keeps out of line is expanded here (0x25d740, in another file): BLOCKED-BY 0x25d740.
// MiniMax S (11-11): no new form, no lower score (382): the frame and the length agree, but the early false exit's place, the object/magnitude homes and the query scalar homes still differ.
// score 382 (was 382): the frame and the length agree, and the early false exit's place, the
// object/magnitude homes and the query scalar homes still differ (one earlier round found no new form).
// b2G walk (r719): every block, field offset, constant and call argument (including 0xfa1a0's 3 register + 11 stack arguments, whose
// first output is the reused speed local) checked against retail 0x1aab50-0x1ab2bd; no logic or precision difference found. The written-out
// object-speed root now uses length_sq3f (same code, 382); magnitude3d would be a call (481, 1887 bytes) where retail inlines the x87 body;
// dot3f sums i,j,k where retail sums k,j,i, so it is not used. What remains is allocation and scheduling only.
// Round 53 (lane 11, fast): value fix kept: the trajectory call's origin is the actor's position and its
// target the prop's position (retail passes the prop's point in EAX and the actor's in ECX, the registers the
// matched 0x2900b0 uses for target and origin) 384 -> 382. Tried: the entry flags through a forceinline
// predicate 519 (exit still last, frame 0x54, 0x25d740 inlined); the flag preset true and cleared in an `if`
// then tested 527; local_1/local_3/local_4 declared at the top 384 (same code); the computed flag declared
// before local_0 517; on the value fix: an object pointer for the velocity reads 411 (1919 bytes), the
// computed entry flag 517. Every form whose false exit is threaded (computed flag, false default) also moves
// arg_0 into EDI and drops the frame to 0x54.
// Earlier (384): Frame 0x58 and length agree; early rejection, object/magnitude and query scalar homes remain.
// Frame 0x58 and length agree; early rejection, object/magnitude and query scalar homes remain.
// Lane 6 r85: retail lays the early false exit (`xor al, al; pop ebp; add esp, 0x58; ret 8`) right after the
// `goto local_7` jump; ours puts it last. `bool local_2 = !f13 && !f14 && !f2d; if (local_2) {...} return
// local_2;` (and the same with an if/else setting the flag) lands it where retail does, but arg_0 then takes
// edi from the settings pointer and the frame drops to 0x54: 517. Inverted test with the false arm first 386
// (identical code); early `return false` then `bool local_2 = true;` 511; named admission bool 517.
// Earlier: frame 0x58 and length agree; early rejection and query scalar/object homes remain.
// Frame 0x58 and 1904-byte length agree; early rejection and query scalar homes remain.
// frame 0x58 and length agree; early rejection and query scalar/object homes remain.
// Earlier: Frame 0x58 and 1904-byte length agree; early rejection and query scalar homes remain.
// Earlier forms: prediction loop 482; initial velocity snapshot 538; direct
// early false exit, frame 0x54 versus 0x58 and scratch
// homes differ; prediction loop 482; initial velocity snapshot 538; direct
// eligibility 484; full-width readiness 495; fallback copy loops 541.
// j92: actor/settings registers agree; frame 0x54 versus 0x58, early false exit and speed/object homes remain; retain facing view and native planar copy.
// shared velocity length 486; separate minimum speed 488; retained facing vector 472; planar direction copy
// 476; direction length by arm 482; facing position view 471; facing prediction seconds 472; facing planar
// copy 466; facing direction length 473; planar speed homes 466; planar position view 464; planar position
// speed homes 464; planar position late 466.
// Previous form: Frame 0x54 versus 0x58, early false exit and speed/object/request homes remain.
// late object receiver 470; trajectory planar view 466; direct request view 466;
// test-derived readiness 410; conditional approach factor 466 (fast).
// Previous form: frame 0x54 versus 0x58, early rejection and query homes remain.
// Separate query in/out scalar 494; wider output scope 410; wider trajectory scope 410;
// scalar/output and scalar/trajectory scopes 494 each; output/trajectory scopes 410;
// all query homes 494; all homes with earlier query-scalar declaration 494.
// Target velocity view 487; object velocity view 386; both 384 (two-point increment rejected);
// byte readiness 410; target/object/both views with byte 487/386/384; native readiness 422.
// Direct six-use object velocity field producer 386 kept (plain), removing the one-use object pointer.
// Actor/target/both position receivers 443/451/461; both at receiver 441; actor at entry 463;
// positions with direct rejection 493, named query scalars 461, named query direction 461.
// Frame 0x54 versus 0x58, early false exit and speed/object/request homes remain.
// frame 0x54 versus 0x58, early rejection and query homes remain.
// query output names 410; native request clear 474; positive mode arms 483; far prediction first 413; request
// mode first 522; relative velocity expression 413; direct facing field 524; deadline mode first 510.
// Complete slot/target/both layouts 386/386/386; paired approach projections 386;
// slot/target/both layouts with projections 386/386/386; both layouts with early rejection 511.
// On the direct velocity producer: false-default result 497; completion-time success 511;
// byte/word results 431/447; early rejection 511; single-exit admission 497;
// completion-time byte/word results 505/510.
// Native distance/rate/both observations 470/495/487; complete actor-facing
// value alone/with distance/with rate/with both 516/575/565/586; complete
// observations plus paired query names 586. Opening 0x58 frame/body restored;
// early rejection and magnitude/object/query homes remain.
// Object velocity before magnitude / after weight / after normalization 384/445/465;
// initialized distinct query speed / flight output / both outputs 453/452/478;
// distinct speed or flight output with earlier object receiver 455/455. Two-count gain rejected.
// Native memcpy: full facing fallbacks 386; actor/constant query fallbacks
// 386/386; facing-test planar initializer 386; request planar publication 386;
// request fallbacks 386; queried planar publication 386; all copies 386.
// Native predicted point/shared difference 451; x/y/z order and aggregate point 447/447;
// point plus shared squared length 447; query output scope, scalar names, declaration
// order and their combination on the point graph 451/451/451/451. Original body restored.
// Entry/eligibility/final-approach/deadline Boolean reductions 506/493/407/440;
// all mode reductions 510; entry plus modes 477; early test-derived eligibility
// on the native velocity receiver 393; entry reduction plus that result 505.
// Round 177: Shared dot at alignment/backward prediction/both/approach/alignment+approach/
// backward+approach/all three, then all with opposite helper inputs: 386 each. Exact-only
// grouping forms restored; early rejection and magnitude/query homes remain.
// Existing squared-length helper 384; uniform projection products 480; both 460;
// all prediction products 386; helper/prediction 384; projection/prediction 480;
// all producers 460; all with target velocity receiver 456. Original restored.
// Round 341: One vector-length scalar before prediction, target receiver and direct-facing graphs 384/383/517/520; after prediction 386/384/513/514. Two/three-count gains dropped; original body restored.
// Round 354: complete shared relative-velocity difference 391; every-use object producer 509;
// both 484; native request initialization on the recovered frame 472; with shared difference 477;
// with every-use object producer 506; all three 430; all with wide eligibility 493. Original restored.
// Round 193: Existing metadata providers ordinary-inline singly, all pairs and all,
// then separate noinline declarations on all three: 386/386/386/386/386/386/386/386. Original restored.
// Round 227: Prediction/facing/query fallback vector setters, all subsets, then early distance default 382/466/467/484/485/491/492/494.
// Original body and all helpers restored.
// Round 392: Entry result from each individual flag and each flag pair, then the first pair in byte/long
// storage: 476/476/476/393/395/396/438/454. Complete original entry/result and body restored.
// Round 399: Magnitude/weight phase scope, readonly magnitude, magnitude/distance names and all
// combinations, then coefficient/time names: 384 each. Original restored.
// Round 206: Combined metadata/prop guard, query rejection else-if, typed request direction and all
// subsets [384, 384, 384, 384, 384, 384, 384]; all with returned admission predicate 517. Original restored.
// Round 220: Early distance-limit scope/exhaustive/conditional/complementary producers 398/402/402/402;
// the same four with the actual native slot formal and no cast alias 398/402/402/402. All retain
// retail length but regress; complete original body, formal, sole declaration and every query restored.
// Round 42: complete target/object velocity scalar values retained at all uses
// 513/502; complete prop-facing value 489; target/object, target/facing and
// object/facing value pairs 555/583/544; all scalar values 603; all native vector
// values 574. Original pointers, fresh reads, arithmetic, query aliases and body
// restored; frame 0x58, early rejection and magnitude/query homes remain.
// Lane 18 round 6: the first 66 instructions agree; retail's false exit after the `goto local_7` jump shifts
// positions by 3-5 to about index 430. Retail keeps the actor in ESI into the velocity block, forms the facing
// address late (`add eax, 0x2c`, later `lea esi, [eax+0x2c]`) and spills the AI object pointer (velocity read
// at +0x88). Then-arm `return local_2;` with a final `return false;` 510; false-initialised result without the
// else arm 496; object pointer local for the six velocity reads 409 (1918 bytes); facing field read at each use
// 513 (1941 bytes); both 524; `facing` declared in the else arm only, the then-arm copy read from the field
// 477 (1906 bytes). Original body restored.
// Round 37 (lane 1, fast): the existing magnitude3d call for the written-out object-speed root 482
// (1886 bytes: our LTCG keeps the call, retail inlines it here and calls it in 0x1aa0d0); result
// preset false without the else 496 (frame 0x54); no facing pointer, the prop view's facing read at
// each use (retail addresses it off the view) 513 (1941 bytes); that with an object pointer and
// `object->velocity` reads (retail spills the object pointer, not object + 0x88) 524; the object
// pointer alone 409.
// Round 181 (lane 3): aligned through the moved early exit only 170 rows differ: our code is
// retail's apart from the early false exit's place (retail puts it after the first `goto local_7`
// jump; ours is last) and the object/facing/query homes. Retail stores the true result once
// (`mov byte [esp+0x13], 1`) and reloads it at the end, so its result is not folded to a constant
// even though the false path has its own exit. The result preset true with the false arm first
// 399 (the true store hoists to the entry, the exit stays last); an early `return false` with the
// result declared true before it 510 (frame 0x54: the result folds); the facing pointer declared
// after the flag and vector 384 (identical); the near-facing flag set before its copy 384
// (identical); that copy read from the view directly 397.
// Lane 18 round 15 (fast with the callees): VC pulls a block up behind its first predecessor's chain only when jump
// threading created it, so retail's false exit is a threaded copy of the return; the computed entry flag
// (`bool local_2 = !f13 && !f14 && !f2d; if (local_2)`, lane 6's form) re-measured 517: it lands the exit where
// retail does, but arg_0 then keeps EDI (the settings pointer is spilled) and the frame is 0x54. On that form:
// `!(f13 || f14 || f2d)` 517; the actor taken before the flag 518 (1921 bytes); the settings lookup before the
// actor 511 (1912 bytes); a redundant `local_2 = true` in the then-arm (diagnostic) 530. On the original form:
// the last 0x1a9e00 dispatch moved after the if and guarded by `local_2 &&` 406 (the exit stays last); the
// result declared before the slot view 384 (identical).
// Round 262: Returned admission predicate with entry declarations of query speed/time/direction,
// their three pairs/all, and all plus actual request: 517 each (1926/1904, frame 0x54).
// Original result, scopes, input/output query aliases, arithmetic and complete body restored.
// Round 288: Prop handle before/after metadata, then before with actor/target/both position receivers and after with actor/target/both receivers,
// 458/473/500/494/477/457/494/483 (fast). Original complete body, observations, helpers and call roles restored.
// Round 232: Complete branch-owned direction distance 382; query scope/names/
// declaration order 382/382/382; target-velocity receiver and scope/names/both 381/381/381/381.
// Three-count best rejected; all executable code, query aliases and observations restored.
// Round 118 (fast): an object pointer local from the object getter 411 (1919 B), from the pool expression 411; the
// dot product through the view's field 400 (1908 B), with the object pointer 484 (1918 B); the weight as a ternary 386;
// the object velocity pointer before the magnitude 381 (one count, not kept).
// Round 318: Progressive returned admission at short-circuit boundaries, first/last/all flag
// partitions in bool/byte, then first/last in long; 517/517/517/526/526/526/543/543. Original
// whole body and helpers restored.
// Round 331: Query and later planar directions share one consumed vector at
// direction/eligible/entry/receiver scopes 392 each; with sequential query/deadline time storage 394
// each. Original workspaces, entry/result graph, query aliases and entire body restored; early
// rejection and magnitude/query homes remain.
// Round 536: True result published after actor/metadata/initial prop phase/before deadline, bool then byte storage: 384/389/419/526/431/436/466/500 fast.
// Original result, admission and complete body restored; early exit and magnitude/query homes remain.
// DL249: Coherent original-provider inline contracts, seven subsets, then all with an
// earlier uninitialized result declaration: 466/440/507/382/466/440/507/507.
// The isolated third contract ties; others lose matched callers. All declarations and bodies restored.
// AC486: Complemented rejection result owned at entry, bool/byte/word/long 528/528/545/544;
// branch-owned complement 437/437/448/453. Original result and whole body restored;
// early rejection placement and magnitude/query homes remain.
// N328: Readiness false producer at far entry versus only the non-action mode, bool/byte/short/long:
// 382/382/482/415/380/380/480/413. NOT kept: complete bool/byte results gain only two. Original
// readiness, admitted return and query aliases restored; early rejection and magnitude/query homes
// remain.
// N334: Paired query success across fallback, nested/flat/exhaustive/complete fallback arms
// in bool then byte storage: 461/461/382/461/461/461/382/461. Exhaustive forms tie; other frames move
// to 0x54 and 1908 bytes. Original query aliases, call roles, result and complete body restored.
// AC492: Complementary internal eligibility, test-produced bool/byte/short/long
// 389/389/450/452; branch-produced complement 383/383/449/451. Original admission,
// eligibility, query aliases and complete body restored; early rejection and magnitude/query homes remain.
// Y547: Active-unit state publication, split/exhaustive-success/conditional/exhaustive-failure,
// bool admitted result 398/470/384/470; byte result 445/500/429/500.
// Original field-result graph and complete body restored; early rejection and scalar homes remain.
// Round 33 (m3 lane 18, fast; 0x1a9d80 and 0x1a9e00 matched each time): the outer test inverted with
// the false arm first 382 (same code); one goto to the single return, result preset false 496; the
// game time read at its only use 441 (1907 bytes); the facing threshold as a 0.95 preset with an if
// 381 (one count, not kept), as a full if/else 382; the distance zeroed in the facing arm 382; the
// limit declared in the flag block 396, as a ternary there 400; the field_2e approach flag as an if /
// else-if chain 455 (1912 bytes). All restored; the threaded false exit's place remains.
// Round 35 (m3 lane 18, fast, 0x1a9d80/0x1a9e00 matched each time): the first goto replaced by nesting
// (the approach arm in the threshold test's else, `field_2c` set in both arms, no near flag) 492 (1888
// bytes); the near-limit comparison written `unknown28 < field_1c` 382; the timeout scale passed
// straight to the rounding helper 382; the near flag declared before the facing pointer 382; kept: the
// second goto (the near-limit stop) as an `else if` for the approach query 382 (plain, same code).
// Y558: Shared add-scaled/difference prediction into a point or consumed direction,
// squared-length boundary independently, 478/478/478/478; complete relative-velocity
// input on each graph 474/474/474/474. NOT kept: all regress and change the frame or
// length. Original prediction, helper set, query aliases and complete body restored.
// Round 57 (m3 lane 13, fast; 0x1a9d80/0x1a9e00 matched each time): the prop-index test inverted (`== NONE`
// clears field_2c first) 497; the inner admission test inverted with `local_6 = false` first 382 (same code);
// local_6 dropped with the second test re-testing field_12/17/18 442 (1907 bytes); the second goto as
// `local_10 = false;` 382 (same code); the query scalars renamed to sort in retail's slot order (local_20 <
// local_19 < local_15 by address: flight_time/launch_speed/prediction_time) 382 and in the reverse order
// (time_of_flight/rise_speed/lead_time) 382: names do not move these homes.
// AC513: One metadata near-limit observation, with readonly metadata/prop/both,
// returned admission, original-object receiver, query-output names or shared squared length:
// 469/469/469/469/519/478/469/471. NOT kept: retail reloads the limit; no exact
// match, frames/lengths move away. Original observations and whole body restored.
// P299: Input-only actor/metadata/paired prop receivers on the current velocity graph,
// seven subsets and immutable bindings: 382 each. Original complete body restored;
// early rejection island and magnitude/object/query homes remain.
// P301: Threaded admitted result plus native complete object-base receiver, acquired
// after magnitude/weight, each with retained/direct facing reads, bool then byte:
// 534/571/522/554/551/588/543/585. NOT kept: all regress and enlarge code; original
// result, velocity/facing receivers, query aliases and full body restored.
// DH322: Sequential magnitude/query speed, prediction/flight duration and limit/direction distance,
// seven subsets then entry declarations: 384/445/382/446/384/445/446/446. NOT kept:
// storage nonmatches; original scalar lifetimes/body restored, early exit and query homes remain.
// AC516: Complete post-query planar-facing admission, bool/byte/short/long at computation
// or with the direction-workspace declaration: 382/382/382/382/382/382/382/382.
// NOT kept: one-use predicate storage unmatched; original gate, query aliases and complete body restored.
// AC520: Planar normalizer returns at query-pair/facing/request-pair producers, subsets
// 1/2/4/3/5/6/7 then phase-owned uninitialized storage: 382/382/382/382/382/382/382/382.
// NOT kept: one-use call-result residence does not match; original calls, aliases and full body restored.
// AC523: Uniform magnitude-square/sum domains, native terms with real/double sum,
// real terms with native/real/double sum, double terms with native/real/double sum:
// 382/381/382/468/381/381/468/381. NOT kept: best gain one and arithmetic storage
// unmatched; original magnitude, query aliases and complete body restored.
// F367: Complete facing-alignment/prediction-seconds/projected-distance nodes, masks 1/2/4/3/5/6/7, then all with remaining-distance node: 382 each. NOT kept: eight byte-neutral trials; original arithmetic, live observations, query aliases and full body restored.
// AC525: Complete final dispatch eligibility/rejection results in bool/byte/short/long,
// eight at 382. NOT kept: predicate residence unmatched; original final guards,
// field observations, query aliases and full body restored; early rejection island remains.
// AC527: Shared two-mode timeout publication, positive/complementary bool/byte/short/long,
// 422/422/434/433/422/422/434/433. NOT kept: assigned-result residence unmatched and
// extent grows; original comparisons, publication paths, query aliases and whole body restored.
// AC531: Live unit-handle field references/pointers acquired at actor/metadata/prop/action
// boundaries, eight at 382/1904 bytes. NOT kept: native field identity canonicalizes;
// early rejection and magnitude/query homes remain. Original whole body restored.
// AC534: Request-submission accepted/rejected results in bool/byte/short/long,
// at the original call with the admitted return live: 382/382/382/382/382/382/382/382.
// NOT kept: one-use call-result ownership lacks equality; original dispatch, state stores,
// query aliases and whole body restored. Early rejection and magnitude/query homes remain.
// AC536: Complete initial near-stop stop/continuation result in bool/byte/short/long:
// 381/381/381/381/381/381/381/381. NOT kept: one-point gain below the gate and no equality;
// original near gate, provider timing, query aliases and full body restored. Early rejection island remains.
// F385: Existing vector-union component-array observations in target velocity, facing
// and working vectors, seven subsets, then actual position arrays: eight at 382.
// NOT kept: byte-neutral homes/exit diagnostic; original complete body and query aliases restored.
// AC560: Returned actor/state/view record references, seven subsets, then complete facing
// reference: 382/382/382/382/382/382/382/382. NOT kept: no byte equality; early rejection and magnitude/query
// homes remain. Original receivers, query aliases, arithmetic and full body restored.
// Round 90 (m3 lane 16, fast): the query vector and scalars declared in reverse order 382 (same code); the else arm as
// `return false;` 509; the result declared before the slot view 382 (same code); the prediction scale written rate first
// 382 (same code); the entry tests as `f13 || f14` and `else if (f2d)` false arms ahead of the body 382 (same code).
// Aligned by sequence, 160 rows differ: the false exit placed last shifts everything up to retail's place for it.
// m3 lane 8 r146 (fast; aligned/masked base 155/105): `local_2 = true` at the end of its block 508; `local_4 = NULL` declared before
// the actor pointer 494 (1913 bytes); `bool local_2 = true;` at its declaration 397; local_2 declared before local_0 382 (same code);
// local_6 computed directly from its three tests, no else arm 389.
// Lane 12 r107 (fast; masked aligned count in brackets, 105 at 382): retail reads `local_8->field_4.x` and the actor
// position's x through the two addresses it later passes to 0xfa1a0 (`lea ecx, [esi+0x238]; lea ebx, [edi+4]` ahead of the
// direction vector), y and z off the bases. Named actor/target position pointers for the direction vector and both 0xfa1a0
// calls 453 [104] (1906 B; every component then reads through them); the facing pointer formed after the first dot (that dot
// through the view's field) 494 [164] (1918 B), formed after the direction vector 509 [168] (1940 B); lane 6's computed
// entry flag with an object pointer for the velocity reads 533 [129] (1945 B); the 0.95 facing test through the existing
// dot_product2d 382 [103] (same score; retail's j-then-i product order is not restored).
// Round 68 (m3 lane 15, fast): the admission as an else-if chain setting the result false per flag 527 (1885 B); the
// result as `char` returned as `!= 0` 438 (1925 B); `bool local_2 = false;` with nested flag ifs 495; the else arm as
// `return false;` 508; the result declared before the state pointer 382 (same code).
// Round 169 (m3 lane 6, fast with the file's 15 matched functions and 0x1aa0d0 unless noted): the object-speed root
// through the existing magnitude3d, plain check, 481 (1887 bytes: still a call in the full build too); the facing-request
// helper shared with 0x1aa0d0 382 (same code); the prediction products written field first 382 (same code); the weight's
// dot read through the prop view (retail's `[eax + 0x34]`) 401 (1909 bytes); no facing pointer until after the prediction
// 516 (1944 bytes); the shared predicted-offset helper 444 (1906 bytes); the shared planar facing test 480 (1915 bytes).
// M3 lane 1 r90 (fast with the file's functions and 0x265d30; aligned/register-masked counts in brackets, control 382
// [155/82]): BLOCKED-BY 0x25d740's inlining. Lane 6's computed entry result (`bool local_2 = !f13 && !f14 && !f2d;
// if (local_2) {...} return local_2;`, re-measured 508 [239/121], 1921 B, frame 0x54) is retail's shape except that our
// link inlines 0x25d740 there (20 calls against retail's 21). Diagnostic, reverted at once: the same form with 0x25d740
// kept out of line (a noinline declaration in its own file) gives retail's frame 0x58, the false exit at retail's place
// (after the first `goto` jump), retail's 1904 bytes and the first difference at +0x147: 394 [151/88]. The call sits
// under three tests there, and one more test (diagnostic) also keeps it out of line, so the inliner goes by the call
// site's static frequency: the && test in the `if` (this body) keeps the site cold but leaves the false exit last, and
// the computed result threads the exit but warms the site. Retail never inlines 0x25d740 (24 callers), and matched
// 0x1b4560 needs `#pragma inline_depth(0)` around its call, so the getter's inline availability (its file's flags) is
// the question for a ruling. Forms: the computed result with the facing read off the view at each use and an AI object
// pointer for the velocity reads 577 [285/160] (frame 0x50; with 0x25d740 out of line 435 [207/118], 1956 B); with the
// object pointer only 533 [241/116] (with 0x25d740 out of line 416 [207/72], 1924 B, frame 0x54); with the f12/f18
// test as nested ifs 514 (still inlined); with the existing magnitude3d for the object-speed root 508 (same code: in this
// form our link inlines magnitude3d, as retail does, and 0x25d740 too); on this body `return local_2 != false;` 438
// [173/96] (1925 B, exit still last; the same numbers as round 68's char result).
// M3 lane 14 r80 (fast; masked = aligned count with registers masked, 111 at 382): register-blind, ours differs from retail in the
// early false exit's place and in the velocity block, where retail keeps the view pointer in EAX (facing read as [view+0x34], the
// facing pointer formed late with `lea esi, [eax+0x2c]`) and the actor in ESI, ours the facing pointer in ESI from the view call with
// the actor spilled. The first dot written facing first 382 (same code); `bool local_10 = 0.8f > unknown28` with the copy arm only
// 481 (masked 186); the distance local_17's zero declared at the top of the far arm 466 (112); the weight local_14's zero declared
// before the magnitude 382 (112); the 0.8 test negated with its arms swapped 468 (129). Re-measured: the facing field read through
// the view at each use, no pointer, 517 (1945 bytes, masked 154): it gives retail's view-in-EAX reads and late `lea` for the
// facing pointer, but local_8 and the actor swap ESI/EDI and the object velocity pointer is still formed (+0x88). On it, the
// object read through an `s_ai_object *` local (as this file's 0x1aa9f0, 0x1a9ec0 and 0x1aa750 do) 531 (1960 bytes, masked 152); that
// object pointer alone on the original body 486 (1924 bytes, masked 169).
// MiniMax A (11-11, second pass): no new form, no lower score (382); the entry tests returning false straight away (the body outside them) makes our build fold the result byte away and shrink the frame to 0x54 (508), so retail's two exits - the false one at 0x1aac35 with its own epilogue and the true one reloading the result byte from the stack - are still not both there: ours ends the rejection path at the false exit at the end of the body instead of retail own epilogue, and in the velocity block ours still forms the facing pointer in ESI from the view call while retail keeps the view pointer in EAX, reads the facing field through it and forms the facing pointer late with lea esi, [eax+0x2c] with the actor in ESI.
// @retail 0x1aab50
bool __stdcall function_1aab50(long arg_0, s_slot *arg_1)
{
	s_1aa0d0 *local_0 = (s_1aa0d0 *)arg_1;
	bool local_2;
	if (!local_0->field_13 && !local_0->field_14 && !local_0->field_2d)
	{
		local_2 = true;
		s_actor_view *local_1 = actor_get(arg_0);
		s_1aab50 *local_3 = (s_1aab50 *)function_1e4f90(arg_0);
		s_prop_node_view *local_4 = NULL;
		if (local_3)
		{
			if (local_1->prop_index != NONE)
			{
				local_4 = prop_node_get(local_1->prop_index);
				real local_5 = 3.402823466e+38f;
				bool local_6 = true;
				if (!local_0->field_12 && !local_0->field_17 && !local_0->field_18)
				{
					if (local_1->unknown225 || !function_1e2030(arg_0))
						local_5 = local_3->field_10;
					if (local_4->unknown28 > local_5 && function_1e2030(arg_0))
					{
						local_0->field_14 = true;
						goto local_7;
					}
				}
				else
					local_6 = false;
				local_0->field_2c = true;
				if (local_6 && !((s_prop_state_64 *)function_25d690((s_prop_datum *)local_4))->unknown64 && local_3->field_1c > local_4->unknown28)
				{
					local_0->field_10 = 0x10;
					local_0->field_14 = true;
				}
			}
			else
				local_0->field_2c = false;
		}
	local_7:
		if (local_0->field_12)
			local_0->field_13 = local_1->unknown018 == NONE || !function_110ab0(local_1->unknown018);
		else if (!local_0->field_18)
		{
			s_1aa0d2 *local_8 = (s_1aa0d2 *)function_25d690((s_prop_datum *)local_4);
			s_prop_view_fields *local_9 = (s_prop_view_fields *)function_25d740((s_prop_node *)local_4);
			vector3f const *facing = &local_9->unknown2c;
			bool local_10 = false;
			vector3f local_11;
			if (0.8f > local_4->unknown28)
			{
				local_11 = *facing;
				local_10 = true;
			}
			else
			{
				real local_12 = (real)sqrt(length_sq3f(&local_8->field_1c));
				vector3f const *object_velocity = &ai_object_get(local_1->unknown018)->velocity;
				real local_14 = 0.0f;
				if (local_12 > 0.0f)
					local_14 = (1.0f + (local_8->field_1c.k * facing->k + local_8->field_1c.j * facing->j + local_8->field_1c.i * facing->i) / local_12) * 0.5f;
				real local_15 = (real)local_0->field_30;
				real local_16 = local_15 * g_510c54->rate * local_14;
				local_11.k = local_16 * local_8->field_1c.k + local_8->field_4.z - local_1->position.z;
				local_11.j = local_16 * local_8->field_1c.j + local_8->field_4.y - local_1->position.y;
				local_11.i = local_16 * local_8->field_1c.i + local_8->field_4.x - local_1->position.x;
				real local_17 = 0.0f;
				if (0.0f > facing->k * local_11.k + facing->j * local_11.j + facing->i * local_11.i)
					local_11 = *facing;
				else
				{
					local_17 = function_30bf0(&local_11);
					if (local_17 == 0.0f)
						local_11 = *facing;
				}
				if (!local_0->field_17)
				{
					if (local_3->field_1c > local_17 && !function_25d9b0(local_1->prop_index) && !((s_prop_state_64 *)local_8)->unknown64)
					{
						local_0->field_14 = true;
					}
					else if (local_3->field_20 > local_17)
					{
						real local_19 = local_3->field_28;
						real local_20;
						vector3f local_22;
						/* retail reuses the speed variable as the vertical speed output and stores that result below */
						if (function_fa1a0(local_19, 1.0f, &local_1->position, &local_8->field_4, &local_15, NULL, NULL, false, &local_22, NULL, NULL, NULL, &local_19, &local_20) ||
							(local_3->field_2c > local_15 && function_fa1a0(local_15 + 0.03f, 1.0f, &local_1->position, &local_8->field_4, NULL, NULL, NULL, false, &local_22, NULL, NULL, NULL, &local_19, &local_20)))
						{
							if (normalize2d((point2f *)&local_22) == 0.0f)
							{
								local_22 = local_1->unknown290;
								if (normalize2d((point2f *)&local_22) == 0.0f)
									local_22 = *g_4687a8;
							}
							local_0->field_24 = local_20;
							local_0->field_18 = true;
							local_0->field_1c = *(point2f *)&local_22;
							local_0->field_28 = local_19;
						}
					}
				}
				else if (local_0->field_2e)
				{
					local_10 = local_0->field_34 > local_17 || (0.0f > local_17 &&
						(local_8->field_1c.k - object_velocity->k) * local_11.k + (local_8->field_1c.j - object_velocity->j) * local_11.j + (local_8->field_1c.i - object_velocity->i) * local_11.i > 0.7f);
				}
				else
					local_10 = 0.5f > local_17 - (object_velocity->k * local_11.k + object_velocity->j * local_11.j + local_11.i * object_velocity->i) * local_15 * g_510c54->rate;
			}
			if (local_0->field_18 || (local_10 && !local_0->field_2e))
			{
				point2f local_25 = *(point2f const *)&local_11;
				if (normalize2d(&local_25) > 0.0f)
				{
					real local_26 = local_0->field_17 ? 0.0f : 0.95f;
					if (local_26 > local_1->unknown290.j * local_25.y + local_1->unknown290.i * local_25.x)
					{
						local_0->field_18 = false;
						goto local_27;
					}
				}
			}
			if (local_10)
			{
				s_unit_request local_28;
				memset(&local_28, 0, sizeof(local_28));
				local_28.type1a.unknown4 = 0;
				local_28.type = 0x1a;
				point2f *local_29 = (point2f *)(local_28.arguments + 4);
				*local_29 = *(point2f *)&local_11;
				local_28.type1a.unknown6 = true;
				if (normalize2d(local_29) == 0.0f)
				{
					*local_29 = *(point2f *)&local_1->unknown290;
					if (normalize2d(local_29) == 0.0f)
						*local_29 = *g_468778;
				}
				if (function_e6900(local_1->unknown018, &local_28))
				{
					unit_submit_entry_request(0x13, NONE, local_1->unknown018, 0xd, NULL);
					local_0->field_12 = true;
				}
			}
		}
	local_27:
		long local_30 = g_510c54->game_time;
		if (!local_0->field_12 && !local_0->field_18)
		{
			if (local_0->field_17)
			{
				if ((real)local_0->field_1a * g_510c54->rate > 0.5f)
					local_0->field_14 = true;
			}
			else
			{
				real local_31 = (real)g_510c54->field_2_3 * local_3->field_14;
				long local_32 = real_to_long(local_31);
				if (local_30 >= local_0->field_c + local_32)
					local_0->field_14 = true;
			}
		}
		if (!local_0->field_12 && !local_0->field_17 && !local_0->field_18)
			function_1a9d80(arg_0, local_3->field_1c, &local_0->field_2c, &local_0->field_2d);
		else
			function_1f4280(arg_0);
		if (!local_0->field_17 && !local_0->field_18 && !local_0->field_12)
			function_1a9e00(arg_0, local_0->field_34, local_0->field_2d);
	}
	else
		local_2 = false;
	return local_2;
}

// @retail 0x1a9ec0
short __stdcall function_1a9ec0(long arg_0)
{
	extern long function_146650(void);
	s_actor_view *local_0 = actor_get(arg_0);
	s_actor_tag_entry *local_1 = (s_actor_tag_entry *)function_1e4f90(arg_0);
	long local_2 = 0;
	if (local_1 && local_0->prop_index != NONE && !function_25d9b0(local_0->prop_index))
	{
		long unit_index = local_0->unknown018;
		if (!function_110ab0(unit_index))
		{
			s_ai_object *local_3 = ai_object_get(local_0->unknown018);
			if (!(local_3->flags19 & 1) || (!(local_3->flags19 & 2) && (local_1->flags & 1)))
			{
				s_prop_node_view *local_4 = prop_node_get(local_0->prop_index);
				s_prop_state_view *local_5 = (s_prop_state_view *)function_25d690((s_prop_datum *)local_4);
				s_prop_view_fields *local_6 = (s_prop_view_fields *)function_25d740((s_prop_node *)local_4);
				vector3f const *facing = &local_0->unknown290;
				if ((local_0->unknown229 || !((s_prop_state_64 *)local_5)->unknown64) && local_4->unknown27 >= 1)
				{
					if (local_0->unknown225)
						local_2 = 3;
					else
					{
						bool choose_four = !function_1e2030(arg_0) || (local_6 && local_1->unknown0c > local_4->unknown28 &&
							-0.4f > local_6->unknown2c.k * facing->k + local_6->unknown2c.j * facing->j + local_6->unknown2c.i * facing->i &&
							!local_0->unknown264 && function_10f340(local_0->unknown018, 0x7000101, 0xa0005b9));
						if (choose_four)
						{
							local_2 = 4;
						}
						else if (local_5->unknown3c == NONE && local_1->unknown04 >= local_4->unknown28 &&
							(local_0->times[3] == NONE || (real)(function_146650() - local_0->times[3]) * g_510c54->rate > function_1e9700(0x15) * local_1->unknown18))
						{
							real local_8 = function_1c9ee0(local_1->unknown08);
							if (local_8 > function_259a0(&g_4e7408->unknown0))
								local_2 = 3;
						}
					}
				}
			}
		}
	}
	return (short)local_2;
}

bool unit_animation_get_timing(long arg_0, long arg_1, long arg_2, short *arg_3, real *arg_4, short *arg_5, real *arg_6);
long function_1e4a50(long arg_0);

struct s_1aa750
{
	byte field_0[4];
	real field_4;
};

struct s_1aa751
{
    long field_0;
    short field_4;
    byte field_6[3];
    char field_9;
    bool field_a;
    byte field_b[0x24 - 0xb];
    short field_24;
    short field_26;
    real field_28;
    real field_2c;
};

// @retail 0x1aa750
bool __stdcall function_1aa750(long arg_0, s_slot *arg_1, bool arg_2)
{
	bool local_11;
	s_actor_view *local_0 = actor_get(arg_0);
	s_1aa0d1 *local_1 = (s_1aa0d1 *)function_1e4f90(arg_0);
	s_1aa751 *local_3 = (s_1aa751 *)((byte *)arg_1 + 0xc);
	s_ai_object *local_2 = ai_object_get(local_0->unknown018);
	local_11 = false;
	if (!((local_2->flags19 & 1) && ((local_2->flags19 & 2) || !(*(byte *)local_1 & 1))))
	{
		short local_4 = 0;
		short local_5 = 0;
		real local_6 = 0.0f;
		real local_7 = 0.0f;
		memset(local_3, 0, 0x30);
		local_3->field_0 = g_510c54->game_time;
		local_3->field_4 = NONE;
		local_3->field_a = (local_2->flags19 & 1) != 0;
		local_3->field_26 = 0;
		local_3->field_2c = 0.0f;
		if (local_1)
		{
			local_3->field_28 = local_1->field_c;
			long unit_index = local_0->unknown018;
			long local_8 = arg_2 ? 0xa000040 : local_3->field_a ? 0xe00067d : 0x500000a;
			if (unit_animation_get_timing(unit_index, 0x7000101, local_8, &local_4, &local_6, &local_5, &local_7))
			{
				short local_16 = local_4;
				if (local_16 == NONE)
				{
					if (arg_2)
					{
						local_6 = 0.0f;
						local_16 = 0;
					}
					else
					{
						local_16 = local_5 / 2;
						local_6 = local_7 * 0.5f;
					}
				}
				local_3->field_24 = local_16;
			}
			else
				local_3->field_24 = 0;
			local_3->field_9 = 0;
			if (local_0->prop_index != NONE)
				function_1fb7e0(0x1f, arg_0, NULL, prop_node_get(local_0->prop_index)->object_index, NONE);
			if (!arg_2)
			{
				s_1aa750 *local_9 = (s_1aa750 *)function_1e4a50(local_0->unknown054);
				real local_10 = 0.0f;
				if (local_9)
					local_10 = local_9->field_4 + 0.2f;
				local_10 += local_6;
				local_3->field_28 = local_3->field_28 > local_10 ? local_3->field_28 : local_10;
				local_8 = local_3->field_a ? 0xe00067d : 0xc0006cd;
				real local_15 = 0.0f;
				short local_14;
				real local_13;
				short local_12 = 0;
				unit_index = local_0->unknown018;
				if (unit_animation_get_timing(unit_index, 0x7000101, local_8, &local_12, &local_13, &local_14, &local_15))
				{
					local_3->field_26 = local_12;
					local_3->field_2c = local_15;
				}
			}
			local_11 = true;
		}
	}
	return local_11;
}
