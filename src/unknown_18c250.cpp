// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_18C250.CPP: the sound source callbacks built with /arch:SSE
   (the tables are in unknown_18c810.cpp) */

#include "unknown_11c920.h"
#include "globals.h"
#include "sound_sources.h"
#include "unknown_249e20.h"
#include "local_cameras.h"
#include "unknown_11a4d0.h"
#include "unknown_1428b0.h"
#include "sound_promotions.h"
#include <math.h>
#include <string.h>

/* an object, as the sound source code reads it */
struct s_sound_object_view
{
	byte unknown00[0x10a];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word unknown10a : 13;
	byte unknown10c[0x1da - 0x10c];
	short machine_index;
};

struct s_sound_object_header
{
	byte unknown00[3];
	byte type;
	byte unknown04[4];
	s_sound_object_view *object;
};

struct s_sound_class_flags
{
	byte unknown00[0xa];
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
	word unknown0a : 4;
};

struct s_sound_tag_class
{
	byte unknown00[2];
	char class_index;
};

struct s_unknown_5c;
s_object *function_badc0(long object_index, dword type_mask);
s_unknown_5c *function_221810(short index);
dword vector3d_compress(vector3f const *vector);
void function_11bed0(s_location *location, point3f const *point);
char function_18d4b0(long tag_index, char audible, long object_index, long *local_player_index);

static inline s_sound_object_header *sound_object_header(long object_index)
{
	return (s_sound_object_header *)g_4e0300->data + (object_index & 0xffff);
}

static inline vector3f *transform4x3f_apply_normal(transform4x3f const *matrix, vector3f const *vector, vector3f *out)
{
	out->i = matrix->up.i * vector->k + matrix->left.i * vector->j + matrix->forward.i * vector->i;
	out->j = matrix->up.j * vector->k + matrix->left.j * vector->j + matrix->forward.j * vector->i;
	out->k = matrix->up.k * vector->k + matrix->left.k * vector->j + matrix->forward.k * vector->i;
	return out;
}

static inline vector3f *cross3f(vector3f const *a, vector3f const *b, vector3f *result)
{
	result->i = b->k * a->j - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
	return result;
}

static inline vector3f *function_x84d9e8(vector3f *vector, vector3f const *axis, real sine, real cosine)
{
	vector3f cross;

	cross3f(axis, vector, &cross);
	vector->i = cosine * vector->i + sine * cross.i;
	vector->j = cosine * vector->j + sine * cross.j;
	vector->k = cosine * vector->k + sine * cross.k;
	return vector;
}

// @retail 0x18c250
bool __stdcall function_18c250(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	bool result = object_index == NONE || function_18c3b0(object_index, tag_index, marker, location);

	if (object_index == NONE || result)
	{
		s_sound_tag *sound = (s_sound_tag *)((s_tag_instance_view *)g_4e3b44)[tag_index & 0xffff].data;
		s_sound_class_view *sound_class = &((s_sound_globals_view *)g_51ebd4)->classes[sound->class_index];
		s_sound_class_spatialization *class_spatialization = sound_class_get_spatialization(sound_class);

		if (class_spatialization && (class_spatialization->flags & 1))
		{
			location->flag0 = true;
			location->spatial.velocity = *g_4687a4;
			location->spatial.position = *(point3f *)g_4687a8;
			function_x84d9e8((vector3f *)&location->spatial.position, g_4687b0, (real)sin(class_spatialization->angle), (real)cos(class_spatialization->angle));
		}
		result = true;
	}
	return result;
}


