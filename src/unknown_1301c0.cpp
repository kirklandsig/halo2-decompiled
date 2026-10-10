// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1301C0.CPP: the fog state blended each tick (lane L's 0x12e5e0
   updates it through these) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

#define PIN(n,floor,ceiling) ((n)<(floor) ? (floor) : ((n)>(ceiling)?(ceiling):(n)))

/* unknown_033a0b.cpp: a global colour pointer (its colour at +4) */
struct s_33a0b_default;
extern s_33a0b_default *g_4686d4;

struct s_fog_layer
{
	color3f color;
	real intensity;
	real distance;
	real height;
};

struct s_fog_state
{
	byte unknown00[0x1c];
	s_fog_layer layers[3];
	byte unknown64[0x6c - 0x64];
	long index6c;
	color3f color70;
	color3f color7c;
	real value88;
	real value8c;
	real value90;
	real value94;
	byte unknown98[0xa0 - 0x98];
	s_fog_layer pending;
	real valueb8;
	real valuebc;
	bool flagc0;
	byte unknownc1[0xf0 - 0xc1];
	dword flags;
	byte unknownf4[0x10c - 0xf4];
	real value10c;
	real value110;
	byte unknown114[0x118 - 0x114];
	real value118;
};

// @retail 0x130bb0
void function_130bb0(s_fog_state *fog)
{
	if (fog->index6c != NONE && fog->value8c > 0.0001f && fog->value94 > 0.0001f)
	{
		real value90;

		fog->color70.red = PIN(fog->color70.red, 0.0f, 1.0f);
		fog->color70.green = PIN(fog->color70.green, 0.0f, 1.0f);
		fog->color70.blue = PIN(fog->color70.blue, 0.0f, 1.0f);
		fog->color7c.red = PIN(fog->color7c.red, 0.0f, 1.0f);
		fog->color7c.green = PIN(fog->color7c.green, 0.0f, 1.0f);
		fog->color7c.blue = PIN(fog->color7c.blue, 0.0f, 1.0f);
		fog->value88 = PIN(fog->value88, 0.0f, 1.0f);
		fog->value8c = PIN(fog->value8c, fog->value88, 1.0f);
		value90 = fog->value90 + 0.0001f;
		fog->value94 = value90 > fog->value94 ? value90 : fog->value94;
		if (fog->value8c > 0.9999f)
		{
			fog->value8c = 1.0f;
		}
	}
	else
	{
		fog->value8c = 0.0f;
	}

	if (fog->value8c == 0.0f)
	{
		color3f const *black = (color3f const *)((byte const *)g_4686d4 + 4);

		fog->color70 = *black;
		fog->color7c = *black;
		fog->value88 = 0.0f;
		fog->value8c = 0.0f;
		fog->value90 = 0.0f;
		fog->value94 = 0.0f;
	}
}

// @retail 0x130de0
long function_130de0(s_fog_state *fog)
{
	if (fog->pending.intensity > 0.0f)
	{
		long result = 3;
		real distance;

		if (fog->value10c != 1.0f)
		{
			goto local_1;
		}
		if (fog->value118 != 0.0f)
		{
			goto local_1;
		}

		if (fog->layers[0].intensity == 0.0f)
		{
			goto use_layer0;
		}
		if (fog->layers[1].intensity == 0.0f)
		{
			goto use_layer1;
		}
		if (fog->pending.intensity != 1.0f)
		{
			goto local_1;
		}
		distance = fog->pending.distance > fog->pending.height ? fog->pending.distance : fog->pending.height;
		if (fog->layers[0].distance >= distance)
		{
use_layer0:
			fog->layers[0].color = fog->pending.color;
			fog->layers[0].intensity = fog->pending.intensity;
			fog->layers[0].distance = fog->value110;
			fog->layers[0].height = fog->pending.distance;
			result = 4;
		}
		else if (fog->layers[1].distance >= distance)
		{
use_layer1:
			fog->layers[1].color = fog->pending.color;
			fog->layers[1].intensity = fog->pending.intensity;
			fog->layers[1].distance = fog->value110;
			fog->layers[1].height = fog->pending.distance;
			result = 5;
		}
		else
		{
			goto local_1;
		}
		fog->pending.intensity = 0.0f;
		fog->value10c = 0.0f;
		fog->value118 = 1.0f;
local_1:
		return result;
	}
	if (fog->layers[0].intensity > 0.0f || fog->layers[1].intensity > 0.0f)
	{
		return 1;
	}
	if (fog->layers[2].intensity > 0.0f)
	{
		return 2;
	}
	return 0;
}

static __forceinline color3f const *fog_black(void)
{
	return (color3f const *)((byte const *)g_4686d4 + 4);
}

/* pins a layer's colour, keeps its height above its distance and clears a
   faded layer */
