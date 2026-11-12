/* OBJECT_DISPATCH.H: the object type definitions (g_468630, one per object type)
   and the views of an object's header and datum that the per-type event
   dispatch (object_dispatch.cpp, object_dispatch_sse.cpp), the object list
   (object_list.cpp) and the scenery animation (scenery_animation.cpp) read */

#ifndef OBJECT_DISPATCH_H
#define OBJECT_DISPATCH_H

#include "unknown_11c920.h"
#include "globals.h"
#include "object_tag_definitions.h"
#include <stddef.h>

/* the object types, in the order of g_468630 (each definition's name is a
   retail string) */
enum e_object_kind
{
	_object_kind_biped = 0,
	_object_kind_vehicle,
	_object_kind_weapon,
	_object_kind_equipment,
	_object_kind_garbage,
	_object_kind_projectile,
	_object_kind_scenery,
	_object_kind_machine,
	_object_kind_control,
	_object_kind_light_fixture,
	_object_kind_sound_scenery,
	_object_kind_crate,
	_object_kind_creature
};

/* an object type definition (0xc8 bytes): retail holds one for each concrete
   type and one for each base type ("object", "unit", "item", "device"). Each
   event function may be NULL; the dispatch calls it, with the object index
   first, for every definition in the type's hierarchy */
struct s_object_type_definition
{
	char const *name;                     /* "object", "unit", "biped", ... */
	long group_tag;                       /* 'obje', 'unit', 'bipd', ... */
	short datum_size;                     /* 0x12c for "object", 0x13c for "scenery" */
	short unknown0a;
	long unknown0c;
	byte unknown10[0x2c - 0x10];

	/* creates the object from its placement; false (with *failed set) undoes it */
	bool (__stdcall *create)(long object_index, long placement, long failed);
	/* places the object from its scenario datum */
	void (__stdcall *place)(long object_index, long scenario_datum);
	byte unknown34[4];
	/* called once the new object is fully set up */
	void (__stdcall *finish_creation)(long object_index);
	void (__stdcall *remove_object)(long object_index);
	/* the per-tick update: true while the object stays active */
	bool (__stdcall *update)(long object_index);
	/* the second per-tick pass, after the node matrices are rebuilt */
	void (__stdcall *late_update)(long object_index);
	/* true when the type decides itself whether the object is in the clusters
	   (the answer goes to *visible) */
	bool (__stdcall *test_cluster_visibility)(long object_index, long clusters, long visible);
	/* true when the type has the named function value (*value, *enabled) */
	bool (__stdcall *query_function_value)(long object_index, long name, real *value, bool *enabled);
	/* the object gained or lost its simulation entity */
	void (__stdcall *attach_simulation_entity)(long object_index);
	void (__stdcall *detach_simulation_entity)(long object_index);
	/* another object or a player is being deleted */
	void (__stdcall *on_object_removed)(long object_index, long deleted_object_index);
	void (__stdcall *on_player_removed)(long object_index, long player_index);
	byte unknown60[4];
	/* the object's parent was destroyed (damage); true stops the walk from
	   reaching the object's own children */
	bool (__stdcall *on_parent_destroyed)(long object_index);
	/* the object was moved to a position and orientation */
	void (__stdcall *on_moved)(long object_index, long position, long forward, long up);
	/* the animation's node orientations, root matrix and node matrices are
	   ready for the type to adjust */
	void (__stdcall *adjust_node_orientations)(long object_index, long node_mask, long node_count, long orientations);
	void (__stdcall *adjust_root_matrix)(long object_index, long matrices);
	void (__stdcall *adjust_node_matrices)(long object_index, long node_count, long nodes);
	/* the object's physics was detached */
	void (__stdcall *on_physics_detached)(long object_index);
	/* a sound started playing on the object */
	void (__stdcall *on_sound_started)(long object_index, long sound_tag_index, long sound_index);
	byte unknown80[4];

	/* the definitions to dispatch to, base type first and this one last;
	   NULL ends the list */
	s_object_type_definition *type_hierarchy[16];
	byte unknownc4[4];
};
typedef char assert_object_type_definition_create[(offsetof(s_object_type_definition, create) == 0x2c) ? 1 : -1];
typedef char assert_object_type_definition_hierarchy[(offsetof(s_object_type_definition, type_hierarchy) == 0x84) ? 1 : -1];
typedef char assert_object_type_definition_size[(sizeof(s_object_type_definition) == 0xc8) ? 1 : -1];

/* the datum every object starts with ("object" datum_size 0x12c) */
struct s_object_common_datum
{
	long definition_index;
	byte unknown04[0xaa - 0x4];
	signed char type;
	byte unknownab[0xb8 - 0xab];
	long platform_index;                  /* the object list member carrying this object */
	byte unknownbc[0xc0 - 0xbc];
	union
	{
		struct
		{
			word : 1;
			word flag1 : 1;
			word carried_by_platform : 1;
			word : 3;
			word flag6 : 1;
			word : 9;
		};
		struct
		{
			byte : 3;
			byte in_object_list : 1;
			byte : 4;
		};
	};
	byte unknownc2[0x12a - 0xc2];
	short animation_state_offset;         /* NONE when the object has no animation state */
};
typedef char assert_object_common_datum_type[(offsetof(s_object_common_datum, type) == 0xaa) ? 1 : -1];
typedef char assert_object_common_datum_platform[(offsetof(s_object_common_datum, platform_index) == 0xb8) ? 1 : -1];
typedef char assert_object_common_datum_size[(sizeof(s_object_common_datum) == 0x12c) ? 1 : -1];

/* an element of the object header data (g_4e0300) */
struct s_object_header_datum
{
	short identifier;
	byte flags;                           /* 1: in use; 0x10: set by 0xb8540 */
	byte type;
	byte unknown04[4];
	s_object_common_datum *object;
};
typedef char assert_object_header_datum_size[(sizeof(s_object_header_datum) == 0xc) ? 1 : -1];

#define OBJECT_DATUM_GET(object_index) (((s_object_header_datum *)g_4e0300->data)[(object_index) & 0xffff].object)
#define OBJECT_TYPE_DEFINITION_FOR(object_index) (g_468630[OBJECT_DATUM_GET(object_index)->type])

#endif
