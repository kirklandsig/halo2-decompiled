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
long function_c0350(long tag_index, long object_index, long node_index, point3f const *position, vector3f const *forward, vector3f const *up, real scale);
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

// MiniMax H (11-14): no new form, no lower score (128): the frame is still 0x40 against retail's 0x44 and the
// spawn record still sits two frame slots lower than retail's, and the particle system still takes EDI where retail
// keeps it in ESI; the header's zone helper, whose zone block then matches retail's byte for byte, measures 165 on
// its own and 161-166 combined with the 0x44 frame, so the kept body stands.
// score 128 (was 128): Frame 0x40 versus 0x44, early color spill, cursor/query homes and zone-base scheduling remain.
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
// MiniMax C (11-09): score 128 (was 128): the early colour home is still missing and the frame is
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

// score 63 (was 63): unchanged this round: retail adds the record base into the scaled index before its
// first load and keeps the object's index in EAX across the validity call, where ours reads the field
// before the add and caches the index in ESI; the validity callee's body is in another file, so the
// reload cannot be produced from here.
// score 63 (was 63): this round found no gain - retail adds the record base into the scaled index before
// its first load and keeps the object index in eax across 0xbadc0, whose argument convention is set in
// src/unknown_0bad50.cpp.
// MiniMax H (11-11): no new form, no lower score (63); the only difference left is retail's reload of the object's index from the effect after the 0xbadc0 validity call, and 0xbadc0's body lives in another file, so the reload cannot be produced from here.
// MiniMax C (11-11): no new form, no lower score (63); the object index stays cached across 0xbadc0 where retail keeps NONE in ESI and reads the object index into EAX, and the head's load is folded into the datum address where retail forms the pointer; 0xbadc0 and 0xbaf80 live in another file.
// MiniMax E (11-11): no new form, no lower score (63); retail still reloads the object's index from the effect after
// the 0xbadc0 validity call, and that reload only appears when the compiler cannot prove that call read-only, so
// the difference cannot be produced from this side (BLOCKED-BY 0xbadc0).
// MiniMax A (11-06): no new form, no lower score; retail reloads the object's index from the effect after
// the 0xbadc0 validity call, and that reload appears only when the compiler cannot prove that call
// read-only, so the difference cannot be produced from this side: the body of 0xbadc0 is in another file
// this lane may not edit (BLOCKED-BY 0xbadc0).
// MiniMax A (11-06): no new form, no lower score; the only difference is the object's index, which retail reads
// again after the 0xbadc0 validity call while ours keeps it in a register across that call; the body of 0xbadc0 is
// in src/unknown_0bad50.cpp, which this lane may not edit, so the reload cannot be produced here
// (BLOCKED-BY 0xbadc0).
// MiniMax E (11-06): no new form, no lower score: the only difference left is the object's index,
// which retail reloads after the 0xbadc0 validity call; that call's body lives in
// src/unknown_0bad50.cpp, which this lane may not edit.
// MiniMax A (11-03): no new form, no lower score: retail still reloads the object's index after the
// 0xbadc0 validity call and keeps the definition pointer on the stack; calling the file-local copy
// object_get_175d70 instead of 0xbadc0 compiles to the same code (63).
// MiniMax R (11-01): no new form, no lower score: retail still reloads the object's index after
// the 0xbadc0 validity call, and 0xbadc0's body is in another file.
// MiniMax R (10-31): no new form, no lower score: the object index is the only difference and it is 0xbadc0's
// and 0xbaf80's bodies (src/unknown_0bad50.cpp) that decide it.
// MiniMax E (11-09): no new form, no lower score; the entry block still differs in where the record base is added into the scaled index and which register keeps the object index across 0xbadc0, and the permuter (29 tries in 311 s) plus the width and statement-order variants found nothing below 63.
// Earlier note: only the object index differs: ours keeps it in ESI from entry across 0xbadc0
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
// Round 90: split root declaration 63; root at function scope 63; definition declared first 63;
// both 63; named root at function scope 63; visible in the outer block 63, at function scope 63;
// tail object index at function scope 63. Original executable body restored.
// Round 58 (m3 lane 11, fast): validity test nested under `object != NONE` with `return true` on deletion 206
// (564 bytes), with the deletion in the if arm and the live work in its else 198 (571 bytes); definition
// lookup moved into the live arm 85; a `deleted` result flag in place of clearing `effect` 202 (589 bytes);
// early `return false` without an object, then the validity test 198. Still BLOCKED-BY 0xbadc0.
// Round 41 (m3 lane 17, fast): the validity result held as the object pointer it returns 202 (587 bytes); the
// definition fetched in the live arm 85; the root index (retail's EAX argument to 0x1794a0) named and passed 197
// (585); the object test split off as an empty first arm 198 (571); root index with the definition in the live arm
// 206 (584). Retail reloads the object index from the effect after 0xbadc0 and 0xbaf80 (both in
// src/unknown_0bad50.cpp, avoid-listed again): BLOCKED-BY 0xbadc0.
// Round 155 (m3 lane 5, fast): root index as 0x1794a0's first argument re-measured 197 (585 B); empty first arm 198
// (571 B); `!(index == NONE || valid)` 63; `NONE != effect->object_index` 63; deletion arm `return true;` with a plain
// second if 198 (572 B). Retail's reload at the join after 0xbadc0 still needs 0xbadc0 seen as writing memory.
// Round 80 (m3 lane 1, fast; aligned counts by difflib with addresses and frame offsets masked, 41 for the
// current body): retail also reloads the index after 0xbaf80 for 0x1794a0's second argument, so 0xbaf80 needs
// the same (BLOCKED-BY 0xbadc0, 0xbaf80). The root index as 0x1794a0's first argument (a value correction)
// 197, aligned 41 (585 B; only the cached index and the scratch rotation it causes differ); that with a plain
// `if (effect && ...)` second test 130, aligned 52 (the head reload then matches; the index is cached for
// 0xbaf80 instead); root index at function scope 197; tail reading the field 197; a named head index reused
// by the tail 197.
// Round 97 (m3 lane 9, fast with 0x1794a0/0x1792b0, both matched throughout; aligned counts in brackets, this body [72]): 0xbadc0
// and 0xbaf80 are matched and write nothing, so retail's reloads are not their summaries; retail also reads the tag index through
// the unadded lookup address and the object index through the formed pointer. Grid of the effect read through a fresh datum
// lookup in place at the else-if test (a), 0xbaf80's argument (b), 0x1794a0's pushed index (c), with or without the root index
// as 0x1794a0's first argument (d), abcd: 0001 197 [48]; 0010 219; 0011 210; 0100 72 [95]; 0101 206; 0110 269; 0111 216; 1000 87;
// 1001 202; 1010 265; 1011 212; 1100 87; 1101 202; 1110 264; 1111 190 (593 bytes) [33]. Second grid, the tag read through the
// lookup (t), the first test and 0xbadc0's argument through it (f), then a, b, d: t alone 84 [84]; t+d 205; t+b 181; t+b+d 170;
// t+a 177; t+a+d 166; t+a+b 177; t+a+b+d 166; f alone 203 (581 bytes); f+d 212; f+b 203; f+b+d 205; f+a 204; f+a+d 263; f+a+b 199;
// f+a+b+d 206; with t too the f rows repeat (203/212/203/205/204/263/199/206). None below 63; body unchanged.
// MiniMax H (11-13): no new form, no lower score (63): the object index is still cached across 0xbadc0 where retail
// reloads it from the effect, and the head's load is still folded into the datum address where retail forms the pointer
// first; both callees live in another file.
// MiniMax C (11-09): score 63 (was 63): the object index stays cached across 0xbadc0, where retail
// reloads it, and the head's load is folded into the datum address where retail forms the pointer
// first; 0xbadc0 and 0xbaf80 live in another file, so the reload cannot be forced here.
// MiniMax R (11-10): no new form, no lower score (63): the object index stays cached across the call where
// retail reloads it, and the callees in the other file decide that caching.
// Round 155 (m3 lane 8): first difference unchanged (+0x29: retail keeps NONE in ESI and reads the object index into EAX
// for 0xbadc0, then reads it again; ours keeps the index in ESI). Still BLOCKED-BY 0xbadc0 and 0xbaf80: their file was
// avoid-listed again, so no new form was tried.
// M3 lane 3 r236: no new form. Re-read against the listing: from the 0xbadc0 call on, every difference is the object
// index cached in ESI (retail reloads it from the effect after 0xbadc0 and 0xbaf80, with NONE in ESI); the grids above
// (fresh datum lookups at each use, root index, tests nested or split, definition placement, inline/noinline helpers,
// widths) cover the caller-side spellings. Retail's 0x1794a0 call takes 0xbaf80's root index in EAX (a value
// difference on this body, listed at 197 as a value correction). The lever is how LTCG summarises 0xbadc0/0xbaf80
// (both matched; src/unknown_0bad50.cpp is avoid-listed this round).
// MiniMax A (11-14): still no lower score (63): the object index named in a local of its own and used
// for both the test and the call compiles to exactly the bytes we already have, and reading the object
// index before the definition lookup scores 78; the listing shows why the register choice matters:
// retail loads the object index into EAX after the definition lookup and reloads it from the effect
// after 0xbadc0, because a value in EAX is clobbered by the call, while we keep it in ESI across the
// call (a callee-saved register), so the else path reuses it instead of reloading it. Nothing in the
// source can move a live value out of a callee-saved register, so the reload has to come from the
// else path reading the effect again: BLOCKED-BY 0xbadc0, 0xbaf80.
// MiniMax A (11-14): no new form, no lower score (63): the head's load still folds into the datum
// address where retail forms the pointer first, and the object index is still cached across
// 0xbadc0 where retail reloads it; both callees live in another file: BLOCKED-BY 0xbadc0, 0xbaf80.
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

// MiniMax H (11-14), NOT kept (coordinator: a 2-point gain on a function still above 30 is under the keep bar; code
// back to main, 93): the block-scope order (the particle cursor, the cached mode as a long, colour B, colour A)
// measures 91 at 337 bytes with the matched caller 0x176f60 still matching; the same order with the
// mode as a word or with the two colours exchanged gives 92, the mode as a short gives 92, the mode read again at
// each call gives 108 and drops that caller to 25, the effect-location cursor moved into the block gives 95 and 103
// (the latter also drops the caller), the particle cursor moved out of the block gives 103, the next index read at
// the end gives 107, the system declared before the effect gives 113 and the type query's locals lifted to the
// function scope gives 91.
// score 91 (was 93): our frame is 0x4c against retail's 0x54 and the particle system takes EDI where retail keeps
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
// MiniMax C (11-09): score 93 (was 93): the system pointer and the next index swap ESI and EDI
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
