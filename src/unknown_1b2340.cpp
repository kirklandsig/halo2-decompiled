// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_26c380.h"

static __forceinline real slot_random(void);
static __forceinline real slot_random_range(real lower, real upper);
static __forceinline long real_to_long(real value);

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* slot type 0x24 */

short __stdcall function_1b2630(long actor_index, s_slot *slot, bool active);
bool __stdcall function_1b23c0(long actor_index, s_slot *slot);
bool __stdcall function_1b2770(long actor_index, s_slot *slot);
void __stdcall function_1b2bb0(long actor_index, s_slot *slot);
bool function_26ba60(long prop_index, long actor_index, long clump_index);
void function_25d420(long prop_ref_index, short type, long actor_index);

// @retail 0x1b2340
short __stdcall function_1b2340(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	short result = 0;

	if (actor->unknown007)
		return 0;

	if (actor->prop_index != NONE)
	{
		s_prop_node_view *node = prop_node_get(actor->prop_index);

		if (prop_node_view(node) && node->unknown24 >= 3)
			result = 3;
	}
	return result;
}

/* slot type 0x24's data: the block after the slot's header (s_slot's unknownc) */
struct s_slot_24_data
{
	bool unknown00;
	bool unknown01;
	byte unknown02[8 - 2];
	short unknown08;
	short unknown0a;
	short unknown0c;
	byte unknown0e[0x34 - 0xe];
};

/* the block of the actor's character tag function_1e4db0 returns */
struct s_character_db0
{
	dword flags;
};

void *function_1e4db0(long actor_index);
short function_2684f0(s_actor_view *actor);

// @retail 0x1b2630
short __stdcall function_1b2630(long actor_index, s_slot *slot, bool active)
{
	s_actor_view *actor = actor_get(actor_index);
	s_slot_24_data *state = (s_slot_24_data *)slot->unknownc;
	short result = g_46fbe8;

	if (state->unknown00)
	{
		if (actor->prop_index != NONE)
		{
			s_prop_node_view *node = prop_node_get(actor->prop_index);
			s_prop_view_fields *view = prop_node_view(node);

			if (view && function_26ba60(node->unknown08, actor_index, actor->unknown07c))
			{
				if (view->unknown70 == 0)
					function_1fb7e0(0x2e, actor_index, NULL, node->object_index, NONE);
				else
					function_1fb7e0(0x30, actor_index, NULL, node->object_index, NONE);
			}
		}
		result = g_46fbe4;
	}
	else if (actor->prop_index == NONE || !prop_node_view(prop_node_get(actor->prop_index)))
	{
		result = g_46fbe4;
	}
	return result;
}

s_slot_handler_2 g_47e080 ={
	{
		0x24, 2, 0, -2, 0,
		function_1b2340, function_1b2630, function_1b23c0, 0, NONE, {0},
		0, 0, 0, 0, 0, 0, 0
	},
	(t_slot_proc)function_1b2770, 0, function_1b2bb0
};

short __stdcall function_1c8df0(short type, short team, point3f const *point, long object_index,
	void const *origin, long count, bool unknown, bool unknown2, bool unknown3, long *arg_9);
void function_1e3b00(long object_index, long mode, point3f const *reference, void const *unknown0,
	void const *unknown1, point3f *position);
short __stdcall function_272af0(s_match_globals *structure, point3f const *point);
bool function_1f86f0(long actor_index);
bool __stdcall function_1f4280(long actor_index);
long function_1fa7f0(void);
bool function_1f4460(long actor_index, s_type_c3b527 const *point, long target_index, long unknown, bool unknown2);

/* slot type 0x24's update: moves the actor to its prop (to the point the
   prop names, or along its direction) and flags the slot when it is there
   or cannot get there (always returns true) */
