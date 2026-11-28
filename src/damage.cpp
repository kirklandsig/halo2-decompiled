// @flags /O2 /Ob1 /arch:SSE /Gr
/* DAMAGE.CPP: object damage */

#include "unknown_11c920.h"
#include "player_queries.h"
#include "bit_vector.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "effects.h"
#include "object_tag_definitions.h"
#include "units.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_1dee50.h"
#include "vector_products.h"
#include "bit_vector.h"
#include "unknown_1e1f20.h"
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
/* function_dbc80 stays declared ahead of function_dbfb0: link-time code generation walks 0xd6800's callees in
   this order, and 0x1c4040, 0x146bf0 and 0x1c51c0 match only with it (see 0x1c4040's comment). __stdcall is
   retail's convention for it: every caller pushes the three arguments and it returns with `ret 0xc` */
void __stdcall function_dbc80(long object_index, short section_mask_a, short section_mask_b);
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

/* the material word is stored through a pointer to its field: as a plain field store the NONE
   constant is scheduled ahead of that store (retail stores it first, then loads the bsp index) */
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
// M3 lane 2 r85 (plain): the +0x7c copy after the +0x22 store 12 (the bsp load hoists above the definition store);
// a two-byte memcpy from the global 3 (same code); the definition store last 17 (105 bytes, `mov eax, -1`); the +0x7c
// copy after the owner's three stores 7 (the global load then waits for those stores: it may alias); the owner in
// declaration order (player, object, team) 5. The reduced (--fast) image hoists the g_47d8e0 load above the
// definition store (not address-taken there), so judge this body only with the plain check.
// M3 lane 13 r98 (plain): the +0x1c/+0x20/+0x22 stores through an inline location-reset helper (leaf, cluster,
// bsp) 3 (same code); `short bsp_index = g_4686c4;` right after the +0x7c copy 3 (same code); the +0x7c copy moved
// down beside the +0x7e store 16 (the bsp load then hoists above the definition store); the location as an
// initialised `s_location` local copied in 46 (119 bytes). Diagnostics: one to seven unused `long` locals 3 each
// (no phase effect); the body through a mirror of the event type without the +4 byte/dword overlay 3 (same code).
// Round 168 (m3 lane 7, plain): unknown14 ahead of the owner's three stores 7; the three 1.0f stores ahead of the +0x7c
// copy 37 (119 bytes); the +0x84 clear ahead of the +0x7e store 5; the location's cluster cleared before its leaf 5; the
// +0x7e store ahead of the three 1.0f stores 7; the +0x7e store right after the owner's team 3 (same code: still stored
// last). Diagnostic (reverted): g_4686c4 read through a pointer to it (address taken) 9: its load then waits for every
// store ahead of +0x22, so retail does not treat it as aliased; its load sits right after the +0x7c store, held only by CX.
// M3 lane 14 r104 (fast with 0xd8cb0 in the set, which reproduces the full build here; a single-address fast image hoists the
// g_47d8e0 load above the definition store): unknown18 ahead of the +0x22 copy 5; unknown14 after the location stores 7; the
// owner's team stored first 6; unknown18 after the three 1.0f stores 7; the +0x22 copy ahead of the leaf and cluster stores 6. Each
// moves only the stores (they follow source order); `or eax, -1` stays right after the g_47d8e0 load.
// M3 lane 3 r273 (fast, and a one-object link-time screen that reproduces this listing): every NONE written `-1` 3; the word
// fields as `(short)NONE` 3; screen: `(short)0xffff`, `0xffff`, `-1` for the word fields, `0xffffffff` for the owner object, the
// memset size as 0x88, the +0x7c copy as a word copy, with a cast or through a `long` local, the definition index through a
// local, 3 each (same code). Diagnostic bodies with stores removed: one NONE store before the +0x22 copy and at most one after
// it gives retail's order; two before it, or three in all, gives ours, so the `or` takes the slot because eight stores use it.
// M3 lane 11 r126 (fast with 0xd8cb0): the owner copied from the default-owner pointer (`*g_467420`) 49 (121 B); the owner,
// unknown14 and the leaf/cluster NONEs as two chained assignments in store order 3 (same code); the material global read into
// a short local ahead of the memset 28 (105 B, the bsp load then hoists above the definition store); the same local read
// after the memset, ahead of the definition store 7 (the material takes EAX and `or` lands after the bsp load as retail,
// but the definition goes through ECX); the material global's extern declared just above this function 3 (same code).
// @retail 0xd6660
void function_d6660(s_type_1e6529 *data, long definition_index)
{
	memset(data, 0, sizeof(*data));
	data->definition_index = definition_index;
	short *material = &data->unknown7c;
	*material = g_47d8e0;
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

/* the squared distance between two points */
static inline real damage_distance_squared3d(point3f const *a, point3f const *b)
{
	real dx = a->x - b->x;
	real dy = a->y - b->y;
	real dz = a->z - b->z;

	return dz * dz + dy * dy + dx * dx;
}

/* damages everything in the damage's radius; returns the first player unit
   hit, else the last object hit */
// @retail 0xd6c80
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index)
{
	s_damage_definition *definition = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	real radius = MAX(definition->radius04, MAX(definition->radius58, definition->radius68));
	point3f const *position = &data->position;
	long objects[k_maximum_area_of_effect_objects];
	long type_mask = (definition->flags0c & 2) ? 3 : 0;
	long object_count = function_bb050(0, type_mask, &data->unknown1c, position, radius, objects,
		k_maximum_area_of_effect_objects);
	long last_object_index = NONE;
	long first_object_index = NONE;

	*(dword *)data->unknown04 |= 1;

	if (definition->player_radius > radius)
	{
		s_player_iterator iterator;

		iterator.data = g_4e8c24;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		while ((iterator.player = (s_player_datum *)data_iterator_next_inlined((s_record_pool_iterator *)&iterator.data)) != NULL)
		{
			if (iterator.player->unit_index == NONE)
				continue;

			s_damage_object *root = DAMAGE_OBJECT(damage_object_root(iterator.player->unit_index));

			if (object_count >= k_maximum_area_of_effect_objects)
				continue;

			real inner = root->field_xae9109 + radius;
			if (damage_distance_squared3d(&root->field_x221477, position) <= inner * inner)
				continue;

			real outer = definition->player_radius + root->field_xae9109;
			if (damage_distance_squared3d(&root->field_x221477, position) <= outer * outer)
				objects[object_count++] = iterator.player->unit_index;
		}
	}

	/* the guard repeats the loop's own test: with it the three object lookups below stay calls, as in retail */
	long i = 0;

	if (0 < object_count)
	{
		for (; i < object_count; i++)
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
bool function_d8b50(bool reset, real *recent_a, real *recent_b, char *timer, byte const *field_4_2)
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
// MiniMax B (11-19): no new form tried this round, no lower score (10): the two remaining differences are the owner
// copy's register rotation (ecx, edx, eax here, eax, ecx, edx in retail) and the flag store's schedule (this build
// stores the finished flags at [esp+0x1c] after the pushes, retail at [esp+0x14] before `push edi`); the earlier
// rounds' copy, cast and pointer forms and a permuter with annealing (66 tries) leave both alone. The flag chain
// written as three ternaries and the union's own dword member for the read, for the write and for both measure 10
// as well, so neither the copy's rotation nor the store's schedule follows from that spelling.
// MiniMax C (11-15): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the damage record's address is built where retail stores them before `push edi`; reading the flags before the owner copy measures 17 (worse); this round the owner copy written as a memcpy of the twelve bytes, as three member assignments (52), as three member assignments through a dword cast, as three long reads and writes through a pointer to the member, and the flags stored through the union's named dword member each measure 10 (unchanged), the first three of them because this build's twelve-byte copy always takes the three chunks through ecx, edx and eax, and a permuter run with annealing (66 tries, 1230 s) found nothing under 10.
// MiniMax G (11-14): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where
// retail uses eax, ecx, edx, and the finished flags are still stored after the record's address is built where retail
// stores them before `push edi`; a pointer to the record for the flags read and write, three dword copies of the
// owner, a conditional expression for the three flag bits and a permuter run with annealing (20 tries) leave the same ten.
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
// MiniMax D (11-14): no new form, no lower score (10): the owner's three dwords still copy through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the record's address is built.
// MiniMax D (11-15): no new form, no lower score (10): the owner's three dwords still copy through ecx, edx, eax where
// retail uses eax, ecx, edx, and the finished flags are still stored after the record's address is built; the copy's
// register order and the store's place are the allocator's and the scheduler's, and the matched 0xd9e70 copies the same
// owner through a third order.
// MiniMax I (11-15): no new form, no lower score (10): the owner's three dwords still go through ecx, edx and eax where retail uses eax, ecx and edx, and the finished flags are still stored after the record's address is built where retail stores them before the push; a memcpy of the owner and the field's dword member instead of the byte array alias both leave the ten. Retail's tail stores the flags into the record right after the four pushes and then forms the record's address into eax, while this build forms that address into ecx and stores the flags after it, so the two differences are the allocator's register order and the scheduler's store place.
// MiniMax R (11-16): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail
// uses eax, ecx, edx, and the finished flags are still stored after the record's address is built where retail stores them
// before the push; the register order and the store's place are the allocator's and the scheduler's, and reading and writing
// the flags through the union's flags member instead of the cast measures the same 10.
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
// MiniMax S (11-16): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail uses eax, ecx, edx, and the finished flags are still stored after the record's address is built where retail stores them before the push; the two are one cause, the late store keeps eax live across the address computation.
// MiniMax C (11-16): no new form, no lower score (10): the owner copy goes through ecx, edx and eax in retail and through eax, ecx and edx here, and retail keeps the event's flags in eax until the address of the event is pushed while this build stores the flags before the four pushes.
// MiniMax I (11-17): no new form, no lower score.
// MiniMax D (11-17): no new form, no lower score (10): the owner's three dwords still go through ecx, edx, eax where retail
// uses eax, ecx, edx and the flags store still sinks below the record's address computation; the union's flags member, a memcpy
// of the owner and a member-wise copy all leave the same ten (the last one worse), and a permuter run over the body (13 tries)
// found nothing below the ten.
// MiniMax R (11-17): no new form, no lower score (10): the owner's three dwords still copy through ecx, edx and eax where retail uses eax, ecx and edx, and the flags store still sinks below the record's address; moving the owner copy after the 1.0f store scores 17 and a cast destination still gives 10.
// Matched in a full build with stand-in SDK headers (was 10): the pointer at g_4e034c + 0x144 is held in a local that a
// const reference names before the definition index is read from it.
// @retail 0xdbfb0
void function_dbfb0(long object_index, s_damage_owner const *owner, bool a, bool b, bool c)
{
	if (TEST_FIELD_BIT(DAMAGE_OBJECT(object_index)->damage_flags.body_depleted))
		return;

	byte *pointer = *(byte **)((byte *)g_4e034c + 0x144);
	byte *const &kept = pointer;
	long definition_index = *(long *)(kept + 0x14);
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
// Round 172 lead 4 (match-only, not kept): only the reload order after function_d6a70 and function_176780 differs: retail reloads the spilled
// shield damage before the zero, ours the zero first (the function_d5bc0 order is now retail's).
// Round 168 (m3 lane 7, fast, each on the six-name set at 4): the scaled shield damage as its own local 25; the depletion
// block as two tests (`vit < 0` overflow block, then `vit <= 0 && !depleted`) 231 (874 bytes); the overflow block's zero
// store ahead of its test 29; the shield-damaged flag set before the function_176780 call 141 (880 bytes); the stun
// test's body-damage term ahead of the vitality zero test 12; the side-effect zeroing as a nested in-radius test 4 (same
// code). Diagnostics (reverted): probe-symbol chains at the top (k = 1..8: 4 4 4 7 8 8 8 8) and right after the shield
// default (k = 9/12/16/24: 8 each) never flip the zero/shield pair; on this set round 139's 1.0f clamp no longer flips it
// either (257); the function_176780 call's first zero as 1.0f 176.
// M3 lane 11 r160 (fast): the overflow as `-object->shield_vitality * maximum_shield` 120 (886 bytes), as `maximum_shield *
// (0.0f - vitality)` 8 (same code); the body damage stored ahead of the end lookup and the 0xb58c0 call 43 (883 bytes);
// the stun bounds into a NULL-initialised local tested after the type test 8 (same code); the vitality clamp as an
// if/else store 206 (885 bytes).
// M3 lane 5 r241 (H2M_FAST), at 8: the shield damage store as `shield_damage * inverse_maximum_shield` 8, the stun test
// as `threshold < shield_damage` 8, the multiplier test as nested ifs 8, the stun seconds as `stun_time * rate` 8 (all
// four the same code); the tail through `DAMAGE_OBJECT(object_index)` at both uses with no `object` refresh 10 (the
// reload orders stay); re-measured: the multiplier declared right after the category bool 206 (901 bytes).
// M3 lane 7 r228 (fast). Reading: the reload order after each call follows split-piece numbers: pieces are made in
// ascending candidate number and draw their numbers from a last-in-first-out pool of seven freed temporaries, then fresh
// ones, so a later-numbered value gets the lower piece number while the pool lasts. On the six-name set (round 172) body
// damage is numbered after the constants, which leaves 1.0f a pooled number (retail's function_d5bc0 order), but the
// scaled shield damage web is still numbered ahead of the zero constant, so the zero reloads first after function_d6a70
// and function_176780. Forms: the scaled damage written `multiplier * shield_damage` at the accumulator store and the
// stun test (no reassignment) 28 (the product takes the inverse's slot), on the six-name set 25, with the absorbed damage
// named 28, with the end lookup fresh too 28; the stun ticks through real_to_long 7 (re-measured), with the product form
// 27, on the six-name set 4, with both 25; the scaled damage as its own named local 28, with the clamped vitality and the
// fresh end lookup 28.
// M3 lane 4 r261 (fast screen, 8). The float split: on main the values are numbered 15-22 (inverse 17, maximum 18,
// shield 19, scaled shield 20, body 21) ahead of the constants (zero 24, 1.0f 25, 0.0001f 26); the pieces take freed
// numbers handed out 37, 35, 34, 31, 30, 29, 28, then new ones from 44, so after function_d5bc0 zero 28 < maximum 35 <
// inverse 37 < 1.0f 45, and after function_d6a70 and function_176780 zero 28 < scaled shield 31. Round 172's six names
// number body damage after the constants (zero 33/32, 1.0f 31: retail's function_d5bc0 order) but keep the scaled
// shield (piece 34) ahead of zero; retail needs the scaled shield numbered after the zero constant. Forms: the
// six-name set re-measured 4; on it `shield_damage *= multiplier` 4, the absorbed product's operands swapped 4, both
// 4; on main `shield_damage *= multiplier` 8 (same code), the clamped vitality named alone 8. All restored.
// M3 lane 2 r152 (H2M_FAST, 8): the inverse maximum as a zero default then a conditional reciprocal 150 (880 B); the
// in-radius flag read once into a bool for its three tests 222 (896 B); the six-name set re-measured 4, with the actor
// index declared inside the type test and tested in a nested if (no assignment in the condition) 4; on it the shield
// damage renamed, and the body and shield damage both renamed, 4/4 (names do not reorder the split pieces). All restored.
// M3 lane 5 r254 (reduced-image screen, control 8). Back-end trace: each float value that lives across a call is
// split and the pieces come back after the call in ascending piece number; the pieces are made in ascending candidate
// number and take the free numbers from the top down (37, 35, 34, 31, 30, 29, 28 here), then new ones. This body
// numbers the inverse 17, the maximum 18, the shield 19, the scaled shield 20, the body damage 21 and then the
// constants (zero 24, 1.0f 25, 0.0001f 26); retail's two orders need inverse, maximum, zero and then the scaled shield
// and 1.0f, so the scaled shield would have to be numbered after the zero constant, which no local is. Forms:
// everything after the scaling in a forced-inline helper taking `multiplier * shield_damage` as an argument
// (diagnostic) 28 (the parameter is numbered among the locals; the value moves to the inverse's stack home); the
// product unstored at the accumulator store and the stun test (the listed 28) is a temporary numbered after the
// constants, but its piece is made last and gets a new number, so the zero still comes back first; the store written
// `(multiplier * shield_damage) * inverse` 28, the product as `shield_damage * multiplier` at both places 28 / 28, a
// three-factor product either way round 184 / 184. Diagnostics: the unstored product with 1 to 12 dead probe locals 28
// each, 25 at 6 and 7 (function_d5bc0's order then retail's, the zero still first after the other two calls); this
// body with 6 probes ahead of the body damage (the listed 4): the body damage moves behind the constants (zero 22,
// 1.0f 23, 0.0001f 24, body 25) and the scaled shield keeps 20; the probes declared after the shield damage instead,
// 5 / 6 / 7 / 8 / 13 / 14 of them, 8 / 4 / 4 / 7 / 8 / 8 (the numbering follows the count of symbols, not their place).
// M3 lane 13 r167 (fast screen, control 8): a forced-ranking search on the six-name set (4) finds one single decision
// that halves it, a scheduling height step for one reload after function_d6a70 (4 -> 2), so each of the two shield/zero
// pairs is its own tie. Forms on main: the shield-damaged compare as `vitality < threshold` 8; the overflow sum as
// `overflow * (1.0f / multiplier) + body_damage` 8, as `(1.0f / multiplier) * overflow` 8; the scaled shield damage
// through a `real &` bound to the shield damage 8; the maximum as a `real const &` bound to the call result 8 (all the
// same code). All restored.
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

				if (!(permutation->skip_chance != 0.0f &&
					permutation->skip_chance > (real)random_next(&g_4e7408->unknown0) * (1.f / 65535.f)))
				{
					if (destroy)
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
						continue;
					}
					if (!blocked)
						continue;
				}
				state->destroyed_permutations |= 1 << i;
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

/* whether the damaged region exists in the info and kills outright */
static inline bool damage_region_is_lethal(s_damage_info const *info, long region_index)
{
	return region_index >= 0 && region_index < info->region_count && (info->regions[region_index].flags & 0x80);
}

/* damages an object's body: scales the damage by the body's material,
   applies region damage, takes it off the body and depletes the body when
   it runs out */
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

		void *entry = accumulator->unknown00;

		if (info && entry)
		{
			if (damage_region_is_lethal(info, *(short *)((byte *)entry + 6)) && (definition->flags & 2))
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
		if ((char)(info->flags & 0x80) != 0 || TEST_FIELD_BIT(object->damage_flags.unknown14))
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

void function_d0590(long object_index, s_unit_child_iterator *iterator);
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
// score 420 (was 420; 513 earlier): The 0x38 frame and 1868-byte extent agree. EBX and EBP still exchange region and permutation roles, and the datum displacement is reloaded.
// MiniMax I (11-17, run 103): no new form, no lower score (420): retail still holds the region index in EBP and the
// permutation pointer in EBX, where this build holds them the other way round, and retail reads the info argument
// after its first push where this build reads it before the pushes; reading the region index directly scores 510 and
// dropping the region local 558, so the body stands.
// MiniMax I (11-16): no new form, no lower score (513): the first difference is the schedule of the argument loads - retail pushes
// the saved registers first and reads the arguments from the shifted slots, this build reads one argument
// before the pushes.
// MiniMax C (11-20): no new form, no lower score (420): a base-pointer local for the region-state update scores 523 (reverted),
// retail keeps the permutation pointer in EBX and the region index in EBP where this build has EBP and EBX the other way round,
// and retail's destroyed-permutation mask is a 32-bit shift in EDI where ours is a 16-bit shift in ESI.
// MiniMax G (11-19): no new form tried this round, no lower score (420): the region index, the datum offset and the
// permutation are still in a cycle against retail's EBP/EBX/ESI, the region index is still read through its own
// address (the parameter stays on the stack as retail has it) and the first argument is still read before the pushes
// where retail reads it after.
// Earlier detail: Frame 0x38, permutation EAX and retail's 1868 bytes agree; first +0xc exchanges region EBX/
// permutation EBP against retail EBP/EBX; the constant-one home and the drop blocks' datum offset (retail ESI, with
// the pool global held in EDI) still differ. Earlier 513: 1884/1868 bytes and the constant-one home differed.
// Earlier header detail: 513 at m3 lane 12 r125 and r128, 517 before m3 lane 12 r125's drop-block helper, 476 before m3 lane 7 r155's region read through its address, 472 before r94's walk; frame 0x38 agrees; permutation remains on the stack instead of EAX,
// MiniMax G (11-12): no new form, no lower score (476); retail's prologue takes only the object index in
// EAX and reads the other five arguments off the stack with a 0x38 frame, ours takes the region index
// and the permutation pointer in registers, and that argument set comes from this function's own first
// pass, so it cannot be changed from here without a new parameter order.
// MiniMax A (11-14): no new form, no lower score (517): retail's entry takes only the permutation index in EAX
// and reads the region index once into EBP off the stack (`ret 0x14`), while this build hands the region index
// over in EBX and the permutation pointer in EBP; that register-argument set follows the parameter order.
// MiniMax D (11-14): no new form, no lower score (517): reading the region index directly instead of through a pointer to the parameter gives 476 here but 67 for 0xdaaa0, so the pointer form stays; retail still takes the permutation in EAX and five arguments on the stack (ret 0x14), where this build takes the permutation from the stack.
// Earlier 517 (was 517): 476 before m3 lane 7 r155's region read through its address, 472 before r94's walk; frame 0x38 agrees; permutation remains on the stack instead of EAX,
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
// m3 lane 7 r160 (fast with 0xdaaa0, 0xdae60 and 0xdb760, control 517): both drop values as long 511 (1868 bytes, re-measure);
// on that: the destroy block written in each arm of the destroy test 530 (1848 bytes); region and permutation pointers
// `const` 511 (same code); the drop and 0x20000 blocks reading the pool at each use 523 (1900 bytes); a local copy of the
// pool for them 523 (1900: the local is rematerialised from g_4e0300 after each drop call, and the datum offset then takes
// EBX with the datum address CSE'd into EDI; retail keeps g_4e0300 in EDI, the offset in ESI and re-reads `data` per use).
// m3 lane 12 r125 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd5de0, 0xd9d60; control 517): kept, both drop blocks' unit
// lookup through the existing unit_get_current_weapon (unknown_1e1f20.h) in place of its body written out 513 (plain;
// the second block re-reads the pool's data after 0x100390 as retail). The region-state update through word_flags_set
// 517 (same code); the drop and 0x20000 blocks reading the pool at each use re-measured 543 plain and fast (1896 bytes),
// with the helper 543; `permutation` declared first and assigned after the region 517 (same code); the object looked
// up after the region and permutation pointers 561. No address of g_4e0300 is taken in src/ and 0x100390 makes no
// calls, so the pool global's reload after the drop calls is register choice, not LTCG mod/ref.
// The seat block is 0xe68c0 (a zeroed request of type 0x1e through 0xe6900, then the player tail 0x1e77c0) inlined in
// retail ([0x467840] is the request table's entry 0x1e): the call `function_e68c0(0x1e, unit)` 673 (1712 bytes, left
// out of line here); its body written out (memset, type, 0xe6900) 531 (1884 bytes, inlined, the block then retail's
// code but for registers; not kept, a helper's body written out).
// m3 lane 12 r128 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd5de0, 0xd9d60; control 513, `ret 0x14` as retail, first difference
// +0xc where the region index takes EBX and the permutation pointer EBP): 0xa8360 given `*region_reference` 513 (same code); the
// region-state update through a state pointer formed at the update 513 (same code); the destroy test as
// `!(flags & 1 ? db110(..) : !(flags & 0x10000) || state == 1 && db110(..))` 522 (1880 bytes); a `survives` flag (true, set from
// db110 in the flags-1 arm and `state == 1 && db110(..)` in the 0x10000 arm, tested once) 529 at 1852 bytes, which gives retail's
// shared 0xdb110 call exactly (`push 0; push 0; jmp` into `push; call; test al, al; jne`), not kept (a flag set in the arms); the
// second arm as a nested ternary ending `: false` 513 (same code). Retail spills a constant 1 into the region argument's dead
// home ([esp + 0x58]) for the region-state shift and later holds 1 in ESI (`or word ptr [x + 0x352], si`, the state compare).
// P579: Actual region/permutation/object receiver role names, seven subsets, then the
// existing region-argument reference renamed as well: 513/513/513/513/513/513/513/513. NOT kept: no exact native
// region/permutation register recovery; original complete body and all observations restored.
// MiniMax D (11-16): no new form, no lower score (513).
// m3 lane 8 r176 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd5de0, 0xd9d60; control 513): retail gives the permutation pointer EBX
// throughout, reloads object_index into EBP block by block and gives ESI to the datum offset (later the constant 1); ours holds
// object_index in ESI and the permutation pointer in EBP. `(void)&region_index;` for the reference pointer 533 (the convention
// changes: 0xdae60 48, 0xdb760 9, 0xdaaa0 67); object_index read once through its address 513 (same code); a named
// `object_index & 0xffff` for all six lookups 571 (1896 B); all six vehicle bits as word ORs 513 (same code); region and
// permutation declared ahead of the object 513.
// m3 lane 1 r115 (fast with 0xdaaa0, 0xdae60, 0xdb760; kept form confirmed plain): kept together, 513 -> 420 at retail's 1868
// bytes (register-masked 142 -> 49): both drop values as `long barrel_index` (the callees' parameter type), the destroy block
// written in each arm of the destroy test (retail's one cross-jumped 0xdb110 call), and the drop, 0x20000 and vehicle blocks
// reading the datum's type through the pool at each use (retail keeps the pool global in EDI and re-reads its data per
// block; the vehicle through DAMAGE_OBJECT, the same code as a datum pointer there). Alone: long drops 544 (1896 B, masked
// 87), per-use reads 543 (1896 B, masked 88); long drops + per arm 528 (1852 B, masked 64). Leave-one-out on 420: without
// long drops 528 (1852 B), without the arms 523 (1900 B), without the per-use reads 528. Same score, not kept (lead): the
// section and seat name scans index-based (`info->unknowne4 + i * 0x14 + 4`, `seats.address + seat_index * 0xb0 + 4`): each
// scan becomes retail's code exactly (retail reads the array pointer after the count guard), 513 on the old body (masked 142
// -> 140 each, 138 both) and 420 on the kept one (masked 51 -> 49).
// Round 96 (m3 lane 2, fast with 0xdaaa0, 0xdae60 and 0xdb760; control 420, register/stack/global-masked aligned 58): the
// section name read from the permutation at each use (no local) 420 (same code); the section kind read at each use 425;
// the region-state update indexed through `*region_reference` 420, with 0xa8360 given it as well 420 (same code); both
// name scans index-based with the reference-free section test 420 (masked 54). Reading: retail's ESI holds the datum
// offset from the join after the mode-4 block (all four callee-saved registers busy, so the shift's 1 is spilled into
// the region argument's dead home), ours holds object_index in ESI and leaves EDI free, so our constant 1 goes to EDI
// from the 0x20000 test on (`push edi`, `or [x + 0x30], edi`) where retail uses immediates there and ESI = 1 only from
// the vehicle block to the state compare.
// One type byte per drop/effect 420; one shifted type mask 421; shared vehicle datum 420;
// one datum per drop/effect 479; unsigned 16-bit slot for datum reads 549; shared weapon-slot
// lookup 420; one attachment index per notification 420; one flags read per drop 420.
// NOT kept: no gain and the region/permutation register assignments still differ.
// Round 636: region and permutation receivers remain exchanged. Pool retained through drops 460, through
// effects 460, through vehicle 481; retained from entry 451, from region update 513; pool and 16-bit
// slot through drops and vehicle 555 or from entry 566; pool retained for both early notifications 420.
// Round 640: region lvalue, permutation lvalue, both, or both read-only 420/420/420/420.
// Region word mask/lvalue/shared setter 533/420/420. Typed seat label cursor and
// index 419/420. NOT kept: the cursor gains one and the register assignments still differ.
// Attachment index read once for each notification with a shared local 420.
// Attachment observations using separate values 420.
// Attachment observations using separate immutable values 420.
// Attachment observations using references to each attachment field 494.
// Attachment observations using separate read-only object pointers 416.
// Attachment observations using separate read-only object references 416.
// Loop over the two notification bits with a separate read for each guard and call 792.
// Loop over the two notification bits with one attachment index per iteration 792.
// NOT kept: the separate object pointers and references gain four points at 1868 bytes, below
// the eight-point threshold. Both put the second notification's pool and pointer loads in retail's
// order, but the region index and permutation receiver still use exchanged registers.
// m3 lane 1 r130 (fast with 0xdaaa0, 0xdae60 and 0xdb760; control 420, aligned 200/200 instructions unmatched ours/retail,
// 38/38 with registers and stack offsets masked; body unchanged). Reading: three values sit in a cycle against retail
// (object_index ESI / datum offset EBX / permutation EBP here; EBP / ESI / EBX in retail), so the register freed when the
// region index dies is EBX here and EBP there, and the constant 1 follows it. Tried: the seat-eject bit in its own `if`
// ahead of the seat-name test 420 (same code); the first attachment notification read through `object` 540 (1852 B); the
// region bit in a named word 533 (1852 B); the region-state update indexed through `*region_reference` 420 (same code);
// the section scan index-based 420 (aligned 193/193, masked 37/37), the seat scan index-based 420 (199/199), both 420
// (192/192, masked 36/36: each scan then reads its array after the count guard as retail, the score does not move); the
// player block pointer named ahead of the controller read 420 (same code), with both scans 420; one to eight unused
// locals 420 each (not a symbol-count tie).
// m3 lane 12 r156 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd5de0, 0xd9d60; control 420): the permutation pointer read
// through a const reference bound to it 420 (same code); the object pointer the same way 420; the region pointer the
// same way 420; the region index read once through its address into a const local used for the permutation lookup,
// 0xa8360 and the region-state update (no region pointer) 558 (1852 B, the dead add gone); the region array named
// before the region pointer 425. The cycle (region index EBX, permutation pointer EBP) does not move.
// m3 lane 5 r217 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd5de0, 0xd9d60, none moved; control 420, aligned 217 rows by
// difflib, 54 with registers masked). LEAD, not kept (a one-use pointer local, and the positional score rises): the
// object's region states named first, as lines 4524 to 4908 of this file do, `s_object_region_state *region_states =
// (...)((byte *)object + object->region_states_offset); region_states[region_index].destroyed_permutations |= 1 <<
// permutation_index;` 523 at 1852 B, but aligned 128 (masked 52) and first difference +0x5a for +0xc: the entry cycle
// is gone (region index EBP, permutation pointer EBX as retail) and the update is retail's `or word [eax + ebp*8],
// reg` in place of the `lea` and plain `or`. Left on it: ours keeps the object pointer in ESI from the update on and
// reloads the datum offset into scratch registers, retail keeps the datum offset in ESI (first loaded at the second
// attachment lookup) with the pool pointer in EDI and reloads the object pointer from its home into EDX or EAX at
// each use, which is the 16 bytes; the shift's 1 then has no free register in retail and goes through the region
// argument's dead home. On the lead: the object declared after the permutation pointer 565 (aligned 154, entry
// exchanged again); a const object pointer 523 (same code); the bit named ahead of the update 521 (aligned 128).
// Without the named states: the element address named (`&states[region_index]`, then `->destroyed |= bit`) 420
// (same code as the body); the shifted bit cast to a word 420 (same code); no region local (one expression for
// the permutation pointer) 558 (1852 B, aligned 210); the object declared after the permutation pointer 462 (1868
// B, aligned 240, first difference +0x4).
// m3 lane 4 r235 (fast; aligned by difflib / registers and frame offsets masked in brackets, control 420 [195 / 33]):
// the region index as a four-byte wrapper aggregate passed by value, replacing the read through its address, 420, the
// same code here (an aggregate parameter stays on the stack exactly as the address-taken long does), while the three
// matched callers lose their matches to the conversion temporaries (0xdaaa0 135, 0xdae60 113, 0xdb760 32): restored.
// What retail's middle shows: the datum offset (index * 12, home +0x14) holds ESI from the second attachment lookup on
// and the pool pointer EDI from the first weapon block on, each use re-reading the pool's data (`mov r, [edi + 0x44]`
// before every `[r + esi + 3]` / `[r + esi + 8]`), while the object pointer stays in its home and is loaded into EDX,
// EAX or EBP per region; the shift's 1 then starts in ECX and is exchanged with the count through the region
// argument's dead home, which the damage-source byte reuses later. Here the object pointer takes ESI and each weapon
// block forms the header address in EDI. The two webs are near a tie (about 18 offset uses against 17 object uses), so
// single uses flip it: on the named-region-states lead (re-measured 523 at 1852 B [109 / 34]) the four flag-byte
// stores re-looked up through the pool 505 at 1884 B [87 / 32], the same without the lead 505 [87 / 33] (the offset
// takes ESI, but retail reads the object's home there, 16 bytes over); the first vitality store re-looked up 461 at
// 1868 B [113 / 35]; the section switch's four section words 565 at 1912 B [146 / 50]; the seat-name block's type and
// tag 521 [144 / 42]; the child walk's start 481 at 1888 B [135 / 43]. None kept (positional rises, and retail reads
// the home at each of those sites).
// m3 lane 6 r221 (H2M fast with 0xdaaa0/0xdae60/0xdb760, all MATCH; control 420): both attachment notifications as a
// file-local forceinline helper taking the object index and the mask 420; the mode block as a forceinline helper taking
// the three indexes 420 (both the same code: the inlined parameter copies fold); per-notification named object pointers
// 416 (listed), on the named-region-states lead 519 (1852 B; the lead re-measured 523). Reading: the notifications'
// address operand order differs too (`[edx + ecx + 8]` here, `[ecx + edx + 8]` in retail): retail's offset there is an
// allocated piece of its ESI web, ours a spill reload.
// M3 lane 14 r138 (fast with 0xdaaa0, 0xdae60, 0xdb760, all MATCH; control 420, `ret 0x14` in both builds): on the
// current body the region-state update through two DAMAGE_OBJECT lookups 478 (1852 B); both destroy arms' vitality
// stores through DAMAGE_OBJECT 593 (1932 B); the child walk's start through DAMAGE_OBJECT 493 (1904 B); the damage-source
// byte passed as the expression (no local) 464; the at-marker flag passed as the expression 420 (same code).
// Shared short or long datum slot 549/571, existing weapon-in-slot helper 420, type mask once per drop 420,
// helper with short or long slot 531/495, short slot with type mask 549, helper with type mask 420, and retained
// pool with long slot and helper 526. NOT kept: the short slot restores the entry register assignment but
// changes the index load and grows to 1880 bytes. The other forms tie or raise the score.
// Conditional receiver test on the first drop, second drop, or both 536/537/519. Receiver selected before
// its test on the first, second, or both 519/527/577. Named unit classification 602, supported-type guard 581.
// NOT kept: The joined receivers can restore the entry register assignment through +0x5a, but change the
// drop control flow and move the length away from 1868 bytes.
// m3 lane 4 r248 (fast with 0xdaaa0, 0xdae60, 0xdb760, all MATCH; control 420, aligned 200): the region-state update as
// pointer arithmetic (`(states + region_index)->`, as the file's other region-state users write it) 420; the two
// notifications through a named attached index 420; the vehicle pointer through the pool cast 420 (all the same code);
// the section-name search indexing the sections (`info->unknowne4 + i * 0x14 + 4`) with no cursor pointer 420 but
// aligned 193: the search loop is then retail's (count ESI, cursor EDX set up after the guard) but for the EBX/EBP
// exchange of the permutation and object pointers, which runs through the whole body; with the pointer-arithmetic
// update or the named attached index 420 (aligned 193).
// M3 lane 2 r138 (h2m fast with 0xdaaa0, 0xdae60, 0xdb760, all MATCH; control 420, aligned 195, registers masked 34):
// main reads the region index through its address once and directly twice; all three uses through the pointer 420,
// only the 0xa8360 argument through it 420, only the state update through it 420, the parameter bound to a
// `long const &` for the lookup 420 and for all three uses 420 (all the same code). Body unchanged.
// M3 lane 2 r140 (h2m fast with 0xdaaa0, 0xdae60, 0xdb760, all MATCH; L = the named region-states lead, re-measured 523
// at 1852 B, aligned 128): on L both notifications through one named attached index read again for the second 523;
// the mode-4 block's `current` as the function's object pointer 539 (1836 B); the 0xa8360 argument through the region
// reference 523; the region lookup reading the parameter directly 523 (0xdaaa0 stays matched on L); no region local
// 560. Paren pairs around L's shifted bit 523 (inert). Retail's second notification lookup starts the datum offset's
// ESI web (the mode-4 path loads it at the join) and its later blocks keep the constant 1 in a register. Body unchanged.
// m3 lane 4 r251 (H2M_FAST with 0xdaaa0, 0xdae60 and 0xdb760, all MATCH; control 420): the region states named at entry
// (after the permutation pointer) and indexed at the update 472 (1880 B); the child walk as a while loop 420 (same code);
// the mode-4 block through `object` 424; the four flag-byte stores through a pointer to the byte 420 (same code); the
// child-name test written name first as the section and seat tests are 421. Reading: retail loads the datum offset
// into ESI at the second attachment lookup and keeps it (the shift's 1 then goes through the dead region home into
// EDI); ours gives ESI to object_index from the 0xa8360 call on.
// Object table before region selection 550; early named object slot 626;
// both table bases before their rows 577; explicit dword slot after both
// bases 625; complete datum type 577; dword or word slot through the
// notifications 578/551; dword slot with one read per attachment 578.
// NOT kept: the entry lookup variants still displace the native reads and
// some change permutation ingress. Typed seat label, indexed scan 420.
// Separate permutation-array read and record address at entry, with region or direct table 439/556.
// Record address after notifications 627 in all three forms. Record address after the region-state
// write, with region, direct or immutable array pointer 529/541/541. NOT kept: the first late form
// loads the region index into EBP and keeps frame 0x38, but adds 12 bytes and raises the score.
// M3 lane 8 r235 (H2M_FAST with 0xdaaa0, 0xdae60, 0xdb760, all MATCH; control 420). Allocator reading: the permutation
// pointer (priority 66) is coloured before the region index (58) and takes EBP, leaving EBX to the index; a forced
// priority of 70 for the index gives retail's entry (first difference +0xc -> +0x59, semantic rows 110 -> 46), so the
// index needs one more weighted reference in its range (entry to the region-state update) or the permutation fewer.
// Forms: the index bound to a `long const &` for the lookup 420, for all three uses 420; the two notifications through
// one attached index read again for the second 420 (all the same code); the region states named at the update 523
// (1852 B); the vehicle block through the entry object pointer 528 (1884 B).
// Round 479: Region ordinal and permutation record still exchange EBX/EBP.
// Named section count with sentinel 420; bounded section index 450; bound-and-name section loop 450;
// selecting the result after the loop 448; bound-and-label seat loop 420; both loop conditions 450;
// word section kind 425; direct nonzero kind ranges 462. Body restored.
// Byte displacement for the typed datum lookups 511; with the slot helper 419, retained pool 459,
// or shared vehicle datum 511. Helper/pool 419, helper/vehicle 419, pool/vehicle 459, all 419.
// NOT kept: the one-point gain leaves the exchanged region and permutation registers.
// The region ordinal and permutation record still exchange EBX and EBP.
// first drop flag snapshot 420; second drop flag snapshot 420.
// independent drop flag snapshots 420; deferred destruction admission 391.
// selected destruction result 499; positive preservation result 409.
// shared vehicle flag setters 420; shared setters and deferred destruction admission 391.
// NOT kept: the 391 form emits a combined mask and a dynamic stack argument. Retail tests two flag bits separately and pushes literal mode values.
// Lane GA round 2 (fast with 0xdaaa0/0xdae60/0xdb760, all MATCH; control 420 at 1868 B, aligned 267, masked 57):
// on the named region-states lead (L = 523 at 1852 B, +0x5a, aligned 126, masked 54): region-states returned by a
// forceinline helper 420 (entry cycle returns), element-pointer helper 420, vehicle block through a datum pointer
// 523 (aligned 107), notifications through a forceinline helper 523, drop blocks through a forceinline helper 521
// (aligned 139), seat event block through an inline helper 706 (1680 B, outlined), unit weapon lookup through
// DAMAGE_OBJECT 523, mark-region-destroyed inline helper 523; single-body destroy test on L via ternary 524 (1884 B),
// via `||` 513 (1868 B, +0xc), ternary with vehicle datum and indexed scans 495 (1884 B), diagnostic `kept` flag
// alone/with vehicle datum 521/523 (1852 B; without L 433 at 1868 B), byte_flags_set for the four +0x19 stores 523,
// word_flags_set for the region-state update 523, entry datum local 523. Diagnostic type-mask (`!= 0`) and null-checked
// vehicle-getter forceinline helpers on baseline/L: 374/463 at 1868 B (+0x5a, masked 72/71: the extra `shr/test 1` and
// null check lengthen permutation's range enough to flip region_index into EBP and pad 1852 to 1868, not retail's code);
// with `kept` and indexed scans 468. On every clean form `object` still beats `datum_offset` for ESI from 0xda1ca on,
// keeping the 16-byte gap and preventing g_4e0300 from holding EDI across the drop blocks. Body restored.
// m3 lane 6 r243 (allocator reading): the permutation pointer (priority 66) colours before the region index (58: one
// reference each at the entry, the 0xa8360 push and the update) and avoids EBX, so the index takes EBX; the named
// region states give the index a second reference at the update (74) and retail's entry. On that lead `object`'s later
// web (32) takes ESI as the only register left, where retail keeps the datum offset there (each type read and each
// inlined weapon lookup addresses the pool data plus the offset directly, the data pointer re-read from the pool global
// held in EDI) and reloads `object` from its slot into EDX at the flag stores. Forms: the datum type read once per drop
// block into a `long type` for both tests 419 (1868 B); the same on the region-states lead 521 (1852 B); the lead with
// the seat block's type and tag read through the lookup 429 (1868 B, the entry exchange returns); diagnostic, the lead
// with the four flag-byte stores through the lookup (a second path, match-only) 505 (1884 B, first +0x32): the datum
// offset then takes ESI from the second notification on as in retail; left there: the pool global in EDI across the
// drop blocks (ours still forms one data-plus-offset sum per block), the constant 1 spilled into the region slot at
// the update, the vehicle block's constant 1 in ESI, the section scan's count/cursor pair, and scratch rotations in the
// seat and event blocks.
// M3 lane GB round 13 (24 trials, fast with 0xdaaa0/0xdae60/0xdb760 all MATCH; control 420 at 1868 B, al=216, mk=51):
// on the named region-states lead (L = 523, 1852 B, +0x5a, al=126, mk=50), dropping the &region_index reference keeps
// ret 0x14, frame 0x38, +0x5a, and all three matched callers MATCH (523, identical code — L needs no address-taken
// hack); + existing function_e6900 inlined for the seat event 523 (al=119, mk=47; on baseline 442, mk=50), + indexed
// section/seat scans 523 (al=109, mk=43: last 193 instructions structurally identical to retail; on baseline 442, mk=46),
// + existing function_cbd50 (inlined via LTCG) and vehicle datum pointer 523 (al=90, mk=44; cbd50 alone on baseline/L
// 420/523, via object->current_weapon_index on L 509 at 1868 B, +0x32, mk=59; via unit_weapon_view_get 523); L +
// object_flags byte pointer 523, + inline damage_object_type 501 (1868 B, mk=74), + single || destroy body 513 (1868 B,
// +0xc), + entry datum local 523, unified s_damage_object_datum on baseline/L 420/523, L + cbd50 + per-notification
// const pointers 519 (al=124), diag pool local in drop blocks 523. On every clean form object still beats datum_offset
// for ESI from 0xda1ca, keeping the 16-byte gap and lea [data+offset] in EDI across the drop blocks. Body unchanged.
// The region and permutation registers remain exchanged. Short displacement,
// long displacement, combined byte offset and staged byte offset score
// 420/420/429/420. With a named full-width destruction bit, each scores 528.
// NOT kept: the inline shifts tie or regress. The named bit shortens the
// body by 16 bytes and leaves the same entry difference.
// M3 lane 7 r231 (H2M_FAST with 0xdaaa0, 0xdae60, 0xdb760, 0xd9d60, all MATCH, and 0xd5de0; control 420). Allocator
// reading of the first difference: the permutation pointer ranks 66 (four entry-block refs +136, -308 over 83 blocks it
// is live but unused) and the region index 58 (entry +34, the 0xa8360 block +16, the state update +16, -8 over the four
// call blocks between), so the pointer colours first and takes EBP; with the region index at 66 or more (it wins the
// tie) it would take EBP and leave EBX to the pointer, as retail. Forced that way: 516 at semantic distance 48 (from
// 110), next difference retail's datum offset held in ESI from the mode block's join. Forms: the attachment re-read
// into a named two-use local in each 0xb58c0 block 420 (same code); the permutation pointer in one expression 558 (1852
// B); the reference read moved to the 0xa8360 argument 420 (same code); the state update as `x = x | bit` 420 and OR-ing
// `(word)(1 << permutation_index)` 420 (same code each). Body unchanged.
// M3 lane 5 r250 (fast with 0xdaaa0, 0xdae60, 0xdb760, 0xd9d60; control 420): the permutation pointer defined after the
// mode block 573 (first +0x3); the permutation pointer in one expression without the region pointer re-measured 558; the
// two ejection tests in the other order 420 (same code); the mode block's attachment reads through `object` 559; the
// marker flag passed as an expression instead of a named bool 420 (same code). Body unchanged.
// M3 lane 6 r249 (fast with 0xdaaa0, 0xdae60, 0xdb760 MATCH unless said; control 420). Allocator ledger re-read: the
// region index 58 = entry block +34 (one reference among 17 values, the owner and accumulator parameters and the
// constants 0, NONE, 0xffff and 4 among them), the 0xa8360 block +16 (8 values), the state update +16 (8 values), -8;
// the permutation pointer 66 = entry +136 (four references), +16 at the update, -308 over 83 blocks. The index passes
// it (the tie goes to the index) with three more values referenced in the 0xa8360 block, or two fewer in the entry
// block; the update block adds to both alike. Forms: the state update as pointer arithmetic (`+ region_index` then
// `->`) 420; the region and permutation pointers as `+` arithmetic 420; the permutation pointer to const 420; the
// update through the existing word_flags_set 420 (same code each); the parameter read directly for the region (no
// pointer to it) 510 at 1868 B, with 0xdaaa0 49, 0xdae60 48, 0xdb760 9 (the convention changes). Body unchanged.
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

			function_d0590(object_index, &iterator);
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

	if (permutation->flags & 0x60)
	{
		long barrel_index = !((permutation->flags >> 5) & 1);

		if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 3)
		{
			long weapon_index = unit_get_current_weapon(object_index);

			if (weapon_index != NONE)
				function_100390(weapon_index, barrel_index);
		}
		else if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 4)
		{
			function_100390(object_index, barrel_index);
		}
	}
	if (permutation->flags & 0x300)
	{
		long barrel_index = !((permutation->flags >> 8) & 1);

		if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 3)
		{
			long weapon_index = unit_get_current_weapon(object_index);

			if (weapon_index != NONE)
				function_1003e0(weapon_index, barrel_index);
		}
		else if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 4)
		{
			function_1003e0(object_index, barrel_index);
		}
	}
	if (permutation->flags & 0x20000)
	{
		if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 0x1c)
			function_10d4e0(object_index);
		else if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 0x20)
			function_fc330(object_index, false, 0.0f);
	}

	if ((1 << ((s_damage_object_datum *)g_4e0300->data)[object_index & 0xffff].type) & 2)
	{
		byte *vehicle = (byte *)DAMAGE_OBJECT(object_index);

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

	if (permutation->flags & 1)
	{
		if (!function_db110(object_index, false, false))
		{
			object->body_vitality = 0.0f;
			function_d6800(object_index, owner, true, false);
			accumulator->flags |= 1;
		}
	}
	else if (permutation->flags & 0x10000)
	{
		if (g_4e6948->state != 1 || !function_db110(object_index, true, false))
		{
			object->body_vitality = 0.0f;
			function_d6800(object_index, owner, true, false);
			accumulator->flags |= 1;
		}
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
		s_unit_seat_definition const *seats = (s_unit_seat_definition const *)definition->unit.seats.address;

		for (seat_index = 0; seat_index < seat_count; ++seat_index)
		{
			if (seats[seat_index].label == seat_name)
				break;
		}
		if (seat_index != seat_count)
		{
			s_unit_child_iterator iterator;

			function_d0590(object_index, &iterator);
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
// score 5 (was 5): only the order of the two cold exits differs.
// MiniMax B (11-20): no new form, no lower score (5): the two cold exits keep this build's order (the stack reload
// before the xmm1 copy) where retail copies xmm1 first and lets the stack reload fall into the shared epilogue; the
// no-team exit written as an early return (`team == NONE || state != 1`) measures 5 and lays the blocks out the same
// way, so the source order of the two returns does not decide the block order.
// MiniMax B (11-19): no new form kept, no lower score (5): this round tried the two early-return spellings of the
// team test - the team test as an early return, and both tests as early returns with a braced enemy block - and both
// compile to the same bytes and leave the same order of the two cold exits, so that order is the scheduler's and not
// the source's; four more spellings measured the same way - the not-enemy return hoisted ahead of the enemy path (60),
// the outer test inverted with an else (5), the range test with an else arm that reaches the enemy path twice (70) and
// the inner damage return dropped so the two exits merge (24) - and a permuter run with annealing over the body (127
// tries in 1206 s, no confirming build) found nothing below the five either. The aligned listing pins the whole
// difference on the block layout: the frame (8 bytes, damage at [esp+8], the flag byte at [esp+7]), the values in both
// cold blocks (the xmm1 copy and the stack reload) and the bool store already agree, and what is left is three branch
// displacements plus the pair of cold return blocks in the other order (this build lays the stack reload out first,
// retail the xmm1 copy); the file was restored to the body that scores 5.
// MiniMax C (11-15): no new form, no lower score (5): the two cold exits keep this build's order (the stack reload before the xmm1 copy) where retail copies first, and inverting the two exits in the source compiles to the same bytes.
// MiniMax G (11-14): no new form, no lower score (5): the two cold exits keep this build's order (the stack reload
// before the xmm1 copy) where retail copies first; the no-team case as an early return, the team test as an early
// return, the no-team return in an else branch, a goto to a separate not-enemy return, the enemy logic inside the
// test with the no-team return in an else (which merges the two exits, 24) and a nested state test with a return for
// each of the team, state and not-enemy cases (505 bytes, same order) all leave the two blocks alone or make them
// worse; the no-team return first with the not-enemy return last compiles to the same bytes (the early return is
// folded back into the outer test) and two separate early returns put the not-enemy exit last but split the no-team
// exit in two (28); a permuter run with annealing (31 tries) found nothing under 5.
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
// MiniMax D (11-14): no new form, no lower score (5): the two cold exits keep our order (the stack reload before the xmm1 copy) where retail copies xmm1 first; a permuter run over this body (99 tries, 900 s) found nothing below 5.
// MiniMax D (11-15): no new form, no lower score (5): the two cold exits still keep this build's order (the stack reload
// before the xmm1 copy) where retail copies xmm1 first, and the early-return form of the team test compiles to the same
// bytes and the same order, so the two blocks' physical order is the scheduler's.
// MiniMax I (11-15): no new form, no lower score (5): the two cold exits still keep this build's order (the stack reload before the xmm1 copy) where retail copies xmm1 first; a permuter run with annealing over this body (67 tries, 603 s) found nothing below 5, and an early return for the no-team case, two separate early returns and an if/else that creates the early exit first all leave the order alone.
// MiniMax R (11-16): no new form, no lower score (5): only the order of the two final "return damage" blocks still differs
// (retail puts the no-team return before the not-enemy one, this build the reverse); the early-return form of the team test
// compiles to the same bytes and the same order, so the two blocks' physical order is the scheduler's.
// MiniMax E (11-20): no new form, no lower score (5): the two cold exits keep this build's order (the stack reload
// before the xmm1 copy) where retail copies xmm1 first; the outer return written as an early return for the team test
// compiles to the same bytes and the same order.
// Earlier (5, was 108): only the order of the two final "return damage" blocks differs (retail
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
// MiniMax S (11-16): no new form, no lower score (5): only the order of the two cold exits differs, and an early return that leaves the state test in place compiles to the same five, so the source order does not decide the block order; dropping the inner damage return merges the two exits (24), inverting the enemy test flips the branch (16) and writing the block as an if/else changes nothing (5).
// MiniMax C (11-16): no new form, no lower score (5): the two cold returns hold the same values on both sides, but retail lays the outer return out before the not-enemy return and this build lays the not-enemy one first; an explicit else on the enemy test compiles to the same order.
// MiniMax I (11-17): no new form, no lower score.
// MiniMax D (11-17): no new form, no lower score (5): the two cold returns still hold the same values on both sides, but retail
// lays the outer return (xmm1) out before the not-enemy return (the stack slot) where this build lays the not-enemy one first;
// the source order of the two returns does not decide it (an early return, an explicit else and a merged tail all keep the
// same order or score worse), so the difference is the compiler's block layout; a permuter run over this body
// (30 tries, 305 s) found nothing below the five.
// MiniMax R (11-17): no new form, no lower score (5): the two cold returns still hold this build's order (the stack reload before the xmm1 copy) where retail copies xmm1 first, and the source order of the two returns does not decide it (an early return, an explicit else and a merged tail all keep the order or score worse).
// MiniMax E (11-19): no new form, no lower score (5): the two cold exits keep this build's order (the stack reload
// before the xmm1 copy) where retail copies xmm1 first; four return shapes were measured (an early return for the
// not-enemy case 16, the enemy test with an else return 5, a merged single return 24, and the outer return in an else
// of the team test 5 with byte-identical code), so the order of the two returns does not decide the block order.
// M3 lane 14 r150 (h2m fast; control 5). Reading: every two-return spelling compiles to one layout, because returns
// that sit out of line come out in the reverse order of their tests, so the no-team exit (tested first) is always the
// last block and falls into the shared epilogue; retail has the not-enemy exit there. New forms: the enemy arm setting a
// `result` and falling through to `return result` with the not-enemy return in its else arm 5 (same bytes); the three
// tests as one early return through a new inline enemy predicate 24 forced inline (the single-return code below; 217 at
// 432 B as a plain inline, which stays a call); the not-enemy case as one `&&` early return with no flag 59 (that return
// stays in line ahead of the enemy code). The single final return (listed 24, 492 B) read in full: it is retail's code
// row for row except that the damage reload into xmm1 sits right after the allegiance call, ahead of the flag test, and
// serves both the enemy arm and the one exit, where retail reloads it at the top of the enemy arm and gives the
// not-enemy edge its own stack-reload exit after the xmm1 exit; on it the product written damage first 24 (same code),
// a named scale with the flag store ahead of the product 26. Diagnostic, restored (it repeats a test): the not-enemy
// early return written `team == NONE || !enemy` after the no-team early return 5 (same bytes: both exits then sit out
// of line, still the stack reload first). Re-measured: the no-team return in an else arm 5, one combined early return
// 5 (both the same bytes), two separate early returns 28, both cases as early returns with the enemy code last 16, the
// enemy arm scaling the damage in place 32.
// M3 lane 14 r151 (fast screen with 0xdb210/0xd8cb0; control 5). Reading (allocator and block trace): no single forced
// register decision gives retail's layout; on the single-final-return form (24) the best forced split of the damage
// value (kept in its stack home across the allegiance test) reaches 17 and leaves the no-team exit merged into the
// enemy arm's epilogue, and on this body the best forced decision is the same 17, so the order of the two cold exits is
// block placement, not register choice. New forms: the team and state tests nested, each with an else arm returning
// the damage 5 (same bytes); the enemy arm storing the flag first and returning the product directly 10; the product
// written damage first 5 (same bytes); diagnostics: the final return given a different value with the no-team return
// in an else arm, or as an early return, 142 each (511 B: the exits then differ in value, retail's order still not
// reached). Re-measured: both cases as early returns 16, the no-team early return with the state test in place 5, the
// single final return 24.
// M3 lane 3 r320 (h2m fast; control 5): the not-enemy case as an early return re-measured 16 (retail's exit order, but
// the enemy arm then sits out of line and the not-enemy return falls through); `enemy = !(team >= 0 && team < 16 &&
// 0xbfe60(...))` 74 (487 B); the enemy arm storing the flag first and returning the product 10; the team read into a
// long 155 (492 B); explicit else arms for both damage returns 5; the range test held in a named bool 5.
// M3 lane 7 r240 (fast screen; control 5). Retail's engine-path `jp` lands in the epilogue of the last exit block (the
// not-enemy stack reload in retail, the no-team xmm1 copy here), so the question is which return block falls into that
// shared epilogue. Forms: the engine test negated with the team logic in its arm and the engine scaling last 51 (the
// team code then comes first, its exits still not-enemy first); the zero-scale store arm with its own return 5; the
// state read into a `long mode` local (0x20f040's spelling) 5; a positive ally flag tested false 74 (487 B); the team
// logic in an explicit else of the engine test 5.
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
// Friendly output second 203, material field pointer/reference 203/203, named armor groups 213.
// Conditional adjustment 203, separate adjusted amount 213, shared point difference 209, named model index 330.
// NOT kept: the material index remains in DI through both lookups, and naming the model index shifts the object spill.
// m3 lane 4 r235: HELD (needs the owner's OK), build/r235/held-d82e0.patch: MATCH, fast and plain, and nothing else moves
// among the 234 functions of the five edited files (0xcf040, 0x184060, 0x188690 and 0xd9020 still match, 0xd9110 8,
// 0xfaa60 1160). Cause: retail hands 0x188690 its material index as a two-byte aggregate by value. Each call copies the
// field afresh (`xor r, r; mov r16, [esi + 0x7c]; push r`, never shared with the inlined test's load), 0x188690 keeps it
// on the stack and reads the whole slot, and 0xcf040 pushes its local's dword. The patch gives 0x188690's parameter and
// the fields passed to it (this record's +0x7c, the info's shield material, one field in each of the other two callers'
// files) the index wrapper type of 0x188690's own file, moved to include/impacts.h, and names the marker name here
// (`long marker_name = *(long *)entry;` ahead of the 0xb8d30 call, a one-use local that the match keeps). Held because
// 0xcf040's call line, inside a contributor's body, has to construct the wrapper explicitly: left to the implicit
// conversion it loses its match (236, a frame temporary). Tried on the way (fast): `inline` on 0x188690's definition 376
// (expanded at every site, 0xd9110 191); the wrapper parameter with the short wrapped at each call 198 (frame
// temporaries, 0xd9110 171); 0x188690 kept out of line and opaque 203 (same code, diagnostic); the field passed as a
// wrapper-typed lvalue 115 at 1272 bytes (the material block is retail's, scratch registers left). On that 115 form: the
// 0xd9b60 parameters as (data, friendly, definition, object_index) 115; one, two or three redundant paren pairs 115
// each; the region lookup as an inline accessor 115 (short parameter) or 306 (long); no region_index local 115; the
// region as a ternary 250; in the share loop a short local for the test 115, for the test and the call 98, a long local
// for the call 98, the count on the left of the test 115; the named marker name MATCH. Without the wrapper parameter
// the named marker name alone 193 (not kept); 0x188690 called through a pointer cast to the wrapper-parameter type
// matches this function but pins 0x188690's convention (0x188690 23, 0xcf040 7, 0x184060 123, 0xd9110 167).
// m3 lane 6 r221 (H2M fast): the held wrapper form re-applied, still MATCH. With an `operator short() const` added to the
// wrapper, contributor 0xd9020 needs no text change and the plain stores keep main's text (0xd82e0 MATCH, every matched
// neighbour as before), but 0xcf040's call line still needs the explicit construction: an implicit conversion of a
// by-value class-type argument builds a frame temporary (0xcf040 236, also with the constructor taking long), and a
// user-declared copy constructor changes the passing (0xd82e0 237, 0x184060 115, 0xd9110 162). HELD patch:
// build/r221/held-d82e0.patch (m3-lane6 worktree). On main's body: the material index bound by `short const &` for the
// test and both calls 203.
// M3 lane 14 r138 (fast; control 203): still BLOCKED-BY 0xcf040 (the held wrapper-parameter patch needs its call line,
// in a contributor's body). Retail's other callers support the aggregate reading: 0x184060 and 0xd9110 load the field
// fresh (`xor ecx, ecx; mov cx, [..]; push ecx`, which our matched 0x184060 gives with the short parameter), and 0xcf040
// stores a word into a stack slot and pushes the slot's whole dword, so an unsigned 16-bit parameter would not explain
// it. Tried in this file only: the two handles named (+0x14 first) 213; one element lookup for both handles 224 (1240 B,
// one call); the index in a long local cast back to short at the test and both calls 203; `amount * d5bc0(..)` 203; the
// material test as `!= NULL` 203.
// @retail 0xd82e0
void __stdcall function_d82e0(s_type_1e6529 *data, real damage, long object_index, s_damage_region_accumulator *accumulator)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	s_object_definition *object_definition = (s_object_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
	s_damage_info *info = (s_damage_info *)function_d5b60(object_index);
	s_damage_definition *jpt = (s_damage_definition *)g_4e3b44[data->definition_index & 0xffff].bytes;
	s_damage_effect_definition *definition = (s_damage_effect_definition *)((byte *)jpt + 0x10);

	if (info && object_definition->model_tag_index != NONE)
	{
		byte *model = g_4e3b44[object_definition->model_tag_index & 0xffff].bytes;
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

/* damages an object, and the objects it rides in: for each, the shield
   first, then the body and regions, then the aftermath */
// score 309 (was 309; 371 before m3 lane 4 r248): the loop's frame homes and ESI/EDI roles are retail's (1885 bytes for 1880); left: the reported
// block's shield material (retail reads the accumulator's info field there, ours the info local, which then stays live
// in EDI), the head (the count in ESI where retail has EDI, `add ecx, 0x10` for retail's `lea ecx, [eax + 0x10]`, the
// store order of the child flags and of friendly), the loop's first index (`dec esi` for `lea eax, [edi - 1]`), the
// kill block's object pointer (ESI, retail EBX, so info is reloaded after it), and scratch rotations in the inlined info
// lookup, the team block and the accumulator's three node fields. Earlier (371): The 0xf4 frame agrees. The 1893-byte
// body exceeds retail's 1880 bytes, and the flag homes
// and ESI/EDI assignments differ. Earlier scores were 470 and 661, with 470 at 1907 bytes.
// Retail's bools sit at esp+0x13..0x17, then object, i, definition, info. This build orders them differently and
// keeps region_absorbs in memory where retail holds it in bl. Retail holds object_count in EDI and this build holds it in ESI.
// The position of the 0xd5b60 call also differed earlier. Done: shield/body children flags were swapped (retail: body from
// info bit 4, shield from bit 6); the loop's walk of 0xd5b60 is damage_object_model_info (retail inlines it
// there but calls it at the first use). Retail calls 0xd5b60 after the parent walk, but there the link-time
// inliner inlines it (1986 bytes, 661); called before the walk it stays a call (544, 470 with the original
// nested unknown7c branch), which is the form kept. Tried: the unknown7c store as one else-if 5 worse than the
// nested form (the compare is repeated); the walk's info/definition declared in the other order 517 on the
// 544 tree (but it shares the tag lookup, which retail recomputes); i/object/info hoisted to function scope
// 544; body/shield declared in the other order 544; the damage-info walk taking the datum index instead of
// the object 666.
// Stand-in image 552 -> 371: the parent walk is a while loop, so the function_d5b60 call behind it stays a call as in
// retail (0xd7c2b; a for loop there raises the site's static frequency and LTCG inlines it); the pass loop keeps the
// current object index in a local and calls function_d5b60, which LTCG inlines there.
// M3 lane 12 r168 (h2m fast with 0xdb210 MATCH; control 371 at 1893 B). Frame read from the listing: retail keeps the five
// bools at frame +3..+7 (body, friendly, reported, shield children, then the region flag), then object +8, the loop index
// +0xc, definition +0x10, info +0x14; ours puts the loop index at +0 below the bools. In the loop the two bodies read the
// same variables in the same order except for two extra reloads in ours (the index after the side-effect block, whose
// team call takes its second argument in SI, and info after the kill block): retail keeps the index in EDI and info in
// ESI, ours the other way round, so the index gets one more reference and sorts first. Tried: the reported block's
// shield material read through the accumulator's info field (retail reads [accumulator+4] there) 365 (1890 B; info and
// definition then take retail's homes; a gain under 8, not kept); on it the loop as `for (i = object_count; --i >= 0;)`
// 365, `long i = object_count; while (i-- > 0)` 505 (1895 B), the test written `i > NONE` 368 (1893 B), the friendly flag
// declared ahead of the definition 364 (a moved declaration), `kill = true;` ahead of the hit byte's store 363 (statement
// order; match-only). Retail also computes the first index into EAX with `lea eax, [edi - 1]` where ours decrements the
// count's register in place. All restored.
// M3 lane 14 r138 (fast; control 371). Frame read push-normalised (F = locals base): retail packs body, friendly,
// reported, shield, region_absorbs at F+3..F+7, then object F+8, i F+0xc, definition F+0x10, info F+0x14; ours has i at
// F+0, a 3-byte hole, the five flags at F+7..F+0xb (body, reported, region_absorbs, friendly, shield), object F+0xc, info
// F+0x10, definition F+0x14 (damage, count and datum offset agree). Registers: after the team test retail reloads i into
// EDI and the object into ESI (then the datum pointer in ESI, i kept across 0x1df560's ESI argument); ours swaps them,
// so i is reloaded once more and info goes to EDI. Tried: `i < object_count - 1` 380 (1888 B); the owner stores through
// `object` instead of a second lookup 405 (1885 B); info looked up ahead of the object 522 (1913 B); region_absorbs
// declared at function scope 371 (same code); `objects[i]` at every use with no current_index local 525 (1920 B); i
// declared at function scope 371 (same code). Diagnostics (behaviour changes, reverted): one or two fewer i references
// 404/462 with i still at F+0 (the layout is not a near tie on i's count); one extra object reference 501.
// Typed object model field 371 (kept). Region absorption from the tests 391, explicit range arms 394, passing
// predicate 392, short region selector 455, renamed parent slot and pass object 371, child/absorption names 371,
// conditional model index 371, and model index inside the info arm 371. NOT kept: all other forms tie or raise
// the score and leave the first flag home.
// Completed count at the join 400, count after link 371, shared original-target test 547, report status first 498,
// reverse while 371, shared masked slot 534, shared owner team 371, and child permissions from the info result 482.
// NOT kept: The count-after-link, reverse while and shared team forms tie and the rest regress. The flag homes remain.
// m3 lane 4 r248 (fast, plain confirmed): the side-effect block's datum local dropped, its type and object read through
// the pool at each use as the db210 test below does, 371 -> 309 (1885 B): kept (the object through DAMAGE_OBJECT or the
// pool cast is the same code). With it the reported block's shield material read through the accumulator's info field
// (retail's `[accumulator + 4]` read there; the d8cb0 and db210 tests already read that field) 297 (1882 B): not kept,
// it mixes the field and the info local (match-only). Alone that info-field read 365; with the datum local kept and
// only the object re-read 297. On 297: the info lookup after the definition lookup
// 376; the child flags declared body first 295; `for (i = object_count; --i >= 0;)` 297; `kill = true;` ahead of the
// hit byte's store 295; both 293; with them `friendly` declared ahead of the definition 291; the definition through a
// named tag pointer plus 0x10 (as 0xd82e0 writes it) 293, with friendly first 291; `object_count` declared first 300;
// the accumulator's direction field typed as a vector pointer 291 (same code). Orders not kept (match-only).
// M3 lane 2 r138 (h2m fast; control 309, aligned 92, registers masked 35, registers and frame slots masked 13): the
// definition through a named damage-tag pointer plus 0x10 on this body 309 (same code); the kill block's two vitality
// stores through the owner-store pointer 404 (1890 B); `for (i = object_count; --i >= 0;)` with the parent test
// `i < object_count - 1` 304 (1884 B, aligned 136); the pending team event named once for the test and the call 309
// (same code); the victim's player index named once for the test and the call 471 (1894 B). Body unchanged.
// M3 lane 2 r140 (h2m fast; control 309, aligned 92, registers masked 42): the three node fields as ternaries on
// `i == 0` 469 (1898 B); `kill` declared at the top of the pass 309; the pass's report declared there 309; the team
// event test as two nested ifs 309 (all three the same code); the node fields stored in the record's order (0c, 10,
// 14) in both arms 369 (1881 B: a positional rise, but the node block is then retail's, its shared `mov [esp+0x44],
// eax` tail included). Lead (match-only parts): that order with the accumulator-field shield read 286 (1880 B), with
// the body flag declared first and `kill = true` ahead of the hit byte 285, with friendly ahead of the definition 283
// (aligned 45, registers masked 10: the frame is retail's; left are the head count ESI for EDI with its `dec`, the
// event word in BX for AX, and the info reload after the kill block). On 283: the definition through a named tag
// pointer 283; `i < object_count - 1` 466; `for (i = object_count; --i >= 0;)` 283; an `if (0 < object_count)` guard
// 283 (1882 B, not folded); the event word or the victim index named 283 (the same colouring). Body unchanged.
// m3 lane 4 r251 (H2M_FAST with 0xdb210 and 0xd8cb0, both MATCH; control 309): the five flag locals renamed to sort
// first or last 309 (same code: names do not order these bool homes); the tag named and the definition formed as `jpt +
// 0x10` 309 (same code; retail's `lea ecx, [eax + 0x10]` head is not that); the team identifier read once into a named
// short for the test and the call 309 (same code); `reported` declared first 489; the parents array declared first 309
// (same code); r248's match-only forms together (the shield material through the accumulator's info field, body flag
// first, friendly ahead of a named tag pointer, `kill = true;` first) 291 at 1882 B, with all five flag homes and the
// info/definition homes as retail; left there: the tag load in ECX + add (retail EAX + lea) and the count in ESI
// (retail EDI, its first index `lea eax, [edi - 1]` so the count is not decremented in place).
// M3 lane 14 r150 (h2m fast with 0xd9b60/0xdb210/0xd8cb0; control 309 at 1885 B for retail's 1880, aligned 92; 371 before
// r248). The aligned rows fall into: the five
// flag homes and the info/definition homes (declaration orders, match-only above); the head's tag load (ECX + add for
// EAX + lea); the count's register (ours ESI, saved in the prologue, with EDI saved at the loop; retail EDI, with ESI
// saved at the loop) and the pass index's first value (`dec esi` + `js` on the count's own register; retail `lea eax,
// [edi - 1]`, `test eax, eax`, `jl`, the count's register left alone); the team event block (event word BX for AX, so
// the victim's index loads straight into EAX where retail loads EDX and copies it at the call); the node fields' last
// store (ECX for EAX, so the two arms' tails do not merge); the kill block's object (ESI, evicting info, which is then
// reloaded; retail EBX, the dead kill flag's register, with info kept in ESI across the two calls); the reported
// block's info read; scratch rotations in the object-definition lookup and the tail. Reading: in retail the count and
// info keep their registers past the point where ours ends them (the count across the pass index's first value, info
// across the kill block), and each time ours then takes ESI where retail takes the next register. Tried for the
// count: `for (long i = object_count; i-- > 0;)` 499 (1890 B: the count is tested and the decrement moves to the loop
// top), as a while loop 499 (same code); one function-scope variable for the parent walk and the pass index 309 (same
// code); the single-object arm's two stores swapped 310; the walk as a for loop 610 (1944 B: 0xd5b60 is then
// expanded at its first call); the walk's store and increment as two statements, or with the increment after the
// parent read, 309 / 309 (same code). Diagnostic, restored: a read of the count at the top of the pass 506 (1899 B: it
// reads the count's home and the head is unchanged, so a later read does not keep the register). For the team event
// block: the victim's player index named in a nested test for the test and the call 309, with the event word named
// too 309 (both the same code).
// M3 lane 14 r151 (fast screen with 0xd9b60/0xdb210/0xd8cb0; plain confirmed with all of this file): KEPT the reported
// block's shield material read through the accumulator's info field 309 -> 297 (1882 B, aligned rows 92 -> 53): retail
// reads [accumulator+4] there while it holds info in a register, so the source read the field (the block's other info
// reads stay on the local, as retail's). Reading (frame trace, replayed): the five flags all have 3 references, so their
// order comes from the frame's density sort with the middle pivot; retail's flag order and its definition-below-info
// dword order follow from one more late reference to the definition or one fewer early one to the inlined info result.
// The event block: the event word (3 references) ranks below the victim's player index (56 against 38), which takes EAX
// for the call first, so the word gets EBX; forcing either rank alone makes the body worse (392/412), as does forcing
// the count into EDI only part way (274 on the 283 lead). On 297: the definition through a named tag pointer 297; the
// pass index declared ahead of its `for` 297; the victim index named in a nested test 297; the event test split in two
// nested ifs 297 (all the same code); the kill block writing through the owner-store pointer 400 (1887 B). The r140 lead
// parts re-measured on this body: all of them 283 (1880 B), the node stores in record order alone 286 (match-only).
// The first flag stores still use different slots. Parent append through end pointer with count 522;
// counted append with an exclusive/current reverse cursor 473/532; pointer append with those cursors
// 448/503; derive the last index from count 304; accumulate the last valid index 267; derive it from
// the end pointer 510. NOT kept: 304 gains five at retail length. The 267 form also reaches retail
// length, but starts at NONE and increments before the store. Retail counts from zero after the store.
// The initial child-permission homes and the parent count register still differ. NOT kept: the chained
// defaults gain one, the separate range test loses one at 1883 bytes, and other forms tie or regress.
// chained child defaults 308; single child flags word 309; native child permission masks 309; shared
// primary pass Boolean 540; nested damage-report admission 309; late absorption initialization 449;
// separate region span and flag tests 310; separate shield-and-body gate 309.
// Clear permissions on exclusion 493, explicit defaults in no-info arm 352,
// snapshot of flags used at the two child gates 538, selected flags then both
// permissions 355, defaults at info lookup 457, counted single-target append 309,
// combined event mask 473, rolling damage initialized first 484.
// NOT kept: the counted append ties and the rolling damage form only fixes length.
// The child-permission homes and parent count register still differ.
// GB15 (fast with 0xd9b60/0xdb210/0xd8cb0; control 309 at 1885/1880 B, +0x21, fr=0xf4; 14 trials, body unchanged):
// consistent accumulator.field_4_2-> replacing all info-> after assignment / in damage block / after 0xd9110: 324 each
// (1883 B, fixes all 5 flag homes and definition/info homes, but 0xd8119 reads [eax] where retail reads [esi]); + body
// first 322 (+0x32), + record-order node stores 399 (1880 B), + all retail orders 396 (1880 B, +0x32), + while loop 324,
// + i > NONE 329. Dropping `current` (DAMAGE_OBJECT at owner stores) 466 (1902 B, naturally recovers retail's `lea eax,
// [reg - 1]; test eax, eax; jl` loop head), + consistent accumulator.field_4_2-> 462. `kill` after owner stores 396
// (1888 B, al 221); `(bool)` cast on kill 309; named `damage_tag` local 309; `if/do-while` parent walk 309 (all same code).
// GB28 (fast with 0xd9b60/0xdb210/0xd8cb0, 0xdb210/0xd8cb0 MATCH; control 309 at 1885/1880 B, +0x21; 13 trials, body unchanged):
// accumulator.field_4_2-> in pre-damage block only 309, + reported shield_material 297 (1882 B, al 179, mk 22; mixed read, match-only),
// + retail orders 283 (1880 B, +0x32, mk 15: lines after kill block through tail exact, leaving only def lea, count EDI/lea, d5b60/obj_def
// scratch, event AX/BX and kill EBX/ESI); const member-address definition 309; inline tag_get 309; 2-use owner_player local 312 (nested 309);
// direct DAMAGE_OBJECT owner stores 466 (1902 B); forceinline set_owner 309; while reverse pass 309; prefix --i 309; short region_index 444;
// event type via s_damage_object_datum 498.
// M3 lane 2 r160 (fast screen with 0xd9b60 5, 0xdb210 and 0xd8cb0 MATCH each time; control 309 at 1885 B): the kill
// block's two vitality stores through a block-local pointer from its own lookup 355 (1902 B); the event word read into a
// named short ahead of the tests 309 (same code); the event block's type test through the pool datum 498 (1891 B); the
// side-effect block's team read through `object` 340 (1880 B); that block's type test through `object` 279 (1885 B: not
// kept, retail reads the pool datum's type byte there, at +3 of the entry it then takes the object from), both 373
// (1867 B); the report call taking `objects[i]` 309 (same code). The r140 lead rebuilt (accumulator-field shield read,
// node stores in record order, body flag first, friendly ahead of the definition, `kill = true` first): 283 at 1880 B.
// On it four groups are left: the head's tag load (ECX with an in-place add; retail EAX then `lea ecx, [eax + 0x10]`),
// the count (ESI; retail EDI, with the first index through `lea eax, [edi - 1]`), the scratch pair in the inlined info
// lookup (ECX / EAX exchanged) and the event block (word in BX; retail AX, victim index in EDX). A forced-ranking search
// on that lead lists eight register moves (2665 candidate decisions, none of the 24 tried lowers 283), so more than
// three decisions still separate it from retail.
// M3 lane GB r36 (14 fast trials with 0xd9b60 5, 0xdb210 and 0xd8cb0 MATCH; control 309 at 1885/1880 B, +0x21, fr=0xf4,
// al=233, mk=21; body unchanged): object_definition before info 321 (1889 B), inside if (info) 406 (1880 B, fr=0xf0);
// info after memset(&accumulator) 463; object_count > i + 1 311; i + 1 != object_count 310; if (object_count > 0) guard
// 311; (word)data->unknown7e != 0xffff 310 (mk=20); nested event/info ifs 309/309; DAMAGE_OBJECT in kill block 370
// (1914 B); definition_tag pointer local 309; object_count before objects[] 309; on r140's 283 lead: object_definition
// first 294 (1882 B, mk=15), (word)unknown7e + guard 284 (1883 B, al=166, mk=10, 533/533 insns).
// @retail 0xd7b80
void __stdcall function_d7b80(s_type_1e6529 *data, long object_index, short node_index, short unknown0c, short region_entry_index,
	vector3f const *unknown14)
{
	bool shield_children = true;
	bool body_children = true;
	s_damage_effect_definition *definition =
		(s_damage_effect_definition *)(g_4e3b44[data->definition_index & 0xffff].bytes + 0x10);
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
		long index = object_index;
		while (index != NONE)
		{
			objects[object_count++] = index;
			index = DAMAGE_OBJECT(index)->parent_object_index;
		}
	}

	s_damage_info *local_9127e6 = (s_damage_info *)function_d5b60(object_index);
	if (local_9127e6)
	{
		body_children = !((local_9127e6->flags >> 4) & 1);
		shield_children = !((local_9127e6->flags >> 6) & 1);
	}

	for (long i = object_count - 1; i >= 0; i--)
	{
		long current_index = objects[i];
		s_damage_object *object = DAMAGE_OBJECT(current_index);
		s_damage_info *info = (s_damage_info *)function_d5b60(current_index);
		s_object_definition *object_definition = (s_object_definition *)g_4e3b44[object->tag_index & 0xffff].bytes;
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

		if (info && object_definition->model_tag_index != NONE)
		{
			byte *model = g_4e3b44[object_definition->model_tag_index & 0xffff].bytes;
			bool region_absorbs = false;

			accumulator.field_4_2 = info;
			if (node_index >= 0 && node_index < info->unknownc4)
				accumulator.unknown08 = *(short *)(info->unknownc8 + node_index * 0x10);
			if (current_index == object_index)
			{
				if (region_entry_index == NONE)
				{
					data->unknown7c = info->unknownce;
				}
				else
				{
					byte *entry = *(byte **)(model + 0x5c) + region_entry_index * 0x14;

					data->unknown7c = *(short *)(entry + 0x10);
					accumulator.unknown00 = entry;
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
				bool kill = (data->flags >> 2) & 1;
				s_damage_object *current = DAMAGE_OBJECT(current_index);

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
					definition->side_effect == 2 && function_cc010(current_index, &data->direction) &&
					!TEST_FIELD_BIT(object->damage_flags.unknown7))
				{
					if (!((1 << ((s_damage_object_datum *)g_4e0300->data)[current_index & 0xffff].type) & 3) ||
						function_1df560(data->owner.team, DAMAGE_OBJECT(current_index)->team))
					{
						*(byte *)&data->unknown84 = (*(byte *)&data->unknown84 & 0x3f) | 0x80;
						kill = true;
					}
				}

				if (i + 1 < object_count)
					accumulator.damage = function_dc230(current_index, objects[i + 1], data) * damage;

				if (!(info->flags & 0x80) && !TEST_FIELD_BIT(object->damage_flags.unknown14) && kill &&
					!TEST_FIELD_BIT(object->damage_flags.body_depleted))
				{
					object->shield_vitality = 0.0f;
					function_d6a70(current_index);
					object->body_vitality = 0.0f;
					function_d6800(current_index, &data->owner, !((data->flags >> 7) & 1), false);
					accumulator.flags |= 0x41;
				}

				if (!(data->flags & 0x20) && !(definition->flags & 0x200) && (!region_absorbs || data->in_unknown_radius) &&
					object->maximum_shield_vitality > 0.0f)
				{
					if (i == 0 || shield_children && (info->flags & 1))
						function_d9110(current_index, definition, data, &accumulator);
				}

				if (g_4e6948->mode != 4 &&
					((1 << ((s_damage_object_datum *)g_4e0300->data)[current_index & 0xffff].type) & 2) && i == 0 &&
					accumulator.field_4_2 && (!(data->flags & 0x1000) || (info->flags & 0x100)))
				{
					function_db210(data, current_index);
				}

				if (!(definition->flags & 0x40) && (i == 0 || body_children && (accumulator.field_4_2->flags & 2)))
				{
					if ((data->flags & 1) || (data->flags & 0x100))
						function_d82e0(data, accumulator.damage, current_index, &accumulator);
					function_d8cb0(current_index, definition, data, &accumulator);
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
					function_b7360(current_index);
					reported = true;
				}
			}
		}

		if (object->node_index != NONE && (accumulator.unknown2c || accumulator.unknown2e))
			function_dbc80(object_index, accumulator.unknown2c, accumulator.unknown2e);

		s_damage_report report;

		function_d9490(data, &accumulator, &report, current_index);
		if (g_4e6948->mode == 4)
			report.flags &= ~1;
		function_d9640(&report, current_index);
		if ((accumulator.flags & 4) && g_4e6948->mode != 4)
			function_b8540(current_index);
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

	function_d0590(vehicle_index, &iterator);
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
// MiniMax C (11-20): no new form, no lower score (194 in the full image, 357 in the reduced image): a `word` local holding the
// team and read at both 0x1e9720 calls moves the frame's other homes and scores 418, and retail's `xor ecx, ecx` ahead of the
// team word comes from its callee taking the team as a 32-bit value where this file declares it `short`, which would change
// every other 0x1e9720 caller in the tree: BLOCKED-BY 0x1e9720's declaration.
// MiniMax C (11-19): no new form, no lower score (194 in the full image, 357 in the reduced image this lane measures):
// retail's 0xda110 takes its first argument in eax while this file declares it __stdcall, so the permutation still
// travels on the stack; changing that declaration would move the call setup of every caller across the tree, which
// this lane cannot check: BLOCKED-BY 0xda110.
// MiniMax E (11-17): no new form, no lower score (194): the first block still masks the index in eax where retail copies it into esi and masks the copy, and the blocks after that then sit at other offsets, so the calls and the jumps below compare one instruction out.
// MiniMax I (11-16): no new form, no lower score (357): the body is two bytes longer than retail's and the first difference is
// the schedule of the mask and the word store (retail moves the mask into ESI and stores the word first, this
// build masks in EAX and stores the word after the move).
// MiniMax I (11-14), NOT kept at review (a one-use team local at each call and a score-picked statement order are kept only with a match; 0xd5de0 357 -> 330 back to main): score 117 (was 194): the object's team read into one `word` local that both 0x1e9720 calls use (retail's `xor ecx, ecx; mov cx` at both), and the body block's info branch takes its rate before its maximum, so retail's XMM2 (maximum) and XMM0 (rate) loads come out in that order.
// MiniMax C (11-14): no new form, no lower score (194 in the full image, 357 in the reduced image this lane measures): the
// 0xda110 permutation still arrives in EAX where retail keeps its convention, the flag pairs still merge into one word test and the
// second 0x138880 is still inlined where retail calls it: BLOCKED-BY 0xda110.
// MiniMax I (11-15): no new form, no lower score (194 in the full image, 357 in the reduced image this lane measures): the flag pairs still merge into one word test at three sites, retail takes the 0xda110 permutation in a register where this build uses another, and the second 0x138880 is still inlined where retail calls it.
// MiniMax S (11-15): no new form, no lower score (194): the 0xda110 permutation still arrives in EAX where retail keeps its convention, the flag pairs still merge into one word test and the second 0x138880 is still inlined where retail calls it: BLOCKED-BY 0xda110.
// MiniMax E (11-20): no new form, no lower score (194): the flag pairs still merge into one word test at three sites, the 0xda110 permutation still arrives where retail keeps its own convention, and the second 0x138880 is still inlined where retail calls it: BLOCKED-BY 0xda110.
// score 194 (was 229): 229 before m3 lane 7 r155's 0xda110 convention; 400, 402, 607 before that; at r94's start 2148 bytes against 2170, frame 0x70. Round 75 (b4E) walked every block against retail 0xd5de0-0xd6657:
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
// m3 lane 7 r160 (fast with 0x192950, 0xd9f70 and 0xd8b50, control 194): the clamp test positive with the clamp arm first 251
// (2158 bytes); the permutation index named before the owner copies 245; read through the `pending_permutation` bitfield 194
// (same code); the team passed as `(word)object->team` 194 (no `xor ecx, ecx`); a one-use `word team` local at both 0x1e9720
// calls 159 (retail's `xor ecx, ecx; mov cx` at both; not kept, one-use locals): retail's team argument is a zero-extended
// 16-bit variable, where 0xd5d20 and 0xd5d80 sign-extend a short one.
// MiniMax R (11-17): no new form, no lower score (194): the flag pairs still merge into one word test at three sites, retail takes the 0xda110 permutation in a register where this build uses another, and the second 0x138880 is still inlined where retail calls it.
// MiniMax S (11-17): no new form, no lower score (194): the flag pairs still merge into one word test at three sites, retail takes the 0xda110 permutation in a register where this build uses another, and the second 0x138880 is still inlined where retail calls it.
// m3 lane 12 r156 (fast with 0x192950, 0xd9f70, 0xd8b50, 0xda110 and 0x138880; control 194, aligned 196): the body
// recharge test as nested ifs (maximum, then rate) 194, the body block's guard as nested ifs 194, both 194 (same code
// each: the second 0x138880 stays inlined, so its site frequency does not follow `&&` against nested tests); one `word
// team` local at the top of the info block for both recharge calls 354, the same as `short` 354. Diagnostics (one-use
// locals and a score-picked arm order, reverted): one-use `word team` locals at both calls 159 at 2172 B but aligned 194
// (the positional gain is the length); the clamp arm first with bool locals for both 0xd8b50 flags 244 at 2170 B; those
// three together 165 at 2170 B (aligned 165; left: the `& 7` copy order, a zero's register, and the body block from the
// inlined 0x138880 on).
// m3 lane 6 r218 (fast with 0x192950, 0xd9f70, 0xd8b50, 0xda110 and 0x138880, control 194; the held form also in a plain check).
// HELD (needs the owner's OK), build/r218/held-0xd5de0.patch: this function MATCHES byte for byte (2170/2170; 0xd8b50, the
// other callees and the caller 0xbc470 as before) with 0xd8b50's flag parameter declared first (at both calls retail loads
// the three address registers and pushes the flag last; 0xd8b50 itself still matches) plus these forms here: an implied
// `stun_ticks != 0 &&` ahead of the second 0x138880 test (the link-time inliner's limit follows how often it estimates a
// site runs, and one more test in front makes that test a call as in retail), a `word team` local at each 0x1e9720 call, a
// bool local for each 0xd8b50 flag, the info branch reading its rate before its maximum, `pending & 7` written at both of
// its uses, the shield clamp test held in a named bool with the clamp arm first, and the last loop reading `states[i]` at
// each use. 0xd8b50 is a contributor's function, so the body here is main's and nothing of this is kept.
// Counts on the way (each form added to the ones before): implied test 231 (2170 bytes), word team locals 179, bool flag
// locals 192 (2180), rate-first info branch 24, 0xd8b50's flag first 16, `pending & 7` at both uses 13, then the clamp
// test: positive 172, parenthesised 172, `!!` 172, `!(shield <= maximum)` 174, a double negation 172, `!(maximum >=
// shield)` 4, `!= false` 2, a named bool 2; on the 2 the last loop: the region pointer at the loop top 132, `state++, i++`
// 7, `i < info->region_count` 4, indexed states with a state local 3 (four spellings), `states[i]` at each use MATCH.
// The same body without the 0xd8b50 reorder 8 (the flag pushed ahead of the three address registers at both calls),
// without the implied test 134, without the rate-first info branch 179. With the positive clamp test the body block's
// float registers move (zero in xmm3, rate in xmm1), whatever the body block's own order: declaration, either branch,
// ternaries, nested tests, recharge spellings and one to three redundant parentheses 169 each, one maximum for shield and
// body 190 (slots rotate), the 0x80 notify in each arm 199 and 200; the two-step `&= 7` 169 (same code).
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

						if (function_d9f70(i, pending & 7, info, object_index))
						{
							s_damage_region_accumulator accumulator;

							memset(&accumulator, 0, sizeof(accumulator));
							function_da110(pending & 7, info, object_index, &owner, i, &accumulator);
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
					{
						word team = object->team;

						recharge = function_1e9720(3, team) * recharge;
					}
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
					bool overcharged = object->shield_vitality > maximum_shield;

					if (overcharged)
					{
						object->shield_vitality = maximum_shield;
						word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_recharging_bit, false);
						word_flags_set(&object->damage_flags_word, _damage_object_flag_shield_damaged_bit, false);
					}
					else
					{
						changed = true;
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
				rate = *(real *)((byte *)info + 0xd4);
				maximum_body = *(real *)((byte *)info + 0x38);
			}
			if (maximum_body > object->body_vitality && rate > 0.0f)
			{
				short stun_ticks = object->body_stun_ticks;

				if (stun_ticks == 0)
				{
					real recharge = g_510c54->rate * rate;

					if ((1 << object->type) & 3)
					{
						word team = object->team;

						recharge = function_1e9720(3, team) * recharge;
					}
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
				else if (stun_ticks != 0 && !function_138880())
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
			s_object_region_state *states = (s_object_region_state *)((byte *)object + object->region_states_offset);

			for (long i = 0; i < state_count; i++)
			{
				if (states[i].unknown02 > 0)
				{
					if (states[i].damage_level_ticks == 0 && info->region_count > i)
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
							if ((long)states[i].unknown02 > amount)
							{
								states[i].unknown02 -= (byte)amount;
							}
							else
							{
								states[i].unknown02 = 0;
								continue;
							}
							changed = true;
						}
					}
					else
					{
						states[i].damage_level_ticks--;
						changed = true;
					}
				}
			}
		}

		/* retail passes the character's vitality properties ([esp+0x14] at
		   0xd6598, NULL for a unit no actor drives), not the damage info */
		bool body_depleted = TEST_FIELD_BIT(object->damage_flags.body_depleted);

		changed |= function_d8b50(body_depleted, &object->unknownf8, &object->unknown100, (char *)&object->unknown109, bounds);
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
			bool shield_depleted = TEST_FIELD_BIT(object->damage_flags.shield_depleted);

			changed |= function_d8b50(shield_depleted, &object->unknownf4, &object->unknownfc, (char *)&object->unknown108, bounds);
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
// MiniMax B (11-19): no new form tried this round, no lower score (4): the two remaining differences are the mask's
// position (after `push ebp` here, before it in retail) and the tag entry's register (edx here, eax in retail), not
// stack slots; the earlier rounds' local and cast forms and a permuter with annealing (110 tries, round 11-16) leave
// them alone.
// MiniMax C (11-15): no new form, no lower score (4): the tag index is still masked after `push ebp` where retail masks it before, and the tag entry is still loaded into edx where retail reuses eax.
// MiniMax G (11-14): no new form, no lower score (4): the tag index is still masked after `push ebp` where retail
// masks it before, and the tag entry is still loaded into edx where retail reuses eax; a permuter run with annealing
// (31 tries) found nothing under 4.
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
// MiniMax D (11-14): no new form, no lower score (4): the model index read through the typed model_tag_index field compiles to the same four instructions as the raw-offset read; retail still masks the tag index before it pushes EBP and keeps the tag entry in EAX, where we mask it after the push and take the entry in EDX.
// MiniMax D (11-15): no new form, no lower score (4): retail still masks the tag index before it pushes ebp and still
// reuses eax for the tag entry where this build masks it after the push and takes the entry in edx; the mask's place and
// the entry's register are the scheduler's and the allocator's.
// MiniMax I (11-15): no new form, no lower score (4): retail still masks the tag index before it pushes ebp and still takes the tag entry in eax where this build masks it after the push and takes the entry in edx; the mask's place and the entry's register are the scheduler's and the allocator's. Retail then reads the entry's model index through eax as well, so its whole tag lookup lives in one register, while this build keeps the entry's pointer in edx and only the model index in eax.
// MiniMax R (11-16): no new form, no lower score (4): the permuter (33 tries in 305 s, annealing) found no variant under 4; retail still masks the tag index before it pushes EBP and reuses EAX for the tag entry, where this build masks it after the push and takes the entry in EDX.
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
// MiniMax S (11-16): no new form, no lower score (4): retail still masks the tag index before it pushes ebp and still reuses eax for the tag entry and its field, where this build masks it after the push and takes the entry in edx; a local for the masked index, a local for the tag bytes and the typed tag field all compile to the same four instructions, and a permuter run over the body (110 tries) found nothing below the four.
// MiniMax C (11-16): no new form, no lower score (4): the model index's mask sits between the two prologue pushes in retail and after both pushes here, and retail loads the entry pointer into eax and reuses that register for the field while this build loads it into edx; both differences are one instruction.
// MiniMax I (11-17): no new form, no lower score.
// MiniMax D (11-17): no new form, no lower score (4): retail still masks the model tag index before it pushes ebp and reuses
// eax for the tag entry and its field where this build masks the index after both pushes and takes the entry in edx; a
// permuter run over the body (27 tries) found nothing below the four, and the frame still lacks retail's extra dword.
// MiniMax R (11-17): no new form, no lower score (4): retail still masks the model tag index before it pushes ebp and reuses eax for the tag entry and its field where this build masks the index after both pushes and takes the entry in edx; the typed model_tag_index field, a local for the masked index and a local for the tag bytes all compile to the same four instructions.
// Matched in a full build with stand-in SDK headers (was 4): the tag entry's data pointer is held in a local that a
// const reference names before the model index is read from it; retail then masks the tag index before the second
// push and reuses eax for the entry and its field.
// @retail 0xdbb40
long function_dbb40(long object_index)
{
	s_damage_object *object = DAMAGE_OBJECT(object_index);
	long result = 0x7fffffff;
	long lowest_rank = 0x7fffffff;
	byte *pointer = g_4e3b44[object->tag_index & 0xffff].bytes;
	byte *const &kept = pointer;
	long model_index = *(long *)(kept + 0x38);

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

/* marks damage sections destroyed, and splits off the model nodes no
   longer joined to the object's root as new objects */
// score 167 (was 167): The section bound and primary mask exchange registers, which rotates the later scratch registers;
// the node loop's head then sits one byte early and loses retail's alignment pad, and the search lacks retail's NONE start.
// Earlier header 178 (was 146): the bitmap zero preceded node-scan admission.
// MiniMax B (11-20): no new form, no lower score (167): the section loop still rotates retail's EAX bound, EDI first mask and EDX bit into our EDI bound, EDX mask and EAX bit, and that one rotation is the whole 13-byte difference - the pad it drops misaligns every later row; this round's spellings of the section loop (condition reversed, or-assign, both, nested tests, operands reversed, cast dropped, store as or) all measure 167, the bound read straight from info->unknowne0 179 and as an int local 179, an int count local 167, a short count local 171, the piece search as a for/break and as a while/break 223 each, a two-use dword mask local (the loop test and the later guard) 173, and the one-use mask local that gives 166 with retail's EAX/EDI registers is match-only; what still differs is that rotation, the zero store ahead of the states call, the piece search entry and the last two lookups' scratch registers.
// m3 lane 5 r195 declared it `__stdcall`, retail's own convention (every caller pushes the three
// Earlier header 178 (was 146). Codex F429: a one-use unsigned first-mask local (`dword m = (word)section_mask_a;` ahead of
// Codex F429: a one-use unsigned first-mask local (`dword m = (word)section_mask_a;` ahead of
// Codex F429: a one-use unsigned first-mask local (`dword m = (word)section_mask_a;` ahead of
// the scan) gives 166 with retail's EAX/EDI section-loop registers; a one-use local is match-only, so not kept. With it
// the first mask still extends before the count guard, and the late detached-mask zero, piece search and final lookups
// still differ. Earlier: m3 lane 5 r195 declared it `__stdcall`, retail's own convention (every caller pushes the three
// arguments, `ret 0xc` at 0xdbfa5): the callee order that matches 0x1c4040, 0x146bf0 and 0x1c51c0 builds 0xd7b80 ahead of
// this function, which without the keyword hands it ECX/EDX (243) and loses 0xd6800 and 0xd8a40. The positional count
// rises at the same 799 bytes, but the top is now retail's (pool and tag loads ahead of the register saves, the datum
// offset kept in EAX): first difference +0x58 (was +0xa), aligned count 49 (was 71), register-masked 21 (was 35).
// Left: the section count's register (ours EDI, retail EAX), one extra zero store at [esp + 0x14] ahead of the states
// call (it shifts everything after it by one instruction), the piece/search entry and the final lookups.
// Earlier at 146 (was 146): Frame 0x13e0 agrees; 799/808 bytes, first +0xa delays pool/tag loads and
// retains the datum offset; detached-mask initialization, piece/search entry and final lookups differ.
// Earlier header detail: 156 before MiniMax A (11-14), 174 before r69; frame 0x13e0, 808 bytes and the first 69 instructions agree (handle in ESI, offset and detached
// MiniMax G (11-14): no new form, no lower score (156): the mask's zero store, the piece loop's guard order, the
// search's entry test and the scratch registers of the last two lookups still differ.
// MiniMax A (11-12): no new form, no lower score (156): the mask zero is still stored ahead of the states call where retail stores it in the node loop preheader, and the piece loop guard order, the search entry test and the scratch registers of the last two lookups still differ; storing the zero inside the block gives retail store but reallocates the frame (170), so the body stands.
// MiniMax I (11-16): no new form, no lower score (146): the first difference is the schedule of the globals' loads (retail loads
// them before the pushes and keeps the table, this build reloads them after the pushes) and the section mask
// test, which this build does against a register copy of the argument.
// MiniMax B (11-12): no new form, no lower score (156): the mask's zero store, the piece loop's guard order, the search's entry test and the last two lookups' scratch registers still differ.
// MiniMax H (11-12): no new form, no lower score (156): the node loop's guard, the search's entry test and the mask's zero placement are unchanged; the lane history's best form (the piece pointer with the piece loop as `!= 0`) is 157 and the retail guard order is 187.
// MiniMax H (11-13): the mask's zero moved after the states call 154 (the store then lands ahead of the mask test, retail has it inside the mask block), the zero assigned after the 0xba690 call 170 and in the for-init 231 (both rebuild the function: the datum offset stays in ESI and the node count takes 0x18), so 156 stands.
// MiniMax A (11-14): the piece loop's bottom test as `!= 0` and the first-node search scanning for the first set bit reach
// 146, ten points and over the keep bar; the mask's zero and the guard order stay as they were.
// MiniMax I (11-15): no new form, no lower score (146): retail looks the tag definition up before it saves the registers and keeps the object index in a register where this build saves first and routes the index through another, and retail spills the index * 12 at +0x10 where this build keeps the offset in a register.
// MiniMax D (11-14): no new form, no lower score (146): retail looks the tag definition up before it saves the registers and keeps the object index in eax, where this build saves first and routes the index through edi.
// Earlier 146 (was 156): 174 before r69; frame 0x13e0, 808 bytes and the first 69 instructions agree (handle in ESI, offset and detached
// mask in memory). Left: the mask's zero is stored ahead of the states call (retail: at the first node loop), the piece
// loop's guard order, the section loop's count spill, and the scratch registers of the last two lookups.
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
// m3 lane 7 r160 (fast with 0xdb810 and 0xdbb40, control 156): the node loop as an inline helper returning the mask 288
// (724 bytes: not inlined, a call); retail's guard (`detached > 0 && tag != NONE`) around `while (detached != 0)` 187
// (777 bytes: the search loses its entry test and its head pad as retail, but the mask takes ESI and node_count EBX
// with object_index left in memory); on that, the search as an inline helper taking the mask by value 187 (same code),
// the search with its own counter 187, the zero declared in the mask block before 0xba690 229 (784 bytes).
// m3 lane 12 r125 (fast with 0xdb810 and 0xdbb40, control 146): same code (146): the object looked up through a
// file-local inline getter, `info` declared first and assigned after the definition, the section masks read once into
// word locals. Retail's guard `detached > 0 && tag != NONE` around a do-while ending `!= 0` with the for/break search
// (re-measured 187, 777 bytes) gives retail's head exactly (globals before the pushes, the datum offset spilled, mask_a
// in EDI); only the mask taking ESI differs (object_index then in memory, node_states/node_count EDI/EBX). On it: the
// mask as a one-element array 187 (same code: scalar-replaced), the while/ternary search 206 (815 bytes), a while loop 187;
// a separate piece-loop copy (`long remaining_nodes = detached_nodes;` after the node loop, round 69's form) 155 at 808
// bytes, first +0xc4: everything from the search's 0xdb810 call on is retail's code one line late; left the zero stored
// before the states call (retail: node-loop preheader), the search's entry test and the back-edge reload order (the copy
// blocks the jump threading retail shows, node-loop `jle` straight to the block end). On 155: the zero after 0xba690 178,
// at the mask block's top 178 (head flips), the search as a do-while 220 (788 bytes), `while (remaining != 0)` 155, a
// dword copy 155; grid of node bit unnamed / piece object pointer / while search with the zero late 178 214 171 215 206
// 174 209 167, with the zero early (node bit, pointer, search) 146 (001) 156 (010) 135 (011, 798 bytes, baseline's head)
// 178 214 171 215; the guard on the mask with the copy inside the guarded block 204, with the zero late 223, with a
// while loop 204. None kept (a copy that only splits the mask's weight; no match).
// On retail's guard form: the node loop as a forceinline helper returning the mask (zero inside it) 178 (head flips as
// with any late zero), returning a dword 178, with the while/ternary search 214, the helper alone on this body 205
// (799 bytes); the zero after 0xba690 re-measured 229 (784 bytes), with the creation record and the node states
// declared at the function or mask-block top 229, the creation record at the top on the early-zero form 187 (all the
// same code). Lead: the copy form's tail is retail's; what is missing is a late zero that keeps object_index in ESI.
// m3 lane 12 r128 (fast with 0xdb810 and 0xdbb40, control 146; a one-file LTCG harness of damage.cpp with its eight callee
// files' objects gives our code exactly but for 0x1c3770's argument passing, aligned counts in brackets, base [72]): a
// per-iteration copy of the mask for the search 146 (folded); the node loop's `|=` through a forceinline pointer helper 146;
// object_index read once through its address 146; the final node-state set given the component recomputed 221 (819 bytes);
// `attached_nodes` as long 146. The root node named (`long root_node = function_dbb40(object_index);`, then 0xdb810 given it)
// 161 (799 bytes, first +0xc4) [47]: the whole head becomes retail's (globals before the pushes, the offset spilled, mask_a in
// EDI) with the mask in memory at +0x10 and node_count sharing the offset's slot, but it is a one-use call result (match-only).
// With it: the for/break search 156 (808 bytes) [25], plus the piece object pointer 157 [15]. Without it, the for/break search
// with the piece pointer is the same code [15] (157 fast, 808 bytes, first +0xc4): left only the zero stored before the states
// call, the guard order and the search's entry test. Harness grid (64 forms, zero declared just before the node loop): root
// node named + retail's guard (`detached > 0 && tag != NONE` around a do-while ending `!= 0`) + piece pointer + node bit unnamed
// + the while/ternary search 77 fast (808 bytes, first +0x37) [29], retail's register roles throughout (left: the offset shares
// the mask's slot, the search); that with the early zero gives retail's slot layout exactly [28] (mask alone at +0x10, node_count
// sharing the offset's +0x14), so retail's mask is live across the inlined 0xba690 lookup; that with `long detached_nodes = 0;`
// at the top of the `if (section_mask_a)` block (the store sinks into retail's preheader slot) 106 fast (810 bytes) [24]: the
// first 122 instructions are retail's exactly; left the search (ours `cmp ecx, edi; jl; or ecx, -1; push ecx`, retail's
// for/break with a NONE start, `or eax, -1` ... `mov eax, ecx`), the 0xb7930 tag load order and the creation-copy registers.
// Not kept: all rest on the named root node. Every for/break search spelling on that form (NONE in the declaration, set before
// the loop, its own counter, a found test after a break loop, first_node at the guard scope, `while (!= 0)`) [89-102] hands the
// mask ESI and object_index leaves it (frame 0x13dc); 80 more grid forms (root node, piece pointer, node bit, long piece_nodes,
// long attached, word section masks, creation at the block top) [99-102]; with the late zero and no named root node the head
// flips in every form [51-79]; r125's copy form with the block-top zero [80-108]; a named node index or model index [102-122].
// P579: Scalar transfers at object tag/model tag/original component/section bound/node state,
// paired metadata zero-byte memset, then creation-tag/created-component transfers: 144/149/146/146/146/144/161/146.
// NOT kept: no exact native entry/bitmap/search recovery; original complete body and observations restored.
// MiniMax D (2026-11-19): the detach count now starts at zero inside the section-mask branch, the section decision is
// `detached > 0 && tag != NONE`, the node bit is unnamed and the search keeps the while/ternary shape: 167 fast (795 B)
// from 178 (799 B). NOT kept: the for/break search (223, frame 0x13dc) and the guard order on its own (177).
// Left: the section loop's count/mask/bit registers, the g_51e9b8 and datum reload registers, the attachment store
// order and the register the search hands to the node lookup.
// m3 lane 8 r176 (fast with 0xdb810 and 0xdbb40, control 146): the section loop's register choice (retail keeps the count in
// EAX and mask_a in EDI and spills the datum offset; ours keeps the offset in EDI and spills the count into the mask's slot):
// the count declared in the for-init 146 (same code); both mask parameters `word` 146 (same code); the bound read as
// `info->unknowne0` 172 (793 B); `info` declared first, assigned after the definition 146; `word` masks without the casts 146.
// m3 lane 1 r115 (fast with 0xdb810 and 0xdbb40, control 146, register-masked 35): lane 12 r128's lead (root node named,
// retail's guard `detached > 0 && tag != NONE` around a do-while ending `!= 0`, the piece object pointer, the node bit
// unnamed, the while/ternary search) re-measured: zero before the node loop 77 (808 B, masked 10), zero where it is now 138
// (814 B, masked 7: only the zero's placement, the search's shape and one definition reload left), zero at the mask block's
// top 106 (810 B, masked 8); with the for/break search 221 (792 B) / 188 (776 B) / 222 (790 B). New on the block-top and
// current-zero leads: the search as an inline helper taking the mask and count by value (`return i;` in the loop, `return
// NONE;` after) 224 (788 B) / 191 (772 B), with a NONE start and break 221 / 188; the helper taking the mask by pointer is the
// same code (the address folds away once inlined); `~piece_nodes` named once for the update and the node mask 146 (same
// code). Diagnostic: one to three dead initialised locals on the for/break lead 221/222 each, so the flip is not a
// symbol-count phase. Every search with retail's for/break shape hands the mask a register (frame 0x13dc).
// Scan-local primary mask and signed/unsigned bitmap: zero after capture 164/164,
// at scan admission 217/217, separate zero after the lookup 217/217, or
// combined with marking-loop initialization 217/217. Positive node count owns
// marking and creation 171 with direct masks, 158 with the scan-local primary.
// NOT kept: the 164 form clears the bitmap before the section-mask branch;
// the 158 form moves the datum spill and adds a second node-count test.
// Nullable bound, assigned bound and nullable while 178/183/178; scoped unsigned first mask 166 (one-use local, match-only: not kept);
// mask shared through detachment 155; nullable bound/shared bit 221; shifted mask tests 180;
// scoped damage record with first mask 166; first mask inside scan or with shared test bit 178/178;
// reversed first mask expression 166; first mask with late scan zero 217.
// NOT kept: sharing the first mask through detachment loses the retail EAX/EDI roles and word admission;
// moving the bitmap zero on the scoped-mask form changes the frame allocation and grows to 811 bytes.
// Plain confirmation of the scoped first mask changes only this function among 65 checked markers.
// Round 431: word/signed-short shared section bit 223/142; first/second word mask intersection
// 194/129; both intersections as words/dwords 205/202; local/entry-initialized section while scan 178/205.
// NOT kept: signed bit sign-extends the shift and spills the count; second mask intersection hoists the
// mask and tests a word. Retail reloads that mask inside the loop and tests the full-width shifted bit.
// Shared dword/word test bit 178/223, shared dword/word store bit 220/156. Primary mask extraction
// with separate/shared writes 208/188, secondary mask extraction 200/188. NOT kept: the 156 form
// hoists the second mask and shares the two store shifts while retail reloads the second mask and
// shifts separately. The count and detach bitmap homes still differ.
// Scoped const damage record, immutable count, int scan, early/late bound names,
// secondary-test continue and body increment 178 each. Both section decisions before the writes 200.
// NOT kept: the decision locals move the datum-offset spill and spill the section count.
// m3 lane 11 r130 (fast with 0xdb810/0xdbb40, control 167 at 795 B): the cause of the shift from the node loop on is one
// byte: the inlined node-state lookup loads g_4e0300 into EAX (the 5-byte form) where retail uses EDX, so the node loop's
// head needs 8 pad bytes and VC drops the 7-byte pad retail has; the rotation starts in the section loop (ours count EDI,
// mask EDX, bit EAX; retail EAX, EDI, EDX). Codex F429's one-use `dword` first-mask local re-measured on this body 112
// (811 B: retail's section registers, the node-loop pad and the piece loop's jmp/pad layout; left the zero-extension ahead
// of the count guard, the search's NONE start and the later rotation; one-use local, not kept). On it: the for/break search
// 247 (777 B, frame 0x13dc, the mask takes ESI); with the root node named 247; with a piece-loop copy of the mask 232
// (792 B, object_index loses ESI to the node count); the while search then `if (i < node_count) first_node = i;` 215
// (810 B). On this body: the section tests written mask first 167 (same code).
// Word setter for both section stores or each store alone 167 each. Damage record, object tag,
// connected mask, detach bitmap and metadata pointer names that change sort order 167 each.
// NOT kept: the bound still takes EDI and the primary mask takes EDX.
// m3 lane 4 r235 (fast, control 167 at 795 B): both section masks as two-byte wrapper aggregates passed by value (the cause
// found for 0xd82e0's material index; every caller on the same type) 167, same code: the member reads hoist exactly as the
// casts do. A dword first-mask local declared inside the section loop's body 167, both masks so 167 (folded back: retail's
// `test edx, edi` operand order says its mask is a hoisted expression, not a variable, yet it holds EDI while the count
// stays in EAX); the two stores through the existing word_flags_set 167 (same code). On the one-use first-mask local
// (re-measured 112 at 811 B, `movzx` ahead of the count guard and `test edi, edx`): the test written mask first 112
// (same code); the local declared under a redundant `if (0 < section_count)` guard 163 (both masks hoist, the count
// spills to +0x14), with the test reversed 163. Nothing kept.
// m3 lane 6 r221 (H2M fast, control 167): the section loop as a forceinline helper taking the object, the bound as a
// by-value count and both masks 167; `section_count` declared at function scope 167; the piece search with its own
// counter 167; `attached_nodes` declared ahead of the section loop 167 (all the same code); the bound read at each test
// 179 (listed). Downstream of the section rotation the definition reload is scheduled ahead of the attached-mask store
// (ECX here, EAX after the store in retail), which carries the g_51e9b8 and node-lookup rotations.
// Word primary mask shared between section marking and detachment 167, before the record query 167;
// shared full-width test bit 167, with both word setters 167; shared word test bit 225, with both setters 225;
// primary or secondary setter with the word mask 167/167. NOT kept: the word test bit shortens the scan
// and moves the first difference earlier. The EDI count and EDX primary mask persist in the tie forms.
// Count and ordinal in one for initializer, both orders with either record scope and
// signed/unsigned membership shifts: 167 in all eight forms. A positive guard with
// terminal-ordinal test, with or without the promoted primary mask, 181/181.
// NOT kept: the count stays in EDI, and the terminal-ordinal loop raises the score.
// m3 lane 4 r248 (h2m fast with 0xdb810/0xdbb40, control 167, aligned 37): the bound read as `info->unknowne0` at each
// test re-measured 179 (811 B): the loop body then has retail's registers exactly (first mask hoisted in EDI, bit EDX,
// index ECX) because info itself stays live in EAX; retail keeps its count in EAX, loaded over info, so retail's count
// is a variable that takes info's register where ours takes EDI. On main's body: the count `long const` 167; a redundant
// `if (section_count > 0)` around the loop 181; info declared in the `if` condition 167; info typed `const` 167.
// M3 lane 2 r138 (h2m fast with 0xdb810/0xdbb40, both MATCH; control 167, aligned 37, registers masked 7): the node
// loop's two tests as one `&&` 167; the section stores or-ing `(word)(1 << i)` 167; the count bound to a
// `long const &` for the loop test 167; info bound to a const pointer reference for the count read 167; the first mask
// bound to a `short const &` inside the loop 167; the count assigned through a `long &` 167 (all the same code); the
// components as typed pointer arithmetic do not compile in this file (incomplete type). Body unchanged.
// M3 lane 2 r140 (h2m fast with 0xdb810/0xdbb40, both MATCH; control 167, registers masked 11): a `dword` primary mask
// local shared by the section loop and the detachment test 169 (811 B; retail reads the argument's word in memory
// there); the piece search as a counted `for` with `first_node = i; break;` 223 (793 B, frame 0x13dc), with its own
// counter 223; an empty-bodied `for` then `if (i < node_count) first_node = i;` 170 (810 B); the count as
// `info ? info->unknowne0 : 0` with the loop outside the `if` 182 (not threaded, count still EDI); the counted break
// search on the one-use first-mask local (re-measured 112) 247 (777 B). Body unchanged.
// M3 lane 14 r151 (fast screen with 0xdb810/0xdbb40; control 167). Reading (allocator trace): in the section loop the
// count ranks last (-3: referenced only by its load, the entry test and the latch, live and unused through the four
// body blocks), after the shifted bit (50) and the hoisted first mask (2), so the bit takes EAX, the mask EDX and the
// count EDI; retail's EAX/EDX/EDI means its count ranked above the bit. Forcing the count's rank to the top alone gives
// 107 (the section loop and the pad become retail's, first difference +0xc2). Forms: the bit named once per pass and
// used for both tests and stores 222 (779 B); one loop per mask, both bounded by the field 237 (843 B); the count
// started at zero outside the info test and set inside it, the loop after the test 172; a guarded do-while
// (diagnostic) 167; the latch written `section_count > i` 167 (same code).
// Shared section/node count at lookup or entry, each with or without one scan
// ordinal, scoped count names, and shared count with read-only or conditional
// information record: seven at 167. Shared count from the nullable record 182.
// The section bound still takes EDI in the tie forms and the node-loop pad differs.
// Accumulate primary, secondary or both section masks before publication, from zero 199/198/243
// or from the current flags 219/219/257. Unconditional mask intersections with primary or secondary
// publication first 201/203. Each retains frame 0x13e0 and moves the first difference ahead of the original loop.
// Remaining-distance comparison 181, inclusive last-section bound 224.
// Model early guard, component early guard, split guards and combined early
// guard 167 each. Reused component index and typed component lookups 167/167.
// The section count still takes EDI and the node-loop pad remains absent.
// Masked unsigned ordinal bit for section tests/stores/both 205/204/193; shared test bit 205,
// shared test and store bit 205, with word setters 205. With masked node bits 232, or with
// word setters and masked node bits 224. NOT kept: the extra mask moves the count and ordinal
// away from the retail allocation. The exact-length form changes the frame and raises the score.
// The count and primary mask registers still differ, as does the node-loop padding.
// primary mask shifted to the selected bit 203; secondary mask shifted to the selected bit 201.
// both masks shifted to the selected bit 184; both selected bits with shared word setters 184.
// attached-node mask shifted to the selected bit 206; piece-search mask shifted to the selected bit 169.
// both node masks shifted to the selected bit 204; selected bits through both phases 220.
// NOT kept: the right-shift test encodings differ from retail and raise the score. Both 808-byte forms change the entry registers.
// M3 lane GA r2 (fast with 0xdb810/0xdbb40/0xd6800/0xd8a40, all MATCH; control 167, aligned 42, masked 12):
// forceinline section-mask helper 167; inline find_first_bit_set helper 223 (793 B, frame 0x13dc); named
// `s_damage_object *piece` used twice for `piece->node_index` 168 (794 B, aligned 32, masked 9: lines 188-226
// byte-exact); `dword detached_nodes` 167; `creation` at `while` scope 167; `bit_vector_test` on `attached_nodes`
// 223 (819 B); signed section tests without `(word)` casts 168; both prototype and definition with `word` masks
// 167 (with `piece` and no casts 168). On `piece`: tag guard outside `while (detached_nodes > 0)` 211 (808 B);
// combined `while (detached_nodes > 0 && tag != NONE)` 220; `do ... while (detached_nodes != 0)` 168; `for/break`
// search 224 (792 B, 0x13dc, `detached_nodes` takes ESI); `while (i < node_count)` with break 224;
// `if (info && info->unknowne0 > 0)` 184; `i` before `section_count` 168; `first_node == NONE` in `for`/`while`
// search header (`i < count && first == NONE`, `first == NONE && i < count`, `while`, `do-while`, baseline) 217
// (808 B, 0x13e0, emits `cmp eax, -1` + `cmovne`), with `else i++` 162 (810 B, aligned 135, masked 54).
// Diagnostics: one-use `dword m = (word)section_mask_a` + `piece` 111 (810 B, aligned 24; fixes section/node/search
// pads and lines 171-226); same + `first_node == NONE` search 215 (808 B). Body unchanged.
// m3 lane 6 r243 (allocator reading): the section loop's rotation is a colouring order. The bit `1 << i` (priority
// 50: the loop head at weight 2 with four referenced values, the second test, minus the first store) colours before
// `info` (48: the entry block's ten referenced values and the count read's four), takes EAX, and the mask and count
// follow into EDX and EDI. With `info` coloured first (EAX) the count's EAX preference keeps EAX away from the bit and
// the mask, which gives retail's EDX/EDI/EAX: forcing the bit into EDX, or info's priority to 51, scores 107 (first
// +0xc2); 49 changes nothing. So the cause is one more value referenced in the entry block (or two more in the count
// read's block), or two more in the first store's block. The one-use first-mask local works that way (its definition
// joins the count read's block, info 52), which is also why its zero-extension lands ahead of the count guard.
// Forms this round: the count from `info ? info->unknowne0 : 0` with no `if (info)` 182; `1u << i` in both tests 167
// (same code); the second mask read through a pointer to the parameter 167 (same code, still hoisted); the info
// lookup ahead of the object and definition lookups 229 (the call moves up); the getter's result as a `long` cast at
// the count read 167 (same code); the one-use first-mask local after the count 112 (811 B, the zero-extension still
// ahead of the guard); `word` mask parameters without casts re-measured 167 (the second mask is still reached only
// through its hoisted load, so it adds no value to the entry block). The for/break search re-measured 223 (793 B):
// `detached_nodes` becomes one value of priority 79 across the detach block and takes ESI. Diagnostic only (changes
// the code): the detach test as `section_mask_a | section_mask_b` gives the second mask a value in the entry block, the
// bit then takes EDX as retail's does, but that value holds EDI and the count is spilled, 128 (811 B); retail's extra
// entry-block value must be one that gets no register there (a constant or a parameter value like the first mask's).
// Section membership by union equality for primary/secondary/both 199/197/202;
// by bit-removal inequality 200/200/202; full intersection equality 200;
// complement exclusion 203. The section count and primary mask still compete
// for registers. NOT kept: the alternate tests lengthen the loop and raise the score.
// M3 lane 7 r231 (H2M_FAST with 0xdb810/0xdbb40, both MATCH; control 167). Allocator ledger of the section loop: the
// bit ranks 50 (head block +48 at weight 2 with four values, second test +8, the first store's block -6) and info 48
// (entry block +40 with ten values, count read +8 with four), so the bit takes EAX first; retail needs info at 51 or
// more, or the bit at 47 or less. Forms: the definition read after the info call 227 (first +0x13); the object through
// a pointer to its header element 167 (same code); the section stores as `field = field | (word)(1 << i)` 167 (same
// code); the counter declared ahead of the `for` 167 (same code); the piece search's result declared at its
// computation with no NONE initialiser 167 (same code). Body unchanged.
// M3 lane 5 r250 (fast with 0xdb810/0xdbb40; control 167): the one-use first-mask local re-measured 112 (first +0x5d: its
// zero-extension still ahead of the count guard, and the creation block's +0x28/+0x2c reads then swap order); a two-use
// first-mask local at function scope serving the section loop and the detach test 169; the piece search as a NONE-start
// for/break on main's body 223 and on the one-use mask local 247 (both frame 0x13dc: the detached mask takes ESI, where
// retail keeps it in memory and reloads it at the loop head); one function-scope counter for the section and node loops
// 167 (same code); the section count read from the record at each test 179. Body unchanged.
// M3 lane 6 r249 (fast with 0xdb810/0xdbb40, both MATCH; control 167). Allocator ledger re-read: the bit 50 (loop
// head +48 at weight 2 with four values, one of them the constant 1), info 48 (entry block +40 with ten values: the
// constants 0xffff, 0 and 1 and the first mask's parameter value among them; count read +8 with four); NONE is not
// among the entry block's values, and the second mask (one read) is no value at all. Forms: the lookup's result kept
// as the `long` it returns and cast at the count read 167 (same code); the bit narrowed to a word in both tests 225
// (779 B); the piece search's test as `(... & (1 << i)) == 0` 167; the piece's node mask as a `long` 167 (same code
// each); the node-index test through a fresh object lookup 237 (827 B, a second path). Body unchanged.
// @retail 0xdbc80
void __stdcall function_dbc80(long object_index, short section_mask_a, short section_mask_b)
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
		s_havok_node_states states;

		havok_component_node_states_get(component, &states);
		if (section_mask_a)
		{
			byte *node_states;
			long node_count;
			long node_records;
			long i;
			long detached_nodes = 0;

			function_ba690(object_index, &node_count, &node_states, NULL, &node_records);

			for (i = 0; i < node_count; i++)
			{
				if (node_states[i] != 0xff)
				{
					if (!(attached_nodes & (1 << i)))
					{
						byte *node = (byte *)node_records + i * 8;

						node_states[i] = 0xff;
						node[0] = 0xff;
						node[1] = 0;
						node[2] = 0;
						*(long *)(node + 4) = NONE;
						detached_nodes |= 1 << i;
					}
				}
			}
			if (detached_nodes > 0 && object_definition->crate_object.tag_index != NONE)
			{
				while (detached_nodes != 0)
				{
					long first_node = NONE;
					s_damage_object_creation creation;

					i = 0;
					while (i < node_count && !(detached_nodes & (1 << i)))
						i++;
					first_node = i < node_count ? i : NONE;

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
