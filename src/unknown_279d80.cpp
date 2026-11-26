// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_279D80.CPP: sampling an animation's nodes, and 256-bit node masks.

   function_2798a0 fills the sampling state (0x504450..0x5044b0) the codecs'
   channel decoders read, then runs the decoders of the animation's static
   and animated data through function_279860. */

#include "unknown_11c920.h"
#include "unknown_1dacb0.h"
#include "unknown_xd56787.h"
#include "unknown_0259d0.h"
#include "unknown_11cb00.h"
#include <math.h>
#include <string.h>
#include <xmmintrin.h>


/* the codecs' channel decoders (animation_channel_decoders.cpp,
   animation_keyframe_decoders.cpp, unknown_2c4d60.cpp) */
void animation_codec_static_rotation_sample(void);
void animation_codec_static_translation_sample(void);
void animation_codec_static_scale_sample(void);
void animation_codec_blend_screen_rotation_interpolate(void);
void animation_codec_blend_screen_translation_interpolate(void);
void animation_codec_scale_interpolate(void);
void animation_codec_blend_screen_rotation_sample(void);
void animation_codec_translation_sample(void);
void animation_codec_quantized_rotation_interpolate(void);
void animation_codec_quantized_translation_interpolate(void);
void animation_codec_quantized_rotation_sample(void);
void animation_codec_byte_keyframe_rotation_interpolate(void);
void animation_codec_byte_keyframe_translation_interpolate(void);
void animation_codec_byte_keyframe_scale_interpolate(void);
void animation_codec_byte_keyframe_rotation_sample(void);
void animation_codec_byte_keyframe_translation_sample(void);
void animation_codec_byte_keyframe_scale_sample(void);
void animation_codec_word_keyframe_rotation_interpolate(void);
void animation_codec_word_keyframe_translation_interpolate(void);
void animation_codec_word_keyframe_scale_interpolate(void);
void animation_codec_word_keyframe_rotation_sample(void);
void animation_codec_word_keyframe_translation_sample(void);
void animation_codec_word_keyframe_scale_sample(void);
void function_2c4d60(void);
void function_2c4da0(void);
void function_2c4de0(void);
char __stdcall function_28d170(long a, long b, long c, long d);

s_animation_codec const g_47fb18[9] =
{
	{ "_no_compression_codec", 1, 0, { { NULL, NULL, NULL }, { NULL, NULL, NULL } }, function_28d170 },
	{ "_uncompressed_static_data_codec", 0, 0,
		{ { animation_codec_static_rotation_sample, animation_codec_static_translation_sample, animation_codec_static_scale_sample }, { animation_codec_static_rotation_sample, animation_codec_static_translation_sample, animation_codec_static_scale_sample } },
		function_28d170 },
	{ "_uncompressed_animated_data_codec", 1, 0,
		{ { function_2c4d60, function_2c4da0, function_2c4de0 }, { function_2c4d60, function_2c4da0, function_2c4de0 } },
		function_28d170 },
	{ "_8byte_quantized_rotation_only_codec", 1, 0,
		{ { animation_codec_quantized_rotation_sample, animation_codec_translation_sample, function_2c4de0 }, { animation_codec_quantized_rotation_interpolate, animation_codec_quantized_translation_interpolate, animation_codec_scale_interpolate } },
		function_28d170 },
	{ "byte_keyframe_lightly_quantized", 2, 1,
		{ { animation_codec_byte_keyframe_rotation_sample, animation_codec_byte_keyframe_translation_sample, animation_codec_byte_keyframe_scale_sample }, { animation_codec_byte_keyframe_rotation_interpolate, animation_codec_byte_keyframe_translation_interpolate, animation_codec_byte_keyframe_scale_interpolate } },
		function_28d170 },
	{ "word_keyframe_lightly_quantized", 2, 2,
		{ { animation_codec_word_keyframe_rotation_sample, animation_codec_word_keyframe_translation_sample, animation_codec_word_keyframe_scale_sample }, { animation_codec_word_keyframe_rotation_interpolate, animation_codec_word_keyframe_translation_interpolate, animation_codec_word_keyframe_scale_interpolate } },
		function_28d170 },
	{ "reverse_byte_keyframe_lightly_quantized", 2, 1,
		{ { animation_codec_byte_keyframe_rotation_sample, animation_codec_byte_keyframe_translation_sample, animation_codec_byte_keyframe_scale_sample }, { animation_codec_byte_keyframe_rotation_interpolate, animation_codec_byte_keyframe_translation_interpolate, animation_codec_byte_keyframe_scale_interpolate } },
		function_28d170 },
	{ "reverse_word_keyframe_lightly_quantized", 2, 2,
		{ { animation_codec_word_keyframe_rotation_sample, animation_codec_word_keyframe_translation_sample, animation_codec_word_keyframe_scale_sample }, { animation_codec_word_keyframe_rotation_interpolate, animation_codec_word_keyframe_translation_interpolate, animation_codec_word_keyframe_scale_interpolate } },
		function_28d170 },
	{ "_blend_screen_codec", 1, 0,
		{ { animation_codec_blend_screen_rotation_sample, animation_codec_translation_sample, function_2c4de0 }, { animation_codec_blend_screen_rotation_interpolate, animation_codec_blend_screen_translation_interpolate, animation_codec_scale_interpolate } },
		function_28d170 },
};

/* the sampling state */
s_animation_sampling_settings g_sampling_settings;

/* the nodes the object-space parent nodes set: rotation, translation and
   scale */
dword g_55e530[8];
dword g_55e550[8];
dword g_55e570[8];

/* the combination of two node masks */
dword g_55e590[8];

/* the samplers' dispatcher (below) */
void function_279860(void);
bool __stdcall function_27a100(s_graph_tag *graph, s_animation *animation, s_graph_inheritance *inheritance,
	long node_count, real_quaternion_transform *transforms);

void node_mask_and(dword *mask, dword const *other);

PRIVATE inline long animation_data_size(s_animation_data_sizes const *sizes)
{
	return sizes->static_node_flags_size + sizes->animated_node_flags_size + sizes->movement_data_size +
		sizes->unknown04 + sizes->static_data_size + sizes->unknown08 + sizes->animated_data_size;
}

PRIVATE inline void node_mask_clear(dword *mask)
{
	memset(mask, 0, 8 * sizeof(dword));
}

/* whether a bit is set in a bit vector */
#define BIT_VECTOR_TEST(vector, bit) ((((dword const *)(vector))[(bit) >> 5] & (1 << ((bit) & 31))) != 0)

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

// @retail 0x2798a0
void function_2798a0(s_animation_data *data, real frame, real weight, s_graph_tag *graph, s_animation *animation,
	long node_count, s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms,
	bool interpolate, bool blend, real blend_frame, real blend_weight)
{
	if (weight > 0.0001f)
	{
		long frame_index = real_truncate(frame);
		long last_frame = animation->frame_count - 1;

		node_mask_clear(g_55e530);
		node_mask_clear(g_55e550);
		node_mask_clear(g_55e570);
		g_sampling_settings.node_count = node_count;
		g_sampling_settings.destination_node_count = node_count;
		frame_index = PIN(frame_index, 0, last_frame);
		g_sampling_settings.inheritance = inheritance;
		g_sampling_settings.destination_node_mask = node_mask;
		memset(&g_sampling_settings.decompressors, 0, sizeof(g_sampling_settings.decompressors));
		g_sampling_settings.frame_index = frame_index;
		g_sampling_settings.next_frame_index = frame_index;
		g_sampling_settings.frame_fraction = 0.0f;
		g_sampling_settings.blend_weight = 1.0f;
		g_sampling_settings.animation = animation;
		g_sampling_settings.field_30 = NULL;
		g_sampling_settings.rotation_bit_flags = NULL;
		g_sampling_settings.translation_bit_flags = NULL;
		g_sampling_settings.scale_bit_flags = NULL;
		g_sampling_settings.field_4c = transforms;
		g_sampling_settings.blend_frames = blend;
		if (blend)
		{
			long blend_frame_index = real_truncate(blend_frame);

			g_sampling_settings.blend_frame_index = PIN(blend_frame_index, 0, last_frame);
			g_sampling_settings.blend_next_frame_index = PIN(blend_frame_index + 1, 0, last_frame);
			g_sampling_settings.blend_frame_fraction = blend_frame - (real)g_sampling_settings.blend_frame_index;
			g_sampling_settings.next_frame_index = PIN(g_sampling_settings.frame_index + 1, 0, last_frame);
			g_sampling_settings.frame_fraction = frame - (real)g_sampling_settings.frame_index;
			g_sampling_settings.blend_fraction = blend_weight;
		}
		else if (interpolate)
		{
			if (frame_index != last_frame)
			{
				long next_frame_index = frame_index + 1;

				g_sampling_settings.next_frame_index = PIN(next_frame_index, 0, last_frame);
				g_sampling_settings.frame_fraction = frame - (real)frame_index;
				if (g_sampling_settings.frame_fraction < 0.0001f)
				{
					g_sampling_settings.frame_fraction = 0.0f;
					interpolate = false;
				}
				else if (g_sampling_settings.frame_fraction > 0.9999f)
				{
					g_sampling_settings.frame_index = next_frame_index;
					g_sampling_settings.frame_fraction = 0.0f;
					interpolate = false;
				}
			}
			else
			{
				interpolate = false;
			}
		}
		g_sampling_settings.node_kind = 0;
		if (g_sampling_settings.inheritance)
		{
			g_sampling_settings.node_kind = (inheritance->flags & 1) ? 2 : 1;
			g_sampling_settings.node_count = g_sampling_settings.animation->node_count;
		}
		else
		{
			g_sampling_settings.node_count = g_sampling_settings.animation->node_count;
			if (g_sampling_settings.destination_node_count <= g_sampling_settings.node_count)
			{
				g_sampling_settings.node_count = g_sampling_settings.destination_node_count;
			}
		}
		g_sampling_settings.blend_method = 0;
		bool full_weight = weight > 0.9999f;
		if (animation->type == 1)
		{
			g_sampling_settings.blend_method = full_weight ? 2 : 3;
			g_sampling_settings.blend_weight = weight;
		}
		else if (!full_weight)
		{
			g_sampling_settings.blend_method = 1;
			g_sampling_settings.blend_weight = weight;
		}
		if (animation->type == 0 && data->sizes->static_data_size != 0)
		{
			long flags_size;

			byte *static_data = data->data;

			g_sampling_settings.field_30 = (struct s_animation_data *)static_data;
			g_sampling_settings.decompressors = g_47fb18[*static_data].samplers[0];
			g_sampling_settings.rotation_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size;
			flags_size = ((data->node_count + 31) >> 3) & ~3;
			g_sampling_settings.translation_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size + flags_size;
			flags_size = ((data->node_count + 31) >> 3) & ~3;
			g_sampling_settings.scale_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size + flags_size * 2;
			g_sampling_settings.interpolated_decompressors = false;
			function_279860();
		}
		if (data->sizes->animated_data_size != 0)
		{
			byte *animated_data;
			long flags_size;

			if (g_sampling_settings.blend_method == 0 && function_27a100(graph, animation, inheritance, node_count, transforms))
			{
				g_sampling_settings.blend_method = 4;
			}
			animated_data = data->data + data->sizes->static_data_size;
			g_sampling_settings.field_30 = (struct s_animation_data *)animated_data;
			g_sampling_settings.decompressors = g_47fb18[*animated_data].samplers[interpolate ? 1 : 0];
			g_sampling_settings.rotation_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size +
				data->sizes->static_node_flags_size;
			flags_size = ((data->node_count + 31) >> 3) & ~3;
			g_sampling_settings.translation_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size +
				data->sizes->static_node_flags_size + flags_size;
			flags_size = ((data->node_count + 31) >> 3) & ~3;
			g_sampling_settings.scale_bit_flags = data->data + data->sizes->static_data_size + data->sizes->animated_data_size +
				data->sizes->static_node_flags_size + flags_size * 2;
			g_sampling_settings.interpolated_decompressors = interpolate;
			function_279860();
		}
	}
}

#pragma inline_depth(0)
// @retail 0x279d40
dword const *node_masks_combine(dword const *mask, dword const *other)
{
	dword const *result = NULL;

	if (mask)
	{
		if (other && mask != other)
		{
			memcpy(g_55e590, mask, sizeof(g_55e590));
			node_mask_and(g_55e590, other);
			result = g_55e590;
		}
		else
		{
			result = mask;
		}
	}
	else if (other)
	{
		result = other;
	}
	return result;
}
#pragma inline_depth(255)

// score 31 (was 31): the three values are rotated between registers: ours keeps the pointer in
// ECX and the accumulator in EAX, retail keeps the pointer in EAX and the
// accumulator in EDX.
// @retail 0x279d80
void function_279d80(s_graph_tag *graph, c_type_709360 animation_id, long node_count, real frame, real weight,
	s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms, bool interpolate)
{
	s_animation *animation = function_1daea0(graph, animation_id);
	s_animation_data data;
	long size;
	byte *address;

	function_1ddb40(&data, graph, animation_id);
	address = data.data;
	size = MIN(animation_data_size(data.sizes), 0x400);
	for (; address < data.data + size; address += 0x20)
	{
		_mm_prefetch((char const *)address, _MM_HINT_T0);
	}
	function_2798a0(&data, frame, weight, graph, animation, node_count, inheritance, node_mask, transforms, interpolate,
		false, 0.0f, 0.0f);
}

// @retail 0x279e40
void function_279e40(s_aiming_screen const *screen, s_graph_tag *graph, c_type_709360 animation_id, long node_count,
	real yaw, real pitch, real weight, s_graph_inheritance *inheritance, dword const *node_mask,
	real_quaternion_transform *transforms)
{
	s_animation *animation = function_1daea0(graph, animation_id);
	s_animation_data data;
	long right_frame_count;
	long left_frame_count;
	long yaw_frame_count;
	long down_frame_count;
	long up_frame_count;
	real yaw_per_frame;
	real yaw_frames;
	real yaw_frame;
	real pitch_per_frame;
	real pitch_frames;
	real pitch_frame;
	real lower_row;
	real upper_row;
	real lower_frame;
	real upper_frame;
	real fraction;

	function_1ddb40(&data, graph, animation_id);
	left_frame_count = screen->left_frame_count;
	right_frame_count = screen->right_frame_count;
	yaw_frame_count = right_frame_count + left_frame_count + 1;
	up_frame_count = screen->up_frame_count;
	down_frame_count = screen->down_frame_count;

	yaw_per_frame = yaw < 0.0f ? screen->right_yaw_per_frame : screen->left_yaw_per_frame;
	yaw_frames = yaw_per_frame < 0.0001f ? 0.0f : yaw / yaw_per_frame;
	yaw_frames = PIN(yaw_frames, (real)-right_frame_count, (real)left_frame_count);
	yaw_frame = (real)right_frame_count + yaw_frames;

	pitch_per_frame = pitch < 0.0f ? screen->down_pitch_per_frame : screen->up_pitch_per_frame;
	pitch_frames = pitch_per_frame < 0.0001f ? 0.0f : pitch / pitch_per_frame;
	pitch_frames = PIN(pitch_frames, (real)-down_frame_count, (real)up_frame_count);
	pitch_frame = (real)down_frame_count + pitch_frames;

	lower_row = (real)floor(pitch_frame);
	upper_row = (real)ceil(pitch_frame);
	lower_row = PIN(lower_row, 0.0f, (real)(down_frame_count + up_frame_count));
	upper_row = PIN(upper_row, 0.0f, (real)(down_frame_count + up_frame_count));
	lower_frame = (real)yaw_frame_count * lower_row + yaw_frame;
	upper_frame = (real)yaw_frame_count * upper_row + yaw_frame;
	fraction = pitch_frame - lower_row;
	if (fraction < 0.0001f)
	{
		function_2798a0(&data, lower_frame, weight, graph, animation, node_count, inheritance, node_mask, transforms,
			true, false, 0.0f, 0.0f);
	}
	else if (upper_row - pitch_frame < 0.0001f)
	{
		function_2798a0(&data, upper_frame, weight, graph, animation, node_count, inheritance, node_mask, transforms,
			true, false, 0.0f, 0.0f);
	}
	else
	{
		function_2798a0(&data, lower_frame, weight, graph, animation, node_count, inheritance, node_mask, transforms,
			true, true, upper_frame, fraction);
	}
}

