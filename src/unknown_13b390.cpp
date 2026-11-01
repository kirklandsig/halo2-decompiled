#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <xmmintrin.h>

// @flags /O2 /arch:SSE /Gr

/* UNKNOWN_13B390.CPP: tag function definitions. A function is a tag data
   block of bytes: a type, flags (bit 0: ranged, the high nibble: the output
   kind), the transition or periodic function types of the low and high
   ranges, the output bounds, then the type's parameters, once per range. */

struct s_tag_data
{
	long size;
	byte *address;
};

enum
{
	_function_identity = 0,
	_function_constant,
	_function_transition,
	_function_periodic,
	_function_linear,
	_function_linear_key,
	_function_multi_linear_key,
	_function_spline,
	_function_multi_spline,
	_function_exponent,
	_function_spline2,
	k_number_of_function_types
};

struct s_function_header
{
	byte type;
	byte flags;
	byte function_type[2];
	union
	{
		real_bounds bounds;
		dword colors[4];
	};
};

/* a spline's parameters per range: its control points and the cubic's
   coefficients */
struct s_function_spline
{
	s_function_header header;
	point2f points[4];
	real coefficients[4];
};

/* the number of parameters of each function type, per range */
byte const g_44178c[k_number_of_function_types] = { 0, 1, 2, 4, 6, 20, 32, 12, 4, 3, 12 };

real function_17c900(short function_type, real input);
real function_17ca10(real x, short curve);

/* sqrt(a) / sqrt(b), approximately: the square root of a from its bits (built in a's own
   storage), times rsqrtss of b */
static __forceinline real square_root_ratio_approximate(real a, real b)
{
	*(long *)&a = (*(long *)&a >> 1) + 0x1fc00000;
	real reciprocal;

	__asm
	{
		rsqrtss xmm0, b
		movss reciprocal, xmm0
		fld reciprocal
		fmul a
	}
}

struct vector2f
{
	real i, j;
};

static inline real length_sq2f(vector2f const *v)
{
	return v->i * v->i + v->j * v->j;
}

/* a squared plus b squared, from two coordinate differences */
static inline real length_squared_components(real a, real b)
{
	return a * a + b * b;
}

static inline real interpolate_linear(real a, real b, real t)
{
	return (1.0f - t) * a + b * t;
}

// @retail 0x13bec0
point2f *function_13bec0(s_tag_data const *function, byte range_index, byte point_index)
{
	byte type = ((s_function_header *)function->address)->type;
	if (type >= _function_linear && (type <= _function_multi_spline || type == _function_spline2))
	{
		s_function_header *header = (s_function_header *)function->address;
		return (point2f *)((real *)(header + 1) + g_44178c[header->type] * range_index + point_index * 2);
	}
	return NULL;
}

