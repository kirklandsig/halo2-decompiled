// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_1F03E0.CPP: the state of a moving physics shape (0x74 bytes) and
   the side of a contact it touches */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_0259d0.h"
#include "normalize3d.h"
#include "vector_products.h"
#include "data_array.h"
#include <math.h>
#include "object_list.h"
#include "unknown_1efac0.h"

struct s_table;
struct s_iterator
{
    s_table *table;
    signed char bytes[16];
    union
    {
        dword position;
        struct { byte first, item, sub, group; };
    };
    void *current;
    bool advance();
};

struct s_mix_output;
struct s_mix_source;
struct s_bsp3d;
struct s_1de2c1
{
    long field_0;
    long field_4[256];
};
struct s_1de2c2
{
    s_1de2c1 field_0;
    s_1de2c1 field_404;
    s_1de2c1 field_808;
    s_1de2c1 field_c0c;
};
struct s_245aa0;
struct s_source_245400;
struct s_collection_245270;
void function_1efbd0(dword id, s_mix_output *output, s_mix_source *source, signed char *indices);
void function_141590(transform4x3f const *in, transform4x3f *out);
bool function_1dde10(s_bsp3d const *bsp, short count, dword const *mask,
    point3f const *point, real radius, s_1de2c2 *hits);
void function_245aa0(s_245aa0 const *hits, s_source_245400 const *source,
    transform4x3f const *matrix, real height, real thickness,
    long object_index, long position, s_collection_245270 *collection);

// @retail 0x1f0230
bool __stdcall function_1f0230(s_lookup *lookup, point3f const *point, real radius,
    real height, real thickness, s_collection_245270 *collection)
{
    bool result = false;
    s_iterator iterator;
    function_1efbd0((dword)lookup->tag_b, (s_mix_output *)&iterator,
        (s_mix_source *)lookup->tag_a, (signed char *)lookup->pointer_a);
    while (iterator.advance())
    {
        dword position = iterator.position;
        transform4x3f const *matrix = (transform4x3f const *)lookup->pointer_b + (position & 0xff);
        transform4x3f inverse;
        function_141590(matrix, &inverse);
        real scale = inverse.scale;
        real scaled_x = point->x;
        real scaled_y = point->y;
        real scaled_z = point->z;
        if (scale != 1.0f)
        {
            scaled_x *= scale;
            scaled_y *= scale;
            scaled_z *= scale;
        }
        point3f local;
        local.x = inverse.up.i * scaled_z + inverse.left.i * scaled_y + inverse.forward.i * scaled_x + inverse.position.x;
        local.y = inverse.up.j * scaled_z + inverse.left.j * scaled_y + inverse.forward.j * scaled_x + inverse.position.y;
        local.z = inverse.up.k * scaled_z + inverse.left.k * scaled_y + inverse.forward.k * scaled_x + inverse.position.z;
        s_1de2c2 hits;
        if (function_1dde10((s_bsp3d *)iterator.current, 0, NULL, &local, scale * radius, &hits))
        {
            function_245aa0((s_245aa0 *)&hits, (s_source_245400 *)iterator.current,
                matrix, height, thickness, lookup->handle, position, collection);
            result = true;
        }
    }
    return result;
}

#define PIN(x, lower, upper) ((x) < (lower) ? (lower) : ((x) > (upper) ? (upper) : (x)))

struct s_shape_state
{
	point3f point;
	long unknown0c;
	byte unknown10[0x1c - 0x10];
	long unknown1c;
	long unknown20;
	transform4x3f matrix;
	long unknown58;
	long unknown5c;
	short material;
	bool unknown62;
	byte unknown63;
	vector3f normal;
	real unknown70;
};

transform4x3f *g_4687d0;
extern short g_47d8e0;
extern short g_54e898;

// @retail 0x1f03e0
void function_1f03e0(s_shape_state *state)
{
	*(volatile long *)&state->unknown0c = NONE;
	state->point = *(point3f *)g_4687a4;
	*(volatile bool *)&state->unknown62 = false;
	state->normal = *g_4687b0;
	*(volatile short *)&state->material = g_47d8e0;
	*(volatile long *)&state->unknown58 = NONE;
	*(volatile long *)&state->unknown5c = NONE;
	*(volatile long *)&state->unknown1c = NONE;
	*(volatile long *)&state->unknown20 = NONE;
	*(volatile real *)&state->unknown70 = 0.0f;
	state->matrix = *g_4687d0;
}

/* the contact at a surface and the side it faces */
struct s_shape_contact
{
	byte unknown00[0xdc];
	vector3f normal;
};

struct s_shape_side
{
	long unknown0;
	long side;
};

// @retail 0x1f1930
void function_1f1930(s_shape_side *side, s_shape_contact const *contact)
{
	if (contact->normal.i != 0.0f || contact->normal.j != 0.0f || contact->normal.k != 0.0f)
	{
		if (fabs(contact->normal.i) < fabs(contact->normal.j))
		{
			if (contact->normal.j < 0.0f)
				side->side = 2;
			else
				side->side = 3;
		}
		else
		{
			if (contact->normal.i < 0.0f)
				side->side = 5;
			else
				side->side = 4;
		}
	}
}

/* the havok components (g_51e9b8, 0xa0 bytes) and their materials */
struct s_component_material
{
	byte unknown00[0x12];
	short material;
	byte unknown14[0x48 - 0x14];
};

struct s_component
{
	byte unknown00[0x88];
	s_component_material *materials;
	byte unknown8c[0xa0 - 0x8c];
};

// @retail 0x1f1df0
void function_1f1df0(long component_index, long material_index, vector3f const *normal, s_shape_state *state)
{
	union { short field_0; word field_2; } local_0;
	local_0.field_0 = 0;
	s_component *component = (s_component *)(g_51e9b8->data + (component_index & 0xffff) * sizeof(s_component));

	state->normal = *normal;
	if (material_index != NONE)
		local_0.field_0 = component->materials[material_index].material;
	if (material_index == NONE)
		state->material = g_54e898;
	else
		state->material = (short)local_0.field_2;
}

struct s_shape_carrier_contact
{
	byte unknown00[0x10];
	long object_index;
	byte unknown14[0x58 - 0x14];
	transform4x3f transform;
	byte unknown8c[0xe8 - 0x8c];
	point3f point;
};

struct s_shape_carrier_state
{
	vector3f previous_velocity;
	byte unknown0c[0x24 - 0xc];
	transform4x3f matrix;
};

bool function_182020(long component_index, vector3f *previous_velocity, point3f const *point,
	transform4x3f const *transform, transform4x3f *matrix, vector3f *velocity,
	vector3f *delta_velocity, matrix3x3 *rotation);

// @retail 0x1f1e50
bool function_1f1e50(s_shape_carrier_state *state, s_shape_carrier_contact const *contact,
	vector3f *velocity, vector3f *delta_velocity, matrix3x3 *rotation)
{
	bool result = false;
	long object_index = contact->object_index;
	if (object_index != NONE)
	{
		s_object_list_state *objects = g_5107f0;
		for (long i = 0; i < objects->object_count; i++)
		{
			if (objects->object_indices[i] == object_index)
			{
				struct s_carrier_header { byte unknown00[8]; byte *object; };
				byte *object = ((s_carrier_header *)g_4e0300->data)[object_index & 0xffff].object;
				result = function_182020(*(long *)(object + 0xb4), &state->previous_velocity,
					&contact->point, &contact->transform, &state->matrix, velocity, delta_velocity, rotation);
				break;
			}
		}
	}
	return result;
}

/* the surfaces' minimum and maximum heights: whether the shape stands at a
   height it can step to */
struct s_shape_ground
{
	byte unknown00[0x1c];
	point3f point;
	byte unknown28[0x34 - 0x28];
	real height;
};

// @retail 0x1f2e60
bool function_1f2e60(s_shape_ground const *ground, bool moving, vector3f const *velocity, point3f const *base, bool stepping, real height)
{
	point3f const *const *base_reference = &base;
	real base_height = (*base_reference)->z;
	real top = base_height + height;

	if (top - 0.001f > base_height)
	{
		point3f point = ground->point;
		point3f center;
		vector3f v;
		real distance;

		if (moving)
		{
			real rate = g_510c54->rate;
			point.x = velocity->i * rate + point.x;
			point.y = velocity->j * rate + point.y;
			point.z = velocity->k * rate + point.z;
		}
		center.x = (*base_reference)->x;
		center.y = (*base_reference)->y;
		center.z = height + (*base_reference)->z;
		vector3d_from_points3d(&center, &point, &v);
		distance = (real)sqrt(v.i * v.i + v.j * v.j + v.k * v.k) - height;
		if (PIN(distance, moving ? -0.25f : -0.1f, stepping ? 0.1f : 0.2f) == distance)
			return true;
		return false;
	}

	if (PIN(ground->height, -0.1f, 0.0328f) == ground->height)
		return true;
	return false;
}

struct s_havok_component;
void havok_component_rigid_body_point_velocity_get(long rigid_body_index, s_havok_component *component, point3f const *point, vector3f *velocity);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
real function_f5e00(long vehicle_index);
// @retail 0x1f1ee0
bool function_1f1ee0(s_shape_carrier_contact const *contact, s_shape_state *state,
    vector3f *velocity, matrix3x3 *rotation)
{
    bool result = false;
    byte const *data = (byte const *)contact;
    byte *components = g_51e9b8->data;
    byte *component = components + (*(long *)(data + 0x18) & 0xffff) * 0xa0;
    real top = contact->point.z + *(real *)(*(byte **)(data + 8) + 0xc);
    long count = *(long *)(component + 0x8c);
    long best = NONE;
    real best_dot = -3.402823466e38f;
    for (long i = 0; i < count; i++)
    {
        byte *entry = *(byte **)(component + 0x88) + i * 0x48;
        if (top - 0.001f > *(real *)(entry + 0x24) &&
            function_1f2e60((s_shape_ground *)entry, false, NULL, (point3f const *)(data + 0xe8),
                ((*(dword *)(data + 0x14) >> 2) & 1) != 0, *(real *)(*(byte **)(data + 8) + 0xc)))
        {
            vector3f const *up = (vector3f const *)(data + 0x8c);
            vector3f *normal = (vector3f *)(entry + 0x28);
            real dot = normal->k * up->k + normal->i * up->i + up->j * normal->j;
            if (dot > 0.8660253882408142f && dot > best_dot)
            {
                best_dot = dot;
                best = i;
            }
        }
    }
    if (best != NONE)
    {
        byte *entry = *(byte **)(component + 0x88) + best * 0x48;
        long component_index = *(long *)(entry + 0x14);
        if (component_index != NONE)
        {
            s_havok_component *other = (s_havok_component *)(components + (component_index & 0xffff) * 0xa0);
            transform4x3f matrix;
            other->rigid_body_matrix_get(*(signed char *)(entry + 0x46), &matrix);
            if (component_index == *(long *)(data + 0x54))
            {
                long object_index = *(long *)(entry + 0x18);
                struct s_contact_object_header { byte unknown0[3]; byte type; byte unknown4[8]; };
                s_contact_object_header *headers = (s_contact_object_header *)g_4e0300->data;
                if (object_index == NONE || !( (1 << headers[object_index & 0xffff].type) & 2) || function_f5e00(object_index) != 1.0f)
                {
                    transform4x3f inverse;
                    transform4x3f relative;
                    function_141590(&matrix, &inverse);
                    function_142a60(&contact->transform, &inverse, &relative);
                    havok_component_rigid_body_point_velocity_get(*(signed char *)(entry + 0x46), other, &contact->point, velocity);
                    function_141590(&relative, &relative);
                    *rotation = relative.rotation;
                    result = true;
                }
            }
            state->matrix = matrix;
            state->unknown1c = component_index;
            state->unknown5c = *(long *)(entry + 0x18);
            state->unknown20 = *(signed char *)(entry + 0x46);
        }
    }
    return result;
}

