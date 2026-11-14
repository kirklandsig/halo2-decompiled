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
void function_1f1df0(s_shape_state *state, long component_index, long material_index, vector3f const *normal)
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
	byte unknown00[8];
	byte *settings;
	byte unknown0c[4];
	long object_index;
	dword flags;
	byte unknown18[8];
	real speed;
	byte unknown24[0x58 - 0x24];
	transform4x3f transform;
	vector3f normal;
	real damping;
	vector3f facing;
	byte flag_a8;
	byte unknowna9[0xe8 - 0xa9];
	point3f point;
	byte unknownf4[0x100 - 0xf4];
	vector3f up;
	byte unknown10c[0x118 - 0x10c];
	vector3f basis;
	vector3f velocity;
	real gravity;
	byte unknown134[4];
	vector3f input;
	real acceleration;
	real direct_acceleration;
};
typedef char assert_shape_carrier_contact_normal[(offsetof(s_shape_carrier_contact, normal) == 0x8c) ? 1 : -1];
typedef char assert_shape_carrier_contact_point[(offsetof(s_shape_carrier_contact, point) == 0xe8) ? 1 : -1];
typedef char assert_shape_carrier_contact_input[(offsetof(s_shape_carrier_contact, input) == 0x138) ? 1 : -1];
typedef char assert_shape_carrier_contact_direct_acceleration[(offsetof(s_shape_carrier_contact, direct_acceleration) == 0x148) ? 1 : -1];

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
void havok_component_rigid_body_point_velocity_get(vector3f *velocity, long rigid_body_index, s_havok_component *component, point3f const *point);
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
                    havok_component_rigid_body_point_velocity_get(velocity, *(signed char *)(entry + 0x46), other, &contact->point);
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

PRIVATE inline void contact_cross(vector3f const *a, vector3f const *b, vector3f *out)
{
    out->i = a->j * b->k - a->k * b->j;
    out->j = a->k * b->i - a->i * b->k;
    out->k = a->i * b->j - a->j * b->i;
}

