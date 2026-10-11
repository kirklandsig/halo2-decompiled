#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_030290.h"
#include "globals.h"
#include <math.h>

// @flags /O2 /Ob1 /Gr /arch:SSE

#define k_real_epsilon 0.0001f
#define k_real_max 3.4028234663852886e+38f

struct s_31f50_object
{
	byte unknown00[0xc];
	long child;
	long next;
	byte unknown14[0x1c];
	point3f center;
	real radius;
};

struct s_31f50_header
{
	byte unknown00[8];
	s_31f50_object *object;
};

PRIVATE inline real bounds_vector_length(vector3f const *vector)
{
	return (real)sqrt(vector->k * vector->k + vector->j * vector->j + vector->i * vector->i);
}

// @retail 0x31f50
void __stdcall function_31f50(long index, point3f const *origin, vector3f const *axis, real *radial, real *axial)
{
	if (index != NONE)
	{
		s_31f50_object *object = ((s_31f50_header *)g_4e0300->data)[index & 0xffff].object;
		vector3f perpendicular;
		vector3f parallel;
		vector3f delta;
		delta.i = object->center.x - origin->x;
		delta.j = object->center.y - origin->y;
		delta.k = object->center.z - origin->z;
		real projection = axis->k * delta.k + axis->j * delta.j + axis->i * delta.i;
		parallel.i = axis->i * projection;
		parallel.j = axis->j * projection;
		parallel.k = axis->k * projection;
		perpendicular.i = delta.i - parallel.i;
		perpendicular.j = delta.j - parallel.j;
		perpendicular.k = delta.k - parallel.k;
		real width = bounds_vector_length(&perpendicular);
		real length = bounds_vector_length(&parallel);
		if (object->radius + width > *radial)
			*radial = object->radius + width;
		if (object->radius + length > *axial)
			*axial = object->radius + length;
		if (object->child != NONE)
			function_31f50(object->child, origin, axis, radial, axial);
		if (object->next != NONE)
			function_31f50(object->next, origin, axis, radial, axial);
	}
}

/* ---- globals ---- */
#define BIT(n) ((bool)((g_4ba014 >> (n)) & 1))
byte *g_4858c4;
s_flag_entry g_4ba074[16];
long g_4ba134;
s_camera g_4b9e14;
point2f g_4b9ecc;

// @retail 0x30290
void function_30290(point3f const *p, s_view const *view, s_camera const *camera, real radius, box2f *bounds)
{
	real dx = p->x - view->position.x;
	real dy = p->y - view->position.y;
	real dz = p->z - view->position.z;
	real sx = dx, sy = dy, sz = dz;
	if (camera->scale != 1.f)
	{
		sx = camera->scale * dx;
		sy = camera->scale * dy;
		sz = camera->scale * dz;
	}

	real cx = camera->forward.i * sz + camera->up.i * sy + camera->right.i * sx;
	real cy = camera->forward.j * sz + camera->up.j * sy + camera->right.j * sx;
	real cz = (camera->forward.k * sz + camera->up.k * sy + camera->right.k * sx) * -1.f;

	if (cz + radius > 0.01f)
	{
		real m78 = camera->unknown78;
		real m98 = camera->unknown98;
		real m8c = camera->unknown8c;
		real m9c = camera->unknown9c;
		real left = view->bounds.left * 0.5f;
		real right = view->bounds.right * 0.5f;
		real top = view->bounds.top * 0.5f;
		real bottom = view->bounds.bottom * 0.5f;

		real xz = cx * cx + cz * cz;
		real r2 = radius * radius;
		real disc = xz - r2;
		if (disc > k_real_epsilon)
		{
			real root = (real)sqrt(disc);
			real k = radius / xz;
			real za = cz - (cz * radius + root * cx) * k;
			real xa = cx - (cx * radius + root * cz) * k;
			real zb = cz - (cz * radius - root * cx) * k;
			real xb = cx - (cx * radius - root * cz) * k;
			if (za > 0.01f)
			{
				real n = xa / za * m78 + m98;
				bounds->x0 = (1.f - n) * left + (n + 1.f) * right;
			}
			else
				bounds->x0 = -k_real_max;
			if (zb > 0.01f)
			{
				real n = xb / zb * m78 + m98;
				bounds->x1 = (1.f - n) * left + (n + 1.f) * right;
			}
			else
				bounds->x1 = k_real_max;
		}
		else
		{
			bounds->x0 = -k_real_max;
			bounds->x1 = k_real_max;
		}

		real yz = cy * cy + cz * cz;
		real disc2 = yz - r2;
		if (disc2 > k_real_epsilon)
		{
			real root = (real)sqrt(disc2);
			real k = radius / yz;
			real za = cz - (cz * radius + root * cy) * k;
			real ya = cy - (cy * radius + root * cz) * k;
			real zb = cz - (cz * radius - root * cy) * k;
			real yb = cy - (cy * radius - root * cz) * k;
			if (za > 0.01f)
			{
				real n = ya / za * m8c + m9c;
				bounds->y0 = (1.f - n) * top + (n + 1.f) * bottom;
			}
			else
				bounds->y0 = -k_real_max;
			if (zb > 0.01f)
			{
				real n = yb / zb * m8c + m9c;
				bounds->y1 = (1.f - n) * top + (n + 1.f) * bottom;
			}
			else
				bounds->y1 = k_real_max;
		}
		else
		{
			bounds->y0 = -k_real_max;
			bounds->y1 = k_real_max;
		}
	}
	else
	{
		bounds->x0 = k_real_max;
		bounds->x1 = -k_real_max;
		bounds->y0 = k_real_max;
		bounds->y1 = -k_real_max;
	}
}

