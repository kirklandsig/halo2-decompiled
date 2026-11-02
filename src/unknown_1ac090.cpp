// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1AC090.CPP: the slot handlers of types 0x2c, 0x2b and 0x54
   (0x47dbe8..0x47dcd4) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "ai_actor.h"
#include "unknown_1fb7e0.h"
#include "unit_requests.h"
#include "unknown_2605d0.h"
#include "unknown_2626b0.h"
#include "vector_products.h"

/* the state of the slots of types 0x2c and 0x2b */
struct s_slot_2c
{
	s_slot_header header;
	bool unknown0c;
	bool unknown0d;
	short ticks;
	bool unknown10;
	byte unknown11;
	bool unknown12;
	bool unknown13;
	vector3f facing;
	short unknown20;
	short unknown22;
	bool unknown24;
	bool unknown25;
	byte unknown26[2];
	s_type_c3b527 point;
	char unknown38;
	bool unknown39;
	bool unknown3a;
	bool unknown3b;
	byte unknown3c[0x40 - 0x3c];
};

/* the state of a slot of type 0x54 */
struct s_slot_54
{
	s_slot_header header;
	bool unknown0c;
	byte unknown0d;
	short ticks;
	byte unknown10[0x40 - 0x10];
};

/* the same state past the slot header (s_slot_54's layout from +0xc) */
struct s_slot_54_data
{
	bool unknown00;
	byte unknown01;
	short ticks;
	byte unknown04[0x34 - 4];
};

struct s_ai_actor_view_3c
{
	byte unknown000[0x3c];
	bool unknown03c;
	byte unknown03d[0x20c - 0x3d];
	long unknown20c;
	byte unknown210[0x328 - 0x210];
	short unknown328;
	byte unknown32a[0x3f4 - 0x32a];
	long unknown3f4;
};

struct s_prop_datum_54
{
	byte unknown00[0x27];
	char unknown27;
	real unknown28;
	byte unknown2c[0x3c - 0x2c];
};

struct s_tag_element_54
{
	byte unknown00[0x98];
	real unknown98;
	real unknown9c;
	real unknowna0;
};

/* the unit's current mode: a short at +0x36 of its current state, whose
   offset is at +0x346 */
#define UNIT_MODE(unit_index) \
	(*(short *)((byte *)ai_object_get(unit_index) + 0x36 + *(short *)((byte *)ai_object_get(unit_index) + 0x346)))

#define ACTOR_VIEW_3C(actor) ((s_ai_actor_view_3c *)(actor))

/* the block of the actor's character tag (function_1e4d10) as handler 0x2b
   reads it */
struct s_character_2b
{
	byte unknown00[4];
	real lower;
	real upper;
	byte unknown0c[0x18 - 0xc];
	real unknown18;
};

void *function_1e4d10(long actor_index);
void function_1f86a0(long index);
bool function_25d9b0(long prop_index);
bool actor_has_joint_invitation(long actor_index, short type);

/* function_259d0 (unknown_0259d0), which retail inlines here: the random value
   is drawn into a local first */
static inline real random_range(real lower, real upper)
{
	real random = slot_random();

	return lower + (upper - lower) * random;
}

bool function_e68c0(long type, long unit_index);
void function_26def0(long actor_index, long owner_index = NONE);
bool function_1f86f0(long index);
short __stdcall function_1ac100(long actor_index, s_slot *slot, s_slot *next);
bool __stdcall function_1acda0(long actor_index, s_slot *slot);
void __stdcall function_1ad130(long actor_index, s_slot *slot);
bool __stdcall function_1ad6a0(long arg_0, s_slot *arg_1);
void __stdcall function_1ada70(long actor_index, s_slot *slot);

/* the slot state of types 0x2c and 0x2b seen past its header (s_slot_2c's layout from +0xc),
   as 0x1ac090 and 0x1acda0 read it */
struct s_1acda0
{
    bool unknown0c;
    bool unknown0d;
    short ticks;
    bool unknown10;
    byte unknown11;
    bool unknown12;
    bool unknown13;
    vector3f facing;
    short unknown20;
    short unknown22;
    bool unknown24;
    bool unknown25;
    byte unknown26[2];
    s_type_c3b527 point;
    char unknown38;
    bool unknown39;
    bool unknown3a;
    bool unknown3b;
    byte unknown3c[0x40 - 0x3c];
};

PRIVATE __forceinline void local_0(real seconds, short *timer)
{
	long ticks;
	__asm
	{
		fld seconds
		fistp ticks
	}
	*timer += (short)ticks;
}

PRIVATE __forceinline void local_1(real seconds, short *timer)
{
	long ticks;
	__asm
	{
		fld seconds
		fistp ticks
	}
	*timer = (short)ticks;
}

/* ---- slot type 0x2c ---- */

// @retail 0x1ac090
bool __stdcall function_1ac090(long actor_index, s_slot *slot)
{
	s_1acda0 *state = (s_1acda0 *)slot->unknownc;

	local_1(g_510c54->field_2_3 * ((function_x82e52f(&g_4e7408->unknown0, 0, 0) + 1.0f) * 2.0f), &state->ticks);
	state->unknown39 = false;
	return true;
}

void function_265bb0(long actor_index);

