#include "effects.h"
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_11cc90.h"
#include "camera_storage.h"
#include "globals.h"
#include "data_iterator.h"
#include <string.h>
#include <math.h>

#include "unknown_123b30.h"
#include "unknown_0b68c0.h"
#include "crc.h"
// @flags /O2 /arch:SSE /Gr
#include "projection_storage.h"
#include "normalize3d.h"
#include "vector_products.h"

/* Light pool queries and the shapes copied from definitions and placements. */
s_record_pool *g_4e030c;
long g_4e0308;


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

__declspec(noinline) void function_c0040();

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0xc0040
void function_c0040()
{
	data_make_valid_inlined(g_4e030c);
	_ReadWriteBarrier();
	g_5107e8->flag8 = true;
	_ReadWriteBarrier();
	memset(g_4e0310, 0xff, 512 * sizeof(long));
	data_make_valid_inlined((s_record_pool *)g_4e0318);
	_ReadWriteBarrier();
	data_make_valid_inlined((s_record_pool *)g_4e0314);
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

struct s_light_ab
{
	short salt;
	word flags;
	long tag_index;
	long scenario_index;
	long stamp;
	long first_cluster_reference;
	byte unknown14[4];
	point3f centre;
	real radius;
	point3f fade_centre;
	real fade_radius;
	point3f location_point;
	byte unknown44[0x84 - 0x44];
	s_light_frame_ab frame;
	byte unknownc8[0xd4 - 0xc8];
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
	byte unknown06[0xc - 6];
	real radius_scale;
	byte unknown10[0x18 - 0x10];
	real radius_a;
	real radius_b;
	s_light_cone_ab cone;
	byte unknown34[0x7c - 0x34];
    long bitmap_index;
    byte unknown80[0x98 - 0x80];
    real minimum_radius;
    byte unknown9c[0xb8 - 0x9c];
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

__declspec(noinline) bool function_c3110(long index);

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
    s_light_definition_ab volatile *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    light->fade = light_distance_fade_ab(light, definition->fade_distance, 10.0f, g_4405f0);
    light->fade_f8 = light_distance_fade_ab(light, definition->fade_colour, 5.0f, g_440604);
    light->fade_fc = light_distance_fade_ab(light, definition->fade_colour, light->fade_radius, g_440618);
    light->fade_104 = light_distance_fade_ab(light, definition->fade_range, 1.0f, g_44062c);
    light->fade_108 = light_distance_fade_ab(light, 0, light->fade_radius * 0.5f, g_440640);
    if (light->fade_f8 > light->fade) light->fade_f8 = light->fade;
    if (light->fade_f8 < light->fade_104) light->fade_104 = light->fade_f8;
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

static __forceinline void point_add_scaled_c35a0(point3f const *point, vector3f const *vector, real scale, point3f *result)
{
    result->x = point->x + vector->i * scale;
    result->y = point->y + vector->j * scale;
    result->z = point->z + vector->k * scale;
}

/* scales v to unit length and returns its old length, or 0 for a vector too short to normalize */
static inline real direction_normalize_c0840(vector3f *v)
{
    real m = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);
    if (!(fabs(m) < 0.0001f))
    {
        real inverse = 1.0f / m;
        v->i = inverse * v->i;
        v->j = inverse * v->j;
        v->k = inverse * v->k;
        return m;
    }
    return 0.f;
}

/* steps along the segment from start to end, doubling the step up to 0.1, until a point lies in a
   BSP leaf with a cluster: that point and its distance along the segment */
// @retail 0xc0840
bool function_c0840(point3f const *start, point3f const *end, point3f *out, real *distance)
{
    bool result = false;
    real travelled = 0.0f;
    real step = 0.001f;
    vector3f direction;
    scaled_difference3f(start->n, end->n, 1.0f, direction.n);
    real length = direction_normalize_c0840(&direction);
    if (length > 0.0f)
    {
        s_bsp3d *bsp = g_4e033c;
        while (!result && length > travelled)
        {
            point3f point;
            point_add_scaled_c35a0(start, &direction, travelled, &point);
            if (g_4686c4 != NONE)
            {
                long leaf = function_14a280(bsp, 0, &point);
                short cluster = leaf != NONE ? *(short *)(*(byte **)((byte *)g_4e0348 + 0x30) + leaf * 8) : NONE;
                if (cluster != NONE)
                {
                    *distance = travelled;
                    *out = point;
                    result = true;
                    continue;
                }
            }
            travelled += step;
            real next = step * 2.0f;
            step = next > 0.1f ? 0.1f : next;
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
        byte *function = animation->pair;
        pair[0] = function_13bb90((s_tag_data *)function, time, range);
        pair[1] = function_13bb90((s_tag_data *)(function + 8), time, range);
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

extern point2f *g_468774;

static inline real dot_product_c1a80(vector3f const *a, vector3f const *b)
{
    return a->k * b->k + a->j * b->j + a->i * b->i;
}

/* result = first x second; the second operand comes first */
static inline void cross_product_c1a80(vector3f const *second, vector3f const *first, vector3f *result)
{
    real k = first->i * second->j - first->j * second->i;
    real j = first->k * second->i - first->i * second->k;
    real i = first->j * second->k - first->k * second->j;
    result->i = i;
    result->j = j;
    result->k = k;
}

static inline void advance_along_vector_c1a80(point2f const *point, vector2f const *vector, real scale, point2f *result)
{
    result->x = vector->i * scale + point->x;
    result->y = vector->j * scale + point->y;
}

/* result = value scaled by the frame's radius */
static inline void scale_by_frame_c1a80(real value, s_light_frame_ab const *frame, real *result)
{
    *result = value * frame->radius;
}

// @retail 0xc1a80
void function_c1a80(s_light_frame_ab const *frame, s_light_shape_ab *shape, s_light_definition_ab const *definition)
{
    struct s_render_cone
        {
            real unused;
            vector3f direction;
            real distance;
            vector2f slope;
            real aspect;
            point2f size;
            real near_distance, far_distance;
            point2f far_size;
            point3f endpoint;
        };
    s_render_cone *cone = 0;

    if ((short)shape->kind == 0)
    {
        scale_by_frame_c1a80(shape->sphere.radius_a, frame, &shape->sphere_render.radius_a);
        scale_by_frame_c1a80(shape->sphere.radius_b, frame, &shape->sphere_render.radius_b);
        shape->sphere_render.radius = shape->sphere_render.radius_a > shape->sphere_render.radius_b ? shape->sphere_render.radius_a : shape->sphere_render.radius_b;
    }
    else
    {
        cone = (s_render_cone *)&shape->cone_render;
        vector3f *direction = &cone->direction;
        vector3d_from_points3d(&frame->position, &frame->endpoint, &cone->direction);
        cone->distance = dot_product_c1a80(&frame->forward, &cone->direction);
        if (fabs(cone->distance) < 0.0001f || cone->distance < 0.0f)
        {
            *direction = frame->forward;
            cone->distance = 1.0f;
        }
        real scale = 1.0f / cone->distance;
        direction->i *= scale;
        direction->j *= scale;
        direction->k *= scale;
        cone->aspect = shape->cone.values[1];
        if (definition->bitmap_index != NONE)
        {
            byte *bitmap = g_4e3b44[definition->bitmap_index & 0xffff].bytes;
            byte *image = *(byte **)(bitmap + 0x48);
            cone->aspect = (real)*(short *)(image + 6) / (real)*(short *)(image + 4) * cone->aspect;
        }
        if (shape->kind == 1)
        {
            cone->slope.i = g_468774->x;
            cone->slope.j = g_468774->y;
        }
        else
        {
            real slope = tanf(shape->cone.values[2] * 0.5f);
            cone->slope.i = (real)slope;
            cone->slope.j = (real)slope * cone->aspect;
        }
        cone->size.x = shape->cone.values[0];
        cone->size.y = cone->aspect * shape->cone.values[0];
        cone->far_distance = shape->cone.values[4] * frame->radius;
        cone->near_distance = shape->cone.values[3] * frame->radius;
        advance_along_vector_c1a80(&cone->size, &cone->slope, cone->far_distance, &cone->far_size);
        point_add_scaled_c35a0(&frame->position, direction, cone->far_distance, &cone->endpoint);
    }
    cross_product_c1a80(&frame->up, &frame->forward, &shape->left);
    point3f *origin = &shape->origin;
    shape->origin_offset = 0.0f;
    *origin = frame->position;
    if (shape->kind == 2 && cone->slope.i > 0.0001f)
    {
        shape->origin_offset = shape->cone.values[0] / cone->slope.i * 0.5f;
        real distance = 0.0f - shape->origin_offset;
        origin->x = cone->direction.i * distance + frame->position.x;
        origin->y = cone->direction.j * distance + frame->position.y;
        origin->z = cone->direction.k * distance + frame->position.z;
    }
    shape->ready = true;
}

/* in unknown_241620.cpp, so that only link-time code generation inlines it */
bool lights_engine_enabled(bool respect_engine);

/* whether a light's shape covers anything */
static __forceinline bool light_shape_visible_c17f0(s_light_shape_ab const *shape)
{
    if ((short)shape->kind == 0)
        return shape->sphere_render.radius > 0.0001f;
    if (shape->cone_render.far_distance > 0.0001f &&
        shape->cone_render.far_width * shape->cone_render.far_width + shape->cone_render.far_height * shape->cone_render.far_height > 0.0001f * 0.0001f)
        return true;
    return false;
}

// @retail 0xc17f0
bool function_c17f0(long light_index, s_light_shape_ab *shape, bool respect_engine)
{
    bool result = false;
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    s_light_definition_ab *definition = (s_light_definition_ab *)g_4e3b44[light->tag_index & 0xffff].bytes;
    bool enabled = true;
    if (!(definition->flags & 0x100))
        enabled = lights_engine_enabled(respect_engine);
    if (g_5107e8->flag8 && enabled && (light->flags & 2))
    {
        function_c19f0(light_index, shape);
        function_c1a80((s_light_frame_ab *)((byte *)light + 0x84), shape, definition);
        result = light_shape_visible_c17f0(shape);
    }
    return result;
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
        long order[4];
        real far_width = (real)(shape.cone.values[4] * shape.cone_render.slope_x * 2.0f) + shape.cone_render.width;
        real far_height = shape.cone_render.aspect * far_width;
        vector3f direction = shape.cone_render.direction;
        function_30bf0(&direction);
        order[0] = 0; order[1] = 1; order[2] = 3; order[3] = 2;
        point3f near_centre;
        real far_y_value, far_z_value, far_x_value;
        near_centre.x = frame->position.x + direction.i * 0.0f;
        near_centre.y = frame->position.y + direction.j * 0.0f;
        near_centre.z = frame->position.z + direction.k * 0.0f;
        far_x_value = frame->position.x + direction.i * shape.cone.values[4];
        far_y_value = frame->position.y + direction.j * shape.cone.values[4];
        far_z_value = frame->position.z + direction.k * shape.cone.values[4];
        point3f far_points[4];
        point3f near_points[4];
        real height = shape.cone_render.height;
        for (long i = 0; i < 4; i++)
        {
            real x = ((order[i] & 1) ? 1.0f : -1.0f) * 0.5f;
            real y = ((order[i] & 2) ? 1.0f : -1.0f) * 0.5f;
            far_points[i].x = far_x_value;
            far_points[i].y = far_y_value;
            far_points[i].z = far_z_value;
            near_points[i].x = near_centre.x;
            near_points[i].y = near_centre.y;
            near_points[i].z = near_centre.z;
            point_add_scaled_c35a0(&near_points[i], &shape.left, (real)(x * shape.cone_render.width), &near_points[i]);
            point_add_scaled_c35a0(&far_points[i], &shape.left, (real)(x * far_width), &far_points[i]);
            point_add_scaled_c35a0(&near_points[i], &frame->up, (real)(y * height), &near_points[i]);
            real far_y = (real)far_height * y;
            point_add_scaled_c35a0(&far_points[i], &frame->up, far_y, &far_points[i]);
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
bool function_2dba0(long tag, bool alternate, long kind, long index, long marker_index, point3f const *position, vector3f const *direction, color3f const *color, real alpha, real amount, real scale);
void function_42850(long object_index, long tag_index, point3f const *position,
    vector3f const *first, vector3f const *second, real scale, real width,
    vector3f const *third);
byte g_55e708;

#define PIN(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

// score 38 (was 38): the loop, frame and clamp are retail's; left are scratch register picks only (object argument and
// count in the fallback arm, the loop's amount/index loads, the single-marker arm, the opacity reload). 0x2dba0's order is
// now applied: earlier BLOCKED-BY 0x2dba0 (parameter order): with its parameters declared (tag, alternate, kind,
// index, marker_index, position, direction, color, alpha, amount, scale) at its definition and every call (0x2dba0 and
// its other callers unchanged) the marker loop anchors on the position pointer as retail (the cursor anchors on the
// address evaluated last) and this is 45 at retail's 607 bytes, 38 with the opacity through the PIN macro; the
// direction after the colour 38, last 153 (0x2dba0 56), tag after alternate/position/direction 141 each.
// Earlier (was 109): 0x2dba0 now takes the tag in ECX and the direction in EDI as in retail; the marker loop
// walks the forward pointer in EDI (position = edi + 0x24, index in EBX) where retail walks the position pointer
// in EBX, derives the forward with `lea edi, [ebx - 0x24]` and keeps the index and count in memory (frame 0xf0
// against 0xec); the opacity clamp stores the product first. Earlier: BLOCKED-BY 0x2dba0: it took the position in
// ecx and the tag on the stack where retail
// passes the tag in ecx and the position on the stack, so the marker loop keeps its index in ebx and
// spills the position where retail walks the position pointer in ebx with index and count in memory.
// Round 70: indexed loop with a marker pointer per pass 109; single-marker case first 146; object
// index local at every use 157; the field read at every use 109; the file's clamp_light helper for
// the opacity 109 (retail's clamp shape, the rest unchanged).
// Round 56: indexed loop without the marker pointer 109; opacity clamp as if/else-if 109; short count
// and short index 125 (607 bytes, aligned diff 86 -> 55: the single-marker arm moves after the loop as
// in retail, but the loop counts down in registers); short index only 114; short count only 128.
// Round 141: bool warning flag 109; `count = 1` before the flag test 126; `count == 0` fallback test
// 109; colour pointer local 109.
// Round 39: count before markers 109; count/index before markers 109; cursor before the count clamp 109;
// const cursor 109; pointer for loop 109; marker_count/marker_index names 109/109;
// cursor after the array declaration 109. Fast preview 131 with caveats; plain all 109; original restored.
// Round 175 (plain): the absolute index written at each use 109; the effect tag read into a named local
// for the test and both calls 163 (retail's 607 bytes, but a 0xf4 frame and every register moved).
// Round 45 (lane 10, plain): an indexed for loop over `&markers[i].matrix` 128; a position pointer local in the
// indexed loop 128, in the pointer loop 128; the product and the clamped opacity as two locals 128 (aligned
// 99 -> 96); short count and index 145 (582 bytes). Compile-only (it reproduces the forward-pointer IV): a matrix
// pointer, both address locals and a marker pointer in the for loop all keep it; a walked position pointer
// gives two pointer IVs, not retail's one.
// Lane 12 r72 (plain; --fast builds this function differently, 189): the opacity as the file's PIN macro
// on the product (the macro moved above this function) 128 (retail's clamp stores, no count change);
// `g_55e708 = true` same code. Compile-only (reproduces the forward-pointer IV): a forward pointer
// local, an indexed loop with it, a position pointer local: same code. BLOCKED-BY 0x2dba0 (parameter
// order): compile-only, with 0x2dba0 declared with the direction after the position (the call passing it
// last) the loop's pointer IV becomes the position and the forward is derived (`lea reg, [iv - 0x24]`) as
// in retail, as matched 0xc6fb0 does for 0xc9040 (position before forward); not built: 0x2dba0's
// definition sits in src/unknown_0350e0.cpp (left alone), and whether LTCG keeps the direction in EDI
// after that reorder is unverified.
// Round 47 (lane 10; fast with 0x31520 and 0x3e9c0 listed agrees with the plain check): the opacity through the
// PIN macro 128 (plain; aligned 99 -> 96); the clamp as if/else-if 128; an indexed for loop 128; object_index for
// every object read in the marker arm 152; the first-person count as a ternary initialiser 128. Compile-only: a
// forward/position/matrix pointer local, both locals in either order, the 0x3e9c0 result as a local, a do-while,
// a for over both cursors keep the forward anchor; mixing `markers[i]` with the walked pointer gives two cursors.
// Round 50 (lane 10, fast with 0x31520 and 0x3e9c0 listed): the marker array and count declared before the object
// and marker-name reads 128; the object_index local also as the loop's 0x3e9c0 argument 148 (587 bytes); the
// fallback count as a ternary 128; the first-person test as nested ifs 128; the cursor advanced before the
// index 128. Still BLOCKED-BY 0x2dba0's parameter order.
// Round 52 (lane 10, fast with 0x31520 and 0x3e9c0 listed): the 0x3e9c0 result as a named bool in the loop
// 111 (607 bytes: frame 0xf0 as retail, but the bool is stored to the frame and the loop still walks the
// forward pointer; a one-use call result, not kept); a do-while under `count > 0` 128; the flags tested as the
// word field 128; the definition looked up inside the outer test 150; the clamp before the flag test 139 (592
// bytes); the first-person test as a named bool 128.
// Round 133 (lane 6, fast with 0x2dba0/0x31520/0x3e9c0/0xb8c40/0xb8d30/0x1662a1/0x42850 listed, which agrees with
// the full build; the file alone does not): the direction read into a pointer local before the call, indexed loop
// 128, pointer loop 128 (copy-propagated: identical code); a transform4x3f pointer local per pass in the real build
// 128 (identical); a pointer loop to `markers + count` with `marker - markers` as the index 161 (642 bytes); a
// guarded do-while 116 (607 bytes only by padding: the count clamp stores immediates and EBX is reloaded after
// 0xb8d30 as retail, but the forward anchor stays; a hand-inverted loop, not kept).
// Round 136 (m3 lane 5, fast with 0x2dba0/0x31520/0x3e9c0/0xb8c40/0xb8d30/0x1662a1/0x42850 listed, plain confirmed):
// 0x2dba0's order applied (retail pushes scale, amount, alpha, color, position, marker index, index, kind, then the
// 0x3e9c0 result, tag in ECX, direction derived from the position cursor) 45, the opacity through PIN 38. On 38: indexed
// while 38; for with marker pointer 40; indexed for 38; count/array declared first 38; object_index for all object reads
// 132, for the 0xb8c40 argument 135; no object_index local 38 (kept); count ternary 38; count if/else 38; `count == 0` 38;
// marker pointer declared with count 38; i before marker 38; marker stepped first 40; count = 1 before the flag test 107;
// PIN as call argument 38; named product 38; assignment inside the test 38; nested first-person test 38; short-cast count
// argument 38; amount local in the single arm 97; alpha local in the loop 114, both arms 115; colour pointer local 143;
// typed light/definition/object fields, singly and all together 38 each; direction after the colour/alpha/amount 38/38/38;
// the count arms in an inline helper 178 (stays a call), forced inline 146; a named fallback count 38. The picks do not
// move with symbols here (unlike 0x17d9a0): retail's fallback arm takes EAX for the object and EDX for the count, then
// `mov eax, edx`; ours takes EDX then EAX, coalesced.
// Round 29 (lane 18, fast with the same list): the fallback count accumulated (`count += ...`) 38 (same code);
// the fallback test `count <= 0` 38; the marker index as a for-loop counter 40; the marker index declared
// before the count 50; the absolute index computed inside the outer test (the light lookup masking the
// parameter itself) 38. In the loop retail loads the absolute index early (EDX) and the marker index after
// three pushes (ECX), the amount in EAX; ours loads the marker index early (EDX), the absolute index late (EAX).
// Round 92: early shared query/output cursor 38; separate long draw count 113; short raw count with long draw count
// 115; short count 131; short index 51; both short 77 (610 bytes); per-pass amount local 118 (613); const
// light/definition 38. Original body restored; fallback/count scratch, loop load schedule, single-marker arm and
// opacity reload remain.
// Round 64 (m3 lane 1, fast with the same list): dword absolute index 38 (same code); count zeroed in an else of the
// first-person test 38; unsigned loop index 39; unsigned count 39; count = 1 before the flag test 107. Permuter
// (600 s, annealing): reduced image scores this body 141 (frame 0xec), nothing better in 47 tries.
// Round 131 (m3 lane 8, fast with the same list): the flag test as an early return inside the 0x31520 test 38; the
// count clamped before the warning flag 107 (retail stores it after); the fallback test `count < 1` 42; the absolute
// index computed after the light and definition 38 (same code); the clamp test `count >= 2` 38. The rotations left
// (fallback object EAX/count EDX, loop amount EAX/absolute EDX/index ECX, single-arm amount ECX/colour EDX/position EAX,
// opacity ECX) do not move with any of these.
// Round 160 (m3 lane 5, fast with the same list), each on the current form: no absolute-index local (the mask at the lookup and both
// calls) 38; the opacity clamp as if/else-if in place of PIN 45; the first-person count as a ternary initialiser 38; the fallback as
// `count = count ? count : ...` 38; the single-marker arm first 107.
// m3 lane 2 r71 (fast with the same list): the loop index declared at the top of the function, before or after the
// absolute index, 38/38; the marker array declared at the top of the function 38 (all three the same code: the loop's
// load order does not follow declaration order here).
// Round 112 (m3 lane 16, fast with the same list): the first-person count assigned inside the fallback test
// (`if (type != 2 || ... || !(count = ...)) count = ...`) 38; the clamp as two tests on `count > 1` 38; the outer
// test as an early return 38; the outer test as nested ifs 38 (same code each); the second tag read into a named local
// for its test and call 40.
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
                byte *object = *(byte **)(g_4e0300->data + (*(long *)(light + 0x4c) & 0xffff) * 12 + 8);
                long marker_name = function_b8c40(*(long *)(light + 0x4c), *(short *)(light + 0x54));
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
                s_object_marker *marker = markers;
                long i = 0;
                while (i < count)
                {
                    function_2dba0(*(long *)(definition + 0x94), function_3e9c0(*(long *)(light + 0x4c)), 2, absolute_index, i, &marker->matrix.position, &marker->matrix.forward, (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f);
                    ++i;
                    ++marker;
                }
            }
            else
                function_2dba0(*(long *)(definition + 0x94), function_3e9c0(*(long *)(light + 0x4c)), 2, absolute_index, 0, (point3f *)(light + 0x84), (vector3f *)(light + 0xac), (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f);
        }
        if ((light[2] & 2) && *(long *)(definition + 0xa0) != NONE)
        {
            real opacity = PIN((1.0f - *(real *)(light + 0xc8)) * *(real *)(light + 0xd0), 0.0f, 1.0f);
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
    if ((short)shape.kind == 0)
    {
        real radius = (definition->flags & 2) ? shape.sphere.radius_a :
            (shape.sphere.radius_a > shape.sphere.radius_b ? shape.sphere.radius_a : shape.sphere.radius_b);
        radius *= definition->radius_scale;
        radius = radius > definition->minimum_radius ? radius : definition->minimum_radius;
        if (light->frame.clip_distance > 0.0f)
        {
            light->centre = light->frame.field_1c_3;
            light->radius = light->frame.clip_distance + radius;
        }
        else
        {
            light->centre = light->frame.position;
            light->radius = radius;
        }
        light->location_point = light->centre;
        light->fade_centre = light->centre;
        light->fade_radius = light->radius;
    }
    else
    {
        function_c1a80(&light->frame, &shape, definition);
        real width = shape.cone.values[4] * shape.cone_render.slope_x * 2.0f + shape.cone_render.width;
        real height = shape.cone_render.aspect * width;
        real side = dot3f(&shape.left, &shape.cone_render.direction);
        real side_sign = side >= 0.0f ? 1.0f : -1.0f;
        real up = dot3f(&shape.cone_render.direction, &light->frame.up);
        real up_sign = up >= 0.0f ? 1.0f : -1.0f;
        vector3f unit = shape.cone_render.direction;
        function_30bf0(&unit);
        point3f corner;
        corner.x = g_468788->x + unit.i * shape.cone.values[4];
        corner.y = g_468788->y + unit.j * shape.cone.values[4];
        corner.z = g_468788->z + unit.k * shape.cone.values[4];
        point_add_scaled_c35a0(&corner, &shape.left, side_sign * width * 0.5f, &corner);
        point_add_scaled_c35a0(&corner, &light->frame.up, up_sign * height * 0.5f, &corner);
        real radius = (real)(sqrt(corner.z * corner.z + corner.y * corner.y + corner.x * corner.x) * definition->radius_scale);
        radius = radius > definition->minimum_radius ? radius : definition->minimum_radius;
        if (light->frame.clip_distance > 0.0f)
        {
            light->centre = light->frame.field_1c_3;
            light->radius = radius;
        }
        else
        {
            light->centre = light->frame.position;
            light->radius = radius;
        }
        light->location_point = light->centre;
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
    plane3f plane;
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

/* result = -vector */
PRIVATE inline void vector3f_negate_ab(vector3f const *vector, vector3f *result)
{
    result->i = 0.0f - vector->i;
    result->j = 0.0f - vector->j;
    result->k = 0.0f - vector->k;
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
    vector3f transformed;
    transformed.i = matrix->forward.k * vk + matrix->forward.j * vj + matrix->forward.i * vi;
    transformed.j = matrix->left.k * vk + matrix->left.i * vi + matrix->left.j * vj;
    transformed.k = matrix->up.k * vk + matrix->up.i * vi + matrix->up.j * vj;
    real scale = -1.0f / transformed.k;
    result->x = scale * transformed.i;
    result->y = scale * transformed.j;
}

// score 290 (was 290; 315 before round 79; 482 before round 68; m3 lane 2 r79 and m3 lane 8 r164 notes at the end): (513 before round 91) the entry pointer's base (+0x4c), the direction cursor and the table stores
// now agree; retail stores the +radius vertex group first (so 1.0 lands in XMM2, not XMM1) and
// computes each projection's forward row first. Kept: the file's cross helper for both crosses
// (it sets the +0x4c base; the shared cross3f for the forward one gives +0x50), normalize3d for
// the left vector, depth copied from the shape field, an aggregate vertex table, the point and
// dot helpers with a plane3f for the plane, one negate helper for both up vectors. Tried: term
// and row orders in the projection (identical); y stored before x with each component
// declaration order 551-571; the radius field at each table use 645; a named negative radius 574.
// Round 64: the face table declared before the direction table 513 (kept; retail stores the rest of the
// faces before the directions). The projection helper through a local vector (rows forward, left, up)
// 511 with retail's x-first rows, -1.0 in XMM7 and the +radius vertex group first (the top matches to
// the first cross product); with y stored first 511; also the left.i product reversed 511; parenthesised
// rows 576; y first with parentheses 579; normalize_inline for the left vector 581 (alone), 586/587
// with the local vector; the shared cross3f for the left vector 570, 528 with the local vector; the
// other table orders 549-568. Next: the local vector plus retail's cross-product loads (up loaded into
// registers, up.i twice) and the normalize's i, k, j product order.
// Round 109: VC re-sorts each projection row (retail x = (forward.j vj + forward.i vi) + forward.k vk,
// z = (up.k vk + up.i vi) + up.j vj; ours x = (k + i) + j, z = (i + j) + k). A local vector with the
// rows through dot3f/dot_product_c1a80/dot3f(v first) 587/587/588; the scale through vector3f_scale
// 714; the up row computed first 513 (identical).
// Round 88: projection through a local vector of the three rows 619; depth stored first and the
// radius read back from it 611; the entry pointer before the bounds copy 511 (retail's store order
// there, two points: not kept); point3f vertex table 513 (identical); named 0 - radius 587.
// Round 73: the projection through the existing 0x1427f0 transform 1211 (kept out of line, also with a
// __forceinline declaration); cross-product operand orders 513 (identical: canonicalised); the
// cross product stored straight to the result 581 (0xc1a80 stops matching); the local-vector projection
// again 511: then the tables agree and the first cross product's loads (retail loads up.i, up.j and the
// reference into registers) and normalize3d's product order are next. Vertex forms with 0 - radius, a
// negative-radius local before or after radius, const radius: our -radius group stays first (harness).
// Round 91 (lane 7): kept the projection rows in a `vector3f transformed` plus the shared
// cross3f_staged for both crosses (the file's twin with swapped operands) 482: the top matches
// retail to the first cross product. Before that: the vertex copied into a local vector 622, with
// the rows too 619; rows alone 511; rows with normalize_inline 587; with cross3f for the left cross
// 537, also normalize_inline 419 (retail's length, 298 aligned). On 482: rows in retail's or natural
// term order 482/482 (canonicalised); y stored first 480; normalize_inline 575; rows through dot3f
// 605; the forward cross's i row with its operands swapped (diagnostic) 482. Left: the first cross
// folds up.i/up.j where retail loads them, normalize3d's sum (j, i, k against i, k, j), the forward
// cross's i-row loads, and the projection rows.
// Round 175 (one-file LTCG harness, exact here; plain where marked): a face row pointer for the four
// projections 481 (plain); a const matrix pointer for them 486, both 483; up / left vector pointers in
// the sphere arm 620/630 (with the face and/or matrix pointer 619-636); a plane pointer for the normal
// and distance 543; depth from the radius local 607; the projections as a corner loop 529 (plain),
// with depth from the radius 610; the projection's vector as one aggregate copy 624 (2892 bytes); one scale
// local for both projection scales 500; the inner scale named `inverse` 482 (identical); the scale as
// three divides 599. Diagnostic: dead initialised locals change it by count alone (+1..+12:
// 479/479/539/315/483/480/480/480/479/479/483/483, any position); +4 gives 2826 bytes and fixes most of
// the projection rows, so a natural form with that many more symbols is the lead.
// Round 68 (lane 12): the cone arm's four bounds values as named locals, each used for the bounds and two
// corners, 315 (2826 bytes; kept): the first cross now loads up/reference as retail, leaving register
// permutations, normalize3d's j, i, k sum and the projection rows' term order. On 315: the bounds in
// x0, y0, x1, y1 order (retail's store order) or after the corners 315/315; plane written through a plane
// pointer 543 (with corner order 543); normalize_inline 567; cross3f for the left cross 526, with
// normalize_inline 541; depth from the radius local 556; reference selected before the negate 498;
// unused-local probes +1/+2/+3/+4/+8/+12 483/480/480/480/483/467 (diagnostic). Before the named values:
// bounds in corner order alone 482.
// Round 133 (m3 lane 6, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0; aligned counts from a difflib
// alignment, control 279): retail's left normalization loads k, j, i (normalize3d's loads) but sums i*i + k*k then
// j*j (normalize_inline's sum). normalize_inline 567 (2806 bytes, aligned 450: sum right, loads k, i, j); the
// existing 0x30bf0 called 695 (2734 bytes: stays a call); a one-caller normalize with the sum spelled
// i*i + k*k + j*j 315 (identical to normalize3d: the unparenthesised sum is re-sorted), i*i + (j*j + k*k) 568
// (aligned 448), (i*i + k*k) + j*j 567 (aligned 450).
// Round 136 (m3 lane 5, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0): an instruction-shape histogram against
// retail shows the same shapes except 7 fewer `movss x, [m]`, 6 more `mulss x, [m]`, 4 more `movaps` and the x87 sum's
// stack order: no block is missing, only which operand folds. Half extents as width*scale then `*= 0.5f` 603 (2773 bytes);
// extents read first, scale after, `*=` steps 315 (same code); bounds stored after the corners in retail's x0, y0, x1, y1
// order 315 (aligned 279 -> 276); both 603; extents-first with bounds last 315; centre read into named x/y 568; plane
// distance through dot3f 315; depth stored after has_plane 377.
// Lane 17 r36 (fast): the file's direction_normalize_c0840 for the left vector 315 (same code as normalize3d: our
// scale keeps left.i in XMM5 from the cross where retail reloads it); reference picked by a ternary copy 647 (2793
// bytes); scale stored before position 331; orthogonal/has_plane stored after the matrix 589; the count as an
// if/else 323. Original body restored.
// Round 61 (m3 lane 10, fast): in the cone arm retail stores the four points, then the bounds as x0, y0, x1, y1,
// then the point count, and divides for the scale only after the centre projection: the bounds stores moved after
// the points in that order 315 (retail's store sequence there, same count); on it the far edges as `half + centre`
// 315; the scale computed before the centre projection 601 (2776 bytes); the edges declared x0, y0, x1, y1 315,
// with the far edges as `half + centre` 315. Retail's normalize sums i, k, j (normalize_inline's grouping, which
// earlier rounds found worse overall).
// Round 79 (m3 lane 9, plain): the plane distance as dot(plane point, normal) 290 (kept, 2830 bytes: the dot's
// loads and products are now retail's; left: the plane point is computed x, z, y where retail computes y, z, x,
// and the distance is stored through [esi + 0x80] where retail stores through the normal's pointer). Before it
// (315): the normal copied before the plane point 564. On 290: a plane3f pointer for n and d 543; plane point
// declared at the cone arm's top 290; the point scaled by the shape's offset field 554; projection products
// written point-first 290 (same code); the left vector through direction_normalize_c0840 290 (same code).
// Round 82 (m3 lane 16, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0): the plane distance as dot(normal,
// plane point) 315 (2826 bytes); one `offset` local for the plane offset, has_plane, depth and scale 486 (2807 bytes);
// `orthogonal = has_plane = false` 292; half extents as `width * 0.5f * scale` 290 (same code); the centre point declared
// at the cone arm's top 290 (same code). Retail stores the plane distance through the plane's own pointer ([ecx + 0xc]).
// Round 81 (m3 lane 9, fast): the first cross through the file's cross_product_c1a80 (operands swapped) 290 (same
// code); both crosses through it 433 (2821 bytes); only the second 433; a one-caller cross with the i term written
// `b->k * a->j` (diagnostic, removed) 290 (same code: the operands re-sort; retail's first cross differs only in
// xmm colours, ref.i/ref.j/ref.k in xmm1/xmm4/xmm1 where ours has xmm4/xmm3/xmm4); a one-caller plane-point add
// computing y, z, then x as `scale * v->i` (diagnostic, removed) 297 (retail computes x last as `movaps xmm0,
// scale; mulss xmm0, [forward.i]` but in xmm0, y/z in xmm1/xmm2).
// Round 82 (m3 lane 16, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0): the plane distance as dot(normal,
// plane point) 315 (2826 bytes); one `offset` local for the plane offset, has_plane, depth and scale 486 (2807 bytes);
// `orthogonal = has_plane = false` 292; half extents as `width * 0.5f * scale` 290 (same code); the centre point declared
// at the cone arm's top 290 (same code). Retail stores the plane distance through the plane's own pointer ([ecx + 0xc]).
// M3 lane 12 r90 (fast, base 290, masked 126): the projection's sum rows grouped, forward (j + i) + k, k + (j + i), (i + j) + k 561/561/561 (2815); left (i + k) + j, j + (i + k) 562/562;
// up (i + k) + j, j + (k + i) 561/561; forward and left 600; all three 554/554. The half extents with the scale inline twice 580, width read first 580, extents read then scaled 600, with the bounds after the
// points 600. Bounds stored after the points in x0, y0, x1, y1 order 290 (masked 119), the edges computed x0, y0, x1, y1 290 (masked 123), both 290 (masked 115); depth operands swapped 290. Diagnostics (identity chains
// in the helper, reverted): on the matrix 1-7 links 585/580/585/580/585/580/585, on the point 484/479/484/479/484/479/484.
// Round 148 (m3 lane 5, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0; aligned 199): a one-caller normalize scaling as `inverse * i`, `j * inverse`, `k * inverse` (retail's
// product shapes) with dot3f 290, length_sq3f 290, the sum i, j, k 290, k, j, i 290 (same code each: product operands re-sort); with double products 463 (2817 bytes). Dead locals +13..+20
// at the loop top 470/470/466/466/466/467/467/472 (diagnostic). After the normalize retail reads left.j and left.k before up.*, and up.* before left.i; ours reads up.* first.
// Directions as `real [6][3]` 290; the 0x11f5f0 count as a constant 4 600 (2777 bytes). Lead, gains too small to keep: `entry` declared before `bounds` 288 (aligned 198); in the cone arm
// `entry = entries; memset(entry, 0, sizeof(*entry));` 290 but aligned 181: the loop tail after 0x141590 becomes retail's (entry reloaded into ECX, the cursors' slots); both 288, aligned 180.
// Round 56 (m3 lane 15, plain; masked = aligned diff with stack offsets and registers masked, 137 here): rows in stages (forward j, i, k; left i, k, j; up k, i, j) as retail's sum trees,
// x first 573 (2801 B, masked 167), y first 571 (2803 B, masked 174); plain rows with y stored first 299 at retail's 2840 B (masked 154); the reference as a ternary 643 (2797 B), as if/else
// 574 (2822 B). Harness with the 0x11f5f0, 0x141590, 0x163db0 files and the ballast is not exact (414 for 290): source term order is canonicalised (same code in five orders).
// Round 59 (m3 lane 15, fast, base 290): the plane normal copied before the plane point 560 (2817 B); the projection's matrix scale in a named local
// for its test and divide 521 (2809 B); the cone reciprocal from `entry->plane_offset` 562 (2815 B); chained flag stores 290 (same code); the cone depth
// summed from `entry->plane_offset` 291; at the first difference (+0x33, retail's memset `lea edi` inside the bounds copy) `memset(&entries[i], ..)`
// before the entry pointer 628 (2821 B). Retail divides for the cone scale after the centre projection, with 1.0 in XMM6 (ours XMM4).
// Statement walk against retail (f1E, base r709, 2840 bytes): no logic or structure difference. Checked: count 6/1 and the
// `i < *count` reload (0xc1da5-0xc1dc1, 0xc287e); entry stride 0x1bc and memset (0xc1e1c); the eight vertices (0xc1eb0-0xc1f57),
// six faces (0xc1f67-0xc1fcf) and six directions (0xc1fd6-0xc206f); depth = radius, orthogonal/has_plane cleared (0xc1ea9,
// 0xc2078, 0xc207b); up = -direction, reference g_4687a8 or g_4687ac for i == 1 or 3 (0xc20bc-0xc20e2); left = up x reference
// and forward = left x up (0xc20e2-0xc213c, 0xc21b0-0xc21fb); normalize skipped for |len| < 1e-4 (0xc216e-0xc2172); four
// projections through faces[i][0..3] (-1 / z, scale test against 1.0) into points[0..3], count 4, 0x11f5f0 (0xc2577); cone arm:
// count 1, forward = shape->left, left = frame->up, up = -frame->forward, orthogonal = (kind == 1), has_plane = offset > 0,
// plane n = frame->forward and d = n . (origin + offset * forward), depth = far_distance + offset, centre projection, half
// extents = size * (1 / offset) * 0.5, corners x0y0 x1y0 x1y1 x0y1, bounds x0 x1 y0 y1 (0xc258f-0xc2820); tail flags,
// 0x141590 (matrix -> ecx, inverse -> eax), 0x163db0 (0xc282a-0xc285f). The only differences are rounding-level:
// retail sums the length as (i*i + k*k) + j*j (ours (j*j + i*i) + k*k), and the projection rows as forward (j, i, k),
// left (i, k, j), up (k, i, j) (ours forward (k, i, j), left (j, k, i), up (j, i, k)); everything else left is
// operand folding and register colours. Callers 0x31c80 (184) and 0x32350 (448) and callees unchanged by this note.
// Permuter, 30 minutes (11-06): 277 only with the sphere radius read through a volatile pointer and word casts. Not kept.
// Round 160 (m3 lane 5, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0): the cone plane through an inline (plane, normal, point)
// helper storing d through the plane pointer 563 (2795 bytes); cone bounds after the points in x0, y0, x1, y1 order with the far edges
// as `half + centre` 290 (masked aligned 126 -> 115); the projection's component locals renamed x/y/z 290 (identical); a face-row
// pointer for the four sphere projections 481 (2814 bytes); a `points` pointer local for the projections and the bounds call 555
// (2798 bytes). Retail's sphere arm sums the projection's forward row (j, i, k) but its cone arm (k, j, i): per-instance order.
// Round 97 (m3 lane 9, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0; aligned in brackets, base [273]): the four sphere projections as a
// corner loop 528 (2810 bytes) [405]; a direction pointer for the negate 290 (same code); the sphere test in a bool 479 (2814) [293]; a
// matrix pointer set after the memset and passed to 0x141590 479 (2814) [293]; a points pointer for the cone corners 479 (2814) [288];
// the orthogonal test with the kind cast to short 599 (2778); the cone width and height read into named locals 576 (2800); the far
// distance in a named local 580 (2796); the sphere point count in a short local 575 (2801). As before, any added local moves the
// projection rows and the length; none below 290. Body unchanged.
// Round 161 (m3 lane 6, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0, all matched): in the sphere arm the two flag clears
// before the depth 352; the reference picked before the up negate 508 (2828 bytes); the matrix scale stored before its
// position 306; the position and scale stored before the up negate 517 (2840 bytes); the tail flags as 84, 85, 8c 290
// (same code); the first three re-measure listed forms. New on top: has_plane as `0.0f < offset` 290 (same code); the cone
// corners read back from the bounds fields 290 (same code); the sphere depth stored right after the radius read 310.
// Body unchanged.
// Round 84 (m3 lane 1, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0, 290 as the full check; aligned and masked aligned counts in brackets,
// base [199, 81]): r148's lead (`entry` declared before `bounds` with the cone arm's `entry = entries; memset(entry, 0, sizeof(*entry));`) re-measured
// 288 [180, 71]; new: those two with the cone bounds stored after the points as x0, y0, x1, y1 288 [178, 67] (everything up to the first cross and the
// loop tail are then retail's; only float operand folding, the length sum and the projection rows are left; two points, not kept:
// build/r84/lead-0xc1d90-288.patch); that with the far edges as `half + centre` 288 [178, 67] (same code); the cone-arm form with the bounds order,
// with or without the far edges, 290 [179, 68]; `entry` first with the bounds order and far edges 288 [196, 76]. Diagnostic on the 288 form: dead
// initialised locals +1..+8 at the top 477/478/478/482/481/481/477/477 (2809-2814 bytes) [189-196]: no symbol-count phase is nearer than the current
// one. On the 288 form: the reference as a pointer chosen by a ternary 701 (2749 bytes, first difference +0x2) [382, 173], chosen by an if 684 (2757)
// [313, 173], chosen by an if before the up negate 704 (2763) [357, 203]; the bounds copied after the memset 305 (2830) [186, 75]. The if-chosen pointer
// on the kept body 685 (2757) [330, 183]: a reference pointer changes the prologue (retail copies the reference). Body unchanged.
// Round 87 (m3 lane 1, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0, base 290): `bounds` declared at function scope and
// assigned at the loop top 290 (same code); the entry pointer as a loop cursor (`entry = entries`, `entry++` in the for) 656
// (2807 bytes). On round 84's 288 lead (re-measured 288, aligned/masked 178/67): the projection storing y before x 297 (2840
// bytes, retail's length, aligned 195), the plane distance as dot(normal, plane point) 313, the sphere depth from the radius local
// 529. No other natural form that this note does not already list is left (projection rows, sums, normalize helpers, vertex and
// face tables, store orders, symbol-count probes). Body unchanged.
// Round 164 (m3 lane 6, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0): light_projection_point_ab reading the point's
// components in the other five orders (it is used only here): k, j, i 531 (2815 bytes); j, i, k 479; i, k, j 293; k, i, j 481;
// j, k, i 532 (the declaration orders listed at 551-571 were on the old y-first base). The read order moves every
// projection's sum tree, but i, j, k stays best. Body unchanged.
// Round 166 (m3 lane 6, fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0; masked = register-masked aligned count, base 122):
// a `short` loop index 659 (2809 bytes); the forward cross through the plain shared cross3f 579 (2801 bytes); in the cone arm
// the up negate before the two copied axes 356 at 2840 bytes (masked 160); the bounds declared once outside the loop 290
// (same code); the cone plane point from the shape's origin instead of the matrix position 600 (2777 bytes). Body unchanged.
// m3 lane 2 r71 (fast with the same list): light_projection_point_ab with `transformed` declared first 290 (same
// code), with the rows as three named scalars 539 (2801 bytes), with the point copied as a vector3f 623 (2891); the
// point read in each arm of the scale test (if/else) 554 (2756); the face table declared first, then vertices and
// directions 424, then directions and vertices 442 (both at retail's 2840 bytes; the bounds copy and memset then
// misalign). Body unchanged.
// M3 lane 9 r109 (fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0, base 290, aligned 273, masked 148): what differs is
// unchanged (the entry pointer formed before the bounds' last stores, the projection rows' sum orders per instance, the left
// normalize's sum, the cone plane distance stored through [esi + 0x80] where retail stores through the plane's own pointer, the
// cone scale divided before the centre projection, the loop-end cursor registers); no untried natural form left and no
// existing plane setter in include/. Body unchanged.
// M3 lane 14 r83 (fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0/0xc35a0/0xc28b0; matched 0xc35a0 reads DIFF in every
// --fast run here, the unmodified file too): the cone half height before the half width 290 (different code); the plane
// distance as dot(plane point, frame->forward) 600 (2777 B); the plane point scaled by shape->origin_offset 554 (2803 B);
// the cone bounds computed into `bounds` and the corners read back from it 486 (2809 B); the scale before the centre
// projection re-measured 597 (2780 B). Round 84's 288 lead with the half height first 288 (masked 106 against 126); on it
// the cone scale from entry->plane_offset 560 (2815 B), depth stored before the plane 301, has_plane tested on
// entry->plane_offset after its store 560, the matrix scale stored before the position 305, the edges computed x0, y0,
// x1, y1 288 (masked 101). Same code: `real const radius`, `transformed` declared first in the projection helper, its
// -1/k local renamed.
// M3 lane 3 r236 (fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0/0xc35a0; aligned 273, register-masked 122): the
// existing vector3f_add_scaled (vector_products.h) for the cone plane point 290, with the distance through dot3f(plane
// point, normal) 290 (same code each: the file's point_add_scaled_c35a0 is the same expression). Checked the other include/
// vector helpers: vector3f_rotate_into_axes sums the matrix columns (not the projection's rows), plane_distance_to_point
// subtracts d (not the plane's own distance), magnitude3d is out of line (retail has no call), the reciprocal normalizers
// skip only a zero length (retail tests 1e-4). Re-measured on this base: cross3f for the left cross with normalize_inline
// 548 (2817 B, masked 146); the file's cross_product_c1a80 there with normalize_inline 566 (2810 B, masked 134). No unlisted
// helper or natural form is left.
// m3 lane 15 r79 (fast with 0xc1a80/0x141590/0x163db0/0x11f5f0/0x30bf0, = full 290; masked = aligned rows with registers and
// constants masked, 108): `__forceinline` declarations on the projection helper, the negate helper, the file's dot helper
// (0xc1a80 and 0xc0520 stay matched), and forceinline redeclarations here of normalize3d and cross3f_staged: 290 each (same
// code). In the forward cross retail loads the left vector's component first in every product except those with left.i,
// and reloads left.i after the normalize where ours keeps it in XMM5 from the first cross. The direction table declared
// before the face table 322, declared first of the three 367.
// M3 lane 2 r79 (fast with the file's functions, 28 MATCH unchanged; base 290): retail's left normalization is 0x30bf0's own code
// (loads k, j, i, sums (i*i + k*k) + j*j, then `v->i * inv`, `inv * v->j`, `inv * v->k`), but that function's file is /Ob1 and
// LTCG keeps it a call here: function_30bf0 with a `__forceinline` redeclaration 701 (2730 B, still called); a private inline
// copy of its exact text 290 (same code as normalize3d: the x87 order follows the context, not the text). Also: the count test
// without its `(short)` cast 598 (2815 B, a dword compare for retail's word neg/sbb); the face and direction tables `const` 290
// (same code); one `vector3f const *` for frame->forward in the cone arm 579 (2797 B); the cone half extents as one point2f
// 480 (2813 B). Lane 1 r84's lead re-measured 288; the private normalize is round 148's listed form (290).
// m3 lane 8 r164 (fast with 0xc1a80, MATCH each time; base 290). Retail sums every projection's rows in one order, i
// (fj vj + fi vi) + fk vk, j (li vi + lk vk) + lj vj, k (uk vk + ui vi) + uj vj, where ours varies per instance: those rows
// parenthesised 554 (2801 B), with the left row as (lk vk + li vi) + lj vj 554. The projection storing y before x 299 at
// retail's 2840 bytes (retail stores y first in every instance); on it the cone corners stored before the bounds, both
// in retail's x0, y0, x1, y1 order, 303 (2832 B: that tail's stores are then retail's, but the cone scale is still divided
// ahead of the centre projection), with the far edges as `half + centre` 303 (same code). All restored.
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
            long faces[6][4] = {
                { 0, 1, 2, 3 }, { 0, 3, 7, 4 }, { 0, 4, 5, 1 },
                { 1, 5, 6, 2 }, { 2, 6, 7, 3 }, { 4, 7, 6, 5 }
            };
            vector3f directions[6] = {
                { 0, 0, -1 }, { 1, 0, 0 }, { 0, 1, 0 },
                { -1, 0, 0 }, { 0, -1, 0 }, { 0, 0, 1 }
            };
            entry->depth = shape->sphere_render.radius;
            entry->orthogonal = false;
            entry->has_plane = false;
            vector3f_negate_ab(&directions[i], &entry->matrix.up);
            vector3f reference = *g_4687a8;
            if (i == 1 || i == 3)
                reference = *g_4687ac;
            cross3f_staged(&entry->matrix.up, &reference, &entry->matrix.left);
            normalize3d(&entry->matrix.left);
            cross3f_staged(&entry->matrix.left, &entry->matrix.up, &entry->matrix.forward);
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
            vector3f_negate_ab(&frame->forward, &entry->matrix.up);
            entry->matrix.position = shape->origin;
            entry->matrix.scale = 1.0f;
            entry->orthogonal = shape->kind == 1;
            entry->has_plane = shape->origin_offset > 0.0f;
            entry->plane_offset = shape->origin_offset;
            point3f plane_point;
            point_add_scaled_c35a0(&entry->matrix.position, &frame->forward, entry->plane_offset, &plane_point);
            entry->plane.n = frame->forward;
            entry->plane.d = dot_product_c1a80((vector3f const *)&plane_point, &entry->plane.n);
            entry->depth = shape->cone_render.far_distance + shape->origin_offset;
            point2f center;
            light_projection_point_ab(&entry->matrix, &shape->cone_render.direction, &center);
            real scale = 1.0f / shape->origin_offset;
            real half_width = shape->cone_render.width * scale * 0.5f;
            real half_height = shape->cone_render.height * scale * 0.5f;
            real x0 = center.x - half_width;
            real x1 = center.x + half_width;
            real y0 = center.y - half_height;
            real y1 = center.y + half_height;
            bounds.x0 = x0;
            bounds.x1 = x1;
            bounds.y0 = y0;
            bounds.y1 = y1;
            entry->points[0].x = x0; entry->points[0].y = y0;
            entry->points[1].x = x1; entry->points[1].y = y0;
            entry->points[2].x = x1; entry->points[2].y = y1;
            entry->points[3].x = x0; entry->points[3].y = y1;
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

void function_1cae40(s_cluster_partition *partition, long data_index, long *first_cluster_reference);

void __declspec(noinline) function_c3220(long light_index);

/* takes a light out of the cluster partition if it is in it (0xc3220; 0xc0110 inlines it) */
PRIVATE __forceinline void light_partition_clear(long light_index)
{
    s_light_ab *light = &((s_light_ab *)g_4e030c->data)[light_index & 0xffff];
    if (light->flags & 2)
    {
        function_1cae40(&g_4e0310_partition, light_index, &light->first_cluster_reference);
        light->flags &= ~8;
    }
}

// @retail 0xc3220
void function_c3220(long light_index)
{
    light_partition_clear(light_index);
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
            light_partition_clear(iterator.datum_index);
    }
}

// @retail 0xc3260
void __stdcall function_c3260(long light_index, bool clear_object_flag)
{
    s_light_delete_view *light = &((s_light_delete_view *)g_4e030c->data)[light_index & 0xffff];
    if (light->flags & 8)
        light_partition_clear(light_index);
    long object_index = light->object_index;
    if (object_index != NONE && clear_object_flag)
    {
        byte *object = (byte *)((s_light_object_header_ab *)g_4e0300->data)[object_index & 0xffff].object;
        bool remaining = false;
        s_record_pool_iterator iterator;
        iterator.data = g_4e030c;
        iterator.index = NONE;
        iterator.datum_index = NONE;
        s_light_delete_view *entry;
        while ((entry = (s_light_delete_view *)data_iterator_next_calling(&iterator)) != 0)
        {
            if (iterator.datum_index != light_index && entry->object_index == object_index)
            {
                remaining = true;
                break;
            }
        }
        dword *flags = (dword *)(object + 4);
        if (remaining)
            *flags |= 0x40;
        else
            *flags &= ~0x40;
    }
    record_pool_release(g_4e030c, light_index);
}

#include "object_queries.h"
#include "object_markers.h"
#include "matrix_transform.h"
#include "slot_handler.h"
struct s_partition_location;
void function_1cac60(dword const *bits, s_cluster_partition *partition, long data_index,
    long *first_cluster_reference, point3f const *point, real radius, s_partition_location const *location,
    long payload_size, void const *payload, bool *overflow);
bool first_person_weapon_get_marker(long object_index, long marker_name, point3f *position, vector3f *forward, vector3f *up);
bool __stdcall function_16a8e0(long flags, point3f const *point, real radius, long a, long b, point3f *out, real *distance);
void function_11bed0(s_location *location, point3f const *point);
point3f *transform4x3f_apply_point(transform4x3f const *matrix, point3f const *point, point3f *out);

// score 2 (was 2): exact in the reduced (--fast) image; in the full image the two reloads after the
// marker call (light_index from its stack home into EBP, the definition spill into EBX) come out in
// the other order, a scheduling tie that the contents of other files flip. Round 76 (m3 lane 10):
// the light and definition through typed record fields 234 fast (1032 bytes: the weapon-marker byte
// test becomes a dword shift-and-test); the same with that test written in the if 156 (1007 bytes);
// the one-use weapon-marker bool removed alone 156 (1007); the marker declared in the else arm 8
// fast (the forward load moves ahead of the position copy); the parameter-address local declared
// before the marker 2 plain (the same tie).
// @retail 0xc2d00
void __stdcall function_c2d00(long light_index)
{
    s_object_marker marker;
    long const *light_reference = &light_index;
    byte *light = g_4e030c->data + (*light_reference & 0xffff) * 0x110;
    byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
    word flags = *(word *)(light + 2);
    if (!(flags & 2))
        return;
    if (flags & 4)
        function_c0840((point3f *)(light + 0x84), (point3f *)(light + 0x90),
            (point3f *)(light + 0xa0), (real *)(light + 0x9c));
    short marker_index = *(short *)(light + 0x54);
    if (marker_index != NONE)
    {
        long marker_name = function_b8c40(*(long *)(light + 0x4c), marker_index);
        bool uses_weapon_marker = (definition[0] & 0x20) != 0;
        if (uses_weapon_marker &&
            first_person_weapon_get_marker(*(long *)(light + 0x4c), marker_name,
                (point3f *)(light + 0x84), (vector3f *)(light + 0xac), (vector3f *)(light + 0xb8)))
        {
            point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
        }
        else
        {
            function_b8d30(*(long *)(light + 0x4c), marker_name, &marker, 1, false);
            point3f *position = (point3f *)(light + 0x84);
            *position = marker.matrix.position;
            *(vector3f *)(light + 0xac) = marker.matrix.forward;
            *(vector3f *)(light + 0xb8) = marker.matrix.up;
            point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
        }
    }
    else
    {
        long parent_index = *(long *)(light + 0x58);
        if (parent_index != NONE)
        {
            s_slot_object_view *object = object_get(parent_index);
            transform4x3f *matrix = object_node_matrix(object, *(short *)(light + 0x5c));
            transform4x3f_apply_point(matrix, (point3f *)(light + 0x60), (point3f *)(light + 0x84));
            matrix_transform_vector(matrix, (vector3f *)(light + 0x6c), (vector3f *)(light + 0xac));
            matrix_transform_vector(matrix, (vector3f *)(light + 0x78), (vector3f *)(light + 0xb8));
            point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
        }
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
        long owner_index = *(long *)(light + 0x4c);
        if (owner_index != NONE)
            object_get_root_location(owner_index, location);
        else
        {
            long parent_index = *(long *)(light + 0x58);
            if (parent_index != NONE)
                object_get_root_location(parent_index, location);
            else
                function_11bed0(location, (point3f *)(light + 0x18));
        }
    }
    bool overflow;
    function_1cac60(NULL, &g_4e0310_partition, light_index, (long *)(light + 0x10), (point3f *)(light + 0x18),
        *(real *)(light + 0x24), (s_partition_location *)location, 0, NULL, &overflow);
    light[2] |= 8;
}

#include "data_array.h"

void function_141ce0(real a, real b, real c, transform4x3f *out);
real function_30bf0(vector3f *vector);
void __stdcall function_c0c80(long light_index);

#define FLAG(bit) (1 << (bit))
#define SET_FLAG(flags, bit, value) ((value) ? ((flags) |= FLAG(bit)) : ((flags) &= ~FLAG(bit)))

/* creates the light of a scenario placement: its frame from the placement's
   rotation, aimed at the placement's target, and its far point; NONE when the
   pool is full or the light is not wanted */
// @retail 0xc0520
long __stdcall function_c0520(long tag_index, long placement_index, bool force,
    s_light_placement_ab *placement)
{
    byte const *source = (byte const *)placement;
    byte const *definition = g_4e3b44[tag_index & 0xffff].bytes;
    long result = NONE;
    bool create = true;
    if ((definition[0] & 1) || (*(short const *)(source + 0x40) == 3 && !force))
        create = false;
    if (*(long const *)(definition + 0x94) != NONE || create)
    {
        result = record_pool_allocate(g_4e030c);
        if (result != NONE)
        {
            byte *light = g_4e030c->data + (result & 0xffff) * 0x110;
            real distance = 0.0f;
            *(long *)(light + 8) = placement_index;
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
            SET_FLAG(*(word *)(light + 2), 5, source[0x3e] & 4);
            *(long *)(light + 0x10) = NONE;
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
            point3f const *source_point = (point3f const *)(source + 8);
            vector3f direction;
            direction.i = *(real const *)(source + 0x4c) - source_point->x;
            direction.j = *(real const *)(source + 0x50) - source_point->y;
            direction.k = *(real const *)(source + 0x54) - source_point->z;
            real length = function_30bf0(&direction);
            if (!(length != 0.0f))
                direction = matrix.up;
            else
            {
                if ((short)shape.kind != 0 && dot_product_c1a80(forward, &direction) < 0.5f)
                    direction = matrix.up;
            }
            if ((short)shape.kind == 0)
                distance = shape.sphere.radius_a > shape.sphere.radius_b ? shape.sphere.radius_a : shape.sphere.radius_b;
            else
                distance = dot_product_c1a80(forward, &direction) * shape.cone.values[4];
            point_add_scaled_c35a0(position, &direction, distance, (point3f *)(light + 0x90));
            if (!(definition[0] & 9))
            {
                point_add_scaled_c35a0(position, &direction, 0.001f, (point3f *)(light + 0x84));
            }
            function_c0c80(result);
            *(long *)(light + 0xc) = g_4e0308 - 1;
            *(long *)(light + 0x10c) = NONE;
        }
    }
    return result;
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
    bool force = mode == 5 || mode == 4;
    long *count = &scenario->placement_count;
    for (long i = 0; i < *count; ++i)
    {
        s_light_placement_ab *placement = &((s_light_setup_scenario_ab *)g_4e0350)->placements[i];
        long palette_index = placement->palette_index;
        if (palette_index != NONE && palette_index >= 0 &&
            palette_index < ((s_light_setup_scenario_ab *)g_4e0350)->palette_count)
        {
            s_light_palette_setup_ab const *entry =
                &((s_light_setup_scenario_ab *)g_4e0350)->palette[palette_index];
            if (entry->tag_index != NONE)
                function_c0520(entry->tag_index, i, force, placement);
        }
    }
}


bool __stdcall function_bab40(long object_index, long name, real *value);
bool function_bad50(long object_index, long index, point3f *out);
color3f *function_131c20(color3f const *a, color3f const *b, dword flags, real t, color3f *result);
color3f *function_131d60(color4f const *a, color4f const *b, dword flags, color3f *result,
    color3f const *tint, real t);
long function_baf80(long object_index);
bool object_or_parent_hidden(long object_index);
bool function_b9d20(long object_index);
struct s_object_list;
extern s_object_list *g_4de2f4;


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
        color3f tint;
        color4f second, first, pair_second, pair_first;
        tint = *(color3f *)g_468710;
        *(color3f *)&first.red = *(color3f *)(definition + 0x58);
        *(color3f *)&second.red = *(color3f *)(definition + 0x64);
        second.alpha = 1.0f;
        first.alpha = 1.0f;
        *(color3f *)&pair_first.red = *(color3f *)(definition + 0x40);
        *(color3f *)&pair_second.red = *(color3f *)(definition + 0x4c);
        pair_second.alpha = 1.0f;
        pair_first.alpha = 1.0f;
        real *fraction = &light->fraction;
        function_bab40(object_index, light->name, fraction);
        function_bad50(object_index, light->colour_index, (point3f *)&tint);
        function_131d60(&first, &second, *(dword *)(definition + 0x34),
            &light->colour, &tint, *fraction);
        function_131d60(&pair_first, &pair_second, *(dword *)(definition + 0x34),
            &light->pair_colour, &tint, light->fraction);
        real blend = light->fraction;
        light->intensity = (1.0f - blend) *
            *(real *)(definition + 0x70) + *(real *)(definition + 0x74) * blend;
    }
    else if (light->flags & 0x10)
    {
        real elapsed = (real)(g_510c54->game_time - light->start_time);
        real duration = (real)g_510c54->field_2_3 * *(real *)(definition + 0xb0);
        real fraction = function_17ca10(elapsed / duration, *(short *)(definition + 0xb6));
        light->fraction = (1.0f - fraction) * light->scale;
        function_131c20((color3f *)(definition + 0x58), (color3f *)(definition + 0x64),
            *(dword *)(definition + 0x34), light->fraction, &light->colour);
        function_131c20((color3f *)(definition + 0x40), (color3f *)(definition + 0x4c),
            *(dword *)(definition + 0x34), light->fraction, &light->pair_colour);
        real blend = light->fraction;
        light->intensity = (1.0f - blend) *
            *(real *)(definition + 0x70) + *(real *)(definition + 0x74) * blend;
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
            !(g_4e3b44[light->tag_index & 0xffff].bytes[0] & 0x80))
        {
            real fade = PIN(*(real *)(parent + 0x2b0), 0.0f, 1.0f);
            light->fade = fade;
            real scale = 1.0f - fade;
            light->colour.red *= scale;
            light->colour.green *= scale;
            light->colour.blue *= scale;
            light->pair_colour.red *= scale;
            light->pair_colour.green *= scale;
            light->pair_colour.blue *= scale;
        }
    }
    light->colour.red = PIN(light->colour.red, 0.0f, 1.0f);
    light->colour.green = PIN(light->colour.green, 0.0f, 1.0f);
    light->colour.blue = PIN(light->colour.blue, 0.0f, 1.0f);
    light->pair_colour.red = PIN(light->pair_colour.red, 0.0f, 1.0f);
    light->pair_colour.green = PIN(light->pair_colour.green, 0.0f, 1.0f);
    light->pair_colour.blue = PIN(light->pair_colour.blue, 0.0f, 1.0f);
    real intensity = light->intensity * 2.0f;
    light->intensity = PIN(intensity, 0.0f, 4.0f);
    light->radius = (1.0f - light->fraction) * *(real *)(definition + 8) +
        *(real *)(definition + 0xc) * light->fraction;
    if ((light->flags & 1) && light->radius > 0.0f &&
        (light->colour.red != 0.0f || light->colour.green != 0.0f || light->colour.blue != 0.0f ||
            light->pair_colour.red != 0.0f || light->pair_colour.green != 0.0f || light->pair_colour.blue != 0.0f))
    {
        if (!(light->flags & 2))
        {
            light->flags |= 2;
            if ((object_index == NONE || (!object_or_parent_hidden(object_index) && function_b9d20(object_index))) &&
                g_4de2f4 && *(byte *)g_4de2f4)
                function_c2d00(light_index);
        }
    }
    else if (light->flags & 2)
    {
        if (light->flags & 8) function_c3220(light_index);
        light->flags &= ~2;
    }
}


void function_1cabc0(s_cluster_partition *partition, char const *name, long payload_size);

// @retail 0xbffa0
void function_bffa0()
{
    s_record_pool *lights = data_new_inlined("lights", 0x15e, 0x110, 0, g_510c2c);
    g_4e030c = lights;
    long size = 12;
    byte *allocation = game_state_globals.base_address + game_state_globals.cpu_allocation_size;
    game_state_globals.cpu_allocation_size += size;
    function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));
    g_5107e8 = (s_5107e8 *)allocation;
    g_5107e8->flag8 = true;
    if (lights)
    {
        function_1cabc0(&g_4e0310_partition, "light", 0);
    }
}


// @retail 0xc1720
long __stdcall function_c1720(long object_index, byte remove_from_partition, byte update)
{
    long count = 0;
    s_data_datum_iterator iterator;
    iterator.data = g_4e030c;
    iterator.datum_index = NONE;
    iterator.index = NONE;
    while (data_datum_iterator_next_inlined(&iterator))
    {
        if (*(long *)(iterator.datum + 0x58) == object_index)
        {
            if (remove_from_partition)
                light_partition_clear(iterator.datum_index);
            if (update)
                function_c2d00(iterator.datum_index);
            ++count;
        }
    }
    return count;
}


struct s_c0230_flags
{
    word low : 4;
    word bit4 : 1;
    word high : 11;
};

// @retail 0xc0230
long __stdcall function_c0230(long tag_index, long object_index, short node_index, long name, short value)
{
    byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
    long result = NONE;
    bool permitted = g_4e6948->state == 5 || g_4e6948->state == 4;
    bool limited = *(short *)(definition + 0xa4) == 2;
    if ((definition[0] & 1) && *(long *)(definition + 0x94) == NONE)
        goto done;
    if (!permitted && limited)
        goto done;
    result = record_pool_allocate(g_4e030c);
    if (result != NONE)
    {
        byte *light = g_4e030c->data + (result & 0xffff) * 0x110;
        *(long *)(light + 4) = tag_index;
        *(long *)(light + 0x4c) = object_index;
        word *flags = (word *)(light + 2);
        *flags = 0;
        *(short *)(light + 0x54) = node_index;
        *(long *)(light + 0x50) = name;
        *(long *)(light + 0x14) = NONE;
        *(long *)(light + 8) = NONE;
        *(short *)(light + 0x56) = value;
        *(long *)(light + 0x58) = NONE;
        *(short *)(light + 0x5c) = (short)NONE;
        *(real *)(light + 0xd0) = 1.0f;
        *(real *)(light + 0xcc) = 1.0f;
        byte flag = (byte)((~definition[0]) & 1);
        *flags = (word)flag;
        ((s_c0230_flags *)(light + 2))->bit4 = 0;
        *(long *)(light + 0x10) = NONE;
        function_c0c80(result);
        *(long *)(light + 0xc) = g_4e0308 - 1;
        *(long *)(light + 0x10c) = NONE;
    }
done:
    return result;
}



/* updates every light; a flag 8 light whose object (or its second object)
   has header bit 2 set leaves the cluster partition and goes to 0xc2d00, and
   a flag 0x10 light past its lifetime goes to 0xc3260 */
// @retail 0xc1400
void function_c1400()
{
    long time = g_510c54->game_time;
    s_record_pool *pool = g_4e030c;
    long index = data_datum_index(pool, function_16bc00(pool, 0));
    while (index != NONE)
    {
        byte *light = g_4e030c->data + (index & 0xffff) * 0x110;
        byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
        function_c0c80(index);
        if (light[2] & 8)
        {
            bool object_flagged;
            if (*(long *)(light + 0x4c) != NONE)
                object_flagged = (bool)(((dword)(g_4e0300->data + (*(long *)(light + 0x4c) & 0xffff) * 12)[2] >> 2) & 1);
            else if (*(long *)(light + 0x58) != NONE)
                object_flagged = (bool)(((dword)(g_4e0300->data + (*(long *)(light + 0x58) & 0xffff) * 12)[2] >> 2) & 1);
            else
                object_flagged = false;
            if (object_flagged)
            {
                light_partition_clear(index);
                function_c2d00(index);
            }
        }
        if ((light[2] & 0x10) && (real)(time - *(long *)(light + 0x14))
            > (real)*(short *)((byte *)g_510c54 + 2) * *(real *)(definition + 0xb0))
            function_c3260(index, true);
        index = data_datum_index(g_4e030c, data_find_index(g_4e030c, index == NONE ? 0 : (index & 0xffff) + 1));
    }
}



// @retail 0xc0350
long function_c0350(long tag_index, long object_index, long node_index, point3f const *position,
    vector3f const *forward, vector3f const *up, real scale)
{
    long result = record_pool_allocate(g_4e030c);
    if (result != NONE)
    {
        byte *light = g_4e030c->data + (result & 0xffff) * 0x110;
        byte *definition = g_4e3b44[tag_index & 0xffff].bytes;
        *(word *)(light + 2) = 0;
        *(long *)(light + 4) = tag_index;
        *(long *)(light + 0x14) = g_510c54->game_time;
        *(long *)(light + 0x50) = 0;
        *(short *)(light + 0x56) = 0;
        *(long *)(light + 8) = NONE;
        *(long *)(light + 0x4c) = NONE;
        *(short *)(light + 0x54) = NONE;
        *(real *)(light + 0xd0) = scale;
        *(real *)(light + 0xcc) = scale;
        *(word *)(light + 2) = 1;
        if (*(real *)(definition + 0xb0) > 0.0f)
            *(word *)(light + 2) = 0x11;
        else
            *(word *)(light + 2) = 1;
        *(long *)(light + 0x10) = NONE;
        if (object_index == NONE)
        {
            *(point3f *)(light + 0x84) = *position;
            ((point3f *)(light + 0x90))->x = position->x + forward->i;
            ((point3f *)(light + 0x90))->y = position->y + forward->j;
            ((point3f *)(light + 0x90))->z = position->z + forward->k;
            *(vector3f *)(light + 0xac) = *forward;
            *(vector3f *)(light + 0xb8) = *up;
            *(long *)(light + 0x58) = NONE;
            *(short *)(light + 0x5c) = NONE;
        }
        else
        {
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            *(point3f *)(light + 0x60) = *position;
            *(vector3f *)(light + 0x6c) = *forward;
            *(vector3f *)(light + 0x78) = *up;
            *(long *)(light + 0x58) = object_index;
            *(short *)(light + 0x5c) = (short)node_index;
            *(dword *)(object + 4) |= 0x40;
        }
        function_c0c80(result);
        *(long *)(light + 0xc) = g_4e0308 - 1;
        *(long *)(light + 0x10c) = NONE;
    }
    return result;
}