// @retail 0x30710
bool function_30710(vector3f const *v, short_rectangle2d const *rect, point2f *out, s_camera const *camera, s_view const *view)
{
	bool result = false;
	if (!rect)
		rect = &view->bounds;
	if (v->k < 0.f)
	{
		real width = (real)(rect->right - rect->left);
		real height = (real)(rect->bottom - rect->top);
		real inv = -1.f / v->k;
		out->x = (camera->unknown78 * v->i + camera->unknown98 * v->k) * inv;
		out->y = 0.f - (camera->unknown8c * v->j + camera->unknown9c * v->k) * inv;
		if (out->x >= -1.f && 1.f >= out->x && out->y >= -1.f && 1.f >= out->y)
			result = true;
		real scaled_x = (out->x + 1.f) * 0.5f;
		out->x = scaled_x * width + (real)rect->left;
		real scaled_y = (out->y + 1.f) * 0.5f;
		out->y = scaled_y * height + (real)rect->top;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
	}
	return result;
}

__inline void transform_point(s_camera const *camera, point3f const *p, point3f *out)
{
	real x = p->x, y = p->y, z = p->z;
	if (camera->scale != 1.f)
	{
		x = camera->scale * x;
		y = camera->scale * y;
		z = camera->scale * z;
	}
	out->x = camera->right.i * x + camera->up.i * y + camera->forward.i * z + camera->position.x;
	out->y = camera->right.j * x + camera->up.j * y + camera->forward.j * z + camera->position.y;
	out->z = camera->right.k * x + camera->up.k * y + camera->forward.k * z + camera->position.z;
}

// @retail 0x30830
long function_30830(point3f const *a, s_camera const *camera, vector3f const *d, point2f const *scale, bool perspective, bool negate, point2f *out)
{
	real inv_a = 0.f;
	real inv_b = 0.f;
	if (!a)
		a = g_468788;
	if (!camera)
		camera = &g_4b9e14;
	if (!scale)
		scale = &g_4b9ecc;

	point3f b;
	b.x = a->x + d->i;
	b.y = d->j + a->y;
	b.z = a->z + d->k;

	point3f ta, tb;
	transform_point(camera, a, &ta);
	transform_point(camera, &b, &tb);

	real fa, fb;
	if (perspective)
	{
		if (fabs(tb.z) < k_real_epsilon)
		{
			point3f b2;
			b2.x = a->x - d->i;
			b2.y = a->y - d->j;
			b2.z = a->z - d->k;
			transform_point(camera, &b2, &tb);
		}
		if (!(fabs(ta.z) < k_real_epsilon))
			inv_a = 1.f / ta.z;
		fa = inv_a;
		if (!(fabs(tb.z) < k_real_epsilon))
			inv_b = 1.f / tb.z;
		fb = inv_b;
	}
	else
	{
		fa = 1.f;
		fb = 1.f;
	}

	if (fa != 0.f && fb != 0.f)
	{
		real dx = tb.x * fb - ta.x * fa;
		real dy = tb.y * fb - ta.y * fa;
		if (negate)
		{
			dx = 0.f - scale->x * dx;
			dy = 0.f - scale->y * dy;
		}
		out->x = dx;
		out->y = dy;
		if (dx != 0.f || dy != 0.f)
			return true;
	}
	else
	{
		out->x = 0.f;
		out->y = 0.f;
	}
	return false;
}

