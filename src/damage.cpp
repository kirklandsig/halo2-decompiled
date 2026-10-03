// @flags /O2 /Ob1 /arch:SSE /Gr
/* DAMAGE.CPP: object damage

The functions follow damage.obj in Bungie's May 2003 builds
(halo-symbol-atlas); the profile build's order and sizes are closest to
retail. */

#include "cseries.h"
#include "globals.h"
#include "data_array.h"
#include "real_math.h"
#include "effects.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_1dee50.h"
#include <math.h>
#include <string.h>

typedef long string_id;

/* a tag block: a count and the elements' address */
template <typename t_element>
struct s_tag_block_of
{
	long count;
	t_element *elements;
};

/* the damage table (the game globals' at +0xd4): per damage group, the
   multiplier each armor type applies */
struct s_armor_modifier
{
	string_id armor;
	real multiplier;
};

struct s_damage_group
{
	string_id name;
	s_tag_block_of<s_armor_modifier> armor_modifiers;
};

struct s_damage_table
{
	s_tag_block_of<s_damage_group> damage_groups;
};

/* the object fields the damage code reads (g_4e0300 holds 12 byte headers
   with the object at +8) */
struct s_damage_object
{
	long tag_index;
	union
	{
		byte unknown04[4];
		struct
		{
			dword unknown0 : 26;
			dword permutation_child : 1;
			dword : 5;
		} flags04;
	};
	byte unknown08[4];
	long next_object_index;
	long first_child_object_index;
	long parent_object_index;
	byte unknown18[0x30 - 0x18];
	real_point3d bounding_sphere_center;
	real bounding_sphere_radius;
	byte unknown40[0xaa - 0x40];
	byte type;
	byte unknownab[0xb4 - 0xab];
	long node_index;
	byte unknownb8[0xc2 - 0xb8];
	short owner_team;
	long owner_player_index;
	long owner_object_index;
	byte unknowncc[0xd4 - 0xcc];
	long unknownd4;
	byte unknownd8[0xe0 - 0xd8];
	word unknowne0;
	word unknowne2;
	real maximum_body_vitality;
	real maximum_shield_vitality;
	real body_vitality;
	real shield_vitality;
	real unknownf4;
	real unknownf8;
	real unknownfc;
	real unknown100;
	short shield_stun_ticks;
	short body_stun_ticks;
	byte unknown108;
	byte unknown109;
	struct
	{
		word unknown0 : 1;
		word shield_damaged : 1;
		word body_depleted : 1;
		word shield_depleted : 1;
		word shield_double_charged : 1;
		word unknown5 : 1;
		word unknown6 : 1;
		word unknown7 : 1;
		word body_recharging : 1;
		word shield_recharging : 1;
		word unknown10 : 1;
		word unknown11 : 1;
		word unknown12 : 1;
		word unknown13 : 1;
		word unknown14 : 1;
	} damage_flags;
	byte unknown10c[0x120 - 0x10c];
	short region_states_size;
	short region_states_offset;
	byte unknown124[0x12c - 0x124];
	long unknown12c;
	byte unknown130[0x138 - 0x130];
	short team;
	byte unknown13a[2];
	long player_index;
	byte unknown140[0x168 - 0x140];
	real_vector3d unknown168;
	byte unknown174[0x1fc - 0x174];
	short unknown1fc;
	byte unknown1fe[0x248 - 0x1fe];
	long unknown248;
};

struct s_damage_object_header
{
	byte unknown00[8];
	s_damage_object *object;
};

#define DAMAGE_OBJECT(index) (((s_damage_object_header *)g_4e0300->data)[(index) & 0xffff].object)

/* who is responsible for damage */
struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

/* a damage event (0x88 bytes) */
struct damage_data
{
	long definition_index;
	union
	{
		byte unknown04[4];
		dword flags;
	};
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	long unknown1c;
	short unknown20;
	short unknown22;
	real_point3d position;
	real_point3d origin;
	real_vector3d direction;
	real_vector3d node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	real_vector3d cone_direction;
	real unknown78;
	short unknown7c;
	short unknown7e;
	byte unknown80[4];
	bool unknown84;
	byte unknown85[3];
};

extern short g_47d8e0;

s_damage_owner const g_440564 = { NONE, NONE, NONE };
s_damage_owner const *g_467420 = &g_440564;

struct s_damage_info;

/* what damaging an object collects (0x34 bytes; object_cause_damage's
   per-object state, passed down to the shield, body and region code) */
struct s_damage_region_accumulator
{
	void *unknown00;
	s_damage_info *damage_info;
	short unknown08;
	byte unknown0a[2];
	long unknown0c;
	long unknown10;
	long unknown14;
	real unknown18;
	real damage;
	real shield_damage;
	real unknown24;
	real unknown28;
	short unknown2c;
	short unknown2e;
	dword flags;
};

/* the damage definition (a jpt! tag's fields from +0x10) */
struct s_damage_effect_definition
{
	short side_effect;
	short category;
	dword flags;
	byte unknown08[0x40 - 0x8];
	string_id damage_group;
	string_id damage_group_b;
};

/* a damage info region (0x38 bytes) */
/* a damage info region's permutation (0x50 bytes) */
struct s_damage_info_permutation
{
	short unknown00;
	word unknown02;
	dword flags;
	real unknown08;
	byte unknown0c[4];
	long unknown10;
	byte unknown14[4];
	long unknown18;
	byte unknown1c[4];
	short unknown20;
	short unknown22;
	long unknown24;
	long unknown28;
	real delay;
	byte unknown30[4];
	long effect_index;
	long unknown38;
	string_id unknown3c;
	string_id unknown40;
	real skip_chance;
	string_id unknown48;
	real body_threshold;
};

struct s_damage_info_region
{
	string_id name;
	dword flags;
	real vitality;
	long permutation_count;
	s_damage_info_permutation *permutations;
	byte unknown14[0x24 - 0x14];
	real damage_level_delay;
	byte unknown28[0x34 - 0x28];
	short unknown34;
	byte unknown36[2];
};

/* an object's per-region damage state (8 bytes, at object + object->+0x122) */
struct s_object_region_state
{
	word destroyed_permutations;
	byte unknown02;
	byte unknown03;
	word pending;
	short damage_level_ticks;
};

struct s_damage_info
{
	dword flags;
	byte unknown04[4];
	short default_region_index;
	byte unknown0a[7];
	byte unknown11;
	byte unknown12[0x2c - 0x12];
	real body_stun_damage_threshold;
	real body_stun_time;
	byte unknown34[0x94 - 0x34];
	real shield_stun_damage_threshold;
	real shield_stun_time;
	byte unknown9c[4];
	real shield_damaged_threshold;
	byte unknowna4[4];
	long shield_damaged_effect;
	byte unknownac[0xbc - 0xac];
	long region_count;
	s_damage_info_region *regions;
	long unknownc4;
	byte *unknownc8;
	short shield_material;
	short unknownce;
	byte unknownd0[8];
	long unknownd8;
	byte *unknowndc;
	long unknowne0;
	byte *unknowne4;
};

/* the object child iterator (unknown_0d0690.cpp) */
struct s_object_child_iterator
{
	long root;
	long current;
	long next;
	long child_value;
	long child_index;
	short child_short;
};

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool game_team_is_enemy(short team_a, short team_b);

/* the object header data's elements (g_4e0300, 12 bytes) */
struct s_damage_object_datum
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	s_damage_object *object;
};

/* the object definition tag fields read here */
struct s_damage_object_definition
{
	byte unknown00[0xbc];
	struct
	{
		dword unknown0 : 1;
		dword can_be_instant_killed : 1;
	} flags;
};

/* the damage definition tag (jpt!) fields read here */
struct s_damage_definition
{
	real minimum_radius;
	real radius04;
	byte unknown08[4];
	dword flags0c;
	byte unknown10[4];
	dword flags14;
	byte unknown18[0x28 - 0x18];
	real cone_inner_angle;
	real cone_outer_angle;
	byte unknown30[0x44 - 0x30];
	real unknown44;
	real unknown48;
	real unknown4c;
	byte unknown50[0x58 - 0x50];
	real radius58;
	byte unknown5c[0x64 - 0x5c];
	real player_radius;
	real radius68;
};

/* the players (g_4e8c24, 0x21c byte elements): the unit at +0x2c */
struct s_damage_player
{
	byte unknown00[0x2c];
	long unit_index;
};

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef CEILING
#define CEILING(n,ceiling) ((n)>(ceiling)?(ceiling):(n))
#endif
#ifndef FLOOR
#define FLOOR(n,floor) ((n)<(floor)?(floor):(n))
#endif
#ifndef PIN
#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : CEILING((n),(ceiling)))
#endif

enum
{
	k_maximum_area_of_effect_objects = 64
};

real function_1e9700(short row);
real function_259a0(dword *seed);

bool function_d0690(s_object_child_iterator *iterator);
void function_d0620(long object_index, s_object_child_iterator *iterator);
void function_b7360(long object_index);
void __stdcall function_b8540(long a);
void function_b8b70(long object_index);
void object_destroy_region(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	s_damage_region_accumulator *accumulator);
void function_da110(long permutation_index, s_damage_info *info, long object_index, s_damage_owner const *owner,
	long region_index, s_damage_region_accumulator *accumulator);
void function_d9d60(bool at_marker, long marker_name, long object_index, long effect_index, s_damage_owner const *owner);
void function_ba690(long object_index, byte **states, long *state_count, long *a, long *b);
void function_a8360(long object_index, long region_index, long permutation_index, bool a);
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c);
void object_cause_damage(damage_data *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	real_vector3d const *unknown14);
void function_dbc80(long object_index, short section_mask_a, short section_mask_b);
void __stdcall function_e6460(long object_index);
void function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b, real_point3d const *origin, real_vector3d const *direction);
void __stdcall function_ba7f0(long object_index, long a, long b, long c);
short __stdcall function_bb050(long a, dword type_mask, void const *location, real_point3d const *position, real radius,
	long *objects, short maximum_count);
void area_of_effect_cause_damage_to_object(damage_data *data, long object_index, bool child);
real function_30bf0(real_vector3d *v);
void function_baff0(long object_index, real_point3d const *origin, real_point3d *closest_point, real_vector3d *normal);
bool function_d6f90(long object_index, real_point3d const *point, damage_data const *data);
long unit_get_player_index(long unit_index);
void __stdcall function_153d10(short team, long definition_index, void *a, void *b, long c, real d, real e, long f);
bool function_d74b0(byte const *owner);

void __stdcall function_184250(damage_data const *data);

long function_d5b60(long object_index);
real function_1e9720(long kind, short team);
bool function_108fd0(long object_index);
void function_b58c0(long index, dword mask);

typedef long (__stdcall *t_bsearch_compare_function)(const void *, const void *, const void *);
long bsearch_elements(const void *key, const void *base, long count, long element_size, t_bsearch_compare_function compare, const void *context);
long __stdcall cache_tag_group_compare(void const *a, void const *b, void const *context);

// @retail 0xd5bc0
real damage_armor_table_lookup(string_id group_a, string_id group_b, string_id armor_a, string_id armor_b)
{
	s_damage_table *table = g_4e034c->damage_table;
	real result = 1.0f;
	string_id groups[2] = { group_a, group_b };
	string_id armors[2] = { armor_a, armor_b };

	for (dword i = 0; i < 2; i++)
	{
		string_id group_key = groups[i];
		long group_index = bsearch_elements(&group_key, table->damage_groups.elements, table->damage_groups.count,
			sizeof(s_damage_group), cache_tag_group_compare, NULL);

		if (group_index == NONE)
			continue;

		s_damage_group *group = &table->damage_groups.elements[group_index];
		for (dword j = 0; j < 2; j++)
		{
			string_id armor_key = armors[j];
			long modifier_index = bsearch_elements(&armor_key, group->armor_modifiers.elements, group->armor_modifiers.count,
				sizeof(s_armor_modifier), cache_tag_group_compare, NULL);

			if (modifier_index != NONE)
				result *= group->armor_modifiers.elements[modifier_index].multiplier;
		}
	}
	return result;
}

// @retail 0xd5c90
void object_initialize_vitality(long object_index, real *body_vitality, real *shield_vitality)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	byte *damage_info = (byte *)function_d5b60(object_index);
	real maximum_body = 0.0f;
	real maximum_shield = 0.0f;

	if (damage_info)
	{
		maximum_body = *(real *)(damage_info + 0x28);
		maximum_shield = *(real *)(damage_info + 0x8c);
	}
	if (body_vitality)
		maximum_body = *body_vitality;
	if (shield_vitality)
		maximum_shield = *shield_vitality;

	object->maximum_body_vitality = maximum_body;
	object->maximum_shield_vitality = maximum_shield;
	object->body_vitality = maximum_body > 0.0f ? 1.0f : 0.0f;
	object->shield_vitality = maximum_shield > 0.0f ? 1.0f : 0.0f;
}

// @retail 0xd5d20
real object_get_maximum_body_vitality(long object_index, bool ignore_difficulty)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real result = object->maximum_body_vitality;

	if (!ignore_difficulty && ((1 << object->type) & 3))
		result = function_1e9720(1, object->team) * result;
	return result;
}

// @retail 0xd5d80
real object_get_maximum_shield_vitality(long object_index, bool ignore_difficulty)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real result = object->maximum_shield_vitality;

	if (!ignore_difficulty && ((1 << object->type) & 3))
		result = function_1e9720(2, object->team) * result;
	return result;
}

// @retail 0xd6660
void damage_data_new(damage_data *data, long definition_index)
{
	memset(data, 0, sizeof(*data));
	data->definition_index = definition_index;
	data->unknown7c = g_47d8e0;
	data->owner.object_index = NONE;
	data->owner.player_index = NONE;
	data->owner.team = NONE;
	data->unknown14 = NONE;
	data->unknown1c = NONE;
	data->unknown20 = NONE;
	data->unknown22 = g_4686c4;
	data->unknown18 = NONE;
	data->unknown54 = 1.0f;
	data->unknown58 = 1.0f;
	data->unknown5c = 1.0f;
	data->unknown7e = NONE;
	data->unknown84 = false;
}

// @retail 0xd6790
bool object_restore_body(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool result = false;

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted) && object->body_vitality < 1.0f)
	{
		object->body_vitality = 1.0f;
		if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
			function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 0x40);
		result = true;
	}
	return result;
}

// @retail 0xd6af0
bool object_double_charge_shield(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (object->shield_vitality <= 1.0f)
	{
		bool result = true;

		object->damage_flags.shield_double_charged = true;
		if (object->shield_vitality == 0.0f)
			object->shield_vitality = 0.01f;
		object->shield_stun_ticks = 0;
		return result;
	}
	return false;
}

// @retail 0xd6b60
void object_destroy_notify_children(long object_index)
{
	long child_index = DAMAGE_OBJECT(object_index)->first_child_object_index;

	while (child_index != NONE)
	{
		long next_index = DAMAGE_OBJECT(child_index)->next_object_index;

		if (!function_108fd0(child_index))
			object_destroy_notify_children(child_index);
		child_index = next_index;
	}
}

