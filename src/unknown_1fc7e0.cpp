// @flags /O2 /arch:SSE /Gr
// BLOCKED-BY the full check (#574, batch r294): enabling this body loses matched 0x1c83c0 and 0x1e2f50
// under whole-program optimisation; it does not byte-match yet. fecheck: behaves like retail on 6/6 in-game
// snapshots (caller 0x1e34e0 pushes actor_index; ret 4). The stub in src/stubs/unknown_0a76b0.cpp stays.
#if 0
/* UNKNOWN_1FC7E0.CPP: one tick of an actor's combat: its state (+0x6fe), its
   target, its aim and the firing outputs (+0x810, +0x820, +0x824) that
   function_1e4390 hands to the unit (see docs/unknown_1fc7e0.md) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "unknown_0259a0.h"
#include "slot_handler.h"
#include "props.h"
#include "unknown_20fe20.h"
#include "unknown_1fb7e0.h"
#include "unknown_1e1f20.h"
#include "unknown_1e3920.h"
#include <math.h>

/* the actor (0x888 bytes) as its combat update reads it */
struct s_actor_combat_view
{
	byte unknown000[0x18];
	long unit_index;
	byte unknown01c[0x24 - 0x1c];
	short team;
	byte unknown026[0x7c - 0x26];
	long clump_object_index;
	byte unknown080[0x86 - 0x80];
	short alert_state;
	byte unknown088[0x223 - 0x88];
	bool unknown223;
	byte unknown224[0x229 - 0x224];
	bool unknown229;
	byte unknown22a[0x22c - 0x22a];
	point3f look_position;
	byte unknown238[0x264 - 0x238];
	bool unknown264;
	bool unknown265;
	byte unknown266;
	bool unknown267;
	byte unknown268[0x26c - 0x268];
	long unknown26c;
	byte unknown270[0x338 - 0x270];
	long prop_index;
	byte unknown33c[0x488 - 0x33c];
	bool unknown488;
	bool unknown489;
	byte unknown48a;
	bool unknown48b;
	bool unknown48c;
	byte unknown48d[0x490 - 0x48d];
	s_type_c3b527 unknown490;
	bool unknown4a0;
	bool unknown4a1;
	bool unknown4a2;
	byte unknown4a3[0x5d0 - 0x4a3];
	bool unknown5d0;
	byte unknown5d1[3];
	bool unknown5d4;
	byte unknown5d5[0x620 - 0x5d5];
	short value620;
	byte unknown622[0x6d0 - 0x622];
	bool unknown6d0;
	byte unknown6d1[0x6f8 - 0x6d1];
	bool aiming;
	byte unknown6f9[0x6fc - 0x6f9];
	word unknown6fc;
	short state;
	short timer;
	short unknown702;
	short shot_delay;
	short blocked_ticks;
	short special_cooldown;
	short unknown70a;
	bool special_pending;
	bool second_barrel;
	bool special_second_barrel;
	byte unknown70f;
	real blend;
	bool unknown714;
	byte unknown715[3];
	long unknown718;
	byte unknown71c[4];
	word unknown720;
	short target_type;
	union
	{
		long prop_reference;
		s_type_c3b527 point;
	} target;
	long target_ticks;
	bool unknown738;
	bool unknown739;
	bool unknown73a;
	bool unknown73b;
	bool unknown73c;
	byte unknown73d;
	short visibility;
	bool unknown740;
	byte unknown741[3];
	s_type_c3b527 aim_position;
	real target_distance;
	vector3f aim_direction;
	real aim_distance;
	s_type_c3b527 aim_base;
	point3f aim_point_base;
	vector3f aim_offset;
	vector3f aim_offset_step;
	point3f aim_point;
	bool unknown7a8;
	byte unknown7a9[3];
	real unknown7ac;
	vector3f unknown7b0;
	byte unknown7bc[0x7c4 - 0x7bc];
	short unknown7c4;
	byte unknown7c6[0x810 - 0x7c6];
	dword flags810;
	byte unknown814[0x820 - 0x814];
	real first_scale;
	real second_scale;
	byte unknown828[0x888 - 0x828];
};
typedef char assert_actor_combat_view_size[(sizeof(s_actor_combat_view) == 0x888) ? 1 : -1];

