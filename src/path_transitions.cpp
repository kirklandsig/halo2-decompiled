// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259a0.h"
#include "unknown_20fe20.h"
#include "normalize3d.h"
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
    real maximum_drops[6];
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
bool function_26f150(short type, point3f const *start, point3f const *alternate_start,
    point3f const *end, point3f const *alternate_end)
{
    real speed = 0.f;
    if (type >= 0 && type < 6 && ((s_transition_globals *)g_4e034c) && ((s_transition_globals *)g_4e034c)->count > 0)
        speed = ((s_transition_globals *)g_4e034c)->settings->speeds[type];
    if (start->z > end->z || (alternate_start && alternate_end && alternate_start->z > alternate_end->z))
    {
        real maximum = 0.f;
        if (type >= 0 && type < 6 && ((s_transition_globals *)g_4e034c) && ((s_transition_globals *)g_4e034c)->count > 0)
            maximum = ((s_transition_globals *)g_4e034c)->settings->maximum_drops[type];
        real drop = start->z - end->z;
        if (alternate_start && alternate_end && !(drop > alternate_start->z - alternate_end->z))
            drop = alternate_start->z - alternate_end->z;
        if (maximum != 0.f && !(maximum > drop))
            return false;
    }
    vector3f direction;
    bool result = function_fa1a0(speed, 1.f, start, end, NULL, NULL, NULL, true,
        &direction, NULL, NULL, NULL, NULL, NULL);
    if (result && alternate_start && alternate_end)
        result = function_fa1a0(speed, 1.f, alternate_start, alternate_end, NULL, NULL, NULL,
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
bool function_26f2d0(point3f const *start, point3f const *end, s_path_settings const *settings)
{
    bool result = false;
    long flags = ((dword)settings->unknown04 >> 10) & 6;
    for (short i = 0; i < 3; i++)
    {
        if ((flags & (1 << i)) && i >= 0 && i < 3 && ((s_transition_globals *)g_4e034c) && ((s_transition_globals *)g_4e034c)->count > 0)
        {
            real_bounds bounds = { ((s_transition_globals *)g_4e034c)->settings->type_bounds[i].lo,
                ((s_transition_globals *)g_4e034c)->settings->type_bounds[i].hi };
            real height = end->z - start->z;
            result = height >= bounds.lo && height <= bounds.hi;
            if (result) break;
        }
    }
    return result;
}

// @retail 0x26f360
bool function_26f360(point3f const *start, point3f const *end, s_path_settings const *settings)
{
    bool result = false;
    long flags = settings->unknown04 & 3;
    s_transition_globals *globals = (s_transition_globals *)g_4e034c;
    for (short i = 0; i < 2; i++)
    {
        if ((flags & (1 << i)) && i >= 0 && i < 2 && globals && globals->count > 0)
        {
            real_bounds bounds = { globals->settings->mode_bounds[i].lo, globals->settings->mode_bounds[i].hi };
            real height = end->z - start->z;
            result = height >= bounds.lo && height <= bounds.hi;
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
short function_26f930(long object_index, s_object_marker *markers, short capacity, word flags)
{
    short count = 0;
    if (flags & 1)
        count = function_b8d30(object_index, 0x0f000526, markers, capacity, false);
    if (count < capacity && (flags & 2))
        count += function_b8d30(object_index, 0x11000527, markers + count, capacity - count, false);
    return count;
}

// @retail 0x26fbf0
short function_26fbf0(long object_index, s_object_marker *markers, short capacity, word flags)
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
short function_26fe20(long object_index, s_object_marker *markers, short capacity, word flags)
{
    short count = 0;
    if (flags & 0x800)
        count = function_b8d30(object_index, 0x1100052f, markers, capacity, false);
    if (count < capacity && (flags & 0x1000))
        count += function_b8d30(object_index, 0x10000530, markers + count, capacity - count, false);
    return count;
}

/* the point at distance t along a vector from a point */
static inline void point_along_vector(point3f const *point, vector3f const *vector, real t, point3f *result)
{
    result->x = vector->i * t + point->x;
    result->y = vector->j * t + point->y;
    result->z = vector->k * t + point->z;
}

// @retail 0x270130
bool function_270130(s_object_marker const *marker, point3f const *position, real margin, point3f *out)
{
    if (marker->unknown6c > 0.f)
    {
        vector3f delta;
        vector3d_from_points3d(&marker->matrix.position, position, &delta);
        real distance = dot3f(&delta, &marker->matrix.up);
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
        point_along_vector(&marker->matrix.position, &marker->matrix.up, distance, out);
    }
    else
        *out = marker->matrix.position;
    return true;
}

// @retail 0x26fc80
bool function_26fc80(long actor_index, long object_index, real distance, void *path, point3f *point)
{
    bool result = false;
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
            real alignment = dot3f(&marker->matrix.forward, g_4687b0);
            if (alignment > 0.95f)
            {
                vector3f delta;
                vector3d_from_points3d(&marker->matrix.position, &actor->position, &delta);
                real squared = length_sq3f(&delta);
                if (best_distance > squared) { best_distance = squared; best = i; }
            }
        }
        if (best != NONE)
        {
            s_object_marker *out = (s_object_marker *)path;
            *out = markers[best];
            if (point && !function_270130(out, &actor->position, distance, point))
                *point = out->matrix.position;
            result = true;
        }
    }
    return result;
}

/* point-to-segment squared distance; retail inlines the same helper in 0x1a83e0 (twice)
   and 0x260670 (at 0x260c73), so whoever matches those should try this body */
static __forceinline void scale_vector3d_bx(vector3f const *vector, real scale, vector3f *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
}

static __forceinline real distance_squared_segment_bx(point3f const *start, vector3f const *segment, point3f const *point)
{
    real squared_distance;
    real length_squared = segment->k * segment->k + segment->j * segment->j + segment->i * segment->i;
    if (length_squared > 0.0001f)
    {
        /* delta is read and written only through a pointer, as segment is through the
           helpers: that keeps the projection's operands in retail's order (delta first)
           while the residual still updates the segment components in place */
        vector3f delta;
        vector3f *d = &delta;
        d->i = point->x - start->x;
        d->j = point->y - start->y;
        d->k = point->z - start->z;
        real t = (d->k * segment->k + d->j * segment->j + d->i * segment->i) / length_squared;
        real clamped;
        if (t < 0.f)
            clamped = 0.f;
        else if (t > 1.f)
            clamped = 1.f;
        else
            clamped = t;
        t = 0.f - clamped;
        d->i += segment->i * t;
        d->j += segment->j * t;
        d->k += segment->k * t;
        squared_distance = d->k * d->k + d->j * d->j + d->i * d->i;
    }
    else
    {
        vector3f offset;
        offset.i = start->x - point->x;
        offset.j = start->y - point->y;
        offset.k = start->z - point->z;
        squared_distance = offset.k * offset.k + offset.j * offset.j + offset.i * offset.i;
    }
    return squared_distance;
}

// @retail 0x26fe90
bool function_26fe90(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point)
{
    bool result = false;
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
            if (dot3f(&markers[i].matrix.forward, g_4687b0) > 0.95f)
            {
                vector3f segment;
                scale_vector3d_bx(&markers[i].matrix.up, markers[i].unknown6c, &segment);
                real squared = distance_squared_segment_bx(&markers[i].matrix.position, &segment, &actor->position);
                if (best_distance > squared)
                {
                    best_distance = squared;
                    best = i;
                }
            }
        }
        if (best != NONE)
        {
            *out = markers[best];
            if (point && !function_270130(out, &actor->position, distance, point))
                *point = out->matrix.position;
            result = true;
        }
    }
    return result;
}

// @retail 0x26f990
bool function_26f990(long actor_index, long object_index, real distance, s_object_marker *out, point3f *point)
{
    bool result = false;
    s_actor_view *actor = actor_get(actor_index);
    s_transition_character *character = (s_transition_character *)function_1e4a50(actor->unknown054);
    if (character)
    {
        s_object_marker markers[32];
        short best = NONE;
        real best_distance = FLT_MAX;
        point3f best_point;
        short count = function_26f930(object_index, markers, 32, character->marker_flags);
        for (short i = 0; i < count; i++)
        {
            if (dot3f(&markers[i].matrix.forward, g_4687b0) > 0.95f)
            {
                point3f candidate;
                function_270130(&markers[i], &actor->position, distance, &candidate);
                vector3f delta;
                vector3d_from_points3d(&candidate, &actor->position, &delta);
                real length = normalize3d(&delta);
                real dot = dot3f(&delta, &markers[i].matrix.left);
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
            result = true;
        }
    }
    return result;
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
    /* retail keeps out_position and position on the stack */
    bool result = false;
    point3f const *const *position_reference = &position;
    s_actor_view *actor = actor_get(actor_index);
    struct { transform4x3f local, inverse, source; } matrices;
    if (function_270240(actor->unknown018, mode, set, *position_reference, forward, &matrices.source) &&
        function_10f9b0(actor->unknown018, target_mode, target_set, 3, &matrices.local, false))
    {
        function_141590(&matrices.local, &matrices.inverse);
        function_142a60(&matrices.source, &matrices.inverse, &matrices.local);
        if (out_position) *out_position = matrices.local.position;
        if (out_forward) *out_forward = matrices.local.forward;
        if (location || node_index)
        {
            s_transition_path_block *block = (s_transition_path_block *)g_4e0348;
            s_pathfinding_data *volatile data = NULL;
            if (block->count > 0) data = block->data;
            s_transition_collision collision;
            collision.prefix.unknown24 = NONE;
            long node = function_26d100(g_4687b0, &matrices.local.position, location, &collision.prefix);
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
    /* retail keeps position on the stack */
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
                long node = function_26d100(g_4687b0, &matrix.position, location, &collision.prefix);
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
            long node = function_26d100(g_4687b0, *position_reference, location, &collision.prefix);
            if (node_index) *node_index = node;
        }
        result = true;
    }
    return result;
}

// score 218 (was 218; 219 before): Frame 0x40 and case 2 agree; the entry (actor loaded there, pathfinding moved to EBX), case 5's
// colours and the projection sum's order differ.
// MiniMax H (11-12): no new form, no lower score (218): the entry still differs - retail moves the pathfinding pointer into ebx and loads the actor index into eax before the switch, ours keeps the pathfinding pointer in eax and reads the actor index in the cases that use it; the frame and case 2 agree.
// MiniMax A (11-12): no new form, no lower score (218); retail copies the pathfinding argument into ebx at the entry and reloads the vertices pointer from that copy before ebx takes the first vertex index, and taking the case-2 vertices and the trace from the pathfinding argument instead of from state->pathfinding scores 265 and 294, so the root cannot be kept alive by the case-2 use.
// MiniMax C (11-11): no new form, no lower score (218); retail copies the pathfinding argument into EBX at the entry
// and reads the actor index (the first stack argument) into EAX there, while this build keeps the pathfinding in EAX
// and reloads the actor index at its case-5 use, so every case is seven bytes out of step; the actor test hoisted
// ahead of case 5's direction (286) and the direction built inside the actor test (336) both score worse and were
// reverted; swapping case 5's position and direction statements so the position comes first scores 265 and was
// reverted, so the case-5 statements stay in their retail order.
// MiniMax D (11-08): no new form, no lower score (219); retail keeps the first argument in EBX and reads the stack
// argument into EAX at the entry, ours uses EAX for the actor and ECX for the stack argument, and the switch's case
// bodies and the counter's register home still differ.
// MiniMax I (11-06): no new form, no lower score; the case-5 direction negated while it is copied from the vertex gives 379 and 391 against 219.
// MiniMax H (11-05): no new form, no lower score; the projection sum's order ((j + k) + i in retail,
// (k + j) + i here) comes from the shared dot3f in include/unknown_0259d0.h, and the entry still loads
// the actor in case 5 where retail loads it at entry and moves pathfinding into EBX.
// Earlier (270, was 278): Frame 0x40 agrees; early root EBX/actor ownership and vector scheduling differ.
// Round 132 (m3 lane 5, fast): case 5's success stores with the output index first 341; case 5's direction
// negated field by field from a vertex pointer 382 (1252 B); case 2's node-index store before the point copies
// 299; case 5 fallback's node-index store before the point copy 307; result declared before the surface 278.
// Earlier (278): frame 0x40 and length agree; early root EBX retention, actor ownership and paired-edge float/vector scheduling remain.
// Earlier: Frame 0x40 agrees; early root EBX retention and float/vector scheduling remain.
// Earlier 278 (was 278): frame 0x40 agrees; early pathfinding EBX ownership and float scheduling differ.
// entry root retention and floating-point scheduling differ;
// result first 299; surface first 299; vector names 299; vector helper 299;
// Earlier (278): frame 0x40 and length agree. Retail loads actor_index into EAX at entry (case
// 5 tests it first) and so moves pathfinding into EBX (pushed at entry); ours loads actor_index in
// case 5 and keeps pathfinding in EAX, which re-colours every case; paired-edge float/vector
// scheduling also differs. Tried: result first 299; surface first 299; vector names 299; vector helper 299;
// frame 0x40 and length agree. Retail moves pathfinding to EBX at entry and
// loads actor_index (first stack argument) into EAX there for case 5's test, which also forces
// result into the dead out slot; ours keeps pathfinding in EAX and loads the actor at its use.
// Everything after the entry follows (case 2 differs only in the pops).
// Tried: result first 299; surface first 299; vector names 299; vector helper 299;
// case order 300; separate vector declarations 299.
// Round 24: trace-time state reload, mutable entry, both, coherent pathfinding/surface swap,
// direct distance arguments and direct projected-origin 299 each; c/b/a/d pointer
// declarations 278; direct case-5 direction 278 (selected with the declaration order).
// Round 210: Four consistent vertex-index values in word/long/dword 338/344/344; word/long with
// the actor value at entry 338/344; word/long with both shared differences 346/360;
// both shared differences on the original c/b/a/d graph 297. All restored.
// Entry vertex table at case/dispatch scope 280/340; ordinary result 340;
// case table with ordinary result 342; early actor predicate 278; common case-5
// outputs 404, with case table 404, with ordinary result 398. Original body restored.
// Round 222: Complete point constructors: paired fallback 366; both case-5 copies 356; both 383; both case-2 copies 365; fallback/case-2 379; case-5/case-2 376; all 390; all with both vector setters 378. Frame 0x40 agrees; early root retention and vector scheduling remain. All restored.
// Actor-valid direction scope with whole-vector initialization, readonly surface and
// ordinary-result interactions: 284/334/284/334/363/357/363/357; restored.
// Complete external vertex observations in paired/actor/trace cases 427/402/370;
// paired+actor/paired+trace/actor+trace 475/475/448; all cases 519; all with
// ordinary result 495. Retained element receivers and original result restored.
// DH round 218: paired-edge vector aggregates/native surface reference/both 278/278/278;
// ordinary result on these three graphs 340/340/340; all endpoint role names with aggregates,
// with/without surface reference 278/278. Original body and qualifier restored.
// Round 187: complete parallel dot-prefix parts/two-use upper fraction/both 278/278/278;
// each and both with existing vector differences 297/297/297; complete incremental rows with
// differences 296; complete shared dot/difference producers 351 (sum order differs, match-only).
// All endpoints, qualifier, original rows and body restored.
// Round 434: parent value captured in paired/actor/trace cases 395/384/330; paired-actor/
// paired-trace/actor-trace 421/399/388; all cases 421, all with native radius snapshot 420.
// Parent records grow the frame to 0x44/0x4c/0x50; original field observations and body restored.
// Round 41 (fast): actor_index declared first (every declaration and call) 278; the actor test
// nested 278; projection terms in z, y, x order (both/one/other) 278; dot3f for both projections,
// either argument order, 278 (identical code). Diagnostic only, reverted: actor_index read at entry
// through a volatile cast gives retail's EBX entry (aligned 199 -> 134, masked 58 -> 39) but swaps
// the EAX/ECX parameters (318, 1284 bytes); a plain entry copy is propagated away (280).
// Round 46: one vertices base for the case 1/6 corners 280; writable start-out parameter 279;
// reversed entry declarations 278; case 5 fallback first 278; shared function-scope position 278.
// Diagnostics: no actor in case 2 326, extra actor tests 360, actor declared first 278 (no entry load).
// Round 6 (lane 17): the actor test as a function-scope bool used in case 5 364 (1292 bytes; aligned
// 200 -> 139: retail's `push ebx; mov ebx, eax` entry and the actor loaded at entry, but compared there
// with setne where retail keeps the value in EAX and compares in case 5); that bool at the top of case 5
// 278; a short type local for the switch 280; `(long)` switch operand 278; const surface pointer 278.
// Round 448: Call-site interpolation constructors (t01-t08) rejected as helper expansion;
// existing shared helper constructor with native origin/direction/output capture
// (t09-t16) 268/368/362/389/372/385/383/396/268/436/436/436/416/436/436/436.
// Original body and helpers restored; Frame 0x40 agrees; early root EBX/actor ownership and vector scheduling remain.
// Round 49 (lane 16, fast = plain 278 here): result without volatile 340 (1252 bytes); the start-out
// parameter declared mutable without the alias local 279 (as round 46); the two point distances
// passed inline 278, case 5's actor test as an outer if 278 and case 2 reading the state's
// pathfinding at each use 278 (all three same code). Retail loads actor_index into EAX at entry
// (an enregistered stack parameter), which pushes pathfinding into EBX; ours loads it at the use.
// Round 99 (lane 8): case 5 with the fallback in an `actor_index == NONE || !...` arm 278; the radius test
// as `radius * 2.f < first_length` 278; `margin = radius * inverse` 278; the half-way branch first 277;
// `if (!parent_point) break;` before the corner tests 278.
// Round 40 (reduced-relink screen, base 275): a case-5 `actor` copy, `*= -1.f`, the existing vector3f_scale for
// the negation, one vertices pointer for case 5, the actor test as a case-5 bool 275 each (same code); case 2
// before case 5 349; case 5's direction before its position 275 (aligned 198 -> 168: case 5's tails become
// retail's, case 2 is re-coloured). The corner pointers in all 24 declaration orders: a, b, c, d 290 (1285
// bytes, aligned 123: actor_index loaded into EAX at entry and pathfinding homed in EBX as retail, case 5
// exact, but pathfinding then arrives in EDX); a, b, d, c 301-305 (1285); b, a, c, d 303-312 (1277); the
// other 19 274-277. On a, b, c, d the projection and origin sums in three term orders each, with case 5's
// and case 2's declaration orders, compile identically (286-290).
// Round 125 (m3 lane 5, fast): retail's caller sets ECX (surface index), EAX (pathfinding), EDI (entry)
// in that order, as ours already does, so no parameter reorder has evidence. a, b, c, d re-measured 299
// (1284 bytes: actor index loaded into EAX at entry and pathfinding homed in EBX as retail, but
// pathfinding arrives in EDX); on it the surface declared first 299, the result declared first 299,
// a, b, d, c 314, one `vertices` base for the four corners 295.
// Round 106 (fast): the corner pointers a, b, c, d now 299 at 1284 bytes (aligned 142 -> 94, case 5 exact,
// but the actor load is the first instruction, so LTCG passes pathfinding in EDX, 0x270d90 313, and
// case 2's 0x26c590 setup is re-coloured). On that graph: result first 299; surface before start_out 299;
// case 5's direction before its position 295; result without volatile 371; the actor test as an outer if 299.
// Round 35 (lane 13): result declared first 278; surface pointer declared first 278; corners a, b, c, d 299
// (1284 bytes: actor_index loaded into EAX at entry and pathfinding moved to EBX as retail, but LTCG then passes
// pathfinding in EDX); with the surface pointer first 299; with case 5's direction copied before its position 295.
// Retail's caller sets up ECX, EAX, EDI in the same order as ours, so no parameter reorder is indicated.
// Round 36 (m3 lane 14, fast; aligned = difflib count, base 199): the a, b, c, d corners re-measured 299 (1284
// bytes, aligned 119; the corner code and case 5 become retail's, actor_index is loaded at entry, but pathfinding
// arrives in EDX). On them: the projection and origin sums grouped `(y + z) + x` 300 (aligned 105; grouped on
// the base form 278, same code); `first` and `second` through the existing vector3d_from_points3d 299 (1288,
// aligned 96), with both sums grouped 304, origin grouped only 304, the projection written vector-first 299;
// with that helper and both sums through dot3f 280 (1284, aligned 89: the projection, origin, distances and
// stores take retail's shape apart from first.j/first.k swapping registers; every dot3f argument order 280);
// dot3f with the corners' own differences 299 (aligned 119); dot3f projection with the origin written out,
// grouped or not, 304/299. Diagnostic: first's components assigned i, k, j give retail's `(j + k) + i`
// projection order (303; the stores reorder). On the dot3f form: fraction and margin as two divisions 280
// (same code: one reciprocal), the reciprocal written in both 280, the radius through a source pointer 280,
// the radius test ahead of the reciprocal 327, the reciprocal named before the projections 280, both reciprocal
// products written the other way round 280. The length
// (+4) is only the margin product: retail multiplies the reciprocal in place by the radius in memory
// (`mulss xmm2, [eax]`), ours loads the radius and multiplies by the reciprocal; with that and the entry's
// pathfinding register fixed, the dot3f form would align everything after case 1/6.
// Round 62: the corner pointers a, b, c, d 299 fast and plain (1284 bytes, aligned 119: actor_index loaded
// into EAX as the first instruction, so pathfinding arrives in EDX and moves to EBX after it; retail moves
// EAX to EBX first and loads the actor fourth); that with case 5's direction first 295; case 5's direction
// first alone 278 (aligned 169); a, b, c, d with the result declared first or the edge vectors declared
// before the corners 299 each. The only caller (0x270d90) does not change the convention in the plain build.
// Round 40 (lane 14, fast): a, b, c, d re-measured 299 (1284 bytes, actor load first, pathfinding in EDX,
// 0x270d90 313); on it the start-out alias dropped for a const_cast at each use 299, a case-5 block copy
// of the actor 299 (propagated), case 2 reading the state's pathfinding at each use 299, one vertices base
// for the corners 295; on the base order first/second through the existing vector3d_from_points3d 297.
// Round 325: Complete native surface snapshot at dispatch, crossed with both difference setters,
// ordinary result and actor-valid direction scope: 387/397/365/366/397/401/373/380.
// All restored; root retention is not recovered and most frames/extents grow.
// Round 275: complete native vertex values in paired/actor/trace cases 502/337/400;
// paired-actor/paired-trace/actor-trace/all 526/551/438/575; all with shared
// differences 583. Exact-only snapshots restored; early root/actor ownership and vector scheduling remain.
// Round 29 (m3 lane 2, fast): case 2's position declared before its direction point 284 (on a, b, c, d 299);
// round 36's vector3d_from_points3d + dot3f form re-measured 280, and on it the 24 corner declaration orders:
// a,b,c,d 280 (1284 B), a,b,d,c 295 (1284 B), b,a,c,d 274 (1280 B, a gain of 4: not kept), the other 21
// 349-353 (1280 B); only a,b,c,d / a,b,d,c load actor_index at entry, and both then take pathfinding in EDX.
// b,a,c,d with the result declared first 274.
// Round 128 (m3 lane 5, fast): retail passes the actor index in EAX to 0x270400 in case 5 straight from the
// entry load and compares it at the case start. `NONE != actor_index` 278; the negation as `*= -1.f` 278;
// `margin = state->source.radius * inverse` 278; the reciprocal before the projections 278 (all same code);
// `first`/`second` through vector3d_from_points3d 297 (masked-register differences 96 before it).
// Round 45 (lane 14, fast), on the b, a, c, d corners with vector3d_from_points3d and both sums through dot3f: the
// base 274 (aligned 199 -> 158); with case 5's direction declared first 274; case 2's position declared first 280;
// both 276; both dot3f calls taking `&first` first 274. Left on it: the entry, first.j/first.k and the 1.0/radius*2
// constants in swapped XMM registers, and the margin formed in the radius register, not in place in the reciprocal's.
// Round 40 (lane 14, fast): a, b, c, d re-measured 299 (1284 bytes, actor load first, pathfinding in EDX,
// 0x270d90 313); on it the start-out alias dropped for a const_cast at each use 299, a case-5 block copy
// of the actor 299 (propagated), case 2 reading the state's pathfinding at each use 299, one vertices base
// for the corners 295; on the base order first/second through the existing vector3d_from_points3d 297.
// Round 58 (m3 lane 1, fast; aligned = difflib count, base 199): the actor read through its address into a
// function-scope local, as a volatile read: after the surface 323 (1288 bytes, aligned 135: EBX entry as retail,
// but pathfinding arrives in ECX and the surface index in EAX), after the result 326, ahead of the surface 358
// (1284 bytes, aligned 190), with the start-out alias dropped 320 (1284, aligned 117); as a plain read 280. The
// surface table in a local read before the actor, then the index: 358 (1284; table/index registers are retail's,
// pathfinding in EDX), plain read 280, table local alone 280. Register parameters entry/pathfinding/surface or
// surface/pathfinding/entry 278/278, with the volatile read 323/323 (the arrival registers do not follow the
// declared order), entry first with the plain read 280; a, b, c, d with the other five orders 299 each (EDX stays).
// Round 74 (m3 lane 12, fast): the radius test inverted with the 0.5 arm first 277; a named upper bound
// `1.f - margin` 278; the clamp as one nested ternary 278; `fraction < margin` for the lower test 278; the margin
// compared with 0.5 before the clamp (not equivalent, diagnostic) 367 (1264 bytes). Retail's case 5 passes the
// entry-loaded actor straight to 0x270400 in EAX (its register parameter); retail's caller 0x270d90 sets ECX, EAX,
// EDI interleaved with the pushes, so it gives no parameter-order evidence.
// DH276: complementary failure outcome at dispatch in inherited Boolean/plain
// Boolean/short/long storage 402/359/356/354; initialized first 402/358/355/353.
// All restored; frame 0x40 and length agree, but early root/actor ownership and
// paired vector scheduling remain. Original result and helper retained.
// DH281: Existing shared point-on-line scalar-product declaration orders, six forms 278; complete
// coordinate results with/without scalar products 268, rejected because the matched helper user regresses
// [278, 278, 278, 278, 278, 278, 268, 268]. Original executable body restored.
// N301: Complete positive switch-outcome result in inherited Boolean/plain Boolean/short/long
// storage at dispatch 366/384/380/370; declared first 366/384/380/370.
// All restored; early root/actor retention and paired-vector scheduling remain.
// Round 51 (m3 lane 13, fast): actor_index read once through its own address into a local at entry 280
// (propagated, no entry load); case 5's two output sets as if/else arms with one `result = true` 278 (same code);
// an explicit `default: break;` 278 (same code); case 2's trace result declared at the case's top 278 (same code).
// Diagnostic only (volatile, reverted at once): a volatile read of actor_index at case 5's test 355 (1284 bytes,
// 0x270d90 334). Retail's case-5 `cmp eax, -1` opens the case (actor loaded at entry, as a load hoisted from the
// two sibling uses in cases 5 and 2).
// DH292: Both vector differences through plain component-array loops, long/dword/short/word
// ordinals 305/305/423/423; with the same direction-negation loop 305/305/401/401.
// Original scalar arithmetic, result qualifier, helpers and complete body restored.
// DH294: Existing shared add-scaled producer at first/second/both paired point outputs
// 278/278/278; each with both native vector differences 297/297/297; complete point outputs
// with native byte success, original/shared differences 358/366. Original helper calls,
// result qualifier, includes and complete body restored; early root/actor ownership remains.
// M323: Actual first-edge/second-edge/projection-field references, seven subsets and complete trace-element references, all 278. Original body restored; early root/actor ingress and vector scheduling remain.
// Lane 17 r36 (fast): corners a, b, c, d re-measured 299 (1284 bytes; the actor load is still scheduled before
// `push ebx`, so pathfinding arrives in EDX, and case 1's projection/origin loads are re-coloured); on that graph case 5
// first in the switch 353, the surface as `surfaces + surface_index` 299 (same code), a const surface pointer 299 (same
// code), the inverse length computed before the projection 299 (same code). Original c, b, a, d body restored.
// Round 33 (m3 lane 18, fast; 0x270d90 295 throughout): the result as a plain bool (the volatile dropped)
// 340 (1252 bytes); case 5's actor test as an outer if 278; one function-scope position pointer for cases
// 5 and 2 278; the second edge declared in the first-length block 278; the margin as `radius * inverse`
// 278 (each the same code). Retail's case 5 tests the actor in EAX loaded at entry while case 2 reloads it
// from the stack. All restored.
// DH317: One pathfinding receiver across dispatch/case 2, with existing first/second
// differences and complete shared projections in eight subsets:
// 278/278/297/297/278/359/297/351. Original body and all helpers restored;
// early root/actor ownership remains, at the original frame and length.
// Round 82 (m3 lane 12, fast; 0x270d90 295 unless noted): case 5 first in the switch body 300; actor test
// constant first 278 (same code); const surface pointer 278; byte volatile result 406 (1228 B); case 5 as
// if/else with the output stores shared after it 404 (1220 B, 0x270d90 368). Original body restored.
// Round 67 (m3 lane 1, fast; 0x270d90 295 unless noted): case 5's negation as unary minus 367 (1264 bytes); case 2's
// trace through the `data` local 319; case 5's call result in a `found` flag set under the actor test 278 (same
// code); the negation as `-1.f * direction.i` 278; the corner tests as `vertices[i] != 0xffff` 357 (1292 bytes,
// 0x270d90 368).
// m3 lane 14 r56 (fast; 0x270d90 295 throughout): case 2's trace through the `data` local 319; the parent-point
// test as an early `break` 278 (same code); the radius test as `radius * 2.f < first_length` 278 (same code);
// the origin sum before the projection 278 (same code); corners declared d, c, b, a 280. All restored.
// DH339: Direct result returns at native paired/actor/trace case ends, seven subsets then
// complete success exits, eight at 270 fast; full form 270 plain, 1280 bytes.
// Kept complete per-case returns; early root/actor ingress and paired vector scheduling remain.
// Round 145 (m3 lane 5, fast; 0x270d90 286): a shared forceinline wrapper over 0x270400 for cases 5 and 2 278;
// an inline vertex accessor for all eight vertex lookups 278; case 5's test as a named `found` bool 278 (all same
// code); the register parameters declared pathfinding, entry, surface_index everywhere 278 (same code); the
// surface read through the table at each use 429 (1336 B). A cdecl compile-only first pass loads actor_index
// only at its uses even with uses added to every case, so no body form here gives retail's entry load.
// DH342: On the retained direct-case-return graph, five alternate whole-case orders
// 342/293/381/345/381; explicit default result returns first/middle/last 270/270/270.
// Original case order and complete body restored; early root/actor ingress remains.
// DH345: Reference producer interfaces, seven subsets/full forced expansion, eight at 270;
// excluded from the trial count because caller adaptation left the queue; all restored.
// Immutable origin/direction/output pointer bindings, seven subsets and full immutable
// distance, eight at 270 with all callers unchanged. Original helper/body restored;
// native root/actor ingress and paired-vector scheduling remain.
// DH347: Last-consumer first/second receiver and projection scopes, seven subsets then
// all with a descriptive trace name: 271/346/270/346/271/346/346/346. All restored;
// frame 0x40 and extent agree, but early root/actor ingress and vector scheduling remain.
// N370: Persistent root/actor, parent and actual-output lexical families, seven
// subsets then complete native local identities: 270 each. NOT kept: no equality
// or gain; early root/actor retention and paired-vector scheduling remain.
// F372: Paired-case normalized-length/projection/distance declaration lifetimes,
// seven complete subsets and all with margin: 270/270/270/270/270/270/270/270.
// Producers never moved; original declarations and complete body restored.
// F374: Paired normalization-continuation results, positive/complementary
// bool/byte/short/long: 270/270/270/270/270/270/270/270. NOT kept: no exact
// ingress/home recovery; original direct tests and complete body restored.
// DH354: Native direct-return workspace identities, edge/actor/trace subsets
// 1/2/4/3/5/6/7 and complete endpoint identities: eight at 270. All restored;
// early root/actor ingress and paired SIMD scheduling remain.
// DH356: Plain coordinate-copy induction at paired/actor/trace publications, seven
// subsets then all with ordinary result: 356/344/357/351/353/345/347/363. All restored;
// native root/actor ingress remains; original record fallback and all helpers retained.
// Y589: Complete sole-exit success bool/byte/short/long 278/358/356/347;
// complementary failure 402/359/356/354. NOT kept: ingress/frame/length
// not recovered; original native case returns and complete body restored.
// F383: Literal outcomes after paired/actor/trace publications, seven case subsets,
// then complete literal returns without the result: 365/361/373/390/370/406/371/384.
// NOT kept: root/actor ingress persists and extents regress; original result, all
// publications, native helpers and complete body restored.
// B433: Called normalizer/trace contracts, each ordinary/inline/forced, excluding
// the ordinary pair: eight at 270. Original declarations and full body restored;
// early root/actor retention and paired-vector scheduling remain.
// P333: Native direction/point-publication/parent-record/all transfers through memcpy/memmove:
// 270/354/270/433/270/414/270/396. NOT kept: no match or gain; original copies, helper and full body restored.
// Round 62 (m3 lane 14, fast): 219 (was 270): corners b, a, c, d, vector3d_from_points3d for both edges and dot3f for
// the projection and origin 266 (aligned 181 -> 143); case 5's direction first 264 (110); case 2 through a `vertices`
// local (no `data`) 219 (70; case 2 is retail's byte for byte, kept). Tried on the way: the reciprocal multiplied in
// place by the radius 270/266 (same code); `first_length * fraction` 266; dot3f taking &first first 266; a named
// parent position 266; origin before projected 266; a named difference 266; case 2's position first 266; case 2's
// state pathfinding read at each use 219 (same code); the trace through `data` 272; a case-5 `vertices` local 264.
// On 219: projection and origin written out, staged with `+=`, or one of each 366 (1288 bytes); `second, first`
// declared 219; `second` declared in its block 219; both reciprocal forms 219; both products turned 219; the radius
// product turned 219. Left: the entry (actor loaded at entry, pathfinding moved to EBX), which colours case 5, and
// the projection sum (retail (j + k) + i with j in XMM3, ours (k + j) + i).
// Y603: Complete root-vertex table values/field references at dispatch/case scope,
// original/shared negation: 230/219/219/219/230/219/219/219. NOT kept: no equality or gain;
// original body restored, early root/actor ingress and projection schedule remain.
// DH377: Read-only root/state and actual mutable-entry contracts, seven coordinated subsets
// 219/219/220/219/220/220/220; complete contracts with a const surface receiver 220.
// NOT kept: no equality or gain; both original declarations, entry alias and body restored.
// Native early root/actor ownership and paired projection/limit scheduling remain.
// DH380: Original five-test corner admission partitioned at four boundaries, positive nested then
// rejecting-prefix returns: 219/219/219/219/219/219/219/219. NOT kept: no native ingress recovery;
// original complete geometry, tests, result, helpers and body restored.
// N395: Actual actor/surface/parent index eight-byte homes, signed/unsigned individually
// 371/371/219/219/370/370; complete signed/unsigned 398/398. NOT kept: native values are narrow,
// ingress is not recovered, and wider actor/parent homes grow the frame/extent. Original body restored.
// N397: Current retained-graph corner orders acbd/acdb/adbc/adcb/bcad/bcda/bdac/bdca 357/355/357/353/356/354/356/353.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// Round 151 (harness 232 for the real 219, aligned 89, masked 47): a plain read of actor_index through its address after the surface, after the result or first,
// used at the test and the call or at the test only, 233 each (the read folds to the use, no entry load); dot sums through a `(j + k) + i` helper, both, either,
// arguments swapped, 232 each (same code); written out grouped, both or either, 371 (1288 bytes); one final `return result` with breaks 242 (aligned 107), without
// volatile 322 (1256 bytes, result in CL), declared later 322, first 321; multiple returns without volatile 335.
// Random grid of 160 natural forms: lead, corners a, b, c, d load actor_index into EAX at entry with no volatile and move pathfinding to EBX, masked 28 (base 47), aligned 64 (base 89), but 289 at 1284 bytes:
// the load comes first, so pathfinding arrives in EDX where retail has EAX (retail's load follows `mov ebx, eax`). Without case 5's position first 287 (masked 33), with `*= -1.f` or the old negation 290 (35);
// corners d, c, b, a 354, c, d, a, b 355, a, b, d, c 304. On the lead: a copy of actor_index after the surface or result 297, the surface after the result 289, origin after projected 289, the
// first three parameters declared in five other orders, with the call changed, 289 each (no effect on the arrival registers), on the old corners 278. Left on the lead: pathfinding's arrival, first.j/first.k, 1.0/radius*2 colours.
// Round 153 (m3 lane 5, fast; 0x270d90 286 throughout), re-measured on the 219 base: case 5's position declared before its
// direction 266; case 5's call under an outer actor test 219 (same code); result, surface, start_out declared in that order
// 219; the radius read once into a named local for the test and the margin 214 (1284 bytes, gain under the gate), with the
// margin product turned 214, with the test written `radius * 2.f < first_length` 214; second_distance computed first 219.
// Case 2 and everything after case 5's actor test agree; the entry (actor_index loaded at entry, pathfinding moved to EBX)
// shifts every line of cases 1/6 and 5.
// Y603: Complete root-vertex table values/field references at dispatch/case scope,
// original/shared negation: 230/219/219/219/230/219/219/219. NOT kept: no equality or gain;
// original body restored, early root/actor ingress and projection schedule remain.
// DH377: Read-only root/state and actual mutable-entry contracts, seven coordinated subsets
// 219/219/220/219/220/220/220; complete contracts with a const surface receiver 220.
// NOT kept: no equality or gain; both original declarations, entry alias and body restored.
// Native early root/actor ownership and paired projection/limit scheduling remain.
// DH380: Original five-test corner admission partitioned at four boundaries, positive nested then
// rejecting-prefix returns: 219/219/219/219/219/219/219/219. NOT kept: no native ingress recovery;
// original complete geometry, tests, result, helpers and body restored.
// N395: Actual actor/surface/parent index eight-byte homes, signed/unsigned individually
// 371/371/219/219/370/370; complete signed/unsigned 398/398. NOT kept: native values are narrow,
// ingress is not recovered, and wider actor/parent homes grow the frame/extent. Original body restored.
// N397: Current retained-graph corner orders acbd/acdb/adbc/adcb/bcad/bcda/bdac/bdca 357/355/357/353/356/354/356/353.
// NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// T65: On the retained shared-vector/exact-case-2 graph, complete case orders 2/5/16,
// 5/16/2, 5/2/16, 2/16/5, 61/5/2, 5/61/2, 5/2/61, 2/5/61:
// 377/329/377/354/219/329/377/377. NOT kept: ties/regressions; original full body restored.
// Early root/actor ingress and paired projection ownership remain.
// P361: Complete actor-query success/rejection admission results in bool/byte/short/long at the original producer:
// 219/219/219/219/219/219/219/219. NOT kept: no byte equality or eligible native-shape gain; original complete body restored.
// Early root EBX/actor EAX ingress and paired projection scheduling remain.
// T68: Individual local names on the native paired-projection graph: both edges,
// parent/origin projection, fraction, inverse length, facing and marker position:
// 219/219/219/219/219/219/219/219. NOT kept: no exact ownership/home repair. Original executable body restored.
// Early root/actor retention and paired projection ownership remain.
// P394: Paired-edge clamp admission before reciprocal/fraction, wide-edge/midpoint truth
// in bool/byte/short/long: 367/367/371/369/367/367/371/369. NOT kept: no byte equality; original
// comparison, geometry, result and whole body restored. Early root/actor ingress remains.
// Round 82 (m3 lane 1, fast; 0x270d90 286, 0x270400 matched throughout; aligned = sequence-aligned count with targets
// masked, base 76): round 151's lead rebuilt on this body, corners declared a, b, c, d: 288 at 1284 bytes (aligned 67);
// with case 5's position declared before its direction 290 (aligned 50: all of case 5, case 2 and the copies then
// agree). Left on that lead, two causes: the entry (actor_index is loaded into EAX first, so pathfinding arrives in EDX
// for retail's EAX; retail copies pathfinding to EBX and only then loads the actor) and both dot sums (retail loads
// first.j, first.k and adds (j + k) + i; ours loads k, j and adds (k + j) + i, which also swaps XMM6/XMM7 and leaves
// the margin a separate `movss` + `mulss` for retail's `mulss xmm2, [radius]`, the 4 extra bytes). On the lead: the
// margin multiplied into the reciprocal in place 288 (same code); the sums written out j, k, i 299 (1288 B, aligned
// 74; the terms still come out k, j); dot3f with &first as its first argument 288 (same code); `second, first`
// declared 288 (same code); the sums written with first's fields on the left 300 (aligned 57); `i + (j + k)` grouped
// 305 (aligned 62); each product cast to real 300 (57); origin computed before projected 290 (same code); a named
// copy of actor_index declared before or after the result, used in cases 5 and 2 or in 5 only, 297 (aligned 51; the
// load still comes first); the surface declared before start_out 290 (same code). Nothing kept: every form scores
// above 219.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 219 (1280 bytes against retail's 1280); the early root and actor ingress and the paired projection's ownership are still the whole of the difference, and every earlier round's actor-query, corner-order and paired-edge forms tie at 219.
// Round 145 (m3 lane 8, fast; masked = register-masked aligned count, base 219 [22], aligned 99): an unused forceinline
// use of actor_index in case 1/6 (diagnostic) 219, same code: no entry load. Corners a, b, c, d re-measured 288 [32]
// (1284 bytes); on them: the surface through a named `surfaces` table local 295 [27]; the actor test nested 288 (same
// code); one function-scope vertices pointer read after the result 306 [49] at 1280 bytes; one case-scope vertices pointer
// for case 1/6 read ahead of the corner tests 174 [34] at 1280 bytes, and the same on the b, a, c, d corners 169 [37] at
// 1280 bytes (aligned 149). Neither is kept: the actor is then loaded as the first instruction, pathfinding arrives in
// EDX and is read from there with no copy to EBX, and the vertex table is loaded ahead of the parent test where retail
// loads it after the corner tests; the lower positional count comes from the entry being one line shorter, which only
// re-aligns the later lines (masked 22 -> 37); plain 169 too, with 0x270d90 still 286 but 1390 bytes for 1389. Left as before: every form that loads the actor at entry loads it ahead of
// `push ebx`, so link-time code generation hands pathfinding over in EDX; retail's load follows `mov ebx, eax`.
// Round 166 (m3 lane 6, fast; 0x270d90 286 throughout; masked = register-masked aligned count, base 22): the actor parameter
// renamed to sort after pathfinding 219 (same code); the pathfinding parameter renamed 219 (same code); case 5's position
// first with the radius in a named local for the test and the margin 261 (1284 bytes, masked 46); case 5's and case 2's
// positions both first 272 (masked 28); the radius read at the top of case 1/6, ahead of the normalizes, 322 (1292 bytes).
// The only caller, 0x270d90, is in a file other lanes hold this round, so its effect on the entry was not measured.
// Round 148 (m3 lane 8, fast with the file's 15 other functions, 0x270d90 and 0x30bf0; aligned/masked = difflib counts on
// the listing, masked ignores registers and stack offsets, base 54/13). Re-measured: case 5's position declared before its
// direction 266 (as round 153); with corners a, b, c, d 290 (1284 B, aligned 37, masked 11, as rounds 151/82); on that the
// function-scope actor bool 364 (1292 B, as lane 17 round 6) and the surface ahead of the start-out alias 290 (same code).
// New: on that a, b, c, d graph the result as a plain bool 281 (1280 B, aligned 80). On the 219 body, each the same code
// (219, aligned 54, masked 13): both projections in one expression with no projected/origin locals; the edge vectors
// declared at function scope; the trace result declared at function scope; case 5's direction declared at function scope;
// case 1/6 leaving through `break` to the final `return result`.
// Round 67 (m3 lane 15, fast with 0x270d90/0x270400; aligned/masked = difflib counts with targets / registers masked,
// base 219 aligned 54 masked 14): retail reloads actor_index from its stack slot in case 2 and uses the entry EAX only
// in case 5 (compare and 0x270400's register argument); EBX is overwritten inside case 1/6 before any call, so the
// pathfinding copy only frees EAX. The start_out alias dropped (const_cast at each use) 220 (masked 17); the radius in
// a named local 214 at 1284 bytes (aligned 103, masked 33: positional only); the corners declared a, b, c, d but
// assigned b, a, c, d 219 (identical: declaration order moves nothing, code order does); the surface fields read
// through pathfinding->surfaces[surface_index] at each use 461 (1364 bytes); `1.f - margin` named for the test and
// the assignment 218 (masked 15).
// M3 lane 10 r92 (fast with 0x270d90 at 286 and 0x270400 MATCH; base 219, aligned 70, masked 22): nothing kept. Case 5's arms joined
// (the failure arm sets only start_out and the parent node, the out stores shared) 400 (1224 bytes; 0x270d90 286 -> 372); its
// negation through the existing vector3f_scale(&direction, -1.f, &direction) 218 (one point), with `state->source.radius *
// inverse` 218; the direction scaled straight from the vertex 379 (1256 bytes); position declared first with vector3f_scale 265;
// `inverse * (projected - origin)` 219 and `state->source.radius * inverse` 219 (same code). Kept: the negation through
// vector3f_scale (main wrote its body out by hand), 218 plain, the file's other functions and 0x270d90 unchanged.
// m3 lane 13 r79 (fast, 0x270d90 286 throughout; brackets: difflib count aligned / with registers masked, base 219
// [52/13]): a `vertices` local for case 1/6's corners inside the corner block 219 (same code); the radius read into
// a named local for the test and the margin with `1.f - margin` named 214 (1284 bytes) [93/26]; the radius local
// declared ahead of the projections 224; a 0.5 default with the fraction computed inside the radius branch 358
// (1284); the radius local with the corner-block `vertices` local 211 (1280 bytes) [96/29], not kept: the count
// falls only by re-alignment, the first/second length homes swap and retail reads the radius from memory twice
// (no radius local). Re-measured: the vertex table read at case 1/6's top 169 [90/22] (round 145's form: the actor
// load goes first and pathfinding arrives in EDX; 0x270d90 1390 bytes); the reciprocal before the projections 219.
// Both builds share the dead `out` slot (E+0x18) between the result byte and first_length.
// M3 lane 10 r94 (fast with 0x270d90 at 286 and 0x270400 MATCH; base 218, aligned 70, masked 23): nothing kept. Case 5's
// position copied by value 345 (1316 bytes); case 1/6's two distances inline in the point_along_vector calls 300 (1284); both edge
// vectors formed before the first length 347; case 1/6's missing parent as an early return 218 (same code); re-measured: the
// surface as a const pointer 218, the entry parameter declared mutable (no start_out alias, the declaration in path.cpp
// too) 220, a case-5 vertices pointer 218. The entry is still the cause: retail loads actor_index into EAX first and moves
// pathfinding to EBX; no spelling of the cases changes it. Also: both lengths computed before either test 349 (1284 bytes, a
// variant of the edges-first form); case 5's fallback copying the parent point and output index by member 353 (1292 bytes).
// MiniMax R (11-11): no new form, no lower score (218): the entry still differs - retail moves the pathfinding
// pointer into ebx and loads the actor index into eax before the switch, ours keeps the pathfinding pointer in eax
// and reads the actor index in the cases that use it; the frame (0x40) and case 2 agree, so the whole difference is
// that entry.
// M3 lane 10 r97 (fast with 0x270d90 at 286 and 0x270400 MATCH; base 218): nothing kept. The actor parameter typed dword 218
// (same code); the actor declared third, ahead of the entry pointer (definition, declaration and the 0x270d90 call; stack order
// unchanged) 218 (same code); that order on corners a, b, c, d 287 (1284 bytes: the actor load still comes first and pathfinding
// arrives in EDX, as with every other order on that graph); corners d, a, b, c 356; c, a, b, d 358.
// T96: Existing result owns paired-corner/both-length/actor-query/trace-query tests,
// then corner-length/corner-actor/length-actor/all outcomes:
// 388/406/357/263/441/408/423/467. NOT kept: no equality or native early root/actor
// recovery; all extents grow. Original result, helpers, calls and full body restored.
// T100: Paired positive/fallback endpoint publication sets 1/2/3/4/5/6/7, then full
// direct-success completion: 417/499/405/353/351/368/406/415. NOT kept: no native
// early root/actor recovery or equality; original helpers, result and full body restored.
// P443: Complete corner-index receiver, double-factor negation, endpoint receivers, radius-owner receiver,
// guarded parent-output reference, four corner role names, paired length declarations and sole result exit:
// 219/362/220/218/219/218/218/228. NOT kept: no native ingress/home repair; original complete body and helpers restored.
// P445: Actual diameter and clearance results, paired widened-distance transport, consumed
// negation return, trace rejection, parent-presence and complete corner admissions, separate projected/clamped fraction:
// 218/218/368/218/218/219/219/228. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// P447: Immutable paired lengths/projections/reciprocal/distances/corner pointers/actor-trace receivers; original-site lower/upper margin admissions:
// 218/218/218/218/218/218/218/218. NOT kept: no eligible native-shape gain or byte equality; original complete body restored.
// M3 lane 10 r101 (fast with 0x270d90 at 286 and 0x270400 MATCH; base 218): nothing kept. The function defined ahead of its
// callee 0x270400 in the file (a prototype added) 218 (same code: definition order moves nothing). Corners a, b, c, d
// re-measured 287 (1284 bytes; entry `mov eax, [actor]; push ebx; push ebp; mov ebp, [out]; mov ebx, edx; mov edx, [ebx+0x3c]`,
// and from the switch on case 1/6 reads as retail). On that graph: surface_index moved to the end of the parameter list
// (definition, declaration and the one call together) 287 (same code); case 5 reading its vertices through the state's
// pathfinding 325 (1296 bytes: the parameter then stays on the stack, 0x270d90 334); a `short type` local read ahead of the
// result for the switch 295; case 5's actor test as an outer if around the call 287 (same code). Reading of the entry: retail
// has the same two homes as that graph (actor EAX from the entry, pathfinding EBX) but its pathfinding arrives in EAX and
// is copied to EBX ahead of the actor load, where ours is handed over in EDX because the actor load is placed first; no
// parameter order, definition order or case spelling changes which register link-time code generation picks for it.
// P450: Separate first/second positive-length continuations, positive/complementary bool/long:
// eight 218s at 1280 bytes. NOT kept: native early root/actor ingress stays different;
// one-consumer decisions do not match. Original geometry, result qualifier and full body restored.
// P452: Separate paired-start/end, actor-success/fallback and trace-initial/recovered point memmoves, then complete paired/actor endpoint contracts:
// 357/354/341/319/335/245/359/400. NOT kept: all regress; native early root/actor ingress remains. Original record publications, result qualifier, helpers, caller roles and full body restored.
// P454: Existing point producer returns pointer/const-pointer/reference/const-reference/value,
// then forced pointer/reference/value: 218/218/218/218/219/218/218/219.
// NOT kept: no native early root/actor ingress repair; value returns also raise the other helper user to 66.
// Original shared producer and complete body restored.
// m3 lane 14 r92 (fast with 0x270d90 at 286 and 0x270400 MATCH, control 218): case 5's fallback as the one block under
// `actor == NONE || !call` with the out stores shared 399 (1224 bytes; 0x270d90 372); a named `has_actor` test at the top 352
// (1288 bytes, the entry loads it first); case 5 as if/else with one `result = true` 218 (same code); case 2 without
// the vertices local 218; the radius test written `radius * 2.f < first_length` 218. Retail's case 5 compares the
// actor in EAX early (the flags held across the scale) and calls with it in place, so the entry load is the whole cause.
// P477: Standard paired dot/difference, actor-direction capture, paired/actor/trace point publications, upper-limit and native-prefix reductions:
// 393/409/281/358/346/347/405/442. NOT kept: all regress and none repairs native early root/actor ingress; sixteen matched controls survive. Original geometry, shared producers, caller roles, result qualifier and complete body restored.
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
            point3f const *b = &pathfinding->vertices[surface->vertices[1]];
            point3f const *a = &pathfinding->vertices[surface->vertices[0]];
            point3f const *c = &pathfinding->vertices[surface->vertices[2]];
            point3f const *d = &pathfinding->vertices[surface->vertices[3]];
            vector3f first, second;
            vector3d_from_points3d(a, b, &first);
            real first_length = function_30bf0(&first);
            if (first_length > 0.f)
            {
                vector3d_from_points3d(c, d, &second);
                real second_length = function_30bf0(&second);
                if (second_length > 0.f)
                {
                    real projected = dot3f((vector3f const *)&parent_point->point, &first);
                    real origin = dot3f((vector3f const *)a, &first);
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
                    point_along_vector(a, &first, first_distance, &start_out->point);
                    point_along_vector(c, &second, second_distance, &out->point);
                    start_out->output_index = parent_point->output_index;
                    out->output_index = surface->destination_output;
                    *parent_node_index_out = parent_node_index;
                    *out_node_index = surface->destination_node;
                    result = true;
                    return result;
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
        return result;
    case 5:
        {
            vector3f direction = *(vector3f const *)&pathfinding->vertices[surface->vertices[3]];
            point3f const *position = &pathfinding->vertices[surface->vertices[2]];
            vector3f_scale(&direction, -1.f, &direction);
            if (actor_index != NONE && function_270400(actor_index, 0x05000534, 0x05000049,
                position, &direction, NULL, (long *)start_out, NULL, parent_node_index_out))
            {
                out->point = *position;
                out->output_index = parent_point->output_index;
                *out_node_index = surface->vertices[0];
                result = true;
                return result;
            }
            *start_out = *parent_point;
            *parent_node_index_out = parent_node_index;
            out->point = *position;
            out->output_index = parent_point->output_index;
            *out_node_index = surface->vertices[0];
            result = true;
        }
        return result;
    case 2:
        {
            point3f const *vertices = ((s_pathfinding_data *)state->pathfinding)->vertices;
            point3f const *direction_point = &vertices[surface->vertices[2]];
            point3f const *position = &vertices[surface->vertices[1]];
            out->point = *position;
            out->output_index = parent_point->output_index;
            *out_node_index = parent_node_index;
            if (!function_270400(actor_index, 0x050000cb, 0x060000cc, position,
                (vector3f const *)direction_point, NULL, (long *)start_out, NULL, parent_node_index_out))
            {
                s_path_trace_result trace;
                function_26c590((s_pathfinding_data *)state->pathfinding, position, parent_node_index, NONE,
                    (vector3f const *)direction_point, 0.2f, NULL, &trace);
                start_out->point = trace.point;
                start_out->output_index = parent_point->output_index;
                *parent_node_index_out = ((s_sector_trace_result *)&trace)->sector_index;
            }
            result = true;
        }
        return result;
    }
    return result;
}