// score 43 (was 43; 43 at r76; before r98 was 122): 978/977 bytes; the first cross, both selected arms and their shared j/k tail are retail's. Left: case 3's
// arms are laid out none-first (retail selected-first, about 22 lines), the first cross's a.k * b.i and average.i load a first
// (retail b.i first), both dots sum k, j, i (retail i, k, j) and the inlined 0x1201a0 tail loads j first (retail copies inv first).
// MiniMax D (11-11): no lower score (43): the permuter over the case arms and the dot products found nothing better in its
// time; re-laying the arms the way retail has them breaks the tail that the two selected arms share, so the body stands.
// Round 219 (lane 3; fast, plain for the kept form): case 3's arms swapped (`selected == NONE` first), the existing cross3f_staged
// in both selected arms and direct `*out = *a` / `*out = average` copies 43 (kept; each alone or in pairs 58-199). On that body:
// case 3 selected-first again with a `break` 178; `if (selected == NONE) { copy; break; }` 43; case 3 through the existing
// vector3f_add_scaled 43 (selected-first 178); cases 1, 3, 2 in source 43; `support` join 58; 0x1f087b arms 198; the selected normal
// after the first cross 204 (82 selected-first); lowest/selected declarations swapped 45; dot_average first 43; average.i as
// a.i + b.i 43; dots as dot3f(a, desired), shape_dot either order, inside the compare 43 each; first cross through cross3f or
// cross3f_staged 43/43; case 3 with no none-arm copy (selected-first, or an early `break`) 192/192 (988); `0.0f - normal->k` 43 (178
// selected-first). Selected-first case 3: average through vector3f_add either order 177 (962 bytes); dots inside the compare,
// cross3f/cross3f_staged first cross, scale local, non-const a/b, loop index local 178 each; entry copy as a `default:` 214.
// Earlier 122: frame 0x14 and scan prefix agree; first +0xd5 is a displaced case span; cross coloring and selected-tail sharing remain.
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
// Round 108 (m3 lane 7, fast): the selected normal copied by value 189 (997 bytes; compile-only it gives
// retail's XMM1/2/0 first cross but adds the copy); `break` after the selected == 0 arm 122 (same code); the
// two dots inside the compare 122 (same code); the averaging arm first (`selected == NONE`) 187 (980 bytes);
// `selected != 0` arm first ending in `break` 164 (975 bytes). Ours already stores i/j/k from the same
// registers in both selected arms, yet VC does not cross-jump them (compile-only too); retail's merged tail
// starts at the g_4687b0 reload after the i store.
// J218: Existing staged-cross output setter/three-contact output-pointer scaled-add/native scratch values, seven subsets, then scratch value assignment: 122/122/122/199/199/199/199/199. Pointer-result staged cross, forced/ordinary inline, exact-only: 122/122. Original complete body restored.
// Round 50 (lane 10, fast with 0x1201a0 and 0x1f2e60 listed): the averaging result as one `*out = dot_a < dot_average
// ? *a : average;` copy (no `support`) 201 (996 bytes); `a`/`b` declared in each branch 306 (1027 bytes); the first
// cross through 0x1f087b 122 (same code); the `selected != 0` arm first 164 (975 bytes, as before); the compare
// written `dot_average > dot_a` 122 (same code); dot_average computed before dot_a 122 (same code). Ours hoists the
// g_4687b0 reload into each selected arm (each computes j, k, i), so the arms' j/k store tails are not shared.
// Round 289: Inherited three-contact default, with direct average publication/shared addition/dot-order
// interactions masks 0..7, 193/211/190/189/193/211/190/189. All restored; original
// default publications, average join, helper bodies, calls and arithmetic retained.
// Round 329: Corresponding first/second/both product parts in every first-cross row
// 122/122/73; selected-cross rows 165/165/122; joint first/all parts 122/122.
// Both parts named in the shared first-cross helper (contact_cross): plain 73 at 978/977 bytes,
// not kept: the helper change raises its other callers (below).
// Seven within-row second-product-first declaration orders all 73, with the same
// caller counts; original product order and selected helper restored. Existing
// unmatched callers: 0x1f1460 182 -> 200, 0x1f2250 418 -> 419, 0x1f0870 stays 590.
// M3 lane 8 r135 (fast and compile-only; aligned in brackets, base 122 [93]; body unchanged, the lead below is match-only): the first
// cross's registers and the arm merge follow the function's tuple phase (period 4): a diagnostic one-product chain with the
// arms computing k, j, i or i, k, j gave retail's XMM1/2/0 and the merged arms (191 [41]); other phases 207-225. Lead: a pointer
// local for the up vector scoped to case 3 at all four of its reads, the existing cross3f_staged in the two selected arms (retail
// computes k, j, i there) and `*out` copies in the averaging arm: 21 [27], 978 bytes, plain, nothing else moved. Not kept: entry and
// tail read the global directly, a hand-reload mix unless it matches. `up` at the entry copy and the fallback instead 21 [19], at entry
// only or as a reference 21; `up` at the whole function 214, entry and case 3 178, arm-only 178. Without any `up` 178. 0x1f087b itself
// reordered gives the same 21 but raises 0x23de50 45 -> 217 (it inlines 0x1f087b across files), so it stays i, j, k. The entry copy as
// memcpy, by index or through a value 178, by fields 317. No other natural form gives the odd phase step: a case-2 or function entries
// pointer shifts by two (needs a further step, compile-only 39-43 with a chain, 75-82 without), named indices and other one-use locals
// shift nothing. On the lead: statement dots `dot = i; dot += k; dot += j` 9 [7] (dot3f's body by hand; dots k,i,j 17, j,i,k 21),
// leaving only b.i's two operands (retail loads b.i: the j term's first product and average.i) and the tail's j product.
// Compile-only, those two unchanged: cross in six parameter orders, arrays, references, `(real)` products, existing cross3f (same
// code), named terms 33-68, a and b declared at function top, in either order, per arm 103, at each use 137, 13 declaration moves
// in pairs; tuple shifts before and after the cross 24-62; b computed first 217 (every product then takes b first).
// Lane 9 r89 (fast; masked counts in brackets, base 122 [59]; body unchanged, all pieces below are match-only): velocity first in the
// parameters (retail sets it last at the call) 120, on the 21 lead 15. On the lead: identity locals at the top 1-7 21/19/23/34/17/13/51; dot_average
// as three statements and dot_a through dot3f(a, desired) 15, with six identity locals 5 [3]: only the tail's j product is left, load-first where retail is copy-first.
// That product follows only a tuple step in the loop or case 3 (`(real)(x * 1.f)`), which breaks the arms: 53 at 977 bytes with 0x1f087b arms [34], arm
// compute/store orders 53-197. Tail written out, compound, staged, scaled, no inv local, in-tail steps 5 each; early return 27; normalize3d 273; 0x30bf0 262.
// Up getter at every read 177; typed component and element fields 181; normal, height, stepping getters 178; identities around a and b 197-224.
// Lane 9 r98 (fast, control 43): both dots written out k, j, i unparenthesized 73 (the dot code is unchanged, the selected arms stop
// merging); a forceinline contact-entry getter (component, index) at all six entry reads 43 (same code), with case 3 selected-first 178
// (the same 178 as without it: the getter shifts nothing); case 2 averaging arm and case 3 selected arm both first 215; case 3
// selected-first with case 2's `selected != 0` arm first 199. Body restored.
// Lane 1 r83 (fast, 0x1201a0/0x1f2e60 matched in each; al = aligned lines left / the same with registers masked, control 43, al 20/15):
// re-measured on today's body: case 3 selected-first 178 (991 bytes, al 52/32), with no none-arm copy 192 (988, al 53/33), case 2 with
// the averaging arm first 222 (992, al 92/82; was 187 on the 122 body), `b` declared before `a` 229 (992, al 74/36; was 228), both dots
// as dot3f(vector, desired) 43 (same code), contact_cross staged through three locals 47. New: the averaging arm first with case 3
// selected-first 215 (1003); contact_cross with every product commuted 43 (same code), reading through two references 43; a pointer local
// to the up vector scoped to case 3 on this none-first body 240 (1010, al 60/30; r135 had it on the selected-first body), with the average
// through vector3f_add 217; the average through the existing vector3f_add(b, a) or (a, b) on this body 67/67 (974, al 24/20); `average`
// declared at case scope 43 (same code); the dots inside the compare 43 (same code); case 1 through a pointer local 225 (995). Retail's
// only cross difference is a.k * b.i with b.i loaded first, and its average.i is b.i + a.i as written where ours loads a.i first: the
// written operand order does not reach either.
// Round 161 (m3 lane 5, fast with 0x1201a0 and 0x1f2e60; aligned in brackets, control 43 [20]): the extra byte is the inlined
// 0x1201a0 tail's j product (ours `movss; mulss xmm0` 9 bytes, retail `movaps xmm1, xmm0; mulss [ebx + 4]` 8). Case 1's copy
// through a named element pointer 225 (995 bytes) [42]; the stepping flag named in the loop 43 [21]; case 2's selected arms as
// one cross3f_staged call with ternary operands 213 (954 bytes) [65]; the stepping flag as `& 2` 43 [20] (same code); case 3's
// k row as `normal->k * scale` 43 [20] (same code).
// M3 lane 2 r67 (fast with 0x1201a0/0x1f2e60; aligned/masked in brackets, control 43 [32/26]): retail keeps the entry's
// g_4687b0 register for cases 1 and 3 and reloads it in case 2's arms. r135's lead (case 3 selected-first with an up
// pointer read once for case 3) re-measured 21 [27/21]; forms listed above re-measured on it: `0.0f - normal->k` 21, the
// k row as `normal->k * scale` 21,
// both dots as dot3f(vector, desired) 21, the first cross through the existing cross3f 21 (all same code), the average
// through the existing vector3f_add(b, a) 47 (974 bytes) [27/25]. Left on the lead: the first cross's a.k * b.i and
// average.i operand order, both dots' term order and the tail's j product.
// Round 226 (lane 3, fast; aligned exact/stack/regs in brackets, control 43 [33/27/27]): lane 9 r89's match-only lead rebuilt (case 3
// selected-first with a case-3 `up` pointer at its four reads, both dots as statement sequences) 9 [8/8/6]: left are the first cross's
// a.k*b.i and average.i (b.i loaded second) and the inlined 0x1201a0 tail's j product. On it: the first cross written out 9, all
// three crosses written out 9 (inline helpers move nothing); case 3 through vector3f_add_scaled 9 (same code), `0.0f - k` 9 (same
// code), no scale local 302 (1016 B); dot3f(desired, x) or dot3f(x, desired) for both dots 21 [28/22/22] with any case-3 form.
// Diagnostic grid on the written-out lead (no-op `(real)(x * 1.f)` chain of depth d3 on case 3's scale, d2 on the first cross's i row;
// not keepable): (d3,d2) (1,0) 197, (2,0) 157, (3,0) 9, (0,1) 198, (0,2) 178, (0,3) 9, (1,1) 177, (1,2) 10 at 977 B, (1,3) 197,
// (2,1) 35, (2,2) 193, (2,3) 193, (3,1) 198, (3,2) 198, (3,3) 178. At (1,2) the tail's j product and both selected arms are
// retail's; left are five product operand orders (the first cross's i and j first products, average.i, dot_a's k and j), and
// writing those five with their operands swapped gives identical code. The two sites do not add: (3,0) and (0,3) are 9 but (1,3),
// (2,2), (3,1) are about 195, so no single natural step at one place will do.
// Also round 226, on the 43 body (fast): one `up` pointer read before the entry copy and used at every later global read (entry copy,
// case 3, the 0x1201a0 fallback) 234 (996 B); with case 3 selected-first 214 (990 B, re-measures r135's 214); a case-2 `entries` base for
// a, b and the normal 222 (986 B); `up` and `entries` 222; all three 199 (980 B); the loop's `z` reference dropped, the height field
// read at each of its three uses 71 (974 B).
// Lane 10 r91 (fast; aligned/masked lines in brackets, control 43 [32/26]): the first cross as contact_cross(b, a) with the two
// selected arms' operands swapped (same result) 201 (991 B). Velocity declared first (the only register argument, set last at
// 0x1f2250's call; definition and call changed together) 37 [18/13]: dot_a then sums i, k, j as retail; left are case 3's arm
// order, the first cross's a.k * b.i, average.i, dot_average (ours i, j, k, retail i, k, j) and the tail's j product. Six
// points, below the eight-point bar above 30, so not kept. On it (re-measures, new base): case 3 without the none-arm copy 192,
// average as a + b 37, selected declared first 39, dots inside the compare 37, dot3f(x, desired) 37, average through
// vector3f_add(b, a) 64, `0.0f - normal->k` 37, no index local 65, the first cross through cross3f or cross3f_staged 37/37, the
// selected arms through contact_cross or cross3f 200/200; case 3 selected-first 178 in every case order (1S2, S21, 2S1, S12,
// 21S: case 2's selected arms stop merging, 991 B), none-first orders 132/321/213 41/41/37; the loop's `z` as a normal pointer
// read at its three uses 240 (1010 B); `selected != 0` arm first 199. No natural untried form left that I could find: the rest
// follow the operand-order phase mapped in lane 3 r226's grid.
// Lane 4 r186 (fast with the file's 13 functions and 0x1e5af0/0x1201a0/0x23de50; aligned/masked lines in brackets, control 43 [20/15]):
// kept: case 3's rows as the existing vector3f_add_scaled(g_4687b0, normal, -normal->k, out) (re-measure of round 219/226's form, byte-identical;
// the rows were that helper's body written out). Velocity declared first re-measured 37 [15/12] (six points, under the bar); on it,
// byte-identical to it: dot_average first, the k row as `normal->k * scale`, `average` at case scope, case 3 through vector3f_add_scaled,
// dot_average alone as dot3f(&average, desired), a and b as `vector3f const &` references (identical on main's body too), the average as
// an aggregate initializer. The switch as an if/else-if chain on count 186 (979 B), with velocity first 186 (code differs). Diagnostic
// re-measures: velocity first with case 3 selected-first 178 (case 2's first cross recolours: case 3's blocks come ahead of case 2's), with a
// case-3 `up` pointer 15 [12/9] (case 3 exact; left the first cross's j-row first product, dot_average's grouping and the tail's j product).
// No untried natural form found that changes this function's code.
// M3 lane 15 r76 (fast with the file's functions and 0x1e5af0/0x1201a0/0x23de50/0x30bf0; aligned/masked in brackets, control 43
// [26/26]): both dots as statement sequences in retail's (i + k) + j order on this body 31 [12/10] (dot3f's body by hand, match only).
// Re-measured: the match-only lead (case 3 selected-first with a case-3 up pointer at its four reads, plus those dots) 9 [7/5], the
// pointer alone 21, case 3 selected-first alone 178. On the lead: the average as an aggregate initializer or through
// vector3f_add(b, a)/(a, b) 9/9/9, the first cross through cross3f or cross3f_staged 9/9 (all the same code), `selected` declared
// before `lowest` 11. Left on the lead, unmoved by any of these: the first cross's j-row first product (ours a.k, retail b.i), average.i
// (ours a.i first, retail b.i) and the inlined 0x1201a0 tail's j row (ours loads v.j, retail copies inv). Permuter (--fast --anneal,
// 600 s): 57 tries, nothing better; its reduced image scores this body 117 against the full image's 42. On this body: the loop test
// as `z < lowest` or `-0.999f < z` 43/43 (same code); case 2's selected normal as `selected == 0 ? a : b` 261 (983 bytes); the two
// selected arms crossing with a and b directly (no normal lookup) 258 (980); `selected >= 0` for the case 2 test 192 (975); the
// component index as a dword 43 (same code).
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
            if (selected != NONE)
            {
                vector3f const *normal = (vector3f *)(*(byte **)(component + 0x88) + indices[selected] * 0x48 + 0x28);
                vector3f axis;
                contact_cross(a, b, &axis);
                if (selected == 0)
                    cross3f_staged(&axis, normal, out);
                else
                    cross3f_staged(normal, &axis, out);
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
                    *out = *a;
                else
                    *out = average;
            }
        }
        break;
    case 3:
        if (selected == NONE)
            *out = *g_4687b0;
        else
        {
            vector3f const *normal = (vector3f *)(*(byte **)(component + 0x88) + indices[selected] * 0x48 + 0x28);
            vector3f_add_scaled(g_4687b0, normal, -normal->k, out);
        }
        break;
    }
    function_1201a0(out, g_4687b0);
}

PRIVATE __forceinline byte function_1f1462(dword arg_0)
{
	return (arg_0 >> 3) & 1;
}