#define FOG_LAYER_VALIDATE(layer) \
	if ((layer).intensity > 0.0001f) \
	{ \
		real height; \
		(layer).color.red = PIN((layer).color.red, 0.0f, 1.0f); \
		(layer).color.green = PIN((layer).color.green, 0.0f, 1.0f); \
		(layer).color.blue = PIN((layer).color.blue, 0.0f, 1.0f); \
		height = (layer).distance + 0.0001f; \
		(layer).height = height > (layer).height ? height : (layer).height; \
		if ((layer).intensity > 0.9999f) \
		{ \
			(layer).intensity = 1.0f; \
		} \
	} \
	else \
	{ \
		(layer).intensity = 0.0f; \
	} \
	if ((layer).intensity == 0.0f) \
	{ \
		(layer).color = *fog_black(); \
		(layer).distance = 0.0f; \
		(layer).height = 0.0f; \
	}

// @retail 0x1301c0
void function_1301c0(s_fog_state *fog)
{
	if (fog->flags & 1)
	{
		fog->layers[0].intensity = 0.0f;
	}
	if (fog->flags & 2)
	{
		fog->layers[1].intensity = 0.0f;
	}
	if (fog->flags & 4)
	{
		fog->pending.intensity = 0.0f;
	}

	FOG_LAYER_VALIDATE(fog->layers[0]);
	FOG_LAYER_VALIDATE(fog->layers[1]);

	if (fog->pending.intensity > 0.0001f)
	{
		fog->pending.color.red = PIN(fog->pending.color.red, 0.0f, 1.0f);
		fog->pending.color.green = PIN(fog->pending.color.green, 0.0f, 1.0f);
		fog->pending.color.blue = PIN(fog->pending.color.blue, 0.0f, 1.0f);
		fog->pending.distance = 0.0001f > fog->pending.distance ? 0.0001f : fog->pending.distance;
		fog->pending.height = 0.0001f > fog->pending.height ? 0.0001f : fog->pending.height;
		if (!fog->flagc0)
		{
			fog->valuebc = 0.0001f > fog->valuebc ? 0.0001f : fog->valuebc;
			fog->valueb8 = fog->valuebc * 0.125f;
		}
		if (fog->pending.intensity > 0.9999f)
		{
			fog->pending.intensity = 1.0f;
		}
	}
	else
	{
		fog->pending.intensity = 0.0f;
	}

	if (fog->pending.intensity == 0.0f)
	{
		fog->pending.color = *fog_black();
		fog->pending.distance = 0.0f;
		fog->pending.height = 0.0f;
		fog->valueb8 = 0.0f;
		fog->valuebc = 0.0f;
	}

	if (fog->layers[2].intensity > 0.0001f)
	{
		fog->layers[2].color.red = PIN(fog->layers[2].color.red, 0.0f, 1.0f);
		fog->layers[2].color.green = PIN(fog->layers[2].color.green, 0.0f, 1.0f);
		fog->layers[2].color.blue = PIN(fog->layers[2].color.blue, 0.0f, 1.0f);
		if (fog->layers[2].intensity > 0.9999f)
		{
			fog->layers[2].intensity = 1.0f;
		}
	}
	else
	{
		fog->layers[2].intensity = 0.0f;
	}

	if (fog->layers[2].intensity == 0.0f)
	{
		fog->layers[2].color = *fog_black();
	}
}

#include "globals.h"
bool function_16e210(long cluster_index, long value);

struct s_fog_plane_view
{
	byte unknown00[0x1c];
	s_fog_layer layers[2];
	color3f color4c;
	real intensity58;
	color3f combined_color;
	real combined_intensity;
	long index6c;
	color3f color70;
	color3f color7c;
	real value88;
	real value8c;
	real value90;
	real value94;
	byte unknown98[4];
	long tag_index;
	s_fog_layer pending;
	real valueb8;
	real valuebc;
	bool flagc0;
	byte unknownc1[3];
	real valuec4;
	byte unknownc8[0x10];
	real valued8;
	real valuedc;
	byte unknowne0[0x14];
	plane3f plane;
	byte unknown104[4];
	real plane_distance;
	real blend;
	real offset;
	byte unknown114[4];
	real fade;
};
struct s_fog_blend_definition
{
	color3f color0;
	color3f colorc;
	real value18;
	real value1c;
	real value20;
	real value24;
	real threshold;
	byte unknown2c[4];
	long index;
};
struct s_fog_blend_tag
{
	byte unknown00[0x30];
	long count;
	s_fog_blend_definition *definitions;
};
__forceinline real fog_pin_unit(real value)
{
	value = PIN(value, 0.0f, 1.0f);
	if (value < 0.0001f)
		value = 0.0f;
	else if (value > 0.9999f)
		value = 1.0f;
	return value;
}

