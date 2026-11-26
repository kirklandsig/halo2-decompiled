// @flags /O2 /arch:SSE /Gr
/* ITEMS.CPP: the item object type (weapons, equipment and garbage; its
   definition is at 0x467c08) */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "sound_sources.h"
#include "vector_products.h"
#include "object_tag_definitions.h"

/* the item (the object data) */
struct s_item
{
	long definition_index;
	union
	{
		dword object_flags;
		struct { dword : 7; dword object_flag7 : 1; };
	};
	byte unknown008[0x14 - 8];
	long parent_index;
	byte unknown018[0x28 - 0x18];
	s_location location;
	byte unknown030[0x64 - 0x30];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f linear_velocity;
	vector3f angular_velocity;
	byte unknown0a0[0xc1 - 0xa0];
	byte flags_c1;
	short location_c2;
	long location_c4;
	long location_c8;
	byte unknown0cc[0x12c - 0xcc];
	union
	{
		byte flags_12c;
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
			word flag4 : 1;
			word flag5 : 1;
			word flag6 : 1;
			word flag7 : 1;
			word : 8;
		};
	};
	short value_12e;
	short bsp_index;
	short surface_index;
	short material_index;
	byte value_136;
	byte value_137;
	byte unknown138[0x13a - 0x138];
	byte value_13a;
	byte unknown13b[0x14c - 0x13b];
	long ignore_object_index;
	long creation_time;
	long unit_index;
	vector3f spin_axis;
	real spin_sine;
	real spin_cosine;
	word flag16c_0 : 1;
	word flag16c_1 : 1;
	word flag16c_2 : 1;
	word flag16c_3 : 1;
	word flag16c_4 : 1;
	word flag16c_5 : 1;
	word flag16c_6 : 1;
	word flag16c_7 : 1;
	word : 8;
};

struct s_item_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_item *item;
};

/* the unit holding an item (a view of the unit) */
struct s_item_unit
{
	byte unknown000[0x13c];
	long player_index;
};

#define ITEM_GET(index) (((s_item_header *)g_4e0300->data)[(index) & 0xffff].item)

void __stdcall function_b9b90(long object_index, bool disable);
point3f *function_b9dd0(long object_index, point3f *result);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x10c850
void function_10c850(long item_index)
{
	s_item *item = ITEM_GET(item_index);

	function_b9b90(item_index, false);
	item->value_13a = 0;
	_ReadWriteBarrier();
	item->flag5 = false;
}

#pragma function(_ReadWriteBarrier)

// @retail 0x10cf50
bool function_10cf50(long item_index)
{
	bool result = false;
	s_item *item = ITEM_GET(item_index);

	if (TEST_FIELD_BIT(item->flag0) && !TEST_FIELD_BIT(item->flag1))
		result = true;
	return result;
}

// @retail 0x10da60
void function_10da60(long item_index, point3f *position)
{
	s_item *item = ITEM_GET(item_index);

	if (TEST_FIELD_BIT(item->flag0))
	{
		s_object_marker marker;

		function_b8d30(item->unit_index, 0x4000095, &marker, 1, false);
		*position = marker.matrix.position;
	}
	else
	{
		function_b9dd0(item_index, position);
	}
}

static __forceinline void item_vector_scale(vector3f *v, real scale)
{
	v->i *= scale;
	v->j *= scale;
	v->k *= scale;
}

/* normalizes the vector in place and returns its old length (0 when it is
   too short) */
static inline real item_axis_normalize(vector3f *v)
{
	real length = (real)sqrt(v->i * v->i + v->j * v->j + v->k * v->k);

	if (fabs(length) < 0.0001f)
	{
		length = 0.0f;
	}
	else
	{
		item_vector_scale(v, 1.0f / length);
	}
	return length;
}

// @retail 0x10d5f0
void function_10d5f0(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	vector3f axis = item->angular_velocity;
	real length = item_axis_normalize(&axis);

	if (length > 0.001f && !(item->flags_c1 & 1))
	{
		real angle = length * g_510c54->rate;

		item->flag4 = true;
		/* stored through a pointer: retail sets the flag before the copy */
		vector3f *spin_axis = &item->spin_axis;
		*spin_axis = axis;
		item->spin_sine = (real)sin(angle);
		item->spin_cosine = (real)cos(angle);
	}
	else
	{
		item->flag4 = false;
		/* through a pointer: retail clears the flag before loading the zero */
		real *sine = &item->spin_sine;
		*sine = 0.0f;
		item->spin_cosine = 1.0f;
	}
}

/* a collision result of function_1697c0 (as items read it) */
struct s_collision_result_1697c0
{
	long type;
	byte unknown04[0x24 - 4];
	short unknown24;
	byte unknown26[0x3c - 0x26];
	short surface_index;
	byte unknown3e[0x50 - 0x3e];
	short material_index;
	byte unknown52[0x58 - 0x52];
	byte value_58;
	byte value_59;
	byte unknown5a[2];
};

bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
	long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
extern vector3f *g_4687bc;

// @retail 0x10b190
void __stdcall function_10b190(long item_index)
{
	s_item *item = ITEM_GET(item_index);

	if (item->parent_index == NONE && (item->flags_c1 & 1) && TEST_FIELD_BIT(item->flag5) && item->bsp_index != g_4686c4)
	{
		vector3f vector;
		s_collision_result_1697c0 collision;

		vector.i = g_4687bc->i * 0.1f;
		vector.j = g_4687bc->j * 0.1f;
		vector.k = g_4687bc->k * 0.1f;
		collision.unknown24 = NONE;
		if (function_1697c0(0x24909c0d, &item->position, &vector, item->ignore_object_index, NONE, &collision) &&
			(collision.type == 1 || collision.type == 3))
		{
			item->bsp_index = g_4686c4;
			item->surface_index = collision.surface_index;
			item->material_index = collision.material_index;
			item->value_136 = collision.value_58;
			item->value_137 = collision.value_59;
		}
		else
		{
			function_10c850(item_index);
		}
	}
}

void function_10dad0(long item_index);

// @retail 0x10b2c0
bool __stdcall function_10b2c0(long item_index, long a, long b)
{
	s_item *item = ITEM_GET(item_index);

	ITEM_GET(item_index)->object_flags |= 0x1000;
	item->spin_sine = 0.0f;
	item->spin_cosine = 1.0f;
	item->ignore_object_index = NONE;
	item->unit_index = NONE;
	item->spin_axis = *g_4687b0;
	ITEM_GET(item_index)->creation_time = g_510c54->game_time;
	function_10dad0(item_index);
	return true;
}

/* the item object type definition */
struct s_item_type_definition
{
	char const *name;
	dword group_tag;
	short datum_size;
	short unknown0a;
	short unknown0c;
	short unknown0e;
	void *unknown10[4];
	void (__stdcall *handler20)(long);
	void *unknown24[2];
	bool (__stdcall *handler2c)(long, long, long);
};

s_item_type_definition g_467c08 =
{
	"item",
	'item',
	0x16c,
	NONE,
	NONE,
	NONE,
	{ 0, 0, 0, 0 },
	function_10b190,
	{ 0, 0 },
	function_10b2c0
};

void function_b7680(long object_index, real scale, real seconds);

struct s_item_definition
{
	byte unknown000[0xbc];
	dword flag0 : 1;
	dword flag1 : 1;
	dword flag2 : 1;
	dword flag3 : 1;
	dword flag4 : 1;
	dword flag5 : 1;
	dword : 26;
	byte unknown0c0[4];
	real scale_multiplayer;
	real scale;
	byte unknown0cc[0x114 - 0xcc];
	real delay_lower;
	real delay_upper;
	byte unknown11c[4];
	long effect_tag_index;
};

// @retail 0x10dad0
void function_10dad0(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	real scale = 1.0f;

	if (!TEST_FIELD_BIT(item->flag0))
	{
		s_item_definition *definition = (s_item_definition *)g_4e3b44[item->definition_index & 0xffff].bytes;
		real value = g_4e6948->state == 2 ? definition->scale_multiplayer : definition->scale;

		if (value > 0.0f)
			scale = value < 0.5f ? 0.5f : (value > 3.0f ? 3.0f : value);
	}
	function_b7680(item_index, scale, 0.0f);
}

/* starts an iteration over the items (weapons, equipment and garbage) */
PRIVATE inline void item_iterator_new(s_type_f1af8e *iterator)
{
	iterator->signature = 0x86868686;
	iterator->type_mask = 0x1c;
	iterator->flags = 1;
	iterator->index = 0;
	iterator->object_index = NONE;
}

// @retail 0x10ca00
bool __stdcall function_10ca00(long *item_index)
{
	struct
	{
		s_item *item;
		s_type_f1af8e iterator;
	} iteration;

	bool result = false;

	item_iterator_new(&iteration.iterator);
	while ((iteration.item = (s_item *)function_baeb0(&iteration.iterator)) != NULL)
	{
		if (iteration.item->value_12e > 0)
		{
			*item_index = iteration.iterator.object_index;
			result = true;
			break;
		}
	}
	return result;
}

void function_15e300(long object_index);

static inline bool item_held_by_player(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	bool result = false;
	if (TEST_FIELD_BIT(item->flag0))
		result = ((s_item_unit *)ITEM_GET(item->unit_index))->player_index != NONE;
	return result;
}

// @retail 0x10ccc0
void function_10ccc0(long item_index)
{
	bool held_by_player = item_held_by_player(item_index);
	s_item *item = ITEM_GET(item_index);

	if (held_by_player)
		item->flag3 = true;
	else
		item->flag3 = false;
	if (((1 << ((s_item_header *)g_4e0300->data)[item_index & 0xffff].type) & 4) && TEST_FIELD_BIT(item->flag16c_6))
		function_15e300(item_index);
}

#include "effects.h"
#include "slot_handler.h"
static __forceinline long real_to_long(real value);

/* function_259d0 (0x259d0) on the first seed of g_4e7408, inlined */
inline real item_real_random_range(real lower, real upper)
{
	dword *seed = &g_4e7408->unknown0;
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return ((upper - lower) * ((real)(*seed >> 16) * (1.0f / 65535.0f)) + lower);
}

// score 2 (was 2): frame 0x14 and retail length agree; the final x87 add
// follows the third owner-field load, where retail puts it before that load.
// Earlier: writing the product before the lower bound fixed the other x87
// scheduling differences.
// Earlier 6 (was 6): the three x87 instructions around the effect call's argument
// pushes are still one slot early here (the fxch st(1) after the first push 0,
// the delay spill before the tag-index load and the fstp st(0) before the push
// eax); this round found no lower form of the argument setup
// Earlier 6 (was 6): the argument setup still keeps the fxch st(1) after the first
// push 0, the delay spill before the tag-index load and the fstp st(0) before the
// push eax; this round added no lower form (the owner's field order, the delay
// expression's spelling and the source permuter leave the three x87
// instructions where they are)
// Earlier 6 (was 6): the effect call's argument setup has three x87
// instructions one slot earlier in ours: the fxch st(1) before the first push 0,
// the delay spill before the tag-index load and the fstp st(0) before the push
// eax; the x87 sequence, the argument registers and the owner struct's stores
// all match
// Product-first range span/sample/offset named subsets 1..7: 2/76/76/61/61/61/61;
// scalar offset accumulator 61. Original range producer and complete body restored;
// frame 0x14 and last add/third owner-field load scheduling remain.
// b14E (11-17) walk against retail, fast build on r847: the body matches retail statement by statement (both ITEM_GET
// reloads, the seed store, the owner stores in the order unknown4/unknown0/unknown8, the 0.0f/NULL pushes). The one
// difference is 0x10d583 `fadd st(1)` (the delay's final add), which retail issues between the c4 field load (0x10d57d)
// and the c2 load `mov cx` (0x10d585); ours issues the c2 load first. Tried, same 2: each field read through
// ITEM_GET(item_index) (the loads fold into one pointer); `owner` declared before `delay` or before `owner_item`;
// `location_c2` as word; the helper returning `lower + span * sample`. Worse: the c2 copy before c4 (5, register
// assignment changes), brace-initialising the owner {c4, c8, c2} (6, c4 loaded first), the helper with a named sample
// local (76, SSE). No behaviour difference found against retail.
// existing full random-range helper with the item seed 76; existing slot sample feeding the product-first
// range 29; existing normalized sample with the explicit seed 29; existing integer draw with the original
// normalization 2; existing object getter for the post-draw owner source 2; existing object getter for both
// independently observed items 2; existing bounds type for the adjacent delay endpoints 2; owner source
// scoped to filling its packet 2. The integer draw, object lookups, bound pair and packet scope preserve the
// same two instructions. The normalized draws move other instructions, and the full range helper grows the
// body. Opening executable source restored.
// Matched in a full build with stand-in SDK headers (was 2): item_real_random_range returns its sum inside one pair
// of parentheses, which puts the delay's last fadd before the field load as in retail.
// @retail 0x10d4e0
void function_10d4e0(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	s_item_definition *definition = (s_item_definition *)g_4e3b44[item->definition_index & 0xffff].bytes;

	if (item->value_12e == 0)
	{
		real delay = item_real_random_range(definition->delay_lower, definition->delay_upper);
		s_item *owner_item = ITEM_GET(item_index);
		s_effect_owner owner;

		owner.unknown4 = owner_item->location_c8;
		owner.unknown0 = owner_item->location_c4;
		owner.unknown8 = owner_item->location_c2;
		function_176780(item_index, &owner, 0.0f, definition->effect_tag_index, 0.0f, NULL, NULL);
		item->value_12e = (short)real_to_long((real)g_510c54->field_2_3 * delay);
	}
}

void function_b9a90(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
void function_b8b70(long object_index);
void function_b7300(long object_index);
void __stdcall function_b87b0(long object_index);
void function_b7290(long object_index);
bool function_b9d20(long object_index);

struct s_predicted_resource_block;
struct s_object_list;
extern s_object_list *g_4de2f4;
bool function_16e5e0(s_predicted_resource_block const *block, short mode);
void __stdcall function_b8600(long object_index, long unknown);

// @retail 0x10cdf0
void function_10cdf0(long object_index)
{
	s_item *item = ITEM_GET(object_index);
	if (item->definition_index != NONE)
		function_16e5e0((s_predicted_resource_block *)(g_4e3b44[item->definition_index & 0xffff].bytes + 0xb4), 1);
	item->flag1 = false;
	item = ITEM_GET(object_index);
	item->object_flags &= ~0x80;
	if (!(bool)((item->object_flags >> 8) & 1) && g_4de2f4 && *(byte *)g_4de2f4)
		function_b8600(object_index, 0);
	item = ITEM_GET(object_index);
	if (*(byte *)&item->object_flags & 1)
	{
		item->object_flags &= ~1;
		if (function_b9d20(object_index))
			function_bef30(object_index, 0, 1, 0, false);
		function_b8b70(object_index);
	}
}

/* puts an item in an inventory: detaches it and takes it out of the world */
// @retail 0x10cd50
void function_10cd50(long item_index)
{
	s_item *item = ITEM_GET(item_index);

	if (item->parent_index != NONE)
		function_b9a90(item_index);
	item->flags_12c |= 2;
	item = ITEM_GET(item_index);
	if (!(item->object_flags & 1))
	{
		if (function_b9d20(item_index))
			function_bef30(item_index, 1, 0, 0, false);
		item->object_flags |= 1;
		function_b8b70(item_index);
	}
	item = ITEM_GET(item_index);
	function_b7300(item_index);
	if ((bool)((item->object_flags >> 8) & 1))
		function_b87b0(item_index);
	item->object_flags |= 0x80;
	function_b7290(item_index);
}

struct s_slot_entry_list;
extern s_slot_entry_list *g_4e0340;
struct s_bsp3d;
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, long plane_index, plane3f *plane);
real function_30bf0(vector3f *vector);
void function_b75a0(long object_index, point3f const *position, vector3f const *forward,
	vector3f const *up, s_location const *location, bool flag);

static __forceinline void item_cross_product(vector3f const *a, vector3f const *b, vector3f *out)
{
	out->i = a->j * b->k - a->k * b->j;
	out->j = a->k * b->i - a->i * b->k;
	out->k = a->i * b->j - a->j * b->i;
}

// @retail 0x10b360
void function_10b360(long object_index)
{
	s_item *item = ITEM_GET(object_index);
	byte *definition = g_4e3b44[item->definition_index & 0xffff].bytes;
	if (item->parent_index == NONE && !TEST_FIELD_BIT(item->object_flag7) &&
		(definition[0xbc] & 1) && !(fabs(item->up.k - 1.0f) < 0.0001f))
	{
		item->up = *g_4687b0;
		vector3f *up = &item->up;
		vector3f *forward = &item->forward;
		vector3f side;
		item_cross_product(&item->up, &item->forward, &side);
		real forward_k = up->j * side.i - up->i * side.j;
		real forward_j = up->i * side.k - up->k * side.i;
		real forward_i = up->k * side.j - up->j * side.k;
		forward->i = forward_i;
		forward->j = forward_j;
		forward->k = forward_k;
		if (function_30bf0(forward) == 0.0f)
			*forward = *g_4687a8;
		function_b75a0(object_index, &item->position, forward, up, &item->location, false);
	}
}
vector3f *random_unit_vector(vector3f *result, dword *seed);
struct s_location;
void __stdcall function_b77d0(long object_index, vector3f const *linear_velocity,
	vector3f const *angular_velocity);

struct s_item_collision_surface
{
	short plane_index;
	byte unknown02[6];
};

struct s_item_collision_data
{
	byte unknown00[0x2c];
	s_item_collision_surface *surfaces;
};

struct s_item_collision_instance
{
	transform4x3f matrix;
	short section_index;
	byte unknown36[0x58 - 0x36];
};

struct s_item_collision_bsp
{
	byte unknown000[0x13c];
	byte *sections;
	byte unknown140[4];
	s_item_collision_instance *instances;
};

