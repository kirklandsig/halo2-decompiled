// @flags /O2 /Gr
/* UNKNOWN_1E1F20.CPP: the actor's weapon (lane M; called by the behaviors of
   0x1a8000..0x1affff) */

#include "unknown_11c920.h"
#include "ai_actor.h"
#include "unknown_2551c0.h"
#include "object_markers.h"
#include "object_queries.h"
#include "unknown_19ec40.h"
#include "unknown_1e46c0.h"
#include "unknown_26b230.h"
#include <string.h>

struct s_ai_weapon_definition
{
	byte unknown000[0x12f];
	byte flags12f_0 : 1;
};

long function_cbd50(long unit_index, short weapon_index);

/* the weapon in the current slot of the unit at +0x274 when the flag at
   +0x268 is set, else (or when that one holds none) of the actor's unit at
   +0x18 (0xcbd50 inlined by LTCG) */
// @retail 0x1e1f20
long function_1e1f20(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;

	if (actor->unknown268 && actor->unknown274 != NONE)
	{
		long unit_index = actor->unknown274;
		result = function_cbd50(unit_index, ai_object_get(actor->unknown274)->current_weapon);
	}
	if (result == NONE && actor->unknown018 != NONE)
	{
		long unit_index = actor->unknown018;
		result = function_cbd50(unit_index, ai_object_get(actor->unknown018)->current_weapon);
	}
	return result;
}

/* the weapon in the slot at +0x213 of the actor's unit, NONE without a unit
   (0xcbd50 inlined by LTCG) */
// @retail 0x1e1fd0
long function_1e1fd0(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long result = NONE;

	if (actor->unknown018 != NONE)
	{
		long unit_index = actor->unknown018;
		result = function_cbd50(unit_index, *((signed char *)ai_object_get(actor->unknown018) + 0x213));
	}
	return result;
}

// @retail 0x1e2030
bool function_1e2030(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long weapon_index = function_1e1f20(actor_index);
	bool result = false;

	if (weapon_index != NONE)
	{
		s_ai_weapon_definition *definition = (s_ai_weapon_definition *)g_4e3b44[ai_object_get(weapon_index)->definition_index & 0xffff].bytes;
		result = !TEST_FIELD_BIT(definition->flags12f_0);
		if (result && actor->unknown018 != NONE && (ai_object_get(actor->unknown018)->flags19 & 2))
			result = false;
	}
	return result;
}

void __stdcall function_cb500(long unit_index, short mode, const point3f *origin,
	const vector3f *forward, const real *offsets, point3f *point);

struct s_object_position_view
{
	byte field_0[0x30];
	point3f position;
	byte field_3c[0xaa - 0x3c];
	byte type;
};

// @retail 0x1e3b00
void function_1e3b00(long object_index, long mode, point3f const *reference,
	void const *unknown0, void const *unknown1, point3f *position)
{
	s_object_position_view *object = (s_object_position_view *)ai_object_get(object_index);
	const long *mode_reference = &mode;
	const point3f *const *origin_reference = &reference;
	const void *const *local_41f0cc = &unknown0;
	const void *const *offset_reference = &unknown1;
	if ((1 << object->type) & 3)
		function_cb500(object_index, (short)*mode_reference, *origin_reference,
			(const vector3f *)*local_41f0cc, (const real *)*offset_reference, position);
	else
		*position = object->position;
}

struct s_unit_state_c6ef0;
void function_c6ef0(s_unit_state_c6ef0 *state);
void function_c6de0(long object_index, void *control);
void function_118e80(long object_index, vector3f *forward);
void __stdcall function_cbf60(long unit_index, bool active);

/* the standard marker (docs/DECOMPILING.md): (1) with it this body matches
   byte for byte (`ret 4`); without it LTCG passes the actor in eax, and
   when 0x2902b0 and 0xb7b40 are built it passes it in ecx even with the
   actor's address taken. (2) retail holds no reference to 0x1e1250's
   address, and its one caller (0x1e0d50) is LTCG code that pushes the
   argument (`push edi; call 0x1e1250`). (3) tried: taking the argument's
   address (matches alone, lost once the object cycle is built). */
// @retail 0x1e1250 standard
void __stdcall function_1e1250(long actor_index)
{
	s_actor_view *actor = actor_get(actor_index);
	long unit_index = actor->unknown018;
	if (unit_index != NONE)
	{
		*(long *)((byte *)ai_object_get(unit_index) + 0x12c) = NONE;
		actor->unknown018 = NONE;
		function_cbf60(unit_index, false);
	}
}

void __stdcall function_1e1a00(long actor_index, bool value);
long function_1e4990(long character_index);

// @retail 0x1e0d50
void function_1e0d50(long actor_index, long unit_index)
{
	long const *unit_reference = &unit_index;
	s_actor_view *actor = actor_get(actor_index);
	s_ai_object *unit = ai_object_get(*unit_reference);
	long old_actor = *(long *)((byte *)unit + 0x12c);
	if (old_actor != actor_index)
	{
		if (old_actor != NONE)
			function_1e1a00(old_actor, 0);
		if (actor->unknown018 != NONE)
			function_1e1250(actor_index);
		actor->unknown018 = *unit_reference;
		*(long *)((byte *)unit + 0x12c) = actor_index;
		function_cbf60(*unit_reference, true);
	}
}

struct s_actor_unit_state
{
	long name;
	short mode;
	byte field_6[0x28 - 6];
	vector3f forward;
	vector3f first;
	vector3f second;
	byte field_4c[0x7c - 0x4c];
};

struct s_actor_unit_vectors
{
	byte field_0[0x168];
	vector3f first;
	byte field_174[0x18c - 0x174];
	vector3f second;
};

// @retail 0x1e31b0
void function_1e31b0(long unit_index)
{
	s_actor_unit_state state;
	function_c6ef0((s_unit_state_c6ef0 *)&state);
	state.name = 0x6000085;
	state.mode = 1;
	function_118e80(unit_index, &state.forward);
	s_actor_unit_vectors *unit = (s_actor_unit_vectors *)ai_object_get(unit_index);
	state.first = unit->first;
	state.second = unit->second;
	function_c6de0(unit_index, &state);
	function_cbf60(unit_index, false);
}

struct s_actor_object_query
{
	long definition_index;
	dword flags;
	byte field_8[0x14 - 8];
	long parent_index;
	byte field_18[0x28 - 0x18];
	dword location[2];
	byte field_30[0x70 - 0x30];
	vector3f direction;
	byte field_7c[0xaa - 0x7c];
	byte type;
	byte field_ab[0x134 - 0xab];
	dword flags134;
	byte field_138[2];
	short link_offset;
};

PRIVATE inline s_actor_object_query *actor_query_object(long index)
{
	return (s_actor_object_query *)ai_object_get(index);
}

PRIVATE inline long actor_query_root(long index)
{
	long result = NONE;
	while (index != NONE)
	{
		result = index;
		index = actor_query_object(index)->parent_index;
	}
	return result;
}

struct s_actor_object_sample
{
	point3f point;
	point3f center;
	vector3f direction;
	dword location[2];
	vector3f velocity;
};

// @retail 0x1e3a00
void function_1e3a00(long object_index, s_actor_object_sample *sample)
{
	s_actor_object_sample *const *sample_reference = &sample;
	point3f *point_output = &(*sample_reference)->point;
	s_actor_object_query *object = actor_query_object(object_index);
	function_b9dd0(object_index, &(*sample_reference)->center);
	(*sample_reference)->direction = object->direction;
	if ((1 << object->type) & 3)
	{
		s_object_marker marker;
		function_b8d30(object_index, 0x4000095, &marker, 1, false);
		memcpy(point_output, &marker.matrix.position, sizeof(point3f));
	}
	else
		memcpy(point_output, &point_output[1], sizeof(point3f)); /* the sample's center follows its point */
	function_ba1d0(object_index, &(*sample_reference)->velocity, NULL);
	s_actor_object_query *root = actor_query_object(actor_query_root(object_index));
	struct location_pair { long first; long second; };
	*(location_pair *)(*sample_reference)->location = *(location_pair *)root->location;
}

struct s_actor_variant_entry
{
	long value;
	short variant;
	byte field_6[6];
};

struct s_actor_variant_definition
{
	byte field_0[0x2c];
	long count;
	s_actor_variant_entry *entries;
};

