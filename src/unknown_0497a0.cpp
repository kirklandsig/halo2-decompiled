// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0497A0.CPP: view parameter computation */

#include "unknown_11c920.h"
#include <math.h>
#include "unknown_0494b0.h"
#include "globals.h"

point3f g_4b9da0;
vector3f g_4b9dac;
vector3f g_4b9db8;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

// @retail 0x497a0
void function_0497a0(
	s_view_source *source,
	s_view_result *result,
	s_view_camera *camera,
	s_view_flags *flags,
	vector3f const *c_in,
	vector3f const *d_in,
	real scale,
	long *mode,
	vector4f *old20,
	vector4f *old30)
{
	real inv = -1.0f;
	s_view_shape_0 * shape0 = 0;
	s_view_shape_1 * shape1;
	shape1 = 0;
	if (!(0 == source->type_low))
	{
		shape1 = &source->shape1;
	}
	else
	{
		shape0 = &source->shape0;
	}

	bool special = ((0x20 & flags->flags)) && (0x40000 & flags->flags);
	point3f v1 = special ? g_4b9da0 : source->v2c;
	vector3f v2 = special ? g_4b9dac : camera->v28;
	vector3f v3 = special ? g_4b9dac : (shape1 ? shape1->v4 : camera->v28);
	vector3f v4 = special ? g_4b9db8 : camera->v34;
	vector3f w;

	if (special)
	{
		vector3f c;
		c.i = v4.j * v2.k - v4.k * v2.j;
		real s1 = (real)sin(g_4b9da0.x * 5.6f + g_4b9da0.y);
		c.j = v4.k * v2.i - v4.i * v2.k;
		real s0 = (real)((sin(g_4b9da0.x * 8.2f) * cos(g_4b9da0.y * 7.8f) * sin(g_4b9da0.x) + cos(g_4b9da0.y * 7.75f)) * 0.5f);
		c.k = v4.i * v2.j - v4.j * v2.i;
		real amp = s0 * 0.03f;
		v1.x = (v4.i * amp) + v1.x;
		v1.y += amp * v4.j;
		v1.z = v1.z + (amp * v4.k);
		real k = s1 * 0.025f;
		v1.x = (c.i * k) + v1.x;
		v1.y += k * c.j;
		v1.z = (k * c.k) + v1.z;
		w.i = v2.j * v4.k - v2.k * v4.j;
		w.j = v2.k * v4.i - v2.i * v4.k;
		w.k = v2.i * v4.j - v2.j * v4.i;
	}
	else
	{
		w = source->v1c;
	}

	switch (flags->w8e)
	{
		case 1:
			if (shape1)
			{
				inv = -1.0f / (shape1->f2c - shape1->f28);
			}
			break;

		default:
			inv = -1.0f / (shape0 ? shape0->f0c : shape1->f2c);
			break;
	}

	if (source->type != 1)
	{
		result->p0 = v1;
		result->f0c = 1.0f;
		result->p10 = v1;
	}

	switch (source->type)
	{
	case 0:
	{
		result->v20.i = source->v1c.i * inv;
		result->v20.j = (inv * source->v1c.j);
		result->v20.k = inv * source->v1c.k;
		result->v30.i = camera->v34.i * inv;
		result->v30.j = inv * camera->v34.j;
		result->v30.k = inv * camera->v34.k;
		result->f2c = 0.0f;
		result->f3c = 0.0f;
		break;
	}

	case 1:
		break;

	default:
	{
		vector3f r;
		r.i = v3.j * v4.k - v3.k * v4.j;
		r.j = v3.k * v4.i - v3.i * v4.k;
		vector3f s;
		r.k = v4.j * v3.i - v3.j * v4.i;
		s.i = v3.j * w.k - v3.k * w.j;
		s.j = v3.k * w.i - w.k * v3.i;
		real sx = -1.0f / shape1->f14;
		s.k = w.j * v3.i - v3.j * w.i;
		real sy = -1.0f / shape1->f18;
		real k = inv * 0.5f;
		result->v20.i = k * (r.i * sx + v2.i);
		result->v20.j = (r.j * sx + v2.j) * k;
		result->v20.k = (r.k * sx + v2.k) * k;
		result->v30.i = k * (s.i * sy + v2.i);
		result->v30.j = k * (s.j * sy + v2.j);
		result->v30.k = k * (s.k * sy + v2.k);
		result->f2c = 0.0f;
		result->f3c = 0.0f;
		break;
	}
	}

	switch (flags->w8e)
	{
	default:
	{
		result->v40.i = v2.i * inv;
		result->v40.j = inv * v2.j;
		result->v40.k = v2.k * inv;
		result->f4c = inv * 0.5f;
		result->v50 = *g_4687a4;
		result->f5c = 0.0f;
		break;
	}

	case 1:
	{
		vector3f const * up = special ? &g_4b9dac : &camera->v28;
		if (source->type != 1)
		{
			result->v40.i = up->i * inv;
			result->v40.j = inv * up->j;
			result->v40.k = inv * up->k;
			result->f4c = inv * (shape1->f28 + source->f28);
			if (!(0x20 & flags->flags))
			{
				real h = (shape1->f2c - shape1->f28) * 0.025f * 0.5f;
				result->v50.i = h * result->v40.i;
				int tmp0;
				tmp0 = result->v40.j * h;
				result->v50.j = tmp0;
				result->v50.k = result->v40.k * h;
				result->f5c = (source->f28 + 0.0078125f) * h * inv + 0.5f;
			}
			else
			{
				result->v50 = *g_4687a4;
				result->f5c = 1.0f;
			}
		}
		break;
	}
	}

	*old20 = *(vector4f *)&result->v20;
	*old30 = *(vector4f *)&result->v30;
	result->v20.i = result->v20.i + (camera->f6c * result->v40.i);
	result->v20.j = (result->v40.j * camera->f6c) + result->v20.j;
	result->v20.k += camera->f6c * result->v40.k;
	result->v30.i += result->v40.i * camera->f70;
	result->v30.j = (result->v40.j * camera->f70) + result->v30.j;
	result->v30.k = (result->v30.k + (camera->f70 * result->v40.k));

	vector3f c = *c_in;
	vector3f d = *d_in;
	real ca = scale * c.i;
	real cb = scale * c.j;
	real cc = c.k * scale;
	real da = (d.i * scale);
	real db = d.j * scale;
	long tmp2 = (((scale * d.k)));
	real dc = tmp2;
	real mc = MAX(ca, MAX(cb, cc));
	real md = MAX(da, MAX(db, dc));
	real m = MAX(mc, md);

	if (m > 2.0f)
	{
		result->f60 = PIN(ca * 0.25f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.25f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.25f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.25f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.25f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.25f, 0.0f, 1.0f);
		*mode = 2;
	}
	else if (m > 1.0f)
	{
		result->f60 = PIN(ca * 0.5f, 0.0f, 1.0f);
		result->f64 = PIN(cb * 0.5f, 0.0f, 1.0f);
		result->f68 = PIN(cc * 0.5f, 0.0f, 1.0f);
		result->f70 = PIN(da * 0.5f, 0.0f, 1.0f);
		result->f74 = PIN(db * 0.5f, 0.0f, 1.0f);
		result->f78 = PIN(dc * 0.5f, 0.0f, 1.0f);
		*mode = 1;
	}
	else
	{
		result->f60 = PIN(ca, 0.0f, 1.0f);
		result->f64 = PIN(cb, 0.0f, 1.0f);
		result->f68 = PIN(cc, 0.0f, 1.0f);
		result->f70 = PIN(da, 0.0f, 1.0f);
		result->f74 = PIN(db, 0.0f, 1.0f);
		result->f78 = PIN(dc, 0.0f, 1.0f);
		*mode = 0;
	}
}


