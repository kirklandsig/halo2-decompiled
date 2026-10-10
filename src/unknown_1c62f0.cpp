// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1C62F0.CPP: the animation channels (0x1c62f0..0x1c7450) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_0259d0.h"
#include "unknown_xd56787.h"
#include "unknown_11cb00.h"
#include <float.h>
#include <string.h>

// @retail 0x1c62f0
c_animation_channel::c_animation_channel()
{
	reset();
}

// @retail 0x1c6340
void c_animation_channel::reset()
{
	graph_tag_index = NONE;
	short *local_0 = (short *)&animation_id;
	local_0[0] = NONE;
	local_0[1] = NONE;
	frame_position = 0.0f;
	unknown10 = 0;
	unknown11 = 0;
	flags = 0;
	rate = 1.0f;
	unknown14 = 0;
	unknown16 = 0;
	unknown08 = NONE;
	unknown0e = NONE;
	unknown0c = NONE;
	unknown0d = NONE;
}

// @retail 0x1c6390
c_animation_channel *c_animation_channel::copy_from(c_animation_channel const *other)
{
	graph_tag_index = other->graph_tag_index;
	animation_id = other->animation_id;
	unknown08 = other->unknown08;
	unknown0c = other->unknown0c;
	unknown0d = other->unknown0d;
	unknown0e = other->unknown0e;
	unknown10 = other->unknown10;
	unknown11 = other->unknown11;
	flags = other->flags;
	unknown14 = other->unknown14;
	unknown16 = other->unknown16;
	rate = other->rate;
	frame_position = other->frame_position;
	return this;
}

// @retail 0x1c63f0
void c_animation_channel::clear()
{
	graph_tag_index = NONE;
	short *local_0 = (short *)&animation_id;
	local_0[0] = NONE;
	local_0[1] = NONE;
	frame_position = 0.0f;
	unknown10 = 0;
	unknown11 = 0;
	flags = 0;
	rate = 1.0f;
	unknown14 = 0;
	unknown16 = 0;
	unknown08 = NONE;
	unknown0c = NONE;
	unknown0d = NONE;
	unknown0e = NONE;
}

// @retail 0x1c6470
bool c_animation_channel::set(long graph_tag_index, word flags, c_type_709360 animation_id, long unknown08,
	char unknown0c, char unknown0d, char unknown0e)
{
	if (graph_tag_index != NONE && animation_id.index != NONE)
	{
		this->graph_tag_index = graph_tag_index;
		this->animation_id.graph_index = NONE;
		this->animation_id.index = NONE;
		this->unknown10 = 0;
		byte *local_0 = &this->unknown10;
		local_0[0] = 0;
		local_0[1] = 0;
		this->unknown14 = 0;
		this->unknown16 = 0;
		this->unknown08 = unknown08;
		this->unknown0c = unknown0c;
		this->flags = flags;
		this->unknown0d = unknown0d;
		this->unknown0e = unknown0e;
		this->frame_position = 0.0f;
		this->rate = 1.0f;
		this->animation_id = animation_id;
		this->unknown10 = (flags >> 14) & 2;
		return true;
	}
	return false;
}

// @retail 0x1c6bb0
s_graph_tag *c_animation_channel_get_graph(c_animation_channel const *channel)
{
	s_graph_tag *graph = graph_tag_get(channel->graph_tag_index);

	if (channel->animation_id.graph_index != NONE)
	{
		return graph_inherited_get(graph, channel->animation_id.graph_index);
	}
	return graph;
}

// @retail 0x1c7380
void c_animation_channel_data_get(c_animation_channel const *channel, s_animation_data *data)
{
	c_type_709360 animation_id = channel->animation_id;

	function_1ddb40(data, graph_tag_get(channel->graph_tag_index), animation_id);
}

/* the codecs' readers of an animation's data (unknown_20aa70.cpp, which
   names s_animation_data s_anim_data) */
