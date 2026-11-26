// @flags /O1 /Oi /arch:SSE /Gr
#include "orientations_storage.h"
#include "view_context_storage.h"
#include "hud_messages.h"
/* UNKNOWN_165CE5.CPP: the first person weapons of the four local
   users (0x165ce5..0x1689a6, built for size; the same object as
   unknown_165cc3.cpp, unknown_16658d.cpp and lane R's unknown_166244.cpp).
   Each user holds the unit it views from and two weapons (the second for
   dual wielding); each weapon has its own animation state, two more
   channels, the node maps of the weapon and of the arms, and the node
   matrices last built for it. */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1c62f0.h"

#include <string.h>

#define MAXIMUM_FIRST_PERSON_USERS 4
#define MAXIMUM_FIRST_PERSON_WEAPONS 2
#define MAXIMUM_FIRST_PERSON_NODES 64

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define NUMBEROF(array) (sizeof(array) / sizeof((array)[0]))
#define FLAG(bit) (1 << (bit))
#define TEST_FLAG(flags, bit) (((flags) & FLAG(bit)) != 0)
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* the flags of a first person weapon */
enum
{
	_first_person_weapon_active_bit = 0,
	_first_person_weapon_animated_bit,
	_first_person_weapon_arms_animated_bit
};

/* the flags of a first person user */
enum
{
	_first_person_user_active_bit = 0,
	_first_person_user_animated_bit,
	_first_person_user_adjusted_bit
};

/* the interpolator at +0x64 of the animation state (src/unknown_1d9240.cpp) */
struct s_1d9240
{
	char value;
	char count;
	byte unknown02;
	byte flag;
};

struct s_blend_orientation;
struct rigid_transform_scaled;
struct real_quaternion_transform;

/* the animation state of src/unknown_1cafc0.cpp, as this file sees it */
struct s_animation_state
{
	c_animation_channel channels[3];
	byte unknown60[4];
	s_1d9240 unknown64;
	long graph_tag_index;
	word flags;
	word unknown6e;
	long unknown70;
	long unknown74;
	long unknown78;
	long animation_name;
	real unknown80;
	vector3f unknown84;

	s_animation_state();
	bool animation_set(long mode, long weapon_class, long weapon_type, long set, long state_flags, long unknown);
	void reset();
	bool initialize(long graph_tag_index, long model_tag_index, bool flag);
	__declspec(noinline) void channels_clear_partial();
	s_graph_entry *entry_get(long index);
	long node_find(long name);
	bool node_map_build(long render_model_tag_index, long *node_count, long *node_map);
	s_graph_tag *graph_get();
	c_type_709360 overlay_get(long set);
	short node_count_get();
	bool pairs_iterate(s_graph_pair_iterator *iterator);
	bool channel_play(c_animation_channel *channel, c_type_709360 animation_id, dword channel_flags);
	void sample(long unused1, real weight, dword const *node_mask, real_quaternion_transform *transforms, long unused5,
		long unused6, long node_count);
	void orientations_blend(s_blend_orientation const *targets, short count, dword const *mask,
		s_blend_orientation *orientations);
	void nodes_compute(transform4x3f *matrices, rigid_transform_scaled const *orientations, transform4x3f const *root);
	void nodes_compute_mirrored(transform4x3f *matrices, rigid_transform_scaled const *orientations,
		transform4x3f const *root, short mirrored_node_index, short mirror_parent_index);
	bool channel_update(c_animation_channel *channel, animation_event_callback callback, long user);
	bool blend_counters_update();
	bool update(animation_event_callback callback, long user, long node_count, s_blend_orientation *orientations,
		s_blend_orientation const *targets);
	bool overlay_play_for_class(c_animation_channel *channel, dword channel_flags, long weapon_class, long weapon_type,
		long set);
	c_type_709360 overlay_find_for_class(long weapon_class, long weapon_type, long set);
};

/* the weapon's indices and timer (at +0xd8, 0x14 bytes) */
struct s_first_person_indices
{
	short unknown0;
	short unknown2;
	short unknown4;
	short unknown6;
	short unknown8;
	short unknowna;
	real unknownc;
	real unknown10;
};

/* the indices are three animation ids (two shorts each) at +4, +8 and +0 */
struct s_first_person_short_pair
{
	short first;
	short second;
};

static inline void first_person_short_pair_reset(s_first_person_short_pair *pair)
{
	pair->first = NONE;
	pair->second = NONE;
}

static inline void first_person_indices_reset(s_first_person_indices *indices)
{
	first_person_short_pair_reset((s_first_person_short_pair *)&indices->unknown4);
	first_person_short_pair_reset((s_first_person_short_pair *)&indices->unknown8);
	first_person_short_pair_reset((s_first_person_short_pair *)&indices->unknown0);
}

/* the first person weapon (0x1010 bytes) */
struct s_first_person_weapon
{
	dword flags;
	long weapon_index;
	s_animation_state animation;
	c_animation_channel channel98;
	c_animation_channel channelb8;
	s_first_person_indices indices;
	short unknownec;
	short unknownee;
	short unknownf0;
	short unknownf2;
	long weapon_model_index;
	long arms_model_index;
	long weapon_node_map[MAXIMUM_FIRST_PERSON_NODES];
	long arms_node_map[MAXIMUM_FIRST_PERSON_NODES];
	short unknown2fc_node;
	short unknown2fe_node;
	long orientation_count;
	long node_count;
	transform4x3f nodes[MAXIMUM_FIRST_PERSON_NODES];
	long sound_index;
	short sound_animation;
	byte unknown100e[2];
};

struct s_first_person_bits
{
	byte unknown0;
	byte unknown1;
	byte unknown2;
	byte unknown3;
};

/* a local user's first person state (0x20cc bytes) */
struct s_first_person_user
{
	dword flags;
	long unit_index;
	long character_index;
	s_first_person_weapon weapons[MAXIMUM_FIRST_PERSON_WEAPONS];
	s_first_person_bits unknown202c;
	byte unknown2030[0x2060 - 0x2030];
	transform4x3f matrix;
	long unknown2094;
	transform4x3f adjustment;
};

/* the node matrices of one model built for rendering (0xd0c bytes) */
struct s_first_person_model
{
	long render_model_index;
	long object_index;
	long unknown08;
	transform4x3f nodes[MAXIMUM_FIRST_PERSON_NODES];
};

/* an object marker (0x70 bytes): its node matrix, then the marker's own */
struct s_first_person_marker
{
	short node_index;
	short unknown02;
	transform4x3f node_matrix;
	transform4x3f matrix;
	byte unknown6c[4];
};

/* an animation event (the sound events 166d50 and 166d62 receive) */
struct s_first_person_event
{
	short type;
	short unknown02;
	long sound_tag_index;
	byte unknown08[4];
	byte flags;
};

/* the objects, as this file reads them */
struct s_first_person_object
{
	long definition_index;
	byte unknown004[0x13c - 0x4];
	long player_index;
	byte unknown140[0x154 - 0x140];
	long unit_index;
};

struct s_first_person_object_header
{
	byte unknown00[8];
	s_first_person_object *object;
};

/* the weapon's first person block entries (16 bytes) */
struct s_first_person_interface
{
	byte unknown00[4];
	long render_model_index;
	byte unknown08[4];
	long animation_graph_index;
};

struct s_first_person_weapon_definition
{
	byte unknown000[0x12c];
	dword unknown12c_bits0 : 17;
	dword unknown12c_bit17 : 1;
	dword unknown12c_bits18 : 14;
	byte unknown130[0x292 - 0x130];
	short unknown292;
	byte unknown294[0x2a8 - 0x294];
	long interface_count;
	s_first_person_interface *interfaces;
	byte unknown2b0[0x2b8 - 0x2b0];
	byte unknown2b8[4];
};

/* the render model (only the node count at +0x48 is read here) */
struct s_first_person_render_model
{
	byte unknown00[0x48];
	long node_count;
};

/* the globals' player representations (0xbc bytes each) */
struct s_player_representation
{
	byte unknown00[4];
	long arms_render_model_index;
	byte unknown08[4];
	long arms_animation_graph_index;
	byte unknown10[0xbc - 0x10];
};

struct s_first_person_globals_view
{
	byte unknown000[0x138];
	long representation_count;
	s_player_representation *representations;
};

/* the players (0x21c bytes) */
struct s_first_person_player
{
	byte unknown000[0x28];
	short local_user_index;
	byte unknown02a[2];
	long unit_index;
	byte unknown030[0x88 - 0x30];
	char character_type;
	byte unknown089[0x21c - 0x89];
};

/* the users (defined in unknown_16658d.cpp) and the orientation buffers, 0x1000
   bytes per weapon (unknown_165cc3.cpp allocates both) */
struct s_16658d_group;
extern s_16658d_group *g_4e9bc8;
#define first_person_users ((s_first_person_user *)g_4e9bc8)
extern void *g_165cc3_aligned_data;
#define first_person_orientations ((byte *)g_165cc3_aligned_data)

extern transform4x3f *g_4687d0;

/* callees in other files */
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_141590(transform4x3f const *in, transform4x3f *out);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
long function_14de70(long local_player_index);
long function_155760(long index);
struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
long function_baf80(long object_index);
long function_cbd50(long unit_index, short weapon_index);
bool function_cd660(long unit_index);
long function_1469f0(real seconds);
long function_189060(long object_index, short value, point3f const *position, vector3f const *direction, real scale, long tag_index);
void function_1d9240(s_1d9240 *p, char flag, real x);
long function_16658d(long group_index, long key);
real function_1d9430(s_1d9240 const *p);
long unit_get_player_index(long unit_index);
/* lane R's unknown_166244.cpp */
long function_166244(long key);

/* 0x1d9430 is in unknown_1d9240.cpp and 0x14de90 in unknown_14b560.cpp; 0x1cba80 is in
   unknown_1cafc0.cpp (state, weapon_class, weapon_type, set) */
real function_1d9430(s_1d9240 const *p);
s_animation const *function_1cba80(s_animation_state *state, long weapon_class, long weapon_type, long name);
long unit_get_player_index(long unit_index);
void function_1776e0(long user_index, long object_index, bool add); /* unknown_175bd0.cpp */
short function_1d90b0(long render_model_index, long marker_name, byte const *unknown0, long model_index, long const *node_map,
	long node_map_count, transform4x3f const *nodes, bool unknown1, s_first_person_marker *markers, long marker_count);

static inline s_first_person_object *first_person_object_get(long object_index)
{
	return ((s_first_person_object_header *)g_4e0300->data)[object_index & 0xffff].object;
}

static inline s_first_person_weapon_definition *first_person_object_definition_get(s_first_person_object *object)
{
	return (s_first_person_weapon_definition *)g_4e3b44[object->definition_index & 0xffff].bytes;
}

static inline s_first_person_player *first_person_player_get(long player_index)
{
	return (s_first_person_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_first_person_player));
}

// @retail 0x16896f
long first_person_character_from_player(long character)
{
	if (g_4e6948->state == 2)
	{
		if (character == 0)
		{
			character = 2;
		}
		if (character == 1)
		{
			character = 3;
		}
	}
	return character;
}

// @retail 0x16898b
long first_person_character_to_interface(long character)
{
	if (g_4e6948->state == 2)
	{
		if (character == 2)
		{
			character = 0;
		}
		if (character == 3)
		{
			character = 1;
		}
	}
	return character;
}

// @retail 0x168395
void first_person_nodes_remap(transform4x3f const *base, transform4x3f *out, long out_count,
	transform4x3f const *nodes, long const *node_map, long render_model_index)
{
	s_first_person_render_model *model = (s_first_person_render_model *)g_4e3b44[render_model_index & 0xffff].bytes;
	long count = model->node_count;
	long i;
	if (count > out_count)
		count = out_count;

	for (i = 0; i < count; i++)
	{
		long node_index = node_map[i];

		if (node_index != NONE)
		{
			if (base)
			{
				function_142a60(base, &nodes[node_index], &out[i]);
			}
			else
			{
				out[i] = nodes[node_index];
			}
		}
	}
}

// @retail 0x165e9f
void first_person_model_build(long render_model_index, long node_count, transform4x3f const *nodes, long object_index, long unknown08,
	transform4x3f const *base, long const *node_map, s_first_person_model *model)
{
	model->object_index = object_index;
	model->render_model_index = render_model_index;
	model->unknown08 = unknown08;
	if (node_map)
	{
		first_person_nodes_remap(base, model->nodes, NUMBEROF(model->nodes), nodes, node_map, render_model_index);
	}
	else
	{
		node_count = MIN((dword)node_count, NUMBEROF(model->nodes));
		if (base)
		{
			long i;

			for (i = 0; i < node_count; i++)
			{
				function_142a60(base, &nodes[i], &model->nodes[i]);
			}
		}
		else
		{
			memcpy(model->nodes, nodes, node_count * sizeof(transform4x3f));
		}
	}
}

// @retail 0x1662c1
short first_person_weapon_get_markers_internal(long weapon_index, long marker_name, s_first_person_marker *markers, short marker_count)
{
	short result = 0;
	s_object *weapon = function_badc0(weapon_index, 4);

	if (weapon)
	{
		long user_index = function_166244(weapon_index);

		if (user_index != NONE && !function_155760(user_index))
		{
			long field_x11c898 = function_16658d(user_index, weapon_index);

			if (field_x11c898 != NONE)
			{
				s_first_person_user *user = &first_person_users[user_index];
				s_first_person_weapon *fp_weapon = &user->weapons[field_x11c898];

				if (marker_name != 0xa0000b6)
				{
					s_first_person_interface *interface = &first_person_object_definition_get((s_first_person_object *)weapon)->interfaces[first_person_character_to_interface(user->character_index)];
					long render_model_index = interface->render_model_index;

					if (TEST_FLAG(fp_weapon->flags, _first_person_weapon_animated_bit) && render_model_index != NONE && interface->animation_graph_index != NONE)
					{
						result = function_1d90b0(render_model_index, marker_name, 0, fp_weapon->weapon_model_index, fp_weapon->weapon_node_map,
							MAXIMUM_FIRST_PERSON_NODES, fp_weapon->nodes, 0, markers, marker_count);
					}
				}
				else
				{
					long render_model_index = ((s_first_person_globals_view *)g_4e034c)->representations[user->character_index].arms_render_model_index;

					if (TEST_FLAG(fp_weapon->flags, _first_person_weapon_arms_animated_bit) && render_model_index != NONE)
					{
						result = function_1d90b0(render_model_index, marker_name, 0, fp_weapon->arms_model_index, fp_weapon->arms_node_map,
							MAXIMUM_FIRST_PERSON_NODES, fp_weapon->nodes, 0, markers, marker_count);
					}
				}

				{
					long i;

					for (i = 0; i < result; i++)
					{
						function_142a60(&user->matrix, &markers[i].matrix, &markers[i].matrix);
					}
				}
			}
		}
	}
	return result;
}

// @retail 0x1662a1
short first_person_weapon_get_markers(long weapon_index, long marker_name, s_first_person_marker *markers, short marker_count)
{
	if (g_4b9ed8 == function_166244(weapon_index))
	{
		return first_person_weapon_get_markers_internal(weapon_index, marker_name, markers, marker_count);
	}
	return 0;
}

// @retail 0x166561
long first_person_weapon_mode_name(long user_index)
{
	s_first_person_user *user = &first_person_users[user_index];
	long result = 0x30000d9;

	if (user->unit_index != NONE && function_cd660(user->unit_index))
	{
		result = 0x400054b;
	}
	return result;
}

// @retail 0x1665c2
long first_person_weapon_state_animation(short state)
{
	long result = NONE;

	switch (state)
	{
	case 0: result = 0x400000c; break;
	case 1: result = 0x6000006; break;
	case 2: result = 0x6000007; break;
	case 3: result = 0x900006a; break;
	case 4: result = 0x900006b; break;
	case 5: result = 0x8000063; break;
	case 6: result = 0x9000062; break;
	case 7: result = 0xc000064; break;
	case 8: result = 0xb000065; break;
	case 13: result = 0xb000067; break;
	case 14: result = 0xb00006d; break;
	case 15: result = 0x8000071; break;
	case 9: result = 0xc0005ae; break;
	case 10: result = 0x150005af; break;
	case 11: result = 0x140005b0; break;
	case 12: result = 0xb0005b1; break;
	case 17: result = 0x500000a; break;
	case 18:
	case 19: result = 0x8000025; break;
	case 20:
	case 21: result = 0x5000024; break;
	case 22:
	case 23: result = 0x1000006e; break;
	case 26: result = 0xd000021; break;
	case 27: result = 0x1000006c; break;
	case 28: result = 0xc000073; break;
	case 29: result = 0x11000074; break;
	case 30: result = 0xc000075; break;
	case 31: result = 0x11000076; break;
	case 32: result = 0xc000077; break;
	case 33: result = 0xe000607; break;
	case 34: result = 0xe000608; break;
	case 35: result = 0xe000609; break;
	case 36: result = 0xe00060a; break;
	case 37: result = 0xa0005bb; break;
	case 38: result = 0xb0005b2; break;
	case 39: result = 0x130005bc; break;
	case 40: result = 0x140005b3; break;
	}
	return result;
}

// @retail 0x166c39
void first_person_weapon_save_orientations(long user_index, long field_x11c898, real blend_time)
{
	s_first_person_user *user = &first_person_users[user_index];
	s_first_person_weapon *weapon = &user->weapons[field_x11c898];

	if (TEST_FLAG(user->flags, _first_person_user_animated_bit))
	{
		byte *orientations = first_person_orientations +(field_x11c898 + user_index * MAXIMUM_FIRST_PERSON_WEAPONS) * 0x1000;

		memcpy(orientations + 0x800, orientations, weapon->orientation_count * 0x20);
		if (blend_time >= function_1d9430(&weapon->animation.unknown64))
		{
			function_1d9240(&weapon->animation.unknown64, true, blend_time);
		}
	}
}

// @retail 0x166cb0
void first_person_weapon_reset_blend(long user_index, long field_x11c898)
{
	s_1d9240 *blend = &first_person_users[user_index].weapons[field_x11c898].animation.unknown64;

	blend->count = 0;
	blend->value = 0;
	blend->flag = 0;
}

static inline s_first_person_user *first_person_user_get(long user_index)
{
	return &first_person_users[user_index];
}

// @retail 0x166cd1
void first_person_weapon_sound_event(long user_index, long field_x11c898, s_first_person_event const *event)
{
	s_first_person_user *user = first_person_user_get(user_index);

	if (user && event->type == 1)
	{
		long sound_tag_index = event->sound_tag_index;

		if (sound_tag_index != NONE && (!function_155760(user_index) || !(event->flags & 8)))
		{
			s_first_person_weapon *weapon = &user->weapons[field_x11c898];

			weapon->sound_index = function_189060(weapon->weapon_index, NONE, g_468788, g_4687a8, 1.0f, sound_tag_index);
			weapon->sound_animation = (short)weapon->animation.animation_name;
		}
	}
}

