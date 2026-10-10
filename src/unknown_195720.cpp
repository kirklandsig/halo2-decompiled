/* UNKNOWN_195720.CPP: the bit stream module, the packet encoding every
   network message codec uses: bit reads and writes, buffers, checkpoints,
   and the direction, logarithmic real and wide string codecs built on them.
   Retail never inlines these, hence /Ob1; the codecs that inline the small
   helpers are in unknown_1946f0.cpp */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_1946f0.h"
#include <math.h>
#include <string.h>

// @flags /O2 /Ob1 /Gr /arch:SSE


// @retail 0x1946f0
bool function_1946f0(s_bitstream *stream)
{
	bool result = stream->bit_position > (stream->size_in_bytes << 3);
	if (stream->error)
		result = true;
	return result;
}

// @retail 0x194710
void function_194710(s_bitstream *stream, bool discard)
{
	long saved = stream->checkpoints[--stream->checkpoint_count];
	if (discard)
	{
		if (stream->mode == 1 && saved < stream->bit_position)
		{
			stream->unknown2c = stream->bit_position - saved;
			long byte_index = saved / 8;
			stream->unknown30++;
			if (byte_index < stream->size_in_bytes)
				stream->data[byte_index] &= (byte)((1 << (saved % 8)) - 1);
			long count = stream->size_in_bytes - byte_index - 1;
			if (count >= 0)
			{
				if (count > 0)
					memset(stream->data + byte_index + 1, 0, count);
			}
		}
		stream->bit_position = saved;
	}
}

// @retail 0x1947a0
void function_1947a0(s_bitstream *stream)
{
	stream->mode = 3;
	stream->bit_position = 0;
	stream->checkpoint_count = 0;
	stream->error = false;
	if (function_1959c0(stream, 32) == k_debug_marker)
	{
		stream->error = true;
		return;
	}
	stream->bit_position = 0;
	stream->error = false;
}

// @retail 0x1947e0
void function_1947e0(s_bitstream *stream, dword value, long bits)
{
	if (bits < 32 && value >= (dword)(1 << bits))
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", value, 1 << bits);
	}
	function_195720(stream, value, bits);
}

PRIVATE __forceinline void vector_basis_cross(vector3f const *v, vector3f const *axis, vector3f *out)
{
	real k = axis->j * v->i - v->j * axis->i;
	real j = v->k * axis->i - v->i * axis->k;
	real i = v->j * axis->k - axis->j * v->k;
	out->i = i;
	out->j = j;
	out->k = k;
}

// @retail 0x194870
real function_194870(vector3f const *v, vector3f *a, vector3f *b)
{
	vector3f const *r0 = g_4687a8;
	vector3f const *r1 = g_4687ac;
	real d0 = (real)fabs(r0->i * v->i + r0->j * v->j + r0->k * v->k);
	real d1 = (real)fabs(r1->i * v->i + r1->j * v->j + r1->k * v->k);
	if (d0 < d1)
		vector_basis_cross(v, r0, a);
	else
		vector_basis_cross(r1, v, a);
	function_30bf0(a);
	vector_basis_cross(v, a, b);
	return function_30bf0(b);
}

// @retail 0x1949b0
real function_1949b0(vector3f const *v, vector3f const *w)
{
	vector3f a, b;
	function_194870(v, &a, &b);
	return (real)atan2(b.i * w->i + b.k * w->k + b.j * w->j, a.i * w->i + a.j * w->j + a.k * w->k);
}

// @retail 0x194a10
real function_194a10(vector3f const *axis, real angle, vector3f *out)
{
	vector3f a, b;
	function_194870(axis, &a, &b);
	real s = (real)sin(angle);
	*out = a;
	real c = (real)cos(angle);
	real k = (axis->i * out->i + axis->k * out->k + axis->j * out->j) * (1.f - c);
	real cross_j = out->k * axis->i - axis->k * out->i;
	real cross_k = axis->j * out->i - out->j * axis->i;
	real cross_i = axis->k * out->j - out->k * axis->j;
	out->i = axis->i * k + out->i * c - cross_i * s;
	out->j = out->j * c + axis->j * k - cross_j * s;
	out->k = out->k * c + axis->k * k - cross_k * s;
	return function_30bf0(out);
}