real function_30bf0(vector3f *v);
real function_1201a0(vector3f *v, vector3f const *fallback);

PRIVATE __forceinline void function_1f087b(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2);

PRIVATE inline void contact_cross(vector3f const *a, vector3f const *b, vector3f *out)
{
    out->i = a->j * b->k - a->k * b->j;
    out->j = a->k * b->i - a->i * b->k;
    out->k = a->i * b->j - a->j * b->i;
}

// score 122 (was 122): frame 0x14 and scan prefix agree; first +0xd5 is a displaced case span; cross coloring and selected-tail sharing remain.
// Earlier: the loop now agrees (0x1f2e60's base parameter before its stepping flag, so the
// request dies at the base argument as in retail); left: `lowest` is stored after `selected` (retail first),
// the first cross keeps i/j/k in XMM0/1/2 (retail XMM1/2/0) so the two selected arms are not tail-merged
// (28 bytes), and the averaging path's copies, sum order and dot order (round 112: dot_average and the
// compare are now retail's; left there are average.i's operands, dot_a's i, k, j term order and the copies).
// Round 112: the two dots through the existing dot3f 124 (aligned 113 -> 106: dot_average's terms come out
// i, k, j as retail); plus the average summed i, j, k and `dot_a < dot_average` 124 (aligned 95: dot_a then
// first and dot_average/compare exactly retail's; dot_a's terms stay j, i, k); plus the normalize tail as the
// existing function_1201a0 call 124 (same code as the written-out tail); plus `lowest` declared before
// `selected` 122 (kept: aligned 93, first difference at +0xd5); dot_a as dot3f(desired, a) 122 (same code;
// kept, so both dots take desired first).
// Round 106: 0x1f2e60's (base, stepping) order at its definition and all four calls 122 (kept); on it,
// `lowest` declared before `selected` 120 (prefix exact through +0xd5; two points, not kept); `*out = *a` /
// `*out = average` in the two arms with no `support` join 197 (both copies then exactly retail's; 1005 bytes),
// with the average summed i, j, k 197 (aligned 63), and with `dot_a < dot_average` 197 (aligned 58: retail's
// compare); the height argument spelled as at 0x1f2250 (same code); contact_cross's j term as b.i * a.k (same code).
// Earlier: Frame 0x14 agrees; request/count/table registers and cross publication still differ.
// Earlier: frame 0x14 agrees; request/count registers and cross scheduling differ.
// Word component handle 207; unsigned selection 203; final scale helper 203;
// reversed independent dot order 203; stepping snapshot 303.
// frame 0x14 agrees; request/count/table registers and cross-result stores still differ.
// Shape pointer 329 (not matched); read-only scan and natural while 203/203; desired copy 271;
// double height comparison 309; size policy 374; floating consistency 314; native default copies 203.
// frame 0x14 agrees; request/count/table homes and cross stores remain.
// Cases 3/1/2 226; second normal first 246; average ikj/jik/jki/kij/kji
// 201/200/198/203/203; native masked handle 203. Average j/k/i kept after full check.
// frame agrees; request/count/table registers and cross-result stores remain;
// selected normal from existing pair 233; direct pair cross inputs 209; first normal reference 203;
// second normal reference 203; both normal references 203; output record reference 226; lower
// height bound first 211; complete average initializer 203.
// Native first/second/both normal values 261/293/334; average initialized from second/first
// normal 285/288; first-value/second-average 315; second-value/first-average 301; both-
// values/second-average 341.
// Separate projection results both/first/second 297/288/288; shared projection result 273;
// common complete pair result 181 at 975 bytes (plain confirmed); swapped dot comparison 198;
// negated average-default arm 198; common pair result with swapped comparison 181.
// First-normal i/k/j product and partial sum 198 each; average i/k/j product and partial sum 198 each, all 983/977 bytes. Original projection expressions and pair-result join restored.
// Fallback pointer/value 244/266; selected contact index 276, with pointer/value 309/299;
// per-case defaults 269; complete normalized result 239, with fallback pointer 310.
// Height-expression and full eligibility values 215/215; nested height filter 181; component fields at uses 370, with height/eligibility/nesting 368/368/370; signed valid-selection range 209. Original pair-result join restored.
// Round 159: Shared cross, native component, native contact, combined native layout and their three
// shared-cross combinations 181 each; native layout/shared cross with selected/lowest initialized first
// 177; original layout/private cross with both results initialized first 177 (975/977 bytes, plain
// confirmed and kept). Only the two constant initializations move; every memory read and call stays in
// place.
// Round 163: Direct averaged support, shared branch direction, both, entry support,
// direct+entry, shared+entry, all, and all with shared cross 235/194/235/177/235/194/235/235.
// Original pair-result join, two entry constants and private products restored.
// Round 166: Corresponding projection products named on both compared dots: i/k/j
// 194/194/194; ik/ij/kj 197/197/197; all 194; all with both first sums 194. Original
// expressions, early constants and pair-result join restored.
// Round 218: Ordered upper/lower/predicate admission continuations, every nonempty
// combination 177 each; all continuations with immutable contact count 194. Restored.
// Round 152: Eight pair-input/result and single-contact library-copy forms 177 each; selected-before-height publication 177; average-only result join 176, with selected-first 176 (983 bytes). One-point joins rejected; original complete pair result and observations restored.
// Round 220: Current pair workspace names, all-case/entry declarations and their combined names 177 each.
// Existing input-preserving cross helper at first/selected-zero/selected-other/both-first-and-zero/
// first-and-other/both-selected/all sites 179/177/177/179/179/177/179. All with direct selected outputs
// 144; only selected helpers with direct outputs 144 (989/977 bytes, plain confirmed and kept).
// First-only/zero-only/other-only helper with direct outputs 178/178/175; averaging-only support
// declaration 144; both average values directly published 221. Original average result join retained.
// Round 202: Paired projection multiplication operand orders i/k/ik/j/ij/kj/all, then all with average-dot-first evaluation 144/144/144/144/144/144/144/144 fast.
// Original executable source restored. Frame 0x14 agrees; request/count homes, initial cross and selected cross/projection scheduling remain.
// Round 257: On the direct snapshot-safe projection graph, one conditional-input shared call
// 228; signed-int count 144; read-only component 144; call/count, call/receiver, count/receiver
// and all three 228/228/144/228; signed-int scan/selection ordinals 144. Original body restored.
// Round 150 (fast): retail tail-merges the two second-cross arms (j/k stores from XMM4/XMM3 and the
// g_4687b0 reload after the i store are shared), which ours cannot while the first cross keeps i/j/k in
// XMM0/XMM1/XMM2 (retail XMM1/XMM2/XMM0). The cross's j term spelled b.i * a.k 144 (same code here and in
// 0x1f1460); average in i/j/k order (retail's store order) 144 (aligned 131 -> 128); average through
// shape_add 238; lowest declared before selected 142; axis declared at case scope 144. 0x1f2250's call
// sites load the stepping flag before the base point in our build too, so the loop's ECX/EBX request/count
// swap is not 0x1f2e60's parameter order.
// DL179: Complete parallel projection products, paired first sums, normalization squares,
// products+squares, sums+squares, joint products, joint products+squares and that graph
// with the input-preserving first cross 146 each (991/977); original direct-output body restored.
// Round 297: Existing staged cross at first/selected-zero/selected-other/first+zero/
// first+other/both selected/all sites 144/182/144/182/144/218/218; all with
// averaging-only support scope 218. Original helpers and direct-output body restored.
// Round 195: Selected two-/three-contact normal values and both 213/300/336;
// existing three-contact value scale/add alone/with either/both values 365/377/384/423;
// complete three-contact computed result 263. All original observations, helpers and body
// restored; request/count homes and selected-cross tail sharing remain.
// Round 174: the tail as a call to 0x1201a0 (normalize with fallback; retail inlines it here, and so
// does ours) 124 (tail bytes unchanged; retail's j term is `movaps xmm1, xmm0; mulss xmm1, [ebx+4]`,
// ours loads j whichever way the product is written). Retail's selected arms compute k, j, i and
// store i, j, k with the j/k stores shared; the first cross gets retail's XMM1/XMM2/XMM0 only with
// direct-store arms. Same helper in both arms, normal pointer before/after the first cross or written
// at each call, direct output or a per-arm local copied to out: contact_cross 154/176/327, local
// copy 246/251/369; cross3f_staged 196/169/324, local copy 246/251/359 (staged arms share their
// products before the branch, 947 bytes); 0x1f087b with normal after the cross or at each call
// 176/325, local copy 246/251/359; the first cross through cross3f_staged with contact_cross or
// cross3f_staged arms gives the same counts (154/176/327/246/251/359/196/169). Declarations
// lowest/selected/component 120 (two points, not kept); component first 122; component, lowest,
// selected 120; `support` before `a`/`b` 122; with the 0x1201a0 tail lowest first 122, component first 124.
// Round 94: the arms' missing tail merge (28 bytes) shifts every later line, so later fixes do not show
// in the count: the average summed i, j, k with `if (dot_a < dot_average)` gives retail's average stores
// and dot_average and computes dot_a first (122, same count; dot_a's terms stay j, i, k whatever their
// written order, and the i row's operands swap); plus `*out = *a` / `*out = average` copies 199 (1005
// bytes; that path is then byte-identical to retail); plus contact_cross arms 177 (999 bytes); plus
// lowest before selected 120; 0x1f087b storing k, j, i 153 (retail's first-cross registers, arms reordered).
// Round 48 (lane 9): in a compile-only screen all 64 operand orders of the first cross's six products give the
// same XMM0/1/2 colouring; retail's XMM1/2/0 appears only when the selected arms write a stack vector (`support`,
// then `*out = support`) or the normal is copied by value. Built: one `entries` base for a/b/normal 212; averaging
// arm first 185; `selected != 0` arm first 168; normal pointer after the first cross 176; loop `z` as a value 263;
// normal after the cross with swapped contact_cross arms 170; contact_cross as a function-like macro 124; 0x1f087b
// as a macro 124. Built with the selected arms writing `support` (contact_cross, 0x1f087b or cross3f_staged):
// `*out = support` in the arm 177/177/177, one `*out = support` after both arms 155/155/155 (aligned 111 -> 100).
// Round 102 (m3 lane 8; one-link LTCG harness, exact once 0x1201a0's object is linked): 23 forms of 0x1f087b's product
// order (i/j/k, k/j/i, k/i/j, j/k/i) x store order (i/j/k, k/j/i, i/k/j) x arm order never give retail's first-cross
// XMM1/XMM2/XMM0. Fast: `selected != 0` arm first 164 (975 bytes); helper computing k, i, j and storing i, k, j 164 (975
// bytes); storing k, j, i 122 (same count); computing and storing k, j, i 165 (975 bytes); both arms into `support` with
// one `*out = support` 190 (1000 bytes), with the averaging arm's direct copies too 1031 bytes (harness).
// Round 28 (lane 14), `--fast`: average.i as `a->i + b->i` 122 (same code); the averaging arm's two
// direct copies 199 (1007 bytes, aligned 93 -> 75: that arm then exactly retail's), with average.i
// swapped 199; the first cross written in both selected arms 316 (1026 bytes), with the two copies 337;
// axis declared before the normal pointer 122 (same code); cross3f_staged in the selected arms 165 (975
// bytes), with the two copies 178 (aligned 75), with the first cross staged too 178; the average through
// vector3f_add(b, a) 170 (972 bytes: shared arm products hoisted above the branch), with the two copies 177
// (aligned 70); the selected arms writing a `support` local copied to out once inside the selected
// branch 190 (1000 bytes), with the two copies 251 (also with `support` local to that branch or through
// cross3f_staged, 251 each).
// Round 120 (m3 lane 5): our two selected arms already store j/k from the same registers; what stops
// the tail merge is where the g_4687b0 reload lands (ours right after the normal pointer's last use,
// retail after the out->i store). `b` declared before `a` 228 (1002 bytes); 0x1f087b as plain
// `inline` 122 (same code; 0x1f0870 590); 0x1f087b storing each component directly i, j, k 121
// (0x1f0870 591 at 3047 bytes; each arm then computes and stores in turn), directly k, j, i 166
// (975 bytes); the component pointer without the index local 172 (972 bytes). Original body restored.
// J218: Existing staged-cross output setter/three-contact output-pointer scaled-add/native scratch values, seven subsets, then scratch value assignment: 122/122/122/199/199/199/199/199. Pointer-result staged cross, forced/ordinary inline, exact-only: 122/122. Original complete body restored.
// @retail 0x1f2a80
void function_1f2a80(byte const *request, bool moving,
    vector3f const *desired, long const *indices, long count, vector3f *out, vector3f const *velocity)
{
    real lowest = 0.0f;
    long selected = NONE;
    long component_index = *(long *)(request + 0x40) & 0xffff;
    byte *component = g_51e9b8->data + component_index * 0xa0;
    for (long i = 0; i < count; i++)
    {
        byte *entry = *(byte **)(component + 0x88) + indices[i] * 0x48;
        real const &z = *(real *)(entry + 0x30);
        if (lowest > z && z > -0.999f && function_1f2e60((s_shape_ground *)entry, moving,
            velocity, (point3f const *)(request + 0x1c),
            ((*(dword *)(request + 0x18) >> 1) & 1) != 0, *(real *)(*(byte *const *)request + 0xc)))
        {
            lowest = z;
            selected = i;
        }
    }
    *out = *g_4687b0;
    switch (count)
    {
    case 1:
        *out = *(vector3f *)(*(byte **)(component + 0x88) + indices[0] * 0x48 + 0x28);
        break;
    case 2:
        {
            vector3f const *a = (vector3f *)(*(byte **)(component + 0x88) + indices[0] * 0x48 + 0x28);
            vector3f const *b = (vector3f *)(*(byte **)(component + 0x88) + indices[1] * 0x48 + 0x28);
            vector3f support;
            if (selected != NONE)
            {
                vector3f const *normal = (vector3f *)(*(byte **)(component + 0x88) + indices[selected] * 0x48 + 0x28);
                vector3f axis;
                contact_cross(a, b, &axis);
                if (selected == 0)
                    function_1f087b(&axis, normal, out);
                else
                    function_1f087b(normal, &axis, out);
            }
            else
            {
                vector3f average;
                average.i = b->i + a->i;
                average.j = b->j + a->j;
                average.k = b->k + a->k;
                function_30bf0(&average);
                real dot_a = dot3f(desired, a);
                real dot_average = dot3f(desired, &average);
                if (dot_a < dot_average)
                    support = *a;
                else
                    support = average;
                *out = support;
            }
        }
        break;
    case 3:
        if (selected != NONE)
        {
            vector3f const *normal = (vector3f *)(*(byte **)(component + 0x88) + indices[selected] * 0x48 + 0x28);
            real scale = -normal->k;
            out->i = normal->i * scale + g_4687b0->i;
            out->j = normal->j * scale + g_4687b0->j;
            out->k = scale * normal->k + g_4687b0->k;
        }
        else
            *out = *g_4687b0;
        break;
    }
    function_1201a0(out, g_4687b0);
}

