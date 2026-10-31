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

// score 36 (was 36): incoming graph save and the late graph store still differ. With retail's
// store order (graph after unknown0d) and the byte-pointer clear the body equals retail except that
// the animation id is copied through ECX (retail: ESI, while CX holds unknown0e from an early load and
// a late store), so ESI is never modified and LTCG passes graph straight in ESI (retail: ECX, then
// push esi; mov esi, ecx); the four matched callers break. Early frame-position store kept (36).
// store order (graph after unknown0d) and the byte-pointer clear the body equals retail except
// that the animation id is copied through ECX (retail: ESI, while CX holds unknown0e), so ESI is
// never modified and LTCG passes graph straight in ESI (retail: ECX, then push esi; mov esi, ecx);
// the four matched callers break (only their graph argument register differs). Retail hoists the
// unknown0e load above the graph store, which is what leaves ESI for the id copy; ours hoists the
// id copy instead (its store moves above the graph and frame stores). Early frame-position store
// kept (36).
// Forms: sentinel/word/byte clear chains 41/41/43; two-byte memset 43; stored flags 41; stored-ID
// result 76; renamed late graph 42; dword/long flags plus early frame 36/37; paired defaults 42;
// identifier memcpy 36; metadata before resets 42; trailing defaults 41; redundant byte clear
// removed 36; early derived state 38; explicit state bit 35; defaults plus memcpy 42;
// validity-result 43; animation guard first 44; inline/noinline declarations 41; existing reset 43;
// state memset 51; high-bit predicate 41; ID word copies 50; retail store order 51; counter from
// bit 15 as conditional/ternary 43/43 (retail order 49/51); graph as a by-value class 43 (on the
// stack); graph store first/after the NONE words 41/41, after the zero words/unknown0d/frame 42
// each; inline definition in retail order 42, with graph copied to a local 42; nested validity
// gates 36; separate invalid guards 42; eager validity tests 47; short-pair memset 50 (eager
// validity and short-pair memset lose the four checked callers); early short metadata 43; early
// flags word 41; early long metadata 43; named short promotion 36; success result/nested success
// result 49/49; early invalid guard with early frame 36; role-named inputs 36; late rate/short
// metadata/both 36/36/36; both with the success result 49; out-of-line member declaration 36/42;
// immutable input ID 36/42; ID sentinel memset 50/51; direct state bytes 43/51.
// Round 75: existing clear() then retail-order assignments 51; const graph 51; __fastcall member
// declaration 51 (LTCG overrides it); retail order with the byte-pointer clear 42; eight orders of
// the graph/frame/id/rate/unknown0e/unknown10 assignments 42 each.
// Round 5: `unknown10 |=` the flag bit with plain byte clears 43 (the clear folds away, graph in EDX,
// five callers lost); retail order with unknown0e before graph 42; `animation_id = c_type_709360()`
// for the NONE pair, current/retail order 55/59 (the dword store absorbs both word stores); retail
// order with unknown0e converted to a short local before graph 42 (propagated; retail's movsx
// still holds CX across the id copy, ours copies the id first).
// Round 113 (the compile-only /Gd pass loads graph once into ESI for every late-store order, which
// is what makes it an ESI argument): retail order with the graph parameter cleared when the index
// is NONE 51 (same code); graph store after unknown16/unknown08 43/40 (graph EDX, flags ECX);
// after unknown0c/flags 51; frame first with graph late 39; retail order with the byte pair 42
// (ESI argument, NONE in ECX as retail).
// Round 95 (one-file harness, LTCG conventions reproduce: retail order passes graph in ESI there
// too): on retail's order, graph copied to a local first, the parameter set to NONE when the id
// is NONE, the last flags bits from the stored field, the frame store before graph 51 each in
// the build (126 bytes, the matched callers lose); the current form with the last flags bits
// from the stored field 36.
// Round 30: retail's store order read from its listing (retail's clear()/reset() keep source
// order) 51 (graph in ESI, five matched callers break); default-constructed id for the two NONE
// stores 62 (dropped as dead); that order with unknown0e after unknown0d 51; unknown0e after
// the id copy 36; flags stored next to the derived byte 36. Retail hoists the unknown0e load
// above the graph store, which is what leaves ESI for the id copy. All restored.
// Round 118 (one-file harness): all 120 orders of the graph/frame/id/rate/unknown0e stores after
// unknown0d pass graph in ESI; so do the stores through a forced-inline initializer taking every
// parameter (its argument copies are propagated) and the id NONE words and/or unknown0c/unknown0d
// stored through pointers (as reset()/clear() do), which on retail's order give 43 in the build
// (the matched callers lose). On the current order: the id words through a pointer 36; unknown0c/
// unknown0d through a char pointer 34; both 34 (two-point gains, pointer locals: not kept).
// from the stored field 36. Round 118 (one-file harness): all 120 orders of the late
// graph/frame/id/rate/unknown0e stores pass graph in ESI, as do a forced-inline initializer taking
// every parameter and pointer stores for the id words.
// Round 30: retail's store order read from its listing 51 (graph in ESI, five matched callers
// break); default-constructed id for the two NONE stores 62 (dropped as dead); that order with
// unknown0e after unknown0d 51; unknown0e after the id copy 36; flags stored next to the derived
// byte 36. Retail hoists the unknown0e load above the graph store, which is what leaves ESI for
// the id copy.
// Round 83: retail order 42 (callers lose); unknown0e stored right after unknown0d 42 (stored at
// once, ECX free for the id copy); the two NONE id words as one chained assignment, retail
// order/current 41/36; the two zero words chained 38.
// Round 85: the byte pair as plain unknown10/unknown11 clears (no pointer) 43 (the four matched callers
// lose); that with retail's order after an early graph store 43; id before rate before unknown0e 36;
// frame store after unknown0d 41; frame, id, rate, unknown0e last in retail's order 41.
// the id copy 36; flags stored next to the derived byte 36.
// Round 42 (all with graph in ESI and the six matched callers off by 1-68): retail order with
// unknown0e stored first/before unknown0d 40/42, graph stored last 42, id stored last 44; existing
// reset() or clear() inlined first, then retail's assignments with or without frame/rate 51 (the
// unknown10 clear is dropped, flags move to ECX); the graph/frame/id/rate/unknown0e/flag-bit stores
// in a forced-inline helper taking unknown0e as a short (call-site conversion) 42.
// Round 85: the byte pair as plain unknown10/unknown11 clears (no pointer) 43 (the four matched callers
// lose); that with retail's order after an early graph store 43; id before rate before unknown0e 36;
// frame store after unknown0d 41; frame, id, rate, unknown0e last in retail's order 41.
// Round 203: identifier memmove early/retail 58/57; native default assignment
// early/retail 68/68; defaults through copy_from 45/45; complete native
// replacement assignment/copy_from 75/52. All lose five matched caller controls;
// original body restored. Saved graph and identifier carriers remain.
// Round 97 (lane 8, fast with 0x1cb410/0x1086e0/0x10fa80 and the other callers): flags declared before
// graph (callers load ECX, then EDX, then EAX), definition and all 15 calls, 36 (same code); that with
// retail's store order 42 (graph still an ESI argument; 0x1cb410 14, 0x1086e0 2); retail's order with
// a dword flags parameter 42; with an early `return false` guard 42; that with unknown0e stored before
// graph 42. In every retail-order form ours stores the id before graph (the id load takes ECX), so ESI
// holds only graph and becomes its argument register; retail reuses ESI for the id after the graph store.
// Round 32 (lane 11, fast with the six matched callers; retail's callers load ECX first, then EDX,
// then EAX, but the matched callers already do so with this declaration, so no reorder was tried):
// `long const` graph parameter 36 (same code); single-exit result 49 (127 bytes, callers lose); the
// id copied right after the NONE pair 41; unknown0e stored before graph 43; rate stored before the
// frame 42. Diagnostic only: size-preference pragma 56 (120 bytes).
// @retail 0x1c6470
bool c_animation_channel::set(long graph_tag_index, word flags, c_type_709360 animation_id, long unknown08,
	char unknown0c, char unknown0d, char unknown0e)
{
	if (graph_tag_index != NONE && animation_id.index != NONE)
	{
		this->frame_position = 0.0f;
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

/* Channel and callback access through local pointers preserves the stack-only entry convention. */
// score 97 (was 97): adjacent flag load/byte clear and callback reload still differ; direct parameters
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
// @retail 0x1c66a0
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user)
{
	c_animation_channel *const *local_0 = &channel;
	animation_event_callback const *local_3 = &callback;
	c_type_709360 animation_id = (*local_0)->animation_id;
	s_graph_tag *graph = c_animation_channel_get_graph(*local_0);
	s_animation *animation = (*local_0)->function_1c6440();
	real position = (*local_0)->frame_position;
	real last_frame;
	real next_frame;
	long events;

	(*local_0)->unknown14 = 0;
	(*local_0)->unknown16 = 0;
	next_frame = frame;
	last_frame = (real)animation->frame_count - 0.0001f;
	if (0.0f > next_frame)
	{
		next_frame = 0.0f;
	}
	(*local_0)->unknown11 &= ~0xe;
	events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, next_frame, *(animation_event_callback volatile *)local_3, user);
	if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
	{
		(*local_0)->unknown14 |= events;
		if (next_frame >= last_frame)
		{
			events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, FLT_MAX, *local_3, user);
			if (animation_id.graph_index == (*local_0)->animation_id.graph_index && animation_id.index == (*local_0)->animation_id.index)
			{
				(*local_0)->unknown14 |= events;
				if (TEST_FIELD_BIT((*local_0)->flag1))
				{
					if (((*local_0)->flags & 0x20) && animation->loop_frame_index == 0)
					{
						animation_id = state->variant_get((*local_0)->animation_id);
						(*local_0)->animation_id = animation_id;
						if ((*local_0)->flags & 8)
						{
							(*local_0)->rate = 1.0f;
						}
						animation = (*local_0)->function_1c6440();
					}
					position = (real)animation->loop_frame_index;
					next_frame = next_frame - last_frame + position;
					events = animation_events_dispatch(graph, animation, (*local_0)->flags, position, next_frame, *local_3, user);
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