// @retail 0x166d50
void __stdcall first_person_weapon_sound_event_right(long user_index, real frame, s_first_person_event const *event)
{
	first_person_weapon_sound_event(user_index, 0, event);
}

// @retail 0x166d62
void __stdcall first_person_weapon_sound_event_left(long user_index, real frame, s_first_person_event const *event)
{
	first_person_weapon_sound_event(user_index, 1, event);
}

// @retail 0x168311
void first_person_weapon_set_active(long user_index, long field_x11c898, bool active)
{
	s_first_person_weapon *weapon = &first_person_users[user_index].weapons[field_x11c898];

	if (active != TEST_FLAG(weapon->flags, _first_person_weapon_active_bit))
	{
		SET_FLAG(weapon->flags, _first_person_weapon_active_bit, active);
	}
}

// @retail 0x16834a
void first_person_weapon_set_object(long user_index, long field_x11c898, long weapon_index)
{
	s_first_person_weapon *weapon = &first_person_users[user_index].weapons[field_x11c898];

	if (weapon_index != weapon->weapon_index)
	{
		if (weapon->weapon_index != NONE)
		{
			function_1776e0(user_index, weapon->weapon_index, false);
		}
		weapon->weapon_index = weapon_index;
		if (weapon_index != NONE)
		{
			function_1776e0(user_index, weapon_index, true);
		}
	}
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// @retail 0x168470
s_animation const *first_person_weapon_animation_get(long weapon_index, long animation_name, long *frame_out)
{
	s_animation const *result = NULL;
	s_first_person_object *weapon = first_person_object_get(weapon_index);
	s_first_person_weapon_definition *definition = first_person_object_definition_get(weapon);

	if (weapon->unit_index != NONE)
	{
		s_first_person_object *unit = first_person_object_get(weapon->unit_index);

		if (unit->player_index != NONE)
		{
			long character = first_person_player_get(unit->player_index)->character_type;

			if (character >= 0 && character < definition->interface_count)
			{
				s_first_person_interface *interface = &definition->interfaces[first_person_character_to_interface(character)];

				if (interface->animation_graph_index != NONE)
				{
					s_animation_state state;

					if (state.initialize(interface->animation_graph_index, NONE, true))
					{
						long weapon_type = 0x7000101;
						long weapon_class = weapon_type;

						if (function_cd660(weapon->unit_index))
						{
							weapon_class = 0x400054b;
						}
						result = function_1cba80(&state, weapon_class, weapon_type, animation_name);
						if (result && (short)function_1dae80(result) != NONE && frame_out)
						{
							*frame_out = *(long *)((byte *)state.entry_get((short)function_1dae80(result)) + 4);
						}
					}
					state.channels_clear_partial();
				}
			}
		}
	}
	return result;
}

// @retail 0x1685a6
short first_person_weapon_animation_ticks(long weapon_index, long animation_name, short type)
{
	short result = 0;
	s_animation const *animation = first_person_weapon_animation_get(weapon_index, animation_name, NULL);

	if (animation)
	{
		long frame_count = *(short const *)((byte const *)animation + 0x14);
		long event_frame = function_1dadb0(animation, 4);
		long frame;

		switch (type)
		{
		case 1:
			if (event_frame != NONE && frame_count > event_frame)
			{
				frame = event_frame;
				break;
			}
		case 0:
			frame = frame_count;
			break;
		case 2:
			frame = function_1dae20(animation);
			break;
		default:
			frame = function_1dae20(animation);
			if (frame <= 0)
			{
				frame = frame_count;
			}
			break;
		}

		if (frame == NONE)
		{
			result = NONE;
		}
		else if (frame > 0)
		{
			result = (short)function_1469f0((real)frame * (1.0f / 30.0f));
			result = MAX(result, 1);
		}
	}
	return result;
}

// @retail 0x16640f
bool first_person_weapon_get_marker(long object_index, long marker_name, point3f *position, vector3f *forward, vector3f *up)
{
	s_first_person_marker marker;
	long unit_index = function_baf80(object_index);
	long user_index = unit_get_player_index(unit_index) == NONE ? NONE : first_person_player_get(unit_get_player_index(unit_index))->local_user_index;
	bool result = false;

	if (user_index != NONE)
	{
		s_first_person_weapon *weapon = &first_person_users[user_index].weapons[0];

		if (TEST_FLAG(weapon->flags, _first_person_weapon_active_bit) &&
			first_person_weapon_get_markers_internal(weapon->weapon_index, marker_name, &marker, 1))
		{
			*position = marker.matrix.position;
			*forward = marker.matrix.forward;
			*up = marker.matrix.up;
			result = true;
		}
	}
	return result;
}
static inline void first_person_bits_reset(s_first_person_bits *bits)
{
	bits->unknown1 = 0;
	bits->unknown0 = 0;
	bits->unknown3 = 0;
}

// @retail 0x165ce5
void function_165ce5(void)
{
	long user_index;

	for (user_index = 0; user_index < MAXIMUM_FIRST_PERSON_USERS; user_index++)
	{
		s_first_person_user *user = &first_person_users[user_index];
		long field_x11c898;

		memset(user, 0, sizeof(s_first_person_user));
		user->unit_index = NONE;
		user->character_index = NONE;
		user->unknown2094 = NONE;
		user->matrix = *g_4687d0;
		for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
		{
			s_first_person_weapon *weapon = &user->weapons[field_x11c898];

			weapon->weapon_index = NONE;
			weapon->sound_index = NONE;
			weapon->sound_animation = NONE;
			weapon->animation.reset();
			weapon->channel98.reset();
			weapon->channelb8.reset();
			first_person_indices_reset(&weapon->indices);
		}
		first_person_bits_reset(&user->unknown202c);
		user->unknown202c.unknown2 = 1;
	}
}

void __stdcall function_167e86(long user_index, long field_x11c898);

/* in its own file (unknown_1682bf.cpp): retail calls it out of line */
void function_1682bf(long unit_index, long user_index, long character_index);

// @retail 0x1682af
void function_1682af(long user_index)
{
	function_1682bf(NONE, user_index, NONE);
}

// @retail 0x165db0
void function_165db0(void)
{
	long user_index;

	for (user_index = 0; user_index < MAXIMUM_FIRST_PERSON_USERS; user_index++)
	{
		function_1682af(user_index);
	}
}

// @retail 0x16651c
void __stdcall function_16651c(long weapon_index)
{
	long user_index;

	for (user_index = 0; user_index < MAXIMUM_FIRST_PERSON_USERS; user_index++)
	{
		s_first_person_user *user = &first_person_users[user_index];
		long field_x11c898;

		for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
		{
			if (user->weapons[field_x11c898].weapon_index == weapon_index)
			{
				function_167e86(user_index, field_x11c898);
			}
		}
	}
}

struct s_predicted_resource_block;
bool function_16e5e0(s_predicted_resource_block const *block, short mode);

// @retail 0x16840e
void function_16840e(long user_index, long field_x11c898)
{
	s_first_person_weapon *weapon = &first_person_users[user_index].weapons[field_x11c898];

	if (weapon->weapon_index != NONE)
	{
		s_first_person_weapon_definition *definition = first_person_object_definition_get(first_person_object_get(weapon->weapon_index));

		function_16e5e0((s_predicted_resource_block const *)definition->unknown2b8, 0);
	}
	weapon->unknownf0 = 30;
}

/* the units' weapon slots, as read here */
struct s_first_person_unit_view
{
	byte unknown000[0x212];
	char weapon_slots[MAXIMUM_FIRST_PERSON_WEAPONS];
};

void __stdcall function_166d75(long user_index);

// @retail 0x165dc1
void function_165dc1(void)
{
	long user_index;

	for (user_index = 0; user_index < MAXIMUM_FIRST_PERSON_USERS; user_index++)
	{
		long player_index = function_14de70(user_index);

		if (player_index != NONE)
		{
			s_first_person_user *user = &first_person_users[user_index];
			s_first_person_player *player = first_person_player_get(player_index);
			long character_index = first_person_character_from_player(player->character_type);

			if (user->unit_index != player->unit_index || user->character_index != character_index)
			{
				function_1682bf(player->unit_index, user_index, character_index);
			}
			if (user->unit_index != NONE)
			{
				long field_x11c898;

				for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
				{
					s_first_person_unit_view *unit = (s_first_person_unit_view *)first_person_object_get(user->unit_index);

					if (user->weapons[field_x11c898].weapon_index != function_cbd50(user->unit_index, unit->weapon_slots[field_x11c898]))
					{
						function_167e86(user_index, field_x11c898);
					}
				}
				function_166d75(user_index);
			}
		}
	}
}

void first_person_weapon_set_animation(long user_index, long field_x11c898, long animation_name, bool restart);
/* not decompiled yet (src/stubs/lane_t.cpp) */
bool __stdcall function_105c20(long weapon_index, long animation_name);

// @retail 0x168896
void function_168896(long user_index, long weapon_index, long field_x11c898, long state)
{
	long animation_name = first_person_weapon_state_animation(state);

	if (user_index != NONE)
	{
		s_first_person_weapon *weapon = &first_person_users[user_index].weapons[field_x11c898];
		bool restart;

		switch (state)
		{
		case 1:
			weapon->unknownf2 = 4;
			weapon->indices.unknown10 += 0.05f;
			break;
		case 20:
		case 21:
		case 24:
		case 25:
			{
				long slot;

				for (slot = 0; slot < MAXIMUM_FIRST_PERSON_WEAPONS; slot++)
				{
					function_167e86(user_index, slot);
				}
			}
			break;
		}
		restart = weapon->sound_index != NONE && weapon->sound_animation != animation_name;
		if (state >= 10 && state <= 12)
		{
			restart = false;
		}
		if (animation_name != NONE)
		{
			first_person_weapon_set_animation(user_index, field_x11c898, animation_name, restart);
		}
	}
	else if (weapon_index != NONE)
	{
		function_105c20(weapon_index, animation_name);
	}
}

/* not decompiled yet (src/stubs/lane_t.cpp) */
void function_126360(long sound_index);

// @retail 0x166992
void first_person_weapon_set_animation(long user_index, long field_x11c898, long animation_name, bool restart)
{
	s_first_person_user *user = &first_person_users[user_index];
	s_first_person_weapon *weapon = &user->weapons[field_x11c898];

	if (user->unit_index != NONE && weapon->weapon_index != NONE)
	{
		s_first_person_weapon_definition *definition = first_person_object_definition_get(first_person_object_get(weapon->weapon_index));
		long current_animation = weapon->animation.animation_name;
		long field_7c = first_person_weapon_mode_name(user_index);
		real blend_time;
		long flags;

		switch (animation_name)
		{
		case 0x5000024:
			if (current_animation == 0x5000024)
			{
				animation_name = NONE;
			}
			break;
		case 0x6000006:
			if (current_animation == 0x6000006 && TEST_FIELD_BIT(definition->unknown12c_bit17))
			{
				return;
			}
			break;
		case 0x8000063:
		case 0x9000062:
			if (current_animation != 0x400000c && current_animation != 0x600005f)
			{
				animation_name = NONE;
			}
			break;
		case 0x1000006e:
			if (current_animation == 0x1000006e)
			{
				animation_name = NONE;
			}
			break;
		}
		if (animation_name == NONE)
		{
			return;
		}

		blend_time = 0.267f;
		switch (animation_name)
		{
		case 0x500000a:
		case 0xb000065:
		case 0xc000064:
		case 0xc000073:
		case 0xc0005ae:
		case 0xe000607:
		case 0xe000608:
		case 0xe000609:
		case 0xe00060a:
			blend_time = 0.1335f;
			break;
		case 0x5000024:
		case 0x6000006:
		case 0x6000007:
		case 0x900006a:
		case 0x900006b:
		case 0xa000066:
		case 0xb0005b1:
		case 0x1000006e:
			blend_time = 0.0f;
			break;
		}
		switch (animation_name)
		{
		case 0x500000a:
		case 0x5000024:
		case 0xc000073:
		case 0xc000075:
		case 0xc000077:
		case 0xe000607:
		case 0xe000608:
		case 0xe000609:
		case 0xe00060a:
		case 0x1000006e:
			first_person_weapon_reset_blend(user_index, field_x11c898);
			break;
		}
		if (blend_time > 0.0f)
		{
			first_person_weapon_save_orientations(user_index, field_x11c898, blend_time);
		}

		flags = 0x3f;
		if (field_7c == 0x400054b)
		{
			flags = field_x11c898 ? 0x203f : 0x103f;
		}
		if (weapon->animation.animation_set(0x7000101, field_7c, 0x7000001, animation_name, 0x82, flags) && restart &&
			weapon->sound_index != NONE && weapon->sound_animation != 0xb00006d)
		{
			function_126360(weapon->sound_index);
			weapon->sound_index = NONE;
			weapon->sound_animation = NONE;
		}
	}
}

bool function_ee8a0(long unit_index, long field_x11c898);

// @retail 0x16674e
void first_person_weapon_animation_finished(long user_index, long field_x11c898)
{
	s_first_person_user *user = &first_person_users[user_index];
	s_first_person_weapon *weapon = &user->weapons[field_x11c898];
	s_first_person_weapon_definition *definition = first_person_object_definition_get(first_person_object_get(weapon->weapon_index));
	long animation_name = weapon->animation.animation_name;
	long next_animation = NONE;

	switch (animation_name)
	{
	case 0x400000c:
	case 0x500000a:
	case 0x5000024:
	case 0x600005f:
	case 0x8000063:
	case 0x8000071:
	case 0x9000062:
	case 0x900006a:
	case 0x900006b:
	case 0xb000065:
	case 0xb0005b1:
	case 0xb0005b2:
	case 0xc000064:
	case 0xc000077:
	case 0xd000021:
	case 0xe000607:
	case 0xe000608:
	case 0xe000609:
	case 0xe00060a:
	case 0x11000074:
	case 0x11000076:
	case 0x140005b3:
		next_animation = 0x400000c;
		break;
	case 0x6000006:
	case 0x6000007:
		if (!TEST_FIELD_BIT(definition->unknown12c_bit17) || weapon->unknownf2 <= 0)
		{
			next_animation = 0x400000c;
		}
		if (definition->unknown292 == 3 && animation_name == 0x6000007)
		{
			next_animation = 0xa000066;
		}
		break;
	case 0x8000025:
		weapon->animation.flags |= 1;
		break;
	case 0xa000066:
		if (!function_ee8a0(user->unit_index, field_x11c898))
		{
			next_animation = 0x8000071;
		}
		break;
	case 0xa0005bb:
		next_animation = 0xb0005b2;
		break;
	case 0xb00006d:
	case 0x1000006c:
	case 0x1000006e:
		next_animation = 0xa000066;
		break;
	case 0xc000073:
		next_animation = 0x11000074;
		break;
	case 0xc000075:
		next_animation = 0x11000076;
		break;
	case 0xc0005ae:
	case 0x140005b0:
	case 0x150005af:
		next_animation = 0xb0005b1;
		break;
	case 0x130005bc:
		next_animation = 0x140005b3;
		break;
	}
	if (next_animation != NONE)
	{
		first_person_weapon_set_animation(user_index, field_x11c898, next_animation, false);
	}
}
static inline void transform4x3f_set_identity(transform4x3f *matrix)
{
	matrix->scale = 1.0f;
	matrix->forward.i = 1.0f;
	matrix->forward.j = 0.0f;
	matrix->forward.k = 0.0f;
	matrix->left.i = 0.0f;
	matrix->left.j = 1.0f;
	matrix->left.k = 0.0f;
	matrix->up.i = 0.0f;
	matrix->up.j = 0.0f;
	matrix->up.k = 1.0f;
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
}

/* rebuilds a weapon of a user from the unit's current weapon in that slot:
   its animation state, the node maps of the weapon and the arms, the
   overlays it plays */
// @retail 0x167e86
void __stdcall function_167e86(long user_index, long field_x11c898)
{
	s_first_person_globals_view *globals = (s_first_person_globals_view *)g_4e034c;
	s_first_person_user *user = &first_person_users[user_index];
	s_first_person_weapon *weapon = &user->weapons[field_x11c898];
	bool active = TEST_FLAG(weapon->flags, _first_person_weapon_active_bit);

	if (active)
	{
		first_person_weapon_set_active(user_index, field_x11c898, false);
	}
	first_person_weapon_set_object(user_index, field_x11c898, NONE);
	SET_FLAG(user->flags, _first_person_user_animated_bit, false);
	transform4x3f_set_identity(&user->adjustment);
	SET_FLAG(user->flags, _first_person_user_adjusted_bit, false);
	weapon->node_count = 0;
	SET_FLAG(weapon->flags, _first_person_weapon_animated_bit, false);
	SET_FLAG(weapon->flags, _first_person_weapon_arms_animated_bit, false);

	if (user->unit_index != NONE && user->character_index >= 0 && user->character_index < globals->representation_count)
	{
		s_player_representation *representation = &globals->representations[user->character_index];
		s_first_person_unit_view *unit = (s_first_person_unit_view *)first_person_object_get(user->unit_index);
		long weapon_index = function_cbd50(user->unit_index, unit->weapon_slots[field_x11c898]);

		if (weapon_index != NONE)
		{
			s_first_person_weapon_definition *definition = first_person_object_definition_get(first_person_object_get(weapon_index));
			long interface_index = first_person_character_to_interface(user->character_index);

			if (interface_index >= 0 && interface_index < definition->interface_count)
			{
				s_first_person_interface *interface = &definition->interfaces[interface_index];
				if (interface->render_model_index != NONE && interface->animation_graph_index != NONE)
				{
					s_animation_state *state;

					first_person_bits_reset(&user->unknown202c);
					state = &weapon->animation;
					state->initialize(interface->animation_graph_index, NONE, false);
					if (field_x11c898 == 0)
					{
						user->unknown2094 = state->node_find(0xe0000e0);
					}
					if (representation->arms_render_model_index != NONE &&
						state->node_map_build(representation->arms_render_model_index, &weapon->arms_model_index, weapon->arms_node_map))
					{
						SET_FLAG(weapon->flags, _first_person_weapon_arms_animated_bit, true);
						if (field_x11c898 != 0)
						{
							s_graph_tag *graph = state->graph_get();
							long node_index;

							for (node_index = 0; node_index < weapon->arms_model_index; node_index++)
							{
								long graph_node_index = weapon->arms_node_map[node_index];

								if (graph_node_index != NONE && !(graph->nodes[graph_node_index].flags & 0x20))
								{
									weapon->arms_node_map[node_index] = NONE;
								}
							}
						}
					}
					if (state->node_map_build(interface->render_model_index, &weapon->weapon_model_index, weapon->weapon_node_map))
					{
						SET_FLAG(weapon->flags, _first_person_weapon_animated_bit, true);
						weapon->unknown2fc_node = NONE;
						weapon->unknown2fe_node = NONE;
						if (field_x11c898 != 0 && weapon->weapon_model_index != 0)
						{
							s_graph_tag *graph = state->graph_get();
							long root_node_index = weapon->weapon_node_map[0];

							if (root_node_index != NONE)
							{
								s_graph_node const *root_node = &graph->nodes[root_node_index];

								if (root_node->parent_index != NONE)
								{
									short node_index;

									if (graph->nodes[root_node->parent_index].flags & 8)
									{
										node_index = (short)function_1dd490(graph, 0x10);
									}
									else
									{
										node_index = (short)function_1dd490(graph, 8);
									}

									weapon->unknown2fe_node = node_index;
									if (node_index != NONE)
									{
										weapon->unknown2fc_node = (short)root_node_index;
									}
								}
							}
						}
					}

					if (TEST_FLAG(weapon->flags, _first_person_weapon_animated_bit) &&
						TEST_FLAG(weapon->flags, _first_person_weapon_arms_animated_bit))
					{
						long mode = first_person_weapon_mode_name(user_index);

						first_person_weapon_set_object(user_index, field_x11c898, weapon_index);
						weapon->unknownec = 0;
						weapon->indices.unknownc = 0.0f;
						weapon->indices.unknown10 = 0.0f;
						first_person_weapon_set_animation(user_index, field_x11c898, 0x400000c, true);
						weapon->sound_index = NONE;
						weapon->sound_animation = NONE;
						weapon->channelb8.clear();
						weapon->channel98.clear();
						first_person_indices_reset(&weapon->indices);
						state->overlay_play_for_class(&weapon->channelb8, 0x803f, mode, 0x7000101, 0x12000068);
						*(c_type_709360 *)&weapon->indices.unknown4 = state->overlay_find_for_class(mode, 0x7000101, 0x8000061);
						*(c_type_709360 *)&weapon->indices.unknown8 = state->overlay_find_for_class(mode, 0x7000101, 0xa000069);
						*(c_type_709360 *)&weapon->indices.unknown0 = state->overlay_find_for_class(mode, 0x7000101, 0xe0000de);
						if (active)
						{
							first_person_weapon_set_active(user_index, field_x11c898, true);
						}
					}
				}
			}
		}
	}
	function_16840e(user_index, field_x11c898);
}

/* the parts of the unit, the weapon and their definitions read here */
struct s_166d75_unit
{
	byte unknown000[0x1b0];
	vector3f aiming_velocity;
};

struct s_166d75_weapon_object
{
	byte unknown000[0x188];
	real unknown188;
};

struct s_166d75_weapon_definition
{
	byte unknown000[0x2e0];
	real unknown2e0;
	real unknown2e4;
	real unknown2e8;
	real unknown2ec;
};

/* the first person globals (g_4e034c + 0x134) */
struct s_first_person_weapon_globals
{
	byte unknown00[0x94];
	real_bounds idle_delay;
	real idle_chance;
};

struct s_166d75_globals_view
{
	byte unknown000[0x134];
	s_first_person_weapon_globals *field_b4;
};

struct s_index_pair;
struct s_index_triple;
struct s_blend_orientation;
bool function_0b6760(const s_index_pair *pair);
bool function_0b6780(const s_index_triple *triple);
real magnitude3d(vector3f const *v);
real function_0bff60(real a, real b);
bool function_13cb40(void);
real function_187420(long player_index);
short function_187a40(long player_index);
bool function_11eed0(real *position, real *velocity, real target, real time_step, real speed, real acceleration, bool wrap, real lower, real upper);
bool function_c8a10(long unit_index);
bool function_d03b0(long unit_index);
void __stdcall function_16760c(long user_index, long field_x11c898);
real function_259d0(dword *seed, char const *file, long line, real lower_bound, real upper_bound);
bool function_1d9320(s_1d9240 *p);

point2f *g_4687c4;

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the per frame update of a local user's first person weapons: the view's
   sway springs, the weapons' animations, zoom overlays and idles */
static __forceinline real function_x82e52f(dword *seed, char const *file, long line);

// score 172 (was 188): the 0x2c frame and the ESI/EDI saves after the unit test are retail's since m3 lane 8 r232 (its
// lines are just above the marker); the unit and mode homes, the weapon-active and zoom homes, the active preset and the
// movzx flag test remain. Before that: frame 0x30 versus 0x2c (the look values have retail's homes and divisions since
// 11-18), the same homes, and the ESI/EDI save placement.
// Earlier first line (220, was 220): frame 0x28 versus 0x2c; unit/mode homes and independent look divisions/homes remain.
// frame 0x30 versus 0x2c, exchanged unit and
// mode homes and earlier ESI/EDI saves. Both look divisions and output homes agree.
// Earlier 220: frame 0x28 versus 0x2c; unit/mode homes and independent look divisions/homes remain.
// Earlier noted 218 (was 218). The full check gave 220 in m3 lane 16 r130.
// MiniMax C (11-17): score 220 (was 220): no new form, no lower score. The look clamps still share one 1/rate
// reciprocal where retail divides x / rate and -1.0f / rate and reloads g_510c54 after the yaw store, and retail's
// look homes are -0x2c/-0x28 in a 0x2c frame against ours' -0xc/-0x8 in a 0x28 frame; the s_view_angles aggregate
// (216), the rate hoisted into a local (541) and the cast-spelled divisor (220) were measured and dropped.
// MiniMax I (11-16): score 220 (was 220): no new form, no lower score; the frame stays 0x28 against
// retail's 0x2c and the ESI/EDI save region is still blocked by the reduced image's stand-in caller.
// Earlier 220 (was 220), merged from a second score line: frame 0x28 versus 0x2c; unit/mode homes and independent look divisions/homes remain.
// Earlier at 220 (was 220). Frame 0x28 versus 0x2c. Unit/mode homes and independent look divisions/homes remain.
// MiniMax A (11-05): no new form, no lower score; our frame stays 0x28 against retail's 0x2c, so the look
// outputs stay at -0xc/-8 instead of retail's -0x2c/-0x28.
// b6D statement walk against retail (11-06, fast build, control 220 before and after; every address an instruction
// start): all four spring defaults and overrides (0x166dbb-0x166e87), the five-term idle-motion test, the idle
// counters (0x1674b3-0x167597), the overlay logic (0x1671d8-0x167307), the fmod frame (0x167353-0x1673ab), the tail
// (0x1675df-0x167602) and every callee argument agree. One BEHAVIOUR FIX kept: the idle-blend spring call passed
// (speed 9, acceleration 6); retail loads xmm5 = 6.0 (0x1673b9, the fifth argument as in the four calls above it) and
// stores 9.0 at [esp + 8] (0x1673df, 0x1673e7), so it is (6.0, 9.0) now (220 -> 220: the pool constants were swapped
// with each other, the positional score cannot see it). Not behaviour, not kept: the yaw is x / rate in retail
// (0x166f2b divss) but ours multiplies by the shared reciprocal (an fp rounding difference that follows from the
// missing rate reload, see rounds 8/68/82); the pitch (-1 / rate) * y is bit-identical either way. `(real)fmod(..)
// * 30.0f` compiles the same with and without the cast (220 both), left as is. Retail's zoomed/scoped bytes at
// [ebp - 2]/[ebp - 1] hold the positive sense of ours (no_zoom/no_scope) and the else-arm `cmp [ebp - 2], 0; je`
// (0x1672cb) is implied by the outer test: the same behaviour, positive form 385 (older rounds), so not kept.
// `volatile` look locals 475 (not allowed without a match). No new include, helper or double local.
// Earlier at 218 (was 218; round 30's set below reached 122 but is not kept, see the review line): frame
// Round 25 (lane I, fast): the two locals whose homes are swapped (`unit` and `mode`, ours
// -0x18/-0x1c against retail's -0x1c/-0x18) declared in the other order, with the two
// initialising calls kept in their original order, compile to byte-identical code (197 at
// 2197 bytes): MSVC does not order the two homes by the declaration order.
// Round 31 (lane I, fast): the pitch's rate read through a copy of the settings pointer
// (`s_game_time_globals *frame_settings = g_510c54;` before the clamp) stops the shared reciprocal,
// so MSVC keeps retail's two separate divisions, the second reloading `[0x510c54]->rate`; the two
// clamped look outputs as one s_view_angles local give retail's grouping of them. Fast score
// 218 -> 200; 2193 -> 2197 bytes (retail 2199). The frame (0x28 versus 0x2c), the look-output homes
// (ours -0xc/-8, retail -0x2c/-0x28), the swapped unit/mode slots, the ESI/EDI save region and the
// clamp's register roles still differ. A rate read through a local (a value or a pointer copy)
// either keeps the shared reciprocal or lands in a stack slot instead of retail's `[eax + 4]`.
// MiniMax I (10-31) review: round 31's 200 came from the `frame_settings` pointer copy, a one-use
// local (kept only with a match); without it the s_view_angles local alone 216 against 220, under the
// 8-point bar, so the body stays as it was.
// Earlier score 218 (was 218; round 30's set below reached 122 but is not kept, see the review line): frame
// 0x28 versus 0x2c (look homes), the shared look reciprocal, ESI/EDI saved before the unit test (retail
// after it, restored at the loop exit) and the channelb8 pointer in ECX remain.
// Earlier 241 from 241: frame 0x30 vs 0x2c; esi/edi saved before the unit test (retail after it,
// frame 0x28 vs 0x2c; esi/edi saved before the unit test (retail after it,
// frame 0x28 vs 0x2c; esi/edi saved before the unit test (retail after it,
// restoring them at the loop exit), the two shared spill slots 0x18/0x1c swapped, and the look rates'
// shared reciprocal (retail divides twice and reloads the time global) differ.
// Round 139: the look rates as one s_view_angles local give retail's 0x2c/0x28 homes 237 (a local-
// grouping struct, kept only on a match; a point2f gives the same code; a bare block changes nothing). Fast previews (kept body
// 239 fast): rates written through an inline helper's pointer 239; through a pointer local 493; mode
// declared before the unit 239; early return for a missing unit 239; springs as an array/two pairs
// 256/258 (frame 0x38/0x30, before the kept change); mode renamed (spill slots are not name-ordered)
// 243 (before). Active preset false with the inner `!155760 && b6780` assignment, with or without
// the right-first callback, 381 (2195 bytes): its
// sequence-aligned difference count is 63 against 68 for the kept body, so it is the closer shape
// once the remaining length differences (look reciprocal, movzx flag test, channel rate store through
// the weapon, overlay flag pushes) are fixed together.
// Earlier 248 (was 248): frame 0x28 vs 0x2c, unit/mode and look-output homes differ;
// frame 0x28 vs 0x2c, unit/mode and look-output homes differ;
// word/byte weapon index 388/373; scoped rate first/shared store 438/258; unscoped selection first 253.
// Earlier 250 (was 250): frame 0x28 vs 0x2c and unit/mode/look homes remain.
// frame 0x28 vs 0x2c, unit/mode homes, the look reciprocal (retail divides twice,
// reloading the time globals) and esi/edi saved before the unit test (retail after it) differ.
// frame 0x28 vs 0x2c, unit/mode and look-output homes differ; the look
// reciprocal (retail divides twice, reloading the time globals) and esi/edi saved before the unit
// test (retail after it) differ.
// Round 55 kept: the speed (5th argument) and acceleration limits passed in retail's roles (15/6 speed,
// 72/27 acceleration), defaults in retail's store order, each look angle clamped in one statement.
// Not kept (each lowers the aligned differences but shifts every later instruction): active preset
// false then the `!155760 && b6780` assignment 385; first-weapon callback test `== 0 ? right : left` 385;
// clear test without the redundant `zoomed &&` 396; channel98 rate/overlay read through the weapon 387;
// overlay mismatch as a ternary 383 (setcc); look angles in an s_view_angles 239 fast (frame 0x30);
// mode looked up before the unit 574; springs renamed speed/acceleration 243 (slots unchanged).
// Word/byte weapon index 388/373; scoped rate first/shared store 438/258; unscoped selection first 253.
// Entry unit capture 252; const unit/primary weapon/weapon object/look receivers 252 each;
// look interval capture 417; combined readers 252; readers plus interval 417 (fast base 252).
// Single spring overrides 548/542/536/542; first/second pairs 559/550; all four 553;
// all four plus yaw/pitch delta inputs 559 (fast; unchanged preview 252).
// Velocity/float-buffer/indices/animation receivers 572/605/252/407; input pair 608; weapon pair 407;
// all receivers/direct velocity 617/617 (fast preview 252).
// External user/unit/weapon records and user-unit pair 252 each; native dword index/mode, callback at
// update, and unit-weapon-index combination 252 each (fast base 252).
// Indexed user-record reads at every access 550; with live flags 544; direct channels 560;
// both 538; complete coefficient selection with those four forms 550/544/560/538.
// Complete entry spring phase, override-only receiver, entry look outputs and both 250 each;
// narrower aiming pair within that phase, with the same four storage combinations, 254 each
// (fast control 250, full baseline 248). Original phase scopes and all five smoothing calls restored.
// Explicit empty-unit return with the user receiver before/after admission and scoped
// bool/byte/short/long storage 243/243/434/429 and 555/555/558/549 (fast control 243).
// Original entry guard, receiver and state types restored.
// Round 187 paired normalized expression parts 243; yaw consumed before pitch production
// alone/with named parts 496/496; fresh view fields alone/with parts 442/442;
// interleaved fresh fields with/without parts 510/510; all plus signed flags 510
// (fast control 243, full baseline 241). All original expressions and calls restored.
// narrower aiming pair within that phase, with the same four storage combinations, 254 each.
// Original phase scopes and all five smoothing calls restored.
// Round 93 (fast base 243): look angles as a two-element local 239 (retail's -0x2c/-0x28 homes,
// frame 0x30, reciprocal still shared; a grouping, not kept); mode renamed past unit 243 (slots
// unchanged). Combined: active preset, channel98 rate/set read through the weapon, the overlay call
// and save in each arm (scoped / else if zoomed), the clear test without `zoomed &&`, callback
// `== 0 ? right : left` and the set read once into a named local: 378 positional but 49 aligned
// differences (82 before; 577 of retail's 578 instructions). The one instruction short is retail's
// `movzx eax, byte [edi+0x19]; test al, 0xa` after the update call, where ours folds `test byte`
// (`!= 0`, two bit tests, a bool local, bitfield/word/shift views and a bool cast all fold).
// Apply that set once the movzx form is found.
// Round 257 (fast base 243): Active int/byte/signed-byte carriers 418/417/417;
// each with complete idle-settings observations at uses 414/413/413; original bool with
// those observations 270; int active with read-only idle settings 418. Original body restored.
// Lane 5 r77 (fast base 243): the movzx is the only `movzx; test al, imm` in retail; casts, `> 0`,
// `!!`, shifted bit pairs, TEST_FLAG pair, byte/word/dword bitfield and dword-shift views, ternary
// and `&` bool forms, and the test in another file's non-inline function (LTCG-inlined) all fold to
// `test byte`. Look angles through a pointer clamp helper 246 (inlined away; slots and shared
// reciprocal kept); if/else clamps 246; mode declared before unit 243; look angles declared before
// the springs 243; the round 93 combined set re-measured 378 (aligned 49, one instruction short).
// Round 231: Aiming/look/paired field references 574/248/572; animation reference 400; aiming-
// animation/look-animation/all receiver references 550/405/548; all plus user-flag field 421.
// Original body restored.
// Round 262 (fast control 243, full 241): complementary scoped bool/byte/short/long
// 242/242/447/457; both zoom/scoped complements 234/234/440/450. Dual bool gives 232 plain;
// direct short-circuit scope complement also 234 fast / 232 plain, kept. Frame 0x28,
// unit/mode homes and shared look reciprocal remain; every smoothing input is unchanged.
// Round 360 (fast base 243): native double aiming-limit/look-limit/look-rate
// pairs, combinations and all three 574/543/259/584/574/531/584; native 64-bit
// user flags 388. Original scalar widths and every smoothing input restored.
// Round 25 (fast control 234, full 232): early return 234; view-named look outputs 234;
// active-result preset 425; look/flags declarations at assignment 234; const first-assignment
// look outputs 234; per-weapon scalar/channel declarations at assignment 234;
// outer active/zoom declarations 234/234. Original body restored.
// Round 225 (fast control 234, full 232): direct negative overlay result 414;
// complete scope-first rate selection 416; native countdown store/test 234;
// overlay/rate, overlay/countdown, rate/countdown and all three 422/414/416/422;
// all plus primary-pointer admission 587. Original complementary states and all calls restored.
// Round 370 (fast control 234): scope-factored clear decision with active-default, callback
// and member-rate interactions 434/410/434/410/401/397/401/397; original body restored.
// Round 165: Native overlay argument-pair flags/set/outer-lifetime subsets
// 386/234/386/234/386/234/386; combined role names 386 (fast control 234, full baseline 232).
// Original pair types, scopes, names and every smoothing input restored. Frame 0x28 versus 0x2c,
// unit/mode homes and shared look reciprocal remain.
// Round 212: plain two-axis aiming-spring traversal, long/short/dword/word indices,
// direct/output-storage receiver [578, 578, 578, 578, 578, 578, 578, 578]. Original independent
// calls restored; frame and look homes remain.
// Round 373 (fast control 234): Structured overlay continuation, with callback/active/both
// choices 234/234/392/392; reusable no-overlay decision with those four forms
// 421/421/403/403. Original body and every smoothing input restored.
// Round 246: complementary activity bool/byte/short/long 424/424/423/425; the same with
// callback selection at update 424/424/423/425 (fast). Original activity and callback restored.
// Round 251 (fast control 234): loop-scoped orientation byte-offset/ordinal/base/cursor
// 410/234/409/254; the four at entry 561/234/576/558. Ordinal forms keep frame
// 0x28; the other forms grow it to 0x30. Original buffer lifetime and body restored.
// Round 255 (fast control 234): native actual aiming/look/view state fields, their
// three pairs/all, and complete native point pairs 234 each. Original byte-buffer
// declaration and every original expression restored; frame remains 0x28.
// Round 174 (fast control 234, full 232): complete mutable/readonly clock roots
// 509/509; mutable/const native interval 543/543. Final plain activity scan,
// long/short condition and long/word body termination 233/230/233/230; four-count
// reduced gains fail the eight-count gate. All snapshots/scans restored; frame,
// unit/mode homes and shared look reciprocal remain.
// Round 34 (fast control 234, full 232): independent primary/idle pointer or reference
// receivers, all four combinations 234 each; those four with first-weapon fields
// observed directly 578 each. Original shared channel lifetime and body restored.
// Round 182: actual aiming-input vector homes before/after other locals 521/521;
// native real/double input scalars 485/510; confined aiming phases repeat
// 521/521/485/510 (fast control 234). Original inputs, calls and body restored.
// Round 389 (fast control 234, full 232): late magnitude double 428; reused native idle tick
// 290; both 427; both named animation-period summands 234. Native double period/duration
// alone, with magnitude, with idle tick and with both 390/415/397/414 (exact-only).
// Original scalar types, arithmetic and calls restored; frame 0x28 and look homes remain.
// DL167: Plain look-history difference traversal long/short/dword/word 599/600/599/600;
// complete per-axis difference/default/history publication 501 each. Existing mode-helper inline
// and inline with separate noinline declaration 234 each (fast control 234). All restored.
// Round 249: native user active-bit stores with original/dispatch/final/both weapon-bit
// access 234 each; complete native user observations with those four forms 470 each.
// Original records, flag snapshot, stores and every smoothing input restored.
// Round 268: uniform direct zoom/idle/both channel receivers 220/234/220;
// native input root alone/with zoom/with idle/with both 441/444/441/444;
// all with flags declared at assignment 444 (fast control 234, full baseline 232).
// Only direct zoom-channel access kept: plain 218, 2193/2199 bytes, differing rows 217 -> 212.
// Frame 0x28 versus 0x2c, unit/mode homes and shared look reciprocal remain.
// DL179: Coordinated scalar-spring/angular-difference ordinary, inline and noinline
// declaration pairs (excluding both ordinary) 218/218/499/499/499/218/218/218 fast;
// original complete body and every helper declaration restored. Frame/look homes remain.
// Round 426 (fast control 220): shared bool/long override with logical publications
// 404/422; bitwise publications 422/413; those four with direct overlay-result assignment
// 431/435/434/422. Original override branch, lookup and complete executable body restored.
// Round 430: Actual weapon-array cursor, body pointer step, long/short/dword/word ordinals: index-
// bound 415/309/415/309; array-end bound 440/474/440/474. Original index-based body restored.
// Round 212: const overlay return pair with two role names 220/220; pair reservation at
// weapon/iteration/user scope, both names, 388 each (fast control 220). Original lookup lifetime and body
// restored; frame and look homes remain.
// Round 317: Existing shared scalar clamp for both look outputs, with zoom clamp,
// inline declaration and confined spring/look-phase subsets, 220 each (fast control 220,
// full 218). Original macro expressions, declarations and complete body restored.
// Round 30 (lane 11): retail's movzx form found with a compile-only harness of this file: the two flag
// bits tested on a `long` copy of the channel byte under the update test (every byte, word, dword,
// shift, bitfield, bool, volatile and helper spelling of the direct test folds to `test byte`; a
// two-definition dword local also gives movzx but keeps an unthreaded join). Kept with the round 93
// set (aligned counts, fast): movzx 116 -> 114, active preset 110, callback `== 0 ? right : left` 109,
// positive zoomed/scoped in retail's order with the clear test lacking `zoomed &&` and the overlay
// play per arm 48, the set read once 47 (plain 218 -> 122, 2196 bytes). Not kept (fast, control 125 /
// aligned 47): set test as a ternary with field reads 323/53; if/else look clamps 128/50 (reciprocal
// still shared); early return for a missing unit 125 (same code); weapon object looked up after the
// orientations and callback 134/76; look rates as one point2f 121/33 (retail's -0x2c/-0x28 homes,
// frame 0x30, reciprocal still shared; a grouping). Harness only: no clamp or rate spelling (pointer or
// reference clamp helper, array, statement order, named rates, explicit reciprocal) stops the shared
// reciprocal; ESI/EDI are shrink-wrapped as in retail without LTCG but not with it, also with the
// post-loop code removed. Lead: 0xb6760 (src/unknown_0b66c0.cpp, not ours to change) may be a callee
// retail's callers treat as clobbering ECX (channelb8 kept in ESI).
// Review of round 30 (m3): not kept: the 48/122 set needs `else if (zoomed)` after `if (scoped)`, a test
// that is always true inside `if ((zoomed || scoped) && ...)`; with a plain `else` it is 306 (fast). Code
// back to the original; the movzx, active-preset and callback findings above still stand alone.
// Round 98 (lane 8): BLOCKED-BY the build's stand-in caller (tools/build.py): an LTCG env link of the
// generated translation unit equals the full listing only while the entry calls the stand-in that
// calls this function; with only 0x165dc1's stand-in (retail's one caller) the same source saves
// ESI/EDI after the unit test and restores them at the loop exit, as retail (no other object
// matters: all 1772 others linked still shrink-wrap). Env rows with the stand-in, base 157/116: each
// look angle divided then clamped in its own statement 160/119; the mode lookup after the spring
// defaults 173/129; `first_weapon = user->weapons` 157/116 (identical); spring defaults declared
// d, c, b, a 161/127; user flags read and set directly without the flags local 157/116 (identical).
// Round 24 (lane 14, fast; aligned counts by sequence alignment, base 116): the movzx flag test (a named
// `long` copy of the channel byte, two bit tests), the active preset and the `== 0 ? right : left` callback
// together 358 (2199 bytes, aligned 109); with the overlay set read once (negative flags) 372; those three
// with positive zoomed/scoped bools in retail's order, the clear test without the extra `zoomed &&` and a
// plain `else` for the play arms 328 (aligned 70); that with the play and save in each arm and the set read
// once 306 (2190 bytes, aligned 53: two instructions short, retail's `cmp zoomed; je` in the else arm); the
// positive bools alone on the original body 401. Unit/mode slots (ours -0x18/-0x1c), the look homes and
// shared reciprocal and the ESI/EDI save region are unchanged by all of them.
// Round 8 (lane 18, fast, aligned counts in brackets, fast control 220 [79], full 218): a `#pragma optimize("p", on)`
// diagnostic (reverted) gives retail's two look divisions with the rate reloaded between them, but
// copies the look-history pair through x87 and grows the body to 2212 bytes, 343 [77]; look rates as
// one s_view_angles 216 [72] (frame 0x30); that declared first 216 [72]; with unit and mode declared
// before their assignments 216 [72]; with the pitch as `x * (-1.0f / rate)` 219 [79]; the two look
// locals' addresses taken as a no-op (diagnostic) 220 [79]; the loop index declared in the for 220 [79].
// N round 257: Actual tag coefficients as array/two spring records, at-use if/conditional defaults and complete two-use observations 220/220/551/551/220/220/551/551.
// All original record fields, coefficient producers, smoothing tuples and body restored; fast control 220, full 218.
// J218: Bounded signed-byte/short/dword/word weapon ordinals, each alone and with both rate-expression nodes on the direct zoom graph: 399/399/421/421/218/218/421/421. Original complete body restored.
// Round 238: existing orientation-buffer/callback/idle-channel role-name subsets 1..7,
// then all with uninitialized idle-channel storage owned by the iteration, eight 220 fast
// (control 220, full 218). Every original declaration, name and complete body restored.
// Round 45 (lane 13, fast control 220): retail gives the look values their own slots (-0x2c/-0x28) and
// reloads g_510c54 between them; ours reuses the spring_a/b slots and shares 1/rate. mode renamed
// weapon_mode 220 (slots unchanged); look values declared at assignment 220; in their own block with
// their two spring calls 220; computed then clamped in a second statement 223; spring defaults declared
// before the unit/mode lookups 236. Original body restored.
// Round 40 (m3 lane 15, fast control 220, full 218): ours stores the two clamped look values into spring_a/spring_b's
// dead slots (-0xc/-0x8) where retail gives them -0x2c/-0x28, and retail reloads [0x510c54] after the yaw store (two
// divisions). Look values renamed, two name pairs, 220/220, spring_a/b renamed 220 (same code); pitch as
// `-(y / rate)` 459 (2196 bytes, reciprocal still shared); pitch clamped before yaw 221.
// Round 23 (lane 18, fast control 220): the d03b0 overrides as one chained assignment 220 (same code);
// the zoom flags preset true and cleared under `if (zoom > 0.1f)` 224; the look history read through a
// point view 220; the zoom flag as `zoom <= 0.1f` 417 (2190 bytes); the four spring overrides as
// conditional expressions 220 (same code). Diagnostic: both look locals' addresses taken as a no-op 220
// (no effect: retail's separate look homes and reloaded rate are not an address-taken local).
// Slot map (ours/retail): unit+flags -0x18/-0x1c, mode -0x1c/-0x18, zoom shares spring d (-0x14) in ours
// and unit's home in retail, the active byte shares unit's home in ours and spring d's in retail.
// Round 443: complete overlay-clear/start/idle decision results, seven bool
// subsets and the complete long set, 220 each (fast control 220, full 218).
// Original decision expressions, clock reads and complete body restored.
// Round 284 (fast control 220, full 218): separate tail expiration decrement/idle
// increment/both 220 each; complete two-weapon active mask alone/with decrement/
// with increment/with both 223 each; both plus final resource guard 223. Original
// complete body restored; frame, unit/mode and independent look homes remain.
// Round 445: Native double raw-input/interval domains, with late-magnitude combinations and
// independent quotient intervals: 510/547/532/518/536/540/550/505.
// Original complete executable body restored.
// Round 294: invariant aiming/look/both spring defaults before admission or user
// receiver 233/235/239 each; all before unit lookup, abcd/badc order, 236/236
// (fast control 220, full 218). Original default lifetimes restored; frame 0x28/0x2c remains.
// Round 341: Two-bit channel-byte ownership/activity preset/zero-first callback,
// subsets 1-7, 412/383/358/220/412/383/358; all with late idle receiver 358
// (fast control 220, full 218). Exact-length forms still miss the frame word;
// original complete body and every smoothing tuple restored.
// Round 449: Consumed-result overlay-index/activity-at-setter/frame-expression subsets 1..7
// 229/406/406/371/373/289/289; all plus direct scope-test result 301 (fast control 220, full 218).
// Original complete executable source restored; opening structural differences remain.
// Round 293: Complete default/history memcpy or memmove publications, four pairs with
// original/confined history scopes: 220/417/404/438/220/417/404/438 (fast control 220, full 218).
// Original body restored; frame, look and unit/mode homes remain.
// Round 463: plain two-axis look-spring traversal with actual paired clamp inputs,
// long/short/dword/word counters, direct/shared actual output-state access:
// 642/643/642/643/639/639/639/639. Grouped inputs require a match; original independent calls restored.
// Round 120 (fast control 220, full 218): const mode/const unit/const look pointer/const user pointer
// 220/220/220/220 (same code); spring defaults declared between the unit and mode lookups 236. Diagnostic
// (reverted at once): the yaw output's address escaping to the second aiming call keeps the shared look
// reciprocal and the -0xc home (228), so neither an escaped nor an address-taken look local stops it.
// Still BLOCKED-BY the build's stand-in caller for the ESI/EDI save placement (round 98).
// DL237: Existing unit getter at every genuine use, crossed with complete weapon-object,
// idle-channel and user-flag observations: 611/602/611/602/611/602/611/602 (fast control 220,
// full 218). All restored; frame, unit/mode/look homes and shared reciprocal remain.
// Round 465: repeated aiming/view/delta scalar destination pointers, seven subsets,
// then all addresses at admission: 222/457/459/225/459/457/236/239
// (fast control 220, full 218). Original addresses and complete body restored;
// frame, unit/mode homes and independent look divisions remain.
// Round 521: two-use native look-coefficient handoff, early/phase capture, optional
// aiming-pair handoff and immutable receivers: eight at 220 (fast control 220, full 218).
// All original carriers restored; frame, unit/mode and independent look homes remain.
// DH285: Native late-magnitude input assignment/memcpy/memmove/components at consumption,
// then with uninitialized iteration-owned storage: 420/420/426/529/420/420/426/529 fast. Exact-only captures
// restored; frame, unit/mode homes and independent look divisions remain.
// Round 476: native pooled-user reference through the existing getter, with
// unit/iteration-weapon/weapon-object reference subsets: eight at 220 fast
// (control 220, full 218). Original object roots and complete body restored.
// Round 478: one native weapon-array pointer with an ordinal derived from its
// real array position; long/short/dword/word ordinals and !=/< end tests give
// 455/475/455/475 in both sets (fast control 220, full 218). Original index
// traversal and complete body restored; frame, unit/mode and look homes remain.
// AC485: previous-angle input assignment/scalars/memcpy/memmove at branch/user scope:
// 453/220/453/469/453/220/453/506 fast (control 220, full 218).
// NOT kept: one-use input captures without a match; original body and smoothing inputs restored.
// AC489: Inactive history, mismatching idle class and idle-motion rejection first, seven
// phase-ownership subsets: 236/254/220/270/236/254/270; all with nested countdown admission 270
// (fast control 220, full 218). Original body restored; frame, unit/mode and look homes remain.
// m3 lane 11 r60 (fast control 220, full 218): the tail's flag tests under a second `unit_index != NONE`
// block 213 (diagnostic, a repeated test; the saves still precede the unit test); the loop index declared
// just above the loop 220; look values in a block around their two smoothing calls 220; look values declared
// before the unit 220; active preset false then `!155760 && b6780` in an if 383 (2197 bytes, aligned 116 -> 112).
// Retail spills `active` to [ebp-0x14] (false store before the unit test) and keeps the look values in their own
// -0x2c/-0x28 slots; ours reuses the spring slots.
// AC495: Post-loop user-activation result from both existing weapon-bit tests, bool/byte/short/long
// at evaluation or with admission-owned uninitialized storage: eight at 220 fast (control 220, full 218).
// Original compound test, declarations and complete body restored; frame, unit/mode and look homes remain.
// AC497: Native duration/remainder call results, real/double at consumption,
// eight exact-only forms 220/389/220/220/383/379/383/379 fast (control 220, full 218).
// Original real argument domain and narrowing preserved; all one-use captures restored.
// Frame, unit/mode homes and independent look divisions remain.
// Round 205 (m3 lane 3, fast control 220, full 218), re-measured listed forms: the settings rate read once into a
// named local for all six uses 526 (2178 B); the mode lookup after the spring defaults 236; the unit fetched after
// the mode 573 (2189 B); a short weapon index 421 (2220 B); the look values declared at their computation 220 (same
// code). New forms: the active result passed straight to first_person_weapon_set_active (no local) 406 (2189 B);
// the orientation offset as user and weapon terms scaled separately 220 (same code); the view entry's yaw/pitch
// read at each use with no look pointer 444 (2205 B); the spring overrides tested as `if (field)` 220 (same code);
// the pitch rate product with its operands swapped 221. The ESI/EDI save region stays BLOCKED-BY the build's
// stand-in caller (the round 98 finding, not re-verified here).
// Round 83 (m3 lane 12, fast control 220, full 218): look_yaw/look_pitch declared after mode, at function
// scope, or at their assignments 220 each; mode declared before the unit and assigned after it 220; the
// pitch as `y * (-1.0f / rate)` 221. Retail divides `x / rate` and `-1.0f / rate` separately and reloads
// g_510c54 after each clamped look store (-0x2c/-0x28), as if those stores could alias the global; ours
// shares one reciprocal.
// AC497: Native duration/remainder call results, real/double at consumption,
// eight exact-only forms 220/389/220/220/383/379/383/379 fast (control 220, full 218).
// Original real argument domain and narrowing preserved; all one-use captures restored.
// Frame, unit/mode homes and independent look divisions remain.
// Round 68 (m3 lane 10, fast control 220, full 218): ours rewrites both look divisions as products with one
// shared 1/rate computed before the first clamp; retail divides x / rate, reloads g_510c54 after the first
// clamp's stores, divides -1 / rate and reloads again after the second. Both look clamps through the existing
// cross-file real_pin (0x2a0c90's file) 220; the two rates through a same-file inline getter by value, by const
// reference, forceinline by const reference 220/220/220; through the existing cross-file forceinline time-step
// getter (0x1d2280's file) 220 (all same code: small inline helpers are fully CSE'd). Restored.
// Round 82 (m3 lane 16, fast control 220, full 218): the look values as one point2f local 216 (frame 0x30 against
// 0x2c: the look values then take their own -0x2c slot as retail, but 1/rate is still shared, so the pitch division is
// not retail's; a grouping struct, not kept); mode renamed weapon_mode 220 (slots do not move); the clamps through a
// file-local pin taking `real const &` 220 (same code), through an in-place pin on `&look_yaw`/`&look_pitch` 223 (still one
// shared reciprocal); the yaw as `1.0f / rate * x` 220; early return for a missing unit 220 (repeat, same code).
// AC497: Native duration/remainder call results, real/double at consumption,
// eight exact-only forms 220/389/220/220/383/379/383/379 fast (control 220, full 218).
// Original real argument domain and narrowing preserved; all one-use captures restored.
// Frame, unit/mode homes and independent look divisions remain.
// AC515: Complete animation-update/event-bit result bool/byte/short/long at computation or
// with iteration-owned uninitialized storage: 220/220/220/220/220/220/220/220 fast (control 220, full 218).
// NOT kept: one-use result diagnostics unmatched; original body and all five smoothing tuples restored.
// AC518: Complete countdown/pause expiry bool/byte/short/long at producer or
// in iteration-owned storage: 220/220/220/220/220/220/220/220 fast (control 220, full 218).
// NOT kept: one-use result unmatched; frame, unit/mode and look homes remain.
// AC523: Signed-short countdown/idle-increment/pause-decrement store-and-test
// results, subsets 1..7 then complete iteration-owned declarations:
// 220/277/277/225/225/276/276/276 fast (control 220, full 218). NOT kept:
// no match; original counters, signed wrap, callbacks and complete body restored.
// P312: Fixed two-weapon !=2 and <=1 exhaustion, long/short/dword/word ordinals:
// 220/421/220/421/221/421/221/421 fast (control 220, full 218). NOT kept: no match; original loop/body restored.
// Frame 0x28/0x2c, unit/mode homes and independent look divisions/homes remain.
// AC525: Post-setter per-weapon presence/absence results in bool/byte/short/long,
// eight at 220 fast (control 220, full 218). NOT kept: predicate residence unmatched;
// original post-call read and full body restored; frame, unit/mode and look homes remain.
// AC527: Shared idle-animation selection as long/dword/int/unsigned-int, at consumption
// or iteration scope: eight at 220 fast (control 220, full 218). NOT kept: selected-result
// residence unmatched; original draws, counters, setter paths and complete body restored.
// Lane 9 r87 (fast; aligned counts by sequence alignment, base 218/104): the overlay play written per arm with its constants 376/95
// (2191 bytes; `mode` then outweighs `unit` and both take retail's homes), with the active preset 377/83 (2195), plus callback and
// movzx 377/83; positive zoomed/scoped alone, plain else 385/61 (2198), with the rest 376/49 (2202 bytes). Retail's own bytes show the
// `else if (zoomed)` test under `(zoomed || scoped)` (`cmp byte [ebp - 2], 0; je` past the play), so it is real, but it stays out
// without a match. Look locals' addresses taken 220/104 (reciprocal still shared); look value and clamp in separate statements 223;
// aiming velocity through a pointer 542/114; `mode` declared first, assigned later 220. Positional stays above 218 until the look
// block's bytes agree, because every form shifts what follows it.
// AC535: Peer processing/final activation-resource phases gated by one entry result,
// positive/complementary bool/byte/short/long: 542/542/573/571/542/542/573/571 fast
// (control 220, full 218). NOT kept: frame 0x28 and native unit/mode/look/save gaps
// remain; original complete body and every observation/call restored.
// AC537: Actual post-spring blend-completion/pending truth, bool/byte/short/long,
// 220/220/220/220/220/220/220/220 (fast control 220, full 218). NOT kept: no equality/gain; original
// completion test, all five smoothing tuples and complete body restored.
// AC539: Independent zoom/idle channel-query return ownership, positive then
// complementary bool/byte/short/long: eight at 220 fast (control 220, full 218).
// NOT kept: one-use call results unmatched; original queries and full body restored.
// AC541: All four early coefficient override/default-presence outcomes, bool/byte/short/long,
// eight at 220 fast (control 220, full 218). NOT kept: no equality or gain;
// original field tests, overrides, smoothing tuples and complete body restored.
// AC543: Independent final resource-presence/complement results at the original
// tail consumer, bool/byte/short/long: eight at 220 fast (control 220, full 218).
// NOT kept: one-use outcomes unmatched; frame, unit/mode/look homes and late restores
// remain. Original final gate, smoothing tuples, observations and complete body restored.
// Round 64 (m3 lane 14, fast control 220, full 218; masked aligned 112, 578 instructions on both sides): the channel byte's `long` copy
// under the update test 412 (2195, aligned 110; shift only); callback `== 0 ? right : left` 220 (aligned 111); both 412 (109); the channel name read once
// 399 (2189, 112); the rate as `if (scoped) 1 else if (zoomed) pin` 407 (2187, 117), with the name read once 409 (2183, 117); the overlay play written per arm 376
// (2191, aligned 103). Retail's logic decoded: zoomed = speed > 0.1; scoped = zoomed && query; both cleared on the second query; scoped cleared
// when the overlay lookup fails; the play's `else if (zoomed)` under `if (zoomed || scoped)` is retail's own implied test (`cmp [ebp-2], 0; je`).
// Still BLOCKED by the stand-in (round 98) for ESI/EDI; no byte match is reachable without it, so no match-only form can stay. Body unchanged.
// Round 76 (m3 lane 1, fast control 220, full 218; aligned 116): look outputs declared first in the unit block 220; declared at
// their PIN assignments 220; the aiming velocity as a named pointer for its three uses 233 (2190 B, aligned 118; the pointer takes
// unit's home, unit/mode homes unchanged); a const unit pointer 220; a const look pointer 220. Retail's movzx flag test is not
// followed by any use of EAX (the next instruction reloads it), so no two-use local is behind it. Body unchanged.
// Round 132 (m3 lane 7, fast, control 220): the look values declared where they are computed 220, as
// `real const` there 220, declared first in the block 220 (all same code: their homes stay -0xc/-8);
// the look pointer, flags and weapon counter declared at first use 220; the pitch as `y * (-1 / rate)`
// 221; divide then clamp in separate statements 223 (a repeat of lane 9 r87's form). Retail's look
// slots -0x2c/-0x28 are never address-taken (no lea of them), yet retail reloads g_510c54 after each
// look store; still BLOCKED by the stand-in (round 98) for the ESI/EDI save after the unit test.
// AC552: Existing object/definition getter forced expansion and outlined magnitude inline,
// subsets 1/2/4/3/5/6/7, then outlined tick-provider inline: eight at 220 fast
// (control 220, full 218). NOT kept: no gain; original helper declarations and complete body restored.
// AC556: Consistent triple-validity char/signed-char/byte return contracts with
// bool/byte/short activity carriers: 218/218/218/415/415/415/414/414
// (unchanged complete-file fast control 218, full 218). NOT kept: query provider loses
// its match; original return declarations, activity domain and entire body restored.
// AC558: Complete primary-weapon override admission/rejection result in
// bool/byte/short/long: eight at 220 (unchanged fast 220, full 218). NOT kept:
// one-consumer truth unmatched; original early gate and complete body restored.
// M398: Original clamped-yaw/clamped-pitch/late-magnitude zero defaults, masks
// 1/2/3/4/5/6/7, then complete activity/zoom/scope defaults: eight at 220 fast (control 220, full 218).
// NOT kept: no frame/look-home gain; original producers, all five smoothing tuples and body restored.
// M3 lane 7 r167 (plain where noted; the reduced image misbuilds caller 0x16760c, 142 with no change, MATCH in plain):
// `inline` on 0xb6760's definition (now ours; round 30's lead) 217 plain, channelb8 then kept in ESI as retail, but
// 0x106790 250->254 and 0x1cd070 210->269, not kept; diagnostic (a file-local copy of 0xb6760's body for this
// function's three calls, not keepable) 215 fast: retail's callers disagree about 0xb6760's register summary, as with
// 0x24590 at 0xe3380. Dead-local phase scan 1-7 at the top 220 each; `long const mode` 220; the overlay channel read
// in place (no `channel` local) 220.
// P569: Idle draw real/const-real/double/const-double receivers 220/220/220/220;
// complete chance decision bool/byte/short/long 220/220/220/220.
// NOT kept: no equality or frame/home recovery; original call/test/body restored.
// Frame 0x28 versus native 0x2c, unit/mode and independent look homes remain.
// M3 lane 1 r113 (fast with 0x165dc1, MATCH; control 220, aligned with offsets masked 86, registers too 88): the two look values
// as one `real look_angles[2]` 216 (frame 0x30: they take retail's -0x2c/-0x28 homes, but 1/rate is still shared and a dword at
// -0x30 is left unused; a grouping array, under the bar), the same array in its own block around the look springs 216, at function
// scope 216, with the callback written `index == 0 ? right : left` (retail's arm order) 216 (masked 82/75); `active` cleared, then
// set from the animation test under the three-term guard 416 (2188 B). On the array forms retail's unit, mode and weapon-active
// homes are -0x1c/-0x18/-0x14 where ours are -0x14/-0x1c/-0x18. Still BLOCKED by the stand-in (round 98) for ESI/EDI.
// M3 lane 12 r132 (fast with 0x165dc1, 0x166561 and 0x11eed0, MATCH; control 220; 0x16760c 142 in the reduced image as noted):
// the four spring overrides as ternaries 220 (same code); the aiming velocity copied into a local vector for the two spring
// calls 512 (2194 B); the look angles copied once into a local point 465 (2214 B); the orientation block index written
// user-major 220 (same code); the unit index in a named local for the test and the lookup 220 (same code). Still BLOCKED by
// the stand-in (round 98) for the ESI/EDI save after the unit test.
// m3 lane 16 r130 (fast with 0x165dc1 and 0x166561, both MATCH; control 220): the weapon loop under a redundant
// `if (0 < MAXIMUM_FIRST_PERSON_WEAPONS)` 220; the two look inputs as `real &` references 220; `active` declared at the
// unit-block scope 220; the animation-finished test as a nested if 220 (all same code). A game-time pointer copy taken
// after the yaw clamp and used for the pitch's rate and the third spring call's rate 208 (2197 B: the reciprocal is
// no longer shared, the pitch reloads the global as retail), but the other rate reads stay direct, a mixed
// local/global read, so not kept; the same pointer local for all eight rate reads 555 (2183 B).
// M3 lane 9 r131 (fast; base 220): a redundant `if (0 < MAXIMUM_FIRST_PERSON_WEAPONS)` guard around the weapon loop 220
// (same code); the two look rates as `real &` references to the user fields, used for the clamps and the smoothed stores
// 225; each look value divided in one statement and clamped in the next 223; the look values declared and clamped in a
// scoped block around their two smoothing calls 220 (same code: they still share the spring slots); guard and block
// together 220. Retail's g_510c54 reload after each look store still says those stores go through something that may
// alias the global; the ESI/EDI save region stays BLOCKED-BY the build's stand-in caller (round 98).
// M3 lane 8 r181 (fast; base 220): kept: the `goto overlay` dropped, the animation test folded into the 0.267 save's
// condition (`no_zoom && no_scope && name == 0x400000c`; the same code, 220: the compiler threads the mismatch to the
// overlay block as retail does). Tried: the clamped look values through a force-inline helper writing them through
// two out pointers 220; the look values as const references bound to the clamps in a block with their two spring
// calls 220; the user flags read and set in place without the flags local 220 (same code); zoom, no_zoom and no_scope
// declared where computed 220 (same code). The reciprocal stays shared and the look values keep the spring_a/b slots.
// Round 563. The frame is 0x28 against 0x2c, and the two look divisions still share a reciprocal. in-place
// lower then upper saturation 222, in-place upper then lower saturation 223, upper default with ordered lower
// and interior assignments 223, lower default with ordered upper and interior assignments 223, lower default
// and a conditional upper selection 487, upper default and a conditional lower selection 487, lower-limited
// rates followed by upper selections 486, upper-limited rates followed by lower selections 487.
// Round 401 tested paired look clamps with lower-first/upper-first branch defaults and upper/lower preset bounds, strict then inclusive: 219/220/223/223/221/222/224/224.
// The one-point lead is below the threshold. The 0x28 frame, shared reciprocal and reused spring homes remain.
// m3 lane 14 r108 (fast with 0x165dc1, MATCH; control 220, as plain). Retail's slot map: the yaw value has -0x2c to itself (3 stores, 1 load) and
// the pitch value shares -0x28 with the temporary that carries the channel duration to fmod; neither shares with the aiming springs or the unit,
// which are dead by then, while the flags, the loop index and the weapon pointer do reuse those slots. Tried: the look values as const references
// bound to the clamps 220 (same code); one point2f initialised with both clamps 216 (frame 0x30, homes -0x2c/-0x28, reciprocal shared; a grouping,
// under the bar); one inline two-axis spring helper for the aiming pair and the look pair, the look pair passed by address 588 (2163 B: the
// time global then lives in EBX across the calls and the user goes to the stack); the look values as an inlined helper's locals 220, as its
// by-value parameters 221; the clamp through an inline function with three returns, value by value 220, by const reference 220, returning a
// const reference 499 (2225 B); an empty inline function taking both look addresses after the fourth spring call 220 (same code: the link
// drops the address-taken state with the call). Diagnostics, reverted at once: both look addresses passed to a fifth 0x11eed0 call 220 (same
// code: that call is inlined and deleted whole, so round 120's escape to the second aiming call proved nothing); passed to an opaque external
// call 454 (2208 B, frame 0x30, look homes -0x14/-0x10): the yaw block is then retail's instruction for instruction (rate pointer reloaded, x
// divided by the rate, the 1.0 load between comiss and jbe) and the pitch divides again after a reload, as (y / rate) * -1 where retail has
// (-1 / rate) * y. So a store to an address-exposed look value is what ends the shared reciprocal, but retail's two homes sit at the frame's
// bottom with the compiler's late temporaries, not where an address-exposed local goes, and no code-free form found gives either.
// AC572: Captured entry unit index 220. Add guarded primary index, conditional
// NONE or branch-set NONE 220/529/529. The primary-index forms alone
// 220/529/529. Both captures with a terminal unit guard 220.
// NOT kept: frame 0x28, exchanged unit/mode homes and shared reciprocal persist.
// the two clamped look values as one `real look_angles[2]` take retail's
// The frame and independent look homes still differ. Separate suppression defaults 253/401/227/404.
// chained suppression defaults 253/401/227/404.
// MiniMax A (11-18): score 188 (was 220): the two clamped look values as one `real look_angles[2]` take retail's
// [ebp - 0x2c]/[ebp - 0x28] homes, the pitch read through a reference to the game-time globals keeps retail's two
// separate divisions (no shared reciprocal) with the multiply's operands in retail's order, and both look springs'
// rates through the same reference; still open: the frame is 0x30 against retail's 0x2c, the unit and mode
// homes are swapped and the ESI/EDI saves sit before the unit test.
// m3 lane 1 r120 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188 on the 11-18 body, so the older leads
// were re-measured on it): `active` cleared, then assigned `!busy && animating` under the unit and weapon guard 345 (2200 B);
// the callback as `index == 0 ? right : left` 188 (same code); the two channel flag bits tested on a long copy of the byte
// under the update test 375 (2198 B); `active` cleared, then assigned the animation test under the three-term guard 378
// (2191 B); the first three together 322 (2202 B); the preset pair with the callback 345; the look array in its own block
// around the clamps and the two look springs 188 (same code, frame still 0x30); the look array declared last or first in
// the unit block 188/188; `unit` and `mode` declared bare and assigned in the next statements 188 (same code).
// Direct look-spring rate reads 188, all six direct reads 219, named
// raw look rates 188, initialized look pair 482, separate mutable/const
// look outputs 196/196, shared aim-velocity pointer 206, unit reference 188.
// NOT kept: independent look locals reduce the frame to 0x28 and raise
// the score. The paired outputs keep both retail clamp homes at frame 0x30.
// Round 187 (m3 lane 8, fast with 0x165dc1 and 0x166561, both MATCH; control 188, aligned counts by sequence alignment in
// brackets, control [85]): 0-7 dead locals at the top 188 each (not a symbol-count state); the channel test as
// `& 2 || & 8` 188, as `& 8 || & 2` 188, through a byte local under the update test 188, with a `(dword)` widening 188 (all
// fold to `test byte ptr [edi + 0x19], 0xa` where retail loads the byte first; a bitfield view, diagnostic only, folds
// too); `active = false;`
// then set true under the whole four-term test 180 [86] (a preset flag, and no aligned gain: not kept); `active = false;`
// then `active = !busy && animating;` under the unit and weapon guard 345 at 2200 bytes [81]: that block is then retail's
// byte for byte, active at -0x14, but the instruction it adds shifts everything after it; the callback as an if/else on
// `index == 0` 188 [84], with the preceding active form 345 [80]; the user flags stored before the look history 195 [92];
// the look pointer declared const at its use 188 (same code). The `fmod` is the x87 intrinsic, so no double slot pads
// the frame. ESI/EDI placement: still BLOCKED-BY the build's stand-in caller (round 98).
// M3 lane 12 r145 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): retail keeps unit (later the zoom
// value) at -0x1c, mode at -0x18 and spring_d with the active byte at -0x14; ours unit and the active byte at -0x18, mode at
// -0x1c, spring_d and zoom at -0x14 (every other slot agrees; ours leaves a dword at -0x30 unreferenced). Renames for the
// name-order tip: mode as weapon_mode or animation_mode, unit as aiming_unit, zoom as aim_speed, active as weapon_active,
// 188 each (same code: these spill slots do not follow names); the active test passed straight to the set-active call 378
// (2192 B). Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 15 r102 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the frame reference dropped, every
// rate read through g_510c54 219 (2193 B: the yaw/pitch division and reload change); the weapon-active value passed
// straight to first_person_weapon_set_active without its local 378 (2192 B); the sound callback written into the update
// call 188 and the channel pointer spelled `&weapon->channelb8` at its uses 188 (same code each); the look pair as two
// scalars with the frame reference kept 196 (their homes move to -0xc/-0x8). Slot map against retail: every EBP offset
// used is the same; ours has one unreferenced dword below the look pair (frame 0x30), and packs the weapon-active byte
// with the unit/flags home (-0x18) and the zoom, indices pointer and idle globals with the spring_d home (-0x14), where
// retail packs the active byte with spring_d (-0x14) and the zoom group with unit/flags (-0x1c), mode at -0x18.
// M3 lane 14 r116 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the saved 40-point set (preset
// active flag, `== 0 ? right : left` callback, channel byte in a long tested bit by bit, positive zoomed/scoped, per-arm
// overlay play under `else if (zoomed)`) re-measured 40 at 2199 B on current main, still match-only and BLOCKED by the
// stand-in (round 98). Its keepable part alone (positive zoomed/scoped with `scoped = zoomed && ...`, rate arms in the
// positive order, the clear test without the implied zoom term, the overlay set read once, one shared overlay call) 386
// (2189 B); positive zoomed/scoped alone 205 (2203 B); the set without the four match-only forms (per-arm play under a
// plain `else`) 379 (2187 B); only the clear test's implied zoom term dropped 368 (2186 B).
// M3 lane 15 r105 (fast, plain where marked; aligned/masked = difflib counts with addresses / also registers masked, 60/57
// at 188): retail's zoom logic as positive bools (zoomed = speed > 0.1f, scoped = zoomed && the 0xc8a10 query, both cleared
// on the 0xd03b0 query, scoped cleared when the overlay lookup fails; rate `if (zoomed) { scoped ? 1 : pin } else if
// (scoped) 1`; clear test `!zoomed && !scoped || scoped && set != 0x60000d8 || !scoped && set == 0x60000d8`) with the play
// as a shared flags/set if/else 375 (2182 B, aligned 43); the play written per arm under `if (scoped) ... else if (zoomed)`
// 365 (2186 B, aligned 31); with `scoped = zoomed && function_c8a10(...)` 371 (2197 B, aligned 26); with the callback as
// `field_x11c898 == 0 ? right : left`, the channel byte in a `long` local tested `& 0xa`, the overlay set read once into a
// local and the active preset (`active = false; if (...) active = true;`) 368 (2193 B, aligned 24); that with a plain
// `else` for the play 371; the active flag as `active = false; if (unit && weapon) active = !0x155760 && 0xb6780;` (retail's
// preset store and branch shape) 338 (2197 B, aligned 12, masked 9: every frame slot is then retail's); the channel byte
// tested as `(flags & 2) || (flags & 8)` 40, plain 40 (2199 B, retail's length, aligned 10, masked 7; lead, saved as
// build/r105/lead-0x166d75-40.diff in the m3-lane15 worktree); on it the clear test as a ternary 278. Not kept: the lead
// needs the implied `else if (zoomed)`, the swapped callback ternary, the one-use channel-byte local and the active preset,
// all match-only, and the function stays BLOCKED by the stand-in (ESI/EDI saved at entry). Left on the lead: those saves,
// frame 0x30 against 0x2c, the weapon reload into EDI one statement late, the overlay set loaded ahead of the scoped test
// and the +0xb8 channel pointer in ECX (retail ESI).
// M3 lane 15 r107 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188), declarations joined to their first
// assignment: the +0xb8 channel pointer initialised where declared 188, `real zoom = magnitude3d(...)` 188, `point2f *look
// = ...` 188, the weapon index declared in the for header 188, `bool active = ...` 188 (same code each). The lead's steps
// stay match-only and the stand-in block stands.
// m3 lane 8 r198 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188). Frame arithmetic: retail has 11
// dword slots (0x2c), the yaw alone at -0x2c and the pitch at -0x28 shared with the duration temporary; ours has the same
// 11 slots and the same sharing, plus 4 bytes of padding: with the look pair as a three-element array (diagnostic) the
// frame is 0x38 (13 slots rounded up to 8, the array kept whole at -0x34 and the duration temporary in a slot of its own),
// so a local aggregate of 8 bytes or more rounds the frame to a multiple of 8, and retail's 0x2c says its two look values
// are no such aggregate. Tried: the look values as two one-element arrays 196 (frame 0x28: scalarised, they take the
// spring slots again); as two scalars in an inner block written through pointer locals 477 (2186 B, frame 0x28; the
// pointers fold away); the reference to the game-time globals as a const pointer local 188 (same code); the function
// with internal linkage 188 and without its explicit convention 188 (same code each: the argument stays on the stack and
// ESI/EDI are still saved ahead of the unit test). Re-measured: every rate read through g_510c54 219. Other matched
// functions of this file save a register after an early exit with the build's extra caller present (0x168896), so the
// extra caller does not forbid a late save in general; what makes this function's unit test wait for both saves is open.
// M3 lane 15 r108 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): every 0xb000067-arm use of the
// +0xb8 channel through the channel pointer (the overlay call, the frame position, the duration and the set) 202 (2189 B);
// the look pointer typed `point2f const *` 188 (same code). On round 105's 40-point lead (re-measured 40 at 2199 B on
// current main; still match-only and BLOCKED by the stand-in): the channel pointer at every channel use 81 (2192 B); the
// callback selected ahead of the orientation pointer 45; the weapon object looked up after the orientation pointer and
// the callback 97 (2200 B). Left on the lead: frame 0x30/0x2c, the ESI/EDI saves, the weapon reloaded into EDI ahead
// of the object lookup (ours after the callback select), the overlay set load order and the channel pointer register.
// M3 lane 14 r123 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): kept, the look pointer typed as the
// existing view-angles record (`&entries[user_index].angles`, yaw/pitch fields) in place of the point2f cast, and the look pair
// as one such record in place of a two-element array, 188 (same code each, plain). Tried: the weapon-active value as an
// if/else on the unit and weapon guard 188 (same code); the look pair as two scalars clamped by if/else-if chains 204 (frame
// 0x28); the weapon-active flag typed byte 387 (2197 B). Slot map (masked listing): retail packs zoom with unit (-0x1c) and the
// active byte with spring_d (-0x14), ours the other way round; the ESI/EDI saves stay BLOCKED by the build's stand-in caller.
// M3 lane 13 r127 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188). Slot references counted on both
// listings: every EBP offset from -1 to -0x2c is used in both builds and ours leaves -0x30 unreferenced; retail's -0x10
// (spring_c) is never reused, its -0x14 holds spring_d and then the active byte, its -0x1c the unit, the user flags and then
// the zoom group; ours puts the active byte with the unit and flags (-0x18) and the zoom group with spring_d (-0x14). The
// homes follow lifetimes, not scopes: `bool active` declared at the unit block's top 188, `real zoom` there 188, both in
// either order 188 / 188, the weapon object pointer declared there and assigned in the loop 188, `flags` declared ahead of
// the look pair 188 (same code each). Retail pops EDI and ESI right after the weapon loop, ahead of the two flag tests
// that close the unit block, and pushes each just ahead of its first use after the unit test; ours saves both ahead of
// the test and restores them at the end (the round 98 block). Body unchanged.
// M3 lane 14 r125 (fast; control 188): slot sharing against retail: retail packs unit, the user-flags copy and zoom in
// -0x1c, mode in -0x18, spring_d and the active byte in -0x14; ours packs unit, flags and active in -0x18, mode in -0x1c,
// spring_d and zoom in -0x14. Tried: `bool active` declared in the unit block 188, `real zoom` declared in the unit block
// 188, the look record declared at function scope 188, `flags` declared after the weapon index 188 (same code each).
// Diagnostic (reverted at once): the look pair as two scalars plus one probe local 537 (2203 B, frame 0x28, ten cells),
// so the frame's odd cell count cannot tell whether the record alone rounds the frame. Retail's epilogue confirms the
// shrink-wrap: ESI/EDI are popped right after the weapon loop, before the flag tail; still BLOCKED by the stand-in.
// MiniMax B (11-20): no new form, no lower score (188): the channel pointer still differs at every channel use and
// the look pair still needs an aggregate slot of its own, so the frame stays 0x30 against retail's 0x2c.
// Round 611. previous angle field view 193, previous angle record store 193, delta angle field view 193, delta
// record with reset 193, both typed angle records 193, scoped angle and flag publication 193, separate clamped
// scalars with angle views 201, current angle snapshot for both consumers 422. NOT kept: the record views leave the
// frame at 0x30 and the scalar form takes it past retail to 0x28. The unit and mode allocation remains unresolved.
// M3 lane 11 r136 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the look pair as two scalars
// declared at the function's top 196, declared at their clamps 196 (frame 0x28 both: they share the spring slots); the
// end-of-loop flag test reading the first weapon through `first_weapon` 188; the weapon counter declared at the function's
// top 188; `unit` declared bare at the function's top and assigned in place 188 (same code each). Still BLOCKED by the
// stand-in (round 98) for the ESI/EDI saves.
// M3 lane 4 r237 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): `bool active` in its own block with the
// set-active call 188; zoom, no_zoom and no_scope declared at the loop body's top 188; both together 188 (same code each); the
// sound callback selected ahead of the orientation pointer 185 (a statement order, under the bar), with the active block 185;
// `unit` as a const pointer 188. Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 8 r208 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the overlay lookup tested
// without the named class-type result 197 (frame 0x2c, but the result temporary then lives at -0x28 and the weapon-object
// and frame homes move; retail's named result at -0x10 is right); the look record in its own block around the four
// spring statements 188; the look pointer and the flags copy declared at their assignments 188; the weapon counter
// declared in the for statement 188; no channel pointer (the +0xb8 channel written out at its test and clear) 188 (same
// code each).
// MiniMax R (11-20): score 188 (was 188): no new form; the frame stays 0x30 against retail's 0x2c (the look pair still
// needs an aggregate slot of its own), the unit and mode still take each other's slot against retail, and esi and edi are
// still saved ahead of the unit test where retail saves them after it, so the body stays BLOCKED by the stand-in.
// M3 lane 12 r166 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the channel byte tested as
// `(byte & 2) || (byte & 8)` on this body 188 (same code: folded to one `test byte ptr, 0xa`); the view angles read at each
// use with no look pointer 415 (2208 B); the 0x98 overlay play written in each arm with constant arguments 339 (2194 B); the
// weapon object looked up after the orientations pointer 188 (2197 B); `no_scope = no_zoom = true;` 188 (same code). Still
// BLOCKED by the stand-in (round 98) for the ESI/EDI saves. Body unchanged.
// M3 lane 2 r126 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): values re-bound by a const
// reference at their later uses: the mode at both overlay calls 188, the zoom speed at its test and clamp 188, the
// weapon-active flag at the set-active call 188, the user-flags copy at its test and store 188 (same code each); the mode
// lookup after the four spring defaults 204. Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 6 r219 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the mode lookup after the four
// spring defaults 204; the game-time reference taken ahead of the first smoothing call and used for every rate read 224
// (2193 B); the reference taken ahead of the yaw only 219 (2193 B); the look entry bound as a const reference instead of
// the look pointer 188 (same code); the first weapon's object named before its definition lookup 188 (same code). Still
// BLOCKED by the stand-in (round 98) for the ESI/EDI saves; unit/mode slots swapped and frame 0x30/0x2c unchanged.
// NOT kept (M3 review 11-21, MiniMax C 11-20): the round-105 lead (score 40) applied again; as lane 15's r105 note says,
// it rests on the swapped callback ternary, the one-use channel-byte local, the active preset and the implied
// else-if, all match-only, so it is kept only with a byte match. The body is main's again.
// M3 lane 4 r241 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the rate read from the global at
// each use instead of the frame-settings reference 219 (2193 B); the +0xb8 channel pointer used for the play, the frame read
// and the frame store as well 202 (2189 B); the look angles read from the global's entry at each use with no look pointer
// 415 (2208 B); `first_weapon = user->weapons` 188 (same code); the set-active flag passed straight to the call with no named
// bool 378 (2192 B). Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// Round 627. Direct scope result from named or const overlay ID 220/220,
// weapon-scope ID with branch or direct result 362/373, signed overlay index
// with direct result or sentinel branch 213/197, direct result with scoped
// look pair 220, renamed overlay ID with direct result 220. NOT kept: the
// 0x30 frame and swapped unit/mode slots remain, and all forms regress.
// M3 lane 3 r291 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): `active` renamed weapon_active
// 188 (same code: the active byte still shares the unit slot, so the slot packing does not follow the names); `bool active
// = false;` as a dead initialiser 188 (same code, no early store); the overlay flags and set passed as two `no_scope ?`
// conditionals 350 (2203 B); the four definition overrides tested as `0.0f != field` 539 (2193 B); the zoom test as
// `0.1f < zoom` with the blend test as `1.0f == ...` 191. Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 16 r168 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the weapon object looked up
// inside the 0xb000067 arm, its only use, 245 (2197 B); the sound callback selected in the update call's argument with no
// callback local 188 (same code); the aiming speed computed ahead of the update call 388 (2204 B); the 0x98 overlay's
// channel flags and set as two conditional expressions on `no_scope` 350 (2203 B); the busy test as `== 0` and the
// countdown as `-= 1` 188 (same code). Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves. Body unchanged.
// Existing object getter at unit/primary weapon/iterated weapon, singly, paired,
// and together 188 each. Existing header getter and object member at all three
// 188. The 0x30 frame, unit/mode slots and early ESI/EDI saves persist.
// Round 666. The 0x30 frame and early register saves persist.
// Unit handle 188, early unit guard 188, guard with reused handle 188, named mutable
// or immutable clamp targets with the look record 188/188, separate scalar inputs
// 196/196, unit handle with immutable weapon flag 188, shared raw clamp target 188,
// and separate clamp-target scopes 188. Scalar spring inputs reserve 0x28 instead of 0x2c.
// M3 lane 2 r135 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): a named const pointer to the
// unit's aiming velocity for both smoothing calls and the zoom magnitude (three uses) 206 (2193 B); the sound callback
// chosen by an if/else statement 188 (same code); the update test and the channel-byte test as two nested ifs 188 (same
// code); the zoom logic in retail's positive sense (`zoomed = zoom > 0.1f; scoped = zoomed && query;`, both cleared on the
// 0xd03b0 query, every later test rewritten directly) 205 (2203 B; the older rounds' 385 is out of date); that with the
// 0x98 overlay play written per arm with constant arguments 352 (2201 B). Still BLOCKED by the stand-in (round 98) for the
// ESI/EDI saves. Body unchanged.
// M3 lane 3 r294 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the four spring defaults declared
// d, c, b, a 192; `no_scope` set by an if/else on `no_zoom` 376 (2197 B); the sound callback chosen by an if/else on the
// nonzero index 188; the idle counter incremented in its own statement ahead of the test 188; the busy and animating
// tests spelled `== 0` / `!= false` 188 (same code each). Re-measured by mistake: zoom computed at its declaration ahead
// of the update call 388 (listed). From the look values to the weapon loop the rows now differ only in the unit and mode
// slot numbers. Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 8 r223 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): `mode` declared ahead of `unit`
// and assigned in place 188; `zoom` declared at the function's top 188; `active` declared at the function's top 188;
// the channel frame declared at its assignment 188; the orientation index written `user_index * 2 + weapon` 188 (same
// code each). Retail's -0x1c slot is shared by the unit, the zoom and two later spills, ours by the unit and the active
// byte. Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves.
// M3 lane 12 r176 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188, aligned 85): our 0x30 frame never
// touches its lowest cell (both builds use -0x2c..-0x4), so the extra four bytes are an unused home; the active byte declared at
// function scope 188, zoom at function scope 188, the two zoom flags at function scope 188, the weapon-slot index declared in
// the for statement 188, the active byte declared at its assignment 188 (same code each: scopes move no home). Still BLOCKED
// by the stand-in (round 98) for the ESI/EDI saves. Body unchanged.
// M3 lane 6 r231 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188, aligned 85): the zoom block in
// retail's positive sense as read from its listing (zoomed = zoom > 0.1, scoped = zoomed && the 0xc8a10 query, both cleared on
// 0xd03b0, scoped cleared when the overlay is missing, rate `zoomed && !scoped` pinned else `scoped` 1.0, clear on `!zoomed &&
// !scoped || scoped && current != set || !scoped && current == set`, play and save per arm with constant arguments) with a
// plain `else` 364 (2191 B, aligned 36); with retail's `else if (zoomed)` (implied test, match-only) 371 (2197 B, aligned 31,
// masked 28); positive sense with the shared flags/set play 381 (2193 B); the else-if form with the channel's current set read
// once into a named local for both compares 376 (2193 B, aligned 30: retail's hoisted `mov eax, [edi + 0xa0]`), the plain-else
// form with it 379 (2187 B). The last six bytes are the active preset and the movzx channel-byte test (both match-only), the
// rest the stand-in's ESI/EDI rows. Body unchanged.
// M3 lane 7 r218 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): a named indices pointer for the
// three indices uses 188 (same code); a named pointer to the +0x98 overlay channel at all ten uses 194 (2187 B), only in the
// first overlay block (where retail holds it in ESI) 194 (2187 B); the zoomed arm's rate as one conditional expression 188
// (same code); the overlay play's flags/set arms in the no-scope-first order 189. Still BLOCKED by the stand-in (round 98)
// for the ESI/EDI saves. Body unchanged.
// M3 lane 1 r161 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188): the scope overlay's index
// tested on the call's returned value with no named local, in the `!no_scope` test 197 or nested under it 197 (frame
// still 0x30: the named animation local is not the unused cell); the channel frame expression passed straight to
// set_frame_position 335 (2189 B); the user flags read and set in place with no `flags` local 188 (same code); the idle
// globals pointer read in place at its three uses 225 (2205 B). Frame 0x30 in all five. Still BLOCKED by the stand-in
// (round 98) for the ESI/EDI saves. Body unchanged.
// M3 lane 2 r144 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 188, aligned 62): retail keeps the two
// look values as separate scalars in their own cells at the frame's bottom (yaw alone at -0x2c, pitch at -0x28 shared
// with a late temporary) where our scalars pack into the dead aiming springs' cells; tried to start their ranges
// earlier: each look value assigned its quotient, then clamped in a second statement, as two scalars with every rate
// read through g_510c54 224 (2193 B, frame 0x28, aligned 85), the same with the pitch and the look springs through the
// frame reference 204 (frame 0x28, aligned 72), the same two statements on the kept pair 202 (frame 0x30, aligned 66);
// two scalars with main's one-statement clamps and no reference re-measured 221 (2193 B, frame 0x28); the frame
// reference bound at the top of the unit block for all four springs and both divisions 224 (2193 B, frame 0x30), bound
// just ahead of the yaw clamp 219 (2193 B: the divisions share the reciprocal again); the reference to const 188 (same
// code). Still BLOCKED by the stand-in (round 98) for the ESI/EDI saves. Body unchanged.
// Round 114: Explicit clamps into the look members 188. Initializing the look pair from clamped scalars 196 (frame
// 0x28). Named values for all four spring overrides 548, primary handle selected before its guard 526, primary slot
// reused through the final test 188, overrides before mode lookup 574, one overlay set read for both clear tests 362,
// and scope flag assigned from the overlay index 220. The unit and mode homes and the early saves still differ.
// Unit/mode and activity/zoom homes still differ. Immutable mode 188, aiming vector reference 569, first
// receiver before unit admission 188, nested handle admission 188, clear activity after failed queries 183,
// nested animation success tests 188, retained clock for late updates 460, direct short overlay index 197.
// NOT kept: the activity form improves by five at the opening length.
// M3 lane 8 r232 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH each run; control 188). The padded frame and the
// ESI/EDI saves at entry were never the build's extra caller: they are 8-byte stack parity, which this function kept
// because its callee 0x11eed0 had a named double local (the same effect as noted at 0x11f0d0). Kept, in
// src/unknown_11eed0.cpp: 0x11eed0's braking-speed product written at both uses with no double local (0x11eed0 still
// MATCH) 172 at 2196 B: frame 0x2c, ESI and EDI pushed after the unit test and popped at the loop's exit, as retail; the
// first difference is now +0x1e, the unit/mode homes. Forms of 0x11eed0 that still pad this frame (188): the product
// through a forced-inline helper taking it as a double parameter, doubled inside or at the call, with the parameter const
// and a const double sum; a diagnostic `real` product gives 172 here but breaks 0x11eed0. Diagnostic, reverted at once:
// the look pair as two scalars plus one volatile cell, eleven cells, still reserved 0x30 on the old base, so the padding
// never came from the look record. On the 172 base (fast): the zoom logic in retail's positive sense with the set read at
// each use and a shared play 374 (2193 B), with a per-arm play under a plain else 357, under `else if (zoomed)` 364; with the
// set read once ahead of the test and a shared play 379, a plain-else play 372; the active preset alone 338 (2200 B); the
// callback as `== 0 ? right : left` alone 172 (same code); the channel byte in a long local tested bit by bit alone 368.
// LEAD, match-only (build/r232/lead-0x166d75-17.patch): the active preset, that callback order, the channel byte local, the
// positive zoom logic, the per-arm play under `else if (zoomed)` and the overlay set read once: 22 at retail's 2199 B, every
// frame home retail's; with the set assigned inside the clear test after its first term, `!zoomed && !scoped || (set = ...,
// scoped && set != ...) || !scoped && set == ...`, 17 (retail loads the set after the first term); without the callback
// order 22 (the select's arms exchange); without the preset 363, without the byte local 331, with a plain-else play 244. On
// the lead the set read at each use 268 (two memory compares), through a one-line inline accessor at each use 268, as a
// ternary on `scoped` 278, as `scoped != (set == ...)` 270; the +0xb8 pointer initialised at the block's top 17 (same
// code). Left at 17: the weapon's reload into EDI sits after the callback select where retail has it ahead of the object
// lookup (retail's EDI range starts one block earlier, with no use of the weapon in that block), and the +0xb8 channel
// pointer is in ECX across the 0xb6760 call where retail holds it in ESI (three rows). Diagnostic for the last: an
// undefined stand-in for 0xb6760 at that one call gives ESI, so retail's call there had no register summary; `inline` on
// 0xb6760's definition is the listed lane 7 form (it changes four other callers) and a second copy of it is barred.
// M3 lane 8 r233 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; round 232's 17 lead rebuilt from its patch,
// 17, the same four rows). On the lead: the channel pointer used for the play, frame and duration calls in the found
// arm 60 (2192 B); no channel local, the +0xb8 field's address at the test and the clear 17 (same code). Body unchanged.
// Cumulative activity across unit, weapon or user readiness 172/172/172; pairs
// of those boundaries and all three stages 172/172/172/172; nested continuation
// from partial readiness 368. NOT kept: the unit and mode homes still exchange.
// M3 lane 1 r164 (h2m fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; lane 8 r232's lead patch still applies and
// reads 17 at 2199 B, main 172). On the lead: the callback chosen ahead of the orientations 22; the weapon object
// looked up after the orientations 72 (2200 B); the animation-name test negated with the clear in the then-arm 79
// (2195 B); the +0xb8 test's result in a named bool 17 (same code); the +0xb8 pointer declared at its assignment 17
// (same code). The channel pointer lives only across the 0xb6760 call and our link knows that callee leaves ECX alone,
// so it stays in ECX; the weapon pointer's reload into EDI is placed at its first use, retail's at the block's start.
// Body unchanged (the lead is match-only).
// On the positive overlay form, admitted handle 334; animation receiver for the whole arm 304 or just
// update and events 320; handle with update receiver 320; post-play channel pointer or reference 340/340;
// direct validity and clear with that frame receiver 340; default callback with admitted handle 411.
// The unit/mode homes and restart-channel register still differ.
// The unit and mode slots still need retail order. Three-state overlay in enum/byte: nested promotion 420/421,
// eligible selection 413/423, exclusive selection 424/407, nested conditional 424/407.
// NOT kept: every result shortens the body and raises the score.
// The unit and mode slots remain exchanged. Unit receiver scoped through the aiming springs or all four springs,
// with its vector address acquired before the mode call, before the first spring, between springs or after aiming:
// 562/190/190/172 in each scope. NOT kept: early address acquisition shortens and raises the score.
// Activity preset on physical admission with readiness rejection 345, fully admitted
// result 373, nested readiness result 371, admission conditional 172. Scope after the
// suppression query 357, shared suppression result 378, nonsuppressed overlay result 179,
// complete optical guard 185. The unit and mode homes still exchange.
// Unit and mode spill slots remain exchanged. Named blend-weight or blend-rate
// field pointers, references or mixed pairs 172 in all eight forms. The later
// weight address still shares the fourth spring slot instead of the unit slot.
// M3 lane 8 r238 (H2M_FAST on the round 232 lead, rebuilt at 17): the allocator puts the +0xb8 channel pointer in ECX
// because nothing else holds ECX or EDX across the 0xb6760 call (a forced-ranking search placing it in ESI gives 14:
// that choice is the three rows); the weapon's EDI piece starts at its first use, after the callback select. On the
// lead: `long weapon_index` read once for the test and the object lookup (0x16760c's spelling) 17; the callback by
// if/else 17; the found arm's duration and frame-position calls through `channel` 17; its frame-position read through
// `channel` 60 (2192 B); the callback ternary inside the update call 17 (same code).
// M3 lane 16 r188 (H2M_FAST with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 172; lane 8 r232's 17 lead rebuilt,
// 17). Frame reading on main: retail packs the weapon-active byte and mode ahead of unit (ours packs unit first, as it
// reaches three references first), so retail's active byte joins spring d's cell and zoom then joins unit's; that needs
// one more reference each for the active byte (the preset) and for mode (the play written per arm), both match-only. The
// active value as `unit and weapon valid ? !busy && animating : false` 172 (same code). On the lead: one channel pointer
// for the +0x98 block, then set to +0xb8 for the last block, 62 (2193 B); the +0xb8 clear through the member 17; the
// orientation index as `user_index * 2 + weapon` 17; the +0xb8 play through the channel pointer 17 (same code each).
// With the +0xb8 pointer forced into ESI (diagnostic, not source) the lead reads 14; the other 14 rows are the weapon's
// EDI reload, placed at its first use where retail places it one block earlier. Body unchanged.
// M3 lane 1 r168 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 172; lane 8 r232's lead rebuilt, 17). On
// the lead: the orientation count read into a local ahead of the callback select 342 (2200 B). Reading on the lead: the
// weapon's EDI piece is ranked 49 over the push block to the last weapon use, so it starts at the push block after the
// select; nothing in the lookup block references the weapon (its index is the test's load). On main: one time-globals
// reference declared ahead of the aiming springs and used for all six rate reads 208 (2193 B); the first-weapon test
// without its null check 565 (2185 B: retail keeps the check); the user flags tested and set through the field (no flags
// local) 172 (same code); the weapon loop's index declared in the for statement 172 (same code). Body unchanged.
// Indexed aiming inputs with mutable or const unit pointer/reference, each with
// or without one vector address shared by the first spring and later magnitude:
// eight at 172. The unit and mode slots remain exchanged.
// Orientation count or buffer ordinal before object lookup 361/172; typed
// target, output or both banks 172/171/171; shared user ordinal or byte offset
// 172/383; both banks and count before lookup 361. NOT kept: the output bank
// gains one and the early count changes its read phase. The byte offset grows
// the frame to 0x30. The unit and mode slots remain exchanged.
// M3 lane 16 r192 (H2M_FAST with 0x165dc1, 0x166561, 0x11eed0; control 172, aligned 76, masked 73): the exchanged
// unit/mode homes against name order: `mode` renamed to a name sorting after `unit` 172, `unit` renamed to one sorting
// before `mode` 172, both 172 (same code each: these homes do not follow names); the unit local replaced by a pointer to
// its aiming velocity (the only member read) 562 (2189 B); the look angles read from the table at each use (no look
// pointer) 408 (2208 B). Body unchanged.
// GA16 (fast with 0x165dc1, 0x166561 and 0x11eed0, all MATCH; control 172, 2196 B, 50 trials):
// LEAD, match-only (build/ga16/lead-0x166d75-score3.patch): on the r232 match-only lead, scoping
// `s_animation_state *animation = &weapon->animation;` to 1..4 uses (`update`, `channels[0]`,
// `overlay_get`, `channel_update`) moves `mov edi, [ebp - 0xc]` up to retail's exact 0x16710b
// (keeping frame 0x2c) and replacing the embedded-assignment clear test with `bool should_clear` +
// `long current_set` in the else branch hits score 3 (2199/2199 B, +0x593, masked 0: 576/579 insns
// exact); 5+ uses of `animation` grow the frame to 0x30 (309..374); `should_clear` omitted 8,
// plain `else` play 230 (2193 B). On the score-3 base, all +0xb8 pointer forms (`get_duration`/
// `set_frame_position`, `overlay_play_for_class`, reference, top/pre-98 init, direct `&weapon->channelb8`)
// stay at 3 (only `lea ecx/mov eax, ecx` x3 vs retail's `esi` across `call 0xb6760`, whose LTCG
// register summary preserves ECX here); `channel++` from channel98 309 (frame 0x30), inverted
// animation-name guard 65 (2195 B). On the 172 base, `s_animation_state *animation` (2..4 uses,
// with or without `== 0 ? right : left`) fixes the 0x16710b reload cleanly at frame 0x2c (173,
// 2196 B; 5+/all uses 374, frame 0x30); shift/bitfield/TEST_FIELD_BIT channel-byte tests all fold
// to `test byte ptr [edi + 0x19], 0xa` (331..337, 2197 B). Body unchanged (score-3 lead is match-only).
// M3 lane 3 r313 (fast with 0x165dc1, 0x166561 and 0x11eed0; main 172, the round-232 lead rebuilt at 3): on the lead,
// `inline` on 0xb6760's definition gives MATCH (a held form) but 0x1cd070 MATCH -> 150 and 0x106790 61 -> 113; `inline` on
// this file's declaration only 3; an explicit cdecl convention on 0xb6760 everywhere 3. Retail's 0x106790 also keeps its
// pointer in ESI across both of its out-of-line 0xb6760 calls (ours ECX) and expands 0xb6760's shape at its channel-0
// tests, so retail's callers had no register summary for it: BLOCKED-BY 0xb6760. On main: the user flags read ahead of the
// look pointer 178; a named aiming-velocity pointer for the two aiming smoothing calls 190 (2193 B); the four overrides as
// conditional assignments 172; the first-weapon pointer ahead of the springs 172; the frame-settings reference taken ahead
// of the yaw clamp 203 (2193 B).
// @retail 0x166d75
void __stdcall function_166d75(long user_index)
{
	s_first_person_user *user = &first_person_users[user_index];

	if (user->unit_index != NONE)
	{
		s_166d75_unit *unit = (s_166d75_unit *)first_person_object_get(user->unit_index);
		long mode = first_person_weapon_mode_name(user_index);
		real spring_a = 72.0f;
		real spring_b = 15.0f;
		real spring_c = 27.0f;
		real spring_d = 6.0f;
		s_first_person_weapon *first_weapon = &user->weapons[0];
		s_view_angles look_angles;
		s_view_angles const *look;
		dword flags;
		long field_x11c898;

		if (first_weapon && first_weapon->weapon_index != NONE)
		{
			s_166d75_weapon_definition *definition =
				(s_166d75_weapon_definition *)first_person_object_definition_get(first_person_object_get(first_weapon->weapon_index));

			if (definition->unknown2e0 != 0.0f)
			{
				spring_a = definition->unknown2e0;
			}
			if (definition->unknown2e4 != 0.0f)
			{
				spring_b = definition->unknown2e4;
			}
			if (definition->unknown2e8 != 0.0f)
			{
				spring_c = definition->unknown2e8;
			}
			if (definition->unknown2ec != 0.0f)
			{
				spring_d = definition->unknown2ec;
			}
		}
		function_11eed0((real *)&user->unknown2030[0x0], (real *)&user->unknown2030[0x8], unit->aiming_velocity.i, g_510c54->rate, spring_b, spring_a, false, -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x4], (real *)&user->unknown2030[0xc], unit->aiming_velocity.j, g_510c54->rate, spring_b, spring_a, false, -1.0f, 1.0f);
		look_angles.yaw = PIN(*(real *)&user->unknown2030[0x28] / g_510c54->rate, -1.0f, 1.0f);
		s_game_time_globals &frame_settings = *g_510c54;
		look_angles.pitch = PIN(*(real *)&user->unknown2030[0x2c] * (-1.0f / frame_settings.rate), -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x10], (real *)&user->unknown2030[0x18], look_angles.yaw, frame_settings.rate, spring_d, spring_c, false, -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x14], (real *)&user->unknown2030[0x1c], look_angles.pitch, frame_settings.rate, spring_d, spring_c, false, -1.0f, 1.0f);

		look = &g_4ed284->entries[user_index].angles;
		flags = user->flags;
		if (TEST_FLAG(flags, _first_person_user_active_bit))
		{
			*(real *)&user->unknown2030[0x28] = function_0bff60(look->yaw, *(real *)&user->unknown2030[0x20]);
			*(real *)&user->unknown2030[0x2c] = function_0bff60(look->pitch, *(real *)&user->unknown2030[0x24]);
		}
		else
		{
			*(point2f *)&user->unknown2030[0x28] = *g_4687c4;
		}
		*(real *)&user->unknown2030[0x20] = look->yaw;
		*(real *)&user->unknown2030[0x24] = look->pitch;
		user->flags = flags | FLAG(_first_person_user_active_bit);

		for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
		{
			s_first_person_weapon *weapon = &user->weapons[field_x11c898];
			bool active;

			if (weapon->unknownf2 > 0)
			{
				weapon->unknownf2--;
			}
			active = user->unit_index != NONE && weapon->weapon_index != NONE &&
				!function_155760(user_index) && function_0b6780((s_index_triple *)&weapon->animation);
			first_person_weapon_set_active(user_index, field_x11c898, active);

			if (weapon->weapon_index != NONE)
			{
				s_166d75_weapon_object *weapon_object = (s_166d75_weapon_object *)first_person_object_get(weapon->weapon_index);
				byte *orientations = first_person_orientations + (field_x11c898 + user_index * MAXIMUM_FIRST_PERSON_WEAPONS) * 0x1000;
				animation_event_callback callback = field_x11c898 ? (animation_event_callback)first_person_weapon_sound_event_left :
					(animation_event_callback)first_person_weapon_sound_event_right;
				real zoom;
				bool no_zoom;
				bool no_scope;
				c_animation_channel *channel;

				if (weapon->animation.update(callback, user_index, weapon->orientation_count,
					(s_blend_orientation *)(orientations + 0x800), (s_blend_orientation const *)orientations) &&
					(weapon->animation.channels[0].unknown11 & 0xa))
				{
					first_person_weapon_animation_finished(user_index, field_x11c898);
				}
				zoom = magnitude3d(&unit->aiming_velocity);
				no_zoom = !(zoom > 0.1f);
				no_scope = no_zoom || !function_c8a10(user->unit_index);
				if (function_d03b0(user->unit_index))
				{
					no_zoom = true;
					no_scope = true;
				}
				if (!no_scope)
				{
					c_type_709360 animation_id = weapon->animation.overlay_get(0x60000d8);

					if (animation_id.index == NONE)
					{
						no_scope = true;
					}
				}
				if (function_0b6760((s_index_pair *)&weapon->channel98))
				{
					if (!no_zoom)
					{
						if (no_scope)
						{
							weapon->channel98.rate = PIN(zoom, 0.5f, 1.0f);
						}
						else
						{
							weapon->channel98.rate = 1.0f;
						}
					}
					else if (!no_scope)
					{
						weapon->channel98.rate = 1.0f;
					}
					weapon->animation.channel_update(&weapon->channel98, NULL, 0);
					if (no_zoom && no_scope ||
						!no_scope && weapon->channel98.unknown08 != 0x60000d8 ||
						!no_zoom && no_scope && weapon->channel98.unknown08 == 0x60000d8)
					{
						weapon->channel98.clear();
						if (no_zoom && no_scope && weapon->animation.animation_name == 0x400000c)
						{
							first_person_weapon_save_orientations(user_index, field_x11c898, 0.267f);
						}
					}
				}
				if ((!no_zoom || !no_scope) && !function_0b6760((s_index_pair *)&weapon->channel98))
				{
					long set;
					dword channel_flags;

					if (!no_scope)
					{
						channel_flags = 0x3f;
						set = 0x60000d8;
					}
					else
					{
						channel_flags = 0x803f;
						set = 0x6000060;
					}
					weapon->animation.overlay_play_for_class(&weapon->channel98, channel_flags, mode, 0x7000101, set);
					first_person_weapon_save_orientations(user_index, field_x11c898, 0.1335f);
				}
				channel = &weapon->channelb8;
				if (function_0b6760((s_index_pair *)channel))
				{
					if (weapon->animation.animation_name == 0xb000067)
					{
						real frame;

						weapon->animation.overlay_play_for_class(&weapon->channelb8, 0x803f, mode, 0x7000101, 0x12000068);
						frame = (weapon_object->unknown188 + 1.0f) * g_510c54->rate * 2.0f +
							weapon->channelb8.frame_position * (1.0f / 30.0f);
						weapon->channelb8.set_frame_position((real)fmod(frame, weapon->channelb8.get_duration()) * 30.0f);
					}
					else
					{
						channel->clear();
					}
				}

				function_11eed0(&weapon->indices.unknownc, &weapon->indices.unknown10, 0.0f, g_510c54->rate, 6.0f, 9.0f, false, 0.0f, 1.0f);
				if (weapon->indices.unknownc == 1.0f)
				{
					weapon->indices.unknown10 = 0.0f;
				}
				weapon->animation.blend_counters_update();
				if (function_187420(user_index) == 0.0f && function_187a40(user_index) == NONE &&
					weapon->indices.unknownc == 0.0f && *(real *)&user->unknown2030[0x0] == 0.0f &&
					*(real *)&user->unknown2030[0x4] == 0.0f && *(real *)&user->unknown2030[0x10] == 0.0f &&
					*(real *)&user->unknown2030[0x14] == 0.0f)
				{
					if (weapon->animation.animation_name == 0x400000c)
					{
						s_first_person_weapon_globals *globals = ((s_166d75_globals_view *)g_4e034c)->field_b4;

						if (weapon->unknownee == 0)
						{
							weapon->unknownee = (short)function_1469f0(function_259d0(&g_4e7408->seed, NULL, 0,
								globals->idle_delay.lo, globals->idle_delay.hi));
						}
						if (++weapon->unknownec > weapon->unknownee)
						{
							weapon->unknownee = 0;
							if (function_x82e52f(&g_4e7408->seed, NULL, 0) >= globals->idle_chance)
							{
								first_person_weapon_set_animation(user_index, field_x11c898, 0x600005f, true);
							}
						}
					}
					else
					{
						weapon->unknownec = 0;
					}
				}
				else
				{
					weapon->unknownec = 0;
					if (weapon->animation.animation_name == 0x600005f)
					{
						first_person_weapon_set_animation(user_index, field_x11c898, 0x400000c, true);
					}
				}
				if (TEST_FLAG(weapon->flags, _first_person_weapon_active_bit))
				{
					function_16760c(user_index, field_x11c898);
				}
			}
			if (--weapon->unknownf0 <= 0 && !function_13cb40())
			{
				function_16840e(user_index, field_x11c898);
			}
		}
		if (!TEST_FLAG(user->weapons[0].flags, _first_person_weapon_active_bit) &&
			!TEST_FLAG(user->weapons[1].flags, _first_person_weapon_active_bit))
		{
			SET_FLAG(user->flags, _first_person_user_active_bit, false);
		}
		if (user->unknown202c.unknown1)
		{
			function_1d9320((s_1d9240 *)&user->unknown202c);
		}
	}
}