PRIVATE __forceinline byte function_1f1462(dword arg_0)
{
	return (arg_0 >> 3) & 1;
}

// score 182 (was 182): Frame 0x28 and retained EDI/EBX convention agree; initial dot displacement bytes and rotation SIMD scheduling remain.
// Earlier: retail keeps the state in EDI and the two result pointers in EBP and EBX and never saves EBX, ours keeps the state in EBX and the results in EDI and EBP, so the prologue and every stack-relative access differ; a named pointer to the state+0x100 vector reproduces the length (1229) and the retail addressing but moves the state to EBX (196) and was dropped.
// frame 0x28 and state/output registers agree; the initial state-dot
// displacement bytes (retail reads the old up through the ESI address it keeps, ours folds
// [EDI+0x108]/[EDI+0x104]: the six extra bytes) and the rotation SSE schedule remain.
// Tried: the state dot through dot3f 262 / through a branch-local pointer 206 (both break caller
// 0x1e5af0); the rotation dot through shape_dot 182 (same code), through dot3f 248; the plain mode
// bit test for the one-caller bit helper 299 (breaks 0x1e5af0); the file's contact_cross for the
// duplicate cross helper 182 (same code; kept, the duplicate removed); repeated up-field reads 182;
// native old-up 325; function-scope workspaces 195; vector names 195; shared dot helper 248; plain
// parameters 182; branch-local up pointer 195; cross before trig 247; late limited snapshot 190;
// cosine first 183; shared cosine scale 241; native settings mask 303; initial-phase up pointer 309;
// between-trig snapshot 190; early alignment 192; rotation-only snapshot crosses 182; native j/k
// rotation loop 240; frame pointer 312; depth zero 654; global optimization off 623; trig scalar
// names 182; shared final normalization length 180 (two-point gain rejected); scaled cross 264;
// scaled limited input 241; scaled axis projection 267; full component rotation 239; accumulated
// alignment 245; normalization decision 183; scaled-cross scalars 186; projected-plus-cosine sums
// 186; ground memcpy 182; limited memcpy 182; separate real components 182; cosine complement 180;
// separate cross workspaces 182; cosine/complement before sine 181; explicit alignment with
// complement 182; initial j/k/i products 179/180/179; first-plane subtotal 180; rotation k/j/i
// products 242/183/237; turn alignment 180; paired jk/ki/ji/all initial products 186/186/187/186;
// word/long rotation decision 182/182; shared entry cross 182; all shared crosses 182; vector home
// names on recovered state base 182; rotation publication ijk/ikj/jik/jki/kji 242/241/244/247/244;
// cross between trig 247; accepted-up library copy 182; deferred k/i results 247; native
// state/workspace/both array views 182/182/182; native dot/up/orientation/full input fields
// 182/182/182/182; test-produced rotation 183; inactive/active/both forward output library copies
// 182/182/182; initial/all snapshot-safe crosses 182/182; ordinary cross inlining 182; both one-use
// aliases removed 180; fallback axis/default library copies 182/182; initial snapshot with aliases
// removed 182; all snapshots with fallback copies 182; settings-only/forward-only alias removal
// 291/291; up-field reference on combined alias removal 195; complementary skip decision
// bool/byte/short/long 182 each, each with consistent old-up field pointer 309 (losing the caller
// match); shared initial/rotation alignment workspace 288, with cosine complement, both one-use
// input aliases removed, or both 183/183/180; common normalization result alone, with complement,
// with alias removal, or all 183/180/180/288; consistent repeated original-up field
// pointer/reference before or after settings 309/309 each; active-entry or after ground-up
// initialization 196/196 each at retail length (the pointer takes a fourth register: state moves
// to EBX); existing flag predicate forced bool shift/mask, ordinary byte, ordinary bool
// shift/mask, forced word/long/signed byte 182/182/182/182/182/182/299/182; whole-dot
// multiplication operands commuted at initial, rotation and limit sites independently, in pairs,
// all three, then with every rotation product 182/183/182/183/182/183/183/183; the state dot
// through the file's shape_dot (state vector first or up first) 262/262 (1244 bytes: ESI loads as
// retail, but j*j comes before k*k and the forward result leaves EBX); the rotation dot as
// i + j + k with limited first 259 (1249 bytes), with axis first 261 (limited first in each
// product and k + j + i as retail, but k is computed before j).
// frame 0x28 agrees; initial state-dot displacement bytes and rotation registers differ.
// Round 90: the state dot through dot3f 262 / through a branch-local pointer 206 (both break caller 0x1e5af0);
// the rotation dot through shape_dot 182 (same code), through dot3f 248; the plain mode bit test for the
// one-caller bit helper 299 (breaks 0x1e5af0); the file's contact_cross for the duplicate cross helper 182
// (same code; kept, the duplicate removed).
// Earlier 182 (was 195): state/forward registers agree; rotation scheduling differs.
// Repeated up-field reads 182; native old-up 325; function-scope workspaces 195;
// vector names 195; shared dot helper 248; plain parameters 182; branch-local up pointer 195.
// Direct state dot loads still cost six bytes; cross before trig 247; late limited snapshot 190;
// cosine first 183; shared cosine scale 241; native settings mask 303.
// Initial-phase up pointer 309; between-trig snapshot 190; early alignment 192;
// rotation-only snapshot crosses 182; native j/k rotation loop 240.
// Frame pointer 312; depth zero 654; global optimization off 623; trig scalar names 182; shared final normalization length 180 (two-point gain rejected).
// Earlier 182 (was 182): direct-dot displacement bytes and rotation scalar lifetimes remain;
// scaled cross 264; scaled limited input 241; scaled axis projection 267;
// full component rotation 239; accumulated alignment 245; normalization decision 183;
// scaled-cross scalars 186; projected-plus-cosine sums 186.
// Frame 0x28 and state/output registers agree; six direct-dot displacement
// bytes and rotation scheduling remain. Ground memcpy 182; limited memcpy 182; separate real
// components 182; cosine complement 180 (two-point gain dropped); separate cross workspaces 182;
// cosine/complement before sine 181; explicit alignment with complement 182 (fast).
// direct state dot loads and rotation carriers remain;
// initial j/k/i products 179/180/179; first-plane subtotal 180; rotation
// k/j/i products 242/183/237; turn alignment 180; paired jk/ki/ji/all
// initial products 186/186/187/186. Three-point best rejected.
// Frame 0x28 and state/output registers agree; direct dot displacement bytes
// and rotation scheduling differ. Word rotation decision 182; long rotation decision 182; shared
// entry cross 182; all shared crosses 182; vector home names on recovered state base 182.
// Frame 0x28 and state/output homes agree; state-dot displacement and rotation schedule remain.
// Rotation publication ijk/ikj/jik/jki/kji 242/241/244/247/244; cross between trig 247;
// accepted-up library copy 182; deferred k/i results 247; native state/workspace/both
// array views 182/182/182. Original scalar expressions, shared helpers and outputs restored.
// Native dot/up/orientation/full input fields 182/182/182/182; test-produced rotation 183;
// inactive forward/active forward/both active output library copies 182/182/182.
// Initial/all snapshot-safe crosses 182/182; ordinary cross inlining 182; both one-use aliases removed 180;
// fallback axis/default library copies 182/182; initial snapshot with aliases removed 182;
// all snapshots with fallback copies 182; settings-only/forward-only alias removal 291/291;
// up-field reference on combined alias removal 195. Two-point gain rejected; original body/helper restored.
// Round 183 complementary skip decision bool/byte/short/long 182 each; each with consistent
// old-up field pointer 309 (fast), losing the caller match. All restored; initial dot
// displacement bytes and rotation schedule remain with EDI state/EBX forward preserved.
// Round 245 (fast): Shared initial/rotation alignment workspace 288; with cosine complement,
// both one-use input aliases removed, or both 183/183/180; common normalization result alone,
// with complement, with alias removal, or all 183/180/180/288. Two-point best rejected;
// matched caller preserved. Direct-dot displacement bytes and rotation SIMD lifetimes remain.
// Round 204: Consistent repeated original-up field pointer/reference before settings
// or after settings 309/309 each; active-entry or after ground-up initialization 196/196 each
// at retail length. Original field reads restored; direct-dot displacement and SIMD schedule remain.
// Round 155: Existing flag predicate: forced bool shift/mask, ordinary byte, ordinary bool
// shift/mask, forced word/long/signed byte: 182/182/182/182/182/182/299/182. Original byte predicate and complete vector body restored.
// Round 257: Complete rotation/limit/both projection products and each with complete initial
// products 186 each; all three with their native first subtotal 239; all three native subtotals
// alone 245. Original expressions restored; matched caller preserved in every preview.
// Round 158: Whole-dot multiplication operands commuted at initial, rotation and limit sites independently,
// in pairs, all three, then with every rotation product. Scores [182, 183, 182, 183, 182, 183, 183, 183].
// Original executable body restored.
// Round 178: complete accepted-up conditional 287; immutable ground-normal input
// 311; active-entry forward workspace 182; conditional plus ground input 334;
// conditional plus forward 287; ground input plus forward 311; all three 334;
// all with forward declared first 334. Input-split forms lose the matched caller;
// original body and its 182 score retained.
// Round 170: complete three-row axial/retained/swept product groups in three schedules
// 180/180/180; row schedule 181; coordinate schedule 180; complete initial products 180;
// cosine complement 183; both 182. Two-point gains rejected; original full body restored.
// Round 188: Branch-owned original-up/input-setter/output-setter subsets 1-7 score 296/297/306/362/384/352/365; all with named cosine complement 362. Every form loses the matched caller in the reduced image and is rejected; original whole body restored.
// Round 229: completed normalized/fallback axis handed to native mutable/const rotation input,
// geometry-join/rotation-owned lifetime and original/staged initial cross: 355/355/279/279/355/355/279/279.
// Original axis ownership, helpers and full body restored.
// Round 174: the rotation through existing helpers with the same formula: rotate_direction (forced
// inline, sine/cosine arguments) 308 (1277 bytes: it re-reads the vector after each store),
// rotate_vector_170d70 441, weapon_emission_rotate with the angle 495 (both out of line); `axis`
// declared before `up` 182, `rotate` declared first 182 (same code). 0x1e5af0 matched in each.
// Round 48 (lane 9): contact_cross as a function-like macro 342; the state dot through dot3f, either argument
// order, 262/262 (ESI loads and up in XMM5/6/7 as retail, but state moves from EDI to EBX and caller 0x1e5af0
// drops to 20); the state dot accumulated j, k, i 185; as k + (j + i) 184.
// Round 462: One complete original-up value for all active consumers, native/library copies
// after/before ground normal, original/removed one-use aliases: 314/314/325/325/314/314/325/325.
// Frames 0x38 or 0x34 versus 0x28; original observations, aliases, helper graph and full body
// restored.
// Round 191 (lane 3, fast): `vector3f up;` then `up = ground->normal;` 182 (same code); up copied
// component by component 293 (breaks 0x1e5af0); the old-forward pointer declared before `up` 297 (1229
// bytes: the first cross then reads the old up through ESI with up as the destination, the state dot
// keeps [edi+0x104/0x108]), declared after `up` 297 (1229), declared at function scope 288 (breaks
// 0x1e5af0). Retail keeps up in XMM5-7 through the state dot and the forward cross and multiplies the
// forward from memory into its own registers; ours keeps up in XMM4-6 as the destinations.
// @retail 0x1f1460
void function_1f1460(byte const *state, s_shape_state const *ground, vector3f *up_result, vector3f *forward_result)
{
    byte *settings = *(byte **)(state + 8);
    if (function_1f1462(*(dword *)settings))
    {
        vector3f up = ground->normal;
        vector3f axis;
        contact_cross((vector3f const *)(state + 0x100), &up, &axis);
        bool rotate = true;
        if (function_30bf0(&axis) == 0.0f)
        {
            real dot = ((vector3f const *)(state + 0x100))->j * up.j +
                ((vector3f const *)(state + 0x100))->k * up.k +
                ((vector3f const *)(state + 0x100))->i * up.i;
            if (dot > 0.0f)
                rotate = false;
            else
                axis = *(vector3f const *)(state + 0x100);
        }
        if (rotate)
        {
            vector3f limited = *(vector3f const *)(state + 0x100);
            real angle = g_510c54->rate * 5.235987663269043f;
            real sine = (real)sin(angle);
            real cosine = (real)cos(angle);
            real dot = (limited.k * axis.k + limited.j * axis.j + limited.i * axis.i) * (1.0f - cosine);
            vector3f cross;
            contact_cross(&limited, &axis, &cross);
            limited.k = axis.k * dot + limited.k * cosine - cross.k * sine;
            limited.i = dot * axis.i + limited.i * cosine - cross.i * sine;
            limited.j = axis.j * dot + limited.j * cosine - cross.j * sine;
            contact_cross(&limited, &up, &cross);
            if (cross.k * axis.k + cross.j * axis.j + cross.i * axis.i > 0.0f)
                up = limited;
        }
        vector3f const *old_forward = (vector3f const *)(state + 0xf4);
        vector3f cross;
        contact_cross(old_forward, &up, &cross);
        vector3f forward;
        contact_cross(&up, &cross, &forward);
        if (function_30bf0(&forward) == 0.0f)
        {
            contact_cross(&up, (vector3f const *)(state + 0x100), &cross);
            contact_cross(&up, &cross, &forward);
            if (function_30bf0(&forward) == 0.0f)
            {
                up = *g_4687b0;
                forward = *g_4687a8;
            }
        }
        *forward_result = forward;
        *up_result = up;
    }
    else
    {
        *forward_result = *(vector3f const *)(state + 0xf4);
        *up_result = *g_4687b0;
        forward_result->k = 0.0f;
        if (function_30bf0(forward_result) == 0.0f)
            *forward_result = *g_4687a8;
    }
}

