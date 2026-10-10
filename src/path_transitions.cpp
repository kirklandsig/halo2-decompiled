// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259a0.h"
#include "unknown_20fe20.h"
#include <float.h>

real function_30bf0(vector3f *v);
long function_1e4a50(long index);
bool function_10f9b0(long unit_index, long mode, long set, long flags, transform4x3f *matrix, bool any_weapon);
void function_141590(transform4x3f const *in, transform4x3f *out);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
bool function_fa1a0(real speed, real gravity_scale, point3f const *origin, point3f const *target,
    real *minimum_speed, real const *time_scale, real const *forced_speed, bool high_arc,
    vector3f *direction, real *speed_out, real *time_out, real *distance, real *vertical_speed, real *horizontal_speed);

/* Settings read through g_4e034c +0xc8/+0xcc. */
struct s_transition_settings
{
    byte unknown00[0x80];
    real speeds[6];
    real maximum_rises[6];
    real_bounds type_bounds[3];
    byte unknownc8[0xe0 - 0xc8];
    real_bounds mode_bounds[2];
};
struct s_transition_globals
{
    byte unknown00[0xc8];
    long count;
    s_transition_settings *settings;
};
struct s_transition_character
{
    byte unknown00[0x1c];
    word marker_flags;
};
struct s_transition_surface
{
    short type, next;
    word vertices[4];
    byte unknown0c[2];
    word destination_node;
    short source_output, destination_output;
};
struct s_transition_path_block
{
    byte unknown00[0xc4];
    long count;
    s_pathfinding_data *data;
};

/* The shared collision prefix is 0x4c bytes; retail 0x26d100 also reads
   +0x50 after its collision call. Keep the complete buffer local. */
struct s_transition_collision
{
    s_collision_result_1697c0 prefix;
    byte unknown4c[0x10];
};

// @retail 0x26f150
bool function_26f150(short type, point3f const *start, point3f const *end,
    point3f const *alternate_start, point3f const *alternate_end)
{
    real speed = 0.f;
    s_transition_globals *globals = (s_transition_globals *)g_4e034c;
    if (type >= 0 && type < 6 && globals && globals->count > 0)
        speed = globals->settings->speeds[type];
    if (end->z > start->z || (alternate_start && alternate_end && alternate_start->z > alternate_end->z))
    {
        real maximum = 0.f;
        if (type >= 0 && type < 6 && globals && globals->count > 0)
            maximum = globals->settings->maximum_rises[type];
        real rise = end->z - start->z;
        if (alternate_start && alternate_end && !(rise > alternate_start->z - alternate_end->z))
            rise = alternate_start->z - alternate_end->z;
        if (maximum != 0.f && !(maximum > rise))
            return false;
    }
    vector3f direction;
    bool result = function_fa1a0(speed, 1.f, start, end, NULL, NULL, NULL, true,
        &direction, NULL, NULL, NULL, NULL, NULL);
    if (result && alternate_start && alternate_end)
        result = function_fa1a0(speed, 1.f, alternate_end, alternate_start, NULL, NULL, NULL,
            true, &direction, NULL, NULL, NULL, NULL, NULL);
    return result;
}

// @retail 0x26f290
real function_26f290(short type)
{
    real result = 0.f;
    s_transition_globals *globals = (s_transition_globals *)g_4e034c;
    if (type >= 0 && type < 6 && globals && globals->count > 0)
        result = globals->settings->speeds[type];
    return result;
}

// @retail 0x26f2d0
bool function_26f2d0(s_path_settings const *settings, point3f const *start, point3f const *end)
{
    long flags = ((dword)settings->unknown04 >> 10) & 6;
    s_transition_globals *globals = (s_transition_globals *)g_4e034c;
    bool result = false;
    for (short i = 0; i < 3; i++)
    {
        if ((flags & (1 << i)) && i >= 0 && i < 3 && globals && globals->count > 0)
        {
            real lo = globals->settings->type_bounds[i].lo;
            real hi = globals->settings->type_bounds[i].hi;
            real height = end->z - start->z;
            result = height >= lo && height <= hi;
            if (result) break;
        }
    }
    return result;
}

