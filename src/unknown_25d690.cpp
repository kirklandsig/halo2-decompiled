// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_25D690.CPP: the ai's props: the "prop", "prop_ref"
   and "tracking" data arrays (include/props.h) */

#include "unknown_11c920.h"
#include "globals.h"
#include "props.h"
#include "unknown_26b230.h"
#include "unknown_1e46c0.h"
#include <string.h>
#include "squads.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_25d020.h"

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
bool __stdcall function_25d020(long actor_index, long prop_ref_index, long a, long b, long c, long d);
bool function_1fb7e0(short type, long actor_index, s_1fb7e0_data const *data, long target_index, long unknown);
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
				function_1fb7e0(0xc2, actor_index, NULL, datum->object_index, NONE);
				prop->unknown3c = false;
			}
		}
	}
}

short function_1a6fe0(long owner_index, short type);
bool function_26ba40(long clump_index, long actor_index, long prop_index);
bool __stdcall function_20ba60(short type, long unit_index, long target_index, long unknown, long unknown2, s_1fb7e0_data const *data);
real normalize2d(point2f *v);
bool function_1f57f0(long actor_index, long animation, long const *target);

// @retail 0x25ace0
void function_25ace0(long arg_0, long arg_1, short arg_2, vector3f const *arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (arg_2 > local_0->unknown3b0)
	{
		local_0->unknown3b0 = arg_2;
		local_0->unknown3b4 = arg_1;
		if (arg_2 >= 3)
		{
			function_1a8220(arg_0, 0x43, 1, 3, 1, 0x38, 3);
			long local_1 = NONE;
			if (arg_1 != NONE)
			{
				local_1 = prop_ref_get(arg_1)->object_index;
			}
			long local_2 = actor_get(arg_0)->unknown018;
			if (local_2 != NONE)
			{
				function_20ba60(0x27, local_2, local_1, NONE, NONE, NULL);
			}
			if (arg_3 && local_0->unknown018 != NONE && !function_110ab0(local_0->unknown018))
			{
				point2f local_3 = *(point2f const *)arg_3;
				if (normalize2d(&local_3) != 0.f)
				{
					real local_4 = local_0->unknown290.i * local_3.x + local_0->unknown290.j * local_3.y;
					if (local_4 > 0.7071067690849304f)
					{
						function_1f57f0(arg_0, 0xe00002a, (long const *)&local_3);
					}
					else if (local_4 < -0.7071067690849304f)
					{
						local_3.x *= -1.f;
						local_3.y *= -1.f;
						function_1f57f0(arg_0, 0xd00002b, (long const *)&local_3);
					}
				}
			}
		}
	}
}

// @retail 0x25b9f0
void function_25b9f0(long arg_0, long arg_1, vector3f const *arg_2)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_datum *local_1 = NULL;
	vector3f const *local_2 = NULL;
	vector3f local_3;
	s_type_f95cd3 *local_4 = NULL;
	if (arg_1 != NONE)
	{
		local_1 = prop_ref_get(arg_1);
		local_4 = prop_ref_view(local_1);
	}
	if (local_4)
	{
		local_2 = (vector3f *)&local_4->unknown2b[1];
	}
	else if (arg_2)
	{
		real local_5 = length_sq3f(arg_2);
		if (local_5 > 0.25f)
		{
			real local_6 = -1.f / (real)sqrt(local_5);
			local_3.i = (real)(local_6 * arg_2->i);
			local_3.j = (real)(local_6 * arg_2->j);
			local_3.k = (real)(local_6 * arg_2->k);
			local_2 = &local_3;
		}
	}
	if ((!local_1 || prop_get(local_1->prop_index)->unknown23) && local_0->unknown084 < 4)
	{
		function_25ace0(arg_0, arg_1, 5, local_2);
	}
	if (local_1 && prop_get(local_1->prop_index)->unknown23)
	{
		actor_get(arg_0)->unknown223 = false;
	}
	s_actor_prop_view *local_7 = (s_actor_prop_view *)local_0;
	if (arg_1 != NONE && local_7->unknown684 <= 0)
	{
		real local_8 = g_510c54->field_2_3 * 2.f;
		long local_9;
		__asm
		{
			fld local_8
			fistp local_9
		}
		local_7->unknown686 = (short)local_9;
		local_7->unknown688 = 1;
		local_7->unknown68c = arg_1;
		local_7->unknown684 = 0;
	}
}

