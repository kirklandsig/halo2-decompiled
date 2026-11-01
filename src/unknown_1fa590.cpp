// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1FA590.CPP: lookups in the structure's 2d trees and surfaces, and
   the surface a contact lies on */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_1fa590.h"
#include <math.h>
#include "slot_handler.h"
#include "normalize3d.h"
#include "vector_products.h"

void function_11d180(vector3f const *in, vector3f const *up, vector3f *forward, vector3f *left, vector3f *out);

struct s_motion_adjustment
{
	byte unknown00[0x20];
	real scale;
	byte unknown24[0xf4 - 0x24];
	vector3f forward;
	byte unknown100[0x124 - 0x100];
	vector3f current;
	byte unknown130[8];
	vector3f target;
	real rate;
};

struct s_motion_adjustment_result
{
	byte unknown00[0xc];
	vector3f velocity;
};

// @retail 0x1fa810
void __stdcall function_1fa810(long *ticks, s_motion_adjustment_result *out, s_motion_adjustment const *state)
{
	vector3f left;
	vector3f up;
	vector3f forward;
	function_11d180(&state->forward, g_4687b0, &forward, &left, &up);
	vector3f target;
	vector3f_scale(&state->target, state->scale, &target);
	vector3f desired;
	vector3f_scale(&state->forward, target.i, &desired);
	desired.i += target.j * left.i;
	desired.j += target.j * left.j;
	desired.k += target.j * left.k;
	desired.i += target.k * up.i;
	desired.j += target.k * up.j;
	desired.k += target.k * up.k;
	vector3f offset;
	offset.i = desired.i - state->current.i;
	offset.j = desired.j - state->current.j;
	offset.k = desired.k - state->current.k;
	vector3f normalized = offset;
	real length = normalize3d(&normalized);
	real step = g_510c54->rate * state->rate;
	vector3f motion;
	if (length > step)
		vector3f_scale(&normalized, step, &motion);
	else
		motion = offset;
	vector3f_add(&motion, &state->current, &out->velocity);
	*ticks = *ticks - 1 < 0 ? 0 : *ticks - 1;
}

/* a node of a 2d tree: a line and the two children (negative: a leaf) */
struct s_tree2d_node
{
	real i;
	real j;
	real d;
	long front;
	long back;
};

struct s_tree2d
{
	byte unknown00[0x10];
	long leaf_count;
	long *leaves;
	long node_count;
	s_tree2d_node *nodes;
};

/* the leaf of the tree the point is in, or NONE */
// @retail 0x1fa590
long function_1fa590(long index, s_tree2d const *tree, point2f const *point)
{
	while (index != NONE)
	{
		s_tree2d_node *node;

		if (!(index & 0x80000000))
			return index;

		index &= 0x7fffffff;
		if (index < 0 || index >= tree->node_count)
			break;

		node = &tree->nodes[index];
		if (node->i * point->x + node->j * point->y - node->d > 0.0)
			index = node->front;
		else
			index = node->back;
	}

	return NONE;
}

// @retail 0x1fa5e0
long *function_1fa5e0(long index, s_tree2d const *tree, long unknown)
{
	long *result = 0;

	if (unknown == NONE && index >= 0 && index < tree->leaf_count)
		result = &tree->leaves[index];
	return result;
}

/* the unit vector of the first axis direction set in the flags */
// @retail 0x1fa600
void function_1fa600(vector3f *vector, word flags)
{
	if (flags & 0x10)
	{
		vector->i = 0.0f;
		vector->j = 0.0f;
		vector->k = 1.0f;
	}
	else if (flags & 0x20)
	{
		vector->i = 0.0f;
		vector->j = 0.0f;
		vector->k = -1.0f;
	}
	else if (flags & 0x40)
	{
		vector->i = 0.0f;
		vector->j = 1.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x80)
	{
		vector->i = 0.0f;
		vector->j = -1.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x100)
	{
		vector->i = 1.0f;
		vector->j = 0.0f;
		vector->k = 0.0f;
	}
	else if (flags & 0x200)
	{
		vector->i = -1.0f;
		vector->j = 0.0f;
		vector->k = 0.0f;
	}
}

struct s_surface_flags
{
	byte unknown0[4];
	byte flags;
	byte bit;
	byte unknown6[2];
};

struct s_structure_view
{
	byte unknown00[0x2c];
	s_surface_flags *flags;
};

struct s_4e0348_view
{
	byte unknown00[0x18];
	s_structure_view *structure;
	byte unknown1c[0xc4 - 0x1c];
	long count;
	long first;
};

byte *function_183fc0(long index);

