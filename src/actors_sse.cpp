// @flags /O2 /arch:SSE /Gr
/* ACTORS_SSE.CPP: the actor functions built with SSE code (the rest are in
   src/actors.cpp): moving an actor's locations into the current structure
   bsp, and the actor helpers the ai script functions (unknown_272b70.cpp)
   call */

#include "unknown_11c920.h"
#include "globals.h"
#include "squads.h"
#include "ai_actor.h"
#include "props.h"

void function_1e23c0(long actor_index);
void function_1e22d0(long actor_index, bool conditional);
bool function_1e13f0(long actor_index);
void function_26dc20(long actor_index);
void function_11bed0(s_location *location, point3f const *point);
struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

/* the leaves of the structure bsp (g_4e0348) */
struct s_actor_bsp_leaf
{
	short cluster_index;
	byte unknown2[6];
};
typedef char assert_actor_bsp_leaf_size[(sizeof(s_actor_bsp_leaf) == 8) ? 1 : -1];

struct s_actor_structure_bsp
{
	byte unknown00[0x30];
	s_actor_bsp_leaf *leaves;
};

/* moves the actor into the current structure bsp (g_4686c4): drops its
   location record, and when function_1e13f0 succeeds recomputes the
   location of the actor and of each prop it knows from a point a tenth of a
   unit along g_4687b0 from their positions, and resets the tracked props'
   views; returns function_1e13f0's result */
// @retail 0x1e1500
bool actor_update_bsp_locations(long actor_index)
{
 const long *index_reference = &actor_index;
 s_actor_datum *actor = actor_datum_get(*index_reference);
 function_1e23c0(*index_reference);
 function_1e22d0(*index_reference, false);
 s_actor_datum *state = actor_datum_get(*index_reference);
 if (state->location_record_owner_index != NONE)
 {
  if (state->location_record_index != NONE)
   function_26dc20(*index_reference);
  state->location_record_owner_index = NONE;
  state->location_record_update_count = 0;
 }
 bool result = function_1e13f0(*index_reference);
 bool const *result_reference = &result;
 if (*result_reference)
 {
  actor->bsp_index = g_4686c4;
  point3f point;
  point.x = actor->position.x + g_4687b0->i * 0.1f;
  point.y = actor->position.y + g_4687b0->j * 0.1f;
  point.z = actor->position.z + g_4687b0->k * 0.1f;
  function_11bed0(&actor->location, &point);
  long prop_index = actor_get(*index_reference)->first_prop_index;
  struct prop_step
  {
   static __forceinline s_prop_node_view *next(long *cursor)
   {
    long index = *cursor;
    if (index == NONE) return NULL;
    s_prop_node_view *node = (s_prop_node_view *)(g_502418->data + (index & 0xffff) * 0x3c);
    *cursor = node->next_index;
    return node;
   }
  };
  s_prop_node_view *prop;
  while ((prop = prop_step::next(&prop_index)) != NULL)
  {
   s_type_5cfb45 *known_prop = function_25d690((s_prop_datum *)prop);
   s_type_f95cd3 *view = NULL;
   if (prop->view_index != NONE)
   {
    s_type_e5ff81 *tracking = tracking_get(prop->view_index);
    if (tracking) view = &tracking->view;
   }
   point.x = known_prop->position.x + g_4687b0->i * 0.1f;
   point.y = known_prop->position.y + g_4687b0->j * 0.1f;
   point.z = known_prop->position.z + g_4687b0->k * 0.1f;
   if (g_4686c4 == NONE)
   {
    known_prop->location.leaf_index = NONE;
    known_prop->location.cluster_index = NONE;
   }
   else
   {
    long leaf = function_14a280(g_4e033c, 0, &point);
    known_prop->location.leaf_index = leaf;
    known_prop->location.cluster_index = leaf != NONE ? ((s_actor_structure_bsp *)g_4e0348)->leaves[leaf].cluster_index : NONE;
   }
   known_prop->location.bsp_index = g_4686c4;
   known_prop->unknown44 = NONE;
   if (view && view->unknown70 == 1)
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
  }
 }
 return *result_reference;
}