/* Retail looks the prop up through a second actor lookup and tests type 0 on the field itself. */
// @retail 0x1b2770
bool __stdcall function_1b2770(long actor_index, s_slot *slot)
{
	s_slot_24_data *state = (s_slot_24_data *)slot->unknownc;
	s_actor_view *actor = actor_get(actor_index);
	s_prop_node_view *node = prop_node_get(actor_get(actor_index)->prop_index);
	s_prop_view_fields *view = prop_node_view(node);
	s_type_c3b527 target;
	short type;
	bool result;

	if (actor->unknown504 == 2)
	{
		state->unknown00 = true;
		return true;
	}
	if (actor->unknown040)
	{
		state->unknown00 = false;
		if (view->unknown70 == 1)
		{
			if (function_210b60(&view->unknown78, &actor->position) > 4.0f)
			{
				state->unknown00 = false;
			}
			else
			{
				if (function_1f86f0(actor_index))
				{
					state->unknown00 = true;
				}
				else
				{
					point3f point;
					point3f position;
					short cluster;

					function_210850(&view->unknown78, &point);
					function_1e3b00(actor->unknown018, 1, &point, NULL, NULL, &position);
					cluster = view->unknown78.output_index == NONE ?
						*(short *)((byte *)view + 0x72) : function_272af0(g_4e0348, &point);
					state->unknown00 = function_1c8df0(*(short *)((byte *)actor + 0x254), cluster, &position, NONE,
						(byte *)actor + 0x22c, 0, false, actor->unknown26c != NONE, true, NULL) == 0;
				}
				if (state->unknown00)
				{
					function_1f4280(actor_index);
					return true;
				}
			}
		}
		type = view->unknown70;
		if (view->unknown70 == 0)
		{
			vector3f *direction = (vector3f *)((byte *)view + 0x94);
			s_prop_state_view *prop_entry = prop_node_state(node);
			s_sector_trace_result trace;

			if ((real)sqrt(direction->i * direction->i + direction->j * direction->j + direction->k * direction->k) > 0.0f &&
				(function_26c590((s_pathfinding_data const *)function_1fa7f0(), &prop_entry->unknown48.point,
					*(long *)((byte *)prop_entry + 0x44), NONE, direction, 1.8f, NULL, (s_path_trace_result *)&trace),
					!trace.blocked || trace.distance > 0.3f))
			{
				real distance = trace.distance;

				if (distance > 1.5f)
					distance = 1.5f;
				distance -= 0.3f;
				point3f const *origin = &prop_entry->unknown48.point;
				target.point.x = direction->i * distance + origin->x;
				target.point.y = direction->j * distance + origin->y;
				target.point.z = direction->k * distance + origin->z;
				target.output_index = prop_entry->unknown48.output_index;
				result = function_1f4460(actor_index, &target, trace.sector_index, NONE, false);
			}
			else
			{
				result = function_1f4810(actor_index, actor->prop_index, 0.7f, false);
				target = prop_entry->unknown48;
			}
			actor->unknown4cc = 0.7f;
		}
		else if (type == 1)
		{
			function_2628f0(actor_index, g_470fa0);
			result = function_1f4460(actor_index, &view->unknown78, *(long *)((byte *)view + 0x74), NONE, false);
			target = view->unknown78;
		}
		else
		{
			function_1f4280(actor_index);
			result = true;
		}
		if (!result)
		{
			point3f point;
			vector3f v;

			function_210850(&target, &point);
			vector3d_from_points3d(&point, &actor->position, &v);
			if ((v.j * v.j + v.k * v.k) + v.i * v.i < 1.0f)
			{
				function_1f4280(actor_index);
				state->unknown00 = true;
			}
			else if (actor->unknown229 || actor->unknown5b4 >= (actor->unknown26c == NONE ? 32 : 3))
			{
				if (view->unknown70 == 0)
					function_25d420(actor->prop_index, 3, actor_index);
				state->unknown00 = true;
			}
		}
	}
	return true;
}