// score 11 (was 11): the remaining differences are in square_root_ratio_approximate, in both
// spline2 ranges (7 instructions in the low range, 4 in the high): retail spills the numerator into
// the slot rsqrtss then reuses for its operand and stores the shifted root before that operand;
// ours uses other slots and emits the two stores in the opposite order.
// Tried: the shifted root in its own long/real local (any name or declaration order) 12; a
// square-root helper and an rsqrtss helper multiplied in C 12, plus the high range's differences
// x first like the low range 8 (retail's slots and store order, but the multiply is deferred past
// the input load where retail multiplies in the asm), the same through a pow helper 8; an rsqrtss
// helper that multiplies by a root parameter 12 (the root copy takes the numerator's slot), its
// parameters swapped 12, the root as a long parameter 16; the root by an out parameter 12-16; the
// ratio helper's parameters swapped 12-16; the root as `long bits` read back through a real
// local that the asm multiplies by 12, with the x-first high range 16.
// Round 52: a square-root helper with its own result local (as 0x2b9750's) and an rsqrtss asm
// helper that only stores the reciprocal, multiplied in C, give retail's slots, stores and
// `fld; fmul`, but pow then loads the input first (no store and reload of the product) 201
// (1944 bytes); the same with the rsqrtss helper returning by `fld` 12; b copied into an
// asm-visible local (retail's store order, the root takes the numerator's slot) 12; an rsqrtss
// helper that multiplies by the estimate passed in 16; the root through a long pointer out
// parameter 12. Screened: names and declaration orders of the root, the copy and the reciprocal
// never move the slots; any root the asm reads takes the numerator's slot.
// Round 62: union root result 221 (1972 bytes); root bits read from the numerator 15; reciprocal declared first 15;
// denominator after the root 8 (retail's store order and denominator home; low root/reciprocal homes exchanged and
// the high result home differs), rejected as only three points without a match; implicit x87 return 12; separate
// result local 46; parameter result 225 (1984); denominator before the root 15. Opening helper restored.
// @retail 0x13b390
real function_13b390(void const *function, real input, real range)
{
	s_tag_data const *field_4_7 = (s_tag_data const *)function;
	real result = 0.0f;

	if (field_4_7->address && field_4_7->size > 0)
	{
		s_function_header const *header = (s_function_header const *)field_4_7->address;
		real const *parameters = (real const *)(header + 1);

		switch (header->type)
		{
		case _function_identity:
			result = input;
			break;
		case _function_constant:
			if (header->flags & 1)
			{
				result = range;
			}
			else
			{
				result = 0.0f;
			}
			break;
		case _function_transition:
		{
			real x = function_17ca10(input, header->function_type[0]);
			result = (parameters[1] - parameters[0]) * x + parameters[0];
			if (header->flags & 1)
			{
				parameters += 2;
				x = function_17ca10(input, header->function_type[1]);
				real high = (parameters[1] - parameters[0]) * x + parameters[0];
				result = interpolate_linear(result, high, range);
			}
			break;
		}
		case _function_exponent:
		{
			real exponent = parameters[2];
			if (fabs(exponent) < 0.0001f || (0.0f > exponent && fabs(input) < 0.0001f))
			{
				result = 1.0f;
			}
			else
			{
				result = (real)((parameters[1] - parameters[0]) * pow(input, exponent) + parameters[0]);
			}
			if (header->flags & 1)
			{
				parameters += 3;
				real high;
				exponent = parameters[2];
				if (fabs(exponent) < 0.0001f || (0.0f > exponent && fabs(input) < 0.0001f))
				{
					high = 1.0f;
				}
				else
				{
					high = (real)((parameters[1] - parameters[0]) * pow(input, exponent) + parameters[0]);
				}
				result = interpolate_linear(result, high, range);
			}
			break;
		}
		case _function_periodic:
		{
			real x = function_17c900(header->function_type[0], parameters[0] * input + parameters[1]);
			result = (parameters[3] - parameters[2]) * x + parameters[2];
			if (header->flags & 1)
			{
				parameters += 4;
				x = function_17c900(header->function_type[1], parameters[0] * input + parameters[1]);
				real high = (parameters[3] - parameters[2]) * x + parameters[2];
				result = interpolate_linear(result, high, range);
			}
			break;
		}
		case _function_linear:
			result = parameters[4] * input + parameters[5];
			if (header->flags & 1)
			{
				parameters += 6;
				result = interpolate_linear(result, parameters[4] * input + parameters[5], range);
			}
			break;
		case _function_linear_key:
		{
			real value;
			value = (input - parameters[9]) * parameters[13];
			real t0 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
			value = (input - parameters[10]) * parameters[14];
			real t1 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
			value = (input - parameters[11]) * parameters[15];
			real t2 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
			result = parameters[19] * t2 + parameters[18] * t1 + parameters[17] * t0 + parameters[16];
			if (header->flags & 1)
			{
				parameters += 20;
				value = (input - parameters[9]) * parameters[13];
				t0 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
				value = (input - parameters[10]) * parameters[14];
				t1 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
				value = (input - parameters[11]) * parameters[15];
				t2 = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
				real high = parameters[19] * t2 + parameters[18] * t1 + parameters[17] * t0 + parameters[16];
				result = interpolate_linear(result, high, range);
			}
			break;
		}
		case _function_spline:
		case _function_spline2:
		{
			real t = input;
			if (header->type == _function_spline2)
			{
				point2f const *p0 = function_13bec0(field_4_7, 0, 0);
				point2f const *p1 = function_13bec0(field_4_7, 0, 1);
				point2f const *p2 = function_13bec0(field_4_7, 0, 2);
				point2f const *p3 = function_13bec0(field_4_7, 0, 3);
				real end_length_squared = length_squared_components(p2->x - p3->x, p2->y - p3->y);
				if (end_length_squared > 0.0001f)
				{
					real start_length_squared = length_squared_components(p0->x - p1->x, p0->y - p1->y);
					if (fabs(start_length_squared) < 0.0001f)
					{
						t = 1.0f;
					}
					else if (!(fabs(start_length_squared - end_length_squared) < 0.0001f))
					{
						real ratio = square_root_ratio_approximate(start_length_squared, end_length_squared);
						real value = (real)pow(input, ratio);
						t = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
					}
				}
			}
			real const *coefficients = ((s_function_spline const *)header)->coefficients;
			real t2 = t * t;
			real t3 = t2 * t;
			result = coefficients[1] * t2 + coefficients[2] * t + coefficients[0] * t3 + coefficients[3];
			if (header->flags & 1)
			{
				coefficients += 12;
				if (header->type == _function_spline2)
				{
					point2f const *p0 = function_13bec0(field_4_7, 1, 0);
					point2f const *p1 = function_13bec0(field_4_7, 1, 1);
					point2f const *p2 = function_13bec0(field_4_7, 1, 2);
					point2f const *p3 = function_13bec0(field_4_7, 1, 3);
					/* the differences are passed y first here, which is the order retail evaluates them in the high range */
					real end_length_squared = length_squared_components(p2->y - p3->y, p2->x - p3->x);
					if (end_length_squared > 0.0001f)
					{
						/* also y first; the length is still x squared plus y squared */
						vector2f vector;
						vector.j = p0->y - p1->y;
						vector.i = p0->x - p1->x;
						real start_length_squared = length_sq2f(&vector);
						if (fabs(start_length_squared) < 0.0001f)
						{
							t = 1.0f;
						}
						else if (!(fabs(start_length_squared - end_length_squared) < 0.0001f))
						{
							real ratio = square_root_ratio_approximate(start_length_squared, end_length_squared);
							real value = (real)pow(input, ratio);
							t = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
						}
					}
					t2 = t * t;
					t3 = t2 * t;
				}
				real high = coefficients[1] * t2 + coefficients[2] * t + coefficients[0] * t3 + coefficients[3];
				result = interpolate_linear(result, high, range);
			}
			break;
		}
		default:
			goto done;
		}
	}

done:
	return 0.0f > result ? 0.0f : (result > 1.0f ? 1.0f : result);
}

