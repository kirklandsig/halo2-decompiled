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

// score 2 (was 2): the two stack slots are read in the opposite order inside the inlined helper.
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

// score 2 (was 2): the two stack slots are read in the opposite order inside the inlined helper.
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