/* new since 2003: who an object's damage is credited to */
// @retail 0xd66d0
void object_get_damage_owner(long object_index, s_damage_owner *owner)
{
	if (object_index == NONE)
	{
		*owner = *g_467420;
		return;
	}

	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if ((1 << object->type) & 3)
	{
		if (object->unknown248 != NONE)
		{
			object_get_damage_owner(object->unknown248, owner);
			return;
		}
		if (object->unknown12c != NONE || object->player_index != NONE)
		{
			owner->object_index = object_index;
			owner->player_index = object->player_index;
			owner->team = object->team;
			return;
		}
	}
	owner->object_index = object->owner_object_index;
	owner->player_index = object->owner_player_index;
	owner->team = object->owner_team;
}

// @retail 0xd6a70
void object_deplete_shield(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (!TEST_FIELD_BIT(object->damage_flags.shield_depleted))
	{
		long damage_info = function_d5b60(object_index);

		if (damage_info && object->maximum_shield_vitality > 0.0f)
			function_176780(object_index, (s_effect_owner const *)g_467420, 0.0f, *(long *)(damage_info + 0xb0), 0.0f, NULL, NULL);
		object->unknownf4 = 0.0f;
		object->damage_flags.shield_depleted = true;
		function_ba7f0(object_index, NONE, 2, NONE);
	}
}

// @retail 0xd6800
void object_deplete_body(long object_index, s_damage_owner const *owner, bool notify_parent, bool unknown)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_region_accumulator accumulator;

	if (TEST_FIELD_BIT(object->damage_flags.body_depleted))
		return;

	object->damage_flags.body_depleted = true;
	function_b8b70(object_index);
	memset(&accumulator, 0, sizeof(accumulator));
	if (unknown)
		accumulator.flags |= 0x400;
	else
		accumulator.flags &= ~0x400;

	if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
		function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 1);

	if (g_4e6948->mode != 4)
	{
		s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

		if (info)
		{
			for (long i = 0; i < info->region_count; i++)
			{
				if (info->regions[i].flags & 2)
					object_destroy_region(info, object_index, owner, i, &accumulator);
			}
		}
	}

	if (g_4e6948->mode != 4 && object->type == 1)
	{
		for (long child_index = object->first_child_object_index; child_index != NONE;)
		{
			s_damage_object *child = DAMAGE_OBJECT(child_index);

			if (child->type == 0 && child->unknown1fc != NONE)
				function_dbfb0(child_index, owner, false, false, false);
			child_index = child->next_object_index;
		}
	}

	object_deplete_shield(object_index);

	if (g_4e6948->mode != 4 && object->parent_object_index != NONE && notify_parent)
	{
		s_damage_object *parent = DAMAGE_OBJECT(object->parent_object_index);

		if (TEST_FIELD_BIT(parent->damage_flags.unknown12) && ((1 << parent->type) & 2))
		{
			s_object_child_iterator iterator;
			bool last = true;

			function_d0620(object->parent_object_index, &iterator);
			while (function_d0690(&iterator))
			{
				if (iterator.child_short != NONE && iterator.child_index != object_index)
					last = false;
			}
			if (last)
			{
				parent->damage_flags.unknown13 = true;
				function_b7360(object->parent_object_index);
			}
		}
	}

	if (accumulator.unknown2c || accumulator.unknown2e)
		function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);
	if ((accumulator.flags & 4) && g_4e6948->mode != 4)
		function_b8540(object_index);
}

// @retail 0xd6bc0
void object_destroy(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_region_accumulator accumulator;

	object_deplete_body(object_index, g_467420, true, false);

	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

	memset(&accumulator, 0, sizeof(accumulator));
	if (info)
	{
		for (long i = 0; i < info->region_count; i++)
		{
			if (info->regions[i].flags & 8)
				object_destroy_region(info, object_index, g_467420, i, &accumulator);
		}
	}
	if (object->type == 0)
		function_e6460(object_index);
	object_destroy_notify_children(object_index);
	function_b8540(object_index);
}

/* whether a damage event can affect an object */
// @retail 0xd72e0
bool function_d72e0(long object_index, damage_data const *data)
{
	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *object = datum->object;
	bool result = false;

	if ((datum->flags & 0x10) || (object->unknown04[0] & 1))
		return result;

	if (definition->flags0c & 2)
	{
		s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);
		long player_index = unit ? unit->player_index : NONE;

		return player_index != NONE;
	}

	dword flags = definition->flags14;
	if ((flags & 1) && object_index == data->owner.object_index)
		return result;

	if ((1 << object->type) & 3)
	{
		if ((flags & 0x8000) && g_4e6948->state == 1 && object->player_index != NONE)
			return result;
		if ((flags & 8) && !game_team_is_enemy(object->team, data->owner.team))
			return result;
	}
	return true;
}

/* whether any entry of the block at +0x70 (0x60 byte entries) has the flag
   at +0x40 of the structure it points to */
// @retail 0xd74b0
bool function_d74b0(byte const *owner)
{
	bool result = false;

	for (long i = 0; i < *(long const *)(owner + 0x74); i++)
	{
		if ((*(byte const *const *)(*(byte const *const *)(owner + 0x70) + i * 0x60 + 0x40))[0x40])
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0xd7ae0
long get_player_index_from_object_or_parents(long object_index)
{
	long result = NONE;

	while (object_index != NONE)
	{
		s_damage_object_datum *datum = (s_damage_object_datum *)datum_get_inlined(g_4e0300, object_index);

		if (datum && ((1 << datum->type) & 3) && datum->object)
		{
			s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);

			return unit ? unit->player_index : NONE;
		}
		object_index = DAMAGE_OBJECT(object_index)->parent_object_index;
	}
	return result;
}

/* the creature instant-kill roll: clears *instant_kill unless the damage may
   kill the object outright, and returns the outcome */
// @retail 0xd73c0
bool function_d73c0(long object_index, damage_data const *data, bool *instant_kill)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	bool result = false;

	if (*instant_kill && (definition->flags14 & 0x1000))
	{
		*instant_kill = result;
		if (((1 << object->type) & 0x1000) &&
			TEST_FIELD_BIT(((s_damage_object_definition *)g_4e3b44[object->tag_index & 0xffff].bytes)->flags.can_be_instant_killed) &&
			object_index != data->owner.object_index)
		{
			real chance = function_1e9700(8);

			*instant_kill = true;
			if ((definition->flags14 & 0x400) && (data->unknown04[0] & 0x40))
				*instant_kill = false;
			if (chance > 0.0f && function_259a0((dword *)g_4e7408) < chance * 0.5f)
			{
				*instant_kill = false;
				return result;
			}
			if (*instant_kill)
				return true;
		}
	}
	return result;
}

/* damages everything in the damage's radius; returns the first player unit
   hit, else the last object hit */
// @retail 0xd6c80
long area_of_effect_cause_damage(damage_data *data, long ignore_object_index)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	real radius = MAX(definition->radius04, MAX(definition->radius58, definition->radius68));
	long objects[k_maximum_area_of_effect_objects];
	long object_count = function_bb050(0, (definition->flags0c & 2) ? 3 : 0, &data->unknown1c, &data->position, radius,
		objects, k_maximum_area_of_effect_objects);
	long first_object_index = NONE;
	long last_object_index = NONE;

	*(dword *)data->unknown04 |= 1;

	if (definition->player_radius > radius)
	{
		long player_index = NONE;

		while ((player_index = data_next_absolute_index_inlined(g_4e8c24, player_index + 1)) != NONE)
		{
			s_damage_player *player = (s_damage_player *)(g_4e8c24->data + g_4e8c24->size * player_index);

			if (!player)
				break;
			if (player->unit_index == NONE)
				continue;

			long root_index = player->unit_index;
			while (DAMAGE_OBJECT(root_index)->parent_object_index != NONE)
				root_index = DAMAGE_OBJECT(root_index)->parent_object_index;

			s_damage_object *root = DAMAGE_OBJECT(root_index);
			if (object_count >= k_maximum_area_of_effect_objects)
				continue;

			real dx = root->bounding_sphere_center.x - data->position.x;
			real dy = root->bounding_sphere_center.y - data->position.y;
			real dz = root->bounding_sphere_center.z - data->position.z;
			real inner = root->bounding_sphere_radius + radius;
			if (inner * inner >= dz * dz + dy * dy + dx * dx)
				continue;

			real outer = definition->player_radius + root->bounding_sphere_radius;
			if (outer * outer >= dz * dz + dy * dy + dx * dx)
				objects[object_count++] = player->unit_index;
		}
	}

	for (long i = 0; i < object_count; i++)
	{
		long object_index = objects[i];

		if (definition->flags0c & 2)
		{
			s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);
			if (!unit || unit->player_index == NONE)
				continue;
		}
		if (object_index == ignore_object_index)
			continue;

		s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);
		damage_data copy = *data;

		area_of_effect_cause_damage_to_object(&copy, object_index, false);
		last_object_index = object_index;
		if (unit)
		{
			if (first_object_index == NONE)
			{
				first_object_index = object_index;
			}
			else
			{
				s_damage_object *other = (s_damage_object *)function_badc0(object_index, 3);
				if (other && other->player_index != NONE)
					first_object_index = object_index;
			}
		}
	}

	if (g_4e6948->mode != 4)
		function_184250(data);

	return first_object_index != NONE ? first_object_index : last_object_index;
}

// @retail 0xd74e0
void area_of_effect_cause_damage_to_object(damage_data *data, long object_index, bool child)
{
	for (;;)
	{
		s_damage_object *object = DAMAGE_OBJECT(object_index);
		bool affects = function_d72e0(object_index, data);
		bool instant_kill = function_d73c0(object_index, data, &affects);

		if (affects)
		{
			long damage_info = function_d5b60(object_index);
			s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
			real_point3d closest_point;
			real_vector3d normal;
			bool outside = false;
			real scale;

			function_baff0(object_index, &data->origin, &closest_point, &normal);
			data->direction.i = closest_point.x - data->origin.x;
			data->direction.j = closest_point.y - data->origin.y;
			data->direction.k = closest_point.z - data->origin.z;
			data->distance = function_30bf0(&data->direction);

			if (object->node_index == NONE)
			{
				data->node_direction = data->direction;
			}
			else
			{
				byte *node = g_51e9b8->data + (object->node_index & 0xffff) * 0xa0;
				real_point3d center;

				if (node && *(long *)(node + 0x74) && !function_d74b0(node))
					center = *(real_point3d *)(*(byte **)(*(byte **)(*(byte **)(node + 0x70) + 0x40) + 0x3c) + 0x70);
				else
					center = object->bounding_sphere_center;

				data->node_direction.i = center.x - data->origin.x;
				data->node_direction.j = center.y - data->origin.y;
				data->node_direction.k = center.z - data->origin.z;
				function_30bf0(&data->node_direction);

				real dx = data->origin.x - closest_point.x;
				real dy = data->origin.y - closest_point.y;
				real dz = data->origin.z - closest_point.z;
				if (0.0025f > dz * dz + dy * dy + dx * dx)
				{
					data->node_direction.i += 0.0f - normal.i;
					data->node_direction.j += 0.0f - normal.j;
					data->node_direction.k += 0.0f - normal.k;
				}
				else
				{
					data->node_direction.i += data->direction.i;
					data->node_direction.j += data->direction.j;
					data->node_direction.k += data->direction.k;
					function_30bf0(&data->node_direction);
				}
			}

			if (definition->cone_outer_angle != 0.0f)
			{
				real_vector3d *cone = &data->cone_direction;

				if (fabs(1.0f - (cone->i * cone->i + cone->j * cone->j + cone->k * cone->k)) <= 0.0001f)
				{
					real dot = cone->k * data->direction.k + cone->j * data->direction.j + cone->i * data->direction.i;
					real angle = (real)acos(PIN(dot, -1.0f, 1.0f));

					if (definition->cone_inner_angle + 0.0001f <= angle)
					{
						if (definition->cone_outer_angle > angle)
						{
							data->unknown5c = PIN(1.0f - (angle - definition->cone_inner_angle) /
								(definition->cone_outer_angle - definition->cone_inner_angle), 0.0f, 1.0f);
						}
						else
						{
							scale = 0.0f;
							outside = true;
							goto distance_done;
						}
					}
				}
			}

			scale = 1.0f;
			if (definition->radius04 - definition->minimum_radius > 0.0f)
			{
				scale = 1.0f - (data->distance - definition->minimum_radius) / (definition->radius04 - definition->minimum_radius);
				if (0.0f > scale)
				{
					outside = true;
					scale = 0.0f;
				}
				else if (scale > 1.0f)
				{
					scale = 1.0f;
				}
			}

		distance_done:
			data->distance_scale = scale;
			data->unknown58 = scale;
			if (outside)
			{
				*(dword *)data->unknown04 |= 0x2000;
				data->unknown5c = 0.0f;
			}
			if (!(definition->flags0c & 1))
				data->unknown54 = scale;

			if (definition->player_radius > 0.0f)
			{
				s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);

				if (unit && unit->player_index != NONE)
					data->unknown58 = 1.0f - PIN(data->distance / definition->player_radius, 0.0f, 1.0f);
				else
					data->unknown58 = 0.0f;
			}

			data->in_unknown_radius = definition->radius68 > data->distance;
			if (scale > 0.0f || data->in_unknown_radius || data->unknown58 > 0.0f)
			{
				if (!child && function_d6f90(object_index, &closest_point, data))
					return;

				if (g_4e6948->mode == 4)
				{
					long definition_index = data->definition_index;

					if (((1 << object->type) & 3) && definition_index != NONE && unit_get_player_index(object_index) != NONE)
					{
						long player_index = unit_get_player_index(object_index);
						short team = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

						if (team != NONE)
						{
							real_vector3d direction;
							s_damage_owner owner = { NONE, NONE, NONE };

							direction.i = 0.0f - object->unknown168.i;
							direction.j = 0.0f - object->unknown168.j;
							direction.k = 0.0f - object->unknown168.k;
							function_153d10(team, definition_index, &owner, &direction, 0, 1.0f, 1.0f, 0);
						}
					}
				}
				else
				{
					object_cause_damage(data, object_index, NONE, NONE, NONE, NULL);
					if (instant_kill)
						*(dword *)data->unknown04 |= 0x40;
					if (damage_info && (*(byte *)damage_info & 8) && object->first_child_object_index != NONE)
						area_of_effect_cause_damage_to_object(data, object->first_child_object_index, true);
				}
			}
		}

		if (!child || object->next_object_index == NONE)
			return;
		child = true;
		object_index = object->next_object_index;
	}
}

/* the material of one of an object's model regions */
// @retail 0xd8ae0
void function_d8ae0(long object_index, long region_index, short *material)
{
	short result = g_47d8e0;

	if (object_index != NONE)
	{
		long model_index = *(long *)(g_4e3b44[DAMAGE_OBJECT(object_index)->tag_index & 0xffff].bytes + 0x38);

		if (model_index != NONE)
		{
			byte *regions = *(byte **)(g_4e3b44[model_index & 0xffff].bytes + 0x5c);

			*material = *(short *)(regions + region_index * 0x14 + 0x10);
			return;
		}
	}
	*material = result;
}

