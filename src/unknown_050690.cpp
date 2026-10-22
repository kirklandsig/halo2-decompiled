// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_050690.CPP: random seeds drawn from the game's random generator
   (lane D) */

#include "unknown_11c920.h"
#include "globals.h"
#include "camera_storage.h"
#include "reset_storage.h"
#include "sound_sources.h"
#include <string.h>
#include <math.h>

/* nine random words and two random reals in [0, 1] (0x50 bytes) */
struct s_random_draw
{
	dword words[9];
	real phases[9];
	real reals[2];
};

static inline dword random_draw_next(dword *seed)
{
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return *seed >> 16;
}

static inline real random_draw_real(dword *seed)
{
	return (real)random_draw_next(seed) * (1.f / 65535.f);
}

// @retail 0x50690
void function_50690(s_random_draw *draw)
{
	memset(draw, 0, sizeof(*draw));
	dword *seed = &g_4e7408->seed;
	for (long i = 0; i < 9; i++)
		draw->words[i] = random_draw_next(seed);
	draw->reals[0] = random_draw_real(seed);
	draw->reals[1] = random_draw_real(seed);
}

struct s_random_draw_definition
{
	short unknown00;
	short additional_word_count;
	byte unknown04[12];
	real period;
	byte unknown14[0x90 - 0x14];
	real rates[9];
};

s_random_draw g_4c9828;
bool g_50944d;

static __forceinline void random_draw_advance(real *value, real elapsed)
{
	elapsed += *value;
	*value = elapsed;
}

// @retail 0x507e0
void __stdcall function_507e0(s_random_draw *draw, const s_random_draw_definition *definition, real elapsed)
{
	long count = definition->additional_word_count + 2;
	if (!draw)
	{
		draw = &g_4c9828;
		if (g_50944d)
		{
			function_50690(draw);
			g_50944d = true;
		}
	}
	if (elapsed > 0.f)
	{
		for (long i = 0; i < count; i++)
		{
			if (definition->rates[i] > 0.f)
			{
				draw->phases[i] += definition->rates[i] * elapsed;
				if (draw->phases[i] >= 1.f)
				{
					draw->phases[i] -= (real)floor(draw->phases[i]);
					draw->words[i]++;
				}
			}
		}
		if (definition->period != 0.f)
			draw->reals[0] += elapsed / definition->period;
		random_draw_advance(&draw->reals[1], elapsed);
	}
}

struct s_noise_curve
{
	long size;
	const byte *data;
};

struct s_noise_draw_definition
{
	short unknown00;
	short exponent;
	byte unknown04[0x34 - 4];
	s_noise_curve horizontal_curve;
	s_noise_curve vertical_curve;
	real bias;
	s_noise_curve weight_curve;
	byte unknown50[0xd0 - 0x50];
	word active_levels;
};

struct s_noise_basis
{
	real scale;
	real forward[3];
	real left[3];
	real up[3];
	real origin[3];
};

struct s_noise_pair
{
	real x;
	real y;
};

struct s_noise_point
{
	real position[3];
	real width;
	real amount;
	dword flags;
};

real function_13b390(void const *function, real input, real range);

static __forceinline real noise_curve_value(const s_noise_curve *curve, real input)
{
	real result;
	if (curve->data && curve->size > 0)
	{
		result = function_13b390(curve, input, 0.0f);
		if (!(curve->data[1] & 0xf0))
		{
			real lower = *(const real *)(curve->data + 4);
			real upper = *(const real *)(curve->data + 8);
			if (result < 0.0f)
				result = 0.0f;
			else if (result > 1.0f)
				result = 1.0f;
			result = lower + (upper - lower) * result;
		}
	}
	else
		result = 0.0f;
	return result;
}

long __stdcall function_516d0(bool reduced, dword seed,
	const s_noise_draw_definition *definition, const s_random_draw *draw,
	const s_noise_basis *basis, real amplitude, s_noise_point *points);

/* the payload function_429a0 queues for the draw below (0x40 bytes) */
struct s_payload_508d0
{
	bool type;
	byte opacity;
	byte unknown02[2];
	long a, b, c;
	point3f position, endpoint;
	vector3f first, second;
};

/* one of an element's parts (0x38 bytes) */
struct s_part_508d0
{
	byte unknown00[0xc];
	short bitmap_index;
	byte unknown0e[2];
	s_noise_curve curve_10;
	s_noise_curve curve_18;
	s_noise_curve curve_20;
	s_noise_curve curve_28;
	s_noise_curve curve_30;
};

