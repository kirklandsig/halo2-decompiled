// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1CAFC0.CPP: the object that drives three animation channels of
   one graph (0x1cafc0..0x1ce1e0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1c62f0.h"
#include "unknown_123680.h"
#include "unknown_1cafc0.h"
#include "unknown_11cb00.h"
#include <math.h>
#include <string.h>

/* an entry of the graph's weapon block at +0x54 (0x1dd560 finds one by name) */
struct s_graph_name_entry
{
	long name;
	long weapon_class;
};

#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

/* callees outside this file */
void function_279d80(s_graph_tag *graph, c_type_709360 animation_id, long node_count, real frame, real weight,
	s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms, bool interpolate);
matrix3x3 *function_141e10(matrix3x3 *out, quaternionf const *q);
c_type_709360 function_1dd0b0(s_graph_tag *graph, long name);
void function_1dd1c0(s_graph_tag *graph, transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root);

/* the blend counters (unknown_1d9240.cpp) */
struct s_1d9240;
void function_1d9240(s_1d9240 *p, char flag, real x);
real function_1d9430(s_1d9240 const *p);
void function_1d9470(s_1d9240 const *p, s_blend_orientation const *targets, long count, dword const *mask,
	s_blend_orientation *orientations);

bool g_46fbf4 = true;
bool g_46fbf5 = true;

inline bool channel_valid(c_animation_channel const *channel)
{
	return channel->graph_tag_index != NONE && channel->animation_id.index != NONE;
}

// @retail 0x1cafc0
void s_animation_state::reset()
{
	long i;

	for (i = 0; i < 3; i++)
	{
		channels[i].reset();
	}
	flags = 0;
	unknown6e = 0;
	graph_tag_index = NONE;
	unknown64.unknown1 = 0;
	unknown64.unknown0 = 0;
	unknown64.unknown3 = 0;
	unknown60.unknown1 = 0;
	unknown60.unknown0 = 0;
	unknown60.unknown3 = 0;
	unknown70 = NONE;
	unknown7c = NONE;
	unknown74 = NONE;
	unknown78 = NONE;
	unknown80 = 0.0f;
}

// @retail 0x1caed0
s_animation_state::s_animation_state()
{
	reset();
}

// @retail 0x1cb0a0
void s_animation_state::channels_clear_partial()
{
	c_animation_channel *channel = &channels[3];
	long i = 3;

	do
	{
		channel--;
		i--;
		channel->graph_tag_index = NONE;
		channel->animation_id.graph_index = NONE;
		channel->animation_id.index = NONE;
		channel->unknown08 = NONE;
		channel->unknown10 = 0;
	}
	while (i);
}

// @retail 0x1cbd30
short s_animation_state::node_count_get()
{
	return (short)graph_get()->node_count;
}

// @retail 0x1cbde0
long s_animation_state::node_find(long name)
{
	s_graph_tag *graph = graph_get();
	long i;

	for (i = 0; i < graph->node_count; i++)
	{
		if (graph->nodes[i].name == name)
		{
			return i;
		}
	}
	return NONE;
}

// @retail 0x1cbe20
s_graph_entry *s_animation_state::entry_get(long index)
{
	if (index == NONE)
	{
		return NULL;
	}
	return &graph_get()->entries[index];
}

// @retail 0x1cbe90
long s_animation_state::node_count()
{
	long count = 0;
	s_graph_tag *graph = graph_get();

	if (graph)
	{
		count = (short)graph->node_count;
	}
	return count;
}

// @retail 0x1cc2d0
s_graph_tag *s_animation_state::graph_get()
{
	return graph_tag_get(graph_tag_index);
}

// @retail 0x1cb680
void s_animation_state::update_blend_flags()
{
	dword flags0 = channels[0].flags;
	dword flags1 = channels[1].flags;

	if (!g_46fbf4 || !channel_valid(&channels[1]) || 0.45f > unknown80)
	{
		flags &= ~0x20;
		flags0 &= ~0x380;
		flags1 |= 0x380;
	}
	else if (unknown80 > 0.55f)
	{
		flags |= 0x20;
		flags0 |= 0x380;
		flags1 &= ~0x380;
	}
	else
	{
		return;
	}
	if (channel_valid(&channels[0]))
	{
		channels[0].flags = (word)flags0;
	}
	if (channel_valid(&channels[1]))
	{
		channels[1].flags = (word)flags1;
	}
}

// @retail 0x1ccba0
void s_animation_state::channels_clear()
{
	long i;

	for (i = 0; i < 3; i++)
	{
		channels[i].clear();
	}
}

// @retail 0x1ccc50
void s_animation_state::secondary_channels_clear()
{
	long i;

	for (i = 1; i < 3; i++)
	{
		channels[i].clear();
	}
}

// @retail 0x1cd510
c_type_709360 *s_animation_state::current_animation_get(c_type_709360 *result)
{
	c_type_709360 none;

	if (g_46fbf5 && channel_valid(&channels[2]))
	{
		none = channels[2].animation_id;
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 >= 0.9999f)
	{
		none = channels[1].animation_id;
	}
	else if (channel_valid(&channels[0]))
	{
		none = channels[0].animation_id;
	}
	*result = none;
	return result;
}

PRIVATE __forceinline real blend_fraction(real blend)
{
	real fraction = PIN(blend + 0.0001f, 0.0f, 0.9999f) * 1.0002f;

	return PIN(fraction, 0.0f, 1.0f);
}

// @retail 0x1cdf00
real s_animation_state::blend_fraction_get()
{
	return blend_fraction(unknown80);
}

// @retail 0x1cc2f0
void s_animation_state::names_resolve(s_animation_names *names, long mode, long weapon_class, long weapon_type, long set)
{
	s_graph_name_entry *entry;
	long resolved_weapon_class;

	names->mode = mode;
	names->weapon_class = weapon_class;
	names->weapon_type = weapon_type;
	names->set = set;
	if (mode == 0x7000101)
	{
		names->mode = unknown70;
		if (names->mode == NONE)
		{
			names->mode = 0x6000086;
		}
	}
	if (names->mode == 0x7000001)
	{
		names->mode = 0x6000086;
	}
	if (weapon_class == 0x7000101)
	{
		names->weapon_class = unknown74;
	}
	if (names->weapon_class == 0x7000001 || names->weapon_class == NONE)
	{
		names->weapon_class = 0x7000083;
	}
	if (weapon_type == 0x7000101)
	{
		names->weapon_type = unknown78;
	}
	if (names->weapon_type == 0x7000001 || names->weapon_type == NONE)
	{
		names->weapon_type = 0x30000d9;
	}
	resolved_weapon_class = names->weapon_class;
	entry = (s_graph_name_entry *)function_1dd560((s_sorted_array *)&graph_get()->weapons, names->weapon_type, sizeof(s_graph_name_entry));
	if (entry && entry->weapon_class != NONE && entry->weapon_class != 0x30000d9 && resolved_weapon_class != 0x400054b)
	{
		resolved_weapon_class = entry->weapon_class;
	}
	names->weapon_class = resolved_weapon_class;
	if (set == 0x7000101)
	{
		names->set = unknown7c;
		if (names->set == NONE)
		{
			names->set = 0x400000c;
		}
	}
	if (names->set == 0x7000001)
	{
		names->set = 0x400000c;
	}
}

// @retail 0x1ce1e0
void s_animation_state::animation_touch(c_type_709360 animation_id)
{
	if (graph_tag_index != NONE)
	{
		function_1dd9d0(graph_tag_get(graph_tag_index), animation_id);
	}
}

// @retail 0x1ccb80
void function_1ccb80(c_animation_channel *channel, real seconds)
{
	if (channel_valid(channel))
	{
		channel->set_frame_position(seconds * 30.0f);
	}
}

// @retail 0x1cbdb0
bool s_animation_state::node_map_build(long render_model_tag_index, long *node_count, long *node_map)
{
	return function_1dd4c0(render_model_tag_index, graph_tag_get(graph_tag_index), node_count, node_map);
}

// @retail 0x1cb5b0
bool s_animation_state::channel_update(c_animation_channel *channel, animation_event_callback callback, long user)
{
	bool result = false;

	if (graph_tag_index != NONE && channel_valid(channel))
	{
		if (!(flags & 1))
		{
			channel->update(this, callback, user);
		}
		result = true;
	}
	return result;
}

// @retail 0x1cbe50
s_graph_inheritance *s_animation_state::inheritance_get(c_type_709360 animation_id)
{
	s_graph_inheritance *result = NULL;

	if (animation_id.index != NONE && animation_id.graph_index != NONE)
	{
		result = function_1daff0(graph_tag_get(*(volatile long const *)&graph_tag_index), animation_id);
	}
	return result;
}

// @retail 0x1ccb40
real function_1ccb40(c_animation_channel const *channel)
{
	real result = 0.0f;

	if (channel_valid(channel))
	{
		result = (real)channel->function_1c6440()->frame_count * (1.0f / 30.0f);
	}
	return result;
}

// @retail 0x1cb3b0
c_type_709360 s_animation_state::variant_get(c_type_709360 animation_id)
{
	c_type_709360 result = animation_id;

	if (result.index != NONE)
	{
		s_graph_tag *graph = graph_tag_get(graph_tag_index);

		if (graph)
		{
			result = *function_1dd630(graph, &animation_id, result, (bool)(((dword)(short)flags >> 1) & 1));
		}
	}
	return result;
}

// @retail 0x1cb410
bool s_animation_state::channel_start(c_animation_channel *channel, c_type_709360 animation_id, long unknown08,
	char unknown0c, char unknown0d, char unknown0e, word channel_flags)
{
	bool result = false;

	if (graph_tag_index != NONE)
	{
		if (channel_flags & 0x10)
		{
			animation_id = variant_get(animation_id);
		}
		long local_0 = graph_tag_index;
		if (channel->set(local_0, channel_flags, animation_id, unknown08, unknown0c, unknown0d, unknown0e))
		{
			if (channel_flags & 4)
			{
				channel->set_frame_position(0.0f);
			}
			if (channel_flags & 8)
			{
				channel->rate = 1.0f;
			}
			result = true;
		}
	}
	return result;
}

// @retail 0x1ce110
void s_animation_state::translation_apply(real_orientation_1ce110 *orientation, real scale)
{
	if (unknown6e & 2)
	{
		c_animation_channel *channel = &channels[2];

		if (channel_valid(channel))
		{
			real fraction = channel->get_frame_ratio() * scale;

			point3f *translation = &orientation->translation;
			real local_0 = *(volatile real const *)&unknown84.i;
			real local_1 = *(volatile real const *)&unknown84.j;
			real local_2 = *(volatile real const *)&unknown84.k;
			real x = translation->x + local_0 * fraction;
			real y = translation->y + local_1 * fraction;
			real z = translation->z + local_2 * fraction;

			translation->x = x;
			translation->y = y;
			translation->z = z;
		}
	}
}

// @retail 0x1ce180
void s_animation_state::resources_request(long mode, long weapon_class, long weapon_type, bool urgent, bool other)
{
	if (graph_tag_index != NONE)
	{
		s_animation_names names;

		names_resolve(&names, mode, weapon_class, weapon_type, 0x7000101);
		function_1ddd00(graph_tag_get(graph_tag_index), names.mode, names.weapon_class, names.weapon_type, urgent, other);
	}
}

// @retail 0x1cdf50
void s_animation_state::channels_finish()
{
	c_animation_channel *local_0 = &channels[2];
	*(volatile long *)&local_0->graph_tag_index = NONE;
	volatile short *local_3 = (volatile short *)&local_0->animation_id;
	local_3[0] = NONE;
	local_3[1] = NONE;
	long local_2 = 0;
	*(volatile real *)&local_0->frame_position = 0.0f;
	local_0->unknown10 = 0;
	local_0->unknown11 = 0;
	local_0->flags = (word)local_2;
	local_0->rate = 1.0f;
	local_0->unknown14 = (short)local_2;
	local_0->unknown16 = (short)local_2;
	local_0->unknown08 = NONE;
	local_0->unknown0c = NONE;
	local_0->unknown0d = NONE;
	local_0->unknown0e = NONE;
	if (channel_valid(&channels[0]))
	{
		channels[0].set_frame_last();
	}
	if (channel_valid(&channels[1]))
	{
		channels[1].set_frame_last();
	}
	unknown64.unknown1 = 0;
	unknown64.unknown0 = 0;
	unknown64.unknown3 = 0;
	if (channel_valid(&channels[2]) && (channels[2].flags & 1))
	{
		channels[2].unknown11 |= 1;
	}
	if (channel_valid(&channels[0]) && (channels[0].flags & 1))
	{
		channels[0].unknown11 |= 1;
	}
	c_animation_channel *local_1 = &channels[1];
	if (channel_valid(local_1) && (local_1->flags & 1))
	{
		local_1->unknown11 |= 1;
	}
}

/* whether the graph has the mode; if so, requests its resources (lane B's
   units.cpp declares it with a void pointer) */
// @retail 0x1cb920
bool function_1cb920(void *data, long mode)
{
	s_animation_state *state = (s_animation_state *)data;
	bool result = false;

	if (state->graph_tag_index != NONE)
	{
		bool found = function_1dd560((s_sorted_array *)&state->graph_get()->mode_count, mode, 0x14) != NULL;

		if (found)
		{
			state->resources_request(mode, 0x7000101, 0x7000101, true, false);
		}
		result = found;
	}
	return result;
}

#define DEFAULT_WEAPON_NAME 0x30000d9

/* requests the urgent resources of a mode, weapon class and weapon type */
PRIVATE __forceinline void graph_weapon_type_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type)
{
	s_graph_weapon_type *animations = (s_graph_weapon_type *)graph_weapon_type_get(graph, mode, weapon_class, weapon_type);

	if (animations)
	{
		long i;

		for (i = 0; i < animations->urgent_resource_count; i++)
		{
			s_cache_resource *resource = &graph->resources[animations->urgent_resources[i]];

			if (resource->streamed)
			{
				function_1236f0(resource, true);
			}
		}
	}
}