struct s_anim_data;
void function_20aa70(vector3f *out, s_anim_data *data, long index, real *w);
void function_20ad40(s_anim_data *data, vector3f *a, vector3f *b, long index);

/* the sampling state the codecs' decoders read (unknown_279d80.cpp,
   unknown_28c510.cpp) */

// @retail 0x1c73a0
void c_animation_channel_node_position_get(c_animation_channel const *channel, point3f *position, real frame,
	short node_index)
{
	s_animation_data data;
	real_quaternion_transform transform;
	byte *animated_data;
	long frame_index;

	c_animation_channel_data_get(channel, &data);
	animated_data = data.data + data.sizes->static_data_size;
	frame_index = real_truncate(frame);
	long local_0 = *(volatile short const *)&node_index;
	g_sampling_settings.frame_index = frame_index;
	g_sampling_settings.next_frame_index = frame_index;
	g_sampling_settings.field_30 = (s_animation_data *)animated_data;
	g_sampling_settings.frame_fraction = 0.0f;
	g_5044c0 = (s_animation_output *)&transform;
	g_5044b8 = local_0;
	g_47fb18[*animated_data].samplers[0].translation();
	*position = transform.position;
}

// @retail 0x1c6dc0
void c_animation_channel_movement_get(c_animation_channel const *channel, vector3f *vector, real *value, long frame)
{
	vector->i = 0.0f;
	vector->j = 0.0f;
	vector->k = 0.0f;
	*value = 0.0f;
	if (channel->animation_id.index != NONE)
	{
		s_animation_data data;

		c_animation_channel_data_get(channel, &data);
		function_20aa70(vector, (s_anim_data *)&data, frame, value);
	}
}

// @retail 0x1c6c80
bool c_animation_channel_frame_sample(c_animation_channel const *channel, real frame, vector3f *position, vector3f *delta)
{
	bool result = false;

	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		long frame_index = real_truncate(frame);
		s_animation_data data;

		c_animation_channel_data_get(channel, &data);
		function_20ad40((s_anim_data *)((dword)&data & ~3UL), delta, position, frame_index);
		result = true;
	}
	return result;
}

#define PIN(value, lower, upper) ((value) < (lower) ? (lower) : (value) > (upper) ? (upper) : (value))

// @retail 0x1c69b0
void c_animation_channel::update_events()
{
	long i = 0;

	unknown16 = 0;
	if (animation_id.index != NONE)
	{
		s_animation *animation = function_1c6440();
		real position = frame_position;
		real end = rate > 0.0f ? (real)animation->frame_count + 1.0f : 0.0f;
		real lower;
		real upper;
		long event_count;

		lower = position > end ? end : position;
		upper = position > end ? position : end;
		event_count = animation->event_count;
		for (i = 0; i < event_count; i++)
		{
			s_animation_event *event = &animation->events[i];
			real frame = (real)event->frame;

			if (lower == frame_position)
			{
				if (frame > lower && upper >= frame)
				{
					unknown16 |= 1 << event->type;
				}
			}
			else if (frame >= lower && upper > frame)
			{
				unknown16 |= 1 << event->type;
			}
		}
	}
}

// @retail 0x1c6440
s_animation *c_animation_channel::function_1c6440() const
{
	s_animation *animation = NULL;

	if (animation_id.index != NONE)
	{
		animation = function_1daea0(graph_tag_get(graph_tag_index), animation_id);
	}
	return animation;
}

// @retail 0x1c6500
void c_animation_channel::set_frame_last()
{
	s_animation *animation = function_1c6440();
	real frame = (real)(animation->frame_count - 1) + 0.0001f;

	if (0.0f > frame)
	{
		frame = 0.0f;
	}
	frame_position = frame;
	unknown11 |= 0xa;
	unknown10 = 0;
}

