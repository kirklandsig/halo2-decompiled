// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0CAFC0.CPP: the point a unit sees from (its eyes, its seat's
   camera marker, or its head marker) */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_0259a0.h"

/* the unit as this file reads it */
struct s_unit_eye_object
{
	long definition_index;
	byte unknown04[0x14 - 0x4];
	long parent_index;
	byte unknown18[0xa0 - 0x18];
	real scale;
	byte unknowna4[0xaa - 0xa4];
	byte type;
	byte unknownab[0xb4 - 0xab];
	long attachment_index;
	byte unknownb8[0x10a - 0xb8];
	word flags10a_0 : 1;
	word flags10a_1 : 1;
	word flags10a_2 : 1;
	word : 13;
	byte unknown10c[0x168 - 0x10c];
	vector3f forward;
	byte unknown174[0x1fc - 0x174];
	short seat_index;
	byte unknown1fe[0x24c - 0x1fe];
	long unknown24c;
	byte unknown250[0x2c4 - 0x250];
	real crouch;
	byte unknown2c8[0x348 - 0x2c8];
	word : 13;
	word flags348_13 : 1;
	word : 2;
	byte unknown34a[0x3dc - 0x34a];
	byte unknown3dc;
	byte unknown3dd[0x3f8 - 0x3dd];
	bool unknown3f8;
};

struct s_unit_eye_object_header
{
	byte unknown00[8];
	s_unit_eye_object *object;
};

/* a seat of a unit's tag (0xb0 bytes) */
struct s_unit_eye_seat
{
	byte unknown00[8];
	long marker_name;
	byte unknown0c[0x60 - 0xc];
	long camera_marker_name;
	byte unknown64[0xb0 - 0x64];
};

/* a unit's tag */
struct s_unit_eye_definition
{
	byte unknown00[0x1c8];
	long seat_count;
	s_unit_eye_seat *seats;
	byte unknown1d0[0x218 - 0x1d0];
	real standing_height;
	real crouching_height;
};

/* an entry of g_51e9b8 (0xa0 bytes) */
struct s_unit_eye_attachment
{
	byte unknown00[0x18];
	char index;
	byte unknown19[0x70 - 0x19];
	struct s_unit_eye_attachment_element
	{
		byte unknown00[0x40];
		struct
		{
			byte unknown00[0x3c];
			struct
			{
				byte unknown00[0x70];
				point3f position;
			} *data;
		} *definition;
		byte unknown44[0x60 - 0x44];
	} *elements;
	long element_count;
	byte unknown78[0xa0 - 0x78];
};

#define EYE_OBJECT(index) (((s_unit_eye_object_header *)g_4e0300->data)[(index) & 0xffff].object)
#define EYE_DEFINITION(index) ((s_unit_eye_definition *)g_4e3b44[(index) & 0xffff].bytes)

point3f *function_b9dd0(long object_index, point3f *result);
vector3f *function_11d090(vector3f const *v, vector3f *out);
real function_30bf0(vector3f *v);
void function_df380(long unit_index, point3f *origin, vector3f *forward, vector3f *up);

// @retail 0xe0dc0
bool function_e0dc0(long unit_index)
{
	s_unit_eye_object *unit = EYE_OBJECT(unit_index);
	bool result = false;

	if (unit->unknown3dc == 6 && unit->unknown3f8 && TEST_FIELD_BIT(unit->flags348_13))
	{
		result = true;
	}
	return result;
}

/* the attachment's selected element, or NONE */
static __forceinline short attachment_element_index_b9ef0(s_unit_eye_attachment const *attachment)
{
	if (attachment->index >= 0 && attachment->index < attachment->element_count)
		return attachment->index;
	return NONE;
}

/* the outermost parent of an object */
static __forceinline long root_object_index_b9ef0(long object_index)
{
	long root_index = NONE;
	long index = object_index;

	while (index != NONE)
	{
		root_index = index;
		index = EYE_OBJECT(index)->parent_index;
	}
	return root_index;
}