// @retail 0x1e06b0
long function_1e06b0(long tag_index)
{
	long result = 0;
	s_actor_variant_definition *definition = (s_actor_variant_definition *)g_4e3b44[tag_index & 0xffff].bytes;
	if (definition->count == 1)
	{
		result = definition->entries[0].value;
		goto done;
	}
	if (definition->count > 1)
	{
		signed char counts[64];
		signed char choices[64];
		memset(counts, 0, sizeof(counts));
		s_actor_iterator iterator;
		function_x66da2b(&iterator, false);
		s_actor_view *actor;
		while ((actor = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
		{
			if (*(long *)((byte *)actor + 0x54) == tag_index && actor->unknown018 != NONE)
			{
				signed char variant = *((signed char *)ai_object_get(actor->unknown018) + 0xb1);
				if (variant >= 0 && variant < 64)
					counts[variant]++;
			}
		}
		short choice_count = 0;
		short minimum = 32767;
		for (short i = 0; i < definition->count; i++)
		{
			s_actor_variant_entry *entry = &definition->entries[i];
			if (entry->value && entry->variant >= 0 && entry->variant < 64)
			{
				short count = counts[entry->variant];
				if (count < minimum)
				{
					choices[0] = (signed char)i;
					choice_count = 1;
					minimum = count;
				}
				else if (count == minimum)
					choices[choice_count++] = (signed char)i;
			}
		}
		if (choice_count > 0)
		{
			dword seed = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
			g_4e7408->unknown0 = seed;
			short choice = (short)(((seed >> 16) * choice_count) >> 16);
			result = definition->entries[choices[choice]].value;
		}
	}
done:
	return result;
}

void swarm_set_team(long perception_index, short team);
void swarm_reset_orientations(long perception_index);

// @retail 0x1e1150
void function_1e1150(long actor_index, short team)
{
 s_actor_view *actor = actor_get(actor_index);
 actor->unknown024 = team;
 if (actor->unknown007)
  swarm_set_team(*(long *)actor->unknown01c, team);
 else if (actor->unknown018 != NONE)
  *(short *)((byte *)ai_object_get(actor->unknown018) + 0x138) = team;
}

// @retail 0x1e3240
void function_1e3240(void)
{
 s_actor_iterator iterator;
 function_x66da2b(&iterator, true);
 while (function_1e46c0(&iterator))
 {
  s_actor_view *actor = actor_get(iterator.actor_index);
  if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
   swarm_reset_orientations(*(long *)actor->unknown01c);
  else
   function_1e31b0(actor->unknown018);
  actor->unknown008 = 1;
 }
}

void function_1f86a0(long actor_index);

// @retail 0x1e22d0
void function_1e22d0(long actor_index, bool conditional)
{
 s_actor_view *actor = actor_get(actor_index);
 function_2628f0(actor_index, g_470fa0);
 if (actor->unknown4ac == 4 || actor->unknown4ac == 5 || actor->unknown4ac == 6)
 {
  if (!conditional || (*(word *)((byte *)actor + 0x4ba) & 0x8000))
   function_1f86a0(actor_index);
 }
 byte *entry = (byte *)actor + 0x402;
 long count = 4;
 do
 {
  if (!conditional || (*(word *)(entry + 2) & 0x8000))
   *(s_reference *)entry = g_470fa0;
  entry += 6;
 } while (--count);
 for (short slot_index = 0; slot_index <= actor->current; ++slot_index)
 {
  short type = actor->slots[slot_index].type;
  s_slot_handler volatile const *handler = g_46eeb8[type];
  t_slot_notify callback = handler->notify34;
  if (callback)
   callback(actor_index, slot_index < 4 ? &actor->slots[slot_index] : NULL, conditional);
 }
}

void swarm_forget_prop(long perception_index, long index);

// @retail 0x1e2150
void function_1e2150(long actor_index, long object_index)
{
 s_actor_view *actor = actor_get(actor_index);
 if (*(long *)((byte *)actor + 0x338) == object_index)
  *(long *)((byte *)actor + 0x338) = NONE;
 if (*(long *)((byte *)actor + 0x32c) == object_index)
  *(long *)((byte *)actor + 0x32c) = NONE;
 if (actor->unknown344 == object_index)
  actor->unknown344 = NONE;
 if (actor->unknown368 == object_index)
 {
  actor->unknown368 = NONE;
  actor->unknown358 = 0;
 }
 if (*(short *)((byte *)actor + 0x722) == 1 && *(long *)((byte *)actor + 0x724) == object_index)
 {
  *(long *)((byte *)actor + 0x724) = NONE;
  *(short *)((byte *)actor + 0x722) = 0;
 }
 if (*(long *)((byte *)actor + 0x7e0) == object_index)
  *(long *)((byte *)actor + 0x7e0) = NONE;
 if (actor->unknown3b4 == object_index)
  actor->unknown3b4 = NONE;
 if (actor->unknown4ac == 7 && *(long *)((byte *)actor + 0x4b8) == object_index)
 {
  actor->unknown4ac = 0;
  actor->unknown4e4 = NONE;
 }
 if (*(short *)((byte *)actor + 0x688) == 1 && *(long *)((byte *)actor + 0x68c) == object_index)
  *(long *)((byte *)actor + 0x68c) = NONE;
 if (*(short *)((byte *)actor + 0x6a0) == 1 && *(long *)((byte *)actor + 0x6a4) == object_index)
  *(long *)((byte *)actor + 0x6a4) = NONE;
 if (*(short *)((byte *)actor + 0x6b0) == 1 && *(long *)((byte *)actor + 0x6b4) == object_index)
  *(long *)((byte *)actor + 0x6b4) = NONE;
 if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
  swarm_forget_prop(*(long *)actor->unknown01c, object_index);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_release callback = g_46eeb8[slot->type]->release28;
  if (callback) callback(actor_index, i < 4 ? &actor->slots[i] : NULL, object_index);
 }
}

void swarm_creatures_clear_index20(long perception_index);

/* empties a location: no leaf or cluster, in the current structure bsp */
PRIVATE inline void location_make_empty(s_location *location)
{
 location->leaf_index = NONE;
 location->cluster_index = NONE;
 location->bsp_index = g_4686c4;
}

/* the walk over an actor's props: 0x26be00 starts it, 0x26be30 returns the
   next prop (NULL at the end) */
struct s_node_view;
void function_26be00(long actor_index, s_iterator *iterator);
s_node_view *function_26be30(s_iterator *iterator);

// @retail 0x1e23c0
void function_1e23c0(long actor_index)
{
 s_actor_view *actor = actor_get(actor_index);
 location_make_empty((s_location *)((byte *)actor + 0x250));
 *(bool *)((byte *)actor + 0x278) = false;
 *(long *)((byte *)actor + 0x28c) = NONE;
 actor_reset_state(actor_index);
 if (actor->unknown4ac == 2)
  *(long *)((byte *)actor + 0x4c8) = NONE;
 *(long *)((byte *)actor + 0x4fc) = NONE;
 if (actor->unknown007 && *(long *)actor->unknown01c != NONE)
  swarm_creatures_clear_index20(*(long *)actor->unknown01c);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_proc callback = g_46eeb8[slot->type]->proc30;
  if (callback)
   callback(actor_index, i < 4 ? &actor->slots[i] : NULL);
 }
 s_iterator iterator;
 function_26be00(actor_index, &iterator);
 s_prop_node_view *prop;
 while ((prop = (s_prop_node_view *)function_26be30(&iterator)) != NULL)
 {
  /* retail calls 0x25d690 here rather than inlining it */
#pragma inline_depth(0)
  byte *prop_block = (byte *)function_25d690((s_prop_datum *)prop);
#pragma inline_depth()
  byte *view = (byte *)prop_node_view(prop);
  location_make_empty((s_location *)(prop_block + 0x28));
  *(long *)(prop_block + 0x44) = NONE;
  if (view && *(short *)(view + 0x70) == 1)
  {
   view[0x68] = false;
   view[0x69] = false;
   view[0x4c] = false;
   *(short *)(view + 0x70) = 0;
   view[0x6c] = true;
   view[0x6d] = true;
   view[0x88] = false;
   *(short *)(view + 0x90) = 0;
   *(short *)(view + 0x8a) = 0;
  }
 }
}

bool function_1e13f0(long actor_index);
void function_203d70(long actor_index, short squad_index, bool keep_team);
void function_203ed0(long actor_index, bool keep_count);
void function_203f60(long actor_index);
void function_203fb0(long actor_index);

// @retail 0x1e3400
void function_1e3400(long actor_index, short squad_index)
{
 s_actor_view *actor = actor_get(actor_index);
 function_1e22d0(actor_index, false);
 if (*((byte *)actor + 0xa))
  function_203fb0(actor_index);
 else if (*(long *)((byte *)actor + 0x30) != NONE)
  function_203ed0(actor_index, false);
 if (function_1e13f0(actor_index))
 {
  if (squad_index != NONE)
  {
   function_203d70(actor_index, squad_index, true);
   return;
  }
 }
 else
  *(long *)((byte *)actor + 0x34) = squad_index;
 function_203f60(actor_index);
}

void function_26def0(long actor_index, long owner_index);
void function_25aa10(long actor_index, long object_index);

// @retail 0x1e17d0
void function_1e17d0(long actor_index, long object_index)
{
 const long *object_reference = &object_index;
 s_actor_view *actor = actor_get(actor_index);
 if (actor->unknown26c == *object_reference)
 {
  actor->unknown26c = NONE;
  *((byte *)actor + 0x266) = 0;
  *((byte *)actor + 0x267) = 0;
  *(short *)((byte *)actor + 0x270) = 0;
 }
 if (actor->unknown274 == *object_reference)
 {
  actor->unknown274 = NONE;
  actor->unknown268 = 0;
 }
 if (*(long *)((byte *)actor + 0x5ac) == *object_reference)
 {
  *(long *)((byte *)actor + 0x5ac) = NONE;
  *(short *)((byte *)actor + 0x5b0) = NONE;
 }
 if (*(long *)((byte *)actor + 0x348) == *object_reference)
  *(long *)((byte *)actor + 0x348) = NONE;
 if (*(long *)((byte *)actor + 0x300) == *object_reference)
 {
  *(long *)((byte *)actor + 0x300) = NONE;
  *(short *)((byte *)actor + 0x304) = 0;
 }
 if (*(long *)((byte *)actor + 0x2e8) == *object_reference)
  *(long *)((byte *)actor + 0x2e8) = NONE;
 if (*(long *)((byte *)actor + 0x7e4) == *object_reference)
  *(long *)((byte *)actor + 0x7e4) = NONE;
 if (*(long *)((byte *)actor + 0x3f8) == *object_reference)
  function_26def0(actor_index, NONE);
 for (short i = 0; i <= actor->current; ++i)
 {
  s_slot *slot = &actor->slots[i];
  t_slot_release callback = g_46eeb8[slot->type]->release2c;
  if (callback) callback(actor_index, i < 4 ? &actor->slots[i] : NULL, *object_reference);
 }
 function_25aa10(actor_index, *object_reference);
}

void function_26dc20(long actor_index);
void function_11bed0(s_location *location, point3f const *point);
struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_actor_leaf_view
{
 short cluster;
 byte field_2[6];
};
struct s_actor_bsp_view
{
 byte field_0[0x30];
 s_actor_leaf_view *leaves;
};

bool actor_update_bsp_locations(long actor_index);

extern void *g_5047f4;
void function_258600(long actor_index);
void function_2694d0(long clump_index, long actor_index);
void record_pool_release(s_record_pool *pool, long index);
void function_1e1740(long actor_index, long object_index);
void function_26dc20(long actor_index);
void swarm_leader_detach(long actor_index);