struct s_unknown_1eb550;
extern s_unknown_1eb550 *g_51e9c4;
real g_47ffb0 = 0.5f;
real normalize2d(point2f *v);
bool __stdcall function_a75d0(vector3f *vector, real maximum);

PRIVATE inline vector3f shape_add(vector3f const &a, vector3f const &b)
{
    vector3f r; r.i = a.i + b.i; r.j = a.j + b.j; r.k = a.k + b.k; return r;
}
PRIVATE inline vector3f shape_subtract(vector3f const &a, vector3f const &b)
{
    vector3f r; r.i = a.i - b.i; r.j = a.j - b.j; r.k = a.k - b.k; return r;
}
PRIVATE inline vector3f shape_scale(vector3f const &a, real scale)
{
    vector3f r; r.i = a.i * scale; r.j = a.j * scale; r.k = a.k * scale; return r;
}
PRIVATE inline real shape_dot(vector3f const &a, vector3f const &b)
{
    return a.k * b.k + a.j * b.j + a.i * b.i;
}


PRIVATE __forceinline void function_1f087b(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
    real local_0 = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
    real local_1 = arg_0->k * arg_1->i - arg_0->i * arg_1->k;
    real local_2 = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
    arg_2->i = local_0;
    arg_2->j = local_1;
    arg_2->k = local_2;
}


struct s_flags_1f0870
{
    dword enabled : 1;
    dword unknown1 : 1;
    dword direct : 1;
    dword gravity : 1;
    dword damping : 1;
    dword unknown5 : 27;
};