// @retail 0x1ac100
short __stdcall function_1ac100(long actor_index, s_slot *slot, s_slot *next)
{
	short result = g_46fbe8;
	s_actor_view *actor = actor_get(actor_index);
	s_1acda0 *state = (s_1acda0 *)slot->unknownc;
	long unit_index = actor->unknown018;
	short mode = UNIT_MODE(unit_index);
	if (mode != 0 && ((real *)actor->unknown2d8)[1] + ((real *)actor->unknown2d8)[0] > g_45dbd8)
	{
		short delay = 2 + random_index(&g_4e7408->unknown0, 3);
		local_0((real)delay * g_510c54->field_2_3, &state->ticks);
		state->unknown12 = false;
		local_1(g_510c54->field_2_3 * 0.5f, &state->unknown22);
		if (actor->prop_index != NONE)
			function_265bb0(actor_index);
		if (function_e68c0(0x24, actor->unknown018))
		{
			g_46eeb8[0x2c]->unknown8 = g_46f348;
			*next = *slot;
			state->unknown39 = true;
			result = 0x2b;
		}
		else
			result = g_46fbe4;
	}
	else if (mode == 0)
		result = 0x2b;
	else if (state->ticks == 0)
	{
		function_e68c0(0x27, unit_index);
		g_46eeb8[0x2b]->unknown8 = g_46f348;
		result = g_46fbe4;
	}
	return result;
}


// @retail 0x1ac270
void __stdcall function_1ac270(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (!state->unknown39)
	{
		s_actor_view *actor = actor_get(actor_index);
		long local_0 = actor->unknown018;
		s_unit_request request;
		memset(&request, 0, sizeof(request));
		request.type = 0x27;
		function_e6900(local_0, &request);
		if (!ACTOR_VIEW_3C(actor)->unknown03c && ACTOR_VIEW_3C(actor)->unknown3f4 != NONE)
			function_26def0(actor_index);
	}
}

// @retail 0x1ac300
bool __stdcall function_1ac300(long actor_index, s_slot *slot)
{
	bool result = false;
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;

	switch (UNIT_MODE(actor->unknown018))
	{
	case 6: result = function_e68c0(0x25, unit_index); break;
	case 7: result = true; break;
	}
	return result;
}

// @retail 0x1ac360
void __stdcall function_1ac360(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (state->ticks > 0)
		state->ticks--;
}

// @retail 0x1ac380
void __stdcall function_1ac380(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;

	actor->unknown41c = 4;
	actor->unknown420 = 2;
	if (UNIT_MODE(unit_index) == 7)
		actor->unknown488 = true;
}

/* ---- slot type 0x2b ---- */

// @retail 0x1ac3f0
short __stdcall function_1ac3f0(long actor_index)
{
	short result = 0;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE && !actor->unknown007)
		result = 3;
	return result;
}

/* starts the slot: draws the delay and the scale, resets the state and its two timers */
// @retail 0x1ac430
bool __stdcall function_1ac430(long actor_index, s_slot *slot)
{
	s_character_2b *character = (s_character_2b *)function_1e4d10(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;
	bool *flag = &state->unknown39;

	if (!state->unknown0c)
	{
		real delay = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, character->lower, character->upper);
		real random = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);
		real scale = 1.0f + 2.0f * random;

		state->unknown0d = true;
		state->unknown12 = false;
		state->unknown10 = false;
		state->unknown25 = false;
		state->unknown3a = false;
		state->unknown0c = true;
		state->unknown22 = 0;
		state->unknown3b = false;
		{
			real seconds = g_510c54->field_2_3 * scale;
			long ticks;
			__asm
			{
				fld seconds
				fistp ticks
			}
			state->unknown20 = (short)ticks;
		}
		state->unknown13 = false;
		function_1f86a0(actor_index);
		if (g_45dbd8 > delay)
		{
			state->unknown24 = true;
		}
		else
		{
			real seconds = g_510c54->field_2_3 * delay;
			long ticks;
			__asm
			{
				fld seconds
				fistp ticks
			}
			state->ticks = (short)ticks;
			state->unknown24 = false;
		}
	}
	*flag = false;
	return true;
}

// @retail 0x1ac570
short __stdcall function_1ac570(long actor_index, s_slot *slot, s_slot *next)
{
	short result = g_46fbe8;
	s_slot_2c *state = (s_slot_2c *)slot;

	if (state->unknown12)
	{
		g_46eeb8[0x2b]->unknown8 = g_46f348;
		if (!actor_has_joint_invitation(actor_index, 0x36) && state->unknown10 && state->unknown25 &&
			g_46eeb8[0x2c]->unknown8 != g_46f348 &&
			(g_46eeb8[0x2c]->mask & g_4ee4ec) == g_4ee4ec &&
			TEST_FIELD_BIT(SLOT_TYPE_BITS->type2c))
		{
			*(s_slot_2c *)next = *state;
			state->unknown39 = true;
			result = 0x2c;
		}
		else
		{
			state->unknown39 = false;
			result = g_46fbe4;
		}
	}
	return result;
}

// @retail 0x1acfd0
void __stdcall function_1acfd0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_character_2b *character = (s_character_2b *)function_1e4d10(actor_index);
	s_slot_2c *state = *(s_slot_2c *volatile *)&slot;

	if (actor->prop_index == NONE)
	{
		state->unknown12 = true;
	}
	else
	{
		s_prop_node_view *prop = prop_node_get(actor->prop_index);

		if (state->unknown22 > 0)
			state->unknown22--;
		if (state->unknown10)
		{
			real scale = random_range(1.0f, 3.0f);
			real seconds;
			long ticks;

			if (state->unknown24)
			{
				state->unknown12 = false;
			}
			else
			{
				short remaining = --state->ticks;

				if (function_25d9b0(actor->prop_index))
					state->unknown12 = false;
				else
					state->unknown12 = actor->unknown225 || actor->unknown2d4 >= character->unknown18 && remaining <= 0;
			}
			seconds = g_510c54->field_2_3 * scale;
			__asm
			{
				fld seconds
				fistp ticks
			}
			state->unknown20 = (short)ticks;
		}
		else if (state->unknown20 > 0 && --state->unknown20 == 0)
		{
			function_1fb7e0(0x47, actor_index, NULL, prop->object_index, NONE);
		}
	}
}

