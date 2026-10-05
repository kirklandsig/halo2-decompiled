// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_25D690.CPP: the ai's props: the "prop", "prop_ref"
   and "tracking" data arrays (include/props.h) */

#include "unknown_11c920.h"
#include "globals.h"
#include "props.h"
#include "unknown_26b230.h"
#include "unknown_1e46c0.h"
#include <string.h>

s_prop_type_entry g_470f10[9] =
{
	{0, 0, -1, -1, -1, {0, 0, 0}},
	{1, 5, 2, 2, 2, {0, 0, 0}},
	{2, 2, 2, 1, 0, {0, 0, 0}},
	{3, 5, 2, 1, 0, {0, 0, 0}},
	{4, 1, 1, 0, 0, {0, 0, 0}},
	{5, 1, 1, 0, 0, {0, 0, 0}},
	{6, 1, 1, 0, 0, {0, 0, 0}},
	{7, 1, 1, 0, 0, {0, 0, 0}},
	{8, 1, 1, 0, 0, {0, 0, 0}},
};

/* the actor types: a name, then the type that leads this one */
struct s_actor_type_definition
{
	char const *name;
	short unknown4;
	short leader_type;
};

s_actor_type_definition g_471010 = {"flood carrier", 0, NONE};
s_actor_type_definition g_471018 = {"crew", 0, NONE};
s_actor_type_definition g_471020 = {"elite", 0, NONE};
s_actor_type_definition g_471028 = {"engineer", 0, NONE};
s_actor_type_definition g_471030 = {"flood", 0, NONE};
s_actor_type_definition g_471038 = {"grunt", 0, 0};
s_actor_type_definition g_471040 = {"hunter", 0, NONE};
s_actor_type_definition g_471048 = {"infection", 1, NONE};
s_actor_type_definition g_471050 = {"jackal", 0, NONE};
s_actor_type_definition g_471058 = {"marine", 0, NONE};
s_actor_type_definition g_471060 = {"mounted_weapon", 0, NONE};
s_actor_type_definition g_471068 = {"prophet", 0, NONE};
s_actor_type_definition g_471070 = {"bugger", 0, NONE};
s_actor_type_definition g_471078 = {"sentinel", 0, NONE};
s_actor_type_definition g_471080 = {"juggernaut", 0, NONE};

s_actor_type_definition *g_471088[16] =
{
	&g_471020, &g_471050, &g_471038, &g_471040, &g_471028, &g_471020, &g_471058, &g_471058,
	&g_471018, &g_471030, &g_471048, &g_471010, &g_471078, &g_471078, &g_471038, &g_471060
};

bool function_1a8220(long index, short a, short b, long unknown, short c, short d, short e);
void __stdcall function_25d020(long actor_index, long prop_ref_index, long a, long b, long c, long d);
bool function_1fb7e0(long actor_index, short type, s_1fb7e0_data const *data, long target_index, long unknown);
bool function_26ba60(long prop_index, long actor_index, long clump_index);
void function_25c780(long actor_index, long prop_ref_index);
void function_25c4e0(long prop_ref_index);
real function_30bf0(vector3f *v);
point3f *function_b9dd0(long object_index, point3f *result);

/* the clumps of g_502420 (0x50 bytes) as the props see them */
struct s_clump_prop_view
{
	byte unknown00[0x30];
	bool unknown30;
	byte unknown31[0x50 - 0x31];
};

/* the view of a prop_ref's tracking, if it has one (function_25d740 inlined) */
inline s_type_f95cd3 *prop_ref_view(s_prop_datum *datum)
{
	s_type_f95cd3 *result = NULL;
	if (datum->tracking_index != NONE)
	{
		s_type_e5ff81 *tracking = tracking_get(datum->tracking_index);
		if (tracking)
		{
			result = &tracking->view;
		}
	}
	return result;
}

/* clears what a prop view remembers of its last update */
inline void prop_view_reset(s_type_f95cd3 *view)
{
	view->unknown68 = false;
	view->unknown69 = false;
	view->unknown4c = false;
	view->unknown70 = 0;
	view->unknown6c = true;
	view->unknown6d = true;
	view->unknown88 = false;
	view->unknown90 = 0;
	view->unknown8a = 0;
}