// @retail 0x26f360
bool function_26f360(s_path_settings const *settings, point3f const *start, point3f const *end)
{
    long flags = settings->unknown04 & 3;
    s_transition_globals *globals = (s_transition_globals *)g_4e034c;
    bool result = false;
    for (short i = 0; i < 2; i++)
    {
        if ((flags & (1 << i)) && i >= 0 && i < 2 && globals && globals->count > 0)
        {
            real lo = globals->settings->mode_bounds[i].lo;
            real hi = globals->settings->mode_bounds[i].hi;
            real height = end->z - start->z;
            result = height >= lo && height <= hi;
            if (result) break;
        }
    }
    return result;
}

// @retail 0x26f8f0
bool function_26f8f0(s_object_marker const *marker)
{
    bool result = false;
    if (g_4687b0->k * marker->matrix.forward.k + g_4687b0->j * marker->matrix.forward.j +
        marker->matrix.forward.i * g_4687b0->i > 0.95f)
        result = true;
    return result;
}

// @retail 0x26f930
short function_26f930(long object_index, s_object_marker *markers, short capacity, long flags)
{
    short count = 0;
    if (flags & 1)
        count = function_b8d30(object_index, 0x0f000526, markers, capacity, false);
    if (count < capacity && (flags & 2))
        count += function_b8d30(object_index, 0x11000527, markers + count, capacity - count, false);
    return count;
}

// @retail 0x26fbf0
short function_26fbf0(long object_index, s_object_marker *markers, short capacity, long flags)
{
    short count = 0;
    if (flags & 0x20)
        count = function_b8d30(object_index, 0x0f00052a, markers, capacity, false);
    if (count < capacity && (flags & 0x40))
        count += function_b8d30(object_index, 0x1100052b, markers + count, capacity - count, false);
    if (count < capacity && (flags & 0x80))
        count += function_b8d30(object_index, 0x1000052c, markers + count, capacity - count, false);
    return count;
}

// @retail 0x26fe20
short function_26fe20(long object_index, s_object_marker *markers, short capacity, long flags)
{
    short count = 0;
    if (flags & 0x800)
        count = function_b8d30(object_index, 0x1100052f, markers, capacity, false);
    if (count < capacity && (flags & 0x1000))
        count += function_b8d30(object_index, 0x10000530, markers + count, capacity - count, false);
    return count;
}

// @retail 0x270130
bool function_270130(s_object_marker const *marker, point3f const *position, real margin, point3f *out)
{
    if (marker->unknown6c > 0.f)
    {
        vector3f delta;
        delta.k = position->z - marker->matrix.position.z;
        delta.j = position->y - marker->matrix.position.y;
        delta.i = position->x - marker->matrix.position.x;
        real distance = marker->matrix.up.k * delta.k + marker->matrix.up.j * delta.j +
            delta.i * marker->matrix.up.i;
        if (distance < 0.f) distance = 0.f;
        else if (distance > marker->unknown6c) distance = marker->unknown6c;
        if (marker->unknown6c > margin * 2.f)
        {
            if (margin > distance) distance = margin;
            else if (distance > marker->unknown6c - margin) distance = marker->unknown6c - margin;
        }
        else
        {
            if (distance < 0.f) distance = 0.f;
            else if (distance > marker->unknown6c) distance = marker->unknown6c;
        }
        out->x = distance * marker->matrix.up.i + marker->matrix.position.x;
        out->y = distance * marker->matrix.up.j + marker->matrix.position.y;
        out->z = distance * marker->matrix.up.k + marker->matrix.position.z;
    }
    else
        *out = marker->matrix.position;
    return true;
}

// @retail 0x26fc80
bool function_26fc80(long actor_index, long object_index, real distance, void *path, point3f *point)
{
    s_actor_view *actor = actor_get(actor_index);
    s_transition_character *character = (s_transition_character *)function_1e4a50(actor->unknown054);
    if (character)
    {
        s_object_marker markers[32];
        real best_distance = FLT_MAX;
        short best = NONE;
        short count = function_26fbf0(object_index, markers, 32, character->marker_flags);
        for (short i = 0; i < count; i++)
        {
            s_object_marker *marker = &markers[i];
            real alignment = marker->matrix.forward.i * g_4687b0->i +
                marker->matrix.forward.k * g_4687b0->k + g_4687b0->j * marker->matrix.forward.j;
            if (alignment > 0.95f)
            {
                real y = actor->position.y - marker->matrix.position.y;
                real x = actor->position.x - marker->matrix.position.x;
                real z = actor->position.z - marker->matrix.position.z;
                real squared = z * z + y * y + x * x;
                if (best_distance > squared) { best_distance = squared; best = i; }
            }
        }
        if (best != NONE)
        {
            s_object_marker *out = (s_object_marker *)path;
            *out = markers[best];
            if (point && !function_270130(out, &actor->position, distance, point))
                *point = out->matrix.position;
            return true;
        }
    }
    return false;
}

