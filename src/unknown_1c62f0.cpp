// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1C62F0.CPP: the animation channels (0x1c62f0..0x1c7450) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1c62f0.h"
#include "unknown_1cafc0.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
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
		short *local_0 = (short *)&this->animation_id;
		local_0[0] = NONE;
		local_0[1] = NONE;
		byte *local_1 = &this->unknown10;
		local_1[0] = 0;
		local_1[1] = 0;
		this->graph_tag_index = graph_tag_index;
		this->animation_id = animation_id;
		this->unknown14 = 0;
		this->unknown16 = 0;
		this->unknown08 = unknown08;
		this->unknown0c = unknown0c;
		this->flags = flags;
		this->unknown0d = unknown0d;
		this->frame_position = 0.0f;
		this->rate = 1.0f;
		this->unknown0e = unknown0e;
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
struct s_anim_header;
struct s_anim_data
{
	byte *base;
	s_anim_header *header;
	byte unknown8;
	byte type;
	short count;

	void movement_get(vector3f *out, long index, real *w);
};
void function_20ad40(s_anim_data *data, vector3f *a, vector3f *b, long index);

/* the sampling state the codecs' decoders read (unknown_279d80.cpp,
   animation_channel_decoders.cpp) */

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
		struct s_movement_1c6dc0 : s_animation_data
		{
			__forceinline void movement_get(vector3f *vector, long frame, real *value)
			{
				((s_anim_data *)this)->movement_get(vector, frame, value);
			}
		};
		s_movement_1c6dc0 data;

		c_animation_channel_data_get(channel, &data);
		data.movement_get(vector, frame, value);
	}
}

