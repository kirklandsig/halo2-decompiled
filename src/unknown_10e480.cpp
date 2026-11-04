// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10E480.CPP: start an object's vibration from a set of curves.
   Decompiled by lane F for 0x18ca20; its callee 0x10de40 is in
   unknown_10dc70.cpp, whose file this function probably shares. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11a4d0.h"

struct s_object_list;
extern s_object_list *g_4de2f4;

struct s_vibration_curve_set;
struct s_vibration_output;
void function_10de40(s_vibration_curve_set *set, s_vibration_output *output, real volatile time, real blend, bool skip_special);

struct s_vibration_object_view
{
	long definition_index;
	byte unknown04[0x33e - 4];
	short vibration_offset;
};

struct s_vibration_object_header
{
	byte unknown00[8];
	s_vibration_object_view *object;
};

struct s_vibration_view
{
	byte unknown00[0x30];
	byte output[6];
	short index;
	long tag_index;
};

struct s_vibration_definition_view
{
	byte unknown00[0x1e4];
	real blend;
};

struct s_vibration_globals_view
{
	byte unknown00[0x81];
	bool enabled;
};

// @retail 0x10e480
void function_10e480(long object_index, long tag_index, s_vibration_curve_set *curves, real time)
{
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_view *vibration = (s_vibration_view *)((byte *)object + object->vibration_offset);
	s_vibration_definition_view *definition = (s_vibration_definition_view *)g_4e3b44[object->definition_index & 0xffff].bytes;
	real blend = 0.95f;

	if (definition->blend > 0.0f)
	{
		blend = definition->blend;
	}
	vibration->tag_index = tag_index;
	if (vibration->index != NONE)
	{
		bool skip_special = false;

		if (((s_vibration_globals_view *)g_4de2f4)->enabled && function_11b930(object_index))
		{
			skip_special = true;
		}
		function_10de40(curves, (s_vibration_output *)vibration->output, time, blend, skip_special);
	}
}

#include "unknown_1cafc0.h"

/* per sample: the animation frame and its signed weight */
struct s_vibration_sample_tables
{
	char frames[12];
	char signed_weights[12];
};

struct s_vibration_sample_view
{
	byte unknown00[2];
	byte countdown;
	byte unknown03[0x30 - 3];
	c_type_709360 base_animation;
	c_type_709360 weighted_animation;
	byte unknown38[8];
	real frame;
	s_vibration_sample_tables tables;
	c_type_709360 animation_id;
	byte unknown60[0x6e - 0x60];
	byte weights[13];
};

extern real g_54764c;
struct s_anim_data;
void function_20adf0(s_anim_data *data, byte *dest);

struct s_output_10ec00 : s_animation_data
{
	__forceinline void write_10ec00(byte *out)
	{
		function_20adf0((s_anim_data *)this, out);
	}
};

// @retail 0x10ec00
void function_10ec00(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	c_type_709360 animation_id = vibration->animation_id;
	if (animation_id.index != NONE)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, animation_id, 0x40))
		{
			s_animation *animation = NULL;
			if (channel.animation_id.index != NONE)
				animation = function_1daea0(graph_tag_get(channel.graph_tag_index), channel.animation_id);
			bool sampled = false;
			for (long i = 0; i < 13; i++)
			{
				real frame = i * 2.0f + vibration->frame;
				if (frame >= 0.0f && frame < animation->frame_count && vibration->weights[i])
				{
					real weight = vibration->weights[i] * g_54764c;
					channel.set_frame_position(frame);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				s_output_10ec00 data;
				function_1ddb40(&data, graph_tag_get(channel.graph_tag_index), channel.animation_id);
				data.write_10ec00(dest);
			}
		}
	}
}

#include <math.h>
extern real g_547648;
void c_animation_channel_data_get(c_animation_channel const *channel, s_animation_data *data);

// @retail 0x10e530
void function_10e530(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	if (vibration->frame > 0.0001f)
	{
		c_animation_channel channel;
		c_type_709360 animation_id = vibration->base_animation;
		if (animation_id.index != NONE && state->channel_play(&channel, animation_id, 0))
		{
			channel.set_frame_position(0.0f);
			channel.sample(vibration->frame, node_mask, node_count, transforms);
			s_output_10ec00 data;
			c_animation_channel_data_get(&channel, &data);
			data.write_10ec00(dest);
		}
		animation_id = vibration->weighted_animation;
		if (animation_id.index != NONE && state->channel_play(&channel, animation_id, 0))
		{
			c_animation_channel const &playing = channel;
			s_animation *animation = playing.function_1c6440();
			bool sampled = false;
			s_vibration_sample_tables const *tables = &vibration->tables;
			for (long i = 0; i < 12; i++)
			{
				real weight = tables->signed_weights[i] * g_547648;
				weight *= vibration->frame;
				long sample_frame = vibration->tables.frames[i];
				if (sample_frame < animation->frame_count && !(fabs(weight) < 0.0001f))
				{
					channel.set_frame_position((real)sample_frame);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				s_output_10ec00 data;
				c_animation_channel_data_get(&channel, &data);
				data.write_10ec00(dest);
			}
		}
	}
	else if (vibration->frame != 0.0f)
	{
		if (vibration->base_animation.index != NONE)
			state->animation_touch(vibration->base_animation);
		if (vibration->weighted_animation.index != NONE)
			state->animation_touch(vibration->weighted_animation);
	}
	else if (vibration->weighted_animation.index != NONE && vibration->countdown > 0 && vibration->countdown < 7)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, vibration->weighted_animation, 0) && channel.function_1c6440()->frame_count > 17)
		{
			real weight = vibration->countdown >= 3 ? (7 - vibration->countdown) * 0.25f : vibration->countdown * (1.0f / 3.0f);
			c_animation_channel *playing = &channel;
			playing->set_frame_position(17.0f);
			playing->sample(weight, node_mask, node_count, transforms);
			s_output_10ec00 data;
			c_animation_channel_data_get(playing, &data);
			data.write_10ec00(dest);
		}
	}
}