/* the bits of the entries (0x14 bytes, at +0xe4, counted at +0xe0) that
   match the target's value at +0x3c and are not masked */
// @retail 0xdaa50
void function_daa50(byte const *owner, byte const *target, word mask, word *bits)
{
	for (long i = 0; i < *(long const *)(owner + 0xe0); i++)
	{
		byte const *entry = *(byte const *const *)(owner + 0xe4) + i * 0x14;

		if (*(long const *)(entry + 8) == *(long const *)(target + 0x3c) && !(mask & (1 << i)))
			*bits |= (word)(1 << i);
	}
}

/* recent damage fades: after its delay (in seconds, from the damage info at
   +0x30/+0x38) each accumulator decays to zero over its decay time
   (+0x34/+0x3c); the timer stops at NONE once both are zero */
// @retail 0xd8b50
bool function_d8b50(real *recent_a, real *recent_b, char *timer, byte const *damage_info, bool reset)
{
	bool result = false;

	if (*timer != NONE)
	{
		real delay_a = 0.0f;
		real decay_a = 2.0f;
		real delay_b = 2.0f;
		real decay_b = 2.0f;

		if (damage_info)
		{
			delay_a = *(real const *)(damage_info + 0x30);
			decay_a = *(real const *)(damage_info + 0x34);
			delay_b = *(real const *)(damage_info + 0x38);
			decay_b = *(real const *)(damage_info + 0x3c);
		}

		*timer = (char)MIN(*timer + 1, 0x7f);

		if (*recent_a > 0.0f)
		{
			real seconds = g_510c54->ticks_per_second * delay_a;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			if (*timer >= ticks)
			{
				if (decay_a > 0.0001f)
				{
					*recent_a -= 1.0f / (g_510c54->ticks_per_second * decay_a);
					*recent_a = FLOOR(*recent_a, 0.0f);
				}
				else
				{
					*recent_a = 0.0f;
				}
			}
		}

		if (*recent_b > 0.0f)
		{
			real seconds = g_510c54->ticks_per_second * delay_b;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			if (*timer >= ticks)
			{
				if (decay_b > 0.0001f)
				{
					*recent_b -= 1.0f / (g_510c54->ticks_per_second * decay_b);
					*recent_b = FLOOR(*recent_b, 0.0f);
				}
				else
				{
					*recent_b = 0.0f;
				}
			}
		}

		if (reset)
			*recent_a = 0.0f;
		if ((reset || *recent_a == 0.0f) && *recent_b == 0.0f)
			*timer = NONE;
		result = true;
	}
	return result;
}

/* a weighted random choice among the entries function_daa50 matches: sets
   the chosen entry's bit */
// @retail 0xda860
void function_da860(byte const *owner, byte const *target, word mask, word *bits)
{
	long count = *(long const *)(owner + 0xe0);
	real total = 0.0f;
	long i;

	for (i = 0; i < count; i++)
	{
		byte const *entry = *(byte const *const *)(owner + 0xe4) + i * 0x14;

		if (!(mask & (1 << i)) && *(long const *)(entry + 8) == *(long const *)(target + 0x3c))
			total += *(real const *)(entry + 0xc);
	}

	real choice = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f) * total;
	real sum = 0.0f;

	for (i = 0; i < count; i++)
	{
		byte const *entry = *(byte const *const *)(owner + 0xe4) + i * 0x14;

		if (!(mask & (1 << i)) && *(long const *)(entry + 8) == *(long const *)(target + 0x3c))
		{
			sum += *(real const *)(entry + 0xc);
			if (sum >= choice)
			{
				*bits |= (word)(1 << i);
				return;
			}
		}
	}
}

/* how much of a vehicle's damage reaches its rider: the rider's seat's
   entry in the vehicle's damage info, scaled by the definition */
// @retail 0xdc230
real function_dc230(long rider_index, long vehicle_index, damage_data const *data)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *rider = DAMAGE_OBJECT(rider_index);
	s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);
	byte *damage_info = (byte *)function_d5b60(vehicle_index);
	real result = 1.0f;

	if (damage_info && ((1 << rider->type) & 3) && ((1 << vehicle->type) & 3) && !(definition->flags14 & 0x2000) &&
		rider->unknown1fc != NONE)
	{
		byte *seats = *(byte **)(g_4e3b44[vehicle->tag_index & 0xffff].bytes + 0x1cc);
		long seat_key = *(long *)(seats + rider->unknown1fc * 0xb0 + 4);
		long count = *(long *)(damage_info + 0xd8);
		byte *entries = *(byte **)(damage_info + 0xdc);

		for (long i = 0; i < count; i++)
		{
			if (*(long *)(entries + i * 0x14) == seat_key)
				return *(real *)(entries + i * 0x14 + 4) * definition->unknown44;
		}
	}
	return result;
}

/* damages an object with the globals' default damage (from +0x144), credited
   to an owner if one is given */
// @retail 0xdbfb0
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c)
{
	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.body_depleted))
		return;

	long definition_index = *(long *)(*(byte **)((byte *)g_4e034c + 0x144) + 0x14);
	if (definition_index == NONE)
		return;

	damage_data data;

	data.unknown7c = NONE;
	damage_data_new(&data, definition_index);
	data.unknown54 = 1.0f;
	if (owner)
		data.owner = *owner;

	dword flags = *(dword *)data.unknown04 | 4;
	if (a)
		flags |= 0x10;
	else
		flags &= ~0x10;
	if (b)
		flags |= 0x80;
	else
		flags &= ~0x80;
	if (c)
		flags |= 0x800;
	else
		flags &= ~0x800;
	*(dword *)data.unknown04 = flags;
	object_cause_damage(&data, object_index, NONE, NONE, NONE, NULL);
}

/* whether an object, or anything seated in or attached to it, is a unit (a
   player's unit, if players_only) */
// @retail 0xdb110
bool object_is_or_contains_player(long object_index, bool players_only, bool walk_siblings)
{
	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	s_damage_object *object = datum->object;
	bool result = false;

	if ((1 << datum->type) & 2)
	{
		s_object_child_iterator iterator;

		function_d0620(object_index, &iterator);
		while (function_d0690(&iterator))
		{
			if (iterator.child_short != NONE)
			{
				if (!players_only)
				{
					result = true;
					break;
				}

				s_damage_object *unit = (s_damage_object *)function_badc0(iterator.child_index, 3);
				if (unit && unit->player_index != NONE)
				{
					result = true;
					break;
				}
			}
		}
	}

	if (object->first_child_object_index != NONE)
	{
		if (result)
			result = true;
		else
			result = object_is_or_contains_player(object->first_child_object_index, players_only, true);
	}

	if (object->next_object_index != NONE && walk_siblings)
	{
		if (result)
			return true;
		return object_is_or_contains_player(object->next_object_index, players_only, true);
	}
	return result;
}

/* the globals' material table (+0x150 count, +0x154 elements of 0xb4 bytes) */
PRIVATE inline byte *global_material_get(short index)
{
	byte *result = 0;

	if (index != NONE && index >= 0 && index < *(long *)((byte *)g_4e034c + 0x150))
		result = *(byte **)((byte *)g_4e034c + 0x154) + index * 0xb4;
	return result;
}

/* the damage multiplier of a resistance against a source on a material: the
   resistance's scale (+0x1c), times the armor table's entry for the source's
   damage groups and the struck material's armor types */
// @retail 0xd9020
real function_d9020(long object_index, byte const *resistance, byte const *source, damage_data const *data)
{
	real result = *(real const *)(resistance + 0x1c);

	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.unknown7))
		result = 0.0f;
	if ((**(byte const *const *)(resistance + 4) & 0x20) && !(source[4] & 0x20))
		result = 0.0f;

	short material = data->unknown7c;
	if (global_material_get(material))
	{
		byte *armor_a = global_material_get(material);
		byte *armor_b = global_material_get(material);

		result = damage_armor_table_lookup(*(string_id const *)(source + 0x40), *(string_id const *)(source + 0x44),
			*(string_id *)(armor_b + 0x10), *(string_id *)(armor_a + 0x14)) * result;
	}
	return result;
}

/* destroys a damage region: picks the permutations whose chance and body
   threshold allow it, and destroys each now or schedules it after its
   delay */
// @retail 0xdae60
void object_destroy_region(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info_region *region = &info->regions[region_index];
	s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset) + region_index;

	for (long i = 0; i < region->permutation_count; i++)
	{
		s_damage_info_permutation *permutation = &region->permutations[i];

		if (state->destroyed_permutations & (1 << i))
			continue;
		if (permutation->unknown08 != 0.0f)
			continue;
		if (permutation->skip_chance != 0.0f &&
			permutation->skip_chance > (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f))
			continue;
		if (1.0f > permutation->body_threshold && (permutation->flags & 0x200000))
			continue;
		if (permutation->body_threshold > 1.0f && !(permutation->flags & 0x200000))
			continue;

		dword exclusive = accumulator->flags & 0x400;
		if (exclusive && (permutation->flags & 0x1000000))
			continue;
		if (!exclusive && (permutation->flags & 0x800000))
			continue;

		if (permutation->delay > 0.0f)
		{
			if ((state->pending & 0xfff8) && (state->pending & 7) != i)
				function_da110(state->pending & 7, info, object_index, owner, region_index, accumulator);

			state->pending ^= (state->pending ^ i) & 7;

			real seconds = g_510c54->ticks_per_second * permutation->delay;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			state->pending = (word)((state->pending & 7) | (ticks << 3));

			if (permutation->effect_index != NONE)
			{
				function_d9d60((permutation->flags >> 20) & 1, permutation->unknown38, object_index,
					permutation->effect_index, owner);
				function_a8360(object_index, region_index, i, true);
			}
		}
		else
		{
			function_da110(i, info, object_index, owner, region_index, accumulator);
		}
	}
	state->unknown02 = 0xff;
}

/* whether a permutation's model state allows it: its state index (+0x22)
   is out of range, or that state's level is below the permutation's (+0x20) */
// @retail 0xd9f70
bool function_d9f70(long region_index, long permutation_index, s_damage_info *info, long object_index)
{
	s_damage_info_permutation *permutation = &info->regions[region_index].permutations[permutation_index];
	byte *states;
	long state_count;
	long a;
	long b;
	bool result = true;

	function_ba690(object_index, &states, &state_count, &a, &b);
	short state_index = *(short *)((byte *)permutation + 0x22);
	if (state_index >= 0 && state_index < state_count &&
		(short)(char)states[state_index * 8 + 1] >= *(short *)((byte *)permutation + 0x20))
		return false;
	return result;
}

struct s_globals_element;
s_globals_element *function_188690(short index);
long function_1e4a10(long index);

/* damages an object's shield: scales the damage by the shield material's
   armor, takes it off the shield, passes what is left over to the body
   (accumulator->damage) and stuns the shield's recharge */
// @retail 0xd9110
void object_damage_shield(long object_index, s_damage_effect_definition const *definition, damage_data *data,
	s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real body_damage = accumulator->damage;
	real shield_damage = MAX(body_damage, 0.0f);

	object->shield_vitality = MAX(object->shield_vitality, 0.0f);
	if (object->shield_vitality == 0.0f)
		shield_damage = 0.0f;

	if (TEST_FIELD_BIT(object->damage_flags.shield_double_charged))
	{
		body_damage = 0.0f;
	}
	else if (!TEST_FIELD_BIT(object->damage_flags.unknown7))
	{
		real maximum_shield = object_get_maximum_shield_vitality(object_index, definition->category == 1);
		real inverse_maximum_shield = maximum_shield > 0.0f ? 1.0f / maximum_shield : 0.0f;
		byte *material = (byte *)function_188690(accumulator->damage_info->shield_material);
		real multiplier = 1.0f;

		if (!data->in_unknown_radius)
			body_damage -= shield_damage;
		if (shield_damage > 0.0f && material)
		{
			multiplier = damage_armor_table_lookup(definition->damage_group, definition->damage_group_b,
				*(string_id *)(material + 0x10), *(string_id *)(material + 0x14));
		}

		shield_damage = multiplier * shield_damage;
		accumulator->shield_damage = inverse_maximum_shield * shield_damage;
		object->shield_vitality -= accumulator->shield_damage;
		if (object->shield_vitality > 0.0f && definition->side_effect == 3 && data->in_unknown_radius)
			object->shield_vitality = 0.0f;

		if (object->shield_vitality <= 0.0f)
		{
			if (object->shield_vitality < 0.0f)
			{
				real overflow = (0.0f - object->shield_vitality) * maximum_shield;

				if (overflow > 0.0f && multiplier > 0.0001f)
					body_damage += overflow * (1.0f / multiplier);
				object->shield_vitality = 0.0f;
			}
			if (!TEST_FIELD_BIT(object->damage_flags.shield_depleted))
			{
				object_deplete_shield(object_index);
				accumulator->flags |= 8;
			}
		}

		if (!TEST_FIELD_BIT(object->damage_flags.shield_damaged) &&
			accumulator->damage_info->shield_damaged_threshold > object->shield_vitality)
		{
			function_176780(object_index, (s_effect_owner const *)&data->owner, 0.0f,
				accumulator->damage_info->shield_damaged_effect, 0.0f, NULL, NULL);
			object->damage_flags.shield_damaged = true;
		}

		if (shield_damage > accumulator->damage_info->shield_stun_damage_threshold ||
			object->shield_vitality == 0.0f && body_damage > 0.0001f ||
			definition->side_effect == 3 && data->in_unknown_radius)
		{
			real stun_time = accumulator->damage_info->shield_stun_time;

			if (((1 << object->type) & 3) && object->unknown12c != NONE)
			{
				byte *bounds = (byte *)function_1e4a10(*(long *)(g_4f55f0->data + (object->unknown12c & 0xffff) * 0x888 + 0x54));

				if (bounds)
					stun_time = *(real *)(bounds + 0x48);
			}
			if (object->shield_stun_ticks != 0x7fff)
			{
				real seconds = g_510c54->ticks_per_second * stun_time;
				long ticks;

				__asm
				{
					fld seconds
					fistp ticks
				}
				object->shield_stun_ticks = (short)ticks;
			}
		}
	}

	if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
		function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 0x80);
	accumulator->damage = body_damage;
}

/* damages a region: raises its damage level by the damage over the span to
   its next permutation's threshold, and when the level fills, destroys the
   permutations at that threshold */
