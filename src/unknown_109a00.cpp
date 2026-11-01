#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_list.h"
#include "unknown_1428b0.h"
#include "vector_products.h"

// @flags /O2 /arch:SSE /Gr

struct s_carried_object
{
	long definition_index;
	byte unknown004[0x14 - 4];
	long parent_index;
	byte unknown018[0x88 - 0x18];
	vector3f velocity;
	byte unknown094[0xb4 - 0x94];
	long component_index;
	long platform_index;
	byte unknown0bc[4];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word : 13;
};

struct s_carried_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_carried_object *object;
};

struct s_carrier_definition
{
	word unknown00;
	word : 7;
	word flag7 : 1;
	word : 8;
};

struct s_carrier_motion
{
	point3f previous_center;
	point3f center;
	vector3f previous_velocity;
	vector3f velocity;
	vector3f previous_angular_velocity;
	vector3f angular_velocity;
	transform4x3f previous_transform;
	transform4x3f current_transform;
	transform4x3f transform;
	bool valid;
	bool active;
	byte unknown0e6[2];
};

struct s_small_index;
struct s_location;
short function_0b67a0(s_small_index const *data);
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
void function_b75a0(long object_index, point3f const *point, vector3f const *forward,
	vector3f const *up, s_location const *location, bool unknown);


bool function_109a00(long object_index, vector3f *delta, bool rotate, vector3f *velocity);

static __forceinline bool carried_object_motion_apply(long object_index, vector3f *delta, bool rotate, vector3f *velocity)
{
	s_carried_object_header *headers = (s_carried_object_header *)g_4e0300->data;
	s_carried_object *object = headers[object_index & 0xffff].object;
	bool result = false;

	if (TEST_FIELD_BIT(object->flag2))
	{
		long platform_index = object->platform_index;
		for (long i = 0; i < g_5107f0->object_count; i++)
		{
			if (g_5107f0->object_indices[i] == platform_index)
			{
				s_carrier_motion *motion = (s_carrier_motion *)&g_5107f0->entries[i];
				if (motion->valid && motion->active && object->parent_index == NONE)
				{
					s_carried_object *platform = headers[platform_index & 0xffff].object;
					s_carrier_definition *definition = (s_carrier_definition *)g_4e3b44[platform->definition_index & 0xffff].bytes;
					long component_index = platform->component_index;
					if (component_index != NONE &&
						function_0b67a0((s_small_index *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0)) != NONE)
					{
						transform4x3f matrix;
						function_ba160(object_index, &matrix);
						if (rotate && TEST_FIELD_BIT(definition->flag7))
						{
							transform4x3f moved;
							vector3f previous_velocity;
							function_142a60(&motion->transform, &matrix, &moved);
							vector3f previous_offset;
							vector3d_from_points3d(&motion->previous_center, &matrix.position, &previous_offset);
							cross3f_staged(&motion->previous_angular_velocity, &previous_offset, &previous_velocity);
							vector3f_add(&motion->previous_velocity, &previous_velocity, &previous_velocity);
							vector3f *object_velocity = &object->velocity;
							object_velocity->i -= previous_velocity.i;
							object_velocity->j -= previous_velocity.j;
							object_velocity->k -= previous_velocity.k;
							function_142640(&motion->transform, object_velocity, object_velocity);
							carrier_point_velocity(&motion->center, &motion->velocity,
								&motion->angular_velocity, &moved.position, delta);
							function_b75a0(object_index, &moved.position, &moved.forward, &moved.up, NULL, false);
							*velocity = *object_velocity;
						}
						else
						{
							point3f moved;
							transform4x3f_apply_point(&motion->transform, &matrix.position, &moved);
							vector3f displacement;
							displacement.i = matrix.position.x - moved.x;
							real scale = (real)g_510c54->field_2_3;
							displacement.j = matrix.position.y - moved.y;
							displacement.k = matrix.position.z - moved.z;
							vector3f change = {
								displacement.i * scale,
								displacement.j * scale,
								displacement.k * scale
							};
							function_b75a0(object_index, &moved, NULL, NULL, NULL, false);
							*velocity = object->velocity;
							vector3f_add(&object->velocity, &change, &object->velocity);
							delta->i = change.i * -1.0f;
							delta->j = change.j * -1.0f;
							delta->k = change.k * -1.0f;
						}
						result = true;
					}
				}
				break;
			}
		}
	}
	return result;
}

