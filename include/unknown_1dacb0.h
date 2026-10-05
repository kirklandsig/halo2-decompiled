/* UNKNOWN_1DACB0.H: the animation graph tag and the lookups of its
   animations (src/arg_0e6cbc.cpp, 0x1dacb0..0x1ddea0).

   An animation is named by a c_type_709360: the graph it is in (NONE for the
   graph itself, else an index into the graph's inheritance list) and its index
   in that graph's animation block. An animation's data may sit in a streamed
   resource of the graph (the cache of unknown_123680.cpp); the lookups request
   the resources of the animations they touch. */

#ifndef UNKNOWN_1DACB0_H
#define UNKNOWN_1DACB0_H

#include "unknown_11c920.h"
#include "globals.h"

struct s_cache_resource;

struct c_type_709360
{
	short graph_index;
	short index;

	c_type_709360() : graph_index(NONE), index(NONE) {}
};

/* a frame event of an animation (type 0 and 1 are the feet) */
struct s_animation_event
{
	short type;
	short frame;
};

/* the sizes of the parts of an animation's data (0x10 bytes) */
struct s_animation_data_sizes
{
	char static_node_flags_size;
	char animated_node_flags_size;
	short movement_data_size;
	short unknown04;
	short static_data_size;
	long unknown08;
	long animated_data_size;
};

/* a sound event of an animation (8 bytes) */
struct s_animation_sound_event
{
	short sound;
	short frame;
	long marker_name;
};

/* an animation (0x6c bytes) */
struct s_animation
{
	long name;
	byte unknown04[0xc];
	byte type;
	char frame_info_type;
	char blend_screen;
	byte node_count;
	short frame_count;
	byte internal_flags;
	byte unknown17;
	byte flag0 : 1;
	byte flag1 : 1;
	byte unknown18_2 : 4;
	byte flag6 : 1;
	byte unknown18_7 : 1;
	byte unknown19[3];
	real weight;
	byte unknown20[4];
	long resource_index;
	long resource_offset;
	short first_frame_index;
	short loop_frame_index;
	short parent_animation;
	short next_animation;
	long data_size;
	byte *data;
	s_animation_data_sizes sizes;
	long event_count;
	s_animation_event *events;
	long sound_event_count;
	s_animation_sound_event *sound_events;
	long effect_event_count;
	s_animation_event *field_60;
	long object_space_parent_node_count;
	struct s_object_space_parent_node *object_space_parent_nodes;
};

/* a graph the graph inherits from (0x20 bytes) */
struct s_graph_inheritance
{
	long group_tag;
	long graph_tag_index;
	long node_map_count;
	void *node_map;
	long node_map_flag_count;
	void *node_map_flags;
	real root_z_offset;
	long flags;
};

/* a blend screen (0x1c bytes) */
struct s_blend_screen
{
	long name;
	real right_yaw_per_frame;
	real left_yaw_per_frame;
	short right_frame_count;
	short left_frame_count;
	real down_pitch_per_frame;
	real up_pitch_per_frame;
	short down_frame_count;
	short up_frame_count;
};

/* a node of the graph's skeleton (0x20 bytes) */
struct s_graph_node
{
	long name;
	short next_sibling_index;
	short first_child_index;
	short parent_index;
	byte flags;
	byte joint_flags;
	byte unknown0c[0x14];
};

/* the data of an animation as the codecs read it */
struct s_animation_data
{
	byte *data;
	s_animation_data_sizes *sizes;
	byte node_count;
	char frame_info_type;
	short frame_count;

	s_animation_data() : data(NULL), sizes(NULL), node_count(0), frame_info_type(0) {}
};

/* a sound or effect the graph's animations play (12 bytes): the graph's blocks at +0x14 and +0x1c */
struct s_graph_entry
{
	long group_tag;
	long tag_index;
	word flags;
	word unknown0a;
};

/* a tag block: a count and the elements */
struct s_graph_block
{
	long count;
	void *elements;
};

/* a group of variants of a weapon type's animation (0x14 bytes, the weapon
   type's block at +0x1c) and its variants (0x14 bytes) */
struct s_graph_variant
{
	byte unknown00[4];
	long unknown04;
	long unknown08;
	char unknown0c;
	byte unknown0d;
	char unknown0e;
	char unknown0f;
	c_type_709360 animation_id;
};

struct s_graph_variant_group
{
	byte unknown00[4];
	long name;
	byte unknown08[2];
	char unknown0a;
	char unknown0b;
	long variant_count;
	s_graph_variant *variants;
};

/* an element of the graph's block at +0x3c (0x28 bytes) */
struct s_graph_element3c
{
	long unknown00;
	c_type_709360 animation_id;
	long unknown08;
	long unknown0c;
	long unknown10;
	long unknown14;
	long unknown18;
	long unknown1c;
	long unknown20;
	long unknown24;
};

