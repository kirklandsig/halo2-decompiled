// @flags /O2 /GL-
// Havok (third-party physics library): boxes held as 16-bit integer corners,
// each word moved one step for each of two limits it passes.
#include <mmintrin.h>

// a box's corner words: lower x, lower y, upper x, upper y, then lower z,
// upper z and two unused words, read as two 64-bit halves
struct __declspec(align(16)) quantized_box_2dfcc0
{
	unsigned short words[8];

	__m64 &xy() { return *(__m64 *)&words[0]; }
	__m64 &z() { return *(__m64 *)&words[4]; }
};

#include "quantized_boxes_2dfcc0.h"

// each corner word gets one added for each limit it passes: the limit less
// one and the limit less two
// @retail 0x2dfcc0
void quantized_boxes_2dfcc0::function_2dfcc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits)
{
	quantized_box_2dfcc0 lower, upper;
	__declspec(align(16)) __m64 mask;
	lower.words[5] = lower.words[4] = limits[4] - 1;
	lower.words[2] = lower.words[0] = limits[0] - 1;
	lower.words[3] = lower.words[1] = limits[1] - 1;
	upper.words[5] = upper.words[4] = limits[5] - 2;
	unsigned short upper_x;
	upper_x = limits[2] - 2;
	upper.words[0] = upper_x;
	upper.words[1] = limits[3] - 2;
	upper.words[3] = limits[3] - 2;
	upper.words[2] = upper_x;
	quantized_box_2dfcc0 *end = (quantized_box_2dfcc0 *)boxes + count;
	quantized_box_2dfcc0 *input = boxes;
	quantized_box_2dfcc0 *output;
	output = boxes;
	for (; !(input >= end); ++input, ++output)
	{
		mask.m64_u16[1] = mask.m64_u16[0] = 0xffff;
		__m64 z = _m_pcmpgtw(input->z(), lower.z());
		mask.m64_u16[3] = mask.m64_u16[2] = 0;
		z = _m_paddw(z, _m_pcmpgtw(input->z(), upper.z()));
		z = _m_pand(z, mask);
		__m64 xy = _m_paddw(_m_pcmpgtw(input->xy(), lower.xy()), _m_pcmpgtw(input->xy(), upper.xy()));
		output->xy() = _m_psubw(input->xy(), xy);
		output->z() = _m_psubw(input->z(), z);
	}
	_m_empty();
}

// each corner word gets one taken off for each limit it passes: the limit
// for the lower corner, and the one for the upper
// @retail 0x2dfdc0
void quantized_boxes_2dfcc0::function_2dfdc0(quantized_box_2dfcc0 *boxes, long count, unsigned short const *limits)
{
	quantized_box_2dfcc0 lower, upper;
	__declspec(align(16)) __m64 mask;
	lower.words[5] = lower.words[4] = limits[4];
	lower.words[2] = lower.words[0] = limits[0];
	lower.words[3] = lower.words[1] = limits[1];
	upper.words[5] = upper.words[4] = limits[5];
	unsigned short upper_x;
	upper_x = limits[2];
	upper.words[0] = upper_x;
	lower.z().m64_i32[1] = -1;
	upper.words[1] = limits[3];
	upper.words[3] = limits[3];
	upper.words[2] = upper_x;
	upper.z().m64_i32[1] = -1;
	quantized_box_2dfcc0 *end = (quantized_box_2dfcc0 *)boxes + count;
	quantized_box_2dfcc0 *input = boxes;
	quantized_box_2dfcc0 *output;
	output = boxes;
	for (; !(input >= end); ++input, ++output)
	{
		mask.m64_u16[1] = mask.m64_u16[0] = 0xffff;
		__m64 z = _m_pcmpgtw(input->z(), lower.z());
		mask.m64_u16[3] = mask.m64_u16[2] = 0;
		z = _m_paddw(z, _m_pcmpgtw(input->z(), upper.z()));
		z = _m_pand(z, mask);
		__m64 xy = _m_paddw(_m_pcmpgtw(input->xy(), lower.xy()), _m_pcmpgtw(input->xy(), upper.xy()));
		output->xy() = _m_paddw(input->xy(), xy);
		output->z() = _m_paddw(input->z(), z);
	}
	_m_empty();
}