// @retail 0x25ac00
void function_25ac00(long prop_ref_index, long actor_index)
{
	if (actor_get(actor_index)->unknown018 != NONE)
	{
		s_prop_datum *datum = prop_ref_get(prop_ref_index);
		s_type_76cf92 *prop = prop_get(datum->prop_index);

		if (!function_25d690(datum)->unknown5e && prop->unknown24 && prop->unknown3c)
		{
			s_type_f95cd3 *view = function_25d740((s_prop_node *)datum);
			real range;

			if (prop->unknown23)
			{
				range = 15.f;
			}
			else if (view && view->unknown39 <= 2)
			{
				range = 10.f;
			}
			else
			{
				range = 3.f;
			}

			bool close = range > datum->unknown28;
			if ((prop->unknown23 && view && view->unknown2a) || close)
			{
				function_1fb7e0(actor_index, 0xc2, NULL, datum->object_index, NONE);
				prop->unknown3c = false;
			}
		}
	}
}

// @retail 0x25bba0
void function_25bba0(long actor_index, long unknown)
{
	s_actor_prop_view *actor = actor_prop_view_get(actor_index);

	if (actor->unknown684 <= 0)
	{
		real ticks = g_510c54->field_2_3 * 1.5f;
		long rounded;
		__asm
		{
			fld ticks
			fistp rounded
		}
		actor->unknown686 = (short)rounded;
		actor->unknown688 = 1;
		actor->unknown68c = unknown;
		actor->unknown684 = 0;
	}
}

// @retail 0x25bc20
void function_25bc20(long actor_index, long unknown)
{
	if (actor_prop_view_get(actor_index)->unknown684 <= 0)
	{
		real ticks = g_510c54->field_2_3 * 4.f;
		long rounded;
		__asm
		{
			fld ticks
			fistp rounded
		}
		actor_prop_view_get(actor_index)->unknown686 = (short)rounded;
		actor_prop_view_get(actor_index)->unknown688 = 6;
		actor_prop_view_get(actor_index)->unknown68c = unknown;
		actor_prop_view_get(actor_index)->unknown684 = 0;
	}
}

// @retail 0x25bcb0
void function_25bcb0(long actor_index, long prop_ref_index)
{
	function_25d020(actor_index, prop_ref_index, 2, NONE, NONE, NONE);
	actor_get(actor_index)->unknown223 = false;
}

// @retail 0x25bf10
void function_25bf10(long actor_index, long other_index, long prop_ref_index)
{
	s_actor_view *actor = actor_get(actor_index);
	s_actor_view *other = actor_get(other_index);
	bool friendly;

	if (prop_ref_index != NONE)
	{
		friendly = prop_get(prop_ref_get(prop_ref_index)->prop_index)->unknown23;
	}
	else
	{
		friendly = function_1df560(actor->unknown024, other->unknown024);
	}

	if (friendly)
	{
		return;
	}

	vector3f delta;

	vector3d_from_points3d(&other->position, &actor->position, &delta);
	if (length_sq3f(&delta) < 64.f)
	{
		if (!other)
		{
			return;
		}
		if (other->unknown004 == g_471088[actor->unknown004]->leader_type)
		{
			function_1a8220(actor_index, 0x3e, 1, 3, 1, 0x2f, 3);
		}
		else if (other->unknown004 == actor->unknown004)
		{
			function_1a8220(actor_index, 0x3f, 1, 3, 1, 0x2f, 3);
		}
	}

	if (other && other->unknown004 == actor->unknown004)
	{
		function_1a8220(actor_index, 0x18, 1, 3, 1, NONE, 0);
	}
}

// @retail 0x25c170
void function_25c170(void)
{
	g_50241c = data_new_inlined("prop", 0x100, sizeof(s_type_76cf92), 0, g_510c2c);
	g_502418 = data_new_inlined("prop_ref", 0x400, sizeof(s_prop_datum), 0, g_510c2c);
	g_502414 = data_new_inlined("tracking", 0x64, sizeof(s_type_e5ff81), 0, g_510c2c);
}

