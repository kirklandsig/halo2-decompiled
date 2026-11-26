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
/* a location's node index without its high flag bit; NONE stays NONE */
#define EFFECT_NODE_INDEX(node_index) ((node_index) == NONE ? NONE : (node_index) & 0x7fff)

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

void random_vector_in_cone(dword *seed, vector3f const *forward, vector3f *result, real min_angle, real max_angle);
long function_baf40(long object_index);
long function_1469f0(real seconds);
bool function_172750(long mode, point3f const *point, real radius);
real function_259a0(dword *seed);
dword vector3d_compress(vector3f const *vector);
long function_189060(long object_index, short value, point3f const *position, vector3f const *direction, real scale, long tag_index);
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
long function_c0350(long tag_index, long object_index, long node_index, point3f const *position, vector3f const *forward, vector3f const *up, real scale);
bool __stdcall function_16a8e0(long name, point3f const *point, real radius, long object_index, long unknown, point3f *origin, real *radius_reference);
void __stdcall function_179880(s_effect_datum *effect, long effect_index);
void __stdcall function_179e80(s_effect_datum *effect);
long __stdcall function_177040(long particle_system_index, long effect_index);
void __stdcall function_174990(real dt);
void function_156b60(s_effect_beam *beam, real progress, transform4x3f const *matrix);
void function_248c60(s_particle_location_datum *particle_location, s_particle_system_datum *particle_system, transform4x3f const *matrix, bool field_b4);
void function_17e670(point3f const *point, s_effect_source *source, long tag_index, vector3f const *vector, real radius, long unknown0, long unknown1, long unknown2);
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

// @retail 0x17aec0
void function_17aec0(short node_index, s_effect_datum *effect, transform4x3f *matrix)
{
    if (node_index != NONE && (node_index & 0x8000) && effect->unknown58 != NONE)
    {
        function_1664da(effect->unknown58, effect->object_index, EFFECT_NODE_INDEX(node_index), matrix);
    }
    else
    {
        short index = EFFECT_NODE_INDEX(node_index);
        s_effect_object *object = OBJECT_GET(effect->object_index);
        transform4x3f const *nodes = (transform4x3f const *)((byte *)object + object->nodes_offset);

        *matrix = nodes[index];
    }
}

