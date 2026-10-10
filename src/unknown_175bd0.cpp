// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_175BD0.CPP: the effects (entry 40 of the lifecycle table) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"
#include "object_markers.h"
#include "unknown_1765e0.h"
#include "sound_sources.h"
#include <string.h>

s_record_pool *g_51ec8c;
s_record_pool *g_51ec88;
s_record_pool *g_51ec84;
s_record_pool *g_510c74;
s_record_pool *g_4ea93c;
s_record_pool *g_4ea938;

/* the up vector of the effect markers (unknown_11d180.cpp) */
extern vector3f *g_4687bc;

/* the source of the effect being started (s_effect_parameters::source) */
s_effect_source *g_510c78;
bool g_510c7c;

struct s_object;
s_object *function_badc0(long object_index, dword type_mask);
bool object_or_parent_hidden(long object_index);
point3f *function_b9dd0(long object_index, point3f *result);
byte *record_pool_lookup(s_record_pool *data, long datum_index);
long function_18a750(long tag_index, long value);
void function_11bed0(s_location *location, point3f const *point);
void function_c40f0(long tag_index, long object_index, real value);
real function_259d0(dword *seed, char const *file, long line, real lower_bound, real upper_bound);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
transform4x3f *function_1664a5(long group_index, long key, short node_index);
void function_1664da(long group_index, long key, short node_index, transform4x3f *out);

/* the views of objects the effects read */
struct s_effect_object_header
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	byte *object;
};

struct s_effect_attachment
{
	byte type;
	byte unknown01[3];
	long index;
};

struct s_effect_object
{
	long tag_index;
	dword : 8;
	dword flag4_8 : 1;
	dword : 23;
	byte unknown08[0x28 - 8];
	s_location location;
	point3f bounding_center;
	real field_xc9d3f8;
	byte unknown40[0x64 - 0x40];
	point3f position;
	byte unknown70[0x88 - 0x70];
	vector3f velocity;
	byte unknown94[0xc2 - 0x94];
	short owner_unknown8;
	long owner_unknown0;
	long owner_unknown4;
	byte unknowncc[0x10a - 0xcc];
	word unknown10a_0 : 2;
	word flag10a_2 : 1;
	word : 13;
	byte unknown10c[0x116 - 0x10c];
	short nodes_offset;
	byte unknown118[4];
	short attachments_size;
	short attachments_offset;
	byte unknown120[0x13c - 0x120];
	long player_index;
	byte unknown140[0x2e0 - 0x140];
	long unknown2e0;
};

struct s_effect_object_definition
{
	byte unknown00[2];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word : 2;
};

/* a marker of an object (0x70 bytes; function_b8d30) */
struct s_effect_object_marker
{
	short node_index;
	byte unknown02[2];
	transform4x3f matrix;
	transform4x3f unknown38;
	byte unknown6c[4];
};

/* unknown_165ce5.cpp (0x1662c1); its markers have the same layout */
struct s_first_person_marker;
short first_person_weapon_get_markers_internal(long weapon_index, long marker_name, s_first_person_marker *markers, short marker_count);

/* the zones of the structure bsp (function_11c120) */
struct s_effect_zone_cluster
{
	byte unknown00[0x70];
	byte zone;
	byte unknown71[0xb0 - 0x71];
};

struct s_effect_zone
{
	byte unknown00[2];
	short index;
	plane3f plane;
	byte unknown14[4];
};

struct s_effect_zone_bsp
{
	byte unknown00[0x68];
	s_effect_zone *zones;
	byte unknown6c[0xa0 - 0x6c];
	s_effect_zone_cluster *clusters;
};

/* a color query of the effects (function_17b5d0) */
struct s_effect_color_query
{
	real unknown00;
	real unknown04;
	real unknown08;
	dword color_a;
	dword color_b;
};

long function_d2bb0(void *source, s_effect_color_query *query);
long __stdcall function_d2a50(long a, long b, long c, s_effect_color_query *query, long d, point3f const *point);
bool function_11c050(s_location const *location);
bool function_11c080(s_location const *location);
bool function_11c120(s_location const *location, point3f const *point, short *zone_index);
bool function_3eb20(long cluster_index);
bool __stdcall function_3ebd0(vector3f const *offset, transform4x3f const *matrices, transform4x3f *out, long count);
long __stdcall function_3ddd0(long object_index);
bool __stdcall function_bab40(long object_index, long name, real *value);
bool function_bad50(long object_index, long index, point3f *out);
long function_baf80(long object_index);
long function_155760(long index);
bool function_163080(void);
long function_166244(long key);
bool function_166283(long group_index, long key);
s_player_state *function_16f3a0(long index);
vector3f *function_11d000(vector3f const *v, vector3f *out);
void function_179fb0(s_effect_datum *effect);
void __stdcall function_17a380(s_effect_datum *effect);
extern s_record_pool *g_509434;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

/* the clusters whose effects are visible */
dword g_4c56c0[64];

#define OBJECT_GET(index) ((s_effect_object *)((s_effect_object_header *)g_4e0300->data)[(index) & 0xffff].object)

static inline transform4x3f *effect_object_node_matrix(long object_index, short node_index)
{
	s_effect_object *object = OBJECT_GET(object_index);

	return (transform4x3f *)((byte *)object + object->nodes_offset) + node_index;
}

/* an object looping sound (g_4ed28c, 0x18 bytes) */
struct s_effect_looping_sound
{
	short salt;
	short unknown02;
	byte flag0 : 1;
	byte flag1 : 1;
	byte : 6;
	byte unknown05[0x18 - 5];
};

/* a player (g_4e8c24, 0x21c bytes) */
struct s_effect_player
{
	byte unknown00[0x28];
	short unknown28;
	byte unknown2a[2];
	long unit_index;
	byte unknown30[0x21c - 0x30];
};

struct s_effect_looping_sound;

/* the effect of an index that may be stale (the salt is checked) */
static inline s_effect_datum *effect_try_and_get(long effect_index)
{
	s_effect_datum *result = 0;

	if (effect_index != NONE)
	{
		s_record_pool *data = g_4ea93c;
		long index = effect_index & 0xffff;

		if (index < data->high_water_index)
		{
			s_effect_datum *effect = (s_effect_datum *)(data->data + data->size * index);

			if (effect->salt != 0 && effect->salt == (effect_index >> 16))
				result = effect;
		}
	}
	return result;
}

struct s_bsp3d;
extern s_bsp3d *g_4e033c;
long function_14a280(s_bsp3d *bsp, long index, point3f *point);

struct s_effect_structure_leaf
{
	short cluster_index;
	byte unknown02[6];
};

struct s_effect_structure_bsp
{
	byte unknown00[0x30];
	s_effect_structure_leaf *leaves;
};

