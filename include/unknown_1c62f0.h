/* UNKNOWN_1C62F0.H: the animation channels (src/unknown_1c62f0.cpp) and the
   object that holds three of them with the graph's tag index (src/unknown_1cafc0.cpp).

   A channel plays one animation of a graph tag (arg_0e6cbc.h). */

#ifndef UNKNOWN_1C62F0_H
#define UNKNOWN_1C62F0_H

#include "unknown_11c920.h"
#include "unknown_1dacb0.h"

struct real_quaternion_transform;
struct s_animation_state;

/* an event an animation reaches between two frames, as the dispatcher
   (function_1c7450) hands it to its callback (0x14 bytes) */
struct s_animation_frame_event
{
	short category; /* 0 a frame event, 1 a sound, 2 an effect */
	short frame;
	long tag_index;
	long type;
	long flags;
	long marker_name;
};

typedef void (__stdcall *animation_event_callback)(long user, real frame, s_animation_frame_event const *event);

class c_animation_channel
{
public:
	c_animation_channel();
	void reset();
	void clear();
	c_animation_channel *copy_from(c_animation_channel const *other);
	bool set(long graph_tag_index, word flags, c_type_709360 animation_id, long unknown08, char unknown0c,
		char unknown0d, char unknown0e);
	s_animation *function_1c6440() const;
	void set_frame_last();
	void set_frame_position(real frame);
	void update_events();
	void sample(real weight, dword const *node_mask, long node_count, real_quaternion_transform *transforms);
	void set_frame_ratio(real ratio);
	void update(s_animation_state *state, animation_event_callback callback, long user);
	void set_frame_ratio_and_advance(real ratio, s_animation_state *state, animation_event_callback callback, long user);
	real get_frame_ratio() const;
	real get_duration() const;
	real get_event_time() const;
	bool is_unflagged0() const;
	bool is_unflagged6() const;
	bool velocity_get(vector3f *delta, vector3f *velocity) const;
	void movement_rate_get(vector3f *vector, real *value) const;
	void sample_aiming(real yaw, real pitch, real weight, dword const *node_mask, long node_count,
		real_quaternion_transform *transforms);
	void sample_ratio(real ratio, real weight, long node_count, real_quaternion_transform *transforms,
		dword const *node_mask);

	long graph_tag_index;
	c_type_709360 animation_id;
	long unknown08;
	char unknown0c;
	char unknown0d;
	short unknown0e;
	byte unknown10;
	byte unknown11;
	union
	{
		word flags;
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flags2 : 14;
		};
	};
	short unknown14;
	short unknown16;
	real rate;
	real frame_position;
};

/* advances a channel to a frame, dispatching the events it passes (0x1c66a0) */
void __stdcall c_animation_channel_advance(c_animation_channel *channel, real frame, s_animation_state *state,
	animation_event_callback callback, long user);

#endif
