// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2551C0.CPP: slot handler 0xc (handler at 0x47f790) */

#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_2551c0.h"
#include "unknown_0259a0.h"
#include "unknown_1dacb0.h"

/* the slot state of handler 0xc */
struct s_slot_0c_state
{
	s_slot_header header;
	short timer;
	byte unknown0e[2];
	point3f point;
	real unknown1c;
};

/* the unit's offset at +0x346 */
struct s_unit_0c_view
{
	byte unknown000[0x346];
	short unknown346;
};

/* turns a vector about a unit axis by the angle whose sine and cosine are
   given */
static inline void function_x84d9e8(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	real i = vector->i;
	real j = vector->j;
	real k = vector->k;
	real projection = (axis->k * k + axis->i * i + j * axis->j) * (1.f - cosine);

	vector->i = (axis->i * projection + i * cosine) - (axis->k * j - k * axis->j) * sine;
	vector->j = (j * cosine + projection * axis->j) - (k * axis->i - axis->k * i) * sine;
	vector->k = (axis->k * projection + k * cosine) - (i * axis->j - j * axis->i) * sine;
}

real function_30bf0(vector3f *v);
bool function_26bf10(long object_index);
long function_1fa7f0(void);
void function_cfec0(long unit_index);
bool function_e68c0(long type, long unit_index);

short g_470b4c = -1;
short g_470b50 = -2;

short __stdcall function_2551c0(long actor_index);
short __stdcall function_255570(long actor_index, s_slot *slot, bool active);
bool __stdcall function_2551f0(long actor_index, s_slot *slot);
void __stdcall function_255520(long actor_index, s_slot *slot);
void __stdcall function_255660(long actor_index, s_slot *slot);

s_slot_handler_2 g_47f790 =
{
	{
		0xc, 2, NONE, -2, 0,
		function_2551c0, function_255570, function_2551f0, function_255520, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)slot_start_true, 0, function_255660
};

// @retail 0x2551c0
short __stdcall function_2551c0(long actor_index)
{
	short local_0 = 0;
	s_actor_view *local_1 = actor_get(actor_index);
	local_0 = local_1->unknown018 == NONE ? local_0 : 3;
	return local_0;
}

// @retail 0x2551f0
bool __stdcall function_2551f0(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;

	if (actor->unknown018 == NONE)
	{
		return false;
	}

	state->timer = g_510c54->field_2_3 * 10;
	*(vector3f *)&state->point = actor->unknown290;
	state->unknown1c = 0.f;
	if (function_30bf0((vector3f *)&state->point) > 0.f)
	{
		if (function_26bf10(actor->unknown018))
		{
			function_26c180(actor_index);
			if (actor->unknown27c.unknown10 != NONE)
			{
				s_pathfinding_data *pathfinding = (s_pathfinding_data *)function_1fa7f0();
				real best_score = 0.f;
				bool found = false;
				vector3f forward;
				vector3f best_direction;

				function_210770(actor->unknown27c.point.output_index, &actor->unknown290, &forward);
				for (long i = 0; i < 8; i++)
				{
					real angle = (real)i * 0.7853982f;
					vector3f direction = forward;
					s_path_trace_result trace;

					function_x84d9e8(&direction, g_4687b0, (real)sin(angle), (real)cos(angle));
					function_26c590(actor->unknown27c.unknown10, &actor->unknown27c.point.point, &trace, pathfinding,
						&actor->unknown27c.point.point, NONE, &direction, 5.f, 0);

					real score = (dot3f(&forward, &direction) + 1.f) * (trace.distance * 0.2f);
					if (score > best_score)
					{
						best_score = score;
						best_direction = direction;
						found = true;
					}
				}

				if (found)
				{
					function_210770(actor->unknown27c.point.output_index, &best_direction, (vector3f *)&state->point);
				}
			}
		}

		function_cfec0(actor->unknown018);
		s_unit_0c_view *unit = (s_unit_0c_view *)object_get(actor->unknown018);
		if (*(short *)((byte *)unit + unit->unknown346 + 0x36) != 0)
		{
			function_e68c0(0x27, actor->unknown018);
		}
		return true;
	}
	return false;
}

// @retail 0x255520
void __stdcall function_255520(long actor_index, s_slot *slot)
{
	dword *flags = &handler_object_get(actor_get(actor_index)->unknown018)->flags134;
	*flags &= ~0x200000;
}