// @retail 0x26fe90
bool function_26fe90(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point)
{
    s_actor_view *actor = actor_get(actor_index);
    s_transition_character *character = (s_transition_character *)function_1e4a50(actor->unknown054);
    if (character)
    {
        s_object_marker markers[32];
        real best_distance = FLT_MAX;
        short best = NONE;
        short count = function_26fe20(object_index, markers, 32, character->marker_flags);
        for (short i = 0; i < count; i++)
        {
            s_object_marker *marker = &markers[i];
            real alignment = marker->matrix.forward.i * g_4687b0->i +
                marker->matrix.forward.k * g_4687b0->k + marker->matrix.forward.j * g_4687b0->j;
            if (alignment > 0.95f)
            {
                vector3f segment;
                segment.i = marker->matrix.up.i * marker->unknown6c;
                segment.j = marker->matrix.up.j * marker->unknown6c;
                segment.k = marker->matrix.up.k * marker->unknown6c;
                real squared_length = segment.k * segment.k + segment.j * segment.j + segment.i * segment.i;
                real squared;
                if (squared_length > 0.0001f)
                {
                    real y = actor->position.y - marker->matrix.position.y;
                    real z = actor->position.z - marker->matrix.position.z;
                    real x = actor->position.x - marker->matrix.position.x;
                    real t = (z * segment.k + y * segment.j + x * segment.i) / squared_length;
                    if (t < 0.f) t = 0.f;
                    else if (t > 1.f) t = 1.f;
                    real negative = 0.f - t;
                    real dy = segment.j * negative + y;
                    real dz = segment.k * negative + z;
                    real dx = negative * segment.i + x;
                    squared = dz * dz + dy * dy + dx * dx;
                }
                else
                {
                    real y = marker->matrix.position.y - actor->position.y;
                    real x = marker->matrix.position.x - actor->position.x;
                    real z = marker->matrix.position.z - actor->position.z;
                    squared = z * z + y * y + x * x;
                }
                if (best_distance > squared) { best_distance = squared; best = i; }
            }
        }
        if (best != NONE)
        {
            *out = markers[best];
            if (point && !function_270130(out, &actor->position, distance, point))
                *point = out->matrix.position;
            return true;
        }
    }
    return false;
}

// @retail 0x26f990
bool function_26f990(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point)
{
    s_actor_view *actor = actor_get(actor_index);
    s_transition_character *character = (s_transition_character *)function_1e4a50(actor->unknown054);
    if (character)
    {
        s_object_marker markers[32];
        real best_distance = FLT_MAX;
        short best = NONE;
        point3f best_point;
        short count = function_26f930(object_index, markers, 32, character->marker_flags);
        for (short i = 0; i < count; i++)
        {
            s_object_marker *marker = &markers[i];
            if (marker->matrix.forward.i * g_4687b0->i + marker->matrix.forward.k * g_4687b0->k +
                marker->matrix.forward.j * g_4687b0->j > 0.95f)
            {
                point3f candidate;
                function_270130(marker, &actor->position, distance, &candidate);
                vector3f delta;
                delta.i = actor->position.x - candidate.x;
                delta.j = actor->position.y - candidate.y;
                delta.k = actor->position.z - candidate.z;
                real length = (real)sqrt(delta.k * delta.k + delta.j * delta.j + delta.i * delta.i);
                if (!(fabs(length) < 0.0001f))
                {
                    real inverse = 1.f / length;
                    delta.i = inverse * delta.i;
                    delta.j = delta.j * inverse;
                    delta.k = delta.k * inverse;
                }
                else length = 0.f;
                real dot = delta.k * marker->matrix.left.k + delta.j * marker->matrix.left.j +
                    delta.i * marker->matrix.left.i;
                length *= (real)(3.f / (1.f + 2.f * fabs(dot)));
                if (best_distance > length)
                {
                    best_distance = length;
                    best = i;
                    best_point = candidate;
                }
            }
        }
        if (best != NONE)
        {
            *out = markers[best];
            if (point) *point = best_point;
            return true;
        }
    }
    return false;
}