/* the units of the actors (a view of the objects of g_4e0300) */
struct s_actor_unit
{
	byte unknown000[0xd4];
	long index0d4;
	byte unknown0d8[0x10a - 0xd8];
	word flags10a;
	byte unknown10c[0x134 - 0x10c];
	dword flags134;
	byte unknown138[0x2b8 - 0x138];
	real value2b8;
};

struct s_actor_unit_header
{
	byte unknown00[8];
	s_actor_unit *object;
};

inline s_actor_unit *actor_unit_get(long object_index)
{
	return ((s_actor_unit_header *)g_4e0300->data)[object_index & 0xffff].object;
}

short actor_behavior_find_level(long owner_index, short type);
void function_b7360(long object_index);
void function_b58c0(long index, dword mask);
void function_d0e00(long unit_index, real rate);
void __stdcall function_1e1a00(long index, bool value);
void swarm_creatures_set_object_flag(long perception_index, bool flag);
void function_203360(long squad_index);

/* whether the active actor, at state level 3 or more and within its first
   20 seconds of state ticks, has no behavior at level 0x38 */
// @retail 0x1e1de0
bool function_1e1de0(long actor_index)
{
	s_actor_datum *actor = actor_datum_get(actor_index);
	bool result = true;

	if (actor->active && actor->state_level > 2 && actor->state_ticks <= g_510c54->field_2_3 * 20)
	{
		if (actor_behavior_find_level(actor_index, 0x38) != NONE)
			result = false;
	}
	else
	{
		result = false;
	}
	return result;
}

/* flags the actor's swarm creature, or its unit (0x40 or 0x20 at +0x10a),
   then unless keep is set calls function_1e1a00 and updates its squad */
// @retail 0x1e2a00
void function_1e2a00(long actor_index, bool flag, bool keep)
{
	s_actor_datum *actor = actor_datum_get(actor_index);
	long squad_index = actor->squad_index;

	if (actor->swarm)
	{
		if (actor->perception_index != NONE)
			swarm_creatures_set_object_flag(actor->perception_index, flag);
	}
	else
	{
		s_actor_unit *unit = actor_unit_get(actor->unit_index);
		if (flag)
			unit->flags10a |= 0x40;
		else
			unit->flags10a |= 0x20;
		function_b7360(actor->unit_index);
	}

	if (!keep)
	{
		function_1e1a00(actor_index, 1);
		if (squad_index != NONE)
			function_203360(squad_index);
	}
}

/* the object of an object index, data first */
static __forceinline s_actor_unit *actor_unit_get_from_headers(long object_index)
{
	s_actor_unit_header *headers = (s_actor_unit_header *)g_4e0300->data;
	return headers[object_index & 0xffff].object;
}

/* with flag set marks the actor's unit (+0x134 bit 8, 0.25 at +0x2b8, its
   +0xd4 object's 0x800000 bit), else calls function_d0e00 at rate 1;
   false when it has no unit */
// @retail 0x1e32e0
bool function_1e32e0(long actor_index, bool flag)
{
	s_actor_datum volatile const *actor = actor_datum_get(actor_index);
	long unit_index = actor->unit_index;
	bool result = false;

	if (unit_index != NONE)
	{
		if (flag)
		{
			s_actor_unit *unit = actor_unit_get_from_headers(unit_index);
			unit->flags134 |= 8;
			unit->value2b8 = 0.25f;
			if (actor_unit_get_from_headers(unit_index)->index0d4 != NONE)
				function_b58c0(actor_unit_get_from_headers(unit_index)->index0d4, 0x800000);
		}
		else
		{
			function_d0e00(unit_index, 1.0f);
		}
		result = true;
	}
	return result;
}