/* the leaf and cluster of the structure bsp a point is in */
static inline void effect_location_from_point(s_location *location, point3f *point)
{
	short bsp_index = g_4686c4;

	if (bsp_index == NONE)
	{
		location->bsp_index = bsp_index;
		location->leaf_index = NONE;
		location->cluster_index = NONE;
	}
	else
	{
		long leaf_index = function_14a280(g_4e033c, 0, point);

		location->leaf_index = leaf_index;
		long cluster_index = leaf_index != NONE ? ((s_effect_structure_bsp *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
		location->cluster_index = (short)cluster_index;
		location->bsp_index = bsp_index;
	}
}

static inline void effect_stop_looping_sound(s_effect_datum *effect)
{
	if (effect->looping_sound_index != NONE)
	{
		DATUM(g_4ed28c, s_effect_looping_sound, effect->looping_sound_index)->flag1 = true;
		effect->looping_sound_index = NONE;
	}
}

static inline void effect_owner_set_none(s_effect_owner *owner)
{
	owner->unknown4 = NONE;
	owner->unknown0 = NONE;
	owner->unknown8 = NONE;
}

static inline void effect_parameters_initialize_inline(s_effect_parameters *parameters)
{
	memset(parameters, 0, sizeof(*parameters));
	parameters->tag_index = NONE;
	parameters->unknown18 = NONE;
	parameters->object_index = NONE;
	parameters->owner.unknown4 = NONE;
	parameters->owner.unknown0 = NONE;
	parameters->unknown34 = 0;
	parameters->owner.unknown8 = NONE;
	parameters->unknown38 = 0;
	parameters->scale_a = 1.0f;
	parameters->scale_b = 1.0f;
	parameters->unknown3c = 0;
	parameters->unknown30 = 0;
	parameters->color_a = 0xff808080;
	parameters->color_b = 0xff808080;
	parameters->source = 0;
}

long __stdcall effect_new_from_parameters(s_effect_parameters *parameters);
bool function_176210(s_effect_parameters *parameters);
long function_178120(s_effect_owner const *owner, bool force, long tag_index);
void function_178240(point3f const *origin, vector3f const *direction, s_effect_datum *effect, real scale_a, real scale_b);
struct s_effect_marker_source;
void function_1786f0(s_effect_marker_source const *source, s_effect_object_marker *out, short marker_index);
long function_1785c0(s_effect_object_marker const *marker, s_effect_datum *effect, short location_index, bool field_b4);
transform4x3f *function_178bc0(s_effect_location_datum *location, s_effect_datum *effect, transform4x3f *matrix, bool field_b4);
void function_178c80(real scale, s_effect_datum *effect, s_particle_system_datum *particle_system, s_effect_particle_system_definition *definition, real unknown, bool field_b4);
bool function_178020(short placement, point3f const *point, s_location const *location);
__declspec(noinline) bool function_17b270(short const *values, s_effect_datum *effect, long mode);
void function_17af30(transform4x3f *matrix, s_effect_datum *effect, bool field_b4, short node_index);
void function_177260(long effect_index, bool flag);
__declspec(noinline) bool function_1794a0(long object_index, long other_object_index);
void function_179020(long effect_index, real scale, real unknown);
long function_179190(s_effect_datum *effect);
bool function_1792b0(long effect_index, real dt);
bool __stdcall function_1794e0(long effect_index);
bool function_179730(long effect_index);
bool function_179810(long effect_index);
bool function_175f50(long object_index);
void function_17b750(long *values, long value);
void function_178360(long effect_index, short unknown18, long object_index, long unknown58, s_effect_marker *markers, long marker_count);
bool function_1789f0(s_effect_datum *effect);
void function_17b5d0(s_effect_datum *effect, long object_index, s_effect_parameters *parameters, bool search);
void function_177310(long effect_index);
__declspec(noinline) void function_179850(long effect_index, real value);
void function_1771a0(s_effect_datum *effect);
void function_1773a0(long effect_index);
void function_177460(s_effect_datum *effect);
void function_177590(long effect_index);
bool function_177610(long effect_index);
dword *function_177c20(long tag_index);
bool function_178060(void);
void function_178ad0(s_effect_datum *effect);
void function_1782a0(long effect_index, short event_index);
void function_17add0(s_effect_datum *effect);
__declspec(noinline) s_effect_location_datum *__stdcall effect_location_next(s_effect_datum *effect, long *location_index, short mode);
void function_176a50(s_effect_parameters *parameters, long tag_index, long marker_count, s_effect_marker *markers, long mode);
s_effect_marker *function_176330(s_effect_marker *markers, point3f const *point);

/* the object tags (only the field the effects read) */
struct s_effect_object_tag
{
	byte unknown00[0x14];
	real unknown14;
};

/* what the damage effect parts fill in (function_d6660, 0xd6660 in damage.cpp) */
struct s_effect_damage_data
{
	byte unknown00[8];
	s_effect_owner owner;
	byte unknown14[8];
	s_location location;
	point3f position;
	point3f origin;
	vector3f direction;
	byte unknown48[0x54 - 0x48];
	real scale;
	byte unknown58[0x6c - 0x58];
	vector3f forward;
	byte unknown78[4];
	short unknown7c;
	byte unknown7e[0xc4 - 0x7e];
};

/* what the object effect parts fill in (function_b7930) */
struct s_effect_object_placement
{
	byte unknown00[0x1c];
	point3f position;
	vector3f forward;
	vector3f up;
	vector3f velocity;
	vector3f angular_velocity;
	byte unknown58[0xc4 - 0x58];
};

/* set once an effect part of an unknown type was seen */
bool g_55e756;

void random_vector_in_cone(vector3f const *forward, vector3f *result, dword *seed, real min_angle, real max_angle);
long function_baf40(long object_index);
long function_1469f0(real seconds);
bool function_172750(long mode, point3f const *point, real radius);
real function_259a0(dword *seed);
dword vector3d_compress(vector3f const *vector);
long function_189060(long object_index, short value, real scale, point3f const *position, vector3f const *direction, long tag_index);
long function_1895f0(s_sound_position const *position, real scale, long tag_index);
void function_bb950(long object_index, bool add, long delta);
long __stdcall function_b7b40(void *creation);
void function_1ca290(long tag_index, long ticks, long object_index, long node_index, real lower, real upper, transform4x3f const *matrix);
struct s_type_1e6529;
void function_d6660(s_type_1e6529 *data, long definition_index); /* damage.cpp */
long __stdcall function_d6c80(s_type_1e6529 *data, long ignore_object_index); /* damage.cpp */
void function_b7930(void *data, long tag_index, long object_index, s_effect_owner const *owner); /* stubs/lane_o.cpp */
bool function_a7640(s_effect_object_placement *data);
void __stdcall function_a7870(long object_index); /* stubs/lane_o.cpp */
void function_c0350(long tag_index, long object_index, long node_index, vector3f const *up, vector3f const *forward, point3f const *position, real scale);
bool __stdcall function_16a8e0(long name, point3f const *point, real radius, long object_index, long unknown, point3f *origin, real *radius_reference);
void __stdcall function_179880(s_effect_datum *effect, long effect_index);
void __stdcall function_179e80(s_effect_datum *effect);
long __stdcall function_177040(long particle_system_index, long effect_index);
void __stdcall function_174990(real dt);
void function_156b60(s_effect_beam *beam, real progress, transform4x3f const *matrix);
void function_248c60(s_particle_location_datum *particle_location, s_particle_system_datum *particle_system, transform4x3f const *matrix, bool field_b4);
void function_17e670(s_effect_source *source, point3f const *point, long tag_index, vector3f const *vector, real radius, long unknown0, long unknown1, long unknown2);
void __stdcall function_b7880(long object_index, long node_index, point3f const *point, vector3f const *impulse, vector3f const *angular_impulse); /* stubs/damage.cpp */

/* unknown_0259d0's inline matrix and vector helpers */
static inline point3f *effect_matrix_transform_point(transform4x3f const *matrix, point3f const *point, point3f *out)
{
	real x = point->x;
	real y = point->y;
	real z = point->z;

	if (matrix->scale != 1.0f)
	{
		x = matrix->scale * x;
		y = matrix->scale * y;
		z = matrix->scale * z;
	}
	out->x = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x + matrix->position.x;
	out->y = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
}

static inline vector3f *effect_matrix_transform_normal(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	out->i = matrix->up.i * vector->k + matrix->left.i * vector->j + matrix->forward.i * vector->i;
	out->j = matrix->up.j * vector->k + matrix->left.j * vector->j + matrix->forward.j * vector->i;
	out->k = matrix->up.k * vector->k + matrix->left.k * vector->j + matrix->forward.k * vector->i;
	return out;
}

static inline vector3f *effect_matrix_transform_normal_179880(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

static inline real effect_normalize(vector3f *v)
{
	real magnitude = (real)sqrt(v->k * v->k + v->j * v->j + v->i * v->i);

	if (fabs(magnitude) < 0.0001f)
		return 0.0f;
	real inverse = 1.0f / magnitude;
	v->i = inverse * v->i;
	v->j = v->j * inverse;
	v->k = v->k * inverse;
	return magnitude;
}

// @retail 0x175b50
void function_175b50(void)
{
	g_4ea93c = data_new_inlined("effect", 0x100, sizeof(s_effect_datum), 0, g_510c2c);
	g_4ea938 = data_new_inlined("effect location", 0x200, sizeof(s_effect_location_datum), 0, g_510c2c);
	function_173de0();
}

// @retail 0x175bd0
void effects_dispose(void)
{
	g_51ec8c = 0;
	g_51ec88 = 0;
	g_51ec84 = 0;
	g_510c74 = 0;
	if (g_4ea93c)
	{
		g_4ea93c = 0;
	}
	if (g_4ea938)
	{
		g_4ea938 = 0;
	}
}

// @retail 0x175c10
void function_175c10(void)
{
	data_make_valid_inlined(g_4ea93c);
	data_make_valid_inlined(g_4ea938);
	function_x496c75();
}

// @retail 0x175c70
void effects_dispose_from_old_map(void)
{
	g_51ec8c->valid = false;
	g_51ec88->valid = false;
	g_51ec84->valid = false;
	g_510c74->valid = false;
	g_4ea93c->valid = false;
	g_4ea938->valid = false;
}

PRIVATE __forceinline long data_scan_175cb0(s_record_pool *data, long index)
{
	if (index >= 0)
	{
		long count = data->high_water_index;
		if (index < count)
		{
			dword const *bits = data->bitmap;
			do
			{
				if (bits[index >> 5] & (1 << (index & 31)))
					return index;
				++index;
			} while (index < count);
		}
	}
	return NONE;
}

PRIVATE __forceinline s_effect_datum *data_step_175cb0(s_record_pool_iterator *iterator)
{
	s_record_pool *data = iterator->data;
	long index = data_scan_175cb0(data, iterator->index + 1);
	if (index == NONE)
		return 0;
	s_effect_datum *result = (s_effect_datum *)(data->data + data->size * index);
	iterator->index = index;
	iterator->datum_index = (result->salt << 16) | index;
	return result;
}

// @retail 0x175cb0
void effects_delete_all(void)
{
	struct
	{
		s_effect_datum *current;
		s_record_pool_iterator cursor;
	} iterator;
	iterator.cursor.data = g_4ea93c;
	iterator.cursor.index = NONE;

	for (;;)
	{
		iterator.current = data_step_175cb0(&iterator.cursor);
		if (!iterator.current)
			break;
		function_1774f0(iterator.cursor.datum_index);
	}
	function_x3bc618();
	function_x496c75();
}

// @retail 0x175d70
void function_175d70(void)
{
	s_record_pool *array = g_4ea93c;
	long datum = data_datum_index(array, function_16bc00(array, 0));
	while (datum != NONE)
	{
		s_effect_datum *effect = DATUM(array, s_effect_datum, datum);
		if (effect->object_index != NONE)
		{
			s_effect_object *object = (s_effect_object *)function_badc0(effect->object_index, (dword)NONE);
			if (!object)
			{
				effect->object_index = NONE;
				effect->location.leaf_index = NONE;
				effect->location.cluster_index = NONE;
				effect->location.bsp_index = g_4686c4;
				function_1774f0(datum);
			}
			else
			{
				effect->location = object->location;
				long index = effect->first_particle_system_index;
				while (index != NONE)
				{
					s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, index);
					index = system->next_index;
					system->set_location(&effect->location);
				}
			}
		}
		else if (function_1789f0(effect))
		{
			long index = effect->first_particle_system_index;
			while (index != NONE)
			{
				s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, index);
				index = system->next_index;
				system->set_location(&effect->location);
			}
		}
		else
			function_1774f0(datum);
		array = g_4ea93c;
		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
	particle_systems_update_locations();
}

// @retail 0x175ee0
void effect_parameters_initialize(s_effect_parameters *parameters)
{
	effect_parameters_initialize_inline(parameters);
}

// @retail 0x175f50
bool function_175f50(long object_index)
{
	bool result = false;
	s_effect_object *object = (s_effect_object *)function_badc0(object_index, 1);

	if (object && TEST_FIELD_BIT(object->flag10a_2))
	{
		result = object->unknown2e0 != NONE && object->unknown2e0 + g_510c54->field_2_3 < g_510c54->game_time;
	}
	return result;
}

// @retail 0x176330
s_effect_marker *function_176330(s_effect_marker *markers, point3f const *point)
{
	markers[0].forward = *g_4687bc;
	markers[0].position = *point;
	markers[0].name = 0x70000c0;
	markers[1].forward = *g_4687b0;
	markers[1].position = *point;
	markers[1].name = 0x20000ca;
	return markers;
}

// @retail 0x1763a0
void function_1763a0(point3f const *point, vector3f const *direction, s_effect_marker *markers, vector3f const *normal)
{
	markers[0].position = *point;
	markers[0].name = 0x60000b8;
	markers[1].position = *point;
	markers[1].name = 0x700054c;
	markers[2].position = *point;
	markers[2].name = 0x8000550;
	markers[3].position = *point;
	markers[3].name = 0xa0000bf;
	markers[4].position = *point;
	markers[4].name = 0x70000c0;
	markers[5].position = *point;
	markers[5].name = 0x20000ca;

	markers[0].forward = *normal;
	markers[1].forward = *direction;
	markers[2].forward.i = direction->i * -1.0f;
	markers[2].forward.j = direction->j * -1.0f;
	markers[2].forward.k = direction->k * -1.0f;

	vector3f *incident = &markers[1].forward;
	vector3f *surface = &markers[0].forward;
	real twice = (incident->j * surface->j + incident->k * surface->k + incident->i * surface->i) * 2.0f;

	markers[3].forward.i = incident->i - surface->i * twice;
	markers[3].forward.j = incident->j - twice * surface->j;
	markers[3].forward.k = incident->k - surface->k * twice;

	real magnitude = (real)sqrt(incident->i * incident->i + incident->j * incident->j + incident->k * incident->k);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real inverse = 1.0f / magnitude;

		incident->i = inverse * incident->i;
		incident->j = inverse * incident->j;
		incident->k = inverse * incident->k;
	}
	markers[0].forward = *normal;
	markers[4].forward = *g_4687bc;
	markers[5].forward = *g_4687b0;
}

// @retail 0x176a50
void function_176a50(s_effect_parameters *parameters, long tag_index, long marker_count, s_effect_marker *markers, long mode)
{
	memset(parameters, 0, sizeof(*parameters));
	parameters->unknown34 = 0;
	parameters->unknown38 = 0;
	parameters->unknown3c = 0;
	parameters->unknown30 = 0;
	parameters->source = 0;
	parameters->flags = 0;
	parameters->tag_index = tag_index;
	parameters->unknown18 = NONE;
	parameters->object_index = NONE;
	effect_owner_set_none(&parameters->owner);
	parameters->markers = markers;
	parameters->color_a = 0xff808080;
	parameters->color_b = 0xff808080;
	parameters->scale_a = 1.0f;
	parameters->scale_b = 1.0f;
	parameters->marker_count = marker_count;
	if (mode == 1)
		parameters->flags = 4;
}

// @retail 0x177260
void function_177260(long effect_index, bool flag)
{
	bool const *flag_reference = &flag;
	flag = *flag_reference;
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		if (effect->looping_sound_index != NONE)
		{
			s_effect_looping_sound *sound = DATUM(g_4ed28c, s_effect_looping_sound, effect->looping_sound_index);
			sound->flag1 = true;
			effect->looping_sound_index = NONE;
		}
		function_1771a0(effect);
		if (((byte)effect->flags >> 1) & 1)
		{
			if (flag)
				effect->flag4 = true;
			else
				effect->flag4 = false;
			effect->flag2 = true;
		}
		else
		{
			function_177610(effect_index);
		}
	}
}

