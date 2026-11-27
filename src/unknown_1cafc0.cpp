// @flags /O2 /arch:SSE /Gr
#include "orientations_storage.h"
/* UNKNOWN_1CAFC0.CPP: the object that drives three animation channels of
   one graph (0x1cafc0..0x1ce1e0) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1c62f0.h"
#include "animation_cache.h"
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
void function_1dd1c0(s_graph_tag *graph, transform4x3f *matrices, rigid_transform_scaled const *orientations,
	transform4x3f const *root);

/* the blend counters (unknown_1d9240.cpp) */
struct s_1d9240;
void function_1d9240(s_1d9240 *p, char flag, real x);
real function_1d9430(s_1d9240 const *p);
void function_1d9470(s_1d9240 const *p, long count, dword const *mask, s_blend_orientation const *targets,
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
  long local_0 = channels[0].graph_tag_index;
  flags0 &= ~0x380;
  flags1 |= 0x380;
  if (local_0 == NONE)
   goto local_2;
 }
 else if (unknown80 > 0.55f)
 {
  long local_1 = channels[0].graph_tag_index;
  flags |= 0x20;
  flags0 |= 0x380;
  flags1 &= ~0x380;
  if (local_1 == NONE)
   goto local_2;
 }
 else
  return;
 if (channels[0].animation_id.index != NONE)
  channels[0].flags = (word)flags0;
local_2:
 if (channel_valid(&channels[1]))
  channels[1].flags = (word)flags1;
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
c_type_709360 s_animation_state::current_animation_get()
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
	return none;
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
	entry = (s_graph_name_entry *)function_1dd560((s_sorted_array *)&((s_graph_tag *)g_4e3b44[graph_tag_index & 0xffff].bytes)->weapons, names->weapon_type, sizeof(s_graph_name_entry));
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
inline bool s_animation_state::channel_start(c_animation_channel *channel, c_type_709360 animation_id, long unknown08,
	char unknown0c, char unknown0d, char unknown0e, dword channel_flags)
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

/* requests the resources of a mode, weapon class and weapon type and their defaults, in the graph and the
   graphs it inherits from (src/unknown_1dacb0.cpp's inline copy of 0x1ddd00, which retail inlines here) */
void graph_resources_request(s_graph_tag *graph, long mode, long weapon_class, long weapon_type, bool urgent,
	bool other);

/* looks an animation up by its resolved names, then with any set and then with the default names, and requests
   the resources of the one it finds */