// @retail 0x25c3e0
void prop_state_initialize(s_type_5cfb45 *state)
{
	state->unknown40 = NONE;
	state->unknown66 = false;
	state->unknown62 = false;
	state->unknown5e = false;
	state->unknown64 = false;
	state->unknown61 = false;
	state->unknown5f = false;
	state->unknown00 = NONE;
	state->unknown60 = true;
	state->unknown63 = false;
	state->unknown69 = false;
	state->unknown65 = false;
	state->unknown3c = NONE;
	state->unknown67 = false;
	state->unknown48 = *g_468788;
	state->unknown54 = NONE;
	state->unknown44 = NONE;
	state->unknown58 = false;
}

// @retail 0x25c440
void prop_view_initialize(s_type_f95cd3 *view)
{
	view->unknown06 = 4;
	view->unknown8c = 4;
	view->unknown10 = NONE;
	view->unknown14 = NONE;
	view->unknown50 = NONE;
	view->unknown8a = 0;
	view->unknown54 = 0.f;
	view->unknown58 = 0.f;
	view->unknown5c = 0.f;
	view->unknown60 = 0.f;
	view->unknowna2 = false;
	view->unknown94 = *g_4687a4;
	view->unknownb0 = NONE;
	view->unknown64 = false;
	view->unknown66 = 0;
	prop_view_reset(view);
}

// @retail 0x25c4e0
void function_25c4e0(long prop_ref_index)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);
	s_type_f95cd3 *view = prop_ref_view(datum);

	if (view)
	{
		prop_view_reset(view);
	}

	s_type_76cf92 *prop = prop_get(datum->prop_index);
	prop->unknown32 = false;
	prop->unknown33 = false;
	prop->unknown34 = false;
}

// @retail 0x25c780
void function_25c780(long actor_index, long prop_ref_index)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);
	record_pool_release(g_502414, datum->tracking_index);
	datum->tracking_index = NONE;

	s_actor_prop_view *actor = actor_prop_view_get(actor_index);
	for (short i = 0; i < 8; i++)
	{
		if (actor->tracked_prop_indices[i] == prop_ref_index)
		{
			actor->tracked_prop_indices[i] = NONE;
			break;
		}
	}

	if (g_470f10[datum->type].unknown8 == 2)
	{
		datum->state = 0;
	}
	else if (datum->state >= 3 || datum->state == 2)
	{
		datum->state = 1;
	}
}

// @retail 0x25c820
void function_25c820(long prop_ref_index, long actor_index)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);

	if (datum->tracking_index != NONE)
	{
		function_25c780(actor_index, prop_ref_index);
	}
	datum->state = 0;
	datum->unknown10 = 0.f;
}

// @retail 0x25cca0
short function_25cca0(long prop_ref_index)
{
	short result = 0;
	if (prop_ref_get(prop_ref_index)->unknown1c == 3)
	{
		result = 3;
	}
	return result;
}

// @retail 0x25d420
void function_25d420(long prop_ref_index, short type, long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);

	if (prop_ref_index != NONE)
	{
		s_prop_datum *datum = prop_ref_get(prop_ref_index);
		s_type_f95cd3 *view = function_25d700(prop_ref_index);

		if (view)
		{
			view->unknown69 = true;
			if (type == 3)
			{
				view->unknown4c = true;
			}

			if (function_26ba60(datum->prop_index, actor_index, actor->unknown07c))
			{
				bool started;

				switch (type)
				{
				case 1:
					started = function_1fb7e0(actor_index, 0x33, NULL, datum->object_index, NONE);
					break;
				case 2:
					started = function_1fb7e0(actor_index, 0x32, NULL, datum->object_index, NONE);
					break;
				case 3:
					started = function_1fb7e0(actor_index, 0x35, NULL, datum->object_index, NONE) ||
						function_1fb7e0(actor_index, 0x3b, NULL, datum->object_index, NONE);
					break;
				default:
					return;
				}

				if (started)
				{
					prop_get(datum->prop_index)->unknown34 = true;
				}
			}
		}
	}
}

// @retail 0x25d510
void function_25d510(long actor_index)
{
	long prop_ref_index = actor_get(actor_index)->first_prop_index;

	for (;;)
	{
		if (prop_ref_index == NONE)
		{
			break;
		}
		s_prop_datum *datum = prop_ref_get(prop_ref_index);
		prop_ref_index = datum->next_index;

		if (datum->state >= 3)
		{
			s_type_f95cd3 *view = prop_ref_view(datum);
			if (view)
			{
				view->unknown69 = true;
			}
		}
	}
}

