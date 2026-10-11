#include "unknown_11c920.h"
#include "globals.h"
#include <string.h>
#include <math.h>

#include "unknown_123b30.h"
#include "crc.h"
// @flags /O2 /arch:SSE /Gr

/* Light pool queries and the shapes copied from definitions and placements. */
s_record_pool *g_4e030c;
long g_4e0308;

extern point3f g_4b9da0;

// @retail 0xc0a50
real function_c0a50(point3f const *point, long index, real range, real radius, real const *table)
{
	real x = point->x - g_4b9da0.x;
	real y = point->y - g_4b9da0.y;
	real z = point->z - g_4b9da0.z;
	radius = (real)sqrt((z * z + x * x) + y * y) - radius - table[index];
#define LIGHT_FADE_AB (radius / (range > 0.1f ? range : 0.1f))
	return 1.0f - (LIGHT_FADE_AB < 0.0f ? 0.0f : LIGHT_FADE_AB > 1.0f ? 1.0f : LIGHT_FADE_AB);
#undef LIGHT_FADE_AB
}

struct s_5107e8
{
	bool flag0;
	byte unknown01[3];
	long time4;
	bool flag8;
};

extern s_5107e8 *g_5107e8;
real function_17ca10(real x, short curve);

extern void *g_4e0310;
extern void *g_4e0314;
extern void *g_4e0318;

__declspec(noinline) void function_c0040();

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0xc0040
void function_c0040()
{
	s_record_pool *array = g_4e030c;
	array->valid = true;
	record_pool_release_all(array);
	_ReadWriteBarrier();
	g_5107e8->flag8 = true;
	_ReadWriteBarrier();
	memset(g_4e0310, 0xff, 512 * sizeof(long));
	_ReadWriteBarrier();
	array = (s_record_pool *)g_4e0318;
	array->valid = true;
	record_pool_release_all(array);
	_ReadWriteBarrier();
	array = (s_record_pool *)g_4e0314;
	array->valid = true;
	record_pool_release_all(array);
	_ReadWriteBarrier();
	g_5107e8->flag0 = false;
	g_5107e8->time4 = 0;
}
#pragma function(_ReadWriteBarrier)

void __stdcall function_c2d00(long light_index);
void __stdcall function_c3260(long light_index, bool clear_object_flag);

// @retail 0xc00a0
void function_c00a0()
{
	s_record_pool_iterator iterator;
	iterator.data = g_4e030c;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	byte *light;
	while ((light = data_iterator_next_inlined(&iterator)) != 0)
	{
		if (*(long *)(light + 0x4c) == NONE && *(long *)(light + 0x58) == NONE)
			function_c2d00(iterator.datum_index);
	}
}

// @retail 0xc01c0
void function_c01c0()
{
	s_record_pool_iterator iterator;
	iterator.data = g_4e030c;
	iterator.index = NONE;
	iterator.datum_index = NONE;
	byte *light;
	while ((light = data_iterator_next_inlined(&iterator)) != 0)
	{
		if (*(long *)(light + 0x4c) == NONE)
			function_c3260(iterator.datum_index, true);
	}
}

struct s_light_object_ab
{
	long tag_index;
	dword : 6;
	dword flag6 : 1;
	dword : 25;
};

struct s_light_object_header_ab
{
	byte unknown00[8];
	s_light_object_ab *object;
};

