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
#include "normalize3d.h"
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
__forceinline void fog_direction_from_angles(vector3f *out, s_fog_angles const *a)
{
	real pitch_cosine = (real)cos(a->pitch);
	out->i = (real)cos(a->yaw) * pitch_cosine;
	out->j = (real)sin(a->yaw) * pitch_cosine;
	out->k = (real)sin(a->pitch);
}
__forceinline void fog_planar_opposite(vector3f *out, vector3f const *in)
{
	out->i = -in->i;
	out->j = -in->j;
	out->k = 0.0f;
}

__forceinline void fog_color_add_scaled(color3f const *a, color3f const *b, real scale, color3f *result)
{
	result->red = a->red + b->red * scale;
	result->green = a->green + b->green * scale;
	result->blue = a->blue + b->blue * scale;
}
__forceinline void fog_accumulate(s_fog_accumulator *out, color3f const *color, real intensity, real distance, real height, real weight)
{
	weight = weight < 0.0f ? 0.0f : (weight > 1.0f ? 1.0f : weight);
	fog_color_add_scaled(&out->color, color, weight, &out->color);
	out->intensity += intensity * weight;
	out->distance += distance * weight;
	out->height += height * weight;
	out->weight += weight;
	out->remaining *= 1.0f - weight;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))
__forceinline void fog_accumulate_colored(s_fog_accumulator *out, s_fog_base_layer const *layer, real weight)
{
	fog_accumulate(out, &layer->color, layer->intensity, 0.0f, 1.0f, weight);
}
__forceinline void fog_accumulate_layer(s_fog_accumulator *out, s_fog_base_layer const *layer, real weight)
{
	fog_accumulate(out, &layer->color, layer->intensity, layer->distance, layer->height, weight);
}
__forceinline void fog_accumulate_sample(s_fog_accumulator *out, s_fog_direction_sample const *sample, real weight)
{
	fog_accumulate(out, &sample->color, sample->intensity, sample->distance, sample->height, weight);
}

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
			angles.yaw += 3.1415927410125732422f;
			angles.pitch = 0.0f;
			fog_direction_from_angles(&direction, &source->angles);
			fog_direction_from_angles(&opposite, &angles);
		}
		else
		{
			fog_planar_opposite(&opposite, &direction);
			normalize3d(&opposite);
		}
		if (length_sq3f(&direction) > 0.0f)
		{
			for (long j = 0; j < 2; j++)
			{
				s_fog_direction_sample *sample;
				real angle;
				switch (j)
				{
				case 0:
					sample = source->samples[0].count > 0 ? source->samples[0].entries : NULL;
					angle = function_11ce20(&direction, view_direction);
					break;
				case 1:
					sample = source->samples[1].count > 0 ? source->samples[1].entries : NULL;
					angle = function_11ce20(&opposite, view_direction);
					break;
				default:
					continue;
				}
				if (sample && sample->angle_end > sample->angle_start)
				{
					real weight = PIN((angle - sample->angle_end) / (sample->angle_start - sample->angle_end), 0.0f, 1.0f);
					fog_accumulate_sample(first, sample, sample->weights[0] * weight);
					fog_accumulate_sample(second, sample, sample->weights[1] * weight);
					fog_accumulate_sample(third, sample, sample->weights[2] * weight);
				}
			}
		}
	}
	real remaining = first->remaining * second->remaining;
	real third_remaining = third->remaining;
	if (tag->first_count > 0)
		fog_accumulate_layer(first, tag->first, remaining);
	else
		first->weight += remaining;
	if (tag->second_count > 0)
		fog_accumulate_layer(second, tag->second, remaining);
	else
		second->weight += remaining;
	if (tag->third_count > 0)
		fog_accumulate_colored(third, tag->third, third_remaining);
	else
		third->weight += third_remaining;
}
