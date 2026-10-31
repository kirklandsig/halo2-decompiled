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
bool function_2dba0(long tag, vector3f const *direction, bool alternate, long kind, long index, long marker_index, point3f const *position, color3f const *color, real alpha, real amount, real scale);
void function_42850(long object_index, long tag_index, point3f const *position,
    vector3f const *first, vector3f const *second, real scale, real width,
    vector3f const *third);
byte g_55e708;

// score 109 (was 109): BLOCKED-BY 0x2dba0: it takes the position in ecx and the tag on the stack where retail
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
                    function_2dba0(*(long *)(definition + 0x94), &marker->matrix.forward, function_3e9c0(*(long *)(light + 0x4c)), 2, absolute_index, i, &marker->matrix.position, (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f);
                    ++i;
                    ++marker;
                }
            }
            else
                function_2dba0(*(long *)(definition + 0x94), (vector3f *)(light + 0xac), function_3e9c0(*(long *)(light + 0x4c)), 2, absolute_index, 0, (point3f *)(light + 0x84), (color3f *)(light + 0xd4), 1.0f - *(real *)(light + 0xc8), *(real *)(light + 0xd0), 1.0f);
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

// score 482 (was 482): (513 before round 91) the entry pointer's base (+0x4c), the direction cursor and the table stores
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
// with depth from the radius 610; the projection's vector as one struct copy 624 (2892 bytes); one scale
// local for both projection scales 500; the inner scale named `inverse` 482 (identical); the scale as
// three divides 599. Diagnostic: dead initialised locals change it by count alone (+1..+12:
// 479/479/539/315/483/480/480/480/479/479/483/483, any position); +4 gives 2826 bytes and fixes most of
// the projection rows, so a natural form with that many more symbols is the lead.
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
            entry->plane.d = dot_product_c1a80(&entry->plane.n, (vector3f const *)&plane_point);
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

// score 6 (was 6): the last flag update holds the object in ecx and the flags in eax, retail the
// reverse. light_partition_clear after the & 8 test with the light from g_4e030c->data and the pool
// read at each use (kept); the same with a pool local 59, through the s_light_ab pointer 80; flags
// through a ternary, a typed object, a dword view, a pointer to the flags 6/6/6/6; flag test negated 7;
// object fetched after the flag 8; datum iterator 6; entry declared in the loop 6; a 600 s permuter
// run (59 tries), none below 6. Round 72: `|=`/`&=` on the object's flags in each arm 68 (the
// result test is threaded away); the flags read in each arm 58; the iterator declared before the
// object 6; the flags as `((dword *)object)[1]` 6; the clear arm first 7; `dword flags = 0;`
// declared before the object 6. Round 95: the loop leaving through `!remaining` in its condition 6;
// the object read as `*(byte **)(g_4e0300->data + i * 12 + 8)` (the file's form) 6, with `remaining`
// declared before it 8; a set-or-clear ternary on the field 68; the light index read directly instead
// of through a reference local 6 (kept). Round 52: explicit per-arm stores, direct per-arm stores,
// and direct stores with an index reference 87/87/87; a direct store ternary 6; the object or flags
// live at entry 6/6; flags declared before the object 6; separate original and updated flag values 6.
// Round 26: the object typed as the file's light object with `flag6` set or cleared in each arm 68,
// `flag6 = remaining` 60; the header read through the file's object header type (kept) 6; signed
// flags 6; the object through an inline getter 6.
// Round 91 (lane 7, harness exact): the object declared at function scope 6; the entry as bytes 6;
// the flags masked then set when remaining 45; a plain `while (entry = ...)` 6; the iterator declared
// before `remaining` 6; the object index read from the light at the lookup / the loop test / the
// guard 6/6/6.
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
        dword flags = *(dword *)(object + 4);
        if (remaining)
            flags |= 0x40;
        else
            flags &= ~0x40;
        *(dword *)(object + 4) = flags;
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