// score 190 (was 198): Frame 0xa0, both plane calls and instance addressing agree. The rotated normal and contact-point registers still differ.
// Earlier detail: Frame 0xa0 and both plane calls agree. The transformed normal, contact point and pending x87 spin scale still use different homes.
// MiniMax C (11-19): no new form, no lower score (190): retail keeps the spin scale in the x87 stack across the branch
// merge where this build gives it a stack home, and the plane distance written as one expression scores 203 against the
// four accumulating statements; the body stands.
// MiniMax E (11-05): no new form, no lower score; the four-term plane-distance accumulator written as one
// expression gives 211, the same sum without the redundant parentheses 205 and the contact point written as
// three products followed by three adds 207 - all worse. Reverted.
// MiniMax D (11-01): no new form, no lower score; the frame still gives the speed slot 0x10 and the spin slots
// 0x20 where retail has the spin at 0x10 and the speed at 0x28, so the spin scale stays in a stack slot instead
// of x87 st(1) and the plane joins keep their own xmm order.
// MiniMax E (11-16): the transformed-plane rows written as accumulating statements score 176 (the compiled tree then matches retail's (up*k + left*j) + forward*i) but that is under the keep threshold, so the body stands; a double spin scale keeps it in x87 but grows the frame to 0xa4 (238), so the scale still sits in a stack slot where retail holds it in st(1).
// MiniMax B (11-18): no new form, no lower score (198): the marker's coordinates still come from different slots than retail's and the plane-normal and spin/speed homes remain; the shared receiver gains six points and the transformed-plane rows 22, both under the eight-point minimum, so the body stands.
// MiniMax G (11-18): no new form, no lower score (198): the frame 0xa0 agrees with retail; the instance indexing, the plane-normal rows and
// the spin/speed homes still differ (the spin scale sits in a stack slot where retail holds it in st(1)), and the accumulating-statement form
// of the plane distance scores 176, under the keep bar, so the body stands.
// MiniMax A (11-18): no new form, no lower score (198): the frame 0xa0 agrees with retail; the transformed-plane rows
// written as accumulating statements fix the sum's association to retail's tree and measure 196, and the accumulating
// plane distance measures 196 as well, both under the eight-point minimum, so the body stands; the marker's coordinates
// still come from different slots, the instance lookup folds the base into a lea where retail adds it, and the position
// products stay in fresh registers where retail overwrites them in place.
// Earlier detail: the 0xa0 frame and transformed instance address agree. The rejection span, plane-normal and contact-position XMM homes, and pending spin scale still differ.
// Earlier score 200.
// Earlier header detail: full build on r803, where the walk's fast build on r727 gave 202 -> 200): b6J statement-by-statement walk against retail (11-06): no behaviour difference found.
// Checked: flag7/parent/flag0 early-out 0x10cfb4..0x10cfcb; trigger test and definition flag 0x10cfd1..0x10d00c; |impulse|^2 < 1e-4 skips
// the settle call 0x10d056 (ja 0x10d245); contact marker query args 0x10d05f..0x10d06d; surface_index == NONE test 0x10d082; instance
// row 0x58, section 0xc8 + 0x70 and matrix products (up.k, left.j, forward.i order) 0x10d0c2..0x10d19f; distance sum z,y,x then -d
// 0x10d1b8..0x10d1ed; 0.05 - distance 0x10d1f1..0x10d1fb; spin gate (ignore_object NONE, flag0 of 0xc1, flag5 or flag6) 0x10d245..0x10d275;
// axis from marker up or the g_4687b0 vector 0x10d293..0x10d2c8; random range -pi/2..pi/2 0x10d2d6..0x10d2e0; sqrt sum order j,k then i
// and float rounding of the speed (fst dword) 0x10d310..0x10d328; seed step, unsigned >>16 convert and 1/65535 0x10d338..0x10d35c; cross
// product components and the length test 0x10d366..0x10d3d1; second seed draw x speed x pi/2 0x10d3de..0x10d413; linear (velocity + impulse)
// and angular (spin + angular for i, angular + spin for j and k) sums 0x10d439..0x10d4a7; all constants read from retail match.
// Only change: the path-A plane copy runs d, k, j, i (retail loads them in that order, 0x10d0a5..0x10d0bd; evaluation order only), 202 -> 200 in that fast build.
// Precision difference (not fixed): retail keeps the spin scale unrounded on the x87 stack from 0x10d2e0 / 0x10d413 to the final fmul
// st(1) at 0x10d439 (spin i/j are float-stored products 0x10d2eb, 0x10d41f, and k is scaled at 0x10d439 after the join); ours stores the scale
// as a float first. Retail passes the plane index to 0x14a300 sign-extended (movsx edx at 0x10d097 and 0x10d0f4; the other callers do the
// same, e.g. 0x17ddcb and 0x245870); ours has a word move. Declaring 0x14a300's parameter `long` (the definition in src/unknown_14a280.cpp and the
// declarations in items.cpp, unknown_17d9a0.cpp, unknown_181a10.cpp, unknown_1fa590.cpp) keeps 0x14a300 MATCH and gives 200 -> 198 here (both movsx, fast build), not applied: other files.
// Tried: y*j + z*k term order for the distance (200). Frame 0xa0 agrees; plane-index width, plane joins and pending spin-k homes differ.
// Earlier 202 (was 211): Frame 0xa0 agrees; protected plane-index width, plane joins and pending spin-k homes differ.
// Earlier 211 (was 249): one shared spin scale for both spin branches, with the third spin
// component scaled after the branch so the pending value and the scale stay on the x87 stack.
// Plane-index width, plane-join registers and the transform product rows still differ.
// Earlier 249 (was 249): Frame 0xa0 agrees; protected plane-index width, plane joins and spin stores remain.
// plane-index width, plane-join registers and spin stores differ;
// seed before square root 249; fallback input copy 364; up pointer 253;
// plane input scalars 254; angular output initializer 253.
// Shared input/output scaling 249; transform value 350; linear/angular loops 249/249; normal-row loop 250.
// plane-index width, plane-join homes and spin stores remain.
// Item/transform references 249/249; surface selector 250;
// speed-role names 249; current-spin reference 249 (plain).
// Round 342: linear-output aggregate 249; native axis CRT copies 249; direct effect-flag read
// 249; shared fallback-only spin scale 249; direct offset expression 249.
// Complete plane memcpy 305; all normal-row partial projections 250; all row products named 250;
// contact-position projection named 249; paired shared spin scales 254. Opening body and includes retained.
// Entry tests flag7/parent/flags_12c in orders 213/132/231/312/321 253/251/265/264/254; reversed spin flags 249;
// reversed spin flags with 213/132 entry orders 253/251.
// Shared matrix rotation alone/point setter 249/249; with shared distance/both 250/250;
// native whole-plane transfer on those four rotation graphs 305 each. Exact-only forms restored.
// Round 376: Complete axial/fallback/both spin outputs 281/279/298; CRT publication 281/279/298;
// both outputs declared before marker with assignment/CRT 298/298. Original spin workspaces,
// inputs and includes restored.
// Round 205: Common seed pointer after admission/before buffers/after buffers 325 each;
// at the spin-branch boundary 280. Corresponding native field references 325/325/325/280.
// Original receivers restored; protected plane-index width, plane joins and spin stores remain.
// Round 188: Paired final linear/angular rows ijk/ikj/jik/jki/kij/kji, then angular-first
// paired rows ijk/kji:
// 249,249,248,248,249,249,249,249 fast; reduced control 249, full 249. Original executable source restored.
// Round 208: six uniform normal-column operand combinations, native cross-product
// operands, and joint complete normal/distance products all 249. Sum trees,
// plane inputs, random draws and the whole original body restored.
// Round 285: existing staged spin cross/shared final linear sum/both/shared final angular sum/
// cross-angular/both sums/all/all with marker-first storage: 249/249/249/249/249/249/249/249 fast.
// Original complete body restored; protected plane width, join homes and spin tail remain.
// Round 434: complete axis/impulse/both snapshots 342/345/351; with the existing staged cross
// 342/345/351; both with a native material index, without/with staged cross 350/350. Combined
// captures grow the frame to 0xac; original observations, plane interface, spin workspaces and body restored.
// DL202: Double pending spin scale, with uniform factor ownership and native linear/angular/both
// output initializers, all eight subsets 252 each (1342/1361 bytes, frame 0xa4 versus 0xa0).
// Original real factor, native outputs and complete body restored; protected plane-index
// width, plane joins and pending spin-k homes remain.
// Round 252: Spin/axis/final-output lexical name families in all seven subsets, then all with contact-
// position naming 211/211/211/211/211/211/211/211. Original complete executable source restored; Protected
// plane-index width, plane joins and pending spin-k homes remain.
// Round 266: untransformed plane copies dijk/kjid/kijd/jikd/ikjd/jkid/dkij/kdji
// 211/210/211/211/210/209/211/210. Two-point best gain rejected; original copies and
// complete body restored. Protected width, plane-join and pending spin homes remain.
// Round 276: Existing scalar setters at
// untransformed/transformed/both-output/untransformed-transformed/untransformed-outputs/transformed-outputs/all
// transfers 210/211/211/210/210/211/210; all with axis copies 269. One-point leads rejected.
// Original executable source restored; Frame 0xa0 agrees; protected plane-index width, plane joins and pending spin-k homes remain.
// Round 288: Existing integer-step/fraction helpers at the speed/final draws on the common spin-k
// join, eight combinations 211/211/211/211/211/263/263/263. Integer-step and final-only fraction forms
// tie; speed-fraction forms shrink to 1331 bytes and regress. Original expanded draws, common scale,
// protected plane interface and whole body restored.
// DL224: Axial source pointer/conditional pointer/selected external reference 311 each; all
// original spin products at the common join on those three graphs 328 each; shared float scale
// with pointer/reference 328 each. Original source observations, random draws, pending-k join and
// helper interfaces restored; protected plane width and plane/spin homes remain.
// Round 461: Common collision-plane query with field/Boolean selection, branch/conditional
// input and either plane declaration order: 290/348/290/348/290/348/290/348.
// Boolean selection raises sibling 140 to 235; original query branches and complete body restored.
// Round 310: Single spin workspace with native/CRT/setter/indexed axial capture, original/early
// factor declaration: 274/274/274/274/255/255/272/272. Original axis, real factor, pending-k
// join and complete body restored; protected plane width and plane/spin homes remain.
// Round 296: Standard native-array normal/paired-axis/paired-spin producer subsets 1..7
// 322/272/293/351/414/326/389; all with standard linear addition 375. Normal range capture grows
// frame to 0xa4; original complete body, protected plane interface and include set restored.
// Round 309: Genuine uninitialized final-linear/final-angular/both-plane output declarations reserved at
// entry, subsets 1..7, then all after admission: 213/211/213/211/213/211/213/213. Original declaration lifetimes
// and body restored; plane width, plane joins and spin/speed homes remain.
// Round 289: Consistent item/matrix/collision reads at each use, subsets 1..7, then all with
// paired shared final sums: 459/328/443/308/474/331/447/474. All regress; original receivers,
// observations, helper calls, protected plane interface and complete body restored.
// Round 312: Both getter results consumed through actual returned plane inputs, with
// scalar/native untransformed publication, original/setter transformed normals and both
// plane declaration orders: 261/283/261/283/261/283/261/283. Native plane publication
// grows frame to 0xa4; original producers and full body restored. Protected index width,
// plane joins and spin/speed homes remain.
// N315: Shared actual collision-instance pointer/readonly pointer/reference/readonly
// reference, with separate/direct matrix fields, 211 each (1365/1361 bytes).
// Original receivers, matrix observations and body restored; plane width, join and spin homes remain.
// N317: Actual fallback-speed scalar reserved at workspace/entry, crossed with
// earlier common-factor and axial-vector storage, all eight forms 211. Original
// uninitialized declarations and complete body restored; protected plane width,
// plane-join allocation and native spin/speed homes remain.
// N319: Axial scalar setter with separate axis retained, crossed with fallback/final setters and exact-group length provider: eight 211. Original producers and full body restored; protected plane width and plane/spin homes remain.
// Round 541: Separate actual contact/axial marker outputs, contact-query/axial-function/spin-after-contact
// lifetime subsets 0..7: 217/211/217/217/217/211/217/217. Expanded frame 0x110; narrow
// contact scope reproduces 0xa0 and the original code. Original marker, spin and body restored.
// DH314: Plain two-component axial spin loops, long/int/short/unsigned-long ordinals:
// 211/211/288/211; both axial/fallback loops: 211/211/296/211. Short loops grow;
// all controls unchanged. Original body, pending-k join and protected plane contract restored.
// B392: Plain normal transfer on every graph with axis/final-output loops and actual matrix-reference
// masks 0..7: 206/303/206/303/206/303/206/303. Original complete body and protected plane interface
// restored; plane joins and pending spin-k homes remain. NOT kept: sub-eight-point lead above 30.
// P292: Common contact completion, bool/byte/word/long admission, compound then nested
// query guards: 293/293/288/291/293/293/288/291. All regress and grow beyond retail length.
// Original one-test contact graph, observations, marker, plane/spin producers and full body
// restored; protected plane-index width, plane joins and pending spin-k homes remain.
// DH321: Real/double raw pending-k carriers after i/j scaling, optional random vector,
// final seed update or scale production: 211/219/219/211/293/286/286/295. NOT kept: one-use/precision
// forms lack equality; original spin, random draws, protected plane contract and body restored.
// DH314: Plain two-component axial spin loops, long/int/short/unsigned-long ordinals:
// 211/211/288/211; both axial/fallback loops: 211/211/296/211. Short loops grow;
// all controls unchanged. Original body, pending-k join and protected plane contract restored.
// DL255: Contact/angular provider ordinary/inline/forced policies, eight pairs:
// 211/386/211/712/211/712/386/734. NOT kept: ties/regressions; forced expansion loses
// 0xd9640 and/or 0x10b190 and raises other users. On the shared spin-k graph, existing
// staged cross/linear add/both: 205/211/209 (1366 bytes). NOT kept: gains six/zero/two,
// below threshold; 0x1c4590 6 -> 11 and 0x24cf66 114 -> 115 also rise.
// Original complete body, vector expressions and both provider declarations restored.
// Native normalization result, squared-speed producer and measured/selected-speed
// ownership on the common pending-k graph: 211/211/245/211/245/245/245;
// complementary speed admission 252. Original complete body and protected plane contract restored.
// B407: Joint pending-factor/fallback-speed scalar identities on the common pending-k
// graph, eight lexical relations: 211 each; all file controls unchanged. Original
// names and complete body restored; plane joins and scalar/spin homes remain.
// N349: Actual contact displacement via scalar/shared-scale producers and field/shared-add/native-point
// publications 211/317/211/317/211/317/211; copied point then scaled additions 282.
// Original complete body restored; protected plane width, plane joins and spin/speed homes remain.
// NOT kept: unmatched displacement/copy forms and frame/extent regressions.
// N356: Bare contact distance/offset storage at plane/marker/entry boundaries, with fallback-speed storage:
// 211/211/211/211/211/211/211/211. NOT kept: unmatched storage/result-lifetime forms.
// Original executable source restored; Protected plane width, plane joins and speed/factor/spin homes remain.
// N358: Complete common spin scaling with separate/direct axial source, private/shared float/shared double/plain component producers:
// 306/306/331/306/282/282/305/282. NOT kept: unmatched source/representation forms; original executable body restored.
// Protected plane width, plane joins and speed/factor/spin homes remain.
// N360: Complete linear production before pending spin-k, fields/initializer/setter/shared-add, then angular initializer:
// 211/211/211/211/211/211/211/211. NOT kept: unmatched source/result-storage forms; original executable source restored.
// Protected plane width, plane joins and spin/speed homes remain.
// N362: Existing marker/normalizer/random-vector/random-scalar inline call visibility,
// four individuals, three random-scalar pairs and all four: 211/211/211/211/211/211/211/211. Original
// declarations and complete body restored; protected plane width, plane joins and pending spin homes remain.
// N366: Actual plane-distance output owns its original four-term accumulator, 202 plain, retained;
// complete normal compound fields/scalars/setter 209 each, with distance 202 each;
// isolated distance also 202. Frame 0xa0 and 1366/1361 bytes unchanged. Normal forms restored;
// protected plane-index width, plane joins and pending spin/speed homes remain.
// N369: Final untransformed query destination, original/branch-owned input plane 206/206;
// complete normal scalar inputs mutable/const 207/207, all four inputs 209/209;
// safe in-place result with captured normals mutable/const 216/216.
// NOT kept: no equality or gain; in-place extent grows. Original body restored.
// F391: Standard backward axial-input/normal transports and complete paired final range sums,
// individual/pair/complete graphs, then backward final traversal: 267/343/370/313/397/334/343/411.
// NOT kept: all regress; normal/range graphs also displace the native frame or extent. Original
// planes, axis/spin homes, arithmetic, random draws, pending-k join, calls and include set restored.
// Paired range sums also raise matched 0xd9640 from 0 to 3 in the reduced image; restored.
// Protected plane-index width, plane joins and native spin/speed homes remain.
// AC534: Actual contact-distance accumulator at whole/pair/individual product boundaries,
// with direct/compound offset subtraction, then all three named products:
// 202/204/204/202/204/204/202/206. NOT kept: expression-boundary nonmatches; original
// complete body restored. Protected plane interface, plane joins and spin/speed homes remain.
// AC536: Contact-query count bool/short/long/dword, then both query counts independent/shared
// short/long: 202/202/202/202/202/202/202/202. NOT kept: no equality or qualifying supported gain.
// Original query consumers, plane interface, random draws and full body restored.
// AC538: Complete axial-spin decision, positive/complementary bool/byte/short/long:
// 204/204/204/204/204/204/204/204. NOT kept: unmatched decision/storage forms; original complete body
// restored.
// Protected plane-index width, plane joins and spin/speed homes remain.
// Y592: Independent branch query-input planes/scalar plane outputs/contact-consumer declarations, masks 1/2/3/4/5/6/7; all with role names:
// 204/207/206/202/204/207/206/206. NOT kept: no exact storage form; protected plane width, plane joins and spin/speed homes remain.
// Y595: Final angular output owns spin, scalar/existing setter at original/entry/effect/contact boundaries:
// 210/210/210/210/210/210/210/210. NOT kept: no byte equality; original complete body restored.
// B443: Independent complete axial scalar inputs, six capture/declaration orders,
// then spin-phase/entry reservations: 272/272/272/272/272/272/272/272. NOT kept: all regress,
// frame grows to 0xa8 and extent to 1383; original axis/body restored.
// DH365: Existing plane-normal/matrix-rotation/marker-coordinate views, seven subsets,
// then all with native spin/output coordinates: 202/202/202/202/202/202/202/202. NOT kept: no equality
// or gain; original complete body restored. Protected width and plane/spin homes remain.
// N371: Single-marker spin lifetime after contact, factor-first/enclosed-phase/scalar-role-name
// subsets 0..7: 202/202/202/202/202/202/202/202. NOT kept: unmatched storage-only forms;
// original complete body restored; protected plane width, plane joins and spin/speed homes remain.
// N386: Complete CRT plane/marker-axis/fallback-axis transports, subsets 1..7:
// 337/251/324/202/333/268/307; native copy construction at all three boundaries 307.
// NOT kept: no byte equality; original planes, axis copies, arithmetic and full body restored.
// F391: Standard backward axial-input/normal transports and complete paired final range sums,
// individual/pair/complete graphs, then backward final traversal: 267/343/370/313/397/334/343/411.
// NOT kept: all regress; normal/range graphs also displace the native frame or extent. Original
// planes, axis/spin homes, arithmetic, random draws, pending-k join, calls and include set restored.
// Paired range sums also raise matched 0xd9640 from 0 to 3 in the reduced image; restored.
// Protected plane-index width, plane joins and native spin/speed homes remain.
// AC560: Actual optional-effect admission at its original guard, positive/complementary
// bool/byte/short/long: 204/204/204/204/204/204/204/204. NOT kept: one-use outcomes do not match; original
// complete body restored. Protected plane width, plane joins and spin/speed homes remain.
// F395: Complete transformed-normal publication ikj/jik/jki/kij/kji on the retained distance
// accumulator 203/206/205/206/204; dependency-preserving normal/distance publication
// 206/205/200. NOT kept: two-point best below the keep threshold; original body restored.
// Protected plane-index width, plane joins and native spin/speed homes remain.
// P544: current baseline already sign-extends both plane-query indices correctly;
// instance-address scheduling, transformed-plane registers and spin/speed homes remain.
// Whole plane/marker-axis/fallback-axis/both-axis memmove 336/247/196/264;
// whole-object forward copies 342/240/194/263; shared instance receiver 192,
// shared signed ordinal 207 (receiver/selector revisited after query-width repair).
// NOT kept: fallback copies gain only two/four and the receiver only six, below
// eight above 30; original complete body, observations and calls restored.
// P564: Actual instance-table base captures or two-step advanced receivers, mutable/readonly
// and existing/direct matrix consumers: 201/288/201/288/192/192/192/192. NOT kept: six-point best below
// threshold; base/direct forms also raise controls. Original instance reads, plane math and body restored.
// MiniMax E (11-16), NOT kept: position.y built from marker.matrix.position.x scored 178 (main 198), but it
// changes what the code does: retail adds the marker's y (the same [esp+0xa4] slot the distance product reads, at
// [esp+0xac] after the two pushes). Body back to main.
// P578: Actual surface/section/material signed ordinal memcpy acquisitions, seven subsets,
// then paired plane-index field transfers: 198/198/198/198/198/198/198/198. NOT kept: no exact acquisition/frame-home
// recovery; original complete body, plane interface, observations and arithmetic restored.
// MiniMax C (11-16): no new form, no lower score (198): the marker's coordinates still come from different slots than retail's and the position's three components keep their own homes, while the two marker lookups and the event helper stay in retail's order; the marker's y is the one read, as retail does.
// P591: Actual matrix receiver before the query/section derivation, ordinary/fixed
// pointer and readonly/mutable reference: 239/239/239/239/239/239/239/239. NOT kept: all regress to
// 1367 bytes; original receiver and full body restored. Instance/plane/spin homes remain.
// MiniMax S (11-17): no new form, no lower score (198): the marker's coordinates still come from different slots than retail's and the position's three components keep their own homes, while the two marker lookups and the event helper stay in retail's order.
// Typed section with embedded collision 198; const section 198; instance
// receiver shared with the matrix 192; shared signed surface index 198;
// named section index 198; section reference 253; collision member reference
// 198; getter result used in the normal rows 252. NOT kept: the six-point
// receiver gain falls short of the threshold. The instance address sequence
// and native plane and spin register allocation still differ.
// Transformed surface first 220; separate section displacement 198; instance byte
// displacement 235, with section displacement 235 or one surface read 235; typed
// base and section displacement 201; shared byte-displaced receiver 192; collision
// displacement 235. NOT kept: the shared receiver gains only six; instance indexing
// still adds the base before the section read.
// instance table field binding through section and transform reads 198; section table field binding for the
// selected geometry 198; signed instance selector field in the guard and both reads 198; instance and section
// table fields kept distinct through the query 198; signed selector and both table fields at their native
// observations 198; section ordinal bound to the indexed instance field 198; signed selector and section
// field on the shared instance record 192; world and instanced surface records supply the two plane indices
// 198; shared signed instance selection with direct section field use 192; direct section field and all three
// transformed normal accumulators 190. The first eight trials include the shared instance selector at 192.
// NOT kept on its own: the exact instance-address sequence gains six at the original extent. The direct
// section field with all three normal accumulators scores 190 in both checks at 1366 bytes and preserves the
// 0xa0 frame. Retail reads the section index before adding the instance base, then adds the up and left
// products before the forward product. The shared instance and staged normal rows are retained. All plain
// controls keep their opening scores.
// Spin k finished in each branch, carried to the join, published with angular velocity, or finished with a
// shared factor. Real factors 233/233/233/197; double factors 238/250/250/238. NOT kept: the shared real
// factor recovers spin, axis and speed homes and the x87 scale at 197, but raises the score overall.
// Round 640: contact point accumulates normal displacement 199; updates a marker copy 267;
// shared scale and point setter 199; indexed projection 190; two coordinate passes 492;
// complete point initializer 190; named displacement components 194; scaled vector plus
// indexed marker sum 199. Transformed plane and contact point homes still differ.
// Marker X and Z kept for both calculations, Z declared first 190.
// Marker X and Z kept for both calculations, X declared first 190.
// Marker X Y and Z captured with the distance terms' Z Y X order 190.
// Marker X Y and Z captured in coordinate order 190.
// Marker X and Z kept as mutable scalar values 190.
// Complete marker point value supplies both calculations 261.
// Complete marker coordinates initialized at the join 190.
// Marker Z at the first distance sum and marker X at its contribution 192.
// NOT kept: six marker coordinate forms tie at 190. The complete point copy grows the body
// and the split distance expression loses two points. The transformed normal and contact
// position still use different XMM homes, and the spin scale still spills.
// Axis-wide field passes 194, Up scale helper with axis-wide field additions 194, Left and forward multiply-add
// helpers 194, Shared scale and multiply-add helpers for all axes 194, Indexed normal with a complete component
// expression 196, Indexed normal with a field accumulator 192, Separate scaled left vector before the first sum
// 248, Separate scaled forward vector before the final sum 194. NOT kept: The forward-vector form reaches 1365
// bytes at 194. The indexed field accumulator scores 192 at 1366 bytes. The transformed-plane and spin homes
// still differ.
// Rotating into a separate normal vector and publishing it after/before the distance gives
// 311/322 for compound or aggregate rows and 343/353 when copying the input normal first.
// NOT kept: every form grows the body, and early publication also grows frame 0xa0 to 0xa4.
// The transformed normal and contact position registers and pending x87 spin scale still differ.
// Queried normal coefficients carried through all rows, i/j/k and ordered pairs jk/kj/ji/ki/ik,
// 190 each. NOT kept: the first coefficient loads, contact-point home and pending spin factor remain.
// Left-axis products as a vector 192, all scalars 195, or row-local scalars 195;
// output carrying left products into the row sums 196 or all sums before forward 194;
// separate paired-product vector 193; separate left and up vectors 194; all three basis
// vectors 246. NOT kept: all regress. The transformed plane's j input still misses its
// retail XMM home through the three rows.
// Shared left coefficient 190, left and up coefficients 190. Basis field references or pointers 190/190.
// Left axis value 271, three independent axis values 350, embedded rotation reference or value 190/338.
// NOT kept: aliases and coefficients tie. Axis copies grow the body. The transformed-plane XMM homes
// and pending x87 spin scale still differ.
// Overlapping left products before normal-row completion, next-row/first-pair/early first sums/early last sums:
// 191/191/198/193 through plane fields, 191/191/191/191 through scalar sums. NOT kept: every schedule
// raises the score at 1366 bytes and frame 0xa0. The normal and contact XMM homes and pending spin factor remain.
// MiniMax E (11-19): no new form, no lower score (190, baseline): the plane transform still differs in its register
// assignment (the two component loads at [esp+0x30] and [esp+0x34] land in xmm3 and xmm4 here and in xmm2 and xmm4 in
// retail, and every row's product and sum then differs by one register), the body is still 1366 bytes against retail's
// 1361, and the branches inside the function still reach their blocks at other offsets.
// Left products seeded from the axis or coefficient in all rows, field/scalar 195/195 and 193/193.
// Naming each up product gives 191 on all four forms. NOT kept: the normal coefficient and
// contact registers still differ at the 1366-byte extent.
// Normal rows paired with translation products in coordinate order: late scale with expression or
// accumulator 203/199, early scale 199/201; in distance-sum order, late scale 203/199, early scale 199/201.
// NOT kept: the scalar products do not give a byte match or recover the retail coefficient homes.
// Coordinate-array views for queried normal, matrix axes and result normal, alone and
// in all combinations, then also at the contact join: 190 each. NOT kept: the 0xa0 frame
// agrees, and the transformed-plane and contact-point registers still differ.
// Left products from a carried j coefficient, j/k, all three at first use, or all three
// with overlapping rows: 191 each through plane fields or row scalars. NOT kept: j now
// loads before k, but its XMM home and the first axis operands still differ. The 0xa0
// frame agrees and the rebuilt body still exceeds retail by five bytes.
// Contact point declaration beside marker, at the surface arm, before the marker query or
// beside its plane workspaces. Direct stores and the shared point setter at each site all score
// 190. NOT kept: the placement slot and 1366-byte extent stay the same.
// MiniMax H (11-20): no new form, no lower score (190); the plane transform's component loads still land in xmm3/xmm4 where retail has xmm2/xmm4, the body is still 1366 bytes against retail's 1361, and a permuter run over the whole body found no variant below the baseline.
// Accumulated normal by axis 194, add-scaled left/forward/both 194/194/194,
// indexed rows with direct or local accumulation 192/192, shared contact
// translation 190, shared normal and translation 193. The coefficient loads
// and pending spin scale still differ.
// Vector setter for the rotated normal 193; scalar normal rows published before or after
// the distance translation 199 each; left product seeds all three normal rows 191;
// instanced query result pointer 246; instanced plane output with saved equation 291;
// named real or double random fraction 196/232. NOT kept: all scores exceed 190.
// The transformed normal and the common x87 spin factor still use different homes.
// Complete axis contributions staged before the normal rows. Left, up, forward, up/left, left/forward,
// up/forward, and all three 194/194/191/195/195/195/246. All three summed with the shared vector addition 253.
// NOT kept: partial staging raises the score and full staging also extends the body to 1382 bytes.
// The normal coefficient homes and contact point still differ in the 0xa0 frame.
// Overlap the j, k or both forward products in plane or scalar rows: 194 each.
// Overlap both left and forward products in plane or scalar rows: 191 each.
// NOT kept: the paired overlaps load j before k and still place j in XMM3.
// Every preview retains the 1366-byte extent against retail's 1361.
// Transposed rows through the shared dot product with coefficients before/after row setup:
// separate aggregates 269/269, separate vector setters 269/269, reused row setter 194/194,
// and a three-axis matrix 273/273. NOT kept: the reused row leaves the normal coefficients
// in other registers at 1366 bytes. The other forms grow to 1392 bytes.
// Dead normal scaled in place with field point publication 247 / In-place scale and point setter 247 /
// In-place scale and indexed point publication 247 / In-place scale and translated marker copy 283 /
// Marker point translated in place 200 / Corrected coordinates in dead normal with setter 247 /
// Corrected coordinates in dead normal with point initializer 247 / Corrected coordinates in dead normal
// by index 247.
// NOT kept: Reusing the normal makes the contact block longer, and the marker-point form reaches 200 at
// 1378 bytes. The earlier instance-plane coefficient homes still differ.
// The transformed-plane XMM homes and contact slot still differ. Angular impulse, contact point and travel speed slot names 190/190/190.
// Conditional const axis 259; shared double-factor vector scale in the axial arm, fallback arm, both arms or at the joined spin 239/239/238/319.
// NOT kept: the names tie and none of the scale forms recovers the pending x87 factor at a lower score.
// Completed transformed-normal rows i/k/j, j/i/k, j/k/i, k/i/j, k/j/i: 193/196/196/197/194.
// Overlapped row completion i/j, j/k, or all three in reverse: 201/198/195.
// NOT kept: the native coefficient registers and rejection span still differ.
// Plane-join homes differ and the real spin scale spills at the join. Wide axial, fallback or joined k
// products 240/240/198, both initial pairs 194, axial plus k 242, fallback plus k 241, all products 241.
// Wide scale with explicit real fallback rounding 237. NOT kept: wider products need a match. The joined
// k alone restores the +0x28 speed slot but shortens the body to 1342 bytes.
// Embedded rotation with direct translation, fully direct instance, or separately bound origin 190 each.
// Shared basis rotation into the plane, separate normal, separate normal through distance, early scaled
// distance or in-place queried plane 336/312/333/336/303. NOT kept: the shared rotation forms change
// sum grouping and shorten the body. The normal and contact-point XMM homes still differ.
// The transformed plane coefficient homes and the pending x87 factor still differ. Late axial,
// fallback or both factor assignments 233/233/232, with a separate raw third component
// 233/233/232. Separate raw third component alone 190; finishing its product at angular output
// 186. NOT kept: the latter reaches 1362 bytes but gains four, below the keeper threshold.
// Coefficient-first products across all left, up and forward rows, separately and in every combination,
// 190 each. All three axes with coefficient-first translation products 190.
// NOT kept: the first coefficient loads, 0xa0 frame and five excess bytes stay the same.
// Native basis-axis references for left, up, forward, each pair and all three score 190.
// The embedded basis reference also scores 190. Reading j/k/i once with the left-axis or
// embedded-basis reference scores 190/190.
// NOT kept: the first coefficient loads, 0xa0 frame and five excess bytes stay the same.
// Separate surface normal and distance with field or aggregate position, each with accumulated
// distance or a translation subtotal, 199 each. Copy the full untransformed normal into the
// separate vector, 288 for either distance form. Separate scalar coordinates, 199 for either.
// NOT kept: the coefficient homes still differ, and the whole-normal copy grows the body.
// Named up-plus-left, forward, left, or left and forward normal terms in each row
// or across all rows before publication: 193/193, 191/191, 191/191, 195/195.
// NOT kept: each raises the score at unchanged length. The coefficient registers
// and contact-point home still differ.
// Shared queried plane with scalar expression or accumulator rows, stored directly
// or through the setter: 244/244/242/242. Vector expression or accumulator rows
// 310/310. In-place rotation with scalar or vector coefficient capture 232/278.
// NOT kept: the single-plane forms grow the body and still miss the XMM homes.
// Native basis-axis references for left, up, forward, each pair and all three score 190.
// The embedded basis reference also scores 190. Reading j/k/i once with the left-axis or
// embedded-basis reference scores 190/190.
// NOT kept: the first coefficient loads, 0xa0 frame and five excess bytes stay the same.
// Pending spin k product scheduled between linear or paired output stores, including compound multiply:
// 190 in all eight forms. Last angular store with the direct or named product after the paired i/j
// outputs 186/186 at 1362 bytes. NOT kept: four-point gain below the threshold. The transformed-plane
// registers and pending x87 scale still differ.
// Named translation and scale products 203/203, named translation with a field accumulator
// 199/199, scaled source distance before translation in a local 203/203 or the output field
// 199/199, with direct or setter contact stores. NOT kept: all raise the score at the same
// extent. Normal coefficient and contact-point homes still differ.
// Translation terms after their rows, after the first two rows, after all rows, or with X
// early and Z before Y: 199/199/199/199 in both plane-row and scalar-row storage.
// NOT kept: the length and frame stay at 1366 and 0xa0. The queried coefficient order still differs.
// The rotated normal coefficients and pending x87 factor still use different homes.
// Seed-step in fallback speed 190, seed-step in spin factor 190, seed-step in fallback speed, seed-step
// in spin factor 190, site-tagged seed-step in fallback speed 190, site-tagged seed-step in spin factor
// 190, site-tagged seed-step in fallback speed, site-tagged seed-step in spin factor 190, seed-step in
// fallback speed, site-tagged seed-step in spin factor 190, site-tagged seed-step in fallback speed,
// seed-step in spin factor 190, planar normal dot with the basis row first 198, planar normal dot with
// coefficients first 198.
// NOT kept: The integer draw providers tie the baseline. Both planar products grow the body and raise
// the score.
// Existing planar scale in the axial, fallback or both branches 190/190/190; axial aggregate
// and in-place scale 288. Joined scale with separate/direct axis 294/270, joined separate
// planar output 294, branch-local separate planar output 266. NOT kept: the branch helpers
// tie, and the joined or separate-output forms shrink the frame and raise the score.
// The rotated normal registers and pending x87 spin factor still differ.
// M3 lane GB round 5 (40 trials, fast with 0x10c850/0x10cec0/0x10d4e0/0x10d5f0 all MATCH; control 190, 1366 B, al=109, mask=63):
// MAJOR LEAD: shared real spin_scale finishing all 3 spin.{i,j,k} in BOTH branches (or vector3f_scale in both) with
// `real r = (real)(*seed >> 16) * (1.0f / 65535.0f); spin_scale = (r * speed) * 1.5707964f;` (`B2_07`/`B2_08`) reaches 1364/1361 B,
// 326/325 insns, al=58 (vs 109), mask=35 (vs 63), recovering EVERY retail stack home, all 3 un-folded fmuls, retail's exact x87
// tail-merge at 0x10d431, and matching 98/98 opening + 157/160 closing insns (235 raw solely from +1 insn shift in lines 99-169).
// Also tried on main and B2_07: single-expr fwd+left+up / left+up+fwd / up+left+fwd plane 197-200; 2-stmt plane + no-paren dist +
// marker-first pos 242 (mask=32); x+y+z-d dist 193; surface_plane*=offset 249-250; branch-local spin factor 240 (al=66);
// double spin factor 244 (fr=0xa4); outer k + no-fold r 196; sp=plane copy 307 (fr=0xa4); normal+plane_d 244 (mask=33);
// inline dist/offset 235/244; single plane in-place 245. Body unchanged.
// Snapshot the up axis 273, forward 271, left and up 306, left and forward 305,
// up and forward 304, or all three axes 350. Complete basis with or without a
// shared left coefficient 338 each. NOT kept: all copies grow the body and
// raise the score. The original rotated-normal and contact registers still differ.
// Marker Y, Y/Z or X/Y shared through distance and correction 190 each.
// Y/X/Z corrected coordinate locals with fields, setter or initializer 190 each.
// Y/X/Z scaled normal components 194; paired distance contribution 190.
// NOT kept: all retain 1366 bytes and leave the normal and contact homes displaced.
// Shared unit-real draw for speed/spin/both 246/190/246. Sequential spin
// scale with the spin draw or both draws 196/240. Third product at the
// last angular store with the spin draw/both/both plus sequential scale
// 186/250/244. NOT kept: 186 at 1362 bytes gains four, and the instanced
// plane allocations still differ.
// Both signed plane queries agree. The left weight and contact-point homes differ.
// Weight-seeded left, up, forward, left/up and all plane products 191/192/190/193/193.
// Complete scalar rows with seeded left, left/up or left-basis products 191/193/191.
// The left coefficient and contact-point homes still differ. Shared plane
// query with optional instance, optional matrix, default world copy, or
// conditional geometry selection 338/338/351/329. Reading the returned
// plane in the same forms 363/363/367/262.
// NOT kept: each raises the score and merges retail's two query sites.
// @retail 0x10cf80
void function_10cf80(vector3f const *impulse, long item_index, bool trigger_effect)
{
	s_item *item = ITEM_GET(item_index);
	if (TEST_FIELD_BIT(item->flag7) || item->parent_index != NONE || (item->flags_12c & 1))
		return;

	if (trigger_effect && g_4e6948->state != 2)
	{
		s_item_definition *definition = (s_item_definition *)g_4e3b44[item->definition_index & 0xffff].bytes;
		if (TEST_FIELD_BIT(definition->flag1))
			function_10d4e0(item_index);
	}

	vector3f spin;
	s_object_marker marker;

	if (TEST_FIELD_BIT(item->flag5))
	{
		if (!(0.0001f > impulse->i * impulse->i + impulse->j * impulse->j + impulse->k * impulse->k))
		{
			if (function_b8d30(item_index, 0xc0000c1, &marker, 1, false))
			{
				plane3f plane;
				plane3f surface_plane;
				short const &instance_index = item->surface_index;
				if (instance_index == NONE)
				{
					s_item_collision_data *collision = (s_item_collision_data *)g_4e0340;
					bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
					surface_plane.d = plane.d;
					surface_plane.k = plane.k;
					surface_plane.j = plane.j;
					surface_plane.i = plane.i;
				}
				else
				{
					s_item_collision_bsp *bsp = (s_item_collision_bsp *)g_4e0348;
					s_item_collision_instance const &instance = bsp->instances[instance_index];
					s_item_collision_data *collision = (s_item_collision_data *)(bsp->sections + instance.section_index * 0xc8 + 0x70);
					bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
					transform4x3f const *matrix = &instance.matrix;
					surface_plane.i = matrix->up.i * plane.k;
					surface_plane.i += matrix->left.i * plane.j;
					surface_plane.i += matrix->forward.i * plane.i;
					surface_plane.j = matrix->up.j * plane.k;
					surface_plane.j += matrix->left.j * plane.j;
					surface_plane.j += matrix->forward.j * plane.i;
					surface_plane.k = matrix->up.k * plane.k;
					surface_plane.k += matrix->left.k * plane.j;
					surface_plane.k += matrix->forward.k * plane.i;
					surface_plane.d = matrix->position.z * surface_plane.k;
					surface_plane.d += matrix->position.y * surface_plane.j;
					surface_plane.d += matrix->position.x * surface_plane.i;
					surface_plane.d += matrix->scale * plane.d;
				}
				real distance = (marker.matrix.position.z * surface_plane.k + marker.matrix.position.y * surface_plane.j) +
					surface_plane.i * marker.matrix.position.x - surface_plane.d;
				real offset = 0.05f - distance;
				point3f position;
				position.x = surface_plane.i * offset + marker.matrix.position.x;
				position.y = surface_plane.j * offset + marker.matrix.position.y;
				position.z = surface_plane.k * offset + marker.matrix.position.z;
				function_b75a0(item_index, &position, NULL, NULL, NULL, false);
			}
			function_10c850(item_index);
		}
	}
	else
	{
		function_10c850(item_index);
	}

	real spin_scale;
	if (item->ignore_object_index == NONE && (item->flags_c1 & 1) &&
		(TEST_FIELD_BIT(item->flag5) || TEST_FIELD_BIT(item->flag6)))
	{
		vector3f axis;
		if (function_b8d30(item_index, 0xc0000c1, &marker, 1, false))
			axis = marker.matrix.up;
		else
			axis = *g_4687b0;
		spin_scale = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -1.5707964f, 1.5707964f);
		spin.i = axis.i * spin_scale;
		spin.j = axis.j * spin_scale;
		spin.k = axis.k;
	}
	else
	{
		dword *seed = &g_4e7408->unknown0;
		real speed = (real)sqrt((impulse->j * impulse->j + impulse->k * impulse->k) + impulse->i * impulse->i);
		if (speed < 0.0001f)
		{
			*seed = *seed * 0x19660d + 0x3c6ef35f;
			speed = (real)(*seed >> 16) * (1.0f / 65535.0f);
		}
		spin.i = g_4687b0->j * impulse->k - g_4687b0->k * impulse->j;
		spin.j = g_4687b0->k * impulse->i - g_4687b0->i * impulse->k;
		spin.k = g_4687b0->i * impulse->j - g_4687b0->j * impulse->i;
		if (!(function_30bf0(&spin) > 0.0f))
			random_unit_vector(&spin, seed);
		*seed = *seed * 0x19660d + 0x3c6ef35f;
		spin_scale = (real)(*seed >> 16) * (1.0f / 65535.0f) * speed * 1.5707964f;
		spin.i *= spin_scale;
		spin.j *= spin_scale;
	}
	spin.k = spin.k * spin_scale;
	{
		vector3f velocity;
		vector3f angular_velocity;
		velocity.i = item->linear_velocity.i + impulse->i;
		velocity.j = item->linear_velocity.j + impulse->j;
		velocity.k = item->linear_velocity.k + impulse->k;
		angular_velocity.i = spin.i + item->angular_velocity.i;
		angular_velocity.j = item->angular_velocity.j + spin.j;
		angular_velocity.k = item->angular_velocity.k + spin.k;
		function_b77d0(item_index, &velocity, &angular_velocity);
	}
	function_10d5f0(item_index);
}