// @retail 0xdaaa0
void apply_region_damage(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	real damage, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info_region *region = &info->regions[region_index];

	if (region_index >= 0 && region_index < object->region_states_size / (long)sizeof(s_object_region_state))
	{
		s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset) + region_index;
		real previous_threshold = 1.0f;
		real next_threshold = 0.0f;
		long i;
		s_damage_info_permutation *permutation;
		bool last_permutation = true;

		for (i = 0, permutation = region->permutations; i < region->permutation_count; i++, permutation++)
		{
			if (!(state->destroyed_permutations & (1 << i)))
			{
				next_threshold = permutation->unknown08;
				last_permutation = next_threshold == 0.0f;
				break;
			}
			previous_threshold = permutation->unknown08;
		}

		real fraction = damage / MAX(previous_threshold - next_threshold, 1.0f / 255.0f);

		if (fraction > 0.0f)
		{
			long level = (long)(fraction * 255.0f + (real)state->unknown02);

			state->unknown02 = (byte)PIN(level, 0, 255);

			real seconds = g_510c54->ticks_per_second * region->damage_level_delay;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			state->damage_level_ticks = (short)ticks;
		}

		if (state->unknown02 == 0xff)
		{
			state->unknown02 = 0;

			bool have_threshold = false;
			real threshold = 0.0f;

			for (i = 0; i < region->permutation_count; i++)
			{
				if (state->destroyed_permutations & (1 << i))
					continue;

				permutation = &region->permutations[i];
				bool blocked = false;

				if (!have_threshold)
				{
					have_threshold = true;
					threshold = permutation->unknown08;
				}
				else if (permutation->unknown08 != threshold)
				{
					break;
				}

				bool destroy = true;

				if (last_permutation && (region->flags & 4) && object_is_or_contains_player(object_index, false, false))
					destroy = false;
				if (!function_d9f70(region_index, i, info, object_index))
				{
					destroy = false;
					blocked = true;
				}
				if (damage > permutation->body_threshold)
				{
					if (permutation->flags & 0x200000)
					{
						destroy = false;
						blocked = true;
					}
				}
				else if (!(permutation->flags & 0x200000))
				{
					destroy = false;
					blocked = true;
				}
				if ((accumulator->flags & 0x400) ? (permutation->flags & 0x1000000) : (permutation->flags & 0x800000))
				{
					destroy = false;
					blocked = true;
				}

				if (permutation->skip_chance != 0.0f &&
					permutation->skip_chance > (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f))
				{
					state->destroyed_permutations |= 1 << i;
				}
				else if (destroy)
				{
					if (permutation->delay > 0.0f)
					{
						if ((state->pending & 0xfff8) && (state->pending & 7) != i)
							function_da110(state->pending & 7, info, object_index, owner, region_index, accumulator);

						state->pending ^= (state->pending ^ i) & 7;

						real seconds = g_510c54->ticks_per_second * permutation->delay;
						long ticks;

						__asm
						{
							fld seconds
							fistp ticks
						}
						state->pending = (word)((state->pending & 7) | (ticks << 3));

						if (permutation->effect_index != NONE)
						{
							function_d9d60((permutation->flags >> 20) & 1, permutation->unknown38, object_index,
								permutation->effect_index, owner);
							function_a8360(object_index, region_index, i, true);
						}
					}
					else
					{
						function_da110(i, info, object_index, owner, region_index, accumulator);
					}
				}
				else if (blocked)
				{
					state->destroyed_permutations |= 1 << i;
				}
			}
		}
	}
}

/* applies damage to the region a model region entry (or the damage info's
   default) belongs to; returns the damage left for the body, none if the
   region absorbs it */
// @retail 0xdb090
real function_db090(s_damage_info *info, void const *region_entry, long object_index, s_damage_owner const *owner,
	real damage, real scale, s_damage_region_accumulator *accumulator)
{
	real region_damage = damage * scale;
	long region_index = info->default_region_index;

	if (region_entry)
		region_index = *(short const *)((byte const *)region_entry + 6);
	if (region_index >= 0 && region_index < info->region_count)
	{
		s_damage_info_region *region = &info->regions[region_index];

		if (region->vitality > 0.0f)
			apply_region_damage(info, object_index, owner, region_index, region_damage / region->vitality, accumulator);
		if (region->flags & 1)
			damage = 0.0f;
	}
	return damage;
}

/* damages an object's body: scales the damage by the body's material,
   applies region damage, takes it off the body and depletes the body when
   it runs out */
// @retail 0xd8cb0
void object_damage_body(long object_index, s_damage_effect_definition const *definition, damage_data *data,
	s_damage_region_accumulator *accumulator)
{
	s_damage_info *info = accumulator->damage_info;
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real damage = function_d9020(object_index, (byte const *)accumulator, (byte const *)definition, data);

	if (damage > 0.0f)
	{
		real maximum_body = object_get_maximum_body_vitality(object_index, definition->category == 1);
		real inverse_maximum_body = maximum_body > 0.0f ? 1.0f / maximum_body : 0.0f;

		if (info && accumulator->unknown00)
		{
			long region_index = *(short *)((byte *)accumulator->unknown00 + 6);

			if (region_index >= 0 && region_index < info->region_count && (info->regions[region_index].flags & 0x80) &&
				(definition->flags & 2))
			{
				if (g_4e6948->state != 1 || object->type != 0 || DAMAGE_OBJECT(object_index)->player_index == NONE)
				{
					*(byte *)&data->unknown84 = (*(byte *)&data->unknown84 & 0x3f) | 0x40;
					object->body_vitality = 0.0f;
					accumulator->flags |= 0x1c0;
				}
			}
		}

		damage = function_db090(info, accumulator->unknown00, object_index, &data->owner, damage, inverse_maximum_body,
			accumulator);

		real body_damage = inverse_maximum_body * damage;

		object->body_vitality -= body_damage;
		if ((info->flags & 0x80) || TEST_FIELD_BIT(object->damage_flags.unknown14))
		{
			if (0.0001f > object->body_vitality)
				object->body_vitality = 0.0001f;
		}
		if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
			function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 0x40);
		accumulator->unknown28 = body_damage;
	}

	s_damage_object *current = DAMAGE_OBJECT(object_index);
	real body_vitality = current->body_vitality;
	real maximum_body_vitality = current->maximum_body_vitality;

	if ((1 << current->type) & 3)
		maximum_body_vitality = function_1e9720(1, current->team) * maximum_body_vitality;
	if (maximum_body_vitality * body_vitality <= 0.0f)
	{
		if (object_is_or_contains_player(object_index, false, false))
		{
			object->damage_flags.unknown12 = true;
		}
		else if (!TEST_FIELD_BIT(object->damage_flags.body_depleted))
		{
			object_deplete_body(object_index, &data->owner, true, (accumulator->flags >> 10) & 1);
			accumulator->flags |= 1;
		}
	}

	if (damage > accumulator->damage_info->body_stun_damage_threshold)
	{
		real stun_time = accumulator->damage_info->body_stun_time;

		if (((1 << object->type) & 3) && object->unknown12c != NONE)
		{
			byte *bounds = (byte *)function_1e4a10(*(long *)(g_4f55f0->data + (object->unknown12c & 0xffff) * 0x888 + 0x54));

			if (bounds)
				stun_time = *(real *)(bounds + 0x40);
		}

		real seconds = g_510c54->ticks_per_second * stun_time;
		long ticks;

		__asm
		{
			fld seconds
			fistp ticks
		}
		object->body_stun_ticks = (short)ticks;
	}

	if ((data->unknown04[0] & 1) && object->type == 0)
	{
		long effect_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x260);

		if (effect_index != NONE &&
			(!TEST_FIELD_BIT(object->damage_flags.body_depleted) && damage > 10.0f || (accumulator->flags & 0x41)))
		{
			function_176780(object_index, (s_effect_owner const *)&data->owner, 0.0f, effect_index, 0.0f, NULL, NULL);
		}
	}
	accumulator->damage -= damage;
}

void function_176870(long object_index, s_effect_owner const *owner, long marker_name, real scale_a, long tag_index,
	short unknown18, real scale_b, real_point3d const *origin, real_vector3d const *direction);
void function_176ad0(long marker_count, s_effect_marker *markers, s_effect_owner const *owner,
	real_vector3d const *velocity, long tag_index, long unknown30, real scale_a, real scale_b, real_point3d const *origin,
	real_vector3d const *direction, long mode);
s_effect_marker *function_176330(s_effect_marker *markers, real_point3d const *point);

/* starts a destroyed permutation's effect: attached to the object, or at
   the marker with the object's velocity */
// @retail 0xd9d60
void function_d9d60(bool at_marker, long marker_name, long object_index, long effect_index, s_damage_owner const *owner)
{
	if (!at_marker)
	{
		function_176870(object_index, (s_effect_owner const *)owner, marker_name, 1.0f, effect_index, NONE, 1.0f, NULL,
			NULL);
	}
	else
	{
		s_object_marker marker;
		real_vector3d velocity;
		s_effect_marker markers[3];

		if (!marker_name)
			marker_name = 0x400054f;
		function_b8d30(object_index, marker_name, &marker, 1, false);
		object_get_velocities(object_index, &velocity, NULL);
		function_176330(markers, &marker.matrix.position);
		markers[2].position = marker.matrix.position;
		markers[2].forward = marker.matrix.forward;
		markers[2].name = marker_name;
		function_176ad0(3, markers, (s_effect_owner const *)owner, &velocity, effect_index, marker_name, 1.0f, 1.0f,
			NULL, NULL, 1);
	}
}

/* a destroyed permutation's area damage, at its marker */
// @retail 0xd9e70
void function_d9e70(long object_index, s_damage_owner const *owner, long definition_index, long marker_name,
	byte unknown)
{
	s_object_marker marker;
	short marker_count = function_b8d30(object_index, marker_name, &marker, 1, false);

	if (marker_name == NONE || marker_name == 0 || marker_count)
	{
		damage_data data;

		damage_data_new(&data, definition_index);
		data.unknown7c = NONE;
		data.owner = *owner;
		*(byte *)&data.unknown84 = unknown;
		data.unknown54 = 1.0f;
		object_get_root_location(object_index, (s_location *)&data.unknown1c);
		data.position = marker.matrix.position;
		data.origin = marker.matrix.position;
		data.direction = marker.matrix.forward;
		data.unknown18 = object_index;
		area_of_effect_cause_damage(&data, NONE);
	}
}

/* destroys the child objects a destroyed permutation named: those attached
   for the model's permutations of that name */
// @retail 0xd9ff0
void function_d9ff0(long object_index, string_id name)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	long model_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38);

	if (model_index != NONE)
	{
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		long region_count = *(long *)(model + 0x50);

		for (long i = 0; i < region_count; i++)
		{
			byte *region = *(byte **)(model + 0x54) + i * 0x38;
			long permutation_count = *(long *)(region + 0x1c);

			for (long j = 0; j < permutation_count; j++)
			{
				byte *permutation = *(byte **)(region + 0x20) + j * 0x10;

				if (*(string_id *)permutation == name)
				{
					long child_tag_index = *(long *)(permutation + 0xc);

					if (child_tag_index != NONE)
					{
						long child_index = object->first_child_object_index;

						while (child_index != NONE)
						{
							s_damage_object *child = DAMAGE_OBJECT(child_index);
							long next_index = child->next_object_index;

							if (TEST_FIELD_BIT(child->flags04.permutation_child) && child->tag_index == child_tag_index)
								object_destroy(child_index);
							child_index = next_index;
						}
					}
				}
			}
		}
	}
}

/* the units among an object's children, with their seats */
struct s_unit_child_iterator
{
	long object_index;
	long unit_index;
	short seat_index;
	long next_index;
};

void function_d0590(s_unit_child_iterator *iterator, long object_index);
s_damage_object *function_d05c0(s_unit_child_iterator *iterator);
void function_b9b90(long object_index, bool disable);
void function_b9c60(long object_index, bool flag);
void __stdcall function_ba6f0(long object_index, long region_index, long state, bool flag);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, long d);
bool function_100390(long weapon_index, long barrel_index);
bool function_1003e0(long weapon_index, long barrel_index);
void function_10d4e0(long object_index);
void __stdcall projectile_detonate(long projectile_index, bool detach_contrail, real contrail_time); /* projectiles.cpp, 0xfc330 */
void function_da860(byte const *owner, byte const *target, word mask, word *bits);
void function_daa50(byte const *owner, byte const *target, word mask, word *bits);
struct s_time_entry;
void function_1e6980(s_time_entry *entries, short a, byte b);
struct s_unknown_1e6a40;
extern s_unknown_1e6a40 *g_51e9c0;

/* the unit event handler (its first function takes a unit and an event) */
struct s_unit_event_handler
{
	bool (__stdcall *handle)(long unit_index, void *event);
};

s_unit_event_handler *g_467840 = NULL;

/* a unit event (0x20 bytes) */
struct s_unit_event
{
	long type;
	byte unknown04[0x1c];
};

/* destroys a permutation of a region: hides it, starts its effects and
   area damage, and applies its side effects to the object (depletes the
   body, drops weapons, ejects riders, destroys attached children) */