// @retail 0x1c6c80
bool c_animation_channel_frame_sample(c_animation_channel const *channel, real frame, vector3f *position, vector3f *delta)
{
	bool result = false;

	if (channel->graph_tag_index != NONE && channel->animation_id.index != NONE)
	{
		long frame_index = real_truncate(frame);
		struct s_sample_1c6c80 : s_animation_data
		{
			__forceinline void frame_sample(vector3f *delta, vector3f *position, long frame_index)
			{
				function_20ad40((s_anim_data *)this, delta, position, frame_index);
			}
		};
		s_sample_1c6c80 data;

		c_animation_channel_data_get(channel, &data);
		data.frame_sample(delta, position, frame_index);
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
				bool local_724437 = false;
				if (frame > lower)
					local_724437 = upper >= frame;
				if (local_724437)
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

dword const *node_masks_combine(dword const *mask, dword const *other);

// @retail 0x1c6f60
void c_animation_channel::sample(real weight, dword const *node_mask, long node_count, real_quaternion_transform *transforms)
{
	s_animation *animation = function_1c6440();
	bool interpolate;
	dword const *graph_mask;
	dword const *mask;
	s_graph_inheritance *inheritance;
	s_tag_instance *tags;
	word channel_flags;

	if (unknown10 && animation->type)
	{
		c_type_709360 id = animation_id;
		function_1dd9d0(graph_tag_get(graph_tag_index), id);
		return;
	}
	channel_flags = flags;
	interpolate = (channel_flags >> 6) & 1;
	if ((channel_flags & 1) && rate != 0.0f && (rate != 1.0f || g_510c54->field_2_3 != 30))
	{
		interpolate = true;
	}
	tags = g_4e3b44;
	graph_mask = NULL;
	if (channel_flags & 0x800)
	{
		graph_mask = ((s_graph_tag *)tags[graph_tag_index & 0xffff].bytes)->node_mask5c;
	}
	else if (channel_flags & 0x400)
	{
		graph_mask = ((s_graph_tag *)tags[graph_tag_index & 0xffff].bytes)->node_mask7c;
	}
	mask = NULL;
	if (graph_mask || node_mask)
	{
		mask = node_masks_combine(graph_mask, node_mask);
	}
	inheritance = NULL;
	if (animation_id.index != NONE && animation_id.graph_index != NONE)
	{
		inheritance = function_1daff0(graph_tag_get(*(volatile long const *)&graph_tag_index), animation_id);
	}
	c_type_709360 id = animation_id;
	function_279d80(graph_tag_get(graph_tag_index), id, node_count, frame_position, weight, inheritance, mask, transforms,
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

/* Callback access through a local pointer preserves the stack-only entry convention. */
// Earlier at 97 (MiniMax S, 10-31): retail loads the flags word before the byte clear (movzx edx, word ptr
// [ebp + 0x12]) and keeps the callback in ebx across the second dispatch, while ours loaded the flags after the
// clear and reloaded the callback from its stack home; reading the callback once, plainly or through the
// volatile slot, kept one load but changed the register allocation (133, 132, 141).
// score 93 (was 93): the second-dispatch callback reload and the third dispatch's scratch registers (rotated
// one step) still differ; earlier also the adjacent flag load/byte clear. Direct parameters
// 139; direct channel/state/user 113, with word events 113; word/short events 97/97; separate bit clears
// 126; clears spanning clamp 136, with word events 136. Original body and parameter accesses restored.
// adjacent word-load/byte-clear order and callback reload remain.
// Native state bit clears/chained clears/publication/both 126/126/97/126;
// dispatcher word flags 122; with separate/chained/all native state writes 137/137/137.
// Direct variant publication 103; identity after lookup 102; early next frame 97; early frame limit 120;
// position after clears 103; explicit event merge/result at dispatch 97/97; end arm first 98.
// Direct callback with user/frame/state/frame-user/state-user/frame-state 125 each; callback with
// channel/channel-user 139 each. Entry convention shifts and matched callers regress; original
// callback and argument homes restored. Word-load schedule and callback reload remain.
// Frame, state and user read directly 97 (kept: callers still match); channel also direct 113; plain callback
// read at the first dispatch 133 (callback held in ebx from entry); shared inline dispatch helper for the
// three dispatches 140, with the first callback read kept 126; channel read directly for the first flags 97.
// Chained/reversed event-word resets 99/99; unsigned mask 97; index-first identity 161; sampled
// callback parameter with/without chaining 132/132; first-pair callback with/without chaining
// 138/138.
// Round 408: dispatcher inline/noinline/both declarations 97 each; those with
// direct callback transport 125 each; inline/both with early state clear 116 each.
// Original dispatcher contract, state writes and callback transport restored.
// Round 85 (retail loads the callback into EBX at the first dispatch, reuses it at the second and
// reloads it at the third; the second-dispatch reload is our 4 extra bytes): callback read directly at
// all three dispatches 133 (loaded into EBX at entry, graph spilled); plain read at the first dispatch
// 133; channel read directly for the byte clear 97, also for the first flags 97 (same code); the
// volatile read moved to the second dispatch 97 (same code); volatile read at the third dispatch only
// 141 (callback in EBX from entry, frame 0x10).
// Round 419: end failure exit 97; loop failure exit 97; both later failure exits 97; first/end failure
// exits 97; first/loop failure exits 97; all dispatch failure exits 97; all failures with direct final
// update 109; native events with direct final update 109. Word-load/state-clear scheduling and second
// callback reload remain. Original body restored.
// Round 3 (lane 18): retail stores the looked-up variant to the channel, tests flag 8, sets the rate and
// only then stores the identity local from the same register. The channel field stored first and the
// identity reread after the rate 103; a variant local stored to the channel and, after the rate, to the
// identity 99 (the identity store lands where retail's does, but the result is read from the stack, not
// through EAX); chained identity/channel assignment 97 (same code); flag1 tested as `(flags >> 1) & 1` 90
// at 531 bytes (`test al, 2` in place of retail's shr/test; the gain is only realignment); no volatile read,
// with the third dispatch reading the parameter 133 (+0x4). Original body restored.
// Round 126: the callback read through the pointer at every dispatch (no volatile) 133 (loaded at entry);
// read as the plain parameter at every dispatch, pointer dropped, channel pointer kept 125 (LTCG passes
// the callback in EAX; callers 0x1c68c0 16, 0x1c6920 8, 0x1cb710 19); the volatile read at the first two
// dispatches 97 (same code); the pointer read at the first and the parameter at the others 133; the
// channel flags read into a local before the byte clear 121 (538 bytes).
// Round 315: Independent initial/end/loop dispatch outputs with native-word versus long carriers, all
// eight masks 97. Every one-use result form is restored. Original shared result, callback
// observations, parameter aliases and state graph retained; word-load schedule and second callback
// reload remain.
// Round 22 (lane 13; aligned difflib count 15 at 97: the flags-word load after the byte clear, the
// second-dispatch callback reload, the identity spill before the rate branch and the third
// dispatch's argument registers): next frame computed before the position store 97 (aligned 18);
// the callback sampled once into a named local from the volatile read 132 (frame 0x10); a named
// loop-frame value for both 97 (same code); flag 1 as `((dword)flags >> 1) & 1` 97 (same code);
// the rate reset before the identity store 98.
// Round 116 (lane 6, fast; callers 0x1c68c0, 0x1c6920, 0x1cb710 still match): at the second dispatch both
// builds hold the callback in EBX; ours reloads it only because the first read is volatile. Frame limit
// after the clamp 118; byte clear before the event-word clears 116; event-word clears after the clamp 114;
// `next_frame < 0.0f` 97 (same code); frame limit after the byte clear 119.
// Round 16 (m3 lane 17, fast): the `&channel` pointer local is what keeps the first-dispatch flags load
// below the byte clear (channel read directly: flags load hoisted as retail, but the identity compares
// are CSE'd into CX, 113); the identity compared through an `operator==` on the identity type with
// channel read directly 93 (memory-operand compares as retail); on it: no volatile 136; channel store
// first and the identity re-read after the rate 98; a variant local initialised from the call 94 (it
// becomes the return buffer); the variant local declared, then assigned and stored to the identity
// after the rate 93 (the variant block is then retail's instruction for instruction; not kept, same
// count); plain callback at the second dispatch 93 (same code); user read through its address without
// volatile 133; flags through a plain word cast 97 (same code). Left: the second-dispatch callback reload
// (volatile), after which the third dispatch's scratch registers are rotated one step (EAX/ECX/EDX).
// Round 18 (lane 17, fast; diagnostics with the `standard` marker, retail's callers push all five arguments and
// nothing holds the address): the marker with plain `callback` reads and no pointer 124 (the second dispatch now
// reuses EBX as retail, but the third keeps it too where retail reloads, so `animation` spills: frame 0x10,
// callers 0x1c68c0/0x1c6920/0x1cb710 still match); with the variant stored to the channel before the rate and
// to the identity after it 125; with one read through `&callback` at the third dispatch 124 (CSE'd);
// uninitialised locals declared first 124; the marker on this body 93 (same code). Marker not kept (unmatched).
// Without the marker: the `&callback` pointer declared just before the first dispatch with plain reads 136, with
// the volatile first read 93 (same code).
// Round 470: six lexical orders of the position/limit/next scalars 97 each;
// first and last float orders with later saved-identity names 97/97. Original
// names and body restored; adjacent flags scheduling and callback reload remain.
// Round 126 (m3 lane 5, fast; retail's callback acts as a memory value: loaded at the first dispatch, reused
// at the second, reloaded after the join at the third): a local taken from the volatile read before the
// first dispatch, used at the first two 141 (frame 0x10, the zero in EBX), at all three 135; a local from a
// plain pointer read used at the first two 136; `(void)&callback` with the parameter read at each dispatch
// 128 (LTCG passes the callback in EAX; callers 0x1c68c0 16, 0x1c6920 8, 0x1cb710 19); a const reference
// to the callback read at each dispatch 136.
// Round 47 (lane 11, fast; callers 0x1c68c0, 0x1c6920, 0x1cb710 match unless given): retail treats the
// callback (and the graph) as memory variables whose register copy is reused: loaded into EBX at the first
// dispatch, kept for the second, reloaded after the variant join for the third. A plain callback with the
// pointer and volatile dropped and the parameter renamed to sort after `graph` 128 (callback in EAX, kept
// in EBX from entry; callers 16/8/19, same as without the rename); the volatile read taken once into a
// named local for the first two dispatches 141 (frame 0x10, EBX becomes the zero); the callback pointer
// declared `const *const` 93, declared just before the first dispatch 93, declared after the position
// read 93 (same code each). Lead not tried: callback and user passed as one by-value pair would keep both
// in memory as retail, but every caller up the chain would change with it.
// Round 70 (lane 12, fast with callers and callees): the callback parameter read plainly at the first
// dispatch (pointer at the others) 136 (537 bytes); the frame clamp as `0.0f > frame ? 0.0f : frame` 107; the
// last frame computed after the clamp 119; the loop frame as `next_frame -= last_frame; ... += position` 93
// (same code); the non-looping end frame as `next_frame = last_frame` 91 (531 bytes only because that arm loses
// retail's recomputed frame count; every later line still differs).
// Round 237: graph/animation/event lexical homes, seven nonempty subsets 93/93/93/93/93/93/93;
// all three with earlier uninitialized declarations 93
// Original executable body and helpers restored.
// @retail 0x1c66a0
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user)
{
	animation_event_callback const *local_3 = &callback;
	c_type_709360 animation_id = channel->animation_id;
	s_graph_tag *graph = c_animation_channel_get_graph(channel);
	s_animation *animation = channel->function_1c6440();
	real position = channel->frame_position;
	real last_frame;
	real next_frame;
	long events;

	channel->unknown14 = 0;
	channel->unknown16 = 0;
	next_frame = frame;
	last_frame = (real)animation->frame_count - 0.0001f;
	if (0.0f > next_frame)
	{
		next_frame = 0.0f;
	}
	channel->unknown11 &= ~0xe;
	events = animation_events_dispatch(graph, animation, channel->flags, position, next_frame, *(animation_event_callback volatile *)local_3, user);
	if (animation_id == channel->animation_id)
	{
		channel->unknown14 |= events;
		if (next_frame >= last_frame)
		{
			events = animation_events_dispatch(graph, animation, channel->flags, position, FLT_MAX, *local_3, user);
			if (animation_id == channel->animation_id)
			{
				channel->unknown14 |= events;
				if (TEST_FIELD_BIT(channel->flag1))
				{
					if ((channel->flags & 0x20) && animation->loop_frame_index == 0)
					{
						animation_id = state->variant_get(channel->animation_id);
						channel->animation_id = animation_id;
						if (channel->flags & 8)
						{
							channel->rate = 1.0f;
						}
						animation = channel->function_1c6440();
					}
					position = (real)animation->loop_frame_index;
					next_frame = next_frame - last_frame + position;
					events = animation_events_dispatch(graph, animation, channel->flags, position, next_frame, *local_3, user);
					if (animation_id == channel->animation_id)
					{
						channel->unknown14 |= events;
						channel->unknown11 |= 4;
					}
				}
				else
				{
					channel->unknown11 |= 8;
					next_frame = (real)animation->frame_count - 0.0001f;
				}
			}
		}
		if (animation_id == channel->animation_id)
		{
			channel->set_frame_position(next_frame);
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
		vector3f_scale(&position, scale, velocity);
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
			c_type_709360 id = animation_id;
			function_279e40(screen, graph_tag_get(graph_tag_index), id, node_count, yaw, pitch, weight,
				inheritance, mask, transforms);
		}
	}
}

// @retail 0x1c7200
void c_animation_channel::sample_ratio(real ratio, real weight, long node_count, real_quaternion_transform *transforms,
	dword const *node_mask)
{
	s_tag_instance *tags = g_4e3b44;
	dword const *graph_mask = NULL;
	dword const *mask;
	s_graph_inheritance *inheritance;

	if (flags & 0x800)
	{
		graph_mask = ((s_graph_tag *)tags[graph_tag_index & 0xffff].bytes)->node_mask5c;
	}
	else if (flags & 0x400)
	{
		graph_mask = ((s_graph_tag *)tags[graph_tag_index & 0xffff].bytes)->node_mask7c;
	}
	mask = NULL;
	if (graph_mask || node_mask)
	{
		mask = node_masks_combine(graph_mask, node_mask);
	}
	inheritance = NULL;
	if (animation_id.index != NONE && animation_id.graph_index != NONE)
	{
		inheritance = function_1daff0(graph_tag_get(*(volatile long const *)&graph_tag_index), animation_id);
	}
	c_type_709360 id = animation_id;
	function_27a060(graph_tag_get(graph_tag_index), id, node_count, ratio, weight, inheritance, mask,
		transforms);
}