// @retail 0x1c6560
void c_animation_channel::set_frame_position(real frame)
{
	s_animation *animation = function_1c6440();

	frame_position = PIN(frame, 0.0f, (real)animation->frame_count - 0.0001f);
	if ((flags & 1) && !(unknown11 & 9))
	{
		if (g_510c54->rate * rate * 30.0f + frame_position >= (real)animation->frame_count - 0.0001f)
		{
			unknown11 |= 2;
		}
		else
		{
			unknown11 &= ~2;
		}
	}
	update_events();
}

// @retail 0x1c6620
void c_animation_channel::set_frame_ratio(real ratio)
{
	s_animation *animation = function_1c6440();
	real last_frame = (real)(animation->frame_count - 1) + 0.0001f;
	real frame = last_frame * ratio;

	set_frame_position(PIN(frame, 0.0f, last_frame));
}

// @retail 0x1c68c0
void c_animation_channel::update(s_animation_state *state, animation_event_callback callback, long user)
{
	unknown14 = 0;
	unknown16 = 0;
	if (unknown10 > 0)
	{
		unknown10--;
	}
	else if ((flags & 1) && !(unknown11 & 9))
	{
		c_animation_channel_advance(this, g_510c54->rate * rate * 30.0f + frame_position, state, callback, user);
	}
}

// @retail 0x1c6920
void c_animation_channel::set_frame_ratio_and_advance(real ratio, s_animation_state *state,
	animation_event_callback callback, long user)
{
	s_animation *animation = function_1c6440();
	real last_frame;
	real frame;

	unknown10 = 0;
	last_frame = (real)(animation->frame_count - 1) + 0.0001f;
	frame = last_frame * ratio;
	c_animation_channel_advance(this, PIN(frame, 0.0f, last_frame), state, callback, user);
}

// @retail 0x1c6e30
real c_animation_channel::get_frame_ratio() const
{
	real result = 0.0f;

	if (animation_id.index != NONE)
	{
		real frame_count = (real)function_1c6440()->frame_count;

		if (frame_count > 0.0f)
		{
			real ratio = frame_position / frame_count;

			result = PIN(ratio, 0.0f, 1.0f);
		}
	}
	return result;
}

// @retail 0x1c6ea0
real c_animation_channel::get_duration() const
{
	real result = 0.0f;

	if (animation_id.index != NONE)
	{
		result = (real)function_1c6440()->frame_count * (1.0f / 30.0f);
	}
	return result;
}

// @retail 0x1c6ee0
real c_animation_channel::get_event_time() const
{
	real result = -1.0f;

	if (animation_id.index != NONE)
	{
		long frame = animation_event_frame_get(function_1c6440(), 0);

		if (frame != NONE)
		{
			result = (real)frame * (1.0f / 30.0f);
		}
	}
	return result;
}

// @retail 0x1c7300
bool c_animation_channel::is_unflagged0() const
{
	bool result = true;

	if (graph_tag_index != NONE && animation_id.index != NONE)
	{
		result = !TEST_FIELD_BIT(function_1c6440()->flag0);
	}
	return result;
}

// @retail 0x1c7340
bool c_animation_channel::is_unflagged6() const
{
	bool result = true;

	if (graph_tag_index != NONE && animation_id.index != NONE)
	{
		result = !TEST_FIELD_BIT(function_1c6440()->flag6);
	}
	return result;
}

/* the animation sampler and the node masks (unknown_279d80.cpp) */
void function_279d80(s_graph_tag *graph, c_type_709360 animation_id, long node_count, real frame, real weight,
	s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms, bool interpolate);
void node_mask_and(dword *mask, dword const *other);

extern dword g_55e590[8];

