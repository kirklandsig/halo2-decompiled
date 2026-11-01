/* UNKNOWN_2BA3E0.CPP: filling a placement (a position, a unit direction and
   a second vector) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_1eb550.h"
#include "normalize3d.h"
#include "vector_products.h"

// @flags /O2 /arch:SSE /Gr

/* a placement (0x28 bytes) */
struct s_placement
{
	point3f position;
	vector3f direction;
	vector3f up;
	short value24;
};

/* the direction used when the given one cannot be normalized */
extern vector3f *g_4687bc;

real function_1201a0(vector3f *v, vector3f const *fallback);

// @retail 0x2ba3e0
void placement_set(s_placement *placement, vector3f const *direction, point3f const *position, vector3f const *up, short value)
{
	vector3f normal = *direction;

	function_1201a0(&normal, g_4687bc);
	placement->value24 = value;
	placement->position = *position;
	placement->direction = normal;
	placement->up = *up;
}

#include "data_array.h"
extern s_record_pool *g_51ec84;

struct s_particle_2b96
{
    long id;
    long next;
    byte unknown08[0x1c - 8];
    point3f position;
    vector3f velocity;
    byte unknown34[12];
};

PRIVATE __forceinline real inverse_sqrt_2b96(real squared)
{
    real inverse;
    __asm
    {
        rsqrtss xmm0, squared
        movss inverse, xmm0
    }
    return inverse;
}

// @retail 0x2b9670
s_particle_2b96 *function_2b9670(s_particle_2b96 *particle, long index)
{
    s_particle_2b96 *result = 0;
    vector3f velocity = particle->velocity;
    real squared = velocity.i * velocity.i + velocity.j * velocity.j + velocity.k * velocity.k;
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        velocity.i *= inverse;
        velocity.j *= inverse;
        velocity.k *= inverse;
    }
    real closest = 0.0f;
    while (index != NONE)
    {
        s_particle_2b96 *other = &((s_particle_2b96 *)g_51ec84->data)[index & 0xffff];
        index = other->next;
        if (other != particle)
        {
            vector3f delta;
            delta.i = particle->position.x - other->position.x;
            delta.j = particle->position.y - other->position.y;
            delta.k = particle->position.z - other->position.z;
            real distance = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
            if (distance < closest || !result)
            {
                result = other;
                closest = distance;
            }
        }
    }
    return result;
}

PRIVATE __forceinline real particle_vector_squared_2b(vector3f const *v)
{
    real squared = v->k * v->k;
    squared += v->i * v->i;
    squared += v->j * v->j;
    return squared;
}

PRIVATE __forceinline void scale_particle_vector_2b(real scale, vector3f *v)
{
    v->i = scale * v->i;
    v->j = v->j * scale;
    v->k = v->k * scale;
}

PRIVATE __forceinline void normalize_particle_vector_2b(vector3f *v)
{
    real squared = particle_vector_squared_2b(v);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, v);
    }
}

PRIVATE __forceinline real particle_direction_squared_2b(vector3f const *v)
{
    real squared = v->k * v->k;
    squared += v->j * v->j;
    squared += v->i * v->i;
    return squared;
}

PRIVATE __forceinline void normalize_particle_direction_2b(vector3f *v)
{
    real squared = particle_direction_squared_2b(v);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, v);
    }
}

// @retail 0x2b98b0
void function_2b98b0(s_particle_2b96 const *particle, s_particle_2b96 const *other, real scale, vector3f *result)
{
    vector3f a = particle->velocity;
    vector3f b = other->velocity;
    normalize_particle_vector_2b(&a);
    normalize_particle_direction_2b(&b);
    vector3f delta;
    scaled_difference3f(a.n, b.n, scale, delta.n);
    for (long component = 0; component < 3; ++component)
        result->n[component] += delta.n[component];
}

PRIVATE __forceinline real particle_distance_estimate_2b(real squared)
{
    real estimate;
    *(long *)&estimate = (*(long *)&squared >> 1) + 0x1fc00000;
    return estimate;
}

// @retail 0x2b9750
void function_2b9750(s_particle_2b96 const *other, s_particle_2b96 const *particle, real radius, real scale, vector3f *result)
{
    if (fabs(scale) < 0.0001f)
        return;
    vector3f delta;
    vector3d_from_points3d(&particle->position, &other->position, &delta);
    real initial_squared = dot3f(&delta, &delta);
    real bounded_radius = radius > 0.1f ? radius : 0.1f;
    real ratio = particle_distance_estimate_2b(initial_squared) / bounded_radius;
    vector3d_from_points3d(&other->position, &particle->position, &delta);
    normalize_particle_direction_2b(&delta);
    vector3f force;
    if (ratio < 1.0f)
        vector3f_scale(&delta, 0.0f - ratio * scale, &force);
    else
        vector3f_scale(&delta, scale, &force);
    result->i += force.i;
    result->j += force.j;
    result->k += force.k;
}

#include "globals.h"
#include "unknown_246cd0.h"
void __stdcall function_173ba0(dword mask, void *a, void *b, void const *c, real *values);
void function_211b70(short index, real const *position, byte flags, vector3f *out);
real function_17c900(short function_type, real input);

struct s_particle_property_entry_2ba
{
    long unused;
    s_particle_property property;
};
struct s_particle_properties_2ba
{
    byte unknown00[8];
    s_particle_property_entry_2ba *properties;
    dword constant_mask;
    dword input_mask;
};

struct s_particle_cache_2ba
{
    real values[17];
    dword valid;
    void *current_system;
    void *current_emitter;
    s_particle_2b96 *current_particle;
};