// score 38 (was 38): only the node-matrix arm's rotations differ (231 before earlier rounds; one round
// reached score 40 (was 38) when the else arm also reloaded ebx before ebp after the marker call, where retail reloads
// ebp first). Ours folds the forward.i load into
// only the node-matrix arm's rotations differ (231 when first written; at 40 the
// else arm also reloaded ebx before ebp after the marker call where retail reloads ebp first). Ours
// folds the forward.i load into
// the i product of both (`movaps; mulss [m + 4]`, a byte shorter each, which shifts every later jump and
// call by two bytes) and sums the second rotation's rows in another order (its vector loaded i first);
// retail compiles both rotations alike (up*k + left*j, then forward*i; vector loaded j, k, i). Round 141:
// the partition as the global itself, the rotations through matrix_transform_vector, the node matrix
// through object_node_matrix(object_get()) and the far point through point_add_scaled_c35a0 in each arm
// (once after the marker test 295; a new two-pointer add helper 70). Node matrix through the out-of-line
// 0xb8bd0 aligned 71 but stays a call (/Ob1 file; inline/forceinline redeclarations ignored); position/
// forward/up pointer locals aligned 127/114; named rotation outputs/inputs 88/157; const matrix 72; the
// helper's sums in up-first text order 70 (canonicalised). Round 143: diagnostics on the shared header
// (restored): vector-first products 70 (identical), components read at each use 153; const vector
// arguments 70; a file-private up-first normal transform returning its output (the form other files
// keep) 70 (identical); the parameter alias dropped 158. Round 147: whichever rotation comes second gets
// the other order (diagnostics: the two swapped, or a call between them); the arm's locals declared at
// the top 70 (identical); a named object pointer 72; a named node index 72; the axes as two-element
// vector arrays 86; a `__forceinline` redeclaration of the rotation helper 70 (identical). Round 81: the
// marker local scoped to its arm 78; the overflow flag declared first 70 (same code); forward/up output
// pointer locals 86; the location pointer taken at the top 155; the marker flag tested as a byte 70
// (same code); diagnostics on the shared header (restored): the helper's components loaded j,k,i /
// k,j,i / i,k,j / j,i,k / k,i,j 78/76/78/75/81 (i,j,k stays best). Round 38: an explicit final return
// and the marker after the early guard 70 each; a rotation for loop 158 (1005 bytes); a local matrix
// 306 (1067); rotations before the point transform 124 (1027); the returned position 77; the existing
// scaled rotation callee 391 (893); a rotation do loop 286 (890). Round 80 (the tail now matches, 38):
// a named parent index 40; a forward / position pointer local in the node arm 42/44, in the marker arm
// 198/210. LTCG harness diagnostics of the arm (not kept): the second rotation takes the
// first's form when the marker arms' far-point adds are removed, and the fold goes when the marker call
// is removed; the rotations written out in the arm give both retail's form apart from the fold; helper
// parameter orders, reference parameters, by-value components or results, an orientation helper and
// the existing vector3f_rotate_into_axes change nothing or lose more.
// only the node arm's two rotations differ: retail compiles both alike (sum up*k + left*j,
// then forward*i, the matrix element loaded first; vector loaded j, k, i); ours folds the first one's forward.i
// product (`movaps; mulss [m + 4]`) and orders the second one i, k, j (the one-byte shift also flags the later
// jumps and calls). The ranks are function-wide: in a compile-only screen the second rotation turns retail's
// when the point transform call before it is removed or the matrix comes back from the out-of-line 0xb8bd0
// (diagnostics only; that is a call, and 0xb8bd0's file is /Ob1). Round 141: the partition as the global itself,
// the rotations through matrix_transform_vector, the node matrix through object_node_matrix(object_get()) and
// the far point through point_add_scaled_c35a0 in each arm (once after the marker test 295; a new two-pointer
// add helper 70). Node matrix through the out-of-line 0xb8bd0 aligned 71 but stays a call (/Ob1 file;
// inline/forceinline redeclarations ignored); position/forward/up pointer locals aligned 127/114; named rotation
// outputs/inputs 88/157; const matrix 72; the helper's sums in up-first text order 70 (canonicalised). Round
// 143: diagnostics on the shared header (restored): vector-first products 70 (identical), components read at
// each use 153; const vector arguments 70; a file-private up-first normal transform returning its output (the
// form other files keep) 70 (identical); the parameter alias dropped 158. Round 147: whichever rotation comes
// second gets the other order (diagnostics: the two swapped, or a call between them); the arm's locals declared
// at the top 70 (identical); a named object pointer 72; a named node index 72; the axes as two-element vector
// arrays 86; a `__forceinline` redeclaration of the rotation helper 70 (identical). Round 81: the marker local
// scoped to its arm 78; the overflow flag declared first 70 (same code); forward/up output pointer locals 86;
// the location pointer taken at the top 155; the marker flag tested as a byte 70 (same code); diagnostics on
// the shared header (restored): the helper's components loaded j,k,i / k,j,i / i,k,j / j,i,k / k,i,j
// 78/76/78/75/81 (i,j,k stays best). Round 38: an explicit final return and the marker after the early guard 70
// each; a rotation for loop 158 (1005 bytes); a local matrix 306 (1067); rotations before the point transform
// 124 (1027); the returned position 77; the existing scaled rotation callee 391 (893); a rotation do loop 286
// (890). Round 10 (lane 14): the file's s_light_ab type for the light 38; const matrix with the definition
// flag as a byte 38; the word flag tests as bytes 46; the existing function_2477b1 rotation 306; an
// s_light_frame_ab pointer for the frame fields 352. Original body restored.
// Round 12 (lane 14): the node matrix through the out-of-line 0xb8bd0 184 (1002 bytes); with `inline` on
// 0xb8bd0's definition 38 (identical, but 0xbcc20/0x11b710 then inline it too); the rotation inputs
// through one `vector3f const *` axes pointer 40; the light re-read for the second rotation 266; the node
// matrix fetched again for the second rotation 304. Compile-only screens (no gain): second output via a
// local, second input copied first, both inputs copied before the point call, both outputs via locals.
// Lane 18 r1: the light index read through the alias at every later use (16a8e0, 1cac60 too) 38 (gives
// retail's ebp/ebx reload order; a 2-point gain, not kept); c28b0 called with the parameter itself 154;
// the second rotation through function_2477b1 256, the first 243; the point transform between the two
// rotations 54.
// only the node-matrix arm's two rotations differ. Ours folds the forward.i load
// into the i product of both (`movaps; mulss [m + 4]`, a byte shorter each, which shifts every later
// jump and call) and computes the second rotation in another order (its vector loaded i first);
// retail compiles both rotations alike (up*k + left*j, then forward*i, the matrix element loaded
// first; vector loaded j, k, i). Cause of the second-rotation order: the helper's per-call matrix
// parameter. A compile-only build of the file reproduces the arm exactly; there the helper's body
// written out in the arm with the caller's `matrix` gives both rotations retail's form apart from
// the fold (4 differing SSE lines against 34; under LTCG 20, 6 aligned), while a block-local copy of
// the matrix pointer in both rotations, or any helper call (inline/static/forceinline, const or
// not, any parameter order, by reference, returning its output, matrix3x3 or row pointers, an
// LTCG-inlined plain function, a two-vector helper) gives our form. The fold stays in every
// written term and operand order (canonicalised).
// Tried: round 141: the partition as the global itself, the rotations through
// matrix_transform_vector, the node matrix through object_node_matrix(object_get()) and the far
// point through point_add_scaled_c35a0 in each arm (once after the marker test 295; a new
// two-pointer add helper 70). Node matrix through the out-of-line 0xb8bd0 aligned 71 but stays a
// call (/Ob1 file; inline/forceinline redeclarations ignored); position/forward/up pointer locals
// aligned 127/114; named rotation outputs/inputs 88/157; const matrix 72; the helper's sums in
// up-first text order 70 (canonicalised). Round 143 (shared header, restored): vector-first
// products 70 (identical), components read at each use 153; const vector arguments 70; a
// file-private up-first normal transform returning its output 70 (identical); the parameter alias
// dropped 158. Round 147: whichever rotation comes second gets the other order (the two swapped,
// or a call between them); the arm's locals declared at the top 70 (identical); a named object
// pointer 72; a named node index 72; the axes as two-element vector arrays 86; a `__forceinline`
// redeclaration of the rotation helper 70 (identical). Round 81: the marker local scoped to its
// arm 78; the overflow flag declared first 70 (same code); forward/up output pointer locals 86;
// the location pointer taken at the top 155; the marker flag tested as a byte 70 (same code);
// helper components loaded j,k,i / k,j,i / i,k,j / j,i,k / k,i,j 78/76/78/75/81 (shared header,
// restored). Round 38: an explicit final return and the marker after the early guard 70 each; a
// rotation for loop 158 (1005 bytes); a local matrix 306 (1067); rotations before the point
// transform 124 (1027); the returned position 77; the existing scaled rotation callee 391 (893);
// a rotation do loop 286 (890). Round 80 (the tail now matches, 38): a named parent index 40; a
// forward / position pointer local in the node arm 42/44, in the marker arm 198/210. Round 10
// (lane 14): the file's s_light_ab type for the light 38; const matrix with the definition flag
// as a byte 38; the word flag tests as bytes 46; the existing function_2477b1 rotation 306; an
// s_light_frame_ab pointer for the frame fields 352. Round 12 (lane 14): the node matrix through
// the out-of-line 0xb8bd0 184 (1002 bytes); with `inline` on 0xb8bd0's definition 38 (identical,
// but 0xbcc20/0x11b710 then inline it too); the rotation inputs through one `vector3f const *`
// axes pointer 40; the light re-read for the second rotation 266; the node matrix fetched again
// for the second rotation 304; compile-only screens (no gain): second output via a local, second
// input copied first, both inputs copied before the point call, both outputs via locals. Lane 13
// round 18 (fast counts, 38 here): a plain auto-inlined copy of the helper 38 (same code); const
// matrix local 38; the rotations swapped 48; the up input copied into a local first 190.
// Round 91 (lane 7; harness counts, where the unchanged source gives 38; the real build's two
// extra are a marker-arm reload pair that drifts with LTCG context): the definition typed as the
// file's light definition 38; the node arm's object read from the header table directly 40; the
// light parameter passed directly to 0xc28b0 154; the far point added before the second rotation
// 220; a `word flags` local for the entry tests 40; a rotation helper taking the vector by value 235
// or returning it 268.
// Round 32 (lane 10, fast counts): a file-local copy of the rotation helper 38 (same code), with its
// component locals renamed (x/y/z, a/b/c, z/y/x, c/b/a, vi/vj/vk) 38 each (names do not move the
// order), with const component locals 38, with a `vector3f v = *vector` copy 235 (1077 bytes); the
// node matrix as a `transform4x3f *const` local 38.
// Round 175 (one-file LTCG harness, 38 for the unchanged source; plain where marked): the location
// pointer dropped (a cast at each use) 40; the marker name declared at the function top 38; the
// distance declared at the function top 38; the rotations through the matrix's rotation member 38;
// the three far points through the shared vector3f_add 40 (plain). Diagnostics: a matrix struct
// without the union 38, a matrix3x3 rotation helper 45; dead initialised locals change it by count
// alone (+1..+5: 40/56/56/60/60, any position or type) and the fold stays at every count, and also
// without the first-person marker call, without 0xb8d30 or without 0xc28b0.
// Round 47 (lane 16, fast with 0xc0840/0xc28b0/0x14a280; unchanged by 0xc0840's new body): the node
// arm's far point through the existing vector3f_add_scaled 38 (same code); both rotations through the
// existing vector3f_rotate_into_axes on the rotation member 525 (759 bytes); the light index read
// through the alias at the 0x16a8e0 and 0x1cac60 calls too 38 (same code here); the second / first
// rotation alone through vector3f_rotate_into_axes 388 / 390 (894 bytes).
// Round 51 (lane 16; compile-only screen of this file, which reproduces the node arm: 19 differing SSE
// lines for the unchanged source): a file-local rotation helper in all 6 input-load orders x 3 row
// orders 19..54 (i, j, k loads and i, j, k rows stay best); with up-first rows (0x163db0's matched plane
// rows) 19 (same code); unused reads of the node matrix axes before the rotations 19/28 (diagnostic);
// the node arm first in source (marker test inverted) 44.
// Round 19 (lane 2, fast with 0xc0840/0xc28b0/0x14a280): the node matrix declared at the function
// top 38; both rotation outputs addressed from one forward pointer (`forward + 1` for up) 40; the far
// point from apply_point's returned position 45; the node arm as a forceinline helper taking the light
// 40; the body under one enabled test instead of the early return 38 (same code).
// @retail 0xc2d00
void __stdcall function_c2d00(long light_index)
{
    s_object_marker marker;
    long const *light_reference = &light_index;
    byte *light = g_4e030c->data + (*light_reference & 0xffff) * 0x110;
    byte *definition = g_4e3b44[*(long *)(light + 4) & 0xffff].bytes;
    if (!(*(word *)(light + 2) & 2))
        return;
    if (*(word *)(light + 2) & 4)
        function_c0840((point3f *)(light + 0x84), (point3f *)(light + 0x90),
            (point3f *)(light + 0xa0), (real *)(light + 0x9c));
    if (*(short *)(light + 0x54) != NONE)
    {
        long marker_name = function_b8c40(*(long *)(light + 0x4c), *(short *)(light + 0x54));
        if ((*(long *)definition & 0x20) &&
            first_person_weapon_get_marker(*(long *)(light + 0x4c), marker_name,
                (point3f *)(light + 0x84), (vector3f *)(light + 0xac), (vector3f *)(light + 0xb8)))
        {
            point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
        }
        else
        {
            function_b8d30(*(long *)(light + 0x4c), marker_name, &marker, 1, false);
            *(point3f *)(light + 0x84) = marker.matrix.position;
            *(vector3f *)(light + 0xac) = marker.matrix.forward;
            *(vector3f *)(light + 0xb8) = marker.matrix.up;
            point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
        }
    }
    else if (*(long *)(light + 0x58) != NONE)
    {
        transform4x3f *matrix = object_node_matrix(object_get(*(long *)(light + 0x58)), *(short *)(light + 0x5c));
        transform4x3f_apply_point(matrix, (point3f *)(light + 0x60), (point3f *)(light + 0x84));
        matrix_transform_vector(matrix, (vector3f *)(light + 0x6c), (vector3f *)(light + 0xac));
        matrix_transform_vector(matrix, (vector3f *)(light + 0x78), (vector3f *)(light + 0xb8));
        point_add_scaled_c35a0((point3f *)(light + 0x84), (vector3f *)(light + 0xac), 1.0f, (point3f *)(light + 0x90));
    }
    function_c28b0(*light_reference);
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
color3f *function_131d60(color4f const *a, color4f const *b, dword flags, real t,
    color3f *result, color3f const *tint);
long function_baf80(long object_index);
bool object_or_parent_hidden(long object_index);
bool function_b9d20(long object_index);
struct s_object_list;
extern s_object_list *g_4de2f4;

#define PIN(x, low, high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

// score 49 (was 49): four bytes over retail's length; only the colour-copy block differs: retail
// hoists each next definition load above the previous copy's last store, loads 1.0f before the tint
// stores and stores the last pair-colour dword after the pushes, so `&light->fraction` takes EBX and
// is read back as `[ebx]` after the calls (ours stores each copy's last dword first, takes EAX and
// reloads the field). Round 95: the end tests and `|= 2` on the word field 184 (aligned 46 -> 43);
// with the definition test as `!(bytes[0] & 0x80)` (gives retail's `js`) 184 (aligned 42); fade
// products written `scale * field` for the last two or all three pair channels 184/184; vector3f_scale
// in place on both colours 298; the fade as a named local used twice 184. Round 141: the grouped
// colour struct and the parameter alias replaced by plain locals (LTCG then inlined 0x131d60: 607 at
// 1789 bytes) with 0x131d60 kept out of line by a noinline declaration in its file 147 (kept as the
// steering removal); on that base `!(bytes[0] & 0x80)` 151, the word-field end tests 183, both 185,
// the pair colour through vector3f_scale 281, `scale * field` for the pair 147, the colour through
// vector3f_scale 161. Round 143: every clamp through a PIN macro that re-reads its field (the channel
// clamps stored in order) 85 at retail's length; with the word-field end tests 51; with the `js` test
// and the clamped fade named 50; the alphas after the rgb copies in declaration order 49 (kept). On
// that: alphas before each copy / after in source order / first 50/50/49; tint copied last 66; memcpy
// copies 49; per-field float copies 414; `&light->fraction` passed directly 214; the fraction pointer
// declared at the block top 88; every fade product `*= 1.0f - light->fade` 50. Round 81 (aligned
// count 8: the 4 bytes are `[ebx]` against our `[ebp + 0xd0]`, which shifts every later call): RA
// gives the pointer eax (then rematerialised) because our scheduler stores the last pair-colour dword
// before the pushes; the colour struct given an rgb member (shared header, restored) 49 (same code);
// the pointer declared at the function top 93 (it then takes edi and the late store moves as
// retail's); the pointer also for the second call and/or the blend 49/49/49 (same code);
// `&light->fraction` passed with no pointer 214. Round 14 (lane 10): a typed definition struct for
// every offset 49 (identical code); alphas after each copy 50; pair copies after the second/first alphas
// 49; second/first alphas between the copies 49; the fraction pointer before the alphas / after the tint
// / between the alphas 49/75/49 (retail keeps it in EBX across both calls; ours rematerialises it).
// Round 62: a point3f tint 49; alpha-only aggregate initializers 438; the last pair colour copied as
// a whole color4f 365; a reused input pointer 357; a reused output pointer, chained alphas, a const
// fraction handle, separate declarations, function-scope colours and const copy sources 49 each.
// Round 16 (lane 10): the function-driven colour block as a forceinline helper of the light and
// definition 49 (identical code); with `&light->fraction` as its pointer parameter 88, and every
// fraction read through it 88; the fraction as a reference 49; the tint initialised at its
// declaration 49.
// Round 77: `real &fraction` 49 (same code); the tint copied after the first call 75; the colour copies
// after the second call 82; the pointer for the first call only (`light->fraction` after it) 214; the
// pointer declared with the colours 88. Round 80: the four alpha stores chained in one assignment 51
// (in reverse order 49); a const fraction pointer 49; the tint declared after the colours 49; the
// definition's function flags named for both calls 377.
// pointer declared with the colours 88. Round 10 (lane 14): the tint read through a `color3f const *`
// local 49; the four definition colours (0x40, 0x4c, 0x58, 0x64) as one `color3f *` array for the copies
// 417, and in every arm 370; `real *const fraction` 49; the function flags as a named local for both
// calls 399. Round 12 (lane 14): the fraction pointer at the function top 93 (computed at entry in EDI),
// also for the arm's later reads 93; at the arm's top with every read through it 88; every arm read through
// it 49 (same code); the four alpha stores before the copies 49 (same code).
// Lane 18 r1 (statement orders of the block, the rest unchanged): alphas between the tint and the copies
// 49; pair copies first with the tint after them 65; tint after the four copies 65; copies 2, 1, 4, 3 with
// alphas likewise 58; copies 1, 3, 2, 4 55; the fraction pointer before the copies 75; alphas reversed 51.
// it 49 (same code); the four alpha stores before the copies 49 (same code). Lane 13 round 18:
// the whole count is the fraction pointer (`[ebx]` against our `[ebp + 0xd0]`, 4 bytes, shifting
// every later call) and the copy-store schedule that frees EAX for it; a compile-only build does
// not reproduce this block (it gives EBX), so it is an LTCG effect. Pointer as a byte offset from
// the light 49, as an element of the radius block 49 (same code); re-derived from the pool global
// 282; `&light->fraction` passed to 0xbab40 with the pointer kept for the later read 49, the
// pointer declared between the two calls 49 (same code).
// Lane 14 r19 (fast with the callees listed reproduces 49; 0xbab40 gets retail's three pushes and
// nothing scratch lives across it, so it is not the blocker): the fraction pointer defined between
// copies 3 and 4 / 2 and 3 / 1 and 2 / the tint and copy 1 62/75/76/75 (still EAX, rematerialised);
// the light's name read into a local before the 0xbab40 call 49 (same code).
// Round 91 (lane 7; harness counts, where the unchanged source gives 50): the colour copies after
// the 0xbab40 call 79; the copied view point typed point3f 50; the first blend's fraction named
// after 0xbad50 50; `object` declared before `object_index` 52; the object through `object_get`
// 50; the alphas chained 52; a trivial 0xbab40 body (diagnostic) 393.
// Round 32 (lane 10, fast counts; aligned 8: the colour-copy schedule and `[ebx]` only): the fraction
// pointer renamed (value / blend_value / zfraction / a_fraction) 49 each; the else arm's colour copy as
// a plain struct assignment 49 (same code); the pointer declared at function scope and assigned in the
// arm 49; the light as an `s_light_dynamic *const` 49; the first arm's blend read from the field at each
// use 324 (aligned 16); `&light->fraction` passed directly again 214 (1616 bytes, aligned 13: also
// turns the pair fade products).
// Round 175: the four colours as aggregate initialisers (alpha first) 414 (plain); the name read into
// a local before 0xbab40 50; the object index chosen by one ternary 428; the function arm's blend read
// from the field at each use 325; the pair colours copied before the other two (tint first) 58
// (harness, where the unchanged source gives 50). Diagnostics on the
// callee (harness): 0xbab40 returning true straight from its 1.0 case or without its recursive call
// leaves this at 50; only a body small enough to inline moves it (393).
// Round 111 (lane 6, fast): the fraction pointer from a second view of the light record (re-indexed from the pool)
// 310 (1607 B; the light index moves to the stack and the later read is still folded to [ebp + 0xd0]); the first
// blend's fraction read through that view 283; the colours declared before the tint 49; colours declared first,
// second, pair_first, pair_second 49; pair_second, pair_first, second, first 49. 0xbab40 saves EBX/ESI/EDI/EBP as
// retail does, so its body is not what keeps EBX from the pointer.
// Round 51 (lane 16, fast with 0xbab40/0xbad50/0x131d60/0xc0b00): the pointer read at the second 0x131d60
// call and `light->fraction` at the first 49. 0xbab40 (ours, 28) with the 1.0 case's own `return enabled`
// 117, with a named early result too 115 (this function stays 49 with either).
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
        *(color3f *)&pair_first.red = *(color3f *)(definition + 0x40);
        *(color3f *)&pair_second.red = *(color3f *)(definition + 0x4c);
        second.alpha = 1.0f;
        first.alpha = 1.0f;
        pair_second.alpha = 1.0f;
        pair_first.alpha = 1.0f;
        real *fraction = &light->fraction;
        function_bab40(object_index, light->name, fraction);
        function_bad50(object_index, light->colour_index, (point3f *)&tint);
        function_131d60(&first, &second, *(dword *)(definition + 0x34),
            *fraction, &light->colour, &tint);
        function_131d60(&pair_first, &pair_second, *(dword *)(definition + 0x34),
            light->fraction, &light->pair_colour, &tint);
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
long function_c0350(long tag_index, long object_index, long node_index, vector3f const *up,
    point3f const *position, vector3f const *forward, real scale)
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