// @retail 0x1c6f60
void c_animation_channel::sample(real weight, dword const *node_mask, long node_count, real_quaternion_transform *transforms)
{
	s_animation *animation = function_1c6440();
	bool interpolate;
	dword const *graph_mask;
	dword const *mask;
	s_graph_inheritance *inheritance;

	if (unknown10 && animation->type)
	{
		function_1dd9d0(graph_tag_get(graph_tag_index), animation_id);
		return;
	}
	interpolate = (flags >> 6) & 1;
	if ((flags & 1) && rate != 0.0f && (rate != 1.0f || g_510c54->field_2_3 != 30))
	{
		interpolate = true;
	}
	graph_mask = NULL;
	if (flags & 0x800)
	{
		graph_mask = graph_tag_get(graph_tag_index)->node_mask5c;
	}
	else if (flags & 0x400)
	{
		graph_mask = graph_tag_get(graph_tag_index)->node_mask7c;
	}
	mask = NULL;
	if (graph_mask || node_mask)
	{
		if (graph_mask && node_mask && graph_mask != node_mask)
		{
			memcpy(g_55e590, graph_mask, sizeof(g_55e590));
			node_mask_and(g_55e590, node_mask);
			mask = g_55e590;
		}
		else if (graph_mask)
		{
			mask = graph_mask;
		}
		else if (node_mask)
		{
			mask = node_mask;
		}
	}
	inheritance = NULL;
	if (animation_id.index != NONE && animation_id.graph_index != NONE)
	{
		inheritance = function_1daff0(graph_tag_get(graph_tag_index), animation_id);
	}
	function_279d80(graph_tag_get(graph_tag_index), animation_id, node_count, frame_position, weight, inheritance, mask, transforms,
		interpolate);
}
/* the graph's sound or effect an animation's event names, or NULL */
inline s_graph_entry *graph_sound_get(s_graph_tag *graph, long index)
{
	s_graph_entry *result = NULL;

	if (index != NONE)
	{
		result = &graph->entries[index];
	}
	return result;
}

inline s_graph_entry *graph_effect_get(s_graph_tag *graph, long index)
{
	s_graph_entry *result = NULL;

	if (index != NONE)
	{
		result = &graph->effects[index];
	}
	return result;
}

// @retail 0x1c7450
long animation_events_dispatch(s_graph_tag *graph, s_animation *animation, dword flags, real frame_a, real frame_b,
	animation_event_callback callback, long user)
{
	long events = 0;
	real lower = frame_a > frame_b ? frame_b : frame_a;
	real upper = frame_a > frame_b ? frame_a : frame_b;

	if (!(flags & 0x80))
	{
		long count = animation->event_count;
		long i;

		for (i = 0; i < count; i++)
		{
			s_animation_event *event = &animation->events[i];
			real frame = (real)event->frame;

			if (frame >= lower && upper > frame)
			{
				events |= 1 << event->type;
				if (callback)
				{
					s_animation_frame_event data;

					data.category = 0;
					data.frame = event->frame;
					data.tag_index = NONE;
					data.type = event->type;
					data.flags = 0;
					data.marker_name = 0x600008a;
					callback(user, frame, &data);
				}
			}
		}
	}
	if (!(flags & 0x100))
	{
		long count = animation->sound_event_count;
		long i;

		for (i = 0; i < count; i++)
		{
			s_animation_sound_event *event = &animation->sound_events[i];
			real frame = (real)event->frame;

			if (frame >= lower && upper > frame && event->sound != NONE)
			{
				s_graph_entry *sound = graph_sound_get(graph, event->sound);

				if (sound && sound->tag_index != NONE && callback)
				{
					word sound_flags = sound->flags;
					bool play;

					if ((sound_flags & 2) && !(flags & 0x2000))
					{
						continue;
					}
					if ((sound_flags & 4) && !(flags & 0x1000))
					{
						continue;
					}
					play = true;
					if ((sound_flags & 0x10) && frame_a > frame_b)
					{
						play = false;
					}
					if ((sound_flags & 0x20) && frame_b > frame_a)
					{
						continue;
					}
					if (play)
					{
						s_animation_frame_event data;

						data.category = 1;
						data.tag_index = sound->tag_index;
						data.type = NONE;
						data.marker_name = event->marker_name;
						data.flags = sound_flags;
						data.frame = event->frame;
						callback(user, frame, &data);
					}
				}
			}
		}
	}
	if (!(flags & 0x200))
	{
		long count = animation->effect_event_count;
		long i;

		for (i = 0; i < count; i++)
		{
			s_animation_event *event = &animation->field_60[i];
			real frame = (real)event->frame;

			if (frame >= lower && upper > frame && event->type != NONE)
			{
				s_graph_entry *effect = graph_effect_get(graph, event->type);

				if (effect && effect->tag_index != NONE)
				{
					word effect_flags = effect->flags;
					bool play;

					if ((effect_flags & 2) && !(flags & 0x2000))
					{
						continue;
					}
					if ((effect_flags & 4) && !(flags & 0x1000))
					{
						continue;
					}
					play = true;
					if ((effect_flags & 0x10) && frame_a > frame_b)
					{
						play = false;
					}
					if ((effect_flags & 0x20) && frame_b > frame_a)
					{
						continue;
					}
					if (play && callback)
					{
						s_animation_frame_event data;

						data.category = 2;
						data.tag_index = effect->tag_index;
						data.type = NONE;
						data.frame = event->frame;
						data.flags = effect_flags;
						data.marker_name = 0x600008a;
						callback(user, frame, &data);
					}
				}
			}
		}
	}
	return events;
}