// score 590 (was 590): frame 0x34 versus 0x30 and direct/vector homes remain; retail threads the
// flying test across the desired normalization (eight 0x30bf0 calls, the ground arm and the plain limited
// arm laid out after the return), ours does not (seven calls, all arms in place).
// Round 90: the body moved out of its one-caller forceinline wrapper, its volatile velocity stores, volatile
// gravity flag, size pragma and four one-caller vector helpers removed (582 -> 590, kept as clean-up: the
// casts alone 634, casts and flag 603, pragma alone 582). On that body: shift-form flags 614; the existing
// cross helper for the frame crosses 598. A from-scratch body in retail's map (basis copies, flags in
// retail's order, the magnitude through magnitude3d, settings read at each use) 699-720: the output
// pointer moves to the stack (ret 0xc) and the settings pointer stays in ebp across the normalizations.
// Earlier 582 (was 614): Frame 0x34 versus 0x30 and direct/vector homes remain.
// Earlier 621 from 625: frame 0x34 versus 0x30; direct-bit and vector homes still differ.
// Frame 0x34 versus 0x30; direct-mode carrier and original/limited vector homes remain.
// Earlier: frame 0x34 versus 0x30; direct-bit and vector homes still differ.
// Settings snapshot 631; common step 631; inline depth one 1367; scoped size 924; shared scale 630.
// Ordinary gravity with shared normalization 622; cached mode 648; limited-first 625;
// request bitfields 621; global optimization off 2562; direct scalar lifetimes 623; bitfields plus scalars 626.
// Request-record snapshot 625; inline depth two 621; ordinary gravity on native flags 621;
// shared direct/general delta 621; complete horizontal point copies 630.
// Earlier 614 from 621: frame 0x34 versus 0x30 and vector homes still differ.
// Native enabled test 626; byte flag view 619; low-normal first 618; current-velocity snapshot 625;
// repeated damping 624; native enabled/byte view 629; gravity first 616; byte view/low-normal 616;
// gravity/low-normal 614 kept; gravity/byte view 618.
// Earlier 614 from 614: Frame 0x34 versus 0x30; direct-bit and vector homes remain. Captured-word
// Earlier: frame 0x34 versus 0x30 and vector homes still differ.
// Earlier: Frame 0x34 versus 0x30; direct-bit and vector homes remain. Captured-word
// direct 626; damping 619; gravity 619; all predicates 627; later input magnitude 619 (fast).
// frame 0x34 versus 0x30; direct-mode register and vector homes still differ.
// Native enabled on gravity/low-normal body 624; direct before damping 618; direct distance result 619;
// general distance result 614; masked final gravity word 634.
// Earlier 614 from 614: frame 0x34 versus 0x30 and direct/general vector homes differ. Tried native damping
// Earlier: frame 0x34 versus 0x30 and direct/general vector homes differ. Tried native damping
// direct gravity 618; ordinary local output stores 634; native direct damping gravity 615; native direct
// gravity damping 615; history time snapshot 614; ground vertical value 614; ground vector arm first 629.
// frame 0x34 versus 0x30; direct-mode carrier and vector homes remain.
// Basis capture on current join 631; desired/delta scope 614; scale/subtract value helpers 681;
// later history maximum 614; shared lateral workspace 616.
// Earlier 614 from 614: Frame 0x34 versus 0x30 and direct/vector homes remain. Joint magnitude/desired scope
// 614; normal-only value 685; facing-only value 641; up-only value 632; damping/gravity/direct order 614
// (fast).
// Earlier 614 from 614: frame 0x34 versus 0x30; direct/vector homes remain. Ground-only basis
// 627; direct clipping scope 614; general clipping scope 614; scalar history change 741
// (frame 0x30, 2920 bytes); shared cross products 626 (fast).
// General/history/both value-add publications 659/641/690; disabled-input shared cross 582 (plain,
// kept); final direct reuse 620; final mode/direct reuse 631; per-use flight field 623; four Boolean-
// role names 614 (fast). Keep only the existing cross helper with the distinct local output.
// Earlier: Frame 0x34 versus 0x30 and direct/vector homes remain. Joint magnitude/desired scope
// Earlier: frame 0x34 versus 0x30; direct/vector homes remain. Ground-only basis
// Horizontal delta on retained join 614; signed-word direct/damping/gravity/flying 622/622/624/616.
// Unsigned-byte gravity/direct/damping/all states each 582; input squared-length helper 582;
// consistent active direct crosses 580; in-place history addition 641 (frame 0x30, 2981 bytes);
// byte states with direct crosses 580; flying-only/ground-only direct crosses 580/582.
// Two-point cross gain rejected; original private implementation and inherited qualifiers restored.
// Round 183 existing returned-vector cross for first flying / remaining flying / ground / all flying
// 607/659/590/694; first flying plus ground / remaining flying plus ground / all active / all paths
// 637/689/724/834 (fast). All restored; extra frame dword, direct bit and vector homes remain.
// Round 218, cleaned body: complete native flag observation 623; gravity conjunction 611;
// complete mode observation 625; flag/conjunction 645, flag/mode 605, conjunction/mode 652,
// all 628; all with byte step result 628. Original complete observations and result restored.
// Round 152: Disabled-input weighted forward/lateral/both axis scale helpers 590/610/610; vertical 590 at 3047 bytes, forward plus vertical 590, lateral plus vertical/all/all plus bounded-step helper 610/610/610. No score gain; original complete body and helper boundaries restored.
// Round 220: Separate flight-first/ground-first normalization and ground adjustment 616/607;
// separate normalization continuations 623; branch-produced gravity on those three graphs
// 616/607/623; single magnitude assigned in each mode arm with separate normalization,
// alone/with branch gravity, 647/647. Original clean shared-normalization body restored.
// Round 225: existing scaled-add producers for forward/lateral/both ground projections
// 590/590/590; complete parallel weighted sums in both motion modes 594, with forward/
// lateral/both projections 594/591/602; all with byte step result 602. Original body and
// helper boundaries restored; frame 0x34 and direct/vector homes remain.
// Round 179: direct initial/fallback/both eight-byte library copies 604/612/646; direct shared
// output setter 592; initial/fallback/both copies with that setter 595/600/641; native horizontal
// point with both copies and shared publication 641. Original clean body and helpers restored.
// Folded movement/history/both vector lifetimes 591/594/594; lateral shared across all three paths
// 590; movement/lateral 590; history/lateral 594; all 598; all with bounded declaration first 598.
// History reuse recovers frame 0x30 at 594/3060; captured flags/native enabled/both there 618/594/618.
// Round 235: Existing vector setters for disabled initial/lateral/vertical/initial+lateral/
// initial+vertical/lateral+vertical/all/all plus general output 627/590/600/627/628/600/628/629.
// All restored; the first three-axis forms reach 3047 bytes but retain the extra frame dword.
// Round 411: native setters at bounded initial/fallback/both/history copies
// 590/613/619/716; initial+history/fallback+history/all/all with direct output
// 710/692/686/680. Initial copy reaches retail length without a score gain;
// complete original body, copies, history publication and output receiver restored.
// Round 297: Native flying/ground basis values, shared staged flying crosses and
// their four combinations 646/642/591/666/646/631/666; all with the remaining
// crosses staged 666. Original basis observations, helpers and full body restored.
// Round 195: Native damping/direct consumers 633/612; two-component direct delta
// 589 (one-point gain rejected); both consumers 628; damping-delta/direct-delta/all
// 633/612/628; all with complete horizontal assignment 634. Original flags, vector
// types, copies, helpers and full body restored; frame 0x34 and direct/vector homes remain.
// Round 319: Complete rate/tick/time-receiver snapshots, seven subsets then early
// receiver 593/620/625/615/618/622/624/622; frames 0x34-0x3c versus 0x30.
// All restored; original flag/vector homes and normalization continuations remain.
// Round 48 (lane 9): flying through the mode-bit helper 0x1f1462 590; no `mode` local 592; flags dword and damping
// declared first 594; the enabled test through the flag view 591; one by-value flag-bits copy for all four tests
// 625; contact_cross as a function-like macro 621; 0x1f087b or shape_dot as macros 590/590.
// Round 248: complete native input/result-history/tuning-record field views, seven subsets
// and all with a twice-used history timestamp, 590 each. All types and executable body restored.
// J218: Output-pointer history change/offset/relative difference producers, seven subsets, then both named squared-length nodes: 659/647/723/611/687/672/743/748. Original complete body restored.
// @retail 0x1f0870
void function_1f0870(byte *history, vector3f const *current, byte const *state, byte *out)
{
#define settings (*(byte **)(state + 8))
    dword mode = *(dword *)settings;
    bool flying = ((mode >> 3) & 1) != 0;
    dword flags = *(dword *)(state + 0x14);
    s_flags_1f0870 const *flags_view = (s_flags_1f0870 const *)(state + 0x14);
    bool gravity = TEST_FIELD_BIT(flags_view->gravity);
    bool damping = TEST_FIELD_BIT(flags_view->damping);
    bool direct = TEST_FIELD_BIT(flags_view->direct);
    vector3f const &input = *(vector3f const *)(state + 0x138);
    vector3f const &facing = *(vector3f const *)(state + 0x9c);
    vector3f const &up = *(vector3f const *)(state + 0x100);
    vector3f const &normal = *(vector3f const *)(state + 0x8c);
    vector3f const &basis = *(vector3f const *)(state + 0x118);
    vector3f *velocity = (vector3f *)(out + 0xc);
    if (!(flags & 1))
    {
        vector3f left;
        contact_cross(&up, &facing, &left);
        real scale = *(real *)(state + 0x20);
        real local_0 = input.i * scale;
        real local_1 = input.j * scale;
        real local_2 = input.k * scale;
        velocity->i = facing.i * local_0;
        velocity->j = facing.j * local_0;
        velocity->k = facing.k * local_0;
        velocity->i += left.i * local_1;
        velocity->j += left.j * local_1;
        velocity->k += left.k * local_1;
        velocity->i += up.i * local_2;
        velocity->j += up.j * local_2;
        velocity->k += up.k * local_2;
        gravity = false;
    }
    else if (direct)
    {
        vector3f delta;
        delta.i = (*(real *)(state + 0x20)) * (facing.i * input.i - facing.j * input.j) - current->i;
        delta.j = (*(real *)(state + 0x20)) * (facing.j * input.i + facing.i * input.j) - current->j;
        point2f limited = { delta.i, delta.j };
        real step = *(real *)(state + 0x148) * g_510c54->rate;
        if (normalize2d(&limited) > step)
        {
            limited.x *= step;
            limited.y *= step;
        }
        else
        {
            limited.x = delta.i;
            limited.y = delta.j;
        }
        velocity->i = current->i + limited.x;
        velocity->j = current->j + limited.y;
        velocity->k = current->k - *(real *)(state + 0x130) * g_510c54->rate;
    }
    else
    {
        real magnitude = (real)sqrt(input.i * input.i + input.j * input.j + input.k * input.k);
        vector3f desired;
        if (flying)
        {
            vector3f left;
            function_1f087b(&normal, &basis, &left);
            if (function_30bf0(&left) == 0.0f)
            {
                function_1f087b(&normal, g_4687b0, &left);
                if (function_30bf0(&left) == 0.0f)
                {
                    function_1f087b(&normal, g_4687a8, &left);
                    function_30bf0(&left);
                }
            }
            vector3f forward;
            function_1f087b(&left, &normal, &forward);
            function_30bf0(&forward);
            desired.i = left.i * input.j + forward.i * input.i;
            desired.j = left.j * input.j + forward.j * input.i;
            desired.k = left.k * input.j + forward.k * input.i;
            desired.k += input.k;
        }
        else
        {
            if (!(normal.k > 0.001f))
            {
                vector3f left;
                function_1f087b(g_4687b0, &basis, &left);
                function_30bf0(&left);
                real local_3 = -shape_dot(normal, basis);
                vector3f forward;
                forward.i = basis.i + normal.i * local_3;
                forward.j = basis.j + normal.j * local_3;
                forward.k = basis.k + normal.k * local_3;
                real local_4 = -shape_dot(normal, left);
                left.i += normal.i * local_4;
                left.j += normal.j * local_4;
                left.k += normal.k * local_4;
                desired.i = left.i * input.j + forward.i * input.i;
                desired.j = left.j * input.j + forward.j * input.i;
                desired.k = left.k * input.j + forward.k * input.i;
                desired.k = (desired.k + input.k) * 5.0f;
            }
            else
            {
                desired.i = facing.i * input.i - facing.j * input.j;
                desired.j = facing.j * input.i + facing.i * input.j;
                desired.k = input.k - (normal.j * desired.j + normal.i * desired.i) / normal.k;
            }
        }
        function_30bf0(&desired);
        if (!flying)
        {
            if (*(real *)(settings + 0x58) >= desired.k)
                magnitude *= *(real *)(settings + 0x4c);
            else if (*(real *)(settings + 0x58) > desired.k)
                magnitude *= 1.0f + (*(real *)(settings + 0x4c) - 1.0f) * (desired.k - *(real *)(settings + 0x58)) /
                    (*(real *)(settings + 0x5c) - *(real *)(settings + 0x58));
            else if (desired.k >= *(real *)(settings + 0x64))
                magnitude *= *(real *)(settings + 0x50);
            else if (desired.k > *(real *)(settings + 0x60))
                magnitude *= 1.0f + (desired.k - *(real *)(settings + 0x60)) * (*(real *)(settings + 0x50) - 1.0f) /
                    (*(real *)(settings + 0x64) - *(real *)(settings + 0x60));
        }
        vector3f delta;
        real speed = *(real *)(state + 0x20) * magnitude;
        delta.i = desired.i * speed - current->i;
        delta.j = desired.j * speed - current->j;
        delta.k = desired.k * speed - current->k;
        vector3f limited = delta;
        real step = *(real *)(state + 0x144) * g_510c54->rate;
        bool limited_step = function_30bf0(&limited) > step;
        if (limited_step)
        {
            limited.i *= step;
            limited.j *= step;
            limited.k *= step;
        }
        else
            limited = delta;
        if (!limited_step || flying)
            gravity = false;
        if (!state[0xa8] && *(long *)((byte *)g_51e9c4 + 0x18) - g_510c54->game_time <= 0)
        {
            limited.i -= normal.i * 0.234f;
            limited.j -= normal.j * 0.234f;
            limited.k -= normal.k * 0.234f;
        }
        if (damping)
        {
            real fraction = 1.0f - *(real *)(state + 0x98);
            limited.i *= fraction;
            limited.j *= fraction;
        }
        if (gravity)
            limited.k -= *(real *)(state + 0x130) * g_510c54->rate;
        velocity->i = current->i + limited.i;
        velocity->j = current->j + limited.j;
        velocity->k = current->k + limited.k;
    }
    if (!((*(dword *)settings >> 2) & 1) && !(*(dword const *)(state + 0x14) & 4))
    {
        if (*(long *)(history + 0xc) != NONE && *(long *)(history + 0xc) == g_510c54->game_time - 1)
        {
            vector3f change = shape_add(shape_subtract(input, *(vector3f *)(history + 0x10)), *(vector3f *)(out + 0x64));
            real maximum = (real)sqrt(change.k * change.k + change.j * change.j + change.i * change.i) + g_47ffb0;
            vector3f offset = shape_subtract(*(vector3f const *)(state + 0x124), *(vector3f *)(out + 0x64));
            vector3f relative = shape_subtract(*velocity, offset);
            function_a75d0(&relative, maximum);
            velocity->i = relative.i + offset.i;
            velocity->j = relative.j + offset.j;
            velocity->k = relative.k + offset.k;
        }
        *(vector3f *)(history + 0x10) = input;
        *(long *)(history + 0xc) = g_510c54->game_time;
    }
    if (gravity)
        *(dword *)out |= 1;
    else
        *(dword *)out &= ~1;
}
#undef settings