// @retail 0xda110
void function_da110(long permutation_index, s_damage_info *info, long object_index, s_damage_owner const *owner,
	long region_index, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info_permutation *permutation = &info->regions[region_index].permutations[permutation_index];

	if (g_4e6948->mode != 4)
	{
		function_a8360(object_index, region_index, permutation_index, false);
		if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
			function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 0x100);
		if (DAMAGE_OBJECT(object_index)->unknownd4 != NONE)
			function_b58c0(DAMAGE_OBJECT(object_index)->unknownd4, 0x200);
	}

	((s_object_region_state *)((byte *)object + object->region_states_offset))[region_index].destroyed_permutations |=
		1 << permutation_index;

	short model_region_index = *(short *)((byte *)permutation + 0x22);
	short model_state = *(short *)((byte *)permutation + 0x20);

	if (model_region_index != NONE && model_state != NONE)
	{
		function_b9b90(object_index, false);
		function_ba6f0(object_index, *(short *)((byte *)permutation + 0x22), *(short *)((byte *)permutation + 0x20), false);
		if (((1 << object->type) & 3) && *(short *)((byte *)permutation + 0x20) >= 3)
		{
			byte *unit_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
			s_unit_child_iterator iterator;

			function_d0590(&iterator, object_index);
			while (function_d05c0(&iterator))
			{
				if (iterator.seat_index != NONE)
				{
					byte *seat = *(byte **)(unit_definition + 0x1cc) + iterator.seat_index * 0xb0;

					if (((*(dword *)seat >> 19) & 1) && *(long *)(seat + 0xac) == *(short *)((byte *)permutation + 0x22))
						function_b9c60(iterator.unit_index, false);
				}
			}
		}
	}

	if (permutation->unknown10 != NONE)
	{
		function_d9d60((permutation->flags >> 20) & 1, permutation->unknown24, object_index, permutation->unknown10,
			owner);
	}
	if (g_4e6948->mode != 4 && permutation->unknown18 != NONE)
		function_d9e70(object_index, owner, permutation->unknown18, permutation->unknown28, info->unknown11 & 0x3f);

	if (permutation->flags & 2)
		object->unknown18[1] |= 1;
	if (permutation->flags & 4)
		object->unknown18[1] |= 2;
	if (permutation->flags & 8)
		object->unknown18[1] |= 4;
	if (permutation->flags & 0x10)
		object->unknown18[1] |= 8;
	if (permutation->flags & 0x400000)
		accumulator->flags |= 0x400;

	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];

	if (permutation->flags & 0x60)
	{
		bool drop = !((permutation->flags >> 5) & 1);

		if ((1 << datum->type) & 3)
		{
			byte *unit = (byte *)datum->object;
			char weapon_slot = *(char *)(unit + 0x212);

			if (weapon_slot != NONE)
			{
				long weapon_index = *(long *)(unit + 0x218 + weapon_slot * 4);

				if (weapon_index != NONE)
					function_100390(weapon_index, drop);
			}
		}
		else if ((1 << datum->type) & 4)
		{
			function_100390(object_index, drop);
		}
	}
	if (permutation->flags & 0x300)
	{
		bool drop = !((permutation->flags >> 8) & 1);

		if ((1 << datum->type) & 3)
		{
			byte *unit = (byte *)datum->object;
			char weapon_slot = *(char *)(unit + 0x212);

			if (weapon_slot != NONE)
			{
				long weapon_index = *(long *)(unit + 0x218 + weapon_slot * 4);

				if (weapon_index != NONE)
					function_1003e0(weapon_index, drop);
			}
		}
		else if ((1 << datum->type) & 4)
		{
			function_1003e0(object_index, drop);
		}
	}
	if (permutation->flags & 0x20000)
	{
		if ((1 << datum->type) & 0x1c)
			function_10d4e0(object_index);
		else if ((1 << datum->type) & 0x20)
			projectile_detonate(object_index, false, 0.0f);
	}

	datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	if ((1 << datum->type) & 2)
	{
		word *vehicle_flags = (word *)((byte *)datum->object + 0x352);

		if (permutation->flags & 0x400)
			*vehicle_flags |= 1;
		if (permutation->flags & 0x800)
			*(byte *)vehicle_flags |= 2;
		if (permutation->flags & 0x1000)
			*(byte *)vehicle_flags |= 4;
		if (permutation->flags & 0x2000)
			*(byte *)vehicle_flags |= 8;
		if (permutation->flags & 0x4000)
			*(byte *)vehicle_flags |= 0x10;
		if (permutation->flags & 0x8000)
			*(byte *)vehicle_flags |= 0x20;
	}

	if ((permutation->flags & 1) ? !object_is_or_contains_player(object_index, false, false) :
		(permutation->flags & 0x10000) &&
		(g_4e6948->state != 1 || !object_is_or_contains_player(object_index, true, false)))
	{
		object->body_vitality = 0.0f;
		object_deplete_body(object_index, owner, true, false);
		accumulator->flags |= 1;
	}

	if (permutation->flags & 0x80)
	{
		if (g_4e6948->mode == 4)
		{
			s_damage_object *current = DAMAGE_OBJECT(object_index);

			if (!(current->unknown04[0] & 1))
			{
				if (function_b9d20(object_index))
					function_bef30(object_index, 1, 0, 0, 0);
				*(dword *)current->unknown04 |= 1;
				function_b8b70(object_index);
			}
		}
		else
		{
			accumulator->flags |= 4;
		}
	}

	if (permutation->flags & 0x80000)
	{
		for (long child_index = object->first_child_object_index; child_index != NONE; )
		{
			s_damage_object *child = DAMAGE_OBJECT(child_index);

			if (TEST_FIELD_BIT(child->flags04.permutation_child))
				object_deplete_body(child_index, owner, true, false);
			child_index = child->next_object_index;
		}
	}

	string_id section_name = permutation->unknown3c;

	if (section_name && section_name != NONE)
	{
		long section_index = NONE;
		byte *sections = info->unknowne4;

		for (long i = 0; i < info->unknowne0; i++, sections += 0x14)
		{
			if (*(string_id *)(sections + 4) == section_name)
			{
				section_index = i;
				break;
			}
		}
		if (section_index != NONE)
		{
			word kind = permutation->unknown02;

			if (kind)
			{
				if (kind > 2)
					accumulator->unknown2e |= 1 << section_index;
				else
					accumulator->unknown2c |= 1 << section_index;
			}
		}
		else
		{
			switch (permutation->unknown02)
			{
			case 1:
				function_da860((byte const *)info, (byte const *)permutation, object->unknowne0, (word *)&accumulator->unknown2c);
				break;
			case 2:
				function_daa50((byte const *)info, (byte const *)permutation, object->unknowne0, (word *)&accumulator->unknown2c);
				break;
			case 3:
				function_da860((byte const *)info, (byte const *)permutation, object->unknowne2, (word *)&accumulator->unknown2e);
				break;
			case 4:
				function_daa50((byte const *)info, (byte const *)permutation, object->unknowne2, (word *)&accumulator->unknown2e);
				break;
			}
		}
	}

	string_id seat_name = permutation->unknown40;

	if (seat_name && seat_name != NONE && ((1 << object->type) & 3))
	{
		byte *unit_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
		long seat_count = *(long *)(unit_definition + 0x1c8);
		long seat_index;
		byte *seat = *(byte **)(unit_definition + 0x1cc);

		for (seat_index = 0; seat_index < seat_count; seat_index++, seat += 0xb0)
		{
			if (*(string_id *)(seat + 4) == seat_name)
				break;
		}
		if (seat_index != seat_count)
		{
			s_unit_child_iterator iterator;

			function_d0590(&iterator, object_index);
			while (function_d05c0(&iterator))
			{
				if (iterator.seat_index == seat_index)
				{
					long unit_index = iterator.unit_index;
					s_unit_event_handler *handler = g_467840;
					s_unit_event event;

					memset(&event, 0, sizeof(event));
					event.type = 0x1e;
					function_b7360(unit_index);

					bool handled = handler->handle(unit_index, &event);

					if (unit_index != NONE)
					{
						long player_index = DAMAGE_OBJECT(unit_index)->player_index;

						if (player_index != NONE)
						{
							short controller = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

							if (controller != NONE)
							{
								function_1e6980((s_time_entry *)((byte *)g_51e9c0 + controller * 0x1b0 + 0x150),
									(short)event.type, handled);
							}
						}
					}
				}
			}
		}
	}

	string_id child_name = permutation->unknown48;

	if (child_name != NONE && child_name)
		function_d9ff0(object_index, child_name);
}

struct s_ai_scale_source;
bool function_1c8440(long actor_index, real *value, s_ai_scale_source const *source);
real function_15dec0(long player_index, long owner_player_index);
bool function_0bfe60(const dword *flags, long bit);
struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;

/* a random real in [lower, upper] */
PRIVATE inline real damage_random_range(real lower, real upper)
{
	real random = _real_random(&g_4e7408->unknown0, __FILE__, __LINE__);

	return (upper - lower) * random + lower;
}

/* the damage a damage event does to an object before armor: a random
   amount between the definition's bounds, scaled by the event, the owner's
   AI and the game engine; sets *friendly when the team scale applies */
// @retail 0xd9b60
real function_d9b60(damage_data *data, byte const *definition, long object_index, bool *friendly)
{
	real amount = *(real const *)(definition + 0x10);

	if (*(real const *)(definition + 0x10) != *(real const *)(definition + 0x14))
		amount = damage_random_range(*(real const *)(definition + 0x10), *(real const *)(definition + 0x14));

	real damage = ((1.0f - data->unknown54) * *(real const *)(definition + 0xc) + amount * data->unknown54) *
		data->unknown5c;

	if (data->owner.object_index != NONE)
	{
		byte *owner = (byte *)function_badc0(data->owner.object_index, 3);

		if (owner)
		{
			if (*(long *)(owner + 0x24c) != NONE)
				owner = (byte *)DAMAGE_OBJECT(*(long *)(owner + 0x24c));
			if (*(long *)(owner + 0x12c) != NONE)
				function_1c8440(*(long *)(owner + 0x12c), &damage, (s_ai_scale_source const *)data);
			if (g_4e6948->state == 1)
			{
				s_damage_object *object = DAMAGE_OBJECT(object_index);

				if (((1 << object->type) & 3) && ((*(dword *)((byte *)object + 0x134) >> 31) & 1) &&
					*(short *)(owner + 0x138) != 1)
				{
					damage = 0.0f;
				}
			}
		}
	}

	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		real scale = function_15dec0(get_player_index_from_object_or_parents(object_index), data->owner.player_index);

		if (scale == 0.0f)
			data->in_unknown_radius = false;
		return scale * damage;
	}

	short team = data->owner.team;

	if (team != NONE && g_4e6948->state == 1)
	{
		bool enemy = true;

		if (team >= 0 && team < 16)
			enemy = !function_0bfe60((dword const *)((byte *)g_4f55ec + 0xc4), team * 16 + 1);
		if (enemy)
		{
			*friendly = true;
			return function_1e9700(0) * damage;
		}
	}
	return damage;
}

/* what an object is told about the damage it took */
struct s_damage_report
{
	byte unknown00;
	byte unknown01[3];
	dword flags;
	long definition_index;
	s_damage_owner owner;
	real_vector3d direction;
	real_point3d origin;
	byte unknown30[4];
	real scale;
	real unknown38;
	long unknown3c;
	short unknown40;
	byte unknown42[2];
	real unknown44;
	real unknown48;
	real distance;
	long unknown50;
};

void function_c9e70(long unit_index, dword flags, damage_data const *data, s_damage_report const *report);
void function_119280(long object_index, dword flags);

/* reports damage to the object it hit (units and type 12 objects) */
// @retail 0xd9490
void function_d9490(s_damage_report *report, damage_data const *data, s_damage_region_accumulator const *accumulator,
	long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	report->flags = 0;
	report->definition_index = data->definition_index;
	report->unknown44 = accumulator->unknown28;
	report->unknown48 = accumulator->shield_damage;
	report->origin = data->origin;
	report->owner = data->owner;
	report->scale = data->unknown5c * data->unknown54;
	report->unknown38 = data->unknown58;
	report->unknown3c = accumulator->unknown08;
	report->unknown40 = (short)accumulator->unknown10;
	report->distance = data->distance;
	report->unknown50 = 0;
	if (data->unknown04[0] & 1)
		report->direction = data->node_direction;
	else
		report->direction = data->direction;
	report->unknown00 = data->unknown84;

	if (accumulator->flags & 1)
		report->flags |= 1;
	if (accumulator->flags & 4)
		report->flags |= 0x400;
	if (accumulator->flags & 8)
		report->flags |= 2;
	if (data->unknown04[0] & 0x80)
		report->flags |= 0x800;
	if (data->unknown04[0] & 1)
		report->flags |= 4;
	if (accumulator->flags & 0x10)
		report->flags |= 0x10;
	if (data->unknown04[0] & 0x10)
		report->flags |= 0x20;
	if (accumulator->flags & 0x40)
		report->flags |= 0x40;
	if (accumulator->flags & 0x80)
		report->flags |= 0x80;
	if (*(dword const *)data->unknown04 & 0x2000)
		report->flags |= 0x200;
	if (accumulator->flags & 0x200)
		report->flags |= 0x100;
	if (accumulator->unknown18 > 0.0f)
		report->flags |= 8;

	if ((1 << object->type) & 3)
		function_c9e70(object_index, accumulator->flags, data, report);
	else if ((1 << object->type) & 0x1000)
		function_119280(object_index, accumulator->flags);
}

long function_cbd50(long unit_index, short weapon_slot);
void __stdcall function_101c80(long object_index);
void __stdcall function_b7880(long object_index, long node_index, real_point3d const *point, real_vector3d const *impulse,
	bool flag);
void projectile_accelerate(long projectile_index, real_vector3d const *impulse); /* projectiles.cpp, 0xfa820 */
void function_10cf80(real_vector3d const *impulse, long item_index, bool flag);
void __stdcall function_de620(long biped_index, real_vector3d const *impulse);
void function_119020(long creature_index, real_vector3d const *impulse);
struct s_statborg;
s_statborg *game_engine_get_statborg();
void __stdcall function_1e9fa0(void *engine_globals, long object_index, long player_index, word team, byte kind);
void function_1e8fa0(long player_index, long object_index, byte kind);
void __stdcall function_ca0b0(long unit_index, s_damage_report const *report);
void function_a80f0(long object_index, s_damage_report const *report);

bool g_4f55e4 = false;
bool g_4f55e6 = false;

/* what follows damage: the recent damage the object took, the impulse the
   damage gives it, the game engine's and the players' notices, and the
   object's own response */
// @retail 0xd9640
void object_damage_aftermath(s_damage_report const *report, long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	byte *object_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
	byte *definition = g_4e3b44[report->definition_index & 0xffff].bytes;
	real body_damage = report->unknown44;

	if ((report->flags & 1) && !(body_damage > 1.0f))
		body_damage = 1.0f;
	if (body_damage > 0.0001f)
	{
		object->unknown100 = CEILING(body_damage + object->unknown100, 1.0f);
		object->unknownf8 = CEILING(object->unknownf8 + body_damage, 1.0f);
		object->unknown109 = 0;
	}

	if ((report->flags & 1) && report->owner.object_index != NONE)
	{
		s_damage_object *owner = (s_damage_object *)function_badc0(report->owner.object_index, 3);

		if (owner)
		{
			long weapon_index = function_cbd50(report->owner.object_index,
				*(char *)((byte *)DAMAGE_OBJECT(report->owner.object_index) + 0x212));

			if (weapon_index != NONE)
			{
				byte kind = report->unknown00 & 0x3f;

				if (kind == 0x14)
				{
					function_101c80(object_index);
				}
				else if (owner->player_index != NONE && g_4e6948->state == 1 && g_4f55e4 && kind == 3)
				{
					owner->shield_vitality += 1.0f;
					if (owner->shield_vitality > 3.0f)
						owner->shield_vitality = 3.0f;
				}
			}
		}
	}

	real shield_damage = report->unknown48;

	if (!TEST_FIELD_BIT(object->damage_flags.unknown11))
	{
		if ((report->flags & 2) && !(shield_damage > 1.0f))
			shield_damage = 1.0f;
		if (shield_damage > 0.0001f)
		{
			object->unknownfc = CEILING(shield_damage + object->unknownfc, 1.0f);
			object->unknownf4 = CEILING(object->unknownf4 + shield_damage, 1.0f);
			object->unknown108 = 0;
		}
	}

	if (*(real *)(object_definition + 0x14) > 0.001f && (report->flags & 0x100))
	{
		real_vector3d direction = report->direction;
		real scale = *(real *)(definition + 0x40) * *(real *)(object_definition + 0x14);

		if (!(report->flags & 0x200) && scale > 0.001f)
		{
			long type = (char)object->type;
			dword type_mask = 1 << type;

			if (!(type_mask & 0x20))
			{
				object->owner_player_index = report->owner.player_index;
				object->owner_object_index = report->owner.object_index;
				object->owner_team = report->owner.team;
			}
			if (!(type_mask & 3) || !((*(dword *)((byte *)object + 0x134) >> 19) & 1))
			{
				bool large = report->scale > 0.5f && (*(dword *)(definition + 0x14) & 0x20);
				real_vector3d impulse;
				real_vector3d lifted_impulse;
				real_vector3d thrown_impulse;

				impulse.i = direction.i * scale;
				impulse.j = direction.j * scale;
				impulse.k = direction.k * scale;
				if (g_4e6948->state == 1 && g_4f55e6)
					scale *= 3.0f;

				direction.k += 0.15f;
				function_30bf0(&direction);
				lifted_impulse.i = direction.i * scale;
				lifted_impulse.j = direction.j * scale;
				lifted_impulse.k = direction.k * scale;
				direction.k += 0.3f;
				function_30bf0(&direction);
				thrown_impulse.i = direction.i * scale;
				thrown_impulse.j = direction.j * scale;
				thrown_impulse.k = direction.k * scale;

				if (!(type_mask & 0x1883) || scale > 0.3f)
				{
					switch (type)
					{
					case 0:
						function_de620(object_index, &thrown_impulse);
						break;
					case 1:
					case 7:
					case 11:
						if (report->flags & 4)
							function_b7880(object_index, NONE, &report->origin, &lifted_impulse, false);
						else
							function_b7880(object_index, report->unknown40, &report->origin, &impulse, false);
						break;
					case 2:
					case 3:
					case 4:
						function_10cf80(&thrown_impulse, object_index, large);
						break;
					case 5:
						projectile_accelerate(object_index, &thrown_impulse);
						break;
					case 12:
						function_119020(object_index, &thrown_impulse);
						break;
					}
				}
			}
		}
	}

	if (g_4e6948->mode != 4 && !(report->flags & 0x800) && ((1 << object->type) & 3) && (report->flags & 1))
	{
		if (game_engine_get_statborg())
		{
			function_1e9fa0(game_engine_get_statborg(), object_index, report->owner.player_index, report->owner.team,
				report->unknown00);
		}

		long player_index = report->owner.player_index;

		if (player_index != NONE && datum_get(g_4e8c24, player_index))
			function_1e8fa0(player_index, object_index, report->unknown00);
	}
	if ((1 << object->type) & 3)
		function_ca0b0(object_index, report);
	if (report->flags & 0x400)
		object_destroy(object_index);
	function_a80f0(object_index, report);
}