// @retail 0x1fa6b0
bool function_1fa6b0(s_pathfinding_node const *node, s_pathfinding_data const *pathfinding)
{
	bool result = false;
	long index;

	for (index = node->first_surface; index != NONE; index = pathfinding->surfaces[index].next)
	{
		s_pathfinding_surface *record = &pathfinding->surfaces[index];

		if (record->type == 7)
		{
			s_surface_flags *flags = &((s_4e0348_view *)g_4e0348)->structure->flags[record->index];

			if (flags->flags & 8)
			{
				dword bit = flags->bit;
				dword *bits = (dword *)function_183fc0(record->object_index);

				result = (bits[bit >> 5] & (1 << (bit & 31))) == 0;
			}
			break;
		}
	}

	return result;
}

// @retail 0x1fa720
s_pathfinding_edge *function_1fa720(s_edge_iterator *iterator)
{
	s_pathfinding_edge *edge = 0;

	while (iterator->surface_index != NONE)
	{
		s_pathfinding_surface *surface;

		iterator->previous_surface_index = iterator->surface_index;
		iterator->edge_index = (word)NONE;
		surface = &iterator->pathfinding->surfaces[iterator->previous_surface_index];
		iterator->surface_index = surface->next;
		if (surface->type == 0)
		{
			iterator->edge_index = (word)surface->index;
			edge = &iterator->pathfinding->edges[iterator->edge_index];
			iterator->forward = edge->surface == iterator->surface;
			goto found;
		}
	}

	if (iterator->next_edge_index != (word)NONE)
	{
		iterator->edge_index = iterator->next_edge_index;
		iterator->previous_surface_index = NONE;
		edge = &iterator->pathfinding->edges[iterator->edge_index];
		iterator->forward = edge->surface == iterator->surface;
		iterator->next_edge_index = edge->next_edges[iterator->forward];
		if (iterator->next_edge_index == iterator->first_edge_index)
			iterator->next_edge_index = (word)NONE;
found:
		if (iterator->forward)
			iterator->vertex = edge->vertices[0];
		else
			iterator->vertex = edge->vertices[1];
	}

	if (++iterator->count > 2000)
		edge = 0;
	return edge;
}

// @retail 0x1fa7f0
long function_1fa7f0(void)
{
	s_4e0348_view *globals = (s_4e0348_view *)g_4e0348;
	long result = 0;

	if (globals->count > 0)
		result = globals->first;
	return result;
}

/* the contact at a surface and its orientation (2..5 by the larger of i and j) */
struct s_contact
{
	byte unknown00[0xdc];
	vector3f normal;
};

struct s_contact_result
{
	long unknown0;
	long orientation;
};

// @retail 0x1faeb0
void function_1faeb0(s_contact const *contact, s_contact_result *result)
{
	result->orientation = 1;
	if (fabs(contact->normal.i) < fabs(contact->normal.j))
	{
		if (contact->normal.j < -0.0001f)
			result->orientation = 2;
		else if (contact->normal.j > 0.0001f)
			result->orientation = 3;
	}
	else
	{
		if (contact->normal.i < -0.0001f)
			result->orientation = 5;
		else if (contact->normal.i > 0.0001f)
			result->orientation = 4;
	}
}

/* a timer of 1.5 seconds and a zero vector */
struct s_1faf30_timer
{
	long ticks;
	vector3f vector;
};

// @retail 0x1faf30
void function_1faf30(s_1faf30_timer *timer)
{
	real seconds = (real)g_510c54->field_2_3 * 1.5f;
	long ticks;

	__asm
	{
		fld seconds
		fistp ticks
	}
	timer->ticks = ticks;
	timer->vector = *g_4687a4;
}

struct s_contact_surface_range
{
    long key;
    long first;
    long last;
    long unknown0c;
};

struct s_contact_surface_owner
{
    long unknown00;
    long first;
    long last;
    long range_count;
    s_contact_surface_range *ranges;
    byte unknown14[8];
};

struct s_contact_surface_table
{
    long count;
    byte unknown04[0x30 - 4];
    long owner_count;
    s_contact_surface_owner *owners;
    byte unknown38[8];
    long mapping_count;
    short (*mapping)[2];
};

struct s_bsp3d;
struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
short function_120850(vector3f const *v);
real *function_120810(real const *point, short axis, byte side, real *out);
short function_296230(long object_index);