struct s_particle_entry_group_2ba
{
    byte unknown00[8];
    long mode;
    long count;
    s_particle_properties_2ba *entries;
};

void function_2b8da0(s_particle_properties_2ba const *definition, void *system,
    long first, void const *origin, real scale, long mode);

// @retail 0x2ba3a0
void function_2ba3a0(s_particle_entry_group_2ba const *group, long first,
    void *system, void const *origin, real scale)
{
    for (long i = 0; i < group->count; ++i)
    {
        function_2b8da0(&group->entries[i], system, first, origin, scale, group->mode);
    }
}

// @retail 0x2ba100
void function_2ba100(s_particle_properties_2ba const *definition, void *system, long first, real scale)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    real properties_values[3] = { 0.0f, 0.0f, 0.0f };
    dword remaining = definition->constant_mask;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    for (dword i = 0; i < 3 && remaining; ++i)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            properties_values[i] = function_246cd0(&properties[i].property, cache.values);
            remaining &= ~bit;
        }
    }
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        if (!(*((byte *)particle + 2) & 9) && *(real *)((byte *)particle + 8) <= 1.0f)
        {
            if (particle != cache.current_particle)
            {
                cache.current_particle = particle;
                cache.valid &= 0xffff07f0;
            }
            requested = definition->input_mask & 0xf80f;
            function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
            cache.valid |= requested;
            for (dword j = 0; j < 3; ++j)
            {
                if (!(definition->constant_mask & (1 << j)))
                    properties_values[j] = function_246cd0(&properties[j].property, cache.values);
            }
            long wind_index = NONE;
            short cluster = *(short *)((byte *)system + 0x20);
            if (cluster != NONE)
                wind_index = *(short *)(*(byte **)((byte *)g_4e0348 + 0xa0) + cluster * 0xb0 + 0x76);
            vector3f wind;
            function_211b70(wind_index, particle->position.n, 8, &wind);
            real time = g_510c54->game_time * g_510c54->rate;
            real phase = function_17c900(10, time + properties_values[1] * scale);
            real amount = (phase - 0.5f) * scale;
            *(real *)((byte *)particle + 0x34) += amount * properties_values[2];
            amount *= properties_values[0];
            vector3f delta;
            delta.i = amount * wind.i;
            delta.j = wind.j * amount;
            delta.k = wind.k * amount;
            particle->velocity.i += delta.i;
            particle->velocity.j += delta.j;
            particle->velocity.k += delta.k;
        }
    }
}

#include "unknown_0259a0.h"

// @retail 0x2b9a00
void function_2b9a00(s_particle_2b96 const *particle, real distance, vector3f *result)
{
    vector3f direction = particle->velocity;
    union
    {
        s_collision_result_1697c0 value;
        byte storage[0x5c];
    } collision;
    collision.value.unknown24 = NONE;
    real squared = particle_direction_squared_2b(&direction);
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        scale_particle_vector_2b(inverse, &direction);
    }
    scale_particle_vector_2b(distance, &direction);
    if (function_1697c0(0x800005, &particle->position, &direction, NONE, NONE, &collision.value))
    {
        result->i += ((vector3f const *)(collision.storage + 0x28))->i;
        result->j += ((vector3f const *)(collision.storage + 0x28))->j;
        result->k += ((vector3f const *)(collision.storage + 0x28))->k;
    }
}

PRIVATE __forceinline vector3f *normalize_particle_velocity_2b(vector3f *v)
{
    real squared = v->i * v->i + v->j * v->j + v->k * v->k;
    if (squared != 0.0f)
    {
        real inverse = inverse_sqrt_2b96(squared);
        v->i *= inverse;
        v->j *= inverse;
        v->k *= inverse;
    }
    return v;
}