/* inline, as Bungie's header math was: the out-of-line copy then gives its callers no register summary, so they assume edx is clobbered across the call, as retail's 360 callers do */
__declspec(noinline) real function_30bf0(vector3f *v);
// @retail 0x30bf0
inline real function_30bf0(vector3f *v)
{
	real m = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);
	if (!(fabs(m) < k_real_epsilon))
	{
		real inv = 1.f / m;
		v->i = v->i * inv;
		v->j = inv * v->j;
		v->k = inv * v->k;
		return m;
	}
	return 0.f;
}

// @retail 0x30c60
void function_30c60(dword handle, point3f *position, long *out)
{
	s_obj *obj = ((s_obj_array *)g_4e0300->data)->elements[handle & 0xffff].obj;
	*position = obj->position;
	*out = obj->unknown3c;
}

// @retail 0x30ca0
void function_30ca0(bool value, long bit)
{
	if (value)
		g_4ba014 |= 1 << bit;
	else
		g_4ba014 &= ~(1 << bit);
}

// @retail 0x30cd0
long function_30cd0(bool a, bool b)
{
	if (a)
		return -1;
	if (b)
	{
				if (BIT(4) && !BIT(5))
			return 0;
		return 1;
	}
	return 2;
}

// @retail 0x30d10
long function_30d10(bool a, bool b, bool c)
{
	if (BIT(0)) return NONE;
	if (*g_4858c4 & 0x20) return 2;
	if (!BIT(2) || !b)
	{
		if (BIT(1) && a) return 2;
		return NONE;
	}
	if (c) return 2;
	if (BIT(6)) return 6;
	if (BIT(4)) return BIT(5) ? 3 : 4;
	return (long)((byte)(g_4ba014 >> 3)) & 1;
}

// @retail 0x30da0
long function_30da0(bool a, bool b)
{
	if (BIT(0))
		return 1;
	if (*g_4858c4 & 0x20)
		return 4;
	if (BIT(1) && a)
		return 0;
	if (BIT(2) && b)
		return 2;
	return 3;
}
// @retail 0x30e00
long function_30e00(bool a)
{
		if (BIT(0) && BIT(1) && a)
		return 0;
	return -1;
}

// @retail 0x30e30
void function_30e30(long key, long value, bool add, real x)
{
	if (key == NONE)
		return;
	long count = g_4ba134;
	long i;
	for (i = 0; i < count; i++)
	{
		if (g_4ba074[i].key == key)
		{
			if (add)
			{
				s_flag_entry *entry = &g_4ba074[i];
				entry->value = value;
				entry->x = x;
			}
			else
			{
				count--;
				g_4ba134 = count;
				if (i < count)
					g_4ba074[i] = g_4ba074[count];
			}
			return;
		}
	}
	if (add && (dword)count < 16)
	{
		s_flag_entry *entry = &g_4ba074[count];
		g_4ba134 = count + 1;
		entry->key = key;
		entry->value = value;
		entry->x = x;
	}
}

real function_30bf0(vector3f *v);

PRIVATE __forceinline void normalize_cross_vector(vector3f *v)
{
	real length = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);
	if (!(fabs(length) < k_real_epsilon))
	{
		real inverse = 1.0f / length;
		v->i = inverse * v->i;
		v->j = v->j * inverse;
		v->k = v->k * inverse;
	}
}