// score 163 (was 163; 163 at r76; 162 with the one-use settings copy dropped, not kept): 1229 bytes, frame 0x28 and the EDI/EBX convention agree; left are up's registers (XMM4-6, retail XMM5-7) from the state dot on and the rotation's schedule.
// MiniMax H (11-11): no new form, no lower score (163); the state dot still takes its registers from XMM4-6 where retail uses
// XMM5-7, and the rotation block still schedules its products differently.
// score 163 (was 163; 162 with the one-use settings copy dropped, not kept): 1229 bytes, frame 0x28 and the EDI/EBX convention agree; left are up's registers (XMM4-6, retail XMM5-7) from the state dot on and the rotation's schedule.
// MiniMax R (11-06): no new form, no lower score (163); a compile-only probe of the same three-term float sum
// gives retail's (k + j) + i order when the three products have fresh component nodes, so the
// state dot's shared ground-normal loads cannot reach that order from the forms the note below records.
// MiniMax R (11-06): the state dot written i, j, k reaches 162 (the products pair old_up->x with up->x as retail's
// do); the parenthesized (k + j) + i form of the same dot 176, the parenthesized rotation dot 235, the parenthesized
// final test dot 235; the one point is below this round's eight-point bar, so the original body was restored.
// Round 219 (lane 3, fast, 0x1e5af0 listed; the arm-order and staged-cross levers that took 0x1f2a80 to 43): `if (!(dot > 0.0f)) axis = *old_up;
// else rotate = false;` 163; the same as `dot <= 0.0f` 164; fallback stores forward first 186 (1230); up result stored first 196; inactive
// arm's up store first 183 (1230); all five crosses through cross3f_staged or cross3f, the first only, the two rotation crosses only, the
// four final ones only 163 each; the outer test inverted with the inactive arm first 302 (0x1e5af0 36).
// Lane 9 r89: named old-up pointer with the parameters as retail's call sets them (forward at entry, up pushed, ground last) and state last 163 with 0x1e5af0 still matched;
// that pointer with state first in six orders 196 (0x1e5af0 20-22), other orders 163 (g,f,s,u; g,f,u,s; f,s,g,u; f,g,s,u; f,g,u,s) with 0x1e5af0 2 for u,g,f,s; u,f,s,g; u,f,g,s; f,s,u,g; f,u,s,g; f,u,g,s.
// On it: state dot written i + j + k 162 (retail's instruction order; one point, not kept), other term orders 163-165, through dot3f 253 (1242); rotation rows and dot orders, 18 forms, 228-248 [73-103];
// rotation cross components and rows, 36 orders, 162-250 [76-]; identity locals 1-7 163/302/173/162/162/162/162; no angle local 175; settings alias removed 162, old-forward alias removed 163, both 162;
// old-forward pointer beside old-up 310, in the other arm 217 (0x1e5af0 20); named axis length 176.
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
// Round 481: Complete fallback plane/forward candidate with publication after normalization,
// eight independent fallback/forward-phase/active-entry declaration pairs 247 each (1243 bytes).
// Original in-place fallback and full body restored.
// Round 191 (lane 3, fast): `vector3f up;` then `up = ground->normal;` 182 (same code); up copied
// component by component 293 (breaks 0x1e5af0); the old-forward pointer declared before `up` 297 (1229
// bytes: the first cross then reads the old up through ESI with up as the destination, the state dot
// keeps [edi+0x104/0x108]), declared after `up` 297 (1229), declared at function scope 288 (breaks
// 0x1e5af0). Retail keeps up in XMM5-7 through the state dot and the forward cross and multiplies the
// forward from memory into its own registers; ours keeps up in XMM4-6 as the destinations.
// Round 232: Angle initialized before limited copy, declared before up/beside axis/
// at entry 190/182/182/182; those four with named cosine complement 188/180/180/180.
// Two-count best rejected; original angle lifetime, expressions and complete body restored.
// Round 235: shared initial dot with settings/forward/both receiver removal, then
// each cosine-complement interaction and two dot declaration-scope combinations:
// 262/262/266/262/262/262/266/262. All lose the caller match; original dot,
// receivers, scalar graph, helper declaration state and complete body restored.
// Round 481: Complete fallback plane/forward candidate with publication after normalization,
// eight independent fallback/forward-phase/active-entry declaration pairs 247 each (1243 bytes).
// Original in-place fallback and full body restored.
// M3 lane 8 r135 (fast; aligned in brackets, base 182 [214]): diagnostic tuple phases 183/186/186 [200/207/207]; rotation rows
// and cross components written out, 108 orders compile-only, best real rows j,k,i with cross j,i,k 242 [184], cross k,i,j 260
// [192]. The first cross's `state + 0x100` temp is retail's ESI; retail keeps it across the first call for the state dot,
// but every form that does (dot3f in both argument orders 262 [196], a named pointer at each use 265 [203], each with the
// phases 264-268 [188-203]) colours the state EBX and the forward result EDI (retail EDI/EBX) and drops 0x1e5af0 to 20. On
// dot3f: settings or old-forward alias removed, result stores swapped in either arm, all 16 subsets 260-267, caller still 20.
// Lane 9 r98 (fast, 0x1e5af0 matched in each): the state dot with `up` first in each product 163 (same code); the rotation dot
// with axis first in each product 164; the rotation test dot through dot3f(&cross, &axis) 163; the rotation rows written cosine
// term first (`limited.k * cosine + axis.k * dot - ...`) 163; the final crosses' workspace renamed `side` 163. Body restored.
// Lane 1 r83 (fast, 0x1e5af0 matched in each; al = aligned lines left / the same with registers masked, control 163, al 134/55): sinf and
// cosf for the rotation, either or both 176 (1233 bytes, al 131/49); the three dots parenthesized (k + j) + i, state/rotation/final-test
// subsets 1-7 176/236/236/176/176/236/236 (al 131-133/48-50; the state dot's loads still start at j; the three single ones re-measure
// the 11-06 forms at 176/235/235, the pairs and the triple are new). Retail's forward cross and its
// fallback cross both multiply in the helper's written order (first argument's component first: memory times up's register in the first,
// up's register times memory in the second); ours has the second argument's component first in both. contact_cross with its parameters
// as (out, a, b), (b, a, out), (out, b, a), (a, out, b), (b, out, a) 163 each (identical code, diagnostic through a macro); every
// product commuted in the helper 163 (identical code, 0x1f2a80 43); the helper staged through three locals 163 (0x1f2a80 47, 0x1f0870
// 562, 0x1f2250 418); the helper reading through two references 174 (1231, al 133/52). Body unchanged.
// Round 226 (lane 3, fast, 0x1e5af0 matched in each): `up` renamed ground_up, new_up or normal, `limited` renamed rotated, `axis`
// renamed rotation_axis: 163 each, identical code (names do not move up from XMM4-6 to retail's XMM5-7).
// Lane 10 r91 (fast, plain for the kept form; masked = aligned lines with registers masked, control 163 [85]): the one-use
// `settings` copy of the state's settings pointer dropped (read in the bit test) 162 [82], plain-confirmed, 0x1e5af0 matched; not kept (1 point, under the keep bar).
// Before that, on 163: the rotation dot named as `projection` and scaled by `1.0f - cosine` after the angle or after the sine
// 184/184 (1232 B), after the cosine 163 [82]; on that, the state dot written i + j + k 162 [79] (retail's instruction order; up
// then sits in XMM6/5/4 for k/j/i where retail has XMM7/6/5, so something else holds XMM7 across that block), with the projection
// written i + j + k 232, with the rotation's cross reading old_up for the still-equal `limited` 247, with the settings copy dropped
// 162, the old-forward pointer dropped 173, both 162.
// R229 (lane 3, fast, 0x1e5af0 matched unless noted): re-measure of lane 9 r89's state dot written
// `old_up->i * up.i + old_up->j * up.j + old_up->k * up.k` on the current body 162 (that block is then retail's instruction
// for instruction, loads k, j, i and (k + j) + i; only up's XMM4-6 against XMM5-7 is left there; one point, not kept); on
// it the rotation dot written i + j + k 248 (1243 bytes: (k + j) + i as retail but limited.i folded from memory; aligned
// 212 -> 194, register-masked 82 -> 71), the rotation test dot written i + j + k 162 (same code), both dots 248, and
// (re-measure of round 232's angle before the limited copy, 190 then) `angle` declared before `limited` 172. On the
// original body: the double fallback writing both outputs and returning 271 (1254 bytes, 0x1e5af0 20); the forward cross
// computed straight into `forward_result` (no `forward` local) 315 (0x1e5af0 20). Diagnostic, removed: 1-16 unused locals
// before `old_forward`'s initialisation change the code only at 3 and 11 (174) and never turn the forward cross's folded
// [edi+0xf4..0xfc] operands into retail's separate loads.
// Lane 4 r186 (fast, 0x1e5af0 matched in each; control 163 [134/55]): `double angle` 163 (the rotation schedule moves, same count); the
// rotation dot scaled in a second statement `dot *= 1.0f - cosine` 176 (1233 B); the rotation cross computed before the rotation dot 237
// (1239 B); the rotation as axis x vector added (`contact_cross(&axis, &limited, &cross)`, `+ cross * sine`) 234 (1233 B); the acceptance
// test as `contact_cross(&up, &limited, &cross)` with `< 0.0f` 231. Same code: the angle as `5.235987663269043f * rate`; the rotation dot as
// `(1.0f - cosine) * (...)`.
// M3 lane 15 r76 (fast, 0x1e5af0 listed; aligned/masked in brackets, control 163 [222/85]): the state dot reading ground->normal
// directly instead of the up copy 304 (1236 bytes, 0x1e5af0 36); the rotation angle computed at the top of the active arm 314 (1249,
// 0x1e5af0 36); the rotation written from old_up into an uninitialised `limited` (dot, cross and rows read old_up) 250;
// `rotate = !(dot > 0.0f); if (rotate) axis = *old_up;` 163 and `bool rotate = true;` before the axis cross 163 (both the same
// code). Re-measured: `angle` declared before `limited` 173. Also: the inactive arm's forward built in a local and stored once
// with the up store after it 311 (1242 bytes, 0x1e5af0 43); the state test as `0.0f < dot` 163 (same code).
// @retail 0x1f1460
void function_1f1460(s_shape_state const *ground, vector3f *up_result, vector3f *forward_result, byte const *state)
{
    byte *settings = *(byte **)(state + 8);
    if (function_1f1462(*(dword *)settings))
    {
        vector3f const *old_up = (vector3f const *)(state + 0x100);
        vector3f up = ground->normal;
        vector3f axis;
        contact_cross(old_up, &up, &axis);
        bool rotate = true;
        if (function_30bf0(&axis) == 0.0f)
        {
            real dot = old_up->j * up.j +
                old_up->k * up.k +
                old_up->i * up.i;
            if (dot > 0.0f)
                rotate = false;
            else
                axis = *old_up;
        }
        if (rotate)
        {
            vector3f limited = *old_up;
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
            contact_cross(&up, old_up, &cross);
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

/* retail sums these dots (k + j) + i (0x1f1202-0x1f12be in 0x1f0870, 0x1f2981-0x1f29af in 0x1f2250); the
   parentheses keep that grouping, an unparenthesized k, j, i chain is regrouped (i + k) + j by the compiler */
PRIVATE inline real shape_dot(vector3f const &a, vector3f const &b)
{
    return (a.k * b.k + a.j * b.j) + a.i * b.i;
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




// score 347 (was 347; round 95 m3 lane 11 near the end, m3 lane 2 r79 and m3 lane 8 r164 at the end): before r723 358; retail inlines the existing magnitude3d (0x24590) for both lengths: its out-of-line
// MiniMax G (11-12): the flag order flying, direct, damping, gravity reaches 343 (the damping bit then lands in ECX where
// retail uses EDX), the direct arm's product sides and its speed load, the crosses' operand sides and the general arm's
// history block are unchanged; a four-point gain is below the eight-point floor, so the body is restored.
// Earlier 347 (was 347): before r723 358; retail inlines the existing magnitude3d (0x24590) for both lengths: its out-of-line
// 33 bytes equal retail's input-length block, and `magnitude3d(&change) + g_47ffb0` alone makes the history block
// (0x1f0fd0-0x1f10ab) byte-exact, registers included: 573 (3038 B; aligned 152 -> 165, masked 64 -> 73), the product operand
// orders in the crosses and the low arm flipping instead. With the input length as `magnitude3d(&state->input)` too, the input
// block is exact as well, 583 (3027 B; aligned 177, masked 80; the low arm 18 B short); through the `input` pointer local the
// inlined sum moves the offset-0 term last, (j2 + k2) + i2, 585 (3027 B). On that both-sites form (r170, plain): the flags
// declared flying, direct, damping, gravity 579 (3027 B, first difference +0x31); no `input` local, every read through
// state->input, 573 (3033 B). Lead: keep both magnitude3d calls in any rewrite; the product operand order moves with each
// added inlined call, so look for what else retail inlines here.
// no lower form in round 170 (m3 lane 6); see that round's line at the end of this note.
// MiniMax C (11-06): the direct arm's magnitude and the history arm's maximum now add their three squares in
// retail's term order and grouping (344, 3052 bytes against retail's 3047). Left: the magnitude's x87 stack still
// shuffles differently (ours fmulp/faddp/fxch/fstp against retail's fmul/faddp/fstp), the direct arm's four
// products and its two vector homes sit in other registers and stack slots than retail's, and the crosses' product
// sides and the low-normal arm's homes remain as noted below.

// Earlier 347 (was 347; before r723 358): shape_dot now sums its dots (k + j) + i as retail (#1102, r723). Before that, 358 (was 344): 3049
// Earlier 347 (was 347): shape_dot now sums its dots (k + j) + i as retail (#1102, r723). Before that, 358 (was 344): 3049
// bytes (retail 3047). Statement-by-statement walk against retail (b2K, 2026-11-06, fast): every
// arm, compare direction, field offset (flags 0x14, settings 0x8, speed 0x20, normal 0x8c, damping 0x98, facing 0x9c, flag 0xa8,
// up 0x100, basis 0x118, velocity 0x124, gravity 0x130, input 0x138, accelerations 0x144/0x148), constant (0.001, 0.234, 5.0, 1.0,
// 0.5 read from the XBE), callee and argument agrees with retail; what is left is operand/register/stack-slot scheduling (delta and
// limited homes swapped in both arms, arm 1's z store not tail-shared with the general arm). The one class of behaviour difference
// found was float summation order: VC re-associates an unparenthesised a + b + c, parentheses are kept, and retail's order is
// (i2 + j2) + k2 for the input length (0x1f0b07-0x1f0b22), (j2 + k2) + i2 for the history change length (0x1f0fe1-0x1f1047),
// (in.j*side.k + projected.k) + in.k for the low arm's desired.k (0x1f1327-0x1f1337); those three are now parenthesised
// (343 / 345 / 364 alone, 358 together; the history length costs the 14 points, ours came out (i2 + k2) + j2 before).
// Done in #1102 (r723, outside this function): the low arm's two plane dots go through shape_dot, whose a.k*b.k + a.j*b.j +
// a.i*b.i compiled as (i + k) + j where retail (0x1f1202-0x1f12be) sums (k + j) + i; shape_dot is now
// `(a.k * b.k + a.j * b.j) + a.i * b.i`, retail's order: 347 here, 0x1f0450 MATCH. Tried and dropped for the input
// length: magnitude3d(input) (0x24590 is exactly retail's block, inlines with the (j2 + k2) + i2 order, 403, 3038 bytes), dot3f 403,
// length_sq3f 403, six unparenthesised spellings 403, three scalar locals loaded k, j, i reproduce retail's x87 block exactly
// (3047 bytes) but are steering locals, 568. The (real) on the history sqrt rounds nothing in ours either: both keep the
// sqrt unrounded into the fadd, and the first sqrt and the maximum are stored once (fstp dword) as in retail.
// Earlier 416 (was 433): 3038 bytes (retail 3047), frame 0x30; left are product sides and register homes in the crosses, the
// direct arm's rows, the low-normal arm and the history row. Closest form so far: `input->i` for `state->input.i` in the direct
// arm's rows and the low arm's projected rows, one settings local for the ground compares: 433 at 3047 bytes, aligned 138 and the ground
// chain exact (mixed reads, not kept).
// This round (fast, positional, with aligned count stack-masked): two normalize calls and the ground arm after the return, as retail
// 616 (the listed separate normalization; aligned 624 in a compile-only harness from 853); flags read from the field at each
// use, one load 607; `(vector3f *)(out + 0xc)` at each use stops the store forwarding; 0xa75d0 inline, xmm and edx not kept
// across it 340 at 3047 bytes; `input` pointer declared first; rotated point2f 439 before the inline; shared rotate/combine
// helpers 638; speed declared before desired 500; `magnitude = speed * magnitude` as the scale 433 (ground chain exact).
// On that 433 form: all 24 parameter orders 433 or 552 (3040 bytes; sides do not move; half lose 0x1f0450); facing pointer in the
// direct arm 447, with the speed first 555, the speed first alone 549; every input read through the pointer 537 (2992 bytes),
// through the field 450 (3050); the direct arm's read through the field alone 461, the low arm's 449, both 423 with one settings
// local for the ground compares and field reads in the two interpolations (a mixed read, not kept); the settings local everywhere 464
// (3035, aligned 211); the field at each use 433 (3037, aligned 270, kept).
// On it: a new `speed` local for the scale 435 (3035), the scale as the helper's argument 473 (3031).
// Earlier 590 (was 590): frame 0x34 versus 0x30 and direct/vector homes remain; retail threads the
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
// Round 448: Shared direct/general, direct/history, general/history and all-phase
// maximum-change scalar 591/590/590/591; sole mode read at its test on those
// graphs 593/592/592/593. Original limits, mode, vector workspaces and body restored.
// Round 288: Continued magnitude/input squared sum/both/history squared sum/magnitude+history/input+history/all/all with entry magnitude,
// 593/590/593/590/593/590/593/593 (fast). Original complete body, observations, helpers and call roles restored.
// Round 296: Actual bounded-step result long/dword/short/word at its producer, then the same four entry
// declaration lifetimes. Scores 590/590/590/590/590/590/590/590. Original complete body restored.
// Round 232: Branch-owned initial gravity/general-only damping/general-only flying
// scope subsets and complete graph with early bounded-step declaration 610/629/638/626/628/634/637/637.
// All restored; original frame, flag observations and shared normalization retained.
// Round 235: complete movement-scale/history-anchor/both/current+scale/current+anchor/
// all/current/all with split anchor assignment: 623/665/672/627/661/666/621/666. Current-value
// forms revisit the clean store graph; original observations and full body restored.
// Round 296: Actual bounded-step result long/dword/short/word at its producer, then the same four entry
// declaration lifetimes. Scores 590/590/590/590/590/590/590/590. Original complete body restored.
// Round 257: Native coordinate consumers for general delta/bounded scale/publication, seven subsets,
// then all with history publication: 587/587/587/752/752/752/752/764. Individual loops reach
// retail length at 587; the three-point gains are rejected. Original complete body restored;
// frame 0x34 and direct/vector homes remain.
// Round 311: General movement through the existing output-pointer scale, plain/shared difference and
// ordinary/returned-output ownership 590 each; all four with native two-component direct delta 589 each
// (one-point gain rejected). Original body restored; frame, flag homes and normalization continuations remain.
// DL238: Native grounded/gravity-disabled/direct-disabled predicate subsets 1/2/3/4/5/6/7/all plus
// damping-disabled: 604/606/614/605/612/614/627/629. Mutable response-word gravity/plus
// damping/plus direct/all 631/646/645/669, each loses matched 0x1f0450 in reduced controls.
// Complete original body and helpers restored; frame 0x34/0x30, flag and vector homes remain.
// Round 534: complete predicate/direct raw-bounded/general raw-bounded roles, seven
// subsets then early uninitialized direct bounded workspace: 433/433/433/433/433/433/433/433.
// Original complete body restored; damping carrier, raw/bounded homes and product sides remain.
// DH297: Complete direct/positive-normal/low-normal product groups, subsets 1-7,
// then column-owned planar terms: 556/556/437/473/425/417/473/473. Best: the positive-normal and both
// low-normal product groups as ten one-use named product locals, 417 plain (3038 bytes; first difference
// at +0xd, damping in ECX/EDX). NOT kept: one-use temporaries that only steer evaluation order are kept
// only with a match; body restored at 433.
// DL249: Complete existing setters for projected-forward/projected-side/general raw-delta,
// subsets 1-7, then all with history difference: 473/473/431/463/413/411/465/463.
// NOT kept: joint projection gains two; pairs with raw setter gain only in combination
// while their parts regress. Exact-match-only steering; original body and helpers restored.
// F335: native direct clip predicate/direct fallback first/general fallback first, subsets 1/2/4/3/5/6/7
// 360/432/498/359/482/497/481; all with named normalization return 470. NOT kept: one-use
// predicate steers allocation; fallback alone gains one. Frame 0x30 agrees, raw/bounded homes still differ.
// B374: planar bounded producer with native/library raw backup before/after step, alone or
// with general producer: 393/385/388/398 twice; 3034-3035/3047 bytes, frame 0x30.
// NOT kept: reversed producer and length farther from retail; original raw-first body restored at 433.
// DH305: Complete state/settings words and mask predicates on the planar/eight-normalization
// graph, seven subsets then opposite complete-word declaration order: 526/610/562/495/573/609/559/560.
// NOT kept: retail reads both words again at history admission; unsupported observation/predicate
// steering needs a match. Original body restored; damping and raw/bounded homes remain.
// N334: Plain vector component loops at flying/paired ground projections/low-normal output,
// subsets 1-7 514/467/514/418/526/533/616; complete unsigned ordinals 616. NOT kept: the
// low-normal output loop alone 418 plain (frame 0x30, 3037 bytes) is one of seven subsets picked by
// score, the like rows beside it stay written out. Body restored at 433; damping and vector homes remain.
// AC492: Twice-used general normalization return, real/double at producer/entry
// 488/496/488/496; complemented clipping result 433; direct producer with general return 485;
// direct branch-owned result alone/with complemented general result 345/345 (3050/3047 bytes).
// NOT kept: one-use call-result carriers require a byte match. Original complete body restored;
// damping ECX/EDX and raw/bounded homes remain.
// DL251: Plain general bounded-scale/normal-correction/history-publication component loops,
// subsets 1/2/4/3/5/6/7, then unsigned ordinals: 433/433/435/499/433/433/500/500.
// Original complete source restored; no eligible gain.
// B389: Complete native aggregate producers: direct rotation/raw/working, general working/raw,
// direct raw+working, general raw+working and all: 433/433/497/551/433/497/551/584.
// Original raw-first body restored; damping and raw/bounded homes remain.
// DL253: Independent shared general/history output sums 473/435; history difference/offset/relative
// setters 431/433/433; flying desired setter 428; direct two-point difference 572; general unity
// scaled difference 463. NOT kept: gains two/five below the bar; flying setter also shortens away
// from retail. Original raw-first body restored; damping and raw/bounded homes remain.
// B391: Actual external normal/facing/input identities, subsets 1/2/4/3/5/6/7, then
// all with up identity: 346/509/533/477/511/587/513/511. NOT kept: the normal-field
// reference reaches 346 at 3052 bytes by changing existing product scheduling; retail
// does not show that new field-alias lifetime or its projection load pattern. Exact-only
// identities and complete body restored; damping and raw/bounded homes remain.
// Lane 18 r41 (fast): the direct arm's speed read from the field at each use 443 at 3047 bytes (x computed first as
// retail, register-masked aligned 234 -> 143, but speed loads twice where retail loads once), with the step computed
// before the delta 471; the speed local declared first in the arm 480 (3034); the delta products written rotated-first
// 433 (same code); the direct arm's limited renamed to sort before delta, or in both arms, 433/443 (no slot moves:
// retail keeps delta below limited in the direct arm and above it in the general arm, ours the reverse in each).
// Y562: Planar/eight-normalization branch-owned damping/flying/direct admission, subsets
// 1/2/4/3/5/6/7, then late damping: 574/530/629/658/633/639/652/659. Original entry observations and whole body restored.
// NOT kept: all regress; combined predicate scopes also lose the exact caller.
// Y564: Planar/eight-normalization initial/fallback/both scalar direct copies
// 497/593/594; initial/fallback/both library copies 433/433/433; both scalar
// copies with native raw aggregate 594; both overlap-safe copies 586. NOT kept:
// no gain; original full body, raw/bounded homes, helpers and observations restored.
// DL256: Existing cross/dot expansion pairs forced-inline/inline, inline/forced-inline, both forced,
// ordinary/inline, inline/ordinary, ordinary/forced-inline, forced-inline/ordinary and both ordinary:
// 433 each (3037/3047 bytes); every shared user unchanged. Original annotations and full body restored;
// damping ECX/EDX, raw/bounded homes and cross/history schedules remain.
// B409: Declared word-bit consumers for damping/direct/gravity/the three/enabled/
// all four/all with history/the three with history: 427/438/427/425/441/457/575/444.
// NOT kept: six-point leads below gate; combined 425 shortens away from retail to
// 3034/3047. Original word layout, flag producers and complete body restored.
// B411: Complete entry flying/damping/direct/gravity declaration orders on the planar graph
// 441/432/441/434/444/445/445/440; NOT kept: one-point reduction and order-only nonmatches.
// Original observations/body restored; damping ECX/EDX and raw/bounded homes remain.
// M354: Split damping extraction in byte/dword storage, mask at producer/entry/general/consumer:
// 427/412/627/532/464/451/630/557. NOT kept: split predicate transport needs byte equality.
// DH328: Existing staged force-inline cross at disabled/flying/ground subsets
// 436/473/429/436/431/479/431; shared staged cross at every remaining site 431.
// NOT kept: four/two-point leads fall below threshold; length regresses or ties. Full body restored.
// DL261: Ordinary/forced cross templates; native response/history views; actual response/history/both formals: 1135/1135/1135/1135/1135/1135/426/417/433/433/433.
// NOT kept: no exact native form; original complete body and all temporary contracts/types restored.
// M371: Existing coordinate-difference helper at history input/anchor/relative,
// subsets 1/2/4/3/5/6/7, then all with general raw delta: 423/433/435/423/435/441/441/465.
// NOT kept: 423 shortens farther from retail (3034/3047); remaining forms tie or regress.
// Original whole body and helpers restored; damping and raw/bounded homes remain.
// Lane 9 r87 (fast; aligned counts by sequence alignment, base 433/344): up, facing and the low arm's normal read from the field at each
// use, speed read first in the direct arm 416/311 (kept). Each alone 423/313, 424/308, 415/326; the same three with no speed local at
// all, 421/167 at 3050 bytes (the closest shape, retail's rotated roles in the direct arm; speed loads twice where retail loads once);
// speed local after rotated 460/356; speed alone removed 443/189 (3047). Identity pointer locals at the top, 1-7, on the base
// 428/427/431/559/420/473/423, repeating every 8, with no speed local 423/424/427/561/558/548/429 (548 is 3047 bytes, aligned 157), with
// speed first 416/496/593/583/502/499/492. rotated.x and y as scalars 581/560; product sides in delta or limited declared first 421.
// On the kept form: restoring up, facing, the low normal, up and facing 416/416/420/499; the low facing removed too 447.
// J254: Plain history input/anchor/relative component-array producer loops, subsets 1/2/4/3/5/6/7 then
// unsigned ordinals: 601/556/556/469/469/563/546/546. All preserve matches but grow to 3090-3103
// versus 3047. Original complete body restored; damping ECX/EDX, raw/bounded homes and
// cross/history scheduling remain.
// Lane 9 r89 (fast): the layout already follows retail's calls and arms line for line. Flags in all 24 orders: flying, direct, damping, gravity 412 (entry block retail's but
// one store; a gain of 4, not kept), flying, direct, gravity, damping 415, others to 500. On it: input pointer after the flags 417 (entry exact, 3037 bytes), first arm's cross through
// 0x1f087b 577, staged 573, up pointer 419, facing pointer 417; identity locals 1-7 415/490/584/574/493/493/491 (register-masked aligned 182/174/182/151/128/129/171 from 173); scaled before left 430;
// step after limited 412 in the direct arm, 422 in the general arm; desired before magnitude 412; scale expression as the helper's argument 493. Base identity locals 416/496/593/583/502/499/492.
// Round 91 (m3 lane 9, fast): the shared cross helper's products written with every operand pair swapped, only the
// second of each pair swapped, only the first swapped 416/416/416 (same code everywhere: VC orders the product
// operands itself; retail puts up.k ahead of facing.i/j where ours puts it after); the flags declared in retail's
// computation order (damping, direct, gravity, flying) 427; the input pointer declared after the flags 421 (3037
// bytes). The call sequence (eight 0x30bf0 calls, 0xa75d0, both `ret 8`) and the block layout agree with retail.
// Round 226 (lane 3, fast, base 347; 0x1f0450 matched in each): re-measured on the 347 body, the flags declared flying, direct,
// damping, gravity 343 (the flag block and damping's EDX become retail's; first difference +0x31, the input pointer's lea scheduled
// before the gravity spill, then the first cross's product sides; a gain of 4, not kept), flying, direct, gravity, damping 346; on
// the first order the input pointer declared after the flags 346 (3051 B; r89 had 417 on its form); the general arm's input length
// through magnitude3d 587 (3025 B; 403 on the 358-era body), with the first order 583.
// Round 178 (m3 lane 4, fast; aligned = difflib count with addresses masked, 152 here): left are product operand
// orders (retail loads the state field first in the crosses and the flying arm's 0x1f087b products), the input
// length's x87 load order (retail k, j, i), the low arm's two projections' schedule, the damping flag's register
// (retail EDX, computed first) and the delta/limited homes in both arms. Tried: `gravity = false` written in each arm
// of the general step test (no limited_step bool) 516 (aligned 278); that with the short-step arm first 517 (285); the
// general arm clamping delta from the normalized copy (no copy back, delta used after) 574 (285); the direct arm the
// same way 566 (3037 bytes, aligned 175); both 557 (297).
// Round 183 (m3 lane 4, fast; aligned = difflib count with addresses masked, 152 here, register-masked 98): retail
// tail-shares the first arm's out.k store with the general arm's (its first arm jumps to 0x1f0f58, the 5 bytes of the
// length gap); ours stores the gravity flag after that store, and the general arm's k sum commutes into xmm2 (`addss
// xmm2, [eax+8]`, retail `movss xmm0, [eax+8]; addss xmm0, xmm2`), so the tails differ. Retail's general limited shares
// desired's slot (0x20) and delta takes 0x2c; ours the reverse (and the direct pair is swapped too). Tried: `gravity =
// false` at the top of the first arm 346 (aligned 152; the flag store lands before the out.k store as retail, a
// statement order only, not kept); with the general output through vector3f_add 468 (3047 bytes, aligned 154; the
// history sum then takes retail's `addss xmm0, [edi+0x64]`, the k sum still commutes); delta written from desired with
// no scaled vector 391 (aligned 211, homes unchanged); vector3f_add(&difference, out + 0x64, &change) 346 (same code);
// desired reused as the bounded copy (diagnostic) 514 (general delta takes 0x2c but every other slot moves);
// vector3f_scale into delta, then `delta.i -= current->i` per row 351 (3053); on the first-arm form, the general
// arm's k row as `limited.k + current->k` 346 (same code: the sum still commutes into xmm2).
// MiniMax S (11-11): no new form, no lower score (347): the entry block still extracts the damping bit into ECX
// where retail uses EDX and is one `mov ecx, eax` short, retail stores the gravity byte before its `lea ebp` where
// ours stores it after, and the two cross products that take their operands the other way round belong to the
// shared contact_cross helper, which nine matched functions in this file use.
// MiniMax S (11-10): no new form, no lower score (347): the entry block still extracts the damping bit into ECX where
// retail uses EDX and is one `mov ecx, eax` short, and retail stores the gravity byte before its `lea ebp` where ours
// stores it after. The recorded flag-order sweep (412-500) and the flag widths recorded above never beat this body.
// Round 170 (m3 lane 6, fast, 0x1f0450 MATCH in each): the history parameter typed as the caller's `s_shape_state *` (its +0x10
// vector typed) 347 (same code); magnitude3d(input) with a `__forceinline` or an `inline` declaration in this file 587/587 (still
// called out of line, as the plain call: retail's input-length block is 0x24590's own code, ours never expands that function
// here; its file was held by another lane this round). On the 347 body: the direct arm's speed read from the field at each use
// 437 (3053 B), the ground arm's facing read from the field (no pointer local) 415 (3047 B), the low arm's normal read from the
// field 350, both 364, the direct arm's rotation rows through `input` 570 (3029 B). Diagnostics (not kept): 1-8 unused initialised
// locals at the top 347/478/579/575/490/479/474/474; magnitude3d with 0-7 of them 587/584/592/599/611/603/599/588. Seen in the
// listing: in the first cross retail makes up.k the loaded operand of both products it is in, ours the memory operand.
// m3 lane 4 r191 (fast with 0x1f0450/0x24590; aligned/masked 186/90 at 347): a new no-argument inline tick-step getter for
// the four `* g_510c54->rate` reads (a new helper, match-only): on this body 541 (3048 B, masked 77), with both magnitude3d
// lengths 356, with the input length only 346 (3052 B, masked 79), with the history length only 553 (3047 B); the existing
// game_ticks_to_seconds(1) instead, on the both-lengths form, 585 (2 lines from it); `getter * x`: same code. On the 346
// form: flags flying, direct, damping, gravity 342; `gravity = false` at the top of the first arm 345; both 341 (aligned
// 171, masked 79); on 341: the input pointer after the flags 344, the history length through magnitude3d 351,
// `flying || !limited_step` 523, the direct arm's speed per use 434, the low arm's normal from the field 421 (aligned 163).
// The masked residue stays: the crosses' and low arm's product sides and the history block's x87 order.
// M3 lane 11 r95 (fast with the file's functions and 0x24590; base 347, masked aligned 82): the flags declared flying, direct,
// damping, gravity with `input` declared after them and `gravity = false` at the top of the first arm, together, 345 (3051 B,
// masked 84; the entry block through the `lea ebp` is then retail's; a gain of 2, not kept); the general arm's step scaling
// through the existing vector3f_scale(&limited, step, &limited) 469 (3048 B, masked 71: the history block's change.i add
// becomes retail's in-place `addss xmm0, [edi+0x64]`, but the low arm's products reschedule); that with the three entry
// forms 467 (3047 B, masked 73; the first arm now stores the gravity byte before out.k as retail, but the general arm's k
// sum still commutes into xmm2, so the 5-byte k-store tail is not shared and the low arm is 5 B short); that plus
// magnitude3d(&change) for the history maximum 561 (3047 B, masked 110); the general arm's output rows through the existing
// vector3f_add(current, &limited, out + 0xc) 469 (3047 B, masked 86); the general delta through the existing
// vector3d_from_points3d(current, &scaled, &delta) 352 (3052 B, masked 82). Body restored.
// Round 425: multi-use direct/flying/positive-normal/low-normal input weights,
// mutable 528/527/477/405; immutable 528/527/477/405. NOT kept: no exact native result;
// original input observations and complete body restored. Damping carrier and
// raw/bounded homes remain.
// M3 lane 2 r79 (fast with the file's functions; base 347, register-masked aligned 127): the flying arm's desired vector
// through vector3f_scale(&forward, input.i) and vector3f_add_scaled(&desired, &left, input.j) with `desired.k += input.k` 564
// (3047 B but frame 0x28 for 0x30); the low-normal arm's projected and side vectors through vector3f_add_scaled (forward/left
// plus normal times the plane offset) and vector3f_scale 429 (3057 B); one `normal` pointer for both ground branches 352 (3049
// B); the final gravity bit as one ternary store 355 (3049 B); the flying arm's first cross through contact_cross in place of
// function_1f087b 469 (3048 B, masked 120). Re-measured on this base, same code (347): the general arm's bounded vector declared
// before delta, or renamed to sort before it; retail's delta/bounded slots (0x2c/0x20, ours 0x20/0x2c) follow neither. Both
// lengths through magnitude3d via the `input` pointer, re-measured, 585 (3027 B; the low arm then reschedules).
// m3 lane 8 r164 (fast with 0x1f0450, MATCH each time; base 347): one `vector3f *velocity` for out + 0xc at all 15 uses 592
// (3035 B); the general arm's three output rows as `limited.x + current->x` 347 (same code); the step read before the scaled
// vector 360; the step test as `step < length` 347 (same code); `magnitude *= state->speed` 347 (same code). Diagnostic (reverted): without the gravity subtraction the
// general arm's k row still adds into XMM2 (520, 3019 B), so the commuted `addss xmm2, [eax + 8]` is not caused by that
// block. Retail computes that row into XMM0 like the j row, which lets the first arm's `movss [edi + 0x14], xmm0` jump into
// it (0x1f09dc -> 0x1f0f58); ours keeps the store in each arm, and the one-instruction shift covers about 170 lines.
// P472: Independent immutable rotation/planar raw/spatial raw/ground basis/projected/side/history difference/history offset producers
// 347/347/347/347/347/347/347/347. NOT kept: no exact native producer ownership; original complete body restored. Damping carrier and raw/bounded homes remain.
// @retail 0x1f0870
void function_1f0870(byte *history, vector3f const *current, s_shape_carrier_contact const *state, byte *out)
{
    vector3f const *input = &state->input;
    bool flying = ((*(dword *)state->settings >> 3) & 1) != 0;
    bool damping = ((state->flags >> 4) & 1) != 0;
    bool direct = ((state->flags >> 2) & 1) != 0;
    bool gravity = ((state->flags >> 3) & 1) != 0;
    if (!(state->flags & 1))
    {
        vector3f left;
        contact_cross(&state->up, &state->facing, &left);
        vector3f scaled;
        vector3f_scale(input, state->speed, &scaled);
        vector3f_scale(&state->facing, scaled.i, (vector3f *)(out + 0xc));
        vector3f_add_scaled((vector3f *)(out + 0xc), &left, scaled.j, (vector3f *)(out + 0xc));
        vector3f_add_scaled((vector3f *)(out + 0xc), &state->up, scaled.k, (vector3f *)(out + 0xc));
        gravity = false;
    }
    else if (direct)
    {
        real speed = state->speed;
        point2f rotated;
        rotated.x = state->facing.i * state->input.i - state->facing.j * state->input.j;
        rotated.y = state->facing.j * state->input.i + state->facing.i * state->input.j;
        point2f delta;
        delta.x = speed * rotated.x - current->i;
        delta.y = speed * rotated.y - current->j;
        real step = state->direct_acceleration * g_510c54->rate;
        point2f limited = delta;
        if (normalize2d(&limited) > step)
        {
            limited.x *= step;
            limited.y *= step;
        }
        else
        {
            limited = delta;
        }
        ((vector3f *)(out + 0xc))->i = current->i + limited.x;
        ((vector3f *)(out + 0xc))->j = current->j + limited.y;
        ((vector3f *)(out + 0xc))->k = current->k - state->gravity * g_510c54->rate;
    }
    else
    {
        // parentheses kept: retail sums (i2 + j2) + k2 here (0x1f0b07-0x1f0b22); VC re-associates a bare chain
        real magnitude = (real)sqrt((input->i * input->i + input->j * input->j) + input->k * input->k);
        vector3f desired;
        if (flying)
        {
            vector3f left;
            vector3f forward = state->basis;
            function_1f087b(&state->normal, &forward, &left);
            if (function_30bf0(&left) == 0.0f)
            {
                contact_cross(&state->normal, g_4687b0, &left);
                if (function_30bf0(&left) == 0.0f)
                {
                    contact_cross(&state->normal, g_4687a8, &left);
                    function_30bf0(&left);
                }
            }
            contact_cross(&left, &state->normal, &forward);
            function_30bf0(&forward);
            desired.i = state->input.j * left.i + forward.i * state->input.i;
            desired.j = state->input.j * left.j + forward.j * state->input.i;
            desired.k = state->input.j * left.k + forward.k * state->input.i + state->input.k;
            function_30bf0(&desired);
        }
        else
        {
            if (state->normal.k > 0.001f)
            {
                vector3f const *facing = &state->facing;
                desired.i = facing->i * state->input.i - facing->j * state->input.j;
                desired.j = facing->j * state->input.i + facing->i * state->input.j;
                desired.k = state->input.k - (state->normal.j * desired.j + state->normal.i * desired.i) / state->normal.k;
            }
            else
            {
                vector3f left;
                vector3f forward = state->basis;
                contact_cross(g_4687b0, &state->basis, &left);
                function_30bf0(&left);
                vector3f const *normal = &state->normal;
                real d1 = 0.0f - shape_dot(*normal, forward);
                vector3f projected;
                projected.i = (normal->i * d1 + forward.i) * state->input.i;
                projected.j = (normal->j * d1 + forward.j) * state->input.i;
                projected.k = (normal->k * d1 + forward.k) * state->input.i;
                real d2 = 0.0f - shape_dot(*normal, left);
                vector3f side;
                side.i = normal->i * d2 + left.i;
                side.j = normal->j * d2 + left.j;
                side.k = normal->k * d2 + left.k;
                desired.i = state->input.j * side.i + projected.i;
                desired.j = state->input.j * side.j + projected.j;
                // retail adds projected.k before input.k (0x1f1333-0x1f1337); VC re-associates a bare chain
                desired.k = ((state->input.j * side.k + projected.k) + state->input.k) * 5.0f;
            }
            function_30bf0(&desired);
            if (*(real *)(state->settings + 0x58) >= desired.k)
                magnitude *= *(real *)(state->settings + 0x4c);
            else if (*(real *)(state->settings + 0x58) > desired.k)
                magnitude *= 1.0f + (*(real *)(state->settings + 0x4c) - 1.0f) * (desired.k - *(real *)(state->settings + 0x58)) /
                    (*(real *)(state->settings + 0x5c) - *(real *)(state->settings + 0x58));
            else if (desired.k >= *(real *)(state->settings + 0x64))
                magnitude *= *(real *)(state->settings + 0x50);
            else if (desired.k > *(real *)(state->settings + 0x60))
                magnitude *= 1.0f + (desired.k - *(real *)(state->settings + 0x60)) * (*(real *)(state->settings + 0x50) - 1.0f) /
                    (*(real *)(state->settings + 0x64) - *(real *)(state->settings + 0x60));
        }
        magnitude = state->speed * magnitude;
        vector3f scaled;
        vector3f_scale(&desired, magnitude, &scaled);
        vector3f delta;
        delta.i = scaled.i - current->i;
        delta.j = scaled.j - current->j;
        delta.k = scaled.k - current->k;
        vector3f limited = delta;
        real step = state->acceleration * g_510c54->rate;
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
        if (!state->flag_a8 && *(long *)((byte *)g_51e9c4 + 0x18) - g_510c54->game_time <= 0)
        {
            limited.i -= state->normal.i * 0.234f;
            limited.j -= state->normal.j * 0.234f;
            limited.k -= state->normal.k * 0.234f;
        }
        if (damping)
        {
            real fraction = 1.0f - state->damping;
            limited.i *= fraction;
            limited.j *= fraction;
        }
        if (gravity)
            limited.k -= state->gravity * g_510c54->rate;
        ((vector3f *)(out + 0xc))->i = current->i + limited.i;
        ((vector3f *)(out + 0xc))->j = current->j + limited.j;
        ((vector3f *)(out + 0xc))->k = current->k + limited.k;
    }
    if (!(bool)((*(dword *)state->settings >> 2) & 1) && !(state->flags & 4))
    {
        if (*(long *)(history + 0xc) != NONE && g_510c54->game_time - 1 == *(long *)(history + 0xc))
        {
            vector3f difference;
            difference.i = input->i - *(real *)(history + 0x10);
            difference.j = input->j - *(real *)(history + 0x14);
            difference.k = input->k - *(real *)(history + 0x18);
            vector3f change;
            vector3f_add((vector3f *)(out + 0x64), &difference, &change);
            // retail sums (j2 + k2) + i2 (0x1f0fe1-0x1f1047); VC re-associates a bare chain
            real maximum = (real)sqrt((change.j * change.j + change.k * change.k) + change.i * change.i) + g_47ffb0;
            vector3f offset;
            offset.i = state->velocity.i - *(real *)(out + 0x64);
            offset.j = state->velocity.j - *(real *)(out + 0x68);
            offset.k = state->velocity.k - *(real *)(out + 0x6c);
            vector3f relative;
            relative.i = ((vector3f *)(out + 0xc))->i - offset.i;
            relative.j = ((vector3f *)(out + 0xc))->j - offset.j;
            relative.k = ((vector3f *)(out + 0xc))->k - offset.k;
            function_a75d0(&relative, maximum);
            ((vector3f *)(out + 0xc))->i = relative.i + offset.i;
            ((vector3f *)(out + 0xc))->j = relative.j + offset.j;
            ((vector3f *)(out + 0xc))->k = relative.k + offset.k;
        }
        *(vector3f *)(history + 0x10) = *input;
        *(long *)(history + 0xc) = g_510c54->game_time;
    }
    if (gravity)
        *(dword *)out |= 1;
    else
        *(dword *)out &= ~1;
}


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
    function_1f0870((byte *)history, &current, contact, out);
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
    /* grouped (j + i) + k as retail's 0x1f2250 sum at 0x1f251b-0x1f2535 (the unparenthesized chain compiles (k + j) + i) */
    real length = (real)sqrt((vector->j * vector->j + vector->i * vector->i) + vector->k * vector->k);
    if (fabs(length) < 0.0001f)
        return 0.0f;
    real inverse = 1.0f / length;
    vector->i *= inverse;
    vector->j *= inverse;
    vector->k *= inverse;
    return length;
}

// MiniMax H (11-11): no new form, no lower score (413); retail keeps the entry zero in a persistent XMM6 and puts the accepted
// count in the dead incoming request slot while ours does the opposite, and the direction components stay in different homes.
// score 413 (was 413; earlier 418): Frame 0x64 agrees; persistent zero, count/component homes and direction residency remain.
// Statement walk against retail (r721, b3B): every statement, offset, condition direction, callee, argument, loop and
// switch shape agrees; the only behaviour differences were float sum groupings, which compile differently for an
// unparenthesized chain (the compiler regroups it) than for a parenthesized one. Retail's groupings and ours now:
// desired length 0x1f251b-0x1f2535 (j² + i²) + k², now explicit 418 -> 413; candidate dot 0x1f2651-0x1f2671 (j + i) + k,
// now explicit (dot3f gave (k + j) + i) 413 -> 413; tangent length 0x1f2813-0x1f2823 (k² + j²) + i², by the existing
// normalize3d (same as the file's helper) 413; final-loop dot 0x1f2981-0x1f29af (k + j) + i, shape_dot (now
// parenthesized, same bytes here). NOT fixed: the tangent dot 0x1f2859-0x1f2879 is (j + i) + k in retail, dot3f gives
// (k + j) + i; the written-out sum scores 434 (named `along`), 478 inline, k-first grouping 510. The other candidates
// for a source-order fix, six term orders of the unparenthesized sums, change nothing (the compiler regroups them).
// Round 219 (lane 3, fast, 0x1f2a80 and 0x1f19c0 listed): both negations through vector3f_scale(normal, -1.0f, &desired) 425, the first
// loop's only 425, the fallback's only 418; the fallback's minimum/selected stores before the negation 418, its direction copy after
// them 418; one inline helper for the 0x1f2e60 request call (base point, stepping bit, height) at the three calls here and 0x1f2a80's
// loop, inline or forced inline, 425/425 (0x1f2a80 43 either way).
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
// Round 289: Response-only selected contact address, with preferred cursor/count-store phase/entry success
// result interactions masks 0..7, 430/456/429/455/524/521/525/522. All restored; original
// receiver, qualified count, copies, response order and arithmetic retained.
// Round 481: Independent preferred/candidate/final scan-ordinal identities, all seven subsets
// 418 each; all three with response-pass identity 418. Original full body and qualified count restored.
// Round 235: existing normalizer complete native/scalar/native-scale/scalar-setter
// input captures, original exits 555/425/555/419; single-result joins 555/567/555/567.
// Candidate-dot admission bool/byte/short/long 419 each; rejection 420 each.
// Original normalizer, comparisons and full body restored; zero/count homes remain.
// Round 481: Independent preferred/candidate/final scan-ordinal identities, all seven subsets
// 418 each; all three with response-pass identity 418. Original full body and qualified count restored.
// Lane 9 r89 (fast; register-masked in brackets, base 163): count without volatile, a switch on it (retail's `dec eax; je; dec eax; jne`) and the bit helper at both flying tests give retail's homes,
// count in the bottom slot and the component in the dead request slot, 433 [157]; each alone 531 [203] no volatile, 445 [173] switch, 464 [155] helper; switch and helper 425; switch without volatile 436 [160].
// On that form: negation through vector3f_scale 423, count store between the NONE stores 421 [159], list and count read in the loop heads 421 [156], stepping flag named before the final loop 424-426 (2096 bytes, [132]).
// What is left starts at one cause: the -1 constant is shared by both negation arms and held in XMM5 from the first loop, which recolours desired and pushes zero out of XMM6; distinct constants (diagnostic) 411 at
// 2080 bytes [113], with direction copied by fields 392, written-out scale 399. Natural spellings keep it: fallback through minimum set first 423, written out 423, a negate local 424, negation after the first loop with a
// found flag or match pointer 424-428; zero through a local 426; candidate from the list each iteration 519 (frame 0x60); shared velocity subtraction arm 455. None under 418, so the volatile stays.
// Input parameter at all 7 positions 418, on the rewrite 421; 0x1f2e60's register parameters in 24 orders 421 and 0x1f2a80 144 (base before stepping decides). Identity locals 1-7 418-419.
// Round 226 (lane 3, fast, base 413; aligned exact/stack/regs base 214/182/119): plain count, the bit helper at both flying tests and a
// switch on count (retail's dec/je/dec/jne) re-measured on today's body 423 (retail's count/component homes); with count declared
// between the surface and object NONE stores 421 (entry block exact but for the XMM6 zero); that with the flying test's arms swapped
// 427 (the velocity subtraction still merges into the flying copy, retail merges into the non-flying one); with the fallback
// negation through vector3f_scale 421 (aligned 203/176/110, -1 still in XMM5). Diagnostic: direction by fields with a named dot
// in the count-2 arm 435 (direction stays memory-only, so its address is not why retail's XMM6/7 hold it).
// On the 421 form: case 1 written before case 2 421 (same code), with the fallback helper 421; `selected` declared before `minimum` 421.
// Lane 10 r91 (fast; masked = aligned lines with registers and stack offsets masked, base 413 [118]): retail forms the selected contact
// from the list loaded before the `*surface` store (`lea edi, [edi + edx*8]`, edi = the inner loop's preheader load or the fallback's
// reload) and case 2 reloads the list after the stores; ours reloads it after the stores and case 2 reuses it. Plain count + switch +
// bit helper at both flying tests re-measured 423 [124]. New: non-flying defaults first and one stepping subtraction after both arms,
// `else if (!flying && no input)` 469 (2057 B; the flying bit stays in AL and is retested); `contact` formed before the two stores
// 437 (2057 B [117]: retail's contact base and case-2 reload appear), on the switch form 421 (2073 [114]); on that, the inner loop's
// candidate as `list + i * 0x48` each iteration (listed 519, new base) 400 (2089 [99]), with count declared between the two NONE
// stores 398 [97]: a lead, but it needs the moved `contact` declaration (the same body with `contact` after the stores 507 at
// 2084 and 0x1f19c0 103). On 398: the helper only at the fallback's flying test 436 [88] (the stepping subtraction still merges
// into the flying copy; the if-chain instead of the switch gets retail's merge but scores 498 and 0x1f19c0 11, so the switch
// flips it); case 2's normals read through the list at each argument, no `contacts` local, 398 [84] (both address computations in
// retail's order, three of the six x87 products load as retail); contact_cross with every product commuted 398 (identical code in
// all callers); counts read in the loop heads (listed, new base) 397 [78]; an if-chain testing `count == 1` first 550; the
// fallback reading the list at each use 507 (convention flip). Left on 397: the -1 constant held across the iteration loop
// (retail reloads it in the fallback), zero in XMM6, direction in XMM6/7, the stepping subtraction's merge and the final loop's
// stepping byte. Then on main's body: case 2's normals read through the list at each argument 435 (2105 B [102]), the bit
// helper only at the fallback's flying test 417 [101], the tangent through cross3f 413 (same code), the inner loop's count read
// in its head 413 [108].
// Lane 4 r186 (fast, 0x1f19c0 matched in each; control 413 [185/117]; variants of the listed `candidate from the list each iteration`
// 519 with the selected contact formed from the same base): one `list` base local for the inner loop's candidates (`list + i * 0x48`, no
// cursor) and the selected contact 556 (2019 B: the base stays in a register and the frame shrinks), the fallback's first contact through
// it too (same code), case 2's normals through it as well 557; plain count + switch + the bit helper at both flying tests re-measured 423
// (case 1 or case 2 first, same code), the list base on it 551 (2035 B); the first loop's negation through a named normal pointer 413
// (six lines move; the fallback's negation the same way is identical); the selected contact as a normal pointer for the one-contact arm 419.
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
    /* retail keeps count in the bottom stack slot (0x1f22ce, 0x1f2791); a plain long scores 531 */
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
                    /* retail sums this dot (j + i) + k (0x1f2651-0x1f2671); dot3f compiles (k + j) + i */
                    vector3f const *candidate_normal = (vector3f *)(candidate + 0x28);
                    real dot = (candidate_normal->j * direction.j + candidate_normal->i * direction.i) +
                        candidate_normal->k * direction.k;
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
                normalize3d(&tangent);
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

/* one step of the shape against its contacts: picks the supporting contact or asks 0x1f2250 for a
   surface, and sets the shape's normal and material from it. The accumulated dots, the named velocity
   pointers, `material` and the selected `contact` give retail's operand sides, store order and `add
   eax, 0x28` (byte match); 0x1f1df0 takes state first because both calls here set ESI last. */
// @retail 0x1f19c0
void function_1f19c0(byte const *request, s_shape_state *history, byte *out)
{
    byte *component = g_51e9b8->data + (*(long *)(request + 0x40) & 0xffff) * 0xa0;
    dword flags = *(dword *)(request + 0x18);
    bool preserve = (flags & 4) || ((flags & 1) && !(flags & 2)) || (flags & 0x18);
    real movement = (real)sqrt(history->point.x * history->point.x + history->point.y * history->point.y + history->point.z * history->point.z);
    bool moving = movement != 0.0f;
    *(dword *)(out + 4) &= ~1;
    if (!preserve)
    {
        /* the velocity of the component's first rigid body */
        s_havok_component *typed_component = (s_havok_component *)component;
        s_havok_array60 const *rigid_bodies = &typed_component->rigid_bodies;
        s_havok_component_rigid_body const *rigid_body = rigid_bodies->data;
        vector3f const *velocity = &rigid_body->linear_velocity;
        history->normal = *g_4687b0;
        short *material = &history->material;
        *material = g_47d8e0;
        history->unknown58 = NONE;
        long selected = NONE;
        long highest = NONE;
        history->unknown70 = 0.0f;
        bool flying = ((**(dword **)request >> 3) & 1) != 0;
        bool best_supported = false;
        bool best_flag = false;
        real best_height = -3.4028234663852886e+38f;
        real best_speed = -3.4028234663852886e+38f;
        real highest_z = -3.4028234663852886e+38f;
        long i = 0;
        while (i < *(long *)(component + 0x8c))
        {
            byte *contact = *(byte **)(component + 0x88) + i * 0x48;
            vector3f *normal = (vector3f *)(contact + 0x28);
            real speed = normal->k;
            speed *= velocity->k;
            real speed_j = normal->j * velocity->j;
            speed += speed_j;
            speed += velocity->i * normal->i;
            speed = 0.0f - speed;
            real approach = normal->k * ((vector3f *)(request + 0x28))->k;
            approach += normal->j * ((vector3f *)(request + 0x28))->j;
            approach += ((vector3f *)(request + 0x28))->i * normal->i;
            approach = 0.0f - approach * *(real *)(request + 0x34);
            bool flag = ((*(byte *)(contact + 0x44) >> 1) & 1) != 0;
            bool supported = (flying || (*(byte *)(contact + 0x44) & 4)) ? approach >= -0.001f : false;
            if (normal->k > highest_z)
            {
                highest = i;
                highest_z = normal->k;
            }
            if (*(long *)(contact + 0x14) != NONE || speed > 0.0f)
            {
                if ((supported
                        ? (flying || !(normal->j * velocity->j + normal->i * velocity->i > 0.5f))
                            && (!best_supported || speed > best_speed)
                        : flag
                            ? (best_flag ? normal->k > best_height : true)
                            : !best_supported && !best_flag && normal->k > best_height)
                    && (!supported || !(fabs(*(real *)(contact + 0x38)) < 0.0001f)))
                {
                    selected = i;
                    best_height = normal->k;
                    best_supported = supported;
                    best_speed = speed;
                    best_flag = flag;
                }
            }
            ++i;
        }
        if ((short)selected != NONE && best_supported)
        {
            byte *contact = *(byte **)(component + 0x88) + (short)selected * 0x48;
            vector3f normal = *(vector3f *)(contact + 0x28);
            function_1201a0(&normal, g_4687b0);
            function_1f1df0(history, *(long *)(request + 0x40), (short)selected, &normal);
        }
        else
        {
            vector3f normal;
            long surface;
            if (function_1f2250((vector3f *)history, request, velocity, moving, &normal, &surface,
                &history->unknown70, (long *)(out + 0xc)))
            {
                function_1f1df0(history, *(long *)(request + 0x40), highest, &normal);
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
        bool airborne = (flags & 8) || (flags & 0x10);
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
