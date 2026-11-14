// @flags /O2 /Ob1 /arch:SSE /Gr
/* DAMAGE.CPP: object damage */

#include "unknown_11c920.h"
#include "bit_vector.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include "object_tag_definitions.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_1dee50.h"
#include "vector_products.h"
#include "bit_vector.h"
#include <math.h>
#include <string.h>

typedef long string_handle;

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
	string_handle armor;
	real multiplier;
};

struct s_damage_group
{
	string_handle name;
	s_tag_block_of<s_armor_modifier> armor_modifiers;
};

struct s_damage_table
{
	s_tag_block_of<s_damage_group> damage_groups;
};

/* the object fields the damage code reads (g_4e0300 holds 12 byte headers
   with the object at +8) */
/* an object's damage flags, at +0x10a */
struct s_damage_object_flags
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
};

/* the bit numbers of those flags in the word that holds them */
enum e_damage_object_flag
{
	_damage_object_flag_shield_damaged_bit = 1,
	_damage_object_flag_shield_depleted_bit = 3,
	_damage_object_flag_shield_double_charged_bit = 4,
	_damage_object_flag_unknown5_bit = 5,
	_damage_object_flag_unknown6_bit = 6,
	_damage_object_flag_body_recharging_bit = 8,
	_damage_object_flag_shield_recharging_bit = 9,
	_damage_object_flag_unknown10_bit = 10,
	_damage_object_flag_unknown13_bit = 13
};

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
	point3f field_x221477;
	real field_xae9109;
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
	union
	{
		s_damage_object_flags damage_flags;
		word damage_flags_word;
	};
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
	vector3f unknown168;
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
	short unknown0a;
};

/* a damage event (0x88 bytes) */
struct s_type_1e6529
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
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	vector3f cone_direction;
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

/* what damaging an object collects (0x34 bytes; function_d7b80's
   per-object state, passed down to the shield, body and region code) */
struct s_damage_region_accumulator
{
	void *unknown00;
	s_damage_info *field_4_2;
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
	string_handle damage_group;
	string_handle damage_group_b;
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
	string_handle unknown3c;
	string_handle unknown40;
	real skip_chance;
	string_handle unknown48;
	real body_threshold;
};

struct s_damage_info_region
{
	string_handle name;
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
	union
	{
		word pending;
		struct
		{
			word pending_permutation : 3;
			word pending_ticks : 13;
		};
	};
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
bool function_1df560(short team_a, short team_b);

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
	real ray_spread;
	byte unknown1c[0x28 - 0x1c];
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
void function_d0620(s_object_child_iterator *iterator, long object_index);
void function_b7360(long object_index);
void __stdcall function_b8540(long a);
void function_b8b70(long object_index);
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c);
void __stdcall function_dae60(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	s_damage_region_accumulator *accumulator);
void __stdcall function_da110(long permutation_index, s_damage_info *info, long object_index, s_damage_owner const *owner,
	long region_index, s_damage_region_accumulator *accumulator);
void function_d9d60(long object_index, long marker_name, long effect_index, s_damage_owner const *owner,
	bool at_marker);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);
void function_a8360(long object_index, long region_index, long permutation_index, long a);
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14);
void function_dbc80(long object_index, short section_mask_a, short section_mask_b);
struct s_damage_report;
void function_d9640(s_damage_report const *report, long object_index);
void __stdcall function_e6460(long object_index);
long function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b, point3f const *origin, vector3f const *direction);
void __stdcall function_ba7f0(long object_index, long a, long b, long c);
short __stdcall function_bb050(long a, dword type_mask, void const *location, point3f const *position, real radius,
	long *objects, short maximum_count);
void __stdcall function_d74e0(s_type_1e6529 *data, long object_index, bool child);
real function_30bf0(vector3f *v);
void function_baff0(long object_index, point3f const *origin, point3f *arg_149545, vector3f *normal);
bool function_d6f90(long object_index, point3f const *point, s_type_1e6529 const *data);
long unit_get_player_index(long unit_index);
void __stdcall function_153d10(long team, long definition_index, void *a, void *b, long c, real d, real e, bool f);
bool function_d74b0(byte const *owner);

void __stdcall function_184250(s_type_1e6529 const *data);

long function_d5b60(long object_index);
real function_1e9720(short kind, short team);
bool function_108fd0(long object_index);
void function_b58c0(long index, dword mask);

typedef long (__stdcall *t_bsearch_compare_function)(const void *, const void *, const void *);
long function_13ddd0(const void *key, const void *base, long count, long element_size, t_bsearch_compare_function compare, const void *context);
long __stdcall cache_tag_group_compare(void const *a, void const *b, void const *context);

// @retail 0xd5bc0
real function_d5bc0(string_handle group_a, string_handle group_b, string_handle armor_a, string_handle armor_b)
{
	s_damage_table *table = g_4e034c->damage_table;
	real result = 1.0f;
	string_handle groups[2] = { group_a, group_b };
	string_handle armors[2] = { armor_a, armor_b };

	for (dword i = 0; i < 2; i++)
	{
		string_handle group_key = groups[i];
		long group_index = function_13ddd0(&group_key, table->damage_groups.elements, table->damage_groups.count,
			sizeof(s_damage_group), cache_tag_group_compare, NULL);

		if (group_index == NONE)
			continue;

		s_damage_group *group = &table->damage_groups.elements[group_index];
		for (dword j = 0; j < 2; j++)
		{
			string_handle armor_key = armors[j];
			long modifier_index = function_13ddd0(&armor_key, group->armor_modifiers.elements, group->armor_modifiers.count,
				sizeof(s_armor_modifier), cache_tag_group_compare, NULL);

			if (modifier_index != NONE)
				result *= group->armor_modifiers.elements[modifier_index].multiplier;
		}
	}
	return result;
}

// @retail 0xd5c90
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	byte *field_4_2 = (byte *)function_d5b60(object_index);
	real maximum_body = 0.0f;
	real maximum_shield = 0.0f;

	if (field_4_2)
	{
		maximum_body = *(real *)(field_4_2 + 0x28);
		maximum_shield = *(real *)(field_4_2 + 0x8c);
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
real function_d5d20(long object_index, bool ignore_difficulty)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real result = object->maximum_body_vitality;

	if (!ignore_difficulty && ((1 << object->type) & 3))
	{
		/* a short local: retail sign-extends it into eax and copies it to ecx */
		short team = object->team;
		result = function_1e9720(1, team) * result;
	}
	return result;
}

// @retail 0xd5d80
real function_d5d80(long object_index, bool ignore_difficulty)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real result = object->maximum_shield_vitality;

	if (!ignore_difficulty && ((1 << object->type) & 3))
	{
		short team = object->team;
		result = function_1e9720(2, team) * result;
	}
	return result;
}

// score 3 (was 3): with the callers' image, only the schedule differs: ours puts `or eax, -1` right after the
// load of g_47d8e0 (before that value's store at +0x7c); retail stores it first, loads g_4686c4, then does the
// `or`. Tried: unknown22 assigned right after unknown7c (11), g_4686c4 in a local (3, same code), both globals
// in locals (28), team/player/object order swapped (5), unknown7c before definition_index (7), chained 1.0f
// stores (5). A one-function check scores this differently (5); judge it on the 515-source set.
// Also tried: +0x1c typed as s_location (3), the owner copied from the const NONE owner (42), unknown18 before
// the location (7), `player_index = object_index = NONE` (3), `unknown1c = unknown14 = NONE` (3). The alias status
// of both globals is retail's (g_47d8e0 address-taken there too, g_4686c4 not), so only the scheduler is left.
// M272: complete long/short/both sentinel chains 9/14/18 fast; adjacent byte clear alone/with long
// chain 18/18 fast (unchanged reduced preview 5 versus full 3). Short chain plus byte clear: Wine hang;
// remaining two prepared forms not run. Original body restored; the full-image OR schedule remains.
// R75: second global read after definition/before memset 3/3; both globals read early together 3; owner pointer 3;
// mutable/const owner aggregate 54/54; aggregate member reads 3; definition parameter through its address 3.
// Round 307: clear result reused 3; zero event aggregate copy 79; owner long/complete
// payload memset 36/36; individual long/short sentinel memsets 3/3;
// owner longs plus long sentinels 36; complete owner plus remaining sentinel memsets 35.
// P422: Complete zero-clear partitions at flags/owner/handle/location/vector/scale/
// material/tail boundaries 34/43/55/63/66/45/50/41, 111-133/104 bytes.
// NOT kept: split bulk clearing adds stores or another clear and lengthens native code.
// MiniMax G (11-12): no new form, no lower score (3): nothing new this round; ours still materialises `or eax,-1` right after the
// load of g_47d8e0 where retail stores that word at +0x7c first, loads g_4686c4 and only then materialises
// the sentinel.
// MiniMax G (11-12): no new form, no lower score (3): ours materialises `or eax, -1` right after the load of
// g_47d8e0, where retail stores that word at +0x7c first and only then loads g_4686c4 and does the `or`; the
// assignment orders around the sentinel stores and a local for either global leave the same schedule.
// MiniMax A (11-12, run 63): no new form, no lower score (3); the unknown18 assignment moved above the unknown22
// one is 5, and the two global loads stay where they are, so the constant's place is still the only difference.
// MiniMax A (11-12, run 62): no new form, no lower score (3); the owner.object_index assignment moved below the
// unknown22 assignment 12, the permuter on this function was stopped before it produced a try, and `or eax, -1`
// still comes right after the load of g_47d8e0 where retail stores that word at +0x7c first, loads g_4686c4 and
// only then materialises the constant.
// MiniMax H (11-12): no new form, no lower score (3): the `or eax,-1` still comes before the word store of g_47d8e0 where retail stores that word first, loads g_4686c4 and only then materialises the sentinel; a permuter run over this body was stopped before it printed a baseline.
// MiniMax C (11-09): no new form, no lower score (3); a 300 s permuter run (24 tries) found nothing lower.
// MiniMax E (11-09): no new form, no lower score (3); the `or eax, -1` still comes right after the load of g_47d8e0, where retail stores that word at +0x7c first, loads g_4686c4 and only then does the `or`.
// MiniMax D (11-06, run 49): no new form, no lower score (3); the `or eax, -1` still comes right after the load of g_47d8e0, retail stores that value at +0x7c first, loads g_4686c4 and only then does the `or`.
// MiniMax A (11-04, run 41): no new form, no lower score (3); the permuter on this function was stopped by the
// harness before it produced a try, and only the `or eax, -1` schedule against the two global loads is left.
// MiniMax B (11-06, run 42): no new form and no lower score (3); retail stores the g_47d8e0 word at +0x7c, then
// loads g_4686c4 and only then materialises `or eax, -1`; ours materialises the -1 right after the
// definition-index store, so the constant sits two instructions early.
// Round 163 (m3 lane 8, plain; the fast check carries a CAVEAT here): the +0x7c store as the existing material-type
// class (callers clear +0x7c to NONE as a default constructor would) built from the global 3 (same code); the same as an
// aggregate copy of the global viewed as that type 3; as an unsigned 16-bit copy 3; a barrier after the definition
// store (diagnostic) 12 (the 1.0 load sinks and `or eax, -1` still fills the load slot); the owner's three NONE stores
// through a forced-inline helper 3. Ours fills the 16-bit load's latency with the `or`; retail stores right after it.
// Also round 163 (plain): `data = (s_type_1e6529 *)memset(...)` 3 (same code); the +0x7e NONE store moved up beside +0x7c 15;
// the +0x84 field typed `byte` and cleared with 0 3 (same code).
// @retail 0xd6660
void function_d6660(s_type_1e6529 *data, long definition_index)
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
bool function_d6790(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool result = false;

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted) && object->body_vitality < 1.0f)
	{
		object->body_vitality = 1.0f;
		s_damage_object *damage = DAMAGE_OBJECT(object_index);
		if (damage->unknownd4 != NONE)
			function_b58c0(damage->unknownd4, 0x40);
		result = true;
	}
	return result;
}

// @retail 0xd6af0
bool function_d6af0(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool result = object->shield_vitality <= 1.0f;

	if (result)
	{
		object->damage_flags.shield_double_charged = true;
		if (object->shield_vitality == 0.0f)
			object->shield_vitality = 0.01f;
		object->shield_stun_ticks = 0;
	}
	return result;
}

// @retail 0xd6b60
void __stdcall function_d6b60(long object_index)
{
	long child_index = DAMAGE_OBJECT(object_index)->first_child_object_index;

	while (child_index != NONE)
	{
		long next_index = DAMAGE_OBJECT(child_index)->next_object_index;

		if (!function_108fd0(child_index))
			function_d6b60(child_index);
		child_index = next_index;
	}
}