PRIVATE __forceinline void graph_weapon_types_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type)
{
	graph_weapon_type_request(graph, mode, weapon_class, weapon_type);
	graph_weapon_type_request(graph, mode, weapon_class, DEFAULT_WEAPON_NAME);
	graph_weapon_type_request(graph, mode, DEFAULT_WEAPON_NAME, weapon_type);
	graph_weapon_type_request(graph, mode, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME);
	graph_weapon_type_request(graph, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME);
}

// @retail 0x1cc3f0
bool s_animation_state::animation_lookup(s_animation_names *names, s_animation_names *found, long mode, long weapon_class,
	long weapon_type, long set, long lookup_flags, c_type_709360 *result)
{
	s_graph_tag *graph = graph_get();
	bool success = false;
	c_type_709360 animation_id;

	names_resolve(names, mode, weapon_class, weapon_type, set);
	*found = *names;
	*result = graph->animation_get(names->mode, names->weapon_class, names->weapon_type, names->set,
		&found->mode, &found->weapon_class, &found->weapon_type);
	if (result->index != NONE)
	{
		success = true;
	}
	else
	{
		if (lookup_flags & 2)
		{
			*result = graph->animation_get(names->mode, names->weapon_class, names->weapon_type, 0x400000c,
				&found->mode, &found->weapon_class, &found->weapon_type);
			if (result->index != NONE)
			{
				found->set = 0x400000c;
				success = true;
			}
		}
		if (result->index == NONE && (lookup_flags & 4))
		{
			long names_weapon_type = names->weapon_type;
			long names_weapon_class = names->weapon_class;
			long names_mode = names->mode;

			if (function_1db120(graph, names_mode, names_weapon_class, names_weapon_type))
			{
				success = true;
			}
			else if (function_1db120(graph, names_mode, names_weapon_class, DEFAULT_WEAPON_NAME))
			{
				success = true;
				found->weapon_type = DEFAULT_WEAPON_NAME;
			}
			else if (function_1db120(graph, names_mode, DEFAULT_WEAPON_NAME, names_weapon_type))
			{
				success = true;
				found->weapon_class = DEFAULT_WEAPON_NAME;
			}
			else if (function_1db120(graph, names_mode, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME))
			{
				success = true;
				found->weapon_class = DEFAULT_WEAPON_NAME;
				found->weapon_type = DEFAULT_WEAPON_NAME;
			}
			else if (function_1db120(graph, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_NAME))
			{
				success = true;
				found->mode = DEFAULT_WEAPON_NAME;
				found->weapon_class = DEFAULT_WEAPON_NAME;
				found->weapon_type = DEFAULT_WEAPON_NAME;
			}
		}
	}
	if (success)
	{
		long found_mode = found->mode;
		long found_weapon_class = found->weapon_class;
		long found_weapon_type = found->weapon_type;
		long i;

		graph_weapon_types_request(graph, found_mode, found_weapon_class, found_weapon_type);
		for (i = 0; i < graph->inheritance_count; i++)
		{
			graph_weapon_types_request(graph_inherited_get(graph, i), found_mode, found_weapon_class, found_weapon_type);
		}
	}
	return success;
}