// @retail 0x194b60
void function_194b60(s_bitstream *stream, real value, real lo, real hi, long bits)
{
	long n = (1 << bits) - 1;
	real log_lo = (real)log(lo);
	real a = (real)log(value) - log_lo;
	real b = (real)log(hi) - log_lo;
	real r = a / (b / (real)n);
	long q;
	__asm
	{
		fld r
		fistp q
	}
	function_195720(stream, q, bits);
}

// @retail 0x194bc0
void function_194bc0(s_bitstream *stream, vector3f const *direction)
{
	long index = function_24f590(direction);
	if ((dword)index >= k_direction_limit)
	{
		char message[256];
		message[0] = 0;
		csprintf_256(message, "%u exceeds max value of %u", index, k_direction_limit);
	}
	function_195720(stream, index, k_direction_bits);
}

// @retail 0x194fa0
void function_194fa0(s_bitstream *stream, word *buffer, long count)
{
	long i;
	for (i = 0; i < count; i++)
	{
		buffer[i] = (word)function_1959c0(stream, 16);
		if (buffer[i] == 0)
			break;
	}
	if (i >= count)
	{
		buffer[count - 1] = 0;
		stream->error = true;
	}
}

// @retail 0x194ff0
real function_194ff0(s_bitstream *stream, real lo, real hi, long bits)
{
	long value = (long)function_1959c0(stream, bits);
	if (value != 0)
	{
		long n = (1 << bits) - 1;
		if (value >= n)
			return hi;
		lo = (real)exp(((real)(n - value) * (real)log(lo) + (real)value * (real)log(hi)) / (real)n);
	}
	return lo;
}

// @retail 0x195240
void function_195240(s_bitstream *stream, vector3f *up, vector3f *forward)
{
	if (function_1957d0(stream))
		*forward = *g_4687b0;
	else
		function_24f6b0(function_1959c0(stream, k_direction_bits), forward);
	long value = (long)function_1959c0(stream, 8);
	real angle;
	if (value == 0)
		angle = -k_pi;
	else if (value < 254)
		angle = ((real)value * k_pi - (real)(254 - value) * k_pi) * (1.f / 254.f);
	else
		angle = k_pi;
	function_194a10(forward, angle, up);
}

// @retail 0x1952f0
bool function_1952f0(real a1, real a2, real a3, real a4, long bits)
{
	bool result = false;
	if (a1 >= 0.f && a2 >= 0.f)
	{
		long n = (1 << bits) - 1;
		if (fabs(log(a2) - log(a1)) < (log(a4) - (real)log(a3)) / (real)n)
			result = true;
	}
	return result;
}

// @retail 0x195560
bool function_195560(vector3f const *b, vector3f const *a, vector3f const *up_a, vector3f const *up_b)
{
	bool result = false;
	if (a->quantized_equal(b))
	{
		real angle_a = function_1949b0(a, up_a);
		real angle_b = function_1949b0(a, up_b);
		real difference = (real)fabs(angle_b - angle_a);
		if (difference < 0.024736950173974037f)
			result = true;
		else
			result = fabs(difference - k_two_pi) < 0.024736950173974037f;
	}
	return result;
}