struct s_item_impact_definition
{
	byte unknown000[0x1a];
	byte kind;
	byte unknown01b[0x58 - 0x1b];
	long effect_index;
	byte unknown05c[0x100 - 0x5c];
	long sound_index;
};

#include "impacts.h"
struct s_item_impact
{
	long type;
	byte unknown04[4];
	point3f point;
	byte unknown14[8];
	s_location location;
	c_type_47f957 material;
	byte unknown26[2];
	vector3f normal;
	byte unknown34[0x40 - 0x34];
	long object_index;
};

real function_1201a0(vector3f *v, vector3f const *fallback);
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	c_type_47f957 material, long effect_value, s_location const *location, real scale);
dword vector3d_compress(vector3f const *vector);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);

// @retail 0x10c880
void function_10c880(long item_index, s_item_impact const *impact, vector3f const *velocity, point3f const *point)
{
	s_item_header *headers = (s_item_header *)g_4e0300->data;
	s_tag_instance *tags = g_4e3b44;
	s_item *item = headers[item_index & 0xffff].item;
	s_item_impact_definition *definition = (s_item_impact_definition *)tags[item->definition_index & 0xffff].bytes;
	vector3f direction = *velocity;
	real scale = function_1201a0(&direction, g_4687bc) * (1.0f / 3.0f);
	scale = scale < 0.0f ? 0.0f : (scale > 1.0f ? 1.0f : scale);
	long effect_index = definition->effect_index;
	if (effect_index != NONE)
	{
		long kind = definition->kind;
		if (impact->type == 4)
		{
			byte other_kind = ((s_item_impact_definition *)tags[headers[impact->object_index & 0xffff].item->definition_index & 0xffff].bytes)->kind;
			kind = definition->kind > other_kind ? other_kind : definition->kind;
		}
		function_188180(&impact->point, &impact->normal, effect_index, item_index, kind + 10, kind,
			impact->material, (long)&direction, &impact->location, scale);
	}
	if (definition->sound_index != NONE)
	{
		s_sound_position sound;
		sound.position = *point;
		sound.compressed_forward = vector3d_compress(&impact->normal);
		sound.velocity = *g_4687a4;
		sound.location = item->location;
		function_1895f0(&sound, scale, definition->sound_index);
	}
}


void function_bfa40(long object_index, long a);
bool function_b9d20(long object_index);
void __stdcall function_bef30(long object_index, long remove, long add, long siblings, bool flags);
void function_b8b70(long object_index);
void function_bb950(long object_index, bool add, long delta);
void function_a7a60(long object_index);
void __stdcall function_a7870(long object_index);
bool function_100f00(long object_index);
void function_15e250(long object_index, long other_index);
void function_15e460(long object_index, long other_index);
void function_1060a0(long weapon_index, long unit_index);

// @retail 0x10ca80
void function_10ca80(long object_index, long owner_index)
{
    long const *owner_reference = &owner_index;
    s_record_pool *objects = g_4e0300;
    byte *data = objects->data;
    s_item *item = *(s_item **)(data + (object_index & 0xffff) * 12 + 8);
    byte *header = objects->data + (object_index & 0xffff) * 12;
    byte *item_flags = (byte *)item + 0x12c;
    struct s_node_flags { unsigned long : 29; unsigned long dirty : 1; unsigned long : 2; };
    s_node_flags *node_flags = (s_node_flags *)((byte *)item + 4);
    if (*owner_reference != NONE)
    {
        if (TEST_FIELD_BIT(node_flags->dirty) && object_index != NONE &&
            *(short *)((byte *)item + 0x112) != NONE)
        {
            if ((bool)((item->object_flags >> 29) & 1))
                function_bfa40(object_index, 0);
            item->object_flags &= ~0x20000000;
        }
        item->flag0 = true;
        item->flag2 = true;
        item->unit_index = *owner_reference;
        data = objects->data;
        byte *current = *(byte **)(data + (object_index & 0xffff) * 12 + 8);
        if (current[4] & 1)
        {
            *(dword *)(current + 4) &= ~1;
            if (function_b9d20(object_index))
                function_bef30(object_index, 0, 1, 0, false);
            function_b8b70(object_index);
        }
        item->flag1 = false;
        function_bb950(object_index, false, NONE);
        item->flag5 = false;
        item->flag6 = false;
        item->flag7 = false;
        *(long *)((byte *)item + 0x28) = NONE;
        *(short *)((byte *)item + 0x2c) = NONE;
        *(short *)((byte *)item + 0x2e) = g_4686c4;
        function_10dad0(object_index);
        function_a7a60(object_index);
        byte type = g_4e0300->data[(object_index & 0xffff) * 12 + 3];
        if ((1 << type) & 4)
        {
            if (TEST_FIELD_BIT(item->flag16c_6))
                function_15e250(object_index, *owner_reference);
            function_1060a0(object_index, *owner_reference);
            function_10ccc0(object_index);
            return;
        }
    }
    else
    {
        long old_owner = item->unit_index;
        if (g_4e6948->state == 1)
        {
            bool add = true;
            if (TEST_FIELD_BIT(item->flag3) && header[3] == 2 && function_100f00(object_index) &&
                !(((byte *)item)[0x12d] & 1))
                add = false;
            function_bb950(object_index, add, real_to_long((real)g_510c54->field_2_3 * 30.0f));
        }
        data = objects->data;
        s_item *current = *(s_item **)(data + (object_index & 0xffff) * 12 + 8);
        current->creation_time = g_510c54->game_time;
        *item_flags &= 0xf6;
        item->unit_index = NONE;
        function_10dad0(object_index);
        function_a7870(object_index);
        byte type = g_4e0300->data[(object_index & 0xffff) * 12 + 3];
        if (((1 << type) & 4) && TEST_FIELD_BIT(item->flag16c_6) && g_55e4d0[g_4e9ae8->engine_index])
            function_15e460(object_index, old_owner);
    }
    function_10ccc0(object_index);
}


#include <math.h>
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_b92d0(long index, s_object_marker const *old_parent, transform4x3f const *new_parent);

