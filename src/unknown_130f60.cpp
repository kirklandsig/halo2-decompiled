// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_130F60.CPP */

#include "unknown_11c920.h"
#include "globals.h"

/* the scenario's palette entries (8 bytes each): a tag reference */
struct s_scenario_palette_entry
{
	byte unknown00[4];
	long tag_index;
};

/* the palette sources at +0x214 (0x44 bytes each) */
struct s_palette_source_view
{
	byte unknown00[0x3c];
	byte flags;
	byte unknown3d[3];
	short palette_index;
	byte unknown42[2];
};

struct s_scenario_palette_view
{
	byte unknown000[8];
	long palette_count;
	s_scenario_palette_entry *palette;
	byte unknown010[0x214 - 0x10];
	s_palette_source_view *sources;
};

struct s_palette_owner
{
	byte unknown00[0x6c];
	char palette_index;
	byte unknown6d;
	char default_palette_index;
};

struct s_palette_tag_view
{
	byte unknown00[0x10];
	byte flags;
};

// @retail 0x130f60
long function_130f60(s_palette_owner const *owner, long *palette_index)
{
	s_scenario_palette_view *scenario = (s_scenario_palette_view *)g_4e0350;
	long result = NONE;
	*palette_index = NONE;

	if (owner->palette_index != NONE)
	{
		if (owner->palette_index >= 0 && owner->palette_index < scenario->palette_count)
		{
			s_scenario_palette_entry *entry = &scenario->palette[owner->palette_index];
			*palette_index = owner->palette_index;
			result = entry->tag_index;
			goto local_1;
		}
	}
	else
	{
		if (owner->default_palette_index != NONE && owner->default_palette_index >= 0 && owner->default_palette_index < scenario->palette_count)
		{
			s_scenario_palette_entry *entry = &scenario->palette[owner->default_palette_index];
			if (entry->tag_index != NONE && !(((s_palette_tag_view *)g_4e3b44[entry->tag_index & 0xffff].bytes)->flags & 0x10))
			{
				*palette_index = owner->default_palette_index;
				result = entry->tag_index;
			goto local_1;
			}
		}
	}

	s_palette_source_view *source = &scenario->sources[g_4686c4];
	if (source->flags & 1)
	{
		short index = source->palette_index;
		if (index != NONE && index >= 0 && index < scenario->palette_count)
		{
			s_scenario_palette_entry *entry = &scenario->palette[index];
			if (entry->tag_index != NONE)
			{
				*palette_index = index;
				result = entry->tag_index;
			}
		}
	}

local_1:
	return result;
}

#include "unknown_0259d0.h"
#include <math.h>
#include <string.h>

struct s_fog_accumulator
{
	color3f color;
	real intensity;
	real distance;
	real height;
	real weight;
	real remaining;
};
struct s_fog_direction_sample
{
	color3f color;
	real intensity;
	real distance;
	real height;
	real angle_start;
	real angle_end;
	real weights[3];
};
struct s_fog_angles
{
	real yaw;
	real pitch;
};
struct s_fog_sample_set
{
	long count;
	s_fog_direction_sample *entries;
};
struct s_fog_direction
{
	vector3f direction;
	s_fog_angles angles;
	byte unknown14[8];
	s_fog_sample_set samples[2];
	byte unknown2c[8];
};
typedef char assert_fog_sample_set_size[(sizeof(s_fog_sample_set) == 8) ? 1 : -1];
typedef char assert_fog_direction_size[(sizeof(s_fog_direction) == 0x34) ? 1 : -1];
typedef char assert_fog_sample_offset[(offsetof(s_fog_direction, samples) == 0x1c) ? 1 : -1];
struct s_fog_base_layer
{
	color3f color;
	real intensity;
	real distance;
	real height;
};
struct s_fog_direction_tag
{
	byte unknown00[0x48];
	long first_count;
	s_fog_base_layer *first;
	long second_count;
	s_fog_base_layer *second;
	long third_count;
	s_fog_base_layer *third;
	byte unknown60[0x18];
	long direction_count;
	s_fog_direction *directions;
};
real function_11ce20(vector3f const *a, vector3f const *b);

