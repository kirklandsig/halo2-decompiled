/* UNKNOWN_1CAFC0.H: the object that drives three animation channels of one
   graph (src/unknown_1cafc0.cpp) */

#ifndef UNKNOWN_1CAFC0_H
#define UNKNOWN_1CAFC0_H

#include "unknown_11c920.h"
#include "unknown_1c62f0.h"

struct rigid_transform_scaled;
struct s_blend_orientation;

/* a 4 byte state at +0x60 and +0x64 of the animation state */
struct s_animation_bits
{
	byte unknown0;
	byte unknown1;
	byte unknown2;
	byte unknown3;

	s_animation_bits() : unknown0(0), unknown1(0), unknown2(0), unknown3(0) {}
};

/* the names an animation is looked up by */
struct s_animation_names
{
	long mode;
	long weapon_class;
	long weapon_type;
	long set;
};

/* an orientation: a quaternion, a translation and a scale */
struct real_orientation_1ce110
{
	real quaternion[4];
	point3f translation;
	real scale;
};

struct s_animation_state
{
	c_animation_channel channels[3];
	s_animation_bits unknown60;
	s_animation_bits unknown64;
	long graph_tag_index;
	word flags;
	word unknown6e;
	long unknown70;
	long unknown74;
	long unknown78;
	long unknown7c;
	real unknown80;
	vector3f unknown84;

	s_animation_state();
	void reset();
	void names_resolve(s_animation_names *names, long mode, long weapon_class, long weapon_type, long set);
	bool animation_lookup(s_animation_names *names, s_animation_names *found, long mode, long weapon_class,
		long weapon_type, long set, long lookup_flags, c_type_709360 *result);
	c_type_709360 *transition_lookup(c_type_709360 *result, long set, long mode, long weapon_class, long weapon_type,
		long *transition_set);
	c_type_709360 *transition_find(c_type_709360 *result, long mode, long set, bool *blend);
	void transition_offset_compute();
	bool animation_set(long mode, long weapon_class, long weapon_type, long set, long state_flags, long channel_flags);
	bool initialize(long graph_tag_index, long model_tag_index, bool flag);
	__declspec(noinline) void channels_clear_partial();
	short node_count_get();
	long node_find(long name);
	s_graph_entry *entry_get(long index);
	long node_count();
	s_graph_tag *graph_get();
	void update_blend_flags();
	void channels_clear();
	void secondary_channels_clear();
	c_type_709360 *current_animation_get(c_type_709360 *result);
	real blend_fraction_get();
	void animation_touch(c_type_709360 animation_id);
	bool node_map_build(long render_model_tag_index, long *node_count, long *node_map);
	bool channel_update(c_animation_channel *channel, animation_event_callback callback, long user);
	s_graph_inheritance *inheritance_get(c_type_709360 animation_id);
	c_type_709360 variant_get(c_type_709360 animation_id);
	bool channel_start(c_animation_channel *channel, c_type_709360 animation_id, long unknown08, char unknown0c,
		char unknown0d, char unknown0e, word channel_flags);
	void translation_apply(real_orientation_1ce110 *orientation, real scale);
	void resources_request(long mode, long weapon_class, long weapon_type, bool urgent, bool other);
	void channels_finish();
	void animation_transform_get(c_type_709360 animation_id, real seconds, real_quaternion_transform *transform);
	void animation_matrix_get(c_type_709360 animation_id, real seconds, long unused, transform4x3f *matrix);
	void animation_velocity_get(c_type_709360 animation_id, real seconds, real rate, long unused, vector3f *velocity);
	c_type_709360 animation_find(long name);
	void nodes_compute(transform4x3f *matrices, rigid_transform_scaled const *orientations, transform4x3f const *root);
	void orientations_blend(s_blend_orientation const *targets, short count, dword const *mask,
		s_blend_orientation *orientations);
	bool channel_play(c_animation_channel *channel, c_type_709360 animation_id, word channel_flags);
	bool channel_play_named(c_animation_channel *channel, long name, word channel_flags);
	bool blend_counters_update();
	void sample(long unused1, real weight, dword const *node_mask, real_quaternion_transform *transforms, long unused5,
		long unused6, long node_count);
	void movement_rate_get(long object_index, vector3f *vector, real *value);
	bool velocity_get(vector3f *delta, vector3f *velocity);
	c_type_709360 animation_get(long set, long weapon_class, long weapon_type);
	bool pairs_iterate(s_graph_pair_iterator *iterator);
	void nodes_compute_mirrored(transform4x3f *matrices, rigid_transform_scaled const *orientations,
		transform4x3f const *root, short mirrored_node_index, short mirror_parent_index);
	c_type_709360 overlay_default_get();
	c_type_709360 overlay_get(long set);
	bool overlay_exists();
	c_type_709360 overlay_kind_get(long kind);
	bool overlay_play(c_animation_channel *channel, word channel_flags, long set, long weapon_class, long weapon_type);
	bool channel_refresh(c_animation_channel *channel, long weapon_class, long weapon_type);
	bool update(animation_event_callback callback, long user, long node_count, s_blend_orientation *orientations,
		s_blend_orientation const *targets);
	c_type_709360 overlay_find(long set, long weapon_class, long weapon_type);
	bool channel_play_indexed(c_animation_channel *channel, long set, short item_index, short animation_index,
		short *found_item_index, short *found_animation_index);
	bool play_indexed(long set, short item_index, short animation_index, short *found_item_index,
		short *found_animation_index);
	bool play(c_type_709360 animation_id, word channel_flags);
};

/* an overlay of the set, or else its animation (0x1cbad0), and that animation's
   definition (0x1cba80, unknown_165ce5.cpp calls it) */
c_type_709360 animation_state_overlay_or_animation_get(s_animation_state *state, long weapon_class, long set,
	long weapon_type);
s_animation const *function_1cba80(s_animation_state *state, long weapon_class, long weapon_type, long set);

#endif