// @retail 0x1305d0
void function_1305d0(point3f const *point, long cluster_index, s_fog_plane_view *fog, bool force)
{
	if (fog->pending.intensity > 0.0f)
	{
		if (force)
		{
			fog->plane_distance = 0.0f - fog->pending.height;
			fog->blend = 1.0f;
		}
		else
		{
			fog->plane_distance = point->x * fog->plane.i + fog->plane.j * point->y + fog->plane.k * point->z - fog->plane.d;
			fog->blend = fog_pin_unit(0.0f - fog->plane_distance / fog->pending.height);
			fog->offset = 0.0f - (fog->valuec4 / fog->pending.distance) * (0.0f > fog->plane_distance ? 0.0f : fog->plane_distance);
			if (0.0f > fog->plane_distance && !function_16e210(cluster_index, fog->tag_index))
			{
				fog->plane_distance = 0.0f;
				fog->blend = 0.0f;
				fog->offset = 0.0f;
			}
		}
		if (fog->tag_index != NONE)
		{
			s_fog_blend_tag *tag = (s_fog_blend_tag *)g_4e3b44[fog->tag_index & 0xffff].bytes;
			if (tag->count > 0)
			{
				s_fog_blend_definition *definition = tag->definitions;
				real threshold = PIN(definition->threshold, 0.0f, 0.9999f);
				real blend = PIN((fog->blend - threshold) / (1.0f - threshold), 0.0f, 1.0f);
				if (blend > 0.0f)
				{
					fog->color70.red += (definition->color0.red - fog->color70.red) * blend;
					fog->color70.green += (definition->color0.green - fog->color70.green) * blend;
					fog->color70.blue += (definition->color0.blue - fog->color70.blue) * blend;
					fog->color7c.red += (definition->colorc.red - fog->color7c.red) * blend;
					fog->color7c.green += (definition->colorc.green - fog->color7c.green) * blend;
					fog->color7c.blue += (definition->colorc.blue - fog->color7c.blue) * blend;
					fog->value88 += (definition->value18 - fog->value88) * blend;
					fog->value8c += (definition->value1c - fog->value8c) * blend;
					fog->value90 += (definition->value20 - fog->value90) * blend;
					fog->value94 += (definition->value24 - fog->value94) * blend;
					if (fog->index6c == NONE)
						fog->index6c = definition->index;
				}
			}
		}
		fog->offset += fog->valuedc * fog->valued8;
		if (fog->flagc0)
		{
			fog->valuebc = 1024.0f;
			fog->valueb8 = 1023.0f;
		}
		fog->valueb8 = fog->valueb8 > 0.0f ? fog->valueb8 : 0.0f;
		real end = fog->valueb8 + 0.0001f;
		fog->valuebc = fog->valuebc > end ? fog->valuebc : end;
		if (fog->layers[0].intensity > 0.0f || fog->layers[1].intensity > 0.0f)
			fog->fade = fog_pin_unit((fog->plane_distance - fog->valueb8) / (fog->valuebc - fog->valueb8));
		else
			fog->fade = 0.0f;
	}
	else
		fog->fade = 1.0f;
	real weight = PIN(fog->blend * fog->pending.intensity, 0.0f, 1.0f);
	if (fog->blend > 0.0f)
	{
		real scale = PIN(fog->intensity58, 0.0f, 1.0f);
		real remainder = 1.0f - weight;
		scale = PIN(remainder, 0.0f, 1.0f) * scale;
		real value = fog->pending.color.red * weight + fog->color4c.red * scale;
		fog->combined_color.red = PIN(value, 0.0f, 1.0f);
		value = fog->pending.color.green * weight + fog->color4c.green * scale;
		fog->combined_color.green = PIN(value, 0.0f, 1.0f);
		value = fog->pending.color.blue * weight + fog->color4c.blue * scale;
		fog->combined_color.blue = PIN(value, 0.0f, 1.0f);
		value = 1.0f - fog->intensity58;
		value = PIN(value, 0.0f, 1.0f);
		real remaining = PIN(remainder, 0.0f, 1.0f);
		fog->combined_intensity = fog_pin_unit(1.0f - remaining * value);
	}
	else
	{
		real scale = PIN(fog->intensity58, 0.0f, 1.0f);
		real value = fog->color4c.red * scale;
		fog->combined_color.red = PIN(value, 0.0f, 1.0f);
		value = fog->color4c.green * scale;
		fog->combined_color.green = PIN(value, 0.0f, 1.0f);
		value = fog->color4c.blue * scale;
		fog->combined_color.blue = PIN(value, 0.0f, 1.0f);
		fog->combined_intensity = fog_pin_unit(fog->intensity58);
	}
}