// @retail 0x25af70
void function_25af70(long arg_0, long arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_datum *local_1 = prop_ref_get(arg_1);
	s_type_f95cd3 *local_2 = prop_ref_view(local_1);
	if ((g_510c54->game_time - local_2->unknownb0) * g_510c54->rate > 2.f &&
		arg_1 == *(long *)((byte *)local_0 + 0x338))
	{
		if (function_1a6fe0(arg_0, 0x2b) != NONE)
		{
			if (local_2->unknown39 <= 2)
			{
				function_1fb7e0(0x1a, arg_0, NULL, local_1->object_index, NONE);
			}
		}
		else if (function_1a6fe0(arg_0, 0x38) != NONE)
		{
			if (local_2->unknown39 <= 2)
			{
				function_1fb7e0(0x1b, arg_0, NULL, local_1->object_index, NONE);
			}
		}
		else if (function_26ba40(local_0->unknown07c, arg_0, local_1->prop_index))
		{
			if (local_2->unknown69)
			{
				function_1fb7e0(0x19, arg_0, NULL, local_1->object_index, NONE);
			}
			else if (function_1a6fe0(arg_0, 0x1b) != NONE)
			{
				function_1fb7e0(0x18, arg_0, NULL, local_1->object_index, NONE);
			}
			else if (local_2->unknown70 == 0)
			{
				function_1fb7e0(0x17, arg_0, NULL, local_1->object_index, NONE);
			}
			else
			{
				function_1fb7e0(0x19, arg_0, NULL, local_1->object_index, NONE);
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
	real local_0 = delta.k * delta.k;
	local_0 += delta.i * delta.i;
	local_0 += delta.j * delta.j;
	if (64.f > local_0)
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

#pragma inline_depth(0)
// @retail 0x25d420
void function_25d420(long prop_ref_index, short type, long actor_index)
{
	long const *local_0 = &actor_index;
	s_actor_view *actor = (s_actor_view *)(g_4f55f0->data + (*local_0 & 0xffff) * sizeof(s_actor_view));

	if (prop_ref_index != NONE)
	{
		s_prop_datum *datum = (s_prop_datum *)(g_502418->data + (prop_ref_index & 0xffff) * sizeof(s_prop_datum));
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
					started = function_1fb7e0(0x33, actor_index, NULL, datum->object_index, NONE);
					break;
				case 2:
					started = function_1fb7e0(0x32, actor_index, NULL, datum->object_index, NONE);
					break;
				case 3:
					started = function_1fb7e0(0x35, actor_index, NULL, datum->object_index, NONE) ||
						function_1fb7e0(0x3b, actor_index, NULL, datum->object_index, NONE);
					break;
				default:
					return;
				}

				if (started)
				{
					((s_type_76cf92 *)(g_50241c->data + (datum->prop_index & 0xffff) * sizeof(s_type_76cf92)))->unknown34 = true;
				}
			}
		}
	}
}

#pragma inline_depth(255)

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

PRIVATE __forceinline s_type_76cf92 *function_25da41(long arg_0)
{
	return (s_type_76cf92 *)(g_50241c->data + (arg_0 & 0xffff) * sizeof(s_type_76cf92));
}

// @retail 0x25da40
bool function_25da40(s_prop_datum *datum)
{
	s_type_76cf92 *prop = function_25da41(datum->prop_index);
	long *local_2 = &datum->tracking_index;
	long local_1 = *(volatile long *)local_2;
	bool local_0 = true;

	if (local_1 != NONE)
	{
		if (prop->actor_index != NONE)
		{
			s_actor_view *actor = actor_get(prop->actor_index);
			if (actor->unknown004 == 15 || (actor->unknown267 && !actor->unknown268))
			{
				local_0 = false;
			}
		}
	}
	else
	{
		local_0 = prop->unknown25 && !prop->unknown23;
	}
	return local_0;
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
	long tracking_index = NONE;
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
	}
	return tracking_index;
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
			function_1fb7e0(0xd, actor_index, NULL, datum->object_index, NONE);
		}
		else
		{
			function_1fb7e0(0xf, actor_index, NULL, datum->object_index, NONE);
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
		point3f const *local_0 = &state->position;
		real ticks;
		long rounded;

		view->unknown90 = 0;
		view->unknowna2 = true;
		view->unknowna4 = *local_0;
		view->unknown8a = datum->unknown27;
		view->unknown8c = view->unknown06;
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
			vector3d_from_points3d(local_0, &origin, &view->unknown94);
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
	bool result = false;
	s_prop_type_entry *entry = &g_470f10[datum->type];
	long local_0 = datum->prop_index & 0xffff;
	s_type_76cf92 *local_1 = (s_type_76cf92 *)((s_record_pool volatile *)g_50241c)->data;
	short type = local_1[local_0].unknown04;
	s_type_76cf92 *prop = &local_1[local_0];

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

bool __stdcall function_25c230(long actor_index, long prop_ref_index, short unknown);
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
			volatile long local_0;

			if (actor->unknown268)
			{
				local_0 = 0x6b;
			}
			else
			{
				local_0 = 0x6c;
			}
			function_1fb7e0((short)local_0, actor_index, NULL, target_index, NONE);
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

struct s_object;
s_object *function_badc0(long arg_0, dword arg_1);
bool g_4f55ea;
struct s_perception_origin_view;
struct s_object_motion_view;
short function_263ed0(long actor_index, s_perception_origin_view const *origin, point3f const *point,
	s_location const *location, short type, short mode);
short function_263810(point3f const *origin, long actor_index, point3f const *point, point3f const *endpoint, char posture, short mode, bool use_facing, bool *out_of_range);
void function_2640c0(long object_index, s_object_motion_view *result);

// @retail 0x25cb60
long function_25cb60(long arg_0, long arg_1, short arg_2, void *arg_3, void *arg_4, long arg_5, long arg_6)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_squad_datum *local_1 = local_0->unknown030 != NONE ? squad_get(local_0->unknown030) : NULL;
	if (local_1 && local_1->flag1)
		return 0;
	if (arg_2 == 1 || arg_2 == 2)
		return 3;
	long local_2 = arg_1;
	s_slot_object_view *local_3 = (s_slot_object_view *)function_badc0(arg_1, 3);
	if (local_3)
	{
		if (local_3->actor_index != NONE && actor_get(local_3->actor_index)->unknown004 == 15)
			local_2 = NONE;
		else if (local_3->parent_index != NONE && object_get(local_3->parent_index)->type == 1)
			local_2 = local_3->parent_index;
	}
	local_3 = (s_slot_object_view *)function_badc0(local_2, 3);
	if (local_3)
	{
		byte *local_4 = g_4e3b44[local_3->tag_index & 0xffff].bytes;
		short local_5 = g_4e6948->state == 1 && g_4f55ea ? 2 : *(short *)(local_4 + 0xc2);
		return function_263ed0(arg_0, (s_perception_origin_view const *)arg_3, (point3f const *)arg_4, (s_location const *)arg_5, local_5, (short)arg_6);
	}
	return 0;
}

