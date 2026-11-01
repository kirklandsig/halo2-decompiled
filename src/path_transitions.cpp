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
bool function_26f2d0(s_path_settings const *settings, point3f const *start, point3f const *end)
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
bool function_26f360(s_path_settings const *settings, point3f const *start, point3f const *end)
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

// score 20 (was 20): Flags promotion and marker-query/loop-entry scheduling remain; protected helper interface unchanged.
// Alignment pointer snapshot 55; actor point snapshot 131; direct marker indexing 20;
// selected marker memcpy 20; unsigned-word ordinal 111; long best index 76;
// entry best sentinel 46; both vector observations 114.
// retail passes the marker flags to 0x26fbf0 as a 16-bit argument
// (`xor edx, edx; mov dx, [eax + 0x1c]`); declaring that parameter `word` gives 4 and keeps
// 0x26fbf0 matched, but 0x26fbf0 is contributor code: BLOCKED-BY 0x26fbf0. The squared distance
// through vector3d_from_points3d/length_sq3f 30; difference orders xzy/yzx/zxy/zyx 24/26/26/24;
// best before best_distance 20; the marker query before both 99; dot3f alignment 26; sum
// x + y + z 20; nested point test 20; flags through a word local 22; alignment k + i + j 20;
// the markers array after best 20; long count 111; const character 20.
// A direct word cast 20; the existing orientation predicate forced inline 22; count declared before
// best_distance and assigned in place 20; const count 20; the loop ordinal declared outside 20;
// const actor 20; success result assigned before the point query 106; const marker 20.
// Round 16: dot3f alignment 26; a forward-vector pointer 26; a short cast at the call 20; the
// flags field typed short 20; a long loop ordinal 827. With 0x26fbf0's flags as word (diagnostic,
// restored) this is 4, and with dot3f for the alignment too 2 (only the j product's operands).
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
            real alignment = marker->matrix.forward.i * g_4687b0->i +
                marker->matrix.forward.k * g_4687b0->k + g_4687b0->j * marker->matrix.forward.j;
            if (alignment > 0.95f)
            {
                real x = actor->position.x - marker->matrix.position.x;
                real y = actor->position.y - marker->matrix.position.y;
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
            result = true;
        }
    }
    return result;
}

// score 165 (was 209): Frame and incoming actor/point homes agree; marker flags, cursor and SSE homes remain.
// Round 428: repeated span snapshot 204; single result 189; both 185; existing point differences
// in both geometry branches 191; with span 187; with result 167; all three 165 (656/660, kept).
// Shared actor-position pointer with span 240. Original sum trees and call tuples preserved.
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
            s_object_marker *marker = &markers[i];
            real alignment = marker->matrix.forward.i * g_4687b0->i +
                marker->matrix.forward.k * g_4687b0->k + marker->matrix.forward.j * g_4687b0->j;
            if (alignment > 0.95f)
            {
                real span = marker->unknown6c;
                vector3f segment;
                segment.i = marker->matrix.up.i * span;
                segment.j = marker->matrix.up.j * span;
                segment.k = marker->matrix.up.k * span;
                real squared_length = segment.k * segment.k + segment.j * segment.j + segment.i * segment.i;
                real squared;
                if (squared_length > 0.0001f)
                {
                    vector3f delta;
                    vector3d_from_points3d(&marker->matrix.position, &actor->position, &delta);
                    real t = (delta.k * segment.k + delta.j * segment.j + delta.i * segment.i) / squared_length;
                    if (t < 0.f) t = 0.f;
                    else if (t > 1.f) t = 1.f;
                    real negative = 0.f - t;
                    real dy = segment.j * negative + delta.j;
                    real dz = segment.k * negative + delta.k;
                    real dx = negative * segment.i + delta.i;
                    squared = dz * dz + dy * dy + dx * dx;
                }
                else
                {
                    vector3f delta;
                    vector3d_from_points3d(&actor->position, &marker->matrix.position, &delta);
                    squared = delta.k * delta.k + delta.j * delta.j + delta.i * delta.i;
                }
                if (best_distance > squared) { best_distance = squared; best = i; }
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

// score 278 (was 278): Frame 0x40 agrees; early root EBX/actor ownership and vector scheduling differ.
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
            point3f const *c = &pathfinding->vertices[surface->vertices[2]];
            point3f const *b = &pathfinding->vertices[surface->vertices[1]];
            point3f const *a = &pathfinding->vertices[surface->vertices[0]];
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
                    point_along_vector(a, &first, first_distance, &start_out->point);
                    point_along_vector(c, &second, second_distance, &out->point);
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
            vector3f direction = *(vector3f const *)&pathfinding->vertices[surface->vertices[3]];
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
                function_26c590((s_pathfinding_data *)state->pathfinding, position, parent_node_index, NONE,
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