static inline s_actor_combat_view *actor_combat_get(long actor_index)
{
	return (s_actor_combat_view *)(g_4f55f0->data + (actor_index & 0xffff) * sizeof(s_actor_combat_view));
}

/* the unit object as the update reads it */
struct s_combat_unit_view
{
	byte unknown000[0x14];
	long parent_index;
	byte unknown018[0xaa - 0x18];
	byte type;
	byte unknown0ab[0x134 - 0xab];
	dword flags_134;
	byte unknown138[0x1fc - 0x138];
	short parent_seat_index;
	byte unknown1fe[0x2c4 - 0x1fe];
	real unknown2c4;
};

static inline s_combat_unit_view *combat_unit_get(long object_index)
{
	return (s_combat_unit_view *)((s_moving_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

/* a ramp of the blend factor per difficulty */
struct s_combat_ramp
{
	real start;
	real end;
	real seconds;
};

/* the character's entry for the weapon it holds (function_1e5280) */
struct s_combat_weapon_entry
{
	dword flags;
	byte unknown04[0xc - 0x4];
	real range;
	real minimum_range;
	byte unknown14[0x1c - 0x14];
	real unknown1c;
	byte unknown20[0x78 - 0x20];
	short special_mode;
	short type;
	real special_chance;
	real special_delay;
	byte unknown84[0x98 - 0x84];
	s_combat_ramp difficulty[3];
};

/* the character's entry for the unit's grenade type (function_1e5380) */
struct s_combat_grenade_entry
{
	byte unknown00[4];
	short grenade_type;
};

/* the weapon definition's barrels */
struct s_combat_weapon_data
{
	byte unknown000[0x2d0];
	long barrel_count;
	byte *barrels;
};

struct s_combat_blend_values
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
	real field_10;
	real field_14;
	real field_18;
	real field_1c;
	real field_20;
	real field_24;
	real field_28;
	real field_2c;
	real field_30;
	real field_34;
	real field_38;
	real field_3c;
};

/* doubles the rate of fire of actors that are enemies of team 1 in a
   campaign game (cf. g_4f55e4 and g_4f55e6, damage.cpp) */
bool g_4f55e9 = false;

struct s_type_67e06b;
struct s_combat_delay;
struct s_combat_wait_range;

long function_1e1fd0(long actor_index);
void *function_1e5380(long actor_index);
s_type_67e06b *function_1fe0e0(long actor_index);
void function_1e4220(long actor_index, bool enabled);
bool function_1e2030(long actor_index);
void function_1fc710(long actor_index, point3f *position);
bool function_255ce0(long arg_0);
short function_cdff0(long unit_index, short grenade_type);
short function_cccd0(long unit_index, short grenade_type, short delta);
bool function_1e4310(long actor_index);
void function_26b770(long clump_index);
long function_113da0(long unit_index);
bool function_e4020(long arg_159e6d);
real function_1e9720(short kind, short team);
bool function_1fdbf0(long actor_index, short type);
bool function_1fdce0(long arg_0, short arg_1);
bool function_200110(long arg_0, long arg_1, point3f *arg_2, s_type_c3b527 *arg_3);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
bool function_1ffbd0(long actor_index, long weapon_index, short barrel_index,
	point3f const *origin, point3f const *target, bool flag, vector3f *direction,
	void *unknown0, void *unknown1, void *unknown2, void *unknown3);
bool function_1f50e0(long actor_index);
bool function_298b60(long actor_index);
bool function_298bc0(long actor_index);
void function_1fe120(long actor_index, vector3f *direction);
bool function_255d60(long arg_0, point3f const *arg_1, real arg_2, real arg_3, short *arg_4);
bool function_1fe1e0(long actor_index, real blend, s_combat_blend_values *out);
void function_1fed70(long actor_index, s_combat_wait_range const *range);
bool function_1fee20(long actor_index, s_combat_delay const *delay);
void function_1fe620(long arg_0, byte const *arg_1, byte const *arg_2);
real function_1e9700(short row);
real weapon_barrel_estimate_time_to_target(long weapon_index, short barrel_index, real distance);
real function_30bf0(vector3f *v);
bool function_1c8d20(long actor_index, long excluded_object, point3f const *origin,
	vector3f const *direction, long *blocking_index);

/* whether a ray from the actor's eyes reaches the target point: 0 when
   nothing is hit, 2 when the hit is short of a unit of distance, else 4 */
static inline short actor_target_visibility(s_actor_combat_view *actor, point3f const *target)
{
	s_collision_result_1697c0 collision;
	short visibility;
	long flags;
	vector3f vector;

	collision.unknown24 = NONE;
	flags = actor->unknown26c != NONE ? 0x15808c0f : 0x15808c2f;
	vector.i = target->x - actor->look_position.x;
	vector.j = target->y - actor->look_position.y;
	vector.k = target->z - actor->look_position.z;
	if (!function_1697c0(flags, &actor->look_position, &vector, NONE, NONE, &collision))
	{
		visibility = 0;
	}
	else
	{
		real dx = target->x - actor->look_position.x;
		real dy = target->y - actor->look_position.y;
		real dz = target->z - actor->look_position.z;
		real distance = (real)sqrt(dx * dx + dy * dy + dz * dz);

		/* the hit's fraction of the ray (+4) */
		if (1.0f > distance || !(1.0f > distance * *(real *)(collision.unknown00 + 4)))
			visibility = 4;
		else
			visibility = 2;
	}
	return visibility;
}

/* one tick of the actor's combat */
// Retail 0x1fc7e0, disabled with its blocker above.
void __stdcall function_1fc7e0(long actor_index)
{
	/* retail keeps actor_index on the stack (ret 4); its caller pushes it */
	long const *actor_on_stack = &actor_index;
	s_actor_combat_view *actor = actor_combat_get(*actor_on_stack);
	long weapon_index = function_1e1f20(actor_index);
	long secondary_weapon_index = function_1e1fd0(actor_index);
	s_combat_weapon_entry const *entry = NULL;
	s_combat_weapon_data const *weapon_tag_data = NULL;
	s_combat_grenade_entry const *grenade_entry = (s_combat_grenade_entry const *)function_1e5380(actor_index);
	s_combat_unit_view *unit = NULL;
	short next_state = NONE;
	bool may_advance = false;
	bool not_facing = false;
	bool changed = false;
	bool evaluated = false;
	bool in_range_check;
	short old_type;
	point3f origin;
	point3f aim_position;
	s_combat_blend_values blend;

	if (actor->unit_index != NONE)
		unit = combat_unit_get(actor->unit_index);
	actor->unknown6fc = 0;
	if (actor->unknown5d0)
	{
		actor->unknown714 = true;
		actor->unknown720 = 0;
	}
	if (weapon_index != NONE)
	{
		byte const *weapon = (byte const *)moving_object_get(weapon_index);

		weapon_tag_data = (s_combat_weapon_data const *)function_1fe0e0(actor_index);
		entry = (s_combat_weapon_entry const *)function_1e5280(actor_index, *(long const *)weapon);
		if ((weapon[0x16c] >> 1) & 1)
			actor->unknown6fc |= 1;
	}

	if (actor->unknown488)
		function_1e4220(actor_index, false);
	else if (actor->unknown4a1)
		function_1e4220(actor_index, true);
	else if (actor->unknown4a2)
		function_1e4220(actor_index, false);
	else
		function_1e4220(actor_index, actor->alert_state < 6);

	in_range_check = function_1e2030(actor_index);
	if (actor->timer > 0)
		actor->timer--;
	if (actor->unknown702 > 0)
		actor->unknown702--;
	if (actor->shot_delay > 0)
		actor->shot_delay--;
	if (actor->special_cooldown > 0)
		actor->special_cooldown--;
	old_type = actor->target_type;
	if (old_type > 0)
		actor->target_ticks++;
	if (actor->value620 > 0)
		actor->value620--;

	if (unit && unit->parent_index != NONE && unit->parent_seat_index != NONE &&
		((*(dword const *)(*(byte * const *)(g_4e3b44[*(long const *)combat_unit_get(unit->parent_index) & 0xffff].bytes + 0x1cc) +
			unit->parent_seat_index * 0xb0) >> 12) & 1) &&
		((combat_unit_get(actor->unit_index)->flags_134 >> 25) & 1))
	{
		actor->target_type = 0;
		function_1fb7e0(0x38, actor_index, NULL, NONE, NONE);
	}
	else if (actor->state != 2)
	{
		short type = 0;

		if (actor->unknown488)
		{
			if (actor->unknown48c)
				type = 2;
			else if (actor->prop_index != NONE)
				type = 1;
		}
		if (type != old_type)
			changed = true;
		else if (type == 1)
			changed = actor->prop_index != actor->target.prop_reference;
		else if (type == 2)
			changed = function_210a30(&actor->target.point, &actor->unknown490) > 0.25f;
		if (changed)
			actor->target_ticks = 0;
		actor->target_type = type;
		if (type == 1)
			actor->target.prop_reference = actor->prop_index;
		else if (type == 2)
			actor->target.point = actor->unknown490;
	}

	actor->unknown740 = false;
	function_1fc710(actor_index, &origin);

	if (actor->unknown7c4 > 0)
	{
		if (function_255ce0(actor_index))
		{
			if (grenade_entry)
			{
				long unit_index = actor->unit_index;
				short grenade_type = grenade_entry->grenade_type;

				if (function_cdff0(unit_index, grenade_type) == 0)
					function_cccd0(unit_index, grenade_type, 1);
			}
			if (function_1e4310(actor_index))
			{
				function_1fb7e0(0x1c, actor_index, NULL, NONE, NONE);
				function_26b770(actor->clump_object_index);
				actor->unknown7c4 = 0;
				goto reset;
			}
		}
		actor->unknown7c4--;
		goto reset;
	}

	if (actor->value620 > 0 || !function_1e2030(actor_index) || actor->unknown6d0 ||
		!function_113da0(actor->unit_index) || actor->unknown26c != NONE && actor->unknown5d4 ||
		unit && unit->type == 0 && function_e4020(actor->unit_index))
	{
		goto reset;
	}
	if (actor->state == 4)
	{
		may_advance = true;
		goto proximity;
	}
	if (actor->unknown223 || !entry)
		goto reset;
	if (unit && !actor->unknown267 && (entry->flags & 2) && 1.0f > unit->unknown2c4)
		goto reset;

	if (entry->special_mode > 0 && actor->state != 2 && actor->special_cooldown <= 0 && actor->unknown70a <= 0)
	{
		bool allowed = true;

		function_1e9720(0x12, actor->team);
		switch (entry->special_mode)
		{
		case 1:
			function_1e9720(0x11, actor->team);
			allowed = weapon_tag_data->barrel_count > 0;
			break;
		case 2:
			allowed = weapon_tag_data->barrel_count > 1;
			break;
		}
		if (allowed && function_1fdbf0(actor_index, entry->type))
		{
			real seconds = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 0.0f, 1.5f) + entry->special_delay;
			real chance = function_259a0(&g_4e7408->unknown0);

			actor->special_cooldown = (short)function_1469f0(seconds);
			if (entry->special_chance > chance && function_1fdce0(actor_index, entry->type))
			{
				if (entry->type == 3)
					actor->unknown70a = 3;
				switch (entry->special_mode)
				{
				case 1:
					actor->special_pending = true;
					break;
				case 2:
					actor->special_second_barrel = true;
					break;
				}
			}
		}
	}

	if (actor->target_type > 0)
	{
		long period = real_to_long(g_510c54->field_2_3 * 0.33f);

		if (actor->target_type == 1)
		{
			long prop_reference = actor->target.prop_reference;
			s_prop_datum *datum = (s_prop_datum *)(g_502418->data + (prop_reference & 0xffff) * sizeof(s_prop_datum));
			s_type_5cfb45 *state = function_25d690(datum);
			s_type_f95cd3 *view = function_25d740((s_prop_node *)datum);

			actor->target_distance = datum->unknown28;
			function_200110(actor_index, prop_reference, &aim_position, &actor->aim_position);
			evaluated = true;
			if (view)
				actor->visibility = view->unknown06;
			else if (actor->target_ticks % period == 0)
				actor->visibility = actor_target_visibility(actor, &aim_position);
			actor->unknown739 = state->unknown69;
			actor->unknown73c = true;
			if (state->location.cluster_index != NONE)
			{
				long cluster = state->location.cluster_index;
				actor->unknown73c = (g_4e6948->active_clusters[cluster >> 5] & (1 << (cluster & 31))) == 0;
			}
		}
		else
		{
			actor->aim_position = actor->target.point;
			actor->target_distance = function_210ac0(&actor->target.point, &actor->look_position);
			actor->unknown739 = false;
			actor->unknown73c = false;
			function_210850(&actor->aim_position, &aim_position);
			evaluated = true;
			if (actor->target_ticks % period == 0)
				actor->visibility = actor_target_visibility(actor, &aim_position);
		}

		actor->unknown73a = false;
		actor->unknown73b = actor->unknown489 && entry->unknown1c > 0.0f;
		if (!function_1ffbd0(actor_index, weapon_index, 0, &origin, &aim_position, false,
			&actor->aim_direction, NULL, &actor->aim_distance, NULL, NULL))
		{
			actor->target_type = 0;
		}
	}

	if (actor->target_type == 0 || actor->unknown73c)
		goto reset;
	if (!actor->unknown48b && actor->unknown702 > 0)
		goto reset;
	if (function_1f50e0(actor_index))
		goto reset;
	if (!actor->unknown48b && actor->unknown264 && !actor->unknown229)
		goto reset;
	if (actor->unknown739 || actor->unknown265 || !actor->aiming)
		goto reset;
	if (!actor->unknown48c && (!function_298b60(actor_index) || !function_298bc0(actor_index)))
		goto reset;
	if (actor->state == 2)
	{
		may_advance = true;
		goto proximity;
	}
	actor->unknown738 = actor->visibility == 0 || actor->visibility == 1;
	if (!actor->unknown4a0 && !actor->unknown738 && !actor->unknown73b)
		goto reset;
	if (!actor->unknown48b && in_range_check && actor->target_distance >= entry->range)
		goto reset;
	if (!actor->unknown48b && in_range_check && entry->minimum_range >= actor->target_distance)
		goto reset;
	{
		real threshold;
		vector3f facing;

		may_advance = true;
		actor->unknown740 = true;
		if (1.5f > actor->target_distance)
			threshold = actor->target_distance * 0.17526217f + 0.70710677f;
		else
			threshold = 0.97f;
		function_1fe120(actor_index, &facing);
		if (threshold > actor->aim_direction.j * facing.j + actor->aim_direction.k * facing.k + facing.i * actor->aim_direction.i)
			not_facing = true;
	}

proximity:
	if (actor->target_type > 0 && evaluated && weapon_tag_data->barrel_count > 0)
	{
		long projectile_index = *(long const *)(weapon_tag_data->barrels + 0x90);

		if (projectile_index != NONE)
		{
			real unknown_radius = *(real const *)(g_4e3b44[projectile_index & 0xffff].bytes + 0xd0);

			if (unknown_radius > 0.0f && function_255d60(actor_index, &aim_position, 0.0f, unknown_radius + 1.0f, NULL))
				goto reset;
		}
	}
	goto transition;

reset:
	may_advance = false;
	actor->state = 0;

transition:
	{
		short state = actor->state;

		switch (state)
		{
		case 0:
			if (may_advance)
				next_state = 1;
			break;
		case 1:
		case 3:
			if (!not_facing && actor->timer == 0)
				next_state = 2;
			break;
		case 2:
			if (actor->timer == 0)
				next_state = 3;
			break;
		case 4:
			if (actor->timer == 0)
				next_state = 0;
			break;
		}

		if (entry)
		{
			short difficulty = g_4e6948->state == 1 ? g_4e6948->difficulty : 1;
			s_combat_ramp const *ramp;
			real factor;

			switch (difficulty)
			{
			case 2:
				ramp = &entry->difficulty[1];
				break;
			case 3:
				ramp = &entry->difficulty[2];
				break;
			default:
				ramp = &entry->difficulty[0];
				break;
			}
			if (changed || state == 1 || state == 4 || state == 0)
				actor->blend = ramp->start;
			else
				actor->blend += (ramp->end - ramp->start) / ((real)g_510c54->field_2_3 * ramp->seconds);
			if (0.0f > actor->blend)
				factor = 0.0f;
			else if (actor->blend > ramp->end)
				factor = ramp->end;
			else
				factor = actor->blend;
			actor->blend = factor;
			if (!function_1fe1e0(actor_index, factor, &blend))
			{
				if (actor->state == 0)
					goto aim;
				next_state = 0;
			}
		}

		if (next_state != NONE)
		{
			switch (next_state)
			{
			case 1:
				if (!function_1fee20(actor_index, (s_combat_delay const *)entry) && !not_facing)
				{
					next_state = 2;
					function_1fe620(actor_index, (byte const *)&blend, (byte const *)entry);
				}
				break;
			case 2:
				function_1fe620(actor_index, (byte const *)&blend, (byte const *)entry);
				break;
			case 3:
				function_1fed70(actor_index, (s_combat_wait_range const *)&blend);
				break;
			}
			actor->state = next_state;
		}
	}

aim:
	if (!entry)
		return;
	{
		bool primary_clear = false;
		bool secondary_clear = false;
		bool fire = false;
		bool second_barrel = false;
		real scale;

		actor->unknown7a8 = false;
		if (actor->state == 4)
		{
			primary_clear = true;
			actor->unknown718 = NONE;
		}
		else if (actor->state == 2)
		{
			long ignore_object = NONE;
			real distance_squared;
			real speed;
			bool linear;
			vector3f aim;
			long blocker;

			function_210850(&actor->aim_base, &actor->aim_point_base);
			if (actor->target_type == 1)
			{
				s_type_5cfb45 *state = function_25d690(
					(s_prop_datum *)(g_502418->data + (actor->target.prop_reference & 0xffff) * sizeof(s_prop_datum)));
				real follow;

				ignore_object = state->unknown40;
				follow = function_1e9700(15) + blend.field_4;
				if ((follow >= 1.0f || follow > 0.0f) && !actor->unknown73b)
				{
					point3f base;
					point3f prop_point;

					function_210850(&actor->aim_base, &base);
					function_200110(actor_index, actor->target.prop_reference, &prop_point, NULL);
					actor->aim_point_base.x += (prop_point.x - base.x) * blend.field_4;
					actor->aim_point_base.y += (prop_point.y - base.y) * blend.field_4;
					actor->aim_point_base.z += (prop_point.z - base.z) * blend.field_4;
				}
				if (blend.field_8 >= 1.0f || blend.field_8 > 0.0f)
				{
					/* the prop's velocity */
					vector3f const *velocity = (vector3f const *)((byte const *)state + 0x1c);
					real time = weapon_barrel_estimate_time_to_target(weapon_index, actor->second_barrel != 0, actor->aim_distance);

					actor->aim_point_base.x += velocity->i * time * blend.field_8;
					actor->aim_point_base.y += velocity->j * time * blend.field_8;
					actor->aim_point_base.z += velocity->k * time * blend.field_8;
				}
			}

			actor->aim_offset.i += actor->aim_offset_step.i;
			actor->aim_offset.j += actor->aim_offset_step.j;
			actor->aim_offset.k += actor->aim_offset_step.k;
			actor->aim_point.x = actor->aim_point_base.x + actor->aim_offset.i;
			actor->aim_point.y = actor->aim_point_base.y + actor->aim_offset.j;
			actor->aim_point.z = actor->aim_point_base.z + actor->aim_offset.k;
			if (actor->second_barrel)
				function_1fc710(actor_index, &origin);

			{
				real dx = actor->aim_point.x - origin.x;
				real dy = actor->aim_point.y - origin.y;
				real dz = actor->aim_point.z - origin.z;

				distance_squared = dz * dz + dy * dy + dx * dx;
			}
			if (entry->minimum_range * entry->minimum_range > distance_squared)
			{
				vector3f direction;

				direction.i = actor->aim_point.x - origin.x;
				direction.j = actor->aim_point.y - origin.y;
				direction.k = actor->aim_point.z - origin.z;
				if (function_30bf0(&direction) > 0.0f)
				{
					double push = entry->minimum_range - sqrt(distance_squared);

					actor->aim_point.x = (real)(direction.i * push + actor->aim_point.x);
					actor->aim_point.y = (real)(direction.j * push + actor->aim_point.y);
					actor->aim_point.z = (real)(direction.k * push + actor->aim_point.z);
				}
			}

			function_1ffbd0(actor_index, weapon_index, actor->second_barrel != 0, &origin, &actor->aim_point,
				actor->unknown73a, &actor->unknown7b0, NULL, NULL, &speed, &linear);
			actor->unknown7a8 = !linear;
			if (actor->unknown7a8)
				actor->unknown7ac = speed;

			aim.i = actor->aim_point.x - origin.x;
			aim.j = actor->aim_point.y - origin.y;
			aim.k = actor->aim_point.z - origin.z;
			if (function_1c8d20(actor_index, ignore_object, &origin, &aim, &blocker))
			{
				actor->blocked_ticks = 0;
				if (actor->second_barrel)
					secondary_clear = true;
				else
					primary_clear = true;
			}
			else
			{
				if (actor->blocked_ticks < g_510c54->field_2_3 * 60)
					actor->blocked_ticks++;
				actor->timer++;
			}
		}

		scale = 0.0f;
		if (primary_clear)
		{
			real rate = blend.field_0;

			if (g_4e6948->state == 1 && g_4f55e9 && team_is_enemy(1, actor->team))
				rate *= 2.0f;
			if (actor->special_pending)
			{
				actor->special_pending = false;
			}
			else if (0.0001f > rate)
			{
				fire = true;
			}
			else if (actor->shot_delay == 0)
			{
				short ticks;

				fire = true;
				ticks = (short)function_1469f0(1.0f / rate);
				actor->shot_delay = ticks < 2 ? 2 : ticks;
			}
		}
		else if (secondary_clear)
		{
			if (actor->special_pending)
				actor->special_pending = false;
			else
				second_barrel = true;
		}
		else if (actor->special_pending)
		{
			fire = true;
		}

		if (fire)
		{
			scale = 1.0f;
			actor->flags810 |= 0x10000;
		}
		else
		{
			actor->flags810 &= ~0x10000;
		}
		actor->first_scale = scale;
		if (second_barrel)
			actor->flags810 |= 0x20000;
		else
			actor->flags810 &= ~0x20000;
		if (secondary_weapon_index != NONE)
		{
			if (fire)
				actor->flags810 |= 0x200000;
			else
				actor->flags810 &= ~0x200000;
			actor->second_scale = scale;
			if (second_barrel)
				actor->flags810 |= 0x400000;
			else
				actor->flags810 &= ~0x400000;
		}
	}
}
#endif