// @retail 0x27a060
void function_27a060(s_graph_tag *graph, c_type_709360 animation_id, long node_count, real ratio, real weight,
	s_graph_inheritance *inheritance, dword const *node_mask, real_quaternion_transform *transforms)
{
	s_animation *animation = function_1daea0(graph, animation_id);
	s_animation_data data;
	real last_frame;

	function_1ddb40(&data, graph, animation_id);
	last_frame = (real)(animation->frame_count - 1) + 0.0001f;
	real const frame = PIN(last_frame * ratio, 0.0f, last_frame);
	function_2798a0(&data, frame, weight, graph, animation, node_count,
		inheritance, node_mask, transforms, true,
		false, 0.0f, 0.0f);
}

/* a transform with a quantized rotation (0x18 bytes) */
struct s_quantized_transform
{
	short rotation[4];
	point3f translation;
	real scale;
};

/* a node an animation places in object space (0x1c bytes): its transform
   relative to its parent, and which of its parts it sets */
struct s_object_space_parent_node
{
	short node_index;
	word rotation_flag : 1;
	word translation_flag : 1;
	word scale_flag : 1;
	word unknown02 : 13;
	s_quantized_transform transform;
};

__forceinline void quantized_transform_decompress(s_quantized_transform const *in, real_quaternion_transform *out)
{
	s_quantized_transform const *a = in;
	real_quaternion_transform *result = out;

	__asm
	{
		mov ecx, a
		mov eax, result
		movq mm3, [ecx]
		punpcklwd mm1, mm3
		punpckhwd mm2, mm3
		psrad mm1, 0x10
		psrad mm2, 0x10
		cvtpi2ps xmm1, mm1
		cvtpi2ps xmm2, mm2
		emms
		movlhps xmm1, xmm2
		movaps xmm0, xmm1
		mulps xmm0, xmm1
		movaps xmm3, xmm0
		shufps xmm3, xmm3, 0x4e
		addps xmm0, xmm3
		movaps xmm4, xmm0
		shufps xmm4, xmm4, 0x11
		addps xmm0, xmm4
		rsqrtps xmm0, xmm0
		mulps xmm1, xmm0
		movaps [eax], xmm1
	}
	out->position = in->translation;
	out->scale = in->scale;
}

/* the transforms of the nodes the object-space parent nodes set */
real_quaternion_transform g_502430[255];

/* the transform helpers (unknown_11cb00.cpp) */
void function_11dbb0(real_quaternion_transform *out, real_quaternion_transform const *a,
	real_quaternion_transform const *b);
void function_11dd80(real_quaternion_transform const *in, real_quaternion_transform *out);

#define NODE_MASK_SET(mask, index, value) \
	if (value) \
	{ \
		(mask)[(index) >> 5] |= 1 << ((index) & 31); \
	} \
	else \
	{ \
		(mask)[(index) >> 5] &= ~(1 << ((index) & 31)); \
	}

// @retail 0x27a100
bool __stdcall function_27a100(s_graph_tag *graph, s_animation *animation, s_graph_inheritance *inheritance,
	long node_count, real_quaternion_transform *transforms)
{
	bool result = false;
	long i;

	if (animation->object_space_parent_node_count <= 0)
		goto done;
	for (i = 0; i < animation->object_space_parent_node_count; i++)
	{
		s_object_space_parent_node *entry = &animation->object_space_parent_nodes[i];
		long node_index = entry->node_index;

		if (inheritance)
		{
			if (!(((dword *)inheritance->node_map_flags)[node_index >> 5] & (1 << (node_index & 31))))
			{
				continue;
			}
			node_index = ((short *)inheritance->node_map)[node_index];
		}
		if (node_index >= 0 && node_index < node_count)
		{
			s_graph_node *node = &graph->nodes[node_index];
			long parent_index = node->parent_index;
			__declspec(align(16)) real_quaternion_transform parent = transforms[node_index];
			__declspec(align(16)) real_quaternion_transform inverse;
			__declspec(align(16)) real_quaternion_transform local;
			__declspec(align(16)) real_quaternion_transform object_space;
			long child_index;

			while (parent_index != NONE)
			{
				function_11dbb0(&parent, &transforms[parent_index], &parent);
				parent_index = graph->nodes[parent_index].parent_index;
			}
			function_11dd80(&parent, &inverse);
			quantized_transform_decompress(&entry->transform, &local);
			function_11dbb0(&object_space, &inverse, &local);
			for (child_index = node->first_child_index; child_index >= 0;
				child_index = graph->nodes[child_index].next_sibling_index)
			{
				if (child_index < node_count)
				{
					g_502430[child_index] = object_space;
					NODE_MASK_SET(g_55e530, child_index, TEST_FIELD_BIT(entry->rotation_flag));
					NODE_MASK_SET(g_55e550, child_index, TEST_FIELD_BIT(entry->translation_flag));
					NODE_MASK_SET(g_55e570, child_index, TEST_FIELD_BIT(entry->scale_flag));
					result = true;
				}
			}
		}
	}
	done:
	return result;
}

// @retail 0x27a380
inline void node_mask_and(dword *mask, dword const *other)
{
	long i;

	for (i = 0; i < 8; i++)
	{
		mask[i] &= other[i];
	}
}

/* the samplers of each blend method, node kind, node mask and interpolation
   (0x27a6e0..0x28c090, below) */
void function_27a6e0(void);
void function_27aac0(void);
void function_27ace0(void);
void function_27b020(void);
void function_27b1d0(void);
void function_27b660(void);
void function_27b920(void);
void function_27bd60(void);
void function_27bfe0(void);
void function_27c490(void);
void function_27c750(void);
void function_27cb90(void);
void function_27ce20(void);
void function_27d420(void);
void function_27d770(void);
void function_27dd10(void);
void function_27dff0(void);
void function_27e6b0(void);
void function_27eae0(void);
void function_27f160(void);
void function_27f530(void);
void function_27fc10(void);
void function_280050(void);
void function_2806e0(void);
void function_280ac0(void);
void function_281090(void);
void function_281370(void);
void function_2818e0(void);
void function_281b60(void);
void function_2821f0(void);
void function_2825b0(void);
void function_282be0(void);
void function_282f60(void);
void function_283600(void);
void function_2839d0(void);
void function_284010(void);
void function_2843a0(void);
void function_284a20(void);
void function_284de0(void);
void function_285400(void);
void function_285750(void);
void function_285e90(void);
void function_286320(void);
void function_286a00(void);
void function_286e40(void);
void function_287590(void);
void function_287a30(void);
void function_288120(void);
void function_288570(void);
void function_288bd0(void);
void function_288f80(void);
void function_289580(void);
void function_2898b0(void);
void function_289fa0(void);
void function_28a3e0(void);
void function_28aaa0(void);
void function_28ae70(void);
void function_28b570(void);
void function_28b9c0(void);
void function_28c090(void);

// @retail 0x27a5f0
void function_27a5f0(void)
{
	if (g_sampling_settings.destination_node_mask)
	{
		if (g_sampling_settings.blend_frames)
		{
			function_27bfe0();
		}
		else
		{
			function_27c490();
		}
	}
	else
	{
		if (g_sampling_settings.blend_frames)
		{
			function_27c750();
		}
		else
		{
			function_27cb90();
		}
	}
}

// @retail 0x27a620
void function_27a620(void)
{
	if (g_sampling_settings.destination_node_mask)
	{
		if (g_sampling_settings.blend_frames)
		{
			function_27f530();
		}
		else
		{
			function_27fc10();
		}
	}
	else
	{
		if (g_sampling_settings.blend_frames)
		{
			function_280050();
		}
		else
		{
			function_2806e0();
		}
	}
}

// @retail 0x27a650
void function_27a650(void)
{
	if (g_sampling_settings.destination_node_mask)
	{
		if (g_sampling_settings.blend_frames)
		{
			function_282f60();
		}
		else
		{
			function_283600();
		}
	}
	else
	{
		if (g_sampling_settings.blend_frames)
		{
			function_2839d0();
		}
		else
		{
			function_284010();
		}
	}
}

// @retail 0x27a680
void function_27a680(void)
{
	if (g_sampling_settings.destination_node_mask)
	{
		if (g_sampling_settings.blend_frames)
		{
			function_286e40();
		}
		else
		{
			function_287590();
		}
	}
	else
	{
		if (g_sampling_settings.blend_frames)
		{
			function_287a30();
		}
		else
		{
			function_288120();
		}
	}
}

// @retail 0x27a6b0
void function_27a6b0(void)
{
	if (g_sampling_settings.destination_node_mask)
	{
		if (g_sampling_settings.blend_frames)
		{
			function_28ae70();
		}
		else
		{
			function_28b570();
		}
	}
	else
	{
		if (g_sampling_settings.blend_frames)
		{
			function_28b9c0();
		}
		else
		{
			function_28c090();
		}
	}
}

// @retail 0x27a3c0
void function_27a3c0(void)
{
	if (g_sampling_settings.node_kind == 0)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27a6e0();
			}
			else
			{
				function_27aac0();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27ace0();
			}
			else
			{
				function_27b020();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 1)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27b1d0();
			}
			else
			{
				function_27b660();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27b920();
			}
			else
			{
				function_27bd60();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 2)
	{
		function_27a5f0();
	}
}

// @retail 0x27a430
void function_27a430(void)
{
	if (g_sampling_settings.node_kind == 0)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27ce20();
			}
			else
			{
				function_27d420();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27d770();
			}
			else
			{
				function_27dd10();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 1)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27dff0();
			}
			else
			{
				function_27e6b0();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_27eae0();
			}
			else
			{
				function_27f160();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 2)
	{
		function_27a620();
	}
}

// @retail 0x27a4a0
void function_27a4a0(void)
{
	if (g_sampling_settings.node_kind == 0)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_280ac0();
			}
			else
			{
				function_281090();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_281370();
			}
			else
			{
				function_2818e0();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 1)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_281b60();
			}
			else
			{
				function_2821f0();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_2825b0();
			}
			else
			{
				function_282be0();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 2)
	{
		function_27a650();
	}
}

// @retail 0x27a510
void function_27a510(void)
{
	if (g_sampling_settings.node_kind == 0)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_2843a0();
			}
			else
			{
				function_284a20();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_284de0();
			}
			else
			{
				function_285400();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 1)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_285750();
			}
			else
			{
				function_285e90();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_286320();
			}
			else
			{
				function_286a00();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 2)
	{
		function_27a680();
	}
}

// @retail 0x27a580
void function_27a580(void)
{
	if (g_sampling_settings.node_kind == 0)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_288570();
			}
			else
			{
				function_288bd0();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_288f80();
			}
			else
			{
				function_289580();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 1)
	{
		if (g_sampling_settings.destination_node_mask)
		{
			if (g_sampling_settings.blend_frames)
			{
				function_2898b0();
			}
			else
			{
				function_289fa0();
			}
		}
		else
		{
			if (g_sampling_settings.blend_frames)
			{
				function_28a3e0();
			}
			else
			{
				function_28aaa0();
			}
		}
	}
	else if (g_sampling_settings.node_kind == 2)
	{
		function_27a6b0();
	}
}

// @retail 0x279860
void function_279860(void)
{
	if (g_sampling_settings.blend_method == 0)
	{
		function_27a3c0();
	}
	else if (g_sampling_settings.blend_method == 1)
	{
		function_27a430();
	}
	else if (g_sampling_settings.blend_method == 2)
	{
		function_27a4a0();
	}
	else if (g_sampling_settings.blend_method == 3)
	{
		function_27a510();
	}
	else if (g_sampling_settings.blend_method == 4)
	{
		function_27a580();
	}
}

/* the samplers: one
   for each blend method, node kind, destination mask and interpolation. Each
   runs the codec's decoders over the nodes the animation's bit flags select,
   component by component (rotation, translation, scale), and applies what
   they decode to the destination orientations.

   The blend methods: 0 decodes into the destination; 1 blends toward the
   decoded orientation by g_sampling_settings.blend_weight; 2 overlays it (rotations and scales
   multiply, translations add); 3 overlays it weighted by g_sampling_settings.blend_weight; 4 decodes
   into the destination, then moves it into its parent's object space.
   The node kinds: 0 maps node i to destination i; 1 maps nodes through the
   graph inheritance; 2 does too, and only samples the root's translation and
   scale. Interpolation decodes a second frame and blends toward it by
   g_sampling_settings.blend_fraction */

/* the two decoded orientations of an interpolation: the second frame's, and
   the first frame's when the method blends */
PRIVATE __declspec(align(16)) s_animation_output g_504410;
PRIVATE __declspec(align(16)) s_animation_output g_504430;

/* the sign bit of the first lane */
static __declspec(align(16)) dword const g_47ffc0[4] = { 0x80000000, 0, 0, 0 };

/* the normalized linear blend of two quaternions, along the shorter arc */
__forceinline void quaternion_blend(quaternionf *destination, quaternionf const *source, real fraction)
{
	__asm
	{
		mov eax, destination
		mov ebx, source
		movaps xmm0, [eax]
		movaps xmm7, xmm0
		movaps xmm2, [ebx]
		mulps xmm0, xmm2
		movhlps xmm1, xmm0
		addps xmm0, xmm1
		movaps xmm1, xmm0
		shufps xmm1, xmm1, 0x55
		addss xmm0, xmm1
		movss xmm3, fraction
		shufps xmm3, xmm3, 0
		andps xmm0, g_47ffc0
		shufps xmm0, xmm0, 0
		xorps xmm2, xmm0
		subps xmm2, xmm7
		mulps xmm2, xmm3
		addps xmm2, xmm7
		movaps xmm0, xmm2
		mulps xmm0, xmm2
		movhlps xmm7, xmm0
		addps xmm0, xmm7
		movaps xmm7, xmm0
		shufps xmm7, xmm7, 0x55
		addss xmm0, xmm7
		rsqrtss xmm0, xmm0
		shufps xmm0, xmm0, 0
		mulps xmm2, xmm0
		mov eax, destination
		movaps [eax], xmm2
	}
}