// @retail 0x1771a0
void function_1771a0(s_effect_datum *effect)
{
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	if (TEST_FIELD_BIT(definition->flag5))
	{
		for (long i = 0; i < definition->event_count; i++)
		{
			s_effect_event *event = &definition->events[i];

			for (long j = 0; j < event->part_count; j++)
			{
				s_effect_part *part = &event->parts[j];

				if (part->group_tag == 'tdtl' && part->tag_index != NONE)
					function_c40f0(part->tag_index, effect->object_index, 0.0f);
			}
		}
	}
}

// @retail 0x177310
void function_177310(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	effect->flag2 = false;
	function_1782a0(effect_index, 0);
	if (definition->looping_sound_tag_index != NONE && definition->looping_sound_location != NONE && effect->looping_sound_index == NONE)
	{
		long location_index = effect->location_indices[definition->looping_sound_location];

		if (effect_location_next(effect, &location_index, 3))
			effect->looping_sound_index = function_18a750(definition->looping_sound_tag_index, effect_index);
	}
}

// @retail 0x1773a0
void function_1773a0(long effect_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

		for (long i = 0; i < definition->location_count; i++)
		{
			long index = effect->location_indices[i];

			while (index != NONE)
			{
				long next_index = DATUM(g_4ea938, s_effect_location_datum, index)->next_index;

				record_pool_release(g_4ea938, index);
				index = next_index;
			}
			effect->location_indices[i] = NONE;
		}
	}
}

// @retail 0x177460
void function_177460(s_effect_datum *effect)
{
	long index = effect->last_particle_system_index;

	while (index != NONE)
	{
		s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, index);
		long previous_index = particle_system->previous_index;

		particle_system_unlink(particle_system, &effect->first_particle_system_index, &effect->last_particle_system_index);
		function_174180(index);
		index = previous_index;
	}
	effect->first_particle_system_index = NONE;
	effect->last_particle_system_index = NONE;
	function_178ad0(effect);
}

// @retail 0x1774f0
void function_1774f0(long effect_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		function_177590(effect_index);
		effect_stop_looping_sound(effect);
		function_1771a0(effect);
		function_1773a0(effect_index);
		function_177460(effect);
		record_pool_release(g_4ea93c, effect_index);
	}
}

// @retail 0x177590
void function_177590(long effect_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		s_effect_object *object = (s_effect_object *)function_badc0(effect->object_index, NONE);

		if (object)
		{
			long count = (long)((dword)(long)object->attachments_size / sizeof(s_effect_attachment));
			s_effect_attachment *attachments = (s_effect_attachment *)((byte *)object + object->attachments_offset);

			for (long i = 0; i < count; i++)
			{
				if (attachments[i].index == effect_index && attachments[i].type == 2)
				{
					attachments[i].index = NONE;
					return;
				}
			}
		}
	}
}

// @retail 0x177610
bool function_177610(long effect_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);
	bool result = true;

	if (effect)
	{
		function_177590(effect_index);
		effect_stop_looping_sound(effect);
		if (effect->first_particle_system_index == NONE)
		{
			function_1774f0(effect_index);
		}
		else
		{
			for (long index = effect->last_particle_system_index; index != NONE; )
			{
				s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, index);

				particle_system->flag0 = false;
				index = particle_system->previous_index;
			}
			effect->flag2 = true;
			effect->flag6 = true;
			result = false;
		}
	}
	return result;
}

__declspec(noinline) dword *function_177c20(long tag_index);

// @retail 0x177c20
dword *function_177c20(long tag_index)
{
	if (TEST_FIELD_BIT(TAG_GET(s_effect_definition, tag_index)->flag2))
		return &g_4e7408->unknown0;
	return &g_4e7408->seed;
}

// @retail 0x178060
bool function_178060(void)
{
	s_record_pool *array = g_4ea93c;
	bool result = false;
	long datum = data_datum_index(array, function_16bc00(array, 0));

	while (datum != NONE)
	{
		s_effect_datum *effect = DATUM(array, s_effect_datum, datum);

		if (!TEST_FIELD_BIT(TAG_GET(s_effect_definition, effect->tag_index)->flag2))
		{
			function_1774f0(datum);
			result = true;
			break;
		}
		datum = data_datum_index(array, function_16bc00(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
	return result;
}

// @retail 0x178120
long function_178120(s_effect_owner const *owner, bool force, long tag_index)
{
	long effect_index = NONE;

	if (tag_index != NONE)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, tag_index);

		if ((force || !TEST_FIELD_BIT(definition->flag2)) && definition->event_count > 0)
		{
			s_record_pool *effects = g_4ea93c;

			effect_index = record_pool_allocate(effects);
			if (effect_index == NONE)
			{
				if (TEST_FIELD_BIT(definition->flag2) && function_178060())
					effect_index = record_pool_allocate(effects);
			}
			if (effect_index != NONE)
			{
				s_effect_datum *effect = DATUM(effects, s_effect_datum, effect_index);

				effect->tag_index = tag_index;
				if (owner)
				{
					effect->owner = *owner;
				}
				else
				{
					effect->flag8 = true;
					effect_owner_set_none(&effect->owner);
				}
				function_178ad0(effect);
				effect->looping_sound_index = NONE;
				effect->object_index = NONE;
				effect->unknown58 = NONE;
				effect->flags = 0;
				effect->first_particle_system_index = NONE;
				effect->last_particle_system_index = NONE;
				effect->color_a = 0xff808080;
				effect->color_b = 0xff808080;
				effect->unknown5e = NONE;
				effect->unknown74 = 0.0f;
				effect->unknown78 = 0.0f;
				function_1782a0(effect_index, 0);
			}
		}
	}
	return effect_index;
}

// @retail 0x178240
void function_178240(point3f const *origin, vector3f const *direction, s_effect_datum *effect, real scale_a, real scale_b)
{
	effect->scale_a = scale_a;
	effect->scale_b = scale_b;
	if (!origin)
		origin = g_468710;
	effect->origin = *origin;
	if (direction)
	{
		effect->direction = *direction;
	}
	else
	{
		effect->unknown40 = 0;
		effect->unknown44 = 0;
	}
}

// @retail 0x1782a0
void function_1782a0(long effect_index, short event_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

		if (event_index >= 0 && event_index < definition->event_count)
		{
			s_effect_event *event;

			effect->flag0 = false;
			effect->event_index = event_index;
			effect->unknown60 = 0.0f;
			event = &definition->events[event_index];
			if (event->delay_lower == event->delay_upper)
				effect->event_delay = event->delay_lower;
			else
				effect->event_delay = function_259d0(function_177c20(effect->tag_index), __FILE__, __LINE__, event->delay_lower, event->delay_upper);
			function_17add0(effect);
		}
	}
}

// @retail 0x1789f0
bool function_1789f0(s_effect_datum *effect)
{
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	s_effect_location_datum *location = 0;

	for (long i = 0; i < definition->location_count; i++)
	{
		long location_index = effect->location_indices[i];

		if (location_index != NONE)
		{
			location = effect_location_next(effect, &location_index, 0);
			break;
		}
	}
	if (location)
	{
		effect_location_from_point(&effect->location, &location->matrix.position);
	}
	else
	{
		effect->location.bsp_index = g_4686c4;
		effect->location.leaf_index = NONE;
		effect->location.cluster_index = NONE;
	}
	return effect->location.cluster_index != NONE;
}

// @retail 0x178ad0
void function_178ad0(s_effect_datum *effect)
{
	s_effect_event_slot *slot = effect->event_slots;

	for (long i = 16; i; i--, slot++)
	{
		slot->unknown0 = 0;
		slot->unknown4 = NONE;
	}
}

// @retail 0x178af0
bool function_178af0(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	bool result = false;

	for (dword i = 0; i < 16; i++)
	{
		if (effect->event_slots[i].unknown4 == NONE && effect->event_slots[i].unknown0 == 0)
		{
			result = true;
			break;
		}
	}
	return result;
}

// @retail 0x178b30
void function_178b30(long effect_index, long unknown0, long unknown4)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

	for (dword i = 0; i < 16; i++)
	{
		s_effect_event_slot *slot = &effect->event_slots[i];

		if (slot->unknown4 == NONE && slot->unknown0 == 0)
		{
			slot->unknown4 = unknown4;
			slot->unknown0 = unknown0;
			return;
		}
	}
}

// @retail 0x178b80
void effect_remove_event_slot(long effect_index, long value)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

	for (dword i = 0; i < 16; i++)
	{
		s_effect_event_slot *slot = &effect->event_slots[i];

		if (slot->unknown4 == value)
		{
			slot->unknown4 = NONE;
			slot->unknown0 = 0;
			return;
		}
	}
}

// @retail 0x1794a0
bool function_1794a0(long object_index, long other_object_index)
{
	bool result = false;

	if (((s_effect_object_header *)g_4e0300->data)[object_index & 0xffff].flags & 1)
	{
		if (!object_or_parent_hidden(other_object_index))
			result = true;
	}
	return result;
}

// @retail 0x17add0
void function_17add0(s_effect_datum *effect)
{
	long index = effect->first_particle_system_index;

	while (index != NONE)
	{
		s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, index);

		if (particle_system->event_index == effect->event_index)
		{
			real delay = effect->event_delay;

			particle_system->flag0 = true;
			particle_system->unknown04 = 0.0f;
			if (delay > 0.0f)
				particle_system->unknown08 = 1.0f / delay;
			else
				particle_system->unknown08 = 1.0f;
		}
		else if (particle_system->event_index == effect->unknown5e)
		{
			particle_system->flag0 = false;
		}
		index = particle_system->next_index;
	}
}

// @retail 0x17ae50
s_effect_location_datum *__stdcall effect_location_next(s_effect_datum *effect, long *location_index, short mode)
{
	s_effect_location_datum *location = 0;

	if (*location_index != NONE)
	{
		location = DATUM(g_4ea938, s_effect_location_datum, *location_index);
		*location_index = location->next_index;
		bool on_first_person = location->node_index != NONE && (location->node_index & 0x8000);
		bool wanted = false;

		switch (mode)
		{
		case 1:
			wanted = true;
			break;
		case 3:
			goto done;
		}
		if (on_first_person != wanted)
			location = effect_location_next(effect, location_index, mode);
	}
done:
	return location;
}

__declspec(noinline) void function_17aec0(s_effect_datum *effect, transform4x3f *matrix, short node_index);

// @retail 0x17aec0
void function_17aec0(s_effect_datum *effect, transform4x3f *matrix, short node_index)
{
	if (node_index != NONE && (node_index & 0x8000) && effect->unknown58 != NONE)
	{
		function_1664da(effect->unknown58, effect->object_index, node_index & 0x7fff, matrix);
	}
	else
	{
		*matrix = *effect_object_node_matrix(effect->object_index, node_index == NONE ? NONE : (short)(node_index & 0x7fff));
	}
}

// @retail 0x17af30
void function_17af30(transform4x3f *matrix, s_effect_datum *effect, bool field_b4, short node_index)
{
	dword node = (word)node_index;
	if (field_b4 && (short)node != NONE && (node & 0x8000) && effect->unknown58 != NONE)
		*matrix = *function_1664a5(effect->unknown58, effect->object_index, node & 0x7fff);
	else
		function_17aec0(effect, matrix, (short)node);
}

__declspec(noinline) void function_17af80(long effect_index, long particle_system_index);

// @retail 0x17af80
void function_17af80(long effect_index, long particle_system_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);

	particle_system_unlink(particle_system, &effect->first_particle_system_index, &effect->last_particle_system_index);
	effect_remove_event_slot(effect_index, particle_system_index);
}