// score 10 (was 10): frame 0xb4 and length 1516 agree. The back-edge parameter reloads (ours first,
// next, definition; retail definition, next, first) and the maximum-speed branch's squared length
// differ (ours the vector normalizer's k,i,j statements; retail (i+j)+k with k in the lower register).
// Round 90: first-particle local 10; settings pointer 10; const properties 10; const particle 10;
// const nearest 10; separate speed-squared value 10; length_sq3f 105; the particle squared helper
// 107; force-inline dot3f declaration 10. Original body and declarations restored.
// Round 125 (lane 4): a cache.values pointer local 10; properties read through the definition at
// each use 297; the vector normalizer's sum as one expression 34; the velocity normalizer in both
// branches with k read first and (i+j)+k 34; the first input mask read before the cache setup 38. A
// one-link LTCG harness (exact for 0x2b98b0/0x2b9750) finds no body of the vector, direction or
// velocity normalizer that gives this branch's (i+j)+k and keeps 0x2b98b0 and 0x2b9750: the k-first
// i,j,k statement form needs a normalizer of its own.
// Round 7 (lane 14): the minimum-speed normalization folds retail's j/k loads only when the maximum
// branch sums its squared length in statements; maximum branch through the vector normalizer and the
// minimum branch's normalize split from its scale 10 (kept); maximum branch alone 52; the direction
// normalizer there 17; a one-caller i,j,k statement normalizer 9, with k read into a local first 2
// (only the reloads left; not kept, new helper); the velocity normalizer in both branches with k (or
// k,j,i) read first 10/10 (1517 bytes); next before properties 10; properties/next declared at the
// loop 43/64; on the 2 form: parameter orders with the record second or before scale 2/2, particle
// declared outside the loop 2, a separate particle mask 51, the successor through a local 2.
// Round 9 (lane 14): change scaled through the file's scale helper 10 (same code); the maximum-change
// scale the same 10; the change's squared length through the direction helper 42; the loop's mask
// computed at the top of the loop 328; the maximum branch's final scale through the scale helper 45.
// Earlier rounds (the minimum normalization then stored j and k one by one):
// Round 90: the blend's previous add through vector3f_add with the squared speed from dot3f 34 (kept;
// the add is retail's in-place block); with the direction/vector squared helpers 105/102, length_sq3f 113,
// written-out sum 115; inverse-first j/k products in the normalizer 34 (same code); the minimum branch
// split into normalize then scale 34; next declared before properties 34; dot3f in the normalizer or
// for the change's squared length 34/34.
// Frame 0xb4 agrees; 1517/1516 bytes. Back-edge parameter reloads and velocity
// blend/normalization stores differ; shared scale return plus addition recovers the attraction's in-place products.
// Earlier frame-matched body kept attraction j/k products in temporaries and deferred velocity stores.
// Round 9 (lane 11): steering record replaced by plain locals 291 (frame 0xac); the distance estimate with
// separate input and result variables (as flexible_surface.cpp) 254 (frame 0xb4); the 0x2b9750 call with
// retail's argument registers 252; change zeroed i first 250; attraction delta and length through the
// existing point-difference and direction-length helpers 213 (1516 bytes); its normalize through the
// existing direction helper 196. On 196: compound multiplies in the scale helper 196; scaled delta added
// straight into the change 213; direction helper in the change clamp 213 (1525 bytes); estimate before
// and amount after the normalize 185 (lockstep only: retail divides before the branch; not kept);
// names of the steering locals do not move their slots (spill slots).
// Earlier: frame 0xb8 versus 0xb4; cache/steering/estimate/inverse homes and staged stores remained.
// Round 138: the extra frame dword is the attraction delta: as a member of the escaped
// steering record its scaled values are stored back and never read (retail keeps them in
// registers), and ours shares two float homes retail keeps apart. Delta as its own local
// in its block/before/after the record 301/301/301; delta and previous both outside 281;
// the existing squared-length helper for the attraction/clamp/velocity sums 280/282/294,
// all three 294.
// component loops: velocity plus change 248; velocity plus previous 288;
// attraction addition 280; change scale 269; maximum-change scale 269.
// Shared inverse 249; shared squared length 277; both carriers 276; unsigned handle 249; attraction scale loop 269.
// Frame 0xb8/0xb4 and scalar/vector/cache homes remain;
// independent delta/previous 281; independent change/previous 293; all vectors independent 303;
// property-array field reference 324; complete cache zero aggregate 410 (fast).
// Frame 0xb8 versus 0xb4 and cache/scratch homes remain; hoisted origin pointer 249; previous library copy 249; change library clear 297; both serial additions 286; both additions plus library copy 286.
// Round 210: Direct scalar members 277; direct change/delta/previous members 313/324/274; all
// vector members direct 349. Frame 0xb8 versus 0xb4 and cache/estimate/inverse homes remain.
// frame 0xb8/0xb4 and scalar/vector/cache homes remain;
// Round 342: later nearest-result lifetime 360; successor after nearest 320; property-field
// rereads 324; shared previous-velocity scale 249; shared final speed scales 269.
// Round 215: independent minimum/maximum/change limits 278/270/280;
// independent minimum+maximum limits 274; minimum+change limits 264.
// Independent maximum-speed/maximum-change pair 264/1517; with fresh property fields 365/1516;
// library previous copy with those two forms 264/365; separate particle mask 280/1530, plus
// fresh properties 351/1521, plus library copy 280, all three 351. Frame remains 0xb8/0xb4.
// Original full steering/cache layout, masks, vector copies and every call role restored.
// Nearest, delta, previous-velocity, requested-mask and cursor names 249 each; cache/cursor
// declaration and cache initialization order 249; redundant cache reset removed 366.
// Round 350: independent cache validity/system/emitter/particle 408/330/340/354;
// three contexts 377; validity+particle 412; complete native cache 425;
// complete native cache with independent steering 435. Frame 0xb8/0xb4 and estimate/inverse homes remain.
// Round 250: Ordinary inline reciprocal/estimate/both/scale/reciprocal+scale/estimate+scale/all
// 440/249/440/249/440/249/440; all with external inline linkage 440. Reciprocal metadata loses four
// reduced sibling controls; every helper contract restored. Frame 0xb8/0xb4 and
// estimate/reciprocal/cache homes remain.
// Round 253: Native scalar reciprocal with original steering storage 412;
// independent delta/previous/both 432/382/406; all vectors 431; all limits 399;
// maximum-speed/change pair 397; all independent 415. Every form aligns the
// frame to 16 and loses four matched siblings in preview; original helper/storage restored.
// Round 84: velocity block through a named velocity pointer 430 (1290 bytes); attraction amount
// through the existing scale helper 196 (same code); an inline pointer-parameter vector add for both
// velocity additions 234 (1541 bytes); next declared at the loop 242; attraction normalize through the
// header's vector3f_scale 213. Restored.
// Round 27: native scale/direction/scale+direction/velocity input-output contracts
// 199/196/199/243; scale+velocity/direction+velocity/all 255/243/255; all with
// shared velocity squared length 255. Direction contracts lose two matched
// siblings in preview; every original helper contract and body restored.
// Round 245: shared in-place scale orders ikj/jik/jki/kij/kji 208/209/216/223/222;
// complete factor-first products in ijk/jik/kji order 196/209/222. Original helper restored.
// Back-edge reload order, attraction temporaries and serial velocity stores remain.
// Round 208: loop-entry input-mask acquisition 271; scoped linked induction on the
// independent-vector graph 242; existing shared addition for
// attraction/change/attraction+change/previous/attraction+previous/change+previous
// 196/208/208/212/212/234. All restored. Frame and length agree; back-edge reloads and
// normalization/velocity stores remain.
// Round 377: independent caller attraction/change/both-speed normalized outputs 423/387/325;
// paired attraction+change/attraction+speed/change+speed/all 591/514/490/679;
// all with shared previous output 629. All restored; frame/reload/store differences remain.
// Round 211: Complete shared direction/velocity/scale input snapshots 563/327/585;
// direction+velocity/direction+scale/velocity+scale/all 653/831/677/923; all with read-only
// scale/velocity snapshots 923. All restored; normalization transfers and velocity stores remain.
// Round 31: shared attraction scale return consumed by shared addition 159; native change-stage scales 196;
// both boundaries 158 (only one additional point, not retained); settings/origin/first placeholder names 196,
// with attraction/change/both contracts 159/196/158; all five placeholder names plus both contracts 158.
// Original parameter names and both original change-scale stages retained.
// Round 379: Successor after bounds/attraction/collision/at back edge 310/341/364/247; with separate particle-input mask 300/272/353/223. Original early snapshot and body restored.
// DH round 215: Distinct velocity-normalizer store orders ikj/jik/jki/kij/kji,
// existing shared scale at its output, and complete inverse-first products ijk/kji
// 196 each. Original helper and every caller stage restored; reloads and transfers remain.
// Round 34: On the retained attraction-return graph, shared change addition 67 (plain);
// previous-scale/quarter-scale/both returns 171/112/173; those with shared change
// 178/114/178; scaled previous addition 171; shared previous addition 93, with
// shared change 112, with both native scales 110. Back-edge reloads and final stores remain.
// Round 166: shared scaled-add blend boundaries quarter/previous/native-previous/composed
// 124/117/124/115; each with both final returned-output scales
// 124/125/124/122. Original blend stages and stores restored.
// Round 228: Native velocity-normalizer output consumed by both final scales, pointer/reference
// inputs with existing/shared scales 67/67/47/47; corresponding shared final squared producers
// 116 each. Keep pointer output into shared scale, plain 47 (1517/1516 bytes); final-store and
// back-edge reload differences remain. Original magnitude expression and pointer input retained.
// DH round 230: On the retained normalizer-output graph, shared final addition/reversed inputs 115/119;
// native normalizer values alone/with those additions 398/386/386; explicit source-output normalization
// with shared scale alone/with those additions 138/177/183. All restored; final stores, lower tail and reload order remain.
// Round 394: direct previous-vector scale producer with native/quarter/maximum/maximum+quarter/
// initial/initial+quarter/initial+maximum/all scales 208/203/208/203/208/203/208/203; restored.
// DH round 232: On the retained pointer-output graph, quarter-output/final-add composition 117;
// final component loop 111; scaled-add publication 115; normalizer loop/ordinary inline/inverse
// lifetime/native slot names 47 each; both publication and normalizer loops 107 at retail length.
// Every executable form restored; lower-tail stores and back-edge reload order remain.
// Round 6: the minimum branch alone differs in the tail (retail's j/k products are the copy and
// in-place forms of values defined before the inverse; the identical maximum branch is
// sequential in both). Minimum scale with the normalized pointer as its output 34; the in-place
// scale helper on the normalized pointer 34; a named velocity pointer for the blend and clamp
// 397, for the clamp only 260; previous velocity scaled into its own vector 191. Restored.
// Round 282: selected-bound common completion, upper/lower defaults and split normalize/scale
// 365/362/342/365; with shared change magnitude 359/360/355/359 (frame 0xac).
// Shared normalization-product setter 37 (1519 bytes). All restored; native 0xb4 frame,
// back-edge definition/first reloads and lower normalization publication remain.
// Round 30: matched 0x30bf0/0x1201a0 get retail's paired j/k products from `v->i = v->i * inv;
// v->j = inv * v->j; v->k = inv * v->k;` with a computed inv; with the asm-written inverse every
// product order compiles the same. That order in the normalizer 34 (same code); file scale
// helper in the normalizer 34 (same code); file scale helper for the minimum's final scale 34
// (same code); squared-first minimum test 34 (same code); next assigned just before the loop 86.
// Round 204: vector-squared input by const reference, ordinary/forced squared and normalizer
// availability, and void/pointer normalizer output: eight contracts 10 each. All original
// helpers and marked bodies restored; back-edge reloads and maximum squared producer remain.
// Round 24 (lane 10): the vector normalizer's sum through length_sq3f 34 (0x2b98b0 59); the
// vector-squared statements in i,j,k order 9 (0x2b98b0 61); the velocity normalizer with k read first
// and i,j,k statements, in both branches 10 (1517 bytes: maximum branch exact, the minimum's j product
// loads j instead of copying the inverse); successor read right after the particle pointer 328; cache
// declared first 10. Compile-only screen of the tail (~900 forms): an expression sum in the maximum
// branch always turns the minimum's j/k scale sequential; k read first with i,j,k statements gives the
// whole tail, but no vector-squared body with it keeps 0x2b98b0 (its first normalize needs k,i,j).
// Round 33 (lane 14): the back-edge reloads follow the prologue order in ours (first, next, definition)
// and run reversed in retail. Loop over the index parameter with the head in a local 79 (1524 bytes);
// index/head names 10 (same code); change zeroed i first 19; change declared at the loop top 10 (same
// code); unsigned index 10 (same code).
// @retail 0x2b9b10
void function_2b9b10(s_particle_properties_2ba const *definition, void const *origin, long first, real scale, void *system)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        if (particle != cache.current_particle)
        {
            cache.current_particle = particle;
            cache.valid &= 0xffff07f0;
        }
        requested = definition->input_mask & 0xf80f;
        function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
        cache.valid |= requested;
        next = particle->next;
        s_particle_2b96 *nearest = function_2b9670(particle, first);
        real minimum_speed = function_246cd0(&properties[6].property, cache.values);
        real maximum_speed = function_246cd0(&properties[7].property, cache.values);
        real maximum_change = function_246cd0(&properties[8].property, cache.values);
        vector3f change;
        change.k = change.j = change.i = 0.0f;
        if (nearest)
        {
            function_2b9750(particle, nearest,
                function_246cd0(&properties[0].property, cache.values),
                function_246cd0(&properties[1].property, cache.values), &change);
            function_2b98b0(particle, nearest,
                function_246cd0(&properties[4].property, cache.values), &change);
        }
        real attraction_scale = function_246cd0(&properties[3].property, cache.values);
        real attraction_radius = function_246cd0(&properties[2].property, cache.values);
        if (attraction_radius > 0.0f)
        {
            point3f const *center = (point3f const *)((byte const *)origin + 0x10);
            vector3f delta;
            vector3d_from_points3d(&particle->position, center, &delta);
            real squared = particle_direction_squared_2b(&delta);
            real amount = particle_distance_estimate_2b(squared) / attraction_radius * attraction_scale;
            normalize_particle_direction_2b(&delta);
            vector3f_add(&change, vector3f_scale(&delta, amount, &delta), &change);
        }
        function_2b9a00(particle, function_246cd0(&properties[5].property, cache.values), &change);
        change.i *= scale;
        change.j *= scale;
        change.k *= scale;
        real squared = change.k * change.k + change.j * change.j + change.i * change.i;
        if (particle_distance_estimate_2b(squared) > maximum_change)
        {
            if (squared != 0.0f)
            {
                real inverse = inverse_sqrt_2b96(squared);
                scale_particle_vector_2b(inverse, &change);
            }
            change.i *= maximum_change;
            change.j *= maximum_change;
            change.k *= maximum_change;
        }
        vector3f previous = particle->velocity;
        scale_particle_vector_2b(0.75f, &previous);
        vector3f_add(&particle->velocity, &change, &particle->velocity);
        scale_particle_vector_2b(0.25f, &particle->velocity);
        vector3f_add(&particle->velocity, &previous, &particle->velocity);
        squared = dot3f(&particle->velocity, &particle->velocity);
        if (squared > maximum_speed * maximum_speed)
        {
            normalize_particle_vector_2b(&particle->velocity);
            vector3f_scale(&particle->velocity, maximum_speed, &particle->velocity);
        }
        else if (minimum_speed * minimum_speed > squared)
        {
            normalize_particle_velocity_2b(&particle->velocity);
            vector3f_scale(&particle->velocity, minimum_speed, &particle->velocity);
        }
    }
}