/* the product of two quaternions */
__forceinline void quaternion_multiply(quaternionf const *a, quaternionf const *b, quaternionf *result)
{
	__declspec(align(16)) dword sign[4] = { 0, 0, 0, 0x80000000 };

	__asm
	{
		mov eax, a
		movaps xmm0, [eax]
		mov eax, b
		movaps xmm1, [eax]
		movaps xmm3, xmm0
		movaps xmm7, xmm1
		shufps xmm0, xmm0, 0x24
		shufps xmm3, xmm3, 0xff
		shufps xmm7, xmm7, 0x3f
		mulps xmm3, xmm1
		mulps xmm7, xmm0
		shufps xmm0, xmm0, 0x49
		movaps xmm6, xmm1
		xorps xmm7, sign
		shufps xmm6, xmm6, 0x52
		addps xmm3, xmm7
		mulps xmm6, xmm0
		shufps xmm0, xmm0, 0x49
		xorps xmm6, sign
		shufps xmm1, xmm1, 0x89
		addps xmm3, xmm6
		mulps xmm0, xmm1
		mov eax, result
		subps xmm3, xmm0
		movaps [eax], xmm3
	}
}

/* the value a fraction of the way from a to b */
__forceinline real interpolate_real(real a, real b, real fraction)
{
	return (b - a) * fraction + a;
}

/* the linear blends of translations and scales */
__forceinline void orientation_vector_blend(s_animation_output *destination, s_animation_output const *source,
	real const &fraction)
{
	destination->vector.i = (source->vector.i - destination->vector.i) * fraction + destination->vector.i;
	destination->vector.j = (source->vector.j - destination->vector.j) * fraction + destination->vector.j;
	destination->vector.k = (source->vector.k - destination->vector.k) * fraction + destination->vector.k;
}

__forceinline void orientation_scale_blend(s_animation_output *destination, s_animation_output const *source,
	real const &fraction)
{
	destination->scale = (source->scale - destination->scale) * fraction + destination->scale;
}

/* the destination of a node, or false when it has none */
__forceinline bool node_destination_get(s_graph_inheritance const *inheritance, long node_kind, long node_index, long *destination_index)
{
	if (node_kind == 0)
	{
		*destination_index = node_index;
		return true;
	}
	if (((dword const *)inheritance->node_map_flags)[node_index >> 5] & (1 << (node_index & 31)))
	{
		long index = ((short const *)inheritance->node_map)[node_index];

		if (index >= 0 && index < g_sampling_settings.destination_node_count)
		{
			*destination_index = index;
			return true;
		}
	}
	return false;
}

/* whether a node's bit is set in a node mask */
__forceinline bool node_flags_test(dword const *flags, long index)
{
	bool result = (flags[index >> 5] & (1 << (index & 31))) != 0;

	return result;
}

__forceinline void component_decompress(long component)
{
	if (component == 0)
	{
		g_sampling_settings.decompressors.rotation();
	}
	else if (component == 1)
	{
		g_sampling_settings.decompressors.translation();
	}
	else
	{
		g_sampling_settings.decompressors.scale();
	}
}

/* interpolates the decoded orientation toward the second frame's */
__forceinline void component_interpolate(long component)
{
	if (component == 0)
	{
		quaternion_blend(&g_504430.rotation, &g_504410.rotation, g_sampling_settings.blend_fraction);
	}
	else if (component == 1)
	{
		g_504430.vector.i = interpolate_real(g_504430.vector.i, g_504410.vector.i, g_sampling_settings.blend_fraction);
		g_504430.vector.j = interpolate_real(g_504430.vector.j, g_504410.vector.j, g_sampling_settings.blend_fraction);
		g_504430.vector.k = interpolate_real(g_504430.vector.k, g_504410.vector.k, g_sampling_settings.blend_fraction);
	}
	else
	{
		orientation_scale_blend(&g_504430, &g_504410, g_sampling_settings.blend_fraction);
	}
}

/* scales the root's decoded vertical translation by the inherited graph's
   root z offset */
__forceinline void root_offset_scale(long node_kind, long component, long node_index)
{
	if (component == 1 && node_kind != 0 && node_index == 0)
	{
		g_504430.vector.k = g_sampling_settings.inheritance->root_z_offset * g_504430.vector.k;
	}
}

/* applies the decoded orientation to the destination */
__forceinline void component_apply(long blend_method, long component, s_animation_output *destination, long node_index)
{
	if (blend_method == 1)
	{
		if (component == 0)
		{
			quaternion_blend(&destination->rotation, &g_504430.rotation, g_sampling_settings.blend_weight);
		}
		else if (component == 1)
		{
			orientation_vector_blend(destination, &g_504430, g_sampling_settings.blend_weight);
		}
		else
		{
			orientation_scale_blend(destination, &g_504430, g_sampling_settings.blend_weight);
		}
	}
	else if (blend_method == 2)
	{
		if (component == 0)
		{
			quaternion_multiply(&destination->rotation, &g_504430.rotation, &destination->rotation);
		}
		else if (component == 1)
		{
			destination->vector.i = destination->vector.i + g_504430.vector.i;
			destination->vector.j = destination->vector.j + g_504430.vector.j;
			destination->vector.k = destination->vector.k + g_504430.vector.k;
		}
		else
		{
			destination->scale = destination->scale * g_504430.scale;
		}
	}
	else if (blend_method == 3)
	{
		if (component == 0)
		{
			/* scales the decoded rotation toward the identity (the one on
			   the same side as the rotation): the offset from the identity,
			   scaled, added back to it */
			real one = 1.0f;
			real fraction = g_sampling_settings.blend_weight;

			*(dword *)&one |= *(dword *)&g_504430.rotation.w & 0x80000000;
			g_504430.rotation.w -= one;
			g_504430.rotation.i *= fraction;
			g_504430.rotation.j *= fraction;
			g_504430.rotation.k *= fraction;
			g_504430.rotation.w *= fraction;
			g_504430.rotation.w += one;
			quaternion_multiply(&destination->rotation, &g_504430.rotation, &destination->rotation);
		}
		else if (component == 1)
		{
			destination->vector.i = g_sampling_settings.blend_weight * g_504430.vector.i + destination->vector.i;
			destination->vector.j = g_sampling_settings.blend_weight * g_504430.vector.j + destination->vector.j;
			destination->vector.k = g_sampling_settings.blend_weight * g_504430.vector.k + destination->vector.k;
		}
		else
		{
			destination->scale = ((g_504430.scale - 1.0f) * g_sampling_settings.blend_weight + 1.0f) * destination->scale;
		}
	}
	else if (blend_method == 4)
	{
		if (component == 0)
		{
			if (node_flags_test(g_55e530, node_index))
			{
				quaternion_multiply(&g_502430[node_index].rotation, &g_5044c0->rotation, &g_5044c0->rotation);
			}
		}
		else if (component == 1)
		{
			if (node_flags_test(g_55e550, node_index))
			{
				s_animation_output *current = g_5044c0;

				current->vector.i = g_502430[node_index].position.x + current->vector.i;
				current->vector.i = g_502430[node_index].position.y + current->vector.i;
				current->vector.i = g_502430[node_index].position.z + current->vector.i;
			}
		}
		else
		{
			if (node_flags_test(g_55e570, node_index))
			{
				real scale = g_502430[node_index].scale;

				g_5044c0->scale = scale * g_5044c0->scale;
			}
		}
	}
}

__forceinline void compute_component_orientations(long blend_method, long node_kind, bool destination_mask, bool interpolate, long component, byte const *&bit_flags, long &node_index, long node_count, s_graph_inheritance const *const &inheritance)
{
	bool in_place = blend_method == 0 || blend_method == 4;
	s_animation_output *destination = (s_animation_output *)g_sampling_settings.field_4c;

	g_5044c0 = in_place ? destination : &g_504430;
	bit_flags = component == 0 ? g_sampling_settings.rotation_bit_flags : (component == 1 ? g_sampling_settings.translation_bit_flags : g_sampling_settings.scale_bit_flags);
	for (node_index = 0; node_index < node_count; )
	{
		long flags = *bit_flags++;

		if (flags == 0)
		{
			node_index += 8;
			if (node_kind == 0)
			{
				if (in_place)
				{
					g_5044c0 += 8;
				}
				else
				{
					destination += 8;
				}
			}
		}
		else
		{
			long last = node_index + 8 > node_count ? node_count : node_index + 8;

			for (; node_index < last; node_index++)
			{
				if (flags & 1)
				{
					long destination_index;

					if (node_destination_get(inheritance, node_kind, node_index, &destination_index) &&
						(!destination_mask || node_flags_test(g_sampling_settings.destination_node_mask, destination_index)))
					{
						if (node_kind != 0)
						{
							destination = (s_animation_output *)g_sampling_settings.field_4c + destination_index;
							if (in_place)
							{
								g_5044c0 = destination;
							}
						}
						component_decompress(component);
						if (!interpolate && !in_place)
						{
							root_offset_scale(node_kind, component, node_index);
						}
						long frame_index;
						long frame_index2;
						real frame_fraction;

						if (interpolate)
						{
							frame_index = g_sampling_settings.frame_index;
							frame_index2 = g_sampling_settings.next_frame_index;
							frame_fraction = g_sampling_settings.frame_fraction;
							g_sampling_settings.frame_index = g_sampling_settings.blend_frame_index;
							g_sampling_settings.next_frame_index = g_sampling_settings.blend_next_frame_index;
							g_sampling_settings.frame_fraction = g_sampling_settings.blend_frame_fraction;
							g_5044c0 = &g_504410;
							component_decompress(component);
							component_interpolate(component);
							if (blend_method != 4)
							{
								root_offset_scale(node_kind, component, node_index);
							}
							if (blend_method == 0)
							{
								__assume(0);
							}
						}
						component_apply(blend_method, component, destination, node_kind == 0 ? node_index : destination_index);
						if (interpolate)
						{
							g_sampling_settings.frame_index = frame_index;
							g_sampling_settings.next_frame_index = frame_index2;
							g_sampling_settings.frame_fraction = frame_fraction;
							g_5044c0 = &g_504430;
						}
					}
					if (component == 0)
					{
						g_5044b4++;
					}
					else if (component == 1)
					{
						g_5044b8++;
					}
					else
					{
						g_5044bc++;
					}
				}
				if (node_kind == 0)
				{
					if (in_place)
					{
						g_5044c0++;
					}
					else
					{
						destination++;
					}
				}
				flags >>= 1;
			}
		}
	}
}

__forceinline void compute_orientations(long blend_method, long node_kind, bool destination_mask, bool interpolate)
{
	long node_index;
	byte const *bit_flags;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 0, bit_flags, node_index, node_count, g_sampling_settings.inheritance);
	if (node_kind == 2 && node_count > 1)
	{
		node_count = 1;
	}
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 1, bit_flags, node_index, node_count, g_sampling_settings.inheritance);
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 2, bit_flags, node_index, node_count, g_sampling_settings.inheritance);
}

__forceinline void function_288573(long arg_0, long arg_1, s_animation_output *arg_2, long arg_3)
{
	if (arg_0 == 1)
	{
		if (arg_1 == 0)
		{
			quaternion_blend(&arg_2->rotation, &g_504430.rotation, g_sampling_settings.blend_weight);
		}
		else if (arg_1 == 1)
		{
			orientation_vector_blend(arg_2, &g_504430, g_sampling_settings.blend_weight);
		}
		else
		{
			orientation_scale_blend(arg_2, &g_504430, g_sampling_settings.blend_weight);
		}
	}
	else if (arg_0 == 2)
	{
		if (arg_1 == 0)
		{
			quaternion_multiply(&arg_2->rotation, &g_504430.rotation, &arg_2->rotation);
		}
		else if (arg_1 == 1)
		{
			arg_2->vector.i = arg_2->vector.i + g_504430.vector.i;
			arg_2->vector.j = arg_2->vector.j + g_504430.vector.j;
			arg_2->vector.k = arg_2->vector.k + g_504430.vector.k;
		}
		else
		{
			arg_2->scale = arg_2->scale * g_504430.scale;
		}
	}
	else if (arg_0 == 3)
	{
		if (arg_1 == 0)
		{
			/* scales the decoded rotation toward the identity (the one on
			   the same side as the rotation): the offset from the identity,
			   scaled, added back to it */
			real one = 1.0f;
			real fraction = g_sampling_settings.blend_weight;

			*(dword *)&one |= *(dword *)&g_504430.rotation.w & 0x80000000;
			g_504430.rotation.w -= one;
			g_504430.rotation.i *= fraction;
			g_504430.rotation.j *= fraction;
			g_504430.rotation.k *= fraction;
			g_504430.rotation.w *= fraction;
			g_504430.rotation.w += one;
			quaternion_multiply(&arg_2->rotation, &g_504430.rotation, &arg_2->rotation);
		}
		else if (arg_1 == 1)
		{
			arg_2->vector.i = g_sampling_settings.blend_weight * g_504430.vector.i + arg_2->vector.i;
			arg_2->vector.j = g_sampling_settings.blend_weight * g_504430.vector.j + arg_2->vector.j;
			arg_2->vector.k = g_sampling_settings.blend_weight * g_504430.vector.k + arg_2->vector.k;
		}
		else
		{
			arg_2->scale = ((g_504430.scale - 1.0f) * g_sampling_settings.blend_weight + 1.0f) * arg_2->scale;
		}
	}
	else if (arg_0 == 4)
	{
		if (arg_1 == 0)
		{
			if (node_flags_test(g_55e530, arg_3))
			{
				quaternion_multiply(&g_502430[arg_3].rotation, &g_5044c0->rotation, &g_5044c0->rotation);
			}
		}
		else if (arg_1 == 1)
		{
			if (node_flags_test(g_55e550, arg_3))
			{
				s_animation_output *local_0 = g_5044c0;

				local_0->vector.i = g_502430[arg_3].position.x + local_0->vector.i;
				local_0->vector.i = g_502430[arg_3].position.y + local_0->vector.i;
				local_0->vector.i = g_502430[arg_3].position.z + local_0->vector.i;
			}
		}
		else
		{
			if (node_flags_test(g_55e570, arg_3))
			{
				s_animation_output *local_0 = g_5044c0;
				real local_1 = g_502430[arg_3].scale;

				local_0->scale = local_0->scale * local_1;
			}
		}
	}
}

__forceinline void function_288572(long arg_0, long arg_1, bool arg_2,
	bool arg_3, long arg_4, long &arg_5, byte const *&arg_6, long arg_7,
	s_graph_inheritance const *const &arg_8)
{
	bool local_0 = arg_0 == 0 || arg_0 == 4;
	s_animation_output *local_1 = (s_animation_output *)g_sampling_settings.field_4c;

	g_5044c0 = local_0 ? local_1 : &g_504430;
	arg_6 = arg_4 == 0 ? g_sampling_settings.rotation_bit_flags : (arg_4 == 1 ? g_sampling_settings.translation_bit_flags : g_sampling_settings.scale_bit_flags);
	for (arg_5 = 0; arg_5 < arg_7; )
	{
		long local_2 = *arg_6++;

		if (local_2 == 0)
		{
			arg_5 += 8;
			if (arg_1 == 0)
			{
				if (local_0)
				{
					g_5044c0 += 8;
				}
				else
				{
					local_1 += 8;
				}
			}
		}
		else
		{
			long local_3 = arg_5 + 8 > arg_7 ? arg_7 : arg_5 + 8;

			for (; arg_5 < local_3; arg_5++)
			{
				if (local_2 & 1)
				{
					long local_4;

					if (node_destination_get(arg_8, arg_1, arg_5, &local_4) &&
						(!arg_2 || node_flags_test(g_sampling_settings.destination_node_mask, local_4)))
					{
						if (arg_1 != 0)
						{
							local_1 = (s_animation_output *)g_sampling_settings.field_4c + local_4;
							if (local_0)
							{
								g_5044c0 = local_1;
							}
						}
						component_decompress(arg_4);
						if (!arg_3 && !local_0)
						{
							root_offset_scale(arg_1, arg_4, arg_5);
						}
						if (arg_3)
						{
							dword local_5 = g_sampling_settings.frame_index;
							long local_6 = g_sampling_settings.next_frame_index;
							real local_7 = g_sampling_settings.frame_fraction;

							g_sampling_settings.frame_index = g_sampling_settings.blend_frame_index;
							g_sampling_settings.next_frame_index = g_sampling_settings.blend_next_frame_index;
							g_sampling_settings.frame_fraction = g_sampling_settings.blend_frame_fraction;
							g_5044c0 = &g_504410;
							component_decompress(arg_4);
							component_interpolate(arg_4);
							if (arg_0 != 4)
							{
								root_offset_scale(arg_1, arg_4, arg_5);
							}
							function_288573(arg_0, arg_4, local_1, arg_1 == 0 ? arg_5 : local_4);
							g_sampling_settings.frame_index = local_5;
							g_sampling_settings.next_frame_index = local_6;
							g_sampling_settings.frame_fraction = local_7;
							g_5044c0 = &g_504430;
						}
						else
						{
							function_288573(arg_0, arg_4, local_1, arg_1 == 0 ? arg_5 : local_4);
						}
					}
					if (arg_4 == 0)
					{
						g_5044b4++;
					}
					else if (arg_4 == 1)
					{
						g_5044b8++;
					}
					else
					{
						g_5044bc++;
					}
				}
				if (arg_1 == 0)
				{
					if (local_0)
					{
						g_5044c0++;
					}
					else
					{
						local_1++;
					}
				}
				local_2 >>= 1;
			}
		}
	}
}