// @retail 0x270240
bool function_270240(long object_index, long mode, long set, point3f const *position,
    vector3f const *forward, transform4x3f *out)
{
    bool result = false;
    /* Keep both matrix inputs in one scratch object: the existing matrix
       multiply uses register-only inline assembly to read its inputs. */
    struct { transform4x3f local, world; } matrices;
    if (function_10f9b0(object_index, mode, set, 3, &matrices.local, false))
    {
        function_1420f0(&matrices.world, position, forward, g_4687b0);
        function_142a60(&matrices.world, &matrices.local, out);
        result = true;
    }
    return result;
}

// @retail 0x2702a0
bool function_2702a0(long actor_index, long mode, long set, long target_mode, long target_set,
    point3f const *position, vector3f const *forward, point3f *out_position,
    long *location, vector3f *out_forward, long *node_index)
{
    point3f *const *reference = &out_position;
    bool result = false;
    point3f const *const *position_reference = &position;
    s_actor_view *actor = actor_get(actor_index);
    struct { transform4x3f local, inverse, source; } matrices;
    if (function_270240(actor->unknown018, mode, set, *position_reference, forward, &matrices.source) &&
        function_10f9b0(actor->unknown018, target_mode, target_set, 3, &matrices.local, false))
    {
        function_141590(&matrices.local, &matrices.inverse);
        function_142a60(&matrices.source, &matrices.inverse, &matrices.local);
        if (*reference) **reference = matrices.local.position;
        if (out_forward) *out_forward = matrices.local.forward;
        if (location || node_index)
        {
            s_transition_path_block *block = (s_transition_path_block *)g_4e0348;
            s_pathfinding_data *volatile data = NULL;
            if (block->count > 0) data = block->data;
            s_transition_collision collision;
            collision.prefix.unknown24 = NONE;
            long node = function_26d100(g_4687b0, location, &collision.prefix, &matrices.local.position);
            if (node_index) *node_index = node;
        }
        result = true;
    }
    return result;
}

// @retail 0x270400
bool function_270400(long actor_index, long mode, long set, point3f const *position,
    vector3f const *forward, point3f *out_position, long *location, vector3f *out_forward, long *node_index)
{
    point3f const *const *position_reference = &position;
    bool result = false;
    transform4x3f matrix;
    if (actor_index != NONE)
    {
        s_actor_view *actor = actor_get(actor_index);
        if (function_270240(actor->unknown018, mode, set, *position_reference, forward, &matrix))
        {
            if (out_position) *out_position = matrix.position;
            if (out_forward) *out_forward = matrix.forward;
            if (location || node_index)
            {
                s_transition_path_block *block = (s_transition_path_block *)g_4e0348;
                s_pathfinding_data *volatile data = NULL;
                if (block->count > 0) data = block->data;
                s_transition_collision collision;
                collision.prefix.unknown24 = NONE;
                long node = function_26d100(g_4687b0, location, &collision.prefix, &matrix.position);
                if (node_index) *node_index = node;
            }
            result = true;
        }
    }
    else
    {
        if (out_position) *out_position = **position_reference;
        if (out_forward) *out_forward = *forward;
        if (node_index || location)
        {
            s_transition_path_block *block = (s_transition_path_block *)g_4e0348;
            s_pathfinding_data *volatile data = NULL;
            if (block->count > 0) data = block->data;
            s_transition_collision collision;
            collision.prefix.unknown24 = NONE;
            long node = function_26d100(g_4687b0, location, &collision.prefix, *position_reference);
            if (node_index) *node_index = node;
        }
        result = true;
    }
    return result;
}