PRIVATE inline void shape_rotate_vector(matrix3x3 const *matrix, vector3f const *input, vector3f *out)
{
    vector3f copy;
    if (input == out)
    {
        copy = *input;
        input = &copy;
    }
    vector3f const *left = &matrix->left;
    out->i = (input->j * left->i + input->k * matrix->up.i) + input->i * matrix->forward.i;
    out->j = (input->j * left->j + input->k * matrix->up.j) + input->i * matrix->forward.j;
    real k = input->j * left->k;
    k += input->k * matrix->up.k;
    k += input->i * matrix->forward.k;
    out->k = k;
}

/* one step of the shape against its contact, in the frame of a moving carrier or support when it has one */
// @retail 0x1f0450
void __stdcall function_1f0450(s_shape_state *history, byte *out, s_shape_carrier_contact const *contact)
{
    byte const *data = (byte const *)contact;
    vector3f current = *(vector3f *)(data + 0x124);
    byte *component = g_51e9b8->data + (*(long *)(data + 0x18) & 0xffff) * 0xa0;
    history->unknown1c = NONE;
    history->unknown5c = NONE;
    history->unknown20 = NONE;
    matrix3x3 rotation = *(matrix3x3 *)((byte *)g_4687d0 + 4);
    vector3f velocity = *g_4687a4;
    vector3f carrier_velocity, delta_velocity;
    bool override_velocity = *(*(byte **)(component + 0x70) + 0x44) != 0;
    bool carrier = function_1f1e50((s_shape_carrier_state *)history, contact, &carrier_velocity, &delta_velocity, &rotation);
    bool support;
    if (!carrier && function_1f1ee0(contact, history, &velocity, &rotation))
        support = true;
    else
    {
        support = false;
        if (carrier)
        {
            velocity = carrier_velocity;
            *(dword *)out |= 8;
            *(matrix3x3 *)(out + 0x30) = rotation;
        }
    }
    if (override_velocity)
    {
        vector3f const *velocity_source = (vector3f const *)(*(byte **)(component + 0x70) + 0x30);
        real_point3d_set((point3f *)(out + 0x64), velocity_source->i, velocity_source->j, velocity_source->k);
        current.i -= *(real *)(out + 0x64);
        current.j -= *(real *)(out + 0x68);
        current.k -= *(real *)(out + 0x6c);
    }
    else if (support || carrier)
    {
        current.i -= velocity.i;
        current.j -= velocity.j;
        current.k -= velocity.k;
        *(vector3f *)(out + 0x64) = velocity;
        *(matrix3x3 *)(out + 0x30) = rotation;
        if (carrier && *(byte *)((byte *)history + 0x62) > 1)
        {
            current.i += delta_velocity.i;
            current.j += delta_velocity.j;
            current.k += delta_velocity.k;
        }
    }
    function_1f0870((byte *)history, &current, data, out);
    if (override_velocity)
    {
        vector3f_add((vector3f *)(out + 0xc), (vector3f *)(out + 0x64), (vector3f *)(out + 0xc));
    }
    else if (support || carrier)
    {
        vector3f_add(&velocity, (vector3f *)(out + 0xc), (vector3f *)(out + 0xc));
        vector3f_rotate_into_axes(&rotation, (vector3f *)(out + 0x18), (vector3f *)(out + 0x18));
        shape_rotate_vector(&rotation, (vector3f *)(out + 0x24), (vector3f *)(out + 0x24));
    }
    long count = *(byte *)((byte *)history + 0x62) + 1;
    *(byte *)((byte *)history + 0x62) = (byte)(count > 0xfe ? 0xfe : count);
}


extern vector3f *g_4687bc;
bool function_109fd0(long object_index, vector3f *velocity);

PRIVATE inline real contact_normalize(vector3f *vector)
{
    real length = (real)sqrt(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabs(length) < 0.0001f)
        return 0.0f;
    real inverse = 1.0f / length;
    vector->i *= inverse;
    vector->j *= inverse;
    vector->k *= inverse;
    return length;
}