/* compute_orientations with the decoded orientation applied in each arm of
   the interpolation test (compute_component_orientations applies it once,
   after the test) */
__forceinline void function_288571(long arg_0, long arg_1, bool arg_2, bool arg_3)
{
	long local_0;
	byte const *local_1;
	long local_2 = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	function_288572(arg_0, arg_1, arg_2, arg_3, 0, local_0, local_1, local_2, g_sampling_settings.inheritance);
	if (arg_1 == 2 && local_2 > 1)
	{
		local_2 = 1;
	}
	function_288572(arg_0, arg_1, arg_2, arg_3, 1, local_0, local_1, local_2, g_sampling_settings.inheritance);
	function_288572(arg_0, arg_1, arg_2, arg_3, 2, local_0, local_1, local_2, g_sampling_settings.inheritance);
}

/* compute_orientations with a node index for each component pass */
__forceinline void compute_orientations_split(long blend_method, long node_kind, bool destination_mask, bool interpolate)
{
	long rotation_index;
	long translation_index;
	long scale_index;
	byte const *bit_flags;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 0, bit_flags, rotation_index, node_count, g_sampling_settings.inheritance);
	if (node_kind == 2 && node_count > 1)
	{
		node_count = 1;
	}
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 1, bit_flags, translation_index, node_count, g_sampling_settings.inheritance);
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 2, bit_flags, scale_index, node_count, g_sampling_settings.inheritance);
}

/* compute_component_orientations with every loop variable passed by
   reference, so a sampler can share each one across the three passes or give
   each pass its own: which variables are shared decides VC's register and
   operand choices, and it differs between samplers */
__forceinline void compute_component_orientations_shared(long blend_method, long node_kind, bool destination_mask,
	bool interpolate, long component, long node_count, long &node_index, byte const *&bit_flags, long &destination_index, long &flags, long &last)
{
	bool in_place = blend_method == 0 || blend_method == 4;
	s_animation_output *destination = (s_animation_output *)g_sampling_settings.field_4c;

	g_5044c0 = in_place ? destination : &g_504430;
	bit_flags = component == 0 ? g_sampling_settings.rotation_bit_flags : (component == 1 ? g_sampling_settings.translation_bit_flags : g_sampling_settings.scale_bit_flags);
	for (node_index = 0; node_index < node_count; )
	{
		flags = *bit_flags++;

		if (flags == 0)
		{
			node_index += 8;
			if (node_kind == 0)
			{
				if (in_place)
				{
					g_5044c0 += 8;
				}
				else
				{
					destination += 8;
				}
			}
		}
		else
		{
			last = node_index + 8 > node_count ? node_count : node_index + 8;

			for (; node_index < last; node_index++)
			{
				if (flags & 1)
				{
					if (node_destination_get(g_sampling_settings.inheritance, node_kind, node_index, &destination_index) &&
						(!destination_mask || node_flags_test(g_sampling_settings.destination_node_mask, destination_index)))
					{
						if (node_kind != 0)
						{
							destination = (s_animation_output *)g_sampling_settings.field_4c + destination_index;
							if (in_place)
							{
								g_5044c0 = destination;
							}
						}
						component_decompress(component);
						if (!interpolate && !in_place)
						{
							root_offset_scale(node_kind, component, node_index);
						}
						if (interpolate)
						{
							dword frame_index = g_sampling_settings.frame_index;
							long frame_index2 = g_sampling_settings.next_frame_index;
							real frame_fraction = g_sampling_settings.frame_fraction;

							g_sampling_settings.frame_index = g_sampling_settings.blend_frame_index;
							g_sampling_settings.next_frame_index = g_sampling_settings.blend_next_frame_index;
							g_sampling_settings.frame_fraction = g_sampling_settings.blend_frame_fraction;
							g_5044c0 = &g_504410;
							component_decompress(component);
							component_interpolate(component);
							if (blend_method != 4)
							{
								root_offset_scale(node_kind, component, node_index);
							}
							if (blend_method == 0)
							{
								__assume(0);
							}
							component_apply(blend_method, component, destination, node_kind == 0 ? node_index : destination_index);
							g_sampling_settings.frame_index = frame_index;
							g_sampling_settings.next_frame_index = frame_index2;
							g_sampling_settings.frame_fraction = frame_fraction;
							g_5044c0 = &g_504430;
						}
						else
						{
							component_apply(blend_method, component, destination, node_kind == 0 ? node_index : destination_index);
						}
					}
					if (component == 0)
					{
						g_5044b4++;
					}
					else if (component == 1)
					{
						g_5044b8++;
					}
					else
					{
						g_5044bc++;
					}
				}
				if (node_kind == 0)
				{
					if (in_place)
					{
						g_5044c0++;
					}
					else
					{
						destination++;
					}
				}
				flags >>= 1;
			}
		}
	}
}

__forceinline void compute_orientations_split_index_last(long blend_method, long node_kind, bool destination_mask, bool interpolate)
{
	long rotation_node_index;
	long translation_node_index;
	long scale_node_index;
	byte const *bit_flags;
	long destination_index;
	long flags;
	long rotation_last;
	long translation_last;
	long scale_last;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations_shared(blend_method, node_kind, destination_mask, interpolate, 0, node_count, rotation_node_index, bit_flags, destination_index, flags, rotation_last);
	if (node_kind == 2 && node_count > 1)
	{
		node_count = 1;
	}
	compute_component_orientations_shared(blend_method, node_kind, destination_mask, interpolate, 1, node_count, translation_node_index, bit_flags, destination_index, flags, translation_last);
	compute_component_orientations_shared(blend_method, node_kind, destination_mask, interpolate, 2, node_count, scale_node_index, bit_flags, destination_index, flags, scale_last);
}

/* compute_orientations with the node count of the translation and scale
   passes (only the root's for kind 2) in its own variable */
__forceinline void compute_orientations_component_count(long blend_method, long node_kind, bool destination_mask, bool interpolate)
{
	long node_index;
	byte const *bit_flags;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 0, bit_flags, node_index, node_count, g_sampling_settings.inheritance);
	long component_count = node_kind == 2 && node_count > 1 ? 1 : node_count;
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 1, bit_flags, node_index, component_count, g_sampling_settings.inheritance);
	compute_component_orientations(blend_method, node_kind, destination_mask, interpolate, 2, bit_flags, node_index, component_count, g_sampling_settings.inheritance);
}

__forceinline void compute_orientations_inheritance_mapping(long blend_method, long node_kind, bool destination_mask, bool interpolate);

// @retail 0x27a6e0
void function_27a6e0(void)
{
	compute_orientations(0, 0, true, true);
}

// @retail 0x27aac0
void function_27aac0(void)
{
	compute_orientations(0, 0, true, false);
}

// @retail 0x27ace0
void function_27ace0(void)
{
	compute_orientations(0, 0, false, true);
}

// @retail 0x27b020
void function_27b020(void)
{
	compute_orientations(0, 0, false, false);
}

// @retail 0x27b1d0
void function_27b1d0(void)
{
	compute_orientations_inheritance_mapping(0, 1, true, true);
}

// @retail 0x27b660
void function_27b660(void)
{
	compute_orientations(0, 1, true, false);
}

// @retail 0x27b920
void function_27b920(void)
{
	compute_orientations_inheritance_mapping(0, 1, false, true);
}

// @retail 0x27bd60
void function_27bd60(void)
{
	compute_orientations(0, 1, false, false);
}

// @retail 0x27bfe0
void function_27bfe0(void)
{
	compute_orientations_inheritance_mapping(0, 2, true, true);
}

// @retail 0x27c490
void function_27c490(void)
{
	compute_orientations(0, 2, true, false);
}

// @retail 0x27c750
void function_27c750(void)
{
	compute_orientations_inheritance_mapping(0, 2, false, true);
}

// @retail 0x27cb90
void function_27cb90(void)
{
	compute_orientations(0, 2, false, false);
}

// @retail 0x27ce20
void function_27ce20(void)
{
	compute_orientations(1, 0, true, true);
}

// @retail 0x27d420
void function_27d420(void)
{
	compute_orientations(1, 0, true, false);
}

// @retail 0x27d770
void function_27d770(void)
{
	compute_orientations(1, 0, false, true);
}

// @retail 0x27dd10
void function_27dd10(void)
{
	compute_orientations(1, 0, false, false);
}