real function_17ca10(real value, short curve);

// @retail 0x42c50
real function_42c50(point3f const *position, vector3f const *normal, short mode, byte const *flags)
{
	(void)&flags;
	real result = 1.0f;
	if (mode)
	{
		real x = g_4b9da0.x - position->x;
		real y = g_4b9da0.y - position->y;
		real z = g_4b9da0.z - position->z;
		result = (real)fabs((z * normal->k + y * normal->j + x * normal->i) / sqrt(z * z + y * y + x * x));
		if (*flags & 0x40)
			result = function_17ca10(result, 2);
		if (mode == 2)
			result = 1.0f - result;
	}
	return result;
}


extern s_record_pool *g_4ea940;
bool function_bad50(long object_index, long index, point3f *out);

struct s_ribbon_source
{
    byte unknown00[8];
    long object_index;
    short marker_index;
    byte unknown0e[6];
    real scale;
    byte unknown18[4];
    real u, v;
    byte unknown24[12];
    short point_count[4];
    long point_index[4];
};
struct s_ribbon_state
{
    byte unknown00[0x18];
    real width;
    color4f lower, upper;
    dword flags;
};
struct s_ribbon_definition
{
    word flags00, flags02;
    byte unknown04[0x14];
    short mode;
    byte unknown1a[2];
    real u_step, v_width;
    byte unknown24[0xc4];
    long state_count;
    s_ribbon_state *states;
};
struct s_ribbon_point
{
    short salt;
    byte flags;
    char state;
    real transition, unknown08, scale;
    byte unknown10[0xc];
    point3f position;
    vector3f velocity;
    long next;
};
struct s_ribbon_input
{
    s_ribbon_source *source;
    s_ribbon_definition *definition;
    short slot, unknown0a;
    byte *shader;
    bool normal;
    byte unknown11[3];
    long vertex_count;
};
struct s_ribbon_vertex
{
    point3f position;
    real u, v;
    dword color;
};