// @retail 0xc1670
void __stdcall function_c1670(long object_index)
{
	s_light_object_ab *object = ((s_light_object_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
	// Preserve the saved pointer for the shared flag-clear exit.
	s_light_object_ab *volatile saved_object = object;
	if (TEST_FIELD_BIT(object->flag6))
	{
		s_record_pool_iterator iterator;
		iterator.data = g_4e030c;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		byte *light;
		while ((light = data_iterator_next_inlined(&iterator)) != 0)
		{
			if (*(long *)(light + 0x58) == object_index)
				function_c3260(iterator.datum_index, false);
		}
	}
	saved_object->flag6 = false;
}

struct s_light_ab
{
	short salt;
	word flags;
	long tag_index;
	long scenario_index;
	long stamp;
	byte unknown10[0x24 - 0x10];
	real radius;
	point3f fade_centre;
	real fade_radius;
	byte unknown38[0xd4 - 0x38];
	vector3f colour_a;
	vector3f colour_b;
	byte unknownec[0xf8 - 0xec];
	real fade_f8;
	real fade_fc;
	real fade;
	real fade_104;
	real fade_108;
	byte unknown10c[4];
};

struct s_light_cone_ab
{
	real values[5];
};

struct s_light_shape_ab
{
	long kind;
	union
	{
		struct { real radius_a, radius_b; } sphere;
		s_light_cone_ab cone;
	};
	bool ready;
    byte unknown19[3];
    vector3f left;
    real origin_offset;
    point3f origin;
    union
    {
        struct { real unused, radius_a, radius_b, radius; } sphere_render;
        struct
        {
            real unused;
            vector3f direction;
            real distance;
            real slope_x, slope_y, aspect;
            real width, height;
            real near_distance, far_distance;
            real far_width, far_height;
            point3f endpoint;
        } cone_render;
    };
};

struct s_light_definition_ab
{
	dword flags;
	short kind;
	byte unknown06[0x18 - 6];
	real radius_a;
	real radius_b;
	s_light_cone_ab cone;
	byte unknown34[0x7c - 0x34];
    long bitmap_index;
    byte unknown80[0xb8 - 0x80];
	short fade_distance;
	short fade_colour;
	short fade_range;
};

struct s_light_placement_ab
{
	short palette_index;
	short name_index;
	byte unknown04[0x3c - 4];
	short kind;
	byte unknown3e[0x58 - 0x3e];
	s_light_cone_ab cone;
};

// @retail 0xc1930
void function_c1930(s_light_definition_ab const *definition, s_light_shape_ab *shape)
{
	memset(shape, 0, sizeof(*shape));
	shape->kind = definition->kind;
	if ((short)shape->kind == 0)
	{
		shape->sphere.radius_a = definition->radius_a;
		shape->sphere.radius_b = definition->radius_b;
	}
	else
	{
		shape->cone = definition->cone;
	}
}

// @retail 0xc1980
void function_c1980(s_light_placement_ab const *placement, s_light_definition_ab const *definition, s_light_shape_ab *shape)
{
	memset(shape, 0, sizeof(*shape));
	shape->kind = placement->kind;
	if ((short)shape->kind == 0)
	{
		shape->sphere.radius_a = placement->cone.values[4];
		shape->sphere.radius_b = placement->cone.values[4];
		if (definition->radius_b != 0.0f)
			shape->sphere.radius_b *= definition->radius_b / definition->radius_a;
	}
	else
	{
		shape->cone.values[0] = placement->cone.values[0];
		shape->cone.values[1] = placement->cone.values[1];
		shape->cone.values[2] = placement->cone.values[2];
		shape->cone.values[3] = placement->cone.values[3];
		shape->cone.values[4] = placement->cone.values[4];
	}
}

// @retail 0xc18e0
real function_c18e0()
{
	s_5107e8 *globals = g_5107e8;
	real time = (g_510c54->game_time - globals->time4) * g_510c54->rate;
	if (!(1.0f > time))
		time = 1.0f;
	real result = function_17ca10(time, 5);
	if (!globals->flag0)
		result = 1.0f - result;
	return result;
}

// @retail 0xc3110
bool function_c3110(long index)
{
	s_light_ab *light = (s_light_ab *)g_4e030c->data + (index & 0xffff);
	if (light->stamp != g_4e0308)
	{
		light->stamp = g_4e0308;
		return true;
	}
	return false;
}

static inline real light_colour_magnitude_squared(vector3f const *colour)
{
	return colour->i * colour->i + colour->j * colour->j + colour->k * colour->k;
}

// @retail 0xc3140
bool function_c3140(long index)
{
	s_light_ab *light = (s_light_ab *)g_4e030c->data + (index & 0xffff);
	bool result = false;
	if (light->flags & 2)
	{
		result = light->radius > 0.0001f && light->fade > 0.0001f;
		result &= light_colour_magnitude_squared(&light->colour_a) > 0.05f ||
			light_colour_magnitude_squared(&light->colour_b) > 0.05f;
	}
	return result;
}

struct s_light_scenario_ab
{
	byte unknown00[0xec];
	s_light_placement_ab *lights;
};

// @retail 0xc19f0
void function_c19f0(long light_index, s_light_shape_ab *shape)
{
	s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
	s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    s_light_placement_ab *placement;
    bool from_definition = true;
    if (light->scenario_index != NONE)
    {
        placement = &((s_light_scenario_ab *)g_4e0350)->lights[light->scenario_index];
        if (placement->unknown3e[0] & 1)
            from_definition = false;
    }
    if (from_definition)
        function_c1930(definition, shape);
    else
        function_c1980(placement, definition, shape);
	switch (shape->kind)
	{
	case 1: shape->cone.values[2] = 0.0f; break;
	case 3: shape->cone.values[0] = 0.0f; break;
	}
}

// @retail 0xc38e0
long function_c38e0(short name_index)
{
	long result = NONE;
	if (name_index >= 0 && name_index < 0x280)
	{
		s_record_pool *array = g_4e030c;
		s_light_scenario_ab *scenario = (s_light_scenario_ab *)g_4e0350;
		s_record_pool_iterator iterator;
		iterator.data = array;
		iterator.index = NONE;
		iterator.datum_index = NONE;
		s_light_ab *light;
		while ((light = (s_light_ab *)data_iterator_next_calling(&iterator)) != 0)
		{
			if (light->scenario_index != NONE && scenario->lights[light->scenario_index].name_index == name_index)
			{
				result = iterator.datum_index;
				break;
			}
		}
	}
	return result;
}


static real const g_4405f0[] = { 20.0f, 15.0f, 10.0f, 5.0f, 0.0f };
static real const g_440604[] = { 15.0f, 10.0f, 7.0f, 3.0f, 0.0f };
static real const g_440618[] = { 2.5f, 2.0f, 1.5f, 1.0f, 0.5f };
static real const g_44062c[] = { 2.5f, 2.0f, 1.5f, 1.0f, 0.5f };
static real const g_440640[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

static __forceinline real light_distance_fade_ab(s_light_ab *light, long index, real range, real const *table)
{
    return function_c0a50(&light->fade_centre, index, range, light->fade_radius, table);
}

// @retail 0xc12d0
void function_c12d0(long light_index)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    light->fade = light_distance_fade_ab(light, definition->fade_distance, 10.0f, g_4405f0);
    light->fade_f8 = light_distance_fade_ab(light, definition->fade_colour, 5.0f, g_440604);
    light->fade_fc = light_distance_fade_ab(light, definition->fade_colour, light->fade_radius, g_440618);
    light->fade_104 = light_distance_fade_ab(light, definition->fade_range, 1.0f, g_44062c);
    light->fade_108 = light_distance_fade_ab(light, 0, light->fade_radius * 0.5f, g_440640);
    if (light->fade_f8 > light->fade) light->fade_f8 = light->fade;
    if (light->fade_104 > light->fade_f8) light->fade_104 = light->fade_f8;
}

bool function_31520(long index);

// @retail 0xc3950
long function_c3950(long object_index, long *indices, long maximum)
{
    long count = 0;
    if (object_index != NONE)
    {
        s_record_pool_iterator iterator;
        iterator.data = g_4e030c;
        iterator.index = NONE;
        iterator.datum_index = NONE;
        while (data_iterator_next_inlined(&iterator))
        {
            long index = iterator.datum_index;
            s_light_ab *light = &((s_light_ab *)iterator.data->data)[index & 0xffff];
            if (light->tag_index != NONE)
            {
                s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
                if (function_31520(index) && (definition->flags & 0x20) && (definition->flags & 0x40000) && count < maximum)
                    indices[count++] = index;
            }
        }
    }
    return count;
}


struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);
real function_30bf0(vector3f *vector);

// @retail 0xc0840
bool function_c0840(point3f const *start, point3f const *end, point3f *out, real *distance)
{
    vector3f direction;
    direction.i = (real)((double)end->x - start->x);
    direction.j = (real)((double)end->y - start->y);
    direction.k = (real)((double)end->z - start->z);
    bool result = false;
    real travelled = 0.0f;
    real step = 0.001f;
    real length = function_30bf0(&direction);
    if (length > 0.0f)
    {
        real x = direction.i;
        real y = direction.j;
        real z = direction.k;
        s_bsp3d *bsp = g_4e033c;
        short bsp_index = g_4686c4;
        s_match_globals *structure = g_4e0348;
        while (length > travelled)
        {
            point3f point;
            point.x = x * travelled + start->x;
            point.y = y * travelled + start->y;
            point.z = z * travelled + start->z;
            if (bsp_index != NONE)
            {
                long leaf = function_14a280(bsp, 0, &point);
                if (leaf != NONE && *(short *)(*(byte **)((byte *)structure + 0x30) + leaf * 8) != NONE)
                {
                    *distance = travelled;
                    *out = point;
                    return true;
                }
            }
            travelled += step;
            step *= 2.0f;
            if (step > 0.1f) step = 0.1f;
        }
    }
    return result;
}


struct s_tag_data;
real function_13b390(void const *function, real input, real range);
real function_13bb90(s_tag_data const *function, real input, real range);
dword function_13bc00(s_tag_data const *function, real input);

struct s_light_animation_ab
{
    dword flags;
    long intensity_count;
    byte *intensity;
    long colour_count;
    byte *colour;
    long pair_count;
    byte *pair;
};

#pragma inline_depth(0)
// @retail 0xc0b00
void function_c0b00(s_light_animation_ab const *animation, long seed, real range, real *intensity, vector3f *colour, real *pair)
{
    real time;
    if (animation->flags & 1)
        time = g_510c54->game_time * g_510c54->rate;
    else
        time = (((seed * 0x1387) & 0x7fffffff) % (60 * g_510c54->field_2_3) + g_510c54->game_time) * g_510c54->rate;
    if (animation->intensity_count > 0)
    {
        byte *function = animation->intensity;
        real value = function_13b390(function, time, range);
        byte *data = *(byte **)(function + 4);
        if (!(data[1] & 0xf0))
        {
            real low = *(real *)(data + 4);
            real high = *(real *)(data + 8);
            if (0.0f > value) value = 0.0f;
            else if (value > 1.0f) value = 1.0f;
            value = low + (high - low) * value;
        }
        *intensity *= value;
    }
    if (animation->colour_count > 0)
    {
        s_tag_data *function = (s_tag_data *)animation->colour;
        dword value = function_13bc00(function, function_13b390(function, time, range));
        colour->i = (real)(((value >> 16) & 0xff) * colour->i * (1.0f / 255.0f));
        colour->j = (real)(((value >> 8) & 0xff) * colour->j * (1.0f / 255.0f));
        colour->k = (real)((value & 0xff) * colour->k * (1.0f / 255.0f));
    }
    if (animation->pair_count > 0)
    {
        pair[0] = function_13bb90((s_tag_data *)animation->pair, time, range);
        pair[1] = function_13bb90((s_tag_data *)(animation->pair + 8), time, range);
    }
}
#pragma inline_depth()


// @retail 0xc15a0
void function_c15a0()
{
    s_record_pool *array = g_4e030c;
    long index = data_datum_index(array, function_16bc00(array, 0));
    while (index != NONE)
    {
        s_light_ab *light = &((s_light_ab *)array->data)[index & 0xffff];
        if (light->flags & 2)
            function_c12d0(index);
        index = data_datum_index(array, data_find_index(array, index == NONE ? 0 : (index & 0xffff) + 1));
    }
}

struct s_light_frame_ab
{
    point3f position;
    point3f endpoint;
    real clip_distance;
    point3f field_1c_3;
    vector3f forward;
    vector3f up;
    real radius;
};
extern point2f *g_468774;

// @retail 0xc1a80
void function_c1a80(s_light_frame_ab const *frame, s_light_shape_ab *shape, s_light_definition_ab const *definition)
{
    if ((short)shape->kind == 0)
    {
        shape->sphere_render.radius_a = shape->sphere.radius_a * frame->radius;
        shape->sphere_render.radius_b = shape->sphere.radius_b * frame->radius;
        shape->sphere_render.radius = shape->sphere_render.radius_a > shape->sphere_render.radius_b ? shape->sphere_render.radius_a : shape->sphere_render.radius_b;
    }
    else
    {
        vector3f *direction = &shape->cone_render.direction;
        direction->i = frame->endpoint.x - frame->position.x;
        direction->j = frame->endpoint.y - frame->position.y;
        direction->k = frame->endpoint.z - frame->position.z;
        shape->cone_render.distance = frame->forward.k * direction->k + frame->forward.j * direction->j + frame->forward.i * direction->i;
        if (fabs(shape->cone_render.distance) < 0.0001f || shape->cone_render.distance < 0.0f)
        {
            *direction = frame->forward;
            shape->cone_render.distance = 1.0f;
        }
        real scale = 1.0f / shape->cone_render.distance;
        direction->i *= scale;
        direction->j *= scale;
        direction->k *= scale;
        shape->cone_render.aspect = shape->cone.values[1];
        if (definition->bitmap_index != NONE)
        {
            byte *bitmap = g_4e3b44[definition->bitmap_index & 0xffff].bytes;
            byte *image = *(byte **)(bitmap + 0x48);
            shape->cone_render.aspect = (real)*(short *)(image + 6) / (real)*(short *)(image + 4) * shape->cone_render.aspect;
        }
        if (shape->kind == 1)
        {
            shape->cone_render.slope_x = g_468774->x;
            shape->cone_render.slope_y = g_468774->y;
        }
        else
        {
            double slope = tan(shape->cone.values[2] * 0.5f);
            shape->cone_render.slope_x = (real)slope;
            shape->cone_render.slope_y = (real)(slope * shape->cone_render.aspect);
        }
        shape->cone_render.width = shape->cone.values[0];
        shape->cone_render.height = shape->cone_render.aspect * shape->cone.values[0];
        shape->cone_render.far_distance = shape->cone.values[4] * frame->radius;
        shape->cone_render.near_distance = shape->cone.values[3] * frame->radius;
        shape->cone_render.far_width = shape->cone_render.slope_x * shape->cone_render.far_distance + shape->cone_render.width;
        shape->cone_render.far_height = shape->cone_render.slope_y * shape->cone_render.far_distance + shape->cone_render.height;
        real distance = shape->cone_render.far_distance;
        shape->cone_render.endpoint.x = direction->i * distance + frame->position.x;
        shape->cone_render.endpoint.y = direction->j * distance + frame->position.y;
        shape->cone_render.endpoint.z = direction->k * distance + frame->position.z;
    }
    real x = frame->forward.j * frame->up.k - frame->forward.k * frame->up.j;
    real y = frame->forward.k * frame->up.i - frame->forward.i * frame->up.k;
    real z = frame->forward.i * frame->up.j - frame->forward.j * frame->up.i;
    shape->left.j = y;
    shape->left.k = z;
    shape->left.i = x;
    shape->origin_offset = 0.0f;
    shape->origin = frame->position;
    if (shape->kind == 2 && shape->cone_render.slope_x > 0.0001f)
    {
        shape->origin_offset = shape->cone.values[0] / shape->cone_render.slope_x * 0.5f;
        real distance = 0.0f - shape->origin_offset;
        shape->origin.x = shape->cone_render.direction.i * distance + frame->position.x;
        shape->origin.y = shape->cone_render.direction.j * distance + frame->position.y;
        shape->origin.z = shape->cone_render.direction.k * distance + frame->position.z;
    }
    shape->ready = true;
}

// @retail 0xc17f0
bool function_c17f0(long light_index, s_light_shape_ab *shape, bool respect_engine)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    bool enabled = true;
    if (!(definition->flags & 0x100) && respect_engine)
    {
        s_mp_globals *globals = g_4e9ae8;
        if (g_55e4d0[globals->engine_index])
            enabled = !(bool)((*(dword *)globals >> 1) & 1);
    }
    if (g_5107e8->flag8 && enabled && (light->flags & 2))
    {
        function_c19f0(light_index, shape);
        function_c1a80((s_light_frame_ab *)((byte *)light + 0x84), shape, definition);
        if ((short)shape->kind == 0)
        {
            if (shape->sphere_render.radius > 0.0001f) return true;
        }
        else if (shape->cone_render.far_distance > 0.0001f &&
            shape->cone_render.far_width * shape->cone_render.far_width + shape->cone_render.far_height * shape->cone_render.far_height > 0.0001f * 0.0001f)
            return true;
    }
    return false;
}