/* an element of the definition (0xec bytes); the same bytes are read as
   s_random_draw_definition and s_noise_draw_definition */
struct s_element_508d0
{
	byte flags;
	byte unknown01;
	short exponent;
	real length;
	short copies;
	byte unknown0a[2];
	real angle;
	real period;
	byte unknown14[0x20 - 0x14];
	long effect_tag_index;
	byte unknown24[4];
	long bitmap_tag_index;
	byte unknown2c[0xd4 - 0x2c];
	long part_count;
	s_part_508d0 *parts;
	s_noise_curve curve_dc;
	s_noise_curve curve_e4;
};

struct s_definition_508d0
{
	byte unknown00[2];
	short kind;
	byte unknown04[0x40 - 4];
	real near_distance;
	real far_distance;
	byte unknown48[0x68 - 0x48];
	long element_count;
	s_element_508d0 *elements;
};

struct s_collision_result_1697c0
{
	long type;
	real t;
	point3f point;
	s_location start_location;
	s_location end_location;
	union
	{
		short material_type;
		long material;
	};
	plane3f plane;
	byte unknown38[0x5c - 0x38];
};

struct s_render_reset_state;
struct s_tag_data;
struct s_501e0;
real function_30bf0(vector3f *v);
void function_16b10(s_render_reset_state *state);
byte *function_c4210(long index, long element_index);
long function_c4250(long index);
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, short element_index, long unused, long index, long variant,
	long *first_value04, long *second_value04, long *first_value, long *second_value, long *first_value0c, long *second_value0c);
long function_1765e0(point3f const *point, vector3f const *direction, vector3f const *normal, long tag_index, long mode, long deterministic);
dword vector3d_compress(vector3f const *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);
bool __stdcall function_4f7e0(long tag, s_501e0 const *points, long count, short bitmap,
	real cosine, real sine, bool segments, bool perspective, bool filtered);
extern byte g_46713c;
extern byte g_4670bc;
extern dword g_4b8474;
extern short g_47d8e0;
extern "C" void __stdcall D3DDevice_SetRenderState_SampleAlpha(dword value);