__forceinline void fog_accumulate(s_fog_accumulator *out, color3f const *color, real intensity, real distance, real height, real weight)
{
	weight = weight < 0.0f ? 0.0f : (weight > 1.0f ? 1.0f : weight);
	out->color.red += color->red * weight;
	out->color.green += color->green * weight;
	out->color.blue += color->blue * weight;
	out->intensity += intensity * weight;
	out->distance += distance * weight;
	out->height += height * weight;
	out->weight += weight;
	out->remaining *= 1.0f - weight;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

// score 324 (was 355): Entry ESI save (image composition: it moves to the entry when src/unknown_0350e0.cpp is linked), the 1.0 constant's register (XMM5, retail XMM6) and the third accumulation's scheduling still differ.
// Round 291: Paired double-square terms, complete direction-square terms, both, ordered accumulator
// clamp, and clamp combinations 410/414/420/402/410/414/420; all with disjoint clears before common
// remaining initialization 454 (fast control 402; full 444).
// Round 300: Existing vector setter for source/opposite/trigonometric constructions in seven
// nonempty subsets 413/408/414/410/414/416/423; complete setter graph with readonly source/sample
// records 423 (fast control 402; full 444). Original constructions and arithmetic restored.
// Round 221: real paired descriptor table with original switch 402 fast; direct index 374;
// descriptor pointer/reference 382/382; index after axis 501; null-first descriptor 318;
// word side ordinal 441; readonly direct index 374 (unchanged fast control 402).
// Plain direct index 355 at 1893/1892 kept; descriptor conditional 361 at 1888/1892,
// null-first descriptor 433 at 1886/1892. Entry ESI save, vector homes and x87 storage remain.
// N227: Complete color-by-value/accumulator/angle-copy subsets 571/947/1184/367/676/970/1197;
// all with native input CRT copy 1197 (fast control 374). Seven-point preview lead below gate;
// original helper and complete body restored.
// Round 47: the entry ESI save comes from the rest of the image: a reduced image of this file,
// 0x11ce20's file and globals.cpp saves ESI only around the loop as retail; adding
// src/unknown_0350e0.cpp alone moves it to the entry (other large files do not). Reduced-image
// forms (control 374): `opposite` renamed three ways 374; declared before `direction` 374; `j` at
// function scope 374; sample/axis chosen per case (retail's constant-offset case blocks) 402, axis
// first 484; the angle call with retail's argument order (axis in EDX) 374, plain 355; with
// the per-case blocks 402, plain 444.
// Lane 14 r16 (fast control 374 at 1892 bytes; the fast build shrink-wraps esi like retail, the plain
// build saves it at entry and shifts every loop offset by 4): angle call as (axis, view_direction),
// the callee being symmetric, 374 (retail's ecx/edx setup); each switch case selecting its own sample
// set 402 fast (retail's switch block and [esp+8] counter); opposite renamed to sort first 402
// (same code); the existing normalize_inline for the opposite vector with both 347 fast, 367 plain
// (retail's vector homes; plain still saves esi at entry and spills the third remaining weight);
// the existing normalize 0x30bf0 called instead 526. Body restored.
// Round 240: inner descriptor cursor/two-axis dispatch/byte ordinal and their pairs/all
// 369/417/448/443/452/416/468; direct table-end cursor 512 (fast control 374).
// With full-context control recovered at 355, the descriptor cursor scores 419; original body restored.
// Round 244: complete final base-layer receivers, seven subsets, fast
// 378/374/378/375/379/379/379; complete native layer values 520.
// Original final consumers and complete body restored (fast control 374).
// Round 49 (lane 16): the opposite vector's magnitude from float squares (no double casts) gives
// retail's x87 block and vector homes exactly (fast 372, plain 409 at 1879 bytes alone); with each
// switch case selecting its own sample set 324 plain at 1893 bytes (kept); the angle call as
// (axis, view_direction), retail's EDX/ECX setup, 324 (kept, same count); float squares and the
// call order without the per-case sets 409; the existing vector3f_scale for the reciprocal scale
// 344 fast (1895 bytes, same multiplies). Screened compile-only (same code as kept): the scale rows
// in any operand or row order or through vector3f_assign (retail loads j and multiplies by the
// reciprocal, ours copies the reciprocal; the 3-byte difference drops retail's loop-head padding and
// shifts the rest); the remaining stores after all three memsets (1.0 stays in XMM5). Without the
// angles copy the frame drops to 0x28 (retail 0x30). On the kept form (fast 336): the weight
// clamped in its own statement 406 (1845 bytes); `remaining = remaining * (1 - weight)` 337; the
// reciprocal written inline in each row 336 (same code).
// @retail 0x131030
void function_131030(long tag_index, vector3f const *view_direction, s_fog_accumulator *first, s_fog_accumulator *second, s_fog_accumulator *third)
{
	s_fog_direction_tag *tag = (s_fog_direction_tag *)g_4e3b44[tag_index & 0xffff].bytes;
	memset(first, 0, sizeof(*first));
	first->remaining = 1.0f;
	memset(second, 0, sizeof(*second));
	second->remaining = 1.0f;
	memset(third, 0, sizeof(*third));
	third->remaining = 1.0f;
	for (long i = 0; i < tag->direction_count; i++)
	{
		s_fog_direction *source = &tag->directions[i];
		vector3f direction = source->direction;
		vector3f opposite;
		if (direction.i == 0.0f && direction.j == 0.0f && direction.k == 0.0f)
		{
			s_fog_angles angles = source->angles;
			double yaw = angles.yaw + 3.1415927410125732422f;
			double pitch_cosine = cos(source->angles.pitch);
			direction.i = (real)(cos(source->angles.yaw) * pitch_cosine);
			direction.j = (real)(sin(source->angles.yaw) * pitch_cosine);
			direction.k = (real)sin(source->angles.pitch);
			opposite.i = (real)(cos(yaw) * cos(0.0));
			opposite.j = (real)(sin(yaw) * cos(0.0));
			opposite.k = (real)sin(0.0);
		}
		else
		{
			opposite.i = -direction.i;
			opposite.j = -direction.j;
			opposite.k = 0.0f;
			real magnitude = (real)sqrt(opposite.j * opposite.j + opposite.i * opposite.i);
			if (!(0.0001f > fabs(magnitude)))
			{
				real reciprocal = 1.0f / magnitude;
				opposite.i *= reciprocal;
				opposite.j *= reciprocal;
				opposite.k *= reciprocal;
			}
		}
		if (direction.k * direction.k + direction.j * direction.j + direction.i * direction.i > 0.0f)
		{
			for (long j = 0; j < 2; j++)
			{
				s_fog_direction_sample *sample;
				vector3f const *axis;
				switch (j)
				{
				case 0:
					sample = source->samples[0].count > 0 ? source->samples[0].entries : NULL;
					axis = &direction;
					break;
				case 1:
					sample = source->samples[1].count > 0 ? source->samples[1].entries : NULL;
					axis = &opposite;
					break;
				default:
					continue;
				}
				real angle = function_11ce20(axis, view_direction);
				if (sample && sample->angle_end > sample->angle_start)
				{
					real weight = PIN((angle - sample->angle_end) / (sample->angle_start - sample->angle_end), 0.0f, 1.0f);
					fog_accumulate(first, &sample->color, sample->intensity, sample->distance, sample->height, sample->weights[0] * weight);
					fog_accumulate(second, &sample->color, sample->intensity, sample->distance, sample->height, sample->weights[1] * weight);
					fog_accumulate(third, &sample->color, sample->intensity, sample->distance, sample->height, sample->weights[2] * weight);
				}
			}
		}
	}
	real remaining = first->remaining * second->remaining;
	real third_remaining = third->remaining;
	if (tag->first_count > 0)
		fog_accumulate(first, &tag->first->color, tag->first->intensity, tag->first->distance, tag->first->height, remaining);
	else
		first->weight += remaining;
	if (tag->second_count > 0)
		fog_accumulate(second, &tag->second->color, tag->second->intensity, tag->second->distance, tag->second->height, remaining);
	else
		second->weight += remaining;
	if (tag->third_count > 0)
		fog_accumulate(third, &tag->third->color, tag->third->intensity, 0.0f, 1.0f, third_remaining);
	else
		third->weight += third_remaining;
}