// @retail 0x17afd0
bool function_17afd0(long effect_index, long value)
{
	bool result = false;
	s_effect_object *object = (s_effect_object *)function_badc0(DATUM(g_4ea93c, s_effect_datum, effect_index)->object_index, 3);

	if (object)
	{
		long player_index = object->player_index;

		if (player_index != NONE)
			result = DATUM(g_4e8c24, s_effect_player, player_index)->unknown28 == value;
	}
	return result;
}

PRIVATE __forceinline byte effect_placement_allowed_17b270(s_effect_datum const *effect, short placement)
{
	if (TEST_FIELD_BIT(effect->flag5))
		return placement != 1;
	return placement != 2;
}

// @retail 0x17b270
bool function_17b270(short const *values, s_effect_datum *effect, long mode)
{
	bool result = effect_placement_allowed_17b270(effect, values[8]) != 0;
	if (*(short *)&g_4e8c20->unknown00[8] == 1)
	{
		if (result && (values[9] != 2 || mode == 0) && (values[9] != 1 || mode == 1))
			return true;
		return false;
	}
	return result;
}

// @retail 0x17b750
void function_17b750(long *values, long value)
{
	for (long i = 0; i < 32; i++)
		values[i] = value;
}

// @retail 0x175fa0
long __stdcall effect_new_from_parameters(s_effect_parameters *parameters)
{
	bool force = TEST_FIELD_BIT(parameters->flag2);
	long effect_index = NONE;

	if (force || function_176210(parameters))
	{
		effect_index = function_178120(&parameters->owner, force, parameters->tag_index);
		if (effect_index != NONE)
		{
			s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
			s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

			effect->unknown0c = parameters->unknown30;
			if (g_4e6948->state == 2 && function_163080() && parameters->object_index != NONE)
			{
				s_effect_object *object = OBJECT_GET(parameters->object_index);

				effect->flag10 = TEST_FIELD_BIT(TAG_GET(s_effect_object_definition, object->tag_index)->flag13);
			}
			if (TEST_FIELD_BIT(parameters->attached))
			{
				long object_index = parameters->object_index;
				long group_index = function_166244(object_index);

				if (function_badc0(object_index, NONE))
					effect->object_index = object_index;
				if (group_index != NONE && function_166283(group_index, parameters->object_index))
					effect->unknown58 = group_index;
				effect->flag7 = true;
			}
			else
			{
				effect->object_index = NONE;
				effect->velocity = parameters->velocity;
			}
			if (TEST_FIELD_BIT(parameters->flag1))
			{
				effect->flag1 = true;
				effect->unknown10 = parameters->unknown34;
				effect->unknown14 = parameters->unknown38;
				effect->unknown18 = parameters->unknown3c;
				effect->unknown40 = 0;
				effect->unknown44 = 0;
			}
			else
			{
				function_178240(parameters->origin, parameters->direction, effect, parameters->scale_a, parameters->scale_b);
			}
			if (effect->object_index != NONE && g_510c7c && function_175f50(effect->object_index))
				effect->flag5 = true;
			function_17b750(effect->location_indices, NONE);
			function_178360(effect_index, parameters->unknown18, effect->object_index, effect->unknown58, parameters->markers, parameters->marker_count);
			if (!TEST_FIELD_BIT(parameters->attached))
				function_1789f0(effect);
			if (TEST_FIELD_BIT(definition->flag3) || TEST_FIELD_BIT(definition->flag4))
				function_17b5d0(effect, parameters->object_index, parameters, true);
			if (parameters->unknown50)
			{
				*(long *)&effect->unknown74 = parameters->unknown50[0];
				*(long *)&effect->unknown78 = parameters->unknown50[1];
			}
			function_177310(effect_index);
			g_510c78 = parameters->source;
			function_179850(effect_index, (TEST_FIELD_BIT(definition->flag1) && !TEST_FIELD_BIT(effect->flag1) && !TEST_FIELD_BIT(effect->flag9)) ? 1.0f : 0.0f);
			g_510c78 = 0;
			if (!record_pool_lookup(g_4ea93c, effect_index))
				return NONE;
		}
	}
	return effect_index;
}

// @retail 0x176210
bool function_176210(s_effect_parameters *parameters)
{
	bool result = true;

	if (parameters->tag_index != NONE)
	{
		s_effect_source *source = parameters->source;

		if (source && source->index != NONE && !(g_510c50 && ((byte *)g_510c50)[5]))
		{
			s_effect_definition *definition = TAG_GET(s_effect_definition, parameters->tag_index);
			real lower = definition->distance_lower;
			real upper = definition->distance_upper;

			if (lower != 0.0f && upper != 0.0f)
			{
				real minimum = 3.4028235e38f;

				for (long i = 0; i < 4; i++)
				{
					s_player_state *player = function_16f3a0(i);

					if (player)
					{
						real dx = source->position.x - player->position.x;
						real dy = source->position.y - player->position.y;
						real dz = source->position.z - player->position.z;
						real distance_squared = dx * dx + dy * dy + dz * dz;

						if (minimum > distance_squared)
							minimum = distance_squared;
					}
				}
				if (!(lower * lower > minimum))
				{
					if (minimum > upper * upper)
						result = false;
					else
						result = (g_4c56c0[source->index >> 5] & (1 << (source->index & 31))) != 0;
				}
			}
		}
	}
	return result;
}

// @retail 0x1765e0
long function_1765e0(point3f const *point, vector3f const *direction, vector3f const *normal, long tag_index, long mode, long deterministic)
{
	if (tag_index != NONE && point)
	{
		s_effect_marker markers[7];
		s_effect_parameters parameters;
		long marker_count = 6;

		function_1763a0(point, direction, markers, normal);
		if (mode == 1)
		{
			markers[6].position = *point;
			markers[6].forward = *direction;
			markers[6].name = 0x30000d9;
			marker_count = 7;
		}
		parameters.flags = 0;
		function_176a50(&parameters, tag_index, marker_count, markers, deterministic);
		effect_new_from_parameters(&parameters);
	}
	return NONE;
}

// @retail 0x1766b0
long function_1766b0(long object_index, long tag_index, long unknown34, long unknown38, short unknown3c)
{
	s_effect_parameters parameters;
	point3f point;
	s_effect_marker markers[2];

	effect_parameters_initialize_inline(&parameters);
	parameters.tag_index = tag_index;
	parameters.object_index = object_index;
	parameters.attached = true;
	parameters.flag1 = true;
	parameters.flag2 = true;
	parameters.unknown34 = unknown34;
	parameters.unknown38 = unknown38;
	parameters.unknown3c = unknown3c;
	function_b9dd0(object_index, &point);
	parameters.markers = function_176330(markers, &point);
	parameters.marker_count = 2;
	return effect_new_from_parameters(&parameters);
}

// @retail 0x176780
long function_176780(long object_index, s_effect_owner const *owner, real scale_a, long tag_index, real scale_b, point3f const *origin, vector3f const *direction)
{
	s_effect_parameters parameters;
	point3f point;
	s_effect_marker markers[2];

	effect_parameters_initialize_inline(&parameters);
	parameters.tag_index = tag_index;
	parameters.object_index = object_index;
	parameters.attached = true;
	parameters.flag2 = true;
	parameters.scale_a = scale_a;
	parameters.scale_b = scale_b;
	parameters.origin = origin;
	parameters.direction = direction;
	function_b9dd0(object_index, &point);
	parameters.markers = function_176330(markers, &point);
	parameters.marker_count = 2;
	if (owner)
		parameters.owner = *owner;
	return effect_new_from_parameters(&parameters);
}

// @retail 0x176870
void function_176870(s_effect_owner const *owner, long object_index, long marker_name, real scale_a, long tag_index, short unknown18, real scale_b, point3f const *origin, vector3f const *direction)
{
	s_effect_parameters parameters;
	s_effect_marker markers[2];
	s_effect_object_marker object_markers[1];

	effect_parameters_initialize_inline(&parameters);
	parameters.attached = true;
	parameters.flag2 = true;
	parameters.tag_index = tag_index;
	if (owner)
		parameters.owner = *owner;
	parameters.object_index = object_index;
	parameters.unknown18 = unknown18;
	parameters.unknown30 = marker_name;
	parameters.scale_a = scale_a;
	parameters.scale_b = scale_b;
	parameters.origin = origin;
	parameters.direction = direction;
	function_b8d30(object_index, marker_name, (s_object_marker *)object_markers, 1, false);
	parameters.markers = function_176330(markers, &object_markers[0].unknown38.position);
	parameters.marker_count = 2;
	effect_new_from_parameters(&parameters);
}

// @retail 0x176970
void function_176970(s_effect_owner const *owner, real scale_a, long tag_index, long object_index, short unknown18, short marker_count, s_effect_marker *markers, real scale_b, point3f const *origin, vector3f const *direction, bool flag1)
{
	s_effect_parameters parameters;

	effect_parameters_initialize_inline(&parameters);
	parameters.flags = 5;
	parameters.tag_index = tag_index;
	if (owner)
		parameters.owner = *owner;
	parameters.object_index = object_index;
	parameters.unknown18 = unknown18;
	parameters.origin = origin;
	parameters.direction = direction;
	parameters.marker_count = marker_count;
	parameters.scale_a = scale_a;
	parameters.scale_b = scale_b;
	parameters.markers = markers;
	if (flag1)
		parameters.flags = 7;
	effect_new_from_parameters(&parameters);
}

// @retail 0x176ad0
void function_176ad0(long marker_count, s_effect_marker *markers, s_effect_owner const *owner, vector3f const *velocity, long tag_index, long unknown30, real scale_a, real scale_b, point3f const *origin, vector3f const *direction, long mode)
{
	s_effect_parameters parameters;

	parameters.flags = 0;
	function_176a50(&parameters, tag_index, marker_count, markers, mode);
	if (owner)
		parameters.owner = *owner;
	if (velocity)
		parameters.velocity = *velocity;
	parameters.origin = origin;
	parameters.scale_a = scale_a;
	parameters.scale_b = scale_b;
	parameters.direction = direction;
	parameters.unknown30 = unknown30;
	effect_new_from_parameters(&parameters);
}

// @retail 0x176b60
void function_176b60(long marker_count, s_effect_marker *markers, long mode, s_effect_owner const *owner, long tag_index)
{
	s_effect_parameters parameters;

	parameters.flags = 0;
	function_176a50(&parameters, tag_index, marker_count, markers, mode);
	if (owner)
		parameters.owner = *owner;
	effect_new_from_parameters(&parameters);
}


// @retail 0x178020
bool function_178020(short placement, point3f const *point, s_location const *location)
{
	bool result;
	switch (placement)
	{
	case 0:
		result = true;
		break;
	case 1:
		result = !function_11c120(location, point, 0);
		break;
	case 2:
		result = function_11c120(location, point, 0);
		break;
	case 3:
		result = false;
		break;
	default:
		__assume(0);
	}
	return result;
}

/* the markers an effect is started at (function_178360) */
struct s_effect_marker_source
{
	short node_index;
	transform4x3f *node_matrix;
	long marker_count;
	s_effect_marker *markers;
};

static inline long effect_marker_find(s_effect_marker const *markers, long marker_count, dword name)
{
	long result = NONE;

	for (long i = 0; i < marker_count; i++)
	{
		if (markers[i].name == name)
		{
			result = i;
			break;
		}
	}
	return result;
}

