// @flags /O2 /Gr
/* BITSET.CPP: integer base-2 logarithms and bit vectors (arrays of
   dwords holding bit_count bits, the lowest bit first) */

#include "unknown_11c920.h"

/* the index of value's highest set bit (0 for 0) */
// @retail 0x13e3a0
long log2_floor(dword value)
{
	long result = 0;
	if (value > 0)
	{
		dword copy = value;
		while (copy != 1)
		{
			copy >>= 1;
			result++;
		}
	}
	return result;
}

/* the bits needed to store an index below value, at least 1: value's
   base-2 logarithm rounded up */
// @retail 0x13e3c0
long log2_ceiling_plus_one(dword value)
{
	long result = 0;
	if (value > 1)
	{
		value--;
		while (value != 1)
		{
			value >>= 1;
			result++;
		}
	}
	return result + 1;
}

/* destination = a | b, over bit_count bits */
// @retail 0x13e3e0
void bitset_or(const dword *a, const dword *b, dword *destination, long bit_count)
{
	for (long i = ((bit_count + 31) >> 5) - 1; i >= 0; i--)
		destination[i] = b[i] | a[i];
}

/* the mask of the bits of the last dword that a vector of bit_count bits uses */
static dword bit_mask_for_count(long bit_count)
{
	long remainder = bit_count & 31;
	dword mask = 0xffffffff;
	if (remainder > 0)
		mask >>= 32 - remainder;
	return mask;
}

static dword popcount32(dword v)
{
	v = ((v >> 1) & 0x55555555) + (v & 0x55555555);
	v = ((v >> 2) & 0x33333333) + (v & 0x33333333);
	v = ((v >> 4) & 0x0f0f0f0f) + (v & 0x0f0f0f0f);
	v = ((v >> 8) & 0x00ff00ff) + (v & 0x00ff00ff);
	return (v >> 16) + (v & 0xffff);
}

/* the number of set bits */
// @retail 0x13e420
long bit_vector_count_bits(const dword *bits, long bit_count)
{
	long word_count = (bit_count + 31) >> 5;
	long total = 0;
	for (long i = 0; i < word_count - 1; i++)
		total += popcount32(bits[i]);
	return total + popcount32(bits[word_count - 1] & bit_mask_for_count(bit_count));
}

static long highest_set_bit(dword value)
{
	long result = -1;
	if (value)
	{
		__asm { bsr eax, value }
		__asm { mov result, eax }
	}
	return result;
}

/* the index of the highest set bit, or -1 */
// @retail 0x13e530
long bit_vector_highest_set_bit(const dword *bits, long bit_count)
{
	long word_count = (bit_count + 31) >> 5;
	long result = highest_set_bit(bits[word_count - 1] & bit_mask_for_count(bit_count));
	if (result >= 0)
		result += word_count * 32 - 32;
	if (result == -1)
	{
		long i = word_count - 2;
		do
		{
			if (i < 0)
				break;
			result = highest_set_bit(bits[i]);
			if (result >= 0)
				result += i * 32;
			i--;
		}
		while (result == -1);
	}
	return result;
}