// @retail 0x17af30
void function_17af30(bool field_b4, short node_index, s_effect_datum *effect, transform4x3f *matrix)
{
	if (field_b4 && node_index != NONE && (node_index & 0x8000) && effect->unknown58 != NONE)
		*matrix = *function_1664a5(effect->unknown58, effect->object_index, EFFECT_NODE_INDEX(node_index));
	else
		function_17aec0(node_index, effect, matrix);
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
long function_176780(long tag_index, s_effect_owner const *owner, long object_index, real scale_a, real scale_b, point3f const *origin, vector3f const *direction)
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
void function_176ad0(s_effect_owner const *owner, vector3f const *velocity, long marker_count, s_effect_marker *markers, long tag_index, long unknown30, real scale_a, real scale_b, point3f const *origin, vector3f const *direction, long mode)
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

// @retail 0x1786f0
void function_1786f0(s_effect_object_marker *out, s_effect_marker_source const *source, short marker_index)
{
	point3f position;
	vector3f forward;
	vector3f up;

	out->node_index = source->node_index;
	transform4x3f const *matrix = source->node_matrix;
	if (matrix)
	{
		s_effect_marker const *markers = source->markers;
		s_effect_marker const *marker = &markers[marker_index];

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
		real i = markers[marker_index].forward.i;
		real j = markers[marker_index].forward.j;
		real k = markers[marker_index].forward.k;

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

// MiniMax G (11-20): no new form, no lower score (128): the frame is still 0x40 against retail's 0x44 with no home for
// colour A, the particle system still lands in EDI where retail keeps it in ESI, and the colours are still read after the
// type lookup where retail reads them before it and spills colour A to a frame slot; the permuter (140 tries) and the
// colour-local forms measured above all leave the system in EDI.
// MiniMax H (11-19, round 95): no new form, no lower score (128): retail keeps the particle system in ESI and colour B in
// EDI across the two indirect calls and spills colour A to a stack slot, while this build keeps the particle system in EDI
// and re-reads colour A at the colour call; measured and reverted: the two virtual calls in the other order (148), the
// definition's location mode and placement read into locals before the loop so the definition is not live across the
// set_location call (143, frame 0x48), so the body stands.
// MiniMax A (11-20): no new form, no lower score 128: retail keeps the particle system in ESI and colour B in EDI, while every form here colours them the other way round.
// score 128 (was 128): The frame is still 0x40 versus retail 0x44, and the early color-A spill and ESI system are missing.
// MiniMax B (11-19): no new form, no lower score (128): retail spills colour A to frame+0x00 and keeps the particle
// system in ESI and colour B in EDI across the two indirect calls, while this build re-reads colour A from the effect
// and keeps the particle system in EDI; measured and reverted: colour A in a named local assigned after the
// set_location call gives retail's frame (0x44) and retail's 552 bytes with colour A at frame+0x00, but the particle
// system stays in EDI and the score is 130; the same with colour B a local as well spills colour B (all four
// callee-saved registers are taken) and measures 186; the location index in a local of its own measures 135; the two
// virtual calls written as the arguments of the colour call give retail's 552 bytes but measure 146; a permuter run
// over the body from the colour A form was stopped at its time limit with no result. The sibling 0x177040's own shape
// tried here too, and all of it measures worse: colour A and colour B read early as locals with the multiplied query
// written as the colour call's argument 178 (572 bytes); colour A alone with that argument 146; colour B alone read
// early as a local 187 (572 bytes); each of them takes a stack home for a colour and leaves the particle system in
// EDI, where retail keeps it in ESI and takes colour B into EDI, so the frame grows past retail's 0x44. A permuter run
// over the committed body (540 s, 67 tries) found nothing below the baseline - its best was the baseline.
// MiniMax A (11-18): no new form, no lower score (128): the frame is 0x40 against retail's 0x44 and the first colour lacks its retail spill (colour A in a named local measures 188 at 572 bytes, and the multiplied argument passed straight from the query measures 135), so the body stands.
// MiniMax I (11-15): no new form kept: reading colour B into a local ahead of the spawn stores measures 187 (was 128), so the body stands.
// MiniMax G (11-18): no new form, no lower score (128): the frame is still 0x40 against retail's 0x44 with colour A packed
// into the dead argument home, the particle system is still kept in EDI where retail uses ESI, and the zone block still
// takes its zone byte from the cluster field.
// frame 0x40 versus 0x44 (colour A packed into the dead `unknown` home), the system kept in EDI
// through the type queries where retail reloads it from its home for the colour call (so colour B misses EDI), and the
// zone block's scheduling.
// MiniMax E (11-14): no new form, no lower score (128): reading the location index into a local of its own makes the
// head match retail's bytes but measures 124, a gain of four on a function still above 30, so it is not kept; reading
// the zone byte from the cluster field instead of a copy makes the zone block match retail's bytes but measures 161,
// since the definition reload and the alignment pad leave two extra instructions before the switch dispatch; the
// particle system still takes EDI where retail keeps it in ESI, and the frame is still 0x40 against retail's 0x44.
// MiniMax H (11-14): no new form, no lower score (128): the frame is still 0x40 against retail's 0x44 and the
// spawn record still sits two frame slots lower than retail's, and the particle system still takes EDI where retail
// keeps it in ESI; the header's zone helper, whose zone block then matches retail's byte for byte, measures 165 on
// its own and 161-166 combined with the 0x44 frame, so the kept body stands.
// Earlier at 128 (was 128): Frame 0x40 versus 0x44, early color spill, cursor/query homes and zone-base scheduling remain.
// MiniMax S (11-11): no new form, no lower score (128); retail spills colour A into a frame home at +0x10 right after set_location, and those four bytes are what make its frame 0x44 against our 0x40 and shift every stack reference; the tint also still reaches the colour call on the stack, because 0x175a80's register contract (ECX colour A, EDI colour B, AL tint) lives in another lane's file and cannot be changed from here.
// MiniMax E (11-11): no new form, no lower score (128); reading both colours into locals before the type-query
// block with the two type queries inline as the call's arguments measures 181 against the kept 128, so the
// original body was restored; the frame is still 0x40 against retail's 0x44 and the system is still in EDI where
// retail keeps it in ESI.
// MiniMax A (11-06): no new form, no lower score; retail's head takes the definition (its third stack
// argument) into EAX and the location index from there, keeps the effect in EBP and the particle system in
// ESI, reads both colours before the 0x1751d0 type query (colour A in a frame home, colour B in EDI, the
// tint in BL) and pushes the multiplied value straight from EAX; ours keeps the particle system in EDI
// across the query, reads both colours at the 0x175a80 call site and spills the multiplied value to a
// byte home before the push, so every later jump is shifted; the frame is 0x40 against retail's 0x44.
// Retail's argument order is the one we already use (the scale in xmm0, then the effect, the particle
// system and the definition).
// MiniMax A (11-06): no new form, no lower score; retail keeps the first colour in a frame home at frame+0 and
// the second in EDI, so both colours are read before the type query; reading them into two locals after the
// location setup gives 184 and, with the two declaration orders exchanged, 185 (the frame grows to 0x48 and the
// location index leaves the argument home); a permuter run on this function finished with no candidate (5 tries
// in 315 s, best the baseline, no confirming build).
// MiniMax E (11-06): no new form, no lower score; the colour query and zone-base scheduling still keep
// the frame at 0x40 and both colours spilled.
// MiniMax E (11-09): no kept form, no lower score; naming the definition's location index in a local reaches 124 (a four-point gain, under the eight-point gate, so the body was restored) and the entry block still differs: retail keeps the particle system in ESI and reads both colours before the type query, ours promotes the system to EDI and re-reads the colours at the call; the permuter (three tries, best the baseline) and the colour-placement, store-order, scope, width and zone-helper variants all score 124 or worse.
// MiniMax R (11-04): no new form, no lower score; retail's early colour read still needs a form that
// spills only color A and keeps color B in EDI, which every snapshot point tried so far fails.
// MiniMax R (10-31): no kept form, no lower score: colour locals reach the frame 0x44 but spill both colours
// (195/188); the permuter's 124 promotes the unknown to xmm0 and breaks retail's argument layout.
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
// DH round 273: complete transform values in existing/distinct output, crossed with native node word
// and iteration scope: 161/197/161/197/161/197/161/197. Original pointer-result ownership, node
// observations, workspaces and body restored; early color home and frame remain.
// Round 501: complete placement case orders 0132/0213/0231/0312/0321/1023/1032/1230
// 128/138/138/128/138/128/128/128. Original case order, body and zone producer
// restored; early color/query ownership and frame 0x40 versus 0x44 remain.
// Round 117: retail reads the system from its stack home again before the colour call and into EBX before 0x175270, and
// reads the definition from its home before the loop and at its bottom, holding it in EDI across the next-location call.
// Non-volatile: early colour locals with multiplied() as the argument 178; a plain local copy of the system 178, without the
// colour locals 135; with a named multiplied 184; the system read through a pointer to the parameter 178. Diagnostics
// only, volatile reads of the own parameters, reverted: system at the colour call 137 (EDI/ESI, frame, length as retail); plus the
// zone field read at each use 159 (aligned 26); plus definition at its first read 43 (552/552, aligned 19), that first read
// through a named local 42 (aligned 17). Left there: colour A in EBX with the tint spilled, retail the reverse, which also
// moves the location index out of the parameter home. Colour A loaded at the function top flips it 32 (but above set_location
// where retail loads it after); colour order, long/const colour A, byte/char/long tint, declarations apart from the reads,
// reads around the spawn stores, block scoping, a colour helper 37 and named tag/settings temps do not (42-46).
// Round 311: Negative native tint/multiplication outcomes, bool subsets 1/2/3, then both
// in byte/signed-char/short/word/long storage: 188/190/190/190/190/193/193/193. Original positive
// query carriers, color timing, zone producer and full body restored; frame and early color homes remain.
// Round 519: Spawn outputs deferred past type/color calls, scale/unknown/location
// masks 1..7: 127/123/189/187/189/189/188; complete late aggregate 188.
// The five-count unknown-only lead fails the eight-point gate; scale-containing
// forms lose callers. Original publications and body restored; early color/frame homes remain.
// Round 288: Native placement rejection in bool/byte/signed-char/short/word/long
// 139/139/139/168/168/164; true/false default rejection 139/139. Original positive
// placement result, color observations, query carriers and full body restored.
// AC474: Complete positive/failure zone-admission results in bool/byte/word/long: 379 each
// (outlined); forced positive-result expansions 238/238/284/271. Original zone producer,
// queries and full body restored; early color/query/cursor homes and frame 0x40/0x44 remain.
// Round 476: direct per-case placement rejection, case orders 0123/0213/1023/2013,
// named/direct final multiplication query: 164/156/164/156/184/172/184/172.
// Original placement-result lifetime, type-query block and complete body restored.
// Round 478: complete native zone-entry/cluster/both/const-both values with
// cached/live zone selectors: 379/375/375/375 in both sets. Every producer
// outlines; original complete zone helper, observations and body restored.
// AC494: Native node value after the unchanged returned-transform producer,
// short/word/long/dword with direct/named positive argument: 128/131/133/133/135/137/139/139.
// Named positive arguments recover frame 0x44 but grow to 564/552 bytes. On that
// new frame, direct query and early A/B/both color combinations: 142/195/195/192/153/153/186/194.
// NOT kept: no eligible gain; unsupported widths and one-use color snapshots need a match.
// Original complete body, node observations, query timing and helper calls restored.
// M328: Post-transform node rejection by OR/conditional/branch/default-result,
// bool then byte: 147/159/159/151/147/159/159/151. All regress; complete original
// body restored. Early color/query homes and frame 0x40/0x44 remain.
// AC498: Isolated query-receiver/tint/multiply/placement/transform-result and
// effect/system/settings formal names: 128 each. Original complete body restored;
// frame 0x40/0x44, early color/query/cursor and zone-base homes remain.
// Round 58 (m3 lane 11, fast): spawn stores as location/scale/unknown 128, as unknown/scale/location 124 (gain
// 4, not kept); spawn declared first 128; location pointer declared in the while condition 128. On the
// unknown-first form: cursor and colour locals read before the spawn stores 180 (572 bytes); multiplied() as
// the call's argument 132; spawn declared first 124; effect cursor read after set_location 132; matrix at
// function scope 124.
// Round 123 (m3 lane 7, fast with callers/callees): spawn as an aggregate `= { scale, unknown, location }` after
// set_location 128 (same code); with colour locals after/before it 184/184 (572 bytes); the type-query block's
// braces removed 128 (same code); the mode cached in a word local before the loop 179 (564 bytes).
// DH315: Live mode/placement field references, readonly/mutable single/both,
// entry and iteration scopes: 128 in all eight forms, 556/552 bytes. Both
// matched callers and transform producer stay exact; original descriptor reads,
// query/color timing, cases, spawn fields and complete body restored.
// Round 67 (m3 lane 1, fast): spawn as an aggregate initialiser `{ scale, unknown, location }` 128; the type-query
// pointer at function scope 128; multiplied declared before tinted 128; the matrix producer as the spawn call's
// argument 162 (552 bytes); the location mode in a short/word local before the loop 143/143 (552 bytes).
// AC506: Initial location-setter presence/absence results in bool/byte/short/long:
// 194/194/188/188/194/194/188/188 (572/552 bytes), all matched controls preserved.
// NOT kept: one-use admission results regress and retain frame 0x40 versus 0x44;
// original initial guard, color/query observations and complete body restored.
// N354: Both external color-field identities, readonly/mutable references/pointers,
// at the spawn/query boundary and before location propagation: 128 each (eight forms).
// NOT kept: one-use field identities require equality; original complete body restored.
// Missing early color slot and frame 0x40 versus 0x44 remain.
// AC528: First-traversal cursor acquisition with late spawn-field masks 0..7
// 167/166/166/160/163/164/164/165. NOT kept: no exact match or eligible gain; original complete body restored.
// Early color/query/cursor homes and frame remain. These changed observation/residence forms require equality; no retained candidate.
// DH372: Partial clipped-zone halfspace acceptance/rejection, bool/byte/short/long,
// 131 each at 556 bytes. NOT kept: regressions; original helper and body restored.
// Missing early color home and frame 0x40/0x44 remain; all checked controls survive.
// AC531: Actual plane-field pointer/reference at entry lookup or after index admission,
// ordinary/forced existing zone expansion: eight at 129/556 bytes. NOT kept: no frame
// or color-home recovery; original producer, all live reads and complete body restored.
// AC535: Positive tint/multiplication query results with false/true-default publication,
// single queries then four pairs: 190/190/189/189/189/189/189/189 at 572 bytes. NOT kept:
// all regress/lengthen; original complete body restored, early color/query/cursor homes remain.
// Y599: Complete early color/direct-final-query graph with actual cursor, matrix and
// spawn lexical-home priorities: 178/178/178/178/178/178/178/178. NOT kept: no exact
// color/query home recovery; original complete body and observations restored.
// Frame 0x44 recovers but all eight grow to 572/552 bytes; input-home reuse remains.
// DH372: Partial clipped-zone halfspace acceptance/rejection, bool/byte/short/long,
// 131 each at 556 bytes. NOT kept: regressions; original helper and body restored.
// Missing early color home and frame 0x40/0x44 remain; all checked controls survive.
// Round 92 (m3 lane 16, fast with 0x177040 and 0x176f60): colour locals after the spawn stores with `multiplied()` as the
// argument 178 (572 B; frame 0x44 and the colour loads at retail's spot, but in the one block before 0x175a80 the
// system keeps EDI, colour A takes EBX and colour B and the tint spill, where retail keeps colour B in EDI, the tint
// in BL and reloads the system); the same locals inside the type-query block 178; colours at the call with
// `multiplied()` as the argument 135 (aligned 76 -> 74); `dword const` colour locals with both queries named 184;
// colour B declared first 178; both colour locals at the function top 195; only colour B read early (colour A at
// the call) 146 (556 B, aligned 76 -> 66: colour B takes EBX, the system keeps EDI, the tint spills). The loop already
// matches retail apart from frame offsets and the definition reload.
// Round 155 (m3 lane 5, fast with 0x178eb0/0x179020): the pointer parameters `*const` 128 (identical); colour A alone read
// after the spawn stores 187 (572 B); colour B alone early with `multiplied()` as the argument 146; colour A alone early with
// `multiplied()` as the argument 146 (the system keeps EDI in all three); the zone test through the existing
// location_point_in_zone_inline 165 (556 B; the zone block is then retail's: base in EDI, add unfolded; left are the entry's
// colour/system registers, frame 0x40/0x44 and the definition not held in EDI across the back edge); that with both colours
// early and `multiplied()` as the argument 181 (572 B, frame 0x44, the loop laid out with a jump to its test).
// Round 80 (m3 lane 1; one-file LTCG harness with the link ballast, exact here: 128; aligned counts by difflib with
// addresses and frame offsets masked, 68 for this body): all 119 other parameter orders of 0x175a80 128-184, none
// lower; a random search of 120 combinations of colour read point (call/top/before or after the spawn stores/query
// block), multiplied and tint named or in the argument list, the six spawn store orders, zone helper and declaration
// order: best 125 (unknown, scale, location with the spawn declared first; gain 3); the existing
// location_point_in_zone_inline in place of the file-local copy 165 but aligned 35 (the whole loop then matches retail
// but the definition reload at the loop top; left are the entry's system register, the early colour reads and the
// frame), with spawn stores unknown, location, scale 162; on that form the system or the definition read once through
// its own address 165/165; colours read early with both queries in the call's argument list 183 (568 B), at the top
// 192.
// Round 160 (m3 lane 5, fast with 0x177040/0x178eb0/0x179020/0x176f60/0x178bc0): early colours re-measured 178 (572 B); on them the
// system passed through a volatile read of its parameter (diagnostic, not kept) 137 (556 B: the system in ESI reloaded from its slot
// for the call and colour B in EDI as retail; colour A takes EBX, so the tint spills, and the location index loses its slot home),
// with colour B read first 138, with the tint declared above the colours 137; a forceinline colour helper shared with 0x177040 178
// (0x177040 93; same code as written out); the system through one plain read of its address 178 (folds away).
// Round 97 (m3 lane 9, fast with 0x177040/0x176f60/0x178eb0/0x179020): retail's allocation before the loop gives colour B EDI and the
// tint BL (their registers at the 0x175a80 call), the type object ESI, and leaves the system and colour A in memory; ours ranks the
// system (EDI) and colour A (EBX) first. A begin-spawn inline helper shared with 0x177040, taking the spawn record (it stores the
// particle-location index, runs the type queries and the colour call): parameters (system, effect, spawn) with early colour locals and
// `multiplied()` as the argument 179 (572 bytes, aligned 88), with a named multiplied 234 (488 bytes: the helper stays out of line;
// 0x178eb0 13, 0x179020 11); colours read at the call 135 / 235 (out of line); parameters (effect, system, spawn) 179; (spawn,
// system, effect) 179. Body unchanged.
// MiniMax C (11-09): earlier at 128 (was 128): the early colour home is still missing and the frame is
// 0x40 against retail's 0x44. Untouched this round.
// Round 161 (m3 lane 6, fast with 0x178eb0/0x179020/0x178bc0, all matched): matched 0x174a30's shape (colour locals read
// before the spawn record, a named definition from 0x1751d0, both queries in the call's argument list) 187 (568 bytes);
// the spawn record declared at its stores 128; the 0x1751d0 definition named in the type block 128; the query results
// `bool const` 128 (the last three the same code); the colour locals declared first and read after the spawn stores,
// `multiplied()` as the argument 178 (572 bytes, as the earlier early-colour forms; the first and last re-measure listed
// forms). New on top: the zone helper reading g_4e0348 at each use instead of a bsp local 128 (same code); the node flag as
// `node_index != NONE && node_index < 0` 133. Untested lead: 0x175a80's body (its file held by another lane this round),
// though the call's argument registers already agree with retail's.
// Round 165 (m3 lane 6, fast with 0x178eb0/0x179020/0x178bc0): colour locals read after set_location ahead of the spawn
// stores with the type-query block as `do { } while (false)` 184 (572 bytes; the same code as the colour locals without
// it, re-measured 184 here, listed at 187: the region folds away); the type-query block alone as `do { } while (false)` 128 (same code); colour locals with the type object declared
// at function scope before them 184; on that, the tint and colour A renamed to sort the other way (diagnostic) 184 (same
// code: names don't order the registers); the location index stored first, then the colour locals, then scale and unknown
// 184 (masked 29); a `dword const *colors = &effect->color_a` pointer after set_location with colors[0] and colors[1]
// in the call 128 (the loads still sink to the call). In every early-colour form colour A takes EBX and the system EDI,
// so colour B and the tint spill.
// Round 76 (m3 lane 14, fast with 0x178eb0/0x179020/0x177040/0x176f60/0x178bc0): retail's type-query block holds the
// tint in EBX, colour B in EDI, the type object in ESI and the effect in EBP, so the system and colour A have no
// register there (colour A gets the frame slot, the system is read from its parameter slot for the colour call), and
// the definition takes EDI through the loop where ours keeps the system there. Early colour locals with the spawn
// stores as unknown, scale, location 184 (572 bytes); colours read between the scale/unknown stores and the location
// store 184; the system's location index named and read with both colours before the spawn stores 180; colours read
// at the top of the type-query block with `multiplied()` in the call 178; the first form with colour B declared and read
// first 185 (the first, third, fourth and fifth re-measure round 58/92/160 forms; the second is new). 0x175a80's
// body (its file held this round) stays the untested lead; no untried natural colour placement is left.
// Round 105 (m3 lane 16, read against the fast listing): the frame and length gaps are the same colour-call cause.
// Retail's extra frame dword is colour A spilled before the type query; its other 4 bytes are the multiplied value
// pushed straight from EAX (ours spills it to a byte home) and the definition kept in EDI from the loop's bottom reload
// to the placement switch (ours reloads it into a scratch register at each use, with a `mov edi, edi` pad). Both
// follow the system/colour/tint ranks set by 0x175a80's register contract (src/unknown_173b90.cpp, Avoid this round).
// Round 184 (m3 lane 4, fast; aligned/masked as difflib counts with addresses / also registers masked, 76/32 at 128):
// re-measured: the existing location_point_in_zone_inline (location_zones.h, NULL zone index) for the private zone
// copy 165 (aligned 45, masked 28), with the spawn stores unknown, scale, location 162 (aligned 41, masked 19). New on
// that: both colours read into locals before the type-query block 187 (572 bytes), inside it 187; `multiplied()` as
// the call's argument 170; both queries as the call's arguments 164 at 552/552 bytes (aligned 36, masked 15: the loop
// then holds the definition in EDI and reloads the system as retail; LEAD), on it both colours early 188 (568 bytes),
// colour B only early 153 (aligned 35), colour A only early 153 (masked 26). Left on the lead: the system in EDI at the
// entry (retail ESI, reloaded for the colour call), the early colour reads and frame 0x40/0x44.
// Round 93 (m3 lane 10): no new form. Retail reads both colours right after set_location (colour A to +0x10, colour B
// in EDI), keeps the system in ESI up to the colour call and the definition in EDI through the loop; those registers are
// set by the 0x175a80 call, whose file is in another lane's set this round (the lead this note already names).
// M3 lane 3 r236 (fast with 0x177040/0x176f60/0x178eb0/0x179020/0x178bc0/0x1794e0, all matched or unchanged): 0x175a80 is
// not the lever these notes name: its register contract (colour A ECX, colour B EDI, system ESI, tint AL, one push) is retail's
// and it saves no callee-saved register in either build. The 0x177040 order that spills colour A as retail (cursor stored
// first, colour B then colour A) applied here: location stored first, colour B, colour A, then scale and unknown 184 (572 B);
// location, scale, unknown, then colours B/A read inside the type-query block 181 (572 B, frame 0x44, but the location index
// takes the new frame slot and colour A the dead `unknown` home, retail the reverse; the tint spills); the first with both
// queries in the call's arguments 187 (568 B). Round 117's volatile-parameter diagnostics stand as the cause: retail splits
// the system's live range (ESI to the 0x1751d0 call, then its home), which no non-volatile form here reproduces.
// MiniMax A (11-12): no new form, no lower score (128); the gap starts in the prologue (frame 0x40 against retail's 0x44) and in the order of the 0x175180/0x1751d0/0x137bd0 calls, so it needs a body rewrite rather than a local spelling.
// M3 lane 3 r238 (fast with 0x177040/0x176f60/0x178bc0/0x178eb0/0x179020): the node flag for 0x175270 in a named bool 135 at
// 564 B (frame 0x44 as retail, register-masked aligned 41 -> 31, but the bool takes the effect argument's home and the
// location index the frame slot, retail the reverse); round 184's lead re-measured 166 fast (552 B); the lead with that
// named bool 172 (560 B, masked 21). Body unchanged at 128.
// M3 lane 11 r97 (fast with 0x178eb0/0x179020, matched throughout): re-measured the lead (existing
// location_point_in_zone_inline, both queries in the colour call's arguments) 166 at 552 bytes; with both colours read
// into locals after the spawn stores 184 at 568 bytes. On that form frame 0x44, the location cursor in the effect
// argument's home, colour A's slot at +0x10, the tint in BL and the type object in ESI are all retail's; the one
// difference left ahead of the loop is the system kept in EDI through the queries (retail drops it when the type object
// takes ESI and reloads it from its argument home for the colour call), which leaves colour B without EDI. On it: colour
// B read first 184; the colours ahead of the spawn stores 190; colour B first and ahead 190; the location store first,
// then the colours, then scale and unknown 190; the system read once through its own address for the colour call 184,
// for the colour call and the loop call 184 (both fold away); a named tint with `multiplied()` as the argument 181 (572
// bytes). Diagnostics: no system argument in the loop's 0x175270 call 65 at 552 bytes (colour B takes EDI, the system
// moves to EBX, the tint spills); no set_location call 178 (540 bytes); the type looked up from the definition argument
// 153 (552 bytes, system still EDI). Locating diagnostic only (a forced re-read of the system parameter at the colour
// call on the early-colour lead; forbidden form, reverted at once): 40 at 552 bytes, the same with the loop call re-read
// 40; the system then sits in ESI and is reloaded, colour B takes EDI. Left on it: colour A spills to the dead `unknown`
// argument home instead of a new frame slot (frame 0x40 against 0x44, which shifts every stack offset), one scratch
// register step from the first statement on (the definition's location index lands in ECX, retail reuses EAX), the
// third zone product's operand order, and the order of the two loads ahead of the colour call.
// M3 lane 9 r120 (fast): both type queries in the colour call's arguments 146 (552 B: definition in EDI for the loop
// as retail, the loop entered through a jump to an aligned head); multiplied() alone in the call 135; colour locals in
// the type-query block 178 (572 B); the spawn's location index stored first 128; colour locals after the spawn stores
// 184 (572 B).
// P479: Standard location/iterator/node-sentinel/node-sign predicates 193/221/173/172;
// node conjunction/complementary disjunction, zone complement and paired negative
// query producers 173/183/171/204. NOT kept: all regress/lengthen; first form loses both
// exact callers. Original full body restored; early color/frame/query homes remain.
// Round 84 (m3 lane 2, fast with 0x178eb0/0x179020, matched unless given): `&effect->location` held in an `s_location *`
// local for the cluster test, set_location and both zone tests 142 at retail's 552 bytes (the definition then stays in
// EDI from the loop's mode read to the placement switch, as retail; entry registers and frame unchanged); on it both
// colours read into locals after set_location ahead of the spawn stores 178 (568 B), after the spawn stores 178 (568 B).
// On the current body: the spawn stores as unknown, scale, location 124 (0x178eb0 4, 0x179020 6); the type-query
// block's braces removed 128 (same code); both colours read into locals as the first statement 198 (572 B).
// M3 lane 5 r183 (fast with 0x178eb0/0x179020, both matched throughout): lane 11 r97's early-colour lead (colours read
// after the spawn stores, both queries in the colour call, location_point_in_zone_inline) re-measured 184 (568 B); on it:
// the colour block through a forceinline helper taking the system by const reference 184, through a forceinline helper
// taking (colour A, colour B, system) 184, that helper fed `effect->color_a`/`effect->color_b` as arguments 184 (all the
// lead's code: the helper parameter is coalesced); colours read between the scale and unknown stores 190 (first
// difference +0x2); colours read before the location-index store 185; the system parameter renamed a_system or
// zz_system 184 each (diagnostic). Diagnostic only, reverted at once: the volatile re-read at the colour call
// re-measured 40 (552 B). On the kept body: the spawn record declared first 128; a named `s_location *effect_location`
// for the cluster test, set_location and both zone tests 142 (552 B).
// MiniMax G (11-14): no new form, no lower score (128): retail's frame is 0x44 with colour A spilled to frame+0x00 and
// the spawn record at frame+0x04, while ours keeps the spawn at frame+0x00 and colour A in a register; reading colour A
// into a local declared before the spawn (this round) gives 187 at 572 bytes, so the body stands.
// M3 lane 15 r89 (fast with 0x178eb0/0x179020/0x178bc0, matched throughout; control 128): the system's definition named and
// the type looked up from it (0x174a30's matched spelling) 128; the location matrix at function scope 128 (same code); 0x178bc0's
// result passed straight to 0x175270 162 (552 B; the node flag is then evaluated before the 0x178bc0 call, retail after); both
// queries as the colour call's arguments with one `s_location *` for the cluster test, set_location and both zone tests 146
// (552 B); 0x174a30's whole colour pattern (colours read before the spawn stores, definition named, queries as arguments) 187 (568 B).
// MiniMax S (11-15): no new form, no lower score (128): retail's frame is 0x44 against our 0x40, it keeps the particle system
// in ESI where ours promotes it to EDI, it spills colour A to a frame home of its own, and it reads the location mode from the
// definition again for each call.
// M3 lane 5 r191 (fast with 0x178eb0/0x179020/0x178bc0/0x177040/0x176f60, matched or unchanged throughout): the
// definition's location index named first (`long definition_location_index = definition->location_index;`) re-measured
// 124 (the head is then retail's, gain 4, not kept); on it: the existing location_point_in_zone_inline (NULL zone index)
// with both type queries in the colour call's argument list 162 at 552 bytes (that lead was listed at 164); that with
// both colours read into locals ahead of the spawn stores 184 (568 bytes, frame 0x48); the spawn record declared first
// 124 and the location cursor assigned after the declarations 124 (both same code); the spawn stores moved after the
// colour call 186 (572 bytes). Our loop already reloads the system from its argument home for 0x175270 as retail
// does; the gap stays in the entry block (system EDI against ESI, colour A's frame slot, frame 0x40/0x44).
// P589: Placement cases 1302/1320/3012/3021/3102/3120/3201/3210:
// 128/128/128/138/128/128/138/138. NOT kept: no native frame/color/query/zone recovery;
// original complete body and case order restored.
// MiniMax D (11-17): no new form, no lower score (128): retail's frame is 0x44 against our 0x40, it keeps the particle system in
// ESI where ours promotes it to EDI, it spills colour A to a frame home of its own, and it loads the location mode into EAX where
// ours uses ECX; the multiplied query inlined gives 135, colour A read early with it 146, and both queries inlined 146.
// Shared inline zone test on the outside arm 164.
// Shared inline zone test on the inside arm 165.
// Existing zone call on the outside arm 263.
// Existing zone call on the inside arm 252.
// Existing zone call on both conditional arms 375.
// Existing placement helper for the complete dispatch 375.
// Existing placement helper for the two conditional arms with the actual placement 367.
// Existing placement helper on the outside arm 263.
// NOT kept: the single-arm inline changes raised the score and the helper calls shortened the expanded zone region; the original body was restored.
// P594: Scalar memcpy at spawn scale/unknown/location subsets 1..7, then the input
// location cursor alone: 126/129/169/128/126/129/169/128. NOT kept: no exact native frame/color/query recovery;
// original spawn publication, parameters, helpers and complete body restored.
// Best preview 126 gains only 2; below the keep threshold.
// Round 563. The frame is 0x40 against 0x44 and the first colour lacks its retail spill. paired colour
// initializers before the spawn stores 184; paired colour initializers at the type query 184; entry colour
// declarations with paired assignments after the spawn stores 184; entry colour declarations with paired
// assignments before the spawn stores 184; paired colour assignments in the type query scope 184; paired
// colour initializers with a native tag index at lookup 184; paired location cursor and short cluster test
// inputs 140; paired colour initializers in a distinct post-location scope 184. NOT kept: paired early colour
// reads introduce a 0x48 frame and 20 extra bytes over retail.
// M3 lane 16 r133 (fast with 0x178eb0/0x179020/0x178bc0, matched throughout; control 128): the spawn stores as scale,
// location index, unknown 128 (same code); colours B then A read at the top of the colour block with `multiplied()` in the
// call 178 (572 B, frame 0x44: colour B in EBX, the system kept in EDI to the colour call, tinted spilled to the effect
// argument's home, colour A to the dead `unknown` home and the cursor to frame+0, where retail keeps tinted in BL, colour
// B in EDI, drops the system after the type query and reloads it from its home, the cursor in the effect home and colour
// A at frame+0); on it the colour locals renamed to sort after the cursor 178 (same code: names do not move these homes),
// tinted declared ahead of the colours 178 (same code), the system bound by reference for the colour call 178 (same code).
// Staggered color reads around settings/type lookup or type/tint query, both color orders and
// named/direct multiplied query: 189/186/190/186 and 189/186/190/186. NOT kept: the direct forms
// recover frame 0x44 but all grow to 572 bytes and score worse.
// M421: Body-advanced for/while 128/128, mode field reference 128, traversal settings reference 128.
// Early colors with while body/for body/for step 184/184/376, forced-inline zone predicate with for step 128.
// NOT kept: early colors lengthen the body. The forced inline restores both zone expansions at the opening score.
// The color A slot and system register still differ.
// M3 lane 8 r186 (fast with 0x178eb0/0x179020, both matched throughout; control 128): the matched sibling 0x174a30's
// colour pattern re-measured (both colours read into locals after set_location and ahead of the spawn stores, both type
// queries in the colour call's arguments) 187 at 568 bytes: tint BL, type object ESI and the loop's definition in EDI are
// then retail's and the only register off before the loop is the system held in EDI through the queries, which leaves
// colour B without EDI (frame 0x48, both colours spilled). New on it: with the definition's location index named and the
// existing location_point_in_zone_inline 184; the spawn stores, the particle-location index, the type queries and the
// colour call in one inline helper taking (system, spawn, scale, unknown, colour A, colour B) 184, with the helper's
// parameters as (colour A, colour B, scale, unknown, spawn, system) 184 (both the same code as the locals); set_location
// called after the colour reads 190. The type query through a two-caller inline helper with a named definition local
// (the form that takes 0x177040 to 54) leaves this body at 128 (same code); that helper was not kept at review (see
// 0x177040's note).
// M3 lane 15 r105 (fast; aligned/masked = difflib counts with addresses / also registers masked, 76/32 at 128): the early-colour
// lead (existing location_point_in_zone_inline, both queries in the colour call's arguments, colours read into locals after the
// spawn stores) re-measured 184 (568 B, aligned 38, masked 18); on it the 0x1751d0 result named 184 (same code), colour B
// declared `register` 184 (ignored), one to eight dead initialised locals 184 each (this allocation does not follow the
// symbol count). The colours read after set_location and ahead of the spawn stores 190 (568 B, aligned 52); on that the
// definition's location index named 184, a named tint with multiplied() as the argument 187 (572 B), the colours read through
// a `dword const *colors = &effect->color_a` pointer 190 (same code). Diagnostic only, reverted at once: the volatile system
// re-read at the colour call on the colours-ahead form 29 at 552 B (aligned 23, masked 9; 40 on the after-the-stores form), so
// retail reads both colours before `spawn.unknown = unknown` (colour A then gets its own frame slot instead of the dead
// `unknown` home, by the parameter-home rule). Left on that diagnostic: one scratch-register step from the first statement
// (the named location index form fixes the head), the [esi+0x30]/colour A load order, the third zone product's operand sides
// and the loop-bottom scratch rotation. Blocker unchanged: retail treats the system as a memory value cached in ESI until the
// type object takes ESI, and no plain form here stops ours from giving it EDI through the colour call.
// The first colour still needs its own spill and the system still needs ESI.
// Carry the initial spawn location across the type lookup, tint, multiply and
// colour update, assembling the spawn record after each boundary in turn.
// Colours at the update 188/186/188/188, early colours 210/210/208/208.
// NOT kept: the tint boundary supplies a 0x44 frame at 572 bytes, but it
// spills the traversal cursor and retains the system in EDI.
// M3 lane 3 r269 (fast with 0x178eb0/0x179020/0x178bc0/0x177040/0x176f60/0x175a80/0x174a30, matched or unchanged): the
// early-colour lead (colours after the spawn stores, both queries in the colour call, location_point_in_zone_inline)
// re-measured 184 at 568 B; on it 1..7 unreferenced locals, plain or initialised, 184 each (no tie); the location loop
// in a forceinline helper taking the system 184 (same code); the 0x1751d0 result named 184 (same code).
// M3 lane 5 r206 (fast with 0x178eb0, 0x179020 and the callees, all matched throughout; positional, then aligned and
// register-masked aligned counts, control 128 [85/40]): both colours read into locals after the spawn stores with both type
// queries as the colour call's arguments 181 [73/26] (frame 0x44 and colour A's spill at +0x10 as retail; system in EDI and
// colour B spilled to a dead argument home where retail keeps colour B in EDI and reloads the system from its home); the
// colours before the spawn stores 187 [89/44]; the 0x1751d0 result named or not, same code; the tint named 178 [88/35]; colour B
// read first, no block braces, colours inside the block 181 each. The zone helper reading `cluster->zone` at each use (retail's
// clusters base in EDI and the byte through the formed record) 165 [52/32] alone; with the colour form 184 [40/18] (lead: left
// are the system/colour B registers, the mode push's scratch registers and, from those extra bytes, a padded loop entry);
// on that: the tint named 181 [52/29], both queries named 187 [66/42], an inline type accessor on the definition 184 (same
// code), the whole colour block as a shared inline helper 184 (same code), the colours read between the spawn stores 185
// [41/22], the colours before the spawn stores 190 [57/36].
// M3 lane 2 r113 (fast with 0x177040/0x178eb0/0x179020/0x178bc0/0x175a80/0x176f60, matched or unchanged; aligned/masked
// in brackets, control 128 [76/32]): combinations of listed partial leads: spawn scale and unknown stored ahead of the
// cluster test (the location index still read after set_location) 130 [78/19]; that with both type queries as the colour
// call's arguments 148 (552 B) [75/15]; plus the existing location_point_in_zone_inline 168 (552 B) [47/14]; both queries
// in the arguments with that zone test and the node flag named for 0x175270 172 (560 B) [35/13]; the spawn stores first
// with that zone test 167 [51/19]. Aligned counts fall to 35 but the system/colour registers and frame 0x40 stay.
// Shared placement query as an acceptance arm or rejection guard, with direct or named
// placement. At-call colors 375/375/371/371. Early color pair 360/360/356/356.
// NOT kept: the placement call replaces retail's inline cases. The early pair reserves
// 0x48 instead of 0x44.
// M3 lane 5 r215 (fast with 0x178eb0/0x179020, matched unless given; control 128): the early-colour lead re-measured 190
// (568 B). Diagnostics, reverted at once (an unreachable call given the system parameter's address keeps the parameter
// memory-homed; not a keepable form): on the lead 163 at 552 B (frame 0x44, colour A's slot, colour B in EDI, the tint in
// BL and the type object in ESI are then retail's, but the system is also reloaded after set_location, where retail
// keeps ESI); that plus a by-value copy of the system for its first three uses (set_location, the location index, the
// 0x1751d0 call), the parameter itself at the colour call and in the loop, 35 at 552 B, and 31 with the definition's
// location index named (left: scratch-register rotation at the join, at the colour call and at the location-cursor
// call, and the third zone product's sides). So retail's entry is a memory-homed system parameter plus a register copy
// that dies at the 0x1751d0 call. Natural spellings of that, 190 each (folded before allocation): the plain copy; the
// copy read once through the parameter's own address, declared after or before the location index; an empty inline
// helper given the parameter's address; an empty static function given it; an inline getter taking a reference to the
// pointer; an inline getter taking its address; a dead pointer local under a false test; a reference local at the
// later uses; memcpy from the parameter. The system as a by-value one-pointer aggregate parameter 190 (scalar-replaced,
// same code); system and definition as one two-member aggregate parameter 190; with unknown and the flag in it too 190
// (0x178eb0 61, 0x179020 140). On the first diagnostic, six orders of the colour reads and spawn stores: scale,
// location, A, B, unknown / location, A, B, scale, unknown / location, scale, A, B, unknown 163 (register-masked
// aligned 13); A, B, scale, location, unknown 163 (masked 16); with unknown stored ahead of the colours 168 (masked 33
// and 32): retail reads both colours before the unknown store. The 31 form is build/r215/lead-0x178c80-31-diagnostic.txt.
// M3 lane 1 r138 (fast with 0x178eb0/0x179020/0x178bc0/0x177040/0x176f60; control 128): the type lookup and colour call
// moved into a function of another effects file (to test whether retail's short system copy is a link-time inlined
// helper's parameter) 215 (504 B: the link does not expand it, with or without `__forceinline`; 0x178eb0 13, 0x179020 11);
// the system parameter as a reference, the definition, declaration and both calls changed together, 128 (same code; both
// callers matched); that with both colours read into locals ahead of the spawn stores and both queries in the colour
// call 187 (568 B); with `multiplied()` as the colour call's argument 135; a `s_location const *` for the cluster test and
// set_location only 128 (same code). All restored.
// M3 lane 3 r278 (fast with 0x178eb0/0x179020, both matched throughout): both colours read into locals right after
// set_location, then the spawn record by an aggregate initialiser (scale, unknown, location index) 184 (572 B); the
// initialised record ahead of the colour reads 184 (572 B); that first form with both queries as the colour call's
// arguments and no inner block 187 (568 B); the system parameter declared `*const` with the colours read after the spawn
// stores and the queries as arguments 181 (568 B); the first form with `multiplied` queried ahead of `tinted` 182 (572 B).
// M3 lane 1 r142 (fast with 0x178eb0/0x179020/0x178bc0/0x177040/0x176f60/0x175a80/0x174a30, matched or unchanged unless
// given; aligned / register-masked aligned in brackets, control 128 [85/27]): the colour block as an inline taking the
// effect, with both colours in its own locals ahead of the type lookup, called after the spawn stores 178 at 572 B
// [88/31]; that inline called ahead of the spawn stores 190 [92/39] (0x178eb0 32, 0x179020 28); an inline taking the
// colours by value, called between the location-index store and the other two spawn stores 181 [90/35]; the type lookup
// through an inline that holds the 0x1751d0 result in a local, colours read early 178 [88/31]; the first inline reading
// colour B first 178 [88/29]. All five are the early-colour lead's code again (system in EDI through the queries, colour
// B spilled): an inline's locals for the loaded colours do not add the reference colour B is short of.
// M3 lane 13 r130 (fast with 0x178eb0/0x179020, both matched throughout; control 128; "lead" = the early-colour shape:
// the location index named, the scale and location-index stores, both colours read into locals, the unknown store, both
// type queries in the colour call, location_point_in_zone_inline; re-measured 185 at 568 B). Diagnostics, reverted at
// once, locate the cause as a ranking tie and not a second variable or a hidden block boundary: a null test with a
// return after the 0x1751d0 call 195, after the 0x137bd0 call 192 (a boundary alone leaves the system in EDI); one more
// unconditional use of colour B (a store of it) 172 with retail's whole entry and type block (the system cached in ESI
// and evicted when the type object takes ESI, reloaded from its home for the colour call; colour B in EDI; colour A at
// +0x10; frame 0x44); that plus one more system use 187 (back to EDI); a conditional extra colour B use 190 (no effect);
// the colour call given another pointer 133 at 552 B (entry retail's), the colour call and the loop call 146, the loop
// call only 169; the location index not read from the system 160 and 0x1751d0 called on another pointer 43 at 552 B
// (colour B in EDI, but the system is then loaded at each use); set_location on another pointer 189 (no effect). So
// ahead of the loop the system ranks exactly one counted reference above colour B (its three unconditional uses against
// colour B's read and use; conditional uses add nothing) and a tie goes to colour B. Natural forms on the lead, all 185
// and the same code unless given: the 0x1751d0 result, or the type object, through an inline returning a one-pointer
// aggregate; the first three system uses through a const reference to a named copy, or to the parameter; the later uses
// through a const reference to the parameter; the spawn record returned by value from an inline 204 (584 B); the
// colours declared first with a local system copy declared after them and used everywhere, or ahead of the loop only;
// the colours declared at the function top; the colours in a two-element array, copied as a two-member pair, copied by
// memcpy, both or colour B alone initialised at the declaration and then assigned; colour A as a member of one
// aggregate with the spawn record; colour B alone read early with colour A at the call 152 (552 B); the type lookup,
// the location setup with the first spawn stores, and the colour call each through an inline taking the system, alone
// and together. Next: a natural extra unconditional reference to colour B, or one fewer for the system that keeps its
// three early uses; retail's 0x177040 shows the same preference of colour B over colour A.
// Partial spawn initialization with early colours A/B or B/A. After the location
// setter, scale only 212/212, both scalars 209/209. Before the setter, scale only
// 216/216, both scalars 211/211. NOT kept: the forms retain overwritten zero
// stores, grow to 588 bytes and reserve 0x48. The colour-A spill and ESI system
// lifetime still differ.
// M3 lane 13 r133 (h2m fast with 0x177040, and with each changed callee's own address so its file rebuilds; control
// 128): `inline` on the definition of a callee, so callers keep no link-time summary of it: 0x178bc0, effect_location_next,
// 0x248c60, 0x175a80, 0x1751d0 or 0x175270 128 each (same code); set_location 188 (572 B: the scale can no longer stay
// in XMM0 across that call, which retail's does, so retail had its summary); 0x137bd0 233 (604 B). In the loop retail
// keeps the definition in EDI and reloads the system for each spawn call, this build the other way round. The system
// parameter renamed `system` 128 and the spawn record declared first 128 (same code each). All restored.
// M3 lane 11 r146 (h2m fast with 0x178eb0/0x179020/0x177040, matched or unchanged; control 128): colour A named right after
// set_location with both type queries as the colour call's arguments 130 (552 B, frame 0x44, colour A at +0x10 as retail; the
// system in EDI through the queries and colour B read at the call); that with colour B named after colour A 187 (568 B,
// colour B spilled); with colour B read at the call through a const reference to that named copy 187 (same code: the
// reference folds); the first form with the definition's location index named 131 (head exact), plus the existing
// location_point_in_zone_inline for both zone tests 151. The three pre-loop callees that could take a code-free extra
// colour B use (set_location, 0x1751d0, 0x175a80) live in another lane's file this round.
// M3 lane 8 r215 (h2m fast with 0x178eb0/0x179020, both matched throughout; control 128): the type lookup and colour
// call as a plain static helper in this file taking the effect and the system (not marked inline, so that a late
// link-time expansion could keep a parameter copy, as 0xb8bd0's expansion does in 0xbd090) 233 (488 B: it stays a call),
// with the colours read first inside it 233; colour B read into a local before the cluster test 188 (572 B); both
// colours read there 195 (572 B); colour B read after the spawn stores, ahead of the type lookup 187 (572 B).
// Color lookup tag reference, tag value, settings pointer plus tag and settings reference plus tag 137/128/128/128
// with colors read at publication. With early colors, 189/184/184/184 at 572 bytes and frame 0x48. The first color
// still misses its retail spill.
// M3 lane 11 r150 (h2m fast with 0x178eb0/0x179020, both matched throughout, and 0x177040 93; control 128; a/m = aligned /
// register-masked aligned unmatched lines, control 75/31): both type queries as the colour call's arguments with the
// colours read at the call (main's colour reads, no query locals) 146 at 552 B, a 73/29 (after the call the definition
// takes EDI and the multiplied result is pushed straight from EAX as retail, but the loop is entered by a jump into the
// dispatch); that with the 0x1751d0 definition named 146, a 70/29; with the spawn's location index stored first 146; with
// the existing location_point_in_zone_inline (NULL zone index) for both zone tests 166, a 44/27; the 0x1751d0 result's
// tag index named ahead of the 0x137bd0 call 128 (same code). The entry (system in EDI, colours read at the call) is
// unchanged in all. All restored.
// Round 627. Early color A with cursor, spawn or matrix renamed 187/187/187;
// spawn declaration at initialization 187, aggregate 187, aggregate with named
// or test-scoped cursor 187/187, scoped spawn with direct multiply query 146.
// NOT kept: color A takes a register while the cursor spills, the system stays
// in EDI, and all forms score above 128.
// Round 666. The 0x40 frame and late color reads remain.
// Immutable tag and flag values 128, confined virtual receiver 128. Capture color A
// or both before the spawn record 187/184, between tag read and type lookup 189/191,
// and between type lookup and virtual queries 187/189. Single captures recover 0x44
// but the system remains in EDI and the body grows to 572 bytes.
// M3 lane 3 r306 (H2M_FAST with 0x178eb0/0x179020, both matched throughout; control 128; "rank" = the register
// allocator's colouring priority from a back-end trace): on the early-colour form (both colours read into locals right
// after set_location, ahead of the spawn stores; 184 at 572 B) the system parameter itself ranks -64 (live through the
// loop) and is split; its piece over the entry block ranks 58 (three uses there: the location index, the 0x1751d0
// call and the colour call, each counted with the block's nine candidates) and takes EDI, while colour A, colour B and
// the tint rank 36 each (one read and one use in that block), so colour A then takes EBX and colour B and the tint
// spill. Retail's allocation (colour B EDI, tint BL, colour A in a frame slot, the system reloaded from its home for
// the colour call) needs that piece below the colours. Forced, as a diagnostic (not source): the piece at 18 gives
// frame 0x44 and the 552-byte length (178, first difference +0x7); left there are colour A's spill slot, the tint in a
// byte home instead of BL, the loop's definition not held in EDI and the zone block. New this round: on the
// early-colour form, `dword const` colours read B first 185 (572 B); the tint and multiplied results declared bare at
// the top and assigned after a type lookup with no block 184; the spawn's location index stored ahead of the colour
// reads 184; multiplied queried ahead of tinted 182 (572 B). Forced ranks of colour A/B and the system's first piece
// (22 single decisions) stay at 178 or above.
// BSP base passed at the zone calls or captured at successful advancement,
// with the location pointer or the cluster word, 128 each. The four forms
// with an early color A capture give 187 each. The frame reaches 0x44 there,
// with 572 bytes and the system still in EDI.
// M3 lane 13 r156 (H2M_FAST with 0x178eb0/0x179020, both matched throughout; control 128). Allocator reading on main: the
// type object (priority 70) takes ESI ahead of the system's entry piece (46, set_location through the colour call), so
// the system falls to EDI; the definition is never coloured (it is live across the whole loop body and referenced only
// by the mode and placement reads), where retail keeps it in EDI in the loop and reloads the system for each spawn call.
// Forms: `multiplied()` as the colour call's argument with the loop's matrix declared at the top of the loop body 135;
// the spawn record as an aggregate initialiser with `multiplied()` as the argument 135; `spawn_here` as a long 167 (560
// B); the location's node index named for the spawn call's flag 128 (same code); the 0x1751d0 result named ahead of the
// 0x137bd0 call 128 (same code). Body unchanged.
// M3 lane 6 r244 (h2m fast; a ranking trace): the particle system's whole web is not coloured in the first pass (live
// across the loop, -76), and its entry piece runs through the colour call, so it ranks 46 under the type object (70)
// and takes EDI; retail's entry piece ends at the 0x1751d0 call and the colour call reloads the system into ESI.
// Both colours as locals after the spawn setup 184 (572 B); colour A alone right after set_location 187 (572 B, the
// system piece 52 against the type object 80); multiplied() as the colour call's argument with tinted named (the
// 0x177040 spelling) 135; that with both colour locals 178; the spawn location index first and colour B a local
// before the spawn floats 185.
// The early system and color slots still disagree. Preset true or false placement
// result, positive or inverted, with assigned or branch-set zone decisions:
// positive 128/164 and inverted 139/164 for each preset. NOT kept: each retains
// frame 0x40, and the branch-set results lengthen the body to 564 bytes.
// Lane AO r3 (fast with 0x178eb0/0x179020 and the seven callees, all MATCH throughout; control 128): the zone helper's
// zone byte read straight from `bsp->clusters[cluster_index]` 128, the helper `__forceinline` 128, both 128 (same code
// each); the spawn guard as `if (!spawn_here) continue;` 128; the loop's matrix declared at function scope 128; the spawn
// record declared ahead of the cursor 128 (same code each); the zone entry read through `bsp->zones[zone & 0x7f]` at
// each use 379 (336 B: the helper is then called out of line); `&effect->location` named once for the set_location test,
// the call and both zone calls 142 at retail's 552 B (the system still in EDI). Retail's callers confirm the declared
// order (scale in xmm0, then effect, system, definition, unknown and the flag pushed last to first). Body unchanged.
// M3 lane 11 r169 (h2m fast with 0x178eb0, 0x179020, 0x175a80, 0x175270, 0x177040 and 0x176f60, all MATCH unless given;
// control 128). Ranking with both colours read into locals in the type-query block (184, listed): the type object 90
// takes ESI, the effect 52 EBP, the particle system's piece over that block 58 EDI, colour A 36 EBX, colour B 36 spills;
// retail's colour B holds EDI and the system is re-read from its argument slot for the colour call. A forced-ranking
// search there (colour B raised to the top, colour A or the type object lowered) stays at 170-185. Forms: the colour
// locals ahead of the spawn stores, A first or B first, 184/185; at the function's top, before the location test, 195;
// the spawn record's three stores after the colour call 188 (0x178eb0 32, 0x179020 28), with the colour locals 190.
// Body unchanged.
// The early color spill and system register still differ. With the two colors
// read on opposite sides of individual spawn stores, A first scores
// 184/184/184/184 and B first scores 185/185/185/185.
// NOT kept: every split grows the frame to 0x48 and the body to 572 bytes.
// M3 lane 2 r154 (fast with 0x178eb0/0x179020, matched unless given; control 128). Ranking on the early-colour form (colour
// locals read after set_location, ahead of the spawn stores): in the entry block (nine candidates) the type object scores 90,
// the system's piece 58 (three references: the location index, the 0x1751d0 call and the colour call) and each colour 36 (one
// read, one use), so the system piece keeps EDI and colour B cannot take it. On the lead (location_point_in_zone_inline for
// both zone tests, both queries as the colour call's arguments; re-measured 166 at 552 B): the spawn record as an aggregate
// initialiser declared after set_location 166 (same code); that with both colour locals read ahead of it 190 (568 B); the
// tint named with `multiplied()` as the argument 172 (556 B); `&effect->location` named for the test, set_location and both
// zone calls 166 (same code); that with the spawn stores as unknown, scale, location 164 (0x178eb0 4 and 0x179020 6, lost).
// Body unchanged.
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

// Updates an effect that follows an object: deletes it when the object is gone, otherwise copies the root
// object's velocity and location, refreshes the marker-driven scales and origin, and takes the root's colours.
// Returns true when the effect was deleted. The velocity copy goes through a pointer to the field: a store
// through a pointer local ends the compiler's cached read of the object index, which retail reads again for
// 0x1794a0 (root index first, as retail's EAX shows); the deletion path joins ahead of the second object test.
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
	if (effect && effect->object_index != NONE)
	{
		long root_index = function_baf80(effect->object_index);
		s_effect_object *root = OBJECT_GET(root_index);

		if (TEST_FIELD_BIT(root->flag4_8))
		{
			vector3f *velocity = &effect->velocity;

			*velocity = root->velocity;
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

			if (function_1794a0(root_index, effect->object_index))
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

					random_vector_in_cone(&g_4e7408->unknown0, &direction, &impulse, acceleration->inner_cone_angle, acceleration->outer_cone_angle);
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

// score 23 (2 held; was 23; 30 before): with the held change below only the decal call's two register arguments remain,
// BLOCKED-BY 0x17e670 (contributor): its
// body takes the source in ECX and the point in EDX in retail, ours the other way round. Round 80 (m3 lane 1): the
// round 117 lead applied, 0x189060 as (object, value, position, direction, scale, tag) at its definition, every
// declaration and all nine calls (each retail caller evaluates the scale before the position/direction register
// setup) 29 alone, with the detached record filled position, compression, velocity, location (every other
// 0x1895f0 caller's order) 2 (all 0x189060 callers unchanged). HELD by the coordinator 11-09, not on main: the
// reorder edits the call in 0xbf600, a contributor's finished function (src/unknown_0b9fc0.cpp), so it waits for
// that owner's OK (BLOCKED-BY 0xbf600 call). Both changes go in together or not at all.
// Earlier 23 (was 30): Frame 0x100, length and prologue agree; the decal call's register arguments (BLOCKED-BY
// 0x17e670) and the sound arms' scratch registers remain. Earlier 30: the attached light call's register arguments
// (computed before its pushes, retail after: ours forms forward with add ecx, retail with lea edx and keeps the
// location pointer live), fixed by 0xc0350's order in round 117. Round 85: `effect` declared before `up` gives retail's EAX up (38 -> 30, kept);
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
// Round 117: 0xc0350 as (tag, object, node, position, forward, up, scale) per the attached call's setup 23 (kept). Lead,
// not kept because 0xbf600 has a call to it: 0x189060 as (object, value, position, direction, scale, tag) with velocity
// before location in the detached arm 2 (scale evaluated before the address arguments, which also rotates the detached
// arm's scratch registers; only the decal call is left, and 0xbf600, 0xceee0, 0xf6670 stay put, its matched callers
// match); that order alone 29; (object, value, direction, scale, position, tag) 55, with velocity first 28. On the current
// order: velocity first 40; a compress local before the velocity copy 14, stored after location 21 (one-use local); velocity
// pointer 40; matrix pointer in the attached sound call 23, with velocity first 40; position and forward pointers with
// velocity first 40; node ternary in the argument list with velocity first 40; tag local with velocity first 40.
// This round: function-scope direction and velocity 23; that with detached velocity before location 40;
// compound velocity additions 23; the shared vector-add helper 23; a zero-initialized sound record 104
// (1363 bytes); a sound location snapshot 26; sound velocity and combined snapshots 23 each. Original
// body restored.
// Round 58 (m3 lane 11, fast): detached sound record with the compression after location and velocity 28;
// velocity copied first of all 28; attached sound test as `!detached && object != NONE` 28; 'snd!' case
// before 'ligh' 23; light test inverted with the detached call first 23. Every other 0x1895f0 caller fills the
// record as position, compressed forward, velocity, location (retail's order here too); that order needs
// 0x189060's register order (round 117 lead), BLOCKED-BY 0xbf600's call (contributor).
// Round 54 (m3 lane 15, fast): every order of the detached record's position, compression, location and velocity: 23 for the current
// one, then 26 (location, position, velocity, compression), 27 (three forms), 28 (nine), 29 (six), 40 (retail's order), 42, 43, 43.
// The attached call loads scale into EDI because position and direction already hold EAX and ECX in our evaluation order; that and
// the detached arm's rotation still need 0x189060's order. BLOCKED-BY 0xbf600, 0xceee0 (their calls) and 0x17e670.
// Round 101 (m3 lane 9, fast): retail's record order re-measured 40 (aligned 36; only scratch rotation differs: the
// point copy starts in EDX against retail's ECX); the sound case's object index as a case-scope local for the test
// and the call 23 (same code), with retail's order 40; a case-scope tag index for both arms 62, with retail's order
// 63; the sound record declared at function scope 23 (same code), with retail's order 40. The attached call's scale
// takes EDI because the position and direction temporaries are defined before it (arguments are evaluated last to
// first), so the cause stays 0x189060's order: BLOCKED-BY 0xbf600 (its call) and 0x17e670 (the decal call).
// M3 lane 15 r76: no trial. The first difference (+0x24f) is still the decal call's two register arguments into 0x17e670
// (contributor, unmatched at 429), and the sound arms still need the held 0x189060 order (aligned 25, register-masked 14).
// M3 lane 16 r112: no trial (first difference still 0x17e670's register arguments, contributor). The held 0x189060
// order's saved patch (lane 7 r117) no longer applies to current main and needs regenerating once its owner agrees.
// M3 lane 10 r100 (fast, with every 0x189060 caller): the held change re-measured on current main and its patch
// regenerated (build/r100/held-0x17a8a0.patch; still HELD, it edits the call line in 0xbf600): 0x189060 as (object, value,
// position, direction, scale, tag) with the detached record filled position, compression, velocity, location 2; that
// order alone 29; the record order alone 40. 0x189060, 0x107980, 0x166cd1, 0xf6670, 0x105dd0 and 0x1039a0 match in all
// three, 0xceee0 (97) and 0xbf600 (50) are unchanged. The first difference is still the decal call's two register
// arguments, BLOCKED-BY 0x17e670 (contributor).
// M3 lane 10 r104: no trial. The first difference (+0x24f) is still the decal call's two register arguments into
// 0x17e670 (contributor, unmatched), and the sound arms still need the held 0x189060 order (BLOCKED-BY 0xbf600's call).
// M3 lane 11 r106: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two
// register arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order.
// M3 lane 16 r126: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two
// register arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order.
// M3 lane 15 r94: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two register
// arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order (0xbf600's call line).
// M3 lane 13 r106: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two
// register arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order (0xbf600's call line).
// M3 lane 13 r108: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two
// register arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order (0xbf600's call line).
// M3 lane 13 r110: no trial. Baseline 23 at 1328 bytes; the first difference (+0x24f) is still the decal call's two
// register arguments into 0x17e670 (contributor), the rest the sound arms that need the held 0x189060 order (0xbf600's
// call line). Checked this round: retail never takes 0x17e670's address (no immediate or table entry holds it), and
// its two retail callers pass its first two parameters in ECX and EDX, so its convention comes from its own body.
// M3 lane 2 r102 (fast): baseline 23 at 1328 bytes, first difference +0x24f (0x17e670's EDX/ECX register arguments,
// BLOCKED-BY 0x17e670). The attached sound node index passed straight in the call 23 (same code); the detached record
// filled compression, position, velocity, location 43. Body unchanged.
// M3 lane 14 r116 (fast with 0x17a380, 0x189060, 0x1895f0, 0xc0350 and 0x177d10, all matched each time): baseline 23 at
// 1328 bytes, first difference +0x24f (0x17e670's register arguments, BLOCKED-BY 0x17e670, contributor; 0x189060's file was
// another lane's this round, so the held order could not be applied). Retail's record order (position, compression,
// velocity, location) re-measured 40 (the arm is a byte shorter, so ten branch targets move); the record renamed `sound` 23;
// the node index passed straight in the call 23 (r102's form again); the compressed field bound as a `dword &` with retail's
// order 40; the velocity field bound as a `vector3f &` with retail's order 40.
// M3 lane 16 r146 (fast with 0x189060, 0x1895f0, 0xc0350, 0x177d10, 0x17a380, all matched each time): baseline 23 at 1328
// bytes, first difference +0x24f (0x17e670's register arguments, contributor); 0x189060's file is on this round's avoid
// list, so the held order could not be applied. Detached record with a compress local: declared first, retail's record
// order 29; after the point copy, velocity, then the compressed store, location 14 (round 117's form: point copy and
// store order left; a one-use local, not kept without a match); after the point copy, location, compressed store,
// velocity 37; retail's order re-measured 40. Body unchanged.
// Coordinator 11-19: 0x17e670 is now declared (point, source, ...) for 0x23aad0's match; that gives 21 here (first
// difference at +0x472), so the call's register pair no longer blocks this function.
// Matched in a full build with stand-in SDK headers (was 23): function_189060 takes (object, value, position,
// direction, scale, tag), and the detached sound's velocity is stored before its location, as in the other
// callers of function_1895f0.
// @retail 0x17a8a0
void function_17a8a0(s_effect_location_datum *location, bool detached, s_effect_datum *effect, vector3f const *up, s_effect_part *part, point3f const *point, vector3f const *forward, real scale)
{
	switch (part->base_group_tag)
	{
	case 'char':
		if (part->tag_index != NONE && !(location->node_index != NONE && (location->node_index & 0x8000)))
		{
			long node_index = EFFECT_NODE_INDEX(location->node_index);

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
			function_17e670(point, g_510c78, part->tag_index, &velocity, function_259d0(&random->seed, __FILE__, __LINE__, part->radius_lower, part->radius_upper), 0, NONE, 0);
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
			long node_index = EFFECT_NODE_INDEX(location->node_index);

			function_c0350(part->tag_index, effect->object_index, node_index, &location->matrix.position, &location->matrix.forward, &location->matrix.up, scale);
		}
		else
		{
			function_c0350(part->tag_index, NONE, NONE, point, forward, up, scale);
		}
		break;
	case 'snd!':
		if (effect->object_index != NONE && !detached)
		{
			short node_index = EFFECT_NODE_INDEX(location->node_index);

			function_189060(effect->object_index, node_index, &location->matrix.position, &location->matrix.forward, scale, part->tag_index);
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

// MiniMax G (11-20): no new form, no lower score (93): the frame is still 0x4c against retail's 0x54, and the particle system is still in EDI.
// The definition is still unspilled and the location mode cached where retail spills the definition and reads the mode at each call.
// MiniMax A (11-20): no new form, no lower score 93: the same register preference as 0x178c80 - retail keeps the particle system in ESI and colour B in EDI; every form here swaps them.
// score 93 (was 93): The frame is 0x4c against 0x54, system/successor use exchanged registers and the definition is unspilled.
// MiniMax H (11-19, round 95): no new form, no lower score (49): the live mode read at each location call measures 103 and moves
// the effect index out of the stack into a register, which loses the matched caller 0x176f60, so the cached mode in the
// definition's slot stands.
// Earlier at 93 (was 93); m3 lane 8 r186 reached 53 (not kept at review, see its line at the end); on that form: frame 0x54, the system in ESI, the next index and then colour B in EDI, the type
// MiniMax A (11-18): no new form, no lower score (93): the live mode reads recover the 0x54 frame but move the effect index from the stack into a register, which loses the matched update caller 0x176f60, so the body stands.
// MiniMax I (11-15): no new form kept: the block-scope order (the particle cursor, the cached mode as a long, colour B, colour A) measures 91 (was 93), under the keep bar, and the mode read after the colour call measures 118, so the body stands.
// MiniMax G (11-18): no new form, no lower score (93): the frame is still 0x4c against retail's 0x54, the system and the
// next index still take EDI and ESI the other way round, and the definition still has no spill for the per-call mode reads.
// Not kept at review (11-20, batch 303): MiniMax H's 49 form below needs the 0x1751d0 result in a one-use pointer local
// (an element pointer used once), the long cached mode and the reordered locals, which are kept only with a match;
// without the named result it measures 120 and loses the matched caller 0x176f60 (lane 8 r186), so main's body stays at 93.
// MiniMax H form, 49 (was 93; not kept at review, see below): the frame is 0x54 with the system in ESI, the next index in EDI, the type object in EBX and colour A in a frame slot, as retail's; left: retail spills the definition at [esp+0x10] and reads the mode from it at each call, where this build caches the mode in that slot.
// MiniMax H (11-19): 49 (was 93): the particle-location cursor is the location index of a spawn record, the result of the second 0x1751d0 call is named and both type queries are the colour call's arguments; the caller 0x176f60 stays matched. The mode read at each call measures 103 and loses the caller, and the mode's width and the colour order each move the score by one to three points.
// Earlier score 93 (was 93): The frame is 0x4c against 0x54, system/successor use exchanged registers and the definition is unspilled.
// Earlier score 53 (was 93): m3 lane 8 r186. Frame 0x54, the system in ESI, the next index and then colour B in EDI, the type
// object in EBX, the tint in the effect index's home and colour A in a frame slot now agree with retail; left: the mode
// is cached in the slot where retail spills the definition and reads the mode at each call (that form makes LTCG pass
// both indices in registers and loses the matched caller 0x176f60). Before, 93: the frame was 0x4c versus retail 0x54,
// the system and successor registers were exchanged, and the definition had no spill for per-call mode reads.
// MiniMax A (11-18): no new form, no lower score (93): the live mode reads recover the 0x54 frame but move the effect index from the stack into a register, which loses the matched update caller 0x176f60, so the body stands.
// MiniMax I (11-15): no new form kept: the block-scope order (the particle cursor, the cached mode as a long, colour B, colour A) measures 91 (was 93), under the keep bar, and the mode read after the colour call measures 118, so the body stands.
// MiniMax G (11-18): no new form, no lower score (93): the frame is still 0x4c against retail's 0x54, the system and the
// next index still take EDI and ESI the other way round, and the definition still has no spill for the per-call mode reads.
// the system takes EDI and the next index ESI where retail has ESI and EDI (colour B and the type
// object rotate with them), the frame is 0x4c against 0x54 and the location mode is cached where retail spills the
// definition and reads the mode at each call.
// MiniMax E (11-14): no new form, no lower score (93, was 93): retail keeps the particle system in ESI and the next
// index in EDI, spills the definition pointer and reads the location mode at each call; this build gives the system
// EDI and the next index ESI and caches the mode, so the frame is 0x4c against retail's 0x54. The per-call mode form
// measures 108 here and drops the matched caller 0x176f60, which lives in another file.
// MiniMax H (11-14), NOT kept (coordinator: a 2-point gain on a function still above 30 is under the keep bar; code
// back to main, 93): the block-scope order (the particle cursor, the cached mode as a long, colour B, colour A)
// measures 91 at 337 bytes with the matched caller 0x176f60 still matching; the same order with the
// mode as a word or with the two colours exchanged gives 92, the mode as a short gives 92, the mode read again at
// each call gives 108 and drops that caller to 25, the effect-location cursor moved into the block gives 95 and 103
// (the latter also drops the caller), the particle cursor moved out of the block gives 103, the next index read at
// the end gives 107, the system declared before the effect gives 113 and the type query's locals lifted to the
// function scope gives 91.
// Earlier at 93 (was 93; 91 recorded before): our frame is 0x4c against retail's 0x54 and the particle system takes EDI where retail keeps
// Earlier at 91 (was 93; MiniMax H's block-scope form, not kept): our frame is 0x4c against retail's 0x54 and the particle system takes EDI where retail keeps
// it in ESI, so the homes of the two pointers, of the definition and of the cached mode sit at other offsets than
// retail's; retail spills the definition and reads the mode at each call, which needs the per-call form that drops
// the matched caller.
// system takes EDI where retail keeps it in ESI, so the homes of the two pointers and of the definition
// sit at other offsets than retail's.
// Earlier at 93 (was 93): frame 0x4c versus 0x54, the particle system in EDI (retail ESI) and the location mode cached
// where retail spills the definition and reads the mode at each call.
// MiniMax E (11-11): no new form, no lower score (93): the frame is still 0x4c against retail's 0x54,
// and the per-call mode and the particle system still take the opposite registers.
// MiniMax C (11-11): no new form, no lower score (93): retail keeps the particle system in ESI and
// colour B in EDI and reads the location mode at each call, but that per-call read flips the LTCG
// convention and breaks the matched caller 0x176f60, and 0x175a80's register contract (colour A in
// ECX, colour B in EDI, the tint in AL) is decided in src/unknown_173b90.cpp, another lane's file.
// MiniMax A (11-06): no new form, no lower score; the same cause as 0x178c80: retail reads both colours
// before the type query and keeps the particle system in ESI with colour B in EDI, ours keeps the system
// in EDI and the datum in ESI, and retail's frame is 0x54 against our 0x4c; the recorded best form (the
// spawn cursor with a long mode) reaches 89, a gain of four that the gate drops, and the per-call mode
// read reaches 108 while the matched caller 0x176f60 falls to 25.
// MiniMax A (11-06): no new form, no lower score; retail's frame is 0x54 with the definition in a frame home, the
// first colour spilled and the second in EDI, and the location mode read again for each call, while ours keeps
// the mode in a frame slot and the type object in ESI; the mode read for each call gives 108 and the matched
// caller 0x176f60 falls to 25, and the spawn-cursor form reaches 89/90, a gain under the gate.
// MiniMax E (11-06): the location mode read live from the definition reaches retail's 0x54 frame but
// scores 108, the same form with the particle system declared before the effect 119 and with color B
// read at the call 110; no new form, no lower score.
// MiniMax E (11-09): no new form, no lower score; the particle system and the next index trade ESI and EDI against retail, and the form that would free the definition's frame slot (the location mode read again for each call) is not usable because it drops the matched caller 0x176f60 to 25; the declaration-order and width variants score 93 or worse.
// Earlier summary (frame 0x4c versus 0x54; retail spills the definition and keeps two more
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
// MiniMax R (11-04): the location mode read late from the definition reaches retail's 0x54 frame but
// scores 108, the pool declaration swap 114 (345 bytes); no new form, no lower score.
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
// DH round 270: complete native transform result in the existing matrix, crossed with per-call mode, twice-
// used node word and traversal-scope matrix, 110/140/110/140/110/140/110/140. Cached-mode forms retain frame
// 0x4c and add copy work; per-call forms recover frame 0x54 but lose matched caller 0x176f60 (25). All
// exact-only forms, input contracts, pointer-result ownership and complete original body restored.
// Round 117 (fast with 0x176f60): spawn cursor re-measured 90 (aligned 35); colour A before the spawn local 90; colour B
// first 90 (aligned 34); long cached mode 89 at 337 bytes, with colour B first 89 (aligned 34); colour A read at the function top 102
// (326 bytes, 0x176f60 25). The system/next-index register swap and the mode cached where retail reads it per call remain.
// Round 312: negative tint/multiplication/both query outcomes 113/112/130;
// paired byte/signed-char/short/word/long storage 130/130/147/147/147.
// All inverse-outcome forms restored; caller convention, frame, system/successor and color homes remain.
// DH round 273: native pooled-location pointer frontier with live mode/native node word/inactive-
// return subsets: 170/181/170/181/161/177/161/177. All original handle cursors, mode observation, node
// tests, successor result and executable body restored; frame and system/color homes remain.
// Round 519: Actual successor sampled after color publication/at completion,
// cached/live mode and original/native node word: 98/108/112/153/98/108/112/153.
// All exact-only observation forms regress; complete original successor, mode,
// node observations and body restored. Frame and system/color homes remain.
// Round 288: Single successor-return exit with result-first, live mode and complete
// node-at-consumer subsets 0..7: 93/93/108/108/107/107/112/112. Live-mode and
// node-at-consumer forms lose the exact caller; original body and input contract restored.
// Round 476: condition-owned location pointer, bool/byte/short/long tint,
// cached/live mode: 93/98/99/99/108/108/111/109. Live-mode forms lose
// the matched update caller; original location scope, carriers and body restored.
// Round 478: single signed-node threshold in bool/byte/short/long storage,
// cached/live mode, 111/111/111/111/97/97/97/97. Live threshold preserves the
// matched caller but still regresses. Separate signed/bitwise tests on fields
// or one native node word, cached/live: 94/109/105/113/94/109/96/115.
// Original complete body restored; frame and system/color/definition homes remain.
// M328: Post-transform inverse node admission, complement/OR in
// bool/byte/short/long: 111/111/107/108/111/111/107/108. All regress; complete original
// body restored. Frame, system/successor/color homes and cached mode remain.
// AC498: Isolated system/effect/successor/first-color/second-color/type/mode/tint
// local names, original scopes and observations: 93 each. Original full body restored;
// frame 0x4c/0x54 and system/successor/color/definition homes remain.
// Round 58 (m3 lane 11, fast with 0x176f60): the type query fed from `definition` instead of a second
// function_1751d0() call 85 (331 bytes; not kept, retail makes that call); node predicate before the transform
// call 115 (0x176f60 25); colours declared before the particle-location cursor 93; location pointer at function
// scope 93; colours as long 93; location pointer declared in the while condition 93.
// Round 123 (m3 lane 7, fast with callers/callees): spawn cursor re-measured 89; with the effect location index
// declared inside the definition test 93; spawn + per-call mode 109 (0x176f60 25: the definition takes EBX, the
// effect is computed in EAX and its index becomes a register argument), with the location index inside the test
// 109, the node flag written in the call's argument 112 (327 bytes), the matrix declared beside the spawn 109.
// DH315: Live mode-field reference, readonly/mutable, original/outer cursor
// declaration and direct/named multiplication: 108/108/108/108/122/122/122/122.
// All lose matched caller 0x176f60 (25); original cached mode, cursor, query,
// successor, input contract and complete body restored.
// Round 81 (m3 lane 9, fast with 0x176f60): the live mode (definition->location_mode in the loop call) 108 with
// 0x176f60 25: both parameters then become register arguments (effect index in EAX, system in ECX) because the
// effect pointer is formed in EAX and copied to EBP, where retail and our cached form load the index straight into
// EBP; on the live form the system looked up before the effect 119 (348 bytes); colours read at the call 122 (320
// bytes); colours declared before the particle-location index 108; the location pointer declared first 108.
// AC506: Activity-admission bool/byte/short/long at the original cursor-read boundary
// 93/93/93/93; before cursor acquisition 112/112/112/113. Earlier reads change the
// argument convention and lose the matched caller. NOT kept: no gain or exact match;
// original activity field read, definition/cursor lifetimes and complete body restored.
// N354: Exhausted-particle continuation with original/for-owned effect traversal,
// cached/live mode and original/native node word: 93/93/108/108/93/93/108/108.
// NOT kept: ties/regressions; every live-mode form loses caller 0x176f60 (25).
// Original complete body restored; native frame and system/successor/color homes remain.
// AC528: Complete positive node classification, exhaustive then false-default in bool/byte/short/long
// 96/110/106/107/94/96/95/96. NOT kept: no exact match or eligible gain; original complete body restored.
// Native frame and system/successor/color homes remain. Several exhaustive non-Boolean forms also lose the exact caller; no qualifying gain or equality.
// DH372: Particle availability/exhaustion across the original transform call,
// bool/byte/short/long 119/119/126/132/119/119/126/132. NOT kept: regressions; original body
// restored, frame 0x4c/0x54 and system/successor/color homes remain.
// AC531: Complete immutable color pair, dword/long/unsigned-int/int, active/query-owned
// scopes: eight at 93/338 bytes. NOT kept: color residence canonicalizes; native
// frame, definition spill and system/successor roles remain. Original whole body restored.
// AC535: Positive tint result with false/true default publication, bool/byte/short/long:
// 113/112/117/116/113/112/117/116. NOT kept: all regress and lose matched caller
// 0x176f60 (25); original full body restored, native frame and system/successor homes remain.
// Y599: Incoming system handle advanced to saved successor, effect/system lookup order,
// cached/live mode and direct/native node word: 120/120/128/128/117/117/124/124.
// NOT kept: no exact residence/frame recovery; original full body restored.
// Frames 0x48/0x50 versus 0x54; every form loses the exact update caller.
// DH372: Particle availability/exhaustion across the original transform call,
// bool/byte/short/long 119/119/126/132/119/119/126/132. NOT kept: regressions; original body
// restored, frame 0x4c/0x54 and system/successor/color homes remain.
// Round 92 (m3 lane 16, fast with 0x176f60, which matches unless given): the cached mode as `long` on this body (no
// spawn cursor) 92 at retail's 337 bytes (gain 1, not kept); the cached word mode declared at function scope 93; the
// system pointer declared before the effect pointer 93 (same code); the definition declared first with the mode read
// per call 108 (0x176f60 25: effect index in EAX, system index in ECX, definition in EBX until the loop); `word const`
// mode 93. On the cached form the prologue is retail's except the system/next ESI/EDI swap.
// Round 155 (m3 lane 5, fast with 0x176f60, which matches unless given): mode read per call re-measured 108 (0x176f60 25);
// spawn cursor + long mode re-measured 89 (337 B, the loop then retail's); on it: system/next/definition locals const 89,
// colours read before the cursor 90, type object declared at the top of the block 89, mode as dword 89, the cursor copied
// to a named index 89, the matrix declared before the loop 89. The system EDI / next index ESI entry swap remains.
// Round 80 (m3 lane 1; a one-file LTCG harness with the link ballast reproduces 93 here, 128 for 0x178c80 and 17 for
// 0x175a80): all 119 other parameter orders of 0x175a80 (definition, declaration, three calls) 93-98, none lower; a
// random search of 136 combinations of lookup order, next-index read point, word/long/per-call mode, colour read
// point, multiplied argument/named, cursor local/spawn record, node flag point and block/early-return test: best 96
// (every per-call-mode form turns 0x176f60 to 25); the tint query in the colour call's argument list 120 (0x176f60
// 25), with the spawn cursor and a long mode 120; the location declared in the while condition 93 (same code).
// Diagnostic only: without the particle system argument of the loop's 0x248c60 call the system takes ESI and the
// next index EDI as in retail, so the system's loop use sets the swap.
// Round 160 (m3 lane 5, fast with 0x176f60/0x178bc0, which match unless given): mode per call re-measured 108 (0x176f60 25; frame 0x54,
// effect index EAX, system index ECX, definition EBX); with the effect index read once through its address 127 (352 bytes, 0x176f60 18:
// the effect index returns to the stack but the system index moves to ECX). A forceinline colour helper shared with 0x178c80 (colours,
// type query, tint, `multiplied()` as the argument) 93 (same code); in it colour B first 92 (0x178c80 178); both queries in the argument
// list 120 (325 bytes, 0x176f60 25); both queries named 105 (346 bytes).
// Round 97 (m3 lane 9, fast with 0x176f60/0x178c80): spawn record + mode read per call + colours early re-measured 109 (0x176f60 25;
// aligned 65: ours passes the effect index in a register, forms the effect pointer in EAX, keeps the definition in EBX and gives the
// tint and node flag frame slots of their own, where retail keeps them in the effect index's stack home). A begin-spawn inline
// helper shared with 0x178c80, taking the spawn record (cursor store, type queries, colour call), six forms (parameter orders
// system/effect/spawn, effect/system/spawn, spawn/system/effect; colours early or at the call; multiplied named or as the argument):
// 185 each at 258 bytes (the helper stays out of line here; 0x176f60 25). Body unchanged.
// MiniMax C (11-09): earlier at 93 (was 93): the system pointer and the next index swap ESI and EDI
// (retail keeps the system in ESI and the next index in EDI), retail's frame is 0x54 against our
// 0x4c and it spills the definition pointer early. Untouched this round.
// Round 161 (m3 lane 6, fast with 0x176f60/0x178bc0, both matched throughout), all on the spawn-record cursor with a long
// cached mode (frame 0x54, 337 bytes; masked aligned difference 18): the cursor stored before the colour reads 88; colour B
// read first 88; the mode cached before the cursor 89; the colour reads inside the type-query block 88; the type-query
// block's braces removed 88. Left on it: system EDI / next index ESI (retail ESI / EDI), colour A in EBX and the type object
// in ESI (retail colour A spilled, colour B in EDI, type object in EBX), and the cached mode where retail spills the
// definition and reads the mode per call. Not kept (the record only groups the cursor; gain 5). Four of these re-measure
// listed forms; new on the current body: the node flag as `node_index != NONE && node_index < 0` 94 (339 bytes); the
// particle system derived again from g_510c74 and its index for the 0x1751d0 type query 118, for the loop's 0x248c60
// call 112, for the particle-location cursor 119 (each turns the effect index into a register argument; 0x176f60 25).
// Round 147 (m3 lane 7, fast with 0x176f60, 0x178bc0, 0x178c80, 0x17ae50): mode read per call with colour B read
// first 108; per call with both colours read inside the type-query block (B first) 108; per call with the colours read
// before the particle-location cursor 108 (each: definition in EBX until the loop, effect index in EAX and system
// index in ECX, 0x176f60 25); cached mode with colour B first and the cursor after the colours 93; cached mode with
// the cursor read last 93 (same code). Retail's 0x175a80 call takes the system in ESI and colour B in EDI, which
// sets the system/next ESI/EDI swap; 0x175a80 is in src/unknown_173b90.cpp, an Avoid file this round.
// Round 180 (m3 lane 4, fast with 0x176f60, which matches unless given; aligned/masked = difflib counts with
// addresses / also stack offsets masked): permuter 30 minutes, best 81 on its own count only with a volatile tinted
// result, colour A narrowed to a word (a value change) and the loop rotated by hand: not kept. The tinted result
// declared at the block scope above the type query 93 (same code); on the spawn cursor with a long mode
// (re-measured 89, aligned 37, masked 33): that 89 (same code), colour B read first 89 (aligned 36); tinted at block
// scope with the mode read per call 108 (0x176f60 25). On the spawn form only the system/next ESI/EDI swap, the
// type object/colour A registers and the per-call mode read from the spilled definition remain. Also: the effect
// looked up after the definition call (declared first) 121 (346 bytes); the node flag's tests swapped, sign bit first,
// 92 (aligned 51, against 48 at 93).
// MiniMax R (11-10): no new form, no lower score (93): the system pointer and the next index still take the
// opposite registers from retail, and the frame is 0x4c against retail's 0x54.
// Round 93 (m3 lane 10): no new form. Every remaining lever in this note runs through 0x175a80's body (its file is in
// another lane's set this round) or changes the convention that matched caller 0x176f60 relies on (the mode read per call
// passes the effect index in a register). The /Gd first-pass harness loads the system index into EBP, where the full
// build passes it in EAX, so it cannot screen this convention either.
// Round 105 (m3 lane 16, fast with 0x176f60): the particle-location cursor `location_index` declared first
// (uninitialised, assigned where it was) 93 (same code); that with the mode read per call 108 (0x176f60 25, as the
// listed per-call form). A compile-only `/Gd` first pass loads the system index into EBP and the effect index into
// EDI, the opposite of both builds' conventions, so it cannot screen the per-call convention here. Levers left:
// 0x175a80's register contract (src/unknown_173b90.cpp, an Avoid file this round), the per-call mode's LTCG
// convention (breaks matched 0x176f60) and the system/next swap set by the matched 0x248c60 call's system argument.
// M3 lane 3 r236 (fast with 0x176f60/0x178c80/0x178eb0/0x179020/0x178bc0/0x1794e0/0x1794a0/0x1792b0/0x179850/0x17ae50/0x176e40,
// matched unless given): 0x175a80 is not the cause these notes name: its contract is the same in both builds (colour A ECX,
// colour B EDI, system ESI, tint AL, multiplied pushed) and it saves no callee-saved register in either, so its body cannot
// give the callers other registers. New: the system re-derived from g_510c74 for the colour call 117 (342 B, 0x176f60 25);
// diagnostic, per-call mode with the system looked up first and the effect index read once through its address 121 (348 B,
// both indices register arguments, 0x176f60 25). On the spawn cursor with a long cached mode: the cursor stored first, then
// colour B, colour A, the mode 88 (colour A then spills to retail's home); the cursor, the mode, colour B, colour A 87 (best,
// gain 6, not kept); that with the colours read inside the type-query block, const colours, const next index, the type
// block's braces removed or the node test with `!= 0` 87 each (same code as the 87 form); the mode as a word 88 (on the 88
// form 89); `multiplied` named 105 (346 B). Re-measured: spawn + per-call mode 109, spawn + long mode 89, both queries in the call's arguments 120 (325 B).
// Left on the 87 form: system/colour B/type object in EDI/EBX/ESI against retail's ESI/EDI/EBX from the entry on, and the
// mode cached where retail spills the definition (the per-call read makes both indices register arguments).
// Permuter on the 87 form (fast, anneal, 600 s, 68 tries; 87 in the full image too): best 86 only with volatile reads of
// the mode and colour A and the tint and colour B swapped in the colour call (wrong values); not used.
// M3 lane 3 r238 (fast with 0x176f60/0x178bc0/0x178c80/0x178eb0/0x179020): re-measured the mode read per call 108 (0x176f60
// 25: both indices register arguments, the system in EDI, the definition in EBX); no untried natural form found.
// M3 lane 11 r97 (fast with 0x176f60, matched unless given): re-measured the spawn-record cursor with a long cached mode
// (cursor, mode, colour B, colour A) 87 at 337 bytes; on it the loop step (particle-location lookup, 0x248c60, cursor
// advance) as a forceinline helper taking system, spawn record, matrix and node flag 87 (same code), with the node flag
// as its argument 87, with the mode read at each call 109 (338 bytes, 0x176f60 25); the whole `unknown0c` body as a
// forceinline helper taking effect, system, definition and the location cursor 92. Both swapped pairs follow from one
// choice: retail's system takes ESI at the entry (ESI and EDI are the 0x175a80 call's own registers for the system and
// colour B) where ours gives ESI to the type object first; a helper around the loop does not split the system's range.
// P479: Standard activity/cursor/node-sentinel/node-sign/location-presence predicates
// 132/124/123/129/162; complete node conjunction/complementary disjunction and negative
// query producers 113/138/112. NOT kept: all regress and lengthen; native frame,
// definition spill and system/successor/color homes remain. Original full body restored.
// Round 84 (m3 lane 2, fast with 0x176f60, matched unless given; this note's first line read 91, the baseline full check
// measures 93): effect and particle system as `&((T *)pool->data)[index & 0xffff]` 93, the definition pointer const 93,
// the location matrix as `transform4x3f const *` 93 (all same code); both colours read at the function top ahead of the
// definition test 108 (0x176f60 25); the const definition with the mode read per call 108 (0x176f60 25).
// M3 lane 5 r183 (fast with 0x176f60, matched unless given): mode read per call re-measured 108 (0x176f60 25); on the
// spawn-record cursor with a long cached mode (cursor, mode, colour B, colour A; re-measured 87, 337 B): dead-local phase
// scan n = 1..7 87 each (diagnostic, same code); the particle-location pointer declared at the loop body's top 87 (same
// code); colour A read at the colour call 128 (317 B, 0x176f60 25); the node flag computed inside the cursor test 110
// (325 B, 0x176f60 25); the matrix declared at the definition-test block's scope 87; the effect-location pointer
// declared above the spawn record 87.
// MiniMax G (11-14): no new form, no lower score (93): swapping the two DATUM statements gives 114 at 345 bytes; retail
// keeps the system in ESI and its successor in EDI (ours the other way round), which is what puts colour B in the frame
// slot and colour A in a register here while retail spills colour A to frame+0x0c and holds colour B in EDI, and
// retail's 0x54 frame comes from the definition pointer it spills at frame+0x00 and reads the mode from on every call
// of the location cursor, the per-call form in the lines above measuring 108; a 300 s permuter run (5 tries) stays at
// 92 differing instructions.
// M3 lane 15 r89 (fast with 0x176f60, matched throughout; control 93): the system's definition named and the type looked up
// from it (0x174a30's spelling) 93; the matrix declared at the definition-test block's scope 93; both colour locals `dword const`
// 93; the cached mode as a `long` 92 (337 B); that with the block's locals in the order colours, mode, particle cursor 92 (337 B).
// MiniMax S (11-15): no new form, no lower score (93): retail's frame is 0x54 against our 0x4c, it keeps the particle system in
// ESI and its successor in EDI where ours has them the other way round until the colour call, it spills colour A to a frame
// home of its own instead of re-reading the effect, and it reads the location mode from the definition again for each call;
// the form that reads the mode per call reaches retail's frame but flips the entry parameter placement.
// M3 lane 5 r191 (fast with 0x176f60/0x178c80/0x178bc0, matched unless given): the definition bound by a const
// reference to the 0x1751d0 result (`s_effect_particle_system_definition *const &definition = ...`; retail stores it
// right after the call and reloads it for each mode read) with the mode read per call 108 (0x176f60 25: the reference
// temporary is stored but the definition still takes EBX and the effect index becomes a register argument); the
// reference with the cached mode 93 (same code); that with colour B read first 93; the reference on the spawn-record
// cursor with the mode per call 109 (0x176f60 25); on the spawn-record cursor with a long cached mode and colour B
// first 87 at 337 bytes (the listed 87 form; the reference changes nothing).
// P589: Existing effect/system/successor/settings/matrix/query/tint/location bindings,
// independently fixed on the simple-cursor graph: 93/93/93/93/93/93/93/93. NOT kept:
// no native frame/residence recovery; original complete body, mode and callers retained.
// MiniMax D (11-17): no new form, no lower score (93): retail's frame is 0x54 against our 0x4c, it keeps the particle system in
// ESI and its successor in EDI where ours has them the other way round, it spills colour A to a frame home of its own with
// colour B in EDI, and it re-reads the location mode from the spilled definition on every cursor call; the mode-per-call form
// reaches retail's frame but flips the entry parameter placement (108), the two DATUM statements swapped give 114, and a 300 s
// permuter run stays at 92.
// Typed low-word array subscript at the effect pool 90.
// Typed low-word array subscript at the system pool 80.
// Typed low-word array subscript at the particle location pool 94.
// Typed low-word array subscript at the effect and system pool 93.
// Typed low-word array subscript at the effect and particle location pool 91.
// Typed low-word array subscript at the system and particle location pool 88.
// Typed low-word array subscript at the effect and system and particle location pool 100.
// Typed array subscripts fed by named low-word pool slots 100.
// NOT kept: the 80-point system-pool form changed the stack/register arguments and lost 0x176f60. Forms preserving the caller gained at most three and shortened the body further from retail.
// P594: Scalar memcpy at first/second/both colors, successor, effect cursor, particle
// cursor, mode and all original acquisition boundaries: 93/93/93/93/93/93/93/93. NOT kept: no exact
// native frame/system/successor/color recovery; original complete body restored.
// Round 563. The frame is 0x4c against 0x54 and the system/successor registers still trade places. word slot
// for the effect datum 90; word slot for the system datum 80; word slot for the particle-location datum 94;
// word slots for both entry datums 93; word slots for the effect and particle location 91; word slots for the
// system and particle location 88; word slots at all three datum lookups 100; both entry slots acquired before
// the pool lookups, plus the word location slot 100. NOT kept: the gains shorten the body past retail length.
// The system-slot forms also demote the matched update caller.
// M3 lane 16 r133 (fast with 0x176f60, matched unless given; control 93): the mode read per call re-measured 108
// (0x176f60 25: both indices then arrive in registers, effect EAX, system ECX, `ret`); that with the effect index read once
// through its own address 127 (352 B, effect index back on the stack but the system index in ECX, frame 0x50; 0x176f60
// 18); an early `return next_index;` for an empty definition 97 (334 B); the cached mode read first in the block 94; the
// effect-location pointer declared first 93 (same code); the cached mode as `short` 93 (same code).
// Explicit first and next queries in while/for traversal, with per-location/traversal-scope matrix:
// cached mode 93/93/93/93, mode read per query 108/108/108/108. NOT kept: the 0x54-frame forms
// move both inputs into registers and demote 0x176f60.
// M421: Bound mode field, cached for-step/while-body, live for-step/while-body, bound for-step/while-body,
// and live for-body 108/93/93/108/108/108/108/108. NOT kept: the live reads recover frame 0x54 but move the effect
// index from the stack into a register and lose the matched update caller.
// M3 lane 8 r186 (caller 0x176f60 in brackets, [0] = still matched; control 93 [0]). Best 53 [0] at 334 bytes (plain
// check): the type object looked up through a two-caller inline type accessor (its parameter
// is the definition 0x1751d0 returns; 0x178c80 uses it too and keeps its 128), both type queries in the colour call's
// arguments as the matched sibling 0x174a30 writes them, and the particle-location cursor as the location index of a
// spawn-record local (retail's 0x54 frame with the record's two unused dwords ahead of the cursor). Measured on the
// way (fast): the 0x1751d0 result named in a one-use local, `function_137bd0(system_definition->tag_index)`, with both
// queries as arguments 68 [0] on the old locals, with the spawn record 53 [0] (word mode) / 50 [0] (long mode), 49 [0]
// at 333 bytes with the order cursor, long mode, colour B, colour A, 52 [0] the same with a word mode (the one-use
// local, the long mode and the reorder are match-only forms, not kept); the helper taking the definition without the
// spawn record 68 [0]; a helper taking the system and returning `function_137bd0(particle_system->function_1751d0()
// ->tag_index)` 120 [25], that helper with the definition named inside it 54 [0] / 68 [0] without the record. Without
// any name for the 0x1751d0 result (the call written inside the 0x137bd0 call's argument) the same bodies give 120 [25]
// at 325 bytes: both indices then arrive in registers. On the named spawn-record form the mode read at each call
// (retail's shape) 103 [25] at 331 bytes: the body is then retail's (definition spilled at its definition, type object
// EBX, system ESI) but LTCG passes both indices in registers (effect EAX, system ECX), so EBX is saved late and the tint
// needs a frame byte (frame 0x58); that with colour B first 103 [25], the colours ahead of the cursor 103 [25], the
// type block's braces removed 103 [25], an early return for an empty definition 117 [25], the tint named 126 [0] at 352
// bytes (convention kept, but the definition takes EBX and the system EDI again), the tint named with the colours first
// 126 [0], the definition bound by a const reference 103 [25], the system looked up ahead of the effect 107 [25] at 337
// bytes, the next index read after the definition call 102 [25], the definition's location index named 103 [25].
// Re-measured: spawn record with the mode per call and no named definition 109 [25]; the per-call mode through a second
// `particle_system->function_1751d0()` in the loop condition 122 [19] at 358 bytes; spawn record, long cached mode,
// colour B then A and the definition's location index named 87 [0]. Next: a per-call form that keeps the effect index
// on the stack (retail forms the effect pointer in EBP straight from its stack home).
// Not kept at review: the inline type accessor only gives the 0x1751d0 result a name (a steering helper,
// kept only with a match); without it the spawn record and the in-argument queries give 120 [25] (fast), so main's body
// stays at 93. The 53 form is saved as build/r186/lead-0x177040-53-accessor.patch in lane 8's worktree.
// M3 lane 15 r105 (fast with 0x176f60, [0] = still matched; aligned = difflib count with addresses masked, 71 at 93): lane 8
// r186's form rebuilt with the named 0x1751d0 result instead of the accessor (spawn-record cursor, long cached mode, colour B
// then A, both queries in the colour call's arguments) 49 [0] at 333 B, aligned 3: the only difference left is retail's
// definition spill at [esp+0x10] with the mode read from it at each call (ours caches the mode in that slot). The mode per
// call on it 103 [25] (plain 103 [25]: both indices then arrive in registers, effect EAX, system ECX). On that per-call
// form: the two lookups swapped 107 [25]; an early return for an empty definition 117 [25]; `(long)` on the mode 103 [25];
// the declaration without __stdcall 103 [25] (same code); the node flag written into the call 112 [25]; a plain long
// cursor 103 [25]; colours A then B 103 [25]; the 0x1751d0 result unnamed 103 [25]; the next index read after the
// definition call 102 [25]; `if (definition->unknown0c)` 103 [25]; the effect looked up after the definition 127 [8]; the
// tint named 126 [0] (352 B, convention kept but the definition takes EBX and the system EDI), with multiplied named too
// 127 [0], with colours A then B 126 [0], with the 0x1751d0 result unnamed 109 [25], with a plain cursor 126 [0]. The
// parameters as (effect index, system index) at the definition, the declaration and the call: per-call 98 [8] (system
// index in EAX as retail, effect index in ECX), cached mode 49 [0] (same code). Diagnostics (not kept): per-call without the
// colour block still passes both in registers; per-call without the particle-location block keeps the effect index on the
// stack [0]; the cached-mode form with a constant tint and node flag (no byte spills) passes both in registers. So retail's
// stack effect index goes with the tint and node-flag bytes sharing its dead home. Next: a per-call form whose tint and
// node-flag bytes still take the effect index's home.
// System lookup at color 117, location update 112, both 127, both and type query 148, all descriptor
// and update uses 145; location update through the held pool 124, with live mode 135, with a word
// slot 95. The held-pool and live-mode forms recover 0x54. Every relookup changes the caller.
// M3 lane 3 r269 (fast with 0x176f60/0x178c80/0x175a80/0x174a30/0x178bc0; 0x176f60 in brackets, [0] = matched): retail's
// shape written plainly (spawn record whose location index is the particle-location cursor, colours read after it, both
// queries in the colour call, the mode read from the definition at each call) 103 [25] at 331 B; on it 1..7 unreferenced
// and 1..16 dead initialised locals 103 [25] each (no symbol-count effect); a named tint 109 [25]; the tint and the 0x1751d0
// result both named 126 [0] (352 B; convention kept, definition EBX, system EDI), with colour B read first 126 [0]; tint
// and multiplied named 127 [0] (result named) / 122 [25] (not named); multiplied alone named with the result named 110 [25].
// A compile-only (no /GL) listing shows the same first-pass frame for the [25] and [0] forms.
// M3 lane 5 r206 (fast with 0x176f60, 0x178c80 and the callees; caller in brackets, [0] = still matched; control 93 [0]):
// the mode read at each call re-measured 108 [25] (effect index EAX, system index ECX); with the effect index read once
// through its own address 127 [18] (effect on the stack, system ECX, frame 0x50, 352 B); with the system index read so 127
// [19] (system on the stack, effect EAX). The two parameters declared (effect, system) at the definition, the declaration
// and the 0x176f60 call: on the cached-mode body 93 [0] (same code); with the per-call mode 120 [8]; with the per-call mode
// and the effect index read through its address 128 [0] (retail's convention, system EAX and the effect on the stack, but
// the address-taken home no longer holds the tint byte: frame 0x50, 352 B, system EDI and the definition in EBX); with the
// system index read so 128 [19]. A compile-only first pass loads each index once in both mode forms, so it does not
// predict this convention (a one-object link with the colour and type callees' objects gives EAX/ECX for both forms).
// M3 lane 2 r113 (fast with 0x176f60/0x178c80/0x175a80/0x178bc0/0x248c60/0x17ae50; control 93): the mode read at each call
// re-measured 108 (0x176f60 25: effect index EAX, system index ECX); diagnostic dead-local phase scan 1-7 on that form 108
// each (the convention does not follow the symbol count); on it `tinted` declared at function scope 108, the node flag
// declared there 108, both 108, both in one declaration 108 (all the same code); both at function scope on the cached
// form 93 (same code). The byte locals' scope never moves their homes or the effect index's convention.
// Short node snapshot as a complete bit test, explicit sentinel arms, false-default nested
// tests, or signed range below NONE. Cached mode 93/96/93/111. Per-query mode 108/112/126/97.
// Per-query mode with a conditional signed result 127 and true-default rejection 108.
// NOT kept: the 0x54 forms change the matched update caller. The live forms that preserve
// it reserve 0x50 and raise the score.
// MiniMax B (11-19): no new form, no lower score (93): retail keeps the particle system in ESI and next_index in EDI and
// re-reads definition->location_mode from the spilled definition on each call, where this build keeps them in EDI/ESI and
// caches the mode in a word local; the live-mode form gives retail's frame 0x54 but measures 108, with the swapped
// declarations 119 and 114, and the two parameters declared at the definition and the 0x176f60 call measure 93 on the
// cached-mode body, 120 with the per-call mode; a permuter run (64 tries, 306 s) found nothing below the 92 differing
// instructions of the baseline.
// M3 lane 5 r215 (fast with 0x176f60, [0] = caller still matched; control 93 [0]): the definition and the effect-location
// index held in one two-member cursor aggregate whose index address goes to the next-location call (retail's frame has
// them side by side at +0x10 and +0x14): the definition is then stored once and read from its slot at each call as in
// retail, without taking EBX, and the type block is retail's (system ESI, colour B EDI, type object EBX), but both
// indices still arrive in registers and the tint takes a new frame byte: with the spawn-record cursor and both queries in
// the colour call 103 [25] (331 B); colours A then B 103 [25]; a plain particle cursor 103 [25]; the tint named 112 [25]
// (333 B); named with a plain cursor 112 [25]; named with colours A then B 112 [25]; the second 0x1751d0 result named 103
// [25]; that with the tint named 112 [25], and with colours A then B 112 [25]; the two lookups swapped 107 [25] (337 B);
// swapped with the tint named 109 [25]; the next index read after the cursor's two fields 102 [25]; after its definition
// field 102 [25]; the node flag written in the 0x248c60 call 110 [25] (323 B); that with the tint named 117 [25]; with
// the late next index 112 [25]; the cursor declared first 103 [25]. The plain per-call form re-measured 103 [25]. A
// compile-only first pass gives the same entry (system index into EBP, effect index into EDI) for the cached form and
// the cursor form, so it does not show what flips the convention.
// M3 lane 1 r138 (fast with 0x176f60 and 0x178c80; caller in brackets, [0] = still matched; control 93 [0]): lane 15 r105's
// lead (spawn-record cursor, long cached mode, colour B then A, named 0x1751d0 result, both queries in the colour call)
// re-measured 49 [0] at 333 B, with the mode read at each call 103 [25]. New on the per-call lead: the effect index read
// once through its own address 86 [18] at 337 B (effect index on the stack, but the system index in ECX); that with the two
// parameters as (effect index, system index) at the definition, the declaration and the call 90 [0] at 340 B (retail's
// convention: system index in EAX, effect index on the stack, tint and node flag in its home; left: the address read
// loads the effect index first into ECX and folds the pool base, so EBX is pushed late, and the per-call mode loads
// use `xor`/`mov cx` for retail's `movzx eax`); on that, the system looked up before the effect 116 [0] (342 B), a plain
// long cursor in place of the spawn record 96 [0], the 0x1751d0 result unnamed 90 [0] (same code). None kept: the address
// read, the record cursor and the colour order are match-only forms. All restored.
// M3 lane 3 r278 (fast with 0x176f60; 0x176f60 still matches unless given): the spawn-record cursor re-measured 89
// (frame 0x54 and every slot home as retail; left: system EDI / next index ESI against retail's ESI / EDI, the mode
// cached where retail keeps the definition and reads the mode at each call, colour A in EBX and colour B spilled where
// retail spills colour A and keeps colour B in EDI); that cursor with the record declared at function scope 89 (same
// code); with both queries as the colour call's arguments 120 (325 B, 0x176f60 25); that with the mode cached as a long
// 120 (same code); with the colours read at the call 134 (310 B, 0x176f60 25); that with both queries as arguments and the
// mode as a long 135 (309 B, 0x176f60 25).
// M3 lane 13 r130 (fast with 0x176f60, [0] = caller still matched; control 93 [0]): the mode per call re-measured 108
// [25]; the plain shape (spawn-record cursor, colours after it, both queries in the colour call, mode per call)
// re-measured 103 [25] at 331 B; on it the colours read ahead of the cursor store 103 [25], and the three loads and two
// stores ahead of the second 0x1751d0 call are then in retail's order (cursor into ECX, colour A into EAX, colour B
// into EDI; cursor stored, then colour A), so retail reads the colours before it sets the cursor; the next-location
// call through an inline taking the definition 103 [25] (same code), through an inline taking the mode as a short 113
// [25] (329 B), the mode cast to short at the call 103 [25] (same code); the particle-location step through an inline
// taking the spawn record 103 [25] (same code), with the tint named 109 [25] (338 B), with the matrix call as its
// argument 106 [25]; colours first with the tint named and no helper 109 [25]. Diagnostics, reverted: one more
// unconditional colour B use (the lever that gives 0x178c80 retail's entry) on the per-call body 123 [0], on the cached
// body 105 [0], with both queries in the call 107 [0]: system ESI and next index EDI as retail, but the definition then
// takes EBX and the type object EDI, so it is not the lever here. Left as before: the convention (retail: system index
// in EAX, effect index on the stack with the tint and node-flag bytes in its dead home, EBX saved at entry).
// First query reading the definition and continuation using the saved mode,
// for/while with direct or named multiplication 119/129 each. Reversing the
// mode observations scores 112/114 each. NOT kept: named multiplication
// recovers frame 0x54 and preserves the caller, but the body grows. The
// definition spill and system/successor registers still differ.
// M3 lane 13 r133 (h2m fast with 0x176f60, and with 0x178c80, 0x248c60, 0x175a80 and 0x1751d0 for the cross-file
// forms): `inline` on the definition of 0x178bc0, effect_location_next, 0x248c60, 0x175a80 or 0x1751d0 (each alone;
// its callers then keep no link-time summary of it) 93 each on this body and 108 each on the per-call mode form
// (0x176f60 25 there, 0x178c80 128 throughout: the same code as without); on the per-call form the tint and node-flag
// bytes take homes of their own (+0x13, +0x14) where retail puts both in the effect index's dead stack slot. Locals
// renamed (slot-name tie): the system pointer `system` 93, the next index `next_system_index` 93, the location cursor
// `location_cursor` 93, the definition `system_definition` 93 (same code each). All restored.
// M3 lane 11 r146 (h2m fast with 0x176f60, matched throughout; control 93): lane 1 r138's per-call form rebuilt 90 at 340 B
// (parameters as (effect index, system index) at the definition, declaration and call; the effect index read once through
// its own address; spawn-record cursor; colours B then A; named second 0x1751d0 result; both queries in the colour call;
// mode read from the definition at each call: retail's convention, system index EAX and the effect index on the stack).
// On it: the definition and location cursor grouped in one two-member local 92 (the mode still takes ECX after the reload
// into EAX, `xor`/`mov cx` where retail reuses EAX with `movzx`); the address read into a named index local after the
// system lookup 116 (342 B: EBX then pushed first as retail but given the system pool's data, where retail gives it the
// effect pool's); the node flag written into the 0x248c60 call 97 (341 B); the tint named ahead of the colour call 126
// (352 B); both colours read ahead of the cursor store 91.
// M3 lane 8 r215 (h2m fast with 0x176f60, still matched throughout; control 93): colour B declared ahead of colour A 93
// (same code); the cached mode as a long 92 (337 B, retail's length; one point, not kept), as a short 93; the long mode
// with colour B first 92, with the second 0x1751d0 result named 92. This build already has retail's convention here
// (system index in EAX, effect index from the stack); left: frame 0x4c against 0x54, system EDI / next index ESI, the
// cached mode, colour A in EBX with colour B spilled (retail spills colour A and keeps colour B in EDI).
// Named color lookup settings or tag with a common successor return or separate active-path return, all 93 with the
// cached mode or 126 with per-call reads. The per-call forms keep the caller exact and recover four of the eight
// missing frame bytes, at 352 bytes against retail 337.
// Effect lookup by low word, typed byte span, saved pool bases or active scope.
// Cached mode 90/93/93/116. Live mode 117/108/108/121.
// Live low-word lookups with named multiply result, system record first, mode-field
// address or scoped index 127/128/117/117. The low-word lookup can keep the
// caller with live mode. Naming the multiply result reaches frame 0x54 at 352 bytes.
// The cached low-word form gains three and shortens to 333 bytes, so it is restored.
// M3 lane 11 r150 (h2m fast with 0x176f60, matched in a control run, and 0x178c80 128): both type queries as the colour
// call's arguments (no tint local) 120 (325 B; 0x176f60 25); that with the colours read at the call 135 (310 B); with the
// cached mode as a long 120 (325 B); with the mode read from the definition at each call 103 (331 B); with colour B read
// ahead of colour A 120 (0x176f60 25 in all five: the shorter body changes the caller). All restored.
// The frame is 0x4c versus 0x54. Retail keeps the system in ESI and reloads the traversal mode from the spilled definition.
// Post-gate classification from a signed/unsigned node word 93/94; sentinel/high-bit split 93/102.
// The same four forms with per-call mode reads 108/109/108/116.
// NOT kept: The cached forms tie or regress. Per-call mode recovers the 0x54 frame and passes the effect index in a register, raising matched caller 0x176f60 to 25 in each preview.
// M3 lane 3 r306 (H2M_FAST with 0x176f60; control 93, caller matched): the trace ranks the type object 110 above the
// system 96 (it takes ESI, the system EDI); retail's system ESI, type object EBX, colour B EDI. The convention is the
// blocker for every per-call mode form: with the cached mode this function takes the particle system index in EAX and
// the effect index on the stack (retail's); with the mode read at each call both indices go in registers (effect index
// EAX, system index ECX), and with the effect index read once through its address it stays on the stack but the system
// index moves to ECX (the caller then sets ECX, 18). Measured: the mode at each call 108 (0x176f60 25); that with the
// effect index read through its address 127 (352 B; 0x176f60 18); the address read alone 93 (same code); both type
// queries as the colour call's arguments 120 (325 B; 0x176f60 25); queries as arguments, mode at each call and the
// address read 92 at 337 bytes (system ESI, next index EDI, type object EBX, colour B EDI, definition spilled and read
// for each call as retail; left the system index in ECX, the effect pool base not held in EBX, frame 0x4c; 0x176f60 18,
// so not kept); that with the type lookup's definition named 92 (same), without the address read 103 (331 B).
// Actual pooled effect, system and definition object references, each alone,
// each pair and all three 93 each. All three with the particle-location
// object reference 93. The system/successor registers and 0x4c frame persist.
// M3 lane 13 r156 (H2M_FAST with 0x176f60; control 93, caller matched). Allocator reading on main: the type object
// (priority 110) is coloured ahead of the system (96) and takes ESI. Convention reading, on lane 3 r306's per-call form
// (both type queries in the colour call, the mode read from the definition at each call, the effect index read once
// through its own address; re-measured 92 at 337 B, 0x176f60 18): the system index arrives in ECX whenever the effect
// index is first read ahead of the 0x1751d0 call, and in EAX (retail's) once that read comes after the call. Forms on
// that per-call body: the system lookup declared ahead of the effect lookup 129 (352 B; 0x176f60 18); the effect lookup
// after the next index 109 (342 B; 18); the effect lookup after the definition 130 (352 B; 0x176f60 MATCH, retail's
// convention, but the effect pool load then follows the call); that without the address read 127 (348 B; both indices
// in registers, 0x176f60 8); with the effect pool's base named at the top and indexed after the call (diagnostic) 124
// (352 B; 0x176f60 MATCH; the index arithmetic still follows the call, system EDI / next index EBX). Body unchanged.
// M3 lane 6 r244 (h2m fast, caller 0x176f60 in brackets): a frame trace shows this body gives the effect index a stack
// home that the tint and the node flag join, as retail does; with the mode read at each call LTCG passes the index in
// a register and that home is gone. Per-call mode re-measured 108 [25]; it with the loop as a `for` calling the
// location walker in its init and step 108 [25] (same code); this body so written 93 [0] (same code); the per-call form
// with a const definition pointer 108 [25] (same code), with the system re-derived from its index for the loop's call
// 135 [25] (358 B).
// The system still needs ESI and the definition needs a spill. Explicit initial and
// recurring advances, in a for increment or at the while tail, with cached word/short
// 93/93 and direct/reference mode reads 108/108 in each loop. NOT kept: the live reads
// recover the 0x54 frame but move the effect index off the stack and break the update caller.
// Lane AO r3 (fast with 0x176f60 and the six callees; control 93; "[lost]" = 0x176f60 no longer matches). On main's
// body, the particle-location cursor as a spawn record's location index 89 (338 B; a struct around one local, so kept
// only with a match). With the cursor in the record: the mode read at each call 109 [lost]; that with both type queries
// as the colour call's arguments 103 (331 B) [lost], plus the second 0x1751d0 result named 103 [lost]; the queries as
// arguments with the cached mode 120 (325 B) [lost], plus the named result 53 (334 B; MiniMax H's form re-built: only
// the cached word mode against retail's spilled definition is left). On the per-call/arguments/named form: the effect
// index read once through a pointer to it, effect looked up first 86 at 337 B (frame 0x54, effect index on the stack,
// but the system index then arrives in ECX and 0x176f60 is lost, 18), the effect looked up after the system 128 (352 B)
// [lost], after the next index 109 (342 B) [lost], after the definition 129 (352 B; caller matched); no address read and
// the effect after the definition 127 (348 B) [lost], after the system 107 [lost]. The per-call form with the tint in a
// local and the named result 126 (352 B, frame 0x58, caller matched, system in EDI), plus `multiplied` in a local 127
// (357 B), without the record 126. On the per-call/arguments/named form the particle-location pointer or the loop's
// matrix declared at the block's head 103 each [lost], without the record 103 [lost]. Reading: in every form here the
// convention follows the count of locals rather than
// the code: dropping the cached mode lets LTCG put the effect index in a register too. Body unchanged.
// Named cursor slot, color tag, and both 93 each. With direct per-call mode,
// named cursor slot 108, color tag 126, both 126. Mode-field pointer 108,
// with named color tag 126. The 108 forms lose the matched update caller;
// the 126 forms retain it with frame 0x50 and 352 bytes.
// M3 lane 11 r169 (h2m fast with 0x176f60, 0x175a80, 0x178c80, 0x17ae50, 0x178bc0 and 0x248c60, all MATCH unless given;
// control 93). Ranking on main: the type object 110 takes ESI before the particle system 96 (EDI), and the two colours
// tie at 44, colour A first (referenced first); retail gives the system ESI, colour B EDI and the type object EBX. The
// colours declared B first flip the tie (colour B then takes EBX) but leave 93 (same score); the cached mode declared
// first 93, both 92; both colour locals at the top of the guarded block, A or B first, 93/93. The per-call mode read
// re-measured 108 (0x176f60 25): its frame then has no argument home, so the tint and the flag that main keeps in the
// effect index's slot (as retail) go to frame cells, and the effect index arrives in EAX with the system index in ECX;
// with an unused initialised local at the top or in the block 108 each, with the colours B first 108. Body unchanged.
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
bool function_2dba0(long tag, bool alternate, long c, long d, long e, point3f const *position, vector3f const *direction, color3f const *color, real alpha, real amount, real scale);
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

// MiniMax R (11-04): the round 117 form again (default-first scale, `scale = scale_b * scale`, `(void)position.y;`
// before the transform) matches byte for byte; not kept: the dead read is a steering statement (coordinator).
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
// Round 457: Actual position/forward/up inputs through memmove in all seven subsets, then all
// three plus color; original observations, scale selection, transforms and render arguments
// retained. Scores 353/350/341/351/340/339/306/257. Original executable body restored.
// Round 247: Complete staged point rows in row/column schedules, then with staged normal rows and the
// default-scale prefix: 174/174/74/74/74/74/285/285; original body and helpers restored.
// Round 501: bounded iterator mode word/byte/signed-char/dword, with repeated/once-captured
// association index, 74/74/351/351/351/351/74/74. Original mode, observations and body
// restored; scale prefix and point Y-copy remain. Shared transforms stay unchanged.
// Round 117: `position.y` needs a by-name reference that emits no code. `(void)position.y;` before the transform call with
// the default-first scale (declared after the position copy, `scale = scale_b * scale`) MATCHES byte for byte; not kept, a
// redundant statement: what natural source names the field is still open. With the scale default after the colour copy 2.
// Unused read/unused pointer/self-compare give the same block in the compile-only screen; contrary to the round 105 note.
// Written-out transform: x/y by name and z through a pointer 353, all by name 354, y only 363 (frame 0x84: the helper's
// out pointers give retail's frame). Compile-only: field-by-field position copy names z too (z in the scale register); a
// y store after the copy leaves a real store; a C++ reference to position, a separate output and memcpy keep y in place.
// Round 312: plain output-row loops in point/direction/both 131/74/344; with
// default-first scale 270/270/297; point loop with complete named basis products
// 125, with default-first scale 270. The named-product forms also lose the matched
// shared user 0x17a380. All helper input captures, original row stores and body restored.
// Round 347: Complete point row publication xzy/yxz/yzx/zxy/zyx, then xzy/zxy/yxz with default-before-
// color scale: 303/102/131/303/127/262/264/270.
// Original complete body restored; scale prefix and point-Y preservation remain; both matched helper users regress under every order.
// Round 470: Complete eligibility/dispatch category predicates bool/long/short/byte/char
// 403/427/423/403/403; one dword/long group observation 380/380; Boolean categories
// with byte capped flare ordinal 422. Original group observations and body restored;
// scale prefix and preserved point-Y source remain.
// Fraction producer independent bounds/reversed bounds/early returns/successive selections
// 74/86/147/74; each with default-before-color scale 270/270/258/270.
// AC474: Paired scale-enable expression results in bool/byte/word/long, original/default-before-color
// scale production: 380/380/375/321/315/315/318/306. Narrow forms lose both update callers.
// Original scale, observations and full body restored; scale prefix and point-Y preservation remain.
// Round 476: existing shared native point return, pointer/reference matrix and point
// inputs, original/default-before-color scale: 340/340/340/340/277/277/277/277.
// Both other shared users regress; original output contract, helpers and body restored.
// Round 478: complete native coordinate snapshot with indexed conditional scaling,
// long/short/byte ordinals and repeated scale observation: 387/402/402/387;
// with default-before-directions scale 354/374/374/354. Both other shared point
// users regress in every form; original complete helper and body restored.
// Round 58 (m3 lane 11, fast with 0x17aec0, 0x179fb0, 0x17a380, 0x176bb0): the transform through the existing
// 0x1778d0 point helper 294 (1535 bytes); the house scale form re-measured 270; house scale with that helper
// 252 at retail's 1525 bytes (scale block retail's; still no y copy, and the helper's product order is not
// retail's); house scale with `scale * effect->scale_b` 270; node index named for the test and the call 74,
// on the house scale form 270.
// Round 41 (m3 lane 17, fast with 0x17aec0, 0x179fb0, 0x17a380, 0x176bb0; all four still match): on the house scale
// form (re-measured 270, 1519 bytes): the point transformed into a fresh point and copied back 324 (1558); the
// position copied after the scale block, just before the node test 305; the node matrix declared with the copies
// at the loop top 270 (same code); forward and up transformed before the point 312 (1500); forward and up copied
// before the position 282. The y copy (retail keeps raw y and scales into a fresh register) still appears only
// with a by-name reference to position.y.
// Round 111: direct copy construction with the scale after the position 312; a shared point pointer there 320;
// on the default-before-color scale, a shared point pointer 280, output-only 272, input-only 278, and the same
// pointer at both render calls 280; direct copy construction 272; an explicit temporary copy 272. None preserves
// position.y; both matched helper users and both callers stay matched. Opening body restored.
// Round 155 (m3 lane 5, fast with callers, callees, 0x179fb0 and 0x17a380): copies through a `transform4x3f const &` to
// the location matrix 74, with the house scale 270 (1519 B); position/forward/up declared apart and copied in position, up,
// forward order 74, with the house scale 278; the lens flare limit as `continue` 74. Compile-only: the reference, the
// separate copies and a cast copy of position keep y in place; only a by-name position.y (the listed read, or a
// self-assignment) gives retail's copy: retail keeps the loaded x and y and scales into fresh registers, z in place.
// Round 80 (m3 lane 1, fast with 0x179fb0, 0x17a380, 0x176bb0; aligned counts by difflib with addresses and frame
// offsets masked: 8 for this body, 4 for the house scale form, which is the y copy alone): on the house scale form,
// the mode as one ternary 270; the up copy after the colour and scale default 286; forward and up transformed from
// the location matrix with copies in an else arm (as 0x179fb0) 357; a NULL-initialised location cursor 270; the
// forward normal transformed before the point 298. Only 0x179880 transforms its point copy in place.
// MiniMax I (11-08): the scale default is declared before the colour copy, which puts the scale default store
// ahead of the colour-component store as retail does; with the scale block and the point transform's y copy
// already in place the function now matches byte for byte.
// score 0 (was 74): the scale block, the point transform's y copy and the colour-component store against the scale
// default store now all agree with retail (MiniMax I, 11-08).
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
				real scale = 1.0f;
				color3f color = *(color3f *)&g_4686cc->red;
				if (part->a_scales & 0x20)
					scale = effect->scale_a;
				if (part->b_scales & 0x20)
					scale = effect->scale_b * scale;
				if (location->node_index != NONE)
				{
					transform4x3f matrix;
					function_17aec0(location->node_index, effect, &matrix);
					(void)position.y;
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
						function_2dba0(part->tag_index, function_3e9c0(effect->object_index), 1, effect_index & 0xffff, flare_index, &position, &forward, (color3f const *)&effect->origin, 1.0f, scale, 1.0f);
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