// MiniMax E (11-20): no new form, no lower score (130): our frame is 0xc0 against retail's 0xb4, the dot product associates as i, k, j where retail has j, i, k, and retail keeps the cross product in a register where ours has a stack home; the alignment dot as its own statement gives retail's j, i, k order (175) and writing the cross out in place also gives retail's frame and 848 bytes but scores 173.
// MiniMax E (11-20): no new form, no lower score: our frame is 0xc0 against retail's 0xb4, the dot product associates as i, k, j where retail has j, i, k, and retail keeps the cross product in a register where ours has a stack home; the inverted threshold test with a split dot scores 199.
// MiniMax B (11-20): no new form, no lower score (130): the cross result keeps a 12-byte stack home because its address is passed to the scale helper, which makes our frame 0xc0 against retail's 0xb4 and our locals sit 12 bytes higher; retail multiplies the dot as j, i, k and keeps the cross in registers, and a dot written as a separate statement reaches that product order but moves the old vector's components into other registers. Writing the cross out in place gives retail's frame and 848 bytes but scores 159.
// score 130 (was 130): frame 0xc0 versus 0xb4 and initial product ranks differ. Shared scale 130; inline
// depth one 130; size preference 277; complete rotated vector 207; fallback vector initializer 130.
// The axis as three real locals (no cross3f address) gives retail's frame 0xb4 and 848 bytes but 159.
// The square's products come out i, k, j whatever their source order; retail's j, i, k come only
// from a separate statement for the dot product, which then folds facing loads into mulss: 173-175.
// Square-to-root, root through half-root and doubled magnitude, and all four stages each score 130.
// Named reciprocal with half-root update 204. Separate axial contribution with and without the
// magnitude update 267/268. Final shared vector sum with and without it 194/194.
// NOT kept: frame remains 0xc0 and the scaled axis i still spills in the tied forms.
// Staged fallback side/up/both 130/130/130; fallback first with both staged 163.
// Named rotation cross numerators 130; named two-term rotation sums 130; named
// forward-projection products 130; sqrtf 130. The extra vector-sized frame region
// and the first alignment-product order remain.
// Staged cross / returned cross / output axis with aggregate / in-place fallback / common branch cross
// workspace / item dot / native rotation / output axis with setter / returned fallback / separate axis
// with setter / staged output axis with setter: 130/248/207/160/130/130/133/117/281/130/117.
// NOT kept: the two 117 forms recover the 0xb4 frame but spill the intermediate axis k to the
// result k home before its final store. Retail keeps that intermediate in a register.
// Scaled-axis scalars 130; forward scalars 130; separate shared-scale output 204; cross numerators
// staged together 130; reciprocal before cross 205; doubled half-root before projection 184;
// conditional point selections 130; normalized vector value 246.
// NOT kept: the scalar forms that tie retain frame 0xc0. The separate scaled vector, reciprocal and
// vector value grow the body, and the earlier doubled half-root regresses.
// Scalar scale after doubled or undoubled projection 133/130; later forward binding 130;
// named alignment products with one sum or staged accumulation 148/148; weighted cross
// contribution, with shared operations, or via the output workspace 224/277/277.
// NOT kept: the 0xc0 frame remains throughout and weighted cross forms grow the body.
// Item cross 130, mixed-product scale 130, shared rotation with axial or forward
// contribution first 273/278, shared i-k-j projection 154, value-returning cross
// with shared scale 248, and staged cross in place on the marker or facing operand
// 241/271. NOT kept: the shared projection recovers frame 0xb4 and 848 bytes but
// raises the score and changes its sum grouping.
// quaternion vector output 133, quaternion with direct components 133, quaternion scalar before scaling
// 187, quaternion scalar before cross 144, quaternion cross before root 133, quaternion with staged
// cross 133, staged quaternion with direct components and early scalar 187, quaternion with shared scale
// 133.
// NOT kept: The quaternion forms grow the frame to 0xc4 and raise the score.
// The 0xc0 frame exceeds 0xb4. The cross product has a separate stack home and the alignment products are scheduled differently.
// Scoped normalized scalars 206, initialized scaled vector 206, shared setter for scaling 204.
// Indexed scale 191, descending projection loop 195, first-component projection seed 130.
// Origin default at transform 174, output workspace with scaled scalars 206.
// NOT kept: the 0xc0 frame and rotation-axis spill remain. The projection seed ties.
// Rotation-axis, adjusted-forward and perpendicular names 130 each; transform-forward workspace 153.
// Projection in three additions, staged diagonal scale, grouped cross rows and interleaved rows 130 each.
// NOT kept: the transform workspace grows the function. The other forms retain frame 0xc0 and the axis spill.
// Raw shared or staged cross with scaled scalar, const-vector, setter or component-loop
// results: 206/206/204/191 for either cross. NOT kept: all retain frame 0xc0 and
// grow the body. The loop reaches 877 bytes with the axis home still present.
// Returned unscaled cross with scaled scalar coefficients 245, scaled j/i/k 244, separate alignment dot 237,
// same-tree projection 245, cross before root 245, early half-root 249, half-root after projection 245,
// named doubled scalar 245. NOT kept: removing the scaled-axis address this way retains the frame
// excess and grows the body to 909–919 bytes.
// Scale and accumulate projection 200 / Native projection-product loop 168 / In-place scale and
// projection with setter 188 / In-place last-use components 205 / In-place deferred rows 174 / Split
// alignment with in-place setter 166 / In-place workspace in both branches 161 / Shared cross and
// uniform rotation loop 220.
// NOT kept: The both-branch workspace recovers frame 0xb4 and 848 bytes at 161 differences. The
// alignment and projection-product forms also recover the frame and raise the score. Retail still
// retains the scaled axis in different registers.
// The 0xc0 frame and scaled-axis spill remain. Output cross/scale with scalar or vector capture 200/234.
// Separate cross scaled into output, with a final setter or scalar capture 183/202.
// Separate alignment via the item/shared dot with output cross 167/167, with separate cross 168/168.
// NOT kept: the best output-plus-dot forms recover 0xb4 but raise the score and remain seven bytes long.
// The scaled axis spill and initial product order remain. scale projection in place 151, name lateral subtotal
// 130, scale final expression from lateral subtotal 130, shared marker transform pointer 165, ordinary dot for
// alignment 130, shared fallback side cross 130, shared fallback result cross 130, staged axis cross and in-
// place projection scale 151.
// Shared double-factor scale with direct or staged cross, in-place or independent output,
// and rounded or double reciprocal: 197 each at 869 bytes.
// NOT kept: the 0xc0 frame remains and the scale expands as x87.
// The scaled axis i still spills and the frame stays at 0xc0. Marker-up-first alignment, axis-first projection,
// axis-first and forward-first rotation cross products, scalar-first diagonal, reversed axial and weighted cross
// products, and axis-first rows through the shared setter all score 130.
// Transform-left axis workspace 212. Common projection home with half-root update, final setter or
// forward scalars 151 each; separate weighted cross 267, staged cross with common weight 267,
// scaled-axis initializer 206 and scaled coefficients 206. NOT kept: the 151 forms recover 0xb4
// and 848 bytes but raise the score. The initial alignment and scaled-axis register order still differ.
// The 0xc0 frame and scaled-axis spill remain. Cross contribution before projection, with direct,
// staged, returned or staged-and-scaled cross into a separate vector 206/206/228/231; into the
// result workspace 190/190/228/228. NOT kept: both workspace families grow the rotation, and
// neither recovers the 0xb4 frame or the initial alignment-product order.
// Consume the existing scale helper's returned axis with direct or staged cross, an in-place or
// separate scale output, and a separate or in-place doubled projection: 205 in all eight forms.
// NOT kept: the 0xc0 frame and axis spill persist and the body grows to 878 bytes.
// The 0xc0 frame and scaled axis i spill persist. Parallel weighted cross, second products,
// final products, weighted cross with second/final products, and per-row scalar accumulation
// all score 130. Output accumulation in three passes or with the first pair combined scores
// 239/235. NOT kept: both output accumulations grow the body and retain the spill.
// Address-free normalized coefficients or scaled scalar numerators with member stores,
// the shared setter, or named rows and a setter score 159 each. Complete aggregate
// publication scores 204 with either producer. Shared or paired alignment on the
// address-free axis scores 159/172.
// NOT kept: the 159 forms recover frame 0xb4 and 848 bytes but score higher. The
// initial alignment-product order and the scaled-axis registers still differ.
// The 0xc0 frame and scaled axis i spill persist. Parallel weighted cross, second products,
// final products, weighted cross with second/final products, and per-row scalar accumulation
// all score 130. Output accumulation in three passes or with the first pair combined scores
// 239/235. NOT kept: both output accumulations grow the body and retain the spill.
// Address-free axis with marker up, facing or marker forward snapshots 159/213/159. Up/facing,
// up/forward, facing/forward and all three 213/159/213/213. Up snapshot and in-place projection 159.
// NOT kept: 159 reaches frame 0xb4 and 848 bytes, with different alignment products and greater score.
// Standard element-wise scale with direct or staged cross: separate axis and
// member stores 293, separate axis and setter 293, output axis and setter 298,
// output axis and complete result 323 for either cross. NOT kept: scale adds calls
// and grows the frame. The scaled axis still lacks retail's register allocation.
// Output axis with direct or staged cross and shared fallback side, up, both, or final
// setter: 117 in all eight forms, 857 bytes with frame 0xb4.
// NOT kept: each writes the intermediate axis k to the final output slot. Retail keeps
// that component in a register and writes the slot only after the rotation.
// Transform up as scaled-axis storage 212 for either cross. As fallback-cross storage 195
// for either cross. Both lifetimes in transform up 232 for either rotation cross. Transform
// left or forward for fallback 195/195. NOT kept: the 0xb4 forms expand the body and score
// higher. Fallback-only forms retain 0xc0.
// The normalized axis still requires a separate home in the opening body.
// Scaled axis scalars from planar cross products 215, planar cross products produced before the root
// 215, scaled vector initializer from planar cross products 218, shared scale on a planar cross
// initializer 208, planar cross products through the axis setter and shared scale 208, scaled planar
// cross products through the output workspace 184, scaled axis scalars from planar products with a
// separate alignment dot 210, shared scale on a planar cross initializer with a separate alignment dot
// 203.
// NOT kept: The planar cross producers lengthen the body and increase the score. The output workspace
// recovers the 0xb4 frame with a higher score.
// Root as all three cross weights with ordinary/staged axis 170/170, or with the output
// workspace and setter 170/170. Separate ordinary/staged rotation cross scaled first or at
// use 257/218 for each helper. NOT kept: both output workspace forms attain 0xb4 and 848
// bytes but increase the score. The scaled-axis registers and alignment order still differ.
// M3 lane GB round 5 (32 trials, fast with 0x10c850/0x10cec0/0x10d4e0/0x10d5f0 all MATCH; control 130, fr=0xc0, 859 B, al=94, mask=37):
// single-write address-free axis (or address-free quaternionf, dot3f, separate raw->scaled scalars, 2.0f*left, cross-last, scale-last, or inline 1/root)
// recovers exact fr=0xb4, 848/848 B, +0x2b and 0 diffs on all 32 prologue/default and 65 else/epilogue insns, but scores 159 raw (80 vs 82 if-branch insns
// shifts else/epilogue by 2 lines); naming `real align = ...` before `square = (align + 1.0f) * 2.0f` recovers retail's exact j*j+i*i+k*k dot order at
// fr=0xb4 (831 B, 173; with j,i,k scaled scalars 848 B, 170); in-place facing-first cross + j,i,k scale 129 (fr=0xc0, -1 below bar); in-place raw->scale /
// cross-before-sqrt / two-struct raw+axis 130 (fr=0xc0); sequential `align +=` 168-172 (fr=0xb8/0xc0); cross into up 200 (fr=0xbc). Body unchanged.
// Address-free axis with a seeded projection, named projection products, scalar row
// results or grouped weighted cross terms, 159 each. With a named alignment dot,
// 173 each. NOT kept: both groups recover 0xb4 but raise the score. The named
// alignment also shortens the function to 831 bytes. Rotation scheduling still differs.
// Cross in output with normalized scalars 206; reciprocal before cross 207; scaled
// j/i/k order 206; staged cross 206; depth-seeded projection 206; complete alignment
// local 193; shared doubled scalar after projection 206; independent cross with
// normalized scalars 206. NOT kept: all retain frame 0xc0 and grow the rotation.
// The 0xc0 frame and the scaled axis spill still differ.
// Scalar SSE alignment/projection/both, rotation weights alone or with
// alignment/projection/both, and all with SSE reciprocal: 249/250/290/234/275/278/314/302.
// NOT kept: the intrinsic forms align the frame to sixteen bytes,
// lengthen the body and change matched calls in the reduced image.
// The scaled axis still spills on the 0xc0 frame.
// Component-array reads for scaled axis, facing, marker up, marker forward,
// paired alignment, paired projection or all rotation operands 130 each.
// All component arrays with a separate alignment sum 178.
// The scaled axis still spills and the frame remains 0xc0. Half-angle square
// or full diagonal before the cross 141/189, or between cross and scale 184/190.
// With a shared cross weight 182/188/128, early diagonal and reciprocal 218.
// NOT kept: the 128 form gains two and leaves the extra axis frame space.
// @retail 0x10d710
void function_10d710(vector3f const *facing, long object_index, point3f const *origin, point3f *position)
{
    s_item *item = ITEM_GET(object_index);
    if (item->parent_index == NONE && !(item->flags_12c & 1))
    {
        s_object_marker marker;
        if (function_b8d30(object_index, 0xc0000c1, &marker, 1, false))
        {
            if (!origin) origin = &marker.matrix.position;
            if (!position) position = &marker.matrix.position;
            vector3f up;
            vector3f const *old = &marker.matrix.up;
            vector3f const *forward = &marker.matrix.forward;
            real square = (facing->j * old->j + facing->i * old->i + facing->k * old->k + 1.0f) * 2.0f;
            if (square > 0.0001f)
            {
                real root = (real)sqrt(square);
                vector3f axis;
                cross3f(old, facing, &axis);
                item_vector_scale(&axis, 1.0f / root);
                real scalar = root * 0.5f;
                real scale = scalar * scalar * 2.0f - 1.0f;
                real projection = forward->k * axis.k + forward->j * axis.j;
                projection += forward->i * axis.i;
                real dot = projection * 2.0f;
                up.i = (forward->k * axis.j - forward->j * axis.k) * (scalar * 2.0f) + forward->i * scale + dot * axis.i;
                up.j = (axis.k * forward->i - forward->k * axis.i) * (scalar * 2.0f) + axis.j * dot + forward->j * scale;
                up.k = (forward->j * axis.i - axis.j * forward->i) * (scalar * 2.0f) + axis.k * dot + forward->k * scale;
            }
            else
            {
                vector3f side;
                side.i = facing->j * forward->k - facing->k * forward->j;
                side.j = facing->k * forward->i - facing->i * forward->k;
                side.k = facing->i * forward->j - facing->j * forward->i;
                up.i = facing->k * side.j - facing->j * side.k;
                up.j = facing->i * side.k - facing->k * side.i;
                up.k = facing->j * side.i - facing->i * side.j;
            }
            function_30bf0(&up);
            transform4x3f transform;
            function_1420f0(&transform, origin, &up, facing);
            transform.scale = *(real *)((byte *)item + 0xa0);
            function_b92d0(object_index, &marker, &transform);
            *position = item->position;
        }
    }
}


void __stdcall function_b8ee0(long parent_index, long marker_name, long object_index, long a);
// @retail 0x10cec0
void function_10cec0(long weapon_index, long unit_index, long parent_marker_name, long marker_name)
{
    (void)&parent_marker_name;
    (void)&marker_name;
    function_10cdf0(weapon_index);
    function_b8ee0(unit_index, parent_marker_name, weapon_index, marker_name);
    byte *unit = (byte *)ITEM_GET(unit_index);
    s_item_definition *definition = (s_item_definition *)g_4e3b44[*(long *)unit & 0xffff].bytes;
    if (TEST_FIELD_BIT(definition->flag5))
    {
        s_item *item = ITEM_GET(weapon_index);
        if (!(*(byte *)&item->object_flags & 1))
        {
            if (function_b9d20(weapon_index)) function_bef30(weapon_index, 1, 0, 0, false);
            item->object_flags |= 1;
            function_b8b70(weapon_index);
        }
    }
}



#include "unknown_1eb550.h"
#include "unknown_1428b0.h"
struct s_vehicle_ray;
bool __stdcall function_168f40(long, s_vehicle_ray const *, long, long);
bool function_109a00(long, vector3f *, bool, vector3f *);
bool function_181db0(long, vector3f *);
void function_b7740(long, vector3f const *, vector3f const *, bool);
void function_1c4b00(long, void *, void *, long);
transform4x3f *function_b8bd0(long, short);

static __forceinline real item_dot(vector3f const &a, vector3f const &b)
{
    return a.j*b.j + a.i*b.i + a.k*b.k;
}
static __forceinline vector3f item_cross(vector3f const &a, vector3f const &b)
{
    vector3f result = {a.j*b.k-a.k*b.j, a.k*b.i-a.i*b.k, a.i*b.j-a.j*b.i};
    return result;
}
static __forceinline void item_add_scaled(vector3f *a, vector3f const &b, real scale)
{
    a->i += b.i*scale; a->j += b.j*scale; a->k += b.k*scale;
}
static __forceinline void item_spin_vector(vector3f *vector, s_item const *item)
{
    real i = vector->i;
    real j = vector->j;
    real k = vector->k;
    real projection = (i*item->spin_axis.i + j*item->spin_axis.j + k*item->spin_axis.k) * (1.0f-item->spin_cosine);
    real cross_i = j*item->spin_axis.k - k*item->spin_axis.j;
    real cross_j = k*item->spin_axis.i - i*item->spin_axis.k;
    real cross_k = i*item->spin_axis.j - j*item->spin_axis.i;
    real direct_i = i*item->spin_cosine;
    vector->i = direct_i + projection*item->spin_axis.i - cross_i*item->spin_sine;
    real direct_j = j*item->spin_cosine;
    vector->j = direct_j + projection*item->spin_axis.j - cross_j*item->spin_sine;
    real direct_k = k*item->spin_cosine;
    vector->k = direct_k + projection*item->spin_axis.k - cross_k*item->spin_sine;
}