// @retail 0xc35a0
bool function_c35a0(long light_index, point3f *corners)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_frame_ab *frame = (s_light_frame_ab *)((byte *)light + 0x84);
    s_light_shape_ab shape;
    bool result = false;
    if (function_c17f0(light_index, &shape, true))
    {
        real far_width = shape.cone_render.slope_x * shape.cone.values[4] * 2.0f + shape.cone_render.width;
        real far_height = shape.cone_render.aspect * far_width;
        vector3f direction = shape.cone_render.direction;
        function_30bf0(&direction);
        point3f near_centre, far_centre;
        near_centre.x = frame->position.x + direction.i * 0.0f;
        near_centre.y = frame->position.y + direction.j * 0.0f;
        near_centre.z = frame->position.z + direction.k * 0.0f;
        far_centre.x = frame->position.x + direction.i * shape.cone.values[4];
        far_centre.y = frame->position.y + direction.j * shape.cone.values[4];
        far_centre.z = frame->position.z + direction.k * shape.cone.values[4];
        point3f far_points[4], near_points[4];
        long order[4] = {0, 1, 3, 2};
        for (long i = 0; i < 4; i++)
        {
            real x = ((order[i] & 1) ? 1.0f : -1.0f) * 0.5f;
            real y = ((order[i] & 2) ? 1.0f : -1.0f) * 0.5f;
            far_points[i].x = far_centre.x;
            far_points[i].y = far_centre.y;
            far_points[i].z = far_centre.z;
            near_points[i].x = near_centre.x;
            near_points[i].y = near_centre.y;
            near_points[i].z = near_centre.z;
            real near_x = x * shape.cone_render.width;
            real far_x = x * far_width;
            near_points[i].x += shape.left.i * near_x;
            near_points[i].y += shape.left.j * near_x;
            near_points[i].z += shape.left.k * near_x;
            far_points[i].x += shape.left.i * far_x;
            far_points[i].y += shape.left.j * far_x;
            far_points[i].z += shape.left.k * far_x;
            real near_y = y * shape.cone_render.height;
            real far_y = y * far_height;
            near_points[i].x += frame->up.i * near_y;
            near_points[i].y += frame->up.j * near_y;
            near_points[i].z += frame->up.k * near_y;
            far_points[i].x += frame->up.i * far_y;
            far_points[i].y += frame->up.j * far_y;
            far_points[i].z += frame->up.k * far_y;
        }
        memcpy(corners, near_points, sizeof(near_points));
        memcpy(corners + 4, far_points, sizeof(far_points));
        result = true;
    }
    return result;
}