// @retail 0x1b2bb0
void __stdcall function_1b2bb0(long actor_index, s_slot *slot)
{
	s_slot_24_data *state = (s_slot_24_data *)slot->unknownc;
	s_actor_view *actor = actor_get(actor_index);

	if (actor->prop_index != NONE)
	{
		s_prop_view_fields *view = prop_node_view(prop_node_get(actor->prop_index));
		s_character_db0 *character;

		actor->unknown4a2 = true;
		if (actor->unknown5d0)
		{
			actor->unknown41c = 3;
			actor->unknown420 = 0;
		}
		else if (state->unknown0c - state->unknown0a < MAX(state->unknown0c / 3, real_to_long((real)g_510c54->field_2_3 * 3.0f)))
		{
			if (view)
			{
				if (view->unknown70 == 0)
				{
					actor->unknown41c = 3;
					actor->unknown420 = 2;
				}
				else
				{
					point3f point;

					function_210850(&view->unknown78, &point);
					actor->unknown41c = 3;
					actor->unknown420 = 3;
					actor->unknown424.point = point;
				}
			}
		}
		if (view && view->unknown70 == 0)
			actor->unknown488 = function_2684f0(actor) >= 5;
		character = (s_character_db0 *)function_1e4db0(actor_index);
		if (character)
		{
			if (character->flags & 1)
			{
				actor->unknown44a = true;
				actor->unknown449 = true;
			}
			else if ((character->flags & 2) && view && actor->unknown086 <= 3)
			{
				actor->unknown483 = true;
			}
		}
	}
}
bool function_2601f0(long actor_index, s_type_c3b527 const *point, long sector, real radius, real path_distance);
void function_1f86a0(long actor_index);

// @retail 0x1b23c0
bool __stdcall function_1b23c0(long actor_index, s_slot *slot)
{
    volatile bool result = true;
    s_actor_view *actor = actor_get(actor_index);
    s_slot_24_data *state = (s_slot_24_data *)slot->unknownc;
    s_prop_node_view *node = prop_node_get(actor->prop_index);
    s_prop_state_view *local_5cfb45 = prop_node_state(node);
    s_prop_view_fields *view = prop_node_view(node);
    real duration;
    if (!view)
        goto failed;
    if (view->unknown70 == 0)
    {
        duration = 3.0f;
        if (*(long *)((byte *)local_5cfb45 + 0x44) == NONE ||
            !function_2601f0(actor_index, &local_5cfb45->unknown48, *(long *)((byte *)local_5cfb45 + 0x44),
                actor->unknown26c != NONE ? 6.0f : 3.0f, 0.0f))
        {
            function_25d420(actor->prop_index, 3, actor_index);
            goto failed;
        }
    }
    else
    {
        byte *character = (byte *)function_1e4db0(actor_index);
        duration = character ? function_259d0(&g_4e7408->unknown0, NULL, 0, *(real *)(character + 4), *(real *)(character + 8)) : 8.0f;
        if (!function_2601f0(actor_index, &view->unknown78, *(long *)((byte *)view + 0x74),
            actor->unknown26c == NONE ? 2.0f : 5.0f, 0.0f))
            goto failed;
    }
    long ticks = real_to_long((real)g_510c54->field_2_3 * duration);
    state->unknown0a = (short)ticks;
    state->unknown0c = (short)ticks;
    state->unknown08 = 0;
    state->unknown00 = false;
    state->unknown01 = false;
    if (function_26ba60(node->unknown08, actor_index, actor->unknown07c))
    {
        if (view->unknown70 == 0)
        {
            byte *prop = g_50241c->data + (node->unknown08 & 0xffff) * 0xc4;
            if (!prop[0x32])
                prop[0x32] = function_1fb7e0(0x31, actor_index, NULL, node->object_index, NONE);
        }
        else
            function_1fb7e0(0x36, actor_index, NULL, node->object_index, NONE);
    }
    function_1f86a0(actor_index);
    return result;
failed:
    return false;
}