// score 10 (was 10): Frame 0x88 agrees; two x-component adds remain: the second point-velocity first
// linear add uses a register copy and memory add (retail loads linear.x and adds the cross term), and the
// velocity update adds change.x as a load and add (retail copies change.x and adds the velocity from memory).
// Frame 0x88 and first point block agree; the second point first linear add uses a register
// copy and memory add instead of a direct load/add. The nonrotating scale/add schedule remains at retail length.
// Earlier: point-velocity and nonrotating scale/add
// Frame 0x88 and first point-velocity agree; second point-velocity and nonrotating schedules remain.
// Frame 0x88 and first point block agree; second point inputs, nonrotating products and five extra bytes remain.
// Earlier: Frame 0x88 agrees; point-velocity and nonrotating scale/add
// scheduling and the four-byte extent excess remain. Earlier shared point-velocity expansion changed
// Frame 0x88 agrees; second point-velocity and nonrotating scheduling
// and five excess bytes remain. Earlier shared point-velocity expansion changed
// Frame 0x88 agrees; first point-velocity schedule recovered; second
// point-velocity and nonrotating scale/add scheduling and five excess bytes remain.
// Frame 0x88 and first point-velocity block agree; second point-velocity
// and nonrotating scale/add scheduling and five excess bytes remain.
// Earlier shared point-velocity expansion changed
// 128/1015 to 164/1018 and aligned differences 66 to 40; second call schedule recovered.
// Earlier first point-velocity and nonrotating difference/add forms: external motion record 164;
// external object record 164; returned transform input 249; first angular input value 205; first linear
// input value 197; first center input value 206; first velocity input values 246; immutable displacement
// rate 164.
// First point-velocity and nonrotating component schedules still differ.
// receiver before point operation 164f; receiver before transform 166f; native relative velocity
// 202f; subtraction component loop 171f; addition component loop 176f; native addition result
// 157p; delta component loop 165f; difference and scale loops 165f; component result construction
// 157f; first input sum copy 157f; second input sum copy 157f; native-result formal order 157f;
// native-result component loop 157f; forced native addition result 195f; forced change-input sum
// 235f; forced result loop 201f.
// Returned addition result 157p adds a call absent from retail; original helper retained.
// Earlier first point-velocity input and nonrotating scale/add forms:
// Nonrotating shared point difference 165; shared scale 164; difference plus scale 166;
// shared scaled difference 169; final delta scale 164; both scale boundaries 164;
// actual motion-field bindings 166; object-velocity field reference 164 (fast scores).
// Native clock short/long/double 179/175/193; native difference initializer 164;
// separate displacement/scaled-change vectors 126 (kept); with short/long/double clock
// 173/168/199. The actual scaled change remains used by the addition and delta outputs.
// Separate-output shared real scale 123 (three-point gain rejected), double scale 195;
// final shared real scale 126; both real 165; double then real 198; real then double
// 152 (retail length); both double 198; real scale plus forced original addition 123.
// native scaled change initialized by value then scaled 243; native scaled change assigned by component
// 126; clock conversion after completed native displacement 168; shared point difference with separate
// scaled change 125; plain component loop for separate scaled change 164; scaled change workspace declared
// before displacement 126; immutable completed displacement and scaled change 126; complete native delta
// value before output copy 174.
// Separate displacement components xzy/yxz/yzx/zxy/zyx 122/122/118/126/126;
// clock-first products 126; scaled-output orders jki/kij 140/146. yzx kept after plain check.
// Relative-velocity subtraction stores ikj/jik/jki/kij/kji 118 each;
// final delta publication jik/jki/kji 122/124/122.
// At-use clock field 88 kept: all three products read the field consistently; first
// point-velocity block now agrees. Native two-use initial velocity/field receiver/both
// 185/166/185; with at-use clock 187/167/194; initial velocity with shared real scale 185.
// The two point-velocity copies change schedule together: forms that give the second copy
// retail's code break the first. Points-difference helper plus named scale plus scale helper
// 164; i difference, named scale, then j and k 165 (nonrotating block exact, first copy off);
// that with direct velocity fields 210; with the scale helper 165; with scale-helper delta 239.
// With the returned point input: returned point 167; returned point and native original input
// 201; returned point and early displacement 167; returned point, native original and early
// displacement 201; returned point and complete scaled assignments 167; returned point, native
// original and scaled assignments 201; returned point, early displacement and scaled assignments
// 167; returned point with all three native boundaries 201. All restored.
// Second point native center/linear/both/angular/center-angular/linear-angular/all inputs
// 212/189/229/211/247/216/256/286; the last form also gives the completed position both consumers.
// Single-use input copies restored; original shared helper and game calls retained.
// At-use clock graph: scaled-change component orders ijk/ikj/jik/jki/kij/kji
// 88/87/86/87/86/88; native value then in-place scale 164; paired native vector names 88.
// Two-point forms rejected; second point-velocity and nonrotating schedule remain.
// Void-output native addition inputs first/second/both 123/126/175; forced 123/126/175;
// complete moved position 202; with coherent nonrotating frame names 202. Original helpers restored.
// At-use clock displacement producer orders ijk/ikj/jik/kij/kji 88/87/88/87/87;
// shared displacement producer 177; aggregate and immutable aggregate 88/88.
// Complete native moving record / read-only record / object-header payload / both 88 each;
// read-only headers / both with read-only motion / both with read-only headers / all 88 each.
// Round 38: second staged 181; first staged 176; first direct 177; second direct 181. With a named
// clock: first staged 74 (plain-confirmed, kept), second staged 186, first direct 89, second direct 179.
// Displacement orders on the kept graph ijk/ikj/jik/kij/kji 74/73/74/73/73; the one-point forms restored;
// a nonrotating velocity receiver 164.
// Round 418 on the first-staged graph: second complete point result 179; clock before
// all displacement producers / after j / after k 68/31/30; second complete result with
// those clock positions 171/175/175, with the first clock and shared change scale 171.
// Shared change scale on the after-k/after-j clocks 30/31; final delta scale 30;
// both scales on the after-k clock 173. Only the after-k clock read is kept, plain 30;
// all helpers and output boundaries restored. Frame and retail length agree; the
// second point's first linear add and nonrotating SIMD scheduling remain.
// Round 97: x difference, then the clock scale, then the y and z differences 10 (kept: retail copies the
// scale for x and scales y and z in place); x, scale, z, y 18; on the kept form: velocity-add operands
// swapped 10 (identical); one/two throwaway tuples at entry 163/172 (diagnostic; the adds never flip);
// delta before the velocity add 61; `+=` component adds 37; displacement scaled in place 172.
// Round 296: Point-transform/velocity-transform/publication inline-noinline summaries, all seven nonempty subsets, 30 each; clock after completed displacement on the first-staged graph 74. All declarations and arithmetic restored.
// Round 202 on the first-staged/x-before-clock graph: second shared math boundaries 170;
// complete second result 175; both 200; complete updated velocity 193; math/update 207;
// second/update 221; all 230; direct-cross/update 214. CRT output copies rotating/nonrotating/both:
// memcpy 10/10/10; memmove 195/194/226; both copies with second shared math 170/237.
// All complete output locals, CRT calls, includes and math boundaries restored.
// Round 16: the change through the existing in/out scale helper 10; the delta negation through it 10;
// both 172; the object velocity written out without its pointer local 238; the output copy read from
// the object 10. The same point-velocity helper loads the linear x first in matched 0x109e00.
// Round 89: shared object-velocity receiver 199; second output pointer 163; nonrotating receiver 162;
// change renamed scaled_change and displacement renamed offset 10 each; second linear pointer 164;
// const change and const scale 10 each; force-inline point-velocity helper 10 (0x109e00 still matched).
// Round 31 (0x109e00 matched each time): a nonrotating velocity receiver pointer also used for the
// output copy 162 (compile-only: it gives the x add retail's copy-then-memory form but reorders the
// rotating block); `result` declared first 12; the change built by statements 10 (identical);
// previous velocity declared with the offset and the moved transform at its call 10; the delta as
// `-change.i` 52. Compile-only screens of 128 local-declaration combinations (headers, platform index,
// definition, velocity pointer, declaration places) found none that flips only the two x adds.
// @retail 0x109a00
bool function_109a00(long object_index, vector3f *delta, bool rotate, vector3f *velocity)
{
	return carried_object_motion_apply(object_index, delta, rotate, velocity);
}