/* the transition sets of four animation sets */
struct s_transition_set
{
	long set;
	long transition_set;
};

s_transition_set const g_46fbf8[4] =
{
	{ 0xa000014, 0xa000521 },
	{ 0x9000015, 0x9000522 },
	{ 0x9000016, 0x9000523 },
	{ 0xa000017, 0xa000524 },
};

PRIVATE __forceinline long transition_set_get(long set)
{
	long i;

	for (i = 0; i < 4; i++)
	{
		if (set == g_46fbf8[i].set)
		{
			return g_46fbf8[i].transition_set;
		}
	}
	return NONE;
}

// @retail 0x1cccd0
c_type_709360 *s_animation_state::transition_lookup(c_type_709360 *result, long set, long mode, long weapon_class,
	long weapon_type, long *transition_set)
{
	c_type_709360 animation_id;
	long found_set = transition_set_get(set);

	*transition_set = NONE;
	if (g_46fbf4 && found_set != NONE)
	{
		s_animation_names found;
		s_animation_names names;

		if (animation_lookup(&names, &found, mode, weapon_class, weapon_type, found_set, 0, &animation_id))
		{
			*transition_set = found_set;
		}
		else
		{
			animation_id.graph_index = NONE;
			animation_id.index = NONE;
		}
	}
	*result = animation_id;
	return result;
}

/* the frame event type that marks where a transition of one kind may start */
PRIVATE inline long transition_event_type(long kind)
{
	long type = 5;

	if (kind == 1)
	{
		type = 5;
	}
	else if (kind == 2)
	{
		type = 6;
	}
	else if (kind == 3)
	{
		type = 7;
	}
	else if (kind == 4)
	{
		type = 8;
	}
	return type;
}

/* function_1dade0's search, which retail inlines here */
PRIVATE inline short animation_event_next(s_animation const *animation, long type, long frame)
{
	long i;

	for (i = 0; i < animation->event_count; i++)
	{
		s_animation_event const *event = &animation->events[i];

		if (event->type == type && event->frame > frame)
		{
			return event->frame;
		}
	}
	return NONE;
}