/* the parts of the weapon, its definition and the render models read here */
struct s_16760c_weapon_object
{
	byte unknown000[0x188];
	real unknown188;
	byte unknown18c[0x226 - 0x18c];
	short unknown226;
	short unknown228;
	byte unknown22a[2];
	short unknown22c;
};

struct s_16760c_magazine_definition
{
	byte unknown00[0xa];
	short rounds_maximum;
};

struct s_16760c_weapon_definition
{
	byte unknown000[0x292];
	short unknown292;
	byte unknown294[0x2a8 - 0x294];
	long interface_count;
	s_first_person_interface *interfaces;
	byte unknown2b0[0x2c4 - 0x2b0];
	s_16760c_magazine_definition *magazines;
	byte unknown2c8[0x308 - 0x2c8];
	point3f first_person_offset;
};

struct s_16760c_render_model_node
{
	byte unknown00[0xc];
	point3f default_translation;
	quaternionf default_rotation;
	byte unknown28[0x60 - 0x28];
};

struct s_16760c_render_model
{
	byte unknown00[0x48];
	long node_count;
	s_16760c_render_model_node *nodes;
};

/* an orientation (0x20 bytes) */
struct s_16760c_orientation
{
	quaternionf rotation;
	point3f position;
	real scale;
};

extern real_quaternion_transform *g_4687d8;