// @retail 0x255570
short __stdcall function_255570(long actor_index, s_slot *slot, bool active)
{
	s_slot_0c_state *state = (s_slot_0c_state *)slot;
	short result = g_470b50;
	real ticks;
	long rounded;

	state->timer--;
	if (state->timer <= 0)
	{
		return g_470b4c;
	}

	ticks = g_510c54->field_2_3 * 0.5f;
	__asm
	{
		fld ticks
		fistp rounded
	}

	if (state->timer > rounded)
	{
		s_actor_view *actor = actor_get(actor_index);

		if (actor->unknown018 != NONE)
		{
			long child_index = handler_object_get(actor->unknown018)->first_child_index;

			while (child_index != NONE)
			{
				s_handler_object_view *child = handler_object_get(child_index);
				if (child->type == 5)
				{
					return result;
				}
				child_index = child->next_object_index;
			}

			long timer;

			ticks = g_510c54->field_2_3 * 0.5f;
			__asm
			{
				fld ticks
				fistp timer
			}
			state->timer = (short)timer;
		}
	}

	return result;
}

// @retail 0x255660
void __stdcall function_255660(long actor_index, s_slot *slot)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_0c_state *state = (s_slot_0c_state *)slot;

	actor->unknown458 = *(vector3f *)&state->point;
	actor->unknown450 = 0x4000089;
	actor->unknown456 = true;
}

struct s_2556b0
{
	byte field_0[0xc4];
	long field_c4;
	s_pathfinding_data *field_c8;
};

struct s_2556c0
{
	byte field_0[4];
	word field_4;
	byte field_6[10];
};

// @retail 0x2556b0
long function_2556b0(point3f const *arg_0, vector3f const *arg_1, long arg_2, real *arg_3)
{
	s_pathfinding_data *local_0 = NULL;
	s_2556b0 *local_1 = (s_2556b0 *)g_4e0348;
	if (local_1->field_c4 > 0)
	{
		local_0 = local_1->field_c8;
	}
	s_sector_trace_result local_2;
	function_26c590(local_0, arg_0, arg_2, NONE, arg_1, 1.5f, NULL, (s_path_trace_result *)&local_2);
	if (local_2.blocked)
	{
		long local_5 = local_2.edge_index;
		if (local_5 >= 0 && local_5 < *(long *)local_0->unknown08)
		{
			s_pathfinding_edge const *local_6 = local_0->edges + local_5;
			byte const *local_7 = (byte const *)local_6;
			if (local_7[5] & 4)
			{
				local_2.distance = 1.5f;
				local_2.blocked = false;
			}
		}
	}
	if (arg_3)
	{
		*arg_3 = local_2.distance;
	}
	return !local_2.blocked;
}

struct s_255d60
{
	byte field_0[4];
	point3f field_4;
	short field_10;
	short field_12;
	long field_14;
	long field_18;
};

point3f *function_b9dd0(long object_index, point3f *result);

// @retail 0x255d60
bool function_255d60(long arg_0, point3f const *arg_1, real arg_2, real arg_3, short *arg_4)
{
	bool local_3 = false;
	long local_4 = 0;
	s_record_pool_iterator local_5;
	real local_20 = arg_2 > arg_3 ? arg_2 : arg_3;
	s_actor_view *local_0 = actor_get(arg_0);
	real local_1 = local_20 * local_20;
	real local_2 = arg_2 * arg_2;
	if (g_4f55d0->active)
	{
		local_5.data = g_502420;
		local_5.index = NONE;
	}
	for (;;)
	{
		s_255d60 *local_6 = NULL;
		if (g_4f55d0->active)
		{
			long local_7 = data_next_absolute_index_inlined(local_5.data, local_5.index + 1);
			if (local_7 != NONE)
			{
				local_6 = (s_255d60 *)(local_5.data->data + local_5.data->size * local_7);
				local_5.index = local_7;
			}
		}
		if (!local_6)
		{
			break;
		}
		bool local_8 = function_1df560(local_0->unknown024, local_6->field_12);
		if (distance3d(&local_6->field_4, arg_1) < local_20 + 6.0)
		{
			long local_10 = local_6->field_18;
			while (local_10 != NONE)
			{
				s_actor_view *local_11 = actor_get(local_10);
				local_10 = local_11->next_index;
				if (local_11->unknown018 != NONE)
				{
					point3f local_12;
					vector3f local_13;
					function_b9dd0(local_11->unknown018, &local_12);
					vector3d_from_points3d(arg_1, &local_12, &local_13);
					real local_14 = length_sq3f(&local_13);
					if (local_8)
					{
						if (local_2 > local_14)
						{
							local_4++;
						}
					}
					else if (local_1 > local_14)
					{
						local_3 = true;
						break;
					}
				}
			}
		}
	}
	if (!local_3 && !team_is_enemy(local_0->unknown024, 1))
	{
		s_record_pool *local_15 = g_4e8c24;
		long local_16 = NONE;
		for (;;)
		{
			local_16 = function_16bc00(local_15, local_16 + 1);
			if (local_16 == NONE)
			{
				break;
			}
			byte *local_17 = local_15->data + local_15->size * local_16;
			if (!local_17)
			{
				break;
			}
			long local_18 = *(long *)(local_17 + 0x2c);
			if (local_18 != NONE)
			{
				vector3f local_19;
				vector3d_from_points3d(arg_1, &object_get(local_18)->unknown030, &local_19);
				if (local_1 > length_sq3f(&local_19))
				{
					local_3 = true;
					break;
				}
			}
		}
	}
	if (arg_4)
	{
		*arg_4 = (short)local_4;
	}
	return local_3;
}