// @retail 0x25c9d0
long function_25c9d0(long arg_0, long arg_1, bool arg_2, long arg_3, void *arg_4, byte *arg_5, long arg_6)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_squad_datum *local_1 = local_0->unknown030 != NONE ? squad_get(local_0->unknown030) : NULL;
	s_prop_datum *local_2 = prop_ref_get(arg_1);
	byte *local_3 = (byte *)function_badc0(local_2->object_index, 3);
	bool local_4 = local_2->type == 1;
	bool local_5 = (local_1 && local_1->flag0) || local_0->unknown084 == 1 || local_0->unknown228;
	if (local_3 && (arg_2 ? *(real *)(local_3 + 0x2b0) > 0.05f : *(real *)(local_3 + 0x2b0) > 0.8f))
	{
		if (local_4)
			return 0;
		local_5 |= arg_2 && local_2->unknown28 > 4.f;
	}
	if (local_5)
		return 0;
	bool local_6 = true;
	if (local_0->unknown270 == 4 || local_0->unknown004 == 15 ||
		(!local_4 && local_2->type != 3) ||
		(local_2->state >= 1 && local_2->state <= 2) || local_0->unknown223)
		local_6 = false;
	volatile long local_7;
	if (local_0->unknown086 >= 3)
		local_7 = 2;
	else
		local_7 = local_0->unknown084 >= 4;
	return function_263810((point3f const *)arg_5, arg_0, (point3f const *)arg_4, (point3f const *)(arg_5 + 0xc), 2, (short)arg_3, local_6, (bool *)arg_6);
}

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