struct s_carrier_search_view
{
	byte unknown00[0x28];
	byte location[8];
	point3f center;
	real radius;
};

short __stdcall function_bb050(long mask, dword type_mask, void const *location, point3f const *position, real radius, long *objects, short maximum_count);
void function_b7360(long object_index);
void function_bba20(long object_index);

// @retail 0x109580
void function_109580(long object_index)
{
	(void)&object_index;
	long objects[1024];
	s_record_pool *pool = g_4e0300;
	s_carrier_search_view *object = (s_carrier_search_view *)((s_carried_object_header *)pool->data)[object_index & 0xffff].object;
	long count = function_bb050(0, 0x3f, object->location, &object->center, object->radius, objects, 1024);
	for (long i = 0; i < count; i++)
	{
		long index = objects[i];
		s_carried_object_header *header = &((s_carried_object_header *)pool->data)[index & 0xffff];
		if ((header->flags & 1) && !(header->flags & 0x18))
		{
			s_carried_object *child = header->object;
			if (!TEST_FIELD_BIT(child->flag2))
			{
				s_carried_object *attached = ((s_carried_object_header *)pool->data)[index & 0xffff].object;
				attached->flag2 = true;
				attached->platform_index = object_index;
				function_b7360(index);
				if ((1 << header->type) & 0x3c)
				{
					function_bba20(index);
					header->flags |= 8;
				}
			}
		}
	}
}