// @retail 0x26f3f0
bool function_26f3f0(s_pathfinding_data *pathfinding, long surface_index,
    s_type_c3b527 const *entry, long actor_index, s_type_f17a25 *state,
    s_type_c3b527 const *parent_point, long parent_node_index,
    long *parent_node_index_out, s_type_c3b527 *out, long *out_node_index)
{
    /* The existing caller's declaration uses const, but retail writes this
       node field: 0x270ffb passes its mutable node +0x18 in EDI. */
    s_type_c3b527 *start_out = const_cast<s_type_c3b527 *>(entry);
    s_transition_surface *surface = (s_transition_surface *)&pathfinding->surfaces[surface_index];
    bool volatile result = false;
    switch (surface->type)
    {
    case 1:
    case 6:
        if (parent_point && (long)surface->vertices[0] != NONE && (long)surface->vertices[1] != NONE &&
            (long)surface->vertices[2] != NONE && (long)surface->vertices[3] != NONE)
        {
            point3f const *a = &pathfinding->vertices[surface->vertices[0]];
            point3f const *b = &pathfinding->vertices[surface->vertices[1]];
            point3f const *c = &pathfinding->vertices[surface->vertices[2]];
            point3f const *d = &pathfinding->vertices[surface->vertices[3]];
            vector3f first, second;
            first.i = b->x - a->x;
            first.j = b->y - a->y;
            first.k = b->z - a->z;
            real first_length = function_30bf0(&first);
            if (first_length > 0.f)
            {
                second.i = d->x - c->x;
                second.j = d->y - c->y;
                second.k = d->z - c->z;
                real second_length = function_30bf0(&second);
                if (second_length > 0.f)
                {
                    real projected = parent_point->point.y * first.j + parent_point->point.z * first.k +
                        parent_point->point.x * first.i;
                    real origin = first.j * a->y + a->z * first.k + a->x * first.i;
                    real inverse = 1.f / first_length;
                    real fraction = (projected - origin) * inverse;
                    if (first_length > state->source.radius * 2.f)
                    {
                        real margin = inverse * state->source.radius;
                        if (margin > fraction) fraction = margin;
                        else if (fraction > 1.f - margin) fraction = 1.f - margin;
                    }
                    else fraction = 0.5f;
                    real first_distance = fraction * first_length;
                    real second_distance = fraction * second_length;
                    start_out->point.x = first.i * first_distance + a->x;
                    start_out->point.y = first.j * first_distance + a->y;
                    start_out->point.z = first.k * first_distance + a->z;
                    out->point.x = second.i * second_distance + c->x;
                    out->point.y = second.j * second_distance + c->y;
                    out->point.z = second.k * second_distance + c->z;
                    start_out->output_index = parent_point->output_index;
                    out->output_index = surface->destination_output;
                    *parent_node_index_out = parent_node_index;
                    *out_node_index = surface->destination_node;
                    result = true;
                    break;
                }
            }
            start_out->point = *a;
            start_out->output_index = parent_point->output_index;
            *parent_node_index_out = parent_node_index;
            out->point = *c;
            out->output_index = surface->destination_output;
            *out_node_index = surface->destination_node;
            result = true;
        }
        break;
    case 5:
        {
            point3f const *position = &pathfinding->vertices[surface->vertices[2]];
            point3f const *direction_point = &pathfinding->vertices[surface->vertices[3]];
            vector3f direction = *(vector3f const *)direction_point;
            direction.i = direction.i * -1.f;
            direction.j = direction.j * -1.f;
            direction.k = direction.k * -1.f;
            if (actor_index != NONE && function_270400(actor_index, 0x05000534, 0x05000049,
                position, &direction, NULL, (long *)start_out, NULL, parent_node_index_out))
            {
                out->point = *position;
                out->output_index = parent_point->output_index;
                *out_node_index = surface->vertices[0];
                result = true;
                break;
            }
            *start_out = *parent_point;
            *parent_node_index_out = parent_node_index;
            out->point = *position;
            out->output_index = parent_point->output_index;
            *out_node_index = surface->vertices[0];
            result = true;
        }
        break;
    case 2:
        {
            s_pathfinding_data *data = (s_pathfinding_data *)state->pathfinding;
            point3f const *direction_point = &data->vertices[surface->vertices[2]];
            point3f const *position = &data->vertices[surface->vertices[1]];
            out->point = *position;
            out->output_index = parent_point->output_index;
            *out_node_index = parent_node_index;
            if (!function_270400(actor_index, 0x050000cb, 0x060000cc, position,
                (vector3f const *)direction_point, NULL, (long *)start_out, NULL, parent_node_index_out))
            {
                s_path_trace_result trace;
                function_26c590(data, position, parent_node_index, NONE,
                    (vector3f const *)direction_point, 0.2f, NULL, &trace);
                start_out->point = trace.point;
                start_out->output_index = parent_point->output_index;
                *parent_node_index_out = ((s_sector_trace_result *)&trace)->sector_index;
            }
            result = true;
        }
        break;
    }
    return result;
}