bool function_11c120(s_location const *arg_0, point3f const *arg_1, short *arg_2);
long function_1caa10(long arg_0);
bool function_f0f90(long arg_0);
bool function_26bf10(long arg_0);
struct s_location_target_view;
void function_26c240(long arg_1, s_location_target_view *arg_0);
long function_baf80(long arg_0);
bool function_1e1de0(long arg_0);

// @retail 0x25ccd0
bool function_25ccd0(s_type_5cfb45 *arg_0, long arg_1, short arg_2, s_2640c0 *arg_3)
{
	volatile bool local_0 = true;
	long local_1 = g_510c54->game_time;
	if (arg_0->unknown00 == local_1)
		return false;
	s_slot_object_view *local_2 = object_get(arg_1);
	s_slot_object_view *local_3 = NULL;
	long local_4 = NONE;
	if ((1 << local_2->type) & 3)
	{
		local_3 = local_2;
		if (local_3->actor_index != NONE)
			local_4 = local_3->actor_index;
	}
	union
	{
		s_2640c0 field_0;
		s_object_marker field_1;
	} local_5;
	if (!arg_3)
	{
		function_2640c0(arg_1, (s_object_motion_view *)&local_5.field_0);
		arg_3 = &local_5.field_0;
	}
	*(point3f *)((byte *)arg_0 + 0x30) = arg_3->field_0;
	arg_0->position = arg_3->field_c;
	*(s_location *)((byte *)arg_0 + 0x28) = arg_3->field_24;
	*(vector3f *)((byte *)arg_0 + 0x1c) = arg_3->field_2c;
	bool local_6 = false;
	if (local_3)
	{
		function_b8d30(arg_1, 0x40000bd, &local_5.field_1, 1, false);
		*(point3f *)((byte *)arg_0 + 0x10) = local_5.field_1.matrix.position;
		arg_0->unknown67 = ((*(dword *)((byte *)local_3 + 0x134) >> 8) & 1) != 0;
	}
	else
	{
		*(point3f *)((byte *)arg_0 + 0x10) = arg_0->position;
	}
	arg_0->unknown69 = function_11c120((s_location *)((byte *)arg_0 + 0x28), (point3f *)((byte *)arg_0 + 0x10), NULL);
	arg_0->unknown3c = NONE;
	arg_0->unknown65 = false;
	arg_0->unknown66 = false;
	arg_0->unknown40 = NONE;
	arg_0->unknown63 = arg_2 == 1;
	if (local_2->parent_index != NONE)
	{
		s_slot_object_view *local_7 = object_get(local_2->parent_index);
		if (local_3 && local_7->type == 1)
		{
			arg_0->unknown3c = function_1caa10(local_3->parent_index);
			arg_0->unknown65 = *(long *)((byte *)local_7 + 0x24c) == arg_1 || local_3->type == 15;
			arg_0->unknown66 = *(long *)((byte *)local_7 + 0x248) == arg_1 && function_f0f90(local_3->parent_index);
		}
		else if ((1 << local_7->type) & 3)
		{
			arg_0->unknown40 = local_2->parent_index;
		}
	}
	if (function_26bf10(arg_1))
		function_26c240(arg_1, (s_location_target_view *)arg_0);
	else
		arg_0->unknown58 = false;
	s_slot_object_view *local_8 = object_get(function_baf80(arg_1));
	switch ((char)local_8->type)
	{
	case 0:
		arg_0->unknown64 = *((byte *)local_8 + 0x3dc) == 2;
		break;
	case 1:
		arg_0->unknown64 = ((*(dword *)(g_4e3b44[local_8->tag_index & 0xffff].bytes + 0x1ec) >> 12) & 1) != 0;
		break;
	case 12:
		arg_0->unknown64 = *((byte *)local_8 + 0x17c) == 2;
		break;
	default:
		arg_0->unknown64 = false;
		break;
	}
	bool local_9 = ((*((byte *)local_2 + 0x10a) >> 2) & 1) != 0;
	arg_0->unknown5f = local_9 && !arg_0->unknown5e;
	arg_0->unknown5e = local_9;
	bool local_10;
	bool local_11;
	if (local_4 == NONE)
	{
		local_10 = false;
		local_11 = !local_9;
	}
	else
	{
		local_6 = actor_get(local_4)->unknown084 < 4;
		local_10 = function_1e1de0(local_4);
		local_11 = function_1a6fe0(local_4, 14) != NONE;
	}
	arg_0->unknown60 = local_6;
	arg_0->unknown62 = local_11;
	arg_0->unknown61 = local_10;
	arg_0->unknown00 = local_1;
	return local_0;
}


