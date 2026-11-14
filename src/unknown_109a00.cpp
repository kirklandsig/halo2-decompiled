#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "object_list.h"
#include "unknown_1428b0.h"
#include "vector_products.h"
#include "normalize3d.h"
#include "object_tag_definitions.h"

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

/* the time globals' tick count as a real */
static inline real game_time_ticks_as_real(void)
{
	s_game_time_globals *globals = g_510c54;
	short ticks = globals->field_2_3;
	real rate = (real)ticks;
	return rate;
}

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
							real scale = game_time_ticks_as_real();
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

// score 8 (was 8): frame 0x48 and 925-byte length agree; the i product loads up first, and
// the not-valid edge and failure pops/stores differ. Earlier at 14: 56 before round 101, 112 before that; frame 0x48, the entry registers, the definition spill, the search and the axis bases agree (round 101); earlier at 56,
// frame 0x48 and the entry registers agree, 0xb7060 matches; what remains: the definition spilled at its load (retail EAX, ours ECX, then the
// default body and index registers), the search's bodies/offset load order, transform kept in EBP where retail has `lea ecx` and left in EBP, the dot's first two
// products (retail k then j, up first), one cross product side, and the failure stores (retail after the pops).
// MiniMax C (11-11): no new form, no lower score (8); the failure block as a label with the two outer paths jumping into it, and the goto tail, both give the same listing, so the dot's i term still loads up first and the failure block still stores before its pops.
// MiniMax R (11-10), earlier at 8 (was 14): the projection's dot, the scale's input and the cross now take the transform's fields
// directly, so the dot's term order (retail k, j, i) and the cross's products agree; left: the dot's i product (ours
// up.i*forward.i, retail forward.i*up.i), the not-valid exit's jump (ours into the failure block's pops, retail to the
// shared epilogue) and the failure block's order (ours stores then pops, retail pops then stores).
// MiniMax C (11-09): no new form, no lower score (56); the definition's early spill, the axis base and the failure tail still differ.
// MiniMax B (11-06): no new form, no lower score (56); a named physics slot index 58, the swapped count comparison 56, both 58 and the widened default body 251.
// MiniMax I (11-05, run 37): no new form, no lower score (56); the transform-base lea, the dot's product order and the failure stores' placement still differ.
// MiniMax A (11-04, run 41): no new form, no lower score (112); the entry registers are still reversed (retail
// object in eax, entry in ecx) and the physics pointer is still spilled early.
// MiniMax S (11-10): no new form, no lower score (56); the definition still spills to ECX where retail spills it to EAX (which also swaps the count and default-body
// registers), the transform pointer still moves before the 0x141590 call, and the failure stores still precede the pops where retail's follow them.
// Earlier at 56 (was 112): frame 0x48 and the entry registers agree, 0xb7060 matches; what remains: the definition spilled at its load (retail EAX, ours ECX, then the
// default body and index registers), the search's bodies/offset load order, transform kept in EBP where retail has `lea ecx` and left in EBP, the dot's first two
// products (retail k then j, up first), one cross product side, and the failure stores (retail after the pops).
// Earlier at 112: object/entry registers, early physics-pointer spill and center/projection scheduling.
// Earlier at 112 (was 112): retail takes the object index in eax and the entry
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
// Round 292: Overlap-safe previous transform/velocity-pair/center transfers, subsets 1/2/4/3/5/6/7,
// 236/229/220/219/223/243/256; all with existing center converter 248. Original complete file,
// helpers, copies and caller restored; input register pair, physics spill and projection schedule remain.
// Round 28 (lane 17, fast with 0xb7060): the search loop as a force-inlined helper 324 (813 bytes, both velocity
// getters then stay out of line; head registers retail's and 0xb7060 matches), with the getters force-inlined too 132
// (925, head and 0xb7060 back), plus the center through the converter 129 (928, entry on the stack). Diagnostics:
// only both getters out of line, or only the default-body test removed, give retail's head (328, 269); either
// `entry->active` or `entry->valid` test removed puts the entry on the stack (226, 192). Model index inline 135,
// scaled locals inline 131, no body pointer 235, those together 131/235. Default-body forms (ternary, field in a
// local, default-first, count in a local) by found-first and center converter: 112 to 265, converter 149 to 238.
// N308: Both velocity getters inactive-first/inactive-return/native output/active-path output,
// original center 147/147/328/219, native center 177/177/305/255. Forced existing getter expansion
// on the four native-output graphs 261/219/248/255. Original getters, center and body restored;
// object/entry roles, physics spill and projection schedule remain.
// Round 310: Plain component loops for previous transform/previous vector family/current center,
// subsets 1..7 then all unsigned counters: 382/233/364/224/401/219/384/384. Matrix and
// center loops recover the input register pair and caller 0xb7060, but regress the body;
// center alone first differs at +0x35 and reloads its source receiver per component.
// Original copies, getters and body restored; physics/default/search/projection scheduling remains.
// N313: Existing default producer owns the complete marked-body search; eager/late fallback with
// latch/early return 525/524/525/525; existing native center conversion 519/518/519/519.
// The larger existing helper stays out of line, shrinking the caller to 659..666 bytes and worsening
// its caller to 11; exact-only boundary forms restored. Original default producer, search and center
// transfer restored; frame 0x48, input register pair and physics/projection scheduling remain.
// N314: Force expansion of the existing complete default/search selector and both velocity getters;
// eager/late fallback with latch/early result 251/272/199/275, existing center conversion 241/264/254/268.
// The selector now expands, but input roles and scalar/transfer scheduling regress; six forms worsen the
// caller from 2 to 11. All complete-search ownership, helper qualifiers, center transfers and executable
// body restored; original private selector, getters, caller inputs and arithmetic retained.
// N316: Natural predicate/bound inner search with direct-result/count/center combinations:
// 157/243/223/209/180/243/234/245. Original search, center copy, helper bodies and caller retained; input
// roles and physics/projection scheduling remain.
// N318: Complete current-center memcpy/memmove/standard forward/backward copy
// 112/125/128/128; with complete previous-transform memcpy 116/128/123/123. Original
// center/copies and full body restored; input roles, physics spill and projection scheduling remain.
// Round 123 (lane 8, fast with 0xb7060; "aligned" counts differences after aligning the two listings): retail's head
// needs two things: the center through carrier_vector_from_havok (object index EAX, entry chain in one register) and the
// component taking EBP with the entry in EBX. Diagnostics with probe stores to a test global, not kept, on the converter
// form (149, aligned 119, 0xb7060 11): one value use of the component pointer at the physics block flips them and gives
// retail's head and 0xb7060 0 (208, 926 bytes, aligned 42); two count reads there 201 (aligned 41, 0xb7060 0), one 229
// (no flip), four reads 208 (926, 0xb7060 0); two count reads before the model index 242 (aligned 51, 0xb7060 0); no flip
// from count reads inside the update block 224, default-body reads 230, entry reads 257, definition values 230 or constant
// stores 256. Without the converter the same probes leave the head (value use 247, count reads 266). Natural forms on the
// converter, none flips: velocity getters taking the component and index 154 (alone 113); a NULL-initialised component
// tested after, three shapes 243/242/243 (one alone 263); a separate havok-component local 129 (alone 132); body index
// NONE with one update if/else 149 (alone 112), with the update returning 149; guard tests with one failure label 224
// (alone 218, 911 bytes). Failure stores in else arms give retail's pops-before-stores: three arms without the converter
// give retail's head and 0xb7060 0 but the pushes sink past the tests and three epilogues stay (282, 956 bytes); on the
// converter three arms or returns 224 (969), two arms 189 (946). On the two-read probe: the `+=` dot 206 (926, aligned 32;
// on the value-use probe 208, aligned 43; alone 110), `up->i = up->i - scaled_i` 201, dot3f(up, forward) 201, the failure
// stores swapped 201, the axis pointers together 200 (aligned 45), inline scaled terms 201 (aligned 47). What stays with
// the head flipped: the definition's load register and early store, the default body's movsx/count order, the search
// body's offset temporary, the dot and scale products' memory operands, the failure pops.
// Round 540: Common vector converter through shared scalar builder/returned output pointer/output
// reference/native value, original center 112/112/112/214; the same producer for center
// 149/149/149/262. Native value returns grow the body to 965/1007 bytes; all restored.
// Round 56 (m3 lane 13, fast with 0xb7060): the inner search ending on `&& !found` with no break 256 (908 bytes,
// 0xb7060 2); the center through carrier_vector_from_havok re-measured 149 (0xb7060 11; entry EBP, component EBX,
// entry index on the stack); that with the entry after the component index and the no-break search 232; that with a
// body local for the three body uses 192, with a body-entry pointer 230. All restored.
// Round 158 (m3 lane 4, fast with 0xb7060): center through the converter re-measured 149 (aligned 119, against 75 unchanged); that with a named current-transform pointer for the matrix call 129 (928);
// a named selected-body pointer for both velocity getters and the center 192, with the plain center copy 171 (905 bytes); the two closing clears swapped on the converter form 149.
// Round 77 (m3 lane 9, fast with 0xb7060): the projection written `up->i -= forward->i * dot` without the scaled
// locals 131; a rigid-body local for the two velocity getters with the center through carrier_vector_from_havok 192
// (0xb7060 11); body_index declared short 197; the matrix through the existing havok_component_rigid_body_matrix_get
// 112 (same code); the center read through the existing havok_component_rigid_body_get 112 (same code). Restored.
// N347: Existing point/vector setters for current center, previous center and previous velocity pair,
// seven subsets then the complete family with prior matrix memcpy: 161/207/220/207/222/220/216/216.
// Previous-center setter recovers the caller but gives 207/911 bytes; NOT kept. Original complete body
// restored; native input registers, early physics spill and center/projection remain.
// Round 48 (m3 lane 2, fast with 0xb7060): the inner search tested `j < count && !found` without the break 256
// (908 B); the model index folded into the physics lookup 135; the component pointer computed before the NONE
// test 270; the center copy before the velocity getters 174; diagnostic, `entry->active = true` moved ahead of
// the matrix call (not retail's order: retail stores it after the center) 218, but the head then equals retail
// (object index EAX, entry index ECX, first difference +0x35) and 0xb7060 matches: the register arguments follow
// where the entry flag store sits, so look for a retail-order form that changes the same first-pass liveness.
// N349: Actual selected component-row result, immediate/deferred ordinal and original/row-owned
// downstream inputs 247/268/237/277; with existing center conversion 222/256/249/286.
// Original complete body restored; input roles, early physics spill and center/projection scheduling remain.
// NOT kept: selected-row result forms did not match.
// N347: Existing point/vector setters for current center, previous center and previous velocity pair,
// seven subsets then the complete family with prior matrix memcpy: 161/207/220/207/222/220/216/216.
// Previous-center setter recovers the caller but gives 207/911 bytes; NOT kept. Original complete body
// restored; native input registers, early physics spill and center/projection remain.
// Round 69 (m3 lane 10, fast with 0xb7060, aligned counts in brackets): the axis block written with no pointer locals (normalize,
// dot3f(up, forward), vector3f_scale(forward, dot, &scaled), a subtract helper, normalize, cross3f_staged, each on
// `&entry->transform.axis`) gives retail's dot, scale and subtract instruction for instruction, compile-only and on the full
// listing; only four cross product sides differ. Forward first in the dot 16 lines off in that block; direct `up->i -= scaled.i` 16; every scale statement order and side 16.
// That form alone 132 (aligned 57); with the center through carrier_vector_from_havok 120 (927 bytes, aligned 106, 264 lines as
// retail, masked 33: only the entry/component EBX/EBP swap and entry index on the stack); with the search reading
// `component->bodies[i].count` and `.indices[j]` directly 117 (928, masked 30, retail's offset temporary); that without the converter 235.
// One hot memory reference decides the swap: compile-only, deleting either `= true` store flips entry to EBX, but deleting a failure
// store, a previous-state copy or a flag test does not, and the two listings have the same references to entry and component.
// No flip: the default body as a ternary or default-first, loop test order, a count local, a body pointer as pointer arithmetic, a named
// havok pointer, valid or active in a local, entry/component declared in either order or at the top, byte flag fields or
// found, active stored ahead of the center or a velocity getter (124), valid read before the active store (118), a named current transform (121).
// F361: Existing default-body producer as component member, mutable/read-only receiver
// and ordinary/forced inline: short result 112/112/112/112; long 267/267/267/267.
// NOT kept: no native entry-register/spill repair; original helper, search and body restored.
// DH331: Typed standard ranges for prior transform/velocity-pair/center, subsets 1..7,
// then all with backward ranges: 228/242/235/210/219/253/281/285. Original copies, helpers,
// body and caller inputs restored; native input registers, physics spill and projection remain.
// DH334: Standard native-array inner products, forward/reverse and both product orientations,
// each with original/shared add-scale projection: 180/185/180/185/262/264/262/264; caller 11/11/11/11/0/0/0/0.
// NOT kept: no exact match; original complete body restored; native input roles, physics spill and center/projection remain.
// N351: Native handle/ordinal domains dword-long/long-dword/dword-dword/int-long/long-int/int-int/dword-int/int-dword:
// 112/112/112/112/112/112/112/112. NOT kept: no exact match or qualifying gain; complete original body restored.
// Remaining: entry registers, physics-pointer spill and center/projection schedule.
// Y581: Existing full component/row model and main-body selector, direct/shared
// acquisition and accessor/data, original center 112/112/112/112; converted
// center 149/149/149/149 (frame 0x44, caller 2 -> 11). NOT kept: no gain or
// native input/spill repair. Original views, helpers, center and full body restored.
// Round 151 (one-link harness with 0xb7060's and five callee files, exact; fast and plain confirmed): 112 -> 89 `transform` pointer for the axis block with the center through the
// existing converter, dot3f(forward, up), vector3f_scale, direct subtract; 76 with transform declared before the validity test and passed to the matrix call, and the search reading the component
// directly (0xb7060 0); 65 with forward/left/up pointers from it; 58 with only `up` named; 56 with the dot passed straight to vector3f_scale. Forms, in harness counts: transform declared in the arm 237-272 (caller 2),
// before `entry->active` 231, at the top of the function 272; the axis from entry fields 148 (940 bytes, caller 11), pointers from entry 122 (927, caller 11); valid cleared first 56 (positional only); dot3f(forward, up) 56 (same code);
// `up` before the validity test 145 (caller 11), declared first 56; a const forward pointer 65; found as long 75; direct products 69-73, scalar scaled_i/j/k 66, cross3f 69; the definition through a const
// reference to its pointer 97 (911 bytes), through a reference to the record 65 (same code); a second definition test threaded away 65 (same code), `if (definition)` 247; failure stores in early returns 288 (943 bytes,
// retail's pops before stores, caller 0), threaded through `body_index = NONE` and else 112 (same code), `== NONE` first with a return 218 (911), a success flag 132; on the earlier axis form, no `valid = true` or `valid = 2` flips EBX/EBP (diagnostic).
// Round 153 (fast with 0xb7060): six of the 56 are `call` rows flagged only by a 9-byte offset shift (ours forms the axes from the transform
// in EBP, 3-byte leas; retail from the entry in EBX, 6-byte leas) and ten are padding after our shorter tail, where the not-valid exit shares
// the failure block's epilogue (retail gives it its own, so the failure block pops before its stores). Tried: the validity test as an early
// return 56; the definition fetched after the default body 81; the axes from the entry with the transform kept for the matrix call 115
// (928 bytes, the head flips, 0xb7060 11; close to round 151's pointers from entry); an explicit return closing the validity block 56 (same
// code); the definition as a pointer to const 56; the matrix call given `&entry->transform` 56 (same code). Re-measured: `forward` named
// beside `up` 65.
// P360: Existing default selector complete valid/invalid comparison result in bool/byte/short/long: 61/61/61/61/61/61/61/61.
// NOT kept: no exact native source recovery; original complete body restored; early physics spill, transform-base production, dot/cross schedule and failure tail remain.
// P369: Actual rotation-subobject pointer/reference ownership before validity, inverse workspace, after inverse and
// after composition: 124/55/54/48 twice. NOT kept: no exact native axis/dot shape; late subobject bases still feed
// derived axis addresses. Original whole body restored; frame 0x48, definition spill and failure tail remain. Early
// pointer/reference forms lose 0xb7060 (0 -> 11); all restored.
// P372: Bare inverse workspace before model/physics/admitted-record/default/latch/search,
// body admission and previous-state transfer: eight 56. NOT kept: no native early spill,
// axis/dot or failure-tail repair. Original complete body restored; all six exact controls
// including the caller survive; frame 0x48 and 925-byte extent agree.
// T68: Individual local names on the native transform graph: physics, selected body,
// search latch, both ordinals, inverse, relative transform and up axis:
// 56/56/56/56/56/56/56/56. NOT kept: no exact ownership/home repair. Original executable body restored.
// Early definition register/spill, projection and failure tails remain.
// T70: Actual default-body output starts from the signed byte, then invalid flat/nested guards
// correct it; char/short/int/long 249/235/252/252/249/237/252/252. NOT kept: result allocation regresses;
// original selector, physics observations, geometry, helpers and complete body restored.
// P388: Actual object-model/physics named fields and retained physics-handle
// receiver graph, seven subsets and complete read-only views: 56/56/275/56/275/275/275/275.
// NOT kept: no exact native early spill/axis/failure-tail recovery; original
// record layouts, observations, complete body and call tuples restored.
// P398: Actual motion/component/tag-instance reference owners, seven subsets,
// then read-only tag slot: 56/56/58/56/58/58/58/58; all at 925 bytes.
// NOT kept: no native spill/axis/failure-tail repair or exact ownership-form match.
// Original complete body and contracts restored; all six exact controls survive.
// Lane 1 r83 (fast with 0xb7060; al = aligned lines left / the same with registers masked, control 56, al 37/24): retail forms each axis
// address from the entry (`lea` off EBX, the matrix call's own `lea ecx`), as a helper taking the transform would after inlining. The axis
// block as a new inline helper taking the transform (one caller, match-only): plain inline, given the local pointer or `&entry->transform`
// 329/460 (690 bytes, it stays out of line; 0xb7060 lost in the second); only the three normalize calls in the helper 93; force-inlined,
// given the local pointer 62 (al 43/28), given `&entry->transform` with or without the local 238/238 (the head flips, 0xb7060 lost); the
// matrix call given `&entry->transform` 56 (same code, re-measured). Body unchanged.
// Round 100 (m3 lane 12, fast with 0xb7060, which matched in each): body_index short 178; the projection as vector3f_add_scaled(up, forward,
// -dot, up) 101 (910 bytes); the dot as a statement sum k, j, i on this body 62; `up` as a reference 56 (same code); the inner search as a while
// loop with its own counter 56 (same code); the outer loop's found test as a break at the top of its body 56 (same code). All restored.
// Lane 12 r104 (fast with 0xb7060, which matched in each; the 48 confirmed plain): the transform local dropped (the composition
// call takes `&entry->transform`) and the axes reached through `matrix3x3 *rotation = &entry->transform.rotation;` declared after
// that call 48 (925 bytes; the call's `lea ecx, [ebx+0xb0]` and forward's `lea edi, [ebx+0xb4]` are retail's; P369's number),
// NOT kept: the same pointer declared at the top of the validity block 55, so the gain comes from where it is declared. Left at
// 48: the head (definition register and spill, default-body registers, search offset load order), left and up formed from forward
// (EDI) where retail forms them from the entry, the dot's terms (ours i, j, k forward-first; retail k, j up-first, then i), one
// cross product side, the failure stores before the pops. Tried: the rotation pointer from the old transform local, after the call
// 65; from the entry with the transform local kept 55; on 48: dot3f(forward, up) 48 (same code); forward, left and up named from the
// rotation pointer 49; `up` taken from the entry instead 38 (up's lea and left's EBP as retail; a mixed spelling), on that
// dot3f(forward, up) 38 (same code). Every second axis taken from the entry flips the head (entry EBP, 0xb7060 11): left and up from
// the entry 117 (928), the three axes named from the entry after the call 124 (927), the axes as entry fields at each use with up
// named 115 (928).
// Round 101 (m3 lane 16, fast with 0xb7060, which matched in each; plain confirmed 14). Kept: the model's tag data named
// (`byte *model`, used for the physics test and the definition lookup) with the physics index read through it at each use and
// `long model_index` kept: 56 -> 43 (the definition loads into EAX and spills at once, the default body and search registers
// are retail's); no `transform` pointer, the matrix call, normalizes, scale and cross on `&entry->transform...` (retail forms
// each axis off the entry): 43 -> 20; `forward` and `left` named beside `up` (each axis through its own pointer): 20 -> 14
// (`forward` alone also 14). Left: the dot's terms (retail k, j up-first, then i forward-first; ours j, i, k forward-first), the
// cross's i row (retail loads up.j), and the failure block: retail's not-valid
// exit uses the main epilogue so its failure block pops edi/esi/ebp before the two stores; ours jumps into the failure block's
// epilogue. Tried: the three tag lookups through a new inline getter 56 (same code); the component index read at each use 64;
// the model index inline too 51; on 20: no `up` pointer 145 (head flips, 0xb7060 11), dot3f arguments swapped 20 (same code),
// the dot named 22; on 14: dot3f(forward, up) 14 (same code); with `forward` alone: the dot as statements k, j, i 63, one exit
// through `body_index = NONE` and an else arm 14 (same code), forward and up declared together before the normalizes 28. A 600 s
// permuter run (62 tries, from the `forward`-only form) found nothing below 14.
// Round 103 (m3 lane 16; fast with 0xb7060, which matched in each; a compile-only /O2 /arch:SSE /Gr harness reproduces the
// SSE residue and the failure tail exactly). The tail: retail's not-valid exit and the success path share the epilogue after
// the last normalize, so the join after the valid test survives and the failure block merges with its own epilogue copy
// (pops scheduled before the stores); ours threads the join away. Tried: guard gotos with the failure block before the
// success code 14 (same code), after it 171 (head flips); `entry->active = true;` moved after the valid block 67 (retail's
// tail, but the store then sits in the join, aligned 9); harness only, all keeping our tail: an early return on !valid, a
// return inside the valid block, `else return;`, an empty else arm, three else arms, a success flag, the failure stores
// swapped or chained, a `valid` local read before the active store, `inverse` declared outside the valid block or at the
// top, the valid block as a forced-inline helper (with or without an early return). The SSE part: dot3f(up,
// &entry->transform.forward) 10 (the cross's i row right); the dot, the scale's input and the cross's first operand through
// `&entry->transform.up/forward` with the subtraction and normalizes through `up` 8 (left: the dot's i product, forward
// first in retail; a mix of pointer and field spellings, so it needs a match); that with the active store after the valid
// block 67 (aligned 3, register-masked 1: only the store's place and the i product left). Harness only: the subtraction
// through the field instead 34 SSE lines; all 192 term orders, groupings and product orders of the dot as a local helper
// leave the i product (2); a projection helper (vector, axis) 8-12; cross3f 31; the dot named 8; direct `up->i -= forward->i
// * dot` 23; identity-local chains (diagnostic) at the pointers or the entry index: period 8, no state below the base; the
// active store written at the head or the tail of both arms of the valid test 17 SSE lines (the tail moves but not to retail's).
// Fast as well: the empty else arm 14 and the early return on !valid 14 (both the same code); the field-spelled dot, scale and cross
// with the subtraction through the field too 143 (940 bytes, the head flips, 0xb7060 11).
// M3 lane 3 r231 (fast with 0xb7060, which matched in each; the compile-only /Gr harness screened the tails first). New: the
// body index declared NONE beside the component index, the failure stores as the then-arm of `if (body_index == NONE)` and the
// update in its else (update last in source) 171: the not-valid exit then shares the success epilogue and the failure block
// pops before its stores as retail, but the block moves up behind the body test (the head flips at +0x35); the whole update
// block as a new forced-inline helper 18 (harness: our tail). Re-measured listed forms (round 103's guard gotos, round 101's
// `body_index = NONE` exit): the failure block under a label at the top with gotos and the update last 171, the same block as
// the body test's then-arm 171, the label block with the update in the body test's then-arm and `else goto failed;` 14 (same
// code), a closing `if (body_index == NONE)` for the stores 14 (same code). So retail's epilogue sharing comes with the update
// last in source, and retail keeps it with the failure block laid out last; no form found that does both. No other untried
// natural form is left: the tail forms (early returns, flags, else arms, gotos, helpers, the active store's place) and the SSE
// forms (all dot orders, field spellings, projection and cross helpers) are listed above, and the field-spelled SSE fix (8) is
// a mix that needs a match, which the tail blocks.
// Round 88: required typed model field 8; existing scalar vector builder for subtraction/scaling/
// both 16/119/119; these with the shared direct cross 26/128/128; validity from both prior flags 244.
// NOT kept: arithmetic/control forms regress, and scaling builders lose the matched caller (0 to 11).
// Original arithmetic, transfers, search, calls and control restored; typed field confirmed plain 8.
// m3 lane 1 r95 (fast with 0xb7060; base 8: the not-valid exit's target, the dot's i product and the failure block's
// pops): guard clauses that clear the entry and return at each failed lookup (three copies of the two stores, the update
// last with no return) 311 (960 B; 0xb7060 lost, 11); the body index defaulting to NONE with the component lookup and
// search nested and one `if (body_index != NONE)` update / `else` clear 8 (same code); on that, the valid block as
// `if (!entry->valid) return;` 8 (same code); the valid flag read into a local before the active store 14; the failure
// stores chained `entry->active = entry->valid = false;` 8 (the two stores then run valid first, the other way round).
// Also: the model's physics index named and used at its test and its lookup 117 (928 B; 0xb7060 lost, 11).
// Round 78 (m3 lane 15, fast with 0xb7060, which matched in each; base 8). A non-null `__assume` on the object, on the
// entry or on the component ahead of the lookups 11 each (the same code: three `[eax + ecx + 0x40]` body loads swap their
// base and index); on `up` ahead of the dot 31, with `forward` too 33, with `left` as well 33 (the head flips); an
// assume ahead of the success `return` (`entry->active`, or `body_index != NONE`) 31/31, or a non-null one on the entry
// ahead of the failure stores 31 (the head flips, the tail stays ours). The function returning `bool` (true after the
// update, false after the failure stores; the declaration in 0xb7060's file changed with it) 22 (929 bytes): every line
// is retail's, tail included (the not-valid exit shares the success epilogue, the failure block pops before its
// stores), but for `mov al, 1` and `xor al, al` ahead of the two `pop ebx` (link-time optimisation does not drop the
// unused result). So retail's two returns were not the same block when its jumps were threaded, and left no code for
// the difference. The body as a new forced-inline helper returning `bool`, the failure stores under `if (!...)` in the
// function 22 (925 bytes: our tail again, and three cross and scale products change sides).
// Round 115 (m3 lane 16, single-address fast, control 8): 0-7 dead initialised locals move the product sites but not
// the tail: the dot's i term is retail's only at 5 (12: the cross products' first two rows then flip), the cross rows
// only at 0, so the two need a relative shift of three steps. The tail: retail's not-valid branch shares the epilogue
// the cross path falls into, so the failure block's own epilogue has one predecessor and its pops are scheduled above the
// stores (the same tail as matched 0xb7060). A compile-only screen gives that tail when the failure is the first arm
// (`if (body_index == NONE) failure; else success;`, or an early return before a success path written last), but the
// failure block is then laid out inline after the body test where retail jumps forward to it at the end: both 172
// (real build). The failure stores chained (`valid = active = false`) 8 (same code); the not-valid path as an early
// return 8 (same code).
// Also, at the same phase: the dot taking the up and forward pointers 14 (its k and j rows flip); the cross taking the
// three axis pointers 12 (two cross rows flip, the dot's i row stays); the scale taking the forward pointer 14 (the
// first cross rows flip).
// @retail 0x109660
void function_109660(long object_index, long entry_index)
{
	s_carried_object *object = ((s_carried_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_carrier_motion *entry = (s_carrier_motion *)&g_5107f0->entries[entry_index];
	long component_index = object->component_index;
	if (component_index != NONE)
	{
		s_carrier_component *component = (s_carrier_component *)(g_51e9b8->data + (component_index & 0xffff) * 0xa0);
		long model_index = ((s_object_definition const *)g_4e3b44[object->definition_index & 0xffff].bytes)->model_tag_index;
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		if (*(long *)(model + 0x24) != NONE)
		{
			s_carrier_physics_definition *definition = (s_carrier_physics_definition *)g_4e3b44[*(long *)(model + 0x24) & 0xffff].bytes;
			long body_index = carrier_component_default_body(component);
			bool found = false;
			for (long i = 0; i < component->count && !found; i++)
			{
				for (long j = 0; j < component->bodies[i].count; j++)
				{
					if (definition->bodies[component->bodies[i].indices[j]].flags & 0x10)
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
				carrier_vector_from_havok((vector3f *)&entry->center, &component->bodies[body_index].body->motion->center);
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
					vector3f scaled;
					vector3f_scale(&entry->transform.forward, dot3f(&entry->transform.up, &entry->transform.forward), &scaled);
					up->i -= scaled.i;
					up->j -= scaled.j;
					up->k -= scaled.k;
					function_30bf0(up);
					cross3f_staged(&entry->transform.up, &entry->transform.forward, &entry->transform.left);
					function_30bf0(left);
				}
				return;
			}
		}
	}
	entry->active = false;
	entry->valid = false;
}