// score 59 (was 59): r143 and m3 lane 8 r221 unchanged (r221's line just above the marker). Earlier 59 (was 236): m3 lane 11 r141 kept the memory walk as a plain loop testing through the cached actor with the release steps as retail expands them, the prop walk through 0x26be00/0x26be30 with the node re-read after the callback, the iterator compared against the cached actor and `value` as a bool (see the end of this note). Left: the slot loop forms the slot address ahead of the handler loads where retail forms it after the test, the definition bit is tested in place where retail stores it in a byte first, and the compare's operand order. Earlier 236 (was 236): actor pool register, displacement spill and invitation countdown still differ. M3 lane 3 r268
// MiniMax G (11-20): no new form, no lower score (236): the raw value turned into a bool before the 0x203ed0 call rises to 289, the prop walk through prop_node_get rises to 280 and to 274 with the cached-actor iterator compare, and calling the real 0x26ed40 instead of expanding it rises to 348 (910 bytes); the offset-first head form compiles to the same bytes as the kept body and was dropped; the body is still 22 bytes short of retail's 980 because retail expands 0x26ed40 into it (BLOCKED-BY 0x26ed40, src/unknown_26e370.cpp), and the head still loads the pool global into EAX where retail keeps it in ECX and holds the scaled actor index in EBX where retail keeps it in EAX.
// MiniMax B (11-20): no new form, no lower score (236): the head still loads the pool global into EAX where retail keeps it in ECX and holds the scaled actor index in EBX where retail keeps it in EAX, and the body is still 22 bytes short of retail's 980 because retail expands 0x26ed40 into it (BLOCKED-BY 0x26ed40, src/unknown_26e370.cpp); the known 216 form holds the definition bit in a byte flag set by one test and read by the next, which is kept only with a byte match; this round's permuter (2 tries, 339.9 s) reaches 205 differing instructions against its baseline of 219 only through a constant-only local and a bool flag set by one test, both banned.
// MiniMax G (11-20): no new form, no lower score (236): the head still loads the actor pool global into eax where retail keeps it in ecx and holds the scaled actor index in ebx where retail keeps it in eax, and the tail still keeps the pool in edi where retail keeps it in ebp; the body is still 22 bytes short of retail's 980 because retail expands 0x26ed40 into it.
// (fast with 0xf0fd0, 0x1fb360 and the eleven matched callers/neighbours, all MATCH): the prop walk through 0x26be00/
// 0x26be30 with the view index read through a prop_node_get pointer after the callback 285 (948 bytes); the
// actor-iterator test as `actor != other` against the cached actor 308 (942 bytes); the participant slot read at each of
// the two stores 282 (974 bytes); the slot loop through a handler pointer (`handler->stop` tested and called) 236 (same
// code); the prop walk and the cached compare together 299. Without the expanded 0x26ed40 (its file keeps it a call)
// and the bool parameter (one declaration sits in src/unknown_1e3920.cpp, avoided this round) every form here rises:
// BLOCKED-BY 0x26ed40.
// MiniMax G (11-18): no new form, no lower score (236): the head still loads the actor pool into EAX where retail
// loads it into ECX and keeps the scaled index in EAX where this build keeps it in EBX, and the body is still 22
// bytes short of retail's 980 because retail expands 0x26ed40 into it; that expansion is settled in
// src/unknown_26e370.cpp, which this lane may not edit, so the body stands.
// MiniMax E (11-14): no new form, no lower score (236): the first difference is still at +0x3, where the actor
// pool goes into EAX and retail puts it in ECX; the slot loop is still rotated by a jump with an alignment pad
// retail has not, retail's memory walk still indexes the record as actor base + 12*i in EDX and re-reads the slot
// after the store, and the body still expands the callee at 0x26ed40 that lives in another file:
// BLOCKED-BY 0x26ed40.
// MiniMax B (11-11), NOT kept (coordinator): 236 to 216 (plain, 966 bytes) holds the definition bit in a byte flag set
// by tests (`definition_counted`), a match-only form. The lead as written: the definition bit is now shifted and kept in
// the dead parameter slot as retail does, and the counts loop and the tail follow it; still differs:
// ours loads the actor pool into EAX and keeps the scaled index in EBX (retail: ECX and EAX) and
// spills the actor pointer to a stack slot retail has not; the slot loop is rotated with an extra
// jump and its slot displacement is in ECX (retail: EDX); the memory-entry walk keeps its countdown
// in EBX and folds the entry pointer into the offset (retail: an entry pointer in EBX, the byte
// offset in EDX and the countdown in a stack slot); the prop entry address is folded into the loads
// instead of being materialised and re-derived after the callback; the iterator compare reloads the
// actor pointer instead of comparing against the cached one; and the second parameter becomes a
// Boolean before the 0x203ed0 call where retail pushes the raw value.
// MiniMax D (11-15): 216 (was 236): the definition bit is now tested through a named flag after a separate
// null check, so the tested bit is kept in a byte slot and the branch reads it from there, as retail does.
// What still differs: the prologue loads the record pool into EAX and keeps the scaled index in EBX (retail
// loads the pool into ECX and the index into EAX and leaves the actor pointer unspilled in EBP) and this
// build spills the actor pointer to a stack slot retail has not; the memory-entry walk keeps its countdown
// in EBX and folds the entry pointer into the offset (retail: an entry pointer in EBX, the byte offset in
// EDX and the countdown in a stack slot); and the body is 22 bytes short of retail's 980 because retail
// expands 0x26ed40 into it (src/unknown_26e370.cpp).
// MiniMax I (11-17): no new form, no lower score (236): the head still loads the actor pool into EAX where retail
// loads it into ECX and keeps the scaled index in EAX where this build keeps it in EBX, and this build still spills
// the actor pointer to [esp+0x14] where retail holds it in EBP; the definition-bit test written as a nested if
// compiles to the same bytes as the && form, and the permuter's best variant (300 s, two tries, 205 differing
// instructions on the reduced image against a baseline of 219) only reaches it through a constant local and a bool
// flag set by one test, both banned, so the body stands.
// MiniMax A (11-18): no new form tried this round, no lower score (236): the head still loads the actor pool into
// EAX where retail loads it into ECX and keeps the scaled index in EBX where retail keeps it in EAX, and this build
// still spills the actor pointer to a stack slot retail holds in EBP; the body is also still 22 bytes short of
// retail's 980 because retail expands 0x26ed40 into it, and that helper's body lives in src/unknown_26e370.cpp,
// which this lane may not edit: BLOCKED-BY 0x26ed40.
// Earlier 236 (was 236): actor pool register, displacement spill and invitation countdown still differ.
// Earlier: no new form and no lower score; the prologue still loads the actor pool into ECX and
// keeps the scaled index in EAX where retail loads the pool into EAX and keeps the index in EBX, and the body
// is 22 bytes short of retail's 980 because retail expands 0x26ed40 into it (src/unknown_26e370.cpp).
// MiniMax E (11-16): no new form, no lower score (236); the body is still 22 bytes short of retail's 980 because
// retail expands 0x26ed40 into it (src/unknown_26e370.cpp, BLOCKED-BY), and the prologue still loads the actor pool
// into EAX where retail loads it into ECX.
// MiniMax B (11-14): no new form, no lower score (236); retail's first slot loop builds the `slot_index < 4` argument with
// setge/dec/and and its memory loop is a countdown over the actor base, while ours branches on the ternary and re-reads the
// actor through the pool; a permuter run was stopped at its time limit without reporting a variant.
// Earlier at 236 from 281, frame 0x1c versus 0x20 (earlier body), entry actor displacement and cleanup stack homes remain;
// at 236 the prologue registers, the memory walk (retail's inlined joint_decline: offset in EDX, count in a stack
// slot) and the iterator compare against the cached actor differ.
// MiniMax I (11-09): no new form, no lower score; the body has retail's shape but the allocation differs from the first instruction: retail loads the actor table into EAX and keeps the scaled actor index in EBX with the actor pointer in EBP, ours loads the table into ECX and keeps the index in EAX; the memory walk's helper 0x26ed40 is another lane's file: BLOCKED-BY 0x26ed40 (src/unknown_26e370.cpp).
// this round: no new form and no lower score; the prologue still loads the actor
// pool into EAX and keeps the byte offset in EBX where retail uses ECX and EAX, the memory-entry walk
// still keeps its count in EBX and folds the entry pointer into the offset, and the body is 22 bytes
// short of retail's 980.
// MiniMax S (11-09): two more forms for the memory-entry walk: the type test through the cached actor
// with every other field read through a fresh actor_get gives 344, and the whole walk through the
// cached actor gives 308, against the unchanged 236. Both put the entry pointer in EBX as retail has
// it, but they fold the byte offset into that pointer and lose retail's offset in EDX with the count
// in a stack slot, so the body stays.
// MiniMax S (11-09): the actor view is the cached local for the fields retail reads through EBP (0x7c,
// the prop-list clear, 0x3f4, 7, 0x1c, 0x18 and 0x54) and the prop entry is reached through one pointer;
// the frame is 0x20 and the score is 236 (was 281). Still differs: the prologue loads the actor pool into
// EAX and keeps the byte offset in EBX (retail ECX and EAX), the memory-entry walk keeps its count in
// EBX and folds the entry pointer into the offset (retail a pointer in EBX, the offset in EDX and the
// count in a stack slot), and the prop entry address is folded into the load instead of being
// materialised before it.
// MiniMax H (11-07): no new form, no lower score; the prologue still differs (retail loads the actor pool into ecx and the byte offset into eax, ours into eax and ebx, with a frame of 0x1c against 0x20) and the memory walk with it: retail walks an entry pointer in ebx with the byte offset in edx, the count in a stack slot and the actor re-derived inside the body, and it reads the slot field twice, while ours keeps the count in ebx, the offset in esi and the slot in a local.
// MiniMax I (11-06): the actor view inlined at every use (310) and a local copy of the actor
// index (same code) were tried; the memory loop as a countdown, with the slot read twice and with
// the fields read through the indexed form gave 268, 286 and 363; no lower score.
// Earlier: Frame 0x20 agrees; early actor-pool allocation, cleanup and slot homes remain.
// frame 0x1c versus 0x20, early actor/pool allocation,
// cleanup countdown and slot homes remain.
// Frame 0x1c versus 0x20; entry actor/displacement allocation and cleanup countdown/slot homes remain.
// Round 141: the memory loop is the existing joint_decline (0x26ed40) inlined in retail (test through the
// cached actor, body re-derived, participant index re-read per store), BLOCKED-BY 0x26ed40: its /Ob1
// file keeps it out of line (a call there 317/343). The tail block (flag 9 clear, as 0x1c7bb0/0x1e3920
// write it) takes one fresh actor lookup as retail does: 277 (a shared helper in slot_handler.h gives
// the same, but any new inline function there costs 0x73360). `value` is a bool in retail (pushed
// through to 0x203ed0 unconverted, tested as a byte): 275 with it, but src/unknown_1bba20.cpp declares
// the parameter long (MiniMax file).
// Round 139's actor local + walk helpers with the bool and the shared tail 306 (942 bytes, aligned 114),
// with the memory test through the local 299 (974 bytes) or re-derived and per-store reads 304.
// Round 139: retail keeps the actor pointer in EBP from the entry (slot loop, current store, 0xa/0x7c
// tests, prop-list clear, iterator compare, 0x3f4/7/0x1c/0x18/0x54 reads) and re-derives it from the
// pool and the saved offset only in the memory-slot body, the prop walk start, the inlined 0x1e1250,
// the tag lookup and the tail. An actor local at those first uses with the prop walk through the
// existing 0x26be00/0x26be30 gives retail's length (980) and 270, but mixes the local with direct
// re-reads (kept only on a match); the local everywhere 343 (904 bytes, 127 sequence-aligned
// differences against 192); local without the walk helpers 316/289; cached memory test 300; tail
// through the local 323. The tag walk now calls the existing 0x1e4990 (identical code).
// Earlier: Frame 0x20, early actor offset and cleanup/scan homes remain; slot element
// 290; scan for-header 338; repeated input predicate 305; retained reference pool 290; word slot
// Frame 0x20 agrees; early actor displacement, cleanup countdown and scan homes remain.
// Earlier slot element 290; scan for-header 338; repeated input predicate 305; retained reference pool 290; word slot
// with fresh lookup 304.
// frame 0x20 agrees; actor-offset storage, cleanup counter and slot reloads differ.
// frame 0x20 agrees; early actor displacement, cleanup and slot homes remain.
// frame 0x20 agrees; early actor-offset retention and cleanup/slot homes differ.
// Final actor lifetime 290; actor/prop-handle names 290/290; byte/signed-byte cleanup 301/301;
// native memory cursor with fresh lookup 344; final lifetime plus actor name 290; direct stop member 290.
// frame 0x20 agrees; actor-offset storage, cleanup counter and slot reloads differ.
// Ordinary actor/iterator locals 330; direct parameters 330; guarded callback 324; fresh memory lookup 291;
// live memory-slot reads 326; slot reuse at lookups 301; callback plus fresh lookup 290; direct combined
// parameters 290.
// actor offset retention, fresh memory-slot reads and cleanup homes remain.
// Short cleanup index 301; pretested cleanup loop 560; entry iterator declaration 290;
// direct current-slot store 290; ancestor failure-first continuation 290. Original body retained.
// Fresh-lookup live slots 300; unsigned cleanup index 290; iterator local home 290; Boolean bit
// test 290; native masked slot 308.
// Round 122: Shared marked scan 289; native actor for 290; ancestor cursor scope 290; empty-memory
// continue 290; exact cleanup endpoint 290.
// Guarded-callback/direct-parameter context: word slot at memory/prop/early/tag/all
// resolutions 302/312/327/313/301;
// natural long slot at all/early/memory resolutions 307/314/311;
// every original pool refresh and full callback handle retained.
// Cleanup/dispatch local homes separately and together 290 each; outer/shared prop workspace
// 290/290; released reference home 290; slot before entry-address reads 288 (two-point gain
// rejected). Original body retained.
// Real signed-word memory field with promoted/native/per-use slot 290/304/300;
// promoted/native slot before entry 288/304; per-use slot plus entry stop ordinal 300;
// direct entry query with promoted/per-use field 290/300. Two-point gain rejected; record declaration restored.
// Complete actor getter observations, full-handle/native-slot 283/314; with shared prop
// byte displacement 282/315; with live count record 283/314; both additions 282/315.
// Full-handle actor getter plus reused prop displacement kept; getter alone gains only seven.
// Round 168: Complete getter graph: short/byte/signed-byte cleanup 282 each; live slots 284; live
// slots with those ordinals 284 each; shared callback element 284. Opening body restored.
// Round 180: Shared cleanup getter/native prop fields/both 282 each; terminal indexed for
// alone and with those three forms 314 each; signed stop while on the complete getter graph 299.
// Existing complete count-record fields, terminal do with long/short/byte/unsigned
// indices 282 each; terminal for 314/282/282/314. Original body and record view restored.
// Round 344: phase-specific stop/memory receivers 304/286; shared cleanup/terminal ordinal 282;
// stop/memory pair 293; stop/memory with shared ordinal 304/286; both with ordinal 293;
// both with byte shared ordinal 293. All phase snapshots and ordinal changes restored.
// Round 143: 0x26ed40 is open now, but its file is another lane's this round; the bool parameter
// still needs src/unknown_1bba20.cpp. Short memory index 277 (identical); the memory loop as a for
// loop 565 (1204 bytes); the counts loop as a for loop 309; both 565; the `current` store as a plain
// `= NONE` 277 (identical, kept: the word cast was not needed).
// Round 26 (lane 9): a __forceinline declaration of 0x26ed40 in this file with the memory loop calling
// it 324 (917 bytes; it stays a call, so the helper's own file must allow inlining); flag test as a bool
// cast 291; actor local in the slot loop 309; memory test without the entry pointer 277; prop view index
// re-derived 278. Still BLOCKED-BY 0x26ed40 (src/unknown_26e370.cpp) and the bool parameter
// (src/unknown_1bba20.cpp).
// Round 31: still BLOCKED-BY 0x26ed40 (lane file) and the bool parameter (src/unknown_1bba20.cpp).
// Prop walk through 0x26be00/0x26be30 alone (fresh pool reads after the callback) 298 (996 bytes);
// tail read per use 282; prop record re-derived per use 278; named bool for the definition bit 277;
// the bit test as `(bool)((x >> 3) & 1)` 291 (retail: shr/and al, stored in the dead index slot).
// Earlier: the frame is 0x1c instead of 0x20, the actor byte offset is kept in EBX
// instead of EAX and the slot index in ECX instead of EDX, and the cleanup loop keeps its
// countdown in a register instead of a stack slot.
// Round 167: entry cleanup/scan/both constants 285/322/322; uninitialized
// callback/memory/both workspaces 285/285/285; both constants and workspaces
// alone/with final receiver 322/322. Entry scan recovers frame 0x20, not allocation.
// Native unsigned scan late/early 285/322; prop displacement/complete handles
// 285/285; early scan with displacement/handles 322/322; all domains late/early
// 285/322. Original declarations, domains, observations and executable body restored.
// Round 27 reduced preview 261 with a full-image caveat: cleanup pointer/offset/stack countdown 283/981;
// with shared prop walk 297/964; explicit actor offset/retained pointer 311/958; with countdown 310/958;
// native bool predicate 265/983; with countdown 304/999; declarations reordered 320/1012;
// countdown for 283/981, ordinary 285/981. Opening body restored.
// Round 250: existing cleanup helper with fresh admission and long/short/byte/unsigned
// indices 335/335/325/335; cached admission with those domains
// 347/347/346/347. Shared declaration unchanged; original body restored.
// Shared inline/forced declaration diagnostics, each long/short/byte/unsigned,
// 330/303/285/330 and 330/303/285/330; declarations restored.
// Round 29: Existing record/current-datum reference scans, normal/entry workspace declarations and both workspace names: 266 each against unchanged fast 261; ordinary baseline 285 retained. Original scan and include restored.
// Round 109: joint_decline declared inline and called from the memory loop 330 (it then also inlines
// into 0x1bead0 and 0x26ef70, which lose their matches); on the fast image (261 there, caveat): the
// memory test through the cached actor 352, the participant index re-read per store 287, both 371,
// a short memory index 261. The bool `value` needs every declaration changed, one of them in
// src/unknown_1e3920.cpp (another lane's this round).
// Round 383 (fast control 261, ordinary 285): native invitation/destination/both records
// 261/261/261; per-store participant field with invitation/destination/both/both plus short
// ordinal/both plus destination reference 287 each. Original types, field reads and body restored.
// Round 222: Native callback-handler ownership, conditional iterator-result declaration,
// cleanup-only workspace scope, all seven subsets and readonly handler: eight 261 fast
// against unchanged fast 261 (ordinary 285). Original executable body restored.
// Self-exclusion by native pool-slot identity, word/long/dword/unsigned-int
// entry carriers 288/312/312/312; uniformly reused getter carriers 294/313/313/313
// against unchanged fast 261 (ordinary 285). Original pointer test, complete
// handles, getter observations and executable body restored.
// DH round 230 (fast control 261, ordinary 285): iterator actor/handle/both outputs initialized
// 279/274/274; complete zero aggregate 301; their entry-declaration forms 279/274/274/326.
// All restored; frame 0x1c/0x20, actor/displacement allocation and cleanup countdown remain.
// DH round 234 (fast control 261, ordinary 285): direct callback-field observations with
// native iterator/cleanup lexical scopes and unsigned cleanup ordinal, eight forms 261 each.
// Original body restored; frame 0x1c/0x20 and actor/displacement/countdown homes remain.
// Round 186 (fast control 261, ordinary 285): participant pool/readonly pool/guarded data/pool-data
// observations with original/const callback locals 262/262, 262/262, 261/261, 262/262.
// Original pool observations, callback local and body restored; frame and cleanup homes remain.
// DH round 236: terminal three-slot table ordinal initialized at function entry or before
// the tag query, long/short/byte/dword in each lifetime: eight 261 fast, equal to unchanged
// fast control (full 285). Original ordinal lifetime and entire executable body restored;
// frame 0x1c/0x20, actor displacement and cleanup countdown homes remain.
// Y423: Absent-stop continuation with empty-memory continuation, false-count
// dispatch first and ordinary-unit branch first, all eight subsets 261 fast
// against unchanged fast 261 (ordinary 285). Original complete body restored.
// Round 179 (plain 285; an LTCG link of this file with the callee closure, globals and the callers' files
// reproduces the full listing exactly, where --fast does not): `value` as bool at the definition and all
// eight declarations 286 plain; memory test through the entry actor with the body re-derived 1028 bytes,
// with the participant index read per store 1044; joint_decline called (stays a call, 900 bytes); the
// participant index read per store alone 996 bytes; the duplicate memory pointer re-derivation dropped:
// identical code, kept. Without unknown_272b70's object (0x272b70 unresolved, so the 0x258600 ->
// 0x258540 -> 0x258480 chain is compiled with unknown clobbers) the entry and slot loop take retail's
// registers (actor EBP, index EDI, offset spilled to [esp+0x10]): the allocation follows what LTCG knows
// about 0x258600's chain. The memory loop is joint_decline inlined, which its /Ob1 file prevents.
// Round 184 (lane 3, fast control 261, plain 285): the iterator compare against the entry actor 285;
// the 0x7c test and argument through the entry actor 287 (964 bytes); the tail reads (0x3f4, 7, 0x1c,
// 0x18, 0x54) through it 291 (974 bytes); the prop-list clear through it 290; 0x1e1740's slot-local
// loop 261 (same code). 0x258600 takes the actor index in EDI in both builds. Still BLOCKED-BY
// 0x26ed40 (inlined in retail, kept out of line by its file's flags).
// Round 132 (lane 6, plain): the prop-owner scan through the existing record-pool iterator
// (data_iterator_next_inlined) in place of the hand-expanded index scan and NULL test 281 (kept;
// 981 bytes; the found block now sits out of line as in retail); on that: the prop walk through
// prop_node_get at each use 284; through one prop_node_get pointer local 290 (965 bytes); the
// participant index read at each store 299 (997 bytes); the scan through the datum iterator 281
// (same code); every actor read through a fresh actor_get with no actor local 310 (1012 bytes, but
// aligned 153 against 190: frame 0x20, index EDI, callback ECX and the spilled offset are then all
// retail's, and only retail's cached actor pointer in EBP is missing).
// Round 67 (lane 16, plain each): every function in 0x258600's callee closure (0x258540, 0x258480, 0x16ba40) is
// matched, so that lead is not a stub or unmatched body there; 0x2628f0 (a direct callee) and deeper 0x1e3b60,
// 0xcc810, 0x203bf0 and 0x28f8d0 are unmatched. A named bool for the definition bit (`bool counted = definition
// && ...; if (counted)`) 285 (identical code); the memory test through the entry actor with the body re-derived
// 352 (1028 bytes: frame 0x20 and the cursor spilled to +0x14 as retail, but the actor offset and index fold into
// one `index * 0xb6 + i` multiply instead of retail's spilled offset plus an i*12 IV); that with a short memory
// index 371 (1044 bytes); the bit tested as `*definition & 8` 285; 0x203ed0's argument as `value != 0` 285.
// Y488: native actor members at 0x10/0x1c/0x3f4, seven subsets and
// all with short bounded cleanup ordinal, eight 261 against fast control 261.
// Real type and complete body restored; ordinary 285, frame/actor/cleanup homes remain.
// Y499: native cleanup/prop-iterator/actor-iterator phase scopes, seven subsets and
// all with terminal receiver role name, eight 266 against fast control 266 (ordinary 281).
// Original body restored; frame 0x1c/0x20, actor/displacement and cleanup homes remain.
// M305: Native terminal parallel-table bound walk, for/while, less/not-equal and pointer declaration orders
// 270/269/270/269/270/269/270/269 fast versus unchanged fast 266 (ordinary 281).
// M3 lane 10 r73 (fast control 266 at 980 bytes, plain 281; aligned/masked in brackets, control [182/112]): the slot pointer named after the stop read 283
// (981) [192/110], before it 283 [192/110], with a type local and a selected pointer 286 [193/114]; the memory test through the entry actor alone 293 (997)
// [155/90], the best shape here; the actor refreshed after 0x258600 296 (997) [200/112]. Retail reads `.type` folded and computes the slot address with a
// separate `lea` before the branch, with the stop pointer in ECX and the index in EDI; ours keeps the index in ECX, so stop takes EDI. Still BLOCKED-BY 0x26ed40.
// T61: Actual memory-entry/callback-field/terminal-actor record references, seven subsets
// 266/282/282/266/266/282/282; all with bounded short memory ordinal 282
// (unchanged reduced control 266, full 281). NOT kept: no exact receiver graph;
// original observations, ordinal, iterators and complete body restored.
// MiniMax E (11-10): no new form, no lower score: the memory walk is still expanded at this site, so
// the walk's index keeps the actor offset and the step's size apart where retail keeps them
// together; that expansion's shape is set in another file, which this lane may not edit.
// MiniMax E (11-04): no new form, no lower score; the memory walk's pointer walk, its index
// body, the twice-read slot and the for-loop forms score 285, 370, 286, 288 and 555 against the
// unchanged 266 (fast; 281 ordinary).
// Y606: Actual participant element pointer/reference, promoted/native slot: four
// at 288 fast; member-array mutable/immutable bases, captured/live slots:
// 259/259/285/285 versus fast 266 (full 281). NOT kept: seven-point lead;
// native frame remains 0x1c/0x20. Original type/body restored.
// Y608: One twice-used low-byte input truth carrier, bool/byte/signed-char/word/short/
// dword/long/64-bit, 307/266/266/282/282/282/282/343 fast against control 266 (full 281).
// NOT kept: no exact graph; frame/displacement/countdown homes remain. Original body restored.
// DH379: Condition-owned stop/prop-next/actor-next results, seven subsets and immutable
// bindings: 266/266/266/266/266/266/266/266 fast, against its unchanged reduced control. NOT kept:
// no byte match or native frame/receiver recovery; original complete body restored.
// Round 82 (m3 lane 10, fast control 236, aligned 106 [LCS, frame offsets kept]): the prop walk through the
// existing 0x26be00/0x26be30 with the node pointer for the view index 285 (974 bytes) [142]; the same with the
// view index re-read through prop_node_get after the callback 287 (948) [141]; that plus the iterator compare
// against the cached actor 298 (942) [76: the prop walk, slot loop head and compare then line up with retail;
// the frame is 0x1c without retail's memory-walk counter slot]; the cached-actor compare alone 308 (942) [104];
// that walk and compare with the memory type test through the cached actor 292 (990) [124], with a short
// memory index 294 (990) [124]; walk and compare with a short memory index 298 (942) [76]. Still BLOCKED-BY
// 0x26ed40: the memory walk is joint_decline inlined (its short index keeps the actor offset and i*12 apart).
// Round 62 (m3 lane 2, fast; 0x26ed40 matched each time it was called): the counts record read through g_5047f4 at each
// use 236 (same code); the stop argument as `slot_index >= 4 ? NULL : slot` 236 (same code); the joint entry address
// written at each store 314 (1006 bytes); the memory walk as retail's source presumably was (short index, type tested
// through the cached actor, a call to the existing 0x26ed40) 364 (894 bytes: the actor offset then goes to EAX and is
// spilled as in retail, the index takes EBX); that with the prop walk through the existing 0x26be00/0x26be30 and
// prop_node_get for the view index 361 (884 bytes); the actor-iterator test by index (`iterator.actor_index !=
// actor_index`) 289; the slot loop with a slot pointer as the matched 0x1e23c0 writes it 236 (same code). Retail's tail
// also keeps NONE in EDI for the definition walk and stores the definition bit into the dead index slot. Still
// BLOCKED-BY 0x26ed40 (inlined in retail; its file's /Ob1 keeps it out of line here: a build-flag decision).
// M3 lane 10 r94 (fast, 0x26ed40/0x1e2150/0x258600/0x1e4990 listed, all MATCH; base 236, aligned 106, masked 65): no new form.
// The first difference (+0x3: retail loads the actor pool into ECX and the offset into EAX, and keeps the actor pointer in EBP
// unspilled) follows the memory walk, which is joint_decline (0x26ed40, MATCH) inlined in retail: its file's /Ob1 keeps it out
// of line (a build-flag decision; `inline` on it costs 0x1bead0 and 0x26ef70), and the bool `value` needs the declaration in
// src/unknown_1e3920.cpp, another lane's file this round. Still BLOCKED-BY 0x26ed40. Tried in the body: the prop walk with a
// `next_index` local in place of the `current` copy 237; the unit branch first (`if (!actor->unknown007)`) 271; the prop record
// released before the view record 286; the counts loop index as short 236 and the stop callback read at each use 236 (both the
// same code).
// MiniMax A (11-13): no new form, no lower score (236): the first difference is at +0x3, where retail loads the record pool
// pointer into ecx and this build loads it into eax, and the body is 22 bytes short because retail expands 0x26ed40 into it
// (BLOCKED-BY 0x26ed40, whose file is not this lane's).
// MiniMax B (11-14, run 75): no new form, no lower score (236); the body is left as it was. What still differs: the first difference is
// at +0x3, where retail loads the record pool pointer into ECX and this build loads it into EAX, and the body is 22 bytes short of
// retail's 980 because retail expands 0x26ed40 into it: BLOCKED-BY 0x26ed40 (src/unknown_26e370.cpp).
// M3 lane 10 r106 (fast, callers and callees matched throughout): `value` as bool at the definition and all eight
// declarations 288 (retail pushes the raw dword to 0x203ed0 and later tests its low byte); the memory walk as a test
// through the cached actor and a call to joint_decline with a file-local `__forceinline` declaration 364 (894 B: the
// redeclaration is not honoured across files); the view index re-read from the pool after the 0x1e2150 callback 280
// (974 B), with the iterator compare against the cached actor 274; the memory walk as a counted for 526 (1182 B, it
// unrolls). Retail also stores the definition-bit test into the dead actor_index slot ([esp+0x34]) before branching on
// it, the shape of an inlined helper's returned bool.
// NOT kept (coordinator review, MiniMax D 11-15): 236 -> 216 rested on a bool flag set by one test and read by the
// next (definition_bit = false; if (definition) definition_bit = ...; if (definition_bit)), a form kept only with
// a byte match.
// Round 454: successor captured before unlink, while, byte address, same tracking receiver 237, successor
// captured before unlink, while, typed helper, same tracking receiver 237, successor captured before
// unlink, for, byte address, same tracking receiver 237, successor captured before unlink, for, typed
// helper, same tracking receiver 237, successor captured before unlink, while, byte address, refreshed
// tracking lookup 278, successor captured before unlink, while, typed helper, refreshed tracking lookup
// 278, successor captured before unlink, for, byte address, refreshed tracking lookup 278, successor
// captured before unlink, for, typed helper, refreshed tracking lookup 278. Capturing the successor
// separately scores 237 with the existing tracking receiver. Refreshing the receiver after the callback
// follows the retail load but scores 278. The original walk and body are restored. The two opening score
// headings are merged into one.
// MiniMax D (11-17): no new form, no lower score (236): the first difference is still at +0x3, where retail
// loads the record pool pointer into ECX and this build loads it into EAX, and the body is still 22 bytes
// short of retail's 980 because retail expands 0x26ed40 into it; BLOCKED-BY 0x26ed40
// (src/unknown_26e370.cpp), which this lane may not edit.
// M3 lane 5 r197 (fast with 0x1e0160, 0x26ed40, both MATCH throughout; aligned exact / register-masked counts in
// brackets, base [106/69]; all restored). Diagnostic, reverted at once (a hand copy of an existing helper is not keepable):
// a file-local forceinline twin of joint_decline (actor re-derived, participant index read at each store) called from
// `for (short i = 0; i < 4; i++) if (actor->memory[i].type != NONE)` 301 at 1006 B [75/48]: the head then takes retail's
// registers (pool ECX, offset EAX and spilled to +0x10) and the memory walk is retail's (cursor EBX, byte offset EDX,
// countdown in a stack slot); left with it: frame 0x24 against 0x20 with the actor pointer spilled (ours uses EBP inside
// the prop walk), the raw `value` push, retail's prop walk (pool re-read after the 0x1e2150 callback, its two record
// lookups) and the definition bit's byte in the dead index slot. On the twin: the prop record re-derived from the pool at
// both reads 325 (1022 B) [84/72], that with the iterator compared against the cached actor 284 (990 B) [77/64]; those
// two without the twin 274. The 0x258600 call replaced by an undefined extern (no register summary) 240: not the cause
// (the opaque call takes no argument, so the index leaves EDI). The definition-bit test through a new forceinline helper
// with a false-then-set result 216 (966 B; the byte lands in the dead index slot as retail, the same code as the flag
// form reverted on 11-15, a match-only form), through a helper returning the direct expression 236 (same code). Still
// BLOCKED-BY 0x26ed40: retail expands it here and its file's inline setting keeps it a call in this build.
// M3 lane 4 r222 (fast with 0x26ed40, 0x1e2150, 0x258600, 0x1e4990 and 0x1e0160, all matched; control 236, aligned 108): the
// iterator compared against the cached actor alone 308 (942 B); the memory walk as the existing joint_decline call under the
// type test 348 (910 B, a call where retail expands it); the definition bit as a byte test `*definition & 8` 236 (same code);
// `(bool)value` for the 0x203ed0 argument 236 (same code); the counts loop index as short 236 (same code). Still
// BLOCKED-BY 0x26ed40 (expanded in retail, kept out of line by its file's inline setting).
// M3 lane 13 r115 (plain unless marked fast; each step on the one before; lead saved as build/r115/lead-1e1a00.patch
// in the m3-lane13 worktree; all restored): memory walk as `for (short i...) if (actor->memory[i].type != NONE)` calling
// a file-local forceinline copy of joint_decline 301 (1006 B, fast); prop walk through 0x26be00/0x26be30 as matched
// 0x1e23c0 writes it, the view index re-read through prop_node_get after the 0x1e2150 callback 303; the iterator
// compared against the cached actor 223 (974 B); written `actor != other` (retail's cmp order) 223; the post-callback
// prop node as a named pointer for the test and the release 196 (prop walk exact); the definition bit as a bool flag
// (false, set when the definition exists) 218 (980 B, tail exact); `value` as bool at the definition and all eight
// declarations, passed to 0x203ed0 unconverted, 4 plain and fast: only the slot loop's `lea` of the slot address
// sits before the handler-table loads where retail has it after the `test`. Slot-loop spellings on that 4: a slot
// pointer local 4 / passed as the argument 6; the type named first 4; a handler pointer with a stop local 4; the table
// expression at the test and the call 4; type and handler and stop locals 4; a handler pointer with `handler->stop`
// tested and called, also const, also with a stop local tested: MATCH on the fast image but 297 plain (996 B, frame
// 0x24: actor_index leaves EDI in the head and the byte offset takes EBX); the type named with the table read at the
// test and the call 297. The fast image flips to 297 too once it holds both 0xf0fd0's closure and the file of
// 0x1fb360/0x1fb510 (each alone keeps the match; their path into 0xcbf60 changes where LTCG enters the cycle
// 0x1e1a00 -> 0xcbf60 -> 0xcc810 -> 0x1bbdf0 -> 0x1e1a00; with 0x1fb510 emptied, diagnostic, still 297, so 0x1fb360's
// path decides it); 0x1e1a00's definition moved to the end of its file 297.
// Permuter on the 4 (600 s): no better variant. Alone on the original body, `value` as bool 288.
// M3 lane 5 r212 (fast on round 189's full-image list: 0xf0fd0, 0x1fb360, the callers and callees, all as before; control 236
// [masked 41, exact 82]): `(bool)value` at the 0x203ed0 call 236 (same code); `actor_get(actor_index) != other` 236 (same
// code); `!unit[0xaa]` 236 (same code); the memory walk through the cached actor pointer 308 (942 B), with a short index 308.
// Diagnostic (reverted): the memory walk removed 397 (862 B): the head then loads the pool into ECX and the offset into EAX
// as retail (actor_index moves to EBX), so the walk's shape is what sets the head. Still BLOCKED-BY 0x26ed40 (retail expands
// it here) and the bool parameter's declaration in src/unknown_1e3920.cpp (avoided this round).
// M3 lane 7 r189 (fast with 0xf0fd0, 0x1fb360, the ten callers, 0x26be00, 0x26be30, 0x203ed0, 0x1e22d0, 0x26ed40: this list
// reproduces the full image's allocation; control 236, nothing kept). Re-built lane 13's lead on today's body (file-local
// forceinline copy of joint_decline called from a `for (short ...)` memory walk with the test through the entry actor, the
// prop walk through 0x26be00/0x26be30 with a two-use node pointer, `actor != other`, the definition bit as a false-then-set
// bool, `value` as bool at the definition and in all eight declarations): 4 at 980 bytes, the only difference being the slot
// loop, where ours forms the slot address right after the type read and loads the handler into EDX, retail loads handler and
// stop through ECX and forms the address after the test. On that lead, same code (4 each): the stop read twice with no
// local; the type in a short or long local; `slot_index >= 4 ? NULL : &slot`; `stop != NULL` with `(*stop)(...)` and
// `actor->slots + slot_index`; the handler held as a dword or an untyped pointer and cast for the stop read; a handler
// pointer read once into the stop local; the stop local with the test re-reading the table; one more named local in the tail
// (the game time). Flips the whole allocation (frame 0x24, the byte offset in EBX, the actor index read from the stack and
// EDI pushed only before 0x258600): the handler pointer with `handler->stop` tested and called 297 (996 bytes); that with one
// to seven unused locals 297 each; with the handler const, at function scope, with `other != actor`, with the slot index
// declared ahead of its loop, with a slot pointer in the call's block or ahead of the loop, with the counts pointer dropped
// or one more local 297 each; with the direct definition test 289 (990); with a long memory index 494 (1172); the slot
// pointer named in the call's block 297, between the stop read and its test 297, as the selected argument 298; a pointer to
// the stop field tested and called 299; the handler pointer with a stop local and the test on the field 297. So any pointer
// local the loop reads twice, or a named slot address, flips it; integers and once-read pointers are propagated away.
// The handler-pointer form is byte-exact (MATCH) in a reduced image without src/unknown_1fb350.cpp, and the permuter's
// baseline shows the flip in the full image too (285 by its count, `sub esp, 0x24`); its 600 s annealed run with 0xf0fd0
// and 0x1fb360 as context (21 tries) found nothing below that. Diagnostic: this whole file's contents placed in
// src/actors.cpp (same flags, earlier in source order) 297, so the file's position does not choose the state. Built only
// from existing functions (joint_decline called, direct definition test, the prop walk, the cached compare, bool value) 397
// (862 bytes), so no part of the lead is a gain without the expanded helper. Still BLOCKED-BY 0x26ed40 for a natural form
// (marked inline it also expands in 0x1bead0 and 0x26ef70), and on the link-time state for the handler pointer.
// MiniMax C (11-19): no gain (236, baseline; body restored). The known 216 form holds the definition bit in a byte flag set by the tests and read by the next test, which is kept only with a byte match. What is left is the head: ours loads the pool global and the slot table into other registers than retail (the pool into EAX against retail ECX) and keeps the actor index in EBX against retail EAX.
// M3 lane 2 r114 (fast with 0xf0fd0/0x1fb360/0x26be00/0x26be30/0x203ed0/0x1e22d0/0x26ed40; control 236), tail forms the
// list above lacks: the definition test held in a bool from the `&&` expression (retail stores the bit into the dead
// [esp+0x34] slot before branching, the shape of a named phi) 236 (same code: no store); the definition read from the
// cached actor instead of a fresh lookup 298 (946 B); the last block through the cached actor 296 (990 B); the last
// block re-deriving the actor at each of its three uses 251 (980 B, retail's length, aligned 161 / masked 114 against
// 106 / 65); the bit test spelled `!= NULL` / `!= 0` 236 (same code). In the tail retail keeps NONE in EDI and the pool in
// EBP after the 0xcbf60 call and reloads the scaled index from +0x10 at each lookup; ours puts the pool in EDI, the index
// in EBP and compares with -1 immediates. Still BLOCKED-BY 0x26ed40.
// M3 lane 8 r202 (fast with the nine callers, all MATCH each time; control 236, aligned 110, register-masked 69): every caller is
// machine 3's, so `value` as bool at the definition and all eight declarations (retail's raw push to 0x203ed0 and low-byte
// tests) 288 (aligned 108, masked 66); that with the view index re-read after the 0x1e2150 callback and the iterator compared
// against the cached actor 208 at 958 B but aligned 113, masked 86 (the prop walk hoists the pool's data address; one shifted
// region re-syncs, so not a real gain), the same through the existing prop_node_get at each use 208 (same code); bool with the
// re-read 255; bool with the cached compare 327 (926 B, aligned 105, masked 63). Still BLOCKED-BY 0x26ed40.
// M3 lane 12 r160 (fast with 0x26ed40's nine callers and the matched functions of its file; control 236): the memory
// walk calling 0x26ed40 under the type test through the cached actor 364 as a short for loop or a long do loop (a call);
// with `inline` on 0x26ed40 301/319 (1006 bytes), 0x1bead0 and 0x26ef70 then expanding it too and losing their match;
// its file without the inline-depth flag (diagnostic, flags unchanged) the same three expansions; `value != 0` 236 and
// `memory + memory_index` 236 (same code). Retail keeps the call at all eight other sites. Still BLOCKED-BY 0x26ed40.
// Signed/unsigned byte-displacement for/do walks 297 each. End-bounded screen with a
// fresh entry from byte distance, element distance, short index or advanced byte
// displacement 285/362/341/296. The actor-pool load and scaled-index register remain.
// M3 lane 2 r118 (fast with 0xf0fd0/0x1fb360/0x26be00/0x26be30/0x203ed0/0x1e22d0/0x26ed40, all MATCH; control 236): the
// memory walk's entry through the existing element_502424_get 236 (same code); the prop walk's node through the existing
// prop_node_get view (next_index, view_index) 236 (same code); both 236; the slot loop with a slot pointer local as
// matched 0x1e23c0 writes it 236 (same code); the prop walk through the 0x26be00/0x26be30 iterator with the node
// re-derived through prop_node_get after the 0x1e2150 callback (retail's re-read) 285 (948 B). Still BLOCKED-BY
// 0x26ed40 (expanded in retail, kept a call by its file's inline setting) and by `value` as bool, whose declaration in
// src/unknown_1e3920.cpp sits in a file another lane holds this round.
// Signed/unsigned byte-displacement for/do walks 297 each. End-bounded screen with a
// fresh entry from byte distance, element distance, short index or advanced byte
// displacement 285/362/341/296. The actor-pool load and scaled-index register remain.
// M3 lane 6 r217 (fast with 0xf0fd0/0x1fb360/0x26be00/0x26be30/0x203ed0/0x1e22d0/0x26ed40, all MATCH; control 236, aligned
// 82, masked 41): the invitation test through the entry actor with the body through the re-derived entry 319 (1006 B), that
// as a `for (short ...)` walk 339 (1022 B); the first prop index read through the cached actor 309 (942 B); the invitation
// index as short in the do-while 236 (same code); the invitation walk as a plain `for (long ...)` 526 (1182 B). Still
// BLOCKED-BY 0x26ed40.
// M3 lane 11 r141 (fast with 0xf0fd0, 0x1fb360, the ten callers, 0x26be00, 0x26be30, 0x203ed0, 0x1e22d0, 0x26ed40, 0x1e2150,
// 0x258600 and 0x1e4990, all as before in every run; control 236, aligned differing rows 82, 43 with registers masked):
// kept, 236 -> 59 at 974 B (aligned 9), plain check 59. No copy of a helper function is needed for round 115's lead: the
// memory walk written in place as `for (short ...)` testing `actor->memory[i].type` through the cached actor, with the
// release steps as retail expands them (the owner looked up again, the entry's short read at each of its two uses, the
// final store through `owner->memory[i]`), together with the prop walk through 0x26be00/0x26be30 (the node re-read
// through prop_node_get after the 0x1e2150 callback) and the iterator compared against the cached actor, is 196 (aligned
// 9, first difference +0x40); each alone or in pairs raises the count (walk 301, prop walk 285, compare 308, walk and
// prop walk 303, prop walk and compare 299). On 196: `value` as bool at the definition and its eight declarations
// (retail pushes the parameter's dword to 0x203ed0 unconverted and tests its low byte) 58 with `actor != other`, 59 with
// `other != actor` (kept: the operand order main had); with the definition bit as a false-then-set bool 4 at retail's
// 980 B (the flag form, match-only: only the slot loop's `lea` then sits ahead of the handler loads) and on 196 alone
// 218. Spellings of the walk on the 58 form: the final store through the entry pointer (`memory->type = NONE`) 294, also
// without the owner local 294; the entry's short in a named local 107; a long counter 486 (1166 B); the real
// joint_decline called in place of the steps 397 at 862 B (retail expands it here and calls it from its six other
// callers, all matched). Slot loop on the 4 form: the slot address in a pointer local between the stop read and its
// test 4 (on 196: 198, the select then masks the other way round), ahead of the stop read 4 (same code); the type in a
// short local or bound to a `short const &` 4 (same code); the stop bound to a const reference 4 (same code); the
// argument through a new inline accessor returning the slot or NULL 4 (same code); the handler pointer with its stop
// tested and called 297 (996 B, frame 0x24: the listed whole-allocation flip), with the accessor 297. Still open for the
// match: the handler loads staying in ECX with the `lea` after the test, and the stored definition bit.
// M3 lane 3 r289 (fast with 0x1e23c0, 0x1e22d0, 0x1e2150 and 0x1e17d0, all MATCH throughout; control 59; plain where noted).
// Retail's sibling slot loops fall in two groups: 0x1e23c0, 0x1e2150 and 0x1e17d0 keep the actor index on the stack in the
// loop and form the slot address right after the type load (their slot-pointer-first source matches), while 0x1e22d0 and
// this function keep it in a callee-saved register and form the address after the stop test, the handler chain staying
// in ECX. On the listed 4 form (flag bool + `actor != other`; 4 in the plain check too): address spellings for the call's
// slot pointer or the type read (byte arithmetic in either association, `actor->slots + slot_index`, a `slots` pointer
// for both) 4 each (same code); a `slots` pointer for only one of the two 264 (980 B, the flipped allocation); a
// file-local inline accessor for the handler, the type, the stop pointer or two of them 4 each; handler and type in
// named locals 4; the stop read twice through the type 297; the selected slot pointer in a local before or after the
// stop read 298; references or an entry pointer for the handler, the stop field or the type 4 each; a memcpy read of
// the stop field or of the type 4 each; 1..13 unused locals at the top 4, 14..64 8 (no matching state). Diagnostic
// (reverted at once): the handler read through a pointer-to-volatile, as 0x1e22d0 has it, gives MATCH in the fast image;
// so does the handler read as a four-byte memcpy of its table entry (const or plain pointer, `&table[type]` or
// `table + type`; check.py --fast agreed, 19 of 19 in this file; without the flag 53, without the compare order 1). The
// plain check gives 297 at 996 B for every one of them (the volatile read, the memcpy in three spellings and with a type
// local, with 1, 2, 4 or 8 unused locals; 289 at 990 B without the flag): in the full image they push EDI only after the
// loop, keep the actor index on the stack inside it and form the address early, the other group's shape. The fast image
// is not faithful for this function once the loop holds such a read: trust only the plain check. Body unchanged (59).
// Open: what keeps the actor index in EDI through the loop in the full image while the address is formed late.
// M3 lane 1 r143 (fast with 0xf0fd0 and 0x1fb360, the image that reproduces the plain result): the definition bit as a
// named bool in its own `if` nested under `if (definition)` 54 (976 B; the byte store is still dropped); the false-then-set
// bool 5, with `actor != other` 4 (B: only the slot loop's `lea` left; ours gives the handler EDX once the `lea` frees it,
// retail reuses ECX and forms the `lea` after the test). On B: the handler and stop read again at the call 4; a handler
// pointer local 4; a short type local plus the handler local (0x1e22d0's text without its volatile) 4; the handler bound
// by `s_slot_handler *const &` 4, with the type local 4, through a slot pointer 297 (996 B); the type bound by
// `short const &` 4; the argument as `actor->slots + slot_index` 4, as `slot_index >= 4 ? NULL : ...` 4.
// M3 lane 3 r291 (H2M_FAST with 0xf0fd0 and 0x1fb360, the image that reproduces the plain result; on the listed 4 form,
// the false-then-set definition flag plus `actor != other`, rebuilt: 4 at 980 B, only the slot loop's `lea` ahead of the
// handler loads): the slot loop under a guard `actor->current >= 0` 272 (first difference +0x23); the stop pointer
// declared in the `if` condition 4; the slot index declared ahead of the loop 4; the argument as a slot pointer
// local set to NULL and assigned under `slot_index < 4` 281 (964 B: a branch, not the setge select); the loop as a
// `while` with the increment at the end 4; diagnostic, reverted at once: a compiler read/write barrier between the
// stop read and its test 4 (the barrier does not keep the `lea` after the loads; only a volatile or copied handler
// read does, and those flip the full image to 297). Body unchanged (59).
// M3 lane 7 r211 (h2m fast with 0xf0fd0 and 0x1fb360; on the listed 4 form, rebuilt: 4 at 980 B): the type in a `long`
// local 4; `if (!stop) continue;` then the call 4; the call as `(*stop)(...)` with a typed null arm 4; the handler table
// bound to a `s_slot_handler *const *` local before the loop 4 (all the same code); the argument's slot through a second
// actor_get(actor_index) 273. Body unchanged (59).
// M3 lane 16 r174 (h2m fast with 0xf0fd0 and 0x1fb360; control 59, aligned 13): the false-then-set flag rebuilt as
// `bool counted = false; if (definition) counted = (*(dword *)definition >> 3) & 1; if (counted)` with `actor != other`
// 4 at 980 B (plain check 4 too); the same flag set by `if (definition && bit) counted = true;` 57 (974 B, the byte store
// dropped) and nested under `if (definition)` 57. On the 4 form: the stop assigned inside the `if` condition 4; the
// stop declared ahead of the loop 4; the selected slot in a local inside the call's block 4; the table read twice (test
// and call) 4; `slot_index >= 4 ? NULL : ...` 4 (all the same code). Diagnostic, reverted: the handler read as a
// four-byte memcpy now gives 297 in the reduced image too (996 B, frame 0x24, actor index on the stack in the loop), so
// the reduced image agrees with the plain check on that form. Reading of the last 4: retail's handler reuses the type's
// ECX while the slot offset is still live (the `lea` comes after the stop test); ours forms the `lea` first, the offset's
// EDX is then free and the handler takes it, which pins the `lea` ahead of the handler load. Body unchanged (59).
// M3 lane 8 r221 (h2m fast with 0xf0fd0 and 0x1fb360; control 59): the listed 4 form rebuilt (false-then-set definition
// flag plus `actor != other`; 4 at 980 B, only the slot loop's `lea` ahead of the handler loads). On it: the slot loop's
// condition as `actor->current >= slot_index` 6; `if (stop != NULL) (*stop)(..., slot_index > 3 ? NULL : &slot)` 6; the
// flag computed as `(... & 8) != 0` 4 (same code). On main's body with `actor != other`: the bit held by a conditional
// initialiser (`bool counted = definition ? bit : false;`) 52 (976 B, no byte store). Body unchanged.
// M3 lane 7 r215 (h2m fast with 0xf0fd0 and 0x1fb360; control 59; the listed 4 form rebuilt, 4 at 980 B). Retail's own
// 0x1e22d0 loop keeps the slot offset in EAX and forms the address in place after the stop test, 0x1e23c0's forms it
// right after the type load: the placement follows from whether the type load and the argument share one address
// temporary. On the 4 form: the slot loop, the `current` store and the 0x258600 call as a force-inlined routine that
// looks the actor up itself 275 (980 B), the loop alone in such a routine 275: the loop then forms the address after
// the test as retail does (offset in EAX, address in place, the handler still through EDX), but two actor pointers
// are live, the outer one is spilled to a stack home and the memory countdown takes EBP (aligned 19). The same
// routines declared plain `static inline` stay out of line: 366 (894 B), 347 (910 B), and 328 (932 B) for the one
// that receives the actor pointer; force-inlined, the one that receives the actor pointer 4 (same code); a per-slot
// routine taking the actor pointer and the index 4 (same code); a per-slot routine taking the slot pointer 297
// (996 B, the listed whole-allocation flip). A second actor pointer local for the loop, declared after or ahead of
// the main one 275/275, also with the `current` store through it 275; a slots pointer taken from a second lookup
// 275; only a loop-local actor pointer, the main one looked up after the 0x258600 call 282 (pool reloaded there).
// Diagnostic, restored at once: 0x1e22d0's text (short type local, handler through a pointer-to-volatile) in this
// image 297 (996 B, frame 0x24), as the plain check gave in r289. The loop's pointer as a plain copy of the main
// one 4, the main one copied from the loop's after the 0x258600 call 4 (same code: copies coalesce, so the late
// address needs two looked-up pointers live at once). Body unchanged (59).
// M3 lane 15 r137 (H2M_FAST with 0xf0fd0 and 0x1fb360; control 59; the listed 4 form rebuilt, 4 at 980 B, only the
// slot loop's `lea` ahead of the handler loads). Retail's own 0x1e2150 and 0x1e23c0 (slot pointer named, type read
// through it) form the address right after the type load as ours does; 0x1e22d0 and this function form it after the
// stop test, where the handler reuses the type's ECX. On the 4 form: the type in a `short` local and the handler in a
// plain `s_slot_handler const *` local 4; the type read by a raw offset associated the other way
// (`actor + slot_index * 0x40 + 0x90`) 4; the argument as `(s_slot *)((byte *)actor + 0x90) + slot_index` 4 and as
// `actor->slots + slot_index` 4 (all the same code: the address is shared whatever its spelling). On main's body with
// `actor != other`: the bit held as `bool counted = definition && bit;` 57 (974 B, no byte store). Body unchanged (59).
// M3 lane 3 r302 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59; the listed 4 form rebuilt, 4 at
// 980 B). Reading of the definition block: retail computes the bit into AL (`shr eax, 3; and al, 1`), stores it to the
// dead actor-index home [esp+0x34] and branches on the flags, which is exactly the false-then-set flag form; so that form
// is retail's there and only the slot loop's `lea` stays (ours forms it after the type load, the handler then takes EDX;
// retail's handler reuses the type's ECX and the `lea` sits between the stop test and its jump). On the 4 form, each a
// no-code file-local inline accessor: for the loop bound (`actor->current`) 4, for the `slot_index < 4` test 4, for the
// type read 4, all three together 4 (same code each). Diagnostic, restored at once: the handler read as a four-byte
// memcpy of its table entry 297 (996 B, frame 0x24, EDI pushed after the loop: the listed whole-allocation flip, now in the reduced image too).
// Body unchanged (59).
// M3 lane 5 r240 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH throughout; on the listed 4 form, rebuilt: 4 at 980 B): the
// stop call split into two calls by `slot_index < 4` arms 282 (964 B, a branch for the select); a `slots` base local for
// both the type read and the argument 4 (same code); the handler bound by `s_slot_handler const &`, its stop tested and
// called through it 297 (996 B); the loop's actor pointer bound by `s_actor_view *const &` 4 (same code); the stop bound
// by `t_slot_proc const &` to the handler's field 299 (996 B). Body unchanged (59).
// M3 lane 4 r257 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59; the listed 4 form rebuilt, 4 at
// 980 B). Measured cause of the last 4: on that form the code is generated slot address first, then the type load;
// the three loop temporaries then take scratch registers in turn (type ECX, offset EDX, handler EDX again because the
// address has already consumed the offset), and the handler overwriting EDX is what pins the `lea` ahead of it in the
// schedule. Retail generates the address last (after the stop test): the offset is still live when the handler is
// loaded, EAX is already bound to the address, so the handler reuses the type's ECX. So retail's slot address is
// materialised after its first use inside the type load, not ahead of it. Forced schedule changes in that block (30
// tried) leave the 4, as expected from that reading. On the 4 form: a plain slot pointer taken after the stop read
// and ahead of the test, the call passing `slot_index < 4 ? slot : NULL` 297 (996 B, frame 0x24: the listed
// whole-allocation flip, in which the actor's pool offset keeps EBX through the memory loop and the actor index
// stays on the stack in the slot loop); that with a `short` type local 297; that with the pointer declared ahead of
// the stop read and assigned after it 297; the pointer assigned inside the type read's index expression 297; the
// pointer declared in the call's block 297; a NULL pointer assigned under `slot_index < 4` in the call's block 281
// (964 B, a branch for the select); a named byte offset used for the type read and for the argument 299 (996 B).
// Body unchanged (59).
// M3 lane 13 r154 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59, the listed 4 form rebuilt: 4 at 980 B).
// Scheduler reading of the slot loop's test block on the 4 form: the slot address (`lea eax <- edx, ebp`) is generated
// before the type load and shared with it, so the handler load is given EDX after the address has read it and must
// follow the `lea`; retail's handler reuses the type's ECX and the `lea` lands after the stop test. On the 4 form:
// 0x1a80e0's spelling (slot pointer first, type read through it, `slot_index < 4 ? slot : 0`) 297 (996 B); a slot pointer
// for the type read with the argument formed again 4 (same code); a short type local with the handler table read at the
// test and at the call 297 (996 B); a NULL slot pointer set under `slot_index < 4` in the call's block 281 (964 B). On
// main's body: `actor != other` alone 58 (one point). Body unchanged (59).
// With the false-then-set definition Boolean and actor-first compare, slot type through a member
// pointer 4/4, header view 4/4, or fixed-size copy 297/297 with stop-member/guard forms.
// Direct type with stop member pointer 4, and const slot record with guard declaration 4.
// NOT kept: the six 980-byte forms still form the slot address before the handler test.
// The fixed-size copies grow the frame to 0x24 and keep the early address.
// M3 lane 15 r145 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59; the listed 4 form rebuilt, 4 at
// 980 B). Measured cause of the listed whole-allocation flip (297): on the 4 form the handler is a scratch temporary
// (EDX, the slot offset's register once the address is formed); a handler pointer local with `handler->stop` tested and
// called makes it an allocator candidate that takes ECX as retail's does, but the slot loop's test block then holds six
// candidates instead of five, the actor pointer's rank rises from 14 to 18 (its one reference there, weight 2), passes
// the memory walk's pointer (15), the actor is coloured first and the scaled actor offset then takes EBX for the head
// and the memory walk. With that one rank forced back under the walk pointer's (a forced compile, not source) the
// handler-local form MATCHes, so one count is left: the actor pointer must rank at or below the memory walk's pointer
// with the handler named (three points). On the handler-local form: the handler read once into a `stop` local 4 (it
// folds back); the clump index, the unit index, the prop index or the view index named for their two uses 297 / 297 /
// 297 / 295; a guard `if (0 <= actor->current)` around the slot loop 270 at 980 B (the allocation and the slot loop are
// retail's: the index's initialisation leaves the entry block, nine candidates become eight and the actor ranks 8; but
// the head then tests `cmp ..., 0` ahead of `xor esi, esi` and jumps over padding), written `actor->current >= 0` 272;
// the index initialised ahead of the guard, or assigned inside it 297 / 297; the index declared ahead of the lookup,
// or initialised outside a `for (; ...)` 297 / 297; an empty `do {} while (false);` after the lookup, after the slot
// loop, ahead of the memory walk, or wrapped round the slot loop 297 each; the memory test through a pointer local 297;
// the owner looked up after the entry pointer 355 (1028 B); the `current` store after the 0x258600 call 297; the memory
// walk in a force-inlined helper 297; the slot loop (handler local) in a force-inlined helper, with or without the
// `current` store 297 / 297. Body unchanged (59).
// With the separate definition flag, slot-owner bound/type/both/callback/bound and callback/type and callback/all 4/4/4/4/4/8/4.
// Slot-owner view acquired first 4. NOT kept: the match-only forms still form the slot address before the handler test.
// M3 lane 16 r189 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59; the listed 4 form rebuilt, 4 at 980 B;
// with the handler named and `handler->stop` tested and called 297 at 996 B). The allocator's ranking on the named form:
// the slot loop's test block holds six values (five on the 4 form), so the actor ranks 18 (14) and is coloured ahead of
// the memory walk's stepped pointer (15, created later than the actor), and the scaled actor offset then takes EBX for the
// head and the walk. One more value referenced in the walk's test block or in its latch block would put the walk pointer
// first. On the named form: the walk's last store through the body's `memory` pointer 304 (998 B); the walk's test
// through a named pointer 297 (same code); the joint index read once into a named short for both stores 281 (980 B, the
// head still flipped); the walk index typed long 494 (1172 B). Body unchanged (59).
// M3 lane 5 r252 (H2M_FAST and the plain check, both with 0xf0fd0 and 0x1fb360 listed; control 59; the listed 4 form
// rebuilt, 4 at 980 B; with the handler named, `handler->stop` tested and called, 297 at 996 B). Caution: screened with
// 0x1e1a00 alone (both fast tools agree there) the handler-named form with `other != actor` reads 1 (only the compare's
// operand order left), and with `if (actor == other) continue;`, with `if (!(actor == other))`, or with `actor != other`
// and `other` declared ahead of the iterator it reads MATCH; with 0xf0fd0 and 0x1fb360 listed each is 297, and the plain
// check of the `continue` form is 297 too, so those single-address results are reduced-image artifacts. Allocator reading
// of the handler-named form: the actor pointer ranks 18 (slot-loop test block 6 values, 5 on the 4 form);
// a forced-ranking search: the actor at 15 MATCHes, at 16 not; the memory walk's stepped pointer raised to 16 or 17 does nothing
// and to the top 277. Body unchanged (59).
// M3 lane 6 r251 (H2M_FAST with 0xf0fd0 and 0x1fb360, MATCH each run; control 59; the listed 4 form rebuilt, 4 at 980 B).
// Allocator reading of the handler-named form (297), read with 0xf0fd0 and 0x1fb360 kept in the reduced image (without
// them that image does not reproduce 297): the actor ranks 18 (the slot-loop test block references six values: index, actor,
// slot address, handler, stop and the zero constant) above the memory walk's stepped pointer (15); forced compiles:
// the actor at 15 MATCH, and the walk pointer at 18 (a tie it wins as the later-created value) or 19 MATCH as well, so
// either the actor -3 or the walk pointer +3 is the count to move (the walk pointer loses 22 in the walk body, eleven
// values, where it is live and unreferenced). A forced-ranking search on the 4 form (16 decisions) found nothing below
// 4. Forms on the handler-named form: the walk body without the `memory` pointer local 281 (964 B); the stop tested
// `!= NULL` and called as `(*handler->stop)(...)` 297 (same code). On the 4 form: the slot loop test written
// `actor->current >= slot_index` 6. Body unchanged (59).
// @retail 0x1e1a00
void __stdcall function_1e1a00(long actor_index, bool value)
{
	s_actor_view *actor = actor_get(actor_index);
	for (short slot_index = 0; slot_index <= actor->current; ++slot_index)
	{
		t_slot_proc stop = g_46eeb8[actor->slots[slot_index].type]->stop;
		if (stop)
			stop(actor_index, slot_index < 4 ? &actor->slots[slot_index] : NULL);
	}
	actor->current = NONE;
	function_258600(actor_index);
	for (short memory_index = 0; memory_index < 4; memory_index++)
	{
		if (actor->memory[memory_index].type != NONE)
		{
			s_actor_view *owner = actor_get(actor_index);
			s_slot_memory_entry *memory = &owner->memory[memory_index];
			byte *entry = g_502424->data + (memory->unknown4 & 0xffff) * 0xbc;
			*(long *)(entry + *(short *)((byte *)memory + 2) * 12 + 4) = NONE;
			*(short *)(entry + *(short *)((byte *)memory + 2) * 12 + 8) = 1;
			owner->memory[memory_index].type = NONE;
		}
	}
	if (actor->unknown00a[0])
		function_203fb0(actor_index);
	else
		function_203ed0(actor_index, value);
	if (actor->unknown07c != NONE)
		function_2694d0(actor->unknown07c, actor_index);
	s_iterator prop_walk;
	function_26be00(actor_index, &prop_walk);
	while (function_26be30(&prop_walk))
	{
		function_1e2150(actor_index, prop_walk.index);
		s_prop_node_view *node = prop_node_get(prop_walk.index);
		if (node->view_index != NONE)
			record_pool_release(g_502414, node->view_index);
		record_pool_release(g_502418, prop_walk.index);
	}
	actor->first_prop_index = NONE;
	s_record_pool_iterator prop_iterator;
	byte *prop;
	prop_iterator.data = g_50241c;
	prop_iterator.index = NONE;
	while ((prop = data_iterator_next_inlined(&prop_iterator)) != NULL)
	{
		if (*(long *)(prop + 0x1c) == actor_index)
			*(long *)(prop + 0x1c) = NONE;
	}
	s_actor_iterator iterator;
	function_x66da2b(&iterator, false);
	s_actor_view *other;
	while ((other = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
	{
		if (other != actor)
			function_1e1740(iterator.actor_index, actor_index);
	}
	function_2628f0(actor_index, g_470fa0);
	if (*(long *)((byte *)actor + 0x3f4) != NONE)
		function_26dc20(actor_index);
	if (actor->unknown007)
	{
		if (*(long *)((byte *)actor + 0x1c) != NONE)
			swarm_leader_detach(actor_index);
	}
	else
	{
		if (actor->unknown018 != NONE)
		{
			byte *unit = (byte *)ai_object_get(actor->unknown018);
			if (*(byte *)(unit + 0xaa) == 0)
				*(long *)(unit + 0x3d8) = actor->unknown054;
		}
		function_1e1250(actor_index);
	}
	byte *definition = (byte *)function_1e4990(actor_get(actor_index)->unknown054);
	if (definition && ((*(dword *)definition >> 3) & 1))
	{
		byte *counts = (byte *)g_5047f4;
		--*(short *)(counts + 6);
		if (value)
		{
			++*(short *)(counts + 8);
			*(long *)(counts + 0xc) = g_510c54->game_time;
		}
		long i = 0;
		do
		{
			if (*(long *)(counts + 0x18 + i * 4) == actor_index)
			{
				*(long *)(counts + 0x18 + i * 4) = NONE;
				*(short *)(counts + 0x12 + i * 2) = 0;
			}
		} while (++i < 3);
	}
	s_actor_view *actor_view = actor_get(actor_index);
	if (actor_view->unknown009)
	{
		actor_view->unknown009 = false;
		*(long *)((byte *)actor_view + 0x10) = g_510c54->game_time;
		--*(short *)((byte *)g_4f55d0 + 0x36a);
	}
	record_pool_release(g_4f55f0, actor_index);
}

long __stdcall function_1dfb90(long definition_index);
void function_1e3b60(long actor_index);

/* the initial team of each actor type */
extern short const g_44aee8[20];

// @retail 0x1e0160
long __stdcall function_1e0160(long squad_index, long entry_index, long unit_index, bool flag)
{
	long actor_index = NONE;
	long resolved_squad = squad_index;
	long const *unit_reference = &unit_index;
	bool const *flag_reference = &flag;
	byte *squad = NULL;
	if (squad_index != NONE)
		squad = g_51e9d8->data + (squad_index & 0xffff) * 0x98;
	if (*unit_reference != NONE && entry_index != NONE)
	{
		actor_index = function_1dfb90(entry_index);
		if (actor_index != NONE)
		{
			s_actor_view *actor = actor_get(actor_index);
			function_1e0d50(actor_index, *unit_reference);
			if (squad_index == NONE)
				function_203f60(actor_index);
			else
			{
				if (!(squad_index & 0xffff0000))
					resolved_squad = (*(short *)squad << 16) | (squad_index & 0xffff);
				function_203d70(actor_index, (short)resolved_squad, false);
			}
			byte *unit = (byte *)ai_object_get(*unit_reference);
			short entry_team = 0;
			if (resolved_squad != NONE)
			{
				byte *entry = *(byte **)((byte *)g_4e0350 + 0x164) + (resolved_squad & 0xffff) * 0x74;
				if (entry)
					entry_team = *(short *)(entry + 0x24);
			}
			short unit_team;
			if (entry_team != 0)
				function_1e1150(actor_index, entry_team);
			else if (unit && (unit_team = *(short *)(unit + 0x138)) != NONE && unit_team != 0)
				function_1e1150(actor_index, unit_team);
			else
				function_1e1150(actor_index, g_44aee8[actor->unknown004]);
			if (squad && !squad[0x76] && actor->unknown024 != 0)
				squad[0x76] = (byte)actor->unknown024;
			actor->unknown084 = *flag_reference ? 0 : 3;
			function_1e3b60(actor_index);
			if (*(short *)((byte *)actor + 0x254) == NONE)
			{
				if (*(long *)((byte *)actor + 0x1c) != NONE)
					swarm_leader_detach(actor_index);
				function_1e1a00(actor_index, 0);
				actor_index = NONE;
			}
		}
	}
	return actor_index;
}

void __stdcall swarm_leader_attach(long actor_index, long perception_index);

// @retail 0x1e02f0
long __stdcall function_1e02f0(long squad_index, long entry_index, long unit_index, long perception_index, bool flag)
{
	long actor_index = NONE;
	long const *unit_reference = &unit_index;
	long const *perception_reference = &perception_index;
	bool const *flag_reference = &flag;
	byte *squad = NULL;
	if (squad_index != NONE)
		squad = g_51e9d8->data + (squad_index & 0xffff) * 0x98;
	if ((*unit_reference != NONE || *perception_reference != NONE) && entry_index != NONE)
	{
		actor_index = function_1dfb90(entry_index);
		if (actor_index != NONE)
		{
			s_actor_view *actor = actor_get(actor_index);
			if (*unit_reference != NONE)
				function_1e0d50(actor_index, *unit_reference);
			else if (*perception_reference != NONE)
				swarm_leader_attach(actor_index, *perception_reference);
			if (squad_index == NONE)
				function_203f60(actor_index);
			else
			{
				if (!(squad_index & 0xffff0000))
					squad_index = (*(short *)squad << 16) | (squad_index & 0xffff);
				function_203d70(actor_index, (short)squad_index, false);
			}
			byte *unit = *unit_reference != NONE ? (byte *)ai_object_get(*unit_reference) : NULL;
			short entry_team = 0;
			if (squad_index != NONE)
			{
				byte *entry = *(byte **)((byte *)g_4e0350 + 0x164) + (squad_index & 0xffff) * 0x74;
				if (entry)
					entry_team = *(short *)(entry + 0x24);
			}
			short unit_team;
			if (entry_team != 0)
				function_1e1150(actor_index, entry_team);
			else if (unit && (unit_team = *(short *)(unit + 0x138)) != NONE && unit_team != 0)
				function_1e1150(actor_index, unit_team);
			else
				function_1e1150(actor_index, g_44aee8[actor->unknown004]);
			if (squad && !squad[0x76] && actor->unknown024 != 0)
				squad[0x76] = (byte)actor->unknown024;
			actor->unknown084 = *flag_reference ? 0 : 3;
			function_1e3b60(actor_index);
			if (*(short *)((byte *)actor + 0x254) == NONE)
			{
				if (*(long *)((byte *)actor + 0x1c) != NONE)
					swarm_leader_detach(actor_index);
				function_1e1a00(actor_index, 0);
				actor_index = NONE;
			}
		}
	}
	return actor_index;
}

#include "unit_requests.h"

bool g_4f55e8;
void function_caf00(long unit_index);
long function_1e4a10(long tag_index);
void function_d5c90(long object_index, real *body_vitality, real *shield_vitality);
bool function_1df5d0(short first, short second);
bool function_1e11b0(long actor_index, bool active);
bool function_114b60(short entry_index, short fallback_index, long unit_index, long priority, void const *extra);

bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);