struct s_255b10
{
	byte field_0[0x68];
	long field_68;
	byte field_6c[4];
	long field_70;
	long field_74;
	long field_78;
};

struct s_255ce0
{
	byte field_0[0x7cc];
	bool field_7cc;
	byte field_7cd[3];
	s_type_c3b527 field_7d0;
	long field_7e0;
};

real normalize2d(point2f *v);
bool function_10f340(long unit_index, long mode, long set);
void *function_1e5380(long actor_index);
bool function_1ff6c0(long actor_index, long prop_index, point3f const *point, long *target);
bool __stdcall function_1ffa30(s_type_c3b527 const *arg_0, long arg_4, long arg_1, long arg_2, bool arg_3);

// @retail 0x255b10
bool function_255b10(long arg_0, s_type_c3b527 const *arg_1, long arg_2, bool arg_3)
{
	s_actor_view *local_0 = actor_get(arg_0);
	long local_2 = NONE;
	point3f local_3;
	if (arg_1->output_index == NONE || !function_2104b0(arg_1->output_index, &arg_1->point, &local_3))
	{
		local_3 = arg_1->point;
	}
	long local_4 = local_0->unknown018;
	byte *local_5 = (byte *)object_get(local_4);
	s_255b10 *local_6 = (s_255b10 *)(local_5 + *(short *)(local_5 + 0x12a));
	long local_7 = local_6->field_70;
	if (local_7 == NONE)
	{
		local_7 = 0x6000086;
	}
	c_type_709360 local_8 = ((s_graph_tag *)g_4e3b44[local_6->field_68 & 0xffff].bytes)->overlay_get(
		local_7, local_6->field_74, local_6->field_78, 0xd000021, NULL, NULL, NULL);
	if ((local_8.index == NONE && !function_10f340(local_4, 0x7000101, 0xd000021)) || local_0->unknown07c == NONE)
		return false;
	point2f local_9;
	local_9.x = local_3.x - local_0->position.x;
	local_9.y = local_3.y - local_0->position.y;
	if (!(normalize2d(&local_9) > 0.f &&
		local_0->unknown290.i * local_9.x + local_0->unknown290.j * local_9.y >= 0.f))
		return false;
	byte *local_10 = (byte *)function_1e5380(arg_0);
	if (!local_10 || !function_1ff6c0(arg_0, arg_2, &local_3, &local_2))
		return false;
	short local_11;
	if (function_255d60(arg_0, &local_3, *(real *)(local_10 + 0xc), *(real *)(local_10 + 0x20), &local_11))
		return false;
	return function_1ffa30(arg_1, arg_0, arg_2, local_2, arg_3);
}

// @retail 0x255ce0
bool function_255ce0(long arg_0)
{
	s_actor_view *local_0 = actor_get(arg_0);
	byte *local_1 = (byte *)object_get(local_0->unknown018);
	bool local_2 = false;
	if (!function_110ab0(local_0->unknown018) && *(real *)(local_1 + 0xf8) == 0.f)
	{
		s_255ce0 *local_3 = (s_255ce0 *)local_0;
		local_2 = function_255b10(arg_0, &local_3->field_7d0, local_3->field_7e0, local_3->field_7cc);
	}
	return local_2;
}

