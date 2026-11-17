#ifndef SWEEP_QUANTIZE_2E1090_H
#define SWEEP_QUANTIZE_2E1090_H
// Havok (third-party physics library): the three-axis sweep's shared inline
// pieces (axis_sweep_2e1090.h): quantizing a corner or a box to 16-bit words,
// clearing a bit array in 16-byte blocks, flipping and clearing a box's bit,
// and the y/z overlap test of two boxes. (src/unknown_2e18e0.cpp,
// src/unknown_2e1020.cpp, src/unknown_2e24b0.cpp, src/unknown_2e3370.cpp,
// src/unknown_2e3d40.cpp, src/unknown_2e0350.cpp, src/unknown_2e50d0.cpp)
#include <xmmintrin.h>
#include "axis_sweep_2e1090.h"

// a corner quantized to four 16-bit words
union corner_words_2e1090
{
	unsigned short words[4];
	unsigned __int64 whole;
};

// a box quantized: its lower words (the low bit cleared) and its upper words
// (the low bit set), widened to 32 bits
struct __declspec(align(16)) quantized_box_2e1090
{
	unsigned long low[4];
	unsigned long high[4];
};

// a corner moved by `offset`, scaled, clamped between `floor` and `ceiling`
// and quantized: the float's integer part, above the bias, in 16-bit words
__forceinline void quantize_corner_2e1090(__m128 const &corner, __m128 const &offset, __m128 const &scale,
	__m128 const &floor, __m128 const &ceiling, corner_words_2e1090 &out)
{
	__m128 const &bias = *(__m128 const *)g_4125c0;
	union
	{
		__m128 vector;
		unsigned long lanes[4];
	} quantized;
	__m128 value = _mm_add_ps(corner, offset);
	value = _mm_mul_ps(value, scale);
	value = _mm_min_ps(value, ceiling);
	value = _mm_max_ps(value, floor);
	quantized.vector = _mm_add_ps(value, bias);
	out.words[0] = (short)(quantized.lanes[0] >> 7);
	out.words[1] = (short)(quantized.lanes[1] >> 7);
	out.words[2] = (short)(quantized.lanes[2] >> 7);
	out.words[3] = (short)(quantized.lanes[3] >> 7);
}

// both corners of a box, the lower one rounded down to an even word and the
// upper one up to an odd word
__forceinline void quantize_box_2e1090(class_2e1090 const *sweep, bounds_2e0020 const &box, quantized_box_2e1090 &out)
{
	corner_words_2e1090 lower;
	corner_words_2e1090 upper;
	quantize_corner_2e1090(box.lower, sweep->lower_offset, sweep->scale,
		*(__m128 const *)g_4125e0, *(__m128 const *)g_4125d0, lower);
	quantize_corner_2e1090(box.upper, sweep->upper_offset, sweep->scale,
		*(__m128 const *)g_4125e0, *(__m128 const *)g_4125d0, upper);
	out.low[0] = lower.words[0] & 0xfffe;
	out.low[1] = lower.words[1] & 0xfffe;
	out.low[2] = lower.words[2] & 0xfffe;
	out.high[0] = upper.words[0] | 1;
	out.high[1] = upper.words[1] | 1;
	out.high[2] = upper.words[2] | 1;
}

// zeroes `bytes` bytes (and up to 16 more) in 16-byte blocks
__forceinline void clear_blocks_2e1090(void *block, long bytes)
{
	long blocks = bytes >> 4;
	__m128 const zero = _mm_setzero_ps();
	__m128 *cursor = (__m128 *)block;
	while (blocks >= 0)
	{
		_mm_store_ps((float *)cursor++, zero);
		blocks--;
	}
}

// flips box `id`'s bit
__forceinline void flip_bit_2e1090(long *bits, int id)
{
	int const word = id >> 5;
	int const bit = 1 << (id & 31);
	bits[word] ^= bit;
}

// clears box `id`'s bit
__forceinline void drop_bit_2e1090(long *bits, int id)
{
	int const word = id >> 5;
	int const bit = 1 << (id & 31);
	bits[word] &= ~bit;
}

// non-zero when the boxes are apart on y or z: either difference of a
// box's upper words and the other's lower words goes negative
__forceinline unsigned long apart_yz_2e1090(element_2e1090 const &box, element_2e1090 const &other)
{
	unsigned long const box_lower = ((unsigned long const *)box.slots)[0];
	unsigned long box_upper = ((unsigned long const *)box.slots)[1];
	unsigned long const other_lower = ((unsigned long const *)other.slots)[0];
	unsigned long other_upper = ((unsigned long const *)other.slots)[1];
	box_upper -= other_lower;
	other_upper -= box_lower;
	other_upper |= box_upper;
	return other_upper & 0x80008000;
}

#endif