// @retail 0x1ccda0
c_type_709360 *s_animation_state::transition_find(c_type_709360 *result, long mode, long set, bool *blend)
{
	c_type_709360 none;
	c_type_709360 local_0;

	*blend = false;
	if (g_46fbf5)
	{
		long name = unknown7c;
		real fraction = blend_fraction(unknown80);

		if (g_46fbf4 && channel_valid(&channels[1]) && fraction > 0.55f)
		{
			long transition_set = transition_set_get(name);

			if (transition_set != NONE)
			{
				name = transition_set;
			}
		}
		c_type_709360 transition = graph_get()->transition_find(unknown70, unknown74, unknown78, name, NONE, NONE, mode, set,
			NONE, NONE, 0);
		c_type_709360 event_transition;
		s_animation const *animation = channels[0].function_1c6440();
		bool found = false;

		if (animation)
		{
			long frame = real_truncate(channels[0].frame_position);
			long kind;

			for (kind = 1; kind <= 4; kind++)
			{
				c_type_709360 id = graph_get()->transition_find(unknown70, unknown74, unknown78, unknown7c, NONE, NONE,
					mode, set, NONE, NONE, (char)kind);

				if (id.index != NONE)
				{
					long type = transition_event_type(kind);
					long event_frame = function_1dade0(animation, type, NONE);

					while (event_frame != NONE && event_transition.index == NONE)
					{
						long delta = frame - event_frame;

						found = true;
						if (delta < 1 && delta > -3)
						{
							event_transition = id;
						}
						else
						{
							event_frame = animation_event_next(animation, type, event_frame);
						}
					}
				}
			}
		}
		if (transition.index != NONE && event_transition.index == NONE)
			local_0 = transition;
		else if (event_transition.index != NONE)
			local_0 = event_transition;
		else
		{
			*blend = found;
			local_0 = none;
		}
	}
	else
		local_0 = none;
	*result = local_0;
	return result;
}

/* out-of-line copies of channel inlines (unknown_0b66c0.cpp, unknown_0e6800.cpp) */
struct s_index_pair;
bool function_0b6760(s_index_pair const *pair);
bool function_0e6800(c_animation_channel const *channel);

PRIVATE inline void channel_loop_clear(c_animation_channel *channel)
{
	if (channel_valid(channel) && (channel->flags & 1))
	{
		channel->unknown11 &= ~1;
	}
}

// @retail 0x1cd070
bool s_animation_state::animation_set(long mode, long weapon_class, long weapon_type, long set, long state_flags, long channel_flags)
{
	bool result = false;
	s_animation_names names;
	s_animation_names found;
	c_type_709360 animation_id;
	bool loops = true;

	names.mode = mode;
	names.weapon_class = weapon_class;
	names.weapon_type = weapon_type;
	names.set = set;
	if (channel_valid(&channels[0]))
	{
		loops = !TEST_FIELD_BIT(function_1daea0(graph_tag_get(channels[0].graph_tag_index), channels[0].animation_id)->flag1);
	}
	if (state_flags & 0x10)
	{
		channels_clear();
	}
	unknown6e &= ~1;
	if (animation_lookup(&names, &found, mode, weapon_class, weapon_type, set, state_flags, &animation_id) &&
		animation_id.index != NONE && !(state_flags & 8))
	{
		c_type_709360 transition_id;
		c_type_709360 blend_id;
		bool blend = false;
		real fraction;
		long transition_set;

		if (!(state_flags & 0x200))
		{
			c_type_709360 temporary;

			transition_id = *transition_find(&temporary, names.mode, names.set, &blend);
			blend &= !(flags & 1);
			blend &= ((state_flags >> 10) & (channel_valid(&channels[0]) && !function_0e6800(&channels[0]))) & 1;
			if (blend)
			{
				unknown6e |= 1;
			}
			else
			{
				unknown6e &= ~1;
			}
		}
		else
		{
			unknown6e &= ~1;
		}
		if (transition_id.index != NONE || !blend)
		{
			fraction = 0.0f;
			if (((state_flags & 0x20) || (state_flags & 0x40)) && channel_valid(&channels[0]))
			{
				real frame_count = (real)channels[0].function_1c6440()->frame_count - 0.0001f;

				if (frame_count > 0.0f)
				{
					fraction = PIN(channels[0].frame_position, 0.0f, frame_count) / frame_count;
					if (state_flags & 0x40)
					{
						fraction = 1.0f - fraction;
					}
					fraction = PIN(fraction, 0.0f, 1.0f);
				}
			}
			transition_set = NONE;
			if (!(state_flags & 0x100))
			{
				c_type_709360 temporary;

				blend_id = *transition_lookup(&temporary, names.set, names.mode, names.weapon_class, names.weapon_type, &transition_set);
			}
			channels_clear();
			if (channel_start(&channels[0], animation_id, found.set, NONE, NONE, 0, (word)channel_flags))
			{
				bool unflagged;

				result = true;
				unflagged = channels[0].is_unflagged0() & loops;
				if (transition_id.index != NONE && graph_tag_index != NONE)
				{
					if (channels[2].set(graph_tag_index, 0x15, variant_get(transition_id), names.set, NONE, NONE, 3))
					{
						channels[2].set_frame_position(0.0f);
						if (channels[0].flags & 1)
						{
							channels[0].unknown11 |= 1;
						}
						unflagged &= channels[2].is_unflagged0();
						if (unflagged)
						{
							transition_offset_compute();
						}
					}
				}
				if (!function_0b6760((s_index_pair const *)&channels[2]))
				{
					if (((state_flags & 0x20) || (state_flags & 0x40)) && fraction > 0.0f)
					{
						channels[0].set_frame_position(((real)channels[0].function_1c6440()->frame_count - 0.0001f) * fraction);
					}
				}
				if (!unflagged)
				{
					flags |= 0x10;
				}
				else
				{
					flags &= ~0x10;
				}
				if (blend_id.index != NONE && graph_tag_index != NONE)
				{
					if (channels[1].set(graph_tag_index, 0x3c0, blend_id, transition_set, NONE, NONE, 0))
					{
						unknown80 = 0.0f;
					}
				}
				flags &= ~1;
				goto names_store;
			}
		}
	}
	if (!(state_flags & 4))
	{
		return result;
	}
	result = true;

names_store:
	{
		bool changed = unknown70 != names.mode || unknown74 != names.weapon_class || unknown78 != names.weapon_type;

		unknown70 = names.mode;
		unknown74 = names.weapon_class;
		unknown78 = names.weapon_type;
		unknown7c = names.set;
		if (changed)
		{
			if (graph_tag_index != NONE && !(flags & 1))
			{
				resources_request(unknown70, unknown74, unknown78, true, true);
			}
			if (!channel_valid(&channels[2]))
			{
				channel_loop_clear(&channels[2]);
				if (!channel_valid(&channels[2]))
				{
					channel_loop_clear(&channels[0]);
					channel_loop_clear(&channels[1]);
				}
			}
		}
	}
	return result;
}