PRIVATE __forceinline dword ribbon_pixel(color4f const *color)
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

// @retail 0x42cf0
bool __stdcall function_42cf0(s_ribbon_vertex *vertices, long unknown, s_ribbon_input *input)
{
    // Retail keeps all three callback arguments on the stack.
    (void)&unknown;
    s_ribbon_source *source = input->source;
    s_ribbon_definition *definition = input->definition;
    real u = source->u;
    real u_step = definition->u_step;
    if (definition->flags02 & 0x40)
        u_step *= source->scale;
    u_step = -u_step;
    real v = source->v;
    real v_width = definition->v_width;
    if (definition->flags02 & 0x80)
        v_width *= source->scale;
    v_width += v;
    long index = source->point_index[input->slot];
    s_ribbon_point *previous = NULL;
    while (index != NONE)
    {
        s_ribbon_point *point = (s_ribbon_point *)(g_4ea940->data + (index & 0xffff) * 0x38);
        long state_index = point->state < 0 ? 0 : ((long)point->state > definition->state_count - 1 ? definition->state_count - 1 : (long)point->state);
        color4f color;
        real width;
        if (state_index != point->state)
        {
            width = 0.0f;
            color.alpha = color.red = color.green = color.blue = 0.0f;
        }
        else
        {
            s_ribbon_state *state = &definition->states[state_index];
            real scale = (state->flags & 0x20) ? point->scale : 1.0f;
            width = state->width;
            if (state->flags & 0x10)
                width *= point->scale;
            real *lower = (real *)&state->lower;
            real *upper = (real *)&state->upper;
            real *out = (real *)&color;
            for (long i = 0; i < 4; ++i)
                out[i] = (upper[i] - lower[i]) * scale + lower[i];
            if (point->flags & 2)
            {
                s_ribbon_state *next = &definition->states[point->state + 1];
                real next_width = next->width;
                if (next->flags & 0x10)
                    next_width *= point->scale;
                width += (next_width - width) * point->transition;
                lower = (real *)&next->lower;
                upper = (real *)&next->upper;
                for (long j = 0; j < 4; ++j)
                    out[j] += ((upper[j] - lower[j]) * scale + lower[j] - out[j]) * point->transition;
            }
        }
        if (source->object_index != NONE && !(definition->flags00 & 0x80))
        {
            point3f tint = *g_468710;
            byte *object = *(byte **)(g_4e0300->data + (source->object_index & 0xffff) * 12 + 8);
            byte *object_tag = g_4e3b44[(*(long *)object) & 0xffff].bytes;
            byte *markers = *(byte **)(object_tag + 0x98);
            function_bad50(source->object_index, *(short *)(markers + source->marker_index * 24 + 12), &tint);
            color.red *= tint.x;
            color.green *= tint.y;
            color.blue *= tint.z;
        }
        real half_width = width * 0.5f;
        vertices[0].u = vertices[1].u = u;
        vertices[0].v = v;
        vertices[1].v = v_width;
        point3f first = point->position, second = point->position;
        vector3f normal;
        switch (definition->mode)
        {
        case 0:
            first.z += half_width;
            second.z -= half_width;
            if (input->normal)
            {
                s_ribbon_point *neighbor = previous ? previous : (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                real dx = previous ? point->position.x - neighbor->position.x : neighbor->position.x - point->position.x;
                real dy = previous ? neighbor->position.y - point->position.y : point->position.y - neighbor->position.y;
                normal.i = dy; normal.j = dx; normal.k = 0.0f;
                real length = (real)sqrt(dx * dx + dy * dy);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    normal.i *= inverse; normal.j *= inverse; normal.k *= inverse;
                }
            }
            break;
        case 1:
        case 2:
            {
                s_ribbon_point *neighbor = previous ? previous : (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                real dx = previous ? point->position.x - neighbor->position.x : neighbor->position.x - point->position.x;
                real dy = previous ? neighbor->position.y - point->position.y : point->position.y - neighbor->position.y;
                real length = (real)sqrt(dx * dx + dy * dy);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    dx *= inverse; dy *= inverse;
                }
                first.x += dy * half_width; second.x -= dy * half_width;
                first.y += dx * half_width; second.y -= dx * half_width;
                if (input->normal) normal = *g_4687b0;
            }
            break;
        case 4:
            {
                s_ribbon_point *neighbor = previous ? previous : (s_ribbon_point *)(g_4ea940->data + (point->next & 0xffff) * 0x38);
                point3f origin = previous ? neighbor->position : point->position;
                vector3f view, direction, side;
                view.i = g_4b9da0.x - origin.x; view.j = g_4b9da0.y - origin.y; view.k = g_4b9da0.z - origin.z;
                direction.i = previous ? point->position.x - neighbor->position.x : neighbor->position.x - point->position.x;
                direction.j = previous ? point->position.y - neighbor->position.y : neighbor->position.y - point->position.y;
                direction.k = previous ? point->position.z - neighbor->position.z : neighbor->position.z - point->position.z;
                side.i = direction.k * view.j - direction.j * view.k;
                side.j = direction.i * view.k - direction.k * view.i;
                side.k = direction.j * view.i - direction.i * view.j;
                real length = (real)sqrt(side.k * side.k + side.j * side.j + side.i * side.i);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.0f / length;
                    side.i *= inverse; side.j *= inverse; side.k *= inverse;
                }
                first.x += side.i * half_width; second.x -= side.i * half_width;
                first.y += side.j * half_width; second.y -= side.j * half_width;
                first.z += side.k * half_width; second.z -= side.k * half_width;
                if (input->normal)
                {
                    normal.i = side.k * direction.j - side.j * direction.k;
                    normal.j = side.i * direction.k - side.k * direction.i;
                    normal.k = side.j * direction.i - side.i * direction.j;
                    length = (real)sqrt(normal.k * normal.k + normal.j * normal.j + normal.i * normal.i);
                    if (!(fabs(length) < 0.0001f))
                    {
                        real inverse = 1.0f / length;
                        normal.i *= inverse; normal.j *= inverse; normal.k *= inverse;
                    }
                }
            }
            break;
        default:
            return false;
        }
        vertices[0].position = first;
        vertices[1].position = second;
        color.alpha *= function_42c50(&point->position, &normal, *(short *)(input->shader + 0x2c), (byte *)definition);
        color.alpha = PIN(color.alpha, 0.0f, 1.0f);
        vertices[0].color = vertices[1].color = ribbon_pixel(&color);
        vertices += 2;
        u += u_step;
        previous = point;
        index = point->next;
    }
    vertices -= input->vertex_count;
    if (!(definition->flags00 & 1) && source->point_count[input->slot] > 2)
        vertices[0].color = vertices[1].color = vertices[0].color & 0xffffff;
    if (!(definition->flags00 & 2))
        vertices[input->vertex_count - 1].color = vertices[input->vertex_count - 2].color = vertices[input->vertex_count - 1].color & 0xffffff;
    return true;
}

