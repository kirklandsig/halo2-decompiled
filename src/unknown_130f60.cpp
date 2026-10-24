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
struct s_fog_direction
{
	vector3f direction;
	s_fog_angles angles;
	byte unknown14[8];
	long forward_count;
	s_fog_direction_sample *forward;
	long opposite_count;
	s_fog_direction_sample *opposite;
	byte unknown2c[8];
};
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
	out->height += height * weight;
	out->weight += weight;
	out->intensity += intensity * weight;
	out->distance += distance * weight;
	out->remaining *= 1.0f - weight;
}

#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

// score 308 (was 444): retail saves esi only around the outer loop (ours at entry, shifting the
// first block by one instruction), copies the clamped weight (movaps) before multiplying the
// green and blue fields, and orders the accumulator loads and stores differently after the loop.
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
				case 0: sample = source->forward_count > 0 ? source->forward : NULL; axis = &direction; break;
				case 1: sample = source->opposite_count > 0 ? source->opposite : NULL; axis = &opposite; break;
				default: continue;
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