// score 2 (was 2): r131, r232 and r234 unchanged. The two stack slots are read in the opposite order inside the inlined helper.
// r133 unchanged. The two stack slots are read in the opposite order inside the inlined helper.
// The order is a register-allocator tie that follows the constants live in the walk's loops (any
// new one flips it) and not the source shape. Tried: blend-1 translation and scale through
// interpolate_real 159; a node lookup returning the index or NONE 382; the per-arm walk as a
// template 2114 (out of line); orientation_vector_blend taking the fraction by value 147.
// Round 40 (screen): the existing compute_component_orientations_shared with each of node index, bit
// flags, destination index, flags and last shared or per pass (32 forms; the 0x2898b0 form matched
// that function) best 4 (flags and last per pass, with or without the destination index: the same tie in
// two places), the rest 22-83; on the two best, declarations by kind or by pass, count first or last 4.
// Round 100 (lane 7, --fast): the per-arm apply reading the blend weight into a local at its top 2,
// in the blend-1 arm (all three components) 147; the flags test as `!= 0` 2; the apply index in a
// named local 2. Compile-only screens: the walk's bound as an if or with the compare reversed, the
// frame-state saves or restores reversed and the pass counters as a switch keep the tie and change
// matched neighbours; a constant subtraction before the blend-1 rotation apply (semantics changing)
// is the only form that gives retail's order.
// Round 98: inheritance pointer by value 399; the apply through the existing component_apply 2;
// the alternate existing node lookup 2; rotation and translation cases swapped 2; vector blending
// through interpolate_real 155; scale blending through interpolate_real 42; root scaling through
// interpolate_real 2; blend-1 and blend-2 arms swapped 2. All restored.
// Round 99: the blend fraction by const reference 338; a source pointer local 307; quaternion source
// and destination parameters swapped 2; the saved fraction declared first 2; the orchestration output
// and cursor declarations swapped 2; a const in-place predicate 2; output selection as if/else 2;
// a constant-initialized blend-weight local 2. All restored.
// Round 194 (fast): the wrapper written out as three function_288572 calls, count declared last/first
// 2/2, node index per pass 20, bit flags per pass 40, both 44; in function_288572 the destination index
// declared at the inner body's top 2, with the last node 2, the frame-state saves declared before the
// interpolation test 2, the inheritance read from the global 2, flags at function scope 2; in
// function_288571 the later passes' count as a ternary copy 2 (0x281b60 39), as a clamped copy 2,
// separate translation and scale copies 2 (0x288570 120). All restored.
// Round 79 (fast; 0x281b60 has the same tie through function_288572): the count by const reference 2; the clamp operands reversed 53;
// the flags byte read and pointer step split 2; the shift before the tail steps 2 (0x288570 12); a per-pass count copy 2 (0x281b60 62,
// 0x288570 120); the shift in the for increment 12.
// Round 86 (fast and compile-only over all six walk users): k identity locals at the walk's top, k = 1..8, 2 each (no period, the
// reload pair does not follow symbol numbering); bit flags declared first in the wrapper, passed by reference 39 (0x281b60 87, 0x288570 152; compile-only shows no change, so not usable there); all 24 orders of the
// index, flags pointer, count and inheritance parameters 2 each; natural rewrites of the arms of other blend methods (+=, *=, named
// weight, interpolate_real, no scale local) 2 each, so arms folded away do not count; saved frame state as dword or long, fraction
// first 2, next index first 6; apply index named in both arms 2; counters pre-incremented or += 1 2; the group end with the compare reversed 8;
// a named table pointer for the outputs 2.
// Round 67 (m3 lane 14, fast; 0x281b60 moves with it): in function_288572 the inner bound as an if-clamp
// after `arg_5 + 8` 208 (0x281b60 241, 0x288570 401), as `local_3 = arg_7; if (arg_5 + 8 < local_3) ...` 5
// (`jge` for retail's `jg`, the reload pair unchanged; 0x281b60 64); the outer for as a while 2; the
// output selection as if/else 2; in function_288573 the blend-1 rotation weight named 2.
// Round 140 (lane 8, fast with the four matched walk users): the wrapper's count local renamed
// `node_count` or `count` 2/2 and the walk's bit-flags local renamed 2 (the reload pair is not a name
// tie); the bit flags as int 2; the count parameter `long const` 2 (0x281b60 2 throughout). The two
// reloads sit on the bit-set path only (the bit-clear path joins at the bound load), after the blend-1
// asm that clobbers EBX; blend 3 (0x285750, matched) applies through C code and the EAX-only multiply.
// Round 141 (m3 lane 8, fast over all 78 functions of the file; 0x281b60 2 throughout): the sign mask as a
// local static inside the blend 2 (no change); the two decode outputs as one two-element array 352 (1742
// bytes, the file's walk users all regress); the interpolation's translation through orientation_vector_blend
// 2 (13 matched walk users regress); the destination-mask test and the inheritance map test through
// BIT_VECTOR_TEST 2/2; the bit flags declared at the walk's top ahead of a count copy 2 (0x288570 120).
// Round 60 (m3 lane 2, fast over all 78 functions of the file, 0x281b60 2 throughout): in function_288572 the output
// pointer as an array element address 2; the empty-group test as `!local_2` 2; the bit-flags selection as an if/else-if
// chain 2; the destination lookup and mask test as nested ifs 2; the destination index initialised to NONE 2 (all the
// same code).
// Round 89 (m3 lane 2, fast with the four matched walk users): the shared walk (function_288571/2/3) as
// __forceinline function templates over method, kind, mask, interpolation and component 2114 (0x281b60
// 2048; every instance stays out of line, as round 100's template); the blend asm's sign-bit constant
// typed as reals with -0.0f 2; in function_288572 the output cursor declared ahead of the in-place flag
// 2 (0x288570 3, 0x289580 78); the inheritance map entry read as short 382 (36 matched walk users
// regress); this function and 0x281b60 through the existing compute_orientations 4 (a second swapped
// pair in pass 1: the apply-in-each-arm form is what fixes that pass); the frame-state saves declared
// at the walk helper's top 2. Diagnostics: the apply as a member of a type derived from the output, 2
// (same code); a probe global store at the wrapper's end 2. The pair is in pass 0 only (flags in EBX
// after the blend asm, count in ECX after the decompressor calls). All restored.
// Round 174 (m3 lane 8, fast with 0x281b60 and the five matched walk users): the multiply's sign mask as a
// const local 2; the apply helper as plain inline 2 (still inlined); the pass walk as plain inline 1995 (out
// of line, every walk user regresses); the node flag test returning its expression 2; the inheritance map
// test through the existing node flag test 2 (0x289580 78). Diagnostic: the kind-2 count clamp removed 2,
// so the clamp's second definition of the count is not the tie. Retail has no float literal here (only
// the blend's sign constant); matched 0x285750 references its 1.0f twice. All restored.
// Round 253 (m3 lane 3, fast over all 78 functions of the file; 0x281b60 alongside): in function_288572 a
// redundant `if (0 < count)` guard around the group loop 56 (0x281b60 97, 0x285750 56, 0x288570 51, 0x289580
// 13, 0x28aaa0 10); the same guard around the inner loop 2 (same code); the bit flags bound to a `long &`
// for the inner loop 2 (0x288570 120); the count bound to a `long const &` at the group clamp 2 (0x281b60
// 39, 0x288570 120); the pass counters as a switch on the component 2 (0x288570 3, 0x289580 78). All restored.
// Round 132 (m3 lane 9, fast with 0x281b60 and the five matched walk users): this function and 0x281b60 through each
// of the file's other existing wrappers: compute_orientations_inheritance_mapping 417 (1749 bytes; 0x281b60 397),
// compute_orientations_component_count 4 (0x281b60 41), compute_orientations_split 22 (0x281b60 72),
// compute_orientations_split_index_last 29 (0x281b60 62); compute_orientations re-measured 4 (0x281b60 4).
// Diagnostic: the scale pass given the node count from the settings global 154 (1738 bytes, every walk user
// regresses), but the rotation pass's pair then reloads the flags first as retail does: the order follows the
// count's weight against the flags'. Reference-bound locals: the wrapper's count as `long node_count = ...; long
// &count = node_count;` 2 (0x281b60 39, 0x288570 3, 0x289580 78); the group end bound to a `long &` 2 (0x288570
// 120); the destination index bound to a `long &` 2 (0x288570 120); the walk's count parameter as `long &` 2
// (0x281b60 2, the five matched users keep). All restored.
// M3 lane 13 r106 (fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0): in function_288572 the
// three counter increments through a forceinline (component) step helper 2 (0x281b60 2); the empty-group test as `!= 0`
// with the arms swapped 377 (1735 bytes; 0x281b60 352, every walk user regresses); in function_288571 the counter resets
// as one chained assignment 2 (0x281b60 2, 0x288570 120); the node index zeroed in the wrapper before each pass instead of
// in the walk's for 6 (0x281b60 6, 0x285750 4, 0x288570 2, 0x289580 5). Diagnostics (semantics changed): the empty
// group's step or the group end's `+ 8` as 9 or 7, the bit test as `& 3`: the pair stays reversed in both functions.
// Also the interpolation's three frame-state restores in reverse order 14 (0x281b60 14, 0x285750 12, 0x288570 19).
// M3 lane 5 r198 (fast over all 78 functions of the file, 0x281b60 alongside; control 2/2 with 76 matched): the three
// channel counters reached as members just past the settings global (they follow it in memory), in the walk's
// increments and the wrapper's resets (diagnostic) 2 (0x281b60 2, 0x285750 17, 0x289580 78); only the increments or only
// the resets that way 387 (0x281b60 4, 0x285750 21, 0x28aaa0 4); the output pointer as such a member in the walk 2
// (0x281b60 2, 0x285750 17, 0x289580 203, 0x28aaa0 279); the blend-1 rotation through a new inline wrapper taking the
// two orientations and the weight by reference 2; the blend-2 rotation through such a wrapper 2 (0x281b60 2; both the
// same code everywhere); the blend-2 translation and scale through new inline (destination, source) helpers 2 (0x281b60
// 2, 0x285750 33, 0x289580 78). Diagnostics: 1 to 4 unused inline helper definitions ahead of function_288573, empty or
// with pointer parameters, change nothing in the file; each blend method's arm of function_288573 emptied in turn leaves
// the pair as it is in the function that does not use that arm (0x281b60 2 without the blend-1, 3 or 4 arm, this
// function 2 without the blend-2, 3 or 4 arm), while matched walk users change with arms they do not use (0x289580 78
// without the blend-1 or blend-3 arm, 0x285750 33 and 0x28aaa0 92 without the blend-2 arm). So the pair does not follow
// the folded arms or the counters' symbols; helper calls added in any arm move the other walk users first.
// M3 lane 8 r183 (fast with 0x281b60 and the five matched walk users): in function_288572 the count read through a
// no-code forceinline accessor at the group clamp 2 (0x281b60 2, 0x288570 120), at the outer loop bound 2 (0x288570 120);
// the bit test through the accessor 2; the group end declared in the inner for's init 2; the blend frame state copied in
// reverse order before the second decode 11 (0x281b60 11, 0x285750 9, 0x288570 9; the pair unchanged). All restored.
// M3 lane 1 r123 (a compile-only build of the file is exact here: this function 2, 0x281b60 2, 0x285750 0; agreed with
// --fast): all 243 placements of the walk's node index, bit-flags pointer, flags, group end and destination index as
// walk-local, one wrapper local by reference or one per pass (apply in each arm) best 2 (the current one); a variant walk
// for this function and 0x281b60 only, 600 random combinations of those placements with the clamp operands reversed, the
// counters as a switch, the empty-group step after the pointer advance, the in-place flag declared after the output
// pointer, the saves or the restores fraction first: best 2. The interpolation's output restore between the frame
// restores 8 (--fast; 0x281b60 8, 0x285750 6), before them 13; the output store to the second frame ahead of the frame
// swaps 14; the frame restores reversed 12; the output saved and restored through a local 337; the frame state as one
// 12-byte aggregate copy (integer copies, frame 0x34). Diagnostic: without the `= &g_504430` restore 0x281b60's pair is
// in retail's order. All restored.
// M3 lane 4 r214 (fast with 0x281b60 and the five matched walk users): in function_288571 the inheritance read once
// into a local passed to the three passes (compute_orientations_inheritance_mapping's form), declared after or before
// the count, 417/417 (1749 bytes; 0x281b60 397, 0x285750 420, 0x28aaa0 165); the count declared, then assigned after
// the declarations 2; the node index and the cursor initialised (0, NULL) 2; in function_288572 the empty group's
// index step after the output advance 2 (0x288570 10, 0x289580 9). All restored.
// M3 lane 15 r106 (fast with 0x281b60 and the matched walk users; all 78 functions of the file for the shared-helper forms):
// blend 3's `real one = 1.0f;` declared at function_288573's top 2 (the dead literal store is dropped early); the vector
// and scale blends as `+=` statements 2 (76 others still match); the walk's node index and cursor as pointer parameters 2;
// quaternion_blend's parameters const 2; the interpolation's root scaling gated on `!in_place` 2 (0x281b60 62).
// Diagnostics: a used float literal in the blend-1 scale arm fixes this pair (one or two of them); a non-literal float
// multiply there does not; `g_5044bc += 3` there fixes this pair but reverses pass 1's (EDX flags, ECX count); 15 to 24
// dead initialised locals in this body 2 each. All restored.
// M3 lane 7 r190 (fast with 0x281b60 and the four matched walk users): g_504430 declared ahead of g_504410 2; pass 0
// given the count straight from the settings global, the local count only for passes 1-2, 286 (1742 bytes, every walk
// user regresses); the bit-flags pointer stepped in the outer for's increment 414 (1751 bytes); in function_288572 the
// bit-flags selection ahead of the output-cursor store 2 (0x288570 3, 0x289580 78); the blend's sign mask with external
// linkage 2. All restored.
// M3 lane 16 r146 (compile-only build of the file, exact here: this function 2, 0x281b60 2, 0x285750 0): only pass 0's
// pair is reversed (count, flags); pass 1's (EDX flags, ECX count) is already retail's. The flip from a float operation in
// the scale arm (`+ 1.0f`, `+ 2.5f`, `* 2.5f`, a negate; diagnostics) does not need a new constant (1.0f is already used
// earlier in the file), so it follows the float work in the function, not a new symbol. Same pair, no change: the scale
// blend's fraction by value 2; the vector blend's fraction by value 138 (pair unchanged); both 138; the interpolation's
// translation fraction read once into a local 2; interpolate_real's fraction by const reference 2; all three of its
// parameters by const reference 2. All restored.
// M3 lane 10 r132 (compile-only pair detector, exact here, then fast with 0x281b60 and the matched 0x285750/0x288570/
// 0x289580/0x28aaa0/0x27f530): in function_288572 the three frame-state saves declared ahead of the interpolation test and
// assigned inside it (compute_component_orientations' form) 2; that walk's `if (blend == 0) __assume(0);` line in the
// interpolation arm 2; both 2; in function_288573 the blend-1 weight through a `real const &` for the three arms 2; the
// blend-1 arms as a switch on the component 2 (all the same code; 0x281b60 2 throughout). The screen still flips pass 0
// with `+= 1.0f` in the blend-1 scale arm (diagnostic). All restored.
// M3 lane 2 r116 (fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0; control 2/2): the vector and
// scale blends' bodies through the existing interpolate_real 159 (1735 bytes; 0x288570 120); the walk's inheritance
// parameter by value instead of `const *const &` 399 (1745 bytes; 0x281b60 389, 0x285750 427); the interpolation's
// scale arm through interpolate_real instead of the scale blend 2 (0x285750 17); the map-index test bound-first
// (`index < count && index >= 0`) 16 (0x281b60 16, 0x285750 12); the root offset scale as `*=` 2 (same code). All restored.
// M3 lane 3 r278 (compile-only build of the file, exact here, then fast with 0x281b60 and the four matched walk users):
// function_288573 as a switch on the blend method 2 (0x288570 120, 0x289580 2); blend 1's vector arm through
// interpolate_real per component with its scale arm as `+=` 155 (1732 B); the output pointer declared ahead of the in-place
// flag 2 (0x288570 3, 0x289580 78); the in-place test's operands swapped, the bit test as `!= 0`, the shift as an
// assignment and the counters pre-incremented 2 each (same code); all 35 combinations of blend 1's vector and scale arms
// (helper, interpolate_real, `+=`) with blend 2's (`+=`, `*=`) keep the pair (30 of them move 0x288570).
// M3 lane 11 r136 (fast with 0x281b60 and the four matched walk users, all unchanged): function_288573 ending in `else
// { __assume(0); }` 2 (the method is a constant, so it folds); function_288571/2/3 `static` 2; the bit test as
// `(local_2 & 1) != 0` 2; function_288573's parameters const 2; the empty group ending in `continue;` with the other
// arm unnested 2 (same code each). Diagnostics (fast): `__assume(local_2 >= 0)` after the flags read 348 (1733 bytes,
// every walk user regresses: the node tests no longer fold); with node_flags_test returning its expression 2 (0x281b60
// 39, 0x288570 120, 0x289580 2), so a run-time assume flips the pair only together with the unfolded tests. All restored.
// M3 lane 1 r140 (compile-only build of the file, exact here: this function 2, 0x281b60 2, 0x285750 0): the interpolation's three
// frame-state saves as wrapper locals shared by the passes (by reference) 44 (0x281b60 42, 0x285750 46), the walk's output pointer
// shared that way 329 (0x281b60 318); the second decode with its blend in a forceinline helper 2, the frame-state restore in one 2,
// both 4 (0x281b60 4, 0x285750 2), the destination lookup with the mask test in one 2 (0x281b60 28); 150 random combinations of those
// two helpers with the apply index named, the destination index declared at the group's top, the bit test as `!= 0`, the counters as
// `+= 1`, the count declared first, the output and bit-flags selections as if/else and the saves declared ahead of the interpolation
// test: 2 (115 forms) or 4 (35), the pair never moves. Diagnostics: the settings read as separate globals (every field, or the frame
// trio, the blend-frame trio, the bit flags, the count, the inheritance, the mask or the decompressors alone) 2 each, so the pair does
// not follow how many global symbols the function references; a statement under a constant-false test of the blend method at each of
// 117 places in the walk and its helpers never moves the pair (it changes 0x285750's blend-3 registers at 89 of them); live probes at
// the wrapper's start or end flip it only for `g = h << 2` and `g = h + k` (stores, `+= 3`, float work, `/ 3`, `& 7` do not).
// M3 lane 3 r284 (h2m fast screen with 0x281b60 and the four matched walk users): in function_288572 `__assume(local_2 !=
// 0)` at the top of the non-empty group arm 348 (1733 bytes, every walk user regresses); `__assume(arg_7 > 0)` after the
// flags read 348; `__assume(local_3 <= arg_7)` after the group end 348; `__assume(arg_5 < arg_7)` ahead of the bit test 348
// (0x288570 362); the flags local declared `register long` 2 (same code). Every run-time assume in the walk unfolds the
// node tests, whatever it states. All restored.
// M3 lane 14 r137 (H2M_FAST with 0x281b60 and the five matched walk users): the pair sits in the counter block that joins
// the destination-miss exits and the interpolation path (the bit-clear path enters below it); pass 1's identical join (EDX
// flags, ECX count) reloads the flags first in both builds, so the order is not the register number. The wrapper's count
// read after the three counter resets 6 (0x281b60 6, the four other walk users 4: the spill store sinks, the pair stays);
// the inner group loop as a while with the index step at its end 2; the node lookup as a single-exit result 2 (both the
// same code everywhere); the destination-mask test as `mask ? node_flags_test(...) : true` 2 (0x281b60 39, 0x288570 120);
// function_288573's output pointer as `s_animation_output *const &` 365 (1742 bytes; 0x281b60 354, 0x285750 386). All restored.
// M3 lane 13 r133 (fast with 0x281b60 and the five matched walk users; control 2): in function_288572 the inner for's
// test as `local_3 > arg_5` 10 (every walk user 6-124), the outer for's test as `arg_7 > arg_5` 8 (only the three
// loop-end compares flip, the pair stays; every user 6); node_destination_get's map entry held in a `short` 382 (1753 B);
// the channel-counter step written in both arms of the destination test 248 (0x289580 78); the destination index
// initialised to the node index 2 (same code). All restored.
// M3 lane 12 r171 (H2M fast with 0x281b60 and the four matched walk users; control 2): the in-place flag computed once in
// function_288571 and passed to the three passes 4 (0x281b60 4, 0x28aaa0 2); the count as function_288572's first parameter
// 2 (same code); the kind-2 count clamp moved into function_288572's top for the translation pass, count by `long &` 2 (same
// code); the flags pointer step written in each arm of the empty-group test 72 (1731 B, every walk user regresses);
// node_destination_get's out-parameter as `long &` 2 (same code; 0x27f530 and 0x2898b0 keep). All restored.
// M3 lane 16 r168 (h2m fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0; control 2): in
// function_288572 the interpolation test's arms swapped (`if (!arg_3)` apply, else the frame-state path) 4 (0x281b60 4,
// 0x285750 184, 0x28aaa0 2); the channel-counter step as one increment of a selected global (`(c == 0 ? a : c == 1 ?
// b : d)++`) 2 (0x288570 120); node_destination_get's output index as a `long &` 2 (same code everywhere); the walk's
// count copied into a `long const` local for the loop bound and the clamp 2 (0x288570 3, 0x289580 78); in
// function_288571 passes 1 and 2 as a counted loop 450 (1776 B, every walk user regresses). All restored.
// M3 lane 15 r131 (h2m fast with 0x281b60 and the four matched walk users; control 2): component_interpolate as a switch
// on the component 2 (0x288570 120); quaternion_blend's fraction as its first parameter, definition and all three calls, 2
// (same code); the walk's output-cursor advances as `+= 1` 2 (same code); the interpolation's translation arm as `+=`
// statements with a `real const &` fraction 2 (same code); component_interpolate reading the blend fraction once into a
// value local for all three arms 2 (0x288570 120). All restored.
// M3 lane 5 r232 (h2m fast with 0x281b60 and the four matched walk users; control 2): quaternion_blend's fraction
// copied into a local that the asm reads 2; component_interpolate's rotation arm reading the blend fraction into a local
// first 2; in function_288572 the destination lookup's result named and read through a `bool const &` 2 (0x289580 78);
// that result in a plain named bool 2 (same code); the interpolation arm's apply destination through an
// `s_animation_output *const &` bound to the output pointer 365 (1742 B; 0x281b60 352, 0x285750 370). All restored.
// M3 lane 5 r234 (h2m fast with 0x281b60 and the four matched walk users; control 2): quaternion_blend taking the
// destination and source by reference 2; component_decompress as a switch on the component 2 (0x288570 120, 0x289580
// 78); orientation_vector_blend and orientation_scale_blend taking both outputs by reference 2; node_destination_get
// with failure early returns (`if (!bit) return false;`, `if (index < 0 || index >= count) return false;`) 2; the
// output pointer as `&g_sampling_settings.field_4c[index]` 2 (all but the switch the same code everywhere). All restored.
// M3 lane 15 r133 (one-object link-time screen of this file scoring all 60 samplers against retail at once; control 58
// of 60 equal, this function 2, 0x281b60 2). Retail has both reload orders among siblings of one source (0x285750 and
// these two flags first, 0x2898b0 count first), and the pair exists only with the destination mask (the map-flag bit
// test then takes the flags' EBX on the miss path; mask-less 0x27eae0 reloads only the count at the join). Forms, as
// this function / 0x281b60 / samplers equal: the samplers as instances of a function template over the four constants
// calling the pass helper 4 / 4 / 28 (method 3's twelve all 13; instances expand their helpers only without the
// generated unit's trailing inline pragmas), with the per-arm apply walk 2 / 2 / 28; the three counters as one array
// global indexed by the component 53 / 4 / 29 (`inc [mem]` for retail's load, inc, store); the decoders as an array
// member indexed by the component 2 / 2 / 33; the bit-flag pointers as an array member 2 / 2 / 46; the map-flag bit test
// in node_destination_get mask first, with a named mask, shift, word or word index, both named in either order, or the
// mask through a new one-line helper 2 / 2 / 58 each (same code); those spellings in node_flags_test 2 / 2 / 57; the
// method-2 translation add with its operands swapped 2 / 2 / 58 (same code). Each existing wrapper applied to all 60:
// compute_orientations 4 / 4 / 41, function_288571 2 / 2 / 35, the split form 22 / 31 / 25, the component-count form
// 4 / 7 / 29, the split-index-last form 29 / 32 / 8, the inheritance-mapping form 110 / 46 / 15. A generated walk over
// apply once or per arm, node index, flags, group end and destination index each walk-local, shared or per pass, count
// shared, separate for passes 1-2 or by reference, inheritance by reference or read in place, either node lookup and the
// mask test as function or macro (3,888 combinations, plus 233 with the bit-flag pointer or a wrapper inheritance local
// varied): best 2 / 2, best family 44 (mapping lookup with the macro mask test); no combination matches either function.
// Diagnostics: 0..47 dead initialised locals at the wrapper's top, the pass helper's top or in the sampler's own body
// never move the pair (they move method 3's float registers for 8..28).
// M3 lane 14 r147 (H2M_FAST with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0; control
// 2): node_destination_get storing the map entry into its out-parameter and testing that 2 (0x281b60 2, 0x28aaa0 20); the
// interpolation's frame-state saves interleaved with the sets (save, set, save, set, save, set) 94 (1731 B; 0x281b60 47,
// 0x285750 45, 0x288570 219); the group end's `arg_5 + 8` named and used twice in the ternary 2 (0x281b60 62, 0x288570 3,
// 0x289580 78); the counter chain's last arm as `else if (arg_4 == 2)` 2 (same code). All restored.
// M3 lane 15 r135 (h2m fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0; control 2): the
// walk's mask and interpolation flags as long parameters of function_288571 and function_288572 2 (0x281b60 2, 0x288570
// 120); the three counter resets in reverse order 2; the wrapper's count declared `register` 2; the in-place flag as long
// 2; the group end declared `long const` 2; the method-2 scale product with its operands swapped 2 (all the same code
// here and in 0x281b60). All restored.
// M3 lane 11 r160 (h2m fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0; control
// 2): in function_288572 the saved frame index as long 2 (same code); the 0x504410 target set ahead of the three
// frame-state sets 14 (0x285750 12, 0x288570 12); the 0x504430 target restored ahead of the three restores 13 (0x285750
// 11, 0x288570 98); the bit-flags word as dword 5 (four siblings 3); the group end capped by an if 208; the pass counters
// pre-incremented 2 (same code). Matched 0x285750 reloads flags first after the same quaternion_multiply asm, its arm
// doing integer work in EAX/ECX first. All restored.
// M3 lane 13 r151 (h2m fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0; control 2): in
// function_288572 the bit-flags read and the cursor step as two statements 2; the outer for as `arg_5 = 0; while
// (arg_5 < arg_7)` 2; the destination lookup and the mask test as two nested ifs 2 (the three the same code); the
// zero-flags arm and the group arm swapped (`if (local_2 != 0)`) 377 (0x281b60 352); the cursor stepped at the end of the outer
// iteration 414 (0x281b60 404) (every sibling lost). All restored.
// M3 lane 1 r163 (h2m fast with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0; control
// 2). Reading of the allocator: the two loads are block-entry reloads of the walk's count and of pass 0's bit flags at
// the counter block; both have the same scheduler rank there, so they are emitted in the order of their allocator
// candidate numbers, lower first. Those numbers are not source order: each time a pick takes a candidate's last free
// register the candidate is split and every new piece takes a recycled number (shared with the float candidates and
// with the constants 0, 1, 8, 31 and the two decode-target addresses, which are candidates too), so the final numbers
// are the end of a 15-round split cascade: count 24, pass 0's flags 52, pass 1's flags 12 (why pass 1 is already
// retail's); matched 0x285750 ends count 61, flags 27. A form that leaves the allocator's input unchanged cannot move
// them. Forms: node_flags_test's mask bit as `1u << (index & 31)` 2 (0x289580 lost; the same candidate numbers);
// node_destination_get's map-flag bit as `1u <<` 2; both 2 (0x289580 lost); the walk's bit test as `local_2 & 1u` 2;
// all three unsigned 2 (0x289580 lost). Diagnostic: 0x27e6b0 defined ahead of this function in the file 2 (the
// numbering does not carry over from the function compiled before it). All restored.
// M3 lane 8 r235 (H2M_FAST with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0;
// control 2). Scheduler reading: the pair is the end of the counter block (load, inc, store of the pass counter), where
// the allocator reloads the walk's count (ECX, [esp + 0x18]) and the shifted bit flags (EBX, [esp + 0x1c]) for the
// latch; neither has a consumer in that block, so both rank 35 and fall back to emission order (ECX first). A forced
// search finds three single decisions that give a byte match: the flags reload one height step higher, the count
// reload one lower, or the count reload not ready before cycle 2. So a source form needs the flags reload to gain a
// use inside the counter block (or the count reload to wait on one there). Forms in function_288572: the bit flags as
// `byte` 286 (0x281b60 280, every sibling lost), as `short` 341 (375), the empty-group arm ending in `continue` with
// the group arm unnested 2 (same code), the flags read by index `arg_6[arg_5 >> 3]` 444 (410), the empty-group arm's
// index step after the output step 2 (0x288570 10, 0x289580 9 lost). All restored.
// M3 lane 8 r236 (H2M_FAST with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0;
// control 2). The one untried idea, a use of the flags inside the counter block: in function_288572 the output step and
// the flags shift written in each arm of the bit test (so the shift sits behind the counter step until the tails are
// merged) 153 (0x281b60 199, 0x285750 75, 0x288570 101, 0x289580 78, 0x28aaa0 82); only the shift in each arm, the output
// step after the test, 153 (199, 75, 310, 270, 82). The loop is the same shape, but the flags then outrank the count
// for their register and every home moves, so the tail is not reached that way. All restored.
// M3 lane 4 r261 (fast screen with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0;
// control 2): in function_288572 the group end as `arg_5 + 8 < arg_7 ? arg_5 + 8 : arg_7` 53 (0x281b60 53, 0x285750
// 53, three siblings 9); the output target as an if/else 2; the bit-flag pointer by an if/else-if chain 2;
// function_288573's destination index as `arg_1 != 0 ? local_4 : arg_5` 2; the saved frame fraction read ahead of the
// two saved indices 2 (all four the same code everywhere). All restored.
// M3 lane 5 r247 (fast screen with 0x281b60 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0;
// control 2): node_destination_get's map-flag test through node_flags_test (defined ahead of it) 2 (0x289580 78);
// node_destination_get as one bool result set in each arm 2 (same code everywhere); both 2 (0x289580 78); the walk's
// bit test as `(local_2 & 1) != 0` 2; quaternion_blend's three parameters const 2 (same code). All restored.
// @retail 0x27dff0
void function_27dff0(void)
{
	function_288571(1, 1, true, true);
}