// @retail 0x1ad130
void __stdcall function_1ad130(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_1acda0 *state = (s_1acda0 *)slot->unknownc;
	if (actor_get(actor_index)->unknown504 == 2)
	{
		if (state->unknown25)
		{
			if (state->unknown13 && dot3f(&actor->unknown290, &state->facing) > 0.95f)
			{
				short mode = UNIT_MODE(actor->unknown018);
				if (mode == 0 || mode == 7)
				{
					s_unit_request request;
					request.type = 0x24;
					function_210850(&state->point, &request.type25.point);
					union s_1ad133 { dword field_0[3]; vector3f field_00; };
					s_1ad133 local_4;
					local_4.field_00 = state->facing;
					dword local_1 = local_4.field_0[0];
					dword local_2 = local_4.field_0[1];
					dword local_3 = local_4.field_0[2];
					*(dword *)&request.type25.facing.i = local_1;
					request.type25.unknown1c = state->unknown38;
					*(dword *)&request.type25.facing.j = local_2;
					*(dword *)&request.type25.facing.k = local_3;
					if (function_e6900(actor->unknown018, &request))
					{
						real seconds = g_510c54->field_2_3 * 0.5f;
						long ticks;
						__asm
						{
							fld seconds
							fistp ticks
						}
						state->unknown22 = (short)ticks;
						state->unknown3a = true;
					}
					else
						state->unknown25 = false;
				}
			}
		}
		else
			actor->unknown449 = true;
	}
	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);
		s_prop_view_fields *view = prop_node_view(node);
		actor->unknown4a2 = true;
		if (view)
		{
			if (actor_get(actor_index)->unknown504 == 2 && state->unknown13 && state->unknown25)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 4;
				actor->unknown424.vector = state->facing;
				actor->unknown44d = true;
			}
			else if (node->unknown27 >= 1 || 1.0f > (g_510c54->game_time - view->unknown10) * g_510c54->rate)
			{
				actor->unknown488 = true;
				actor->unknown41c = 4;
				actor->unknown420 = 2;
			}
			else if (!view->unknown88 && view->unknown8c <= 0)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 2;
			}
			else if (function_1f86f0(actor_index) && state->unknown13)
			{
				actor->unknown41c = 3;
				actor->unknown420 = 4;
				actor->unknown424.vector = state->facing;
			}
			else
			{
				actor->unknown41c = 2;
				actor->unknown420 = 2;
			}
		}
		else
		{
			actor->unknown41c = 2;
			actor->unknown420 = 2;
		}
		if (actor->unknown229)
			actor->unknown482 = true;
	}
}

// @retail 0x1acd30
bool function_1acd30(long actor_index, s_prop_datum_54 *prop)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;

	if (ACTOR_VIEW_3C(actor)->unknown328 >= 12)
	{
		result = true;
	}
	else if (UNIT_MODE(actor->unknown018) == 5)
	{
		if (2.0f > prop->unknown28)
			result = true;
	}
	else if (prop->unknown27 >= 1)
	{
		result = true;
	}
	return result;
}

// @retail 0x1ad400
void __stdcall function_1ad400(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_2c *state = (s_slot_2c *)slot;

	ACTOR_VIEW_3C(actor)->unknown20c = g_510c54->game_time;
	long unit_index = actor->unknown018;
	short mode = UNIT_MODE(unit_index);
	if (!state->unknown39)
	{
		if (mode == 6 || mode == 7)
			function_e68c0(0x27, unit_index);
		if (!ACTOR_VIEW_3C(actor)->unknown03c && ACTOR_VIEW_3C(actor)->unknown3f4 != NONE)
			function_26def0(actor_index);
	}
}

// @retail 0x1ad4a0
void __stdcall function_1ad4a0(long actor_index, s_slot *slot)
{
	s_slot_2c *state = (s_slot_2c *)slot;

	if (state->unknown25)
		state->unknown25 = false;
}

/* ---- slot type 0x54 ---- */

// @retail 0x1ad4c0
bool function_1ad4c0(long actor_index, long prop_index)
{
	s_actor_view *actor = actor_get(actor_index);
	bool result = false;
	s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);

	if (element)
	{
		s_prop_datum_54 *prop = (s_prop_datum_54 *)prop_node_get(prop_index);
		real range = element->unknown98 > g_45dbd8 ? element->unknown98 : 20.0f;
		result = prop->unknown28 > range;
	}
	return result;
}

// @retail 0x1ad550
short __stdcall function_1ad550(long actor_index)
{
	short result = 0;

	if (actor_get(actor_index)->prop_index != NONE)
		result = 3;
	return result;
}

// @retail 0x1ad590
bool __stdcall function_1ad590(long actor_index, s_slot *slot)
{
	s_slot_54 *state = (s_slot_54 *)slot;

	state->unknown0c = false;
	state->ticks = 0;
	return true;
}

// @retail 0x1ad5b0
short __stdcall function_1ad5b0(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = g_46fbe4;

	if (actor->unknown26c != NONE)
	{
		s_slot_54_data *state = (s_slot_54_data *)slot->unknownc;
		s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
		if (actor->prop_index != NONE && element)
		{
			result = g_46fbe8;
			if (function_1ad4c0(actor_index, actor->prop_index))
				state->ticks++;
			else
				state->ticks = 0;
			if (state->unknown00 && function_1f86f0(actor_index) ||
				(real)state->ticks * g_510c54->rate > element->unknown9c)
			{
				result = 0xe;
			}
		}
	}
	return result;
}

void function_26c180(long actor_index);
real function_30bf0(vector3f *v);
void function_210be0(s_type_c3b527 const *a, s_type_c3b527 const *b, vector3f *out);