void __stdcall function_bd970(long weapon_index, s_16760c_render_model *render_model, s_animation_state *state, long unknown,
	long node_count, byte *orientations);
long function_100b40(long magazine_index, long weapon_index, bool weapon_only);
void function_1416c0(point3f const *position, transform4x3f *out);
real function_1d9370(s_1d9240 const *p);


static __forceinline void first_person_orientations_from_model_16760c(s_16760c_render_model const *render_model, long const *node_map,
	s_16760c_orientation *orientations)
{
	short node_index;

	for (node_index = 0; node_index < render_model->node_count; node_index++)
	{
		s_16760c_render_model_node const *node = &render_model->nodes[node_index];
		short graph_node_index = (short)node_map[node_index];

		if (graph_node_index != NONE)
		{
			orientations[graph_node_index].rotation = node->default_rotation;
			orientations[graph_node_index].position = node->default_translation;
			orientations[graph_node_index].scale = 1.0f;
		}
	}
}


/* samples the channel at the positive or the negative frame by a sway value
   (an lvalue, read again after the frame is set), scaled by the weight */
#define FIRST_PERSON_CHANNEL_SAMPLE_SWAY(channel, value, positive_frame, negative_frame, weight, node_count, orientations) \
	if ((value) > 0.0f) \
	{ \
		(channel).set_frame_position(positive_frame); \
		(channel).sample((value) * (weight), NULL, (node_count), (real_quaternion_transform *)(orientations)); \
	} \
	else if (0.0f > (value)) \
	{ \
		(channel).set_frame_position(negative_frame); \
		(channel).sample(0.0f - (value) * (weight), NULL, (node_count), (real_quaternion_transform *)(orientations)); \
	}