/* an element of the graph's block at +0x44 (0x14 bytes) */
struct s_graph_element44
{
	long name;
	c_type_709360 animation_id;
	byte unknown08[0xc];
};

/* the iterator of the graph's block at +0x3c (0x1dceb0) */
struct s_graph_iterator3c
{
	long unknown00;
	long unknown04;
	long unknown08;
	long unknown0c;
	long unknown10;
	long unknown14;
	long unknown18;
	long unknown1c;
	long index;
	c_type_709360 animation_id;
	byte unknown28[2];
	short next_index;
};

/* the first frame of an animation's node 0 (0x18 bytes): a quantized rotation, a position and a scale */
struct s_animation_first_frame
{
	short rotation[4];
	point3f position;
	real scale;
};

/* the yaw and pitch frames of an aiming screen (a blend screen without its
   name) */
struct s_aiming_screen
{
	real right_yaw_per_frame;
	real left_yaw_per_frame;
	short right_frame_count;
	short left_frame_count;
	real down_pitch_per_frame;
	real up_pitch_per_frame;
	short down_frame_count;
	short up_frame_count;
};

/* an iterator over the pairs of a mode (function_1dcf20) or of a mode's
   weapon class (function_1dcfa0), falling back to the "any" names */
struct s_graph_pair_iterator
{
	long a;
	long b;
	short index;
	short step;
	long mode;
	long weapon_class;
};

/* the graph tag */
struct s_graph_tag
{
	c_type_709360 transition_find(long mode, long weapon_class, long weapon_type, long name, char a, char b, long c,
		long d, char e, char f, char g);
	c_type_709360 animation_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
		long *found_weapon_class, long *found_weapon_type);
	c_type_709360 overlay_get(long mode, long weapon_class, long weapon_type, long set, long *found_mode,
		long *found_weapon_class, long *found_weapon_type);
	c_type_709360 animation_find(long mode, long weapon_class, long weapon_type, long set, long item_index,
		long animation_index, long *found_mode, long *found_weapon_class, long *found_weapon_type);

	byte unknown00[0xc];
	long node_count;
	s_graph_node *nodes;
	long entry_count;
	s_graph_entry *entries;
	long effect_count;
	s_graph_entry *effects;
	byte unknown24[0x28 - 0x24];
	s_blend_screen *blend_screens;
	long animation_count;
	s_animation *animations;
	long mode_count;
	void *modes;
	long unknown3c_count;
	struct s_graph_element3c *unknown3c;
	long unknown44_count;
	struct s_graph_element44 *unknown44;
	long inheritance_count;
	s_graph_inheritance *inheritance;
	s_graph_block weapons;
	dword node_mask5c[8];
	dword node_mask7c[8];
	byte unknown9c[0xac - 0x9c];
	long resource_count;
	s_cache_resource *resources;
	long first_frame_count;
	s_animation_first_frame *first_frames;
};

inline s_graph_tag *graph_tag_get(long tag_index)
{
	return (s_graph_tag *)g_4e3b44[tag_index & 0xffff].bytes;
}

inline s_animation *graph_animation_get(s_graph_tag *graph, long index)
{
	s_animation *animation = NULL;

	if (index != NONE)
	{
		animation = &graph->animations[index];
	}
	return animation;
}

/* the graph an animation id's graph index names (inlined copies; the
   out-of-line one is function_1dafc0) */
inline s_graph_tag *graph_inherited_get(s_graph_tag *graph, long graph_index)
{
	s_graph_tag *result = NULL;

	if (graph_index < graph->inheritance_count)
	{
		s_graph_inheritance *inheritance = &graph->inheritance[graph_index];

		if (inheritance->graph_tag_index != NONE)
		{
			result = graph_tag_get(inheritance->graph_tag_index);
		}
	}
	return result;
}

/* the frame of an animation's first event of a type, or NONE (inlined
   copies; the out-of-line one is function_1dadb0) */
inline short animation_event_frame_get(s_animation const *animation, long type)
{
	long i;

	for (i = 0; i < animation->event_count; i++)
	{
		s_animation_event const *event = &animation->events[i];

		if (event->type == type)
		{
			return event->frame;
		}
	}
	return NONE;
}

/* a binary search of a sorted block (sorted_array.cpp) */
struct s_sorted_array;
void *__stdcall function_1dd560(s_sorted_array *array, long key, long element_size);

/* the animations of a weapon type (0x34 bytes): the resources they need
   first and the rest */
struct s_graph_weapon_type
{
	c_type_709360 variant_find(long name, char a, char b, long c, long d, char e, char f, char g);