enum
{
	k_maximum_damage_markers = 32
};

/* damages an object's regions through the model's damage markers within
   the damage's radius and cone, then its attached children the same way */
// @retail 0xd82e0
void function_d82e0(damage_data *data, real damage, long object_index, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	byte *object_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	s_damage_definition *jpt = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_effect_definition *definition = (s_damage_effect_definition *)((byte *)jpt + 0x10);

	if (info && *(long *)(object_definition + 0x38) != NONE)
	{
		byte *model = g_4e3b44[*(long *)(object_definition + 0x38) & 0xffff].bytes;
		long marker_count = 0;
		byte marker_indices[k_maximum_damage_markers];
		real marker_scales[k_maximum_damage_markers];

		for (long i = 0; i < *(long *)(model + 0x68); i++)
		{
			byte *entry = *(byte **)(model + 0x6c) + i * 0x1c;
			short region_index = *(short *)(entry + 0xc);
			s_damage_info_region *region = NULL;

			if (region_index >= 0 && region_index < info->region_count)
				region = &info->regions[region_index];
			if (region && !(region->vitality > 0.0f))
				continue;

			s_object_marker marker;

			if (function_b8d30(object_index, *(long *)entry, &marker, 1, false) == 1)
			{
				real_vector3d direction;

				direction.i = data->origin.x - marker.matrix.position.x;
				direction.j = data->origin.y - marker.matrix.position.y;
				direction.k = data->origin.z - marker.matrix.position.z;

				real distance = function_30bf0(&direction) - *(real *)(entry + 4);

				if (!(distance > 0.0f))
					distance = 0.0f;
				if (jpt->radius04 >= distance &&
					*(real *)(entry + 8) > acos(marker.matrix.forward.k * direction.k +
						marker.matrix.forward.j * direction.j + marker.matrix.forward.i * direction.i))
				{
					real range = jpt->radius04 - jpt->minimum_radius;
					real scale;

					if (range > 0.0f)
						scale = PIN(1.0f - (distance - jpt->minimum_radius) / range, 0.0f, 1.0f);
					else
						scale = 1.0f;
					marker_indices[marker_count] = (byte)i;
					marker_scales[marker_count] = scale;
					marker_count++;
				}
			}
		}

		if (marker_count > 0)
		{
			bool friendly = false;
			real amount = function_d9b60(data, (byte const *)definition, object_index, &friendly);

			if (global_material_get(data->unknown7c))
			{
				amount = damage_armor_table_lookup(definition->damage_group, definition->damage_group_b,
					*(string_id *)((byte *)function_188690(data->unknown7c) + 0x10),
					*(string_id *)((byte *)function_188690(data->unknown7c) + 0x14)) * amount;
			}
			if (amount > 0.0f)
			{
				real total = 0.0f;
				real maximum_body = object_get_maximum_body_vitality(object_index, friendly);
				real scale = (maximum_body > 0.0f ? 1.0f / maximum_body : 0.0f) * amount;
				long i;

				for (i = 0; i < marker_count; i++)
					total += marker_scales[i];
				for (i = 0; i < marker_count; i++)
				{
					byte *entry = *(byte **)(model + 0x6c) + marker_indices[i] * 0x1c;
					real share = 0.0f;

					if (total > 0.0001f)
						share = marker_scales[i] / total * scale;

					short region_index = *(short *)(entry + 0xc);

					if (region_index >= 0 && region_index < info->region_count)
					{
						apply_region_damage(info, object_index, &data->owner, region_index,
							share / info->regions[region_index].vitality, accumulator);
					}
				}
			}
		}
	}

	for (long child_index = object->first_child_object_index; child_index != NONE; )
	{
		s_damage_object *child = DAMAGE_OBJECT(child_index);

		if (TEST_FIELD_BIT(child->flags04.permutation_child) && !TEST_FIELD_BIT(child->damage_flags.unknown7))
		{
			s_damage_region_accumulator child_accumulator;
			s_damage_report report;

			memset(&child_accumulator, 0, sizeof(child_accumulator));
			function_d82e0(data, damage, child_index, &child_accumulator);
			function_d9490(&report, data, &child_accumulator, child_index);
			if (g_4e6948->mode == 4)
				report.flags &= ~1;
			object_damage_aftermath(&report, child_index);
			if ((child_accumulator.flags & 4) && g_4e6948->mode != 4)
				function_b8540(child_index);
		}
		child_index = child->next_object_index;
	}
}

bool function_cc010(long object_index, real_vector3d const *direction);
void function_db210(damage_data const *data, long vehicle_index);
void function_15cd90(long player_index, short identifier, long other_player_index);

enum
{
	k_maximum_damage_parents = 16
};

/* damages an object, and the objects it rides in: for each, the shield
   first, then the body and regions, then the aftermath */
// @retail 0xd7b80
void object_cause_damage(damage_data *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	real_vector3d const *unknown14)
{
	s_damage_effect_definition *definition =
		(s_damage_effect_definition *)(g_4e3b44[data->definition_index & 0xffff].bytes + 0x10);
	bool shield_children = true;
	bool body_children = true;
	bool friendly = false;
	real amount = function_d9b60(data, (byte const *)definition, object_index, &friendly);
	real damage = amount;
	long objects[k_maximum_damage_parents];
	long object_count = 0;
	bool reported = false;

	if ((data->flags & 5) || (data->flags & 0x200))
	{
		objects[0] = object_index;
		object_count = 1;
	}
	else
	{
		for (long index = object_index; index != NONE; index = DAMAGE_OBJECT(index)->parent_object_index)
			objects[object_count++] = index;
	}

	s_damage_info *target_info = (s_damage_info *)function_d5b60(object_index);

	if (target_info)
	{
		shield_children = !((target_info->flags >> 4) & 1);
		body_children = !((target_info->flags >> 6) & 1);
	}

	for (long i = object_count - 1; i >= 0; i--)
	{
		s_damage_object *object = DAMAGE_OBJECT(objects[i]);
		s_damage_info *info = (s_damage_info *)function_d5b60(objects[i]);
		byte *object_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
		s_damage_region_accumulator accumulator;

		memset(&accumulator, 0, sizeof(accumulator));
		accumulator.unknown18 = amount;
		accumulator.damage = damage;
		accumulator.unknown08 = NONE;
		if (i == 0)
			accumulator.flags |= 0x200;

		if (data->unknown7e != NONE && data->owner.player_index != NONE && ((1 << object->type) & 3) &&
			object->player_index != NONE)
		{
			function_15cd90(data->owner.player_index, data->unknown7e, object->player_index);
			data->unknown7e = NONE;
		}

		if (info && *(long *)(object_definition + 0x38) != NONE)
		{
			byte *model = g_4e3b44[*(long *)(object_definition + 0x38) & 0xffff].bytes;
			bool region_absorbs = false;

			accumulator.damage_info = info;
			if (node_index >= 0 && node_index < info->unknownc4)
				accumulator.unknown08 = *(short *)(info->unknownc8 + node_index * 0x10);
			if (objects[i] == object_index)
			{
				if (region_entry_index != NONE)
				{
					byte *entry = *(byte **)(model + 0x5c) + region_entry_index * 0x14;

					data->unknown7c = *(short *)(entry + 0x10);
					accumulator.unknown00 = entry;
				}
				else
				{
					data->unknown7c = info->unknownce;
				}
			}
			else if (data->flags & 1)
			{
				data->unknown7c = info->unknownce;
			}

			if (i == 0)
			{
				accumulator.unknown0c = unknown0c;
				accumulator.unknown14 = (long)unknown14;
				accumulator.unknown10 = node_index;
			}
			else
			{
				accumulator.unknown0c = NONE;
				accumulator.unknown14 = 0;
				accumulator.unknown10 = NONE;
			}

			long region_index = accumulator.unknown00 ? *(short *)((byte *)accumulator.unknown00 + 6) :
				info->default_region_index;

			if (region_index >= 0 && region_index < info->region_count && (info->regions[region_index].flags & 0x100))
				region_absorbs = true;

			if (damage == 0.0f && i == 0 && object->shield_vitality > 0.0f && !region_absorbs)
			{
				data->unknown7c = info->shield_material;
				data->unknown78 = object->shield_vitality;
			}

			if (damage > 0.0f || data->in_unknown_radius || data->unknown58 > 0.0f)
			{
				s_damage_object *current = DAMAGE_OBJECT(objects[i]);
				bool kill = (data->flags >> 2) & 1;

				current->owner_object_index = data->owner.object_index;
				current->owner_player_index = data->owner.player_index;
				current->owner_team = data->owner.team;
				if (friendly)
					accumulator.flags |= 0x20;
				if (data->owner.team != NONE && ((1 << object->type) & 3) &&
					!game_team_is_enemy(object->team, data->owner.team))
				{
					accumulator.flags |= 0x10;
				}

				if (i == 0 && object->parent_object_index == NONE && !(data->flags & 0x200) &&
					definition->side_effect == 2 && function_cc010(objects[i], &data->direction) &&
					!TEST_FIELD_BIT(object->damage_flags.unknown7))
				{
					s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[objects[i] & 0xffff];

					if (!((1 << datum->type) & 3) || game_team_is_enemy(datum->object->team, data->owner.team))
					{
						*(byte *)&data->unknown84 = (*(byte *)&data->unknown84 & 0x3f) | 0x80;
						kill = true;
					}
				}

				if (i + 1 < object_count)
					accumulator.damage = function_dc230(objects[i], objects[i + 1], data) * damage;

				if (!(info->flags & 0x80) && !TEST_FIELD_BIT(object->damage_flags.unknown14) && kill &&
					!TEST_FIELD_BIT(object->damage_flags.body_depleted))
				{
					object->shield_vitality = 0.0f;
					object_deplete_shield(objects[i]);
					object->body_vitality = 0.0f;
					object_deplete_body(objects[i], &data->owner, !((data->flags >> 7) & 1), false);
					accumulator.flags |= 0x41;
				}

				if (!(data->flags & 0x20) && !(definition->flags & 0x200) && (!region_absorbs || data->in_unknown_radius) &&
					object->maximum_shield_vitality > 0.0f)
				{
					if (i == 0 || shield_children && (info->flags & 1))
						object_damage_shield(objects[i], definition, data, &accumulator);
				}

				if (g_4e6948->mode != 4 &&
					((1 << ((s_damage_object_datum *)g_4e0300->data)[objects[i] & 0xffff].type) & 2) && i == 0 &&
					accumulator.damage_info && (!(data->flags & 0x1000) || (info->flags & 0x100)))
				{
					function_db210(data, objects[i]);
				}

				if (!(definition->flags & 0x40) && (i == 0 || body_children && (accumulator.damage_info->flags & 2)))
				{
					if ((data->flags & 1) || (data->flags & 0x100))
						function_d82e0(data, accumulator.damage, objects[i], &accumulator);
					object_damage_body(objects[i], definition, data, &accumulator);
				}

				if (!reported &&
					(accumulator.shield_damage > 0.0001f || accumulator.unknown24 > 0.0001f || accumulator.unknown28 > 0.0001f))
				{
					if (accumulator.shield_damage > MAX(accumulator.unknown28, accumulator.unknown24))
					{
						data->unknown7c = info->shield_material;
						data->unknown78 = object->shield_vitality;
					}
					else
					{
						data->unknown78 = PIN(object->body_vitality, 0.0f, 1.0f);
					}
					function_b7360(objects[i]);
					reported = true;
				}
			}
		}

		if (object->node_index != NONE && (accumulator.unknown2c || accumulator.unknown2e))
			function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);

		s_damage_report report;

		function_d9490(&report, data, &accumulator, objects[i]);
		if (g_4e6948->mode == 4)
			report.flags &= ~1;
		object_damage_aftermath(&report, objects[i]);
		if ((accumulator.flags & 4) && g_4e6948->mode != 4)
			function_b8540(objects[i]);
		damage = accumulator.damage;
	}
}

/* passes damage a vehicle takes on to its riders, scaled by their seats'
   entries in its damage info, and to the vehicles it carries */
// @retail 0xdb210
void function_db210(damage_data const *data, long vehicle_index)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);
	s_damage_info *info = (s_damage_info *)function_d5b60(vehicle_index);
	byte *vehicle_definition = g_4e3b44[vehicle->tag_index & 0xffff].bytes;
	damage_data rider_data = *data;
	s_unit_child_iterator iterator;

	function_d0590(&iterator, vehicle_index);
	rider_data.flags |= 0x200;
	while (function_d05c0(&iterator))
	{
		long unit_index = iterator.unit_index;
		s_damage_object *unit = DAMAGE_OBJECT(unit_index);
		real distance_squared =
			(unit->bounding_sphere_center.z - data->origin.z) * (unit->bounding_sphere_center.z - data->origin.z) +
			(unit->bounding_sphere_center.y - data->origin.y) * (unit->bounding_sphere_center.y - data->origin.y) +
			(unit->bounding_sphere_center.x - data->origin.x) * (unit->bounding_sphere_center.x - data->origin.x);

		if (unit->unknown1fc == NONE || unit_index == data->unknown18)
			continue;
		if ((definition->flags14 & 1) && unit_index == data->owner.object_index)
			continue;
		if ((definition->flags14 & 8) && !game_team_is_enemy(unit->team, data->owner.team))
			continue;

		long seat_key = *(long *)(*(byte **)(vehicle_definition + 0x1cc) + unit->unknown1fc * 0xb0 + 4);

		for (long i = 0; i < info->unknownd8; i++)
		{
			byte *entry = info->unknowndc + i * 0x14;

			if (*(long *)entry == seat_key)
			{
				real radius = *(real *)(entry + 8);
				real scale;

				if (radius * radius > distance_squared)
					scale = *(real *)(entry + 0xc) * definition->unknown48;
				else
					scale = *(real *)(entry + 0x10) * definition->unknown4c;
				rider_data.unknown5c = data->unknown5c * scale;
				object_cause_damage(&rider_data, unit_index, NONE, NONE, NONE, NULL);
				break;
			}
		}
	}

	for (long child_index = vehicle->first_child_object_index; child_index != NONE; )
	{
		s_damage_object *child = DAMAGE_OBJECT(child_index);

		if (((1 << child->type) & 2) && child_index != NONE && function_d5b60(child_index))
			function_db210(&rider_data, child_index);
		child_index = child->next_object_index;
	}
}