// @retail 0x1fa3a0
long function_1fa3a0(long structure_index, long surface_index, long object_index,
    long node_index, point3f const *point)
{
    long const *node_reference = &node_index;
    long result = NONE;
    long type = object_index != NONE ? 4 : (structure_index != NONE ? 3 : 1);
    byte *globals = (byte *)g_4e0348;
    long table_count = *(long *)(globals + 0xc4);
    s_contact_surface_table *table = NULL;
    if (table_count > 0)
        table = *(s_contact_surface_table **)(globals + 0xc8);
    if (table)
    {
        switch (type)
        {
        case 4:
            {
                short owner_index = function_296230(object_index);
                if (owner_index >= 0 && owner_index < table->owner_count)
                {
                    s_contact_surface_owner *owner = &table->owners[owner_index];
                    for (short i = 0; i < owner->range_count; i++)
                    {
                        s_contact_surface_range *range = &owner->ranges[i];
                        if (range->key == *node_reference)
                        {
                            result = range->first;
                            if (result == NONE || surface_index < 0 || surface_index > range->last - result)
                                result = NONE;
                            else
                            {
                                result += surface_index;
                                if (result < 0 || result >= table->count)
                                    result = NONE;
                            }
                            break;
                        }
                    }
                }
            }
            break;
        case 3:
            if (structure_index >= 0 && structure_index < table->mapping_count)
            {
                short owner_index = table->mapping[structure_index][0];
                if (owner_index >= 0 && owner_index < table->owner_count)
                {
                    s_contact_surface_owner *owner = &table->owners[owner_index];
                    long first = owner->first;
                    if (first == NONE || surface_index < 0 || surface_index > owner->last - first)
                        result = NONE;
                    else
                    {
                        result = first + surface_index;
                        if (result < 0 || result >= table->count)
                            result = NONE;
                    }
                }
            }
            break;
        case 1:
            {
                s_slot_entry_list *bsp = g_4e0340;
                if (bsp && surface_index != NONE)
                {
                    long *leaf = function_1fa5e0(surface_index, (s_tree2d *)table, object_index);
                    if (leaf)
                    {
                        byte *surfaces = *(byte **)((byte *)bsp + 0x2c);
                        plane3f plane;
                        bsp3d_get_plane((s_bsp3d *)bsp, *(short *)(surfaces + surface_index * 8), &plane);
                        short axis = function_120850((vector3f *)&plane);
                        byte side = ((real *)&plane)[axis] > 0.0f;
                        point2f projected;
                        function_120810((real const *)point, axis, side, (real *)&projected);
                        result = function_1fa590(*leaf, (s_tree2d *)table, &projected);
                    }
                }
            }
            break;
        }
    }
    return result;
}

real g_557c70;
real function_30bf0(vector3f *v);
void function_c7840(transform4x3f const *frame, vector3f *current, vector3f const *desired, real rate,
    vector3f *velocity, real const *limits, real max_speed, real acceleration);

// @retail 0x1faf80
bool function_1faf80(long object_index, void const *settings_pointer, vector3f const *control,
    real threshold, vector3f *facing, real *turn)
{
    s_slot_object_view *object = object_get(object_index);
    real const *settings = (real const *)settings_pointer;
    vector3f desired;
    if (object->velocity.i * object->velocity.i + object->velocity.j * object->velocity.j +
        object->velocity.k * object->velocity.k < 0.25f &&
        ((vector3f *)object->unknown094)->i * ((vector3f *)object->unknown094)->i + ((vector3f *)object->unknown094)->j * ((vector3f *)object->unknown094)->j + ((vector3f *)object->unknown094)->k * ((vector3f *)object->unknown094)->k < g_557c70 * g_557c70 &&
        control->i * control->i + control->j * control->j + control->k * control->k < 0.0001f &&
        object->forward.k * facing->k + object->forward.j * facing->j + object->forward.i * facing->i > threshold)
        desired = object->forward;
    else
    {
        real vertical = settings[3] * control->k;
        desired = *facing;
        if (vertical != 0.0f)
        {
            desired.k += vertical;
            if (function_30bf0(&desired) == 0.0f)
                desired = *facing;
        }
    }
    vector3f *forward = &object->forward;
    vector3f const *up = (vector3f const *)((byte *)object + 0x7c);
    vector3f left;
    left.i = forward->j * up->k - *(real const *)((byte *)object + 0x80) * forward->k;
    left.j = up->i * forward->k - up->k * object->forward.i;
    left.k = up->j * forward->i - up->i * forward->j;
    real value = (facing->k * left.k + facing->j * left.j + facing->i * left.i) * 3.3333332538604736f;
    value *= control->i;
    value -= control->j;
    if (value > 1.5f)
        value = 1.5f;
    real target = settings[0] * value;
    real fraction;
    if (*turn * target <= 0.0f)
        fraction = 1.0f;
    else if ((target >= 0.0f ? target : -target) > 0.001f)
    {
        real ratio = *turn / target;
        fraction = 1.0f - (ratio > 1.0f ? 1.0f : ratio);
    }
    else
        fraction = 0.0f;
    real duration = ((1.0f - fraction) * settings[2] + settings[1] * fraction) * (real)g_510c54->field_2_3;
    if (duration > 0.0f)
        target = (target - *turn) / duration + *turn;
    *turn = target;
    real limits[4] = { -3.1415927410125732f, 3.1415927410125732f, -1.5707963705062866f, 1.5707963705062866f };
    if (settings[9] == 0.0f)
        *forward = desired;
    else
        function_c7840(NULL, forward, &desired, g_510c54->rate, (vector3f *)object->unknown094, limits, settings[8], settings[9]);
    return true;
}