// @retail 0x178360
void function_178360(long effect_index, short unknown18, long object_index, long unknown58, s_effect_marker *markers, long marker_count)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	long default_marker_index = effect_marker_find(markers, marker_count, 0x30000d9);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	s_effect_marker_source source;
	s_effect_object_marker object_markers[16];

	function_1773a0(effect_index);
	source.marker_count = marker_count;
	source.markers = markers;
	if (object_index != NONE)
	{
		s_effect_object *object = OBJECT_GET(object_index);

		source.node_index = unknown18 == NONE ? 0 : unknown18;
		source.node_matrix = &((transform4x3f *)((byte *)object + object->nodes_offset))[source.node_index];
	}
	else
	{
		source.node_index = NONE;
		source.node_matrix = 0;
	}
	for (long location_index = 0; location_index < definition->location_count; location_index++)
	{
		dword name = definition->locations[location_index];
		long count = 0;
		dword first_person_mask = 0;

		if (name == 0x700054e)
			name = effect->unknown0c;
		if (name)
		{
			long marker_index = NONE;

			if (marker_count > 0)
				marker_index = effect_marker_find(markers, marker_count, name);
			if (marker_index != NONE)
			{
				function_1786f0(&source, object_markers, (short)marker_index);
				count = 1;
			}
			else
			{
				if (object_index != NONE)
				{
					count = function_b8d30(object_index, name, (s_object_marker *)object_markers, 16, false);
					if (count == 0 && name == 0x400054f)
						count = 1;
					if (unknown58 != NONE)
					{
						short first_person_count = first_person_weapon_get_markers_internal(unknown58, name, (s_first_person_marker *)&object_markers[count], (short)(16 - count));

						for (long i = 0; i < first_person_count; i++)
							first_person_mask |= 1 << (i + count);
						count += first_person_count;
					}
				}
				if (count == 0 && default_marker_index != NONE)
				{
					function_1786f0(&source, object_markers, (short)default_marker_index);
					count = 1;
				}
			}
		}
		for (long i = 0; i < count; i++)
		{
			if (function_1785c0(&object_markers[i], effect, (short)location_index, (first_person_mask & (1 << i)) != 0) == NONE)
				break;
		}
	}
}

/* the last time effect locations ran out */
long g_47ff8c;

// @retail 0x1785c0
long function_1785c0(s_effect_object_marker const *marker, s_effect_datum *effect, short location_index, bool field_b4)
{
	s_record_pool *locations = g_4ea938;
	long index = record_pool_allocate(locations);

	if (index != NONE)
	{
		s_effect_location_datum *location = DATUM(locations, s_effect_location_datum, index);
		short node_index = marker->node_index;

		location->node_index = node_index == NONE ? NONE : ((node_index & 0x7fff) | (field_b4 ? 0x8000 : 0));
		location->matrix = marker->matrix;
		if (function_3eb20(effect->location.cluster_index) && effect->object_index != NONE)
		{
			s_effect_object *object = OBJECT_GET(effect->object_index);
			point3f const &position = object->position;
			vector3f offset;

			offset.i = g_468788->x - position.x;
			offset.j = g_468788->y - position.y;
			offset.k = g_468788->z - position.z;
			function_3ebd0(&offset, &location->matrix, &location->matrix, 1);
		}
		location->next_index = effect->location_indices[location_index];
		effect->location_indices[location_index] = index;
	}
	else
	{
		long game_time = g_510c54->game_time;

		if (game_time - g_47ff8c > g_510c54->field_2_3 * 60)
			g_47ff8c = game_time;
	}
	return index;
}

// @retail 0x1786f0
void function_1786f0(s_effect_marker_source const *source, s_effect_object_marker *out, short marker_index)
{
	transform4x3f const *matrix = source->node_matrix;
	point3f position;
	vector3f forward;
	vector3f up;

	out->node_index = source->node_index;
	if (matrix)
	{
		s_effect_marker const *marker = &source->markers[marker_index];

		if (matrix->scale != 0.0f)
		{
			vector3f offset;

			offset.i = marker->position.x - matrix->position.x;
			offset.j = marker->position.y - matrix->position.y;
			offset.k = marker->position.z - matrix->position.z;
			if (matrix->scale != 1.0f)
			{
				real inverse = 1.0f / matrix->scale;

				offset.i = inverse * offset.i;
				offset.j = inverse * offset.j;
				offset.k = inverse * offset.k;
			}
			up.i = matrix->forward.k * offset.k + matrix->forward.j * offset.j + matrix->forward.i * offset.i;
			up.j = matrix->left.k * offset.k + matrix->left.j * offset.j + matrix->left.i * offset.i;
			up.k = matrix->up.k * offset.k + matrix->up.j * offset.j + matrix->up.i * offset.i;
		}
		else
		{
			up.i = 0.0f;
			up.j = 0.0f;
			up.k = 0.0f;
		}
		position.x = up.i;
		position.y = up.j;
		position.z = up.k;
		forward.i = matrix->forward.k * marker->forward.k + matrix->forward.j * marker->forward.j + matrix->forward.i * marker->forward.i;
		forward.j = matrix->left.k * marker->forward.k + matrix->left.j * marker->forward.j + matrix->left.i * marker->forward.i;
		forward.k = matrix->up.k * marker->forward.k + matrix->up.j * marker->forward.j + matrix->up.i * marker->forward.i;
	}
	else
	{
		s_effect_marker const *marker = &source->markers[marker_index];

		position = marker->position;
		forward = marker->forward;
	}
	function_11d000(&forward, &up);

	real magnitude = (real)sqrt(up.k * up.k + up.j * up.j + up.i * up.i);
	if (!(fabs(magnitude) < 0.0001f))
	{
		real inverse = 1.0f / magnitude;

		up.i = inverse * up.i;
		up.j = up.j * inverse;
		up.k = up.k * inverse;
	}
	out->matrix.scale = 1.0f;
	out->matrix.forward = forward;
	out->matrix.left.i = up.j * forward.k - up.k * forward.j;
	out->matrix.left.j = up.k * forward.i - forward.k * up.i;
	out->matrix.left.k = forward.j * up.i - up.j * forward.i;
	out->matrix.up = up;
	out->matrix.position.x = 0.0f;
	out->matrix.position.y = 0.0f;
	out->matrix.position.z = 0.0f;
	out->matrix.position = position;
}

// @retail 0x178bc0
transform4x3f *function_178bc0(s_effect_location_datum *location, s_effect_datum *effect, transform4x3f *matrix, bool field_b4)
{
	if ((word)location->node_index != 0xffff && effect->object_index != NONE)
	{
		transform4x3f node_matrix;

		function_17af30(&node_matrix, effect, field_b4, (word)location->node_index);
		function_142a60(&node_matrix, &location->matrix, matrix);
		if (function_3eb20(effect->location.cluster_index) && effect->object_index != NONE)
		{
			point3f const &origin = *g_468788;
			s_effect_object *object = OBJECT_GET(effect->object_index);
			point3f const &position = object->position;
			vector3f offset;

			offset.i = origin.x - position.x;
			offset.j = origin.y - position.y;
			offset.k = origin.z - position.z;
			function_3ebd0(&offset, matrix, matrix, 1);
		}
		return matrix;
	}
	return &location->matrix;
}

/* whether a point is inside the zone of a cluster (an inline copy of
   function_11c120) */
static inline bool effect_location_in_zone(s_location const *location, point3f const *point)
{
	bool result = false;
	short cluster_index = location->cluster_index;

	if (cluster_index != NONE)
	{
		s_effect_zone_bsp *bsp = (s_effect_zone_bsp *)g_4e0348;
		byte zone = bsp->clusters[cluster_index].zone;

		if (zone != 0xff)
		{
			s_effect_zone *entry = &bsp->zones[zone & 0x7f];

			if (entry->index != NONE)
			{
				if (!(zone & 0x80) || 0.0f > entry->plane.k * point->z + entry->plane.j * point->y + entry->plane.i * point->x - entry->plane.d)
					result = true;
			}
		}
	}
	return result;
}

// @retail 0x178c80
void function_178c80(real scale, s_effect_datum *effect, s_particle_system_datum *particle_system, s_effect_particle_system_definition *definition, real unknown, bool field_b4)
{
	real *scale_reference = &scale;
	long location_index = effect->location_indices[definition->location_index];
	s_effect_location_datum *location;
	s_particle_system_spawn spawn;

	if (effect->location.cluster_index != NONE)
		particle_system->set_location(&effect->location);
	dword color_a = effect->color_a;

	spawn.scale = (*scale_reference);
	spawn.unknown = unknown;
	spawn.location_index = particle_system->location_index;
	{
		c_type_4e7709 *particle_definition = function_137bd0(particle_system->function_1751d0()->tag_index);
		bool tinted = particle_definition->tinted();
		bool multiplied = particle_definition->multiplied();

		function_175a80(tinted, color_a, effect->color_b, particle_system, multiplied);
	}
	while ((location = effect_location_next(effect, &location_index, definition->location_mode)) != 0)
	{
		bool spawn_here;

		switch (definition->placement)
		{
		case 0:
			spawn_here = true;
			break;
		case 1:
			spawn_here = !effect_location_in_zone(&effect->location, &location->matrix.position);
			break;
		case 2:
			spawn_here = effect_location_in_zone(&effect->location, &location->matrix.position);
			break;
		case 3:
			spawn_here = false;
			break;
		default:
			__assume(0);
		}
		if (spawn_here)
		{
			transform4x3f matrix;
			transform4x3f *location_matrix = function_178bc0(location, effect, &matrix, field_b4);
			bool node_first_person = location->node_index != NONE && (location->node_index & 0x8000);

			function_175270(particle_system, &spawn, location_matrix, node_first_person);
		}
	}
}


// @retail 0x178eb0
s_particle_system_datum *function_178eb0(short definition_index, long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_event *event = &TAG_GET(s_effect_definition, effect->tag_index)->events[effect->event_index];
	s_effect_particle_system_definition *definition = &event->particle_systems[definition_index];
	long mode = 0;

	if (effect->unknown58 != NONE && !function_155760(effect->unknown58))
		mode = 1;
	if (function_17b270(&definition->unknown10, effect, mode))
	{
		long location_index = definition->location_index;

		if (location_index != NONE)
		{
			long first_location_index = effect->location_indices[location_index];
			s_effect_location_datum *location = effect_location_next(effect, &first_location_index, definition->location_mode);

			if (location && function_178020(definition->placement, &location->matrix.position, &effect->location))
			{
				long particle_system_index = function_173fd0(definition, effect_index, effect->tag_index, definition_index, effect->event_index);

				if (particle_system_index != NONE)
				{
					s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);
					real delay = effect->event_delay;

					particle_system->flag0 = true;
					particle_system->unknown04 = 0.0f;
					if (delay > 0.0f)
						particle_system->unknown08 = 1.0f / delay;
					else
						particle_system->unknown08 = 1.0f;
					particle_system_link(particle_system, &effect->last_particle_system_index, &effect->first_particle_system_index);
					particle_system->unknown24 = location_index;
					function_178c80(0.0f, effect, particle_system, definition, 0.0f, definition->unknown0c != 0);
					return particle_system;
				}
			}
		}
	}
	return 0;
}

PRIVATE __forceinline bool effect_event_placement_allowed(s_effect_particle_system_definition const *definition, s_effect_datum *effect, long mode)
{
	bool result = effect_placement_allowed_17b270(effect, definition->unknown10) != 0;
	if (*(short *)&g_4e8c20->unknown00[8] == 1)
	{
		if (result && (definition->location_mode != 2 || mode == 0) && (definition->location_mode != 1 || mode == 1))
			return true;
		return false;
	}
	return result;
}