void function_1df080(point3f const *points, long count, point3f *center, real *radius);

// @retail 0xc2c50
void function_c2c50(long light_index)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_shape_ab shape;
    function_c19f0(light_index, &shape);
    if ((short)shape.kind == 0)
    {
        light->fade_centre = *(point3f *)((byte *)light + 0x18);
        light->fade_radius = light->radius;
    }
    else
    {
        point3f corners[8];
        if (function_c35a0(light_index, corners))
            function_1df080(corners, 8, &light->fade_centre, &light->fade_radius);
        else
        {
            light->fade_radius = 0.0f;
            light->fade_centre = *g_468788;
        }
    }
}

#include "object_markers.h"

struct s_first_person_marker;
short first_person_weapon_get_markers(long weapon_index, long marker_name, s_first_person_marker *markers, short count);
bool function_3e9c0(long object_index);
long function_b8c40(long object_index, short entry_index);
bool function_2dba0(long tag, vector3f const *direction, long kind, long index, long marker_index, point3f const *position, color3f const *color, real alpha, real amount, real scale, bool alternate);
void function_42850(long object_index, long tag_index, point3f const *position,
    vector3f const *first, vector3f const *second, real scale, real width,
    vector3f const *third);
byte g_55e708;

// @retail 0xc3340
void __stdcall function_c3340(long light_index, long unused)
{
    long absolute_index = light_index & 0xffff;
    byte *light = g_4e030c->data + absolute_index * 0x110;
    byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
    if (function_31520(light_index) && (light[2] & 2))
    {
        if (*(long *)(definition + 0x94) != NONE)
        {
            if (*(short *)(light + 0x54) != NONE)
            {
                long object_index = *(long *)(light + 0x4c);
                byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
                long marker_name = function_b8c40(object_index, *(short *)(light + 0x54));
                s_object_marker markers[2];
                long count = 0;
                if (object[0xaa] == 2 && *(long *)(object + 0x14) != NONE)
                    count = first_person_weapon_get_markers(*(long *)(light + 0x4c), marker_name,
                        (s_first_person_marker *)markers, 1);
                if (!count)
                    count = function_b8d30(*(long *)(light + 0x4c), marker_name, markers, 2, false);
                if (count > 1)
                {
                    if (!g_55e708)
                        g_55e708 = 1;
                    count = 1;
                }
                for (long i = 0; i < count; i++)
                    function_2dba0(*(long *)(definition + 0x94), &markers[i].matrix.forward, 2, absolute_index, i, &markers[i].matrix.position, (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f, function_3e9c0(*(long *)(light + 0x4c)));
            }
            else
                function_2dba0(*(long *)(definition + 0x94), (vector3f *)(light + 0xac), 2, absolute_index, 0, (point3f *)(light + 0x84), (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f, function_3e9c0(*(long *)(light + 0x4c)));
        }
        if ((light[2] & 2) && *(long *)(definition + 0xa0) != NONE)
        {
            real opacity = (1.0f - *(real *)(light + 0xc8)) * *(real *)(light + 0xd0);
            opacity = opacity < 0.0f ? 0.0f : opacity > 1.0f ? 1.0f : opacity;
            function_42850(*(long *)(light + 0x4c), *(long *)(definition + 0xa0),
                (point3f *)(light + 0x84), (vector3f *)(light + 0xac), (vector3f *)(light + 0xb8),
                1.0f, opacity, (vector3f *)((byte *)g_4686cc + 4));
        }
    }
}

// @retail 0xc28b0
void function_c28b0(long light_index)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    s_light_shape_ab shape;
    function_c19f0(light_index, &shape);
    s_light_frame_ab *frame = (s_light_frame_ab *)((byte *)light + 0x84);
    point3f *centre = (point3f *)((byte *)light + 0x18);
    if ((short)shape.kind == 0)
    {
        real radius = (definition->flags & 2) ? shape.sphere.radius_a :
            (shape.sphere.radius_a > shape.sphere.radius_b ? shape.sphere.radius_a : shape.sphere.radius_b);
        radius *= *(real *)((byte *)definition + 0xc);
        real minimum = *(real *)((byte *)definition + 0x98);
        radius = radius > minimum ? radius : minimum;
        if (frame->clip_distance > 0.0f)
        {
            *centre = frame->field_1c_3;
            light->radius = frame->clip_distance + radius;
        }
        else
        {
            *centre = frame->position;
            light->radius = radius;
        }
        *(point3f *)((byte *)light + 0x38) = *centre;
        light->fade_centre = *centre;
        light->fade_radius = light->radius;
    }
    else
    {
        function_c1a80(frame, &shape, definition);
        real width = shape.cone_render.slope_x * shape.cone.values[4] * 2.0f + shape.cone_render.width;
        real height = shape.cone_render.aspect * width;
        vector3f *direction = &shape.cone_render.direction;
        real side = direction->k * shape.left.k + direction->j * shape.left.j + direction->i * shape.left.i;
        real side_sign = side < 0.0f ? -1.0f : 1.0f;
        real up = direction->k * frame->up.k + direction->j * frame->up.j + direction->i * frame->up.i;
        real up_sign = up < 0.0f ? -1.0f : 1.0f;
        vector3f unit = *direction;
        function_30bf0(&unit);
        point3f corner;
        corner.x = g_468788->x + unit.i * shape.cone.values[4];
        corner.y = g_468788->y + unit.j * shape.cone.values[4];
        corner.z = g_468788->z + unit.k * shape.cone.values[4];
        real side_extent = side_sign * width * 0.5f;
        corner.x += shape.left.i * side_extent;
        corner.y += shape.left.j * side_extent;
        corner.z += shape.left.k * side_extent;
        real up_extent = up_sign * height * 0.5f;
        corner.x += frame->up.i * up_extent;
        corner.y += frame->up.j * up_extent;
        corner.z += frame->up.k * up_extent;
        real radius = (real)(sqrt((double)corner.z * corner.z + (double)corner.y * corner.y + (double)corner.x * corner.x) * *(real *)((byte *)definition + 0xc));
        real minimum = *(real *)((byte *)definition + 0x98);
        light->radius = radius > minimum ? radius : minimum;
        *centre = frame->clip_distance > 0.0f ? frame->field_1c_3 : frame->position;
        *(point3f *)((byte *)light + 0x38) = *centre;
    }
    function_c2c50(light_index);
}


struct s_light_projection_ab
{
    bool orthogonal;
    byte unknown01[3];
    transform4x3f inverse;
    transform4x3f matrix;
    bool has_plane;
    byte unknown6d[3];
    real plane_offset;
    vector3f plane_normal;
    real plane_distance;
    bool flag84, flag85;
    byte unknown86[2];
    real depth;
    bool flag8c;
    byte unknown8d[3];
    byte frustum[0x108];
    long point_count;
    point2f points[4];
};
struct s_frustum_1648d0;
struct s_camera_163db0;
extern box2f *g_4687dc;
void function_11f5f0(box2f *bounds, point2f const *points, long count);
void function_141590(transform4x3f const *in, transform4x3f *out);
bool function_163db0(s_camera_163db0 const *camera, box2f const *rectangle, long identifier, s_frustum_1648d0 *result);

PRIVATE inline void light_projection_cross_ab(vector3f const *a, vector3f const *b, vector3f *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
}

PRIVATE inline void light_projection_point_ab(transform4x3f const *matrix, vector3f const *point, point2f *result)
{
    real vi = point->i;
    real vj = point->j;
    real vk = point->k;
    if (matrix->scale != 1.0f)
    {
        real scale = 1.0f / matrix->scale;
        vi *= scale;
        vj *= scale;
        vk *= scale;
    }
    real x = matrix->forward.k * vk + matrix->forward.j * vj + matrix->forward.i * vi;
    real y = matrix->left.k * vk + matrix->left.i * vi + matrix->left.j * vj;
    real z = matrix->up.k * vk + matrix->up.i * vi + matrix->up.j * vj;
    real scale = -1.0f / z;
    result->x = scale * x;
    result->y = scale * y;
}

// @retail 0xc1d90
void __stdcall function_c1d90(s_light_frame_ab const *frame, s_light_shape_ab const *shape, s_light_projection_ab *entries, short *count)
{
    *count = (short)shape->kind == 0 ? 6 : 1;
    for (long i = 0; i < *count; i++)
    {
        box2f bounds = *g_4687dc;
        s_light_projection_ab *entry = &entries[i];
        memset(entry, 0, sizeof(*entry));
        if ((short)shape->kind == 0)
        {
            real radius = shape->sphere_render.radius;
            vector3f vertices[8] = {
                { radius, radius, -radius }, { -radius, radius, -radius },
                { -radius, -radius, -radius }, { radius, -radius, -radius },
                { radius, radius, radius }, { -radius, radius, radius },
                { -radius, -radius, radius }, { radius, -radius, radius }
            };
            vector3f directions[6] = {
                { 0, 0, -1 }, { 1, 0, 0 }, { 0, 1, 0 },
                { -1, 0, 0 }, { 0, -1, 0 }, { 0, 0, 1 }
            };
            long faces[6][4] = {
                { 0, 1, 2, 3 }, { 0, 3, 7, 4 }, { 0, 4, 5, 1 },
                { 1, 5, 6, 2 }, { 2, 6, 7, 3 }, { 4, 7, 6, 5 }
            };
            entry->depth = radius;
            entry->orthogonal = false;
            entry->has_plane = false;
            entry->matrix.up.i = 0.0f - directions[i].i;
            entry->matrix.up.j = 0.0f - directions[i].j;
            entry->matrix.up.k = 0.0f - directions[i].k;
            vector3f reference = *g_4687a8;
            if (i == 1 || i == 3)
                reference = *g_4687ac;
            light_projection_cross_ab(&entry->matrix.up, &reference, &entry->matrix.left);
            vector3f *up = &entry->matrix.left;
            real length = (real)sqrt((double)up->i * up->i + (double)up->k * up->k + (double)up->j * up->j);
            if (fabs(length) >= 0.0001f)
            {
                real scale = 1.0f / length;
                up->i *= scale;
                up->j *= scale;
                up->k *= scale;
            }
            light_projection_cross_ab(&entry->matrix.left, &entry->matrix.up, &entry->matrix.forward);
            entry->matrix.position = shape->origin;
            entry->matrix.scale = 1.0f;
            light_projection_point_ab(&entry->matrix, &vertices[faces[i][0]], &entry->points[0]);
            light_projection_point_ab(&entry->matrix, &vertices[faces[i][1]], &entry->points[1]);
            light_projection_point_ab(&entry->matrix, &vertices[faces[i][2]], &entry->points[2]);
            light_projection_point_ab(&entry->matrix, &vertices[faces[i][3]], &entry->points[3]);
            entry->point_count = 4;
            function_11f5f0(&bounds, entry->points, entry->point_count);
        }
        else
        {
            *count = 1;
            memset(entries, 0, sizeof(*entries));
            entry = entries;
            entry->matrix.forward = shape->left;
            entry->matrix.left = frame->up;
            entry->matrix.up.i = 0.0f - frame->forward.i;
            entry->matrix.up.j = 0.0f - frame->forward.j;
            entry->matrix.up.k = 0.0f - frame->forward.k;
            entry->matrix.position = shape->origin;
            entry->matrix.scale = 1.0f;
            entry->orthogonal = shape->kind == 1;
            entry->has_plane = shape->origin_offset > 0.0f;
            entry->plane_offset = shape->origin_offset;
            point3f plane_point;
            plane_point.x = entry->matrix.position.x + frame->forward.i * entry->plane_offset;
            plane_point.y = entry->matrix.position.y + frame->forward.j * entry->plane_offset;
            plane_point.z = entry->matrix.position.z + frame->forward.k * entry->plane_offset;
            entry->plane_normal = frame->forward;
            entry->plane_distance = entry->plane_normal.k * plane_point.z + entry->plane_normal.j * plane_point.y + entry->plane_normal.i * plane_point.x;
            entry->depth = shape->cone_render.far_distance + shape->origin_offset;
            point2f center;
            light_projection_point_ab(&entry->matrix, &shape->cone_render.direction, &center);
            real scale = 1.0f / shape->origin_offset;
            real half_width = shape->cone_render.width * scale * 0.5f;
            real half_height = shape->cone_render.height * scale * 0.5f;
            bounds.x0 = center.x - half_width;
            bounds.x1 = center.x + half_width;
            bounds.y0 = center.y - half_height;
            bounds.y1 = center.y + half_height;
            entry->points[0].x = bounds.x0; entry->points[0].y = bounds.y0;
            entry->points[1].x = bounds.x1; entry->points[1].y = bounds.y0;
            entry->points[2].x = bounds.x1; entry->points[2].y = bounds.y1;
            entry->points[3].x = bounds.x0; entry->points[3].y = bounds.y1;
            entry->point_count = 4;
        }
        entry->flag8c = true;
        entry->flag84 = true;
        entry->flag85 = false;
        function_141590(&entry->matrix, &entry->inverse);
        function_163db0((s_camera_163db0 *)entry, &bounds, 0, (s_frustum_1648d0 *)entry->frustum);
    }
}

struct s_light_delete_view
{
    short identifier;
    byte flags;
    byte unknown03[0x10 - 3];
    long first_cluster_reference;
    byte unknown14[0x58 - 0x14];
    long object_index;
    byte unknown5c[0x110 - 0x5c];
};

struct s_cluster_partition
{
    long *cluster_first_data_references;
    s_record_pool *data_references;
    s_record_pool *cluster_references;
};

void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference);