/* sets or clears whether the objects of an object list, and their
   attached children, can't take damage */
// @retail 0xd87e0
void function_d87e0(long list_index, bool can_take_damage)
{
	long reference_index;
	long object_index = object_list_get_first(list_index, &reference_index);

	while (object_index != NONE)
	{
		s_damage_object *object = DAMAGE_OBJECT(object_index);

		if (!can_take_damage)
			object->damage_flags.unknown7 = true;
		else
			object->damage_flags.unknown7 = false;
		for (long child_index = object->first_child_object_index; child_index != NONE; )
		{
			s_damage_object *child = DAMAGE_OBJECT(child_index);

			if (TEST_FIELD_BIT(child->flags04.permutation_child))
			{
				if (!can_take_damage)
					child->damage_flags.unknown7 = true;
				else
					child->damage_flags.unknown7 = false;
			}
			child_index = child->next_object_index;
		}
		object_index = object_list_get_next(&reference_index);
	}
}

/* how many of the model markers of a name an object and its attached
   children are missing */
// @retail 0xd88f0
short __stdcall function_d88f0(long object_index, long marker_name)
{
	short missing_count = 0;

	for (long index = object_index; index != NONE; )
	{
		s_damage_object_datum *datum = (s_damage_object_datum *)datum_get_inlined(g_4e0300, index);
		s_damage_object *object = NULL;

		if (datum && (1 << datum->type))
		{
			object = datum->object;
			if (object && (index == object_index || TEST_FIELD_BIT(object->flags04.permutation_child)))
			{
				long model_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38);

				if (model_index != NONE)
				{
					byte *model = g_4e3b44[model_index & 0xffff].bytes;
					long marker_count = *(long *)(model + 0x68);

					for (long i = 0; i < marker_count; i++)
					{
						string_id name = *(string_id *)(*(byte **)(model + 0x6c) + i * 0x1c);

						if (name == marker_name)
						{
							s_object_marker marker;

							if (!function_b8d30(index, name, &marker, 1, false))
								missing_count++;
						}
					}
				}
			}
		}
		if (index == NONE)
			break;
		if (index == object_index)
			index = object->first_child_object_index;
		else
			index = object->next_object_index;
	}
	return missing_count;
}

/* damages an object's region of a name */
// @retail 0xd8a40
void function_d8a40(string_id region_name, long object_index, real damage)
{
	if (object_index != NONE)
	{
		s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

		if (info && region_name && region_name != NONE)
		{
			long region_index;
			s_damage_info_region *region;

			for (region_index = 0, region = info->regions; region_index < info->region_count; region_index++, region++)
			{
				if (region->name == region_name)
					break;
			}
			if (region_index != info->region_count)
			{
				s_damage_region_accumulator accumulator;

				memset(&accumulator, 0, sizeof(accumulator));
				apply_region_damage(info, object_index, g_467420, region_index, damage, &accumulator);
				if (accumulator.unknown2c || accumulator.unknown2e)
					function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);
				if (accumulator.flags & 4)
					object_destroy(object_index);
			}
		}
	}
}

bool function_cc410(long unit_index);
void function_155b60(long unit_index);

/* kills an object outright with the globals' default damage (falling,
   for one), reporting it as maximum damage */
// @retail 0xdc0a0
void function_dc0a0(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted))
	{
		byte *damage_globals = *(byte **)((byte *)g_4e034c + 0x144);

		if (*(long *)(damage_globals + 0x14) != NONE)
		{
			bool unknown = ((1 << object->type) & 3) ? function_cc410(object_index) : false;

			if ((1 << object->type) & 1)
			{
				real_vector3d velocity;

				object_get_velocities(object_index, &velocity, NULL);
				if (*((byte *)object + 0x3dc) == 1 && -velocity.k > *(real *)(damage_globals + 0x5c) &&
					object->player_index != NONE)
				{
					function_155b60(object_index);
				}
			}

			object_deplete_shield(object_index);
			object->body_vitality = 0.0f;
			object_deplete_body(object_index, g_467420, true, false);

			damage_data data;
			s_damage_region_accumulator accumulator;
			s_damage_report report;

			data.unknown7c = NONE;
			damage_data_new(&data, *(long *)(damage_globals + 0x14));
			memset(&accumulator, 0, sizeof(accumulator));
			data.unknown54 = 1.0f;
			accumulator.shield_damage = 1.0f;
			accumulator.unknown28 = 1.0f;
			accumulator.unknown08 = NONE;
			data.flags |= 4;
			accumulator.flags = 0x49;
			accumulator.unknown18 = 3.4028235e38f;
			function_d9490(&report, &data, &accumulator, object_index);
			if (unknown)
				report.unknown50 = 0;
			object_damage_aftermath(&report, object_index);
		}
	}
}

struct s_collision_result_1697c0
{
	byte unknown00[8];
	real_point3d point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x4c - 0x26];
};

bool __stdcall function_1697c0(long flags, real_point3d const *point, real_vector3d const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
real_vector3d *function_11d000(real_vector3d const *v, real_vector3d *out);
long function_baf80(long object_index);

PRIVATE inline void cross_product3d(real_vector3d const *a, real_vector3d const *b, real_vector3d *out)
{
	out->i = a->j * b->k - a->k * b->j;
	out->j = a->k * b->i - a->i * b->k;
	out->k = a->i * b->j - a->j * b->i;
}

PRIVATE inline void scale_vector3d(real_vector3d const *v, real scale, real_vector3d *out)
{
	out->i = scale * v->i;
	out->j = v->j * scale;
	out->k = v->k * scale;
}

/* whether the structure blocks damage from the damage's origin to a point
   on an object; around units, four rays offset sideways by the
   definition's spread must all be blocked */
// @retail 0xd6f90
bool function_d6f90(long object_index, real_point3d const *point, damage_data const *data)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_collision_result_1697c0 collision;

	if (((1 << object->type) & 3) && *(real *)((byte *)definition + 0x18) > 0.0001f)
	{
		real_vector3d offset;
		real_vector3d side;
		real_vector3d up;
		bool result;

		vector3d_from_points3d(&data->origin, point, &offset);
		function_30bf0(function_11d000(&offset, &side));
		cross_product3d(&offset, &side, &up);
		function_30bf0(&up);

		for (long i = 0; i < 4; i++)
		{
			collision.unknown24 = NONE;
			switch (i)
			{
			case 0:
				scale_vector3d(&side, *(real *)((byte *)definition + 0x18), &offset);
				break;
			case 1:
				scale_vector3d(&side, -*(real *)((byte *)definition + 0x18), &offset);
				break;
			case 2:
				scale_vector3d(&up, *(real *)((byte *)definition + 0x18), &offset);
				break;
			case 3:
				scale_vector3d(&up, -*(real *)((byte *)definition + 0x18), &offset);
				break;
			}

			function_1697c0(0x4a08c2d, &data->origin, &offset, function_baf80(object_index), data->unknown18, &collision);

			real_point3d start = collision.point;
			real_vector3d remaining;

			vector3d_from_points3d(&start, point, &remaining);
			result = function_1697c0(0x14a08c2d, &start, &remaining, function_baf80(object_index), data->unknown18,
				&collision);
			if (!result)
				return result;
		}
		return result;
	}
	else
	{
		real_vector3d vector;

		collision.unknown24 = NONE;
		vector3d_from_points3d(&data->origin, point, &vector);
		return function_1697c0(0x14a08c2d, &data->origin, &vector, function_baf80(object_index), data->unknown18,
			&collision);
	}
}

/* whether an object's damage section with these two keys is in the first
   (0xdb4c0) or second (0xdb540) of its section masks */
// @retail 0xdb4c0
bool function_db4c0(long object_index, long key_a, long key_b)
{
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool result = false;

	if (info)
	{
		for (long i = 0; i < info->unknowne0; i++)
		{
			byte *section = info->unknowne4 + i * 0x14;

			if (*(short *)(section + 0x12) == key_b && *(short *)(section + 0x10) == key_a)
			{
				result = (object->unknowne0 & (1 << i)) != 0;
				break;
			}
		}
	}
	return result;
}

// @retail 0xdb540
bool function_db540(long object_index, long key_a, long key_b)
{
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool result = false;

	if (info)
	{
		for (long i = 0; i < info->unknowne0; i++)
		{
			byte *section = info->unknowne4 + i * 0x14;

			if (*(short *)(section + 0x12) == key_b && *(short *)(section + 0x10) == key_a)
			{
				result = (object->unknowne2 & (1 << i)) != 0;
				break;
			}
		}
	}
	return result;
}

/* restores an object's damage state: clears its damage flags and the
   destroyed permutations of its regions that can be restored */
// @retail 0xdb5c0
void function_db5c0(long object_index)
{
	if (g_4e6948->mode != 4)
	{
		s_damage_object *object = DAMAGE_OBJECT(object_index);
		s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

		*(word *)&object->damage_flags = 0;
		if (info)
		{
			s_object_region_state *states = (s_object_region_state *)((byte *)object + object->region_states_offset);

			for (long i = 0; i < info->region_count; i++)
			{
				s_damage_info_region *region = &info->regions[i];

				if (region->flags & 0x10)
				{
					states[i].destroyed_permutations = 0;
					if (region->unknown34 != NONE)
						function_ba6f0(object_index, region->unknown34, 0, true);
				}
			}
		}
	}
}

/* when no biped is left among a vehicle's children, lets it be destroyed
   again and clears its regions' damage levels */
// @retail 0xdb670
void function_db670(long object_index, long vehicle_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted))
	{
		bool no_bipeds = true;
		s_object_child_iterator iterator;

		function_d0620(vehicle_index, &iterator);
		while (function_d0690(&iterator))
		{
			if ((1 << DAMAGE_OBJECT(iterator.child_index)->type) & 1)
				no_bipeds = false;
		}
		if (no_bipeds)
		{
			s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);
			s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset);

			vehicle->damage_flags.unknown12 = false;
			if (0.25f > vehicle->body_vitality)
				vehicle->body_vitality = 0.25f;
			for (long i = object->region_states_size / (long)sizeof(s_object_region_state); i > 0; i--, state++)
			{
				if (state->unknown02 > 0)
				{
					state->unknown02 = 0;
					state->damage_level_ticks = 0;
				}
			}
		}
	}
}

/* a permutation destroyed elsewhere (over the network): starts its delayed
   effect, or destroys it here */
// @retail 0xdb760
void function_db760(long region_index, long permutation_index, long object_index, long mode)
{
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

	if (info)
	{
		s_damage_owner owner = { 0 };

		owner.object_index = NONE;
		owner.player_index = NONE;
		owner.team = NONE;
		if (mode == 1)
		{
			s_damage_info_permutation *permutation = &info->regions[region_index].permutations[permutation_index];

			function_d9d60((permutation->flags >> 20) & 1, permutation->unknown38, object_index, permutation->effect_index,
				&owner);
		}
		else
		{
			s_damage_region_accumulator accumulator = { 0 };

			function_da110(permutation_index, info, object_index, &owner, region_index, &accumulator);
		}
	}
}

real function_1588b0(long player_index, long mode);
void function_13a6e8(long player_index, real amount);
void function_a7a30(long object_index, dword mask);
bool function_138880();

/* an object's damage per tick: delayed permutations, stun timers, shield
   and body recharge, the regions' damage levels and the recent damage;
   returns whether anything changed */
