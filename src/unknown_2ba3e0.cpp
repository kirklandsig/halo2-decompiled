/* UNKNOWN_2BA3E0.CPP: filling a placement (a position, a unit direction and
   a second vector) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "unknown_1eb550.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "reciprocal_normalize.h"

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

/* the placement's word-sized value: callers build it in the stack argument slot */
class c_placement_value
{
public:
	c_placement_value(short value) : m_value(value) {}
	short m_value;
};

// @retail 0x2ba3e0
void placement_set(s_placement *placement, vector3f const *direction, point3f const *position, vector3f const *up, c_placement_value value)
{
	vector3f normal = *direction;

	function_1201a0(&normal, g_4687bc);
	placement->value24 = value.m_value;
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
// Round 49 (lane 14, fast): 1-8 unused locals at entry never move the back-edge reloads (10/142/16/10/23/23/110/
// 110 from float operand orders), so their order is no numbering tie; the attraction center written at its
// use 10; the distance estimate named 10; the attraction scale and add as two statements 10; the separation
// radius/scale named before the call 303 (1523 bytes); the 0x2b9a00 property named before its call 236
// (1521 bytes); re-measured: a separate speed-squared value 10.
// Round 120 (lane 7, fast): the two branches' normalizers swapped 35 (1520 bytes); the header's
// normalize_inline in the maximum branch 230; the velocity normalizer then the file scale helper there
// 32 (1521); the velocity normalizer's sum through length_sq3f, in both branches 29 (1521); its sum as
// i,j,k statements in both 41 (1520); as k + (i + j) in both 75.
// Round 135: k-first velocity sum in maximum/vector in minimum 26 (1517 bytes);
// k-first velocity sum in maximum/direction in minimum 26 (1517 bytes);
// properties/cache/next declarations 10; next/cache/properties 10; cache/next/properties 10;
// the same three declaration orders with original properties/next assignments 10 each.
// All restored; back-edge reloads and the maximum squared producer are unchanged.
// Round 127 (lane 7, fast), on the one-caller k-first normalizer form 2: a head copy of first at the top 2, after next 2;
// a definition copy for the properties and masks 34 (1521 bytes); both copies 129 (1518); a named index before the nearest
// call 2; a per-particle mask 51 (1517), assigned to requested 2; a definition copy for the later mask only 2. The reload
// order never moves; matched functions (0x1384a0, 0x168e20, 0x2c05f0) reload in reverse prologue order as retail does here.
// Round 94 (fast): both cache updates through the existing s_particle_value_cache::function_173c10 member (it inlines) 128
// (1518 bytes); system declared second at the definition and its one call 10; the velocity normalizer in the maximum branch
// too 34 (1521); the cache type renamed 10 (same code); `if (next != NONE) do ... while` 10 (same code). As in 0x2bdc70 and
// 0x281b60, ours reloads the parameters in descending declaration order and retail ascending. The blend's first add as
// change + velocity 10 (same code).
// Round 94 (lane 16, fast): particle as the pool data plus the masked index 10; change zeroed by an aggregate
// initializer 10; maximum test as `maximum_speed * maximum_speed < squared` 10; minimum test as
// `squared < minimum_speed * minimum_speed` 10; change scaled through vector3f_scale 10. With first and definition
// swapped (diagnostic) the reloads keep their positional order (third parameter, next, first parameter).
// Round 72 (m3 lane 14, fast): first typed dword 10; next typed dword 10; next declared in a `for` header with the
// successor read in the body 64 (1517 bytes); nearest declared at function scope 10; the particle-change clear as
// `&= ~0xf80f` 10.
// Round 60 (m3 lane 2, fast): the system block storing the validity before the system 12; the three cache
// pointers cleared by one chained assignment 12; first as `long const` 10; definition as a const pointer 10; the
// attraction test as `0.0f < attraction_radius` 10 (the 10s the same code).
// Round 145 (m3 lane 7, fast): one redundant pair of parentheses each: the attraction quotient, the change's first
// squared product and the maximum test's square 59 each (1517 bytes), the minimum test's square 42 (1521); the
// masks, the particle index, the center pointer, the chained zeroing and the tests 10 (same code). Renames
// reversing the name order of definition, first and next (one, two or all three) 10; renames of particle, nearest,
// properties, cache and requested 10; requested declared beside the cache 10; change at function scope 10. The
// back-edge reloads never move.
// Round 74 (m3 lane 2, fast): the velocity normalizer in the maximum branch too 34 (1521 bytes; re-measured, as round 94:
// the maximum branch is then exact, but the minimum branch's j/k inverse products turn sequential where retail pairs them,
// so that pairing follows a numbering tie like the reloads); on that, vector3f_scale / scale_particle_vector_2b /
// chained normalizer returns per branch 34/37; a 48-form random screen (comparison orientation, change and
// maximum-change scales through the file scale helper, a separate speed-squared local, the center inline, chained
// returns) 34 in all; the direction normalizer in the maximum branch 17 (re-measured, as round 7); the previous blend
// through vector3f_scale on the shared-normalizer form 34.
// Round 117 (m3 lane 12): the permuter (`--fast --anneal`, 600 s, 82 tries) best 10. The tree's statement-form i, j, k
// squared sums are all file-local (path.cpp, unknown_1a58b0.cpp, impacts.cpp) and none reads k first; matched 0x2b8e10
// keeps nothing in registers across its back edge, so it shows no reload order to follow.
// Round 91 (m3 lane 14, fast): the EDX parameter system moved to the first, second, third or fourth position
// (definition and the call together, stack unchanged) 10/10/10/10; the maximum test's square named where it is
// computed 59 (1517 bytes). Phase scan (1-7 unused initialised locals) of the velocity normalizer in both
// branches: 34/34/128/33/39/41/41/93; the minimum branch's inverse-first j/k pair appears at no phase.
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

// MiniMax G (11-10): no new form, no lower score (102): the sweep vector and the collision record both sit twelve bytes lower here than in retail (sweep +0x38 against +0x44, record +0xa4 against +0xb0), so retail's frame holds twelve bytes more before the sweep; the aggregates this body declares do not account for that gap.
// MiniMax S (11-09): no new form, no lower score (102); retail's sweep and collision homes
// sit twelve bytes higher in the frame than ours (F+0x44 against F+0x38), the local handed to
// the projection call sits at F+0xb0 against our F+0xa4 and retail's body is twelve bytes
// shorter than ours; the dot's association and the declaration orders keep the score at 102.
// MiniMax G (11-06): the dot grouped explicitly 191, the same sum written with += 103, the tangent built by an
// aggregate initialiser 102 and the dot's three terms reordered 102; retail's association is
// (v.i*n.i + n.k*v.k) + (n.j*v.j) and the source spelling does not change it; the permuter tried no variant
// in 900 s; no lower score.
// MiniMax G (11-05): retail's sweep at F+0x44 against our F+0x38 leaves twelve bytes of frame that ours does not have (the cache at seventeen values is confirmed by the matched 0x2b8e10); the declaration orders give 102, the scope-hoisted forms 110 and the grown cache 125; reading the velocity in place gives 199, the dot product as three statements gives 103 and the displacement as a whole-struct copy gives 310, and retail's union at F+0xb0 against our F+0xa4 stays unmatched; no new form, no lower score.
// MiniMax D (11-09): no new form, no lower score; retail's sweep and collision block sit twelve
// bytes higher in the frame than ours, and the body is twelve bytes longer than retail's 1537.
// MiniMax H (11-13): no new form, no lower score (102); the frame is 0x124 in both, but retail's sweep sits
// at F+0x44 and its collision record at F+0xb0 where ours keeps F+0x38 and F+0xa4, so every home from
// there up and the twelve-byte length gap that follows differ: that is the frame's allocation order. The
// permuter ran over this body for 600 s (fifteen tries) and never beat its baseline of 93 differing
// instructions, so the statement order retail used is not what the search finds.
// MiniMax R (11-14), NOT kept (coordinator: the 102 -> 57 rests on the velocity-normal dot written out in full in each
// of the three perpendicular products; retail computes that dot once (0x2b93fb-0x2b941c) and scales the normal by it,
// so the repeated expression only steers the order; R's body with the dot computed once into a local measures 195.
// LEAD: the typed record, the velocity pointer and real_point3d_set are natural; find the once-computed dot form
// that keeps 57): 57 (was 102): the particle record is typed (identifier and flags words at +0 and
// +2, the reals at +8 and +0x34), the velocity is read through a pointer, the dot is written out in
// each of the three perpendicular products and the position is set through the existing real_point3d_set.
// The frame's homes now agree with retail's (the collision record at F+0xb0, the normal at F+0xd8, the
// impact at F+0x110). Left: the order of the loads at the head of the reflection block and the last
// argument's load, which retail takes after the three sums; the plain position stores instead of the
// setter score 120, so the setter is kept.
// R's form, not kept: score 57 (was 102): the frame's homes agree now; the reflection block's load order and the last
// argument's load still differ.
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
// Round 53 (lane 10, fast): the reflection block is 14 bytes longer than retail's (0x367..0x466 against
// 0x367..0x458); ours stores position.x before the normal's j/k reads, so it reloads them, where retail holds
// the normal in xmm1/4/2 and stores the three position sums together. Position into a local point copied out
// 286 (1594 bytes); the point copied from the collision and offset in place 335 (1618); the velocity pointer
// with the local point 232 (1560); the sums in three locals stored after 245 (1570), with the velocity pointer
// 189 (1536); the velocity pointer with real_point3d_set 188 (1536), with impact.index stored before it 188,
// with perpendicular/tangent interleaved per component 179 (1537 bytes, retail's length: normal held in
// registers, tangent.i/j spilled at +0x38/+0x3c as retail, but frame 0x118: ours keeps perp.k in xmm7 and
// spills tangent.k where retail spills perp.k at +0xac, because ours multiplies normal * dot, copying the
// normal, where retail multiplies dot * normal and reuses dot's register for perp.k); on that 179 form the
// dot or the perpendicular products in the other text order 179/179 (same code), the dot read from the
// particle fields 189, keep computed after the velocity pointer 188; the normal as a union member read
// directly 605 (h0, copy form), 651 (on the 179 form).
// Round 43 (lane 14, fast): collision record declared in the loop block 243 (1570 bytes), at the countdown block's
// top 243; velocity pointer 195; pointer plus loop-block collision 184 (1536 bytes, frame 0x118: tangent +0x38, sweep
// +0x44, cache +0x50, collision +0xa4 as retail, no perpendicular home, j/material/next slots rotated); that with
// retail's computation order 170 (1547), with rows i, i, j, j, k, k 179 (1537 bytes); on the last: velocity pointer
// before normal 191, keep before normal 179, both 190, normal before the coefficient loop 179, `normal->x * dot`
// 179, `keep * field` 179, keep at each use 120 (1525, keep recomputed after the call). Ours keeps the normal as the
// destination of every `dot * normal` product, so tangent.k spills where retail multiplies dot in place, spills perp.k
// (+0xac) and keeps tangent.k (that spill is what gives retail its perpendicular home and the 0x124 frame).
// Round 65 (lane 16, fast): velocity pointer with the position through the existing real_point3d_set (the three
// sums held until the stores, as retail) 189 (1536 bytes, frame 0x118; velocity read twice through `lea esi` as
// retail); final stores direct to the particle 188 (j/k stores then [ebp+0x2c]/[ebp+0x30] as retail); on that
// dot3f 188, impact first 188, tangent declared first 188, perpendicular through vector3f_scale 188; the
// perpendicular/tangent pairs interleaved per component 179 (1537 bytes; tangent.i/j spill to retail's +0x38/+0x3c
// and the sweep sits at retail's +0x44, but ours spills tangent.k where retail spills perp.k, so the perpendicular
// gets no home); on that dot3f/impact first/dot declared first/normal-first products 179 each, twelve dot term
// orders 179 each (identical code), pair orders ikj/jik/jki/kij/kji 181/183/191/183/189; the normal through a
// named header field 179 (identical), read as a direct field 651 (1749 bytes). Retail multiplies the perpendicular
// with the dot as destination (dot * n), ours copies the normal (n * dot). Retail's six-statement order (perp.i,
// tangent.i, perp.j, perp.k, tangent.j, tangent.k) on that pointer form 185 (1547 bytes; frame 0x124 and the
// collision at +0xb0 as retail, but perp.j spills to +0x3c, tangent.j to +0xa8 and perp.k to +0x40).
// Round 30 (lane 2, fast): retail loads every normal/point component and the placement's last argument
// above the position stores and reuses the dot's normal registers for the position; ours reloads the normal
// after each position store, so our particle stores may alias the collision record. The particle's +2 word read
// as a struct field (identifier/flags words) in place of the flags pointer 99 (the flags test after the call
// then hoists above the velocity stores as retail; three-point gain, not kept); on that body the impact declared
// at the hit block's top 99, the velocity pointer 196. Position stores before the dot 116 (frame 0x118); a
// position local stored after impact.index 286 (1594 bytes); a normal copy with real_point3d_set after
// impact.index 269; the normal as a union member 605.
// Round 126 (lane 5, fast; 0x2ba3e0 and 0x2b8da0 match throughout): perpendicular and tangent declared at
// function scope 102; the impact at function scope 102; the impact declared and its index stored right after
// the normal pointer 102 (all the same code); the sweep at function scope 110; the collision word stored
// before the start stores with perpendicular/tangent at function scope 100, with the early impact 100 (two
// points, not kept); the velocity pointer form re-measured 194 (1541 bytes), with the dot through a
// forceinline pointer helper 194 (identical: the helper's loads still merge, so the twice-read velocity.i of
// retail is not an inline-boundary effect).
// Round 194 (fast): the normal's three components read into named locals at the block's top with the
// velocity pointer and the position through real_point3d_set 179 (1537 bytes: retail's length, its dot
// product and its normal registers xmm1/xmm4/xmm2 held through the position; tangent.i/j spilled at +0x38/+0x3c
// as retail, but tangent.k spills where retail spills perp.k, so perpendicular gets no home and the frame
// is 0x118; perp = n * dot copies the normal where retail copies dot; the position stores go through
// the pointer, [ebx+4]/[ebx+8], where retail stores y/z through the particle); the same with the set
// nested as placement_set's argument 179; with plain position stores 223 (1541); with the velocity copy
// plain/set/nested 237/245/245; the velocity pointer with set or nested and no normal locals 188/188 (1536).
// On the 179 form: perpendicular/tangent per component 182, in retail's computation order 191, tangent
// declared first 179, tangent.k first 179, the final rows through the velocity pointer 180, the +0x34
// scale before the rows 187.
// Round 55 (lane 10, fast): the particle struct typed (flags word at +2, reals at +8 and +0x34, read as
// fields with no flags pointer) 99 (the flags test after the call then hoists as retail); that with the
// collision word stored before the start stores 97 (five points, not kept); on that typed body: the
// velocity pointer with real_point3d_set and perpendicular/tangent interleaved 195 (1539 bytes), in retail's
// computation order (perp.i, tangent.i, perp.j, perp.k, tangent.j, tangent.k) 182; the velocity pointer
// alone 194; the velocity read at each use 198; real_point3d_set alone 242 (1570 bytes). On the untyped
// body: the 179 form in retail's computation order 185 (1547 bytes, frame 0x124 again).
// Round 441: actual shared collision normal member, plus native flags/kind/fraction
// subsets 102/99/102/99; those four with the existing full response setter 191 each.
// Three-point reductions rejected. Original particle type, shared collision header and body
// restored; reflection spill ownership, sweep/collision homes and normal reloads remain.
// Round 310: Complete dot-product expression parts in all six orders 103 each; keep at the
// projection boundary 103; that with native velocity capture 219. Original expression, factor
// lifetime, velocity snapshot and complete body restored; sweep/collision homes and reflection
// spills remain.
// Round 254: Native complete collision record without byte-storage union; velocity pointer,
// complete position setter and paired projection/tangent combinations
// 102/209/245/224/194/195/188/179. Original union, shared result extent and full body restored;
// sweep/collision homes and projection spills remain.
// Round 75 (m3 lane 9, fast): MAIN LEAD, not kept because a value written out at each use needs a match: retail has no `dot` local. Writing the dot expression out in each of the three
// perpendicular products (it becomes a compiler temp, numbered after the normal's) copies dot and multiplies perp.k in place as retail, so perp.k spills, frame 0x124 and every home agree:
// with normal locals, the velocity pointer, velocity reads through it and the set: 64 (1537, aligned 12); with the particle's flags word and the reals at +8 and +0x34 as fields 47 (aligned 11);
// with the collision word stored before the start stores 44 (aligned 10); with named position sums, nested in the call 44 (aligned 7), plain stores 44 (aligned 8). Left at 44: retail loads v.i
// before n.k/n.i, loads the placement's last argument at the block's top and stores the position just before the call, and computes the x sum first. On that form: normal locals in six orders
// 44 (aligned 8-15), sums in six orders 44 (8-9), keep after velocity/perpendicular/tangent/sums/impact 44/61/44/200/222; last argument named `long` 118-131 (movsx), `short` at eight positions or
// widths 59-61 (the load and the position stores land as retail, but in EAX where retail has EDX, and eax/edx rotate in later blocks); normal through its pointer 57 (1538); normal as a direct
// field 39-44 (aligned 16-20, twelve written term orders); a one-use velocity.i local 162-170; last sum read in the store 44; plain position expressions 237 (1554), typed 238; by-value dot
// through vector3f_scale 187-198 (frame 0x118); dot3f written three times through the pointer 172 (1540); tangent after the sums 230; tangent reads through the particle 81.
// With a `dot` local: helper grid (dot text/dot3f, perpendicular text/vector3f_scale, tangent text/point difference, position text/set/add-scaled) 188-194; normal locals 179, typed 188; staged
// dot 186/185/179; `perp = dot; perp *= n` 187; `dot *= n.k` last 179; all perps set to dot then scaled 179; normal copied by fields 181; normal k,i,j 180; dot declared first 179; interleaved 182.
// AC475: Canonical repeated mode-two truth bool/byte/short/word, original sentinel
// 273/273/269/269; sentinel before start stores 271/271/269/269.
// Original raw mode mask, sentinel order, reflection inputs and complete body restored.
// Round 158 (m3 lane 4, fast): on round 75's form (dot written at each product, normal locals, velocity pointer, typed flags/+8/+0x34,
// collision word before the start stores): setter statement 50 (1537, aligned 18), nested 50; named sums with plain stores after the impact
// index 50 (y/z stores as retail, [ebp+0x20]); the first final row through the particle 44 (aligned 16, flags test hoists as retail). What is left is
// one block: v.i first, the last argument above the stores, x sum first. Named last argument: `short` 59 (aligned 42), at the impact index 59,
// through a collision pointer 59, `word` 59, dword view 59, `dword` from `word` 129 (1542), inline helper's by-value parameter 59: all land the load and
// the stores as retail but in EAX, where the unnamed read gets EDX, and every later block rotates one step. Unused named read 44; const reference 44.
// Position through vector3f_add_scaled before/after the impact 116 (1548), with the named last argument 155; assignments in a comma argument 236
// (1554: the stores still precede the load); normal direct fields 60/54, with the named argument 82/76; impact at the block's top 50/65; helper
// taking the collision point 156 (1547). Next: an unnamed read that sits ahead of the stores.
// P289: Uniform scaled-response tangent/normal/both products, row-local then staged,
// staged and row-local complete differences: 103/103/102/103/103/102/102/102. Original complete response
// restored; sweep/collision homes and cross-placement projection spills remain.
// Round 141 (m3 lane 6, fast): final rows from tangent and perpendicular each scaled in place through
// vector3f_scale 101 (not kept, one point); tangent alone scaled in place 102; tangent scaled in place
// right after it is formed 202 (1555 bytes); displacement/sweep/start and the collision call in their own
// block with a bool result 102 (homes unmoved); `vector3f velocity = particle->velocity;` 223 (1556
// bytes), with both in-place scales 223. Original body restored.
// P292: Actual owning 16-bit flag record with contact/floor/stop bitfield subsets
// 1/2/4/3/5/6/7, then all with native three-bit countdown: 99 each, 1549/1537 bytes.
// NOT kept: only three points gained; all ten other same-file users unchanged.
// Original full type layout, raw flag observations, projection and complete body restored;
// sweep/collision homes and cross-placement tangent/perpendicular spills remain.
// Lane 17 r42 (fast; all match-only, body restored): round 75's form rebuilt (dot written at each product, normal
// locals, velocity pointer, typed flags/+8/+0x34, collision word before the start stores, position setter) with the
// j/k final rows through the pointer 60 (aligned 24), all three final rows through the particle 44 (aligned 18); named
// sums nested in the call 44 (aligned 14); a named `short` last argument read before the sums, named `surface_index` or
// `a_value` 61 each (1535 bytes: the load moves to the block's top but in EAX whatever the name, so same-size name order
// does not pick its register); that read with plain position stores after the impact index 234 (frame 0x12c).
// Round 149 (m3 lane 6, fast; all match-only, body restored; aligned, masked = registers and stack offsets ignored): round 75's form rebuilt (dot written at each product, normal locals,
// velocity pointer, typed flags/+8/+0x34, the three final rows through the particle). Named sums nested in the setter, with the collision word before the sweep: 84 (1537 bytes, aligned 18, masked 17), plain stores 84
// (20, 15); the word left before the call 47 (16, 15), unnamed sums 47 (20, 15), normal locals k/i/j 47 (24, 17), unnamed sums with the early word 84 (22, 17); the word right before the start
// stores (retail's slot, among the pushes) 44 (14, 13). A `short` last argument read after the sums and nested 61 (1535, 42, 11); plain stores 59 (1535, 42, masked 9), read before the sums 59, at the impact
// index 59: load above the stores but in EAX, later blocks rotate. Direct position stores (`position.x = ...` in place of named sums) 236 (1554, frame 0x12c), with the read before them 234 (1552), after them 236;
// read first with each sum stored after its statement 234. At 44 only one block is left: v.i load first, retail's `mov edx, [esp + 0xd4]` above the position stores (ours after them), the x sum first, `lea ebx` among the
// sums. Lead, BLOCKED-BY 0x1697c0 (its file is another lane's this round): ours keeps that load behind the stores through the particle, so check what the record pointer does in 0x1697c0's source for the caller's alias
// summary; 0x1697c0 itself is at 25, first +0x314. The 44 form leans on the dot written out three times, so it is match-only.
// M380: Displacement/sweep scalar, aggregate and immutable aggregate producers,
// all eight non-original pairs: 129 each at 1549 bytes. NOT kept: all regress;
// original interleaved travel products, reflection, collision extent and full body restored.
// M3 lane 4 r164: LEAD for 0x247950: retail builds placement_set's last argument in its stack slot there (`push ecx; mov eax, esp;
// mov word ptr [eax], 0`), so it is a by-value word-sized class with a constructor. The class (2 or 4 bytes) leaves this body at 102
// (same code) and takes 0x247950 from 356 to 351, 1643 to 1653 B against retail's 1652 (not kept: five points without a match).
// The position or the value declared last 102, but 0x2ba3e0 0 -> 6. Round 75's form rebuilt: normal locals in six orders 44 (aligned
// 8 for i,j,k; j,i,k; j,k,i; 15 for the others). On it: the class read into a local at the top, keep, velocity or perpendicular 208
// (1545 B, the class gets a stack home); a named short 59 (aligned 27: load and stores land as retail, EAX for EDX, later blocks
// rotate); the position set nested as the argument with named sums 44 (aligned 7), written out 44 (aligned 9), assignments in a
// comma argument 44 (aligned 8): the stores still precede the load; a plain collision record for the union with the class member or a
// short member 44 (aligned 8); the normal as direct record fields 44 (aligned 15), with no normal pointer 647 (1745 B).
// M380: Displacement/sweep scalar, aggregate and immutable aggregate producers,
// all eight non-original pairs: 129 each at 1549 bytes. NOT kept: all regress;
// original interleaved travel products, reflection, collision extent and full body restored.
// M3 lane 12 r91 (fast; masked with registers and stack offsets): retail's particle stores do not block reads of the collision record. With 0x1697c0's capture
// of the record cut (diagnostic, wrong values: no record to 0x244980 and none through 0x169510's recursion; either alone changes nothing) our build pushes the
// last argument first and stores the position last at retail's frame offsets, but entry registers swap: 380 (1540), velocity pointer 363 (1519, frame 0x118), round
// 75's form 384 (1527). Round 75's form rebuilt 44, masked 13 of 401, every register as retail. A nested setter or comma stores still run before a plain last
// argument; an inline getter as the last argument with the setter nested evaluates right to left, so the load lands first and the push precedes the stores as
// retail: 59 (1535, masked 11), with a setter taking the particle, so y and z store through it as retail, 59 (masked 9). Left there: EAX for retail's EDX, each
// later block rotating one step and a tail cross-jump that rotation undoes (the two missing bytes), the v.i load's place and the x sum first. Getter with a named
// value, by reference, through one or two pointer locals 59/59/59/59; a placement or record pointer local 59/59; sums in six orders with normal locals i,j,k or j,k,i
// 59 each, k,i,j 66; an inline reset of the impact 59, inside the first argument 63 (masked 11); comma stores with named sums and the getter 44 (masked 11, stores
// first), without named sums 236 (1554). By-value class for the last parameter: direct read with the setter 44 (masked 11, stores first, EDX), a getter returning it
// 212 (1549). LEAD for 0x247950: that class with a converting constructor takes it 356 -> 337 at retail's 1652 bytes (its file was held; not kept).
// Round 159 (m3 lane 6, fast; all match-only, body restored; 0x2ba3e0 and 0x2b8da0 match throughout; aligned counts from an
// instruction diff): round 75's form rebuilt with the position setter 44 (1537 bytes, aligned 18); with named sums and plain
// position stores after the impact index 44 (aligned 16: left are v.i loaded first, the last argument's load above the stores
// and the x sum first); on that: the last argument built by the value type's converting constructor from the record's word 44
// (aligned 16, same code); velocity.i named once ahead of the normal locals 170 (1542 bytes, aligned 33); the velocity pointer
// and keep declared ahead of the normal locals 41 (aligned 31). Retail hoists the record's word above the particle stores,
// which ours cannot while the record escapes: BLOCKED-BY 0x1697c0 (unmatched) and 0x169510 (contributor).
// Round 107 (m3 lane 9, fast; the file's other scores unchanged throughout): the particle record typed (identifier
// and flags words, reals at +8 and +0x34, no flags pointer) re-measured 99; with the final rows scaled in place
// through vector3f_scale 103; with dot3f 99; the perpendicular as a copy of the normal scaled by the in-file
// scale_particle_vector_2b 265 (1591 B); the final rows through scale_particle_vector_2b 101 (one point, not kept);
// both 264; the typed record with the perpendicular helper 265, with both helpers 264, with the rows helper 103; only
// the tangent through the helper 102, only the perpendicular 198 (1552 B). 0x1697c0's own 25 are dot-product operand
// orders and its result byte, not the record capture.
// MiniMax A (11-12, run 63): no new form, no lower score (102); the prologue and the loop match instruction for
// instruction, but retail's locals sit twelve bytes higher and its body is twelve bytes shorter, so its epilogue
// starts at +0x5f4 where ours starts at +0x600 and the loop's exit jump differs.
// MiniMax A (11-12, run 62): no new form, no lower score (102); the frame is 0x124 in both, but retail's sweep
// vector sits at +0x44 and its collision record at +0xb0 where ours keeps the sweep at +0x38 and the record at
// +0xa4, so every home from there up (and the twelve-byte length gap that follows) differs; that is the frame's
// allocation order, which the rules bar me from reshaping.
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
                    /* retail copies the collision's value whole (`mov edx, [esp + x]; push edx`), so it is this class there */
                    placement_set((s_placement *)&impact, &particle->velocity, &particle->position, normal, *(c_placement_value const *)&collision.value.unknown24);
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