// @retail 0xc3220
void function_c3220(long light_index)
{
    s_light_delete_view *light = &((s_light_delete_view *)g_4e030c->data)[light_index & 0xffff];
    if (light->flags & 2)
    {
        s_cluster_partition partition =
        {
            (long *)g_4e0310,
            (s_record_pool *)g_4e0314,
            (s_record_pool *)g_4e0318
        };
        function_1cae40(&partition, light_index, &light->first_cluster_reference);
        light->flags &= ~8;
    }
}

// @retail 0xc0110
void function_c0110()
{
    s_record_pool_iterator iterator;
    iterator.data = g_4e030c;
    iterator.index = NONE;
    iterator.datum_index = NONE;
    byte *light;
    while ((light = data_iterator_next_inlined(&iterator)) != 0)
    {
        if (*(long *)(light + 0x4c) == NONE && *(long *)(light + 0x58) == NONE && (light[2] & 8))
            function_c3220(iterator.datum_index);
    }
}

// @retail 0xc3260
void __stdcall function_c3260(long light_index, bool clear_object_flag)
{
    long const *light_index_reference = &light_index;
    bool const *clear_flag_reference = &clear_object_flag;
    s_record_pool *lights = g_4e030c;
    s_light_delete_view *light = &((s_light_delete_view *)lights->data)[*light_index_reference & 0xffff];
    if ((light->flags & 8) && (light->flags & 2))
    {
        s_cluster_partition partition =
        {
            (long *)g_4e0310,
            (s_record_pool *)g_4e0314,
            (s_record_pool *)g_4e0318
        };
        function_1cae40(&partition, *light_index_reference, &light->first_cluster_reference);
        light->flags &= ~8;
    }
    long object_index = light->object_index;
    if (object_index != NONE && *clear_flag_reference)
    {
        struct s_object_header { byte unknown00[8]; byte *object; };
        byte *object = ((s_object_header *)g_4e0300->data)[object_index & 0xffff].object;
        bool remaining = false;
        long next = NONE;
        while ((next = function_16bc00(lights, next + 1)) != NONE)
        {
            long size = *(long volatile *)&lights->size;
            byte *data = *(byte *volatile *)&lights->data;
            s_light_delete_view *entry = (s_light_delete_view *)(data + next * size);
            long index = ((long)entry->identifier << 16) | next;
            if (index != *light_index_reference && entry->object_index == object_index)
            {
                remaining = true;
                break;
            }
        }
        if (remaining)
            *(dword *)(object + 4) |= 0x40;
        else
            *(dword *)(object + 4) &= ~0x40;
    }
    record_pool_release(lights, *light_index_reference);
}