// The first lookup's four names are read into locals in the fallback chain's order (type, class, mode) and then the
// set, and the status is a bool (retail returns the byte as it was stored, 0x1ccac3). The locals put four more
// values in the entry block than passing the fields does, and that ranks the two found-name pointers (EBX, EBP)
// above the inner results of the inheritance loop, 123 against 108 (107 without the locals). Retail's request
// follows from that order: graph + 0x34 formed in place in EBX and the graph loaded into EBP at each loop.
// @retail 0x1cc3f0
bool s_animation_state::animation_lookup(s_animation_names *names, s_animation_names *found, long mode, long weapon_class,
	long weapon_type, long set, long lookup_flags, c_type_709360 *result)
{
	s_graph_tag *graph = graph_get();
	bool success = false;

	names_resolve(names, mode, weapon_class, weapon_type, set);
	*found = *names;

	long resolved_weapon_type = names->weapon_type;
	long resolved_weapon_class = names->weapon_class;
	long resolved_mode = names->mode;
	long resolved_set = names->set;

	*result = graph->animation_get(resolved_mode, resolved_weapon_class, resolved_weapon_type, resolved_set,
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
		graph_resources_request(graph, found->mode, found->weapon_class, found->weapon_type, true, false);
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
c_type_709360 s_animation_state::transition_lookup(long set, long mode, long weapon_class, long weapon_type,
	long *transition_set)
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
	return animation_id;
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
c_type_709360 s_animation_state::transition_find(long mode, long set, bool *blend)
{
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
			*blend = found;
	}
	return local_0;
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

// MiniMax H (11-19): no new form, no lower score (210): the two id slots are still the other way round (retail keeps the transition id at frame+0x14 and the animation id at frame+0x18) and the blend-query byte is still computed after the 0xe6800 call where retail computes it before; the forms that align more instructions all raise the positional score.
// MiniMax H (11-18): no new form, no lower score (210): the two id slots are still the other way round (retail keeps
// the transition id at frame+0x14/0x16 with its stores in the block and the animation id at frame+0x18/0x1a with its
// stores at the entry), the blend-query byte is still computed after the 0xe6800 call where retail computes it before,
// and the set() call's fourth argument still differs; the 0xe6800 call's body is in another file.
// MiniMax E (11-17): no new form, no lower score (210): the two id slots are still swapped (retail keeps the animation id at frame+0x18/0x1a with its -1 stores at the entry and the transition id at frame+0x14/0x16 with its stores in the block); four declaration and scope orderings measured at 210 or 211, and moving the transition id to function scope gives retail's slots but leaves its constructor at the entry (353).
// MiniMax E (11-15): no form kept. The permuter's best variant (195, same in both images) swaps the first and
// fourth arguments of the transition channel's set() call, which would put the animation set name into the
// channel's graph tag index instead of the state's graph tag index that retail loads from [edi+0x68] at
// 0x1cd312 and passes at 0x1cd324, and it adds a local that holds only a constant; both are on the reverted
// list, so the original call stands. Still differs: the two id slots are swapped (retail keeps the transition id
// at frame+0x04/0x06 with its -1 stores in the block and the animation id at frame+0x08/0x0a with its -1 stores
// at the entry), the blend-query byte's residency, and the set() call's fourth argument (retail keeps the set
// name in ebp, ours reloads it from a stack slot).
// MiniMax h (11-04): no lower score; a block around the lookup leaves the id homes swapped (retail animation id 0x18, transition id 0x14).
// MiniMax S (11-16): no new form, no lower score (210); the permuter's only lower form swaps the first and fourth arguments of the channels[2].set call, which retail does not do (retail passes the state's graph tag in ecx and the set name in ebp), so it is not kept; passing the set parameter instead of names.set measures 223, the third parameter 221, and naming the third and fourth parameters the other way round 214.
// MiniMax B (11-18): no new form, no lower score (210): the transition id still sits at frame+0x08 where retail keeps it at frame+0x04 with its stores in the block; hoisting it to function scope in either order (357 and 359), an aggregate of the two ids (368) and a two-element array (361) all score worse because the hoist changes the register allocation, and the blend byte's residency across the 0xe6800 call follows that callee's file.
// score 210 (was 210): Frame 0x38 and result/prior-loop bytes agree; the two id slots are swapped: retail keeps the
// transition id at frame+0x04/0x06 with its -1 stores at the block entry and the animation id at frame+0x08/0x0a
// with its -1 stores at the function entry, ours is the reverse. Moving the transition id to function scope gives
// retail's slots but leaves its constructor at the entry (353); re-initialising it in the block adds the
// block-entry stores but the entry constructor stays (353); no C++ scoping gives both. Blend-query lifetime
// (retail keeps the blend byte in memory across 0xe6800) and the result/loop byte homes also differ.
// Earlier 210 (was 210): Frame 0x38 and result/prior-loop bytes agree; ID homes and blend-query lifetime still differ.
// Round 15: inline channel query 210; early success with separate name comparisons 256; separate comparisons
// with late success 265; bitwise separate comparisons 256; inline transition-find 210 (callee still matched);
// fraction clamp workspace 287; signed-byte result 210; primary-ID lexical rename 210. Original body restored.
// Shared loop query/getter 251/251; shared query with byte result 249, signed-byte/long loop
// 251/337; getter with byte result 249; shared query with signed-byte result/both bytes 249/249.
// Two-point gains dropped.
// frame 0x38 agrees; ID slots, result/loop byte homes,
// blend reloads and the frame clamp differ. Success after setup 251; loop
// Frame 0x38 agrees; identifier/result/loop homes and protected query lifetime remain.
// Success after setup 251; loop
// state width 283; channel state width 314; name aggregate 322; flag mask 353
// (six match losses). On 251: logical channel 287; logical offset 248 (gain
// three, dropped); loop expression 345; success after query 274; before
// overlay 255; previous-loop home name 251.
// Byte result 249 (gain two, dropped); entry third ID 306; two-channel clear loop 251; masked blend bit 311; byte combined state 251.
// Transition ID name 251; blend-first join 327; failed result from flag 354;
// frame-position snapshot 297; reversed bitwise loop result 251; previous-loop
// lifetime first 254; false result after channel observation 286 (fast).
// Word result 255; word previous loop 334; word combined state 312; reused selected set 251; found record at lookup 251; byte result plus logical offset 269 (fast).
// Full-width return 255; signed-byte previous loop 251; primary ID at lookup 339; names initialized at
// lookup 443; aggregate names on late-success form 320 (fast).
// This round: channel result producer 248; channel result late false 286; channel result byte 271;
// channel result word 260; channel result long 259; channel result changed width 291; channel result
// previous byte 248; channel result carrier pair 291; channel producer logical offset 293; channel
// producer logical queries 327.
// Frame 0x38 agrees; ID/Boolean homes and blend call lifetime remain. Byte result 249 (gain two, dropped); entry third ID 306; two-channel clear loop 251; masked blend bit 311; byte combined state 251.
// ID/result/loop homes, blend reloads and frame clamp remain.
// Frame 0x38 agrees; ID/Boolean homes and blend lifetime remain. Word result 255; word previous loop 334; word combined state 312; reused selected set 251; found record at lookup 251; byte result plus logical offset 269 (fast).
// Frame 0x38 agrees; identifier/result/loop homes and protected query lifetime remain.
// Optional blend-ID initialization 289; transition-set lookup scope 251; both 289; channel-state
// initialization 251; positive channel flag 254; reversed independent clears 251;
// state/flag/clear combination 254; native ID/set/state combination 289.
// First logical blend 311; query-first logical blend 310; both logical 318; staged channel
// admission 248 (gain three, dropped); logical staged 315; named readiness 254;
// logical named readiness 312; lookup-owned work 251.
// Round 167: Three separate name-change predicates on the late-success body, logical/bitwise joins:
// bool 260/254, byte 260/254, short 280/272, long 277/267.
// All three comparisons precede the original four name stores; original combined predicate restored.
// Round 194: Native output-record field/aggregate/logical/aggregate-logical pointer forms
// 250/222/254/252; references 250/222/254/252; declared member 255 (plain 255).
// Declared member with repeated field pointer/reference 222/222; field pointer kept (plain 222, 1139/1133).
// Round 375: Native optional transition construction, mutable/const 341/341; with native
// optional overlay 338/338; uniform three-ID role names on those graphs 341/341/338/338.
// Frame 0x38 retained; all eight forms restored.
// Complete lookup/admission/transition/channel result chain, branch/derived fallback:
// bool 371/354, byte 377/342, short 391/339, long 390/312. All shrink frame to 0x34 versus
// retail 0x38. Original late-success body, identifier lifetimes and protected queries restored.
// Round 337: complementary prior-loop bool/byte/short/long 325/325/315/331;
// negative combined state 282; both negative bool/byte 343/343;
// both negative with complete De Morgan producer 327. Original states and body restored.
// Round 199: Destination-record pointer at four earlier boundaries 222 each; full ID role names,
// swapped record declarations and selected-set lifetime 222 each. Byte result with early pointer
// 210 (plain 210); byte result with original pointer lifetime 210. Original pointer lifetime retained.
// On the byte-result/native-record graph, eight primary/offset/overlay ID-name families
// each 210; identifier homes and protected blend-query lifetime remain. Original names restored.
// Round 204: repeated native channel receivers primary/transition/overlay 210/269/403;
// primary-transition/primary-overlay/transition-overlay/all 269/403/369/369;
// all receivers before result storage 369. Identifier homes and blend-query lifetime remain.
// Round 187: primary ID at lookup 342; transition ID at entry 357; both 304;
// overlay ID at entry 267; with primary at lookup 332; with transition at entry 347;
// all three scope changes 333; entry primary/overlay/transition order 349. Restored.
// Input-record field orders 0-2-1-3/3-0-1-2/3-2-1-0/1-2-3-0/2-3-0-1/0-3-2-1/
// 2-1-0-3/1-0-3-2 give 212/213/215/216/213/212/215/216 (mode/class/type/set = 0/1/2/3).
// Complete publication schedules do not recover the identifier homes; original order restored.
// Round 143: the blend flag's 0x6e update after the 0x200 test (retail threads that test straight to
// one shared clear) 337 (1110 bytes); blend declared before the transition ID 211; the animation ID
// declared first 210; transition_set declared with the blend ID 213; frame position read into a
// local before the frame-count test 287 (1133 bytes).
// Round 212: success byte before/after the prior-loop result 274/275; before offset-valid
// query/before loop-flag publication 271/271; the same four with byte combined loop state
// 274/275/271/271. Original late success, identifiers and protected query restored.
// Round 62: retail keeps the transition ID in memory and the resolved set in EBP, ours caches the
// transition ID's index in BP. Success stored before the channel queries 274; member NONE test
// on the transition ID 210 (identical); resolved set named after the lookup, first/last 210/210
// (identical); blend declared first 211; graph test before the transition test 272.
// Round 64 (aligned counts in brackets, baseline [106]): retail keeps blend and the unflagged byte in
// the [esp+0x50] slot (stored after every `&=`, reloaded after the 0xe6800 call), as if &blend escaped
// in transition_find; a `bool *const *` read of transition_find's blend parameter (0x1ccda0 still
// matches) 298 [110] moves the primary ID to retail's slot. Three name-change bools tested with `||`
// after the store 265 [53]; with result true right after channel_start 256 [53]; plus bool result 249
// [51] (362 instructions as retail; left: blend memory traffic, the cold `state_flags & 4` block after
// the epilogue, the fraction pin's load order); the same with the gotos replaced by `if (!result &&
// (state_flags & 4)) result = true; if (result) {...}` 249 [51] (identical code); `result ||` test 295;
// bool result with early success 269 [105]; three bools with bool result 295 [65]; all with the
// captured blend 261 [65].
// Lane 10 r13: retail holds the set (its register argument) in EBP from the lookup to the
// transition channel, reloaded from the names.set slot, and keeps the transition ID in memory; ours
// caches the transition ID's index in BP instead. Animation ID renamed to sort last 210 (identical
// layout); the set parameter at the three later uses 277 (retail's ID slots and memory transition ID,
// 1133 bytes, but the set moves to the stack and every argument slot shifts); only at
// transition_find 309; only at transition_lookup/the transition channel 223/223 (set stacked);
// a set local copied after the lookup 210 (identical, read back from names.set).
// Round 228: entry declaration scopes for fraction/resolved set/combined state,
// seven subsets 210/210/210/210/210/210/210; all three with entry blend default 266. All restored.
// Round 202: End found-name/primary-ID lookup workspace before name publication, with
// byte/bool/short/long result: 210/222/226/227 for either bool or byte prior-loop truth.
// Original workspace, byte result, loop truth and all constructors restored.
// Round P208: native library name-record publication 210; with aggregate input 318;
// with logical change join 244; with both 320; the same four with forced existing
// clear helper 210/318/244/320. Original record publication, identifiers and helper restored.
// Existing primary lookup/transition lookup/channel-start inline declarations,
// their three pairs, all three, and all three kept out of line: 210 each.
// Original declaration contracts, identifier homes and complete body restored.
// Round 383: existing variant helper ordinary/forced/noinline/inline-noinline
// contracts 210/338/210/210; offset helper counterparts 210/654/210/210.
// Original class declarations and complete body restored.
// Round 170: Failure-owned false result, byte/bool/short/long, with bool then byte prior-loop truth
// 357/337/346/344/357/337/346/344. Original entry false, constructors and protected queries restored.
// Native word flag-bit publication at entry/transition/disabled joins 210/318/318/210/210/318/318/210;
// native completion return contracts 222/222/206/206; reused transition-availability bool/byte/short/long 273/273/264/266;
// original word operations, availability observations and Boolean interface restored.
// Round 263: previous-record/validity inline/noinline declarations singly and all four pairs
// 210/210/210/210/210/210/210/210. Original constructors, provider declarations and protected query restored.
// Round 185: Prior-name record initialization/assignment/library copies, eager/logical joins,
// 271/257 in each pair; overlap-safe copy 265/272. Eager copies recover identifier homes but regress
// overall. Original names, identifier scopes, queries, publication and complete body restored.
// Round 268: Phase-preservation predicate once at phase/entry, bool/byte/short/long:
// 321/321/316/334/345/345/332/349. Original body restored.
// Round 218: Complete final name-change result byte/short/long/dword 210/249/250/250; all-equal
// bool/byte/short/long 210/210/249/250. Identifier homes and protected-query blend residency
// remain; original eager change producer restored.
// Round 197: Existing validity provider at prior/phase/publication masks 1..7 gives
// 251/209/329/245/286/243/313; all with native-byte prior-loop truth 313.
// The one-point phase gain is rejected; original predicates and body restored.
// DL204: conditional blend/loop preservation 328/322/309/368; native three-long name-prefix
// comparison alone/with blend updates 338/349/361/386. All original executable forms restored.
// Round 459: Unsigned state/channel/both flag domains 210/210/210; uniform phase mask with
// original/state/channel/both domains 337/337/337/337; all with retained phase predicate 315. All
// executable trial forms restored.
// Round 262: Existing library lower/upper selection at fraction only 318/318;
// phase only 323/323; all four phase/fraction order combinations 337 each.
// Original conditional clamps, identifier scopes, byte result and all query contracts restored.
// Walk (b6F, 11-06, fast build 210 before and after; comments only): compared with retail statement by statement, no
// behaviour difference found. Checked: prior-loop truth from the channel-0 graph entry flag (0x1cd0ba-0x1cd0e0);
// lookup argument order and the id/found/names slots (0x1cd114); transition_find argument order and the blend mask by
// flags&1, e6800 and state_flags>>10 (0x1cd162-0x1cd1b9); the 0x200 path clearing unknown6e (0x1cd1c1); the early
// exit when no transition id and blend (0x1cd1c5); the fraction chain, 0.0001/0.0/1.0 constants confirmed in the XBE
// (0x1cd1d8-0x1cd272); the blend-id lookup only without 0x100 (0x1cd28b-0x1cd2a6); channel_start arguments
// (0x1cd2cd); transition channel set(0x15, ..., 3) and frame 0 (0x1cd324-0x1cd331); loop flag copy (0x1cd336);
// offset compute guarded by the combined unflagged byte (0x1cd34f); frame placement guarded by 0x20/0x40 and
// fraction > 0 (0x1cd36f-0x1cd3a3); flags 0x10 (0x1cd3b0/0x1cd3b6); blend channel set(0x3c0, ..., 0) and
// unknown80 = 0 (0x1cd3e2-0x1cd3ee); flags &= ~1 (0x1cd3f6); name compare before the store, request guard and
// the three loop clears (0x1cd3fa-0x1cd4b9); state_flags & 4 fallback (0x1cd4ce). No hand-expanded helper bodies
// (channel_loop_clear, channel_valid, PIN are used), no in-body pragma, no new include, helper or double local.
// MiniMax D (11-17): no new form, no lower score (210): the frame of 0x38 still agrees, but retail keeps the
// transition id at [esp+0x14] and the animation id at [esp+0x18] while ours has the two the other way round
// (declaring the transition id first at the top was screened before and gave 342-344), and retail keeps the
// blend byte in memory across the 0xe6800 call while ours keeps it in a register, because the build knows
// that callee's clobbers; that callee's file is not this lane's to edit.
// MiniMax I (11-17): no new form, no lower score (210): the only difference left is the order of the two ids in
// the frame - this build puts the animation id at esp+0x14 and the transition id at esp+0x18, retail puts the
// transition id at esp+0x14 and the animation id at esp+0x18 - while both ids' -1 stores, the lookup's out-parameter
// and the blend id's home agree; declaring the transition id at the function scope measures 357 (the fraction then
// takes esp+0x14), with the same re-init 250, and the transition id declared after the animation id measures 250.
// M3 lane 8 r183 (fast with the nine callers, 0x1ccda0 and 0x1cc3f0, control 210): transition_find's blend output as `bool &`
// (declaration, definition and call; 0x1ccda0 still matched) 210; the transition id read through a const reference at the
// channel test 210; the 0x200 test with its arms swapped 214 (1133 B); the fraction declared where first set 210; the
// transition set declared at its initialisation 210. Diagnostic: the channel test's id use dropped 337 (1116 B): the id
// then stays in memory, but the id slots stay swapped and the set still does not take EBP. All restored.
// Completed transition ID copied at admission, before clear or after start 210/210/210.
// Completed primary ID copied at lookup return alone or with those transition
// copies 210/210/210/210. Primary copy before clear with transition copy at
// admission 210. All leave the ID slots and blend lifetime where they were.
// Lookup inputs from the set, mode, weapon fields or all name fields 210/210/210/210;
// resolved set or resolved mode and set at lookup return 210/210; complete record lookup with
// early resolved set 210; early set under split lookup admission 210. NOT kept: all forms
// tie with the primary and transition ID homes exchanged.
// M3 lane 1 r122 (fast with 0x1ccda0, 0x1cccd0, 0x1ccae0, 0x10f430, 0x1cb410; aligned counts in brackets, the kept body
// [103]; nothing kept: every form below is closer when aligned and further by position). The set parameter refreshed
// from names.set in the block and used at the three later places 210 (same code); the channel byte held in the blend
// variable 210 (same code); both 210; the set parameter at the three later uses re-measured 277 [99] (1133 bytes;
// callers 0x10f430 80, 0x10fd40 13, 0x119650 8, 0x118510 2, 0x1ccae0 2). Lead A (round 64's form rebuilt): bool
// result, `result = true;` as the first statement after channel_start, `if (!result && (state_flags & 4)) result =
// true; if (result) {...}` and three name-change bools joined with `||` after the store 249 [51] (1140 bytes, first
// +0xb1: the two id homes, the memory transition id and the set in EBP are then retail's); A with the request's three
// arguments read through stored_names (retail reads `[eax]`, `[edi+0x78]`, `[edi+0x74]`) 249 [47]; A2 with the
// transition channel in a pointer local declared between the id test and the graph test (`if (id != NONE) { channel =
// &channels[2]; if (graph != NONE && channel->set(...))`, three uses) 282 [35] (1142 bytes: the channel byte is then
// memory-resident with retail's `and byte ptr [esp+0x50], al` and `lea ebx, [edi+0x40]` sits where retail has it); the
// pointer declared inside the two-test block instead 282 [48] with the frame position local; the frame position in a
// local declared after the frame count 261 [59] (retail's single load and in-place pin, but position and count
// exchange XMM0/XMM1), declared before it 269 [62] (1153 bytes, loaded ahead of the call), with the pointer 206 [50]
// (1155 bytes; positional accident, not kept). The store through a forced inline helper called on the success path and
// on the flag-4 path 560/547 (two copies, not merged); the same helper not forced 442/448 (out of line); the flag-4
// block written after the return and reached by goto (diagnostic) 249 [47], the same code as A2: the build lays that
// block ahead of the store whatever the text order, where retail has it after the epilogue jumping back; an early
// `return result` form without gotos 272 [112]. Diagnostics, reverted at once: the blend query's callee swapped for an
// undefined extern 253 [33]: the blend byte then goes through memory around that call but in EBX, not retail's
// load/store at every `&=`; the blend byte's address also passed to an undefined extern 331 [96] (frame 0x3c). Left on
// A2 [35]: the blend byte is never register-resident in retail (stored before the 0xe6800 call, reloaded after it and
// at the join, state_flags reloaded into EBX right after that call, one shared `and byte ptr [edi+0x6e], 0xfe`), the
// frame position's single load, the flag-4 block's place, and a scratch-register rotation at the 0x1cccd0 and 0x1cb410
// calls. BLOCKED-BY 0xe6800 for the blend byte only if retail built that callee as opaque; not shown.
// M3 lane 15 r104 (fast with 0x1ccda0, which stays MATCH, and 0x1cc3f0 348; control 210, all restored): the animation id
// renamed to sort after the transition id 210 (same code; a re-measure of lane 10 r13's form); the blend flag reused for
// the unflagged state (retail's [esp+0x50] holds both) 210 (same code: ours still keeps it in DL); `result = true;` first
// in the channel_start success block (retail's `mov byte ptr [esp + 0x54], 1` there) 274 (1139 B); a named pointer to
// channels[2] declared after the unflagged state and used for its set, frame and unflagged calls 237 (1133 B as retail,
// first +0xa9: both id slots and the transition id's memory compares as retail; left: names.set not cached in EBP and
// the blend flag kept in DL across 0xe6800); on that, a resolved-set local for the three later set uses 237 (same code);
// on that, transition_find's blend parameter read through its address (diagnostic) 285 (1136 B; the transition id
// moves to [esp+0x4c]).
// M3 lane 13 r120 (fast with 0x1ccda0/0x1cccd0/0x1ccae0/0x1cb410, plus 0x10ee20 and 0xe6800 where the definition of
// 0xe6800 was touched; control 210, body unchanged): HELD (needs the owner's OK), build/r120/held-1cd070.patch: MATCH
// in the plain check and in a full check (12233 matched, nothing lost; 0xe6800, 0x10ee20 and the callers 0x10f430,
// 0x10fd40, 0x119650, 0x118510, 0x1ccae0 still match) with `inline` on the definition of 0xe6800 (a contributor's
// function: retail expands that body once inside 0x10ee20 and calls it there and here, so it is a header inline in
// retail; with it this caller no longer keeps the blend byte in EDX across that call) and this body: bool result with
// `result = true;` first in the channel_start block; the unknown6e update written once after the 0x200 block (`if
// (blend) |= 1; else &= ~1;`, which keeps the blend byte in memory with retail's load/and/store at each `&=`);
// `real frame_position = channels[0].frame_position;` declared after the frame count and pinned in place
// (`frame_position = PIN(frame_position, 0.0f, frame_count); fraction = frame_position / frame_count;`: position in
// XMM0, count in XMM1); the transition channel and the blend channel started through the existing channel_start
// (flags 0x15 and 0x3c0) in place of its body written out (retail's `lea ebx, [edi + 0x40]` before the graph test and
// the id copied without a rotation step); the tail as `if (!result) { if (state_flags & 4) result = true; } else {
// flags &= ~1; } if (result) {...} return result;` (the flag block then sits after the return and jumps back to the
// store); the store as three bools (`mode_changed`, `class_changed`, `type_changed`) around `stored_names = names;`
// joined with `||`, the request reading stored_names. Without the 0xe6800 change that body is 305 (1118 B, first
// +0xb1: the blend byte stays in DL with no stores), so it is not kept here.
// Steps on the way, each with the 0xe6800 change unless noted: lead A rebuilt without it 249 (1140 B); with it 253;
// + shared unknown6e update 244 (without it 276); + the channel pointer between the id and graph tests 248; + frame
// position local declared after the count 106 (after a short count 243); tail as `result = result || (state_flags &
// 4)` or an early `return result` 260 (the frame changes), `if (!result) result = (state_flags & 4) != 0;` 346,
// `if (result || (state_flags & 4)) { result = true; ...}` 202, nested `if (!result) { if (state_flags & 4) ...}`
// with the flag clear left in the block 106, a second flag for the store 106; the flag clear in the else-arm 19; arms
// the other way round (`if (result) flags &= ~1; else if (state_flags & 4) ...`) 87; a two-use last-frame helper
// 261/276; the position pinned in place 2; second frame-count read through an animation pointer local, a count
// local or with the product swapped 19 each (same code); a named `state_flags & 0x20` local 4; a named variant id
// 168; `state_flags & 0x60` at the second test 360; the fraction test first 10; channel_start for the transition
// channel alone MATCH, for the blend channel alone 2, both MATCH; on the match: one changed bool with `||` 301, byte
// result 21, the frame count through an animation pointer local MATCH (not needed).
// In-tree forms without the 0xe6800 change (64 subsets of the six changes on main's body, then 16 result/tail/store
// combinations): every subset is above 210 except bool result + the else-arm tail + three bools 168 (1139 B) and
// the same with byte result 184; those two need the flag clear moved to the else-arm and three one-use bools (with
// the clear left in the block 249/256, with one bool 272-279), so neither is kept without the match.
// S2 harvest (11-27): with `inline` on 0xb6760's definition (see 0x166d75) the link expands the channel-2 test below
// (150); retail calls it there. The redundant `result &&` guard ahead of that call (lane 14 r134's held bool) keeps the
// call out of line as retail: MATCH.
// @retail 0x1cd070
bool s_animation_state::animation_set(long mode, long weapon_class, long weapon_type, long set, long state_flags, long channel_flags)
{
	bool result = false;
	s_animation_names names;
	s_animation_names found;
	c_type_709360 animation_id;

	names.mode = mode;
	names.weapon_class = weapon_class;
	names.weapon_type = weapon_type;
	names.set = set;
	bool loops = true;
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
		bool blend = false;
		real fraction;
		long transition_set;

		if (!(state_flags & 0x200))
		{
			transition_id = transition_find(names.mode, names.set, &blend);
			blend &= !(flags & 1);
			blend &= (((dword)state_flags >> 10) & (channel_valid(&channels[0]) && !function_0e6800(&channels[0]))) & 1;
		}
		if (blend)
		{
			unknown6e |= 1;
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
				real frame_position = channels[0].frame_position;

				if (frame_count > 0.0f)
				{
					frame_position = PIN(frame_position, 0.0f, frame_count);
					fraction = frame_position / frame_count;
					if (state_flags & 0x40)
					{
						fraction = 1.0f - fraction;
					}
					fraction = PIN(fraction, 0.0f, 1.0f);
				}
			}
			c_type_709360 blend_id;
			transition_set = NONE;
			if (!(state_flags & 0x100))
			{
				blend_id = transition_lookup(names.set, names.mode, names.weapon_class, names.weapon_type, &transition_set);
			}
			channels_clear();
			if (channel_start(&channels[0], animation_id, found.set, NONE, NONE, 0, channel_flags))
			{
				bool unflagged;

				result = true;
				unflagged = channels[0].is_unflagged0() & loops;
				if (transition_id.index != NONE)
				{
					if (channel_start(&channels[2], transition_id, names.set, NONE, NONE, 3, 0x15))
					{
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
				if (result && !function_0b6760((s_index_pair const *)&channels[2]))
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
				if (blend_id.index != NONE)
				{
					if (channel_start(&channels[1], blend_id, transition_set, NONE, NONE, 0, 0x3c0))
					{
						unknown80 = 0.0f;
					}
				}
			}
		}
	}
	if (!result)
	{
		if (state_flags & 4)
		{
			result = true;
		}
	}
	else
	{
		flags &= ~1;
	}
	if (result)
	{
		bool mode_changed = stored_names.mode != names.mode;
		bool class_changed = stored_names.weapon_class != names.weapon_class;
		bool type_changed = stored_names.weapon_type != names.weapon_type;

		stored_names = names;
		if (mode_changed || class_changed || type_changed)
		{
			if (graph_tag_index != NONE && !(flags & 1))
			{
				resources_request(stored_names.mode, stored_names.weapon_class, stored_names.weapon_type, true, true);
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

bool animation_graph_fits_model(long graph_tag_index, long model_tag_index);

// @retail 0x1cb0d0
bool s_animation_state::initialize(long graph_tag_index, long model_tag_index, bool flag)
{
	bool result = false;
	if (model_tag_index == NONE || animation_graph_fits_model(graph_tag_index, model_tag_index))
	{
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
		for (long i = 0; i < 3; i++)
		{
			channels[i].clear();
		}
		animation_set(0x7000001, 0x7000001, 0x7000001, 0x7000001, 0x317, 0x3f);
		s_animation_bits *bits = &unknown64;
		bits->unknown1 = 0;
		bits->unknown0 = 0;
		bits->unknown3 = 0;
		bits->unknown2 = 1;
		unknown60.unknown1 = 0;
		unknown60.unknown0 = 0;
		unknown60.unknown3 = 0;
		unknown6e = 0;
		unknown80 = 0.0f;
		result = true;
	}
	return result;
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
	return graph_get()->animation_by_name(name);
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
	function_1d9470((s_1d9240 const *)&unknown64, count, mask, targets, orientations);
}

// @retail 0x1cb5f0
bool function_1cb5f0(s_animation_state *state, real seconds, long node_count, s_blend_orientation *orientations,
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
			function_1cb5f0(this, 0.267f, node_count, orientations, targets);
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
inline bool s_animation_state::channel_play(c_animation_channel *channel, c_type_709360 animation_id, dword channel_flags)
{
	bool result = false;

	if (graph_tag_index != NONE)
	{
		result = channel_start(channel, animation_id, NONE, NONE, NONE, NONE, channel_flags);
	}
	return result;
}

// @retail 0x1cb4c0
bool s_animation_state::channel_play_named(c_animation_channel *channel, long name, dword channel_flags)
{
	bool result = false;

	if (graph_tag_index != NONE)
	{
		c_type_709360 animation_id = graph_get()->animation_by_name(name);

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
	bool frozen = object_index != NONE && ((s_animated_object_header *)g_4e0300->data)[object_index & 0xffff].object->unknownb3 > 0;

	if (!frozen)
	{
		if (g_46fbf5 && channel_valid(&channels[2]))
		{
			channels[2].movement_rate_get(vector, value);
		}
		else if (g_46fbf4 && channel_valid(&channels[1]) && unknown80 >= 0.9999f)
		{
			channels[1].movement_rate_get(vector, value);
		}
		else if (g_46fbf4 && channel_valid(&channels[1]) && !(0.0001f >= unknown80) && !(unknown80 >= 0.9999f))
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
	else if (g_46fbf4 && channel_valid(&channels[1]) && !(0.0001f >= unknown80) && !(unknown80 >= 0.9999f))
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
bool s_animation_state::overlay_play_for_class(c_animation_channel *channel, dword channel_flags, long weapon_class,
	long weapon_type, long set)
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
				result = true;
			}
			else
			{
				result = channel_start(channel, animation_id, name, NONE, NONE, kind, channel->flags);
			}
		}
		if (!result)
		{
			channel->clear();
		}
	}
	return result;
}

// @retail 0x1cb9d0
c_type_709360 s_animation_state::overlay_find_for_class(long weapon_class, long weapon_type, long set)
{
	c_type_709360 none;

	if (graph_tag_index != NONE)
	{
		c_animation_channel channel;

		if (overlay_play_for_class(&channel, 0, weapon_class, weapon_type, set))
		{
			none = channel.animation_id;
		}
	}
	return none;
}

// @retail 0x1cbad0
c_type_709360 s_animation_state::overlay_or_animation_get(long weapon_class, long set, long weapon_type)
{
	c_type_709360 animation_id;

	if (weapon_class == 0x7000101 || weapon_class == NONE)
	{
		weapon_class = unknown74;
	}
	if (weapon_type == 0x7000101 || weapon_type == NONE)
	{
		weapon_type = unknown78;
	}
	animation_id = overlay_find_for_class(weapon_class, weapon_type, set);
	if (animation_id.index == NONE)
	{
		animation_id = animation_get(set, weapon_class, weapon_type);
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
	c_type_709360 animation_id;
	s_graph_tag *graph = graph_get();
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
		dword channel_flags = 0x3f;
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
bool s_animation_state::play(c_type_709360 animation_id, dword channel_flags)
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
            weight = 1.0f - fraction;
            if (weight < 0.0f) weight = 0.0f;
            else if (weight > 1.0f) weight = 1.0f;
            rate = PIN(rate, 0.0f, 1.0f);
        }
        real smoothed = state->unknown80 * 0.9f;
        smoothed += weight * 0.1f;
        state->unknown80 = smoothed;
        if (rate > 0.9f)
            rate = 1.0f;
        primary->rate = rate;
        result = true;
    }
    else
        state->unknown80 = 0.0f;
    }
    return result;
}