// @retail 0x1ada70
void __stdcall function_1ada70(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);

	actor->unknown41c = 2;
	actor->unknown420 = 2;
	actor->unknown488 = true;
	if (((s_slot_54 *)slot)->unknown0c)
	{
		char index = actor->unknown53a;

		if (index < actor->unknown539)
		{
			s_tag_element_54 *element = (s_tag_element_54 *)function_1e5450(actor_index, ai_object_get(actor->unknown26c)->definition_index);
			real distance = 4.0f;
			s_actor_point_entry *entry = &actor->unknown53c[index];
			vector3f direction;

			if (element->unknowna0 > 0.0f)
				distance = element->unknowna0;
			function_26c180(actor_index);
			function_210be0(&actor->unknown27c.point, &entry->point, &direction);
			if (function_30bf0(&direction) > distance && dot3f(&actor->unknown290, &direction) > 0.9f)
				function_1e4290(actor_index, true);
		}
	}
}
/* ---- the handlers ---- */

s_slot_handler_2 g_47dbe8 =
{
	{
		0x2c, 2, 0, -2, 0,
		0, (t_slot_evaluate)function_1ac100, function_1ac090, function_1ac270, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1ac300, function_1ac360, function_1ac380
};

s_slot_handler_2 g_47dc38 =
{
	{
		0x2b, 2, 0, -2, 0,
		function_1ac3f0, (t_slot_evaluate)function_1ac570, function_1ac430, function_1ad400, NONE, {0},
		0, 0, 0, function_1ad4a0, 0, 0, 0
	},
	(t_slot_proc)function_1acda0, function_1acfd0, function_1ad130
};

s_slot_handler_2 g_47dc88 =
{
	{
		0x54, 2, 0, -2, 0,
		function_1ad550, function_1ad5b0, function_1ad590, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1ad6a0, 0, function_1ada70
};


void function_b9fc0(long arg_0, vector3f *arg_1, vector3f *arg_2);
bool function_f47d0(long arg_0, long arg_1);
bool function_1f8660(long arg_0);
// score 7 (was 7): Frame 0x80c, all exits and every instruction but seven agree: the actor and
// scratch-buffer spill homes are exchanged (+0x24/+0x28; retail keeps the actor at +0x28).
// Round 144: the homes follow use weight (heavier is lower): three extra buffer releases put the
// buffer below the actor, one or two do not, nor one actor use fewer. Getter for the entry test with
// the actor declared inside 63; object index read once for both uses 211; reference test reversed
// 137; prop index named for its two uses 13; object index read before the prop tests 235.
// Round 62: the second point difference in its own vector 7 (kept: retail's direction loads in the
// second dot product); a release in each arm 66; one position pointer for both differences 7;
// status output scoped into the request block 11; separate first-call reference 7; early test
// folded into the outer condition (no label) 7.
// Result first 203; request outer scope 187; role names 181; shared point differences 179;
// failure result before release 207; shared cross product 174.
// Count name with shared cross 174; request-output declarations first 174;
// outer vector workspaces 174; separate direction vectors 171; request defaults with overrides 186.
// unsigned-byte result 224; direct shared reference equality 174; direct slot observations 174; separate selected alignment 174; range selection arms 174.
// Short/native-long success 232/228; unsigned selection count 174; native zero/kind request
// initialization 172/173; status-output name 174; const tag input 174; request through validation 174;
// success from availability 217; direct exits with failure retained through release 232.
// Round 358: Entry selection outputs 174; complete vector role names with cross output 174; failure-default
// success paths 215; combined work guard 174; positive tag work 174. Actor/result/count and direction homes
// remain in frame 0x80c; opening body restored.
// Round 195: complete k-j-i projections/status 174/174; both shared differences 172/172;
// every component product/status 173/173; both shared differences 172/172. Gains of one/two restored.
// Round 197: read-only actor/state/both views 174/174/174; distinct selection/validation
// reference lifetimes alone/actor/state/both 174/174/174/174; actor lookup at every use 230.
// Round 204: shared release 204; native reference-produced result 226; absent-tag return
// alone/reference result 226/220; skip return alone/reference result 223/229; both early
// exits alone/reference result 221/222. Actor/result/count and direction homes remain.
// Round 323: vehicle field at each use 174; radius read once 191; shared projections
// 194; vehicle/radius 191; vehicle/projections 194; radius/projections 213; all 213; radius
// and projections with native vehicle index 213. Original actor/result/count and direction homes remain.
// Round 168: Shared complete position with original/split directions and actor/count role names 170
// each; with entry result 193 each; three scalar coordinates original/split 182/185. Four-point
// best rejected; snapshot frame 0x81c; opening body restored.
// Round 199: Count/status scope 175; complete selected-point copy 182; both 180;
// reference before selected output 174; scope/reference 175; point/reference 182;
// all three 180; scoped count/status role names 175. Original output lifetimes restored.
// Round 60 (kept): count output scoped to the request block 173; failure result stored through the
// flag with one shared release 203; the rest nested under the tag test 95; dot3f for the three dot
// products 44; vector3d_from_points3d for both differences 34. Not kept: count and status both
// scoped 175; names sorting the buffer first 173; buffer declared at entry/after the actor 95/95;
// dot3f argument orders (each, pairs, all) 34-44 unchanged; direction declared first 44; dot3f
// tested directly 44; actor getter at the object/vehicle/second-difference use 60/135/238;
// result first 68; slot first 34.
// Round 49: const actor/buffer/both pointers 7/7/7; request/validation/release buffer aliases
// 7/7/7; first/second position pointers 7/7. Starting body restored.
// Round 16: the two homes are frame-relative +0x1c (actor) and +0x18 (buffer) in retail, the other
// way round here; the request sits at +0xb4 in both. The buffer declared at function scope before
// or after the actor (with 0, with no initializer, with a dummy initializer) 7/7/7/7; the buffer
// declared after the entry/status/reference outputs 7/7/7; the actor typed as a long address 7;
// redundant extra buffer uses (self-assign, self-compare) 7/7/7; the release written in both arms
// 66; the actor declared inside the flag test with the getter in the test 63; the actor in a nested
// block with the buffer outside 52; the actor looked up at every use 243; the slot and entry-guard
// conditions written as nested ifs 7/7. Only the actor and buffer homes remain exchanged; the
// opening body is restored.
// Round 206: complete position after/before orientation 143/150; position plus post-clear buffer 215;
// buffer after request clear/constants 198/198; each with real reference-field equality 224/224;
// renewed actor view for the complete direction phase 223. Actor/buffer homes remain exchanged.
// Round 69: actor named to sort after the buffer 7 (names do not order these homes); failure flag
// after the actor store 7; prop test as its own if 7; third vector declared before the call 7;
// untyped buffer pointer 7.
// Round 41: entry flag test through a fresh actor lookup 33; buffer cleared after its release 7;
// request outputs declared before the buffer 7; request before the count 7; reference output first 7.
// Round 53: copied later actor view 7; actor copy with slot data view 7; actor copy, slot data view
// and flattened request scope 34; slot data view alone 7; state scoped to the outer test 7;
// flattened request scope alone 34; actor declaration split from its lookup 52; buffer declaration
// split from acquisition 7. Actor/buffer homes remain exchanged; the opening body is restored.
// Round 11 (lane 17): REFERENCE_EQUAL test with the failure store first and a `selected` copy for the
// second call 7; radius if/else 7, default-then-override 43, `<= 0.0f` ternary 37; slot data view from
// arg_1->unknownc in place/declared first 7/7; that view plus one position pointer 7; inverted tag test 188;
// renames 7. Extra buffer releases before the first call or three after it give retail's homes but other
// code (156-206); fresh actor lookups at the 270/26c/position reads keep the homes (140-235).
// @retail 0x1ad6a0
bool __stdcall function_1ad6a0(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	bool local_1 = true;
	s_slot_54 *local_2 = (s_slot_54 *)arg_1;
	if (local_0->unknown040)
	{
		if (local_2->unknown0c && function_1f8660(arg_0))
			goto done;
		s_tag_element_54 *local_3 = (s_tag_element_54 *)function_1e5450(arg_0,
			ai_object_get(local_0->unknown26c)->definition_index);
		if (local_3)
		{
			byte *local_4 = ai_scratch_buffer_get();
			s_261d20_entry local_7;
			bool local_9;
			s_reference local_10;
			{
				long local_8;
				s_2605d0_request local_5;
				memset(&local_5, 0, sizeof(local_5));
				*(real *)((byte *)&local_5 + 0x60) = 15.0f;
				local_5.type = 7;
				local_5.unknown015 = true;
				local_5.unknown05b = true;
				real local_6 = local_3->unknown98 > 0.0f ? local_3->unknown98 : 20.0f;
				local_5.unknown008 = local_6 * 0.5f;
				local_5.unknown00c = local_6;
				local_5.unknown010 = local_6 + 10.0f;
				local_10 = function_2605d0(arg_0, &local_5, (long)&local_7,
					(long)&local_8, local_4, &local_9);
				local_10 = function_2626b0(arg_0, local_10, local_8, local_4, local_9, true);
			}
			s_reference local_11 = g_470fa0;
			if (*(long *)&local_10 == *(long *)&local_11)
			{
				local_1 = false;
				actor_get(arg_0)->unknown040 = local_1;
			}
			else
			{
				if (!local_2->unknown0c && local_0->unknown270 == 4 && local_0->prop_index != NONE)
				{
					s_prop_node_view *local_12 = prop_node_get(local_0->prop_index);
					s_prop_state_view *local_13 = prop_node_state(local_12);
					if (local_12->unknown24 >= 1 && local_12->unknown24 <= 2 && *(bool *)((byte *)local_13 + 0x64))
					{
						long local_14 = local_0->unknown26c;
						vector3f local_15;
						vector3f local_16;
						function_b9fc0(local_14, &local_15, &local_16);
						vector3f local_17;
						vector3d_from_points3d(&local_0->position, &local_13->position, &local_17);
						function_30bf0(&local_17);
						real local_18 = dot3f(&local_15, &local_17);
						if (local_18 > 0.9f)
						{
							vector3f local_21;
							vector3d_from_points3d(&local_0->position, &local_7.point, &local_21);
							real local_19 = function_30bf0(&local_21);
							local_18 = dot3f(&local_15, &local_21);
							if (local_18 > 0.707106769f)
							{
								vector3f side;
								cross3f(&local_16, &local_15, &side);
								real local_20 = dot3f(&side, &local_21);
								local_20 *= local_19;
								if (local_20 > 1.0f)
									function_f47d0(local_14, 1);
								else if (-1.0f > local_20)
									function_f47d0(local_14, 2);
							}
						}
					}
				}
				local_2->unknown0c = true;
			}
			ai_scratch_buffer_release(local_4);
		}
		else
			local_1 = false;
	}
done:
	return local_1;
}

#include "object_markers.h"

struct s_record_motion_view;
point3f *function_b9dd0(long arg_0, point3f *arg_1);
short __stdcall function_bb050(long arg_0, dword arg_1, void const *arg_2,
	point3f const *arg_3, real arg_4, long *arg_5, short arg_6);
bool function_26df40(long arg_0, long arg_1, bool arg_2);
short __stdcall function_26e090(long arg_0, s_object_marker *arg_1, short *arg_2, short arg_3);
long function_26db50(long arg_0, short arg_1);
long function_26d500(long arg_0);
void function_26dbd0(long arg_0, long arg_1);
void function_26dc20(long arg_0);
bool function_26e180(long arg_0, s_record_motion_view const *arg_1, short arg_2,
	point3f *arg_3, s_type_c3b527 *arg_4, long *arg_5);
bool function_26d9c0(long arg_0, s_type_c3b527 const *arg_1, short arg_2,
	short arg_3, long arg_4, vector3f const *arg_5);

struct s_1ac610
{
	byte field_0[0x5c];
	long field_5c;
	dword *field_60;
};

struct s_1ac611
{
	byte field_0[0xc4];
	long field_c4;
	void *field_c8;
};

struct s_1ac612
{
	byte field_0[0x40];
	dword field_40[1];
	byte field_44[0x484 - 0x44];
};

// score 29 (was 29): frame 0x8c agrees; the actor pointer and actor offset swap EDI/EBP (so the
// candidate takes EDI), their spill homes swap with the selected index (+0x1c/+0x20), the best
// value/index initialisations sit before the zero-count branch, one product loads its operands the
// other way and the up.j load runs before the up-vector address.
// Round 49: result declared first 30; global factor first in the third product 29; best index and
// value before the query 258 (983 bytes); one-expression distance 137; named unit index 29;
// candidate comparison reversed, new actor local for the cleanup and one-expression up product
// 29/29/128 (screened); 36 combinations of these, none below 29.
// Earlier: separate cleanup receiver 29; success return 74; shared displacement 29; scan scope
// 44; unsigned candidate counter 227; unsigned marker counter 153; const source/marker 29;
// shared offset 29; lifetime names 29; actor parameter name 29; unsigned handles 29; location
// memcpy 58; distance scope 29; continue guards 29; joint vector helpers 41; result declared
// first 30; best index named before actor home 29; source actor named after index home 29;
// best index and source actor named homes 29; candidate/marker difference shared helpers 29;
// forward product shared helper 135; up product shared helper 41; native source location 58;
// query outputs at loop/entry and source receiver at entry 29; marker reference, readonly
// candidate tag, distance predicate and aligned up-product scope 29; positive-count scope 254;
// up-vector private pointer 41; actor-input private reference 137.
// actor offset/candidate registers and two spill homes differ;
// separate cleanup receiver 29; success return 74; shared displacement 29; scan scope 44;
// unsigned candidate counter 227; unsigned marker counter 153; const source/marker 29/29.
// actor/candidate registers and two spill homes differ; shared offset 29, lifetime names 29, actor parameter name 29, unsigned handles 29, location memcpy 58, distance scope 29, continue guards 29, joint vector helpers 41 (round 115).
// frame 0x8c agrees; EBP/EDI, two spill homes and product/address scheduling still differ;
// tried result declared first 30; best index named before actor home 29;
// source actor named after index home 29; best index and source actor named homes 29;
// candidate difference shared helper 29; marker difference shared helper 29;
// forward product shared helper 135; up product shared helper 41. Restored.
// Earlier: frame 0x8c agrees; EBP/EDI and offset/selection spill homes remain exchanged.
// Native source location 58; query outputs at loop/entry and source receiver at entry 29 each;
// marker reference/readonly candidate tag/distance predicate/aligned up-product scope 29 each.
// All executable forms and the experimental include restored.
// Earlier: actor-offset/selection registers and spill homes still differ.
// Candidate cursor 244; marker cursor 234; both cursors 249; distance first 29;
// distance across queries 266; point declaration first 29; selection update first 30;
// short ordinal entry scopes 29. Restored.
// Round 41: candidate read from the query array at each use 58; forward test through the existing
// 0x26f8f0 helper (retail inlines it here; its /Ob1 file keeps it out of line even with an inline or
// __forceinline declaration) 206/206/206; best-index renames (best_index, winner, zz_index) 29;
// forward product in the helper's operand order 137; up product after the forward test 29, both 137.
// Round 41 (lane 10): the closing cleanup is the body of the existing 0x26def0 with a NONE owner
// (retail calls 0x26def0 out of line at 0x1ac270/0x1ad400 but has this body in line here); the call
// itself stays out of line 301 (918 bytes); the body as a file-local forceinline helper 29, as a
// plain inline helper 29; through actor_datum_get with the location-record field names 29; best
// index at function scope with the selection block after the first one 273 (984 bytes).
// Round 72 (lane 9, fast): the forward product computed before the marker difference 29; the up axis
// global read into a pointer local used for all three terms 58; the marker difference through the
// existing vector3d_from_points3d 29; best index/value declared before the query and assigned after it 29,
// the index declared at entry 29. Diagnostic: actor_get as offset-first sum or array element 29/29
// (header restored), so the EBP/EDI swap is not the accessor's spelling.
// @retail 0x1ac610
bool __stdcall function_1ac610(long arg_0, point3f const *arg_1,
	s_object_marker *arg_2, short *arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	bool local_1 = false;
	if (local_0->unknown018 != NONE)
	{
		s_ai_object *local_2 = ai_object_get(local_0->unknown018);
		dword local_3[2];
		local_3[0] = *(dword *)((byte *)local_2 + 0x28);
		local_3[1] = *(dword *)((byte *)local_2 + 0x2c);
		point3f local_4;
		function_b9dd0(local_0->unknown018, &local_4);
		long local_5[20];
		short local_6 = function_bb050(1, 0x8c3, local_3, &local_4, 4.0f, local_5, 20);
		long local_7 = NONE;
		real local_8 = 3.402823466e38f;
		for (short local_9 = 0; local_9 < local_6; ++local_9)
		{
			long local_10 = local_5[local_9];
			if (local_10 != local_0->unknown018)
			{
				s_ai_object *local_11 = ai_object_get(local_10);
				s_1ac610 *local_12 = (s_1ac610 *)g_4e3b44[local_11->definition_index & 0xffff].bytes;
				if (local_12->field_5c > 0 && (*local_12->field_60 & 4) && function_26df40(arg_0, local_10, false))
				{
					vector3f local_13;
					local_13.i = local_11->position.x - local_2->position.x;
					local_13.j = local_11->position.y - local_2->position.y;
					local_13.k = local_11->position.z - local_2->position.z;
					real local_14 = local_13.k * local_13.k;
					local_14 += local_13.j * local_13.j;
					local_14 += local_13.i * local_13.i;
					if (local_8 > local_14)
					{
						local_8 = local_14;
						local_7 = local_10;
					}
				}
			}
		}
		if (local_7 != NONE)
		{
			short local_15 = function_26e090(local_7, arg_2, arg_3, 32);
			if (local_15 > 0 && ((s_1ac611 *)g_4e0348)->field_c4 > 0 && ((s_1ac611 *)g_4e0348)->field_c8)
			{
				long local_16 = NONE;
				s_1ac612 *local_17 = NULL;
				for (short local_18 = 0; local_18 < local_15; ++local_18)
				{
					s_object_marker *local_19 = &arg_2[local_18];
					vector3f local_20;
					local_20.i = arg_1->x - local_19->matrix.position.x;
					local_20.j = arg_1->y - local_19->matrix.position.y;
					local_20.k = arg_1->z - local_19->matrix.position.z;
					real local_21 = local_19->matrix.forward.k * g_4687b0->k;
					local_21 += local_19->matrix.forward.j * g_4687b0->j;
					local_21 += local_19->matrix.forward.i * g_4687b0->i;
					real local_22 = local_19->matrix.up.k * local_20.k;
					local_22 += local_19->matrix.up.j * local_20.j;
					local_22 += local_19->matrix.up.i * local_20.i;
					if (local_21 > 0.95f && 0.0f > local_22)
					{
						if (local_16 == NONE)
						{
							local_16 = function_26db50(local_7, 1);
							if (local_16 == NONE)
								local_16 = function_26d500(local_7);
							if (local_16 == NONE)
								break;
							function_26def0(arg_0, local_7);
							function_26dbd0(local_16, arg_0);
							local_17 = (s_1ac612 *)g_51eca4->data + (local_16 & 0xffff);
							local_1 = true;
						}
						if (local_16 != NONE && !(local_17->field_40[local_18 >> 5] & (1UL << (local_18 & 31))))
						{
							s_type_c3b527 local_23;
							long local_24;
							if (function_26e180(local_16, (s_record_motion_view const *)local_19,
								arg_3[local_18], NULL, &local_23, &local_24) && local_24 != NONE && local_24 != 0xffff)
								function_26d9c0(local_16, &local_23, local_18, arg_3[local_18], local_24, &local_19->matrix.up);
						}
					}
				}
			}
		}
	}
	if (!local_1)
	{
		local_0 = actor_get(arg_0);
		if (*(long *)((byte *)local_0 + 0x3f8) != NONE)
		{
			if (*(long *)((byte *)local_0 + 0x3f4) != NONE)
				function_26dc20(arg_0);
			*(long *)((byte *)local_0 + 0x3f8) = NONE;
			*(short *)((byte *)local_0 + 0x3fc) = 0;
		}
	}
	return local_1;
}


struct s_1f4a20_entry;
struct s_1f4a20_source;
bool function_1f4a20(s_1f4a20_entry const *arg_2, long arg_0, s_reference arg_1, s_1f4a20_source const *arg_8,
	bool arg_3, s_type_c3b527 *arg_4, vector3f *arg_5,
	signed char *arg_6, bool *arg_7);

struct s_1ac9e1
{
	byte field_0;
	bool field_1;
	byte field_2[5];
	bool field_7;
	vector3f field_8;
	byte field_14[5];
	bool field_19;
	byte field_1a[2];
	s_type_c3b527 field_1c;
	signed char field_2c;
	byte field_2d;
	bool field_2e;
};

// @retail 0x1ac9e0
bool function_1ac9e0(long arg_0, s_reference arg_1, s_1f4a20_entry const *arg_2,
	byte *arg_4, bool arg_5, long arg_3, s_1f4a20_source const *arg_6, s_1ac9e1 *arg_7)
{
	s_actor_view *local_1 = actor_get(arg_0);
	bool result = false;
	bool local_0 = false;
	arg_7->field_7 = false;
	s_262b40_result *local_2 = function_262b40(arg_1);
	if (*(short const *)((byte const *)arg_2 + 0x5c) != 0 &&
		*(long *)((byte *)local_2 + 0x14) != NONE &&
		*(long *)((byte *)local_2 + 0x14) != 0xffff)
	{
		result = function_1f4a20(arg_2, arg_0, arg_1, arg_6, false, &arg_7->field_1c,
			&arg_7->field_8, &arg_7->field_2c, &local_0);
		if (result)
		{
			arg_7->field_19 = true;
			arg_7->field_2e = false;
			arg_7->field_7 = true;
			arg_7->field_1 = false;
			goto done;
		}
		if (local_0)
			goto done;
	}
	{
		s_reference local_3 = function_2626b0(arg_0, arg_1, arg_3, arg_4, arg_5, true);
		arg_7->field_1 = false;
		s_reference local_4 = g_470fa0;
		if (*(long *)&local_3 == *(long *)&local_4)
		{
			actor_get(arg_0)->unknown040 = false;
			result = false;
			goto done;
		}
		arg_7->field_19 = false;
		result = true;
		if (local_1->unknown539 > 0)
		{
			function_26c180(arg_0);
			s_type_c3b527 const *local_6 = (s_type_c3b527 const *)((byte *)local_1 +
				0x52c + local_1->unknown539 * 0x1c);
			s_type_c3b527 const *local_7;
			if (local_1->unknown539 > 1)
				local_7 = (s_type_c3b527 const *)((byte *)local_1 +
					0x510 + local_1->unknown539 * 0x1c);
			else
				local_7 = (s_type_c3b527 const *)((byte *)local_1 + 0x27c);
			if (local_6 && local_7)
			{
				function_210be0(local_6, local_7, &arg_7->field_8);
				arg_7->field_8.k = 0.0f;
				arg_7->field_7 = function_30bf0(&arg_7->field_8) > g_45dbd8;
			}
		}
	}
done:
	return result;
}


union s_1acb51
{
	short field_0[32];
	s_261d20_entry field_00;
};

// @retail 0x1acb50
bool __stdcall function_1acb50(long arg_0, s_slot *arg_1)
{
	bool local_16 = false;
	s_actor_view *local_0 = actor_get(arg_0);
	s_type_5cfb45 *local_1 = function_25d690((s_prop_datum *)prop_node_get(local_0->prop_index));
	bool local_2 = false;
	*(bool *)((byte *)arg_1 + 4) = false;
	((s_1ac9e1 *)arg_1)->field_19 = false;
	long local_3 = local_0->unknown018;
	short local_4 = UNIT_MODE(local_3);
	if (local_4 != 7)
	{
		if (local_4 == 6)
			function_e68c0(0x27, local_3);
		if (!REFERENCE_EQUAL(local_0->unknown418, g_470fa0))
			function_262800(arg_0, local_0->unknown418, false);
	}
	s_object_marker local_6[32];
	s_1acb51 local_7;
	if (!ACTOR_VIEW_3C(local_0)->unknown03c)
		local_2 = function_1ac610(arg_0, (point3f const *)((byte *)local_1 + 4), local_6, local_7.field_0);
	s_2605d0_request local_8;
	memset(&local_8, 0, sizeof(local_8));
	*(volatile bool *)((byte *)&local_8 + 0x46) = true;
	*(volatile bool *)&local_8.unknown015 = true;
	*(volatile bool *)((byte *)&local_8 + 0x59) = true;
	s_object_marker *local_9 = local_2 ? local_6 : NULL;
	*(real *)((byte *)&local_8 + 0x48) = 10.0f;
	*(volatile short *)&local_8.type = 2;
	*(volatile real *)((byte *)&local_8 + 0x4c) = 3.0f;
	*(s_object_marker *volatile *)((byte *)&local_8 + 0x64) = local_9;
	byte *local_10 = ai_scratch_buffer_get();

	long local_12;
	bool local_13;
	s_reference local_14 = function_2605d0(arg_0, &local_8, (long)&local_7.field_00,
		(long)&local_12, local_10, &local_13);
	s_reference local_15 = g_470fa0;
	if (*(long *)&local_14 == *(long *)&local_15)
	{
		local_16 = false;
		actor_get(arg_0)->unknown040 = local_16;
	}
	else
		local_16 = function_1ac9e0(arg_0, local_14, (s_1f4a20_entry const *)&local_7.field_00,
			local_10, local_13, local_12, (s_1f4a20_source const *)local_9, (s_1ac9e1 *)arg_1);
	ai_scratch_buffer_release(local_10);
	return local_16;
}

#include "unknown_0259a0.h"
bool function_2613d0(long arg_0, s_reference arg_1, s_prop_search *arg_2);
bool __stdcall function_1acb50(long arg_0, s_slot *arg_1);

// @retail 0x1acda0
bool __stdcall function_1acda0(long arg_0, s_slot *arg_1)
{
    s_actor_view *local_0 = (s_actor_view *)(g_4f55f0->data + (arg_0 & 0xffff) * sizeof(s_actor_view));
    // the slot is read once at entry, as 0x1acfd0 reads its own
    s_1acda0 *local_1 = (s_1acda0 *)((byte *)*(s_slot *volatile *)&arg_1 + sizeof(s_slot_header));
    volatile bool local_2 = true;
    if (local_0->unknown007)
        return false;
    if (local_1->unknown12)
    {
        local_2 = false;
        return local_2;
    }
    s_prop_node_view *local_3 = NULL;
    if (local_0->prop_index != NONE)
        local_3 = (s_prop_node_view *)(g_502418->data + (local_0->prop_index & 0xffff) * sizeof(s_prop_node_view));
    if (local_1->unknown22 > 0 || !local_1->unknown0d && function_1f86f0(arg_0) && !local_1->unknown10)
        function_265bb0(arg_0);
    long unit_index;
    if (REFERENCE_EQUAL(local_0->unknown418, g_470fa0) ||
        local_1->unknown10 && local_3 && local_1->unknown22 == 0 &&
        (unit_index = local_0->unknown018, !function_110ab0(unit_index)) && function_1acd30(arg_0, (s_prop_datum_54 *)local_3))
    {
        local_1->unknown0d = true;
        local_1->unknown10 = false;
    }
    if (!local_1->unknown0d)
    {
        bool active = ((s_actor_view *)g_4f55f0->data)[arg_0 & 0xffff].unknown504 == 2;
        if (active)
        {
            if (!local_1->unknown10 &&
                (!local_1->unknown25 || local_1->unknown3a && (unit_index = local_0->unknown018, !function_110ab0(unit_index))))
            {
                if (local_3)
                    function_1fb7e0(0x8a, arg_0, NULL, local_3->object_index, NONE);
                if (!local_1->unknown3b)
                {
                    function_e68c0(0, local_0->unknown018);
                    local_1->unknown3b = true;
                }
                local_1->unknown10 = true;
            }
        }
        else
            local_1->unknown10 = active;
    }
    if (local_0->unknown040)
    {
        if (local_1->unknown0d)
        {
            bool child_result = function_1acb50(arg_0, (s_slot *)local_1);
            local_2 = child_result;
            return child_result;
        }
        if (function_1f8660(arg_0) && !REFERENCE_EQUAL(local_0->unknown418, g_470fa0))
        {
            s_prop_search local_4;
            memset(&local_4, 0, sizeof(local_4));
            local_4.type = 2;
            *(bool *)((byte *)&local_4 + 0x46) = true;
            *(real *)((byte *)&local_4 + 0x48) = 10.0f;
            *(real *)((byte *)&local_4 + 0x4c) = 3.0f;
            *(bool *)((byte *)&local_4 + 0x15) = true;
            local_4.unknown59 = true;
            if (!function_2613d0(arg_0, local_0->unknown418, &local_4))
                local_1->unknown0d = true;
        }
    }
    return local_2;
}