void function_25ae40(long arg_0, long arg_1, bool arg_2);

// @retail 0x25c230
bool __stdcall function_25c230(long arg_0, long arg_1, short arg_2)
{
	s_prop_datum *local_0 = prop_ref_get(arg_1);
	s_type_76cf92 *local_1 = prop_get(local_0->prop_index);
	local_0->type = *(short *)((byte *)local_1 + 2);
	short local_2 = *(short *)((byte *)local_1 + 0xc);
	local_0->unknown1a = arg_2 > (local_2 > local_0->unknown1a ? local_2 : local_0->unknown1a) ? arg_2 :
		(local_2 > local_0->unknown1a ? local_2 : local_0->unknown1a);
	bool local_3 = local_0->state == 0;
	bool local_4 = local_0->state >= 1 && local_0->state <= 2;
	local_0->state = 1;
	if (g_470f10[local_0->type].unknown8 == 2 && local_0->tracking_index == NONE)
		function_25c570(arg_0, arg_1, local_0->unknown1a);
	if (!local_4)
	{
		function_25d690(local_0)->unknown68 = 1;
		if (local_1->unknown04 <= 0 && g_470f10[*(short *)((byte *)local_1 + 2)].unknown4 == 1)
		{
			local_1->unknown04 = 1;
			function_25ccd0(&local_1->state, *(long *)((byte *)local_1 + 8), NONE, NULL);
		}
		s_2640c0 local_5;
		s_2641c0 local_6;
		function_2640c0(local_0->object_index, (s_object_motion_view *)&local_5);
		if (function_2641c0(arg_0, &local_6, &local_5.field_c))
			function_264330(arg_0, arg_1, &local_6, &local_5, false);
		function_25ae40(arg_0, arg_1, local_3);
	}
	return true;
}

void __stdcall function_26c2d0(long arg_0);
void function_1e4500(long arg_0, short arg_1);

// @retail 0x25b440
void __stdcall function_25b440(long arg_0, long arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->unknown086 >= 3)
		return;
	s_prop_datum *local_1 = prop_ref_get(arg_1);
	long local_2 = NONE;
	long local_3 = local_0->first_prop_index;
	while (local_3 != NONE)
	{
		s_prop_datum *local_4 = prop_ref_get(local_3);
		long local_5 = local_3;
		local_3 = local_4->next_index;
		if (local_4->type == 1)
		{
			if (prop_get(local_4->prop_index)->unknown25)
			{
				local_2 = local_5;
				break;
			}
			if (local_4->unknown28 < 3.402823466e38f)
				local_2 = local_5;
		}
	}
	if (local_2 != NONE)
	{
		s_prop_datum *local_6 = prop_ref_get(local_2);
		if (function_25d020(arg_0, local_6->object_index, 0, local_2, NONE, NONE))
		{
			s_type_5cfb45 *local_7 = function_25d690(local_1);
			s_type_5cfb45 *local_8 = function_25d690(local_6);
			if (local_7 && local_8)
			{
				function_26c2d0(arg_1);
				*(point3f *)((byte *)local_8 + 0x30) = *(point3f *)((byte *)local_7 + 0x30);
				local_8->position = local_7->position;
				*(long *)((byte *)local_8 + 0x28) = *(long *)((byte *)local_7 + 0x28);
				*(long *)((byte *)local_8 + 0x2c) = *(long *)((byte *)local_7 + 0x2c);
				local_8->unknown44 = local_7->unknown44;
				*(s_type_c3b527 *)&local_8->unknown48 = *(s_type_c3b527 *)&local_7->unknown48;
				long local_9 = props_ticks_round(g_510c54->field_2_3 * 2.f);
				function_1a8220(arg_0, 0x64, (short)local_9, 3, 0xe, 0x51, 3);
				return;
			}
		}
	}
	function_1e4500(arg_0, 3);
	long local_10 = props_ticks_round(g_510c54->field_2_3 * 2.f);
	function_1a8220(arg_0, 0x64, (short)local_10, 3, 0x68, NONE, 0);
}

long function_1fa7f0(void);
long __stdcall function_26d0e0(point3f const *arg_0, s_type_c3b527 *arg_1, long arg_2);