struct s_carrier_vector4
{
	real n[4];

	real const &operator()(long i) const { return n[i]; }
};

struct s_carrier_body_motion
{
	byte unknown00[0x40];
	s_carrier_vector4 velocity;
	s_carrier_vector4 angular_velocity;
	byte unknown60[0x70 - 0x60];
	s_carrier_vector4 center;
};

struct s_carrier_body
{
	byte unknown00[0x3c];
	s_carrier_body_motion *motion;
	byte inactive;
};

struct s_carrier_body_entry
{
	byte unknown00[0x40];
	s_carrier_body *body;
	byte unknown44[4];
	char *indices;
	long count;
	byte unknown50[0x60 - 0x50];
};

struct s_carrier_component
{
	byte unknown00[0x18];
	char default_body;
	byte unknown19[0x70 - 0x19];
	s_carrier_body_entry *bodies;
	long count;
};

struct s_carrier_physics_body
{
	byte unknown00[0x18];
	byte flags;
	byte unknown19[0x90 - 0x19];
};

struct s_carrier_physics_definition
{
	byte unknown00[0x3c];
	s_carrier_physics_body *bodies;
};

struct s_havok_component;
real function_30bf0(vector3f *vector);
void function_141590(transform4x3f const *in, transform4x3f *out);

PRIVATE inline short carrier_component_default_body(s_carrier_component const *component)
{
	short body_index;
	if (component->default_body >= 0 && component->default_body < component->count)
		body_index = component->default_body;
	else
		body_index = NONE;
	return body_index;
}

PRIVATE inline void carrier_make_vector3f(vector3f *vector, real i, real j, real k)
{
	vector->i = i;
	vector->j = j;
	vector->k = k;
}