// @retail 0x1cb0d0
bool s_animation_state::initialize(long graph_tag_index, long model_tag_index, bool flag)
{
	if (model_tag_index != NONE)
	{
		if (graph_tag_index == NONE)
		{
			return false;
		}
		if ((short)graph_tag_get(graph_tag_index)->node_count != *(long *)((byte *)graph_tag_get(model_tag_index) + 0x78))
		{
			return false;
		}
	}
	if (flag)
	{
		flags |= 2;
	}
	else
	{
		flags &= ~2;
	}
	unknown70 = NONE;
	unknown7c = NONE;
	unknown74 = NONE;
	unknown78 = NONE;
	this->graph_tag_index = graph_tag_index;
	channels[0].clear();
	channels[1].clear();
	channels[2].clear();
	animation_set(0x7000001, 0x7000001, 0x7000001, 0x7000001, 0x317, 0x3f);
	unknown64.unknown1 = 0;
	unknown64.unknown0 = 0;
	unknown64.unknown3 = 0;
	unknown64.unknown2 = 1;
	unknown60.unknown1 = 0;
	unknown60.unknown0 = 0;
	unknown60.unknown3 = 0;
	unknown6e = 0;
	unknown80 = 0.0f;
	return true;
}

/* the identity transform (arg_0e6cbc.cpp) */
extern real_quaternion_transform *g_4687d8;


#pragma inline_depth(0)
PRIVATE __forceinline s_graph_inheritance *function_1cbec1(s_animation_state *arg_0, c_type_709360 arg_1)
{
    return arg_0->inheritance_get(arg_1);
}
#pragma inline_depth(255)

// @retail 0x1cbec0
void s_animation_state::animation_transform_get(c_type_709360 animation_id, real seconds,
	real_quaternion_transform *transform)
{
	real frame = seconds * 30.0f;

	if (fabs(frame) < 0.0001f && function_1dd8f0(graph_get(), animation_id, transform))
	{
		return;
	}
	function_279d80(graph_get(), animation_id, 1, frame, 1.0f, function_1cbec1(this, animation_id), NULL, transform, true);
}

// @retail 0x1cbf50
void s_animation_state::animation_matrix_get(c_type_709360 animation_id, real seconds, long unused,
	transform4x3f *matrix)
{
	// Taking the address keeps the unused model argument in its retail stack slot.
	long const *unused_reference = &unused;
	__declspec(align(16)) real_quaternion_transform transform = *g_4687d8;

	animation_transform_get(animation_id, seconds, &transform);
	function_141e10(&matrix->rotation, &transform.rotation);
	matrix->position.x = 0.0f;
	matrix->position.y = 0.0f;
	matrix->position.z = 0.0f;
	matrix->scale = 1.0f;
	matrix->position = transform.position;
}

// @retail 0x1cbfd0
void s_animation_state::animation_velocity_get(c_type_709360 animation_id, real seconds, real rate, long unused,
	vector3f *velocity)
{
	long const *local_0 = &unused;
	__declspec(align(16)) real_quaternion_transform previous;
	__declspec(align(16)) real_quaternion_transform current;
	real scale;

	if (!(seconds > 1.0f / 30.0f))
	{
		seconds = 1.0f / 30.0f;
	}
	previous = *g_4687d8;
	current = *g_4687d8;
	animation_transform_get(animation_id, seconds - 1.0f / 30.0f, &previous);
	animation_transform_get(animation_id, seconds, &current);
	scale = rate * 30.0f;
	velocity->i = (current.position.x - previous.position.x) * scale;
	velocity->j = (current.position.y - previous.position.y) * scale;
	velocity->k = (current.position.z - previous.position.z) * scale;
}

PRIVATE __forceinline void function_1ce011(word *arg_0, vector3f *arg_1,
	real_quaternion_transform const *arg_2, real_quaternion_transform const *arg_3)
{
	arg_1->i = arg_2->position.x - arg_3->position.x;
	arg_1->j = arg_2->position.y - arg_3->position.y;
	*(volatile real *)&arg_1->k = arg_2->position.z - arg_3->position.z;
	*arg_0 |= 2;
}

// @retail 0x1ce010
void s_animation_state::transition_offset_compute()
{
	unknown84 = *g_4687a4;
	unknown6e &= ~2;
	if (channel_valid(&channels[2]) && channel_valid(&channels[0]) && channels[2].is_unflagged6())
	{
		__declspec(align(16)) real_quaternion_transform transition = *g_4687d8;
		__declspec(align(16)) real_quaternion_transform current = *g_4687d8;

		c_type_709360 local_0;
		*(long *)&local_0 = *(volatile long const *)&channels[2].animation_id;
		animation_transform_get(local_0, channels[2].get_duration(), &transition);
		*(long *)&local_0 = *(volatile long const *)&channels[0].animation_id;
		animation_transform_get(local_0, 0.0f, &current);
		function_1ce011(&unknown6e, &unknown84, &current, &transition);
	}
}

// @retail 0x1cbd10
c_type_709360 s_animation_state::animation_find(long name)
{
	return function_1dd0b0(graph_get(), name);
}

// @retail 0x1cbd50
void s_animation_state::nodes_compute(transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root)
{
	function_1dd1c0(graph_get(), matrices, orientations, root);
}

// @retail 0x1cc2b0
void s_animation_state::orientations_blend(s_blend_orientation const *targets, short count, dword const *mask,
	s_blend_orientation *orientations)
{
	function_1d9470((s_1d9240 const *)&unknown64, targets, count, mask, orientations);
}