// @retail 0xd5de0
bool object_damage_update(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	bool changed = false;

	if (((1 << object->type) & 3) && g_4e6948->state == 1 && g_4f55e4 && object->player_index != NONE)
		object->shield_stun_ticks = 10;
	if (!info)
		return changed;

	byte *bounds = NULL;

	if (info->region_count > 0)
	{
		long state_count = object->region_states_size / (long)sizeof(s_object_region_state);
		s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset);

		for (long i = 0; i < state_count; i++, state++)
		{
			word pending = state->pending;

			if ((word)(pending >> 3) > 0)
			{
				pending = (word)((((pending >> 3) - 1) << 3) | (pending & 7));
				changed = true;
				state->pending = pending;
				if (!(pending & 0xfff8))
				{
					s_damage_object *current = DAMAGE_OBJECT(object_index);
					s_damage_owner owner;
					long permutation_index = pending & 7;

					owner.object_index = current->owner_object_index;
					owner.player_index = current->owner_player_index;
					owner.team = current->owner_team;
					if (function_d9f70(i, permutation_index, info, object_index))
					{
						s_damage_region_accumulator accumulator;

						memset(&accumulator, 0, sizeof(accumulator));
						function_da110(permutation_index, info, object_index, &owner, i, &accumulator);
						if (accumulator.unknown2c || accumulator.unknown2e)
							function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);
						if (accumulator.flags & 4)
							object_destroy(object_index);
					}
				}
			}
		}
	}

	if (((1 << object->type) & 3) && object->unknown12c != NONE)
		bounds = (byte *)function_1e4a10(*(long *)(g_4f55f0->data + (object->unknown12c & 0xffff) * 0x888 + 0x54));

	if (TEST_FIELD_BIT(object->damage_flags.unknown13))
	{
		s_damage_owner owner;

		object_get_damage_owner(object_index, &owner);
		object_deplete_body(object_index, &owner, true, false);
		object->damage_flags.unknown13 = false;
	}
	if (TEST_FIELD_BIT(object->damage_flags.unknown5) || TEST_FIELD_BIT(object->damage_flags.unknown6) ||
		TEST_FIELD_BIT(object->damage_flags.unknown10))
	{
		if (g_4e6948->mode != 4)
		{
			function_dbfb0(object_index, NULL, TEST_FIELD_BIT(object->damage_flags.unknown6),
				TEST_FIELD_BIT(object->damage_flags.unknown10), false);
		}
		object->damage_flags.unknown5 = false;
		object->damage_flags.unknown6 = false;
		object->damage_flags.unknown10 = false;
	}

	long state = g_4e6948->state;
	real maximum_shield = 1.0f;

	if (state == 2 && ((1 << object->type) & 3) && object->player_index != NONE)
		maximum_shield = function_1588b0(object->player_index, 2);

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted) && object->maximum_shield_vitality > 0.0f)
	{
		if (TEST_FIELD_BIT(object->damage_flags.shield_double_charged))
		{
			object->shield_vitality += 2.0f / (g_510c54->ticks_per_second * 2.0f);
			if (object->shield_vitality < 3.0f)
			{
				object->damage_flags.shield_recharging = true;
				changed = true;
			}
			else
			{
				object->shield_vitality = 3.0f;
				object->damage_flags.shield_double_charged = false;
				object->damage_flags.shield_recharging = false;
			}
		}
		else if (object->shield_vitality > maximum_shield && state == 2)
		{
			long player_index = unit_get_player_index(object_index);
			real drain = 2.0f / (g_510c54->ticks_per_second * 90.0f);
			real excess;

			object->damage_flags.shield_recharging = false;
			excess = object->shield_vitality - 1.0f;
			if (drain > excess)
			{
				object->shield_vitality = maximum_shield;
				if (player_index != NONE)
					function_13a6e8(player_index, excess);
			}
			else
			{
				object->shield_vitality -= drain;
				if (player_index != NONE)
					function_13a6e8(player_index, drain);
			}
			function_a7a30(object_index, 0x80);
			changed = true;
		}
		else if (maximum_shield > object->shield_vitality)
		{
			short stun_ticks = object->shield_stun_ticks;

			if (stun_ticks == 0)
			{
				real rate = bounds ? *(real *)(bounds + 0x6c) : *(real *)((byte *)info + 0xd0);
				real recharge = g_510c54->rate * rate;

				if ((1 << object->type) & 3)
					recharge = function_1e9720(3, object->team) * recharge;
				if (TEST_FIELD_BIT(object->damage_flags.shield_depleted))
				{
					object->damage_flags.shield_depleted = false;
					function_ba7f0(object_index, NONE, 2, 0);
				}
				if (!TEST_FIELD_BIT(object->damage_flags.shield_recharging))
				{
					s_damage_owner owner;

					object_get_damage_owner(object_index, &owner);
					function_176780(object_index, (s_effect_owner const *)&owner, 0.0f, *(long *)((byte *)info + 0xb8), 0.0f,
						NULL, NULL);
				}
				object->damage_flags.shield_recharging = true;
				object->shield_vitality += recharge;
				if (object->shield_vitality > maximum_shield)
				{
					object->shield_vitality = maximum_shield;
					object->damage_flags.shield_recharging = false;
					object->damage_flags.shield_damaged = false;
					function_a7a30(object_index, 0x80);
				}
				else
				{
					changed = true;
					function_a7a30(object_index, 0x80);
				}
			}
			else
			{
				if (!function_138880() && stun_ticks != 0x7fff)
				{
					object->shield_stun_ticks = stun_ticks - 1;
					changed = true;
				}
				object->damage_flags.shield_recharging = false;
				function_a7a30(object_index, 0x80);
			}
		}
		else
		{
			object->damage_flags.shield_recharging = false;
		}
	}

	if (object->maximum_body_vitality > 0.0f && !TEST_FIELD_BIT(object->damage_flags.body_depleted))
	{
		real maximum_body;
		real rate;

		if (bounds)
		{
			maximum_body = *(real *)(bounds + 0x14);
			rate = *(real *)(bounds + 0x68);
		}
		else
		{
			maximum_body = *(real *)((byte *)info + 0x38);
			rate = *(real *)((byte *)info + 0xd4);
		}
		if (maximum_body > object->body_vitality && rate > 0.0f)
		{
			short stun_ticks = object->body_stun_ticks;

			if (stun_ticks == 0)
			{
				real recharge = g_510c54->rate * rate;

				if ((1 << object->type) & 3)
					recharge = function_1e9720(3, object->team) * recharge;
				object->damage_flags.body_recharging = true;
				object->body_vitality += recharge;
				if (object->body_vitality > maximum_body)
				{
					object->body_vitality = maximum_body;
					object->damage_flags.body_recharging = false;
				}
				else
				{
					changed = true;
				}
			}
			else if (!function_138880())
			{
				object->body_stun_ticks = stun_ticks - 1;
				changed = true;
			}
			function_a7a30(object_index, 0x40);
		}
	}

	if (object->maximum_body_vitality > 0.0f && !TEST_FIELD_BIT(object->damage_flags.body_depleted))
	{
		long state_count = object->region_states_size / (long)sizeof(s_object_region_state);
		s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset);

		for (long i = 0; i < state_count; i++, state++)
		{
			if (state->unknown02 > 0)
			{
				if (state->damage_level_ticks == 0)
				{
					if (i < info->region_count && *(real *)((byte *)&info->regions[i] + 0x2c) > 0.0f)
					{
						real decay = g_510c54->rate * *(real *)((byte *)&info->regions[i] + 0x2c) * 255.0f;
						long amount;

						__asm
						{
							fld decay
							fistp amount
						}
						if ((long)state->unknown02 > amount)
						{
							state->unknown02 -= (byte)amount;
						}
						else
						{
							state->unknown02 = 0;
							continue;
						}
						changed = true;
					}
				}
				else
				{
					state->damage_level_ticks--;
					changed = true;
				}
			}
		}
	}

	changed |= function_d8b50(&object->unknownf8, &object->unknown100, (char *)&object->unknown109, (byte const *)info,
		TEST_FIELD_BIT(object->damage_flags.body_depleted));
	if (TEST_FIELD_BIT(object->damage_flags.unknown11))
	{
		if ((char)object->unknown108 > 0)
		{
			object->unknownf4 = object->unknownfc + object->unknownf4;
			object->unknown108--;
		}
		return true;
	}
	return changed | function_d8b50(&object->unknownf4, &object->unknownfc, (char *)&object->unknown108,
		(byte const *)info, TEST_FIELD_BIT(object->damage_flags.shield_depleted));
}

/* an object's render model sections by list (unknown_181a80.cpp) */
struct s_section_list
{
	byte sections[0x40];
	long count;
};

struct s_section_lists
{
	long count;
	s_section_list lists[256];
	s_section_list unlisted;
};

s_section_lists *function_181a80(s_section_lists *lists, long object_index, bool all_sections);

/* walks the constraints of a physics model, of six kinds */
struct s_physics_constraint_iterator
{
	byte *physics;
	short type;
	short index;
};

struct s_physics_constraint_block
{
	long count;
	byte *elements;
};

void function_1eb110(s_physics_constraint_iterator *iterator);
void function_1eb160(s_physics_constraint_iterator *iterator);
struct s_physics_model_shape_key;
struct s_impact_tag_block;
s_impact_tag_block *physics_model_shape_block_get(byte *physics_model, s_physics_model_shape_key const *key, long *element_size);
long render_model_find_marker_group(long render_model_index, long index);

PRIVATE inline s_section_list *section_list_get(s_section_lists *lists, long list_index)
{
	return list_index == NONE ? &lists->unlisted : &lists->lists[list_index];
}

/* the model node a physics node belongs to (a copy of
   render_model_find_marker_group, 0x16d890) */
PRIVATE inline long model_find_physics_node(byte *model, short physics_node)
{
	long result = NONE;

	for (long i = 0; i < *(long *)(model + 0x70); i++)
	{
		if ((short)*(char *)(*(byte **)(model + 0x74) + i * 0x10 + 5) == physics_node)
		{
			result = i;
			break;
		}
	}
	return result;
}

/* the model nodes still joined to a node through constraints that hold
   (those whose damage sections are intact) */
// @retail 0xdb810
dword function_db810(long object_index, long node_index)
{
	byte *object_definition = g_4e3b44[DAMAGE_OBJECT(object_index)->tag_index & 0xffff].bytes;
	dword node_mask = 1 << node_index;
	long model_index = *(long *)(object_definition + 0x38);

	if (model_index != NONE)
	{
		byte *model = g_4e3b44[model_index & 0xffff].bytes;

		if (*(long *)(model + 0x24) != NONE)
		{
			s_section_lists lists;
			byte *physics;
			bool changed;

			function_181a80(&lists, object_index, true);
			physics = g_4e3b44[*(long *)(model + 0x24) & 0xffff].bytes;
			node_mask |= 1 << node_index;
			do
			{
				s_physics_constraint_iterator iterator;

				iterator.physics = g_4e3b44[*(long *)(model + 0x24) & 0xffff].bytes;
				changed = false;
				function_1eb110(&iterator);
				for (;;)
				{
					long element_size;
					s_physics_constraint_block *block = (s_physics_constraint_block *)physics_model_shape_block_get(iterator.physics,
						(s_physics_model_shape_key const *)&iterator.type, &element_size);

					if (iterator.index >= block->count)
						break;

					byte *constraint = block->elements + iterator.index * element_size;

					if (!constraint)
						break;
					if (!function_db4c0(object_index, iterator.type, iterator.index))
					{
						short list_a = *(short *)(constraint + 4);
						word list_b = *(word *)(constraint + 6);

						if (list_a != NONE && list_b != 0xffff)
						{
							for (long i = 0; i < section_list_get(&lists, list_a)->count; i++)
							{
								for (long j = 0; j < section_list_get(&lists, (short)list_b)->count; j++)
								{
									long section_a = (char)section_list_get(&lists, list_a)->sections[i];
									long section_b = (char)section_list_get(&lists, (short)list_b)->sections[j];

									if (section_a != NONE && section_b != NONE)
									{
										byte *rows = *(byte **)(physics + 0x3c);
										byte *node_model = g_4e3b44[*(long *)(object_definition + 0x38) & 0xffff].bytes;
										long node_a = model_find_physics_node(node_model, *(short *)(rows + section_a * 0x90 + 2));
										long node_b = model_find_physics_node(node_model, *(short *)(rows + section_b * 0x90 + 2));

										if (node_a != NONE && node_b != NONE)
										{
											dword bit_a = 1 << node_a;
											dword bit_b = 1 << node_b;

											if (((node_mask & bit_a) != 0) != ((node_mask & bit_b) != 0))
											{
												node_mask |= bit_a | bit_b;
												changed = true;
											}
										}
									}
								}
							}
						}
					}
					function_1eb160(&iterator);
				}
			} while (changed);
		}
	}
	return node_mask;
}

/* the model node of the object's lowest ranked physics node */
// @retail 0xdbb40
long function_dbb40(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	long model_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38);
	long result = 0x7fffffff;
	long lowest_rank = 0x7fffffff;

	if (model_index != NONE && object->node_index != NONE)
	{
		long physics_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0x24);

		if (physics_index != NONE)
		{
			byte *physics = g_4e3b44[physics_index & 0xffff].bytes;
			byte *component = g_51e9b8->data + (object->node_index & 0xffff) * 0xa0;
			long rigid_body_count = *(long *)(component + 0x74);
			byte *rigid_body = *(byte **)(component + 0x70) + 0x48;

			for (long i = rigid_body_count; i > 0; i--, rigid_body += 0x60)
			{
				long node_count = *(long *)(rigid_body + 4);

				for (long j = 0; j < node_count; j++)
				{
					byte *row = *(byte **)(physics + 0x3c) + (*(char **)rigid_body)[j] * 0x90;
					long rank = *(short *)row;

					if (rank < lowest_rank)
					{
						lowest_rank = rank;
						result = render_model_find_marker_group(model_index, *(short *)(row + 2));
					}
				}
			}
		}
	}
	return result;
}

struct s_havok_component;

/* a havok component's node matrices and velocities (unknown_1cec30.cpp) */
struct s_havok_node_states
{
	dword valid[2];
	real_matrix4x3 matrices[64];
	real_vector3d linear_velocities[64];
	real_vector3d angular_velocities[64];
};

void havok_component_node_states_get(s_havok_component *component, s_havok_node_states *states);
void havok_component_node_states_set(s_havok_component *component, s_havok_node_states const *states);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
real_point3d *function_b9dd0(long object_index, real_point3d *result);
long function_b7b40(void *creation);
void __stdcall function_1c3770(long object_index, dword flags);
extern s_data_array *g_51e9b8;

/* what a new object is made from (b7930, b7b40) */
struct s_damage_object_creation
{
	byte unknown00[0x1c];
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_vector3d linear_velocity;
	real_vector3d angular_velocity;
	byte unknown58[0xac - 0x58];
	dword node_mask;
	word section_mask_a;
	word section_mask_b;
	byte unknownb4[4];
	bool unknownb8;
	byte unknownb9[3];
	long unknownbc;
	long unknownc0;
};

/* marks damage sections destroyed, and splits off the model nodes no
   longer joined to the object's root as new objects */
// @retail 0xdbc80
void function_dbc80(long object_index, short section_mask_a, short section_mask_b)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	byte *object_definition = g_4e3b44[object->tag_index & 0xffff].bytes;
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

	if (info)
	{
		for (long i = 0; i < info->unknowne0; i++)
		{
			if ((1 << i) & (word)section_mask_a)
				object->unknowne0 |= 1 << i;
			if ((1 << i) & (word)section_mask_b)
				object->unknowne2 |= 1 << i;
		}
	}

	dword attached_nodes = function_db810(object_index, function_dbb40(object_index));

	if (*(long *)(object_definition + 0x38) != NONE && object->node_index != NONE)
	{
		s_havok_component *component = (s_havok_component *)(g_51e9b8->data + (object->node_index & 0xffff) * 0xa0);
		s_havok_node_states states;

		havok_component_node_states_get(component, &states);
		if (section_mask_a)
		{
			s_damage_object *current = DAMAGE_OBJECT(object_index);
			byte *node_states = (byte *)current + *(short *)((byte *)current + 0x11a);
			long node_count = *(short *)((byte *)current + 0x118) / 10;
			dword detached_nodes = 0;
			long i;

			for (i = 0; i < node_count; i++)
			{
				if (node_states[i] != 0xff && !(attached_nodes & (1 << i)))
				{
					byte *node = node_states + node_count * 2 + i * 8;

					node_states[i] = 0xff;
					detached_nodes |= 1 << i;
					node[0] = 0xff;
					node[1] = 0;
					node[2] = 0;
					*(long *)(node + 4) = NONE;
				}
			}
			while (detached_nodes > 0 && *(long *)(object_definition + 0x40) != NONE)
			{
				long first_node = NONE;
				s_damage_object_creation creation;

				for (i = 0; i < node_count; i++)
				{
					if (detached_nodes & (1 << i))
					{
						first_node = i;
						break;
					}
				}

				dword piece_nodes = function_db810(object_index, first_node);

				detached_nodes &= ~piece_nodes;
				function_b7930(&creation, *(long *)(object_definition + 0x40), object_index, 0);
				creation.unknownc0 = *(long *)((byte *)object + 0x2c);
				creation.unknownbc = *(long *)((byte *)object + 0x28);
				creation.unknownb8 = true;
				function_b9dd0(object_index, &creation.position);
				creation.up = *(real_vector3d *)((byte *)object + 0x7c);
				creation.forward = *(real_vector3d *)((byte *)object + 0x70);
				object_get_velocities(object_index, &creation.linear_velocity, &creation.angular_velocity);
				creation.node_mask = ~piece_nodes;
				creation.section_mask_a = object->unknowne0;
				creation.section_mask_b = object->unknowne2;

				long piece_index = function_b7b40(&creation);

				if (piece_index != NONE && DAMAGE_OBJECT(piece_index)->node_index != NONE)
				{
					havok_component_node_states_set(
						(s_havok_component *)(g_51e9b8->data + (DAMAGE_OBJECT(piece_index)->node_index & 0xffff) * 0xa0),
						&states);
				}
			}
		}
		function_1c3770(object_index, 0);
		havok_component_node_states_set(component, &states);
	}
}
