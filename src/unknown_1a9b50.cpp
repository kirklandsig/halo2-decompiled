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
	byte field_19[0x2c - 0x19];
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

// score 375 (was 375): Frame 0x40 agrees; early actor/settings/prop carriers and prediction homes remain.
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

struct s_1aab51
{
	byte field_0[0x1c];
	point2f field_1c;
	real field_24;
	real field_28;
};

static __forceinline long real_to_long(real value);

// score 382 (was 384): the frame and the length agree, and the early false exit's place, the
// object/magnitude homes and the query scalar homes still differ (one earlier round found no new form).
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
				real local_12 = (real)sqrt(local_8->field_1c.i * local_8->field_1c.i + local_8->field_1c.j * local_8->field_1c.j + local_8->field_1c.k * local_8->field_1c.k);
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
						goto local_18;
					}
					if (local_3->field_20 > local_17)
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
							s_1aab51 *local_24 = (s_1aab51 *)local_0;
							local_24->field_24 = local_20;
							local_0->field_18 = true;
							local_24->field_1c = *(point2f *)&local_22;
							local_24->field_28 = local_19;
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
		local_18:
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
				if ((real)*(short *)((byte *)local_0 + 0x1a) * g_510c54->rate > 0.5f)
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