// @retail 0x1cb5f0
bool function_1cb5f0(long node_count, s_animation_state *state, real seconds, s_blend_orientation *orientations,
	s_blend_orientation const *targets)
{
	bool result = false;

	if (node_count > 0 && seconds > 0.0f)
	{
		s_animation_bits *counter = &state->unknown64;

		if (counter->unknown1 == 0 || (counter->unknown3 & 2) || seconds >= function_1d9430((s_1d9240 const *)counter))
		{
			counter->unknown1 = 0;
			counter->unknown0 = 0;
			counter->unknown3 = 0;
			memcpy(orientations, targets, node_count * 0x20);
			function_1d9240((s_1d9240 *)counter, true, seconds);
			result = true;
		}
	}
	return result;
}

/* whether a channel's animation lacks flag1 (true without one) */
inline bool channel_is_unflagged1(c_animation_channel const *channel)
{
	bool result = true;

	if (channel_valid(channel))
	{
		result = !TEST_FIELD_BIT(channel->function_1c6440()->flag1);
	}
	return result;
}

// @retail 0x1cb710
bool s_animation_state::update(animation_event_callback callback, long user, long node_count,
	s_blend_orientation *orientations, s_blend_orientation const *targets)
{
	bool result = true;
	c_animation_channel *channel = channels;
	long i;

	for (i = 0; result; i++, channel++)
	{
		if (i >= 3)
			break;
		if (channel_valid(channel))
		{
			result &= channel_update(channel, callback, user);
		}
	}
	if (channel_valid(&channels[2]) && (channels[2].unknown11 & 0xa))
	{
		if (channel_is_unflagged1(&channels[2]))
		{
			function_1cb5f0(node_count, this, 0.267f, orientations, targets);
		}
		channels[2].clear();
		if (channel_valid(&channels[0]) && (channels[0].flags & 1))
		{
			channels[0].unknown11 &= ~1;
		}
	}
	update_blend_flags();
	if (g_46fbf4 && channel_valid(&channels[1]))
	{
		s_animation *animation = channels[0].function_1c6440();
		real ratio = 0.0f;

		if (animation->frame_count > 0)
		{
			ratio = channels[0].frame_position / (real)animation->frame_count;
			ratio = PIN(ratio, 0.0f, 1.0f);
		}
		c_animation_channel_advance(&channels[1], (real)channels[1].function_1c6440()->frame_count * ratio, this,
			callback, user);
	}
	return result;
}

// @retail 0x1cb490
bool s_animation_state::channel_play(c_animation_channel *channel, c_type_709360 animation_id, word channel_flags)
{
	bool result = false;

	if (graph_tag_index != NONE)
	{
		result = channel_start(channel, animation_id, NONE, NONE, NONE, NONE, channel_flags);
	}
	return result;
}

// @retail 0x1cb4c0
bool s_animation_state::channel_play_named(c_animation_channel *channel, long name, word channel_flags)
{
	bool result = false;

	if (graph_tag_index != NONE)
	{
		c_type_709360 animation_id = function_1dd0b0(graph_get(), name);

		if (animation_id.index != NONE)
		{
			result = channel_start(channel, animation_id, name, NONE, NONE, 0, channel_flags);
		}
	}
	return result;
}

/* the blend counters (unknown_1d9240.cpp) */
bool function_1d9320(s_1d9240 *p);

// @retail 0x1cd4e0
bool s_animation_state::blend_counters_update()
{
	bool result = function_1d9320((s_1d9240 *)&unknown64);

	if (unknown64.unknown3 & 2)
	{
		s_animation_bits *bits = &unknown64;

		bits->unknown1 = 0;
		bits->unknown0 = 0;
		bits->unknown3 = 0;
	}
	result |= function_1d9320((s_1d9240 *)&unknown60);
	return result;
}

// @retail 0x1cd590
void s_animation_state::sample(long unused1, real weight, dword const *node_mask, real_quaternion_transform *transforms,
	long unused5, long unused6, long node_count)
{
	long const *local_0 = &unused1;
	long const *local_1 = &unused5;
	long const *local_2 = &unused6;
	if (g_46fbf5 && channel_valid(&channels[2]))
	{
		channels[2].sample(weight, node_mask, node_count, transforms);
		if (node_count > 0 && (!node_mask || (node_mask[0] & 1)))
		{
			translation_apply((real_orientation_1ce110 *)transforms, weight);
		}
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 >= 0.9999f)
	{
		channels[1].sample(weight, node_mask, node_count, transforms);
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && !(0.0001f >= unknown80 || unknown80 >= 0.9999f))
	{
		real fraction = blend_fraction_get();

		channels[0].sample(weight, node_mask, node_count, transforms);
		channels[1].sample(fraction * weight, node_mask, node_count, transforms);
	}
	else
	{
		channels[0].sample(weight, node_mask, node_count, transforms);
	}
}

/* an object as the animation state reads it */
struct s_animated_object
{
	byte unknown00[0xb3];
	byte unknownb3;
};

struct s_animated_object_header
{
	byte unknown00[8];
	s_animated_object *object;
};

// @retail 0x1cd6c0
void s_animation_state::movement_rate_get(long object_index, vector3f *vector, real *value)
{
	if (object_index != NONE && ((s_animated_object_header *)g_4e0300->data)[object_index & 0xffff].object->unknownb3)
	{
		*vector = *g_4687a4;
		*value = 0.0f;
	}
	else if (g_46fbf5 && channel_valid(&channels[2]))
	{
		channels[2].movement_rate_get(vector, value);
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 >= 0.9999f)
	{
		channels[1].movement_rate_get(vector, value);
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 > 0.0001f && unknown80 < 0.9999f)
	{
		real fraction = blend_fraction_get();
		real blend_value;
		real base_value;
		vector3f blend_vector;
		vector3f base_vector;

		channels[1].movement_rate_get(&blend_vector, &blend_value);
		channels[0].movement_rate_get(&base_vector, &base_value);
		vector->i = (blend_vector.i - base_vector.i) * fraction + base_vector.i;
		vector->j = (blend_vector.j - base_vector.j) * fraction + base_vector.j;
		vector->k = (blend_vector.k - base_vector.k) * fraction + base_vector.k;
		*value = (blend_value - base_value) * fraction + base_value;
	}
	else if (channel_valid(&channels[0]))
	{
		channels[0].movement_rate_get(vector, value);
	}
	else
	{
		*vector = *g_4687a4;
		*value = 0.0f;
	}
}