// @retail 0x179020
void function_179020(long effect_index, real scale, real unknown)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_event *event = &TAG_GET(s_effect_definition, effect->tag_index)->events[effect->event_index];
	volatile long mode = 0;

	if (effect->unknown58 != NONE && !function_155760(effect->unknown58))
		mode = 1;
	for (long i = 0; i < event->particle_system_count; i++)
	{
		s_effect_particle_system_definition *definition = &event->particle_systems[i];
		bool field_b4 = definition->unknown0c != 0;

		if (effect_event_placement_allowed(definition, effect, mode))
		{
			s_particle_system_datum *particle_system = 0;

			for (dword j = 0; j < 16; j++)
			{
				if ((s_effect_particle_system_definition *)effect->event_slots[j].unknown0 == definition)
				{
					particle_system = DATUM(g_510c74, s_particle_system_datum, effect->event_slots[j].unknown4);
					break;
				}
			}
			if (particle_system || (particle_system = function_178eb0((short)i, effect_index)) != 0)
			{
				if (definition->location_index != NONE)
					function_178c80(scale, effect, particle_system, definition, unknown, field_b4);
			}
		}
	}
}

// @retail 0x179190
long function_179190(s_effect_datum *effect)
{
	s_effect_datum *const volatile *effect_argument = &effect;
	effect = *effect_argument;
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	long restart_index = definition->restart_event_index;
	long last_index = definition->event_count - 1;
	long event_index = effect->event_index;

	if ((TEST_FIELD_BIT(effect->flag1) || TEST_FIELD_BIT(effect->flag9)) && restart_index == NONE)
		restart_index = 0;
	for (;;)
	{
		if ((TEST_FIELD_BIT(effect->flag1) || TEST_FIELD_BIT(effect->flag9)) && event_index == last_index && restart_index != NONE)
			event_index = restart_index;
		else
			event_index++;
		if (event_index >= definition->event_count)
			break;
		real skip_chance = definition->events[event_index].skip_chance;

		if (!(skip_chance > 0.0f))
			break;
		dword *seed = &g_4e7408->unknown0;
		if (!TEST_FIELD_BIT(TAG_GET(s_effect_definition, effect->tag_index)->flag2))
			seed = &g_4e7408->seed;
		if (!(skip_chance > function_x82e52f(seed, __FILE__, __LINE__)))
			break;
	}
	return event_index;
}

// @retail 0x1792b0
bool function_1792b0(long effect_index, real dt)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	if (!TEST_FIELD_BIT(effect->flag6))
	{
		long iterations = 0;

		while (dt >= 0.0f && !TEST_FIELD_BIT(effect->flag2) && iterations < 8)
		{
			real remaining = effect->event_delay - effect->unknown60;
			real start = effect->unknown60;
			real step = dt > remaining ? remaining : dt;
			bool finished;

			if (dt >= remaining)
			{
				effect->unknown60 = effect->event_delay;
				finished = true;
				dt -= remaining;
			}
			else
			{
				effect->unknown60 += dt;
				finished = false;
				dt = -1.0f;
			}
			if (TEST_FIELD_BIT(effect->flag0))
			{
				if (!TEST_FIELD_BIT(effect->flag3))
					function_179020(effect_index, start, step);
				if (!TEST_FIELD_BIT(effect->flag3))
					function_179fb0(effect);
				if (finished)
				{
					long event_index = function_179190(effect);

					if (event_index >= definition->event_count)
					{
						if (function_177610(effect_index))
							return true;
						break;
					}
					function_1782a0(effect_index, (short)event_index);
				}
			}
			else if (finished)
			{
				s_effect_event *event = &definition->events[effect->event_index];

				effect->flag0 = true;
				effect->unknown60 = 0.0f;
				effect->unknown68 = -1.0f;
				if (event->duration_lower == event->duration_upper)
					effect->event_delay = event->duration_lower;
				else
					effect->event_delay = function_259d0(function_177c20(effect->tag_index), __FILE__, __LINE__, event->duration_lower, event->duration_upper);
				if (!TEST_FIELD_BIT(effect->flag3))
					function_17a380(effect);
				function_17add0(effect);
				effect->unknown5e = effect->event_index;
			}
			iterations++;
		}
		effect->flag9 = false;
	}
	return effect == 0;
}

// @retail 0x1794e0
bool __stdcall function_1794e0(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	if (effect->object_index != NONE && !function_badc0(effect->object_index, NONE))
	{
		function_1774f0(effect_index);
		effect = 0;
	}
	else if (effect->object_index != NONE)
	{
		s_effect_object *root = OBJECT_GET(function_baf80(effect->object_index));

		if (TEST_FIELD_BIT(root->flag4_8))
		{
			effect->velocity = root->velocity;
			effect->location = root->location;
		}
		else
		{
			effect->location.leaf_index = NONE;
			effect->location.cluster_index = NONE;
			effect->location.bsp_index = g_4686c4;
		}
		if (TEST_FIELD_BIT(effect->flag1) || TEST_FIELD_BIT(effect->flag9))
		{
			bool visible;

			if (function_1794a0(effect->object_index, effect->object_index))
			{
				visible = !effect->unknown10 || function_bab40(effect->object_index, effect->unknown10, &effect->scale_a);
				function_bab40(effect->object_index, effect->unknown14, &effect->scale_b);
				if (effect->unknown18)
					function_bad50(effect->object_index, effect->unknown18, &effect->origin);
			}
			else
			{
				effect->scale_a = 0.0f;
				effect->scale_b = 0.0f;
				if (effect->unknown18)
					effect->origin = *g_468710;
				visible = false;
			}
			if (visible)
			{
				if (TEST_FIELD_BIT(effect->flag2))
				{
					if (!TEST_FIELD_BIT(effect->flag4))
						function_177310(effect_index);
					else if (function_177610(effect_index))
						return true;
				}
			}
			else if (TEST_FIELD_BIT(definition->flag0))
			{
				if (function_177610(effect_index))
					return true;
			}
			else if (!TEST_FIELD_BIT(effect->flag2))
			{
				function_177260(effect_index, false);
			}
		}
		if (TEST_FIELD_BIT(definition->flag3) || TEST_FIELD_BIT(definition->flag4))
		{
			long object_index = effect->object_index;

			if (function_badc0(object_index, NONE))
			{
				long index = function_3ddd0(function_baf80(object_index));

				if (index != NONE)
				{
					byte *datum = g_509434->data + (index & 0xffff) * 0x100;

					if (datum[2])
					{
						effect->color_a = *(dword *)(datum + 0x2c);
						effect->color_b = *(dword *)(datum + 0x28);
					}
				}
			}
		}
	}
	return effect == 0;
}

// @retail 0x179730
bool function_179730(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	bool hidden = true;

	if (effect->location.cluster_index != NONE)
	{
		if (function_3eb20(effect->location.cluster_index) && effect->object_index != NONE)
			hidden = false;
		else if (g_510c50 && ((byte *)g_510c50)[5])
			hidden = false;
		else if (TEST_FIELD_BIT(definition->flag2))
			hidden = !function_11c080(&effect->location);
		else
			hidden = !function_11c050(&effect->location);
	}
	if (hidden != TEST_FIELD_BIT(effect->flag3))
	{
		if (hidden)
		{
			effect->flag3 = true;
			if (!TEST_FIELD_BIT(effect->flag1) && !TEST_FIELD_BIT(effect->flag9) && function_177610(effect_index))
				effect = 0;
		}
		else
		{
			effect->flag3 = false;
		}
	}
	return effect == 0;
}

// @retail 0x179810
bool function_179810(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

	if (TEST_FIELD_BIT(effect->flag6) && effect->first_particle_system_index == NONE)
	{
		function_1774f0(effect_index);
		effect = 0;
	}
	return effect == 0;
}

// @retail 0x179850
void function_179850(long effect_index, real dt)
{
	if (!function_1794e0(effect_index) && !function_179730(effect_index) && !function_179810(effect_index))
		function_1792b0(effect_index, dt);
}

// @retail 0x179fb0
void function_179fb0(s_effect_datum *effect)
{
	s_effect_event *event = &TAG_GET(s_effect_definition, effect->tag_index)->events[effect->event_index];

	if (effect->object_index != NONE)
	{
		s_effect_object_tag *object_definition = TAG_GET(s_effect_object_tag, OBJECT_GET(effect->object_index)->tag_index);

		for (long i = 0; i < event->acceleration_count; i++)
		{
			s_effect_acceleration *acceleration = &event->accelerations[i];

			if (acceleration->location != NONE)
			{
				long location_index = effect->location_indices[acceleration->location];
				s_effect_location_datum *location;

				while ((location = effect_location_next(effect, &location_index, 0)) != 0)
				{
					point3f point;
					vector3f forward;
					vector3f direction;

					if (location->node_index != NONE)
					{
						transform4x3f matrix;

						function_17aec0(effect, &matrix, location->node_index);
						effect_matrix_transform_point(&matrix, &location->matrix.position, &point);
						effect_matrix_transform_normal(&matrix, &location->matrix.forward, &forward);
					}
					else
					{
						point = location->matrix.position;
						forward = location->matrix.forward;
					}
					random_vector_in_cone(&forward, &direction, &g_4e7408->unknown0, acceleration->inner_cone_angle, acceleration->outer_cone_angle);
					effect_normalize(&direction);

					real magnitude = acceleration->acceleration * object_definition->unknown14;
					s_effect_object *object = OBJECT_GET(effect->object_index);

					object->owner_unknown4 = effect->owner.unknown4;
					object->owner_unknown0 = effect->owner.unknown0;
					object->owner_unknown8 = effect->owner.unknown8;
					direction.i *= magnitude;
					direction.j *= magnitude;
					direction.k *= magnitude;
					function_b7880(effect->object_index, NONE, &point, &direction, NULL);
				}
			}
		}
	}
}

// @retail 0x177c50
real function_177c50(dword *seed, real lower, real upper, dword a_scales, dword b_scales, s_effect_datum *effect, long bit)
{
	real base = lower;

	if (a_scales & (1 << (byte)bit))
		base = effect->scale_a * lower;
	if (b_scales & (1 << (byte)bit))
		base = effect->scale_b * base;

	real range = upper - lower;

	if (a_scales & (1 << (bit + 1)))
		range = effect->scale_a * range;
	if (b_scales & (1 << (bit + 1)))
		range = effect->scale_b * range;
	return base + range * ((real)_random(seed, __FILE__, __LINE__) * (1.f / 65535.f));
}

// @retail 0x177d10
void function_177d10(s_effect_datum *effect, dword *seed, vector3f const *forward, vector3f *direction, vector3f *velocity, real lower, real upper, real cone_angle, dword a_scales, dword b_scales)
{
	real speed = function_177c50(seed, lower, upper, a_scales, b_scales, effect, 0);
	real angle = cone_angle;

	if (a_scales & 4)
		angle = effect->scale_a * angle;
	if (b_scales & 4)
		angle = effect->scale_b * angle;
	angle = angle * ((real)_random(seed, __FILE__, __LINE__) * (1.f / 65535.f));
	*direction = *forward;
	if (angle != 0.0f)
	{
		real s = (real)sin(angle);
		real c = (real)cos(angle);
		vector3f axis = g_4417f0[random_index(seed, 0x402)];
		real dot = direction->i * axis.i + direction->k * axis.k + direction->j * axis.j;
		real t = dot * (1.0f - c);
		real x = direction->i;
		real y = direction->j;
		real z = direction->k;

		direction->i = x * c + axis.i * t - (y * axis.k - z * axis.j) * s;
		direction->j = y * c + axis.j * t - (z * axis.i - x * axis.k) * s;
		direction->k = z * c + axis.k * t - (x * axis.j - y * axis.i) * s;
	}
	velocity->i = direction->i * speed;
	velocity->j = direction->j * speed;
	velocity->k = direction->k * speed;
}