PRIVATE __forceinline void motion_cross(vector3f const &a, vector3f const &b, vector3f *result)
{
    result->i = a.j * b.k - a.k * b.j;
    result->j = a.k * b.i - a.i * b.k;
    result->k = a.i * b.j - a.j * b.i;
}

// score 214 (was 251): frame 0x28 agrees; trig homes and projection registers differ;
// shared trig scope 254; sine declared first 251; forward projection loop 235;
// rotation loop 246; idle base loop 235; projection plus base loops 235; all loops 246.
// pointer scale loop 239; fixed do scale 237; idle reconstruction loop 235;
// two-component idle projection loop 235; native bounds pair 235.
// global optimization off 721; size optimization 409; inline depth zero 362;
// active rotation scope 235; idle sine after scaling 235.
// active/idle/both real trig 233/235/233; shared weighted axis 214 plain kept;
// weighted cross 218; both weights 258; output cosine scale 235; real trig plus axis 226.
// score 214 (was 214): Frame 0x28 agrees; angle/trig homes and projection scheduling remain. Active angle
// name 214; external up reference 212; forward reference 216; both output references
// 178 (held: reference aliases of the two parameters); external velocity reference 214.
// @retail 0x1faa20
void function_1faa20(vector3f *up, byte const *state, vector3f *forward, long *ticks)
{
    if (*ticks > 0)
    {
        *up = *(vector3f const *)(state + 0x100);
        *forward = *(vector3f const *)(state + 0xf4);
        byte *settings = *(byte **)(state + 8);
        real limits[4] = { -3.1415927410125732f, 3.1415927410125732f, -3.1415927410125732f, 3.1415927410125732f };
        vector3f *velocity = (vector3f *)(ticks + 1);
        function_c7840(NULL, up, g_4687b0, g_510c54->rate, velocity, limits,
            *(real *)(settings + 0x88), *(real *)(settings + 0x8c));
        vector3f axis = *velocity;
        real angle = function_30bf0(&axis) * g_510c54->rate;
        real cosine = (real)cos(angle);
        real sine = (real)sin(angle);
        real dot = (up->j * axis.j + up->k * axis.k + up->i * axis.i) * (1.0f - cosine);
        vector3f cross;
        motion_cross(*up, axis, &cross);
        vector3f_scale(&axis, dot, &axis);
        up->i = up->i * cosine + axis.i - cross.i * sine;
        up->j = up->j * cosine + axis.j - cross.j * sine;
        up->k = up->k * cosine + axis.k - cross.k * sine;
        real projection = up->k * forward->k + up->j * forward->j + forward->i * up->i;
        forward->i -= up->i * projection;
        forward->j -= up->j * projection;
        forward->k -= up->k * projection;
        if (function_30bf0(forward) == 0.0f)
        {
            *up = *g_4687b0;
            *forward = *g_4687a8;
        }
        if (angle == 0.0f)
            *ticks = 0;
    }
    else
    {
        vector3f axis;
        motion_cross(*forward, *g_4687b0, &axis);
        vector3f base;
        motion_cross(axis, *forward, &base);
        if (function_30bf0(&base) == 0.0f)
        {
            base = *g_4687a8;
            axis = *g_4687ac;
        }
        real angle = *(real const *)(state + 0x28);
        real cosine = (real)cos(angle);
        real sine = (real)sin(angle);
        for (long component = 0; component < 3; ++component)
            base.n[component] *= cosine;
        function_30bf0(&axis);
        up->i = axis.i * sine + base.i;
        up->j = axis.j * sine + base.j;
        up->k = axis.k * sine + base.k;
        function_30bf0(forward);
        real projection = up->k * forward->k + up->j * forward->j + forward->i * up->i;
        up->i -= forward->i * projection;
        up->j -= projection * forward->j;
        up->k -= projection * forward->k;
        function_30bf0(up);
    }
}