/* Parameter access through local pointers preserves the stack-only entry convention. */
// @retail 0x1c66a0
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user)
{
	c_animation_channel *const *local_0 = &channel;
	real const *local_1 = &frame;
	s_animation_state *const *local_2 = &state;
	animation_event_callback const *local_3 = &callback;
	long const *local_4 = &user;
	c_type_709360 animation_id = (*local_0)->animation_id;
	s_graph_tag *graph = c_animation_channel_get_graph(*local_0);
	s_animation *animation = (*local_0)->function_1c6440();
	real position = (*local_0)->frame_position;
	real last_frame;
	real next_frame;
	long events;

	(*local_0)->unknown14 = 0;
	(*local_0)->unknown16 = 0;
	next_frame = *local_1;
	last_frame = (real)animation->frame_count - 0.0001f;
	if (0.0f > next_frame)
	{
		next_frame = 0.0f;
	}
	(*local_0)->unknown11 &= ~0xe;
	events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, next_frame, *(animation_event_callback volatile *)local_3, *local_4);
	if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
	{
		(*local_0)->unknown14 |= events;
		if (next_frame >= last_frame)
		{
			events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, FLT_MAX, *local_3, *local_4);
			if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
			{
				(*local_0)->unknown14 |= events;
				if (TEST_FIELD_BIT((*local_0)->flag1))
				{
					if (((*local_0)->flags & 0x20) && animation->loop_frame_index == 0)
					{
						animation_id = (*local_2)->variant_get((*local_0)->animation_id);
						(*local_0)->animation_id = animation_id;
						if ((*local_0)->flags & 8)
						{
							(*local_0)->rate = 1.0f;
						}
						animation = (*local_0)->function_1c6440();
					}
					position = (real)animation->loop_frame_index;
					next_frame = next_frame - last_frame + position;
					events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, next_frame, *local_3, *local_4);
					if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
					{
						(*local_0)->unknown14 |= events;
						(*local_0)->unknown11 |= 4;
					}
				}
				else
				{
					(*local_0)->unknown11 |= 8;
					next_frame = (real)animation->frame_count - 0.0001f;
				}
			}
		}
		if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
		{
			(*local_0)->set_frame_position(next_frame);
		}
	}
}

// @retail 0x1c6c00
bool c_animation_channel::velocity_get(vector3f *delta, vector3f *velocity) const
{
	bool result = false;

	if (graph_tag_index != NONE && animation_id.index != NONE)
	{
		real scale = rate * 30.0f;
		vector3f position;

		c_animation_channel_frame_sample(this, frame_position, &position, delta);
		velocity->i = position.i * scale;
		velocity->j = position.j * scale;
		velocity->k = position.k * scale;
		result = true;
	}
	return result;
}