/* samples a weapon's animation, its overlays and its sway into the user's
   orientations, then builds the weapon's node matrices */
// @retail 0x16760c
void __stdcall function_16760c(long user_index, long field_x11c898)
{
	s_first_person_user *user = &first_person_users[user_index];
	s_first_person_weapon *weapon = &user->weapons[field_x11c898];
	long weapon_index = weapon->weapon_index;

	if (weapon_index != NONE)
	{
		s_16760c_weapon_object *weapon_object = (s_16760c_weapon_object *)first_person_object_get(weapon_index);
		s_16760c_weapon_definition *definition =
			(s_16760c_weapon_definition *)first_person_object_definition_get((s_first_person_object *)weapon_object);
		long interface_index = first_person_character_to_interface(user->character_index);
		s_16760c_render_model *arg_2f82bc = (s_16760c_render_model *)
			g_4e3b44[definition->interfaces[interface_index].render_model_index & 0xffff].bytes;
		s_16760c_render_model *arms_model = (s_16760c_render_model *)
			g_4e3b44[((s_first_person_globals_view *)g_4e034c)->representations[user->character_index].arms_render_model_index & 0xffff].bytes;
		s_animation_state *state = &weapon->animation;
		c_animation_channel channel;
		byte *orientations;
		transform4x3f root;
		long node_index;

		if (weapon->weapon_model_index != arg_2f82bc->node_count || weapon->arms_model_index != arms_model->node_count)
		{
			function_167e86(user_index, field_x11c898);
		}
		weapon->orientation_count = state->node_count_get();
		orientations = first_person_orientations + (field_x11c898 + user_index * MAXIMUM_FIRST_PERSON_WEAPONS) * 0x1000;
		for (node_index = 0; node_index < weapon->orientation_count; node_index++)
		{
			((s_16760c_orientation *)orientations)[node_index] = *(s_16760c_orientation *)g_4687d8;
		}
		first_person_orientations_from_model_16760c(arg_2f82bc, weapon->weapon_node_map, (s_16760c_orientation *)orientations);
		first_person_orientations_from_model_16760c(arms_model, weapon->arms_node_map, (s_16760c_orientation *)orientations);

		if (function_0b6780((s_index_triple *)state))
		{
			bool full_blend = true;
			s_animation *animation;
			real blend_time;

			state->sample(0, 1.0f, NULL, (real_quaternion_transform *)orientations, 0, 0, weapon->orientation_count);
			function_bd970(weapon_index, arg_2f82bc, state, 0, weapon->orientation_count, orientations);
			animation = state->channels[0].function_1c6440();
			if (animation && (((byte *)animation)[0x18] & 0x10))
				full_blend = false;
			blend_time = full_blend ? 0.4f : 0.2f;
			function_1d9240((s_1d9240 *)&user->unknown202c, full_blend, blend_time);
			if (user->unknown202c.unknown1)
			{
				real weight = function_1d9370((s_1d9240 *)&user->unknown202c);

				if (weight > 0.0001f)
				{
					bool aimed;

					if (function_0b6760((s_index_pair *)&weapon->channel98))
					{
						weapon->channel98.sample(weight, NULL, weapon->orientation_count, (real_quaternion_transform *)orientations);
					}
					if (function_0b6760((s_index_pair *)&weapon->channelb8))
					{
						weapon->channelb8.sample(weight * (weapon_object->unknown188 + 0.5f), NULL, weapon->orientation_count,
							(real_quaternion_transform *)orientations);
					}
					aimed = false;
					if (weapon->indices.unknown2 != NONE &&
						state->channel_play(&channel, *(c_type_709360 *)&weapon->indices.unknown0, 0))
					{
						channel.sample_aiming(*(real *)&user->unknown2030[0x10], *(real *)&user->unknown2030[0x24], weight, NULL,
							weapon->orientation_count, (real_quaternion_transform *)orientations);
						aimed = true;
					}
					if (weapon->indices.unknown6 != NONE &&
						state->channel_play(&channel, *(c_type_709360 *)&weapon->indices.unknown4, 0))
					{
						if (channel.function_1c6440()->frame_count >= 9)
						{
							FIRST_PERSON_CHANNEL_SAMPLE_SWAY(channel, *(real const *)&user->unknown2030[0x0], 0.0f, 1.0f, weight, weapon->orientation_count, orientations);
							FIRST_PERSON_CHANNEL_SAMPLE_SWAY(channel, *(real const *)&user->unknown2030[0x4], 3.0f, 2.0f, weight, weapon->orientation_count, orientations);
							if (!aimed)
							{
								FIRST_PERSON_CHANNEL_SAMPLE_SWAY(channel, *(real const *)&user->unknown2030[0x10], 4.0f, 5.0f, weight, weapon->orientation_count, orientations);
								FIRST_PERSON_CHANNEL_SAMPLE_SWAY(channel, *(real const *)&user->unknown2030[0x14], 7.0f, 6.0f, weight, weapon->orientation_count, orientations);
							}
							if (weapon->indices.unknownc > 0.0f)
							{
								channel.set_frame_position(8.0f);
								channel.sample(weapon->indices.unknownc * weight, NULL, weapon->orientation_count,
									(real_quaternion_transform *)orientations);
							}
						}
						else
						{
							weapon->indices.unknown4 = NONE;
							weapon->indices.unknown6 = NONE;
						}
					}
				}
			}
			if (weapon->indices.unknowna != NONE &&
				state->channel_play(&channel, *(c_type_709360 *)&weapon->indices.unknown8, 0))
			{
				s_animation *ammo_animation = channel.function_1c6440();
				long frame;

				if (definition->unknown292 == 2 &&
					(state->animation_name == 0xc000064 || state->animation_name == 0xb000065))
				{
					short rounds = weapon_object->unknown22c;
					short loaded = weapon_object->unknown228 - weapon_object->unknown226;

					if (loaded >= 0x2c)
					{
						real excess = (real)(loaded - 0x2c);
						real fraction = excess * 0.2f;
						s_16760c_magazine_definition *magazine = definition->magazines;
						short maximum;
						short total;

						if (fraction > 1.0f)
						{
							fraction = 1.0f;
						}
						total = (short)function_100b40(0, weapon_index, false);
						maximum = magazine->rounds_maximum;
						if (total > maximum)
						{
							total = maximum;
						}
						rounds += (long)((real)(total - rounds) * fraction);
					}
					frame = rounds;
				}
				else
				{
					short rounds = weapon_object->unknown22c;

					if (rounds < 0)
					{
						frame = 0;
					}
					else
					{
						frame = ammo_animation->frame_count - 1;
						if (rounds <= frame)
							frame = rounds;
					}
				}
				channel.set_frame_position((real)frame);
				channel.sample(1.0f, NULL, weapon->orientation_count, (real_quaternion_transform *)orientations);
			}
			if (TEST_FLAG(user->flags, _first_person_user_animated_bit) && state->unknown64.count)
			{
				state->orientations_blend((s_blend_orientation const *)(orientations + 0x800), (short)weapon->orientation_count,
					NULL, (s_blend_orientation *)orientations);
			}
		}

		SET_FLAG(user->flags, _first_person_user_animated_bit, true);
		function_1416c0(&definition->first_person_offset, &root);
		weapon->node_count = weapon->orientation_count;
		if (weapon->unknown2fc_node != NONE && weapon->unknown2fe_node != NONE)
		{
			state->nodes_compute_mirrored(weapon->nodes, (rigid_transform_scaled const *)orientations, &root,
				weapon->unknown2fc_node, weapon->unknown2fe_node);
		}
		else
		{
			state->nodes_compute(weapon->nodes, (rigid_transform_scaled const *)orientations, &root);
		}
		if (field_x11c898 == 0)
		{
			if (user->unknown2094 != NONE)
			{
				user->adjustment = weapon->nodes[user->unknown2094];
				user->adjustment.position.x -= definition->first_person_offset.x;
				user->adjustment.position.y -= definition->first_person_offset.y;
				user->adjustment.position.z -= definition->first_person_offset.z;
				SET_FLAG(user->flags, _first_person_user_adjusted_bit, true);
			}
			else
			{
				SET_FLAG(user->flags, _first_person_user_adjusted_bit, false);
			}
		}
	}
}