PRIVATE __declspec(noinline) void particle_damping_update_2b(s_particle_properties_2ba const *definition,
    void *system, long first, real scale);

// @retail 0x2b8e10
PRIVATE void particle_damping_update_2b(s_particle_properties_2ba const *definition,
    void *system, long first, real scale)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    long next = first;
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    real properties_values[3] = { 0.0f, 0.0f, 0.0f };
    dword remaining = definition->constant_mask;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    for (dword i = 0; i < 3 && remaining; ++i)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            properties_values[i] = function_246cd0(&properties[i].property, cache.values);
            remaining &= ~bit;
        }
    }
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        if (!(*((byte *)particle + 2) & 9) && *(real *)((byte *)particle + 8) <= 1.0f)
        {
            if (particle != cache.current_particle)
            {
                cache.current_particle = particle;
                cache.valid &= 0xffff07f0;
            }
            requested = definition->input_mask & 0xf80f;
            function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
            cache.valid |= requested;
            for (dword j = 0; j < 3; ++j)
            {
                if (!(definition->constant_mask & (1 << j)))
                    properties_values[j] = function_246cd0(&properties[j].property, cache.values);
            }
            real linear = properties_values[1] * scale;
            real angular = properties_values[2] * scale;
            linear = linear < 0.0f ? 0.0f : (linear > 1.0f ? 1.0f : linear);
            angular = angular < 0.0f ? 0.0f : (angular > 1.0f ? 1.0f : angular);
            particle->velocity.k -= g_51e9c4->unknown0 * properties_values[0] * scale;
            scale_particle_vector_2b(1.0f - linear, &particle->velocity);
            *(real *)((byte *)particle + 0x34) *= 1.0f - angular;
        }
    }
}