// score 418 (was 418): Frame 0x64 agrees; persistent zero, count/component homes and direction residency remain.
// Retail keeps zero in XMM6 and count at the frame bottom, component in the dead request slot;
// count initialization between the NONE stores 417, unqualified count 530; homes remain swapped.
// Contact-dot order and material-call expansion still differ.
// Frame 0x64 agrees; persistent zero and count/component homes remain.
// Frame 0x64 agrees; persistent zero and count/component homes remain.
// Frame 0x64 agrees; persistent zero, count/component homes and direction residency remain.
// Earlier 428 from 489: frame now 0x64; count/component and vector homes still differ.
// Frame 0x64 agrees; persistent zero, count/component homes and direction retention differ.
// Earlier: frame now 0x64; count/component and vector homes still differ.
// Three-pass countdown 490; preferred-contact cursor 527; search cursor 428;
// inline depth one 489; existing snapshot cross helper 488.
// Byte flag helper 471; shared adjustment join 437; final contact cursor 595;
// accepted-count pass bound 492; result publication 431.
// Ordinary count with search cursor 525; candidate countdown 474; fixed-pass do loop 427;
// snapshot projection helper 428; native relative-velocity subtraction 642.
// Frame pointer 692; depth zero 967; size preference 1011; moving arm first 449; native component subtraction loops 427 (one-point gain rejected).
// Earlier 428 from 428: Frame 0x64 agrees; persistent zero and count/component homes remain. Cursor-context entry count 427; entry result 425; entry desired/direction 428; short pass counter 428; final speed eligibility 430 (fast).
// Earlier 428 from 428: persistent zero and accepted-count/component homes still differ.
// Entry count/result interaction 528; contact-base field reference 428; count-field references 428;
// entry count/result plus base reference 528; entry count/result plus count references 528.
// Earlier 428 from 428: Frame 0x64 agrees; zero/count/component homes remain. Native component 428; stepping
// value subtraction 445; dual preferred/search cursors 434; response-scoped indices 428; native
// component/scoped indices 428; dual cursors with projection helper 454; dual cursors with entry count 433
// (fast).
// Earlier 421 from 428: frame 0x64 agrees; persistent zero and count/component homes remain.
// Two-contact response first 421 kept; entry index array 428; dynamic preferred bound 428;
// fallback-only shared negative scale 428; byte pass ordinal 434 (fast).
// Request flags once 494; flight once 531; preferred surface once 467; complete contact-base snapshot
// 587; signed contact-bound snapshot 496; flags/flight 596; base/bound 651; flags/flight/preferred 659
// (fast). All restored.
// Round 361: native contact records in preferred/candidate/response/final scans 421/421/421/421;
// preferred plus candidate, response plus final, all scans and all read-only records 421/421/421/421
// (fast). Existing record layout and complete body restored.
// Earlier: Frame 0x64 agrees; persistent zero and count/component homes remain. Cursor-context entry count 427; entry result 425; entry desired/direction 428; short pass counter 428; final speed eligibility 430 (fast).
// Earlier: persistent zero and accepted-count/component homes still differ.
// Earlier: Frame 0x64 agrees; zero/count/component homes remain. Native component 428; stepping
// Earlier: frame 0x64 agrees; persistent zero and count/component homes remain.
// Byte/unsigned-word accepted count 462/426; unsigned fixed pass 421; separate entry
// desired/direction storage 421/421.
// Round 163: Local speed/object/both/surface/speed+surface/object+surface/all
// scalar outputs and complete output/Boolean join 554/499/554/570/621/622/613/547.
// All regress; original direct publications, qualified count and cursor restored.
// Round 166: Whole squared/absolute/both magnitude names in both normalizations
// 421/422/422; desired-length result alone/with square/with absolute/with both
// 421/421/422/422 (one-use return forms match-only); existing squared-length helper 421;
// Boolean flight expressions at both tests 466. Original helper, expressions and body restored.
// Existing selection/tangent/speed dot inputs reversed independently, in all seven combinations,
// and together with the accepted-count lexical home: 421 each. Helpers and body restored.
// Round 152: Eight relative/desired/direction library-copy forms 421 each; complete accepted-count producer shared by insertion, increment, store and both tests 419, with one-contact-first 419 (2089 bytes). Two-point forms rejected; original copies, qualified count, cursor and response order restored.
// Round 225: actual accepted-index array traversal 423; response-scoped minimum/selected
// 421; both 423; index-first selection publication 421, with array/scope/both 423/421/423;
// all plus a final Boolean-result join 422 at retail length. Original body, qualified count,
// candidate cursor and response order restored; zero and count/component homes remain.
// Round 28: Existing query/support-output ordinary/inline/noinline declaration pairs:
// inline query with each support contract 434; the other five pairs 421.
// Original declarations, qualified count, candidate cursor and response order restored.
// Round 179: retained selected record 433; initial speed zero after other outputs 453; both 458;
// minimum-first acceptance 506; selected record/minimum 505; zero/minimum 490; all 498; all with
// explicit final result 499. Original selected ordinal, publications, count and body restored.
// Round 170: count initialized between surface/object publications 420; separate array store
// and increment 421; both 420; one-based pass alone/with initialization/with split producer/all
// 421/420/421/420; count initialized at the relative/desired boundary 520. One-point gains
// rejected; original full body restored, with persistent zero and count/component homes unresolved.
// Round 235: Existing vector setters for default/negated desired/projected direction/
// default+negation/default+projection/negation+projection/all/all with test-produced result
// 482/421/421/482/482/421/482/479; all restored; persistent zero and scalar homes remain.
// Round 411: existing setters at input/relative, relative/desired, both and
// desired/direction clones 584/511/615/428; input+direction/desired+direction/
// all/all plus fallback direction clone 585/509/616/615. All restored; original
// copies, qualified count, candidate cursor, outputs and arithmetic retained.
// Round 188: Single-exit desired/tangent normalization and existing in-place difference producers in subsets 1-7 score 422/421/422/421/422/421/422; all with test-produced success 523. Original helpers, count qualification, cursor and full body restored.
// N226: Complete preferred/fallback/one-contact normal snapshots, seven
// subsets 465/533/571/483/571/533/621; all with native-size default output copy 621.
// All regress; original complete body, qualified count and candidate cursor restored.
// Round 174 (on 418): the file's bit helper 0x1f1462 for the shape's bit-3 test (retail's
// `shr eax, 3; test al, 1`) at the first test 440, at both tests 464, also for 0x1f19c0's flying
// flag 464; one inline helper for 0x1f2e60's request argument pack at all three calls and
// 0x1f2a80's loop 425 (0x1f2a80 124); `count` without volatile 531; the stepping flag named before
// the final loop and tested there (retail keeps `(flags >> 1) & 1` in a stack byte for that test)
// 426, declared before the height 426 (the loop then lines up; retail hoists the zero for
// `0.0f - dot` into XMM6 where ours hoists 0.001).
// Round 48 (lane 9): ours already returns `ret 0x18` with velocity in EAX and input in ECX, as retail; input
// declared last 418 (same code). Final loop: height summed settings-first 418 (same code); the projection as unary
// minus 418 (same code: `xorps`/`subss` either way); the flag test as `((flags >> 1) & 1) != 0` 418; a named
// `height - 0.001f` 419; the flag named before the loop in shift form 426; contact_cross as a macro 508; shape_dot
// as a macro 418.
// Round 248: coherent complete-state first parameter with initial-point helper views, repeated
// normal observation and native request/tuning fields 418/418/460/460/418/418/460/460.
// Caller stays 230; all seven matched file controls survive. Original contract and body restored.
// @retail 0x1f2250
bool function_1f2250(vector3f const *velocity, byte const *request, vector3f const *input,
    bool moving, vector3f *out, long *surface, real *speed, long *object_index)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    vector3f relative = *input;
    *speed = 0.0f;
    *out = *g_4687b0;
    *surface = NONE;
    *object_index = NONE;
    volatile long count = 0;
    long indices[3];
    if (!moving)
    {
        if (*(long *)(request + 4) != NONE)
        {
            vector3f object_velocity;
            if (function_109fd0(*(long *)(request + 4), &object_velocity))
            {
                relative.i -= object_velocity.i;
                relative.j -= object_velocity.j;
                relative.k -= object_velocity.k;
            }
        }
    }
    else
    {
        relative.i -= velocity->i;
        relative.j -= velocity->j;
        relative.k -= velocity->k;
    }
    vector3f desired = relative;
    if (*(long *)(request + 0x44) != NONE)
    {
        long contacts = *(long *)(component + 0x8c);
        for (long i = 0; i < contacts; ++i)
        {
            byte *contact = *(byte **)(component + 0x88) + i * 0x48;
            if (*(long *)(request + 0x44) == *(long *)(contact + 0x14) &&
                *(real *)(contact + 0x30) > *(real *)((*(byte **)request) + 0x54) &&
                function_1f2e60((s_shape_ground *)contact, moving, velocity, (point3f const *)(request + 0x1c),
                    (((*(dword *)(request + 0x18) >> 1) & 1) != 0), *(real *)((*(byte **)request) + 0xc)))
            {
                desired.i = *(real *)(contact + 0x28) * -1.0f;
                desired.j = *(real *)(contact + 0x2c) * -1.0f;
                desired.k = *(real *)(contact + 0x30) * -1.0f;
                break;
            }
        }
    }
    if ((*(dword *)(*(byte **)request) >> 3) & 1)
    {
        if (((*(dword *)(request + 0x18) & 2) != 0))
            {
                desired.i -= *(real *)((byte *)velocity + 0x64);
                desired.j -= *(real *)((byte *)velocity + 0x68);
                desired.k -= *(real *)((byte *)velocity + 0x6c);
            }
    }
    else
    {
        if (!((*(dword *)(request + 0x18) & 2) != 0))
        {
            if (*(long *)(request + 8) == *(long *)(request + 4))
                desired = *g_4687bc;
            else if (desired.k < 0.0f)
                desired.k *= 30.0f;
        }
        if (((*(dword *)(request + 0x18) & 2) != 0))
            {
                desired.i -= *(real *)((byte *)velocity + 0x64);
                desired.j -= *(real *)((byte *)velocity + 0x68);
                desired.k -= *(real *)((byte *)velocity + 0x6c);
            }
        else if (length_sq3f((vector3f const *)(request + 0x34)) == 0.0f)
            desired = *g_4687bc;
    }
    if (contact_normalize(&desired) != 0.0f)
    {
        vector3f direction = desired;
        for (long iteration = 0; iteration < 3; ++iteration)
        {
            real minimum = 3.4028234663852886e+38f;
            long selected = NONE;
            long contacts = *(long *)(component + 0x8c);
            byte *candidate = *(byte **)(component + 0x88);
            for (long i = 0; i < contacts; ++i)
            {
                if (function_1f2e60((s_shape_ground *)candidate, moving, velocity, (point3f const *)(request + 0x1c),
                    (((*(dword *)(request + 0x18) >> 1) & 1) != 0), *(real *)((*(byte **)request) + 0xc)) &&
                    (!((*(dword *)(request + 0x18) & 2) != 0) || *(long *)(request + 0x44) == NONE || *(long *)(request + 0x44) == *(long *)(candidate + 0x14)))
                {
                    real dot = dot3f(&direction, (vector3f *)(candidate + 0x28));
                    if (dot < -0.025f && dot < minimum)
                    {
                        minimum = dot;
                        selected = i;
                    }
                }
                candidate += 0x48;
            }
            if (selected == NONE)
            {
                if (count || !((*(dword *)(*(byte **)request) >> 3) & 1) || !contacts)
                    break;
                byte *first = *(byte **)(component + 0x88);
                desired.i = *(real *)(first + 0x28) * -1.0f;
                desired.j = *(real *)(first + 0x2c) * -1.0f;
                desired.k = *(real *)(first + 0x30) * -1.0f;
                direction = desired;
                minimum = -1.0f;
                selected = 0;
            }
            *surface = selected;
            indices[count++] = selected;
            byte *contact = *(byte **)(component + 0x88) + selected * 0x48;
            if (count == 2)
            {
                byte *contacts = *(byte **)(component + 0x88);
                vector3f tangent;
                contact_cross((vector3f *)(contacts + indices[0] * 0x48 + 0x28),
                    (vector3f *)(contacts + indices[1] * 0x48 + 0x28), &tangent);
                contact_normalize(&tangent);
                vector3f_scale(&tangent, dot3f(&tangent, &desired), &direction);
            }
            else if (count == 1)
            {
                direction.i = desired.i - *(real *)(contact + 0x28) * minimum;
                direction.j = desired.j - *(real *)(contact + 0x2c) * minimum;
                direction.k = desired.k - *(real *)(contact + 0x30) * minimum;
            }
        }
        function_1f2a80(request, moving, &desired, indices, count, out, velocity);
    }
    real height = *(real *)(request + 0x24) + *(real *)((*(byte **)request) + 0xc);
    for (long i = 0; i < *(long *)(component + 0x8c); ++i)
    {
        byte *contact = *(byte **)(component + 0x88) + i * 0x48;
        if (function_1f2e60((s_shape_ground *)contact, moving, velocity, (point3f const *)(request + 0x1c),
            (((*(dword *)(request + 0x18) >> 1) & 1) != 0), *(real *)((*(byte **)request) + 0xc)))
        {
            real projection = 0.0f - shape_dot(*(vector3f *)(contact + 0x28), relative);
            if (((*(dword *)(request + 0x18) & 2) != 0) && height - 0.001f > *(real *)(contact + 0x24) && projection > *speed)
                *speed = projection;
        }
        long object = *(long *)(contact + 0x18);
        if (object != NONE && (*object_index == NONE || ((1 << g_4e0300->data[(object & 0xffff) * 12 + 3]) & 1)))
            *object_index = object;
    }
    return count && out->k > *(real *)((*(byte **)request) + 0x54);
}


real function_1201a0(vector3f *vector, vector3f const *fallback);
bool havok_component_any_rigid_body_active(s_havok_component *component);

