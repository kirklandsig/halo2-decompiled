// @flags /O2 /arch:SSE /Gr
/* ITEMS.CPP: the item object type (weapons, equipment and garbage; its
   definition is at 0x467c08) */

#include "unknown_11c920.h"
#include "globals.h"
#include "object_markers.h"
#include "object_iterator.h"
#include "sound_sources.h"

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

// @retail 0x10c850
void function_10c850(long item_index)
{
	s_item *item = ITEM_GET(item_index);

	function_b9b90(item_index, false);
	item->value_13a = 0;
	item->flag5 = false;
}

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

// @retail 0x10d5f0
void function_10d5f0(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	vector3f axis = item->angular_velocity;
	real length = (real)sqrt(axis.i * axis.i + axis.j * axis.j + axis.k * axis.k);

	if (fabs(length) < 0.0001f)
	{
		length = 0.0f;
	}
	else
	{
		real inverse = 1.0f / length;
		axis.i = inverse * axis.i;
		axis.j = inverse * axis.j;
		axis.k = inverse * axis.k;
	}

	if (length > 0.001f && !(item->flags_c1 & 1))
	{
		real angle = length * g_510c54->rate;

		item->flag4 = true;
		item->spin_axis = axis;
		item->spin_sine = (real)sin(angle);
		item->spin_cosine = (real)cos(angle);
	}
	else
	{
		item->flag4 = false;
		item->spin_sine = 0.0f;
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

	item->object_flags |= 0x1000;
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
	dword : 30;
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

// @retail 0x10ccc0
void function_10ccc0(long item_index)
{
	s_item *item = ITEM_GET(item_index);
	bool held_by_player = false;

	if (item->flags_12c & 1)
		held_by_player = ((s_item_unit *)ITEM_GET(item->unit_index))->player_index != NONE;
	if (held_by_player)
		item->flags_12c |= 8;
	else
		item->flags_12c &= ~8;
	if (((1 << ((s_item_header *)g_4e0300->data)[item_index & 0xffff].type) & 4) && TEST_FIELD_BIT(item->flag16c_6))
		function_15e300(item_index);
}

#include "effects.h"

/* function_259d0 (0x259d0) on the first seed of g_4e7408, inlined */
inline real item_real_random_range(real lower, real upper)
{
	dword *seed = &g_4e7408->unknown0;
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return lower + (upper - lower) * ((real)(*seed >> 16) * (1.0f / 65535.0f));
}

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
		long ticks;

		owner.unknown4 = owner_item->location_c8;
		owner.unknown0 = owner_item->location_c4;
		owner.unknown8 = owner_item->location_c2;
		function_176780(item_index, &owner, 0.0f, definition->effect_tag_index, 0.0f, NULL, NULL);
		delay = (real)g_510c54->field_2_3 * delay;
		__asm
		{
			fld delay
			fistp ticks
		}
		item->value_12e = (short)ticks;
	}
}

void function_b9a90(long object_index);
void __stdcall function_bef30(long object_index, long a, long b, long c, bool d);
void function_b8b70(long object_index);
void function_b7300(long object_index);
void __stdcall function_b87b0(long object_index);
void function_b7290(long object_index);
bool function_b9d20(long object_index);

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
	if ((item->object_flags >> 8) & 1)
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