// @retail 0x27e6b0
void function_27e6b0(void)
{
	compute_orientations(1, 1, true, false);
}

// @retail 0x27eae0
void function_27eae0(void)
{
	compute_orientations(1, 1, false, true);
}

// @retail 0x27f160
void function_27f160(void)
{
	compute_orientations(1, 1, false, false);
}

// @retail 0x27f530
void function_27f530(void)
{
	compute_orientations(1, 2, true, true);
}

// @retail 0x27fc10
void function_27fc10(void)
{
	compute_orientations(1, 2, true, false);
}

// @retail 0x280050
void function_280050(void)
{
	compute_orientations(1, 2, false, true);
}

// @retail 0x2806e0
void function_2806e0(void)
{
	compute_orientations(1, 2, false, false);
}

// @retail 0x280ac0
void function_280ac0(void)
{
	compute_orientations_component_count(2, 0, true, true);
}

// @retail 0x281090
void function_281090(void)
{
	compute_orientations_component_count(2, 0, true, false);
}

// @retail 0x281370
void function_281370(void)
{
	compute_orientations_component_count(2, 0, false, true);
}

// @retail 0x2818e0
void function_2818e0(void)
{
	compute_orientations_component_count(2, 0, false, false);
}

// score 2 (was 2): r131, r232 and r234 unchanged. The two stack slots are read in the opposite order inside the inlined helper.
// r133 unchanged. The two stack slots are read in the opposite order inside the inlined helper.
// The same allocator tie as 0x27dff0. Tried: blend-2 `+=` and `*=` 2; the blend-2 translation as a
// loop over vector.n 364; the multiply's sign mask as long 2, as `1UL << 31` 2, as real lanes 245.
// Round 40 (screen): the existing compute_component_orientations_shared with each of node index, bit
// flags, destination index, flags and last shared or per pass (32 forms; the 0x2898b0 form matched
// that function) best 4 (flags and last per pass, with or without the destination index: the same tie in
// two places), the rest 22-83; on the two best, declarations by kind or by pass, count first or last 4.
// Round 100 (lane 7, --fast): the per-arm apply reading the blend weight into a local at its top 2,
// in the blend-1 arm 2; the blend-2 rotation through a pointer local to the destination rotation
// 352; the flags test as `!= 0` 2; the apply index in a named local 2. KIMI1 round 100: real zero
// lanes plus a sign store 256; a negative-zero scalar 308; an __m128 mask 235; a zero __m128 plus
// a sign store 290; split dword initialization 2 (0x289580 78); quaternion fields 256; ordinary
// inline multiply 480; the flag declaration split from its read 2.
// Round 194 (fast, shared helpers): in function_288572 the destination index declared at the inner
// body's top or with the last node, frame-state saves before the interpolation test, flags at function
// scope 2 each; the later passes' count as a ternary or clamped copy in function_288571 39, as
// separate translation and scale copies 2. All restored.
// Lane 13 r48 (fast, with 0x27dff0 2 and 0x285750/0x288570/0x289580/0x28aaa0 matched throughout): the code is
// retail's up to the two reloads after the multiply's asm block (retail flags then count, as 0x285750's blend-3
// pass gets). The destination index declared at the pass helper's top 2; the pass bound declared there 2; the node
// count declared first in the orchestration 2; the blend-2 arm tested before blend-1 in the apply 2; the flags
// declared then assigned at the loop top 2 (all the same code).
// Round 30 (lane 17): a compile-only build gives the same order (count reload, then flags); renaming the count local
// keeps it; any float literal left in the code anywhere in the function (`+ 1.0f` in the rotation, translation or scale
// arm) gives retail's order, while float global loads, an int constant, folded `* 1.0f` or an unused float local do not
// (retail has no such literal). Fast: the blend-2 rotation multiply through a named rotation pointer 352 (1685 bytes);
// named source vector 2; named scale 2; named destination vector 2; the existing vector3f_add for the translation 4.
// Round 84 (lane 16): a compile-only copy scored against retail gives this function's 2 exactly (0x285750 0). The pair also flips
// for unrelated statements in the scale arm: a float negate, a compare with 0.0f, a division by 3, a sum of two globals; not a float
// divide, an int-to-float conversion or an int store. So the literal is one trigger of a wider tie. Fast: the counter through
// `long *counter = &g_5044b4 + component` MATCH, but it offsets a global's address and costs 0x27dff0 387 (1752 bytes), 0x288570 120,
// 0x28aaa0 4 (restored); the counter as a `long &` parameter 2; the group end assigned then clamped 241 (1659 bytes); the inner `for` as
// `while` with the index step last 2; the frame state restored before the apply 65; the node lookup as a single-exit result 2. Compile-only: a real `long[3]` counter array 4
// (0x285750 21, 0x27dff0 355); the counters as one struct 2 (0x285750 17); decoders or bit flags as array members 2 (0x285750 12); counter,
// decoder or bit flags by const reference 2 (0x285750 6 with decoder or bit flags); the multiply's output parameter first 2 (0x285750 2);
// 105 blend-2 arm combinations (`+=`, swapped operands, named source, destination, scale) 2 at best; the flag test without a local, sign as
// long, int, const or struct 2.
// Round 143: real sign lanes with negative zero 245 (1685 bytes, all controls regressed); the sign mask renamed 2;
// prefix / += / assignment counter updates 2 each; register flags 2; register count 2; a const group bound 2.
// All restored; the count/flags reload pair and frame 0x34 remain.
// Lane 11 round 70 (fast): a ternary lvalue counter update 2; a counter pointer by ternary at the increment 2 or at the helper's top 2 (0x288570 120 in
// all three); the bit flags shifted by an explicit assignment 2; the bit test as `(flags & 1) == 1` 2 (0x288570 3, 0x289580 78). Here and in 0x2bdc70
// ours reloads the later-declared variable first, retail the earlier-declared one.
// Round 154 (m3 lane 5, fast with 0x27dff0, which has the same pair): in function_288571 the counter resets before the
// node-count read 6 (both); the resets in reverse order 2; in function_288572 the flags byte read and the cursor stepped
// as separate statements 2; the outer group loop as `while` 2; the later passes' clamp tested count-first 2 (both
// functions identical in each but the first).
// Round 141 (m3 lane 8, fast, the shared-helper trials listed above 0x27dff0): local static sign mask 2; one
// output array 333 (1681 bytes); vector-blend interpolation 2; BIT_VECTOR_TEST tests 2/2; bit flags ahead of a
// count copy 2. The multiply asm leaves EBX alone; the EBX reload follows the interpolation's blend asm.
// Lane 11 round 80 (fast, with 0x27dff0 2 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x2898b0): function_288573 as a
// switch on the method 2 (0x288570 120); in function_288572 the output cursor set to the scratch output and overridden by
// `if (in_place)` 2 (0x288570 3, 0x289580 78), the node admission as a bool refined by a second `if` 357 (1690 bytes), the
// destination index passed straight to the apply 2 (0x288570 3), the group end as `count < index + 8 ? count : index + 8` 8
// (all controls 6-8), the interpolation test as one if/else holding the root scale 4 (0x285750 184), the flags shift in the
// inner for's step 12; component_decompress as a switch 2 (0x288570 120, 0x289580 78). All restored.
// Round 89 (m3 lane 2, fast, the shared-helper trials listed above 0x27dff0): templates 2048; real-typed
// sign constant 2; cursor declared ahead of the in-place flag 2; short inheritance map entry 367; the
// existing compute_orientations 4; frame-state saves at the walk helper's top 2. All restored.
// Round 174 (m3 lane 8, the shared-helper trials listed above 0x27dff0): const sign mask local 2; plain
// inline apply 2; plain inline walk 1929 (out of line); flag test returning its expression 2; map test
// through the node flag test 2. All restored.
// Round 253 (m3 lane 3, the shared-walk trials listed above 0x27dff0): redundant group-loop guard 97;
// inner-loop guard 2; bit flags by `long &` 2; count by `long const &` at the clamp 39; pass counters as a
// switch 2. All restored.
// Round 132 (m3 lane 9, the wrapper trials listed above 0x27dff0): compute_orientations_inheritance_mapping 397
// (1682 bytes); compute_orientations_component_count 41; compute_orientations_split 72;
// compute_orientations_split_index_last 62; compute_orientations 4; the scale pass's count from the settings
// global (diagnostic) 152 (1680 bytes); the wrapper's count through a reference-bound local 39; the group end or
// the destination index bound to a `long &` 2/2; the walk's count parameter as `long &` 2. All restored.
// M3 lane 13 r106 (the shared-walk trials listed above 0x27dff0): counter step helper 2; empty-group arms swapped 352;
// chained counter resets 2; the node index zeroed in the wrapper 6; the frame-state restores reversed 14; constant-change
// diagnostics leave the pair reversed.
// M3 lane 5 r198 (the shared-helper trials listed above 0x27dff0): the counters as members past the settings global 2,
// only the increments or only the resets 4; the output pointer as such a member 2; the blend-2 rotation through a new
// inline wrapper 2; the blend-2 translation and scale through new inline helpers 2; without the blend-1, 3 or 4 arm of
// function_288573 2 (diagnostic). All restored.
// M3 lane 8 r183 (the shared-walk trials listed above 0x27dff0): count accessor at the clamp 2, at the loop bound 2;
// bit test through the accessor 2; group end in the inner for's init 2; frame state copied in reverse order 11. All restored.
// M3 lane 4 r214 (the shared-walk trials listed above 0x27dff0): inheritance local in function_288571 397 (1682
// bytes); count assigned after the declarations 2; node index and cursor initialised 2; empty group's step after
// the output advance 2 (0x288570 10, 0x289580 9). All restored.
// M3 lane 4 r216 (fast, this function alone): 1 to 14 dead initialised locals in the body 2 each (not a symbol-count
// tie); the wrapper written out as three function_288572 calls with one count 2, with translation and scale count copies
// 2, with per-pass counts read from the settings global 400 (1684 bytes), in three declaration orders each; the
// inheritance through a `s_graph_inheritance const *const &` local with one count 62 (the translation pass's fold
// order flips, the reload pair stays), with the count copies 2. All restored.
// M3 lane 3 r266 (fast with 0x27dff0, 0x285750, 0x288570, 0x289580, 0x28aaa0): function_288572 applying through the
// file's component_apply in place of its duplicate function_288573 2 (0x288570 3, 0x289580 78: the duplicate is part of
// the tie); the group end as `index + 8 < count ? index + 8 : count` 53 (all six 9-53); the saved frame index typed long
// as its field 2; the scale counter under `else if (component == 2)` 2; the output pointer chosen by if/else in place of
// the ternary 2 (all three the same code). All restored.
// M3 lane 4 r221 (fast with 0x27dff0 and the five matched walk users): in function_288572 the non-empty arm as
// `else if (local_2 != 0)` 2 (every user unchanged); the channel counters as a switch on the component with a
// `default: __assume(0)` 357 (1690 bytes, every walk user regresses); component_decompress as such a switch 352 (1686
// bytes); the interpolation's output restore written as the top's `local_0 ? local_1 : &g_504430` 2 (0x288570 383,
// 0x289580 78); the kind-2 clamp as `local_2 >= 2` 2 (same code); the group end as `arg_7 < arg_5 + 8 ? arg_7 :
// arg_5 + 8` 8 (every walk user 6-8). Diagnostic: `__assume(local_2 != 0)` at the top of the non-empty arm 350. In a
// compile-only build of the file any `__assume` in the walk (on the flags, on the count, or a switch default) puts
// the pair in retail's order, but it also stops node_flags_test's bool from folding into the branch (setne/test),
// and with node_flags_test returning its expression directly the pair stays reversed. Compile-only screens (exact
// here, with 0x27dff0 and the matched walk users): all 216 orders of the interpolation's three saves, three swaps and
// three restores best 2 (two save orders give the same code, every other order moves 0x285750 or 0x288570);
// root_offset_scale's three conditions in all six orders, its product as `*=` or with the operands swapped 2 (same
// code or only a branch order). All restored.
// M3 lane 4 r226 (fast with 0x27dff0 and the five matched walk users): the pair is in the rotation pass, the one pass
// with no C-level float arithmetic for blend methods 1 and 2 (both asm), where method 3's matched 0x285750 has literal
// float code. interpolate_real's fraction as `real const &` 2; the blend weight passed to function_288573 as a parameter
// 2 (0x27dff0 147, 0x285750 157); quaternion_blend's fraction copied into a local the asm reads 2; the saved frame
// fraction `real const` 2; component_interpolate's fraction read once into a named local 2 (0x288570 120). All restored.
// M3 lane 8 r197 (fast with 0x27dff0 and the matched walk users; a compile-only screen of this file is exact here, 2):
// the blend asm split into two blocks at the fraction load 2; the multiply asm split after its first operand load 2;
// the interpolation's blend fraction named before the blend call 2; the blend asm's sign constant as a const SSE
// vector initialised `{ -0.0f, 0, 0, 0 }` 2 (0x285750 33), non-const 2 (screen); the blend asm loading the source
// before the destination 4 (screen). Diagnostics (screen): 1 to 8 dead locals at the top of function_288573,
// of function_288572 or of the wrapper move every symbol number by up to 400 and never flip the pair (pass 0 count
// first, pass 1 flags first throughout), so it is not a symbol-number tie; any float constant in emitted code
// flips pass 0 (`* 2.0f` on the blend-2 scale 1, one extra instruction; 1, 2 or 3 distinct constants alike), a
// double negation 6, while an extra product with a float global, an int store to a new global or a folded
// `* 1.0f`, `+ 0.0f`, `- 0.0f`, `/ 1.0f` do not. With both blend-3 arms written as blend 2's, 0x285750 gets the
// pair reversed too; either of its arms alone keeps retail's order. All restored.
// M3 lane 12 r155 (a one-object compile of this file gives the real build's pairs: count first in this function and
// 0x27dff0, flags first in 0x285750; linking it alone, with the ballast or with the graphics library objects changes
// nothing): the output pointer's restore ahead of the three frame-state restores 13 (0x27dff0 13, 0x285750 11,
// 0x288570 98); the bit flags as `dword` 5 (every walk user 3-5); screen only, the pair unchanged in all three: the
// output pointer set ahead of the three frame-state sets, the bit flags as `unsigned long`, the saved frame index as
// long (same code). All restored.
// M3 lane 4 r231 (screened with 0x27dff0 and the four matched walk users): in function_288572 the outer loop bounded by a
// `long const` copy of the count 95 (0x288570 120, 0x289580 2); the group end declared at the walk's top 2 (same code); the
// destination-mask test as an if nested under the lookup 2 (same code); the interpolation test inverted with the plain apply
// first 4 (0x27dff0 4, 0x285750 184, 0x28aaa0 2); in function_288571 pass 0 given the settings' node count directly, the local
// for the later passes 397 (1681 bytes, every walk user 217-433). All restored.
// M3 lane 13 r129 (screened with 0x27dff0 and the five matched walk users): the saved frame fraction declared with a
// `0.0f` initialiser ahead of the interpolation test (diagnostic: the dead literal store is dropped before allocation) 2;
// the walk's flags declared `byte` 280 (every walk user 213-304); node_flags_test, node_destination_get,
// component_decompress, component_interpolate or root_offset_scale as plain `inline` 2 each (same code). All restored.
// M3 lane 7 r198 (fast with 0x27dff0, 0x285750, 0x288570; a compile-only build of the file is exact here, 2): in
// function_288572 the bit-flag shift ahead of the cursor step 2 (same code here; 0x288570 12); node_destination_get as one
// exit through a found flag 2 (same code everywhere, though every symbol number moves: pass 0's flags then number below
// the count and the count is still reloaded first, so the pair does not follow symbol numbers); in function_288571 the
// counter resets ahead of the count read 6 (the count's store moves; 0x285750 4, 0x288570 4); the destination-mask test
// as `arg_2 ? node_flags_test(...) : true` 39 (the pass-2 scale product changes; 0x288570 120). Diagnostics (screen, not
// kept): `__assume(node_count >= 0)` in this wrapper or on the count in function_288571 moves 8 lines, pair unchanged;
// 0 to 15 dead initialised locals in this wrapper shift the walk's symbols by 3 each, pair unchanged. All restored.
// M3 lane 1 r142 (compile-only screen of the whole file, exact here as rounds 84 and 197 found; nothing reached retail's
// order, so nothing went on to check.py): the wrapper's definition moved to six other places in the file (first wrapper,
// ahead of 0x27dff0, 0x27e6b0, 0x281370, 0x2825b0, 0x285400, end of file) 2 each (same code: the pair does not depend on
// what is compiled ahead of it). On the apply-in-each-arm walk (function_288572): node index, bit-flag pointer, flags,
// group end and destination index each as a reference to a function_288571 local or as a local of the walk, all 32
// masks, with one count / a clamped copy for passes 1 and 2 / separate translation and scale copies (96 forms): 34 are
// the present code, the rest move 2-10 lines elsewhere, and the pair is count-then-flags in every one, 2 at best. The
// channel counter incremented through a dword view 2 (same code); the blend-2 scale with a folded `* 1.0f` 2 (same code),
// under an `if (arg_0 == 2)` whose dead else arm holds a float literal 2 (same code). Diagnostics (reverted at once): a
// float literal operand in pass 1 or 2 flips the pair with no other change than its own instruction, the same instruction
// with a float global in the literal's place does not; one to three reads of new int globals do not, an int sum of two
// loads (even of one global) does; a load, multiply and store of one new global does not; the compiler's temporary
// numbers do not correlate (0 to +3 on both sides). Retail's pair (EBX then ECX) and pass 1's (EDX then ECX) are both in
// descending register order; ours in pass 0 is the one ascending pair.
// M3 lane 6 r217 (fast with 0x27dff0 and the four matched walk users; control 2): the pair is pass 0's count (ECX) and bit
// flags (EBX) reloaded inside the counter increment that every failed destination test joins; pass 1 reloads flags then
// count as retail. Blend 2's rotation through a named rotation pointer used twice 352 (1685 B); the non-interpolated root
// scale moved into the interpolation test's else arm 2 (same code); blend 2's translation through the existing vector3f_add
// 4; a separate applied-destination local beside the kind-0 cursor 2 (0x289580 2); the kind-2 clamp as an unconditional
// ternary self-assignment 2 (same code). All restored.
// M3 lane 2 r126 (fast with 0x27dff0 and the four matched walk users; control 2): redundant parens around the blend-2
// translation arm's first sum in function_288573, one pair 7 (the pair stays; that arm's addss operand orders move),
// two pairs 2 (0x289580 78), three pairs 39 (0x289580 78): the arm's float keys move, and parens in an arm a user never
// runs still move that user, so node keys are counted before dead arms go; around the blend-2 scale product one, two or
// three pairs 2 (0x289580 78 with two or three); around the walk's group-end sum, around both its sums, around
// node_flags_test's word index 2 each (same code). All restored.
// M3 lane 5 r225 (h2m fast with 0x27dff0 2 and the five matched walk users; control 2): commutative operand orders not
// listed above, each the same code (2, 0x27dff0 2, controls matched): interpolate_real as `a + (b - a) * fraction` and as
// `fraction * (b - a) + a`; orientation_vector_blend and orientation_scale_blend written base first; the destination
// pointer as `&((s_animation_output *)field_4c)[local_4]`; node_destination_get's map bit test and node_flags_test with
// the mask first. Diagnostic (restored at once): the blend asm's source pointer in ECX instead of EBX 393 (1652 B, the
// aligned frame moves). Reading: the pair sits in the pass-0 join block 0x281dc3 (counter load, the two reloads, the
// increment); pass 1's join orders flags then count in both builds, so only pass 0's order is ours.
// M3 lane 13 r138 (h2m fast with 0x27dff0 and the four matched walk users; control 2): the apply's node index argument
// always the destination index (the lookup sets it to the node index for kind 0) 2 (0x288570 3); component_decompress
// choosing the decompressor by a ternary, as the walk chooses its bit flags, and calling it once 2 (0x289580 78);
// node_destination_get returning the destination index or NONE, callers testing `!= NONE`, 374 (1699 B, the NONE compares
// stay); the destination pointer cast after the sum 2 (same code); the walk reading the settings' inheritance itself in
// place of the by-reference parameter 2 (same code). All restored.
// M3 lane 11 r150 (h2m fast with 0x27dff0 and the four matched walk users; control 2): node_destination_get's map bit
// test through the existing node_flags_test 2 (0x289580 78); node_destination_get with the kind test inverted (the
// mapped arm under `node_kind != 0`, the kind-0 result last) 2; its out parameter as `long &` at all three walks 2; in
// function_288572 the bit-flags selection ahead of the output selection 2 (0x288570 3, 0x289580 78); root_offset_scale
// given the walk's inheritance as a first parameter at all eight calls 2 (same code). All restored.
// M3 lane 15 r131 (h2m fast with 0x27dff0 and the four matched walk users; control 2): in function_288572 the output
// cursor selected from the settings field directly with the output pointer local declared after it 2 (0x288570 120); the
// three frame-state saves declared const 2 (same code); the first-frame decode written through a `const &` to the
// settings' decoder table in place of component_decompress 39 (0x289580 2, 0x288570 120); the pass's bit-flags pointer
// selected in function_288571 ahead of each pass instead of in the walk 2 (same code); function_288573's apply index
// ahead of its output parameter (definition and both calls) 2 (same code). All restored.
// M3 lane 5 r232 (the shared-walk trials listed above 0x27dff0): the blend fraction local in quaternion_blend 2; the
// interpolation's rotation fraction local 2; the lookup result through a `bool const &` 2; the lookup result in a plain
// named bool 2; the apply destination through an `s_animation_output *const &` 352 (1685 B). All restored.
// M3 lane 5 r234 (measured with 0x27dff0, the same walk forms; control 2): the blend's destination and source by
// reference 2; the decompressor choice as a switch 2; the vector and scale blends' outputs by reference 2; the node
// lookup with failure early returns 2; the output pointer as a typed element address 2. All restored.
// M3 lane 15 r133 (one-object link-time screen scoring all 60 samplers; the forms and counts are listed above
// 0x27dff0): template instances 4 (per-arm apply 2); the counters as one array global 4; the decoders or the bit-flag
// pointers as array members 2 / 2; bit-test spellings in either helper 2; each existing wrapper 4, 2, 31, 7, 32 and 46
// (compute_orientations, function_288571, split, component count, split index last, inheritance mapping); the 3,888
// generated walk combinations best 2. Nothing below 2.
// M3 lane 14 r147 (measured with 0x27dff0, the walk forms listed above it; control 2): the out-parameter-first node lookup
// 2; the interleaved frame-state saves 47; the named group-end sum 62; the explicit last counter arm 2; the kind-2 clamp in
// function_288571 as `if (arg_1 == 2) local_2 = MIN(local_2, 1);` 2 (same code). All restored.
// M3 lane 15 r135 (the shared-walk trials listed above 0x27dff0, control 2): mask and interpolation flags as long
// parameters 2; counter resets reversed 2; `register` count 2; in-place flag as long 2; group end const 2; the method-2
// scale product's operands swapped 2. All restored.
// M3 lane 11 r160 (h2m fast with 0x27dff0 and the matched 0x285750/0x288570/0x289580/0x28aaa0/0x27f530/0x2898b0; control
// 2): in function_288572 the saved frame index as long 2 (same code); the 0x504410 target set ahead of the three
// frame-state sets 14 (0x285750 12, 0x288570 12); the 0x504430 target restored ahead of the three restores 13 (0x285750
// 11, 0x288570 98); the bit-flags word as dword 5 (four siblings 3); the group end capped by an if 241; the pass counters
// pre-incremented 2 (same code). Matched 0x285750 reloads flags first after the same quaternion_multiply asm, its arm
// doing integer work in EAX/ECX first. All restored.
// M3 lane 13 r151 (h2m fast with 0x27dff0 and the matched 0x285750/0x288570/0x289580/0x28aaa0; control 2): in
// function_288572 the bit-flags read and the cursor step as two statements 2; the outer for as `arg_5 = 0; while
// (arg_5 < arg_7)` 2; the destination lookup and the mask test as two nested ifs 2 (the three the same code); the
// zero-flags arm and the group arm swapped (`if (local_2 != 0)`) 352 (0x27dff0 377); the cursor stepped at the end of the outer
// iteration 404 (0x27dff0 414) (every sibling lost). All restored.
// M3 lane 1 r163 (h2m fast with 0x27dff0 and the matched walk users; control 2): the same reading as 0x27dff0's r163
// note (the pair is ordered by recycled allocator candidate numbers at the end of the split cascade). The mask bits as
// `1u <<` in node_flags_test, in node_destination_get or in both 2 each; the walk's bit test as `local_2 & 1u` 2; all
// three 2; 0x27e6b0 defined ahead of 0x27dff0 in the file 2. All restored.
// M3 lane 8 r235: the forms and the scheduler reading above 0x27dff0's marker (same pass helper): flags as `byte` 280,
// as `short` 375, the early `continue` 2, the indexed flags read 410, the empty-group index step moved 2. All restored.
// M3 lane 8 r236 (H2M_FAST with 0x27dff0 and the six matched walk users; control 2): in function_288572 the output step
// and the flags shift written in each arm of the bit test 199 (0x27dff0 153, 0x285750 75, 0x288570 101, 0x289580 78,
// 0x28aaa0 82); only the shift in each arm 199 (153, 75, 310, 270, 82). All restored.
// M3 lane 1 r169 (h2m fast with 0x27dff0 and the four matched walk users; control 2): a forced scheduling search
// matches with either of the two reloads' heights moved by one (equal heights, so the code-generation order of the
// split pieces decides, as round 163 read it). Respellings of function_288573 arms this function never runs (the
// numbering is taken before dead arms go): blend 4's scale arm with no locals 2 (0x288570 3, 0x289580 2); its
// `*=` form 2; blend 4's translation arm through `g_5044c0` with no local 2 (0x288570 3, 0x289580 78); blend 3's
// scale as `*=` 2; blend 3's translation as `+=` 2 (all controls matched in the last three). All restored.
// @retail 0x281b60
void function_281b60(void)
{
	function_288571(2, 1, true, true);
}