#include "object_queries.h"
#include "object_markers.h"
struct s_partition_location;
void function_1cac60(dword const *bits, s_cluster_partition *partition, long data_index,
    long *first_cluster_reference, point3f const *point, real radius, s_partition_location const *location,
    long payload_size, void const *payload, bool *overflow);
bool first_person_weapon_get_marker(long object_index, long marker_name, point3f *position, vector3f *forward, vector3f *up);
bool __stdcall function_16a8e0(long flags, point3f const *point, real radius, long a, long b, point3f *out, real *distance);
void function_11bed0(s_location *location, point3f const *point);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);

// @retail 0xc2d00
void __stdcall function_c2d00(long light_index)
{
    union { s_object_marker marker; s_cluster_partition partition; } scratch;
    byte *light = g_4e030c->data + (light_index & 0xffff) * 0x110;
    byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
    if (!(*(word *)(light + 2) & 2))
        return;
    if (*(word *)(light + 2) & 4)
        function_c0840((point3f *)(light + 0x84), (point3f *)(light + 0x90),
            (point3f *)(light + 0xa0), (real *)(light + 0x9c));
    if (*(short *)(light + 0x54) != NONE)
    {
        long marker_name = function_b8c40(*(long *)(light + 0x4c), *(short *)(light + 0x54));
        bool in_own_view = (*(long *)definition & 0x20) &&
            first_person_weapon_get_marker(*(long *)(light + 0x4c), marker_name,
                (point3f *)(light + 0x84), (vector3f *)(light + 0xac), (vector3f *)(light + 0xb8));
        if (!in_own_view)
        {
            function_b8d30(*(long *)(light + 0x4c), marker_name, &scratch.marker, 1, false);
            *(point3f *)(light + 0x84) = scratch.marker.matrix.position;
            *(vector3f *)(light + 0xac) = scratch.marker.matrix.forward;
            *(vector3f *)(light + 0xb8) = scratch.marker.matrix.up;
        }
        *(real *)(light + 0x90) = *(real *)(light + 0x84) + *(real *)(light + 0xac);
        *(real *)(light + 0x94) = *(real *)(light + 0x88) + *(real *)(light + 0xb0);
        *(real *)(light + 0x98) = *(real *)(light + 0x8c) + *(real *)(light + 0xb4);
    }
    else if (*(long *)(light + 0x58) != NONE)
    {
        struct s_header { byte unknown00[8]; byte *object; };
        byte *object = ((s_header *)g_4e0300->data)[*(long *)(light + 0x58) & 0xffff].object;
        transform4x3f *matrix = (transform4x3f *)(object + *(short *)(object + 0x116)) + *(short *)(light + 0x5c);
        transform4x3f_apply_point(matrix, (point3f *)(light + 0x60), (point3f *)(light + 0x84));
        vector3f *forward = (vector3f *)(light + 0x6c);
        vector3f *up = (vector3f *)(light + 0x78);
        vector3f *world_forward = (vector3f *)(light + 0xac);
        vector3f *world_up = (vector3f *)(light + 0xb8);
        world_forward->i = matrix->up.i * forward->k + matrix->left.i * forward->j + matrix->forward.i * forward->i;
        world_forward->j = matrix->forward.j * forward->i + matrix->up.j * forward->k + matrix->left.j * forward->j;
        world_forward->k = matrix->forward.k * forward->i + matrix->up.k * forward->k + matrix->left.k * forward->j;
        world_up->i = matrix->up.i * up->k + matrix->left.i * up->j + matrix->forward.i * up->i;
        world_up->j = matrix->forward.j * up->i + matrix->up.j * up->k + matrix->left.j * up->j;
        world_up->k = matrix->forward.k * up->i + matrix->up.k * up->k + matrix->left.k * up->j;
        *(real *)(light + 0x90) = *(real *)(light + 0x84) + world_forward->i;
        *(real *)(light + 0x94) = *(real *)(light + 0x88) + world_forward->j;
        *(real *)(light + 0x98) = *(real *)(light + 0x8c) + world_forward->k;
    }
    function_c28b0(light_index);
    if (!(light[2] & 8) && *(short *)(light + 0x48) == NONE &&
        (*(long *)definition & 0x8000) && *(real *)(light + 0x24) > 0.0f)
    {
        real distance;
        function_16a8e0(0xd800001, (point3f *)(light + 0x18), *(real *)(light + 0x24),
            light_index, NONE, (point3f *)(light + 0x18), &distance);
        *(point3f *)(light + 0x38) = *(point3f *)(light + 0x18);
    }
    s_location *location = (s_location *)(light + 0x44);
    function_11bed0(location, (point3f *)(light + 0x38));
    if (*(short *)(light + 0x48) == NONE && (!(light[2] & 1) || (*(long *)definition & 8)))
    {
        if (*(long *)(light + 0x4c) != NONE)
            object_get_root_location(*(long *)(light + 0x4c), location);
        else if (*(long *)(light + 0x58) != NONE)
            object_get_root_location(*(long *)(light + 0x58), location);
        else
            function_11bed0(location, (point3f *)(light + 0x18));
    }
    bool overflow;
    scratch.partition.cluster_first_data_references = (long *)g_4e0310;
    scratch.partition.data_references = (s_record_pool *)g_4e0314;
    scratch.partition.cluster_references = (s_record_pool *)g_4e0318;
    function_1cac60(NULL, &scratch.partition, light_index, (long *)(light + 0x10), (point3f *)(light + 0x18),
        *(real *)(light + 0x24), (s_partition_location *)location, 0, NULL, &overflow);
    light[2] |= 8;
}