// @retail 0xb9ef0
point3f *function_b9ef0(long object_index, point3f *result)
{
	long root_index = root_object_index_b9ef0(object_index);
	long attachment_index;

	s_unit_eye_object *root_object = EYE_OBJECT(root_index);
	s_unit_eye_object *const *root_reference = &root_object;
	attachment_index = (*root_reference)->attachment_index;
	if (attachment_index != NONE)
	{
		s_unit_eye_attachment *attachment = &((s_unit_eye_attachment *)g_51e9b8->data)[attachment_index & 0xffff];
		long element_index = attachment_element_index_b9ef0(attachment);

		if (element_index != NONE)
		{
			point3f const *position = &attachment->elements[element_index].definition->data->position;
			return real_point3d_set(result, position->x, position->y, position->z);
		}
	}
	function_b9dd0(object_index, result);
	return result;
}

// score 2 (was 2): only constant-one and crouching-height load scheduling remains; frame 0xa4 and all other instructions agree.
// Round 73: definition-pointer rename 2; direct explicit read-add-store 17; explicit store on the two-local graph 2;
// standing-only/crouching-only temporaries 2/2; named total with explicit store 2; early constant-one 2;
// raw crouching-height load before the calls 12. Opening graph restored.
// Earlier: frame 0xa4, entry, collision block and marker-branch layout agree; the two
// height loads are swapped, the seated object goes to EDX (retail EBP, so the marker temporaries
// rotate one register off), and the parent branch loads the tag table before the object table
// (ours EAX for the object table is a byte shorter, which also keeps a later je short: 5 bytes).
// Earlier allocation/collision forms follow.
// frame 0xa4 and entry agree; height loads, dot input carriers and marker/parent tails differ.
// frame 0xa4 and entry allocation agree; height-load order, dot-input carriers,
// selected-seat/marker tails and parent-table scheduling differ. Earlier allocation/collision forms follow.
// Frame 0xa4 agrees; object-data/tag-table allocation
// and collision-point scheduling differ.
// frame 0xa4 and entry allocation agree; height loads, collision products and marker tails differ.
// only the two height loads differ: retail loads the 1.0 constant before the
// crouching height, ours the other way round. Round 110: the seated object as a pointer local
// used for the seat test and the marker index, the default marker call after that branch, took
// 31 -> 2 (the marker registers and the parent-branch table loads now agree). Height forms on
// that graph, all 2: standing/crouching products reversed, sum reversed, crouching term first,
// named complement, raw standing/crouching reads in either order, one named height sum in either
// term order; the whole height in one statement 17; a long seat index 91.
// Basis-only/point-only scopes 309/311; collision created at its first query alone/with basis scope
// 306/306 (three-point gains rejected), with point/both scopes 310/310; complete displacement
// initialization alone/with all three narrowed lifetimes 309/310. Original body restored.
// frame 0xa4 and the marker branch agree; retail keeps the object data base in EBP
// (ours the tag table), and the collision point's last component reuses the fraction register, which
// shifts the tail by one instruction. Retail reloads the tag table after the 0xb9ef0 call (parent
// branch); ours treats that call as unable to change it and keeps it in EBP from entry.
// Round 10 (lane 10): outer definition declared first 199 (same code); outer definition through
// unit->definition_index 281; hit point through a point-plus-scaled-vector inline 199 (same);
// default marker call first 199 (same).
// Round 8 (lane 10): up vector and point in their own block scopes (frame 0xa4) 292; with the heights
// definition read through the unit index 273; marker name as an if/else 310; two marker calls 281;
// default call first 281; one definition for both branches 336; outer definition read through the unit
// index 266; with two marker calls 199 (kept); point written position-first/fraction-first 199/199;
// point as a component loop 200; comparison written collision-first 200 (retail's dot order exactly,
// shifted by the point block; apply once that is fixed); crouch blend operand orders 200/200;
// collision initialiser before the forward copy or the origin 198/198.
// Earlier: frame 0xb0 vs 0xa4 and vertical-basis/point homes still differ.
// vertical role name 309; native word flags 290; both 290; shared displacement 309;
// displacement/word/name 290; separate backward query vector 309; complete vector
// graph/word 290; native copies on that graph 290; local-up name 309; word/local-up 290.
// Retain the actual flag-word declaration with unchanged following field offsets.
// Round 57: object header table local for entry and seat lookups 192 (frame and EBP base agree;
// under the keep bar, restored); height accumulator 192; reversed point products 192; point setter 192;
// collision-first comparison 193; separate backward vector 192; collision scoped to displacement 296;
// outer definition through the unit 281; fraction at point scope 192; difference vectors swapped 192.
// Round 195: Common collision result, query/final/both point-set calls [211, 211, 211, 211]; original
// output with query/final/both point-set calls [199, 199, 199]; common result, both setters and
// collision-first comparison 211. Original queries and output graph restored; tag-table retention and
// fraction component reuse remain.
// Complete coordinate products scalar/vector/scaled-point/original-point 199/206/206/237;
// the same four point producers with collision-first comparison 200/206/206/238.
// Round 225: object-table/sentinel-before-forward or origin 190/190; native backward assignment 78;
// with either sentinel move 76/76; sentinel and collision-first 191/191; all three 59.
// Paired height terms 55 (plain, kept); collision production first 61; native marker copies 125;
// twice-used seated object 146; first three boundaries 116; all four 154. Retain object-table,
// existing backward-vector assignment, sentinel before forward, collision-first and paired height terms.
// Round 373: Offset-first contact/origin/both dot inputs 55/55/55;
// each with paired contact/origin role names 55/55/55; symmetric dots with
// a common exit, without/with paired names 55/55. Original graph restored.
// Paired raw height inputs 55; resolved marker seat short/long 55/100; conditional short/long 55/100;
// marker handle 55; handle with short/long/conditional-short seat 55/100/55. All restored.
// Default-first crouch 59; named standing complement 55; both 59; complete paired
// distance results 55; with crouch/complement/both 59/55/59; entry distance declarations
// 55. Unmatched distance captures restored; original height and collision graph retained.
// Round 70: position copy in each marker arm 40 (kept); to-collision vector first 31 (kept);
// parent branch without its index local 46; parent through a table local / header pointer 31/31;
// seated lookups through the object macro 31; crouching term named first 31.
// Round 165: parent-first admission with original/absent-seat/camera/both exits
// 299/299/299/299; one root admission with the same four parent-exit combinations
// 306/306/306/306. Both partitions lose the 0xa4 frame; original body restored.
// Round 68: direct height sum 46; separate same-parent table base 31; parent definition
// through the existing/new local 31/31; reversed crouching product/sum 31/31;
// fresh post-call parent table base 31; separate same-parent and parent bases 31.
// Shared object getter at entry/same-parent/both/parent/entry-parent/same-parent-parent/all
// 327/31/334/31/327/31/334; all with readonly receiver views 334. Original lookups restored.
// Round 398: early/reversed weighted declarations; lexical contribution names; readonly terms; height-phase scope with original/named/readonly terms; scoped readonly tag
// 2/2/2/2/2/2/2/2; all restored.
// Round 400: If-else crouch, staged complement and complete staged weights in masks 1-7, then
// complete height scope: 2/2/2/2/2/2/2/2. Original producers and arithmetic restored.
// Round 41: scale-first product 2; named height sum 2; scale local 2; `z = product + z` 2; crouch declared
// at function scope 2. Retail's biped copy of this blend (0xe5d50) loads the crouching height before the 1.0
// constant, as ours does here; 1.0 is used once in 0xcafc0, so it is not a shared value.
// Round 52: the parentless branch as a force-inline helper with its own lookups 267; heights read through
// the definition macro at each use 117; the raise through the point 0xb9dd0 returns 265; heights as a
// two-element field 2; definition local declared after the others 2; a 600 s permuter run, best 2.
// Compile-only screens of the blend (unused locals, if/else or default-first crouch, sum/product orders,
// named or array terms, 1.0 as a global, lerp helpers; flag variants only as compile-only screens) all keep the
// crouching height first.
// @retail 0xcafc0
void function_cafc0(long unit_index, point3f *position)
{
	s_unit_eye_object_header *objects = (s_unit_eye_object_header *)g_4e0300->data;
	s_unit_eye_object *unit = objects[unit_index & 0xffff].object;
	s_unit_eye_definition *definition = EYE_DEFINITION(objects[unit_index & 0xffff].object->definition_index);

	if (unit->parent_index == NONE && !TEST_FIELD_BIT(unit->flags10a_2) && !unit->type)
	{
		s_unit_eye_definition *local_98b918 = EYE_DEFINITION(unit->definition_index);
		real crouch;
		point3f origin;
		vector3f forward;
		vector3f offset;
		s_collision_result_1697c0 collision;

		function_b9dd0(unit_index, position);
		crouch = function_e0dc0(unit_index) ? 0.0f : unit->crouch;
		real standing_height = (1.0f - crouch) * local_98b918->standing_height;
		real crouching_height = local_98b918->crouching_height * crouch;
		position->z += (standing_height + crouching_height) * unit->scale;
		origin = *position;
		collision.unknown24 = NONE;
		forward = unit->forward;
		{
			vector3f up;

			function_11d090(&forward, &up);
			function_df380(unit_index, &origin, &forward, &up);
		}
		offset.i = origin.x - position->x;
		offset.j = origin.y - position->y;
		offset.k = origin.z - position->z;
		if (function_30bf0(&offset) > g_45dbd8)
		{
			point3f point;

			vector3f_assign(&forward, offset.i * -0.25f, offset.j * -0.25f, offset.k * -0.25f);
			if (function_1697c0(0x4808c2d, position, &forward, NONE, NONE, &collision))
			{
				real t = *(real *)&collision.unknown00[4] * 0.9f;

				point.x = forward.i * t + position->x;
				point.y = forward.j * t + position->y;
				point.z = forward.k * t + position->z;
			}
			else
			{
				point = collision.point;
			}
			if (function_1697c0(0x4808c2d, &point, &offset, NONE, NONE, &collision))
			{
				vector3f to_origin;
				vector3f to_collision;

				vector3d_from_points3d(&point, &collision.point, &to_collision);
				vector3d_from_points3d(&point, &origin, &to_origin);
				if (dot3f(&to_collision, &offset) < dot3f(&to_origin, &offset) + 0.05f)
				{
					position->x = collision.point.x - offset.i * 0.05f;
					position->y = collision.point.y - offset.j * 0.05f;
					position->z = collision.point.z - offset.k * 0.05f;
					return;
				}
			}
			*position = origin;
		}
	}
	else if (unit->parent_index == NONE)
	{
		s_object_marker marker;

		if (unit->unknown24c != NONE)
		{
			s_unit_eye_object *seated = objects[unit->unknown24c & 0xffff].object;

			if (seated->seat_index != NONE)
			{
				function_b8d30(unit_index, definition->seats[seated->seat_index].marker_name, &marker, 1, false);
				*position = marker.matrix.position;
				return;
			}
		}
		function_b8d30(unit_index, 0x4000095, &marker, 1, false);
		*position = marker.matrix.position;
	}
	else
	{
		function_b9ef0(unit->parent_index, position);
		if (unit->seat_index != NONE)
		{
			long parent_index = unit->parent_index;
			s_unit_eye_object *parent = EYE_OBJECT(parent_index);
			s_unit_eye_seat *seat = &EYE_DEFINITION(parent->definition_index)->seats[unit->seat_index];

			if (parent->type != 1 || seat->camera_marker_name)
			{
				s_object_marker marker;

				function_b8d30(parent_index, seat->camera_marker_name, &marker, 1, false);
				*position = marker.matrix.position;
			}
		}
	}
}