/* who an object's damage is credited to */
// @retail 0xd66d0
void __stdcall object_get_damage_owner(long object_index, s_damage_owner *owner)
{
	if (object_index != NONE)
	{
		s_damage_object *object = DAMAGE_OBJECT(object_index);

		if ((1 << object->type) & 3)
		{
			if (object->unknown248 != NONE)
			{
				object_get_damage_owner(object->unknown248, owner);
				/* always true here, so it compiles to nothing; without a test
				   after the call the compiler turns this tail call into a loop,
				   where retail jumps back to its own entry (and so keeps the
				   stack convention its callers use) */
				if (object->unknown248 != NONE)
					return;
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
	else
	{
		*owner = *g_467420;
	}
}

// MATCH (coordinator 11-11 on r754): MiniMax C's held word_flags_set form (11-09), kept now that 0x54590 (network_voice.cpp),
// which it cost at link time, is an accepted loss on main (r753); full check: gained 0xd6a70, lost none.
// @retail 0xd6a70
void function_d6a70(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (!TEST_FIELD_BIT(object->damage_flags.shield_depleted))
	{
		long field_4_2 = function_d5b60(object_index);

		if (field_4_2 && object->maximum_shield_vitality > 0.0f)
			function_176780(object_index, (s_effect_owner const *)g_467420, 0.0f, *(long *)(field_4_2 + 0xb0), 0.0f, NULL, NULL);
		object->unknownf4 = 0.0f;
		word_flags_set((word *)&object->damage_flags, 3, true);
		function_ba7f0(object_index, NONE, 2, NONE);
	}
}

/* Retail keeps the standard stack convention here. The body matches only with it: with this marker every byte agrees;
   without it (with or without `__stdcall`) LTCG's custom regime keeps the object index in EBP and reloads g_4e0300 at each
   lookup (172, 600 bytes). Retail holds no reference to the address (no dword 0x000d6800 in the image), and its callers
   0xc42e0, 0xd5de0, 0xd6bc0, 0xd7b80, 0xd8cb0, 0xda110 (two sites), 0xdc0a0 and 0x11a320 are all LTCG code that pushes
   all four arguments. Tried without the marker: a volatile read of the index parameter (162-203), a read through its
   address (184), a local pool pointer (162), the flag writes in byte and word forms (170-190). */
// @retail 0xd6800 standard
void __stdcall function_d6800(long object_index, s_damage_owner const *owner, bool notify_parent, bool unknown)
{
	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	s_damage_object *object = datum->object;
	s_damage_region_accumulator accumulator;

	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.body_depleted))
		return;

	object->damage_flags.body_depleted = true;
	function_b8b70(object_index);
	memset(&accumulator, 0, sizeof(accumulator));
	dword flags = accumulator.flags;

	if (unknown)
		flags |= 0x400;
	else
		flags &= ~0x400;

	s_damage_object *notified = DAMAGE_OBJECT(object_index);

	accumulator.flags = flags;
	if (notified->unknownd4 != NONE)
		function_b58c0(notified->unknownd4, 1);

	if (g_4e6948->mode != 4)
	{
		s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

		if (info)
		{
			for (long i = 0; i < info->region_count; i++)
			{
				if (info->regions[i].flags & 2)
					function_dae60(info, object_index, owner, i, &accumulator);
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

	function_d6a70(object_index);

	if (g_4e6948->mode != 4 && object->parent_object_index != NONE && notify_parent)
	{
		s_damage_object *parent = DAMAGE_OBJECT(object->parent_object_index);

		if (TEST_FIELD_BIT(DAMAGE_OBJECT(object->parent_object_index)->damage_flags.unknown12) && ((1 << parent->type) & 2))
		{
			s_object_child_iterator iterator;
			bool last = true;

			function_d0620(&iterator, object->parent_object_index);
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
void __stdcall function_d6bc0(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_region_accumulator accumulator;

	function_d6800(object_index, g_467420, true, false);

	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

	memset(&accumulator, 0, sizeof(accumulator));
	if (info)
	{
		for (long i = 0; i < info->region_count; i++)
		{
			if (info->regions[i].flags & 8)
				function_dae60(info, object_index, g_467420, i, &accumulator);
		}
	}
	if (object->type == 0)
		function_e6460(object_index);
	function_d6b60(object_index);
	function_b8540(object_index);
}

/* whether a damage event can affect an object */
// @retail 0xd72e0
bool function_d72e0(long object_index, s_type_1e6529 const *data)
{
	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *object = datum->object;
	bool result = false;

	if (!(datum->flags & 0x10) && !(object->unknown04[0] & 1))
	{
		if (definition->flags0c & 2)
		{
			s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);
			long player_index = NONE;

			if (unit)
				player_index = unit->player_index;
			result = player_index != NONE;
		}
		else
		{
			dword flags = definition->flags14;

			if (!((flags & 1) && object_index == data->owner.object_index))
			{
				if (!((1 << object->type) & 3))
					result = true;
				else if (!((flags & 0x8000) && g_4e6948->state == 1 && object->player_index != NONE))
				{
					if (!(flags & 8) || function_1df560(object->team, data->owner.team))
						result = true;
				}
			}
		}
	}
	return result;
}

/* whether any rigid body of the havok component (the 0x60 byte entries of the
   array at +0x70, counted at +0x74) is fixed: the byte at +0x40 of the Havok
   body each entry points to at +0x40 */
// Standard convention (DECOMPILING.md evidence): (1) this body matches retail byte for byte only with the
// standard convention (the component in ecx); with LTCG's own convention a direct call moves it to edx (8).
// (2) Retail holds no absolute reference to 0xd74b0 and has one call, 0xd7603 in 0xd74e0 (LTCG code,
// `mov ecx, edi`). (3) Tried (m3 lane 16 r114): `inline` on the definition and the declaration 8, PRIVATE 8,
// a member function of a probe aggregate 8 (`this` moves to edx too), one to eight probe locals 8.
// @retail 0xd74b0 standard
bool function_d74b0(byte const *owner)
{
	bool result = false;
	long count = *(long const *)(owner + 0x74);

	for (long i = 0; i < count; i++)
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
long function_d7ae0(long object_index)
{
	long result = NONE;

	while (object_index != NONE)
	{
		s_damage_object_datum *datum = (s_damage_object_datum *)datum_get_inlined(g_4e0300, object_index);

		if (datum && ((1 << datum->type) & 3) && datum->object)
		{
			s_damage_object *unit = (s_damage_object *)function_badc0(object_index, 3);

			long player_index = NONE;

			if (unit)
				player_index = unit->player_index;
			result = player_index;
			break;
		}
		object_index = DAMAGE_OBJECT(object_index)->parent_object_index;
	}
	return result;
}

/* the creature instant-kill roll: clears *instant_kill unless the damage may
   kill the object outright, and returns the outcome */
// @retail 0xd73c0
bool function_d73c0(long object_index, s_type_1e6529 const *data, bool *instant_kill)
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
				*instant_kill = false;
			else if (*instant_kill)
				result = true;
		}
	}
	return result;
}

/* the root of an object's parents, as 0xbaf80 (which is noinline): retail
   writes this walk out inline in 0xd6f90's ray loop */
PRIVATE inline long damage_object_root(long object_index)
{
	long root = NONE;

	while (object_index != NONE)
	{
		root = object_index;
		object_index = DAMAGE_OBJECT(object_index)->parent_object_index;
	}
	return root;
}

// MiniMax A (11-13): no new form, no lower score (206): the damage-record copy is 136 bytes in both builds, but retail starts it 16 bytes later (its locals take four more dwords and the copy is eight-aligned), so the frame is still 0x1a4 against retail's 0x1b4; the count limit and the array size agree, and the first 0xbadc0 use is still expanded here where retail calls it. BLOCKED-BY 0xbadc0.
// MiniMax A (11-12): no new form, no lower score (206): the frame is still 0x1a4 against retail 0x1b4 and the entry block still pushes before it reads the flags word and negates it, where retail reads the flags first; the first 0xbadc0 use is expanded in this build while retail calls it, and the out-of-line form that makes the body byte exact lives in src/unknown_0bad50.cpp, which this lane may not edit. BLOCKED-BY 0xbadc0.
/* damages everything in the damage's radius; returns the first player unit
   hit, else the last object hit */
// MiniMax B (11-12): no new form, no lower score (206): the frame is 0x1a4 against retail's 0x1b4 and the argument
// slots sit lower here; retail reads the flag word at [esi+0xc], masks bit 1 and negates it before the pushes where this
// build pushes first and loads the flag afterwards, and the link-time inliner still folds the callee in.
// score 206 (was 206): left is 0xbadc0 and register allocation. The link-time inliner inlines 0xbadc0 at the first
// MiniMax A (11-11): no new form, no lower score (206); the frame is still 0x1a4 against retail's 0x1b4 and the
// entry block still pushes before it reads the flags word and negates it, where retail reads the flags first. The
// first 0xbadc0 use is expanded in this build while retail calls it, and the out-of-line form that makes the body
// byte exact lives in src/unknown_0bad50.cpp, which this lane may not edit. BLOCKED-BY 0xbadc0.
// MiniMax D (11-10): no new form, no lower score (206); the first 0xbadc0 use is still expanded in this build, so
// the frame stays 0x1a4 against retail's 0x1b4; the out-of-line form of that site that makes the body byte exact
// is in a file this lane may not edit.
// MiniMax S (11-10): no lower score (206): the record-pool iterator with the iterator's address escaping gives 225, the index-based scan with it 228, and the squared-distance helper called at both tests with the mask in a named local 225; the first 0xbadc0 use is still expanded here, and the frame stays 0x1a4 against retail's 0x1b4.
// MiniMax D (11-09): no new form, no lower score; retail's player scan keeps the ordinal in [esp + 0x34] and tests
// the pool's used-bit word at [g_4e8c24 + 0x48 + (index >> 5) * 4] itself, and its frame is 0x1b4 against our
// 0x1a4, so its definition and objects array sit where ours do not.
// MiniMax G (11-06): matches only when the three object lookups call 0x175d70's file-local noinline copy of
// 0xbadc0 (object_get_175d70) from here, which keeps them out of line as retail has them; held under the
// noinline ruling (an existing noinline copy called from another file is the same steer), patch kept. The rest
// of that body (the s_player_iterator scan, a squared-distance helper called at both tests, the mask in a local
// before 0xbb050, the last object index declared first) with the lookups through 0xbadc0 is 246.
// MiniMax G (11-04): no new form, no lower score; the first 0xbadc0 call only leaves the line behind a second
// count test (248), and the squared distance at both tests (284) or with the mask in a local (275) is worse.
// MiniMax E (11-06): no new form, no lower score; the first 0xbadc0 use is still expanded here while
// retail calls it, and that body lives in src/unknown_0bad50.cpp, which this lane may not edit.
// MiniMax C (11-09): no new form, no lower score (206); the frame is still 0x1a4 against retail's 0x1b4 and the
// first 0xbadc0 use is still expanded here while retail calls it (BLOCKED-BY 0xbadc0).
// MiniMax E (11-10): no new form, no lower score (206).
// use (the flags & 2 unit test) but calls it at the other two; retail calls all three. Retail keeps data in edi,
// the definition in esi and &data->position in ebx (the distances are [ebx], [ebx+4], [ebx+8]) and spills the
// player index to the frame; it also recomputes the squared distance for the second test (ours shares it).
// Done: the players' parent walk is the damage_object_root loop (783 bytes, retail 782; 229 -> 209), and
// the cached `position` pointer (206). Tried: the player_index/NONE split of the first badc0 use (354, all three
// inlined); separate `!unit` / `player_index == NONE` tests (209); a squared-distance helper called twice (249,
// still shared); s_record_pool_iterator + data_iterator_next_inlined for the players (208).
// Round 273: Outer copy/player ordinal/return-candidate lifetimes, seven subsets,
// 206/230/230/225/225/233/233; all plus descriptive result names 233. Original body restored;
// frame 0x1a4 versus 0x1b4, ordinal/result spills and first lookup expansion remain.
// Round 529: existing point-difference helper with native vector workspace, crossed with
// complete player-scan lifetime, paired squared extents and complete-record library copy:
// 206/206/206/206/206/206/206/206. Frame and first expanded verifier remain; original body restored.
// Round 532: object ordinal consumed by the array read before validation, signed char/short/int/long,
// with original conditional return or explicit final result: 320/320/310/310/320/320/310/310.
// Extents 872-880 against 782; frame and verifier expansion remain. Original object induction and body restored.
// F335: last-default, full-branch, existing-result fallback and early-return completion, each
// with positive-unit admission or absent-unit skip: 206 each (783 bytes). Aligned frame and first verifier remain.
// B374: twice-consumed inner radius maximum via macro/branches/lower/upper fallback,
// outer expression or test-produced result: 230/230/235/234 twice; 767/782 bytes, frame still 0x1a4.
// No match; original repeated selector and complete body restored.
// P274: Object-array cursor/native short count/shared sequential verifier receiver,
// subsets 1/2/4/3/5/6/7: 211/240/206/231/211/240/231; all with entry cursor declaration 231.
// Original body restored; frame 0x1a4 versus 0x1b4 and first verifier expansion remain.
// N330: Positive player/capacity/annulus/all admissions, each with direct unit fields
// or the native shared player-unit handle: 206/206/206/206/206/206/206/206.
// Frame and first verifier expansion remain; original player scan and complete body restored.
// DH305: Player-policy admission result before damage, bool/byte/long/dword, rejection/positive
// admission: 206/206/206/206/206/206/206/206. NOT kept: a predicate set in the test and consumed later requires
// byte equality. Copy extent 0x88 agrees; original body restored, with frame gap and first verifier expansion.
// Round 544: Two displacement observations, direct/scalar/shared-vector/
// independent-vector, negative then positive admission: 284/249/284/284/284/249/284/284.
// No match; repeated-computation forms NOT kept. Original whole body restored.
// P282: Existing player-index getter at policy/preference sites and actual flags member, subsets
// 1/2/4/3/5/6/7: 314/206/206/314/314/206/314; all with library record copy 314. Policy-getter forms
// grow to 874/782, frame still 0x1a4; originals and all observations restored.
// P288: Absolute player-ordinal for traversals, returned-index/search-start ownership with
// four lexical homes: 396/396/396/396/331/331/331/331. NOT kept: frame/extent regress; original full
// body restored. Sixteen frame bytes and first verifier call boundary remain.
// M333: Actual query-mask conditional/zero-default/full-branch/three-default producers:
// 206/215/207/211; each with distinct policy/damage/preference verifier names repeats
// those scores. Frame 0x1a4 and first verifier expansion remain; original full body restored.
// Round 69 (lane 1): BLOCKED-BY 0xbadc0's inline decision at the first site. With 0xbadc0 out of line in a scratch copy
// (harness only, never in src/) this function is byte exact (0, 782/782): s_player_iterator local (retail's 16 unused
// frame bytes) + data_iterator_next_inlined, a squared-distance inline (a - b, root center first) called at both tests
// with the distance on the left of `<=`, the mask in a named local before 0xbb050, last_object_index declared first.
// The same body with 0xbadc0 inlining 246 (862 bytes, plain build agrees with the harness); so not kept.
// Keeps the site out of line: one more real test above it; `if (object_count > 0)` around the loop 48 (782 bytes, a
// second test retail lacks, repeats the loop's own). Does not: 0xbaf80 inlining 246, helper-built or costlier 0xbadc0
// bodies, no /Ob1 246, memory argument 246, bit-test and == spellings 246-249, null test above it 259, getter at the
// first site 356 (942), one helper at sites 1 and 3 356, admission helper 376, while 309/356, guard + do-while 250.
// Partial with the site inlining: iterator 207 (787), + mask and declaration order 200, shared distance 200: below the bar.
// m3 lane 1 r94 (plain full checks, on the walk kept for 0xd9d60; 206 there too): the unit-only flag named once and used for
// the query mask and the loop test 216 (785 bytes); the object loop's counter declared with the function's locals 206, one
// `unit` variable for the first two lookups 206, and that variable reused for the third lookup 206 (same code each); `objects[i]`
// read at each use instead of the `object_index` local 225 (798 bytes). The first 0xbadc0 site is still inlined.
// MiniMax I (11-10): no lower score (206 kept); an `if (object_count > 0)` guard around the object loop gives 248 (782
// bytes, a second test retail lacks, the same extra test the out-of-line site needs) and a for-form player scan gives
// 396; the frame stays 0x1a4 against retail's 0x1b4 and the first verifier call is still expanded in ours, and the
// sixteen frame bytes retail leaves unused have no spelling that recovers them.
// MiniMax H (11-13), NOT kept (coordinator): 206 -> 80 came with the three object lookups calling
// object_get_175d70, 0x175d70's file-local noinline copy of 0xbadc0, from this file: the same steer held under the
// noinline ruling (MiniMax G 11-06). The rest of that round: the flag update through a local (load, or, store), the
// outer radius added in two statements, the parent walk through an index-returning helper, the s_data_datum_iterator
// player scan and a squared-distance helper called at both tests.
// MiniMax H (11-14): no new form, no lower score (206): the frame is still 0x1a4 against retail's 0x1b4 and the first 0xbadc0 use is still expanded here where retail calls it, and that callee's own file is not this lane's.
// @retail 0xd6c80
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	real radius = MAX(definition->radius04, MAX(definition->radius58, definition->radius68));
	point3f const *position = &data->position;
	long objects[k_maximum_area_of_effect_objects];
	long object_count = function_bb050(0, (definition->flags0c & 2) ? 3 : 0, &data->unknown1c, position, radius,
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

			s_damage_object *root = DAMAGE_OBJECT(damage_object_root(player->unit_index));

			if (object_count >= k_maximum_area_of_effect_objects)
				continue;

			real dx = root->field_x221477.x - position->x;
			real dy = root->field_x221477.y - position->y;
			real dz = root->field_x221477.z - position->z;
			real inner = root->field_xae9109 + radius;
			if (inner * inner >= dz * dz + dy * dy + dx * dx)
				continue;

			real outer = definition->player_radius + root->field_xae9109;
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
		s_type_1e6529 copy = *data;

		function_d74e0(&copy, object_index, false);
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

/* damages one object of the radius (and its children and siblings): the geometry to it, then the damage */
// @retail 0xd74e0
void __stdcall function_d74e0(s_type_1e6529 *data, long object_index, bool child)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	bool affects = function_d72e0(object_index, data);
	bool instant_kill = function_d73c0(object_index, data, &affects);

	if (affects)
	{
		long field_4_2 = function_d5b60(object_index);
		s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
		point3f surface_point;
		vector3f surface_normal;
		bool outside = false;
		real scale;

		point3f const *origin = &data->origin;
		vector3f *direction = &data->direction;

		function_baff0(object_index, origin, &surface_point, &surface_normal);
		vector3d_from_points3d(origin, &surface_point, direction);
		data->distance = function_30bf0(direction);

		if (object->node_index == NONE)
		{
			data->node_direction = data->direction;
		}
		else
		{
			byte *node = g_51e9b8->data + (object->node_index & 0xffff) * 0xa0;
			point3f node_center;
			if (node && *(long *)(node + 0x74) && !function_d74b0(node))
			{
				point3f *body_center = (point3f *)(*(byte **)(*(byte **)(*(byte **)(node + 0x70) + 0x40) + 0x3c) + 0x70);

				real_point3d_set(&node_center, body_center->x, body_center->y, body_center->z);
			}
			else
			{
				node_center = object->field_x221477;
			}

			vector3d_from_points3d(origin, &node_center, &data->node_direction);
			function_30bf0(&data->node_direction);

			real dx = origin->x - surface_point.x;
			real dy = origin->y - surface_point.y;
			real dz = origin->z - surface_point.z;
			if (0.05f * 0.05f > dz * dz + dy * dy + dx * dx)
			{
				surface_normal.i = 0.0f - surface_normal.i;
				surface_normal.j = 0.0f - surface_normal.j;
				surface_normal.k = 0.0f - surface_normal.k;
				vector3f_add(&data->node_direction, &surface_normal, &data->node_direction);
			}
			else
			{
				vector3f_add(&data->node_direction, direction, &data->node_direction);
				function_30bf0(&data->node_direction);
			}
		}

		if (definition->cone_outer_angle != 0.0f)
		{
			vector3f *cone = &data->cone_direction;

			if (!(fabs(1.0f - (cone->i * cone->i + cone->j * cone->j + cone->k * cone->k)) > 0.0001f))
			{
				real dot = cone->k * direction->k + cone->j * direction->j + cone->i * direction->i;
				real angle = (real)acos(PIN(dot, -1.0f, 1.0f));

				if (!(definition->cone_inner_angle + 0.0001f > angle))
				{
					if (definition->cone_outer_angle > angle)
					{
						real cone_scale = 1.0f - (angle - definition->cone_inner_angle) /
							(definition->cone_outer_angle - definition->cone_inner_angle);

						data->unknown5c = PIN(cone_scale, 0.0f, 1.0f);
					}
					else
					{
						scale = 0.0f;
						outside = true;
					}
				}
			}
		}

		if (!outside)
		{
			real falloff = 1.0f;

			if (definition->radius04 - definition->minimum_radius > 0.0f)
			{
				falloff = 1.0f - (data->distance - definition->minimum_radius) / (definition->radius04 - definition->minimum_radius);
				if (0.0f > falloff)
				{
					outside = true;
					falloff = 0.0f;
				}
				else if (falloff > 1.0f)
				{
					falloff = 1.0f;
				}
			}
			scale = falloff;
		}

		data->distance_scale = scale;
		data->unknown58 = scale;
		if (outside)
		{
			data->unknown5c = 0.0f;
			*(dword *)data->unknown04 |= 0x2000;
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
			if (!child && function_d6f90(object_index, &surface_point, data))
				return;

			if (g_4e6948->mode == 4)
			{
				if (((1 << object->type) & 3) && data->definition_index != NONE)
				{
					s_damage_object *unit = DAMAGE_OBJECT(object_index);

					if (unit_get_player_index(object_index) != NONE)
					{
						long player_index = unit_get_player_index(object_index);
						long team = *(short *)(g_4e8c24->data + (player_index & 0xffff) * 0x21c + 0x28);

						if (team != NONE)
						{
							s_damage_owner owner = { NONE, NONE, NONE };
							vector3f unit_direction = { 0.0f };

							unit_direction.i = 0.0f - unit->unknown168.i;
							unit_direction.j = 0.0f - unit->unknown168.j;
							unit_direction.k = 0.0f - unit->unknown168.k;
							function_153d10(team, data->definition_index, &owner, &unit_direction, 0, 1.0f, 1.0f, 0);
						}
					}
				}
			}
			else
			{
				function_d7b80(data, object_index, NONE, NONE, NONE, NULL);
				if (instant_kill)
					*(dword *)data->unknown04 |= 0x40;
				if (field_4_2 && (*(byte *)field_4_2 & 8) && object->first_child_object_index != NONE)
					function_d74e0(data, object->first_child_object_index, true);
			}
		}
	}

	if (child && object->next_object_index != NONE)
		function_d74e0(data, object->next_object_index, true);
}

/* the material of one of an object's model regions */
// @retail 0xd8ae0
short *function_d8ae0(long object_index, long region_index, short *material)
{
	/* a 16-bit bit-field, as in 0x1886d0: retail loads the default with a
	   16-bit move, where a plain short local is loaded as a dword */
	struct s_default_material { signed short value : 16; };
	s_default_material result;
	result.value = g_47d8e0;

	if (object_index != NONE)
	{
		long model_index = *(long *)(g_4e3b44[DAMAGE_OBJECT(object_index)->tag_index & 0xffff].bytes + 0x38);

		if (model_index != NONE)
		{
			byte *regions = *(byte **)(g_4e3b44[model_index & 0xffff].bytes + 0x5c);

			*material = *(short *)(regions + region_index * 0x14 + 0x10);
			return material;
		}
	}
	*material = result.value;
	return material;
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
bool function_d8b50(real *recent_a, real *recent_b, char *timer, byte const *field_4_2, bool reset)
{
	bool result = false;

	if (*timer != NONE)
	{
		real delay_a = 0.0f;
		real decay_a = 2.0f;
		real delay_b = 2.0f;
		real decay_b = 2.0f;

		if (field_4_2)
		{
			delay_a = *(real const *)(field_4_2 + 0x30);
			decay_a = *(real const *)(field_4_2 + 0x34);
			delay_b = *(real const *)(field_4_2 + 0x38);
			decay_b = *(real const *)(field_4_2 + 0x3c);
		}

		*timer = (char)MIN(*timer + 1, 0x7f);

		if (*recent_a > 0.0f)
		{
			real seconds = g_510c54->field_2_3 * delay_a;
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
					*recent_a -= 1.0f / (g_510c54->field_2_3 * decay_a);
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
			real seconds = g_510c54->field_2_3 * delay_b;
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
					*recent_b -= 1.0f / (g_510c54->field_2_3 * decay_b);
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
	real total = 0.0f;

	for (long i = 0; i < *(long const *)(owner + 0xe0); i++)
	{
		byte const *entry = *(byte const *const *)(owner + 0xe4) + i * 0x14;

		if (!(mask & (1 << i)) && *(long const *)(entry + 8) == *(long const *)(target + 0x3c))
			total = *(real const *)(entry + 0xc) + total;
	}

	real fraction = (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f);
	real choice = fraction * total;
	long count = *(long const *)(owner + 0xe0);
	real sum = 0.0f;

	for (long i = 0; i < count; i++)
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
real function_dc230(long rider_index, long vehicle_index, s_type_1e6529 const *data)
{
	real result = 1.0f;
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *rider = DAMAGE_OBJECT(rider_index);
	s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);
	byte *field_4_2 = (byte *)function_d5b60(vehicle_index);
	long seat_index;

	if (field_4_2 && ((1 << rider->type) & 3) && ((1 << vehicle->type) & 3) && !(definition->flags14 & 0x2000) &&
		(seat_index = rider->unknown1fc) != NONE)
	{
		byte *seat = seat_index * 0xb0 + *(byte **)(g_4e3b44[vehicle->tag_index & 0xffff].bytes + 0x1cc);
		long count = *(long *)(field_4_2 + 0xd8);

		for (long i = 0; i < count; i++)
		{
			byte *entry = *(byte **)(field_4_2 + 0xdc) + i * 0x14;

			if (*(long *)entry == *(long *)(seat + 4))
			{
				result = *(real *)(entry + 4) * definition->unknown44;
				break;
			}
		}
	}
	return result;
}

/* damages an object with the globals' default damage (from +0x144), credited
   to an owner if one is given */
// MiniMax B (11-13): no new form, no lower score (10): a copy through a local (10) and a cast destination (10) leave the same code; the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx.
// MiniMax A (11-13): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the damage record's address is built where retail stores them before.
// MiniMax G (11-12): no new form, no lower score (10): nothing new this round; retail copies the owner through eax, ecx, edx
// (this uses ecx, edx, eax) and stores the finished flags before `push edi` and the `lea` (this stores them
// after the lea).
// MiniMax G (11-12): no new form, no lower score (10): retail copies the owner through eax, ecx, edx (this uses
// ecx, edx, eax) and stores the finished flags before `push edi` and the `lea` (this stores them after the lea).
// MiniMax H (11-12): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the damage record's address is built where retail stores them before it.
// MiniMax G (11-11): no new form, no lower score (10); the aggregate copy of *owner still goes through ecx, edx, eax where retail
// has eax, ecx, edx, and the finished flags are still stored after the `lea` where retail stores them before it; the copy
// written as separate field reads splits it into more instructions, and the flag store moved ahead of the copy changes the
// schedule elsewhere.
// MiniMax H (11-11): no new form, no lower score (10); the copy of the owner's three dwords still goes through ecx,
// edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the address of the damage
// record is built where retail stores them before it; the flags read and written through the union member (10), a
// pointer to the flags word (10), a copy through a temporary (10) and the flags read before the copy (17) all fail.
// MiniMax A (11-11): no new form, no lower score (10); the aggregate copy of *owner still goes through ecx, edx, eax
// where retail has eax, ecx, edx, and the finished flags are still stored after the `lea eax, [data]` where retail
// stores them before; the copy written as four field reads (the owner's three longs and its two shorts) splits into
// more stores and worse, and a memcpy of the owner compiles to the same three loads (10).
// MiniMax I (11-11): no new form, no lower score (10); the copy of the owner's three dwords still goes through
// ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the whole
// argument setup where retail stores them before the `push edi`; reading and writing the flags through a
// pointer local leaves the same code (10), and a permuter run over this body was killed by its outer time limit
// before it printed anything.
// MiniMax I (11-10): no new form, no lower score (10); a memcpy of the owner (10) and reading the flags through the union member instead of the byte array (10) compile to the same code, and copying the owner's four fields one by one splits the copy into three dwords and two words (52); the owner copy before the unknown54 store (17) and after the flags read (17) both move the first difference earlier, so the copy stays where it is.
// MiniMax C (11-09): no new form, no lower score (10).
// MiniMax E (11-09): no new form, no lower score (10); the copy of *owner still goes through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after `push edi` where retail stores them before; reading and writing the flags through the overlapping dword instead of the byte-array alias changes nothing (10), and so does putting the store in the first argument of the call (10).
// MiniMax D (11-06, run 49): no new form, no lower score (10); the copy of *owner still goes through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after `push edi` where retail stores them before.
// MiniMax A (11-04, run 41): no new form, no lower score (10); the aggregate copy of *owner keeps ecx, edx, eax
// where retail has eax, ecx, edx, and retail stores the flags before `push edi`.
// MiniMax B (11-06, run 42): no new form and no lower score (10); the aggregate copy of *owner still goes through
// ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after `push edi` where
// retail stores them before.
// score 10 (was 10): two schedule differences remain. Retail copies *owner through eax, ecx, edx (this uses ecx,
// edx, eax) and stores the finished flags before `push edi` / `lea eax,[data]` (this stores them after the lea,
// which then goes through ecx instead of eax). Everything else, including the usercall registers, matches.
// Tried: memcpy of the owner (10), the owner copied through a local (10), flags updated with ?: chains (10),
// the flags read and written through data.flags instead of the byte-array alias (10).
// Round 39 (lane 13, --fast): definition index through a named globals pointer 10, data declared first 10, object
// pointer local 10, single exit 10, `flags = data; flags |= 4;` 10, with blank-line splits 10, data.flags
// read-modify-writes 10; permuter 16 tries, best 10. 0xd6660 (score 3) uses retail's registers, so its summary is not
// the cause.
// Round 473: required bit at final publication 32; flag declaration before event 10; event name 10;
// damage_flags name 10; owner before scale 17; owner/late bit 35; both names/late bit 32; flags before
// disjoint publications 17. Original body restored; aggregate-copy registers and final flag-store schedule
// remain.
// Round 303: one initial clear of the three controlled bits, positive branches in abc/acb/bac/bca/cab/cba
// order 51,51,52,52,52,52; combined/separate conditional contributions 67,67.
// Original copy, flag graph and body restored; aggregate registers and final store schedule remain.
// MiniMax C (11-14): the flags read and stored through the dword alias of the four-byte field, and the owner
// copy read through a named pointer and through a named value, each give 10 (unchanged); no new form, no lower score.
// MiniMax H (11-14): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the record's address is built; a copy through a local and a cast destination both leave the same ten.
// @retail 0xdbfb0
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c)
{
	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.body_depleted))
		return;

	long definition_index = *(long *)(*(byte **)((byte *)g_4e034c + 0x144) + 0x14);
	if (definition_index == NONE)
		return;

	s_type_1e6529 data;

	data.unknown7c = NONE;
	function_d6660(&data, definition_index);
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
	function_d7b80(&data, object_index, NONE, NONE, NONE, NULL);
}

/* whether an object, or anything seated in or attached to it, is a unit (a
   player's unit, if players_only) */
// score 70 (was 70): frame 0x18 and entry homes agree; first +0x39 enters a shorter iterator loop.
// The verifier is called out of line; preheader/latch placement and result-tail scheduling remain, 233/246 bytes.
// Earlier 187: the compiler inlines 0xbadc0 (the datum lookup and the type test) at the unit check in the
// iterator loop; retail calls it (`mov eax,child_index; push 3; call 0xbadc0`). That is the only real difference:
// with the call kept out of line (checked with a throwaway noinline copy of 0xbadc0 in this file) the rest of the
// function compiles to retail's 246 bytes with `result` in the dead first argument slot, if the tails are written
//     if (first_child != NONE) result = result || db110(first_child, players_only, true);
//     if (next != NONE && walk_siblings) result = result || db110(next, players_only, true);
//     return result;
// (the first-child part is `result = result || ...` in either form). The if/else form below turns the sibling
// `return function_db110(...)` into a loop (246 bytes, 82 with the call out of line) and so does not match even then;
// it is kept only because with 0xbadc0 inlined it scores 187 where the `||` form scores 206 (212 with
// `return result || db110(next...)`, whose end is `xor eax,eax` / `mov eax,1` where retail returns al). Also tried
// with the `||` tails: local copy of child_index (212), `!= NULL` (212), `||` inside one condition (212), nested
// `if (players_only)` (212), `for` and `do`-style loops and `unit` declared outside the loop (206), `return true`
// from the loop (253). All inline 0xbadc0. A noinline declaration of 0xbadc0 in its own file (unknown_0bad50.cpp)
// makes this function MATCH with the `||` tails, but breaks 0x11a570, 0x11a680, 0x274f30, 0x275160, 0x275270 and
// 0x2736c0 (retail inlines it there), so it cannot be used. 0xd0620 (not ours) takes the iterator in ebx in retail.
// Round 478: result-first recursive admission and one return, bool/byte/short/long result
// 184/195/207/205; comparison-produced iterator result 190/201/175/170.
// Three-point ordinary gain rejected; comparison-result flags are exact-only and restored.
// The verifier still expands, frame remains 0x1c versus 0x18, and recursive/result homes differ.
// Round 529: independent short/player rejection guards and one admitted-child commit,
// result/iterator lifetime subsets 99/99/99/99; common recursive result tails 71/78/71/78.
// Nested admission/positive player arms and their for initializers 206 each (verifier expands).
// Guarded admission with common recursive results kept: 70 plain, 233/246 bytes, first +0x39.
// Round 532: result-owned iterator termination with bool/byte/short/long carriers, guarded success
// or comparison-produced success: fast 71/67/95/104/58/57/93/98 against unchanged fast 71.
// Byte predicate measures 246 bytes but none matches; result-tested flags restored. First-call receiver and latch remain.
// B374: complete type-owned false before iterator initialization, OR/conditional/negative/positive
// recursive joins 82/89/108/89; after initialization 102/108/125/110.
// No exact match; original entry default, verifier boundary, loop and recursive tails restored.
// Round 534: rejection-guard for initializer 70; direct successful search
// returns 84/84; canonical byte/short/long/word/dword results
// 66/95/104/95/104. Byte result gains four only; original body restored.
// Initializer receiver and independent latch remain; iterator still owns the search test.
// N324: Separate null/player rejection, scan-scoped iterator and result-admitted recursive calls, seven
// subsets then result-first initialization 71/71/71/118/118/118/118/120. Original complete body restored;
// initializer receiver and loop latch remain.
// DH305: Per-visit result refinement on the retained recursive-OR graph, nested admission,
// short rejection, short-predicate refinement and null/player refinement, bool/byte:
// 58/58/78/75/56/56/78/75. NOT kept: result-tested predicates require an exact match; initializer receiver and latch remain.
// Round 69 (lane 1): BLOCKED-BY 0xbadc0's inline decision. With 0xbadc0 out of line in a scratch copy, or with one
// diagnostic file-scope inline_depth(0) around this function (not kept), a plain rotated `while (next)` with positive
// arms (`if (child != NONE) { if (players_only) { unit test: result = true, break } else { result = true, break } }`)
// and the `||` tails is byte exact (0, 246/246). Rotated forms inline it, 206 (338 bytes): positive player arm with
// else, continue guard, continue inside a positive guard, getter in && / || / positive arm; `players_only == true`
// 204. One more real test above the call keeps it out (97, 258 bytes). `while (!result && next)` 70 (unrotated).
// @retail 0xdb110
bool __stdcall function_db110(long object_index, bool players_only, bool walk_siblings)
{
	s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	s_damage_object *object = datum->object;
	bool result = false;
	s_object_child_iterator iterator;

	if ((1 << datum->type) & 2)
	{
		function_d0620(&iterator, object_index);
		if (function_d0690(&iterator))
		{
			do
			{
				if (iterator.child_short == NONE)
					continue;
				if (players_only)
				{
					s_damage_object *unit = (s_damage_object *)function_badc0(iterator.child_index, 3);
					if (!unit || unit->player_index == NONE)
						continue;
				}
				result = true;
				break;
			} while (function_d0690(&iterator));
		}
	}

	if (object->first_child_object_index != NONE)
		result = result || function_db110(object->first_child_object_index, players_only, true);
	if (object->next_object_index != NONE && walk_siblings)
		result = result || function_db110(object->next_object_index, players_only, true);
	return result;
}

/* the damage multiplier of a resistance against a source on a material: the
   resistance's scale (+0x1c), times the armor table's entry for the source's
   damage groups and the struck material's armor types */
// @retail 0xd9020
real function_d9020(long object_index, byte const *resistance, byte const *source, s_type_1e6529 const *data)
{
	real result = *(real const *)(resistance + 0x1c);

	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.unknown7))
		result = 0.0f;
	if ((**(byte const *const *)(resistance + 4) & 0x20) && !(source[4] & 0x20))
		result = 0.0f;

	if (global_material_get(data->unknown7c))
	{
		byte *armor_a = global_material_get(data->unknown7c);
		byte *armor_b = global_material_get(data->unknown7c);

		result = function_d5bc0(*(string_handle const *)(source + 0x40), *(string_handle const *)(source + 0x44),
			*(string_handle *)(armor_b + 0x10), *(string_handle *)(armor_a + 0x14)) * result;
	}
	return result;
}

/* destroys a damage region: picks the permutations whose chance and body
   threshold allow it, and destroys each now or schedules it after its
   delay */
// @retail 0xdae60
void __stdcall function_dae60(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info_region *region = &info->regions[region_index];
	s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset) + region_index;

	for (long i = 0; i < region->permutation_count; i++)
	{
		if (state->destroyed_permutations & (1 << i))
			continue;

		s_damage_info_permutation *permutation = &region->permutations[i];

		if (permutation->unknown08 != 0.0f)
			continue;

		if (permutation->skip_chance != 0.0f &&
			permutation->skip_chance > (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f))
			continue;
		if (1.0f > permutation->body_threshold && (permutation->flags & 0x200000))
			continue;
		if (permutation->body_threshold > 1.0f && !(permutation->flags & 0x200000))
			continue;

		if ((accumulator->flags & 0x400) && (permutation->flags & 0x1000000))
			continue;
		if (!(accumulator->flags & 0x400) && (permutation->flags & 0x800000))
			continue;

		if (permutation->delay > 0.0f)
		{
			if ((state->pending & 0xfff8) && (state->pending & 7) != i)
				function_da110(state->pending & 7, info, object_index, owner, region_index, accumulator);

			state->pending = (state->pending & 0xfff8) | (i & 7);

			real seconds = g_510c54->field_2_3 * permutation->delay;
			long ticks;

			__asm
			{
				fld seconds
				fistp ticks
			}
			state->pending = (word)((state->pending & 7) | (ticks << 3));

			if (permutation->effect_index != NONE)
			{
				function_d9d60(object_index, permutation->unknown38, permutation->effect_index, owner,
					(permutation->flags >> 20) & 1);
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

/* whether a permutation's model state allows it: false when its state index
   (+0x22) is in range and that region's chosen state (ba690's fifth array,
   8 bytes per region, state at +1) is at or above the permutation's (+0x20) */
// @retail 0xd9f70
bool function_d9f70(long region_index, long permutation_index, s_damage_info *info, long object_index)
{
	s_damage_info_permutation *permutation = &info->regions[region_index].permutations[permutation_index];
	byte *states;
	long state_count;
	long saved_states;
	long choices;
	bool result = true;

	function_ba690(object_index, &state_count, &states, &saved_states, &choices);
	if (permutation->unknown22 >= 0 && permutation->unknown22 < state_count)
	{
		if ((short)(char)((byte *)choices)[permutation->unknown22 * 8 + 1] >= permutation->unknown20)
			result = false;
	}
	return result;
}

struct s_globals_element;
s_globals_element *function_188690(short index);
long function_1e4a10(long index);

/* damages an object's shield: scales the damage by the shield material's
   armor, takes it off the shield, passes what is left over to the body
   (accumulator->damage) and stuns the shield's recharge */
// Round 172 lead 4 (match-only, not kept): only the reload order after function_d6a70 and function_176780 differs: retail reloads the spilled
// shield damage before the zero, ours the zero first (the function_d5bc0 order is now retail's).
// score 8 (was 8): only the reload order after three calls differs: after function_d5bc0 retail reloads 1.0f, the
// zero, the maximum and the inverse (ours the zero, maximum, inverse, then 1.0f); after function_d6a70 and
// function_176780 retail reloads the spilled shield damage before the zero (ours the zero first).
// MiniMax B (11-06, run 42): no new form and no lower score (8); after the call to function_d5bc0 retail reloads the
// 1.0f constant before the `xorps` of zero and the spilled locals, ours reloads it last; moving the declaration of
// the multiplier only makes the constant take a stack home and scores far worse.
// Earlier (8, was 8): only the schedule differs: after the call to function_d5bc0 retail reloads the 1.0f
// MiniMax D (11-06, run 49): no new form, no lower score (8); after the call to function_d5bc0 the 1.0f constant is still reloaded last, retail reloads it before the `xorps` of zero and the spilled locals.
// MiniMax A (11-04, run 41): no new form, no lower score (10); assigning the multiplier in an else arm instead
// of at the declaration scores 156, and the reload order after the d5bc0 and d6a70 calls stays as it was.
// Earlier (8, was 10): only the schedule differs: after the call to function_d5bc0 retail reloads the 1.0f
// constant before the `xorps` of zero and the spilled locals (ours reloads 1.0f last). The `lea ebx, [edi + 8]`
// schedule for function_176780 was its parameter order (round 74, m3 lane 1). Tried: owner pointer in
// a local for the function_176780 call (no change), the tail's unknownd4 handle in a long local (12).
// Also tried: `multiplier = 1.0f` declared at the function top (228) or first in the else-if block (206, both
// give it a stack home), declared after the in-radius subtraction (38), `multiplier = cond ? d5bc0(...) : 1.0f`
// (156), the inverse maximum as an if/else statement (10, the same code).
// Round 489: separate scaled damage / conditional multiplier result / named reciprocal,
// seven subsets 30/156/160/10/30/156/160; all with named depletion expression 160.
// Original damage phases, reciprocal expression and whole body restored.
// M404: complete stun admission in positive bool/byte/word/long and
// complementary bool/byte/word/long forms: 8/8/8/8/8/8/8/8. Original damage phases,
// constants and body restored; the post-call reload schedule remains.
// MiniMax A (11-04, run 41): no new form, no lower score (10); assigning the multiplier in an else arm instead
// of at the declaration scores 156, and the reload order after the d5bc0 and d6a70 calls stays as it was.
// Round 139 (m3 lane 8, fast): the reload order after each call is the same at function_d6a70 and function_176780:
// retail reloads the spilled shield damage before the zero, ours the zero first. Diagnostic, reverted: one 1.0f clamp
// of the body damage ahead of the first 0.0f (real code, 254) gives retail's shield-then-zero order at both later calls
// and puts 1.0f first after function_d5bc0 (with the zero last, retail has it second); the same clamp with 5.0f leaves
// every order as ours, so the order follows where 1.0f is first used. Real: the shield clamp as an if 10; multiplier
// declared before the material lookup 14, before the inverse 48, before the maximum 206; `0.0f < maximum_shield` 8;
// the first zero, or every zero ahead of function_d5d80, written as integer 0 8/8 (same code). The 1.0f clamp moved
// after the early zeros (just ahead of function_d5d80, 209, or of the inverse, 198) flips no order.
// Round 58 (m3 lane 2, fast): the d5bc0 reload order follows the function's total symbol count, wherever the extra
// symbols sit (diagnostic dead locals, reverted): +6/+7 give retail's d5bc0 order (4), +8/+9 7, +10 to +64 8; no count
// flips the shield/zero order and renames change nothing. Real: the end lookup into a fresh `damaged` local as 0xd8cb0
// writes it 8 (same code); an `info` local for the five field_4_2 reads 220 (883 bytes; it flips shield/zero to
// retail's); the category test passed directly 189 (874 bytes); the stun ticks through slot_handler.h's real_to_long
// 7 (also with `seconds` named, 7); the actor through actor_get 8 (same code); 18 combinations of those with the
// inverse as an if/else 7 or 8 each.
// Round 157 (m3 lane 5): the permuter (15 tries in 600 s, best 8); the stun ticks through slot_handler.h's real_to_long
// 7 (after function_d5bc0 the order becomes zero, inverse, maximum, 1.0f); every asm rounding block of the file's
// non-contributor functions through real_to_long 7, or through one shared seconds-to-ticks inline helper 7 (d5de0
// 400 -> 393, the rest unchanged); not kept (one point). `shield_damage *= multiplier` 8 (same code); the side effect 3
// and in-radius test as one named bool used at both places 231 (895 bytes).
// Round 67 (m3 lane 15, fast): 0xd6a70 matched through word_flags_set (0x54590 lost, not kept) 8 (same reload orders);
// both object lookups through slot_handler.h's object_get 8 (same code); the stun block's actor index in a local used at
// the test and the lookup 23 (883 bytes, read ahead of the type test), declared inside the type test 8; the multiplier
// declared uninitialised at the block top and assigned 1.0f at its old place 8 (same code); the shield default as
// `shield_damage = body_damage; if (shield_damage <= 0.0f) shield_damage = 0.0f;` 10 (the compare flips).
// Round 172 (m3 lane 6, plain-confirmed 4): kept six named values, each compiling to the same code alone: the clamped
// vitality (store and test), the absorbed damage (accumulator store and subtraction), the actor index in the type test
// (test and lookup), the end lookup in a fresh `damaged` local, and two parts named where they are computed: the
// depletion below zero and the multiplier's reciprocal. Together they give retail's function_d5bc0 reload order (8 -> 4,
// round 58's symbol-count lead): five of them 8, four 8, seven 4; the shield-damaged block's field_4_2 read into an info
// pointer under a nested test in place of the depletion 4, with a second info pointer for the stun block in place of the
// reciprocal too 4 (field copies mixed with direct reads, not kept); one info pointer for all five reads, re-measured on the
// kept set, 223 (876 bytes); slot_handler.h's real_to_long for the ticks with five 7, with seven 4. Tried (fast): the object lookup
// after the two damage locals 139 (876 bytes); shield damage declared first, read from the accumulator 220 (889); the
// material's armor handles named 168, all four handles named 239 (907). On the info-pointer set (the same code as the
// kept one): the stun threshold compare as
// `threshold < shield` 4, the zero arguments of function_176780 as integer 0 4, `real shield_damage;` declared above
// body damage 4 (all the same code); `0.0f == vitality && 0.0001f < body` 72 (878); one info pointer for the damaged
// and stun blocks 206 (871); the in-radius subtraction ahead of the material lookup 145 (897); the overflow test
// with the multiplier first 32; the side-effect test with the radius flag first 10. Diagnostic: a dead `1.0f` local
// at the top moves nothing.
// NOT kept (coordinator, lane 6 r172): 8 -> 4 needs all six named locals together (shield_vitality, absorbed,
// actor_index, damaged, deficit, inverse_multiplier; five give 8). deficit and inverse_multiplier are one-use, and the
// gain comes from the symbol count, so the set is match-only: re-apply it only with a form that reaches a byte match.
// M3 lane 16 r114 (fast): the shield default as `shield_damage = body_damage; if (!(shield_damage > 0.0f)) shield_damage = 0.0f;` 8 (same
// code); the actor stun-bounds lookup as an inline shared with 0xd8cb0 (the same lookup stands in both and in 0xd5de0): plain inline
// 270 (834 bytes: kept out of line; shield then reloads before zero), forced inline 8 (same code). Diagnostics re-run (probe locals
// `long p = object_index;`, k = 1..16): 8 8 8 8 8 4 4 7 7 8 8 8 8 8 8 8, shield/zero never flips; unreferenced locals count for nothing;
// an `info` local for the five reads 220 (declared first) / 216 (after the shield default). The order after function_d5bc0 permutes
// among the four values as the count changes (zero, maximum, inverse, 1.0f tie), while zero stays ahead of the shield damage at every
// count, so that pair is not a tie in our build.
// @retail 0xd9110
void function_d9110(long object_index, s_damage_effect_definition const *definition, s_type_1e6529 *data,
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
		/* a bool local: retail stores it to a stack byte and pushes the whole dword */
		bool is_category_1 = definition->category == 1;
		real maximum_shield = function_d5d80(object_index, is_category_1);
		real inverse_maximum_shield = maximum_shield > 0.0f ? 1.0f / maximum_shield : 0.0f;
		byte *material = (byte *)function_188690(accumulator->field_4_2->shield_material);
		real multiplier = 1.0f;

		if (!data->in_unknown_radius)
			body_damage -= shield_damage;
		if (shield_damage > 0.0f && material)
		{
			multiplier = function_d5bc0(definition->damage_group, definition->damage_group_b,
				*(string_handle *)(material + 0x10), *(string_handle *)(material + 0x14));
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
				function_d6a70(object_index);
				accumulator->flags |= 8;
			}
		}

		if (!TEST_FIELD_BIT(object->damage_flags.shield_damaged) &&
			accumulator->field_4_2->shield_damaged_threshold > object->shield_vitality)
		{
			function_176780(object_index, (s_effect_owner const *)&data->owner, 0.0f,
				accumulator->field_4_2->shield_damaged_effect, 0.0f, NULL, NULL);
			object->damage_flags.shield_damaged = true;
		}

		if (shield_damage > accumulator->field_4_2->shield_stun_damage_threshold ||
			object->shield_vitality == 0.0f && body_damage > 0.0001f ||
			definition->side_effect == 3 && data->in_unknown_radius)
		{
			real stun_time = accumulator->field_4_2->shield_stun_time;

			if (((1 << object->type) & 3) && object->unknown12c != NONE)
			{
				byte *bounds = (byte *)function_1e4a10(*(long *)(g_4f55f0->data + (object->unknown12c & 0xffff) * 0x888 + 0x54));

				if (bounds)
					stun_time = *(real *)(bounds + 0x48);
			}
			if (object->shield_stun_ticks != 0x7fff)
			{
				real seconds = g_510c54->field_2_3 * stun_time;
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

	object = DAMAGE_OBJECT(object_index);
	if (object->unknownd4 != NONE)
		function_b58c0(object->unknownd4, 0x80);
	accumulator->damage = body_damage;
}

/* damages a region: raises its damage level by the damage over the span to
   its next permutation's threshold, and when the level fills, destroys the
   permutations at that threshold */
// MiniMax G (11-12): no new form, no lower score (73): nothing new this round; the pending field is a real bitfield (3 bits of
// permutation and 13 of ticks), and the remaining difference is in the material block.
// MiniMax G (11-12): no new form, no lower score (73): the pending field is a real bitfield (3 bits of permutation
// and 13 of ticks), which gives the pending mask and the tick rows; the remaining difference is in the material
// block.
// MiniMax B (11-12): no new form, no lower score (73): the instructions agree with retail but the register pair is
// swapped - retail loads the accumulator pointer into ECX and the flags into EAX where this build loads the pointer into
// EAX and the flags into ECX, the same shape as 0xdae60; the body is 961 bytes against retail's 958.
// MiniMax E (11-11, run 80): no new form, no lower score (73): the instructions agree with retail's and the
// differing lines are the two early exits, whose targets land five bytes past retail's because the tail is five
// bytes longer; the setup of the 0xda110 and 0xd9d60 calls decides that tail, and both callees are unmatched
// bodies in this file whose own walk keeps their conventions, so the parameter order is not mine to change.
// MiniMax H (11-11): no new form, no lower score (73); the delay block's pending word sits in ecx where retail keeps it in eax, because retail's 0xda110 call takes the permutation in eax and ours still hands two arguments over in registers, so the two early exits keep landing five bytes past retail's.
// MiniMax S (11-11): no new form, no lower score (73); retail passes 0xda110's permutation in eax at all six of
// its call sites in this file (0xd5de0, four here and 0xdb760), so the transport is one decision for the whole file.
// MiniMax S (11-10): no new form, no lower score (100): our frame is four bytes shorter than retail's, so the two arguments' stack homes sit four bytes lower, and the 0xda110 call setup pushes the arguments in another order with retail's xorps after that call where ours zeroes xmm2 before it.
// MiniMax A (11-10): no new form, no lower score (100): the instructions agree with retail's and the
// differing lines are only the branch targets, because the two early exits land five bytes past
// retail's; the call setups of 0xda110 and 0xd9d60, both in this file, decide that tail.
// MiniMax S (11-10): no new form, no lower score (100): the two early exits still branch to targets five bytes after retail's, because the tail is five bytes longer (the 0xda110 index in eax and the 0xd9d60 flag in al are theirs to give), and the delay block's register rotation goes with them.
// MiniMax D (11-09): no new form, no lower score; the first difference (+0x39) is only the two early exits, whose
// branch targets land five bytes after retail's because the tail is five bytes longer (retail passes 0xda110's index
// in eax and 0xd9d60's flag in al, ours on the stack), so the head follows the tail.
// MiniMax C (11-09): no new form, no lower score (100); the two call setups of 0xda110 and 0xd9d60 still follow
// the conventions their own unmatched bodies decide, and the delay block's register rotation stays with them.
// MiniMax E (11-10): no new form, no lower score (100); the callees' parameter order is not mine to change.
// score 45 (was 67; 110 before that): 67 before m3 lane 7 r155's 0xda110 convention (its region index read through its address), 73 before m3 lane 2 r76's 0xd9d60 parameter order, 100 before m3 lane 1 r94's walk, kept for 0xd9d60's match; the pending field is a real bitfield, 3 bits of permutation and 13 of ticks, which gives
// retail's byte xor and `or` forms in the delay block. Left: the setup of the 0xda110 and 0xd9d60 calls (their
// conventions, BLOCKED-BY 0xd9d60's walk, see 0xdae60; one instruction more at each call, so every later line is off)
// and an eax/ecx/edx rotation in the delay block and the destroyed-flag store. Round 44 (lane 15, fast): `i !=
// state->pending_permutation` 101; the effect index named (two uses) 100; the pending permutation named before the
// ticks test 116 (966 bytes), named inside a nested ticks test 100; the delay rounding through a forceinline helper
// 100. Diagnostic, reverted: 0xda110's 0xd9d60 call removed, so 0xd9d60 takes its register convention (8): this
// function 80 (959 bytes), the rotation stays.
// MiniMax C (11-01): the first permutation loop indexes the permutation array instead of walking it with a pointer, so
// the permutation count is read before the array as retail does.
// MiniMax C (11-01): the pending xor narrowed to a byte reproduces retail's byte form and measures 111, seven points and
// under the keep bar for a function still above 30, so it was reverted.
// Earlier 118 (was 134): the first permutation loop indexes the permutation array instead of walking it with a pointer,
// which reads the permutation count before the array as retail does and leaves the region pointer in edx. Left: retail
// tests the accumulator's exclusive flag with `test dh, 4` and ours with `test ch, 4`, the random global is in edx in
// retail and ecx here, the pending index is used through ax in retail and cx here, and the calls to 0xda110 and 0xd9d60
// take their arguments in another shape (the conventions of those two unmatched callees, see 0xdae60).
// Earlier 134 (was 229): the exclusive test is written as two mask conditions and the blocked flag is stored before the
// threshold check, which puts `destroy` in bl and the permutation index in memory as retail does. Left: retail keeps
// the region pointer in edx and ours in edi, retail applies `and eax, 0xffff` before the prologue's pushes and ours
// after, retail tests the accumulator's exclusive flag with `test dh, 4` and ours with `test ch, 4`, and the pending
// index and the random global land in different registers. The conventions of 0xd9d60 and 0xda110 are unmatched
// callees' (see 0xdae60).
// Earlier 229 (was 265): register and stack-slot assignment only. Retail keeps `destroy` in bl and the permutation index
// in memory, ours the reverse; the damage-level delay is read through a region pointer reloaded from the stack right at
// the use in retail, ours reloads it into edi early on each path; retail tests the exclusive flag with `test eax, mask`
// on each side of the `?:`, ours materializes `and eax, mask`. The state-count check is unsigned (`shr ecx, 3`, signed
// compare) and the last-permutation flag is computed from the threshold after the search loop, which makes the compiler
// store it on each path as retail does. The conventions of 0xd9d60 and 0xda110 are unmatched callees' (see 0xdae60).
// Tried: the flag preset true and cleared inside the loop (284), `if (i < count) {...} else flag = true` after the loop
// (512), the `?:` operands as `!= 0`, `!!`, `(bool)` or `== mask` (286, 286, 284, 262 with +3 bytes), `region` declared
// before `object` (275 with the preset flag), a separate loop variable for the second loop (no change).
// Round 273: Destruction produced by its complete original admission test in
// bool/byte/short/long storage 234/234/240/241; with entry-initialized shared
// ordinal 243/243/251/252. Wider decisions reach 958 bytes but leave frame
// 0x1c versus 0x20 and wrong ordinal/destruction ownership. Original body restored.
// Round 157 (m3 lane 5): 103 in every walk that frees 0xd9d60; aligned 45: the 0xda110/0xd9d60 call setups and a
// one-step eax/ecx/edx rotation in the delay block and the destroyed-mask store that follows the extra push at each
// 0xda110 call; the two asm roundings through real_to_long 100.
// m3 lane 16 r107 (plain, damage.cpp callee mention orders, see 0xd9d60's note; all restored): function_dbfb0 and function_d9640
// first 103 (0x1c39c0 lost); function_10d4e0 first 80 (0xd8cb0 299); function_d9e70 first 80 (aligned 38 -> 31); with 0xd9d60's
// marker/object swap 96 (aligned 29); function_dbfb0 ahead of 0xdae60 and function_d9640 after function_dbc80 103, with the swap
// 73 (aligned 48: 0xda110's call takes two registers where retail passes one). BLOCKED-BY 0xda110 in that walk.
// m3 lane 2 r76 (plain 67 after 0xd9d60's flag moved last; fast, all on 0xda110 and restored): its 0xd9d60 call now agrees.
// BLOCKED-BY 0xda110: every remaining line follows its convention (ours region EAX + permutation ECX, retail permutation EAX
// alone). 0xda110 without its region local 67; object declared after region and permutation 67; no `__stdcall` 67; a named
// region-state array for the update 67; 0xda110's permutation_index declared last 45 (permutation in EAX, region in ECX: the
// accumulator test, random global and pending block then agree; not kept, no retail evidence for the declared position).
// m3 lane 7 r155 (fast; 0xda110 now takes retail's convention, so its two call setups push the same arguments): left are
// register choices only (masked 7): at each 0xda110 call retail loads the owner first (EDX) where ours loads the object first,
// and the delay block's temporaries rotate (retail g_510c54 in ECX and the pending byte in EAX, ours EAX and EDX). Tried: the
// pending field in 0xdae60's mask forms, all four uses 45, test and argument only 45 (same code); the pending permutation
// stored after the delay product 33 (not kept: retail stores it first, the gain is the branch targets realigning); the pending
// permutation named before the test 109; `seconds` and `ticks` declared at the delay block's top 45.
// @retail 0xdaaa0
void function_daaa0(s_damage_info *info, long object_index, s_damage_owner const *owner, long region_index,
	real damage, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info_region *region = &info->regions[region_index];

	if (region_index >= 0 && region_index < (long)((dword)(long)object->region_states_size / sizeof(s_object_region_state)))
	{
		s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset) + region_index;
		real previous_threshold = 1.0f;
		real next_threshold = 0.0f;
		long i;
		s_damage_info_permutation *permutation;

		for (i = 0; i < region->permutation_count; i++)
		{
			permutation = &region->permutations[i];

			if (!(state->destroyed_permutations & (1 << i)))
			{
				next_threshold = permutation->unknown08;
				break;
			}
			previous_threshold = permutation->unknown08;
		}

		bool last_permutation = next_threshold == 0.0f;

		real fraction = damage / MAX(previous_threshold - next_threshold, 1.0f / 255.0f);

		if (fraction > 0.0f)
		{
			long level = (long)(fraction * 255.0f + (real)state->unknown02);

			state->unknown02 = (byte)PIN(level, 0, 255);

			real seconds = g_510c54->field_2_3 * region->damage_level_delay;
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

				if (last_permutation && (region->flags & 4) && function_db110(object_index, false, false))
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
				if (accumulator->flags & 0x400)
				{
					if (permutation->flags & 0x1000000)
					{
						destroy = false;
						blocked = true;
					}
				}
				else if (permutation->flags & 0x800000)
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
						if (state->pending_ticks && state->pending_permutation != i)
							function_da110(state->pending_permutation, info, object_index, owner, region_index, accumulator);

						state->pending_permutation = i;

						real seconds = g_510c54->field_2_3 * permutation->delay;
						long ticks;

						__asm
						{
							fld seconds
							fistp ticks
						}
						state->pending_ticks = ticks;

						if (permutation->effect_index != NONE)
						{
							function_d9d60(object_index, permutation->unknown38, permutation->effect_index, owner,
								(permutation->flags >> 20) & 1);
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
			function_daaa0(info, object_index, owner, region_index, region_damage / region->vitality, accumulator);
		if (region->flags & 1)
			damage = 0.0f;
	}
	return damage;
}

/* damages an object's body: scales the damage by the body's material,
   applies region damage, takes it off the body and depletes the body when
   it runs out */
// MiniMax G (11-12): no new form, no lower score (3): nothing new this round; retail still hoists `mov eax, [edi]` above the
// info `je` and addresses the region flags as [eax + edx + 4] where ours uses [edx + eax + 4].
// MiniMax G (11-12): no new form, no lower score (3): the passed bool and the short team are locals as in 0xd5d20;
// what is left is retail's hoisted `mov eax, [edi]` above the `je` and the base/index order of the amode.
// MiniMax H (11-12): no new form, no lower score (3): the region entry is still loaded below the info test where retail loads it above, and the region flags are still addressed as [edx + eax + 4] where retail has [eax + edx + 4]; the entry cached in a local before the guard gives 228 and the scaled-index-first address gives 3.
// MiniMax A (11-11): no lower score (3); two forms not in the rounds below: the entry test nested inside the info test
// (3, the load stays behind the branch) and a named region offset read before the regions pointer (3, the flags address
// stays [edx + eax + 4]); an entry read into an integer local instead of a pointer (228) and a typed entry local (228)
// give the same allocation as the plain pointer local. The original body is kept.
// MiniMax G (11-11): no new form, no lower score (3); retail loads the region entry before the branch that tests the info
// pointer and addresses the region flags as [eax + edx + 4] where this build uses [edx + eax + 4], the same address with the
// base and index registers the other way round; naming the region entry in a local moves the load into the branch and scores
// 228, and the other address spellings keep the current form.
// MiniMax H (11-11): no lower score (3); the region entry is still loaded after the `info` test where retail loads it
// before, and the region flags are still addressed as [edx + eax + 4]; a nested entry test (3), an entry local read
// before the test (3), a top-level entry local (3) and an entry local shared with the 0xdb090 call (218) all leave
// the load behind the branch.
// MiniMax A (11-11): no lower score (3); retail's region entry is loaded before the branch that tests the info
// pointer (the `je` at 0xd8d46 belongs to the `test ebx, ebx`) and its flags are addressed as [eax + edx + 4] where
// ours uses [edx + eax + 4] - the same address with the base and index registers the other way round. The address
// shapes the earlier rounds tried (the struct field, a byte pointer addition, an integer address) all keep our order,
// and an entry local read before the branch moves the load up but rebuilds the prologue (228). The original body is kept.
// MiniMax A (11-11): no lower score (3); a test-only local for the region entry and an integer address for the entry give
// retail's code exactly (3), while an entry local that the region index reads puts the load above the info branch like
// retail but swaps the info and object registers and rebuilds the prologue (228). The region flags address stays
// [edx + eax + 4] for every source form tried: the struct field, a byte pointer addition, an integer address with the
// regions pointer cast to dword and a dword base all give the same code (3).
// MiniMax I (11-10): no new form, no lower score (3); a local for the region block loaded before the info test is spilled and changes the whole prologue (228), so the load of the block still stays behind the branch.
// MiniMax C (11-09): no new form, no lower score (3); an entry local before the info test moves the load above the
// branch but costs the register allocation (228), the two null tests nested (228) and the && operands swapped (3, the
// tests then in the wrong order).
// MiniMax E (11-09): no new form, no lower score (3); retail loads the region entry above the `info` test and addresses the region flags as [eax + edx + 4], ours loads the entry after the test and uses [edx + eax + 4]; an entry local before the test moves the definition into a register argument and spills the object (227), and the scaled-index-first form of the same address grows the function to 880 bytes (34).
// MiniMax D (11-06, run 49): no new form, no lower score (3); the region pointer is still loaded after the `info` test where retail loads it before, the object pointer and the 1 << type shift still use different registers, and the `lea ebx, [ecx + 8]` still comes before the last call's first push.
// MiniMax B (11-06, run 42): no new form and no lower score (3); retail loads the region index before the branch
// that tests the entry pointer, ours loads it after the branch, so the load and the `je` are swapped.
// score 3 (was 5; earlier 40): the passed `bool` and the `short` team are locals as in 0xd5d20 (retail stores the comparison
// result to a stack byte and pushes the dword, and sign-extends the team into esi/ecx); what is left is scheduling (the
// `mov eax, [edi]` before the `info` test, which register the object pointer and the 1 << type shift use) and the
// `lea ebx, [ecx + 8]` before the last call's first push.
// Tried: the region entry cached in a local before the `info &&` test (226).
// @coldspear's permuter (#104, 2026-10-30): 40 -> 36 only by reading a short where retail reads a dword (not kept).
// Round 473: coherent region input across guard/index/damage call, reciprocal lifetime/null order/native
// name combinations: 230/230/230/230/149/149/230/230. Original field reads with pointer-first guard 166,
// nested guard 40, eager null comparisons 238, nullable ordinal 40, definition flag first 58, upper range
// first 44; three-use owner address after/before initial damage 180/185. All restored; pointer-load
// scheduling, object/shift registers and final owner-address schedule remain.
// Round 40 (lane 14): the 0xb58c0 notify written as 0xd6790 has it (an object pointer read once for the
// test and the argument) took 40 -> 5: every register now agrees. Left: retail loads `mov eax, [edi]` above
// the info `je`, addresses the region flags as [eax + edx + 4] (ours [edx + eax + 4]) and does the `lea ebx`
// before the effect call's `push eax` (0xd9110 has the same swap). On that graph: the effect call straight
// to the owner-first 0x176780 40 (same code, on the old graph); region entry local before the guard 226
// (accumulator in EAX, old graph); region element pointer 5; one combined guard reading the entry at each
// use 201 (880 bytes); short region index 201; `!= NULL` guard 5; player test joined into the region test 5;
// a 600 s permuter run, 12 tries, none below 5.
// Round 339: Entry declaration swap 41; default-owned reciprocal 196; separate lower region gate 5,
// separate effect gate 5, coordinated info/object/ordinal/scale names 5; declarations/default 215;
// both gates/names 5; all forms 215. Original body restored; scheduling remains.
// Round 28 (lane 17, fast): the guard as an inline taking (info, entry) 227 (870 bytes; retail's `mov eax, [edi]` above
// the je and [eax + ecx + 4], but definition in EAX, info in EBP, the object spilled), a result-style inline 204 (877),
// an entry local before the guard 227; each with `(void)&definition` 227/204/227. Definition read once through its
// address, at the call / category / flag 224 each (back on the stack, info still in EBP), on the original guard 5.
// Object before info 227, 224 with that read; entry declared at the top 227, 224; flags through the field 228, 223.
// The first 0xdb090 argument through the field: entry local 26 (871, aligned 3), inline 25 (871, aligned 2: only
// `mov ecx, [edi + 4]` against `mov ecx, ebx`, and the lea), with the address read 25, result-style 206. On the entry
// form, the object field for the re-lookup 196, count through the field 186, test through the field 202. With info as
// the argument: `-=` written out 227, inline parameters swapped 227, nested flag test 227, result in a named bool 227.
// A test-only entry local 5, a has-entry bool 5, nested ifs 5 (the read stays below the je); an info copy 227; entry
// tested first 166; one symbol for entry and index 227; index read at each use 233 (877); short index 233.
// The lea: a shared inline effect wrapper in four parameter orders 5 (0xd9110 10); an owner pointer local 5.
// Round 337: result, normalized-damage and paired vitality workspaces at enclosing scope,
// seven subsets 5 each; all with the maximum-body workspace 5. Computations stayed at their
// original sites. Original body restored; region-load and final owner-address schedules remain.
// Round 69 (lane 1; the one-link harness reads 37 for this body, h): entry local with nested tests, NONE-default index
// or region pointer 224h (definition to a register argument, info in EBP, object spilled); short entry pointer read at
// each use 234h (880 bytes); owner-first call on today's graph, effect test split, `region_index + regions`, byte
// offset flags either order 37h (same code); fast: regions local 5, effect index read at each use 5, owner local 5.
// Round 74 (m3 lane 1): the `lea ebx` schedule was 0x176780's parameter order (tag, owner, object, scales...), 5 -> 3. Left: `mov
// eax, [edi]` above the info je and [eax + edx + 4]. The entry local gives both registers wrong only because info then outranks
// object: without one info read object takes EBP again (db090 arg 44h, flags 221h, regions 189h, guard test 203h). Harness (reads
// 35 for this body, h): entry local + k dead locals, k 0-12, 225h each; dead locals alone 35h; bool inline in the call 220h,
// inverse defaulted or as if/else 164h/224h, short index 234h, object first 224h, damage before info 239h, nested flag test 224h,
// `!(a && b && c)` 224h, second entry for the 0xdb090 argument 223h; region flag inlines 190h/35h/35h/35h, index inline 35h, region
// flags inline 35h, inline guard and flag-parameter guard 225h; word_flags_set for unknown12 35h, with the entry 225h; entry read in
// both arms of `if (info)` 216h (load above the je, accumulator in EBP); datum head same guard, 228h with the entry.
// M404: whole bounded-region admission in positive bool/byte/word/long and
// complementary bool/byte/word/long forms: 3/3/3/3/3/3/3/3. Frame, extent and
// region-load/address gaps unchanged; original nullable guard and complete body restored.
// P422: Native model-region record receiver, const/mutable with combined/nested/
// entry-first/named-admission guards: 228/228/167/228 twice (870/870).
// NOT kept: typed entry ownership changes the incoming convention and saved roles.
// P422: The required typed biped effect-tag field preserves 3/870 in the calibrated preview.
// M3 lane 16 r114 (fast): retail's form is the region entry read ahead of the guard: a two-use `entry` local (228, 870 bytes) or
// an inline guard taking (info, entry) (228) gives retail's hoisted `mov eax, [edi]`, and the inline also gives [index + regions + 4];
// what then differs is one choice: our build keeps `info` in EBP for its whole life and stores the object at entry (loaded into EBX
// after the definition's last use), retail the mirror image (object EBP, info stored at entry and loaded into EBX at the guard).
// Diagnostics on the entry local (reverted): one more read of `object` anywhere restores retail's registers with all five `info` uses
// (the player test through `object` 196 / aligned 5 lines; an extra field test in the last block 47 / aligned 3; one after 0xdb090
// 145 / aligned 4), as does one `info` use fewer (0xdb090's first argument through the field 24, aligned 3); k probe locals, k = 1..8,
// 228 each, and k = 1..16 on this body 3 each. Tried on the entry local or the inline guard, none tips it: the subtraction written out
// 228; the stun block's two field reads through a two-use pointer 228; the notify handle in a two-use local 227 (38 on this body);
// both 227; the guard taking the definition too 228; taking the object and its index as well (the player test inside) 318 (770 bytes,
// kept out of line); the guard with early returns 220, with a result flag 204, returning the flag test 220, parameters swapped 228;
// the count and elements read through a pointer to the pair 228; the shared actor stun-bounds inline (see 0xd9110) forced 228 with
// the entry local and 3 without. Lead: a natural form that gives `object` one more use (or `info` one fewer) on the inline-guard
// body should match; build/r114/lead-d8cb0.patch has that body.
// @retail 0xd8cb0
void function_d8cb0(long object_index, s_damage_effect_definition const *definition, s_type_1e6529 *data,
	s_damage_region_accumulator *accumulator)
{
	s_damage_info *info = accumulator->field_4_2;
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	real damage = function_d9020(object_index, (byte const *)accumulator, (byte const *)definition, data);

	if (damage > 0.0f)
	{
		bool ignore_difficulty = definition->category == 1;
		real maximum_body = function_d5d20(object_index, ignore_difficulty);
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
		s_damage_object *damaged = DAMAGE_OBJECT(object_index);
		if (damaged->unknownd4 != NONE)
			function_b58c0(damaged->unknownd4, 0x40);
		accumulator->unknown28 = body_damage;
	}

	s_damage_object *current = DAMAGE_OBJECT(object_index);
	real body_vitality = current->body_vitality;
	real maximum_body_vitality = current->maximum_body_vitality;

	if ((1 << current->type) & 3)
	{
		short team = current->team;
		maximum_body_vitality = function_1e9720(1, team) * maximum_body_vitality;
	}
	if (maximum_body_vitality * body_vitality <= 0.0f)
	{
		if (function_db110(object_index, false, false))
		{
			object->damage_flags.unknown12 = true;
		}
		else if (!TEST_FIELD_BIT(object->damage_flags.body_depleted))
		{
			function_d6800(object_index, &data->owner, true, (accumulator->flags >> 10) & 1);
			accumulator->flags |= 1;
		}
	}

	if (damage > accumulator->field_4_2->body_stun_damage_threshold)
	{
		real stun_time = accumulator->field_4_2->body_stun_time;

		if (((1 << object->type) & 3) && object->unknown12c != NONE)
		{
			byte *bounds = (byte *)function_1e4a10(*(long *)(g_4f55f0->data + (object->unknown12c & 0xffff) * 0x888 + 0x54));

			if (bounds)
				stun_time = *(real *)(bounds + 0x40);
		}

		real seconds = g_510c54->field_2_3 * stun_time;
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
		long effect_index = ((s_biped_definition const *)g_4e3b44[object->tag_index & 0xffff].bytes)->biped.area_damage_effect.tag_index;

		if (effect_index != NONE &&
			(!TEST_FIELD_BIT(object->damage_flags.body_depleted) && damage > 10.0f || (accumulator->flags & 0x41)))
		{
			function_176780(object_index, (s_effect_owner const *)&data->owner, 0.0f, effect_index, 0.0f, NULL, NULL);
		}
	}
	accumulator->damage -= damage;
}

void function_176870(s_effect_owner const *owner, long object_index, long marker_name, real scale_a, long tag_index, short unknown18, real scale_b, point3f const *origin, vector3f const *direction);
void function_176ad0(s_effect_owner const *owner, vector3f const *velocity, long marker_count,
	s_effect_marker *markers, long tag_index, long unknown30, real scale_a, real scale_b, point3f const *origin,
	vector3f const *direction, long mode);
void function_176330(point3f const *point, s_effect_marker *markers);

/* starts a destroyed permutation's effect: attached to the object, or at
   the marker with the object's velocity; the flag is the last parameter because
   retail's four callers evaluate it before the owner and effect pushes */
// @retail 0xd9d60
void function_d9d60(long object_index, long marker_name, long effect_index, s_damage_owner const *owner,
	bool at_marker)
{
	if (!at_marker)
	{
		function_176870((s_effect_owner const *)owner, object_index, marker_name, 1.0f, effect_index, NONE, 1.0f, NULL, NULL);
	}
	else
	{
		s_object_marker marker;
		vector3f velocity;
		s_effect_marker markers[3];

		if (!marker_name)
			marker_name = 0x400054f;
		function_b8d30(object_index, marker_name, &marker, 1, false);
		function_ba1d0(object_index, &velocity, NULL);
		function_176330(&marker.matrix.position, markers);
		markers[2].name = marker_name;
		markers[2].position = marker.matrix.position;
		markers[2].forward = marker.matrix.forward;
		function_176ad0((s_effect_owner const *)owner, &velocity, 3, markers, effect_index, marker_name, 1.0f, 1.0f,
			NULL, NULL, 1);
	}
}

/* a destroyed permutation's area damage, at its marker */
// @retail 0xd9e70
void function_d9e70(long object_index, s_damage_owner const *owner, long definition_index, long marker_name,
	byte unknown)
{
	s_object_marker marker;
	long marker_count = function_b8d30(object_index, marker_name, &marker, 1, false);

	if (marker_name == NONE || marker_name == 0 || marker_count)
	{
		s_type_1e6529 data;

		data.unknown7c = NONE;
		function_d6660(&data, definition_index);
		data.owner = *owner;
		*(byte *)&data.unknown84 = unknown;
		data.unknown54 = 1.0f;
		object_get_root_location(object_index, (s_location *)&data.unknown1c);
		data.origin = marker.matrix.position;
		data.position = marker.matrix.position;
		data.direction = marker.matrix.forward;
		data.unknown18 = object_index;
		function_d6c80(&data, NONE);
	}
}

/* destroys the child objects a destroyed permutation named: those attached
   for the model's permutations of that name */
// @retail 0xd9ff0
void function_d9ff0(long object_index, string_handle name)
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

				if (*(string_handle *)permutation == name)
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
								function_d6bc0(child_index);
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
void __stdcall function_b9b90(long object_index, bool disable);
void function_b9c60(long object_index, bool flag);
void __stdcall function_ba6f0(long object_index, long region_index, long state, bool flag);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
bool function_100390(long weapon_index, long barrel_index);
bool function_1003e0(long weapon_index, long barrel_index);
void function_10d4e0(long object_index);
void __stdcall function_fc330(long projectile_index, bool detach_contrail, real contrail_time); /* projectiles.cpp, 0xfc330 */
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
// MiniMax G (11-12): no new form, no lower score (476); retail's prologue takes only the object index in
// EAX and reads the other five arguments off the stack with a 0x38 frame, ours takes the region index
// and the permutation pointer in registers, and that argument set comes from this function's own first
// pass, so it cannot be changed from here without a new parameter order.
// score 517 (was 517): 476 before m3 lane 7 r155's region read through its address, 472 before r94's walk; frame 0x38 agrees; permutation remains on the stack instead of EAX,
// frame 0x38 agrees; permutation remains on the stack instead of EAX,
// r101: unchanged at 472; frame 0x38 agrees; permutation remains on the stack instead of EAX,
// with register homes/call setup and 1884/1868-byte length still differing.
// MiniMax D (11-10): no new form, no lower score (472): the second lookup still reloads the table pointer where retail derives the array base from the pointer it kept in ECX into EDX.
// MiniMax R (11-06): no new form, no lower score (472); the frame agrees, and the entry takes the argument first here and the global first in retail.
// b5N walk (11-06, against retail block by block, fast 472 unchanged): no behaviour difference found. Every field, offset, test
// direction, callee and argument agrees: region state word |= 1 << permutation (0xda1e8), the 0x22/0x20 NONE tests, the type&3 and
// unknown20 >= 3 seat-eject scan (0xda23b-0xda2bf), d9d60 (al = flags bit 20, 0xda2d9-0xda2e8), d9e70 (info+0x11 & 0x3f), the four
// flag-to-byte-0x19 stores, accumulator 0x400, the 0x60/0x300 weapon blocks, the six vehicle bits at +0x352, the db110 test (flags
// & 1 -> (obj, 0, 0), else flags & 0x10000 with state != 1 destroying or (obj, 1, 0)), the mode-4 once-only block, the child walk
// (bit 26), the section scan and kind > 2 split, the switch (jump table 0xda848, no default), the seat scan with the inlined
// iterator and event, and the tail d9ff0. No float precision issue: the only reals are the 0.0f at body_vitality (xorps/movss) and
// the 0.0f contrail time pushed as 0. The remaining differences are encoding only: the permutation (eax in retail, caller 0xd5f50
// `mov eax, esi`), `or word [x+0x352], si` (retail keeps the 1 in esi, ours narrows to `or byte [x+0x352], 1`), the drop value
// (retail `not eax; and eax, 1` as a long, it is function_100390/1003e0's barrel_index; as a long here 531), and the db110 result
// test (retail one shared call, `test al, al`; ours `neg al; sbb; inc`). Nothing was added or kept.
// Earlier 533 from 624: the body is statement for statement retail's (same size, same block order). What is left is
// the calling convention and the register allocation that follows from it: retail takes only the permutation index
// in eax and leaves `info` on the stack (`ret 0x14`, frame 0x38); ours passes the index in ecx and `info` in edx
// (`ret 0x10`, frame 0x3c, one spill slot more), so every [esp + N] is 4 off and the registers rotate. Retail also
// keeps the constant 1 in esi (`or word ptr [x + 0x352], si`, `cmp [state], esi`) where ours uses immediates, and
// calls 0xd9d60 with the flag in al; ours keeps passing it on the stack, because 0xd9d60 only takes retail's
// convention when this call is left out (then 0xd9d60 scores 8). Tried, no gain: `info` read through a volatile
// cast at every use (567, convention unchanged), a local for the bytes passed to 0xda860/0xdaa50 (534), the
// destroy test as a result variable (576), as `!(a ? b : c)` (542), as two || arms (559), permutation read before
// the object (546), the datum looked up afresh in the weapon blocks (571), the flag as `!= 0` or a bit-test local
// (546, 534). The destroy test also differs by retail's one shared call to 0xdb110 and our `neg al; sbb; inc` on its
// result; every form of it scored worse, and 0xdb110 itself does not match (187), so that part may follow from it.
// M272: native __stdcall declaration alone/with section, seat, both indexed scans 531/531/531/531;
// each with both drop results bool 472/472/472/472; bool results on the original declaration 545.
// Keep the native declaration plus both bool results; original cursor loops and every call role remain.
// M315: entry region/permutation before object and native ejection/seat cursor lifetimes,
// subsets 1/2/4/3/5/6/7 then all with entry event: 558/472/472/558/558/472/558/558.
// Original body restored; permutation transport, register homes and sixteen-byte excess remain.
// N321: One permutation-flag observation, shared resettable child iterator and direct nested
// permutation receiver tie or regress, including read-only input. Permutation stack/EAX transport and
// call-cycle homes remain. Scores 547/472/537/547/538/537/538/538.
// Original executable source restored.
// F335: uniform direct permutation/object/datum receivers, subsets 1/2/4/3/5/6/7, then
// entry event declaration: 980/662/541/1165/1022/794/1204/1204. Original cached receivers and
// body restored; incoming permutation transport and excess length remain.
// P274: Native zero-then-type/type-in-value event initialization, crossed with actual
// read-only info and existing inline declaration contracts: eight forms 477 at 1880 bytes.
// Original body and both declarations restored; permutation stack/EAX ingress and call-cycle homes remain.
// P281: Existing native-width writers for complete object, vehicle and accumulator flag groups:
// seven subsets all 472; all with a plain vehicle-byte bit ordinal 604 at 1940/1868 bytes.
// Original header, stores and full body restored; incoming permutation and first-word narrowing remain.
// M322: Direct section/seat/child handle observations, seven subsets 472 each;
// all with direct seat bound 503 at 1896 bytes. Original body restored;
// permutation stack/EAX transport, register homes and sixteen-byte excess remain.
// N328: Native section/seat/child last-consumer scopes, seven subsets, then one reset name carrier across
// all phases: 472 each. Original body restored; permutation stack/EAX transport, call-cycle register
// homes and sixteen excess bytes remain.
// M328: Independent child/seat iterator and event role names, seven subsets then
// all with the primary receiver renamed: eight 472s. Complete original body
// restored; frame agrees but permutation ingress and call-cycle homes remain.
// B383: Direct seat-ejection/current-state/child-bit consumers, seven subsets, then
// the effect flag at its argument: 504/519/512/531/530/519/531/551. Original native bit values and
// whole body restored; permutation ingress and call-cycle register homes remain.
// M330: Independent object/region/permutation declaration ranks, six orders, then
// both edge orders with uninitialized datum at entry: eight 472s. All producer
// assignments stayed fixed; original declarations and body restored.
// B386: Complete existing dispatcher orders 01234/02143/04213/01324/03124/02431/04231/03412:
// 515/513/474/522/521/473/475/515. Original cases and body restored; permutation ingress remains.
// Y560: Complete complementary body preservation, conditional/branch-owned
// bool/byte/short/long outcomes: 536/536/530/518/536/536/530/518. NOT kept: no byte match;
// original destruction expression and full body restored; permutation stack/EAX ingress remains.
// Round 74 (m3 lane 1, fast; the plain body stays the 472 form): retail's merged 0xdb110 call comes from a kept flag (`kept = true; if
// (flags & 1) kept = call; else if (flags & 0x10000) { if (state != 1) kept = false; else kept = call; } if (!kept) ...`) 565 at 1836;
// that + the datum read at each use in the drop blocks (retail's pool in EDI) and a datum pointer for the vehicle block 406 at 1868,
// masked 86; with long drops (`not eax; and eax, 1`) 500 at 1868, masked 75; NOT kept, the flag is set in the arms and tested after, the
// direct ternary on it 541 at 1900. Object first and no region local 513 (masked 83, retail's EBX permutation and EBP region, but no dead
// add); region spellings 500/500/474/515; long drop alone 531; `destroy` flag 570; nested kept 533; no `__stdcall` 545 (ECX/EDX, 1868).
// Y610: Permutation-flag/paired state-key/accumulator lvalue identities,
// subsets 1/2/4/3/5/6/7, then all with info-record identity:
// 472/472/472/472/472/472/472/472 (fast control 472).
// NOT kept: no exact identity-form match; original complete body restored.
// Lane 3 r230 (plain 472 at 1884 bytes; ours `ret 0x18`, all six on the stack under the explicit __stdcall, retail
// `ret 0x14` with the permutation in EAX): the whole function moved to the end of this file, plain 472 with `ret 0x18`
// unchanged (0xd9d60 69, 0xdae60 82), so its order inside this file does not choose the cycle entry; still BLOCKED-BY
// 0xb8540. The destroy block written in each arm (`if (flags & 1) { if (!db110(..)) destroy } else if (flags & 0x10000)
// { if (state != 1 || !db110(..)) destroy }`) gives retail's one shared 0xdb110 call (`push 0; push 0; jmp` into
// `push; call; test al, al; jne`) and keeps the constant 1 in a register as retail does, but 545 (1848 bytes); that
// without `__stdcall` 588 (`ret 0x10`); with long drop values 533; with the second arm as one condition 545.
// Round 101 (m3 lane 16): the walk. With function_d9d60's prototype moved ahead of function_dae60's and function_da110's (no
// `__stdcall`), 0xda110 gets a custom convention, region index in EAX and permutation in ECX, `ret 0x10`, frame 0x40: 528 (plain,
// 1868 bytes; dae60 83, daaa0 141, d5de0 394, db760 35). On that walk: no `region` local 555 (1852 bytes, still two register
// arguments); permutation_index declared last (all six calls changed with it) 530: the permutation arrives in EAX as in retail
// (`mov edi, eax`), the region index still in ECX (dae60 69, daaa0 64, d5de0 395); declared fifth 530 (same code); declared last
// without the region local 555. All restored. Retail loads the region index once into EBP, which never becomes a register
// argument, and keeps the permutation pointer in EBX; ours has them the other way. Retail also stores a constant 1 into the
// region argument's dead stack home and reloads it for `1 << permutation`, and holds 1 in ESI for the vehicle word and the
// state compare: a value 1 kept across the body (perhaps an inlined helper's parameter).
// m3 lane 1 r94: with lane 16 r107's t7 walk kept (0xd9d60 now matches) this function takes the region index in EAX and the
// permutation in ECX (`ret 0x10`, frame 0x40), 472 -> 476 by the walk alone; retail takes only the permutation in EAX and loads
// the region index once into EBP (so it stays a stack argument), with the permutation pointer in EBX, where this build gives the
// permutation pointer EBP. This build now keeps the constant 1 in EBX as retail keeps it in ESI. Plain full checks on that base
// (476): no region local with the permutation pointer declared first 571 (region still EAX); the effect flag passed to 0xd9d60
// without the `at_marker` local 476 (same code); the damage source passed to 0xd9e70 without its local 528; the seat-eject bit
// tested without `seat_ejects` 538; the child bit tested without `is_child` 513; the destroy block written in each arm (round
// 230's form, re-measured on this base) 553. Compile-only /Gd screen: the region-state update through word_flags_set gives the
// same code. BLOCKED-BY its own register allocation: the register-argument set follows the first pass's homes for the region
// index and the permutation pointer.
// m3 lane 11 r88 (plain unless marked fast): ours is now `ret 0x18`, all six on the stack under the kept `__stdcall`. A
// compile-only /Gd pass without it loads region_index and permutation_index once each (both become register arguments)
// and info, object_index, owner and accumulator several times; retail holds region in EBP. The at-marker bit passed
// straight to 0xd9d60 538 fast (1880 bytes); the destroy test as nested arms, `if (flags & 1) { if (!db110(o, false,
// false)) destroy } else if (flags & 0x10000) { if (state != 1 || !db110(o, true, false)) destroy }`, 545 (1848 bytes,
// masked 143 -> 130: retail's single cross-jumped 0xdb110 call, its 1 in a register and `or word ptr [x + 0x352], reg`,
// but 20 bytes short); that without `__stdcall` 588 (`ret 0x10`, still two register arguments); that with the drop
// blocks reading the datum through the pool at each use 546 (1900 bytes; 466 fast, where the reduced image keeps
// g_4e0300 in a register across the drop calls as retail does); with both drops as `long barrel_index` 543 fast (1900).
// m3 lane 16 r107: the t7 walk of 0xd9d60's note (function_dbfb0 ahead of function_dae60, function_d9640 after
// function_dbc80, 0xd9d60's marker/object swapped; a full check there loses nothing, 10563 matched) gives this function a
// custom convention, region in EAX and permutation in ECX, 476 at 1852 bytes (retail: permutation alone in EAX). Plain, on
// that walk, all restored: no region local 568 (1848 bytes); region and permutation declared before the object 570; no
// `__stdcall` 476 (same code); the region state pointer named at entry as in 0xdae60 572 (the permutation then arrives in
// EAX as retail, the region in ECX; 0xdae60 42, 0xdaaa0 126); that without the region local 562. The region index stays a
// register argument in every form.
// MiniMax A (11-12, run 63): no new form, no lower score (476); the run 62 note's "callers in other files" is
// wrong, all six call sites are in this file, and retail's prologue takes only the object index in EAX and reads
// info, permutation, owner, region and accumulator off the stack with a 0x38 frame, so matching it needs the
// declaration's parameter order changed, which is out of bounds.
// MiniMax A (11-12, run 62): no new form, no lower score (476); the frame is 0x40 against retail's 0x38 and the
// region index arrives in ECX where retail keeps it on the stack, so every stack home below the frame differs;
// that register-argument set belongs to this function's callers in other files (BLOCKED-BY).
// m3 lane 2 r76 (fast; with 0xd9d60's flag now last, this function 476): no region local 568; object declared after region and
// permutation 570; no `__stdcall` 476 (same code); a named region-state array for the update 547; permutation_index declared
// last 478 (the permutation arrives in EAX as retail, region in ECX; callers 0xdaaa0 67 -> 45, 0xdae60 48 -> 31; not kept: no
// retail evidence for the declared position). Region stays a register argument in every form: retail loads it after the
// pool, info and permutation loads have taken EAX/ECX/EDX, ours loads it first (into EBX).
// m3 lane 7 r155 (plain unless marked fast): the region index read once through its address for the region pointer gives
// retail's convention (`ret 0x14`, frame 0x38, the permutation alone in EAX, the region index a stack argument) and matches
// 0xdae60 and 0xdb760 (0xdaaa0 67 -> 45, 0xd5de0 229 -> 194; full check: nothing lost), kept; this function 476 -> 517 (1852
// bytes): the region index takes EBX and the permutation pointer EBP where retail has them the other way, and the constant 1
// stays in EBX from the drop blocks on (retail: immediates there, ESI from the vehicle block). On it, fast: both drop values
// as `long barrel_index` (the callees' parameter type, retail's `not eax; and eax, 1`) 511 at 1868 bytes (not kept, under 8);
// on that, the address read moved to the 0xa8360 call, declared first, or at the region-state update 511 each (same code);
// the drop blocks reading the datum through the pool at each use 523 (1900 bytes, the pool global reloaded per block); a local
// copy of the pool for them 523 (masked 80: the global reloaded rather than kept, the offset in EBX); the destroy block
// written in each arm of the destroy test 530 (masked 73, retail's shared 0xdb110 call shape).
// MiniMax A (11-13): no new form, no lower score (517): taking the permutation pointer straight from the info's region array
// (the one-use region pointer dropped) gives 557, so the region pointer stays; the first difference is still at +0xc, where
// the region index takes EBX and the permutation pointer EBP where retail keeps them the other way round, and retail's
// constant 1 travels through the region argument's dead stack home.
// m3 lane 14 r92 (fast with 0xdaaa0, 0xdae60 and 0xdb760, control 517): both drop values as long 511 (1868 bytes,
// re-measure); that with the drop and 0x20000 blocks reading the datum through a local copy of the pool at each use
// 523 (1900 bytes); the pool form alone 543 (1896 bytes); the drop values written at each call with no local 512 (1868
// bytes). Kept: the seat reads through the typed unit definition (unit.seats.count / unit.seats.address, the local named definition) 517 (same code).
// MiniMax H (11-14): no new form, no lower score (517): the first difference is still at +0xc, where the region index takes EBX and the permutation pointer EBP where retail keeps them the other way round, and the region's stack home follows from the declaration's parameter order, which this lane may not change.
// @retail 0xda110
void __stdcall function_da110(long permutation_index, s_damage_info *info, long object_index, s_damage_owner const *owner,
	long region_index, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	/* retail keeps the region index on the stack (`ret 0x14`, every caller pushes it): read once through its address */
	long const *region_reference = &region_index;
	s_damage_info_region *region = &info->regions[*region_reference];
	s_damage_info_permutation *permutation = &region->permutations[permutation_index];

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

	if (permutation->unknown22 != NONE && permutation->unknown20 != NONE)
	{
		function_b9b90(object_index, false);
		function_ba6f0(object_index, permutation->unknown22, permutation->unknown20, false);
		if (((1 << object->type) & 3) && permutation->unknown20 >= 3)
		{
			s_unit_definition *definition = (s_unit_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
			s_unit_child_iterator iterator;

			function_d0590(&iterator, object_index);
			while (function_d05c0(&iterator))
			{
				if (iterator.seat_index != NONE)
				{
					byte *seat = (byte *)definition->unit.seats.address + iterator.seat_index * 0xb0;

					bool seat_ejects = (*(dword *)seat >> 19) & 1;

					if (seat_ejects && *(long *)(seat + 0xac) == permutation->unknown22)
						function_b9c60(iterator.unit_index, false);
				}
			}
		}
	}

	if (permutation->unknown10 != NONE)
	{
		bool at_marker = (permutation->flags >> 20) & 1;

		function_d9d60(object_index, permutation->unknown24, permutation->unknown10, owner, at_marker);
	}
	if (g_4e6948->mode != 4 && permutation->unknown18 != NONE)
	{
		byte damage_source = info->unknown11 & 0x3f;

		function_d9e70(object_index, owner, permutation->unknown18, permutation->unknown28, damage_source);
	}

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
			short field_x11c898 = *(char *)(unit + 0x212);

			if (field_x11c898 != NONE)
			{
				long weapon_index = *(long *)(unit + 0x218 + field_x11c898 * 4);

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
			short field_x11c898 = *(char *)(unit + 0x212);

			if (field_x11c898 != NONE)
			{
				long weapon_index = *(long *)(unit + 0x218 + field_x11c898 * 4);

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
			function_fc330(object_index, false, 0.0f);
	}

	datum = &((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff];
	if ((1 << datum->type) & 2)
	{
		byte *vehicle = (byte *)datum->object;

		if (permutation->flags & 0x400)
			*(word *)(vehicle + 0x352) |= 1;
		if (permutation->flags & 0x800)
			vehicle[0x352] |= 2;
		if (permutation->flags & 0x1000)
			vehicle[0x352] |= 4;
		if (permutation->flags & 0x2000)
			vehicle[0x352] |= 8;
		if (permutation->flags & 0x4000)
			vehicle[0x352] |= 0x10;
		if (permutation->flags & 0x8000)
			vehicle[0x352] |= 0x20;
	}

	if ((permutation->flags & 1) ? !function_db110(object_index, false, false) :
		(permutation->flags & 0x10000) &&
		(g_4e6948->state != 1 || !function_db110(object_index, true, false)))
	{
		object->body_vitality = 0.0f;
		function_d6800(object_index, owner, true, false);
		accumulator->flags |= 1;
	}

	if (permutation->flags & 0x80)
	{
		if (g_4e6948->mode == 4)
		{
			s_damage_object *current = DAMAGE_OBJECT(object_index);

			bool already_set = current->unknown04[0] & 1;

			if (!already_set)
			{
				if (function_b9d20(object_index))
					function_bef30(object_index, 1, 0, 0, false);
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

			bool is_child = TEST_FIELD_BIT(child->flags04.permutation_child);

			if (is_child)
				function_d6800(child_index, owner, true, false);
			child_index = child->next_object_index;
		}
	}

	string_handle section_name = permutation->unknown3c;

	if (section_name && section_name != NONE)
	{
		long section_index = NONE;
		byte *sections = info->unknowne4;

		for (long i = 0; i < info->unknowne0; i++, sections += 0x14)
		{
			if (*(string_handle *)(sections + 4) == section_name)
			{
				section_index = i;
				break;
			}
		}
		if (section_index != NONE)
		{
			long kind = permutation->unknown02;

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
			case 0:
				break;
			case 2:
				function_daa50((byte const *)info, (byte const *)permutation, object->unknowne0, (word *)&accumulator->unknown2c);
				break;
			case 4:
				function_daa50((byte const *)info, (byte const *)permutation, object->unknowne2, (word *)&accumulator->unknown2e);
				break;
			case 1:
				function_da860((byte const *)info, (byte const *)permutation, object->unknowne0, (word *)&accumulator->unknown2c);
				break;
			case 3:
				function_da860((byte const *)info, (byte const *)permutation, object->unknowne2, (word *)&accumulator->unknown2e);
				break;
			default:
				__assume(0);
			}
		}
	}

	string_handle seat_name = permutation->unknown40;

	if (seat_name && seat_name != NONE && ((1 << object->type) & 3))
	{
		s_unit_definition *definition = (s_unit_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
		long seat_count = definition->unit.seats.count;
		long seat_index;
		byte *seat = (byte *)definition->unit.seats.address;

		for (seat_index = 0; seat_index < seat_count; seat_index++, seat += 0xb0)
		{
			if (*(string_handle *)(seat + 4) == seat_name)
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

	string_handle child_name = permutation->unknown48;

	if (child_name != NONE && child_name)
		function_d9ff0(object_index, child_name);
}

struct s_ai_scale_source;
bool function_1c8440(long actor_index, real *value, s_ai_scale_source const *source);
real function_15dec0(long player_index, long other_index);
bool function_0bfe60(const dword *flags, long bit);
struct s_game_allegiance_globals;
extern s_game_allegiance_globals *g_4f55ec;

/* a random real in [lower, upper] */
PRIVATE inline real damage_random_range(real lower, real upper)
{
	real random = function_x82e52f(&g_4e7408->unknown0, __FILE__, __LINE__);

	return (upper - lower) * random + lower;
}

/* the damage a damage event does to an object before armor: a random
   amount between the definition's bounds, scaled by the event, the owner's
   AI and the game engine; sets *friendly when the team scale applies */
// MiniMax B (11-13): no new form, no lower score (5): the permuter with annealing (27 tries) found nothing below 5.
// MiniMax B (11-12): no new form, no lower score (5): the two final return blocks keep this build's order - the stack reload before the register copy where retail puts the copy first; an inverted outer test with an early return (5), separate team and state returns (28), the not-enemy return hoisted (60) and a merged tail return (24) all fail to swap them.
// MiniMax A (11-13): no new form, no lower score (5): the CFG is retail's (both arms of the team test reach the one no-team exit) and only the order of the two cold exits differs; an inverted outer test (with and without an else), an explicit else and a single shared return all leave the layout alone or move the reload earlier, and two separate early returns give 28.
// MiniMax G (11-12): no new form, no lower score (5): the tail's two `return damage` blocks swapped inside the
// team test give 16, the no-team case as an early return before the team test gives 5 and the friendly case as
// an early return with one enemy tail gives 59; the two cold blocks keep retail's code but not retail's order.
// MiniMax G (11-12): no new form, no lower score (5): both cold return blocks have retail's code (the no-team block copies xmm1
// into xmm0, the not-enemy block reloads the damage from the stack), only the two blocks are laid out the
// other way round; an inverted outer test (5), an early return for the not-enemy case (16), an if/else (5),
// a single tail return (24) and a permuter run with annealing (30 tries) all leave the order unchanged.
// MiniMax G (11-12): no new form, no lower score (5): only the order of the two final "return damage" blocks
// differs - ours lays out the block that reloads the value from the stack before the one that returns the register
// copy, retail the other way round. The if/else arm, the early return, the inverted condition and the enemy flag's
// scope all leave the same two blocks.
// MiniMax H (11-12): no new form, no lower score (5): the two final `return damage` blocks keep our order (the stack reload before the xmm1 copy) where retail has the xmm1 copy first; the swapped tail conditions score 9, the inverted enemy test 16, an early team return 5 and a single tail return 24.
// MiniMax G (11-11): no new form, no lower score (5); the two final `return damage` blocks keep this build's order and retail
// puts the xmm1 copy before the stack reload; inverting the outer test (an early `return damage` when the team is none or the
// state is not 1) gives the same two blocks in the same order and the same score, and the enemy test in a bare block with one
// return at the end gives a single block instead of two.
// MiniMax H (11-11): no new form, no lower score (5); the two final `return damage` blocks keep our order, retail
// puts the xmm1 copy before the stack reload in the block that returns without a team.
// MiniMax A (11-11): no new form, no lower score (5); the two final `return damage` blocks keep our order, retail
// puts the xmm1 copy before the stack reload. Inverting the outer test (`if (team == NONE || state != 1) return
// damage;` with the enemy logic in a bare block) leaves the two blocks in the same order and the same score (5), and
// so do the team test as an early return and the damage product written in two statements (5). The original body is
// kept.
// MiniMax I (11-10): no new form, no lower score (5); an early return for the no-team case with the enemy test
// flattened into it keeps the same two blocks but pushes ecx in the prologue (59), so the frame changes. The two cold
// return blocks are still in the wrong order: retail's no-team block at 0xd9d42 copies xmm1 into xmm0 and the not-enemy
// block at 0xd9d4c reloads the damage from [esp+8], ours has the two the other way round.
// flattened into it keeps the same two blocks but pushes ecx in the prologue (59), so the frame changes.
// MiniMax C (11-09): an explicit else scores 5 and splitting the two returns into separate exits scores 28; an early return for the team test keeps the same block order (5), a single final return 24, an inverted enemy test 16 and a function-scope enemy flag 24; no lower score (5).
// MiniMax E (11-09): no new form, no lower score (5); the two final return blocks keep our order, retail puts the xmm1 copy before the stack reload; an early `return damage` for the not-enemy case moves the blocks the other way and costs 11 (16).
// MiniMax D (11-06, run 49): no new form, no lower score (5); the two final return blocks keep our order, retail puts the no-team return before the not-enemy one.
// MiniMax A (11-04, run 41): no new form, no lower score (5); the two final return blocks keep our order (the
// team-false one reloads damage from the stack where retail copies the xmm1 copy), and splitting the damage
// product into two statements gives the same code.
// MiniMax B (11-06, run 42): no new form and no lower score (5); the two final `return damage` blocks keep our order:
// retail puts the no-team return (the xmm1 copy) before the not-enemy one (the stack reload), ours the reverse, and
// writing the team test as an early return compiles to the same bytes.
// MiniMax I (11-11): no new form, no lower score (5); an early-return restructure (5), two separate early
// returns (28: the two blocks are tail-merged) and a single return (28) all fail, and `if (!enemy) return
// damage;` leaves the layout unchanged and only flips the branch polarity; a permuter run over this body was
// killed by its outer time limit before it printed anything.
// score 5 (was 108): only the order of the two final "return damage" blocks differs (retail
// puts the no-team return before the not-enemy one, ours the reverse). Tried: three early returns
// (16), a goto to one shared return (24), a flat team/state test (5, same order), separate team and
// state tests (28)
// Also tried: a single-exit `result` for the team part (44, frame 0xc), `else { return damage; }` on the team
// test (5), the team test as an early return before the enemy block (5), the enemy branch scaling `damage` in
// place with one final return (32), `enemy = team < 0 || team >= 16 || !bfe60(...)` (73). Every `return damage`
// becomes one block whose damage location (xmm1 or the stack home) the allocator picks; its order follows that.
// Also tried: damage declared before the random amount (5), team read at every use (5), a positive
// allegiance predicate (74, frame 4), enemy assigned in the valid/invalid team arms (67, frame 4), one result
// across both engine/team paths (42, frame 0xc), both paths scaling damage in place (5), the engine product
// computed at return (52), a team result initialized from damage then multiplied (5), enemy live from entry (5).
// MiniMax C (11-14): the team test as an early return with the state test left in place 5, both tests as
// early returns 28, and one enemy flag with a single final return (worse); no new form, no lower score.
// MiniMax H (11-14): no new form, no lower score (5): only the order of the two cold exits differs (this build reloads the stack before it copies xmm1, retail copies first); a merged tail return measures 24 and an explicit else 5, so the block order stands.
// @retail 0xd9b60
real function_d9b60(s_type_1e6529 *data, byte const *definition, long object_index, bool *friendly)
{
	real amount = *(real const *)(definition + 0x10);

	if (amount == *(real const *)(definition + 0x14))
		amount = *(real const *)(definition + 0x10);
	else
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

				if (((1 << object->type) & 3) && (bool)((*(dword *)((byte *)object + 0x134) >> 31) & 1) &&
					*(short *)(owner + 0x138) != 1)
				{
					damage = 0.0f;
				}
			}
		}
	}

	if (g_55e4d0[g_4e9ae8->engine_index])
	{
		real scale = function_15dec0(data->owner.player_index, function_d7ae0(object_index));
		real scaled = scale * damage;

		if (scale == 0.0f)
			data->in_unknown_radius = false;
		return scaled;
	}

	short team = data->owner.team;

	if (team != NONE && g_4e6948->state == 1)
	{
		bool enemy = true;

		if (team >= 0 && team < 16)
			enemy = !function_0bfe60((dword const *)((byte *)g_4f55ec + 0xc4), team * 16 + 1);
		if (enemy)
		{
			real result = function_1e9700(0) * damage;

			*friendly = true;
			return result;
		}
		return damage;
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
	vector3f direction;
	point3f origin;
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

void function_c9e70(long unit_index, dword flags, s_type_1e6529 const *data, s_damage_report const *report);
void function_211600(long arg_0, word arg_1, long arg_2);
void function_119280(long object_index, dword flags, s_damage_report const *report);

/* the creature branch of the damage report: when the damage has depleted the
   creature's body, starts its death timer (a random time in the tag's range)
   and tells its controller the damage's category; see
   docs/unknown_119280.md */
#pragma inline_depth(0)
// @retail 0x119280
void function_119280(long object_index, dword flags, s_damage_report const *report)
{
	s_damage_object *creature = DAMAGE_OBJECT(object_index);
	if (flags & 1)
	{
		byte *tag = g_4e3b44[creature->tag_index & 0xffff].bytes;

		if ((*(real *)(tag + 0x178) > *(real *)(tag + 0x17c) ? *(real *)(tag + 0x178) : *(real *)(tag + 0x17c)) > 0.0f)
		{
			real b = *(real *)(tag + 0x17c);
			real a = *(real *)(tag + 0x178);
			real ticks = ((b - a) * ((real)((g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f) >> 16) * (1.f / 65535.f)) + a) * g_510c54->field_2_3;
			long countdown;
			__asm
			{
				fld ticks
				fistp countdown
			}
			*(short *)((byte *)creature + 0x206) = (short)countdown;
		}
		if (*(long *)((byte *)creature + 0x134) != NONE)
		{
			byte *damage_effect_tag = g_4e3b44[report->definition_index & 0xffff].bytes;
			function_211600(object_index, *(word *)(damage_effect_tag + 0x12), report->owner.object_index);
		}
	}
}
#pragma inline_depth(255)

/* reports damage to the object it hit (units and type 12 objects); callers set up its register arguments
   last to first, and retail's 0xdc0a0 leas the report, then the accumulator, then the data */
// @retail 0xd9490
void function_d9490(s_type_1e6529 const *data, s_damage_region_accumulator const *accumulator, s_damage_report *report,
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
		function_119280(object_index, accumulator->flags, report);
}

long function_cbd50(long unit_index, short field_x11c898);
void function_101c80(long weapon_index, long unit_index);
void __stdcall function_b7880(long object_index, long node_index, point3f const *point, vector3f const *impulse,
	vector3f const *angular_impulse);
void function_fa820(long projectile_index, vector3f const *impulse); /* projectiles.cpp, 0xfa820 */
void function_10cf80(vector3f const *impulse, long item_index, bool flag);
void __stdcall function_de620(long arg_159e6d, vector3f const *impulse);
void function_119020(long creature_index, vector3f const *impulse);
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
void function_d9640(s_damage_report const *report, long object_index)
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
					function_101c80(weapon_index, object_index);
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
		vector3f direction = report->direction;
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
			bool pushable = true;

			if (type_mask & 3)
				pushable = !((*(dword *)((byte *)object + 0x134) >> 19) & 1);
			if (pushable)
			{
				bool large = report->scale > 0.5f && (*(dword *)(definition + 0x14) & 0x20);
				vector3f impulse;
				vector3f lifted_impulse;
				vector3f thrown_impulse;

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
					case 1:
					case 7:
					case 11:
						if (report->flags & 4)
							function_b7880(object_index, NONE, &report->origin, &lifted_impulse, NULL);
						else
							function_b7880(object_index, report->unknown40, &report->origin, &impulse, NULL);
						break;
					case 5:
						function_fa820(object_index, &thrown_impulse);
						break;
					case 2:
					case 3:
					case 4:
						function_10cf80(&thrown_impulse, object_index, large);
						break;
					case 0:
						function_de620(object_index, &thrown_impulse);
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

		if (player_index != NONE && record_pool_lookup(g_4e8c24, player_index))
			function_1e8fa0(player_index, object_index, report->unknown00);
	}
	if ((1 << object->type) & 3)
		function_ca0b0(object_index, report);
	if (report->flags & 0x400)
		function_d6bc0(object_index);
	function_a80f0(object_index, report);
}

enum
{
	k_maximum_damage_markers = 32
};

/* damages an object's regions through the model's damage markers within
   the damage's radius and cone, then its attached children the same way */
// score 203 (was 215): the code is the same instruction for instruction except for the material block: retail
// loads data->unknown7c again (`xor ecx, ecx; mov cx, [esi + 0x7c]`) for each function_188690 call, while ours
// keeps the first load (the inlined global_material_get's argument) in di across both calls, which makes the
// function 16 bytes shorter and shifts the loop padding after it. Tried: a short local for the material index in
// front of the check or inside the block (203), (word) casts on the arguments (203), armor pointers in locals
// (203), the multiplier in a local or `amount *=` (203), calling function_188690 for the check too (259, not
// inlined), the check's result in a local (203); `scale = 1` as the initial value with the PIN inside the if
// (270), PIN as a ternary (216), the PIN's lower/upper tests as statements (216).
// @retail 0xd82e0
void __stdcall function_d82e0(s_type_1e6529 *data, real damage, long object_index, s_damage_region_accumulator *accumulator)
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
				vector3f direction;

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
						scale = 1.0f - (distance - jpt->minimum_radius) / range;
					else
						scale = 1.0f;
					scale = PIN(scale, 0.0f, 1.0f);
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
				amount = function_d5bc0(definition->damage_group, definition->damage_group_b,
					*(string_handle *)((byte *)function_188690(data->unknown7c) + 0x10),
					*(string_handle *)((byte *)function_188690(data->unknown7c) + 0x14)) * amount;
			}
			if (amount > 0.0f)
			{
				real total = 0.0f;
				real maximum_body = function_d5d20(object_index, friendly);
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

					if (*(short *)(entry + 0xc) >= 0 && *(short *)(entry + 0xc) < info->region_count)
					{
						function_daaa0(info, object_index, &data->owner, *(short *)(entry + 0xc),
							share / info->regions[*(short *)(entry + 0xc)].vitality, accumulator);
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
			function_d9490(data, &child_accumulator, &report, child_index);
			if (g_4e6948->mode == 4)
				report.flags &= ~1;
			function_d9640(&report, child_index);
			if ((child_accumulator.flags & 4) && g_4e6948->mode != 4)
				function_b8540(child_index);
		}
		child_index = child->next_object_index;
	}
}

bool __stdcall function_cc010(long object_index, vector3f const *direction);
void __stdcall function_db210(s_type_1e6529 const *data, long vehicle_index);
void function_15cd90(long player_index, short identifier, long other_player_index);

enum
{
	k_maximum_damage_parents = 16
};

/* the damage info of an object's model (as 0xd5b60), which retail writes out inline in 0xd7b80's loop */
PRIVATE inline s_damage_info *damage_object_model_info(long object_index, s_damage_object const *object)
{
	s_damage_info *info = NULL;

	if (object_index != NONE)
	{
		byte *definition = g_4e3b44[object->tag_index & 0xffff].bytes;
		long model_index = *(long *)(definition + 0x38);

		if (model_index != NONE)
		{
			byte *model = g_4e3b44[model_index & 0xffff].bytes;

			if (*(long *)(model + 0x60) > 0)
				info = *(s_damage_info **)(model + 0x64);
		}
	}
	return info;
}

/* damages an object, and the objects it rides in: for each, the shield
   first, then the body and regions, then the aftermath */
// score 470 (was 661): close to retail's size (1907 bytes, retail 1880); what is left is the frame layout
// (retail's bools sit at esp+0x13..0x17, then object, i, definition, info; ours orders them differently and
// keeps region_absorbs in memory where retail holds it in bl), registers (retail's object_count is in edi, ours
// esi) and the position of the 0xd5b60 call. Done: shield/body children flags were swapped (retail: body from
// info bit 4, shield from bit 6); the loop's walk of 0xd5b60 is damage_object_model_info (retail inlines it
// there but calls it at the first use). Retail calls 0xd5b60 after the parent walk, but there the link-time
// inliner inlines it (1986 bytes, 661); called before the walk it stays a call (544, 470 with the original
// nested unknown7c branch), which is the form kept. Tried: the unknown7c store as one else-if 5 worse than the
// nested form (the compare is repeated); the walk's info/definition declared in the other order 517 on the
// 544 tree (but it shares the tag lookup, which retail recomputes); i/object/info hoisted to function scope
// 544; body/shield declared in the other order 544; the damage-info walk taking the datum index instead of
// the object 666.
// @retail 0xd7b80
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14)
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
	s_damage_info *local_9127e6 = (s_damage_info *)function_d5b60(object_index);

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

	if (local_9127e6)
	{
		body_children = !((local_9127e6->flags >> 4) & 1);
		shield_children = !((local_9127e6->flags >> 6) & 1);
	}

	for (long i = object_count - 1; i >= 0; i--)
	{
		s_damage_object *object = DAMAGE_OBJECT(objects[i]);
		s_damage_info *info = damage_object_model_info(objects[i], object);
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

			accumulator.field_4_2 = info;
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
					!function_1df560(object->team, data->owner.team))
				{
					accumulator.flags |= 0x10;
				}

				if (i == 0 && object->parent_object_index == NONE && !(data->flags & 0x200) &&
					definition->side_effect == 2 && function_cc010(objects[i], &data->direction) &&
					!TEST_FIELD_BIT(object->damage_flags.unknown7))
				{
					s_damage_object_datum *datum = &((s_damage_object_datum *)g_4e0300->data)[objects[i] & 0xffff];

					if (!((1 << datum->type) & 3) || function_1df560(data->owner.team, datum->object->team))
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
					function_d6a70(objects[i]);
					object->body_vitality = 0.0f;
					function_d6800(objects[i], &data->owner, !((data->flags >> 7) & 1), false);
					accumulator.flags |= 0x41;
				}

				if (!(data->flags & 0x20) && !(definition->flags & 0x200) && (!region_absorbs || data->in_unknown_radius) &&
					object->maximum_shield_vitality > 0.0f)
				{
					if (i == 0 || shield_children && (info->flags & 1))
						function_d9110(objects[i], definition, data, &accumulator);
				}

				if (g_4e6948->mode != 4 &&
					((1 << ((s_damage_object_datum *)g_4e0300->data)[objects[i] & 0xffff].type) & 2) && i == 0 &&
					accumulator.field_4_2 && (!(data->flags & 0x1000) || (info->flags & 0x100)))
				{
					function_db210(data, objects[i]);
				}

				if (!(definition->flags & 0x40) && (i == 0 || body_children && (accumulator.field_4_2->flags & 2)))
				{
					if ((data->flags & 1) || (data->flags & 0x100))
						function_d82e0(data, accumulator.damage, objects[i], &accumulator);
					function_d8cb0(objects[i], definition, data, &accumulator);
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

		function_d9490(data, &accumulator, &report, objects[i]);
		if (g_4e6948->mode == 4)
			report.flags &= ~1;
		function_d9640(&report, objects[i]);
		if ((accumulator.flags & 4) && g_4e6948->mode != 4)
			function_b8540(objects[i]);
		damage = accumulator.damage;
	}
}

/* passes damage a vehicle takes on to its riders, scaled by their seats'
   entries in its damage info, and to the vehicles it carries */
// @retail 0xdb210
void __stdcall function_db210(s_type_1e6529 const *data, long vehicle_index)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);
	s_damage_info *info = (s_damage_info *)function_d5b60(vehicle_index);
	byte *local_a2a045 = g_4e3b44[vehicle->tag_index & 0xffff].bytes;
	s_type_1e6529 rider_data = *data;
	s_unit_child_iterator iterator;

	function_d0590(&iterator, vehicle_index);
	rider_data.flags |= 0x200;
	while (function_d05c0(&iterator))
	{
		long unit_index = iterator.unit_index;
		s_damage_object *unit = DAMAGE_OBJECT(unit_index);
		real delta_x = unit->field_x221477.x - data->origin.x;
		real delta_y = unit->field_x221477.y - data->origin.y;
		real delta_z = unit->field_x221477.z - data->origin.z;
		real distance_squared = delta_z * delta_z + delta_y * delta_y + delta_x * delta_x;

		if (unit->unknown1fc == NONE || unit_index == data->unknown18)
			continue;
		if ((definition->flags14 & 1) && unit_index == data->owner.object_index)
			continue;
		if ((definition->flags14 & 8) && !function_1df560(unit->team, data->owner.team))
			continue;

		byte *seat = *(byte **)(local_a2a045 + 0x1cc) + unit->unknown1fc * 0xb0;

		for (long i = 0; i < info->unknownd8; i++)
		{
			byte *entry = info->unknowndc + i * 0x14;

			if (*(long *)entry == *(long *)(seat + 4))
			{
				real radius = *(real *)(entry + 8);
				real scale;

				if (radius * radius > distance_squared)
					scale = *(real *)(entry + 0xc) * definition->unknown48;
				else
					scale = *(real *)(entry + 0x10) * definition->unknown4c;
				rider_data.unknown5c = data->unknown5c * scale;
				function_d7b80(&rider_data, unit_index, NONE, NONE, NONE, NULL);
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
	long object_index = list_index;

	if (list_index != NONE)
	{
		reference_index = ((s_object_list_1dee50 *)g_4f55d8->data)[list_index & 0xffff].first_reference_index;
		object_index = function_1dee80(&reference_index);
	}

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
		object_index = function_x457076(&reference_index);
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
			if (object && (object_index == index || TEST_FIELD_BIT(object->flags04.permutation_child)))
			{
				long model_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38);

				if (model_index != NONE)
				{
					byte *model = g_4e3b44[model_index & 0xffff].bytes;
					long marker_count = *(long *)(model + 0x68);

					for (long i = 0; i < marker_count; i++)
					{
						byte *entry = *(byte **)(model + 0x6c) + i * 0x1c;

						if (*(string_handle *)entry == marker_name)
						{
							s_object_marker marker;

							if (!function_b8d30(index, *(string_handle *)entry, &marker, 1, false))
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
void function_d8a40(long object_index, string_handle region_name, real damage)
{
	if (object_index != NONE)
	{
		s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

		if (info && region_name && region_name != NONE)
		{
			long region_index;

			for (region_index = 0; region_index < info->region_count; region_index++)
			{
				if (info->regions[region_index].name == region_name)
					break;
			}
			if (region_index != info->region_count)
			{
				s_damage_region_accumulator accumulator;

				memset(&accumulator, 0, sizeof(accumulator));
				function_daaa0(info, object_index, g_467420, region_index, damage, &accumulator);
				if (accumulator.unknown2c || accumulator.unknown2e)
					function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);
				if (accumulator.flags & 4)
					function_d6bc0(object_index);
			}
		}
	}
}

bool function_cc410(long unit_index);
void function_155b60(long unit_index);

/* kills an object outright with the globals' default damage (falling,
   for one), reporting it as maximum damage */
// Setting `data.flags |= 4` right after `data.unknown54 = 1.0f` (before the accumulator stores) puts the `or edx, 4`
// in retail's place. The `lea`s for 0xd9490 come out in retail's order (report, accumulator, data) only with its
// parameters in the order data, accumulator, report.
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
				vector3f velocity;

				function_ba1d0(object_index, &velocity, NULL);
				if (*((byte *)object + 0x3dc) == 1 && -velocity.k > *(real *)(damage_globals + 0x5c) &&
					object->player_index != NONE)
				{
					function_155b60(object_index);
				}
			}

			function_d6a70(object_index);
			object->body_vitality = 0.0f;
			function_d6800(object_index, g_467420, true, false);

			s_type_1e6529 data;
			s_damage_region_accumulator accumulator;
			s_damage_report report;

			data.unknown7c = NONE;
			function_d6660(&data, *(long *)(damage_globals + 0x14));
			memset(&accumulator, 0, sizeof(accumulator));
			data.unknown54 = 1.0f;
			data.flags |= 4;
			accumulator.shield_damage = 1.0f;
			accumulator.unknown28 = 1.0f;
			accumulator.unknown08 = NONE;
			accumulator.flags = 0x49;
			accumulator.unknown18 = 3.4028235e38f;
			function_d9490(&data, &accumulator, &report, object_index);
			if (unknown)
				report.unknown50 = 0;
			function_d9640(&report, object_index);
		}
	}
}

struct s_collision_result_1697c0
{
	byte unknown00[8];
	point3f point;
	byte unknown14[0x24 - 0x14];
	short unknown24;
	byte unknown26[0x5c - 0x26];
};

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
vector3f *function_11d000(vector3f const *v, vector3f *out);
long function_baf80(long object_index);

PRIVATE inline bool collision_test_points(long flags, point3f const *start, point3f const *end,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result)
{
	vector3f vector;

	vector3d_from_points3d(start, end, &vector);
	return function_1697c0(flags, start, &vector, ignore_object_index, ignore_unit_index, result);
}

/* whether the structure blocks damage from the damage's origin to a point
   on an object; around units, four rays offset sideways by the
   definition's spread must all be blocked */
// @retail 0xd6f90
bool function_d6f90(long object_index, point3f const *point, s_type_1e6529 const *data)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_collision_result_1697c0 collision;

	if (((1 << object->type) & 3) && definition->ray_spread > 0.0001f)
	{
		vector3f offset;
		vector3f side;
		vector3f up;
		real side_scale;
		real up_i;
		real up_scale;
		bool result;

		vector3d_from_points3d(&data->origin, point, &offset);
		function_30bf0(function_11d000(&offset, &side));
		cross3f(&offset, &side, &up);
		function_30bf0(&up);

		for (long i = 0; i < 4; i++)
		{
			collision.unknown24 = NONE;
			switch (i)
			{
			case 0:
				side_scale = definition->ray_spread;
				offset.i = side.i * side_scale;
				offset.j = side.j * side_scale;
				offset.k = side.k * side_scale;
				break;
			case 1:
				side_scale = -definition->ray_spread;
				offset.i = side.i * side_scale;
				offset.j = side.j * side_scale;
				offset.k = side.k * side_scale;
				break;
			case 2:
				up_i = up.i;
				up_scale = definition->ray_spread;
				offset.i = up_i * up_scale;
				offset.j = up.j * up_scale;
				offset.k = up.k * up_scale;
				break;
			case 3:
				up_i = up.i;
				up_scale = -definition->ray_spread;
				offset.i = up_i * up_scale;
				offset.j = up.j * up_scale;
				offset.k = up.k * up_scale;
				break;
			}

			function_1697c0(0x4a08c2d, &data->origin, &offset, damage_object_root(object_index), data->unknown18,
				&collision);

			point3f start = collision.point;

			result = collision_test_points(0x14a08c2d, &start, point, damage_object_root(object_index),
				data->unknown18, &collision);
			if (!result)
				break;
		}
		return result;
	}
	else
	{
		collision.unknown24 = NONE;
		return collision_test_points(0x14a08c2d, &data->origin, point, function_baf80(object_index), data->unknown18,
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
// score 44 (was 62): the rest comes from function_d0620's convention. Retail passes it the
// iterator in ebx (object in eax) and keeps ebx as the zero constant afterwards, so no_bipeds
// lives in the frame; ours (d0620 is unmatched in unknown_0d0690.cpp) passes the iterator on
// the stack and keeps no_bipeds in bl. The tail loop after the vehicle update is otherwise
// identical (unsigned region_states_size / 8, state pointer loaded after the vitality clamp).
// @retail 0xdb670
void function_db670(long object_index, long vehicle_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);

	if (!TEST_FIELD_BIT(object->damage_flags.body_depleted))
	{
		bool no_bipeds = true;
		s_object_child_iterator iterator;

		function_d0620(&iterator, vehicle_index);
		while (function_d0690(&iterator))
		{
			if ((1 << DAMAGE_OBJECT(iterator.child_index)->type) & 1)
				no_bipeds = false;
		}
		if (no_bipeds)
		{
			s_damage_object *vehicle = DAMAGE_OBJECT(vehicle_index);

			vehicle->damage_flags.unknown12 = false;
			if (0.25f > vehicle->body_vitality)
				vehicle->body_vitality = 0.25f;

			s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset);

			for (long i = (dword)(long)object->region_states_size / sizeof(s_object_region_state); i > 0; i--, state++)
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
void function_db760(long region_index, long object_index, long permutation_index, long mode)
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

			function_d9d60(object_index, permutation->unknown38, permutation->effect_index, &owner,
				(permutation->flags >> 20) & 1);
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
// score 229 (was 400): 402, 607 before that; at r94's start 2148 bytes against 2170, frame 0x70. Round 75 (b4E) walked every block against retail 0xd5de0-0xd6657:
// one behaviour fix (the region decay else arm also runs when `info->region_count <= i`, retail 0xd6518 jle -> 0xd6571); every other test,
// field, callee, argument order and float op agrees. `if (shield > max) {clamp} else {changed}` arm order: no change (393 alone, 400 with the callers, the same as without it). Left: flag pairs that cross the word's bytes merge into one `and word` (three
// sites, each a line short, which shifts what follows), retail takes the 0xda110 permutation in EAX (`mov eax, esi`, BLOCKED-BY 0xda110's
// convention), `xor ecx, ecx` ahead of the team word at both 0x1e9720 calls, bounds and info in swapped frame slots, the 0x80 notify
// arguments hoisted over the clamp test, the second 0x138880 inlined (retail calls it), and scratch rotation in the region loop.
// Round 74 (m3 lane 1): `/ sizeof` without a cast (retail `shr`), one exit, flag writes through a flags pointer so they don't share the
// test's word load (retail's byte and/or), the permutation named after the owner copies, `>=` and `!(a > b)` arm orders, 0xd8b50's flag first, region pointer.
// Flag writes: direct bitfields 543 at 2193; existing word_flags_set 284 at 2170 but it loses matched 0xc2d00 (any inlined setter
// here does: an object-level one too, 543); the second flag of each pair through the object 284 at 2170 (two paths to one field to
// stop the merge: not kept, needs a ruling); word pointer or SET_FLAG through it 402; byte pointer, byte array or byte bitfields,
// through the object or a pointer, 457-472, register RMW. Figures below are on the 284 form. Bool locals for 0xd8b50's flag 281, masked 27 (direct 284, masked 38: one-use locals, not kept). `word team` at the two calls
// fixes the xor but is one-use (not kept). One maximum for shield and body 284 (slots rotate), with rate, recharge and stun shared 299; names don't move
// the slots (bounds renamed 290). Recharge spellings 299 each; ternary arms 299; `dword pending` 496; permutation `&= 7` 281; second
// 0x138880 nested, arms swapped: still inlined. --fast leaves src/juggernaut.cpp out, and 0x1588b0 only leaves xmm3 alone with it in:
// add 0x192950 to a fast check of this function.
// m3 lane 1 r94 (plain full checks): 400 -> 394 with the walk kept for 0xd9d60; every flag write through the existing
// word_flags_set on a word view of the damage flags declared beside them in the object (round 74's 284 form, whose one loss,
// 0xc2d00, no longer happens) 278 at 2170 bytes (kept; bits named after the members, same code); the 0x80 notify written once
// after the recharge/stun if/else instead of in its three arms 229 (kept: retail calls it in each arm through tail
// duplication, without our hoist of its arguments over the clamp test); only the two clamp arms merged 373. On 229: the clamp
// test positive with the clamp arm first 285 (2159 bytes, masked 38 -> 27: the notify block then agrees, the shortfall is
// the two 0xd8b50 flags); that with bool locals for those flags 278 at 2170, masked 23 (retail's byte temp; one-use, not
// kept); the body maximum and rate as ternaries 229, the stun count declared with them 229, the shield rate as an if/else
// 229, the two 0xd8b50 flags read through the word view 229 (all same code). Left (masked 38): 0xda110's convention at its
// call (ours region EAX + permutation ECX, retail permutation EAX), `xor ecx, ecx` before both team calls, the clamp arm
// order, the second 0x138880 inlined (retail calls it, so its stun count sits in SI), the 0xd8b50 flag temps.
// @retail 0xd5de0
bool function_d5de0(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	bool changed = false;

	if (((1 << object->type) & 3) && g_4e6948->state == 1 && g_4f55e4 && object->player_index != NONE)
		object->shield_stun_ticks = 10;
	if (info)
	{
		byte *bounds = NULL;

		if (info->region_count > 0)
		{
			long state_count = object->region_states_size / sizeof(s_object_region_state);
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

						owner.object_index = current->owner_object_index;
						owner.player_index = current->owner_player_index;
						owner.team = current->owner_team;

						long permutation_index = pending & 7;

						if (function_d9f70(i, permutation_index, info, object_index))
						{
							s_damage_region_accumulator accumulator;

							memset(&accumulator, 0, sizeof(accumulator));
							function_da110(permutation_index, info, object_index, &owner, i, &accumulator);
							if (accumulator.unknown2c || accumulator.unknown2e)
								function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);
							if (accumulator.flags & 4)
								function_d6bc0(object_index);
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
			function_d6800(object_index, &owner, true, false);
			word_flags_set(&object->damage_flags_word, _damage_object_flag_unknown13_bit, false);
		}
		if (TEST_FIELD_BIT(object->damage_flags.unknown5) || TEST_FIELD_BIT(object->damage_flags.unknown6) ||
			TEST_FIELD_BIT(object->damage_flags.unknown10))
		{
			if (g_4e6948->mode != 4)
			{
				function_dbfb0(object_index, NULL, TEST_FIELD_BIT(object->damage_flags.unknown6),
					TEST_FIELD_BIT(object->damage_flags.unknown10), false);
			}
			word_flags_set(&object->damage_flags_word, _damage_object_flag_unknown5_bit, false);
			word_flags_set(&object->damage_flags_word, _damage_object_flag_unknown6_bit, false);
			word_flags_set(&object->damage_flags_word, _damage_object_flag_unknown10_bit, false);
		}

		long state = g_4e6948->state;
		real maximum_shield = 1.0f;

		if (state == 2 && ((1 << object->type) & 3) && object->player_index != NONE)
			maximum_shield = function_1588b0(object->player_index, 2);

		if (!TEST_FIELD_BIT(object->damage_flags.body_depleted) && object->maximum_shield_vitality > 0.0f)
		{
			if (TEST_FIELD_BIT(object->damage_flags.shield_double_charged))
			{
				object->shield_vitality += 2.0f / (g_510c54->field_2_3 * 2.0f);
				if (object->shield_vitality >= 3.0f)
				{
					object->shield_vitality = 3.0f;
					word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_double_charged_bit, false);
					word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
				}
				else
				{
					word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, true);
					changed = true;
				}
			}
			else if (object->shield_vitality > maximum_shield && state == 2)
			{
				long player_index = unit_get_player_index(object_index);
				real drain = 2.0f / (g_510c54->field_2_3 * 90.0f);
				real excess;

				excess = object->shield_vitality - 1.0f;
				word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
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
						word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_depleted_bit, false);
						function_ba7f0(object_index, NONE, 2, 0);
					}
					if (!TEST_FIELD_BIT(object->damage_flags.shield_recharging))
					{
						s_damage_owner owner;

						object_get_damage_owner(object_index, &owner);
						function_176780(object_index, (s_effect_owner const *)&owner, 0.0f, *(long *)((byte *)info + 0xb8), 0.0f,
							NULL, NULL);
					}
					word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, true);
					object->shield_vitality += recharge;
					if (!(object->shield_vitality > maximum_shield))
					{
						changed = true;
					}
					else
					{
						object->shield_vitality = maximum_shield;
						word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
						word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_damaged_bit, false);
					}
				}
				else
				{
					if (!function_138880() && stun_ticks != 0x7fff)
					{
						object->shield_stun_ticks = stun_ticks - 1;
						changed = true;
					}
					word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
				}
				function_a7a30(object_index, 0x80);
			}
			else
			{
				word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
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
					word_flags_set(&object->damage_flags_word, _damage_object_flag_body_recharging_bit, true);
					object->body_vitality += recharge;
					if (object->body_vitality > maximum_body)
					{
						object->body_vitality = maximum_body;
						word_flags_set(&object->damage_flags_word, _damage_object_flag_body_recharging_bit, false);
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
			long state_count = object->region_states_size / sizeof(s_object_region_state);
			s_object_region_state *state = (s_object_region_state *)((byte *)object + object->region_states_offset);

			for (long i = 0; i < state_count; i++, state++)
			{
				if (state->unknown02 > 0)
				{
					if (state->damage_level_ticks == 0 && info->region_count > i)
					{
						s_damage_info_region *region = &info->regions[i];

						if (*(real *)((byte *)region + 0x2c) > 0.0f)
						{
							real decay = g_510c54->rate * *(real *)((byte *)region + 0x2c) * 255.0f;
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

		/* retail passes the character's vitality properties ([esp+0x14] at
		   0xd6598, NULL for a unit no actor drives), not the damage info */
		changed |= function_d8b50(&object->unknownf8, &object->unknown100, (char *)&object->unknown109, bounds,
			TEST_FIELD_BIT(object->damage_flags.body_depleted));
		if (TEST_FIELD_BIT(object->damage_flags.unknown11))
		{
			if ((char)object->unknown108 > 0)
			{
				object->unknown108--;
				object->unknownf4 = object->unknownfc + object->unknownf4;
			}
			changed = true;
		}
		else
		{
			changed |= function_d8b50(&object->unknownf4, &object->unknownfc, (char *)&object->unknown108, bounds,
				TEST_FIELD_BIT(object->damage_flags.shield_depleted));
		}
	}
	return changed;
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

PRIVATE inline long damage_section_list_size(s_section_lists *lists, long list_index)
{
	return list_index == NONE ? lists->unlisted.count : lists->lists[list_index].count;
}

PRIVATE inline long damage_section_list_entry(s_section_lists *lists, long list_index, long i)
{
	return list_index == NONE ? (char)lists->unlisted.sections[i] : (char)lists->lists[list_index].sections[i];
}

PRIVATE inline byte *damage_constraint_element(s_physics_constraint_iterator *iterator, long *element_size)
{
	s_physics_constraint_block *block = (s_physics_constraint_block *)physics_model_shape_block_get(iterator->physics,
		(s_physics_model_shape_key const *)&iterator->type, element_size);

	if (iterator->index >= block->count)
		return NULL;
	return block->elements + iterator->index * *element_size;
}

/* the model node a physics node belongs to (a copy of
   render_model_find_marker_group, 0x16d890) */
PRIVATE inline long model_find_physics_node(long model_index, short physics_node)
{
	byte *model = g_4e3b44[model_index & 0xffff].bytes;
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

				changed = false;
				iterator.physics = g_4e3b44[*(long *)(model + 0x24) & 0xffff].bytes;
				function_1eb110(&iterator);
				for (;;)
				{
					long element_size;
					byte *constraint = damage_constraint_element(&iterator, &element_size);

					if (!constraint)
						break;
					if (!function_db4c0(object_index, iterator.type, iterator.index))
					{
						word list_a = *(word *)(constraint + 4);
						if (list_a != 0xffff)
						{
							word list_b = *(word *)(constraint + 6);

							if ((short)list_b != NONE)
							{
								for (long i = 0; i < damage_section_list_size(&lists, (short)list_a); i++)
								{
									for (long j = 0; j < damage_section_list_size(&lists, (short)list_b); j++)
									{
										long section_a = damage_section_list_entry(&lists, (short)list_a, i);
										long section_b = damage_section_list_entry(&lists, (short)list_b, j);

										if (section_a != NONE && section_b != NONE)
										{
											byte *rows = *(byte **)(physics + 0x3c);
											byte *row_a = rows + section_a * 0x90;
											byte *row_b = rows + section_b * 0x90;
											long node_a = model_find_physics_node(*(long *)(object_definition + 0x38), *(short *)(row_a + 2));
											long node_b = model_find_physics_node(*(long *)(object_definition + 0x38), *(short *)(row_b + 2));

											if (node_a != NONE && node_b != NONE)
											{
												dword bit_b = 1 << node_b;
												dword bit_a = 1 << node_a;

												if (((node_mask & bit_a) != 0) ^ ((node_mask & bit_b) != 0))
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
					}
					function_1eb160(&iterator);
				}
			} while (changed);
		}
	}
	return node_mask;
}

/* the model node of the object's lowest ranked physics node */
// MiniMax B (11-13): no new form, no lower score (4): the tag index in a local (4), the definition bytes in a local (4), the definition bytes local first (7), both locals (4), the short tag index (45) and the typed object definition's model tag read (4) all leave the same four instructions, and the permuter with annealing (23 tries) found nothing below 4.
// MiniMax A (11-13): no new form, no lower score (4): the tag index is still masked after `push ebp` where retail masks it before, and the tag entry still goes into edx where retail reuses eax; a local for the masked index and a local for the tag entry leave the same four instructions, the permuter (307 s, 29 tries) found nothing under the baseline, and caching the masked index or the tag data pointer in a local changes neither difference.
// MiniMax G (11-12): no new form, no lower score (4): nothing new this round; the tag index is still masked after `push ebp`
// where retail masks it before, and the tag entry still goes into edx where retail reuses eax.
// MiniMax G (11-12): no new form, no lower score (4): the tag index is still masked after `push ebp` where retail
// masks it before, and the tag entry still goes into edx where retail reuses eax; a local for the masked index and
// one for the tag entry leave the same four instructions.
// MiniMax H (11-12): no new form, no lower score (4): the tag index is still masked after `push ebp` where retail masks it before, and the tag entry still goes into edx where retail reuses eax.
// MiniMax G (11-11): no new form, no lower score (4); retail masks the tag index before it pushes EBP and reuses EAX for the
// tag entry while this build masks it after the push and takes the entry in EDX; a local for the masked index, a local for the
// definition bytes and the tag table read through a local all leave the current code in place.
// MiniMax H (11-11): no new form, no lower score (4); retail masks the tag index before it pushes EBP and reuses EAX
// for the tag entry; a local for the model bytes, a local for the masked tag index and a local for the tag entry all
// compile to the same four instructions, and the same source line in the matched 0xd9ff0 gives retail's EAX reuse.
// MiniMax A (11-11): no new form, no lower score (4); retail masks the tag index before it pushes EBP and reuses
// EAX for the tag entry (`mov eax, [eax + esi + 8]; mov eax, [eax + 0x38]`), ours masks it after the push and takes
// the entry in EDX. A local for the masked tag index, a local for the definition bytes and the tag table read through
// its own address all leave the same four instructions (4), and the model index read first only moves the first
// difference (7). The original body is kept.
// MiniMax I (11-11): no new form, no lower score (4); naming the masked tag index in its own local leaves the same
// four instructions (the mask after `push ebp`, and the tag entry in edx where retail uses eax).
// MiniMax I (11-10): no new form, no lower score (4); a local for the model bytes and one for the masked tag index compile to the same four instructions, so the entry load still lands in edx; naming the masked tag index in its own local (4) leaves the same four instructions.
// MiniMax C (11-09): no new form, no lower score (4).
// MiniMax E (11-09): no new form, no lower score (4); the tag index is still masked after `push ebp` where retail masks it before, and the tag entry is still loaded into edx where retail reuses eax; a tag-entry local and a masked-index local both leave the same four instructions.
// MiniMax D (11-06, run 49): no new form, no lower score (4); the tag index is still masked after `push ebp` where retail masks it before, and the tag entry is still loaded into edx where retail uses eax.
// MiniMax A (11-04, run 41): no new form, no lower score (4); naming the masked tag index in a local leaves the
// same four instructions (the mask after `push ebp`, and the tag entry in edx where retail uses eax).
// MiniMax B (11-06, run 42): no new form and no lower score (4); retail masks the tag index before `push ebp` and
// keeps the tag entry in eax, ours masks it after the push and takes the entry in edx.
// score 4 (was 134): retail calls 0x16d890 here (its only caller), so it is declared noinline ahead of its
// definition in unknown_16d280.cpp (noinline only on this file's declaration still inlines it: 134). Left: the
// prologue's order (retail masks the tag index before it pushes EBP) and the register that holds the tag's
// definition (retail reuses EAX). Fixes made: the rigid body loop guarded by its count before its first
// pointer is formed, `j` declared before the inner guard, the model index read after the two results are set
// (28 -> 4). Tried: the model index read first 7, a definition pointer local 4, the row read inside the loop
// 4, an up-counting loop over an indexed rigid body 4. Before the guard: the row and index base pointers as
// locals before the inner loop (87, 116 with both), a component local dropped (84), `long j = 0` before the
// `if` (111), the inner `for` under the `if` (69).
// Also tried: `model_index` renamed (4) and every head local renamed (4: names do not steer it), no `object`
// local with DAMAGE_OBJECT(object_index) at each use (17), `result = lowest_rank = 0x7fffffff` (7), `lowest_rank`
// declared first (7); the permuter (51 tries) found nothing under 4.
// Also tried: result before the object lookup (17), rank before it (7), both before it (17), rank after the
// model lookup (6), a signed-short rank (4), the pointer advanced in the body (4), a count-consuming while
// with pointer advance in the body (4), read-only physics/rows/node indices (4).
// Round 113 (fast): the TAG_GET macro 4; a tag-entry pointer 4; a direct tag-object lookup with the object kept 4;
// nested initial guards 4; read-only object/model 4/4; the model declared before its assignment 4; output and rank
// assigned after the lookup 7; rank from output 4; output from rank 7.
// P338: complete tag-table snapshot at entry/object/output/minimum boundaries, direct or tag-data local:
// 39/34/34/34 twice (313/305 bytes). One scalar owns successive tag handles at four input boundaries,
// mask at acquisition or traversal: eight trials 4 (305/305 bytes). Original body restored;
// NOT kept: input lifetime/scalar ownership did not recover EAX reuse and the native save/mask schedule.
// MiniMax C (11-14): the model tag read through the typed tag field, the tag index through a named local
// and the definition through a named pointer all give 4 (unchanged); no new form, no lower score.
// MiniMax H (11-14): no new form, no lower score (4): the four instructions that differ are the tag index's mask, the tag entry's two loads and the entry's field; retail masks the index before it pushes EBP and reuses EAX for the entry, and no spelling of them that this lane may write moves the mask ahead of the push.
// @retail 0xdbb40
long function_dbb40(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	long result = 0x7fffffff;
	long lowest_rank = 0x7fffffff;
	long model_index = *(long *)(g_4e3b44[object->tag_index & 0xffff].bytes + 0x38);

	if (model_index != NONE && object->node_index != NONE)
	{
		long physics_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0x24);

		if (physics_index != NONE)
		{
			byte *physics = g_4e3b44[physics_index & 0xffff].bytes;
			byte *component = g_51e9b8->data + (object->node_index & 0xffff) * 0xa0;
			long rigid_body_count = *(long *)(component + 0x74);

			if (rigid_body_count > 0)
			{
				byte *rigid_body = *(byte **)(component + 0x70) + 0x48;

				for (long i = rigid_body_count; i > 0; i--, rigid_body += 0x60)
				{
					long node_count = *(long *)(rigid_body + 4);
					long j = 0;

					if (node_count > 0)
					{
						byte *rows = *(byte **)(physics + 0x3c);
						char *node_indices = *(char **)rigid_body;

						do
						{
							byte *row = rows + node_indices[j] * 0x90;
							long rank = *(short *)row;

							if (rank < lowest_rank)
							{
								lowest_rank = rank;
								result = render_model_find_marker_group(model_index, *(short *)(row + 2));
							}
						} while (++j < node_count);
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
	transform4x3f matrices[64];
	vector3f linear_velocities[64];
	vector3f angular_velocities[64];
};

void havok_component_node_states_get(s_havok_component *component, s_havok_node_states *states);
void havok_component_node_states_set(s_havok_component *component, s_havok_node_states const *states);
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner);
point3f *function_b9dd0(long object_index, point3f *result);
long __stdcall function_b7b40(void *creation);
void __stdcall function_1c3770(long object_index, dword flags);
extern s_record_pool *g_51e9b8;

/* what a new object is made from (b7930, b7b40) */
struct s_damage_object_creation
{
	byte unknown00[0x1c];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
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

// MiniMax G (11-14): no new form, no lower score (156): the mask's zero store, the piece loop's guard order, the
// search's entry test and the scratch registers of the last two lookups still differ.
// MiniMax A (11-12): no new form, no lower score (156): the mask zero is still stored ahead of the states call where retail stores it in the node loop preheader, and the piece loop guard order, the search entry test and the scratch registers of the last two lookups still differ; storing the zero inside the block gives retail store but reallocates the frame (170), so the body stands.
/* marks damage sections destroyed, and splits off the model nodes no
   longer joined to the object's root as new objects */
// MiniMax B (11-12): no new form, no lower score (156): the mask's zero store, the piece loop's guard order, the search's entry test and the last two lookups' scratch registers still differ.
// MiniMax H (11-12): no new form, no lower score (156): the node loop's guard, the search's entry test and the mask's zero placement are unchanged; the lane history's best form (the piece pointer with the piece loop as `!= 0`) is 157 and the retail guard order is 187.
// MiniMax H (11-13): the mask's zero moved after the states call 154 (the store then lands ahead of the mask test, retail has it inside the mask block), the zero assigned after the 0xba690 call 170 and in the for-init 231 (both rebuild the function: the datum offset stays in ESI and the node count takes 0x18), so 156 stands.
// score 156 (was 156): 174 before r69; frame 0x13e0, 808 bytes and the first 69 instructions agree (handle in ESI, offset and detached
// mask in memory). Left: the mask's zero is stored ahead of the states call (retail: at the first node loop), the piece
// loop's guard order and bottom test, the search's entry test, and the scratch registers of the last two lookups.
// MiniMax C (11-01): the shared loop counter and the permuter's variant reach 217, four points and under the keep bar, so nothing is kept.
// MiniMax E (11-06): no new form, no lower score; the mask's zero store, the piece loop's guard order
// and the last two lookups' scratch registers remain, and the shared loop counter and the permuter's
// variant only reach 217.
// MiniMax G (11-06): no new form, no lower score kept; the mask's zero is stored before the mask block,
// where retail stores it in the node loop's preheader; storing it inside the block gives retail's store and
// nop but a different frame layout (170). Retail's guard order and its do-while search each cost four bytes.
// MiniMax D (11-09): no lower score kept; the detached mask's zero declared inside the mask block after the model
// lookup reaches retail's store and its loop-alignment nop but reallocates the whole frame (170), the zero assigned
// inside the block 170, an explicit `node_count > 0` guard 156, the zero declared after the states call but outside
// the block 154 by a declaration move only.
// MiniMax C (11-09): no new form, no lower score (156); the datum offset still spills at +0xc instead of +0x10 and
// the definition and detached/node homes still differ, so the first 69 instructions agree and the rest do not.
// Earlier 156 (was 156): 174 before m3 lane 1 r69. Frame 0x13e0, 808 bytes and the first 69 instructions agree (handle in ESI, offset and detached
// mask in memory). Left: the mask's zero is stored ahead of the states call (retail: at the first node loop), the piece
// loop's guard order and bottom test, the search's entry test, and the scratch registers of the last two lookups.
// Earlier 174 (was 188): frame 0x13e0 and 808-byte length agree; first +0xb acquires the handle/offset
// in different registers. Datum/definition and detached/node homes, creation copies and scheduling remain.
// Earlier 188: frame 0x13e0, handle ESI and first-mask EDI agree; first +0x37 is the offset home.
// Definition/detached-mask/node and copy homes remain; 776 bytes against retail 808.
// Earlier 221: caching `info->unknowne0` as the loop bound (retail does not reload it; 235 -> 221) and
// making detached_nodes signed (`test / jle` as retail; same score). Left is register allocation: retail keeps
// object_index in esi, hoists the section_mask_a zero-extension into edi and spills the datum offset, so its
// frame is 4 bytes larger; ours keeps the offset in esi and object_index in edi. Tried: the loop as
// `if (detached_nodes > 0 && ...) while (detached_nodes != 0)` (236), a `word mask_a` local (221).
// Round 273: Shared scan ordinal/node-count-first/entry node-state workspace, seven
// subsets 221/225/225/221/221/225/225; all with entry creation workspace 225.
// Shared ordinal recovers base-first byte access at 221; frame 0x13dc versus 0x13e0
// and handle/offset allocation remain. Original complete body restored.
// Round 529: creation workspace at first writer with detached-mask/piece-mask/node-result
// lifetimes 221/188/221/188/221/188/221/188. Earlier detached-mask initialization alone
// with original creation and other lifetimes 188, confirmed plain; kept at 776/808 bytes.
// Frame 0x13e0, handle ESI and first-mask EDI recovered; offset and later homes remain.
// Round 532: entry detached bitmap, shared marking/node ordinal on the recovered frame, complete
// scalar role names, seven subsets 211/188/211/188/211/188/211; full workspace names 211.
// Entry bitmap is 797/808 bytes, first +0xa; all others retain displaced homes. Opening body restored.
// B374: actual metadata cursor crossed with flag-byte cursor, ordinal-first initialization and
// pre-capture cursor declaration: eight trials 209 at 799/808 bytes, first +0xa.
// Original body restored; offset/definition and detached-mask homes remain despite the closer length.
// Round 534: connectivity-mask workspace shared by root and piece scans, dword/long,
// entry/producer declaration and sequential section/node count: 188/188/188/188/188/188/188/188.
// Original complete body restored; offset, definition and detached/node homes remain.
// N324: Complete section/connectivity/selection shift parts, seven subsets then both basis vectors copied by
// library 242/188/242/188/242/188/242/242. Original expressions/copies and body restored; offset/definition
// and detached/node homes remain.
// P281: Creation-tag admission outside the signed detached-mask loop, with conditional info scope,
// complemented result and first-writer creation scope: eight forms 174 at 808/808 bytes. Only the
// invariant admission retained, ordinary confirmed; every added scope/result form restored.
// DH305: Twice-consumed complemented piece mask, entry/component/detach/piece declaration
// scopes, dword/long: 188/188/188/188/188/188/188/188. All canonicalize; original body restored;
// offset/definition and detached/node homes remain.
// M331: Detached bitmap zero at six capture/receiver/count boundaries, then two separate
// declaration/assignment forms on invariant creation admission: 174/194/194/194/215/215/194/215. NOT kept:
// no qualifying gain or byte match. Original complete body restored; entry handle/offset and detached/node
// homes remain.
// P283: Actual node-size/offset fields, eight-byte metadata records and shared
// header index, seven subsets then unsigned index: 174 each (808/808). All tie;
// original data layouts, indices, observations and complete body restored.
// Native handle/offset and detached-mask homes remain unresolved.
// AC499: Complete scaled header-offset ownership / native definition fields / native basis fields,
// seven subsets then split offset declaration: 174/174/174/174/174/174/174/174 (808/808).
// Original data layouts, observations and complete body restored; handle/offset and node homes remain.
// N336: Combined/separate admission returns, section/tag cleanup joins and detach continues
// 174 each; bounded-search result and direct selected-node scan 217/217 (811/808 bytes);
// piece-failure continues 174. Original complete body restored; handle/offset and later homes remain.
// N338: Actual object-header/tag/component lookup-source receivers, seven subsets and
// table-first acquisition, 174 each at 808/808 bytes. NOT kept: one-use receiver forms
// require a match. Original complete body restored; native handle/offset and detached/node homes remain.
// Round 69 (lane 1): existing 0xba690 for the node state arrays, same code 174 (kept, replaces the written-out lookup).
// A named node bit in the node loop with the mask updated last 156 (kept); update last alone 174; with the zero moved
// into the block 174. unknownbc stored before unknownc0 156 (kept: aligned 33 -> 29). On it: a separate remaining mask
// for the piece loop under `remaining > 0 && tag != NONE` + `while (remaining != 0)` 155, aligned 23, retail's guard
// order; NOT kept, its guard repeats the loop's test for one point. Same with a search helper 155; the search as
// do-while, helper or not, 220 (788 bytes, head flips); zero at the block top, or after 0xba690, 178. Without the
// remaining mask: the guard 187 (777), search do-while 185; zero moved 170. Piece node in a named local 156 (no change).
// Harness (reads 176, 805 bytes for this body): outer `detached > 0 && tag != NONE` + `while (detached != 0)` 188,
// aligned 126 -> 72, 776 bytes; on it do-while, search as do-while, both, dword detached, long attached, declaration
// orders, loop-scoped ordinals, named tag 188 each; zeroing at the first node loop 217 (790, head allocation flips).
// Round 74 (m3 lane 1, h = harness at 155, 805): the mask's zero, search do-while and guard grid 155/172/186/225/188/225/188/225h; with
// a remaining mask 154/180/218/218h (fast 155, 178); zero at block top 180h, node locals at wider scopes 178 fast; dead locals 1-8
// on the all-retail form 225h, on the zero-moved form 172h; masks as one-member structs, inline mask helpers 217-225h (still
// enregistered); loop-local detached mask copied out 201/180/218h; top loop named mask, named bit, word_flags_set 232/230/230h (zero
// moved) and 246h; piece node local 156, piece object pointer 157 (aligned 25 -> 15); `standard` 173 (diagnostic).
// Y610: Paired section-word/node-state-byte/creation-tag lvalue identities,
// subsets 1/2/4/3/5/6/7, then all with component-record identity:
// 156/156/156/156/156/156/156/156 (fast control 156).
// NOT kept: no exact identity-form match; original complete body restored.
// Round 157 (m3 lane 5): the mask's zero declared, or assigned, just before the first node loop 170/170 (aligned 107
// against 25: object_index loses ESI to the datum offset); the piece loop as `!= 0` 156 (aligned 24, retail's bottom
// jne), with a dword mask 156; the tag test in the loop condition, retail's guard order, 188 (776 bytes).
// T79: Mutable/immutable next-detached bitmap at node-record production, state-byte store,
// first metadata byte and final metadata word: eight 156s. NOT kept: no exact recurrence form;
// original complete body restored; late bitmap zero and piece/search entry remain.
// m3 lane 13 r77 (fast; masked aligned count in brackets, base [22]): both Havok component lookups through the existing
// havok_component_get (include/unknown_1cec30.h) 156 (same code); the piece object pointer with the piece loop as `!= 0` 157 [13]
// (the two forms together: the last lookups and the bottom jne become retail's); on that, retail's guard order
// (`detached_nodes > 0 && tag != NONE`) 188 (776 bytes, the mask enregistered), with a separate remaining mask for the piece
// loop 156 [11] (left: the zero's placement, the first loop's head pad, the node-count reload and the search's entry test), plus
// `long detached_nodes = 0;` declared in the block before the first loop 171 [86] (the head flips: the datum offset takes ESI);
// that declaration on the 157 form alone 163 [76]. (The 188 form is round 157's guard result again, here with the piece pointer.)
// Round 101 (m3 lane 16, fast): function-scope `long i` shared by the section, node and search loops with the zero declared
// after 0xba690 170 (same code as the late zero alone, re-measured 170); the node loop's two tests merged with node_bit at the
// top of the body 190; the search ending on `first_node == NONE && i < node_count` without the break 233 (779 bytes); the search
// with its own counter 156 (same code); `creation` declared at the mask block's top 156 (same code). Re-measured: the guard
// `detached > 0 && tag != NONE` with a do-while on `!= 0` 187 (777 bytes, the mask leaves memory). In retail the node loop's
// guard jumps to the block's end and the search has no entry test: node_count > 0 is known inside the piece loop. The piece
// definition index named once after the node loop (test and 0xb7930 argument) 160.
// m3 lane 11 r88 (fast): retail's mask and the datum offset share [esp + 0x10] (the mask's zero is stored after the offset's
// last use, with node_count's store, and the node loop's guard then jumps to the block's end); ours zeroes it early in its
// own slot. On retail's guard order (`detached > 0 && tag != NONE`, `while (!= 0)`, 187 at 777 bytes): `creation` or
// `first_node` declared at the guard's scope, the piece test as nested ifs, `~piece_nodes` named once for the mask update
// and the node mask, the piece's node index named 187 each (same code); with the piece pointer 188. On this body: the node
// mask stored right after 0xb7930 146 (790 bytes, but aligned 29 -> 66, masked 20 -> 40: the mask moves into ESI, xor-zeroed),
// with the section masks after it too 152 (791), with retail's guard 185 (779); the section masks before the node mask
// 154 (aligned 31); unknownb8 stored before unknownbc/c0 155; forward copied before up 156 (aligned 37); the piece test as
// nested ifs alone 156 (same code).
// m3 lane 2 r70 (fast, control 156, aligned 29, masked 20): the zero after the states call 154 (re-measure), with the piece
// loop as `!= 0` 154, with retail's guard order 226, with `!= 0` and the piece object pointer 155, guard and pointer 227, `!= 0`
// and pointer under `if (node_count > 0)` 157 (159 with the zero at the top; the search keeps its entry test). The mask update
// `detached_nodes &= ~piece_nodes;` after the 0xb7930 call 151 (aligned 36, masked 23; retail does it before the call), with the
// zero after the states call and `!= 0` 149. On that: the piece pointer 154, `!= 0` and pointer 154, zero after states `!= 0` and
// pointer 152; retail's guard order `detached > 0 && tag != NONE` + `while (!= 0)` 128 (813 bytes, masked 15: the mask stays in
// memory, so the guard, the pads and the search without an entry test agree), plus the piece pointer 127 (812, aligned 47, masked
// 13; left: the zero's placement, EBX/EDI swapped for node_states/node_count in the node loop, the `and` order). On 127 the zero
// after the states call 226/227, declared before the node loop 228/227 (230 without the moved update), at the mask block's top
// 228/227: every later zero shrinks the frame to 0x13dc (the datum offset stays in ESI). 127 is not kept (moved statement against
// retail's order, a guard repeating the loop test, a one-use pointer); it cannot become exact as written, since retail does the
// `and` before that call. What it shows: under retail's guard order the mask must stay in memory across the piece loop.
// m3 lane 1 r94 (fast, control 156, masked 20): the mask's zero declared at function scope above the attached mask 174;
// the node states declared before the zeroed mask 156 (same code); the zeroed mask declared before the component pointer
// 157; the state test as `if (node_states[i] == 0xff) continue;` 156 (same code); node_bit named above the state test 190
// (likely round 101's merged form again); `creation.node_mask = ~piece_nodes;` right after the mask update 191 (779 bytes).
// m3 lane 7 r155 (fast with 0xdb810 and 0xdbb40, control 156, aligned 29, masked 20): this body already keeps the mask in
// memory in retail's slot; retail's guard order (`detached > 0 && tag != NONE`) as an if around a do-while ending on `!= 0`
// 187 (aligned 54: the mask moves to ESI, `xor esi, esi` early, the frame 4 bytes smaller), with the mask declared after
// `states` 187 (same code); with the zero declared after the 0xba690 call 229 as a while or a do-while, and with it declared at
// the mask block's top 229.
// m3 lane 14 r92 (fast with 0xdb810 and 0xdbb40, control 156): the search as `for (i = 0; i < node_count &&
// !(mask & (1 << i)); i++) ;` with `first_node = i < node_count ? i : NONE` 146 (799 bytes, first +0xa: the whole
// head reallocates, positional only); the tag test as `if (tag == NONE) break;` inside the piece loop 188 (776 bytes);
// one function-scope `long i` for the section, node and search loops 156 (same code); piece_nodes as long 156. Kept:
// the object definition read through the typed header (model_tag_index, crate_object.tag_index) 156 (same code).
// @retail 0xdbc80
void function_dbc80(long object_index, short section_mask_a, short section_mask_b)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_object_definition *object_definition = (s_object_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);

	if (info)
	{
		long section_count = info->unknowne0;

		for (long i = 0; i < section_count; i++)
		{
			if ((1 << i) & (word)section_mask_a)
				object->unknowne0 |= 1 << i;
			if ((1 << i) & (word)section_mask_b)
				object->unknowne2 |= 1 << i;
		}
	}

	dword attached_nodes = function_db810(object_index, function_dbb40(object_index));

	if (object_definition->model_tag_index != NONE && object->node_index != NONE)
	{
		s_havok_component *component = (s_havok_component *)(g_51e9b8->data + (object->node_index & 0xffff) * 0xa0);
		long detached_nodes = 0;
		s_havok_node_states states;

		havok_component_node_states_get(component, &states);
		if (section_mask_a)
		{
			byte *node_states;
			long node_count;
			long node_records;
			long i;

			function_ba690(object_index, &node_count, &node_states, NULL, &node_records);

			for (i = 0; i < node_count; i++)
			{
				if (node_states[i] != 0xff)
				{
					dword node_bit = 1 << i;

					if (!(attached_nodes & node_bit))
					{
						byte *node = (byte *)node_records + i * 8;

						node_states[i] = 0xff;
						node[0] = 0xff;
						node[1] = 0;
						node[2] = 0;
						*(long *)(node + 4) = NONE;
						detached_nodes |= node_bit;
					}
				}
			}
			if (object_definition->crate_object.tag_index != NONE)
			{
				while (detached_nodes > 0)
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
					function_b7930(&creation, object_definition->crate_object.tag_index, object_index, 0);
					creation.unknownbc = *(long *)((byte *)object + 0x28);
					creation.unknownc0 = *(long *)((byte *)object + 0x2c);
					creation.unknownb8 = true;
					function_b9dd0(object_index, &creation.position);
					creation.up = *(vector3f *)((byte *)object + 0x7c);
					creation.forward = *(vector3f *)((byte *)object + 0x70);
					function_ba1d0(object_index, &creation.linear_velocity, &creation.angular_velocity);
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

		}
		function_1c3770(object_index, 0);
		havok_component_node_states_set(component, &states);
	}
}