struct s_item_flags_view_10b850
{
    byte unknown000[0x12c];
    byte flag0 : 1;
    byte flag1 : 1;
    byte flag2 : 1;
    byte flag3 : 1;
    byte flag4 : 1;
    byte flag5 : 1;
    byte flag6 : 1;
    byte flag7 : 1;
};
// score 813 (was 824): Frame 0x154 versus 0x12c, entry Boolean byte stores and settle/bounce order still differ. GA16 (18 fast trials, control 813 at 4096/4096 B, al=807, mk=589, body unchanged): settle-first `((surface && floor && slow) || (slow && item->value_13a >= 4)) && !(collision.value_58 & 0x20)` reproduces retail's exact 0x10bb6f..0x10bc2f split bounce-header/settle CFG (alone 929, +if/else correction 913 mk=530, +single-store value_13a 919 mk=524, +cross3f & SSE ternary abs 882 fr=0x13c); rotate_direction-shape `item_spin_vector` (dot3f + unhoisted 1-cos) 841 (al=763, mk=552; direct ijk 883; by-val axis/sin/cos 821 al=752 mk=553, +cross3f_staged 823 mk=527; +settle_cond 897 al=683, +cross3f_staged 884 ins=877/877 al=644, +ifelse 912 mk=498); full clean combo (settle_cond + ifelse_corr + single_13a + spin_rot_dir + cross3f_staged + cross3f/SSE-abs + early settled/collision.unknown24 + TEST_FIELD_BIT flag4 + vector3f_add_scaled/vector3d_from_points3d) 888 (4061 B, fr=0x13c, al=567, mk=399), +word_flags_set+surface_early+bit14 877 (4073 B, mk=378; no vecray 871; no sse_flow 894 fr=0x14c; by-val spin 881 mk=364); diagnostic match-only fr=0x12c combo (1-caller 2-def flag helper + scoped ray) 814/808 (exact 4096/4096 B, +0x3e, fr=0x12c, al=469, mk=343, all 24 call sites within 0-1 insn of retail, NOT kept: blocked on 0x16a040 noinline and multi-caller flag helper). GB20 (24 fast trials, 8 peers MATCH, 0x10b4d0 140, body unchanged at 813): typed s_collision_result_1697c0 dropping byte* hit/normal/point refs 877 (fr=0x144, exact retail slow-dot registers); + cross3f/SSE ternary abs 955 (fr=0x13c, mk 430; + dot3f 955); + settle-first/if-else count/if-else correction 887 (mk 374); + word_flags_set/early settled+unknown24+surface/TEST_FIELD_BIT bit14+flag4 875 (mk 384; + vector3f_add_scaled/vector3d_from_points3d/vector3f_scale 875 mk 378); + by-val spin + cross3f_staged 843 (4096/4096 B, mk 308); + unhoisted item spin + cross3f_staged 869 (mk 362; + existing function_16a040 931 fr=0x130 al=761 mk=340); + existing include/axis_rotation.h function_x84d9e8 + cross3f_staged 879 (fr=0x13c, 870/877 insns, mk 276 vs baseline 513 — only the 5 entry dead-byte-store and 2 gravity-reload insns remain; + function_16a040 928 fr=0x130 al=653 mk=283); on 813 base: normalize3d.h vector3f_scale angular 822 (inline dot 822), cross3f orient 813, s_item_definition + TEST_FIELD_BIT 842, spin jki unhoist 823, spin cross3f+dot3f 1047, unhoist spin 829 (+ cross3f_staged 826), function_x84d9e8 + cross3f_staged 846, single-exit outer if 813, no-ternary single_13a 891, typed+SSE+wflags+spin+cstage without settle-first 906.
// GA10 (17 fast trials, baseline 813 at 4096/4096 B, body unchanged): byte_flags_set 871 (fr=0x148); spin no-hoist / direct-mem +
// cross3f_staged 858/880 (mk 763->718/697); item_dot i+j+k 813; vector3f_add_scaled + vector3d_from_points3d 890 (fr=0x148, exact
// [edi]/[edi+4]/[edi+8] integration/ray); SSE >=0 flow abs 907 (fr=0x13c); TEST_FIELD_BIT flag14/flag4 + tail cross3f_staged 841;
// combined clean forms 857-921 (fr=0x13c, al 927->672, mk 763->487, exact flow/0x168f40..0xb8d30 sequence); entry local bools 882
// (still fold to shr/test); scoped ray 920/931 (fr=0x130, 4 B from 0x12c due to carrier spilling without the entry flag byte slot
// and 0x16a040 staying noinline).
// GB8 (36 fast trials, plain-confirmed 813 at 4096/4096 B, al 929->807, mk 622->589; 17 peers + 0x109a00 + 0x54590 MATCH):
// KEPT semantic/retail fixes: swapped `function_109a00` args fixed to `(object_index, &carrier_delta, true, &local_c01284_3)`
// (matches retail 0x10b901 and matched 0x109a00; alone 824), `switch (collision.type)` + `TEST_FIELD_BIT` bit-6 for `surface`
// (alone 878), and `switch (collision.type)` (cases 1/3, default) + `short node = *(short *)(hit + 0x46)` for settle (closes raw-dword node diff;
// alone 886; together 813). Not kept (match-only or worse): early coll/settled 824/822; `TEST_FIELD_BIT(item->flag4)` 822;
// plain carrier add 884; `surface = false` before 0x10c880 856; bounce-first 881/889; `flags_12c` byte ops 856; `0xcd0c0` local
// entry bools 904; `item_cross_product` + ternary abs + scoped ray 929 (fr=0x130) and with two-def `item_flag_bit_test` +
// `surface` before 0x10c880 + ternary settle 783 (4100 B, fr=0x12c, fd=+0x3e, match-only one-caller helper/hit local), bounce-first
// + `tag_definition` + `flag4` 853 (4043 B, fr=0x12c, fd=+0x3e, mk=496).
// GB11 (30 fast trials, control 813 at 4096 B, al=807, mk=589; all restored, body unchanged): existing `word_flags_set` 859
// (4096 B, fr=0x148, mk=572: exact unmerged `and/or byte ptr` settle flags at 0x10bd41/0x10bd78); `byte_flags_set` 871;
// `TEST_FIELD_BIT` at flag14+flag4 842 (mk=554), flag14 alone 846; single-store `value_13a` ternary 893 (mk=565); `if/else`
// `correction` 867; both 867; + `word_flags_set` + `TEST_FIELD_BIT` 898 (al=723, mk=545); `cross3f_staged` 818 (mk=555;
// tail-only 818, mk=552; + flag14 843); direct `item_spin_vector` 876; combined 857 (al=643, mk=529); SSE flow (`cross3f` +
// ternary abs + `vector3f_add_scaled`) 912 (fr=0x13c, mk=525), + early `settled`/`collision.unknown24` 914 (al=635, mk=477);
// bounce-first (`!((surface && floor && slow) || (slow && value_13a >= 4)) || (value_58 & 0x20)`) + single-store `value_13a`
// + `if/else` `correction` 902 (mk=505; `!((...) && slow && ...)` 890; ternary cond 818; without `if/else` 880); + `word_flags_set`
// + `TEST_FIELD_BIT` 913 / + `cross3f_staged` 903; full SSE (`item_cross_product` or `dot3f`) 877 each (4075 B, fr=0x13c, al=612,
// mk=408: 8 calls 0x168f40..0xb8d30 locked at constant -12 delta; + scoped ray diag 878, fr=0x130, mk=402); + `surface = false`
// before 0x10c880 858 (4096 B, fr=0x13c, 873/877 insns, mk=405; + direct spin 882, al=589, mk=393; alone 856, + cstage/early 860,
// + cstage/wflags 858, ternary bf 912). Positional score stays above 813 because entry flag-byte stores and 0x16a040 (noinline)
// shift all offsets by 0x10..0x28.
// Earlier (824, was 813): Frame 0x154 versus 0x12c, entry Boolean bytes and the carrier, collision and rotation
// stack homes still differ.
// MiniMax B (11-18, round 2): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, so
// forty bytes of frame are unaccounted for and every stack home sits further out than retail's; the entry flags are
// still held in registers where retail keeps them in stack bytes, and the forms the earlier rounds tried all grow
// past retail's 4096 bytes or land below the eight-point gate, while the permuter over the body (one job, --fast,
// 300 s) tried sixteen forms and its best is the 824 baseline, so the body stands.
// MiniMax B (11-18): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c (forty spare
// bytes below the homes), so every stack home sits further out; retail keeps the two entry flags in stack slots where
// this build holds them in registers, and the item walk still differs, so no form of the body's locals changes it.
// MiniMax H (11-18): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, so every
// stack home sits 0x28 further out; the two entry flags are still tested in registers where retail keeps them in stack
// bytes, and the item's own flag byte is still written with a load, a mask and a store where retail masks the byte in
// place (the byte's own view of the field, the bit-field view and a byte pointer all give the same bytes here). The
// collision result's home is at esp+0xdc here and at esp+0xb4 in retail, so the thirty-six bytes of difference sit
// below it in the locals of the main block; copying the normal and the point out of the result instead of taking a
// reference into it grows the frame to 0x168 and scores 895.
// MiniMax H (11-11): no new form, no lower score (824); the frame is 0x154 against retail's 0x12c from the first instruction, so every stack home sits 0x28 further out, and retail still keeps the two entry flags in stack slots where ours holds them in registers.
// MiniMax D (11-11): no new form, no lower score (824); the frame is still 0x154 against retail's 0x12c from the
// first instruction, so every stack home sits 0x28 further out, and retail still extracts the flag bits in place
// through a byte field view and folds the 0x20000000 bit into the field in memory where ours copies the word
// out first. The open behaviour differences stand as they were: the node handed to 0xb8bd0 as a raw dword and the
// type chain's compare at 0x10bd1c.
// MiniMax S (11-11): no new form, no lower score (824): the frame is 0x154 against retail's 0x12c, the two entry
// bit tests stay in registers here while retail keeps the bytes in stack slots and re-reads the flags word, and
// the definition pointer sits at 0x38 against retail's 0x1c.
// MiniMax A (11-10): no new form, no lower score (824): our frame is 0x154 against retail's 0x12c
// from the first instruction, so every stack home sits 0x28 further out and the two open behaviour
// differences - the node handed to 0xb8bd0 as a raw dword and the type chain's compare - stay.
// MiniMax E (11-11): no new form, no lower score (824); our frame is 0x154 bytes where retail's is 0x12c, so every
// stack slot sits 0x28 higher, and the entry flag test reads the definition's byte in the other order.
// MiniMax H (11-12): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, and the entry Boolean and the carrier, collision and rotation homes still differ.
// MiniMax S (11-14): no new form, no lower score (824): the frame here is 0x154 against retail's 0x12c, but the
// marker's address and the block copy's source sit at the same distance from the entry stack pointer in both, so
// the forty spare bytes lie below the homes and the rest of the gap is in the block bodies.
// MiniMax I (11-15): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, so every stack home
// sits 0x28 higher; the marker's address and the block copy's source are at the same distance from the entry stack pointer
// in both, so the forty spare bytes lie below the homes and the rest of the gap is in the block bodies, which this round
// did not move.
// MiniMax E (11-16): no new form, no lower score (824): the frame is 0x154 against retail's 0x12c, retail keeps the two entry flags in stack bytes where this build holds them in registers, and the item walk still differs.
// Earlier score 824 (was 813): statement-by-statement walk against retail (s1B, 11-06). Behaviour fixes, all kept even though
// the score rose 11: (1) the byte at [esp+0x12] is ONE flag: the definition's +0xbc bit 2 (skips gravity), set to true
// before the ray query (0x10ba18) and cleared only by the settle branch (0x10bcf8); the draft's separate
// `update_position = fixed` skipped the position update after a bounce when the bit was clear (retail 0x10c136
// tests the flag, bounce never touches it); (2) the 0.05 nudge before function_10c880 is added to the predicted
// `position`, not to the hit point (0x10bac7 adds [position], only the later bounce nudges at 0x10c0ca use the
// point); (3) the flow side vector is normal x flow (0x10beb0..0x10bef8), not flow x normal. Not kept: along_side and
// along_flow are i,j,k dots in retail (0x10bf06..0x10bf22, 0x10bf96..0x10bfc2; the other three are j,i,k, as item_dot); dot3f
// from vector_products.h gives that order and the same 824, but in the full build it costs matched 0x54590
// (src/network_voice.cpp, 0 -> 2) through LTCG, so both stay item_dot (a rounding-order difference only).
// Open behaviour differences: retail passes the node as a raw dword at hit+0x46 (push [esp+0x9e], 0x10bd2b) and
// stores its low byte, ours zero-extends one byte (callee 0xb8bd0 only reads the word, `long` forms 887/902);
// the type chain at 0x10bd1c compares 4 and falls into the same code either way (switch with case 4/default merges
// it away, 837). Frame 0x154 versus 0x12c; entry Boolean publications ([esp+8], [esp+0x10]) and slot homes remain.
// Frame forms (all behaviour-neutral): ray in an inner block 846 (frame 0x148); inner ray + scalar side vector 949
// (0x130); + `x < 0 ? -x : x` instead of fabs 940 (frame 0x12c, 4013 bytes, slots all differ); scalar side alone
// 925; cross3f into a vector 931; ternary abs alone 860; merged value_13a store 866; declaration reorders of
// carrier_delta/collision with the 0x12c form 940 each.
// MiniMax S (11-10): no new form, no lower score (824); the frame is 0x154 against retail's 0x12c, the two entry bit tests stay in registers here while retail keeps the bytes in stack slots and re-reads the flags word, and the definition pointer sits at 0x38 against retail's 0x1c. The 815 of the earlier round is the two-flag body (a separate flag for the definition bit and another for the position update); retail keeps one flag, set before the ray query and cleared only by the settle branch, so that body is not restored.
// Earlier: Frame 0x154 versus 0x12c; entry Boolean publications and carrier/collision/flow/marker/rotation homes differ.

