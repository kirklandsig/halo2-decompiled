// @flags /O2 /arch:SSE /Gr
#include "cluster_mask_storage.h"
#include "view_context_storage.h"
/* UNKNOWN_175BD0.CPP: the effects (entry 40 of the lifecycle table) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "effects.h"
#include "object_markers.h"
#include "unknown_1765e0.h"
#include "sound_sources.h"
#include <string.h>
#include "normalize3d.h"
#include "vector_products.h"

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
long __stdcall function_d2a50(long a, long b, bool c, s_effect_color_query *query, long d, point3f const *point);
__declspec(noinline) bool function_11c050(s_location const *location);
__declspec(noinline) bool function_11c080(s_location const *location);
bool function_11c120(s_location const *location, point3f const *point, short *zone_index);
bool function_3eb20(long cluster_index);
bool __stdcall function_3ebd0(vector3f const *offset, transform4x3f const *matrices, transform4x3f *out, long count);
long __stdcall function_3ddd0(long object_index);
bool __stdcall function_bab40(long object_index, long name, real *value);
bool function_bad50(long object_index, long index, point3f *out);
long function_baf80(long object_index);
long function_155760(long index);
__declspec(noinline) bool function_163080(void);
long function_166244(long key);
__declspec(noinline) bool function_166283(long group_index, long key);
__declspec(noinline) s_player_state *function_16f3a0(long index);
vector3f *function_11d000(vector3f const *v, vector3f *out);
void function_179fb0(s_effect_datum *effect);
void __stdcall function_17a380(s_effect_datum *effect);
extern s_record_pool *g_509434;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

/* the clusters whose effects are visible */

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
	word flag0 : 1;
	word flag1 : 1;
	word : 14;
	byte unknown06[0x18 - 6];
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
	if (g_4686c4 == NONE)
	{
		location->leaf_index = NONE;
		location->cluster_index = NONE;
	}
	else
	{
		long leaf_index = function_14a280(g_4e033c, 0, point);

		location->leaf_index = leaf_index;
		long cluster_index = leaf_index != NONE ? ((s_effect_structure_bsp *)g_4e0348)->leaves[leaf_index].cluster_index : NONE;
		location->cluster_index = (short)cluster_index;
	}
	location->bsp_index = g_4686c4;
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
long function_178120(long tag_index, s_effect_owner const *owner, bool force);
void function_178240(point3f const *origin, vector3f const *direction, s_effect_datum *effect, real scale_a, real scale_b);
struct s_effect_marker_source;
void function_1786f0(s_effect_object_marker *out, s_effect_marker_source const *source, short marker_index);
long function_1785c0(s_effect_object_marker const *marker, s_effect_datum *effect, short location_index, bool field_b4);
transform4x3f *function_178bc0(s_effect_location_datum *location, s_effect_datum *effect, transform4x3f *matrix, bool field_b4);
void function_178c80(real scale, s_effect_datum *effect, s_particle_system_datum *particle_system, s_effect_particle_system_definition *definition, real unknown, bool field_b4);
bool function_178020(short placement, point3f const *point, s_location const *location);
__declspec(noinline) bool function_17b270(s_effect_datum *effect, short const *values, long mode);
void function_17af30(bool field_b4, short node_index, s_effect_datum *effect, transform4x3f *matrix);
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
void function_178360(long effect_index, long object_index, long unknown58, s_effect_marker *markers, long marker_count, long unknown18);
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
void function_176330(point3f const *point, s_effect_marker *markers);

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
long function_c0350(long tag_index, long object_index, long node_index, vector3f const *up, point3f const *position, vector3f const *forward, real scale);
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

/* a file-local copy of the shared object getter (0xbadc0): retail's effect location update calls the getter out of line, where the optimizer expands the shared one inline; the linker folds this copy into the shared function */
struct s_object_header_175d70
{
	short identifier;
	byte flags;
	byte type;
	byte unknown04[4];
	s_object *object;
};

PRIVATE __declspec(noinline) s_object *object_get_175d70(long object_index, dword type_mask)
{
	s_object_header_175d70 *header = 0;

	if (object_index != NONE)
	{
		long index = object_index & 0xffff;
		if (index < g_4e0300->high_water_index)
		{
			s_object_header_175d70 *h = (s_object_header_175d70 *)(g_4e0300->data + g_4e0300->size * index);
			if (h->identifier && h->identifier == (object_index >> 16))
				header = h;
		}
	}

	s_object *result = 0;
	if (header && (type_mask & (1 << header->type)))
		result = header->object;
	return result;
}

// @retail 0x175d70
void function_175d70(void)
{
	long datum = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));
	while (datum != NONE)
	{
		long index = datum & 0xffff;
		s_effect_datum *effect = (s_effect_datum *)g_4ea93c->data + index;
		if (effect->object_index != NONE)
		{
			s_effect_object *object = (s_effect_object *)object_get_175d70(effect->object_index, (dword)NONE);
			if (!object)
			{
				effect->object_index = NONE;
				effect->location.leaf_index = NONE;
				effect->location.cluster_index = NONE;
				effect->location.bsp_index = g_4686c4;
				function_1774f0(datum);
				goto next;
			}
			effect->location = object->location;
		}
		else if (!function_1789f0(effect))
		{
			function_1774f0(datum);
			goto next;
		}
		{
			long system_index = effect->first_particle_system_index;
			while (system_index != NONE)
			{
				s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, system_index);
				long next_index = system->next_index;
				system->set_location(&effect->location);
				system_index = next_index;
			}
		}
next:
		datum = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, datum == NONE ? 0 : index + 1));
	}
	particle_systems_update_locations();
}

__declspec(noinline) void effect_parameters_initialize(s_effect_parameters *parameters);

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
void function_176330(point3f const *point, s_effect_marker *markers)
{
	markers[0].forward = *g_4687bc;
	markers[0].position = *point;
	markers[0].name = 0x70000c0;
	markers[1].forward = *g_4687b0;
	markers[1].position = *point;
	markers[1].name = 0x20000ca;
}

// score 20 (was 20): the copy of the normal and the swap of the two pointers happen in the other order
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

	vector3f *surface = &markers[0].forward;
	vector3f *incident = &markers[1].forward;

	*surface = *normal;
	*incident = *direction;
	markers[2].forward.i = direction->i * -1.0f;
	markers[2].forward.j = direction->j * -1.0f;
	markers[2].forward.k = direction->k * -1.0f;

	/* marker 3 bounces the incoming direction off the surface; marker 1 is
	   then made unit length */
	vector3f const *incoming = &markers[1].forward;
	vector3f *unit_direction = &markers[1].forward;
	vector3f *bounce = &markers[3].forward;
	vector3f const *surface_normal = &markers[0].forward;
	real twice = (incoming->i * surface_normal->i +
		(incoming->j * surface_normal->j + incoming->k * surface_normal->k)) * 2.0f;

	bounce->i = incoming->i - surface_normal->i * twice;
	bounce->j = incoming->j - surface_normal->j * twice;
	bounce->k = incoming->k - surface_normal->k * twice;

	normalize3d(unit_direction);
	markers[0].forward = *normal;
	markers[4].forward = *g_4687bc;
	markers[5].forward = *g_4687b0;
}