// @retail 0x1cdcd0
bool s_animation_state::velocity_get(vector3f *delta, vector3f *velocity)
{
	if (g_46fbf5 && channel_valid(&channels[2]))
	{
		return channels[2].velocity_get(delta, velocity);
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 >= 0.9999f)
	{
		return channels[1].velocity_get(delta, velocity);
	}
	else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 > 0.0001f && unknown80 < 0.9999f)
	{
		real fraction = blend_fraction_get();
		vector3f blend_velocity;
		vector3f blend_delta;
		vector3f base_velocity;
		vector3f base_delta;
		bool result = channels[1].velocity_get(&blend_delta, &blend_velocity);

		result &= channels[0].velocity_get(&base_delta, &base_velocity);
		delta->i = (blend_delta.i - base_delta.i) * fraction + base_delta.i;
		delta->j = (blend_delta.j - base_delta.j) * fraction + base_delta.j;
		delta->k = (blend_delta.k - base_delta.k) * fraction + base_delta.k;
		velocity->i = (blend_velocity.i - base_velocity.i) * fraction + base_velocity.i;
		velocity->j = (blend_velocity.j - base_velocity.j) * fraction + base_velocity.j;
		velocity->k = (blend_velocity.k - base_velocity.k) * fraction + base_velocity.k;
		return result;
	}
	return channels[0].velocity_get(delta, velocity);
}

// @retail 0x1cb970
c_type_709360 s_animation_state::animation_get(long set, long weapon_class, long weapon_type)
{
	c_type_709360 none;
	c_type_709360 animation_id;
	s_animation_names found;
	s_animation_names names;

	if (animation_lookup(&names, &found, unknown70, weapon_class, weapon_type, set, 0, &animation_id))
	{
		none = animation_id;
	}
	return none;
}

// @retail 0x1cbce0
bool s_animation_state::pairs_iterate(s_graph_pair_iterator *iterator)
{
	s_graph_tag *local_0 = graph_get();
	iterator->mode = unknown70;
	iterator->weapon_class = unknown74;
	return function_1dcfa0(local_0, iterator);
}
void function_1dd290(s_graph_tag *graph, transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root, short mirrored_node_index, short mirror_parent_index);

// @retail 0x1cbd80
void s_animation_state::nodes_compute_mirrored(transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root, short mirrored_node_index, short mirror_parent_index)
{
	function_1dd290(graph_get(), matrices, orientations, root, mirrored_node_index, mirror_parent_index);
}

// @retail 0x1cc150
c_type_709360 s_animation_state::overlay_default_get()
{
	c_type_709360 animation_id = graph_get()->overlay_get(unknown70, unknown74, unknown78, 0x400004c, NULL, NULL, NULL);

	return animation_id;
}

// @retail 0x1cc260
c_type_709360 s_animation_state::overlay_get(long set)
{
	c_type_709360 animation_id = graph_get()->overlay_get(unknown70, unknown74, unknown78, set, NULL, NULL, NULL);

	return animation_id;
}

// @retail 0x1cc0a0
bool s_animation_state::overlay_exists()
{
	s_graph_tag *graph = graph_get();
	c_type_709360 animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xe000044, NULL, NULL, NULL);

	if (animation_id.index == NONE)
	{
		animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xe000045, NULL, NULL, NULL);
		if (animation_id.index == NONE)
		{
			animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xb000046, NULL, NULL, NULL);
		}
	}
	byte local_0 = animation_id.index != NONE;
	return local_0;
}

// @retail 0x1cc1a0
c_type_709360 s_animation_state::overlay_kind_get(long kind)
{
	s_graph_tag *graph = graph_get();
	c_type_709360 animation_id;

	if (kind == 1)
		animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xe000045, NULL, NULL, NULL);
	else if (kind == 0)
		animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xe000044, NULL, NULL, NULL);
	else if (kind == 2)
		animation_id = graph->overlay_get(unknown70, unknown74, unknown78, 0xb000046, NULL, NULL, NULL);
	return animation_id;
}

// @retail 0x1cb520
bool s_animation_state::overlay_play(c_animation_channel *channel, word channel_flags, long set, long weapon_class,
	long weapon_type)
{
	bool result = false;

	if (weapon_class == 0x7000101 || weapon_class == NONE)
	{
		weapon_class = unknown74;
	}
	if (weapon_type == 0x7000101 || weapon_type == NONE)
	{
		weapon_type = unknown78;
	}
	if (graph_tag_index != NONE)
	{
		c_type_709360 animation_id = graph_get()->overlay_get(unknown70, weapon_class, weapon_type, set, NULL, NULL, NULL);

		if (animation_id.index != NONE)
		{
			result = channel_start(channel, animation_id, set, NONE, NONE, 1, channel_flags);
		}
	}
	return result;
}

// @retail 0x1cb230
bool s_animation_state::channel_refresh(c_animation_channel *channel, long weapon_class, long weapon_type)
{
	bool result = false;

	if (channel_valid(channel))
	{
		s_graph_tag *graph = graph_get();
		c_type_709360 old_animation_id = channel->animation_id;
		long name = channel->unknown08;
		c_type_709360 animation_id;
		char kind;

		if (weapon_class == 0x7000101)
		{
			weapon_class = unknown74;
		}
		if (weapon_type == 0x7000101)
		{
			weapon_type = unknown78;
		}
		kind = channel->unknown0e;
		switch (kind)
		{
		case 0:
			animation_id = graph->animation_get(unknown70, weapon_class, weapon_type, name, NULL, NULL, NULL);
			break;
		case 1:
			animation_id = graph->overlay_get(unknown70, weapon_class, weapon_type, name, NULL, NULL, NULL);
			break;
		case 4:
		{
			s_graph_element3c *element = (s_graph_element3c *)function_1dd560((s_sorted_array *)&graph->unknown3c_count,
				name, sizeof(s_graph_element3c));

			if (element)
			{
				animation_id = element->animation_id;
			}
			break;
		}
		case 5:
		{
			s_graph_element44 *element = (s_graph_element44 *)function_1dd560((s_sorted_array *)&graph->unknown44_count,
				name, sizeof(s_graph_element44));

			if (element)
			{
				animation_id = element->animation_id;
			}
			break;
		}
		}
		if (animation_id.index != NONE)
		{
			if (animation_id.graph_index == old_animation_id.graph_index && animation_id.index == old_animation_id.index)
			{
				return true;
			}
			result = channel_start(channel, animation_id, name, NONE, NONE, kind, channel->flags);
		}
		if (!result)
		{
			channel->clear();
		}
	}
	return result;
}