// MiniMax I (11-19): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, so every stack home sits 0x28 further out, and retail still extracts the two flag bits into a byte in the frame (mov byte ptr [esp+8], al) where this build tests them in place.
// MiniMax I (11-16): no new form, no lower score (824): retail extracts the two flag bits into a byte in the frame (mov byte ptr
// [esp+8], al) where this build tests them in place, and this build's frame is forty bytes larger (0x154
// against 0x12c); a named bool for the bits measures the same 824.
// MiniMax E (11-15): no new form, no lower score (824): the frame is still 0x154 bytes against retail's 0x12c, so every stack home sits 0x28 further out, and retail keeps the two entry flags in stack slots where this build holds them in registers.
// MiniMax G (11-13): no new form, no lower score (824): the frame is 0x154 against retail's 0x12c,
// the entry's flag byte differs (retail shifts the value and stores the parity byte where this
// build tests a bit of a shifted register), and the walk still differs in the item table's own
// expressions.
// MiniMax B (11-12): no new form, no lower score (824): the frame is 0x154 against retail's 0x12c and the entry's flag
// byte differs - retail shifts the value and stores the parity byte where this build tests a bit of a shifted register;
// the permuter on this address was killed by its time limit without a result.
// Earlier: Frame 0x154 versus 0x12c, entry Boolean bytes and collision/carrier/rotation homes still differ.
// Earlier: Frame 0x154 versus 0x12c and Boolean/vector homes remain.
// Frame 0x154 versus 0x12c and Boolean/vector homes remain.
// frame 0x154 versus 0x12c; direction workspace 861;
// frame 0x154 versus 0x12c; direction workspace 861;
// bounce-first 894; shared support predicate 838; direct rotation first 819;
// correction assigned by branch 861. Original body retained.
// j91: frame 0x154 versus 0x12c and Boolean homes remain; native rotation copies reach 0x148 but regress.
// collision result predicate 819; settle result predicate 888; collision settle predicate 879; native rotation
// copies 885; shared native rotation 886
// Frame pointer 1103; inline depth one 819; trigonometric snapshots 891;
// flow output in dead motion workspace 818 (frame 0x148); cross scalars first 820.
// Normal value copy 882 (frame 0x15c); point value copy 852; marker
// matrix memcpy 819; settled result long 844; position decision long 887.
// Original body and helper operands kept.
// Frame 0x154 versus 0x12c and Boolean/vector homes remain; native contact fields 907; plus float absolute 880, shared addition loop 900, by-value cross inputs 928, named turn weight 907; contact pointers/float absolute 873 (fast).
// Direct byte saturation 878; fixed before vector copies 858; matrix assignment 858;
// flow-side assignment 858; support test before slow 821; reversed rotation input copies 858;
// ordinary cross inline 970; float absolute alone 873; support test plus float absolute 837.
// Round 349: Weighted cross terms 821; per-row cosine products 813 (plain); weighted terms before projection 822; both 813; later angular snapshot 819 (fast).
// Complete rotation-axis snapshot 953; named one-minus-cosine term 821; per-row direct sums 824; axis snapshot
// plus scale term 951; signed-byte floor and slow results 821. Frame 0x154 versus 0x12c and original
// Boolean/vector homes remain; all private helpers restored.
// Rotation helper explicit axis 853; explicit trigonometric inputs 893; all rotation inputs 892;
// native reference output 821; output return 821; ordinary rotation inline 1926.
// Frame/Boolean/native workspace homes remain; original body and every helper restored.
// dot products 880; scaled-add products 813.
// rotation projections 839; dot/add products 858.
// dot/projection products 864; add/projection products 839.
// all three product families 856; cross input products 834.
// Round 126: Native dot/scaled-add pointer inputs 813/813; complete rotation outputs 813 at retail length;
// component cross-result fill 813; ordinary dot inline 813; reused geometry predicate 858; reused contact
// kind 878; all velocity copies by memcpy 813 (fast); all restored.
// First-computation cosine 845; first-output sine 844; both 828; actual axis-field
// pointer 849; axis with cosine/sine/both 823/889/870; entry trigonometric declarations
// with unchanged late assignments 828 at retail length. Original helper and body restored.
// Round 134: separate vector input pointer/reference/value with pointer/reference output
// 816/816/816/816/936/936; actual vector return with pointer/reference input
// 939/939 (exact only).
// Complemented motion/floor/speed/surface 922/813/811/890; floor-speed/surface-floor/surface-speed/all 811/890/888/879 (fast). Two-count speed gain rejected; frame 0x154/0x12c and native workspace homes remain.
// Rotation publication ikj/jik/jki/kij/kji 814/819/814/807/815;
// scaled-add jki/kij/kji 859/818/860. Kij rotation with scaled-add
// ikj/jik/jki/kij/kji 811/811/858/812/859. Six-point 807 rejected;
// original complete body and helpers restored; frame and Boolean homes remain.
// Round 174: dot first/second/both native values 1039/1012/1200; scaled-add input value 942;
// dot first/second/both plus scaled-add value 1117/1200/1302; native add output reference with value input 942.
// Original helpers and body restored; frame 0x154/0x12c and Boolean/workspace homes remain.
// Rotation cross-term declaration orders ikj/jik/jki/kij/kji 814/813/813/814/814;
// cross-first orders jki/kij/kji 814/815/815. Frame 0x154/0x12c and Boolean/native
// rotation homes remain. Original complete body and all helpers restored.
// Record-returning cross pointer/reference inputs 813/813/813; first/second native
// values 894/863, with opposite pointer 894/863; both native values 927. Original
// result publication, helpers and complete body restored.
// Round 219: Independent motion-copy/gravity-update/floor-speed observation order masks 1-7; all
// with projection role names; existing direct-output side cross helper:
// 812/813/812/863/862/863/862/862/921. Original body restored.
// Round 373: Reused scalar axis components in ijk order, with sine/cosine/both
// snapshots 850/894/879/909; kji axis input order with the same four forms
// 864/845/879/909. Frame remains 0x154; all original helpers and body restored.
// Round 159: Native attachment fields 813; step field at each use and with attachment fields 893/893;
// named bounce coefficient and with attachment fields 887/887; step/coefficient and all three 893/893;
// direct bound selection 813. Native collision location/object-node/material/all layouts 887/863/859/894.
// All restored. Coefficient and collision layouts reach frame 0x148 without an eligible gain.
// Round 281: existing record-returning cross for all four orientation outputs 1084
// (4298 bytes, frame 0x148). Complete cosine products before projection, all six
// computation orders 825 each; kji before crosses/stores 824/824 (4103 bytes).
// All original helpers and body restored; frame and native Boolean/vector homes remain.
// Round 378: Existing shared scaled-add at all four sites, with carrier/settled/position
// polarity masks 0-7: 813/896/814/896/874/899/874/899. Original accumulation,
// result carriers, full body and helpers restored; frame 0x154/0x12c remains.
// Round 161: Native item flag8/flag29/state0/flag8-29/flag8-state/flag29-state/all-three 813 each;
// late flag14 view 846. Original actual item declaration and whole body restored.
// Round 176: Library input captures in rotation/dot/both/cross/rotation-cross/dot-cross/all
// 931/1229/1381/1400/1517/1736/1829; all with library matrix copy 1829.
// Original producers, scalar input captures, in-place behavior and flow operands restored.
// Round 33: Rotation source-component, turn-term and projection scalar role-name subsets,
// then complete cosine-product roles: 813/813/813/813/813/813/813/813. Original body and all helpers restored.
// Round 388: Native item receiver reference, with pointer/reference vector outputs and
// both complete formal orders, 813 each; reversed original pointer contract 813;
// reversed private dot formals on the latter and two new native receiver contracts 813 each.
// Original rotation helper, all four callers and every dot tuple restored; frame remains 0x154.
// Round 285: existing staged orientation cross at first/second/third/fourth/first-two/last-two/
// all sites, then all with staged flow cross: 821/821/821/818/813/820/820/935 fast.
// Original complete body and helpers restored; frame and Boolean/vector homes remain.
// Round 223: complete persistent decisions in signed-byte/byte/short/word storage 813/813/872/872;
// complete contact decisions in those domains 813/813/897/897. Original complete body restored.
// Round 237: Lexical persistent/contact/motion families 1/2/3/4/5/6/7, then all with earlier constant settled declaration: 813/813/813/813/813/813/813/813.
// Original executable body restored.
// Round 252: Native accumulated-vector return with pointer/reference/value accumulator and additive inputs
// 964/964/1106/964/964/1106/1092/1092. Original complete executable source restored; Frame 0x154 versus
// 0x12c and Boolean/native workspace homes remain.
// Round 266: library counter/correction/lift bounds, seven nonempty subsets and
// all with bounded unsigned count, 871/897/912/876/895/923/939/939. Correction/lift recover frame
// 0x148 but regress. Original bounds, flow operands, helpers and body restored.
// Round 276: Complete carrier-input/linear/angular/carrier-linear/carrier-angular/linear-angular/all
// scalar snapshot transfers 863/879/884/896/863/886/916; all with scalar ray construction 908. Initial
// forms recover frame 0x148 but regress.
// Original executable source restored; Original frame 0x154 versus 0x12c, Boolean publications and carrier/collision/rotation homes remain.
// Round 288: Paired contact-position setters with predicted-position/contact-copy/ray helper masks
// 0..7 894/887/883/883/821/886/876/882. Native point boundaries recover frame 0x148 in initial forms
// but regress; complete original constructions, copies, helpers, rotation and flow operands restored.
// DL224: Isolated staged flow cross 927; with complete rotation publication kij 934; with pre-
// projection cosine-product orders ijk/ikj/jik/jki/kij/kji 924 each. All original flow operands,
// private helpers and complete body restored; frame, entry Boolean publications and
// carrier/collision/rotation homes remain.
// Round 461: Overlap-safe carrier/linear/angular input transfers, subsets 1-7, then all
// with the marker-matrix transfer: 892/873/859/837/916/861/907/920.
// Original native copies, input observations, helper graph and complete body restored.
// Round 310: Explicit native active/attached/anchored gate producers in all Boolean subsets,
// then all byte carriers, 887 each (4068 bytes; frame 0x148). None matches; all one-use
// observations restored. Original frame 0x154/0x12c, entry publications and motion/rotation
// homes remain.
// Round 296: Standard carrier/linear/angular input-copy subsets 1..7 872/900/880/811/876/871/879;
// all with standard matrix transfer 896. Angular copy with rotation publication kij/kji 806/814;
// kij with angular-before-linear capture 805, angular role name 806. The 805 form reaches 4096
// bytes but plain loses 0x54590 by two reloads; nearby standard include and backward angular copy
// remain 805 with the same loss. Original body, rotation producer and includes restored.
// Round 309: Plain component loops at complete integration/ray, paired contact displacement and settled
// angular projection producers, subsets 1..7, then all unsigned counters: 862/871/970/856/866/833/929/929. Some loops
// reach frame 0x148 but regress. Original output producers and body restored; frame, entry Boolean
// and native workspace homes remain.
// Round 289: Narrow initial query-ray lifetime with early collision constant, narrow flow-side
// scope and complete rotation kij publication, eight subsets: 887/886/887/886/887/886/887/886.
// Frame falls to 0x13c and extent 4068 but scores regress; exact-only query result, original scopes,
// rotation helper and complete executable body restored.
// Round 312: Actual private producer output pointers consumed at basis-cross, cross-normalizer
// and carrier-add motion boundaries, subsets 1-7, then all with library matrix transfer:
// 813 each. Matched cross-helper sibling preserved throughout; all helper interfaces,
// consumers and entire body restored. Frame 0x154/0x12c and Boolean/native homes remain.
// N315: Complete axial-dot producer, direct/successive accumulation, original/
// early plain-cross phase and complementary-cosine node, masks 0..7:
// 813/823/814/823/813/823/814/823. Original helper/body restored; frame and native homes remain.
// N317: Successive destination rotation rows, two/three stores, immediate/early
// complete cosine products and compound/explicit updates: 877/914/902/915/
// 877/914/902/915. Frame stays 0x154; original helper and whole body restored;
// entry Boolean publications and native collision/carrier/rotation homes remain.
// N319: Retained cosine locals plus complete per-row sums, sequential/staged sums and cosine/sine products: 832/840/841/840/815/837/826/837. Original helper and full body restored; frame 0x154/0x12c and Boolean/vector homes remain.
// Round 541: Native flow-cross result through the shared scalar setter, first/second reused scalar
// inputs and ordinary inline boundary, masks 0..7: 813/821/821/887/969/969/969/932. Both
// scalar inputs recover frame 0x148 but regress; original helper, operands and body restored.
// Y551: Complete first-use contact-flag value in byte/char/word/long storage
// 887/887/873/872; with native contact-kind ownership 881/881/836/835. Frame
// falls to 0x148 but scores regress; all snapshots, original field reads, helpers
// and body restored. Entry Boolean and collision/carrier/rotation homes remain.
// DH321: Existing shared rotation dot/staged-cross/cosine-scale families, masks 1..7,
// then all with ordinary helper inline: 858/884/876/876/914/927/884/1926. NOT kept: mixed-read/native-
// vector storage forms lack equality; original helper, inputs, includes and full body restored.
// B392: Native rotation input/cross with plain output loops, direct/staged cross, immediate/separate
// cosine loops and long/unsigned ordinals: 1000/1024/1009/1041/1000/1024/1009/1041. Original complete
// helper and body restored; frame and Boolean/vector homes remain.
// B401: Independent fixed predicate with unified post-query position/settle outcomes,
// byte/word/long/dword bit results then three-state results: 859/880/880/880/882/905/902/902.
// NOT kept: unmatched conditional results enlarge length/frame; original full body and helpers restored.
// B403: Complete accumulator base/additive/both scalar inputs 820/813/816;
// base/additive/both with complete scaled products 816/849/813; reversed paired
// input priority without/with products 816/813. NOT kept: all remain unmatched;
// original accumulator and full body restored. Frame and entry Boolean homes remain.
// P307: Complete in-place cross differences, with component cosine ownership, cross sine
// ownership and full projection product: 832/834/832/834/811/813/811/813. The 811 forms
// reach 4096 bytes; NOT kept: two-count gain. Original helpers and complete body restored.
// M345: Complete free/fixed/settled motion state in char/signed-char/byte/short/word/int/long/dword:
// 853/853/853/909/909/905/905/905. Byte domains recover frame 0x150 but grow to 4113 bytes.
// NOT kept: scalar-state carriers miss; original separate flags, every consumer and full body restored.
// DH335: Axial-row scalar homes 839/860/864/878/839/860/864/878; first 839
// is prior-form calibration. Complete axial-vector component loop 1000.
// NOT kept: frame/length regress; original helper, inputs and full body restored.
// N352: Actual item/typed-tag/external-hit-header bindings, families 1..7 and
// complete read-only views: 813 each. NOT kept: one-use/combined record forms
// remain unmatched; original body/helpers restored, frame stays 0x154/0x12c.
// N356: Isolated late spin admission, bool/byte/short/long with positive/complementary truth:
// 880/880/880/880/863/863/863/863. NOT kept: unmatched storage/result-lifetime forms.
// Original executable source restored; Frame 0x154/0x12c, entry Boolean and carrier/collision/rotation homes remain.
// N358: One actual gate-test result reused across active/anchored/attached admissions, eight native scalar domains:
// 887/887/887/887/887/887/887/887. NOT kept: unmatched source/representation forms; original executable body restored.
// Frame 0x154/0x12c and entry Boolean/carrier/collision/rotation homes remain.
// N360: Planar subtotal dot producers, initialized/accumulating, with unchanged/initialized/accumulating/immutable axial subtotals:
// 875/893/861/884/858/880/861/884. NOT kept: unmatched source/result-storage forms; original executable source restored.
// Frame 0x154/0x12c, entry Boolean and carrier/collision/rotation homes remain.
// N362: Existing carrier/collision/material-flow/motion-publication inline call visibility,
// four individuals, three carrier pairs and all four: 813/813/813/813/813/813/813/813. Original
// declarations, helpers and complete body restored; frame and Boolean/carrier/collision/rotation homes remain.
// N366: Actual integration/query-ray/paired-contact adjacent destination producers and complete settled
// angular projection regress. Individual output producers reach retail length at 820; frame and entry
// Boolean/collision/carrier/rotation homes remain. Original body and helpers restored. Scores
// 820/820/858/820/858/858/855/906. NOT kept: unmatched result-ownership forms.
// P346: Actual flow-query available/missing bool/byte/short/long results:
// 887/887/887/887/887/887/887/887, 4068 bytes and frame 0x148. NOT kept:
// comparison regresses and length moves away from retail; original body and all helpers restored.
// N375: Actual collision impact-prefix member with direct/pointer/reference ownership
// and consistent point/normal fields: 911/887/887/887/887/904/858/858. NOT kept:
// all regress; original record, field observations, helpers and complete body restored.
// N377: Complete scaled-additive aggregate/shared-scaler/standard-range/copied-range
// producers, each with existing or standard vector sum: 864/924/865/924/1124/997/1214/1087.
// NOT kept: all regress/lengthen; every frame stays 0x154 versus native 0x12c.
// Original accumulator, helpers, observations and complete executable body restored.
// N380: Shared scaled-difference ray/angular scale/paired contact scaled-add masks
// 1..7, then all with predicted-position scaled-add: 856/813/856/813/856/813/856/871.
// NOT kept: no native-shape gain; original helpers, observations and body restored.
// Frame 0x154/0x12c and entry Boolean/carrier/collision/rotation homes remain.
// N382: Complete collision-kind switch with six geometry/object/default group priorities,
// then two reversed geometry-label priorities: 863/863/863/863/863/863/863/863. NOT kept: all regress and lengthen;
// original classification, observations, helpers and full body restored. Frame 0x154/0x12c
// and entry Boolean/carrier/collision/rotation homes remain.
// P460: Corrected single gravity/query/settle update carrier char/signed-char/byte/short/word/int/long/dword: 824/824/824/843/843/858/858/858. NOT kept: native frame and entry-byte homes remain; original behavior fixes and whole body restored.
// Earlier: the frame is 0x154 against retail's 0x12c, the entry's flag byte differs
// (retail shifts the value and stores the parity byte where this build tests a bit of a shifted
// register), and the walk still differs in the item table's own expressions.
// P482: Standard copies at settled velocity/published spin-axis/paired contact-position
// outputs, subsets 1..7, then all with fallback position: 824/862/862/824/824/862/862/862.
// NOT kept: ties/regressions; spin-axis copies grow to 4137 bytes. Original semantic fixes,
// every record assignment, input observation, include, helper and complete body restored.
// Frame 0x154/0x12c and entry Boolean/carrier/contact/rotation homes remain.
// P486: Existing shared direction rotation at matrix forward/up and item forward/up,
// individuals, matrix/item pairs, all and paired forward outputs:
// 821/838/843/821/830/823/841/824. NOT kept: three-/one-point best gains; frame remains 0x154.
// Original private helper, four call sites, declaration set and complete body restored.
// MiniMax H (11-14): no new form, no lower score (824): the frame is still 0x154 against retail's 0x12c, so every stack home sits 0x28 further out, and retail still keeps the two entry flags in stack slots where this build holds them in registers.
// T108: Ordinary inline motion/marker-query/marker-publish/inverse-point metadata: eight at 824;
// existing point-to-point query wrapper 857 (frame 0x148 versus native 0x12c), same 4096 bytes.
// NOT kept: no qualifying gain; original complete body and every provider declaration restored.
// P509: Exclusive geometry/object contact-result graphs with guarded/logical tag fallback:
// 852/897/857/878/828/877/821/876. NOT kept: best gains three at retail length;
// frame stays 0x154/0x12c. Original classifier, helpers and full body restored.
// P523: Actual collision-kind long/dword/int/unsigned-int full-width fields, flag-byte
// unsigned/signed fields and material/surface word fields: eight at 824/4096. NOT kept:
// frame remains 0x154/0x12c; original record, helpers, semantic fixes and body restored.
// P571: Actual flow-magnitude nonzero/zero decision at its consumer, bool/byte/short/long:
// 817/817/817/817/817/817/817/817. NOT kept: no byte match; the seven-point improvement is below the eight-
// point gate and grows beyond native 4096 bytes. Original magnitude test, corrected flow operands and
// complete body restored.
// P585: Existing carrier-query result on the corrected single-position-flag graph, false/true
// defaults, positive/complementary exhaustive bool, then false-default char/byte/short/long:
// 860/868/874/874/860/860/881/875. NOT kept: all grow beyond native extent; frame 0x150/0x154
// misses 0x12c and entry/carrier/contact/rotation homes remain. Original complete body restored.
// MiniMax S (11-17): no new form, no lower score (824): the frame still misses retail's size (0x150/0x154 against 0x12c) and the entry, carrier, contact and rotation homes stay in other registers; the seven forms tried earlier all grow past the native extent or land below the eight-point gate.
// Shared lateral direction 820; existing output cross 932; staged
// cross 928. With a ray scoped to its query 840/944/955.
// Shared direction with endpoint query 857; early collision and settle
// initialization 820. NOT kept: shared-motion forms miss the native
// frame or the gain threshold. Entry flag and motion workspaces still differ.
// Contacted object tag flags read through the typed definition field 824.
// position and collision result scoped through the position write 820; carrier and contact samples scoped
// through the carrier correction 820; geometry and carrier samples end at their separate last consumers 820;
// timestep scope ends immediately after position integration 824; timestep scope inside the position and
// collision scope 820; timestep scope inside the carrier and contact sample scope 820; separate timestep,
// geometry and carrier-sample scopes 820; bounded geometry workspace and flow magnitude declared at its test
// 820. NOT kept: limiting the geometry or carrier scope reaches 820 at 4096 bytes, four below baseline. All
// eight forms retain the 0x154 frame and first entry difference. The timestep scope alone ties at 824.
// Opening executable source restored.
// Ray and flow share storage, with the lateral result from a value-returning cross, direct cross, staged
// cross or existing item output helper. Component ray 881/943/944/943; point-difference ray
// 829/944/949/944; component ray and direct cross with real absolute value 923. NOT kept: the real absolute
// value recovers the 0x12c frame at 923 and 4029 bytes. The entry flags and later code still differ, and
// all forms raise the score.
// Lateral absolute value with ordered conditional 853, Lateral absolute value with unary negation 859, Named
// lateral distance from conditional 853, Lateral distance adjusted after initialization 854, Lateral distance
// selected by full branch 853, Float result of the absolute-value call 824, Float absolute-value function 849,
// Signed lateral interval 856. NOT kept: The ordered absolute value preserves 4096 bytes and cuts the frame to
// 0x150 at score 853. A float result from the original call ties 824 without changing the frame. The opening body
// is restored.
// Collision origin pointer used through integration and fallback 824, Collision origin reference used through
// integration and fallback 824, Linear-velocity field pointer shared across the carrier query 824, Linear-
// velocity field reference shared across the carrier query 824, Collision origin and linear-velocity field
// pointers 904, Collision origin and linear-velocity field references 904, Collision origin bound for the
// complete motion phase 868, Linear-velocity pointer scoped through the two carrier samples 824, Point-difference
// helper on the original collision origin 824, Point-difference helper on the shared collision origin 823. NOT
// kept: Paired fields cut the frame to 0x148 but score 904 at 4051 bytes. The ray helper with the shared origin
// saves one difference, below the retention threshold. The opening body is restored.
// Lateral response scope 824, correction step after lateral projection 845, shared reversed step 850,
// shared lateral magnitude and correction sign 878, branch-local lateral step 859, longitudinal step
// 892, or both 883, shared carrier addition 824. The 0x148 forms fall short of retail extent and raise
// the score. The motion frame remains 0x154.
// Point setter for pre-impact, bounce or both nudges 904/821/911. Bounce on the copied
// point with scalar adds, the point setter, both point setters or shared scaled addition
// 817/825/894/817. Named bounce displacement vector 870. NOT kept: the 821 and 817
// gains are below eight, and the 817 forms are three bytes past retail with frame 0x154.
// Separate lateral/longitudinal/both fluid response writebacks and existing scaled-add output:
// 905/901/894/884 through fabs, 903/902/899/890 through scalar absolute value. NOT kept: all grow
// past 4096 bytes and raise the score. The fluid response frame stays above 0x12c.
// MiniMax E (11-19): no new form, no lower score (824, baseline): the frame is still 0x154 against retail's 0x12c, so
// every stack home sits 0x28 further out, and retail still keeps the two entry flags in stack bytes (esp+8) where this
// build holds them in registers and shifts one of them right by eight.
// The frame remains 0x154 versus retail 0x12c and the entry flag bytes differ.
// Paired integration/ray, with shared origin component, with query origin, and with typed gravity flag
// 821/842/822/821. Copied origin and point difference, loop/scaled add 913/882;
// direct scaled add with ray initializer/point difference 885/877.
// NOT kept: the tied-length gains are below eight, and the helper forms change extent and regress.
// Separate local normal vector plus sum at impact/bounce/both 911/870/862.
// Shared scale plus sum at impact/bounce/both 904/855/863.
// Ray storage for both offsets 885, also with the shared scaler 885.
// Ray storage for bounce only 879, impact only 860. The local-sum forms
// reach frame 0x148, while the reused-ray forms stay above retail’s 0x12c.
// All score higher and the original position and ray expressions are restored.
// Shared integration and fluid step, before the cross or at the first response:
// single product 824/845, split multiplication 817/852. With a query-scoped ray,
// 846/862 and 815/843. NOT kept: the 815 form reaches the retail extent and frame
// 0x148, but the ray and flow still have separate slots and the scale stays in XMM4.
// The entry Boolean stores and the 0x12c frame remain unresolved.
// Motion-vector names for pre/post-query velocities, both query vectors, those
// vectors plus carrier output, and all same-size motion vectors: 824 each.
// Early collision sentinel 823, early settle result 822, both 823, and motion-entry
// collision declaration with early sentinel 823. NOT kept: gains below eight.
// The 0x154 frame and entry flag stores still differ.
// MiniMax H (11-20): no new form, no lower score (824); the frame is still 0x154 against retail's 0x12c, so every stack home sits 0x28 further out, and retail still keeps the two entry flags in stack bytes where this build holds them in registers; a permuter run over the whole body found no variant below the baseline.
// Typed item gravity bit, unscaled carrier add, both 824/824/824. Typed
// collision prefix through position write, alone or with either or both
// forms, 817/817/817/817. NOT kept: seven points at 4099 bytes, frame 0x154.
// Hit-scoped prefix with both 904. Entry flag stores and the motion frame differ.
// Shared scaled sum at lateral, longitudinal, both flow sites, contact, all contact/flow sites,
// or carrier with lateral/longitudinal/contact: 824 each. The frame stays 0x154 against 0x12c,
// and the reused entry Boolean bytes still differ.
// Working or sampled gravity step 837/837. Gravity-adjusted velocity initializer 868.
// Working, sampled or both conditional vertical values 851/858/858. Separate height 824.
// Single gravity observation 824. NOT kept: the frame remains 0x154 and the entry flag stores differ.
// Endpoint declared before the carrier 824, early or late point setter 904/904,
// copied-origin component or indexed integration 886/905, early ray setter 824,
// paired point and displacement setters 824, and component writes through the query origin 824.
// NOT kept: the tied forms retain the 0x154 frame. The point setter alone shortens the body and the copied-origin forms grow it.
// Shared forward/up spin cosine, sine, or both 852/853/848; shared axis 932;
// axis with cosine, sine, or both 872/878/878; all with shared cosine complement 861.
// NOT kept: all raise the score and retain or expand the 0x154 frame. Entry Boolean and motion homes remain.
// Settle eligibility from floor fallback, conditional fallback, slow gate, slow selection, rejected-floor
// fallback, surface arms, qualified publication, shared support and bounce rejection
// 859/862/871/871/892/886/817/875/903. NOT kept: the seven-point lead grows to 4099 bytes and retains frame
// 0x154. Entry flag bytes and low motion homes still differ.
// M3 lane 15 r135 (the coordinator's rotation-family screen, h2m fast): the four item_spin_vector calls through include/axis_rotation.h's helper (the item's axis, sine and
// cosine) 865 (4143 B); the first two only 829. Restored.
// Sign-selected lateral bound 878, shared direction test 858, nonnegative selection 901,
// direction-owned bound and step 861, direct/staged cross with bound 944/955,
// common query vector with bound 880, and with direct cross 918.
// NOT kept: the 918 form recovers frame 0x12c and first +0x3e, but shortens to
// 4034 bytes and raises the score. The entry Boolean byte stores still differ.
// One relocation decision shared with settling 885. A separate gravity test with one settled result
// before sampling, beside carrier output, beside gravity or at the query 884/884/885/885.
// A separate gravity test with one relocation result before sampling, beside carrier output or at the query 898/898/885.
// NOT kept: each reserves 0x150 and shortens to 4066 bytes. Retail uses separate relocation and settling bytes.
// The 0x154 frame still exceeds retail's 0x12c, and the entry Boolean homes differ.
// Cap within contact kind 824, capped initialization 873, conditional lift 824, lift floor in
// correction 824, shared inward speed 830, retained pre-impact projection 884, cap-result
// arms 873, named scaled lift 824. NOT kept: ties or regressions. Retail recomputes the
// normal projection after impact.
// Ray as lateral cross output with direct or staged cross and coordinate or shared point difference
// 901/902/909/914. With single-precision absolute value 879/880/885/892. NOT kept: all reduce the frame
// to 0x13c but shorten the body and raise the score. The entry Boolean stores still differ.
// Floor/slow test producers, true default on floor/slow/both, false default on both,
// explicit/complementary arms on both, true floor with complementary slow, or opposite defaults:
// 824/871/855/855/824/824/824/855/855. NOT kept: the equivalent test forms tie or regress, with the same 0x154 frame.
// Impact/bounce offsets as component stores, point setter, or shared scaled add:
// components/setter 821, components/scaled add 824; setter with components/setter/scaled add
// 904/911/904; scaled add with components/setter/scaled add 824/821/824.
// NOT kept: the bounce setter alone gains three and the impact setter shortens the body.
// Function scope for the incoming linear sample, working linear velocity and angular
// velocity, seven subsets and all with the carrier delta: eight at 824.
// The 0x154 frame, entry Boolean stores and ray/flow home still differ.
// First object bit, second object bit, both, second read and write, or both reads
// and the write as bit members: 824 each. Both reads, second read and write,
// or both reads and the write with the signed short field: 824 each.
// The entry flag bytes and 0x154 frame still differ.
// The entry Boolean stores and ray/flow home still differ. Zero magnitude on a missing
// flow query, selected by a conditional, a guarded default, or either branch order:
// 824 each with separate vectors, 881 each with shared ray and flow storage. NOT kept:
// the shared form shrinks the frame to 0x148 but shortens the body and raises the score.
// Shared predicted coordinates for aggregate stores, paired stores, displacement
// setter or both setters, then a staged step vector, step held in the ray with
// component or helper subtraction, and named steps in paired stores: 891/886/891/891/839/858/857/909.
// NOT kept: the smaller 0x148 frames fall short of 4096 bytes and raise the score.
// The entry Boolean stores and retail frame 0x12c still differ.
// The motion frame and entry Boolean byte stores still differ. Explicit real zero subtraction for slow speed,
// impact threshold, lift, both flow steps, lateral magnitude, complete flow, shared ray with complete flow,
// and shared ray with all subtractions: 855/842/813/813/878/878/891/871. NOT kept: the overlap forms shorten the frame to 0x144
// but expand the body and raise the score.
// GA22 (29 fast trials, control 813 at 4096/4096 B, al=773, mk=513, 15 peers MATCH, 0x10b4d0 140, body unchanged):
// on 813 base: default: case 4: settle 813 (with word_flags_set 859 fr=0x148), bit_vector_test entry/gravity/bit14 860,
// c_type_47f957 default ctor on s_collision_result_1697c0::unknown24 with early collision/settled 813 (ins 868, mk 496; + default: case 4: 813),
// vector3f_add_scaled/vector3f_add 813, typed s_item fields + s_item_definition 813, swapped gravity with physics_gravity_get() 865 (fr=0x148).
// On clean retail stack (c_type_47f957 + typed collision + settle-first + word_flags_set + cross3f/SSE abs + vector3f_add_scaled/vector3d_from_points3d +
// function_x84d9e8 + cross3f_staged): base 871 (4075 B, fr=0x138, al=567, mk=380), + bit_vector_test 880 (4074 B, fr=0x138, al=530, mk=222: locks
// collision [esp+0x58], flow [esp+0xb4], angular [esp+0x4c], velocity [esp+0x14], position [esp+0x28], matrix [esp+0x58]), + ternary value_13a 861
// (4097 B, fr=0x140, mk=273; if/else 866 fr=0x138 al=515; item_spin_vector 860 at 4096 B; no settle-first 909; typed s_item fields 861; case 4: only 868;
// s_item union bitfields 862; outer-cast / <=254 ternary 883); + scoped ray block: on base 876 (4073 B, +0x3e, fr=0x12c, al=514, mk=374, no flag helper),
// on bit_vector_test + ternary value_13a 861 (exact 4096/4096 B, fr=0x134, ins=876/877, al=461, mk=210, ray/flow shared at [esp+0xb8]; renaming ray/flow 861;
// item_spin_vector 860; outer-cast / <=254 ternary 885; + case 4: only 876; no ctor 894 fr=0x12c +0x3e; entry local bools diag 891 fr=0x12c +0x3e);
// calling noinline function_16a040 on clean stack 938 (4013 B, fr=0x12c, +0x3e). Still blocked on entry dead-byte stores (carrier slot reuse) and noinline 0x16a040.
// @retail 0x10b850
void __stdcall function_10b850(long object_index)
{
    s_item *item = ITEM_GET(object_index);
    byte *definition = g_4e3b44[item->definition_index & 0xffff].bytes;
    if (!(bool)((item->object_flags >> 8) & 1)) return;
    if (!(bool)((item->object_flags >> 29) & 1) && object_index != NONE && *(short *)((byte *)item + 0x112) != NONE)
        item->object_flags |= 0x20000000;
    function_10b360(object_index);
    if (!(item->flags_c1 & 1))
    {
        real dt = g_510c54->rate;
        vector3f local_c01284_3 = item->linear_velocity;
        vector3f carrier_delta;
        bool carrier = function_109a00(object_index, &carrier_delta, true, &local_c01284_3);
        vector3f velocity = item->linear_velocity;
        vector3f angular = item->angular_velocity;
        bool update_position = ((*(dword *)(definition + 0xbc) >> 2) & 1) != 0;
        bool settled = false;
        if (!update_position)
        {
            velocity.k -= g_51e9c4->unknown0 * dt;
            local_c01284_3.k -= g_51e9c4->unknown0 * dt;
        }
        point3f position = {velocity.i*dt + item->position.x, velocity.j*dt + item->position.y, velocity.k*dt + item->position.z};
        vector3f ray = {position.x-item->position.x, position.y-item->position.y, position.z-item->position.z};
        s_collision_result_1697c0 collision;
        collision.unknown24 = NONE;
        update_position = true;
        if (function_1697c0(0x24909c0d, &item->position, &ray, item->ignore_object_index, NONE, &collision))
        {
            byte *hit = (byte *)&collision;
            vector3f const &normal = *(vector3f *)(hit + 0x28);
            point3f const &point = *(point3f *)(hit + 8);
            bool floor = normal.k > 0.707099974155426f;
            bool slow = 1.5f > -item_dot(local_c01284_3, normal);
            position.x += normal.i*0.05f;
            position.y += normal.j*0.05f;
            position.z += normal.k*0.05f;
            function_10c880(object_index, (s_item_impact const *)&collision, &velocity, &position);
            bool surface = false;
            switch (collision.type)
            {
            case 1:
            case 3:
                surface = true;
                break;
            case 4:
                {
                    long other = *(long *)(hit + 0x40);
                    s_item_header *header = &((s_item_header *)g_4e0300->data)[other & 0xffff];
                    if (((1 << header->type) & 0x3c0) != 0)
                        surface = true;
                    else
                        surface = TEST_FIELD_BIT((((s_object_definition const *)g_4e3b44[header->item->definition_index & 0xffff].bytes)->flags >> 6) & 1);
                }
                break;
            }
            if (((surface && floor) || item->value_13a >= 4) && slow && !(collision.value_58 & 0x20))
            {
                position = point;
                item->value_13a = 0;
                function_10d710(&normal, object_index, &point, &position);
                velocity = *g_4687a4;
                real dot = item_dot(angular, normal);
                angular.i = normal.i*dot; angular.j = normal.j*dot; angular.k = normal.k*dot;
                item->spin_axis = normal;
                update_position = false;
                settled = true;
                function_b9b90(object_index, true);
                switch (collision.type)
                {
                case 1:
                case 3:
                    ((s_item_flags_view_10b850 *)item)->flag5 = true; ((s_item_flags_view_10b850 *)item)->flag6 = false;
                    item->bsp_index = g_4686c4;
                    item->surface_index = collision.surface_index;
                    item->material_index = collision.material_index;
                    item->value_136 = collision.value_58;
                    item->value_137 = collision.value_59;
                    break;
                default:
                    {
                        long other = *(long *)(hit + 0x40);
                        short node = *(short *)(hit + 0x46);
                        transform4x3f *matrix = function_b8bd0(other, node);
                        ((s_item_flags_view_10b850 *)item)->flag5 = false; ((s_item_flags_view_10b850 *)item)->flag6 = true;
                        *(long *)((byte *)item + 0x13c) = other;
                        *((byte *)item + 0x13b) = (byte)node;
                        function_142700(matrix, &point, (point3f *)((byte *)item + 0x140));
                    }
                    break;
                }
            }
            else
            {
                if (slow)
                {
                    long count = item->value_13a + 1;
                    item->value_13a = count > 254 ? 254 : (byte)count;
                }
                else item->value_13a = 0;
                real dot = item_dot(local_c01284_3, normal);
                real correction = dot * ((surface && floor) || -dot > 1.0f ? -1.4f : -1.8f);
                if (collision.type != 1 && collision.type != 3 && correction > 15.0f) correction = 15.0f;
                if ((collision.value_58 & 0x20) && collision.type == 1)
                {
                    byte *materials = *(byte **)((byte *)g_4e0340 + 0x2c);
                    long material = *(short *)(materials + *(long *)(hit + 0x50)*8 + 6);
                    vector3f flow;
                    if (function_181db0(material, &flow))
                    {
                        real length = function_30bf0(&flow);
                        if (length != 0.0f)
                        {
                            real amount = g_510c54->rate * 12.0f;
                            vector3f side = item_cross(normal, flow);
                            real along_side = item_dot(side, local_c01284_3);
                            if (fabs(along_side) < 3.25f) item_add_scaled(&velocity, side, along_side < 0.0f ? -amount : amount);
                            real along_flow = item_dot(flow, velocity);
                            if (along_flow < length) item_add_scaled(&velocity, flow, along_flow < 0.0f ? -amount : amount);
                            real lift = -dot;
                            if (lift < 1.5f) lift = 1.5f;
                            correction = lift * 1.8f;
                        }
                    }
                }
                item_add_scaled(&velocity, normal, correction);
                position = point;
                if (function_168f40(0x24909c0d, (s_vehicle_ray const *)&position, object_index, NONE))
                {
                    position.x = point.x + normal.i*0.05f;
                    position.y = point.y + normal.j*0.05f;
                    position.z = point.z + normal.k*0.05f;
                }
                if (function_168f40(0x24909c0d, (s_vehicle_ray const *)&position, object_index, NONE)) position = item->position;
            }
            item->ignore_object_index = NONE;
        }
        if (update_position) function_b75a0(object_index, &position, NULL, NULL, (s_location *)((byte *)&collision + 0x1c), false);
        if (carrier) item_add_scaled(&velocity, carrier_delta, 1.0f);
        function_b7740(object_index, &velocity, &angular, false);
        function_1c4b00(object_index, &velocity, &angular, 1);
        if (settled) function_10d5f0(object_index);
        if (item->object_flags & (1 << 14)) function_bb950(object_index, true, real_to_long((real)g_510c54->field_2_3 * 15.0f));
    }
    if (item->flag4)
    {
        s_object_marker marker;
        if ((item->flags_c1 & 1) && function_b8d30(object_index, 0xc0000c1, &marker, 1, false))
        {
            transform4x3f matrix = marker.matrix;
            item_spin_vector(&matrix.forward, item);
            item_spin_vector(&matrix.up, item);
            item_cross_product(&matrix.up, &matrix.forward, &matrix.left);
            item_cross_product(&matrix.left, &matrix.up, &matrix.forward);
            function_30bf0(&matrix.forward); function_30bf0(&matrix.left); function_30bf0(&matrix.up);
            function_b92d0(object_index, &marker, &matrix);
        }
        else
        {
            item_spin_vector(&item->forward, item);
            item_spin_vector(&item->up, item);
        }
        function_30bf0(&item->up);
        vector3f side;
        item_cross_product(&item->up, &item->forward, &side);
        item_cross_product(&side, &item->up, &item->forward);
        function_30bf0(&item->forward);
    }
}