/* the unit's aiming blend (at the offset in +0x33e), as read here */
struct s_168644_unit
{
	byte unknown000[0x33e];
	short aiming_offset;
};

struct s_168644_aiming
{
	byte unknown00[0x80];
	s_1d9240 blend;
};

/* a render model's marker group (0xc bytes) and its markers */
struct s_168644_marker
{
	byte unknown00[2];
	byte node_index;
	byte unknown03;
	point3f position;
	quaternionf rotation;
};

struct s_168644_marker_group
{
	long name;
	long marker_count;
	s_168644_marker *markers;
};

struct s_168644_render_model
{
	byte unknown00[0x48];
	long node_count;
	byte unknown4c[0x5c - 0x4c];
	s_168644_marker_group *marker_groups;
};

bool function_cd660(long unit_index);
long function_1d8f00(long render_model_index, long marker_name);
void function_1d90e0(long render_model_index, long node_index, transform4x3f *nodes, transform4x3f const *marker_matrix,
	transform4x3f const *target_matrix, real weight, long node_count);
void function_1421b0(transform4x3f *out, point3f const *position, quaternionf const *rotation);
void function_1dd7a0(long *reference);

/* attaches the arms to the weapon's markers while the unit's aiming blends */
// @retail 0x168644
void function_168644(long user_index, s_first_person_model *arms, s_first_person_model *arg_2f82bc)
{
	s_first_person_user *user = &first_person_users[user_index];
	long unit_index = user->unit_index;

	if (unit_index != NONE && TEST_FLAG(user->weapons[0].flags, _first_person_weapon_active_bit) &&
		!TEST_FLAG(user->weapons[1].flags, _first_person_weapon_active_bit) && user->weapons[0].weapon_index != NONE &&
		!function_cd660(unit_index))
	{
		s_168644_unit *unit = (s_168644_unit *)first_person_object_get(unit_index);
		s_168644_aiming *aiming = (s_168644_aiming *)((byte *)unit + unit->aiming_offset);
		s_animation_state *state = &user->weapons[0].animation;

		if (function_0b6780((s_index_triple *)state) && aiming->blend.count)
		{
			real weight = function_1d9370(&aiming->blend);

			if (!(0.0001f > (real)fabs(weight)))
			{
				s_graph_pair_iterator iterator;

				function_1dd7a0((long *)&iterator);
				while (state->pairs_iterate(&iterator))
				{
					if (iterator.b != NONE && iterator.b != 0 && user->character_index >= 0 &&
						user->character_index < ((s_first_person_globals_view *)g_4e034c)->representation_count)
					{
						long arms_render_model_index =
							((s_first_person_globals_view *)g_4e034c)->representations[user->character_index].arms_render_model_index;
						long interface_index = first_person_character_to_interface(user->character_index);
						long weapon_render_model_index =
							first_person_object_definition_get(first_person_object_get(user->weapons[0].weapon_index))->
								interfaces[interface_index].render_model_index;

						if (arms_render_model_index != NONE && weapon_render_model_index != NONE)
						{
							s_168644_render_model *arms_definition =
								(s_168644_render_model *)g_4e3b44[arms_render_model_index & 0xffff].bytes;
							long arms_group_index = function_1d8f00(arms_render_model_index, iterator.a);
							long weapon_group_index = function_1d8f00(weapon_render_model_index, iterator.b);

							if (arms_group_index != NONE && weapon_group_index != NONE)
							{
								s_168644_marker_group *arms_group = &arms_definition->marker_groups[arms_group_index];
								s_168644_marker_group *weapon_group = &((s_168644_render_model *)
									g_4e3b44[weapon_render_model_index & 0xffff].bytes)->marker_groups[weapon_group_index];

								/* read once through the pointer: retail builds the arms group in the
								   index's register (ecx) and reads its count from there */
								long arms_marker_count = *(volatile long *)&arms_group->marker_count;

								if (arms_marker_count > 0 && weapon_group->marker_count > 0)
								{
									s_168644_marker *weapon_marker = weapon_group->markers;
									s_168644_marker *arms_marker = arms_group->markers;
									transform4x3f arms_matrix;
									transform4x3f weapon_matrix;

									function_1421b0(&arms_matrix, &arms_marker->position, &arms_marker->rotation);
									function_1421b0(&weapon_matrix, &weapon_marker->position,
										&weapon_marker->rotation);
									function_142a60(&arg_2f82bc->nodes[weapon_marker->node_index], &weapon_matrix, &weapon_matrix);
									function_1d90e0(arms_render_model_index, arms_marker->node_index, arms->nodes, &arms_matrix,
										&weapon_matrix, weight, arms_definition->node_count);
								}
							}
						}
					}
				}
			}
		}
	}
}

