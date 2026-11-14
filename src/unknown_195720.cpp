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

// score 2 (was 2): the masked-byte and clear-start LEAs keep the reversed data/index operand order.
// Lane 16 round 74: a data local assigned before the index gives [index + data] (assigned after it,
// [data + index]; its declaration position does not matter), but retail reads the field in each block.
// Real: data local at the top of the discard block 40; explicit sums with the division inline, in
// either operand order 2/2; the masked byte by inline division with the clear from the index 2; data
// local declared first and assigned in each block 35.
// Reversed additions 2; grouped clear start 2; data plus one then index 2; shared clear start 24;
// count before mask 47; count declared first 2; split index declaration 2; index plus one then data 2;
// element address plus one 2; const index and count 2; size local 47.
// discard entry guard 2; nested writing gates 2; unchanged writer guard 2; accounting store order 13;
// positive clear count 21; inclusive remaining count 2; named byte mask 2; native signed bit offset 2.
// Index expression at each use 2; bytes through the dword buffer member 2; data pointer local 35;
// plain masked assignment 2; size-first bound comparison 4.
// `saved / 8` at the two address uses or at every use 2/2; a `byte *const &` reference to the data
// field before/after the index 2/2; the masked byte as an explicit read-modify-write 2.
// Round 37: the byte index declared at the function's top, after `saved`, at the discard block's top, at the
// mode block's top with the count 2/2/2/2; an int index 2; the index computed before the counter store 17.
// Round 97: masked pointer dereference 2; clear-start index first alone 2; two-use buffer pointer 40;
// current-byte pointer 46; two-use next-byte index 24; grouped remaining count 2;
// buffer member only at the mask or only at the clear 2/2.
// Round 70 (--fast): index and count declared at function scope 2; a named bit index 2; the mask named
// in its block 2; the clear from `&data[index + 1]` 2; a named size for the count 2. Compile-only screens
// (same code as the build here): no spelling, cast, integer address sum, scope, temp count or deleted
// neighbour statement flips either lea; small probes give [index + data] only when the address is the
// index's sole use (0x1957d0's shape) and [data + index] whenever the index is used again.
// Round 87 (m3 lane 12, fast): 1, 2, 3, 5 or 8 unused initialised locals at the top, 1, 2 or 4 before the
// index: 2 each (diagnostic: not a numbering tie); `byte_index[stream->data]` 2; `saved & 7` 42 (not the same
// value for negative positions); clear start and count from a parenthesised `byte_index + 1` 2; the index
// computed before the discard counter stores 17; `count >= 0 && count > 0` 2.
// Round 139: data loaded then the index reassigned in the mask block 23 (141 bytes), in the clear block 2;
// a distinct clear index after the data load 2; count formed by subtract then decrement, or size minus one
// then subtract 2/2; clear start derived from size minus count 45 (142); index reread from the checkpoint 60;
// saved declaration split from its assignment 2. Original body restored.
// Round 72 (m3 lane 10, fast): the index copied in the subscript by an embedded assignment 2; block-local data, copied index and a pointer to
// the byte 9; block-local data, own division, direct subscript 35 (its memory operand is [index + data], not a LEA); inline mask helper 35;
// inline address helper with the index first 2. One-file harness (exact here; diagnostics): identity pointer chains of 1 to 8 locals, at the top
// or used for both reads 2 each; one or two new stored fields 3/4; extra arithmetic at the top, in the block or after it 33/16/6, always data first.
// Round 77 (m3 lane 1, fast): the count tests and memset as a forceinline clear helper 2; a data local assigned
// before the index, used for the masked byte only 39 (that access becomes index-first `[edx + edi]`); single-use
// `saved / 8` index expressions and a single-use start index 2 (identical); the early data local used for the clear
// only 40 (the clear LEA becomes retail's `[edx + edi + 1]`); the data local read at the function's top for both 52.
// The newer value goes first, so retail orders its in-block field loads as values older than the index.
// Round 173 (m3 lane 4): compile-only screen of 22 forms (size-first compare, data + index pointer sum, `saved / 8` at the
// mask or the clear, a named next index, a named bit first, counter stores after the index, a size local, the count before
// the mask, buffer member or char casts, data locals at the function, discard-block, mode-block or mask-block top): no
// spelling flips the masked-byte LEA; only a data local read before the index flips the clear LEA (and keeps the data in
// a register). Real (fast): size-first compare 4; char-typed masked byte 2; data local at the mode block's top 49; data
// local in the mask block 35; counter stores after the index 17.
// Round 142 (m3 lane 8, fast): the pop as a separate decrement then the read 2; the read from `count - 1` with the decrement
// after 62; a named popped checkpoint index 2; the mask without the byte cast 2; the subscript through `(dword)byte_index` 2.
// Compile-only, both LEAs data-first: embedded index assignment in the bound test, a byte mask local in a block,
// `*(byte_index + stream->data)`, `saved % 8` named before the index.
// Round 162 (m3 lane 5, fast; all the same code, 2): a local view of the stream with a plain data pointer, with data and size
// as a nested first member, as a base type, or the data as a one-element array (diagnostics); the index cast to a pointer and
// the data to an integer at both addresses (diagnostic); forceinline address helpers taking (data, index), (stream, index) or
// (index, data); the address as an integer sum from the dword buffer member.
// Round 95 (m3 lane 10, fast): the whole discard block as a force-inlined helper taking (stream, bit) 2; the byte
// index from a force-inlined bit-to-byte helper at each of its four uses 2; `if (!discard) return;` then the flat body
// 2; a named bit-position local for the compare and the subtraction 4; the masked byte as `mask & byte` 2. A 600 s
// permuter run on the body (76 tries) found none below 2.
// Round 76 (m3 lane 2, fast): the clear from `saved / 8` with the count from the index 2; a second named index in the mask block
// 2; the mask indexed by a copy of saved 2; the mask index as `(long)(saved / 8)` 2; the mask as `(byte)~(-1 << (saved % 8))` 4.
// Compile-only screen (exact here): every spelling whose index value numbers to the named index (`saved / 8` at each use, in an
// inline, through a copy, with `+ 0`, through a second local, in the join block) keeps both LEAs data-first; only an index that
// stays a separate computation (`saved >> 3`, `(saved & ~7) / 8`, a reread checkpoint) ranks first, and it is then recomputed.
// Round 192 (m3 lane 4, fast): the mode and position tests as nested ifs 2; the mask as `(byte)~(0xff << (saved % 8))` 4;
// `unknown30 += 1` with the clear under one `count >= 0 && count != 0` 3; the count as `size_in_bytes - 1 - byte_index` 2;
// the pop as a decrement statement then the read 2. Compile-only screens (diagnostic): 0 to 40 unused locals at the top and
// 1 to 8 dead inline-helper locals at the top, the mode block, before the bound test or before the count: both LEAs stay
// data-first.
// Round 119 (m3 lane 9, fast): the index bound to a `long const &` 2; `int saved` 2; `register` on the index 2; a
// `s_bitstream *const` parameter 2; the index incremented after the mask with count and clear from it, as a statement
// or as `size - ++byte_index`, 11 each (first difference +0x15: `inc edx` then `add edi, edx`, no clear LEA).
// @retail 0x194710
void function_194710(s_bitstream *stream, bool discard)
{
	long saved = stream->checkpoints[--stream->checkpoint_count];
	if (discard)
	{
		if (stream->mode == 1 && saved < stream->bit_position)
		{
			stream->unknown30++;
			stream->unknown2c = stream->bit_position - saved;
			long byte_index = saved / 8;
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
	if ((real)fabs(g_4687a8->i * v->i + g_4687a8->j * v->j + g_4687a8->k * v->k) < (real)fabs(g_4687ac->i * v->i + g_4687ac->j * v->j + g_4687ac->k * v->k))
		vector_basis_cross(v, g_4687a8, a);
	else
		vector_basis_cross(g_4687ac, v, a);
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
		if (value >= (1 << bits) - 1)
			return hi;
		lo = (real)exp(event_dequantize_real(value, (1 << bits) - 1, (real)log(lo), (real)log(hi)));
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
bool function_195560(vector3f const *up_a, vector3f const *a, vector3f const *up_b, vector3f const *b)
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