// @retail 0x1cb9d0
c_type_709360 s_animation_state::overlay_find(long set, long weapon_class, long weapon_type)
{
	c_type_709360 none;

	if (graph_tag_index != NONE)
	{
		c_animation_channel channel;

		if (overlay_play(&channel, 0, set, weapon_class, weapon_type))
		{
			none = channel.animation_id;
		}
	}
	return none;
}

// @retail 0x1cbad0
c_type_709360 animation_state_overlay_or_animation_get(s_animation_state *state, long weapon_class, long set,
	long weapon_type)
{
	c_type_709360 animation_id;

	if (weapon_class == 0x7000101 || weapon_class == NONE)
	{
		weapon_class = state->unknown74;
	}
	if (weapon_type == 0x7000101 || weapon_type == NONE)
	{
		weapon_type = state->unknown78;
	}
	animation_id = state->overlay_find(set, weapon_class, weapon_type);
	if (animation_id.index == NONE)
	{
		animation_id = state->animation_get(set, weapon_class, weapon_type);
	}
	return animation_id;
}

// @retail 0x1cba80
s_animation const *function_1cba80(s_animation_state *state, long weapon_class, long weapon_type, long set)
{
	s_animation const *animation = NULL;
	c_type_709360 animation_id = animation_state_overlay_or_animation_get(state, weapon_class, set, weapon_type);

	if (animation_id.index != NONE)
	{
		animation = function_1daea0(state->graph_get(), animation_id);
	}
	return animation;
}

/* the next item and animation index to try when a set has none at an index */
short const g_46fc18[11] = { 2, 0, 1, 1, 3, 0, 5, 1, 7, 0, 9 };
short const g_4454a0[4] = { 1, 3, 0, 2 };

// @retail 0x1cbb50
bool s_animation_state::channel_play_indexed(c_animation_channel *channel, long set, short item_index,
	short animation_index, short *found_item_index, short *found_animation_index)
{
	s_graph_tag *graph = graph_get();
	c_type_709360 animation_id;
	short first_item_index = item_index;
	short first_animation_index = animation_index;
	bool searching = true;
	long attempts = 0;

	while (searching && attempts < 4)
	{
		long i;

		attempts++;
		animation_index = first_animation_index;
		i = 0;
		do
		{
			long found_mode;
			long found_weapon_class;
			long found_weapon_type;

			if (i >= 11)
			{
				break;
			}
			i++;
			animation_id = graph->animation_find(unknown70, unknown74, unknown78, set, item_index, animation_index,
				&found_mode, &found_weapon_class, &found_weapon_type);
			if (animation_id.index != NONE)
			{
				searching = false;
				break;
			}
			animation_index = g_46fc18[animation_index];
		}
		while (animation_index != first_animation_index);
		if (animation_id.index == NONE)
		{
			item_index = g_4454a0[item_index];
			if (item_index == first_item_index)
			{
				return false;
			}
		}
	}
	if (animation_id.index != NONE)
	{
		word channel_flags = 0x3f;
		bool result;

		if (set == 0x600008b)
		{
			channel_flags = 0x803f;
		}
		result = channel_start(channel, animation_id, set, (char)item_index, (char)animation_index, 2, channel_flags);
		if (found_item_index)
		{
			*found_item_index = item_index;
		}
		if (found_animation_index)
		{
			*found_animation_index = animation_index;
		}
		return result;
	}
	return false;
}

// @retail 0x1cbcb0
bool s_animation_state::play_indexed(long set, short item_index, short animation_index, short *found_item_index,
	short *found_animation_index)
{
	bool result = channel_play_indexed(&channels[0], set, item_index, animation_index, found_item_index,
		found_animation_index);

	if (result)
	{
		secondary_channels_clear();
	}
	return result;
}

// @retail 0x1ccae0
bool s_animation_state::play(c_type_709360 animation_id, word channel_flags)
{
	bool result;

	animation_set(0x7000101, 0x7000101, 0x7000101, 0xe0000c2, 4, 0x3f);
	result = channel_play(&channels[0], animation_id, channel_flags);
	if (result)
	{
		secondary_channels_clear();
	}
	return result;
}


struct s_anim_data;
void function_20ab60(s_anim_data *data, vector3f *sum, real *w);
void c_animation_channel_data_get(c_animation_channel const *channel, s_animation_data *data);
real magnitude3d(vector3f const *vector);

// @retail 0x1cd8a0
bool __stdcall function_1cd8a0(s_animation_state *state, long object_index, vector3f const *velocity)
{
    bool result = false;
    if ((object_index == NONE || !(((s_animated_object_header *)g_4e0300->data)[object_index & 0xffff].object->unknownb3 > 0)) &&
        !(state->unknown6e & 1))
    {
    c_animation_channel *primary = &state->channels[0];
    c_animation_channel *secondary;
    s_animation *animation;
    if (g_46fbf4 && channel_valid(primary) && (animation = primary->function_1c6440()) != NULL &&
        animation->frame_info_type && function_0b6760((s_index_pair const *)(secondary = &state->channels[1])) &&
        secondary->function_1c6440() && secondary->function_1c6440()->frame_count > 0)
    {
        s_animation_data data;
        vector3f movement;
        real unused;
        c_animation_channel_data_get(primary, &data);
        real measured = magnitude3d(velocity);
        function_20ab60((s_anim_data *)&data, &movement, &unused);
        real primary_speed = magnitude3d(&movement) * 30.0f;
        real rate = 1.0f;
        c_animation_channel_data_get(secondary, &data);
        function_20ab60((s_anim_data *)&data, &movement, &unused);
        real secondary_speed = magnitude3d(&movement) * 30.0f;
        real weight = 0.0f;
        real difference = primary_speed - secondary_speed;
        if (difference > 0.0f)
        {
            real fraction = (measured - secondary_speed) / difference;
            if (fraction < 0.0f) fraction = 0.0f;
            else if (fraction > 1.0f) fraction = 1.0f;
            real primary_frames = (real)primary->function_1c6440()->frame_count;
            real ratio = primary_frames / (real)secondary->function_1c6440()->frame_count;
            rate = (1.0f - ratio) * fraction + ratio;
            weight = PIN(1.0f - fraction, 0.0f, 1.0f);
            rate = PIN(rate, 0.0f, 1.0f);
        }
        real smoothed = state->unknown80 * 0.9f;
        smoothed += weight * 0.1f;
        state->unknown80 = smoothed;
        if (rate > 0.9f)
            rate = 1.0f;
        primary->rate = rate;
        return true;
    }
    else
        state->unknown80 = 0.0f;
    }
    return result;
}