// @retail 0x25d580
void function_25d580(long actor_index)
{
	long prop_ref_index = actor_get(actor_index)->first_prop_index;

	for (;;)
	{
		if (prop_ref_index == NONE)
		{
			break;
		}
		s_prop_datum *datum = prop_ref_get(prop_ref_index);
		long current_index = prop_ref_index;
		prop_ref_index = datum->next_index;

		if (datum->state >= 3)
		{
			function_25c820(current_index, actor_index);
		}
	}
}

// @retail 0x25d670
s_type_5cfb45 *function_25d670(long prop_ref_index)
{
	return function_25d690(prop_ref_get(prop_ref_index));
}

// @retail 0x25d690
s_type_5cfb45 *function_25d690(s_prop_datum *datum)
{
	if (g_470f10[datum->type].kind == 1)
	{
		return &prop_get(datum->prop_index)->state;
	}
	if (datum->tracking_index != NONE)
	{
		return &tracking_get(datum->tracking_index)->state;
	}
	return &prop_get(datum->prop_index)->state;
}

// @retail 0x25d700
s_type_f95cd3 *function_25d700(long index)
{
	long tmp0 = index & 0xffff;
	s_type_f95cd3 *result = NULL;
	s_prop_datum *datum = (s_prop_datum *)(g_502418->data + tmp0 * sizeof(s_prop_datum));
	if (datum->tracking_index != NONE)
	{
		byte *base = g_502414->data + (datum->tracking_index & 0xffff) * sizeof(s_type_e5ff81);
		if (base)
		{
			result = (s_type_f95cd3 *)(base + 0x70);
		}
	}
	return result;
}

// @retail 0x25d740
s_type_f95cd3 *function_25d740(s_prop_node *node)
{
	s_type_f95cd3 *result = NULL;
	if (node->tracking_index != NONE)
	{
		byte *base = g_502414->data + (node->tracking_index & 0xffff) * sizeof(s_type_e5ff81);
		if (base)
		{
			result = (s_type_f95cd3 *)(base + 0x70);
		}
	}
	return result;
}

// @retail 0x25d970
bool function_25d970(s_prop_datum *datum)
{
	if (datum->state >= 3)
	{
		s_type_f95cd3 *view = prop_ref_view(datum);
		if (view)
		{
			return !view->unknown88;
		}
	}
	return false;
}

// @retail 0x25d9b0
bool function_25d9b0(long prop_index)
{
	bool result = false;
	s_type_f95cd3 *view = function_25d700(prop_index);

	if (view)
	{
		result = view->unknown50 != NONE;
	}
	return result;
}

// @retail 0x25da00
bool function_25da00(s_prop_node_view *node)
{
	s_prop_datum *datum = (s_prop_datum *)node;

	if (datum->state >= 3)
	{
		s_type_f95cd3 *view = prop_ref_view(datum);
		if (view)
		{
			return view->unknown88;
		}
	}
	return false;
}

// @retail 0x25da40
bool function_25da40(s_prop_datum *datum)
{
	s_type_76cf92 *prop = prop_get(datum->prop_index);

	if (datum->tracking_index != NONE)
	{
		if (prop->actor_index != NONE)
		{
			s_actor_view *actor = actor_get(prop->actor_index);
			if (actor->unknown004 == 15 || (actor->unknown267 && !actor->unknown268))
			{
				return false;
			}
		}
		return true;
	}

	return prop->unknown25 && !prop->unknown23;
}

// @retail 0x25dac0
short function_25dac0(long actor_index)
{
	short count = 0;
	long prop_ref_index = actor_get(actor_index)->first_prop_index;

	for (;;)
	{
		if (prop_ref_index == NONE)
		{
			break;
		}
		s_prop_datum *datum = prop_ref_get(prop_ref_index);
		long current_index = prop_ref_index;
		prop_ref_index = datum->next_index;

		if (datum->tracking_index != NONE)
		{
			function_25c820(current_index, actor_index);
			count++;
		}
	}
	return count;
}