long function_137590(long group_index, short frame_index, short sequence_index);
struct D3DTexture;
struct s_bitmap_view;
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
void function_52d40(long bitmap_index, byte *shader, long tag, dword flags);
bool function_1e5e0(point3f const *position, long size, long a, long b, long c,
    long d, long e, long f, long g, void const *data);

struct s_43740_input
{
    s_ribbon_input vertices;
    long tag;
    long bitmap;
};

// @retail 0x43720
void __stdcall function_43720(s_43740_input *input)
{
    function_52d40(input->bitmap, input->vertices.shader, input->tag, false);
}

// @retail 0x43740
void function_43740(s_ribbon_source *source, s_ribbon_definition *definition, short slot)
{
    (void)&slot;
    byte *bytes = (byte *)definition;
    long tag = *(long *)(bytes + 0x34);
    if (tag != NONE)
    {
        long bitmap = function_137590(tag, *(short *)((byte *)source + 0x18), *(short *)((byte *)source + 0x1a));
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        s_bitmap_view *image = (s_bitmap_view *)(*(byte **)(group + 0x48) + bitmap * 0x74);
        if (function_12360(image, 0.0f))
        {
            s_43740_input input;
            input.vertices.source = source;
            input.vertices.definition = definition;
            input.vertices.slot = slot;
            input.vertices.shader = bytes + 0x3c;
            input.vertices.normal = *(short *)(bytes + 0x68) != 0;
            input.vertices.vertex_count = (short)(source->point_count[slot] * 2);
            input.tag = tag;
            input.bitmap = bitmap;
            point3f const *position = (point3f *)(g_4ea940->data + (source->point_index[slot] & 0xffff) * 0x38 + 0x1c);
            function_1e5e0(position, 0x20, (long)function_43720, (long)function_42cf0,
                0, 4, 0x28, 0x18, input.vertices.vertex_count, &input);
        }
    }
}

extern s_record_pool *g_4ea944;

// @retail 0x42b20
void __stdcall function_42b20(dword mask)
{
    long index = data_datum_index(g_4ea944, function_16bc00(g_4ea944, 0));
    while (index != NONE)
    {
        s_ribbon_source *source = (s_ribbon_source *)(g_4ea944->data + (index & 0xffff) * 0x48);
        long tag = *(long *)((byte *)source + 4);
        s_ribbon_definition *definition = (s_ribbon_definition *)g_4e3b44[tag & 0xffff].bytes;
        for (short slot = 0; slot < 4; slot++)
        {
            if ((mask & (1 << (byte)definition->mode)) && source->point_count[slot] >= 2)
                function_43740(source, definition, slot);
        }
        index = data_datum_index(g_4ea944,
            data_find_index(g_4ea944, index == NONE ? 0 : (index & 0xffff) + 1));
    }
}