PRIVATE inline void carrier_vector_from_havok(vector3f *vector, s_carrier_vector4 const *havok)
{
	carrier_make_vector3f(vector, (*havok)(0), (*havok)(1), (*havok)(2));
}

PRIVATE inline void carrier_body_velocity_get(s_carrier_body const *body, vector3f *velocity)
{
	if (!body->inactive)
	{
		carrier_vector_from_havok(velocity, &body->motion->velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

PRIVATE inline void carrier_body_angular_velocity_get(s_carrier_body const *body, vector3f *velocity)
{
	if (!body->inactive)
	{
		carrier_vector_from_havok(velocity, &body->motion->angular_velocity);
	}
	else
	{
		*velocity = *g_4687a4;
	}
}

// score 112 (was 112): retail takes the object index in eax and the entry
// index in ecx (ours the other way round, which is also 0xb7060's last
// difference), and orders the dot product's terms k, j, i (ours k, i, j;
// the source order of the terms does not change the code). Retail copies the
// center with carrier_vector_from_havok like the velocities (movss), but that
// alone makes ours pass the entry index on the stack and swap ebx and ebp
// (149; as three field copies 140, through a motion local 129). Tried:
// parameters swapped at both ends (no change), entry declared before the
// object or after the component index (113, 128), inline object and entry
// getters (no change). The three axis pointers declared together before the
// normalize calls 111 (retail's registers: forward edi, up esi, left ebp; the
// products then come out forward-first and the dot sums k,i,j where retail has
// up-first and k,j,i; the source operand order of the dot does not change it,
// 111); that plus the center through carrier_vector_from_havok 151 (object index
// in eax as retail, entry index on the stack); with the entry pointer computed
// first 153; the pointers together with the entry first 112.
// Round 279: physics-array/count/both/index-list/array-list/count-list/all metadata
// snapshots 247/135/247/169/247/171/247; all with existing center conversion 239.
// Original search and center copy restored; entry registers and projection order remain.
// Complete previous-state library transfers: transform 112, three vectors 116, both 114;
// with existing center conversion 151/154/149. Existing staged cross alone 112, with complete
// transfers and center conversion 149. Original copies, search, products and caller restored;
// object/entry input registers, physics-record spill and center/projection schedule remain.
// Round 419: object record 112; physics record 112; object record/physics record 112; search body record
// 112; object record/search body record 112; physics record/search body record 112; object record/physics
// record/search body record 112; object record/physics record/search body record/existing center
// conversion 149. Entry register pair, physics-record spill and center/projection scheduling remain.
// Original body restored.
// Complete previous-state library transfers: transform 112, three vectors 116, both 114;
// with existing center conversion 151/154/149. Existing staged cross alone 112, with complete
// transfers and center conversion 149. Original copies, search, products and caller restored;
// object/entry input registers, physics-record spill and center/projection schedule remain.
// Round 33 (fast, agreeing with the full count; 0xb7060 checked each time): with the center through
// carrier_vector_from_havok (149) the object index arrives in EAX as retail, but the entry pointer
// takes EBP and the component EBX (retail EBX/EBP), so the entry index stays on the stack (ebp is never
// a register argument) and 0xb7060 goes 2 -> 11. On that form: the entry declared after the component
// index 149, before the object 151; the entry as `entries + entry_index` 149; the search loop indexing
// component->bodies[i] directly (no element pointer) 123 (928 bytes, the found flag moves to a
// parameter slot, same EBX/EBP swap). Without the center change: that loop indexing 235, with the
// entry after the component index 146.
// Round 236: signed/unsigned matching-body result, native/late default and existing center converter 231/239/253/246/231/239/253/246.
// Original executable body restored.
// Round 241: entry/component/entry-component/object/entry-object/component-object/
// all receivers at use 303/303/353/133/303/307/352; all with existing center converter 362.
// Original body restored; input registers, physics spill and center/projection scheduling remain.
// Round 40 (screen, base 101; 0xb7060 2 throughout): bisected, only dropping the body search loop moves the
// object index to EAX as retail, and then the entry index stays on the stack (the transform block, the
// transfer block and the calls move nothing). The dot as `dot = k; dot += j; dot += i` 99 (the k and j
// products up-first as retail); dot3f in either order, the first two terms parenthesised, the terms i, j, k
// 101 each; cross3f_staged(up, forward, left) 101, cross3f 113; with the `+=` dot: the scaled terms as one
// vector3f 124 (918 bytes), updated in place 122, `up->i = up->i - scaled_i` 99.
// Round 121 (m3 lane 5, fast): the three axis pointers declared before the normalize calls, orders
// left/forward/up, left/up/forward, up/left/forward 131/131/132 (all three lea hoisted above the
// first call; retail interleaves); one exit with body_index declared NONE beside the component index
// and the update under `if (body_index != NONE)` 112 (same code), that with the center through
// carrier_vector_from_havok 149. Compile-only (no LTCG, same axis block as ours): every one-expression
// dot spelling gives the same code; no by-name or pointer access per axis or per written component
// moved the dot's k,i,j order or the swapped products. A 600 s permuter run was cut off by its time
// cap while confirming a candidate.
// Round 125 (m3 lane 5, fast): the entry pointer declared before the object lookup 113 (the entry index
// still arrives in EAX); the entry as `entries + entry_index` 112 (same code); the hand-written cross and
// dot replaced by the existing cross3f_staged(up, forward, left) and dot3f 112 (same code, kept).
// Round 60 (m3 lane 9, fast with 0xb7060; real builds of round 40's screened forms): the dot as `dot = k;
// dot += j; dot += i` 110 (k and j products up-first as retail), with the three axis pointers declared
// together before the normalize calls 109, with `up->i = up->i - scaled_i` 110; the center through
// carrier_vector_from_havok with the `+=` dot and the axis pointers 153 (264 instructions as retail, object
// index in EAX, entry index on the stack); the loop's body pointer declared before the outer loop 112.
// Retail's call in 0xb7060 sets ECX (entry) before EAX (object), as the declared order implies.
// Round 63 (lane 12, fast; an LTCG env link with the stand-in callers and 0xb7060 is exact: 915 bytes of code
// against retail's 925): the failure stores valid-first 112 (same code); `up->i = up->i - scaled_i` on the current
// body 112 (same code); the entry before the object 113; the search loop tested `!found && i < count` 253 (908);
// `found` declared before the default body 119. The dot written k/j/i up-first, forward-first, all up-first or as
// i, j, k terms: same code (env). A /Gd compile loads the object index into EAX and the entry index into EBP.
// Round 150 (0xb7060 checked): center through carrier_vector_from_havok 149 (0xb7060 11; entry pointer EBP,
// component EBX); with a statement-sum dot k, j, i 151; the failure stores as one chained assignment 112 (same
// code), with the center change 149; the center change with the search's body pointer declared before the outer
// loop 149. Compile-only: 1-32 unused-local probes before the axis pointers or after the inverse never change
// the dot's k, i, j term order (only its registers).
// Round 128 (m3 lane 5, fast with 0xb7060): retail's roles (object EAX, entry ECX) are the declared-order rule
// (first EAX, last ECX); with the center through carrier_vector_from_havok (149, 0xb7060 11) the body is retail's
// but for the entry/component EBX/EBP swap and the entry index on the stack, whose home slot then holds `found`
// (frame 0x44 against 0x48; retail keeps `found` at +0x13 and the definition at +0x20). On that form: `found`
// declared before the default body 154; the entry before the object 151; the object through a `headers` local
// (as carried_object_motion_apply) 129 (928 bytes). Without it: `found` first 119; the `headers` local 132.
// Round 70 (m3 lane 9, fast with 0xb7060): our head coalesces the object index with the object pointer (ECX), so the
// entry index takes EAX. A named masked object index 132, a header element pointer 132 (head unchanged both times),
// the entry by byte offset 112; 0xb7060 stays 2 in each.
// Round 291: uninitialized entry/component/physics pointer declarations at entry, all
// subsets then existing center conversion, 112/112/112/112/112/112/112/149. Original body restored.
// Round 261: existing default-body helper result-first/reject-first/conditional/positive admission
// 232/112/265/112; with existing center converter 156/149/238/149 (caller 11). Original helper,
// body and call inputs restored; input register pair and early physics-pointer spill remain.
// Round 290: actual default-body field captured once, char/long value with short/long result,
// original/converted center, 115/149/267/257/265/257/272/257. Converted centers keep frame 0x44
// and caller 11 versus 2; original helper, search and body restored. Entry roles and early physics spill remain.
// Round 219: canonical search latch as byte/char/short/long, each with original/native center
// transfer: 112/149/112/149/131/236/238/160. Original Boolean latch, center transfer, helpers and
// body restored; object/entry convention, physics spill and projection schedule remain.
// Round 334: frame 0x48 agrees; first +0 reverses object/entry registers,
// then the physics spill and center/projection schedule differ. Actual body-array
// byte displacement stepped in the original loop, signed/unsigned and bool/byte
// found combinations, 140/140/140/140; with existing complete center conversion
// 130/130/130/130 at 928 bytes, frame 0x44, caller 2 -> 11. Original search,
// center transfer, helpers, arithmetic and complete body restored.
// @retail 0x109660
void function_109660(long object_index, long entry_index)
{
	s_carried_object *object = ((s_carried_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_carrier_motion *entry = (s_carrier_motion *)&g_5107f0->entries[entry_index];
	long component_index = object->component_index;
	if (component_index != NONE)
	{
		s_carrier_component *component = (s_carrier_component *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0);
		long model_index = *(long *)(g_4e3b44[object->definition_index & 0xffff].bytes + 0x38);
		long physics_index = *(long *)(g_4e3b44[model_index & 0xffff].bytes + 0x24);
		if (physics_index != NONE)
		{
			s_carrier_physics_definition *definition = (s_carrier_physics_definition *)g_4e3b44[physics_index & 0xffff].bytes;
			long body_index = carrier_component_default_body(component);
			bool found = false;
			for (long i = 0; i < component->count && !found; i++)
			{
				s_carrier_body_entry *body = &component->bodies[i];
				for (long j = 0; j < body->count; j++)
				{
					if (definition->bodies[body->indices[j]].flags & 0x10)
					{
						body_index = i;
						found = true;
						break;
					}
				}
			}
			if (body_index != NONE)
			{
				if (entry->active)
				{
					entry->previous_transform = entry->current_transform;
					entry->previous_velocity = entry->velocity;
					entry->previous_angular_velocity = entry->angular_velocity;
					entry->previous_center = entry->center;
					entry->valid = true;
				}
				((s_havok_component *)component)->rigid_body_matrix_get(body_index, &entry->current_transform);
				carrier_body_velocity_get(component->bodies[body_index].body, &entry->velocity);
				carrier_body_angular_velocity_get(component->bodies[body_index].body, &entry->angular_velocity);
				*(vector3f *)&entry->center = *(vector3f *)&component->bodies[body_index].body->motion->center;
				entry->active = true;
				if (entry->valid)
				{
					transform4x3f inverse;
					function_141590(&entry->previous_transform, &inverse);
					function_142a60(&entry->current_transform, &inverse, &entry->transform);
					vector3f *forward = &entry->transform.forward;
					function_30bf0(forward);
					vector3f *left = &entry->transform.left;
					function_30bf0(left);
					vector3f *up = &entry->transform.up;
					function_30bf0(up);
					real dot = dot3f(forward, up);
					real scaled_i = forward->i * dot;
					real scaled_j = forward->j * dot;
					real scaled_k = forward->k * dot;
					up->i -= scaled_i;
					up->j -= scaled_j;
					up->k -= scaled_k;
					function_30bf0(up);
					cross3f_staged(up, forward, left);
					function_30bf0(left);
				}
				return;
			}
		}
	}
	entry->active = false;
	entry->valid = false;
}