// @retail 0x1c6d00
void c_animation_channel::movement_rate_get(vector3f *vector, real *value) const
{
	*vector = *g_4687a4;
	*value = 0.0f;
	if (animation_id.index != NONE)
	{
		real movement_value;
		vector3f movement;
		long frame = real_truncate(frame_position);
		real scale = rate * 30.0f;

		c_animation_channel_movement_get(this, &movement, &movement_value, frame);
		vector->i = movement.i * scale;
		vector->j = movement.j * scale;
		vector->k = movement.k * scale;
		*value = movement_value * scale;
	}
}

/* the samplers of an aiming screen and of a frame ratio, and the node masks'
   combination (unknown_279d80.cpp) */
void function_279e40(s_aiming_screen const *screen, s_graph_tag *graph, c_type_709360 animation_id, long node_count,
	real yaw, real pitch, real weight, s_graph_inheritance *inheritance, dword const *node_mask,
	real_quaternion_transform *transforms);
void function_27a060(s_graph_tag *graph, c_type_709360 animation_id, long node_count, real ratio, real weight,
	s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms);
dword const *node_masks_combine(dword const *mask, dword const *other);

/* the node mask the channel's flags select in its graph, or NULL */
inline dword const *c_animation_channel_graph_mask(c_animation_channel const *channel)
{
	dword const *mask = NULL;

	if (channel->flags & 0x800)
	{
		mask = graph_tag_get(channel->graph_tag_index)->node_mask5c;
	}
	else if (channel->flags & 0x400)
	{
		mask = graph_tag_get(channel->graph_tag_index)->node_mask7c;
	}
	return mask;
}

PRIVATE __forceinline dword const *function_1c7101(c_animation_channel const *arg_0)
{
	dword const *local_0 = NULL;
	if (arg_0->flags & 0x800)
		local_0 = graph_tag_get(arg_0->graph_tag_index)->node_mask5c;
	else if (arg_0->flags & 0x400)
		local_0 = graph_tag_get(arg_0->graph_tag_index)->node_mask7c;
	return local_0;
}

// @retail 0x1c7100
void c_animation_channel::sample_aiming(real yaw, real pitch, real weight, dword const *node_mask, long node_count,
	real_quaternion_transform *transforms)
{
	dword const *const *local_0 = &node_mask;
	if (animation_id.index != NONE)
	{
		s_aiming_screen const *screen = (s_aiming_screen const *)function_1daf30(graph_tag_get(graph_tag_index), animation_id);

		if (screen)
		{
			dword const *graph_mask = function_1c7101(this);
			dword const *mask = NULL;
			s_graph_inheritance *inheritance;

			if (graph_mask || *local_0)
			{
				mask = node_masks_combine(graph_mask, *local_0);
			}
			inheritance = NULL;
			if (animation_id.index != NONE && animation_id.graph_index != NONE)
			{
				inheritance = function_1daff0(graph_tag_get(*(volatile long const *)&graph_tag_index), animation_id);
			}
			function_279e40(screen, graph_tag_get(*(volatile long const *)&graph_tag_index), animation_id, node_count, yaw, pitch, weight,
				inheritance, mask, transforms);
		}
	}
}

// @retail 0x1c7200
void c_animation_channel::sample_ratio(real ratio, real weight, long node_count, real_quaternion_transform *transforms,
	dword const *node_mask)
{
	dword const *graph_mask = c_animation_channel_graph_mask(this);
	dword const *mask = NULL;
	s_graph_inheritance *inheritance;

	if (graph_mask || node_mask)
	{
		mask = node_masks_combine(graph_mask, node_mask);
	}
	inheritance = NULL;
	if (animation_id.index != NONE && animation_id.graph_index != NONE)
	{
		inheritance = function_1daff0(graph_tag_get(graph_tag_index), animation_id);
	}
	function_27a060(graph_tag_get(graph_tag_index), animation_id, node_count, ratio, weight, inheritance, mask,
		transforms);
}