// @retail 0x177f60
void function_177f60(s_effect_datum *effect, dword a_scales, dword b_scales, dword *seed, vector3f *vector, real lower, real upper)
{
	real magnitude = function_177c50(seed, lower, upper, a_scales, b_scales, effect, 3);

	if (magnitude != 0.0f)
	{
		*vector = g_4417f0[random_index(seed, 0x402)];
		vector->i *= magnitude;
		vector->j *= magnitude;
		vector->k *= magnitude;
	}
	else
	{
		*vector = *g_4687a4;
	}
}

// @retail 0x17a8a0
void function_17a8a0(s_effect_location_datum *location, bool detached, vector3f const *up, s_effect_datum *effect, s_effect_part *part, point3f const *point, vector3f const *forward, real scale)
{
	switch (part->base_group_tag)
	{
	case 'char':
		if (part->tag_index != NONE && !(location->node_index != NONE && (location->node_index & 0x8000)))
		{
			long node_index = location->node_index == NONE ? NONE : location->node_index & 0x7fff;

			function_1ca290(part->tag_index, function_1469f0(effect->event_delay), effect->object_index, node_index, part->velocity_lower, part->velocity_upper, &location->matrix);
		}
		break;
	case 'coln':
		function_259d0(&g_4e7408->seed, __FILE__, __LINE__, part->radius_lower, part->radius_upper);
		break;
	case 'jpt!':
	{
		s_effect_damage_data data;

		function_d6660((s_type_1e6529 *)&data, part->tag_index);
		data.unknown7c = NONE;
		data.owner = effect->owner;
		data.location = effect->location;
		data.origin = *point;
		data.position = *point;
		data.direction = *forward;
		data.forward = *forward;
		data.scale = scale;
		function_d6c80((s_type_1e6529 *)&data, NONE);
		break;
	}
	case 'deca':
	{
		s_random_globals *random = g_4e7408;

		if (TEST_FIELD_BIT(part->flag4) || function_172750(1, point, 20.0f) || function_259a0(&random->seed) < 0.25f)
		{
			vector3f direction;
			vector3f velocity;

			function_177d10(effect, &random->seed, forward, &direction, &velocity, part->velocity_lower, part->velocity_upper, part->velocity_cone_angle, part->a_scales, part->b_scales);
			function_17e670(g_510c78, point, part->tag_index, &velocity, function_259d0(&random->seed, __FILE__, __LINE__, part->radius_lower, part->radius_upper), 0, NONE, 0);
		}
		break;
	}
	case 'obje':
	{
		s_effect_object_placement data;

		function_b7930(&data, part->tag_index, effect->object_index, &effect->owner);
		if (function_a7640(&data))
		{
			dword *seed = &g_4e7408->unknown0;
			vector3f direction;

			data.position = *point;
			data.forward = *forward;
			data.up = *up;
			function_177d10(effect, seed, forward, &direction, &data.velocity, part->velocity_lower, part->velocity_upper, part->velocity_cone_angle, part->a_scales, part->b_scales);
			data.velocity.i = effect->velocity.i + data.velocity.i;
			data.velocity.j = effect->velocity.j + data.velocity.j;
			data.velocity.k = effect->velocity.k + data.velocity.k;
			function_177f60(effect, part->a_scales, part->b_scales, seed, &data.angular_velocity, part->angular_velocity_lower, part->angular_velocity_upper);

			long object_index = function_b7b40(&data);

			if (object_index != NONE)
			{
				function_bb950(object_index, true, 0);
				function_a7870(object_index);
			}
		}
		break;
	}
	case 'ligh':
		if (effect->object_index != NONE && !detached)
		{
			long node_index = location->node_index == NONE ? NONE : location->node_index & 0x7fff;

			function_c0350(part->tag_index, effect->object_index, node_index, &location->matrix.up, &location->matrix.forward, &location->matrix.position, scale);
		}
		else
		{
			function_c0350(part->tag_index, NONE, NONE, up, forward, point, scale);
		}
		break;
	case 'snd!':
		if (effect->object_index != NONE && !detached)
		{
			short node_index = location->node_index == NONE ? NONE : location->node_index & 0x7fff;

			function_189060(effect->object_index, node_index, scale, &location->matrix.position, &location->matrix.forward, part->tag_index);
		}
		else
		{
			s_sound_position position;

			position.position = *point;
			position.compressed_forward = vector3d_compress(forward);
			position.velocity = *g_4687a4;
			position.location = effect->location;
			function_1895f0(&position, scale, part->tag_index);
		}
		break;
	case 'lens':
	case 'MGS2':
	case 'tdtl':
		break;
	default:
		if (!g_55e756)
			g_55e756 = true;
		break;
	}
}

// @retail 0x17a380
void __stdcall function_17a380(s_effect_datum *effect)
{
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	s_effect_event *event = &definition->events[effect->event_index];

	for (short i = 0; i < event->part_count; i++)
	{
		s_effect_part *part = &event->parts[i];
		short location_index = part->location;

		if (location_index < 0 || location_index >= definition->location_count || part->tag_index == NONE || location_index == NONE)
			continue;
		if (!(TEST_FIELD_BIT(effect->flag5) ? part->create_in_mode != 1 : part->create_in_mode != 2))
			continue;

		long next_index = effect->location_indices[location_index];
		s_effect_location_datum *location;

		while ((location = effect_location_next(effect, &next_index, 0)) != 0)
		{
			bool detached = TEST_FIELD_BIT(part->flag2);
			point3f point;
			vector3f forward;
			vector3f up;
			bool create;

			if (location->node_index != NONE)
			{
				transform4x3f matrix;

				function_17aec0(effect, &matrix, location->node_index);
				effect_matrix_transform_point(&matrix, &location->matrix.position, &point);
				effect_matrix_transform_normal(&matrix, &location->matrix.forward, &forward);
				effect_matrix_transform_normal(&matrix, &location->matrix.up, &up);
			}
			else
			{
				point = location->matrix.position;
				forward = location->matrix.forward;
				up = location->matrix.up;
			}
			if (TEST_FIELD_BIT(part->flag0))
			{
				forward = *g_4687bc;
				up = *g_4687a8;
			}
			if (TEST_FIELD_BIT(part->flag1))
			{
				real radius = part->radius_lower + (part->radius_upper - part->radius_lower) * function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__);
				long object_index = effect->object_index;

				if (object_index != NONE)
					object_index = function_baf40(object_index);
				function_16a8e0(0xd800005, &point, radius, object_index, NONE, &point, &radius);
				detached = true;
			}
			switch (part->create_in)
			{
			case 0:
				create = true;
				break;
			case 1:
				create = !function_11c120(&effect->location, &point, 0);
				break;
			case 2:
				create = function_11c120(&effect->location, &point, 0);
				break;
			case 3:
				create = false;
				break;
			default:
				__assume(0);
			}
			if (create)
			{
				real scale = 1.0f;

				if (part->a_scales & 0x20)
					scale = effect->scale_a;
				if (part->b_scales & 0x20)
					scale = effect->scale_b * scale;
				if (!TEST_FIELD_BIT(effect->flag10) || part->base_group_tag != 'obje')
					function_17a8a0(location, detached, &up, effect, part, &point, &forward, scale);
			}
		}
	}
}

// @retail 0x177040
long __stdcall function_177040(long particle_system_index, long effect_index)
{
	long const *effect_index_argument = &effect_index;
	effect_index = *effect_index_argument;
	s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	long next_index = particle_system->next_index;
	s_effect_particle_system_definition *definition = particle_system->function_1751d0();
	long location_index = effect->location_indices[definition->location_index];

	if (definition->unknown0c != 0)
	{
		long particle_location_index = particle_system->location_index;
		dword color_a = effect->color_a;
		dword color_b = effect->color_b;
		s_effect_location_datum *location;

		{
			c_type_4e7709 *particle_definition = function_137bd0(particle_system->function_1751d0()->tag_index);
			bool tinted = particle_definition->tinted();
			bool multiplied = particle_definition->multiplied();

			function_175a80(tinted, color_a, color_b, particle_system, multiplied);
		}
		while ((location = effect_location_next(effect, &location_index, definition->location_mode)) != 0)
		{
			transform4x3f matrix;
			transform4x3f *location_matrix = function_178bc0(location, effect, &matrix, true);
			bool field_b4 = location->node_index != NONE && (location->node_index & 0x8000);

			if (particle_location_index != NONE)
			{
				s_particle_location_datum *particle_location = DATUM(g_51ec8c, s_particle_location_datum, particle_location_index);

				function_248c60(particle_location, particle_system, location_matrix, field_b4);
				particle_location_index = particle_location->next_index;
			}
		}
	}
	return next_index;
}

// @retail 0x1778d0
bool function_1778d0(void)
{
	bool result = false;
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

		if (!TEST_FIELD_BIT(effect->flag2) && definition->unknown08 != 0.0f)
		{
			long player_index = NONE;

			for (;;)
			{
				player_index = data_find_index(g_4e8c24, player_index + 1);
				if (player_index == NONE)
					break;

				s_effect_player *player = (s_effect_player *)(g_4e8c24->data + g_4e8c24->size * player_index);

				if (!player)
					break;
				if (player->unit_index != NONE)
				{
					s_effect_object *unit = OBJECT_GET(player->unit_index);
					long location_index = effect->location_indices[0];
					s_effect_location_datum *location;

					while ((location = effect_location_next(effect, &location_index, 0)) != 0)
					{
						point3f point;
						vector3f delta;

						if (location->node_index != NONE)
						{
							transform4x3f matrix;

							function_17aec0(effect, &matrix, location->node_index);
							effect_matrix_transform_point(&matrix, &location->matrix.position, &point);
						}
						else
						{
							point = location->matrix.position;
						}
						vector3d_from_points3d(&unit->bounding_center, &point, &delta);

						real radius = unit->field_xc9d3f8 + definition->unknown08;

						if (radius * radius >= length_sq3f(&delta))
							return true;
					}
				}
			}
		}
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
	return result;
}

extern long g_4b9ed8;
bool function_3e9c0(long object_index);
bool function_2dba0(long tag, vector3f const *direction, long c, long d, long e, point3f const *position, color3f const *color, real alpha, real amount, real scale, bool alternate);
void function_42850(long a, long b, point3f const *position, vector3f const *first,
	vector3f const *second, real scale, real width, vector3f const *third);
real function_17ca10(real x, short curve);

PRIVATE inline real effect_remaining_fraction_179880(s_effect_datum const *effect)
{
	real result = 1.0f - effect->unknown60 / effect->event_delay;
	if (result < 0.0f)
		result = 0.0f;
	else if (result > 1.0f)
		result = 1.0f;
	return result;
}