long function_1e4a50(long arg_0);
void function_26bfa0(long object_index, long *location_index, s_location_view *location);
bool function_10fa80(long unit_index, long mode, long set, short *event_ticks, real *distance, short *duration_ticks, real *event_distance);
short __stdcall function_255740(long arg_0, s_slot *arg_1);

s_slot_handler_0 g_47f7dc = {8, 0, 0, -2, 0, function_255740};

PRIVATE __forceinline real function_255741(vector3f const *arg_0, vector3f const *arg_1)
{
	real local_0 = arg_0->i * arg_1->i;
	local_0 += arg_0->j * arg_1->j;
	local_0 += arg_0->k * arg_1->k;
	return local_0;
}

// @retail 0x255740
short __stdcall function_255740(long arg_0, s_slot *arg_1)
{
	s_actor_view *local_0 = actor_get(arg_0);
	if (local_0->unknown5ac != NONE && local_0->unknown5b0 == 2 &&
		!function_110ab0(local_0->unknown018) && local_0->unknown5d0)
	{
		long local_1 = local_0->unknown5ac;
		s_slot_object_view *local_2 = object_get(local_1);
		vector3f local_3 = local_0->unknown5ec;
		vector3f local_4 = local_0->unknown290;
		local_3.k = 0.f;
		local_4.k = 0.f;
		real local_5 = function_30bf0(&local_3);
		if (local_5 > 0.f && function_30bf0(&local_4) > 0.f)
		{
			real local_6 = *(real *)((byte *)function_1e4a50(local_0->unknown054) + 4);
			vector3f local_7;
			vector3d_from_points3d(&local_0->position, &local_2->unknown030, &local_7);
			real local_8 = function_255741(&local_7, &local_3);
			if (local_8 > 0.f)
			{
				real local_9 = local_2->unknown03c + local_6;
				if (local_9 + local_5 > local_8)
				{
					vector3f local_10;
					local_10.i = 0.f - local_3.j;
					local_10.j = local_3.i;
					local_10.k = 0.f;
					real local_11 = local_7.j * local_10.j + local_7.i * local_10.i + local_7.k * local_10.k;
					if (fabs(local_11) < local_9 * 0.6f)
					{
						s_slot_object_view *local_12 = object_get(local_0->unknown018);
						if (local_12->type == 0)
							local_8 -= *(real *)(g_4e3b44[local_12->tag_index & 0xffff].bytes + 0x270);
						local_8 -= local_2->unknown03c;
						if (local_4.i * local_3.i + local_4.j * local_3.j + local_4.k * local_3.k > 0.95f)
						{
							real local_13 = 0.f;
							real local_14 = 0.f;
							long local_15;
							s_location_view local_16;
							function_26bfa0(local_1, &local_15, &local_16);
							local_4.i = local_10.i * -1.f;
							local_4.j = local_10.j * -1.f;
							local_4.k = -0.f;
							volatile short local_17;
							if (function_2556b0(&local_16.point.point, &local_10, local_15, &local_13) && local_11 > 0.f)
								local_17 = 1;
							else if (function_2556b0(&local_16.point.point, &local_4, local_15, &local_14) && local_11 < 0.f)
								local_17 = 2;
							else if ((local_13 > local_14 ? local_13 : local_14) > 0.5f)
								local_17 = local_13 + local_11 > local_14 - local_11 ? 1 : 2;
							else
								local_17 = 0;
							long local_18;
							switch (local_17)
							{
							case 0:
							case 1:
								local_18 = 0xa00022d;
								break;
							case 2:
								local_18 = 0xb00022e;
								break;
							}
							short local_19, local_21;
							real local_20, local_22;
							real local_23;
							if (function_10fa80(local_0->unknown018, local_18, 0x7000101, &local_19, &local_20, &local_21, &local_22))
								local_23 = local_20 * 1.33f;
							else
								local_23 = 0.4f;
							if (local_23 > local_8)
							{
								s_unit_request local_24;
								local_24.type = 0x29;
								*(short *)local_24.arguments = local_17;
								*(long *)(local_24.arguments + 4) = local_0->unknown5ac;
								function_e6900(local_0->unknown018, &local_24);
							}
						}
					}
				}
			}
		}
	}
	return g_470b4c;
}