// @retail 0x2821f0
void function_2821f0(void)
{
	compute_orientations_split(2, 1, true, false);
}

// @retail 0x2825b0
void function_2825b0(void)
{
	compute_orientations(2, 1, false, true);
}

// @retail 0x282be0
void function_282be0(void)
{
	compute_orientations(2, 1, false, false);
}

// @retail 0x282f60
void function_282f60(void)
{
	compute_orientations(2, 2, true, true);
}

// @retail 0x283600
void function_283600(void)
{
	compute_orientations_split_index_last(2, 2, true, false);
}

/* compute_orientations(2, 2, false, true) with the destination index, the flags
   and the last node of each group of eight kept per component pass, and the
   translation and scale passes' node counts (only the root's) in their own
   variables */
// @retail 0x2839d0
void function_2839d0(void)
{
	long node_index;
	byte const *bit_flags;
	long rotation_destination_index;
	long translation_destination_index;
	long scale_destination_index;
	long rotation_flags;
	long translation_flags;
	long scale_flags;
	long rotation_last;
	long translation_last;
	long scale_last;
	long node_count = g_sampling_settings.node_count;
	long translation_count = node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations_shared(2, 2, false, true, 0, node_count, node_index, bit_flags, rotation_destination_index, rotation_flags, rotation_last);
	if (translation_count > 1)
	{
		translation_count = 1;
	}
	long scale_count = translation_count;
	compute_component_orientations_shared(2, 2, false, true, 1, translation_count, node_index, bit_flags, translation_destination_index, translation_flags, translation_last);
	compute_component_orientations_shared(2, 2, false, true, 2, scale_count, node_index, bit_flags, scale_destination_index, scale_flags, scale_last);
}