// @retail 0x1955d0
void function_1955d0(s_bitstream *stream, void const *source, long bits)
{
	long position = stream->bit_position;
	long remaining = stream->size_in_bytes * 8 - position;
	long n;
	if (remaining > bits)
		n = bits;
	else
		n = remaining;
	if (n > 0)
	{
		if (position % 8 == 0)
		{
			memcpy(stream->data + (position >> 3), source, (n + 7) >> 3);
			if (((stream->bit_position + n) & 7) != 0)
				stream->data[(stream->bit_position + n) >> 3] &= (byte)(0xff >> (8 - ((stream->bit_position + n) & 7)));
		}
		else
		{
			long shift = position % 32;
			dword *end = stream->buffer + ((position + n - 1) >> 5) + 1;
			dword *destination = stream->buffer + (position >> 5);
			dword const *s = (dword const *)source;
			*destination++ |= *s << shift;
			if (destination < end)
			{
				do
				{
					*destination = *s >> (32 - shift);
					s++;
					*destination++ |= *s << shift;
				}
				while (destination < end);
			}
			if (((stream->bit_position + n) & 0x1f) != 0)
			{
				dword *last = stream->buffer + ((stream->bit_position + n) >> 5);
				*last &= 0xffffffff >> (32 - ((stream->bit_position + n) & 0x1f));
			}
		}
	}
	stream->bit_position += bits;
}

// @retail 0x195720
void function_195720(s_bitstream *stream, dword value, long count)
{
	long remaining = (stream->size_in_bytes << 3) - stream->bit_position;
	long n;
	if (remaining > count)
		n = count;
	else
		n = remaining;
	if (0 < n)
	{
		value &= 0xffffffff >> (32 - n);
		long first;
		first = stream->bit_position >> 5;
		long last = (stream->bit_position + n - 1) >> 5;
		long offset = stream->bit_position & 0x1f;
		value &= 0xffffffff >> (32 - n);
		if (first == last)
			stream->buffer[first] |= value << offset;
		else
		{
			stream->buffer[first] |= value << offset;
			stream->buffer[first + 1] = value >> (32 - offset);
		}
	}
	stream->bit_position += count;
}

// @retail 0x1957d0
bool function_1957d0(s_bitstream *stream)
{
	long position = stream->bit_position;
	bool bit = false;
	if (position <= (stream->size_in_bytes << 3))
		bit = (stream->data[position / 8] & (1 << (position % 8))) != 0;
	stream->bit_position = position + 1;
	return bit;
}

// @retail 0x195820
void function_195820(s_bitstream *stream, void *destination, long bits)
{
	long remaining = stream->size_in_bytes * 8 - stream->bit_position;
	long n = remaining > bits ? bits : remaining;
	if (n > 0)
	{
		long position = stream->bit_position;
		if (position % 8 == 0)
		{
			byte const *source = stream->data + (position >> 3);
			memcpy(destination, source, (n + 7) >> 3);
			long tail = n & 7;
			if (tail != 0)
				((byte *)destination)[n >> 3] = ((byte *)destination)[n >> 3] & (byte)(0xff >> (8 - tail));
		}
		else
		{
			long shift = position % 32;
			dword const *s = stream->buffer + (position >> 5);
			dword *end = (dword *)destination + ((n - 1) >> 5) + 1;
			dword *d = (dword *)destination;
			long tail = n & 0x1f;
			dword saved = 0;
			if (tail > 0)
			{
				dword bytes = (tail + 7) >> 3;
				if (bytes < 4)
					saved = (0xffffffff << (bytes * 8)) & d[n >> 5];
			}
			while (d < end)
			{
				*d = *s >> shift;
				s++;
				*d++ |= *s << (32 - shift);
			}
			if (tail != 0)
			{
				dword *last = (dword *)destination + (n >> 5);
				*last = (*last & (0xffffffff >> (32 - tail))) | saved;
			}
		}
	}
	stream->bit_position += bits;
}

// @retail 0x1959c0
dword function_1959c0(s_bitstream *stream, long count)
{
	long position = stream->bit_position;
	long remaining = (stream->size_in_bytes << 3) - position;
	dword result = 0;
	long n;
	if (remaining > count)
		n = count;
	else
		n = remaining;
	if (n > 0)
	{
		long first = position >> 5;
		long last = (position + n - 1) >> 5;
		long offset = position & 0x1f;
		if (first == last)
			result = stream->buffer[first] >> offset;
		else
			result = (stream->buffer[first + 1] << (32 - offset)) | (stream->buffer[first] >> offset);
		result &= 0xffffffff >> (32 - n);
	}
	stream->bit_position = position + count;
	return result;
}