// @retail 0x13bb40
real function_13bb40(s_tag_data const *function, real value)
{
	s_function_header const *header = (s_function_header const *)function->address;
	if (!(header->flags & 0xf0))
	{
		real lo = header->bounds.lo;
		real hi = header->bounds.hi;
		real t = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
		return (hi - lo) * t + lo;
	}
	return value;
}

// @retail 0x13bb90
real function_13bb90(s_tag_data const *function, real input, real range)
{
	if (function->address && function->size > 0)
	{
		real value = function_13b390(function, input, range);
		s_function_header const *header = (s_function_header const *)function->address;
		if (!(header->flags & 0xf0))
		{
			real lo = header->bounds.lo;
			real hi = header->bounds.hi;
			value = 0.0f > value ? 0.0f : (value > 1.0f ? 1.0f : value);
			value = (hi - lo) * value + lo;
		}
		return value;
	}
	return 0.0f;
}

long function_016ae0(real value);

real const g_47ff50 = 16384.0f;

/* interpolates two colours channel by channel in fixed point (MMX) */
static __forceinline dword pixel32_interpolate(real t, dword second, dword first)
{
	__int64 zero = 0;
	__int64 rounding = 0x0001000100010001;

	__asm
	{
		movd mm0, first
		movd mm1, second
		punpcklbw mm0, zero
		punpcklbw mm1, zero
		movss xmm2, t
		mulss xmm2, g_47ff50
		cvtps2pi mm2, xmm2
		pshufw mm2, mm2, 0
		psubw mm1, mm0
		psllw mm1, 3
		pmulhw mm1, mm2
		paddw mm1, rounding
		psraw mm1, 1
		paddw mm0, mm1
		packuswb mm0, mm0
		movd eax, mm0
		emms
	}
}

enum
{
	_function_output_scalar = 0,
	_function_output_color_constant,
	_function_output_color_2,
	_function_output_color_3,
	_function_output_color_4
};

/* retail keeps the frame pointer here (an ebp frame, nothing aligned) */
#pragma optimize("y", off)
// @retail 0x13bc00
dword function_13bc00(s_tag_data const *function, real input)
{
	dword result = 0xffffffff;
	s_function_header const *header = (s_function_header const *)function->address;

	if (header && function->size > 0)
	{
		if (header->type == _function_constant && (!(header->flags & 1) || (header->flags & 0xf0) <= 0x10))
		{
			result = header->colors[0];
			goto local_1;
		}

		input = 0.0f > input ? 0.0f : (input > 1.0f ? 1.0f : input);

		switch (header->flags >> 4)
		{
		case _function_output_scalar:
		{
			input *= 255.0f;
			long value;
			__asm
			{
				fld input
				fistp value
			}
			byte intensity = (byte)value;
			result = 0xff000000 | (intensity << 16) | (intensity << 8) | intensity;
			goto local_1;
		}
		case _function_output_color_constant:
			result = header->colors[0];
			goto local_1;
		case _function_output_color_2:
			result = pixel32_interpolate(input, header->colors[3], header->colors[0]);
			goto local_1;
		case _function_output_color_3:
		{
			input *= 2.0;
			long index = function_016ae0(input);
			input -= index;
			dword a;
			dword b;
			if (index >= 2)
			{
				input = 1.0f;
				goto upper;
			}
			if (index == 0)
			{
				a = header->colors[0];
				b = header->colors[1];
			}
			else
			{
			upper:
				a = header->colors[1];
				b = header->colors[3];
			}
			result = pixel32_interpolate(input, b, a);
			goto local_1;
		}
		case _function_output_color_4:
		{
			input *= 3.0;
			long index = function_016ae0(input);
			input -= index;
			if (index >= 3)
			{
				input = 1.0f;
				index = 2;
			}
			result = pixel32_interpolate(input, header->colors[index + 1], header->colors[index]);
			goto local_1;
		}
		}
	}

local_1:
	return result;
}
#pragma optimize("y", on)

color3f *unpack_color3f(dword pixel, color3f *color);

/* the global colours: white first */
color4f const *g_4686cc;

// @retail 0x13be80
void function_13be80(s_tag_data const *function, real input, color3f *color)
{
	if (function->address && function->size > 0)
	{
		unpack_color3f(function_13bc00(function, input), color);
	}
	else
	{
		*color = *(color3f const *)&g_4686cc->red;
	}
}