/* the actor as the props' cleanup sees it */
struct s_actor_prop_cleanup_view
{
	byte unknown00[9];
	bool active;
	byte unknown0a[0x10 - 0xa];
	long unknown10;
	byte unknown14[0x3e - 0x14];
	short unknown3e;
};

/* rounds as the x87 does */
static __forceinline long props_ticks_round(real ticks_real)
{
	long ticks;

	__asm
	{
		fld ticks_real
		fistp ticks
	}
	return ticks;
}

/* forgets the props of inactive actors, in up to three passes until five are
   forgotten: those away for 15 seconds outside g_4686c4, then those away 15
   seconds or outside it, then all; true if any was forgotten */
// @retail 0x25db60
bool function_25db60()
{
	long game_time = g_510c54->game_time;
	short count = 0;

	if (g_4f55d0->unknown370 < game_time)
	{
		s_actor_iterator iterator;
		s_actor_prop_cleanup_view *actor;

		function_x66da2b(&iterator, false);
		while ((actor = (s_actor_prop_cleanup_view *)function_1e46c0(&iterator)) != NULL)
		{
			if (!actor->active && actor->unknown3e != g_4686c4 &&
				game_time - actor->unknown10 > props_ticks_round(g_510c54->field_2_3 * 15.0f))
			{
				count += function_25dac0(iterator.actor_index);
			}
		}
		if (count < 5)
		{
			function_x66da2b(&iterator, false);
			while ((actor = (s_actor_prop_cleanup_view *)function_1e46c0(&iterator)) != NULL)
			{
				if (!actor->active && (actor->unknown3e != g_4686c4 ||
					game_time - actor->unknown10 > props_ticks_round(g_510c54->field_2_3 * 15.0f)))
				{
					count += function_25dac0(iterator.actor_index);
				}
			}
			if (count < 5)
			{
				function_x66da2b(&iterator, false);
				while ((actor = (s_actor_prop_cleanup_view *)function_1e46c0(&iterator)) != NULL)
				{
					if (!actor->active)
					{
						count += function_25dac0(iterator.actor_index);
					}
				}
			}
		}
	}
	return count > 0;
}

/* starts tracking the prop: takes a free slot of the actor's eight, or the
   one of its lowest priority below this one (the oldest of equal ones);
   returns the tracking, or NONE */
// @retail 0x25c570
long function_25c570(long actor_index, long prop_ref_index, short priority)
{
	s_actor_prop_view *actor = actor_prop_view_get(actor_index);
	short slot_index = NONE;
	long best_time = NONE;
	short best_priority = NONE;
	short i;

	for (i = 0; i < 8; i++)
	{
		long *tracked = &actor->tracked_prop_indices[i];

		if (*tracked == NONE)
		{
			slot_index = i;
			break;
		}

		s_prop_datum *datum = (s_prop_datum *)datum_get_inlined(g_502418, *tracked);

		if (!datum)
		{
			*tracked = NONE;
			slot_index = i;
			break;
		}

		short datum_priority = datum->unknown1a;

		if (datum_priority < priority &&
			(slot_index == NONE || datum_priority < best_priority ||
			(datum_priority == best_priority && function_25d690(datum)->unknown00 < best_time)))
		{
			slot_index = i;
			best_priority = datum_priority;
			best_time = function_25d690(datum)->unknown00;
		}
	}
	if (slot_index != NONE)
	{
		long tracking_index;
		long *tracked = &actor->tracked_prop_indices[slot_index];
		long old_index = *tracked;

		if (old_index != NONE)
		{
			function_25c820(old_index, prop_ref_get(old_index)->actor_index);
		}
		tracking_index = record_pool_allocate(g_502414);
		if (tracking_index == NONE && function_25db60())
		{
			tracking_index = record_pool_allocate(g_502414);
		}
		if (tracking_index != NONE)
		{
			prop_ref_get(prop_ref_index)->tracking_index = tracking_index;
			*tracked = prop_ref_index;

			s_type_e5ff81 *tracking = tracking_get(tracking_index);

			prop_state_initialize(&tracking->state);
			prop_view_initialize(&tracking->view);
		}
		else
		{
			*tracked = NONE;
		}
		return tracking_index;
	}
	return NONE;
}

