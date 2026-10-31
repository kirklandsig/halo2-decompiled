// @flags /O2 /arch:SSE /Gr
/* ITEMS.CPP: the item object type (weapons, equipment and garbage; its
   definition is at 0x467c08) */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "sound_sources.h"
#include "vector_products.h"

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
	return ((upper - lower) * ((real)(*seed >> 16) * (1.0f / 65535.0f))) + lower;
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
plane3f *bsp3d_get_plane(s_bsp3d const *bsp, short plane_index, plane3f *plane);
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

// score 211 (was 211): Frame 0xa0 agrees; protected plane-index width, plane joins and pending spin-k homes differ.
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
				if (item->surface_index == NONE)
				{
					s_item_collision_data *collision = (s_item_collision_data *)g_4e0340;
					bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
					surface_plane.i = plane.i;
					surface_plane.j = plane.j;
					surface_plane.k = plane.k;
					surface_plane.d = plane.d;
				}
				else
				{
					s_item_collision_bsp *bsp = (s_item_collision_bsp *)g_4e0348;
					s_item_collision_data *collision = (s_item_collision_data *)(bsp->sections +
						bsp->instances[item->surface_index].section_index * 0xc8 + 0x70);
					bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
					transform4x3f const *matrix = &bsp->instances[item->surface_index].matrix;
					surface_plane.i = matrix->up.i * plane.k + matrix->left.i * plane.j + matrix->forward.i * plane.i;
					surface_plane.j = matrix->up.j * plane.k + matrix->left.j * plane.j + matrix->forward.j * plane.i;
					surface_plane.k = matrix->up.k * plane.k + matrix->left.k * plane.j + matrix->forward.k * plane.i;
					surface_plane.d = matrix->position.z * surface_plane.k + matrix->position.y * surface_plane.j +
						matrix->position.x * surface_plane.i + matrix->scale * plane.d;
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

struct s_item_impact
{
	long type;
	byte unknown04[4];
	point3f point;
	byte unknown14[8];
	s_location location;
	short material;
	byte unknown26[2];
	vector3f normal;
	byte unknown34[0x40 - 0x34];
	long object_index;
};

real function_1201a0(vector3f *v, vector3f const *fallback);
void function_188180(point3f const *point, vector3f const *forward, long tag_index, long object_index, long index, long variant,
	long unused, long effect_value, s_location const *location, real scale);
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
			(word)impact->material, (long)&direction, &impact->location, scale);
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

// score 130 (was 130): frame 0xc0 versus 0xb4 and initial product ranks differ. Shared scale 130; inline
// depth one 130; size preference 277; complete rotated vector 207; fallback vector initializer 130.
// The axis as three real locals (no cross3f address) gives retail's frame 0xb4 and 848 bytes but 159.
// The square's products come out i, k, j whatever their source order; retail's j, i, k come only
// from a separate statement for the dot product, which then folds facing loads into mulss: 173-175.
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
// score 813 (was 813): Frame 0x154 versus 0x12c; entry Boolean publications and collision/carrier/rotation homes remain.
// Earlier: Frame 0x154 versus 0x12c; entry Boolean publications and carrier/collision/flow/marker/rotation homes differ.

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
        bool carrier = function_109a00(object_index, &local_c01284_3, true, &carrier_delta);
        vector3f velocity = item->linear_velocity;
        vector3f angular = item->angular_velocity;
        bool fixed = ((*(dword *)(definition + 0xbc) >> 2) & 1) != 0;
        bool settled = false;
        if (!fixed)
        {
            velocity.k -= g_51e9c4->unknown0 * dt;
            local_c01284_3.k -= g_51e9c4->unknown0 * dt;
        }
        point3f position = {velocity.i*dt + item->position.x, velocity.j*dt + item->position.y, velocity.k*dt + item->position.z};
        vector3f ray = {position.x-item->position.x, position.y-item->position.y, position.z-item->position.z};
        s_collision_result_1697c0 collision;
        collision.unknown24 = NONE;
        bool update_position = true;
        if (function_1697c0(0x24909c0d, &item->position, &ray, item->ignore_object_index, NONE, &collision))
        {
            byte *hit = (byte *)&collision;
            vector3f const &normal = *(vector3f *)(hit + 0x28);
            point3f const &point = *(point3f *)(hit + 8);
            bool floor = normal.k > 0.707099974155426f;
            bool slow = 1.5f > -item_dot(local_c01284_3, normal);
            position.x = point.x + normal.i*0.05f;
            position.y = point.y + normal.j*0.05f;
            position.z = point.z + normal.k*0.05f;
            function_10c880(object_index, (s_item_impact const *)&collision, &velocity, &position);
            bool surface = collision.type == 1 || collision.type == 3;
            if (collision.type == 4)
            {
                long other = *(long *)(hit + 0x40);
                s_item_header *header = &((s_item_header *)g_4e0300->data)[other & 0xffff];
                surface = ((1 << header->type) & 0x3c0) != 0;
                if (!surface) surface = (g_4e3b44[header->item->definition_index & 0xffff].bytes[2] & 0x40) != 0;
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
                fixed = false;
                settled = true;
                function_b9b90(object_index, true);
                if (collision.type == 1 || collision.type == 3)
                {
                    ((s_item_flags_view_10b850 *)item)->flag5 = true; ((s_item_flags_view_10b850 *)item)->flag6 = false;
                    item->bsp_index = g_4686c4;
                    item->surface_index = collision.surface_index;
                    item->material_index = collision.material_index;
                    item->value_136 = collision.value_58;
                    item->value_137 = collision.value_59;
                }
                else
                {
                    long other = *(long *)(hit + 0x40);
                    short node = *(byte *)(hit + 0x46);
                    transform4x3f *matrix = function_b8bd0(other, node);
                    ((s_item_flags_view_10b850 *)item)->flag5 = false; ((s_item_flags_view_10b850 *)item)->flag6 = true;
                    *(long *)((byte *)item + 0x13c) = other;
                    *((byte *)item + 0x13b) = (byte)node;
                    function_142700(matrix, &point, (point3f *)((byte *)item + 0x140));
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
                            vector3f side = item_cross(flow, normal);
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
            update_position = fixed;
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

// score 158 (was 158): the frame 0xac agrees; the release byte's read-modify-write,
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
                    data.origin = *event_point;
                    data.position = data.origin;
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