// score 221 (was 230): Frame 0x28 and the 1066-byte length agree; movement zero width, reset/selection homes, cold arms and second material-call expansion remain.
// The real movement carrier fixes the x87 prefix through +0x8b (233 overall); retail
// forms the contact dot k/j before i while our products schedule differently.
// Frame 0x28 agrees; zero-load width, reset/selection homes and material-call
// expansion remain. Our build inlines 0x1f1df0 at the second call (after 0x1f2250) where retail
// calls it at both sites; retail also lays the not-supported arms of the contact loop out after the
// first post-loop call block, keeps the supported flag in its stack byte and compares the movement
// length with a float constant.
// Tried: float consistency 392; native normal 368; current supported long 261; current flag long 361;
// unsupported-first with original magnitude 258; native movement input 284; combined eligibility
// 240; per-contact body-velocity snapshot 346; highest reduction after selection 258; approach at
// use 351; surface output at entry 230; preservation arm first 288; entry score storage 230; shared
// squared length with double comparison 230; byte current flag 240; byte current supported 239;
// library history resets 238; contact-base field reference 235; unsigned selected ordinal 230;
// native saved predicates at original magnitude 298; preservation predicate at dispatch 382; native
// component 230; flag after supported 355; long moving 300; long saved supported 293; long saved
// flag 296; separate inline declaration 230; entry normal workspace 230; unsigned highest 234;
// native-long flying 306; final cleanup exit 230; normal/ordinal combination 232; signed-word
// moving/saved-supported/saved-flag/current-supported/current-flag 298/283/295/272/351;
// component/component+contact/velocity/contact+velocity/component+velocity/all fields at use, all
// with real magnitude, component root plus shared squared length 460/404/334/336/501/540/540/460;
// the movement length as `real` 233 (float constant as retail, first difference moves to +0x8b);
// with the two NONE locals declared before the unknown70 store 283; with the contact list read
// from the component field in the loop 234 (loaded after the count guard as retail), with dot3f
// for the two contact dots 234 (same code: VC swaps the first two terms either way); with a
// separate `__declspec(noinline)` declaration of 0x1f1df0 297 (1005 bytes: both calls out of line
// as retail, 0x1f1df0 still matches; the arm layout and the supported flag's home are what remain).
// Round 179: common normal/material-call join 254; native movement 253; combined preservation
// mask/both 320/320; the same four joins with continue 254/253/320/320; original calls with the
// combined preservation mask 324. Original body, magnitude, call sites and normal scopes restored.
// Complete native contact record alone/real magnitude/noinline/both 230/233/301/300;
// the same four forms with operation-specific normal names 230/233/301/300. All restored.
// Round 235: Named squared sum with double/real movement 241/229; all three named squared
// products 243/229; products plus sum 243/233; complete graph with long/short moving 307/305.
// One-point forms restored; original magnitude, selection and material calls retained.
// Round 411: stationary/unsupported/both/unflagged predicate ownership
// 257/266/285/293; stationary+unflagged/unsupported+unflagged/all/all with
// native magnitude 286/293/286/287. Complete original magnitude, positive
// current/saved predicates, observations, selection and calls restored.
// Round 188: Existing reset/selected-normal/final-vector setters in subsets 1-7 score 292/247/304/241/297/256/309; all with real magnitude 308. Original copies, magnitude, input timing and calls restored.
// N226: Shared contact-byte observation; native movement, highest-first and
// eligibility-owned support masks 0..7 give 240/237/278/277/356/356/304/304.
// Existing complete length directly at the movement test, original/shared-byte
// graphs 320/320; all original observations, magnitude, body and declarations restored.
// Round 174: the contact list read at each use (its load then follows the count test as in retail)
// 235, with the selected/highest NONE locals declared at the top of the block 233 (on the original
// body 235); 0x1f1df0 written without its union local, `== NONE` or `!= NONE` first, 238/244 (0x1f1df0
// itself 8/4, not kept); the movement length as `real` 233, with the contact list at each use 234;
// the five `goto local_10` skips as `continue` 230 (same code; kept, the label removed).
// Round 48 (lane 9): the twice-written surface clear as one inline / `__forceinline` helper 230/230 (same code);
// the flag/no-flag skip tests as one ternary, nested in one `else`, or the no-flag arm as two tests 230/230/230
// (same code); the no-flag arm first 252; 0x1f2250's input parameter declared last 230 (same code: ours already
// passes it in ECX); shape_dot as a macro 230; the five accept assignments written in each of the three arms 361.
// Round 248: complete native request/result/tuning-field identities, four double-magnitude
// forms 230 each; four real-magnitude counterparts 233 each. Original body and types restored.
// Round 62 (lane 12, fast): the velocity pointer declared after the four reset stores 221 (kept, plain
// 221, retail's length; the second 0x1f1df0 call is still inlined). Airborne as `(flags & 8) || (flags & 0x10)`
// (a compile-only screen gives retail's `xor al, al / mov al, 1` for it) 312 on the old body, 311 with the
// `real` length, 308 on the real-length velocity form (the component pointer moves from EBX to ESI); the
// selected contact's normal through `byte *contact = contacts + (short)selected * 0x48` 241 on the real-length
// body (retail's separate `add eax, 0x28` appears); on the kept form: the `real` length 224 (the loop's k/j
// products then match, the i term flips), `selected`/`highest` declared first in the block 220, the same with
// the `real` length 223, the contact list read at each use 222, the not-supported arms written first 250,
// the velocity after `flying` 221, and (with the `real` length) the motion pointer read before the reset 294.
// Round 120 (m3 lane 5): a per-arm `better` result tested once after the arms 240 (same layout: the
// flag/no-flag arms stay inline and still share one height test); unknown58/selected/highest as one
// chained NONE store 284; the airborne test as `(flags & 8) || (flags & 0x10)` 312 (that block is then
// retail's branchy sequence exactly, but the component moves from EBX to ESI), with the movement as
// `real` 311, with the contact list read at each use 232 (1072 bytes, component back in EBX; the NONE
// locals still store after the count test); the flying test's planar dot i-first or with each
// product's operands swapped 230/230 (same code: the speed dot still computes j before k).
// Diagnostic, not kept: 0x1f1df0 out of line at both calls 301 (1005 bytes). Original body restored.
// Round 191 (lane 3, fast): retail's float 0.0 constant (0x45dbd8) confirms the `real` magnitude. On
// it, the velocity pointer declared first, selected/highest declared right after the unknown58 store
// and the contact list read at each use 273 (1076 bytes): the reset block is then retail's exactly
// (velocity load first, NONE in EAX stored to unknown58, selected and highest, the list loaded after
// the count test) but the arms' shared height test and the inlined second 0x1f1df0 call stay; that
// with the airborne test as `(flags & 8) || (flags & 0x10)` 270 (1072); with the double magnitude 274.
// The existing havok_component_get for the component (plain inline, to spend inline budget) 221
// (same code, on either base); 0x1f1df0 defined after this function 221 (same code: the second call
// stays inlined). Retail tests the height in both not-supported arms (two `comiss; ja` copies, laid
// out after the first post-loop block); ours merges them into one.
// @retail 0x1f19c0
void function_1f19c0(byte const *request, s_shape_state *history, byte *out)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    dword flags = *(dword *)(request + 0x18);
    bool preserve = (flags & 4) || ((flags & 1) && !(flags & 2)) || (flags & 0x18);
    double movement = sqrt(history->point.x * history->point.x + history->point.y * history->point.y + history->point.z * history->point.z);
    bool moving = movement != 0.0f;
    *(dword *)(out + 4) &= ~1;
    if (!preserve)
    {
        history->normal = *g_4687b0;
        history->material = g_47d8e0;
        history->unknown58 = NONE;
        history->unknown70 = 0.0f;
        vector3f const *velocity = (vector3f const *)(*(byte **)(component + 0x70) + 0xc);
        bool flying = ((**(dword **)request >> 3) & 1) != 0;
        long selected = NONE;
        long highest = NONE;
        real best_height = -3.4028234663852886e+38f;
        real best_speed = -3.4028234663852886e+38f;
        real highest_z = -3.4028234663852886e+38f;
        bool best_supported = false;
        bool best_flag = false;
        byte *contacts = *(byte **)(component + 0x88);
        for (long i = 0; i < *(long *)(component + 0x8c); ++i)
        {
            byte *contact = contacts + i * 0x48;
            vector3f *normal = (vector3f *)(contact + 0x28);
            real speed = 0.0f - shape_dot(*normal, *velocity);
            real approach = 0.0f - shape_dot(*normal, *(vector3f *)(request + 0x28)) * *(real *)(request + 0x34);
            bool flag = ((*(byte *)(contact + 0x44) >> 1) & 1) != 0;
            bool supported = (flying || (*(byte *)(contact + 0x44) & 4)) && approach >= -0.001f;
            if (normal->k > highest_z)
            {
                highest = i;
                highest_z = normal->k;
            }
            if (*(long *)(contact + 0x14) != NONE || speed > 0.0f)
            {
                if (supported)
                {
                    if (!flying && normal->j * velocity->j + normal->i * velocity->i > 0.5f)
                        continue;
                    if (best_supported && !(speed > best_speed))
                        continue;
                    if (fabs(*(real *)(contact + 0x38)) < 0.0001f)
                        continue;
                }
                else if (flag)
                {
                    if (best_flag && !(normal->k > best_height))
                        continue;
                }
                else if (best_supported || best_flag || !(normal->k > best_height))
                    continue;
                selected = i;
                best_height = normal->k;
                best_supported = supported;
                best_speed = speed;
                best_flag = flag;
            }
        }
        if ((short)selected != NONE && best_supported)
        {
            vector3f normal = *(vector3f *)(contacts + (short)selected * 0x48 + 0x28);
            function_1201a0(&normal, g_4687b0);
            function_1f1df0(*(long *)(request + 0x40), (short)selected, &normal, history);
        }
        else
        {
            vector3f normal;
            long surface;
            if (function_1f2250((vector3f *)history, request, velocity, moving, &normal, &surface,
                &history->unknown70, (long *)(out + 0xc)))
            {
                function_1f1df0(*(long *)(request + 0x40), highest, &normal, history);
                if ((short)selected != NONE)
                {
                    if (history->unknown70 > 0.0f)
                        *(dword *)(out + 4) |= 2;
                    if (!best_supported)
                        *(long *)(out + 8) = (short)selected;
                }
            }
            else
                *(dword *)(out + 4) |= 1;
        }
    }
    else
    {
        flags = *(dword *)(request + 0x18);
        bool airborne = (flags & 0x18) != 0;
        if ((flags & 2) || airborne)
        {
            history->normal = *g_4687b0;
            history->material = g_47d8e0;
            history->unknown58 = NONE;
            history->unknown70 = 0.0f;
            if (airborne)
                *(dword *)(out + 4) |= 1;
        }
    }
    if (!havok_component_any_rigid_body_active((s_havok_component *)component))
        *(dword *)(out + 4) &= ~1;
    if (*(long *)(request + 8) == NONE)
    {
        *(vector3f *)history = *g_4687a4;
        if (!(*(byte *)(out + 4) & 1) && *(long *)(out + 8) != NONE)
        {
            byte *contact = *(byte **)(component + 0x88) + *(long *)(out + 8) * 0x48;
            if (*(long *)(contact + 0x18) == *(long *)(request + 4) && *(long *)(request + 8) == NONE)
            {
                *(long *)out = *(long *)(request + 4);
                *(vector3f *)history = *g_4687a4;
            }
        }
    }
}