struct s_damage_owner
{
	long player_index;
	long object_index;
	short team;
};

/* a damage event (0x88 bytes) */
struct s_type_1e6529
{
	long definition_index;
	union
	{
		byte unknown04[4];
		dword flags;
	};
	s_damage_owner owner;
	long unknown14;
	long unknown18;
	long unknown1c;
	short unknown20;
	short unknown22;
	point3f position;
	point3f origin;
	vector3f direction;
	vector3f node_direction;
	real unknown54;
	real unknown58;
	real unknown5c;
	real distance;
	real distance_scale;
	bool in_unknown_radius;
	byte unknown69[3];
	vector3f cone_direction;
	real unknown78;
	short unknown7c;
	short unknown7e;
	byte unknown80[4];
	bool unknown84;
	byte unknown85[3];
};


#include "object_queries.h"
struct s_16eb20_block;
void function_16eb20(s_16eb20_block const *);
void function_bc190(long, s_damage_owner *);
void function_d6660(s_type_1e6529 *, long);
long __stdcall function_d6c80(s_type_1e6529 *, long);
void function_bba20(long);
long function_184000(long index, long bit);
void __stdcall function_b8540(long);

static __forceinline void item_release_impulse_10b4d0(long object_index)
{
	vector3f const *direction = g_4687bc;
	real scale = g_510c54->rate * g_51e9c4->unknown0;
	vector3f velocity = {direction->i*scale, direction->j*scale, direction->k*scale};
	function_10cf80(&velocity, object_index, false);
}
// MiniMax A (11-20): no new form, no lower score (140): the release flag is still cleared with a load, an and and a
// store where retail masks the byte in place, the returned byte still sits at esp+0xe where retail keeps it at esp+0xf,
// and the release impulse is still one shared copy where retail keeps two that differ in where the flag clear sits;
// splitting the impulse so that the flag-5 path clears the flag before the velocity is computed and the flag-6 path
// clears it after measures 223, and clearing through the flag bits themselves measures 257, so the body stands.
// MiniMax B (11-18, round 2): no new form, no lower score (140): the permuter over this body (17 tries, 301 s) found
// nothing below the 140, and the one new form - the spin flag's local lifted to the function's scope so that it
// takes a frame slot of its own - compiles to the same code (140 at 892 bytes, the first difference still at
// +0x4c), so the body stands; the result byte still sits at [esp+0xe] where retail has [esp+0xf], the release flag
// is still cleared by a load, an and and a store where retail masks the byte in place, and the impulse helper is
// still shared here where retail has two inlined copies, and the callee 0xba300's parameter order is fixed in a file
// this lane may not edit: BLOCKED-BY 0xba300's file.
// MiniMax B (11-18): no new form, no lower score (140): the frame 0xac agrees with retail's, but the returned byte
// still sits at +0xe where retail keeps it at +0xf, retail clears the release flag with one byte read-modify-write where
// this build splits it into a load, an and and a store, and the event and point homes and the location call's setup
// still differ; no form of the return value's home or the flag clear lowered the score.
// MiniMax H (11-18): no new form, no lower score (140): the release byte is still cleared with a load, an and and a
// store where retail masks the byte in place, the release impulse is still one shared copy where retail keeps two, and
// the returned byte still sits at esp+0xe where retail keeps it at esp+0xf; the flag clear moved into the impulse
// helper (a mask argument) measures 198, the flag-4 test without its own local measures 192, and the direct success in
// the damping branch measures 190, so the body stands.
// MiniMax R (11-16): no new form, no lower score (140): the release scale's two factors swapped in the source leave the
// bytes unchanged (the compiler picks the operand order), retail clears the release flag with one byte read-modify-write
// where this build splits it into a load, an and and a store, retail keeps two copies of the release impulse (the flag-5
// path computing the first two components and the flag-6 path its own) where the shared helper here gives one copy that
// both paths jump to, and the returned byte still sits at esp+0xe where retail keeps it at esp+0xf. This round's battery
// on the flag clear (the byte-bitfield view, a byte pointer local, the plain assignment and the inverted mask) leaves all
// four at 140 with the same bytes: this build always splits the byte mask into a load, an and and a store.
// MiniMax H (11-11): no new form, no lower score (140); the release flag is still cleared by a load and a store where retail clears it with one byte read-modify-write, the return byte still sits at esp+0xe where retail puts it at esp+0xf, and the damage data block and the position local still sit twelve bytes below retail's.
// MiniMax A (11-06, run 52): no new form, no lower score (140) - retail keeps the two release
// impulse paths apart with the flag clear at a different point in each, so splitting ours that way
// (223) and giving the clear a byte-unit bit field (206) both score worse.

