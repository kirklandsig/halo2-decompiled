// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_165CE5.CPP: the first person weapons of the four local
   users (0x165ce5..0x1689a6, built for size; the same object as
   unknown_165cc3.cpp, unknown_16658d.cpp and lane R's unknown_166244.cpp).
   Each user holds the unit it views from and two weapons (the second for
   dual wielding); each weapon has its own animation state, two more
   channels, the node maps of the weapon and of the arms, and the node
   matrices last built for it. */

#include "unknown_11c920.h"
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
	bool channel_play(c_animation_channel *channel, c_type_709360 animation_id, word channel_flags);
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
	bool overlay_play(c_animation_channel *channel, word channel_flags, long set, long weapon_class, long weapon_type);
	c_type_709360 overlay_find(long set, long weapon_class, long weapon_type);
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

static inline void first_person_indices_reset(s_first_person_indices *indices)
{
	indices->unknown4 = NONE;
	indices->unknown6 = NONE;
	indices->unknown8 = NONE;
	indices->unknowna = NONE;
	indices->unknown0 = NONE;
	indices->unknown2 = NONE;
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
extern long g_4b9ed8;
extern point3f g_4b9da0;
extern vector3f g_4b9dac;
extern vector3f g_4b9db8;
real g_4b9db4;

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
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index);
void function_1d9240(s_1d9240 *p, char flag, real x);
long function_16658d(long group_index, long key);
real function_1d9430(s_1d9240 const *p);
long unit_get_player_index(long unit_index);
/* lane R's unknown_166244.cpp */
long function_166244(long key);

/* 0x1d9430 is in unknown_1d9240.cpp and 0x14de90 in unknown_14b560.cpp; 0x1cba80 is in
   unknown_1cafc0.cpp (state, weapon_class, weapon_type, set) */
real function_1d9430(s_1d9240 const *p);
s_animation const *function_1cba80(s_animation_state *state, long mode, long weapon_class, long name);
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
void first_person_nodes_remap(transform4x3f const *base, transform4x3f *out, volatile long out_count,
	transform4x3f const *nodes, long const *node_map, long render_model_index)
{
	s_first_person_render_model *model = (s_first_person_render_model *)g_4e3b44[render_model_index & 0xffff].bytes;
	long capacity = out_count;
	long count = MIN(model->node_count, capacity);
	long i;
	if (count <= 0)
		return;
	transform4x3f *destination = out;

	for (i = 0; i < count; i++, destination++)
	{
		long node_index = node_map[i];

		if (node_index != NONE)
		{
			if (base)
			{
				function_142a60(base, &nodes[node_index], destination);
			}
			else
			{
				*destination = nodes[node_index];
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

			weapon->sound_index = function_189060(weapon->weapon_index, NONE, 1.0f, g_468788, g_4687a8, sound_tag_index);
			weapon->sound_animation = (short)weapon->animation.animation_name;
		}
	}
}

// @retail 0x166d50
void __stdcall first_person_weapon_sound_event_right(long user_index, long unused, s_first_person_event const *event)
{
	first_person_weapon_sound_event(user_index, 0, event);
}

// @retail 0x166d62
void __stdcall first_person_weapon_sound_event_left(long user_index, long unused, s_first_person_event const *event)
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
						long mode = 0x7000101;
						long weapon_class = mode;

						if (function_cd660(weapon->unit_index))
						{
							weapon_class = 0x400054b;
						}
						result = function_1cba80(&state, mode, weapon_class, animation_name);
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
		long frame = event_frame;

		switch (type)
		{
		case 0:
			frame = frame_count;
			break;
		case 1:
			if (event_frame == NONE || frame_count < event_frame)
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
		s_player_representation *volatile representation = &globals->representations[user->character_index];
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
								short parent_index = graph->nodes[root_node_index].parent_index;

								if (parent_index != NONE)
								{
									long flags;
									short node_index;

									if (graph->nodes[parent_index].flags & 8)
									{
										flags = 0x10;
									}
									else
									{
										flags = 8;
									}
									node_index = (short)function_1dd490(graph, flags);

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
						state->overlay_play(&weapon->channelb8, 0x803f, 0x12000068, mode, 0x7000101);
						*(c_type_709360 *)&weapon->indices.unknown4 = state->overlay_find(0x8000061, mode, 0x7000101);
						*(c_type_709360 *)&weapon->indices.unknown8 = state->overlay_find(0xa000069, mode, 0x7000101);
						*(c_type_709360 *)&weapon->indices.unknown0 = state->overlay_find(0xe0000de, mode, 0x7000101);
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
bool function_11eed0(real *velocity, real *position, volatile real dt, bool wrap, real target, real a, real b, real lo, real hi);
bool function_c8a10(long unit_index);
bool function_d03b0(long unit_index);
void __stdcall function_16760c(long user_index, long field_x11c898);
real function_259d0(dword *seed, char const *file, long line, real lower_bound, real upper_bound);
bool function_1d9320(s_1d9240 *p);

point2f *g_4687c4;

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

/* the per frame update of a local user's first person weapons: the view's
   sway springs, the weapons' animations, zoom overlays and idles */
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
		real look_yaw;
		real look_pitch;
		point2f *look;
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
		function_11eed0((real *)&user->unknown2030[0x0], (real *)&user->unknown2030[0x8], g_510c54->rate, false,
			unit->aiming_velocity.i, spring_a, spring_b, -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x4], (real *)&user->unknown2030[0xc], g_510c54->rate, false,
			unit->aiming_velocity.j, spring_a, spring_b, -1.0f, 1.0f);
		look_yaw = *(real *)&user->unknown2030[0x28] / g_510c54->rate;
		look_yaw = PIN(look_yaw, -1.0f, 1.0f);
		look_pitch = -1.0f / g_510c54->rate * *(real *)&user->unknown2030[0x2c];
		look_pitch = PIN(look_pitch, -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x10], (real *)&user->unknown2030[0x18], g_510c54->rate, false,
			look_yaw, spring_c, spring_d, -1.0f, 1.0f);
		function_11eed0((real *)&user->unknown2030[0x14], (real *)&user->unknown2030[0x1c], g_510c54->rate, false,
			look_pitch, spring_c, spring_d, -1.0f, 1.0f);

		look = (point2f *)&g_4ed284->entries[user_index].yaw;
		flags = user->flags;
		if (TEST_FLAG(flags, _first_person_user_active_bit))
		{
			*(real *)&user->unknown2030[0x28] = function_0bff60(look->x, *(real *)&user->unknown2030[0x20]);
			*(real *)&user->unknown2030[0x2c] = function_0bff60(look->y, *(real *)&user->unknown2030[0x24]);
		}
		else
		{
			*(point2f *)&user->unknown2030[0x28] = *g_4687c4;
		}
		*(real *)&user->unknown2030[0x20] = look->x;
		*(real *)&user->unknown2030[0x24] = look->y;
		user->flags = flags | FLAG(_first_person_user_active_bit);

		for (field_x11c898 = 0; field_x11c898 < MAXIMUM_FIRST_PERSON_WEAPONS; field_x11c898++)
		{
			s_first_person_weapon *weapon = &user->weapons[field_x11c898];
			bool active;

			if (weapon->unknownf2 > 0)
			{
				weapon->unknownf2--;
			}
			active = false;
			if (user->unit_index != NONE && weapon->weapon_index != NONE)
			{
				active = !function_155760(user_index) && function_0b6780((s_index_triple *)&weapon->animation);
			}
			first_person_weapon_set_active(user_index, field_x11c898, active);

			if (weapon->weapon_index != NONE)
			{
				s_166d75_weapon_object *weapon_object = (s_166d75_weapon_object *)first_person_object_get(weapon->weapon_index);
				byte *orientations = first_person_orientations + (field_x11c898 + user_index * MAXIMUM_FIRST_PERSON_WEAPONS) * 0x1000;
				animation_event_callback callback = field_x11c898 ? (animation_event_callback)first_person_weapon_sound_event_left :
					(animation_event_callback)first_person_weapon_sound_event_right;
				real zoom;
				bool zoomed;
				bool scoped;
				c_animation_channel *channel;

				if (weapon->animation.update(callback, user_index, weapon->orientation_count,
					(s_blend_orientation *)(orientations + 0x800), (s_blend_orientation const *)orientations) &&
					(weapon->animation.channels[0].unknown11 & 0xa))
				{
					first_person_weapon_animation_finished(user_index, field_x11c898);
				}
				zoom = magnitude3d(&unit->aiming_velocity);
				if (zoom > 0.1f)
				{
					zoomed = true;
					scoped = function_c8a10(user->unit_index);
				}
				else
				{
					zoomed = false;
					scoped = false;
				}
				if (function_d03b0(user->unit_index))
				{
					zoomed = false;
					scoped = false;
				}
				if (scoped)
				{
					c_type_709360 animation_id = weapon->animation.overlay_get(0x60000d8);

					if (animation_id.index == NONE)
					{
						scoped = false;
					}
				}
				channel = &weapon->channel98;
				if (function_0b6760((s_index_pair *)channel))
				{
					if (zoomed)
					{
						if (!scoped)
						{
							channel->rate = PIN(zoom, 0.5f, 1.0f);
						}
						else
						{
							channel->rate = 1.0f;
						}
					}
					else if (scoped)
					{
						channel->rate = 1.0f;
					}
					weapon->animation.channel_update(channel, NULL, 0);
					if (!zoomed && !scoped ||
						scoped && channel->unknown08 != 0x60000d8 ||
						zoomed && !scoped && channel->unknown08 == 0x60000d8)
					{
						channel->clear();
						if (!zoomed && !scoped)
						{
							if (weapon->animation.animation_name != 0x400000c)
							{
								goto overlay;
							}
							first_person_weapon_save_orientations(user_index, field_x11c898, 0.267f);
						}
					}
				}
				if ((zoomed || scoped) && !function_0b6760((s_index_pair *)channel))
				{
					long set;
					word channel_flags;

					if (scoped)
					{
						channel_flags = 0x3f;
						set = 0x60000d8;
					}
					else
					{
						channel_flags = 0x803f;
						set = 0x6000060;
					}
					weapon->animation.overlay_play(channel, channel_flags, set, mode, 0x7000101);
					first_person_weapon_save_orientations(user_index, field_x11c898, 0.1335f);
				}
overlay:
				channel = &weapon->channelb8;
				if (function_0b6760((s_index_pair *)channel))
				{
					if (weapon->animation.animation_name == 0xb000067)
					{
						real frame;

						weapon->animation.overlay_play(&weapon->channelb8, 0x803f, 0x12000068, mode, 0x7000101);
						frame = (weapon_object->unknown188 + 1.0f) * g_510c54->rate * 2.0f +
							weapon->channelb8.frame_position * (1.0f / 30.0f);
						weapon->channelb8.set_frame_position((real)fmod(frame, weapon->channelb8.get_duration()) * 30.0f);
					}
					else
					{
						channel->clear();
					}
				}

				function_11eed0(&weapon->indices.unknownc, &weapon->indices.unknown10, g_510c54->rate, false, 0.0f, 9.0f,
					6.0f, 0.0f, 1.0f);
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

static __forceinline void first_person_orientations_from_model(s_16760c_render_model const *render_model, long const *node_map,
	s_16760c_orientation *orientations)
{
	short node_index;

	for (node_index = 0; node_index < render_model->node_count; node_index++)
	{
		short graph_node_index = (short)node_map[node_index];
		s_16760c_render_model_node const *node = &render_model->nodes[node_index];

		if (graph_node_index != NONE)
		{
			s_16760c_orientation *orientation = &orientations[graph_node_index];

			orientation->rotation = node->default_rotation;
			orientation->position = node->default_translation;
			orientation->scale = 1.0f;
		}
	}
}

static __forceinline void first_person_channel_sample_sway(c_animation_channel *channel, real value, real positive_frame,
	real negative_frame, real weight, long node_count, byte *orientations)
{
	if (value > 0.0f)
	{
		channel->set_frame_position(positive_frame);
		channel->sample(weight * value, NULL, node_count, (real_quaternion_transform *)orientations);
	}
	else if (0.0f > value)
	{
		channel->set_frame_position(negative_frame);
		channel->sample(0.0f - weight * value, NULL, node_count, (real_quaternion_transform *)orientations);
	}
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
		first_person_orientations_from_model(arg_2f82bc, weapon->weapon_node_map, (s_16760c_orientation *)orientations);
		first_person_orientations_from_model(arms_model, weapon->arms_node_map, (s_16760c_orientation *)orientations);

		if (function_0b6780((s_index_triple *)state))
		{
			bool full_blend = true;
			s_animation *animation;
			real blend_time;

			state->sample(0, 1.0f, NULL, (real_quaternion_transform *)orientations, 0, 0, weapon->orientation_count);
			function_bd970(weapon_index, arg_2f82bc, state, 0, weapon->orientation_count, orientations);
			animation = state->channels[0].function_1c6440();
			if (animation && (((byte *)animation)[0x18] & 0x10))
			{
				blend_time = 0.2f;
				full_blend = false;
			}
			else
			{
				blend_time = 0.4f;
			}
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
						weapon->channelb8.sample((weapon_object->unknown188 + 0.5f) * weight, NULL, weapon->orientation_count,
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
						if (channel.function_1c6440()->frame_count < 9)
						{
							weapon->indices.unknown4 = NONE;
							weapon->indices.unknown6 = NONE;
						}
						else
						{
							first_person_channel_sample_sway(&channel, *(real *)&user->unknown2030[0x0], 0.0f, 1.0f, weight,
								weapon->orientation_count, orientations);
							first_person_channel_sample_sway(&channel, *(real *)&user->unknown2030[0x4], 3.0f, 2.0f, weight,
								weapon->orientation_count, orientations);
							if (!aimed)
							{
								first_person_channel_sample_sway(&channel, *(real *)&user->unknown2030[0x10], 4.0f, 5.0f, weight,
									weapon->orientation_count, orientations);
								first_person_channel_sample_sway(&channel, *(real *)&user->unknown2030[0x14], 7.0f, 6.0f, weight,
									weapon->orientation_count, orientations);
							}
							if (weapon->indices.unknownc > 0.0f)
							{
								channel.set_frame_position(8.0f);
								channel.sample(weapon->indices.unknownc * weight, NULL, weapon->orientation_count,
									(real_quaternion_transform *)orientations);
							}
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
						real fraction = (real)(loaded - 0x2c) * 0.2f;
						short maximum;
						short total;

						if (fraction > 1.0f)
						{
							fraction = 1.0f;
						}
						total = (short)function_100b40(0, weapon_index, false);
						maximum = definition->magazines->rounds_maximum;
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
						{
							frame = rounds;
						}
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
				SET_FLAG(user->flags, _first_person_user_adjusted_bit, true);
				user->adjustment.position.z -= definition->first_person_offset.z;
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
void function_1d90e0(long render_model_index, transform4x3f *nodes, long node_index, transform4x3f const *marker_matrix,
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

								if (arms_group->marker_count > 0 && weapon_group->marker_count > 0)
								{
									s_168644_marker *weapon_marker = weapon_group->markers;
									s_168644_marker *arms_marker = arms_group->markers;
									transform4x3f arms_matrix;
									transform4x3f weapon_matrix;

									function_1421b0(&arms_matrix, &arms_marker->position, &arms_marker->rotation);
									function_1421b0(&weapon_matrix, &weapon_marker->position,
										&weapon_marker->rotation);
									function_142a60(&arg_2f82bc->nodes[weapon_marker->node_index], &weapon_matrix, &weapon_matrix);
									function_1d90e0(arms_render_model_index, arms->nodes, arms_marker->node_index, &arms_matrix,
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
extern bool g_4f55e2;

/* the render models a user's first person view draws: the arms, each
   active weapon and the unit's own body; returns how many */
// @retail 0x165f16
long __stdcall first_person_weapons_get_models(long user_index, long unit_index, long maximum_count,
	s_first_person_model *models)
{
	long count = 0;

	if (!(g_4e6948->state == 1 ? g_4f55e2 : false) && user_index != NONE)
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
		if (active && armed && user->unit_index == unit_index && unit_index != NONE && user->character_index >= 0 &&
			user->character_index < ((s_first_person_globals_view *)g_4e034c)->representation_count)
		{
			s_player_representation *representation =
				&((s_first_person_globals_view *)g_4e034c)->representations[user->character_index];
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
						first_person_model_build(render_model_index, NONE, weapon->nodes, weapon->weapon_index, 1, &base,
							weapon->weapon_node_map, model);
						count++;
						model++;
					}
				}
			}
			if (count < maximum_count && body_render_model_index != NONE && unit_render_model_index != NONE && unit_nodes)
			{
				s_first_person_render_model *body = (s_first_person_render_model *)g_4e3b44[body_render_model_index & 0xffff].bytes;
				s_first_person_render_model *unit_model =
					(s_first_person_render_model *)g_4e3b44[unit_render_model_index & 0xffff].bytes;

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
	return count;
}
