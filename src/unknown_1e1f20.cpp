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

void __stdcall function_1e1a00(long actor_index, long value);
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

// score 7 (was 7): slot address and callback lookup allocation differ (the slot loop is matched
// 0x1e1740's loop source, and a compile without LTCG allocates it like retail);
// shared slot/condition callback/at-use callback 10/10/10; nullable slot
// before lookup/after test 10/10; handler local/slot reference 10/10;
// direct slot expressions 7.
// slot cursor/cursor callback at use 115/115; signed slot type 7;
// direct actor index 7; inclusive current count 99; empty callback continue 7;
// counter lifetime from entry 30; callback declared before slot 7.
// Type through slot/slot element, ternary on the expression/slot/inverted, callback before/after slot
// 7/7/7/10/7/7; view and prop walk through the existing 0x25d740 wrapper and 0x26be00/0x26be30 7 (same
// code, kept); 0x25d690 through its wrapper or a noinline declaration without the pragma: inlined, 132;
// a 600 s permuter run (72 tries), none below 7.
// Direct lookup with slot callback 10; pointer arithmetic/scaled offset 7/7;
// later index reference/per-iteration index snapshot 7/7;
// slot/callback/both outside the loop 7/7/7; inverted slot test 7.
// Localised (reduced image, prop walk removed): the loop alone, or with any prefix that leaves
// no zero in ebx, gets retail's allocation; every prefix with the reset helper's zero in ebx gets
// ours. Reset with the parameter read directly 7; actor from the parameter 7; 0x28c before 0x278
// 9; location after the two stores 15; 0x4fc before the 0x4ac test 11; nested swarm test with a
// named perception index 7.
// Further trials: slot-offset/type allocation and delayed slot-address formation remain.
// uniform actor reads 157; entry iterator lifetime 7.
// native prop result 7; actor/iterator 157.
// actor/native prop 157; iterator/native prop 7.
// all three receivers 157; all three/nested view validity 157.
// Round 39: plain while loop 7; post-increment 7; `callback != NULL` 7; type in a short local 7;
// type through the element with the slot pointer passed 10.
// Round 184 (lane 3): the parameter read directly (the `&actor_index` reference dropped) 7, same
// code, kept; slot local used only as the callback argument with the type read directly 10; the
// handler's callback field read for the test and again for the call 7 (same code). Diagnostics:
// without the swarm test and call before the loop (0x28fdf0) the loop takes retail's registers
// (offset ECX, type EDX, early slot `lea eax`; 107 overall), without the inlined state reset it
// keeps ours (152): the call ahead of the loop, not the zero register, tips the allocation. The swarm
// test nested in two ifs 7; the swarm index read into a local inside the block for the call 7.
// Round 67: shared slot with named type or callback declared first 10 each; with 0x28c before 0x278 12;
// with a named swarm flag or nested swarm test and two-use perception index 10 each; with location after
// the two stores 18; with 0x4fc before the 0x4ac test 14; plain while 10; handler local 10; callback field
// at test and call through the handler local 10. The slot address still forms after the callback test and
// only the nullable-slot mask changes; none of the prefix combinations moves it or the offset/type registers.
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

// score 285 (was 285): frame 0x1c versus 0x20, entry actor displacement and cleanup stack homes remain.
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
// @retail 0x1e1a00
void __stdcall function_1e1a00(long actor_index, long value)
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
	long memory_index = 0;
	do
	{
		s_slot_memory_entry *memory = &actor_get(actor_index)->memory[memory_index];
		if (memory->type != NONE)
		{
			byte *entry = g_502424->data + (memory->unknown4 & 0xffff) * 0xbc;
			long slot = *(short *)((byte *)memory + 2);
			*(long *)(entry + slot * 12 + 4) = NONE;
			*(short *)(entry + slot * 12 + 8) = 1;
			memory->type = NONE;
		}
	} while (++memory_index < 4);
	if (actor->unknown00a[0])
		function_203fb0(actor_index);
	else
		function_203ed0(actor_index, (byte)value != 0);
	if (actor_get(actor_index)->unknown07c != NONE)
		function_2694d0(actor_get(actor_index)->unknown07c, actor_index);
	long prop_index = actor_get(actor_index)->first_prop_index;
	while (prop_index != NONE)
	{
		long current = prop_index;
		long prop_offset = (current & 0xffff) * 0x3c;
		prop_index = *(long *)((g_502418->data + prop_offset) + 0x2c);
		function_1e2150(actor_index, current);
		long view_index = *(long *)((g_502418->data + prop_offset) + 0x14);
		if (view_index != NONE)
			record_pool_release(g_502414, view_index);
		record_pool_release(g_502418, current);
	}
	actor_get(actor_index)->first_prop_index = NONE;
	long index = NONE;
	while ((index = data_next_absolute_index_inlined(g_50241c, index + 1)) != NONE)
	{
		byte *prop = g_50241c->data + g_50241c->size * index;
		if (!prop)
			break;
		if (*(long *)(prop + 0x1c) == actor_index)
			*(long *)(prop + 0x1c) = NONE;
	}
	s_actor_iterator iterator;
	function_x66da2b(&iterator, false);
	s_actor_view *other;
	while ((other = (s_actor_view *)function_1e46c0(&iterator)) != NULL)
	{
		if (other != actor_get(actor_index))
			function_1e1740(iterator.actor_index, actor_index);
	}
	function_2628f0(actor_index, g_470fa0);
	if (*(long *)((byte *)actor_get(actor_index) + 0x3f4) != NONE)
		function_26dc20(actor_index);
	if (actor_get(actor_index)->unknown007)
	{
		if (*(long *)((byte *)actor_get(actor_index) + 0x1c) != NONE)
			swarm_leader_detach(actor_index);
	}
	else
	{
		if (actor_get(actor_index)->unknown018 != NONE)
		{
			byte *unit = (byte *)ai_object_get(actor_get(actor_index)->unknown018);
			if (*(byte *)(unit + 0xaa) == 0)
				*(long *)(unit + 0x3d8) = actor_get(actor_index)->unknown054;
		}
		function_1e1250(actor_index);
	}
	byte *definition = (byte *)function_1e4990(actor_get(actor_index)->unknown054);
	if (definition && ((*(dword *)definition >> 3) & 1))
	{
		byte *counts = (byte *)g_5047f4;
		--*(short *)(counts + 6);
		if ((byte)value)
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

extern bool g_4f55e5;
bool g_4f55e0;
real function_259a0(dword *seed);
long function_1469f0(real seconds);
void *character_grenade_settings_get(long character_index, short key);
long function_cbd50(long unit_index, short weapon_index);
void __stdcall function_c6f80(long unit_index, long ticks, long flags);
void function_101a10(long weapon_index, real fraction);
void function_1018b0(long weapon_index, short const *rounds);