// @retail 0x10b360
void function_10b360(long object_index)
{
	s_item *item = ITEM_GET(object_index);
	byte *definition = g_4e3b44[item->definition_index & 0xffff].bytes;
	if (item->parent_index == NONE && !TEST_FIELD_BIT(item->object_flag7) &&
		(definition[0xbc] & 1) && !(fabs(item->up.k - 1.0f) < 0.0001f))
	{
		vector3f *up = &item->up;
		*up = *g_4687b0;
		real side_i = item->forward.k * up->j - up->k * item->forward.j;
		real side_j = up->k * item->forward.i - item->forward.k * up->i;
		real side_k = up->i * item->forward.j - item->forward.i * up->j;
		vector3f *forward = &item->forward;
		real forward_k = up->j * side_i - up->i * side_j;
		real forward_j = up->i * side_k - up->k * side_i;
		real forward_i = up->k * side_j - up->j * side_k;
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

PRIVATE __forceinline void item_transform_plane(transform4x3f const *matrix,
	plane3f const *plane, plane3f *result)
{
	vector3f normal;
	normal.i = matrix->up.i * plane->k + matrix->left.i * plane->j + matrix->forward.i * plane->i;
	normal.j = matrix->up.j * plane->k + matrix->left.j * plane->j + matrix->forward.j * plane->i;
	normal.k = matrix->up.k * plane->k + matrix->left.k * plane->j + matrix->forward.k * plane->i;
	result->n = normal;
	result->d = matrix->position.z * normal.k + matrix->position.y * normal.j +
		matrix->position.x * normal.i + matrix->scale * plane->d;
}

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
	vector3f velocity;
	vector3f angular_velocity;
	s_object_marker marker;

	if (!TEST_FIELD_BIT(item->flag5) ||
		!(0.0001f > impulse->i * impulse->i + impulse->j * impulse->j + impulse->k * impulse->k))
	{
		if (TEST_FIELD_BIT(item->flag5) && function_b8d30(item_index, 0xc0000c1, &marker, 1, false))
		{
			plane3f plane;
			if (item->surface_index == NONE)
			{
				s_item_collision_data *collision = (s_item_collision_data *)g_4e0340;
				bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
			}
			else
			{
				s_item_collision_bsp *bsp = (s_item_collision_bsp *)g_4e0348;
				s_item_collision_instance *instance = &bsp->instances[item->surface_index];
				s_item_collision_data *collision = (s_item_collision_data *)(bsp->sections + instance->section_index * 0xc8 + 0x70);
				bsp3d_get_plane((s_bsp3d *)collision, collision->surfaces[item->material_index].plane_index, &plane);
				item_transform_plane(&instance->matrix, &plane, &plane);
			}
			real distance = marker.matrix.position.z * plane.k + marker.matrix.position.y * plane.j +
				plane.i * marker.matrix.position.x - plane.d;
			real offset = 0.05f - distance;
			point3f position;
			position.x = plane.i * offset + marker.matrix.position.x;
			position.y = plane.j * offset + marker.matrix.position.y;
			position.z = plane.k * offset + marker.matrix.position.z;
			function_b75a0(item_index, &position, NULL, NULL, NULL, false);
		}
		function_10c850(item_index);
	}

	if (item->ignore_object_index == NONE && (item->flags_c1 & 1) &&
		(TEST_FIELD_BIT(item->flag5) || TEST_FIELD_BIT(item->flag6)))
	{
		vector3f axis;
		if (function_b8d30(item_index, 0xc0000c1, &marker, 1, false))
			axis = marker.matrix.up;
		else
			axis = *g_4687b0;
		real speed = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, -1.5707964f, 1.5707964f);
		spin.i = axis.i * speed;
		spin.j = axis.j * speed;
		spin.k = axis.k * speed;
	}
	else
	{
		real speed = (real)sqrt(impulse->j * impulse->j + impulse->k * impulse->k + impulse->i * impulse->i);
		dword *seed = &g_4e7408->unknown0;
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
		real random_speed = (real)(*seed >> 16) * (1.0f / 65535.0f) * speed * 1.5707964f;
		spin.i *= random_speed;
		spin.j *= random_speed;
		spin.k *= random_speed;
	}
	velocity.i = item->linear_velocity.i + impulse->i;
	velocity.j = item->linear_velocity.j + impulse->j;
	velocity.k = item->linear_velocity.k + impulse->k;
	angular_velocity.i = spin.i + item->angular_velocity.i;
	angular_velocity.j = item->angular_velocity.j + spin.j;
	angular_velocity.k = item->angular_velocity.k + spin.k;
	function_b77d0(item_index, &velocity, &angular_velocity);
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
	s_item *item = ITEM_GET(item_index);
	s_item_impact_definition *definition = (s_item_impact_definition *)g_4e3b44[item->definition_index & 0xffff].bytes;
	vector3f direction = *velocity;
	real scale = function_1201a0(&direction, g_4687bc) * (1.0f / 3.0f);
	scale = scale < 0.0f ? 0.0f : (scale > 1.0f ? 1.0f : scale);
	long effect_index = definition->effect_index;
	if (effect_index != NONE)
	{
		long kind = definition->kind;
		if (impact->type == 4)
		{
			byte other_kind = ((s_item_impact_definition *)g_4e3b44[ITEM_GET(impact->object_index)->definition_index & 0xffff].bytes)->kind;
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


#include "slot_handler.h"
static __forceinline long real_to_long(real value);

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
    byte *header = objects->data + (object_index & 0xffff) * 12;
    s_item *item = *(s_item **)(header + 8);
    byte *item_flags = (byte *)item + 0x12c;
    struct s_node_flags { unsigned long : 29; unsigned long dirty : 1; unsigned long : 2; };
    s_node_flags volatile *node_flags = (s_node_flags volatile *)((byte *)item + 4);
    if (*owner_reference != NONE)
    {
        if (TEST_FIELD_BIT(node_flags->dirty) && object_index != NONE &&
            *(short *)((byte *)item + 0x112) != NONE)
        {
            if (TEST_FIELD_BIT(node_flags->dirty))
                function_bfa40(object_index, 0);
            item->object_flags &= ~0x20000000;
        }
        *item_flags |= 5;
        item->unit_index = *owner_reference;
        byte *current = *(byte **)(objects->data + (object_index & 0xffff) * 12 + 8);
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
            if ((*item_flags & 8) && header[3] == 2 && function_100f00(object_index) &&
                !(((byte *)item)[0x12d] & 1))
                add = false;
            function_bb950(object_index, add, real_to_long((real)g_510c54->field_2_3 * 30.0f));
        }
        s_item *current = *(s_item **)(objects->data + (object_index & 0xffff) * 12 + 8);
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