// @retail 0x25b620
void function_25b620(long prop_ref_index, long actor_index, bool unknown)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);
	s_actor_view *actor = actor_get(actor_index);

	if (actor->unknown07c != NONE && !((s_clump_prop_view *)element_502420_get(actor->unknown07c))->unknown30)
	{
		if (unknown)
		{
			function_1fb7e0(actor_index, 0xd, NULL, datum->object_index, NONE);
		}
		else
		{
			function_1fb7e0(actor_index, 0xf, NULL, datum->object_index, NONE);
		}
	}
}

// @retail 0x25c860
void function_25c860(long prop_ref_index)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);
	s_type_5cfb45 *state = function_25d690(datum);
	s_type_f95cd3 *view = prop_ref_view(datum);
	bool tracked = datum->state >= 3;

	datum->state = 3;
	if (view && !tracked)
	{
		real ticks;
		long rounded;

		view->unknown90 = 0;
		view->unknowna2 = true;
		view->unknowna4 = state->position;
		view->unknown8c = view->unknown06;
		view->unknown8a = datum->unknown27;
		function_25c4e0(prop_ref_index);

		ticks = g_510c54->field_2_3 * 30.f;
		__asm
		{
			fld ticks
			fistp rounded
		}
		view->unknown8e = (short)rounded;
		view->unknownb0 = g_510c54->game_time;
		view->unknown64 = false;
		view->unknown66 = 0;

		if (view->unknown10 >= 0)
		{
			point3f origin;

			function_b9dd0(datum->object_index, &origin);
			view->unknown94.i = origin.x - state->position.x;
			view->unknown94.j = origin.y - state->position.y;
			view->unknown94.k = origin.z - state->position.z;
			function_30bf0(&view->unknown94);
		}
		else
		{
			view->unknown94 = *g_4687a4;
		}
	}
}

// @retail 0x25d610
bool function_25d610(s_prop_datum *datum)
{
	s_prop_type_entry *entry = &g_470f10[datum->type];
	s_type_76cf92 *prop = prop_get(datum->prop_index);
	short type = prop->unknown04;
	bool result = false;

	if (type != 1 || (entry->kind && (type != prop->unknown04 || entry->unknown8 >= 2 || prop->unknown25)))
	{
		result = true;
	}
	return result;
}

long function_1e3480(long object_index);
long function_26ace0(long object_index, long actor_index, short type);
struct s_node_view;
void function_26be00(long actor_index, s_iterator *iterator);
s_node_view *function_26be30(s_iterator *iterator);

/* the actor's prop_ref of an object: the object itself, or the actor that
   controls it */
// @retail 0x25d770
long function_25d770(long actor_index, long object_index)
{
	long object_actor_index = function_1e3480(object_index);
	s_iterator iterator;
	s_prop_datum *datum;

	function_26be00(actor_index, &iterator);
	while ((datum = (s_prop_datum *)function_26be30(&iterator)) != NULL)
	{
		s_type_76cf92 *prop = prop_get(datum->prop_index);

		if (datum->state >= 1)
		{
			if (datum->object_index == object_index)
			{
				return iterator.index;
			}
			if (prop->unknown22 && prop->actor_index != NONE && prop->actor_index == object_actor_index)
			{
				return iterator.index;
			}
		}
	}
	return NONE;
}

/* the same lookup over all of the actor's prop_refs, creating the prop_ref
   if asked */
// @retail 0x25d810
long function_25d810(long object_index, long actor_index, bool create)
{
	long result = NONE;

	if (object_index != NONE)
	{
		s_actor_prop_view *actor = actor_prop_view_get(actor_index);
		long object_actor_index = function_1e3480(object_index);

		if (object_actor_index != actor_index)
		{
			s_iterator iterator;
			s_prop_datum *datum;

			function_26be00(actor_index, &iterator);
			while ((datum = (s_prop_datum *)function_26be30(&iterator)) != NULL)
			{
				s_type_76cf92 *prop = prop_get(datum->prop_index);

				if (datum->object_index == object_index ||
					prop->unknown22 && prop->actor_index != NONE && prop->actor_index == object_actor_index)
				{
					result = iterator.index;
					break;
				}
			}
			if (result == NONE && create && actor->unknown009 && actor->unknown07c != NONE)
			{
				result = function_26ace0(object_index, actor_index, 3);
				if (result != NONE)
				{
					prop_get(prop_ref_get(result)->prop_index)->unknown10 = g_510c54->game_time + g_510c54->field_2_3 * 60;
				}
			}
		}
	}
	return result;
}