// @retail 0x284010
void function_284010(void)
{
	compute_orientations_split(2, 2, false, false);
}

// @retail 0x2843a0
void function_2843a0(void)
{
	compute_orientations_component_count(3, 0, true, true);
}

// @retail 0x284a20
void function_284a20(void)
{
	compute_orientations(3, 0, true, false);
}

// @retail 0x284de0
void function_284de0(void)
{
	compute_orientations(3, 0, false, true);
}

// @retail 0x285400
void function_285400(void)
{
	compute_orientations(3, 0, false, false);
}

// @retail 0x285750
void function_285750(void)
{
	function_288571(3, 1, true, true);
}

// @retail 0x285e90
void function_285e90(void)
{
	compute_orientations(3, 1, true, false);
}

// @retail 0x286320
void function_286320(void)
{
	compute_orientations(3, 1, false, true);
}

// @retail 0x286a00
void function_286a00(void)
{
	compute_orientations(3, 1, false, false);
}

// @retail 0x286e40
void function_286e40(void)
{
	compute_orientations(3, 2, true, true);
}

// @retail 0x287590
void function_287590(void)
{
	compute_orientations(3, 2, true, false);
}

// @retail 0x287a30
void function_287a30(void)
{
	compute_orientations(3, 2, false, true);
}

// @retail 0x288120
void function_288120(void)
{
	compute_orientations(3, 2, false, false);
}

// @retail 0x288570
void function_288570(void)
{
	function_288571(4, 0, true, true);
}

// @retail 0x288bd0
void function_288bd0(void)
{
	compute_orientations(4, 0, true, false);
}

// @retail 0x288f80
void function_288f80(void)
{
	compute_orientations(4, 0, false, true);
}

// @retail 0x289580
void function_289580(void)
{
	function_288571(4, 0, false, false);
}

/* compute_orientations(4, 1, true, true) with the destination index, the flags
   and the last node of each group of eight kept per component pass */
// @retail 0x2898b0
void function_2898b0(void)
{
	long node_index;
	byte const *bit_flags;
	long rotation_destination_index;
	long translation_destination_index;
	long scale_destination_index;
	long rotation_flags;
	long translation_flags;
	long scale_flags;
	long rotation_last;
	long translation_last;
	long scale_last;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations_shared(4, 1, true, true, 0, node_count, node_index, bit_flags, rotation_destination_index, rotation_flags, rotation_last);
	compute_component_orientations_shared(4, 1, true, true, 1, node_count, node_index, bit_flags, translation_destination_index, translation_flags, translation_last);
	compute_component_orientations_shared(4, 1, true, true, 2, node_count, node_index, bit_flags, scale_destination_index, scale_flags, scale_last);
}

// @retail 0x289fa0
void function_289fa0(void)
{
	compute_orientations(4, 1, true, false);
}

// @retail 0x28a3e0
void function_28a3e0(void)
{
	compute_orientations(4, 1, false, true);
}

// @retail 0x28aaa0
void function_28aaa0(void)
{
	function_288571(4, 1, false, false);
}

// @retail 0x28ae70
void function_28ae70(void)
{
	compute_orientations(4, 2, true, true);
}

// @retail 0x28b570
void function_28b570(void)
{
	compute_orientations(4, 2, true, false);
}

// @retail 0x28b9c0
void function_28b9c0(void)
{
	compute_orientations(4, 2, false, true);
}

// @retail 0x28c090
void function_28c090(void)
{
	compute_orientations(4, 2, false, false);
}


__forceinline bool node_destination_mapping_get(s_graph_inheritance const *inheritance, long node_kind, long node_index, long *destination_index)
{
	if (node_kind == 0)
	{
		*destination_index = node_index;
		return true;
	}
	dword const *flags = (dword const *)inheritance->node_map_flags;
	short const *mapping = (short const *)inheritance->node_map;
	if (flags[node_index >> 5] & (1 << (node_index & 31)))
	{
		long index = mapping[node_index];

		if (index >= 0 && index < g_sampling_settings.destination_node_count)
		{
			*destination_index = index;
			return true;
		}
	}
	return false;
}

__forceinline void compute_component_orientations_mapping(long blend_method, long node_kind, bool destination_mask, bool interpolate, long component, byte const *&bit_flags, long &node_index, long node_count, s_graph_inheritance const *const &inheritance)
{
	bool in_place = blend_method == 0 || blend_method == 4;
	s_animation_output *destination = (s_animation_output *)g_sampling_settings.field_4c;

	g_5044c0 = in_place ? destination : &g_504430;
	bit_flags = component == 0 ? g_sampling_settings.rotation_bit_flags : (component == 1 ? g_sampling_settings.translation_bit_flags : g_sampling_settings.scale_bit_flags);
	for (node_index = 0; node_index < node_count; )
	{
		long flags = *bit_flags++;

		if (flags == 0)
		{
			node_index += 8;
			if (node_kind == 0)
			{
				if (in_place)
				{
					g_5044c0 += 8;
				}
				else
				{
					destination += 8;
				}
			}
		}
		else
		{
			long last = node_index + 8 > node_count ? node_count : node_index + 8;

			for (; node_index < last; node_index++)
			{
				if (flags & 1)
				{
					long destination_index;

					/* the destination mask tested in place: through node_flags_test, VC keeps
					   the test's result in a register when the sampler ends in __assume */
					if (node_destination_mapping_get(inheritance, node_kind, node_index, &destination_index) &&
						(!destination_mask || BIT_VECTOR_TEST(g_sampling_settings.destination_node_mask, destination_index)))
					{
						if (node_kind != 0)
						{
							destination = (s_animation_output *)g_sampling_settings.field_4c + destination_index;
							if (in_place)
							{
								g_5044c0 = destination;
							}
						}
						component_decompress(component);
						if (!interpolate && !in_place)
						{
							root_offset_scale(node_kind, component, node_index);
						}
						if (interpolate)
						{
							dword frame_index = g_sampling_settings.frame_index;
							long frame_index2 = g_sampling_settings.next_frame_index;
							real frame_fraction = g_sampling_settings.frame_fraction;

							g_sampling_settings.frame_index = g_sampling_settings.blend_frame_index;
							g_sampling_settings.next_frame_index = g_sampling_settings.blend_next_frame_index;
							g_sampling_settings.frame_fraction = g_sampling_settings.blend_frame_fraction;
							g_5044c0 = &g_504410;
							component_decompress(component);
							component_interpolate(component);
							if (blend_method != 4)
							{
								root_offset_scale(node_kind, component, node_index);
							}
							if (blend_method == 0)
							{
								__assume(0);
							}
							component_apply(blend_method, component, destination, node_kind == 0 ? node_index : destination_index);
							g_sampling_settings.frame_index = frame_index;
							g_sampling_settings.next_frame_index = frame_index2;
							g_sampling_settings.frame_fraction = frame_fraction;
							g_5044c0 = &g_504430;
						}
						else
						{
							component_apply(blend_method, component, destination, node_kind == 0 ? node_index : destination_index);
						}
					}
					if (component == 0)
					{
						g_5044b4++;
					}
					else if (component == 1)
					{
						g_5044b8++;
					}
					else
					{
						g_5044bc++;
					}
				}
				if (node_kind == 0)
				{
					if (in_place)
					{
						g_5044c0++;
					}
					else
					{
						destination++;
					}
				}
				flags >>= 1;
			}
		}
	}
}

__forceinline void compute_orientations_inheritance_mapping(long blend_method, long node_kind, bool destination_mask, bool interpolate)
{
	s_graph_inheritance const *inheritance = g_sampling_settings.inheritance;
	long node_index;
	byte const *bit_flags;
	long node_count = g_sampling_settings.node_count;

	g_5044b4 = 0;
	g_5044b8 = 0;
	g_5044bc = 0;
	compute_component_orientations_mapping(blend_method, node_kind, destination_mask, interpolate, 0, bit_flags, node_index, node_count, inheritance);
	if (node_kind == 2 && node_count > 1)
	{
		node_count = 1;
	}
	compute_component_orientations_mapping(blend_method, node_kind, destination_mask, interpolate, 1, bit_flags, node_index, node_count, inheritance);
	compute_component_orientations_mapping(blend_method, node_kind, destination_mask, interpolate, 2, bit_flags, node_index, node_count, inheritance);
}
