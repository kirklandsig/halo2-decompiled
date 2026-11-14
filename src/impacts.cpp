// @flags /O2 /arch:SSE /Gr
/* IMPACTS.CPP: the havok contact impacts (sounds and effects played where
   two havok components touch): a data array of 0x20 impacts of 0xa0 bytes
   and one of 0x20 impact arrays (the impacts of one component) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "vector_products.h"
#include "unknown_1cec30.h"
#include "impacts.h"
#include "unknown_1428b0.h"
#include "object_queries.h"
#include "object_markers.h"
#include <math.h>
#include <string.h>

class c_class_227600
{
public:
	static bool function_227600(long impact_definition_index, c_type_47f957 material_a,
		c_type_47f957 material_b, c_type_47f957 impact_material_a,
		c_type_47f957 impact_material_b);
};

struct s_impact
{
	short salt;
	union
	{
		byte flags;
		struct
		{
			byte flag0 : 1;
			byte component_b_faster : 1;
			byte flags2 : 6;
		};
	};
	char sort_index;
	real priority;
	short reference_count;
	char age;
	char unknownb;
	char type;
	char unknownd;
	bool unknowne;
	byte unknownf;
	char unknown10;
	bool unknown11;
	byte unknown12[2];
	real unknown14;
	real unknown18;
	char unknown1c;
	char unknown1d;
	char unknown1e;
	char unknown1f;
	char unknown20;
	byte unknown21[3];
	long component_a;
	long component_b;
	c_type_47f957 material_a;
	c_type_47f957 material_b;
	long looping_sound_a;
	long looping_sound_b;
	long effect_a;
	long effect_b;
	real unknown40;
	real unknown44;
	real unknown48;
	real unknown4c;
	vector3f unknown50;
	vector3f normal;
	point3f position;
	point3f local_position_a;
	point3f local_position_b;
	real unknown8c;
	char unknown90;
	char unknown91;
	byte unknown92[2];
	long time;
	real unknown98;
	s_physics_model_shape_key shape;
};

/* the objects as the impacts see them */
struct s_impact_object
{
	long definition_index;
	byte unknown04[0x28 - 0x4];
	s_location location;
	byte unknown30[0x70 - 0x30];
	vector3f unknown70;
	vector3f unknown7c;
	vector3f velocity;
};

struct s_impact_object_header
{
	short salt;
	byte flags;
	byte type;
	byte unknown04[4];
	s_impact_object *object;
};

PRIVATE inline s_impact_object_header *impact_object_header_get(long object_index)
{
	return (s_impact_object_header *)g_4e0300->data + (object_index & 0xffff);
}

/* the impacts of one havok component */
struct s_impact_array
{
	short salt;
	short count;
	long impacts[15];
};

/* what a player is to the impacts: its local player index */
struct s_impact_player
{
	byte unknown00[0x28];
	short local_index;
};

/* the two data arrays (defined in unknown_03d380.cpp, whose game state
   callbacks clear them) */
extern s_record_pool *g_51ebfc;
extern s_record_pool *g_51ec00;
long g_502138;
long g_50213c[32];
/* retail startup initializer 378170 */
real g_55c2d0 = 0.52532196f;

struct s_small_index;
short function_0b67a0(const s_small_index *data);
point3f *function_b9dd0(long object_index, point3f *result);
real function_30bf0(vector3f *v);

/* the havok components (src/unknown_1cec30.cpp, src/unknown_0dc3a0.cpp) */
void havok_component_contact_properties_get(s_havok_component const *component, long contact_index, long *property_a, long *property_b);
void function_141590(transform4x3f const *in, transform4x3f *out);

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

PRIVATE inline long game_seconds_to_ticks_round(real seconds)
{
	real ticks = g_510c54->field_2_3 * seconds;
	long result;

	__asm
	{
		fld ticks
		fistp result
	}
	return result;
}

long function_146650(void);

PRIVATE inline s_player_state *local_player_state(long local_index)
{
	s_player_state *result = NULL;

	if (local_index != NONE && g_4686c4 != NONE)
	{
		s_player_4e9bd4 *player = &g_4e9bd4[local_index];
		result = &player->state;
	}
	return result;
}

/* the looping sounds (src/unknown_18a2f0.cpp) */
word *function_18a5e0(long datum_index);
long function_18a6c0(long tag_index, long value);
void function_18a720(long datum_index, bool set);

/* the sound and effect callees (src/unknown_188dd0.cpp, src/unknown_189010.cpp,
   src/unknown_187ec0.cpp, src/unknown_11bed0.cpp; the effects are stubs) */
struct s_sound_position
{
	point3f position;
	dword compressed_forward;
	vector3f velocity;
	s_location location;
};

struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

dword vector3d_compress(vector3f const *vector);
long function_189650(s_sound_position const *position, s_sound_label_play const *play);
void function_1883e0(long tag_index, bool ignore_distance, point3f const *point, c_type_47f957 element_index, c_type_47f957 entry_element_index, long index, long variant,
	long *first_value04, long *second_value04, long *first_value, long *second_value, long *first_value0c, long *second_value0c);
void function_11bed0(s_location *location, point3f const *point);
bool function_11c120(s_location const *location, point3f const *point, short *material);

/* where an impact's effect is attached: the contact point and normal */
struct s_impact_effect_location
{
	point3f position;
	vector3f normal;
	dword unknown18;
};

/* what creates an effect (0x175fa0) */
struct s_effect_new_data
{
	long unknown00;
	long definition_index;
	long unknown08;
	long unknown0c;
	long unknown10;
	short unknown14;
	byte unknown16[2];
	short unknown18;
	byte unknown1a[2];
	s_impact_effect_location const *location;
	long unknown20;
	byte unknown24[0x30 - 0x24];
	long unknown30;
	long unknown34;
	long unknown38;
	short unknown3c;
	byte unknown3e[2];
	real scale_a;
	real scale_b;
	byte unknown48[0x54 - 0x48];
	dword color_a;
	dword color_b;
	long unknown5c;
};

/* effects (unknown_175bd0.cpp, effects.h); s_effect_new_data is this file's
   view of its parameters */
struct s_effect_parameters;
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);
bool function_17b030(long effect_index, vector3f const *velocity, real scale_a, real scale_b, transform4x3f const *matrix, real const *values); /* unknown_175bd0.cpp */
void function_177260(long effect_index, bool unknown);

/* the havok component impact lists (src/unknown_1d5460.cpp,
   src/unknown_2055f0.cpp) and their shapes (src/unknown_1eb1b0.cpp) */
struct s_havok_impact_contact;
long havok_component_impact_find(s_havok_component *component, s_havok_impact_contact const *contact, bool check_position);
bool havok_component_impact_make_room(long rigid_body_index, s_havok_component *component, real strength);
bool havok_component_any_rigid_body_active(s_havok_component *component);
real havok_component_rigid_body_mass_get(long rigid_body_index, s_havok_component *component);
void havok_component_rigid_body_point_velocity_get(vector3f *velocity, long rigid_body_index, s_havok_component *component, point3f const *point);
void havok_component_rigid_body_linear_velocity_get(s_havok_component *component, long rigid_body_index, vector3f *velocity);
void havok_component_impact_add(s_havok_component *component, long impact_index);

/* a physics model's material: its two global materials and the shapes made
   of it */
struct s_physics_model_material_shape
{
	short type;
	short index;
	byte flags;
	byte unknown05[0xc - 0x5];
};

struct s_physics_model_material
{
	short material_a;
	short material_b;
	byte unknown04[4];
	long shape_count;
	s_physics_model_material_shape *shapes;
	byte unknown10[0x18 - 0x10];
};

/* the sort routines (unknown_13dcd0.cpp); the third parameter is never read.
   impact_sort_compare returns a long (retail sets all of eax) */
typedef bool (__stdcall *t_sort_4byte_compare_function)(long, long, const void *);
void sort_4byte(long *elements, unsigned long count, void *unused, t_sort_4byte_compare_function compare, const void *context);
void impact_type_update(long impact_index, long rigid_body_index_a, long rigid_body_index_b, s_impact *impact);
long impact_new(s_impact_data const *data, long type);
long __stdcall impact_sort_compare(long impact_index_a, long impact_index_b, void *context);

real function_1201a0(vector3f *v, vector3f const *fallback);

/* impacts.cpp */
void function_2266a0(long impact_index);
void function_2294a0(long impact_index, long rigid_body_index_a, long rigid_body_index_b, s_impact *impact);
void function_228770(long impact_index, s_impact *impact);
void function_226a60(long component_index);
void function_226f80(void);
void function_227280(void);
long function_2273d0(long unknown, long component_b, long component_a);
void impact_rigid_body_indices_get(s_impact const *impact, long impact_index, long *rigid_body_index_a, long *rigid_body_index_b);
void impact_local_positions_update(s_impact *impact, long rigid_body_index_a, long rigid_body_index_b);
void impact_set_peak(s_impact *impact, real value);
bool impact_has_sounds_or_effects(s_impact *impact);
real impact_distance_squared_to_nearest_player(point3f const *point, long type);
bool impact_component_b_is_faster(long component_a, long component_b);
bool impact_components_valid(long component_a, long component_b);
bool impact_matches_data(s_impact *impact, s_impact_data const *data, bool check_position);
void impact_orientation_get(s_impact const *impact, long impact_index, long rigid_body_index, vector3f *forward,
	vector3f *left, real *half_width, real *half_length, bool unknown, vector3f *up, point3f *position);

PRIVATE inline s_impact *impact_get(long impact_index)
{
	return &((s_impact *)g_51ebfc->data)[impact_index & 0xffff];
}

PRIVATE inline s_impact_array *impact_array_get(long array_index)
{
	return &((s_impact_array *)g_51ec00->data)[array_index & 0xffff];
}

PRIVATE inline byte *impact_tag_get(long tag_index)
{
	return g_4e3b44[tag_index & 0xffff].bytes;
}