void __stdcall function_25c230(long actor_index, long prop_ref_index, short unknown);
long function_25c570(long actor_index, long prop_ref_index, short priority);

// @retail 0x25c3a0
long function_25c3a0(long actor_index, long prop_ref_index, short unknown)
{
	s_prop_datum *datum = prop_ref_get(prop_ref_index);

	if (datum->state < 1)
	{
		function_25c230(actor_index, prop_ref_index, unknown);
		return datum->tracking_index;
	}
	return function_25c570(actor_index, prop_ref_index, unknown);
}

/* the unit as 0x25c050 reads it: the object it sits in and its seat */
struct s_seated_unit_view
{
	byte unknown000[0x14];
	long parent_index;
	byte unknown018[0x1fc - 0x18];
	short seat_index;
};

bool __stdcall function_20ba60(short type, long unit_index, long target_index, long unknown, long unknown2, s_1fb7e0_data const *data);
s_ai_player *ai_player_get(long player_index);

/* the actor leaves the vehicle it rides in, and its player remembers the
   seat */
// @retail 0x25c050
void function_25c050(long player_index, long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long target_index = actor->unknown26c;

	if (target_index != NONE)
	{
		s_seated_unit_view *unit = (s_seated_unit_view *)object_get(actor->unknown018);
		long parent_index = unit->parent_index;
		long seat_index = unit->seat_index;
		s_unit_request request;

		if (actor->unknown266)
		{
			if (actor->unknown018 != NONE)
			{
				function_20ba60(0x6a, actor->unknown018, target_index, NONE, NONE, NULL);
			}
		}
		else
		{
			short type;

			if (actor->unknown268)
			{
				type = 0x6b;
			}
			else
			{
				type = 0x6c;
			}
			function_1fb7e0(actor_index, type, NULL, target_index, NONE);
		}
		memset(&request, 0, sizeof(request));
		request.type = 0x1d;
		if (function_e6900(actor->unknown018, &request))
		{
			actor->unknown2f2 = g_510c54->field_2_3 * 15;
			if (player_index != NONE)
			{
				s_ai_player *player = ai_player_get(player_index);

				if (player)
				{
					player->unit_index = parent_index;
					player->unknown08 = (short)seat_index;
					player->unknown0a = g_510c54->field_2_3 * 5;
				}
			}
		}
	}
}

/* the actor fields of its current prop's reaction */
struct s_actor_prop_reaction_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x54 - 0x1c];
	long character_index;
	byte unknown058[0x358 - 0x58];
	short unknown358;
	short unknown35a;
	bool unknown35c;
	byte unknown35d;
	bool unknown35e;
	byte unknown35f[0x368 - 0x35f];
	long prop_ref_index;
	byte unknown36c[0x888 - 0x36c];
};

/* the character's chance at +0xc */
struct s_character_prop_view
{
	byte unknown00[0xc];
	real chance;
};

long function_1e4a50(long index);
real function_259a0(dword *seed);

/* decides whether the actor reacts to its current prop */
// @retail 0x25b910
void function_25b910(long actor_index, long prop_ref_index)
{
	s_actor_prop_reaction_view *actor = (s_actor_prop_reaction_view *)actor_prop_view_get(actor_index);
	long object_index = prop_ref_get(prop_ref_index)->object_index;
	long current_index = actor->prop_ref_index;

	if (prop_ref_index == current_index)
	{
		if (actor->unknown35e)
		{
			actor->unknown35c = false;
			long unit_index = ((s_actor_prop_reaction_view *)actor_prop_view_get(actor_index))->unit_index;

			if (unit_index != NONE)
			{
				function_20ba60(0xae, unit_index, object_index, NONE, NONE, NULL);
			}
		}
		else if (actor->unknown35a == 0 && actor->unknown358 != 1 && actor->unknown358 != 4)
		{
			s_character_prop_view *character = (s_character_prop_view *)function_1e4a50(actor->character_index);

			actor->unknown35c = character->chance > function_259a0(&g_4e7408->unknown0);
		}
		else
		{
			actor->unknown35c = true;
		}
	}
}