// MiniMax I (11-20): no new form, no lower score (140): the frame 0xac agrees and the differences are still the return byte's slot (esp+0xe here, esp+0xf in retail) and the release flag's clearing (a load and a store here, one byte read-modify-write in retail, which needs a narrower cast of the field's own address); retail also keeps two copies of the release impulse where a shared helper gives one copy.
// MiniMax B (11-19): no new form tried this round, no lower score (140): the frame 0xac agrees and the remaining
// differences are the return byte's slot (esp+0xe here, esp+0xf in retail) and the release flag's clearing (a load
// and a store here, one byte read-modify-write in retail, which needs a narrower cast of the field's own address - a
// form the rules bar). The clear written through the union's own bitfields (flag5 = 0 and flag6 = 0, 257 and 257)
// gives a word read-modify-write, a byte-unit bitfield member added to the union gives 192, and folding the spinning
// flag into the result instead of OR-ing it later gives 190, so the byte member stays a byte member.
// MiniMax E (11-15): no new form, no lower score (140): the return byte still sits at esp+0xe where retail keeps it at esp+0xf, and the release flag is still cleared with a load and a store where retail masks the byte in place.
// MiniMax B (11-13): no new form, no lower score (140): retail clears the release flag with one byte read-modify-write (a narrower cast of the field's own address, a banned form) and keeps two separate copies of the release impulse, one clearing the flag before it and one after it, where a shared helper gives one copy and the two calls then merge.
// MiniMax G (11-12): no new form, no lower score (140): nothing new this round; frame 0xac agrees; the return bytes +0xe/+0xf,
// the release-byte read-modify-write and the tail still differ.
// MiniMax G (11-12): no new form, no lower score (140): frame 0xac agrees; the return bytes +0xe/+0xf, the
// release-byte read-modify-write and the tail still differ.
// MiniMax H (11-12): no new form, no lower score (140): the return byte still sits at esp+0xe where retail has esp+0xf, the release byte is still cleared with a load, an and and a store where retail masks the byte in place, and the transform helper is still called out of line.
// MiniMax G (11-11): no new form, no lower score (140); the return byte sits at esp+0xe where retail puts it at esp+0xf, the
// release flag is cleared by a load, an and and a store where retail masks the byte in place, and retail expands the transform
// helper into the call site while this build calls it out of line from a file this lane may not edit.
// MiniMax S (11-10): no new form, no lower score (140): the return byte sits at esp+0xe where retail puts it at esp+0xf, the release flag is still cleared by a load and a store where retail clears it with one byte read-modify-write, our frame carries an extra byte local at 0x12, and the damage data block sits twelve bytes below retail's.
// MiniMax I (11-10): no new form, no lower score (140); the release flag is still cleared by a load and a store where retail clears it with one byte read-modify-write, and the return byte still sits at esp+0xe where retail puts it at esp+0xf.
// MiniMax D (11-05): no new form, no lower score.
// MiniMax S (11-14): no new form, no lower score (140): the release helper was respelled so that the rate is
// loaded into a local of its own before the direction pointer, which compiles to the same code; retail's spin
// branch writes the flag byte directly and keeps no home of its own for the spin bit, and retail keeps two
// separate copies of the release impulse where this build merges them into one.
// MiniMax I (11-15): no new form, no lower score (140): the release byte is still cleared by a load, an and and a store
// where retail clears it with one byte read-modify-write - neither the raw byte mask nor the byte-bitfield view
// (s_item_flags_view_10b850) makes this build emit the memory form; dropping the spin local so that the returned byte
// could sit at esp+0xf costs eighteen bytes of body length (874 against 892) and raises the score to 190, so the body stands.
// MiniMax S (11-16): no new form, no lower score (140): the release byte is still cleared by a load, an and and a store where retail
// clears it with one byte read-modify-write, and the return bytes, the event-point homes and the location-call setup remain; that call
// setup follows the allocation callee 0xba300's own convention (retail takes the object in ecx and the location in eax), whose body is
// in another file: BLOCKED-BY 0xba300; the permuter (annealing, 420 s, 15 tries) found no variant below 140.
// score 140 (was 140): frame 0xac agrees; the returned byte sits at +0xe and the result byte at +0x12 where retail has
// +0xf and +0x13, the release byte is cleared by a load, an and and a store where retail clears it with one memory
// read-modify-write, and our one shared release call stands where retail computes the impulse twice and shares a tail.
// event/point homes and location-call setup remain. Paired existing point setters kept.
// Earlier: the frame 0xac agrees; the release byte's read-modify-write,
// the tail, the event-point homes and the location-call setup are unchanged, and
// this round's trials in this file left them where they are
// the frame 0xac agrees; the release byte's read-modify-write,
// the tail, the event-point homes and the location-call setup remain, and this
// round's trials in this file left them where they are
// Frame 0xac agrees; release-byte RMW/tail, event-point homes and location-call setup remain.
// Frame 0xac agrees; release-byte RMW/tail, event-point homes and location-call setup remain.
// Frame 0xac agrees; return byte +0xe versus +0xf, release-byte RMW/tail and event/location homes remain.
// Earlier: the frame 0xac agrees; the release byte's read-modify-write,
// Earlier: Frame 0xac agrees; release-byte RMW/tail, event-point homes and location-call setup remain.
// Frame 0xac agrees; release-byte RMW/tail, event-point homes and location-call setup remain.
// Earlier: Frame 0xac agrees; release-byte RMW/tail, event homes and protected location-call setup remain.
// Earlier 154 (was 154): frame 0xac agrees; release RMW/tail, event homes and location-call setup remain.
// Frame 0xac agrees; release-byte RMW/tail and event/location homes remain.
// frame 0xac agrees; release RMW/tail, event homes and location-call setup remain.
// Earlier 182 (was 182): frame 0xac agrees; release-byte clears, vector-tail sharing
// and event-point homes differ; impulse loop 182; damping loop 182; both loops 182;
// source-vector reference 182; owner/request at entry 184.
// release-byte RMW, impulse sharing and event-point homes differ;
// scale-first 204; shared release with current carriers 182; shared damping 182; both scales 183;
// detached arm first 199; expiry guard first 181; event point function scope 184.
// impulse output name 182; contact output name 182.
// both output names 182; immutable damage key 182.
// read-only event result 182; ordinary release inline 290.
// immutable release scale 182; angular input value 191.
// rate input snapshot 182; gain input snapshot 182; both input snapshots 182; direction input 204.
// external direction reference input 204; rate input formal 182; scale input formal 204; separate impulse noinline 182.
// Earlier 163 (was 182): frame 0xac agrees; release RMW/tail, event-point homes and location-output call setup remain.
// Event-point value 174; origin/position publication copies 163/163; item/angular references 182/182;
// integrated expiry 181; owner entry/impulse output lifetimes 182/182. Origin publication copy retained.
// Known-set XOR removals on flag 5/6/both 182/182/182; subtractive removals
// on flag 5/6/both 182/182/182; XOR/subtract mixed both ways 182/182.
// Earlier 154 (was 163): Release RMW/tail and event homes remain; idle/count results 166/169;
// earlier damage key 185; event-first/key plus event-first 196/196; early complete point/origin
// publication 198/157; initializer-owned sentinel 154 kept, combined origin 176.
// Earlier 154 (was 154): release-byte RMW/tail, event homes and location-call setup remain.
// Native byte clears 5/6/both 154/154/154; native guard/clear 5/6/both 192/167/207;
// all native guards/clears 216; native guards with original byte stores 216.
// release-byte RMW/tail and event-point homes remain.
// Scalar/component factor orders, eight forms 154; direct output 207; native
// point value 176; owner/origin/position/pair/all library copies 154; direct
// output with library copies 207. Retained origin reuse and initializer unchanged.
// Earlier 154 from 154: release-byte RMW/tail and event homes remain.
// Returned vector/output buffer 274/274; output with shared scaler 200;
// native attachment handle/node plus point/all fields 154/154/154;
// all fields with output buffer/returned vector 274/274.
// release-byte RMW/tail, event homes and location-call setup remain.
// Earlier 154 (was 154): release-byte RMW/tail, event homes and location-call setup remain.
// Surface and parent queries inline/noinline 154 each; contact noinline 154;
// event initializer inline/noinline 154 each; final update noinline 154.
// release-byte RMW/tail and event/location homes remain;
// signed/word update results 155/188; release-byte reference/pointer 200/200;
// native category 154, with byte reference/signed result 200/155; unsigned attachment 154.
// Native tag flags/child block/damage/effect/all fields 154 each;
// common contact/event point at entry or active scope 201;
// native tag plus common point 201. Complete tag, helper and body restored.
// Native state-byte/countdown/both field references 200/200/200; damage-key reference 154; state/key, countdown/key and all three 200 each; surface-contact expression 200. Original release and event code restored.
// Complete scalar release-product lifetimes before ijk vector publication, all six
// computation orders 200 each; immutable products 200; genuine output declaration
// before inputs with full product staging 200. Frame 0xac remains; original producer restored.
// Round 140: eight caller-only declaration/canonical-call forms tie 154 and are excluded diagnostics. Eight
// coordinated callee declarations (point, matrix, owner, parent, effect and combinations) all 154; owner forms
// lose a reduced control. Every caller/provider declaration and executable body restored.
// Round 134: native direction value alone/with rate/with scale/with separate factors,
// index first 194/194/205/194; index last 194/194/205/194.
// Direction global at every use with bool/long/short update result 229/243/242; bool/byte/short/long idle result 223/223/243/244; entry bool idle 224 (fast). All restored; release RMW/tail and event/location homes remain.
// Release-vector publication ikj/jik/jki/kij/kji 157/160/208/162/208;
// angular damping jki/kij/kji 155/155/155. Original release helper and body
// restored; byte RMW/tail and event/location homes remain.
// Round 376: Predicate-produced surface/attachment/both results 244/228/185; both with signed-
// byte/byte/short/word/long carriers 183/183/196/196/196. Original release and event code
// restored.
// Event owner/category/magnitude publication permutations 154/154/154/154/154; location first, owner last and location between owner/scalars 154/154/154. Original event and release code restored.
// Whole flags byte as unsigned/signed/plain-char full-byte bitfields 154/154/154;
// plain/signed-char scalars 154/154; full-byte bitfields with byte-unit named
// flags 216/216/216. Original record declaration, masks and entire body restored.
// Round 221: current event graph with object roots live, normal/entry result
// 414/414; tag roots live 228/228; both roots 388; both with byte/long/short
// result 395/387/388. Original root snapshots, result and body restored.
// Round 248: Shared release state-mask/updated-byte inputs, publication before
// inputs/after coefficient/after vector 200/203/204 each; reacquired item
// before inputs/after vector 200/204. Original helper and byte stores restored.
// Existing scaler return consumed by impulse call, direct/native-copy/library-copy/early-output forms 200/246/246/200; each with shared damping 200/246/246/200. Original release helper, includes and damping restored.
// DH round 189: Native/const/library attachment matrix values 151/151/151; native/const offsets
// 183/183; native/const/library paired values 190/190/190. All one-use snapshots restored; matrix
// gain only three, despite retail length. Original helper, contact/event code and includes
// retained.
// Round 259: Complete contact/event/owner/impulse workspace-name families, eight
// lexical home orders 154 each. Original helper and all workspaces restored; release
// byte RMW/tail and event-point homes remain.
// Round 205: immutable addressed impulse output, with none/rate/gain/both/
// direction/direction-rate/direction-gain/all input snapshots 154/200/200/200/194/194/194/194.
// Original mutable output and input graph restored; release-byte RMW/tail and event homes remain.
// Round 223: unsigned/signed word and dword full-byte bitfields, each with word/byte named-bit units,
// 185/213/185/213/185/213/185/213. Actual record layout and complete body restored.
// Round 237: Two-use spinning predicate feeds accumulated result, bool/byte/short/long after damping and before damping: 158/191/191/195/190/239/188/193.
// Keep the bool spin predicate for damping admission and result accumulation after damping; it is observed once and used twice. All other variants restored; frame 0xac, return-byte home +0xe versus +0xf and release/event/location differences remain.
// Round 210: On the retained twice-used spin graph, spin entry declaration, spin name, both,
// result name, result plus entry, both names, and both names plus entry 158 each; that complete
// home graph with byte accumulator 159 (873 bytes). Names do not move the +0xe return byte to
// +0xf; frame 0xac, release tail and event homes remain. Original names, declarations, Boolean
// accumulator, impulse helper and complete body restored.
// Round 46: Captured-spin logical OR previous-first/spin-first/conditional 185/188/202; byte bitwise/logical accumulation 159/182; long bitwise/logical 178/205; entry bool with logical OR 186. Original body restored.
// Direct native event origin/position getter output, native/library parallel copy,
// owner/scalar publication before/after getter: all eight 227 (921 bytes, frame 0xa0).
// Original separate point, returned receiver, initializer order and complete body restored.
// Round 465: uninitialized owner/event/point entry-declaration subsets 1..7, then all three
// at active-object scope, 158 each on the two-Boolean graph. Original workspaces and full body restored.
// Round 267: Conditional success publication after damping with bool/byte/short/long accumulator
// 202/197/212/213; before damping 202/197/212/213. All exact-match-only forms restored;
// original twice-used spin snapshot and bitwise result accumulation retained.
// Round 269: Native two-use spin byte/char/short/long 191/190/191/195; exhaustive branch-owned
// result defaults with those carriers 191/190/191/195. Frame/return-byte/release homes remain;
// original spin Boolean, default, helper and complete body restored.
// N271: full spin-byte capture, byte/signed-byte/word/dword with mask decoding 191/194/189/190;
// with shift decoding 158/158/185/189. Original twice-used Boolean and complete body restored;
// result byte +0xe/+0xf, release-byte RMW/early impulse sharing and event homes remain.
// Round N274: Existing scalar setters at release/damping/event boundaries, masks 1..7,
// 171/159/180/140/140/141/141; all with entry contact point 141. Paired point setters with
// direct filled-buffer inputs 158; with entry event record 158. Minimal paired setters
// retained, plain 140 at 892/892 bytes; other graphs and original impulse helper restored.
// Round 297: Merged spin-success arm, bool/byte/short/long accumulator, completion 190/185/202/203 and entry 190/185/202/203. Bool recovers return-byte +0xf but shrinks to 874/892 bytes. Original two-use spin result, paired point setters and full body restored; release RMW/tail and event/location homes remain.
// Round 288: Native Boolean flag allocation units, with branch-spin/release-setter/damping-setter
// masks 0..7, 242/233/242/233/242/234/242/234. Native Boolean units move the frame to 0xa8 and disrupt
// entry allocation; original word units, shared release helper, paired point setters and full body
// restored.
// Round 453: Final other-update/spin reduction, bool/long and bool/byte carriers with
// bitwise/logical joins, masks 0..7 (exact-only): 195/207/193/207/209/206/209/206.
// Original executable body restored.
// Round 242: On the paired-setter/two-use-spin graph, native short/long countdown values
// captured before admission or after the positive test, with spin consumed after damping,
// 140/176/140/144; before damping 173/178/173/173. Original timer reads, spin consumption,
// paired event setters and full body restored; frame 0xac, return-byte +0xe/+0xf and
// release/event homes remain. Earlier terminal-only short work preceded this graph.
// Round 506: native event category/location/both on paired setters 140/140/140; each
// with actual filled-point consumption 158/158/158; signed category/location 140;
// complete graph with accumulated spin arm 189. Original types and full body restored.
// Round 306: Joint contact and owner/event/position entry declarations in seven subsets, then the
// complete reverse declaration order: 140 each. Original body and all workspaces restored; return byte
// +0xe/+0xf, release RMW/tail and event/location homes remain. Both existing point setters and the
// two-use spin capture are preserved.
// Round 311: Current paired-setter/two-use-spin graph with inverse idle outcome,
// bool/byte/short/long and bitwise/logical spin reduction 179/189/179/189/186/182/187/184.
// Original result, spin capture, helper, setters and full body restored; return-byte and release/event homes remain.
// Round 296: Standard native-array release/damping multiplication and paired event-transfer
// subsets 1..7 289/231/260/181/278/247/249; all with backward paired event transfer 247. Original
// helper, setters, body and includes restored; return-byte, release RMW/tail and event homes remain.
// N304: Spin field read at both uses before damping, bool/byte/short/long results
// with bitwise/logical accumulation: 173/173/168/174/198/191/185/190. Bool/byte
// recover return byte +0xf but regress; original two-use spin snapshot retained.
// Round 466: Complete independent direction-component inputs in the existing two-site
// release helper, all six acquisition orders and scale before/among captures: 171 each,
// 901 bytes. Exact-only input captures restored with the complete helper and body;
// return-byte, release-tail and event/location homes remain.
// Round 257: Surface-clause/timer-admission evaluation priority and adjacent parent
// declaration, seven subsets, then failed-attachment arm first: 139/142/140/141/139/142/141/207.
// One-point forms rejected; original paired setters, spin capture, release helper and
// complete body restored. Return byte +0xe/+0xf and release/event homes remain.
// P261: shared contact/event point at entry/active scope 137/137 (three-point gain, rejected);
// with sentinel before/after point query 157/160 in either scope; separate points with
// sentinel before/after 160/161. Paired setters, spin capture and complete body restored.
// P264: constant accumulator before object acquisition, bool/byte/short/long
// 143/144/160/160; between object/tag acquisition 143/144/160/160.
// Original result lifetime, paired point setters, two-use spin and body restored.
// P269: shared contact/event point with owner memcpy/memmove at initialization, magnitude,
// location joins 137/137/140/165/165/164; complete object/team/player publication at the
// latter two joins 132/138; the magnitude-join field publication with the common active-scope
// point reached 132 plain at 892/892. Not kept (coordinator): the same form as N315 (owner copy
// written out as three field stores, the contact and event points folded into one local), a
// steering form at exactly the keep bar. Body restored.
// N315: Complete owner-field publication (P/O/T: player/object/team)
// POT/PTO/OPT/OTP/TPO/TOP
// 138/138/137/136/138/137; metadata/location joins 138/139. OTP and OPT
// with a common contact/event output point, entry/active scope, 132/132/133/133.
// Plain 132 at 892/892 with the shared point and OTP: not kept (coordinator: the owner struct
// copy written out field by field in an order picked from six by score). Shared point alone not
// measured. Return-byte +0xe/+0xf and release/event loads remain.
// N319: Inactive-parent default with late active overwrite, long/dword parent and bool/byte/short/long accumulator: 196/192/200/202/196/192/200/202. Original parent join and full body restored; return-byte/release/event homes remain.
// AC491: Independent contact/activation/expiry outcome results, masks 1/2/4/3/5/6/7, then direct spin
// arm: 190/196/196/174/174/212/202/202. NOT kept: conditional outcome flags require an exact match.
// Original accumulator, spin capture and body restored.
// N333: One captured object-activation predicate drives admission and parent selection,
// bool/byte/short/long with conditional/branch parent producer: 193/193/195/194 in each graph.
// Original parent join and full body restored; return-byte/release/event homes remain.
// NOT kept: extra predicate/width-selected graphs remained unmatched and regressed.
// N335: Exhaustive active/inactive result construction, bool/byte/short/long: 140/141/160/162;
// exhaustive inner-state construction: 140/141/160/162. No gain; complete original body
// restored. Return-byte, release tail and event/location homes remain.
// N337: Late selected-parent presence/missing predicates, bool/byte/short/long,
// 146/146/151/150 in each polarity. NOT kept: conditional predicates remain unmatched;
// original parent handle, return byte, release helper and complete body restored.
// AC501: Completed spin/expiry-owner/item-tag phase scopes, masks 1/2/4/3/5/6/7,
// then all with active-phase spin declaration: 140/140/143/140/143/143/143/143.
// Original complete body restored; return-byte, release and event homes remain.
// N339: Staged release rate-to-scale accumulator with native aggregate/setter/array/
// copied-vector outputs, receiver before then between factors:
// 140/140/171/237/140/140/140/237. Original helper and full body restored.
// NOT kept: no gain; copied-value and input-order forms remain unmatched.
// Return byte, release RMW/tail and event/location homes remain.
// N341: genuine source-point/matrix input pointers and native node byte, seven independent/joint
// input graphs 140/140/140/140/140/140/141; complete matrix-before-address graph 141.
// NOT kept: all one-use input carriers remain unmatched. Original helper, two-use spin,
// paired point setters and body restored; return-byte, contact/event and release homes remain.
// N346: Immutable positive twice-used spin bool/byte/short/long, direct/conditional:
// 140/140/173/173/177/177/176/176. Original mutable spin, paired setters and full body restored;
// NOT kept: unmatched predicate/representation forms. Return-byte and release/event homes remain.
// AC503: Existing two-site release producer owns the actual item receiver and byte/dword mask,
// entry/direction/scale/vector clear boundaries 140/140/182/183/140/140/182/183. Original helper and complete body
// restored; return byte, release RMW/tail and event homes remain. NOT kept: unmatched producer forms.
// N350: Complete update counts in byte/short/long/dword 143/173/176/176;
// category-bit aggregation 182/181/181/181 on the paired-setter graph. NOT kept:
// numeric return representations regress; original Boolean accumulator and full body restored.
// B395: Captured-spin logical OR on paired point setters, result/spin-first with
// bool/byte/short/long accumulators: 179/176/186/183/185/179/186/179. NOT kept: all
// regress; original bitwise join, two-use spin value, helper and complete body restored.
// B401: Complete unchanged-state conjunction accumulator, positive/negative spin truth,
// bool/byte/short/long: 179/179/186/187/176/176/179/181. NOT kept: unmatched result
// representations regress; original success accumulator, helper, paired setters and full body restored.
// B403: Completion-owned damping result after the call, bool/byte/short/long,
// default-zero 131/129/175/170 and exhaustive outcomes 176/176/180/176.
// NOT kept: branch-set outcome flags require equality; 129 recovers the return-byte
// home at retail length but remains unmatched. Original two-use spin and full body restored.
// B407: Native addressed contact/event/owner/payload identities on the paired-setter graph,
// four isolated and four complete role-name families: 140 each. Original names, helper,
// workspaces and complete body restored; return-byte, release and event homes remain.
// DH333: Typed owner/origin/position record-range copies, seven subsets
// 167/165/171/164/171/181/187; complete backward ranges 189. Unlike scalar-coordinate ranges,
// these copy complete records. NOT kept: regressions/shorter extent; full original body restored.
// N352: Eight return/spin lexical home pairs on the paired-setter graph: 140 each.
// NOT kept: return byte stays +0xe/+0xf and release/event homes remain; original
// names, declaration sites, helper, paired setters and complete body restored.
// N356: Pre-admission countdown truth, bool/byte/short/long with post/pre-processing accumulation:
// 184/191/298/293/177/180/186/181. NOT kept: unmatched storage/result-lifetime forms.
// Original executable source restored; Return byte, release RMW/tail and event/location homes remain.
// N358: Complementary two-consumer damping admission, bool/byte/char/signed-char/short/word/long/dword, original positive return:
// 177/177/177/177/176/176/176/176. NOT kept: unmatched source/representation forms; original executable body restored.
// Return byte, release RMW/tail and contact/event homes remain.
// N360: Native conditional spin-result selection, result-first/spin-first, bool/byte/short/long:
// 172/181/179/138/176/194/179/195. NOT kept: unmatched selection/representation forms;
// 138 gains only two and moves the frame. Original complete body restored; return-byte and release/event homes remain.
// N362: Existing impulse/contact/damage-dispatch/damping inline call visibility, four
// individuals, three impulse pairs and all four: 140/140/140/140/140/140/140/140. Original
// declarations and complete body restored; return-byte, release-tail and event homes remain.
// N364: Complete native 16-bit motion flags captured once, word/short/dword/long,
// mask then shift decoding at both consumers: 172/172/172/172/272/272/171/171.
// NOT kept: no equality; temporary external word view and original full body restored.
// Return-byte +0xe/+0xf and release/event homes remain.
// N366: Complete damping result copy/compound, interleaved/staged scalar, setter and native vector
// publication regress; return byte, release RMW/tail and event homes remain. Original damping and body
// restored. Scores 141/141/141/141/143/143/196/141. NOT kept: unmatched result-ownership forms.
// N375: Complete post-decrement mode/count expiry admission in positive/complementary
// native truth domains, eight valid forms 140 each; original full body restored.
// NOT kept: no byte match or frame/home gain; return byte and release/event homes remain.
// MiniMax C (11-06): no new form, no lower score - retail still emits the release impulse once per clear
// branch and still clears the flag byte with a read-modify-write on both paths; the callee 0xba300's
// convention lives in src/unknown_0b8bd0.cpp, which this lane may not edit.
// Earlier note: the return byte and the release-byte read-modify-write still sit in other homes,
// and the callee's parameter order is fixed in a file this lane may not edit.
// N377: Existing release producer time/gain root pointers, both and native references,
// then release-completion success at first/second/both sites and accumulated completion:
// 140/140/171/171/140/140/140/140. NOT kept: no equality or gain; full helper and body restored.
// Return byte +0xe/+0xf, release RMW/tail and event/contact homes remain.
// N382: Actual function return channel byte/signed-char/char/word/short/int/unsigned-int/long,
// with original Boolean accumulator and twice-used spin: 140/140/140/138/138/139/139/139. NOT kept: unmatched ABI
// forms; one/two-point gains fail the bar. Original signature and full body restored.
// Return byte +0xe/+0xf and release/contact/event homes remain.
// N384: Caller-owned release output at branch/physics/function/entry scopes, with
// submission in the existing producer or its returned output: 169/140/140/140/140/171/171/171.
// NOT kept: ties/regressions; original private producer and full body restored. Return byte,
// release-byte RMW/tail and contact/event homes remain.
// MiniMax A (11-11): no new form, no lower score (140); the return byte still sits at esp+0xe where retail puts it at esp+0xf, and the release flag is still cleared by a load and a store where retail clears it with one byte read-modify-write (the flag is a bitfield in a header-owned struct); the callee 0xba300's parameter order is fixed in src/unknown_0b8bd0.cpp, which this lane may not edit.
// MiniMax E (11-08): no new form, no lower score: the return byte sits at esp+0xe here and at esp+0xf in retail, and retail clears the release flag with a read-modify-write that we split into a load and a store.
// MiniMax E (11-09): no new form, no lower score: the return byte sits at esp+0xe here and at esp+0xf in retail, and retail clears the release flag with a read-modify-write that we split into a load and a store.
// P402: Complete definition-flag input at original gate, dword/long/unsigned-int/int/word/short/byte/signed-
// byte domains:
// 140/141/140/141/142/142/195/195. NOT kept: one-use field inputs require equality; original direct flag
// expression, two-use spin, release helper and full body restored. Return byte and release/contact/event homes
// remain.
// MiniMax R (11-10): no new form, no lower score; the release flag is still cleared by a load and a store where retail clears it with one byte read-modify-write (a byte pointer cast, a masked assignment and a narrowed mask all leave the code as it was), the return byte still sits at esp+0xe where retail puts it at esp+0xf, and the callee 0xba300's parameter order is fixed in a file this lane may not edit.
// MiniMax B (11-11): no new form, no lower score (140); the return flag still lands on esp+0xe where retail keeps
// it on esp+0xf, the release byte is still cleared with a load, an and and a store where retail masks the byte
// in place, and retail expands the transform helper into the call site while this build calls it out of line
// (its declaration is in src/unknown_09b910.cpp).
// MiniMax A (11-13): no new form, no lower score (140): retail clears the release flag with one byte read-modify-write (a narrower
// cast of the field's own address, a banned form), keeps two separate copies of the release impulse, one clearing the flag before it
// and one after it (a shared helper gives one copy and the two calls then merge), and returns the byte at esp+0xf with the flag byte at
// esp+0x13; dropping the spin flag's own local and setting the flag inside that branch moves the returned byte to esp+0xf but leaves
// the body 874 bytes long (190), and giving the flag6 path its own copy of the impulse makes it 936 bytes (227).
// P442: Actual activity-derived accumulated result, bool/byte/short/long, positive then
// complementary outcomes: 273/277/232/239/197/196/194/196. NOT kept: no exact result-home repair;
// original parent, spin, release, event and full body restored. Return/release/contact homes remain.
// P478 m3: Standard damping transform, origin/position/both coordinate copies,
// complementary/logical Boolean spin joins, parent predicates and countdown predicates:
// 231/165/164/181/212/170/195/195. NOT kept: no native return/release/event home repair;
// original complete body and helpers restored. Unsupported bitwise-OR functor did not compile, uncredited.
// P480 m3: Point lifetime through origin/pair publication, damage/expiry event storage,
// mutable/readonly returned receiver: 140/140/140/140/140/140/140/140. NOT kept: no native
// return-byte or event-home repair; original full body, helpers and interfaces restored.
// P486: Actual last surface/attachment/spin/activation/timer update ordinal,
// byte/char/signed-char/short/word/int/long/dword: 251/251/251/252/252/251/251/251.
// NOT kept: no return-byte/release/event-home repair and larger extents;
// original Boolean result, spin capture, helpers and entire body restored.
// T104: Returned contact-transform/origin-setter chains, mutable/readonly and actual filled
// point-query outputs: 178/171/180/171/180/157/176/176. NOT kept: all regress; original
// two-use spin, release helper, paired setters and full body restored. Return/release/event homes remain.
// MiniMax H (11-14): no new form, no lower score (140): the release flag is still cleared with a load, an and and a store where retail clears the byte in place, and the return byte still sits at esp+0xe where retail puts it at esp+0xf.
// T108: Ordinary inline metadata at existing node-transform/point/owner/point-transform providers, four isolated, three pairs and all:
// 140/140/140/138/138/138/140/138. NOT kept: no qualifying native-shape gain or equality; original complete source
// and every provider declaration restored. Native frame and first storage/scheduling gap remain.
// P509: Two-consumer isolated spin-bit mask across damping, char/signed-char/byte/short/
// word/int/long/dword: 174/174/174/169/169/173/173/173.
// NOT kept: all regress at 892 bytes; original Boolean capture, helper and body restored.
// Return-byte, release RMW/tail and contact/event homes remain.
// P523: Actual object activation bit 8, dword/long/word/short/byte/signed-char/char/bool
// allocation units: 140/201/205/203/205/205/205/205. NOT kept: no exact activation/return-
// byte or release/event recovery; original record overlay, entry test and full body restored.
// P527: Native coordinate-array impulse/damping/both for loops, then while loops;
// independent fixed-three do loops on the current spin/paired-setter graph:
// 171/140/140/171/140/140/256/208. NOT kept: no exact return-byte/release/event-home recovery; original
// shared impulse producer, damping statements and complete body restored.
// P540: Eight coordinated contact-provider orders at its definition and both callers, 140 each. NOT kept: no return-byte/release/event-home recovery; original contracts, calls, helper and entire body restored.
// MiniMax C (11-16): no new form, no lower score (140): retail still clears the release flag with one byte read-modify-write where this build splits it into a load, an and and a store, and retail still keeps two copies of the release impulse where this build shares one: BLOCKED-BY the transform helper's own file.
// MiniMax I (11-17): no new form, no lower score.
// MiniMax D (11-17): no new form, no lower score (140): the release flag is still cleared with a load, an and and a store where
// retail masks the byte in place, the returned byte still sits at esp+0xe where retail keeps it at esp+0xf, and retail still
// keeps two copies of the release impulse where this build shares one: BLOCKED-BY the transform helper's own file.
// P604: Existing item scaler and shared in-place scaler, each with captured-bit OR,
// direct success after/before damping, and logical accumulation: 140/190/190/179/140/190/190/179.
// NOT kept: both captured-bit OR forms tie. Direct success restores +0xf and
// shortens the body by 18 bytes. The logical join still spills the return byte at +0xe.
// MiniMax R (11-17): no new form, no lower score (140): the result byte sits at [esp+0xe] where retail has [esp+0xf], the flag clear is a load, an and and a store where retail has a byte read-modify-write, and the impulse helper is shared here where retail has two inlined copies; a permuter run over the body (17 tries, 308 s) found nothing below the 140.
// MiniMax A (11-19): no new form, no lower score (140): the three differences that are left are all frame or width
// ones. The returned flag lives one byte lower here (ours [esp+0xe], retail [esp+0xf] in the same 0xac frame), and
// retail keeps the event buffer at [esp+0x28] with the damage record at [esp+0x38] where this build keeps the buffer
// at [esp+0xb0] and the record at [esp+0x2c]. The release flag is a byte inside a union with word bitfields, and this
// compiler writes that union member through memory (a load, an `and` and a store) where retail masks the byte in
// place (`and byte ptr [ebp+0x12c], 0xdf`); a narrower view of the field's own address would give retail's
// instruction, but that is a cast of the field's own address. Retail computes the release impulse twice, once in
// each branch, with the two flag clears around the two copies, where this build shares one copy - BLOCKED-BY the
// transform helper's own file.
// Direct spin with separate active/inactive defaults 190, separate
// active result 206, release success before the queries 180, countdown success
// after decrement 190, returned contact point 191, center workspace used directly
// 189, returned center for both event points 171, and owner copied after flag and
// scale 190. The shared center form reaches the retail return-byte slot and length
// with the release-byte clear and impulse branches still differing.
// MiniMax G (11-20): no new form, no lower score (140): the returned flag still sits one byte lower in the
// frame than retail's, retail still clears the release flag with one byte read-modify-write where this build
// loads it, masks it and stores it back, and retail still keeps two copies of the release impulse where this
// build shares one: BLOCKED-BY the transform helper's own file.
// MiniMax R (11-20): re-checked, no new form and no lower score (140): the three flag arguments to the 0xe2d80 call are written to different stack slots than retail uses, and the flag byte is cleared with a load, an and and a store where retail clears it in place with an and against [ebp + 0x12c].
// Direct spin publication after/before the damping call with saved short countdown 190/190,
// saved long countdown 194/194, short terminal count 190/190, or long terminal count 190/190.
// NOT kept: the +0xf byte home returns, but the body shortens to 870–874 bytes against retail's 892
// and the release-flag operations still differ.
// Paired event-point coordinate passes, ascending/descending, with separate buffers with returned
// point 140/140 or filled point 171/171, common buffer with returned point 171/171 or filled
// point 158/158. The return byte, release mask and point homes still differ.
// Select or branch on one contact predicate, with success in the guarded update or with
// the complete predicate published: 238/205 for each with captured spin, 261/228 for
// each with direct damping success. NOT kept: publication with captured spin reaches
// 892 bytes with different entry registers, and direct damping shrinks the frame.
// @retail 0x10b4d0
bool __stdcall function_10b4d0(long object_index)
{
    s_item *item;
    byte *definition;
    item = ITEM_GET(object_index);
    definition = g_4e3b44[item->definition_index & 0xffff].bytes;
    bool result = false;
    long parent;
    if ((bool)((item->object_flags >> 8) & 1))
    {
        if (item->flags_c1 & 1)
        {
            if (!(bool)((*(dword *)(definition + 0xbc) >> 2) & 1))
            {
                if (TEST_FIELD_BIT(item->flag5) && item->bsp_index == g_4686c4)
                {
                    if (item->value_136 & 8)
                    {
                        if (!(byte)function_184000(item->surface_index, item->value_137))
                        {
                            item->flags_12c &= 0xdf;
                            item_release_impulse_10b4d0(object_index);
                        }
                        result = true;
                    }
                }
                else if (TEST_FIELD_BIT(item->flag6))
                {
                    long other = *(long *)((byte *)item + 0x13c);
                    if (function_badc0(other, ~0UL))
                    {
                        point3f point;
                        transform4x3f_apply_point(function_b8bd0(other, *((byte *)item + 0x13b)),
                            (point3f *)((byte *)item + 0x140), &point);
                        function_10d710(&item->spin_axis, object_index, &point, NULL);
                    }
                    else
                    {
                        item->flags_12c &= 0xbf;
                        item_release_impulse_10b4d0(object_index);
                    }
                    result = true;
                }
            }
            bool spinning = TEST_FIELD_BIT(item->flag4);
            if (spinning)
            {
                item->angular_velocity.i *= 0.9f;
                item->angular_velocity.j *= 0.9f;
                item->angular_velocity.k *= 0.9f;
                function_10d5f0(object_index);
            }
            result |= spinning;
        }
        if (!(item->flags_c1 & 1))
        {
            function_bba20(object_index);
            result = true;
        }
        if (item->value_12e > 0)
        {
            --item->value_12e;
            if (g_4e6948->mode != 4 && item->value_12e == 0)
            {
                s_damage_owner owner;
                function_bc190(object_index, &owner);
                long damage_index = *(long *)(definition + 0x110);
                if (damage_index != NONE)
                {
                    point3f position;
                    s_type_1e6529 data;
                    point3f *event_point = function_b9dd0(object_index, &position);
                    function_d6660(&data, damage_index);
                    data.owner = owner;
                    *(byte *)&data.unknown84 = 4;
                    data.unknown54 = 1.0f;
                    object_get_root_location(object_index, (s_location *)&data.unknown1c);
                    real_point3d_set(&data.origin, event_point->x, event_point->y, event_point->z);
                    real_point3d_set(&data.position, data.origin.x, data.origin.y, data.origin.z);
                    function_d6c80(&data, NONE);
                }
                function_176780(object_index, (s_effect_owner const *)&owner, 0.0f, *(long *)(definition + 0x128), 0.0f, NULL, NULL);
                function_b8540(object_index);
            }
            result = true;
        }
        parent = item->location_c8;
    }
    else
    {
        parent = item->parent_index;
    }
    if (parent != NONE && *(long *)(definition + 0x104) > 0)
        function_16eb20((s_16eb20_block *)(definition + 0x104));
    function_10b850(object_index);
    return result;
}