/* the definition of a havok component's object */
PRIVATE inline byte *impact_component_definition_get(long component_index)
{
	return impact_tag_get(impact_object_header_get(havok_component_get(component_index)->object_index)->object->definition_index);
}

PRIVATE __forceinline bool impact_type_is_continuous(char type)
{
	return PIN(type, 2, 3) == type;
}

real g_47f05c = 0.0328f;
bool g_47f07a = true;

// @retail 0x2263c0
void function_2263c0(void)
{
	g_51ebfc = data_new_inlined("impacts", 0x20, sizeof(s_impact), 0, g_468758);
	g_51ec00 = data_new_inlined("impact arrarys", 0x20, sizeof(s_impact_array), 0, g_468758);
}

// @retail 0x226440
void function_226440(void)
{
	data_dispose(g_51ebfc);
	data_dispose(g_51ec00);
	g_51ebfc = NULL;
	g_51ec00 = NULL;
}

// @retail 0x227600
bool c_class_227600::function_227600(
	long impact_definition_index,
	c_type_47f957 material_a,
	c_type_47f957 material_b,
	c_type_47f957 impact_material_a,
	c_type_47f957 impact_material_b)
{
	if (material_a == impact_material_a && material_b == impact_material_b ||
		material_b == impact_material_a && material_a == impact_material_b)
	{
		return true;
	}
	return false;
}

// @retail 0x227350
long __stdcall impact_sort_compare(
	long impact_index_a,
	long impact_index_b,
	void *context)
{
	s_impact *impacts = (s_impact *)g_51ebfc->data;

	return impacts[impact_index_a & 0xffff].priority > impacts[impact_index_b & 0xffff].priority;
}

// @retail 0x227390
long impacts_last_sorted(void)
{
	long index = g_502138;
	long result = NONE;

	if (index != NONE)
	{
		result = g_50213c[index];
		while (index >= 0 && g_50213c[index] == NONE)
		{
			g_502138 = --index;
		}
	}
	return result;
}

// @retail 0x22a0e0
bool impact_has_sounds_or_effects(
	s_impact *impact)
{
	return impact->looping_sound_a != NONE || impact->looping_sound_b != NONE || impact->effect_a != NONE || impact->effect_b != NONE;
}

// @retail 0x229430
void impact_set_peak(
	s_impact *impact,
	real value)
{
	if (!impact->unknown11 || value > impact->unknown18)
	{
		if (impact->time == NONE ||
			function_146650() - impact->time > game_seconds_to_ticks_round(0.2f) ||
			value > impact->unknown98 * 1.3f)
		{
			impact->unknown11 = true;
			impact->unknown18 = value;
		}
	}
}