#include "data_array.h"

void function_141ce0(real a, real b, real c, transform4x3f *out);
real function_30bf0(vector3f *vector);
void __stdcall function_c0c80(long light_index);

// @retail 0xc0520
long __stdcall function_c0520(long tag_index, long placement_index, bool force,
    s_light_placement_ab *placement)
{
    long result_value = 0;
    long const *placement_index_reference = &placement_index;
    bool const *force_reference = &force;
    s_light_placement_ab *const *placement_reference = &placement;
    byte const *source = (byte const *)*placement_reference;
    byte const *definition = g_4e3b44[tag_index & 0xffff].bytes;
    long result = NONE;
    bool create = true;
    if ((definition[0] & 1) || (*(short const *)(source + 0x40) == 3 && !*force_reference))
        create = false;
    if (*(long const *)(definition + 0x94) != NONE || create)
    {
        result = record_pool_allocate(g_4e030c);
        if (result != NONE)
        {
            byte *light = g_4e030c->data + (result & 0xffff) * 0x110;
            *(long *)(light + 8) = *placement_index_reference;
            *(long *)(light + 0x14) = NONE;
            *(long *)(light + 0x4c) = NONE;
            *(short *)(light + 0x54) = NONE;
            *(long *)(light + 0x58) = NONE;
            *(short *)(light + 0x5c) = NONE;
            *(long *)(light + 0x50) = 0;
            *(short *)(light + 0x56) = 0;
            *(word *)(light + 2) = 5;
            *(long *)(light + 4) = tag_index;
            *(real *)(light + 0xd0) = 1.0f;
            *(real *)(light + 0xcc) = 1.0f;
            *(long *)(light + 0x10) = NONE;
            *(word *)(light + 2) = ((word)(source[0x3e] & 4) << 3) | 5;
            s_light_shape_ab shape;
            function_c19f0(result, &shape);
            transform4x3f matrix;
            function_141ce0(*(real const *)(source + 0x14), *(real const *)(source + 0x18),
                *(real const *)(source + 0x1c), &matrix);
            point3f *position = (point3f *)(light + 0x84);
            *position = *(point3f const *)(source + 8);
            vector3f *forward = (vector3f *)(light + 0xac);
            *forward = matrix.up;
            *(vector3f *)(light + 0xb8) = matrix.left;
            vector3f direction;
            direction.i = *(real const *)(source + 0x4c) - *(real const *)(source + 8);
            direction.j = *(real const *)(source + 0x50) - ((point3f const *)(source + 8))->y;
            direction.k = *(real const *)(source + 0x54) - ((point3f const *)(source + 8))->z;
            real length = function_30bf0(&direction);
            if (!(length != 0.0f))
                direction = matrix.up;
            else
            {
                if ((short)shape.kind != 0 && forward->k * direction.k +
                    forward->j * direction.j + forward->i * direction.i < 0.5f)
                    direction = matrix.up;
            }
            real distance;
            if ((short)shape.kind != 0)
                distance = (forward->k * direction.k + forward->j * direction.j +
                    forward->i * direction.i) * shape.cone.values[4];
            else
                distance = shape.sphere.radius_a > shape.sphere.radius_b ? shape.sphere.radius_a : shape.sphere.radius_b;
            *(real *)(light + 0x90) = direction.i * distance + position->x;
            *(real *)(light + 0x94) = direction.j * distance + position->y;
            *(real *)(light + 0x98) = direction.k * distance + position->z;
            if (!(definition[0] & 9))
            {
                position->x = direction.i * 0.001f + position->x;
                position->y = direction.j * 0.001f + position->y;
                position->z = direction.k * 0.001f + position->z;
            }
            function_c0c80(result);
            *(long *)(light + 0xc) = g_4e0308 - 1;
            *(long *)(light + 0x10c) = NONE;
        }
    }
    { result_value = result; goto return_exit; }

return_exit:
    return result_value;
}

struct s_light_palette_setup_ab
{
    byte unknown00[4];
    long tag_index;
    byte unknown08[0x20];
};

struct s_light_setup_scenario_ab
{
    byte unknown00[0xe8];
    long placement_count;
    s_light_placement_ab *placements;
    long palette_count;
    s_light_palette_setup_ab *palette;
};

// @retail 0xc09c0
void function_c09c0(long mode)
{
    s_light_setup_scenario_ab *scenario = (s_light_setup_scenario_ab *)g_4e0350;
    long force = mode == 5 || mode == 4;
    long const *force_reference = &force;
    long *count = &scenario->placement_count;
    for (long i = 0; i < *count; ++i)
    {
        s_light_placement_ab *placement = &scenario->placements[i];
        long palette_index = placement->palette_index;
        if (palette_index != NONE && palette_index >= 0 && palette_index < scenario->palette_count)
        {
            long tag_index = scenario->palette[palette_index].tag_index;
            if (tag_index != NONE)
                function_c0520(tag_index, i, *force_reference, placement);
        }
        scenario = (s_light_setup_scenario_ab *)g_4e0350;
    }
}


bool __stdcall function_bab40(long object_index, long name, real *value);
bool function_bad50(long object_index, long index, point3f *out);
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);
color3f *function_131d60(color4f const *a, color4f const *b, dword flags, real t,
    color3f const *tint, color3f *result);
long function_baf80(long object_index);
bool object_or_parent_hidden(long object_index);
bool function_b9d20(long object_index);
struct s_object_list;
extern s_object_list *g_4de2f4;

PRIVATE __forceinline real clamp_light(real x, real low, real high)
{
    if (low > x) return low;
    if (x > high) return high;
    return x;
}