void __stdcall function_2b9060(s_particle_properties_2ba const *definition, void *system,
    long first, real scale, long mode);

#include "effects.h"

struct s_object_246eb0;
struct s_particle_impact
{
    point3f position;
    vector3f direction;
    vector3f normal;
    short index;
};

class c_particle_collision_interface_2b
{
public:
    virtual dword group() = 0;
    virtual long tag_index() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void v13() = 0;
    virtual bool stop_on_surface() = 0;
    virtual bool stop_on_object() = 0;
};

void function_246fa0(s_object_246eb0 const *particle, s_particle_impact const *impact, long tag_index);
void function_247000(s_object_246eb0 const *particle, volatile long tag_index);
void function_247070(s_object_246eb0 const *particle, s_particle_impact const *impact, long definition_index, long tag_index);

real g_55e618;

// score 102 (was 102): frame 0x124 agrees; sweep/collision homes and projection spills differ.
// Velocity snapshot argument 676; reflection loop 102; reflected value 241; sweep scale 129;
// both scales 129; coefficient local 102; displacement scale 129; snapshot plus loop 676;
// shared tangent 190; shared start 102; both shared differences 102. Original body retained.
// Earlier: retail copies dot for perp.i/j and multiplies perp.k in place, ours
// copies the normal components. On the velocity-pointer form with the position built first
// and the products interleaved (185): velocity pointer before normal 187, normal pointer
// after velocity 189, keep after the products 190 (1537 bytes, frame 0x118); interleaved
// products on this body 209 / 211.
// Round 410: Shared full-vector response assignment, with shared projection, shared tangent, and
// both producers 191/191/191/191. Separate placement inline/noinline declarations with that
// consumer 285/191; alone 289/102. All restored; sweep/collision homes and projection spills
// remain.
// Round 293: Destructive normal-copy projection/velocity-copy tangent/both 265/283/332; scaled response 101 (one-count gain rejected); projection/response 264, tangent/response 278, all stages 337, all with direct velocity producer 285. Original vectors and complete body restored.
// Round 44: velocity read at each use 199; velocity pointer 194 (frame 0x118, sweep at +0x44); renamed
// perpendicular/tangent locals 102/102 (no code change); normal read as a union member 605, with the velocity pointer
// 601, interleaved 596. Grid of velocity copy/pointer/initialised copy, three dot-product text orders, sequential or
// per-component interleaved perpendicular/tangent, keep factor early/late, position after/before the tangent: best 102
// (this body), interleaved with the position first 124, pointer with the position first 125, the rest 194-237; the dot
// product's text order never changed the code. Retail homes tangent at +0x38 and perpendicular at +0xa4.
// Round 83: tangent/perpendicular/both declared before the sweep 102/102/102; scalar perpendicular
// 147; scalar tangent 113; both scalar 140; call-only velocity pointer 102; that pointer with a
// staged position 245. Original body restored.
// Round r4 (lane 17): velocity pointer (no copy) 194 (1541 bytes, frame 0x118): every stack home below the
// collision buffer then agrees with retail, which also homes perpendicular (perp.k spilled at +0xac) under the
// buffer, and keeps the normal in registers until the position stores, which it sinks after impact.index;
// tangent through the point-difference helper on that form 194; per-component perpendicular/tangent order 195;
// position assigned after impact.index on the pointer/copy forms 194/102 (stores already scheduled the same);
// a local copy of the normal for dot/perpendicular/position 279 (1590 bytes), with the velocity pointer 227 (1559).
// Round 105: position through the existing real_point3d_set 245; with the velocity pointer 188;
// velocity read at each use 189; pointer with interleaved perpendicular/tangent 185; the set nested
// as placement_set's position argument 185/188/245 (code identical to the separate statement).
// Retail loads the collision fields (and the call's last argument) above the particle stores; ours
// cannot, so the collision local may escape through 0x1697c0 in our build.
// Round 100 (lane 8, fast): dot3f(&velocity, normal) 102 and dot3f(normal, &velocity) 102 (same code); no velocity copy
// with dot3f on the particle velocity 200, with the dot in i/k/j text order 199, velocity operand first 199 (frame
// 0x118 each); the normal as a `vector3f const &` 102 (same code), with no copy 199; the placement's last argument read
// into a local before the position stores (diagnostic) 137. 0x1697c0's callees that receive the result are all LTCG
// bodies and none stores the pointer.
// Round 115: position through the existing vector3f_add_scaled 102 (same code); perpendicular through the
// existing vector3f_scale 102; both 102; with the velocity pointer 194 (frame 0x118: the perpendicular loses its
// home); with the velocity read at each use 199. Local names do not move the stack homes (compile-only frame
// listing unchanged for four renames); ours homes sweep, perpendicular, (cache, collision), tangent, retail
// tangent, sweep, (cache), perpendicular, (collision).
// Round 43 (one-file LTCG link of this file with the callee closure and the inlining ballast object, equal to
// the full listing, 74 there): the collision pointer does reach a non-LTCG body (0x244980 stores it in its
// ray-cast callback and hands that to the Havok cast at 0x2df720), but retail's Havok is the same, so that
// does not separate the builds. On the velocity pointer form (harness 183, frame 0x118): keep computed after
// the tangent 187; tangent declared before perpendicular 183; the impact declared at the block's top 183;
// perpendicular and tangent interleaved per component 183, with keep late 187 (all at least 185 here).
// Round 116 (lane 6, fast, LTCG build): four rename sets of perpendicular/tangent/sweep (normal/tangent
// velocity, bounce/slide, travel, along/motion/rebound) 102 each (homes unmoved, so the same-size name order
// does not apply here); tangent, tangent and perpendicular, tangent at the top with perpendicular after the
// cache 102 each; velocity read at each use with the perpendicular/tangent rows interleaved i, i, j, k, j, k
// 200 (frame changes, +0x2); the copy form with that interleave 211. Retail keeps perp.i/j, tangent.k and
// keep in xmm5/6/7/3 across the placement call (its LTCG callee leaves them) and spills only tangent.i/j and
// perp.k.
// Round 40 (lane 11, fast): retail stores the collision record's +0x24 NONE word before the start point's
// x/z stores; `collision.value.unknown24 = NONE;` ahead of `start.x -= displacement.i;` gives that order
// 100 (two-point gain, not kept), ahead of the displacement 134. On that order: the velocity read from the
// particle at each use 197 (frame 0x118), with the position stores after impact.index 197; the position
// stores after impact.index alone 100 (same code); a `vector3f *velocity = &particle->velocity` pointer
// (also passed to the placement and used for the final stores) 193 (frame 0x118; the dot's k term and
// `lea esi` then match retail, but the i product keeps the normal as its destination where retail loads
// velocity.i, and the perpendicular gets no home).
// Round 64 (lane 12; one-link LTCG env of this file with its stand-in callers and depth-10 closure, equal to
// the full listing; env positional/aligned 74/84 for this body): retail loads every velocity component twice (dot,
// then tangent) and the placement's last argument before the position stores. New: the dot read straight from
// the particle with a velocity pointer for the tangent 187/108; the velocity copy's three stores in the five
// other orders 76/88, 73/80, 73/80, 76/88, 76/86 (ikj, jik, jki, kij, kji); the perpendicular's 76/84, 76/82,
// 76/85, 76/85, 76/82; the tangent's 74/84 each; a non-escaping stand-in for 0x1697c0 (diagnostic) gives a
// 0x10c frame, so the collision record escapes in retail as in ours and the early argument read is not
// escape-driven. Re-measured in the env only (listed above): the velocity pointer, with dot3f, with
// vector3f_scale 183/102 each.
// Round 67 (lane 12, same env; positional/aligned): the tangent through the existing vector3f_assign 74/84 (same
// code); with the perpendicular through vector3f_scale 76/87; the final velocity through vector3f_assign 175/77
// (1545 bytes); the velocity pointer with perpendicular/tangent/final setters, with dot3f, or with only the final
// setter 184/103 each (1541 bytes, frame 0x118: perpendicular still gets no home); the pointer declared at the
// loop top and used for the displacement too 279/201 (1567 bytes).
// Round 36 (lane 13, fast): retail's homes follow its spills (tangent.i/j spilled twice each sit low at +0x38,
// perp.k once at +0xa4 under the collision); it holds the three position sums in xmm1/xmm4/xmm2 until after
// impact.index, so only perp.i/j, tangent.k and keep cross the call. On the velocity pointer form (194): dot3f
// 194; dot3f with the perpendicular through vector3f_scale 194; dot3f(normal, velocity) with it 194 (all the
// same code: helpers don't move the dot * normal operand order, ours keeps the normal as the destination);
// tangent written as `velocity - dot * normal` 207 (1547 bytes); on the copy form 242 (1558 bytes).
// Then 45 random dependency-respecting orders of the reflection block's 15 statements (keep, copy, dot,
// perpendicular, tangent, position, impact.index) and of the four updates after the call, a third of them on
// the pointer form, in a one-file LTCG link equal to the full build: none below this body (link positional
// 74, aligned 84; the next best 76/100 and 91/93, sizes 1543-1555).
// Round 60 (lane 16, fast): retail reads each velocity component twice from the particle (dot, then tangent)
// and keeps keep/perp.i/perp.j/tangent.k in xmm3/5/6/7 across the placement call (tangent.i/j and perp.k
// in memory); ours keeps five values in registers. The velocity pointer form 195 (frame 0x118); with the
// impact declared and its index stored before the position stores 195; with perpendicular/tangent in retail's
// computation order (perp.i, tangent.i, perp.j, perp.k, tangent.j, tangent.k) 196; on the copy form the
// collision word stored before the start stores plus the impact before the position 100 (two-point gain,
// not kept); the copy form in retail's computation order 211; keep computed after impact.index 197 (1536 bytes).
// @retail 0x2b9060
void __stdcall function_2b9060(s_particle_properties_2ba const *definition, void *system,
    long first, real scale, long mode)
{
    s_particle_property_entry_2ba *properties = definition->properties;
    c_particle_collision_interface_2b *material = (c_particle_collision_interface_2b *)function_137bd0(
        ((s_particle_system_datum *)system)->function_1751d0()->tag_index);
    long next = first;
    union
    {
        s_collision_result_1697c0 value;
        byte storage[0x5c];
    } collision;
    s_particle_cache_2ba cache;
    cache.valid = 0;
    cache.current_system = 0;
    cache.current_emitter = 0;
    cache.current_particle = 0;
    real coefficients[2] = { 0.0f, 0.0f };
    dword remaining = definition->constant_mask;
    if (system)
    {
        cache.current_system = system;
        cache.valid = 0;
    }
    dword requested = definition->input_mask & 0x107f0;
    function_173ba0(requested, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
    cache.valid |= requested;
    for (dword i = 0; i < 2 && remaining; ++i)
    {
        dword bit = 1 << i;
        if (remaining & bit)
        {
            coefficients[i] = function_246cd0(&properties[i].property, cache.values);
            remaining &= ~bit;
        }
    }
    while (next != NONE)
    {
        s_particle_2b96 *particle = &((s_particle_2b96 *)g_51ec84->data)[next & 0xffff];
        next = particle->next;
        word *flags = (word *)((byte *)particle + 2);
        if ((*flags & 9) || !(*(real *)((byte *)particle + 8) <= 1.0f))
            continue;
        word countdown = *flags >> 13;
        if (countdown == 0)
        {
            if (particle != cache.current_particle)
            {
                cache.current_particle = particle;
                cache.valid &= 0xffff07f0;
            }
            requested = definition->input_mask & 0xf80f;
            function_173ba0(requested & ~cache.valid, cache.current_system, cache.current_emitter, cache.current_particle, cache.values);
            cache.valid |= requested;
            long mode2 = mode & 2;
            dword collision_flags = 0x800000 + (mode2 != 0);
            if (mode2) collision_flags |= 4;
            else collision_flags &= ~4;
            if (mode & 4) collision_flags |= 2;
            else collision_flags &= ~2;
            if (mode & 0x10) collision_flags |= 0x20;
            else collision_flags &= ~0x20;
            if (mode & 8) collision_flags |= 0x400;
            else collision_flags &= ~0x400;
            if (mode & 0x20) collision_flags |= 0x10;
            else collision_flags &= ~0x10;
            if (collision_flags & 0x430) collision_flags |= 8;
            if (!(mode & 1) && !mode2 && !(mode & 4) && !(mode & 8))
            {
                *flags &= ~2;
                continue;
            }
            vector3f displacement;
            vector3f sweep;
            displacement.i = particle->velocity.i * scale;
            point3f start = particle->position;
            sweep.i = displacement.i * 12.0f;
            displacement.j = particle->velocity.j * scale;
            sweep.j = displacement.j * 12.0f;
            displacement.k = particle->velocity.k * scale;
            sweep.k = displacement.k * 12.0f;
            start.x -= displacement.i;
            start.y -= displacement.j;
            start.z -= displacement.k;
            collision.value.unknown24 = NONE;
            if (function_1697c0(collision_flags, &start, &sweep, NONE, NONE, &collision.value))
            {
                if (g_55e618 > *(real *)(collision.storage + 4))
                {
                    for (dword j = 0; j < 2 && remaining; ++j)
                    {
                        dword bit = 1 << j;
                        if (remaining & bit)
                        {
                            coefficients[j] = function_246cd0(&properties[j].property, cache.values);
                            remaining &= ~bit;
                        }
                    }
                    vector3f const *normal = (vector3f const *)(collision.storage + 0x28);
                    real keep = 1.0f - coefficients[0];
                    vector3f velocity;
                    velocity.i = particle->velocity.i;
                    velocity.j = particle->velocity.j;
                    velocity.k = particle->velocity.k;
                    real dot = velocity.i * normal->i + normal->k * velocity.k + normal->j * velocity.j;
                    vector3f perpendicular;
                    perpendicular.i = dot * normal->i;
                    perpendicular.j = dot * normal->j;
                    perpendicular.k = dot * normal->k;
                    vector3f tangent;
                    tangent.i = velocity.i - perpendicular.i;
                    tangent.j = velocity.j - perpendicular.j;
                    tangent.k = velocity.k - perpendicular.k;
                    particle->position.x = normal->i * 0.005f + collision.value.point.x;
                    particle->position.y = normal->j * 0.005f + collision.value.point.y;
                    particle->position.z = normal->k * 0.005f + collision.value.point.z;
                    s_particle_impact impact;
                    impact.index = NONE;
                    placement_set((s_placement *)&impact, &particle->velocity, &particle->position, normal, collision.value.unknown24);
                    particle->velocity.i = tangent.i * keep - perpendicular.i * coefficients[1];
                    particle->velocity.j = tangent.j * keep - perpendicular.j * coefficients[1];
                    particle->velocity.k = tangent.k * keep - perpendicular.k * coefficients[1];
                    *(real *)((byte *)particle + 0x34) *= keep;
                    if (!(*flags & 2))
                    {
                        if (((s_particle_system_datum *)system)->function_1751d0()->location_mode != 1 && material->tag_index() != NONE)
                        {
                            long definition_index = ((s_particle_system_datum *)system)->function_1751d0()->tag_index;
                            long tag_index = material->tag_index();
                            dword group = material->group();
                            if (tag_index != NONE)
                            {
                                switch (group)
                                {
                                case 'snd!': function_247000((s_object_246eb0 *)particle, tag_index); break;
                                case 'foot': function_247070((s_object_246eb0 *)particle, &impact, definition_index, tag_index); break;
                                case 'effe': function_246fa0((s_object_246eb0 *)particle, &impact, tag_index); break;
                                }
                            }
                        }
                        *flags |= 2;
                    }
                    if (normal->k > 0.8f) *flags |= 0x10;
                    else *flags &= ~0x10;
                    if ((*(long *)collision.storage == 1 && material->stop_on_surface()) ||
                        (*(long *)collision.storage == 3 && material->stop_on_surface()) ||
                        (*(long *)collision.storage == 2 && material->stop_on_object()))
                        *flags |= 1;
                }
                else
                    *flags &= 0x1fff;
            }
            else
                *flags = (*flags & 0x1ffd) | 0xa000;
        }
        else
        {
            *flags = (*flags & 0x1fff) | ((countdown - 1) << 13);
        }
    }
}

// @retail 0x2b8da0
void function_2b8da0(s_particle_properties_2ba const *definition, void *system,
    long first, void const *origin, real scale, long mode)
{
    switch (*(word const *)definition)
    {
    case 0:
        particle_damping_update_2b(definition, system, first, scale);
        break;
    case 1:
        function_2b9060(definition, system, first, scale, mode);
        break;
    case 2:
        function_2b9b10(definition, origin, first, scale, system);
        break;
    case 3:
        function_2ba100(definition, system, first, scale);
        break;
    }
}