#pragma optimize("s", on)
// @retail 0x25b6b0
void function_25b6b0(long arg_0, long arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_datum *local_1 = prop_ref_get(arg_1);
	long local_2 = local_1->object_index;
	s_slot_object_view *local_3 = object_get(local_2);
	if (local_3->type == 5 && local_1->unknown27 > 0 && local_0->unknown018 != NONE)
		function_20ba60(6, local_0->unknown018, local_2, NONE, NONE, NULL);
	if (local_3->type == 5)
	{
		long local_4 = *(long *)((byte *)local_3 + 0xc8);
		if (local_4 != NONE && function_badc0(local_4, NONE) &&
			function_1df560(local_0->unknown024, *(short *)((byte *)local_3 + 0xc2)))
		{
			long local_5 = function_25d810(local_4, arg_0, false);
			if (local_5 != NONE)
			{
				s_prop_datum *local_6 = prop_ref_get(local_5);
				s_type_f95cd3 *local_7 = function_25d740((s_prop_node *)local_6);
				if (local_6->state < 1 || (local_7 && *(long *)((byte *)local_7 + 0xc) + 3 * g_510c54->field_2_3 < g_510c54->game_time))
				{
					if (function_25d020(arg_0, local_4, 0, local_5, NONE, NONE))
					{
						s_type_5cfb45 *local_8 = function_25d690(local_6);
						if (local_8)
						{
							point3f local_9;
							function_b9dd0(local_2, &local_9);
							*(point3f *)((byte *)local_8 + 0x30) = local_9;
							local_8->position = local_9;
							s_slot_object_view *local_10 = object_get(function_baf80(local_2));
							*(long *)((byte *)local_8 + 0x28) = *(long *)((byte *)local_10 + 0x28);
							*(long *)((byte *)local_8 + 0x2c) = *(long *)((byte *)local_10 + 0x2c);
							s_type_c3b527 local_11;
							local_8->unknown44 = function_26d0e0(&local_9, &local_11, function_1fa7f0());
							*(s_type_c3b527 *)&local_8->unknown48 = local_11;
						}
						if (local_7 && local_3->parent_index == NONE)
						{
							local_7->unknown94.i = local_3->velocity.i * -1.f;
							local_7->unknown94.j = local_3->velocity.j * -1.f;
							local_7->unknown94.k = local_3->velocity.k * -1.f;
							local_7->unknown94.k = 0.f;
							function_30bf0(&local_7->unknown94);
						}
					}
				}
			}
		}
	}
}
#pragma optimize("", on)

bool function_2675b0(long arg_0);
bool function_267680(long arg_0);
void function_26b7a0(long prop_index, long clump_index, bool immediate, bool *available, bool *first, bool *notify);

// @retail 0x25b120
void __stdcall function_25b120(long arg_0, long arg_1, bool arg_2)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_datum *local_1 = prop_ref_get(arg_1);
	s_type_5cfb45 *local_2 = function_25d690(local_1);
	s_type_f95cd3 *local_3 = prop_ref_view(local_1);
	s_type_76cf92 *local_4 = prop_get(local_1->prop_index);
	if (local_3 && local_3->unknown14 == NONE)
	{
		local_3->unknown14 = g_510c54->game_time;
		arg_2 = true;
	}
	if (local_4->unknown25 && local_0->unknown030 != NONE)
		*((byte *)squad_get(local_0->unknown030) + 2) |= 8;
	if (arg_2)
	{
		if (local_1->type == 7)
		{
			function_25b440(arg_0, arg_1);
			return;
		}
		bool local_5 = false;
		arg_2 = false;
		bool local_6 = false;
		bool local_7;
		if (local_1->type == 1 && ((1 << object_header_get(local_1->object_index)->type) & 3) &&
			object_get(local_1->object_index)->unknown1fc != NONE)
		{
			local_5 = true;
			local_7 = false;
		}
		else if (prop_get(local_1->prop_index)->unknown25)
			local_7 = false;
		else if (local_1->type == 1 && function_2675b0(arg_1))
		{
			local_6 = true;
			local_7 = true;
		}
		else if (local_1->type == 1 && local_2->unknown3c == NONE && function_267680(arg_1))
		{
			arg_2 = true;
			local_7 = true;
		}
		else
		{
			if (local_0->unknown086 >= 6)
				return;
			local_7 = false;
		}
		bool local_8, local_9, local_10;
		function_26b7a0(local_1->prop_index, local_0->unknown07c, local_7, &local_8, &local_9, &local_10);
		if (local_8 && !function_25d690(local_1)->unknown5e && local_10)
		{
			if (arg_2 && function_1fb7e0(0xb, arg_0, NULL, local_1->object_index, NONE))
				return;
			if (local_5)
				function_1fb7e0(6, arg_0, NULL, local_1->object_index, NONE);
			else if (local_9)
				function_1fb7e0(0xa, arg_0, NULL, local_1->object_index, NONE);
			else if (local_6)
				function_1fb7e0(7, arg_0, NULL, local_1->object_index, NONE);
			else
				function_1fb7e0(6, arg_0, NULL, local_1->object_index, NONE);
		}
	}
	else if (local_3)
	{
		s_actor_prop_view *local_11 = (s_actor_prop_view *)local_0;
		if (local_11->unknown684 <= 0)
		{
			local_11->unknown686 = (short)props_ticks_round(g_510c54->field_2_3 * 2.f);
			local_11->unknown688 = 1;
			local_11->unknown68c = arg_1;
			local_11->unknown684 = 0;
		}
		function_25af70(arg_0, arg_1);
	}
}