transform4x3f *function_b8c00(long object_index, long *node_count);

/* the render models a user's first person view draws: the arms, each
   active weapon and the unit's own body; returns how many */
// score 9 (was 20): retail loads the representation table before the character index's sign
// test (ours after it), and the second weapon loop's call forms the nodes pointer first and the
// node map from it (lea edi, [edx - 0x20c]); ours forms each from the weapon. The flag test reads
// the hud message table through its accessor (retail's dword and), and `active` is a byte that
// reuses al. Tried before: the flag as a separate bool global with bool active = false (23); the
// reused local as char/byte, with `: 0`, with a (long) cast (all 236); `!= 0` of a long ternary
// (249); a nodes local in the second loop (20).
// Matched in a full build with stand-in SDK headers (was 9): the test is two nested ifs with the globals pointer
// declared between them (retail loads it before the jl), and the node map argument is formed from the nodes pointer
// (`nodes - (offset of nodes - offset of weapon_node_map)`), which gives retail's lea from the first lea's register.
// @retail 0x165f16
long __stdcall first_person_weapons_get_models(long user_index, long unit_index, long maximum_count,
	s_first_person_model *models)
{
	long count = 0;

	if (!function_2ac270_get(6) && user_index != NONE)
	{
		s_first_person_user *user = &first_person_users[user_index];
		bool active = false;
		bool armed = false;
		long field_x11c898;

		for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
		{
			s_first_person_weapon *weapon = &user->weapons[field_x11c898];

			active |= TEST_FLAG(weapon->flags, _first_person_weapon_active_bit);
			armed |= weapon->weapon_index != NONE;
		}
		if (active && armed && user->unit_index == unit_index && user->unit_index != NONE)
		{
			s_first_person_globals_view *globals = (s_first_person_globals_view *)g_4e034c;

			if (user->character_index >= 0 && user->character_index < globals->representation_count)
			{
				s_player_representation *representation = &globals->representations[user->character_index];
				long arms_render_model_index = representation->arms_render_model_index;
				long body_render_model_index = representation->arms_animation_graph_index;
				long unit_node_count = 0;
				transform4x3f *unit_nodes = NULL;
				long unit_render_model_index = NONE;
				transform4x3f base;
				transform4x3f adjustment;
				s_first_person_model *model;

				if (unit_index != NONE)
				{
					long model_index = *(long *)((byte *)first_person_object_definition_get(first_person_object_get(unit_index)) + 0x38);

					if (model_index != NONE)
					{
						long render_model_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 4);

						if (render_model_index != NONE)
						{
							unit_render_model_index = render_model_index;
							unit_nodes = function_b8c00(unit_index, &unit_node_count);
						}
					}
				}
				function_1420f0(&base, &g_4b9da0, &g_4b9dac, &g_4b9db8);
				if (TEST_FLAG(user->flags, _first_person_user_adjusted_bit))
				{
					function_141590(&user->adjustment, &adjustment);
					function_142a60(&base, &adjustment, &base);
				}
				user->matrix = base;
				count = 1;
				for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
				{
					s_first_person_weapon *weapon = &user->weapons[field_x11c898];

					if (TEST_FLAG(weapon->flags, _first_person_weapon_active_bit) && weapon->weapon_index != NONE &&
						maximum_count > 1 && TEST_FLAG(weapon->flags, _first_person_weapon_arms_animated_bit) &&
						arms_render_model_index != NONE)
					{
						first_person_model_build(arms_render_model_index, weapon->node_count, weapon->nodes, unit_index, 1, &base,
							weapon->arms_node_map, models);
					}
				}
				model = &models[1];
				for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
				{
					s_first_person_weapon *weapon = &user->weapons[field_x11c898];

					if (TEST_FLAG(weapon->flags, _first_person_weapon_active_bit) && weapon->weapon_index != NONE)
					{
						long interface_index = first_person_character_to_interface(user->character_index);
						long render_model_index = first_person_object_definition_get(first_person_object_get(weapon->weapon_index))->
							interfaces[interface_index].render_model_index;

						if (count < maximum_count && TEST_FLAG(weapon->flags, _first_person_weapon_animated_bit) &&
							render_model_index != NONE)
						{
							transform4x3f *nodes = weapon->nodes;

							first_person_model_build(render_model_index, NONE, nodes, weapon->weapon_index, 1, &base,
								(long *)((byte *)nodes - ((byte *)weapon->nodes - (byte *)weapon->weapon_node_map)), model);
							count++;
							model++;
						}
					}
				}
				if (count < maximum_count && body_render_model_index != NONE && unit_render_model_index != NONE && unit_nodes)
				{
					s_first_person_render_model *unit_model =
						(s_first_person_render_model *)g_4e3b44[unit_render_model_index & 0xffff].bytes;
					s_first_person_render_model *body = (s_first_person_render_model *)g_4e3b44[body_render_model_index & 0xffff].bytes;

					if (body->node_count == unit_model->node_count &&
						*(long *)((byte *)body + 0x44) == *(long *)((byte *)unit_model + 0x44) &&
						(*(real *)((byte *)body + 0x70) == 0.0f || *(real *)((byte *)body + 0x70) > g_4b9db4))
					{
						first_person_model_build(body_render_model_index, unit_node_count, unit_nodes, unit_index, 0, NULL, NULL,
							&models[count]);
						count++;
					}
				}
				function_168644(user_index, models, &models[1]);
			}
		}
	}
	return count;
}