// @retail 0xc0c80
void __stdcall function_c0c80(long light_index)
{
    struct s_light_dynamic
    {
        short salt;
        word flags;
        long tag_index;
        byte unknown08[0x14 - 8];
        long start_time;
        byte unknown18[0x4c - 0x18];
        long owner;
        long name;
        short function_index, colour_index;
        long parent;
        byte unknown5c[0xc4 - 0x5c];
        real radius, fade, scale, fraction;
        color3f colour, pair_colour;
        real intensity;
        real pair[2];
        byte unknownf8[0x110 - 0xf8];
    };
    long const *light_reference = &light_index;
    s_light_dynamic *light = &((s_light_dynamic *)g_4e030c->data)[light_index & 0xffff];
    byte *definition = g_4e3b44[light->tag_index & 0xffff].bytes;
    long object_index = NONE;
    byte *object = NULL;
    if (light->owner != NONE)
    {
        object_index = light->owner;
        object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    }
    else if (light->parent != NONE)
    {
        object_index = light->parent;
        object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    }
    if (light->function_index != NONE)
    {
        struct { color3f tint; color4f second, first, pair_second, pair_first; } colours;
        color3f &tint = colours.tint;
        color4f &first = colours.first;
        color4f &second = colours.second;
        color4f &pair_first = colours.pair_first;
        color4f &pair_second = colours.pair_second;
        tint = *(color3f *)g_468710;
        *(color3f *)&first.red = *(color3f *)(definition + 0x58);
        first.alpha = 1.0f;
        *(color3f *)&second.red = *(color3f *)(definition + 0x64);
        second.alpha = 1.0f;
        *(color3f *)&pair_first.red = *(color3f *)(definition + 0x40);
        pair_first.alpha = 1.0f;
        *(color3f *)&pair_second.red = *(color3f *)(definition + 0x4c);
        pair_second.alpha = 1.0f;
        function_bab40(object_index, light->name, &light->fraction);
        function_bad50(object_index, light->colour_index, (point3f *)&tint);
        function_131d60(&first, &second, *(dword *)(definition + 0x34),
            light->fraction, &tint, &light->colour);
        function_131d60(&pair_first, &pair_second, *(dword *)(definition + 0x34),
            light->fraction, &tint, &light->pair_colour);
        light->intensity = (1.0f - light->fraction) *
            *(real *)(definition + 0x70) + *(real *)(definition + 0x74) * light->fraction;
    }
    else if (((byte *)&light->flags)[0] & 0x10)
    {
        real elapsed = (real)(g_510c54->game_time - light->start_time);
        real duration = (real)g_510c54->field_2_3 * *(real *)(definition + 0xb0);
        real fraction = function_17ca10(elapsed / duration, *(short *)(definition + 0xb6));
        light->fraction = (1.0f - fraction) * light->scale;
        function_131c20((color3f *)(definition + 0x58), (color3f *)(definition + 0x64),
            *(dword *)(definition + 0x34), light->fraction, &light->colour);
        function_131c20((color3f *)(definition + 0x40), (color3f *)(definition + 0x4c),
            *(dword *)(definition + 0x34), light->fraction, &light->pair_colour);
        light->intensity = (1.0f - light->fraction) *
            *(real *)(definition + 0x70) + *(real *)(definition + 0x74) * light->fraction;
    }
    else
    {
        light->fraction = light->scale;
        *&light->colour = *(color3f *)(definition + 0x64);
        *&light->pair_colour = *(color3f *)(definition + 0x4c);
        light->intensity = *(real *)(definition + 0x74);
    }
    function_c0b00((s_light_animation_ab *)(definition + 0xc0), light_index,
        light->fraction, &light->intensity, (vector3f *)&light->colour, light->pair);
    light->fade = 0.0f;
    if (object)
    {
        byte *parent = *(byte **)(g_4e0300->data + (function_baf80(object_index) & 0xffff) * 12 + 8);
        if (((1 << parent[0xaa]) & 3) && *(real *)(parent + 0x2b0) > 0.0f &&
            (signed char)g_4e3b44[light->tag_index & 0xffff].bytes[0] >= 0)
        {
            light->fade = clamp_light(*(real *)(parent + 0x2b0), 0.0f, 1.0f);
            real scale = 1.0f - light->fade;
            light->colour.red *= scale;
            light->colour.green *= scale;
            light->colour.blue *= scale;
            light->pair_colour.red *= scale;
            light->pair_colour.green *= scale;
            light->pair_colour.blue *= scale;
        }
    }
    real red = clamp_light(light->colour.red, 0.0f, 1.0f);
    light->colour.red = red;
    real green = clamp_light(light->colour.green, 0.0f, 1.0f);
    light->colour.green = green;
    real blue = clamp_light(light->colour.blue, 0.0f, 1.0f);
    light->colour.blue = blue;
    real pair_red = clamp_light(light->pair_colour.red, 0.0f, 1.0f);
    light->pair_colour.red = pair_red;
    real pair_green = clamp_light(light->pair_colour.green, 0.0f, 1.0f);
    light->pair_colour.green = pair_green;
    real pair_blue = clamp_light(light->pair_colour.blue, 0.0f, 1.0f);
    light->pair_colour.blue = pair_blue;
    light->intensity = clamp_light(light->intensity * 2.0f, 0.0f, 4.0f);
    light->radius = (1.0f - light->fraction) * *(real *)(definition + 8) +
        *(real *)(definition + 0xc) * light->fraction;
    if ((((byte *)&light->flags)[0] & 1) && light->radius > 0.0f &&
        (red != 0.0f || green != 0.0f || blue != 0.0f ||
            pair_red != 0.0f || pair_green != 0.0f || pair_blue != 0.0f))
    {
        if (!(((byte *)&light->flags)[0] & 2))
        {
            light->flags |= 2;
            if ((object_index == NONE || (!object_or_parent_hidden(object_index) && function_b9d20(object_index))) &&
                g_4de2f4 && *(byte *)g_4de2f4)
                function_c2d00(*light_reference);
        }
    }
    else if (((byte *)&light->flags)[0] & 2)
    {
        if (((byte *)&light->flags)[0] & 8) function_c3220(light_index);
        ((byte *)&light->flags)[0] &= ~2;
    }
}


void function_1cabc0(s_cluster_partition *partition, char const *name, long payload_size);

// @retail 0xbffa0
void function_bffa0()
{
    s_record_pool *lights = data_new_inlined("lights", 0x15e, 0x110, 0, g_510c2c);
    long size = 12;
    byte *allocation = game_state_globals.base_address + game_state_globals.cpu_allocation_size;
    game_state_globals.cpu_allocation_size += size;
    g_4e030c = lights;
    dword checksum = game_state_globals.allocation_size_checksum;
    function_163ba0(&checksum, &size, sizeof(size));
    game_state_globals.allocation_size_checksum = checksum;
    g_5107e8 = (s_5107e8 *)allocation;
    g_5107e8->flag8 = true;
    if (lights)
    {
        s_cluster_partition partition;
        function_1cabc0(&partition, "light", 0);
        g_4e0310 = partition.cluster_first_data_references;
        g_4e0314 = partition.data_references;
        g_4e0318 = partition.cluster_references;
    }
}


// @retail 0xc1720
long __stdcall function_c1720(long object_index, long remove_from_partition, long update)
{
    s_record_pool *lights = g_4e030c;
    s_record_pool_iterator iterator;
    iterator.data = lights;
    iterator.datum_index = NONE;
    iterator.index = NONE;
    long count = 0;
    byte *entry;
    while ((entry = (byte *)data_iterator_next_inlined(&iterator)) != NULL)
    {
        if (*(long *)(entry + 0x58) == object_index)
        {
            if (remove_from_partition)
            {
                byte *light = lights->data + (iterator.datum_index & 0xffff) * 0x110;
                if (light[2] & 2)
                {
                    s_cluster_partition partition =
                    {
                        (long *)g_4e0310,
                        (s_record_pool *)g_4e0314,
                        (s_record_pool *)g_4e0318
                    };
                    function_1cae40(&partition, iterator.datum_index, (long *)(light + 0x10));
                    light[2] &= ~8;
                }
            }
            if (update)
                function_c2d00(iterator.datum_index);
            ++count;
        }
    }
    return count;
}