// @retail 0x179880
void __stdcall function_179880(s_effect_datum *effect, long effect_index)
{
	/* Retail keeps both arguments on the stack; their local references retain that convention. */
	s_effect_datum *const *effect_reference = &effect;
	long const *index_reference = &effect_index;
	effect = *effect_reference;
	effect_index = *index_reference;
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	s_effect_event *event = &definition->events[effect->event_index];
	long flare_index = 0;
	for (long i = 0; i < event->part_count; ++i)
	{
		s_effect_part *part = &event->parts[i];
		if ((part->group_tag == 'lens' || part->group_tag == 'MGS2' || part->group_tag == 'tdtl') &&
			part->tag_index != NONE && part->location >= 0 && part->location < definition->location_count)
		{
			long index = effect->location_indices[part->location];
			short mode;
			if (effect->unknown58 != NONE && g_4b9ed8 == effect->unknown58 && !function_155760(effect->unknown58))
				mode = 1;
			else
				mode = 2;
			s_effect_location_datum *location;
			while ((location = effect_location_next(effect, &index, mode)) != 0)
			{
				point3f position = location->matrix.position;
				vector3f forward = location->matrix.forward;
				vector3f up = location->matrix.up;
				color3f color = *(color3f *)&g_4686cc->red;
				real scale = 1.0f;
				if (part->a_scales & 0x20)
					scale = effect->scale_a;
				if (part->b_scales & 0x20)
					scale *= effect->scale_b;
				if (location->node_index != NONE)
				{
					transform4x3f matrix;
					function_17aec0(effect, &matrix, location->node_index);
					effect_matrix_transform_point(&matrix, &position, &position);
					effect_matrix_transform_normal_179880(&matrix, &forward, &forward);
					effect_matrix_transform_normal_179880(&matrix, &up, &up);
				}
				if (part->group_tag == 'lens')
				{
					if (flare_index < 64)
					{
						if (effect->event_delay != 0.0f)
						{
							byte *tag = TAG_GET(byte, part->tag_index);
							real remaining = effect_remaining_fraction_179880(effect);
							scale *= 1.0f - function_17ca10(1.0f - remaining, *(short *)(tag + 0x3c));
						}
						function_2dba0(part->tag_index, &forward, 1, effect_index & 0xffff, flare_index, &position, (color3f const *)&effect->origin, 1.0f, scale, 1.0f, function_3e9c0(effect->object_index));
						++flare_index;
					}
				}
				else if (part->group_tag == 'MGS2')
				{
					if (effect->event_delay != 0.0f)
					{
						byte *tag = TAG_GET(byte, part->tag_index);
						if (*(long *)(tag + 8) > 0 && !(**(byte **)(tag + 0xc) & 0x10))
						{
							real remaining = effect_remaining_fraction_179880(effect);
							color.red *= remaining;
							color.green *= remaining;
							color.blue *= remaining;
						}
					}
					function_42850(effect->object_index, part->tag_index, &position, &forward, &up, 1.0f, scale, (vector3f const *)&color);
				}
				else if (part->group_tag == 'tdtl')
				{
					real remaining = 1.0f;
					if (effect->event_delay > 0.0f)
						remaining = effect_remaining_fraction_179880(effect);
					function_c40f0(part->tag_index, effect->object_index, remaining);
				}
			}
		}
	}
}

// @retail 0x179e80
void __stdcall function_179e80(s_effect_datum *effect)
{
	s_effect_event *event = &TAG_GET(s_effect_definition, effect->tag_index)->events[effect->event_index];

	for (long i = 0; i < event->beam_count; i++)
	{
		s_effect_beam *beam = &event->beams[i];
		real progress = 0.0f;

		if (effect->event_delay > 0.0f)
			progress = effect->unknown60 / effect->event_delay;

		long location_index = effect->location_indices[beam->location];

		while (location_index != NONE)
		{
			s_effect_location_datum *location = DATUM(g_4ea938, s_effect_location_datum, location_index);

			location_index = location->next_index;
			if (location->node_index != NONE)
			{
				transform4x3f node_matrix;
				transform4x3f matrix;

				function_17aec0(effect, &node_matrix, location->node_index);
				function_142a60(&node_matrix, &location->matrix, &matrix);
				function_156b60(beam, progress, &matrix);
			}
			else
			{
				function_156b60(beam, progress, &location->matrix);
			}
		}
	}
}

// @retail 0x176bb0
void function_176bb0(void)
{
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

		if (TEST_FIELD_BIT(effect->flag0) && !TEST_FIELD_BIT(effect->flag2) && !TEST_FIELD_BIT(effect->flag3) && !TEST_FIELD_BIT(effect->flag6))
		{
			function_179880(effect, effect_index);
			function_179e80(effect);
		}
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
}

// @retail 0x176e40
void function_176e40(void)
{
	volatile real dt = g_510c54->rate;
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		if (!function_1794e0(effect_index) && !function_179730(effect_index) && !function_179810(effect_index))
			function_1792b0(effect_index, dt);
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
}

// @retail 0x176f60
void function_176f60(real dt)
{
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		long particle_system_index = DATUM(g_4ea93c, s_effect_datum, effect_index)->first_particle_system_index;

		if (particle_system_index != NONE)
		{
			do
			{
				particle_system_index = function_177040(particle_system_index, effect_index);
			}
			while (particle_system_index != NONE);
		}
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
	function_174990(dt);
}

// @retail 0x1776e0
void function_1776e0(long unknown58, long object_index, bool attach)
{
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

		if (attach)
		{
			if (effect->object_index == object_index && TEST_FIELD_BIT(effect->flag7))
			{
				effect->unknown58 = unknown58;
				function_178360(effect_index, NONE, effect->object_index, unknown58, 0, 0);
			}
		}
		else if (effect->unknown58 == unknown58 && effect->object_index == object_index)
		{
			s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

			for (long i = 0; i < definition->location_count; i++)
			{
				long *location_index = &effect->location_indices[i];

				while (*location_index != NONE)
				{
					s_effect_location_datum *location = DATUM(g_4ea938, s_effect_location_datum, *location_index);

					if (location->node_index != NONE && (location->node_index & 0x8000))
					{
						long next_index = location->next_index;

						record_pool_release(g_4ea938, *location_index);
						*location_index = next_index;
					}
					else
					{
						location_index = &location->next_index;
					}
				}
			}
			effect->unknown58 = NONE;
		}
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
}

// @retail 0x17b030
bool function_17b030(long effect_index, vector3f const *velocity, real scale_a, real scale_b, transform4x3f const *matrix, real const *values)
{
	s_effect_datum *effect = (s_effect_datum *)record_pool_lookup_checked(g_4ea93c, effect_index);
	bool result = false;

	if (effect)
	{
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	effect->flag9 = true;
	if (velocity)
		effect->velocity = *velocity;
	if (values)
	{
		effect->unknown74 = values[0];
		effect->unknown78 = values[1];
	}
	effect->scale_a = scale_a;
	effect->scale_b = scale_b;
	if (matrix)
	{
		for (long i = 0; i < definition->location_count; i++)
		{
			long location_index = effect->location_indices[i];

			while (location_index != NONE)
			{
				s_effect_location_datum *location = DATUM(g_4ea938, s_effect_location_datum, location_index);

				transform4x3f *location_matrix = &location->matrix;

				location_index = location->next_index;
				*location_matrix = *matrix;
				location_matrix->scale = 1.0f;
			}
		}
	}
	result = true;
	}
	return result;
}

// @retail 0x17b1d0
void function_17b1d0(long object_index)
{
	s_effect_object *object = OBJECT_GET(object_index);
	long count = object->attachments_size / sizeof(s_effect_attachment);
	s_effect_attachment *attachments = (s_effect_attachment *)((byte *)object + object->attachments_offset);

	for (long i = 0; i < count; i++)
	{
		if (attachments[i].type == 2)
		{
			long effect_index = attachments[i].index;
			s_effect_datum *effect = (s_effect_datum *)record_pool_lookup_checked(g_4ea93c, effect_index);

			if (effect)
				function_178360(effect_index, NONE, effect->object_index, effect->unknown58, 0, 0);
			return;
		}
	}
}

// @retail 0x17b2d0
bool function_17b2d0(long effect_index, point3f *point, vector3f *forward, real *scale)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	bool result = false;

	if (effect)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
		long location_index = effect->location_indices[definition->looping_sound_location];
		s_effect_location_datum *location = effect_location_next(effect, &location_index, 3);

		if (!location)
			return false;

		transform4x3f node_matrix;
		transform4x3f world_matrix;
		transform4x3f *matrix;

		if (location->node_index != NONE)
		{
			function_17aec0(effect, &node_matrix, location->node_index);
			function_142a60(&node_matrix, &location->matrix, &world_matrix);
			matrix = &world_matrix;
		}
		else
		{
			matrix = &location->matrix;
		}
		*point = matrix->position;
		*forward = matrix->forward;
		*scale = effect->scale_a;
		return true;
	}
	return result;
}

static __forceinline s_effect_location_datum *effect_detach_location_next(s_effect_datum *effect, long *location_index, s_effect_location_datum *&location)
{
	if (*location_index == NONE)
		return 0;
	location = DATUM(g_4ea938, s_effect_location_datum, *location_index);
	*location_index = location->next_index;
	if (location->node_index != NONE && (location->node_index & 0x8000))
		location = effect_location_next(effect, location_index, 0);
	return location;
}

// @retail 0x17b490
bool function_17b490(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	bool result = true;

	if (!function_177610(effect_index))
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

		for (long i = 0; i < definition->location_count; i++)
		{
			long location_index = effect->location_indices[i];
			bool field_b4 = i < 16 ? (definition->location_flags_low & (1 << i)) != 0 : (definition->location_flags_high & (1 << (i - 16))) != 0;

			if (field_b4)
			{
				s_effect_location_datum *location;

				while ((location = effect_detach_location_next(effect, &location_index, location)) != 0)
				{
					if (location->node_index != NONE)
					{
						transform4x3f node_matrix;

						function_17aec0(effect, &node_matrix, location->node_index);
						function_142a60(&node_matrix, &location->matrix, &location->matrix);
					}
					location->node_index = NONE;
				}
			}
		}
		effect->object_index = NONE;
		function_1789f0(effect);
		result = false;
	}
	return result;
}

// @retail 0x17b3c0
void __stdcall function_17b3c0(long object_index)
{
	s_record_pool *effects = g_4ea93c;
	long effect_index = data_datum_index(effects, function_16bc00(effects, 0));

	while (effect_index != NONE)
	{
		if (DATUM(effects, s_effect_datum, effect_index)->object_index == object_index)
			function_17b490(effect_index);
		effect_index = data_datum_index(effects, data_find_index(effects, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
}

// @retail 0x17b160
bool function_17b160(long effect_index, long tag_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);
	bool result = false;

	if (effect && effect->tag_index == tag_index)
	{
		effect->flag6 = false;
		result = true;
		function_177310(effect_index);
	}
	return result;
}

// @retail 0x17b5d0
void function_17b5d0(s_effect_datum *effect, long object_index, s_effect_parameters *parameters, bool search)
{
	s_effect_parameters *const *parameters_reference = &parameters;
	parameters = *parameters_reference;
	bool found = false;

	if (parameters && TEST_FIELD_BIT(parameters->colors_set))
	{
		effect->color_a = parameters->color_a;
		effect->color_b = parameters->color_b;
		found = true;
	}
	else if (function_badc0(object_index, NONE))
	{
		long index = function_3ddd0(function_baf80(object_index));

		if (index != NONE)
		{
			byte *datum = g_509434->data + (index & 0xffff) * 0x100;

			if (datum[2])
			{
				effect->color_a = *(dword *)(datum + 0x2c);
				effect->color_b = *(dword *)(datum + 0x28);
				found = true;
			}
		}
	}
	if (search && !found)
	{
		s_effect_color_query query;
		long location_index = effect->location_indices[0];

		query.unknown00 = 0.0f;
		query.unknown04 = 0.0f;
		query.unknown08 = -1.0f;
		query.color_a = 0xff404040;
		query.color_b = 0xff404040;
		if (!parameters || !parameters->source || function_d2bb0(parameters->source, &query))
		{
			s_effect_location_datum *location = effect_location_next(effect, &location_index, 3);

			if (!location)
				goto done;
			{
				transform4x3f matrix;
				transform4x3f *location_matrix = function_178bc0(location, effect, &matrix, false);

				if (function_d2a50(0, NONE, 0, &query, 0, &location_matrix->position))
					goto done;
			}
		}
		found = true;
		effect->color_a = query.color_b;
		effect->color_b = query.color_a;
	}
done:
	if (parameters && !TEST_FIELD_BIT(parameters->colors_set) && found)
	{
		parameters->colors_set = true;
		parameters->color_a = effect->color_a;
		parameters->color_b = effect->color_b;
	}
}