// @retail 0x22a560
real impact_distance_squared_to_nearest_player(
	point3f const *point,
	long type)
{
	real result = 25000000.0f;
	s_record_pool_iterator iterator;
	s_impact_player *player;

	iterator.data = g_4e8c24;
	iterator.index = NONE;
	while ((player = (s_impact_player *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (player->local_index != NONE)
		{
			s_player_state *state = local_player_state(player->local_index);
			vector3f delta;
			real distance;

			vector3d_from_points3d(point, &state->position, &delta);
			switch (type)
			{
			case 0:
				distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				break;
			case 1:
				distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				break;
			case 2:
				distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				break;
			case 3:
				distance = delta.j * delta.j + delta.i * delta.i + delta.k * delta.k;
				break;
			default:
				__assume(0);
			}
			result = MIN(distance, result);
		}
	}
	return result;
}

// @retail 0x2283e0
bool impact_component_b_is_faster(
	long component_a,
	long component_b)
{
	bool result = false;

	if (component_b != NONE)
	{
		s_impact_object *object_b = impact_object_header_get(havok_component_get(component_b)->object_index)->object;
		s_impact_object *object_a = impact_object_header_get(havok_component_get(component_a)->object_index)->object;

		result = length_sq3f(&object_b->velocity) > length_sq3f(&object_a->velocity);
	}
	return result;
}

PRIVATE inline bool impact_component_is_biped(long component_index)
{
	return component_index != NONE &&
		((1 << impact_object_header_get(havok_component_get(component_index)->object_index)->type) & 1);
}

// @retail 0x2276d0
bool impact_components_valid(
	long component_a,
	long component_b)
{
	bool a_is_biped = impact_component_is_biped(component_a);
	bool b_is_biped = impact_component_is_biped(component_b);
	bool a_valid = true;
	bool b_valid = true;

	if (a_is_biped)
		a_valid = TEST_FIELD_BIT(havok_component_get(component_a)->flag11);
	if (b_is_biped)
		b_valid = TEST_FIELD_BIT(havok_component_get(component_b)->flag11);
	return a_valid && b_valid;
}

// @retail 0x22a440
void impact_set_contact(
	s_impact *impact,
	s_impact_data const *data,
	vector3f const *vector,
	real unknown44,
	bool unknownf)
{
	impact->normal = data->normal;
	impact->position = data->position;
	impact->unknown40 = 0.0f;
	impact->unknown44 = unknown44;
	impact->unknown50 = *vector;
	impact->unknownf = unknownf;
}

// @retail 0x22a4b0
void impact_set_contact_from_component(
	s_impact *impact)
{
	s_havok_component *component = havok_component_get(impact->component_a);
	s_impact_object *object = impact_object_header_get(component->object_index)->object;

	impact->normal = object->unknown7c;
	function_b9dd0(component->object_index, &impact->position);
	impact->unknown40 = 0.0f;
	impact->unknown44 = 0.0f;
	impact->unknown50 = object->velocity;
	if (function_30bf0(&impact->unknown50) == g_45dbd8)
		impact->unknown50 = object->unknown70;
}

// @retail 0x227640
bool impacts_match(
	long impact_component_a,
	long impact_component_b,
	long component_a,
	long component_b,
	c_type_47f957 impact_material_a,
	c_type_47f957 impact_material_b,
	c_type_47f957 material_a,
	c_type_47f957 material_b,
	long impact_type,
	long type,
	bool impact_flag,
	bool flag,
	long impact_unknown,
	long unknown)
{
	if (impact_flag)
	{
		if (flag)
		{
			if (impact_material_a == material_a && impact_material_b == material_b ||
				impact_material_b == material_a && impact_material_a == material_b)
				goto local_0;
		}
	}
	else if (!flag)
	{
		if ((impact_component_a == component_a && impact_component_b == component_b ||
			impact_component_a == component_b && impact_component_b == component_a) &&
			c_class_227600::function_227600(NONE, impact_material_a, impact_material_b, material_a, material_b) &&
			impact_unknown == unknown &&
			impact_type == type)
			goto local_0;
	}
	return false;
local_0:
	return true;
}

PRIVATE __forceinline real function_2274f1(vector3f const *arg_0)
{
	real local_0 = arg_0->i * arg_0->i;
	local_0 += arg_0->j * arg_0->j;
	local_0 += arg_0->k * arg_0->k;
	return local_0;
}

// @retail 0x2274f0
bool impact_matches_data(
	s_impact *impact,
	s_impact_data const *data,
	bool check_position)
{
	unsigned long local_0 = *(volatile long *)&data->component_b;

	if (impacts_match(impact->component_a, impact->component_b, data->component_a, local_0,
		impact->material_a, impact->material_b, data->material_a, data->material_b,
		impact->unknownd, data->type, impact->unknowne, data->unknown38, impact->shape.type, data->shape.type))
	{
		if (check_position)
		{
			vector3f delta;
			real distance_squared;

			vector3d_from_points3d(&impact->position, &data->position, &delta);
			distance_squared = function_2274f1(&delta);
			if (!(distance_squared < 0.25f) &&
				(!(distance_squared < 16.0f) || !(dot3f(&impact->normal, &data->normal) > g_55c2d0)))
			{
				return false;
			}
		}
		if (impact_components_valid(data->component_a, local_0))
			return true;
	}
	return false;
}

PRIVATE __forceinline real function_227811(vector3f const *arg_0, vector3f const *arg_1)
{
	real local_0 = arg_0->k * arg_1->k;
	local_0 += arg_0->j * arg_1->j;
	local_0 += arg_0->i * arg_1->i;
	return local_0;
}

PRIVATE __forceinline void function_227812(vector3f const *arg_0, vector3f *arg_1)
{
	real local_0 = function_227811(arg_0, arg_1);
	vector3f local_1;
	local_1.i = local_0;
	local_1.i *= arg_0->i;
	local_1.j = arg_0->j * local_0;
	local_1.k = arg_0->k * local_0;
	arg_1->i -= local_1.i;
	arg_1->j -= local_1.j;
	arg_1->k -= local_1.k;
}

PRIVATE __forceinline vector3f const *function_227813(s_impact_object *arg_0, vector3f const *arg_1)
{
	vector3f const *local_1 = &arg_0->unknown70;
	real local_0 = local_1->k * arg_1->k;
	local_0 += arg_0->unknown70.j * arg_1->j;
	local_0 += arg_0->unknown70.i * arg_1->i;
	if (!(local_0 >= 0.0f))
		local_0 = -local_0;
	if (!(local_0 < 0.9f))
		local_1 = &arg_0->unknown7c;
	return local_1;
}

PRIVATE __forceinline void function_227814(vector3f const *arg_0, vector3f const *arg_1, vector3f *arg_2)
{
	real local_0 = arg_0->i * arg_1->j - arg_0->j * arg_1->i;
	real local_1 = arg_1->i * arg_0->k - arg_0->i * arg_1->k;
	real local_2 = arg_0->j * arg_1->k - arg_0->k * arg_1->j;
	arg_2->i = local_2;
	arg_2->j = local_1;
	arg_2->k = local_0;
}

// score 5 (371 bytes against retail's 370): the only real difference is in
// the inlined 0x227812, where `local_1.i = local_0; local_1.i *= arg_0->i;`
// compiles to `movss xmm1, [edi]; mulss xmm1, xmm0` and retail has
// `movaps xmm1, xmm0; mulss xmm1, [edi]` (one byte shorter; the other reported
// differences are the CALL alignment that follows the extra byte). One-line
// multiplication, a named factor and the commutative order kept or worsened
// the form. Permuter: 174 tries in 300 s (seed 999), no lower form.
// @retail 0x227810
void impact_build_matrix(
	long component_index,
	s_impact const *impact,
	matrix3x3 *matrix)
{
	s_impact_object *object = impact_object_header_get(havok_component_get(component_index)->object_index)->object;

	matrix->up = impact->normal;
	vector3f const *forward = function_227813(object, &impact->normal);
	matrix->forward = *forward;
	function_227812(&matrix->up, &matrix->forward);
	function_30bf0(&matrix->forward);
	function_227814(&matrix->up, &matrix->forward, &matrix->left);
	function_30bf0(&matrix->left);
}

// @retail 0x2273d0
long function_2273d0(
	long unknown,
	long component_b,
	long component_a)
{
	long result = 2;

	if (unknown == NONE &&
		((1 << impact_object_header_get(havok_component_get(component_a)->object_index)->type) & 0x1001))
	{
		result = 0;
	}
	if (component_a != NONE && component_b != NONE)
	{
		s_havok_component *havok_component_a = havok_component_get(component_a);
		s_havok_component *havok_component_b = havok_component_get(component_b);
		long rigid_body_index_a = havok_component_main_rigid_body_index_get(havok_component_a);
		long rigid_body_index_b = havok_component_main_rigid_body_index_get(havok_component_b);

		if (rigid_body_index_a != NONE && rigid_body_index_b != NONE &&
			!havok_component_rigid_body_get(rigid_body_index_a, havok_component_a)->m_fixed &&
			!havok_component_a->rigid_body_keyframed(rigid_body_index_a) &&
			!havok_component_rigid_body_get(rigid_body_index_b, havok_component_b)->m_fixed &&
			!havok_component_b->rigid_body_keyframed(rigid_body_index_b))
		{
			result = 1;
		}
	}
	return result;
}

// @retail 0x227990
void impact_rigid_body_indices_get(
	s_impact const *impact,
	long impact_index,
	long *rigid_body_index_a,
	long *rigid_body_index_b)
{
	s_havok_component *component = havok_component_get(impact->component_a);
	long i;

	if (impact->shape.type != NONE)
	{
		long contact_index = NONE;

		for (i = 0; i < component->unknown7c.size; i++)
		{
			s_havok_component_element0c *contact = &component->unknown7c.data[i];

			if (contact->unknown00 == impact->shape.type && contact->unknown02 == (*(s_impact const *const volatile *)&impact)->shape.index)
			{
				contact_index = i;
			}
		}
		havok_component_contact_properties_get(component, contact_index, rigid_body_index_a, rigid_body_index_b);
	}
	else if (impact->unknownd != NONE)
	{
		*rigid_body_index_a = havok_component_main_rigid_body_index_get(component);
		*rigid_body_index_b = NONE;
	}
	else
	{
		*rigid_body_index_a = NONE;
		*rigid_body_index_b = NONE;
		for (i = 0; i < component->unknown88.size; i++)
		{
			s_havok_component_element48 *constraint = &component->unknown88.data[i];

			if (constraint->impact_index == impact_index)
			{
				if (*rigid_body_index_a == NONE || constraint->rigid_body_index_a < *rigid_body_index_a)
				{
					*rigid_body_index_a = constraint->rigid_body_index_a;
				}
				if (constraint->component_b != NONE &&
					(*rigid_body_index_b == NONE || constraint->rigid_body_index_b < *rigid_body_index_b))
				{
					*rigid_body_index_b = constraint->rigid_body_index_b;
				}
			}
		}
	}
}

PRIVATE __forceinline void function_2281a1(transform4x3f const *arg_0, point3f const *arg_1, point3f *arg_2)
{
	real local_0 = arg_1->x;
	real local_1 = arg_1->y;
	real local_2 = arg_1->z;
	if (arg_0->scale != 1.f)
	{
		local_0 = arg_0->scale * local_0;
		local_1 = arg_0->scale * local_1;
		local_2 = arg_0->scale * local_2;
	}
	arg_2->x = arg_0->up.i * local_2 + arg_0->left.i * local_1 + arg_0->forward.i * local_0 + arg_0->position.x;
	real local_3 = arg_0->up.j * local_2;
	local_3 += arg_0->left.j * local_1;
	local_3 += arg_0->forward.j * local_0;
	arg_2->y = local_3 + arg_0->position.y;
	arg_2->z = arg_0->up.k * local_2 + arg_0->left.k * local_1 + arg_0->forward.k * local_0 + arg_0->position.z;
}

// @retail 0x2281a0
void impact_local_positions_update(
	s_impact *impact,
	long rigid_body_index_a,
	long rigid_body_index_b)
{
	transform4x3f matrix;
	transform4x3f inverse;

	havok_component_get(impact->component_a)->rigid_body_matrix_get(rigid_body_index_a, &matrix);
	if (impact->shape.type != NONE)
	{
		impact->position = matrix.position;
	}
	else
	{
		function_141590(&matrix, &inverse);
		function_2281a1(&inverse, &impact->position, &impact->local_position_a);
		if (impact->component_b != NONE)
		{
			havok_component_get(impact->component_b)->rigid_body_matrix_get(rigid_body_index_b, &matrix);
			function_141590(&matrix, &inverse);
			function_2281a1(&inverse, &impact->position, &impact->local_position_b);
		}
	}
}

struct s_227af0
{
	real field_0;
	real field_4;
	real field_8;
	real field_c;
	real field_10;
	real field_14;
	real field_18;
	real field_1c;
};

// score 4 (was 4): no logic difference found. The only differing instructions are the operand order of two
// `movss`/`mulss` pairs in the inlined cross product of the second call (g_4687a8) and two in the third call
// (forward x normal): retail's second call has the same order as its first, ours swaps one pair; retail's third call
// has `a.i * b.j` first and `b.i * a.j` second, ours swaps both. The order follows the pointer variables' identity, not
// the source order of the products. Tried: using `up` instead of `normal` in the cross products, every combination of
// the three calls (6-8); a fresh alias for the second or third call's operand (6-16); swapping each product's operands
// inside function_227814 (no change); `normal` as a non-const pointer or declared at the top (4).
// Permuter, 35 minutes (11-05): nothing below 4.
// @retail 0x227af0
void impact_orientation_get(
	s_impact const *impact,
	long impact_index,
	long rigid_body_index,
	vector3f *forward,
	vector3f *left,
	real *half_width,
	real *half_length,
	bool unknown,
	vector3f *up,
	point3f *position)
{
	*up = impact->normal;
	*position = impact->position;
	if (impact->shape.type != NONE)
	{
		transform4x3f matrix;

		havok_component_get(impact->component_a)->rigid_body_matrix_get(rigid_body_index, &matrix);
		*position = matrix.position;
		*forward = matrix.forward;
		*up = matrix.up;
		*left = matrix.left;
		*half_width = 0.0f;
		*half_length = 0.0f;
	}
	else if (impact->reference_count <= 1 && !(impact->unknowne && unknown))
	{
		*half_width = 0.0f;
		*half_length = 0.0f;
		vector3f const *normal = up;
		function_227814(normal, g_4687b0, forward);
		if (function_30bf0(forward) < 0.001f)
		{
			function_227814(normal, g_4687a8, forward);
			function_30bf0(forward);
		}
		function_227814(normal, forward, left);
		function_30bf0(left);
	}
	else
	{
		s_havok_component *component = havok_component_get(impact->component_a);
		bool has_component_b = impact->component_b != NONE;
		s_227af0 local_1;
		local_1.field_10 = 3.4028235e+38f;
		local_1.field_0 = -3.4028235e+38f;
		local_1.field_14 = 3.4028235e+38f;
		local_1.field_c = -3.4028235e+38f;
		local_1.field_4 = 3.4028235e+38f;
		local_1.field_1c = -3.4028235e+38f;
		local_1.field_8 = 3.4028235e+38f;
		local_1.field_18 = -3.4028235e+38f;
		matrix3x3 matrix_a;
		matrix3x3 matrix_b;
		long i;

		impact_build_matrix(impact->component_a, impact, &matrix_a);
		if (has_component_b)
		{
			impact_build_matrix(impact->component_b, impact, &matrix_b);
		}
		for (i = 0; i < component->unknown88.size; i++)
		{
			s_havok_component_element48 *constraint = &component->unknown88.data[i];

			if (constraint->impact_index == impact_index)
			{
				vector3f delta;
				real x;
				real y;

				point3f const *point = &constraint->position;
				delta.i = point->x - position->x;
				delta.j = point->y - position->y;
				delta.k = point->z - position->z;

				x = dot3f(&matrix_a.forward, &delta);
				y = dot3f(&matrix_a.left, &delta);
				if (!(local_1.field_0 > x))
					local_1.field_0 = x;
				if (local_1.field_10 > x)
					local_1.field_10 = x;
				if (!(local_1.field_c > y))
					local_1.field_c = y;
				if (local_1.field_14 > y)
					local_1.field_14 = y;
				if (has_component_b)
				{
					x = matrix_b.up.i * delta.k + matrix_b.left.i * delta.j + matrix_b.forward.i * delta.i;
					y = matrix_b.up.j * delta.k + matrix_b.left.j * delta.j + matrix_b.forward.j * delta.i;
					if (!(local_1.field_1c > x))
						local_1.field_1c = x;
					if (local_1.field_4 > x)
						local_1.field_4 = x;
					if (!(local_1.field_18 > y))
						local_1.field_18 = y;
					if (local_1.field_8 > y)
						local_1.field_8 = y;
				}
			}
		}
		if (has_component_b && !((local_1.field_c - local_1.field_14) * (local_1.field_0 - local_1.field_10) > (local_1.field_1c - local_1.field_4) * (local_1.field_18 - local_1.field_8)))
		{
			real x_center;
			real y_center;

			*half_width = (local_1.field_1c - local_1.field_4) * 0.5f;
			*half_length = (local_1.field_18 - local_1.field_8) * 0.5f;
			*forward = matrix_b.forward;
			*left = matrix_b.left;
			x_center = (local_1.field_1c + local_1.field_4) * 0.5f;
			position->x += forward->i * x_center;
			position->y += forward->j * x_center;
			position->z += forward->k * x_center;
			y_center = (local_1.field_18 + local_1.field_8) * 0.5f;
			position->x += left->i * y_center;
			position->y += left->j * y_center;
			position->z += left->k * y_center;
		}
		else
		{
			real x_center;
			real y_center;

			*half_width = (local_1.field_0 - local_1.field_10) * 0.5f;
			*half_length = (local_1.field_c - local_1.field_14) * 0.5f;
			*forward = matrix_a.forward;
			*left = matrix_a.left;
			x_center = (local_1.field_0 + local_1.field_10) * 0.5f;
			position->x += forward->i * x_center;
			position->y += forward->j * x_center;
			position->z += forward->k * x_center;
			y_center = (local_1.field_c + local_1.field_14) * 0.5f;
			position->x += left->i * y_center;
			position->y += left->j * y_center;
			position->z += left->k * y_center;
		}
	}
}


// score 241: walked against retail statement by statement (11-05): no logic difference (both switches, every branch,
// constant and field; the pool constants at 0x45e294 and 0x45e298 are 1/3.95 and 2.4674013, as in the source). Not kept,
// though closer to retail's structure (opcode alignment 0.84 -> 0.95): the angular velocity read and subtracted through a
// `vector3f const *` (retail's dead lea/add after the b-subtract), one shared velocity local, the 8.0f speed ratio as an
// initializer and the material compare as `other->material_a == impact->material_a` together score 261 (fast). With them
// what differs is registers: retail keeps 1.0f in xmm4 (it is also rotation_threshold) and minimum_impulse in xmm6, ours
// uses xmm0/xmm4/xmm5 and keeps 0.5f live into the third branch; retail scales rigid_body_index_a in place in ebp; the
// material compare is `cmp [esi+0x2c], cx` in retail; unknown8c is loaded before the velocity copy; frame 0x1c vs 0x18.
// Also tried on that form: unknown91 as one store (183, but rigid_body_index_b then travels in eax and the function
// returns with ret 8 where retail has ret 0xc, which would change what 0x228770 sees); `rotation_threshold = 1.0f; if
// (physics_type) ...` before or after the switches (327-352); rotation_threshold set inside the min switch (352);
// declaration order (no change).
// Permuter, 35 minutes (11-05): full image 180 only with volatile locals and casts that change no value. Its plain
// parts (rotation_threshold declared after physics_type, component->unknown20 read at each use, the sliding/rolling
// else-if chain inverted, speed_ratio declared first, velocity k stored before j) give 239 fast at best (k before j).
// @retail 0x228f30
void impact_type_update(
	long impact_index,
	long rigid_body_index_a,
	long rigid_body_index_b,
	s_impact *impact)
{
	s_havok_component *component = havok_component_get(impact->component_a);
	long physics_type = function_2273d0(impact->unknownd, impact->component_b, impact->component_a);
	real maximum_impulse;
	real minimum_impulse;
	real rotation_threshold;
	real strength;
	vector3f angular_velocity;

	switch (physics_type)
	{
	case 0:
		maximum_impulse = 5.0f;
		break;
	case 1:
		maximum_impulse = 0.5f;
		break;
	default:
		maximum_impulse = 2.0f;
		break;
	}
	switch (physics_type)
	{
	case 0:
		minimum_impulse = 1.0f;
		break;
	case 1:
		minimum_impulse = 0.05f;
		break;
	default:
		minimum_impulse = 0.3f;
		break;
	}
	rotation_threshold = physics_type ? 0.5f : 1.0f;

	strength = 0.0f;
	angular_velocity = component->rigid_bodies.data[rigid_body_index_a].angular_velocity;
	if (impact->component_b != NONE)
	{
		s_havok_component_rigid_body const *rigid_body_b = &havok_component_get(impact->component_b)->rigid_bodies.data[rigid_body_index_b];

		angular_velocity.i -= rigid_body_b->angular_velocity.i;
		angular_velocity.j -= rigid_body_b->angular_velocity.j;
		angular_velocity.k -= rigid_body_b->angular_velocity.k;
	}
	impact->type = NONE;
	if ((impact->unknown8c > rotation_threshold * rotation_threshold ||
		length_sq3f(&angular_velocity) > 2.4674013f) &&
		impact->unknown40 > minimum_impulse)
	{
		impact->type = 0;
		strength = (MIN(impact->unknown40, maximum_impulse) - minimum_impulse) / (maximum_impulse - minimum_impulse);
	}
	else if (impact->unknowne)
	{
		long list_index = component->unknown20;
		long sliding_count = 0;
		long rolling_count = 0;
		real sliding_total = 0.0f;
		real rolling_total = 0.0f;
		long i;

		for (i = 0; i < (list_index == NONE ? 0 : ((s_impact_array *)g_51ec00->data)[list_index & 0xffff].count); i++)
		{
			long other_index = ((s_impact_array *)g_51ec00->data)[list_index & 0xffff].impacts[i];

			if (other_index != impact_index)
			{
				s_impact *other = &((s_impact *)g_51ebfc->data)[other_index & 0xffff];

				if (!other->unknowne &&
					impact->material_a == other->material_a &&
					impact->material_b == other->material_b &&
					other->unknownd != NONE)
				{
					switch (other->type)
					{
					case 2:
						sliding_count++;
						sliding_total += other->unknown4c;
						break;
					case 3:
						rolling_count++;
						rolling_total += other->unknown48;
						break;
					}
				}
			}
		}
		if (rolling_count > 0 && rolling_count >= sliding_count)
		{
			impact->type = 3;
			impact->unknown48 = rolling_total / rolling_count;
		}
		else if (sliding_count > 0)
		{
			impact->type = 2;
			impact->unknown4c = sliding_total / sliding_count;
		}
		else
		{
			impact->type = 1;
		}
	}
	else
	{
		s_havok_component *component_a = havok_component_get(impact->component_a);
		real maximum_speed = 7.0f;
		real speed_ratio;
		hkRigidBody *rigid_body;
		vector3f velocity;
		real speed;

		if (impact->unknownd == NONE)
		{
			maximum_speed = 0.5f;
			speed_ratio = 1.25f;
		}
		else
		{
			speed_ratio = 8.0f;
		}
		rigid_body = component_a->rigid_bodies.data[rigid_body_index_a].rigid_body;
		if (!rigid_body->m_fixed)
		{
			point3f const *local_1 = (point3f const *)&rigid_body->m_motion->m_linear_velocity;
			velocity.i = local_1->x;
			velocity.j = local_1->y;
			velocity.k = local_1->z;
		}
		else
		{
			velocity = *g_4687a4;
		}
		if (impact->component_b != NONE)
		{
			vector3f velocity_b;

			havok_component_rigid_body_linear_velocity_get(havok_component_get(impact->component_b), rigid_body_index_b, &velocity_b);
			velocity.i -= velocity_b.i;
			velocity.j -= velocity_b.j;
			velocity.k -= velocity_b.k;
		}
		speed = (real)sqrt(length_sq3f(&velocity));
		if (speed > 0.05f && !impact->unknownf &&
			(impact->unknown44 < 0.001f || speed / impact->unknown44 > speed_ratio))
		{
			impact->type = 2;
			impact->unknown4c = (MIN(speed, maximum_speed) - 0.05f) / (maximum_speed - 0.05f);
		}
		else if (impact->unknown44 > 0.05f)
		{
			impact->type = 3;
			impact->unknown48 = (MIN(impact->unknown44, 4.0f) - 0.05f) / (4.0f - 0.05f);
		}
		else
		{
			impact->type = 1;
		}
	}
	if (impact->type == 0)
	{
		if (!impact->unknown10)
		{
			impact->unknown14 = 0.0f;
		}
		impact->unknown14 = MAX(impact->unknown14, strength);
		if (impact->unknown10 == 1)
		{
			impact_set_peak(impact, impact->unknown14);
		}
		impact->unknown10 = MIN(impact->unknown10 + 1, 1);
	}
	else
	{
		if (impact->unknown10)
		{
			impact_set_peak(impact, impact->unknown14);
		}
		impact->unknown10 = 0;
	}
	if (PIN(impact->type, 2, 3) == impact->type)
	{
		impact->unknown91 = MIN(impact->unknown91 + 1, 0x7e);
	}
	else
	{
		impact->unknown91 = 0;
	}
}

// @retail 0x22a060
void impact_sounds_stop(
	s_impact *impact,
	bool stop_looping_sounds,
	bool stop_effects)
{
	if (stop_looping_sounds)
	{
		if (impact->looping_sound_a != NONE)
		{
			function_18a5e0(impact->looping_sound_a);
			impact->looping_sound_a = NONE;
		}
		if (impact->looping_sound_b != NONE)
		{
			function_18a5e0(impact->looping_sound_b);
			impact->looping_sound_b = NONE;
		}
	}
	if (stop_effects)
	{
		if (impact->effect_a != NONE)
		{
			function_177260(impact->effect_a, true);
			impact->effect_a = NONE;
		}
		if (impact->effect_b != NONE)
		{
			function_177260(impact->effect_b, true);
			impact->effect_b = NONE;
		}
	}
}

// @retail 0x2277b0
void impact_release(
	long impact_index,
	s_impact *impact)
{
	if (impact->reference_count == 1 && PIN(impact->unknown10, 1, 1) == impact->unknown10)
	{
		long rigid_body_index_a;
		long rigid_body_index_b;

		impact_rigid_body_indices_get(impact, impact_index, &rigid_body_index_a, &rigid_body_index_b);
		impact_set_peak(impact, impact->unknown14);
		function_2294a0(impact_index, rigid_body_index_a, rigid_body_index_b, impact);
	}
	impact->reference_count--;
}

// @retail 0x2266a0
void function_2266a0(
	long impact_index)
{
	s_impact *impact = impact_get(impact_index);
	s_havok_component *component = havok_component_get(impact->component_a);
	s_impact_array *impacts;

	if (impact->sort_index != NONE)
	{
		g_50213c[impact->sort_index] = NONE;
	}
	impact_sounds_stop(impact, true, true);
	if (impact->reference_count != 0)
	{
		if (impact->unknownd != NONE)
		{
			impact_release(impact_index, impact);
		}
		else
		{
			for (long i = 0; i < component->unknown88.size; i++)
			{
				s_havok_component_element48 *constraint = &component->unknown88.data[i];

				if (constraint->impact_index == impact_index)
				{
					impact_release(impact_index, impact);
					constraint->impact_index = NONE;
				}
			}
			for (long i = 0; i < component->unknown7c.size; i++)
			{
				s_havok_component_element0c *contact = &component->unknown7c.data[i];

				if (contact->impact_index == impact_index)
				{
					impact_release(impact_index, impact);
					component->unknown7c.data[i].impact_index = NONE;
				}
			}
		}
	}
	impacts = impact_array_get(component->unknown20);
	for (long i = 0; i < impacts->count; i++)
	{
		if (impacts->impacts[i] == impact_index)
		{
			impacts->impacts[i] = impacts->impacts[impacts->count - 1];
			impacts->count--;
			break;
		}
	}
	if (impact_array_get(component->unknown20)->count == 0)
	{
		record_pool_release(g_51ec00, component->unknown20);
		component->unknown20 = NONE;
	}
	record_pool_release(g_51ebfc, impact_index);
}

// @retail 0x2264a0
long impact_new(
	s_impact_data const *data,
	long type)
{
	long impact_index = record_pool_allocate(g_51ebfc);
	byte *definition_a;
	s_impact *impact = impact_get(impact_index);

	definition_a = impact_component_definition_get(data->component_a);

	if (data->component_b != NONE)
	{
		byte *definition_b = impact_component_definition_get(data->component_b);

		impact->unknownb = MIN(definition_a[0x1a], definition_b[0x1a]);
	}
	else
	{
		impact->unknownb = definition_a[0x1a];
	}
	impact->flags = 0;
	impact->priority = impact_distance_squared_to_nearest_player(&data->position, type);
	impact->type = (char)type;
	impact->reference_count = 0;
	impact->age = 0;
	impact->component_a = data->component_a;
	impact->component_b = data->component_b;
	impact->unknownd = (char)data->type;
	impact->unknowne = data->unknown38;
	impact->unknownf = false;
	impact->material_a = data->material_a;
	impact->material_b = data->material_b;
	impact->unknown1c = 0x7f;
	impact->unknown1e = 0x7f;
	impact->unknown20 = 0x7f;
	impact->looping_sound_a = NONE;
	impact->looping_sound_b = NONE;
	impact->effect_a = NONE;
	impact->effect_b = NONE;
	impact->unknown1d = NONE;
	impact->unknown10 = 0;
	impact->unknown11 = false;
	impact->unknown14 = 0.0f;
	impact->unknown18 = 0.0f;
	impact->unknown1f = NONE;
	impact->normal = data->normal;
	function_1201a0(&impact->normal, g_4687b0);
	dword const *local_0 = (dword const *)&data->position;
	*(dword *)&impact->position.x = local_0[0];
	*(dword *)&impact->position.y = local_0[1];
	*(dword *)&impact->position.z = local_0[2];
	impact->unknown48 = 0.0f;
	impact->unknown4c = 0.0f;
	impact->sort_index = NONE;
	impact->unknown8c = 0.0f;
	impact->unknown90 = 0;
	impact->unknown91 = 0;
	impact->shape = data->shape;
	impact->time = NONE;
	impact->unknown98 = 0.0f;
	if (impact_component_b_is_faster(data->component_a, data->component_b))
	{
		impact->flags |= 2;
	}
	else
	{
		impact->flags &= ~2;
	}
	if (data->type == NONE)
	{
		impact->reference_count++;
		impact_local_positions_update(impact, data->unknown08, data->unknown14);
		impact_release(impact_index, impact);
	}
	return impact_index;
}

// @retail 0x2285d0
void impact_material_effects_get_for_component(
	vector3f const *normal,
	long component_index,
	long unknownd,
	long unknownb,
	bool has_component_b,
	point3f const *position,
	c_type_47f957 material_a,
	c_type_47f957 material_b,
	long type,
	long *first_value04,
	long *second_value04,
	long *first_value,
	long *second_value,
	long *first_value0c,
	long *second_value0c)
{
	point3f point = *position;
	long tag_index = NONE;
	short material = NONE;
	s_location location;
	long index;

	if (!has_component_b)
	{
		point.x += normal->i * g_47f05c;
		point.y += normal->j * g_47f05c;
		point.z += normal->k * g_47f05c;
	}
	function_11bed0(&location, &point);
	if (location.cluster_index == NONE)
	{
		location = impact_object_header_get(havok_component_get(component_index)->object_index)->object->location;
	}
	if (function_11c120(&location, &point, &material))
	{
		material_b.m_index = material;
	}
	if (component_index != NONE && unknownd == NONE)
	{
		tag_index = *(long *)(impact_component_definition_get(component_index) + 0x58);
	}
	switch (type)
	{
	case 0:
		index = unknownb + 10;
		break;
	case 2:
		index = 14;
		break;
	default:
		index = 13;
		break;
	}
	function_1883e0(tag_index, false, position, material_a, material_b, index, unknownb,
		first_value04, second_value04, first_value, second_value, first_value0c, second_value0c);
}

PRIVATE __forceinline byte *function_2284c1(long arg_0)
{
	return impact_tag_get(impact_object_header_get(havok_component_get(arg_0)->object_index)->object->definition_index);
}

// @retail 0x2284c0
void impact_material_effects_get(
	s_impact const *impact,
	point3f const *position,
	c_type_47f957 material_a,
	c_type_47f957 material_b,
	long type,
	long *first_value04,
	long *second_value04,
	long *first_value,
	long *second_value,
	long *first_value0c,
	long *second_value0c)
{
	long component_index;
	c_type_47f957 first_material;
	c_type_47f957 second_material;

	if (impact->component_b != NONE &&
		*(long *)(function_2284c1(impact->component_a) + 0x58) == NONE &&
		*(long *)(function_2284c1(impact->component_b) + 0x58) != NONE)
	{
		first_material = material_a;
		second_material = material_b;
		component_index = impact->component_b;
	}
	else
	{
		first_material = material_b;
		second_material = material_a;
		component_index = impact->component_a;
	}
	impact_material_effects_get_for_component(&impact->normal, component_index, impact->unknownd, impact->unknownb, impact->component_b != NONE,
		position, second_material, first_material, type,
		first_value04, second_value04, first_value, second_value, first_value0c, second_value0c);
}

PRIVATE __forceinline real impact_level_get(s_impact const *impact, char type)
{
	real level;

	if (type == 3)
	{
		level = PIN(impact->unknown48, 0.0f, 1.0f);
	}
	else
	{
		level = PIN(impact->unknown4c, 0.0f, 1.0f);
	}
	return level;
}

PRIVATE inline void impact_effect_data_new(s_effect_new_data *data, long definition_index, s_impact_effect_location const *location, real scale)
{
	memset(data, 0, sizeof(*data));
	data->color_a = 0xff808080;
	data->color_b = 0xff808080;
	data->unknown34 = 0;
	data->unknown38 = 0;
	data->unknown3c = 0;
	data->unknown30 = 0;
	data->unknown5c = 0;
	data->unknown00 = 0;
	data->unknown18 = NONE;
	data->unknown08 = NONE;
	data->unknown10 = NONE;
	data->unknown0c = NONE;
	data->unknown14 = NONE;
	data->definition_index = definition_index;
	data->unknown20 = 1;
	data->location = location;
	data->scale_a = scale;
	data->scale_b = scale;
}

PRIVATE __forceinline void function_2294a1(s_effect_new_data *data, long definition_index, s_impact_effect_location const *location, real scale)
{
	memset(data, 0, sizeof(*data));
	data->color_a = 0xff808080;
	data->color_b = 0xff808080;
	data->unknown34 = 0;
	data->unknown38 = 0;
	data->unknown3c = 0;
	data->unknown30 = 0;
	data->unknown5c = 0;
	data->unknown00 = 0;
	data->unknown18 = NONE;
	data->unknown08 = NONE;
	data->unknown10 = NONE;
	data->unknown0c = NONE;
	data->unknown14 = NONE;
	data->definition_index = definition_index;
	data->unknown20 = 1;
	data->location = location;
	data->scale_a = scale;
	data->scale_b = scale;
}

// score 555 (was 688): statement-by-statement walk against retail found no logic differences. What still differs is
// register and stack allocation. Retail keeps the impulse block's scale in xmm5 (spilled at [esp+8], reloaded right after
// each call) where ours reloads xmm0 before each use. It also passes the impact to 0x2284c0 in ECX where ours uses EAX,
// and passes both materials through stack temporaries ([esp+0x24] and [esp+0x94]) where ours reads material_a directly.
// The previous_type load is a separate fresh load in retail's two blocks and is spilled to [esp+0xe]; ours shares one load
// from the top. Tried: previous_type declared at the top of the else block (634), inside the condition (688), a copy of
// material_a as a local (656), explicit if/else for scale instead of PIN (686), scale declared in each block (688),
// level folded into the condition (640).
// Permuter, 35 minutes (11-05): its best form needs a flag set inside a test, the impact object lookup written out,
// temporaries and casts. Its one plain part, impact->type read at each use instead of the `type` local, gives 553.
// @retail 0x2294a0
void function_2294a0(
	long impact_index,
	long rigid_body_index_a,
	long rigid_body_index_b,
	s_impact *impact)
{
	s_impact_object *object = impact_object_header_get(havok_component_get(impact->component_a)->object_index)->object;
	s_impact_effect_location effect_location;
	long impulse_effect_a = NONE;
	long impulse_effect_b = NONE;
	real scale;
	bool impulse = false;
	union { short field_0; point2f field_0_1; } local_3;

	effect_location.position = impact->position;
	effect_location.normal = impact->normal;
	effect_location.unknown18 = 0x600022b;

	char previous_type = impact->unknown1d;

	if (impact->unknown11 && g_47f07a && impact->unknown1c > 2)
	{
		impulse = true;
		if (impact_type_is_continuous(previous_type))
		{
			real level = impact->unknown1d == 3 ? impact->unknown48 : impact->unknown4c;

			if (previous_type != 2 || !(level < 0.5f || impact->unknown18 > 0.25f))
			{
				impulse = false;
			}
		}
	}
	if (impulse)
	{
		s_sound_position sound_position;

		scale = PIN(impact->unknown18, 0.0f, 1.0f);
		long sound_a = NONE;
		long effect_a = NONE;
		long sound_b = NONE;
		long effect_b = NONE;
		long sound_c = NONE;
		long effect_c = NONE;
		sound_position.position = impact->position;
		sound_position.compressed_forward = vector3d_compress(&impact->unknown50);
		sound_position.velocity = *g_4687a4;
		sound_position.location = object->location;
		local_3.field_0 = *(short const *)&impact->material_b;
		impact_material_effects_get(impact, &impact->position, impact->material_a, *(c_type_47f957 const *)&local_3.field_0, 0,
			&sound_a, &effect_a, &sound_b, &effect_b, &sound_c, &effect_c);
		if (sound_a != NONE)
		{
			s_sound_label_play play;

			play.tag_index = sound_a;
			play.label = NONE;
			play.scale = scale;
			play.variant = NULL;
			function_189650(&sound_position, &play);
		}
		if (sound_b != NONE)
		{
			s_sound_label_play play;

			play.tag_index = sound_b;
			play.label = NONE;
			play.scale = scale;
			play.variant = NULL;
			function_189650(&sound_position, &play);
		}
		if (sound_c != NONE)
		{
			s_sound_label_play play;

			play.tag_index = sound_c;
			play.label = NONE;
			play.scale = scale;
			play.variant = NULL;
			function_189650(&sound_position, &play);
		}
		if (impact->shape.type == NONE)
		{
			s_effect_new_data data;

			if (effect_a != NONE)
			{
				function_2294a1(&data, effect_a, &effect_location, scale);
				impulse_effect_a = effect_new_from_parameters((s_effect_parameters *)&data);
			}
			if (effect_b != NONE)
			{
				function_2294a1(&data, effect_b, &effect_location, scale);
				impulse_effect_b = effect_new_from_parameters((s_effect_parameters *)&data);
			}
			if (effect_c != NONE)
			{
				function_2294a1(&data, effect_c, &effect_location, scale);
				impulse_effect_b = effect_new_from_parameters((s_effect_parameters *)&data);
			}
		}
		impact->time = g_510c54->game_time;
		impact->unknown98 = scale;
		impact->unknown1c = 0;
		impact->unknown10 = 0;
	}
	else
	{
		char type = impact->type;
		if (type == 1 && impact->unknown1e * g_510c54->rate > 0.5f && impact_has_sounds_or_effects(impact))
		{
			impact_sounds_stop(impact, true, true);
			impact->unknown1d = NONE;
		}
		else if (impact_type_is_continuous(type) && impact->unknown91 > 2)
		{
			if (type != previous_type && impact->unknown1e * g_510c54->rate > 0.5f)
			{
				bool continuing = false;

				if (impact->unknown1f != type)
				{
					impact->unknown20 = 0;
					impact->unknown1f = type;
				}
				if (impact->unknown1f == type)
				{
					if (impact->unknown20 * g_510c54->rate > 0.3f || !impact_has_sounds_or_effects(impact))
					{
						if (impact_has_sounds_or_effects(impact))
						{
							continuing = impact_type_is_continuous(previous_type) && impact_type_is_continuous(impact->unknown1f);
							impact_sounds_stop(impact, !continuing, true);
						}
						impact->unknown1d = NONE;
					}
				}
				if (g_47f07a && (!impact_has_sounds_or_effects(impact) || continuing))
				{
					bool start_looping_sounds = impact->unknownd == NONE || impact->unknowne;
					bool start_effects = (impact->unknownd == NONE || !impact->unknowne) && impact->shape.type == NONE;
					long looping_sound_a = NONE;
					long looping_sound_b = NONE;
					long effect_a = NONE;
					long effect_b = NONE;

					scale = impact_level_get(impact, impact->type);
					local_3.field_0 = *(short const *)&impact->material_b;
					impact_material_effects_get(impact, &impact->position, impact->material_a, *(c_type_47f957 const *)&local_3.field_0, impact->type,
						&looping_sound_a, &effect_a, &looping_sound_b, &effect_b, NULL, NULL);
					if (start_looping_sounds)
					{
						if (continuing)
						{
							if (impact->looping_sound_a != NONE)
							{
								function_18a720(impact->looping_sound_a, impact->type != 2);
							}
							if (impact->looping_sound_b != NONE)
							{
								function_18a720(impact->looping_sound_b, impact->type != 2);
							}
						}
						else
						{
							if (looping_sound_a != NONE)
							{
								impact->looping_sound_a = function_18a6c0(looping_sound_a, impact_index);
								function_18a720(impact->looping_sound_a, impact->type != 2);
							}
							if (looping_sound_b != NONE)
							{
								impact->looping_sound_b = function_18a6c0(looping_sound_b, impact_index);
								function_18a720(impact->looping_sound_b, impact->type != 2);
							}
						}
					}
					if (start_effects)
					{
						s_effect_new_data data;

						if (effect_a != NONE)
						{
							function_2294a1(&data, effect_a, &effect_location, scale);
							impact->effect_a = effect_new_from_parameters((s_effect_parameters *)&data);
						}
						if (effect_b != NONE)
						{
							function_2294a1(&data, effect_b, &effect_location, scale);
							impact->effect_b = effect_new_from_parameters((s_effect_parameters *)&data);
						}
					}
					if (impact_has_sounds_or_effects(impact))
					{
						impact->unknown1e = 0;
						impact->unknown1d = impact->type;
					}
				}
			}
		}
	}
	impact->unknown20 = MIN(impact->unknown20 + 1, 0x7e);
	impact->unknown1c = MIN(impact->unknown1c + 1, 0x7e);
	impact->unknown1e = MIN(impact->unknown1e + 1, 0x7e);
	if (impact->effect_a != NONE || impact->effect_b != NONE || impulse_effect_a != NONE || impulse_effect_b != NONE)
	{
		real level = impact_level_get(impact, impact->unknown1d);
		s_havok_component *component = havok_component_get(impact->component_a);
		transform4x3f matrix;
		point2f &size=local_3.field_0_1;
		vector3f velocity;
		hkRigidBody *rigid_body;

		impact_orientation_get(impact, impact_index, rigid_body_index_a, &matrix.up, &matrix.left, &size.x, &size.y, false,
			&matrix.forward, &matrix.position);
		matrix.left.i *= -1.0f;
		matrix.left.j *= -1.0f;
		matrix.left.k *= -1.0f;
		rigid_body = component->rigid_bodies.data[rigid_body_index_a].rigid_body;
		if (!rigid_body->m_fixed)
		{
			point3f const *local_2=(point3f const *)&rigid_body->m_motion->m_linear_velocity;
			velocity.i=local_2->x;
			velocity.j=local_2->y;
			velocity.k=local_2->z;
		}
		else
		{
			velocity = *g_4687a4;
		}
		if (impact->reference_count != 0 && impact->component_b != NONE)
		{
			vector3f velocity_b;

			havok_component_rigid_body_linear_velocity_get(havok_component_get(impact->component_b), rigid_body_index_b, &velocity_b);
			velocity.i -= velocity_b.i;
			velocity.j -= velocity_b.j;
			velocity.k -= velocity_b.k;
		}
		if (impact->effect_a != NONE && !function_17b030(impact->effect_a, &velocity, level, 0.0f, &matrix, &size.x))
		{
			impact->effect_a = NONE;
		}
		if (impact->effect_b != NONE && !function_17b030(impact->effect_b, &velocity, level, 0.0f, &matrix, &size.x))
		{
			impact->effect_b = NONE;
		}
		if (impulse_effect_a != NONE)
		{
			function_17b030(impulse_effect_a, &velocity, PIN(impact->unknown18, 0.0f, 1.0f), level, &matrix, &size.x);
		}
		if (impulse_effect_b != NONE)
		{
			function_17b030(impulse_effect_b, &velocity, PIN(impact->unknown18, 0.0f, 1.0f), level, &matrix, &size.x);
		}
	}
	impact->unknown11 = false;
}

// score 485 (was 485): walked statement by statement against retail 0x228770-0x228f2a (2026-11-05): fields, offsets,
// condition directions, callee arguments and loop shape all agree; the remaining difference is register allocation
// and block layout. Retail keeps &impact->position in ebx for the whole function (lea ebx,[edi+0x68] at 0x228803,
// [ebx] in the constraint loop) and spills component to [esp+0x18] (frame 0x70); ours folds the address into
// [edi+0x68], keeps component in ebx and impact_index in ebp (frame 0x6c), and lays out the constraint if/else with
// the impact_release block first. Retail's two point-velocity calls before the shape test both write velocity_b, so
// they are dead and kept as is. Tried: a point3f* local for impact->position (485), function_227811 for the dots
// (485), !()/De Morgan/swapped forms of the constraint test (485), component computed after the early return (487),
// matrix at function scope (485), relative_velocity = velocity_a per component (508, 28 bytes short).
// Permuter, 35 minutes (11-05): 274 only with `constraint->impact_index` cleared as a volatile byte (a behaviour
// change), temporaries and casts. Lead: the constraint count read once into a local before the loop (`long
// constraint_count = component->unknown88.size;`) gives 327 fast, both callers still matching, since the stack slots
// and homes then line up; not kept, as retail rereads the size on every pass (latch 0x228bfa: `mov edx, [ecx+0x8c]`
// with the component reloaded from [esp+0x18] at 0x228bf2). The take block's sums in another order give 480; the
// constraint test inverted or the mass-ratio test as nested ifs change nothing.
// @retail 0x228770
void function_228770(
	long impact_index,
	s_impact *impact)
{
	s_havok_component *component = havok_component_get(impact->component_a);
	long rigid_body_index_a;
	long rigid_body_index_b;
	vector3f normal;
	point3f position;

	impact_rigid_body_indices_get(impact, impact_index, &rigid_body_index_a, &rigid_body_index_b);
	if (impact->unknownd != NONE)
	{
		impact_type_update(impact_index, rigid_body_index_a, rigid_body_index_b, impact);
		function_2294a0(impact_index, rigid_body_index_a, rigid_body_index_b, impact);
		return;
	}

	{
		transform4x3f matrix;

		component->rigid_body_matrix_get(rigid_body_index_a, &matrix);
		function_2281a1(&matrix, &impact->local_position_a, &impact->position);
		if (impact->component_b != NONE)
		{
			point3f position_b;

			havok_component_get(impact->component_b)->rigid_body_matrix_get(rigid_body_index_b, &matrix);
			function_2281a1(&matrix, &impact->local_position_b, &position_b);
			impact->position.x += (position_b.x - impact->position.x) * 0.5f;
			impact->position.y += (position_b.y - impact->position.y) * 0.5f;
			impact->position.z += (position_b.z - impact->position.z) * 0.5f;
		}
	}
	normal = *g_4687a4;
	position = *g_468788;
	impact->unknown44 = 0.0f;
	impact->unknown40 = 0.0f;
	if (impact->shape.type == NONE)
	{
		real mass_ratio = 1.0f;
		long i;

		if (impact->component_b != NONE && impact->reference_count != 0)
		{
			s_havok_component *component_b = havok_component_get(impact->component_b);
			long mass_rigid_body_index_a;
			long mass_rigid_body_index_b;
			real mass_a;
			real mass_b;

			impact_rigid_body_indices_get(impact, impact_index, &mass_rigid_body_index_a, &mass_rigid_body_index_b);
			mass_a = havok_component_rigid_body_mass_get(mass_rigid_body_index_a, component);
			mass_b = havok_component_rigid_body_mass_get(mass_rigid_body_index_b, component_b);
			if (mass_a > mass_b)
			{
				mass_ratio = mass_a / mass_b;
			}
		}
		for (i = 0; i < component->unknown88.size; i++)
		{
			s_havok_component_element48 *constraint = &component->unknown88.data[i];

			if (constraint->impact_index == impact_index)
			{
				vector3f delta;
				real distance_squared;

				vector3d_from_points3d(&impact->position, &constraint->position, &delta);
				distance_squared = delta.i * delta.i;
				distance_squared += delta.k * delta.k;
				distance_squared += delta.j * delta.j;
				if (distance_squared < 0.25f ||
					distance_squared < 16.0f && dot3f(&impact->normal, &constraint->normal) > g_55c2d0)
				{
					impact->unknown40 = MAX(constraint->impulse * mass_ratio, impact->unknown40);
					normal.i = constraint->normal.i + normal.i;
					normal.j = constraint->normal.j + normal.j;
					normal.k = constraint->normal.k + normal.k;
					position.x += constraint->position.x;
					position.y += constraint->position.y;
					position.z += constraint->position.z;
				}
				else
				{
					impact_release(impact_index, impact);
					constraint->impact_index = NONE;
				}
			}
		}
		if (function_146650() - component->unknown10 < game_seconds_to_ticks_round(0.35f))
		{
			impact->unknown40 += component->unknown14;
		}
	}
	if (impact->reference_count != 0)
	{
		vector3f velocity_a;
		vector3f velocity_b;
		real inverse_count;
		vector3f velocity;
		real speed_squared;

		impact->normal = normal;
		function_1201a0(&impact->normal, g_4687b0);
		havok_component_rigid_body_point_velocity_get(&velocity_b, rigid_body_index_a, component, &impact->position);
		if (impact->component_b != NONE)
		{
			havok_component_rigid_body_point_velocity_get(&velocity_b, rigid_body_index_b, havok_component_get(impact->component_b), &impact->position);
		}
		if (impact->shape.type == NONE)
		{
			vector3f relative_velocity;
			real dot;

			havok_component_rigid_body_point_velocity_get(&velocity_a, rigid_body_index_a, component, &impact->position);
			if (impact->component_b != NONE)
			{
				havok_component_rigid_body_point_velocity_get(&velocity_b, rigid_body_index_b, havok_component_get(impact->component_b), &impact->position);
				relative_velocity.i = velocity_a.i - velocity_b.i;
				relative_velocity.j = velocity_a.j - velocity_b.j;
				relative_velocity.k = velocity_a.k - velocity_b.k;
			}
			else
			{
				relative_velocity = velocity_a;
			}
			dot = dot3f(&impact->normal, &relative_velocity);
			impact->unknown50.i = relative_velocity.i - dot * impact->normal.i;
			impact->unknown50.j = relative_velocity.j - impact->normal.j * dot;
			impact->unknown50.k = relative_velocity.k - impact->normal.k * dot;
			impact->unknown44 = function_30bf0(&impact->unknown50);
			if (impact->unknown44 == 0.0f)
			{
				impact->unknown50 = *g_4687b0;
			}
		}
		inverse_count = 1.0f / impact->reference_count;
		position.x = inverse_count * position.x;
		position.y *= inverse_count;
		position.z *= inverse_count;
		impact->position = position;
		impact_local_positions_update(impact, rigid_body_index_a, rigid_body_index_b);
		velocity = component->rigid_bodies.data[rigid_body_index_a].linear_velocity;
		if (impact->component_b != NONE)
		{
			s_havok_component_rigid_body const *rigid_body_b = &havok_component_get(impact->component_b)->rigid_bodies.data[rigid_body_index_b];

			velocity.i -= rigid_body_b->linear_velocity.i;
			velocity.j -= rigid_body_b->linear_velocity.j;
			velocity.k -= rigid_body_b->linear_velocity.k;
		}
		speed_squared = velocity.i * velocity.i;
		speed_squared += velocity.k * velocity.k;
		speed_squared += velocity.j * velocity.j;
		if (impact->unknown90 >= 3)
		{
			impact->unknown8c = 0.0f;
			impact->unknown90 = 0;
		}
		if (speed_squared > impact->unknown8c)
		{
			impact->unknown8c = speed_squared;
			impact->unknown90 = 0;
		}
		impact->unknown90++;
		impact_type_update(impact_index, rigid_body_index_a, rigid_body_index_b, impact);
		function_2294a0(impact_index, rigid_body_index_a, rigid_body_index_b, impact);
	}
}

PRIVATE inline s_physics_model_shape_key physics_model_shape_key_make(short type, short index)
{
	s_physics_model_shape_key key;

	key.type = type;
	key.index = index;
	return key;
}

// @retail 0x226a60
void function_226a60(
	long component_index)
{
	s_havok_component *component = havok_component_get(component_index);

	if (TEST_FIELD_BIT(component->flag11) && havok_component_any_rigid_body_active(component))
	{
		byte *definition = impact_tag_get(impact_object_header_get(component->object_index)->object->definition_index);
		byte *model = impact_tag_get(*(long *)(definition + 0x38));
		byte *physics_model = impact_tag_get(*(long *)(model + 0x24));
		long i;

		for (i = 0; i < component->unknown7c.size; i++)
		{
			if (component->unknown7c.data[i].impact_index == NONE)
			{
				s_havok_component_element0c *contact = &component->unknown7c.data[i];
				s_physics_model_shape_key shape;
				long element_size;
				s_impact_tag_block *block;
				byte *element = NULL;
				short material_index;

				shape = physics_model_shape_key_make(contact->unknown00, contact->unknown02);
				block = physics_model_shape_block_get(physics_model, &shape, &element_size);
				if (shape.index < block->count)
				{
					element = block->address + shape.index * element_size;
				}
				material_index = *(short *)(element + 0x70);
				if (material_index != NONE)
				{
					long local_2;
					s_physics_model_material *material = &(*(s_physics_model_material **)(physics_model + 0x34))[material_index];
					local_2 = material->shape_count;
					long j;

					for (j = 0; j < local_2; j++)
					{
						s_physics_model_material_shape *material_shape = &material->shapes[j];

						if (material_shape->type == shape.type && material_shape->index == shape.index)
						{
							if (material_shape->flags & 2)
							{
								goto next;
							}
							break;
						}
					}
					{
						s_impact_data data;
						transform4x3f matrix;
						long rigid_body_index_a;
						long rigid_body_index_b;
						long impact_index;

						havok_component_contact_properties_get(component, i, &rigid_body_index_a, &rigid_body_index_b);
						component->rigid_body_matrix_get(rigid_body_index_a, &matrix);
						impact_data_set(&data, false, component_index, rigid_body_index_a, *(c_type_47f957 const *)&material->material_a,
							rigid_body_index_b != NONE ? component_index : NONE, rigid_body_index_b, *(c_type_47f957 const *)&material->material_b,
							&matrix.position, &matrix.forward, NONE, &shape);
						impact_index = havok_component_impact_find(component, (s_havok_impact_contact const *)&data, false);
						if (impact_index == NONE)
						{
							if (havok_component_impact_make_room(impact_index, component, impact_distance_squared_to_nearest_player(&data.position, 1) * 1.1f))
							{
								impact_index = impact_new(&data, 1);
								havok_component_impact_add(component, impact_index);
							}
						}
						if (impact_index != NONE)
						{
							s_impact *impact = impact_get(impact_index);

							component->unknown7c.data[i].impact_index = impact_index;
							impact->reference_count++;
						}
					}
				}
			}
		next:;
		}
	}
}

/* an iteration over the impacts that keeps the current impact */
struct s_impact_iterator
{
	s_impact *impact;
	s_record_pool_iterator iterator;
};

/* the havok component the constraint update reached (g_5021bc) */
struct s_impact_component_iterator
{
	s_havok_component *component;
	s_record_pool_iterator iterator;
};

s_impact_component_iterator g_5021bc;

// @retail 0x226ce0
void function_226ce0(void)
{
	long count = 0;

	if (!data_datum_iterator_next((s_data_datum_iterator *)&g_5021bc))
	{
		g_5021bc.iterator.data = g_51e9b8;
		g_5021bc.iterator.index = NONE;
		g_5021bc.iterator.datum_index = NONE;
	}
	do
	{
		s_havok_component *component = (s_havok_component *)data_iterator_next_inlined(&g_5021bc.iterator);

		g_5021bc.component = component;
		if (!component)
		{
			break;
		}
		if (havok_component_any_rigid_body_active(component) &&
			(component->unknown20 == NONE || impact_array_get(component->unknown20)->count < 15))
		{
			long i;

			for (i = 0; i < component->unknown88.size; i++)
			{
				s_havok_component_element48 *constraint = &component->unknown88.data[i];

				if (constraint->impact_index == NONE)
				{
					s_havok_component *component_b = constraint->component_b != NONE ? havok_component_get(constraint->component_b) : NULL;
					s_impact_data data;
					long impact_index;

					impact_data_set(&data, false, g_5021bc.iterator.datum_index, constraint->rigid_body_index_a,
						c_type_47f957(constraint->material_a), constraint->component_b,
						constraint->rigid_body_index_b, c_type_47f957(constraint->material_b),
						&constraint->position, &constraint->normal, NONE, NULL);
					impact_index = havok_component_impact_find(component, (s_havok_impact_contact const *)&data, true);
					if (impact_index == NONE)
					{
						if ((!component_b || havok_component_impact_find(component_b, (s_havok_impact_contact const *)&data, false) == impact_index) &&
							impact_components_valid(data.component_a, data.component_b) &&
							havok_component_impact_make_room(constraint->rigid_body_index_a, component,
								impact_distance_squared_to_nearest_player(&data.position, 1) * 1.1f))
						{
							impact_index = impact_new(&data, 1);
							havok_component_impact_add(component, impact_index);
						}
					}
					if (impact_index != NONE)
					{
						constraint->impact_index = impact_index;
						impact_get(impact_index)->reference_count++;
					}
				}
			}
		}
		function_226a60(g_5021bc.iterator.datum_index);
		count++;
	} while (count <= 0x20);
}

/* an iteration over the impacts: the current impact, then the record-pool iterator */
struct s_impact_iterator_226f80
{
	s_impact *impact;
	s_record_pool_iterator iterator;
};

static inline bool impact_iterator_next_226f80(s_impact_iterator_226f80 *iterator)
{
	iterator->impact = (s_impact *)data_iterator_next_inlined(&iterator->iterator);
	return iterator->impact != NULL;
}

// @retail 0x226f80
void function_226f80(void)
{
	s_impact_iterator_226f80 iterator;

	iterator.iterator.data = g_51ebfc;
	iterator.iterator.index = NONE;
	while (impact_iterator_next_226f80(&iterator))
	{
		s_impact *impact = iterator.impact;
		long impact_index = iterator.iterator.datum_index;
		s_havok_component *component = havok_component_get(impact->component_a);
		bool has_sounds = impact_has_sounds_or_effects(impact);
		bool merged = false;
		long i;

		for (i = 0; i < (component->unknown20 == NONE ? 0 : impact_array_get(component->unknown20)->count); i++)
		{
			long other_index = impact_array_get(component->unknown20)->impacts[i];

			if (other_index != impact_index)
			{
				s_impact *other = impact_get(other_index);
				bool other_has_sounds = impact_has_sounds_or_effects(other);

				if (other->reference_count != 0)
				{
					vector3f delta;

					vector3d_from_points3d(&impact->position, &other->position, &delta);
					if (delta.i * delta.i + delta.j * delta.j + delta.k * delta.k < 4.0f &&
						(has_sounds && !other_has_sounds || has_sounds == other_has_sounds))
					{
						bool other_merged = false;
						long j;

						for (j = 0; j < component->unknown88.size; j++)
						{
							s_havok_component_element48 *constraint = &component->unknown88.data[j];

							if (constraint->impact_index == other_index)
							{
								s_impact_data data;

								impact_data_set(&data, false, other->component_a, constraint->rigid_body_index_a, other->material_a,
									other->component_b, constraint->rigid_body_index_b, other->material_b,
									&constraint->position, &constraint->normal, NONE, NULL);
								if (impact_matches_data(impact, &data, true))
								{
#pragma inline_depth(0)
									impact_release(other_index, other);
#pragma inline_depth()
									impact->reference_count++;
									constraint->impact_index = impact_index;
									merged = true;
									other_merged = true;
								}
							}
						}
						if (other_merged && other->reference_count != 0)
						{
							function_228770(other_index, other);
						}
					}
				}
			}
		}
		if (merged)
		{
			function_228770(impact_index, impact);
		}
	}
}

// @retail 0x227280
void function_227280(void)
{
	s_record_pool_iterator iterator;
	long count = 0;
	long i;

	iterator.data = g_51ebfc;
	iterator.index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		g_50213c[count++] = iterator.datum_index;
	}
	if (count > 1)
	{
		long temporary;

		sort_4byte(g_50213c, count, &temporary, (t_sort_4byte_compare_function)impact_sort_compare, NULL);
	}
	for (i = 0; i < count; i++)
	{
		impact_get(g_50213c[i])->sort_index = (char)i;
	}
	if (count > 0)
	{
		g_502138 = count - 1;
	}
	else
	{
		g_502138 = NONE;
	}
}

// @retail 0x2268e0
void impacts_update(void)
{
	s_impact_iterator iterator;

	function_226ce0();
	iterator.iterator.data = g_51ebfc;
	iterator.iterator.index = NONE;
	while ((iterator.impact = (s_impact *)data_iterator_next_inlined(&iterator.iterator)) != NULL)
	{
		s_impact *impact = iterator.impact;
		long impact_index = iterator.iterator.datum_index;
		s_havok_component *component = havok_component_get(impact->component_a);

		if (impact->reference_count != 0 && havok_component_any_rigid_body_active(component))
		{
			impact->age = 0;
			function_228770(impact_index, impact);
		}
		else
		{
			impact->age = MIN(impact->age + 1, 0x7e);
			if (!impact_has_sounds_or_effects(impact) &&
				(impact->time == NONE || function_146650() - impact->time > game_seconds_to_ticks_round(0.2f)) ||
				impact->age * g_510c54->rate >= 0.2f)
			{
				function_2266a0(impact_index);
				continue;
			}
		}
		impact->priority = impact_distance_squared_to_nearest_player(&impact->position, impact->type);
		if (impact_has_sounds_or_effects(impact))
		{
			impact->priority *= 1.5f;
		}
	}
	function_226f80();
	function_227280();
}

/* ---- where an impact's sound plays ---- */

extern vector3f *g_4687a4;

/* the location of an impact's sound */
struct s_impact_sound_location
{
	point3f position;
	dword normal;
	vector3f velocity;
	s_location location;
};

/* the center of mass of a component's main rigid body, when it has one */
static __forceinline bool impact_component_center_get(s_havok_component *component, point3f *center)
{
	if ((char)component->unknown1c <= 3 && function_0b67a0((s_small_index const *)component) != NONE)
	{
		hkRigidBody *rigid_body = component->rigid_bodies.data[function_0b67a0((s_small_index const *)component)].rigid_body;

		point3f const *local_0 = (point3f const *)((byte *)rigid_body->m_motion + 0x70);
		real x = local_0->x;
		real z = local_0->z;
		real y = local_0->y;
		center->x = x;
		center->y = y;
		center->z = z;
		return true;
	}
	return false;
}

// @retail 0x22a110
void impact_sound_location_get(s_impact const *impact, s_impact_sound_location *arg_26d7e7, real *scale)
{
	s_havok_component *component = havok_component_get(impact->component_a);
	s_impact_object *object = impact_object_header_get(component->object_index)->object;
	real value = impact->unknown1d == 3 ? impact->unknown48 : impact->unknown4c;

	if (impact->unknowne)
	{
		s_object_marker marker;

		if (function_b8d30(component->object_index, 0x60005bd, &marker, 1, false) == 1)
		{
			arg_26d7e7->position = marker.matrix.position;
		}
		else
		{
			function_b9dd0(component->object_index, &arg_26d7e7->position);
		}
	}
	else
	{
		arg_26d7e7->position = impact->position;
		if (impact->reference_count > 0)
		{
			if (impact->component_b != NONE)
			{
				s_havok_component *component_b = havok_component_get(impact->component_a);
				point3f center_a;
				point3f center_b;
				bool has_a = false;
				bool has_b = false;

				if (impact_component_center_get(component, &center_a))
				{
					arg_26d7e7->position = center_a;
					has_a = true;
				}
				if (impact_component_center_get(component_b, &center_b))
				{
					arg_26d7e7->position = center_b;
					has_b = true;
				}
				if (has_a && has_b)
				{
					arg_26d7e7->position.x = (center_a.x + center_b.x) * 0.5f;
					arg_26d7e7->position.y = (center_a.y + center_b.y) * 0.5f;
					arg_26d7e7->position.z = (center_a.z + center_b.z) * 0.5f;
				}
			}
			else
			{
				impact_component_center_get(component, &arg_26d7e7->position);
			}
		}
	}

	arg_26d7e7->normal = vector3d_compress(&impact->unknown50);
	arg_26d7e7->velocity = *g_4687a4;
	arg_26d7e7->location = object->location;
	*scale = PIN(value, 0.0f, 1.0f);
}