// @retail 0x31b40
real function_31b40(vector3f *vector, vector3f const *axis)
{
	vector3f cross;
	cross.i = axis->k * vector->j - vector->k * axis->j;
	cross.j = vector->k * axis->i - axis->k * vector->i;
	cross.k = axis->j * vector->i - axis->i * vector->j;
	normalize_cross_vector(&cross);
	vector3f result;
	result.i = axis->j * cross.k - cross.j * axis->k;
	result.j = cross.i * axis->k - axis->i * cross.k;
	result.k = axis->i * cross.j - axis->j * cross.i;
	*vector = result;
	return function_30bf0(vector);
}

extern s_record_pool *g_4e030c;

struct s_render_entry_110
{
    long unknown00;
    long tag;
    byte unknown08[0x4c - 8];
    long object;
    long unknown50;
    short index;
    byte unknown56[0x110 - 0x56];
};

struct s_index_flags_31660
{
    long identifier;
    short count;
    short unknown06;
    word *values;
    long *groups;
};
struct s_index_state_31660
{
    byte unknown00[0xc];
    s_index_flags_31660 *first;
    void *unknown10;
    s_index_flags_31660 *second;
    s_index_flags_31660 *third;
};

// @retail 0x31660
void function_31660(long index, long tag, s_index_state_31660 *state,
    long first_group, long second_group, bool skip, bool *output)
{
    long const *first_reference = &first_group;
    long const *second_reference = &second_group;
    bool const *skip_reference = &skip;
    bool *const *output_reference = &output;
    s_render_entry_110 *entry = &((s_render_entry_110 *)g_4e030c->data)[index & 0xffff];
    dword const *definition = (dword const *)g_4e3b44[tag & 0xffff].bytes;
    if (*definition & 0x10000)
    {
        for (long i = 0; i < state->first->count; ++i)
        {
            word *value = &state->first->values[(short)i];
            long flags = *value;
            flags &= 0xdfff;
            flags |= 0x1000;
            *value = (word)flags;
        }
        for (long i = 0; i < state->third->count; ++i)
        {
            word *value = &state->third->values[(short)i];
            long flags = *value;
            flags &= 0xdfff;
            flags |= 0x1000;
            *value = (word)flags;
        }
    }
    if (*definition & 0x20000)
        for (long i = 0; i < state->second->count; ++i)
        {
            word *value = &state->second->values[(short)i];
            long flags = *value;
            flags &= 0xdfff;
            flags |= 0x1000;
            *value = (word)flags;
        }
    if (*definition & 0x3800)
    {
        byte flag11 = (byte)~(*definition >> 11);
        byte flag13_source = (byte)~(*definition >> 13);
        byte flag12_source = (byte)~(*definition >> 12);
        byte flag12 = (flag11 | flag13_source) & 1;
        byte flag13 = (flag12_source | flag13_source) & 1;
        for (long i = 0; i < state->second->count; ++i)
            if (*first_reference == NONE || state->second->groups[(short)i] == *first_reference)
            {
                word *value = &state->second->values[(short)i];
                long flags = *value;
                if (!(flag13))
                    flags &= ~0x2000;
                else
                    flags |= 0x2000;
                if (flag12) flags |= 0x1000; else flags &= ~0x1000;
                *value = (word)flags;
            }
        if (*first_reference != *second_reference)
            for (long i = 0; i < state->second->count; ++i)
                if (*second_reference == NONE || state->second->groups[(short)i] == *second_reference)
                {
                    word *value = &state->second->values[(short)i];
                    long flags = *value;
                    if (flag13) flags |= 0x2000; else flags &= ~0x2000;
                    if (flag12) flags |= 0x1000; else flags &= ~0x1000;
                    *value = (word)flags;
                }
    }
    if (state->first->count > 0 && (*definition & 4) && state->first->count > 1)
    {
        *(real *)((byte *)entry + 0xf8) = 1.0f;
        // Retail's final store is reached only with a null output argument.
        if (!*output_reference && !*skip_reference)
        {
            for (long i = 0; i < state->first->count; ++i)
                state->first->values[(short)i] |= 0x3000;
            **output_reference = true;
        }
    }
}