// @retail 0x176a50
void function_176a50(s_effect_parameters *parameters, long tag_index, long marker_count, s_effect_marker *markers, long mode)
{
	effect_parameters_initialize_inline(parameters);
	parameters->flags = 0;
	parameters->tag_index = tag_index;
	parameters->marker_count = marker_count;
	parameters->markers = markers;
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
		effect_stop_looping_sound(effect);
		function_1771a0(effect);
		if (TEST_FIELD_BIT(effect->flag1))
		{
			word *flags = &effect->flags;

			if (flag)
				*flags |= 0x10;
			else
				*flags &= ~0x10;
			*flags |= 4;
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
long function_178120(long tag_index, s_effect_owner const *owner, bool force)
{
	long effect_index = NONE;

	if (tag_index != NONE)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, tag_index);

		if ((force || !TEST_FIELD_BIT(definition->flag2)) && definition->event_count > 0)
		{
			effect_index = record_pool_allocate(g_4ea93c);
			if (effect_index == NONE)
			{
				if (TEST_FIELD_BIT(definition->flag2) && function_178060())
					effect_index = record_pool_allocate(g_4ea93c);
			}
			if (effect_index != NONE)
			{
				s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

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
				real *values = &effect->unknown74;
				values[0] = 0.0f;
				values[1] = 0.0f;
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

/* starts the effect's event: clears flag 0 and draws the event's delay */
// @retail 0x1782a0
void function_1782a0(long effect_index, short event_index)
{
	s_effect_datum *effect = effect_try_and_get(effect_index);

	if (effect)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

		if (event_index >= 0 && event_index < definition->event_count)
		{
			s_effect_event *event = &definition->events[event_index];
			word *flags = &effect->flags;

			*flags &= ~1;
			effect->event_index = event_index;
			effect->unknown60 = 0.0f;
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

/* effect_location_next expanded at the call (retail inlines it in some of its location loops) */
static __forceinline s_effect_location_datum *effect_location_next_inline(s_effect_datum *effect, long *location_index, short mode)
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

__declspec(noinline) void function_17aec0(short node_index, s_effect_datum *effect, transform4x3f *matrix);

// score 11 (was 11): the node index is masked after the branch that tests the high bit, retail masks it before the call
// Round 44 (lane 1, fast): the copy through the file's effect_object_node_matrix 37 (esi shrink-wrapped);
// the copied index as `short` 28 (effect esi/index edx as retail, re-test not threaded); the copy kept and
// `node_index` re-tested 28 (the copy in the test's block stops threading); the threaded ternary with the
// parameter as its NONE arm 35, with a `long` index 35 (threaded as retail, but effect edx/index esi); the
// index masked into a variable before the high-bit test and passed to the call 47 (96 bytes: the convention
// moves to effect eax/node ecx and 0x17af30 goes to 29). Threaded forms always colour the effect first.
// Also: `short index = node_index;` at the top with `if (node_index != NONE) index = node_index & 0x7fff;` after
// the call 38, `short index = NONE;` at the top 40 (both thread the re-test with retail's effect esi/index edx,
// but the copy or the `or edx, -1` stays at entry instead of on the NONE path); one index assigned in the call
// branch and by the threaded ternary 35 (the call branch's value becomes its own web).
// Round 111: outer NONE guard around the special case with a copied long index 29 (105 bytes); copied-index
// special/fallback arms with one exit 23; threaded index with a const effect 35; threaded index with a two-use
// hosted identifier 35; threaded index with memcpy 35; copied index with memcpy 11; dominating long copy before
// the original special guard 29; outer NONE test on the copied index 41. Original body restored.
// @retail 0x17aec0
void function_17aec0(short node_index, s_effect_datum *effect, transform4x3f *matrix)
{
    if (node_index != NONE && (node_index & 0x8000) && effect->unknown58 != NONE)
    {
        function_1664da(effect->unknown58, effect->object_index, node_index & 0x7fff, matrix);
        return;
    }
    long index = node_index;
    if (index != NONE)
        index &= 0x7fff;
    s_effect_object *object = OBJECT_GET(effect->object_index);
    transform4x3f const *nodes = (transform4x3f const *)((byte *)object + object->nodes_offset);
    *matrix = nodes[(short)index];
}

// score 13 (was 13): the node index is masked in ecx in retail, ours loads it from the stack into ecx and masks it later.
// Tried (r27b): the plain form without the volatile reread, with or without the node local, 37 (the node
// moves to eax and the flag to cl); a `short const *` reference to the node keeps it on the stack but
// swaps the flag and matrix registers (cl/eax, retail al/ecx) 32.
// Tried (r35): the qualified snapshot with a later plain parameter read 13; the direct fallback 31;
// the initial read through its address 13; a C++ reference 32; an overwritten snapshot 37;
// memcpy without the reread 37; `(void)&node_index` with either snapshot form 37.
// Tried (r105): parameters reversed (matrix first, flag last; definition and the 0x178bc0 call)
// with the node read once through its address 31 (flag cl, matrix on the stack), with the
// current body 13 (0x178bc0 2: its setup follows the declared order), plain 37; a word or
// long node parameter, plain body, 37.
// Round 44 (lane 1, fast): the `node` copy dropped (parameter read directly) 13 (same code); the condition
// inverted 13 (same code); the copy as memcpy 13; a `word` node parameter (0x178bc0 pushes a zero-extended
// word) 13; the reread moved into the copy's initialization (one read, all uses on the copy) 37 (the node
// becomes an eax argument again, 0x178bc0 50); diagnostic only, the reread moved to the 0x17aec0 call 31
// (convention kept, the node then takes ecx and `push esi` is shrink-wrapped).
// @retail 0x17af30
void function_17af30(bool field_b4, short node_index, s_effect_datum *effect, transform4x3f *matrix)
{
	short node = node_index;

	if (field_b4 && node != NONE && (node & 0x8000) && effect->unknown58 != NONE)
		/* the node reread from its stack slot here: retail takes the flag in al and the matrix in ecx */
		*matrix = *function_1664a5(effect->unknown58, effect->object_index, *(short const volatile *)&node_index & 0x7fff);
	else
		function_17aec0(node, effect, matrix);
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
bool function_17b270(s_effect_datum *effect, short const *values, long mode)
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
	long result = NONE;
	bool force = TEST_FIELD_BIT(parameters->flag2);

	if (force || function_176210(parameters))
	{
		long effect_index = function_178120(parameters->tag_index, &parameters->owner, force);
		result = effect_index;
		if (effect_index != NONE)
		{
			s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
			s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

			effect->unknown0c = parameters->unknown30;
			if (g_4e6948->state == 2 && function_163080() && parameters->object_index != NONE)
			{
				s_effect_object *object = OBJECT_GET(parameters->object_index);

				if (TEST_FIELD_BIT(TAG_GET(s_effect_object_definition, object->tag_index)->flag13))
					effect->flag10 = true;
				else
					effect->flag10 = false;
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
			function_178360(effect_index, effect->object_index, effect->unknown58, parameters->markers, parameters->marker_count, parameters->unknown18);
			if (!TEST_FIELD_BIT(parameters->attached))
				function_1789f0(effect);
			if (TEST_FIELD_BIT(definition->flag3) || TEST_FIELD_BIT(definition->flag4))
				function_17b5d0(effect, parameters->object_index, parameters, true);
			long const *scales = parameters->unknown50;
			if (scales)
			{
				*(long *)&effect->unknown74 = scales[0];
				*(long *)&effect->unknown78 = scales[1];
			}
			function_177310(effect_index);
			g_510c78 = parameters->source;
			if (TEST_FIELD_BIT(definition->flag1) && !TEST_FIELD_BIT(effect->flag1) && !TEST_FIELD_BIT(effect->flag9))
				function_179850(effect_index, 1.0f);
			else
				function_179850(effect_index, 0.0f);
			g_510c78 = 0;
			result = record_pool_lookup(g_4ea93c, effect_index) ? effect_index : NONE;
		}
	}
	return result;
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
						real distance_squared = dx * dx;
						distance_squared += dy * dy;
						distance_squared += dz * dz;

						if (minimum > distance_squared)
							minimum = distance_squared;
					}
				}
				if (!(lower * lower > minimum))
				{
					if (minimum > upper * upper)
						result = false;
					else
						result = (g_4c56c0[parameters->source->index >> 5] & (1 << (parameters->source->index & 31))) != 0;
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
	function_176330(&point, markers);
	parameters.markers = markers;
	parameters.marker_count = 2;
	return effect_new_from_parameters(&parameters);
}

// @retail 0x176780
long function_176780(s_effect_owner const *owner, long object_index, real scale_a, long tag_index, real scale_b, point3f const *origin, vector3f const *direction)
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
	function_176330(&point, markers);
	parameters.markers = markers;
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
	function_176330(&object_markers[0].unknown38.position, markers);
	parameters.markers = markers;
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
void function_176b60(s_effect_owner const *owner, long marker_count, s_effect_marker *markers, long mode, long tag_index)
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
void function_178360(long effect_index, long object_index, long unknown58, s_effect_marker *markers, long marker_count, long unknown18)
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
		source.node_index = unknown18 == NONE ? 0 : unknown18;
		source.node_matrix = effect_object_node_matrix(object_index, source.node_index);
	}
	else
	{
		source.node_index = NONE;
		source.node_matrix = 0;
	}
	for (long location_index = 0; location_index < definition->location_count; location_index++)
	{
		dword first_person_mask = 0;
		dword name = definition->locations[location_index];
		long count = 0;

		if (name == 0x700054e)
			name = effect->unknown0c;
		if (name)
		{
			long marker_index = NONE;

			if (marker_count > 0)
				marker_index = effect_marker_find(markers, marker_count, name);
			if (marker_index != NONE)
			{
				function_1786f0(object_markers, &source, (short)marker_index);
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
						long first_person_count = first_person_weapon_get_markers_internal(object_index, name, (s_first_person_marker *)&object_markers[count], (short)(16 - count));

						for (long i = 0; i < first_person_count; i++)
							first_person_mask |= 1 << (i + count);
						count += first_person_count;
					}
				}
				if (count == 0 && default_marker_index != NONE)
				{
					function_1786f0(object_markers, &source, (short)default_marker_index);
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

// score 9 (was 9): only the node/matrix entry order remains (round 67, round 70). Earlier: node/matrix entry order and
// the forward/up xmm homes remained (retail forward
// in xmm4/2/3 and up in xmm5/1/0; the marker forward values take xmm2 first here); frame 0x1c agrees.
// The point computed into position, normalize3d and cross3f give retail's early up.j/up.k loads.
// Node index first 107 (marker index sign extension hoisted); forward without locals 132;
// locals k,j,i 80, j,k,i 81; dot3f for forward 132, for position 82; written cross 119;
// marker pointer in the else branch 198. Shared difference 187; shared scale 94; both helpers 186;
// separate point workspace 125; normalized workspace 164; scale snapshot 185; marker vector
// snapshot 184; point setters 227. Cartesian scalar names, ordered direction/point accumulation,
// displacement scalars, output-matrix receiver, early direction inputs and immutable accumulation
// 77 each; existing inverse-point call 417. Native node copy 77; copy first 107;
// complete/immutable/shared marker inputs 212/212/208; node-first marker value 211 (marker values
// lose the matched caller); direction results 77; results with node first 107. Direction inputs
// i,k,j / j,i,k / k,i,j 79/80/81; with node copied first 109/110/111. Read-only node-matrix input
// and marker-array input 77/77.
// Node copy first (retail entry and length 758, index sign extension hoisted into the entry) 107;
// with the matrix local inside the branch 107; marker forward value copy 166; marker forward
// fields read directly 139; forward locals named x/y/z or forward_i/j/k 107/107; declarations
// forward/position/up or up/forward/position 107/107.
// Round 95: node copy before the matrix load 107 (758 bytes; the marker index sign extension is
// hoisted above the matrix test, retail extends it in each arm); with a marker pointer in the else
// arm 198 (762 bytes), the same pointer without node-first 198; node-first with pointer-sum marker
// address 107; node-first with the else arm copying forward first 121. Original body restored.
// Product order inside each forward row (any of six per row) 77 (identical code); the
// tail as matched 0x1420f0's body (scale, forward, two-step cross, up, zeroed position,
// position) through a forceinline copy 137 (its extra tuples flip the magnitude's sum
// order and move 1.0 out of xmm7), with the node copied first 149, with a direction
// pointer for the forward rows 158, the zero position or the inverse scale through the
// existing setters 137/137; `up` declared just before the 0x11d000 call 77; branches
// swapped (`if (!matrix)`) 163. Round 7 (lane 14): the staged cross product 77 (same code); node copy
// first 107 (758 bytes; aligned 66, the markers base then stays in EBX); the cross product before
// the forward copy 85; scale stored last 101 (766 bytes); forward locals read before the scale test
// 236 (795 bytes).
// Round 165: the five other position/forward/up declaration orders 77 (identical); forward rows before the
// position block 198; cross product from the stored output forward 169 (758 bytes); forward rows with `+=`
// 77 (identical); node matrix read through the source at each use 202 (caller 0x178360 86).
// 236 (795 bytes). Round 99: the xmm rotation is one joint choice: retail's normalize puts up.i/up.j
// in xmm5/xmm1, leaving xmm2-4 for forward across 0x11d000; ours puts them in xmm1/xmm2, forward in
// xmm3-5. Cross product reading the stored forward field 169 (758 bytes, forward reloaded); up
// declared first 77 (same code); normalize3d on 0x11d000's returned pointer 187; forward stored
// before the 0x11d000 call 111; node copied first with the matrix read from the source at each use
// 202 (768 bytes).
// Round 170: node copy first 107 (the marker index extension hoists into the entry); with a per-arm
// long index 107; with the branches inverted 173; forward components at function scope 77; read
// before the position block 236 (795 bytes).
// Round 172: the forward rows' accumulators follow row statement order (XMM5, XMM4, XMM3); statement
// orders i,k,j / j,i,k / j,k,i / k,i,j / k,j,i 82/77/82/81/81.
// Round 450: complete live marker fields alone/node-first/matrix-live/both
// 215/218/223/223; with staged cross 215/218/223/223. All lose the matched caller;
// original point, marker, normalizer and cross producers restored.
// Round 51 (lane 16, fast with 0x178360): the inverse scaling through the existing vector3f_scale and
// the null-scale zero position through real_point3d_set 77 (identical code, kept); the offset through
// vector3d_from_points3d 190 (752 bytes), with vector3f_scale too 189; the output position zeroed
// through real_point3d_set and then assigned 109 (766 bytes: the position pointer local gives retail's
// `lea eax, [esi + 0x2c]` stores). A compile-only build does not model this function (retail keeps
// forward in xmm2-4 across the LTCG 0x11d000 call).
// Round 118 (lane 5, fast with 0x178360): node copy first with one function-scope marker pointer assigned
// in each arm 198 (762 bytes); node copy first with the direction inputs read j, k, i 111 (758 bytes);
// the direction inputs read j, k, i 81, read k, j, i 80; the cross product into a `left` local then
// assigned 143 (792 bytes). 0x11d000 is a leaf using only xmm0/xmm1, so any of xmm2-xmm5 can carry
// forward across it; retail picks xmm4/2/3 (the marker's k lands in xmm5 where ours takes xmm2).
// Round 42 (lane 1, fast with 0x178360): node copy first re-viewed (107: the hoisted extension also
// moves the markers base to EBX in the matrix arm; the xmm rotation is unchanged); with one marker
// pointer taken before the matrix test 202 (747 bytes); up stored before the cross product 86; the
// else arm copying forward before position 115 (762 bytes); `(long)marker_index` at the three
// indexings 77 (identical); the zero position as a chained assignment 79.
// Round 67 (lane 12, fast with 0x178360; plain 9): the forward rows through the existing vector3f_assign
// (forward's fields then written only through the setter) give retail's forward block, xmm homes and
// cross product: 77 -> 9 (758 bytes, kept). Left: retail copies the node index first (CX, before the
// matrix load) where ours loads the matrix first and copies the index through DI, and keeps the markers
// base in EAX (ours EBX). Node copy first on this body 101 (entry exact, but the marker index sign
// extension hoists above the matrix test and is shared by both arms); with the matrix read in the arm
// 101; declarations first 101; with an else-arm marker pointer 198; with the arms inverted 189. Earlier
// on the old body: the position rows in each of the other five product orders (all rows or row 2 only)
// 77 (identical); one or two unused locals 77; an extra named store to forward.i/j/k or position.x/y/z
// in the else arm (diagnostic) 128/94/92/211/212/212.
// Round 105 (lane 7, fast with 0x178360): node copy first 107 (758 bytes, entry as retail up to the hoisted marker
// index extension; aligned 66), with the position/forward/up declarations above it 107, with up declared first
// 107, with the forward inputs declared at function scope 107; node first with the marker fields read at each
// use (no marker pointer) 218 (779 bytes, caller 0x178360 86).
// Round 59 (lane 16, fast with 0x178360): node-first loses because VC merges the two arms' identical
// leading `movsx edx, dx` into the entry, where retail's else arm extends into ECX (base in EDX, `lea eax,
// [ecx + edx]`), so its arms differ. On node-first: the else arm copying a whole marker local 141 (774
// bytes, 0x178360 56); the matrix arm reading the marker directly 218 (779 bytes, 0x178360 86); the arms
// swapped 173; one markers base local for both arms 124; the scale test inverted (zero arm first) 156.
// Round 70 (lane 12, fast with 0x178360): node-first re-measured 101 (the arms' identical `movsx edx, dx`
// moves above the test); on it the matrix-arm marker as `source->markers + marker_index` 101, `matrix != NULL`
// 101, one `markers` local for both arms 102 (0x178360 1); the parameters as (source, out, marker_index) or
// (source, marker_index, out), the order retail's esi-before-eax setup suggests, 9/9 (callee unchanged, 0x178360
// 2 each; with node-first 101/101), so the current order is retail's.
// Round 260: null-arm point/vector setters, singly/both and node-first/reversed publication,
// 210/215/193/208/212/191/193/191; native block-copy equivalents 9/9/9/101/101/101/9/101.
// Setters lose the matched caller; native copies retain it but do not fix entry or index hoisting.
// Round 260: a 600 s bounded search, 55 attempts (7 failed), none below 9.
// @retail 0x1786f0
void function_1786f0(s_effect_object_marker *out, s_effect_marker_source const *source, short marker_index)
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
				vector3f_scale(&offset, 1.0f / matrix->scale, &offset);
			position.x = matrix->forward.k * offset.k + matrix->forward.j * offset.j + matrix->forward.i * offset.i;
			position.y = matrix->left.k * offset.k + matrix->left.j * offset.j + matrix->left.i * offset.i;
			position.z = matrix->up.k * offset.k + matrix->up.j * offset.j + matrix->up.i * offset.i;
		}
		else
		{
			real_point3d_set(&position, 0.0f, 0.0f, 0.0f);
		}
		real i = marker->forward.i;
		real j = marker->forward.j;
		real k = marker->forward.k;

		vector3f_assign(&forward,
			matrix->forward.k * k + matrix->forward.j * j + matrix->forward.i * i,
			matrix->left.k * k + matrix->left.j * j + matrix->left.i * i,
			matrix->up.k * k + matrix->up.j * j + matrix->up.i * i);
	}
	else
	{
		position = source->markers[marker_index].position;
		forward = source->markers[marker_index].forward;
	}
	function_11d000(&forward, &up);
	normalize3d(&up);
	out->matrix.scale = 1.0f;
	out->matrix.forward = forward;
	cross3f(&up, &forward, &out->matrix.left);
	out->matrix.up = up;
	point3f *out_position = &out->matrix.position;

	out_position->x = 0.0f;
	out_position->y = 0.0f;
	out_position->z = 0.0f;
	*out_position = position;
}

// @retail 0x178bc0
transform4x3f *function_178bc0(s_effect_location_datum *location, s_effect_datum *effect, transform4x3f *matrix, bool field_b4)
{
	if (location->node_index != NONE && effect->object_index != NONE)
	{
		transform4x3f node_matrix;

		function_17af30(field_b4, (word)location->node_index, effect, &node_matrix);
		function_142a60(&node_matrix, &location->matrix, matrix);
		if (function_3eb20(effect->location.cluster_index) && effect->object_index != NONE)
		{
			s_effect_object *object = OBJECT_GET(effect->object_index);
			point3f const &position = object->position;
			vector3f offset;

			offset.i = g_468788->x - position.x;
			offset.j = g_468788->y - position.y;
			offset.k = g_468788->z - position.z;
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

static inline bool effect_location_in_zone_178c80(s_location const *location, point3f const *point)
{
	bool result = false;
	short cluster_index = location->cluster_index;

	if (cluster_index != NONE)
	{
		s_effect_zone_bsp *bsp = (s_effect_zone_bsp *)g_4e0348;
		s_effect_zone_cluster *cluster = bsp->clusters + cluster_index;
		byte zone = cluster->zone;

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

// MiniMax R (10-31): no kept form, no lower score: colour locals reach the frame 0x44 but spill both colours
// (195/188); the permuter's 124 promotes the unknown to xmm0 and breaks retail's argument layout.
// score 128 (was 128): Frame 0x40 versus 0x44, early color spill, cursor/query homes and zone-base scheduling remain.
// Earlier forms: frame 0x44 agrees; settings/color/query registers, cursor home and zone-base scheduling remain.
// tried native slot scalar 159 (a one-use copy of the field; not kept without a match); cursor/request/transform names and late settings receiver 166 (fast control 166);
// shared zone predicate 286; zone cases first 166; float consistency 166; immutable loop selectors 169 (fast).
// frame 0x44 agrees; entry cursor/settings homes, color staging and zone-base scheduling remain.
// Type query, color update, matrix query and spawn update noinline declarations 163 each;
// existing tint/multiplication result byte carriers 146/159, both 142 kept;
// placement result byte carrier 163. No new result copy or field read.
// frame 0x40 versus 0x44; color/cursor homes and zone-base carriers remain.
// Round 173: tint bool 165; multiply bool 137; both bool 128 kept plain; direct multiply
// with signed-byte/bool tint 149/135. The merged Boolean consumer re-normalized the byte carriers.
// Native spawn initialization/query outputs first/both 130 each; complete placement helper 375;
// returned-location for 379; placement output first/rejection guard 130 each; native spawn with for 379.
// Zone cluster input 130; direct leaf result 173; live zone base 130; native byte result 152; point-first
// input 130; cluster/direct result 173; live-base/direct result 173; cluster/byte result 152.
// Existing tint result unsigned byte/unsigned word/signed word 139/189/189;
// multiplication result unsigned byte/unsigned word/signed word 163/137/137;
// both unsigned byte/word 169/192. Early color frame home and native query pushes remain.
// Spawn location first/middle 130/130; unknown before scale 127 (three-count gain rejected);
// query receiver scope/enduring query outputs 130/130; signed node domain/sign after NONE 151/135;
// condition-local location 130. Original stores, query scopes and node predicate restored.
// Round 116: Unsigned-byte placement 130; signed-word placement 165; signed-byte zone selector 130; true-default placement 130; private zone failure guards 172. Opening body and helper restored.
// Round 88: an inline color helper (particle system, both colors; lookup, tinted, multiplied() as the
// argument) shared with 0x177040 gives frame 0x44 and retail's early color loads but 177 (the particle
// system in EDI, retail ESI; color B in EBX, retail EDI; tinted spilled, retail BL); its colors named
// to sort after location_index 177; parameters reordered 177; colors read from the effect inside 177;
// const color parameters 177.
// Round 222: native-long cluster 139; word/dword zone 151/151; native cluster with
// word/dword zone 157/157; all point-first products 130, with native cluster 139, with word zone 151.
// Original zone producer and complete body restored; color/query frame remains 0x40 versus 0x44.
// Round 198: register-only scale formal in five source positions 130 each; read-only descriptor/original scale/scale last/scale middle 130/130/130. All definitions, declarations and both calls restored.
// Round 204: complete zone distance/products 133/133; forced zone expansion 130; signed-byte/native-long result 152/168; forced distance/products 133/133; long result plus distance 168. Whole original helper and body restored.
// Round 151 (0x175a80 now takes retail's registers): inline color helper shared with 0x177040, colors as
// its arguments 178 (frame 0x44 and early color loads, but the system keeps EDI, color B EBX, tinted spilled);
// multiplied() as the call's argument 135; helper with colors first 185; inline member color update 185;
// a loop-only definition copy 128; a cluster-array base local 128.
// Round 417: spawn scale/unknown/location outputs before location propagation, masks 1-7,
// 130/126/130/130/130/126/130; complete early aggregate 130. The 126 forms lose callers.
// Round 165: the zone helper reading `cluster->zone` at each use 165 (aligned 85 -> 52: the zone block then
// matches retail, base in EDI and the add unfolded; the entry's missing color loads keep the count up), with
// an inline color helper shared with 0x177040 181, with `multiplied()` as the argument 172; colors read into
// locals before the spawn stores 187; color B then A read early 185.
// Round 170: colours as locals after the spawn stores 184, with multiplied() as the argument 178; a
// colour helper (system, A, B) 178 (frame 0x44, but colour A lands in the dead `unknown` slot and the
// cursor in the new frame slot, retail the reverse); colours inside the update block 184; zone read
// at each use 165; helper with that zone 181.
// Round 172: shared colour helper (system, A, B) 178 (0x177040 92); its parameters renamed 178
// (identical); both type queries as the call's arguments 145 (`multiplied()` runs first); cursor computed
// after the colour call 158 (552 bytes); matrix at function scope 178.
// Round 450: live cluster field alone/zone/entry/both 128/165/379/379; with
// live base 128/165/379/379. Entry-field forms outline the existing zone producer;
// its forced expansion on live cluster/entry gives 186. Original producer and body restored.
// Round 440: shared plane producer 128; complete plane/point/both/zone/zone-point/zone-producer inputs and
// assigned plane result 379 each (producer outlined). Forced existing expansion on
// plane/point/both/zone/zone-point/assigned graphs 281/240/358/340/400/405. All input objects, original
// arithmetic, helper and body restored.
// Round 493: complete point/plane/both values, mutable then const, 379/379/379/379/379/379;
// complete point and plane with named distance, mutable/const 379/379. Original zone producer
// restored; early color spill, query/cursor homes and frame 0x40/0x44 remain.
// Complete point/plane/both/const-both values with forced zone expansion 280/281/406/406; restored.
// Round 501: complete placement case orders 0132/0213/0231/0312/0321/1023/1032/1230
// 128/138/138/128/138/128/128/128. Original case order, body and zone producer
// restored; early color/query ownership and frame 0x40 versus 0x44 remain.
// @retail 0x178c80
void function_178c80(real scale, s_effect_datum *effect, s_particle_system_datum *particle_system, s_effect_particle_system_definition *definition, real unknown, bool field_b4)
{
	long location_index = effect->location_indices[definition->location_index];
	s_effect_location_datum *location;
	s_particle_system_spawn spawn;

	if (effect->location.cluster_index != NONE)
		particle_system->set_location(&effect->location);

	spawn.scale = scale;
	spawn.unknown = unknown;
	spawn.location_index = particle_system->location_index;
	{
		c_type_4e7709 *particle_definition = function_137bd0(particle_system->function_1751d0()->tag_index);
		bool tinted = particle_definition->tinted();
		bool multiplied = particle_definition->multiplied();

		function_175a80(effect->color_a, effect->color_b, particle_system, multiplied, tinted);
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
			spawn_here = !effect_location_in_zone_178c80(&effect->location, &location->matrix.position);
			break;
		case 2:
			spawn_here = effect_location_in_zone_178c80(&effect->location, &location->matrix.position);
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

			function_175270(particle_system, &spawn, location_matrix, location->node_index != NONE && (location->node_index & 0x8000));
		}
	}
}


// @retail 0x178eb0
s_particle_system_datum *function_178eb0(long effect_index, long definition_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_event *event = &TAG_GET(s_effect_definition, effect->tag_index)->events[effect->event_index];
	s_effect_particle_system_definition *definition = &event->particle_systems[definition_index];
	long mode = 0;

	if (effect->unknown58 != NONE && !function_155760(effect->unknown58))
		mode = 1;
	if (function_17b270(effect, (short const *)definition, mode))
	{
		long location_index = definition->location_index;

		if (location_index != NONE)
		{
			long first_location_index = effect->location_indices[location_index];
			short location_mode = definition->location_mode;
			s_effect_location_datum *location = effect_location_next(effect, &first_location_index, location_mode);

			if (location)
			{
				short placement = definition->placement;
				if (!function_178020(placement, &location->matrix.position, &effect->location))
					return 0;
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
					particle_system_link(particle_system, &effect->first_particle_system_index, &effect->last_particle_system_index);
					particle_system->unknown24 = location_index;
					function_178c80(0.0f, effect, particle_system, definition, 0.0f, definition->unknown0c != 0);
					return particle_system;
				}
			}
		}
	}
	return 0;
}

/* the particle system an effect keeps for one of its event's particle system definitions */
PRIVATE inline s_particle_system_datum *effect_event_slot_particle_system(s_effect_datum *effect, s_effect_particle_system_definition const *definition)
{
	s_particle_system_datum *particle_system = 0;

	for (dword j = 0; j < 16; j++)
	{
		s_effect_event_slot *slot = &effect->event_slots[j];

		if ((s_effect_particle_system_definition const *)slot->unknown0 == definition)
		{
			particle_system = DATUM(g_510c74, s_particle_system_datum, slot->unknown4);
			break;
		}
	}
	return particle_system;
}

PRIVATE __forceinline bool effect_event_placement_allowed(s_effect_particle_system_definition const *definition, s_effect_datum *effect, long const *mode)
{
	bool result = effect_placement_allowed_17b270(effect, definition->unknown10) != 0;
	if (*(short *)&g_4e8c20->unknown00[8] == 1)
	{
		if (result && (definition->location_mode != 2 || *mode == 0) && (definition->location_mode != 1 || *mode == 1))
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
	long mode = 0;

	if (effect->unknown58 != NONE && !function_155760(effect->unknown58))
		mode = 1;
	for (long i = 0; i < event->particle_system_count; i++)
	{
		s_effect_particle_system_definition *definition = &event->particle_systems[i];
		bool field_b4 = definition->unknown0c != 0;

		if (effect_event_placement_allowed(definition, effect, &mode))
		{
			s_particle_system_datum *particle_system = effect_event_slot_particle_system(effect, definition);

			if (particle_system || (particle_system = function_178eb0(effect_index, i)) != 0)
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
	s_effect_datum *const *effect_argument = &effect;
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
		dword *seed = TEST_FIELD_BIT(TAG_GET(s_effect_definition, effect->tag_index)->flag2) ? &g_4e7408->unknown0 : &g_4e7408->seed;
		real skip_roll = function_x82e52f(seed, __FILE__, __LINE__);
		if (!(skip_roll < definition->events[event_index].skip_chance))
			break;
	}
	return event_index;
}

// retail does not inline the delay draw (0x259d0) into this loop
#pragma inline_depth(0)
// @retail 0x1792b0
bool function_1792b0(long effect_index, real dt)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);

	if (!TEST_FIELD_BIT(effect->flag6))
	{
		long iterations = 0;

		while (dt >= 0.0f)
		{
			if (TEST_FIELD_BIT(effect->flag2) || iterations >= 8)
				break;
			real remaining = effect->event_delay - effect->unknown60;
			real start = effect->unknown60;
			real step = remaining > dt ? dt : remaining;
			bool finished;

			if (dt >= remaining)
			{
				finished = true;
				effect->unknown60 = effect->event_delay;
				dt -= remaining;
			}
			else
			{
				finished = false;
				effect->unknown60 += dt;
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
						{
							effect = 0;
							goto event_done;
						}
						break;
					}
					function_1782a0(effect_index, (short)event_index);
				}
			}
			else if (finished)
			{
				s_effect_event *event = &definition->events[effect->event_index];

				word *flags = &effect->flags;

				*flags |= 1;
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
event_done:
	return effect == 0;
}

#pragma inline_depth()

// MiniMax R (11-01): no new form, no lower score: retail still reloads the object's index after
// the 0xbadc0 validity call, and 0xbadc0's body is in another file.
// MiniMax R (10-31): no new form, no lower score: the object index is the only difference and it is 0xbadc0's
// and 0xbaf80's bodies (src/unknown_0bad50.cpp) that decide it.
// score 63 (was 63): only the object index differs: ours keeps it in ESI from entry across 0xbadc0
// Four-byte frame agrees; initial object handle remains retained across read-only queries.
// Earlier: only the object index differs: ours keeps it in ESI from entry across 0xbadc0
// and 0xbaf80 (NONE in EBX), retail reloads it after each call with NONE in ESI. Retail's callers
// 0x10a390 and 0x16c2b0 keep an index across 0xbadc0, so its summary is not the cause. Retail passes
// 0xbaf80's result (still in EAX) as 0x1794a0's first argument, not the effect's object index (BLOCKED-BY
// 0xbadc0: retail reloads the index after it; matched 0x138ab0 gets the same reload by reading the
// field through its global again). The merged destruction call's place (ours keeps the invisible
// arm's copy before the visible tests, retail the visible arm's copy) also differs.
// Tried: deleted effect cleared without the gotos, later block guarded by `effect &&` 63 (kept: gives
// retail's destruction block placement); one destroy call through a flag 137; root index as 0x1794a0's
// first argument 197 (585 bytes; aligned 88 -> 61: the cached index then takes ESI where retail keeps
// NONE, and EDI is reloaded), with the visible arm first 224, result false by default 197, the index
// named in the arm 197; the object test as a second `if (effect && ...)` reached from the deletion path
// 207 (retail's reload appears, NONE moves to EBX), that with the root index 130; the index named at
// entry for the first test 63; the second test nested in `if (effect)` 207; invisible arm's flag test
// the other way round (call to 0x177260 first) 87 (kept), with the visible arm written first 183, with
// the visible arm's flag test also swapped 163; visible arm first, invisible arm unswapped 163; only
// the visible arm's flag test swapped 178; visible arm as `flag2 && flag4` then `flag2` 138; update
// word 109; response word 119; both words 119; native missing location 204; root location value 194;
// root velocity value 198; both root values 218; origin value 109; native velocity/location/origin/all
// copies 109; native color record 109; default visibility 109; short origin index 109; shared
// destruction call 122; const tag/root 109/109; external datum reference 109; visibility result tests
// 189; inline depth one/two 109/109; global optimization off 550; frame pointer 200; external tag
// record 109; external root record 109; wide visibility result 144; byte visibility result 109;
// unsigned color handle 109; root velocity address 109; root location address 109; origin output
// lifetime 247; tag at every use 205; integer destruction result 230; word visibility 130; tag/result
// 244; tag/word 212; result/word 239; all three 248; unsigned result with all 248; start/visibility/both
// inline declarations 87/87/87; start noinline alone/with inline visibility 87/87; entry visibility
// alone/with both inline/with signed-byte result 87/87/87; the else-if test read through the datum
// lookup 87; root index with the first test through the lookup 212; only the first test through the
// lookup 203; deletion path returning true with a plain second if 198; else-if test and 0xbaf80
// argument both through the lookup 87.
// 0x10a390 and 0x16c2b0 keep an index across 0xbadc0, so its summary is not the cause. 0x1794a0's
// first argument is the root index in retail (EAX after 0xbaf80).
// Round 72: deleted effect cleared without the gotos, later block guarded by `effect &&` 63 (kept:
// gives retail's destruction block placement); one destroy call through a flag 137; root index as
// 0x1794a0's first argument 197; the object test as a second `if (effect && ...)` reached from the
// deletion path 207 (retail's reload appears, NONE moves to EBX); that with the root index 130;
// the index named at entry for the first test 63; the second test nested in `if (effect)` 207.
// Object-index reload (BLOCKED-BY 0xbadc0: retail reloads the index after it, so
// retail's 0xbadc0 is not read-only to LTCG) and the merged destruction call's place (ours keeps the
// invisible arm's copy before the visible tests, retail the visible arm's copy) still differ.
// Round 88: invisible arm's flag test the other way round (call to 0x177260 first) 87 (kept);
// with the visible arm written first 183; with the visible arm's flag test also swapped 163;
// visible arm first, invisible arm unswapped 163; only the visible arm's flag test swapped 178;
// visible arm as `flag2 && flag4` then `flag2` 138.
// Earlier: object-index reload and visibility joins differ. Update word 109; response
// word 119; both words 119; native missing location 204; root location value 194; root velocity value 198;
// both root values 218; origin value 109.
// object index remains cached across the validity call; visibility blocks differ.
// Native velocity/location/origin/all copies 109; native color record 109;
// default visibility 109; short origin index 109; shared destruction call 122.
// frame agrees; object-index caching and visibility block order differ.
// Const tag/root 109/109; external datum reference 109; visibility result tests 189;
// inline depth one/two 109/109; global optimization off 550; frame pointer 200.
// Four-byte frame agrees; cached object index across validity query and visibility/destruction
// joins differ. external tag record 109; external root record 109; wide visibility result 144;
// byte visibility result 109; unsigned color handle 109; root velocity address 109; root location
// address 109; origin output lifetime 247.
// Round 208: tag at every use 205; integer destruction result 230; word visibility 130;
// tag/result 244; tag/word 212; result/word 239; all three 248; unsigned result with all 248.
// Round 333: Start/visibility/both inline declarations 87/87/87; start noinline alone/with inline visibility 87/87; entry visibility alone/with both inline/with signed-byte result 87/87/87. All restored; object reload and merged-call placement remain.
// Round 135: retail passes 0xbaf80's result (still in EAX) as 0x1794a0's first argument, not
// the effect's object index; with that root index 197 (585 bytes; aligned 88 -> 61: the cached
// object index then takes ESI where retail keeps NONE, and EDI is reloaded); with the visible
// arm first 224; result false by default 197 (87 without the root index); the object index
// named in the arm 197. Not kept until the reload blocker is gone.
// Round 157: release inline 63; destruction inline 63; target inline 63; release/destruction inline 63;
// release/target inline 63; destruction/target inline 63; all three inline 63; release/destruction inline
// with separate noinline release 63. Original executable source restored.
// Round 147: update mask 202; response masks 167; both masks 215; admission returns 201;
// returns/update 215; returns/response 200; returns/both 228; shared admission/root handle 198.
// All executable forms restored; sources and comparisons saved.
// Round 282: tag acquired before root query, after query and after root copies
// 85/113/89, each with ordinary/read-only inputs; external tag references before query
// and after copies 85/89. All restored; cached object across validity query remains.
// Round 95: root index (0xbaf80's result) as 0x1794a0's first argument 197 (585 bytes; the index
// stays in ESI); location stores cluster first 63, with the root index 197; chained NONE location
// stores 63, with the root index 197. Original body restored.
// Round 7 (lane 14): root index named and passed first 197 (585 bytes; aligned 72 -> 48), named
// without the argument change 63, with the entry index local 176 (579 bytes); entry index local
// alone 198 (571 bytes); validity result compared with 0 63. BLOCKED-BY 0xbadc0/0xbaf80
// (src/unknown_0bad50.cpp, avoid-listed this round).
// Round 165: tail reading the field instead of its local 63 (identical: the field is cached across
// 0xbadc0); one chain (`== NONE` arm, validity test, body) 198; second `if (effect && ...)` with the root
// index and the tail field 130; outer `NONE || valid` test with the deletion in the else arm 63. Probe: the
// second if, root index and 0x1794a0's index through a fresh datum lookup 177 but aligned 23, the head's
// reloads then match exactly, so 0xbadc0/0xbaf80 being read-only to LTCG here is the blocker.
// (src/unknown_0bad50.cpp, avoid-listed this round). Round 99: from the 0x1794a0 call to the end
// ours and retail agree instruction for instruction; every difference is the cached object index
// across 0xbadc0 (no new forms: its file was avoid-listed again).
// Round 95: root index (0xbaf80's result) as 0x1794a0's first argument 197 (585 bytes; the index
// stays in ESI); location stores cluster first 63, with the root index 197; chained NONE location
// stores 63, with the root index 197. Original body restored.
// Round 172: retail calls 0x1794a0(root index, object index) (the first argument's header flag is
// tested). With the root index: second test as `NONE != ...` 197; a bool helper for the first test 197;
// `if (effect && effect->object_index != NONE)` 130 (retail's head and reload, but the reloaded index is
// then cached in ESI for 0x1794a0 and NONE takes EBX); nested head ifs 130; tail reading the field 130.
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
			if (!visible)
			{
				if (!TEST_FIELD_BIT(definition->flag0))
				{
					if (!TEST_FIELD_BIT(effect->flag2))
						function_177260(effect_index, false);
				}
				else if (function_177610(effect_index))
					effect = 0;
			}
			else if (TEST_FIELD_BIT(effect->flag2))
			{
				if (TEST_FIELD_BIT(effect->flag4))
				{
					if (function_177610(effect_index))
						effect = 0;
				}
				else
					function_177310(effect_index);
			}
		}
		if (effect && (TEST_FIELD_BIT(definition->flag3) || TEST_FIELD_BIT(definition->flag4)))
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

static inline bool effect_object_visible_179730(s_effect_datum *effect)
{
	return function_3eb20(effect->location.cluster_index) && effect->object_index != NONE;
}

// @retail 0x179730
bool function_179730(long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
	bool hidden = true;

	if (effect->location.cluster_index != NONE)
	{
		if (effect_object_visible_179730(effect) || (g_510c50 && ((byte *)g_510c50)[5]))
			hidden = false;
		else if (TEST_FIELD_BIT(definition->flag2))
			hidden = !function_11c080(&effect->location);
		else
			hidden = !function_11c050(&effect->location);
	}
	if (hidden != (((byte)effect->flags >> 3) & 1))
	{
		word *flags = &effect->flags;

		if (hidden)
			*flags |= 8;
		else
			*flags &= ~8;
		if (hidden && !TEST_FIELD_BIT(effect->flag1) && !TEST_FIELD_BIT(effect->flag9) && function_177610(effect_index))
			effect = 0;
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

static __forceinline s_effect_location_datum *effect_location_next_179fb0(s_effect_datum *effect, long *location_index)
{
	s_effect_location_datum *location = 0;

	if (*location_index != NONE)
	{
		location = DATUM(g_4ea938, s_effect_location_datum, *location_index);
		*location_index = location->next_index;
		if (location->node_index != NONE && (location->node_index & 0x8000))
			location = effect_location_next(effect, location_index, 0);
	}
	return location;
}

/* pushes the effect's object along each acceleration of the event, from every location */
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

				while ((location = effect_location_next_179fb0(effect, &location_index)) != 0)
				{
					point3f point;
					vector3f direction;

					if (location->node_index != NONE)
					{
						transform4x3f matrix;

						function_17aec0(location->node_index, effect, &matrix);
						effect_matrix_transform_point(&matrix, &location->matrix.position, &point);
						effect_matrix_transform_normal_179880(&matrix, &location->matrix.forward, &direction);
					}
					else
					{
						point = location->matrix.position;
						direction = location->matrix.forward;
					}
					vector3f impulse;

					random_vector_in_cone(&direction, &impulse, &g_4e7408->unknown0, acceleration->inner_cone_angle, acceleration->outer_cone_angle);
					normalize3d(&impulse);
					vector3f_scale(&impulse, acceleration->acceleration * object_definition->unknown14, &impulse);

					s_effect_object *object = OBJECT_GET(effect->object_index);

					object->owner_unknown4 = effect->owner.unknown4;
					object->owner_unknown0 = effect->owner.unknown0;
					object->owner_unknown8 = effect->owner.unknown8;
					function_b7880(effect->object_index, NONE, &point, &impulse, NULL);
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
		real axis_k = axis.k;
		real t = (direction->i * axis.i + direction->j * axis.j + direction->k * axis_k) * (1.0f - c);
		real cross_j = direction->k * axis.i - direction->i * axis_k;
		real cross_k = direction->i * axis.j - direction->j * axis.i;
		real cross_i = direction->j * axis_k - direction->k * axis.j;

		direction->i = direction->i * c + axis.i * t - cross_i * s;
		direction->j = direction->j * c + axis.j * t - cross_j * s;
		direction->k = direction->k * c + axis_k * t - cross_k * s;
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

// score 30 (was 30): Frame 0x100, length and prologue agree; the decal call's register arguments (BLOCKED-BY
// 0x17e670), the attached light call's register arguments (computed before its pushes, retail after: ours forms
// forward with add ecx, retail with lea edx and keeps the location pointer live) and the sound arms' scratch
// registers remain. Round 85: `effect` declared before `up` gives retail's EAX up (38 -> 30, kept);
// Frame 0x100, length and prologue agree; protected decal transfer, attached
// light argument scheduling and sound scratch carriers remain.
// Earlier 30 (was 38): Frame 0x100, length and prologue agree; the decal call's register arguments (BLOCKED-BY
// 0x17e670), the attached light call's register arguments (computed before its pushes, retail after) and the sound
// arms' scratch registers remain. Round 85: `effect` declared before `up` gives retail's EAX up (38 -> 30, kept);
// then velocity before location in the detached sound arm 47; light/sound node index in the argument list 30/30;
// one inline node-index helper for three arms 415; sound record at the case scope 30.
// Earlier measurements and forms:
// Frame 0x100 and length agree; incoming up/effect carriers, projection call, light scheduling and sound publication remain.
// frame 0x100 and length agree. Retail passes up in EAX (moved to EDI) and effect
// on the stack; ours passes effect in EAX and up on the stack whatever the declared order, which also
// moves two scratch registers in the sound arms. 0x17e670's register arguments (BLOCKED-BY 0x17e670,
// contributor) swap g_510c78 and the point; the attached light call computes its up pointer after the
// up/position registers (retail before), and the detached sound arm loads g_4687a4 into EAX after
// storing the compressed forward (retail ECX, before the store).
// Tried: placement name 345; request name 345; retained decal seed pointer 313; direct case returns
// 345; failed-placement guard 345; radius case lifetime 313; shared light call 380; initialized node
// masks 411; damage-output reuse 313; sound aggregate 313; direct radius on the seed-pointer body 344
// (frame 0x100); shared direction output 313; owner/location CRT copies 313; sound stores before
// compression 313; detached arms first 318; early damage scale 313; shared damage direction 313;
// retained light object 313; retained sound object 313; object-case random record 313; full-width
// sound node 313; word flags/admission 319/319; word flags/direct radius 309 plain (caller 200 versus
// 166, rejected); admission/chained copies/shared seed 313 each; word flags/shared seed 319; vector
// CRT copies 313; byte detached with word flags/direct radius 309 (caller 200); native velocity loop
// 313; native placement component loop 342; detached flag first 318; separate character guards 313;
// location-before-owner copy 319; native point/up/forward/all input references 313 each; all
// references/direct decal radius 344; decal velocity at entry/created-object guard/native node
// admission/active cases by value/immutable sound record 313 each; direct default flag 322; position
// before origin/case order plus velocity lifetime 313/313; with the matched light call: native light
// node/radius argument/both 177/203/286; uniform live decal seed 248, with native node/radius/both
// 190/208/277; both native attachment nodes with radius argument 286; native light-node result 177
// (kept). Round 60: radius drawn in 0x17e670's argument list 286 (frame 0x100); with the seed address
// at each use 277; with a random-globals local 285; that plus the part flags as a word bitfield
// (include/effects.h) 62 (kept); with a plain seed pointer instead of the random local 109; up or
// effect declared first/last 62/38 (no change); damage scale assigned before the point copies 55
// (kept); matrix pointer local in the light call 55; sound location before its velocity 38 (kept);
// short node index in the light call 126; long node index in the sound call 38; compressed forward
// before the point copy 58; light up pointer named before the call 38.
// Frame 0x104 versus 0x100 and point/up eligibility remain; native formals do not move them.
// Earlier: Frame 0x104 versus 0x100 and point/up eligibility remain; native input-reference contracts leave caller and callee unchanged.
// frame 0x104 versus 0x100, incoming point/up eligibility and record/call setup remain.
// frame 0x104 versus 0x100; point/up eligibility and record/call setup remain.
// Placement name 345; request name 345; retained decal seed pointer 313;
// direct case returns 345; failed-placement guard 345.
// Radius case lifetime 313; shared light call 380; initialized node masks 411; damage-output reuse 313;
// sound aggregate 313; direct radius on the seed-pointer body 344 (frame 0x100).
// Shared direction output 313;
// owner/location CRT copies 313; sound stores before compression 313; detached arms first 318; early damage
// scale 313.
// Shared damage direction 313; retained light object 313; retained sound object 313;
// object-case random record 313; full-width sound node 313.
// Word flags/admission 319/319; word flags/direct radius 309 plain (caller 200 versus 166, rejected); admission/chained copies/shared seed 313 each; word flags/shared seed 319; vector CRT copies 313; byte detached with word flags/direct radius 309 (caller 200).
// Native velocity loop 313; native placement component loop 342; detached flag first 318; separate character guards 313; location-before-owner copy 319.
// Round 123: Native point/up/forward/all input references 313 each; all references/direct decal radius 344. All pointer contracts and sole caller restored.
// Decal velocity at entry/created-object guard/native node admission/active cases by value/immutable sound
// record 313 each; direct default flag 322; position before origin/case order plus velocity lifetime 313/313.
// With the matched light call: native light node/radius argument/both 177/203/286;
// uniform live decal seed 248, with native node/radius/both 190/208/277;
// both native attachment nodes with radius argument 286. Callers and light callee checked.
// Native light-node result 177 at 1330/1328 bytes, kept after ordinary full check;
// every other form restored. No call expression or input value changed.
// Round 157: early radius zero 177; early character node default 177; early light node default 177; early
// sound node default 177; all node defaults 177; radius/light defaults 177; radius/all node defaults 177;
// all defaults with paired record names 177. Original executable source restored.
// Round 147: location reference 177; effect reference 177; part reference 177; location/effect
// 177; location/part 177; effect/part 177; all record references 177; all references/point value
// 350; direction inline 177; angular inline 177; both vector inline 177; light inline (lost
// matches) 177. All executable forms restored; sources and comparisons saved.
// Round 155: owned input values point 421; forward 434; both 466; up 424; point/up 483;
// forward/up 493; all 520; all with shared point setters 560; target inline 38;
// native up CRT copy 38; uniform placement CRT copies 38. Owned up restores the incoming
// EAX up pointer but adds entry copies; every executable form restored.
// Round 204: native placement up/forward/both/all transfers 119/221/237/250; placement plus damage vectors/all damage/all sound/velocity 374/416/427/427; definition after caller 38. Original executable source restored.
// Round 379: Sound point/velocity/both setters 60/57/72; location before point/compressor 36/34; location and velocity before point 36; early location with velocity/point setter 61/61. Original body restored.
// DH round 218: sound point/tail/all CRT copies 30/30/30; sound case tag/native or point CRT 69/69;
// writable/const sound aggregates 47/47; aggregate plus sound tag 70. Opening body restored.
// Round 88: named forward pointer in the attached light call 30; word sound node index 52 (1330 bytes);
// named position pointer in the attached sound call 30; light node index at case scope 30; light test
// as !detached first 35.
// Round 383: native light/sound/both matrix subobject references 30/30/30; sentinel-result
// assignments for light/sound/both with matrix receivers/all nodes 30/30/30/30; joint receiver/result
// graph with the sound workspace renamed 30. Original executable source restored.
// Round 170: detached sound velocity before location/before the compression 47/49; compression before
// the point copy 35; attached light matrix pointer before the node index 30; attached sound scale local 30.
// Round 140 (lane 4): lead: 0xc0350 declared (tag, object, node, position, forward, up, scale), definition,
// declaration and both calls, keeps 0xc0350 matched and gives the attached light call exactly (23: the
// stack up pointer in EDX before the pushes, the EDI/EBX register arguments last); not kept because
// src/unknown_0bffa0.cpp was another lane's file that round. 0x189060's scale push (EDI, retail EAX) looks
// like the same cause (forward declared before scale), its definition sits in src/unknown_189010.cpp.
// Tried: detached sound point copied member-wise 60; location field by field 73; detached arm first with
// the test inverted 30 (same code); tag index local before the fill 35; the record as one aggregate
// initializer 47 (as struct-order statements, re-measured 47; 40 on top of the 0xc0350 order).
// @retail 0x17a8a0
void function_17a8a0(s_effect_location_datum *location, bool detached, s_effect_datum *effect, vector3f const *up, s_effect_part *part, point3f const *point, vector3f const *forward, real scale)
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

		data.unknown7c = NONE;
		function_d6660((s_type_1e6529 *)&data, part->tag_index);
		data.owner = effect->owner;
		data.location = effect->location;
		data.scale = scale;
		data.origin = *point;
		data.position = *point;
		data.direction = *forward;
		data.forward = *forward;
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

			function_c0350(part->tag_index, effect->object_index, node_index, &location->matrix.up, &location->matrix.position, &location->matrix.forward, scale);
		}
		else
		{
			function_c0350(part->tag_index, NONE, NONE, up, point, forward, scale);
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
			position.location = effect->location;
			position.velocity = *g_4687a4;
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

static inline vector3f *effect_matrix_transform_normal_17a380(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	real x = vector->i;
	real y = vector->j;
	real z = vector->k;
	out->i = matrix->up.i * z + matrix->left.i * y + matrix->forward.i * x;
	out->j = matrix->up.j * z + matrix->left.j * y + matrix->forward.j * x;
	out->k = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x;
	return out;
}

/* creates the current event's parts at each of their effect locations */
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
		bool reversed = TEST_FIELD_BIT(effect->flag5);
		short mode = part->create_in_mode;
		bool in_mode = reversed ? mode != 1 : mode != 2;
		if (!in_mode)
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

				function_17aec0(location->node_index, effect, &matrix);
				effect_matrix_transform_point(&matrix, &location->matrix.position, &point);
				effect_matrix_transform_normal_17a380(&matrix, &location->matrix.forward, &forward);
				effect_matrix_transform_normal_17a380(&matrix, &location->matrix.up, &up);
			}
			else
			{
				point = location->matrix.position;
				forward = location->matrix.forward;
				up = location->matrix.up;
			}
			bool oriented = TEST_FIELD_BIT(part->flag0);

			if (oriented)
			{
				forward = *g_4687bc;
				up = *g_4687a8;
			}
			bool scattered = TEST_FIELD_BIT(part->flag1);

			if (scattered)
			{
				real upper = part->radius_upper;
				real lower = part->radius_lower;
				real radius = lower + (upper - lower) * (function_x82e52f(&g_4e7408->seed, __FILE__, __LINE__));
				real scatter_radius;
				long object_index = effect->object_index;

				if (object_index != NONE)
					object_index = function_baf40(object_index);
				function_16a8e0(0xd800005, &point, radius, object_index, NONE, &point, &scatter_radius);
				detached = true;
			}
			create = function_178020(part->create_in, &point, &effect->location);
			if (create)
			{
				real scale = 1.0f;

				if (part->a_scales & 0x20)
					scale = effect->scale_a;
				if (part->b_scales & 0x20)
					scale = effect->scale_b * scale;
				if (!TEST_FIELD_BIT(effect->flag10) || part->base_group_tag != 'obje')
					function_17a8a0(location, detached, effect, &up, part, &point, &forward, scale);
			}
		}
	}
}

// score 93 (was 93): frame 0x4c versus 0x54 (retail spills the definition and keeps two more
// dwords), the particle system in EDI (retail ESI), color B spilled (retail EDI) and the cached
// mode differ. Tried: transform name 109; query results, particle cursor and colors at entry 109;
// read-only settings 109; immutable mode once 96 (kept); size policy 146; inline depth zero 109;
// deferred particle cursor 125; active-only effect cursor 125; particle-location record 109;
// system/definition/type/effect records 109 each; exhausted-particle continue 109; type query,
// color update, matrix query and location update noinline declarations 96 each; signed-byte flag
// 108; word flag 106; unsigned successor 96; read-only transform result 96; unsigned-byte location
// flag 108; mode after color 112; late signed mode 112; late effect cursor home 112; late particle
// cursor home 112; early mode traversal transform/node observation/transform home/query scope 96
// each; native mode 96; entry word/native mode 108/109; unsigned particle cursor alone/with native
// mode 96/96; node predicate at use 109, with native mode 110; late native mode 106.
// Round 88: the next index without `volatile` 122 (the effect index moves to ECX and caller
// 0x176f60 stops matching: the volatile is what keeps it on the stack); mode read per call 108;
// the effect index read once through its address with a plain next index 105; multiplied() in
// the call's argument list (retail pushes it from EAX) 103, with per-call mode 110; an inline
// color helper taking the particle system and both colors (retail reads both colors before the
// definition lookup; same block in 0x178c80) 102, with the address-read effect index 92 (retail's
// two exits; not kept without a match), with per-call mode 127, particle system declared first
// 113, helper parameters reordered 92, helper fed from the color locals 93.
// Round 204: Actual query-result carriers (tinted/multiplied): bool/byte 96, bool/long 119, byte/bool 95,
// byte/byte 93, byte/long 124, long/bool 135, long/byte 124, long/long 138. One- and three-point reductions
// are rejected. Frame 0x50, system/color registers and call staging remain; original query types and body
// restored.
// Round 165: the next index without `volatile` and `multiplied()` as the call's argument (retail pushes it
// from EAX) 93, kept: the effect index stays on the stack and 0x176f60 matches; without volatile alone 105.
// Left: the particle system in EDI and the next index in ESI (retail the other way round), the mode cached
// where retail reads it per call. On that form: mode per call 108 and colors read at the call 135 (both make
// the effect index a register argument, 0x176f60 25); particle system looked up first 114; effect index
// read through its address with mode per call 127; early `return next_index;` 97; next index read after
// the definition call 98; color B declared first 93 (color A then spills as in retail); an inline color
// helper shared with 0x178c80 93 (identical); a 32-combination screen of these found none below 93.
// Round 186: Getter definition/member declaration inline with bool/bool, byte/bool, byte/byte, bool/byte
// query results: 96/95/93/96; all lose matched 0x247470 and are rejected. With separate noinline member
// declaration preserving retail calls: 96/95/93/96, no checked match loss. One- and three-point reductions
// fail retention. Original getter definition/declaration, query types, successor qualifier, mode, every
// call tuple and executable body restored. Frame 0x50 versus 0x54, system/color homes and color-call
// staging remain.
// Round 307: Coordinated effect-first/system-second inputs with mode/tint/cursor lifetimes
// 93/94/106/112/120/110/117/113; direct mode reads also lose the matched caller.
// Original definition, declaration, input order and caller restored; frame 0x4c versus 0x54 remains.
// Round 170: retail's 0x54 frame holds the particle-location cursor as `spawn.location_index` of an
// s_particle_system_spawn local (its two unused dwords sit before it): that alone 90 (frame and every
// slot home agree, 0x176f60 matches; gain under 8, not kept); with the mode read per call as retail
// 109 (the effect index turns into a register argument, 0x176f60 25), plus an address read of the
// effect index 128; system looked up first 114. On the spawn form: colours after the cursor 90, mode
// first 90, mode after the colours 89, colour B in the call 129, b4 first 111, early return 94, shared
// colour helper (system, A, B) 90; per-call-mode colour/scope/b4/next variants 107-122.
// Round 9 (lane 2, fast; 0x176f60 matches unless given): spawn cursor 90; the cursor assigned before the
// colour reads 89; spawn cursor with the mode read per call 109 (0x177040 then takes the effect index in
// EAX and the particle system in ECX, 0x176f60 25); with 0x178c80's colour block (colours read at the
// call, `multiplied` named) 116 (325 bytes, 0x176f60 25); that block with the mode per call 101 (332
// bytes, 0x176f60 25). The one-file harness gives the effect index a register too, so it cannot screen
// this convention.
// Round 172: LTCG takes this convention from the body's allocation. Mode read per call (retail's shape,
// frame 0x54) 108: the effect pointer is computed in EAX and both indices become register arguments;
// parameters swapped 93 (cached, identical)/120 (per call); per call with the index read through its
// address 127; definition before the next index 114; system looked up first 119; shared colour helper
// (system, A, B) 92.
// Round 210: Native immutable settings value with cursor/tint/node combinations
// 109/109/108/108/109/109/108/108. The full value expands frame 0x4c to 0x84 instead of retail
// 0x54 and every form loses matched caller 0x176f60 through the generated input convention.
// Original settings pointer, cached mode, query carriers, cursor and complete body restored.
// Round 46 (lane 9), on the spawn-cursor form (89 here): colour B read first 90; next index read after the
// definition call 96, with colour B first 96; both colours declared inside the type-query block 90 (aligned 27:
// the loop is then retail's except the cached mode push; left are the system in EDI, colour A in EBX, colour B
// spilled and the type object in ESI, retail ESI/spilled/EDI/EBX), that with the mode read per call 109 (register
// effect index, 0x176f60 25), with the mode declared after the colour call 117, with a signed mode 90.
// Round 8 (lane 18, fast; 0x176f60 matches unless given): on this cached-mode form, both colours declared
// inside the type-query block 93; the next index declared before the lookups 93; the particle-location
// cursor declared before the definition test 93. Mode read per call re-measured 108 (0x176f60 25): it gives
// retail's 0x54 frame, but the definition takes EBX across the loop (retail spills it to [esp+0x10] and
// reloads it per call) and both indices become register arguments. The node-flag test with an explicit
// `!= 0` 93; the cached mode declared first 94. Lead: 0x175a80 (its colour callee,
// unmatched, in a file held this round) sets which registers this call's setup may keep.
// Round 59 (lane 16, fast with callees and 0x176f60): spawn cursor re-measured 89; spawn at function scope 89
// (identical); spawn with the colours declared before it 90; spawn with the mode read per call and the
// colours read at the call 122 (320 bytes, 0x176f60 25), with the particle system looked up first 121 (348
// bytes, 0x176f60 25), with spawn at function scope 109 (0x176f60 25). With the mode per call the effect
// index is computed in EAX and moved to EBP (retail computes it in EBP from the stack slot), so LTCG passes
// it in a register, and the definition takes EBX where retail spills it. 0x175a80 gets retail's registers.
// Round 130 (lane 6, fast with 0x176f60): retail's 0x54 frame reads as the spilled definition at [esp+0x10],
// two unused dwords and the particle-location cursor (a 12-byte record like the spawn record of 0x178c80)
// ahead of the 0x34-byte matrix; that layout needs the per-call mode (convention change above). On this
// cached-mode body: the type-query block's braces removed 93; the particle-location cursor declared at function
// scope 93; the matrix declared outside the loop 93; the particle-location pointer declared before its test 93
// (all four the same code); `tinted()` called in the colour call's argument list 120 (325 bytes, 0x176f60 25).
// Round 126 (lane 5, fast; 0x176f60 matches unless given): spawn cursor re-measured 90 (only the cached
// mode push, one instruction short per call, and the system/next/colour/type registers left); spawn with an
// early `return next_index;` on the definition test 94; the mode per call with that early return 111
// (0x176f60 25), with the spawn cursor too 112 (0x176f60 25); spawn with the particle system looked up
// before the effect 113; spawn with both colours declared inside the type-query block and the next index
// read after the definition call 95.
// Round 47 (lane 11, fast; 0x176f60 matches in each): the next index read straight off the datum lookup before
// the particle system pointer is named 118 (345 bytes), on the spawn-cursor form 115 (345 bytes); the spawn
// cursor (re-measured 90) with the cached mode as a long local 89 at retail's 337 bytes (frame 0x54, every
// slot home as retail, the mode in the definition's slot loaded by movzx; left: the particle system in EDI
// and the next index in ESI, so the colours and type object rotate, and the mode cached where retail reloads
// the definition per call); the cached mode as short 93. On spawn + long mode: next index read after the
// location index 95; the cursor assigned before the colour reads 88 (best, gain 5, not kept); that with the
// mode read before the colours 88; particle system looked up before the effect 113 (345 bytes); next index
// declared before the lookups and assigned after 88; effect looked up after the definition 119 (346 bytes);
// early return on the definition test 95 (334 bytes).
// Round 126 (lane 5, fast; 0x176f60 matches unless given): spawn cursor re-measured 90 (only the cached
// mode push, one instruction short per call, and the system/next/colour/type registers left); spawn with an
// early `return next_index;` on the definition test 94; the mode per call with that early return 111
// (0x176f60 25), with the spawn cursor too 112 (0x176f60 25); spawn with the particle system looked up
// before the effect 113; spawn with both colours declared inside the type-query block and the next index
// read after the definition call 95.
// Round 493: native unsigned system/effect/successor handle subsets 93/93/93/93/93/93/93;
// native-int system on the unsigned effect/successor graph 93. Original input and result
// domains restored; frame 0x4c/0x54 and system/successor/color homes remain.
// Round 260: coherent separate inline color/location/matrix declarations, seven subsets 93;
// all three with per-call mode 108, losing the matched caller. Original declarations/body restored.
// @retail 0x177040
long __stdcall function_177040(long particle_system_index, long effect_index)
{
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);
	s_particle_system_datum *particle_system = DATUM(g_510c74, s_particle_system_datum, particle_system_index);
	long next_index = particle_system->next_index;
	s_effect_particle_system_definition *definition = particle_system->function_1751d0();
	long location_index = effect->location_indices[definition->location_index];

	if (definition->unknown0c != 0)
	{
		long particle_location_index = particle_system->location_index;
		dword color_a = effect->color_a;
		dword color_b = effect->color_b;
		s_effect_location_datum *location;
		word location_mode = definition->location_mode;

		{
			c_type_4e7709 *particle_definition = function_137bd0(particle_system->function_1751d0()->tag_index);
			bool tinted = particle_definition->tinted();

			function_175a80(color_a, color_b, particle_system, particle_definition->multiplied(), tinted);
		}
		while ((location = effect_location_next(effect, &location_index, location_mode)) != 0)
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

static inline point3f *effect_matrix_transform_point_1778d0(transform4x3f const *matrix, point3f const *point, point3f *out)
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
	out->x = (matrix->left.i * y + matrix->forward.i * x) + matrix->up.i * z + matrix->position.x;
	out->y = (matrix->left.j * y + matrix->forward.j * x) + matrix->up.j * z + matrix->position.y;
	out->z = matrix->up.k * z + matrix->left.k * y + matrix->forward.k * x + matrix->position.z;
	return out;
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
				long next = data_find_index(g_4e8c24, player_index + 1);

				if (next == NONE)
					break;

				s_effect_player *player = (s_effect_player *)(g_4e8c24->data + g_4e8c24->size * next);

				player_index = next;
				if (!player)
					break;
				if (player->unit_index != NONE)
				{
					s_effect_object *unit = OBJECT_GET(player->unit_index);
					long location_index = effect->location_indices[0];
					s_effect_location_datum *location;

					while ((location = effect_location_next_inline(effect, &location_index, 0)) != 0)
					{
						point3f point;
						vector3f delta;

						if (location->node_index != NONE)
						{
							transform4x3f matrix;

							function_17aec0(location->node_index, effect, &matrix);
							effect_matrix_transform_point_1778d0(&matrix, &location->matrix.position, &point);
						}
						else
						{
							point = location->matrix.position;
						}
						vector3d_from_points3d(&unit->bounding_center, &point, &delta);

						real radius = unit->field_xc9d3f8 + definition->unknown08;

						if (length_sq3f(&delta) <= radius * radius)
						{
							result = true;
							goto done;
						}
					}
				}
			}
		}
		effect_index = data_datum_index(g_4ea93c, data_find_index(g_4ea93c, effect_index == NONE ? 0 : (effect_index & 0xffff) + 1));
	}
done:
	return result;
}

bool function_3e9c0(long object_index);
bool function_2dba0(long tag, vector3f const *direction, bool alternate, long c, long d, long e, point3f const *position, color3f const *color, real alpha, real amount, real scale);
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

// score 74 (was 74): the scale block and the point transform's y copy remain; the lens call now agrees (round 67).
// Earlier: frame 0x94 agrees; the scale default/conditional branch, the point transform's
// y copy (retail keeps y in xmm5 and copies it to xmm4; ours scales it in place) and the lens call
// setup remain. The lens call is BLOCKED-BY 0x2dba0: retail passes its tag index in ecx and the
// position on the stack, ours the other way round (its body is in a contributor's file).
// frame 0x94 agrees; the scale default/conditional branch and the point transform's
// y copy (retail keeps y in xmm5 and copies it to xmm4; ours scales it in place) remain. The lens call
// now takes retail's registers (0x2dba0 matched in round 104; it was BLOCKED-BY 0x2dba0: retail passes
// its tag index in ecx and the position on the stack).
// frame 0x94 agrees and the lens call setup now matches (it was blocked by
// 0x2dba0's convention until round 59). Only two things remain: retail's scale is the default-if form
// (`real scale = 1.0f; if (a) scale = scale_a; if (b) scale *= scale_b;`), and the point transform's
// y copy (retail keeps y in xmm5 and copies it to xmm4; ours scales it in place). Default-if with the
// y copy solved would be retail's 1525 bytes; default-if alone is 1519 bytes and scores 272 because
// the missing copies shift every later instruction.
// Point/normal/both reverse captures 308/187/202; shared color scale 325; existing point operation
// 548; color with point/normal/both captures 324/327/327; lifetime 287; branch color 324;
// event/count names 287; part-pointer walk 394; conditional scale initialization 112. Read-only
// tag/event/part/all metadata 112 each; location cursor home 112; vector homes 112; bounded word
// flare count 208; transform/color homes 112; consistent alternate argument order 88. After the
// consistent call contract: default-first scale 272; default after point 270; default before color
// 270. Fast control 416 (reduced call/global caveats); scale workspace/test arms 416/416; default
// before inputs 425; conditional before inputs/after point/after forward/before color
// 416/416/416/416; B-first product 416.
// Aligned comparison (masked addresses, difflib) instead of the positional count: the original
// body differs in 21 instructions; `real scale = 1.0f; if (a) scale = scale_a; if (b) scale *= scale_b;`
// after the color copy 15 (272 positional, 1518 bytes), with the default declared before the color
// copy 13 (270): the scale block then matches and only the y copy (6 bytes) and the blocked lens call
// remain, so that form should be kept once the y copy is solved. On it: node matrix declared at the
// loop top 13 (270); up transformed before forward 29; forceinline point transform 13 (270).
// Consistent tag/direction formal positions: 82/81/81/414/414/83/82/88; none recovers
// tag ECX/position stack. Native position/forward/up/color 331/329/323/321; both directions
// 361; spatial inputs 392; point/color 328; all inputs 396. All source/contracts restored.
// Indexed point/directions/spatial/separate-spatial copies 323/331/352/511;
// the same copies with native default-first scale 313/333/344/498. All restored.
// Round 204: Complete transform-output boundaries: normal shared setter/named rows 88/187; point shared
// setter alone/with normal setter/with named normal rows 94/88/187; named point rows alone/with normal
// setter/with named normal rows 169/168/187. The two 1525-byte forms regress. Scale prefix, point Y-source
// copy and render-call roles remain; original helpers and body restored.
// Round 165: point transform from the location's position 403 (callers 0x176bb0/0x176cb0 lose); scale after
// the node transform 430; the shared point transform as `y * scale`, through a scale local, or with `*=` 88
// each (identical in all four users).
// Round 186: Immutable coordinate sources with two-arm scaled outputs 140; separate source/scaled
// coordinates 105; one matrix-scale observation 88; immutable sources plus one scale 184. With the known
// default-before-color prefix: 282/266/270/283. Every coordinate-ownership form loses matched 0x179fb0;
// native scale-only forms preserve it but give no gain. All original point/normal helpers, scale selection,
// source observations, in-place behavior and call tuples restored. Original 88 retained; contributor-owned
// 0x2dba0 contract remains untouched.
// Round 170 (plain checks; the reduced image scores 416 here): default-if scale 272 (1518 bytes);
// point transformed from the location's position 332; scale before the copies 95; `scale_b * scale`
// 88; default-if with `scale_b * scale` 272.
// Round 172 (a fast check with the callees listed reproduces 88): retail copies x and y before the scale
// branch, ours only x; no helper spelling (local orders, `*=`, a scale local) or placement changes it.
// Forward/up before the point 334; colour after the transform 219; position copied last 92; copies in
// up/forward/position order 92; default-first scale before the copies 312.
// Round 67 (lane 12; the lens call now agrees, so only the scale block and the y copy remain): the
// house scale form of 0x17a380 (`real scale = 1.0f;` declared before the color copy, `if (a) scale =
// effect->scale_a; if (b) scale = effect->scale_b * scale;`) 270 (1519 bytes: the scale block is then
// byte for byte retail's and the y copy is the only difference left); default-if after the color 272.
// Cause of the y copy, pinned in the compile-only screen: any other reference to `position.y` by name
// in this function (a store before the node test, or even after the transform in the tdtl arm) gives
// retail's `movss xmm5; movaps xmm4, xmm5` and scale-first product; on the house scale form the
// diagnostic store gives retail's whole transform block (295 only from the extra store and moved
// homes). Naming forward, up, color, position.x or position.z does not. Position copied field by
// field or by an aggregate initializer gives the y copy but movss copies (177/177 screen against 163);
// memcpy, a const source pointer, a separate declaration, function-scope position, the copy after up
// and renames give no change; the shared point helper reading a point copy or with an if/else
// 211/169 and worse in all three matched users.
// Round 105 (lane 7, fast with callers and callees; 0x2dba0 now matched): `real scale = 1.0f; if (a) scale =
// scale_a; if (b) scale *= scale_b;` after the colour copy 272 (1519 bytes, aligned 6), with the default
// declared before the colour copy 270 (aligned 4: only the y copy's two movaps and its register remain, 6
// bytes); on that, the node matrix declared at the loop top 270, the point transformed from the location's
// position 339, the mode as long 270. Compile-only screen of the y copy: every order of the copies and the
// scale block, unused/dead locals, and the shared point helper's local, scale-local and product orders: only
// helper forms that change matched 0x179fb0/0x17a380 move it; a field-wise position copy gives retail's y
// copy but turns the struct copy into float moves.
// Round 59 (lane 16, fast with the callees and both callers listed; a fast check of the function alone
// builds it differently): the lens call setup now matches, so only the scale block and the y copy remain.
// Default-if scale 272 (1519 bytes: the scale block is retail's but for one store pair; the six bytes are
// the two y-copy movaps, and the one-instruction shift counts every later line); with the scale declared
// before the copies 270; with the position copied component by component 86 (1538 bytes: x and y are then
// copied as retail but z is scaled in the scale register and moved back, and the copies become movss where
// retail copies dwords); with an aggregate-initialised position 86; with the transform reading the location's
// position 339. Compile-only screens of the 120 copy orders, real_point3d_set, the matrix at loop or function
// scope and the normals first never give retail's x and y copies with z scaled in place.
// Round 70 (lane 12, fast with callers and callees): on the house scale form (270): position/forward/up
// declared apart from their copies 270, at function scope 270; forward/up through the existing
// matrix_transform_vector 270 (same code); through the generic effect_matrix_transform_normal 383 (1431
// bytes). Compile-only: a 179880-only point helper with a scale local, `*=`, `x * scale`, deferred
// declarations, `!(scale == 1.0f)` or a vector local all give our y-in-place code; naming position.z
// alone (or with y) moves z instead, naming x changes nothing: retail names position.y and not z.
// Round 493: native indexed point input/output/both and complete direction access 74/74/74/74;
// each with default-before-color scale 270/270/270/270. Original helpers and body restored;
// scale prefix and point Y-copy remain.
// Round 260: complete native location matrix with original/house scale, original/shared normal
// kernel and color observation order: 410/420/410/420/410/420/410/420; frame 0xa4, all restored.
// Round 501: bounded iterator mode word/byte/signed-char/dword, with repeated/once-captured
// association index, 74/74/351/351/351/351/74/74. Original mode, observations and body
// restored; scale prefix and point Y-copy remain. Shared transforms stay unchanged.
// @retail 0x179880
void __stdcall function_179880(s_effect_datum *effect, long effect_index)
{
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
				real scale = (part->a_scales & 0x20) ? effect->scale_a : 1.0f;
				if (part->b_scales & 0x20)
					scale *= effect->scale_b;
				if (location->node_index != NONE)
				{
					transform4x3f matrix;
					function_17aec0(location->node_index, effect, &matrix);
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
						function_2dba0(part->tag_index, &forward, function_3e9c0(effect->object_index), 1, effect_index & 0xffff, flare_index, &position, (color3f const *)&effect->origin, 1.0f, scale, 1.0f);
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
		real progress = effect->event_delay > 0.0f ? effect->unknown60 / effect->event_delay : 0.0f;

		long location_index = effect->location_indices[beam->location];
		s_effect_location_datum *location;

		while ((location = effect_location_next_inline(effect, &location_index, 3)) != 0)
		{
			if (location->node_index != NONE)
			{
				transform4x3f node_matrix;
				transform4x3f matrix;

				function_17aec0(location->node_index, effect, &node_matrix);
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
	real dt = g_510c54->rate;
	long effect_index = data_datum_index(g_4ea93c, function_16bc00(g_4ea93c, 0));

	while (effect_index != NONE)
	{
		if (!function_1794e0(effect_index) && !function_179730(effect_index))
		{
			bool const &deleted = function_179810(effect_index);
			if (!deleted) function_1792b0(effect_index, dt);
		}
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
				function_178360(effect_index, effect->object_index, unknown58, 0, 0, NONE);
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
				function_178360(effect_index, effect->object_index, effect->unknown58, 0, 0, NONE);
			return;
		}
	}
}

// @retail 0x17b2d0
bool function_17b2d0(long effect_index, point3f *point, vector3f *forward, real *scale)
{
	bool result = false;
	s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, effect_index);

	if (effect)
	{
		s_effect_definition *definition = TAG_GET(s_effect_definition, effect->tag_index);
		long location_index = effect->location_indices[definition->looping_sound_location];
		s_effect_location_datum *location = effect_location_next(effect, &location_index, 3);

		if (location)
		{
			transform4x3f node_matrix;
			transform4x3f world_matrix;
			transform4x3f *matrix;

			if (location->node_index != NONE)
			{
				function_17aec0(location->node_index, effect, &node_matrix);
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
			result = true;
		}
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

						function_17aec0(location->node_index, effect, &node_matrix);
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
		query.color_a = 0xff404040;
		query.color_b = 0xff404040;
		query.unknown08 = -1.0f;
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


struct s_effect_preview_screen_176cb0
{
    byte unknown00[4];
    s_effect_preview_screen_176cb0 *parent;
    byte unknown08[8];
    s_effect_preview_screen_176cb0 *child;
    byte unknown14[0x70 - 0x14];
    long screen_id;
};
s_effect_preview_screen_176cb0 *g_54d760;
void __stdcall function_174420(long system_index, bool view_pass, bool camera_relative, bool force_visible);

PRIVATE __forceinline long data_datum_index_176cb0(s_record_pool *array, long index)
{
	long datum = NONE;
	if (index != NONE)
		datum = (((short *)(array->data + array->size * index))[0] << 16) | index;
	return datum;
}

// @retail 0x176cb0
void function_176cb0(void)
{
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 3 && g_54d760)
	{
		s_effect_preview_screen_176cb0 *screen = g_54d760->child;
		for (;;)
		{
			if (!screen)
			{
				if (!g_54d760->parent)
					screen = g_54d760;
				break;
			}
			if (!screen->child)
				break;
			screen = screen->child;
		}
		if (screen && (screen->screen_id == 6 || screen->screen_id == 9))
		{
			long index = data_datum_index_176cb0(g_4ea93c, function_16bc00(g_4ea93c, 0));
			while (index != NONE)
			{
				s_effect_datum *effect = DATUM(g_4ea93c, s_effect_datum, index);
				/* the second DATUM lookup is deliberate: it makes the compiler address the effect once, as retail does */
				long particle_index = DATUM(g_4ea93c, s_effect_datum, index)->first_particle_system_index;
				if (TEST_FIELD_BIT(effect->flag0) && !TEST_FIELD_BIT(effect->flag2) && !TEST_FIELD_BIT(effect->flag3) && !TEST_FIELD_BIT(effect->flag6))
				{
					function_179880(effect, index);
					function_179e80(effect);
				}
				while (particle_index != NONE)
				{
					s_particle_system_datum *system = DATUM(g_510c74, s_particle_system_datum, particle_index);
					function_174420(particle_index, false, false, true);
					particle_index = system->next_index;
				}
				index = data_datum_index_176cb0(g_4ea93c, function_16bc00(g_4ea93c, index == NONE ? 0 : (index & 0xffff) + 1));
			}
		}
	}
}


s_cluster_mask_storage g_cluster_mask_storage;