// @retail 0x18c3b0
bool __stdcall function_18c3b0(long object_index, long tag_index, s_sound_marker const *marker, s_type_99c531 *location)
{
	bool result = false;
	s_sound_object_view *object = (s_sound_object_view *)function_badc0(object_index, NONE);

	if (object && !object_or_parent_hidden(object_index))
	{
		s_sound_tag_class *sound = (s_sound_tag_class *)g_4e3b44[tag_index & 0xffff].bytes;
		if (!TEST_FIELD_BIT(object->flag2) || !TEST_FIELD_BIT(((s_sound_class_flags *)function_221810(sound->class_index))->flag11))
		{
			s_location object_location;
			object_get_root_location(object_index, &object_location);

			if (object_location.cluster_index != NONE)
			{
				long object_type = sound_object_header(object_index)->type;
				transform4x3f *matrix = function_b8bd0(object_index, marker->node_index < 0xff ? marker->node_index : 0);
				vector3f forward;

				transform4x3f_apply_point(matrix, &marker->position, &location->spatial.position);
				location->spatial.compressed_forward = vector3d_compress(transform4x3f_apply_normal(matrix, &marker->forward, &forward));
				function_ba1d0(object_index, &location->spatial.velocity, NULL);

				if ((1 << object_type) & 3)
				{
					if (function_11b930(object_index))
					{
						function_11bed0(&object_location, &location->spatial.position);
					}
				}
				else if (object_type == 7)
				{
					short machine_index = sound_object_header(object_index)->object->machine_index;
					if (machine_index != NONE)
					{
						s_local_camera *camera = function_125360();
						if (camera && camera->active)
						{
							long camera_index = camera->index;
							if (camera_index != NONE)
							{
								s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
								long audibility_index = function_249e20(bsp, machine_index);
								if (audibility_index != NONE)
								{
									long row = function_249e60(camera_index, bsp, audibility_index);
									if (row != NONE)
									{
										object_location.cluster_index = bsp->sound_clusters[row + 18 * bsp->cluster_map->indices[(word)machine_index]];
									}
								}
							}
						}
					}
				}

				location->spatial.location = object_location;
				location->audible = function_18d4b0(tag_index, location->requested_audible, object_index, NULL);
				result = true;
			}
		}
	}
	return result;
}

struct s_sound_unit_view
{
	byte unknown00[0x208];
	real sound_value;
};

// @retail 0x18c9b0
void function_18c9b0(long object_index, real target)
{
	if (g_4ed28c->valid && function_badc0(object_index, 3))
	{
		real *value = &((s_sound_unit_view *)sound_object_header(object_index)->object)->sound_value;
		real delta = target - *value;

		if (delta < -0.3f)
		{
			delta = -0.3f;
		}
		else if (delta > 0.3f)
		{
			delta = 0.3f;
		}
		*value += delta;
	}
}

struct s_sound_cluster_view
{
	byte unknown00[0x8c];
	long sound_count;
	short *sounds;
	byte unknown94[0xb0 - 0x94];
};

struct s_sound_environment_view
{
	byte unknown00[8];
	point3f position;
	byte unknown14[0x24 - 0x14];
};

struct s_sound_bsp_view
{
	byte unknown00[0x60];
	s_sound_environment_view *environments;
	byte unknown64[0xa0 - 0x64];
	s_sound_cluster_view *clusters;
};

// @retail 0x18cfd0
long function_18cfd0(long cluster_index, point3f const *point, real *distance)
{
	long result = NONE;
	*distance = 3.4028235e38f;

	s_sound_cluster_view *cluster = &((s_sound_bsp_view *)g_4e0348)->clusters[cluster_index];

	for (long i = 0; i < cluster->sound_count; i++)
	{
		short index = cluster->sounds[i];
		point3f *position = &((s_sound_bsp_view *)g_4e0348)->environments[index].position;
		vector3f vector;
		vector3d_from_points3d(position, point, &vector);
		real length = (real)sqrt(vector.j * vector.j + (vector.k * vector.k + vector.i * vector.i));
		if (*distance > length)
		{
			*distance = length;
			result = index;
		}
	}
	return result;
}

struct s_sound_class_play_bits
{
	byte unknown00[8];
	word flag0 : 1;
	word flag1 : 1;
	word unknown08 : 14;
};

struct s_vibration_curve_set;
void function_10e480(long object_index, long tag_index, s_vibration_curve_set *curves, real time);