	long name;
	long named_animation_count;
	struct s_graph_named_animation *named_animations;
	long overlay_count;
	struct s_graph_named_animation *overlays;
	long set_count;
	struct s_graph_set_entry *sets;
	long variant_group_count;
	s_graph_variant_group *variant_groups;
	long urgent_resource_count;
	long *urgent_resources;
	long resource_count;
	long *resources;
};

/* an entry of the graph's modes or of a mode's weapon classes (0x14 bytes):
   its name, the block below it and a block of pairs */
struct s_graph_pair
{
	long a;
	long b;
};

struct s_graph_mode_entry
{
	long name;
	long child_count;
	void *children;
	long pair_count;
	s_graph_pair *pairs;
};

/* an animation of a weapon type found by name (8 bytes) */
struct s_graph_named_animation
{
	long name;
	c_type_709360 animation_id;
};

/* the animations of a weapon type's set (0xc bytes) and the lists of ids in
   it */
struct s_graph_set_entry_item
{
	long count;
	c_type_709360 *animation_ids;
};

struct s_graph_set_entry
{
	long name;
	long item_count;
	s_graph_set_entry_item *items;
};

/* the search of a graph's weapon types with the "any" names as fallbacks
   (unknown_2963f0.cpp) */
struct s_graph_weapon_type_iterator
{
	s_graph_tag *graph;
	long mode;
	long weapon_class;
	long weapon_type;
	dword step;
	s_graph_mode_entry *mode_entry;
	s_graph_mode_entry *class_entry;
};

s_graph_weapon_type *graph_weapon_type_iterate(s_graph_weapon_type_iterator *iterator, long *found_mode,
	long *found_weapon_class, long *found_weapon_type);

/* the animations of a mode, weapon class and weapon type (inlined copies;
   the out-of-line one is function_1db120) */
inline void *graph_weapon_type_get(s_graph_tag *graph, long mode, long weapon_class, long weapon_type)
{
	void *result = NULL;
	byte *mode_entry = (byte *)function_1dd560((s_sorted_array *)&graph->mode_count, mode, 0x14);

	if (mode_entry)
	{
		byte *weapon_class_entry = (byte *)function_1dd560((s_sorted_array *)(mode_entry + 4), weapon_class, 0x14);

		if (weapon_class_entry)
		{
			result = function_1dd560((s_sorted_array *)(weapon_class_entry + 4), weapon_type, 0x34);
		}
	}
	return result;
}

s_animation *function_1daea0(s_graph_tag *graph, c_type_709360 animation_id);
s_graph_inheritance *function_1daff0(s_graph_tag *graph, c_type_709360 animation_id);
s_graph_tag *function_1dafc0(s_graph_tag *graph, long graph_index);
void function_1dd840(s_graph_tag *graph, long animation_index);
void function_1dd9d0(s_graph_tag *graph, c_type_709360 animation_id);
short function_1dadb0(s_animation const *animation, long type);
short function_1dade0(s_animation const *animation, long type, long frame);
short function_1dae20(s_animation const *animation);
short function_1dae50(s_animation const *animation);
long function_1dae80(s_animation const *animation);
real *function_1daf30(s_graph_tag *graph, c_type_709360 animation_id);
void *function_1db120(s_graph_tag *graph, long mode, long weapon_class, long weapon_type);
long function_1dd490(s_graph_tag *graph, long flags);
bool function_1dd4c0(long render_model_tag_index, s_graph_tag *graph, long *node_count, long *node_map);
c_type_709360 function_1dd5d0(s_graph_tag *graph, c_type_709360 animation_id);
c_type_709360 *function_1dd630(s_graph_tag *graph, c_type_709360 *result, c_type_709360 animation_id, bool first_seed);
byte *function_1dd7c0(s_graph_tag *graph, c_type_709360 animation_id);
void function_1dd880(s_graph_tag *graph, c_type_709360 animation_id, s_graph_tag **arg_0e6cbc, s_animation **animation);
void function_1ddab0(s_graph_tag *graph);
void function_1ddaf0(s_graph_tag *graph);
void function_1ddb40(s_animation_data *data, s_graph_tag *graph, c_type_709360 animation_id);
void function_1ddd00(s_graph_tag *graph, long mode, long weapon_class, long weapon_type, bool urgent, bool other);
struct real_quaternion_transform;
bool function_1dd8f0(s_graph_tag *graph, c_type_709360 animation_id, real_quaternion_transform *transform);
bool function_1dcf20(s_graph_tag *graph, s_graph_pair_iterator *iterator);
bool function_1dcfa0(s_graph_tag *graph, s_graph_pair_iterator *iterator);

#endif