PRIVATE inline void point_along_vector_508d0(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

PRIVATE __forceinline void cross_product_508d0(vector3f const *a, vector3f const *b, vector3f *result)
{
	real k = a->i * b->j - b->i * a->j;
	real j = a->k * b->i - a->i * b->k;
	real i = a->j * b->k - a->k * b->j;
	result->i = i;
	result->j = j;
	result->k = k;
}

PRIVATE inline real curve_value_508d0(const s_noise_curve *curve, real input)
{
	if (curve->data && curve->size > 0)
	{
		real value = function_13b390(curve, input, 0.0f);
		const byte *data = curve->data;
		if (!(data[1] & 0xf0))
		{
			real low = *(const real *)(data + 4);
			real high = *(const real *)(data + 8);
			if (value < 0.0f)
				value = 0.0f;
			else if (value > 1.0f)
				value = 1.0f;
			value = low + (high - low) * value;
		}
		return value;
	}
	return 0.0f;
}

/* pack_color4f (0x131e50), which retail inlines here */
PRIVATE __forceinline dword pack_color4f_508d0(const color4f *color)
{
	long b, g, r, a;
	real scale = 255.0f;
	dword pixel = 0;

	__asm
	{
		mov edx, color
		fld dword ptr [edx]
		fld dword ptr [edx + 4]
		fld dword ptr [edx + 8]
		fld dword ptr [edx + 12]
		fld scale
		fmul st(4), st(0)
		fmul st(3), st(0)
		fmul st(2), st(0)
		fmulp st(1), st(0)
		fistp b
		fistp g
		fistp r
		fistp a
		mov edx, b
		mov ebx, g
		mov ecx, r
		mov eax, a
		shl ebx, 8
		shl ecx, 0x10
		shl eax, 0x18
		or edx, ebx
		or edx, ecx
		or edx, eax
		mov pixel, edx
	}
	return pixel;
}

// score 632 (was 887): 3556 bytes, retail 3578. Unmatched callees decide part of it: retail
// passes function_516d0's flag in cl, function_1883e0's tag in ebx, and function_4f7e0's tag in
// ecx with the points on the stack and the short sign-extended; our builds of those callees take
// them differently. The rest: the frame has one scalar slot fewer below the direction vector, so
// it and everything above it sit 4 bytes lower; register choice; and the x87 order of the
// kind-1 distance and of the facing dot product.
// @retail 0x508d0
void __stdcall function_508d0(void *payload)
{
	s_payload_508d0 *state = (s_payload_508d0 *)payload;
	if (state->b == NONE)
		return;
	s_definition_508d0 *definition = (s_definition_508d0 *)g_4e3b44[state->b & 0xffff].bytes;
	if (definition->element_count <= 0)
		return;
	real fade = 1.0f;
	vector3f direction;
	vector3d_from_points3d(&state->position, &g_4b9da0, &direction);
	real distance = function_30bf0(&direction);
	if (definition->far_distance > definition->near_distance)
	{
		if (0.0f > (definition->far_distance - distance) / (definition->far_distance - definition->near_distance))
			fade = 0.0f;
		else if ((definition->far_distance - distance) / (definition->far_distance - definition->near_distance) > 1.0f)
			fade = 1.0f;
		else
			fade = (definition->far_distance - distance) / (definition->far_distance - definition->near_distance);
	}
	if (!(fade > 0.0f))
		return;
	g_46713c = true;
	g_4670bc = true;
	function_16b10((s_render_reset_state *)g_485b48);
	for (long i = 0; i < definition->element_count; i++)
	{
		s_element_508d0 *element = &definition->elements[i];
		if (element->part_count <= 0)
			continue;
		short requested = element->copies;
		if (requested <= 0)
			continue;
		s_random_draw *draw;
		if (state->c != NONE)
			draw = (s_random_draw *)function_c4210(state->c, i);
		else
			draw = &g_4c9828;
		long copies = requested > 8 ? 8 : requested;
		if (state->c == NONE)
			function_507e0(NULL, (const s_random_draw_definition *)element, 1.0f / 30.0f);
		s_noise_basis bases[2];
		vector3f *forward = (vector3f *)bases[0].forward;
		vector3f *left = (vector3f *)bases[0].left;
		vector3f *up = (vector3f *)bases[0].up;
		*(point3f *)bases[0].origin = state->position;
		bases[0].scale = 1.0f;
		if (element->flags & 1)
		{
			*up = state->first;
			*left = state->second;
			cross_product_508d0(up, left, forward);
		}
		else
		{
			*up = state->first;
			*left = *g_4687b0;
			cross_product_508d0(up, left, forward);
			if (function_30bf0(forward) < 0.0001f)
			{
				*forward = *g_4687a4;
				*left = *g_4687a4;
			}
		}
		bases[1] = bases[0];
		point3f *origin = (point3f *)bases[0].origin;
		point3f *end = (point3f *)bases[1].origin;
		point_along_vector_508d0(origin, &state->first, element->length, end);
		for (long j = 0; j < copies; j++)
		{
			real fraction = 1.0f;
			if (element->angle != 0.0f)
			{
				real angle = (real)j * 6.2831855f / (real)copies;
				if (element->period != 0.0f)
					angle = draw->reals[0] * 6.2831855f + angle;
				double radius = tan(element->angle) * element->length;
				point_along_vector_508d0(origin, &state->first, element->length, end);
				point_along_vector_508d0(end, forward, (real)(cos(angle) * radius), end);
				point_along_vector_508d0(end, left, (real)-(sin(angle) * radius), end);
			}
			if (definition->kind == 1)
			{
				*end = state->endpoint;
				if (!(element->length > 0.0f))
					goto curves;
				double x = (double)end->x - origin->x;
				double y = (double)end->y - origin->y;
				double z = (double)end->z - origin->z;
				fraction = (real)(sqrt(x * x + z * z + y * y) / element->length);
			}
			else
			{
				if (!(element->flags & 4))
					goto curves;
				s_collision_result_1697c0 collision;
				vector3f ray;
				collision.material_type = NONE;
				vector3d_from_points3d(origin, end, &ray);
				/* retail calls function_c4250 here rather than inlining it */
#pragma inline_depth(0)
				if (!function_1697c0(0x680000d, origin, &ray, function_c4250(state->c), NONE, &collision))
					goto curves;
#pragma inline_depth()
				long tag_index = element->effect_tag_index;
				*end = collision.point;
				fraction = collision.t;
				if (tag_index != NONE && function_30bf0(&ray) > 0.0001f)
				{
					long second_values[3];
					long first_values[3];
					function_1883e0(tag_index, false, &collision.point, g_47d8e0, collision.material, 15, 0,
						&first_values[0], &second_values[0], &first_values[1], &second_values[1],
						&first_values[2], &second_values[2]);
					for (long k = 0; k < 3; k++)
					{
						if (second_values[k] != NONE)
							function_1765e0(&collision.point, &ray, &collision.plane.n, second_values[k], 0, 0);
						if (first_values[k] != NONE)
						{
							s_sound_position sound;
							memset(&sound, 0, sizeof(sound));
							sound.position = collision.point;
							sound.compressed_forward = vector3d_compress(&ray);
							sound.velocity = *g_4687a4;
							sound.location = collision.end_location;
							function_1895f0(&sound, 1.0f, first_values[k]);
						}
					}
				}
			}
			if (0.0f > fraction)
				fraction = 0.0f;
			else if (fraction > 1.0f)
				fraction = 1.0f;
		curves:
			real amplitude = curve_value_508d0(&element->curve_dc, fraction);
			real opacity = curve_value_508d0(&element->curve_e4, fraction);
			s_noise_point points[257];
			long count = function_516d0(state->type, j << 8, (const s_noise_draw_definition *)element, draw,
				bases, amplitude, points);
			if (count <= 1)
				continue;
			real inverse = 1.0f / (real)(count - 1);
			for (long m = 0; m < element->part_count; m++)
			{
				s_part_508d0 *part = &element->parts[m];
				vector3f const *first = &state->first;
				real alpha = (real)state->opacity * opacity * fade * (1.0f / 255.0f);
				alpha = curve_value_508d0(&part->curve_20, (real)m + draw->reals[1]) * alpha;
				real facing = (real)fabs((double)g_4b9dac.i * first->i +
					(double)g_4b9dac.j * first->j + (double)g_4b9dac.k * first->k);
				alpha = curve_value_508d0(&part->curve_28, facing) * alpha;
				s_noise_point *point = points;
				for (long k = 0; k < count; k++, point++)
				{
					real t = (real)k * inverse;
					real input = function_13b390(&part->curve_18, t, 0.0f);
					color4f colour;
					if (part->curve_18.data && part->curve_18.size > 0)
					{
						/* retail calls unpack_color3f here rather than inlining it */
#pragma inline_depth(0)
						unpack_color3f(function_13bc00((s_tag_data const *)&part->curve_18, input), (color3f *)&colour.red);
#pragma inline_depth()
					}
					else
						*(color3f *)&colour.red = *(color3f const *)&g_4686cc->red;
					colour.alpha = alpha;
					point->flags = pack_color4f_508d0(&colour);
					point->width = curve_value_508d0(&part->curve_30, t);
					point->amount = curve_value_508d0(&part->curve_10, t);
				}
				function_4f7e0(element->bitmap_tag_index, (s_501e0 const *)points, count, part->bitmap_index,
					1.0f, 0.0f, true, !(element->flags & 8), false);
			}
		}
	}
	if (!g_46713c)
	{
		g_4b8474 = 0;
#pragma inline_depth(0)
		D3DDevice_SetRenderState_SampleAlpha(0);
#pragma inline_depth()
	}
	g_46713c = true;
}

// @retail 0x516d0
long __stdcall function_516d0(bool reduced, dword seed,
	const s_noise_draw_definition *definition, const s_random_draw *draw,
	const s_noise_basis *basis, real amplitude, s_noise_point *points)
{
	long exponent;
	if (reduced)
	{
		exponent = definition->exponent - 2;
		exponent = exponent < 0 ? 0 : exponent;
	}
	else
		exponent = definition->exponent;
	long count = (4 << exponent) + 1;
	long level_count = exponent + 2;
	real inverse_count = 1.0f / (real)(count - 1);
	__declspec(align(8)) s_noise_pair noise[257];
	s_noise_pair sums[257] = {0};
	real weights[257] = {0};
	long seed_offset = 0;
	/* volatile keeps the level counter in its stack slot; it takes this near miss
	   from score 778 to 734 */
	for (volatile long level = 0; level < level_count; level++, seed_offset += 3)
	{
		if (!(definition->active_levels & (1 << level)))
			continue;
		real phase = draw->phases[level];
		real square = phase * phase;
		real factors[3];
		factors[0] = square * 0.5f - phase + 0.5f;
		factors[1] = phase - square + 0.5f;
		factors[2] = phase * 0.0f + square * 0.5f;
		long stride = (count - 1) >> level;
		dword level_seed = draw->words[level] + seed_offset + seed;
		dword state = level_seed;
		long i;
		for (i = 0; i < count; i += stride)
		{
			noise[i].x = (random_draw_real(&state) * 2.0f - 1.0f) * factors[0];
			noise[i].y = (random_draw_real(&state) * 2.0f - 1.0f) * factors[0];
		}
		state = level_seed + 1;
		for (i = 0; i < count; i += stride)
		{
			noise[i].x += (random_draw_real(&state) * 2.0f - 1.0f) * factors[1];
			noise[i].y += (random_draw_real(&state) * 2.0f - 1.0f) * factors[1];
		}
		state = level_seed + 2;
		for (i = 0; i < count; i += stride)
		{
			noise[i].x += (random_draw_real(&state) * 2.0f - 1.0f) * factors[2];
			noise[i].y += (random_draw_real(&state) * 2.0f - 1.0f) * factors[2];
		}
		if (stride > 1)
		{
			long half = stride / 2;
			for (i = half; i < count; i += stride)
			{
				noise[i].x = (noise[i + half].x + noise[i - half].x) * 0.5f;
				noise[i].y = (noise[i + half].y + noise[i - half].y) * 0.5f;
			}
			if (stride > 2)
			{
				real inverse_stride = 1.0f / (real)stride;
				s_noise_pair left = noise[0];
				for (i = 0; i < count; i += stride)
				{
					s_noise_pair center = noise[i];
					long right_index = i + half;
					if (right_index > count - 1)
						right_index = count - 1;
					s_noise_pair right = noise[right_index];
					for (long j = 0; j < stride; j++)
					{
						long index = i - half + j;
						if (index < 0 || index >= count)
							continue;
						real fraction = (real)j * inverse_stride;
						real remaining = 1.0f - fraction;
						real middle = fraction * remaining * 2.0f;
						real end = fraction * fraction;
						real start = remaining * remaining;
						noise[index].x = end * right.x + middle * center.x + start * left.x;
						noise[index].y = end * right.y + middle * center.y + start * left.y;
					}
					left = right;
				}
			}
		}
		for (i = 0; i < count; i++)
		{
			real value = noise_curve_value(&definition->weight_curve, (real)i * inverse_count);
			double weight = pow(2.0, -(double)((1.0f - value) * (real)level));
			sums[i].x = (real)(sums[i].x + weight * noise[i].x);
			sums[i].y = (real)(sums[i].y + weight * noise[i].y);
			weights[i] = (real)(weights[i] + weight);
		}
	}
	long i;
	for (i = 0; i < count; i++)
	{
		if (weights[i] > 0.0f)
		{
			real inverse_weight = 1.0f / weights[i];
			sums[i].x *= inverse_weight;
			sums[i].y *= inverse_weight;
		}
		else
		{
			sums[i].x = 0.0f;
			sums[i].y = 0.0f;
		}
	}
	real delta[3];
	for (i = 0; i < 3; i++)
		delta[i] = basis[1].origin[i] - basis[0].origin[i];
	for (i = 0; i < count; i++)
	{
		real fraction = (real)i * inverse_count;
		real vertical = sums[i].y;
		if (definition->bias != 1.0f)
		{
			real linear = (definition->bias + 1.0f) * 0.5f;
			real quadratic = (1.0f - definition->bias) * 0.25f;
			vertical = (linear + vertical * quadratic) * vertical + quadratic;
		}
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] = delta[axis] * fraction + basis[0].origin[axis];
		real horizontal_scale = noise_curve_value(&definition->horizontal_curve, fraction) * sums[i].x * amplitude;
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] += basis[0].forward[axis] * horizontal_scale;
		real vertical_scale = noise_curve_value(&definition->vertical_curve, fraction) * vertical * amplitude;
		for (long axis = 0; axis < 3; axis++)
			points[i].position[axis] += basis[0].left[axis] * vertical_scale;
		points[i].width = 1.0f;
		points[i].amount = 1.0f;
		points[i].flags = 0;
	}
	return count;
}