static inline s_sound_promotion *sound_promotion_get(s_sound_promotion_tag const *sound)
{
	short promotion_index = sound->promotion_index;
	s_sound_promotion *result = NULL;

	if (promotion_index != NONE)
	{
		result = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[promotion_index];
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x18c630
void __stdcall function_18c630(long object_index, long tag_index, long a, long b)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;

	if (function_badc0(object_index, 3) && TEST_FIELD_BIT(((s_sound_class_play_bits *)function_221810(sound->class_index))->flag1))
	{
		short local_0 = sound->promotion_index;
		s_sound_promotion *promotion = NULL;
		if (local_0 != NONE)
			promotion = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[local_0];
		if (promotion)
		{
			function_12de70(&promotion->block, 2);
		}
	}
}
#pragma inline_depth(255)

// @retail 0x18ca20
void __stdcall function_18ca20(long object_index, long tag_index, s_sound_permutation const *permutation, real scale)
{
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;
	s_sound_promotion_data *data = ((s_sound_globals_promotion_view *)g_51ebd4)->promotions[sound->promotion_index].data;
	s_sound_promotion_entry *entry = &data->entries[permutation->entry_index];
	long count = entry->data_count;

	if (count > 0 && g_4ed28c->valid && function_badc0(object_index, 3))
	{
		s_sound_promotion_state curves;
		union { dword field_0; real field_4; } local_0;
		dword const volatile *local_1 = (dword const volatile *)&scale;
		local_0.field_0 = *local_1;
		curves.data = entry->data_offset + data->samples;
		curves.count = count;
		function_10e480(object_index, tag_index, (s_vibration_curve_set *)&curves, local_0.field_4);
	}
}

static __forceinline long float_to_int_nearest(real value)
{
	long result;

	__asm
	{
		fld value
		fistp result
	}
	return result;
}

#pragma inline_depth(0)
// @retail 0x18c720
void function_18c720(long tag_index, long object_index, long set_index, long permutation_index, real scale)
{
	long const *local_3 = &permutation_index;
	s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;

	if (!(sound->flags & 2) && TEST_FIELD_BIT(((s_sound_class_play_bits *)function_221810(sound->class_index))->flag1))
	{
		short local_0 = sound->promotion_index;
		s_sound_promotion *promotion = NULL;
		if (local_0 != NONE)
			promotion = &((s_sound_globals_promotion_view *)g_51ebd4)->promotions[local_0];
		if (promotion && (function_12de70(&promotion->block, 2) || promotion->count > 0))
		{
			s_sound_globals_promotion_view *globals = (s_sound_globals_promotion_view *)g_51ebd4;
			s_sound_permutation_set *set = &globals->sets[sound->permutation_base + set_index];

			real local_1 = scale * 30.0f;
			long local_2;
			__asm
			{
				fld local_1
				fistp local_2
			}
			function_18c9b0(object_index, function_218e50(tag_index, (short)set_index, (short)*local_3, (short)local_2));
			function_18ca20(object_index, tag_index, &globals->permutations[set->first_permutation + *local_3], scale);
		}
	}
}
#pragma inline_depth(255)

// @retail 0x18c6a0
void __stdcall function_18c6a0(long object_index, long unused, long tag_index, long set_index, long permutation, long scale)
{
	if (object_index != NONE && permutation && function_badc0(object_index, 3))
	{
		s_sound_promotion_tag *sound = (s_sound_promotion_tag *)g_4e3b44[tag_index & 0xffff].bytes;
		s_sound_globals_promotion_view *globals = (s_sound_globals_promotion_view *)g_51ebd4;
		s_sound_permutation *first = &globals->permutations[globals->sets[sound->permutation_base + set_index].first_permutation];

		function_18c720(tag_index, object_index, set_index, (s_sound_permutation *)permutation - first, *(real *)&scale);
	}
}

real sound_permutation_reference_duration(long definition_index, s_sound_permutation_reference const *reference); /* unknown_20b3c0.cpp */

/* replays the impulse sounds in the slots of g_4ed288 on their objects and
   frees the slots whose sounds are over or whose objects are gone */
#pragma inline_depth(0)
// @retail 0x18bf90
void function_18bf90(void)
{
	for (long i = 0; i < 16; i++)
	{
		s_looping_sound_slot *slot = &g_4ed288->slots[i];

		if (slot->active)
		{
			if (function_badc0(slot->source_index, 3))
			{
				real elapsed = (real)(g_510c54->game_time - slot->end_time) * g_510c54->rate;
				real duration = sound_permutation_reference_duration(slot->datum_index, &slot->permutation);

				function_18c720(slot->datum_index, slot->source_index, slot->permutation.pitch_range_index, slot->permutation.permutation_index, elapsed);
				if (!(elapsed > g_510c54->rate * 10.0f + duration))
				{
					continue;
				}
			}
			slot->datum_index = NONE;
			slot->active = false;
			slot->source_index = NONE;
		}
	}
}
#pragma inline_depth(255)
/* a listener of the sound system (0x48 bytes) */
struct s_sound_listener_view
{
	long leaf_index;
	short cluster_index;
	bool active;
	byte unknown07[0x48 - 7];
};

struct s_sound_system_listeners_view
{
	byte unknown00[0x88];
	s_sound_listener_view listeners[4];
};

struct s_4e6380;
extern s_4e6380 *g_4e6380;

/* the clusters within earshot of a listener */
dword g_54e8a0[16];

// @retail 0x18cb10
void sound_audible_clusters_update(void)
{
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	s_sound_listener_view *listener;
	long listener_index;

	memset(g_54e8a0, 0, ((bsp->cluster_count + 31) >> 5) * sizeof(dword));
	for (listener_index = 0, listener = ((s_sound_system_listeners_view *)g_4e6380)->listeners; listener_index < 4; listener_index++, listener++)
	{
		if (listener->active && listener->cluster_index != NONE)
		{
			for (long cluster_index = 0; cluster_index < bsp->cluster_count; cluster_index++)
			{
				if (function_249d60(bsp, cluster_index, listener->cluster_index) < 256.0f)
				{
					g_54e8a0[cluster_index >> 5] |= 1 << (cluster_index & 31);
				}
			}
		}
	}
}
#include "object_markers.h"
#include "object_queries.h"

extern long const g_444ae0;
extern s_record_pool *g_4ea93c;
extern s_record_pool *g_51ebfc;
struct s_impact;
struct s_impact_sound_location;
void impact_sound_location_get(s_impact const *impact, s_impact_sound_location *location, real *scale);
bool function_17b2d0(long effect_index, point3f *point, vector3f *forward, real *scale);
bool __stdcall function_bab40(long object_index, long name, real *value);
long function_b8c40(long object_index, short entry_index);
short function_188790(real angle);
void function_18d4f0(long object_index, char *audible, long *local_player_index);

struct s_loop_location_datum
{
	short salt;
	byte field_2;
	char type;
	union
	{
		word flags;
		struct { word flag0 : 1; word flag1 : 1; word field_4_2 : 3; word flag5 : 1; };
	};
	short marker;
	real scale;
	long tag;
	long owner;
	union { long name; struct { char source; char listener; short cluster; }; };
};

struct s_loop_location_object
{
	byte field_0[0x12c];
	byte field_12c;
	byte field_12d[3];
	long field_130;
	real field_134;
	real field_138;
	real field_13c;
	real field_140;
	real field_144;
};

PRIVATE inline long loop_location_object_index(s_loop_location_datum const *sound)
{
	return sound->type == 1 ? sound->owner : NONE;
}

PRIVATE __forceinline void loop_location_clear(s_type_99c531 *location)
{
	location->flags |= 2;
	location->scale = 0.0f;
	memset(&location->spatial, 0, sizeof(location->spatial));
	function_11bed0(&location->spatial.location, g_468788);
	location->spatial.compressed_forward = vector3d_compress(g_4687b0);
}

// @retail 0x18cbc0
void function_18cbc0(long looping_sound_index, s_type_99c531 *location)
{
	s_loop_location_datum *sound = &((s_loop_location_datum *)g_4ed28c->data)[looping_sound_index & 0xffff];
	byte *definition = g_4e3b44[sound->tag & 0xffff].bytes;
	location->flags = 0;
	location->unknown08 = sound->type == 0 ? sound->owner : g_444ae0;
	location->unknown02 = 0;
	switch (sound->type)
	{
	case 3:
		{
			location->audible = 1;
			vector3f forward;
			bool failed = true;
			if (!TEST_FIELD_BIT(sound->flag1))
				failed = !function_17b2d0(sound->owner, &location->spatial.position, &forward, &location->scale);
			if (!failed)
			{
				location->spatial.compressed_forward = vector3d_compress(&forward);
				location->spatial.velocity = *(vector3f *)((byte *)g_4ea93c->data + (sound->owner & 0xffff) * 0x190 + 0x30);
				function_11bed0(&location->spatial.location, &location->spatial.position);
			}
			else loop_location_clear(location);
			return;
		}
	case 2:
		location->audible = 1;
		if (!TEST_FIELD_BIT(sound->flag1))
			impact_sound_location_get((s_impact *)((byte *)g_51ebfc->data + (sound->owner & 0xffff) * 0xa0),
				(s_impact_sound_location *)&location->spatial, &location->scale);
		else loop_location_clear(location);
		return;
	case 0:
	{
		long cluster = sound->type == 0 ? sound->cluster : NONE;
		if (cluster != NONE)
		{
			*(short *)((byte *)&location->spatial.location + 4) = (short)cluster;
			*(long *)&location->spatial.location = NONE;
			*(short *)((byte *)&location->spatial.location + 6) = sound->type == 0 ? sound->listener : NONE;
			((byte *)&location->flags)[1] |= 0x10;
		}
		break;
	}
	}
	if (!(sound->flags & 1))
	{
		long object_index = loop_location_object_index(sound);
		if ((1 << sound_object_header(object_index)->type) & 0x400)
		{
			s_loop_location_object *object = (s_loop_location_object *)sound_object_header(sound->owner)->object;
			location->unknown02 = object->field_12c;
			*(long *)(location->unknown30) = object->field_130;
			if (object->field_134 > 0.0f) { location->flags |= 4; *(real *)(location->unknown30 + 4) = object->field_134; }
			if (object->field_138 > 0.0f) { location->flags |= 8; *(real *)(location->unknown30 + 8) = object->field_138; }
			if (object->field_13c > 0.0f) { location->flags |= 0x10; *(short *)(location->unknown30 + 0xc) = function_188790(object->field_13c); }
			if (object->field_140 > 0.0f) { location->flags |= 0x20; *(short *)(location->unknown30 + 0xe) = function_188790(object->field_140); }
			if (object->field_144 < 0.0f) { location->flags |= 0x40; *(real *)(location->unknown30 + 0x10) = object->field_144; }
		}
		function_bab40(loop_location_object_index(sound), sound->name, &location->scale);
	}
	else location->scale = sound->scale;
	if (sound->type == 1)
	{
		long marker_name = 0;
		if (sound->marker != NONE) marker_name = function_b8c40(sound->owner, sound->marker);
		s_object_marker marker;
		function_b8d30(sound->owner, marker_name, &marker, 1, false);
		location->spatial.position = marker.matrix.position;
		location->spatial.compressed_forward = vector3d_compress(&marker.matrix.forward);
		function_ba1d0(loop_location_object_index(sound), &location->spatial.velocity, NULL);
		object_get_root_location(loop_location_object_index(sound), &location->spatial.location);
		char audible = 1;
		function_18d4f0(loop_location_object_index(sound), &audible, NULL);
		location->audible = audible;
		if ((*definition & 8) && !location->audible)
		{
			location->audible = 1;
			location->flags |= 1;
		}
	}
	else location->audible = 0;
	if (TEST_FIELD_BIT(sound->flag5)) location->flags |= 0x400;
}