void *function_1e51a0(long arg_0);

// @retail 0x25ae40
void function_25ae40(long arg_0, long arg_1, bool arg_2)
{
	s_actor_view *local_0 = actor_get(arg_0);
	s_prop_datum *local_1 = prop_ref_get(arg_1);
	if (arg_2 && local_1->type == 3)
		function_25b6b0(arg_0, arg_1);
	else if (local_1->unknown27 > 0)
		function_25b120(arg_0, arg_1, arg_2);
	else
	{
		s_actor_prop_view *local_2 = (s_actor_prop_view *)local_0;
		if (local_2->unknown684 <= 0)
		{
			local_2->unknown686 = (short)props_ticks_round((real)g_510c54->field_2_3);
			local_2->unknown688 = 1;
			local_2->unknown68c = arg_1;
			local_2->unknown684 = 0;
		}
	}
	if (local_1->type == 1 && arg_2)
	{
		byte *local_3 = (byte *)function_1e51a0(arg_0);
		if (local_3 && *(real *)(local_3 + 0x30) > local_1->unknown28)
		{
			vector3f local_4;
			vector3d_from_points3d(&local_0->position, &function_25d690(local_1)->position, &local_4);
			function_25ace0(arg_0, arg_1, 3, &local_4);
		}
	}
}

// @retail 0x25bcf0
void function_25bcf0(long arg_0, long arg_1, long arg_2)
{
	s_prop_datum *local_0 = NULL;
	if (arg_1 != NONE)
		local_0 = prop_ref_get(arg_1);
	actor_get(arg_0)->unknown223 = false;
	if (local_0 && prop_get(local_0->prop_index)->unknown23)
	{
		function_25d020(arg_0, local_0->object_index, 2, arg_1, NONE, NONE);
		return;
	}
	s_actor_view *local_1 = NULL;
	if (arg_2 != NONE)
		local_1 = actor_get(arg_2);
	long local_2;
	if (local_0)
		local_2 = local_0->object_index;
	else if (local_1)
		local_2 = local_1->unknown018;
	else
		return;
	s_slot_object_view *local_3 = object_get(local_2);
	if (local_3)
	{
		if (local_3->player_index != NONE)
		{
			byte *local_4 = g_4e8c24->data + (local_3->player_index & 0xffff) * 0x21c;
			if (*(long *)(local_4 + 0x174) != NONE &&
				*(long *)(local_4 + 0x178) + props_ticks_round(g_510c54->field_2_3 * 3.f) >= g_510c54->game_time)
			{
				s_object_header_view *local_5 = object_header_get(*(long *)(local_4 + 0x174));
				if (((1 << local_5->type) & 3) &&
					function_1df560(actor_get(arg_0)->unknown024, ((s_slot_object_view *)local_5->object)->team))
					function_25d020(arg_0, *(long *)(local_4 + 0x174), 2, NONE, NONE, NONE);
			}
		}
		else if (local_3->actor_index != NONE)
		{
			long local_6 = local_3->actor_index;
			if (!local_1)
				local_1 = actor_get(local_6);
			if (local_1->unknown086 >= 6 && local_1->prop_index != NONE)
			{
				long local_7 = local_1->prop_index;
				function_25d020(arg_0, prop_ref_get(local_7)->object_index, 2, NONE, local_6, local_7);
			}
		}
	}
}
